int CodeCheck()
{
    std::string input_directory_1 = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Feb102025/Run6_EvtZFitWidthChange/EvtVtxZ/ColumnCheck_INTTz/baseline/completed";
    std::string input_filename_1 = "Data_ColumnCheck_BcoFullDiffCut_Mbin70_VtxZ-30to30cm_ClusQAAdc35PhiSize40_00054280_merged.root";
    std::string input_hist_1 = "h2D_ClusCountPhiIdZId_Layer";

    std::string input_directory_2 = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Feb102025/Run6_EvtZFitWidthChange/EvtVtxZ/ColumnCheck/baseline/completed"; 
    std::string input_filename_2 = "Data_ColumnCheck_BcoFullDiffCut_Mbin70_VtxZ-30to30cm_ClusQAAdc35PhiSize40_00054280_merged_merged.root";
    std::string input_hist_2 = "h2D_ClusCountPhiIdZId_Layer";

    std::string output_directory;
    std::string output_filename;

    TFile * f1 = TFile::Open(Form("%s/%s", input_directory_1.c_str(), input_filename_1.c_str()));
    TFile * f2 = TFile::Open(Form("%s/%s", input_directory_2.c_str(), input_filename_2.c_str()));

    for (int i = 0; i < 4; i++)
    {
        TH2D * h1 = (TH2D *)f1->Get(Form("%s%d", input_hist_1.c_str(), i+3));
        TH2D * h2 = (TH2D *)f2->Get(Form("%s%d", input_hist_2.c_str(), i+3));

        for (int xi = 1; xi <= h1->GetNbinsX(); xi++)
        {
            for (int yi = 1; yi <= h1->GetNbinsY(); yi++)
            {
                double diff = h1->GetBinContent(xi, yi) - h2->GetBinContent(xi, yi);
                double diff_percent = diff / h1->GetBinContent(xi, yi);

                if (h1->GetBinContent(xi, yi) != h2->GetBinContent(xi, yi))
                {
                    if (fabs(diff_percent) > 0.0001){
                        cout<<"Different at "<<xi<<" "<<yi<<", h1: "<<h1->GetBinContent(xi, yi)<<", h2: "<<h2->GetBinContent(xi, yi)<<", Diff: "<<h1->GetBinContent(xi, yi) - h2->GetBinContent(xi, yi)<<endl;
                    }
                }
            }
        }

        std::cout<<std::endl;
    }

    return 888;
}