#!/bin/bash
cp INTT_Calo_trkUser_example5.cpp ../INTT_Calo_trkUser.cpp
cp INTT_Calo_trkUser_example5.h ../INTT_Calo_trkUser.h
cd ..
root -b -q INTT_Calo_Ana.cpp\(\"run2pp_2/53876/ana_53876_00000.root\",1\,\"example5/INTT_Calo_example5_data.root\",-0.018,0.1259,0,100000\)
