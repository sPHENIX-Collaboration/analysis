#include "AvgVtxXY.h"

AvgVtxXY::AvgVtxXY(
    int process_id_in,
    int runnumber_in,
    int run_nEvents_in,
    std::string input_directory_in,
    std::string input_file_name_in,
    std::string output_directory_in,
    std::string output_file_name_suffix_in,

    // note : for the event selection
    std::pair<double,double> MBD_vtxZ_cut_in,
    std::pair<int,int> INTTNClus_cut_in,
    int ClusAdc_cut_in,
    int ClusPhiSize_cut_in,

    bool HaveGeoOffsetTag_in,
    double random_range_XYZ_in,
    int random_seed_in,
    std::string input_offset_map_in // note : full map
):
    process_id(process_id_in),
    runnumber(runnumber_in),
    run_nEvents(run_nEvents_in),
    input_directory(input_directory_in),
    input_file_name(input_file_name_in),
    output_directory(output_directory_in),
    output_file_name_suffix(output_file_name_suffix_in),
    MBD_vtxZ_cut(MBD_vtxZ_cut_in),
    INTTNClus_cut(INTTNClus_cut_in),
    ClusAdc_cut(ClusAdc_cut_in),
    ClusPhiSize_cut(ClusPhiSize_cut_in),
    HaveGeoOffsetTag(HaveGeoOffsetTag_in),
    random_range_XYZ(random_range_XYZ_in),
    random_seed(random_seed_in),
    input_offset_map(input_offset_map_in),

    hopeless_pair_angle_cut(45),                           // todo: // note : degree
    rough_DeltaPhi_cut_degree(7),                          // todo: 
    InnerOuterPhi_DeltaPhi_DCA_Threshold(0.5),             // todo:
    LineFill_Hist_Threshold(0.7),                          // todo:
    InnerPhi_DeltaPhi_DCA_Fit_Tgr_HitContent_Threshold(5), // todo:
    TH2D_threshold_advanced_2_CheckNBins(7)                // todo:
{
    ReadRootFile();
    run_nEvents = (run_nEvents == -1) ? tree_in->GetEntries() : run_nEvents;
    run_nEvents = (run_nEvents > tree_in->GetEntries()) ? tree_in->GetEntries() : run_nEvents;
    
    InitOutRootFile();   

    fit_innerphi_DeltaPhi = new TF1("fit_innerphi_DeltaPhi","pol0",-M_PI, M_PI);
    fit_innerphi_DCA = new TF1("fit_innerphi_DCA","pol0",-M_PI, M_PI);

    h2D_map.clear();
    h1D_map.clear();
    gr_map.clear();
    profile_map.clear();

    for (int layer_i = B0L0_index; layer_i <= B1L1_index; layer_i++){
        double N_layer_ladder = (layer_i == B0L0_index || layer_i == B0L1_index) ? nLadder_inner : nLadder_outer;
        for (int phi_i = 0; phi_i < N_layer_ladder; phi_i++){
            geo_offset_map[Form("%i_%i", layer_i, phi_i)] = {0, 0, 0};
        }
    }

    if (HaveGeoOffsetTag) {
        if (input_offset_map == "no_map"){
            std::cout<<"HaveGeoOffsetTag: "<<HaveGeoOffsetTag<<", but no_map from the input, therefore, generate one"<<std::endl;
            GenGeoOffset();
        }
        else {
            std::cout<<"HaveGeoOffsetTag: "<<HaveGeoOffsetTag<<", with the map: "<<input_offset_map<<std::endl;
            SetGeoOffset();
        }
    }

    // todo: it works if all the events in the files are used
    // out_start_evt = process_id * run_nEvents;
    // out_end_evt   = out_start_evt + run_nEvents - 1;
    
}

void AvgVtxXY::ReadRootFile()
{
    file_in = TFile::Open((input_directory + "/" + input_file_name).c_str());

    if (file_in == nullptr) {
        std::cout << "Error : cannot open the file : " << input_directory + "/" + input_file_name << std::endl;
        exit(1);
    }

    tree_in = (TTree*)file_in->Get(input_tree_name.c_str());

    tree_in -> SetBranchStatus("*", 0);
    tree_in -> SetBranchStatus("MBD_z_vtx", 1);
    tree_in -> SetBranchStatus("is_min_bias", 1);
    tree_in -> SetBranchStatus("MBD_centrality", 1);
    if (runnumber != -1) {
        tree_in -> SetBranchStatus("InttBco_IsToBeRemoved", 1);
    }

    tree_in -> SetBranchStatus("NClus", 1);
    tree_in -> SetBranchStatus("ClusX", 1);
    tree_in -> SetBranchStatus("ClusY", 1);
    tree_in -> SetBranchStatus("ClusLayer", 1);
    tree_in -> SetBranchStatus("ClusLadderPhiId", 1);
    tree_in -> SetBranchStatus("ClusAdc", 1);
    tree_in -> SetBranchStatus("ClusPhiSize", 1);

    ClusX = 0;
    ClusY = 0;
    ClusLayer = 0;
    ClusLadderPhiId = 0;
    ClusAdc = 0;
    ClusPhiSize = 0;

    tree_in -> SetBranchAddress("MBD_z_vtx", &MBD_z_vtx);
    tree_in -> SetBranchAddress("is_min_bias", &is_min_bias);
    tree_in -> SetBranchAddress("MBD_centrality", &MBD_centrality);
    if (runnumber != -1) {
        tree_in -> SetBranchAddress("InttBco_IsToBeRemoved", &InttBco_IsToBeRemoved);
    }

    tree_in -> SetBranchAddress("NClus", &NClus);
    tree_in -> SetBranchAddress("ClusX", &ClusX);
    tree_in -> SetBranchAddress("ClusY", &ClusY);
    tree_in -> SetBranchAddress("ClusLayer", &ClusLayer);
    tree_in -> SetBranchAddress("ClusLadderPhiId", &ClusLadderPhiId);
    tree_in -> SetBranchAddress("ClusAdc", &ClusAdc);
    tree_in -> SetBranchAddress("ClusPhiSize", &ClusPhiSize);
}

void AvgVtxXY::InitOutRootFile()
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

    output_filename = "AvgVtxXY";
    output_filename = (runnumber != -1) ? "Data_" + output_filename : "MC_" + output_filename;
    output_filename += (HaveGeoOffsetTag) ? "_GeoOffset" : "";
    output_filename += output_file_name_suffix;
    output_filename += (runnumber != -1) ? Form("_%s_%s.root",runnumber_str.c_str(),job_index.c_str()) : Form("_%s.root",job_index.c_str());

    file_out = new TFile((output_directory + "/" + output_filename).c_str(), "RECREATE");
    
    // note : this is for the final
    tree_vtxXY = new TTree("tree_vtxXY", "tree_vtxXY");
    tree_vtxXY -> Branch("vtxX_quadrant", &out_vtxX_quadrant);
    tree_vtxXY -> Branch("vtxXE_quadrant", &out_vtxXE_quadrant);
    tree_vtxXY -> Branch("vtxY_quadrant", &out_vtxY_quadrant);
    tree_vtxXY -> Branch("vtxYE_quadrant", &out_vtxYE_quadrant);

    tree_vtxXY -> Branch("vtxX_LineFill", &out_vtxX_LineFill);
    tree_vtxXY -> Branch("vtxXE_LineFill", &out_vtxXE_LineFill);
    tree_vtxXY -> Branch("vtxXStdDev_LineFill", &out_vtxXStdDev_LineFill);
    tree_vtxXY -> Branch("vtxY_LineFill", &out_vtxY_LineFill);
    tree_vtxXY -> Branch("vtxYE_LineFill", &out_vtxYE_LineFill);
    tree_vtxXY -> Branch("vtxYStdDev_LineFill", &out_vtxYStdDev_LineFill);
    tree_vtxXY -> Branch("run_nEvents", &out_run_nEvents);
    tree_vtxXY -> Branch("job_index", &out_job_index);
    tree_vtxXY -> Branch("file_total_event", &out_file_total_event);
    // tree_vtxXY -> Branch("start_evt", &out_start_evt);
    // tree_vtxXY -> Branch("end_evt", &out_end_evt);

    out_job_index = process_id; 
    out_file_total_event = tree_in->GetEntries();
    
    // note : for the geo offset
    if (HaveGeoOffsetTag) {

        tree_geooffset = new TTree("tree_geooffset", "tree_geooffset");
        tree_geooffset -> Branch("LayerID", &out_LayerID);
        tree_geooffset -> Branch("LadderPhiID", &out_LadderPhiID);
        tree_geooffset -> Branch("offset_x", &out_offset_x);
        tree_geooffset -> Branch("offset_y", &out_offset_y);   
        tree_geooffset -> Branch("offset_z", &out_offset_z);
    }

    // note : for the quadrant detail
    tree_quadrant_detail = new TTree("tree_quadrant_detail", "tree_quadrant_detail");
    tree_quadrant_detail -> Branch("iteration", &out_iteration);
    tree_quadrant_detail -> Branch("quadrant", &out_quadrant);
    tree_quadrant_detail -> Branch("assume_center_x", &out_assume_center_x);
    tree_quadrant_detail -> Branch("assume_center_y", &out_assume_center_y);
    tree_quadrant_detail -> Branch("Fit_InnerPhi_DeltaPhi_RChi2", &out_Fit_InnerPhi_DeltaPhi_RChi2);
    tree_quadrant_detail -> Branch("Fit_InnerPhi_DeltaPhi_Err0", &out_Fit_InnerPhi_DeltaPhi_Err0);
    tree_quadrant_detail -> Branch("Fit_InnerPhi_DeltaPhi_LineY", &out_Fit_InnerPhi_DeltaPhi_LineY);
    tree_quadrant_detail -> Branch("Fit_InnerPhi_DCA_RChi2", &out_Fit_InnerPhi_DCA_RChi2);
    tree_quadrant_detail -> Branch("Fit_InnerPhi_DCA_Err0", &out_Fit_InnerPhi_DCA_Err0);
    tree_quadrant_detail -> Branch("Fit_InnerPhi_DCA_LineY", &out_Fit_InnerPhi_DCA_LineY);

    // note : for the output paramteres
    tree_parameters = new TTree("tree_parameters", "tree_parameters");
    out_run_nEvents = run_nEvents;
    out_MBD_vtxZ_cut_l = MBD_vtxZ_cut.first;
    out_MBD_vtxZ_cut_r = MBD_vtxZ_cut.second;
    out_INTTNClus_cut_l = INTTNClus_cut.first;
    out_INTTNClus_cut_r = INTTNClus_cut.second;
    out_ClusAdc_cut = ClusAdc_cut;
    out_ClusPhiSize_cut = ClusPhiSize_cut;
    out_HaveGeoOffsetTag = HaveGeoOffsetTag;
    out_random_range_XYZ = random_range_XYZ;
    out_random_seed = random_seed;
    out_input_offset_map = (input_offset_map == "no_map") ? 0 : 1;
    out_hopeless_pair_angle_cut = hopeless_pair_angle_cut;
    out_rough_DeltaPhi_cut_degree = rough_DeltaPhi_cut_degree;
    out_InnerOuterPhi_DeltaPhi_DCA_Threshold = InnerOuterPhi_DeltaPhi_DCA_Threshold;
    out_LineFill_Hist_Threshold = LineFill_Hist_Threshold;
    out_InnerPhi_DeltaPhi_DCA_Fit_Tgr_HitContent_Threshold = InnerPhi_DeltaPhi_DCA_Fit_Tgr_HitContent_Threshold;
    out_TH2D_threshold_advanced_2_CheckNBins = TH2D_threshold_advanced_2_CheckNBins;

    tree_parameters -> Branch("run_nEvents", &out_run_nEvents);
    tree_parameters -> Branch("MBD_vtxZ_cut_l", &out_MBD_vtxZ_cut_l);
    tree_parameters -> Branch("MBD_vtxZ_cut_r", &out_MBD_vtxZ_cut_r);
    tree_parameters -> Branch("INTTNClus_cut_l", &out_INTTNClus_cut_l);
    tree_parameters -> Branch("INTTNClus_cut_r", &out_INTTNClus_cut_r);
    tree_parameters -> Branch("ClusAdc_cut", &out_ClusAdc_cut);
    tree_parameters -> Branch("ClusPhiSize_cut", &out_ClusPhiSize_cut);
    tree_parameters -> Branch("HaveGeoOffsetTag", &out_HaveGeoOffsetTag);
    tree_parameters -> Branch("random_range_XYZ", &out_random_range_XYZ);
    tree_parameters -> Branch("random_seed", &out_random_seed);
    tree_parameters -> Branch("input_offset_map", &out_input_offset_map);
    tree_parameters -> Branch("hopeless_pair_angle_cut", &out_hopeless_pair_angle_cut);
    tree_parameters -> Branch("rough_DeltaPhi_cut_degree", &out_rough_DeltaPhi_cut_degree);
    tree_parameters -> Branch("InnerOuterPhi_DeltaPhi_DCA_Threshold", &out_InnerOuterPhi_DeltaPhi_DCA_Threshold);
    tree_parameters -> Branch("LineFill_Hist_Threshold", &out_LineFill_Hist_Threshold);
    tree_parameters -> Branch("InnerPhi_DeltaPhi_DCA_Fit_Tgr_HitContent_Threshold", &out_InnerPhi_DeltaPhi_DCA_Fit_Tgr_HitContent_Threshold);
    tree_parameters -> Branch("TH2D_threshold_advanced_2_CheckNBins", &out_TH2D_threshold_advanced_2_CheckNBins);

    tree_parameters -> Fill();
}

void AvgVtxXY::geo_offset_correction_fill()
{
    // note : it's possible that the whole system has a systematic offset, so we need to correct it
    // std::vector<double> XYZ_correction = offset_correction(geo_offset_map);
    // for (const auto& pair : geo_offset_map)
    // {
    //     geo_offset_map[pair.first] = {pair.second[0] - XYZ_correction[0], pair.second[1] - XYZ_correction[1], pair.second[2] - XYZ_correction[2]};
    // }

    for (const auto& pair : geo_offset_map)
    {
        out_LayerID = std::stoi(pair.first.substr(0,pair.first.find("_")));
        out_LadderPhiID = std::stoi(pair.first.substr(pair.first.find("_")+1));
        out_offset_x = pair.second[0];
        out_offset_y = pair.second[1];
        out_offset_z = pair.second[2];
        tree_geooffset -> Fill();
    }
}

void AvgVtxXY::SetGeoOffset()
{
    geo_offset_map.clear();
    TFile * file_offset = TFile::Open((input_offset_map).c_str());
    TTree * tree_offset = (TTree*)file_offset->Get("tree_geooffset");
    int in_LayerID;
    int in_LadderPhiID;
    double in_offset_x;
    double in_offset_y;
    double in_offset_z;

    tree_offset -> SetBranchAddress("LayerID", &in_LayerID);
    tree_offset -> SetBranchAddress("LadderPhiID", &in_LadderPhiID);
    tree_offset -> SetBranchAddress("offset_x", &in_offset_x);
    tree_offset -> SetBranchAddress("offset_y", &in_offset_y);
    tree_offset -> SetBranchAddress("offset_z", &in_offset_z);

    for (int i = 0; i < tree_offset->GetEntries(); i++)
    {
        tree_offset -> GetEntry(i);
        geo_offset_map[Form("%i_%i", in_LayerID, in_LadderPhiID)] = {in_offset_x, in_offset_y, in_offset_z};
    }

    geo_offset_correction_fill();

    file_offset -> Close();
}

void AvgVtxXY::GenGeoOffset()
{
    geo_rand = new TRandom3(0);
    if (random_seed != -999) {
        geo_rand -> SetSeed(random_seed);
    }
    geo_offset_map.clear();

    for (int layer_i = B0L0_index; layer_i <= B1L1_index; layer_i++)
    {
        double N_layer_ladder = (layer_i == B0L0_index || layer_i == B0L1_index) ? nLadder_inner : nLadder_outer;

        for (int phi_i = 0; phi_i < N_layer_ladder; phi_i++)
        {
            geo_offset_map[Form("%i_%i", layer_i, phi_i)] = {
                geo_rand -> Uniform(-random_range_XYZ, random_range_XYZ), 
                geo_rand -> Uniform(-random_range_XYZ, random_range_XYZ),
                geo_rand -> Uniform(-random_range_XYZ, random_range_XYZ)
            };
        }
    }

    geo_offset_correction_fill();
}

std::vector<double> AvgVtxXY::offset_correction(std::map<std::string,std::vector<double>> input_map)
{
    double N_pair = 0;
    double sum_x = 0;
    double sum_y = 0;
    double sum_z = 0;

    for (const auto& pair : input_map)
    {
        N_pair += 1;
        sum_x += pair.second[0];
        sum_y += pair.second[1];
        sum_z += pair.second[2];
    }

    return {sum_x/N_pair, sum_y/N_pair, sum_z/N_pair};
}

void AvgVtxXY::PreparePairs()
{
    cluster_pair_vec.clear();
    inner_cluster_vec.clear();
    outer_cluster_vec.clear();

    for (int i = 0; i < run_nEvents; i++)
    {
        tree_in -> GetEntry(i);

        inner_cluster_vec.clear();
        outer_cluster_vec.clear();        

        if (MBD_centrality != MBD_centrality) {continue;}
        if (is_min_bias != 1) {continue;}
        if (MBD_z_vtx != MBD_z_vtx) {continue;}
        if (MBD_z_vtx < MBD_vtxZ_cut.first || MBD_z_vtx > MBD_vtxZ_cut.second) {continue;}
        if (runnumber != -1 && InttBco_IsToBeRemoved == 1) {continue;}

        if (NClus < INTTNClus_cut.first || NClus > INTTNClus_cut.second) {continue;}

        for (int j = 0; j < NClus; j++)
        {
            if (ClusAdc->at(j) <= ClusAdc_cut) {continue;}
            if (ClusPhiSize->at(j) > ClusPhiSize_cut) {continue;}

            if (geo_offset_map.find(Form("%i_%i", ClusLayer->at(j), ClusLadderPhiId->at(j))) == geo_offset_map.end()) {
                std::cout << "Error : the geo offset map does not have the key : " << Form("%i_%i", ClusLayer->at(j), ClusLadderPhiId->at(j)) << std::endl;
                exit(1);
            }

            double post_ClusX = ClusX->at(j) + geo_offset_map[Form("%i_%i", ClusLayer->at(j), ClusLadderPhiId->at(j))][0];
            double post_ClusY = ClusY->at(j) + geo_offset_map[Form("%i_%i", ClusLayer->at(j), ClusLadderPhiId->at(j))][1];

            if (ClusLayer->at(j) == B0L0_index || ClusLayer->at(j) == B0L1_index)
            {
                inner_cluster_vec.push_back({post_ClusX, post_ClusY});
            }
            else if (ClusLayer->at(j) == B1L0_index || ClusLayer->at(j) == B1L1_index)
            {
                outer_cluster_vec.push_back({post_ClusX, post_ClusY});
            }
        }

        for (const auto& inner_cluster : inner_cluster_vec)
        {
            for (const auto& outer_cluster : outer_cluster_vec)
            {

                double Clus_InnerPhiDegree = (inner_cluster.y < 0) ? atan2(inner_cluster.y, inner_cluster.x) * (180./TMath::Pi()) + 360 : atan2(inner_cluster.y, inner_cluster.x) * (180./TMath::Pi()); 
                double Clus_OuterPhiDegree = (outer_cluster.y < 0) ? atan2(outer_cluster.y, outer_cluster.x) * (180./TMath::Pi()) + 360 : atan2(outer_cluster.y, outer_cluster.x) * (180./TMath::Pi()); 

                // note : to reject the hopeless pairs
                if (fabs(Clus_InnerPhiDegree - Clus_OuterPhiDegree) > hopeless_pair_angle_cut) {continue;}

                cluster_pair_vec.push_back({inner_cluster, outer_cluster});
            }
        }

    }
}

void AvgVtxXY::SetUpHistograms(int iteration, int quadrant)
{
    bool insert_check = true;

    // note : inner
    insert_check = h2D_map.insert( std::make_pair(
        Form("h2_%i_%i_InnerPhi_DeltaPhi", iteration, quadrant), 
        new TH2D(
            Form("h2_%i_%i_InnerPhi_DeltaPhi", iteration, quadrant),
            Form("h2_%i_%i_InnerPhi_DeltaPhi;Inner cluster #phi [radian];#Delta#phi [radian]", iteration, quadrant), 
            h2_nbins, Hist2D_angle_xmin, Hist2D_angle_xmax, h2_nbins, Hist2D_DeltaPhi_min, Hist2D_DeltaPhi_max
        )
    ) ).second; if (insert_check == false) {std::cout<<"Error : h2D_map insert failed"<<std::endl; exit(1);}
    insert_check = h2D_map.insert( std::make_pair(
        Form("h2_%i_%i_InnerPhi_DCA", iteration, quadrant), 
        new TH2D(
            Form("h2_%i_%i_InnerPhi_DCA", iteration, quadrant),
            Form("h2_%i_%i_InnerPhi_DCA;Inner cluster #phi [radian];DCA [cm]", iteration, quadrant), 
            h2_nbins, Hist2D_angle_xmin, Hist2D_angle_xmax, h2_nbins, Hist2D_DCA_min, Hist2D_DCA_max
        )
    ) ).second; if (insert_check == false) {std::cout<<"Error : h2D_map insert failed"<<std::endl; exit(1);}

    
    // note : outer
    insert_check = h2D_map.insert( std::make_pair(
        Form("h2_%i_%i_OuterPhi_DeltaPhi", iteration, quadrant), 
        new TH2D(
            Form("h2_%i_%i_OuterPhi_DeltaPhi", iteration, quadrant),
            Form("h2_%i_%i_OuterPhi_DeltaPhi;Outer cluster #phi [radian];#Delta#phi [radian]", iteration, quadrant), 
            h2_nbins, Hist2D_angle_xmin, Hist2D_angle_xmax, h2_nbins, Hist2D_DeltaPhi_min, Hist2D_DeltaPhi_max
        )
    ) ).second; if (insert_check == false) {std::cout<<"Error : h2D_map insert failed"<<std::endl; exit(1);}
    insert_check = h2D_map.insert( std::make_pair(
        Form("h2_%i_%i_OuterPhi_DCA", iteration, quadrant), 
        new TH2D(
            Form("h2_%i_%i_OuterPhi_DCA", iteration, quadrant),
            Form("h2_%i_%i_OuterPhi_DCA;Outer cluster #phi [radian];DCA [cm]", iteration, quadrant), 
            h2_nbins, Hist2D_angle_xmin, Hist2D_angle_xmax, h2_nbins, Hist2D_DCA_min, Hist2D_DCA_max
        )
    ) ).second; if (insert_check == false) {std::cout<<"Error : h2D_map insert failed"<<std::endl; exit(1);}
    


    insert_check = h2D_map.insert( std::make_pair(
        Form("h2_%i_%i_DeltaPhi_DCA", iteration, quadrant), 
        new TH2D(
            Form("h2_%i_%i_DeltaPhi_DCA", iteration, quadrant),
            Form("h2_%i_%i_DeltaPhi_DCA;#Delta#phi [radian]; DCA [cm]", iteration, quadrant), 
            h2_nbins, Hist2D_DeltaPhi_min, Hist2D_DeltaPhi_max, h2_nbins, Hist2D_DCA_min, Hist2D_DCA_max
        )
    ) ).second; if (insert_check == false) {std::cout<<"Error : h2D_map insert failed"<<std::endl; exit(1);}
    

    
    // note: h1
    insert_check = h1D_map.insert( std::make_pair(
        Form("h1_%i_%i_DeltaPhi", iteration, quadrant), 
        new TH1D(
            Form("h1_%i_%i_DeltaPhi", iteration, quadrant),
            Form("h1_%i_%i_DeltaPhi;#Delta#phi [radian]; Entries", iteration, quadrant), 
            h2_nbins, Hist2D_DeltaPhi_min, Hist2D_DeltaPhi_max
        )
    ) ).second; if (insert_check == false) {std::cout<<"Error : h2D_map insert failed"<<std::endl; exit(1);}

    insert_check = h1D_map.insert( std::make_pair(
        Form("h1_%i_%i_DCA", iteration, quadrant), 
        new TH1D(
            Form("h1_%i_%i_DCA", iteration, quadrant),
            Form("h1_%i_%i_DCA;DCA [cm]; Entries", iteration, quadrant), 
            h2_nbins, Hist2D_DCA_min, Hist2D_DCA_max
        )
    ) ).second; if (insert_check == false) {std::cout<<"Error : h2D_map insert failed"<<std::endl; exit(1);}
}

// note : innerphi_delta_phi_fiterror, innerphi_dca_fiterror
std::pair<double,double> AvgVtxXY::RunSingleCandidate(int iteration, int quadrant, std::pair<double,double> assume_center)
{
    std::cout<<"iteration : "<<iteration<<" quadrant : "<<quadrant<<", set center: ("<<assume_center.first<<", "<<assume_center.second<<")"<<std::endl;

    SetUpHistograms(iteration, quadrant);

    for (auto &pair : cluster_pair_vec){
        
        double DCA_sign = calculateAngleBetweenVectors(
            pair.second.x, pair.second.y,
            pair.first.x, pair.first.y,
            assume_center.first, assume_center.second
        );

        double Clus_InnerPhi_Offset_radian = atan2(pair.first.y - assume_center.second,  pair.first.x - assume_center.first);
        double Clus_OuterPhi_Offset_radian = atan2(pair.second.y - assume_center.second, pair.second.x - assume_center.first);

        double DeltaPhi = Clus_OuterPhi_Offset_radian - Clus_InnerPhi_Offset_radian;

        h2D_map[Form("h2_%i_%i_InnerPhi_DeltaPhi", iteration, quadrant)] -> Fill(Clus_InnerPhi_Offset_radian, DeltaPhi);
        h2D_map[Form("h2_%i_%i_InnerPhi_DCA", iteration, quadrant)] -> Fill(Clus_InnerPhi_Offset_radian, DCA_sign);
        h2D_map[Form("h2_%i_%i_OuterPhi_DeltaPhi", iteration, quadrant)] -> Fill(Clus_OuterPhi_Offset_radian, DeltaPhi);
        h2D_map[Form("h2_%i_%i_OuterPhi_DCA", iteration, quadrant)] -> Fill(Clus_OuterPhi_Offset_radian, DCA_sign);
        h2D_map[Form("h2_%i_%i_DeltaPhi_DCA", iteration, quadrant)] -> Fill(DeltaPhi, DCA_sign);

        h1D_map[Form("h1_%i_%i_DeltaPhi", iteration, quadrant)] -> Fill(DeltaPhi);
        h1D_map[Form("h1_%i_%i_DCA", iteration, quadrant)] -> Fill(DCA_sign);
    }

    bool insert_check = true;

    insert_check = h2D_map.insert( std::make_pair(
        Form("h2_%i_%i_InnerPhi_DeltaPhi_bkgrm", iteration, quadrant), 
        (TH2D*)h2D_map[Form("h2_%i_%i_InnerPhi_DeltaPhi", iteration, quadrant)]->Clone(Form("h2_%i_%i_InnerPhi_DeltaPhi_bkgrm", iteration, quadrant))
    ) ).second; if (insert_check == false) {std::cout<<"Error : h2D_map insert failed"<<std::endl; exit(1);}
    insert_check = h2D_map.insert( std::make_pair(
        Form("h2_%i_%i_InnerPhi_DCA_bkgrm", iteration, quadrant), 
        (TH2D*)h2D_map[Form("h2_%i_%i_InnerPhi_DCA", iteration, quadrant)]->Clone(Form("h2_%i_%i_InnerPhi_DCA_bkgrm", iteration, quadrant))
    ) ).second; if (insert_check == false) {std::cout<<"Error : h2D_map insert failed"<<std::endl; exit(1);}

     insert_check = h2D_map.insert( std::make_pair(
        Form("h2_%i_%i_OuterPhi_DeltaPhi_bkgrm", iteration, quadrant), 
        (TH2D*)h2D_map[Form("h2_%i_%i_OuterPhi_DeltaPhi", iteration, quadrant)]->Clone(Form("h2_%i_%i_OuterPhi_DeltaPhi_bkgrm", iteration, quadrant))
    ) ).second; if (insert_check == false) {std::cout<<"Error : h2D_map insert failed"<<std::endl; exit(1);}
    insert_check = h2D_map.insert( std::make_pair(
        Form("h2_%i_%i_OuterPhi_DCA_bkgrm", iteration, quadrant), 
        (TH2D*)h2D_map[Form("h2_%i_%i_OuterPhi_DCA", iteration, quadrant)]->Clone(Form("h2_%i_%i_OuterPhi_DCA_bkgrm", iteration, quadrant))
    ) ).second; if (insert_check == false) {std::cout<<"Error : h2D_map insert failed"<<std::endl; exit(1);}

    TH2D_threshold_advanced_2(h2D_map[Form("h2_%i_%i_InnerPhi_DeltaPhi_bkgrm", iteration, quadrant)], InnerOuterPhi_DeltaPhi_DCA_Threshold);
    TH2D_threshold_advanced_2(h2D_map[Form("h2_%i_%i_InnerPhi_DCA_bkgrm", iteration, quadrant)], InnerOuterPhi_DeltaPhi_DCA_Threshold);
    TH2D_threshold_advanced_2(h2D_map[Form("h2_%i_%i_OuterPhi_DeltaPhi_bkgrm", iteration, quadrant)], InnerOuterPhi_DeltaPhi_DCA_Threshold);
    TH2D_threshold_advanced_2(h2D_map[Form("h2_%i_%i_OuterPhi_DCA_bkgrm", iteration, quadrant)], InnerOuterPhi_DeltaPhi_DCA_Threshold);
    
    std::vector<double> h2InnerPhiDeltaPhi_column_content = SumTH2FColumnContent(h2D_map[Form("h2_%i_%i_InnerPhi_DeltaPhi_bkgrm", iteration, quadrant)]);
    std::vector<double> h2InnerPhiDCA_column_content = SumTH2FColumnContent(h2D_map[Form("h2_%i_%i_InnerPhi_DCA_bkgrm", iteration, quadrant)]);

    insert_check = profile_map.insert( std::make_pair(
        Form("profile_%i_%i_InnerPhi_DeltaPhi_bkgrm", iteration, quadrant), 
        h2D_map[Form("h2_%i_%i_InnerPhi_DeltaPhi_bkgrm", iteration, quadrant)]->ProfileX(Form("profile_%i_%i_InnerPhi_DeltaPhi_bkgrm", iteration, quadrant))
    ) ).second; if (insert_check == false) {std::cout<<"Error : profile_map insert failed"<<std::endl; exit(1);}
    insert_check = profile_map.insert( std::make_pair(
        Form("profile_%i_%i_InnerPhi_DCA_bkgrm", iteration, quadrant), 
        h2D_map[Form("h2_%i_%i_InnerPhi_DCA_bkgrm", iteration, quadrant)]->ProfileX(Form("profile_%i_%i_InnerPhi_DCA_bkgrm", iteration, quadrant))
    ) ).second; if (insert_check == false) {std::cout<<"Error : profile_map insert failed"<<std::endl; exit(1);}

    insert_check = gr_map.insert( std::make_pair(
        Form("gr_%i_%i_InnerPhi_DeltaPhi_bkgrm", iteration, quadrant), 
        new TGraph()
    ) ).second; if (insert_check == false) {std::cout<<"Error : gr_map insert failed"<<std::endl; exit(1);}
    insert_check = gr_map.insert( std::make_pair(
        Form("gr_%i_%i_InnerPhi_DCA_bkgrm", iteration, quadrant), 
        new TGraph()
    ) ).second; if (insert_check == false) {std::cout<<"Error : gr_map insert failed"<<std::endl; exit(1);}
    gr_map[Form("gr_%i_%i_InnerPhi_DeltaPhi_bkgrm", iteration, quadrant)] -> SetName(Form("gr_%i_%i_InnerPhi_DeltaPhi_bkgrm", iteration, quadrant));
    gr_map[Form("gr_%i_%i_InnerPhi_DCA_bkgrm", iteration, quadrant)] -> SetName(Form("gr_%i_%i_InnerPhi_DCA_bkgrm", iteration, quadrant));


    for (int i = 0; i < profile_map[Form("profile_%i_%i_InnerPhi_DeltaPhi_bkgrm", iteration, quadrant)]->GetNbinsX(); i++){
        if (h2InnerPhiDeltaPhi_column_content[i] < InnerPhi_DeltaPhi_DCA_Fit_Tgr_HitContent_Threshold){continue;} // note : in order to remove some remaining background 

        gr_map[Form("gr_%i_%i_InnerPhi_DeltaPhi_bkgrm", iteration, quadrant)] -> SetPoint(
            gr_map[Form("gr_%i_%i_InnerPhi_DeltaPhi_bkgrm", iteration, quadrant)]->GetN(), 
            profile_map[Form("profile_%i_%i_InnerPhi_DeltaPhi_bkgrm", iteration, quadrant)]->GetBinCenter(i+1), 
            profile_map[Form("profile_%i_%i_InnerPhi_DeltaPhi_bkgrm", iteration, quadrant)]->GetBinContent(i+1)
        );
    }
    for (int i = 0; i < profile_map[Form("profile_%i_%i_InnerPhi_DCA_bkgrm", iteration, quadrant)]->GetNbinsX(); i++){
        if (h2InnerPhiDCA_column_content[i] < InnerPhi_DeltaPhi_DCA_Fit_Tgr_HitContent_Threshold){continue;} // note : in order to remove some remaining background 

        gr_map[Form("gr_%i_%i_InnerPhi_DCA_bkgrm", iteration, quadrant)] -> SetPoint(
            gr_map[Form("gr_%i_%i_InnerPhi_DCA_bkgrm", iteration, quadrant)]->GetN(), 
            profile_map[Form("profile_%i_%i_InnerPhi_DCA_bkgrm", iteration, quadrant)]->GetBinCenter(i+1), 
            profile_map[Form("profile_%i_%i_InnerPhi_DCA_bkgrm", iteration, quadrant)]->GetBinContent(i+1)
        );
    }

    // note : do the fit
    fit_innerphi_DeltaPhi -> SetParameter(0, 0); 
    fit_innerphi_DCA -> SetParameter(0, 0);

    gr_map[Form("gr_%i_%i_InnerPhi_DeltaPhi_bkgrm", iteration, quadrant)] -> Fit(fit_innerphi_DeltaPhi, "NQ", "", -M_PI, M_PI);
    gr_map[Form("gr_%i_%i_InnerPhi_DCA_bkgrm", iteration, quadrant)] -> Fit(fit_innerphi_DCA, "NQ", "", -M_PI, M_PI);

    out_iteration = iteration;
    out_quadrant = quadrant;
    out_assume_center_x = assume_center.first;
    out_assume_center_y = assume_center.second;
    out_Fit_InnerPhi_DeltaPhi_RChi2 = fit_innerphi_DeltaPhi -> GetChisquare() / double(fit_innerphi_DeltaPhi -> GetNDF()); 
    out_Fit_InnerPhi_DeltaPhi_Err0 = fit_innerphi_DeltaPhi -> GetParError(0);
    out_Fit_InnerPhi_DeltaPhi_LineY = fit_innerphi_DeltaPhi -> GetParameter(0);
    out_Fit_InnerPhi_DCA_RChi2 = fit_innerphi_DCA -> GetChisquare() / double(fit_innerphi_DCA -> GetNDF());
    out_Fit_InnerPhi_DCA_Err0 = fit_innerphi_DCA -> GetParError(0);
    out_Fit_InnerPhi_DCA_LineY = fit_innerphi_DCA -> GetParameter(0);

    tree_quadrant_detail -> Fill();

    std::cout<<"iteration : "<<iteration<<" quadrant : "<<quadrant<<", FitErrors: ("<<fit_innerphi_DeltaPhi -> GetParError(0)<<", "<<fit_innerphi_DCA -> GetParError(0)<<")"<<std::endl;

    return{
        fit_innerphi_DeltaPhi -> GetParError(0),
        fit_innerphi_DCA -> GetParError(0)
    };

}

void AvgVtxXY::FindVertexQuadrant(int nIteration, double search_Window_half, std::pair<double,double> set_center)
{
    double dynamic_search_window_half = search_Window_half;
    std::pair<double, double> dynamic_center = set_center;

    int small_index;
    std::pair<double,double> small_pair;

    std::vector<std::pair<double,double>> vtx_vec = Get4vtx(dynamic_center,dynamic_search_window_half);

    bool insert_check = true;

    insert_check = gr_map.insert( std::make_pair(
        Form("History_InnerPhi_DeltaPhi_fitErrorAll"), 
        new TGraph()
    ) ).second; if (insert_check == false) {std::cout<<"Error : gr_map insert failed"<<std::endl; exit(1);}
    insert_check = gr_map.insert( std::make_pair(
        Form("History_InnerPhi_DeltaPhi_fitErrorWinner"), 
        new TGraph()
    ) ).second; if (insert_check == false) {std::cout<<"Error : gr_map insert failed"<<std::endl; exit(1);}

    insert_check = gr_map.insert( std::make_pair(
        Form("History_InnerPhi_DCA_fitErrorAll"), 
        new TGraph()
    ) ).second; if (insert_check == false) {std::cout<<"Error : gr_map insert failed"<<std::endl; exit(1);}
    insert_check = gr_map.insert( std::make_pair(
        Form("History_InnerPhi_DCA_fitErrorWinner"), 
        new TGraph()
    ) ).second; if (insert_check == false) {std::cout<<"Error : gr_map insert failed"<<std::endl; exit(1);}

    insert_check = gr_map.insert( std::make_pair(
        Form("History_Winner_index"), 
        new TGraph()
    ) ).second; if (insert_check == false) {std::cout<<"Error : gr_map insert failed"<<std::endl; exit(1);}

    gr_map[Form("History_InnerPhi_DeltaPhi_fitErrorAll")]->SetName(Form("History_InnerPhi_DeltaPhi_fitErrorAll"));
    gr_map[Form("History_InnerPhi_DeltaPhi_fitErrorWinner")]->SetName(Form("History_InnerPhi_DeltaPhi_fitErrorWinner"));
    gr_map[Form("History_InnerPhi_DCA_fitErrorAll")]->SetName(Form("History_InnerPhi_DCA_fitErrorAll"));
    gr_map[Form("History_InnerPhi_DCA_fitErrorWinner")]->SetName(Form("History_InnerPhi_DCA_fitErrorWinner"));
    gr_map[Form("History_Winner_index")]->SetName(Form("History_Winner_index"));


    for (int trial_i = 0; trial_i < nIteration; trial_i++)
    {   
        for (int quadrant_i = 0; quadrant_i < 4; quadrant_i++)
        {
            // note : innerphi_delta_phi_fiterror, innerphi_dca_fiterror
            std::pair<double,double> current_pair = RunSingleCandidate(trial_i, quadrant_i, vtx_vec[quadrant_i]);

            gr_map[Form("History_InnerPhi_DeltaPhi_fitErrorAll")] -> SetPoint(
                gr_map[Form("History_InnerPhi_DeltaPhi_fitErrorAll")]->GetN(), 
                trial_i, 
                current_pair.first
            );

            gr_map[Form("History_InnerPhi_DCA_fitErrorAll")] -> SetPoint(
                gr_map[Form("History_InnerPhi_DCA_fitErrorAll")]->GetN(), 
                trial_i, 
                current_pair.second
            );

            if (quadrant_i == 0)
            {
                small_index = quadrant_i;
                small_pair = current_pair;
            }
            else 
            {
                if (current_pair.first < small_pair.first && current_pair.second < small_pair.second)
                // if (current_pair.second < small_pair.second) // note : only consider the DCA fit error
                {
                    small_index = quadrant_i;
                    small_pair = current_pair;
                }
            }
        }

        gr_map[Form("History_Winner_index")] -> SetPoint(
            gr_map[Form("History_Winner_index")]->GetN(), 
            trial_i, 
            small_index
        );

        gr_map[Form("History_InnerPhi_DeltaPhi_fitErrorWinner")] -> SetPoint(
            gr_map[Form("History_InnerPhi_DeltaPhi_fitErrorWinner")]->GetN(), 
            trial_i, 
            small_pair.first
        );

        gr_map[Form("History_InnerPhi_DCA_fitErrorWinner")] -> SetPoint(
            gr_map[Form("History_InnerPhi_DCA_fitErrorWinner")]->GetN(), 
            trial_i, 
            small_pair.second
        );

        std::cout<<"In FindVertexQuadrant, iteration : "<<trial_i<<" small_index : "<<small_index<<" small_pair : ("<<vtx_vec[small_index].first<<", "<<vtx_vec[small_index].second<<")"<<std::endl;

        // note : generating the new 4 vertex for the next comparison
        // note : start to shrink the square
        if (trial_i != nIteration - 1)
        {
            dynamic_center = {(vtx_vec[small_index].first + dynamic_center.first)/2., (vtx_vec[small_index].second + dynamic_center.second)/2.};
            // cout<<"test : "<<origin.first<<" "<<origin.second<<" length: "<<length<<endl;
            dynamic_search_window_half /= 2.;
            vtx_vec = Get4vtx(dynamic_center,dynamic_search_window_half); 
        }

    }

    final_InnerPhi_DeltaPhi = (TH2D*)h2D_map[Form("h2_%i_%i_InnerPhi_DeltaPhi", nIteration-1, small_index)]->Clone("final_InnerPhi_DeltaPhi");
    final_InnerPhi_DCA = (TH2D*)h2D_map[Form("h2_%i_%i_InnerPhi_DCA", nIteration-1, small_index)]->Clone("final_InnerPhi_DCA");
    final_InnerPhi_DeltaPhi_bkgrm = (TH2D*)h2D_map[Form("h2_%i_%i_InnerPhi_DeltaPhi_bkgrm", nIteration-1, small_index)]->Clone("final_InnerPhi_DeltaPhi_bkgrm");
    final_InnerPhi_DCA_bkgrm = (TH2D*)h2D_map[Form("h2_%i_%i_InnerPhi_DCA_bkgrm", nIteration-1, small_index)]->Clone("final_InnerPhi_DCA_bkgrm");

    final_OuterPhi_DeltaPhi = (TH2D*)h2D_map[Form("h2_%i_%i_OuterPhi_DeltaPhi", nIteration-1, small_index)]->Clone("final_OuterPhi_DeltaPhi");
    final_OuterPhi_DCA = (TH2D*)h2D_map[Form("h2_%i_%i_OuterPhi_DCA", nIteration-1, small_index)]->Clone("final_OuterPhi_DCA");
    final_OuterPhi_DeltaPhi_bkgrm = (TH2D*)h2D_map[Form("h2_%i_%i_OuterPhi_DeltaPhi_bkgrm", nIteration-1, small_index)]->Clone("final_OuterPhi_DeltaPhi_bkgrm");
    final_OuterPhi_DCA_bkgrm = (TH2D*)h2D_map[Form("h2_%i_%i_OuterPhi_DCA_bkgrm", nIteration-1, small_index)]->Clone("final_OuterPhi_DCA_bkgrm");

    final_DeltaPhi_DCA = (TH2D*)h2D_map[Form("h2_%i_%i_DeltaPhi_DCA", nIteration-1, small_index)]->Clone("final_DeltaPhi_DCA");
    final_DeltaPhi_1D = (TH1D*)h1D_map[Form("h1_%i_%i_DeltaPhi", nIteration-1, small_index)]->Clone("final_DeltaPhi");
    final_DCA_1D = (TH1D*)h1D_map[Form("h1_%i_%i_DCA", nIteration-1, small_index)]->Clone("final_DCA");


    out_vtxX_quadrant = (vtx_vec[small_index].first + dynamic_center.first)/2.;
    out_vtxXE_quadrant = fabs(vtx_vec[small_index].first - dynamic_center.first)/2.;
    out_vtxY_quadrant = (vtx_vec[small_index].second + dynamic_center.second)/2.;;
    out_vtxYE_quadrant = fabs(vtx_vec[small_index].second - dynamic_center.second)/2.;    

}

void AvgVtxXY::FindVertexLineFill(std::pair<double,double> set_center, int Nbins, double search_Window_half, double segmentation)
{
    bool insert_check = true;

    insert_check = h2D_map.insert(
        std::make_pair(
            "h2_LineFilled_XY",
            new TH2D(
                "h2_LineFilled_XY",
                "h2_LineFilled_XY;X [cm];Y [cm]",
                Nbins, set_center.first - search_Window_half, set_center.first + search_Window_half,
                Nbins, set_center.second - search_Window_half, set_center.second + search_Window_half
            )
        )
    ).second; if (insert_check == false) {std::cout<<"Error : h2D_map insert failed"<<std::endl; exit(1);}

    // insert_check = h2D_map.insert(
    //     std::make_pair(
    //         "h2_LineFilled_XY_bkgrm",
    //         new TH2D(
    //             "h2_LineFilled_XY_bkgrm",
    //             "h2_LineFilled_XY_bkgrm;X [cm];Y [cm]",
    //             Nbins, set_center.first - search_Window_half, set_center.first + search_Window_half,
    //             Nbins, set_center.second - search_Window_half, set_center.second + search_Window_half
    //         )
    //     )
    // ).second; if (insert_check == false) {std::cout<<"Error : h2D_map insert failed"<<std::endl; exit(1);}

    for (int i = 0; i < cluster_pair_vec.size(); i++){
        double Clus_InnerPhi_Offset = (cluster_pair_vec[i].first.y - set_center.second < 0) ? atan2(cluster_pair_vec[i].first.y - set_center.second, cluster_pair_vec[i].first.x - set_center.first) * (180./TMath::Pi()) + 360 : atan2(cluster_pair_vec[i].first.y - set_center.second, cluster_pair_vec[i].first.x - set_center.first) * (180./TMath::Pi());
        double Clus_OuterPhi_Offset = (cluster_pair_vec[i].second.y - set_center.second < 0) ? atan2(cluster_pair_vec[i].second.y - set_center.second, cluster_pair_vec[i].second.x - set_center.first) * (180./TMath::Pi()) + 360 : atan2(cluster_pair_vec[i].second.y - set_center.second, cluster_pair_vec[i].second.x - set_center.first) * (180./TMath::Pi());

        std::vector<double> DCA_info_vec = calculateDistanceAndClosestPoint(
            cluster_pair_vec[i].first.x, cluster_pair_vec[i].first.y,
            cluster_pair_vec[i].second.x, cluster_pair_vec[i].second.y,
            set_center.first, set_center.second
        );

        if (fabs(Clus_InnerPhi_Offset - Clus_OuterPhi_Offset) < rough_DeltaPhi_cut_degree)
        {
            TH2DSampleLineFill(h2D_map["h2_LineFilled_XY"], segmentation, {cluster_pair_vec[i].first.x, cluster_pair_vec[i].first.y}, {DCA_info_vec[1], DCA_info_vec[2]});
            // std::cout<<"filled checked, cluster pos : "<<cluster_pair_vec[i].first.x<<" "<<cluster_pair_vec[i].first.y<<" "<<cluster_pair_vec[i].second.x<<" "<<cluster_pair_vec[i].second.y<<std::endl;
        }
    }
    
    insert_check = h2D_map.insert(
        std::make_pair(
            "h2_LineFilled_XY_bkgrm",
            (TH2D*) h2D_map["h2_LineFilled_XY"]->Clone("h2_LineFilled_XY_bkgrm")
        )
    ).second; if (insert_check == false) {std::cout<<"Error : h2D_map insert failed"<<std::endl; exit(1);}

    TH2D_threshold_advanced_2(h2D_map["h2_LineFilled_XY_bkgrm"], LineFill_Hist_Threshold);

    out_vtxX_LineFill       = h2D_map["h2_LineFilled_XY_bkgrm"] -> GetMean(1);     // + xy_hist_bkgrm -> GetXaxis() -> GetBinWidth(1) / 2.; // note : the TH2F calculate the GetMean based on the bin center, no need to apply additional offset
    out_vtxXE_LineFill      = h2D_map["h2_LineFilled_XY_bkgrm"] -> GetMeanError(1);
    out_vtxXStdDev_LineFill = h2D_map["h2_LineFilled_XY_bkgrm"] -> GetStdDev(1);
    out_vtxY_LineFill       = h2D_map["h2_LineFilled_XY_bkgrm"] -> GetMean(2);     // + xy_hist_bkgrm -> GetYaxis() -> GetBinWidth(1) / 2.; // note : the TH2F calculate the GetMean based on the bin center, no need to apply additional offset
    out_vtxYE_LineFill      = h2D_map["h2_LineFilled_XY_bkgrm"] -> GetMeanError(2);
    out_vtxYStdDev_LineFill = h2D_map["h2_LineFilled_XY_bkgrm"] -> GetStdDev(2);

    return;
}

void AvgVtxXY::TH2DSampleLineFill(TH2D * hist_in, double segmentation, std::pair<double,double> inner_clu, std::pair<double,double> outer_clu)
{
    double x_min = hist_in -> GetXaxis() -> GetXmin();
    double x_max = hist_in -> GetXaxis() -> GetXmax();
    double y_min = hist_in -> GetYaxis() -> GetXmin();
    double y_max = hist_in -> GetYaxis() -> GetXmax();

    double seg_x, seg_y;
    double angle;
    int n_seg = 0;

    while (true)
    {
        angle = atan2(inner_clu.second-outer_clu.second, inner_clu.first-outer_clu.first);
        seg_x = (n_seg * segmentation) * cos(angle) + outer_clu.first; // note : atan2(y,x), point.first is the radius
        seg_y = (n_seg * segmentation) * sin(angle) + outer_clu.second;
        
        if ( (seg_x > x_min && seg_x < x_max && seg_y > y_min && seg_y < y_max) != true ) {break;}
        hist_in -> Fill(seg_x, seg_y);
        n_seg += 1;
    }

    n_seg = 1;
    while (true)
    {
        angle = atan2(inner_clu.second-outer_clu.second, inner_clu.first-outer_clu.first);
        seg_x = (-1 * n_seg * segmentation) * cos(angle) + outer_clu.first; // note : atan2(y,x), point.first is the radius
        seg_y = (-1 * n_seg * segmentation) * sin(angle) + outer_clu.second;
        
        if ( (seg_x > x_min && seg_x < x_max && seg_y > y_min && seg_y < y_max) != true ) {break;}
        hist_in -> Fill(seg_x, seg_y);
        n_seg += 1;
    }
}

std::vector<double> AvgVtxXY::calculateDistanceAndClosestPoint(double x1, double y1, double x2, double y2, double target_x, double target_y) 
{
    
    if (x1 != x2)
    {
        // Calculate the slope and intercept of the line passing through (x1, y1) and (x2, y2)
        double a = (y2 - y1) / (x2 - x1);
        double b = y1 - a * x1;

        // cout<<"slope : y="<<a<<"x+"<<b<<endl;
        
        // Calculate the closest distance from (target_x, target_y) to the line y = ax + b
        double closest_distance = std::abs(a * target_x - target_y + b) / std::sqrt(a * a + 1);

        // Calculate the coordinates of the closest point (Xc, Yc) on the line y = ax + b
        double Xc = (target_x + a * target_y - a * b) / (a * a + 1);
        double Yc = a * Xc + b;

        return { closest_distance, Xc, Yc };
    }
    else 
    {
        double closest_distance = std::abs(x1 - target_x);
        double Xc = x1;
        double Yc = target_y;

        return { closest_distance, Xc, Yc };
    }
    
    
}

void AvgVtxXY::EndRun()
{
    std::cout<<std::endl;
    std::cout<<"In runnumber : "<<runnumber<<std::endl;
    std::cout<<"In job : "<<process_id<<std::endl;
    std::cout<<"w/ Nevent: "<<run_nEvents<<std::endl;
    std::cout<<"w/ NCluster pair: "<<cluster_pair_vec.size()<<std::endl;
    std::cout<<"Quadrant method: ("<<Form("%.5f +- %.5f [cm], %.5f +- %.5f [cm]", out_vtxX_quadrant, out_vtxXE_quadrant, out_vtxY_quadrant, out_vtxYE_quadrant)<<")"<<std::endl;
    std::cout<<"LineFill method: ("<<Form("%.5f +- %.5f [cm], %.5f +- %.5f [cm]", out_vtxX_LineFill, out_vtxXE_LineFill, out_vtxY_LineFill, out_vtxYE_LineFill)<<")"<<std::endl;
    std::cout<<"Final : "<<Form("(%.5f [cm], %.5f [cm])", (out_vtxX_quadrant + out_vtxX_LineFill)/2., (out_vtxY_quadrant + out_vtxY_LineFill)/2.)<<std::endl;

    file_out -> cd();
    tree_vtxXY -> Fill();
    tree_vtxXY -> Write();
    if (HaveGeoOffsetTag) {tree_geooffset -> Write();}
    tree_quadrant_detail -> Write();
    tree_parameters -> Write();

    for (const auto& pair : h2D_map)
    {
        pair.second -> Write();
    }

    for (const auto& pair : h1D_map)
    {
        pair.second -> Write();
    }

    for (const auto& pair : gr_map)
    {
        pair.second -> Write();
    }

    for (const auto& pair : profile_map)
    {
        pair.second -> Write();
    }

    final_InnerPhi_DeltaPhi -> Write();
    final_InnerPhi_DCA -> Write();
    final_InnerPhi_DeltaPhi_bkgrm -> Write();
    final_InnerPhi_DCA_bkgrm -> Write();
    final_OuterPhi_DeltaPhi -> Write();
    final_OuterPhi_DCA -> Write();
    final_OuterPhi_DeltaPhi_bkgrm -> Write();
    final_OuterPhi_DCA_bkgrm -> Write();
    final_DeltaPhi_DCA -> Write();
    final_DeltaPhi_1D -> Write();
    final_DCA_1D -> Write();

    file_out -> Close();

}

double AvgVtxXY::calculateAngleBetweenVectors(double x1, double y1, double x2, double y2, double targetX, double targetY) {
    // Calculate the vectors vector_1 (point_1 to point_2) and vector_2 (point_1 to target)
    double vector1X = x2 - x1;
    double vector1Y = y2 - y1;

    double vector2X = targetX - x1;
    double vector2Y = targetY - y1;

    // Calculate the cross product of vector_1 and vector_2 (z-component)
    double crossProduct = vector1X * vector2Y - vector1Y * vector2X;
    
    // cout<<" crossProduct : "<<crossProduct<<endl;

    // Calculate the magnitudes of vector_1 and vector_2
    double magnitude1 = std::sqrt(vector1X * vector1X + vector1Y * vector1Y);
    double magnitude2 = std::sqrt(vector2X * vector2X + vector2Y * vector2Y);

    // Calculate the angle in radians using the inverse tangent of the cross product and dot product
    double dotProduct = vector1X * vector2X + vector1Y * vector2Y;

    double angleInRadians = std::atan2(std::abs(crossProduct), dotProduct);
    // Convert the angle from radians to degrees and return it
    double angleInDegrees = angleInRadians * 180.0 / M_PI;
    
    double angleInRadians_new = std::asin( crossProduct/(magnitude1*magnitude2) );
    double angleInDegrees_new = angleInRadians_new * 180.0 / M_PI;
    
    // cout<<"angle : "<<angleInDegrees_new<<endl;

    double DCA_distance = sin(angleInRadians_new) * magnitude2;

    return DCA_distance;
}

void AvgVtxXY::TH2D_threshold_advanced_2(TH2D * hist, double threshold)
{
    // note : this function is to remove the background of the 2D histogram
    // note : but the threshold is given by average of the contents of the top "chosen_bin" bins and timing the threshold
    // todo : here
    double max_cut = 0;
    int chosen_bin = TH2D_threshold_advanced_2_CheckNBins;

    std::vector<float> all_bin_content_vec; all_bin_content_vec.clear();
    for (int xi = 0; xi < hist -> GetNbinsX(); xi++){
        for(int yi = 0; yi < hist -> GetNbinsY(); yi++){
            all_bin_content_vec.push_back(hist -> GetBinContent(xi + 1, yi +1));
        }
    }
    std::vector<unsigned long> ind(all_bin_content_vec.size(),0);
    TMath::Sort(all_bin_content_vec.size(), &all_bin_content_vec[0], &ind[0]);
    for (int i = 0; i < chosen_bin; i++) {max_cut += all_bin_content_vec[ind[i]]; /*cout<<"test : "<<all_bin_content_vec[ind[i]]<<endl;*/}

    max_cut = (max_cut / double(chosen_bin)) * threshold;

    for (int xi = 0; xi < hist -> GetNbinsX(); xi++){
        for(int yi = 0; yi < hist -> GetNbinsY(); yi++){
            if (hist -> GetBinContent(xi + 1, yi +1) < max_cut){ hist -> SetBinContent(xi + 1, yi +1, 0); }
        }
    }
}

std::vector<double> AvgVtxXY::SumTH2FColumnContent(TH2D * hist_in)
{
    std::vector<double> sum_vec; sum_vec.clear();
    for (int i = 0; i < hist_in -> GetNbinsX(); i++){
        double sum = 0;
        for (int j = 0; j < hist_in -> GetNbinsY(); j++){
            sum += hist_in -> GetBinContent(i+1, j+1);
        }
        sum_vec.push_back(sum);
    }
    return sum_vec;
}

std::vector<std::pair<double,double>> AvgVtxXY::Get4vtx(std::pair<double,double> origin, double length)
{
    std::vector<std::pair<double,double>> unit_vtx = {{1,1},{-1,1},{-1,-1},{1,-1}};
    std::vector<std::pair<double,double>> vec_out; vec_out.clear();

    for (std::pair i1 : unit_vtx)
    {
        vec_out.push_back({i1.first * length + origin.first, i1.second * length + origin.second});
    }

    return vec_out;
}