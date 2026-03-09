#include <algorithm>
#include <cmath>
#include <iostream>
#include <type_traits>
#include <vector>

// Template function to calculate percentile bounds in descending order
template <typename T>
std::vector<std::pair<T, T>> calculatePercentileBounds(const std::vector<T>& data, int interval) {
    // Ensure T is a numeric type
    static_assert(std::is_arithmetic<T>::value, "T must be a numeric type.");

    if (data.empty()) {
        throw std::invalid_argument("Data vector is empty.");
    }

    if (interval <= 0 || interval > 100) {
        throw std::invalid_argument("Interval must be between 1 and 100.");
    }

    // Create a copy of the input vector and sort it in descending order
    std::vector<T> sortedData = data;
    std::sort(sortedData.begin(), sortedData.end(), std::greater<T>());

    std::vector<std::pair<T, T>> percentileBounds;
    size_t n = sortedData.size();

    // Calculate percentile bounds based on the interval
    for (int p = 0; p < 100; p += interval) {
        double lowerRank = p / 100.0 * (n - 1);             // Lower bound rank
        double upperRank = (p + interval) / 100.0 * (n - 1); // Upper bound rank

        size_t lowerIdx = static_cast<size_t>(std::floor(lowerRank)); // Floor index for lower bound
        size_t upperIdx = static_cast<size_t>(std::ceil(upperRank));  // Ceil index for upper bound

        // Interpolate lower bound
        T lowerValue = sortedData[lowerIdx];
        if (lowerIdx < n - 1 && lowerIdx != upperRank) {
            double lowerWeight = lowerRank - lowerIdx;
            lowerValue = static_cast<T>(
                sortedData[lowerIdx] * (1 - lowerWeight) + sortedData[lowerIdx + 1] * lowerWeight
            );
        }

        // Interpolate upper bound
        T upperValue;
        if (p + interval < 100) {
            upperValue = sortedData[upperIdx];
            if (upperIdx > 0 && upperIdx != upperRank) {
                double upperWeight = upperRank - upperIdx;
                upperValue = static_cast<T>(
                    sortedData[upperIdx - 1] * (1 - upperWeight) + sortedData[upperIdx] * upperWeight
                );
            }
        } else {
            // For the last interval, set the upper bound to exceed the max value
            upperValue = sortedData[n - 1] - 1; // Subtract 1 or adjust as needed for descending
        }

        percentileBounds.emplace_back(lowerValue, upperValue);
    }

    return percentileBounds;
}

// make a function to draw 2D histograms
void draw2DHist(TH2 *hist, bool logz, const std::string exttext, const std::string &filename)
{
    TCanvas *c = new TCanvas("canvas", "canvas", 800, 700);
    c->cd();
    c->SetLogz(logz);
    gPad->SetRightMargin(0.2);
    hist->GetZaxis()->SetTitleOffset(1.5);
    hist->Draw("colz");
    // Add exttext to the plot
    TLatex text;
    text.SetNDC();
    text.SetTextFont(43);
    text.SetTextSize(25);
    text.DrawLatex(0.2, 0.85, exttext.c_str());
    // Save the plot
    c->SaveAs(Form("%s.pdf", filename.c_str()));
    c->SaveAs(Form("%s.png", filename.c_str()));

    //properly delete the canvas and memory
    delete c;
}

void INTTVtxZ_Sim()
{
    int Npercentile = 10;

    std::string fileprefix = "Sim_HIJING_ananew_20250131";
    std::string plotdir = "./RecoPV_ana/" + fileprefix;
    system(Form("mkdir -p %s", plotdir.c_str()));

    ROOT::EnableImplicitMT();
    ROOT::RDataFrame df("minitree", Form("/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/minitree/VtxEvtMap_%s/minitree_00*.root", fileprefix.c_str()));
    // get NClusLayer1 in each event in std::vector<int>
    auto NClusLayer1 = df.Take<int>("NClusLayer1");
    // print out the min, max of NClusLayer1
    std::cout << "NClusLayer1: min = " << *std::min_element(NClusLayer1.begin(), NClusLayer1.end()) << ", max = " << *std::max_element(NClusLayer1.begin(), NClusLayer1.end()) << std::endl;
    // get the percentile intervals
    auto percentiles = calculatePercentileBounds(*NClusLayer1, Npercentile);
    std::cout << "Percentiles (NClusLayer1): ";
    for (const auto &p : percentiles)
    {
        std::cout << "[" << p.first << ", " << p.second << "] ";
    }
    std::cout << std::endl;

    // Add a new column to the dataframe: the difference between the reconstructed and truth vertex Z PV_z - TruthPV_z
    auto df_with_diff = df.Define("RecoTruthDiff", "PV_z - TruthPV_z");
    // Add a new column to label the events based on the percentile intervals
    auto df_with_percentile = df_with_diff.Define("NClusLayer1Percentile", [percentiles](int NClusLayer1) {
        for (size_t i = 0; i < percentiles.size(); i++)
        {
            if (NClusLayer1 <= percentiles[i].first && NClusLayer1 > percentiles[i].second)
            {
                return static_cast<int>(i);
            }
        }
        return -1;
    }, {"NClusLayer1"});

    // histogram for reconstructed and truth vertex Z
    auto hM_TruthVtxZ_RecoTruthDiff = df_with_percentile.Filter("is_min_bias").Histo2D({"hM_TruthVtxZ_RecoTruthDiff", ";vtx_{Z}^{Truth} [cm];vtx_{Z}^{Reco}-vtx_{Z}^{Truth} [cm];Entries", 200, -25, 25, 200, -5, 5}, "TruthPV_z", "RecoTruthDiff");
    draw2DHist(hM_TruthVtxZ_RecoTruthDiff.GetPtr(), true, "", Form("%s/TruthVtxZ_RecoTruthDiff_Sim", plotdir.c_str()));
    
    // create histograms for each percentile interval and draw them
    for (size_t i = 0; i < percentiles.size(); i++)
    {
        auto hM_TruthVtxZ_RecoTruthDiff_Percentile = df_with_percentile.Filter("is_min_bias").Filter([i](int NClusLayer1Percentile) { return NClusLayer1Percentile == i; }, {"NClusLayer1Percentile"}).Histo2D({"hM_TruthVtxZ_RecoTruthDiff_Percentile", ";vtx_{Z}^{Truth} [cm];vtx_{Z}^{Reco}-vtx_{Z}^{Truth} [cm];Entries", 200, -25, 25, 200, -5, 5}, "TruthPV_z", "RecoTruthDiff");
        draw2DHist(hM_TruthVtxZ_RecoTruthDiff_Percentile.GetPtr(), true, Form("Cluster multiplicity percentile %d-%d%%", static_cast<int>(i*100/Npercentile), static_cast<int>((i+1)*100/Npercentile)), Form("%s/TruthVtxZ_RecoTruthDiff_Percentile%d_Sim", plotdir.c_str(), static_cast<int>(i)));
    }
}