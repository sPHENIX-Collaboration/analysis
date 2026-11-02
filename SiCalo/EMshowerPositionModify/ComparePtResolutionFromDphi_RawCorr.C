// ComparePtResolutionFromDphi_RawCorr.C
#include <TFile.h>
#include <TTree.h>
#include <TVector2.h>
#include <TH2D.h>
#include <TH1D.h>
#include <TF1.h>
#include <TGraphErrors.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TLine.h>

#include <cmath>
#include <iostream>
#include <vector>

namespace {
double norm2(double x, double y) {
    return std::sqrt(x*x + y*y);
}

bool finite3(double x, double y, double z) {
    return std::isfinite(x) && std::isfinite(y) && std::isfinite(z);
}

double dphi_signed_2d(double v1x, double v1y, double v2x, double v2y) {
    const double phi1 = std::atan2(v1y, v1x);
    const double phi2 = std::atan2(v2y, v2x);
    return TVector2::Phi_mpi_pi(phi2 - phi1);
}

struct Result {
    TH2D* h2 = nullptr;
    TGraphErrors* bias = nullptr;
    TGraphErrors* res = nullptr;
};

Result make_result(
    TTree* tree,
    TF1* f_pt_dphi,
    const char* tag,
    bool use_corr,
    double min_abs_dphi,
    int n_pt_bins,
    double pt_min,
    double pt_max,
    int n_res_bins,
    double res_min,
    double res_max,
    int min_entries_per_bin
) {
    std::vector<int>* trk_layer = nullptr;
    std::vector<double>* trk_X = nullptr;
    std::vector<double>* trk_Y = nullptr;
    std::vector<double>* trk_Z = nullptr;
    std::vector<float>* PrimaryG4P_Pt = nullptr;

    float raw_X = 0, raw_Y = 0, raw_Z = 0;
    float corr_X = 0, corr_Y = 0, corr_Z = 0;
    int corr_valid = 0;

    tree->SetBranchAddress("trk_layer", &trk_layer);
    tree->SetBranchAddress("trk_X", &trk_X);
    tree->SetBranchAddress("trk_Y", &trk_Y);
    tree->SetBranchAddress("trk_Z", &trk_Z);
    tree->SetBranchAddress("PrimaryG4P_Pt", &PrimaryG4P_Pt);

    tree->SetBranchAddress("caloCorr_raw_X", &raw_X);
    tree->SetBranchAddress("caloCorr_raw_Y", &raw_Y);
    tree->SetBranchAddress("caloCorr_raw_Z", &raw_Z);

    tree->SetBranchAddress("caloCorr_X", &corr_X);
    tree->SetBranchAddress("caloCorr_Y", &corr_Y);
    tree->SetBranchAddress("caloCorr_Z", &corr_Z);
    tree->SetBranchAddress("caloCorr_isValid", &corr_valid);

    Result out;

    out.h2 = new TH2D(
        Form("h2_res_vs_truth_%s", tag),
        ";Truth p_{T} [GeV];(p_{T}^{reco}-p_{T}^{truth})/p_{T}^{truth}",
        n_pt_bins, pt_min, pt_max,
        n_res_bins, res_min, res_max
    );

    Long64_t n_all = tree->GetEntries();
    Long64_t n_pass = 0;

    for (Long64_t ievt = 0; ievt < n_all; ++ievt) {
        tree->GetEntry(ievt);

        if (!trk_layer || !trk_X || !trk_Y || !trk_Z) continue;
        if (trk_layer->size() != trk_X->size()) continue;
        if (trk_layer->size() != trk_Y->size()) continue;
        if (trk_layer->size() != trk_Z->size()) continue;
        if (!PrimaryG4P_Pt || PrimaryG4P_Pt->size() != 1) continue;

        int n34 = 0, n56 = 0;
        int idx34 = -1, idx56 = -1;

        for (size_t i = 0; i < trk_layer->size(); ++i) {
            int layer = trk_layer->at(i);
            if (layer == 3 || layer == 4) {
                ++n34;
                idx34 = i;
            }
            if (layer == 5 || layer == 6) {
                ++n56;
                idx56 = i;
            }
        }

        if (n34 != 1 || n56 != 1) continue;

        double xcalo, ycalo, zcalo;

        if (use_corr) {
            if (corr_valid != 1) continue;
            xcalo = corr_X;
            ycalo = corr_Y;
            zcalo = corr_Z;
        } else {
            xcalo = raw_X;
            ycalo = raw_Y;
            zcalo = raw_Z;
        }

        const double x34 = trk_X->at(idx34);
        const double y34 = trk_Y->at(idx34);
        const double z34 = trk_Z->at(idx34);
        const double x56 = trk_X->at(idx56);
        const double y56 = trk_Y->at(idx56);
        const double z56 = trk_Z->at(idx56);
        const double truth_pt = PrimaryG4P_Pt->at(0);

        if (!finite3(x34, y34, z34)) continue;
        if (!finite3(x56, y56, z56)) continue;
        if (!finite3(xcalo, ycalo, zcalo)) continue;
        if (!std::isfinite(truth_pt) || truth_pt <= 0) continue;

        const double v1x = x56 - x34;
        const double v1y = y56 - y34;
        const double v2x = xcalo - x56;
        const double v2y = ycalo - y56;

        if (norm2(v1x, v1y) <= 0 || norm2(v2x, v2y) <= 0) continue;

        const double dphi = dphi_signed_2d(v1x, v1y, v2x, v2y);
        const double abs_dphi = std::abs(dphi);

        if (!std::isfinite(abs_dphi) || abs_dphi <= min_abs_dphi) continue;

        const double reco_pt = f_pt_dphi->Eval(abs_dphi);
        if (!std::isfinite(reco_pt)) continue;

        const double rel_res = (reco_pt - truth_pt) / truth_pt;
        out.h2->Fill(truth_pt, rel_res);
        ++n_pass;
    }

    std::cout << "[" << tag << "] pass = " << n_pass << " / " << n_all << std::endl;

    out.bias = new TGraphErrors();
    out.bias->SetName(Form("gr_bias_%s", tag));
    out.bias->SetTitle(";Truth p_{T} [GeV];Gaussian mean");

    out.res = new TGraphErrors();
    out.res->SetName(Form("gr_resolution_%s", tag));
    out.res->SetTitle(";Truth p_{T} [GeV];Gaussian #sigma");

    for (int ibin = 1; ibin <= out.h2->GetNbinsX(); ++ibin) {
        TH1D* h = out.h2->ProjectionY(Form("h_proj_%s_%02d", tag, ibin), ibin, ibin);
        if (h->GetEntries() < min_entries_per_bin) continue;

        double pt_center = out.h2->GetXaxis()->GetBinCenter(ibin);
        double pt_half_width = 0.5 * out.h2->GetXaxis()->GetBinWidth(ibin);

        int max_bin = h->GetMaximumBin();
        double peak0 = h->GetBinCenter(max_bin);
        double rms0 = h->GetRMS();

        double fit_half_width = std::max(0.08, 1.5 * rms0);
        double fit_low = std::max(res_min, peak0 - fit_half_width);
        double fit_high = std::min(res_max, peak0 + fit_half_width);

        TF1* f = new TF1(Form("f_gaus_%s_%02d", tag, ibin), "gaus", fit_low, fit_high);
        f->SetParameters(h->GetMaximum(), peak0, std::max(0.05, rms0));
        h->Fit(f, "RQ");

        int ip = out.bias->GetN();

        out.bias->SetPoint(ip, pt_center, f->GetParameter(1));
        out.bias->SetPointError(ip, pt_half_width, f->GetParError(1));

        out.res->SetPoint(ip, pt_center, std::abs(f->GetParameter(2)));
        out.res->SetPointError(ip, pt_half_width, f->GetParError(2));
    }

    return out;
}
}

void ComparePtResolutionFromDphi_RawCorr(
    const char* raw_fit_file = "pt_dphi_fit.root",
    const char* corr_fit_file = "pt_dphi_fit_corr.root",
    const char* test_corr_file = "Dataset/ECPM_ana527_Electron_10GeV_2M_test2_corr.root",
    const char* tree_name = "tree",
    const char* output_name = "pt_resolution_compare_raw_corr.root",
    double min_abs_dphi = 1.0e-4,
    int n_pt_bins = 50,
    double pt_min = 0.0,
    double pt_max = 10.0,
    int n_res_bins = 240,
    double res_min = -1.2,
    double res_max = 1.2,
    int min_entries_per_bin = 50
) {
    gStyle->SetOptStat(0);
    gStyle->SetOptFit(0);

    TFile* fraw = TFile::Open(raw_fit_file, "READ");
    TFile* fcorr = TFile::Open(corr_fit_file, "READ");
    TFile* fin = TFile::Open(test_corr_file, "READ");

    if (!fraw || fraw->IsZombie()) {
        std::cerr << "Cannot open raw fit file: " << raw_fit_file << std::endl;
        return;
    }
    if (!fcorr || fcorr->IsZombie()) {
        std::cerr << "Cannot open corr fit file: " << corr_fit_file << std::endl;
        return;
    }
    if (!fin || fin->IsZombie()) {
        std::cerr << "Cannot open test file: " << test_corr_file << std::endl;
        return;
    }

    TF1* f_pt_raw = (TF1*)fraw->Get("f_pt_dphi");
    TF1* f_pt_corr = (TF1*)fcorr->Get("f_pt_dphi");

    if (!f_pt_raw || !f_pt_corr) {
        std::cerr << "Cannot find f_pt_dphi in fit files." << std::endl;
        return;
    }

    TTree* tree = (TTree*)fin->Get(tree_name);
    if (!tree) {
        std::cerr << "Cannot find tree: " << tree_name << std::endl;
        return;
    }

    Result raw = make_result(
        tree, f_pt_raw, "raw", false,
        min_abs_dphi, n_pt_bins, pt_min, pt_max,
        n_res_bins, res_min, res_max, min_entries_per_bin
    );

    Result corr = make_result(
        tree, f_pt_corr, "corr", true,
        min_abs_dphi, n_pt_bins, pt_min, pt_max,
        n_res_bins, res_min, res_max, min_entries_per_bin
    );

    raw.bias->SetMarkerStyle(20);
    raw.bias->SetMarkerColor(kBlack);
    raw.bias->SetLineColor(kBlack);

    corr.bias->SetMarkerStyle(21);
    corr.bias->SetMarkerColor(kRed + 1);
    corr.bias->SetLineColor(kRed + 1);

    raw.res->SetMarkerStyle(20);
    raw.res->SetMarkerColor(kBlack);
    raw.res->SetLineColor(kBlack);

    corr.res->SetMarkerStyle(21);
    corr.res->SetMarkerColor(kRed + 1);
    corr.res->SetLineColor(kRed + 1);

    TCanvas* c_bias = new TCanvas("c_compare_bias", "bias comparison", 950, 650);
    c_bias->SetLeftMargin(0.13);
    c_bias->SetRightMargin(0.05);
    raw.bias->Draw("AP");
    raw.bias->GetYaxis()->SetTitle("Gaussian mean of (p_{T}^{reco}-p_{T}^{truth})/p_{T}^{truth}");
    raw.bias->GetXaxis()->SetLimits(pt_min, pt_max);
    corr.bias->Draw("P SAME");

    TLine* l0 = new TLine(pt_min, 0.0, pt_max, 0.0);
    l0->SetLineStyle(2);
    l0->Draw("SAME");

    TLegend* leg_bias = new TLegend(0.15, 0.72, 0.45, 0.88);
    leg_bias->AddEntry(raw.bias, "Raw position", "lp");
    leg_bias->AddEntry(corr.bias, "Corrected position", "lp");
    leg_bias->Draw();

    c_bias->SaveAs("compare_bias_raw_corr.png");

    TCanvas* c_res = new TCanvas("c_compare_resolution", "resolution comparison", 950, 650);
    c_res->SetLeftMargin(0.13);
    c_res->SetRightMargin(0.05);
    raw.res->Draw("AP");
    raw.res->GetYaxis()->SetTitle("Gaussian #sigma of (p_{T}^{reco}-p_{T}^{truth})/p_{T}^{truth}");
    raw.res->GetXaxis()->SetLimits(pt_min, pt_max);
    corr.res->Draw("P SAME");

    TLegend* leg_res = new TLegend(0.15, 0.72, 0.45, 0.88);
    leg_res->AddEntry(raw.res, "Raw position", "lp");
    leg_res->AddEntry(corr.res, "Corrected position", "lp");
    leg_res->Draw();

    c_res->SaveAs("compare_resolution_raw_corr.png");

    TGraphErrors* gr_improve = new TGraphErrors();
    gr_improve->SetName("gr_sigma_improvement");
    gr_improve->SetTitle(";Truth p_{T} [GeV];#sigma improvement ratio");

    int ip_imp = 0;

    for (int i = 0; i < raw.res->GetN(); ++i) {
        double x_raw, y_raw;
        raw.res->GetPoint(i, x_raw, y_raw);

        for (int j = 0; j < corr.res->GetN(); ++j) {
            double x_corr, y_corr;
            corr.res->GetPoint(j, x_corr, y_corr);

            if (std::abs(x_raw - x_corr) > 1.0e-6) continue;
            if (y_raw <= 0.0) continue;

            const double err_raw = raw.res->GetErrorY(i);
            const double err_corr = corr.res->GetErrorY(j);

            const double improve = (y_raw - y_corr) / y_raw;

            const double improve_err = std::sqrt(
                std::pow(y_corr / (y_raw * y_raw) * err_raw, 2) +
                std::pow(err_corr / y_raw, 2)
            );

            gr_improve->SetPoint(ip_imp, x_raw, improve);
            gr_improve->SetPointError(ip_imp, raw.res->GetErrorX(i), improve_err);
            ++ip_imp;
            break;
        }
    }

    gr_improve->SetMarkerStyle(20);
    gr_improve->SetMarkerSize(1.0);

    TCanvas* c_improve = new TCanvas("c_sigma_improvement", "sigma improvement", 950, 650);
    c_improve->SetLeftMargin(0.13);
    c_improve->SetRightMargin(0.05);
    gr_improve->Draw("AP");
    gr_improve->GetYaxis()->SetTitle("(#sigma_{raw} - #sigma_{corr}) / #sigma_{raw}");
    gr_improve->GetXaxis()->SetTitle("Truth p_{T} [GeV]");
    gr_improve->GetXaxis()->SetLimits(pt_min, pt_max);

    TLine* l_imp0 = new TLine(pt_min, 0.0, pt_max, 0.0);
    l_imp0->SetLineStyle(2);
    l_imp0->Draw("SAME");

    c_improve->SaveAs("compare_sigma_improvement_raw_corr.png");

    TFile* fout = TFile::Open(output_name, "RECREATE");
    raw.h2->Write();
    corr.h2->Write();
    raw.bias->Write();
    corr.bias->Write();
    raw.res->Write();
    corr.res->Write();
    gr_improve->Write();
    c_bias->Write();
    c_res->Write();
    c_improve->Write();
    fout->Close();

    fin->Close();
    fraw->Close();
    fcorr->Close();

    std::cout << "Wrote: " << output_name << std::endl;
}