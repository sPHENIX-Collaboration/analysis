#include "./plotutil.h"

void trackletcorr()
{
    // directory to save the output plots; if the directory does not exist, create it using
    std::string plotdir = "tracklet_clustercorr";
    system(Form("mkdir -p %s", plotdir.c_str()));

    ROOT::EnableImplicitMT();
    ROOT::RDataFrame df("minitree", "/sphenix/tg/tg01/hf/hjheng/ppg02/minitree/TrackletMinitree_Data_Run54280_20250210_ProdA2024/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet1/minitree_segment*.root");

    // is_min_bias && firedTrig10_MBDSNgeq2 && |PV_z| <= 10 && |MBD_z_vtx| <= 10 && MBD_centrality<=70
    auto df_evtsel = df.Filter("is_min_bias && firedTrig10_MBDSNgeq2 && abs(PV_z) <= 10 && abs(MBD_z_vtx) <= 10 && MBD_centrality<=70 && !InttBco_IsToBeRemoved");

    // 2D histograms for correlation of clusters on tracklets
    auto hM_clus1Eta_clus2Eta = df_evtsel.Histo2D({"hM_clus1Eta_clus2Eta", ";Tracklet inner-cluster #eta;Tracklet Tracklet outer-cluster #eta", 200, -2.5, 2.5, 200, -2.5, 2.5}, "tklclus1Eta", "tklclus2Eta");
    auto hM_clus1Phi_clus2Phi = df_evtsel.Histo2D({"hM_clus1Phi_clus2Phi", ";Tracklet inner-cluster #phi;Tracklet outer-cluster #phi", 128, -3.2, 3.2, 128, -3.2, 3.2}, "tklclus1Phi", "tklclus2Phi");
    auto hM_clus1PhiSize_clus2PhiSize = df_evtsel.Histo2D({"hM_clus1PhiSize_clus2PhiSize", ";Tracklet inner-cluster #phi-size;Tracklet outer-cluster #phi-size", 86, -0.5, 85.5, 86, -0.5, 85.5}, "tklclus1PhiSize", "tklclus2PhiSize");
    auto hM_clus1PhiSize_clus2PhiSize_Zoomin = df_evtsel.Histo2D({"hM_clus1PhiSize_clus2PhiSize", ";Tracklet inner-cluster #phi-size;Tracklet outer-cluster #phi-size", 7, 41.5, 48.5, 7, 41.5, 48.5}, "tklclus1PhiSize", "tklclus2PhiSize");
    auto hM_clus1Adc_clus2Adc = df_evtsel.Histo2D({"hM_clus1Adc_clus2Adc", ";Tracklet inner-cluster ADC;Tracklet outer-cluster ADC", 200, 0, 20000, 200, 0, 20000}, "tklclus1ADC", "tklclus2ADC");

    // draw 2D histograms
    std::vector<const char *> plotinfo = {"Centrality#leq70", "Is MinBias, Trigger bit 10 (MBD N&S#geq2)", "|INTT&MBD vtx_{Z}|#leq10cm, cluster ADC>35"};
    draw2Dhist(hM_clus1Eta_clus2Eta.GetPtr(), "Tracklet inner-cluster #eta", "Tracklet outer-cluster #eta", true, plotinfo, "colz", Form("%s/clus1Eta_vs_clus2Eta", plotdir.c_str()));
    draw2Dhist(hM_clus1Phi_clus2Phi.GetPtr(), "Tracklet inner-cluster #phi", "Tracklet outer-cluster #phi", true, plotinfo, "colz", Form("%s/clus1Phi_vs_clus2Phi", plotdir.c_str()));
    draw2Dhist(hM_clus1PhiSize_clus2PhiSize.GetPtr(), "Tracklet inner-cluster #phi-size", "Tracklet outer-cluster #phi-size", true, plotinfo, "colz", Form("%s/clus1PhiSize_vs_clus2PhiSize", plotdir.c_str()));
    draw2Dhist(hM_clus1PhiSize_clus2PhiSize_Zoomin.GetPtr(), "Tracklet inner-cluster #phi-size", "Tracklet outer-cluster #phi-size", true, plotinfo, "colztext", Form("%s/clus1PhiSize_vs_clus2PhiSize_Zoomin", plotdir.c_str()));
    draw2Dhist(hM_clus1Adc_clus2Adc.GetPtr(), "Tracklet inner-cluster ADC", "Tracklet outer-cluster ADC", true, plotinfo, "colz", Form("%s/clus1Adc_vs_clus2Adc", plotdir.c_str()));

    // for hM_clus1PhiSize_clus2PhiSize: count the number of entries for the bins where both phi-sizes are 43 and 46
    int N_clus1PhiSize4346_and_clus2PhiSize4346 = hM_clus1PhiSize_clus2PhiSize->GetBinContent(hM_clus1PhiSize_clus2PhiSize->GetXaxis()->FindBin(43), hM_clus1PhiSize_clus2PhiSize->GetYaxis()->FindBin(46)) + //
                                                  hM_clus1PhiSize_clus2PhiSize->GetBinContent(hM_clus1PhiSize_clus2PhiSize->GetXaxis()->FindBin(46), hM_clus1PhiSize_clus2PhiSize->GetYaxis()->FindBin(43)) + //
                                                  hM_clus1PhiSize_clus2PhiSize->GetBinContent(hM_clus1PhiSize_clus2PhiSize->GetXaxis()->FindBin(43), hM_clus1PhiSize_clus2PhiSize->GetYaxis()->FindBin(43)) + //
                                                  hM_clus1PhiSize_clus2PhiSize->GetBinContent(hM_clus1PhiSize_clus2PhiSize->GetXaxis()->FindBin(46), hM_clus1PhiSize_clus2PhiSize->GetYaxis()->FindBin(46));

    // count the number of entries for the bins where either phi-size is 43 or 46
    int N_clus1PhiSize_or_clus2PhiSize4346 = 0;
    for (int i = 0; i < hM_clus1PhiSize_clus2PhiSize->GetNbinsX(); i++)
    {
        for (int j = 0; j < hM_clus1PhiSize_clus2PhiSize->GetNbinsY(); j++)
        {
            if (i == 43 || i == 46 || j == 43 || j == 46)
            {
                N_clus1PhiSize_or_clus2PhiSize4346 += hM_clus1PhiSize_clus2PhiSize->GetBinContent(i, j);
            }
        }
    }

    // remove double counting
    N_clus1PhiSize_or_clus2PhiSize4346 -= N_clus1PhiSize4346_and_clus2PhiSize4346;

    double frac_clus1PhiSize4346_and_clus2PhiSize4346 = static_cast<double>(N_clus1PhiSize4346_and_clus2PhiSize4346) / static_cast<double>(hM_clus1PhiSize_clus2PhiSize->Integral(-1, -1, -1, -1)) * 100;
    double frac_clus1PhiSize4346_or_clus2PhiSize4346 = static_cast<double>(N_clus1PhiSize_or_clus2PhiSize4346) / static_cast<double>(hM_clus1PhiSize_clus2PhiSize->Integral(-1, -1, -1, -1)) * 100;
    cout << "Total number of entries in hM_clus1PhiSize_clus2PhiSize = " << hM_clus1PhiSize_clus2PhiSize->Integral(-1, -1, -1, -1) << endl                                  //
         << "Number of entries for bins where both constituent cluster have a phi-size of 43 or 46 = " << N_clus1PhiSize4346_and_clus2PhiSize4346 << " -> Fraction (%) = "; //
    print_with_significant_digits(frac_clus1PhiSize4346_and_clus2PhiSize4346, 4);                                                                                           //
    cout << endl << "Number of entries for bins where either cluster has a phi-size of 43 or 46 = " << N_clus1PhiSize_or_clus2PhiSize4346 << " -> Fraction (%)= ";
    print_with_significant_digits(frac_clus1PhiSize4346_or_clus2PhiSize4346, 4);
    cout << endl;
}