#include "GeoAccepCorr.h"

GeoAccepCorr::GeoAccepCorr(
    int process_id_in, 
    int runnumber_in,
    std::string input_directory_in,
    std::string input_file_name_in,
    std::string output_directory_in,
    std::string output_file_name_suffix_in,

    int setMbin_in
) : process_id(process_id_in),
    runnumber(runnumber_in),
    input_directory(input_directory_in),
    input_file_name(input_file_name_in),
    output_directory(output_directory_in),
    output_file_name_suffix(output_file_name_suffix_in),
    setMbin(setMbin_in)
{
    PrepareInputRootFie();

    if (setMbin >= nCentrality_bin && setMbin != Semi_inclusive_bin*10 && setMbin != 100){
        std::cout << "Error : setMbin is out of range" << std::endl;
        std::cout << "setMbin : " << setMbin <<", but : nCentrality_bin: "<< nCentrality_bin << std::endl;
        exit(1);
    }

    PrepareOutPutFileName();

    PrepareOutPutRootFile();

    hstack1D_DeltaPhi_map.clear();
    hstack2D_RotatedBkg_GoodPair_FineBin_map.clear();
    h1D_RotatedBkg_DeltaPhi_Signal_map.clear();

    hstack2D_BestPair_map.clear();

    PrepareHist();

    c1 = new TCanvas("c1", "c1", 800, 600);
}

void GeoAccepCorr::PrepareInputRootFie()
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
    // cut_GoodProtoTracklet_DeltaPhi = 0;

    tree_in -> SetBranchAddress("centrality_edges", &centrality_edges);
    tree_in -> SetBranchAddress("nCentrality_bin", &nCentrality_bin);

    // tree_in -> SetBranchAddress("CentralityFineEdge_min", &CentralityFineEdge_min);
    // tree_in -> SetBranchAddress("CentralityFineEdge_max", &CentralityFineEdge_max);
    // tree_in -> SetBranchAddress("nCentralityFineBin", &nCentralityFineBin);

    tree_in -> SetBranchAddress("EtaEdge_min", &EtaEdge_min);
    tree_in -> SetBranchAddress("EtaEdge_max", &EtaEdge_max);
    tree_in -> SetBranchAddress("nEtaBin", &nEtaBin);

    tree_in -> SetBranchAddress("VtxZEdge_min", &VtxZEdge_min);
    tree_in -> SetBranchAddress("VtxZEdge_max", &VtxZEdge_max);
    tree_in -> SetBranchAddress("nVtxZBin", &nVtxZBin);

    tree_in -> SetBranchAddress("DeltaPhiEdge_min", &DeltaPhiEdge_min);
    tree_in -> SetBranchAddress("DeltaPhiEdge_max", &DeltaPhiEdge_max);
    tree_in -> SetBranchAddress("nDeltaPhiBin", &nDeltaPhiBin);    

    // tree_in -> SetBranchAddress("cut_GoodProtoTracklet_DeltaPhi", &cut_GoodProtoTracklet_DeltaPhi);

    tree_in -> GetEntry(0);

    std::cout<<"------------------------------------------------------------"<<std::endl;
    std::cout<<"In PrepareInputRootFile(), nEntries : "<<tree_in->GetEntries()<<std::endl;
    std::cout<<"nCentrality_bin : "<<nCentrality_bin<<std::endl;
    for(int i = 0; i < nCentrality_bin; i++)
    {
        std::cout<<"centrality_edges["<<i<<"] : "<<centrality_edges->at(i)<<std::endl;
    }
    // std::cout<<"CentralityFineBin : "<<nCentralityFineBin<<", "<<CentralityFineEdge_min<<", "<<CentralityFineEdge_max<<std::endl;
    std::cout<<"EtaBin : "<<nEtaBin<<", "<<EtaEdge_min<<", "<<EtaEdge_max<<std::endl;
    std::cout<<"VtxZBin : "<<nVtxZBin<<", "<<VtxZEdge_min<<", "<<VtxZEdge_max<<std::endl;
    std::cout<<"DeltaPhiBin : "<<nDeltaPhiBin<<", "<<DeltaPhiEdge_min<<", "<<DeltaPhiEdge_max<<std::endl;
    // std::cout<<"DeltaPhi signal region: "<<cut_GoodProtoTracklet_DeltaPhi->first<<" ~ "<<cut_GoodProtoTracklet_DeltaPhi->second<<std::endl;
    std::cout<<"------------------------------------------------------------"<<std::endl;
}

void GeoAccepCorr::PrepareOutPutFileName()
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

    output_filename = "GeoAccep";
    output_filename = (runnumber != -1) ? "Data_" + output_filename : "MC_" + output_filename;
    output_filename += "_Mbin" + std::to_string(setMbin);
    
    output_filename += output_file_name_suffix;
    output_filename += (runnumber != -1) ? Form("_%s_%s.root",runnumber_str.c_str(),job_index.c_str()) : Form("_%s.root",job_index.c_str());
    
}

void GeoAccepCorr::PrepareOutPutRootFile()
{
    file_out = new TFile(Form("%s/%s",output_directory.c_str(),output_filename.c_str()), "RECREATE");
}

void GeoAccepCorr::PrepareHist()
{
    hstack1D_DeltaPhi_map.clear();
    for (int vtxz_bin = 0; vtxz_bin < nVtxZBin; vtxz_bin++)
    {
        for (int eta_bin = 0; eta_bin < nEtaBin; eta_bin++)
        {
            hstack1D_DeltaPhi_map.insert(
                std::make_pair(
                    Form("hstack1D_DeltaPhi_VtxZ%d_Eta%d",vtxz_bin,eta_bin),
                    new THStack(Form("hstack1D_DeltaPhi_VtxZ%d_Eta%d",vtxz_bin,eta_bin), Form("hstack1D_DeltaPhi_VtxZ%d_Eta%d",vtxz_bin,eta_bin))
                )
            );

            hstack1D_DeltaPhi_map.insert(
                std::make_pair(
                    Form("hstack1D_DeltaPhi_VtxZ%d_Eta%d_rotated",vtxz_bin,eta_bin),
                    new THStack(Form("hstack1D_DeltaPhi_VtxZ%d_Eta%d_rotated",vtxz_bin,eta_bin), Form("hstack1D_DeltaPhi_VtxZ%d_Eta%d_rotated",vtxz_bin,eta_bin))
                )
            );

            hstack1D_DeltaPhi_map.insert(
                std::make_pair(
                    Form("hstack1D_typeA_DeltaPhi_VtxZ%d_Eta%d",vtxz_bin,eta_bin),
                    new THStack(Form("hstack1D_typeA_DeltaPhi_VtxZ%d_Eta%d",vtxz_bin,eta_bin), Form("hstack1D_typeA_DeltaPhi_VtxZ%d_Eta%d",vtxz_bin,eta_bin))
                )
            );

            hstack1D_DeltaPhi_map.insert(
                std::make_pair(
                    Form("hstack1D_typeA_DeltaPhi_VtxZ%d_Eta%d_rotated",vtxz_bin,eta_bin),
                    new THStack(Form("hstack1D_typeA_DeltaPhi_VtxZ%d_Eta%d_rotated",vtxz_bin,eta_bin), Form("hstack1D_typeA_DeltaPhi_VtxZ%d_Eta%d_rotated",vtxz_bin,eta_bin))
                )
            );

            h1D_RotatedBkg_DeltaPhi_Signal_map.insert(
                std::make_pair(
                    Form("h1D_RotatedBkg_DeltaPhi_Signal_VtxZ%d_Eta%d", vtxz_bin, eta_bin),
                    new TH1D(Form("h1D_RotatedBkg_DeltaPhi_Signal_VtxZ%d_Eta%d", vtxz_bin, eta_bin), Form("h1D_RotatedBkg_DeltaPhi_Signal_VtxZ%d_Eta%d;#Delta#phi;Entries", vtxz_bin, eta_bin), nDeltaPhiBin, DeltaPhiEdge_min, DeltaPhiEdge_max)
                )
            );

            h1D_RotatedBkg_DeltaPhi_Signal_map.insert(
                std::make_pair(
                    Form("h1D_typeA_RotatedBkg_DeltaPhi_Signal_VtxZ%d_Eta%d", vtxz_bin, eta_bin),
                    new TH1D(Form("h1D_typeA_RotatedBkg_DeltaPhi_Signal_VtxZ%d_Eta%d", vtxz_bin, eta_bin), Form("h1D_typeA_RotatedBkg_DeltaPhi_Signal_VtxZ%d_Eta%d;#Delta#phi;Entries", vtxz_bin, eta_bin), nDeltaPhiBin, DeltaPhiEdge_min, DeltaPhiEdge_max)
                )
            );
        }
    }

    hstack2D_RotatedBkg_GoodPair_FineBin_map.clear();
    hstack2D_RotatedBkg_GoodPair_FineBin_map.insert(
        std::make_pair(
            "hstack2D_RotatedBkg_EtaVtxZ_FineBin",
            new THStack("hstack2D_RotatedBkg_EtaVtxZ_FineBin", "hstack2D_RotatedBkg_EtaVtxZ_FineBin")
        )
    );

    hstack2D_RotatedBkg_GoodPair_FineBin_map.insert(
        std::make_pair(
            "hstack2D_RotatedBkg_EtaVtxZ_rotated_FineBin",
            new THStack("hstack2D_RotatedBkg_EtaVtxZ_rotated_FineBin", "hstack2D_RotatedBkg_EtaVtxZ_rotated_FineBin")
        )
    );

    hstack2D_RotatedBkg_GoodPair_FineBin_map.insert(
        std::make_pair(
            "hstack2D_typeA_RotatedBkg_EtaVtxZ_FineBin",
            new THStack("hstack2D_typeA_RotatedBkg_EtaVtxZ_FineBin", "hstack2D_typeA_RotatedBkg_EtaVtxZ_FineBin")
        )
    );

    hstack2D_RotatedBkg_GoodPair_FineBin_map.insert(
        std::make_pair(
            "hstack2D_typeA_RotatedBkg_EtaVtxZ_rotated_FineBin",
            new THStack("hstack2D_typeA_RotatedBkg_EtaVtxZ_rotated_FineBin", "hstack2D_typeA_RotatedBkg_EtaVtxZ_rotated_FineBin")
        )
    );

    // note : final multiplicity [0-100%]
    h2D_RotatedBkg_TrackletEta_VtxZ = new TH2D(
        "h2D_RotatedBkg_TrackletEta_VtxZ",
        "h2D_RotatedBkg_TrackletEta_VtxZ;#eta;INTT VtxZ [cm]", nEtaBin, EtaEdge_min, EtaEdge_max, nVtxZBin, VtxZEdge_min, VtxZEdge_max
    );
    h2D_typeA_RotatedBkg_TrackletEta_VtxZ = new TH2D(
        "h2D_typeA_RotatedBkg_TrackletEta_VtxZ",
        "h2D_typeA_RotatedBkg_TrackletEta_VtxZ;#eta;INTT VtxZ [cm]", nEtaBin, EtaEdge_min, EtaEdge_max, nVtxZBin, VtxZEdge_min, VtxZEdge_max
    );

    // h2D_RotatedBkg_TrackletEta_VtxZ_FineBin = new TH2D(
    //     "h2D_RotatedBkg_TrackletEta_VtxZ_FineBin",
    //     "h2D_RotatedBkg_TrackletEta_VtxZ_FineBin;#eta;INTT VtxZ [cm]", 540, EtaEdge_min, EtaEdge_max, 450, VtxZEdge_min, VtxZEdge_max
    // );
    // h2D_typeA_RotatedBkg_TrackletEta_VtxZ_FineBin = new TH2D(
    //     "h2D_typeA_RotatedBkg_TrackletEta_VtxZ_FineBin",
    //     "h2D_typeA_RotatedBkg_TrackletEta_VtxZ_FineBin;#eta;INTT VtxZ [cm]", 540, EtaEdge_min, EtaEdge_max, 450, VtxZEdge_min, VtxZEdge_max
    // );
    

    // Division : -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    hstack2D_BestPair_map.clear();

    hstack2D_BestPair_map.insert(
        std::make_pair(
            "hstack2D_BestPair_EtaVtxZ",
            new THStack("hstack2D_BestPair_EtaVtxZ", "hstack2D_BestPair_EtaVtxZ")
        )
    );

    hstack2D_BestPair_map.insert(
        std::make_pair(
            "hstack2D_typeA_BestPair_EtaVtxZ",
            new THStack("hstack2D_typeA_BestPair_EtaVtxZ", "hstack2D_typeA_BestPair_EtaVtxZ")
        )
    );

    // note : final multiplicity [0-100%]
    // h2D_BestPair_TrackletEta_VtxZ = new TH2D(
    //     "h2D_BestPair_TrackletEta_VtxZ",
    //     "h2D_BestPair_TrackletEta_VtxZ;#eta;INTT VtxZ [cm]", 540, EtaEdge_min, EtaEdge_max, 450, VtxZEdge_min, VtxZEdge_max
    // );
    // h2D_typeA_BestPair_TrackletEta_VtxZ = new TH2D(
    //     "h2D_typeA_BestPair_TrackletEta_VtxZ",
    //     "h2D_typeA_BestPair_TrackletEta_VtxZ;#eta;INTT VtxZ [cm]", 540, EtaEdge_min, EtaEdge_max, 450, VtxZEdge_min, VtxZEdge_max
    // );
}

// note : return the bin index of eta, vtxZ, Mbin, typeA, rotated, finebin
std::tuple<int, int, int, int, int, int> GeoAccepCorr::GetHistStringInfo(std::string hist_name)
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

void GeoAccepCorr::PrepareStacks()
{
    int l_eta_bin, l_vtxz_bin, l_Mbin;
    int l_typeA, l_rotated, l_finebin;

    for (auto &pair : h1D_input_map)
    {
        std::tie(l_eta_bin, l_vtxz_bin, l_Mbin, l_typeA, l_rotated, l_finebin) = GetHistStringInfo(pair.first);

        if (l_Mbin == -1) {continue;}

        if (pair.first.find("h1D") != std::string::npos && pair.first.find("_DeltaPhi") != std::string::npos && l_vtxz_bin != -1 && l_eta_bin != -1){

            std::cout<<"----- "<<pair.first<<", eta_bin: "<<l_eta_bin<<", vtxz_bin: "<<l_vtxz_bin<<", Mbin: "<<l_Mbin<<", typeA: "<<l_typeA<<", rotated: "<<l_rotated<<", finebin: "<<l_finebin<<std::endl;

            std::string isTypeA_str = (l_typeA == 1) ? "_typeA" : "";
            std::string isRotated_str = (l_rotated == 1) ? "_rotated" : "";

            std::string target_hstack1D_name = Form("hstack1D%s_DeltaPhi_VtxZ%d_Eta%d%s",isTypeA_str.c_str(),l_vtxz_bin,l_eta_bin,isRotated_str.c_str());
            std::cout<<"target_hstack1D_name: "<<target_hstack1D_name<<std::endl;
            
            if (hstack1D_DeltaPhi_map.find(target_hstack1D_name) == hstack1D_DeltaPhi_map.end()){
                std::cout<<"Error: target_hstack1D_name not found"<<std::endl;
            }

            if (
                setMbin == 100 ||
                (setMbin == Semi_inclusive_bin*10 && l_Mbin <= Semi_inclusive_bin) ||
                (setMbin == l_Mbin)
            )
            {
                THStack * temp_stack1D = hstack1D_DeltaPhi_map[target_hstack1D_name];
                pair.second->SetFillColor(ROOT_color_code[temp_stack1D->GetNhists() % ROOT_color_code.size()]);
                temp_stack1D -> Add(pair.second);
            }
            else {
                std::cout<<"!!! Rejected !!! "<<pair.first<<std::endl;
            }
            

        }
    }

    for (int Mbin = 0; Mbin < nCentrality_bin; Mbin++){
        
        if (
            setMbin == 100 ||
            (setMbin == Semi_inclusive_bin*10 && Mbin <= Semi_inclusive_bin) ||
            (setMbin == Mbin)
        )
        {
            hstack2D_RotatedBkg_GoodPair_FineBin_map["hstack2D_RotatedBkg_EtaVtxZ_FineBin"] -> Add(h2D_input_map[Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d_FineBin", Mbin)]);
            hstack2D_RotatedBkg_GoodPair_FineBin_map["hstack2D_RotatedBkg_EtaVtxZ_rotated_FineBin"] -> Add(h2D_input_map[Form("h2D_GoodProtoTracklet_EtaVtxZ_Mbin%d_rotated_FineBin", Mbin)]);

            hstack2D_RotatedBkg_GoodPair_FineBin_map["hstack2D_typeA_RotatedBkg_EtaVtxZ_FineBin"] -> Add(h2D_input_map[Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d_FineBin", Mbin)]);
            hstack2D_RotatedBkg_GoodPair_FineBin_map["hstack2D_typeA_RotatedBkg_EtaVtxZ_rotated_FineBin"] -> Add(h2D_input_map[Form("h2D_typeA_GoodProtoTracklet_EtaVtxZ_Mbin%d_rotated_FineBin", Mbin)]);

            hstack2D_BestPair_map["hstack2D_BestPair_EtaVtxZ"] -> Add(h2D_input_map[Form("h2D_BestPairEtaVtxZ_Mbin%d_FineBin",Mbin)]);
            hstack2D_BestPair_map["hstack2D_typeA_BestPair_EtaVtxZ"] -> Add(h2D_input_map[Form("h2D_typeA_BestPairEtaVtxZ_Mbin%d_FineBin",Mbin)]);
        }

    }
}

void GeoAccepCorr::PrepareMultiplicity()
{
    for (int vtxz_bin = 0; vtxz_bin < nVtxZBin; vtxz_bin++)
    {
        for (int eta_bin = 0; eta_bin < nEtaBin; eta_bin++)
        {
            auto temp_h1D_DeltaPhi         = (TH1D*)hstack1D_DeltaPhi_map[Form("hstack1D_DeltaPhi_VtxZ%d_Eta%d",vtxz_bin,eta_bin)]->GetStack()->Last();
            auto temp_h1D_DeltaPhi_rotated = (TH1D*)hstack1D_DeltaPhi_map[Form("hstack1D_DeltaPhi_VtxZ%d_Eta%d_rotated",vtxz_bin,eta_bin)]->GetStack()->Last();
            TH1D * temp_hist = h1D_RotatedBkg_DeltaPhi_Signal_map[Form("h1D_RotatedBkg_DeltaPhi_Signal_VtxZ%d_Eta%d", vtxz_bin, eta_bin)];
            temp_hist -> Add(temp_h1D_DeltaPhi, temp_h1D_DeltaPhi_rotated, 1, -1);
            
            std::pair<int, int> DeltaPhi_signal_bin = get_DeltaPhi_SingleBin(temp_hist, BkgRotated_DeltaPhi_Signal_range);

            double cell_multiplicity = temp_hist->Integral(DeltaPhi_signal_bin.first, DeltaPhi_signal_bin.second);
            cell_multiplicity = (cell_multiplicity <= 0) ? 0 : cell_multiplicity;
            h2D_RotatedBkg_TrackletEta_VtxZ -> SetBinContent(eta_bin+1, vtxz_bin+1, cell_multiplicity);



            auto temp_typeA_h1D_DeltaPhi         = (TH1D*)hstack1D_DeltaPhi_map[Form("hstack1D_typeA_DeltaPhi_VtxZ%d_Eta%d",vtxz_bin,eta_bin)]->GetStack()->Last();
            auto temp_typeA_h1D_DeltaPhi_rotated = (TH1D*)hstack1D_DeltaPhi_map[Form("hstack1D_typeA_DeltaPhi_VtxZ%d_Eta%d_rotated",vtxz_bin,eta_bin)]->GetStack()->Last();
            TH1D * temp_typeA_hist = h1D_RotatedBkg_DeltaPhi_Signal_map[Form("h1D_typeA_RotatedBkg_DeltaPhi_Signal_VtxZ%d_Eta%d", vtxz_bin, eta_bin)];
            temp_typeA_hist -> Add(temp_typeA_h1D_DeltaPhi, temp_typeA_h1D_DeltaPhi_rotated, 1, -1);

            std::pair<int, int> DeltaPhi_signal_bin_typeA = get_DeltaPhi_SingleBin(temp_typeA_hist, BkgRotated_DeltaPhi_Signal_range);
            cell_multiplicity = temp_typeA_hist->Integral(DeltaPhi_signal_bin_typeA.first, DeltaPhi_signal_bin_typeA.second);
            cell_multiplicity = (cell_multiplicity <= 0) ? 0 : cell_multiplicity;
            h2D_typeA_RotatedBkg_TrackletEta_VtxZ -> SetBinContent(eta_bin+1, vtxz_bin+1, cell_multiplicity);

        }
    }

    // Division : --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    h2D_RotatedBkg_Rebin               = (TH2D*)((TH2D*)hstack2D_RotatedBkg_GoodPair_FineBin_map["hstack2D_RotatedBkg_EtaVtxZ_FineBin"]->GetStack()->Last())->Clone("h2D_RotatedBkg_Rebin");
    h2D_RotatedBkg_rotated_Rebin       = (TH2D*)((TH2D*)hstack2D_RotatedBkg_GoodPair_FineBin_map["hstack2D_RotatedBkg_EtaVtxZ_rotated_FineBin"]->GetStack()->Last())->Clone("h2D_RotatedBkg_rotated_Rebin");

    h2D_typeA_RotatedBkg_Rebin         = (TH2D*)((TH2D*)hstack2D_RotatedBkg_GoodPair_FineBin_map["hstack2D_typeA_RotatedBkg_EtaVtxZ_FineBin"]->GetStack()->Last())->Clone("h2D_typeA_RotatedBkg_Rebin");
    h2D_typeA_RotatedBkg_rotated_Rebin = (TH2D*)((TH2D*)hstack2D_RotatedBkg_GoodPair_FineBin_map["hstack2D_typeA_RotatedBkg_EtaVtxZ_rotated_FineBin"]->GetStack()->Last())->Clone("h2D_typeA_RotatedBkg_rotated_Rebin");

    double current_bin_widthX = h2D_RotatedBkg_Rebin -> GetXaxis() -> GetBinWidth(1);
    double current_bin_widthY = h2D_RotatedBkg_Rebin -> GetYaxis() -> GetBinWidth(1);
    
    // todo : target bin width
    double target_bin_widthX = 0.2; // note : eta,
    double target_bin_widthY = 1; // note : vtxZ

    int RebinGroupX = int(target_bin_widthX / current_bin_widthX);
    int RebinGroupY = int(target_bin_widthY / current_bin_widthY);

    h2D_RotatedBkg_Rebin -> Rebin2D(RebinGroupX, RebinGroupY);
    h2D_RotatedBkg_rotated_Rebin -> Rebin2D(RebinGroupX, RebinGroupY);

    h2D_typeA_RotatedBkg_Rebin -> Rebin2D(RebinGroupX, RebinGroupY);
    h2D_typeA_RotatedBkg_rotated_Rebin -> Rebin2D(RebinGroupX, RebinGroupY);

    std::cout<<"In PrepareMultiplicity(), RebinGroupX: "<<RebinGroupX<<", RebinGroupY: "<<RebinGroupY<<std::endl;
    std::cout<<"h2D_RotatedBkg_Rebin NbinsX: "<<h2D_RotatedBkg_Rebin->GetNbinsX()<<", NbinsY: "<<h2D_RotatedBkg_Rebin->GetNbinsY()<<std::endl;
    std::cout<<"h2D_RotatedBkg_Rebin BinWidthX: "<<h2D_RotatedBkg_Rebin->GetXaxis()->GetBinWidth(1)<<", BinWidthY: "<<h2D_RotatedBkg_Rebin->GetYaxis()->GetBinWidth(1)<<std::endl;

    h2D_RotatedBkg_TrackletEta_VtxZ_FineBin = (TH2D*)h2D_RotatedBkg_Rebin->Clone("h2D_RotatedBkg_TrackletEta_VtxZ_FineBin");
    h2D_RotatedBkg_TrackletEta_VtxZ_FineBin -> Add(h2D_RotatedBkg_Rebin, h2D_RotatedBkg_rotated_Rebin, 1, -1);
    CleanH2DNegativeBin(h2D_RotatedBkg_TrackletEta_VtxZ_FineBin);
    
    h2D_typeA_RotatedBkg_TrackletEta_VtxZ_FineBin = (TH2D*)h2D_typeA_RotatedBkg_Rebin->Clone("h2D_typeA_RotatedBkg_TrackletEta_VtxZ_FineBin");
    h2D_typeA_RotatedBkg_TrackletEta_VtxZ_FineBin -> Add(h2D_typeA_RotatedBkg_Rebin, h2D_typeA_RotatedBkg_rotated_Rebin, 1, -1);
    CleanH2DNegativeBin(h2D_typeA_RotatedBkg_TrackletEta_VtxZ_FineBin);

    // Division : --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // note : best
    h2D_BestPair_TrackletEta_VtxZ         = (TH2D*)((TH2D*)hstack2D_BestPair_map["hstack2D_BestPair_EtaVtxZ"]->GetStack()->Last())->Clone("h2D_BestPair_TrackletEta_VtxZ");
    h2D_typeA_BestPair_TrackletEta_VtxZ   = (TH2D*)((TH2D*)hstack2D_BestPair_map["hstack2D_typeA_BestPair_EtaVtxZ"]->GetStack()->Last())->Clone("h2D_typeA_BestPair_TrackletEta_VtxZ");

    h2D_BestPair_TrackletEta_VtxZ -> Rebin2D(RebinGroupX, RebinGroupY);
    h2D_typeA_BestPair_TrackletEta_VtxZ -> Rebin2D(RebinGroupX, RebinGroupY);

}

void GeoAccepCorr::EndRun()
{
    file_out -> cd();

    h1D_input_map["h1D_INTTvtxZ_FineBin"] -> Write("h1D_INTTvtxZ_FineBin");
    h1D_input_map["h1D_INTTvtxZ_FineBin_NoVtxZWeight"] -> Write("h1D_INTTvtxZ_FineBin_NoVtxZWeight");
    h2D_input_map["h2D_INTTvtxZFineBin_CentralityBin"] -> Write("h2D_INTTvtxZFineBin_CentralityBin");

    h2D_RotatedBkg_TrackletEta_VtxZ -> Write("h2D_Multiplicity_RotatedBkg_EtaVtxZ"); // note : by 1D hist
    h2D_RotatedBkg_TrackletEta_VtxZ_FineBin -> Write("h2D_Multiplicity_RotatedBkg_EtaVtxZ_FineBin");
    h2D_typeA_RotatedBkg_TrackletEta_VtxZ -> Write("h2D_Multiplicity_typeA_RotatedBkg_EtaVtxZ"); // note : by 1D hist
    h2D_typeA_RotatedBkg_TrackletEta_VtxZ_FineBin -> Write("h2D_Multiplicity_typeA_RotatedBkg_EtaVtxZ_FineBin");
    
    h2D_BestPair_TrackletEta_VtxZ -> Write("h2D_Multiplicity_BestPair_EtaVtxZ_FineBin");
    h2D_typeA_BestPair_TrackletEta_VtxZ -> Write("h2D_Multiplicity_typeA_BestPair_EtaVtxZ_FineBin");

    // note : RotatedBkg - the H2D method
    h2D_RotatedBkg_Rebin -> Write();
    h2D_RotatedBkg_rotated_Rebin -> Write();
    h2D_typeA_RotatedBkg_Rebin -> Write();
    h2D_typeA_RotatedBkg_rotated_Rebin -> Write();

    // note : RotatedBkg - the H2D method - (FineBin)
    ((TH2D*) hstack2D_RotatedBkg_GoodPair_FineBin_map["hstack2D_RotatedBkg_EtaVtxZ_FineBin"]->GetStack()->Last()) -> Write("h2D_RotatedBkg_EtaVtxZ_FineBin");
    ((TH2D*) hstack2D_RotatedBkg_GoodPair_FineBin_map["hstack2D_RotatedBkg_EtaVtxZ_rotated_FineBin"]->GetStack()->Last()) -> Write("h2D_RotatedBkg_EtaVtxZ_rotated_FineBin");
    
    ((TH2D*) hstack2D_RotatedBkg_GoodPair_FineBin_map["hstack2D_typeA_RotatedBkg_EtaVtxZ_FineBin"]->GetStack()->Last()) -> Write("h2D_typeA_RotatedBkg_EtaVtxZ_FineBin");
    ((TH2D*) hstack2D_RotatedBkg_GoodPair_FineBin_map["hstack2D_typeA_RotatedBkg_EtaVtxZ_rotated_FineBin"]->GetStack()->Last()) -> Write("h2D_typeA_RotatedBkg_EtaVtxZ_rotated_FineBin");


    // note : the best pair (fine bin)
    ((TH2D*) hstack2D_BestPair_map["hstack2D_BestPair_EtaVtxZ"]->GetStack()->Last()) -> Write("h2D_BestPair_EtaVtxZ");
    ((TH2D*) hstack2D_BestPair_map["hstack2D_typeA_BestPair_EtaVtxZ"]->GetStack()->Last()) -> Write("h2D_typeA_BestPair_EtaVtxZ");



    for (int vtxz_bin = 0; vtxz_bin < nVtxZBin; vtxz_bin++)
    {
        for (int eta_bin = 0; eta_bin < nEtaBin; eta_bin++)
        {
            auto temp_h1D_DeltaPhi         = (TH1D*)((TH1D*) hstack1D_DeltaPhi_map[Form("hstack1D_DeltaPhi_VtxZ%d_Eta%d",vtxz_bin,eta_bin)] -> GetStack() -> Last()) -> Clone(Form("h1D_DeltaPhi_VtxZ%d_Eta%d",vtxz_bin,eta_bin));
            auto temp_h1D_DeltaPhi_rotated = (TH1D*)((TH1D*) hstack1D_DeltaPhi_map[Form("hstack1D_DeltaPhi_VtxZ%d_Eta%d_rotated",vtxz_bin,eta_bin)] -> GetStack() -> Last()) -> Clone(Form("h1D_DeltaPhi_VtxZ%d_Eta%d_rotated",vtxz_bin,eta_bin));

            if (temp_h1D_DeltaPhi->Integral(-1, -1) <= 0) {continue;}

            temp_h1D_DeltaPhi -> SetFillColorAlpha(2,0);
            temp_h1D_DeltaPhi_rotated -> SetFillColorAlpha(2,0);

            temp_h1D_DeltaPhi -> SetLineColor(4);
            temp_h1D_DeltaPhi_rotated -> SetLineColor(2);

            h1D_RotatedBkg_DeltaPhi_Signal_map[Form("h1D_RotatedBkg_DeltaPhi_Signal_VtxZ%d_Eta%d", vtxz_bin, eta_bin)] -> SetFillColorAlpha(2,0);
            h1D_RotatedBkg_DeltaPhi_Signal_map[Form("h1D_RotatedBkg_DeltaPhi_Signal_VtxZ%d_Eta%d", vtxz_bin, eta_bin)] -> SetLineColor(3);

            c1 -> cd();
            temp_h1D_DeltaPhi -> SetMinimum(0);
            temp_h1D_DeltaPhi -> Draw("hist");
            temp_h1D_DeltaPhi_rotated -> Draw("hist same");
            h1D_RotatedBkg_DeltaPhi_Signal_map[Form("h1D_RotatedBkg_DeltaPhi_Signal_VtxZ%d_Eta%d", vtxz_bin, eta_bin)] -> Draw("hist same");

            c1 -> Write(Form("c1_DeltaPhi_VtxZ%d_Eta%d",vtxz_bin,eta_bin)); 
        }
    }

    for (int vtxz_bin = 0; vtxz_bin < nVtxZBin; vtxz_bin++)
    {
        for (int eta_bin = 0; eta_bin < nEtaBin; eta_bin++)
        {
            auto temp_h1D_typeA_DeltaPhi         = (TH1D*)((TH1D*) hstack1D_DeltaPhi_map[Form("hstack1D_typeA_DeltaPhi_VtxZ%d_Eta%d",vtxz_bin,eta_bin)] -> GetStack() -> Last()) -> Clone(Form("h1D_typeA_DeltaPhi_VtxZ%d_Eta%d",vtxz_bin,eta_bin));
            auto temp_h1D_typeA_DeltaPhi_rotated = (TH1D*)((TH1D*) hstack1D_DeltaPhi_map[Form("hstack1D_typeA_DeltaPhi_VtxZ%d_Eta%d_rotated",vtxz_bin,eta_bin)] -> GetStack() -> Last()) -> Clone(Form("h1D_typeA_DeltaPhi_VtxZ%d_Eta%d_rotated",vtxz_bin,eta_bin));

            if (temp_h1D_typeA_DeltaPhi -> Integral(-1,-1) <= 0) {continue;}

            temp_h1D_typeA_DeltaPhi -> SetFillColorAlpha(2,0);
            temp_h1D_typeA_DeltaPhi_rotated -> SetFillColorAlpha(2,0);

            temp_h1D_typeA_DeltaPhi -> SetLineColor(4);
            temp_h1D_typeA_DeltaPhi_rotated -> SetLineColor(2);

            h1D_RotatedBkg_DeltaPhi_Signal_map[Form("h1D_typeA_RotatedBkg_DeltaPhi_Signal_VtxZ%d_Eta%d", vtxz_bin, eta_bin)] -> SetFillColorAlpha(2,0);
            h1D_RotatedBkg_DeltaPhi_Signal_map[Form("h1D_typeA_RotatedBkg_DeltaPhi_Signal_VtxZ%d_Eta%d", vtxz_bin, eta_bin)] -> SetLineColor(3);

            c1 -> cd();
            temp_h1D_typeA_DeltaPhi -> SetMinimum(0);
            temp_h1D_typeA_DeltaPhi -> Draw("hist");
            temp_h1D_typeA_DeltaPhi_rotated -> Draw("hist same");
            h1D_RotatedBkg_DeltaPhi_Signal_map[Form("h1D_typeA_RotatedBkg_DeltaPhi_Signal_VtxZ%d_Eta%d", vtxz_bin, eta_bin)] -> Draw("hist same");

            c1 -> Write(Form("c1_typeA_DeltaPhi_VtxZ%d_Eta%d",vtxz_bin,eta_bin)); 
        }
    }


    for (auto &pair : h1D_RotatedBkg_DeltaPhi_Signal_map){
        pair.second -> Write();
    }    

    for (auto &pair : hstack1D_DeltaPhi_map){
        pair.second -> Write();
    }

    file_out -> Close();
}

std::pair<int,int> GeoAccepCorr::get_DeltaPhi_SingleBin(TH1D * hist_in, std::pair<double, double> range_in)
{
    int bin_min = hist_in -> FindBin(range_in.first);
    
    int bin_max = hist_in -> FindBin(range_in.second);
    bin_max = (fabs(hist_in -> GetXaxis() -> GetBinLowEdge(bin_max) - range_in.second) < 1e-9) ? bin_max - 1 : bin_max;

    std::cout<<"Input range : "<<range_in.first<<", "<<range_in.second<<std::endl;
    std::cout<<"Final selected bins: "<<bin_min<<", "<<bin_max<<std::endl;
    std::cout<<"Output range : "<<hist_in -> GetXaxis() -> GetBinLowEdge(bin_min)<<", "<<hist_in -> GetXaxis() -> GetBinUpEdge(bin_max)<<std::endl;

    return std::make_pair(bin_min, bin_max);
}

void GeoAccepCorr::CleanH2DNegativeBin(TH2D * hist_in)
{
    for (int i = 1; i <= hist_in -> GetNbinsX(); i++)
    {
        for (int j = 1; j <= hist_in -> GetNbinsY(); j++)
        {
            if (hist_in -> GetBinContent(i,j) <= 0)
            {
                hist_in -> SetBinContent(i,j,0);
            }
        }
    }
}