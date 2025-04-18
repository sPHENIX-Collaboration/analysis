#include "sPhenixStyle.C"

int Combinatorial_Bkg()
{
    std::string mother_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Feb102025/Run6_EvtZFitWidthChange/EvtVtxZ/FinalResult_10cm_Pol2BkgFit/completed";
    std::string sPH_label = "Internal";
    
    SetsPhenixStyle();
    TCanvas * c1 = new TCanvas("c1", "c1", 950,800);
    
    TLatex * ltx = new TLatex();
    ltx->SetNDC();
    ltx->SetTextSize(0.045);
    ltx->SetTextAlign(31);

    TLatex * draw_text = new TLatex();
    draw_text -> SetNDC();
    draw_text -> SetTextSize(0.03);

    TGraph * MC_sbRatio = new TGraph();
    TGraph * data_sbRatio = new TGraph();

    TH2D * h2D_sbRatio = new TH2D("h2D_sbRatio", "h2D_sbRatio;Centrality class index;Combinatorial background ratio, B/(S+B)", 15, 0, 15, 100,0,1);

    
    for (int i = 0; i < 15; i++) // note : 0 to 14, the Mbin
    {
        std::string Mbin_folder_name = Form("vtxZ_-10_10cm_MBin%d",i);
        
        std::string data_file = Form("Data_PreparedNdEtaEach_AlphaCorr_AllSensor_VtxZ10_Mbin%d_00054280_00000_DeltaPhi.root",i);
        std::string MC_file = Form("MC_PreparedNdEtaEach_AllSensor_VtxZ10_Mbin%d_00001_DeltaPhi.root",i);

        TFile * file_data = TFile::Open(Form("%s/%s/Folder_BaseLine/Run_0/completed/%s", mother_directory.c_str(), Mbin_folder_name.c_str(), data_file.c_str()));
        TFile * file_MC = TFile::Open(Form("%s/%s/Folder_BaseLine/Run_0/completed/%s", mother_directory.c_str(), Mbin_folder_name.c_str(), MC_file.c_str()));
        

        for (int eta_i = 8; eta_i <= 18; eta_i++)
        {
            std::string hist_name = Form("hstack1D_DeltaPhi_Eta%d", eta_i);
            std::string hist_name_rotated = Form("hstack1D_DeltaPhi_Eta%d_rotated", eta_i);

            TH1D * data = (TH1D*)((THStack*)file_data->Get(hist_name.c_str()))->GetStack()->Last();
            TH1D * MC = (TH1D*)((THStack*)file_MC->Get(hist_name.c_str()))->GetStack()->Last();

            TH1D * data_rotated = (TH1D*)((THStack*)file_data->Get(hist_name_rotated.c_str()))->GetStack()->Last();
            TH1D * MC_rotated = (TH1D*)((THStack*)file_MC->Get(hist_name_rotated.c_str()))->GetStack()->Last();

            MC_sbRatio -> SetPoint(MC_sbRatio->GetN(), i, MC_rotated->Integral() / MC->Integral()); // note : B / S+B
            data_sbRatio -> SetPoint(data_sbRatio->GetN(), i, data_rotated->Integral() / data->Integral()); // note : B / S+B

            // h2D_sbRatio -> Fill(i, MC_rotated->Integral() / MC->Integral());
            h2D_sbRatio -> Fill(i, data_rotated->Integral() / data->Integral());
        }
    }

    MC_sbRatio -> SetMarkerStyle(20);
    MC_sbRatio -> SetMarkerSize(0.8);
    MC_sbRatio -> SetMarkerColor(kBlue);

    data_sbRatio -> SetMarkerStyle(20);
    data_sbRatio -> SetMarkerSize(0.8);
    data_sbRatio -> SetMarkerColor(kBlack);

    MC_sbRatio -> GetXaxis() -> SetTitle("Mbin");
    MC_sbRatio -> GetYaxis() -> SetTitle("B / S+B");

    // MC_sbRatio -> Draw("AP");
    // data_sbRatio -> Draw("P same");

    h2D_sbRatio -> Draw("colz");

    // draw profile of h2D_sbRatio
    TProfile * profile = h2D_sbRatio -> ProfileX();
    profile -> SetMarkerStyle(20);
    profile -> SetMarkerSize(0.8);
    profile -> SetMarkerColor(kRed);
    profile -> Draw("pl same");

    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_label.c_str())); 
    draw_text -> DrawLatex(0.52, 0.85 + 0.02, "Centrality class 0: 0-3%");
    draw_text -> DrawLatex(0.52, 0.81 + 0.02, "Centrality class 14: 65-70%");
    draw_text -> DrawLatex(0.52, 0.77 + 0.02, "|INTT vtxZ|#kern[1]{#leq} 10.0 cm");
    draw_text -> DrawLatex(0.52, 0.73 + 0.02, "|#eta bin|#kern[1]{#leq} 1.1");

    c1 -> Print("Combinatorial_Bkg.pdf");

    // print the points of the profile
    for (int i = 0; i < profile->GetNbinsX(); i++)
    {
        std::cout<<profile->GetBinCenter(i + 1)<<" "<<profile->GetBinContent(i + 1)<<std::endl;
    }


    return 0;
}