#include "./util_combine.h"

std::vector<float> v_centralitybin = {0, 3, 6, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70};

std::vector<std::string> v_phobos_histname = {"h1D_error_DeltaPhi", "h1D_error_ClusAdc", "h1D_error_ClusPhiSize", "h1D_error_Run_segmentation", "h1D_error_statistic", "h1D_error_GeoOffset"};
std::vector<std::string> v_cms_histname = {"hM_maxreldiff_dRcut", "hM_maxreldiff_clusAdcCut", "hM_maxreldiff_clusPhiSizeCut", "hM_maxreldiff_segment", "hM_statunc_nominal", "hM_maxreldiff_GeoOffset"};
std::vector<std::string> v_unclabel = {"Tracklet reconstruction selection", "Cluster ADC cut", "Cluster #phi-size cut", "Run segments", "Statistical uncertainty in corrections", "Geometric offset"};
std::vector<std::string> v_plotname = {"dRcut", "clusAdcCut", "clusPhiSizeCut", "segment", "statUnc", "GeoOffset"};
std::vector<std::string> v_color = {"#f2777a", "#6699cc", "#9999cc", "#99cc99", "#888888", "#ffcc66"};
std::vector<float> v_alpha = {0.7, 0.7, 0.6, 0.5, 0.8, 0.7};
std::vector<int> v_marker = {20, 21, 33, 34, 47, 45};
std::vector<float> v_corr = {}; // the same order as v_phobos_histname/v_cms_histname
int Nbins_dNdeta = 27;
float xbinlow_dNdeta = -2.7;
float xbinhigh_dNdeta = 2.7;

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

void sysunccorr()
{
    system("mkdir -p ./systematics/correlation/");

    std::vector<TGraph *> v_g_unc;

    for (size_t j = 0; j < v_phobos_histname.size(); j++)
    {
        std::vector<float> v_unc_phobos, v_unc_cms;
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
                float unc_phobos = h_phobos->GetBinContent(k);
                float unc_cms = h_cms->GetBinContent(k);
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

std::tuple<float, float> combined_measurement(float X_phobos,                //
                                              float X_cms,                   //
                                              std::vector<float> unc_phobos, //
                                              std::vector<float> unc_cms,    //
                                              std::vector<float> corr        //
)
{
    int num_sources = unc_phobos.size();
    float sigma1_sq = 0., sigma2_sq = 0., covariance = 0.;
    for (int i = 0; i < num_sources; i++)
    {
        std::cout << "  Uncertainty - " << v_unclabel[i] << " (phobos, cms, correlation): " << unc_phobos[i] << ", " << unc_cms[i] << ", " << corr[i] << std::endl;
        sigma1_sq += unc_phobos[i] * unc_phobos[i];
        sigma2_sq += unc_cms[i] * unc_cms[i];
        covariance += unc_phobos[i] * unc_cms[i] * corr[i];
    }
    // Covariance matrix V = [[V_11, V_12], [V_21, V_22]] = [[sigma1_sq, covariance], [covariance, sigma2_sq]]
    float V_11 = sigma1_sq;
    float V_12 = covariance; // = V_21
    float V_22 = sigma2_sq;
    // weights
    float denominator = V_11 + V_22 - 2 * covariance;
    float w1 = (V_22 - covariance) / denominator;
    float w2 = (V_11 - covariance) / denominator;

    std::cout << "  V_11 (sigma1_sq) = " << V_11 << ", V_12 (covariance) = " << V_12 << ", V_22 (sigma2_sq) = " << V_22 << std::endl                                                                          //
              << "  denominator (V_11 + V_22 - 2 * covariance) = " << denominator << ", w1 ((V_22 - covariance) / denominator) = " << w1 << ", w2 ((V_11 - covariance) / denominator) = " << w2 << std::endl; //
    float X_combined = (X_phobos * sigma2_sq + X_cms * sigma1_sq - covariance * (X_phobos + X_cms)) / denominator;
    float sigma_combined = std::sqrt((V_11 * V_22 - V_12 * V_12) / (V_11 + V_22 - 2 * V_12));

    return std::make_tuple(X_combined, sigma_combined);
}

void combine_leastsquare()
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

        // loop over the bins
        for (int k = 1; k <= h_phobos_X->GetNbinsX(); k++)
        {
            float X_phobos = h_phobos_X->GetBinContent(k);
            float X_cms = h_cms_X->GetBinContent(k);
            for (int b = 0; b < h_cms_X->GetNbinsX(); b++)
            {
                if (h_cms_X->GetBinContent(b) < 0.5)
                {
                    h_cms_X->SetBinContent(b, 0);
                    h_cms_X->SetBinError(b, 0);
                }
            }

            std::vector<float> unc_phobos, unc_cms;
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
            std::tuple<float, float> combined = combined_measurement(X_phobos, X_cms, unc_phobos, unc_cms, v_corr);
            h_combined->SetBinContent(k, std::get<0>(combined));
            h_combined->SetBinError(k, std::get<1>(combined));

            // print out the details
            std::cout << "Combined measurement: " << std::get<0>(combined) << " +/- " << std::get<1>(combined) << std::endl;
            std::cout << "---------------------------------" << std::endl;
        }

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
        h_combined->SetLineColor(kBlack);
        h_combined->SetLineWidth(1);
        h_combined->SetMarkerColor(kBlack);
        h_combined->SetMarkerStyle(33);
        h_combined->SetMarkerSize(1);
        h_combined->SetFillColorAlpha(kBlack, 0.4);
        h_combined->Draw("E2P SAME");
        TLegend *leg = new TLegend(gPad->GetLeftMargin() + 0.05, 1 - gPad->GetTopMargin() - 0.05, gPad->GetLeftMargin() + 0.25, 1 - gPad->GetTopMargin() - 0.25);
        leg->SetHeader(Form("Centrality %d-%d%%", (int)v_centralitybin[i], (int)v_centralitybin[i + 1]));
        leg->SetTextAlign(kHAlignLeft + kVAlignCenter);
        leg->SetTextSize(0.03);
        leg->AddEntry(h_phobos_X, "PHOBOS approach", "pf");
        leg->AddEntry(h_cms_X, "CMS approach", "pf");
        leg->AddEntry(h_combined, "Combined measurement", "pf");
        leg->SetBorderSize(0);
        leg->SetFillStyle(0);
        leg->Draw();
        gPad->Modified();
        gPad->Update();
        c->SaveAs(Form("./systematics/combined/combined_%s_covmatrix.pdf", centstr.c_str()));
        c->SaveAs(Form("./systematics/combined/combined_%s_covmatrix.png", centstr.c_str()));
    }
}