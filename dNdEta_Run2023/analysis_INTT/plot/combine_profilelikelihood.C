#include "./plotutil.h"
#include "./util_combine.h"

std::vector<float> v_centralitybin = {0, 3, 6, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70};

std::vector<std::string> v_phobos_histname = {"h1D_error_DeltaPhi", "h1D_error_ClusAdc", "h1D_error_ClusPhiSize", "h1D_error_Run_segmentation", "h1D_error_statistic", "h1D_error_GeoOffset"};
std::vector<std::string> v_cms_histname = {"hM_maxreldiff_dRcut", "hM_maxreldiff_clusAdcCut", "hM_maxreldiff_clusPhiSizeCut", "hM_maxreldiff_segment", "hM_statunc_nominal", "hM_maxreldiff_GeoOffset"};
std::vector<std::string> v_unclabel = {"Tracklet reconstruction selection", "Cluster ADC cut", "Cluster #phi-size cut", "Run segments", "Statistical uncertainty in corrections", "Geometric offset"};
std::vector<std::string> v_plotname = {"dRcut", "clusAdcCut", "clusPhiSizeCut", "segment", "statUnc", "GeoOffset"};
std::vector<std::string> v_color = {"#f2777a", "#6699cc", "#9999cc", "#99cc99", "#888888", "#ffcc66"};
std::vector<float> v_alpha = {0.7, 0.7, 0.6, 0.5, 0.8, 0.7};
std::vector<int> v_marker = {20, 21, 33, 34, 47, 45};
std::vector<double> v_corr = {}; // the same order as v_phobos_histname/v_cms_histname
int Nbins_dNdeta = 27;
float xbinlow_dNdeta = -2.7;
float xbinhigh_dNdeta = 2.7;
double desiredCL = 0.99;
double step = 1E-5;        // Fine resolution for scanning the profile likelihood
bool set_phobosunc = true; // for testing

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

//------------------------------------------------------
// For a chi-square distribution with 1 degree of freedom,
// the cumulative distribution function can be expressed as:
//    F(x;1) = erf( sqrt(x/2) )
// where erf is the error function provided in <cmath>.
//------------------------------------------------------
double chi_square_cdf_1dof(double x) { return erf(sqrt(x / 2.0)); }

//------------------------------------------------------
// Inverse chi-square function for 1 degree of freedom using binary search.
// For a given probability p (0 < p < 1), it finds x such that
//    chi_square_cdf_1dof(x) = p.
// 'tol' sets the tolerance for convergence.
//------------------------------------------------------
double chi_square_inv_1dof(double p, double tol = 1e-6)
{
    double low = 0.0;
    double high = 10.0;
    // Expand the upper bound until the CDF is at least p.
    while (chi_square_cdf_1dof(high) < p)
    {
        high *= 2;
    }
    // Binary search for the value x such that CDF(x) ~ p.
    while (high - low > tol)
    {
        double mid = (low + high) / 2.0;
        double cdf_mid = chi_square_cdf_1dof(mid);
        if (cdf_mid < p)
            low = mid;
        else
            high = mid;
    }
    return (low + high) / 2.0;
}

//------------------------------------------------------
// get_threshold_for_confidence()
// For a given desired confidence level (e.g., 0.95 for 95%),
// this function returns the corresponding chi-square threshold
// for one degree of freedom using the inverse CDF function.
//------------------------------------------------------
double get_threshold_for_confidence(double desiredCL)
{
    if (desiredCL <= 0.0 || desiredCL >= 1.0)
    {
        cout << "Invalid confidence level. Please enter a value between 0 and 1.\n";
        return -1;
    }
    return chi_square_inv_1dof(desiredCL);
}

//------------------------------------------------------
// Gaussian likelihood function.
// Computes the likelihood of observing X given a predicted value theta
// and a Gaussian uncertainty sigma.
//------------------------------------------------------
double gaussian_likelihood(double X, double theta, double sigma)
{
    double exponent = -0.5 * pow((X - theta) / sigma, 2);
    return exp(exponent) / (sqrt(2 * M_PI) * sigma);
}

//------------------------------------------------------
// Profile likelihood function (systematics only).
// This function loops over the available systematic uncertainty sources
// (each element in the vectors represents one source) and computes the likelihood
// for a given theta. In this simplified example, we take the maximum likelihood
// over the systematic configurations.
// Note: There is no statistical uncertainty in this version.
//------------------------------------------------------
double profile_likelihood(double theta,                     //
                          double X1,                        //
                          double X2,                        //
                          const vector<double> &sigma1_sys, //
                          const vector<double> &sigma2_sys, //
                          const vector<double> &correlation //
)
{
    // Combine systematic uncertainties in quadrature.
    double V11 = 0.0, V22 = 0.0, V12 = 0.0;
    int n = sigma1_sys.size();
    for (int i = 0; i < n; i++)
    {
        V11 += sigma1_sys[i] * sigma1_sys[i];
        V22 += sigma2_sys[i] * sigma2_sys[i];
        V12 += correlation[i] * sigma1_sys[i] * sigma2_sys[i];
    }
    // Form the covariance matrix:
    // V = [ V11, V12 ]
    //     [ V12, V22 ]
    double detV = V11 * V22 - V12 * V12;
    if (detV <= 0)
    {
        // In case of numerical issues, return a very small likelihood.
        return 1e-300;
    }
    // Inverse covariance matrix elements:
    double invV11 = V22 / detV;
    double invV12 = -V12 / detV;
    double invV22 = V11 / detV;

    // The difference vector: d = [X1 - theta, X2 - theta]^T.
    double d1 = X1 - theta;
    double d2 = X2 - theta;

    // Exponent term: -0.5 * d^T * V^-1 * d.
    double exponent = -0.5 * (d1 * d1 * invV11 + 2.0 * d1 * d2 * invV12 + d2 * d2 * invV22);

    // Joint likelihood:
    double L = exp(exponent) / (2.0 * M_PI * sqrt(detV));
    return L;
}

//------------------------------------------------------
// Find the best estimate of theta by scanning a range.
// Returns the value of theta that maximizes the profile likelihood.
//------------------------------------------------------
double find_best_estimate(double X1,                         //
                          double X2,                         //
                          const vector<double> &sigma1_sys,  //
                          const vector<double> &sigma2_sys,  //
                          const vector<double> &correlation, //
                          double theta_min,                  //
                          double theta_max,                  //
                          double step                        //
)
{
    double best_theta = X1; // initial guess
    double max_likelihood = -numeric_limits<double>::infinity();

    for (double theta = theta_min; theta <= theta_max; theta += step)
    {
        double L = profile_likelihood(theta, X1, X2, sigma1_sys, sigma2_sys, correlation);
        if (L > max_likelihood)
        {
            max_likelihood = L;
            best_theta = theta;
        }
    }
    return best_theta;
}

//------------------------------------------------------
// Compute the confidence interval using the likelihood ratio method.
// For a given threshold (obtained from the desired confidence level),
// this function scans from the best estimate toward lower and higher theta values
// to find where the likelihood ratio -2ln(L/L_max) crosses the threshold.
// Linear interpolation is used for better accuracy.
//------------------------------------------------------
pair<double, double> compute_confidence_interval(double X1,                         //
                                                 double X2,                         //
                                                 const vector<double> &sigma1_sys,  //
                                                 const vector<double> &sigma2_sys,  //
                                                 const vector<double> &correlation, //
                                                 double best_theta,                 //
                                                 double L_max,                      //
                                                 double theta_min,                  //
                                                 double theta_max,                  //
                                                 double step,                       //
                                                 double threshold                   //
)
{
    double lower_bound = best_theta;
    double upper_bound = best_theta;
    bool found_lower = false, found_upper = false;

    // Scan downward from best_theta to find the lower bound.
    double prev_theta = best_theta;
    double prev_delta = 0.0;
    for (double theta = best_theta; theta >= theta_min; theta -= step)
    {
        double L = profile_likelihood(theta, X1, X2, sigma1_sys, sigma2_sys, correlation);
        double delta = -2.0 * log(L / L_max);
        if (theta == best_theta)
        {
            prev_delta = delta;
            prev_theta = theta;
            continue;
        }
        if (delta >= threshold && prev_delta < threshold)
        {
            double t = (threshold - prev_delta) / (delta - prev_delta);
            lower_bound = prev_theta + t * (theta - prev_theta);
            found_lower = true;
            break;
        }
        prev_delta = delta;
        prev_theta = theta;
    }

    // Scan upward from best_theta to find the upper bound.
    prev_theta = best_theta;
    prev_delta = 0.0;
    for (double theta = best_theta; theta <= theta_max; theta += step)
    {
        double L = profile_likelihood(theta, X1, X2, sigma1_sys, sigma2_sys, correlation);
        double delta = -2.0 * log(L / L_max);
        if (theta == best_theta)
        {
            prev_delta = delta;
            prev_theta = theta;
            continue;
        }
        if (delta >= threshold && prev_delta < threshold)
        {
            double t = (threshold - prev_delta) / (delta - prev_delta);
            upper_bound = prev_theta + t * (theta - prev_theta);
            found_upper = true;
            break;
        }
        prev_delta = delta;
        prev_theta = theta;
    }

    if (!found_lower)
        lower_bound = theta_min;
    if (!found_upper)
        upper_bound = theta_max;

    return make_pair(lower_bound, upper_bound);
}

void combine_profilelikelihood()
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
        for (int b = 0; b < h_cms_X->GetNbinsX(); b++)
        {
            if (h_cms_X->GetBinContent(b) < 0.5)
            {
                h_cms_X->SetBinContent(b, 0);
                h_cms_X->SetBinError(b, 0);
            }
            // set the uncertainty of the CMS measurement to be the same as phobos
            if (set_phobosunc)
                h_cms_X->SetBinError(b, h_phobos_totlunc->GetBinContent(b) * h_cms_X->GetBinContent(b));
        }

        // histograms for the uncertainties
        std::vector<TH1 *> v_h_phobos_unc, v_h_cms_unc;
        v_h_phobos_unc.clear();
        v_h_cms_unc.clear();
        for (size_t j = 0; j < v_phobos_histname.size(); j++)
        {
            v_h_phobos_unc.push_back(gethist(fphobos, v_phobos_histname[j]));
            v_h_cms_unc.push_back(gethist(fcms, v_cms_histname[j]));
        }

        std::vector<double> v_combined_x, v_combined_y, v_combined_xerr, v_combined_yerrup, v_combined_yerrdown;
        // loop over the bins
        for (int k = 1; k <= h_phobos_X->GetNbinsX(); k++)
        {
            double X_phobos = h_phobos_X->GetBinContent(k);
            double X_cms = h_cms_X->GetBinContent(k);
            if (X_phobos < 5E-1 || X_cms < 5E-1)
                continue;

            std::vector<double> unc_phobos, unc_cms;
            unc_phobos.clear();
            unc_cms.clear();
            for (size_t j = 0; j < v_phobos_histname.size(); j++)
            {
                if (set_phobosunc)
                    v_h_cms_unc[j]->SetBinContent(k, v_h_phobos_unc[j]->GetBinContent(k));

                unc_phobos.push_back(v_h_phobos_unc[j]->GetBinContent(k) * X_phobos);
                unc_cms.push_back(v_h_cms_unc[j]->GetBinContent(k) * X_cms);
            }

            std::cout << "----- Bin " << k << ", eta = " << h_phobos_X->GetBinCenter(k) << " -----" << std::endl;
            std::cout << "PHOBOS measurement: " << X_phobos << " +/- " << h_phobos_totlunc->GetBinContent(k) * X_phobos << std::endl;
            std::cout << "CMS measurement: " << X_cms << " +/- " << h_cms_totlunc->GetBinContent(k) * X_cms << std::endl;
            double threshold = get_threshold_for_confidence(desiredCL);
            if (threshold < 0)
            {
                std::cout << "Invalid confidence level. Please enter a value between 0 and 1." << std::endl;
                exit(1);
            }
            // Define the scanning range and step size for theta (parameter of interest, POI)
            double theta_min = min(X_phobos, X_cms) - 1.;
            double theta_max = max(X_phobos, X_cms) + 1.;

            // Find the best estimate by maximizing the profile likelihood
            double best_theta = find_best_estimate(X_phobos, X_cms, unc_phobos, unc_cms, v_corr, theta_min, theta_max, step);

            // Compute the maximum likelihood at the best estimate
            double L_max = profile_likelihood(best_theta, X_phobos, X_cms, unc_phobos, unc_cms, v_corr);

            // Compute the confidence interval using the likelihood ratio method
            pair<double, double> conf_interval = compute_confidence_interval(X_phobos, X_cms, unc_phobos, unc_cms, v_corr, best_theta, L_max, theta_min, theta_max, step, threshold);

            cout << "Combined measurement, best estimate = " << best_theta << " with confidence interval (" << desiredCL * 100 << "%): [" << conf_interval.first << ", " << conf_interval.second << "]" << endl;

            // assign the combined measurement to the histogram
            v_combined_x.push_back(h_phobos_X->GetBinCenter(k));
            v_combined_y.push_back(best_theta);
            v_combined_xerr.push_back(h_phobos_X->GetBinWidth(k) * 0.49);
            v_combined_yerrup.push_back(conf_interval.second - best_theta);
            v_combined_yerrdown.push_back(best_theta - conf_interval.first);
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
        leg->AddEntry(g_combined, Form("Combined (Profile likelihood, C.L=%.3g%%) ", desiredCL * 100), "pf");
        leg->SetBorderSize(0);
        leg->SetFillStyle(0);
        leg->Draw();
        gPad->Modified();
        gPad->Update();
        if (set_phobosunc)
        {
            c->SaveAs(Form("./systematics/combined/combined_%s_profilelikelihood_setphobosunc.pdf", centstr.c_str()));
            c->SaveAs(Form("./systematics/combined/combined_%s_profilelikelihood_setphobosunc.png", centstr.c_str()));
        }
        else
        {
            c->SaveAs(Form("./systematics/combined/combined_%s_profilelikelihood.pdf", centstr.c_str()));
            c->SaveAs(Form("./systematics/combined/combined_%s_profilelikelihood.png", centstr.c_str()));
        }
    }
}