void load_files(int scan, int location, int *energies, string *filenames){
    
     if(scan == 1){
   	energies[0] = 2;
      	energies[1] = 3;
   	energies[2] = 4;
      	energies[3] = 8;
   	energies[4] = 12;
      	energies[5] = 16;
   	energies[6] = 24;
      	energies[7] = 32;
     }
     if(scan == 2){
   	energies[0] = 1;
      	energies[1] = 2;
   	energies[2] = 3;
      	energies[3] = 4;
   	energies[4] = 6;
      	energies[5] = 8;
   	energies[6] = 12;
      	energies[7] = 16;
     }
     if(scan == 3){
   	energies[0] = 2;
      	energies[1] = 4;
   	energies[2] = 6;
      	energies[3] = 8;
   	energies[4] = 12;
      	energies[5] = 16;
   	energies[6] = 24;
      	energies[7] = 28;
     }

     if(scan == 1){
         filenames[0] = "output_2GeV_2042_hodo_tree6.root";
         filenames[1] = "output_3GeV_2040_hodo_tree6.root";
         filenames[2] = "output_4GeV_2039_hodo_tree6.root";
         filenames[3] = "output_8GeV_2038_hodo_tree6.root";
         filenames[4] = "output_12GeV_2067_hodo_tree6.root";
         filenames[5] = "output_16GeV_2063_hodo_tree6.root";
         filenames[6] = "output_24GeV_2061_hodo_tree6.root";
         filenames[7] = "output_32GeV_2060_hodo_tree6.root";
     }
     if(scan == 2){
         /*locations:
          location 1: UIUC (Tower 21)
          location 2: alt UIUC (Tower 18)
          location 3: THP (Tower 42)
          
          note: some files (marked placeholder) are duplicates of other energies as not all energies for the scan were run for each location
          */
         if (location == 1) { //UIUC
             filenames[0] = "output_UIUCenergyscan_1GeV_2298_hodo_tree7_Jun17.root";
             filenames[1] = "output_UIUCenergyscan_2GeV_2268_hodo_tree7_Jun17.root";
             filenames[2] = "output_UIUCenergyscan_3GeV_2260_hodo_tree7_Jun17.root";
             filenames[3] = "output_UIUCenergyscan_4GeV_2256_hodo_tree7_Jun17.root";
             filenames[4] = "output_UIUCenergyscan_6GeV_2247_hodo_tree7_Jun17.root";
             filenames[5] = "output_UIUCenergyscan_8GeV_2295_hodo_tree7_Jun17.root";
             filenames[6] = "output_UIUCenergyscan_12GeV_2276_hodo_tree7_Jun17.root";
             filenames[7] = "output_UIUCenergyscan_16GeV_2280_hodo_tree7_Jun17.root";
         }
         if (location == 2){//alt UIUC (only 4 and 8 GeV)
             filenames[0] = "output_Tower18_UIUC_2ndenergyscan_4GeV_2256_hodo_tree10_Jul7.root ";//placeholder
             filenames[1] = "output_Tower18_UIUC_2ndenergyscan_4GeV_2256_hodo_tree10_Jul7.root";//placeholder
             filenames[2] = "output_Tower18_UIUC_2ndenergyscan_4GeV_2256_hodo_tree10_Jul7.root";//placeholder
             filenames[3] = "output_Tower18_UIUC_2ndenergyscan_4GeV_2256_hodo_tree10_Jul7.root";
             filenames[4] = "output_Tower18_UIUC_2ndenergyscan_8GeV_2235_hodo_tree10_Jul7.root";//placeholder
             filenames[5] = "output_Tower18_UIUC_2ndenergyscan_8GeV_2235_hodo_tree10_Jul7.root";
             filenames[6] = "output_Tower18_UIUC_2ndenergyscan_8GeV_2235_hodo_tree10_Jul7.root";//placeholder
             filenames[7] = "output_Tower18_UIUC_2ndenergyscan_8GeV_2235_hodo_tree10_Jul7.root";//placeholder
         }
         if (location == 3) {//THP (no 1GeV)
             filenames[0] = "output_THP_2ndenergyscan_2GeV_2269_hodo_tree10_Jul7.root"; //placeholder
             filenames[1] = "output_THP_2ndenergyscan_2GeV_2269_hodo_tree10_Jul7.root";
             filenames[2] = "output_THP_2ndenergyscan_3GeV_2264_hodo_tree10_Jul7.root";
             filenames[3] = "output_THP_2ndenergyscan_4GeV_2257_hodo_tree10_Jul7.root";
             filenames[4] = "output_THP_2ndenergyscan_6GeV_2249_hodo_tree10_Jul7.root";
             filenames[5] = "output_THP_2ndenergyscan_8GeV_2288_hodo_tree10_Jul7.root";
             filenames[6] = "output_THP_2ndenergyscan_12GeV_2278_hodo_tree10_Jul7.root";
             filenames[7] = "output_THP_2ndenergyscan_16GeV_2284_hodo_tree10_Jul7.root";
         }
     }
    if(scan == 3){
         filenames[0] = "output_HCAL_EMCALscan_minus2GeV_2692_hodo_tree9_Jun23.root";
         filenames[1] = "output_HCAL_EMCALscan_minus4GeV_2695_hodo_tree9_Jun23.root";
         filenames[2] = "output_HCAL_EMCALscan_minus6GeV_2698_hodo_tree9_Jun23.root";
         filenames[3] = "output_HCAL_EMCALscan_minus8GeV_2703_hodo_tree9_Jun23.root";
         filenames[4] = "output_HCAL_EMCALscan_minus12GeV_2719_hodo_tree9_Jun23.root";
         filenames[5] = "output_HCAL_EMCALscan_minus16GeV_2722_hodo_tree9_Jun23.root";
         filenames[6] = "output_HCAL_EMCALscan_minus24GeV_lowergain_2727_hodo_tree9_Jun23.root";
         filenames[7] = "output_HCAL_EMCALscan_minus28GeV_lowergain_2730_hodo_tree9_Jun23.root";
     }
}

