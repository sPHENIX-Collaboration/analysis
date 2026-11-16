#!/bin/bash
cp INTT_Calo_trkUser_example4.cpp ../INTT_Calo_trkUser.cpp
cp INTT_Calo_trkUser_example4.h ../INTT_Calo_trkUser.h
cd ..
root INTT_Calo_Ana.cpp\(\"TH_data/ana2_jpsi/alljpsi.root\",2\)
