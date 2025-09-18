# Polarization performance
Beam polarizations in 2024 are plotted.

## Software version
- ana.504 or earlier
  - In ana.505, SpinDBContentv1 was introduced. Some modifications are neded in the analysis macro in the newer softwares, but the software version is not essential in this analysis as it just takes polarization and luminosity values from the spin database.
- Spin database: the default QA level is used. Accessed on Sep/11/2025.

## Runs
All 870 runs in [the golden run list ver2](https://github.com/sPHENIX-Collaboration/analysis/blob/master/runListGeneration/Run2024pp/Full_ppGoldenRunList_Version2.list).
Good spin runs in the golden runs are used.
No polarization selection, such as higher than 30%, is required.

## Analysis
Run-by-run polarization for each blue and yellow beam is filled into a histogram with weight of the run luminosity, which is obtained by SpinDBContent::GetScalerMbdNoCut.
The histograms are scaled with the total luminosity.
The average polarizations are:
- Blue: 51.9%
- Yellow: 52.6%

### Analysis code
`macro/polarization.cc`, and other codes in the directory.

## Results
See [plot](./results/polarization_internal.pdf)



