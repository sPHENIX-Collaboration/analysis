=================================================================
                       Streaks Identifier Module
=================================================================

===========
Building
==========
cd StreakEventsIdentifier
mkdir build && cd build
../autogen.sh --prefix=$MYINSTALL
make -j4
make install

===========
Input list
==========
dst_jetcalo.list
dst_jet.list

=============
Macro Usage:
=============
root -l -b -q 'Fun4All_RunStreakFromDST.C("dst_jetcalo_47289.list","dst_jet_47289.list", 1000, "test_output", 10.0, 0.7, 10.0, 5.0, 0.5, "", false, -10.0, 10.0, "24,25,26,27,36,37,38", true)' 2>&1 | tee test.log


==========================
Streaks Selection Criteria
==========================
1- No back-to-back jets
2- Shower-shape cut

Triggers used: 24,25,26,27,36,37,38 

Cuts: 
Minimum cluster transverse energy (Et_min)                           >> 10.0 GeV
Minimum cluster width in eta (weta_min)                              >> 0.7 
Minimum tower energy for shape calculations (min_tower_E_for_shapes) >> 0.07 GeV


