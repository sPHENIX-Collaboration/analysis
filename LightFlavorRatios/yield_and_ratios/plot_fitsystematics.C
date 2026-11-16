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

void plot_fitsystematics()
{
  gStyle->SetOptStat(0);

  TFile* f = TFile::Open("fits.root");

  TH1F* Ks_sys = (TH1F*)f->Get("K_S0_fit_syserr_vspT");
  TH1F* Lambda_sys = (TH1F*)f->Get("Lambda0_fit_syserr_vspT");
  TH1F* ratio_sys = (TH1F*)f->Get("ratio_fit_syserr_vspT");

  Ks_sys->SetFillColor(kAzure);
  Lambda_sys->SetFillColor(kAzure);
  ratio_sys->SetFillColor(kAzure);

  Lambda_sys->GetYaxis()->SetRangeUser(0.,1.1*Lambda_sys->GetMaximum());

  TCanvas* c = new TCanvas("c","c",800,800);
  Ks_sys->Draw();

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

  TCanvas* cl = new TCanvas("cl","cl",800,800);
  Lambda_sys->Draw();
  pt->Draw();
  ptDate->Draw();
  gPad->Modified();

  TCanvas* cr = new TCanvas("cr","cr",800,800);
  ratio_sys->Draw();
  pt->Draw();
  ptDate->Draw();
  gPad->Modified();

  c->SaveAs("Kshort_yield_sys.pdf");
  cl->SaveAs("Lambda_yield_sys.pdf");
  cr->SaveAs("ratio_yield_sys.pdf");
}
