MC_folder=/sphenix/user/ChengWei/sPH_dNdeta/Sim_Ntuple_HIJING_new_20240424_HR_test

sh run_script.sh monitoring_condor

sh run_script.sh create_new avg_vtxXY MC $MC_folder 6
sh run_script.sh submit_condor_job avg_vtxXY MC $MC_folder
sh run_script.sh get_merged_result avg_vtxXY MC $MC_folder

sh run_script.sh create_new evt_vtxZ MC $MC_folder 199
sh run_script.sh submit_condor_job evt_vtxZ MC $MC_folder
sh run_script.sh get_merged_result evt_vtxZ MC $MC_folder

sh run_script.sh create_new evt_vtxXY MC $MC_folder 28
sh run_script.sh submit_condor_job evt_vtxXY MC $MC_folder
sh run_script.sh get_merged_result evt_vtxXY MC $MC_folder

sh run_script.sh create_new evt_tracklet MC $MC_folder 199
sh run_script.sh submit_condor_job evt_tracklet MC $MC_folder
sh run_script.sh create_new evt_tracklet_inner_phi_rotate MC $MC_folder 199
sh run_script.sh submit_condor_job evt_tracklet_inner_phi_rotate MC $MC_folder

sh run_script.sh get_merged_result evt_tracklet MC $MC_folder
# have to do the merge manually 
sh run_script.sh get_merged_result evt_tracklet_inner_phi_rotate MC $MC_folder
# have to do the merge manually

data_folder=/sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/F4A_20869/2024_05_07/folder_Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey_test