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

void trackAccPlots()
{
   TFile* trackAccFile = TFile::Open("/sphenix/user/rosstom/analysis/LightFlavorRatios/bachelor_pi_efficiency/analysis/trackingAcceptanceEfficiencyCorrections_withSystematics.root", "READ");

   TH1F* h_pT_KS0 = (TH1F*)trackAccFile->Get("h_trackAcc_pT_KS0");
   TH1F* h_pT_Xi = (TH1F*)trackAccFile->Get("h_trackAcc_pT_Xi");
   TH1F* h_eta_KS0 = (TH1F*)trackAccFile->Get("h_trackAcc_eta_KS0");
   TH1F* h_eta_Xi = (TH1F*)trackAccFile->Get("h_trackAcc_eta_Xi");
   TH1F* h_phi_KS0 = (TH1F*)trackAccFile->Get("h_trackAcc_phi_KS0");
   TH1F* h_phi_Xi = (TH1F*)trackAccFile->Get("h_trackAcc_phi_Xi");
   TH1F* h_rap_KS0 = (TH1F*)trackAccFile->Get("h_trackAcc_rap_KS0");
   TH1F* h_rap_Xi = (TH1F*)trackAccFile->Get("h_trackAcc_rap_Xi");

   TH1F* h_ratio_pT = (TH1F*)h_pT_Xi->Clone("h_ratio_pT");
   TH1F* h_ratio_eta = (TH1F*)h_eta_Xi->Clone("h_ratio_eta");
   TH1F* h_ratio_phi = (TH1F*)h_phi_Xi->Clone("h_ratio_phi");
   TH1F* h_ratio_rap = (TH1F*)h_rap_Xi->Clone("h_ratio_rap");

   h_ratio_pT->Divide(h_pT_Xi,h_pT_KS0,1.,1.);
   h_ratio_eta->Divide(h_eta_Xi,h_eta_KS0,1.,1.);
   h_ratio_phi->Divide(h_phi_Xi,h_phi_KS0,1.,1.);
   h_ratio_rap->Divide(h_rap_Xi,h_rap_KS0,1.,1.);

   //pT Figure
   h_ratio_pT->GetYaxis()->SetTitle("Tracking Acceptance Ratio");
   h_ratio_pT->GetXaxis()->SetTitle("Mother p_{T} [GeV/c]");
   h_ratio_pT->SetMaximum(1.0);
   h_ratio_pT->SetMinimum(0.0);
   TCanvas *c1  = new TCanvas("myCanvas", "myCanvas",800,800);
   gPad->SetTopMargin(0.1);
   h_ratio_pT->Draw("EP");
   
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

   c1->SaveAs("trackAcc_pT.png"); 
   c1->Close();   

   //eta Figure
   h_ratio_eta->GetYaxis()->SetTitle("Tracking Acceptance Ratio");
   h_ratio_eta->GetXaxis()->SetTitle("Mother #eta");
   h_ratio_eta->SetMaximum(1.0);
   h_ratio_eta->SetMinimum(0.0);
   TCanvas *c2  = new TCanvas("myCanvas2", "myCanvas2",800,800);
   gPad->SetTopMargin(0.1);
   h_ratio_eta->Draw("EP");
   
   pt->Draw();
   gPad->Modified();

   ptDate->Draw();
   gPad->Modified();  

   c2->SaveAs("trackAcc_eta.png");
   c2->Close(); 

   //phi Figure
   h_ratio_phi->GetYaxis()->SetTitle("Tracking Acceptance Ratio");
   h_ratio_phi->GetXaxis()->SetTitle("Mother #phi [rad]");
   h_ratio_phi->SetMaximum(1.0);
   h_ratio_phi->SetMinimum(0.0);
   TCanvas *c3  = new TCanvas("myCanvas3", "myCanvas3",800,800);
   gPad->SetTopMargin(0.1);
   h_ratio_phi->Draw("EP");
   
   pt->Draw();
   gPad->Modified();

   ptDate->Draw();
   gPad->Modified();  

   c3->SaveAs("trackAcc_phi.png");
   c3->Close(); 

   //rap Figure
   h_ratio_rap->GetYaxis()->SetTitle("Tracking Acceptance Ratio");
   h_ratio_rap->GetXaxis()->SetTitle("Mother Rapidity");
   h_ratio_rap->SetMaximum(1.0);
   h_ratio_rap->SetMinimum(0.0);
   TCanvas *c4  = new TCanvas("myCanvas4", "myCanvas4",800,800);
   gPad->SetTopMargin(0.1);
   h_ratio_rap->Draw("EP");
   
   pt->Draw();
   gPad->Modified();

   ptDate->Draw();
   gPad->Modified();  

   c4->SaveAs("trackAcc_rap.png");
   c4->Close(); 
}
