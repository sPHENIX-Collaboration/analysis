#include "TrackletHistogramFill.h"

TrackletHistogramFill::TrackletHistogramFill(
    int process_id_in,
    int runnumber_in,

    std::string output_directory_in,
    std::string output_file_name_suffix_in
):
    process_id(process_id_in),
    runnumber(runnumber_in),
    output_directory(output_directory_in),
    output_file_name_suffix(output_file_name_suffix_in),
    isWithRotate(true)
{
    nCentrality_bin = centrality_edges.size() - 1;
    vtxZReweightFactor = 1.;
}


void TrackletHistogramFill::PrepareOutPutFileName()
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

    output_filename = "TrackletHistogram";
    output_filename = (runnumber != -1) ? "Data_" + output_filename : "MC_" + output_filename;
    output_filename += (vtxZReweight) ? "_vtxZReweight" : "";
    output_filename += (BcoFullDiffCut && runnumber != -1) ? "_BcoFullDiffCut" : "";
    output_filename += (INTT_vtxZ_QA) ? "_INTT_vtxZ_QA" : "";
    output_filename += (isClusQA.first) ? Form("_ClusQAAdc%.0fPhiSize%.0f",isClusQA.second.first,isClusQA.second.second) : "";
    output_filename += output_file_name_suffix;
    output_filename += (runnumber != -1) ? Form("_%s_%s.root",runnumber_str.c_str(),job_index.c_str()) : Form("_%s.root",job_index.c_str());
}

void TrackletHistogramFill::PrepareOutPutRootFile()
{
    file_out = new TFile(Form("%s/%s", output_directory.c_str(), output_filename.c_str()), "RECREATE");
    tree_out_par = new TTree("tree_par", "used parameters");

    tree_out_par -> Branch("process_id", &process_id);
    tree_out_par -> Branch("runnumber", &runnumber);
    tree_out_par -> Branch("vtxZReweight", &vtxZReweight);
    tree_out_par -> Branch("BcoFullDiffCut", &BcoFullDiffCut);
    tree_out_par -> Branch("INTT_vtxZ_QA", &INTT_vtxZ_QA);
    tree_out_par -> Branch("isWithRotate", &isWithRotate);

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

    tree_out_par -> Branch("DeltaPhiEdge_min", &DeltaPhiEdge_min);
    tree_out_par -> Branch("DeltaPhiEdge_max", &DeltaPhiEdge_max);
    tree_out_par -> Branch("nDeltaPhiBin", &nDeltaPhiBin);

    tree_out_par -> Branch("typeA_sensorZID", &typeA_sensorZID);
    tree_out_par -> Branch("cut_GlobalMBDvtxZ", &cut_GlobalMBDvtxZ);   
    tree_out_par -> Branch("cut_vtxZDiff", &cut_vtxZDiff);
    tree_out_par -> Branch("cut_TrapezoidalFitWidth", &cut_TrapezoidalFitWidth);
    tree_out_par -> Branch("cut_TrapezoidalFWHM", &cut_TrapezoidalFWHM);
    tree_out_par -> Branch("cut_InttBcoFullDIff_next", &cut_InttBcoFullDIff_next);
    tree_out_par -> Branch("cut_bestPair_DeltaPhi", &cut_bestPair_DeltaPhi);
    tree_out_par -> Branch("cut_GoodProtoTracklet_DeltaPhi", &cut_GoodProtoTracklet_DeltaPhi);
}

void TrackletHistogramFill::PrepareHistograms()
{
    h1D_centrality_bin = new TH1D("h1D_centrality_bin","h1D_centrality_bin;Centrality [%];Entries",nCentrality_bin,&centrality_edges[0]); // note : the 0-5%

    h1D_eta_template = new TH1D("h1D_eta_template", "h1D_eta_template", nEtaBin, EtaEdge_min, EtaEdge_max); // note : coarse
    h1D_vtxz_template = new TH1D("h1D_vtxz_template", "h1D_vtxz_template", nVtxZBin, VtxZEdge_min, VtxZEdge_max); // note : coarse

    h1D_PairDeltaEta_inclusive = new TH1D("h1D_PairDeltaEta_inclusive", "h1D_PairDeltaEta_inclusive;Pair #eta;Entries", nDeltaEtaBin, DeltaEtaEdge_min, DeltaEtaEdge_max);
    h1D_PairDeltaPhi_inclusive = new TH1D("h1D_PairDeltaPhi_inclusive", "h1D_PairDeltaPhi_inclusive;Pair #eta;Entries", nDeltaPhiBin, DeltaPhiEdge_min, DeltaPhiEdge_max);

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

        isInserted = h1D_map.insert( std::make_pair(
                Form("h1D_centrality_MBDVtxZ%d", vtxz_bin),
                new TH1D(Form("h1D_centrality_MBDVtxZ%d", vtxz_bin), Form("h1D_centrality_MBDVtxZ%d;Centrality [%];Entries",vtxz_bin), nCentralityFineBin, CentralityFineEdge_min, CentralityFineEdge_max)
            )
        ).second; insert_check.push_back(isInserted);
    }

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

                if (isWithRotate){
                    isInserted = h1D_map.insert( std::make_pair(
                            Form("h1D_DeltaPhi_Mbin%d_Eta%d_VtxZ%d_rotated", Mbin, eta_bin, vtxz_bin),
                            new TH1D(Form("h1D_DeltaPhi_Mbin%d_Eta%d_VtxZ%d_rotated", Mbin, eta_bin, vtxz_bin), Form("h1D_DeltaPhi_Mbin%d_Eta%d_VtxZ%d_rotated;Pair #Delta#phi [radian];Entries", Mbin, eta_bin, vtxz_bin), nDeltaPhiBin, DeltaPhiEdge_min, DeltaPhiEdge_max)
                        )
                    ).second; insert_check.push_back(isInserted);
                }


                isInserted = h1D_map.insert( std::make_pair(
                        Form("h1D_typeA_DeltaPhi_Mbin%d_Eta%d_VtxZ%d", Mbin, eta_bin, vtxz_bin),
                        new TH1D(Form("h1D_typeA_DeltaPhi_Mbin%d_Eta%d_VtxZ%d", Mbin, eta_bin, vtxz_bin), Form("h1D_typeA_DeltaPhi_Mbin%d_Eta%d_VtxZ%d;Pair #Delta#phi [radian] (type A);Entries", Mbin, eta_bin, vtxz_bin), nDeltaPhiBin, DeltaPhiEdge_min, DeltaPhiEdge_max)
                    )
                ).second; insert_check.push_back(isInserted);

                if (isWithRotate){
                    isInserted = h1D_map.insert( std::make_pair(
                            Form("h1D_typeA_DeltaPhi_Mbin%d_Eta%d_VtxZ%d_rotated", Mbin, eta_bin, vtxz_bin),
                            new TH1D(Form("h1D_typeA_DeltaPhi_Mbin%d_Eta%d_VtxZ%d_rotated", Mbin, eta_bin, vtxz_bin), Form("h1D_typeA_DeltaPhi_Mbin%d_Eta%d_VtxZ%d_rotated;Pair #Delta#phi [radian] (type A);Entries", Mbin, eta_bin, vtxz_bin), nDeltaPhiBin, DeltaPhiEdge_min, DeltaPhiEdge_max)
                        )
                    ).second; insert_check.push_back(isInserted);
                }


                // note : for truth eta
                if (runnumber == -1 && eta_bin == 0){
                    isInserted = h1D_map.insert( std::make_pair(
                            Form("h1D_TrueEta_Mbin%d_VtxZ%d", Mbin, vtxz_bin),
                            new TH1D(Form("h1D_TrueEta_Mbin%d_VtxZ%d", Mbin, vtxz_bin), Form("h1D_TrueEta_Mbin%d_VtxZ%d;PHG4Particle #eta;Entries", Mbin, vtxz_bin), nEtaBin, EtaEdge_min, EtaEdge_max)
                        )
                    ).second; insert_check.push_back(isInserted);
                }
            } // note : end Mbin

            // // note : for inclusive [0 - 100]
            // isInserted = h1D_map.insert( std::make_pair(
                //     Form("h1D_DeltaPhi_Inclusive100_Eta%d_VtxZ%d", eta_bin, vtxz_bin),
                //     new TH1D(Form("h1D_DeltaPhi_Inclusive100_Eta%d_VtxZ%d", eta_bin, vtxz_bin), Form("h1D_DeltaPhi_Inclusive100_Eta%d_VtxZ%d;Pair #Delta#phi [radian];Entries", eta_bin, vtxz_bin), nDeltaPhiBin, DeltaPhiEdge_min, DeltaPhiEdge_max)
                // )
            // ).second; insert_check.push_back(isInserted);

            // isInserted = h1D_map.insert( std::make_pair(
                //     Form("h1D_DeltaPhi_Inclusive100_Eta%d_VtxZ%d_rotated", eta_bin, vtxz_bin),
                //     new TH1D(Form("h1D_DeltaPhi_Inclusive100_Eta%d_VtxZ%d_rotated", eta_bin, vtxz_bin), Form("h1D_DeltaPhi_Inclusive100_Eta%d_VtxZ%d_rotated;Pair #Delta#phi [radian];Entries", eta_bin, vtxz_bin), nDeltaPhiBin, DeltaPhiEdge_min, DeltaPhiEdge_max)
                // )
            // ).second; insert_check.push_back(isInserted);


            // // note : for inclusive [0 - 70]
            // isInserted = h1D_map.insert( std::make_pair(
                //     Form("h1D_DeltaPhi_Inclusive70_Eta%d_VtxZ%d", eta_bin, vtxz_bin),
                //     new TH1D(Form("h1D_DeltaPhi_Inclusive70_Eta%d_VtxZ%d", eta_bin, vtxz_bin), Form("h1D_DeltaPhi_Inclusive70_Eta%d_VtxZ%d;Pair #Delta#phi [radian];Entries", eta_bin, vtxz_bin), nDeltaPhiBin, DeltaPhiEdge_min, DeltaPhiEdge_max)
                // )
            // ).second; insert_check.push_back(isInserted);

            // isInserted = h1D_map.insert( std::make_pair(
                //     Form("h1D_DeltaPhi_Inclusive70_Eta%d_VtxZ%d_rotated", eta_bin, vtxz_bin),
                //     new TH1D(Form("h1D_DeltaPhi_Inclusive70_Eta%d_VtxZ%d_rotated", eta_bin, vtxz_bin), Form("h1D_DeltaPhi_Inclusive70_Eta%d_VtxZ%d_rotated;Pair #Delta#phi [radian];Entries", eta_bin, vtxz_bin), nDeltaPhiBin, DeltaPhiEdge_min, DeltaPhiEdge_max)
                // )
            // ).second; insert_check.push_back(isInserted);


            // note : for truth eta
            // if (runnumber == -1 && eta_bin == 0){
            //     isInserted = h1D_map.insert( std::make_pair(
                //         Form("h1D_TrueEta_Inclusive100_VtxZ%d", vtxz_bin),
                //         new TH1D(Form("h1D_TrueEta_Inclusive100_VtxZ%d", vtxz_bin), Form("h1D_TrueEta_Inclusive100_VtxZ%d;PHG4Particle #eta;Entries", vtxz_bin), nEtaBin, EtaEdge_min, EtaEdge_max)
                // )
            //     ).second; insert_check.push_back(isInserted);

            //     isInserted = h1D_map.insert( std::make_pair(
                //         Form("h1D_TrueEta_Inclusive70_VtxZ%d", vtxz_bin),
                //         new TH1D(Form("h1D_TrueEta_Inclusive70_VtxZ%d", vtxz_bin), Form("h1D_TrueEta_Inclusive70_VtxZ%d;PHG4Particle #eta;Entries", vtxz_bin), nEtaBin, EtaEdge_min, EtaEdge_max)
                // )
            //     ).second; insert_check.push_back(isInserted);
            // }
        }
    }

    // note : for the best pairs
    isInserted = h1D_map.insert( std::make_pair(
            Form("h1D_BestPair_Inclusive70_DeltaEta"),
            new TH1D(Form("h1D_BestPair_Inclusive70_DeltaEta"), Form("h1D_BestPair_Inclusive70_DeltaEta;Pair #Delta#eta;Entries"), nDeltaEtaBin, DeltaEtaEdge_min, DeltaEtaEdge_max)
        )
    ).second; insert_check.push_back(isInserted);

    isInserted = h1D_map.insert( std::make_pair(
            Form("h1D_BestPair_Inclusive70_DeltaPhi"),
            new TH1D(Form("h1D_BestPair_Inclusive70_DeltaPhi"), Form("h1D_BestPair_Inclusive70_DeltaPhi;Pair #Delta#phi [radian];Entries"), nDeltaPhiBin, DeltaPhiEdge_min, DeltaPhiEdge_max)
        )
    ).second; insert_check.push_back(isInserted);

    isInserted = h1D_map.insert( std::make_pair(
            Form("h1D_BestPair_Inclusive70_ClusPhiSize"),
            new TH1D(Form("h1D_BestPair_Inclusive70_ClusPhiSize"), Form("h1D_BestPair_Inclusive70_ClusPhiSize;ClusPhiSize;Entries"), 128,0,128)
        )
    ).second; insert_check.push_back(isInserted);

    isInserted = h1D_map.insert( std::make_pair(
            Form("h1D_BestPair_Inclusive70_ClusAdc"),
            new TH1D(Form("h1D_BestPair_Inclusive70_ClusAdc"), Form("h1D_BestPair_Inclusive70_ClusAdc;ClusAdc;Entries"), 200,0,18000)
        )
    ).second; insert_check.push_back(isInserted);



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
                    new TH2D(Form("h2D_TrueEtaVtxZ_Mbin%d_FineBin", Mbin), Form("h2D_TrueEtaVtxZ_Mbin%d_FineBin;PHG4Particle #eta;TruthPV_trig_z [cm]", Mbin), 400, EtaEdge_min, EtaEdge_max, 400, VtxZEdge_min, VtxZEdge_max)
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
        
        // isInserted = h2D_map.insert( std::make_pair(
            //     Form("h2D_Inclusive100_TrueEtaVtxZ"),
            //     new TH2D(Form("h2D_Inclusive100_TrueEtaVtxZ"), Form("h2D_Inclusive100_TrueEtaVtxZ;PHG4Particle #eta;TruthPV_trig_z [cm]"), nEtaBin, EtaEdge_min, EtaEdge_max, nVtxZBin, VtxZEdge_min, VtxZEdge_max)
            // )
        // ).second; insert_check.push_back(isInserted);

        // isInserted = h2D_map.insert( std::make_pair(
            //     Form("h2D_Inclusive100_TrueEtaVtxZ_FineBin"),
            //     new TH2D(Form("h2D_Inclusive100_TrueEtaVtxZ_FineBin"), Form("h2D_Inclusive100_TrueEtaVtxZ_FineBin;PHG4Particle #eta;TruthPV_trig_z [cm]"), 400, EtaEdge_min, EtaEdge_max, 400, VtxZEdge_min, VtxZEdge_max)
            // )
        // ).second; insert_check.push_back(isInserted);

        // isInserted = h2D_map.insert( std::make_pair(
            //     Form("h2D_Inclusive70_TrueEtaVtxZ"),
            //     new TH2D(Form("h2D_Inclusive70_TrueEtaVtxZ"), Form("h2D_Inclusive70_TrueEtaVtxZ;PHG4Particle #eta;TruthPV_trig_z [cm]"), nEtaBin, EtaEdge_min, EtaEdge_max, nVtxZBin, VtxZEdge_min, VtxZEdge_max)
            // )
        // ).second; insert_check.push_back(isInserted);

        // isInserted = h2D_map.insert( std::make_pair(
            //     Form("h2D_Inclusive70_TrueEtaVtxZ_FineBin"),
            //     new TH2D(Form("h2D_Inclusive70_TrueEtaVtxZ_FineBin"), Form("h2D_Inclusive70_TrueEtaVtxZ_FineBin;PHG4Particle #eta;TruthPV_trig_z [cm]"), 400, EtaEdge_min, EtaEdge_max, 400, VtxZEdge_min, VtxZEdge_max)
            // )
        // ).second; insert_check.push_back(isInserted);

        isInserted = h2D_map.insert( std::make_pair(
                Form("h2D_TrueEvtCount_vtxZCentrality"),
                new TH2D(Form("h2D_TrueEvtCount_vtxZCentrality"), Form("h2D_TrueEvtCount_vtxZCentrality;TruthPV_trig_z [cm];Centrality [%]"), nVtxZBin, VtxZEdge_min, VtxZEdge_max, nCentrality_bin, &centrality_edges[0]) // note : 0 - 5%
            )
        ).second; insert_check.push_back(isInserted);
    }

    // note : for good proto tracklet in Eta-VtxZ
    for (int Mbin = 0; Mbin < nCentrality_bin; Mbin++){
        
        isInserted = h2D_map.insert( std::make_pair(
                Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d", Mbin),
                new TH2D(Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d", Mbin), Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d;Pair #eta;INTT vtxZ [cm]", Mbin), nEtaBin, EtaEdge_min, EtaEdge_max, nVtxZBin, VtxZEdge_min, VtxZEdge_max) 
            )
        ).second; insert_check.push_back(isInserted);

        isInserted = h2D_map.insert( std::make_pair(
                Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d", Mbin),
                new TH2D(Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d", Mbin), Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d;Pair #eta;INTT vtxZ [cm]", Mbin), nEtaBin, EtaEdge_min, EtaEdge_max, nVtxZBin, VtxZEdge_min, VtxZEdge_max) 
            )
        ).second; insert_check.push_back(isInserted);

        // note : normal fine bin
        isInserted = h2D_map.insert( std::make_pair(
                Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d_FineBin", Mbin),
                new TH2D(Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d_FineBin", Mbin), Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d_FineBin;Pair #eta;INTT vtxZ [cm]", Mbin), 400, EtaEdge_min, EtaEdge_max, 400, VtxZEdge_min, VtxZEdge_max) 
            )
        ).second; insert_check.push_back(isInserted);

        isInserted = h2D_map.insert( std::make_pair(
                Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d_FineBin", Mbin),
                new TH2D(Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d_FineBin", Mbin), Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d_FineBin;Pair #eta;INTT vtxZ [cm]", Mbin), 400, EtaEdge_min, EtaEdge_max, 400, VtxZEdge_min, VtxZEdge_max) 
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
                new TH2D(Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d_rotated", Mbin), Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d_rotated;Pair #eta;INTT vtxZ [cm]", Mbin), nEtaBin, EtaEdge_min, EtaEdge_max, nVtxZBin, VtxZEdge_min, VtxZEdge_max) 
            )
        ).second; insert_check.push_back(isInserted);
    }


    isInserted = h2D_map.insert( std::make_pair(
            Form("h2D_RecoEvtCount_vtxZCentrality"),
            new TH2D(Form("h2D_RecoEvtCount_vtxZCentrality"), Form("h2D_RecoEvtCount_vtxZCentrality;INTT vtxZ [cm];Centrality [%]"), nVtxZBin, VtxZEdge_min, VtxZEdge_max, nCentrality_bin, &centrality_edges[0]) // note : 0 - 5%
        )
    ).second; insert_check.push_back(isInserted);
    
    // note : best pair in Eta-VtxZ
    isInserted = h2D_map.insert( std::make_pair(
            Form("h2D_Inclusive100_BestPairEtaVtxZ"),
            new TH2D(Form("h2D_Inclusive100_BestPairEtaVtxZ"), Form("h2D_Inclusive100_BestPairEtaVtxZ;Tracklet #eta;INTT vtxZ [cm]"), nEtaBin, EtaEdge_min, EtaEdge_max, nVtxZBin, VtxZEdge_min, VtxZEdge_max)
        )
    ).second; insert_check.push_back(isInserted);

    isInserted = h2D_map.insert( std::make_pair(
            Form("h2D_Inclusive100_BestPairEtaVtxZ_FineBin"),
            new TH2D(Form("h2D_Inclusive100_BestPairEtaVtxZ_FineBin"), Form("h2D_Inclusive100_BestPairEtaVtxZ_FineBin;Tracklet #eta;INTT vtxZ [cm]"), 400, EtaEdge_min, EtaEdge_max, 400, VtxZEdge_min, VtxZEdge_max)
        )
    ).second; insert_check.push_back(isInserted);

    isInserted = h2D_map.insert( std::make_pair(
            Form("h2D_Inclusive70_BestPairEtaVtxZ"),
            new TH2D(Form("h2D_Inclusive70_BestPairEtaVtxZ"), Form("h2D_Inclusive70_BestPairEtaVtxZ;Tracklet #eta;INTT vtxZ [cm]"), nEtaBin, EtaEdge_min, EtaEdge_max, nVtxZBin, VtxZEdge_min, VtxZEdge_max)
        )
    ).second; insert_check.push_back(isInserted);

    isInserted = h2D_map.insert( std::make_pair(
            Form("h2D_Inclusive70_BestPairEtaVtxZ_FineBin"),
            new TH2D(Form("h2D_Inclusive70_BestPairEtaVtxZ_FineBin"), Form("h2D_Inclusive70_BestPairEtaVtxZ_FineBin;Tracklet #eta;INTT vtxZ [cm]"), 400, EtaEdge_min, EtaEdge_max, 400, VtxZEdge_min, VtxZEdge_max)
        )
    ).second; insert_check.push_back(isInserted);


    // note : vtxZ - centrality
    isInserted = h2D_map.insert( std::make_pair(
            Form("h2D_InttVtxZ_Centrality"),
            new TH2D(Form("h2D_InttVtxZ_Centrality"), Form("h2D_InttVtxZ_Centrality;INTT vtxZ [cm];Centrality"), nVtxZBin, VtxZEdge_min, VtxZEdge_max, nCentralityFineBin, CentralityFineEdge_min, CentralityFineEdge_max)
        )
    ).second; insert_check.push_back(isInserted);

    isInserted = h2D_map.insert( std::make_pair(
            Form("h2D_MBDVtxZ_Centrality"),
            new TH2D(Form("h2D_MBDVtxZ_Centrality"), Form("h2D_MBDVtxZ_Centrality;INTT vtxZ [cm];Centrality"), nVtxZBin, VtxZEdge_min, VtxZEdge_max, nCentralityFineBin, CentralityFineEdge_min, CentralityFineEdge_max)
        )
    ).second; insert_check.push_back(isInserted);
    

    for (int isInserted_tag : insert_check){
        if (isInserted_tag == 0){
            std::cout<<"Histogram insertion failed"<<std::endl;
            exit(1);
        }
    }
}

void TrackletHistogramFill::EvtCleanUp()
{
    Used_Clus_index_map.clear();
    Pair_DeltaPhi_vec.clear();
}

void TrackletHistogramFill::FillHistogram(
    // note : MBD & centrality relevant
    float MBD_z_vtx,
    int is_min_bias,
    float MBD_centrality,
    float MBD_south_charge_sum,
    float MBD_north_charge_sum,
    // float MBD_charge_sum,
    // float MBD_charge_asymm,
    int InttBcoFullDiff_next,

    // // note : trigger tag
    int MBDNSg2,
    // int MBDNSg2_vtxZ10cm,
    // int MBDNSg2_vtxZ30cm,
    // int MBDNSg2_vtxZ60cm,

    // std::vector<float> *ClusX,
    // std::vector<float> *ClusY,
    // std::vector<float> *ClusZ,
    // std::vector<int> *ClusLayer,
    std::vector<unsigned char> *ClusLadderZId,
    // std::vector<unsigned char> *ClusLadderPhiId,
    std::vector<int> *ClusAdc,
    std::vector<float> *ClusPhiSize,

    // note : INTT vertex Z
    double INTTvtxZ,
    double INTTvtxZError,
    // double NgroupTrapezoidal,
    // double NgroupCoarse,
    double TrapezoidalFitWidth,
    double TrapezoidalFWHM,

    // note : the tracklet pair
    std::vector<pair_str> *evt_TrackletPair_vec,
    std::vector<pair_str> *evt_TrackletPairRotate_vec,

    // note : MC
    // float TruthPV_trig_x,
    // float TruthPV_trig_y,
    float TruthPV_trig_z,
    int NTruthVtx,
    int NPrimaryG4P,
    // std::vector<float> *PrimaryG4P_Pt,
    std::vector<float> *PrimaryG4P_Eta,
    // std::vector<float> *PrimaryG4P_Phi,
    // std::vector<float> *PrimaryG4P_E,
    // std::vector<float> *PrimaryG4P_PID,
    std::vector<int> *PrimaryG4P_isChargeHadron
)
{
    EvtCleanUp();

    // ==============================================================================================================================================================================================================
    // note : for data
    if (runnumber != -1 && BcoFullDiffCut && InttBcoFullDiff_next <= cut_InttBcoFullDIff_next) {return;}
    if (runnumber != -1 && MBDNSg2 != 1) {return;} // todo: assume MC no trigger

    // note : for MC
    if (runnumber == -1 && NTruthVtx != 1) {return;}

    // note : both data and MC
    if (MBD_z_vtx != MBD_z_vtx) {return;}
    if (MBD_centrality != MBD_centrality) {return;}
    if (MBD_centrality < 0 || MBD_centrality > 1) {return;}
    if (INTTvtxZ != INTTvtxZ) {return;}
    if (MBD_z_vtx < cut_GlobalMBDvtxZ.first || MBD_z_vtx > cut_GlobalMBDvtxZ.second) {return;} // todo: the hard cut 60 cm 
    if (is_min_bias != 1) {return;}
    // ==============================================================================================================================================================================================================
    // todo : No INTT vtxZ reweighting for the h1D_centrality_bin, as the comparison was done in RestDist
    int Mbin = h1D_centrality_bin -> Fill(MBD_centrality);
    Mbin = (Mbin == -1) ? -1 : Mbin - 1;
    if (Mbin == -1) {
        std::cout << "Mbin == -1, MBD_centrality = " << MBD_centrality << std::endl;
        return;
    }

    int INTTvtxz_bin = h1D_vtxz_template->Fill(INTTvtxZ);
    INTTvtxz_bin = (INTTvtxz_bin == -1) ? -1 : INTTvtxz_bin - 1;

    int MBDVtxz_bin = h1D_vtxz_template->Fill(MBD_z_vtx);
    MBDVtxz_bin = (MBDVtxz_bin == -1) ? -1 : MBDVtxz_bin - 1;

    int TruthVtxZ_bin = h1D_vtxz_template->Fill(TruthPV_trig_z);
    TruthVtxZ_bin = (TruthVtxZ_bin == -1) ? -1 : TruthVtxZ_bin - 1;

    if (runnumber == -1){
        
        int NHadrons = 0;
        int NHadrons_OneEtaBin = 0;

        if (h2D_map.find("h2D_TrueEvtCount_vtxZCentrality") != h2D_map.end()){
            h2D_map["h2D_TrueEvtCount_vtxZCentrality"] -> Fill(TruthPV_trig_z, MBD_centrality);
        }

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
            
            if (h1D_map.find(Form("h1D_TrueEta_Mbin%d_VtxZ%d", Mbin, TruthVtxZ_bin)) != h1D_map.end()){
                h1D_map[Form("h1D_TrueEta_Mbin%d_VtxZ%d", Mbin, TruthVtxZ_bin)] -> Fill(PrimaryG4P_Eta->at(true_i));
            }

            if (h2D_map.find(Form("h2D_TrueEtaVtxZ_Mbin%d", Mbin)) != h2D_map.end()){
                h2D_map[Form("h2D_TrueEtaVtxZ_Mbin%d", Mbin)] -> Fill(PrimaryG4P_Eta->at(true_i), TruthPV_trig_z);
            }

            if (h2D_map.find(Form("h2D_TrueEtaVtxZ_Mbin%d_FineBin", Mbin)) != h2D_map.end()){
                h2D_map[Form("h2D_TrueEtaVtxZ_Mbin%d_FineBin", Mbin)] -> Fill(PrimaryG4P_Eta->at(true_i), TruthPV_trig_z);
            }

            // if (h2D_map.find("h2D_Inclusive100_TrueEtaVtxZ") != h2D_map.end()){
            //     h2D_map["h2D_Inclusive100_TrueEtaVtxZ"] -> Fill(PrimaryG4P_Eta->at(true_i), TruthPV_trig_z);
            // }

            // if (h2D_map.find("h2D_Inclusive100_TrueEtaVtxZ_FineBin") != h2D_map.end()){
            //     h2D_map["h2D_Inclusive100_TrueEtaVtxZ_FineBin"] -> Fill(PrimaryG4P_Eta->at(true_i), TruthPV_trig_z);
            // }    

            // if (h1D_map.find("h1D_TrueEta_Inclusive100_VtxZ%d", TruthVtxZ_bin) != h1D_map.end()){
            //     h1D_map["h1D_TrueEta_Inclusive100_VtxZ%d", TruthVtxZ_bin] -> Fill(PrimaryG4P_Eta->at(true_i));
            // }

            // if (0 <= MBD_centrality && MBD_centrality < 0.7){
            //     if (h2D_map.find("h2D_Inclusive70_TrueEtaVtxZ") != h2D_map.end()){
            //         h2D_map["h2D_Inclusive70_TrueEtaVtxZ"] -> Fill(PrimaryG4P_Eta->at(true_i), TruthPV_trig_z);
            //     }

            //     if (h2D_map.find("h2D_Inclusive70_TrueEtaVtxZ_FineBin") != h2D_map.end()){
            //         h2D_map["h2D_Inclusive70_TrueEtaVtxZ_FineBin"] -> Fill(PrimaryG4P_Eta->at(true_i), TruthPV_trig_z);
            //     }

            //     if (h1D_map.find("h1D_TrueEta_Inclusive70_VtxZ%d", TruthVtxZ_bin) != h1D_map.end()){
            //         h1D_map["h1D_TrueEta_Inclusive70_VtxZ%d", TruthVtxZ_bin] -> Fill(PrimaryG4P_Eta->at(true_i));
            //     }

            // }
        
        } // note : end of G4Particle loop
        
        // todo : for debug
        if (TruthPV_trig_z >= -10 && TruthPV_trig_z < 10){

            h1D_map["debug_h1D_NHadron_Inclusive100"] -> Fill(NHadrons);
            h1D_map["debug_h1D_NHadron_OneEtaBin_Inclusive100"] -> Fill(NHadrons_OneEtaBin);
        }

        
    } // note : end of truth

    // note : FineBin
    // todo : No INTT vtxZ reweighting as this is the MBD_vtxZ filled
    if(h1D_map.find(Form("h1D_centrality_MBDVtxZ%d", MBDVtxz_bin)) != h1D_map.end()){
        h1D_map[Form("h1D_centrality_MBDVtxZ%d", MBDVtxz_bin)] -> Fill(MBD_centrality);
    }
    h2D_map["h2D_MBDVtxZ_Centrality"] -> Fill(MBD_z_vtx, MBD_centrality);


    // ==============================================================================================================================================================================================================
    if (INTT_vtxZ_QA && (MBD_z_vtx - INTTvtxZ < cut_vtxZDiff.first || MBD_z_vtx - INTTvtxZ > cut_vtxZDiff.second) ) {return;}
    if (INTT_vtxZ_QA && (TrapezoidalFitWidth < cut_TrapezoidalFitWidth.first || TrapezoidalFitWidth > cut_TrapezoidalFitWidth.second)){return;}
    if (INTT_vtxZ_QA && (TrapezoidalFWHM < cut_TrapezoidalFWHM.first || TrapezoidalFWHM > cut_TrapezoidalFWHM.second)){return;}
    if (INTT_vtxZ_QA && (INTTvtxZError < cut_INTTvtxZError.first || INTTvtxZError > cut_INTTvtxZError.second)){return;}
    // ==============================================================================================================================================================================================================

    if (vtxZReweight && runnumber != -1){
        std::cout<<"Should have no vtxZ reweighting for data"<<std::endl;
        exit(1);
    }

    if (vtxZReweight && h1D_vtxZReweightFactor == nullptr) {
        std::cout<<"vtxZReweightFactor histogram is not found"<<std::endl;
        exit(1);
    }
    vtxZReweightFactor = (vtxZReweight) ? h1D_vtxZReweightFactor -> GetBinContent(h1D_vtxZReweightFactor -> FindBin(INTTvtxZ)) : 1;
    vtxZReweightFactor = (vtxZReweightFactor > 250) ? 0 : vtxZReweightFactor; // todo: the rejection of the large weight

    h1D_map["h1D_centrality"] -> Fill(MBD_centrality, vtxZReweightFactor);

    if (h2D_map.find("h2D_InttVtxZ_Centrality") != h2D_map.end()){
        h2D_map["h2D_InttVtxZ_Centrality"] -> Fill(INTTvtxZ, MBD_centrality, vtxZReweightFactor);
    }

    if (h1D_map.find(Form("h1D_centrality_InttVtxZ%d",INTTvtxz_bin)) != h1D_map.end()){
        h1D_map[Form("h1D_centrality_InttVtxZ%d",INTTvtxz_bin)] -> Fill(MBD_centrality, vtxZReweightFactor);
    }

    // note : the event counting 
    if (h2D_map.find("h2D_RecoEvtCount_vtxZCentrality") != h2D_map.end()){
        h2D_map["h2D_RecoEvtCount_vtxZCentrality"] -> Fill(INTTvtxZ, MBD_centrality, vtxZReweightFactor);
    }   

    std::vector<pair_str> p_evt_TrackletPair_vec = (evt_TrackletPair_vec != nullptr) ? *(evt_TrackletPair_vec) : std::vector<pair_str>(0);
    std::vector<pair_str> p_evt_TrackletPairRotate_vec = (evt_TrackletPairRotate_vec != nullptr) ? *(evt_TrackletPairRotate_vec) : std::vector<pair_str>(0);

    // std::cout<<"test, evt_TrackletPair_vec->size(): "<<evt_TrackletPair_vec->size()<<" p_evt_TrackletPair_vec.size(): "<<p_evt_TrackletPair_vec.size()<<std::endl;

    for (pair_str pair : p_evt_TrackletPair_vec)
    {
        int eta_bin = h1D_eta_template -> Fill(pair.pair_eta_fit);
        eta_bin = (eta_bin == -1) ? -1 : eta_bin - 1;

        int inner_adc = ClusAdc->at(pair.inner_index);
        int outer_adc = ClusAdc->at(pair.outer_index);
        int inner_phi_size = ClusPhiSize->at(pair.inner_index);
        int outer_phi_size = ClusPhiSize->at(pair.outer_index);

        if (isClusQA.first && (inner_adc <= isClusQA.second.first || outer_adc <= isClusQA.second.first)) {continue;} // note : adc
        if (isClusQA.first && (inner_phi_size > isClusQA.second.second || outer_phi_size > isClusQA.second.second)) {continue;} // note : phi size

        // Pair_DeltaR_vec.push_back(sqrt(pow(pair.delta_phi,2)+pow(pair.delta_eta,2)));
        Pair_DeltaPhi_vec.push_back(fabs(pair.delta_phi));

        h1D_PairDeltaEta_inclusive -> Fill(pair.delta_eta, vtxZReweightFactor);
        h1D_PairDeltaPhi_inclusive -> Fill(pair.delta_phi, vtxZReweightFactor);

        // std::cout<<"Mbin: "<<Mbin<<" eta_bin: "<<eta_bin<<" INTTvtxz_bin: "<<INTTvtxz_bin<<std::endl;

        if(h1D_map.find(Form("h1D_DeltaPhi_Mbin%d_Eta%d_VtxZ%d", Mbin, eta_bin, INTTvtxz_bin)) != h1D_map.end()){
            h1D_map[Form("h1D_DeltaPhi_Mbin%d_Eta%d_VtxZ%d", Mbin, eta_bin, INTTvtxz_bin)] -> Fill(pair.delta_phi, vtxZReweightFactor);
            // std::cout<<"In the if(), "<<Form("h1D_DeltaPhi_Mbin%d_Eta%d_VtxZ%d", Mbin, eta_bin, INTTvtxz_bin)<<std::endl;
        }

        if (pair.delta_phi >= cut_GoodProtoTracklet_DeltaPhi.first && pair.delta_phi <= cut_GoodProtoTracklet_DeltaPhi.second){
         
            if(h2D_map.find(Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d", Mbin)) != h2D_map.end()){
                h2D_map[Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d", Mbin)] -> Fill(pair.pair_eta_fit, INTTvtxZ, vtxZReweightFactor);
            }

            if(h2D_map.find(Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d_FineBin", Mbin)) != h2D_map.end()){
                h2D_map[Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d_FineBin", Mbin)] -> Fill(pair.pair_eta_fit, INTTvtxZ, vtxZReweightFactor);
            }
        }

        if (ClusLadderZId->at(pair.inner_index) != typeA_sensorZID[0] && ClusLadderZId->at(pair.inner_index) != typeA_sensorZID[1]) {continue;}
        if (ClusLadderZId->at(pair.outer_index) != typeA_sensorZID[0] && ClusLadderZId->at(pair.outer_index) != typeA_sensorZID[1]) {continue;}

        if(h1D_map.find(Form("h1D_typeA_DeltaPhi_Mbin%d_Eta%d_VtxZ%d", Mbin, eta_bin, INTTvtxz_bin)) != h1D_map.end()){
            h1D_map[Form("h1D_typeA_DeltaPhi_Mbin%d_Eta%d_VtxZ%d", Mbin, eta_bin, INTTvtxz_bin)] -> Fill(pair.delta_phi, vtxZReweightFactor);
        }

        if (pair.delta_phi >= cut_GoodProtoTracklet_DeltaPhi.first && pair.delta_phi <= cut_GoodProtoTracklet_DeltaPhi.second){
         
            if(h2D_map.find(Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d", Mbin)) != h2D_map.end()){
                h2D_map[Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d", Mbin)] -> Fill(pair.pair_eta_fit, INTTvtxZ, vtxZReweightFactor);
            }

            if(h2D_map.find(Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d_FineBin", Mbin)) != h2D_map.end()){
                h2D_map[Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d_FineBin", Mbin)] -> Fill(pair.pair_eta_fit, INTTvtxZ, vtxZReweightFactor);
            }
        }
    }

    if (isWithRotate){
        for (pair_str pair : p_evt_TrackletPairRotate_vec)
        {
            int eta_bin = h1D_eta_template -> Fill(pair.pair_eta_fit);
            eta_bin = (eta_bin == -1) ? -1 : eta_bin - 1;

            int inner_adc = ClusAdc->at(pair.inner_index);
            int outer_adc = ClusAdc->at(pair.outer_index);
            int inner_phi_size = ClusPhiSize->at(pair.inner_index);
            int outer_phi_size = ClusPhiSize->at(pair.outer_index);

            if (isClusQA.first && (inner_adc <= isClusQA.second.first || outer_adc <= isClusQA.second.first)) {continue;} // note : adc
            if (isClusQA.first && (inner_phi_size > isClusQA.second.second || outer_phi_size > isClusQA.second.second)) {continue;} // note : phi size

            if(h1D_map.find(Form("h1D_DeltaPhi_Mbin%d_Eta%d_VtxZ%d_rotated", Mbin, eta_bin, INTTvtxz_bin)) != h1D_map.end()){
                h1D_map[Form("h1D_DeltaPhi_Mbin%d_Eta%d_VtxZ%d_rotated", Mbin, eta_bin, INTTvtxz_bin)] -> Fill(pair.delta_phi, vtxZReweightFactor);
            }

            if (pair.delta_phi >= cut_GoodProtoTracklet_DeltaPhi.first && pair.delta_phi <= cut_GoodProtoTracklet_DeltaPhi.second){
            
                if(h2D_map.find(Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d_rotated", Mbin)) != h2D_map.end()){
                    h2D_map[Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d_rotated", Mbin)] -> Fill(pair.pair_eta_fit, INTTvtxZ, vtxZReweightFactor);
                }
            }

            if (ClusLadderZId->at(pair.inner_index) != typeA_sensorZID[0] && ClusLadderZId->at(pair.inner_index) != typeA_sensorZID[1]) {continue;}
            if (ClusLadderZId->at(pair.outer_index) != typeA_sensorZID[0] && ClusLadderZId->at(pair.outer_index) != typeA_sensorZID[1]) {continue;}

            if(h1D_map.find(Form("h1D_typeA_DeltaPhi_Mbin%d_Eta%d_VtxZ%d_rotated", Mbin, eta_bin, INTTvtxz_bin)) != h1D_map.end()){
                h1D_map[Form("h1D_typeA_DeltaPhi_Mbin%d_Eta%d_VtxZ%d_rotated", Mbin, eta_bin, INTTvtxz_bin)] -> Fill(pair.delta_phi, vtxZReweightFactor);
            }

            if (pair.delta_phi >= cut_GoodProtoTracklet_DeltaPhi.first && pair.delta_phi <= cut_GoodProtoTracklet_DeltaPhi.second){
            
                if(h2D_map.find(Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d_rotated", Mbin)) != h2D_map.end()){
                    h2D_map[Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d_rotated", Mbin)] -> Fill(pair.pair_eta_fit, INTTvtxZ, vtxZReweightFactor);
                }
            }
        }
    }

    // if (MBD_centrality < 0 || MBD_centrality > 0.7) { return; } // note : 0 - 70% centrality

    long long vec_size = Pair_DeltaPhi_vec.size();
    long long ind[Pair_DeltaPhi_vec.size()];
    TMath::Sort(vec_size, &Pair_DeltaPhi_vec[0], ind, false);
    for (int pair_i = 0; pair_i < vec_size; pair_i++)
    {
        pair_str pair = p_evt_TrackletPair_vec[ind[pair_i]];
        if (Used_Clus_index_map.find(pair.inner_index) != Used_Clus_index_map.end() || Used_Clus_index_map.find(pair.outer_index) != Used_Clus_index_map.end()) { continue; }

        if (Pair_DeltaPhi_vec[ind[pair_i]] > cut_bestPair_DeltaPhi.second) { break; }

        h2D_map["h2D_Inclusive100_BestPairEtaVtxZ"] -> Fill(pair.pair_eta_fit, INTTvtxZ, vtxZReweightFactor);
        h2D_map["h2D_Inclusive100_BestPairEtaVtxZ_FineBin"] -> Fill(pair.pair_eta_fit, INTTvtxZ, vtxZReweightFactor);


        if (MBD_centrality >= 0 && MBD_centrality <= 0.7){    
            h2D_map["h2D_Inclusive70_BestPairEtaVtxZ"] -> Fill(pair.pair_eta_fit, INTTvtxZ, vtxZReweightFactor);
            h2D_map["h2D_Inclusive70_BestPairEtaVtxZ_FineBin"] -> Fill(pair.pair_eta_fit, INTTvtxZ, vtxZReweightFactor);

            h1D_map["h1D_BestPair_Inclusive70_DeltaEta"] -> Fill(pair.delta_eta, vtxZReweightFactor);
            h1D_map["h1D_BestPair_Inclusive70_DeltaPhi"] -> Fill(pair.delta_phi, vtxZReweightFactor);
            h1D_map["h1D_BestPair_Inclusive70_ClusPhiSize"] -> Fill(ClusPhiSize->at(pair.inner_index), vtxZReweightFactor);
            h1D_map["h1D_BestPair_Inclusive70_ClusPhiSize"] -> Fill(ClusPhiSize->at(pair.outer_index), vtxZReweightFactor);
            h1D_map["h1D_BestPair_Inclusive70_ClusAdc"] -> Fill(ClusAdc->at(pair.inner_index), vtxZReweightFactor);
            h1D_map["h1D_BestPair_Inclusive70_ClusAdc"] -> Fill(ClusAdc->at(pair.outer_index), vtxZReweightFactor);
        }

        Used_Clus_index_map.insert(std::make_pair(pair.inner_index, 1));
        Used_Clus_index_map.insert(std::make_pair(pair.outer_index, 1));
    }
    
}

void TrackletHistogramFill::EndRun()
{
    file_out -> cd();

    tree_out_par -> Fill();
    tree_out_par -> Write();

    if (h1D_vtxZReweightFactor != nullptr){
        h1D_vtxZReweightFactor -> Write("h1D_vtxZReweightFactor");
    }
    h1D_eta_template -> Write("h1D_eta_template");
    h1D_vtxz_template -> Write("h1D_vtxz_template");

    h1D_PairDeltaEta_inclusive -> Write("h1D_PairDeltaEta_inclusive");
    h1D_PairDeltaPhi_inclusive -> Write("h1D_PairDeltaPhi_inclusive");

    h1D_centrality_bin -> Write();

    for (auto &pair : h2D_map){
        pair.second -> Write();
    }

    for (auto &pair : h1D_map){
        pair.second -> Write();
    }

    

    file_out -> Close();
}