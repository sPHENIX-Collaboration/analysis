#include "./util_combine.h"

bool docombine = true;
std::string comb_fname = "./systematics/combined/combined.root";
std::vector<TGraphAsymmErrors *> v_sphenix_dNdeta;
std::string color_sphenix_combine = "#1A1423";
std::string leg_sphenix_combine = "sPHENIX (Combined)";
int mkstyle_sphenix_combine = 20;
float mksize_sphenix_combine = 1.3;

std::vector<float> v_sphenix_centralitybin = {0, 3, 6, 10, 15, 20, 25, 30, 35, 40, 45, 50};
int nsphenix = v_sphenix_centralitybin.size() - 1;
std::vector<TH1 *> v_sphenix_dNdeta_cmsapproach_hist;
std::vector<TGraphAsymmErrors *> v_SPHENIX_dNdeta_cmsapproach;
std::vector<std::string> v_SPHENIX_labels = {};
std::string color_sphenix_cmsapproach = "#FF6700";
std::string leg_sphenix_cmsapproach = "The closest-match method";
int mkstyle_sphenix_cmsapproach = 34;
float mksize_sphenix_cmsapproach = 1.3;

std::vector<TH1 *> v_sphenix_dNdeta_phobosapproach_hist;
std::vector<TGraphAsymmErrors *> v_SPHENIX_dNdeta_phobosapproach;
std::string color_sphenix_phobosapproach = "#228B22";
std::string leg_sphenix_phobosapproach = "The combinatoric method";
int mkstyle_sphenix_phobosapproach = 47;
float mksize_sphenix_phobosapproach = 1.3;

bool draw_brahms = false;
std::vector<float> v_brahms_centralitybin = {0, 5, 10, 20, 30, 40, 50};
int nbrahms = v_brahms_centralitybin.size() - 1;
std::vector<TGraphAsymmErrors *> v_BRAHMS_dNdeta;
std::vector<std::string> v_BRAHMS_labels = {};
std::string color_brahms = "#7868E6";
std::string leg_brahms = "BRAHMS [Phys. Rev. Lett. 88, 202301 (2002)]";
int mkstyle_brahms = 28;

bool draw_phobos = docombine;
std::vector<float> v_phobos_centralitybin = {0, 3, 6, 10, 15, 20, 25, 30, 35, 40, 45, 50};
int nphobos = v_phobos_centralitybin.size() - 1;
std::vector<TGraphAsymmErrors *> v_PHOBOS_dNdeta;
std::vector<std::string> v_PHOBOS_labels = {};
std::string color_phobos = "#035397";
std::string leg_phobos = "PHOBOS [Phys. Rev. C 83, 024913 (2011)]";
int mkstyle_phobos = 25;
float mksize_phobos = 1.3;

float absxlim = (draw_phobos) ? 2.49 : 1.59;
std::string prelimtext = "Internal";

float LeftMargin = 0.17;
float RightMargin = 0.05;
float TopMargin = 0.12;
float BottomMargin = 0.13;
float ymax = 970;
float ymin = 50;

void BRAHMS_dNdeta()
{
    // load BRAHMS Hepdata
    TFile *f = new TFile("./measurements/HEPData/HEPData-ins567754-v1-root.root", "READ");
    f->cd("Table 1a");
    for (int i = 0; i < nbrahms; i++)
    {
        TGraphAsymmErrors *g = (TGraphAsymmErrors *)gDirectory->Get(Form("Graph1D_y%d", i + 1));
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
        v_PHOBOS_dNdeta.push_back(g);
        v_PHOBOS_labels.push_back(Form("%d-%d%%", (int)v_phobos_centralitybin[i], (int)v_phobos_centralitybin[i + 1]));
    }
    f->Close();
}

TGraphAsymmErrors *TH1toTGraphAsymmErrors(TH1 *h)
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

        if (fabs(h->GetBinCenter(i)) >= 1.1)
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
    TFile *f_comb = new TFile(comb_fname.c_str(), "READ");

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
        TFile *fphobos = new TFile(Form("/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Feb102025/Run6_EvtZFitWidthChange/EvtVtxZ/FinalResult_10cm_Pol2BkgFit/completed/vtxZ_-10_10cm_MBin%d/Final_Mbin%d_00054280/Final_Mbin%d_00054280.root", Mbin, Mbin, Mbin), "READ");
        TH1D *hphobos = (TH1D *)fphobos->Get("h1D_dNdEta_reco");
        hphobos->SetDirectory(0);
        v_sphenix_dNdeta_phobosapproach_hist.push_back(hphobos);
        TGraphAsymmErrors *g_phobos = TH1toTGraphAsymmErrors(hphobos);
        v_SPHENIX_dNdeta_phobosapproach.push_back(g_phobos);
        fphobos->Close();

        // combined measurement
        TFile *f_comb = new TFile(comb_fname.c_str(), "READ");
        TGraphAsymmErrors *g = (TGraphAsymmErrors *)f_comb->Get(Form("tgae_combine_Centrality%dto%d", (int)v_sphenix_centralitybin[i], (int)v_sphenix_centralitybin[i + 1]));
        v_sphenix_dNdeta.push_back(g);
        f_comb->Close();
    }
}

void draw_dNdetaRHIC()
{
    std::string plotdir = "./dNdEtaFinal";
    system(Form("mkdir -p %s", plotdir.c_str()));

    SPHENIX_dNdeta();

    if (draw_phobos)
        PHOBOS_dNdeta();

    if (draw_brahms)
        BRAHMS_dNdeta();

    auto mg_phobos = new TMultiGraph();
    auto mg_brahms = new TMultiGraph();
    auto mg_sphenix_cmsapproach = new TMultiGraph();
    auto mg_sphenix_phobosapproach = new TMultiGraph();
    auto mg_sphenix = new TMultiGraph();

    for (int i = 0; i < nsphenix; i++)
    {
        v_SPHENIX_dNdeta_cmsapproach[i]->SetMarkerStyle(mkstyle_sphenix_cmsapproach);
        v_SPHENIX_dNdeta_cmsapproach[i]->SetMarkerSize(mksize_sphenix_cmsapproach);
        v_SPHENIX_dNdeta_cmsapproach[i]->SetLineColor(TColor::GetColor(color_sphenix_cmsapproach.c_str()));
        v_SPHENIX_dNdeta_cmsapproach[i]->SetLineWidth(1);
        v_SPHENIX_dNdeta_cmsapproach[i]->SetMarkerColor(TColor::GetColor(color_sphenix_cmsapproach.c_str()));
        v_SPHENIX_dNdeta_cmsapproach[i]->SetFillColorAlpha(TColor::GetColor(color_sphenix_cmsapproach.c_str()), 0.2);
        mg_sphenix_cmsapproach->Add(v_SPHENIX_dNdeta_cmsapproach[i], "5 p");

        v_SPHENIX_dNdeta_phobosapproach[i]->SetMarkerStyle(mkstyle_sphenix_phobosapproach);
        v_SPHENIX_dNdeta_phobosapproach[i]->SetMarkerSize(mksize_sphenix_phobosapproach);
        v_SPHENIX_dNdeta_phobosapproach[i]->SetLineColor(TColor::GetColor(color_sphenix_phobosapproach.c_str()));
        v_SPHENIX_dNdeta_phobosapproach[i]->SetLineWidth(1);
        v_SPHENIX_dNdeta_phobosapproach[i]->SetMarkerColor(TColor::GetColor(color_sphenix_phobosapproach.c_str()));
        v_SPHENIX_dNdeta_phobosapproach[i]->SetFillColorAlpha(TColor::GetColor(color_sphenix_phobosapproach.c_str()), 0.2);
        mg_sphenix_phobosapproach->Add(v_SPHENIX_dNdeta_phobosapproach[i], "5 p");

        v_sphenix_dNdeta[i]->SetMarkerStyle(mkstyle_sphenix_combine);
        v_sphenix_dNdeta[i]->SetMarkerSize(mksize_sphenix_combine);
        v_sphenix_dNdeta[i]->SetLineColor(TColor::GetColor(color_sphenix_combine.c_str()));
        v_sphenix_dNdeta[i]->SetLineWidth(1);
        v_sphenix_dNdeta[i]->SetMarkerColor(TColor::GetColor(color_sphenix_combine.c_str()));
        v_sphenix_dNdeta[i]->SetFillColorAlpha(TColor::GetColor(color_sphenix_combine.c_str()), 0.2);
        mg_sphenix->Add(v_sphenix_dNdeta[i], "5 p");
    }

    if (draw_phobos)
    {
        for (int i = 0; i < nphobos; i++)
        {
            v_PHOBOS_dNdeta[i]->SetMarkerStyle(mkstyle_phobos);
            v_PHOBOS_dNdeta[i]->SetMarkerSize(mksize_phobos);
            v_PHOBOS_dNdeta[i]->SetLineColor(TColor::GetColor(color_phobos.c_str()));
            v_PHOBOS_dNdeta[i]->SetLineWidth(0);
            v_PHOBOS_dNdeta[i]->SetMarkerColor(TColor::GetColor(color_phobos.c_str()));
            v_PHOBOS_dNdeta[i]->SetFillColorAlpha(TColor::GetColor(color_phobos.c_str()), 0.15);
            mg_phobos->Add(v_PHOBOS_dNdeta[i], "3 p");
        }
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
    // c->SetLogy();
    c->cd();
    if (draw_phobos) // will always draw combined sPHENIX
    {
        mg_phobos->Draw("A P F");
        mg_sphenix->Draw("5 P F");
    }
    else // not draw phobos, will always draw sPHENIX separately
    {
        mg_sphenix_phobosapproach->Draw("A 5 P F");
        mg_sphenix_cmsapproach->Draw("5 P F");
    }

    if (draw_brahms)
        mg_brahms->Draw("PF");

    // add labels for PHOBOS data
    // for each graph, get the x value of the first point with x < -2 and add a label there
    if (draw_phobos)
    {
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
            // y = v_SPHENIX_dNdeta_phobosapproach[i]->GetPointY(v_SPHENIX_dNdeta_phobosapproach[i]->GetN() - 1) + v_SPHENIX_dNdeta_phobosapproach[i]->GetErrorYhigh(v_SPHENIX_dNdeta_phobosapproach[i]->GetN() - 1);
            y = v_SPHENIX_dNdeta_phobosapproach[i]->GetPointY(v_SPHENIX_dNdeta_phobosapproach[i]->GetN() - 1);
            cout << "phobos: x=" << x << " y=" << y << endl;
            TLatex *lphobos = new TLatex(x + 0.02, y, v_SPHENIX_labels[i].c_str());
            lphobos->SetTextAlign(kHAlignLeft + kVAlignCenter);
            lphobos->SetTextSize(0.02);
            // set color to match the graph
            lphobos->SetTextColor(TColor::GetColor(color_sphenix_phobosapproach.c_str()));
            lphobos->Draw();

            // bin = v_SPHENIX_dNdeta_cmsapproach[i]->FindFirstBinAbove(0);
            x = v_SPHENIX_dNdeta_cmsapproach[i]->GetPointX(0) - v_SPHENIX_dNdeta_cmsapproach[i]->GetErrorXlow(0);
            // y = v_SPHENIX_dNdeta_cmsapproach[i]->GetPointY(0) + v_SPHENIX_dNdeta_cmsapproach[i]->GetErrorYhigh(0);
            y = v_SPHENIX_dNdeta_cmsapproach[i]->GetPointY(0);
            cout << "cms: x=" << x << " y=" << y << endl;
            TLatex *lcms = new TLatex(x - 0.02, y, v_SPHENIX_labels[i].c_str());
            // set right and center adjusted
            lcms->SetTextAlign(kHAlignRight + kVAlignCenter);
            lcms->SetTextSize(0.02);
            // set color to match the graph
            lcms->SetTextColor(TColor::GetColor(color_sphenix_cmsapproach.c_str()));
            lcms->Draw();
        }
    }
    // Change the axis limits
    gPad->Modified();
    if (draw_phobos)
    {
        mg_phobos->GetXaxis()->SetLimits(-absxlim, absxlim);
        mg_phobos->GetXaxis()->SetTitle("Pseudorapidity #eta");
        mg_phobos->GetXaxis()->SetNdivisions(505);
        mg_phobos->GetYaxis()->SetTitle("dN_{ch}/d#eta");
        mg_phobos->GetYaxis()->SetTitleOffset(1.6);
        mg_phobos->GetYaxis()->SetNdivisions(505);
        mg_phobos->SetMaximum(ymax);
        mg_phobos->SetMinimum(ymin);
    }
    else
    {
        mg_sphenix_phobosapproach->GetXaxis()->SetLimits(-absxlim, absxlim);
        mg_sphenix_phobosapproach->GetXaxis()->SetTitle("Pseudorapidity #eta");
        mg_sphenix_phobosapproach->GetXaxis()->SetNdivisions(505);
        mg_sphenix_phobosapproach->GetYaxis()->SetTitle("dN_{ch}/d#eta");
        mg_sphenix_phobosapproach->GetYaxis()->SetTitleOffset(1.6);
        mg_sphenix_phobosapproach->GetYaxis()->SetNdivisions(505);
        mg_sphenix_phobosapproach->SetMaximum(ymax);
        mg_sphenix_phobosapproach->SetMinimum(ymin);
    }
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
    if (draw_phobos)
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
            c->SaveAs(Form("%s/dNdEta_RHIC_combine_wBRAHMS_singlepad.pdf", plotdir.c_str()));
            c->SaveAs(Form("%s/dNdEta_RHIC_combine_wBRAHMS_singlepad.png", plotdir.c_str()));
        }
        else
        {
            c->SaveAs(Form("%s/dNdEta_RHIC_combine_woBRAHMS_singlepad.pdf", plotdir.c_str()));
            c->SaveAs(Form("%s/dNdEta_RHIC_combine_woBRAHMS_singlepad.png", plotdir.c_str()));
        }
    }
    else
    {
        if (draw_brahms)
        {
            c->SaveAs(Form("%s/dNdEta_RHIC_wBRAHMS_singlepad.pdf", plotdir.c_str()));
            c->SaveAs(Form("%s/dNdEta_RHIC_wBRAHMS_singlepad.png", plotdir.c_str()));
        }
        else
        {
            c->SaveAs(Form("%s/dNdEta_RHIC_woBRAHMS_singlepad.pdf", plotdir.c_str()));
            c->SaveAs(Form("%s/dNdEta_RHIC_woBRAHMS_singlepad.png", plotdir.c_str()));
        }
    }
}