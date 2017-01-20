DIR="/phenix/u/cperez/sphenix/software/macros/macros/g4simulations/"

rm G4*
ln -s ${DIR}/G4Setup_sPHENIX.C .
ln -s ${DIR}/G4_TPC.C .
ln -s ${DIR}/G4_Svtx_MAPScyl_ITTcyl_TPC.C
ln -s ${DIR}/G4_Pipe.C .
ln -s ${DIR}/G4_Svtx_maps+IT+tpc.C .
ln -s ${DIR}/G4_Svtx_maps_ladders+intt+tpc.C
ln -s ${DIR}/G4_PreShower.C .
ln -s ${DIR}/G4_CEmc_Spacal.C .
ln -s ${DIR}/G4_HcalIn_ref.C .
ln -s ${DIR}/G4_Magnet.C .
ln -s ${DIR}/G4_HcalOut_ref.C .
ln -s ${DIR}/G4_Bbc.C
ln -s ${DIR}/G4_Global.C
