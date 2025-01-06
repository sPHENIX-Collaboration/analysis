#!/bin/bash

##############################################################################################################################################################################
# Golden Run List Generation Script
#
# Purpose:
#   This script produces a final list of "Golden Runs" for further analysis within sPHENIX data-taking. It ensures that each run passes a series of checks:
#   • Has ≥1M events (and if "noRunNumberLimit" is not specified, also runnumber ≥47289).
#   • Meets "Golden" Calorimeter QA criteria for EMCal, IHCal, and OHCal.
#   • Exceeds a minimum runtime of 5 minutes.
#   • Has an MBD (minimum bias) livetime above 80%.
#   • (Optionally) Is not missing any bad tower maps, if "removeRunsWithMissingMaps" is supplied.
#
# Main Steps:
#   1) Initial run extraction:
#      - Pull all runs from FileCatalog with ≥1M events.
#      - If "noRunNumberLimit" isn't provided, further require runnumber ≥47289.
#   2) Calo QA filtering:
#      - Cross-check these runs against the Production_write database to keep only those marked as "Golden" for EMCal, IHCal, and OHCal.
#   3) Additional cuts:
#      a) Runtime: Only keep runs with more than 5 minutes of data.
#      b) Livetime: Only keep runs where MBD livetime >80%.
#   4) Optional bad tower maps check:
#      - If "removeRunsWithMissingMaps" is given, discard runs that do not have a valid bad tower map available.
#   5) Final run list generation:
#      - Output "Final_RunNumbers_After_All_Cuts.txt" to the "../dst_list/" directory.
#      - Also produce "Full_ppGoldenRunList_Version1.list" for use with CreateDstList.pl (unless "dontGenerateFileLists" is specified).
#   6) DST creation verification:
#      - Compare the final run list to the .list files that were successfully created, to determine how many runs succeeded or failed in generating DST lists.
#   7) No-run-limit comparison:
#      - If "noRunNumberLimit" is provided, the script also reports how many runs/events are included when ignoring the runnumber ≥47289 criterion, versus strictly requiring runnumber ≥47289.
#
# Usage:
#   ./GoldenRunList_ConductorFile.sh [removeRunsWithMissingMaps] [dontGenerateFileLists] [noRunNumberLimit]
#     - removeRunsWithMissingMaps: Exclude runs lacking bad tower maps.
#     - dontGenerateFileLists: Skip creating DST .list files (for use with CreateDstList.pl).
#     - noRunNumberLimit: Do not enforce runnumber ≥47289.
#
# Output:
#   - Final text file: ../dst_list/Final_RunNumbers_After_All_Cuts.txt (and optionally ../dst_list/Final_RunNumbers_After_All_Cuts_ge47289.txt).
#   - Internal .list files for DST creation in the current directory and ../dst_list, unless 'dontGenerateFileLists' was set.
#
# Notes:
#   - Percentages in the summary table are computed two ways:
#       1) Run-based percentages use the "Stage 1" runs as the 100% reference.
#       2) Event-based percentages use the total events from "Stage 1."
#   - The script can also print a comparison for run≥47289 versus no lower limit (if "noRunNumberLimit" is passed).
##############################################################################################################################################################################



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
        case "$arg" in
            removeRunsWithMissingMaps)
                REMOVE_MISSING_MAPS=true
                echo -e "${BOLD}${CYAN}Argument detected:${RESET} Removing runs missing bad tower maps."
                ;;
            dontGenerateFileLists)
                DONT_GENERATE_FILELISTS=true
                echo -e "${BOLD}${CYAN}Argument detected:${RESET} Will not generate DST lists."
                ;;
            noRunNumberLimit)
                NO_RUNNUMBER_LIMIT=true
                echo -e "${BOLD}${CYAN}Argument detected:${RESET} No run number lower limit will be applied."
                ;;
        esac
    done

    if ! $REMOVE_MISSING_MAPS; then
        echo "No removal option detected: Missing-map runs will be kept."
    fi
    echo "----------------------------------------"
}

clean_previous_data() {
    echo -e "${BOLD}${GREEN}Cleaning old data (dst_list contents, intermediate files)...${RESET}"

    rm -rf list/*
    rm -rf FileLists/*
    rm -f "${workplace}/../dst_list/"*.list

    rm -f "${workplace}/../dst_list/Final_RunNumbers_After_All_Cuts.txt"
    rm -f "${workplace}/../dst_list/Final_RunNumbers_After_All_Cuts_ge47289.txt"

    rm -rf "${workplace}/../dst_list_scratch"

    echo "All old data removed. Starting fresh."
    echo "----------------------------------------"
}

setup_directories() {
    echo -e "${BOLD}${GREEN}Setting up directories...${RESET}"

    base_path="${workplace}/.."
    mkdir -p FileLists/
    mkdir -p "${base_path}/dst_list"
    mkdir -p list/

    echo "Directories ready under ${base_path}/"
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
    query = """
    SELECT runnumber
    FROM datasets
    WHERE dsttype='DST_CALO_run2pp' AND dataset='ana450_2024p009'
    GROUP BY runnumber
    HAVING SUM(events) >= 1000000;
    """
else:
    query = """
    SELECT runnumber
    FROM datasets
    WHERE dsttype='DST_CALO_run2pp' AND dataset='ana450_2024p009'
    GROUP BY runnumber
    HAVING SUM(events) >= 1000000 AND runnumber >= 47289;
    """

fc_cursor.execute(query)
all_runs = [row.runnumber for row in fc_cursor.fetchall()]
all_runs.sort()

with open('list/list_runnumber_all.txt', 'w') as f:
    for r in all_runs:
        f.write(f"{r}\\n")
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
    prod_cursor.execute(
        f"SELECT runnumber FROM goodruns WHERE ({d}_auto).runclass='GOLDEN'"
    )
    detector_golden = {row.runnumber for row in prod_cursor.fetchall()}
    golden_runs = golden_runs.intersection(detector_golden)

golden_runs = sorted(golden_runs)

with open('list/Full_ppGoldenRunList.txt', 'w') as f:
    for r in golden_runs:
        f.write(f"{r}\\n")

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
    if [[ ! -f "list/Full_ppGoldenRunList.txt" ]]; then
        error_exit "list/Full_ppGoldenRunList.txt not found. Possibly no runs qualify."
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
            query="SELECT SUM(lastevent - firstevent + 1)
                   FROM filelist
                   WHERE runnumber IN ($run_list)
                     AND filename LIKE '%GL1_physics_gl1daq%.evt';"
            result=$(psql -h sphnxdaqdbreplica -d daq -t -c "$query")
            events=$(echo "$result" | xargs)
            if [[ "$events" =~ ^[0-9]+$ ]]; then
                total_events=$(echo "$total_events + $events" | bc)
            fi
        fi
    done < "$input_file"

    # leftover
    if [[ ${#run_numbers[@]} -gt 0 ]]; then
        run_list=$(IFS=,; echo "${run_numbers[*]}")
        query="SELECT SUM(lastevent - firstevent + 1)
               FROM filelist
               WHERE runnumber IN ($run_list)
                 AND filename LIKE '%GL1_physics_gl1daq%.evt';"
        result=$(psql -h sphnxdaqdbreplica -d daq -t -c "$query")
        events=$(echo "$result" | xargs)
        if [[ "$events" =~ ^[0-9]+$ ]]; then
            total_events=$(echo "$total_events + $events" | bc)
        fi
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
        query="SELECT EXTRACT(EPOCH FROM (ertimestamp - brtimestamp))
               FROM run
               WHERE runnumber = ${runnumber};"
        result=$(psql -h sphnxdaqdbreplica -d daq -t -c "$query" | tr -d '[:space:]')
        duration="$result"

        if [[ "$duration" =~ ^[0-9]+(\.[0-9]+)?$ ]] && (( $(echo "$duration > 300" | bc -l) )); then
            echo "$runnumber" >> "$output_file_duration_v1"
            (( total_runs_duration_v1++ ))
        else
            (( runs_dropped_runtime_v1++ ))
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
        query="SELECT raw, live
               FROM gl1_scalers
               WHERE runnumber = ${runnumber}
                 AND index = ${index_to_check};"
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
            (( total_runs_livetime_v1++ ))
        else
            echo "$runnumber" >> "$bad_file_livetime_v1"
            (( runs_dropped_livetime_v1++ ))
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

    available_bad_tower_runs=$(find /cvmfs/sphenix.sdcc.bnl.gov/calibrations/sphnxpro/cdb/CEMC_BadTowerMap \
                               -name "*p0*" | cut -d '-' -f2 | cut -d c -f1 | sort | uniq)
    echo "$available_bad_tower_runs" > list/available_bad_tower_runs.txt

    grep -Fxvf list/available_bad_tower_runs.txt "$input_file" > "$bad_tower_runs_file"

    total_runs_with_bad_tower=$(grep -Fxf list/available_bad_tower_runs.txt "$input_file" | wc -l)
    total_runs_missing_bad_tower=$(wc -l < "$bad_tower_runs_file")

    echo "Runs with bad tower maps: $total_runs_with_bad_tower"
    echo "Runs missing bad tower maps: $total_runs_missing_bad_tower"
    echo "List of missing map runs: $bad_tower_runs_file"
    echo "----------------------------------------"

    export total_runs_missing_bad_tower
    rm list/available_bad_tower_runs.txt

    if $REMOVE_MISSING_MAPS; then
        echo "Removing runs missing bad tower maps..."
        grep -Fxf "$bad_tower_runs_file" -v "$output_file_final_v1" > tmp && mv tmp "$output_file_final_v1"
        echo "Removal complete."
        echo "----------------------------------------"
    fi

    cp "$output_file_final_v1" "${workplace}/../dst_list/Final_RunNumbers_After_All_Cuts.txt"
    echo "Final run list stored in ${workplace}/../dst_list/Final_RunNumbers_After_All_Cuts.txt"
    echo "----------------------------------------"

    if $NO_RUNNUMBER_LIMIT; then
        awk '$1 >= 47289' "$output_file_final_v1" > FileLists/Full_ppGoldenRunList_ge47289_Version1.txt
        cp FileLists/Full_ppGoldenRunList_ge47289_Version1.txt \
           "${workplace}/../dst_list/Final_RunNumbers_After_All_Cuts_ge47289.txt"

        if $REMOVE_MISSING_MAPS; then
            missing_maps_ge47289=$(grep -Fxf FileLists/Full_ppGoldenRunList_ge47289_Version1.txt \
                                       list/list_runs_missing_bad_tower_maps.txt | wc -l)
            export missing_maps_ge47289
        fi
    fi
}

create_list_file() {
    echo "Creating final .list file from the final run list..."
    cp "FileLists/Full_ppGoldenRunList_Version1.txt" Full_ppGoldenRunList_Version1.list
    echo ".list file created: Full_ppGoldenRunList_Version1.list"
    echo "----------------------------------------"
}

clean_old_dst_lists() {
    echo "Removing any old DST lists from the parent 'dst_list' directory..."
    rm -f "${workplace}/../dst_list/"*.list
    echo "Done removing old .list files in dst_list/."
    echo "----------------------------------------"
}

generate_dst_lists() {
    if $DONT_GENERATE_FILELISTS; then
        echo "[INFO]: Skipping DST list generation due to 'dontGenerateFileLists'."
        return
    fi

    echo "Generating DST lists for the main scenario..."
    cd "${workplace}/../dst_list"

    list_path="${workplace}/Full_ppGoldenRunList_Version1.list"
    if [[ ! -f "$list_path" ]]; then
        echo "[WARNING] Could not find final .list file at: $list_path"
        echo "No DST lists will be created."
    else
        CreateDstList.pl --build ana450 --cdb 2024p009 DST_CALO_run2pp --list "$list_path"
        echo "DST lists generated under ${workplace}/../dst_list"
    fi

    if $NO_RUNNUMBER_LIMIT; then
        echo "No separate scratch folder is used for the ≥47289 subset."
    fi

    echo "----------------------------------------"
    cd "$workplace"
}

apply_createDstList_cut() {
    echo "Collecting CreateDST File List success/failure for the main scenario..."

    if $DONT_GENERATE_FILELISTS; then
        echo "[INFO]: 'dontGenerateFileLists' was provided, so no DST creation was done for main scenario."
        export total_runs_createDst_success=0
        export runs_dropped_createDst=0
        export actual_events_after_createDst=0
        export total_runs_after_createDst=0
    else
        final_stage4_file="FileLists/Full_ppGoldenRunList_Version1.txt"
        if [[ ! -f "$final_stage4_file" ]]; then
            echo "[ERROR]: Cannot find final stage-4 file: $final_stage4_file"
            export total_runs_createDst_success=0
            export runs_dropped_createDst=0
            export actual_events_after_createDst=0
            export total_runs_after_createDst=0
        else
            success_file="list/list_runnumber_createDstSuccess.txt"
            failure_file="list/list_runnumber_createDstFailure.txt"
            > "$success_file"
            > "$failure_file"

            mapfile -t final_stage4_runs < "$final_stage4_file"

            base_path="${workplace}/../dst_list"
            created_run_nums=()

            for f in "${base_path}/dst_calo_run2pp-"*.list; do
                [ -e "$f" ] || continue
                bn=$(basename "$f" .list)
                runnum_str=${bn#dst_calo_run2pp-}
                if [[ "$runnum_str" =~ ^0*([0-9]+)$ ]]; then
                    runnum=${BASH_REMATCH[1]}
                    created_run_nums+=("$runnum")
                fi
            done

            total_runs_createDst_success=0
            runs_dropped_createDst=0
            declare -A in_created
            for rn in "${created_run_nums[@]}"; do
                in_created["$rn"]=1
            done

            for runnumber in "${final_stage4_runs[@]}"; do
                if [[ -n "${in_created[$runnumber]}" ]]; then
                    echo "$runnumber" >> "$success_file"
                    (( total_runs_createDst_success++ ))
                else
                    echo "$runnumber" >> "$failure_file"
                    (( runs_dropped_createDst++ ))
                fi
            done

            echo "Runs with successful .list creation: $total_runs_createDst_success"
            echo "Runs with no .list file: $runs_dropped_createDst"
            echo "List of runs that failed:  $failure_file"
            echo "List of runs that succeeded: $success_file"
            echo "----------------------------------------"

            actual_events_after_createDst=$(get_actual_events_from_evt "$success_file")
            total_runs_after_createDst=$total_runs_createDst_success

            cp "$success_file" "FileLists/Full_ppGoldenRunList_Version1_DSTsuccess.txt"

            export total_runs_createDst_success
            export runs_dropped_createDst
            export actual_events_after_createDst
            export total_runs_after_createDst
        fi
    fi

    # ≥47289 scenario
    if $NO_RUNNUMBER_LIMIT && ! $DONT_GENERATE_FILELISTS; then
        echo "Collecting CreateDST File List success/failure for the '≥47289' scenario..."
        final_stage4_file_ge47289="FileLists/Full_ppGoldenRunList_ge47289_Version1.txt"
        if [[ ! -f "$final_stage4_file_ge47289" ]]; then
            echo "[ERROR]: Cannot find the ge47289 final list: $final_stage4_file_ge47289"
            export total_runs_createDst_success_ge47289=0
            export runs_dropped_createDst_ge47289=0
            export actual_events_after_createDst_ge47289=0
            export total_runs_after_createDst_ge47289=0
        else
            success_file_ge47289="list/list_runnumber_createDstSuccess_ge47289.txt"
            failure_file_ge47289="list/list_runnumber_createDstFailure_ge47289.txt"
            > "$success_file_ge47289"
            > "$failure_file_ge47289"

            mapfile -t final_stage4_runs_ge47289 < "$final_stage4_file_ge47289"

            base_path_scratch="${workplace}/../dst_list"
            created_run_nums_ge47289=()

            for f in "${base_path_scratch}/dst_calo_run2pp-"*.list; do
                [ -e "$f" ] || continue
                bn=$(basename "$f" .list)
                runnum_str=${bn#dst_calo_run2pp-}
                if [[ "$runnum_str" =~ ^0*([0-9]+)$ ]]; then
                    runnum=${BASH_REMATCH[1]}
                    created_run_nums_ge47289+=("$runnum")
                fi
            done

            total_runs_createDst_success_ge47289=0
            runs_dropped_createDst_ge47289=0
            declare -A in_created_ge47289
            for rn in "${created_run_nums_ge47289[@]}"; do
                in_created_ge47289["$rn"]=1
            done

            for runnumber in "${final_stage4_runs_ge47289[@]}"; do
                if [[ -n "${in_created_ge47289[$runnumber]}" ]]; then
                    echo "$runnumber" >> "$success_file_ge47289"
                    (( total_runs_createDst_success_ge47289++ ))
                else
                    echo "$runnumber" >> "$failure_file_ge47289"
                    (( runs_dropped_createDst_ge47289++ ))
                fi
            done

            echo "≥47289 scenario: runs with successful .list creation: $total_runs_createDst_success_ge47289"
            echo "≥47289 scenario: runs with no .list file: $runs_dropped_createDst_ge47289"
            echo "≥47289 scenario: List of runs that failed:  $failure_file_ge47289"
            echo "≥47289 scenario: List of runs that succeeded: $success_file_ge47289"
            echo "----------------------------------------"

            actual_events_after_createDst_ge47289=$(get_actual_events_from_evt "$success_file_ge47289")
            total_runs_after_createDst_ge47289=$total_runs_createDst_success_ge47289

            cp "$success_file_ge47289" "FileLists/Full_ppGoldenRunList_ge47289_Version1_DSTsuccess.txt"

            export total_runs_createDst_success_ge47289
            export runs_dropped_createDst_ge47289
            export actual_events_after_createDst_ge47289
            export total_runs_after_createDst_ge47289
        fi
    fi
}

compute_event_counts() {
    # Runs in list_runnumber_all.txt => "Stage 1" denominator for runs
    # But the code will handle the difference if noRunNumberLimit is used
    actual_events_before_cuts=$(get_actual_events_from_evt 'list/list_runnumber_all.txt')
    total_runs_before_cuts=$(wc -l < 'list/list_runnumber_all.txt')

    # Stage 1 events
    actual_events_initial=$actual_events_before_cuts

    # Stage 2 (Calo QA)
    actual_events_calo_qa=$(get_actual_events_from_evt 'list/Full_ppGoldenRunList.txt')

    # Stage 3 (runtime)
    actual_events_after_runtime=$(get_actual_events_from_evt 'list/list_runnumber_runtime_v1.txt')

    # Stage 4 (livetime)
    actual_events_after_livetime=$(get_actual_events_from_evt 'list/list_runnumber_livetime_v1.txt')

    # Stage 5 (missing map removal)
    actual_events_after_badtower=$(get_actual_events_from_evt 'FileLists/Full_ppGoldenRunList_Version1.txt')
    total_runs_after_badtower=$(wc -l < "FileLists/Full_ppGoldenRunList_Version1.txt")

    # Stage 6 (DST creation success)
    if [[ -f FileLists/Full_ppGoldenRunList_Version1_DSTsuccess.txt ]]; then
        actual_events_after_createDst=$(get_actual_events_from_evt "FileLists/Full_ppGoldenRunList_Version1_DSTsuccess.txt")
        total_runs_after_createDst=$(wc -l < "FileLists/Full_ppGoldenRunList_Version1_DSTsuccess.txt")
    else
        actual_events_after_createDst=0
        total_runs_after_createDst=0
    fi

    # For run-based percentages, define STAGE1_RUNS as "the total after Stage 1"
    # which is total_runs_before_cuts if noRunNumberLimit is used,
    # or equivalently the same if we forcibly filtered to ≥47289 in Stage 1.
    # So STAGE1_RUNS is actually 'total_runs_before_cuts'.
    # We'll recast subsequent run-based percentages using that as 100%.

    STAGE1_RUNS=$total_runs_before_cuts  # denominator for run percentages

    # Next, define how many runs remain after each cut:
    runs_after_calo_qa=$(wc -l < 'list/Full_ppGoldenRunList.txt')
    runs_after_runtime=$(( total_runs_duration_v1 ))   # from runtime_cut
    runs_after_livetime=$(( total_runs_livetime_v1 ))  # from livetime_cut
    runs_after_badtower=$(( total_runs_after_badtower ))
    runs_after_createDst=$(( total_runs_after_createDst ))

    # Now compute run-based percentages relative to STAGE1_RUNS
    percent_runs_calo_qa=$(echo "scale=2; 100.0*$runs_after_calo_qa/$STAGE1_RUNS" | bc)
    percent_runs_after_runtime=$(echo "scale=2; 100.0*$runs_after_runtime/$STAGE1_RUNS" | bc)
    percent_runs_after_livetime=$(echo "scale=2; 100.0*$runs_after_livetime/$STAGE1_RUNS" | bc)
    percent_runs_after_badtower=$(echo "scale=2; 100.0*$runs_after_badtower/$STAGE1_RUNS" | bc)
    percent_runs_after_createDst=$(echo "scale=2; 100.0*$runs_after_createDst/$STAGE1_RUNS" | bc)

    # For event-based percentages, denominator is "Events Before All Cuts"
    if [[ "$actual_events_before_cuts" -eq 0 ]]; then
        percent_actual_events_calo_qa=0
        percent_actual_events_after_runtime=0
        percent_actual_events_after_livetime=0
        percent_actual_events_after_badtower=0
        percent_events_after_createDst=0
    else
        percent_actual_events_calo_qa=$(echo "scale=2; 100.0*$actual_events_calo_qa/$actual_events_before_cuts" | bc)
        percent_actual_events_after_runtime=$(echo "scale=2; 100.0*$actual_events_after_runtime/$actual_events_before_cuts" | bc)
        percent_actual_events_after_livetime=$(echo "scale=2; 100.0*$actual_events_after_livetime/$actual_events_before_cuts" | bc)
        percent_actual_events_after_badtower=$(echo "scale=2; 100.0*$actual_events_after_badtower/$actual_events_before_cuts" | bc)
        percent_events_after_createDst=$(echo "scale=2; 100.0*$actual_events_after_createDst/$actual_events_before_cuts" | bc)
    fi

    # If noRunNumberLimit => also handle ge47289 scenario
    if $NO_RUNNUMBER_LIMIT; then
        if [[ -f FileLists/Full_ppGoldenRunList_ge47289_Version1.txt ]]; then
            ge_temp_list="list/list_runnumber_all_ge47289.txt"
            awk '$1 >= 47289' list/list_runnumber_all.txt > "$ge_temp_list"
            actual_events_before_cuts_ge47289=$(get_actual_events_from_evt "$ge_temp_list")
            total_runs_before_cuts_ge47289=$(wc -l < "$ge_temp_list")
            rm -f "$ge_temp_list"

            # after missing maps
            actual_events_after_badtower_ge47289=$(get_actual_events_from_evt "FileLists/Full_ppGoldenRunList_ge47289_Version1.txt")
            runs_after_badtower_ge47289=$(wc -l < "FileLists/Full_ppGoldenRunList_ge47289_Version1.txt")

            # after DST success
            if [[ -f FileLists/Full_ppGoldenRunList_ge47289_Version1_DSTsuccess.txt ]]; then
                actual_events_after_createDst_ge47289=$(get_actual_events_from_evt "FileLists/Full_ppGoldenRunList_ge47289_Version1_DSTsuccess.txt")
                runs_after_createDst_ge47289=$(wc -l < "FileLists/Full_ppGoldenRunList_ge47289_Version1_DSTsuccess.txt")
            else
                actual_events_after_createDst_ge47289=0
                runs_after_createDst_ge47289=0
            fi

            export actual_events_before_cuts_ge47289
            export total_runs_before_cuts_ge47289
            export actual_events_after_badtower_ge47289
            export runs_after_badtower_ge47289
            export actual_events_after_createDst_ge47289
            export runs_after_createDst_ge47289
        else
            actual_events_before_cuts_ge47289=0
            total_runs_before_cuts_ge47289=0
            actual_events_after_badtower_ge47289=0
            runs_after_badtower_ge47289=0
            actual_events_after_createDst_ge47289=0
            runs_after_createDst_ge47289=0
        fi
    fi

    # Exports for final_summary
    export STAGE1_RUNS
    export runs_after_calo_qa
    export runs_after_runtime
    export runs_after_livetime
    export runs_after_badtower
    export runs_after_createDst

    export actual_events_before_cuts
    export total_runs_before_cuts
    export actual_events_initial
    export actual_events_calo_qa
    export actual_events_after_runtime
    export actual_events_after_livetime
    export actual_events_after_badtower
    export actual_events_after_createDst

    export percent_runs_calo_qa
    export percent_runs_after_runtime
    export percent_runs_after_livetime
    export percent_runs_after_badtower
    export percent_runs_after_createDst

    export percent_actual_events_calo_qa
    export percent_actual_events_after_runtime
    export percent_actual_events_after_livetime
    export percent_actual_events_after_badtower
    export percent_events_after_createDst
}

final_summary() {
    echo -e "${BOLD}${MAGENTA}========================================${RESET}"
    echo -e "${BOLD}${MAGENTA}Final Summary (Version 1)${RESET}"
    echo -e "${MAGENTA}----------------------------------------${RESET}"
    printf "%-50s | %-35s | %-25s\n" "Stage" ".evt File Events" "Runs"
    echo "--------------------------------------------------|-------------------------------------|-------------------------"

    # The label for Stage 1 depends on whether we used noRunNumberLimit
    if $NO_RUNNUMBER_LIMIT; then
        stage1_label="≥1M events"
    else
        stage1_label="≥47289 & ≥1M events"
    fi

    # The run count for Stage 1 is STAGE1_RUNS (the number of runs in list_runnumber_all.txt).
    printf "%-50s | %-35s | %-25s\n" \
    "1) $stage1_label" \
    "${actual_events_initial} (100%)" \
    "${STAGE1_RUNS} (100%)"

    printf "%-50s | %-35s | %-25s\n" \
    "2) && Golden EMCal/HCal" \
    "${actual_events_calo_qa} (${percent_actual_events_calo_qa}%)" \
    "${runs_after_calo_qa} (${percent_runs_calo_qa}%)"

    printf "%-50s | %-35s | %-25s\n" \
    "3) && > 5 minutes" \
    "${actual_events_after_runtime} (${percent_actual_events_after_runtime}%)" \
    "${runs_after_runtime} (${percent_runs_after_runtime}%)"

    printf "%-50s | %-35s | %-25s\n" \
    "4) && MB livetime > 80%" \
    "${actual_events_after_livetime} (${percent_actual_events_after_livetime}%)" \
    "${runs_after_livetime} (${percent_runs_after_livetime}%)"

    if $REMOVE_MISSING_MAPS; then
        step_label="5) && With Bad Tower Map Available (Removed)"
    else
        step_label="5) && With Bad Tower Map Available (Not Removed)"
    fi
    printf "%-50s | %-35s | %-25s\n" \
    "$step_label" \
    "${actual_events_after_badtower} (${percent_actual_events_after_badtower}%)" \
    "${runs_after_badtower} (${percent_runs_after_badtower}%)"

    printf "%-50s | %-35s | %-25s\n" \
    "6) && CreateDST File List Successes" \
    "${actual_events_after_createDst} (${percent_events_after_createDst}%)" \
    "${runs_after_createDst} (${percent_runs_after_createDst}%)"

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
        echo "Final golden run list (missing maps removed):"
    else
        echo "Final golden run list (with runs missing maps included):"
    fi
    echo " --> ${workplace}/../dst_list/Final_RunNumbers_After_All_Cuts.txt"
    echo "Done."

    if $NO_RUNNUMBER_LIMIT; then
        echo ""
        echo "----------------------------------------"
        echo -e "${BOLD}${MAGENTA}Additional Comparison:${RESET} No-limit(All) vs. ≥47289 scenario"
        echo "Now also includes a 'Before All Cuts' row for both scenarios."
        echo ""

        cat <<EOCOMPARISON

Metric                                  | NoLimit(All)                | ≥47289                    | ΔValue           | %Lost
--------------------------------------------------------------------------------------------------------------
EOCOMPARISON

        # Before All Cuts
        diff_events_before=$(echo "${actual_events_before_cuts:-0} - ${actual_events_before_cuts_ge47289:-0}" | bc)
        if (( $(echo "${actual_events_before_cuts:-0} > 0" | bc -l) )); then
            perc_lost_events_before=$(echo "scale=2; 100.0*$diff_events_before/${actual_events_before_cuts:-0}" | bc)
        else
            perc_lost_events_before=0
        fi

        printf "%-40s | %-20s | %-20s | %-20s | %-20s\n" \
          "Events Before All Cuts" \
          "${actual_events_before_cuts:-0}" \
          "${actual_events_before_cuts_ge47289:-0}" \
          "$diff_events_before" \
          "${perc_lost_events_before}%"

        diff_runs_before=$(echo "${total_runs_before_cuts:-0} - ${total_runs_before_cuts_ge47289:-0}" | bc)
        if (( $(echo "${total_runs_before_cuts:-0} > 0" | bc -l) )); then
            perc_lost_runs_before=$(echo "scale=2; 100.0*$diff_runs_before/${total_runs_before_cuts:-0}" | bc)
        else
            perc_lost_runs_before=0
        fi

        printf "%-40s | %-20s | %-20s | %-20s | %-20s\n" \
          "Runs Before All Cuts" \
          "${total_runs_before_cuts:-0}" \
          "${total_runs_before_cuts_ge47289:-0}" \
          "$diff_runs_before" \
          "${perc_lost_runs_before}%"

        echo ""

        # After All Cuts => DST success
        ev_all=${actual_events_after_createDst:-0}
        ev_ge=${actual_events_after_createDst_ge47289:-0}
        diff_ev=$(echo "$ev_all - $ev_ge" | bc)
        if (( $(echo "$ev_all > 0" | bc -l) )); then
            perc_ev_lost=$(echo "scale=2; 100.0*$diff_ev/$ev_all" | bc)
        else
            perc_ev_lost=0
        fi

        printf "%-40s | %-20s | %-20s | %-20s | %-20s\n" \
          "Events After All Cuts" \
          "$ev_all" \
          "$ev_ge" \
          "$diff_ev" \
          "${perc_ev_lost}%"

        run_all=${runs_after_createDst:-0}
        run_ge=${runs_after_createDst_ge47289:-0}
        diff_run=$(echo "$run_all - $run_ge" | bc)
        if (( $(echo "$run_all > 0" | bc -l) )); then
            perc_run_lost=$(echo "scale=2; 100.0*$diff_run/$run_all" | bc)
        else
            perc_run_lost=0
        fi

        printf "%-40s | %-20s | %-20s | %-20s | %-20s\n" \
          "Runs After All Cuts" \
          "$run_all" \
          "$run_ge" \
          "$diff_run" \
          "${perc_run_lost}%"

        echo ""
        echo "≥47289: DST successes = ${runs_after_createDst_ge47289:-0}, events = ${actual_events_after_createDst_ge47289:-0}"

        if $REMOVE_MISSING_MAPS; then
            missing_maps_all=${total_runs_missing_bad_tower:-0}
            missing_maps_ge47289=${missing_maps_ge47289:-0}

            diff_maps=$(echo "$missing_maps_all - $missing_maps_ge47289" | bc)
            if (( $(echo "$missing_maps_all > 0" | bc -l) )); then
                perc_maps_lost=$(echo "scale=2; 100.0*$diff_maps/$missing_maps_all" | bc)
            else
                perc_maps_lost=0
            fi

            echo ""
            echo "Differences in Missing Bad Tower Map Runs:"
            printf "%-40s | %-20s | %-20s | %-20s | %-20s\n" \
              "Missing Map Runs" \
              "$missing_maps_all" \
              "$missing_maps_ge47289" \
              "$diff_maps" \
              "${perc_maps_lost}%"
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
set_workplace
setup_directories
clean_previous_data
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

apply_createDstList_cut
compute_event_counts
final_summary
