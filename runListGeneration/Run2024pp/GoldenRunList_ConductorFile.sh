#!/bin/bash

##############################################################################################################################################################################
# Golden Run List Generation Script
#
# Purpose:
#   This script compiles a final list of "Golden Runs" for sPHENIX analysis. The selection criteria are:
#     • Run contains ≥1M events (and runnumber ≥47289 unless 'noRunNumberLimit' is specified).
#     • Calorimeter QA designates EMCal, IHCal, and OHCal as "Golden."
#     • Runtime exceeds 5 minutes.
#     • MBD (minimum bias) livetime is above 80%.
#     • (Optional) Runs missing bad tower maps can be removed if 'removeRunsWithMissingMaps' is given.
#     • (Optional) Runs without a magnet_on='t' entry in 'magnet_info' can be removed if 'removeNoMagnet' is specified.
#   After constructing this list, the script may also create DST .list files and examine which runs succeeded or failed in generating those lists.
#
# Main Steps:
#   1) FileCatalog extraction:
#       - Retrieve runs with ≥1M events.
#         - If 'noRunNumberLimit' is omitted, an additional runnumber ≥47289 criterion is applied.
#   2) Calo QA filtering:
#       - Intersect these runs with the Production_write database to ensure EMCal, IHCal, and OHCal each classify them as Golden.
#   3) Apply a >5-minute runtime cut.
#   4) Enforce an MBD livetime >80%.
#   5) Optionally remove runs lacking valid bad tower maps.
#   6) Optionally remove runs with no magnet_on='t' if 'removeNoMagnet' is specified.
#   7) Produce a final run list, optionally create DST .list files, and record which runs succeeded or failed to generate those lists.
#   8) If 'noRunNumberLimit' is used, a summary comparison is displayed between no-run-limit and run≥47289 scenarios.
#
# Usage:
#   ./GoldenRunList_ConductorFile.sh [removeRunsWithMissingMaps] [dontGenerateFileLists] [noRunNumberLimit] [removeNoMagnet]
#     - removeRunsWithMissingMaps : Exclude runs missing bad tower maps.
#     - dontGenerateFileLists     : Omit creation of DST .list files.
#     - noRunNumberLimit          : Omit the runnumber≥47289 cutoff.
#     - removeNoMagnet            : Exclude runs lacking magnet_on='t' in magnet_info.
#
# Outputs:
#   - Final run list: ../dst_list/Final_RunNumbers_After_All_Cuts.txt (plus an optional ge47289 variant).
#   - A local Full_ppGoldenRunList_Version1.list for DST creation (unless suppressed).
#   - Console summaries for each step, plus optional no-run-limit comparisons.
##############################################################################################################################################################################

########################################
# GLOBAL STYLES
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

# Prints an error message and exits. If 'dontGenerateFileLists' is set, it
# additionally clarifies that no DST lists would have been generated.
error_exit() {
    echo -e "${BOLD}${YELLOW}[ERROR]:${RESET} $1"
    if $DONT_GENERATE_FILELISTS; then
        echo "Note: 'dontGenerateFileLists' was provided, so no DST lists would have been generated."
    fi
    exit 1
}

# Parses command-line arguments to enable/disable specific features:
#   removeRunsWithMissingMaps, dontGenerateFileLists, noRunNumberLimit, removeNoMagnet.
parse_arguments() {
    REMOVE_MISSING_MAPS=false
    DONT_GENERATE_FILELISTS=false
    NO_RUNNUMBER_LIMIT=false
    REMOVE_NO_MAGNET=false

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
            removeNoMagnet)
                REMOVE_NO_MAGNET=true
                echo -e "${BOLD}${CYAN}Argument detected:${RESET} Removing runs with magnet_off (or missing)."
                ;;
        esac
    done

    if ! $REMOVE_MISSING_MAPS; then
        echo "No removal option detected: Missing-map runs will be kept."
    fi
    if ! $REMOVE_NO_MAGNET; then
        echo "No magnet check requested: Runs with magnet_off or missing are kept."
    fi
    echo "----------------------------------------"
}

# Removes pre-existing lists, FileLists, and old .list files in ../dst_list to
# ensure a clean start, then reports the cleanup results to the user.
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

# Creates or verifies the necessary directories: FileLists/, list/,
# and ../dst_list. Informs the user of the newly set up directories.
setup_directories() {
    echo -e "${BOLD}${GREEN}Setting up directories...${RESET}"
    base_path="${workplace}/.."
    mkdir -p FileLists/
    mkdir -p "${base_path}/dst_list"
    mkdir -p list/

    echo "Directories ready under ${base_path}/"
    echo "----------------------------------------"
}

# Captures the current working directory and displays it, storing the
# path in 'workplace' for subsequent reference.
set_workplace() {
    workplace=$(pwd)
    echo -e "${BOLD}Working directory:${RESET} $workplace"
    echo "----------------------------------------"
}

# (Step 1) Queries FileCatalog for runs with ≥1M events. If 'noRunNumberLimit'
# is disabled, also requires runnumber≥47289. Then intersects these runs with
# Production_write to keep only Golden EMCal/IHCal/OHCal runs, storing results locally.
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
    prod_cursor.execute(f"SELECT runnumber FROM goodruns WHERE ({d}_auto).runclass='GOLDEN'")
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

# (Step 2) Ensures that a valid 'Full_ppGoldenRunList.txt' was produced.
# If not found, abort execution since no runs qualified.
validate_golden_list() {
    echo -e "${BOLD}${MAGENTA}Step 2:${RESET} Validating golden run list..."
    if [[ ! -f "list/Full_ppGoldenRunList.txt" ]]; then
        error_exit "list/Full_ppGoldenRunList.txt not found. Possibly no runs qualify."
    fi
    echo "Golden run list found."
    echo "----------------------------------------"
}

# Summation function for events in .evt files. Given a file of runnumbers,
# this function batches them in groups of 100 for a SQL IN(...) query,
# queries the 'filelist' table, and accumulates the total event count.
get_actual_events_from_evt() {
    input_file=$1
    total_events=0
    batch_size=100
    run_numbers=()

    while IFS= read -r runnumber; do
        [[ -z "$runnumber" ]] && continue
        run_numbers+=("$runnumber")
        # Once we hit batch_size, we query in a single shot.
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

    # Handle leftover runs if any remain below the batch size.
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

# Prints a heading indicating that runtime, livetime, and missing bad tower maps
# will be enforced in the subsequent steps.
apply_incremental_cuts_header() {
    echo "----------------------------------------"
    echo -e "${BOLD}${MAGENTA}Applying incremental cuts:${RESET} runtime, livetime, and missing bad tower maps"
    echo "----------------------------------------"
}

# (Step 3) Reads 'Full_ppGoldenRunList.txt', queries the 'run' table to calculate
# (ertimestamp - brtimestamp). Retains runs with >300s (5 minutes).
# The result is placed in 'list_runnumber_runtime_v1.txt'.
# -- ADDED PROGRESS PRINTS EVERY 100 RUNS PROCESSED.
runtime_cut() {
    input_file="list/Full_ppGoldenRunList.txt"
    output_file_duration_v1="list/list_runnumber_runtime_v1.txt"
    > "$output_file_duration_v1"

    total_runs_duration_v1=0
    runs_dropped_runtime_v1=0

    # Count total for nice progress prints
    total_input_runs=$(wc -l < "$input_file")
    processed=0

    while IFS= read -r runnumber; do
        ((processed++))
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

        # Print progress every 100 runs
        if (( processed % 100 == 0 )); then
            echo "  [Runtime Cut] Processed $processed / $total_input_runs runs so far..."
        fi
    done < "$input_file"

    echo "After runtime cut (>5 mins): $total_runs_duration_v1 runs remain."
    echo "Dropped due to runtime: $runs_dropped_runtime_v1"
    echo "----------------------------------------"
}

# (Step 4) Reads the runtime-filtered list, queries 'gl1_scalers' for index=10
# (minimum bias), ensuring live/raw≥80%. Passing runs are written to
# 'list_runnumber_livetime_v1.txt'; failing runs go to 'list_runnumber_bad_livetime_v1.txt'.
# -- ADDED PROGRESS PRINTS EVERY 100 RUNS PROCESSED.
livetime_cut() {
    input_file="list/list_runnumber_runtime_v1.txt"
    output_file_livetime_v1="list/list_runnumber_livetime_v1.txt"
    bad_file_livetime_v1="list/list_runnumber_bad_livetime_v1.txt"
    > "$output_file_livetime_v1"
    > "$bad_file_livetime_v1"

    total_runs_livetime_v1=0
    runs_dropped_livetime_v1=0

    total_input_runs=$(wc -l < "$input_file")
    processed=0

    while IFS= read -r runnumber; do
        ((processed++))
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

        if (( processed % 100 == 0 )); then
            echo "  [Livetime Cut] Processed $processed / $total_input_runs runs so far..."
        fi
    done < "$input_file"

    echo "After livetime cut (>80%): $total_runs_livetime_v1 runs remain."
    echo "Dropped due to livetime: $runs_dropped_livetime_v1"
    echo "----------------------------------------"
}

# (Step 5) Reads from 'list_runnumber_livetime_v1.txt', producing a
# "preMagnet" list. If 'removeRunsWithMissingMaps' is set,
# any runs not found in the known bad tower maps are excluded.
missing_bad_tower_maps_step() {
    input_file="list/list_runnumber_livetime_v1.txt"
    pre_magnet_file="FileLists/Full_ppGoldenRunList_Version1_preMagnet.txt"
    cp "$input_file" "$pre_magnet_file"

    bad_tower_runs_file="list/list_runs_missing_bad_tower_maps.txt"
    available_bad_tower_runs=$(find /cvmfs/sphenix.sdcc.bnl.gov/calibrations/sphnxpro/cdb/CEMC_BadTowerMap \
                               -name "*p0*" | cut -d '-' -f2 | cut -dc -f1 | sort | uniq)
    echo "$available_bad_tower_runs" > list/available_bad_tower_runs.txt

    total_input_runs=$(wc -l < "$input_file")
    processed=0

    # Step 1: create an empty file for runs that are MISSING maps
    > "$bad_tower_runs_file"

    # Step 2: We'll store "available_bad_tower_runs.txt" lines in an array for quick membership checks
    mapfile -t available_map_array < list/available_bad_tower_runs.txt
    declare -A avail_map
    for runmap in "${available_map_array[@]}"; do
        avail_map["$runmap"]=1
    done

    # Step 3: read input_file line by line to see if run is in avail_map
    # if it's NOT, that means it is missing => put it in $bad_tower_runs_file
    while IFS= read -r runnumber; do
        ((processed++))
        [[ -z "$runnumber" ]] && continue

        if [[ -z "${avail_map[$runnumber]}" ]]; then
            # run is not in the available bad tower array => missing
            echo "$runnumber" >> "$bad_tower_runs_file"
        fi

        # Print progress every 100 runs
        if (( processed % 100 == 0 )); then
            echo "  [Bad Tower Step] Processed $processed / $total_input_runs runs so far..."
        fi
    done < "$input_file"

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
        grep -Fxf "$bad_tower_runs_file" -v "$pre_magnet_file" > tmp && mv tmp "$pre_magnet_file"
        echo "Removal complete."
        echo "----------------------------------------"
    fi

    cp "$pre_magnet_file" "${workplace}/../dst_list/Final_RunNumbers_After_All_Cuts.txt"
    echo "Final run list (pre-magnet step) stored in ${workplace}/../dst_list/Final_RunNumbers_After_All_Cuts.txt"
    echo "----------------------------------------"

    if $NO_RUNNUMBER_LIMIT; then
        awk '$1 >= 47289' "$pre_magnet_file" > FileLists/Full_ppGoldenRunList_ge47289_Version1.txt
        cp FileLists/Full_ppGoldenRunList_ge47289_Version1.txt \
           "${workplace}/../dst_list/Final_RunNumbers_After_All_Cuts_ge47289.txt"

        if $REMOVE_MISSING_MAPS; then
            missing_maps_ge47289=$(grep -Fxf FileLists/Full_ppGoldenRunList_ge47289_Version1.txt \
                                       "$bad_tower_runs_file" | wc -l)
            export missing_maps_ge47289
        fi
    fi
}

# (Step 6) If 'removeNoMagnet' is true, read from the preMagnet file
# and exclude runs where magnet_on != 't'. Otherwise, the preMagnet file
# is simply renamed to the final run list. We'll also add a small progress print.
magnet_check_step() {
    if [[ "$REMOVE_NO_MAGNET" != true ]]; then
        echo "No magnet check requested, skipping..."
        mv FileLists/Full_ppGoldenRunList_Version1_preMagnet.txt FileLists/Full_ppGoldenRunList_Version1.txt
        return
    fi

    echo "Step 6: Checking magnet_on from 'magnet_info' table..."

    pre_magnet_file="FileLists/Full_ppGoldenRunList_Version1_preMagnet.txt"
    if [[ ! -f "$pre_magnet_file" ]]; then
        echo "[ERROR] No 'preMagnet' file found: $pre_magnet_file"
        return
    fi

    magnet_off_file="list/list_runs_no_magnet.txt"
    > "$magnet_off_file"

    final_list_magnet="FileLists/Full_ppGoldenRunList_Version1.txt"
    > "$final_list_magnet"

    total_runs_magnet_ok=0
    runs_dropped_magnet=0

    total_input_runs=$(wc -l < "$pre_magnet_file")
    processed=0

    while IFS= read -r runnumber; do
        ((processed++))
        [[ -z "$runnumber" ]] && continue

        query="SELECT magnet_on
               FROM magnet_info
               WHERE runnumber=${runnumber};"
        result=$(psql -h sphnxdaqdbreplica -d daq -t -c "$query" | tr -d '[:space:]')

        if [[ "$result" == "t" ]]; then
            echo "$runnumber" >> "$final_list_magnet"
            (( total_runs_magnet_ok++ ))
        else
            echo "$runnumber" >> "$magnet_off_file"
            (( runs_dropped_magnet++ ))
        fi

        if (( processed % 100 == 0 )); then
            echo "  [Magnet Check] Processed $processed / $total_input_runs runs so far..."
        fi
    done < "$pre_magnet_file"

    echo "Magnet On check: $total_runs_magnet_ok runs are ON"
    echo "Dropped (magnet off or missing): $runs_dropped_magnet"
    echo "List of dropped runs: $magnet_off_file"
    echo "----------------------------------------"

    # Overwrite final list with magnet-checked version
    cp "$final_list_magnet" "${workplace}/../dst_list/Final_RunNumbers_After_All_Cuts.txt"

    export total_runs_magnet_off=$runs_dropped_magnet
}

# Generates Full_ppGoldenRunList_Version1.list from the final text file
# in FileLists/. If removeNoMagnet is false, the script may rename the
# preMagnet file to that final name. Issues a warning if the final file
# is missing.
create_list_file() {
    echo "Creating final .list file from the final run list..."

    if [[ ! -f "FileLists/Full_ppGoldenRunList_Version1.txt" ]]; then
        if [[ -f "FileLists/Full_ppGoldenRunList_Version1_preMagnet.txt" ]]; then
            mv FileLists/Full_ppGoldenRunList_Version1_preMagnet.txt FileLists/Full_ppGoldenRunList_Version1.txt
        fi
    fi

    if [[ -f "FileLists/Full_ppGoldenRunList_Version1.txt" ]]; then
        cp "FileLists/Full_ppGoldenRunList_Version1.txt" Full_ppGoldenRunList_Version1.list
        echo ".list file created: Full_ppGoldenRunList_Version1.list"
    else
        echo "[WARNING] Could not find 'FileLists/Full_ppGoldenRunList_Version1.txt' to create .list!"
    fi
    echo "----------------------------------------"
}

# Removes pre-existing .list files in ../dst_list/ to avoid confusion
# prior to generating new DST lists.
clean_old_dst_lists() {
    echo "Removing any old DST lists from the parent 'dst_list' directory..."
    rm -f "${workplace}/../dst_list/"*.list
    echo "Done removing old .list files in dst_list/."
    echo "----------------------------------------"
}

# If 'dontGenerateFileLists' is not specified, runs CreateDstList.pl on
# the final .list to build run-specific .list files for DST creation.
# Prints where the DST lists are saved.
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

# After DST .list files are generated, this function checks which runs
# successfully produced a dst_calo_run2pp-*.list and which did not.
# Successful runs are recorded in list_runnumber_createDstSuccess.txt,
# while failures appear in list_runnumber_createDstFailure.txt.
apply_createDstList_cut() {
    echo "Collecting CreateDST File List success/failure for the main scenario..."

    if $DONT_GENERATE_FILELISTS; then
        echo "[INFO]: 'dontGenerateFileLists' was provided, so no DST creation was done for main scenario."
        export total_runs_createDst_success=0
        export runs_dropped_createDst=0
        export actual_events_after_createDst=0
        export total_runs_after_createDst=0
        return
    fi

    final_stage4_file="FileLists/Full_ppGoldenRunList_Version1.txt"
    if [[ ! -f "$final_stage4_file" ]]; then
        echo "[ERROR]: Cannot find final stage file: $final_stage4_file"
        export total_runs_createDst_success=0
        export runs_dropped_createDst=0
        export actual_events_after_createDst=0
        export total_runs_after_createDst=0
        return
    fi

    success_file="list/list_runnumber_createDstSuccess.txt"
    failure_file="list/list_runnumber_createDstFailure.txt"
    > "$success_file"
    > "$failure_file"

    mapfile -t final_stage_runs < "$final_stage4_file"

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

    for runnumber in "${final_stage_runs[@]}"; do
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

    # If noRunNumberLimit is set, repeat the same success/failure process
    # for the run≥47289 scenario.
    if $NO_RUNNUMBER_LIMIT; then
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

            mapfile -t final_stage_runs_ge47289 < "$final_stage4_file_ge47289"

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

            for runnumber in "${final_stage_runs_ge47289[@]}"; do
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

# Collates event/run tallies at each stage. The stages are:
#   1) list_runnumber_all.txt               -> initial
#   2) Full_ppGoldenRunList.txt             -> calo QA
#   3) list_runnumber_runtime_v1.txt        -> runtime
#   4) list_runnumber_livetime_v1.txt       -> livetime
#   5) Full_ppGoldenRunList_Version1_preMagnet.txt -> optional map check
#   6) Full_ppGoldenRunList_Version1.txt    -> optional magnet check
#   7) Full_ppGoldenRunList_Version1_DSTsuccess.txt -> DST creation success
# Compares these runs to a "Stage1" reference for run-based percentages and
# to the Stage1 event count for event-based percentages.
compute_event_counts() {
    pre_magnet_file="FileLists/Full_ppGoldenRunList_Version1_preMagnet.txt"
    final_file="FileLists/Full_ppGoldenRunList_Version1.txt"

    # Stage1 => list/list_runnumber_all.txt
    actual_events_before_cuts=$(get_actual_events_from_evt 'list/list_runnumber_all.txt')
    total_runs_before_cuts=$(wc -l < 'list/list_runnumber_all.txt')
    actual_events_initial=$actual_events_before_cuts

    # Stage2 => list/Full_ppGoldenRunList.txt
    actual_events_calo_qa=$(get_actual_events_from_evt 'list/Full_ppGoldenRunList.txt')
    runs_after_calo_qa=$(wc -l < 'list/Full_ppGoldenRunList.txt')

    # Stage3 => runtime
    actual_events_after_runtime=$(get_actual_events_from_evt 'list/list_runnumber_runtime_v1.txt')
    runs_after_runtime=$(wc -l < 'list/list_runnumber_runtime_v1.txt')

    # Stage4 => livetime
    actual_events_after_livetime=$(get_actual_events_from_evt 'list/list_runnumber_livetime_v1.txt')
    runs_after_livetime=$(wc -l < 'list/list_runnumber_livetime_v1.txt')

    # Stage5 => preMagnet
    if [[ -f "$pre_magnet_file" ]]; then
        actual_events_after_badtower=$(get_actual_events_from_evt "$pre_magnet_file")
        runs_after_badtower=$(wc -l < "$pre_magnet_file")
    else
        actual_events_after_badtower=0
        runs_after_badtower=0
    fi

    # Stage6 => final
    if [[ -f "$final_file" ]]; then
        actual_events_after_magnet=$(get_actual_events_from_evt "$final_file")
        runs_after_magnet=$(wc -l < "$final_file")
    else
        actual_events_after_magnet=0
        runs_after_magnet=0
    fi

    # Stage7 => DST creation success
    if [[ -f FileLists/Full_ppGoldenRunList_Version1_DSTsuccess.txt ]]; then
        actual_events_after_createDst=$(get_actual_events_from_evt "FileLists/Full_ppGoldenRunList_Version1_DSTsuccess.txt")
        runs_after_createDst=$(wc -l < "FileLists/Full_ppGoldenRunList_Version1_DSTsuccess.txt")
    else
        actual_events_after_createDst=0
        runs_after_createDst=0
    fi

    STAGE1_RUNS=$total_runs_before_cuts
    [[ "$STAGE1_RUNS" -eq 0 ]] && STAGE1_RUNS=1
    [[ "$actual_events_before_cuts" -eq 0 ]] && actual_events_before_cuts=1

    # run-based percentages
    percent_runs_calo_qa=$(echo "scale=2; 100.0*$runs_after_calo_qa/$STAGE1_RUNS" | bc)
    percent_runs_after_runtime=$(echo "scale=2; 100.0*$runs_after_runtime/$STAGE1_RUNS" | bc)
    percent_runs_after_livetime=$(echo "scale=2; 100.0*$runs_after_livetime/$STAGE1_RUNS" | bc)
    percent_runs_after_badtower=$(echo "scale=2; 100.0*$runs_after_badtower/$STAGE1_RUNS" | bc)
    percent_runs_after_magnet=$(echo "scale=2; 100.0*$runs_after_magnet/$STAGE1_RUNS" | bc)
    percent_runs_after_createDst=$(echo "scale=2; 100.0*$runs_after_createDst/$STAGE1_RUNS" | bc)

    # event-based percentages
    percent_actual_events_calo_qa=$(echo "scale=2; 100.0*$actual_events_calo_qa/$actual_events_before_cuts" | bc)
    percent_actual_events_after_runtime=$(echo "scale=2; 100.0*$actual_events_after_runtime/$actual_events_before_cuts" | bc)
    percent_actual_events_after_livetime=$(echo "scale=2; 100.0*$actual_events_after_livetime/$actual_events_before_cuts" | bc)
    percent_actual_events_after_badtower=$(echo "scale=2; 100.0*$actual_events_after_badtower/$actual_events_before_cuts" | bc)
    percent_actual_events_after_magnet=$(echo "scale=2; 100.0*$actual_events_after_magnet/$actual_events_before_cuts" | bc)
    percent_events_after_createDst=$(echo "scale=2; 100.0*$actual_events_after_createDst/$actual_events_before_cuts" | bc)

    export STAGE1_RUNS
    export runs_after_calo_qa
    export runs_after_runtime
    export runs_after_livetime
    export runs_after_badtower
    export runs_after_magnet
    export runs_after_createDst

    export actual_events_before_cuts
    export total_runs_before_cuts
    export actual_events_initial

    export actual_events_calo_qa
    export actual_events_after_runtime
    export actual_events_after_livetime
    export actual_events_after_badtower
    export actual_events_after_magnet
    export actual_events_after_createDst

    export percent_runs_calo_qa
    export percent_runs_after_runtime
    export percent_runs_after_livetime
    export percent_runs_after_badtower
    export percent_runs_after_magnet
    export percent_runs_after_createDst

    export percent_actual_events_calo_qa
    export percent_actual_events_after_runtime
    export percent_actual_events_after_livetime
    export percent_actual_events_after_badtower
    export percent_actual_events_after_magnet
    export percent_events_after_createDst
}

# Displays a multi-stage summary table showing how many runs and events remain
# after each cut. If 'noRunNumberLimit' is active, the script also prints
# a comparison between the full run range and the run≥47289 scenario.
final_summary() {
    echo -e "${BOLD}${MAGENTA}========================================${RESET}"
    echo -e "${BOLD}${MAGENTA}Final Summary (Version 1)${RESET}"
    echo -e "${MAGENTA}----------------------------------------${RESET}"
    printf "%-50s | %-35s | %-25s\n" "Stage" ".evt File Events" "Runs"
    echo "--------------------------------------------------|-------------------------------------|-------------------------"

    if $NO_RUNNUMBER_LIMIT; then
        stage1_label="≥1M events"
    else
        stage1_label="≥47289 & ≥1M events"
    fi

    # Stage 1
    printf "%-50s | %-35s | %-25s\n" \
    "1) $stage1_label" \
    "${actual_events_initial} (100%)" \
    "${STAGE1_RUNS} (100%)"

    # Stage 2
    printf "%-50s | %-35s | %-25s\n" \
    "2) && Golden EMCal/HCal" \
    "${actual_events_calo_qa} (${percent_actual_events_calo_qa}%)" \
    "${runs_after_calo_qa} (${percent_runs_calo_qa}%)"

    # Stage 3
    printf "%-50s | %-35s | %-25s\n" \
    "3) && > 5 minutes" \
    "${actual_events_after_runtime} (${percent_actual_events_after_runtime}%)" \
    "${runs_after_runtime} (${percent_runs_after_runtime}%)"

    # Stage 4
    printf "%-50s | %-35s | %-25s\n" \
    "4) && MB livetime > 80%" \
    "${actual_events_after_livetime} (${percent_actual_events_after_livetime}%)" \
    "${runs_after_livetime} (${percent_runs_after_livetime}%)"

    # Stage 5
    if $REMOVE_MISSING_MAPS; then
        step_label="5) && With Bad Tower Map Available (Removed)"
    else
        step_label="5) && With Bad Tower Map Available (Not Removed)"
    fi
    printf "%-50s | %-35s | %-25s\n" \
    "$step_label" \
    "${actual_events_after_badtower} (${percent_actual_events_after_badtower}%)" \
    "${runs_after_badtower} (${percent_runs_after_badtower}%)"

    # Stage 6 - magnet
    if [[ "$REMOVE_NO_MAGNET" == true ]]; then
        step_label_magnet="6) && Magnet On"
        printf "%-50s | %-35s | %-25s\n" \
        "$step_label_magnet" \
        "${actual_events_after_magnet} (${percent_actual_events_after_magnet}%)" \
        "${runs_after_magnet} (${percent_runs_after_magnet}%)"
    fi

    # Stage 7 - DST creation
    step_label_createDST="7) && CreateDST File List Successes"
    printf "%-50s | %-35s | %-25s\n" \
    "$step_label_createDST" \
    "${actual_events_after_createDst} (${percent_events_after_createDst}%)" \
    "${runs_after_createDst} (${percent_runs_after_createDst}%)"

    # Missing map note
    if $REMOVE_MISSING_MAPS; then
        map_note="Removed from final DST"
    else
        map_note="Kept in final DST"
    fi
    printf "%-50s | %-35s | %-25s\n" \
    "Missing Map Runs" \
    "-" \
    "${total_runs_missing_bad_tower} (${map_note})"

    # Magnet note
    if [[ "$REMOVE_NO_MAGNET" == true ]]; then
        if [[ -z "$total_runs_magnet_off" ]]; then
            total_runs_magnet_off=0
        fi
        mag_note="Removed"
        printf "%-50s | %-35s | %-25s\n" \
        "No Magnet-On Runs" \
        "-" \
        "${total_runs_magnet_off} (${mag_note})"
    fi

    echo "================================================="
    echo "========================================"

    # Conclude with the final run list path
    if $REMOVE_MISSING_MAPS; then
        echo "Final golden run list (missing maps removed):"
    else
        echo "Final golden run list (with runs missing maps included):"
    fi
    echo " --> ${workplace}/../dst_list/Final_RunNumbers_After_All_Cuts.txt"
    echo "Done."

    # If noRunNumberLimit => additional table comparing full range vs run≥47289
    if $NO_RUNNUMBER_LIMIT; then
        echo ""
        echo "----------------------------------------"
        echo -e "${BOLD}${MAGENTA}Additional Comparison (Before & After Cuts):${RESET} No-limit vs. ≥47289 scenario"
        echo ""
        cat <<EOCOMPARISON

Metric                                  | NoLimit(All)                | ≥47289                    | ΔValue           | %Lost
--------------------------------------------------------------------------------------------------------------
EOCOMPARISON

        ev_all_initial=${actual_events_before_cuts:-0}
        ev_ge_initial=${actual_events_before_cuts_ge47289:-0}
        diff_ev_initial=$(echo "$ev_all_initial - $ev_ge_initial" | bc)
        if (( ev_all_initial > 0 )); then
            perc_ev_lost_initial=$(echo "scale=2; 100.0*$diff_ev_initial/$ev_all_initial" | bc)
        else
            perc_ev_lost_initial=0
        fi

        run_all_initial=${total_runs_before_cuts:-0}
        run_ge_initial=${total_runs_before_cuts_ge47289:-0}
        diff_run_initial=$(echo "$run_all_initial - $run_ge_initial" | bc)
        if (( run_all_initial > 0 )); then
            perc_run_lost_initial=$(echo "scale=2; 100.0*$diff_run_initial/$run_all_initial" | bc)
        else
            perc_run_lost_initial=0
        fi

        printf "%-40s | %-20s | %-20s | %-20s | %-20s\n" \
          "Events Before All Cuts" "$ev_all_initial" "$ev_ge_initial" "$diff_ev_initial" "${perc_ev_lost_initial}%"
        printf "%-40s | %-20s | %-20s | %-20s | %-20s\n" \
          "Runs Before All Cuts"   "$run_all_initial" "$run_ge_initial" "$diff_run_initial" "${perc_run_lost_initial}%"

        echo ""
        # After All Cuts => DST
        ev_all=${actual_events_after_createDst:-0}
        ev_ge=${actual_events_after_createDst_ge47289:-0}
        diff_ev=$(echo "$ev_all - $ev_ge" | bc)
        if (( ev_all > 0 )); then
            perc_ev_lost=$(echo "scale=2; 100.0*$diff_ev/$ev_all" | bc)
        else
            perc_ev_lost=0
        fi

        run_all=${runs_after_createDst:-0}
        run_ge=${runs_after_createDst_ge47289:-0}
        diff_run=$(echo "$run_all - $run_ge" | bc)
        if (( run_all > 0 )); then
            perc_run_lost=$(echo "scale=2; 100.0*$diff_run/$run_all" | bc)
        else
            perc_run_lost=0
        fi

        printf "%-40s | %-20s | %-20s | %-20s | %-20s\n" \
          "Events After All Cuts" "$ev_all" "$ev_ge" "$diff_ev" "${perc_ev_lost}%"
        printf "%-40s | %-20s | %-20s | %-20s | %-20s\n" \
          "Runs After All Cuts"   "$run_all" "$run_ge" "$diff_run" "${perc_run_lost}%"

        echo ""
        echo "≥47289: DST successes = ${runs_after_createDst_ge47289:-0}, events = ${actual_events_after_createDst_ge47289:-0}"

        if $REMOVE_MISSING_MAPS; then
            missing_maps_all=${total_runs_missing_bad_tower:-0}
            missing_maps_ge47289=${missing_maps_ge47289:-0}

            diff_maps=$(echo "$missing_maps_all - $missing_maps_ge47289" | bc)
            if (( missing_maps_all > 0 )); then
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
# MAIN EXECUTION
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

magnet_check_step

create_list_file
clean_old_dst_lists

if ! $DONT_GENERATE_FILELISTS; then
    generate_dst_lists
fi

apply_createDstList_cut
compute_event_counts
final_summary
