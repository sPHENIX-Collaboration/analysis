// compare_mass_ptbins.C
// Shows invariant mass distributions in each smearing pT bin for:
//   1. Data                       (cuts pre-applied)
//   2. SV sim filtered            (cuts pre-applied)
//   3. SV sim filtered + smeared  (cuts pre-applied)
// All histograms normalised to unit area within each bin.
// Produces one canvas per particle (K0S and Lambda), 4x2 layout:
//   7 pads = 7 pT bins,  8th pad = shared legend.
//
// Run with:  root -l -b -q compare_mass_ptbins.C

#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TPaveText.h"
#include "TLine.h"
#include "TStyle.h"
#include <cmath>
#include <ctime>
#include <sstream>
#include <iostream>

// ── pT bins (same as used in smearing systematic) ────────────────────────────
const int    NBINS   = 7;
const double PT_LO[] = {0.5, 0.8, 1.1, 1.4, 1.8, 2.2, 3.0};
const double PT_HI[] = {0.8, 1.1, 1.4, 1.8, 2.2, 3.0, 4.0};

// ── date ─────────────────────────────────────────────────────────────────────
std::string getDateStr()
{
    std::time_t t = std::time(0); std::tm* n = std::localtime(&t);
    std::stringstream ss;
    ss << (n->tm_mon+1) << '/' << n->tm_mday << '/' << (n->tm_year+1900);
    return ss.str();
}

// ── style ─────────────────────────────────────────────────────────────────────
void hStyle(TH1F* h, Color_t col, Style_t ls, float lw=2)
{
    h->SetLineColor(col); h->SetLineWidth(lw); h->SetLineStyle(ls);
    h->SetFillStyle(0);
}

// ── fill mass-in-pT-bins from any file (no cuts applied) ─────────────────────
void fillMassFiltered(TH1F* h[], const char* fname,
                      const char* pt_br, const char* mass_br)
{
    TFile* f = TFile::Open(fname,"READ");
    if (!f || f->IsZombie()) { std::cerr << "Cannot open " << fname << "\n"; return; }
    TTree* t = (TTree*)f->Get("DecayTree");
    if (!t) { f->Close(); return; }

    float pt, mass;
    t->SetBranchAddress(pt_br,   &pt);
    t->SetBranchAddress(mass_br, &mass);

    Long64_t N = t->GetEntries();
    for (Long64_t i = 0; i < N; ++i) {
        t->GetEntry(i);
        float m_mev = mass * 1000.f;
        for (int b = 0; b < NBINS; ++b)
            if (pt >= PT_LO[b] && pt < PT_HI[b]) { h[b]->Fill(m_mev); break; }
    }
    f->Close();
    std::cout << "  " << fname << " : " << N << " events\n";
}

// ── draw one pad ─────────────────────────────────────────────────────────────
void drawBinPad(TVirtualPad* pad, TH1F* hD, TH1F* hF, TH1F* hS,
                int bin_idx, const char* mass_title, bool fit_gauss = false)
{
    pad->cd();
    pad->SetLeftMargin(0.14); pad->SetRightMargin(0.04);
    pad->SetBottomMargin(0.16); pad->SetTopMargin(0.08);

    // normalise to unit area
    for (TH1F* h : {hD, hF, hS})
        if (h->Integral() > 0) h->Scale(1.0 / h->Integral());

    hStyle(hD, kBlack,   1, 2);
    hStyle(hF, kAzure+7, 2, 2);
    hStyle(hS, kRed+1,   1, 2);

    double ymax = 1.25 * std::max({hD->GetMaximum(), hF->GetMaximum(), hS->GetMaximum()});
    if (ymax == 0) ymax = 1.0;

    hD->GetXaxis()->SetTitle(mass_title);
    hD->GetYaxis()->SetTitle("Norm. counts");
    hD->GetYaxis()->SetRangeUser(0, ymax);
    hD->GetXaxis()->SetTitleSize(0.07);
    hD->GetYaxis()->SetTitleSize(0.065);
    hD->GetXaxis()->SetLabelSize(0.065);
    hD->GetYaxis()->SetLabelSize(0.065);
    hD->GetXaxis()->SetTitleOffset(1.0);
    hD->GetYaxis()->SetTitleOffset(0.95);
    hD->GetYaxis()->SetNdivisions(505);

    hD->Draw("HIST");
    hF->Draw("HIST SAME");
    hS->Draw("HIST SAME");

    // pT bin label
    TLatex tex; tex.SetNDC(); tex.SetTextSize(0.065); tex.SetTextFont(42);
    char label[64];
    if (PT_HI[bin_idx] < 9)
        snprintf(label, sizeof(label), "%.1f < p_{T} < %.1f GeV", PT_LO[bin_idx], PT_HI[bin_idx]);
    else
        snprintf(label, sizeof(label), "p_{T} > %.1f GeV", PT_LO[bin_idx]);
    tex.DrawLatex(0.18, 0.82, label);

    if (fit_gauss) {
        const double FIT_LO = 475, FIT_HI = 520;
        struct Entry { TH1F* h; Color_t col; const char* tag; };
        Entry entries[] = {
            {hD, kBlack,   "Data"},
            {hF, kAzure+7, "Sim "},
            {hS, kRed+1,   "Smr "}
        };
        TLatex stex; stex.SetNDC(); stex.SetTextFont(42); stex.SetTextSize(0.058);
        double y = 0.71;
        for (auto& e : entries) {
            if (e.h->Integral() <= 0) { y -= 0.095; continue; }
            TF1* g = new TF1(Form("gfit_%d_%s", bin_idx, e.tag),
                             "gaus", FIT_LO, FIT_HI);
            e.h->Fit(g, "RQ0");
            stex.SetTextColor(e.col);
            char buf[64];
            snprintf(buf, sizeof(buf), "%s #sigma = %.1f MeV", e.tag, g->GetParameter(2));
            stex.DrawLatex(0.18, y, buf);
            y -= 0.095;
        }
    }
}

// ── draw legend pad ───────────────────────────────────────────────────────────
void drawLegendPad(TVirtualPad* pad)
{
    pad->cd();
    pad->SetLeftMargin(0.05); pad->SetRightMargin(0.05);

    TLegend* leg = new TLegend(0.08, 0.28, 0.92, 0.72);
    leg->SetBorderSize(0); leg->SetFillStyle(0); leg->SetTextSize(0.075);
    // dummy hists for legend entries
    TH1F* dD = new TH1F("dD","",1,0,1); hStyle(dD, kBlack,   1, 2);
    TH1F* dF = new TH1F("dF","",1,0,1); hStyle(dF, kAzure+7, 2, 2);
    TH1F* dS = new TH1F("dS","",1,0,1); hStyle(dS, kRed+1,   1, 2);
    leg->AddEntry(dD, "Data",                    "l");
    leg->AddEntry(dF, "SV sim (filtered)",       "l");
    leg->AddEntry(dS, "SV sim (filt. + smeared)","l");
    leg->Draw();

    // sPHENIX label
    TLatex tex; tex.SetNDC(); tex.SetTextFont(42); tex.SetTextSize(0.085);
    tex.DrawLatex(0.08, 0.85, "#it{#bf{sPHENIX}} Internal");
    tex.SetTextSize(0.075);
    tex.DrawLatex(0.08, 0.76, "#it{p}+#it{p}  #sqrt{s} = 200 GeV");
    tex.SetTextColor(kGray+2); tex.SetTextSize(0.070);
    tex.DrawLatex(0.08, 0.18, getDateStr().c_str());
}

// ── build and save one full canvas ───────────────────────────────────────────
void makeParticleCanvas(
    TH1F* hD[], TH1F* hF[], TH1F* hS[],
    const char* cname, const char* save_base, const char* mass_title,
    bool fit_gauss = false)
{
    TCanvas* c = new TCanvas(cname, cname, 1600, 800);
    c->Divide(4, 2, 0.003, 0.003);

    for (int b = 0; b < NBINS; ++b)
        drawBinPad(c->GetPad(b+1), hD[b], hF[b], hS[b], b, mass_title, fit_gauss);

    drawLegendPad(c->GetPad(8));

    TString pdf = TString(save_base) + ".pdf";
    TString png = TString(save_base) + ".png";
    c->SaveAs(pdf); c->SaveAs(png);
    std::cout << "Saved " << save_base << ".pdf/.png\n";
}

// ── main ──────────────────────────────────────────────────────────────────────
void compare_mass_ptbins()
{
    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(0);

    const char* KS_DATA   = "KShort6RunCombined.root";
    const char* KS_FILT   = "outputKFParticleKShortRecoSV_filtered.root";
    //const char* KS_SMEAR  = "outputKFParticleKShortRecoSV_filtered_smeared.root";
    const char* KS_SMEAR = "outputKFParticleKShortRecoSV_filtered_masssmeared.root";
    const char* LAM_DATA  = "Lambda6RunCombined.root";
    const char* LAM_FILT  = "outputKFParticleLambda0SV_filtered.root";
    const char* LAM_SMEAR = "outputKFParticleLambda0SV_filtered_smeared.root";

    // ── K0S ─────────────────────────────────────────────────────────────────
    std::cout << "\n=== K0S mass in pT bins ===\n";

    const double KS_LO = 450, KS_HI = 550;
    const int    KS_NB = 50;   // 2 MeV/bin

    TH1F* ksD[NBINS], *ksF[NBINS], *ksS[NBINS];
    for (int b = 0; b < NBINS; ++b) {
        ksD[b] = new TH1F(Form("ksD%d",b), "", KS_NB, KS_LO, KS_HI);
        ksF[b] = new TH1F(Form("ksF%d",b), "", KS_NB, KS_LO, KS_HI);
        ksS[b] = new TH1F(Form("ksS%d",b), "", KS_NB, KS_LO, KS_HI);
    }

    fillMassFiltered(ksD, KS_DATA,  "K_S0_pT", "K_S0_mass");
    fillMassFiltered(ksF, KS_FILT,  "K_S0_pT", "K_S0_mass");
    fillMassFiltered(ksS, KS_SMEAR, "K_S0_pT", "K_S0_mass");

    makeParticleCanvas(ksD, ksF, ksS,
                       "cKS", "compare_ks_mass_ptbins",
                       "K_{S}^{0} mass (MeV/#it{c}^{2})", true);

    // ── Lambda ───────────────────────────────────────────────────────────────
    std::cout << "\n=== Lambda mass in pT bins ===\n";

    const double LAM_LO = 1090, LAM_HI = 1150;
    const int    LAM_NB = 60;   // 1 MeV/bin

    TH1F* lmD[NBINS], *lmF[NBINS], *lmS[NBINS];
    for (int b = 0; b < NBINS; ++b) {
        lmD[b] = new TH1F(Form("lmD%d",b), "", LAM_NB, LAM_LO, LAM_HI);
        lmF[b] = new TH1F(Form("lmF%d",b), "", LAM_NB, LAM_LO, LAM_HI);
        lmS[b] = new TH1F(Form("lmS%d",b), "", LAM_NB, LAM_LO, LAM_HI);
    }

    fillMassFiltered(lmD, LAM_DATA,  "Lambda0_pT", "Lambda0_mass");
    fillMassFiltered(lmF, LAM_FILT,  "Lambda0_pT", "Lambda0_mass");
    fillMassFiltered(lmS, LAM_SMEAR, "Lambda0_pT", "Lambda0_mass");

    makeParticleCanvas(lmD, lmF, lmS,
                       "cLam", "compare_lam_mass_ptbins",
                       "#Lambda^{0} mass (MeV/#it{c}^{2})");

    std::cout << "\nAll done.\n";
}
