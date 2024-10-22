# SCorrelatorUtilities 

Collected here useful types, methods, and constant used throughout the sPHENIX Cold QCD Energy-Energy
Correlator analysis. There are four components:

  - `Constants::`: enums and methods used to consolidate important constants as well as mappings between
     various flags, nodes, etc.;
  - `Types::`: classes used to consolidate relevant information and operations on said information from things
     like tracks, calorimeter clusters, jets, etc.;
  - `Tools::`: methods defining frequently used operations and calculations; and
  - `Interfaces::`: methods which streamline interfacing with Fun4All, ROOT, and potentially other frameworks.

Each component has an "all-in-one" header which collects all of the relevant constituent headers for convenience.
For example, to use a data type in another module make sure to include the following with your other includes:

```
// includes
#include "<path-to-install>/scorrelatorutilities/Types.h"

// and where you load libraries
R__LOAD_LIBRARY(<path-to-install>/libscorrelatorutilities.so)
```

## Compilation Instructions

As this is used throughout the entire analysis chain, this library should be built first. As always, build with:

```
cd src
mkdir build
./sphx-build
```
