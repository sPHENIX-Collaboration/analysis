#!/bin/bash
cp INTT_Calo_trkUser_example4.cpp ../INTT_Calo_trkUser.cpp
cp INTT_Calo_trkUser_example4.h ../INTT_Calo_trkUser.h
cd ..
root -b -q INTT_Calo_Ana.cpp\(\"run2pp_2/53876/ana_53876_00000.root\",1\,\"example4/INTT_Calo_example4_data.root\",0.,0.,0,1000000\)
