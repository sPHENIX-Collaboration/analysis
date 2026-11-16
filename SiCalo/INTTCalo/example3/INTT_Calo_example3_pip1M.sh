#!/bin/bash
cp INTT_Calo_trkUser_example3.cpp ../INTT_Calo_trkUser.cpp
cp INTT_Calo_trkUser_example3.h ../INTT_Calo_trkUser.h
cd ..
root -b -q INTT_Calo_Ana.cpp\(\"TH_data/ana2_pi+/allpip.root\",1\,\"example3/INTT_Calo_example3_pip1M.root\",0.,0.,0,1000000\)
