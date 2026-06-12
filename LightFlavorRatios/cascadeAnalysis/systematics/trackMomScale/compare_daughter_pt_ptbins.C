// compare_daughter_pt_ptbins.C
// Shows daughter track pT distributions in each smearing pT bin for:
//   1. Data                       (quality cuts applied)
//   2. SV sim filtered            (cuts pre-applied)
//   3. SV sim filtered + smeared  (cuts pre-applied)
// All histograms normalised to unit area within each bin.
// Produces 4 canvases (4x2 layout, 7 pT bins + legend pad):
//   compare_ks_t1pt_ptbins   — K0S  track_1_pT (π⁺) in each K0S pT bin
//   compare_ks_t2pt_ptbins   — K0S  track_2_pT (π⁻) in each K0S pT bin
//   compare_lam_t1pt_ptbins  — Λ⁰   track_1_pT (π⁻) in each Λ⁰ pT bin
//   compare_lam_t2pt_ptbins  — Λ⁰   track_2_pT (p)  in each Λ⁰ pT bin
//
// Run with:  root -l -b -q compare_daughter_pt_ptbins.C

#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TPaveText.h"
#include "TStyle.h"
#include <cmath>
#include <ctime>
#include <sstream>
#include <iostream>

// ── pT bins ───────────────────────────────────────────────────────────────────
const int    NBINS   = 7;
const double PT_LO[] = {0.5, 0.8, 1.1, 1.4, 1.8, 2.2, 3.0};
const double PT_HI[] = {0.8, 1.1, 1.4, 1.8, 2.2, 3.0, 4.0};

// ── daughter pT histogram range ───────────────────────────────────────────────
const int    DPT_NB = 60;
const double DPT_LO = 0.0, DPT_HI = 3.0;   // pion
const double PRO_LO = 0.0, PRO_HI = 4.0;   // proton

// ── date ─────────────────────────────────────────────────────────────────────
std::string getDateStr3()
{
    std::time_t t = std::time(0); std::tm* n = std::localtime(&t);
    std::stringstream ss;
    ss << (n->tm_mon+1) << '/' << n->tm_mday << '/' << (n->tm_year+1900);
    return ss.str();
}

// ── style ─────────────────────────────────────────────────────────────────────
void hSty3(TH1F* h, Color_t col, Style_t ls, float lw=2)
{
    h->SetLineColor(col); h->SetLineWidth(lw); h->SetLineStyle(ls);
    h->SetFillStyle(0);
}

// ── fill daughter pT from any file (no cuts applied) ─────────────────────────
// h1[b] = track_1_pT in parent-pT bin b
// h2[b] = track_2_pT in parent-pT bin b
void fillDptFiltered(TH1F* h1[], TH1F* h2[], const char* fname,
                     const char* parent_pt_br)
{
    TFile* f = TFile::Open(fname,"READ");
    if (!f||f->IsZombie()) { std::cerr<<"Cannot open "<<fname<<"\n"; return; }
    TTree* t = (TTree*)f->Get("DecayTree");
    if (!t) { f->Close(); return; }

    float ppt, dpt1, dpt2;
    t->SetBranchAddress(parent_pt_br,  &ppt);
    t->SetBranchAddress("track_1_pT",  &dpt1);
    t->SetBranchAddress("track_2_pT",  &dpt2);

    Long64_t N = t->GetEntries();
    for (Long64_t i = 0; i < N; ++i) {
        t->GetEntry(i);
        for (int b = 0; b < NBINS; ++b) {
            if (ppt >= PT_LO[b] && ppt < PT_HI[b]) {
                h1[b]->Fill(dpt1);
                h2[b]->Fill(dpt2);
                break;
            }
        }
    }
    f->Close();
    std::cout << "  " << fname << " : " << N << " events\n";
}

// ── draw one pad ─────────────────────────────────────────────────────────────
void drawDptPad(TVirtualPad* pad, TH1F* hD, TH1F* hF, TH1F* hS,
                int bin_idx, const char* xtitle)
{
    pad->cd();
    pad->SetLeftMargin(0.14); pad->SetRightMargin(0.04);
    pad->SetBottomMargin(0.16); pad->SetTopMargin(0.08);

    for (TH1F* h : {hD, hF, hS})
        if (h->Integral() > 0) h->Scale(1.0 / h->Integral());

    hSty3(hD, kBlack,   1, 2);
    hSty3(hF, kAzure+7, 2, 2);
    hSty3(hS, kRed+1,   1, 2);

    double ymax = 1.25 * std::max({hD->GetMaximum(), hF->GetMaximum(), hS->GetMaximum()});
    if (ymax == 0) ymax = 1.0;

    hD->GetXaxis()->SetTitle(xtitle);
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

    TLatex tex; tex.SetNDC(); tex.SetTextSize(0.065); tex.SetTextFont(42);
    char label[64];
    snprintf(label, sizeof(label), "%.1f < p_{T} < %.1f GeV", PT_LO[bin_idx], PT_HI[bin_idx]);
    tex.DrawLatex(0.18, 0.82, label);
}

// ── legend pad ────────────────────────────────────────────────────────────────
void drawLegendPad3(TVirtualPad* pad, const char* track_label)
{
    pad->cd();
    pad->SetLeftMargin(0.05); pad->SetRightMargin(0.05);

    TLegend* leg = new TLegend(0.08, 0.28, 0.92, 0.72);
    leg->SetBorderSize(0); leg->SetFillStyle(0); leg->SetTextSize(0.075);
    TH1F* dD = new TH1F(Form("dD_%s",track_label),"",1,0,1); hSty3(dD,kBlack,  1,2);
    TH1F* dF = new TH1F(Form("dF_%s",track_label),"",1,0,1); hSty3(dF,kAzure+7,2,2);
    TH1F* dS = new TH1F(Form("dS_%s",track_label),"",1,0,1); hSty3(dS,kRed+1,  1,2);
    leg->AddEntry(dD,"Data",                    "l");
    leg->AddEntry(dF,"SV sim (filtered)",       "l");
    leg->AddEntry(dS,"SV sim (filt. + smeared)","l");
    leg->Draw();

    TLatex tex; tex.SetNDC(); tex.SetTextFont(42);
    tex.SetTextSize(0.085);
    tex.DrawLatex(0.08, 0.86, "#it{#bf{sPHENIX}} Internal");
    tex.SetTextSize(0.075);
    tex.DrawLatex(0.08, 0.78, "#it{p}+#it{p}  #sqrt{s} = 200 GeV");
    tex.SetTextColor(kGray+2); tex.SetTextSize(0.070);
    tex.DrawLatex(0.08, 0.18, getDateStr3().c_str());
}

// ── build canvas ──────────────────────────────────────────────────────────────
void makeCanvas3(TH1F* hD[], TH1F* hF[], TH1F* hS[],
                 const char* cname, const char* save_base,
                 const char* xtitle, const char* track_label)
{
    TCanvas* c = new TCanvas(cname, cname, 1600, 800);
    c->Divide(4, 2, 0.003, 0.003);

    for (int b = 0; b < NBINS; ++b)
        drawDptPad(c->GetPad(b+1), hD[b], hF[b], hS[b], b, xtitle);

    drawLegendPad3(c->GetPad(8), track_label);

    c->SaveAs(TString(save_base)+".pdf");
    c->SaveAs(TString(save_base)+".png");
    std::cout << "Saved " << save_base << ".pdf/.png\n";
}

// ── main ──────────────────────────────────────────────────────────────────────
void compare_daughter_pt_ptbins()
{
    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(0);

    const char* KS_DATA   = "KShort6RunCombined.root";
    const char* KS_FILT   = "outputKFParticleKShortRecoSV_filtered.root";
    const char* KS_SMEAR  = "outputKFParticleKShortRecoSV_filtered_smeared.root";
    const char* LAM_DATA  = "Lambda6RunCombined.root";
    const char* LAM_FILT  = "outputKFParticleLambda0SV_filtered.root";
    const char* LAM_SMEAR = "outputKFParticleLambda0SV_filtered_smeared.root";

    // ── K0S ──────────────────────────────────────────────────────────────────
    std::cout << "\n=== K0S daughter pT in K0S pT bins ===\n";

    TH1F *ksD1[NBINS], *ksD2[NBINS], *ksF1[NBINS], *ksF2[NBINS], *ksS1[NBINS], *ksS2[NBINS];
    for (int b = 0; b < NBINS; ++b) {
        ksD1[b] = new TH1F(Form("ksD1_%d",b),"", DPT_NB, DPT_LO, DPT_HI);
        ksD2[b] = new TH1F(Form("ksD2_%d",b),"", DPT_NB, DPT_LO, DPT_HI);
        ksF1[b] = new TH1F(Form("ksF1_%d",b),"", DPT_NB, DPT_LO, DPT_HI);
        ksF2[b] = new TH1F(Form("ksF2_%d",b),"", DPT_NB, DPT_LO, DPT_HI);
        ksS1[b] = new TH1F(Form("ksS1_%d",b),"", DPT_NB, DPT_LO, DPT_HI);
        ksS2[b] = new TH1F(Form("ksS2_%d",b),"", DPT_NB, DPT_LO, DPT_HI);
    }

    fillDptFiltered(ksD1, ksD2, KS_DATA,  "K_S0_pT");
    fillDptFiltered(ksF1, ksF2, KS_FILT,  "K_S0_pT");
    fillDptFiltered(ksS1, ksS2, KS_SMEAR, "K_S0_pT");

    makeCanvas3(ksD1, ksF1, ksS1, "cKS1",
                "compare_ks_t1pt_ptbins",
                "#pi^{+} #it{p}_{T} (GeV/#it{c})", "ks1");

    makeCanvas3(ksD2, ksF2, ksS2, "cKS2",
                "compare_ks_t2pt_ptbins",
                "#pi^{-} #it{p}_{T} (GeV/#it{c})", "ks2");

    // ── Lambda ───────────────────────────────────────────────────────────────
    std::cout << "\n=== Lambda daughter pT in Lambda pT bins ===\n";

    TH1F *lmD1[NBINS], *lmD2[NBINS], *lmF1[NBINS], *lmF2[NBINS], *lmS1[NBINS], *lmS2[NBINS];
    for (int b = 0; b < NBINS; ++b) {
        lmD1[b] = new TH1F(Form("lmD1_%d",b),"", DPT_NB, DPT_LO, DPT_HI);
        lmD2[b] = new TH1F(Form("lmD2_%d",b),"", DPT_NB, PRO_LO, PRO_HI);
        lmF1[b] = new TH1F(Form("lmF1_%d",b),"", DPT_NB, DPT_LO, DPT_HI);
        lmF2[b] = new TH1F(Form("lmF2_%d",b),"", DPT_NB, PRO_LO, PRO_HI);
        lmS1[b] = new TH1F(Form("lmS1_%d",b),"", DPT_NB, DPT_LO, DPT_HI);
        lmS2[b] = new TH1F(Form("lmS2_%d",b),"", DPT_NB, PRO_LO, PRO_HI);
    }

    fillDptFiltered(lmD1, lmD2, LAM_DATA, "Lambda0_pT");
    fillDptFiltered(lmF1, lmF2, LAM_FILT,  "Lambda0_pT");
    fillDptFiltered(lmS1, lmS2, LAM_SMEAR, "Lambda0_pT");

    makeCanvas3(lmD1, lmF1, lmS1, "cLam1",
                "compare_lam_t1pt_ptbins",
                "#pi^{-} #it{p}_{T} (GeV/#it{c})", "lam1");

    makeCanvas3(lmD2, lmF2, lmS2, "cLam2",
                "compare_lam_t2pt_ptbins",
                "p #it{p}_{T} (GeV/#it{c})", "lam2");

    std::cout << "\nAll done.\n";
}
