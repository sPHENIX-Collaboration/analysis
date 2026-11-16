#!/bin/bash
cp INTT_Calo_trkUser_example1.cpp ../INTT_Calo_trkUser.cpp
cp INTT_Calo_trkUser_example1.h ../INTT_Calo_trkUser.h
cd ..
root -b -q INTT_Calo_Ana.cpp\(\"run2pp_2/53876/ana_53876_00000.root\",1\,\"example1/INTT_Calo_example1_data.root\",-0.018,0.1259,0,1000000\)
