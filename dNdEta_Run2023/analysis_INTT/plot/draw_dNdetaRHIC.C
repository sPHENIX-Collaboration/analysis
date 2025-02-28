#include "./util_combine.h"

std::vector<float> v_sphenix_centralitybin = {0, 3, 6, 10, 15, 20, 25, 30, 35, 40, 45, 50};
int nsphenix = v_sphenix_centralitybin.size() - 1;
std::vector<TH1 *> v_sphenix_dNdeta_cmsapproach_hist;
std::vector<TGraphAsymmErrors *> v_SPHENIX_dNdeta_cmsapproach;
std::vector<std::string> v_SPHENIX_labels = {};
std::string color_sphenix_cmsapproach = "#1A1423";
// std::string leg_sphenix_cmsapproach = "#splitline{sPHENIX}{(CMS-inspired approach)}";
std::string leg_sphenix_cmsapproach = "sPHENIX (CMS-inspired approach)";
int mkstyle_sphenix_cmsapproach = 24;

std::vector<TH1 *> v_sphenix_dNdeta_phobosapproach_hist;
std::vector<TGraphAsymmErrors *> v_SPHENIX_dNdeta_phobosapproach;
std::string color_sphenix_phobosapproach = "#618264";
// std::string leg_sphenix_phobosapproach = "#splitline{sPHENIX}{(PHOBOS-inspired approach)}";
std::string leg_sphenix_phobosapproach = "sPHENIX (PHOBOS-inspired approach)";
int mkstyle_sphenix_phobosapproach = 24;

bool draw_brahms = false;
std::vector<float> v_brahms_centralitybin = {0, 5, 10, 20, 30, 40, 50};
int nbrahms = v_brahms_centralitybin.size() - 1;
std::vector<TGraphAsymmErrors *> v_BRAHMS_dNdeta;
std::vector<std::string> v_BRAHMS_labels = {};
std::string color_brahms = "#7868E6";
// std::string leg_brahms = "#splitline{BRAHMS}{[Phys. Rev. Lett. 88, 202301 (2002)]}";
std::string leg_brahms = "BRAHMS [Phys. Rev. Lett. 88, 202301 (2002)]";
int mkstyle_brahms = 28;

std::vector<float> v_phobos_centralitybin = {0, 3, 6, 10, 15, 20, 25, 30, 35, 40, 45, 50};
int nphobos = v_phobos_centralitybin.size() - 1;
std::vector<TGraphAsymmErrors *> v_PHOBOS_dNdeta;
std::vector<std::string> v_PHOBOS_labels = {};
std::string color_phobos = "#035397";
// std::string leg_phobos = "#splitline{PHOBOS}{[Phys. Rev. C 83, 024913 (2011)]}";
std::string leg_phobos = "PHOBOS [Phys. Rev. C 83, 024913 (2011)]";
int mkstyle_phobos = 25;

bool docombine = false;
std::vector<TGraphAsymmErrors *> v_sphenix_dNdeta;
std::string color_sphenix_combine = "#1A1423";
std::string leg_sphenix_combine = "sPHENIX (Combined)";
int mkstyle_sphenix_combine = 24;

float absxlim = 2.49;
std::string prelimtext = "Internal";

float LeftMargin = 0.17;
float RightMargin = 0.05;
float TopMargin = (docombine) ? ((draw_brahms) ? 0.18 : 0.12) : ((draw_brahms) ? 0.24 : 0.18);
float BottomMargin = 0.13;
float ymax = (docombine) ? 940 : 970;
float ymin = 50;

void BRAHMS_dNdeta()
{
    // load BRAHMS Hepdata
    TFile *f = new TFile("./measurements/HEPData/HEPData-ins567754-v1-root.root", "READ");
    f->cd("Table 1a");
    for (int i = 0; i < nbrahms; i++)
    {
        TGraphAsymmErrors *g = (TGraphAsymmErrors *)gDirectory->Get(Form("Graph1D_y%d", i + 1));
        // exclude points with |eta| >= 1.35
        // for (int j = 0; j < g->GetN(); j++)
        // {
        //     double x, y;
        //     g->GetPoint(j, x, y);
        //     if (fabs(x) >= 1.35)
        //     {
        //         g->RemovePoint(j);
        //         j--;
        //     }
        // }
        v_BRAHMS_dNdeta.push_back(g);
        v_BRAHMS_labels.push_back(Form("%d-%d%%", (int)v_brahms_centralitybin[i], (int)v_brahms_centralitybin[i + 1]));
    }
    f->Close();
}

void PHOBOS_dNdeta()
{
    // load PHOBOS measurements
    TFile *f = new TFile("./measurements/PHOBOS-PhysRevC.83.024913/auau_200GeV.root", "READ");
    f->cd();
    for (int i = 0; i < nphobos; i++)
    {
        TGraphAsymmErrors *g = (TGraphAsymmErrors *)gDirectory->Get(Form("AuAu_200GeV_Centrality_%dto%d", (int)v_phobos_centralitybin[i], (int)v_phobos_centralitybin[i + 1]));
        // exclude points with |eta| >= 1.35
        // for (int j = 0; j < g->GetN(); j++)
        // {
        //     double x, y;
        //     g->GetPoint(j, x, y);
        //     if (fabs(x) >= 1.35)
        //     {
        //         g->RemovePoint(j);
        //         j--;
        //     }
        // }
        // artificially set the x-error to 0.5 of the difference between the x values of two adjacent points
        // for (int j = 0; j < g->GetN() - 1; j++)
        // {
        //     double x1, y1, x2, y2;
        //     g->GetPoint(j, x1, y1);
        //     g->GetPoint(j + 1, x2, y2);
        //     double xerr = 0.2 * (x2 - x1);
        //     g->SetPointEXhigh(j, xerr);
        //     g->SetPointEXlow(j, xerr);
        // }
        v_PHOBOS_dNdeta.push_back(g);
        v_PHOBOS_labels.push_back(Form("%d-%d%%", (int)v_phobos_centralitybin[i], (int)v_phobos_centralitybin[i + 1]));
    }
    f->Close();
}

TGraphAsymmErrors* TH1toTGraphAsymmErrors(TH1 *h)
{
    int Nbins = h->GetNbinsX();

    std::vector<float> vx, vy, vxerr, vyerr;
    vx.clear();
    vy.clear();
    vxerr.clear();
    vyerr.clear();
    for (int i = 1; i <= Nbins; i++)
    {
        if (h->GetBinContent(i) <= 0)
            continue;

        vx.push_back(h->GetBinCenter(i));
        vy.push_back(h->GetBinContent(i));
        vxerr.push_back(h->GetBinWidth(i) * 0.49);
        vyerr.push_back(h->GetBinError(i));
    }

    TGraphAsymmErrors *g = new TGraphAsymmErrors(vx.size(), vx.data(), vy.data(), vxerr.data(), vxerr.data(), vyerr.data(), vyerr.data());
    return g;
}

void SPHENIX_dNdeta()
{
    for (int i = 0; i < nsphenix; i++)
    {
        int Mbin = GetMbinNum(Form("Centrality%dto%d", (int)v_sphenix_centralitybin[i], (int)v_sphenix_centralitybin[i + 1]));

        // CMS-inspired approach
        TFile *f = new TFile(Form("./systematics/Centrality%dto%d_Zvtxm10p0to10p0_noasel/finalhists_systematics_Centrality%dto%d_Zvtxm10p0to10p0_noasel.root", (int)v_sphenix_centralitybin[i], (int)v_sphenix_centralitybin[i + 1], (int)v_sphenix_centralitybin[i], (int)v_sphenix_centralitybin[i + 1]), "READ");
        TH1D *h = (TH1D *)f->Get("hM_final");
        h->SetDirectory(0);
        v_sphenix_dNdeta_cmsapproach_hist.push_back(h);
        TGraphAsymmErrors *g_cms = TH1toTGraphAsymmErrors(h);
        v_SPHENIX_dNdeta_cmsapproach.push_back(g_cms);
        v_SPHENIX_labels.push_back(Form("%d-%d%%", (int)v_sphenix_centralitybin[i], (int)v_sphenix_centralitybin[i + 1]));
        f->Close();

        // PHOBOS-inspired approach
        TFile *fphobos = new TFile(Form("/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Feb102025/Run5/EvtVtxZ/FinalResult/completed/vtxZ_-10_10cm_MBin%d/Final_Mbin%d_00054280/Final_Mbin%d_00054280.root", Mbin, Mbin, Mbin), "READ");
        TH1D *hphobos = (TH1D *)fphobos->Get("h1D_dNdEta_reco");
        hphobos->SetDirectory(0);
        v_sphenix_dNdeta_phobosapproach_hist.push_back(hphobos);
        TGraphAsymmErrors *g_phobos = TH1toTGraphAsymmErrors(hphobos);
        v_SPHENIX_dNdeta_phobosapproach.push_back(g_phobos);
        fphobos->Close();
    }
}

// a function to combine sPHENIX CMS-inspired and PHOBOS-inspired dN/deta; return a TGraphAsymmErrors object
TGraphAsymmErrors *sphnex_dNdeta(TH1 *h_phobos, TH1 *h_cms)
{
    int Nbins = h_phobos->GetNbinsX();

    std::vector<float> vx, vy, vxerr, vyerr;
    vx.clear();
    vy.clear();
    vxerr.clear();
    vyerr.clear();
    for (int i = 1; i <= Nbins; i++)
    {
        double x = h_phobos->GetBinCenter(i);
        double y_phobos = h_phobos->GetBinContent(i);
        double y_cms = h_cms->GetBinContent(i);
        double y = (y_phobos + y_cms) / 2;
        double yerr_phobos = h_phobos->GetBinError(i);
        double yerr_cms = h_cms->GetBinError(i);

        if (y_phobos == 0 || y_cms == 0)
            continue;

        double yerr = (yerr_phobos > yerr_cms) ? yerr_phobos : yerr_cms;
        double xerr = h_phobos->GetBinWidth(i) * 0.49;

        // cout << "Measurement: " << i << " " << x << " " << y << " " << xerr << " " << yerr << endl;

        vx.push_back(x);
        vy.push_back(y);
        vxerr.push_back(xerr);
        vyerr.push_back(yerr);
    }

    TGraphAsymmErrors *g = new TGraphAsymmErrors(vx.size(), vx.data(), vy.data(), vxerr.data(), vxerr.data(), vyerr.data(), vyerr.data());
    return g;
}

void draw_dNdetaRHIC()
{
    std::string plotdir = "./dNdEtaFinal";
    system(Form("mkdir -p %s", plotdir.c_str()));

    SPHENIX_dNdeta();
    PHOBOS_dNdeta();
    if (draw_brahms)
        BRAHMS_dNdeta();
    // auto ts_sphenix_cmsapproach = new THStack("ts_sphenix_cmsapproach", "sPHENIX AuAu 200 GeV (CMS-inspired approach)");
    // auto ts_sphenix_phobosapproach = new THStack("ts_sphenix_phobosapproach", "sPHENIX AuAu 200 GeV (PHOBOS-inspired approach)");
    auto mg_phobos = new TMultiGraph();
    auto mg_brahms = new TMultiGraph();
    auto mg_sphenix_cmsapproach = new TMultiGraph();
    auto mg_sphenix_phobosapproach = new TMultiGraph();
    auto mg_sphenix = new TMultiGraph();

    for (int i = 0; i < nsphenix; i++)
    {
        v_SPHENIX_dNdeta_cmsapproach[i]->SetMarkerStyle(mkstyle_sphenix_cmsapproach);
        v_SPHENIX_dNdeta_cmsapproach[i]->SetMarkerSize(0.8);
        v_SPHENIX_dNdeta_cmsapproach[i]->SetLineColor(TColor::GetColor(color_sphenix_cmsapproach.c_str()));
        v_SPHENIX_dNdeta_cmsapproach[i]->SetLineWidth(1);
        v_SPHENIX_dNdeta_cmsapproach[i]->SetMarkerColor(TColor::GetColor(color_sphenix_cmsapproach.c_str()));
        v_SPHENIX_dNdeta_cmsapproach[i]->SetFillColorAlpha(TColor::GetColor(color_sphenix_cmsapproach.c_str()), 0.35);
        mg_sphenix_cmsapproach->Add(v_SPHENIX_dNdeta_cmsapproach[i], "5 p");

        v_SPHENIX_dNdeta_phobosapproach[i]->SetMarkerStyle(mkstyle_sphenix_phobosapproach);
        v_SPHENIX_dNdeta_phobosapproach[i]->SetMarkerSize(0.8);
        v_SPHENIX_dNdeta_phobosapproach[i]->SetLineColor(TColor::GetColor(color_sphenix_phobosapproach.c_str()));
        v_SPHENIX_dNdeta_phobosapproach[i]->SetLineWidth(1);
        v_SPHENIX_dNdeta_phobosapproach[i]->SetMarkerColor(TColor::GetColor(color_sphenix_phobosapproach.c_str()));
        v_SPHENIX_dNdeta_phobosapproach[i]->SetFillColorAlpha(TColor::GetColor(color_sphenix_phobosapproach.c_str()), 0.7);
        mg_sphenix_phobosapproach->Add(v_SPHENIX_dNdeta_phobosapproach[i], "5 p");

        TGraphAsymmErrors *g = sphnex_dNdeta(v_sphenix_dNdeta_phobosapproach_hist[i], v_sphenix_dNdeta_cmsapproach_hist[i]);
        g->SetMarkerStyle(mkstyle_sphenix_combine);
        g->SetMarkerSize(0.8);
        g->SetLineColor(TColor::GetColor(color_sphenix_combine.c_str()));
        g->SetLineWidth(1);
        g->SetMarkerColor(TColor::GetColor(color_sphenix_combine.c_str()));
        g->SetFillColorAlpha(TColor::GetColor(color_sphenix_combine.c_str()), 0.3);
        v_sphenix_dNdeta.push_back(g);
        mg_sphenix->Add(g, "5 p");
    }

    for (int i = 0; i < nphobos; i++)
    {
        v_PHOBOS_dNdeta[i]->SetMarkerStyle(mkstyle_phobos);
        v_PHOBOS_dNdeta[i]->SetMarkerSize(0.8);
        v_PHOBOS_dNdeta[i]->SetLineColor(TColor::GetColor(color_phobos.c_str()));
        v_PHOBOS_dNdeta[i]->SetLineWidth(0);
        v_PHOBOS_dNdeta[i]->SetMarkerColor(TColor::GetColor(color_phobos.c_str()));
        v_PHOBOS_dNdeta[i]->SetFillColorAlpha(TColor::GetColor(color_phobos.c_str()), 0.15);
        mg_phobos->Add(v_PHOBOS_dNdeta[i], "3 p");
    }

    if (draw_brahms)
    {
        for (int i = 0; i < nbrahms; i++)
        {
            v_BRAHMS_dNdeta[i]->SetMarkerStyle(mkstyle_brahms);
            v_BRAHMS_dNdeta[i]->SetMarkerSize(0.8);
            v_BRAHMS_dNdeta[i]->SetLineWidth(0);
            v_BRAHMS_dNdeta[i]->SetMarkerColor(TColor::GetColor(color_brahms.c_str()));
            v_BRAHMS_dNdeta[i]->SetFillColorAlpha(TColor::GetColor(color_brahms.c_str()), 0.2);
            mg_brahms->Add(v_BRAHMS_dNdeta[i], "3 p");
        }
    }

    TCanvas *c = new TCanvas("c", "c", 800, 1200);
    gPad->SetTopMargin(TopMargin);
    gPad->SetLeftMargin(LeftMargin);
    gPad->SetRightMargin(RightMargin);
    c->cd();
    mg_phobos->Draw("A PF");
    mg_brahms->Draw("PF");
    if (docombine)
        mg_sphenix->Draw("5 P F");
    else
    {
        mg_sphenix_phobosapproach->Draw("5 P F");
        mg_sphenix_cmsapproach->Draw("5 P F");
    }
    // add labels for PHOBOS data
    // for each graph, get the x value of the first point with x < -2 and add a label there
    for (int i = 0; i < nphobos; i++)
    {
        double x, y;
        for (int j = 0; j < v_PHOBOS_dNdeta[i]->GetN(); j++)
        {
            v_PHOBOS_dNdeta[i]->GetPoint(j, x, y);
            if (x > -2.1)
                break;
        }
        TLatex *l = new TLatex(x, y + 5, v_PHOBOS_labels[i].c_str());
        // set right and center adjusted
        l->SetTextAlign(kHAlignRight + kVAlignBottom);
        l->SetTextSize(0.02);
        // set color to match the graph
        l->SetTextColor(TColor::GetColor(color_phobos.c_str()));
        l->Draw();
    }

    if (draw_brahms)
    {
        // add labels for BRAHMS data
        // for each graph, get the x value of the leftmost point and add a label there
        for (int i = 0; i < nbrahms; i++)
        {
            double x, y;
            for (int j = 0; j < v_BRAHMS_dNdeta[i]->GetN(); j++)
            {
                v_BRAHMS_dNdeta[i]->GetPoint(j, x, y);
                if (x > 1.8)
                    break;
            }
            TLatex *l = new TLatex(x, y + 3, v_BRAHMS_labels[i].c_str());
            // set right and center adjusted
            l->SetTextAlign(kHAlignLeft + kVAlignBottom);
            l->SetTextSize(0.02);
            // set color to match the graph
            l->SetTextColor(TColor::GetColor(color_brahms.c_str()));
            l->Draw();
        }
    }

    // add labels for sPHENIX data
    // for each graph, get the binx of the rightmost bin and bin content and add a label there
    if (docombine)
    {
        double x, y;
        for (int i = 0; i < nsphenix; i++)
        {
            double x, y;
            x = v_sphenix_dNdeta[i]->GetPointX(v_sphenix_dNdeta[i]->GetN() - 1) + v_sphenix_dNdeta[i]->GetErrorXhigh(v_sphenix_dNdeta[i]->GetN() - 1);
            y = v_sphenix_dNdeta[i]->GetPointY(v_sphenix_dNdeta[i]->GetN() - 1) + v_sphenix_dNdeta[i]->GetErrorYhigh(v_sphenix_dNdeta[i]->GetN() - 1);
            // print out the first point for debugging
            cout << v_sphenix_dNdeta[i]->GetPointX(0) << " " << v_sphenix_dNdeta[i]->GetPointY(0) << " " << v_sphenix_dNdeta[i]->GetErrorYlow(0) << " " << v_sphenix_dNdeta[i]->GetErrorYhigh(0) << endl;
            TLatex *l = new TLatex(x, y, v_SPHENIX_labels[i].c_str());
            l->SetTextAlign(kHAlignLeft + kVAlignBottom);
            l->SetTextSize(0.02);
            // set color to match the graph
            l->SetTextColor(TColor::GetColor(color_sphenix_combine.c_str()));
            l->Draw();
        }
    }
    else
    {
        for (int i = 0; i < nsphenix; i++)
        {
            double x, y;
            // int bin = v_SPHENIX_dNdeta_phobosapproach[i]->FindLastBinAbove(0);
            x = v_SPHENIX_dNdeta_phobosapproach[i]->GetPointX(v_SPHENIX_dNdeta_phobosapproach[i]->GetN() - 1) + v_SPHENIX_dNdeta_phobosapproach[i]->GetErrorXlow(v_SPHENIX_dNdeta_phobosapproach[i]->GetN() - 1);
            y = v_SPHENIX_dNdeta_phobosapproach[i]->GetPointY(v_SPHENIX_dNdeta_phobosapproach[i]->GetN() - 1) + v_SPHENIX_dNdeta_phobosapproach[i]->GetErrorYhigh(v_SPHENIX_dNdeta_phobosapproach[i]->GetN() - 1);
            cout << "phobos: x=" << x << " y=" << y << endl;
            TLatex *lphobos = new TLatex(x, y, v_SPHENIX_labels[i].c_str());
            if (v_sphenix_centralitybin[i] >= 20)
                lphobos->SetTextAlign(kHAlignLeft + kVAlignBottom);
            else
                lphobos->SetTextAlign(kHAlignLeft + kVAlignTop);
            lphobos->SetTextSize(0.02);
            // set color to match the graph
            lphobos->SetTextColor(TColor::GetColor(color_sphenix_phobosapproach.c_str()));
            lphobos->Draw();

            // bin = v_SPHENIX_dNdeta_cmsapproach[i]->FindFirstBinAbove(0);
            x = v_SPHENIX_dNdeta_cmsapproach[i]->GetPointX(0) - v_SPHENIX_dNdeta_cmsapproach[i]->GetErrorXlow(0);
            y = v_SPHENIX_dNdeta_cmsapproach[i]->GetPointY(0) + v_SPHENIX_dNdeta_cmsapproach[i]->GetErrorYhigh(0);
            cout << "cms: x=" << x << " y=" << y << endl;
            TLatex *lcms = new TLatex(x, y, v_SPHENIX_labels[i].c_str());
            // set right and center adjusted
            if (v_sphenix_centralitybin[i] >= 20)
                lcms->SetTextAlign(kHAlignRight + kVAlignBottom);
            else
                lcms->SetTextAlign(kHAlignRight + kVAlignTop);
            lcms->SetTextSize(0.02);
            // set color to match the graph
            lcms->SetTextColor(TColor::GetColor(color_sphenix_cmsapproach.c_str()));
            lcms->Draw();
        }
    }
    // Change the axis limits
    gPad->Modified();
    mg_phobos->GetXaxis()->SetLimits(-absxlim, absxlim);
    mg_phobos->GetXaxis()->SetTitle("Pseudorapidity #eta");
    mg_phobos->GetXaxis()->SetNdivisions(505);
    mg_phobos->GetYaxis()->SetTitle("dN_{ch}/d#eta");
    mg_phobos->GetYaxis()->SetTitleOffset(1.6);
    mg_phobos->GetYaxis()->SetNdivisions(505);
    mg_phobos->SetMaximum(ymax);
    mg_phobos->SetMinimum(ymin);
    c->RedrawAxis();
    // Legend
    TLegend *leg = new TLegend(gPad->GetLeftMargin(), 1 - gPad->GetTopMargin() + 0.02, 1 - gPad->GetRightMargin(), 0.98);
    leg->SetTextAlign(kHAlignLeft + kVAlignCenter);
    if (docombine)
        leg->AddEntry(v_sphenix_dNdeta[0], leg_sphenix_combine.c_str(), "pf");
    else
    {
        leg->AddEntry(v_SPHENIX_dNdeta_phobosapproach[0], leg_sphenix_phobosapproach.c_str(), "pf");
        leg->AddEntry(v_SPHENIX_dNdeta_cmsapproach[0], leg_sphenix_cmsapproach.c_str(), "pf");
    }
    leg->AddEntry(v_PHOBOS_dNdeta[0], leg_phobos.c_str(), "pf");
    if (draw_brahms)
        leg->AddEntry(v_BRAHMS_dNdeta[0], leg_brahms.c_str(), "pf");
    leg->SetTextSize(0.03);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->Draw();
    TLegend *sphnxleg = new TLegend(gPad->GetLeftMargin() + 0.05, (1 - TopMargin) - 0.11, gPad->GetLeftMargin() + 0.2, (1 - TopMargin) - 0.03);
    sphnxleg->SetTextAlign(kHAlignLeft + kVAlignCenter);
    sphnxleg->SetTextSize(0.04);
    sphnxleg->SetFillStyle(0);
    sphnxleg->AddEntry("", Form("#it{#bf{sPHENIX}} %s", prelimtext.c_str()), "");
    sphnxleg->AddEntry("", "Au+Au #sqrt{s_{NN}}=200 GeV", "");
    sphnxleg->Draw();
    if (docombine)
    {
        if (draw_brahms)
        {
            c->SaveAs(Form("%s/dNdEta_RHIC_combine_wBRAHMS.pdf", plotdir.c_str()));
            c->SaveAs(Form("%s/dNdEta_RHIC_combine_wBRAHMS.png", plotdir.c_str()));
        }
        else
        {
            c->SaveAs(Form("%s/dNdEta_RHIC_combine_woBRAHMS.pdf", plotdir.c_str()));
            c->SaveAs(Form("%s/dNdEta_RHIC_combine_woBRAHMS.png", plotdir.c_str()));
        }
    }
    else
    {
        if (draw_brahms)
        {
            c->SaveAs(Form("%s/dNdEta_RHIC_wBRAHMS.pdf", plotdir.c_str()));
            c->SaveAs(Form("%s/dNdEta_RHIC_wBRAHMS.png", plotdir.c_str()));
        }
        else
        {
            c->SaveAs(Form("%s/dNdEta_RHIC_woBRAHMS.pdf", plotdir.c_str()));
            c->SaveAs(Form("%s/dNdEta_RHIC_woBRAHMS.png", plotdir.c_str()));
        }
    }
}