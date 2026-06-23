void rawDataYield()
{
  std::vector<double> pT_yield_KS0 = {1.56838e+06,466592,346953,275590};
  std::vector<double> pT_yield_KS0_unc = {1310.56,732.058,653.129,611.569};
  std::vector<double> pT_yield_Xi = {666.42,813.952,1157.94,1160.66};
  std::vector<double> pT_yield_Xi_unc = {49.0957,61.9994,66.6396,67.3105};

  std::vector<double> phi_yield_KS0 = {1.03834e+06,1.25342e+06,1.28936e+06,635891,533332,831440};
  std::vector<double> phi_yield_KS0_unc = {1067.37,1200.14,1230.64,843.862,773.927,961.485};
  std::vector<double> phi_yield_Xi = {615.514,1155.15,1278.81,530.745,244.084,670.816};
  std::vector<double> phi_yield_Xi_unc = {48.4438,73.9671,64.3481,47.5336,39.8697,50.3296};

  std::vector<double> eta_yield_KS0 = {942748,1.05173e+06,908385,826072,847009,731772};
  std::vector<double> eta_yield_KS0_unc = {1026.21,1089.52,1022.4,980.568,991.139,911.075};
  std::vector<double> eta_yield_Xi = {1039.68,921.916,560.377,511.261,579.641,749.629};
  std::vector<double> eta_yield_Xi_unc = {57.6352,57.5487,50.4322,41.9356,51.3805,60.8867};

  std::vector<double> rap_yield_KS0 = {913665,1.11902e+06,1.02323e+06,926873,889687,705360};
  std::vector<double> rap_yield_KS0_unc = {1017.74,1121.89,1060.37,1053.75,1004.96,902.663};
  std::vector<double> rap_yield_Xi = {699.09,1179.11,719.61,640.866,757.92,537.411};
  std::vector<double> rap_yield_Xi_unc = {50.1173,59.2896,55.6634,60.7515,63.1075,44.4095};

  const int nBins_pT = 4;
  double pTbins[nBins_pT + 1] = {0.75, 1.07, 1.25, 1.49, 2.0};

  const int nBins_phi = 6;
  double phibins[nBins_phi + 1] = {-M_PI,-2.09439,-1.04719,0.0,1.04721,2.09441,M_PI};

  const int nBins_eta = 6;
  double etabins[nBins_eta + 1] = {-1.1,-0.733333,-0.366666,0.0,0.366668,0.733335,1.1};

  const int nBins_rap = 6;
  double rapbins[nBins_rap + 1] = {-1.0,-0.66,-0.33,0.0,0.33,0.66,1.0};

  TH1F* h_pT_base_KS0 = new TH1F("h_pT_base_KS0",";K_{S}^{0} p_{T} [GeV/c];Counts",nBins_pT,pTbins);
  TH1F* h_pT_base_Xi = new TH1F("h_pT_base_Xi",";#Xi^{-} p_{T} [GeV/c];Counts",nBins_pT,pTbins);
  TH1F* h_phi_base_KS0 = new TH1F("h_phi_base_KS0",";K_{S}^{0} #phi [rad];Counts",nBins_phi,phibins);
  TH1F* h_phi_base_Xi = new TH1F("h_phi_base_Xi",";#Xi^{-} #phi [rad];Counts",nBins_phi,phibins);
  TH1F* h_eta_base_KS0 = new TH1F("h_eta_base_KS0",";K_{S}^{0} #eta;Counts",nBins_eta,etabins);
  TH1F* h_eta_base_Xi = new TH1F("h_eta_base_Xi",";#Xi^{-} #eta;Counts",nBins_eta,etabins);
  TH1F* h_rap_base_KS0 = new TH1F("h_rap_base_KS0",";K_{S}^{0} y;Counts",nBins_rap,rapbins);
  TH1F* h_rap_base_Xi = new TH1F("h_rap_base_Xi",";#Xi^{-} y;Counts",nBins_rap,rapbins);

  h_pT_base_KS0->Sumw2();
  h_pT_base_Xi->Sumw2();
  h_phi_base_KS0->Sumw2();
  h_phi_base_Xi->Sumw2();
  h_eta_base_KS0->Sumw2();
  h_eta_base_Xi->Sumw2();
  h_rap_base_KS0->Sumw2();
  h_rap_base_Xi->Sumw2();

  for (int i = 1; i <= nBins_pT; i++)
  {
    h_pT_base_KS0->SetBinContent(i,pT_yield_KS0[i-1]);
    h_pT_base_KS0->SetBinError(i,pT_yield_KS0_unc[i-1]);
    h_pT_base_Xi->SetBinContent(i,pT_yield_Xi[i-1]);
    h_pT_base_Xi->SetBinError(i,pT_yield_Xi_unc[i-1]);
  }
  for (int i = 1; i <= nBins_phi; i++)
  {
    h_phi_base_KS0->SetBinContent(i,phi_yield_KS0[i-1]);
    h_phi_base_KS0->SetBinError(i,phi_yield_KS0_unc[i-1]);
    h_phi_base_Xi->SetBinContent(i,phi_yield_Xi[i-1]);
    h_phi_base_Xi->SetBinError(i,phi_yield_Xi_unc[i-1]);
    
    h_eta_base_KS0->SetBinContent(i,eta_yield_KS0[i-1]);
    h_eta_base_KS0->SetBinError(i,eta_yield_KS0_unc[i-1]);
    h_eta_base_Xi->SetBinContent(i,eta_yield_Xi[i-1]);
    h_eta_base_Xi->SetBinError(i,eta_yield_Xi_unc[i-1]);
    
    h_rap_base_KS0->SetBinContent(i,rap_yield_KS0[i-1]);
    h_rap_base_KS0->SetBinError(i,rap_yield_KS0_unc[i-1]);
    h_rap_base_Xi->SetBinContent(i,rap_yield_Xi[i-1]);
    h_rap_base_Xi->SetBinError(i,rap_yield_Xi_unc[i-1]);
  }

  TFile* outputFile = TFile::Open("rawDataYield.root","RECREATE");
  outputFile->cd();
  h_pT_base_KS0->Write();
  h_pT_base_Xi->Write();
  h_phi_base_KS0->Write();
  h_phi_base_Xi->Write();
  h_eta_base_KS0->Write();
  h_eta_base_Xi->Write();
  h_rap_base_KS0->Write();
  h_rap_base_Xi->Write();

  outputFile->Close();
}
