#!/usr/bin/env bash
set -euo pipefail

# 1) Read runs from runlist.txt (one run per line, integers only)
echo " "
echo "Starting do_cosmics_calibration.sh"

if [[ ! -s runlist.txt ]]; then
  echo "ERROR: runlist.txt missing or empty" >&2
  exit 1
fi

mapfile -t run_array < <(awk 'NF && $1 ~ /^[0-9]+$/ {print $1}' runlist.txt)

if (( ${#run_array[@]} == 0 )); then
  echo "ERROR: no valid run numbers in runlist.txt" >&2
  exit 1
fi

min_run=${run_array[0]}
max_run=${run_array[0]}
for r in "${run_array[@]}"; do
  (( r < min_run )) && min_run=$r
  (( r > max_run )) && max_run=$r
done

echo "Run range: ${min_run} - ${max_run}"

mkdir -p hist
mkdir -p output

ohcal_partial=()
ihcal_partial=()

# 2) Per-run hadd for OHCal and IHCal
echo " "
echo "1) Processing hadd for production results:"
echo " "
for run in "${run_array[@]}"; do

  ########## OHCal: HIST_COSMIC_HCALOUT ##########
  ohcal_out="hist/ohcal_hist_${run}.root"

  if [[ -f "${ohcal_out}" ]]; then
    echo "    Skipping OHCal hadd run ${run} - output already exists"
    ohcal_partial+=("${ohcal_out}")
  else
    ohcal_files=$(
      psql -d FileCatalog -X -A -t -P pager=off -v ON_ERROR_STOP=1 -c "
        SELECT coalesce(string_agg(f.full_file_path, ' '), '')
        FROM public.datasets d
        JOIN public.files   f ON f.lfn = d.filename
        WHERE d.dsttype  = 'HIST_COSMIC_HCALOUT'
          AND d.runnumber = ${run};
      "
    )

    if [[ -n "${ohcal_files// }" ]]; then
      echo "    Processing OHCal hadd run ${run}"
      hadd -j -k "${ohcal_out}" ${ohcal_files} >/dev/null
      ohcal_partial+=("${ohcal_out}")
    else
      echo "    No OHCal files for run ${run}"
    fi
  fi

  ########## IHCal: HIST_COSMIC_HCALIN ##########
  ihcal_out="hist/ihcal_hist_${run}.root"

  if [[ -f "${ihcal_out}" ]]; then
    echo "    Skipping IHCal hadd run ${run} - output already exists"
    ihcal_partial+=("${ihcal_out}")
  else
    ihcal_files=$(
      psql -d FileCatalog -X -A -t -P pager=off -v ON_ERROR_STOP=1 -c "
        SELECT coalesce(string_agg(f.full_file_path, ' '), '')
        FROM public.datasets d
        JOIN public.files   f ON f.lfn = d.filename
        WHERE d.dsttype  = 'HIST_COSMIC_HCALIN'
          AND d.runnumber = ${run};
      "
    )

    if [[ -n "${ihcal_files// }" ]]; then
      echo "    Processing IHCal hadd run ${run}"
      hadd -j -k "${ihcal_out}" ${ihcal_files} >/dev/null
      ihcal_partial+=("${ihcal_out}")
    else
      echo "No IHCal files for run ${run}"
    fi
  fi

done

# 3) Final hadd across per-run ROOTs
# Name final outputs with the min and max run numbers
echo " "
if (( ${#ohcal_partial[@]} > 0 )); then
  echo "Processing final OHCal hadd"
  ohcal_final="hist/ohcal_hist_${min_run}_${max_run}.root"
  hadd -j -k -f "${ohcal_final}" "${ohcal_partial[@]}" >/dev/null
  echo "Final OHCal hadd output: ${ohcal_final}"
else
  echo "No OHCal files processed. Done."
fi

if (( ${#ihcal_partial[@]} > 0 )); then
  echo "Processing final IHCal hadd"
  ihcal_final="hist/ihcal_hist_${min_run}_${max_run}.root"
  hadd -j -k -f "${ihcal_final}" "${ihcal_partial[@]}" >/dev/null
  echo "Final IHCal hadd output: ${ihcal_final}"
else
  echo "No IHCal files processed. Done."
fi

# 4) Run function_fitting.C for ihcal and ohcal
echo " "
echo "2) Running function_fitting.C for IHCal and OHCal to get MPV values"
root -l -q -b "script/function_fitting.C(${min_run},${max_run},\"ihcal\")"
root -l -q -b "script/function_fitting.C(${min_run},${max_run},\"ohcal\")"

# 3) Run readhcalcalib.C (no arguments)
echo " "
echo "3) Running readhcalcalib.C to read pre-calibration constants (Need already prepared ohcal_precalib.root and ihcal_precalib.root from CDB)"
root -l -q -b 'script/readhcalcalib.C'

# 4) Run get_calibration_factor.C(min_run, max_run)
echo " "
echo "4) Running get_calibration_factor.C to compute new calibration factors"
root -l -q -b "script/get_calibration_factor.C(${min_run},${max_run})"
echo "Done. New calibration factor files have been saved to output/"

# 5) Run get_CDBTree.C(min_run, max_run)
echo " "
echo "5) Running get_CDBTree.C to form CDB Trees with new calibration factors"
root -l -q -b "script/get_CDBTree.C(${min_run},${max_run})"
echo "Done. Calibration factor files and CDB TTrees have been saved to output/"