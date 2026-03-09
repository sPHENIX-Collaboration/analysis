#include "PreparedNdEtaEach.h"

PreparedNdEtaEach::PreparedNdEtaEach(
    int process_id_in,
    int runnumber_in,
    std::string input_directory_in,
    std::string input_file_name_in,
    std::string output_directory_in,
    std::string output_file_name_suffix_in,

    bool ApplyAlphaCorr_in,
    bool ApplyGeoAccCorr_in,

    bool isTypeA_in,
    std::pair<double,double> cut_INTTvtxZ_in,
    int SelectedMbin_in
) : process_id(process_id_in),
    runnumber(runnumber_in),
    input_directory(input_directory_in),
    input_file_name(input_file_name_in),
    output_directory(output_directory_in),
    output_file_name_suffix(output_file_name_suffix_in),

    ApplyAlphaCorr(ApplyAlphaCorr_in),
    ApplyGeoAccCorr(ApplyGeoAccCorr_in),

    isTypeA(isTypeA_in),
    cut_INTTvtxZ(cut_INTTvtxZ_in),
    SelectedMbin(SelectedMbin_in)
{
    PrepareInputRootFie();

    if (SelectedMbin >= nCentrality_bin && SelectedMbin != Semi_inclusive_interval && SelectedMbin != 100){
        std::cout << "Error : SelectedMbin is out of range" << std::endl;
        std::cout << "SelectedMbin : " << SelectedMbin <<", but : nCentrality_bin: "<< nCentrality_bin << std::endl;
        exit(1);
    }

    vtxZ_index_map = GetVtxZIndexMap();

    PrepareOutPutFileName();

    PrepareOutPutRootFile();

    PrepareHistFits();

    c1 = new TCanvas("c1", "c1", 950, 800);
    c1 -> Print(Form("%s/%s(", output_directory.c_str(), output_filename_pdf.c_str()));

    h1D_alpha_correction_map_in.clear();
    h1D_alpha_correction_map_out.clear();

    h2D_GeoAccCorr_map.clear();

    std::string isTypeA_str = (isTypeA) ? "_typeA" : "";
    GeoAccCorr_name_map = {
        Form("h2D%s_BestPair_EtaVtxZ_Rebin_Selected",isTypeA_str.c_str()),
        Form("h2D%s_BestPair_EtaVtxZ_Rebin_Selected",isTypeA_str.c_str())
    };
}

void PreparedNdEtaEach::PrepareInputRootFie()
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

std::map<int, int> PreparedNdEtaEach::GetVtxZIndexMap()
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

void PreparedNdEtaEach::PrepareOutPutFileName()
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

    output_filename = "PreparedNdEtaEach";
    output_filename = (runnumber != -1) ? "Data_" + output_filename : "MC_" + output_filename;

    output_filename += (ApplyAlphaCorr) ? "_AlphaCorr" : "";
    output_filename += (ApplyGeoAccCorr) ? "_GeoAccCorr" : "";

    output_filename += (isTypeA) ? "_TypeA" : "_AllSensor";
    output_filename += Form("_VtxZ%.0f", fabs(cut_INTTvtxZ.first - cut_INTTvtxZ.second)/2.); // todo : check this
    output_filename += Form("_Mbin%d",SelectedMbin);
    
    output_filename += output_file_name_suffix;
    
    output_filename_DeltaPhi = output_filename;
    output_filename_dNdEta = output_filename;
    output_filename_pdf = output_filename;
    
    // todo: check th name 
    output_filename_DeltaPhi += (runnumber != -1) ? Form("_%s_%s_DeltaPhi.root",runnumber_str.c_str(),job_index.c_str()) : Form("_%s_DeltaPhi.root",job_index.c_str());
    output_filename_dNdEta   += (runnumber != -1) ? Form("_%s_%s_dNdEta.root",runnumber_str.c_str(),job_index.c_str()) : Form("_%s_dNdEta.root",job_index.c_str());
    output_filename_pdf      += (runnumber != -1) ? Form("_%s_%s.pdf",runnumber_str.c_str(),job_index.c_str()) : Form("_%s.pdf",job_index.c_str());
}

void PreparedNdEtaEach::PrepareOutPutRootFile()
{
    file_out_DeltaPhi = new TFile(Form("%s/%s",output_directory.c_str(),output_filename_DeltaPhi.c_str()), "RECREATE");
    file_out_dNdEta = new TFile(Form("%s/%s",output_directory.c_str(),output_filename_dNdEta.c_str()), "RECREATE");
}

void PreparedNdEtaEach::PrepareHistFits()
{
    hstack1D_BestPair_ClusPhiSize = new THStack("hstack1D_BestPair_ClusPhiSize","hstack1D_BestPair_ClusPhiSize");
    hstack1D_BestPair_ClusAdc = new THStack("hstack1D_BestPair_ClusAdc","hstack1D_BestPair_ClusAdc");
    hstack1D_BestPair_DeltaPhi = new THStack("hstack1D_BestPair_DeltaPhi","hstack1D_BestPair_DeltaPhi");
    hstack1D_BestPair_DeltaEta = new THStack("hstack1D_BestPair_DeltaEta","hstack1D_BestPair_DeltaEta");
    hstack2D_BestPairEtaVtxZ = new THStack("hstack2D_BestPairEtaVtxZ","hstack2D_BestPairEtaVtxZ");
    hstack2D_BestPairEtaVtxZ_FineBin = new THStack("hstack2D_BestPairEtaVtxZ_FineBin","hstack2D_BestPairEtaVtxZ_FineBin");

    // Division:-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    hstack2D_GoodProtoTracklet_map.clear();

    hstack2D_GoodProtoTracklet_map.insert( std::make_pair(
            Form("hstack2D_GoodProtoTracklet_EtaVtxZ"),
            new THStack(Form("hstack2D_GoodProtoTracklet_EtaVtxZ"), Form("hstack2D_GoodProtoTracklet_EtaVtxZ;Pair #eta;INTT vtxZ [cm]")) 
        )
    ).second;

    // note : normal fine bin
    hstack2D_GoodProtoTracklet_map.insert( std::make_pair(
            Form("hstack2D_GoodProtoTracklet_EtaVtxZ_FineBin"),
            new THStack(Form("hstack2D_GoodProtoTracklet_EtaVtxZ_FineBin"), Form("hstack2D_GoodProtoTracklet_EtaVtxZ_FineBin;Pair #eta;INTT vtxZ [cm]")) 
        )
    ).second;    

    // note : rotated
    hstack2D_GoodProtoTracklet_map.insert( std::make_pair(
            Form("hstack2D_GoodProtoTracklet_EtaVtxZ_rotated"),
            new THStack(Form("hstack2D_GoodProtoTracklet_EtaVtxZ_rotated"), Form("hstack2D_GoodProtoTracklet_EtaVtxZ_rotated;Pair #eta;INTT vtxZ [cm]")) 
        )
    ).second;


    // Division:-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    h1D_BestPair_RecoTrackletEta_map.clear();
    h1D_BestPair_RecoTrackletEta_map.insert( std::make_pair(
            Form("h1D_BestPair_RecoTrackletEta"),
            new TH1D(Form("h1D_BestPair_RecoTrackletEta"), Form("h1D_BestPair_RecoTrackletEta;Tracklet #eta;Entries"), nEtaBin, EtaEdge_min, EtaEdge_max)
        )
    ).second;

    h1D_RotatedBkg_RecoTrackletEta_map.clear();
    h1D_RotatedBkg_RecoTrackletEta_map.insert( std::make_pair(
            Form("h1D_RotatedBkg_RecoTrackletEta"),
            new TH1D(Form("h1D_RotatedBkg_RecoTrackletEta"), Form("h1D_RotatedBkg_RecoTrackletEta;Tracklet #eta;Entries"), nEtaBin, EtaEdge_min, EtaEdge_max)
        )
    ).second;

    h1D_RotatedBkg_DeltaPhi_Signal_map.clear();
    for (int eta_bin = 0; eta_bin < nEtaBin; eta_bin++){
        h1D_RotatedBkg_DeltaPhi_Signal_map.insert(
            std::make_pair(
                Form("h1D_RotatedBkg_DeltaPhi_Signal_Eta%d", eta_bin),
                new TH1D(Form("h1D_RotatedBkg_DeltaPhi_Signal_Eta%d", eta_bin), Form("h1D_RotatedBkg_DeltaPhi_Signal_Eta%d;Pair #Delta#phi [radian];Entries", eta_bin), nDeltaPhiBin, DeltaPhiEdge_min, DeltaPhiEdge_max)
            )
        );

        h1D_RotatedBkg_DeltaPhi_Signal_map[Form("h1D_RotatedBkg_DeltaPhi_Signal_Eta%d", eta_bin)] -> Sumw2(true);

        if (isTypeA) {
            h1D_RotatedBkg_DeltaPhi_Signal_map[Form("h1D_RotatedBkg_DeltaPhi_Signal_Eta%d", eta_bin)] -> GetXaxis() -> SetTitle("Pair #Delta#phi (type A) [radian]");
        }

    }

    // Division:-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    hstack1D_DeltaPhi_map.clear();
    for (int eta_bin = 0; eta_bin < nEtaBin; eta_bin++)
    {
        hstack1D_DeltaPhi_map.insert( std::make_pair(
                Form("hstack1D_DeltaPhi_Eta%d", eta_bin),
                new THStack(Form("hstack1D_DeltaPhi_Eta%d", eta_bin), Form("hstack1D_DeltaPhi_Eta%d;Pair #Delta#phi [radian];Entries", eta_bin))
            )
        ).second;

        
        hstack1D_DeltaPhi_map.insert( std::make_pair(
                Form("hstack1D_DeltaPhi_Eta%d_rotated", eta_bin),
                new THStack(Form("hstack1D_DeltaPhi_Eta%d_rotated", eta_bin), Form("hstack1D_DeltaPhi_Eta%d_rotated;Pair #Delta#phi [radian];Entries", eta_bin))
            )
        ).second;
    }

    // Division:-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // f1_BkgPol2_Fit_map.clear();
    // f1_BkgPol2_Draw_map.clear();

    f1_SigBkgPol2_Fit_map.clear();
    f1_SigBkgPol2_DrawSig_map.clear();
    f1_SigBkgPol2_DrawBkgPol2_map.clear();

    f1_BkgPolTwo_Fit_map.clear();

    for (auto &pair : hstack1D_DeltaPhi_map){
        if (pair.first.find("hstack1D_DeltaPhi") != std::string::npos){

            std::string f1_name = pair.first + "_BkgPolTwo_Fit";
            f1_BkgPolTwo_Fit_map.insert(
                std::make_pair(
                    f1_name,
                    new TF1(f1_name.c_str(), "pol2", DeltaPhiEdge_min, DeltaPhiEdge_max)
                )
            ).second;
            f1_BkgPolTwo_Fit_map[f1_name] -> SetLineColor(6);
            f1_BkgPolTwo_Fit_map[f1_name] -> SetLineStyle(9);
            f1_BkgPolTwo_Fit_map[f1_name] -> SetLineWidth(3);
            f1_BkgPolTwo_Fit_map[f1_name] -> SetNpx(1000);


            // f1_name = pair.first + "_BkgPol2_Fit";
            // f1_BkgPol2_Fit_map.insert(
            //     std::make_pair(
            //         f1_name,
            //         new TF1(f1_name.c_str(), bkg_pol2_func, DeltaPhiEdge_min, DeltaPhiEdge_max, 6)
            //     )
            // ).second;
            // f1_BkgPol2_Fit_map[f1_name] -> SetLineColor(2);
            // f1_BkgPol2_Fit_map[f1_name] -> SetNpx(1000);


            // f1_name = pair.first + "_BkgPol2_Draw";
            // f1_BkgPol2_Draw_map.insert(
            //     std::make_pair(
            //         f1_name,
            //         new TF1(f1_name.c_str(), full_pol2_func, DeltaPhiEdge_min, DeltaPhiEdge_max, 4)
            //     )
            // ).second;
            // f1_BkgPol2_Draw_map[f1_name] -> SetLineColor(6);
            // f1_BkgPol2_Draw_map[f1_name] -> SetLineStyle(9);
            // f1_BkgPol2_Draw_map[f1_name] -> SetLineWidth(3);
            // f1_BkgPol2_Draw_map[f1_name] -> SetNpx(1000);

            
            f1_name = pair.first + "_SigBkgPol2_Fit";
            f1_SigBkgPol2_Fit_map.insert(
                std::make_pair(
                    f1_name,
                    new TF1(f1_name.c_str(), gaus_pol2_func, DeltaPhiEdge_min, DeltaPhiEdge_max, 7)
                )
            ).second;
            f1_SigBkgPol2_Fit_map[f1_name] -> SetLineColor(46);
            f1_SigBkgPol2_Fit_map[f1_name] -> SetLineStyle(1);
            f1_SigBkgPol2_Fit_map[f1_name] -> SetNpx(1000);


            f1_name = pair.first + "_SigBkgPol2_DrawSig";
            f1_SigBkgPol2_DrawSig_map.insert(
                std::make_pair(
                    f1_name,
                    new TF1(f1_name.c_str(), gaus_func, DeltaPhiEdge_min, DeltaPhiEdge_max, 4)
                )
            ).second;
            f1_SigBkgPol2_DrawSig_map[f1_name] -> SetLineColor(46);
            f1_SigBkgPol2_DrawSig_map[f1_name] -> SetLineStyle(2);
            f1_SigBkgPol2_DrawSig_map[f1_name] -> SetNpx(1000);


            f1_name = pair.first + "_SigBkgPol2_DrawBkgPol2";
            f1_SigBkgPol2_DrawBkgPol2_map.insert(
                std::make_pair(
                    f1_name,
                    new TF1(f1_name.c_str(), full_pol2_func, DeltaPhiEdge_min, DeltaPhiEdge_max, 4)
                )
            ).second;
            f1_SigBkgPol2_DrawBkgPol2_map[f1_name] -> SetLineColor(8);
            f1_SigBkgPol2_DrawBkgPol2_map[f1_name] -> SetLineStyle(2);
            f1_SigBkgPol2_DrawBkgPol2_map[f1_name] -> SetNpx(1000);

        }
    }

    // Division:-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    if (runnumber == -1){
        hstack2D_TrueEtaVtxZ_map.clear();

        // note : centrality inclusive 
        hstack2D_TrueEtaVtxZ_map.insert( std::make_pair(
                Form("hstack2D_TrueEtaVtxZ"),
                new THStack(Form("hstack2D_TrueEtaVtxZ"), Form("hstack2D_TrueEtaVtxZ;PHG4Particle #eta;TruthPV_trig_z [cm]"))
            )
        ).second;

        hstack2D_TrueEtaVtxZ_map.insert( std::make_pair(
                Form("hstack2D_TrueEtaVtxZ_FineBin"),
                new THStack(Form("hstack2D_TrueEtaVtxZ_FineBin"), Form("hstack2D_TrueEtaVtxZ_FineBin;PHG4Particle #eta;TruthPV_trig_z [cm]"))
            )
        ).second;
    }

    // Division:-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    if (runnumber == -1){
        hstack1D_TrueEta_map.clear();

        hstack1D_TrueEta_map.insert( std::make_pair(
            Form("hstack1D_TrueEta"),
            new THStack(Form("hstack1D_TrueEta"), Form("hstack1D_TrueEta;PHG4Particle #eta;Entries"))
        )
        ).second;

    }

}

// note : return the bin index of eta, vtxZ, Mbin, typeA, rotated, finebin
std::tuple<int, int, int, int, int, int> PreparedNdEtaEach::GetHistStringInfo(std::string hist_name)
{

    // std::cout<<"In GetHistStringInfo(), input name: "<<hist_name<<std::endl;
    
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

void PreparedNdEtaEach::PrepareStacks()
{
    std::cout<<"In PrepareStacks()"<<std::endl;

    int l_eta_bin, l_vtxz_bin, l_Mbin;
    int l_typeA, l_rotated, l_finebin;

    if (ApplyGeoAccCorr && h2D_GeoAccCorr_map.size() == 1){
        if (
                cut_INTTvtxZ.first < h2D_GeoAccCorr_map[GeoAccCorr_name_map[0]] -> GetYaxis() -> GetXmin() || // todo: the map ID
                cut_INTTvtxZ.second > h2D_GeoAccCorr_map[GeoAccCorr_name_map[0]] -> GetYaxis() -> GetXmax()   // todo: the map ID
            )
        {
            std::cout<<"Error: cut_INTTvtxZ is outside the h2D_GeoAccr map"<<std::endl;
            exit(1);
        }
    }
    else if (ApplyGeoAccCorr && h2D_GeoAccCorr_map.size() != 1){
        std::cout<<"Error: h2D_GeoAccCorr_map is empty"<<std::endl;
        exit(1);
    }

    for (auto &pair : h1D_input_map)
    {
        std::tie(l_eta_bin, l_vtxz_bin, l_Mbin, l_typeA, l_rotated, l_finebin) = GetHistStringInfo(pair.first);

        // note : the vtxz_bin of the hist is not in the interest
        if (vtxZ_index_map.find(l_vtxz_bin) == vtxZ_index_map.end()){
            continue;
        }

        if (pair.first.find("h1D") != std::string::npos && pair.first.find("_DeltaPhi") != std::string::npos && l_Mbin != -1 && l_eta_bin != -1){

            if (!ApplyGeoAccCorr) {std::cout<<"----- "<<pair.first<<", eta_bin: "<<l_eta_bin<<", vtxz_bin: "<<l_vtxz_bin<<", Mbin: "<<l_Mbin<<", typeA: "<<l_typeA<<", rotated: "<<l_rotated<<", finebin: "<<l_finebin<<std::endl;}
            
            double GeoAcc_correction;

            if (ApplyGeoAccCorr && h2D_GeoAccCorr_map.size() != 0)
            {
                // note : to find the corrsponding eta_bin and vtxz_bin in the h2D_GeoAccCorr_map
                double h1D_eta_center = h2D_input_map["h2D_GoodProtoTracklet_EtaVtxZ_Mbin0"] -> GetXaxis() -> GetBinCenter(l_eta_bin + 1); // note : for DeltaPhi
                double h1D_Z_center   = h2D_input_map["h2D_GoodProtoTracklet_EtaVtxZ_Mbin0"] -> GetYaxis() -> GetBinCenter(l_vtxz_bin + 1); // note : for DeltaPhi
                double h1D_Z_BinWidth = h2D_input_map["h2D_GoodProtoTracklet_EtaVtxZ_Mbin0"] -> GetYaxis() -> GetBinWidth(l_vtxz_bin + 1); // note : for DeltaPhi

                auto temp_RotatedBkg_GeoAccCorr_h2D = h2D_GeoAccCorr_map[GeoAccCorr_name_map[0]]; // todo: the map ID

                if (
                        h1D_Z_center < temp_RotatedBkg_GeoAccCorr_h2D -> GetYaxis() -> GetXmin() || 
                        h1D_Z_center > temp_RotatedBkg_GeoAccCorr_h2D -> GetYaxis() -> GetXmax() || 
                        h1D_Z_BinWidth != temp_RotatedBkg_GeoAccCorr_h2D -> GetYaxis() -> GetBinWidth(1)
                    )
                {
                    std::cout<<"Error: h1D_Z_center is outside the h2D_GeoAccCorr_map or the binwidths are not matched"<<std::endl;
                    exit(1);
                }

                GeoAcc_correction = temp_RotatedBkg_GeoAccCorr_h2D -> GetBinContent( temp_RotatedBkg_GeoAccCorr_h2D -> FindBin(h1D_eta_center, h1D_Z_center) );
                GeoAcc_correction = (GeoAcc_correction <= 0) ? 0. : 1.;

                std::cout<<std::endl;
                std::cout<<"----- "<<pair.first<<", eta_bin: "<<l_eta_bin<<", vtxz_bin: "<<l_vtxz_bin<<", Mbin: "<<l_Mbin<<", typeA: "<<l_typeA<<", rotated: "<<l_rotated<<", finebin: "<<l_finebin<<std::endl;
                std::cout<<"----- eta_center: "<<h1D_eta_center<<", Z_center: "<<h1D_Z_center<<", GeoAcc_correction: "<<GeoAcc_correction<<std::endl;
                std::cout<<"----- h1D_DeltaPhi Integral: "<<pair.second -> Integral()<<std::endl;
                pair.second -> Scale(GeoAcc_correction);
                std::cout<<"----- h1D_DeltaPhi Integral post : "<<pair.second -> Integral()<<std::endl;
                
            }
            else if (ApplyGeoAccCorr && h2D_GeoAccCorr_map.size() == 0)
            {
                std::cout<<"Error: h2D_GeoAccCorr_map is empty"<<std::endl;
                exit(1);
            }
            else 
            {
                GeoAcc_correction = 1;
            }

            // note : normal, with Mbin
            // note : normal, with Inclusive100
            // note : normal, with Inclusive{Semi_inclusive_Mbin * 10}

            // note : typeA, with Mbin
            // note : typeA, with Inclusive100
            // note : typeA, with Inclusive{Semi_inclusive_Mbin * 10}
            
            // Form("hstack1D_DeltaPhi_Eta%d", eta_bin)
            // Form("hstack1D_DeltaPhi_Eta%d_rotated", eta_bin)

            if (isTypeA == l_typeA){ // note : {isTypeA == 0 -> inclusive, l_typeA should be zero}, {isTypeA == 1 -> typeA, l_typeA should be 1}
                if  (
                        SelectedMbin == 100 || 
                        (SelectedMbin == Semi_inclusive_interval && l_Mbin <= Semi_inclusive_Mbin) ||
                        SelectedMbin == l_Mbin
                    )
                { // note : inclusive centrality
                    if (l_rotated == 0){
                        pair.second->SetFillColor(ROOT_color_code[hstack1D_DeltaPhi_map[Form("hstack1D_DeltaPhi_Eta%d", l_eta_bin)]->GetNhists() % ROOT_color_code.size()]);

                        hstack1D_DeltaPhi_map[Form("hstack1D_DeltaPhi_Eta%d", l_eta_bin)] -> Add(pair.second);
                    }
                    else if (l_rotated == 1){
                        pair.second->SetFillColor(ROOT_color_code[hstack1D_DeltaPhi_map[Form("hstack1D_DeltaPhi_Eta%d_rotated", l_eta_bin)]->GetNhists() % ROOT_color_code.size()]);

                        hstack1D_DeltaPhi_map[Form("hstack1D_DeltaPhi_Eta%d_rotated", l_eta_bin)] -> Add(pair.second);
                    }
                    else {
                        std::cout<<"wtf_Inclusive100: "<<pair.first<<std::endl;
                    }
                }
                // todo: need to check if the centrality bin is changed
                // else if (SelectedMbin == Semi_inclusive_Mbin * 10){
                //     if (l_Mbin > Semi_inclusive_Mbin) {continue;}

                //     if (l_rotated == 0){
                //         pair.second->SetFillColor(ROOT_color_code[hstack1D_DeltaPhi_map[Form("hstack1D_DeltaPhi_Eta%d", l_eta_bin)]->GetNhists() % ROOT_color_code.size()]);

                //         hstack1D_DeltaPhi_map[Form("hstack1D_DeltaPhi_Eta%d", l_eta_bin)] -> Add(pair.second);
                //     }
                //     else if (l_rotated == 1){
                //         pair.second->SetFillColor(ROOT_color_code[hstack1D_DeltaPhi_map[Form("hstack1D_DeltaPhi_Eta%d_rotated", l_eta_bin)]->GetNhists() % ROOT_color_code.size()]);

                //         hstack1D_DeltaPhi_map[Form("hstack1D_DeltaPhi_Eta%d_rotated", l_eta_bin)] -> Add(pair.second);
                //     }
                //     else {
                //         std::cout<<"wtf_Inclusive"<<Semi_inclusive_Mbin*10<<" : "<<pair.first<<std::endl;
                //     }
                // }

                // else if (SelectedMbin == l_Mbin){ // note : each bin
                //     if (l_rotated == 0){
                //         pair.second->SetFillColor(ROOT_color_code[hstack1D_DeltaPhi_map[Form("hstack1D_DeltaPhi_Eta%d", l_eta_bin)]->GetNhists() % ROOT_color_code.size()]);

                //         hstack1D_DeltaPhi_map[Form("hstack1D_DeltaPhi_Eta%d", l_eta_bin)] -> Add(pair.second);
                //     }
                //     else if (l_rotated == 1){
                //         pair.second->SetFillColor(ROOT_color_code[hstack1D_DeltaPhi_map[Form("hstack1D_DeltaPhi_Eta%d_rotated", l_eta_bin)]->GetNhists() % ROOT_color_code.size()]);

                //         hstack1D_DeltaPhi_map[Form("hstack1D_DeltaPhi_Eta%d_rotated", l_eta_bin)] -> Add(pair.second);
                //     }
                //     else {
                //         std::cout<<"wtf_singlebin: "<<pair.first<<std::endl;
                //     }
                // }
                else 
                {
                    std::cout<<"wtf_inclusive_to_Mbin: "<<pair.first<<std::endl;
                }
            }
        }
        // Division:--------------------------------------------------------------------------------------------------------------------------------------------------------------------
        // note : for truth
        if (runnumber == -1 && pair.first.find("h1D") != std::string::npos && pair.first.find("_TrueEta") != std::string::npos && l_Mbin != -1){
            if (SelectedMbin == 100){
                pair.second->SetFillColor(ROOT_color_code[hstack1D_TrueEta_map[Form("hstack1D_TrueEta")]->GetNhists() % ROOT_color_code.size()]);
                hstack1D_TrueEta_map[Form("hstack1D_TrueEta")] -> Add(pair.second);
            }

            else if (SelectedMbin == Semi_inclusive_interval){
                if (l_Mbin > Semi_inclusive_Mbin) {continue;}

                pair.second->SetFillColor(ROOT_color_code[hstack1D_TrueEta_map[Form("hstack1D_TrueEta")]->GetNhists() % ROOT_color_code.size()]);
                hstack1D_TrueEta_map[Form("hstack1D_TrueEta")] -> Add(pair.second);
            }

            else if (SelectedMbin == l_Mbin){

                pair.second->SetFillColor(ROOT_color_code[hstack1D_TrueEta_map[Form("hstack1D_TrueEta")]->GetNhists() % ROOT_color_code.size()]);
                hstack1D_TrueEta_map[Form("hstack1D_TrueEta")] -> Add(pair.second);
            }
            else {
                std::cout<<"wtf_TruthEta_inclusive_to_Mbin: "<<pair.first<<std::endl;
            }
        }

        // Division:--------------------------------------------------------------------------------------------------------------------------------------------------------------------
        if (pair.first.find("h1D") != std::string::npos && pair.first.find("BestPair") != std::string::npos && isTypeA == l_typeA){
            if (
                    SelectedMbin == 100 ||
                    (SelectedMbin == Semi_inclusive_interval && l_Mbin <= Semi_inclusive_Mbin) ||
                    SelectedMbin == l_Mbin
                ){
                if (pair.first.find("ClusPhiSize") != std::string::npos){
                    pair.second->SetFillColor(ROOT_color_code[hstack1D_BestPair_ClusPhiSize->GetNhists() % ROOT_color_code.size()]);
                    hstack1D_BestPair_ClusPhiSize -> Add(pair.second);
                }
                else if (pair.first.find("ClusAdc") != std::string::npos){
                    pair.second->SetFillColor(ROOT_color_code[hstack1D_BestPair_ClusAdc->GetNhists() % ROOT_color_code.size()]);
                    hstack1D_BestPair_ClusAdc -> Add(pair.second);
                }
                else if (pair.first.find("DeltaPhi") != std::string::npos){
                    pair.second->SetFillColor(ROOT_color_code[hstack1D_BestPair_DeltaPhi->GetNhists() % ROOT_color_code.size()]);
                    hstack1D_BestPair_DeltaPhi -> Add(pair.second);
                }
                else if (pair.first.find("DeltaEta") != std::string::npos){
                    pair.second->SetFillColor(ROOT_color_code[hstack1D_BestPair_DeltaEta->GetNhists() % ROOT_color_code.size()]);
                    hstack1D_BestPair_DeltaEta -> Add(pair.second);
                }
            }
        }
    } // note : end of -> for (auto &pair : h1D_input_map)


    // note : for h1D_RotatedBkg_DeltaPhi_Signal_map
    for (auto &pair : hstack1D_DeltaPhi_map)
    {
        std::tie(l_eta_bin, l_vtxz_bin, l_Mbin, l_typeA, l_rotated, l_finebin) = GetHistStringInfo(pair.first);

        if (pair.first.find("hstack1D") != std::string::npos && pair.first.find("_DeltaPhi") != std::string::npos && pair.first.find("_rotated") == std::string::npos && l_eta_bin != -1){

            std::cout<<"----- "<<pair.first<<", eta_bin: "<<l_eta_bin<<", vtxz_bin: "<<l_vtxz_bin<<", Mbin: "<<l_Mbin<<", typeA: "<<l_typeA<<", rotated: "<<l_rotated<<", finebin: "<<l_finebin<<std::endl;


            auto temp_hist = (TH1D*) pair.second -> GetStack() -> Last();
            auto temp_hist_rotate = (TH1D*) hstack1D_DeltaPhi_map[pair.first + "_rotated"] -> GetStack() -> Last();

            double hist_offset_rotate = get_dist_offset(temp_hist_rotate, 15); // todo:

            std::string f1_BkgPolTwo_Fit_map_key  = pair.first + "_rotated" + "_BkgPolTwo_Fit";
            f1_BkgPolTwo_Fit_map[f1_BkgPolTwo_Fit_map_key] -> SetParameters(hist_offset_rotate, 0, 0); // SetParameter(0, hist_offset_rotate);
            temp_hist_rotate -> Fit(f1_BkgPolTwo_Fit_map[f1_BkgPolTwo_Fit_map_key], "N");


            // todo: two ways of getting the signal
            // h1D_RotatedBkg_DeltaPhi_Signal_map[Form("h1D_RotatedBkg_DeltaPhi_Signal_Eta%d", l_eta_bin)] -> Add(temp_hist, temp_hist_rotate, 1, -1);
            
            h1D_RotatedBkg_DeltaPhi_Signal_map[Form("h1D_RotatedBkg_DeltaPhi_Signal_Eta%d", l_eta_bin)] = (TH1D*) temp_hist -> Clone(Form("h1D_RotatedBkg_DeltaPhi_Signal_Eta%d", l_eta_bin));
            TH1D * temp_signal_hist = h1D_RotatedBkg_DeltaPhi_Signal_map[Form("h1D_RotatedBkg_DeltaPhi_Signal_Eta%d", l_eta_bin)];
            for (int hist_i = 1; hist_i <= temp_signal_hist -> GetNbinsX(); hist_i++){
                // temp_signal_hist -> SetBinContent(hist_i, temp_hist -> GetBinContent(hist_i) - f1_BkgPol0_Fit_map[f1_BkgPol0_Fit_map_key] -> GetParameter(0));
                temp_signal_hist -> SetBinContent(hist_i, temp_hist -> GetBinContent(hist_i) - f1_BkgPolTwo_Fit_map[f1_BkgPolTwo_Fit_map_key] -> Eval(temp_hist -> GetBinCenter(hist_i)));
            }


            // h1D_RotatedBkg_DeltaPhi_Signal_map[Form("h1D_RotatedBkg_DeltaPhi_Signal_Eta%d", l_eta_bin)] -> SetLineStyle(2);
            h1D_RotatedBkg_DeltaPhi_Signal_map[Form("h1D_RotatedBkg_DeltaPhi_Signal_Eta%d", l_eta_bin)] -> SetFillColorAlpha(1,0);
            h1D_RotatedBkg_DeltaPhi_Signal_map[Form("h1D_RotatedBkg_DeltaPhi_Signal_Eta%d", l_eta_bin)] -> SetLineColor(8);

        }
    }

    // Division:--------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // note : for good proto tracklet
    for (int Mbin = 0; Mbin < nCentrality_bin; Mbin++){

        if (isTypeA == 0 && SelectedMbin == 100){
            hstack2D_GoodProtoTracklet_map["hstack2D_GoodProtoTracklet_EtaVtxZ"] -> Add(h2D_input_map[Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d",Mbin)]);
            hstack2D_GoodProtoTracklet_map["hstack2D_GoodProtoTracklet_EtaVtxZ_FineBin"] -> Add(h2D_input_map[Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d_FineBin",Mbin)]);
            hstack2D_GoodProtoTracklet_map["hstack2D_GoodProtoTracklet_EtaVtxZ_rotated"] -> Add(h2D_input_map[Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d_rotated",Mbin)]);
        }
        else if (isTypeA == 0 && SelectedMbin == Semi_inclusive_interval){
            if (Mbin > Semi_inclusive_Mbin) {continue;}

            hstack2D_GoodProtoTracklet_map["hstack2D_GoodProtoTracklet_EtaVtxZ"] -> Add(h2D_input_map[Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d",Mbin)]);
            hstack2D_GoodProtoTracklet_map["hstack2D_GoodProtoTracklet_EtaVtxZ_FineBin"] -> Add(h2D_input_map[Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d_FineBin",Mbin)]);
            hstack2D_GoodProtoTracklet_map["hstack2D_GoodProtoTracklet_EtaVtxZ_rotated"] -> Add(h2D_input_map[Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d_rotated",Mbin)]);
        }
        else if (isTypeA == 0 && SelectedMbin == Mbin){
            hstack2D_GoodProtoTracklet_map["hstack2D_GoodProtoTracklet_EtaVtxZ"] -> Add(h2D_input_map[Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d",Mbin)]);
            hstack2D_GoodProtoTracklet_map["hstack2D_GoodProtoTracklet_EtaVtxZ_FineBin"] -> Add(h2D_input_map[Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d_FineBin",Mbin)]);
            hstack2D_GoodProtoTracklet_map["hstack2D_GoodProtoTracklet_EtaVtxZ_rotated"] -> Add(h2D_input_map[Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d_rotated",Mbin)]);
        }
        
        // note : type A
        else if (isTypeA == 1 && SelectedMbin == 100){
            hstack2D_GoodProtoTracklet_map["hstack2D_GoodProtoTracklet_EtaVtxZ"] -> Add(h2D_input_map[Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d",Mbin)]);
            hstack2D_GoodProtoTracklet_map["hstack2D_GoodProtoTracklet_EtaVtxZ_FineBin"] -> Add(h2D_input_map[Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d_FineBin",Mbin)]);
            hstack2D_GoodProtoTracklet_map["hstack2D_GoodProtoTracklet_EtaVtxZ_rotated"] -> Add(h2D_input_map[Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d_rotated",Mbin)]);
        }
        else if (isTypeA == 1 && SelectedMbin == Semi_inclusive_interval){
            if (Mbin > Semi_inclusive_Mbin) {continue;}

            hstack2D_GoodProtoTracklet_map["hstack2D_GoodProtoTracklet_EtaVtxZ"] -> Add(h2D_input_map[Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d",Mbin)]);
            hstack2D_GoodProtoTracklet_map["hstack2D_GoodProtoTracklet_EtaVtxZ_FineBin"] -> Add(h2D_input_map[Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d_FineBin",Mbin)]);
            hstack2D_GoodProtoTracklet_map["hstack2D_GoodProtoTracklet_EtaVtxZ_rotated"] -> Add(h2D_input_map[Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d_rotated",Mbin)]);
        }
        else if (isTypeA == 1 && SelectedMbin == Mbin){
            hstack2D_GoodProtoTracklet_map["hstack2D_GoodProtoTracklet_EtaVtxZ"] -> Add(h2D_input_map[Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d",Mbin)]);
            hstack2D_GoodProtoTracklet_map["hstack2D_GoodProtoTracklet_EtaVtxZ_FineBin"] -> Add(h2D_input_map[Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d_FineBin",Mbin)]);
            hstack2D_GoodProtoTracklet_map["hstack2D_GoodProtoTracklet_EtaVtxZ_rotated"] -> Add(h2D_input_map[Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d_rotated",Mbin)]);
        }

    }


    // Division:--------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // note : for hstack2D_TrueEtaVtxZ_map
    // Form("hstack2D_TrueEtaVtxZ"),
    // Form("hstack2D_TrueEtaVtxZ_FineBin"),
    if (runnumber == -1)
    {
        for (auto &pair : h2D_input_map)
        {
            std::tie(l_eta_bin, l_vtxz_bin, l_Mbin, l_typeA, l_rotated, l_finebin) = GetHistStringInfo(pair.first);

            if (pair.first.find("h2D_TrueEtaVtxZ") != std::string::npos && l_Mbin != -1){
                
                std::cout<<" Truth for TrueEtaVtxZ ----- "<<pair.first<<", eta_bin: "<<l_eta_bin<<", vtxz_bin: "<<l_vtxz_bin<<", Mbin: "<<l_Mbin<<", typeA: "<<l_typeA<<", rotated: "<<l_rotated<<", finebin: "<<l_finebin<<std::endl;

                if (SelectedMbin == 100){
                    if (l_finebin == 0){
                        hstack2D_TrueEtaVtxZ_map["hstack2D_TrueEtaVtxZ"] -> Add(pair.second);
                    }
                    else if (l_finebin == 1){
                        hstack2D_TrueEtaVtxZ_map["hstack2D_TrueEtaVtxZ_FineBin"] -> Add(pair.second);
                    }
                    else {
                        std::cout<<"wtf_TrueEtaVtxZ_inclusive100: "<<pair.first<<std::endl;
                    }
                }
                else if (SelectedMbin == Semi_inclusive_interval){
                    if (l_Mbin > Semi_inclusive_Mbin) {continue;}

                    if (l_finebin == 0){
                        hstack2D_TrueEtaVtxZ_map["hstack2D_TrueEtaVtxZ"] -> Add(pair.second);
                    }
                    else if (l_finebin == 1){
                        hstack2D_TrueEtaVtxZ_map["hstack2D_TrueEtaVtxZ_FineBin"] -> Add(pair.second);
                    }
                    else {
                        std::cout<<"wtf_TrueEtaVtxZ_inclusive"<<Semi_inclusive_interval<<" : "<<pair.first<<std::endl;
                    }
                }
                else if (SelectedMbin == l_Mbin){
                    if (l_finebin == 0){
                        hstack2D_TrueEtaVtxZ_map["hstack2D_TrueEtaVtxZ"] -> Add(pair.second);
                    }
                    else if (l_finebin == 1){
                        hstack2D_TrueEtaVtxZ_map["hstack2D_TrueEtaVtxZ_FineBin"] -> Add(pair.second);
                    }
                    else {
                        std::cout<<"wtf_TrueEtaVtxZ_singlebin: "<<pair.first<<std::endl;
                    }
                }
                else {
                    std::cout<<"wtf_TrueEtaVtxZ_inclusive_to_Mbin: "<<pair.first<<std::endl;
                }

            }
            
            
        }
    }


    // Division : -----------------------------------------------------------------------------------------------------------------------------------------------------------------
    for (int Mbin = 0; Mbin < nCentrality_bin; Mbin++)
    {
        std::string isTypeA_str = (isTypeA == 0) ? "" : "_typeA";

        if (
                SelectedMbin == 100 ||
                (SelectedMbin == Semi_inclusive_interval && Mbin <= Semi_inclusive_Mbin) ||
                SelectedMbin == Mbin
            )
        {
            hstack2D_BestPairEtaVtxZ -> Add(h2D_input_map[Form("h2D%s_BestPairEtaVtxZ_Mbin%d",isTypeA_str.c_str(), Mbin)]);
            hstack2D_BestPairEtaVtxZ_FineBin -> Add(h2D_input_map[Form("h2D%s_BestPairEtaVtxZ_Mbin%d_FineBin",isTypeA_str.c_str(), Mbin)]);
        }
    }

}

void PreparedNdEtaEach::DoFittings()
{
    std::cout<<"In DoFittings()"<<std::endl;
    
    int stack_count = 0;

    int l_eta_bin, l_vtxz_bin, l_Mbin;
    int l_typeA, l_rotated, l_finebin;

    for (auto &pair : hstack1D_DeltaPhi_map)
    {
        std::tie(l_eta_bin, l_vtxz_bin, l_Mbin, l_typeA, l_rotated, l_finebin) = GetHistStringInfo(pair.first);

        if (pair.first.find("hstack1D") != std::string::npos && pair.first.find("_DeltaPhi") != std::string::npos){
            if (stack_count % 20 == 0){
                std::cout<<"Fitting stack : "<<stack_count<<", "<<pair.first<<std::endl;
            }

            auto temp_hist = (TH1D*) pair.second -> GetStack() -> Last();
            
            std::vector<double> N_group_info = find_Ngroup(temp_hist);

            // std::string f1_BkgPol2_Fit_map_key  = pair.first + "_BkgPol2_Fit";
            // std::string f1_BkgPol2_Draw_map_key = pair.first + "_BkgPol2_Draw";

            // if (f1_BkgPol2_Fit_map.find(f1_BkgPol2_Fit_map_key) == f1_BkgPol2_Fit_map.end()){
            //     std::cout<<f1_BkgPol2_Fit_map_key<<" not found in f1_BkgPol2_Fit_map !!"<<std::endl;
            //     exit(1);
            // }

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

            // note : par[0] + par[1]* (x[0]-par[3]) + par[2] * pow((x[0]-par[3]),2);
            // std::cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<std::endl;
            // todo: the parameters
            // f1_BkgPol2_Fit_map[f1_BkgPol2_Fit_map_key] -> SetParameters(hist_offset, 0, 500000, 0, signal_region_l, signal_region_r);
            // f1_BkgPol2_Fit_map[f1_BkgPol2_Fit_map_key] -> FixParameter(4, signal_region_l);
            // f1_BkgPol2_Fit_map[f1_BkgPol2_Fit_map_key] -> FixParameter(5, signal_region_r);
            // // f1_BkgPol2_Fit_map[f1_BkgPol2_Fit_map_key] -> SetParLimits(2, -100000000, 0);

            // // std::cout<<"for "<<f1_BkgPol2_Fit_map_key<<"fit parameters: "<<hist_offset<<", 0, -0.2, 0, "<<signal_region_l<<", "<<signal_region_r<<std::endl;

            // temp_hist -> Fit(f1_BkgPol2_Fit_map[f1_BkgPol2_Fit_map_key], "N");

            // // std::cout<<"for "<<f1_BkgPol2_Fit_map_key<<"fit parameters: "<<f1_BkgPol2_Fit_map[f1_BkgPol2_Fit_map_key] -> GetParameter(0)<<", "<<f1_BkgPol2_Fit_map[f1_BkgPol2_Fit_map_key] -> GetParameter(1)<<", "<<f1_BkgPol2_Fit_map[f1_BkgPol2_Fit_map_key] -> GetParameter(2)<<", "<<f1_BkgPol2_Fit_map[f1_BkgPol2_Fit_map_key] -> GetParameter(3)<<std::endl;

            // // std::cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<std::endl;

            // f1_BkgPol2_Draw_map[f1_BkgPol2_Draw_map_key] -> SetParameters(
            //     f1_BkgPol2_Fit_map[f1_BkgPol2_Fit_map_key] -> GetParameter(0),
            //     f1_BkgPol2_Fit_map[f1_BkgPol2_Fit_map_key] -> GetParameter(1),
            //     f1_BkgPol2_Fit_map[f1_BkgPol2_Fit_map_key] -> GetParameter(2),
            //     f1_BkgPol2_Fit_map[f1_BkgPol2_Fit_map_key] -> GetParameter(3)
            // );

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
                // todo: the parameters
                f1_SigBkgPol2_Fit_map[f1_SigBkgPol2_Fit_map_key] -> SetParameters(
                    gaus_height, 0, gaus_width, 
                    hist_offset, 0, 500000, 0
                );
                // f1_SigBkgPol2_Fit_map[f1_SigBkgPol2_Fit_map_key] -> SetParLimits(5, -100000000, 0);

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

            file_out_DeltaPhi -> cd();
            c1 -> cd();
            if (pair.first.find("_rotated") == std::string::npos){
                auto temp_hist_rotate = (TH1D*) hstack1D_DeltaPhi_map[pair.first + "_rotated"] -> GetStack() -> Last();

                std::string f1_BkgPolTwo_Fit_map_key  = pair.first + "_rotated" + "_BkgPolTwo_Fit";
                
                temp_hist -> SetFillColor(0);
                temp_hist -> SetLineColor(9);
                temp_hist -> SetMinimum(0);
                temp_hist -> SetMaximum(temp_hist -> GetBinContent(temp_hist -> GetMaximumBin()) * 1.5);
                temp_hist_rotate -> SetFillColor(0);
                temp_hist_rotate -> SetLineColor(46);
                
                temp_hist -> Draw("hist");
                temp_hist_rotate -> Draw("ep same");
                h1D_RotatedBkg_DeltaPhi_Signal_map[Form("h1D_RotatedBkg_DeltaPhi_Signal_Eta%d", l_eta_bin)] -> Draw("hist same");
                f1_BkgPolTwo_Fit_map[f1_BkgPolTwo_Fit_map_key] -> Draw("l same");
                c1 -> Write(Form("c1_%s", pair.first.c_str()));

                // f1_BkgPol2_Fit_map[f1_BkgPol2_Fit_map_key] -> Draw("l same");
                // f1_BkgPol2_Draw_map[f1_BkgPol2_Draw_map_key] -> Draw("l same");
                // c1 -> Write(Form("c1_%s", f1_BkgPol2_Draw_map_key.c_str()));
                // c1 -> Write(Form("c1_%s", f1_BkgPolTwo_Fit_map_key.c_str()));
                c1 -> Clear();

                temp_hist -> Draw("hist");
                f1_SigBkgPol2_Fit_map[f1_SigBkgPol2_Fit_map_key] -> Draw("l same");
                f1_SigBkgPol2_DrawSig_map[f1_SigBkgPol2_DrawSig_map_key] -> Draw("l same");
                f1_SigBkgPol2_DrawBkgPol2_map[f1_SigBkgPol2_DrawBkgPol2_map_key] -> Draw("l same");
                c1 -> Write(Form("c1_%s", f1_SigBkgPol2_Fit_map_key.c_str()));
                c1 -> Clear();

                // todo : the range of the Y axis, for checking the bkg fittings.
                temp_hist->SetMinimum(hist_offset * 0.9);
                temp_hist->SetMaximum(hist_offset * 1.1);
                temp_hist -> Draw("hist");
                temp_hist_rotate -> Draw("ep same");
                h1D_RotatedBkg_DeltaPhi_Signal_map[Form("h1D_RotatedBkg_DeltaPhi_Signal_Eta%d", l_eta_bin)] -> Draw("hist same");
                f1_BkgPolTwo_Fit_map[f1_BkgPolTwo_Fit_map_key] -> Draw("l same");
                c1 -> Print(Form("%s/%s", output_directory.c_str(), output_filename_pdf.c_str()));
                c1 -> Clear();
            }


        } // note : end of the selection of the deltaphi distribution 

        else {
            std::cout<<"wtf_DoFittings: "<<pair.first<<std::endl;
        }

        stack_count++;
    }
}

void PreparedNdEtaEach::PrepareMultiplicity()
{
    std::cout<<"In PrepareMultiplicity()"<<std::endl;

    h1D_RotatedBkg_RecoTrackletEta_map[Form("h1D_RotatedBkg_RecoTrackletEta")] -> Sumw2(false);

    for (int eta_bin = 0; eta_bin < nEtaBin; eta_bin++){
        
        // double pol2_bkg_integral = fabs(f1_BkgPol2_Draw_map[Form("hstack1D_DeltaPhi_Eta%d_BkgPol2_Draw", eta_bin)] -> Integral( cut_GoodProtoTracklet_DeltaPhi->first, cut_GoodProtoTracklet_DeltaPhi->second )) / ((DeltaPhiEdge_max - DeltaPhiEdge_min)/double(nDeltaPhiBin));
        double rotated_bkg_count = get_hstack2D_GoodProtoTracklet_count(hstack2D_GoodProtoTracklet_map[Form("hstack2D_GoodProtoTracklet_EtaVtxZ_rotated")], eta_bin);

        // std::cout<<Form("FitBkg: hstack1D_DeltaPhi_Eta%d_BkgPol2_Draw", eta_bin)<<": "<<pol2_bkg_integral<<", rotated_bkg_count: "<<rotated_bkg_count<<std::endl;

        auto temp_h2D = (TH2D*) hstack2D_BestPairEtaVtxZ -> GetStack() -> Last();

        if (false /*ApplyGeoAccCorr && runnumber != -1*/) // note : for data
        {
            SetH2DNonZeroBins(h2D_GeoAccCorr_map[GeoAccCorr_name_map[0]], 1);
        }

        if (false /*ApplyGeoAccCorr*/)
        {
            double GelAcc_BestPair_BinWidthY = h2D_GeoAccCorr_map[GeoAccCorr_name_map[0]] -> GetYaxis() -> GetBinWidth(1);
            double temp_h2D_BinWidthY = temp_h2D -> GetYaxis() -> GetBinWidth(1);
            int Ngroup = temp_h2D_BinWidthY / GelAcc_BestPair_BinWidthY;

            std::cout<<"GelAcc_BestPair_BinWidthY: "<<GelAcc_BestPair_BinWidthY<<", temp_h2D_BinWidthY: "<<temp_h2D_BinWidthY<<", Ngroup: "<<Ngroup<<std::endl;

            h2D_GeoAccCorr_map[GeoAccCorr_name_map[0]] -> Rebin2D(1, Ngroup);
        }

        if (false /*ApplyGeoAccCorr*/){

            if (
                temp_h2D -> GetNbinsX() != h2D_GeoAccCorr_map[GeoAccCorr_name_map[0]] -> GetNbinsX() ||
                temp_h2D -> GetXaxis() -> GetXmin() != h2D_GeoAccCorr_map[GeoAccCorr_name_map[0]] -> GetXaxis() -> GetXmin() ||
                temp_h2D -> GetXaxis() -> GetXmax() != h2D_GeoAccCorr_map[GeoAccCorr_name_map[0]] -> GetXaxis() -> GetXmax() ||

                temp_h2D -> GetNbinsY() != h2D_GeoAccCorr_map[GeoAccCorr_name_map[0]] -> GetNbinsY() ||
                temp_h2D -> GetYaxis() -> GetXmin() != h2D_GeoAccCorr_map[GeoAccCorr_name_map[0]] -> GetYaxis() -> GetXmin() ||
                temp_h2D -> GetYaxis() -> GetXmax() != h2D_GeoAccCorr_map[GeoAccCorr_name_map[0]] -> GetYaxis() -> GetXmax()
            )
            {
                std::cout<<"h2D_BestPairEtaVtxZ and h2D_GeoAccCorr_map[For_BestPair] have different binning!!"<<std::endl;
                std::cout<<"temp_h2D X: "<<temp_h2D -> GetNbinsX()<<", "<<temp_h2D -> GetXaxis() -> GetXmin()<<", "<<temp_h2D -> GetXaxis() -> GetXmax()<<std::endl;
                std::cout<<"temp_h2D Y: "<<temp_h2D -> GetNbinsY()<<", "<<temp_h2D -> GetYaxis() -> GetXmin()<<", "<<temp_h2D -> GetYaxis() -> GetXmax()<<std::endl;

                std::cout<<"h2D_GeoAccCorr_map[For_BestPair] X: "<<h2D_GeoAccCorr_map[GeoAccCorr_name_map[0]] -> GetNbinsX()<<", "<<h2D_GeoAccCorr_map[GeoAccCorr_name_map[0]] -> GetXaxis() -> GetXmin()<<", "<<h2D_GeoAccCorr_map[GeoAccCorr_name_map[0]] -> GetXaxis() -> GetXmax()<<std::endl;
                std::cout<<"h2D_GeoAccCorr_map[For_BestPair] Y: "<<h2D_GeoAccCorr_map[GeoAccCorr_name_map[0]] -> GetNbinsY()<<", "<<h2D_GeoAccCorr_map[GeoAccCorr_name_map[0]] -> GetYaxis() -> GetXmin()<<", "<<h2D_GeoAccCorr_map[GeoAccCorr_name_map[0]] -> GetYaxis() -> GetXmax()<<std::endl;
                exit(1);
            }

            temp_h2D -> Multiply(h2D_GeoAccCorr_map[GeoAccCorr_name_map[0]]);
        }
        
        h1D_BestPair_RecoTrackletEta_map[Form("h1D_BestPair_RecoTrackletEta")] -> SetBinContent(
            eta_bin + 1,
            // get_hstack2D_GoodProtoTracklet_count(hstack2D_BestPairEtaVtxZ, eta_bin)
            get_h2D_GoodProtoTracklet_count(temp_h2D, eta_bin)
        );

        std::pair<int, int> DeltaPhi_signal_bin = get_DeltaPhi_SingleBin(h1D_RotatedBkg_DeltaPhi_Signal_map[Form("h1D_RotatedBkg_DeltaPhi_Signal_Eta%d", eta_bin)], BkgRotated_DeltaPhi_Signal_range);

        // todo : for the test, try to use different way to determine the signal size of this method, we take the full range of th DeltaPhi distribution
        h1D_RotatedBkg_RecoTrackletEta_map[Form("h1D_RotatedBkg_RecoTrackletEta")] -> SetBinContent(
            eta_bin + 1,

            h1D_RotatedBkg_DeltaPhi_Signal_map[Form("h1D_RotatedBkg_DeltaPhi_Signal_Eta%d", eta_bin)] -> Integral(DeltaPhi_signal_bin.first, DeltaPhi_signal_bin.second) // todo: it's a test, bin 51 -> 90 corresponds to -0.02 -> 0.02

            // get_hstack2D_GoodProtoTracklet_count(hstack2D_GoodProtoTracklet_map[Form("hstack2D_GoodProtoTracklet_EtaVtxZ")], eta_bin) - 
            // rotated_bkg_count
        );

        std::cout<<"h1D_RotatedBkg_RecoTrackletEta, bin: "<<eta_bin + 1
        <<", Eta: ["<<h1D_RotatedBkg_RecoTrackletEta_map[Form("h1D_RotatedBkg_RecoTrackletEta")]->GetXaxis()->GetBinLowEdge(eta_bin + 1)<<", "<<h1D_RotatedBkg_RecoTrackletEta_map[Form("h1D_RotatedBkg_RecoTrackletEta")]->GetXaxis()->GetBinUpEdge(eta_bin + 1)<<"], "
        <<"Content&Errors: {"<<h1D_RotatedBkg_RecoTrackletEta_map[Form("h1D_RotatedBkg_RecoTrackletEta")]->GetBinContent(eta_bin + 1)<<", "<<h1D_RotatedBkg_RecoTrackletEta_map[Form("h1D_RotatedBkg_RecoTrackletEta")]->GetBinError(eta_bin + 1)<<"}"<<std::endl;
    }
}

void PreparedNdEtaEach::PreparedNdEtaHist()
{
    std::cout<<"In PreparedNdEtaHist()"<<std::endl;

    if (runnumber == -1){
        h1D_TruedNdEta_map.clear();

        h1D_TruedNdEta_map.insert( 
            std::make_pair(
                "h1D_TruedNdEta",
                (TH1D*)((TH1D*)hstack1D_TrueEta_map["hstack1D_TrueEta"] -> GetStack() -> Last()) -> Clone("h1D_TruedNdEta")
            )
        );
        h1D_TruedNdEta_map["h1D_TruedNdEta"] -> SetName("h1D_TruedNdEta");
        h1D_TruedNdEta_map["h1D_TruedNdEta"] -> SetTitle("h1D_TruedNdEta");
        h1D_TruedNdEta_map["h1D_TruedNdEta"] -> Scale(1. / get_EvtCount(h2D_input_map["h2D_TrueEvtCount_vtxZCentrality"], SelectedMbin));
        h1D_TruedNdEta_map["h1D_TruedNdEta"] -> Scale(1. / double(h1D_TruedNdEta_map["h1D_TruedNdEta"]->GetBinWidth(1)));
        
    } // note : end of truth

    // Division:-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    h1D_BestPair_RecoTrackletEtaPerEvt_map.clear();
    h1D_BestPair_RecoTrackletEtaPerEvt_map.insert( 
        std::make_pair(            
            "h1D_BestPair_RecoTrackletEtaPerEvt",
            (TH1D*)h1D_BestPair_RecoTrackletEta_map["h1D_BestPair_RecoTrackletEta"] -> Clone("h1D_BestPair_RecoTrackletEtaPerEvt")
        )
    );
    h1D_BestPair_RecoTrackletEtaPerEvt_map["h1D_BestPair_RecoTrackletEtaPerEvt"] -> SetName("h1D_BestPair_RecoTrackletEtaPerEvt");
    h1D_BestPair_RecoTrackletEtaPerEvt_map["h1D_BestPair_RecoTrackletEtaPerEvt"] -> SetTitle("h1D_BestPair_RecoTrackletEtaPerEvt");
    h1D_BestPair_RecoTrackletEtaPerEvt_map["h1D_BestPair_RecoTrackletEtaPerEvt"] -> Scale(1. / get_EvtCount(h2D_input_map["h2D_RecoEvtCount_vtxZCentrality"], SelectedMbin));
    h1D_BestPair_RecoTrackletEtaPerEvt_map["h1D_BestPair_RecoTrackletEtaPerEvt"] -> Scale(1. / double(h1D_BestPair_RecoTrackletEtaPerEvt_map["h1D_BestPair_RecoTrackletEtaPerEvt"]->GetBinWidth(1)));



    h1D_RotatedBkg_RecoTrackletEtaPerEvt_map.clear();
    h1D_RotatedBkg_RecoTrackletEtaPerEvt_map.insert( 
        std::make_pair(            
            "h1D_RotatedBkg_RecoTrackletEtaPerEvt",
            (TH1D*)h1D_RotatedBkg_RecoTrackletEta_map["h1D_RotatedBkg_RecoTrackletEta"] -> Clone("h1D_RotatedBkg_RecoTrackletEtaPerEvt")
        )
    );
    h1D_RotatedBkg_RecoTrackletEtaPerEvt_map["h1D_RotatedBkg_RecoTrackletEtaPerEvt"] -> SetName("h1D_RotatedBkg_RecoTrackletEtaPerEvt");
    h1D_RotatedBkg_RecoTrackletEtaPerEvt_map["h1D_RotatedBkg_RecoTrackletEtaPerEvt"] -> SetTitle("h1D_RotatedBkg_RecoTrackletEtaPerEvt");
    h1D_RotatedBkg_RecoTrackletEtaPerEvt_map["h1D_RotatedBkg_RecoTrackletEtaPerEvt"] -> Scale(1. / get_EvtCount(h2D_input_map["h2D_RecoEvtCount_vtxZCentrality"], SelectedMbin));
    h1D_RotatedBkg_RecoTrackletEtaPerEvt_map["h1D_RotatedBkg_RecoTrackletEtaPerEvt"] -> Scale(1. / double(h1D_RotatedBkg_RecoTrackletEtaPerEvt_map["h1D_RotatedBkg_RecoTrackletEtaPerEvt"]->GetBinWidth(1)));

    
    
    // Division:-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    h1D_BestPair_RecoTrackletEtaPerEvtPostAC_map.clear();
    h1D_BestPair_RecoTrackletEtaPerEvtPostAC_map.insert( 
        std::make_pair(            
            "h1D_BestPair_RecoTrackletEtaPerEvtPostAC",
            (TH1D*)h1D_BestPair_RecoTrackletEtaPerEvt_map["h1D_BestPair_RecoTrackletEtaPerEvt"] -> Clone("h1D_BestPair_RecoTrackletEtaPerEvtPostAC")
        )
    );
    h1D_BestPair_RecoTrackletEtaPerEvtPostAC_map["h1D_BestPair_RecoTrackletEtaPerEvtPostAC"]  -> SetName("h1D_BestPair_RecoTrackletEtaPerEvtPostAC");
    h1D_BestPair_RecoTrackletEtaPerEvtPostAC_map["h1D_BestPair_RecoTrackletEtaPerEvtPostAC"] -> SetTitle("h1D_BestPair_RecoTrackletEtaPerEvtPostAC");

    h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map.clear();
    h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map.insert( 
        std::make_pair(            
            "h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC",
            (TH1D*)h1D_RotatedBkg_RecoTrackletEtaPerEvt_map["h1D_RotatedBkg_RecoTrackletEtaPerEvt"] -> Clone("h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC")
        )
    );
    h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map["h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC"]  -> SetName("h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC");
    h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map["h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC"] -> SetTitle("h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC");


    // Division:-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // note : apply the alpha correction from the "h1D_alpha_correction_map_in"

    if (h1D_alpha_correction_map_in.size() != 0 && ApplyAlphaCorr == true)
    {
        // note : BestPair
        h1D_BestPair_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_BestPair_RecoTrackletEtaPerEvtPostAC")] -> Sumw2(true);
        h1D_BestPair_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_BestPair_RecoTrackletEtaPerEvtPostAC")] -> Divide(h1D_BestPair_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_BestPair_RecoTrackletEtaPerEvtPostAC")], h1D_alpha_correction_map_in[alpha_correction_name_map[0]], 1, 1);

        // note : RotatedBkg
        h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC")] -> Sumw2(true);
        h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC")] -> Divide(h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC")], h1D_alpha_correction_map_in[alpha_correction_name_map[1]], 1, 1);


        for (int eta_bin = 0; eta_bin < nEtaBin; eta_bin++)
        {   
            if (
                cut_EtaRange_pair.first == cut_EtaRange_pair.first && 
                ( 
                    h1D_BestPair_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_BestPair_RecoTrackletEtaPerEvtPostAC")] -> GetXaxis() -> GetBinUpEdge(eta_bin + 1) < cut_EtaRange_pair.first || 
                    h1D_BestPair_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_BestPair_RecoTrackletEtaPerEvtPostAC")] -> GetXaxis() -> GetBinLowEdge(eta_bin + 1) > cut_EtaRange_pair.second
                )
            ){
                h1D_BestPair_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_BestPair_RecoTrackletEtaPerEvtPostAC")] -> SetBinContent(eta_bin + 1, 0);
                h1D_BestPair_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_BestPair_RecoTrackletEtaPerEvtPostAC")] -> SetBinError(eta_bin + 1, 0);

                h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC")] -> SetBinContent(eta_bin + 1, 0);
                h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC")] -> SetBinError(eta_bin + 1, 0);

                continue;
            }
        }

    }
    else if (h1D_alpha_correction_map_in.size() == 0 && ApplyAlphaCorr == true){
        std::cout<<"wtf_PreparedNdEtaHist: h1D_alpha_correction_map_in.size() == 0"<<std::endl;
        exit(1);
    }

}

void PreparedNdEtaEach::DeriveAlphaCorrection()
{
    if (runnumber != -1) {
        std::cout<<"wtf_DeriveAlphaCorrection: runnumber != -1"<<std::endl;
        return;
    }

    std::cout<<"In DeriveAlphaCorrection()"<<std::endl;

    h1D_alpha_correction_map_out.insert(
        std::make_pair(
            alpha_correction_name_map[0],
            new TH1D(alpha_correction_name_map[0].c_str(), Form("%s;#eta;#alpha corrections", alpha_correction_name_map[0].c_str()), nEtaBin, EtaEdge_min, EtaEdge_max) 
        )
    );   

    h1D_alpha_correction_map_out.insert(
        std::make_pair(
            alpha_correction_name_map[1],
            new TH1D(alpha_correction_name_map[1].c_str(), Form("%s;#eta;#alpha corrections",alpha_correction_name_map[1].c_str()), nEtaBin, EtaEdge_min, EtaEdge_max)
        )
    );
    

    h1D_alpha_correction_map_out[alpha_correction_name_map[0]] -> Sumw2(true);
    h1D_alpha_correction_map_out[alpha_correction_name_map[1]] -> Sumw2(true);

    h1D_alpha_correction_map_out[alpha_correction_name_map[0]] -> Divide(h1D_BestPair_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_BestPair_RecoTrackletEtaPerEvtPostAC")], h1D_TruedNdEta_map[Form("h1D_TruedNdEta")]);
    h1D_alpha_correction_map_out[alpha_correction_name_map[1]] -> Divide(h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map[Form("h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC")], h1D_TruedNdEta_map[Form("h1D_TruedNdEta")]);
}

void PreparedNdEtaEach::EndRun()
{
    c1 -> Print(Form("%s/%s)", output_directory.c_str(), output_filename_pdf.c_str()));

    std::cout<<111<<std::endl;

    file_out_DeltaPhi -> cd();

    for (auto &pair : hstack1D_DeltaPhi_map)
    {
        pair.second -> Write();
    }

    std::cout<<2222<<std::endl;
    
    for (auto &pair : h1D_RotatedBkg_DeltaPhi_Signal_map){
        pair.second -> Write();
    }
    
    std::cout<<3333<<std::endl;

    file_out_dNdEta -> cd();
    ((TH2D*)hstack2D_GoodProtoTracklet_map[Form("hstack2D_GoodProtoTracklet_EtaVtxZ")]->GetStack()->Last()) -> Write("h2D_GoodProtoTracklet_EtaVtxZ");
    ((TH2D*)hstack2D_GoodProtoTracklet_map[Form("hstack2D_GoodProtoTracklet_EtaVtxZ_FineBin")]->GetStack()->Last()) -> Write("h2D_GoodProtoTracklet_EtaVtxZ_FineBin");
    ((TH2D*)hstack2D_GoodProtoTracklet_map[Form("hstack2D_GoodProtoTracklet_EtaVtxZ_rotated")]->GetStack()->Last()) -> Write("h2D_GoodProtoTracklet_EtaVtxZ_rotated");
    h2D_input_map["h2D_RecoEvtCount_vtxZCentrality"] -> Write();

    std::cout<<4444<<std::endl;

    if (runnumber == -1){
        ((TH2D*)hstack2D_TrueEtaVtxZ_map["hstack2D_TrueEtaVtxZ"]->GetStack()->Last()) -> Write("h2D_TrueEtaVtxZ");
        ((TH2D*)hstack2D_TrueEtaVtxZ_map["hstack2D_TrueEtaVtxZ_FineBin"]->GetStack()->Last()) -> Write("h2D_TrueEtaVtxZ_FineBin");

        h2D_input_map["h2D_TrueEvtCount_vtxZCentrality"] -> Write();
    }

    std::cout<<5555<<std::endl;
    std::cout<<"test: hstack1D_BestPair_ClusPhiSize size(): "<<hstack1D_BestPair_ClusPhiSize->GetNhists()<<std::endl;
    ((TH1D*) hstack1D_BestPair_ClusPhiSize -> GetStack() -> Last()) -> Write("h1D_BestPair_ClusPhiSize");
    std::cout<<55551<<std::endl;
    ((TH1D*) hstack1D_BestPair_ClusAdc -> GetStack() -> Last()) -> Write("h1D_BestPair_ClusAdc");
    std::cout<<55552<<std::endl;
    ((TH1D*) hstack1D_BestPair_DeltaPhi -> GetStack() -> Last()) -> Write("h1D_BestPair_DeltaPhi");
    std::cout<<55553<<std::endl;
    ((TH1D*) hstack1D_BestPair_DeltaEta -> GetStack() -> Last()) -> Write("h1D_BestPair_DeltaEta");
    std::cout<<55554<<std::endl;
    ((TH2D*) hstack2D_BestPairEtaVtxZ -> GetStack() -> Last()) -> Write("h2D_BestPairEtaVtxZ");
    std::cout<<55555<<std::endl;
    ((TH2D*) hstack2D_BestPairEtaVtxZ_FineBin -> GetStack() -> Last()) -> Write("h2D_BestPairEtaVtxZ_FineBin");
    std::cout<<55556<<std::endl;

    std::cout<<6666<<std::endl;

    h1D_BestPair_RecoTrackletEta_map["h1D_BestPair_RecoTrackletEta"] -> Write();
    h1D_BestPair_RecoTrackletEtaPerEvt_map["h1D_BestPair_RecoTrackletEtaPerEvt"] -> Write();
    h1D_BestPair_RecoTrackletEtaPerEvtPostAC_map["h1D_BestPair_RecoTrackletEtaPerEvtPostAC"] -> Write();
    h1D_RotatedBkg_RecoTrackletEta_map["h1D_RotatedBkg_RecoTrackletEta"] -> Write();
    h1D_RotatedBkg_RecoTrackletEtaPerEvt_map["h1D_RotatedBkg_RecoTrackletEtaPerEvt"] -> Write();
    h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map["h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC"] -> Write();

    std::cout<<7777<<std::endl;

    if (ApplyGeoAccCorr && h2D_GeoAccCorr_map.size() != 0){
        h2D_GeoAccCorr_map[GeoAccCorr_name_map[0]]   -> Write("GeoAccMap_For_BestPair");
        h2D_GeoAccCorr_map[GeoAccCorr_name_map[0]] -> Write("GeoAccMap_For_RotatedBkg"); // todo: the map ID
    }


    if (runnumber == -1){
        ((TH1D*)hstack1D_TrueEta_map["hstack1D_TrueEta"]->GetStack()->Last()) -> Write("h1D_TrueEta");
        h1D_TruedNdEta_map["h1D_TruedNdEta"] -> Write();
    }

    std::cout<<8888<<std::endl;

    if (h1D_alpha_correction_map_out.size() != 0){
        h1D_alpha_correction_map_out[alpha_correction_name_map[0]] -> Write();
        h1D_alpha_correction_map_out[alpha_correction_name_map[1]] -> Write();
    }

    std::cout<<9999<<std::endl;

    hstack2D_GoodProtoTracklet_map[Form("hstack2D_GoodProtoTracklet_EtaVtxZ")] -> Write();
    hstack2D_GoodProtoTracklet_map[Form("hstack2D_GoodProtoTracklet_EtaVtxZ_FineBin")] -> Write();
    hstack2D_GoodProtoTracklet_map[Form("hstack2D_GoodProtoTracklet_EtaVtxZ_rotated")] -> Write();
    if (runnumber == -1){
        hstack2D_TrueEtaVtxZ_map["hstack2D_TrueEtaVtxZ"] -> Write();
        hstack2D_TrueEtaVtxZ_map["hstack2D_TrueEtaVtxZ_FineBin"] -> Write();
        hstack1D_TrueEta_map["hstack1D_TrueEta"] -> Write();
    }

    std::cout<<1010101<<std::endl;

    hstack1D_BestPair_ClusPhiSize -> Write();
    hstack1D_BestPair_ClusAdc -> Write();
    hstack1D_BestPair_DeltaPhi -> Write();
    hstack1D_BestPair_DeltaEta -> Write();
    hstack2D_BestPairEtaVtxZ -> Write();
    hstack2D_BestPairEtaVtxZ_FineBin -> Write();
    std::cout<<101011111<<std::endl;

    if (runnumber == -1){
        c1 -> Clear();
        c1 -> cd();

        h1D_TruedNdEta_map["h1D_TruedNdEta"] -> SetFillColorAlpha(0,0);
        h1D_TruedNdEta_map["h1D_TruedNdEta"] -> SetMinimum(0.);
        h1D_TruedNdEta_map["h1D_TruedNdEta"] -> Draw("hist");
        h1D_BestPair_RecoTrackletEtaPerEvtPostAC_map["h1D_BestPair_RecoTrackletEtaPerEvtPostAC"]->SetLineColor(2);
        h1D_BestPair_RecoTrackletEtaPerEvtPostAC_map["h1D_BestPair_RecoTrackletEtaPerEvtPostAC"]->Draw("ep same");
        h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map["h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC"]->SetLineColor(8);
        h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC_map["h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC"]->Draw("ep same");

        c1 -> Write("c1_comp_h1D_dNdEta");
        c1 -> Clear();
    }
    
    std::cout<<10102222<<std::endl;

    file_out_dNdEta -> Close();

    file_out_DeltaPhi -> Close();
}

std::vector<double> PreparedNdEtaEach::find_Ngroup(TH1D * hist_in)
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

double PreparedNdEtaEach::get_dist_offset(TH1D * hist_in, int check_N_bin) // note : check_N_bin 1 to N bins of hist
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

double PreparedNdEtaEach::get_hstack2D_GoodProtoTracklet_count(THStack * stack_in, int eta_bin_in)
{
    auto temp_hist = (TH2D*) stack_in -> GetStack() -> Last();

    double count = 0;
    
    // note : hist
    // note : X: eta
    // note : Y: VtxZ

    for (int yi = 1; yi <= temp_hist->GetNbinsY(); yi++){ // note : vtxZ bin
            
        if (vtxZ_index_map.find(yi - 1) == vtxZ_index_map.end()){
            continue;
        }

        count += temp_hist -> GetBinContent(eta_bin_in + 1, yi);
    }

    return count;

}

double PreparedNdEtaEach::get_h2D_GoodProtoTracklet_count(TH2D * h2D_in, int eta_bin_in)
{
    double count = 0;
    
    // note : hist
    // note : X: eta
    // note : Y: VtxZ

    for (int yi = 1; yi <= h2D_in->GetNbinsY(); yi++){ // note : vtxZ bin
            
        if (vtxZ_index_map.find(yi - 1) == vtxZ_index_map.end()){
            continue;
        }

        count += h2D_in -> GetBinContent(eta_bin_in + 1, yi);
    }

    return count;   
}

double PreparedNdEtaEach::get_EvtCount(TH2D * hist_in, int centrality_bin_in)
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
    // todo: 
    else if (centrality_bin_in == Semi_inclusive_interval){
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

void PreparedNdEtaEach::Convert_to_PerEvt(TH1D * hist_in, double Nevent)
{
    for (int i = 1; i <= hist_in->GetNbinsX(); i++){
        hist_in -> SetBinContent(i, hist_in -> GetBinContent(i) / Nevent);
        hist_in -> SetBinError(i, hist_in -> GetBinError(i) / Nevent);
    }

}

std::pair<int,int> PreparedNdEtaEach::get_DeltaPhi_SingleBin(TH1D * hist_in, std::pair<double, double> range_in)
{
    int bin_min = hist_in -> FindBin(range_in.first);
    
    int bin_max = hist_in -> FindBin(range_in.second);
    bin_max = (fabs(hist_in -> GetXaxis() -> GetBinLowEdge(bin_max) - range_in.second) < 1e-9) ? bin_max - 1 : bin_max;

    std::cout<<std::endl;
    std::cout<<"Input range : "<<range_in.first<<", "<<range_in.second<<std::endl;
    std::cout<<"Final selected bins: "<<bin_min<<", "<<bin_max<<std::endl;
    std::cout<<"Output range : "<<hist_in -> GetXaxis() -> GetBinLowEdge(bin_min)<<", "<<hist_in -> GetXaxis() -> GetBinUpEdge(bin_max)<<std::endl;

    return std::make_pair(bin_min, bin_max);
}

void PreparedNdEtaEach::SetH2DNonZeroBins(TH2D* hist_in, double value_in)
{
    for (int xi = 1; xi <= hist_in->GetNbinsX(); xi++){
        for (int yi = 1; yi <= hist_in->GetNbinsY(); yi++){
            if (hist_in -> GetBinContent(xi, yi) != 0){
                hist_in -> SetBinContent(xi, yi, value_in);
            }
        }
    }
}