#include "./util_combine.h"

std::vector<double> v_centralitybin = {0, 3, 6, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70};

std::vector<std::string> v_phobos_histname = {"h1D_error_DeltaPhi", "h1D_error_ClusAdc", "h1D_error_ClusPhiSize", "h1D_error_Run_segmentation", "h1D_error_statistic", "h1D_error_GeoOffset"};
std::vector<std::string> v_cms_histname = {"hM_maxreldiff_dRcut", "hM_maxreldiff_clusAdcCut", "hM_maxreldiff_clusPhiSizeCut", "hM_maxreldiff_segment", "hM_statunc_nominal", "hM_maxreldiff_GeoOffset"};
std::vector<std::string> v_unclabel = {"Tracklet reconstruction selection", "Cluster ADC cut", "Cluster #phi-size cut", "Run segments", "Statistical uncertainty in corrections", "Geometric offset"};
std::vector<std::string> v_plotname = {"dRcut", "clusAdcCut", "clusPhiSizeCut", "segment", "statUnc", "GeoOffset"};
std::vector<std::string> v_color = {"#f2777a", "#6699cc", "#9999cc", "#99cc99", "#888888", "#ffcc66"};
std::vector<double> v_alpha = {0.7, 0.7, 0.6, 0.5, 0.8, 0.7};
std::vector<int> v_marker = {20, 21, 33, 34, 47, 45};
std::vector<double> v_corr = {}; // the same order as v_phobos_histname/v_cms_histname
int Nbins_dNdeta = 27;
double xbinlow_dNdeta = -2.7;
double xbinhigh_dNdeta = 2.7;

void setbinattr()
{
    TFile *f = new TFile("./systematics/Centrality0to3_Zvtxm10p0to10p0_noasel/finalhists_systematics_Centrality0to3_Zvtxm10p0to10p0_noasel.root", "READ");
    TH1 *h = (TH1 *)f->Get("hM_final");
    h->SetDirectory(0);
    Nbins_dNdeta = h->GetNbinsX();
    xbinlow_dNdeta = h->GetXaxis()->GetXmin();
    xbinhigh_dNdeta = h->GetXaxis()->GetXmax();
    f->Close();
}

TH1 *gethist(std::string fname, std::string histname)
{
    TFile *f = new TFile(fname.c_str(), "READ");
    TH1 *h;
    if (!f->Get(histname.c_str()))
    {
        std::cout << "Warning: cannot find histogram " << histname << " in file " << fname << std::endl;
        std::cout << "Set a hisgoram with all bins = 0" << std::endl;
        h = new TH1D(histname.c_str(), histname.c_str(), Nbins_dNdeta, xbinlow_dNdeta, xbinhigh_dNdeta);
        for (int i = 1; i <= Nbins_dNdeta; i++)
        {
            h->SetBinContent(i, 0);
        }
    }
    else
    {
        h = (TH1 *)f->Get(histname.c_str());
    }
    h->SetDirectory(0);
    f->Close();
    return h;
}

void drawTGraph(TGraph *g, std::string xtitle, std::string ytitle, std::vector<std::string> plotinfo, std::string plotname)
{
    TCanvas *c = new TCanvas("c", "c", 800, 700);
    gPad->SetRightMargin(0.07);
    gPad->SetTopMargin(0.07);
    gPad->SetLeftMargin(0.17);
    c->cd();
    g->SetMarkerStyle(20);
    g->SetMarkerSize(1);
    g->GetXaxis()->SetTitle(xtitle.c_str());
    g->GetYaxis()->SetTitle(ytitle.c_str());
    g->GetYaxis()->SetTitleOffset(1.7);
    g->GetXaxis()->SetLimits(g->GetXaxis()->GetXmin() * 0.9, g->GetXaxis()->GetXmax() * 1.1);
    g->GetHistogram()->SetMaximum(g->GetYaxis()->GetXmax() * 1.1);
    g->GetHistogram()->SetMinimum(g->GetYaxis()->GetXmin() * 0.9);
    g->Draw("AP");
    TLatex *t = new TLatex();
    t->SetTextSize(0.035);
    for (size_t i = 0; i < plotinfo.size(); ++i)
    {
        t->DrawLatexNDC(gPad->GetLeftMargin() + 0.05, 1 - gPad->GetTopMargin() - 0.08 - i * 0.045, plotinfo[i].c_str());
    }
    // covariance and correlation factors
    t->DrawLatexNDC(gPad->GetLeftMargin() + 0.05, 1 - gPad->GetTopMargin() - 0.08 - plotinfo.size() * 0.045, Form("Covariance: %.3g", g->GetCovariance()));
    t->DrawLatexNDC(gPad->GetLeftMargin() + 0.05, 1 - gPad->GetTopMargin() - 0.08 - (plotinfo.size() + 1) * 0.045, Form("Correlation: %.3g", g->GetCorrelationFactor()));
    c->SaveAs(Form("%s.pdf", plotname.c_str()));
    c->SaveAs(Form("%s.png", plotname.c_str()));
}

void drawTMultiGraph(std::vector<TGraph *> v_graph, std::string xtitle, std::string ytitle, std::string plotname)
{
    TCanvas *c = new TCanvas("c", "c", 800, 700);
    c->cd();
    TMultiGraph *mg = new TMultiGraph();
    for (size_t i = 0; i < v_graph.size(); i++)
    {
        v_graph[i]->SetMarkerColorAlpha(TColor::GetColor(v_color[i].c_str()), v_alpha[i]);
        v_graph[i]->SetMarkerStyle(v_marker[i]);
        v_graph[i]->SetMarkerSize(0.8);
        mg->Add(v_graph[i]);
    }
    mg->Draw("AP");
    mg->GetXaxis()->SetTitle(xtitle.c_str());
    mg->GetYaxis()->SetTitle(ytitle.c_str());
    mg->GetYaxis()->SetTitleOffset(1.5);
    gPad->Modified();
    float axismax = (mg->GetXaxis()->GetXmax() > mg->GetYaxis()->GetXmax()) ? mg->GetXaxis()->GetXmax() : mg->GetYaxis()->GetXmax();
    mg->GetXaxis()->SetLimits(0, axismax * 1.1);
    mg->GetHistogram()->SetMaximum(axismax * 1.1);
    mg->GetHistogram()->SetMinimum(0);
    TLegend *leg = new TLegend(gPad->GetLeftMargin() + 0.05, 1 - gPad->GetTopMargin() - 0.05, gPad->GetLeftMargin() + 0.25, 1 - gPad->GetTopMargin() - 0.05 - 0.05 * v_graph.size());
    leg->SetTextAlign(kHAlignLeft + kVAlignTop);
    leg->SetTextSize(0.03);
    for (size_t i = 0; i < v_graph.size(); i++)
    {
        leg->AddEntry(v_graph[i], Form("%s (corr. coefficient = %.3g)", v_unclabel[i].c_str(), v_graph[i]->GetCorrelationFactor()), "p");
    }
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->Draw();
    gPad->Modified();
    gPad->Update();
    c->SaveAs(Form("%s.pdf", plotname.c_str()));
    c->SaveAs(Form("%s.png", plotname.c_str()));
}

// calculate the correlation in the systematic uncertainties between the PHOBOS and CMS approaches
void sysunccorr()
{
    system("mkdir -p ./systematics/correlation/");

    std::vector<TGraph *> v_g_unc;

    for (size_t j = 0; j < v_phobos_histname.size(); j++)
    {
        std::vector<double> v_unc_phobos, v_unc_cms;
        v_unc_phobos.clear();
        v_unc_cms.clear();

        for (size_t i = 0; i < v_centralitybin.size() - 1; i++)
        {
            std::string centstr = Form("Centrality%dto%d", (int)v_centralitybin[i], (int)v_centralitybin[i + 1]);
            std::string fphobos = Form("/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Jan172025/Run4/EvtVtxZ/FinalResult/completed/vtxZ_-10_10cm_MBin%d/Final_Mbin%d_00054280/Final_Mbin%d_00054280.root", GetMbinNum(centstr), GetMbinNum(centstr), GetMbinNum(centstr));
            std::string fcms = Form("./systematics/Centrality%dto%d_Zvtxm10p0to10p0_noasel/finalhists_systematics_Centrality%dto%d_Zvtxm10p0to10p0_noasel.root", (int)v_centralitybin[i], (int)v_centralitybin[i + 1], (int)v_centralitybin[i], (int)v_centralitybin[i + 1]);

            TH1 *h_phobos = gethist(fphobos, v_phobos_histname[j]);
            TH1 *h_cms = gethist(fcms, v_cms_histname[j]);

            // now loop over the bins with non-zero content
            for (int k = 1; k <= h_phobos->GetNbinsX(); k++)
            {
                double unc_phobos = h_phobos->GetBinContent(k);
                double unc_cms = h_cms->GetBinContent(k);
                if (unc_cms == 0)
                    continue;
                v_unc_phobos.push_back(unc_phobos * 100);
                v_unc_cms.push_back(unc_cms * 100);
            }
        }

        TGraph *g_unc = new TGraph(v_unc_phobos.size(), v_unc_phobos.data(), v_unc_cms.data());
        v_corr.push_back(g_unc->GetCorrelationFactor());
        v_g_unc.push_back(g_unc);
        drawTGraph(g_unc, "PHOBOS approach [%]", "CMS approach [%]", {"Uncertainty: " + v_unclabel[j]}, Form("./systematics/correlation/correlation_%s", v_plotname[j].c_str()));
    }

    drawTMultiGraph(v_g_unc, "PHOBOS approach [%]", "CMS approach [%]", "./systematics/correlation/correlation_all");
}

// combination method according to the method in CMS HIN-10-001
// return <the combined value, uncorrelated uncertainty, correlated uncertainty, total uncertainty>
std::tuple<double, double, double, double> combined_measurement(double X_phobos,                //
                                                                double X_cms,                   //
                                                                std::vector<double> unc_phobos, //
                                                                std::vector<double> unc_cms,    //
                                                                std::vector<double> corr        //
)
{
    int num_sources = unc_phobos.size();
    double unc_corr = 0., unc_uncorr = 0., unc_total = 0.;
    for (int i = 0; i < num_sources; i++)
    {
        std::cout << "  Uncertainty - " << v_unclabel[i] << " (phobos, cms, correlation): " << unc_phobos[i] << ", " << unc_cms[i] << ", " << corr[i] << std::endl;
        if (corr[i] < 0.1) // treated as uncorrelated uncertainty -> quadrature sum
        {
            unc_uncorr += unc_phobos[i] * unc_phobos[i] + unc_cms[i] * unc_cms[i];
        }
        else // treated as fully correlated uncertainty -> sum quadrature
        {
            unc_corr += std::pow(unc_phobos[i] + unc_cms[i], 2);
        }
    }
    unc_corr = std::sqrt(unc_corr) / 2.;
    unc_uncorr = std::sqrt(unc_uncorr) / 2.;
    unc_total = std::sqrt(unc_corr * unc_corr + unc_uncorr * unc_uncorr);

    // combined result is the arithmetic mean of the two measurements
    double X_combined = (X_phobos + X_cms) / 2.;
    return std::make_tuple(X_combined, unc_uncorr, unc_corr, unc_total);
}

void combine_CMSHIN10001()
{
    setbinattr();

    // First, calculate the correlation between the systematic uncertainties in the PHOBOS and CMS approaches
    sysunccorr();

    system("mkdir -p ./systematics/combined/");

    std::cout << "Correlation in the uncertainty between PHOBOS and CMS approaches:" << std::endl;
    for (size_t i = 0; i < v_corr.size(); i++)
    {
        std::cout << v_unclabel[i] << ": " << v_corr[i] << std::endl;
    }

    // start the loop over the centrality bins
    for (size_t i = 0; i < v_centralitybin.size() - 1; i++)
    // for (size_t i = 0; i < 1; i++)
    {
        std::cout << "Combine the PHOBOS and CMS measurements for centrality bin " << v_centralitybin[i] << " to " << v_centralitybin[i + 1] << std::endl;

        std::string centstr = Form("Centrality%dto%d", (int)v_centralitybin[i], (int)v_centralitybin[i + 1]);
        std::string fphobos = Form("/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Jan172025/Run4/EvtVtxZ/FinalResult/completed/vtxZ_-10_10cm_MBin%d/Final_Mbin%d_00054280/Final_Mbin%d_00054280.root", GetMbinNum(centstr), GetMbinNum(centstr), GetMbinNum(centstr));
        std::string fcms = Form("./systematics/Centrality%dto%d_Zvtxm10p0to10p0_noasel/finalhists_systematics_Centrality%dto%d_Zvtxm10p0to10p0_noasel.root", (int)v_centralitybin[i], (int)v_centralitybin[i + 1], (int)v_centralitybin[i], (int)v_centralitybin[i + 1]);

        // histograms for the central values
        TH1 *h_phobos_X = gethist(fphobos, "h1D_dNdEta_reco");
        TH1 *h_cms_X = gethist(fcms, "hM_final");
        // histograms for the total uncertainties, for checking
        TH1 *h_phobos_totlunc = gethist(fphobos, "h1D_error_Final");
        TH1 *h_cms_totlunc = gethist(fcms, "hM_TotalRelUnc");
        // histograms for the uncertainties
        std::vector<TH1 *> v_h_phobos_unc, v_h_cms_unc;
        v_h_phobos_unc.clear();
        v_h_cms_unc.clear();
        for (size_t j = 0; j < v_phobos_histname.size(); j++)
        {
            v_h_phobos_unc.push_back(gethist(fphobos, v_phobos_histname[j]));
            v_h_cms_unc.push_back(gethist(fcms, v_cms_histname[j]));
        }

        // histogram for the combined measurement
        TH1 *h_combined = new TH1D(Form("h_combined_%s", centstr.c_str()), Form("Combined dN/d#eta for %s", centstr.c_str()), h_phobos_X->GetNbinsX(), h_phobos_X->GetXaxis()->GetXmin(), h_phobos_X->GetXaxis()->GetXmax());
        // TGraphAsymmErrors for the combined measurement
        // TGraphAsymmErrors *g_combined = new TGraphAsymmErrors(h_phobos_X->GetNbinsX());

        std::vector<double> v_combined_x, v_combined_y, v_combined_xerr, v_combined_yerrup, v_combined_yerrdown;
        // loop over the bins
        for (int k = 1; k <= h_phobos_X->GetNbinsX(); k++)
        {
            double X_phobos = h_phobos_X->GetBinContent(k);
            double X_cms = h_cms_X->GetBinContent(k);
            for (int b = 0; b < h_cms_X->GetNbinsX(); b++)
            {
                if (h_cms_X->GetBinContent(b) < 0.5)
                {
                    h_cms_X->SetBinContent(b, 0);
                    h_cms_X->SetBinError(b, 0);
                }
            }

            std::vector<double> unc_phobos, unc_cms;
            unc_phobos.clear();
            unc_cms.clear();
            for (size_t j = 0; j < v_phobos_histname.size(); j++)
            {
                unc_phobos.push_back(v_h_phobos_unc[j]->GetBinContent(k) * X_phobos);
                unc_cms.push_back(v_h_cms_unc[j]->GetBinContent(k) * X_cms);
            }

            std::cout << "----- Bin " << k << ", eta = " << h_phobos_X->GetBinCenter(k) << " -----" << std::endl;
            std::cout << "PHOBOS measurement: " << X_phobos << " +/- " << h_phobos_totlunc->GetBinContent(k) * X_phobos << std::endl;
            std::cout << "CMS measurement: " << X_cms << " +/- " << h_cms_totlunc->GetBinContent(k) * X_cms << std::endl;
            std::tuple<double, double, double, double> combined = combined_measurement(X_phobos, X_cms, unc_phobos, unc_cms, v_corr);

            // print out the details
            std::cout << "Combined measurement: " << std::get<0>(combined) << " +/- " << std::get<3>(combined) << std::endl;
            std::cout << "---------------------------------" << std::endl;

            // assign the combined measurement to the histogram
            v_combined_x.push_back(h_phobos_X->GetBinCenter(k));
            v_combined_y.push_back(std::get<0>(combined));
            v_combined_xerr.push_back(h_phobos_X->GetBinWidth(k) * 0.49);
            v_combined_yerrup.push_back(std::get<3>(combined));
            v_combined_yerrdown.push_back(std::get<3>(combined));
        }

        // set up the TGraphAssymErrors for the combined measurement
        TGraphAsymmErrors *g_combined = new TGraphAsymmErrors(v_combined_x.size(), v_combined_x.data(), v_combined_y.data(), v_combined_xerr.data(), v_combined_xerr.data(), v_combined_yerrdown.data(), v_combined_yerrup.data());

        // draw the combined measurement
        TCanvas *c = new TCanvas("c", "c", 800, 700);
        c->cd();
        gPad->SetRightMargin(0.07);
        gPad->SetTopMargin(0.07);
        gPad->SetLeftMargin(0.17);
        h_phobos_X->SetLineColor(kRed);
        h_phobos_X->SetLineWidth(0);
        h_phobos_X->SetMarkerColor(kRed);
        h_phobos_X->SetMarkerStyle(20);
        h_phobos_X->SetMarkerSize(1);
        h_phobos_X->SetFillColorAlpha(kRed, 0.1);
        h_phobos_X->GetXaxis()->SetTitle("#eta");
        h_phobos_X->GetYaxis()->SetTitle("dN_{ch}/d#eta");
        h_phobos_X->GetYaxis()->SetTitleOffset(1.7);
        h_phobos_X->GetYaxis()->SetRangeUser(h_cms_X->GetMinimum(0) * 0.7, h_cms_X->GetMaximum() * 1.3);
        h_phobos_X->Draw("E2P");
        h_cms_X->SetLineColor(kBlue);
        h_cms_X->SetLineWidth(0);
        h_cms_X->SetMarkerColor(kBlue);
        h_cms_X->SetMarkerStyle(21);
        h_cms_X->SetMarkerSize(1);
        h_cms_X->SetFillColorAlpha(kBlue, 0.1);
        h_cms_X->Draw("E2P SAME");
        g_combined->SetLineColor(kBlack);
        g_combined->SetLineWidth(1);
        g_combined->SetMarkerColor(kBlack);
        g_combined->SetMarkerStyle(33);
        g_combined->SetMarkerSize(1);
        g_combined->SetFillColorAlpha(kBlack, 0.4);
        g_combined->Draw("5E P SAME");
        TLegend *leg = new TLegend(gPad->GetLeftMargin() + 0.05, 1 - gPad->GetTopMargin() - 0.05, gPad->GetLeftMargin() + 0.25, 1 - gPad->GetTopMargin() - 0.25);
        leg->SetHeader(Form("Centrality %d-%d%%", (int)v_centralitybin[i], (int)v_centralitybin[i + 1]));
        leg->SetTextAlign(kHAlignLeft + kVAlignCenter);
        leg->SetTextSize(0.03);
        leg->AddEntry(h_phobos_X, "PHOBOS approach", "pf");
        leg->AddEntry(h_cms_X, "CMS approach", "pf");
        leg->AddEntry(g_combined, "Combined (Same method as JHEP 08 (2011) 141) ", "pf");
        leg->SetBorderSize(0);
        leg->SetFillStyle(0);
        leg->Draw();
        gPad->Modified();
        gPad->Update();
        c->SaveAs(Form("./systematics/combined/combined_%s_CMSHIN10001.pdf", centstr.c_str()));
        c->SaveAs(Form("./systematics/combined/combined_%s_CMSHIN10001.png", centstr.c_str()));
    }
}