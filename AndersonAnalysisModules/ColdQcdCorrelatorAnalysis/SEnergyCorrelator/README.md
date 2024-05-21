# SEnergyCorrelator

A module to compute N-Point Energy-Energy Correlators (ENC) in the SPHENIX software stack. Calclations can be done manually or via Patrick
Komiske's [Energy-Energy Correlator](https://github.com/pkomiske/EnergyEnergyCorrelators).

## Usage

After copying the source files (located in `src/`), compile the module with the `sphx-build` script in `src`:

```
./sphx-build
```


The macros `DoStandaloneCorrelatorCalculation.cxx` and `macros/Fun4All_DoStandaloneCalculation.C` illustrate how to use the module in standalone mode
(i.e. *after* running Fun4All).  In the case of the former, the module is ran by itself in a ROOT macro.

```
root -b -q DoStandaloneCorrelatorCalculation.cxx
```

All of the relevant parameters such as input/output files, n-points, jet pT bins, etc. are set in `EnergyCorrelatorOptions.h`  The script
`DoStandaloneCorrelatorCalculation.rb` can be used to run this macro with fewer keystrokes.

The latter macro illustrates how to run the module in standalone mode as an "afterburner" to Fun4All.  First, Fun4All runs the `SCorrelatorJetTreeMaker`
module over the specified input files to produce the input jet trees for `SEnergyCorrelator`.  Then the module is run over the output from Fun4All. 
This macro is run, like always, with:

```
root -b -q Fun4All_DoStandaloneCalculation.C
```

---

## To-Do:

- [Major] Add functionality to run multiple n-points per calculation
- [Minor] Update afterburner example macro
