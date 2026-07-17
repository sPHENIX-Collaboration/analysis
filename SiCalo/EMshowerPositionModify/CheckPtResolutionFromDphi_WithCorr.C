// ROOT macro: check pT resolution on an independent test file using f_pt_dphi.
//
// Input fit function:
//   f_pt_dphi from FitPtFromDphi_WithCorr.C, with pT_reco = P0 / |dphi| + P1
//
// Output:
//   - one TH2D: x = truth pT, y = (reco - truth) / truth
//   - TGraphErrors bias vs pT: Gaussian mean from each truth-pT bin projection
//   - TGraphErrors resolution vs pT: Gaussian sigma from each truth-pT bin projection
//
// Raw run:
//   root -l -q 'CheckPtResolutionFromDphi_WithCorr.C("pt_dphi_fit.root", "Dataset/ECPM_ana527_Electron_5GeV_1M_test.root")'
//
// Corrected run:
//   root -l -q 'CheckPtResolutionFromDphi_WithCorr.C("pt_dphi_fit_corr.root", "Dataset/ECPM_ana527_Electron_5GeV_1M_test_corr.root", "tree", "pt_resolution_dphi_check_corr.root", true)'

#include <TFile.h>
#include <TTree.h>
#include <TVector2.h>
#include <TH2D.h>
#include <TH1D.h>
#include <TF1.h>
#include <TGraphErrors.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TLine.h>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

namespace {

double norm2(double x, double y)
{
    return std::sqrt(x * x + y * y);
}

bool finite3(double x, double y, double z)
{
    return std::isfinite(x) && std::isfinite(y) && std::isfinite(z);
}

double dphi_signed_2d(double v1x, double v1y, double v2x, double v2y)
{
    const double phi1 = std::atan2(v1y, v1x);
    const double phi2 = std::atan2(v2y, v2x);
    return TVector2::Phi_mpi_pi(phi2 - phi1);
}

}  // namespace

void CheckPtResolutionFromDphi_WithCorr(
    const char* fit_file_name = "pt_dphi_fit_corr.root",
    const char* test_file_name = "Dataset/ECPM_ana527_Electron_10GeV_2M_test2_corr.root",
    const char* tree_name = "tree",
    const char* output_name = "pt_resolution_dphi_check_corr.root",
    bool use_corrected_calo_position = true,
    bool use_inner_calo_cluster = false,
    double min_abs_dphi = 1.0e-4,
    int n_pt_bins = 50,
    double pt_min = 0.0,
    double pt_max = 10.0,
    int n_res_bins = 240,
    double res_min = -1.2,
    double res_max = 1.2,
    int min_entries_per_bin = 50)
{
    gStyle->SetOptFit(1111);
    gStyle->SetOptStat(0);

    TFile* ffit = TFile::Open(fit_file_name, "READ");
    if (!ffit || ffit->IsZombie()) {
        std::cerr << "Cannot open fit file: " << fit_file_name << std::endl;
        return;
    }

    TF1* f_pt_dphi = dynamic_cast<TF1*>(ffit->Get("f_pt_dphi"));
    if (!f_pt_dphi) {
        std::cerr << "Cannot find TF1 named f_pt_dphi in: " << fit_file_name << std::endl;
        ffit->ls();
        ffit->Close();
        return;
    }

    TFile* fin = TFile::Open(test_file_name, "READ");
    if (!fin || fin->IsZombie()) {
        std::cerr << "Cannot open test file: " << test_file_name << std::endl;
        ffit->Close();
        return;
    }

    TTree* tree = dynamic_cast<TTree*>(fin->Get(tree_name));
    if (!tree) {
        std::cerr << "Cannot find TTree: " << tree_name << std::endl;
        fin->ls();
        fin->Close();
        ffit->Close();
        return;
    }

    std::vector<int>* trk_layer = nullptr;
    std::vector<double>* trk_X = nullptr;
    std::vector<double>* trk_Y = nullptr;
    std::vector<double>* trk_Z = nullptr;

    std::vector<int>* caloClus_system = nullptr;
    std::vector<double>* caloClus_X = nullptr;
    std::vector<double>* caloClus_Y = nullptr;
    std::vector<double>* caloClus_Z = nullptr;

    std::vector<double>* caloClus_innr_X = nullptr;
    std::vector<double>* caloClus_innr_Y = nullptr;
    std::vector<double>* caloClus_innr_Z = nullptr;
    std::vector<double>* caloClus_innr_edep = nullptr;

    std::vector<float>* PrimaryG4P_Pt = nullptr;

    float caloCorr_X = 0.0;
    float caloCorr_Y = 0.0;
    float caloCorr_Z = 0.0;
    int caloCorr_isValid = 0;

    tree->SetBranchAddress("trk_layer", &trk_layer);
    tree->SetBranchAddress("trk_X", &trk_X);
    tree->SetBranchAddress("trk_Y", &trk_Y);
    tree->SetBranchAddress("trk_Z", &trk_Z);
    tree->SetBranchAddress("PrimaryG4P_Pt", &PrimaryG4P_Pt);

    if (use_corrected_calo_position) {
        if (!tree->GetBranch("caloCorr_X") || !tree->GetBranch("caloCorr_Y") ||
            !tree->GetBranch("caloCorr_Z") || !tree->GetBranch("caloCorr_isValid")) {
            std::cerr << "Corrected branches not found. Run CorrectP_CloneTree.py first, or set use_corrected_calo_position=false." << std::endl;
            fin->Close();
            ffit->Close();
            return;
        }
        tree->SetBranchAddress("caloCorr_X", &caloCorr_X);
        tree->SetBranchAddress("caloCorr_Y", &caloCorr_Y);
        tree->SetBranchAddress("caloCorr_Z", &caloCorr_Z);
        tree->SetBranchAddress("caloCorr_isValid", &caloCorr_isValid);
    } else {
        tree->SetBranchAddress("caloClus_system", &caloClus_system);
        tree->SetBranchAddress("caloClus_X", &caloClus_X);
        tree->SetBranchAddress("caloClus_Y", &caloClus_Y);
        tree->SetBranchAddress("caloClus_Z", &caloClus_Z);

        tree->SetBranchAddress("caloClus_innr_X", &caloClus_innr_X);
        tree->SetBranchAddress("caloClus_innr_Y", &caloClus_innr_Y);
        tree->SetBranchAddress("caloClus_innr_Z", &caloClus_innr_Z);
        tree->SetBranchAddress("caloClus_innr_edep", &caloClus_innr_edep);
    }

    TFile* fout = TFile::Open(output_name, "RECREATE");

    TH2D* h2_res_vs_truth = new TH2D(
        "h2_res_vs_truth",
        ";Truth p_{T} [GeV];(p_{T}^{reco} - p_{T}^{truth}) / p_{T}^{truth}",
        n_pt_bins,
        pt_min,
        pt_max,
        n_res_bins,
        res_min,
        res_max);

    Long64_t n_all = tree->GetEntries();
    Long64_t n_pass = 0;
    Long64_t fail_trk = 0;
    Long64_t fail_calo = 0;
    Long64_t fail_truth = 0;
    Long64_t fail_dphi = 0;
    Long64_t fail_reco = 0;

    for (Long64_t ievt = 0; ievt < n_all; ++ievt) {
        tree->GetEntry(ievt);

        if (!trk_layer || !trk_X || !trk_Y || !trk_Z ||
            trk_layer->size() != trk_X->size() ||
            trk_layer->size() != trk_Y->size() ||
            trk_layer->size() != trk_Z->size()) {
            ++fail_trk;
            continue;
        }

        int n34 = 0;
        int n56 = 0;
        int idx34 = -1;
        int idx56 = -1;

        for (size_t i = 0; i < trk_layer->size(); ++i) {
            const int layer = trk_layer->at(i);
            if (layer == 3 || layer == 4) {
                ++n34;
                idx34 = static_cast<int>(i);
            }
            if (layer == 5 || layer == 6) {
                ++n56;
                idx56 = static_cast<int>(i);
            }
        }

        if (n34 != 1 || n56 != 1) {
            ++fail_trk;
            continue;
        }

        if (!PrimaryG4P_Pt || PrimaryG4P_Pt->size() != 1) {
            ++fail_truth;
            continue;
        }

        double xcalo = 0.0;
        double ycalo = 0.0;
        double zcalo = 0.0;

        if (use_corrected_calo_position) {
            if (caloCorr_isValid != 1) {
                ++fail_calo;
                continue;
            }
            xcalo = static_cast<double>(caloCorr_X);
            ycalo = static_cast<double>(caloCorr_Y);
            zcalo = static_cast<double>(caloCorr_Z);
        } else if (use_inner_calo_cluster) {
            if (!caloClus_innr_X || !caloClus_innr_Y || !caloClus_innr_Z ||
                !caloClus_innr_edep || caloClus_innr_X->size() != 1 ||
                caloClus_innr_Y->size() != 1 || caloClus_innr_Z->size() != 1 ||
                caloClus_innr_edep->size() != 1) {
                ++fail_calo;
                continue;
            }
            xcalo = caloClus_innr_X->at(0);
            ycalo = caloClus_innr_Y->at(0);
            zcalo = caloClus_innr_Z->at(0);
        } else {
            if (!caloClus_system || !caloClus_X || !caloClus_Y || !caloClus_Z) {
                ++fail_calo;
                continue;
            }

            int n_emcal = 0;
            int idx_calo = -1;
            for (size_t i = 0; i < caloClus_system->size(); ++i) {
                if (caloClus_system->at(i) == 0) {
                    ++n_emcal;
                    idx_calo = static_cast<int>(i);
                }
            }

            if (n_emcal != 1) {
                ++fail_calo;
                continue;
            }

            xcalo = caloClus_X->at(idx_calo);
            ycalo = caloClus_Y->at(idx_calo);
            zcalo = caloClus_Z->at(idx_calo);
        }

        const double x34 = trk_X->at(idx34);
        const double y34 = trk_Y->at(idx34);
        const double z34 = trk_Z->at(idx34);
        const double x56 = trk_X->at(idx56);
        const double y56 = trk_Y->at(idx56);
        const double z56 = trk_Z->at(idx56);
        const double truth_pt = PrimaryG4P_Pt->at(0);

        if (!finite3(x34, y34, z34) || !finite3(x56, y56, z56) ||
            !finite3(xcalo, ycalo, zcalo) || !std::isfinite(truth_pt) || truth_pt <= 0.0) {
            ++fail_dphi;
            continue;
        }

        const double v1x = x56 - x34;
        const double v1y = y56 - y34;
        const double v2x = xcalo - x56;
        const double v2y = ycalo - y56;

        if (norm2(v1x, v1y) <= 0.0 || norm2(v2x, v2y) <= 0.0) {
            ++fail_dphi;
            continue;
        }

        const double dphi = dphi_signed_2d(v1x, v1y, v2x, v2y);
        const double abs_dphi = std::abs(dphi);

        if (!std::isfinite(abs_dphi) || abs_dphi <= min_abs_dphi) {
            ++fail_dphi;
            continue;
        }

        double reco_pt = f_pt_dphi->Eval(abs_dphi);
        double bias = 0.03/reco_pt - 0.01;
        // reco_pt = reco_pt / (1.0 + bias);
        if (!std::isfinite(reco_pt)) {
            ++fail_reco;
            continue;
        }

        const double rel_res = (reco_pt - truth_pt) / truth_pt;
        h2_res_vs_truth->Fill(truth_pt, rel_res);
        ++n_pass;
    }

    std::cout << "use_corrected_calo_position : " << use_corrected_calo_position << std::endl;
    std::cout << "Total entries                : " << n_all << std::endl;
    std::cout << "Selected entries             : " << n_pass << std::endl;
    std::cout << "Failed trk cut               : " << fail_trk << std::endl;
    std::cout << "Failed calo cut              : " << fail_calo << std::endl;
    std::cout << "Failed truth cut             : " << fail_truth << std::endl;
    std::cout << "Failed dphi cut              : " << fail_dphi << std::endl;
    std::cout << "Failed reco                  : " << fail_reco << std::endl;

    TGraphErrors* gr_bias = new TGraphErrors();
    gr_bias->SetName("gr_bias_vs_truth_pt");
    gr_bias->SetTitle(";Truth p_{T} [GeV];Gaussian mean of (p_{T}^{reco}-p_{T}^{truth})/p_{T}^{truth}");

    TGraphErrors* gr_resolution = new TGraphErrors();
    gr_resolution->SetName("gr_resolution_vs_truth_pt");
    gr_resolution->SetTitle(";Truth p_{T} [GeV];Gaussian #sigma of (p_{T}^{reco}-p_{T}^{truth})/p_{T}^{truth}");

    std::vector<TH1D*> projections;
    std::vector<TF1*> fits;

    for (int ibin = 1; ibin <= h2_res_vs_truth->GetNbinsX(); ++ibin) {
        const double pt_center = h2_res_vs_truth->GetXaxis()->GetBinCenter(ibin);
        const double pt_half_width = 0.5 * h2_res_vs_truth->GetXaxis()->GetBinWidth(ibin);

        TH1D* h_proj = h2_res_vs_truth->ProjectionY(
            Form("h_res_proj_ptbin_%02d", ibin),
            ibin,
            ibin);
        h_proj->SetTitle(Form("%.2f < truth p_{T} < %.2f GeV;%s;Entries",
                              h2_res_vs_truth->GetXaxis()->GetBinLowEdge(ibin),
                              h2_res_vs_truth->GetXaxis()->GetBinUpEdge(ibin),
                              "(p_{T}^{reco}-p_{T}^{truth})/p_{T}^{truth}"));
        projections.push_back(h_proj);

        if (h_proj->GetEntries() < min_entries_per_bin) {
            continue;
        }

        const int max_bin = h_proj->GetMaximumBin();
        const double peak0 = h_proj->GetBinCenter(max_bin);
        const double rms0 = h_proj->GetRMS();
        const double fit_half_width = std::max(0.08, 1.5 * rms0);
        const double fit_low = std::max(res_min, peak0 - fit_half_width);
        const double fit_high = std::min(res_max, peak0 + fit_half_width);

        TF1* f_gaus = new TF1(Form("f_gaus_ptbin_%02d", ibin), "gaus", fit_low, fit_high);
        f_gaus->SetParameters(h_proj->GetMaximum(), peak0, std::max(0.05, rms0));
        h_proj->Fit(f_gaus, "RQ");
        fits.push_back(f_gaus);

        const double bias = f_gaus->GetParameter(1);
        const double bias_err = f_gaus->GetParError(1);
        const double sigma = std::abs(f_gaus->GetParameter(2));
        const double sigma_err = f_gaus->GetParError(2);

        const int ip = gr_bias->GetN();
        gr_bias->SetPoint(ip, pt_center, bias);
        gr_bias->SetPointError(ip, pt_half_width, bias_err);
        gr_resolution->SetPoint(ip, pt_center, sigma);
        gr_resolution->SetPointError(ip, pt_half_width, sigma_err);
    }

    TCanvas* c_res2d = new TCanvas("c_res2d", "resolution 2D", 900, 700);
    h2_res_vs_truth->Draw("COLZ");
    TLine* zero_line = new TLine(pt_min, 0.0, pt_max, 0.0);
    zero_line->SetLineStyle(2);
    zero_line->Draw("SAME");
    c_res2d->SaveAs(use_corrected_calo_position ? "pt_resolution_2d_corr.pdf" : "pt_resolution_2d_raw.pdf");

    TCanvas* c_bias = new TCanvas("c_bias_vs_truth_pt", "bias vs truth pt", 800, 650);
    gr_bias->SetMarkerStyle(20);
    gr_bias->SetMarkerSize(1.0);
    gr_bias->Draw("AP");
    c_bias->SaveAs(use_corrected_calo_position ? "pt_bias_vs_truth_pt_corr.pdf" : "pt_bias_vs_truth_pt_raw.pdf");

    TCanvas* c_resolution = new TCanvas("c_resolution_vs_truth_pt", "resolution vs truth pt", 800, 650);
    gr_resolution->SetMarkerStyle(20);
    gr_resolution->SetMarkerSize(1.0);
    gr_resolution->Draw("AP");
    c_resolution->SaveAs(use_corrected_calo_position ? "pt_resolution_vs_truth_pt_corr.pdf" : "pt_resolution_vs_truth_pt_raw.pdf");

    fout->cd();
    h2_res_vs_truth->Write();
    gr_bias->Write();
    gr_resolution->Write();
    zero_line->Write("zero_line");
    c_res2d->Write();
    c_bias->Write();
    c_resolution->Write();

    for (TH1D* h : projections) {
        h->Write();
    }
    for (TF1* f : fits) {
        f->Write();
    }

    fout->Close();
    fin->Close();
    ffit->Close();

    std::cout << "Wrote: " << output_name << std::endl;
}
