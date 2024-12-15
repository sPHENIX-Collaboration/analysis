#!/bin/bash

# Usage:
# ./GoldenRunList_ConderFile.sh [removeRunsWithMissingMaps] [dontGenerateFileLists] [noRunNumberLimit]
#
# Optional Arguments:
#   removeRunsWithMissingMaps: If provided, runs that lack bad tower maps are excluded.
#   dontGenerateFileLists: If provided, the final step of generating DST lists is skipped.
#   noRunNumberLimit: If provided, the run number lower limit (>=47289) is removed from the event selection query.
#
# The script retrieves, filters, and processes run information based on event criteria, calorimeter quality assurance
# (Calo QA), runtime, livetime, and bad tower map availability. If removeRunsWithMissingMaps is specified, runs without
# bad tower maps are removed. If dontGenerateFileLists is specified, the DST list creation step at the end is not executed.
# If noRunNumberLimit is specified, the lower run number limit is not applied.
#
# Logic, output, and ordering remain unchanged unless dontGenerateFileLists or noRunNumberLimit are specified. In that case,
# either the DST list generation step is skipped or the run number lower limit is removed from the initial query, respectively.

########################################
# FUNCTIONS
########################################

parse_arguments() {
    REMOVE_MISSING_MAPS=false
    DONT_GENERATE_FILELISTS=false
    NO_RUNNUMBER_LIMIT=false

    for arg in "$@"; do
        if [[ "$arg" == "removeRunsWithMissingMaps" ]]; then
            REMOVE_MISSING_MAPS=true
            echo "Argument detected: Will remove runs missing bad tower maps."
        fi
        if [[ "$arg" == "dontGenerateFileLists" ]]; then
            DONT_GENERATE_FILELISTS=true
        fi
        if [[ "$arg" == "noRunNumberLimit" ]]; then
            NO_RUNNUMBER_LIMIT=true
            echo "Argument detected: No run number lower limit will be applied."
        fi
    done

    if ! $REMOVE_MISSING_MAPS; then
        echo "No removal option detected: Missing map runs will be noted but kept."
    fi
    echo "----------------------------------------"
}

setup_directories() {
    echo "Setting up directories..."
    mkdir -p FileLists/ dst_list/ list/
    echo "Directories ready."
    echo "----------------------------------------"
}

set_workplace() {
    workplace=$(pwd)
    echo "Working directory: $workplace"
    echo "----------------------------------------"
}

extract_initial_runs() {
    echo "Step 1: Extracting initial runs from databases..."

    # Create a temporary Python script dynamically to handle noRunNumberLimit condition
    python_script=$(cat <<EOF
import pyodbc
import sys

no_limit = False
if "NO_LIMIT" in sys.argv:
    no_limit = True

def get_all_run_numbers(cursor):
    if no_limit:
        query = """
        SELECT runnumber
        FROM datasets
        WHERE dsttype='DST_CALO_run2pp' AND dataset='ana446_2024p007'
        GROUP BY runnumber
        HAVING SUM(events) >= 1000000;
        """
    else:
        query = """
        SELECT runnumber
        FROM datasets
        WHERE dsttype='DST_CALO_run2pp' AND dataset='ana446_2024p007'
        GROUP BY runnumber
        HAVING SUM(events) >= 1000000 AND runnumber >= 47289;
        """
    cursor.execute(query)
    return [row.runnumber for row in cursor.fetchall()]

def get_golden_run_numbers(detector, runlist, cursor):
    query = f"""
    SELECT runnumber
    FROM goodruns
    WHERE ({detector}_auto).runclass='GOLDEN'
    """
    cursor.execute(query)
    golden = {row.runnumber for row in cursor.fetchall()}
    return golden.intersection(set(runlist))

def main():
    no_limit_mode = False
    if "NO_LIMIT" in sys.argv:
        no_limit_mode = True

    fc_conn = pyodbc.connect("DSN=FileCatalog;UID=phnxrc;READONLY=True")
    fc_cursor = fc_conn.cursor()
    all_runs = get_all_run_numbers(fc_cursor)
    all_runs.sort()
    with open('list/list_runnumber_all.txt', 'w') as f:
        for r in all_runs:
            f.write(f"{r}\n")
    print(f"TOTAL_RUNS:{len(all_runs)}")
    fc_conn.close()

    prod_conn = pyodbc.connect("DSN=Production_write")
    prod_cursor = prod_conn.cursor()

    detectors = ['emcal', 'ihcal', 'ohcal']
    golden_runs = set(all_runs)
    for d in detectors:
        golden_runs = golden_runs.intersection(get_golden_run_numbers(d, all_runs, prod_cursor))

    golden_runs = sorted(golden_runs)
    with open('list/Full_ppGoldenRunList.txt', 'w') as f:
        for r in golden_runs:
            f.write(f"{r}\n")
    print(f"COMBINED_GOLDEN_RUNS:{len(golden_runs)}")

    prod_conn.close()

if __name__ == "__main__":
    main()
EOF
    )

    # Execute the temporary Python script with or without NO_LIMIT argument
    if $NO_RUNNUMBER_LIMIT; then
        python_output=$(python3 <(echo "$python_script") NO_LIMIT)
    else
        python_output=$(python3 <(echo "$python_script"))
    fi

    echo "Python extraction completed."
    echo "----------------------------------------"

    total_runs=$(echo "$python_output" | grep 'TOTAL_RUNS' | cut -d':' -f2)
    combined_golden_runs=$(echo "$python_output" | grep 'COMBINED_GOLDEN_RUNS' | cut -d':' -f2)

    echo "Summary after initial extraction:"
    echo "Total initial runs: $total_runs"
    echo "Runs after Calo QA: $combined_golden_runs"
    echo "----------------------------------------"
}

validate_golden_list() {
    echo "Step 2: Validating golden run list..."
    golden_run_list="list/Full_ppGoldenRunList.txt"
    if [[ ! -f "$golden_run_list" ]]; then
        echo "[ERROR]: $golden_run_list not found."
        exit 1
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
    echo "Applying incremental cuts: runtime, livetime, and missing bad tower maps"
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
        query="SELECT runnumber, EXTRACT(EPOCH FROM (ertimestamp - brtimestamp)) AS duration FROM run WHERE runnumber = ${runnumber};"
        result=$(psql -h sphnxdaqdbreplica -d daq -t -c "$query" | tr -d '[:space:]')
        duration=$(echo "$result" | awk -F '|' '{print $2}')

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
        query="SELECT index, raw, live FROM gl1_scalers WHERE runnumber = ${runnumber} AND index = ${index_to_check};"
        result=$(psql -h sphnxdaqdbreplica -d daq -t -c "$query")

        index_pass=false
        while IFS='|' read -r index raw live; do
            index=$(echo "$index" | xargs)
            raw=$(echo "$raw" | xargs)
            live=$(echo "$live" | xargs)
            if [[ "$raw" =~ ^[0-9]+$ && "$live" =~ ^[0-9]+$ && "$raw" -ne 0 ]]; then
                ratio=$(echo "scale=2; $live / $raw * 100" | bc -l)
                if [[ "$index" -eq "$index_to_check" && $(echo "$ratio >= 80" | bc -l) -eq 1 ]]; then
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
    echo "Checking for runs missing bad tower maps..."

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

    rm list/available_bad_tower_runs.txt

    if $REMOVE_MISSING_MAPS; then
        echo "Removing runs missing bad tower maps..."
        grep -Fxf list/available_bad_tower_runs.txt "list/list_runnumber_livetime_v1.txt" > "$output_file_final_v1"
        echo "Removal complete."
        echo "----------------------------------------"
    fi

    cp "$output_file_final_v1" dst_list/Final_RunNumbers_After_All_Cuts.txt
    echo "Final run list stored in dst_list/Final_RunNumbers_After_All_Cuts.txt"
    echo "----------------------------------------"
}

create_list_file() {
    echo "Creating .list file..."
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
}

final_summary() {
    echo "========================================"
    echo "Final Summary (Version 1)"
    printf "%-50s | %-35s | %-25s\n" "Stage" ".evt File Events" "Runs"
    echo "--------------------------------------------------|-------------------------------------|-------------------------"

    # If NO_RUNNUMBER_LIMIT is true, note that the run number column represents a full run list.
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

    printf "%-50s | %-35s | %-35s\n" \
    "4) && MB livetime > 80%" \
    "${actual_events_after_livetime} (${percent_actual_events_after_livetime}%)" \
    "${total_runs_livetime_v1} (${percent_runs_after_livetime}%)"

    if $REMOVE_MISSING_MAPS; then
        step_label="5) && With Bad Tower Map Available"
    else
        step_label="5) Identified missing Bad Tower Maps (not removed)"
    fi

    printf "%-50s | %-35s | %-25s\n" \
    "$step_label" \
    "${actual_events_after_badtower} (${percent_actual_events_after_badtower}%)" \
    "${total_runs_after_badtower} (${percent_runs_after_badtower}%)"

    echo "================================================="
    echo "========================================"

    if $REMOVE_MISSING_MAPS; then
        echo "Final golden run list (missing maps removed): $workplace/dst_list/Final_RunNumbers_After_All_Cuts.txt"
    else
        echo "Final golden run list (with runs missing maps included): $workplace/dst_list/Final_RunNumbers_After_All_Cuts.txt"
    fi

    echo "Done."
}

########################################
# MAIN EXECUTION FLOW
########################################

echo "========================================"
echo "Starting the Golden Run List Generation"
echo "========================================"
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
