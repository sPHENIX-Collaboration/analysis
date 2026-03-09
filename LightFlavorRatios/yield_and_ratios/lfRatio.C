#include "util.C"

template <typename T>
string to_string_with_precision(const T a_value, const int n = 0)
{
    ostringstream out;
    out.precision(n);
    out << fixed << a_value;
    return out.str();
}

//=====================
void lfRatio(bool save_plot = true)
{
  string plotName = "lfRatio";

  string axisTitles = ";p_{T} [GeV];#Lambda^{0}/2K^{0}_{S}";

  // ===== Graph configuration =====
  vector<plot_info> plot_infos;

  //sPHENIX Proj
  float proj_lumi = 4; // inv nb
  float scale = 13./proj_lumi; // Simulated lumi / projected lumi
  float yValue = 0.9;

  //CMS https://www.hepdata.net/record/ins1464834 
  vector<data_point> cms_high_mult;
  cms_high_mult.push_back({0.5,0.4,0.6,0.094,0.007,0.0076,0.0076});
  cms_high_mult.push_back({0.7,0.6,0.8,0.168,0.0042,0.0136,0.0136});
  cms_high_mult.push_back({0.9,0.8,1.0,0.229,0.0037,0.0185,0.0185});
  cms_high_mult.push_back({1.1,1.0,1.2,0.287,0.0039,0.0232,0.0232});
  cms_high_mult.push_back({1.3,1.2,1.4,0.35,0.0044,0.0286,0.0286});
  cms_high_mult.push_back({1.5,1.4,1.6,0.43,0.0051,0.0266,0.0266});
  cms_high_mult.push_back({1.7,1.6,1.8,0.49,0.0059,0.0301,0.0301});
  cms_high_mult.push_back({1.9,1.8,2.0,0.54,0.0067,0.033,0.033});
  cms_high_mult.push_back({2.1,2.0,2.2,0.57,0.0076,0.036,0.036});
  cms_high_mult.push_back({2.3,2.2,2.4,0.63,0.0089,0.039,0.039});
  cms_high_mult.push_back({2.5,2.4,2.6,0.65,0.01,0.041,0.041});
  cms_high_mult.push_back({2.7,2.6,2.8,0.68,0.012,0.042,0.042});
  cms_high_mult.push_back({2.9,2.8,3.0,0.68,0.013,0.042,0.042});
  cms_high_mult.push_back({3.2,3.0,3.4,0.69,0.011,0.043,0.043});
  cms_high_mult.push_back({3.6,3.4,3.8,0.67,0.013,0.042,0.042});
  cms_high_mult.push_back({4.0,3.8,4.2,0.66,0.016,0.041,0.041});
  cms_high_mult.push_back({4.6,4.2,5.0,0.64,0.015,0.04,0.04});
  plot_infos.push_back(plot_info("CMS 6.2 pb^{-1} pp, 7 TeV, 110#kern[0.3]{#leq}N_{trk}#kern[-0.3]{#leq}130", black_color, 34, cms_high_mult));

  vector<data_point> cms_low_mult;
  cms_low_mult.push_back({0.5,0.4,0.6,0.24,0.0052,0.0195,0.0195});
  cms_low_mult.push_back({0.7,0.6,0.8,0.299,0.0037,0.0242,0.0242});
  cms_low_mult.push_back({0.9,0.8,1.0,0.37,0.0037,0.0297,0.0297});
  cms_low_mult.push_back({1.1,1.0,1.2,0.41,0.0042,0.033,0.033});
  cms_low_mult.push_back({1.3,1.2,1.4,0.47,0.005,0.038,0.038});
  cms_low_mult.push_back({1.5,1.4,1.6,0.51,0.0058,0.0314,0.0314});
  cms_low_mult.push_back({1.7,1.6,1.8,0.54,0.0068,0.033,0.033});
  cms_low_mult.push_back({1.9,1.8,2.0,0.55,0.0079,0.034,0.034});
  cms_low_mult.push_back({2.1,2.0,2.2,0.54,0.0088,0.034,0.034});
  cms_low_mult.push_back({2.3,2.2,2.4,0.55,0.0083,0.034,0.034});
  cms_low_mult.push_back({2.5,2.4,2.6,0.55,0.012,0.034,0.034});
  cms_low_mult.push_back({2.7,2.6,2.8,0.52,0.013,0.032,0.032});
  cms_low_mult.push_back({2.9,2.8,3.0,0.54,0.016,0.033,0.033});
  cms_low_mult.push_back({3.2,3.0,3.4,0.49,0.012,0.031,0.031});
  cms_low_mult.push_back({3.6,3.4,3.8,0.44,0.015,0.027,0.027});
  cms_low_mult.push_back({4.0,3.8,4.2,0.4,0.018,0.025,0.025});
  cms_low_mult.push_back({4.6,4.2,5.0,0.32,0.016,0.02,0.02});
  plot_infos.push_back(plot_info("CMS 6.2 pb^{-1} pp, 7 TeV, 0#kern[0.]{#leq}N_{trk}#kern[0.]{#leq}35", red_color, 29, cms_low_mult));


  //STAR anti-proton/pion https://www.hepdata.net/record/ins709170
  vector<data_point> star;
  star.push_back({0.468,0.468,0.468,0.068,0.005,0.01,0.01});
  star.push_back({0.563,0.563,0.563,0.091,0.006,0.014,0.014});
  star.push_back({0.661,0.661,0.661,0.123,0.008,0.019,0.019});
  star.push_back({0.759,0.759,0.759,0.132,0.009,0.02,0.02});
  star.push_back({0.858,0.858,0.858,0.165,0.012,0.026,0.026});
  star.push_back({0.957,0.957,0.957,0.188,0.016,0.029,0.029});
  star.push_back({1.06,1.06,1.06,0.229,0.018,0.026,0.026});
  star.push_back({1.16,1.16,1.16,0.238,0.022,0.027,0.027});
  star.push_back({1.31,1.31,1.31,0.265,0.021,0.03,0.03});
  star.push_back({1.5,1.5,1.5,0.268,0.03,0.03,0.03});
  star.push_back({1.7,1.7,1.7,0.352,0.054,0.04,0.04});
  star.push_back({1.9,1.9,1.9,0.324,0.064,0.046,0.046});
  star.push_back({2.25,2.25,2.25,0.32,0.071,0.084,0.084});
  star.push_back({2.75,2.75,2.75,0.289,0.007,0.04,0.04});
  star.push_back({3.25,3.25,3.25,0.34,0.013,0.048,0.048});
  star.push_back({3.75,3.75,3.75,0.34,0.061,0.049,0.049});
  star.push_back({4.25,4.25,4.25,0.271,0.027,0.039,0.039});
  star.push_back({4.75,4.75,4.75,0.269,0.042,0.039,0.039});
  star.push_back({5.5,5.5,5.5,0.236,0.048,0.035,0.035});
  star.push_back({6.5,6.5,6.5,0.179,0.075,0.027,0.027});
  //plot_infos.push_back(plot_info("STAR pp, 200 GeV, R(#bar{p}/#pi^{-})", green_color, 33, star));

  //sPHENIX
  vector<data_point> sphenix;
  // xMean, xLow, xHigh, yMean, yStat, ySystLow, ySystHigh
  sphenix.push_back({.1, 0, .2, 0.001726438, 0.0003682367, 0, 0});
  sphenix.push_back({.3, .2, .4, 0.01498159, 0.0006501319, 0, 0});
  sphenix.push_back({.5, .4, .6, 0.1310696, 0.001475939, 0, 0});
  sphenix.push_back({.7, .6, .8, 0.2864402, 0.002205694, 0, 0});
  sphenix.push_back({.9, .8, 1.0, 0.3850074, 0.002991455, 0, 0});
  sphenix.push_back({1.1, 1.0, 1.2, 0.4419578, 0.004010049, 0, 0});
  sphenix.push_back({1.3, 1.2, 1.4, 0.4672351, 0.005322076, 0, 0});
  sphenix.push_back({1.5, 1.4, 1.6, 0.4689924, 0.007005992, 0, 0});
  sphenix.push_back({1.7, 1.6, 1.8, 0.4359442, 0.0088045, 0, 0});
  sphenix.push_back({1.9, 1.8, 2.0, 0.4278972, 0.01167023, 0, 0});
  sphenix.push_back({2.1, 2.0, 2.2, 0.3912449, 0.01460502, 0, 0});
  sphenix.push_back({2.3, 2.2, 2.4, 0.3576375, 0.01853251, 0, 0});
  sphenix.push_back({2.5, 2.4, 2.6, 0.2896642, 0.02089558, 0, 0});
  sphenix.push_back({2.7, 2.6, 2.8, 0.2957284, 0.02726913, 0, 0});
  sphenix.push_back({2.9, 2.8, 3.0, 0.2348485, 0.03152833, 0, 0});
  sphenix.push_back({3.1, 3.0, 3.2, 0.1843575, 0.03353905, 0, 0});
  sphenix.push_back({3.3, 3.2, 3.4, 0.2028985, 0.04646774, 0, 0});
  sphenix.push_back({3.5, 3.4, 3.6, 0.09677419, 0.04045244, 0, 0});
  sphenix.push_back({3.7, 3.6, 3.8, 0.1388889, 0.0642335, 0, 0});
  sphenix.push_back({3.9, 3.8, 4.0, 0.212766, 0.1000852, 0, 0});
  sphenix.push_back({4.1, 4.0, 4.2, 0.05128205, 0.05193535, 0, 0});
  //sphenix.push_back({4.3, 4.2, 4.4, , , 0, 0});
  sphenix.push_back({4.5, 4.4, 4.6, 0.2352941, 0.1758929, 0, 0});
  sphenix.push_back({4.7, 4.6, 4.8, 0.2857143, 0.2159797, 0, 0});
  sphenix.push_back({4.9, 4.8, 5.0, 0.25, 0.265165, 0, 0});
  //plot_infos.push_back(plot_info("sPHENIX, 0.05 pb^{-1}, 200 GeV, 0#kern[0.]{#leq}N_{trk}#kern[0.]{#leq}25", blue_color, 22, sphenix));

  vector<data_point> sphenix_OO;
  sphenix_OO.push_back({.3, .2, .4, 0.01089852, 0.001913207, 0, 0});
  sphenix_OO.push_back({.5, .4, .6, 0.03602371, 0.001676573, 0, 0});
  sphenix_OO.push_back({.7, .6, .8, 0.04499276, 0.001248984, 0, 0});
  sphenix_OO.push_back({.9, .8, 1.0, 0.05446533, 0.001498692, 0, 0});
  sphenix_OO.push_back({1.1, 1.0, 1.2, 0.06701498, 0.002057071, 0, 0});
  sphenix_OO.push_back({1.3, 1.2, 1.4, 0.08006025, 0.002934237, 0, 0});
  sphenix_OO.push_back({1.5, 1.4, 1.6, 0.09150492, 0.004256059, 0, 0});
  sphenix_OO.push_back({1.7, 1.6, 1.8, 0.09419005, 0.005842278, 0, 0});
  sphenix_OO.push_back({1.9, 1.8, 2.0, 0.1107023, 0.009028991, 0, 0});
  sphenix_OO.push_back({2.1, 2.0, 2.2, 0.05622295, 0.007143845, 0, 0});
  sphenix_OO.push_back({2.3, 2.2, 2.4, 0.09146372, 0.01814119, 0, 0});
  sphenix_OO.push_back({2.5, 2.4, 2.6, 0.01732629, 0.005819996, 0, 0});
  sphenix_OO.push_back({2.7, 2.6, 2.8, 0.0410619, 0.0243738, 0, 0});
  sphenix_OO.push_back({2.9, 2.8, 3.0, 0.08192092, 0.106904, 0, 0});
  //plot_infos.push_back(plot_info("sPHENIX, 200GeV, O+O",green_color,22,sphenix_OO));

  vector<data_point> sphenix_3run;
  sphenix_3run.push_back({.34, .20, .48, 0.04549573, 0.003857831, 0, 0});
  sphenix_3run.push_back({.62, .48, .76, 0.2267537, 0.001509736, 0, 0});
  sphenix_3run.push_back({.90, .76, 1.04, 0.3664095, 0.002076299, 0, 0});
  sphenix_3run.push_back({1.18, 1.04, 1.32, 0.4429479, 0.003736974, 0, 0});
  sphenix_3run.push_back({1.46, 1.32, 1.60, 0.4611664, 0.006344749, 0, 0});
  sphenix_3run.push_back({1.74, 1.60, 1.88, 0.4358365, 0.01007528, 0, 0});
  sphenix_3run.push_back({2.02, 1.88, 2.16, 0.3596045, 0.01446338, 0, 0});
  sphenix_3run.push_back({2.30, 2.16, 2.44, 0.2959594, 0.02216775, 0, 0});
  sphenix_3run.push_back({2.58, 2.44, 2.72, 0.2461123, 0.03135817, 0, 0});
  sphenix_3run.push_back({2.86, 2.72, 3.00, 0.2900647, 0.0132378, 0, 0});
  //plot_infos.push_back(plot_info("sPHENIX, run3 pp, runs 79514-79516",green_color,22,sphenix_3run));

  vector<data_point> sphenix_effcorr;
  sphenix_effcorr.push_back({0.65,0.5,0.8,0.5344604,0.001185643,0,0});
  sphenix_effcorr.push_back({0.95,0.8,1.1,0.5638949,0.001404973,0,0});
  sphenix_effcorr.push_back({1.25,1.1,1.4,0.5729109,0.001866033,0,0});
  sphenix_effcorr.push_back({1.6,1.4,1.8,0.5708982,0.002445078,0,0});
  sphenix_effcorr.push_back({2.,1.8,2.2,0.5638121,0.004267988,0,0});
  sphenix_effcorr.push_back({2.6,2.2,3.,0.532113,0.005824996,0,0});
  sphenix_effcorr.push_back({3.5,3.,4.,0.5047523,0.0100021,0,0});
  plot_infos.push_back(plot_info("sPHENIX, run3 pp, runs 79514-79516",blue_color,22,sphenix_effcorr));

  vector<data_point> sphenix_79514;
  sphenix_79514.push_back({.34, .20, .48, 0.01089852, 0.001913207, 0, 0});
  sphenix_79514.push_back({.62, .48, .76, 0.03602371, 0.001676573, 0, 0});
  sphenix_79514.push_back({.90, .76, 1.04, 0.04499276, 0.001248984, 0, 0});
  sphenix_79514.push_back({1.18, 1.04, 1.32, 0.05446533, 0.001498692, 0, 0});
  sphenix_79514.push_back({1.46, 1.32, 1.60, 0.06701498, 0.002057071, 0, 0});
  sphenix_79514.push_back({1.74, 1.60, 1.88, 0.08006025, 0.002934237, 0, 0});
  sphenix_79514.push_back({2.02, 1.88, 2.16, 0.09150492, 0.004256059, 0, 0});
  sphenix_79514.push_back({2.30, 2.16, 2.44, 0.09419005, 0.005842278, 0, 0});
  sphenix_79514.push_back({2.58, 2.44, 2.72, 0.1107023, 0.009028991, 0, 0});
  sphenix_79514.push_back({2.86, 2.72, 3.00, 0.05622295, 0.007143845, 0, 0});
  //plot_infos.push_back(plot_info("sPHENIX, run3 pp, run 79514",green_color,22,sphenix_79514));

  // ===== Draw histograms =====
  TCanvas *canvas = new TCanvas("canvas", "", 800, 600);
  canvas->cd();

  auto multigraph = new TMultiGraph();

  TLegend *legend = new TLegend(0.15, 0.68, 0.8, 0.88);
  legend->SetBorderSize(0);
  legend->SetLineColor(1);
  legend->SetLineStyle(1);
  legend->SetLineWidth(1);
  legend->SetFillColor(0);
  legend->SetFillStyle(0);
  legend->SetTextSize(text_size);

  for (unsigned int i = 0; i < plot_infos.size(); ++i)
  {
    TGraphMultiErrors* myGraph = makeGraph(plot_infos[i]);
    multigraph->Add(myGraph);
    TLegendEntry *entry=legend->AddEntry(myGraph, plot_infos[i].legend_info.c_str(), "PE1");
  }

  multigraph->SetTitle(axisTitles.c_str());
  //multigraph->GetXaxis()->SetRangeUser(0, 33);
  multigraph->GetYaxis()->SetRangeUser(0, 1.1);
  multigraph->Draw("APE1S ; E1 ; 5 s=0.1");
  
  //makeLine(0, 33);

  legend->Draw();

  TPaveText *pt;
  pt = new TPaveText(0.15,0.9,0.95,1., "NDC");
  pt->SetFillColor(0);
  pt->SetFillStyle(0);
  pt->SetTextFont(42);
  TText *pt_LaTex;
  string labelString = "#it{#bf{sPHENIX}} Internal, Preliminary efficiency correction";
  pt_LaTex = pt->AddText(labelString.c_str());
  pt->SetBorderSize(0);
  pt->Draw();
  gPad->Modified();


  if(save_plot)
  {
    string extensions[] = {".png", ".pdf"};
    for (auto extension : extensions)
    {
      string output = "plots/" + plotName + extension;
      canvas->SaveAs(output.c_str());
    }
  }
}
