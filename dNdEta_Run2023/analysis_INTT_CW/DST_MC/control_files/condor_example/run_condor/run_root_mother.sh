#!/bin/bash                                                                                                                                                                                                                                                                   
source /opt/sphenix/core/bin/sphenix_setup.sh -n ana.388
source /opt/sphenix/core/bin/setup_local.sh /sphenix/user/ChengWei/sPH_dNdeta/build_folder/build_HR_ntuple/install

i=the_file_number

root.exe -l -b -q /sphenix/user/ChengWei/sPH_dNdeta/dNdEta_Run2023/macros/Fun4All_G4_sPHENIX.C\(0,0,\"/sphenix/user/ChengWei/sPH_dNdeta/dNdEta_INTT_MC_388_000/INTTRecoClusters_test_400_$i.root\",\"/sphenix/user/ChengWei/sPH_dNdeta/dNdEta_Run2023/macros/list/dNdEta_INTT/list_folder_388_000/dst_INTTdNdEta_400_$i.list\",0\)
