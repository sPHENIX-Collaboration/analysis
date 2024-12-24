#!/bin/bash

################################################################################
# Golden Run List Generation Script
#
# This script:
#   1) Extracts runs from FileCatalog with ≥1M events (and optionally ≥47289 run#).
#   2) Applies Calo QA (golden EMCal/HCal).
#   3) Applies runtime (>5min) and livetime (>80%) cuts.
#   4) Optionally removes runs missing bad tower maps if 'removeRunsWithMissingMaps' is given.
#   5) Produces final run lists and optionally DST file lists.
#
# Arguments:
#   removeRunsWithMissingMaps : Remove runs missing bad tower maps from the final list.
#   dontGenerateFileLists     : Skip generating DST lists at the end.
#   noRunNumberLimit          : Do not apply the ≥47289 run number lower limit.
#
# If noRunNumberLimit is given:
#   - We process runs without the limit.
#   - After all cuts, produce a comparison with the scenario as if the limit were applied.
#   - Show differences in runs/events, and if removing missing maps, also show differences in missing-map runs.
#
# If dontGenerateFileLists is given:
#   - DST list generation is skipped, but comparisons are still shown.
#
# Missing bad tower maps:
# - Always reported in the final summary.
# - If removeRunsWithMissingMaps is not given, these runs are kept and DST lists include them.
# - If removeRunsWithMissingMaps is given, these runs are removed before final DST creation, and reported in the summary.
################################################################################

########################################
# GLOBAL STYLES FOR OUTPUT
########################################
BOLD="\e[1m"
RESET="\e[0m"
GREEN="\e[32m"
CYAN="\e[36m"
MAGENTA="\e[35m"
YELLOW="\e[33m"

########################################
# FUNCTIONS
########################################

error_exit() {
    echo -e "${BOLD}${YELLOW}[ERROR]:${RESET} $1"
    if $DONT_GENERATE_FILELISTS; then
        echo "Note: 'dontGenerateFileLists' was provided, so no DST lists would have been generated."
    fi
    exit 1
}

parse_arguments() {
    REMOVE_MISSING_MAPS=false
    DONT_GENERATE_FILELISTS=false
    NO_RUNNUMBER_LIMIT=false

    for arg in "$@"; do
        if [[ "$arg" == "removeRunsWithMissingMaps" ]]; then
            REMOVE_MISSING_MAPS=true
            echo -e "${BOLD}${CYAN}Argument detected:${RESET} Removing runs missing bad tower maps."
        fi
        if [[ "$arg" == "dontGenerateFileLists" ]]; then
            DONT_GENERATE_FILELISTS=true
            echo -e "${BOLD}${CYAN}Argument detected:${RESET} Will not generate DST lists."
        fi
        if [[ "$arg" == "noRunNumberLimit" ]]; then
            NO_RUNNUMBER_LIMIT=true
            echo -e "${BOLD}${CYAN}Argument detected:${RESET} No run number lower limit will be applied."
        fi
    done

    if ! $REMOVE_MISSING_MAPS; then
        echo "No removal option detected: Missing-map runs will be kept."
    fi
    echo "----------------------------------------"
}

setup_directories() {
    echo -e "${BOLD}${GREEN}Setting up directories...${RESET}"
    mkdir -p FileLists/ dst_list/ list/
    echo "Directories ready."
    echo "----------------------------------------"
}

set_workplace() {
    workplace=$(pwd)
    echo -e "${BOLD}Working directory:${RESET} $workplace"
    echo "----------------------------------------"
}

extract_initial_runs() {
    echo -e "${BOLD}${MAGENTA}Step 1:${RESET} Extracting initial runs from databases..."
    python_script=$(cat <<EOF
import pyodbc
import sys

no_limit = "NO_LIMIT" in sys.argv

try:
    fc_conn = pyodbc.connect("DSN=FileCatalog;UID=phnxrc;READONLY=True")
except:
    print("TOTAL_RUNS:0")
    sys.exit(1)

fc_cursor = fc_conn.cursor()

if no_limit:
    query = '''
    SELECT runnumber
    FROM datasets
    WHERE dsttype='DST_CALO_run2pp' AND dataset='ana450_2024p009'
    GROUP BY runnumber
    HAVING SUM(events) >= 1000000;
    '''
else:
    query = '''
    SELECT runnumber
    FROM datasets
    WHERE dsttype='DST_CALO_run2pp' AND dataset='ana450_2024p009'
    GROUP BY runnumber
    HAVING SUM(events) >= 1000000 AND runnumber >= 47289;
    '''

fc_cursor.execute(query)
all_runs = [row.runnumber for row in fc_cursor.fetchall()]
all_runs.sort()
with open('list/list_runnumber_all.txt', 'w') as f:
    for r in all_runs:
        f.write(f"{r}\n")
print(f"TOTAL_RUNS:{len(all_runs)}")
fc_conn.close()

try:
    prod_conn = pyodbc.connect("DSN=Production_write")
except:
    print("COMBINED_GOLDEN_RUNS:0")
    sys.exit(1)

prod_cursor = prod_conn.cursor()
detectors = ['emcal', 'ihcal', 'ohcal']
golden_runs = set(all_runs)

for d in detectors:
    prod_cursor.execute(f"SELECT runnumber FROM goodruns WHERE ({d}_auto).runclass='GOLDEN'")
    detector_golden = {row.runnumber for row in prod_cursor.fetchall()}
    golden_runs = golden_runs.intersection(detector_golden)

golden_runs = sorted(golden_runs)
with open('list/Full_ppGoldenRunList.txt', 'w') as f:
    for r in golden_runs:
        f.write(f"{r}\n")
print(f"COMBINED_GOLDEN_RUNS:{len(golden_runs)}")
prod_conn.close()
EOF
    )

    if $NO_RUNNUMBER_LIMIT; then
        python_output=$(python3 <(echo "$python_script") NO_LIMIT)
    else
        python_output=$(python3 <(echo "$python_script"))
    fi

    total_runs=$(echo "$python_output" | grep 'TOTAL_RUNS' | cut -d':' -f2)
    combined_golden_runs=$(echo "$python_output" | grep 'COMBINED_GOLDEN_RUNS' | cut -d':' -f2)

    echo "Summary after initial extraction:"
    echo "Total initial runs: ${total_runs:-0}"
    echo "Runs after Calo QA: ${combined_golden_runs:-0}"
    echo "----------------------------------------"

    if [[ "${total_runs:-0}" -eq 0 ]]; then
        error_exit "No runs found after initial extraction. No data matches your criteria."
    fi
}

validate_golden_list() {
    echo -e "${BOLD}${MAGENTA}Step 2:${RESET} Validating golden run list..."
    golden_run_list="list/Full_ppGoldenRunList.txt"
    if [[ ! -f "$golden_run_list" ]]; then
        error_exit "$golden_run_list not found. Possibly no runs qualify."
    fi
    echo "Golden run list found."
    echo "----------------------------------------"
}

get_actual_events_from_evt() {
    input_file=$1
    total_events=0
    batch_size=100
    run_numbers=()

    while IFS= read -r runnumber; do
        [[ -z "$runnumber" ]] && continue
        run_numbers+=("$runnumber")
        if [[ ${#run_numbers[@]} -ge $batch_size ]]; then
            run_list=$(IFS=,; echo "${run_numbers[*]}")
            run_numbers=()
            query="SELECT SUM(lastevent - firstevent + 1) FROM filelist WHERE runnumber IN ($run_list) AND filename LIKE '%GL1_physics_gl1daq%.evt';"
            result=$(psql -h sphnxdaqdbreplica -d daq -t -c "$query")
            events=$(echo "$result" | xargs)
            [[ "$events" =~ ^[0-9]+$ ]] && total_events=$(echo "$total_events + $events" | bc)
        fi
    done < "$input_file"

    if [[ ${#run_numbers[@]} -gt 0 ]]; then
        run_list=$(IFS=,; echo "${run_numbers[*]}")
        query="SELECT SUM(lastevent - firstevent + 1) FROM filelist WHERE runnumber IN ($run_list) AND filename LIKE '%GL1_physics_gl1daq%.evt';"
        result=$(psql -h sphnxdaqdbreplica -d daq -t -c "$query")
        events=$(echo "$result" | xargs)
        [[ "$events" =~ ^[0-9]+$ ]] && total_events=$(echo "$total_events + $events" | bc)
    fi

    echo "$total_events"
}

apply_incremental_cuts_header() {
    echo "----------------------------------------"
    echo -e "${BOLD}${MAGENTA}Applying incremental cuts:${RESET} runtime, livetime, and missing bad tower maps"
    echo "----------------------------------------"
}

runtime_cut() {
    input_file="list/Full_ppGoldenRunList.txt"
    output_file_duration_v1="list/list_runnumber_runtime_v1.txt"
    > "$output_file_duration_v1"
    total_runs_duration_v1=0
    runs_dropped_runtime_v1=0

    while IFS= read -r runnumber; do
        [[ -z "$runnumber" ]] && continue
        query="SELECT EXTRACT(EPOCH FROM (ertimestamp - brtimestamp)) FROM run WHERE runnumber = ${runnumber};"
        result=$(psql -h sphnxdaqdbreplica -d daq -t -c "$query" | tr -d '[:space:]')
        duration="$result"

        if [[ $duration =~ ^[0-9]+(\.[0-9]+)?$ ]] && (( $(echo "$duration > 300" | bc -l) )); then
            echo "$runnumber" >> "$output_file_duration_v1"
            ((total_runs_duration_v1++))
        else
            ((runs_dropped_runtime_v1++))
        fi
    done < "$input_file"

    echo "After runtime cut (>5 mins): $total_runs_duration_v1 runs remain."
    echo "Dropped due to runtime: $runs_dropped_runtime_v1"
    echo "----------------------------------------"
}

livetime_cut() {
    input_file="list/list_runnumber_runtime_v1.txt"
    output_file_livetime_v1="list/list_runnumber_livetime_v1.txt"
    bad_file_livetime_v1="list/list_runnumber_bad_livetime_v1.txt"
    > "$output_file_livetime_v1"
    > "$bad_file_livetime_v1"
    total_runs_livetime_v1=0
    runs_dropped_livetime_v1=0

    while IFS= read -r runnumber; do
        [[ -z "$runnumber" ]] && continue
        index_to_check=10
        query="SELECT raw, live FROM gl1_scalers WHERE runnumber = ${runnumber} AND index = ${index_to_check};"
        result=$(psql -h sphnxdaqdbreplica -d daq -t -c "$query")

        index_pass=false
        while IFS='|' read -r raw live; do
            raw=$(echo "$raw" | xargs)
            live=$(echo "$live" | xargs)
            if [[ "$raw" =~ ^[0-9]+$ && "$live" =~ ^[0-9]+$ && "$raw" -ne 0 ]]; then
                ratio=$(echo "scale=2; $live / $raw * 100" | bc -l)
                if (( $(echo "$ratio >= 80" | bc -l) )); then
                    index_pass=true
                fi
            fi
        done <<< "$result"

        if $index_pass; then
            echo "$runnumber" >> "$output_file_livetime_v1"
            ((total_runs_livetime_v1++))
        else
            echo "$runnumber" >> "$bad_file_livetime_v1"
            ((runs_dropped_livetime_v1++))
        fi
    done < "$input_file"

    echo "After livetime cut (>80%): $total_runs_livetime_v1 runs remain."
    echo "Dropped due to livetime: $runs_dropped_livetime_v1"
    echo "----------------------------------------"
}

missing_bad_tower_maps_step() {
    input_file="list/list_runnumber_livetime_v1.txt"
    output_file_final_v1="FileLists/Full_ppGoldenRunList_Version1.txt"
    bad_tower_runs_file="list/list_runs_missing_bad_tower_maps.txt"
    cp "$input_file" "$output_file_final_v1"

    available_bad_tower_runs=$(find /cvmfs/sphenix.sdcc.bnl.gov/calibrations/sphnxpro/cdb/CEMC_BadTowerMap -name "*p0*" | cut -d '-' -f2 | cut -d c -f1 | sort | uniq)
    echo "$available_bad_tower_runs" > list/available_bad_tower_runs.txt
    grep -Fxvf list/available_bad_tower_runs.txt "$input_file" > "$bad_tower_runs_file"

    total_runs_with_bad_tower=$(grep -Fxf list/available_bad_tower_runs.txt "$input_file" | wc -l)
    total_runs_missing_bad_tower=$(wc -l < "$bad_tower_runs_file")

    echo "Runs with bad tower maps: $total_runs_with_bad_tower"
    echo "Runs missing bad tower maps: $total_runs_missing_bad_tower"
    echo "List of missing map runs: $bad_tower_runs_file"
    echo "----------------------------------------"

    # Store these globally for final summary:
    export total_runs_missing_bad_tower

    rm list/available_bad_tower_runs.txt

    if $REMOVE_MISSING_MAPS; then
        echo "Removing runs missing bad tower maps..."
        grep -Fxf "$bad_tower_runs_file" -v "$output_file_final_v1" > tmp && mv tmp "$output_file_final_v1"
        echo "Removal complete."
        echo "----------------------------------------"
    fi

    cp "$output_file_final_v1" dst_list/Final_RunNumbers_After_All_Cuts.txt
    echo "Final run list stored in dst_list/Final_RunNumbers_After_All_Cuts.txt"
    echo "----------------------------------------"

    # If noRunNumberLimit is set, also create ≥47289-only version for comparison.
    if $NO_RUNNUMBER_LIMIT; then
        awk '$1 >= 47289' "$output_file_final_v1" > FileLists/Full_ppGoldenRunList_ge47289_Version1.txt
        cp FileLists/Full_ppGoldenRunList_ge47289_Version1.txt dst_list/Final_RunNumbers_After_All_Cuts_ge47289.txt

        if $REMOVE_MISSING_MAPS; then
            # Calculate missing maps in ≥47289 scenario:
            missing_maps_ge47289=$(grep -Fxf FileLists/Full_ppGoldenRunList_ge47289_Version1.txt list/list_runs_missing_bad_tower_maps.txt | wc -l)
            export missing_maps_ge47289
        fi
    fi
}

create_list_file() {
    echo "Creating final .list file..."
    cp "FileLists/Full_ppGoldenRunList_Version1.txt" Full_ppGoldenRunList_Version1.list
    echo ".list file created."
    echo "----------------------------------------"
}

clean_old_dst_lists() {
    echo "Cleaning old DST lists..."
    rm -f dst_list/*.list
    echo "Old DST lists removed."
    echo "----------------------------------------"
}

generate_dst_lists() {
    if $DONT_GENERATE_FILELISTS; then
        echo "[INFO]: Skipping DST list generation due to 'dontGenerateFileLists'."
        return
    fi
    echo "Generating DST lists..."
    cd dst_list
    CreateDstList.pl --build ana437 --cdb 2024p007 DST_CALO_run2pp --list ../Full_ppGoldenRunList_Version1.list
    echo "DST lists generated."
    echo "----------------------------------------"
    cd ..
}

compute_event_counts() {
    actual_events_initial=$(get_actual_events_from_evt 'list/list_runnumber_all.txt')
    actual_events_calo_qa=$(get_actual_events_from_evt 'list/Full_ppGoldenRunList.txt')
    actual_events_after_runtime=$(get_actual_events_from_evt 'list/list_runnumber_runtime_v1.txt')
    actual_events_after_livetime=$(get_actual_events_from_evt 'list/list_runnumber_livetime_v1.txt')
    actual_events_after_badtower=$(get_actual_events_from_evt "FileLists/Full_ppGoldenRunList_Version1.txt")

    total_runs_after_badtower=$(wc -l < "FileLists/Full_ppGoldenRunList_Version1.txt")

    percent_runs_calo_qa=$(echo "scale=2; $combined_golden_runs / $total_runs * 100" | bc)
    percent_runs_after_runtime=$(echo "scale=2; $total_runs_duration_v1 / $total_runs * 100" | bc)
    percent_runs_after_livetime=$(echo "scale=2; $total_runs_livetime_v1 / $total_runs * 100" | bc)
    percent_runs_after_badtower=$(echo "scale=2; $total_runs_after_badtower / $total_runs * 100" | bc)

    percent_actual_events_calo_qa=$(echo "scale=2; $actual_events_calo_qa / $actual_events_initial * 100" | bc)
    percent_actual_events_after_runtime=$(echo "scale=2; $actual_events_after_runtime / $actual_events_initial * 100" | bc)
    percent_actual_events_after_livetime=$(echo "scale=2; $actual_events_after_livetime / $actual_events_initial * 100" | bc)
    percent_actual_events_after_badtower=$(echo "scale=2; $actual_events_after_badtower / $actual_events_initial * 100" | bc)

    if $NO_RUNNUMBER_LIMIT; then
        if [[ -f FileLists/Full_ppGoldenRunList_ge47289_Version1.txt ]]; then
            actual_events_after_badtower_ge47289=$(get_actual_events_from_evt "FileLists/Full_ppGoldenRunList_ge47289_Version1.txt")
            total_runs_after_badtower_ge47289=$(wc -l < "FileLists/Full_ppGoldenRunList_ge47289_Version1.txt")
            if $REMOVE_MISSING_MAPS; then
                # missing_maps_ge47289 was exported previously
                :
            fi
        else
            actual_events_after_badtower_ge47289=0
            total_runs_after_badtower_ge47289=0
            missing_maps_ge47289=0
        fi
    fi
}

final_summary() {
    echo -e "${BOLD}${MAGENTA}========================================${RESET}"
    echo -e "${BOLD}${MAGENTA}Final Summary (Version 1)${RESET}"
    echo -e "${MAGENTA}----------------------------------------${RESET}"
    printf "%-50s | %-35s | %-25s\n" "Stage" ".evt File Events" "Runs"
    echo "--------------------------------------------------|-------------------------------------|-------------------------"

    if $NO_RUNNUMBER_LIMIT; then
        run_label="${total_runs} (OVER FULL RUN LIST)"
    else
        run_label="${total_runs} (100%)"
    fi

    printf "%-50s | %-35s | %-25s\n" \
    "1) After firmware fix (47289) and >1M events" \
    "${actual_events_initial} (100%)" \
    "$run_label"

    printf "%-50s | %-35s | %-25s\n" \
    "2) && Golden EMCal/HCal" \
    "${actual_events_calo_qa} (${percent_actual_events_calo_qa}%)" \
    "${combined_golden_runs} (${percent_runs_calo_qa}%)"

    printf "%-50s | %-35s | %-25s\n" \
    "3) && > 5 minutes" \
    "${actual_events_after_runtime} (${percent_actual_events_after_runtime}%)" \
    "${total_runs_duration_v1} (${percent_runs_after_runtime}%)"

    printf "%-50s | %-35s | %-25s\n" \
    "4) && MB livetime > 80%" \
    "${actual_events_after_livetime} (${percent_actual_events_after_livetime}%)" \
    "${total_runs_livetime_v1} (${percent_runs_after_livetime}%)"

    if $REMOVE_MISSING_MAPS; then
        step_label="5) && With Bad Tower Map Available (Removed)"
    else
        step_label="5) Identified Missing Bad Tower Maps (Not Removed)"
    fi

    printf "%-50s | %-35s | %-25s\n" \
    "$step_label" \
    "${actual_events_after_badtower} (${percent_actual_events_after_badtower}%)" \
    "${total_runs_after_badtower} (${percent_runs_after_badtower}%)"

    # Always report missing-map runs:
    # If remove not requested: note they remain in final DST
    # If remove requested: note they are not in final DST
    if $REMOVE_MISSING_MAPS; then
        map_note="Removed from final DST"
    else
        map_note="Kept in final DST"
    fi

    printf "%-50s | %-35s | %-25s\n" \
    "Missing Map Runs" \
    "-" \
    "${total_runs_missing_bad_tower} (${map_note})"

    echo "================================================="
    echo "========================================"

    if $REMOVE_MISSING_MAPS; then
        echo "Final golden run list (missing maps removed): $workplace/dst_list/Final_RunNumbers_After_All_Cuts.txt"
    else
        echo "Final golden run list (with runs missing maps included): $workplace/dst_list/Final_RunNumbers_After_All_Cuts.txt"
    fi

    echo "Done."

    if $NO_RUNNUMBER_LIMIT; then
        echo ""
        echo "----------------------------------------"
        echo -e "${BOLD}${MAGENTA}Additional Comparison:${RESET} No-limit vs. ≥47289 scenario"
        echo ""

        ev_all=${actual_events_after_badtower:-0}
        ev_ge=${actual_events_after_badtower_ge47289:-0}
        run_all=${total_runs_after_badtower:-0}
        run_ge=${total_runs_after_badtower_ge47289:-0}

        diff_ev=$(echo "$ev_all - $ev_ge" | bc)
        if (( $(echo "$ev_all > 0" | bc -l) )); then
            perc_ev_lost=$(echo "scale=2; $diff_ev/$ev_all*100" | bc)
        else
            perc_ev_lost=0
        fi

        diff_run=$(echo "$run_all - $run_ge" | bc)
        if (( $(echo "$run_all > 0" | bc -l) )); then
            perc_run_lost=$(echo "scale=2; $diff_run/$run_all*100" | bc)
        else
            perc_run_lost=0
        fi

        printf "%-40s | %-20s | %-20s | %-20s | %-20s\n" "Metric" "NoLimit(All)" "≥47289" "ΔValue" "%Lost"
        echo "---------------------------------------------------------------------------------------------------------"
        printf "%-40s | %-20s | %-20s | %-20s | %-20s\n" \
          "Events After All Cuts" "$ev_all" "$ev_ge" "$diff_ev" "${perc_ev_lost}%"
        printf "%-40s | %-20s | %-20s | %-20s | %-20s\n" \
          "Runs After All Cuts" "$run_all" "$run_ge" "$diff_run" "${perc_run_lost}%"

        if $REMOVE_MISSING_MAPS; then
            missing_maps_all=${total_runs_missing_bad_tower:-0}
            missing_maps_ge47289=${missing_maps_ge47289:-0}

            diff_maps=$(echo "$missing_maps_all - $missing_maps_ge47289" | bc)
            if (( $(echo "$missing_maps_all > 0" | bc -l) )); then
                perc_maps_lost=$(echo "scale=2; $diff_maps/$missing_maps_all*100" | bc)
            else
                perc_maps_lost=0
            fi

            echo ""
            echo "Differences in Missing Bad Tower Map Runs:"
            printf "%-40s | %-20s | %-20s | %-20s | %-20s\n" "Metric" "NoLimit(All)" "≥47289" "ΔValue" "%Lost"
            echo "---------------------------------------------------------------------------------------------------------"
            printf "%-40s | %-20s | %-20s | %-20s | %-20s\n" \
              "Missing Map Runs" "$missing_maps_all" "$missing_maps_ge47289" "$diff_maps" "${perc_maps_lost}%"
        fi
    fi
}

########################################
# MAIN EXECUTION FLOW
########################################

echo -e "${BOLD}${GREEN}========================================${RESET}"
echo -e "${BOLD}${GREEN}Starting the Golden Run List Generation${RESET}"
echo -e "${BOLD}${GREEN}========================================${RESET}"
parse_arguments "$@"
setup_directories
set_workplace
extract_initial_runs
validate_golden_list
apply_incremental_cuts_header
runtime_cut
livetime_cut
missing_bad_tower_maps_step
create_list_file
clean_old_dst_lists

if ! $DONT_GENERATE_FILELISTS; then
    generate_dst_lists
fi

compute_event_counts
final_summary
