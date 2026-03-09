int ratio_check()
{
    string hist_name = "h2D_Mbin_NClus";

    string zf_data_file = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Feb102025/Run6_EvtZFitWidthChange/EvtVtxZ/completed/Data_MBin70_NClus.root";
    string zf_mc_file = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/sPH_dNdeta/Run24AuAuMC/Sim_HIJING_MDC2_ana472_20250307/Run7/EvtVtxZ/completed/MC_MBin70_NClus.root";

    TCanvas * c1 = new TCanvas("c1", "c1", 2400,600);
    c1 -> Divide(4,1);

    TFile * zf_data = TFile::Open(zf_data_file.c_str());
    TH2D * zf_data_hist = (TH2D*)zf_data->Get(hist_name.c_str());
    zf_data_hist -> GetYaxis() -> SetTitle("NClus (data)");
    TProfile * zf_data_profile = zf_data_hist -> ProfileX("zf_data_profile");
    zf_data_profile -> SetMarkerStyle(20);
    zf_data_profile -> SetMarkerSize(0.8);
    zf_data_profile -> SetMarkerColor(kRed);
    c1 -> cd(1);
    zf_data_hist -> Draw("colz0");
    zf_data_profile -> Draw("pl same");


    
    TFile * zf_mc = TFile::Open(zf_mc_file.c_str());
    TH2D * zf_mc_hist = (TH2D*)zf_mc->Get(hist_name.c_str());
    zf_mc_hist -> GetYaxis() -> SetTitle("NClus (MC)");
    TProfile * zf_mc_profile = zf_mc_hist -> ProfileX("zf_mc_profile");
    zf_mc_hist -> SetMarkerStyle(20);
    zf_mc_hist -> SetMarkerSize(0.8);
    zf_mc_hist -> SetMarkerColor(kYellow);
    c1 -> cd(2);
    zf_mc_hist -> Draw("colz0");
    zf_mc_profile -> Draw("pl same");

    c1 -> cd(3);
    zf_data_profile -> GetYaxis() -> SetTitle("NClus");
    zf_data_profile -> Draw("pl");
    zf_mc_profile -> Draw("pl same");

    c1 -> cd(4);
    TH1D * zf_data_profile_Backh1D = (TH1D*)zf_data_profile->Clone("zf_data_profile_Backh1D");
    TH1D * zf_mc_profile_Backh1D = (TH1D*)zf_mc_profile->Clone("zf_mc_profile_Backh1D");

    TH1D * zf_ratio = new TH1D("zf_ratio", "zf_ratio;centrality;data/MC", zf_data_hist->GetNbinsX(), zf_data_hist->GetXaxis()->GetXmin(), zf_data_hist->GetXaxis()->GetXmax());
    zf_ratio -> SetMarkerStyle(20);
    zf_ratio -> SetMarkerSize(0.8);
    // zf_ratio -> Sumw2(false);
    zf_ratio -> Divide(zf_data_profile_Backh1D, zf_mc_profile_Backh1D);
    
    zf_ratio -> SetMinimum(0.5);
    zf_ratio -> SetMaximum(1.5);

    TF1 * f1 = new TF1("f1", "pol1", 0, 100);
    f1 -> SetLineColor(kRed);
    f1 -> SetLineStyle(2);
    zf_ratio -> Fit(f1, "", "", 61, 81);
    f1 -> SetRange(61., 100.);
    f1 -> Draw("same");

    zf_ratio -> Draw("p");

    TPad * pad5 = new TPad("pad5", "pad5", 0.2,0.2,0.55,0.5);
    pad5 -> Draw();
    pad5 -> cd();

    TH1D * zf_ratio_clone = new TH1D("zf_ratio_clone", "zf_ratio_clone;centrality;Diff", zf_data_hist->GetNbinsX(), zf_data_hist->GetXaxis()->GetXmin(), zf_data_hist->GetXaxis()->GetXmax());
    for (int i = 1; i <= zf_ratio_clone->GetNbinsX(); i++)
    {
        zf_ratio_clone -> SetBinContent(i, zf_ratio->GetBinContent(i) - f1->Eval(zf_ratio_clone->GetBinCenter(i)));
        zf_ratio_clone -> SetBinError(i, sqrt(pow(zf_ratio->GetBinError(i),2) + pow(f1->GetParError(0),2)));
        // std::cout<<"i = "<<i<<", X center: "<<zf_ratio_clone->GetBinCenter(i)<<", Y: "<<zf_ratio_clone->GetBinContent(i)<<std::endl;
    }

    zf_ratio_clone -> GetXaxis()->SetRangeUser(61, 100);
    zf_ratio_clone -> SetMinimum(-0.1);
    zf_ratio_clone -> SetMaximum(0.1);
    zf_ratio_clone -> Draw("ep");

    TLine * line = new TLine(61, 0, 100, 0);
    line -> SetLineColor(kRed);
    line -> SetLineStyle(2);
    line -> Draw("same");

    // zf_ratio -> SetLineColorAlpha(1,0);

    // for (int i = 1; i <= zf_data_profile_Backh1D->GetNbinsX(); i++)
    // {
    //     std::cout<<"i = "<<i<<", X center: "<<zf_data_profile_Backh1D->GetBinCenter(i)<<", Y data: "<<zf_data_profile_Backh1D->GetBinContent(i)<<", Y MC: "<<zf_mc_profile_Backh1D->GetBinContent(i)<<std::endl;
    //     std::cout<<"i = "<<i<<", X center: "<<zf_data_profile_Backh1D->GetBinCenter(i)<<", Y Unc data: "<<zf_data_profile_Backh1D->GetBinError(i)<<", Y Unc MC: "<<zf_mc_profile_Backh1D->GetBinError(i)<<std::endl;
    //     // zf_ratio->SetBinError(i,0);
    //     std::cout<<"i = "<<i<<", X center: "<<zf_data_profile_Backh1D->GetBinCenter(i)<<", Y ratio: "<<zf_ratio->GetBinContent(i)<<", Y Unc ratio: "<<zf_ratio->GetBinError(i)<<std::endl;

    //     std::cout<<std::endl;

        
    // }

    // TH1D * zf_ratio = (TH1D*)zf_data_profile->Clone("zf_ratio");
    // zf_ratio -> Sumw2(true);
    // zf_ratio -> Divide(zf_data_profile_Backh1D, zf_mc_profile_Backh1D);
    

    return 111;
}