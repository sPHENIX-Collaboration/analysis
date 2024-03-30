# INTT dNdeta analysis
The codes prepared by Cheng-Wei for the analysis 

## Introduction
1. Folder `DST_MC`
  - The major analysis codes used in analysis note are in this folder, as shown in the following:
    -  `INTTXYvtx.h` for the reconstruction of `average vtx XY`.
    -  `INTTZvtx.h` for the reconstruction of `per-event vtx Z`.
    -  `INTTXYEvt.h` for the reconstruction of `per-event vtx XY`.
  - The codes related to the tracklet reconstruction are `INTTEta.h` and `MegaTrackFinder.h`, which are under development.
  - The codes under the folder `DeltaR_check` are for the reconstructed tracklets matching with the true particles, which is under development.
2. Folder `DST_MC/control_files` places the codes including those major analysis header files for different batches of data/MC.
  - Folder `MC_HIJING_398` and `data_20869_private_test1` place the scripts that read the header files and run the vertex reconstructions.
  - Take the `MC_HIJING_398` as example:
    - `run_xy_398.C` for the reconstruction of average vertex XY
    - `evt_z_398.C` for the reconstruction of per-event vertex Z
    - `evt_xy_HIJING_398.C`, for the reconstruction of per-event vertex XY
  - If one would like to try to run the code, the following steps are suggested, taking the `run_xy_398.C` as example:
    1. Modify the `string input_directory`, `string out_folder_directory`, `string MC_list_name` in lines 6, 8 and 9, respectively.
    2. Do the following
      ```  bash
      root -l -b -q run_xy_398.C 
      ```
    3. Once the running is finished, the `out_folder_directory` will be generated, and it includes all the results related to the `average vertex XY reconstruction`
    4. the average vertex XY can be taken, and then be filled in the line 16, `pair<double,double> beam_origin`, of the code `evt_z_398.C`.
    5. The following can be executed once the directories have been set properly.
       ``` bash
       root -l -b -q evt_z_398.C
       ```
