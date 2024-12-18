# SCorrelatorJetTree

A class to construct a tree of jets from a specified set of events within the sPHENIX software stack.  The produced trees will serve as input for the calculation of N-Point Energy Correlations.  Additional macros and scripts which may be useful can be found in `macros` and `scripts` respectively.

Initially derived from code by Antonio Silva (thanks!!)

## Usage

After copying the source files (in `src/`), compile the module using the `sphx-build` script in `src`:

```
./sphx-build
```

The class can then be added to and ran in a Fun4All macro, e.g.

```
root -b -q Fun4All_RunCorrelatorJetTree.C
```

---

### TODO Items:
  - [Major] Implement jet area calculation
