#!/usr/bin/env python3
"""
filtered_smear_syst.py
----------------------
Estimates the smearing systematic uncertainty on K0S and Xi yields
using the pre-filtered simulation files (quality cuts already applied).
No additional cuts are needed.

For each particle, counts events in bins of pT, computes
  ratio = N_smeared / N_raw
  syst% = |ratio - 1| * 100

Generates:
  filtered_ks_syst.txt
  filtered_xi_syst.txt
  plot_filtered_ks_syst.C
  plot_filtered_xi_syst.C
  plot_filtered_combined_syst.C   (both particles on one canvas)

Run with:  python3 filtered_smear_syst.py
"""

import numpy as np
import uproot

# ── files ─────────────────────────────────────────────────────────────────────
KS_RAW    = "outputKFParticleKShortRecoSV_filtered.root"
KS_SMEAR  = "outputKFParticleKShortRecoSV_smeared.root"
XI_RAW   = "outputKFParticleXiminusSV_filtered.root"
XI_SMEAR = "outputKFParticleXiSV_smeared.root"
TREE      = "DecayTree"

# ── pT bins (GeV) ─────────────────────────────────────────────────────────────
PT_BINS = [0.75, 1.07, 1.25, 1.49, 2.0]


# ── core logic ────────────────────────────────────────────────────────────────

def load(filepath, pt_branch, mass_branch):
    with uproot.open(filepath) as f:
        d = f[TREE].arrays([pt_branch, mass_branch], library="np")
    print(f"  {len(d[pt_branch]):,} events  ←  {filepath}")
    return d


def run_syst(raw_file, smear_file, pt_branch, mass_branch, label):
    print(f"\n{'─'*60}")
    print(f"  {label} systematic")
    print(f"{'─'*60}")

    d_raw = load(raw_file,   pt_branch, mass_branch)
    d_smr = load(smear_file, pt_branch, mass_branch)

    results = []
    n_bins  = len(PT_BINS) - 1

    hdr = (f"{'pT bin':>16}  {'avg_pT':>7}  "
           f"{'N_raw':>8}  {'N_smear':>8}  "
           f"{'ratio':>7}  {'syst%':>7}")
    print("\n" + hdr)
    print("─" * len(hdr))

    for i in range(n_bins):
        lo, hi = PT_BINS[i], PT_BINS[i+1]

        sel_r = (d_raw[pt_branch] >= lo) & (d_raw[pt_branch] < hi)
        sel_s = (d_smr[pt_branch] >= lo) & (d_smr[pt_branch] < hi)

        nr = int(sel_r.sum())
        ns = int(sel_s.sum())

        avg_pt = float(d_raw[pt_branch][sel_r].mean()) if nr > 0 else (lo + hi) / 2

        if nr == 0:
            print(f"  [{lo:.1f},{hi:.1f})  — no raw events")
            results.append(dict(lo=lo, hi=hi, avg_pt=avg_pt,
                                nr=0, ns=ns, ratio=np.nan, syst=np.nan,
                                ratio_stat=np.nan))
            continue

        ratio      = ns / nr
        syst       = abs(ratio - 1.0) * 100.0
        ratio_stat = ratio * np.sqrt(1.0/nr + 1.0/ns) if ns > 0 else np.nan

        print(f"  [{lo:.1f},{hi:.1f})  {avg_pt:>7.3f}  "
              f"{nr:>8d}  {ns:>8d}  "
              f"{ratio:>7.4f}  {syst:>7.2f}%")

        results.append(dict(lo=lo, hi=hi, avg_pt=avg_pt,
                            nr=nr, ns=ns,
                            ratio=ratio, ratio_stat=ratio_stat, syst=syst))

    valid = [r for r in results if not np.isnan(r["ratio"])]
    if valid:
        print(f"\n  Range: {min(r['syst'] for r in valid):.2f}%"
              f" – {max(r['syst'] for r in valid):.2f}%")
        print(f"  Mean:  {np.mean([r['syst'] for r in valid]):.2f}%")

    return results


# ── text output ───────────────────────────────────────────────────────────────

def save_table(results, label, path):
    with open(path, "w") as fh:
        fh.write(f"# {label} smearing systematic (pre-filtered MC, no fit)\n")
        fh.write("# ratio = N_smeared / N_raw per pT bin\n")
        fh.write("# syst_pct = |ratio - 1| * 100\n\n")
        fh.write("bin_lo,bin_hi,avg_pt,N_raw,N_smeared,ratio,ratio_stat_err,syst_pct\n")
        for r in results:
            if np.isnan(r["ratio"]):
                fh.write(f"{r['lo']:.2f},{r['hi']:.2f},{r['avg_pt']:.4f},"
                         f"{r['nr']},{r['ns']},nan,nan,nan\n")
            else:
                fh.write(f"{r['lo']:.2f},{r['hi']:.2f},{r['avg_pt']:.4f},"
                         f"{r['nr']},{r['ns']},"
                         f"{r['ratio']:.6f},{r['ratio_stat']:.6f},"
                         f"{r['syst']:.4f}\n")
    print(f"  Table → {path}")


# ── ROOT macro helpers ────────────────────────────────────────────────────────

HEADER = """#include <ctime>
#include <sstream>
#include <algorithm>
std::string getDate2(){
    std::time_t t=std::time(0); std::tm* now=std::localtime(&t);
    std::stringstream d;
    d<<(now->tm_mon+1)<<'/'<<now->tm_mday<<'/'<<(now->tm_year+1900);
    return d.str();
}
auto addLabel=[](double x1,double y1,double x2,double y2){
    TPaveText *pt=new TPaveText(x1,y1,x2,y2,"NDC");
    pt->SetFillStyle(0); pt->SetBorderSize(0); pt->SetTextFont(42);
    pt->AddText("#it{#bf{sPHENIX}} Internal,  #it{p}+#it{p}  #sqrt{s} = 200 GeV");
    pt->Draw();
};
auto addDate=[](double x1,double y1,double x2,double y2){
    TPaveText *pt=new TPaveText(x1,y1,x2,y2,"NDC");
    pt->SetFillStyle(0); pt->SetBorderSize(0); pt->SetTextFont(42);
    pt->AddText(getDate2().c_str()); pt->Draw();
};
"""


def arrays_str(results, key, fmt=".4f"):
    valid = [r for r in results if not np.isnan(r["ratio"])]
    return ", ".join(f"{r[key]:{fmt}}" for r in valid)


def write_single_macro(results, label, pt_label, path):
    """Three-canvas macro: ratio, counts, syst% for one particle."""
    valid = [r for r in results if not np.isnan(r["ratio"])]
    N    = len(valid)
    pts  = arrays_str(results, "avg_pt")
    rats = arrays_str(results, "ratio",      ".6f")
    rerr = arrays_str(results, "ratio_stat", ".6f")
    syst = arrays_str(results, "syst",       ".4f")
    nr   = ", ".join(str(r["nr"]) for r in valid)
    ns   = ", ".join(str(r["ns"]) for r in valid)
    xerr = ", ".join(f"{(r['hi']-r['lo'])/2:.3f}" for r in valid)

    macro = f"""// {path}
// {label} yield ratio (smeared / raw) vs pT — smearing systematic.
// Pre-filtered MC: no additional cuts applied.
// Run with:  root -l -b -q {path}

{HEADER}
void {path.replace('.C','')}() {{
    gStyle->SetOptStat(0); gStyle->SetOptTitle(0);

    const int N = {N};
    double avg_pt[]    = {{{pts}}};
    double ratio[]     = {{{rats}}};
    double ratio_err[] = {{{rerr}}};
    double xerr[]      = {{{xerr}}};
    double nr[]        = {{{nr}}};
    double ns[]        = {{{ns}}};
    double syst_pct[]  = {{{syst}}};

    // ── canvas 1: yield ratio ────────────────────────────────────────────────
    TCanvas *c1 = new TCanvas("c1","{label} yield ratio",900,650);
    c1->SetLeftMargin(0.13); c1->SetBottomMargin(0.13); c1->SetRightMargin(0.06);

    TGraphErrors *gR = new TGraphErrors(N,avg_pt,ratio,xerr,ratio_err);
    gR->SetMarkerStyle(20); gR->SetMarkerSize(1.4);
    gR->SetMarkerColor(kRed+1); gR->SetLineColor(kRed+1); gR->SetLineWidth(2);
    gR->Draw("APE");
    gR->GetXaxis()->SetTitle("{pt_label}");
    gR->GetYaxis()->SetTitle("N_{{smeared}} / N_{{raw}}");
    gR->GetYaxis()->SetRangeUser(0.7,1.3);
    gR->GetXaxis()->SetTitleSize(0.05); gR->GetYaxis()->SetTitleSize(0.05);

    TLine *unity = new TLine({PT_BINS[0]},1.0,{PT_BINS[-1]},1.0);
    unity->SetLineStyle(2); unity->SetLineColor(kGray+1); unity->SetLineWidth(2);
    unity->Draw();
    TBox *band = new TBox({PT_BINS[0]},0.95,{PT_BINS[-1]},1.05);
    band->SetFillColorAlpha(kGray,0.30); band->SetLineWidth(0); band->Draw();
    gR->Draw("PE SAME");

    addLabel(0.14,0.88,0.65,0.96);
    addDate(0.70,0.88,0.98,0.96);
    TLegend *leg1 = new TLegend(0.14,0.74,0.70,0.87);
    leg1->SetBorderSize(0); leg1->SetFillStyle(0); leg1->SetTextSize(0.030);
    leg1->AddEntry(gR,   "N_{{smeared}}/N_{{raw}}  (stat. err)","lpe");
    leg1->AddEntry(unity,"Unity","l");
    leg1->AddEntry(band, "#pm5% band","f");
    leg1->Draw();

    c1->SaveAs("{path.replace('.C','')}_ratio.pdf");
    c1->SaveAs("{path.replace('.C','')}_ratio.png");
    std::cout << "Saved {path.replace('.C','')}_ratio\\n";

    // ── canvas 2: raw vs smeared counts ─────────────────────────────────────
    TCanvas *c2 = new TCanvas("c2","{label} counts",900,650);
    c2->SetLeftMargin(0.13); c2->SetBottomMargin(0.13); c2->SetRightMargin(0.06);

    TGraph *gRaw = new TGraph(N,avg_pt,nr);
    TGraph *gSmr = new TGraph(N,avg_pt,ns);
    gRaw->SetMarkerStyle(20); gRaw->SetMarkerSize(1.4);
    gRaw->SetMarkerColor(kBlue+1); gRaw->SetLineColor(kBlue+1); gRaw->SetLineWidth(2);
    gSmr->SetMarkerStyle(22); gSmr->SetMarkerSize(1.4);
    gSmr->SetMarkerColor(kRed+1);  gSmr->SetLineColor(kRed+1);  gSmr->SetLineWidth(2);

    TMultiGraph *mg = new TMultiGraph();
    mg->Add(gRaw,"PL"); mg->Add(gSmr,"PL"); mg->Draw("A");
    mg->GetXaxis()->SetTitle("{pt_label}");
    mg->GetYaxis()->SetTitle("Events (filtered)");
    mg->GetXaxis()->SetTitleSize(0.05); mg->GetYaxis()->SetTitleSize(0.05);

    TLegend *leg2 = new TLegend(0.50,0.72,0.92,0.88);
    leg2->SetBorderSize(0); leg2->SetFillStyle(0); leg2->SetTextSize(0.032);
    leg2->AddEntry(gRaw,"Filtered raw",    "lp");
    leg2->AddEntry(gSmr,"Filtered smeared","lp");
    leg2->Draw();
    addLabel(0.14,0.88,0.65,0.96);
    addDate(0.70,0.88,0.98,0.96);

    c2->SaveAs("{path.replace('.C','')}_counts.pdf");
    c2->SaveAs("{path.replace('.C','')}_counts.png");
    std::cout << "Saved {path.replace('.C','')}_counts\\n";

    // ── canvas 3: systematic % ───────────────────────────────────────────────
    TCanvas *c3 = new TCanvas("c3","{label} syst%",900,650);
    c3->SetLeftMargin(0.13); c3->SetBottomMargin(0.13); c3->SetRightMargin(0.06);

    TGraph *gS = new TGraph(N,avg_pt,syst_pct);
    gS->SetMarkerStyle(20); gS->SetMarkerSize(1.4);
    gS->SetMarkerColor(kMagenta+1); gS->SetLineColor(kMagenta+1); gS->SetLineWidth(2);
    gS->Draw("APL");
    gS->GetXaxis()->SetTitle("{pt_label}");
    gS->GetYaxis()->SetTitle("Smearing systematic |#DeltaN/N| (%)");
    double ymax = *std::max_element(syst_pct,syst_pct+N)*1.35 + 0.5;
    gS->GetYaxis()->SetRangeUser(0,ymax);
    gS->GetXaxis()->SetTitleSize(0.05); gS->GetYaxis()->SetTitleSize(0.05);
    addLabel(0.14,0.88,0.65,0.96);
    addDate(0.70,0.88,0.98,0.96);

    c3->SaveAs("{path.replace('.C','')}_pct.pdf");
    c3->SaveAs("{path.replace('.C','')}_pct.png");
    std::cout << "Saved {path.replace('.C','')}_pct\\n";
}}
"""
    with open(path, "w") as fh:
        fh.write(macro)
    print(f"  Macro  → {path}")


def write_combined_macro(ks_results, xi_results, path):
    """Single canvas overlay: K0S and Xi syst% on same axes."""
    ks_v  = [r for r in ks_results  if not np.isnan(r["ratio"])]
    xi_v = [r for r in xi_results if not np.isnan(r["ratio"])]

    def arr(lst, key, fmt=".4f"):
        return ", ".join(f"{r[key]:{fmt}}" for r in lst)

    Nk = len(ks_v);  Nx = len(xi_v)

    macro = f"""// {path}
// Smearing systematic (|ΔN/N|%) for K0S and Xi on one canvas.
// Run with:  root -l -b -q {path}

{HEADER}
void {path.replace('.C','')}() {{
    gStyle->SetOptStat(0); gStyle->SetOptTitle(0);

    const int Nk = {Nk}, Nx = {Nx};
    double ks_pt[]   = {{{arr(ks_v,  'avg_pt')}}};
    double ks_syst[] = {{{arr(ks_v,  'syst')}}};
    double xi_pt[]  = {{{arr(xi_v, 'avg_pt')}}};
    double xi_syst[]= {{{arr(xi_v, 'syst')}}};

    TCanvas *c = new TCanvas("c","Smearing systematic",900,650);
    c->SetLeftMargin(0.13); c->SetBottomMargin(0.13); c->SetRightMargin(0.06);

    TGraph *gK = new TGraph(Nk, ks_pt,  ks_syst);
    TGraph *gX = new TGraph(Nx, xi_pt, xi_syst);

    gK->SetMarkerStyle(20); gK->SetMarkerSize(1.4);
    gK->SetMarkerColor(kBlue+1);  gK->SetLineColor(kBlue+1);  gK->SetLineWidth(2);
    gX->SetMarkerStyle(21); gX->SetMarkerSize(1.4);
    gX->SetMarkerColor(kRed+1);   gX->SetLineColor(kRed+1);   gX->SetLineWidth(2);

    TMultiGraph *mg = new TMultiGraph();
    mg->Add(gK,"PL"); mg->Add(gX,"PL"); mg->Draw("A");
    mg->GetXaxis()->SetTitle("#it{{p}}_{{T}} (GeV/#it{{c}})");
    mg->GetYaxis()->SetTitle("Smearing systematic |#DeltaN/N| (%)");
    mg->GetXaxis()->SetTitleSize(0.05); mg->GetYaxis()->SetTitleSize(0.05);

    TLegend *leg = new TLegend(0.14,0.74,0.55,0.87);
    leg->SetBorderSize(0); leg->SetFillStyle(0); leg->SetTextSize(0.032);
    leg->AddEntry(gK,"K_{{S}}^{{0}} smearing syst.","lp");
    leg->AddEntry(gL,"#Xi^{{-}} smearing syst.","lp");
    leg->Draw();

    addLabel(0.14,0.88,0.65,0.96);
    addDate(0.70,0.88,0.98,0.96);

    c->SaveAs("{path.replace('.C','')}.pdf");
    c->SaveAs("{path.replace('.C','')}.png");
    std::cout << "Saved {path.replace('.C','')}\\n";
}}
"""
    with open(path, "w") as fh:
        fh.write(macro)
    print(f"  Macro  → {path}")


# ── main ──────────────────────────────────────────────────────────────────────

def main():
    ks_results  = run_syst(KS_RAW,  KS_SMEAR,
                           "K_S0_pT",    "K_S0_mass",    "K0S")
    xi_results = run_syst(XI_RAW, XI_SMEAR,
                           "Ximinus_pT", "Ximinus_mass", "Xi")

    print("\n" + "─"*60)
    save_table(ks_results,  "K0S",    "filtered_ks_syst.txt")
    save_table(xi_results, "Xi", "filtered_xi_syst.txt")

    write_single_macro(ks_results,  "K0S",
                       "K_{S}^{0} #it{p}_{T} (GeV/#it{c})",
                       "plot_filtered_ks_syst.C")
    write_single_macro(xi_results, "Xi",
                       "#Xi^{-} #it{p}_{T} (GeV/#it{c})",
                       "plot_filtered_xi_syst.C")
    write_combined_macro(ks_results, xi_results,
                         "plot_filtered_combined_syst.C")

    print("\nTo plot:")
    print("  root -l -b -q plot_filtered_ks_syst.C")
    print("  root -l -b -q plot_filtered_xi_syst.C")
    print("  root -l -b -q plot_filtered_combined_syst.C")


if __name__ == "__main__":
    main()
