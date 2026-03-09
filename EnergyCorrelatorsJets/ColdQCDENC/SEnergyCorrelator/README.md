# SEnergyCorrelator

A module to compute N-Point Energy-Energy Correlators (ENC) in the SPHENIX software stack. Calclations can be done manually or via Patrick
Komiske's [Energy-Energy Correlator](https://github.com/pkomiske/EnergyEnergyCorrelators).

## Usage

After copying the source files (located in `src/`), compile the module with the `sphx-build` script in `src`:

```
./sphx-build
```

The macros `DoStandaloneCorrelatorCalculation.cxx` illustrate how to use the module.

```
root -b -q RunENCCalculation.cxx
```

All of the relevant parameters such as input/output files, n-points, jet pT bins, etc. are set in `EnergyCorrelatorOptions.h`  The script
`RunENCCalculation.rb` can be used to run this macro with fewer keystrokes.

---

## To-Do:

- [Major] Add functionality to run multiple n-points per calculation
