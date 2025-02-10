#include "PreparedNdEtaClusEach.h"

PreparedNdEtaClusEach::PreparedNdEtaClusEach(
    int process_id_in,
    int runnumber_in,
    std::string input_directory_in,
    std::string input_file_name_in,
    std::string output_directory_in,
    std::string output_file_name_suffix_in,

    bool ApplyAlphaCorr_in,
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
    isTypeA(isTypeA_in),
    cut_INTTvtxZ(cut_INTTvtxZ_in),
    SelectedMbin(SelectedMbin_in)
{
    PrepareInputRootFie();
    
    if (SelectedMbin >= nCentrality_bin && SelectedMbin != Semi_inclusive_Mbin*10 && SelectedMbin != 100){
        std::cout << "Error : SelectedMbin is out of range" << std::endl;
        std::cout << "SelectedMbin : " << SelectedMbin <<", but : nCentrality_bin: "<< nCentrality_bin << std::endl;
        exit(1);
    }

    vtxZ_index_map = GetVtxZIndexMap();

    PrepareOutPutFileName();

    PrepareOutPutRootFile();

    PrepareHist();

    c1 = new TCanvas("c1", "c1", 950, 800);

    h1D_alpha_correction_map_in.clear();
    h1D_alpha_correction_map_out.clear();

}

void PreparedNdEtaClusEach::PrepareInputRootFie()
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
    std::cout<<"------------------------------------------------------------"<<std::endl;
}

std::map<int, int> PreparedNdEtaClusEach::GetVtxZIndexMap()
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

void PreparedNdEtaClusEach::PrepareOutPutFileName()
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

    output_filename = "PreparedNdEtaClus";
    output_filename = (runnumber != -1) ? "Data_" + output_filename : "MC_" + output_filename;

    output_filename += (ApplyAlphaCorr) ? "_ApplyAlphaCorr" : "";
    output_filename += (isTypeA) ? "_TypeA" : "_AllSensor";
    output_filename += Form("_VtxZ%.0f", fabs(cut_INTTvtxZ.first - cut_INTTvtxZ.second)/2.); // todo : check this
    output_filename += Form("_Mbin%d",SelectedMbin);
    
    output_filename += output_file_name_suffix;
    
    output_filename += (runnumber != -1) ? Form("_%s_%s.root",runnumber_str.c_str(),job_index.c_str()) : Form("_%s.root",job_index.c_str());    
}

void PreparedNdEtaClusEach::PrepareOutPutRootFile()
{
    file_out = new TFile(Form("%s/%s",output_directory.c_str(),output_filename.c_str()), "RECREATE");
}

void PreparedNdEtaClusEach::PrepareHist()
{
    hstack2D_NClusEtaVtxZ_map.clear();

    hstack2D_NClusEtaVtxZ_map.insert( std::make_pair(
            Form("hstack2D_inner_NClusEtaVtxZ"),
            new THStack(Form("hstack2D_inner_NClusEtaVtxZ"), Form("hstack2D_inner_NClusEtaVtxZ;Cluster #eta (inner);INTT vtxZ [cm]")) 
        )
    );

    // note : normal fine bin
    hstack2D_NClusEtaVtxZ_map.insert( std::make_pair(
            Form("hstack2D_inner_NClusEtaVtxZ_FineBin"),
            new THStack(Form("hstack2D_inner_NClusEtaVtxZ_FineBin"), Form("hstack2D_inner_NClusEtaVtxZ_FineBin;Cluster #eta (inner);INTT vtxZ [cm]")) 
        )
    ); 

    hstack2D_NClusEtaVtxZ_map.insert( std::make_pair(
            Form("hstack2D_outer_NClusEtaVtxZ"),
            new THStack(Form("hstack2D_outer_NClusEtaVtxZ"), Form("hstack2D_outer_NClusEtaVtxZ;Cluster #eta (outer);INTT vtxZ [cm]")) 
        )
    );

    // note : normal fine bin
    hstack2D_NClusEtaVtxZ_map.insert( std::make_pair(
            Form("hstack2D_outer_NClusEtaVtxZ_FineBin"),
            new THStack(Form("hstack2D_outer_NClusEtaVtxZ_FineBin"), Form("hstack2D_outer_NClusEtaVtxZ_FineBin;Cluster #eta (outer);INTT vtxZ [cm]")) 
        )
    ); 

    // if (isTypeA){
    //     std::cout<<111<<std::endl;
    //     for (auto &pair : hstack2D_NClusEtaVtxZ_map){
    //         std::string X_title = pair.second->GetXaxis()->GetTitle();
    //         X_title = X_title.substr(0, X_title.find_last_of(")"));
    //         X_title += ", TypeA)";

    //         pair.second->GetXaxis()->SetTitle(X_title.c_str());
    //     }
    //     std::cout<<222<<std::endl;
    // }

    // Division:-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    hstack1D_NClusEta_map.clear();

    hstack1D_NClusEta_map.insert(
        std::make_pair(
            "hstack1D_inner_NClusEta",
            new THStack("hstack1D_inner_NClusEta", "hstack1D_inner_NClusEta;Cluster #eta (inner);Entries")
        )
    );

    hstack1D_NClusEta_map.insert(
        std::make_pair(
            "hstack1D_outer_NClusEta",
            new THStack("hstack1D_outer_NClusEta", "hstack1D_outer_NClusEta;Cluster #eta (outer);Entries")
        )
    );

    // if (isTypeA){
    //     for (auto &pair : hstack1D_NClusEta_map){
    //         std::string X_title = pair.second->GetXaxis()->GetTitle();
    //         X_title = X_title.substr(0, X_title.find_last_of(")"));
    //         X_title += ", TypeA)";

    //         pair.second->GetXaxis()->SetTitle(X_title.c_str());
    //     }
    // }

    // Division:-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    if (runnumber == -1){
        hstack2D_TrueEtaVtxZ_map.clear();

        // note : centrality inclusive 
        hstack2D_TrueEtaVtxZ_map.insert( std::make_pair(
                Form("hstack2D_TrueEtaVtxZ"),
                new THStack(Form("hstack2D_TrueEtaVtxZ"), Form("hstack2D_TrueEtaVtxZ;PHG4Particle #eta;TruthPV_trig_z [cm]"))
            )
        );

        hstack2D_TrueEtaVtxZ_map.insert( std::make_pair(
                Form("hstack2D_TrueEtaVtxZ_FineBin"),
                new THStack(Form("hstack2D_TrueEtaVtxZ_FineBin"), Form("hstack2D_TrueEtaVtxZ_FineBin;PHG4Particle #eta;TruthPV_trig_z [cm]"))
            )
        );
    }

    // Division:-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    if (runnumber == -1){
        hstack1D_TrueEta_map.clear();

        hstack1D_TrueEta_map.insert( std::make_pair(
            Form("hstack1D_TrueEta"),
            new THStack(Form("hstack1D_TrueEta"), Form("hstack1D_TrueEta;PHG4Particle #eta;Entries"))
        )
        );

    }

}

// note : return the bin index of vtxZ, Mbin, typeA, inner, finebin
std::tuple<int, int, int, int, int> PreparedNdEtaClusEach::GetHistStringInfo(std::string hist_name)
{

    // std::cout<<"In GetHistStringInfo(), input name: "<<hist_name<<std::endl;
    
    int vtxz_bin;
    int Mbin;

    std::string vtxz_bin_str = "";
    std::string Mbin_str = "";    

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

    // note : typeA, inner, finebin
    int typeA = (hist_name.find("_typeA") != std::string::npos) ? 1 : 0;
    int isInner = (hist_name.find("_inner") != std::string::npos) ? 1 : 0;
    int finebin = (hist_name.find("_FineBin") != std::string::npos) ? 1 : 0;


    return std::make_tuple(vtxz_bin, Mbin, typeA, isInner, finebin);
}

void PreparedNdEtaClusEach::PrepareStacks()
{
    std::cout<<"In PrepareStacks()"<<std::endl;

    int l_vtxz_bin, l_Mbin;
    int l_typeA, l_inner, l_finebin;

    for (auto &pair : h1D_input_map)
    {
        std::tie(l_vtxz_bin, l_Mbin, l_typeA, l_inner, l_finebin) = GetHistStringInfo(pair.first);

        // note : the vtxz_bin of the hist is not in the interest
        if (vtxZ_index_map.find(l_vtxz_bin) == vtxZ_index_map.end()){
            continue;
        }

        if (pair.first.find("h1D") != std::string::npos && pair.first.find("_NClusEta") != std::string::npos && l_Mbin != -1){
            std::cout<<"---in hstack1D--- "<<pair.first<<", vtxz_bin: "<<l_vtxz_bin<<", Mbin: "<<l_Mbin<<", typeA: "<<l_typeA<<", inner: "<<l_inner<<", finebin: "<<l_finebin<<std::endl;
            std::string inner_outer_str = (l_inner) ? "inner" : "outer";


            if (isTypeA == l_typeA){ // note : {isTypeA == 0 -> inclusive, l_typeA should be zero}, {isTypeA == 1 -> typeA, l_typeA should be 1}
                if (SelectedMbin == 100) // note : inclusive centrality
                {
                    pair.second->SetFillColor(ROOT_color_code[hstack1D_NClusEta_map[Form("hstack1D_%s_NClusEta",inner_outer_str.c_str())]->GetNhists() % ROOT_color_code.size()]);
                    hstack1D_NClusEta_map[Form("hstack1D_%s_NClusEta",inner_outer_str.c_str())]->Add(pair.second);
                }
                // todo: need to check if the centrality bin is changed
                else if (SelectedMbin == Semi_inclusive_Mbin * 10){
                    if (l_Mbin > Semi_inclusive_Mbin) {continue;}
                    pair.second->SetFillColor(ROOT_color_code[hstack1D_NClusEta_map[Form("hstack1D_%s_NClusEta",inner_outer_str.c_str())]->GetNhists() % ROOT_color_code.size()]);
                    hstack1D_NClusEta_map[Form("hstack1D_%s_NClusEta",inner_outer_str.c_str())]->Add(pair.second);
                }
                else if (SelectedMbin == l_Mbin)
                {
                    pair.second->SetFillColor(ROOT_color_code[hstack1D_NClusEta_map[Form("hstack1D_%s_NClusEta",inner_outer_str.c_str())]->GetNhists() % ROOT_color_code.size()]);
                    hstack1D_NClusEta_map[Form("hstack1D_%s_NClusEta",inner_outer_str.c_str())]->Add(pair.second);
                }
                else 
                {
                    std::cout<<"wtf the Mbin: "<<pair.first<<std::endl;   
                }
            }
        }

        if (pair.first.find("h1D") != std::string::npos && pair.first.find("_TrueEta") != std::string::npos && l_Mbin != -1){
            
            if (SelectedMbin == 100){
                pair.second->SetFillColor(ROOT_color_code[hstack1D_TrueEta_map[Form("hstack1D_TrueEta")]->GetNhists() % ROOT_color_code.size()]);
                hstack1D_TrueEta_map[Form("hstack1D_TrueEta")] -> Add(pair.second);
            }

            else if (SelectedMbin == Semi_inclusive_Mbin * 10){
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
    }

    // Division:--------------------------------------------------------------------------------------------------------------------------------------------------------------------

    for (int Mbin = 0; Mbin < nCentrality_bin; Mbin++)
    {
        std::string isTypeA_str = (isTypeA) ? "_typeA" : "";

        if (SelectedMbin == 100)
        {
            hstack2D_NClusEtaVtxZ_map[Form("hstack2D_inner_NClusEtaVtxZ")] -> Add(h2D_input_map[Form("h2D%s_inner_NClusEta_Mbin%d", isTypeA_str.c_str(), Mbin)]);
            hstack2D_NClusEtaVtxZ_map[Form("hstack2D_outer_NClusEtaVtxZ")] -> Add(h2D_input_map[Form("h2D%s_outer_NClusEta_Mbin%d", isTypeA_str.c_str(), Mbin)]);
            hstack2D_NClusEtaVtxZ_map[Form("hstack2D_inner_NClusEtaVtxZ_FineBin")] -> Add(h2D_input_map[Form("h2D%s_inner_NClusEta_Mbin%d_FineBin", isTypeA_str.c_str(), Mbin)]);
            hstack2D_NClusEtaVtxZ_map[Form("hstack2D_outer_NClusEtaVtxZ_FineBin")] -> Add(h2D_input_map[Form("h2D%s_outer_NClusEta_Mbin%d_FineBin", isTypeA_str.c_str(), Mbin)]);
        }
        else if (SelectedMbin == Semi_inclusive_Mbin * 10){
            if (Mbin > Semi_inclusive_Mbin) {continue;}

            hstack2D_NClusEtaVtxZ_map[Form("hstack2D_inner_NClusEtaVtxZ")] -> Add(h2D_input_map[Form("h2D%s_inner_NClusEta_Mbin%d", isTypeA_str.c_str(), Mbin)]);
            hstack2D_NClusEtaVtxZ_map[Form("hstack2D_outer_NClusEtaVtxZ")] -> Add(h2D_input_map[Form("h2D%s_outer_NClusEta_Mbin%d", isTypeA_str.c_str(), Mbin)]);
            hstack2D_NClusEtaVtxZ_map[Form("hstack2D_inner_NClusEtaVtxZ_FineBin")] -> Add(h2D_input_map[Form("h2D%s_inner_NClusEta_Mbin%d_FineBin", isTypeA_str.c_str(), Mbin)]);
            hstack2D_NClusEtaVtxZ_map[Form("hstack2D_outer_NClusEtaVtxZ_FineBin")] -> Add(h2D_input_map[Form("h2D%s_outer_NClusEta_Mbin%d_FineBin", isTypeA_str.c_str(), Mbin)]);
        }
        else if (SelectedMbin == Mbin)
        {
            hstack2D_NClusEtaVtxZ_map[Form("hstack2D_inner_NClusEtaVtxZ")] -> Add(h2D_input_map[Form("h2D%s_inner_NClusEta_Mbin%d", isTypeA_str.c_str(), Mbin)]);
            hstack2D_NClusEtaVtxZ_map[Form("hstack2D_outer_NClusEtaVtxZ")] -> Add(h2D_input_map[Form("h2D%s_outer_NClusEta_Mbin%d", isTypeA_str.c_str(), Mbin)]);
            hstack2D_NClusEtaVtxZ_map[Form("hstack2D_inner_NClusEtaVtxZ_FineBin")] -> Add(h2D_input_map[Form("h2D%s_inner_NClusEta_Mbin%d_FineBin", isTypeA_str.c_str(), Mbin)]);
            hstack2D_NClusEtaVtxZ_map[Form("hstack2D_outer_NClusEtaVtxZ_FineBin")] -> Add(h2D_input_map[Form("h2D%s_outer_NClusEta_Mbin%d_FineBin", isTypeA_str.c_str(), Mbin)]);
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
            std::tie(l_vtxz_bin, l_Mbin, l_typeA, l_inner, l_finebin) = GetHistStringInfo(pair.first);

            if (pair.first.find("h2D_TrueEtaVtxZ") != std::string::npos && l_Mbin != -1){
                
                std::cout<<" Truth for TrueEtaVtxZ ----- "<<pair.first<<", vtxz_bin: "<<l_vtxz_bin<<", Mbin: "<<l_Mbin<<", typeA: "<<l_typeA<<", inner: "<<l_inner<<", finebin: "<<l_finebin<<std::endl;

                std::string finebin_str = (l_finebin) ? "_FineBin" : "";

                if (SelectedMbin == 100){
                    hstack2D_TrueEtaVtxZ_map[Form("hstack2D_TrueEtaVtxZ%s",finebin_str.c_str())] -> Add(pair.second);
                }
                else if (SelectedMbin == Semi_inclusive_Mbin * 10){
                    if (l_Mbin > Semi_inclusive_Mbin) {continue;}

                    hstack2D_TrueEtaVtxZ_map[Form("hstack2D_TrueEtaVtxZ%s",finebin_str.c_str())] -> Add(pair.second);
                }
                else if (SelectedMbin == l_Mbin){
                    hstack2D_TrueEtaVtxZ_map[Form("hstack2D_TrueEtaVtxZ%s",finebin_str.c_str())] -> Add(pair.second);
                }
                else {
                    std::cout<<"wtf_TrueEtaVtxZ_inclusive_to_Mbin: "<<pair.first<<std::endl;
                }

            }
            
            
        }
    }

    h1D_inner_NClusEta = (TH1D*)((TH1D*) hstack1D_NClusEta_map["hstack1D_inner_NClusEta"] -> GetStack() -> Last())->Clone("h1D_inner_NClusEta");
    h1D_outer_NClusEta = (TH1D*)((TH1D*) hstack1D_NClusEta_map["hstack1D_outer_NClusEta"] -> GetStack() -> Last())->Clone("h1D_outer_NClusEta");

    h1D_inner_NClusEta -> SetFillColorAlpha(2,0);
    h1D_outer_NClusEta -> SetFillColorAlpha(2,0);
}

void PreparedNdEtaClusEach::PreparedNdEtaHist()
{
    if (runnumber == -1){
        h1D_TruedNdEta_map.clear();

        h1D_TruedNdEta_map.insert( 
            std::make_pair(
                "h1D_TruedNdEta",
                (TH1D*)((TH1D*)hstack1D_TrueEta_map["hstack1D_TrueEta"] -> GetStack() -> Last()) -> Clone("h1D_TruedNdEta")
            )
        );
        h1D_TruedNdEta_map["h1D_TruedNdEta"] -> SetFillColorAlpha(3,0);
        h1D_TruedNdEta_map["h1D_TruedNdEta"] -> SetName("h1D_TruedNdEta");
        h1D_TruedNdEta_map["h1D_TruedNdEta"] -> SetTitle("h1D_TruedNdEta");
        h1D_TruedNdEta_map["h1D_TruedNdEta"] -> Scale(1. / get_EvtCount(h2D_input_map["h2D_TrueEvtCount_vtxZCentrality"], SelectedMbin));
        h1D_TruedNdEta_map["h1D_TruedNdEta"] -> Scale(1. / double(h1D_TruedNdEta_map["h1D_TruedNdEta"]->GetBinWidth(1)));
        
    } // note : end of truth

    // Division:-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


    h1D_inner_NClusEtaPerEvt = (TH1D*) h1D_inner_NClusEta -> Clone("h1D_inner_NClusEtaPerEvt");
    h1D_inner_NClusEtaPerEvt -> SetName("h1D_inner_NClusEtaPerEvt");
    h1D_inner_NClusEtaPerEvt -> SetTitle("h1D_inner_NClusEtaPerEvt");
    h1D_inner_NClusEtaPerEvt -> Scale(1. / get_EvtCount(h2D_input_map["h2D_RecoEvtCount_vtxZCentrality"], SelectedMbin));
    h1D_inner_NClusEtaPerEvt -> Scale(1. / double(h1D_inner_NClusEtaPerEvt->GetBinWidth(1)));


    h1D_outer_NClusEtaPerEvt = (TH1D*) h1D_outer_NClusEta -> Clone("h1D_outer_NClusEtaPerEvt");
    h1D_outer_NClusEtaPerEvt -> SetName("h1D_outer_NClusEtaPerEvt");
    h1D_outer_NClusEtaPerEvt -> SetTitle("h1D_outer_NClusEtaPerEvt");
    h1D_outer_NClusEtaPerEvt -> Scale(1. / get_EvtCount(h2D_input_map["h2D_RecoEvtCount_vtxZCentrality"], SelectedMbin));
    h1D_outer_NClusEtaPerEvt -> Scale(1. / double(h1D_outer_NClusEtaPerEvt->GetBinWidth(1)));

    // Division:-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    h1D_inner_NClusEtaPerEvtPostAC = (TH1D*) h1D_inner_NClusEtaPerEvt -> Clone("h1D_inner_NClusEtaPerEvtPostAC");
    h1D_inner_NClusEtaPerEvtPostAC -> SetName("h1D_inner_NClusEtaPerEvtPostAC");
    h1D_inner_NClusEtaPerEvtPostAC -> SetTitle("h1D_inner_NClusEtaPerEvtPostAC");
    

    h1D_outer_NClusEtaPerEvtPostAC = (TH1D*) h1D_outer_NClusEtaPerEvt -> Clone("h1D_outer_NClusEtaPerEvtPostAC");
    h1D_outer_NClusEtaPerEvtPostAC -> SetName("h1D_outer_NClusEtaPerEvtPostAC");
    h1D_outer_NClusEtaPerEvtPostAC -> SetTitle("h1D_outer_NClusEtaPerEvtPostAC");

    // Division:-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    if (h1D_alpha_correction_map_in.size() != 0 && ApplyAlphaCorr == true)
    {
        h1D_inner_NClusEtaPerEvtPostAC -> Sumw2(true);
        h1D_inner_NClusEtaPerEvtPostAC -> Divide(h1D_alpha_correction_map_in[alpha_correction_name_map[0]]);

        h1D_outer_NClusEtaPerEvtPostAC -> Sumw2(true);
        h1D_outer_NClusEtaPerEvtPostAC -> Divide(h1D_alpha_correction_map_in[alpha_correction_name_map[1]]);

        if (cut_EtaRange_pair.first == cut_EtaRange_pair.first){
            for (int eta_bin = 0; eta_bin < h1D_inner_NClusEtaPerEvtPostAC -> GetNbinsX(); eta_bin++)
            {
                if ( 
                    h1D_inner_NClusEtaPerEvtPostAC -> GetXaxis() -> GetBinUpEdge(eta_bin + 1) < cut_EtaRange_pair.first || 
                    h1D_inner_NClusEtaPerEvtPostAC -> GetXaxis() -> GetBinLowEdge(eta_bin + 1) > cut_EtaRange_pair.second
                ){
                    h1D_inner_NClusEtaPerEvtPostAC -> SetBinContent(eta_bin + 1, 0);
                    h1D_inner_NClusEtaPerEvtPostAC -> SetBinError(eta_bin + 1, 0);

                    h1D_outer_NClusEtaPerEvtPostAC -> SetBinContent(eta_bin + 1, 0);
                    h1D_outer_NClusEtaPerEvtPostAC -> SetBinError(eta_bin + 1, 0);
                }
            }
        }


    }
    else if (h1D_alpha_correction_map_in.size() == 0 && ApplyAlphaCorr == true){
        std::cout<<"wtf_PreparedNdEtaHist: h1D_alpha_correction_map_in.size() == 0"<<std::endl;
        exit(1);
    }

}

void PreparedNdEtaClusEach::DeriveAlphaCorrection()
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

    h1D_alpha_correction_map_out[alpha_correction_name_map[0]] -> Divide(h1D_inner_NClusEtaPerEvtPostAC, h1D_TruedNdEta_map[Form("h1D_TruedNdEta")]);
    h1D_alpha_correction_map_out[alpha_correction_name_map[1]] -> Divide(h1D_outer_NClusEtaPerEvtPostAC, h1D_TruedNdEta_map[Form("h1D_TruedNdEta")]);
}

void PreparedNdEtaClusEach::EndRun()
{
    file_out -> cd();

    ((TH2D*) hstack2D_NClusEtaVtxZ_map["hstack2D_inner_NClusEtaVtxZ"]->GetStack()->Last()) -> Write("h2D_inner_NClusEtaVtxZ");
    ((TH2D*) hstack2D_NClusEtaVtxZ_map["hstack2D_inner_NClusEtaVtxZ_FineBin"]->GetStack()->Last()) -> Write("h2D_inner_NClusEtaVtxZ_FineBin");
    ((TH2D*) hstack2D_NClusEtaVtxZ_map["hstack2D_outer_NClusEtaVtxZ"]->GetStack()->Last()) -> Write("h2D_outer_NClusEtaVtxZ");
    ((TH2D*) hstack2D_NClusEtaVtxZ_map["hstack2D_outer_NClusEtaVtxZ_FineBin"]->GetStack()->Last()) -> Write("h2D_outer_NClusEtaVtxZ_FineBin");
    h2D_input_map["h2D_RecoEvtCount_vtxZCentrality"] -> Write();

    if (runnumber == -1){
        h2D_input_map["h2D_TrueEvtCount_vtxZCentrality"] -> Write();

        ((TH2D*)hstack2D_TrueEtaVtxZ_map["hstack2D_TrueEtaVtxZ"]->GetStack()->Last()) -> Write("h2D_TrueEtaVtxZ");
        ((TH2D*)hstack2D_TrueEtaVtxZ_map["hstack2D_TrueEtaVtxZ_FineBin"]->GetStack()->Last()) -> Write("h2D_TrueEtaVtxZ_FineBin");

        
        ((TH1D*) hstack1D_TrueEta_map["hstack1D_TrueEta"]->GetStack()->Last()) -> Write("h1D_TrueEta");
        h1D_TruedNdEta_map["h1D_TruedNdEta"] -> Write();
    }

    h1D_inner_NClusEta -> Write();
    h1D_inner_NClusEtaPerEvt -> Write();
    h1D_inner_NClusEtaPerEvtPostAC -> Write();
    
    h1D_outer_NClusEta -> Write();
    h1D_outer_NClusEtaPerEvt -> Write();
    h1D_outer_NClusEtaPerEvtPostAC -> Write();


    for (auto &pair : h1D_alpha_correction_map_out){
        pair.second -> Write();
    }

    if (h1D_alpha_correction_map_in.size() != 0){
        for (auto &pair : h1D_alpha_correction_map_in){
            pair.second -> Write(("Used_" + pair.first).c_str());
        }
    }



    for (auto &pair : hstack2D_NClusEtaVtxZ_map){
        pair.second -> Write();
    }
    for (auto &pair : hstack1D_NClusEta_map){
        pair.second -> Write();
    }

    if (runnumber == -1){
        for (auto &pair : hstack2D_TrueEtaVtxZ_map){
            pair.second -> Write();
        }
        for (auto &pair : hstack1D_TrueEta_map){
            pair.second -> Write();
        }
    }

    if (runnumber == -1){
        c1 -> Clear();
        c1 -> cd();

        h1D_TruedNdEta_map["h1D_TruedNdEta"] -> SetFillColorAlpha(0,0);
        h1D_TruedNdEta_map["h1D_TruedNdEta"] -> SetMinimum(0.);
        h1D_TruedNdEta_map["h1D_TruedNdEta"] -> Draw("hist");
        h1D_inner_NClusEtaPerEvtPostAC->SetLineColor(2);
        h1D_inner_NClusEtaPerEvtPostAC->Draw("ep same");
        h1D_outer_NClusEtaPerEvtPostAC->SetLineColor(8);
        h1D_outer_NClusEtaPerEvtPostAC->Draw("ep same");

        c1 -> Write("c1_comp_h1D_dNdEta");
        c1 -> Clear();
    }

}


double PreparedNdEtaClusEach::get_EvtCount(TH2D * hist_in, int centrality_bin_in)
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
    else if (centrality_bin_in == Semi_inclusive_Mbin * 10){
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