#! /bin/bash

# Beam spot reconstruction
# python runCondor_beamspot.py --isdata --infiledir /sphenix/tg/tg01/hf/hjheng/ppg02/dst/Data_Run54280_20250117_ProdA2024 --outfiledir BeamspotMinitree_Data_Run54280_20250117_ProdA2024 --dphicut 0.122 --nJob 326 
# python runCondor_beamspot.py --isdata --infiledir /sphenix/tg/tg01/hf/hjheng/ppg02/dst/Data_Run54280_20250117_ProdA2024_Zclustering --outfiledir BeamspotMinitree_Data_Run54280_20250117_ProdA2024_Zclustering --dphicut 0.122 --nJob 326 --submitcondor
# python runCondor_beamspot.py --infiledir /sphenix/tg/tg01/hf/hjheng/ppg02/dst/Sim_HIJING_ana457_20250110 --outfiledir BeamspotMinitree_Sim_HIJING_ana457_20250110 --dphicut 0.122 --nJob 1000
# python runCondor_beamspot.py --infiledir /sphenix/tg/tg01/hf/hjheng/ppg02/dst/Sim_HIJING_ananew_20250106 --outfiledir BeamspotMinitree_Sim_HIJING_ananew_20250106 --dphicut 0.122 --nJob 200
# python runCondor_beamspot.py --infiledir /sphenix/tg/tg01/hf/hjheng/ppg02/dst/Sim_HIJING_ananew_20250131 --outfiledir BeamspotMinitree_Sim_HIJING_ananew_20250131 --dphicut 0.122 --nJob 1000 --submitcondor

# Vertex Z reconstruction
# python runCondor_recovtxz.py --isdata --filedesc Data_Run54280_20250117_ProdA2024 --nevents 10000 --nJob 326 --submitcondor
# python runCondor_recovtxz.py --isdata --filedesc Data_Run54280_20250117_ProdA2024_Zclustering --nevents 10000 --nJob 326 --submitcondor
# python runCondor_recovtxz.py --filedesc Sim_HIJING_ananew_20250106 --nevents 400 --nJob 200 --submitcondor
# python runCondor_recovtxz.py --filedesc Sim_HIJING_ananew_20250107 --nevents 1000 --nJob 500 --submitcondor
# python runCondor_recovtxz.py --filedesc Sim_HIJING_ananew_20250131 --nevents 500 --nJob 1000 --submitcondor


# Tracklet reconstruction
# # Data - Varying deltaR cut
# python runCondor_recotracklet.py --isdata --filedesc Data_Run54280_20250117_ProdA2024 --nEvents 10000 --nJob 326 --drcut 0.5 --randomclusset 0 --clusadccutset 0 --clusphisizecutset 0 --submitcondor
# python runCondor_recotracklet.py --isdata --filedesc Data_Run54280_20250117_ProdA2024 --nEvents 10000 --nJob 326 --drcut 0.4 --randomclusset 0 --clusadccutset 0 --clusphisizecutset 0 --submitcondor
# python runCondor_recotracklet.py --isdata --filedesc Data_Run54280_20250117_ProdA2024 --nEvents 10000 --nJob 326 --drcut 0.6 --randomclusset 0 --clusadccutset 0 --clusphisizecutset 0 --submitcondor
# python runCondor_recotracklet.py --isdata --filedesc Data_Run54280_20250117_ProdA2024 --nEvents 10000 --nJob 326 --drcut 0.1 --randomclusset 0 --clusadccutset 0 --clusphisizecutset 0 --submitcondor
# python runCondor_recotracklet.py --isdata --filedesc Data_Run54280_20250117_ProdA2024 --nEvents 10000 --nJob 326 --drcut 999 --randomclusset 0 --clusadccutset 0 --clusphisizecutset 0 --submitcondor
# # Data - Random Z vertex 
# python runCondor_recotracklet.py --isdata --filedesc Data_Run54280_20250117_ProdA2024 --nEvents 10000 --nJob 326 --randomvtxz --drcut 0.5 --randomclusset 0 --clusadccutset 0 --clusphisizecutset 0 --submitcondor
# # Data - Varying cluster adc cut: set 0 is 35, set 1 is 0 (none)
# python runCondor_recotracklet.py --isdata --filedesc Data_Run54280_20250117_ProdA2024 --nEvents 10000 --nJob 326 --drcut 0.5 --randomclusset 0 --clusadccutset 1 --clusphisizecutset 0 --submitcondor
# python runCondor_recotracklet.py --isdata --filedesc Data_Run54280_20250117_ProdA2024 --nEvents 10000 --nJob 326 --drcut 0.5 --randomclusset 0 --clusadccutset 2 --clusphisizecutset 0 --submitcondor
# # Data - Varying cluster phi size cut: set 0 is 40, set 1 is none
# python runCondor_recotracklet.py --isdata --filedesc Data_Run54280_20250117_ProdA2024 --nEvents 10000 --nJob 326 --drcut 0.5 --randomclusset 0 --clusadccutset 0 --clusphisizecutset 1 --submitcondor
# # Simulation - Varying deltaR cut
# python runCondor_recotracklet.py --filedesc Sim_HIJING_ananew_20250131 --nEvents 500 --nJob 1000 --drcut 0.5 --randomclusset 0 --clusadccutset 0 --clusphisizecutset 0 --submitcondor
# python runCondor_recotracklet.py --filedesc Sim_HIJING_ananew_20250131 --nEvents 500 --nJob 1000 --drcut 0.4 --randomclusset 0 --clusadccutset 0 --clusphisizecutset 0 --submitcondor
# python runCondor_recotracklet.py --filedesc Sim_HIJING_ananew_20250131 --nEvents 500 --nJob 1000 --drcut 0.6 --randomclusset 0 --clusadccutset 0 --clusphisizecutset 0 --submitcondor
# python runCondor_recotracklet.py --filedesc Sim_HIJING_ananew_20250131 --nEvents 500 --nJob 1000 --drcut 0.1 --randomclusset 0 --clusadccutset 0 --clusphisizecutset 0 --submitcondor
# python runCondor_recotracklet.py --filedesc Sim_HIJING_ananew_20250131 --nEvents 500 --nJob 1000 --drcut 999 --randomclusset 0 --clusadccutset 0 --clusphisizecutset 0 --submitcondor
# # Simulation - Random Z vertex 
# python runCondor_recotracklet.py --filedesc Sim_HIJING_ananew_20250131 --nEvents 500 --nJob 1000 --randomvtxz --drcut 0.5 --randomclusset 0 --clusadccutset 0 --clusphisizecutset 0 --submitcondor
# # Simulation - Varying cluster adc cut
# python runCondor_recotracklet.py --filedesc Sim_HIJING_ananew_20250131 --nEvents 500 --nJob 1000 --drcut 0.5 --randomclusset 0 --clusadccutset 1 --clusphisizecutset 0 --submitcondor
# python runCondor_recotracklet.py --filedesc Sim_HIJING_ananew_20250131 --nEvents 500 --nJob 1000 --drcut 0.5 --randomclusset 0 --clusadccutset 2 --clusphisizecutset 0 --submitcondor
# # Simulation - Varying cluster phi size cut
# python runCondor_recotracklet.py --filedesc Sim_HIJING_ananew_20250131 --nEvents 500 --nJob 1000 --drcut 0.5 --randomclusset 0 --clusadccutset 0 --clusphisizecutset 1 --submitcondor
# # Simulation - Varying random cluster set
# python runCondor_recotracklet.py --filedesc Sim_HIJING_ananew_20241202 --nEvents 400 --nJob 200 --drcut 0.5 --randomclusset 1 --clusadccutset 0 --clusphisizecutset 0 --submitcondor
# python runCondor_recotracklet.py --filedesc Sim_HIJING_ananew_20241202 --nEvents 400 --nJob 200 --drcut 0.5 --randomclusset 2 --clusadccutset 0 --clusphisizecutset 0 --submitcondor
# python runCondor_recotracklet.py --filedesc Sim_HIJING_ananew_20241202 --nEvents 400 --nJob 200 --drcut 0.5 --randomclusset 3 --clusadccutset 0 --clusphisizecutset 0 --submitcondor
# python runCondor_recotracklet.py --filedesc Sim_HIJING_ananew_20241202 --nEvents 400 --nJob 200 --drcut 0.5 --randomclusset 4 --clusadccutset 0 --clusphisizecutset 0 --submitcondor

# Plot reconstructed vertex 
# python runCondor_plotRecoVtx.py --filedesc Sim_HIJING_ananew_20250106 --nJob 200 --submitcondor
# python runCondor_plotRecoVtx.py --filedesc Sim_HIJING_ananew_20250131 --nJob 1000 --submitcondor
# python runCondor_plotRecoVtx.py --isdata --filedesc Data_Run54280_20241113 --nJob 326 --submitcondor
# python runCondor_plotRecoVtx.py --isdata --filedesc Data_Run54280_20250117_ProdA2024 --nJob 326 --submitcondor

# Plot TrkrHit

# Plot reconstructed cluster
# python runCondor_plotCluster.py --isdata --filedesc Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey --nJob 549 --submitcondor
# python runCondor_plotCluster.py --isdata --filedesc Data_CombinedNtuple_Run54280_HotChannel_BCO --nJob 100 --submitcondor
# python runCondor_plotCluster.py --isdata --filedesc Data_Run54280_20250117_ProdA2024 --nJob 326 --submitcondor
# python runCondor_plotCluster.py --isdata --filedesc Data_Run54280_20241114_Zclustering --nJob 326 --submitcondor
# python runCondor_plotCluster.py --filedesc Sim_HIJING_ananew_20250106 --nJob 200 --submitcondor
# python runCondor_plotCluster.py --filedesc Sim_HIJING_ana443_20241105_diffusionradius10um --nJob 200 --submitcondor
# python runCondor_plotCluster.py --filedesc Sim_HIJING_ana443_20241107_diffusionradius20um --nJob 200 --submitcondor
# python runCondor_plotCluster.py --filedesc Sim_HIJING_ana443_20241107_diffusionradius30um --nJob 200 --submitcondor
# python runCondor_plotCluster.py --filedesc Sim_HIJING_ana443_20241108_diffusionradius50um --nJob 200 --submitcondor
# python runCondor_plotCluster.py --filedesc Sim_HIJING_ana443_20241109_diffusionradius100um --nJob 200 --submitcondor
# python runCondor_plotCluster.py --filedesc Sim_HIJING_ana443_20241109_diffusionradius200um --nJob 200 --submitcondor
# python runCondor_plotCluster.py --filedesc Sim_HIJING_ana443_20241114_diffusionradius5um_Zclustering --nJob 200 --submitcondor

# # Plot reconstructed tracklet
# # Data - Different deltaR cut
# python runCondor_plotTracklet.py --isdata --filedesc Data_Run54280_20241113 --nJob 326 --drcut 0.5 --randomclusset 0 --clusadccutset 0 --clusphisizecutset 0 --submitcondor
# python runCondor_plotTracklet.py --isdata --filedesc Data_Run54280_20241114_Zclustering --nJob 326 --drcut 0.5 --randomclusset 0 --clusadccutset 0 --clusphisizecutset 0
# python runCondor_plotTracklet.py --isdata --filedesc Data_Run54280_20241204_ProdA2024 --nJob 326 --drcut 0.5 --randomclusset 0 --clusadccutset 0 --clusphisizecutset 0 --submitcondor
# python runCondor_plotTracklet.py --isdata --filedesc Data_Run54280_20250117_ProdA2024 --nJob 326 --drcut 0.5 --randomclusset 0 --clusadccutset 0 --clusphisizecutset 0 --submitcondor
# # Data - Different cluster adc cut
# python runCondor_plotTracklet.py --isdata --filedesc Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey --nJob 549 --drcut 0.5 --randomclusset 0 --clusadccutset 1 --submitcondor
# python runCondor_plotTracklet.py --isdata --filedesc Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey --nJob 549 --drcut 0.5 --randomclusset 0 --clusadccutset 2 --submitcondor
# # Simulation - Different deltaR cut
# python runCondor_plotTracklet.py --filedesc Sim_HIJING_ananew_20241202 --nJob 200 --drcut 0.5 --randomclusset 0 --clusadccutset 0 --clusphisizecutset 0 --submitcondor
# python runCondor_plotTracklet.py --filedesc Sim_HIJING_ananew_20241202_Zclustering --nJob 200 --drcut 0.5 --randomclusset 0 --clusadccutset 0 --clusphisizecutset 0
# python runCondor_plotTracklet.py --filedesc Sim_HIJING_ananew_20250131 --nJob 1000 --drcut 0.5 --randomclusset 0 --clusadccutset 0 --clusphisizecutset 0 --submitcondor
# # Simulation - Different cluster adc cut
# python runCondor_plotTracklet.py --filedesc Sim_Ntuple_HIJING_ana419_20240910 --nJob 200 --drcut 0.5 --randomclusset 0 --clusadccutset 1 --submitcondor
# python runCondor_plotTracklet.py --filedesc Sim_Ntuple_HIJING_ana419_20240910 --nJob 200 --drcut 0.5 --randomclusset 0 --clusadccutset 2 --submitcondor
# # Simulation - Different random cluster set
# python runCondor_plotTracklet.py --filedesc Sim_Ntuple_HIJING_ana419_20240910 --nJob 200 --drcut 0.5 --randomclusset 1 --clusadccutset 0 --submitcondor
# python runCondor_plotTracklet.py --filedesc Sim_Ntuple_HIJING_ana419_20240910 --nJob 200 --drcut 0.5 --randomclusset 2 --clusadccutset 0 --submitcondor
# python runCondor_plotTracklet.py --filedesc Sim_Ntuple_HIJING_ana419_20240910 --nJob 200 --drcut 0.5 --randomclusset 3 --clusadccutset 0 --submitcondor
# python runCondor_plotTracklet.py --filedesc Sim_Ntuple_HIJING_ana419_20240910 --nJob 200 --drcut 0.5 --randomclusset 4 --clusadccutset 0 --submitcondor

# # Corrections
# # Nominal analysis
MINITREEDIR=/sphenix/tg/tg01/hf/hjheng/ppg02/minitree
MINITREENAMESIM=TrackletMinitree_Sim_HIJING_ananew_20250131
MINITREENAMEDATA=TrackletMinitree_Data_Run54280_20250117_ProdA2024
python runCondor_corrections.py --corrfiletag HIJING_Baseline_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --outfilepathbaseline HIJING_Baseline_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --outfilepathclosure HIJING_closure_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --outfilepathdata Data_Run54280_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --hijingbaselinedir $MINITREEDIR/$MINITREENAMESIM/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --hijingclosuredir $MINITREEDIR/$MINITREENAMESIM/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --datadir $MINITREEDIR/$MINITREENAMEDATA/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --submitcondor
# # Different delta-R cut
python runCondor_corrections.py --corrfiletag HIJING_Baseline_dRcut0p1_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --outfilepathbaseline HIJING_Baseline_dRcut0p1_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --outfilepathclosure HIJING_closure_dRcut0p1_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --outfilepathdata Data_Run54280_dRcut0p1_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --hijingbaselinedir $MINITREEDIR/$MINITREENAMESIM/dRcut0p1_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --hijingclosuredir $MINITREEDIR/$MINITREENAMESIM/dRcut0p1_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --datadir $MINITREEDIR/$MINITREENAMEDATA/dRcut0p1_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --submitcondor
python runCondor_corrections.py --corrfiletag HIJING_Baseline_dRcut0p4_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --outfilepathbaseline HIJING_Baseline_dRcut0p4_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --outfilepathclosure HIJING_closure_dRcut0p4_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --outfilepathdata Data_Run54280_dRcut0p4_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --hijingbaselinedir $MINITREEDIR/$MINITREENAMESIM/dRcut0p4_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --hijingclosuredir $MINITREEDIR/$MINITREENAMESIM/dRcut0p4_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --datadir $MINITREEDIR/$MINITREENAMEDATA/dRcut0p4_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --submitcondor
python runCondor_corrections.py --corrfiletag HIJING_Baseline_dRcut0p6_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --outfilepathbaseline HIJING_Baseline_dRcut0p6_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --outfilepathclosure HIJING_closure_dRcut0p6_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --outfilepathdata Data_Run54280_dRcut0p6_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --hijingbaselinedir $MINITREEDIR/$MINITREENAMESIM/dRcut0p6_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --hijingclosuredir $MINITREEDIR/$MINITREENAMESIM/dRcut0p6_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --datadir $MINITREEDIR/$MINITREENAMEDATA/dRcut0p6_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --submitcondor
python runCondor_corrections.py --corrfiletag HIJING_Baseline_dRcut999p0_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --outfilepathbaseline HIJING_Baseline_dRcut999p0_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --outfilepathclosure HIJING_closure_dRcut999p0_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --outfilepathdata Data_Run54280_dRcut999p0_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --hijingbaselinedir $MINITREEDIR/$MINITREENAMESIM/dRcut999p0_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --hijingclosuredir $MINITREEDIR/$MINITREENAMESIM/dRcut999p0_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --datadir $MINITREEDIR/$MINITREENAMEDATA/dRcut999p0_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --submitcondor
# # Different cluster adc cut
python runCondor_corrections.py --corrfiletag HIJING_Baseline_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet1_clusPhiSizeCutSet0 --outfilepathbaseline HIJING_Baseline_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet1_clusPhiSizeCutSet0 --outfilepathclosure HIJING_closure_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet1_clusPhiSizeCutSet0 --outfilepathdata Data_Run54280_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet1_clusPhiSizeCutSet0 --hijingbaselinedir $MINITREEDIR/$MINITREENAMESIM/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet1_clusPhiSizeCutSet0 --hijingclosuredir $MINITREEDIR/$MINITREENAMESIM/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet1_clusPhiSizeCutSet0 --datadir $MINITREEDIR/$MINITREENAMEDATA/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet1_clusPhiSizeCutSet0 --submitcondor
python runCondor_corrections.py --corrfiletag HIJING_Baseline_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet2_clusPhiSizeCutSet0 --outfilepathbaseline HIJING_Baseline_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet2_clusPhiSizeCutSet0 --outfilepathclosure HIJING_closure_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet2_clusPhiSizeCutSet0 --outfilepathdata Data_Run54280_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet2_clusPhiSizeCutSet0 --hijingbaselinedir $MINITREEDIR/$MINITREENAMESIM/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet2_clusPhiSizeCutSet0 --hijingclosuredir $MINITREEDIR/$MINITREENAMESIM/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet2_clusPhiSizeCutSet0 --datadir $MINITREEDIR/$MINITREENAMEDATA/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet2_clusPhiSizeCutSet0 --submitcondor
# # Different cluster phi size cut
python runCondor_corrections.py --corrfiletag HIJING_Baseline_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet1 --outfilepathbaseline HIJING_Baseline_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet1 --outfilepathclosure HIJING_closure_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet1 --outfilepathdata Data_Run54280_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet1 --hijingbaselinedir $MINITREEDIR/$MINITREENAMESIM/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet1 --hijingclosuredir $MINITREEDIR/$MINITREENAMESIM/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet1 --datadir $MINITREEDIR/$MINITREENAMEDATA/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet1 --submitcondor
# # Different random cluster set (derive the correction factors with different random cluster set, apply the correction factors to the baseline simulation closure and data)
# python runCondor_corrections.py --corrfiletag HIJING_Baseline_dRcut0p5_NominalVtxZ_RandomClusSet1_clusAdcCutSet0_clusPhiSizeCutSet0 --outfilepathbaseline HIJING_Baseline_dRcut0p5_NominalVtxZ_RandomClusSet1_clusAdcCutSet0_clusPhiSizeCutSet0 --outfilepathclosure HIJING_closure_dRcut0p5_NominalVtxZ_RandomClusSet1_clusAdcCutSet0_clusPhiSizeCutSet0 --outfilepathdata Data_Run54280_dRcut0p5_NominalVtxZ_RandomClusSet1_clusAdcCutSet0_clusPhiSizeCutSet0 --hijingbaselinedir $MINITREEDIR/$MINITREENAMESIM/dRcut0p5_NominalVtxZ_RandomClusSet1_clusAdcCutSet0_clusPhiSizeCutSet0 --hijingclosuredir $MINITREEDIR/$MINITREENAMESIM/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --datadir $MINITREEDIR/$MINITREENAMEDATA/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --submitcondor
# python runCondor_corrections.py --corrfiletag HIJING_Baseline_dRcut0p5_NominalVtxZ_RandomClusSet2_clusAdcCutSet0_clusPhiSizeCutSet0 --outfilepathbaseline HIJING_Baseline_dRcut0p5_NominalVtxZ_RandomClusSet2_clusAdcCutSet0_clusPhiSizeCutSet0 --outfilepathclosure HIJING_closure_dRcut0p5_NominalVtxZ_RandomClusSet2_clusAdcCutSet0_clusPhiSizeCutSet0 --outfilepathdata Data_Run54280_dRcut0p5_NominalVtxZ_RandomClusSet2_clusAdcCutSet0_clusPhiSizeCutSet0 --hijingbaselinedir $MINITREEDIR/$MINITREENAMESIM/dRcut0p5_NominalVtxZ_RandomClusSet2_clusAdcCutSet0_clusPhiSizeCutSet0 --hijingclosuredir $MINITREEDIR/$MINITREENAMESIM/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --datadir $MINITREEDIR/$MINITREENAMEDATA/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --submitcondor
# python runCondor_corrections.py --corrfiletag HIJING_Baseline_dRcut0p5_NominalVtxZ_RandomClusSet3_clusAdcCutSet0_clusPhiSizeCutSet0 --outfilepathbaseline HIJING_Baseline_dRcut0p5_NominalVtxZ_RandomClusSet3_clusAdcCutSet0_clusPhiSizeCutSet0 --outfilepathclosure HIJING_closure_dRcut0p5_NominalVtxZ_RandomClusSet3_clusAdcCutSet0_clusPhiSizeCutSet0 --outfilepathdata Data_Run54280_dRcut0p5_NominalVtxZ_RandomClusSet3_clusAdcCutSet0_clusPhiSizeCutSet0 --hijingbaselinedir $MINITREEDIR/$MINITREENAMESIM/dRcut0p5_NominalVtxZ_RandomClusSet3_clusAdcCutSet0_clusPhiSizeCutSet0 --hijingclosuredir $MINITREEDIR/$MINITREENAMESIM/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --datadir $MINITREEDIR/$MINITREENAMEDATA/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --submitcondor
# python runCondor_corrections.py --corrfiletag HIJING_Baseline_dRcut0p5_NominalVtxZ_RandomClusSet4_clusAdcCutSet0_clusPhiSizeCutSet0 --outfilepathbaseline HIJING_Baseline_dRcut0p5_NominalVtxZ_RandomClusSet4_clusAdcCutSet0_clusPhiSizeCutSet0 --outfilepathclosure HIJING_closure_dRcut0p5_NominalVtxZ_RandomClusSet4_clusAdcCutSet0_clusPhiSizeCutSet0 --outfilepathdata Data_Run54280_dRcut0p5_NominalVtxZ_RandomClusSet4_clusAdcCutSet0_clusPhiSizeCutSet0 --hijingbaselinedir $MINITREEDIR/$MINITREENAMESIM/dRcut0p5_NominalVtxZ_RandomClusSet4_clusAdcCutSet0_clusPhiSizeCutSet0 --hijingclosuredir $MINITREEDIR/$MINITREENAMESIM/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --datadir $MINITREEDIR/$MINITREENAMEDATA/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --submitcondor