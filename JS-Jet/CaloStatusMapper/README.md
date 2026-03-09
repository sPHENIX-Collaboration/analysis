# Calo Status Mapper

### Author:
Derek Anderson

### Description:
A Fun4All QA module to plot the no. of towers as a function of
their eta, phi indices with a given status (good, hot, etc.).
Also tracks the average number of towers with a given status
per event. The macro `Fun4All_TestCaloStatusMapper.C` illustrates
basic usage of the module.

The module can be pointed at any number of nodes holding `TowerInfo`
objects. For example, if the EMCal has been retowered, then one
could add it to the list of nodes to histogram via:


```
  cfg_mapper.inNodeNames.push_back(
    {"TOWERINFO_CALIB_CEMC_RETOWER", CaloStatusMapperDefs::Calo::HCal}
  );
```

The `CaloStatusMapperDefs::Calo::HCal` indicates what geometry to
use (i.e. what range of eta/phi indices to expect). At the moment,
only the EMCal and I/OHCal geometries are supported.
