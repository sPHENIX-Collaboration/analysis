#include <TFile.h>
#include <TNtuple.h>
#include <TCanvas.h>
#include <TH1D.h>
#include <TMath.h>
#include <TCanvas.h>
#include <TStopwatch.h>
#include <TChain.h>
#include "TStyle.h"
#include "TColor.h"
#include <iostream>
#include <cmath>
#include <TH1.h>
TChain* ReadData() {
  std::cout << "\033[1;34mLoading data...\033[0m" << std::endl;
  TChain *chain = new TChain("T");
    // To include other runs, uncomment the desired run numbers from the list below:

  const std::vector<std::string> runs = { "21518", "21520", "21598", "21599", "21615", "21796", "21813", "21889", "21891", "22949", "22950", "22951", "22979", "22982" };
  std::string baseDir = "/Users/patsfan753/Desktop/Pi0_Organized/ntp_rootFiles_11_6/";
  for (const auto& run : runs) {
    chain->Add((baseDir + run + "/ntp.root").c_str());
  }
  std::cout << "\033[1;32mData loaded.\033[0m" << std::endl;
  return chain;
}
void FillHistograms(TChain *chain, TH1D *hAsym_all, TH1D *hAsym_pt2to3, TH2D *hAsym_vs_mass_all, TH2D *hAsym_vs_mass_pt2to3, TH2D *hChi_all, TH2D *hChi_pt2to3, TH1D *hDeltaR_all, TH1D *hDeltaR_pt2to3, TH2D *hDeltaR_vs_mass_all, TH2D *hDeltaR_vs_mass_pt2to3, TH1D *hpT_all) {
  std::cout << "\033[1;34mFilling Histograms...\033[0m" << std::endl;
  float clus_E, clus_E2, pi0_pt, pi0_mass, clus_chi, clus_chi2, clus_eta, clus_phi, clus_eta2, clus_phi2, MBD_charge;
    
  const float cut_delta_R = 0.08;
  const float cut_asymmetry = 0.5;
  const float cut_clus_chi = 4;
  const float cut_clus_chi2 = 4;
  const float cut_clus_E = 1.0;
    
    
  chain->SetBranchAddress("clus_E", &clus_E);
  chain->SetBranchAddress("clus_E2", &clus_E2);
  chain->SetBranchAddress("pi0_pt", &pi0_pt);
  chain->SetBranchAddress("pi0_mass", &pi0_mass);
  chain->SetBranchAddress("clus_chi", &clus_chi);
  chain->SetBranchAddress("clus_chi2", &clus_chi2);
  chain->SetBranchAddress("clus_eta", &clus_eta);
  chain->SetBranchAddress("clus_phi", &clus_phi);
  chain->SetBranchAddress("clus_eta2", &clus_eta2);
  chain->SetBranchAddress("clus_phi2", &clus_phi2);
  chain->SetBranchAddress("totalMBD", &MBD_charge);
  
    
    
  Long64_t nEntries = chain->GetEntries();
  for (Long64_t i = 0; i < nEntries; ++i) {
    chain->GetEntry(i);
  
    float asymmetry = fabs(clus_E - clus_E2) / (clus_E + clus_E2);
      
    float delta_eta = clus_eta - clus_eta2;
    float delta_phi = clus_phi - clus_phi2;
    delta_phi = fabs(delta_phi);
    if(delta_phi > M_PI) delta_phi = 2*M_PI-delta_phi;
    float delta_R = TMath::Sqrt(delta_eta * delta_eta + delta_phi * delta_phi);

    hAsym_all->Fill(asymmetry);
      
    hpT_all->Fill(pi0_pt);
      
    hAsym_vs_mass_all->Fill(pi0_mass, asymmetry);
    hChi_all->Fill(clus_chi, clus_chi2);
    hDeltaR_all->Fill(delta_R);
    hDeltaR_vs_mass_all->Fill(pi0_mass, delta_R);
      
    if (pi0_pt >= 3.0 && pi0_pt < 4.0 && MBD_charge >= 21395.5 && MBD_charge < 53640.9) {
        //delta_R < cut_delta_R ||
        if (asymmetry >= cut_asymmetry ||  clus_chi >= cut_clus_chi || clus_chi2 >= cut_clus_chi2 || clus_E < cut_clus_E || clus_E2 < cut_clus_E) {
            continue;  // Skip the current iteration if any of the conditions are not met
        }

        // If all conditions are met, fill the histograms
        hAsym_pt2to3->Fill(asymmetry);
        hAsym_vs_mass_pt2to3->Fill(pi0_mass, asymmetry);
        hChi_pt2to3->Fill(clus_chi, clus_chi2);
        hDeltaR_pt2to3->Fill(delta_R);
        hDeltaR_vs_mass_pt2to3->Fill(pi0_mass, delta_R);
    }
  }
  std::cout << "\033[1;32mAsymmetry histograms filled.\033[0m" << std::endl;
}
// Function to print statistics in a color-coded and formatted manner
void PrintHistogramStatistics(TH1D *histogram) {
    std::string histName = histogram->GetName();
    std::cout << "\033[1;34m" << histName << " Statistics\033[0m" << std::endl;
    std::cout << "---------------------------------------" << std::endl;
    std::cout << "\033[1;32mMean:\033[0m " << histogram->GetMean() << std::endl;
    std::cout << "\033[1;32mStandard Deviation:\033[0m " << histogram->GetStdDev() << std::endl;
    std::cout << "\033[1;32mRMS:\033[0m " << histogram->GetRMS() << std::endl;
    std::cout << "\033[1;32mEntries:\033[0m " << histogram->GetEntries() << std::endl;
    std::cout << "\033[1;32mMode (approximated by bin max):\033[0m " << histogram->GetBinCenter(histogram->GetMaximumBin()) << std::endl;
    std::cout << "---------------------------------------" << std::endl;
}

// Usage example:
// PrintHistogramStatistics(hDeltaR_all);

void Cleanup(std::vector<TObject*> &toBeDeleted) {
  for (auto &obj : toBeDeleted) {
    delete obj;
  }
  toBeDeleted.clear();
}
void Cuts_Analysis() {
  TStopwatch timer;
  timer.Start();
  TChain *chain = ReadData();
  std::vector<TObject*> toBeDeleted;
    
  /*
   Delta R Creating histogram over All EvENTS
   */
  TH1D *hDeltaR_all = new TH1D("hDeltaR_all", "#Delta R Distribution (No Cuts)", 400, 0, 10);
  hDeltaR_all->GetXaxis()->SetTitle("#Delta R"); // x-axis title
  hDeltaR_all->GetYaxis()->SetTitle("Counts");   // y-axis title
  //hDeltaR_all->Rebin(2);
    
  TH1D *hpT_all = new TH1D("hpT_all", "Reconstructed Parton p_{T} Distribution (All Events)", 400, 0, 10);
  hpT_all->GetXaxis()->SetTitle("Reconstructed Parton p_{T}"); // x-axis title
  hpT_all->GetYaxis()->SetTitle("Counts");   // y-axis title

  
    
    
    
  /*
   2D hsitgoram pi0 mass versus Delta r over all events
   */
  TH2D *hDeltaR_vs_mass_all = new TH2D("hDeltaR_vs_mass_all", "#Delta R vs Diphoton Mass (No Cuts)", 500, 0, 12, 200, 0, 5);
  hDeltaR_vs_mass_all->GetXaxis()->SetTitle("Invariant Mass [GeV]");
  hDeltaR_vs_mass_all->GetYaxis()->SetTitle("#Delta R");
  hDeltaR_vs_mass_all->GetZaxis()->SetRangeUser(0, 5000000);
    
    /*
     2D histogram pi0 mass versus Delta r over pt 2 to 3
     */
    TH2D *hDeltaR_vs_mass_pt2to3 = new TH2D("hDeltaR_vs_mass_pt2to3MBDperiph", "#Delta R vs Mass Distribution (With Cuts, #Delta R Excluded)", 200, 0, 2, 300, 0, 2);
    hDeltaR_vs_mass_pt2to3->GetXaxis()->SetTitle("Invariant Mass [GeV]");
    hDeltaR_vs_mass_pt2to3->GetYaxis()->SetTitle("#Delta R");
    hDeltaR_vs_mass_pt2to3->GetZaxis()->SetRangeUser(0, 200);
    
    
    
    
    
    
    
    
  /*
   2D histogram pi0 mass versus asymmetry over all events
   */
  TH2D *hAsym_vs_mass_all = new TH2D("hAsym_vs_mass_all", "Asymmetry vs Mass (No Cuts)", 500, 0, 10, 300, 0, 1);
  hAsym_vs_mass_all->GetXaxis()->SetTitle("Invariant Mass [GeV]");
  hAsym_vs_mass_all->GetYaxis()->SetTitle("Asymmetry");
  hAsym_vs_mass_all->GetZaxis()->SetRangeUser(0, 1500000);

    
  /*
    Delta R Creating Histogram over pt range 2 to 3 GeV
   */
  TH1D *hDeltaR_pt2to3 = new TH1D("hDeltaR_pt3to10", "#Delta R Distribution (3 < p_{T} < 10 GeV)", 400, 0, 7);
  hDeltaR_pt2to3->GetXaxis()->SetTitle("#Delta R"); // x-axis title
  hDeltaR_pt2to3->GetYaxis()->SetTitle("Events");   // y-axis title

 // hDeltaR_pt2to3->GetYaxis()->SetRangeUser(0, 150000);
    
    
  /*
   2D histogram pi0 mass versus asymmetry over pt range 2 to 3
   */
  TH2D *hAsym_vs_mass_pt2to3 = new TH2D("hAsym_vs_mass_pt3to4MBDCentral", "Asymmetry vs Mass (with cuts, excluding asymmetry)", 200, 0, 10, 200, 0, 1);
  
  hAsym_vs_mass_pt2to3->GetXaxis()->SetTitle("Invariant Mass [GeV]");
  hAsym_vs_mass_pt2to3->GetYaxis()->SetTitle("Asymmetry");
  hAsym_vs_mass_pt2to3->GetZaxis()->SetRangeUser(0, 100);

    
    
    
  //1D histogram Asymmetry Cut over all events
  TH1D *hAsym_all = new TH1D("hAsym_all", "Asymmetry Distribution (All Events)", 250, 0, 1);
  hAsym_all->GetXaxis()->SetTitle("Asymmetry");
  hAsym_all->GetYaxis()->SetTitle("Events");
  // Set x-axis range
  hAsym_all->GetXaxis()->SetRangeUser(0, 1.0);
  // Set y-axis range
 // hAsym_all->GetYaxis()->SetRangeUser(0, 3000000);
    
    
  //1D histogram Asymmetry Cut over pt range 2 to 3
  TH1D *hAsym_pt2to3 = new TH1D("hAsym_pt2to3", "Asymmetry Distribution (With Cuts, Asymmetry Excluded)", 200, 0, 1);
  hAsym_pt2to3->GetXaxis()->SetTitle("Asymmetry");
  hAsym_pt2to3->GetYaxis()->SetTitle("Events");
  // Set x-axis range
  hAsym_pt2to3->GetXaxis()->SetRangeUser(0, 1.0);
  // Set y-axis range
  //hAsym_pt2to3->GetYaxis()->SetRangeUser(0, 200000);
      
/*
 TH2D(const char *name, const char *title, Int_t nbinsx, Double_t xlow, Double_t xup, Int_t nbinsy, Double_t ylow, Double_t yup);
 */


  /*
   2D Correlation chi2 cluster 1 versus cluster 2 over all events
   */
  TH2D *hChi_all = new TH2D("hChi_all", "Cluster 1 Chi2 vs Cluster 2 Chi2 (All Events)", 200, 0, 10, 200, 0, 10);

  hChi_all->GetXaxis()->SetTitle("chi2 Cluster 1");
  hChi_all->GetYaxis()->SetTitle("chi2 Cluster 2");

  hChi_all->GetZaxis()->SetRangeUser(0, 100000);


    
  //2D histgoram chi2 cluster 1 versus cluster 2 over pt 2 to 3
  TH2D *hChi_pt2to3 = new TH2D("hChi_pt2to3", "Cluster 1 Chi2 vs Cluster 2 Chi2  (2 < p_{T} < 3 GeV, 0 <= MBD < 50000)", 300, 0, 10, 300, 0, 10);
  hChi_pt2to3->GetXaxis()->SetTitle("chi2 Cluster 1");
  hChi_pt2to3->GetYaxis()->SetTitle("chi2 Cluster 2");
  hChi_pt2to3->GetZaxis()->SetRangeUser(0, 2000);

    

  FillHistograms(chain, hAsym_all, hAsym_pt2to3, hAsym_vs_mass_all, hAsym_vs_mass_pt2to3, hChi_all, hChi_pt2to3, hDeltaR_all, hDeltaR_pt2to3, hDeltaR_vs_mass_all, hDeltaR_vs_mass_pt2to3, hpT_all);
    
  // Path to save the histograms
  std::string baseSavePath = "/Users/patsfan753/Desktop/Pi0_Organized/CutsAnalysisPlots";

  //gPad->SetLogy(1);  // Using global gPad to set log scale
    
  // Saving hDeltaR_all
  TCanvas *c1 = new TCanvas("c1", "Delta R (All Events)");
  hDeltaR_all->Draw();
  c1->SaveAs((baseSavePath + "/1Dhistograms/DeltaR_AllEvents.png").c_str());
    
  // Saving hDeltaR_all
  TCanvas *c15 = new TCanvas("c15", "Reconstructed Parton pT (All Events)");
  c15->SetLogy();
  hpT_all->Draw();
  c15->SaveAs((baseSavePath + "/1Dhistograms/hpT_allEvents.png").c_str());

  // Saving hDeltaR_pt2to3
  TCanvas *c2 = new TCanvas("c2", "Delta R (3 < p_{T} < 10 GeV)");
  hDeltaR_pt2to3->Draw();
  c2->SaveAs((baseSavePath + "/1Dhistograms/DeltaR_pt2to3.png").c_str());

  // Saving hAsym_all
  TCanvas *c3 = new TCanvas("c3", "Asymmetry (All Events)");
  c3->SetLogy();
  hAsym_all->Draw();
  c3->SaveAs((baseSavePath + "/1Dhistograms/Asym_AllEvents.png").c_str());

  // Saving hAsym_pt2to3
  TCanvas *c4 = new TCanvas("c4", "Asymmetry (2 < p_{T} < 3 GeV)");
  hAsym_pt2to3->Draw();
  c4->SaveAs((baseSavePath + "/1Dhistograms/Asym_pt2to3.png").c_str());

    
  gStyle->SetPalette(kRainBow); // kRainBow, kCool, etc.
  // Saving hAsym_vs_mass_all
  TCanvas *c5 = new TCanvas("c5", "Asymmetry vs Mass (All Events)", 900, 600);
    
  c5->SetRightMargin(0.15);  // Give more space for the Z-axis palette on the right
  hAsym_vs_mass_all->Draw("COLZ");
  c5->SetLogz(1);  // This will set the z-axis (color scale) to logarithmic
  hAsym_vs_mass_all->SetStats(kFALSE); // Disable statistics box
  hAsym_vs_mass_all->GetZaxis()->SetLabelSize(0.03);  // Adjust the Z-axis label size, if needed
    

  c5->SaveAs((baseSavePath + "/2Dcorrelations/Asym_vs_Mass_AllEvents.png").c_str());

  // Saving hAsym_vs_mass_pt2to3
  TCanvas *c6 = new TCanvas("c6", "Asymmetry vs Mass (2 < p_{T} < 3 GeV and 0 < MBD < 50000)", 900, 600);
  c6->SetRightMargin(0.15);  // Give more space for the Z-axis palette on the right
  hAsym_vs_mass_pt2to3->Draw("COLZ");
  hAsym_vs_mass_pt2to3->SetStats(kFALSE); // Disable statistics box
  hAsym_vs_mass_pt2to3->GetZaxis()->SetLabelSize(0.03);
  c6->SetLogz();
  c6->SaveAs((baseSavePath + "/2Dcorrelations/Asym_vs_Mass_pt2to3.png").c_str());

  // Saving hDeltaR_vs_mass_all
  TCanvas *c7 = new TCanvas("c7", "#Delta R vs Diphoton Mass (No Cuts)", 900, 600);
  c7->SetRightMargin(0.15);  // Give more space for the Z-axis palette on the right
  hDeltaR_vs_mass_all->Draw("COLZ");
  hDeltaR_vs_mass_all->SetStats(kFALSE); // Disable statistics box
  hDeltaR_vs_mass_all->GetZaxis()->SetLabelSize(0.03);
  c7->SetLogz();
  c7->SaveAs((baseSavePath + "/2Dcorrelations/DeltaR_vs_Mass_AllEvents.png").c_str());

  // Saving hDeltaR_vs_mass_pt2to3
  TCanvas *c8 = new TCanvas("c8", "#Delta R vs Mass (3 < p_{T} < 4 GeV and 21395.5 < MBD < 53640.9)", 900, 600);
  c8->SetRightMargin(0.15);  // Give more space for the Z-axis palette on the right
  c8->SetLogz();
  hDeltaR_vs_mass_pt2to3->Draw("COLZ");
  hDeltaR_vs_mass_pt2to3->SetStats(kFALSE); // Disable statistics box
  hDeltaR_vs_mass_pt2to3->GetZaxis()->SetLabelSize(0.03);  // Adjust the Z-axis label size,
    
  c8->SaveAs((baseSavePath + "/2Dcorrelations/DeltaR_vs_Mass_pt2to3.png").c_str());

  /*
   Chi2 Correlation Plot Over All events canvas creation and saving
   */
  TCanvas *c9 = new TCanvas("c9", "Chi2 (All Events)", 900, 600);
  c9->SetRightMargin(0.15);  // Give more space for the Z-axis palette on the right
  hChi_all->Draw("COLZ");
  hChi_all->SetStats(kFALSE); // Disable statistics box
  hChi_all->GetZaxis()->SetLabelSize(0.03);
  // Set log scale on Z-axis
  //c9->SetLogz();
  // Set palette
  c9->SaveAs((baseSavePath + "/2Dcorrelations/Chi_AllEvents.png").c_str());

    
  // Saving hChi_pt2to3
  TCanvas *c10 = new TCanvas("c10", "Chi2 (2 < p_{T} < 3 GeV)", 900, 600);
  c10->SetRightMargin(0.15);
  hChi_pt2to3->Draw("COLZ");
  hChi_pt2to3->SetStats(kFALSE);
  hChi_pt2to3->GetZaxis()->SetLabelSize(0.03);
  c10->SaveAs((baseSavePath + "/2Dcorrelations/Chi_pt2to3.png").c_str());
    
  /*
   TH1D* ProjectionY(const char* name, Int_t firstbinx, Int_t lastbinx, Option_t* option);
   
   The ProjectionY method creates a 1D histogram by summing up the contents of the 2D histogram along the specified range of x-bins (firstbinx to lastbinx). In your case, the range is from the 1st x-bin to the last x-bin of the original 2D histogram.
   */
  // AFTER filling, project the Asymmetry to a 1D histogram
  // Define x-axis range of interest
  //int binX2_Asym_All = hAsym_vs_mass_all->GetXaxis()->FindBin(2.5); // End of the range of interest
    
  //if want to go to last bin in x--better to set it to this and change the actual 2D histograms bounds
  int binX2_Asym_All = hAsym_vs_mass_all->GetNbinsX();
  TH1D *hAsym_ProjectionY_Allevents = hAsym_vs_mass_all->ProjectionY("hAsym_ProjectionY_allEvents", 0, 1, "e");
  hAsym_ProjectionY_Allevents->SetTitle("Projection of Asymmetry onto Y-axis (Around Pi0 Mass)");
  hAsym_ProjectionY_Allevents->GetXaxis()->SetTitle("Asymmetry Projected");
  hAsym_ProjectionY_Allevents->GetYaxis()->SetTitle("Counts");
  // Rebin the histogram to smooth it out
  //hAsym_ProjectionY->Rebin(2); // Combines every 2 bins into 1
    
  TCanvas *c11 = new TCanvas("c11", "Weighted Asymmetry Projection (All Events)");
  hAsym_ProjectionY_Allevents->Draw("HIST");
  c11->SaveAs((baseSavePath + "/1Dhistograms/weightedAsym_Projection_Allevents.png").c_str());

    
  // DO the same for pt 2 to 3
  int binX2_Asym_pt2to3 = hAsym_vs_mass_all->GetXaxis()->FindBin(2.5); // End of the range of interest
  TH1D *hAsym_ProjectionY_pt2to3 = hAsym_vs_mass_pt2to3->ProjectionY("hAsym_ProjectionY_pt2to3", 1, binX2_Asym_pt2to3, "e");
  hAsym_ProjectionY_pt2to3->SetTitle("Projection of Asymmetry onto Y-axis (2 < pT < 3)");
  hAsym_ProjectionY_pt2to3->GetXaxis()->SetTitle("Asymmetry Projected");
  hAsym_ProjectionY_pt2to3->GetYaxis()->SetTitle("Counts");
  // Rebin the histogram to smooth it out
  //hAsym_ProjectionY->Rebin(5); // Combines every 5 bins into 1
  
  TCanvas *c12 = new TCanvas("c12", "Weighted Asymmetry Projection (2 < pT < 3)");
  hAsym_ProjectionY_pt2to3->Draw("HIST");
  c12->SaveAs((baseSavePath + "/1Dhistograms/weightedAsym_Projection_pT2to3.png").c_str());
    

  // Projections for Delta R 2D hists
  //int binX2_hDeltaR_vs_mass_all = hDeltaR_vs_mass_all->GetXaxis()->FindBin(2.5); // End of the range of interest
  int binX2_hDeltaR_vs_mass_all = hDeltaR_vs_mass_all->GetNbinsX();
    
  TH1D *hDeltaR_vs_mass_all_ProjectionY = hDeltaR_vs_mass_all->ProjectionY("hDeltaR_ProjectionY_AllEvents", 0, binX2_hDeltaR_vs_mass_all, "e");
  hDeltaR_vs_mass_all_ProjectionY->SetTitle("Projection of Delta R onto Y-axis Over All Events");
  hDeltaR_vs_mass_all_ProjectionY->GetXaxis()->SetTitle("Delta R Projected");
  hDeltaR_vs_mass_all_ProjectionY->GetYaxis()->SetTitle("Counts");
  // Rebin the histogram to smooth it out
  hDeltaR_vs_mass_all_ProjectionY->Rebin(5); // Combines every 5 bins into 1

  TCanvas *c13 = new TCanvas("c13", "Weighted Delta R Projection Over All Events");
  hDeltaR_vs_mass_all_ProjectionY->Draw("HIST");
  c13->SaveAs((baseSavePath + "/1Dhistograms/weightedDeltaR_Projection_Allevents.png").c_str());


  // DO the same for pt 2 to 3
  int binX2_hDeltaR_vs_mass_pt2to3 = hDeltaR_vs_mass_pt2to3->GetXaxis()->FindBin(2.5); // End of the range of interest
  TH1D *hDeltaR_vs_mass_pt2to3_ProjectionY = hDeltaR_vs_mass_pt2to3->ProjectionY("hDeltaR_ProjectionY_pt2to3", 1, binX2_hDeltaR_vs_mass_pt2to3, "e");
  hDeltaR_vs_mass_pt2to3_ProjectionY->SetTitle("Projection of Delta R onto Y-axis (2 < pT < 3)");
  hDeltaR_vs_mass_pt2to3_ProjectionY->GetXaxis()->SetTitle("Delta R Projected");
  hDeltaR_vs_mass_pt2to3_ProjectionY->GetYaxis()->SetTitle("Counts");
  // Rebin the histogram to smooth it out
  //hAsym_ProjectionY->Rebin(5); // Combines every 5 bins into 1

  TCanvas *c14 = new TCanvas("c14", "Weighted Delta R Projection (2 < pT < 3)");
  hDeltaR_vs_mass_pt2to3_ProjectionY->Draw("HIST");
  c14->SaveAs((baseSavePath + "/1Dhistograms/weightedDeltaR_Projection_pT2to3.png").c_str());
    
    
  /*
   Statistics Outputted at end
   */
  PrintHistogramStatistics(hDeltaR_all);
  PrintHistogramStatistics(hDeltaR_pt2to3);
  PrintHistogramStatistics(hAsym_all);
  PrintHistogramStatistics(hAsym_pt2to3);
    
  PrintHistogramStatistics(hDeltaR_vs_mass_all_ProjectionY);
  PrintHistogramStatistics(hDeltaR_vs_mass_pt2to3_ProjectionY);
  PrintHistogramStatistics(hAsym_ProjectionY_Allevents);
  PrintHistogramStatistics(hAsym_ProjectionY_pt2to3);
  // Calculate and output the correlation factor
  double correlationAllEvents = hChi_all->GetCorrelationFactor();
  std::cout << "Correlation factor between chi2 Cluster 1 and chi2 Cluster 2 Over All Events: " << correlationAllEvents << std::endl;

  // Calculate and output the correlation factor
  double correlationPt2to3 = hChi_pt2to3->GetCorrelationFactor();
  std::cout << "Correlation factor between chi2 Cluster 1 and chi2 Cluster 2 Over 2 < pT < 3: " << correlationPt2to3 << std::endl;

    
  toBeDeleted.push_back(hDeltaR_all);
  toBeDeleted.push_back(hpT_all);
  toBeDeleted.push_back(hDeltaR_pt2to3);
  toBeDeleted.push_back(hAsym_all);
  toBeDeleted.push_back(hAsym_pt2to3);
  toBeDeleted.push_back(chain);
  toBeDeleted.push_back(hAsym_vs_mass_all);
  toBeDeleted.push_back(hAsym_vs_mass_pt2to3);
  toBeDeleted.push_back(hDeltaR_vs_mass_all);
  toBeDeleted.push_back(hDeltaR_vs_mass_pt2to3);
  toBeDeleted.push_back(hChi_all);
  toBeDeleted.push_back(hChi_pt2to3);
  Cleanup(toBeDeleted);
  timer.Stop();
  std::cout << "Macro completed. Time elapsed: " << timer.RealTime() << " seconds." << std::endl;
}
