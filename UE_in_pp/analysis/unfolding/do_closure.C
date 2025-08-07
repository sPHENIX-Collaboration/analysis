#include <TChain.h>
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TF1.h>
#include <TMath.h>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include "RooUnfold.h"
#include "RooUnfoldResponse.h"
#include "RooUnfoldBayes.h"

void toy_errors(RooUnfoldBayes& unfold, TH2D* h_unfolded, int ntoys = 1000) {
    // Vectors to hold toy results
    std::vector<TVectorD> vx;   // unfolded values for each toy
    std::vector<TVectorD> vxe;  // errors for each toy (optional)
    std::vector<double> chi2;   // chi2 per toy (optional)

    // Run toys
    unfold.SetNToys(ntoys);
    unfold.RunToys(ntoys, vx, vxe, chi2);

    // Check toy results
    if (vx.empty()) {
        std::cerr << "No toy results returned!" << std::endl;
        return;
    }

    int nbins = vx[0].GetNoElements();

    // Calculate mean per bin over toys
    TVectorD mean(nbins);
    for (int i = 0; i < nbins; ++i) {
        double sum = 0;
        for (int t = 0; t < ntoys; ++t) {
            sum += vx[t][i];
        }
        mean[i] = sum / ntoys;
    }

    // Calculate variance per bin
    TVectorD variance(nbins);
    for (int i = 0; i < nbins; ++i) {
        double var = 0;
        for (int t = 0; t < ntoys; ++t) {
            double diff = vx[t][i] - mean[i];
            var += diff * diff;
        }
        variance[i] = var / (ntoys - 1);
    }

    // Apply errors to histogram
    int nx = h_unfolded->GetNbinsX();
    int ny = h_unfolded->GetNbinsY();
    for (int j = 1; j <= ny; ++j) {
        for (int i = 1; i <= nx; ++i) {
            int flatIndex = (j - 1) * nx + (i - 1);  
            // statistical uncertainty is quad sum of unfolded data errors from kError 
            // and response matrix errors determined from variane of toys sampling
            h_unfolded->SetBinError(i, j, std::sqrt(h_unfolded->GetBinError(i,j)*h_unfolded->GetBinError(i,j) + variance[flatIndex]));
        }
    }
}

void do_closure(const char* simfile = "analysis_sim_run21_output/output_dijet_sim_iter_3.root",
                const char* closurefile = "output_closure_sim_run21_iter_3_1000toys.root") {

    std::cout << gSystem->GetLibraries() << std::endl;
    std::cout << RooUnfoldResponse::Class()->GetImplFileName() << std::endl;
 
  //********** Files **********//
  TFile *f_sim = new TFile(simfile, "READ");
  TFile *f_out = new TFile(closurefile, "RECREATE");

  int ntoys = 1000;
  std::vector<std::string> syst = {"calib_dijet","calib_dijet_jesdown","calib_dijet_jesup","calib_dijet_jerdown","calib_dijet_jerup","calib_dijet_half1","calib_dijet_half2"};;
  std::vector<std::string> trim = {"","_trim_5","_trim_10","_reweight","_reweight_trim_5","_reweight_trim_10"};
  
  RooUnfoldResponse* h_respmatrix[7][6];
  TH2D* h_truth[7][6];
  TH2D* h_measure[7][6];
  for (int i = 0; i < 7; i++) {
    for (int j = 0; j < 6; j++) {
      h_respmatrix[i][j] = (RooUnfoldResponse*)f_sim->Get(("h_respmatrix_"+syst[i]+trim[j]).c_str());
      h_truth[i][j] = (TH2D*)h_respmatrix[i][j]->Htruth(); h_truth[i][j]->SetName(("h_truth_"+syst[i]+trim[j]).c_str());
      h_measure[i][j] = (TH2D*)h_respmatrix[i][j]->Hmeasured(); h_measure[i][j]->SetName(("h_measure_"+syst[i]+trim[j]).c_str());
    }
  }

  RooUnfoldBayes unfold_full_1[7][6];
  RooUnfoldBayes unfold_half[6][10];
  TH2D* h_unfold_full_1[7][6];
  TH2D* h_unfold_half[6][10];

  for (int i = 0; i < 7; i++) {
    for (int j = 0; j < 6; j++) {
        unfold_full_1[i][j] = RooUnfoldBayes(h_respmatrix[i][j], h_measure[i][j]);
        unfold_full_1[i][j].SetIterations(1);
        unfold_full_1[i][j].HandleFakes(true);
        h_unfold_full_1[i][j] = (TH2D*)unfold_full_1[i][j].Hunfold(RooUnfolding::kErrors);
        h_unfold_full_1[i][j]->SetName(("h_unfold_full_"+syst[i]+trim[j]+"_1").c_str());
        toy_errors(unfold_full_1[i][j], h_unfold_full_1[i][j], ntoys);
    }
  }
  
  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 10; j++) {
        unfold_half[i][j] = RooUnfoldBayes(h_respmatrix[5][i], h_measure[6][i]);
        unfold_half[i][j].SetIterations(j+1);
        unfold_half[i][j].HandleFakes(true);
        h_unfold_half[i][j] = (TH2D*)unfold_half[i][j].Hunfold(RooUnfolding::kErrors); 
        h_unfold_half[i][j]->SetName(("h_unfold_half"+trim[i]+"_"+to_string(j+1)).c_str());
        toy_errors(unfold_half[i][j], h_unfold_half[i][j], ntoys);
    }
  }
  

  // Write histograms.
  std::cout << "Writing histograms..." << std::endl;
  f_out->cd();

  for (int i = 0; i < 7; i++) {
    for (int j = 0; j < 6; j++) {
        h_truth[i][j]->Write();
        h_measure[i][j]->Write();
        h_unfold_full_1[i][j]->Write();
    }
  }
  
  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 10; j++) {
        h_unfold_half[i][j]->Write();
    }
  }
  
  f_out->Close();
  std::cout << "All done!" << std::endl;
}