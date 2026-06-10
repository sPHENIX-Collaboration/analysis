std::string getDate()
{
    std::time_t t = std::time(0);   // get time now
    std::tm* now = std::localtime(&t);

    std::stringstream date;
    date << (now->tm_mon + 1) << '/'
         <<  now->tm_mday << '/'
         << (now->tm_year + 1900);
    return date.str();
}

void plot_cutsystematics()
{
  gStyle->SetOptStat(0);

  TFile* f = TFile::Open("LamdbaKsCutEfficiency_200MeV_hists.root");

  TH1F* hCentral = (TH1F*)f->Get("hEffRatio_pT");
  TH1F* hMax = (TH1F*)f->Get("hEffRatio_pT_max");
  TH1F* hMin = (TH1F*)f->Get("hEffRatio_pT_min");

  TH1F* hSys = (TH1F*)f->Get("cuteff_syserr_pT");

  hCentral->SetLineColor(kBlack);
  hCentral->SetMarkerColor(kBlack);
  hCentral->SetMarkerStyle(kFullCircle);
  hCentral->SetMarkerSize(0.7);
  hCentral->GetYaxis()->SetRangeUser(0.0, 1.05);

  hMin->SetLineColor(kBlue);
  hMin->SetMarkerColor(kBlue);
  hMin->SetMarkerStyle(kFullCircle);
  hMin->SetMarkerSize(0.7);

  hMax->SetLineColor(kBlue);
  hMax->SetMarkerColor(kBlue);
  hMax->SetMarkerStyle(kFullCircle);
  hMin->SetMarkerSize(0.7);

  hSys->SetFillColor(kBlue);

  TCanvas* c = new TCanvas("c","c",800,800);
  hCentral->Draw("E");
  hMin->Draw("E SAME");
  hMax->Draw("E SAME");

  TPaveText *pt;
  pt = new TPaveText(0.15,0.75,0.65,1., "NDC");
  pt->SetFillColor(0);
  pt->SetFillStyle(0);
  pt->SetTextFont(42);
  string label = "#it{#bf{sPHENIX}} Simulation";
  TText *pt_LaTex = pt->AddText(label.c_str());
  pt->SetBorderSize(0);
  pt->Draw();
  gPad->Modified();

  TPaveText *ptDate;
  ptDate = new TPaveText(0.67,0.79,0.85,0.95, "NDC");
  ptDate->SetFillColor(0);
  ptDate->SetFillStyle(0);
  ptDate->SetTextFont(42);
  std::string compilation_date = getDate();
  TText *pt_LaTexDate = ptDate->AddText(compilation_date.c_str());
  ptDate->SetBorderSize(0);
  ptDate->Draw();
  gPad->Modified();

  TCanvas* cs = new TCanvas("cs","cs",800,800);
  hSys->Draw();
  pt->Draw();
  ptDate->Draw();
  gPad->Modified();

  c->SaveAs("systematics_varying_cuteff.pdf");
  cs->SaveAs("cuteff_sys_pT.pdf");
}
