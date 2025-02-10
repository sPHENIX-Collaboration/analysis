#include "TrackletHistogramNew.h"

TrackletHistogramNew::TrackletHistogramNew(
    int process_id_in,
    int runnumber_in,
    int run_nEvents_in,
    std::string input_directory_in,
    std::string input_file_name_in,
    std::string output_directory_in,

    std::string output_file_name_suffix_in,
    std::pair<double, double> vertexXYIncm_in,

    std::pair<bool, TH1D*> vtxZReweight_in,
    bool BcoFullDiffCut_in,
    bool INTT_vtxZ_QA_in,
    std::pair<bool, std::pair<double, double>> isClusQA_in,
    bool HaveGeoOffsetTag_in,
    std::pair<bool, int> SetRandomHits_in,
    bool RandInttZ_in,
    bool ColMulMask_in
) : ClusHistogram(
    process_id_in,
    runnumber_in,
    run_nEvents_in,
    input_directory_in,
    input_file_name_in,
    output_directory_in,

    output_file_name_suffix_in,
    vertexXYIncm_in,

    vtxZReweight_in,
    BcoFullDiffCut_in,
    INTT_vtxZ_QA_in,
    isClusQA_in,
    HaveGeoOffsetTag_in,
    SetRandomHits_in,
    RandInttZ_in,
    ColMulMask_in,

    1 // note : constructor type
){
    track_gr = new TGraphErrors();
    fit_rz = new TF1("fit_rz","pol1",-1000,1000);

    PrepareOutPutFileName();
    PrepareOutPutRootFile();
    PrepareHistograms();
}

void TrackletHistogramNew::PrepareOutPutFileName()
{
    std::string job_index = std::to_string( process_id );
    int job_index_len = 5;
    job_index.insert(0, job_index_len - job_index.size(), '0');

    std::string runnumber_str = std::to_string( runnumber );
    if (runnumber != -1){
        int runnumber_str_len = 8;
        runnumber_str.insert(0, runnumber_str_len - runnumber_str.size(), '0');
    }

    if (output_file_name_suffix.size() > 0 && output_file_name_suffix[0] != '_') {
        output_file_name_suffix = "_" + output_file_name_suffix;
    }

    output_filename = "TrackHist";
    output_filename = (runnumber != -1) ? "Data_" + output_filename : "MC_" + output_filename;
    output_filename += (vtxZReweight.first) ? "_vtxZReweight" : "";
    output_filename += (BcoFullDiffCut && runnumber != -1) ? "_BcoFullDiffCut" : "";
    output_filename += (INTT_vtxZ_QA) ? "_VtxZQA" : "";
    output_filename += (isClusQA.first) ? Form("_ClusQAAdc%.0fPhiSize%.0f",isClusQA.second.first,isClusQA.second.second) : "";
    output_filename += (ColMulMask) ? "_ColMulMask" : "";
    output_filename += (HaveGeoOffsetTag) ? "_GeoOffset" : "";
    output_filename += (SetRandomHits.first) ? Form("_Rand%dHits",SetRandomHits.second) : "";
    output_filename += (RandInttZ) ? "_RandInttZ" : "";
    output_filename += output_file_name_suffix;
    output_filename += (runnumber != -1) ? Form("_%s_%s.root",runnumber_str.c_str(),job_index.c_str()) : Form("_%s.root",job_index.c_str());
}

void TrackletHistogramNew::PrepareOutPutRootFile()
{
    file_out = new TFile(Form("%s/%s", output_directory.c_str(), output_filename.c_str()), "RECREATE");
    tree_out_par = new TTree("tree_par", "used parameters");

    tree_out_par -> Branch("process_id", &process_id);
    tree_out_par -> Branch("runnumber", &runnumber);
    tree_out_par -> Branch("vtxZReweight", &vtxZReweight.first);
    tree_out_par -> Branch("BcoFullDiffCut", &BcoFullDiffCut);
    tree_out_par -> Branch("INTT_vtxZ_QA", &INTT_vtxZ_QA);
    tree_out_par -> Branch("isClusQA", &isClusQA.first);
    tree_out_par -> Branch("isClusQA_adc", &isClusQA.second.first);
    tree_out_par -> Branch("isClusQA_phiSize", &isClusQA.second.second);

    tree_out_par -> Branch("centrality_edges", &centrality_edges);
    tree_out_par -> Branch("nCentrality_bin", &nCentrality_bin);

    tree_out_par -> Branch("CentralityFineEdge_min", &CentralityFineEdge_min);
    tree_out_par -> Branch("CentralityFineEdge_max", &CentralityFineEdge_max);
    tree_out_par -> Branch("nCentralityFineBin", &nCentralityFineBin);

    tree_out_par -> Branch("EtaEdge_min", &EtaEdge_min);
    tree_out_par -> Branch("EtaEdge_max", &EtaEdge_max);
    tree_out_par -> Branch("nEtaBin", &nEtaBin);

    tree_out_par -> Branch("VtxZEdge_min", &VtxZEdge_min);
    tree_out_par -> Branch("VtxZEdge_max", &VtxZEdge_max);
    tree_out_par -> Branch("nVtxZBin", &nVtxZBin);

    tree_out_par -> Branch("typeA_sensorZID", &typeA_sensorZID);
    tree_out_par -> Branch("cut_GlobalMBDvtxZ", &cut_GlobalMBDvtxZ);   
    tree_out_par -> Branch("cut_vtxZDiff", &cut_vtxZDiff);
    tree_out_par -> Branch("cut_TrapezoidalFitWidth", &cut_TrapezoidalFitWidth);
    tree_out_par -> Branch("cut_TrapezoidalFWHM", &cut_TrapezoidalFWHM);
    tree_out_par -> Branch("cut_INTTvtxZError", &cut_INTTvtxZError);

    tree_out_par -> Branch("cut_InttBcoFullDIff_next", &cut_InttBcoFullDIff_next);

    tree_out_par -> Branch("DeltaPhiEdge_min", &DeltaPhiEdge_min);
    tree_out_par -> Branch("DeltaPhiEdge_max", &DeltaPhiEdge_max);
    tree_out_par -> Branch("nDeltaPhiBin", &nDeltaPhiBin);

    tree_out_par -> Branch("cut_bestPair_DeltaPhi", &cut_bestPair_DeltaPhi);
    tree_out_par -> Branch("cut_GoodProtoTracklet_DeltaPhi", &cut_GoodProtoTracklet_DeltaPhi);
}

void TrackletHistogramNew::PrepareHistograms()
{   

    h1D_GoodColMap_ZId = new TH1D("h1D_GoodColMap_ZId","h1D_GoodColMap_ZId;ClusZ [cm];Entries",nZbin, Zmin, Zmax);

    h1D_INTTvtxZ = new TH1D("h1D_INTTvtxZ", "h1D_INTTvtxZ;INTT vtxZ [cm];Entries", 60, -60, 60);
    h1D_centrality_bin = new TH1D("h1D_centrality_bin","h1D_centrality_bin;Centrality [%];Entries",nCentrality_bin,&centrality_edges[0]); // note : the 0-5%
    h1D_centrality_bin_weighted = new TH1D("h1D_centrality_bin_weighted","h1D_centrality_bin_weighted;Centrality [%];Entries",nCentrality_bin,&centrality_edges[0]); // note : the 0-5%

    h1D_INTTvtxZ_FineBin = new TH1D("h1D_INTTvtxZ_FineBin", "h1D_INTTvtxZ_FineBin;INTT vtxZ [cm];Entries", 120, -60, 60);
    h1D_INTTvtxZ_FineBin_NoVtxZWeight = new TH1D("h1D_INTTvtxZ_FineBin_NoVtxZWeight", "h1D_INTTvtxZ_FineBin_NoVtxZWeight;INTT vtxZ [cm];Entries", 120, -60, 60);
    h2D_INTTvtxZFineBin_CentralityBin = new TH2D("h2D_INTTvtxZFineBin_CentralityBin", "h2D_INTTvtxZFineBin_CentralityBin;INTT vtxZ [cm];Centrality [%]", 120, -60, 60, nCentrality_bin, &centrality_edges[0]);

    h1D_eta_template = new TH1D("h1D_eta_template", "h1D_eta_template", nEtaBin, EtaEdge_min, EtaEdge_max); // note : coarse
    h1D_vtxz_template = new TH1D("h1D_vtxz_template", "h1D_vtxz_template", nVtxZBin, VtxZEdge_min, VtxZEdge_max); // note : coarse

    // note : for inclusive 
    h1D_PairDeltaEta_inclusive = new TH1D("h1D_PairDeltaEta_inclusive", "h1D_PairDeltaEta_inclusive;Pair #eta;Entries", nDeltaEtaBin, DeltaEtaEdge_min, DeltaEtaEdge_max);
    h1D_PairDeltaPhi_inclusive = new TH1D("h1D_PairDeltaPhi_inclusive", "h1D_PairDeltaPhi_inclusive;Pair #eta;Entries", nDeltaPhiBin, DeltaPhiEdge_min, DeltaPhiEdge_max);

    // note : cluster Eta
    h1D_Inner_ClusEta_INTTz = new TH1D("h1D_Inner_ClusEta_INTTz","h1D_Inner_ClusEta_INTTz;ClusEta (Inner, w.r.t INTTz);Entries", nEtaBin, EtaEdge_min, EtaEdge_max);
    h1D_Outer_ClusEta_INTTz = new TH1D("h1D_Outer_ClusEta_INTTz","h1D_Outer_ClusEta_INTTz;ClusEta (Outer, w.r.t INTTz);Entries", nEtaBin, EtaEdge_min, EtaEdge_max);

    std::vector<int> insert_check; insert_check.clear();
    bool isInserted = false;

    // note : for 1D
    h1D_map.clear();

    isInserted = h1D_map.insert( std::make_pair(
            "h1D_centrality",
            new TH1D("h1D_centrality", "h1D_centrality;Centrality [%];Entries", nCentralityFineBin, CentralityFineEdge_min, CentralityFineEdge_max)
        )
    ).second; insert_check.push_back(isInserted);

    for (int vtxz_bin = 0; vtxz_bin < nVtxZBin; vtxz_bin++)
    {
        isInserted = h1D_map.insert( std::make_pair(
                Form("h1D_centrality_InttVtxZ%d", vtxz_bin),
                new TH1D(Form("h1D_centrality_InttVtxZ%d", vtxz_bin), Form("h1D_centrality_InttVtxZ%d;Centrality [%];Entries",vtxz_bin), nCentralityFineBin, CentralityFineEdge_min, CentralityFineEdge_max)
            )
        ).second; insert_check.push_back(isInserted);

        // isInserted = h1D_map.insert( std::make_pair(
        //         Form("h1D_centrality_MBDVtxZ%d", vtxz_bin),
        //         new TH1D(Form("h1D_centrality_MBDVtxZ%d", vtxz_bin), Form("h1D_centrality_MBDVtxZ%d;Centrality [%];Entries",vtxz_bin), nCentralityFineBin, CentralityFineEdge_min, CentralityFineEdge_max)
        //     )
        // ).second; insert_check.push_back(isInserted);
    }

    // Division : ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    for (int eta_bin = 0; eta_bin < nEtaBin; eta_bin++)
    {
        for (int vtxz_bin = 0; vtxz_bin < nVtxZBin; vtxz_bin++)
        {
            for (int Mbin = 0; Mbin < nCentrality_bin; Mbin++)
            {
                isInserted = h1D_map.insert( std::make_pair(
                        Form("h1D_DeltaPhi_Mbin%d_Eta%d_VtxZ%d", Mbin, eta_bin, vtxz_bin),
                        new TH1D(Form("h1D_DeltaPhi_Mbin%d_Eta%d_VtxZ%d", Mbin, eta_bin, vtxz_bin), Form("h1D_DeltaPhi_Mbin%d_Eta%d_VtxZ%d;Pair #Delta#phi [radian];Entries", Mbin, eta_bin, vtxz_bin), nDeltaPhiBin, DeltaPhiEdge_min, DeltaPhiEdge_max)
                    )
                ).second; insert_check.push_back(isInserted);

                isInserted = h1D_map.insert( std::make_pair(
                        Form("h1D_DeltaPhi_Mbin%d_Eta%d_VtxZ%d_rotated", Mbin, eta_bin, vtxz_bin),
                        new TH1D(Form("h1D_DeltaPhi_Mbin%d_Eta%d_VtxZ%d_rotated", Mbin, eta_bin, vtxz_bin), Form("h1D_DeltaPhi_Mbin%d_Eta%d_VtxZ%d_rotated;Pair #Delta#phi [radian];Entries", Mbin, eta_bin, vtxz_bin), nDeltaPhiBin, DeltaPhiEdge_min, DeltaPhiEdge_max)
                    )
                ).second; insert_check.push_back(isInserted);
                


                isInserted = h1D_map.insert( std::make_pair(
                        Form("h1D_typeA_DeltaPhi_Mbin%d_Eta%d_VtxZ%d", Mbin, eta_bin, vtxz_bin),
                        new TH1D(Form("h1D_typeA_DeltaPhi_Mbin%d_Eta%d_VtxZ%d", Mbin, eta_bin, vtxz_bin), Form("h1D_typeA_DeltaPhi_Mbin%d_Eta%d_VtxZ%d;Pair #Delta#phi [radian] (type A);Entries", Mbin, eta_bin, vtxz_bin), nDeltaPhiBin, DeltaPhiEdge_min, DeltaPhiEdge_max)
                    )
                ).second; insert_check.push_back(isInserted);

                isInserted = h1D_map.insert( std::make_pair(
                        Form("h1D_typeA_DeltaPhi_Mbin%d_Eta%d_VtxZ%d_rotated", Mbin, eta_bin, vtxz_bin),
                        new TH1D(Form("h1D_typeA_DeltaPhi_Mbin%d_Eta%d_VtxZ%d_rotated", Mbin, eta_bin, vtxz_bin), Form("h1D_typeA_DeltaPhi_Mbin%d_Eta%d_VtxZ%d_rotated;Pair #Delta#phi [radian] (type A);Entries", Mbin, eta_bin, vtxz_bin), nDeltaPhiBin, DeltaPhiEdge_min, DeltaPhiEdge_max)
                    )
                ).second; insert_check.push_back(isInserted);
                


                // note : for truth eta
                if (runnumber == -1 && eta_bin == 0){
                    isInserted = h1D_map.insert( std::make_pair(
                            Form("h1D_TrueEta_Mbin%d_VtxZ%d", Mbin, vtxz_bin),
                            new TH1D(Form("h1D_TrueEta_Mbin%d_VtxZ%d", Mbin, vtxz_bin), Form("h1D_TrueEta_Mbin%d_VtxZ%d;PHG4Particle #eta;Entries", Mbin, vtxz_bin), nEtaBin, EtaEdge_min, EtaEdge_max)
                        )
                    ).second; insert_check.push_back(isInserted);
                }
            } // note : end Mbin
        }
    }// note : end of Delta_Phi hist

    // Division : ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    // note : for the best pairs
    for (int Mbin = 0; Mbin < nCentrality_bin; Mbin++)
    {
        for (int vtxz_bin = 0; vtxz_bin < nVtxZBin; vtxz_bin++){

            isInserted = h1D_map.insert( std::make_pair(
                    Form("h1D_BestPair_DeltaEta_Mbin%d_VtxZ%d",Mbin, vtxz_bin),
                    new TH1D(Form("h1D_BestPair_DeltaEta_Mbin%d_VtxZ%d",Mbin, vtxz_bin), Form("h1D_BestPair_DeltaEta_Mbin%d_VtxZ%d;Pair #Delta#eta;Entries",Mbin, vtxz_bin), nDeltaEtaBin, DeltaEtaEdge_min, DeltaEtaEdge_max)
                )
            ).second; insert_check.push_back(isInserted);

            isInserted = h1D_map.insert( std::make_pair(
                    Form("h1D_BestPair_DeltaPhi_Mbin%d_VtxZ%d",Mbin, vtxz_bin),
                    new TH1D(Form("h1D_BestPair_DeltaPhi_Mbin%d_VtxZ%d",Mbin, vtxz_bin), Form("h1D_BestPair_DeltaPhi_Mbin%d_VtxZ%d;Pair #Delta#phi [radian];Entries",Mbin, vtxz_bin), nDeltaPhiBin, DeltaPhiEdge_min, DeltaPhiEdge_max)
                )
            ).second; insert_check.push_back(isInserted);

            isInserted = h1D_map.insert( std::make_pair(
                    Form("h1D_BestPair_ClusPhiSize_Mbin%d_VtxZ%d",Mbin, vtxz_bin),
                    new TH1D(Form("h1D_BestPair_ClusPhiSize_Mbin%d_VtxZ%d",Mbin, vtxz_bin), Form("h1D_BestPair_ClusPhiSize_Mbin%d_VtxZ%d;ClusPhiSize;Entries",Mbin, vtxz_bin), 128,0,128)
                )
            ).second; insert_check.push_back(isInserted);

            isInserted = h1D_map.insert( std::make_pair(
                    Form("h1D_BestPair_ClusAdc_Mbin%d_VtxZ%d",Mbin, vtxz_bin),
                    new TH1D(Form("h1D_BestPair_ClusAdc_Mbin%d_VtxZ%d",Mbin, vtxz_bin), Form("h1D_BestPair_ClusAdc_Mbin%d_VtxZ%d;ClusAdc;Entries",Mbin, vtxz_bin), 200,0,18000)
                )
            ).second; insert_check.push_back(isInserted);

            
            // note : typeA : ------------------------------------------------------------------------------------------------------------------------------------------------------------------
            isInserted = h1D_map.insert( std::make_pair(
                    Form("h1D_typeA_BestPair_DeltaEta_Mbin%d_VtxZ%d",Mbin, vtxz_bin),
                    new TH1D(Form("h1D_typeA_BestPair_DeltaEta_Mbin%d_VtxZ%d",Mbin, vtxz_bin), Form("h1D_typeA_BestPair_DeltaEta_Mbin%d_VtxZ%d;Pair #Delta#eta (type A);Entries",Mbin, vtxz_bin), nDeltaEtaBin, DeltaEtaEdge_min, DeltaEtaEdge_max)
                )
            ).second; insert_check.push_back(isInserted);

            isInserted = h1D_map.insert( std::make_pair(
                    Form("h1D_typeA_BestPair_DeltaPhi_Mbin%d_VtxZ%d",Mbin, vtxz_bin),
                    new TH1D(Form("h1D_typeA_BestPair_DeltaPhi_Mbin%d_VtxZ%d",Mbin, vtxz_bin), Form("h1D_typeA_BestPair_DeltaPhi_Mbin%d_VtxZ%d;Pair #Delta#phi [radian] (type A);Entries",Mbin, vtxz_bin), nDeltaPhiBin, DeltaPhiEdge_min, DeltaPhiEdge_max)
                )
            ).second; insert_check.push_back(isInserted);

            isInserted = h1D_map.insert( std::make_pair(
                    Form("h1D_typeA_BestPair_ClusPhiSize_Mbin%d_VtxZ%d",Mbin, vtxz_bin),
                    new TH1D(Form("h1D_typeA_BestPair_ClusPhiSize_Mbin%d_VtxZ%d",Mbin, vtxz_bin), Form("h1D_typeA_BestPair_ClusPhiSize_Mbin%d_VtxZ%d;ClusPhiSize (type A);Entries",Mbin, vtxz_bin), 128,0,128)
                )
            ).second; insert_check.push_back(isInserted);

            isInserted = h1D_map.insert( std::make_pair(
                    Form("h1D_typeA_BestPair_ClusAdc_Mbin%d_VtxZ%d",Mbin, vtxz_bin),
                    new TH1D(Form("h1D_typeA_BestPair_ClusAdc_Mbin%d_VtxZ%d",Mbin, vtxz_bin), Form("h1D_typeA_BestPair_ClusAdc_Mbin%d_VtxZ%d;ClusAdc (type A);Entries",Mbin, vtxz_bin), 200,0,18000)
                )
            ).second; insert_check.push_back(isInserted);
        }

    }


    // Division : ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    // note : for 2D
    h2D_map.clear();

    // note : truth nPHG4Particle in Eta-VtxZ
    if (runnumber == -1){
        for (int Mbin = 0; Mbin < nCentrality_bin; Mbin++)
        {
            isInserted = h2D_map.insert( std::make_pair(
                    Form("h2D_TrueEtaVtxZ_Mbin%d", Mbin),
                    new TH2D(Form("h2D_TrueEtaVtxZ_Mbin%d", Mbin), Form("h2D_TrueEtaVtxZ_Mbin%d;PHG4Particle #eta;TruthPV_trig_z [cm]", Mbin), nEtaBin, EtaEdge_min, EtaEdge_max, nVtxZBin, VtxZEdge_min, VtxZEdge_max)
                )
            ).second; insert_check.push_back(isInserted);

            isInserted = h2D_map.insert( std::make_pair(
                    Form("h2D_TrueEtaVtxZ_Mbin%d_FineBin", Mbin),
                    new TH2D(Form("h2D_TrueEtaVtxZ_Mbin%d_FineBin", Mbin), Form("h2D_TrueEtaVtxZ_Mbin%d_FineBin;PHG4Particle #eta;TruthPV_trig_z [cm]", Mbin), 540, EtaEdge_min, EtaEdge_max, nVtxZBin_FineBin, VtxZEdge_min, VtxZEdge_max)
                )
            ).second; insert_check.push_back(isInserted);
        }

        isInserted = h1D_map.insert( std::make_pair(
                "debug_h1D_NHadron_Inclusive100",
                new TH1D("debug_h1D_NHadron_Inclusive100", "debug_h1D_NHadron_Inclusive100;N Hadrons;Entries", 200, 0, 10000)
            )
        ).second; insert_check.push_back(isInserted);

        isInserted = h1D_map.insert( std::make_pair(
                "debug_h1D_NHadron_OneEtaBin_Inclusive100",
                new TH1D("debug_h1D_NHadron_OneEtaBin_Inclusive100", "debug_h1D_NHadron_OneEtaBin_Inclusive100;N Hadrons;Entries", 200, 0, 400)
            )
        ).second; insert_check.push_back(isInserted);

        isInserted = h2D_map.insert( std::make_pair(
                Form("h2D_TrueEvtCount_vtxZCentrality"),
                new TH2D(Form("h2D_TrueEvtCount_vtxZCentrality"), Form("h2D_TrueEvtCount_vtxZCentrality;TruthPV_trig_z [cm];Centrality [%]"), nVtxZBin, VtxZEdge_min, VtxZEdge_max, nCentrality_bin, &centrality_edges[0]) // note : 0 - 5%
            )
        ).second; insert_check.push_back(isInserted);
    }

    // Division : ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    // note : for good proto tracklet in Eta-VtxZ
    for (int Mbin = 0; Mbin < nCentrality_bin; Mbin++){
        
        isInserted = h2D_map.insert( std::make_pair(
                Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d", Mbin),
                new TH2D(Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d", Mbin), Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d;Pair #eta;INTT vtxZ [cm]", Mbin), nEtaBin, EtaEdge_min, EtaEdge_max, nVtxZBin, VtxZEdge_min, VtxZEdge_max) 
            )
        ).second; insert_check.push_back(isInserted);

        isInserted = h2D_map.insert( std::make_pair(
                Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d", Mbin),
                new TH2D(Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d", Mbin), Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d;Pair #eta (type A);INTT vtxZ [cm]", Mbin), nEtaBin, EtaEdge_min, EtaEdge_max, nVtxZBin, VtxZEdge_min, VtxZEdge_max) 
            )
        ).second; insert_check.push_back(isInserted);

        // note : normal fine bin
        isInserted = h2D_map.insert( std::make_pair(
                Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d_FineBin", Mbin),
                new TH2D(Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d_FineBin", Mbin), Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d_FineBin;Pair #eta;INTT vtxZ [cm]", Mbin), 540, EtaEdge_min, EtaEdge_max, nVtxZBin_FineBin, VtxZEdge_min, VtxZEdge_max) 
            )
        ).second; insert_check.push_back(isInserted);

        isInserted = h2D_map.insert( std::make_pair(
                Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d_FineBin", Mbin),
                new TH2D(Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d_FineBin", Mbin), Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d_FineBin;Pair #eta (type A);INTT vtxZ [cm]", Mbin), 540, EtaEdge_min, EtaEdge_max, nVtxZBin_FineBin, VtxZEdge_min, VtxZEdge_max) 
            )
        ).second; insert_check.push_back(isInserted);
        

        // note : rotated
        isInserted = h2D_map.insert( std::make_pair(
                Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d_rotated", Mbin),
                new TH2D(Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d_rotated", Mbin), Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d_rotated;Pair #eta;INTT vtxZ [cm]", Mbin), nEtaBin, EtaEdge_min, EtaEdge_max, nVtxZBin, VtxZEdge_min, VtxZEdge_max) 
            )
        ).second; insert_check.push_back(isInserted);

        isInserted = h2D_map.insert( std::make_pair(
                Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d_rotated", Mbin),
                new TH2D(Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d_rotated", Mbin), Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d_rotated;Pair #eta (type A);INTT vtxZ [cm]", Mbin), nEtaBin, EtaEdge_min, EtaEdge_max, nVtxZBin, VtxZEdge_min, VtxZEdge_max) 
            )
        ).second; insert_check.push_back(isInserted);

        // note : FineBin
        isInserted = h2D_map.insert( std::make_pair(
                Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d_rotated_FineBin", Mbin),
                new TH2D(Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d_rotated_FineBin", Mbin), Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d_rotated_FineBin;Pair #eta;INTT vtxZ [cm]", Mbin), 540, EtaEdge_min, EtaEdge_max, nVtxZBin_FineBin, VtxZEdge_min, VtxZEdge_max) 
            )
        ).second; insert_check.push_back(isInserted);

        isInserted = h2D_map.insert( std::make_pair(
                Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d_rotated_FineBin", Mbin),
                new TH2D(Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d_rotated_FineBin", Mbin), Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d_rotated_FineBin;Pair #eta (type A);INTT vtxZ [cm]", Mbin), 540, EtaEdge_min, EtaEdge_max, nVtxZBin_FineBin, VtxZEdge_min, VtxZEdge_max) 
            )
        ).second; insert_check.push_back(isInserted);
    }


    isInserted = h2D_map.insert( std::make_pair(
            Form("h2D_RecoEvtCount_vtxZCentrality"),
            new TH2D(Form("h2D_RecoEvtCount_vtxZCentrality"), Form("h2D_RecoEvtCount_vtxZCentrality;INTT vtxZ [cm];Centrality [%]"), nVtxZBin, VtxZEdge_min, VtxZEdge_max, nCentrality_bin, &centrality_edges[0]) // note : 0 - 5%
        )
    ).second; insert_check.push_back(isInserted);
    
    // note : best pair in Eta-VtxZ
    for (int Mbin = 0; Mbin < nCentrality_bin; Mbin++){
        isInserted = h2D_map.insert( std::make_pair(
                Form("h2D_BestPairEtaVtxZ_Mbin%d",Mbin),
                new TH2D(Form("h2D_BestPairEtaVtxZ_Mbin%d",Mbin), Form("h2D_BestPairEtaVtxZ_Mbin%d;Tracklet #eta;INTT vtxZ [cm]",Mbin), nEtaBin, EtaEdge_min, EtaEdge_max, nVtxZBin, VtxZEdge_min, VtxZEdge_max)
            )
        ).second; insert_check.push_back(isInserted);

        isInserted = h2D_map.insert( std::make_pair(
                Form("h2D_BestPairEtaVtxZ_Mbin%d_FineBin",Mbin),
                new TH2D(Form("h2D_BestPairEtaVtxZ_Mbin%d_FineBin",Mbin), Form("h2D_BestPairEtaVtxZ_Mbin%d_FineBin;Tracklet #eta;INTT vtxZ [cm]",Mbin), 540, EtaEdge_min, EtaEdge_max, nVtxZBin_FineBin, VtxZEdge_min, VtxZEdge_max)
            )
        ).second; insert_check.push_back(isInserted);

        isInserted = h2D_map.insert( std::make_pair(
                Form("h2D_typeA_BestPairEtaVtxZ_Mbin%d",Mbin),
                new TH2D(Form("h2D_typeA_BestPairEtaVtxZ_Mbin%d",Mbin), Form("h2D_typeA_BestPairEtaVtxZ_Mbin%d;Tracklet #eta (type A);INTT vtxZ [cm]",Mbin), nEtaBin, EtaEdge_min, EtaEdge_max, nVtxZBin, VtxZEdge_min, VtxZEdge_max)
            )
        ).second; insert_check.push_back(isInserted);

        isInserted = h2D_map.insert( std::make_pair(
                Form("h2D_typeA_BestPairEtaVtxZ_Mbin%d_FineBin",Mbin),
                new TH2D(Form("h2D_typeA_BestPairEtaVtxZ_Mbin%d_FineBin",Mbin), Form("h2D_typeA_BestPairEtaVtxZ_Mbin%d_FineBin;Tracklet #eta (type A);INTT vtxZ [cm]",Mbin), 540, EtaEdge_min, EtaEdge_max, nVtxZBin_FineBin, VtxZEdge_min, VtxZEdge_max)
            )
        ).second; insert_check.push_back(isInserted);
    }

    


    // note : vtxZ - centrality
    isInserted = h2D_map.insert( std::make_pair(
            Form("h2D_InttVtxZ_Centrality"),
            new TH2D(Form("h2D_InttVtxZ_Centrality"), Form("h2D_InttVtxZ_Centrality;INTT vtxZ [cm];Centrality"), nVtxZBin, VtxZEdge_min, VtxZEdge_max, nCentralityFineBin, CentralityFineEdge_min, CentralityFineEdge_max)
        )
    ).second; insert_check.push_back(isInserted);

    // isInserted = h2D_map.insert( std::make_pair(
    //         Form("h2D_MBDVtxZ_Centrality"),
    //         new TH2D(Form("h2D_MBDVtxZ_Centrality"), Form("h2D_MBDVtxZ_Centrality;INTT vtxZ [cm];Centrality"), nVtxZBin, VtxZEdge_min, VtxZEdge_max, nCentralityFineBin, CentralityFineEdge_min, CentralityFineEdge_max)
    //     )
    // ).second; insert_check.push_back(isInserted);
    

    for (int isInserted_tag : insert_check){
        if (isInserted_tag == 0){
            std::cout<<"Histogram insertion failed"<<std::endl;
            exit(1);
        }
    }
}

void TrackletHistogramNew::EvtCleanUp()
{
    Used_Clus_index_vec.clear();
    Pair_DeltaPhi_vec.clear();

    evt_sPH_inner_nocolumn_vec.clear();
    evt_sPH_outer_nocolumn_vec.clear();

    evt_TrackletPair_vec.clear();
    evt_TrackletPairRotate_vec.clear();

    inner_clu_phi_map.clear();
    outer_clu_phi_map.clear();
    inner_clu_phi_map = std::vector<std::vector<std::pair<bool,ClusHistogram::clu_info>>>(360);
    outer_clu_phi_map = std::vector<std::vector<std::pair<bool,ClusHistogram::clu_info>>>(360);
}

void TrackletHistogramNew::GetTrackletPair(std::vector<pair_str> &input_TrackletPair_vec, bool isRotated)
{
    input_TrackletPair_vec.clear();

    inner_clu_phi_map.clear();
    outer_clu_phi_map.clear();
    inner_clu_phi_map = std::vector<std::vector<std::pair<bool,ClusHistogram::clu_info>>>(360);
    outer_clu_phi_map = std::vector<std::vector<std::pair<bool,ClusHistogram::clu_info>>>(360);

    if (INTTvtxZ != INTTvtxZ || INTTvtxZError != INTTvtxZError) {return;}

    std::vector<ClusHistogram::clu_info> temp_evt_sPH_inner_nocolumn_vec = (isRotated) ? GetRotatedClusterVec(evt_sPH_inner_nocolumn_vec) : evt_sPH_inner_nocolumn_vec;

    for (int inner_i = 0; inner_i < int(temp_evt_sPH_inner_nocolumn_vec.size()); inner_i++) {
      double Clus_InnerPhi_Offset = (temp_evt_sPH_inner_nocolumn_vec[inner_i].y - vertexXYIncm.second < 0) ? atan2(temp_evt_sPH_inner_nocolumn_vec[inner_i].y - vertexXYIncm.second, temp_evt_sPH_inner_nocolumn_vec[inner_i].x - vertexXYIncm.first) * (180./TMath::Pi()) + 360 : atan2(temp_evt_sPH_inner_nocolumn_vec[inner_i].y - vertexXYIncm.second, temp_evt_sPH_inner_nocolumn_vec[inner_i].x - vertexXYIncm.first) * (180./TMath::Pi());
      inner_clu_phi_map[ int(Clus_InnerPhi_Offset) ].push_back({false,temp_evt_sPH_inner_nocolumn_vec[inner_i]});
    }
    for (int outer_i = 0; outer_i < int(evt_sPH_outer_nocolumn_vec.size()); outer_i++) {
        double Clus_OuterPhi_Offset = (evt_sPH_outer_nocolumn_vec[outer_i].y - vertexXYIncm.second < 0) ? atan2(evt_sPH_outer_nocolumn_vec[outer_i].y - vertexXYIncm.second, evt_sPH_outer_nocolumn_vec[outer_i].x - vertexXYIncm.first) * (180./TMath::Pi()) + 360 : atan2(evt_sPH_outer_nocolumn_vec[outer_i].y - vertexXYIncm.second, evt_sPH_outer_nocolumn_vec[outer_i].x - vertexXYIncm.first) * (180./TMath::Pi());
        outer_clu_phi_map[ int(Clus_OuterPhi_Offset) ].push_back({false,evt_sPH_outer_nocolumn_vec[outer_i]});
    }


    for (int inner_phi_i = 0; inner_phi_i < 360; inner_phi_i++) // note : each phi cell (1 degree)
    {
        // note : N cluster in this phi cell
        for (int inner_phi_clu_i = 0; inner_phi_clu_i < inner_clu_phi_map[inner_phi_i].size(); inner_phi_clu_i++)
        {
            if (inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].first == true) {continue;}

            ClusHistogram::clu_info inner_clu = inner_clu_phi_map[inner_phi_i][inner_phi_clu_i].second;

            double Clus_InnerPhi_Offset_radian = atan2(inner_clu.y - vertexXYIncm.second, inner_clu.x - vertexXYIncm.first);
            double Clus_InnerPhi_Offset = (inner_clu.y - vertexXYIncm.second < 0) ? Clus_InnerPhi_Offset_radian * (180./TMath::Pi()) + 360 : Clus_InnerPhi_Offset_radian * (180./TMath::Pi());

            // todo: change the outer phi scan range
            // note : the outer phi index, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5
            for (int scan_i = -5; scan_i < 6; scan_i++)
            {
                int true_scan_i = ((inner_phi_i + scan_i) < 0) ? 360 + (inner_phi_i + scan_i) : ((inner_phi_i + scan_i) > 359) ? (inner_phi_i + scan_i)-360 : inner_phi_i + scan_i;

                // note : N clusters in that outer phi cell
                for (int outer_phi_clu_i = 0; outer_phi_clu_i < outer_clu_phi_map[true_scan_i].size(); outer_phi_clu_i++)
                {
                    if (outer_clu_phi_map[true_scan_i][outer_phi_clu_i].first == true) {continue;}

                    ClusHistogram::clu_info outer_clu = outer_clu_phi_map[true_scan_i][outer_phi_clu_i].second;

                    double Clus_OuterPhi_Offset_radian = atan2(outer_clu.y - vertexXYIncm.second, outer_clu.x - vertexXYIncm.first);
                    double Clus_OuterPhi_Offset = (outer_clu.y - vertexXYIncm.second < 0) ? Clus_OuterPhi_Offset_radian * (180./TMath::Pi()) + 360 : Clus_OuterPhi_Offset_radian * (180./TMath::Pi());
                    double delta_phi = get_delta_phi(Clus_InnerPhi_Offset, Clus_OuterPhi_Offset);
                    // note : degree to radian 
                    double delta_phi_radian = delta_phi * (TMath::Pi() / 180.);
                    
                    // if (fabs(delta_phi) > 5.72) {continue;}
                    // if (fabs(delta_phi) > 3.5) {continue;}

                    double inner_clu_eta = get_clu_eta({vertexXYIncm.first, vertexXYIncm.second, INTTvtxZ},{inner_clu.x, inner_clu.y, inner_clu.z});
                    double outer_clu_eta = get_clu_eta({vertexXYIncm.first, vertexXYIncm.second, INTTvtxZ},{outer_clu.x, outer_clu.y, outer_clu.z});
                    double delta_eta = inner_clu_eta - outer_clu_eta;

                    std::pair<double,double> z_range_info = Get_possible_zvtx( 
                        0., // get_radius(vertexXYIncm.first,vertexXYIncm.second), 
                        {
                            get_radius(inner_clu.x - vertexXYIncm.first, inner_clu.y - vertexXYIncm.second), 
                            inner_clu.z,
                            double(inner_clu.sensorZID)
                        }, // note : unsign radius
                        
                        {
                            get_radius(outer_clu.x - vertexXYIncm.first, outer_clu.y - vertexXYIncm.second), 
                            outer_clu.z,
                            double(outer_clu.sensorZID)
                        }  // note : unsign radius
                    );

                    // note : this is a cut to constraint on the z vertex, only if the tracklets with the range that covers the z vertex can pass this cut
                    if (z_range_info.first - z_range_info.second > INTTvtxZ + INTTvtxZError || z_range_info.first + z_range_info.second < INTTvtxZ - INTTvtxZError) {continue;}

                    // note : do the fill here (find the best match outer cluster with the inner cluster )
                    // std::pair<double,double> Get_eta_pair = Get_eta(
                    //     {0., INTTvtxZ,INTTvtxZError},
                    //     {
                    //         get_radius(inner_clu.x - vertexXYIncm.first, inner_clu.y - vertexXYIncm.second), 
                    //         inner_clu.z,
                    //         double(inner_clu.sensorZID)
                    //     },
                    //     {
                    //         get_radius(outer_clu.x - vertexXYIncm.first, outer_clu.y - vertexXYIncm.second), 
                    //         outer_clu.z,
                    //         double(outer_clu.sensorZID)
                    //     }
                    // );  

                    // if (isRotated == false && fabs(delta_phi_radian) < 0.021 && INTTvtxZ > 30 && Get_eta_pair.second > 0){
                    //     std::cout<<"Check, vertex: {"<<vertexXYIncm.first<<", "<<vertexXYIncm.second<<", "<<INTTvtxZ<<"}, INTTvtxZError: "<<INTTvtxZError<<std::endl;
                    //     std::cout<<"Inner cluster Pos: {"<<inner_clu.x<<", "<<inner_clu.y<<", "<<inner_clu.z<<"}, eta_INTTz: "<<inner_clu_eta<<", sensorID: "<<inner_clu.sensorZID<<std::endl;
                    //     std::cout<<"Outer cluster Pos: {"<<outer_clu.x<<", "<<outer_clu.y<<", "<<outer_clu.z<<"}, eta_INTTz: "<<outer_clu_eta<<", sensorID: "<<outer_clu.sensorZID<<std::endl;
                    //     std::cout<<"DeltaPhi: "<<delta_phi_radian<<", DeltaEta: "<<delta_eta<<", PairEta: "<<(inner_clu_eta + outer_clu_eta) / 2.<<", PairEtaFit: "<<Get_eta_pair.second<<std::endl;
                    //     std::cout<<std::endl;
                    // }

                    pair_str temp_pair_str;
                    temp_pair_str.delta_phi = delta_phi_radian;
                    temp_pair_str.delta_eta = delta_eta;
                    temp_pair_str.pair_eta_num = (inner_clu_eta + outer_clu_eta) / 2.;
                    temp_pair_str.pair_eta_fit = std::nan("");//Get_eta_pair.second; // todo: Cancel the eta fitting

                    // temp_pair_str.inner_phi_id = inner_clu.ladderPhiID;
                    // temp_pair_str.inner_layer_id = inner_clu.layerID;
                    temp_pair_str.inner_zid = inner_clu.sensorZID;
                    temp_pair_str.inner_phi_size = inner_clu.phi_size;
                    temp_pair_str.inner_adc = inner_clu.adc;
                    temp_pair_str.inner_x = inner_clu.x;
                    temp_pair_str.inner_y = inner_clu.y;
                    // temp_pair_str.inner_z = inner_clu.z;
                    // temp_pair_str.inner_phi = Clus_InnerPhi_Offset_radian;
                    // temp_pair_str.inner_eta = inner_clu_eta;
                    temp_pair_str.inner_index = inner_clu.index;
                    
                    // temp_pair_str.outer_phi_id = outer_clu.ladderPhiID;
                    // temp_pair_str.outer_layer_id = outer_clu.layerID;
                    temp_pair_str.outer_zid = outer_clu.sensorZID;
                    temp_pair_str.outer_phi_size = outer_clu.phi_size;
                    temp_pair_str.outer_adc = outer_clu.adc;
                    temp_pair_str.outer_x = outer_clu.x;
                    temp_pair_str.outer_y = outer_clu.y;
                    // temp_pair_str.outer_z = outer_clu.z;
                    // temp_pair_str.outer_phi = Clus_OuterPhi_Offset_radian;
                    // temp_pair_str.outer_eta = outer_clu_eta;
                    temp_pair_str.outer_index = outer_clu.index;

                    input_TrackletPair_vec.push_back(temp_pair_str);

                }
            }
        }
    }


}

void TrackletHistogramNew::FillPairs(std::vector<pair_str> input_TrackletPair_vec, bool isRotated, int Mbin_in, int vtxz_bin_in, double vtxZ_weight_in, int eID_in)
{
    Pair_DeltaPhi_vec.clear();
    Used_Clus_index_vec.clear();

    std::string rotated_text = (isRotated) ? "_rotated" : "";

    for (int pair_i = 0; pair_i < input_TrackletPair_vec.size(); pair_i++) // note : either non-rotated or rotated
    {
        pair_str this_pair = input_TrackletPair_vec[pair_i];

        if (isRotated == false) {Pair_DeltaPhi_vec.push_back(fabs(this_pair.delta_phi));}

        int eta_bin = h1D_eta_template -> Fill(this_pair.pair_eta_num);
        eta_bin = (eta_bin == -1) ? -1 : eta_bin - 1;

        if (eta_bin == -1) {continue;}

        if (isRotated == false){
            h1D_PairDeltaEta_inclusive -> Fill(this_pair.delta_eta, vtxZ_weight_in);
            h1D_PairDeltaPhi_inclusive -> Fill(this_pair.delta_phi, vtxZ_weight_in);
        }

        // note : for both rotated and non-rotated
        h1D_map[Form("h1D_DeltaPhi_Mbin%d_Eta%d_VtxZ%d%s", Mbin_in, eta_bin, vtxz_bin_in, rotated_text.c_str())] -> Fill(this_pair.delta_phi, vtxZ_weight_in);

        if (this_pair.delta_phi > cut_GoodProtoTracklet_DeltaPhi.first && this_pair.delta_phi < cut_GoodProtoTracklet_DeltaPhi.second)
        {
            h2D_map[Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d%s", Mbin_in, rotated_text.c_str())] -> Fill(this_pair.pair_eta_num, INTTvtxZ, vtxZ_weight_in);
            h2D_map[Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d%s_FineBin", Mbin_in, rotated_text.c_str())] -> Fill(this_pair.pair_eta_num, INTTvtxZ, vtxZ_weight_in);
        }
        // note : for both rotated and non-rotated

        // Division : ---type A---------------------------------------------------------------------------------------------------------------------------------------------------------------------    
        // if (ClusLadderZId->at(this_pair.inner_index) != typeA_sensorZID[0] && ClusLadderZId->at(this_pair.inner_index) != typeA_sensorZID[1]) {continue;}
        // if (ClusLadderZId->at(this_pair.outer_index) != typeA_sensorZID[0] && ClusLadderZId->at(this_pair.outer_index) != typeA_sensorZID[1]) {continue;}
        if (this_pair.inner_zid != typeA_sensorZID[0] && this_pair.inner_zid != typeA_sensorZID[1]) {continue;}
        if (this_pair.outer_zid != typeA_sensorZID[0] && this_pair.outer_zid != typeA_sensorZID[1]) {continue;}

        // note : for both rotated and non-rotated
        h1D_map[Form("h1D_typeA_DeltaPhi_Mbin%d_Eta%d_VtxZ%d%s", Mbin_in, eta_bin, vtxz_bin_in, rotated_text.c_str())] -> Fill(this_pair.delta_phi, vtxZ_weight_in);

        if (this_pair.delta_phi > cut_GoodProtoTracklet_DeltaPhi.first && this_pair.delta_phi < cut_GoodProtoTracklet_DeltaPhi.second)
        {
            h2D_map[Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d%s", Mbin_in, rotated_text.c_str())] -> Fill(this_pair.pair_eta_num, INTTvtxZ, vtxZ_weight_in);
            h2D_map[Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d%s_FineBin", Mbin_in, rotated_text.c_str())] -> Fill(this_pair.pair_eta_num, INTTvtxZ, vtxZ_weight_in);
        }
        // note : for both rotated and non-rotated
    }

    if (isRotated == false){
        if (Pair_DeltaPhi_vec.size() != input_TrackletPair_vec.size()){
            std::cout<<"Something wrong: Pair_DeltaPhi_vec.size() != input_TrackletPair_vec.size()"<<std::endl;
            exit(1);
        }

        long long vec_size = Pair_DeltaPhi_vec.size();
        long long ind[Pair_DeltaPhi_vec.size()];
        TMath::Sort(vec_size, &Pair_DeltaPhi_vec[0], ind, false);

        for (int pair_i = 0; pair_i < vec_size; pair_i++){

            pair_str pair = input_TrackletPair_vec[ind[pair_i]];
            if (std::find(Used_Clus_index_vec.begin(), Used_Clus_index_vec.end(), pair.inner_index) != Used_Clus_index_vec.end()) {continue;}
            if (std::find(Used_Clus_index_vec.begin(), Used_Clus_index_vec.end(), pair.outer_index) != Used_Clus_index_vec.end()) {continue;}

            if (eID_in % 250 == 0){
                std::cout<<"Event ID : "<<eID_in<<" Mbin : "<<Mbin_in<<" pair_i : "<<pair_i<<", ind[pair_i] : "<<ind[pair_i]<<", pair.delta_phi : "<<pair.delta_phi<<" pair.delta_eta : "<<pair.delta_eta<<std::endl;
            }

            if (Pair_DeltaPhi_vec[ind[pair_i]] > cut_bestPair_DeltaPhi.second) { break; }

            h1D_map[Form("h1D_BestPair_DeltaEta_Mbin%d_VtxZ%d",Mbin_in,vtxz_bin_in)] -> Fill(pair.delta_eta, vtxZ_weight_in);
            h1D_map[Form("h1D_BestPair_DeltaPhi_Mbin%d_VtxZ%d",Mbin_in,vtxz_bin_in)] -> Fill(pair.delta_phi, vtxZ_weight_in);

            h2D_map[Form("h2D_BestPairEtaVtxZ_Mbin%d",Mbin_in)] -> Fill(pair.pair_eta_num, INTTvtxZ, vtxZ_weight_in);
            h2D_map[Form("h2D_BestPairEtaVtxZ_Mbin%d_FineBin",Mbin_in)] -> Fill(pair.pair_eta_num, INTTvtxZ, vtxZ_weight_in);

            h1D_map[Form("h1D_BestPair_ClusPhiSize_Mbin%d_VtxZ%d",Mbin_in,vtxz_bin_in)] -> Fill(pair.inner_phi_size, vtxZ_weight_in);
            h1D_map[Form("h1D_BestPair_ClusPhiSize_Mbin%d_VtxZ%d",Mbin_in,vtxz_bin_in)] -> Fill(pair.outer_phi_size, vtxZ_weight_in);
            h1D_map[Form("h1D_BestPair_ClusAdc_Mbin%d_VtxZ%d",Mbin_in,vtxz_bin_in)] -> Fill(pair.inner_adc, vtxZ_weight_in);
            h1D_map[Form("h1D_BestPair_ClusAdc_Mbin%d_VtxZ%d",Mbin_in,vtxz_bin_in)] -> Fill(pair.outer_adc, vtxZ_weight_in);

            // note : typeA
            if (
                // (ClusLadderZId->at(pair.inner_index) == typeA_sensorZID[0] || ClusLadderZId->at(pair.inner_index) == typeA_sensorZID[1]) &&
                // (ClusLadderZId->at(pair.outer_index) == typeA_sensorZID[0] || ClusLadderZId->at(pair.outer_index) == typeA_sensorZID[1])
                
                (pair.inner_zid == typeA_sensorZID[0] || pair.inner_zid == typeA_sensorZID[1]) &&
                (pair.outer_zid == typeA_sensorZID[0] || pair.outer_zid == typeA_sensorZID[1])

            ){
                h1D_map[Form("h1D_typeA_BestPair_DeltaEta_Mbin%d_VtxZ%d",Mbin_in,vtxz_bin_in)] -> Fill(pair.delta_eta, vtxZ_weight_in);
                h1D_map[Form("h1D_typeA_BestPair_DeltaPhi_Mbin%d_VtxZ%d",Mbin_in,vtxz_bin_in)] -> Fill(pair.delta_phi, vtxZ_weight_in);

                h2D_map[Form("h2D_typeA_BestPairEtaVtxZ_Mbin%d",Mbin_in)] -> Fill(pair.pair_eta_num, INTTvtxZ, vtxZ_weight_in);
                h2D_map[Form("h2D_typeA_BestPairEtaVtxZ_Mbin%d_FineBin",Mbin_in)] -> Fill(pair.pair_eta_num, INTTvtxZ, vtxZ_weight_in);

                h1D_map[Form("h1D_typeA_BestPair_ClusPhiSize_Mbin%d_VtxZ%d",Mbin_in,vtxz_bin_in)] -> Fill(pair.inner_phi_size, vtxZ_weight_in);
                h1D_map[Form("h1D_typeA_BestPair_ClusPhiSize_Mbin%d_VtxZ%d",Mbin_in,vtxz_bin_in)] -> Fill(pair.outer_phi_size, vtxZ_weight_in);
                h1D_map[Form("h1D_typeA_BestPair_ClusAdc_Mbin%d_VtxZ%d",Mbin_in,vtxz_bin_in)] -> Fill(pair.inner_adc, vtxZ_weight_in);
                h1D_map[Form("h1D_typeA_BestPair_ClusAdc_Mbin%d_VtxZ%d",Mbin_in,vtxz_bin_in)] -> Fill(pair.outer_adc, vtxZ_weight_in);
            }


            Used_Clus_index_vec.push_back(pair.inner_index);
            Used_Clus_index_vec.push_back(pair.outer_index);
        }

    }

    

    // h1D_PairDeltaEta_inclusive
    // h1D_PairDeltaPhi_inclusive

    // Form("h1D_DeltaPhi_Mbin%d_Eta%d_VtxZ%d", Mbin, eta_bin, vtxz_bin)
    // Form("h1D_typeA_DeltaPhi_Mbin%d_Eta%d_VtxZ%d", Mbin, eta_bin, vtxz_bin)
    // Form("h1D_DeltaPhi_Mbin%d_Eta%d_VtxZ%d_rotated", Mbin, eta_bin, vtxz_bin)
    // Form("h1D_typeA_DeltaPhi_Mbin%d_Eta%d_VtxZ%d_rotated", Mbin, eta_bin, vtxz_bin)

    // Form("h1D_BestPair_DeltaEta_Mbin%d",Mbin_in)
    // Form("h1D_BestPair_DeltaPhi_Mbin%d",Mbin_in)
    // Form("h1D_BestPair_ClusPhiSize_Mbin%d",Mbin_in)
    // Form("h1D_BestPair_ClusAdc_Mbin%d",Mbin_in)
    // Form("h1D_typeA_BestPair_DeltaEta_Mbin%d",Mbin_in)
    // Form("h1D_typeA_BestPair_DeltaPhi_Mbin%d",Mbin_in)
    // Form("h1D_typeA_BestPair_ClusPhiSize_Mbin%d",Mbin_in)
    // Form("h1D_typeA_BestPair_ClusAdc_Mbin%d",Mbin_in)

    // Form("h2D_BestPairEtaVtxZ_Mbin%d",Mbin_in)
    // Form("h2D_BestPairEtaVtxZ_Mbin%d_FineBin",Mbin_in)
    // Form("h2D_typeA_BestPairEtaVtxZ_Mbin%d",Mbin_in)
    // Form("h2D_typeA_BestPairEtaVtxZ_Mbin%d_FineBin",Mbin_in)

    // Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d", Mbin_in)
    // Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d_FineBin", Mbin_in)
    // Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d_rotated", Mbin_in)

    // Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d", Mbin_in)
    // Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d_FineBin", Mbin_in)
    // Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d_rotated", Mbin_in)

    
}

void TrackletHistogramNew::MainProcess()
{
    if (SetRandomHits.first){PrepareUniqueClusXYZ();}
    
    if (ColMulMask && h2D_GoodColMap == nullptr){
        std::cout<<"The GoodColMap is not set correctly"<<std::endl;
        exit(1);
    }

    if (HaveGeoOffsetTag && CheckGeoOffsetMap() <= 0){
        std::cout<<"The geo offset map is not set correctly"<<std::endl;
        exit(1);
    }

    if (
        ColMulMask &&
        (
            h1D_GoodColMap_ZId -> GetNbinsX() != h2D_GoodColMap -> GetNbinsY() ||
            fabs(h1D_GoodColMap_ZId -> GetXaxis() -> GetXmin() - h2D_GoodColMap -> GetYaxis() -> GetXmin()) > 0.0000001 ||
            fabs(h1D_GoodColMap_ZId -> GetXaxis() -> GetXmax() - h2D_GoodColMap -> GetYaxis() -> GetXmax()) > 0.0000001
        )

    ){
        std::cout<<"The setting of h1D_GoodColMap_ZId is different from h2D_GoodColMap"<<std::endl;
        std::cout<<"h1D_GoodColMap_ZId : "<<h1D_GoodColMap_ZId -> GetNbinsX()<<" "<<h1D_GoodColMap_ZId -> GetXaxis() -> GetXmin()<<" "<<h1D_GoodColMap_ZId -> GetXaxis() -> GetXmax()<<std::endl;
        std::cout<<"h2D_GoodColMap : "<<h2D_GoodColMap -> GetNbinsY()<<" "<<h2D_GoodColMap -> GetYaxis() -> GetXmin()<<" "<<h2D_GoodColMap -> GetYaxis() -> GetXmax()<<std::endl;
        exit(1);
    }

    for (int i = 0; i < run_nEvents; i++)
    {
        tree_in -> GetEntry(i);

        EvtCleanUp();

        if (i % 10 == 0) {std::cout << "Processing event " << i<<", NClus : "<< ClusX -> size() << std::endl;}

        if (RandInttZ){
            // INTTvtxZ = rand3 -> Uniform(VtxZEdge_min,VtxZEdge_max);
            INTTvtxZ = rand3 -> Uniform(-10, 10);
            INTTvtxZError = 0.;
        }

        // =======================================================================================================================================================
        // note : hard cut

        // note : for data
        if (runnumber != -1 && BcoFullDiffCut && InttBcoFullDiff_next <= cut_InttBcoFullDIff_next) {continue;}
        if (runnumber != -1 && MBDNSg2 != 1) {continue;} // todo: assume MC no trigger

        // note : for MC
        if (runnumber == -1 && NTruthVtx != 1) {continue;}

        // note : both data and MC
        if (is_min_bias != 1) {continue;}
        if (MBD_z_vtx != MBD_z_vtx) {continue;}
        if (MBD_centrality != MBD_centrality) {continue;}
        if (MBD_centrality < 0 || MBD_centrality > 100) {continue;}
        if (INTTvtxZ != INTTvtxZ) {continue;}
        if (MBD_z_vtx < cut_GlobalMBDvtxZ.first || MBD_z_vtx > cut_GlobalMBDvtxZ.second) {continue;} // todo: the hard cut 60 cm 
        // =======================================================================================================================================================

        int Mbin = h1D_centrality_bin -> Fill(MBD_centrality);
        Mbin = (Mbin == -1) ? -1 : Mbin - 1;
        if (Mbin == -1) {
            std::cout << "Mbin == -1, MBD_centrality = " << MBD_centrality << std::endl;
            continue;
        }
        // =======================================================================================================================================================


        // note : for truth
        if (runnumber == -1){
            int NHadrons = 0;
            int NHadrons_OneEtaBin = 0;

            int TruthVtxZ_bin = h1D_vtxz_template->Fill(TruthPV_trig_z);
            TruthVtxZ_bin = (TruthVtxZ_bin == -1) ? -1 : TruthVtxZ_bin - 1;

            if (TruthVtxZ_bin != -1){
                for (int true_i = 0; true_i < NPrimaryG4P; true_i++){
                    if (PrimaryG4P_isChargeHadron->at(true_i) != 1) { continue; }

                    // todo : for debug
                    if (TruthPV_trig_z >= -10 && TruthPV_trig_z < 10){
                        NHadrons++;
                        // todo : the selected bin
                        if (PrimaryG4P_Eta->at(true_i) >= -1.1 && PrimaryG4P_Eta->at(true_i) < -0.9){
                            NHadrons_OneEtaBin++;
                        }
                    }
                    
                    h1D_map[Form("h1D_TrueEta_Mbin%d_VtxZ%d", Mbin, TruthVtxZ_bin)] -> Fill(PrimaryG4P_Eta->at(true_i));
                    h2D_map[Form("h2D_TrueEtaVtxZ_Mbin%d", Mbin)] -> Fill(PrimaryG4P_Eta->at(true_i), TruthPV_trig_z);
                    h2D_map[Form("h2D_TrueEtaVtxZ_Mbin%d_FineBin", Mbin)] -> Fill(PrimaryG4P_Eta->at(true_i), TruthPV_trig_z);
                }

                h2D_map["h2D_TrueEvtCount_vtxZCentrality"]->Fill(TruthPV_trig_z, MBD_centrality);

                // todo : for debug
                if (TruthPV_trig_z >= -10 && TruthPV_trig_z < 10){

                    h1D_map["debug_h1D_NHadron_Inclusive100"] -> Fill(NHadrons);
                    h1D_map["debug_h1D_NHadron_OneEtaBin_Inclusive100"] -> Fill(NHadrons_OneEtaBin);
                }

            }
        }


        // =======================================================================================================================================================
        // note : optional cut
        if (INTT_vtxZ_QA && (MBD_z_vtx - INTTvtxZ < cut_vtxZDiff.first || MBD_z_vtx - INTTvtxZ > cut_vtxZDiff.second) ) {continue;}
        if (INTT_vtxZ_QA && (TrapezoidalFitWidth < cut_TrapezoidalFitWidth.first || TrapezoidalFitWidth > cut_TrapezoidalFitWidth.second)){continue;}
        if (INTT_vtxZ_QA && (TrapezoidalFWHM < cut_TrapezoidalFWHM.first || TrapezoidalFWHM > cut_TrapezoidalFWHM.second)){continue;}
        if (INTT_vtxZ_QA && (INTTvtxZError < cut_INTTvtxZError.first || INTTvtxZError > cut_INTTvtxZError.second)){continue;}
        // =======================================================================================================================================================
        
        if (vtxZReweight.first && runnumber != -1){
            std::cout<<"Should not have the vtxZ weighting from the data"<<std::endl;
            exit(1);
        }

        double INTTvtxZWeighting;
        if (vtxZReweight.first && h1D_INTT_vtxZ_reweighting != nullptr){
            INTTvtxZWeighting = h1D_INTT_vtxZ_reweighting -> GetBinContent(h1D_INTT_vtxZ_reweighting -> FindBin(INTTvtxZ));
        }
        else if (vtxZReweight.first && h1D_INTT_vtxZ_reweighting == nullptr){
            std::cout << "ApplyVtxZReWeighting is true, but h1D_INTT_vtxZ_reweighting is nullptr" << std::endl;
            exit(1);
        }
        else {
            INTTvtxZWeighting = 1.0;
        }

        // =======================================================================================================================================================
        
        int InttVtxZ_bin = h1D_vtxz_template->Fill(INTTvtxZ);
        InttVtxZ_bin = (InttVtxZ_bin == -1) ? -1 : InttVtxZ_bin - 1;

        // int MBDVtxZ_bin = h1D_vtxz_template->Fill(MBD_z_vtx);
        // MBDVtxZ_bin = (MBDVtxZ_bin == -1) ? -1 : MBDVtxZ_bin - 1;

        // note : everything below is within INTT vtxZ -45 cm  ~ 45 cm
        if (InttVtxZ_bin == -1) {continue;}


        // note : for reconstructed 
        // note : INTT vtxZ range -45 cm ~ 45 cm
        h1D_centrality_bin_weighted -> Fill(MBD_centrality,INTTvtxZWeighting);
        h1D_INTTvtxZ -> Fill(INTTvtxZ, INTTvtxZWeighting);
        h1D_INTTvtxZ_FineBin -> Fill(INTTvtxZ, INTTvtxZWeighting);
        h1D_INTTvtxZ_FineBin_NoVtxZWeight -> Fill(INTTvtxZ); // note : no vtxZ weighting
        h2D_INTTvtxZFineBin_CentralityBin -> Fill(INTTvtxZ, MBD_centrality, INTTvtxZWeighting);
        h1D_map["h1D_centrality"] -> Fill(MBD_centrality,INTTvtxZWeighting); // note : fine bins in centrality

        h2D_map["h2D_RecoEvtCount_vtxZCentrality"] -> Fill(INTTvtxZ, MBD_centrality, INTTvtxZWeighting);
        h2D_map["h2D_InttVtxZ_Centrality"] -> Fill(INTTvtxZ, MBD_centrality, INTTvtxZWeighting); // note : fine bins in centrality
        h1D_map[Form("h1D_centrality_InttVtxZ%d", InttVtxZ_bin)] -> Fill(MBD_centrality,INTTvtxZWeighting); // note : fine bins in centrality

        PrepareClusterVec();

        for (ClusHistogram::clu_info this_clu : evt_sPH_inner_nocolumn_vec){
            h1D_Inner_ClusEta_INTTz -> Fill(this_clu.eta_INTTz, INTTvtxZWeighting);
        }
        for (ClusHistogram::clu_info this_clu : evt_sPH_outer_nocolumn_vec){
            h1D_Outer_ClusEta_INTTz -> Fill(this_clu.eta_INTTz, INTTvtxZWeighting);
        }
        

        GetTrackletPair(evt_TrackletPair_vec, false);
        FillPairs(evt_TrackletPair_vec, false, Mbin, InttVtxZ_bin, INTTvtxZWeighting, i);

        GetTrackletPair(evt_TrackletPairRotate_vec, true);  
        FillPairs(evt_TrackletPairRotate_vec, true, Mbin, InttVtxZ_bin, INTTvtxZWeighting, i);

    }
}

void TrackletHistogramNew::EndRun()
{
    file_out -> cd();

    tree_out_par -> Fill();
    tree_out_par -> Write();

    h1D_INTTvtxZ -> Write();
    h1D_INTTvtxZ_FineBin -> Write();
    h1D_INTTvtxZ_FineBin_NoVtxZWeight -> Write();
    h2D_INTTvtxZFineBin_CentralityBin -> Write();

    h1D_centrality_bin -> Write();
    h1D_centrality_bin_weighted -> Write();
    
    h1D_eta_template -> Write();
    h1D_vtxz_template -> Write();

    h1D_PairDeltaEta_inclusive -> Write();
    h1D_PairDeltaPhi_inclusive -> Write();

    h1D_Inner_ClusEta_INTTz -> Write();
    h1D_Outer_ClusEta_INTTz -> Write();

    if (h1D_RandClusZ_ref != nullptr){
        h1D_RandClusZ_ref -> Write();
    }

    if (h2D_RandClusXY_ref != nullptr){
        h2D_RandClusXY_ref -> Write();
    }

    for (auto &pair : h2D_map){
        pair.second -> Write();
    }

    for (auto &pair : h1D_map){
        pair.second -> Write();
    }

    file_out -> Close();
}

std::vector<ClusHistogram::clu_info> TrackletHistogramNew::GetRotatedClusterVec(std::vector<ClusHistogram::clu_info> input_cluster_vec)
{
    std::vector<ClusHistogram::clu_info> output_cluster_vec; output_cluster_vec.clear();

    for (ClusHistogram::clu_info this_clu : input_cluster_vec)
    {
        std::pair<double,double> rotated_xy = rotatePoint(this_clu.x, this_clu.y);
     
        ClusHistogram::clu_info rotated_clu = this_clu;
        rotated_clu.x = rotated_xy.first;
        rotated_clu.y = rotated_xy.second;

        rotated_clu.eta_INTTz = ClusHistogram::get_clu_eta({vertexXYIncm.first, vertexXYIncm.second, INTTvtxZ}, {rotated_clu.x, rotated_clu.y, rotated_clu.z});

        output_cluster_vec.push_back(rotated_clu);
    }

    return output_cluster_vec;
}

std::pair<double,double> TrackletHistogramNew::rotatePoint(double x, double y)
{
    // Convert the rotation angle from degrees to radians
    double rotation = rotate_phi_angle; // todo rotation is here
    double angleRad = rotation * M_PI / 180.0;

    // Perform the rotation
    double xOut = x * cos(angleRad) - y * sin(angleRad);
    double yOut = x * sin(angleRad) + y * cos(angleRad);

    xOut = (fabs(xOut) < 0.0000001) ? 0 : xOut;
    yOut = (fabs(yOut) < 0.0000001) ? 0 : yOut;

    // cout<<"Post rotation: "<<xOut<<" "<<yOut<<endl;

    return {xOut,yOut};
}

std::pair<double,double> TrackletHistogramNew::Get_possible_zvtx(double rvtx, std::vector<double> p0, std::vector<double> p1) // note : inner p0, outer p1, vector {r,z, zid}, -> {y,x}
{
    std::vector<double> p0_z_edge = { ( p0[2] == typeA_sensorZID[0] || p0[2] == typeA_sensorZID[1] ) ? p0[1] - typeA_sensor_half_length_incm : p0[1] - typeB_sensor_half_length_incm, ( p0[2] == typeA_sensorZID[0] || p0[2] == typeA_sensorZID[1] ) ? p0[1] + typeA_sensor_half_length_incm : p0[1] + typeB_sensor_half_length_incm}; // note : vector {left edge, right edge}
    std::vector<double> p1_z_edge = { ( p1[2] == typeA_sensorZID[0] || p1[2] == typeA_sensorZID[1] ) ? p1[1] - typeA_sensor_half_length_incm : p1[1] - typeB_sensor_half_length_incm, ( p1[2] == typeA_sensorZID[0] || p1[2] == typeA_sensorZID[1] ) ? p1[1] + typeA_sensor_half_length_incm : p1[1] + typeB_sensor_half_length_incm}; // note : vector {left edge, right edge}

    double edge_first  = Get_extrapolation(rvtx,p0_z_edge[0],p0[0],p1_z_edge[1],p1[0]);
    double edge_second = Get_extrapolation(rvtx,p0_z_edge[1],p0[0],p1_z_edge[0],p1[0]);

    double mid_point = (edge_first + edge_second) / 2.;
    double possible_width = fabs(edge_first - edge_second) / 2.;

    return {mid_point, possible_width}; // note : first : mid point, second : width

}

// note : angle_1 = inner clu phi
// note: angle_2 = outer clu phi
double TrackletHistogramNew::get_delta_phi(double angle_1, double angle_2)
{
    std::vector<double> vec_abs = {std::fabs(angle_1 - angle_2), std::fabs(angle_1 - angle_2 + 360), std::fabs(angle_1 - angle_2 - 360)};
    std::vector<double> vec = {(angle_1 - angle_2), (angle_1 - angle_2 + 360), (angle_1 - angle_2 - 360)};
    return vec[std::distance(vec_abs.begin(), std::min_element(vec_abs.begin(),vec_abs.end()))];
}

double TrackletHistogramNew::get_radius(double x, double y)
{
    return sqrt(pow(x,2)+pow(y,2));
}

// note : pair<reduced chi2, eta of the track>
// note : vector : {r,z}
// note : p0 : vertex point {r,z,zerror}
// note : p1 : inner layer
// note : p2 : outer layer
std::pair<double,double> TrackletHistogramNew::Get_eta(std::vector<double>p0, std::vector<double>p1, std::vector<double>p2)
{
    // if (track_gr -> GetN() != 0) {cout<<"In TrackletHistogramNew class, track_gr is not empty, track_gr size : "<<track_gr -> GetN()<<endl; exit(0);}
    
    if (track_gr -> GetN() != 0) {track_gr -> Set(0);}
    
    std::vector<double> r_vec  = {p0[0], p1[0], p2[0]}; 
    std::vector<double> z_vec  = {p0[1], p1[1], p2[1]}; 
    std::vector<double> re_vec = {0, 0, 0}; 
    std::vector<double> ze_vec = {
        p0[2],
        (p1[2] == typeA_sensorZID[0] || p1[2] == typeA_sensorZID[1]) ? typeA_sensor_half_length_incm : typeB_sensor_half_length_incm,
        (p2[2] == typeA_sensorZID[0] || p2[2] == typeA_sensorZID[1]) ? typeA_sensor_half_length_incm : typeB_sensor_half_length_incm
    };

    // std::vector<double> ze_vec = {p0[2], ( fabs( p1[1] ) < sensor_type_segment ) ? sensor_length_typeA : sensor_length_typeB, ( fabs( p2[1] ) < sensor_type_segment ) ? sensor_length_typeA : sensor_length_typeB}; 

    // note : swap the r and z, to have easier fitting 
    // note : in principle, Z should be in the x axis, R should be in the Y axis
    for (int i = 0; i < 3; i++)
    {
        track_gr -> SetPoint(i,r_vec[i],z_vec[i]);
        track_gr -> SetPointError(i,re_vec[i],ze_vec[i]);

        // cout<<"In TrackletHistogramNew class, point : "<<i<<" r : "<<r_vec[i]<<" z : "<<z_vec[i]<<" re : "<<re_vec[i]<<" ze : "<<ze_vec[i]<<endl;
    }    
    
    double vertical_line = ( fabs( grEY_stddev(track_gr) ) < 0.00001 ) ? 1 : 0;
    
    if (vertical_line) {return {0,0};}
    else 
    {
        fit_rz -> SetParameters(0,0);

        track_gr -> Fit(fit_rz,"NQ");

        std::pair<double,double> ax_b = mirrorPolynomial(fit_rz -> GetParameter(1),fit_rz -> GetParameter(0));

        return  {(fit_rz -> GetChisquare() / double(fit_rz -> GetNDF())), -1 * TMath::Log( fabs( tan( atan2(ax_b.first, (ax_b.first > 0) ? 1. : -1. ) / 2 ) ) )};
    }

}

double TrackletHistogramNew::grEY_stddev(TGraphErrors * input_grr)
{
    std::vector<double> input_vector; input_vector.clear();
    for (int i = 0; i < input_grr -> GetN(); i++) {input_vector.push_back( input_grr -> GetPointY(i) );}

    double average = std::accumulate( input_vector.begin(), input_vector.end(), 0.0 ) / double(input_vector.size());

    double sum_subt = 0;
	for (int ele : input_vector) {sum_subt+=pow((ele-average),2);}
	
	return sqrt(sum_subt/(input_vector.size()-1));
}	

std::pair<double, double> TrackletHistogramNew::mirrorPolynomial(double a, double b) {
    // Interchange 'x' and 'y'
    double mirroredA = 1.0 / a;
    double mirroredB = -b / a;

    return {mirroredA, mirroredB};
}

double TrackletHistogramNew::Get_extrapolation(double given_y, double p0x, double p0y, double p1x, double p1y) // note : x : z, y : r
{
    if ( fabs(p0x - p1x) < 0.000001 ){ // note : the line is vertical (if z is along the x axis)
        return p0x;
    }
    else {
        double slope = (p1y - p0y) / (p1x - p0x);
        double yIntercept = p0y - slope * p0x;
        double xCoordinate = (given_y - yIntercept) / slope;
        return xCoordinate;
    }
}