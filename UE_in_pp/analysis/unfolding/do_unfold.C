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


void do_unfold(const char* simfile = "analysis_sim_run21_output/output_dijet_sim_iter_3.root",
               const char* datafile = "analysis_data_output/output_0mrad_1.5mrad_4bin_newetbin_dijet.root") {

    std::cout << gSystem->GetLibraries() << std::endl;
    std::cout << RooUnfoldResponse::Class()->GetImplFileName() << std::endl;

    //********** Files **********//
    TFile *f_sim = new TFile(simfile, "READ");
    TFile *f_data = new TFile(datafile, "READ");

    int ntoys = 1000;
    std::vector<std::string> syst = {"calib_dijet","calib_dijet_jesdown","calib_dijet_jesup","calib_dijet_jerdown","calib_dijet_jerup","calib_dijet_half1","calib_dijet_half2"};;
    std::vector<std::string> trim = {"","_trim_5","_trim_10","_reweight","_reweight_trim_5","_reweight_trim_10"};
    TH2D* h_measure = (TH2D*)f_data->Get("h_calibjet_pt_dijet_eff");

    for (int i = 0; i < 7; i++) {
        TFile *f_out = new TFile(Form("output_unfolded_data_%s_run21_iter_3_notoys.root",syst[i].c_str()),"RECREATE");
        
        RooUnfoldResponse* h_respmatrix[6];
        TH2D* h_truth[6];
        for (int j = 0; j < 6; j++) {
            h_respmatrix[j] = (RooUnfoldResponse*)f_sim->Get(("h_respmatrix_"+syst[i]+trim[j]).c_str());
            h_truth[j] = (TH2D*)h_respmatrix[j]->Htruth(); h_truth[j]->SetName(("h_truth_"+syst[i]+trim[j]).c_str());
        }

        RooUnfoldBayes unfold[6][20];
        TH2D* h_unfold[6][20];
        for (int j = 0; j < 6; j++) {
            for (int n = 0; n < 20; n++) {
                unfold[j][n] = RooUnfoldBayes(h_respmatrix[j], h_measure);
                unfold[j][n].SetIterations(n+1);
                unfold[j][n].HandleFakes(true);
                h_unfold[j][n] = (TH2D*)unfold[j][n].Hunfold(RooUnfolding::kErrors); 
                h_unfold[j][n]->SetName(("h_unfold_"+syst[i]+trim[j]+"_"+to_string(n+1)).c_str());
                //toy_errors(unfold[j][n], h_unfold[j][n], ntoys);
            }
        }

        // Write histograms.
        std::cout << "Writing histograms... iter " << i << std::endl;
        f_out->cd();
        if (i == 0) h_measure->Write();
        for (int j = 0; j < 6; j++) {
            h_truth[j]->Write();
            for (int n = 0; n < 20; n++) {
                h_unfold[j][n]->Write();
            }
        }
        f_out->Close();

    }

  std::cout << "All done!" << std::endl;
}