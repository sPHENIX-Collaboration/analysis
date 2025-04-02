#include "./util_combine.h"

bool verbose = true;

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
std::string leg_brahms = "#splitline{BRAHMS}{[Phys. Rev. Lett. 88, 202301 (2002)]}";
int mkstyle_brahms = 28;

bool draw_phobos = docombine;
std::vector<float> v_phobos_centralitybin = {0, 3, 6, 10, 15, 20, 25, 30, 35, 40, 45, 50};
int nphobos = v_phobos_centralitybin.size() - 1;
std::vector<TGraphAsymmErrors *> v_PHOBOS_dNdeta;
std::vector<std::string> v_PHOBOS_labels = {};
std::string color_phobos = "#035397";
std::string leg_phobos = "#splitline{PHOBOS}{[Phys. Rev. C 83, 024913 (2011)]}";
int mkstyle_phobos = 25;
float mksize_phobos = 1.3;

float absxlim = (draw_phobos) ? 1.99 : 1.59;
std::string prelimtext = "Internal";

float LeftMargin = 0.17;
float RightMargin = 0.03;
float TopMargin = 0.08;
float BottomMargin = 0.15;
float ymax_pad1 = (!docombine) ? 1080 : 1030;
float ymin_pad1 = (!docombine) ? 610 : 560;
float ymax_pad2 = (!docombine) ? 760 : 760;
float ymin_pad2 = (!docombine) ? 540 : 510;
float ymax_pad3 = (!docombine) ? 640 : 640;
float ymin_pad3 = (!docombine) ? 480 : 460;
float ymax_pad4 = (!docombine) ? 540 : 540;
float ymin_pad4 = (!docombine) ? 401 : 385;
float ymax_pad5 = (!docombine) ? 449 : 449;
float ymin_pad5 = (!docombine) ? 55 : 55;
float axis_title_size = 0.05;
float axis_label_size = 0.05;
float leg_textsize = 0.04;
float centlabel_textsize = 0.025;
float xticklength = 0.06;
float yticklength = 0.03;
float pad1_legtextscale = (!docombine) ? 3 : 2.7;
float pad1_labeltextscale = 2.7;
float pad1_tickscale = 1.7;
float pad2_labeltextscale = 5.2;
float pad2_tickscale = 1.9;
float pad3_labeltextscale = pad2_labeltextscale;
float pad3_tickscale = pad2_tickscale;
float pad4_labeltextscale = pad2_labeltextscale;
float pad4_tickscale = pad2_tickscale;
float pad5_labeltextscale = 1.05;
float pad5_tickscale = 1.8;

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
        TFile *fphobos = new TFile(Form("/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_HIJING_MDC2_ana472_20250307/Run7/EvtVtxZ/FinalResult_10cm_Pol2BkgFit_DeltaPhi0p026/completed/vtxZ_-10_10cm_MBin%d/Final_Mbin%d_00054280/Final_Mbin%d_00054280.root", Mbin, Mbin, Mbin), "READ");
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

void draw_dNdetaRHIC_v2()
{
    std::string plotdir = "./dNdEtaFinal";
    system(Form("mkdir -p %s", plotdir.c_str()));

    SPHENIX_dNdeta();

    if (draw_phobos)
        PHOBOS_dNdeta();

    for (int i = 0; i < nsphenix; i++)
    {
        v_SPHENIX_dNdeta_cmsapproach[i]->SetMarkerStyle(mkstyle_sphenix_cmsapproach);
        v_SPHENIX_dNdeta_cmsapproach[i]->SetMarkerSize(mksize_sphenix_cmsapproach);
        v_SPHENIX_dNdeta_cmsapproach[i]->SetLineColor(TColor::GetColor(color_sphenix_cmsapproach.c_str()));
        v_SPHENIX_dNdeta_cmsapproach[i]->SetLineWidth(1);
        v_SPHENIX_dNdeta_cmsapproach[i]->SetMarkerColor(TColor::GetColor(color_sphenix_cmsapproach.c_str()));
        v_SPHENIX_dNdeta_cmsapproach[i]->SetFillColorAlpha(TColor::GetColor(color_sphenix_cmsapproach.c_str()), 0.2);

        v_SPHENIX_dNdeta_phobosapproach[i]->SetMarkerStyle(mkstyle_sphenix_phobosapproach);
        v_SPHENIX_dNdeta_phobosapproach[i]->SetMarkerSize(mksize_sphenix_phobosapproach);
        v_SPHENIX_dNdeta_phobosapproach[i]->SetLineColor(TColor::GetColor(color_sphenix_phobosapproach.c_str()));
        v_SPHENIX_dNdeta_phobosapproach[i]->SetLineWidth(1);
        v_SPHENIX_dNdeta_phobosapproach[i]->SetMarkerColor(TColor::GetColor(color_sphenix_phobosapproach.c_str()));
        v_SPHENIX_dNdeta_phobosapproach[i]->SetFillColorAlpha(TColor::GetColor(color_sphenix_phobosapproach.c_str()), 0.2);

        v_sphenix_dNdeta[i]->SetMarkerStyle(mkstyle_sphenix_combine);
        v_sphenix_dNdeta[i]->SetMarkerSize(mksize_sphenix_combine);
        v_sphenix_dNdeta[i]->SetLineColor(TColor::GetColor(color_sphenix_combine.c_str()));
        v_sphenix_dNdeta[i]->SetLineWidth(1);
        v_sphenix_dNdeta[i]->SetMarkerColor(TColor::GetColor(color_sphenix_combine.c_str()));
        v_sphenix_dNdeta[i]->SetFillColorAlpha(TColor::GetColor(color_sphenix_combine.c_str()), 0.2);

        // print the points and errors for debugging
        if (verbose)
        {
            cout << "Centrality: " << v_SPHENIX_labels[i] << endl;
            for (int j = 0; j < v_SPHENIX_dNdeta_cmsapproach[i]->GetN(); j++)
            {
                double x, y, xerrl, xerru, yerrl, yerru;
                v_SPHENIX_dNdeta_cmsapproach[i]->GetPoint(j, x, y);
                xerrl = v_SPHENIX_dNdeta_cmsapproach[i]->GetErrorXlow(j);
                xerru = v_SPHENIX_dNdeta_cmsapproach[i]->GetErrorXhigh(j);
                yerrl = v_SPHENIX_dNdeta_cmsapproach[i]->GetErrorYlow(j);
                yerru = v_SPHENIX_dNdeta_cmsapproach[i]->GetErrorYhigh(j);
                cout << "CMS approach: " << x << " " << y << " " << xerrl << " " << xerru << " " << yerrl << " " << yerru << endl;
            }
            for (int j = 0; j < v_SPHENIX_dNdeta_phobosapproach[i]->GetN(); j++)
            {
                double x, y, xerrl, xerru, yerrl, yerru;
                v_SPHENIX_dNdeta_phobosapproach[i]->GetPoint(j, x, y);
                xerrl = v_SPHENIX_dNdeta_phobosapproach[i]->GetErrorXlow(j);
                xerru = v_SPHENIX_dNdeta_phobosapproach[i]->GetErrorXhigh(j);
                yerrl = v_SPHENIX_dNdeta_phobosapproach[i]->GetErrorYlow(j);
                yerru = v_SPHENIX_dNdeta_phobosapproach[i]->GetErrorYhigh(j);
                cout << "PHOBOS approach: " << x << " " << y << " " << xerrl << " " << xerru << " " << yerrl << " " << yerru << endl;
            }
        }
    }

    if (draw_phobos)
    {
        for (int i = 0; i < nphobos; i++)
        {
            v_PHOBOS_dNdeta[i]->SetMarkerStyle(mkstyle_phobos);
            v_PHOBOS_dNdeta[i]->SetMarkerSize(mksize_phobos);
            v_PHOBOS_dNdeta[i]->SetLineColor(TColor::GetColor(color_phobos.c_str()));
            v_PHOBOS_dNdeta[i]->SetLineWidth(1);
            v_PHOBOS_dNdeta[i]->SetMarkerColor(TColor::GetColor(color_phobos.c_str()));
            v_PHOBOS_dNdeta[i]->SetFillColorAlpha(TColor::GetColor(color_phobos.c_str()), 0.15);
            // mg_phobos->Add(v_PHOBOS_dNdeta[i], "3 p");
        }
    }

    TCanvas *c = new TCanvas("c", "c", 800, 1200);
    TPad *pad1 = new TPad("pad1", "pad1", 0, 0.8, 1, 1);
    TPad *pad2 = new TPad("pad2", "pad2", 0, 0.7, 1, 0.8);
    TPad *pad3 = new TPad("pad3", "pad3", 0, 0.6, 1, 0.7);
    TPad *pad4 = new TPad("pad4", "pad4", 0, 0.5, 1, 0.6);
    TPad *pad5 = new TPad("pad5", "pad5", 0, 0.0, 1, 0.5);
    pad1->SetTopMargin(TopMargin);
    pad1->SetBottomMargin(0);
    pad1->SetLeftMargin(LeftMargin);
    pad1->SetRightMargin(RightMargin);
    pad2->SetTopMargin(0);
    pad2->SetBottomMargin(0);
    pad2->SetLeftMargin(LeftMargin);
    pad2->SetRightMargin(RightMargin);
    pad3->SetTopMargin(0);
    pad3->SetBottomMargin(0);
    pad3->SetLeftMargin(LeftMargin);
    pad3->SetRightMargin(RightMargin);
    pad4->SetTopMargin(0);
    pad4->SetBottomMargin(0);
    pad4->SetLeftMargin(LeftMargin);
    pad4->SetRightMargin(RightMargin);
    pad5->SetTopMargin(0);
    pad5->SetBottomMargin(BottomMargin);
    pad5->SetLeftMargin(LeftMargin);
    pad5->SetRightMargin(RightMargin);
    pad1->Draw();
    pad2->Draw();
    pad3->Draw();
    pad4->Draw();
    pad5->Draw();
    pad1->cd();
    // Legend
    TLegend *leg = new TLegend(pad1->GetLeftMargin() + 0.02,                                                     //
                               (!docombine) ? 1 - pad1->GetTopMargin() - 0.38 : 1 - pad1->GetTopMargin() - 0.43, //
                               (!docombine) ? pad1->GetLeftMargin() + 0.35 : pad1->GetLeftMargin() + 0.25,       //
                               (!docombine) ? 1 - pad1->GetTopMargin() - 0.10 : 1 - pad1->GetTopMargin() - 0.05  //
    );
    leg->SetLineStyle(1);
    leg->SetLineWidth(1);
    leg->SetLineColor(1);
    leg->SetTextAlign(kHAlignLeft + kVAlignCenter);
    leg->SetTextSize((leg_textsize - 0.01) * pad1_legtextscale);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    if (draw_phobos) // will always draw combined sPHENIX
    {
        v_PHOBOS_dNdeta[0]->SetMaximum(ymax_pad1);
        v_PHOBOS_dNdeta[0]->SetMinimum(ymin_pad1);
        v_PHOBOS_dNdeta[0]->GetYaxis()->SetNdivisions(4, 2, 0);
        v_PHOBOS_dNdeta[0]->GetXaxis()->SetLimits(-absxlim, absxlim);
        v_PHOBOS_dNdeta[0]->GetXaxis()->SetNdivisions(505);

        v_PHOBOS_dNdeta[0]->GetYaxis()->SetTitle("dN_{ch}/d#eta");
        v_PHOBOS_dNdeta[0]->GetYaxis()->CenterTitle(true);
        v_PHOBOS_dNdeta[0]->GetYaxis()->SetTitleOffset(0.55);
        v_PHOBOS_dNdeta[0]->GetYaxis()->SetTitleSize(axis_title_size * 3);
        v_PHOBOS_dNdeta[0]->GetYaxis()->SetLabelSize(axis_label_size * pad1_labeltextscale);
        v_PHOBOS_dNdeta[0]->GetYaxis()->SetTickLength(yticklength / pad1_tickscale);
        v_PHOBOS_dNdeta[0]->GetXaxis()->SetTickLength(xticklength / pad1_tickscale * 0.9);

        v_PHOBOS_dNdeta[0]->Draw("A 3 P");

        v_sphenix_dNdeta[0]->Draw("5 P same");

        leg->AddEntry(v_sphenix_dNdeta[0], leg_sphenix_combine.c_str(), "pf");
        leg->AddEntry(v_PHOBOS_dNdeta[0], leg_phobos.c_str(), "pf");

        double x, y;
        x = v_sphenix_dNdeta[0]->GetPointX(v_sphenix_dNdeta[0]->GetN() - 1) + v_sphenix_dNdeta[0]->GetErrorXhigh(v_sphenix_dNdeta[0]->GetN() - 1);
        y = v_sphenix_dNdeta[0]->GetPointY(v_sphenix_dNdeta[0]->GetN() - 1) + v_sphenix_dNdeta[0]->GetErrorYhigh(v_sphenix_dNdeta[0]->GetN() - 1);
        // print out the first point for debugging
        cout << v_sphenix_dNdeta[0]->GetPointX(0) << " " << v_sphenix_dNdeta[0]->GetPointY(0) << " " << v_sphenix_dNdeta[0]->GetErrorYlow(0) << " " << v_sphenix_dNdeta[0]->GetErrorYhigh(0) << endl;
        TLatex *l = new TLatex(x, y, v_SPHENIX_labels[0].c_str());
        l->SetTextAlign(kHAlignLeft + kVAlignBottom);
        l->SetTextSize(centlabel_textsize * pad1_labeltextscale);
        // set color to match the graph
        l->SetTextColor(TColor::GetColor(color_sphenix_combine.c_str()));
        l->Draw();
    }
    else // not draw phobos, will always draw sPHENIX separately
    {
        v_SPHENIX_dNdeta_phobosapproach[0]->SetMaximum(ymax_pad1);
        v_SPHENIX_dNdeta_phobosapproach[0]->SetMinimum(ymin_pad1);
        v_SPHENIX_dNdeta_phobosapproach[0]->GetYaxis()->SetNdivisions(4, 2, 0);
        v_SPHENIX_dNdeta_phobosapproach[0]->GetXaxis()->SetLimits(-absxlim, absxlim);
        v_SPHENIX_dNdeta_phobosapproach[0]->GetXaxis()->SetNdivisions(505);

        v_SPHENIX_dNdeta_phobosapproach[0]->GetYaxis()->SetTitle("dN_{ch}/d#eta");
        v_SPHENIX_dNdeta_phobosapproach[0]->GetYaxis()->CenterTitle(true);
        v_SPHENIX_dNdeta_phobosapproach[0]->GetYaxis()->SetTitleOffset(0.55);
        v_SPHENIX_dNdeta_phobosapproach[0]->GetYaxis()->SetTitleSize(axis_title_size * pad1_legtextscale);
        v_SPHENIX_dNdeta_phobosapproach[0]->GetYaxis()->SetLabelSize(axis_label_size * pad1_labeltextscale);
        v_SPHENIX_dNdeta_phobosapproach[0]->GetYaxis()->SetTickLength(yticklength / pad1_tickscale);
        v_SPHENIX_dNdeta_phobosapproach[0]->GetXaxis()->SetTickLength(xticklength / pad1_tickscale * 0.9);
        v_SPHENIX_dNdeta_phobosapproach[0]->Draw("A 5 P");
        v_SPHENIX_dNdeta_cmsapproach[0]->Draw("5 P same");

        leg->AddEntry(v_SPHENIX_dNdeta_phobosapproach[0], leg_sphenix_phobosapproach.c_str(), "pf");
        leg->AddEntry(v_SPHENIX_dNdeta_cmsapproach[0], leg_sphenix_cmsapproach.c_str(), "pf");

        // draw labels
        double x, y;
        x = v_SPHENIX_dNdeta_phobosapproach[0]->GetPointX(v_SPHENIX_dNdeta_phobosapproach[0]->GetN() - 1) + v_SPHENIX_dNdeta_phobosapproach[0]->GetErrorXlow(v_SPHENIX_dNdeta_phobosapproach[0]->GetN() - 1);
        y = v_SPHENIX_dNdeta_phobosapproach[0]->GetPointY(v_SPHENIX_dNdeta_phobosapproach[0]->GetN() - 1);
        cout << "phobos: x=" << x << " y=" << y << endl;
        TLatex *lphobos = new TLatex(x + 0.02, y, v_SPHENIX_labels[0].c_str());
        lphobos->SetTextAlign(kHAlignLeft + kVAlignCenter);
        lphobos->SetTextSize(centlabel_textsize * pad1_labeltextscale);
        // set color to match the graph
        lphobos->SetTextColor(TColor::GetColor(color_sphenix_phobosapproach.c_str()));
        lphobos->Draw();

        x = v_SPHENIX_dNdeta_cmsapproach[0]->GetPointX(0) - v_SPHENIX_dNdeta_cmsapproach[0]->GetErrorXlow(0);
        y = v_SPHENIX_dNdeta_cmsapproach[0]->GetPointY(0);
        cout << "cms: x=" << x << " y=" << y << endl;
        TLatex *lcms = new TLatex(x - 0.02, y, v_SPHENIX_labels[0].c_str());
        // set right and center adjusted
        lcms->SetTextAlign(kHAlignRight + kVAlignCenter);
        lcms->SetTextSize(centlabel_textsize * pad1_labeltextscale);
        // set color to match the graph
        lcms->SetTextColor(TColor::GetColor(color_sphenix_cmsapproach.c_str()));
        lcms->Draw();
    }
    leg->Draw();
    TLegend *sphnxleg = new TLegend(pad1->GetRightMargin() + 0.53,    //
                                    1 - pad1->GetTopMargin() - 0.38,  //
                                    1 - pad1->GetLeftMargin() - 0.01, //
                                    1 - pad1->GetTopMargin() - 0.1    //
    );
    sphnxleg->SetLineStyle(1);
    sphnxleg->SetLineWidth(1);
    sphnxleg->SetLineColor(1);
    sphnxleg->SetTextAlign(kHAlignLeft + kVAlignCenter);
    sphnxleg->SetTextSize(leg_textsize * 2.7);
    sphnxleg->SetFillStyle(0);
    sphnxleg->AddEntry("", Form("#it{#bf{sPHENIX}} %s", prelimtext.c_str()), "");
    sphnxleg->AddEntry("", "Au+Au #sqrt{s_{NN}}=200 GeV", "");
    sphnxleg->Draw();

    pad2->cd();
    if (draw_phobos) // will always draw combined sPHENIX
    {
        v_PHOBOS_dNdeta[1]->SetMaximum(ymax_pad2);
        v_PHOBOS_dNdeta[1]->SetMinimum(ymin_pad2);
        v_PHOBOS_dNdeta[1]->GetYaxis()->SetNdivisions(4, 2, 0);
        v_PHOBOS_dNdeta[1]->GetXaxis()->SetLimits(-absxlim, absxlim);
        v_PHOBOS_dNdeta[1]->GetXaxis()->SetNdivisions(505);
        v_PHOBOS_dNdeta[1]->Draw("A 3 P");

        v_PHOBOS_dNdeta[1]->GetYaxis()->SetLabelSize(axis_label_size * pad2_labeltextscale);
        v_PHOBOS_dNdeta[1]->GetYaxis()->SetTickLength(yticklength / pad2_tickscale);
        v_PHOBOS_dNdeta[1]->GetXaxis()->SetTickLength(xticklength / pad2_tickscale * 2);

        v_sphenix_dNdeta[1]->Draw("5 P same");

        double x, y;
        x = v_sphenix_dNdeta[1]->GetPointX(v_sphenix_dNdeta[1]->GetN() - 1) + v_sphenix_dNdeta[1]->GetErrorXhigh(v_sphenix_dNdeta[1]->GetN() - 1);
        y = v_sphenix_dNdeta[1]->GetPointY(v_sphenix_dNdeta[1]->GetN() - 1) + v_sphenix_dNdeta[1]->GetErrorYhigh(v_sphenix_dNdeta[1]->GetN() - 1);
        // print out the first point for debugging
        cout << v_sphenix_dNdeta[1]->GetPointX(0) << " " << v_sphenix_dNdeta[1]->GetPointY(0) << " " << v_sphenix_dNdeta[1]->GetErrorYlow(0) << " " << v_sphenix_dNdeta[1]->GetErrorYhigh(0) << endl;
        TLatex *l = new TLatex(x, y, v_SPHENIX_labels[1].c_str());
        l->SetTextAlign(kHAlignLeft + kVAlignBottom);
        l->SetTextSize(centlabel_textsize * pad2_labeltextscale);
        // set color to match the graph
        l->SetTextColor(TColor::GetColor(color_sphenix_combine.c_str()));
        l->Draw();
    }
    else // not draw phobos, will always draw sPHENIX separately
    {
        v_SPHENIX_dNdeta_phobosapproach[1]->SetMaximum(ymax_pad2);
        v_SPHENIX_dNdeta_phobosapproach[1]->SetMinimum(ymin_pad2);
        v_SPHENIX_dNdeta_phobosapproach[1]->GetYaxis()->SetNdivisions(4, 2, 0);
        v_SPHENIX_dNdeta_phobosapproach[1]->GetXaxis()->SetLimits(-absxlim, absxlim);
        v_SPHENIX_dNdeta_phobosapproach[1]->GetXaxis()->SetNdivisions(505);

        v_SPHENIX_dNdeta_phobosapproach[1]->GetYaxis()->SetLabelSize(axis_label_size * pad2_labeltextscale);
        v_SPHENIX_dNdeta_phobosapproach[1]->GetYaxis()->SetTickLength(yticklength / pad2_tickscale);
        v_SPHENIX_dNdeta_phobosapproach[1]->GetXaxis()->SetTickLength(xticklength / pad2_tickscale * 2);
        v_SPHENIX_dNdeta_phobosapproach[1]->Draw("A 5 P");
        v_SPHENIX_dNdeta_cmsapproach[1]->Draw("5 P");

        // draw labels
        double x, y;
        x = v_SPHENIX_dNdeta_phobosapproach[1]->GetPointX(v_SPHENIX_dNdeta_phobosapproach[1]->GetN() - 1) + v_SPHENIX_dNdeta_phobosapproach[1]->GetErrorXlow(v_SPHENIX_dNdeta_phobosapproach[1]->GetN() - 1);
        y = v_SPHENIX_dNdeta_phobosapproach[1]->GetPointY(v_SPHENIX_dNdeta_phobosapproach[1]->GetN() - 1);
        cout << "phobos: x=" << x << " y=" << y << endl;
        TLatex *lphobos = new TLatex(x + 0.02, y, v_SPHENIX_labels[1].c_str());
        lphobos->SetTextAlign(kHAlignLeft + kVAlignCenter);
        lphobos->SetTextSize(centlabel_textsize * pad2_labeltextscale);
        // set color to match the graph
        lphobos->SetTextColor(TColor::GetColor(color_sphenix_phobosapproach.c_str()));
        lphobos->Draw();

        x = v_SPHENIX_dNdeta_cmsapproach[1]->GetPointX(0) - v_SPHENIX_dNdeta_cmsapproach[1]->GetErrorXlow(0);
        y = v_SPHENIX_dNdeta_cmsapproach[1]->GetPointY(0);
        cout << "cms: x=" << x << " y=" << y << endl;
        TLatex *lcms = new TLatex(x - 0.02, y, v_SPHENIX_labels[1].c_str());
        // set right and center adjusted
        lcms->SetTextAlign(kHAlignRight + kVAlignCenter);
        lcms->SetTextSize(centlabel_textsize * pad2_labeltextscale);
        // set color to match the graph
        lcms->SetTextColor(TColor::GetColor(color_sphenix_cmsapproach.c_str()));
        lcms->Draw();
    }

    pad3->cd();
    if (draw_phobos) // will always draw combined sPHENIX
    {
        v_PHOBOS_dNdeta[2]->SetMaximum(ymax_pad3);
        v_PHOBOS_dNdeta[2]->SetMinimum(ymin_pad3);
        v_PHOBOS_dNdeta[2]->GetYaxis()->SetNdivisions(3, 2, 0);
        v_PHOBOS_dNdeta[2]->GetXaxis()->SetLimits(-absxlim, absxlim);
        v_PHOBOS_dNdeta[2]->GetXaxis()->SetNdivisions(505);
        v_PHOBOS_dNdeta[2]->Draw("A 3 P");

        v_PHOBOS_dNdeta[2]->GetYaxis()->SetLabelSize(axis_label_size * pad3_labeltextscale);
        v_PHOBOS_dNdeta[2]->GetYaxis()->SetTickLength(yticklength / pad3_tickscale);
        v_PHOBOS_dNdeta[2]->GetXaxis()->SetTickLength(xticklength / pad3_tickscale * 2);

        v_sphenix_dNdeta[2]->Draw("5 P same");

        double x, y;
        x = v_sphenix_dNdeta[2]->GetPointX(v_sphenix_dNdeta[2]->GetN() - 1) + v_sphenix_dNdeta[2]->GetErrorXhigh(v_sphenix_dNdeta[2]->GetN() - 1);
        y = v_sphenix_dNdeta[2]->GetPointY(v_sphenix_dNdeta[2]->GetN() - 1) + v_sphenix_dNdeta[2]->GetErrorYhigh(v_sphenix_dNdeta[2]->GetN() - 1);
        // print out the first point for debugging
        cout << v_sphenix_dNdeta[2]->GetPointX(0) << " " << v_sphenix_dNdeta[2]->GetPointY(0) << " " << v_sphenix_dNdeta[2]->GetErrorYlow(0) << " " << v_sphenix_dNdeta[2]->GetErrorYhigh(0) << endl;
        TLatex *l = new TLatex(x, y, v_SPHENIX_labels[2].c_str());
        l->SetTextAlign(kHAlignLeft + kVAlignBottom);
        l->SetTextSize(centlabel_textsize * pad3_labeltextscale);
        // set color to match the graph
        l->SetTextColor(TColor::GetColor(color_sphenix_combine.c_str()));
        l->Draw();
    }
    else // not draw phobos, will always draw sPHENIX separately
    {
        v_SPHENIX_dNdeta_phobosapproach[2]->SetMaximum(ymax_pad3);
        v_SPHENIX_dNdeta_phobosapproach[2]->SetMinimum(ymin_pad3);
        v_SPHENIX_dNdeta_phobosapproach[2]->GetYaxis()->SetNdivisions(3, 2, 0);
        v_SPHENIX_dNdeta_phobosapproach[2]->GetXaxis()->SetLimits(-absxlim, absxlim);
        v_SPHENIX_dNdeta_phobosapproach[2]->GetXaxis()->SetNdivisions(505);

        v_SPHENIX_dNdeta_phobosapproach[2]->GetYaxis()->SetLabelSize(axis_label_size * pad3_labeltextscale);
        v_SPHENIX_dNdeta_phobosapproach[2]->GetYaxis()->SetTickLength(yticklength / pad3_tickscale);
        v_SPHENIX_dNdeta_phobosapproach[2]->GetXaxis()->SetTickLength(xticklength / pad3_tickscale * 2);
        v_SPHENIX_dNdeta_phobosapproach[2]->Draw("A 5 P");
        v_SPHENIX_dNdeta_cmsapproach[2]->Draw("5 P");

        // draw labels
        double x, y;
        x = v_SPHENIX_dNdeta_phobosapproach[2]->GetPointX(v_SPHENIX_dNdeta_phobosapproach[2]->GetN() - 1) + v_SPHENIX_dNdeta_phobosapproach[2]->GetErrorXlow(v_SPHENIX_dNdeta_phobosapproach[2]->GetN() - 1);
        y = v_SPHENIX_dNdeta_phobosapproach[2]->GetPointY(v_SPHENIX_dNdeta_phobosapproach[2]->GetN() - 1);
        cout << "phobos: x=" << x << " y=" << y << endl;
        TLatex *lphobos = new TLatex(x + 0.02, y, v_SPHENIX_labels[2].c_str());
        lphobos->SetTextAlign(kHAlignLeft + kVAlignCenter);
        lphobos->SetTextSize(centlabel_textsize * pad3_labeltextscale);
        // set color to match the graph
        lphobos->SetTextColor(TColor::GetColor(color_sphenix_phobosapproach.c_str()));
        lphobos->Draw();

        x = v_SPHENIX_dNdeta_cmsapproach[2]->GetPointX(0) - v_SPHENIX_dNdeta_cmsapproach[2]->GetErrorXlow(0);
        y = v_SPHENIX_dNdeta_cmsapproach[2]->GetPointY(0);
        cout << "cms: x=" << x << " y=" << y << endl;
        TLatex *lcms = new TLatex(x - 0.02, y, v_SPHENIX_labels[2].c_str());
        // set right and center adjusted
        lcms->SetTextAlign(kHAlignRight + kVAlignCenter);
        lcms->SetTextSize(centlabel_textsize * pad3_labeltextscale);
        // set color to match the graph
        lcms->SetTextColor(TColor::GetColor(color_sphenix_cmsapproach.c_str()));
        lcms->Draw();
    }

    pad4->cd();
    if (draw_phobos) // will always draw combined sPHENIX
    {
        v_PHOBOS_dNdeta[3]->SetMaximum(ymax_pad4);
        v_PHOBOS_dNdeta[3]->SetMinimum(ymin_pad4);
        v_PHOBOS_dNdeta[3]->GetYaxis()->SetNdivisions(4, 3, 0);
        v_PHOBOS_dNdeta[3]->GetXaxis()->SetLimits(-absxlim, absxlim);
        v_PHOBOS_dNdeta[3]->GetXaxis()->SetNdivisions(505);
        v_PHOBOS_dNdeta[3]->Draw("A 3 P");

        v_PHOBOS_dNdeta[3]->GetYaxis()->SetLabelSize(axis_label_size * pad4_labeltextscale);
        v_PHOBOS_dNdeta[3]->GetYaxis()->SetTickLength(yticklength / pad4_tickscale);
        v_PHOBOS_dNdeta[3]->GetXaxis()->SetTickLength(xticklength / pad4_tickscale * 2);

        v_sphenix_dNdeta[3]->Draw("5 P same");

        double x, y;
        x = v_sphenix_dNdeta[3]->GetPointX(v_sphenix_dNdeta[3]->GetN() - 1) + v_sphenix_dNdeta[3]->GetErrorXhigh(v_sphenix_dNdeta[3]->GetN() - 1);
        y = v_sphenix_dNdeta[3]->GetPointY(v_sphenix_dNdeta[3]->GetN() - 1) + v_sphenix_dNdeta[3]->GetErrorYhigh(v_sphenix_dNdeta[3]->GetN() - 1);
        // print out the first point for debugging
        cout << v_sphenix_dNdeta[3]->GetPointX(0) << " " << v_sphenix_dNdeta[3]->GetPointY(0) << " " << v_sphenix_dNdeta[3]->GetErrorYlow(0) << " " << v_sphenix_dNdeta[3]->GetErrorYhigh(0) << endl;
        TLatex *l = new TLatex(x, y, v_SPHENIX_labels[3].c_str());
        l->SetTextAlign(kHAlignLeft + kVAlignBottom);
        l->SetTextSize(centlabel_textsize * pad4_labeltextscale);
        // set color to match the graph
        l->SetTextColor(TColor::GetColor(color_sphenix_combine.c_str()));
        l->Draw();
    }
    else // not draw phobos, will always draw sPHENIX separately
    {
        v_SPHENIX_dNdeta_phobosapproach[3]->SetMaximum(ymax_pad4);
        v_SPHENIX_dNdeta_phobosapproach[3]->SetMinimum(ymin_pad4);
        v_SPHENIX_dNdeta_phobosapproach[3]->GetYaxis()->SetNdivisions(4, 3, 0);
        v_SPHENIX_dNdeta_phobosapproach[3]->GetXaxis()->SetLimits(-absxlim, absxlim);
        v_SPHENIX_dNdeta_phobosapproach[3]->GetXaxis()->SetNdivisions(505);

        v_SPHENIX_dNdeta_phobosapproach[3]->GetYaxis()->SetLabelSize(axis_label_size * pad4_labeltextscale);
        v_SPHENIX_dNdeta_phobosapproach[3]->GetYaxis()->SetTickLength(yticklength / pad4_tickscale);
        v_SPHENIX_dNdeta_phobosapproach[3]->GetXaxis()->SetTickLength(xticklength / pad4_tickscale * 2);
        v_SPHENIX_dNdeta_phobosapproach[3]->Draw("A 5 P");
        v_SPHENIX_dNdeta_cmsapproach[3]->Draw("5 P");

        // draw labels
        double x, y;
        x = v_SPHENIX_dNdeta_phobosapproach[3]->GetPointX(v_SPHENIX_dNdeta_phobosapproach[3]->GetN() - 1) + v_SPHENIX_dNdeta_phobosapproach[3]->GetErrorXlow(v_SPHENIX_dNdeta_phobosapproach[3]->GetN() - 1);
        y = v_SPHENIX_dNdeta_phobosapproach[3]->GetPointY(v_SPHENIX_dNdeta_phobosapproach[3]->GetN() - 1);
        cout << "phobos: x=" << x << " y=" << y << endl;
        TLatex *lphobos = new TLatex(x + 0.02, y, v_SPHENIX_labels[3].c_str());
        lphobos->SetTextAlign(kHAlignLeft + kVAlignCenter);
        lphobos->SetTextSize(centlabel_textsize * pad4_labeltextscale);
        // set color to match the graph
        lphobos->SetTextColor(TColor::GetColor(color_sphenix_phobosapproach.c_str()));
        lphobos->Draw();

        x = v_SPHENIX_dNdeta_cmsapproach[3]->GetPointX(0) - v_SPHENIX_dNdeta_cmsapproach[3]->GetErrorXlow(0);
        y = v_SPHENIX_dNdeta_cmsapproach[3]->GetPointY(0);
        cout << "cms: x=" << x << " y=" << y << endl;
        TLatex *lcms = new TLatex(x - 0.02, y, v_SPHENIX_labels[3].c_str());
        // set right and center adjusted
        lcms->SetTextAlign(kHAlignRight + kVAlignCenter);
        lcms->SetTextSize(centlabel_textsize * pad2_labeltextscale);
        // set color to match the graph
        lcms->SetTextColor(TColor::GetColor(color_sphenix_cmsapproach.c_str()));
        lcms->Draw();
    }

    pad5->cd();
    if (draw_phobos) // will always draw combined sPHENIX
    {
        for (int i = 4; i < nsphenix; i++)
        {
            if (i == 4)
            {
                v_PHOBOS_dNdeta[i]->SetMaximum(ymax_pad5);
                v_PHOBOS_dNdeta[i]->SetMinimum(ymin_pad5);
                v_PHOBOS_dNdeta[i]->GetYaxis()->SetNdivisions(10, 3, 0);
                v_PHOBOS_dNdeta[i]->GetXaxis()->SetLimits(-absxlim, absxlim);
                v_PHOBOS_dNdeta[i]->GetXaxis()->SetNdivisions(505);

                v_PHOBOS_dNdeta[i]->GetYaxis()->SetLabelSize(axis_label_size * pad5_labeltextscale);
                v_PHOBOS_dNdeta[i]->GetYaxis()->SetTickLength(yticklength / pad5_tickscale);
                v_PHOBOS_dNdeta[i]->GetXaxis()->SetTitle("Pseudorapidity #eta");
                v_PHOBOS_dNdeta[i]->GetXaxis()->SetTitleOffset(1.4);
                v_PHOBOS_dNdeta[i]->GetXaxis()->SetTitleSize(axis_title_size * pad5_labeltextscale);
                v_PHOBOS_dNdeta[i]->GetXaxis()->SetLabelSize(axis_label_size * pad5_labeltextscale);
                v_PHOBOS_dNdeta[i]->GetXaxis()->SetTickLength(xticklength / pad5_tickscale * 0.5);

                v_PHOBOS_dNdeta[i]->Draw("A 3 P");
            }
            else
            {
                v_PHOBOS_dNdeta[i]->Draw("3 P");
            }

            v_sphenix_dNdeta[i]->Draw("5 P same");

            double x, y;
            x = v_sphenix_dNdeta[i]->GetPointX(v_sphenix_dNdeta[i]->GetN() - 1) + v_sphenix_dNdeta[i]->GetErrorXhigh(v_sphenix_dNdeta[i]->GetN() - 1);
            y = v_sphenix_dNdeta[i]->GetPointY(v_sphenix_dNdeta[i]->GetN() - 1) + v_sphenix_dNdeta[i]->GetErrorYhigh(v_sphenix_dNdeta[i]->GetN() - 1);
            // print out the first point for debugging
            cout << v_sphenix_dNdeta[i]->GetPointX(0) << " " << v_sphenix_dNdeta[i]->GetPointY(0) << " " << v_sphenix_dNdeta[i]->GetErrorYlow(0) << " " << v_sphenix_dNdeta[i]->GetErrorYhigh(0) << endl;
            TLatex *l = new TLatex(x, y, v_SPHENIX_labels[i].c_str());
            l->SetTextAlign(kHAlignLeft + kVAlignBottom);
            l->SetTextSize(centlabel_textsize * pad5_labeltextscale);
            // set color to match the graph
            l->SetTextColor(TColor::GetColor(color_sphenix_combine.c_str()));
            l->Draw();
        }
    }
    else // not draw phobos, will always draw sPHENIX separately
    {
        for (int i = 4; i < nsphenix; i++)
        {
            if (i == 4)
            {
                v_SPHENIX_dNdeta_phobosapproach[i]->SetMaximum(ymax_pad5);
                v_SPHENIX_dNdeta_phobosapproach[i]->SetMinimum(ymin_pad5);
                v_SPHENIX_dNdeta_phobosapproach[i]->GetYaxis()->SetNdivisions(10, 3, 0);
                v_SPHENIX_dNdeta_phobosapproach[i]->GetXaxis()->SetLimits(-absxlim, absxlim);
                v_SPHENIX_dNdeta_phobosapproach[i]->GetXaxis()->SetNdivisions(505);

                v_SPHENIX_dNdeta_phobosapproach[i]->GetYaxis()->SetLabelSize(axis_label_size * pad5_labeltextscale);
                v_SPHENIX_dNdeta_phobosapproach[i]->GetYaxis()->SetTickLength(yticklength / pad5_tickscale);
                v_SPHENIX_dNdeta_phobosapproach[i]->GetXaxis()->SetTitle("Pseudorapidity #eta");
                v_SPHENIX_dNdeta_phobosapproach[i]->GetXaxis()->SetTitleOffset(1.4);
                v_SPHENIX_dNdeta_phobosapproach[i]->GetXaxis()->SetTitleSize(axis_title_size * pad5_labeltextscale);
                v_SPHENIX_dNdeta_phobosapproach[i]->GetXaxis()->SetLabelSize(axis_label_size * pad5_labeltextscale);
                v_SPHENIX_dNdeta_phobosapproach[i]->GetXaxis()->SetTickLength(xticklength / pad5_tickscale * 0.5);
                v_SPHENIX_dNdeta_phobosapproach[i]->Draw("A 5 P");
                v_SPHENIX_dNdeta_cmsapproach[i]->Draw("5 P");
            }
            else
            {
                v_SPHENIX_dNdeta_phobosapproach[i]->Draw("5 P");
                v_SPHENIX_dNdeta_cmsapproach[i]->Draw("5 P");
            }

            // draw labels
            double x, y;
            x = v_SPHENIX_dNdeta_phobosapproach[i]->GetPointX(v_SPHENIX_dNdeta_phobosapproach[i]->GetN() - 1) + v_SPHENIX_dNdeta_phobosapproach[i]->GetErrorXlow(v_SPHENIX_dNdeta_phobosapproach[i]->GetN() - 1);
            y = v_SPHENIX_dNdeta_phobosapproach[i]->GetPointY(v_SPHENIX_dNdeta_phobosapproach[i]->GetN() - 1);
            cout << "phobos: x=" << x << " y=" << y << endl;
            TLatex *lphobos = new TLatex(x + 0.02, y, v_SPHENIX_labels[i].c_str());
            lphobos->SetTextAlign(kHAlignLeft + kVAlignCenter);
            lphobos->SetTextSize(centlabel_textsize * pad5_labeltextscale);
            // set color to match the graph
            lphobos->SetTextColor(TColor::GetColor(color_sphenix_phobosapproach.c_str()));
            lphobos->Draw();

            x = v_SPHENIX_dNdeta_cmsapproach[i]->GetPointX(0) - v_SPHENIX_dNdeta_cmsapproach[i]->GetErrorXlow(0);
            y = v_SPHENIX_dNdeta_cmsapproach[i]->GetPointY(0);
            cout << "cms: x=" << x << " y=" << y << endl;
            TLatex *lcms = new TLatex(x - 0.02, y, v_SPHENIX_labels[i].c_str());
            // set right and center adjusted
            lcms->SetTextAlign(kHAlignRight + kVAlignCenter);
            lcms->SetTextSize(centlabel_textsize * pad5_labeltextscale);
            // set color to match the graph
            lcms->SetTextColor(TColor::GetColor(color_sphenix_cmsapproach.c_str()));
            lcms->Draw();
        }
    }

    c->RedrawAxis();
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