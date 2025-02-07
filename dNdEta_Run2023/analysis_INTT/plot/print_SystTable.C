#include "./plotutil.h"

std::vector<float> centralitybins = {0, 3, 6, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70};

std::vector<float> getuncertainty(std::string filename, std::string histname)
{
    TFile *f = new TFile(filename.c_str(), "READ");
    TH1F *h = (TH1F *)f->Get(histname.c_str());
    h->SetDirectory(0);
    std::vector<float> v_uncertainty;
    v_uncertainty.clear();
    for (int i = 1; i <= h->GetNbinsX(); i++)
    {
        if (h->GetBinContent(i) != 0)
            v_uncertainty.push_back(h->GetBinContent(i));
    }
    f->Close();

    return v_uncertainty;
}

void print_SystTable()
{
    // loop over all centrality bins
    std::vector<float> v_unc_statunccorr_max_allcent, v_unc_dRcut_max_allcent, v_unc_clusAdcCut_max_allcent, v_unc_clusSizeCut_max_allcent, v_unc_segment_max_allcent, v_unc_total_max_allcent;
    std::vector<float> v_unc_statunccorr_min_allcent, v_unc_dRcut_min_allcent, v_unc_clusAdcCut_min_allcent, v_unc_clusSizeCut_min_allcent, v_unc_segment_min_allcent, v_unc_total_min_allcent;
    for (int i = 0; i < centralitybins.size() - 1; i++)
    {
        std::cout << "Centrality " << (int)centralitybins[i] << "-" << (int)centralitybins[i + 1] << "%" << std::endl;
        std::string fname = Form("./systematics/Centrality%dto%d_Zvtxm10p0to10p0_noasel/finalhists_systematics_Centrality%dto%d_Zvtxm10p0to10p0_noasel.root", (int)centralitybins[i], (int)centralitybins[i + 1], (int)centralitybins[i], (int)centralitybins[i + 1]);
        std::vector<float> v_unc_statunccorr = getuncertainty(fname, "hM_statunc_nominal");
        std::vector<float> v_unc_dRcut = getuncertainty(fname, "hM_maxreldiff_dRcut");
        std::vector<float> v_unc_clusAdcCut = getuncertainty(fname, "hM_maxreldiff_clusAdcCut");
        std::vector<float> v_unc_clusSizeCut = getuncertainty(fname, "hM_maxreldiff_clusPhiSizeCut");
        std::vector<float> v_unc_segment = getuncertainty(fname, "hM_maxreldiff_segment");
        std::vector<float> v_unc_total = getuncertainty(fname, "hM_TotalRelUnc");

        float unc_statunccorr_max = *std::max_element(v_unc_statunccorr.begin(), v_unc_statunccorr.end());
        float unc_dRcut_max = *std::max_element(v_unc_dRcut.begin(), v_unc_dRcut.end());
        float unc_clusAdcCut_max = *std::max_element(v_unc_clusAdcCut.begin(), v_unc_clusAdcCut.end());
        float unc_clusSizeCut_max = *std::max_element(v_unc_clusSizeCut.begin(), v_unc_clusSizeCut.end());
        float unc_segment_max = *std::max_element(v_unc_segment.begin(), v_unc_segment.end());
        float unc_total_max = *std::max_element(v_unc_total.begin(), v_unc_total.end());
        // the minimum of non-zero values
        float unc_statunccorr_min = *std::min_element(v_unc_statunccorr.begin(), v_unc_statunccorr.end());
        float unc_dRcut_min = *std::min_element(v_unc_dRcut.begin(), v_unc_dRcut.end());
        float unc_clusAdcCut_min = *std::min_element(v_unc_clusAdcCut.begin(), v_unc_clusAdcCut.end());
        float unc_clusSizeCut_min = *std::min_element(v_unc_clusSizeCut.begin(), v_unc_clusSizeCut.end());
        float unc_segment_min = *std::min_element(v_unc_segment.begin(), v_unc_segment.end());
        float unc_total_min = *std::min_element(v_unc_total.begin(), v_unc_total.end());

        std::cout << "Max uncertainty for (statunc, dRcut, clusAdcCut, clusSizeCut, segment, total) = (" << unc_statunccorr_max * 100 << ", " << unc_dRcut_max * 100 << ", " << unc_clusAdcCut_max * 100 << ", " << unc_clusSizeCut_max * 100 << ", " << unc_segment_max * 100 << ", " << unc_total_max * 100 << ")" << std::endl;
        std::cout << "Min uncertainty for (statunc, dRcut, clusAdcCut, clusSizeCut, segment, total) = (" << unc_statunccorr_min * 100 << ", " << unc_dRcut_min * 100 << ", " << unc_clusAdcCut_min * 100 << ", " << unc_clusSizeCut_min * 100 << ", " << unc_segment_min * 100 << ", " << unc_total_min * 100 << ")" << std::endl;

        v_unc_statunccorr_max_allcent.push_back(unc_statunccorr_max);
        v_unc_dRcut_max_allcent.push_back(unc_dRcut_max);
        v_unc_clusAdcCut_max_allcent.push_back(unc_clusAdcCut_max);
        v_unc_clusSizeCut_max_allcent.push_back(unc_clusSizeCut_max);
        v_unc_segment_max_allcent.push_back(unc_segment_max);
        v_unc_total_max_allcent.push_back(unc_total_max);
        v_unc_statunccorr_min_allcent.push_back(unc_statunccorr_min);
        v_unc_dRcut_min_allcent.push_back(unc_dRcut_min);
        v_unc_clusAdcCut_min_allcent.push_back(unc_clusAdcCut_min);
        v_unc_clusSizeCut_min_allcent.push_back(unc_clusSizeCut_min);
        v_unc_segment_min_allcent.push_back(unc_segment_min);
        v_unc_total_min_allcent.push_back(unc_total_min);
    }

    std::cout << "--------------------------------" << std::endl;
    std::cout << "Max uncertainty for all centralities for (statuncorr, dRcut, clusAdcCut, clusSizeCut, segment, total) = (" << *std::max_element(v_unc_statunccorr_max_allcent.begin(), v_unc_statunccorr_max_allcent.end()) * 100 << ", " << *std::max_element(v_unc_dRcut_max_allcent.begin(), v_unc_dRcut_max_allcent.end()) * 100 << ", "
              << *std::max_element(v_unc_clusAdcCut_max_allcent.begin(), v_unc_clusAdcCut_max_allcent.end()) * 100 << ", " << *std::max_element(v_unc_clusSizeCut_max_allcent.begin(), v_unc_clusSizeCut_max_allcent.end()) * 100 << ", " << *std::max_element(v_unc_segment_max_allcent.begin(), v_unc_segment_max_allcent.end()) * 100 << ", "
              << *std::max_element(v_unc_total_max_allcent.begin(), v_unc_total_max_allcent.end()) * 100 << ")" << std::endl;
    std::cout << "Min uncertainty for all centralities for (statuncorr, dRcut, clusAdcCut, clusSizeCut, segment, total) = (" << *std::min_element(v_unc_statunccorr_min_allcent.begin(), v_unc_statunccorr_min_allcent.end()) * 100 << ", " << *std::min_element(v_unc_dRcut_min_allcent.begin(), v_unc_dRcut_min_allcent.end()) * 100 << ", "
              << *std::min_element(v_unc_clusAdcCut_min_allcent.begin(), v_unc_clusAdcCut_min_allcent.end()) * 100 << ", " << *std::min_element(v_unc_clusSizeCut_min_allcent.begin(), v_unc_clusSizeCut_min_allcent.end()) * 100 << ", " << *std::min_element(v_unc_segment_min_allcent.begin(), v_unc_segment_min_allcent.end()) * 100 << ", "
              << *std::min_element(v_unc_total_min_allcent.begin(), v_unc_total_min_allcent.end()) * 100 << ")" << std::endl;

    // print the LaTeX table: for each category, print the min and max uncertainty for all centralities in the format of %.2f-%.2f %
    bool unc_phisize_small = (*std::max_element(v_unc_clusSizeCut_max_allcent.begin(), v_unc_clusSizeCut_max_allcent.end()) * 100 < 0.1) ? true : false;
    // if the uncertainty is less than 0.1%, round up the number to the nearest 0.01 and print it as <[rounded number]
    float unc_phisize_small_rounded = 0.0;
    if (unc_phisize_small)
    {
        unc_phisize_small_rounded = ceil(*std::max_element(v_unc_clusSizeCut_max_allcent.begin(), v_unc_clusSizeCut_max_allcent.end()) * 100 * 100) / 100;
    }
    // format to print 3 significant digits
    std::ostringstream oss;
    oss << "<" << std::fixed << std::setprecision(2) << unc_phisize_small_rounded;
    std::string unc_phisize_small_str = oss.str();
    std::cout << "\\begin{table}[H]" << std::endl;
    std::cout << "\t\\centering" << std::endl;
    std::cout << "\t\\begin{tabular}{l c c}" << std::endl;
    std::cout << "\t\\hline" << std::endl;
    std::cout << "\t\\textbf{Source} & \\textbf{\\phobos approach [\\%]} & \\textbf{\\cms approach [\\%]} \\\\" << std::endl;
    std::cout << "\t\\hline" << std::endl;
    std::cout << "\tStatistical Uncertainty in corrections & & ";
    print_with_significant_digits(*std::min_element(v_unc_statunccorr_min_allcent.begin(), v_unc_statunccorr_min_allcent.end()) * 100, 3);
    std::cout << "--";
    print_with_significant_digits(*std::max_element(v_unc_statunccorr_max_allcent.begin(), v_unc_statunccorr_max_allcent.end()) * 100, 3);
    std::cout << " \\\\" << std::endl;
    std::cout << "\tTracklet reconstruction cut variation & & ";
    print_with_significant_digits(*std::min_element(v_unc_dRcut_min_allcent.begin(), v_unc_dRcut_min_allcent.end()) * 100, 3);
    std::cout << "--";
    print_with_significant_digits(*std::max_element(v_unc_dRcut_max_allcent.begin(), v_unc_dRcut_max_allcent.end()) * 100, 3);
    std::cout << " \\\\" << std::endl;
    std::cout << "\tCluster ADC cut variation & & ";
    print_with_significant_digits(*std::min_element(v_unc_clusAdcCut_min_allcent.begin(), v_unc_clusAdcCut_min_allcent.end()) * 100, 3);
    std::cout << "--";
    print_with_significant_digits(*std::max_element(v_unc_clusAdcCut_max_allcent.begin(), v_unc_clusAdcCut_max_allcent.end()) * 100, 3);
    std::cout << " \\\\" << std::endl;
    std::cout << "\tCluster \$\\phi\$-size cut variation & & ";
    print_with_significant_digits(*std::min_element(v_unc_clusSizeCut_min_allcent.begin(), v_unc_clusSizeCut_min_allcent.end()) * 100, 3);
    std::cout << "--";
    print_with_significant_digits(*std::max_element(v_unc_clusSizeCut_max_allcent.begin(), v_unc_clusSizeCut_max_allcent.end()) * 100, 3);
    std::cout << " \\\\" << std::endl;
    std::cout << "\tRun segment variation & & ";
    print_with_significant_digits(*std::min_element(v_unc_segment_min_allcent.begin(), v_unc_segment_min_allcent.end()) * 100, 3);
    std::cout << "--";
    print_with_significant_digits(*std::max_element(v_unc_segment_max_allcent.begin(), v_unc_segment_max_allcent.end()) * 100, 3);
    std::cout << " \\\\" << std::endl;
    std::cout << "\tMisalignment & & -- \\\\\\cmidrule(rl){2-3}" << std::endl;
    std::cout << "\t\\textbf{Total} & & ";
    print_with_significant_digits(*std::min_element(v_unc_total_min_allcent.begin(), v_unc_total_min_allcent.end()) * 100, 3);
    std::cout << "--";
    print_with_significant_digits(*std::max_element(v_unc_total_max_allcent.begin(), v_unc_total_max_allcent.end()) * 100, 3);
    std::cout << " \\\\" << std::endl;
    std::cout << "\t\\hline" << std::endl;
    std::cout << "\t\\end{tabular}" << std::endl;
    std::cout << "\t\\caption{Systematic uncertainties for different sources.}" << std::endl;
    std::cout << "\t\\label{tab:uncsummary}" << std::endl;
    std::cout << "\\end{table}" << std::endl;
}