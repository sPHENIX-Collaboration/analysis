TH2D * GetHist(std::string directory, std::string file_name, std::string hist_name)
{
    TFile * file = new TFile(Form("%s/%s", directory.c_str(), file_name.c_str()), "READ");
    TH2D * hist = (TH2D*)file->Get(hist_name.c_str());

    return hist;
}

int MulMap_comp()
{
    // std::string black_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_HIJING_ananew_20250131/Run6_EvtZFitWidthChange/EvtVtxZ/ColumnCheck/baseline/completed/MulMap/completed";
    // std::string black_file_name = "MulMap_BcoFullDiffCut_Mbin70_VtxZ-30to30cm_ClusQAAdc35PhiSize40_00054280.root";
    // std::string black_hist_name = "h2D_MulMap";
    // std::string black_str = "Old";

    std::string black_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_HIJING_strangeness_MDC2_ana472_20250310/Run7/EvtVtxZ/ColumnCheck/baseline/completed/MulMap/completed";
    std::string black_file_name = "MulMap_BcoFullDiffCut_Mbin70_VtxZ-30to30cm_ClusQAAdc35PhiSize40_00054280.root";
    std::string black_hist_name = "h2D_MulMap";
    std::string black_str = "HIJING_strange_official";

    std::string red_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_HIJING_MDC2_ana472_20250307/Run7/EvtVtxZ/ColumnCheck/baseline/completed/MulMap/completed";
    std::string red_file_name = "MulMap_BcoFullDiffCut_Mbin70_VtxZ-30to30cm_ClusQAAdc35PhiSize40_00054280.root";
    std::string red_hist_name = "h2D_MulMap";
    std::string red_str = "official_HIJING";

    std::string output_directory = black_directory;
    // std::string output_filename = "comp_h2D_MulMap.root";
    std::string output_filename_pdf = "comp_h2D_MulMap"; 

    bool DoNormalize = false;
    bool set_log_y = false;

    // TFile * file_out = new TFile(Form("%s/%s", output_directory.c_str(), output_filename.c_str()), "RECREATE");

    TH2D * black_hist = GetHist(black_directory, black_file_name, black_hist_name);
    TH2D * red_hist = GetHist(red_directory, red_file_name, red_hist_name);


    for (int i = 1; i <= black_hist->GetNbinsX(); i++)
    {
        for (int j = 1; j <= black_hist->GetNbinsY(); j++)
        {
            double black_content = black_hist->GetBinContent(i, j);
            double red_content = red_hist->GetBinContent(i, j);

            if (black_content != red_content)
            {
                std::cout << "i = " << i << ", j = " << j <<", center: "<< black_hist->GetXaxis()->GetBinCenter(i) << ", " << black_hist->GetYaxis()->GetBinCenter(j)
                << ", black = " << black_content << ", red = " << red_content << std::endl;
            }
        }
    }

    return 888;
}


int MulMap_comp_check()
{
    std::string common_folder = "/Run7/EvtVtxZ/ColumnCheck/baseline/noAdcCut/MulMap/completed";
    std::string black_file_name = "MulMap_BcoFullDiffCut_Mbin70_VtxZ-30to30cm_ClusQAAdc0PhiSize40_00054280.root";

    std::string black_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_HIJING_strangeness_MDC2_ana472_20250310" + common_folder;
    std::string black_hist_name = "h2D_MulMap";
    std::string black_str = "Official_HIJING_strange";

    std::string red_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_HIJING_MDC2_ana472_20250307" + common_folder;
    std::string red_file_name = black_file_name;
    std::string red_hist_name = "h2D_MulMap";
    std::string red_str = "New_HIJING_MDC2_ana472_20250307";

    std::string output_directory = black_directory;
    // std::string output_filename = "comp_h2D_MulMap.root";
    std::string output_filename_pdf = "comp_h2D_MulMap"; 

    bool DoNormalize = false;
    bool set_log_y = false;

    // TFile * file_out = new TFile(Form("%s/%s", output_directory.c_str(), output_filename.c_str()), "RECREATE");

    TH2D * black_hist = GetHist(black_directory, black_file_name, black_hist_name);
    TH2D * red_hist = GetHist(red_directory, red_file_name, red_hist_name);


    for (int i = 1; i <= black_hist->GetNbinsX(); i++)
    {
        for (int j = 1; j <= black_hist->GetNbinsY(); j++)
        {
            double black_content = black_hist->GetBinContent(i, j);
            double red_content = red_hist->GetBinContent(i, j);

            if (black_content != red_content)
            {
                std::cout << "i = " << i << ", j = " << j <<", center: "<< black_hist->GetXaxis()->GetBinCenter(i) << ", " << black_hist->GetYaxis()->GetBinCenter(j)
                << ", black = " << black_content << ", red = " << red_content << std::endl;
            }
        }
    }

    return 888;
}