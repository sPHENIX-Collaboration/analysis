#include "ColumnCheck.h"

ColumnCheck::ColumnCheck(
    int process_id_in,
    int runnumber_in,
    int run_nEvents_in,
    std::string input_directory_in,
    std::string input_file_name_in,
    std::string output_directory_in,

    std::string output_file_name_suffix_in,
    std::pair<double, double> vertexXYIncm_in,
    double SetMbinFloat_in,

    std::pair<double, double> VtxZRange_in,
    bool IsZClustering_in,
    bool BcoFullDiffCut_in,
    std::pair<bool, std::pair<double, double>> isClusQA_in, // note : {adc, phi size}

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

    {false, nullptr}, // note : vtxZReweight_in
    BcoFullDiffCut_in,
    false, // note : INTT_vtxZ_QA_in
    isClusQA_in, // note : {adc, phi size}
    false, // note : HaveGeoOffsetTag_in
    {false, 0}, // note : SetRandomHits_in
    false, // note : RandInttZ_in
    ColMulMask_in,

    1 // note : c_type_in
), 
IsZClustering(IsZClustering_in),
VtxZRange(VtxZRange_in),
SetMbinFloat(SetMbinFloat_in) // note : 0-100

{
    PrepareOutPutFileName();
    PrepareOutPutRootFile();
    PrepareHistograms();
}

void ColumnCheck::PrepareOutPutFileName()
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

    output_filename = "ColumnCheck";
    output_filename = (runnumber != -1) ? "Data_" + output_filename : "MC_" + output_filename;
    output_filename += (BcoFullDiffCut && runnumber != -1) ? "_BcoFullDiffCut" : "";
    output_filename += (IsZClustering) ? "_ZClustering" : "";
    output_filename += Form("_Mbin%.0f",SetMbinFloat);
    output_filename += (ColMulMask) ? "_ColMulMask" : "";
    output_filename += Form("_VtxZ%.0fto%.0fcm",VtxZRange.first,VtxZRange.second);
    output_filename += (isClusQA.first) ? Form("_ClusQAAdc%.0fPhiSize%.0f",isClusQA.second.first,isClusQA.second.second) : "";
    output_filename += output_file_name_suffix;
    output_filename += (runnumber != -1) ? Form("_%s_%s.root",runnumber_str.c_str(),job_index.c_str()) : Form("_%s.root",job_index.c_str());
}

void ColumnCheck::PrepareOutPutRootFile()
{
    file_out = new TFile(Form("%s/%s", output_directory.c_str(), output_filename.c_str()), "RECREATE");
}

void ColumnCheck::PrepareHistograms()
{
    h1D_map.clear();
    h2D_map.clear();

    h1D_GoodColMap_ZId = new TH1D("h1D_GoodColMap_ZId","h1D_GoodColMap_ZId;ClusZ [cm];Entries",nZbin, Zmin, Zmax);

    h1D_map["h1D_ClusZ"] = new TH1D("h1D_ClusZ","h1D_ClusZ;ClusZ [cm];Entries",nZbin, Zmin, Zmax);

    for (int i = 0; i < nZbin; i++)
    {
        h2D_map[Form("h2D_ClusCountLayerPhiId_ZId%d",i)] = new TH2D(Form("h2D_ClusCountLayerPhiId_ZId%d",i),Form("h2D_ClusCountLayerPhiId_ZId%d;LayerID;LadderPhiId",i),4,3,7,16,0,16);
    }

    for (int layer_i = 3; layer_i < 7; layer_i++)
    {   
        int phi_max = (layer_i == 3 || layer_i == 4) ? 12 : 16;

        for (int phi_i = 0; phi_i < phi_max; phi_i++)
        {
            h2D_map[Form("h2D_ClusRadiusZID_Layer%d_PhiId%d",layer_i,phi_i)] = new TH2D(Form("h2D_ClusRadiusZID_Layer%d_PhiId%d",layer_i,phi_i),Form("h2D_ClusRadiusZID_Layer%d_PhiId%d;Clus radius [cm]; ClusZ [cm]",layer_i,phi_i), 275, 6, 11.5, nZbin, Zmin, Zmax);
        }
    }

    h2D_map[Form("h2D_ClusRadiusZID_All")] = new TH2D(Form("h2D_ClusRadiusZID_All"),Form("h2D_ClusRadiusZID_All;Clus radius [cm]; ClusZ [cm]"), 275, 6, 11.5, nZbin, Zmin, Zmax);

    for (int i = 0; i < 4; i++){ // note : layer
        h2D_map[Form("h2D_ClusCountPhiIdZId_Layer%d",i+3)] = new TH2D(Form("h2D_ClusCountPhiIdZId_Layer%d",i+3),Form("h2D_ClusCountPhiIdZId_Layer%d;LadderPhiId;ZId",i+3),16,0,16, nZbin, Zmin, Zmax);
    }
}

void ColumnCheck::EvtCleanUp()
{
    evt_sPH_inner_nocolumn_vec.clear();
    evt_sPH_outer_nocolumn_vec.clear();   
}

void ColumnCheck::MainProcess()
{
    for (int i = 0; i < run_nEvents; i++)
    {
        tree_in -> GetEntry(i);

        EvtCleanUp();

        if (i % 10 == 0) {std::cout << "Processing event " << i<<", NClus : "<< ClusX -> size() << std::endl;}

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

        if (MBD_centrality > SetMbinFloat) {continue;}
        if (MBD_z_vtx < VtxZRange.first || MBD_z_vtx > VtxZRange.second) {continue;}
        // =======================================================================================================================================================

        PrepareClusterVec();

        for (int barrel_i = 0; barrel_i < 2; barrel_i++)
        {
            std::vector<ClusHistogram::clu_info> &this_vec = (barrel_i == 0) ? evt_sPH_inner_nocolumn_vec : evt_sPH_outer_nocolumn_vec;

            for (ClusHistogram::clu_info this_clu : this_vec)
            {
                int ZID = h1D_map["h1D_ClusZ"] -> Fill(this_clu.z) - 1;
                if (ZID == -2) {continue;}

                double ClusRadius = sqrt(pow(this_clu.x - vertexXYIncm.first,2) + pow(this_clu.y - vertexXYIncm.second,2));
                double ColumnLength = (this_clu.sensorZID == typeA_sensorZID[0] || this_clu.sensorZID == typeA_sensorZID[1]) ? typeA_sensor_half_length_incm : typeB_sensor_half_length_incm;

                h2D_map[Form("h2D_ClusCountLayerPhiId_ZId%d",ZID)] -> Fill(this_clu.layerID, this_clu.ladderPhiID, 1. / (ColumnLength));
                h2D_map[Form("h2D_ClusCountPhiIdZId_Layer%d",this_clu.layerID)] -> Fill(this_clu.ladderPhiID, this_clu.z, 1. / (ColumnLength));
                h2D_map[Form("h2D_ClusRadiusZID_Layer%d_PhiId%d",this_clu.layerID,this_clu.ladderPhiID)] -> Fill(ClusRadius, this_clu.z, 1. / (ColumnLength));
                h2D_map[Form("h2D_ClusRadiusZID_All")] -> Fill(ClusRadius, this_clu.z, 1. / (ColumnLength));
            }
        }
    }
}

void ColumnCheck::EndRun()
{
    file_out -> cd();

    for (auto &pair : h1D_map)
    {
        pair.second -> Write();
    }

    for (auto &pair : h2D_map)
    {
        pair.second -> Write();
    }

    file_out -> Close();

}