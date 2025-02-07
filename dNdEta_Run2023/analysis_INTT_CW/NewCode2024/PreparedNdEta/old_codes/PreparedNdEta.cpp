#include "PreparedNdEta.h"

PreparedNdEta::PreparedNdEta(
    int process_id_in,
    int runnumber_in,
    std::string input_directory_in,
    std::string input_file_name_in,
    std::string output_directory_in,
    std::string output_file_name_suffix_in,
    bool ApplyAlphaCorr_in
) : process_id(process_id_in),
    runnumber(runnumber_in),
    input_directory(input_directory_in),
    input_file_name(input_file_name_in),
    output_directory(output_directory_in),
    output_file_name_suffix(output_file_name_suffix_in),
    ApplyAlphaCorr(ApplyAlphaCorr_in)
{

    PrepareInputRootFie();

    PrepareOutPutFileName();
    PrepareOutPutRootFile();
    PrepareHistFits();

    vtxZ_index_map = GetVtxZIndexMap();

    c1 = new TCanvas("c1", "c1", 950, 800);

    h2D_alpha_correction_map_in.clear();
    h2D_alpha_correction_map_out.clear();
}

void PreparedNdEta::PrepareInputRootFie()
{
    file_in = TFile::Open(Form("%s/%s",input_directory.c_str(),input_file_name.c_str()));
    if (file_in == nullptr)
    {
        std::cout << "Error: file_in can not be opened" << std::endl;
        exit(1);
    }

    for (TObject* keyAsObj : *file_in->GetListOfKeys())
    {
        auto key = dynamic_cast<TKey*>(keyAsObj);
        std::string hist_name  = key->GetName();
        std::string class_name = key->GetClassName();

        if (class_name == "TH2D")
        {
            h2D_input_map[hist_name.c_str()] = (TH2D*) file_in -> Get( hist_name.c_str() );
        }
        else if (class_name == "TH1D")
        {
            h1D_input_map[hist_name.c_str()] = (TH1D*) file_in -> Get( hist_name.c_str() );
            h1D_input_map[hist_name.c_str()] -> SetLineColor(kBlack);
            h1D_input_map[hist_name.c_str()] -> SetLineWidth(2);
        }
    }
    std::cout<<"In PrepareInputRootFie(), number of input TH1D: "<< h1D_input_map.size() <<std::endl;
    std::cout<<"In PrepareInputRootFie(), number of input TH2D: "<< h2D_input_map.size() <<std::endl;

    tree_in = (TTree*) file_in -> Get("tree_par");
    
    centrality_edges = 0;
    cut_GoodProtoTracklet_DeltaPhi = 0;

    tree_in -> SetBranchAddress("centrality_edges", &centrality_edges);
    tree_in -> SetBranchAddress("nCentrality_bin", &nCentrality_bin);

    tree_in -> SetBranchAddress("CentralityFineEdge_min", &CentralityFineEdge_min);
    tree_in -> SetBranchAddress("CentralityFineEdge_max", &CentralityFineEdge_max);
    tree_in -> SetBranchAddress("nCentralityFineBin", &nCentralityFineBin);

    tree_in -> SetBranchAddress("EtaEdge_min", &EtaEdge_min);
    tree_in -> SetBranchAddress("EtaEdge_max", &EtaEdge_max);
    tree_in -> SetBranchAddress("nEtaBin", &nEtaBin);

    tree_in -> SetBranchAddress("VtxZEdge_min", &VtxZEdge_min);
    tree_in -> SetBranchAddress("VtxZEdge_max", &VtxZEdge_max);
    tree_in -> SetBranchAddress("nVtxZBin", &nVtxZBin);

    tree_in -> SetBranchAddress("DeltaPhiEdge_min", &DeltaPhiEdge_min);
    tree_in -> SetBranchAddress("DeltaPhiEdge_max", &DeltaPhiEdge_max);
    tree_in -> SetBranchAddress("nDeltaPhiBin", &nDeltaPhiBin);    

    tree_in -> SetBranchAddress("cut_GoodProtoTracklet_DeltaPhi", &cut_GoodProtoTracklet_DeltaPhi);

    tree_in -> GetEntry(0);

    std::cout<<"------------------------------------------------------------"<<std::endl;
    std::cout<<"In PrepareInputRootFile(), nEntries : "<<tree_in->GetEntries()<<std::endl;
    std::cout<<"nCentrality_bin : "<<nCentrality_bin<<std::endl;
    for(int i = 0; i < nCentrality_bin; i++)
    {
        std::cout<<"centrality_edges["<<i<<"] : "<<centrality_edges->at(i)<<std::endl;
    }
    std::cout<<"CentralityFineBin : "<<nCentralityFineBin<<", "<<CentralityFineEdge_min<<", "<<CentralityFineEdge_max<<std::endl;
    std::cout<<"EtaBin : "<<nEtaBin<<", "<<EtaEdge_min<<", "<<EtaEdge_max<<std::endl;
    std::cout<<"VtxZBin : "<<nVtxZBin<<", "<<VtxZEdge_min<<", "<<VtxZEdge_max<<std::endl;
    std::cout<<"DeltaPhiBin : "<<nDeltaPhiBin<<", "<<DeltaPhiEdge_min<<", "<<DeltaPhiEdge_max<<std::endl;
    std::cout<<"DeltaPhi signal region: "<<cut_GoodProtoTracklet_DeltaPhi->first<<" ~ "<<cut_GoodProtoTracklet_DeltaPhi->second<<std::endl;
    std::cout<<"------------------------------------------------------------"<<std::endl;

}

void PreparedNdEta::PrepareOutPutFileName()
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

    output_filename = "PreparedNdEta";
    output_filename = (runnumber != -1) ? "Data_" + output_filename : "MC_" + output_filename;
    output_filename += (ApplyAlphaCorr) ? "_ApplyAlphaCorr" : "";
    output_filename += output_file_name_suffix;
    output_filename += (runnumber != -1) ? Form("_%s_%s.root",runnumber_str.c_str(),job_index.c_str()) : Form("_%s.root",job_index.c_str());
    // output_filename += (runnumber != -1) ? Form("_%s.root",runnumber_str.c_str()) : Form(".root");
}

void PreparedNdEta::PrepareOutPutRootFile()
{
    file_out = new TFile(Form("%s/%s",output_directory.c_str(),output_filename.c_str()), "RECREATE");
    file_out_dNdEta = new TFile(Form("%s/%s_dNdEta.root",output_directory.c_str(),output_filename.c_str()), "RECREATE");
}

void PreparedNdEta::PrepareTrackletEtaHist(std::map<std::string, TH1D*> &map_in, std::string name_in)
{
    map_in.clear();

    for (int Mbin = 0; Mbin < nCentrality_bin; Mbin++){
        map_in.insert( std::make_pair(
                Form("h1D_Mbin%d", Mbin),
                new TH1D(Form("h1D_%s_Mbin%d", name_in.c_str(), Mbin), Form("h1D_%s_Mbin%d;Tracklet #eta;Entries", name_in.c_str(), Mbin), nEtaBin, EtaEdge_min, EtaEdge_max)
            )
        ).second;


        map_in.insert( std::make_pair(
                Form("h1D_typeA_Mbin%d", Mbin),
                new TH1D(Form("h1D_typeA_%s_Mbin%d", name_in.c_str(), Mbin), Form("h1D_typeA_%s_Mbin%d;Tracklet #eta;Entries", name_in.c_str(), Mbin), nEtaBin, EtaEdge_min, EtaEdge_max)
            )
        ).second;
    }

    map_in.insert( std::make_pair(
            Form("h1D_Inclusive100"),
            new TH1D(Form("h1D_%s_Inclusive100", name_in.c_str()), Form("h1D_%s_Inclusive100;Tracklet #eta;Entries", name_in.c_str()), nEtaBin, EtaEdge_min, EtaEdge_max)
        )
    ).second;
    
    map_in.insert( std::make_pair(
            Form("h1D_Inclusive70"),
            new TH1D(Form("h1D_%s_Inclusive70", name_in.c_str()), Form("h1D_%s_Inclusive70;Tracklet #eta;Entries", name_in.c_str()), nEtaBin, EtaEdge_min, EtaEdge_max)
        )
    ).second;

    map_in.insert( std::make_pair(
            Form("h1D_typeA_Inclusive100"),
            new TH1D(Form("h1D_typeA_%s_Inclusive100", name_in.c_str()), Form("h1D_typeA_%s_Inclusive100;Tracklet #eta;Entries", name_in.c_str()), nEtaBin, EtaEdge_min, EtaEdge_max)
        )
    ).second;
    
    map_in.insert( std::make_pair(
            Form("h1D_typeA_Inclusive70"),
            new TH1D(Form("h1D_typeA_%s_Inclusive70", name_in.c_str()), Form("h1D_typeA_%s_Inclusive70;Tracklet #eta;Entries", name_in.c_str()), nEtaBin, EtaEdge_min, EtaEdge_max)
        )
    ).second;

}

void PreparedNdEta::PrepareHistFits()
{
    std::vector<int> insert_check; insert_check.clear();
    bool isInserted = false;

    // Division:-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // note : multiplicity, dNdeta, type A, inclusive[centrality segemntation, 70%, 100%]
    PrepareTrackletEtaHist(h1D_FitBkg_RecoTrackletEta_map, "FitBkg_RecoTrackletEta");
    // PrepareTrackletEtaHist(h1D_FitBkg_RecoTrackletEtaPerEvt_map);
    // PrepareTrackletEtaHist(h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_map);
    PrepareTrackletEtaHist(h1D_RotatedBkg_RecoTrackletEta_map, "RotatedBkg_RecoTrackletEta");
    // PrepareTrackletEtaHist(h1D_RotatedBkg_RecoTrackletEtaPerEvt_map);
    // PrepareTrackletEtaHist(h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map);
    


    // Division:-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    hstack1D_map.clear();
    if (runnumber == -1){
        for (int Mbin = 0; Mbin < nCentrality_bin; Mbin++){
            isInserted = hstack1D_map.insert( std::make_pair(
                Form("hstack1D_TrueEta_Mbin%d", Mbin),
                new THStack(Form("hstack1D_TrueEta_Mbin%d", Mbin), Form("hstack1D_TrueEta_Mbin%d;PHG4Particle #eta;Entries", Mbin))
            )
            ).second; insert_check.push_back(isInserted);
        }

        isInserted = hstack1D_map.insert( std::make_pair(
                Form("hstack1D_TrueEta_Inclusive100"),
                new THStack(Form("hstack1D_TrueEta_Inclusive100"), Form("hstack1D_TrueEta_Inclusive100;PHG4Particle #eta;Entries"))
        )
        ).second; insert_check.push_back(isInserted);

        isInserted = hstack1D_map.insert( std::make_pair(
                Form("hstack1D_TrueEta_Inclusive70"),
                new THStack(Form("hstack1D_TrueEta_Inclusive70"), Form("hstack1D_TrueEta_Inclusive70;PHG4Particle #eta;Entries"))
        )
        ).second; insert_check.push_back(isInserted);
    }

    // Division:-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // note : for the delta phi, {typeA, rotated} x {eta} x {Mbin [0-100%], Inclusive70, Inclusive100}
    for (int eta_bin = 0; eta_bin < nEtaBin; eta_bin++){
        for (int Mbin = 0; Mbin < nCentrality_bin; Mbin++){
            isInserted = hstack1D_map.insert( std::make_pair(
                    Form("hstack1D_DeltaPhi_Mbin%d_Eta%d", Mbin, eta_bin),
                    new THStack(Form("hstack1D_DeltaPhi_Mbin%d_Eta%d", Mbin, eta_bin), Form("hstack1D_DeltaPhi_Mbin%d_Eta%d;Pair #Delta#phi [radian];Entries", Mbin, eta_bin))
                )
            ).second; insert_check.push_back(isInserted);

            
            isInserted = hstack1D_map.insert( std::make_pair(
                    Form("hstack1D_DeltaPhi_Mbin%d_Eta%d_rotated", Mbin, eta_bin),
                    new THStack(Form("hstack1D_DeltaPhi_Mbin%d_Eta%d_rotated", Mbin, eta_bin), Form("hstack1D_DeltaPhi_Mbin%d_Eta%d_rotated;Pair #Delta#phi [radian];Entries", Mbin, eta_bin))
                )
            ).second; insert_check.push_back(isInserted);
            


            isInserted = hstack1D_map.insert( std::make_pair(
                    Form("hstack1D_typeA_DeltaPhi_Mbin%d_Eta%d", Mbin, eta_bin),
                    new THStack(Form("hstack1D_typeA_DeltaPhi_Mbin%d_Eta%d", Mbin, eta_bin), Form("hstack1D_typeA_DeltaPhi_Mbin%d_Eta%d;Pair #Delta#phi [radian] (type A);Entries", Mbin, eta_bin))
                )
            ).second; insert_check.push_back(isInserted);

            isInserted = hstack1D_map.insert( std::make_pair(
                    Form("hstack1D_typeA_DeltaPhi_Mbin%d_Eta%d_rotated", Mbin, eta_bin),
                    new THStack(Form("hstack1D_typeA_DeltaPhi_Mbin%d_Eta%d_rotated", Mbin, eta_bin), Form("hstack1D_typeA_DeltaPhi_Mbin%d_Eta%d_rotated;Pair #Delta#phi [radian] (type A);Entries", Mbin, eta_bin))
                )
            ).second; insert_check.push_back(isInserted);
        }

        // note : inclusive 100
        isInserted = hstack1D_map.insert( std::make_pair(
                Form("hstack1D_DeltaPhi_Eta%d_Inclusive100", eta_bin),
                new THStack(Form("hstack1D_DeltaPhi_Eta%d_Inclusive100", eta_bin), Form("hstack1D_DeltaPhi_Eta%d_Inclusive100;Pair #Delta#phi [radian];Entries", eta_bin))
            )
        ).second; insert_check.push_back(isInserted);

        
        isInserted = hstack1D_map.insert( std::make_pair(
                Form("hstack1D_DeltaPhi_Eta%d_Inclusive100_rotated", eta_bin),
                new THStack(Form("hstack1D_DeltaPhi_Eta%d_Inclusive100_rotated", eta_bin), Form("hstack1D_DeltaPhi_Eta%d_Inclusive100_rotated;Pair #Delta#phi [radian];Entries", eta_bin))
            )
        ).second; insert_check.push_back(isInserted);
        

        isInserted = hstack1D_map.insert( std::make_pair(
                Form("hstack1D_typeA_DeltaPhi_Eta%d_Inclusive100", eta_bin),
                new THStack(Form("hstack1D_typeA_DeltaPhi_Eta%d_Inclusive100", eta_bin), Form("hstack1D_typeA_DeltaPhi_Eta%d_Inclusive100;Pair #Delta#phi [radian] (type A);Entries", eta_bin))
            )
        ).second; insert_check.push_back(isInserted);

        isInserted = hstack1D_map.insert( std::make_pair(
                Form("hstack1D_typeA_DeltaPhi_Eta%d_Inclusive100_rotated", eta_bin),
                new THStack(Form("hstack1D_typeA_DeltaPhi_Eta%d_Inclusive100_rotated", eta_bin), Form("hstack1D_typeA_DeltaPhi_Eta%d_Inclusive100_rotated;Pair #Delta#phi [radian] (type A);Entries", eta_bin))
            )
        ).second; insert_check.push_back(isInserted);

        // note : inclusive 70
        isInserted = hstack1D_map.insert( std::make_pair(
                Form("hstack1D_DeltaPhi_Eta%d_Inclusive70", eta_bin),
                new THStack(Form("hstack1D_DeltaPhi_Eta%d_Inclusive70", eta_bin), Form("hstack1D_DeltaPhi_Eta%d_Inclusive70;Pair #Delta#phi [radian];Entries", eta_bin))
            )
        ).second; insert_check.push_back(isInserted);

        
        isInserted = hstack1D_map.insert( std::make_pair(
                Form("hstack1D_DeltaPhi_Eta%d_Inclusive70_rotated", eta_bin),
                new THStack(Form("hstack1D_DeltaPhi_Eta%d_Inclusive70_rotated", eta_bin), Form("hstack1D_DeltaPhi_Eta%d_Inclusive70_rotated;Pair #Delta#phi [radian];Entries", eta_bin))
            )
        ).second; insert_check.push_back(isInserted);
        

        isInserted = hstack1D_map.insert( std::make_pair(
                Form("hstack1D_typeA_DeltaPhi_Eta%d_Inclusive70", eta_bin),
                new THStack(Form("hstack1D_typeA_DeltaPhi_Eta%d_Inclusive70", eta_bin), Form("hstack1D_typeA_DeltaPhi_Eta%d_Inclusive70;Pair #Delta#phi [radian] (type A);Entries", eta_bin))
            )
        ).second; insert_check.push_back(isInserted);

        isInserted = hstack1D_map.insert( std::make_pair(
                Form("hstack1D_typeA_DeltaPhi_Eta%d_Inclusive70_rotated", eta_bin),
                new THStack(Form("hstack1D_typeA_DeltaPhi_Eta%d_Inclusive70_rotated", eta_bin), Form("hstack1D_typeA_DeltaPhi_Eta%d_Inclusive70_rotated;Pair #Delta#phi [radian] (type A);Entries", eta_bin))
            )
        ).second; insert_check.push_back(isInserted);
    } // note : end of eta_bin loop

    // Division:-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // note : for the fittings, {typeA, rotated} x {eta} x {Mbin [0-100%], Inclusive70, Inclusive100}
    f1_BkgPol2_Fit_map.clear();
    f1_BkgPol2_Draw_map.clear();

    f1_SigBkgPol2_Fit_map.clear();
    f1_SigBkgPol2_DrawSig_map.clear();
    f1_SigBkgPol2_DrawBkgPol2_map.clear();

    for (auto &pair : hstack1D_map){
        if (pair.first.find("hstack1D_DeltaPhi") != std::string::npos || pair.first.find("hstack1D_typeA_DeltaPhi") != std::string::npos){

            std::string f1_name = pair.first + "_BkgPol2_Fit";
            isInserted = f1_BkgPol2_Fit_map.insert(
                std::make_pair(
                    f1_name,
                    new TF1(f1_name.c_str(), bkg_pol2_func, DeltaPhiEdge_min, DeltaPhiEdge_max, 6)
                )
            ).second; insert_check.push_back(isInserted);
            f1_BkgPol2_Fit_map[f1_name] -> SetNpx(1000);


            f1_name = pair.first + "_BkgPol2_Draw";
            isInserted = f1_BkgPol2_Draw_map.insert(
                std::make_pair(
                    f1_name,
                    new TF1(f1_name.c_str(), full_pol2_func, DeltaPhiEdge_min, DeltaPhiEdge_max, 4)
                )
            ).second; insert_check.push_back(isInserted);
            f1_BkgPol2_Draw_map[f1_name] -> SetLineColor(6);
            f1_BkgPol2_Draw_map[f1_name] -> SetLineStyle(9);
            f1_BkgPol2_Draw_map[f1_name] -> SetLineWidth(3);
            f1_BkgPol2_Draw_map[f1_name] -> SetNpx(1000);

            
            f1_name = pair.first + "_SigBkgPol2_Fit";
            isInserted = f1_SigBkgPol2_Fit_map.insert(
                std::make_pair(
                    f1_name,
                    new TF1(f1_name.c_str(), gaus_pol2_func, DeltaPhiEdge_min, DeltaPhiEdge_max, 7)
                )
            ).second; insert_check.push_back(isInserted);
            f1_SigBkgPol2_Fit_map[f1_name] -> SetLineColor(46);
            f1_SigBkgPol2_Fit_map[f1_name] -> SetLineStyle(1);
            f1_SigBkgPol2_Fit_map[f1_name] -> SetNpx(1000);


            f1_name = pair.first + "_SigBkgPol2_DrawSig";
            isInserted = f1_SigBkgPol2_DrawSig_map.insert(
                std::make_pair(
                    f1_name,
                    new TF1(f1_name.c_str(), gaus_func, DeltaPhiEdge_min, DeltaPhiEdge_max, 4)
                )
            ).second; insert_check.push_back(isInserted);
            f1_SigBkgPol2_DrawSig_map[f1_name] -> SetLineColor(46);
            f1_SigBkgPol2_DrawSig_map[f1_name] -> SetLineStyle(2);
            f1_SigBkgPol2_DrawSig_map[f1_name] -> SetNpx(1000);


            f1_name = pair.first + "_SigBkgPol2_DrawBkgPol2";
            isInserted = f1_SigBkgPol2_DrawBkgPol2_map.insert(
                std::make_pair(
                    f1_name,
                    new TF1(f1_name.c_str(), full_pol2_func, DeltaPhiEdge_min, DeltaPhiEdge_max, 4)
                )
            ).second; insert_check.push_back(isInserted);
            f1_SigBkgPol2_DrawBkgPol2_map[f1_name] -> SetLineColor(8);
            f1_SigBkgPol2_DrawBkgPol2_map[f1_name] -> SetLineStyle(2);
            f1_SigBkgPol2_DrawBkgPol2_map[f1_name] -> SetNpx(1000);

        }
    }

    // Division:-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // note : signal (eta-vtxZ) [centrality segemntation, 70% and 100%]
    h2D_map.clear();


    // Division:-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // note : truth tracks (eta-vtxZ) [70% and 100%], {coarse bin, fine bin}

    hstack2D_map.clear();
    if (runnumber == -1){

        // note : centrality inclusive 
        isInserted = hstack2D_map.insert( std::make_pair(
                Form("hstack2D_TrueEtaVtxZ_Inclusive100"),
                new THStack(Form("hstack2D_TrueEtaVtxZ_Inclusive100"), Form("hstack2D_TrueEtaVtxZ_Inclusive100;PHG4Particle #eta;TruthPV_trig_z [cm]"))
            )
        ).second; insert_check.push_back(isInserted);

        isInserted = hstack2D_map.insert( std::make_pair(
                Form("hstack2D_TrueEtaVtxZ_Inclusive100_FineBin"),
                new THStack(Form("hstack2D_TrueEtaVtxZ_Inclusive100_FineBin"), Form("hstack2D_TrueEtaVtxZ_Inclusive100_FineBin;PHG4Particle #eta;TruthPV_trig_z [cm]"))
            )
        ).second; insert_check.push_back(isInserted);

        // note : centrality inclusive 
        isInserted = hstack2D_map.insert( std::make_pair(
                Form("hstack2D_TrueEtaVtxZ_Inclusive70"),
                new THStack(Form("hstack2D_TrueEtaVtxZ_Inclusive70"), Form("hstack2D_TrueEtaVtxZ_Inclusive70;PHG4Particle #eta;TruthPV_trig_z [cm]"))
            )
        ).second; insert_check.push_back(isInserted);

        isInserted = hstack2D_map.insert( std::make_pair(
                Form("hstack2D_TrueEtaVtxZ_Inclusive70_FineBin"),
                new THStack(Form("hstack2D_TrueEtaVtxZ_Inclusive70_FineBin"), Form("hstack2D_TrueEtaVtxZ_Inclusive70_FineBin;PHG4Particle #eta;TruthPV_trig_z [cm]"))
            )
        ).second; insert_check.push_back(isInserted);   
    }
    
    // Division:-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // note : good pair (eta-vtxZ) [70% and 100%], {typeA, rotated, normal} {coarse bin, fine bin}

    // note : inclusive 100
    isInserted = hstack2D_map.insert( std::make_pair(
            Form("hstack2D_GoodProtoTracklet_EtaVtxZ_Inclusive100"),
            new THStack(Form("hstack2D_GoodProtoTracklet_EtaVtxZ_Inclusive100"), Form("hstack2D_GoodProtoTracklet_EtaVtxZ_Inclusive100;Pair #eta;INTT vtxZ [cm]")) 
        )
    ).second; insert_check.push_back(isInserted);

    isInserted = hstack2D_map.insert( std::make_pair(
            Form("hstack2D_typeA_GoodProtoTracklet_EtaVtxZ_Inclusive100"),
            new THStack(Form("hstack2D_typeA_GoodProtoTracklet_EtaVtxZ_Inclusive100"), Form("hstack2D_typeA_GoodProtoTracklet_EtaVtxZ_Inclusive100;Pair #eta;INTT vtxZ [cm]")) 
        )
    ).second; insert_check.push_back(isInserted);

    // note : normal fine bin
    isInserted = hstack2D_map.insert( std::make_pair(
            Form("hstack2D_GoodProtoTracklet_EtaVtxZ_Inclusive100_FineBin"),
            new THStack(Form("hstack2D_GoodProtoTracklet_EtaVtxZ_Inclusive100_FineBin"), Form("hstack2D_GoodProtoTracklet_EtaVtxZ_Inclusive100_FineBin;Pair #eta;INTT vtxZ [cm]")) 
        )
    ).second; insert_check.push_back(isInserted);

    isInserted = hstack2D_map.insert( std::make_pair(
            Form("hstack2D_typeA_GoodProtoTracklet_EtaVtxZ_Inclusive100_FineBin"),
            new THStack(Form("hstack2D_typeA_GoodProtoTracklet_EtaVtxZ_Inclusive100_FineBin"), Form("hstack2D_typeA_GoodProtoTracklet_EtaVtxZ_Inclusive100_FineBin;Pair #eta;INTT vtxZ [cm]")) 
        )
    ).second; insert_check.push_back(isInserted);
    

    // note : rotated
    isInserted = hstack2D_map.insert( std::make_pair(
            Form("hstack2D_GoodProtoTracklet_EtaVtxZ_Inclusive100_rotated"),
            new THStack(Form("hstack2D_GoodProtoTracklet_EtaVtxZ_Inclusive100_rotated"), Form("hstack2D_GoodProtoTracklet_EtaVtxZ_Inclusive100_rotated;Pair #eta;INTT vtxZ [cm]")) 
        )
    ).second; insert_check.push_back(isInserted);

    isInserted = hstack2D_map.insert( std::make_pair(
            Form("hstack2D_typeA_GoodProtoTracklet_EtaVtxZ_Inclusive100_rotated"),
            new THStack(Form("hstack2D_typeA_GoodProtoTracklet_EtaVtxZ_Inclusive100_rotated"), Form("hstack2D_typeA_GoodProtoTracklet_EtaVtxZ_Inclusive100_rotated;Pair #eta;INTT vtxZ [cm]")) 
        )
    ).second; insert_check.push_back(isInserted);

    // note : inclusive 70
    isInserted = hstack2D_map.insert( std::make_pair(
            Form("hstack2D_GoodProtoTracklet_EtaVtxZ_Inclusive70"),
            new THStack(Form("hstack2D_GoodProtoTracklet_EtaVtxZ_Inclusive70"), Form("hstack2D_GoodProtoTracklet_EtaVtxZ_Inclusive70;Pair #eta;INTT vtxZ [cm]")) 
        )
    ).second; insert_check.push_back(isInserted);

    isInserted = hstack2D_map.insert( std::make_pair(
            Form("hstack2D_typeA_GoodProtoTracklet_EtaVtxZ_Inclusive70"),
            new THStack(Form("hstack2D_typeA_GoodProtoTracklet_EtaVtxZ_Inclusive70"), Form("hstack2D_typeA_GoodProtoTracklet_EtaVtxZ_Inclusive70;Pair #eta;INTT vtxZ [cm]")) 
        )
    ).second; insert_check.push_back(isInserted);

    // note : normal fine bin
    isInserted = hstack2D_map.insert( std::make_pair(
            Form("hstack2D_GoodProtoTracklet_EtaVtxZ_Inclusive70_FineBin"),
            new THStack(Form("hstack2D_GoodProtoTracklet_EtaVtxZ_Inclusive70_FineBin"), Form("hstack2D_GoodProtoTracklet_EtaVtxZ_Inclusive70_FineBin;Pair #eta;INTT vtxZ [cm]")) 
        )
    ).second; insert_check.push_back(isInserted);

    isInserted = hstack2D_map.insert( std::make_pair(
            Form("hstack2D_typeA_GoodProtoTracklet_EtaVtxZ_Inclusive70_FineBin"),
            new THStack(Form("hstack2D_typeA_GoodProtoTracklet_EtaVtxZ_Inclusive70_FineBin"), Form("hstack2D_typeA_GoodProtoTracklet_EtaVtxZ_Inclusive70_FineBin;Pair #eta;INTT vtxZ [cm]")) 
        )
    ).second; insert_check.push_back(isInserted);
    
    // note : rotated
    isInserted = hstack2D_map.insert( std::make_pair(
            Form("hstack2D_GoodProtoTracklet_EtaVtxZ_Inclusive70_rotated"),
            new THStack(Form("hstack2D_GoodProtoTracklet_EtaVtxZ_Inclusive70_rotated"), Form("hstack2D_GoodProtoTracklet_EtaVtxZ_Inclusive70_rotated;Pair #eta;INTT vtxZ [cm]")) 
        )
    ).second; insert_check.push_back(isInserted);

    isInserted = hstack2D_map.insert( std::make_pair(
            Form("hstack2D_typeA_GoodProtoTracklet_EtaVtxZ_Inclusive70_rotated"),
            new THStack(Form("hstack2D_typeA_GoodProtoTracklet_EtaVtxZ_Inclusive70_rotated"), Form("hstack2D_typeA_GoodProtoTracklet_EtaVtxZ_Inclusive70_rotated;Pair #eta;INTT vtxZ [cm]")) 
        )
    ).second; insert_check.push_back(isInserted);


    for (int isInserted_tag : insert_check){
        if (isInserted_tag == 0){
            std::cout<<"Histogram insertion failed"<<std::endl;
            exit(1);
        }
    }

}

std::map<int, int> PreparedNdEta::GetVtxZIndexMap()
{
    if(h2D_input_map.find("h2D_RecoEvtCount_vtxZCentrality") == h2D_input_map.end()){
        std::cout<<"Error: h2D_RecoEvtCount_vtxZCentrality not found"<<std::endl;
        exit(1);
    }

    std::map<int, int> vtxZIndexMap; vtxZIndexMap.clear();

    for (int y_i = 1; y_i <= h2D_input_map["h2D_RecoEvtCount_vtxZCentrality"]->GetNbinsY(); y_i++){

        if (h2D_input_map["h2D_RecoEvtCount_vtxZCentrality"]->GetXaxis()->GetBinLowEdge(y_i) < cut_INTTvtxZ.first || h2D_input_map["h2D_RecoEvtCount_vtxZCentrality"]->GetXaxis()->GetBinLowEdge(y_i) > cut_INTTvtxZ.second){
            continue;
        }
        if (h2D_input_map["h2D_RecoEvtCount_vtxZCentrality"]->GetXaxis()->GetBinUpEdge(y_i) < cut_INTTvtxZ.first || h2D_input_map["h2D_RecoEvtCount_vtxZCentrality"]->GetXaxis()->GetBinUpEdge(y_i) > cut_INTTvtxZ.second){
            continue;
        }

        vtxZIndexMap.insert(
            std::make_pair(
                y_i - 1,
                y_i - 1
            )
        );
    }

    std::cout<<"The selected INTT vtxZ bin : [";
    for (auto &pair : vtxZIndexMap){
        std::cout<<pair.first<<", ";
    }
    std::cout<<"]"<<std::endl;

    for (auto &pair : vtxZIndexMap){
        std::cout<<"vtxZ index : "<<pair.first<<", range : {"<<h2D_input_map["h2D_RecoEvtCount_vtxZCentrality"]->GetXaxis()->GetBinLowEdge(pair.first + 1)<<", "<<h2D_input_map["h2D_RecoEvtCount_vtxZCentrality"]->GetXaxis()->GetBinUpEdge(pair.first + 1)<<"}"<<std::endl;
    }

    return vtxZIndexMap;
}

// note : return the bin index of eta, vtxZ, Mbin, typeA, rotated, finebin
std::tuple<int, int, int, int, int, int> PreparedNdEta::GetHistStringInfo(std::string hist_name)
{

    std::cout<<"In GetHistStringInfo(), input name: "<<hist_name<<std::endl;
    
    int eta_bin;
    int vtxz_bin;
    int Mbin;

    std::string eta_bin_str = "";
    std::string vtxz_bin_str = "";
    std::string Mbin_str = "";

    // note : eta bin
    if (hist_name.find("_Eta") == std::string::npos) {eta_bin = -1;}
    else {
        for (int i = 0; i < hist_name.size(); i++){
            int start_index = hist_name.find("_Eta") + 4 + i;
            if (hist_name[start_index] != '_' && isdigit(hist_name[start_index])){
                eta_bin_str += hist_name[start_index];
            }
            else {
                break;
            }

            if (start_index == hist_name.size() - 1){
                break;
            }
        }

        eta_bin = (eta_bin_str.length() == 0) ? -1 : std::stoi(eta_bin_str);
    }

    

    // note : vtxZ bin
    if (hist_name.find("_VtxZ") == std::string::npos) {vtxz_bin = -1;}
    else {
        for (int i = 0; i < hist_name.size(); i++){
            int start_index = hist_name.find("_VtxZ") + 5 + i;
            if (hist_name[start_index] != '_' && isdigit(hist_name[start_index])){
                vtxz_bin_str += hist_name[start_index];
            }
            else {
                break;
            }

            if (start_index == hist_name.size() - 1){
                break;
            }
        }

        vtxz_bin = (vtxz_bin_str.length() == 0) ? -1 : std::stoi(vtxz_bin_str);
    }

    

    // note : Mbin
    if (hist_name.find("_Mbin") == std::string::npos) {Mbin = -1;}
    else {
        for (int i = 0; i < hist_name.size(); i++){
            int start_index = hist_name.find("_Mbin") + 5 + i;
            if (hist_name[start_index] != '_' && isdigit(hist_name[start_index])){
                Mbin_str += hist_name[start_index];
            }
            else {
                break;
            }

            if (start_index == hist_name.size() - 1){
                break;
            }
        }

        Mbin = (Mbin_str.length() == 0) ? -1 : std::stoi(Mbin_str);
    }

    // note : typeA, rotated, finebin
    int typeA = (hist_name.find("_typeA") != std::string::npos) ? 1 : 0;
    int rotated = (hist_name.find("_rotated") != std::string::npos) ? 1 : 0;
    int finebin = (hist_name.find("_FineBin") != std::string::npos) ? 1 : 0;


    return std::make_tuple(eta_bin, vtxz_bin, Mbin, typeA, rotated, finebin);
}

void PreparedNdEta::PrepareStacks()
{   

    std::cout<<"In PrepareStacks()"<<std::endl;

    int h1D_eta_bin, h1D_vtxz_bin, h1D_Mbin;
    int h2D_eta_bin, h2D_vtxz_bin, h2D_Mbin;
    int typeA, rotated, finebin;

    for (auto &pair : h1D_input_map)
    {
        std::tie(h1D_eta_bin, h1D_vtxz_bin, h1D_Mbin, typeA, rotated, finebin) = GetHistStringInfo(pair.first);

        // note : the vtxz_bin of the hist is not in the interest
        if (vtxZ_index_map.find(h1D_vtxz_bin) == vtxZ_index_map.end()){
            continue;
        }

        // note : reco, pair DeltaPhi for each Mbin and eta_bin, stacked up by vertex z bin
        // note : with typeA, rotate, inclusive100, inclusive70
        if (pair.first.find("h1D") != std::string::npos && pair.first.find("_DeltaPhi") != std::string::npos && h1D_Mbin != -1 && h1D_eta_bin != -1){
            std::cout<<"----- "<<pair.first<<", eta_bin: "<<h1D_eta_bin<<", vtxz_bin: "<<h1D_vtxz_bin<<", Mbin: "<<h1D_Mbin<<", typeA: "<<typeA<<", rotated: "<<rotated<<", finebin: "<<finebin<<std::endl;


            if (typeA == 0 && rotated == 0){
                pair.second->SetFillColor(ROOT_color_code[hstack1D_map[Form("hstack1D_DeltaPhi_Mbin%d_Eta%d", h1D_Mbin, h1D_eta_bin)]->GetNhists() % ROOT_color_code.size()]);
                if ( hstack1D_map.find(Form("hstack1D_DeltaPhi_Mbin%d_Eta%d", h1D_Mbin, h1D_eta_bin)) == hstack1D_map.end() ) {
                    std::cout<<Form("hstack1D_DeltaPhi_Mbin%d_Eta%d", h1D_Mbin, h1D_eta_bin)<<" not found in hstack1D_map !!"<<std::endl;
                    exit(1);
                } 
                hstack1D_map[Form("hstack1D_DeltaPhi_Mbin%d_Eta%d", h1D_Mbin, h1D_eta_bin)]->Add(pair.second);

                if ( hstack1D_map.find(Form("hstack1D_DeltaPhi_Eta%d_Inclusive100", h1D_eta_bin)) == hstack1D_map.end() ) {
                    std::cout<<Form("hstack1D_DeltaPhi_Eta%d_Inclusive100", h1D_eta_bin)<<" not found in hstack1D_map !!"<<std::endl;
                    exit(1);
                } 
                hstack1D_map[Form("hstack1D_DeltaPhi_Eta%d_Inclusive100", h1D_eta_bin)]->Add(pair.second);
                
                if (h1D_Mbin <= Semi_inclusive_Mbin){
                    if ( hstack1D_map.find(Form("hstack1D_DeltaPhi_Eta%d_Inclusive70", h1D_eta_bin)) == hstack1D_map.end() ) {
                        std::cout<<Form("hstack1D_DeltaPhi_Eta%d_Inclusive70", h1D_eta_bin)<<" not found in hstack1D_map !!"<<std::endl;
                        exit(1);
                    } 
                    hstack1D_map[Form("hstack1D_DeltaPhi_Eta%d_Inclusive70", h1D_eta_bin)]->Add(pair.second);
                }
            }

            if (typeA == 0 && rotated == 1){
                pair.second->SetFillColor(ROOT_color_code[hstack1D_map[Form("hstack1D_DeltaPhi_Mbin%d_Eta%d_rotated", h1D_Mbin, h1D_eta_bin)]->GetNhists() % ROOT_color_code.size()]);
                if ( hstack1D_map.find(Form("hstack1D_DeltaPhi_Mbin%d_Eta%d_rotated", h1D_Mbin, h1D_eta_bin)) == hstack1D_map.end() ) {
                    std::cout<<Form("hstack1D_DeltaPhi_Mbin%d_Eta%d_rotated", h1D_Mbin, h1D_eta_bin)<<" not found in hstack1D_map !!"<<std::endl;
                    exit(1);
                } 
                hstack1D_map[Form("hstack1D_DeltaPhi_Mbin%d_Eta%d_rotated", h1D_Mbin, h1D_eta_bin)]->Add(pair.second);

                if ( hstack1D_map.find(Form("hstack1D_DeltaPhi_Eta%d_Inclusive100_rotated", h1D_eta_bin)) == hstack1D_map.end() ) {
                    std::cout<<Form("hstack1D_DeltaPhi_Eta%d_Inclusive100_rotated", h1D_eta_bin)<<" not found in hstack1D_map !!"<<std::endl;
                    exit(1);
                } 
                hstack1D_map[Form("hstack1D_DeltaPhi_Eta%d_Inclusive100_rotated", h1D_eta_bin)]->Add(pair.second);

                if (h1D_Mbin <= Semi_inclusive_Mbin){
                    if ( hstack1D_map.find(Form("hstack1D_DeltaPhi_Eta%d_Inclusive70_rotated", h1D_eta_bin)) == hstack1D_map.end() ) {
                        std::cout<<Form("hstack1D_DeltaPhi_Eta%d_Inclusive70_rotated", h1D_eta_bin)<<" not found in hstack1D_map !!"<<std::endl;
                        exit(1);
                    } 
                    hstack1D_map[Form("hstack1D_DeltaPhi_Eta%d_Inclusive70_rotated", h1D_eta_bin)]->Add(pair.second);
                }
            }

            // note : type A
            if (typeA == 1 && rotated == 0){
                pair.second->SetFillColor(ROOT_color_code[hstack1D_map[Form("hstack1D_typeA_DeltaPhi_Mbin%d_Eta%d", h1D_Mbin, h1D_eta_bin)]->GetNhists() % ROOT_color_code.size()]);
                if ( hstack1D_map.find(Form("hstack1D_typeA_DeltaPhi_Mbin%d_Eta%d", h1D_Mbin, h1D_eta_bin)) == hstack1D_map.end() ) {
                    std::cout<<Form("hstack1D_typeA_DeltaPhi_Mbin%d_Eta%d", h1D_Mbin, h1D_eta_bin)<<" not found in hstack1D_map !!"<<std::endl;
                    exit(1);
                } 
                hstack1D_map[Form("hstack1D_typeA_DeltaPhi_Mbin%d_Eta%d", h1D_Mbin, h1D_eta_bin)]->Add(pair.second);

                if ( hstack1D_map.find(Form("hstack1D_typeA_DeltaPhi_Eta%d_Inclusive100", h1D_eta_bin)) == hstack1D_map.end() ) {
                    std::cout<<Form("hstack1D_typeA_DeltaPhi_Eta%d_Inclusive100", h1D_eta_bin)<<" not found in hstack1D_map !!"<<std::endl;
                    exit(1);
                } 
                hstack1D_map[Form("hstack1D_typeA_DeltaPhi_Eta%d_Inclusive100", h1D_eta_bin)]->Add(pair.second);

                if (h1D_Mbin <= Semi_inclusive_Mbin){
                    if ( hstack1D_map.find(Form("hstack1D_typeA_DeltaPhi_Eta%d_Inclusive70", h1D_eta_bin)) == hstack1D_map.end() ) {
                        std::cout<<Form("hstack1D_typeA_DeltaPhi_Eta%d_Inclusive70", h1D_eta_bin)<<" not found in hstack1D_map !!"<<std::endl;
                        exit(1);
                    } 
                    hstack1D_map[Form("hstack1D_typeA_DeltaPhi_Eta%d_Inclusive70", h1D_eta_bin)]->Add(pair.second);
                }
            }

            if (typeA == 1 && rotated == 1){
                pair.second->SetFillColor(ROOT_color_code[hstack1D_map[Form("hstack1D_typeA_DeltaPhi_Mbin%d_Eta%d_rotated", h1D_Mbin, h1D_eta_bin)]->GetNhists() % ROOT_color_code.size()]);
                if ( hstack1D_map.find(Form("hstack1D_typeA_DeltaPhi_Mbin%d_Eta%d_rotated", h1D_Mbin, h1D_eta_bin)) == hstack1D_map.end() ) {
                    std::cout<<Form("hstack1D_typeA_DeltaPhi_Mbin%d_Eta%d_rotated", h1D_Mbin, h1D_eta_bin)<<" not found in hstack1D_map !!"<<std::endl;
                    exit(1);
                } 
                hstack1D_map[Form("hstack1D_typeA_DeltaPhi_Mbin%d_Eta%d_rotated", h1D_Mbin, h1D_eta_bin)]->Add(pair.second);

                if ( hstack1D_map.find(Form("hstack1D_typeA_DeltaPhi_Eta%d_Inclusive100_rotated", h1D_eta_bin)) == hstack1D_map.end() ) {
                    std::cout<<Form("hstack1D_typeA_DeltaPhi_Eta%d_Inclusive100_rotated", h1D_eta_bin)<<" not found in hstack1D_map !!"<<std::endl;
                    exit(1);
                } 
                hstack1D_map[Form("hstack1D_typeA_DeltaPhi_Eta%d_Inclusive100_rotated", h1D_eta_bin)]->Add(pair.second);

                if (h1D_Mbin <= Semi_inclusive_Mbin){
                    if ( hstack1D_map.find(Form("hstack1D_typeA_DeltaPhi_Eta%d_Inclusive70_rotated", h1D_eta_bin)) == hstack1D_map.end() ) {
                        std::cout<<Form("hstack1D_typeA_DeltaPhi_Eta%d_Inclusive70_rotated", h1D_eta_bin)<<" not found in hstack1D_map !!"<<std::endl;
                        exit(1);
                    } 
                    hstack1D_map[Form("hstack1D_typeA_DeltaPhi_Eta%d_Inclusive70_rotated", h1D_eta_bin)]->Add(pair.second);
                }
            }



        }


        // note : truth eta distribution, stacked up by vertex Z bin
        // note : different Mbin, inclusive100, inclusive70
        if (runnumber == -1 && pair.first.find("h1D") != std::string::npos && pair.first.find("_TrueEta") != std::string::npos && h1D_Mbin != -1){
            
            pair.second->SetFillColor(ROOT_color_code[hstack1D_map[Form("hstack1D_TrueEta_Mbin%d", h1D_Mbin)]->GetNhists() % ROOT_color_code.size()]);

            if (hstack1D_map.find(Form("hstack1D_TrueEta_Mbin%d", h1D_Mbin)) == hstack1D_map.end()){
                std::cout<<Form("hstack1D_TrueEta_Mbin%d", h1D_Mbin)<<" not found in hstack1D_map !!"<<std::endl;
                exit(1);
            }
            hstack1D_map[Form("hstack1D_TrueEta_Mbin%d", h1D_Mbin)]->Add(pair.second);
            
            if (hstack1D_map.find(Form("hstack1D_TrueEta_Inclusive100")) == hstack1D_map.end()){
                std::cout<<Form("hstack1D_TrueEta_Inclusive100")<<" not found in hstack1D_map !!"<<std::endl;
                exit(1);
            }
            hstack1D_map["hstack1D_TrueEta_Inclusive100"] -> Add(pair.second);

            if (h1D_Mbin <= Semi_inclusive_Mbin){
                if (hstack1D_map.find(Form("hstack1D_TrueEta_Inclusive70")) == hstack1D_map.end()){
                    std::cout<<Form("hstack1D_TrueEta_Inclusive70")<<" not found in hstack1D_map !!"<<std::endl;
                    exit(1);
                }
                hstack1D_map["hstack1D_TrueEta_Inclusive70"] -> Add(pair.second);
            }

        }
    }

    // for (auto &pair : h2D_input_map){
    //     // hstack2D_GoodProtoTracklet_EtaVtxZ_Inclusive100             V
    //     // hstack2D_GoodProtoTracklet_EtaVtxZ_Inclusive100_rotated     V
    //     // hstack2D_GoodProtoTracklet_EtaVtxZ_Inclusive70              V
    //     // hstack2D_GoodProtoTracklet_EtaVtxZ_Inclusive70_rotated      V
    //     // hstack2D_typeA_GoodProtoTracklet_EtaVtxZ_Inclusive100         V
    //     // hstack2D_typeA_GoodProtoTracklet_EtaVtxZ_Inclusive100_rotated V
    //     // hstack2D_typeA_GoodProtoTracklet_EtaVtxZ_Inclusive70          V
    //     // hstack2D_typeA_GoodProtoTracklet_EtaVtxZ_Inclusive70_rotated  V

    //     // hstack2D_GoodProtoTracklet_EtaVtxZ_Inclusive100_FineBin        V
    //     // hstack2D_GoodProtoTracklet_EtaVtxZ_Inclusive70_FineBin         V
    //     // hstack2D_typeA_GoodProtoTracklet_EtaVtxZ_Inclusive100_FineBin  V
    //     // hstack2D_typeA_GoodProtoTracklet_EtaVtxZ_Inclusive70_FineBin   V

    //     std::tie(h2D_eta_bin, h2D_vtxz_bin, h2D_Mbin, typeA, rotated, finebin) = GetHistStringInfo(pair.first);
    //     std::string HS2D = "hstack2D";
    //     std::string GPTEVz = "GoodProtoTracklet_EtaVtxZ";
    //     if (pair.first.find("h2D_") != std::string::npos && pair.first.find(GPTEVz) != std::string::npos && h2D_Mbin != -1){
    //         if (typeA == 0 && rotated == 0 && finebin == 0){
    //             hstack2D_map[Form("%s_%s_Inclusive100", HS2D.c_str(), GPTEVz.c_str())]->Add(pair.second);

    //             if (h2D_Mbin <= Semi_inclusive_Mbin){
    //                 hstack2D_map[Form("%s_%s_Inclusive70", HS2D.c_str(), GPTEVz.c_str())]->Add(pair.second);
    //             }
    //         } 

    //         if (typeA == 0 && rotated == 1 && finebin == 0){
    //             hstack2D_map[Form("%s_%s_Inclusive100_rotated", HS2D.c_str(), GPTEVz.c_str())]->Add(pair.second);

    //             if (h2D_Mbin <= Semi_inclusive_Mbin){
    //                 hstack2D_map[Form("%s_%s_Inclusive70_rotated", HS2D.c_str(), GPTEVz.c_str())]->Add(pair.second);
    //             }
    //         }

    //         if (typeA == 1 && rotated == 0 && finebin == 0){
    //             hstack2D_map[Form("%s_typeA_%s_Inclusive100", HS2D.c_str(), GPTEVz.c_str())]->Add(pair.second);

    //             if (h2D_Mbin <= Semi_inclusive_Mbin){
    //                 hstack2D_map[Form("%s_typeA_%s_Inclusive70", HS2D.c_str(), GPTEVz.c_str())]->Add(pair.second);
    //             }
    //         }

    //         if (typeA == 1 && rotated == 1 && finebin == 0){
    //             hstack2D_map[Form("%s_typeA_%s_Inclusive100_rotated", HS2D.c_str(), GPTEVz.c_str())]->Add(pair.second);

    //             if (h2D_Mbin <= Semi_inclusive_Mbin){
    //                 hstack2D_map[Form("%s_typeA_%s_Inclusive70_rotated", HS2D.c_str(), GPTEVz.c_str())]->Add(pair.second);
    //             }
    //         }

    //         if (typeA == 0 && rotated == 0 && finebin == 1){
    //             hstack2D_map[Form("%s_%s_Inclusive100_FineBin", HS2D.c_str(), GPTEVz.c_str())]->Add(pair.second);

    //             if (h2D_Mbin <= Semi_inclusive_Mbin){
    //                 hstack2D_map[Form("%s_%s_Inclusive70_FineBin", HS2D.c_str(), GPTEVz.c_str())]->Add(pair.second);
    //             }
    //         }

    //         if (typeA == 1 && rotated == 0 && finebin == 1){
    //             hstack2D_map[Form("%s_typeA_%s_Inclusive100_FineBin", HS2D.c_str(), GPTEVz.c_str())]->Add(pair.second);

    //             if (h2D_Mbin <= Semi_inclusive_Mbin){
    //                 hstack2D_map[Form("%s_typeA_%s_Inclusive70_FineBin", HS2D.c_str(), GPTEVz.c_str())]->Add(pair.second);
    //             }
    //         }
    //     }
    // }

    for (int Mbin = 0; Mbin < nCentrality_bin; Mbin++){
        hstack2D_map["hstack2D_GoodProtoTracklet_EtaVtxZ_Inclusive100"] -> Add(h2D_input_map[Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d",Mbin)]);
        hstack2D_map["hstack2D_GoodProtoTracklet_EtaVtxZ_Inclusive100_FineBin"] -> Add(h2D_input_map[Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d_FineBin",Mbin)]);
        hstack2D_map["hstack2D_GoodProtoTracklet_EtaVtxZ_Inclusive100_rotated"] -> Add(h2D_input_map[Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d_rotated",Mbin)]);

        hstack2D_map["hstack2D_typeA_GoodProtoTracklet_EtaVtxZ_Inclusive100"] -> Add(h2D_input_map[Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d",Mbin)]);
        hstack2D_map["hstack2D_typeA_GoodProtoTracklet_EtaVtxZ_Inclusive100_FineBin"] -> Add(h2D_input_map[Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d_FineBin",Mbin)]);
        hstack2D_map["hstack2D_typeA_GoodProtoTracklet_EtaVtxZ_Inclusive100_rotated"] -> Add(h2D_input_map[Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d_rotated",Mbin)]);

        if (Mbin <= Semi_inclusive_Mbin){
            hstack2D_map["hstack2D_GoodProtoTracklet_EtaVtxZ_Inclusive70"] -> Add(h2D_input_map[Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d",Mbin)]);
            hstack2D_map["hstack2D_GoodProtoTracklet_EtaVtxZ_Inclusive70_FineBin"] -> Add(h2D_input_map[Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d_FineBin",Mbin)]);
            hstack2D_map["hstack2D_GoodProtoTracklet_EtaVtxZ_Inclusive70_rotated"] -> Add(h2D_input_map[Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d_rotated",Mbin)]);

            hstack2D_map["hstack2D_typeA_GoodProtoTracklet_EtaVtxZ_Inclusive70"] -> Add(h2D_input_map[Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d",Mbin)]);
            hstack2D_map["hstack2D_typeA_GoodProtoTracklet_EtaVtxZ_Inclusive70_FineBin"] -> Add(h2D_input_map[Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d_FineBin",Mbin)]);
            hstack2D_map["hstack2D_typeA_GoodProtoTracklet_EtaVtxZ_Inclusive70_rotated"] -> Add(h2D_input_map[Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d_rotated",Mbin)]);
        }
    }

    // note : for MC
    // hstack2D_TrueEtaVtxZ_Inclusive100         V
    // hstack2D_TrueEtaVtxZ_Inclusive100_FineBin V
    // hstack2D_TrueEtaVtxZ_Inclusive70          V
    // hstack2D_TrueEtaVtxZ_Inclusive70_FineBin  V
    // hstack1D_TrueEta_Mbin%d", Mbin),          V
    // hstack1D_TrueEta_Inclusive100"),          V
    // hstack1D_TrueEta_Inclusive70"),           V
    if (runnumber == -1){
        for (auto &pair : h2D_input_map)
        {
            std::tie(h2D_eta_bin, h2D_vtxz_bin, h2D_Mbin, typeA, rotated, finebin) = GetHistStringInfo(pair.first);

            if (pair.first.find("h2D_TrueEtaVtxZ") != std::string::npos && h2D_Mbin != -1){
                
                std::cout<<" MCMCMC ----- "<<pair.first<<", eta_bin: "<<h2D_eta_bin<<", vtxz_bin: "<<h2D_vtxz_bin<<", Mbin: "<<h2D_Mbin<<", typeA: "<<typeA<<", rotated: "<<rotated<<", finebin: "<<finebin<<std::endl;

                if(finebin == 1){
                    hstack2D_map[Form("hstack2D_TrueEtaVtxZ_Inclusive100_FineBin")] -> Add(pair.second);

                    if (h2D_Mbin <= Semi_inclusive_Mbin){
                        hstack2D_map[Form("hstack2D_TrueEtaVtxZ_Inclusive70_FineBin")] -> Add(pair.second);
                    }
                }
                else {
                    hstack2D_map[Form("hstack2D_TrueEtaVtxZ_Inclusive100")] -> Add(pair.second);

                    if (h2D_Mbin <= Semi_inclusive_Mbin){
                        hstack2D_map[Form("hstack2D_TrueEtaVtxZ_Inclusive70")] -> Add(pair.second);
                    }
                }
            }
            
            
        }
    }
        



    std::cout<<"End the PrepareStacks()"<<std::endl;
}

void PreparedNdEta::DoFittings()
{
    std::cout<<"In DoFittings()"<<std::endl;

    int stack_count = 0;

    for (auto &pair : hstack1D_map){
        if (pair.first.find("hstack1D") != std::string::npos && pair.first.find("_DeltaPhi") != std::string::npos){
            
            if (stack_count % 20 == 0){
                std::cout<<"Fitting stack : "<<stack_count<<", "<<pair.first<<std::endl;
            }

            auto temp_hist = (TH1D*) pair.second -> GetStack() -> Last();
            std::vector<double> N_group_info = find_Ngroup(temp_hist);

            std::string f1_BkgPol2_Fit_map_key  = pair.first + "_BkgPol2_Fit";
            std::string f1_BkgPol2_Draw_map_key = pair.first + "_BkgPol2_Draw";

            if (f1_BkgPol2_Fit_map.find(f1_BkgPol2_Fit_map_key) == f1_BkgPol2_Fit_map.end()){
                std::cout<<f1_BkgPol2_Fit_map_key<<" not found in f1_BkgPol2_Fit_map !!"<<std::endl;
                exit(1);
            }

            std::string f1_SigBkgPol2_Fit_map_key         = pair.first + "_SigBkgPol2_Fit";
            std::string f1_SigBkgPol2_DrawSig_map_key     = pair.first + "_SigBkgPol2_DrawSig";
            std::string f1_SigBkgPol2_DrawBkgPol2_map_key = pair.first + "_SigBkgPol2_DrawBkgPol2";

            if (f1_SigBkgPol2_Fit_map.find(f1_SigBkgPol2_Fit_map_key) == f1_SigBkgPol2_Fit_map.end()){
                std::cout<<f1_SigBkgPol2_Fit_map_key<<" not found in f1_SigBkgPol2_Fit_map !!"<<std::endl;
                exit(1);
            }

            double hist_offset = get_dist_offset(temp_hist, 15); // todo:
            double signal_region_l = cut_GoodProtoTracklet_DeltaPhi->first;
            double signal_region_r = cut_GoodProtoTracklet_DeltaPhi->second; 

            // todo:
            // note : par[0] + par[1]* (x[0]-par[3]) + par[2] * pow((x[0]-par[3]),2);
            // std::cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<std::endl;
            f1_BkgPol2_Fit_map[f1_BkgPol2_Fit_map_key] -> SetParameters(hist_offset, 0, -0.2, 0, signal_region_l, signal_region_r);
            f1_BkgPol2_Fit_map[f1_BkgPol2_Fit_map_key] -> FixParameter(4, signal_region_l);
            f1_BkgPol2_Fit_map[f1_BkgPol2_Fit_map_key] -> FixParameter(5, signal_region_r);
            f1_BkgPol2_Fit_map[f1_BkgPol2_Fit_map_key] -> SetParLimits(2, -100, 0);

            // std::cout<<"for "<<f1_BkgPol2_Fit_map_key<<"fit parameters: "<<hist_offset<<", 0, -0.2, 0, "<<signal_region_l<<", "<<signal_region_r<<std::endl;

            temp_hist -> Fit(f1_BkgPol2_Fit_map[f1_BkgPol2_Fit_map_key], "NQ");

            // std::cout<<"for "<<f1_BkgPol2_Fit_map_key<<"fit parameters: "<<f1_BkgPol2_Fit_map[f1_BkgPol2_Fit_map_key] -> GetParameter(0)<<", "<<f1_BkgPol2_Fit_map[f1_BkgPol2_Fit_map_key] -> GetParameter(1)<<", "<<f1_BkgPol2_Fit_map[f1_BkgPol2_Fit_map_key] -> GetParameter(2)<<", "<<f1_BkgPol2_Fit_map[f1_BkgPol2_Fit_map_key] -> GetParameter(3)<<std::endl;

            // std::cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<std::endl;

            f1_BkgPol2_Draw_map[f1_BkgPol2_Draw_map_key] -> SetParameters(
                f1_BkgPol2_Fit_map[f1_BkgPol2_Fit_map_key] -> GetParameter(0),
                f1_BkgPol2_Fit_map[f1_BkgPol2_Fit_map_key] -> GetParameter(1),
                f1_BkgPol2_Fit_map[f1_BkgPol2_Fit_map_key] -> GetParameter(2),
                f1_BkgPol2_Fit_map[f1_BkgPol2_Fit_map_key] -> GetParameter(3)
            );

            // Division:-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


            // note : the normal cases, with the peak
            if (pair.first.find("_rotated") == std::string::npos)
            {
                // note : for the signal+bkg fit
                double gaus_height = temp_hist->GetBinContent(temp_hist->GetMaximumBin()) - hist_offset;
                double gaus_width = fabs(N_group_info[3]-N_group_info[2]) / 2.;

                // note : 
                // note : par[0] : size
                // note : par[1] : mean
                // note : par[2] : width
                // note : double gaus_func = par[0] * TMath::Gaus(x[0],par[1],par[2]);
                // note : double pol2_func = par[3] + par[4]* (x[0]-par[6]) + par[5] * pow((x[0]-par[6]),2);

                // std::cout<<"=================================================================================================================================="<<std::endl;

                f1_SigBkgPol2_Fit_map[f1_SigBkgPol2_Fit_map_key] -> SetParameters(
                    gaus_height, 0, gaus_width, 
                    hist_offset, 0, -0.2, 0
                );
                f1_SigBkgPol2_Fit_map[f1_SigBkgPol2_Fit_map_key] -> SetParLimits(5, -100, 0);

                // std::cout<<"for "<<f1_SigBkgPol2_Fit_map_key<<", all pars: "<<gaus_height<<", 0, "<<gaus_width<<", "<<hist_offset<<", 0, -0.2, 0"<<std::endl;

                temp_hist -> Fit(f1_SigBkgPol2_Fit_map[f1_SigBkgPol2_Fit_map_key], "NQ");

                // std::cout<<"for "<<f1_SigBkgPol2_Fit_map_key<<", all pars: "<<
                // f1_SigBkgPol2_Fit_map[f1_SigBkgPol2_Fit_map_key] -> GetParameter(0)<<", "<<
                // f1_SigBkgPol2_Fit_map[f1_SigBkgPol2_Fit_map_key] -> GetParameter(1)<<", "<<
                // f1_SigBkgPol2_Fit_map[f1_SigBkgPol2_Fit_map_key] -> GetParameter(2)<<", "<<
                // f1_SigBkgPol2_Fit_map[f1_SigBkgPol2_Fit_map_key] -> GetParameter(3)<<", "<<
                // f1_SigBkgPol2_Fit_map[f1_SigBkgPol2_Fit_map_key] -> GetParameter(4)<<", "<<
                // f1_SigBkgPol2_Fit_map[f1_SigBkgPol2_Fit_map_key] -> GetParameter(5)<<", "<<
                // f1_SigBkgPol2_Fit_map[f1_SigBkgPol2_Fit_map_key] -> GetParameter(6)<<std::endl;
                
                // std::cout<<"=================================================================================================================================="<<std::endl;

                f1_SigBkgPol2_DrawSig_map[f1_SigBkgPol2_DrawSig_map_key] -> SetParameters(
                    f1_SigBkgPol2_Fit_map[f1_SigBkgPol2_Fit_map_key] -> GetParameter(0),
                    f1_SigBkgPol2_Fit_map[f1_SigBkgPol2_Fit_map_key] -> GetParameter(1),
                    f1_SigBkgPol2_Fit_map[f1_SigBkgPol2_Fit_map_key] -> GetParameter(2)
                );

                f1_SigBkgPol2_DrawBkgPol2_map[f1_SigBkgPol2_DrawBkgPol2_map_key] -> SetParameters(
                    f1_SigBkgPol2_Fit_map[f1_SigBkgPol2_Fit_map_key] -> GetParameter(3),
                    f1_SigBkgPol2_Fit_map[f1_SigBkgPol2_Fit_map_key] -> GetParameter(4),
                    f1_SigBkgPol2_Fit_map[f1_SigBkgPol2_Fit_map_key] -> GetParameter(5),
                    f1_SigBkgPol2_Fit_map[f1_SigBkgPol2_Fit_map_key] -> GetParameter(6)
                );
            } // note : end of the normal cases, with the peak
            
            // Division:-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

            // note : save the plot into root files
            c1 -> cd();
            hstack1D_map[pair.first] -> Draw(""); // note : inclusive

            if (pair.first.find("_rotated") == std::string::npos) // note : normal
            {
                f1_SigBkgPol2_Fit_map[f1_SigBkgPol2_Fit_map_key] -> Draw("l same");
                f1_SigBkgPol2_DrawSig_map[f1_SigBkgPol2_DrawSig_map_key] -> Draw("l same");
                f1_SigBkgPol2_DrawBkgPol2_map[f1_SigBkgPol2_DrawBkgPol2_map_key] -> Draw("l same");
            }

            f1_BkgPol2_Draw_map[f1_BkgPol2_Draw_map_key] -> Draw("l same");

            file_out -> cd();
            c1 -> Write(Form("c1_%s", pair.first.c_str()));
            c1 -> Clear();

            if (pair.first.find("_rotated") == std::string::npos){ // note : normal
                temp_hist -> SetFillColor(0);
                temp_hist -> SetLineColor(9);
                temp_hist -> SetMaximum(temp_hist -> GetBinContent(temp_hist -> GetMaximumBin()) * 1.6);
                
                auto temp_hist_rotate = (TH1D*) hstack1D_map[pair.first + "_rotated"] -> GetStack() -> Last();
                temp_hist_rotate -> SetFillColor(0);
                temp_hist_rotate -> SetLineColor(46);

                c1 -> cd();
                temp_hist -> Draw();
                temp_hist_rotate -> Draw("same");

                f1_SigBkgPol2_Fit_map[f1_SigBkgPol2_Fit_map_key] -> Draw("l same");
                f1_SigBkgPol2_DrawSig_map[f1_SigBkgPol2_DrawSig_map_key] -> Draw("l same");
                f1_SigBkgPol2_DrawBkgPol2_map[f1_SigBkgPol2_DrawBkgPol2_map_key] -> Draw("l same");

                f1_BkgPol2_Draw_map[f1_BkgPol2_Draw_map_key] -> Draw("l same");

                file_out -> cd();
                c1 -> Write(Form("c1_%s_h1DFit", pair.first.c_str()));
                c1 -> Clear();
            }

        }

        stack_count++;
    }

    std::cout<<"End the DoFittings()"<<std::endl;
}

double PreparedNdEta::get_h2D_GoodProtoTracklet_count(TH2D * hist_in, int eta_bin_in)
{
    double count = 0;
    
    // note : hist
    // note : X: eta
    // note : Y: VtxZ

    for (int yi = 1; yi <= hist_in->GetNbinsY(); yi++){ // note : vtxZ bin
            
        if (vtxZ_index_map.find(yi - 1) == vtxZ_index_map.end()){
            continue;
        }

        count += hist_in -> GetBinContent(eta_bin_in + 1, yi);
    }

    return count;

}

double PreparedNdEta::get_EvtCount(TH2D * hist_in, int centrality_bin_in)
{
    double count = 0;

    // note : X : vtxZ
    // note : Y : centrality

    if (centrality_bin_in == 100){
        for (int xi = 1; xi <= hist_in->GetNbinsX(); xi++){ // note : vtxZ bin

            if (vtxZ_index_map.find(xi - 1) == vtxZ_index_map.end()){
                continue;
            }

            for (int yi = 1; yi <= hist_in->GetNbinsY(); yi++){ // note : centrality bin
                count += hist_in -> GetBinContent(xi, yi);
            }
        }
    }
    else if (centrality_bin_in == 70){
        for (int xi = 1; xi <= hist_in->GetNbinsX(); xi++){ // note : vtxZ bin

            if (vtxZ_index_map.find(xi - 1) == vtxZ_index_map.end()){
                continue;
            }

            for (int yi = 1; yi <= hist_in->GetNbinsY(); yi++){ // note : centrality bin
                if (yi-1 <= Semi_inclusive_Mbin){
                    count += hist_in -> GetBinContent(xi, yi);
                }
            }
        }
    }
    else {
        for (int xi = 1; xi <= hist_in->GetNbinsX(); xi++){ // note : vtxZ bin

            if (vtxZ_index_map.find(xi - 1) == vtxZ_index_map.end()){
                continue;
            }

            count += hist_in -> GetBinContent(xi, centrality_bin_in + 1);
        }
    }

    return count;

}

void PreparedNdEta::Convert_to_PerEvt(TH1D * hist_in, double Nevent)
{
    for (int i = 1; i <= hist_in->GetNbinsX(); i++){
        hist_in -> SetBinContent(i, hist_in -> GetBinContent(i) / Nevent);
        hist_in -> SetBinError(i, hist_in -> GetBinError(i) / Nevent);
    }

}

void PreparedNdEta::PrepareMultiplicity()
{
    // note : data {typeA, inclusive} {fit bkg, rotated bkg} {centrality bin, inclusive 100, inclusive 70}

    // note : "h2D_RecoEvtCount_vtxZCentrality", the event counting 
    
    // note : h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d
    // note : h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d_rotated
    // note :   V hstack2D_GoodProtoTracklet_EtaVtxZ_Inclusive100
    // note :   V hstack2D_GoodProtoTracklet_EtaVtxZ_Inclusive100_rotated
    // note :   V hstack2D_GoodProtoTracklet_EtaVtxZ_Inclusive70
    // note :   V hstack2D_GoodProtoTracklet_EtaVtxZ_Inclusive70_rotated

    // note : h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d
    // note : h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d_rotated
    // note :   V hstack2D_typeA_GoodProtoTracklet_EtaVtxZ_Inclusive100
    // note :   V hstack2D_typeA_GoodProtoTracklet_EtaVtxZ_Inclusive100_rotated
    // note :   V hstack2D_typeA_GoodProtoTracklet_EtaVtxZ_Inclusive70
    // note :   V hstack2D_typeA_GoodProtoTracklet_EtaVtxZ_Inclusive70_rotated

    // note : V h1D_FitBkg_RecoTrackletEta_map
    // note : h1D_FitBkg_RecoTrackletEtaPerEvt_map
    // note : h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_map
    // note : V h1D_RotatedBkg_RecoTrackletEta_map
    // note : h1D_RotatedBkg_RecoTrackletEtaPerEvt_map
    // note : h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map
    // note :   V h1D_Mbin%d
    // note :   V h1D_typeA_Mbin%d
    // note :   V h1D_Inclusive100")
    // note :   V h1D_Inclusive70")
    // note :   V h1D_typeA_Inclusive100")
    // note :   V h1D_typeA_Inclusive70")

    // note : f1_BkgPol2_Draw_map

    

    for (int Mbin = 0; Mbin < nCentrality_bin; Mbin++){
        for (int eta_bin = 0; eta_bin < nEtaBin; eta_bin++){
            
            // note : normal
            double pol2_bkg_integral = fabs(f1_BkgPol2_Draw_map[Form("hstack1D_DeltaPhi_Mbin%d_Eta%d_BkgPol2_Draw", Mbin, eta_bin)] -> Integral( cut_GoodProtoTracklet_DeltaPhi->first, cut_GoodProtoTracklet_DeltaPhi->second )) / ((EtaEdge_max - EtaEdge_min)/double(nEtaBin));
            h1D_FitBkg_RecoTrackletEta_map[Form("h1D_Mbin%d",Mbin)] -> SetBinContent(
                eta_bin + 1,
                get_h2D_GoodProtoTracklet_count(h2D_input_map[Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d",Mbin)], eta_bin) - pol2_bkg_integral
            );
            h1D_RotatedBkg_RecoTrackletEta_map[Form("h1D_Mbin%d",Mbin)] -> SetBinContent(
                eta_bin + 1,
                get_h2D_GoodProtoTracklet_count(h2D_input_map[Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d",Mbin)], eta_bin) - 
                get_h2D_GoodProtoTracklet_count(h2D_input_map[Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d_rotated",Mbin)], eta_bin)
            );

            // note : type A
            pol2_bkg_integral = fabs(f1_BkgPol2_Draw_map[Form("hstack1D_typeA_DeltaPhi_Mbin%d_Eta%d_BkgPol2_Draw", Mbin, eta_bin)] -> Integral( cut_GoodProtoTracklet_DeltaPhi->first, cut_GoodProtoTracklet_DeltaPhi->second )) / ((EtaEdge_max - EtaEdge_min)/double(nEtaBin));
            h1D_FitBkg_RecoTrackletEta_map[Form("h1D_typeA_Mbin%d",Mbin)] -> SetBinContent(
                eta_bin + 1,
                get_h2D_GoodProtoTracklet_count(h2D_input_map[Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d",Mbin)], eta_bin) - pol2_bkg_integral
            );
            h1D_RotatedBkg_RecoTrackletEta_map[Form("h1D_typeA_Mbin%d",Mbin)] -> SetBinContent(
                eta_bin + 1,
                get_h2D_GoodProtoTracklet_count(h2D_input_map[Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d",Mbin)], eta_bin) - 
                get_h2D_GoodProtoTracklet_count(h2D_input_map[Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d_rotated",Mbin)], eta_bin)
            );
        }   
    }

    // note : inclusive 100, inclusive 70
    for (int eta_bin = 0; eta_bin < nEtaBin; eta_bin++){
        
        // note : normal
        double pol2_bkg_integral = fabs(f1_BkgPol2_Draw_map[Form("hstack1D_DeltaPhi_Eta%d_Inclusive100_BkgPol2_Draw", eta_bin)] -> Integral( cut_GoodProtoTracklet_DeltaPhi->first, cut_GoodProtoTracklet_DeltaPhi->second )) / ((EtaEdge_max - EtaEdge_min)/double(nEtaBin));
        h1D_FitBkg_RecoTrackletEta_map[Form("h1D_Inclusive100")] -> SetBinContent(
            eta_bin + 1,
            get_h2D_GoodProtoTracklet_count(((TH2D*)hstack2D_map[Form("hstack2D_GoodProtoTracklet_EtaVtxZ_Inclusive100")]->GetStack()->Last()), eta_bin) - pol2_bkg_integral
        );
        h1D_RotatedBkg_RecoTrackletEta_map[Form("h1D_Inclusive100")] -> SetBinContent(
            eta_bin + 1,
            get_h2D_GoodProtoTracklet_count(((TH2D*)hstack2D_map[Form("hstack2D_GoodProtoTracklet_EtaVtxZ_Inclusive100")]->GetStack()->Last()), eta_bin) - 
            get_h2D_GoodProtoTracklet_count(((TH2D*)hstack2D_map[Form("hstack2D_GoodProtoTracklet_EtaVtxZ_Inclusive100_rotated")]->GetStack()->Last()), eta_bin)
        );

        pol2_bkg_integral = fabs(f1_BkgPol2_Draw_map[Form("hstack1D_DeltaPhi_Eta%d_Inclusive70_BkgPol2_Draw", eta_bin)] -> Integral( cut_GoodProtoTracklet_DeltaPhi->first, cut_GoodProtoTracklet_DeltaPhi->second )) / ((EtaEdge_max - EtaEdge_min)/double(nEtaBin));
        h1D_FitBkg_RecoTrackletEta_map[Form("h1D_Inclusive70")] -> SetBinContent(
            eta_bin + 1,
            get_h2D_GoodProtoTracklet_count(((TH2D*)hstack2D_map[Form("hstack2D_GoodProtoTracklet_EtaVtxZ_Inclusive70")]->GetStack()->Last()), eta_bin) - pol2_bkg_integral
        );
        h1D_RotatedBkg_RecoTrackletEta_map[Form("h1D_Inclusive70")] -> SetBinContent(
            eta_bin + 1,
            get_h2D_GoodProtoTracklet_count(((TH2D*)hstack2D_map[Form("hstack2D_GoodProtoTracklet_EtaVtxZ_Inclusive70")]->GetStack()->Last()), eta_bin) - 
            get_h2D_GoodProtoTracklet_count(((TH2D*)hstack2D_map[Form("hstack2D_GoodProtoTracklet_EtaVtxZ_Inclusive70_rotated")]->GetStack()->Last()), eta_bin)
        );



        // note : type A
        pol2_bkg_integral = fabs(f1_BkgPol2_Draw_map[Form("hstack1D_typeA_DeltaPhi_Eta%d_Inclusive100_BkgPol2_Draw", eta_bin)] -> Integral( cut_GoodProtoTracklet_DeltaPhi->first, cut_GoodProtoTracklet_DeltaPhi->second )) / ((EtaEdge_max - EtaEdge_min)/double(nEtaBin));
        h1D_FitBkg_RecoTrackletEta_map[Form("h1D_typeA_Inclusive100")] -> SetBinContent(
            eta_bin + 1,
            get_h2D_GoodProtoTracklet_count(((TH2D*)hstack2D_map[Form("hstack2D_typeA_GoodProtoTracklet_EtaVtxZ_Inclusive100")]->GetStack()->Last()), eta_bin) - pol2_bkg_integral
        );
        h1D_RotatedBkg_RecoTrackletEta_map[Form("h1D_typeA_Inclusive100")] -> SetBinContent(
            eta_bin + 1,
            get_h2D_GoodProtoTracklet_count(((TH2D*)hstack2D_map[Form("hstack2D_typeA_GoodProtoTracklet_EtaVtxZ_Inclusive100")]->GetStack()->Last()), eta_bin) - 
            get_h2D_GoodProtoTracklet_count(((TH2D*)hstack2D_map[Form("hstack2D_typeA_GoodProtoTracklet_EtaVtxZ_Inclusive100_rotated")]->GetStack()->Last()), eta_bin)
        );

        pol2_bkg_integral = fabs(f1_BkgPol2_Draw_map[Form("hstack1D_typeA_DeltaPhi_Eta%d_Inclusive70_BkgPol2_Draw", eta_bin)] -> Integral( cut_GoodProtoTracklet_DeltaPhi->first, cut_GoodProtoTracklet_DeltaPhi->second )) / ((EtaEdge_max - EtaEdge_min)/double(nEtaBin));
        h1D_FitBkg_RecoTrackletEta_map[Form("h1D_typeA_Inclusive70")] -> SetBinContent(
            eta_bin + 1,
            get_h2D_GoodProtoTracklet_count(((TH2D*)hstack2D_map[Form("hstack2D_typeA_GoodProtoTracklet_EtaVtxZ_Inclusive70")]->GetStack()->Last()), eta_bin) - pol2_bkg_integral
        );
        h1D_RotatedBkg_RecoTrackletEta_map[Form("h1D_typeA_Inclusive70")] -> SetBinContent(
            eta_bin + 1,
            get_h2D_GoodProtoTracklet_count(((TH2D*)hstack2D_map[Form("hstack2D_typeA_GoodProtoTracklet_EtaVtxZ_Inclusive70")]->GetStack()->Last()), eta_bin) - 
            get_h2D_GoodProtoTracklet_count(((TH2D*)hstack2D_map[Form("hstack2D_typeA_GoodProtoTracklet_EtaVtxZ_Inclusive70_rotated")]->GetStack()->Last()), eta_bin)
        );

    }

    

    // note : MC {centrality bin, inclusive 100, inclusive 70}
    // note : "h2D_TrueEvtCount_vtxZCentrality", the event counting

    // note : hstack1D_TrueEta_Mbin%d", Mbin)
    // note : V hstack1D_TrueEta_Inclusive100")
    // note : V hstack1D_TrueEta_Inclusive70")
    // note : V hstack2D_TrueEtaVtxZ_Inclusive100
    // note : V hstack2D_TrueEtaVtxZ_Inclusive100_FineBin
    // note : V hstack2D_TrueEtaVtxZ_Inclusive70
    // note : V hstack2D_TrueEtaVtxZ_Inclusive70_FineBin

    // note : h1D_TruedNdEta_map

}

void PreparedNdEta::PreparedNdEtaHist()
{
    std::cout<<1111111<<std::endl;
    if (runnumber == -1){
        h1D_TruedNdEta_map.clear();

        h1D_TruedNdEta_map.insert( 
            std::make_pair(
                "h1D_TruedNdEta_Inclusive100",
                (TH1D*)hstack1D_map["hstack1D_TrueEta_Inclusive100"] -> GetStack() -> Last()
            )
        );
        
        std::cout<<22222222<<std::endl;

        Convert_to_PerEvt(h1D_TruedNdEta_map["h1D_TruedNdEta_Inclusive100"], get_EvtCount(h2D_input_map["h2D_TrueEvtCount_vtxZCentrality"], 100));
        
        std::cout<<33333333<<std::endl;

        h1D_TruedNdEta_map.insert( 
            std::make_pair(
                "h1D_TruedNdEta_Inclusive70",
                (TH1D*)hstack1D_map["hstack1D_TrueEta_Inclusive70"] -> GetStack() -> Last()
            )
        );
        Convert_to_PerEvt(h1D_TruedNdEta_map["h1D_TruedNdEta_Inclusive70"], get_EvtCount(h2D_input_map["h2D_TrueEvtCount_vtxZCentrality"], 70));

        std::cout<<44444<<std::endl;

        for (int i = 0; i < nCentrality_bin; i++){
            h1D_TruedNdEta_map.insert( 
                std::make_pair(
                    Form("h1D_TruedNdEta_Mbin%d", i),
                    (TH1D*)hstack1D_map[Form("hstack1D_TrueEta_Mbin%d", i)] -> GetStack() -> Last()
                )
            );
            Convert_to_PerEvt(h1D_TruedNdEta_map[Form("h1D_TruedNdEta_Mbin%d", i)], get_EvtCount(h2D_input_map["h2D_TrueEvtCount_vtxZCentrality"], i));
        }
    } // note : end of truth

    std::cout<<5555<<std::endl;

    // Division:-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // note : FitBkg, {TypeA, normal} x {Mbin, Inclusive100, Inclusive70}
    h1D_FitBkg_RecoTrackletEtaPerEvt_map.clear();
    h1D_FitBkg_RecoTrackletEtaPerEvt_map.insert( 
        std::make_pair(            
            "h1D_Inclusive100",
            (TH1D*)h1D_FitBkg_RecoTrackletEta_map["h1D_Inclusive100"] -> Clone("h1D_FitBkg_RecoTrackletEtaPerEvt_Inclusive100")
        )
    );

    std::cout<<"h1D_FitBkg_RecoTrackletEtaPerEvt_map[\"h1D_Inclusive100\"]->GetEntries(): "<<h1D_FitBkg_RecoTrackletEtaPerEvt_map["h1D_Inclusive100"]->GetEntries()<<std::endl;

    Convert_to_PerEvt(h1D_FitBkg_RecoTrackletEtaPerEvt_map["h1D_Inclusive100"], get_EvtCount(h2D_input_map["h2D_RecoEvtCount_vtxZCentrality"], 100));
    std::cout<<"h1D_FitBkg_RecoTrackletEtaPerEvt_map[\"h1D_Inclusive100\"]->GetEntries(): "<<h1D_FitBkg_RecoTrackletEtaPerEvt_map["h1D_Inclusive100"]->GetEntries()<<std::endl;

    std::cout<<6666<<std::endl;

    h1D_FitBkg_RecoTrackletEtaPerEvt_map.insert( 
        std::make_pair(            
            "h1D_Inclusive70",
            (TH1D*)h1D_FitBkg_RecoTrackletEta_map["h1D_Inclusive70"] -> Clone("h1D_FitBkg_RecoTrackletEtaPerEvt_Inclusive70")
        )
    );
    Convert_to_PerEvt(h1D_FitBkg_RecoTrackletEtaPerEvt_map["h1D_Inclusive70"], get_EvtCount(h2D_input_map["h2D_RecoEvtCount_vtxZCentrality"], 70));

    h1D_FitBkg_RecoTrackletEtaPerEvt_map.insert( 
        std::make_pair(            
            "h1D_typeA_Inclusive100",
            (TH1D*)h1D_FitBkg_RecoTrackletEta_map["h1D_typeA_Inclusive100"] -> Clone("h1D_typeA_FitBkg_RecoTrackletEtaPerEvt_Inclusive100")
        )
    );
    Convert_to_PerEvt(h1D_FitBkg_RecoTrackletEtaPerEvt_map["h1D_typeA_Inclusive100"], get_EvtCount(h2D_input_map["h2D_RecoEvtCount_vtxZCentrality"], 100));

    h1D_FitBkg_RecoTrackletEtaPerEvt_map.insert( 
        std::make_pair(            
            "h1D_typeA_Inclusive70",
            (TH1D*)h1D_FitBkg_RecoTrackletEta_map["h1D_typeA_Inclusive70"] -> Clone("h1D_typeA_FitBkg_RecoTrackletEtaPerEvt_Inclusive70")
        )
    );
    Convert_to_PerEvt(h1D_FitBkg_RecoTrackletEtaPerEvt_map["h1D_typeA_Inclusive70"], get_EvtCount(h2D_input_map["h2D_RecoEvtCount_vtxZCentrality"], 70));

    for (int i = 0; i < nCentrality_bin; i++){
        h1D_FitBkg_RecoTrackletEtaPerEvt_map.insert( 
            std::make_pair(                
                Form("h1D_Mbin%d", i),
                (TH1D*)h1D_FitBkg_RecoTrackletEta_map[Form("h1D_Mbin%d", i)] -> Clone(Form("h1D_FitBkg_RecoTrackletEtaPerEvt_Mbin%d", i))
            )
        );
        Convert_to_PerEvt(h1D_FitBkg_RecoTrackletEtaPerEvt_map[Form("h1D_Mbin%d", i)], get_EvtCount(h2D_input_map["h2D_RecoEvtCount_vtxZCentrality"], i));

        h1D_FitBkg_RecoTrackletEtaPerEvt_map.insert( 
            std::make_pair(                
                Form("h1D_typeA_Mbin%d", i),
                (TH1D*)h1D_FitBkg_RecoTrackletEta_map[Form("h1D_typeA_Mbin%d", i)] -> Clone(Form("h1D_typeA_FitBkg_RecoTrackletEtaPerEvt_Mbin%d", i))
            )
        );
        Convert_to_PerEvt(h1D_FitBkg_RecoTrackletEtaPerEvt_map[Form("h1D_typeA_Mbin%d", i)], get_EvtCount(h2D_input_map["h2D_RecoEvtCount_vtxZCentrality"], i));
    }

    std::cout<<7777<<std::endl;

    // note : copy to the next map
    // note : FitBkg, {TypeA, normal} x {Mbin, Inclusive100, Inclusive70}
    // Division:-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_map.insert( 
        std::make_pair(            
            "h1D_Inclusive100",
            (TH1D*)h1D_FitBkg_RecoTrackletEtaPerEvt_map["h1D_Inclusive100"] -> Clone("h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_Inclusive100")
        )
    );

    h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_map.insert( 
        std::make_pair(            
            "h1D_Inclusive70",
            (TH1D*)h1D_FitBkg_RecoTrackletEtaPerEvt_map["h1D_Inclusive70"] -> Clone("h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_Inclusive70")
        )
    );

    h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_map.insert( 
        std::make_pair(            
            "h1D_typeA_Inclusive100",
            (TH1D*)h1D_FitBkg_RecoTrackletEtaPerEvt_map["h1D_typeA_Inclusive100"] -> Clone("h1D_typeA_FitBkg_RecoTrackletEtaPerEvtPostAC_Inclusive100")
        )
    );

    h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_map.insert( 
        std::make_pair(            
            "h1D_typeA_Inclusive70",
            (TH1D*)h1D_FitBkg_RecoTrackletEtaPerEvt_map["h1D_typeA_Inclusive70"] -> Clone("h1D_typeA_FitBkg_RecoTrackletEtaPerEvtPostAC_Inclusive70")
        )
    );

    std::cout<<88888<<std::endl;

    for (int i = 0; i < nCentrality_bin; i++){
        h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_map.insert( 
            std::make_pair(                
                Form("h1D_Mbin%d", i),
                (TH1D*)h1D_FitBkg_RecoTrackletEtaPerEvt_map[Form("h1D_Mbin%d", i)] -> Clone(Form("h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_Mbin%d", i))
            )
        );

        h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_map.insert( 
            std::make_pair(                
                Form("h1D_typeA_Mbin%d", i),
                (TH1D*)h1D_FitBkg_RecoTrackletEtaPerEvt_map[Form("h1D_typeA_Mbin%d", i)] -> Clone(Form("h1D_typeA_FitBkg_RecoTrackletEtaPerEvtPostAC_Mbin%d", i))
            )
        );
    }

    std::cout<<9999<<std::endl;

    // note : RotatedBkg, {TypeA, normal} x {Mbin, Inclusive100, Inclusive70}
    // Division:-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    h1D_RotatedBkg_RecoTrackletEtaPerEvt_map.clear();
    h1D_RotatedBkg_RecoTrackletEtaPerEvt_map.insert( 
        std::make_pair(            
            "h1D_Inclusive100",
            (TH1D*)h1D_RotatedBkg_RecoTrackletEta_map["h1D_Inclusive100"] -> Clone("h1D_RotatedBkg_RecoTrackletEtaPerEvt_Inclusive100")
        )
    );
    Convert_to_PerEvt(h1D_RotatedBkg_RecoTrackletEtaPerEvt_map["h1D_Inclusive100"], get_EvtCount(h2D_input_map["h2D_RecoEvtCount_vtxZCentrality"], 100));

    std::cout<<10101010<<std::endl;

    h1D_RotatedBkg_RecoTrackletEtaPerEvt_map.insert( 
        std::make_pair(            
            "h1D_Inclusive70",
            (TH1D*)h1D_RotatedBkg_RecoTrackletEta_map["h1D_Inclusive70"] -> Clone("h1D_RotatedBkg_RecoTrackletEtaPerEvt_Inclusive70")
        )
    );
    Convert_to_PerEvt(h1D_RotatedBkg_RecoTrackletEtaPerEvt_map["h1D_Inclusive70"], get_EvtCount(h2D_input_map["h2D_RecoEvtCount_vtxZCentrality"], 70));

    h1D_RotatedBkg_RecoTrackletEtaPerEvt_map.insert( 
        std::make_pair(            
            "h1D_typeA_Inclusive100",
            (TH1D*)h1D_RotatedBkg_RecoTrackletEta_map["h1D_typeA_Inclusive100"] -> Clone("h1D_typeA_RotatedBkg_RecoTrackletEtaPerEvt_Inclusive100")
        )
    );
    Convert_to_PerEvt(h1D_RotatedBkg_RecoTrackletEtaPerEvt_map["h1D_typeA_Inclusive100"], get_EvtCount(h2D_input_map["h2D_RecoEvtCount_vtxZCentrality"], 100));

    h1D_RotatedBkg_RecoTrackletEtaPerEvt_map.insert( 
        std::make_pair(            
            "h1D_typeA_Inclusive70",
            (TH1D*)h1D_RotatedBkg_RecoTrackletEta_map["h1D_typeA_Inclusive70"] -> Clone("h1D_typeA_RotatedBkg_RecoTrackletEtaPerEvt_Inclusive70")
        )
    );
    Convert_to_PerEvt(h1D_RotatedBkg_RecoTrackletEtaPerEvt_map["h1D_typeA_Inclusive70"], get_EvtCount(h2D_input_map["h2D_RecoEvtCount_vtxZCentrality"], 70));

    for (int i = 0; i < nCentrality_bin; i++){
        h1D_RotatedBkg_RecoTrackletEtaPerEvt_map.insert( 
            std::make_pair(                
                Form("h1D_Mbin%d", i),
                (TH1D*)h1D_RotatedBkg_RecoTrackletEta_map[Form("h1D_Mbin%d", i)] -> Clone(Form("h1D_RotatedBkg_RecoTrackletEtaPerEvt_Mbin%d", i))
            )
        );
        Convert_to_PerEvt(h1D_RotatedBkg_RecoTrackletEtaPerEvt_map[Form("h1D_Mbin%d", i)], get_EvtCount(h2D_input_map["h2D_RecoEvtCount_vtxZCentrality"], i));

        h1D_RotatedBkg_RecoTrackletEtaPerEvt_map.insert( 
            std::make_pair(                
                Form("h1D_typeA_Mbin%d", i),
                (TH1D*)h1D_RotatedBkg_RecoTrackletEta_map[Form("h1D_typeA_Mbin%d", i)] -> Clone(Form("h1D_typeA_RotatedBkg_RecoTrackletEtaPerEvt_Mbin%d", i))
            )
        );
        Convert_to_PerEvt(h1D_RotatedBkg_RecoTrackletEtaPerEvt_map[Form("h1D_typeA_Mbin%d", i)], get_EvtCount(h2D_input_map["h2D_RecoEvtCount_vtxZCentrality"], i));
    }

    std::cout<<10101111<<std::endl;

    // note : copy to the next map
    // note : RotatedBkg, {TypeA, normal} x {Mbin, Inclusive100, Inclusive70}
    // Division:-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map.insert( 
        std::make_pair(            
            "h1D_Inclusive100",
            (TH1D*)h1D_RotatedBkg_RecoTrackletEtaPerEvt_map["h1D_Inclusive100"] -> Clone("h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_Inclusive100")
        )
    );


    h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map.insert( 
        std::make_pair(            
            "h1D_Inclusive70",
            (TH1D*)h1D_RotatedBkg_RecoTrackletEtaPerEvt_map["h1D_Inclusive70"] -> Clone("h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_Inclusive70")
        )
    );

    h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map.insert( 
        std::make_pair(            
            "h1D_typeA_Inclusive100",
            (TH1D*)h1D_RotatedBkg_RecoTrackletEtaPerEvt_map["h1D_typeA_Inclusive100"] -> Clone("h1D_typeA_RotatedBkg_RecoTrackletEtaPerEvtPostAC_Inclusive100")
        )
    );

    h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map.insert( 
        std::make_pair(            
            "h1D_typeA_Inclusive70",
            (TH1D*)h1D_RotatedBkg_RecoTrackletEtaPerEvt_map["h1D_typeA_Inclusive70"] -> Clone("h1D_typeA_RotatedBkg_RecoTrackletEtaPerEvtPostAC_Inclusive70")
        )
    );

    for (int i = 0; i < nCentrality_bin; i++){
        h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map.insert( 
            std::make_pair(                
                Form("h1D_Mbin%d", i),
                (TH1D*)h1D_RotatedBkg_RecoTrackletEtaPerEvt_map[Form("h1D_Mbin%d", i)] -> Clone(Form("h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_Mbin%d", i))
            )
        );

        h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map.insert( 
            std::make_pair(                
                Form("h1D_typeA_Mbin%d", i),
                (TH1D*)h1D_RotatedBkg_RecoTrackletEtaPerEvt_map[Form("h1D_typeA_Mbin%d", i)] -> Clone(Form("h1D_typeA_RotatedBkg_RecoTrackletEtaPerEvtPostAC_Mbin%d", i))
            )
        );
    }

    std::cout<<101022222<<std::endl;

    std::cout<<"a h1D_FitBkg_RecoTrackletEtaPerEvt_map[\"h1D_Inclusive100\"]->GetEntries(): "<<h1D_FitBkg_RecoTrackletEtaPerEvt_map["h1D_Inclusive100"]->GetEntries()<<std::endl;
    // note : after the copy, make sure that the title is also changed
    for (auto pair : h1D_FitBkg_RecoTrackletEta_map){
        h1D_FitBkg_RecoTrackletEtaPerEvt_map[pair.first] -> SetTitle(h1D_FitBkg_RecoTrackletEtaPerEvt_map[pair.first]->GetName());
        h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_map[pair.first] -> SetTitle(h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_map[pair.first]->GetName());

        h1D_RotatedBkg_RecoTrackletEtaPerEvt_map[pair.first] -> SetTitle(h1D_RotatedBkg_RecoTrackletEtaPerEvt_map[pair.first]->GetName());
        h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map[pair.first] -> SetTitle(h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map[pair.first]->GetName());
    }
    std::cout<<"b h1D_FitBkg_RecoTrackletEtaPerEvt_map[\"h1D_Inclusive100\"]->GetEntries(): "<<h1D_FitBkg_RecoTrackletEtaPerEvt_map["h1D_Inclusive100"]->GetEntries()<<std::endl;
    

    // note : apply the alpha correction from the "h2D_alpha_correction_map_in"
    // Division:-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    if (h2D_alpha_correction_map_in.size() != 0 && ApplyAlphaCorr == true){
        // h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_map
        // h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map

        // h2D_FitBkg_alpha_correction
        // h2D_RotatedBkg_alpha_correction
        // h2D_typeA_FitBkg_alpha_correction
        // h2D_typeA_RotatedBkg_alpha_correction


        // note : for [0-100%]
        // note : X: eta, Y: Mbin
        // todo: need to fix the zero division issue
        for (int Mbin = 0; Mbin < nCentrality_bin; Mbin++){
            for (int eta_bin = 0; eta_bin < nEtaBin; eta_bin++)
            {   
                // note : FitBkg
                h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_Mbin%d",Mbin)] -> SetBinContent(eta_bin + 1, h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_Mbin%d",Mbin)] -> GetBinContent(eta_bin + 1) / h2D_alpha_correction_map_in["h2D_FitBkg_alpha_correction"] -> GetBinContent(eta_bin + 1, Mbin + 1));
                h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_Mbin%d",Mbin)] -> SetBinError(eta_bin + 1, h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_Mbin%d",Mbin)] -> GetBinError(eta_bin + 1)     / h2D_alpha_correction_map_in["h2D_FitBkg_alpha_correction"] -> GetBinContent(eta_bin + 1, Mbin + 1));
                h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_typeA_Mbin%d",Mbin)] -> SetBinContent(eta_bin + 1, h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_typeA_Mbin%d",Mbin)] -> GetBinContent(eta_bin + 1) / h2D_alpha_correction_map_in["h2D_typeA_FitBkg_alpha_correction"] -> GetBinContent(eta_bin + 1, Mbin + 1));
                h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_typeA_Mbin%d",Mbin)] -> SetBinError(eta_bin + 1, h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_typeA_Mbin%d",Mbin)] -> GetBinError(eta_bin + 1)     / h2D_alpha_correction_map_in["h2D_typeA_FitBkg_alpha_correction"] -> GetBinContent(eta_bin + 1, Mbin + 1));

                // note : RotatedBkg
                h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_Mbin%d",Mbin)] -> SetBinContent(eta_bin + 1, h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_Mbin%d",Mbin)] -> GetBinContent(eta_bin + 1) / h2D_alpha_correction_map_in["h2D_RotatedBkg_alpha_correction"] -> GetBinContent(eta_bin + 1, Mbin + 1));
                h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_Mbin%d",Mbin)] -> SetBinError(eta_bin + 1, h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_Mbin%d",Mbin)] -> GetBinError(eta_bin + 1)     / h2D_alpha_correction_map_in["h2D_RotatedBkg_alpha_correction"] -> GetBinContent(eta_bin + 1, Mbin + 1));

                h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_typeA_Mbin%d",Mbin)] -> SetBinContent(eta_bin + 1, h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_typeA_Mbin%d",Mbin)] -> GetBinContent(eta_bin + 1) / h2D_alpha_correction_map_in["h2D_typeA_RotatedBkg_alpha_correction"] -> GetBinContent(eta_bin + 1, Mbin + 1));
                h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_typeA_Mbin%d",Mbin)] -> SetBinError(eta_bin + 1, h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_typeA_Mbin%d",Mbin)] -> GetBinError(eta_bin + 1)     / h2D_alpha_correction_map_in["h2D_typeA_RotatedBkg_alpha_correction"] -> GetBinContent(eta_bin + 1, Mbin + 1));
            }
        }

        // note : for Inclusive 70
        // todo : the index in the Y axis (for Mbin) is given by (nCentrality_bin + 1)
        for (int eta_bin = 0; eta_bin < nEtaBin; eta_bin++)
        {   
            // note : FitBkg
            h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_Inclusive70")] -> SetBinContent(eta_bin + 1, h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_Inclusive70")] -> GetBinContent(eta_bin + 1) / h2D_alpha_correction_map_in["h2D_FitBkg_alpha_correction"] -> GetBinContent(eta_bin + 1, (nCentrality_bin + 1)));
            h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_Inclusive70")] -> SetBinError(eta_bin + 1, h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_Inclusive70")] -> GetBinError(eta_bin + 1)     / h2D_alpha_correction_map_in["h2D_FitBkg_alpha_correction"] -> GetBinContent(eta_bin + 1, (nCentrality_bin + 1)));
            h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_typeA_Inclusive70")] -> SetBinContent(eta_bin + 1, h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_typeA_Inclusive70")] -> GetBinContent(eta_bin + 1) / h2D_alpha_correction_map_in["h2D_typeA_FitBkg_alpha_correction"] -> GetBinContent(eta_bin + 1, (nCentrality_bin + 1)));
            h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_typeA_Inclusive70")] -> SetBinError(eta_bin + 1, h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_typeA_Inclusive70")] -> GetBinError(eta_bin + 1)     / h2D_alpha_correction_map_in["h2D_typeA_FitBkg_alpha_correction"] -> GetBinContent(eta_bin + 1, (nCentrality_bin + 1)));

            // note : RotatedBkg
            h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_Inclusive70")] -> SetBinContent(eta_bin + 1, h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_Inclusive70")] -> GetBinContent(eta_bin + 1) / h2D_alpha_correction_map_in["h2D_RotatedBkg_alpha_correction"] -> GetBinContent(eta_bin + 1, (nCentrality_bin + 1)));
            h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_Inclusive70")] -> SetBinError(eta_bin + 1, h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_Inclusive70")] -> GetBinError(eta_bin + 1)     / h2D_alpha_correction_map_in["h2D_RotatedBkg_alpha_correction"] -> GetBinContent(eta_bin + 1, (nCentrality_bin + 1)));

            h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_typeA_Inclusive70")] -> SetBinContent(eta_bin + 1, h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_typeA_Inclusive70")] -> GetBinContent(eta_bin + 1) / h2D_alpha_correction_map_in["h2D_typeA_RotatedBkg_alpha_correction"] -> GetBinContent(eta_bin + 1, (nCentrality_bin + 1)));
            h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_typeA_Inclusive70")] -> SetBinError(eta_bin + 1, h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_typeA_Inclusive70")] -> GetBinError(eta_bin + 1)     / h2D_alpha_correction_map_in["h2D_typeA_RotatedBkg_alpha_correction"] -> GetBinContent(eta_bin + 1, (nCentrality_bin + 1)));
        }

        // note : for Inclusive 100
        // todo : the index in the Y axis (for Mbin) is given by (nCentrality_bin + 2)
        for (int eta_bin = 0; eta_bin < nEtaBin; eta_bin++)
        {   
            // note : FitBkg
            h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_Inclusive100")] -> SetBinContent(eta_bin + 1, h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_Inclusive100")] -> GetBinContent(eta_bin + 1) / h2D_alpha_correction_map_in["h2D_FitBkg_alpha_correction"] -> GetBinContent(eta_bin + 1, (nCentrality_bin + 2)));
            h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_Inclusive100")] -> SetBinError(eta_bin + 1, h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_Inclusive100")] -> GetBinError(eta_bin + 1)     / h2D_alpha_correction_map_in["h2D_FitBkg_alpha_correction"] -> GetBinContent(eta_bin + 1, (nCentrality_bin + 2)));
            h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_typeA_Inclusive100")] -> SetBinContent(eta_bin + 1, h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_typeA_Inclusive100")] -> GetBinContent(eta_bin + 1) / h2D_alpha_correction_map_in["h2D_typeA_FitBkg_alpha_correction"] -> GetBinContent(eta_bin + 1, (nCentrality_bin + 2)));
            h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_typeA_Inclusive100")] -> SetBinError(eta_bin + 1, h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_typeA_Inclusive100")] -> GetBinError(eta_bin + 1)     / h2D_alpha_correction_map_in["h2D_typeA_FitBkg_alpha_correction"] -> GetBinContent(eta_bin + 1, (nCentrality_bin + 2)));

            // note : RotatedBkg
            h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_Inclusive100")] -> SetBinContent(eta_bin + 1, h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_Inclusive100")] -> GetBinContent(eta_bin + 1) / h2D_alpha_correction_map_in["h2D_RotatedBkg_alpha_correction"] -> GetBinContent(eta_bin + 1, (nCentrality_bin + 2)));
            h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_Inclusive100")] -> SetBinError(eta_bin + 1, h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_Inclusive100")] -> GetBinError(eta_bin + 1)     / h2D_alpha_correction_map_in["h2D_RotatedBkg_alpha_correction"] -> GetBinContent(eta_bin + 1, (nCentrality_bin + 2)));

            h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_typeA_Inclusive100")] -> SetBinContent(eta_bin + 1, h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_typeA_Inclusive100")] -> GetBinContent(eta_bin + 1) / h2D_alpha_correction_map_in["h2D_typeA_RotatedBkg_alpha_correction"] -> GetBinContent(eta_bin + 1, (nCentrality_bin + 2)));
            h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_typeA_Inclusive100")] -> SetBinError(eta_bin + 1, h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_typeA_Inclusive100")] -> GetBinError(eta_bin + 1)     / h2D_alpha_correction_map_in["h2D_typeA_RotatedBkg_alpha_correction"] -> GetBinContent(eta_bin + 1, (nCentrality_bin + 2)));
        }

    }

    std::cout<<10103333<<std::endl;

}

void PreparedNdEta::DeriveAlphaCorrection()
{
    h2D_alpha_correction_map_out.insert(
        std::make_pair(
            "h2D_FitBkg_alpha_correction",
            new TH2D("h2D_FitBkg_alpha_correction", "h2D_FitBkg_alpha_correction", nEtaBin, 0, nEtaBin, nCentrality_bin+2, 0, nCentrality_bin+2) // todo: additional 2 bins for inclusive 70, 100
        )
    );   

    h2D_alpha_correction_map_out.insert(
        std::make_pair(
            "h2D_RotatedBkg_alpha_correction",
            new TH2D("h2D_RotatedBkg_alpha_correction", "h2D_RotatedBkg_alpha_correction", nEtaBin, 0, nEtaBin, nCentrality_bin+2, 0, nCentrality_bin+2) // todo: additional 2 bins for inclusive 70, 100
        )
    );

    h2D_alpha_correction_map_out.insert(
        std::make_pair(
            "h2D_typeA_FitBkg_alpha_correction",
            new TH2D("h2D_typeA_FitBkg_alpha_correction", "h2D_typeA_FitBkg_alpha_correction", nEtaBin, 0, nEtaBin, nCentrality_bin+2, 0, nCentrality_bin+2) // todo: additional 2 bins for inclusive 70, 100
        )
    );   

    h2D_alpha_correction_map_out.insert(
        std::make_pair(
            "h2D_typeA_RotatedBkg_alpha_correction",
            new TH2D("h2D_typeA_RotatedBkg_alpha_correction", "h2D_typeA_RotatedBkg_alpha_correction", nEtaBin, 0, nEtaBin, nCentrality_bin+2, 0, nCentrality_bin+2) // todo: additional 2 bins for inclusive 70, 100
        )
    );   


    // note : derive the alpha correction regardless whether you have the input alpha correction map or not
    // Division:-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_map
    // h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map

    // h2D_FitBkg_alpha_correction
    // h2D_RotatedBkg_alpha_correction
    // h2D_typeA_FitBkg_alpha_correction
    // h2D_typeA_RotatedBkg_alpha_correction

    // note : for [0-100%]
    // note : X: eta, Y: Mbin
    for (int Mbin = 0; Mbin < nCentrality_bin; Mbin++){
        for (int eta_bin = 0; eta_bin < nEtaBin; eta_bin++)
        {   
            // note : FitBkg
            // todo : how to properly handle the errors
            h2D_alpha_correction_map_out["h2D_FitBkg_alpha_correction"] -> SetBinContent(eta_bin + 1, Mbin + 1, h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_Mbin%d",Mbin)] -> GetBinContent(eta_bin + 1) / h1D_TruedNdEta_map[Form("h1D_TruedNdEta_Mbin%d", Mbin)] -> GetBinContent(eta_bin + 1));
            // h2D_alpha_correction_map_out["h2D_FitBkg_alpha_correction"] -> SetBinError(eta_bin + 1, Mbin + 1, h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_Mbin%d",Mbin)] -> GetBinError(eta_bin + 1)     / h1D_TruedNdEta_map[Form("h1D_TruedNdEta_Mbin%d", Mbin)] -> GetBinContent(eta_bin + 1));
            
            h2D_alpha_correction_map_out["h2D_typeA_FitBkg_alpha_correction"] -> SetBinContent(eta_bin + 1, Mbin + 1, h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_typeA_Mbin%d",Mbin)] -> GetBinContent(eta_bin + 1) / h1D_TruedNdEta_map[Form("h1D_TruedNdEta_Mbin%d", Mbin)] -> GetBinContent(eta_bin + 1));
            // h2D_alpha_correction_map_out["h2D_typeA_FitBkg_alpha_correction"] -> SetBinError(eta_bin + 1, Mbin + 1, h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_typeA_Mbin%d",Mbin)] -> GetBinError(eta_bin + 1)     / h1D_TruedNdEta_map[Form("h1D_TruedNdEta_Mbin%d", Mbin)] -> GetBinContent(eta_bin + 1));

            // note : RotatedBkg
            h2D_alpha_correction_map_out["h2D_RotatedBkg_alpha_correction"] -> SetBinContent(eta_bin + 1, Mbin + 1, h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_Mbin%d",Mbin)] -> GetBinContent(eta_bin + 1) / h1D_TruedNdEta_map[Form("h1D_TruedNdEta_Mbin%d", Mbin)] -> GetBinContent(eta_bin + 1));
            // h2D_alpha_correction_map_out["h2D_RotatedBkg_alpha_correction"] -> SetBinError(eta_bin + 1, Mbin + 1, h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_Mbin%d",Mbin)] -> GetBinError(eta_bin + 1)     / h1D_TruedNdEta_map[Form("h1D_TruedNdEta_Mbin%d", Mbin)] -> GetBinContent(eta_bin + 1));

            h2D_alpha_correction_map_out["h2D_typeA_RotatedBkg_alpha_correction"] -> SetBinContent(eta_bin + 1, Mbin + 1, h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_typeA_Mbin%d",Mbin)] -> GetBinContent(eta_bin + 1) / h1D_TruedNdEta_map[Form("h1D_TruedNdEta_Mbin%d", Mbin)] -> GetBinContent(eta_bin + 1));
            // h2D_alpha_correction_map_out["h2D_typeA_RotatedBkg_alpha_correction"] -> SetBinError(eta_bin + 1, Mbin + 1, h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_typeA_Mbin%d",Mbin)] -> GetBinError(eta_bin + 1)     / h1D_TruedNdEta_map[Form("h1D_TruedNdEta_Mbin%d", Mbin)] -> GetBinContent(eta_bin + 1));
        }
    }

    // note : for Inclusive 70
    // todo : the index in the Y axis (for Mbin) is given by (nCentrality_bin + 1)
    for (int eta_bin = 0; eta_bin < nEtaBin; eta_bin++)
    {   
        // note : FitBkg
        // todo : how to properly handle the errors
        h2D_alpha_correction_map_out["h2D_FitBkg_alpha_correction"] -> SetBinContent(eta_bin + 1, (nCentrality_bin + 1), h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_Inclusive70")] -> GetBinContent(eta_bin + 1) / h1D_TruedNdEta_map[Form("h1D_TruedNdEta_Inclusive70" )] -> GetBinContent(eta_bin + 1));
        // h2D_alpha_correction_map_out["h2D_FitBkg_alpha_correction"] -> SetBinError(eta_bin + 1, (nCentrality_bin + 1), h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_Inclusive70")] -> GetBinError(eta_bin + 1)     / h1D_TruedNdEta_map[Form("h1D_TruedNdEta_Inclusive70" )] -> GetBinContent(eta_bin + 1));
        
        h2D_alpha_correction_map_out["h2D_typeA_FitBkg_alpha_correction"] -> SetBinContent(eta_bin + 1, (nCentrality_bin + 1), h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_typeA_Inclusive70")] -> GetBinContent(eta_bin + 1) / h1D_TruedNdEta_map[Form("h1D_TruedNdEta_Inclusive70" )] -> GetBinContent(eta_bin + 1));
        // h2D_alpha_correction_map_out["h2D_typeA_FitBkg_alpha_correction"] -> SetBinError(eta_bin + 1, (nCentrality_bin + 1), h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_typeA_Inclusive70")] -> GetBinError(eta_bin + 1)     / h1D_TruedNdEta_map[Form("h1D_TruedNdEta_Inclusive70" )] -> GetBinContent(eta_bin + 1));

        // note : RotatedBkg
        h2D_alpha_correction_map_out["h2D_RotatedBkg_alpha_correction"] -> SetBinContent(eta_bin + 1, (nCentrality_bin + 1), h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_Inclusive70")] -> GetBinContent(eta_bin + 1) / h1D_TruedNdEta_map[Form("h1D_TruedNdEta_Inclusive70" )] -> GetBinContent(eta_bin + 1));
        // h2D_alpha_correction_map_out["h2D_RotatedBkg_alpha_correction"] -> SetBinError(eta_bin + 1, (nCentrality_bin + 1), h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_Inclusive70")] -> GetBinError(eta_bin + 1)     / h1D_TruedNdEta_map[Form("h1D_TruedNdEta_Inclusive70" )] -> GetBinContent(eta_bin + 1));

        h2D_alpha_correction_map_out["h2D_typeA_RotatedBkg_alpha_correction"] -> SetBinContent(eta_bin + 1, (nCentrality_bin + 1), h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_typeA_Inclusive70")] -> GetBinContent(eta_bin + 1) / h1D_TruedNdEta_map[Form("h1D_TruedNdEta_Inclusive70" )] -> GetBinContent(eta_bin + 1));
        // h2D_alpha_correction_map_out["h2D_typeA_RotatedBkg_alpha_correction"] -> SetBinError(eta_bin + 1, (nCentrality_bin + 1), h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_typeA_Inclusive70")] -> GetBinError(eta_bin + 1)     / h1D_TruedNdEta_map[Form("h1D_TruedNdEta_Inclusive70" )] -> GetBinContent(eta_bin + 1));
    }

    // note : for Inclusive 100
    // todo : the index in the Y axis (for Mbin) is given by (nCentrality_bin + 2)
    for (int eta_bin = 0; eta_bin < nEtaBin; eta_bin++)
    {   
        // note : FitBkg
        // todo : how to properly handle the errors
        h2D_alpha_correction_map_out["h2D_FitBkg_alpha_correction"] -> SetBinContent(eta_bin + 1, (nCentrality_bin + 2), h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_Inclusive100")] -> GetBinContent(eta_bin + 1) / h1D_TruedNdEta_map[Form("h1D_TruedNdEta_Inclusive100" )] -> GetBinContent(eta_bin + 1));
        // h2D_alpha_correction_map_out["h2D_FitBkg_alpha_correction"] -> SetBinError(eta_bin + 1, (nCentrality_bin + 2), h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_Inclusive100")] -> GetBinError(eta_bin + 1)     / h1D_TruedNdEta_map[Form("h1D_TruedNdEta_Inclusive100" )] -> GetBinContent(eta_bin + 1));
        
        h2D_alpha_correction_map_out["h2D_typeA_FitBkg_alpha_correction"] -> SetBinContent(eta_bin + 1, (nCentrality_bin + 2), h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_typeA_Inclusive100")] -> GetBinContent(eta_bin + 1) / h1D_TruedNdEta_map[Form("h1D_TruedNdEta_Inclusive100" )] -> GetBinContent(eta_bin + 1));
        // h2D_alpha_correction_map_out["h2D_typeA_FitBkg_alpha_correction"] -> SetBinError(eta_bin + 1, (nCentrality_bin + 2), h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_typeA_Inclusive100")] -> GetBinError(eta_bin + 1)     / h1D_TruedNdEta_map[Form("h1D_TruedNdEta_Inclusive100" )] -> GetBinContent(eta_bin + 1));

        // note : RotatedBkg
        h2D_alpha_correction_map_out["h2D_RotatedBkg_alpha_correction"] -> SetBinContent(eta_bin + 1, (nCentrality_bin + 2), h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_Inclusive100")] -> GetBinContent(eta_bin + 1) / h1D_TruedNdEta_map[Form("h1D_TruedNdEta_Inclusive100" )] -> GetBinContent(eta_bin + 1));
        // h2D_alpha_correction_map_out["h2D_RotatedBkg_alpha_correction"] -> SetBinError(eta_bin + 1, (nCentrality_bin + 2), h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_Inclusive100")] -> GetBinError(eta_bin + 1)     / h1D_TruedNdEta_map[Form("h1D_TruedNdEta_Inclusive100" )] -> GetBinContent(eta_bin + 1));

        h2D_alpha_correction_map_out["h2D_typeA_RotatedBkg_alpha_correction"] -> SetBinContent(eta_bin + 1, (nCentrality_bin + 2), h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_typeA_Inclusive100")] -> GetBinContent(eta_bin + 1) / h1D_TruedNdEta_map[Form("h1D_TruedNdEta_Inclusive100" )] -> GetBinContent(eta_bin + 1));
        // h2D_alpha_correction_map_out["h2D_typeA_RotatedBkg_alpha_correction"] -> SetBinError(eta_bin + 1, (nCentrality_bin + 2), h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_typeA_Inclusive100")] -> GetBinError(eta_bin + 1)     / h1D_TruedNdEta_map[Form("h1D_TruedNdEta_Inclusive100" )] -> GetBinContent(eta_bin + 1));
    }

    std::cout<<"cccc h1D_FitBkg_RecoTrackletEtaPerEvt_map[\"h1D_Inclusive100\"]->GetEntries(): "<<h1D_FitBkg_RecoTrackletEtaPerEvt_map["h1D_Inclusive100"]->GetEntries()<<std::endl;

}

void PreparedNdEta::EndRun()
{
    

    std::cout<<"aa"<<111<<std::endl;

    std::cout<<"c h1D_FitBkg_RecoTrackletEtaPerEvt_map[\"h1D_Inclusive100\"]->GetEntries(): "<<h1D_FitBkg_RecoTrackletEtaPerEvt_map["h1D_Inclusive100"]->GetEntries()<<std::endl;

    file_out_dNdEta -> cd();
    for (int Mbin = 0; Mbin < nCentrality_bin; Mbin++){
        h2D_input_map[Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d", Mbin)] -> Write();
        h2D_input_map[Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d_rotated", Mbin)] -> Write();
        h2D_input_map[Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d", Mbin)] -> Write();
        h2D_input_map[Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d_rotated", Mbin)] -> Write();

        h2D_input_map[Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d", Mbin)] -> Write();
        h2D_input_map[Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d_rotated", Mbin)] -> Write();
        h2D_input_map[Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d", Mbin)] -> Write();
        h2D_input_map[Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d_rotated", Mbin)] -> Write();

        h2D_input_map[Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d_FineBin", Mbin)] -> Write();
        h2D_input_map[Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d_FineBin", Mbin)] -> Write();
        h2D_input_map[Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d_FineBin", Mbin)] -> Write();
        h2D_input_map[Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d_FineBin", Mbin)] -> Write();
    }

    std::cout<<"d h1D_FitBkg_RecoTrackletEtaPerEvt_map[\"h1D_Inclusive100\"]->GetEntries(): "<<h1D_FitBkg_RecoTrackletEtaPerEvt_map["h1D_Inclusive100"]->GetEntries()<<std::endl;

    std::cout<<"aa"<<222<<std::endl;

    hstack2D_map["hstack2D_GoodProtoTracklet_EtaVtxZ_Inclusive100"] -> Write();
    hstack2D_map["hstack2D_GoodProtoTracklet_EtaVtxZ_Inclusive100_rotated"] -> Write();
    hstack2D_map["hstack2D_GoodProtoTracklet_EtaVtxZ_Inclusive70"] -> Write();
    hstack2D_map["hstack2D_GoodProtoTracklet_EtaVtxZ_Inclusive70_rotated"] -> Write();

    hstack2D_map["hstack2D_typeA_GoodProtoTracklet_EtaVtxZ_Inclusive100"] -> Write();
    hstack2D_map["hstack2D_typeA_GoodProtoTracklet_EtaVtxZ_Inclusive100_rotated"] -> Write();
    hstack2D_map["hstack2D_typeA_GoodProtoTracklet_EtaVtxZ_Inclusive70"] -> Write();
    hstack2D_map["hstack2D_typeA_GoodProtoTracklet_EtaVtxZ_Inclusive70_rotated"] -> Write();

    hstack2D_map["hstack2D_GoodProtoTracklet_EtaVtxZ_Inclusive100_FineBin"] -> Write();
    hstack2D_map["hstack2D_GoodProtoTracklet_EtaVtxZ_Inclusive70_FineBin"] -> Write();
    hstack2D_map["hstack2D_typeA_GoodProtoTracklet_EtaVtxZ_Inclusive100_FineBin"] -> Write();
    hstack2D_map["hstack2D_typeA_GoodProtoTracklet_EtaVtxZ_Inclusive70_FineBin"] -> Write();

    std::cout<<"aa"<<333<<std::endl;

    std::cout<<"e h1D_FitBkg_RecoTrackletEtaPerEvt_map[\"h1D_Inclusive100\"]->GetEntries(): "<<h1D_FitBkg_RecoTrackletEtaPerEvt_map["h1D_Inclusive100"]->GetEntries()<<std::endl;

    // note : reco, multiplicity
    for (int Mbin = 0; Mbin < nCentrality_bin; Mbin++){
        h1D_FitBkg_RecoTrackletEta_map[Form("h1D_Mbin%d",Mbin)] -> Write();
        h1D_RotatedBkg_RecoTrackletEta_map[Form("h1D_Mbin%d",Mbin)] -> Write();

        h1D_FitBkg_RecoTrackletEta_map[Form("h1D_typeA_Mbin%d",Mbin)] -> Write();
        h1D_RotatedBkg_RecoTrackletEta_map[Form("h1D_typeA_Mbin%d",Mbin)] -> Write();
    }

    h1D_FitBkg_RecoTrackletEta_map[Form("h1D_Inclusive100")] -> Write();
    h1D_FitBkg_RecoTrackletEta_map[Form("h1D_Inclusive70")]  -> Write();
    h1D_FitBkg_RecoTrackletEta_map[Form("h1D_typeA_Inclusive100")] -> Write();
    h1D_FitBkg_RecoTrackletEta_map[Form("h1D_typeA_Inclusive70")]  -> Write();

    h1D_RotatedBkg_RecoTrackletEta_map[Form("h1D_Inclusive100")] -> Write();
    h1D_RotatedBkg_RecoTrackletEta_map[Form("h1D_Inclusive70")]  -> Write();
    h1D_RotatedBkg_RecoTrackletEta_map[Form("h1D_typeA_Inclusive100")] -> Write();
    h1D_RotatedBkg_RecoTrackletEta_map[Form("h1D_typeA_Inclusive70")]  -> Write();
    
    std::cout<<"aa"<<444<<std::endl;

    std::cout<<"f h1D_FitBkg_RecoTrackletEtaPerEvt_map[\"h1D_Inclusive100\"]->GetEntries(): "<<h1D_FitBkg_RecoTrackletEtaPerEvt_map["h1D_Inclusive100"]->GetEntries()<<std::endl;

    // note : multiplicity
    if (runnumber == -1){

        hstack2D_map["hstack2D_TrueEtaVtxZ_Inclusive100"] -> Write();
        hstack2D_map["hstack2D_TrueEtaVtxZ_Inclusive100_FineBin"] -> Write();
        hstack2D_map["hstack2D_TrueEtaVtxZ_Inclusive70"] -> Write();
        hstack2D_map["hstack2D_TrueEtaVtxZ_Inclusive70_FineBin"] -> Write();

        hstack1D_map["hstack1D_TrueEta_Inclusive100"] -> Write();
        hstack1D_map["hstack1D_TrueEta_Inclusive70"] -> Write();

        for (int i = 0; i < nCentrality_bin; i++){
            hstack1D_map[Form("hstack1D_TrueEta_Mbin%d", i)] -> Write();
        }

        std::cout<<"aa"<<555<<std::endl;
    }

    // note : N things for per event 
    if (runnumber == -1){
        h1D_TruedNdEta_map["h1D_TruedNdEta_Inclusive100"] -> Write("h1D_TruedNdEta_Inclusive100");
        h1D_TruedNdEta_map["h1D_TruedNdEta_Inclusive70"] -> Write("h1D_TruedNdEta_Inclusive70");
        for (int i = 0; i < nCentrality_bin; i++){
            h1D_TruedNdEta_map[Form("h1D_TruedNdEta_Mbin%d", i)] -> Write(Form("h1D_TruedNdEta_Mbin%d", i));
        }
    }

    std::cout<<"g h1D_FitBkg_RecoTrackletEtaPerEvt_map[\"h1D_Inclusive100\"]->GetEntries(): "<<h1D_FitBkg_RecoTrackletEtaPerEvt_map["h1D_Inclusive100"]->GetEntries()<<std::endl;

    std::cout<<"aa"<<666<<std::endl;

    for (auto pair : h1D_FitBkg_RecoTrackletEtaPerEvt_map){
        std::cout<<"maps in h1D_FitBkg_RecoTrackletEtaPerEvt_map: "<<pair.first<<", hist entries: "<<pair.second->GetEntries()<<std::endl;
    }


    h1D_FitBkg_RecoTrackletEtaPerEvt_map["h1D_Inclusive100"] -> Write("a");
    h1D_FitBkg_RecoTrackletEtaPerEvt_map["h1D_Inclusive70"] -> Write("b");
    h1D_FitBkg_RecoTrackletEtaPerEvt_map["h1D_typeA_Inclusive100"] -> Write("c");
    h1D_FitBkg_RecoTrackletEtaPerEvt_map["h1D_typeA_Inclusive70"] -> Write("d");

    std::cout<<"aa"<<66666<<std::endl;

    for (int i = 0; i < nCentrality_bin; i++){
        h1D_FitBkg_RecoTrackletEtaPerEvt_map[Form("h1D_Mbin%d", i)] -> Write();
        h1D_FitBkg_RecoTrackletEtaPerEvt_map[Form("h1D_typeA_Mbin%d", i)] -> Write();
    }

    std::cout<<"aa"<<777<<std::endl;

    h1D_RotatedBkg_RecoTrackletEtaPerEvt_map["h1D_Inclusive100"] -> Write();
    h1D_RotatedBkg_RecoTrackletEtaPerEvt_map["h1D_Inclusive70"] -> Write();
    h1D_RotatedBkg_RecoTrackletEtaPerEvt_map["h1D_typeA_Inclusive100"] -> Write();
    h1D_RotatedBkg_RecoTrackletEtaPerEvt_map["h1D_typeA_Inclusive70"] -> Write();

    for (int i = 0; i < nCentrality_bin; i++){
        h1D_RotatedBkg_RecoTrackletEtaPerEvt_map[Form("h1D_Mbin%d", i)] -> Write();
        h1D_RotatedBkg_RecoTrackletEtaPerEvt_map[Form("h1D_typeA_Mbin%d", i)] -> Write();
    }

    std::cout<<"aa"<<888<<std::endl;

    // note : post the alpha correction {if it's applied}
    h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_map["h1D_Inclusive100"] -> Write();
    h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_map["h1D_Inclusive70"] -> Write();
    h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_map["h1D_typeA_Inclusive100"] -> Write();
    h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_map["h1D_typeA_Inclusive70"] -> Write();

    for (int i = 0; i < nCentrality_bin; i++){
        h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_Mbin%d", i)] -> Write();
        h1D_FitBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_typeA_Mbin%d", i)] -> Write();
    }

    std::cout<<"aa"<<999<<std::endl;

    h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map["h1D_Inclusive100"] -> Write();
    h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map["h1D_Inclusive70"] -> Write();
    h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map["h1D_typeA_Inclusive100"] -> Write();
    h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map["h1D_typeA_Inclusive70"] -> Write();

    for (int i = 0; i < nCentrality_bin; i++){
        h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_Mbin%d", i)] -> Write();
        h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_typeA_Mbin%d", i)] -> Write();
    }

    std::cout<<"aa"<<111101010<<std::endl;

    // note : the alpha correction map
    for (auto &pair : h2D_alpha_correction_map_out){
        pair.second -> Write();
    }

    std::cout<<"aa"<<111122222<<std::endl;

    file_out_dNdEta -> Close();

    std::cout<<"aa"<<11113333<<std::endl;

    file_out -> cd();
    file_out -> Close();
}

std::vector<double> PreparedNdEta::find_Ngroup(TH1D * hist_in)
{
    double Highest_bin_Content  = hist_in -> GetBinContent(hist_in -> GetMaximumBin());
    double Highest_bin_Center   = hist_in -> GetBinCenter(hist_in -> GetMaximumBin());

    int group_Nbin = 0;
    int peak_group_ID = -9999;
    double group_entry = 0;
    double peak_group_ratio;
    std::vector<int> group_Nbin_vec; group_Nbin_vec.clear();
    std::vector<double> group_entry_vec; group_entry_vec.clear();
    std::vector<double> group_widthL_vec; group_widthL_vec.clear();
    std::vector<double> group_widthR_vec; group_widthR_vec.clear();

    for (int i = 0; i < hist_in -> GetNbinsX(); i++){
        // todo : the background rejection is here : Highest_bin_Content/2. for the time being
        double bin_content = ( hist_in -> GetBinContent(i+1) <= Highest_bin_Content/2.) ? 0. : ( hist_in -> GetBinContent(i+1) - Highest_bin_Content/2. );

        if (bin_content != 0){
            
            if (group_Nbin == 0) {
                group_widthL_vec.push_back(hist_in -> GetBinCenter(i+1) - (hist_in -> GetBinWidth(i+1)/2.));
            }

            group_Nbin += 1; 
            group_entry += bin_content;
        }
        else if (bin_content == 0 && group_Nbin != 0){
            group_widthR_vec.push_back(hist_in -> GetBinCenter(i+1) - (hist_in -> GetBinWidth(i+1)/2.));
            group_Nbin_vec.push_back(group_Nbin);
            group_entry_vec.push_back(group_entry);
            group_Nbin = 0;
            group_entry = 0;
        }
    }
    if (group_Nbin != 0) {
        group_Nbin_vec.push_back(group_Nbin);
        group_entry_vec.push_back(group_entry);
        group_widthR_vec.push_back(hist_in -> GetXaxis()->GetXmax());
    } // note : the last group at the edge

    // note : find the peak group
    for (int i = 0; i < int(group_Nbin_vec.size()); i++){
        if (group_widthL_vec[i] < Highest_bin_Center && Highest_bin_Center < group_widthR_vec[i]){
            peak_group_ID = i;
            break;
        }
    }
    
    // note : On Nov 6, 2024, we no longer need to calculate the ratio of the peak group
    // double denominator = (accumulate( group_entry_vec.begin(), group_entry_vec.end(), 0.0 ));
    // denominator = (denominator <= 0) ? 1. : denominator;
    // peak_group_ratio = group_entry_vec[peak_group_ID] / denominator;
    peak_group_ratio = -999;

    double peak_group_left  = (double(group_Nbin_vec.size()) == 0) ? -999 : group_widthL_vec[peak_group_ID];
    double peak_group_right = (double(group_Nbin_vec.size()) == 0) ? 999  : group_widthR_vec[peak_group_ID];

    // for (int i = 0; i < group_Nbin_vec.size(); i++)
    // {
    //     cout<<" "<<endl;
    //     cout<<"group size : "<<group_Nbin_vec[i]<<endl;
    //     cout<<"group entry : "<<group_entry_vec[i]<<endl;
    //     cout<<group_widthL_vec[i]<<" "<<group_widthR_vec[i]<<endl;
    // }

    // cout<<" "<<endl;
    // cout<<"N group : "<<group_Nbin_vec.size()<<endl;
    // cout<<"Peak group ID : "<<peak_group_ID<<endl;
    // cout<<"peak group width : "<<group_widthL_vec[peak_group_ID]<<" "<<group_widthR_vec[peak_group_ID]<<endl;
    // cout<<"ratio : "<<peak_group_ratio<<endl;
    
    // note : {N_group, ratio (if two), peak widthL, peak widthR}
    return {double(group_Nbin_vec.size()), peak_group_ratio, peak_group_left, peak_group_right};
}

double PreparedNdEta::get_dist_offset(TH1D * hist_in, int check_N_bin) // note : check_N_bin 1 to N bins of hist
{
    if (check_N_bin < 0 || check_N_bin > hist_in -> GetNbinsX()) {std::cout<<" wrong check_N_bin "<<std::endl; exit(1);}
    double total_entry = 0;
    for (int i = 0; i < check_N_bin; i++)
    {
        total_entry += hist_in -> GetBinContent(i+1); // note : 1, 2, 3.....
        total_entry += hist_in -> GetBinContent(hist_in -> GetNbinsX() - i);
    }

    return total_entry/double(2. * check_N_bin);
}