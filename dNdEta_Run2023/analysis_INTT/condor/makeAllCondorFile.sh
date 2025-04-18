#! /bin/bash

NEVENTDATA=5000
NJOBDATA=1758
FILEDESCDATA=Data_Run54280_20250210_ProdA2024
NEVENTSIM=200
NJOBSIM=5000
FILEDESCSIM=Sim_HIJING_MDC2_ana472_20250307
# FILEDESCSIM=Sim_EPOS_MDC2_ana472_20250310
# FILEDESCSIM=Sim_AMPT_MDC2_ana472_20250310
# FILEDESCSIM=Sim_HIJING_strangeness_MDC2_ana472_20250310

# Beam spot reconstruction
# python runCondor_beamspot.py --isdata --infiledir /sphenix/tg/tg01/hf/hjheng/ppg02/dst/Data_Run54280_20250210_ProdA2024 --outfiledir BeamspotMinitree_Data_Run54280_20250210_ProdA2024 --dphicut 0.122 --nJob 1758 --submitcondor
# python runCondor_beamspot.py --infiledir /sphenix/tg/tg01/hf/hjheng/ppg02/dst/Sim_HIJING_ananew_20250131 --outfiledir BeamspotMinitree_Sim_HIJING_ananew_20250131 --dphicut 0.122 --nJob 1000 --submitcondor
# python runCondor_beamspot.py --infiledir /sphenix/tg/tg01/hf/hjheng/ppg02/dst/Sim_SIMPLE_ana466_20250214 --outfiledir BeamspotMinitree_Sim_SIMPLE_ana466_20250214 --dphicut 0.122 --nJob 500 --submitcondor
# python runCondor_beamspot.py --infiledir /sphenix/tg/tg01/hf/hjheng/ppg02/dst/$FILEDESCSIM --outfiledir BeamspotMinitree_$FILEDESCSIM --dphicut 0.122 --nJob $NJOBSIM --submitcondor
# Vertex Z reconstruction
# python runCondor_recovtxz.py --isdata --filedesc Data_Run54280_20250210_ProdA2024 --nevents 5000 --nJob 1758 --submitcondor
# python runCondor_recovtxz.py --filedesc Sim_HIJING_ananew_20250131 --nevents 500 --nJob 1000 --submitcondor
# python runCondor_recovtxz.py --filedesc Sim_SIMPLE_ana466_20250214 --nevents 200 --nJob 500 --submitcondor
# python runCondor_recovtxz.py --filedesc $FILEDESCSIM --nevents $NEVENTSIM --nJob $NJOBSIM --submitcondor

# Tracklet reconstruction
# # Data - Varying deltaR cut
# python runCondor_recotracklet.py --isdata --filedesc $FILEDESCDATA --nEvents $NEVENTDATA --nJob $NJOBDATA --drcut 0.5 --randomclusset 0 --clusadccutset 0 --clusphisizecutset 0 --submitcondor
# python runCondor_recotracklet.py --isdata --filedesc $FILEDESCDATA --nEvents $NEVENTDATA --nJob $NJOBDATA --drcut 0.4 --randomclusset 0 --clusadccutset 0 --clusphisizecutset 0 --submitcondor
# python runCondor_recotracklet.py --isdata --filedesc $FILEDESCDATA --nEvents $NEVENTDATA --nJob $NJOBDATA --drcut 0.6 --randomclusset 0 --clusadccutset 0 --clusphisizecutset 0 --submitcondor
# python runCondor_recotracklet.py --isdata --filedesc $FILEDESCDATA --nEvents $NEVENTDATA --nJob $NJOBDATA --drcut 0.1 --randomclusset 0 --clusadccutset 0 --clusphisizecutset 0 --submitcondor
# python runCondor_recotracklet.py --isdata --filedesc $FILEDESCDATA --nEvents $NEVENTDATA --nJob $NJOBDATA --drcut 999 --randomclusset 0 --clusadccutset 0 --clusphisizecutset 0 --submitcondor
# # Data - Random Z vertex 
# python runCondor_recotracklet.py --isdata --filedesc $FILEDESCDATA --nEvents $NEVENTDATA --nJob $NJOBDATA --randomvtxz --drcut 0.5 --randomclusset 0 --clusadccutset 0 --clusphisizecutset 0 --submitcondor
# # Data - Varying cluster adc cut: set 0 is 35, set 1 is 0 (none)
# python runCondor_recotracklet.py --isdata --filedesc $FILEDESCDATA --nEvents $NEVENTDATA --nJob $NJOBDATA --drcut 0.5 --randomclusset 0 --clusadccutset 1 --clusphisizecutset 0 --submitcondor
# python runCondor_recotracklet.py --isdata --filedesc $FILEDESCDATA --nEvents $NEVENTDATA --nJob $NJOBDATA --drcut 0.5 --randomclusset 0 --clusadccutset 2 --clusphisizecutset 0 --submitcondor
# # Data - Varying cluster phi size cut: set 0 is 40, set 1 is none
# python runCondor_recotracklet.py --isdata --filedesc $FILEDESCDATA --nEvents $NEVENTDATA --nJob $NJOBDATA --drcut 0.5 --randomclusset 0 --clusadccutset 0 --clusphisizecutset 1 --submitcondor
# # Simulation - Varying deltaR cut
# python runCondor_recotracklet.py --filedesc $FILEDESCSIM --nEvents $NEVENTSIM --nJob $NJOBSIM --drcut 0.5 --randomclusset 0 --clusadccutset 0 --clusphisizecutset 0 --submitcondor
# python runCondor_recotracklet.py --filedesc $FILEDESCSIM --nEvents $NEVENTSIM --nJob $NJOBSIM --drcut 0.4 --randomclusset 0 --clusadccutset 0 --clusphisizecutset 0 --submitcondor
# python runCondor_recotracklet.py --filedesc $FILEDESCSIM --nEvents $NEVENTSIM --nJob $NJOBSIM --drcut 0.6 --randomclusset 0 --clusadccutset 0 --clusphisizecutset 0 --submitcondor
# python runCondor_recotracklet.py --filedesc $FILEDESCSIM --nEvents $NEVENTSIM --nJob $NJOBSIM --drcut 0.1 --randomclusset 0 --clusadccutset 0 --clusphisizecutset 0 --submitcondor
# python runCondor_recotracklet.py --filedesc $FILEDESCSIM --nEvents $NEVENTSIM --nJob $NJOBSIM --drcut 999 --randomclusset 0 --clusadccutset 0 --clusphisizecutset 0 --submitcondor
# # Simulation - Random Z vertex 
# python runCondor_recotracklet.py --filedesc $FILEDESCSIM --nEvents $NEVENTSIM --nJob $NJOBSIM --randomvtxz --drcut 0.5 --randomclusset 0 --clusadccutset 0 --clusphisizecutset 0 --submitcondor
# # Simulation - Varying cluster adc cut
# python runCondor_recotracklet.py --filedesc $FILEDESCSIM --nEvents $NEVENTSIM --nJob $NJOBSIM --drcut 0.5 --randomclusset 0 --clusadccutset 1 --clusphisizecutset 0 --submitcondor
# python runCondor_recotracklet.py --filedesc $FILEDESCSIM --nEvents $NEVENTSIM --nJob $NJOBSIM --drcut 0.5 --randomclusset 0 --clusadccutset 2 --clusphisizecutset 0 --submitcondor
# # Simulation - Varying cluster phi size cut
# python runCondor_recotracklet.py --filedesc $FILEDESCSIM --nEvents $NEVENTSIM --nJob $NJOBSIM --drcut 0.5 --randomclusset 0 --clusadccutset 0 --clusphisizecutset 1 --submitcondor
# # Simulation - Varying random cluster set
# python runCondor_recotracklet.py --filedesc Sim_HIJING_ananew_20241202 --nEvents 400 --nJob 200 --drcut 0.5 --randomclusset 1 --clusadccutset 0 --clusphisizecutset 0 --submitcondor
# python runCondor_recotracklet.py --filedesc Sim_HIJING_ananew_20241202 --nEvents 400 --nJob 200 --drcut 0.5 --randomclusset 2 --clusadccutset 0 --clusphisizecutset 0 --submitcondor
# python runCondor_recotracklet.py --filedesc Sim_HIJING_ananew_20241202 --nEvents 400 --nJob 200 --drcut 0.5 --randomclusset 3 --clusadccutset 0 --clusphisizecutset 0 --submitcondor
# python runCondor_recotracklet.py --filedesc Sim_HIJING_ananew_20241202 --nEvents 400 --nJob 200 --drcut 0.5 --randomclusset 4 --clusadccutset 0 --clusphisizecutset 0 --submitcondor
# Simulation - SIMPLE 
# python runCondor_recotracklet.py --filedesc Sim_SIMPLE_ana466_20250214 --nEvents 200 --nJob 500 --drcut 999 --randomclusset 0 --clusadccutset 0 --clusphisizecutset 0 --submitcondor

# Plot reconstructed vertex 
# python runCondor_plotRecoVtx.py --filedesc $FILEDESCSIM --nJob $NJOBSIM --submitcondor
# python runCondor_plotRecoVtx.py --isdata --filedesc $FILEDESCDATA --nJob $NJOBDATA --submitcondor

# Plot TrkrHit

# Plot reconstructed cluster
# python runCondor_plotCluster.py --isdata --filedesc $FILEDESCDATA --nJob $NJOBDATA --submitcondor
# python runCondor_plotCluster.py --filedesc Sim_HIJING_ananew_20250131 --nJob 1000 --submitcondor
# python runCondor_plotCluster.py --filedesc Sim_HIJING_ana443_20241105_diffusionradius10um --nJob 200 --submitcondor
# python runCondor_plotCluster.py --filedesc Sim_HIJING_ana443_20241107_diffusionradius20um --nJob 200 --submitcondor
# python runCondor_plotCluster.py --filedesc Sim_HIJING_ana443_20241107_diffusionradius30um --nJob 200 --submitcondor
# python runCondor_plotCluster.py --filedesc Sim_HIJING_ana443_20241108_diffusionradius50um --nJob 200 --submitcondor
# python runCondor_plotCluster.py --filedesc Sim_HIJING_ana443_20241109_diffusionradius100um --nJob 200 --submitcondor
# python runCondor_plotCluster.py --filedesc Sim_HIJING_ana443_20241109_diffusionradius200um --nJob 200 --submitcondor

# # Plot reconstructed tracklet
# # Data - Different deltaR cut
# python runCondor_plotTracklet.py --isdata --filedesc $FILEDESCDATA --nJob $NJOBDATA --drcut 0.5 --randomclusset 0 --clusadccutset 0 --clusphisizecutset 0 --submitcondor
# python runCondor_plotTracklet.py --isdata --filedesc $FILEDESCDATA --nJob $NJOBDATA --drcut 0.5 --randomclusset 0 --clusadccutset 0 --clusphisizecutset 1 --submitcondor
# # Data - Different cluster adc cut
# python runCondor_plotTracklet.py --isdata --filedesc Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey --nJob 549 --drcut 0.5 --randomclusset 0 --clusadccutset 1 --submitcondor
# python runCondor_plotTracklet.py --isdata --filedesc Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey --nJob 549 --drcut 0.5 --randomclusset 0 --clusadccutset 2 --submitcondor
# # Simulation - Different deltaR cut
# python runCondor_plotTracklet.py --filedesc $FILEDESCSIM --nJob $NJOBSIM --drcut 0.5 --randomclusset 0 --clusadccutset 0 --clusphisizecutset 0 --submitcondor
# python runCondor_plotTracklet.py --filedesc $FILEDESCSIM --nJob $NJOBSIM --drcut 0.5 --randomclusset 0 --clusadccutset 0 --clusphisizecutset 1 --submitcondor

# # Corrections
# # Nominal analysis
MINITREEDIR=/sphenix/tg/tg01/hf/hjheng/ppg02/minitree
# MINITREENAMESIM=TrackletMinitree_Sim_HIJING_MDC2_ana467_20250225
# MINITREENAMESIM=TrackletMinitree_Sim_HIJING_strangeness_MDC2_ana467_20250226
# MINITREENAMEDATA=TrackletMinitree_Data_Run54280_20250210_ProdA2024
MINITREENAMESIM=TrackletMinitree_$FILEDESCSIM
MINITREENAMEDATA=TrackletMinitree_$FILEDESCDATA
python runCondor_corrections.py --corrfiletag HIJING_Baseline_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --outfilepathbaseline HIJING_Baseline_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --outfilepathclosure HIJING_closure_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --outfilepathdata Data_Run54280_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --hijingbaselinedir $MINITREEDIR/$MINITREENAMESIM/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --hijingclosuredir $MINITREEDIR/$MINITREENAMESIM/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --datadir $MINITREEDIR/$MINITREENAMEDATA/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --submitcondor
# # Different delta-R cut
python runCondor_corrections.py --corrfiletag HIJING_Baseline_dRcut0p4_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --outfilepathbaseline HIJING_Baseline_dRcut0p4_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --outfilepathclosure HIJING_closure_dRcut0p4_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --outfilepathdata Data_Run54280_dRcut0p4_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --hijingbaselinedir $MINITREEDIR/$MINITREENAMESIM/dRcut0p4_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --hijingclosuredir $MINITREEDIR/$MINITREENAMESIM/dRcut0p4_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --datadir $MINITREEDIR/$MINITREENAMEDATA/dRcut0p4_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --submitcondor
python runCondor_corrections.py --corrfiletag HIJING_Baseline_dRcut0p6_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --outfilepathbaseline HIJING_Baseline_dRcut0p6_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --outfilepathclosure HIJING_closure_dRcut0p6_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --outfilepathdata Data_Run54280_dRcut0p6_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --hijingbaselinedir $MINITREEDIR/$MINITREENAMESIM/dRcut0p6_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --hijingclosuredir $MINITREEDIR/$MINITREENAMESIM/dRcut0p6_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --datadir $MINITREEDIR/$MINITREENAMEDATA/dRcut0p6_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --submitcondor
# # Different cluster adc cut
python runCondor_corrections.py --corrfiletag HIJING_Baseline_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet1_clusPhiSizeCutSet0 --outfilepathbaseline HIJING_Baseline_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet1_clusPhiSizeCutSet0 --outfilepathclosure HIJING_closure_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet1_clusPhiSizeCutSet0 --outfilepathdata Data_Run54280_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet1_clusPhiSizeCutSet0 --hijingbaselinedir $MINITREEDIR/$MINITREENAMESIM/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet1_clusPhiSizeCutSet0 --hijingclosuredir $MINITREEDIR/$MINITREENAMESIM/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet1_clusPhiSizeCutSet0 --datadir $MINITREEDIR/$MINITREENAMEDATA/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet1_clusPhiSizeCutSet0 --submitcondor
python runCondor_corrections.py --corrfiletag HIJING_Baseline_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet2_clusPhiSizeCutSet0 --outfilepathbaseline HIJING_Baseline_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet2_clusPhiSizeCutSet0 --outfilepathclosure HIJING_closure_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet2_clusPhiSizeCutSet0 --outfilepathdata Data_Run54280_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet2_clusPhiSizeCutSet0 --hijingbaselinedir $MINITREEDIR/$MINITREENAMESIM/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet2_clusPhiSizeCutSet0 --hijingclosuredir $MINITREEDIR/$MINITREENAMESIM/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet2_clusPhiSizeCutSet0 --datadir $MINITREEDIR/$MINITREENAMEDATA/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet2_clusPhiSizeCutSet0 --submitcondor
# # Different cluster phi size cut
python runCondor_corrections.py --corrfiletag HIJING_Baseline_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet1 --outfilepathbaseline HIJING_Baseline_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet1 --outfilepathclosure HIJING_closure_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet1 --outfilepathdata Data_Run54280_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet1 --hijingbaselinedir $MINITREEDIR/$MINITREENAMESIM/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet1 --hijingclosuredir $MINITREEDIR/$MINITREENAMESIM/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet1 --datadir $MINITREEDIR/$MINITREENAMEDATA/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet1 --submitcondor
# # Enhanced strangeness 
python runCondor_corrections.py --corrfiletag HIJING_Baseline_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0_enhancedstrangeness --outfilepathbaseline HIJING_Baseline_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0_enhancedstrangeness --outfilepathclosure HIJING_closure_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0_enhancedstrangeness --outfilepathdata Data_Run54280_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0_enhancedstrangeness --hijingbaselinedir $MINITREEDIR/TrackletMinitree_Sim_HIJING_strangeness_MDC2_ana472_20250310/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --hijingclosuredir $MINITREEDIR/TrackletMinitree_Sim_HIJING_strangeness_MDC2_ana472_20250310/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --datadir $MINITREEDIR/$MINITREENAMEDATA/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --submitcondor
# # Event generators
# # EPOS
python runCondor_corrections.py --corrfiletag EPOS_Baseline_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --outfilepathbaseline EPOS_Baseline_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --outfilepathclosure EPOS_closure_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --outfilepathdata Data_Run54280_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0_EPOS --hijingbaselinedir $MINITREEDIR/TrackletMinitree_Sim_EPOS_MDC2_ana472_20250310/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --hijingclosuredir $MINITREEDIR/TrackletMinitree_Sim_EPOS_MDC2_ana472_20250310/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --datadir $MINITREEDIR/TrackletMinitree_Data_Run54280_20250210_ProdA2024/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --submitcondor
# # AMPT
python runCondor_corrections.py --corrfiletag AMPT_Baseline_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --outfilepathbaseline AMPT_Baseline_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --outfilepathclosure AMPT_closure_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --outfilepathdata Data_Run54280_dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0_AMPT --hijingbaselinedir $MINITREEDIR/TrackletMinitree_Sim_AMPT_MDC2_ana472_20250310/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --hijingclosuredir $MINITREEDIR/TrackletMinitree_Sim_AMPT_MDC2_ana472_20250310/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --datadir $MINITREEDIR/TrackletMinitree_Data_Run54280_20250210_ProdA2024/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --submitcondor

# # Different random cluster set (derive the correction factors with different random cluster set, apply the correction factors to the baseline simulation closure and data)
# python runCondor_corrections.py --corrfiletag HIJING_Baseline_dRcut0p5_NominalVtxZ_RandomClusSet1_clusAdcCutSet0_clusPhiSizeCutSet0 --outfilepathbaseline HIJING_Baseline_dRcut0p5_NominalVtxZ_RandomClusSet1_clusAdcCutSet0_clusPhiSizeCutSet0 --outfilepathclosure HIJING_closure_dRcut0p5_NominalVtxZ_RandomClusSet1_clusAdcCutSet0_clusPhiSizeCutSet0 --outfilepathdata Data_Run54280_dRcut0p5_NominalVtxZ_RandomClusSet1_clusAdcCutSet0_clusPhiSizeCutSet0 --hijingbaselinedir $MINITREEDIR/$MINITREENAMESIM/dRcut0p5_NominalVtxZ_RandomClusSet1_clusAdcCutSet0_clusPhiSizeCutSet0 --hijingclosuredir $MINITREEDIR/$MINITREENAMESIM/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --datadir $MINITREEDIR/$MINITREENAMEDATA/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --submitcondor
# python runCondor_corrections.py --corrfiletag HIJING_Baseline_dRcut0p5_NominalVtxZ_RandomClusSet2_clusAdcCutSet0_clusPhiSizeCutSet0 --outfilepathbaseline HIJING_Baseline_dRcut0p5_NominalVtxZ_RandomClusSet2_clusAdcCutSet0_clusPhiSizeCutSet0 --outfilepathclosure HIJING_closure_dRcut0p5_NominalVtxZ_RandomClusSet2_clusAdcCutSet0_clusPhiSizeCutSet0 --outfilepathdata Data_Run54280_dRcut0p5_NominalVtxZ_RandomClusSet2_clusAdcCutSet0_clusPhiSizeCutSet0 --hijingbaselinedir $MINITREEDIR/$MINITREENAMESIM/dRcut0p5_NominalVtxZ_RandomClusSet2_clusAdcCutSet0_clusPhiSizeCutSet0 --hijingclosuredir $MINITREEDIR/$MINITREENAMESIM/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --datadir $MINITREEDIR/$MINITREENAMEDATA/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --submitcondor
# python runCondor_corrections.py --corrfiletag HIJING_Baseline_dRcut0p5_NominalVtxZ_RandomClusSet3_clusAdcCutSet0_clusPhiSizeCutSet0 --outfilepathbaseline HIJING_Baseline_dRcut0p5_NominalVtxZ_RandomClusSet3_clusAdcCutSet0_clusPhiSizeCutSet0 --outfilepathclosure HIJING_closure_dRcut0p5_NominalVtxZ_RandomClusSet3_clusAdcCutSet0_clusPhiSizeCutSet0 --outfilepathdata Data_Run54280_dRcut0p5_NominalVtxZ_RandomClusSet3_clusAdcCutSet0_clusPhiSizeCutSet0 --hijingbaselinedir $MINITREEDIR/$MINITREENAMESIM/dRcut0p5_NominalVtxZ_RandomClusSet3_clusAdcCutSet0_clusPhiSizeCutSet0 --hijingclosuredir $MINITREEDIR/$MINITREENAMESIM/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --datadir $MINITREEDIR/$MINITREENAMEDATA/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --submitcondor
# python runCondor_corrections.py --corrfiletag HIJING_Baseline_dRcut0p5_NominalVtxZ_RandomClusSet4_clusAdcCutSet0_clusPhiSizeCutSet0 --outfilepathbaseline HIJING_Baseline_dRcut0p5_NominalVtxZ_RandomClusSet4_clusAdcCutSet0_clusPhiSizeCutSet0 --outfilepathclosure HIJING_closure_dRcut0p5_NominalVtxZ_RandomClusSet4_clusAdcCutSet0_clusPhiSizeCutSet0 --outfilepathdata Data_Run54280_dRcut0p5_NominalVtxZ_RandomClusSet4_clusAdcCutSet0_clusPhiSizeCutSet0 --hijingbaselinedir $MINITREEDIR/$MINITREENAMESIM/dRcut0p5_NominalVtxZ_RandomClusSet4_clusAdcCutSet0_clusPhiSizeCutSet0 --hijingclosuredir $MINITREEDIR/$MINITREENAMESIM/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --datadir $MINITREEDIR/$MINITREENAMEDATA/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0 --submitcondor