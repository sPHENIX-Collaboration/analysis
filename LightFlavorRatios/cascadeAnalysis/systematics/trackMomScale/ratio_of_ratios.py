#!/usr/bin/env python3
"""
ratio_of_ratios.py
------------------
Reads the smearing systematic tables for K0S and Lambda
(filtered_ks_syst.txt, filtered_lam_syst.txt) and computes the
ratio of their yield ratios per pT bin:

    R = (N_Lam_smear / N_Lam_raw) / (N_KS_smear / N_KS_raw)

R = 1 means smearing affects both species identically → full cancellation.
R != 1 shows the residual systematic on the Lambda/K0S ratio.

Stat error on R propagated as:
    σ_R / R = sqrt( (σ_ratio_lam / ratio_lam)^2 + (σ_ratio_ks / ratio_ks)^2 )

Outputs:
  ratio_of_ratios.txt
  plot_ratio_of_ratios.C
"""

import numpy as np

KS_TABLE  = "filtered_ks_syst.txt"
LAM_TABLE = "filtered_lam_syst.txt"


def read_table(path):
    rows = []
    with open(path) as fh:
        for line in fh:
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            if line.startswith("bin_lo"):
                cols = line.split(",")
                continue
            parts = line.split(",")
            d = {c: parts[i] for i, c in enumerate(cols)}
            if d["ratio"] == "nan":
                continue
            rows.append(dict(
                lo         = float(d["bin_lo"]),
                hi         = float(d["bin_hi"]),
                avg_pt     = float(d["avg_pt"]),
                nr         = int(d["N_raw"]),
                ns         = int(d["N_smeared"]),
                ratio      = float(d["ratio"]),
                ratio_stat = float(d["ratio_stat_err"]),
                syst       = float(d["syst_pct"]),
            ))
    return rows


def main():
    ks_rows  = read_table(KS_TABLE)
    lam_rows = read_table(LAM_TABLE)

    # index by (lo, hi) bin edges for matching
    ks_map  = {(r["lo"], r["hi"]): r for r in ks_rows}
    lam_map = {(r["lo"], r["hi"]): r for r in lam_rows}

    common_bins = sorted(set(ks_map) & set(lam_map))
    if not common_bins:
        print("ERROR: no matching pT bins between the two tables.")
        return

    results = []
    hdr = f"{'pT bin':>16}  {'avg_pT':>7}  {'R_lam':>7}  {'R_ks':>7}  {'R_lam/R_ks':>11}  {'stat_err':>9}  {'dev%':>7}"
    print(hdr)
    print("─" * len(hdr))

    for (lo, hi) in common_bins:
        ks  = ks_map[(lo, hi)]
        lam = lam_map[(lo, hi)]

        ror = lam["ratio"] / ks["ratio"]

        # relative stat errors add in quadrature
        rel_err = np.sqrt((lam["ratio_stat"] / lam["ratio"])**2 +
                          (ks["ratio_stat"]  / ks["ratio"])**2)
        ror_err = ror * rel_err

        dev_pct = abs(ror - 1.0) * 100.0

        avg_pt = (ks["avg_pt"] + lam["avg_pt"]) / 2.0

        results.append(dict(
            lo=lo, hi=hi,
            avg_pt=avg_pt,
            ks_avg_pt=ks["avg_pt"],
            lam_avg_pt=lam["avg_pt"],
            ratio_ks=ks["ratio"],
            ratio_lam=lam["ratio"],
            ror=ror,
            ror_err=ror_err,
            dev_pct=dev_pct,
        ))
        print(f"  [{lo:.1f},{hi:.1f})  {avg_pt:>7.3f}  "
              f"{lam['ratio']:>7.4f}  {ks['ratio']:>7.4f}  "
              f"{ror:>11.4f}  {ror_err:>9.4f}  {dev_pct:>7.2f}%")

    # ── text output ────────────────────────────────────────────────────────────
    out_txt = "ratio_of_ratios.txt"
    with open(out_txt, "w") as fh:
        fh.write("# ratio_of_ratios.txt\n")
        fh.write("# R = (N_Lam_smear/N_Lam_raw) / (N_KS_smear/N_KS_raw)\n")
        fh.write("# R=1 → smearing cancels identically; |R-1| = residual syst on Lam/KS ratio\n\n")
        fh.write("bin_lo,bin_hi,avg_pt,ratio_ks,ratio_lam,ror,ror_stat_err,dev_pct\n")
        for r in results:
            fh.write(f"{r['lo']:.2f},{r['hi']:.2f},{r['avg_pt']:.4f},"
                     f"{r['ratio_ks']:.6f},{r['ratio_lam']:.6f},"
                     f"{r['ror']:.6f},{r['ror_err']:.6f},{r['dev_pct']:.4f}\n")

    valid = results
    dev_vals = [r["dev_pct"] for r in valid]
    print(f"\n  R range: {min(r['ror'] for r in valid):.4f} – {max(r['ror'] for r in valid):.4f}")
    print(f"  |R-1| range: {min(dev_vals):.2f}% – {max(dev_vals):.2f}%")
    print(f"  Mean |R-1|:  {np.mean(dev_vals):.2f}%")
    print(f"\nTable → {out_txt}")

    # ── ROOT macro ─────────────────────────────────────────────────────────────
    N     = len(results)
    pts   = ", ".join(f"{r['avg_pt']:.4f}"   for r in results)
    rors  = ", ".join(f"{r['ror']:.6f}"      for r in results)
    errs  = ", ".join(f"{r['ror_err']:.6f}"  for r in results)
    devs  = ", ".join(f"{r['dev_pct']:.4f}"  for r in results)
    xerrs = ", ".join(f"{(r['hi']-r['lo'])/2:.3f}" for r in results)
    r_ks  = ", ".join(f"{r['ratio_ks']:.6f}" for r in results)
    r_lam = ", ".join(f"{r['ratio_lam']:.6f}"for r in results)
    pt_lo = results[0]["lo"]
    pt_hi = results[-1]["hi"]

    macro = f"""// plot_ratio_of_ratios.C
// R = (N_Lam_smear/N_Lam_raw) / (N_KS_smear/N_KS_raw) per pT bin.
// R=1: smearing cancels in the Lambda/K0S ratio.
// |R-1|: residual systematic on the Lambda/K0S yield ratio from smearing.
// Run with:  root -l -b -q plot_ratio_of_ratios.C

#include <ctime>
#include <sstream>
#include <algorithm>
#include <cmath>

std::string getDate2(){{
    std::time_t t=std::time(0); std::tm* now=std::localtime(&t);
    std::stringstream d;
    d<<(now->tm_mon+1)<<'/'<<now->tm_mday<<'/'<<(now->tm_year+1900);
    return d.str();
}}
auto addLabel=[](double x1,double y1,double x2,double y2){{
    TPaveText *pt=new TPaveText(x1,y1,x2,y2,"NDC");
    pt->SetFillStyle(0); pt->SetBorderSize(0); pt->SetTextFont(42);
    pt->AddText("#it{{#bf{{sPHENIX}}}} Internal,  #it{{p}}+#it{{p}}  #sqrt{{s}} = 200 GeV");
    pt->Draw();
}};
auto addDate=[](double x1,double y1,double x2,double y2){{
    TPaveText *pt=new TPaveText(x1,y1,x2,y2,"NDC");
    pt->SetFillStyle(0); pt->SetBorderSize(0); pt->SetTextFont(42);
    pt->AddText(getDate2().c_str()); pt->Draw();
}};

void plot_ratio_of_ratios() {{
    gStyle->SetOptStat(0); gStyle->SetOptTitle(0);

    const int N = {N};
    double avg_pt[]  = {{{pts}}};
    double ror[]     = {{{rors}}};
    double ror_err[] = {{{errs}}};
    double xerr[]    = {{{xerrs}}};
    double dev_pct[] = {{{devs}}};
    double ratio_ks[]  = {{{r_ks}}};
    double ratio_lam[] = {{{r_lam}}};

    // ── canvas 1: ratio of ratios ─────────────────────────────────────────────
    TCanvas *c1 = new TCanvas("c1","Lambda/KS ratio of ratios",900,650);
    c1->SetLeftMargin(0.13); c1->SetBottomMargin(0.13); c1->SetRightMargin(0.06);

    TGraphErrors *gRoR = new TGraphErrors(N,avg_pt,ror,xerr,ror_err);
    gRoR->SetMarkerStyle(20); gRoR->SetMarkerSize(1.4);
    gRoR->SetMarkerColor(kViolet+1); gRoR->SetLineColor(kViolet+1); gRoR->SetLineWidth(2);
    gRoR->Draw("APE");
    gRoR->GetXaxis()->SetTitle("#it{{p}}_{{T}} (GeV/#it{{c}})");
    gRoR->GetYaxis()->SetTitle("(#Lambda smear ratio) / (K_{{S}}^{{0}} smear ratio)");
    gRoR->GetYaxis()->SetRangeUser(0.7, 1.3);
    gRoR->GetXaxis()->SetTitleSize(0.05);
    gRoR->GetYaxis()->SetTitleSize(0.045);
    gRoR->GetYaxis()->SetTitleOffset(1.3);

    TLine *unity = new TLine({pt_lo},1.0,{pt_hi},1.0);
    unity->SetLineStyle(2); unity->SetLineColor(kGray+1); unity->SetLineWidth(2);
    unity->Draw();

    TBox *band = new TBox({pt_lo},0.95,{pt_hi},1.05);
    band->SetFillColorAlpha(kGray,0.30); band->SetLineWidth(0); band->Draw();
    gRoR->Draw("PE SAME");

    addLabel(0.14,0.88,0.65,0.96);
    addDate(0.70,0.88,0.98,0.96);

    TLegend *leg1 = new TLegend(0.14,0.72,0.80,0.87);
    leg1->SetBorderSize(0); leg1->SetFillStyle(0); leg1->SetTextSize(0.030);
    leg1->AddEntry(gRoR, "(N_{{#Lambda}}^{{smear}}/N_{{#Lambda}}^{{raw}}) / (N_{{K_{{S}}^{{0}}}}^{{smear}}/N_{{K_{{S}}^{{0}}}}^{{raw}})  (stat. err)", "lpe");
    leg1->AddEntry(unity, "Unity  (full cancellation)", "l");
    leg1->AddEntry(band,  "#pm5% band", "f");
    leg1->Draw();

    c1->SaveAs("plot_ratio_of_ratios.pdf");
    c1->SaveAs("plot_ratio_of_ratios.png");
    std::cout << "Saved plot_ratio_of_ratios.pdf/.png\\n";

    // ── canvas 2: individual ratios overlaid ──────────────────────────────────
    TCanvas *c2 = new TCanvas("c2","Individual smear ratios",900,650);
    c2->SetLeftMargin(0.13); c2->SetBottomMargin(0.13); c2->SetRightMargin(0.06);

    TGraph *gKS  = new TGraph(N, avg_pt, ratio_ks);
    TGraph *gLam = new TGraph(N, avg_pt, ratio_lam);
    gKS->SetMarkerStyle(20);  gKS->SetMarkerSize(1.4);
    gKS->SetMarkerColor(kBlue+1);  gKS->SetLineColor(kBlue+1);  gKS->SetLineWidth(2);
    gLam->SetMarkerStyle(21); gLam->SetMarkerSize(1.4);
    gLam->SetMarkerColor(kRed+1);  gLam->SetLineColor(kRed+1);  gLam->SetLineWidth(2);

    TMultiGraph *mg = new TMultiGraph();
    mg->Add(gKS,"PL"); mg->Add(gLam,"PL"); mg->Draw("A");
    mg->GetXaxis()->SetTitle("#it{{p}}_{{T}} (GeV/#it{{c}})");
    mg->GetYaxis()->SetTitle("N_{{smeared}} / N_{{raw}}");
    mg->GetYaxis()->SetRangeUser(0.7, 1.3);
    mg->GetXaxis()->SetTitleSize(0.05); mg->GetYaxis()->SetTitleSize(0.05);

    TLine *unity2 = new TLine({pt_lo},1.0,{pt_hi},1.0);
    unity2->SetLineStyle(2); unity2->SetLineColor(kGray+1); unity2->SetLineWidth(2);
    unity2->Draw();
    TBox *band2 = new TBox({pt_lo},0.95,{pt_hi},1.05);
    band2->SetFillColorAlpha(kGray,0.30); band2->SetLineWidth(0); band2->Draw();
    gKS->Draw("PL SAME"); gLam->Draw("PL SAME");

    TLegend *leg2 = new TLegend(0.14,0.74,0.60,0.87);
    leg2->SetBorderSize(0); leg2->SetFillStyle(0); leg2->SetTextSize(0.032);
    leg2->AddEntry(gKS,  "K_{{S}}^{{0}} smear ratio","lp");
    leg2->AddEntry(gLam, "#Lambda^{{0}} smear ratio","lp");
    leg2->Draw();
    addLabel(0.14,0.88,0.65,0.96);
    addDate(0.70,0.88,0.98,0.96);

    c2->SaveAs("plot_smear_ratios_overlay.pdf");
    c2->SaveAs("plot_smear_ratios_overlay.png");
    std::cout << "Saved plot_smear_ratios_overlay.pdf/.png\\n";

    // ── canvas 3: residual systematic on Lambda/K0S ratio ────────────────────
    TCanvas *c3 = new TCanvas("c3","Residual syst on Lam/KS ratio",900,650);
    c3->SetLeftMargin(0.13); c3->SetBottomMargin(0.13); c3->SetRightMargin(0.06);

    TGraph *gDev = new TGraph(N, avg_pt, dev_pct);
    gDev->SetMarkerStyle(20); gDev->SetMarkerSize(1.4);
    gDev->SetMarkerColor(kOrange+1); gDev->SetLineColor(kOrange+1); gDev->SetLineWidth(2);
    gDev->Draw("APL");
    gDev->GetXaxis()->SetTitle("#it{{p}}_{{T}} (GeV/#it{{c}})");
    gDev->GetYaxis()->SetTitle("Residual systematic on #Lambda/K_{{S}}^{{0}} ratio  |R-1| (%)");
    double ymax = *std::max_element(dev_pct,dev_pct+N)*1.4 + 0.3;
    gDev->GetYaxis()->SetRangeUser(0, ymax);
    gDev->GetXaxis()->SetTitleSize(0.05); gDev->GetYaxis()->SetTitleSize(0.042);
    gDev->GetYaxis()->SetTitleOffset(1.4);
    addLabel(0.14,0.88,0.65,0.96);
    addDate(0.70,0.88,0.98,0.96);

    c3->SaveAs("plot_ratio_of_ratios_dev.pdf");
    c3->SaveAs("plot_ratio_of_ratios_dev.png");
    std::cout << "Saved plot_ratio_of_ratios_dev.pdf/.png\\n";
}}
"""
    out_mac = "plot_ratio_of_ratios.C"
    with open(out_mac, "w") as fh:
        fh.write(macro)
    print(f"Macro  → {out_mac}")
    print(f"\nRun:  root -l -b -q {out_mac}")


if __name__ == "__main__":
    main()
