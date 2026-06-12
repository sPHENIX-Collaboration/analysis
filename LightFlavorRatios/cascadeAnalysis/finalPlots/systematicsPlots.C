void systematicsPlots()
{
  TFile* sysFile = TFile::Open("/sphenix/user/rosstom/analysis/LightFlavorRatios/systematics/cutVarianceSystematicUncertainties.root", "READ"); 

  std::string sysPrefix = "cutVarianceSys";
 
  TH1F* h_base_pT = (TH1F*)sysFile->Get("h_ratio_base_pT");
  TH1F* h_sys_pT = (TH1F*)sysFile->Get("h_syst_pT");
  TH1F* h_base_eta = (TH1F*)sysFile->Get("h_ratio_base_eta");
  TH1F* h_sys_eta = (TH1F*)sysFile->Get("h_syst_eta");
  TH1F* h_base_phi = (TH1F*)sysFile->Get("h_ratio_base_phi");
  TH1F* h_sys_phi = (TH1F*)sysFile->Get("h_syst_phi");
  TH1F* h_base_rap = (TH1F*)sysFile->Get("h_ratio_base_rap");
  TH1F* h_sys_rap = (TH1F*)sysFile->Get("h_syst_rap");
   
  TH1F* h_percent_pT = (TH1F*)h_sys_pT->Clone("h_percent_pT");
  h_percent_pT->Divide(h_sys_pT,h_base_pT,100.,1.);
  h_percent_pT->GetYaxis()->SetTitle("Systematic Uncertainty (% of Base Ratio)");
  h_percent_pT->GetXaxis()->SetTitle("Mother p_{T} [GeV/c]");
  h_percent_pT->SetMaximum(10.0);
  h_percent_pT->SetMinimum(0.0);
  
  TH1F* h_percent_eta = (TH1F*)h_sys_eta->Clone("h_percent_eta");
  h_percent_eta->Divide(h_sys_eta,h_base_eta,100.,1.);
  h_percent_eta->GetYaxis()->SetTitle("Systematic Uncertainty (% of Base Ratio)");
  h_percent_eta->GetXaxis()->SetTitle("Mother #eta");
  h_percent_eta->SetMaximum(100.0);
  h_percent_eta->SetMinimum(0.0);
  
  TH1F* h_percent_phi = (TH1F*)h_sys_phi->Clone("h_percent_phi");
  h_percent_phi->Divide(h_sys_phi,h_base_phi,100.,1.);
  h_percent_phi->GetYaxis()->SetTitle("Systematic Uncertainty (% of Base Ratio)");
  h_percent_phi->GetXaxis()->SetTitle("Mother #phi [rad]");
  h_percent_phi->SetMaximum(100.0);
  h_percent_phi->SetMinimum(0.0);
  
  TH1F* h_percent_rap = (TH1F*)h_sys_rap->Clone("h_percent_rap");
  h_percent_rap->Divide(h_sys_rap,h_base_rap,100.,1.);
  h_percent_rap->GetYaxis()->SetTitle("Systematic Uncertainty (% of Base Ratio)");
  h_percent_rap->GetXaxis()->SetTitle("Mother Rapidity (y)");
  h_percent_rap->SetMaximum(100.0);
  h_percent_rap->SetMinimum(0.0);

  TCanvas* c1 = new TCanvas("c1","c1",800,800);
  h_percent_pT->Draw("HIST P");
  c1->SaveAs((sysPrefix + "_pT.png").c_str());  
  h_percent_eta->Draw("HIST P");
  c1->SaveAs((sysPrefix + "_eta.png").c_str());  
  h_percent_phi->Draw("HIST P");
  c1->SaveAs((sysPrefix + "_phi.png").c_str());  
  h_percent_rap->Draw("HIST P");
  c1->SaveAs((sysPrefix + "_rap.png").c_str());  
  c1->Close();
}
