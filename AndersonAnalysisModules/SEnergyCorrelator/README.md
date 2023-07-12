# SEnergyCorrelator

A class to implement Peter Komiske's [Energy-Energy Correlator](https://github.com/pkomiske/EnergyEnergyCorrelators) library in the sPHENIX software stack. This class is designed in such a way that it can run in two modes: a **"standalone"** mode in which the class can be run in a ROOT macro on already produced [input](https://github.com/ruse-traveler/SCorrelatorJetTree), and a **"complex"** mode in which the class is run as a module on the sPHENIX node tree.

## Usage

After copying the source files (located in `src/`), compile the pacakge in the usual manner:

```
./autogen.sh --prefix=$MYINSTALL
make -j 4
make install
```

### Standalone Mode

The macros `DoStandaloneCorrelatorCalculation.C` and `Fun4All_DoStandaloneCalculation.C` illustrate how to use the module in standalone mode.  In the case of the former, the module is ran by itself in a ROOT macro.

```
root -b -q DoStandaloneCorrelatorCalculation.C
```

All of the relevant parameters such as input/output files, n-points, jet pT bins, etc. are set in the macro.  The script `DoStandaloneCorrelatorCalculation.sh` can be used to run this macro with fewer keystrokes.

The latter macro illustrates how to run the module in standalone mode as an "afterburner" to Fun4All.  First, Fun4All runs the `SCorrelatorJetTree` module over the specified input files to produce the input jet trees for `SEnergyCorrelator`.  Then the module is run over the output from Fun4All.  This macro is run, like always, with:

```
root -b -q Fun4All_DoStandaloneCalculation.C
```

---

## To-Do:

 - [Major] Add functionality to run multiple n-points per calculation
 - [Minor] Implement flag to turn constituent cuts on/off
 - [Minor] Implement flag to select returned errors on histograms (variance vs. sqrt(variance))
