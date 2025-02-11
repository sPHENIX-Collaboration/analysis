#include "sPhenixStyle.C"

void GetH2D(
    std::map<std::string,std::pair<TH2D*,TH2D*>> &input_map,
    TFile * input_data_file,
    TFile * input_mc_file
)
{
    if (input_data_file == nullptr)
    {
        std::cout << "Error: input_data_file can not be opened" << std::endl;
        exit(1);
    }   

    if (input_mc_file == nullptr)
    {
        std::cout << "Error: input_mc_file can not be opened" << std::endl;
        exit(1);
    }


    for (TObject* keyAsObj : *input_data_file->GetListOfKeys())
    {
        auto key = dynamic_cast<TKey*>(keyAsObj);
        std::string hist_name  = key->GetName();
        std::string class_name = key->GetClassName();

        if (class_name == "TH2D" && input_map.find(hist_name) != input_map.end())
        {
            input_map[hist_name.c_str()].first = (TH2D*) input_data_file -> Get( hist_name.c_str() );
        }
    }

    for (TObject* keyAsObj : *input_mc_file->GetListOfKeys())
    {
        auto key = dynamic_cast<TKey*>(keyAsObj);
        std::string hist_name  = key->GetName();
        std::string class_name = key->GetClassName();

        if (class_name == "TH2D" && input_map.find(hist_name) != input_map.end())
        {
            input_map[hist_name.c_str()].second = (TH2D*) input_mc_file -> Get( hist_name.c_str() );
        }
    }

}

std::pair<int,int> get_selected_bin_region(TH1D * hist_in, std::pair<double, double> range_in)
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


void GetH1D(
    std::map<std::string,std::pair<TH1D*,TH1D*>> &input_map,
    TFile * input_data_file,
    TFile * input_mc_file
)
{
    if (input_data_file == nullptr)
    {
        std::cout << "Error: input_data_file can not be opened" << std::endl;
        exit(1);
    }   

    if (input_mc_file == nullptr)
    {
        std::cout << "Error: input_mc_file can not be opened" << std::endl;
        exit(1);
    }


    for (TObject* keyAsObj : *input_data_file->GetListOfKeys())
    {
        auto key = dynamic_cast<TKey*>(keyAsObj);
        std::string hist_name  = key->GetName();
        std::string class_name = key->GetClassName();

        if (class_name == "TH1D" && input_map.find(hist_name) != input_map.end())
        {
            input_map[hist_name.c_str()].first = (TH1D*) input_data_file -> Get( hist_name.c_str() );
        }
    }

    for (TObject* keyAsObj : *input_mc_file->GetListOfKeys())
    {
        auto key = dynamic_cast<TKey*>(keyAsObj);
        std::string hist_name  = key->GetName();
        std::string class_name = key->GetClassName();

        if (class_name == "TH1D" && input_map.find(hist_name) != input_map.end())
        {
            input_map[hist_name.c_str()].second = (TH1D*) input_mc_file -> Get( hist_name.c_str() );
        }
    }

}


TH1D * GetReweighting_hist(string input_map_directory, string map_name)
{
  TFile * file_in = TFile::Open(Form("%s", input_map_directory.c_str()));
  TH1D * h1D_INTT_vtxZ_reweighting = (TH1D*)file_in->Get(map_name.c_str()); // todo : the map of the vtxZ reweighting
  return h1D_INTT_vtxZ_reweighting;
}

void ScaleH2D(TH2D * hist_in, double scale_factor = std::nan(""))
{
    // double NonZero_Smallest = std::nan("");

    // for (int i = 1; i <= hist_in->GetNbinsX(); i++)
    // {
    //     for (int j = 1; j <= hist_in->GetNbinsY(); j++)
    //     {
    //         if (NonZero_Smallest != NonZero_Smallest && hist_in->GetBinContent(i,j) != 0)
    //         {
    //             NonZero_Smallest = hist_in->GetBinContent(i,j);
    //         }
    //         else if (hist_in->GetBinContent(i,j) != 0 && hist_in->GetBinContent(i,j) < NonZero_Smallest)
    //         {
    //             NonZero_Smallest = hist_in->GetBinContent(i,j);
            
    //         }
    //     }
    // }

    // hist_in -> Scale(1.0/NonZero_Smallest);

    if (scale_factor != scale_factor)
    {
        // scale_factor = hist_in->GetBinContent(hist_in->GetMaximumBin());
        scale_factor = hist_in->Integral(-1,-1,-1,-1);
        std::cout<<"scale_factor not set, set to: "<<scale_factor<<std::endl;
    }
    
    hist_in -> Scale(1.0/scale_factor);


    // hist_in -> Scale(1.0/hist_in->Integral(-1,-1,-1,-1));
}

std::pair<TH1D * , double> ScaleH2D_by_H1D(TH2D * h2D_in, TH1D* h1D_reweight)
{
    // todo : assumed input h2D:
    // todo : h2D_1: width 5 cm, range [-10, 10] cm
    // todo : h2D_2: width 1 cm, range [-10, 10] cm

    // todo : assumed input h1D:
    // todo : h1D: width 1 cm, range [-45, 45] cm

    int h2D_NbinsY = h2D_in->GetNbinsY();
    double h2D_YLow = h2D_in->GetYaxis()->GetXmin();
    double h2D_YUp = h2D_in->GetYaxis()->GetXmax();
    double h2D_YBinWidth = h2D_in->GetYaxis()->GetBinWidth(1);

    int h1D_NbinsX = h1D_reweight->GetNbinsX();
    double h1D_XLow = h1D_reweight->GetXaxis()->GetXmin();
    double h1D_XUp = h1D_reweight->GetXaxis()->GetXmax();
    double h1D_XBinWidth = h1D_reweight->GetXaxis()->GetBinWidth(1);

    if (h1D_XBinWidth != 1){
        std::cout<<"Error: h1D_XBinWidth != 1"<<std::endl;
        std::cout<<"h1D_XBinWidth: "<<h1D_XBinWidth<<std::endl;
        exit(1);
    }

    double h1D_target_XLow = h2D_YLow;
    double h1D_target_XUp = h2D_YUp;
    double h1D_target_BinWidth = h1D_XBinWidth; // todo : assumed
    int    h1D_target_NbinsX = (h1D_target_XUp - h1D_target_XLow) / h1D_target_BinWidth;

    std::cout<<std::endl;
    std::cout<<"h2D source: "  <<h2D_NbinsY<<", "<<h2D_YLow<<", "<<h2D_YUp<<", BinWidth: "<<h2D_YBinWidth<<std::endl;
    std::cout<<"h1D reweight: "<<h1D_NbinsX<<", "<<h1D_XLow<<", "<<h1D_XUp<<", BinWidth: "<<h1D_XBinWidth<<std::endl;
    std::cout<<"h1D target: "<<h1D_target_NbinsX<<", "<<h1D_target_XLow<<", "<<h1D_target_XUp<<", BinWidth: "<<h1D_target_BinWidth<<std::endl;

    double BinWdith_Ratio_h2D_to_h1D_reweight_rebin = h2D_YBinWidth / h1D_target_BinWidth;
    std::cout<<"BinWdith_Ratio_h2D_to_h1D_reweight_rebin: "<<BinWdith_Ratio_h2D_to_h1D_reweight_rebin<<std::endl;

    TH1D * h1D_reweight_rebin = new TH1D("","",h1D_target_NbinsX, h1D_target_XLow, h1D_target_XUp);
    h1D_reweight_rebin -> Reset("ICESM");

    for (int i = 1; i <= h1D_reweight_rebin->GetNbinsX(); i++)
    {
        double BinCenterX = h1D_reweight_rebin->GetXaxis()->GetBinCenter(i);
        int corresponding_bin = h1D_reweight->FindBin(BinCenterX);

        h1D_reweight_rebin -> SetBinContent(i, h1D_reweight->GetBinContent(corresponding_bin));
        h1D_reweight_rebin -> SetBinError(i, h1D_reweight->GetBinError(corresponding_bin));
    }

    h1D_reweight_rebin -> Rebin(BinWdith_Ratio_h2D_to_h1D_reweight_rebin);
    std::cout<<"h1D_reweight_rebin BinWidth post Rebin: "<<h1D_reweight_rebin->GetXaxis()->GetBinWidth(1)<<std::endl;

    TF1 * f1 = new TF1("","pol0",-500,500);
    f1->SetParameter(0,h1D_reweight_rebin->GetBinContent(h1D_reweight_rebin->GetMaximumBin()));
    h1D_reweight_rebin -> Fit(f1,"NQ");

    for (int i = 1; i <= h2D_in->GetNbinsX(); i++)
    {
        for (int j = 1; j <= h2D_in->GetNbinsY(); j++)
        {
            double BinCenterY = h2D_in->GetYaxis()->GetBinCenter(j);
            int corresponding_bin = h1D_reweight_rebin->FindBin(BinCenterY);
            
            double correcting_factor = h1D_reweight_rebin->GetBinContent(corresponding_bin) / f1->GetParameter(0);

            h2D_in -> SetBinContent(i,j, h2D_in->GetBinContent(i,j) / correcting_factor);
            h2D_in -> SetBinError(i,j, h2D_in->GetBinError(i,j) / correcting_factor);
        }
    }

    return std::make_pair(h1D_reweight_rebin, f1->GetParameter(0));

}

void H2DCountCarryOver(TH2D * wider_hist, TH2D * narrower_hist)
{
    for (int i = 1; i <= narrower_hist->GetNbinsX(); i++)
    {
        for (int j = 1; j <= narrower_hist->GetNbinsY(); j++)
        {   
            int corresponding_bin = wider_hist->FindBin(narrower_hist->GetXaxis()->GetBinCenter(i), narrower_hist->GetYaxis()->GetBinCenter(j));

            narrower_hist->SetBinContent(i, j, wider_hist->GetBinContent(corresponding_bin));
            narrower_hist->SetBinError(i, j, wider_hist->GetBinError(corresponding_bin));
        }
    }
}

void FindGoodRegion(TH2D * h2D_GeoAcce, TH2D * h2D_selected, double threshold)
{
    if (h2D_GeoAcce->GetNbinsX() != h2D_selected->GetNbinsX() || h2D_GeoAcce->GetNbinsY() != h2D_selected->GetNbinsY())
    {
        std::cout<<"Error: h2D_GeoAcce and h2D_selected have different binning"<<std::endl;
        exit(1);
    }

    h2D_selected -> Reset("ICESM");

    for (int i = 1; i <= h2D_GeoAcce->GetNbinsX(); i++)
    {
        for (int j = 1; j <= h2D_GeoAcce->GetNbinsY(); j++)
        {
            if (fabs(1. - h2D_GeoAcce->GetBinContent(i,j)) < threshold)
            {
                h2D_selected -> SetBinContent(i,j, h2D_GeoAcce->GetBinContent(i,j));
                h2D_selected -> SetBinError(i,j, h2D_GeoAcce->GetBinError(i,j));
            }
        }
    }


}

void SetNonZeroEntryToOne(TH2D * hist_in)
{
    for (int i = 1; i <= hist_in->GetNbinsX(); i++)
    {
        for (int j = 1; j <= hist_in->GetNbinsY(); j++)
        {
            if (hist_in->GetBinContent(i,j) > 0)
            {
                hist_in->SetBinContent(i,j,1);
                hist_in->SetBinError(i,j,1);
            }
            else {
                hist_in->SetBinContent(i,j,0);
                hist_in->SetBinError(i,j,0);
            }
        }
    }

}

void h2D_rebin(TH2D * hist_in, double target_X_width, double target_Y_width)
{
    int input_nbinX = hist_in->GetNbinsX();
    double input_xlow = hist_in->GetXaxis()->GetXmin();
    double input_xup = hist_in->GetXaxis()->GetXmax();
    double input_xwidth = hist_in->GetXaxis()->GetBinWidth(1);

    int input_nbinY = hist_in->GetNbinsY();
    double input_ylow = hist_in->GetYaxis()->GetXmin();
    double input_yup = hist_in->GetYaxis()->GetXmax();
    double input_ywidth = hist_in->GetYaxis()->GetBinWidth(1);

    int target_groupX = target_X_width / input_xwidth;
    int target_groupY = target_Y_width / input_ywidth;

    std::cout<<"Input hist 2D: X {"<<input_nbinX<<", "<<input_xlow<<", "<<input_xup<<"}, Width: "<<input_xwidth<<std::endl;
    std::cout<<"Input hist 2D: Y {"<<input_nbinY<<", "<<input_ylow<<", "<<input_yup<<"}, Width: "<<input_ywidth<<std::endl;
    std::cout<<"Target Width: X "<<target_X_width<<", Y "<<target_Y_width<<std::endl;
    std::cout<<"Target Group: X "<<target_groupX<<", Y "<<target_groupY<<std::endl;

    hist_in->Rebin2D(target_groupX, target_groupY);
}

void SetNegativeEntryToZero(TH2D * hist_in)
{
    for (int i = 1; i <= hist_in->GetNbinsX(); i++)
    {
        for (int j = 1; j <= hist_in->GetNbinsY(); j++)
        {
            if (hist_in->GetBinContent(i,j) <= 0)
            {
                hist_in->SetBinContent(i,j,0);
                hist_in->SetBinError(i,j,0);
            }
        }
    }
}

int GetGeoAccepCorr()
{
    int Mbin = 70;

    std::string data_input_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Dec042024/completed/Run3/EvtVtxZ/GeoAcc_RandZ10cm_FineBin_GoodCol/completed/Maps/completed";
    std::string data_input_filename = "Data_GeoAccep_Mbin70_00054280_00000.root";

    std::string mc_input_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_Ntuple_HIJING_ana443_20241102/Run3/EvtVtxZ/GeoAcc_RandZ10cm_FineBin_GoodCol/completed/Maps/completed"; 
    std::string mc_input_filename = "MC_GeoAccep_Mbin70_00000.root";

    std::string output_directory = data_input_directory; // note : auto_folder_creation
    std::string output_filename = Form("GeoAccepMap_Mbin%d.root",Mbin);

    // std::string vtxZReWeighting_input_directory = "/sphenix/user/ChengWei/INTT/INTT_dNdeta_repo/NewCode2024/PrepareAllDist/vtxZDist_comp/plots/Test_20241225_withVtxZQA/INTTvtxZReWeight.root";
    // std::string map_name = "HIJING_noZWeight_WithVtxZQA_Inclusive70";

    std::string plot_label = "Internal";

    double rebin_eta_width = 0.2;
    double rebin_vtxZ_width = 2.0; // note : [cm], Y axis
    double good_region_ratio = 0.1;

    // Division : ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    std::map<std::string,std::pair<TH2D*,TH2D*>> target_h2D_map = { // note : {data, MC}
        {"h2D_RotatedBkg_EtaVtxZ_FineBin", {nullptr, nullptr}},
        {"h2D_RotatedBkg_EtaVtxZ_rotated_FineBin", {nullptr, nullptr}},

        {"h2D_typeA_RotatedBkg_EtaVtxZ_FineBin", {nullptr, nullptr}},
        {"h2D_typeA_RotatedBkg_EtaVtxZ_rotated_FineBin", {nullptr, nullptr}},

        {"h2D_BestPair_EtaVtxZ", {nullptr, nullptr}},
        {"h2D_typeA_BestPair_EtaVtxZ", {nullptr, nullptr}}
    };

    std::map<std::string,std::pair<TH2D*,TH2D*>> NorCount_h2D_map; NorCount_h2D_map.clear();
    std::map<std::string,std::pair<TH2D*,TH2D*>> Rebin_h2D_map; Rebin_h2D_map.clear();

    system(Form("if [ ! -d %s ]; then mkdir -p %s; fi", output_directory.c_str(), output_directory.c_str()));

    // TH1D * h1D_INTT_vtxZ_reweighting = GetReweighting_hist(vtxZReWeighting_input_directory, map_name);
    TFile * data_file_in = TFile::Open(Form("%s/%s", data_input_directory.c_str(), data_input_filename.c_str()));
    TFile * mc_file_in = TFile::Open(Form("%s/%s", mc_input_directory.c_str(), mc_input_filename.c_str()));

    GetH2D(
        target_h2D_map,
        data_file_in,
        mc_file_in
    );

    for (auto &pair : target_h2D_map)
    {
        NorCount_h2D_map[pair.first] = {
            (TH2D*) pair.second.first -> Clone(Form("NorCount_%s", pair.second.first->GetName())),
            (TH2D*) pair.second.second -> Clone(Form("NorCount_%s", pair.second.second->GetName()))
        };
    }

    NorCount_h2D_map["h2D_RotatedBkg_EtaVtxZ_FineBin"].first  -> Add(NorCount_h2D_map["h2D_RotatedBkg_EtaVtxZ_FineBin"].first,  NorCount_h2D_map["h2D_RotatedBkg_EtaVtxZ_rotated_FineBin"].first, 1, -1);
    NorCount_h2D_map["h2D_RotatedBkg_EtaVtxZ_FineBin"].second -> Add(NorCount_h2D_map["h2D_RotatedBkg_EtaVtxZ_FineBin"].second, NorCount_h2D_map["h2D_RotatedBkg_EtaVtxZ_rotated_FineBin"].second, 1, -1);

    NorCount_h2D_map["h2D_typeA_RotatedBkg_EtaVtxZ_FineBin"].first  -> Add(NorCount_h2D_map["h2D_typeA_RotatedBkg_EtaVtxZ_FineBin"].first,  NorCount_h2D_map["h2D_typeA_RotatedBkg_EtaVtxZ_rotated_FineBin"].first, 1, -1);
    NorCount_h2D_map["h2D_typeA_RotatedBkg_EtaVtxZ_FineBin"].second -> Add(NorCount_h2D_map["h2D_typeA_RotatedBkg_EtaVtxZ_FineBin"].second, NorCount_h2D_map["h2D_typeA_RotatedBkg_EtaVtxZ_rotated_FineBin"].second, 1, -1);


    SetNegativeEntryToZero(NorCount_h2D_map["h2D_RotatedBkg_EtaVtxZ_FineBin"].first);
    SetNegativeEntryToZero(NorCount_h2D_map["h2D_RotatedBkg_EtaVtxZ_FineBin"].second);

    SetNegativeEntryToZero(NorCount_h2D_map["h2D_typeA_RotatedBkg_EtaVtxZ_FineBin"].first);
    SetNegativeEntryToZero(NorCount_h2D_map["h2D_typeA_RotatedBkg_EtaVtxZ_FineBin"].second);

    TH2D * h2D_RotatedBkg_EtaVtxZ_FineBin_SigCount_data       = (TH2D*) NorCount_h2D_map["h2D_RotatedBkg_EtaVtxZ_FineBin"].first -> Clone("h2D_RotatedBkg_EtaVtxZ_FineBin_SigCount_data");
    TH2D * h2D_RotatedBkg_EtaVtxZ_FineBin_SigCount_MC         = (TH2D*) NorCount_h2D_map["h2D_RotatedBkg_EtaVtxZ_FineBin"].second -> Clone("h2D_RotatedBkg_EtaVtxZ_FineBin_SigCount_MC");
    TH2D * h2D_typeA_RotatedBkg_EtaVtxZ_FineBin_SigCount_data = (TH2D*) NorCount_h2D_map["h2D_typeA_RotatedBkg_EtaVtxZ_FineBin"].first -> Clone("h2D_typeA_RotatedBkg_EtaVtxZ_FineBin_SigCount_data");
    TH2D * h2D_typeA_RotatedBkg_EtaVtxZ_FineBin_SigCount_MC   = (TH2D*) NorCount_h2D_map["h2D_typeA_RotatedBkg_EtaVtxZ_FineBin"].second -> Clone("h2D_typeA_RotatedBkg_EtaVtxZ_FineBin_SigCount_MC");

    SetNonZeroEntryToOne(NorCount_h2D_map["h2D_RotatedBkg_EtaVtxZ_FineBin"].first);
    SetNonZeroEntryToOne(NorCount_h2D_map["h2D_RotatedBkg_EtaVtxZ_FineBin"].second);

    SetNonZeroEntryToOne(NorCount_h2D_map["h2D_typeA_RotatedBkg_EtaVtxZ_FineBin"].first);
    SetNonZeroEntryToOne(NorCount_h2D_map["h2D_typeA_RotatedBkg_EtaVtxZ_FineBin"].second);

    SetNonZeroEntryToOne(NorCount_h2D_map["h2D_BestPair_EtaVtxZ"].first);
    SetNonZeroEntryToOne(NorCount_h2D_map["h2D_BestPair_EtaVtxZ"].second);

    SetNonZeroEntryToOne(NorCount_h2D_map["h2D_typeA_BestPair_EtaVtxZ"].first);
    SetNonZeroEntryToOne(NorCount_h2D_map["h2D_typeA_BestPair_EtaVtxZ"].second);

    // Division : ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    Rebin_h2D_map["h2D_RotatedBkg_EtaVtxZ_FineBin"] = {
        (TH2D*) NorCount_h2D_map["h2D_RotatedBkg_EtaVtxZ_FineBin"].first -> Clone("NorCount_h2D_RotatedBkg_EtaVtxZ_FineBin_Rebin"),
        (TH2D*) NorCount_h2D_map["h2D_RotatedBkg_EtaVtxZ_FineBin"].second -> Clone("NorCount_h2D_RotatedBkg_EtaVtxZ_FineBin_Rebin")
    };

    Rebin_h2D_map["h2D_typeA_RotatedBkg_EtaVtxZ_FineBin"] = {
        (TH2D*) NorCount_h2D_map["h2D_typeA_RotatedBkg_EtaVtxZ_FineBin"].first -> Clone("NorCount_h2D_typeA_RotatedBkg_EtaVtxZ_FineBin_Rebin"),
        (TH2D*) NorCount_h2D_map["h2D_typeA_RotatedBkg_EtaVtxZ_FineBin"].second -> Clone("NorCount_h2D_typeA_RotatedBkg_EtaVtxZ_FineBin_Rebin")
    };

    Rebin_h2D_map["h2D_BestPair_EtaVtxZ"] = {
        (TH2D*) NorCount_h2D_map["h2D_BestPair_EtaVtxZ"].first -> Clone("NorCount_h2D_BestPair_EtaVtxZ_Rebin"),
        (TH2D*) NorCount_h2D_map["h2D_BestPair_EtaVtxZ"].second -> Clone("NorCount_h2D_BestPair_EtaVtxZ_Rebin")
    };

    Rebin_h2D_map["h2D_typeA_BestPair_EtaVtxZ"] = {
        (TH2D*) NorCount_h2D_map["h2D_typeA_BestPair_EtaVtxZ"].first -> Clone("NorCount_h2D_typeA_BestPair_EtaVtxZ_Rebin"),
        (TH2D*) NorCount_h2D_map["h2D_typeA_BestPair_EtaVtxZ"].second -> Clone("NorCount_h2D_typeA_BestPair_EtaVtxZ_Rebin")
    };

    h2D_rebin(Rebin_h2D_map["h2D_RotatedBkg_EtaVtxZ_FineBin"].first,  rebin_eta_width, rebin_vtxZ_width);
    h2D_rebin(Rebin_h2D_map["h2D_RotatedBkg_EtaVtxZ_FineBin"].second, rebin_eta_width, rebin_vtxZ_width);

    h2D_rebin(Rebin_h2D_map["h2D_typeA_RotatedBkg_EtaVtxZ_FineBin"].first,  rebin_eta_width, rebin_vtxZ_width);
    h2D_rebin(Rebin_h2D_map["h2D_typeA_RotatedBkg_EtaVtxZ_FineBin"].second, rebin_eta_width, rebin_vtxZ_width);

    h2D_rebin(Rebin_h2D_map["h2D_BestPair_EtaVtxZ"].first,  rebin_eta_width, rebin_vtxZ_width);
    h2D_rebin(Rebin_h2D_map["h2D_BestPair_EtaVtxZ"].second, rebin_eta_width, rebin_vtxZ_width);

    h2D_rebin(Rebin_h2D_map["h2D_typeA_BestPair_EtaVtxZ"].first,  rebin_eta_width, rebin_vtxZ_width);
    h2D_rebin(Rebin_h2D_map["h2D_typeA_BestPair_EtaVtxZ"].second, rebin_eta_width, rebin_vtxZ_width);

    // Division : ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    TH2D * h2D_RotatedBkg_EtaVtxZ_ratio = (TH2D*) Rebin_h2D_map["h2D_RotatedBkg_EtaVtxZ_FineBin"].first -> Clone("h2D_RotatedBkg_EtaVtxZ_Rebin_ratio");
    h2D_RotatedBkg_EtaVtxZ_ratio -> Sumw2(true);
    h2D_RotatedBkg_EtaVtxZ_ratio -> Divide(Rebin_h2D_map["h2D_RotatedBkg_EtaVtxZ_FineBin"].second); // note : data / MC

    TH2D * h2D_typeA_RotatedBkg_EtaVtxZ_ratio = (TH2D*) Rebin_h2D_map["h2D_typeA_RotatedBkg_EtaVtxZ_FineBin"].first -> Clone("h2D_typeA_RotatedBkg_EtaVtxZ_Rebin_ratio");
    h2D_typeA_RotatedBkg_EtaVtxZ_ratio -> Sumw2(true);
    h2D_typeA_RotatedBkg_EtaVtxZ_ratio -> Divide(Rebin_h2D_map["h2D_typeA_RotatedBkg_EtaVtxZ_FineBin"].second); // note : data / MC

    TH2D * h2D_BestPair_EtaVtxZ_ratio = (TH2D*) Rebin_h2D_map["h2D_BestPair_EtaVtxZ"].first -> Clone("h2D_BestPair_EtaVtxZ_Rebin_ratio");
    h2D_BestPair_EtaVtxZ_ratio -> Sumw2(true);
    h2D_BestPair_EtaVtxZ_ratio -> Divide(Rebin_h2D_map["h2D_BestPair_EtaVtxZ"].second); // note : data / MC

    TH2D * h2D_typeA_BestPair_EtaVtxZ_ratio = (TH2D*) Rebin_h2D_map["h2D_typeA_BestPair_EtaVtxZ"].first -> Clone("h2D_typeA_BestPair_EtaVtxZ_Rebin_ratio");
    h2D_typeA_BestPair_EtaVtxZ_ratio -> Sumw2(true);
    h2D_typeA_BestPair_EtaVtxZ_ratio -> Divide(Rebin_h2D_map["h2D_typeA_BestPair_EtaVtxZ"].second); // note : data / MC

    // Division : ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    TH2D * h2D_RotatedBkg_EtaVtxZ_Selected = (TH2D*) h2D_RotatedBkg_EtaVtxZ_ratio -> Clone("h2D_RotatedBkg_EtaVtxZ_Rebin_Selected");
    FindGoodRegion(h2D_RotatedBkg_EtaVtxZ_ratio, h2D_RotatedBkg_EtaVtxZ_Selected, good_region_ratio);

    TH2D * h2D_typeA_RotatedBkg_EtaVtxZ_Selected = (TH2D*) h2D_typeA_RotatedBkg_EtaVtxZ_ratio -> Clone("h2D_typeA_RotatedBkg_EtaVtxZ_Rebin_Selected");
    FindGoodRegion(h2D_typeA_RotatedBkg_EtaVtxZ_ratio, h2D_typeA_RotatedBkg_EtaVtxZ_Selected, good_region_ratio);

    TH2D * h2D_BestPair_EtaVtxZ_Selected = (TH2D*) h2D_BestPair_EtaVtxZ_ratio -> Clone("h2D_BestPair_EtaVtxZ_Rebin_Selected");
    FindGoodRegion(h2D_BestPair_EtaVtxZ_ratio, h2D_BestPair_EtaVtxZ_Selected, good_region_ratio);

    TH2D * h2D_typeA_BestPair_EtaVtxZ_Selected = (TH2D*) h2D_typeA_BestPair_EtaVtxZ_ratio -> Clone("h2D_typeA_BestPair_EtaVtxZ_Rebin_Selected");
    FindGoodRegion(h2D_typeA_BestPair_EtaVtxZ_ratio, h2D_typeA_BestPair_EtaVtxZ_Selected, good_region_ratio);

    // Division : ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    SetsPhenixStyle();
    TCanvas * c1 = new TCanvas("c1","c1",950,800);
    gStyle->SetPaintTextFormat("1.3f");

    TLatex * ltx = new TLatex();
    ltx->SetNDC();
    ltx->SetTextSize(0.045);
    ltx->SetTextAlign(31);

    TLatex * draw_text = new TLatex();
    draw_text -> SetNDC();
    draw_text -> SetTextSize(0.03);

    c1 -> cd();

    Rebin_h2D_map["h2D_RotatedBkg_EtaVtxZ_FineBin"].first -> Draw("colz0");
    Rebin_h2D_map["h2D_RotatedBkg_EtaVtxZ_FineBin"].first -> SetMarkerSize(0.5);
    Rebin_h2D_map["h2D_RotatedBkg_EtaVtxZ_FineBin"].first -> Draw("TEXT45E same");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_label.c_str()));
    c1 -> Print(Form("%s/%s.pdf", output_directory.c_str(), Form("%s_data",Rebin_h2D_map["h2D_RotatedBkg_EtaVtxZ_FineBin"].first -> GetName())));
    c1 -> Clear();

    Rebin_h2D_map["h2D_RotatedBkg_EtaVtxZ_FineBin"].second -> Draw("colz0");
    Rebin_h2D_map["h2D_RotatedBkg_EtaVtxZ_FineBin"].second -> SetMarkerSize(0.5);
    Rebin_h2D_map["h2D_RotatedBkg_EtaVtxZ_FineBin"].second -> Draw("TEXT45E same");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_label.c_str()));
    c1 -> Print(Form("%s/%s.pdf", output_directory.c_str(), Form("%s_MC",Rebin_h2D_map["h2D_RotatedBkg_EtaVtxZ_FineBin"].second -> GetName())));
    c1 -> Clear();

    Rebin_h2D_map["h2D_typeA_RotatedBkg_EtaVtxZ_FineBin"].first -> Draw("colz0");
    Rebin_h2D_map["h2D_typeA_RotatedBkg_EtaVtxZ_FineBin"].first -> SetMarkerSize(0.5);
    Rebin_h2D_map["h2D_typeA_RotatedBkg_EtaVtxZ_FineBin"].first -> Draw("TEXT45E same");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_label.c_str()));
    c1 -> Print(Form("%s/%s.pdf", output_directory.c_str(), Form("%s_data",Rebin_h2D_map["h2D_typeA_RotatedBkg_EtaVtxZ_FineBin"].first -> GetName())));
    c1 -> Clear();

    Rebin_h2D_map["h2D_typeA_RotatedBkg_EtaVtxZ_FineBin"].second -> Draw("colz0");
    Rebin_h2D_map["h2D_typeA_RotatedBkg_EtaVtxZ_FineBin"].second -> SetMarkerSize(0.5);
    Rebin_h2D_map["h2D_typeA_RotatedBkg_EtaVtxZ_FineBin"].second -> Draw("TEXT45E same");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_label.c_str()));
    c1 -> Print(Form("%s/%s.pdf", output_directory.c_str(), Form("%s_MC",Rebin_h2D_map["h2D_typeA_RotatedBkg_EtaVtxZ_FineBin"].second -> GetName())));
    c1 -> Clear();


    Rebin_h2D_map["h2D_BestPair_EtaVtxZ"].first -> Draw("colz0");
    Rebin_h2D_map["h2D_BestPair_EtaVtxZ"].first -> SetMarkerSize(0.5);
    Rebin_h2D_map["h2D_BestPair_EtaVtxZ"].first -> Draw("TEXT45E same");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_label.c_str()));
    c1 -> Print(Form("%s/%s.pdf", output_directory.c_str(), Form("%s_data",Rebin_h2D_map["h2D_BestPair_EtaVtxZ"].first -> GetName())));
    c1 -> Clear();

    Rebin_h2D_map["h2D_BestPair_EtaVtxZ"].second -> Draw("colz0");
    Rebin_h2D_map["h2D_BestPair_EtaVtxZ"].second -> SetMarkerSize(0.5);
    Rebin_h2D_map["h2D_BestPair_EtaVtxZ"].second -> Draw("TEXT45E same");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_label.c_str()));
    c1 -> Print(Form("%s/%s.pdf", output_directory.c_str(), Form("%s_MC",Rebin_h2D_map["h2D_BestPair_EtaVtxZ"].second -> GetName())));
    c1 -> Clear();

    Rebin_h2D_map["h2D_typeA_BestPair_EtaVtxZ"].first -> Draw("colz0");
    Rebin_h2D_map["h2D_typeA_BestPair_EtaVtxZ"].first -> SetMarkerSize(0.5);
    Rebin_h2D_map["h2D_typeA_BestPair_EtaVtxZ"].first -> Draw("TEXT45E same");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_label.c_str()));
    c1 -> Print(Form("%s/%s.pdf", output_directory.c_str(), Form("%s_data",Rebin_h2D_map["h2D_typeA_BestPair_EtaVtxZ"].first -> GetName())));
    c1 -> Clear();

    Rebin_h2D_map["h2D_typeA_BestPair_EtaVtxZ"].second -> Draw("colz0");
    Rebin_h2D_map["h2D_typeA_BestPair_EtaVtxZ"].second -> SetMarkerSize(0.5);
    Rebin_h2D_map["h2D_typeA_BestPair_EtaVtxZ"].second -> Draw("TEXT45E same");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_label.c_str()));
    c1 -> Print(Form("%s/%s.pdf", output_directory.c_str(), Form("%s_MC",Rebin_h2D_map["h2D_typeA_BestPair_EtaVtxZ"].second -> GetName())));
    c1 -> Clear();


    c1 -> cd();
    h2D_RotatedBkg_EtaVtxZ_ratio -> Draw("colz0");
    h2D_RotatedBkg_EtaVtxZ_ratio -> SetMarkerSize(0.5);
    h2D_RotatedBkg_EtaVtxZ_ratio -> Draw("TEXTE same");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_label.c_str()));
    c1 -> Print(Form("%s/%s.pdf", output_directory.c_str(), Form("%s",h2D_RotatedBkg_EtaVtxZ_ratio -> GetName())));
    c1 -> Clear();

    c1 -> cd();
    h2D_typeA_RotatedBkg_EtaVtxZ_ratio -> Draw("colz0");
    h2D_typeA_RotatedBkg_EtaVtxZ_ratio -> SetMarkerSize(0.5);
    h2D_typeA_RotatedBkg_EtaVtxZ_ratio -> Draw("TEXTE same");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_label.c_str()));
    c1 -> Print(Form("%s/%s.pdf", output_directory.c_str(), Form("%s",h2D_typeA_RotatedBkg_EtaVtxZ_ratio -> GetName())));
    c1 -> Clear();

    c1 -> cd();
    h2D_BestPair_EtaVtxZ_ratio -> Draw("colz0");
    h2D_BestPair_EtaVtxZ_ratio -> SetMarkerSize(0.5);
    h2D_BestPair_EtaVtxZ_ratio -> Draw("TEXTE same");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_label.c_str()));
    c1 -> Print(Form("%s/%s.pdf", output_directory.c_str(), Form("%s",h2D_BestPair_EtaVtxZ_ratio -> GetName())));
    c1 -> Clear();

    c1 -> cd();
    h2D_typeA_BestPair_EtaVtxZ_ratio -> Draw("colz0");
    h2D_typeA_BestPair_EtaVtxZ_ratio -> SetMarkerSize(0.5);
    h2D_typeA_BestPair_EtaVtxZ_ratio -> Draw("TEXTE same");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_label.c_str()));
    c1 -> Print(Form("%s/%s.pdf", output_directory.c_str(), Form("%s",h2D_typeA_BestPair_EtaVtxZ_ratio -> GetName())));
    c1 -> Clear();


    c1 -> cd();
    h2D_RotatedBkg_EtaVtxZ_Selected -> Draw("colz0");
    h2D_RotatedBkg_EtaVtxZ_Selected -> SetMarkerSize(0.5);
    h2D_RotatedBkg_EtaVtxZ_Selected -> Draw("TEXTE same");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_label.c_str()));
    c1 -> Print(Form("%s/%s.pdf", output_directory.c_str(), Form("%s",h2D_RotatedBkg_EtaVtxZ_Selected -> GetName())));
    c1 -> Clear();

    c1 -> cd();
    h2D_typeA_RotatedBkg_EtaVtxZ_Selected -> Draw("colz0");
    h2D_typeA_RotatedBkg_EtaVtxZ_Selected -> SetMarkerSize(0.5);
    h2D_typeA_RotatedBkg_EtaVtxZ_Selected -> Draw("TEXTE same");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_label.c_str()));
    c1 -> Print(Form("%s/%s.pdf", output_directory.c_str(), Form("%s",h2D_typeA_RotatedBkg_EtaVtxZ_Selected -> GetName())));
    c1 -> Clear();

    c1 -> cd();
    h2D_BestPair_EtaVtxZ_Selected -> Draw("colz0");
    h2D_BestPair_EtaVtxZ_Selected -> SetMarkerSize(0.5);
    h2D_BestPair_EtaVtxZ_Selected -> Draw("TEXTE same");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_label.c_str()));
    c1 -> Print(Form("%s/%s.pdf", output_directory.c_str(), Form("%s",h2D_BestPair_EtaVtxZ_Selected -> GetName())));
    c1 -> Clear();

    c1 -> cd();
    h2D_typeA_BestPair_EtaVtxZ_Selected -> Draw("colz0");
    h2D_typeA_BestPair_EtaVtxZ_Selected -> SetMarkerSize(0.5);
    h2D_typeA_BestPair_EtaVtxZ_Selected -> Draw("TEXTE same");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_label.c_str()));
    c1 -> Print(Form("%s/%s.pdf", output_directory.c_str(), Form("%s",h2D_typeA_BestPair_EtaVtxZ_Selected -> GetName())));
    c1 -> Clear();
    
    // Division : ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    TFile * file_out = TFile::Open(Form("%s/%s", output_directory.c_str(), output_filename.c_str()), "RECREATE");
    file_out -> cd();

    for (auto &pair : target_h2D_map)
    {
        file_out -> cd();
        pair.second.first -> Write(string(pair.first + "_Data").c_str());
        pair.second.second -> Write(string(pair.first + "_MC").c_str());
    }

    h2D_RotatedBkg_EtaVtxZ_FineBin_SigCount_data -> Write();
    h2D_RotatedBkg_EtaVtxZ_FineBin_SigCount_MC -> Write();
    h2D_typeA_RotatedBkg_EtaVtxZ_FineBin_SigCount_data -> Write();
    h2D_typeA_RotatedBkg_EtaVtxZ_FineBin_SigCount_MC -> Write();

    NorCount_h2D_map["h2D_RotatedBkg_EtaVtxZ_FineBin"].first -> Write(Form("%s_data",NorCount_h2D_map["h2D_RotatedBkg_EtaVtxZ_FineBin"].first -> GetName()));
    NorCount_h2D_map["h2D_RotatedBkg_EtaVtxZ_FineBin"].second -> Write(Form("%s_MC",NorCount_h2D_map["h2D_RotatedBkg_EtaVtxZ_FineBin"].second -> GetName()));
    NorCount_h2D_map["h2D_typeA_RotatedBkg_EtaVtxZ_FineBin"].first -> Write(Form("%s_data",NorCount_h2D_map["h2D_typeA_RotatedBkg_EtaVtxZ_FineBin"].first -> GetName()));
    NorCount_h2D_map["h2D_typeA_RotatedBkg_EtaVtxZ_FineBin"].second -> Write(Form("%s_MC",NorCount_h2D_map["h2D_typeA_RotatedBkg_EtaVtxZ_FineBin"].second -> GetName()));

    NorCount_h2D_map["h2D_BestPair_EtaVtxZ"].first -> Write(Form("%s_data",NorCount_h2D_map["h2D_BestPair_EtaVtxZ"].first -> GetName()));
    NorCount_h2D_map["h2D_BestPair_EtaVtxZ"].second -> Write(Form("%s_MC",NorCount_h2D_map["h2D_BestPair_EtaVtxZ"].second -> GetName()));
    NorCount_h2D_map["h2D_typeA_BestPair_EtaVtxZ"].first -> Write(Form("%s_data",NorCount_h2D_map["h2D_typeA_BestPair_EtaVtxZ"].first -> GetName()));
    NorCount_h2D_map["h2D_typeA_BestPair_EtaVtxZ"].second -> Write(Form("%s_MC",NorCount_h2D_map["h2D_typeA_BestPair_EtaVtxZ"].second -> GetName()));

    Rebin_h2D_map["h2D_RotatedBkg_EtaVtxZ_FineBin"].first -> Write(Form("%s_data",Rebin_h2D_map["h2D_RotatedBkg_EtaVtxZ_FineBin"].first -> GetName()));
    Rebin_h2D_map["h2D_RotatedBkg_EtaVtxZ_FineBin"].second -> Write(Form("%s_MC",Rebin_h2D_map["h2D_RotatedBkg_EtaVtxZ_FineBin"].second -> GetName()));
    Rebin_h2D_map["h2D_typeA_RotatedBkg_EtaVtxZ_FineBin"].first -> Write(Form("%s_data",Rebin_h2D_map["h2D_typeA_RotatedBkg_EtaVtxZ_FineBin"].first -> GetName()));
    Rebin_h2D_map["h2D_typeA_RotatedBkg_EtaVtxZ_FineBin"].second -> Write(Form("%s_MC",Rebin_h2D_map["h2D_typeA_RotatedBkg_EtaVtxZ_FineBin"].second -> GetName()));

    Rebin_h2D_map["h2D_BestPair_EtaVtxZ"].first -> Write(Form("%s_data",Rebin_h2D_map["h2D_BestPair_EtaVtxZ"].first -> GetName()));
    Rebin_h2D_map["h2D_BestPair_EtaVtxZ"].second -> Write(Form("%s_MC",Rebin_h2D_map["h2D_BestPair_EtaVtxZ"].second -> GetName()));
    Rebin_h2D_map["h2D_typeA_BestPair_EtaVtxZ"].first -> Write(Form("%s_data",Rebin_h2D_map["h2D_typeA_BestPair_EtaVtxZ"].first -> GetName()));
    Rebin_h2D_map["h2D_typeA_BestPair_EtaVtxZ"].second -> Write(Form("%s_MC",Rebin_h2D_map["h2D_typeA_BestPair_EtaVtxZ"].second -> GetName()));

    h2D_RotatedBkg_EtaVtxZ_ratio -> Write();
    h2D_typeA_RotatedBkg_EtaVtxZ_ratio -> Write();
    h2D_BestPair_EtaVtxZ_ratio -> Write();
    h2D_typeA_BestPair_EtaVtxZ_ratio -> Write();

    h2D_RotatedBkg_EtaVtxZ_Selected -> Write();
    h2D_typeA_RotatedBkg_EtaVtxZ_Selected -> Write();
    h2D_BestPair_EtaVtxZ_Selected -> Write();
    h2D_typeA_BestPair_EtaVtxZ_Selected -> Write();

    file_out -> Close();

    return 0;
}

int GetGeoAccepCorrBkg()
{
    // std::string data_input_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Dec042024/completed/Run3/EvtVtxZ/RandZ_VtxZWeight/completed/Maps/completed";
    // std::string data_input_filename = "Data_GeoAccep_00054280_00000.root";

    // std::string mc_input_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_Ntuple_HIJING_ana443_20241102/Run3/EvtVtxZ/RandZ_VtxZWeight/completed/Maps/completed"; 
    // std::string mc_input_filename = "MC_GeoAccep_00000.root";

    int Mbin = 70;

    std::string data_input_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Dec042024/completed/Run3/EvtVtxZ/RandZ10cm_NoClusQA/completed/Maps/completed";
    std::string data_input_filename = "Data_GeoAccep_Mbin70_00054280_00000.root";

    std::string mc_input_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_Ntuple_HIJING_ana443_20241102/Run3/EvtVtxZ/RandZ10cm_VtxZWeight_NoClusQA/completed/Maps/completed"; 
    std::string mc_input_filename = "MC_GeoAccep_Mbin70_00000.root";

    std::string output_directory = data_input_directory; // note : auto_folder_creation
    std::string output_filename = Form("GeoAccepCorr_Mbin%d.root",Mbin);

    // std::string vtxZReWeighting_input_directory = "/sphenix/user/ChengWei/INTT/INTT_dNdeta_repo/NewCode2024/PrepareAllDist/vtxZDist_comp/plots/Test_20241225_withVtxZQA/INTTvtxZReWeight.root";
    // std::string map_name = "HIJING_noZWeight_WithVtxZQA_Inclusive70";

    std::string plot_label = "Internal";

    std::map<std::string,std::pair<TH2D*,TH2D*>> target_h2D_map = { // note : {data, MC}
        {"h2D_Multiplicity_RotatedBkg_EtaVtxZ", {nullptr, nullptr}},
        {"h2D_Multiplicity_RotatedBkg_EtaVtxZ_FineBin", {nullptr, nullptr}},

        {"h2D_Multiplicity_typeA_RotatedBkg_EtaVtxZ", {nullptr, nullptr}},
        {"h2D_Multiplicity_typeA_RotatedBkg_EtaVtxZ_FineBin", {nullptr, nullptr}},

        {"h2D_Multiplicity_BestPair_EtaVtxZ_FineBin", {nullptr, nullptr}},
        {"h2D_Multiplicity_typeA_BestPair_EtaVtxZ_FineBin", {nullptr, nullptr}}
    };

    std::map<std::string,std::pair<TH1D*,TH1D*>> target_h1D_map = { // note : {data, MC}
        {"h1D_INTTvtxZ_FineBin_NoVtxZWeight", {nullptr, nullptr}}
    };

    system(Form("if [ ! -d %s ]; then mkdir -p %s; fi", output_directory.c_str(), output_directory.c_str()));

    // TH1D * h1D_INTT_vtxZ_reweighting = GetReweighting_hist(vtxZReWeighting_input_directory, map_name);
    TFile * data_file_in = TFile::Open(Form("%s/%s", data_input_directory.c_str(), data_input_filename.c_str()));
    TFile * mc_file_in = TFile::Open(Form("%s/%s", mc_input_directory.c_str(), mc_input_filename.c_str()));

    GetH2D(
        target_h2D_map,
        data_file_in,
        mc_file_in
    );

    GetH1D(
        target_h1D_map,
        data_file_in,
        mc_file_in
    );

    // target_h1D_map["h1D_INTTvtxZ_FineBin_NoVtxZWeight"].first -> Scale(1. / target_h1D_map["h1D_INTTvtxZ_FineBin_NoVtxZWeight"].first -> Integral(-1,-1));
    // target_h1D_map["h1D_INTTvtxZ_FineBin_NoVtxZWeight"].second -> Scale(1. / target_h1D_map["h1D_INTTvtxZ_FineBin_NoVtxZWeight"].second -> Integral(-1,-1));

    

    std::map<std::string,std::pair<TH2D*,TH2D*>> target_h2D_map_narrow; target_h2D_map_narrow.clear();
    for (auto &pair : target_h2D_map)
    {
        int nbinX = pair.second.first -> GetNbinsX();
        double xlow = pair.second.first -> GetXaxis() -> GetXmin();
        double xup = pair.second.first -> GetXaxis() -> GetXmax();

        // todo : the target range is here
        double ybinwidth = pair.second.first -> GetYaxis() -> GetBinWidth(1);
        double Set_ylow = -10;
        double Set_yup = 10;
        int    Set_nbinY = (Set_yup - Set_ylow) / ybinwidth;

        target_h2D_map_narrow[pair.first] = {
            new TH2D(Form("narrow_%s_Data", pair.first.c_str()), Form("narrow_%s_Data;#eta;INTT vtxZ [cm]", pair.first.c_str()), nbinX, xlow, xup, Set_nbinY, Set_ylow, Set_yup),
            new TH2D(Form("narrow_%s_MC", pair.first.c_str()), Form("narrow_%s_MC;#eta;INTT vtxZ [cm]", pair.first.c_str()), nbinX, xlow, xup, Set_nbinY, Set_ylow, Set_yup)
        };

        std::cout<<std::endl;
        std::cout<<"In X: "<<nbinX<<" "<<xlow<<" "<<xup<<", BinWidth: "<<target_h2D_map_narrow[pair.first].first -> GetXaxis() -> GetBinWidth(1)<<std::endl;
        std::cout<<"In Y: "<<Set_nbinY<<" "<<Set_ylow<<" "<<Set_yup<<", BinWidth: "<<target_h2D_map_narrow[pair.first].first -> GetYaxis() -> GetBinWidth(1)<<std::endl;

        H2DCountCarryOver(pair.second.first, target_h2D_map_narrow[pair.first].first); // note : data
        H2DCountCarryOver(pair.second.second, target_h2D_map_narrow[pair.first].second); // note : MC
    }


    SetsPhenixStyle();
    TCanvas * c1 = new TCanvas("c1","c1",950,800);
    c1 -> cd();

    TLatex * ltx = new TLatex();
    ltx->SetNDC();
    ltx->SetTextSize(0.045);
    ltx->SetTextAlign(31);

    TLatex * draw_text = new TLatex();
    draw_text -> SetNDC();
    draw_text -> SetTextSize(0.03);

    TFile * file_out = new TFile(Form("%s/%s", output_directory.c_str(), output_filename.c_str()), "RECREATE");
    file_out -> cd();

    // std::vector<TH2D*> h2D_GeoAccepCorr_vec; h2D_GeoAccepCorr_vec.clear();
    std::vector<TH2D*> h2D_GeoAccepCorr_narrow_vec; h2D_GeoAccepCorr_narrow_vec.clear();
    std::vector<TH2D*> h2D_SelectedRegion_narrow_vec; h2D_SelectedRegion_narrow_vec.clear();

    // for (auto &pair : target_h2D_map)
    // {
    //     ScaleH2D(pair.second.first); // note : data

    //     ScaleH2D(pair.second.second); // note : MC
    //     ScaleH2D_by_H1D(pair.second.second, h1D_INTT_vtxZ_reweighting); // note : MC

    //     std::string hist_name_suffix = pair.first.substr(string("h1D_Multiplicity_").length());

    //     h2D_GeoAccepCorr_vec.push_back(
    //         (TH2D*) pair.second.first -> Clone(Form("h2D_GeoAccepCorr_%s", hist_name_suffix.c_str()))
    //     );

    //     h2D_GeoAccepCorr_vec.back() -> Divide(pair.second.first, pair.second.second, 1, 1);

    //     c1 -> cd();
    //     h2D_GeoAccepCorr_vec.back() -> Draw("colz0");
    //     ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_label.c_str()));
    //     c1 -> Print(Form("%s/%s.pdf", output_directory.c_str(), h2D_GeoAccepCorr_vec.back()->GetName()));
    //     c1 -> Clear();

    //     h2D_GeoAccepCorr_vec.back() -> Write();
    //     pair.second.first -> Write(string(pair.first + "_Data").c_str());
    //     pair.second.second -> Write(string(pair.first + "_MC").c_str());
    // }

    gStyle->SetPaintTextFormat("1.3f");

    for (auto &pair : target_h2D_map_narrow) // note : "type" -> {data_h2D, MC_h2D}
    {
        TH2D * h2D_data_PostVtxZCorr = (TH2D*) pair.second.first -> Clone(Form("%s_PostVtxZCorr", pair.second.first->GetName()));
        std::pair<TH1D *, double> h1D_data_used_vtxZ = ScaleH2D_by_H1D(h2D_data_PostVtxZCorr, target_h1D_map["h1D_INTTvtxZ_FineBin_NoVtxZWeight"].first); // note : data for cancelling the function of the random vertex Z distribution  
        TH2D * h2D_data_PostVtxZCorr_Scale = (TH2D*) h2D_data_PostVtxZCorr -> Clone(Form("%s_Scale", h2D_data_PostVtxZCorr->GetName()));
        
        std::pair<int, int> data_Selected_vtxZ_bins = get_selected_bin_region(
            target_h1D_map["h1D_INTTvtxZ_FineBin_NoVtxZWeight"].first, 
            {
                h2D_data_PostVtxZCorr -> GetYaxis() -> GetXmin(),
                h2D_data_PostVtxZCorr -> GetYaxis() -> GetXmax()
            }
        );
        // ScaleH2D(h2D_data_PostVtxZCorr_Scale, target_h1D_map["h1D_INTTvtxZ_FineBin_NoVtxZWeight"].first -> Integral(data_Selected_vtxZ_bins.first, data_Selected_vtxZ_bins.second)); // note : data
        ScaleH2D(h2D_data_PostVtxZCorr_Scale); // note : data
        // Division : ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

        // ScaleH2D_by_H1D(pair.second.second, h1D_INTT_vtxZ_reweighting); // note : MC
        TH2D * h2D_MC_PostVtxZCorr = (TH2D*) pair.second.second -> Clone(Form("%s_PostVtxZCorr", pair.second.second->GetName()));
        std::pair<TH1D *, double> h1D_MC_used_vtxZ = ScaleH2D_by_H1D(h2D_MC_PostVtxZCorr, target_h1D_map["h1D_INTTvtxZ_FineBin_NoVtxZWeight"].second); // note : MC for cancelling the function of the random vertex Z distribution  
        TH2D * h2D_MC_PostVtxZCorr_Scale = (TH2D*) h2D_MC_PostVtxZCorr -> Clone(Form("%s_Scale", h2D_MC_PostVtxZCorr->GetName()));

        std::pair<int, int> MC_Selected_vtxZ_bins = get_selected_bin_region(
            target_h1D_map["h1D_INTTvtxZ_FineBin_NoVtxZWeight"].second,
            {
                h2D_MC_PostVtxZCorr -> GetYaxis() -> GetXmin(),
                h2D_MC_PostVtxZCorr -> GetYaxis() -> GetXmax()
            }
        );
        // ScaleH2D(h2D_MC_PostVtxZCorr_Scale, target_h1D_map["h1D_INTTvtxZ_FineBin_NoVtxZWeight"].second -> Integral(MC_Selected_vtxZ_bins.first, MC_Selected_vtxZ_bins.second)); // note : MC
        ScaleH2D(h2D_MC_PostVtxZCorr_Scale); // note : MC        
        // Division : ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        std::string hist_name_suffix = pair.first.substr(string("h1D_Multiplicity_").length());

        h2D_GeoAccepCorr_narrow_vec.push_back(
            (TH2D*) pair.second.first -> Clone(Form("narrow_h2D_GeoAccepCorr_%s", hist_name_suffix.c_str()))
        );

        h2D_GeoAccepCorr_narrow_vec.back() -> Divide(h2D_data_PostVtxZCorr_Scale, h2D_MC_PostVtxZCorr_Scale, 1, 1); // note : data/MC

        h2D_SelectedRegion_narrow_vec.push_back(
            (TH2D*) h2D_GeoAccepCorr_narrow_vec.back() -> Clone(Form("narrow_h2D_SelectedRegion_%s", hist_name_suffix.c_str()))
        );

        FindGoodRegion(h2D_GeoAccepCorr_narrow_vec.back(), h2D_SelectedRegion_narrow_vec.back(), 0.1); // todo : threshold

        c1 -> cd();
        h2D_GeoAccepCorr_narrow_vec.back() -> Draw("colz0");
        h2D_GeoAccepCorr_narrow_vec.back() -> SetMarkerSize(0.5);
        h2D_GeoAccepCorr_narrow_vec.back() -> Draw("TEXTE same");
        ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_label.c_str()));
        c1 -> Print(Form("%s/%s.pdf", output_directory.c_str(), h2D_GeoAccepCorr_narrow_vec.back()->GetName()));
        c1 -> Clear();

        c1 -> cd();
        h2D_SelectedRegion_narrow_vec.back() -> Draw("colz0");
        h2D_SelectedRegion_narrow_vec.back() -> SetMarkerSize(0.5);
        h2D_SelectedRegion_narrow_vec.back() -> Draw("TEXTE same");
        ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_label.c_str()));
        c1 -> Print(Form("%s/%s.pdf", output_directory.c_str(), h2D_SelectedRegion_narrow_vec.back()->GetName()));
        c1 -> Clear();

        h2D_SelectedRegion_narrow_vec.back() -> Write();
        h2D_GeoAccepCorr_narrow_vec.back() -> Write();
        pair.second.first -> Write();
        h2D_data_PostVtxZCorr -> Write();
        h2D_data_PostVtxZCorr_Scale -> Write();
        h1D_data_used_vtxZ.first -> Write(Form("narrow_h2D_Multiplicity_%s_vtxZData", hist_name_suffix.c_str()));

        pair.second.second -> Write();
        h2D_MC_PostVtxZCorr -> Write();
        h2D_MC_PostVtxZCorr_Scale -> Write();
        h1D_MC_used_vtxZ.first -> Write( Form("narrow_h2D_Multiplicity_%s_vtxZMC", hist_name_suffix.c_str()) );
    }

    for (auto &pair : target_h2D_map)
    {
        file_out -> cd();
        pair.second.first -> Write(string(pair.first + "_Data").c_str());
        pair.second.second -> Write(string(pair.first + "_MC").c_str());
    }



    file_out -> Close();

    return 0;
}