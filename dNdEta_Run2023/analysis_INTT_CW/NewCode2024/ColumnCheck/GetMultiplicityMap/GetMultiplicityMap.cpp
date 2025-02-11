#include "GetMultiplicityMap.h"

GetMultiplicityMap::GetMultiplicityMap(
    int runnumber_in,
    std::string data_directory_in,
    std::string data_file_name_in,
    std::string MC_directory_in,
    std::string MC_file_name_in,
    std::string output_directory_in,

    std::string output_file_name_suffix_in,

    double SetMbinFloat_in,
    std::pair<double, double> VtxZRange_in,
    bool IsZClustering_in,
    bool BcoFullDiffCut_in,
    std::pair<bool, std::pair<double, double>> isClusQA_in
):
    runnumber(runnumber_in),
    data_directory(data_directory_in),
    data_file_name(data_file_name_in),
    MC_directory(MC_directory_in),
    MC_file_name(MC_file_name_in),
    output_directory(output_directory_in),

    output_file_name_suffix(output_file_name_suffix_in),

    SetMbinFloat(SetMbinFloat_in),
    VtxZRange(VtxZRange_in),
    IsZClustering(IsZClustering_in),
    BcoFullDiffCut(BcoFullDiffCut_in),
    isClusQA(isClusQA_in)

{
    std::cout<<1111<<std::endl;
    h1D_target_map.clear();
    h2D_target_map.clear();
    std::cout<<222<<std::endl;
    h1D_target_map = {
        {"h1D_ClusZ", {nullptr,nullptr}}
    };

    for (int layer_i = 3; layer_i < 7; layer_i++)
    {   
        int phi_max = (layer_i == 3 || layer_i == 4) ? 12 : 16;

        for (int phi_i = 0; phi_i < phi_max; phi_i++)
        {
            h2D_target_map[Form("h2D_ClusRadiusZID_Layer%d_PhiId%d",layer_i,phi_i)] = {nullptr,nullptr}; // note : X: Clus radius, Y: ClusZID
        }
    }

    for (int i = 0; i < nZbin; i++)
    {
        h2D_target_map[Form("h2D_ClusCountLayerPhiId_ZId%d",i)] = {nullptr,nullptr}; // note : X: Layer {3 - 7}, Y: PhiId {0 -16}
    }

    std::cout<<555<<std::endl;

    PrepareInputRootFile();
    
    std::cout<<666<<std::endl;

    PrepareOutPutFileName();

    std::cout<<777<<std::endl;
    PrepareOutPutRootFile();

    std::cout<<888<<std::endl;
}

void GetMultiplicityMap::PrepareInputRootFile()
{
    file_in_data = new TFile(Form("%s/%s", data_directory.c_str(), data_file_name.c_str()), "READ");
    file_in_MC = new TFile(Form("%s/%s", MC_directory.c_str(), MC_file_name.c_str()), "READ");

    if (file_in_data -> IsZombie() || file_in_MC -> IsZombie() || !file_in_data || !file_in_MC)
    {
        std::cerr<<"Error : Cannot open the input file"<<std::endl;
        exit(1);
    }

    TFile * temp;

    for (int i = 0; i < 2; i++){ // note : data -> MC
        temp = (i == 0) ? file_in_data : file_in_MC;

        for (TObject* keyAsObj : *temp->GetListOfKeys())
        {
            auto key = dynamic_cast<TKey*>(keyAsObj);
            std::string hist_name  = key->GetName();
            std::string class_name = key->GetClassName();

            if (class_name == "TH1D" && h1D_target_map.find(hist_name) != h1D_target_map.end())
            {
                if (i == 0) {h1D_target_map[hist_name].first = (TH1D*) temp -> Get( hist_name.c_str() );}
                else {h1D_target_map[hist_name].second = (TH1D*) temp -> Get( hist_name.c_str() );}
            }

            if (class_name == "TH2D" && h2D_target_map.find(hist_name) != h2D_target_map.end())
            {
                if (i == 0) {h2D_target_map[hist_name].first = (TH2D*) temp -> Get( hist_name.c_str() );}
                else {h2D_target_map[hist_name].second = (TH2D*) temp -> Get( hist_name.c_str() );}
            }
        }

    }
}

void GetMultiplicityMap::PrepareOutPutFileName()
{
    std::string runnumber_str = std::to_string( runnumber );
    if (runnumber != -1){
        int runnumber_str_len = 8;
        runnumber_str.insert(0, runnumber_str_len - runnumber_str.size(), '0');
    }

    if (output_file_name_suffix.size() > 0 && output_file_name_suffix[0] != '_') {
        output_file_name_suffix = "_" + output_file_name_suffix;
    }

    output_filename = "MulMap";

    output_filename += (BcoFullDiffCut) ? "_BcoFullDiffCut" : "";
    output_filename += (IsZClustering) ? "_ZClustering" : "";
    output_filename += Form("_Mbin%.0f",SetMbinFloat);
    output_filename += Form("_VtxZ%.0fto%.0fcm",VtxZRange.first,VtxZRange.second);
    output_filename += (isClusQA.first) ? Form("_ClusQAAdc%.0fPhiSize%.0f",isClusQA.second.first,isClusQA.second.second) : "";

    output_filename += output_file_name_suffix;
    output_filename += Form("_%s.root",runnumber_str.c_str());
}

void GetMultiplicityMap::PrepareOutPutRootFile()
{
    file_out = new TFile(Form("%s/%s", output_directory.c_str(), output_filename.c_str()), "RECREATE");
}

void GetMultiplicityMap::GetUsedZIDVec()
{
    used_zid_data_vec.clear();
    used_zid_MC_vec.clear();

    // note : data 
    double HalfMaxEntries_data = h1D_target_map["h1D_ClusZ"].first -> GetBinContent( h1D_target_map["h1D_ClusZ"].first -> GetMaximumBin() ) / 2;
    for (int i = 1; i <= h1D_target_map["h1D_ClusZ"].first -> GetNbinsX(); i++)
    {
        if (h1D_target_map["h1D_ClusZ"].first -> GetBinContent(i) > HalfMaxEntries_data)
        {
            used_zid_data_vec.push_back(i-1); // note : start from zero
        }
    }

    // note : MC
    double HalfMaxEntries_MC = h1D_target_map["h1D_ClusZ"].second -> GetBinContent( h1D_target_map["h1D_ClusZ"].second -> GetMaximumBin() ) / 2;
    for (int i = 1; i <= h1D_target_map["h1D_ClusZ"].second -> GetNbinsX(); i++)
    {
        if (h1D_target_map["h1D_ClusZ"].second -> GetBinContent(i) > HalfMaxEntries_MC)
        {
            used_zid_MC_vec.push_back(i-1); // note : start from zero
        }
    }

    if (used_zid_data_vec.size() != used_zid_MC_vec.size())
    {
        std::cerr<<"Error : The number of ZID is different between data and MC"<<std::endl;
        exit(1);
    }

    std::cout<<"used_zid_data_vec.size(): "<<used_zid_data_vec.size()<<std::endl;

    for (int i = 0; i < used_zid_data_vec.size(); i++)
    {
        std::cout<<"Data: ZID : "<<used_zid_data_vec[i]<<". MC: "<<used_zid_MC_vec[i]<<std::endl;

        if (used_zid_data_vec[i] != used_zid_MC_vec[i])
        {
            std::cerr<<"Error : The ZID is different between data and MC"<<std::endl;
            exit(1);
        }
    }

    for (int i = 0; i < nZbin; i++)
    {
        if (std::find(used_zid_data_vec.begin(), used_zid_data_vec.end(), i) == used_zid_data_vec.end())
        {
            std::cout<<"ZID: "<<i<<" is dropped. Data Entries: "
            <<h2D_target_map[Form("h2D_ClusCountLayerPhiId_ZId%d",i)].first -> GetEntries()<<" MC Entries: "
            <<h2D_target_map[Form("h2D_ClusCountLayerPhiId_ZId%d",i)].second -> GetEntries()<<std::endl;

            h2D_target_map.erase(Form("h2D_ClusCountLayerPhiId_ZId%d",i));
        }
    }

}

void GetMultiplicityMap::h2DNormalizedByRadius()
{
    for (int i : used_zid_data_vec)
    {
        TH2D * temp_h2D_data = h2D_target_map[Form("h2D_ClusCountLayerPhiId_ZId%d",i)].first; // note : X: Layer {3 - 7}, Y: PhiId {0 -16}

        for (int layer_i = 3; layer_i < 7; layer_i++)
        {
            int phi_max = (layer_i == 3 || layer_i == 4) ? 12 : 16;

            for (int phi_i = 0; phi_i < phi_max; phi_i++)
            {
                double radius = h2D_target_map[Form("h2D_ClusRadiusZID_Layer%d_PhiId%d",layer_i,phi_i)].first -> GetMean(1); // note : X axis
                double cover_angle = 2 * atan2((sensor_width/2), radius) * (180./M_PI); // note : in radian

                std::cout<<"Data: Layer: "<<layer_i<<", PhiId: "<<phi_i<<", Radius: "<<radius<<", CoverAngle: "<<cover_angle<<std::endl;

                int correspond_bin = temp_h2D_data -> FindBin(layer_i, phi_i);
                double bin_content = temp_h2D_data -> GetBinContent(correspond_bin);

                temp_h2D_data -> SetBinContent(correspond_bin, bin_content / cover_angle);
                temp_h2D_data -> SetBinError(correspond_bin, temp_h2D_data -> GetBinError(correspond_bin) / cover_angle);
            }
        }

        // Division: ------------------------------------------------------------------------------------------------------------------------------------------------
        std::cout<<std::endl;

        TH2D * temp_h2D_MC   = h2D_target_map[Form("h2D_ClusCountLayerPhiId_ZId%d",i)].second; // note : X: Layer {3 - 7}, Y: PhiId {0 -16}
        for (int layer_i = 3; layer_i < 7; layer_i++)
        {
            int phi_max = (layer_i == 3 || layer_i == 4) ? 12 : 16;

            for (int phi_i = 0; phi_i < phi_max; phi_i++)
            {
                double radius = h2D_target_map[Form("h2D_ClusRadiusZID_Layer%d_PhiId%d",layer_i,phi_i)].second -> GetMean(1); // note : X axis
                double cover_angle = 2 * atan2((sensor_width/2), radius) * (180./M_PI); // note : in radian

                std::cout<<"MC: Layer: "<<layer_i<<", PhiId: "<<phi_i<<", Radius: "<<radius<<", CoverAngle: "<<cover_angle<<std::endl;

                int correspond_bin = temp_h2D_MC -> FindBin(layer_i, phi_i);
                double bin_content = temp_h2D_MC -> GetBinContent(correspond_bin);

                temp_h2D_MC -> SetBinContent(correspond_bin, bin_content / cover_angle);
                temp_h2D_MC -> SetBinError(correspond_bin, temp_h2D_MC -> GetBinError(correspond_bin) / cover_angle);
            }
        }
    }
}

void GetMultiplicityMap::h2DNormalized()
{
    for (int i : used_zid_data_vec)
    {
        TH2D * temp_h2D_data = h2D_target_map[Form("h2D_ClusCountLayerPhiId_ZId%d",i)].first; // note : X: Layer {3 - 7}, Y: PhiId {0 -16}
        TH2D * temp_h2D_MC = h2D_target_map[Form("h2D_ClusCountLayerPhiId_ZId%d",i)].second; // note : X: Layer {3 - 7}, Y: PhiId {0 -16}

        temp_h2D_data -> Sumw2(true);
        temp_h2D_MC -> Sumw2(true);

        temp_h2D_data -> Scale(1. / temp_h2D_data -> GetBinContent(temp_h2D_data -> GetMaximumBin()));
        temp_h2D_MC -> Scale(1. / temp_h2D_MC -> GetBinContent(temp_h2D_MC -> GetMaximumBin()));
    }
}

void GetMultiplicityMap::DataMCDivision()
{
    h2D_map.clear();
    h1D_map.clear();
    
    h1D_map["h1D_Ratio_All"] = new TH1D("h1D_Ratio_All","h1D_Ratio_All;Multiplicity Ratio (data/MC);Entries", 100, 0, 2);

    for (int i : used_zid_data_vec)
    {
        TH2D * temp_h2D_data = h2D_target_map[Form("h2D_ClusCountLayerPhiId_ZId%d",i)].first; // note : X: Layer {3 - 7}, Y: PhiId {0 -16}
        TH2D * temp_h2D_MC = h2D_target_map[Form("h2D_ClusCountLayerPhiId_ZId%d",i)].second; // note : X: Layer {3 - 7}, Y: PhiId {0 -16}

        h2D_map[Form("h2D_RatioLayerPhiId_ZId%d",i)] = (TH2D*) temp_h2D_data -> Clone(Form("h2D_RatioLayerPhiId_ZId%d",i));
        h2D_map[Form("h2D_RatioLayerPhiId_ZId%d",i)] -> SetTitle(Form("h2D_RatioLayerPhiId_ZId%d",i));
        h2D_map[Form("h2D_RatioLayerPhiId_ZId%d",i)] -> Divide(temp_h2D_data, temp_h2D_MC, 1, 1);

        h1D_map[Form("h1D_Ratio_ZId%d",i)] = new TH1D(Form("h1D_Ratio_ZId%d",i),Form("h1D_Ratio_ZId%d;Multiplicity Ratio (data/MC);Entries",i), 100, 0, 2);

        for (int layer_i = 3; layer_i < 7; layer_i++)
        {
            int phi_max = (layer_i == 3 || layer_i == 4) ? 12 : 16;

            for (int phi_i = 0; phi_i < phi_max; phi_i++)
            {
                int correspond_bin = h2D_map[Form("h2D_RatioLayerPhiId_ZId%d",i)] -> FindBin(layer_i, phi_i);
                double bin_content = h2D_map[Form("h2D_RatioLayerPhiId_ZId%d",i)] -> GetBinContent(correspond_bin);

                h1D_map[Form("h1D_Ratio_ZId%d",i)] -> Fill(bin_content);
                h1D_map["h1D_Ratio_All"] -> Fill(bin_content);
            }
        }
    }
}

void GetMultiplicityMap::PrepareMulMap()
{
    h2D_MulMap = new TH2D("h2D_MulMap","h2D_MulMap;(LayerID-3) * 20 + PhiId;ZID", 80, 0, 80, nZbin, Zmin, Zmax);
    h2D_RatioMap = new TH2D("h2D_RatioMap","h2D_RatioMap;(LayerID-3) * 20 + PhiId;ZID", 80, 0, 80, nZbin, Zmin, Zmax);

    for (int i : used_zid_data_vec) // note : only good ZID are here 
    {

        for (int layer_i = 3; layer_i < 7; layer_i++)
        {
            int phi_max = (layer_i == 3 || layer_i == 4) ? 12 : 16;

            for (int phi_i = 0; phi_i < phi_max; phi_i++)
            {
                int correspond_bin = h2D_map[Form("h2D_RatioLayerPhiId_ZId%d",i)] -> FindBin(layer_i, phi_i);
                double column_ratio = h2D_map[Form("h2D_RatioLayerPhiId_ZId%d",i)] -> GetBinContent(correspond_bin);
                column_ratio = (column_ratio != column_ratio) ? 0 : column_ratio;

                int index_x = (layer_i - 3) * 20 + phi_i + 1; // note : bin index, start at 1
                int index_y = i + 1; // note : bin index, start at 1

                std::cout<<"ZID: "<<i<<", Layer: "<<layer_i<<", PhiId: "<<phi_i<<", index_x: "<<index_x<<", index_y: "<<index_y<<", Ratio: "<<column_ratio<<std::endl;

                h2D_RatioMap -> SetBinContent(index_x, index_y, column_ratio);

                if (column_ratio >= Ratio_cut_pair.first && column_ratio <= Ratio_cut_pair.second)
                {
                    h2D_MulMap -> SetBinContent(index_x, index_y, 1);
                }
            }
        }
    }
}

void GetMultiplicityMap::EndRun()
{
    file_out -> cd();

    h2D_RatioMap -> Write();
    h2D_MulMap -> Write();


    for (auto pair : h2D_target_map){
        pair.second.first -> Write(Form("%s_data",pair.first.c_str()));
        pair.second.second -> Write(Form("%s_MC",pair.first.c_str()));
    }
    for (auto pair : h2D_map){
        pair.second -> Write();
    }

    for (auto pair : h1D_map){
        pair.second -> Write();
    }

    for (auto pair : h1D_target_map){
        pair.second.first -> Write(Form("%s_data",pair.first.c_str()));
        pair.second.second -> Write(Form("%s_MC",pair.first.c_str()));
    }


    file_out -> Close();
}