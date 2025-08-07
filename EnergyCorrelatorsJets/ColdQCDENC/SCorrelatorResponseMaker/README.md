# SCorrelatorResponseMaker [Under Construction]

A class to match truth to reconstructed jets and their constituents. This produces a "response tree"
which maps the truth jets/constituents onto their reconstructed counterparts. This tree is then used
by the "SCorrelatorFolder" class to derive corrections for the reconstructed n-point energy correlation
strength functions.

## Usage

After copying the source files (located in `src/`), compile the module with the `sphx-build` script
in `src`:

```
./sphx-build
```

The macro `MakeCorrelatorResponseTree.cxx` illustrate how to use the module.

```
root -b -q MakeCorrelatorResponseTree.cxx
```

The script `MakeCorrelatorResponseTree.rb` can be used to run this macro with fewer keystrokes.

All of the relevant parameters such as input/output files, n-points, jet pT bins, etc. are set in `EnergyCorrelatorOptions.h`  The script
`DoStandaloneCorrelatorCalculation.rb` can be used to run this macro with fewer keystrokes.
