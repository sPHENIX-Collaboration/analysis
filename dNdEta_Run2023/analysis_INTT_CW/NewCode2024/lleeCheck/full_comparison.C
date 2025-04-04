#include "sPhenixStyle.C"

int full_comparison()
{
    SetsPhenixStyle();

    std::string call_hist_name = "h2D_Mbin_NClus_Self";

    std::string Data_Run54280_file_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Feb102025/lltoee_check/Data_MBin92_NClus.root";
    std::string MC_ZF_file_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/sPH_dNdeta/Run24AuAuMC/Sim_HIJING_MDC2_ana472_20250307/lltoee_check/MC_MBin92_NClus.root";

    std::string Data_Run54912_file_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54912/lltoee_check/Data_MBin92_NClus.root";
    std::string MC_Field_file_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/sPH_dNdeta/Run24AuAuMC/WrongStrangeIncrease_FieldOn_Sim_HIJING_strangeness_MDC2_ana467_20250226/lltoee_check/MC_MBin92_NClus.root";



    TFile *Data_Run54280_file = TFile::Open(Data_Run54280_file_directory.c_str());
    TFile *MC_ZF_file = TFile::Open(MC_ZF_file_directory.c_str());

    TFile *Data_Run54912_file = TFile::Open(Data_Run54912_file_directory.c_str());
    TFile *MC_Field_file = TFile::Open(MC_Field_file_directory.c_str());



    TH2D * h2D_Data_Run54280 = (TH2D*)Data_Run54280_file->Get(call_hist_name.c_str());
    TH2D * h2D_MC_ZF = (TH2D*)MC_ZF_file->Get(call_hist_name.c_str());

    TH2D * h2D_Data_Run54912 = (TH2D*)Data_Run54912_file->Get(call_hist_name.c_str());
    TH2D * h2D_MC_Field = (TH2D*)MC_Field_file->Get(call_hist_name.c_str());

    std::cout<<h2D_Data_Run54280->GetMean(2)<<std::endl;
    std::cout<<h2D_MC_ZF->GetMean(2)<<std::endl;
    std::cout<<h2D_Data_Run54912->GetMean(2)<<std::endl;
    std::cout<<h2D_MC_Field->GetMean(2)<<std::endl;



    TProfile * profile_Data_Run54280 = h2D_Data_Run54280->ProfileX("profile_Data_Run54280");
    TProfile * profile_MC_ZF = h2D_MC_ZF->ProfileX("profile_MC_ZF");

    TProfile * profile_Data_Run54912 = h2D_Data_Run54912->ProfileX("profile_Data_Run54912");
    TProfile * profile_MC_Field = h2D_MC_Field->ProfileX("profile_MC_Field");



    TH1D * profile_Data_Run54280_hist = (TH1D*)profile_Data_Run54280->Clone("profile_Data_Run54280_hist");
    TH1D * profile_MC_ZF_hist = (TH1D*)profile_MC_ZF->Clone("profile_MC_ZF_hist");

    TH1D * profile_Data_Run54912_hist = (TH1D*)profile_Data_Run54912->Clone("profile_Data_Run54912_hist");
    TH1D * profile_MC_Field_hist = (TH1D*)profile_MC_Field->Clone("profile_MC_Field_hist");




    TH1D * h1D_MC_ZFtoField = new TH1D("h1D_MC_ZFtoField", "h1D_MC_ZFtoField;centrality;MC_ZF / MC_FieldON", h2D_Data_Run54280->GetNbinsX(), h2D_Data_Run54280->GetXaxis()->GetXmin(), h2D_Data_Run54280->GetXaxis()->GetXmax());
    h1D_MC_ZFtoField->Divide(profile_MC_ZF, profile_MC_Field);

    TH1D * h1D_Data54280to54912 = new TH1D("h1D_Data54280to54912", "h1D_Data54280to54912;centrality;Data_Run54280 / Data_Run54912", h2D_Data_Run54280->GetNbinsX(), h2D_Data_Run54280->GetXaxis()->GetXmin(), h2D_Data_Run54280->GetXaxis()->GetXmax());
    h1D_Data54280to54912->Divide(profile_Data_Run54280, profile_Data_Run54912);

    TH1D * h1D_DATAtoMC = new TH1D("h1D_DATAtoMC", "h1D_DATAtoMC;centrality;Data / MC", h2D_Data_Run54280->GetNbinsX(), h2D_Data_Run54280->GetXaxis()->GetXmin(), h2D_Data_Run54280->GetXaxis()->GetXmax());
    h1D_DATAtoMC->Divide(h1D_Data54280to54912, h1D_MC_ZFtoField);

    TH1D * h1D_DatatoMC_FieldON = new TH1D("h1D_DatatoMC_FieldON", "h1D_DatatoMC_FieldON;centrality;(FieldON) Data_54912 / MC", h2D_Data_Run54280->GetNbinsX(), h2D_Data_Run54280->GetXaxis()->GetXmin(), h2D_Data_Run54280->GetXaxis()->GetXmax());
    h1D_DatatoMC_FieldON->Divide(profile_Data_Run54912, profile_MC_Field);

    TH1D * h1D_DatatoMC_FieldOff = new TH1D("h1D_DatatoMC_FieldOff", "h1D_DatatoMC_FieldOff;centrality;(FieldOff) Data_54280 / MC", h2D_Data_Run54280->GetNbinsX(), h2D_Data_Run54280->GetXaxis()->GetXmin(), h2D_Data_Run54280->GetXaxis()->GetXmax());
    h1D_DatatoMC_FieldOff->Divide(profile_Data_Run54280, profile_MC_ZF);

    TCanvas * c1 = new TCanvas("c_Data54280_Profile", "c_Data54280_Profile", 950, 800);
    c1->cd();
    TPad * pad1 = (TPad*) c1 -> cd();
    pad1 -> SetLeftMargin(0.18);
    pad1 -> SetRightMargin(0.08);
    profile_Data_Run54280_hist->SetTitle("Data_Run54280;centrality interval;(run 54280) NClus");
    profile_Data_Run54280_hist->SetMarkerStyle(20);
    profile_Data_Run54280_hist->SetMarkerColor(kBlack);
    profile_Data_Run54280_hist->SetLineColor(kBlack);
    profile_Data_Run54280_hist->SetMarkerSize(0.5);

    profile_Data_Run54280_hist->Draw("E1");
    c1 -> Print("c_Data54280_Profile.pdf");


    TCanvas * c2 = new TCanvas("c_MC_ZF_Profile", "c_MC_ZF_Profile", 950, 800);
    c2->cd();
    TPad * pad2 = (TPad*) c2 -> cd();
    pad2 -> SetLeftMargin(0.18);
    pad2 -> SetRightMargin(0.08);
    profile_MC_ZF_hist->SetTitle("MC_ZF;centrality interval; (MC zero field) NClus");
    profile_MC_ZF_hist->SetMarkerStyle(20);
    profile_MC_ZF_hist->SetMarkerColor(kRed);
    profile_MC_ZF_hist->SetLineColor(kRed);
    profile_MC_ZF_hist->SetMarkerSize(0.5);

    profile_MC_ZF_hist->Draw("E1");
    c2 -> Print("c_MC_ZF_Profile.pdf");

    TCanvas * c3 = new TCanvas("c_Data54912_Profile", "c_Data54912_Profile", 950, 800);
    c3->cd();
    TPad * pad3 = (TPad*) c3 -> cd();
    pad3 -> SetLeftMargin(0.18);
    pad3 -> SetRightMargin(0.08);
    profile_Data_Run54912_hist->SetTitle("Data_Run54912;centrality interval;(run 54912) NClus");
    profile_Data_Run54912_hist->SetMarkerStyle(20);
    profile_Data_Run54912_hist->SetMarkerColor(kBlack);
    profile_Data_Run54912_hist->SetLineColor(kBlack);
    profile_Data_Run54912_hist->SetMarkerSize(0.5);

    profile_Data_Run54912_hist->Draw("E1");
    c3 -> Print("c_Data54912_Profile.pdf");

    TCanvas * c4 = new TCanvas("c_MC_Field_Profile", "c_MC_Field_Profile", 950, 800);
    c4->cd();
    TPad * pad4 = (TPad*) c4 -> cd();
    pad4 -> SetLeftMargin(0.18);
    pad4 -> SetRightMargin(0.08);
    profile_MC_Field_hist->SetTitle("MC_FieldON;centrality interval; (MC field on) NClus");
    profile_MC_Field_hist->SetMarkerStyle(20);
    profile_MC_Field_hist->SetMarkerColor(kRed);
    profile_MC_Field_hist->SetLineColor(kRed);
    profile_MC_Field_hist->SetMarkerSize(0.5);

    profile_MC_Field_hist->Draw("E1");
    c4 -> Print("c_MC_Field_Profile.pdf");

    TCanvas * c5 = new TCanvas("c_MC_ZFtoField", "c_MC_ZFtoField", 950, 800);
    c5->cd();
    TPad * pad5 = (TPad*) c5 -> cd();
    pad5 -> SetLeftMargin(0.18);
    pad5 -> SetRightMargin(0.08);
    h1D_MC_ZFtoField->SetMinimum(0.5);
    h1D_MC_ZFtoField->SetMaximum(1.5);
    h1D_MC_ZFtoField->SetTitle("MC_ZFtoField;centrality interval;NClus ratio (MC_ZF / MC_FieldON)");
    h1D_MC_ZFtoField->SetMarkerStyle(20);
    h1D_MC_ZFtoField->SetMarkerColor(kBlack);
    h1D_MC_ZFtoField->SetLineColor(kBlack);
    h1D_MC_ZFtoField->SetMarkerSize(0.5);

    h1D_MC_ZFtoField->Draw("E1");
    c5 -> Print("c_MC_ZFtoField.pdf");

    TCanvas * c6 = new TCanvas("c_Data54280to54912", "c_Data54280to54912", 950, 800);
    c6->cd();
    TPad * pad6 = (TPad*) c6 -> cd();
    pad6 -> SetLeftMargin(0.18);
    pad6 -> SetRightMargin(0.08);
    h1D_Data54280to54912->SetMinimum(1.5);
    h1D_Data54280to54912->SetMaximum(2.5);
    h1D_Data54280to54912->SetTitle("Data54280to54912;centrality interval;Data NClus ratio (Run54280 / Run54912)");
    h1D_Data54280to54912->SetMarkerStyle(20);
    h1D_Data54280to54912->SetMarkerColor(kBlack);
    h1D_Data54280to54912->SetLineColor(kBlack);
    h1D_Data54280to54912->SetMarkerSize(0.5);

    h1D_Data54280to54912->Draw("E1");
    c6 -> Print("c_Data54280to54912.pdf");


    TCanvas * c7 = new TCanvas("c_DATAtoMC", "c_DATAtoMC", 950, 800);
    c7->cd();
    TPad * pad7 = (TPad*) c7 -> cd();
    pad7 -> SetLeftMargin(0.18);
    pad7 -> SetRightMargin(0.08);
    h1D_DATAtoMC->SetMinimum(1.5);
    h1D_DATAtoMC->SetMaximum(2.5);
    h1D_DATAtoMC->SetTitle("DATAtoMC;centrality interval;Ratio of ratio (Data / MC)");
    h1D_DATAtoMC->SetMarkerStyle(20);
    h1D_DATAtoMC->SetMarkerColor(kBlack);
    h1D_DATAtoMC->SetLineColor(kBlack);
    h1D_DATAtoMC->SetMarkerSize(0.5);

    h1D_DATAtoMC->Draw("E1");
    c7 -> Print("c_DATAtoMC.pdf");


    TCanvas * c8 = new TCanvas("c_DatatoMC_FieldON", "c_DatatoMC_FieldON", 950, 800);
    c8->cd();
    TPad * pad8 = (TPad*) c8 -> cd();
    pad8 -> SetLeftMargin(0.18);
    pad8 -> SetRightMargin(0.08);
    h1D_DatatoMC_FieldON->SetMinimum(0.5);
    h1D_DatatoMC_FieldON->SetMaximum(1.5);
    h1D_DatatoMC_FieldON->SetTitle("DatatoMC_FieldON;centrality interval;NClus Ratio (Data_54912 / MC_FieldON)");
    h1D_DatatoMC_FieldON->SetMarkerStyle(20);
    h1D_DatatoMC_FieldON->SetMarkerColor(kBlack);
    h1D_DatatoMC_FieldON->SetLineColor(kBlack);
    h1D_DatatoMC_FieldON->SetMarkerSize(0.5);

    h1D_DatatoMC_FieldON->Draw("E1");
    c8 -> Print("c_DatatoMC_FieldON.pdf");

    TCanvas * c9 = new TCanvas("c_DatatoMC_FieldOff", "c_DatatoMC_FieldOff", 950, 800);
    c9->cd();
    TPad * pad9 = (TPad*) c9 -> cd();
    pad9 -> SetLeftMargin(0.18);
    pad9 -> SetRightMargin(0.08);
    h1D_DatatoMC_FieldOff->SetMinimum(0.5);
    h1D_DatatoMC_FieldOff->SetMaximum(1.5);
    h1D_DatatoMC_FieldOff->SetTitle("DatatoMC_FieldOff;centrality interval;NClus Ratio (Data_54280 / MC_ZF)");
    h1D_DatatoMC_FieldOff->SetMarkerStyle(20);
    h1D_DatatoMC_FieldOff->SetMarkerColor(kBlack);
    h1D_DatatoMC_FieldOff->SetLineColor(kBlack);
    h1D_DatatoMC_FieldOff->SetMarkerSize(0.5);

    h1D_DatatoMC_FieldOff->Draw("E1");
    c9 -> Print("c_DatatoMC_FieldOff.pdf");


    return 888;
}