#include "./util_combine.h"

bool verbose = true;

bool docombine = true;
std::string comb_fname = "./systematics/combined/combined.root";
std::vector<TGraphAsymmErrors *> v_sphenix_dNdeta;
std::string color_sphenix_combine = "#1A1423";
std::string leg_sphenix_combine = "sPHENIX (Combined)";
int mkstyle_sphenix_combine = 20;
float mksize_sphenix_combine = 0.9;

std::vector<float> v_sphenix_centralitybin = {0, 3, 6, 10, 15, 20, 25, 30, 35, 40, 45, 50};
int nsphenix = v_sphenix_centralitybin.size() - 1;
std::vector<TH1 *> v_sphenix_dNdeta_cmsapproach_hist;
std::vector<TGraphAsymmErrors *> v_SPHENIX_dNdeta_cmsapproach;
std::vector<std::string> v_SPHENIX_labels = {};
std::string color_sphenix_cmsapproach = "#FF6700";
std::string leg_sphenix_cmsapproach = "The closest-match method";
int mkstyle_sphenix_cmsapproach = 34;
float mksize_sphenix_cmsapproach = 0.9;

std::vector<TH1 *> v_sphenix_dNdeta_phobosapproach_hist;
std::vector<TGraphAsymmErrors *> v_SPHENIX_dNdeta_phobosapproach;
std::string color_sphenix_phobosapproach = "#228B22";
std::string leg_sphenix_phobosapproach = "The combinatoric method";
int mkstyle_sphenix_phobosapproach = 47;
float mksize_sphenix_phobosapproach = 0.9;

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
// std::string leg_phobos = "PHOBOS [Phys. Rev. C 83, 024913 (2011)]";
int mkstyle_phobos = 25;
float mksize_phobos = 0.9;

float absxlim = (draw_phobos) ? 1.99 : 1.59;
// std::string prelimtext = "Internal";
std::string prelimtext = "";

float LeftMargin = 0.25;
float RightMargin = 0.05;
float TopMargin = 0.05;
float BottomMargin = 0.13;
bool logy = true;
float ymax_pad = (logy) ? 1800 : ((docombine) ? 940 : 970);
float ymin_pad = (logy) ? 75 : 60;
float axis_title_size = 0.045;
float axis_label_size = 0.04;
float leg_textsize = 0.04;
float centlabel_textsize = 0.025;
float xticklength = 0.03;
float yticklength = 0.03;
float pad1_legtextscale = 1.1;
float pad1_titletextscale = 1.2;
float pad1_labeltextscale = 1.1;
float pad1_ytitleoffset = 2.2;
float pad2_labeltextscale = 1.4;
float pad2_tickscale = 1.25;

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

void draw_dNdetaRHIC_v3()
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
            // cout << "Centrality: " << v_SPHENIX_labels[i] << endl;
            for (int j = 0; j < v_SPHENIX_dNdeta_cmsapproach[i]->GetN(); j++)
            {
                double x, y, xerrl, xerru, yerrl, yerru;
                v_SPHENIX_dNdeta_cmsapproach[i]->GetPoint(j, x, y);
                xerrl = v_SPHENIX_dNdeta_cmsapproach[i]->GetErrorXlow(j);
                xerru = v_SPHENIX_dNdeta_cmsapproach[i]->GetErrorXhigh(j);
                yerrl = v_SPHENIX_dNdeta_cmsapproach[i]->GetErrorYlow(j);
                yerru = v_SPHENIX_dNdeta_cmsapproach[i]->GetErrorYhigh(j);
                // cout << "CMS approach: " << x << " " << y << " " << xerrl << " " << xerru << " " << yerrl << " " << yerru << endl;
            }
            for (int j = 0; j < v_SPHENIX_dNdeta_phobosapproach[i]->GetN(); j++)
            {
                double x, y, xerrl, xerru, yerrl, yerru;
                v_SPHENIX_dNdeta_phobosapproach[i]->GetPoint(j, x, y);
                xerrl = v_SPHENIX_dNdeta_phobosapproach[i]->GetErrorXlow(j);
                xerru = v_SPHENIX_dNdeta_phobosapproach[i]->GetErrorXhigh(j);
                yerrl = v_SPHENIX_dNdeta_phobosapproach[i]->GetErrorYlow(j);
                yerru = v_SPHENIX_dNdeta_phobosapproach[i]->GetErrorYhigh(j);
                // cout << "PHOBOS approach: " << x << " " << y << " " << xerrl << " " << xerru << " " << yerrl << " " << yerru << endl;
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

    TCanvas *c = new TCanvas("c", "c", 1000, 600);
    TPad *pad1 = new TPad("pad1", "pad1", 0, 0.0, 0.56, 1);
    TPad *pad2 = new TPad("pad2", "pad2", 0.56, 0.0, 1, 1);
    pad1->SetTopMargin(TopMargin);
    pad1->SetBottomMargin(BottomMargin);
    pad1->SetLeftMargin(LeftMargin);
    pad1->SetRightMargin(0);
    pad2->SetTopMargin(TopMargin);
    pad2->SetBottomMargin(BottomMargin);
    pad2->SetRightMargin(RightMargin);
    pad2->SetLeftMargin(0);
    pad1->Draw();
    pad2->Draw();
    pad1->cd();
    pad1->SetLogy(logy);
    // Legend
    if (draw_phobos) // will always draw combined sPHENIX
    {
        // loop over v_PHOBOS_dNdeta and v_sphenix_dNdeta and only draw even indices
        for (size_t i = 0; i < v_PHOBOS_dNdeta.size() && i < v_sphenix_dNdeta.size(); i += 2)
        {
            if (i == 0)
            {
                v_PHOBOS_dNdeta[i]->SetMaximum(ymax_pad);
                v_PHOBOS_dNdeta[i]->SetMinimum(ymin_pad);
                // v_PHOBOS_dNdeta[i]->GetYaxis()->SetNdivisions(4, 2, 0);
                v_PHOBOS_dNdeta[i]->GetXaxis()->SetLimits(-absxlim, absxlim);
                if (logy)
                    v_PHOBOS_dNdeta[i]->GetYaxis()->SetMoreLogLabels();
                v_PHOBOS_dNdeta[i]->GetXaxis()->SetNdivisions(505);

                v_PHOBOS_dNdeta[i]->GetYaxis()->SetTitle("dN_{ch}/d#eta");
                // v_PHOBOS_dNdeta[i]->GetYaxis()->CenterTitle(true);
                v_PHOBOS_dNdeta[i]->GetYaxis()->SetTitleOffset(pad1_ytitleoffset);
                v_PHOBOS_dNdeta[i]->GetYaxis()->SetTitleSize(axis_title_size * pad1_titletextscale);
                v_PHOBOS_dNdeta[i]->GetYaxis()->SetLabelSize(axis_label_size * pad1_labeltextscale);
                v_PHOBOS_dNdeta[i]->GetYaxis()->SetTickLength(yticklength);
                v_PHOBOS_dNdeta[i]->GetXaxis()->SetLabelSize(axis_label_size * pad1_labeltextscale);
                v_PHOBOS_dNdeta[i]->GetXaxis()->SetTickLength(xticklength);

                v_PHOBOS_dNdeta[i]->Draw("A 3 P");
                v_sphenix_dNdeta[i]->Draw("5 P same");
            }
            else
            {
                v_PHOBOS_dNdeta[i]->Draw("3 P same");
                v_sphenix_dNdeta[i]->Draw("5 P same");
            }

            double x, y;
            x = v_sphenix_dNdeta[i]->GetPointX(v_sphenix_dNdeta[i]->GetN() - 1) + v_sphenix_dNdeta[i]->GetErrorXhigh(v_sphenix_dNdeta[i]->GetN() - 1);
            y = v_sphenix_dNdeta[i]->GetPointY(v_sphenix_dNdeta[i]->GetN() - 1) + v_sphenix_dNdeta[i]->GetErrorYhigh(v_sphenix_dNdeta[i]->GetN() - 1);
            // print out the first point for debugging
            cout << v_sphenix_dNdeta[i]->GetPointX(0) << " " << v_sphenix_dNdeta[i]->GetPointY(0) << " " << v_sphenix_dNdeta[i]->GetErrorYlow(0) << " " << v_sphenix_dNdeta[i]->GetErrorYhigh(0) << endl;
            TLatex *l = new TLatex(x, y, v_SPHENIX_labels[i].c_str());
            l->SetTextAlign(kHAlignLeft + kVAlignBottom);
            l->SetTextSize(centlabel_textsize * pad1_labeltextscale);
            // set color to match the graph
            l->SetTextColor(TColor::GetColor(color_sphenix_combine.c_str()));
            l->Draw();
        }
    }
    else
    {
        // loop over v_PHOBOS_dNdeta and v_sphenix_dNdeta and only draw even indices
        for (size_t i = 0; i < v_SPHENIX_dNdeta_phobosapproach.size() && i < v_SPHENIX_dNdeta_cmsapproach.size(); i += 2)
        {
            if (i == 0)
            {
                v_SPHENIX_dNdeta_phobosapproach[i]->SetMaximum(ymax_pad);
                v_SPHENIX_dNdeta_phobosapproach[i]->SetMinimum(ymin_pad);
                // v_SPHENIX_dNdeta_phobosapproach[i]->GetYaxis()->SetNdivisions(4, 2, 0);
                v_SPHENIX_dNdeta_phobosapproach[i]->GetXaxis()->SetLimits(-absxlim, absxlim);
                if (logy)
                    v_SPHENIX_dNdeta_phobosapproach[i]->GetYaxis()->SetMoreLogLabels();
                v_SPHENIX_dNdeta_phobosapproach[i]->GetXaxis()->SetNdivisions(505);

                v_SPHENIX_dNdeta_phobosapproach[i]->GetYaxis()->SetTitle("dN_{ch}/d#eta");
                // v_SPHENIX_dNdeta_phobosapproach[i]->GetYaxis()->CenterTitle(true);
                v_SPHENIX_dNdeta_phobosapproach[i]->GetYaxis()->SetTitleOffset(pad1_ytitleoffset);
                v_SPHENIX_dNdeta_phobosapproach[i]->GetYaxis()->SetTitleSize(axis_title_size * pad1_titletextscale);
                v_SPHENIX_dNdeta_phobosapproach[i]->GetYaxis()->SetLabelSize(axis_label_size * pad1_labeltextscale);
                v_SPHENIX_dNdeta_phobosapproach[i]->GetYaxis()->SetTickLength(yticklength);
                v_SPHENIX_dNdeta_phobosapproach[i]->GetXaxis()->SetTickLength(xticklength);

                v_SPHENIX_dNdeta_phobosapproach[i]->Draw("A 5 P");
                v_SPHENIX_dNdeta_cmsapproach[i]->Draw("5 P same");
            }
            else
            {
                v_SPHENIX_dNdeta_phobosapproach[i]->Draw("5 P same");
                v_SPHENIX_dNdeta_cmsapproach[i]->Draw("5 P same");
            }

            // draw labels
            double x, y;
            x = v_SPHENIX_dNdeta_phobosapproach[i]->GetPointX(v_SPHENIX_dNdeta_phobosapproach[i]->GetN() - 1) + v_SPHENIX_dNdeta_phobosapproach[i]->GetErrorXlow(v_SPHENIX_dNdeta_phobosapproach[i]->GetN() - 1);
            y = v_SPHENIX_dNdeta_phobosapproach[i]->GetPointY(v_SPHENIX_dNdeta_phobosapproach[i]->GetN() - 1);
            cout << "phobos: x=" << x << " y=" << y << endl;
            TLatex *lphobos = new TLatex(x + 0.02, y, v_SPHENIX_labels[i].c_str());
            lphobos->SetTextAlign(kHAlignLeft + kVAlignCenter);
            lphobos->SetTextSize(centlabel_textsize * pad1_labeltextscale);
            // set color to match the graph
            lphobos->SetTextColor(TColor::GetColor(color_sphenix_phobosapproach.c_str()));
            lphobos->Draw();

            x = v_SPHENIX_dNdeta_cmsapproach[i]->GetPointX(0) - v_SPHENIX_dNdeta_cmsapproach[i]->GetErrorXlow(0);
            y = v_SPHENIX_dNdeta_cmsapproach[i]->GetPointY(0);
            cout << "cms: x=" << x << " y=" << y << endl;
            TLatex *lcms = new TLatex(x - 0.02, y, v_SPHENIX_labels[i].c_str());
            // set right and center adjusted
            lcms->SetTextAlign(kHAlignRight + kVAlignCenter);
            lcms->SetTextSize(centlabel_textsize * pad1_labeltextscale);
            // set color to match the graph
            lcms->SetTextColor(TColor::GetColor(color_sphenix_cmsapproach.c_str()));
            lcms->Draw();
        }
    }
    TLegend *sphnxleg = new TLegend(pad1->GetLeftMargin() + 0.04,    //
                                    1 - pad1->GetTopMargin() - 0.16, //
                                    pad1->GetLeftMargin() + 0.2,     //
                                    1 - pad1->GetTopMargin() - 0.05  //
    );
    sphnxleg->SetLineStyle(1);
    sphnxleg->SetLineWidth(1);
    sphnxleg->SetLineColor(1);
    sphnxleg->SetTextAlign(kHAlignLeft + kVAlignCenter);
    sphnxleg->SetTextSize(leg_textsize);
    sphnxleg->SetFillStyle(0);
    sphnxleg->AddEntry("", Form("#it{#bf{sPHENIX}} %s", prelimtext.c_str()), "");
    sphnxleg->AddEntry("", "Au+Au #sqrt{s_{NN}}=200 GeV", "");
    sphnxleg->Draw();

    pad2->cd();
    pad2->SetLogy(logy);
    TLegend *leg = new TLegend((!docombine) ? pad2->GetLeftMargin() + 0.1 : pad2->GetLeftMargin() + 0.07,         //
                               (!docombine) ? 1 - pad2->GetTopMargin() - 0.175 : 1 - pad2->GetTopMargin() - 0.115, //
                               pad2->GetLeftMargin() + 0.55,                                                      //
                               (!docombine) ? 1 - pad2->GetTopMargin() - 0.05 : 1 - pad2->GetTopMargin() - 0.04   //
    );
    // leg->SetTextAlign(kHAlignLeft + kVAlignCenter);
    leg->SetTextSize(leg_textsize);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);

    // for phobos
    TLegend *leg2 = new TLegend((!docombine) ? pad2->GetLeftMargin() + 0.1 : pad2->GetLeftMargin() + 0.07, //
                                1 - pad2->GetTopMargin() - 0.19,                                           //
                                pad2->GetLeftMargin() + 0.55,                                              //
                                1 - pad2->GetTopMargin() - 0.115                                           //
    );
    leg2->SetTextAlign(kHAlignLeft + kVAlignTop);
    leg2->SetTextSize(leg_textsize);
    leg2->SetBorderSize(0);
    leg2->SetFillStyle(0);

    if (draw_phobos) // will always draw combined sPHENIX
    {
        // loop over v_PHOBOS_dNdeta and v_sphenix_dNdeta and only draw even indices
        for (size_t i = 1; i < v_PHOBOS_dNdeta.size() && i < v_sphenix_dNdeta.size(); i += 2)
        {
            if (i == 1)
            {
                v_PHOBOS_dNdeta[i]->SetMaximum(ymax_pad);
                v_PHOBOS_dNdeta[i]->SetMinimum(ymin_pad);
                // v_PHOBOS_dNdeta[i]->GetYaxis()->SetNdivisions(4, 2, 0);
                v_PHOBOS_dNdeta[i]->GetXaxis()->SetLimits(-absxlim, absxlim);
                if (logy)
                    v_SPHENIX_dNdeta_phobosapproach[i]->GetYaxis()->SetMoreLogLabels();
                v_PHOBOS_dNdeta[i]->GetXaxis()->SetNdivisions(505);

                v_PHOBOS_dNdeta[i]->GetXaxis()->SetLabelOffset(-0.005);
                v_PHOBOS_dNdeta[i]->GetXaxis()->SetLabelSize(axis_label_size * pad2_labeltextscale);
                v_PHOBOS_dNdeta[i]->GetXaxis()->SetTickLength(xticklength * pad2_tickscale / 1.6);
                v_PHOBOS_dNdeta[i]->GetYaxis()->SetTickLength(yticklength * pad2_tickscale);

                v_PHOBOS_dNdeta[i]->Draw("A 3 P");
                v_sphenix_dNdeta[i]->Draw("5 P same");
            }
            else
            {
                v_PHOBOS_dNdeta[i]->Draw("3 P same");
                v_sphenix_dNdeta[i]->Draw("5 P same");
            }

            double x, y;
            x = v_sphenix_dNdeta[i]->GetPointX(v_sphenix_dNdeta[i]->GetN() - 1) + v_sphenix_dNdeta[i]->GetErrorXhigh(v_sphenix_dNdeta[i]->GetN() - 1);
            y = v_sphenix_dNdeta[i]->GetPointY(v_sphenix_dNdeta[i]->GetN() - 1) + v_sphenix_dNdeta[i]->GetErrorYhigh(v_sphenix_dNdeta[i]->GetN() - 1);
            // print out the first point for debugging
            cout << v_sphenix_dNdeta[i]->GetPointX(0) << " " << v_sphenix_dNdeta[i]->GetPointY(0) << " " << v_sphenix_dNdeta[i]->GetErrorYlow(0) << " " << v_sphenix_dNdeta[i]->GetErrorYhigh(0) << endl;
            TLatex *l = new TLatex(x, y, v_SPHENIX_labels[i].c_str());
            l->SetTextAlign(kHAlignLeft + kVAlignBottom);
            l->SetTextSize(centlabel_textsize * pad2_labeltextscale);
            // set color to match the graph
            l->SetTextColor(TColor::GetColor(color_sphenix_combine.c_str()));
            l->Draw();
        }

        leg->AddEntry(v_sphenix_dNdeta[0], leg_sphenix_combine.c_str(), "pf");
        leg2->AddEntry(v_PHOBOS_dNdeta[0], leg_phobos.c_str(), "pf");
    }
    else
    {
        for (size_t i = 1; i < v_SPHENIX_dNdeta_phobosapproach.size() && i < v_SPHENIX_dNdeta_cmsapproach.size(); i += 2)
        {
            if (i == 1)
            {
                v_SPHENIX_dNdeta_phobosapproach[i]->SetMaximum(ymax_pad);
                v_SPHENIX_dNdeta_phobosapproach[i]->SetMinimum(ymin_pad);
                // v_SPHENIX_dNdeta_phobosapproach[i]->GetYaxis()->SetNdivisions(4, 2, 0);
                v_SPHENIX_dNdeta_phobosapproach[i]->GetXaxis()->SetLimits(-absxlim, absxlim);
                if (logy)
                    v_SPHENIX_dNdeta_phobosapproach[i]->GetYaxis()->SetMoreLogLabels();
                v_SPHENIX_dNdeta_phobosapproach[i]->GetXaxis()->SetNdivisions(505);

                v_SPHENIX_dNdeta_phobosapproach[i]->GetXaxis()->SetLabelOffset(-0.006);
                v_SPHENIX_dNdeta_phobosapproach[i]->GetXaxis()->SetLabelSize(axis_label_size * pad2_labeltextscale);
                v_SPHENIX_dNdeta_phobosapproach[i]->GetXaxis()->SetTickLength(xticklength * pad2_tickscale / 1.6);
                v_SPHENIX_dNdeta_phobosapproach[i]->GetYaxis()->SetTickLength(yticklength * pad2_tickscale);

                v_SPHENIX_dNdeta_phobosapproach[i]->Draw("A 5 P");
                v_SPHENIX_dNdeta_cmsapproach[i]->Draw("5 P same");
            }
            else
            {

                v_SPHENIX_dNdeta_phobosapproach[i]->Draw("5 P same");
                v_SPHENIX_dNdeta_cmsapproach[i]->Draw("5 P same");
            }

            // draw labels
            double x, y;
            x = v_SPHENIX_dNdeta_phobosapproach[i]->GetPointX(v_SPHENIX_dNdeta_phobosapproach[i]->GetN() - 1) + v_SPHENIX_dNdeta_phobosapproach[i]->GetErrorXlow(v_SPHENIX_dNdeta_phobosapproach[i]->GetN() - 1);
            y = v_SPHENIX_dNdeta_phobosapproach[i]->GetPointY(v_SPHENIX_dNdeta_phobosapproach[i]->GetN() - 1);
            cout << "phobos: x=" << x << " y=" << y << endl;
            TLatex *lphobos = new TLatex(x + 0.02, y, v_SPHENIX_labels[i].c_str());
            lphobos->SetTextAlign(kHAlignLeft + kVAlignCenter);
            lphobos->SetTextSize(centlabel_textsize * pad2_labeltextscale);
            // set color to match the graph
            lphobos->SetTextColor(TColor::GetColor(color_sphenix_phobosapproach.c_str()));
            lphobos->Draw();

            x = v_SPHENIX_dNdeta_cmsapproach[i]->GetPointX(0) - v_SPHENIX_dNdeta_cmsapproach[i]->GetErrorXlow(0);
            y = v_SPHENIX_dNdeta_cmsapproach[i]->GetPointY(0);
            cout << "cms: x=" << x << " y=" << y << endl;
            TLatex *lcms = new TLatex(x - 0.02, y, v_SPHENIX_labels[i].c_str());
            // set right and center adjusted
            lcms->SetTextAlign(kHAlignRight + kVAlignCenter);
            lcms->SetTextSize(centlabel_textsize * pad2_labeltextscale);
            // set color to match the graph
            lcms->SetTextColor(TColor::GetColor(color_sphenix_cmsapproach.c_str()));
            lcms->Draw();
        }

        leg->AddEntry(v_SPHENIX_dNdeta_phobosapproach[0], leg_sphenix_phobosapproach.c_str(), "pf");
        leg->AddEntry(v_SPHENIX_dNdeta_cmsapproach[0], leg_sphenix_cmsapproach.c_str(), "pf");
    }
    leg->Draw();
    if (draw_phobos)
        leg2->Draw();

    c->cd();
    // use TLatex to draw X-axis title
    TLatex *xaxistitle = new TLatex();
    xaxistitle->SetTextAlign(kHAlignCenter + kVAlignCenter);
    xaxistitle->SetTextSize(0.049);
    xaxistitle->DrawLatexNDC(0.56, 0.035, "Pseudorapidity #eta");
    c->RedrawAxis();
    if (docombine)
    {
        if (draw_brahms)
        {
            c->SaveAs(Form("%s/dNdEta_RHIC_combine_wBRAHMS_altCent.pdf", plotdir.c_str()));
            c->SaveAs(Form("%s/dNdEta_RHIC_combine_wBRAHMS_altCent.png", plotdir.c_str()));
        }
        else
        {
            c->SaveAs(Form("%s/dNdEta_RHIC_combine_woBRAHMS_altCent.pdf", plotdir.c_str()));
            c->SaveAs(Form("%s/dNdEta_RHIC_combine_woBRAHMS_altCent.png", plotdir.c_str()));
        }
    }
    else
    {
        if (draw_brahms)
        {
            c->SaveAs(Form("%s/dNdEta_RHIC_wBRAHMS_altCent.pdf", plotdir.c_str()));
            c->SaveAs(Form("%s/dNdEta_RHIC_wBRAHMS_altCent.png", plotdir.c_str()));
        }
        else
        {
            c->SaveAs(Form("%s/dNdEta_RHIC_woBRAHMS_altCent.pdf", plotdir.c_str()));
            c->SaveAs(Form("%s/dNdEta_RHIC_woBRAHMS_altCent.png", plotdir.c_str()));
        }
    }
}