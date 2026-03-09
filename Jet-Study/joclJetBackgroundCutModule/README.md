This module checks a set of cuts on jets to improve the quality of the sample (reject fake jets).

Parameters for instantiation are:
jet node name (required)
module name (has default ")
do debug (default to 0; increase for more verbosity)
do abort (default to false, does not abort events which fail cuts)

If do abort is false, the module will add intflags to recoconsts named:
failsLoEmJetCut
failsHiEmJetCut
failsIhJetCut
failsAnyJetCut (OR of the other three)

required nodes:
one of either GlobalVertexMapv1 (named GlobalVertexMap) or MbdVertexMapv1 (named MbdVertexMap)
TowerInfoContainer of retowered EMCal towers named "TOWERINFO_CALIB_CEMC_RETOWER"
TowerInfoContainer of OHCal towers named "TOWERINFO_CALIB_HCALOUT"
RawTowerGeomContainer of IHCal towers named "TOWERGEOM_HCALIN"
RawTowerGeomContainer of OHCal towers named "TOWERGEOM_HCALOUT"
JetContainerv1 with jet node name (see above)

These required nodes may be changed in the future for more generality