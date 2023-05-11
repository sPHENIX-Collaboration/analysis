void Lc_BUP2022_AuAu_3yr()
{
//=========Macro generated from canvas: Lc_BUP2022_AuAu_3yr/Lc_BUP2022_AuAu_3yr
//=========  (Thu May 12 08:26:25 2022) by ROOT version 6.24/06
   TCanvas *Lc_BUP2022_AuAu_3yr = new TCanvas("Lc_BUP2022_AuAu_3yr", "Lc_BUP2022_AuAu_3yr",0,23,1100,800);
   gStyle->SetOptStat(0);
   gStyle->SetOptTitle(0);
   Lc_BUP2022_AuAu_3yr->Range(0,0,1,1);
   Lc_BUP2022_AuAu_3yr->SetFillColor(0);
   Lc_BUP2022_AuAu_3yr->SetBorderMode(0);
   Lc_BUP2022_AuAu_3yr->SetBorderSize(2);
   Lc_BUP2022_AuAu_3yr->SetTickx(1);
   Lc_BUP2022_AuAu_3yr->SetTicky(1);
   Lc_BUP2022_AuAu_3yr->SetLeftMargin(0.16);
   Lc_BUP2022_AuAu_3yr->SetRightMargin(0.05);
   Lc_BUP2022_AuAu_3yr->SetTopMargin(0.05);
   Lc_BUP2022_AuAu_3yr->SetBottomMargin(0.16);
   Lc_BUP2022_AuAu_3yr->SetFrameBorderMode(0);
  
// ------------>Primitives in pad: Lc_BUP2022_AuAu_3yr_1
   TPad *Lc_BUP2022_AuAu_3yr_1 = new TPad("Lc_BUP2022_AuAu_3yr_1", "Lc_BUP2022_AuAu_3yr_1",0.01,0.01,0.99,0.99);
   Lc_BUP2022_AuAu_3yr_1->Draw();
   Lc_BUP2022_AuAu_3yr_1->cd();
   Lc_BUP2022_AuAu_3yr_1->Range(-2.227848,-1.21519,11.6962,6.379747);
   Lc_BUP2022_AuAu_3yr_1->SetFillColor(0);
   Lc_BUP2022_AuAu_3yr_1->SetBorderMode(0);
   Lc_BUP2022_AuAu_3yr_1->SetBorderSize(2);
   Lc_BUP2022_AuAu_3yr_1->SetTickx(1);
   Lc_BUP2022_AuAu_3yr_1->SetTicky(1);
   Lc_BUP2022_AuAu_3yr_1->SetLeftMargin(0.16);
   Lc_BUP2022_AuAu_3yr_1->SetRightMargin(0.05);
   Lc_BUP2022_AuAu_3yr_1->SetTopMargin(0.05);
   Lc_BUP2022_AuAu_3yr_1->SetBottomMargin(0.16);
   Lc_BUP2022_AuAu_3yr_1->SetFrameBorderMode(0);
   Lc_BUP2022_AuAu_3yr_1->SetFrameBorderMode(0);
   
   TH1F *hframe__1 = new TH1F("hframe__1","",1000,0,11);
   hframe__1->SetMinimum(0);
   hframe__1->SetMaximum(6);
   hframe__1->SetDirectory(0);
   hframe__1->SetStats(0);
   hframe__1->SetLineWidth(2);
   hframe__1->SetMarkerStyle(20);
   hframe__1->SetMarkerSize(1.2);
   hframe__1->GetXaxis()->SetTitle("#it{p}_{T} [GeV]");
   hframe__1->GetXaxis()->SetLabelFont(42);
   hframe__1->GetXaxis()->SetLabelSize(0.05);
   hframe__1->GetXaxis()->SetTitleSize(0.05);
   hframe__1->GetXaxis()->SetTitleOffset(1.4);
   hframe__1->GetXaxis()->SetTitleFont(42);
   hframe__1->GetYaxis()->SetTitle("(#Lambda_{c}^{+}+#bar{#Lambda}_{c}^{-})/(D^{0}+#bar{D}^{0})");
   hframe__1->GetYaxis()->SetLabelFont(42);
   hframe__1->GetYaxis()->SetLabelSize(0.05);
   hframe__1->GetYaxis()->SetTitleSize(0.05);
   hframe__1->GetYaxis()->SetTitleOffset(1.4);
   hframe__1->GetYaxis()->SetTitleFont(42);
   hframe__1->GetZaxis()->SetLabelFont(42);
   hframe__1->GetZaxis()->SetLabelSize(0.05);
   hframe__1->GetZaxis()->SetTitleSize(0.05);
   hframe__1->GetZaxis()->SetTitleOffset(1);
   hframe__1->GetZaxis()->SetTitleFont(42);
   hframe__1->Draw(" ");
   
   Double_t Graph_from_LctoD0_pt_whole_rebin_fx1[32] = {
   0.2,
   0.6,
   1,
   1.4,
   1.8,
   2.2,
   2.6,
   3,
   3.4,
   3.8,
   4.2,
   4.6,
   5,
   5.6,
   6.4,
   7.2,
   8,
   8.8,
   9.6,
   10.4,
   11.2,
   12,
   12.8,
   13.6,
   14.4,
   15.2,
   16,
   16.8,
   17.6,
   18.4,
   19.2,
   19.8};
   Double_t Graph_from_LctoD0_pt_whole_rebin_fy1[32] = {
   0.05708453,
   0.05746944,
   0.05883775,
   0.0616128,
   0.0657317,
   0.06966373,
   0.07396804,
   0.07795289,
   0.08288672,
   0.08805873,
   0.08695824,
   0.09054691,
   0.09243361,
   0.09298433,
   0.09397365,
   0.09553719,
   0.1024978,
   0.0958231,
   0.08790383,
   0.09756098,
   0.1095238,
   0.08571429,
   0.07784431,
   0.09345794,
   0.115942,
   0.1,
   0.03225806,
   0.05,
   0.07142857,
   0.2,
   0.3333333,
   0.3333333};
   TGraph *graph = new TGraph(32,Graph_from_LctoD0_pt_whole_rebin_fx1,Graph_from_LctoD0_pt_whole_rebin_fy1);
   graph->SetName("Graph_from_LctoD0_pt_whole_rebin");
   graph->SetTitle("LctoD0_pt_whole_rebin");
   graph->SetLineStyle(2);
   graph->SetLineWidth(2);
   graph->SetMarkerStyle(20);
   graph->SetMarkerSize(1.2);
   
   TH1F *Graph_Graph_from_LctoD0_pt_whole_rebin1 = new TH1F("Graph_Graph_from_LctoD0_pt_whole_rebin1","LctoD0_pt_whole_rebin",100,0,21.76);
   Graph_Graph_from_LctoD0_pt_whole_rebin1->SetMinimum(0.002150538);
   Graph_Graph_from_LctoD0_pt_whole_rebin1->SetMaximum(0.3634409);
   Graph_Graph_from_LctoD0_pt_whole_rebin1->SetDirectory(0);
   Graph_Graph_from_LctoD0_pt_whole_rebin1->SetStats(0);
   Graph_Graph_from_LctoD0_pt_whole_rebin1->SetLineWidth(2);
   Graph_Graph_from_LctoD0_pt_whole_rebin1->SetMarkerStyle(20);
   Graph_Graph_from_LctoD0_pt_whole_rebin1->SetMarkerSize(1.2);
   Graph_Graph_from_LctoD0_pt_whole_rebin1->GetXaxis()->SetLabelFont(42);
   Graph_Graph_from_LctoD0_pt_whole_rebin1->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph_from_LctoD0_pt_whole_rebin1->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph_from_LctoD0_pt_whole_rebin1->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph_from_LctoD0_pt_whole_rebin1->GetXaxis()->SetTitleFont(42);
   Graph_Graph_from_LctoD0_pt_whole_rebin1->GetYaxis()->SetLabelFont(42);
   Graph_Graph_from_LctoD0_pt_whole_rebin1->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph_from_LctoD0_pt_whole_rebin1->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph_from_LctoD0_pt_whole_rebin1->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph_from_LctoD0_pt_whole_rebin1->GetYaxis()->SetTitleFont(42);
   Graph_Graph_from_LctoD0_pt_whole_rebin1->GetZaxis()->SetLabelFont(42);
   Graph_Graph_from_LctoD0_pt_whole_rebin1->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph_from_LctoD0_pt_whole_rebin1->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph_from_LctoD0_pt_whole_rebin1->GetZaxis()->SetTitleOffset(1);
   Graph_Graph_from_LctoD0_pt_whole_rebin1->GetZaxis()->SetTitleFont(42);
   graph->SetHistogram(Graph_Graph_from_LctoD0_pt_whole_rebin1);
   
   graph->Draw("c");
   
   Double_t Graph_from_LctoD0_pt_whole_rebin_fx2[32] = {
   0.2,
   0.6,
   1,
   1.4,
   1.8,
   2.2,
   2.6,
   3,
   3.4,
   3.8,
   4.2,
   4.6,
   5,
   5.6,
   6.4,
   7.2,
   8,
   8.8,
   9.6,
   10.4,
   11.2,
   12,
   12.8,
   13.6,
   14.4,
   15.2,
   16,
   16.8,
   17.6,
   18.4,
   19.2,
   19.8};
   Double_t Graph_from_LctoD0_pt_whole_rebin_fy2[32] = {
   0.4895522,
   0.4933722,
   0.4967906,
   0.4948327,
   0.4907336,
   0.4828507,
   0.473438,
   0.4588629,
   0.4415104,
   0.419853,
   0.4061102,
   0.3799067,
   0.3534212,
   0.3268751,
   0.293903,
   0.2546349,
   0.2435297,
   0.207082,
   0.1900561,
   0.1908178,
   0.1945137,
   0.1376518,
   0.1655629,
   0.09174312,
   0.08571429,
   0.1388889,
   0.1428571,
   0.2105263,
   0,
   0.1818182,
   0.2,
   0.3333333};
   graph = new TGraph(32,Graph_from_LctoD0_pt_whole_rebin_fx2,Graph_from_LctoD0_pt_whole_rebin_fy2);
   graph->SetName("Graph_from_LctoD0_pt_whole_rebin");
   graph->SetTitle("LctoD0_pt_whole_rebin");
   graph->SetLineWidth(2);
   graph->SetMarkerStyle(20);
   graph->SetMarkerSize(1.2);
   
   TH1F *Graph_Graph_from_LctoD0_pt_whole_rebin2 = new TH1F("Graph_Graph_from_LctoD0_pt_whole_rebin2","LctoD0_pt_whole_rebin",100,0,21.76);
   Graph_Graph_from_LctoD0_pt_whole_rebin2->SetMinimum(0);
   Graph_Graph_from_LctoD0_pt_whole_rebin2->SetMaximum(0.5464697);
   Graph_Graph_from_LctoD0_pt_whole_rebin2->SetDirectory(0);
   Graph_Graph_from_LctoD0_pt_whole_rebin2->SetStats(0);
   Graph_Graph_from_LctoD0_pt_whole_rebin2->SetLineWidth(2);
   Graph_Graph_from_LctoD0_pt_whole_rebin2->SetMarkerStyle(20);
   Graph_Graph_from_LctoD0_pt_whole_rebin2->SetMarkerSize(1.2);
   Graph_Graph_from_LctoD0_pt_whole_rebin2->GetXaxis()->SetLabelFont(42);
   Graph_Graph_from_LctoD0_pt_whole_rebin2->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph_from_LctoD0_pt_whole_rebin2->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph_from_LctoD0_pt_whole_rebin2->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph_from_LctoD0_pt_whole_rebin2->GetXaxis()->SetTitleFont(42);
   Graph_Graph_from_LctoD0_pt_whole_rebin2->GetYaxis()->SetLabelFont(42);
   Graph_Graph_from_LctoD0_pt_whole_rebin2->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph_from_LctoD0_pt_whole_rebin2->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph_from_LctoD0_pt_whole_rebin2->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph_from_LctoD0_pt_whole_rebin2->GetYaxis()->SetTitleFont(42);
   Graph_Graph_from_LctoD0_pt_whole_rebin2->GetZaxis()->SetLabelFont(42);
   Graph_Graph_from_LctoD0_pt_whole_rebin2->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph_from_LctoD0_pt_whole_rebin2->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph_from_LctoD0_pt_whole_rebin2->GetZaxis()->SetTitleOffset(1);
   Graph_Graph_from_LctoD0_pt_whole_rebin2->GetZaxis()->SetTitleFont(42);
   graph->SetHistogram(Graph_Graph_from_LctoD0_pt_whole_rebin2);
   
   graph->Draw("c");
   
   Double_t Graph0_fx3[80] = {
   0.1,
   0.3,
   0.5,
   0.7,
   0.9,
   1.1,
   1.3,
   1.5,
   1.7,
   1.9,
   2.1,
   2.3,
   2.5,
   2.7,
   2.9,
   3.1,
   3.3,
   3.5,
   3.7,
   3.9,
   4.1,
   4.3,
   4.5,
   4.7,
   4.9,
   5.1,
   5.3,
   5.5,
   5.7,
   5.9,
   6.1,
   6.3,
   6.5,
   6.7,
   6.9,
   7.1,
   7.3,
   7.5,
   7.7,
   7.9,
   8.1,
   8.3,
   8.5,
   8.7,
   8.9,
   9.1,
   9.3,
   9.5,
   9.7,
   9.9,
   10.1,
   10.3,
   10.5,
   10.7,
   10.9,
   11.1,
   11.3,
   11.5,
   11.7,
   11.9,
   12.1,
   12.3,
   12.5,
   12.7,
   12.9,
   13.1,
   13.3,
   13.5,
   13.7,
   13.9,
   14.1,
   14.3,
   14.5,
   14.7,
   14.9,
   15.1,
   15.3,
   15.5,
   15.7,
   15.9};
   Double_t Graph0_fy3[80] = {
   0.7776731,
   0.7937624,
   0.800278,
   0.824499,
   0.8638282,
   0.9138736,
   0.979411,
   1.068881,
   1.141782,
   1.209339,
   1.246834,
   1.252347,
   1.273,
   1.23797,
   1.170376,
   1.12892,
   1.054241,
   0.9733799,
   0.892422,
   0.8194392,
   0.745157,
   0.6891895,
   0.6505289,
   0.5973671,
   0.5487912,
   0.4908995,
   0.4621268,
   0.4110246,
   0.3905921,
   0.3607044,
   0.3280429,
   0.2976189,
   0.2798096,
   0.2454676,
   0.2280412,
   0.2146023,
   0.191189,
   0.1753998,
   0.1607895,
   0.1482379,
   0.1376264,
   0.1273169,
   0.1165738,
   0.1116416,
   0.1029749,
   0.100235,
   0.105441,
   0.1307842,
   0.2005114,
   0.2751935,
   0.2670397,
   0.227519,
   0.2442464,
   0.2903015,
   0.3808528,
   0.5226285,
   0.6875288,
   0.7955178,
   0.7362956,
   0.5895105,
   0.6368915,
   0.5943223,
   0.5573675,
   0.5284283,
   0.4810794,
   0.3784206,
   0.3140254,
   0.3050774,
   0.2459259,
   0.2144255,
   0.205932,
   0.2013865,
   0.1605674,
   0.1501592,
   0.1305038,
   0.1122719,
   0.1083042,
   0.09270688,
   0.07300942,
   0.07642714};
   graph = new TGraph(80,Graph0_fx3,Graph0_fy3);
   graph->SetName("Graph0");
   graph->SetTitle("dat/Greco_LcD0_1_AuAu200GeV_0_20.dat");
   graph->SetFillStyle(1000);
   graph->SetLineColor(4);
   graph->SetLineStyle(2);
   graph->SetLineWidth(2);
   graph->SetMarkerStyle(20);
   graph->SetMarkerSize(1.2);
   
   TH1F *Graph_Graph03 = new TH1F("Graph_Graph03","dat/Greco_LcD0_1_AuAu200GeV_0_20.dat",100,0,17.48);
   Graph_Graph03->SetMinimum(0.06570848);
   Graph_Graph03->SetMaximum(1.392999);
   Graph_Graph03->SetDirectory(0);
   Graph_Graph03->SetStats(0);
   Graph_Graph03->SetLineWidth(2);
   Graph_Graph03->SetMarkerStyle(20);
   Graph_Graph03->SetMarkerSize(1.2);
   Graph_Graph03->GetXaxis()->SetLabelFont(42);
   Graph_Graph03->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph03->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph03->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph03->GetXaxis()->SetTitleFont(42);
   Graph_Graph03->GetYaxis()->SetLabelFont(42);
   Graph_Graph03->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph03->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph03->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph03->GetYaxis()->SetTitleFont(42);
   Graph_Graph03->GetZaxis()->SetLabelFont(42);
   Graph_Graph03->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph03->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph03->GetZaxis()->SetTitleOffset(1);
   Graph_Graph03->GetZaxis()->SetTitleFont(42);
   graph->SetHistogram(Graph_Graph03);
   
   graph->Draw("c");
   
   Double_t Graph1_fx4[80] = {
   0.1,
   0.3,
   0.5,
   0.7,
   0.9,
   1.1,
   1.3,
   1.5,
   1.7,
   1.9,
   2.1,
   2.3,
   2.5,
   2.7,
   2.9,
   3.1,
   3.3,
   3.5,
   3.7,
   3.9,
   4.1,
   4.3,
   4.5,
   4.7,
   4.9,
   5.1,
   5.3,
   5.5,
   5.7,
   5.9,
   6.1,
   6.3,
   6.5,
   6.7,
   6.9,
   7.1,
   7.3,
   7.5,
   7.7,
   7.9,
   8.1,
   8.3,
   8.5,
   8.7,
   8.9,
   9.1,
   9.3,
   9.5,
   9.7,
   9.9,
   10.1,
   10.3,
   10.5,
   10.7,
   10.9,
   11.1,
   11.3,
   11.5,
   11.7,
   11.9,
   12.1,
   12.3,
   12.5,
   12.7,
   12.9,
   13.1,
   13.3,
   13.5,
   13.7,
   13.9,
   14.1,
   14.3,
   14.5,
   14.7,
   14.9,
   15.1,
   15.3,
   15.5,
   15.7,
   15.9};
   Double_t Graph1_fy4[80] = {
   1.533892,
   1.557891,
   1.585308,
   1.627073,
   1.688132,
   1.773658,
   1.864591,
   1.973388,
   2.099648,
   2.234283,
   2.335378,
   2.396462,
   2.535217,
   2.577498,
   2.543019,
   2.545115,
   2.497429,
   2.391786,
   2.281513,
   2.173218,
   2.033196,
   1.944806,
   1.91574,
   1.792616,
   1.708811,
   1.577383,
   1.505755,
   1.392437,
   1.354321,
   1.305396,
   1.209651,
   1.11139,
   1.072272,
   0.9651342,
   0.8896018,
   0.8758375,
   0.7738856,
   0.7200204,
   0.6675105,
   0.6200472,
   0.5798999,
   0.5417659,
   0.4843489,
   0.4663665,
   0.4053768,
   0.379064,
   0.3450989,
   0.3252435,
   0.3076776,
   0.2813757,
   0.258674,
   0.2203914,
   0.2365947,
   0.2812071,
   0.3689216,
   0.5062559,
   0.6659903,
   0.7705962,
   0.7132294,
   0.5710427,
   0.6169393,
   0.5757037,
   0.5399066,
   0.511874,
   0.4660084,
   0.3665656,
   0.3041878,
   0.2955201,
   0.2382216,
   0.2077081,
   0.1994806,
   0.1950775,
   0.1555372,
   0.1454551,
   0.1264154,
   0.1087548,
   0.1049113,
   0.08980261,
   0.07072222,
   0.07403287};
   graph = new TGraph(80,Graph1_fx4,Graph1_fy4);
   graph->SetName("Graph1");
   graph->SetTitle("dat/Greco_LcD0_2_AuAu200GeV_0_20.dat");
   graph->SetFillStyle(1000);
   graph->SetLineColor(4);
   graph->SetLineWidth(2);
   graph->SetMarkerStyle(20);
   graph->SetMarkerSize(1.2);
   
   TH1F *Graph_Graph14 = new TH1F("Graph_Graph14","dat/Greco_LcD0_2_AuAu200GeV_0_20.dat",100,0,17.48);
   Graph_Graph14->SetMinimum(0.06365);
   Graph_Graph14->SetMaximum(2.828176);
   Graph_Graph14->SetDirectory(0);
   Graph_Graph14->SetStats(0);
   Graph_Graph14->SetLineWidth(2);
   Graph_Graph14->SetMarkerStyle(20);
   Graph_Graph14->SetMarkerSize(1.2);
   Graph_Graph14->GetXaxis()->SetLabelFont(42);
   Graph_Graph14->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph14->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph14->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph14->GetXaxis()->SetTitleFont(42);
   Graph_Graph14->GetYaxis()->SetLabelFont(42);
   Graph_Graph14->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph14->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph14->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph14->GetYaxis()->SetTitleFont(42);
   Graph_Graph14->GetZaxis()->SetLabelFont(42);
   Graph_Graph14->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph14->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph14->GetZaxis()->SetTitleOffset(1);
   Graph_Graph14->GetZaxis()->SetTitleFont(42);
   graph->SetHistogram(Graph_Graph14);
   
   graph->Draw("c");
   
   Double_t Graph2_fx5[12] = {
   0,
   0.2631,
   0.5,
   0.75,
   1,
   1.5,
   2,
   2.5,
   3,
   4,
   5,
   6};
   Double_t Graph2_fy5[12] = {
   0.602817,
   0.670423,
   0.749296,
   0.811268,
   0.859155,
   0.926761,
   0.957746,
   0.952113,
   0.898592,
   0.71831,
   0.512676,
   0.349296};
   graph = new TGraph(12,Graph2_fx5,Graph2_fy5);
   graph->SetName("Graph2");
   graph->SetTitle("Graph");
   graph->SetFillStyle(1000);
   graph->SetLineColor(2);
   graph->SetLineStyle(2);
   graph->SetLineWidth(2);
   graph->SetMarkerStyle(20);
   graph->SetMarkerSize(1.2);
   
   TH1F *Graph_Graph25 = new TH1F("Graph_Graph25","Graph",100,0,6.6);
   Graph_Graph25->SetMinimum(0.288451);
   Graph_Graph25->SetMaximum(1.018591);
   Graph_Graph25->SetDirectory(0);
   Graph_Graph25->SetStats(0);
   Graph_Graph25->SetLineWidth(2);
   Graph_Graph25->SetMarkerStyle(20);
   Graph_Graph25->SetMarkerSize(1.2);
   Graph_Graph25->GetXaxis()->SetLabelFont(42);
   Graph_Graph25->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph25->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph25->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph25->GetXaxis()->SetTitleFont(42);
   Graph_Graph25->GetYaxis()->SetLabelFont(42);
   Graph_Graph25->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph25->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph25->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph25->GetYaxis()->SetTitleFont(42);
   Graph_Graph25->GetZaxis()->SetLabelFont(42);
   Graph_Graph25->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph25->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph25->GetZaxis()->SetTitleOffset(1);
   Graph_Graph25->GetZaxis()->SetTitleFont(42);
   graph->SetHistogram(Graph_Graph25);
   
   graph->Draw("c");
   
   Double_t Graph3_fx6[12] = {
   0,
   0.2631,
   0.5,
   0.75,
   1,
   1.5,
   2,
   2.5,
   3,
   4,
   5,
   6};
   Double_t Graph3_fy6[12] = {
   1.25634,
   1.3662,
   1.42535,
   1.4507,
   1.4338,
   1.36338,
   1.26761,
   1.15775,
   1.02817,
   0.76338,
   0.532394,
   0.357746};
   graph = new TGraph(12,Graph3_fx6,Graph3_fy6);
   graph->SetName("Graph3");
   graph->SetTitle("Graph");
   graph->SetFillStyle(1000);
   graph->SetLineColor(2);
   graph->SetLineWidth(2);
   graph->SetMarkerStyle(20);
   graph->SetMarkerSize(1.2);
   
   TH1F *Graph_Graph36 = new TH1F("Graph_Graph36","Graph",100,0,6.6);
   Graph_Graph36->SetMinimum(0.2484506);
   Graph_Graph36->SetMaximum(1.559995);
   Graph_Graph36->SetDirectory(0);
   Graph_Graph36->SetStats(0);
   Graph_Graph36->SetLineWidth(2);
   Graph_Graph36->SetMarkerStyle(20);
   Graph_Graph36->SetMarkerSize(1.2);
   Graph_Graph36->GetXaxis()->SetLabelFont(42);
   Graph_Graph36->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph36->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph36->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph36->GetXaxis()->SetTitleFont(42);
   Graph_Graph36->GetYaxis()->SetLabelFont(42);
   Graph_Graph36->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph36->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph36->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph36->GetYaxis()->SetTitleFont(42);
   Graph_Graph36->GetZaxis()->SetLabelFont(42);
   Graph_Graph36->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph36->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph36->GetZaxis()->SetTitleOffset(1);
   Graph_Graph36->GetZaxis()->SetTitleFont(42);
   graph->SetHistogram(Graph_Graph36);
   
   graph->Draw("c");
   
   Double_t Graph4_fx7[14] = {
   0.11344,
   0.49159,
   0.99579,
   1.5,
   2.0042,
   2.5084,
   3,
   3.5042,
   3.9958,
   4.5,
   5.0042,
   5.5084,
   6,
   6.30252};
   Double_t Graph4_fy7[14] = {
   1.06143,
   1.10996,
   1.25051,
   1.44071,
   1.59913,
   1.62314,
   1.4843,
   1.21379,
   0.942128,
   0.725841,
   0.580436,
   0.511373,
   0.481779,
   0.464159};
   graph = new TGraph(14,Graph4_fx7,Graph4_fy7);
   graph->SetName("Graph4");
   graph->SetTitle("dat/Tsinghua_LcD0_1_AuAu200GeV_10_80.txt");
   graph->SetFillStyle(1000);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#66cc66");
   graph->SetLineColor(ci);
   graph->SetLineStyle(2);
   graph->SetLineWidth(2);
   graph->SetMarkerStyle(20);
   graph->SetMarkerSize(1.2);
   
   TH1F *Graph_Graph47 = new TH1F("Graph_Graph47","dat/Tsinghua_LcD0_1_AuAu200GeV_10_80.txt",100,0,6.921428);
   Graph_Graph47->SetMinimum(0.3482609);
   Graph_Graph47->SetMaximum(1.739038);
   Graph_Graph47->SetDirectory(0);
   Graph_Graph47->SetStats(0);
   Graph_Graph47->SetLineWidth(2);
   Graph_Graph47->SetMarkerStyle(20);
   Graph_Graph47->SetMarkerSize(1.2);
   Graph_Graph47->GetXaxis()->SetLabelFont(42);
   Graph_Graph47->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph47->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph47->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph47->GetXaxis()->SetTitleFont(42);
   Graph_Graph47->GetYaxis()->SetLabelFont(42);
   Graph_Graph47->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph47->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph47->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph47->GetYaxis()->SetTitleFont(42);
   Graph_Graph47->GetZaxis()->SetLabelFont(42);
   Graph_Graph47->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph47->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph47->GetZaxis()->SetTitleOffset(1);
   Graph_Graph47->GetZaxis()->SetTitleFont(42);
   graph->SetHistogram(Graph_Graph47);
   
   graph->Draw("c");
   
   Double_t Graph5_fx8[14] = {
   0.10084,
   0.5042,
   1.0084,
   1.5,
   2.0042,
   2.5084,
   3,
   3.5042,
   4.0084,
   4.5,
   5.0042,
   5.4958,
   6,
   6.28992};
   Double_t Graph5_fy8[14] = {
   1.77497,
   1.884,
   2.17055,
   2.59561,
   3.12713,
   3.6842,
   3.91051,
   3.6297,
   2.94616,
   2.32111,
   1.941,
   1.71005,
   1.63528,
   1.61109};
   graph = new TGraph(14,Graph5_fx8,Graph5_fy8);
   graph->SetName("Graph5");
   graph->SetTitle("dat/Tsinghua_LcD0_2_AuAu200GeV_10_80.txt");
   graph->SetFillStyle(1000);

   ci = TColor::GetColor("#66cc66");
   graph->SetLineColor(ci);
   graph->SetLineWidth(2);
   graph->SetMarkerStyle(20);
   graph->SetMarkerSize(1.2);
   
   TH1F *Graph_Graph58 = new TH1F("Graph_Graph58","dat/Tsinghua_LcD0_2_AuAu200GeV_10_80.txt",100,0,6.908828);
   Graph_Graph58->SetMinimum(1.381148);
   Graph_Graph58->SetMaximum(4.140452);
   Graph_Graph58->SetDirectory(0);
   Graph_Graph58->SetStats(0);
   Graph_Graph58->SetLineWidth(2);
   Graph_Graph58->SetMarkerStyle(20);
   Graph_Graph58->SetMarkerSize(1.2);
   Graph_Graph58->GetXaxis()->SetLabelFont(42);
   Graph_Graph58->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph58->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph58->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph58->GetXaxis()->SetTitleFont(42);
   Graph_Graph58->GetYaxis()->SetLabelFont(42);
   Graph_Graph58->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph58->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph58->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph58->GetYaxis()->SetTitleFont(42);
   Graph_Graph58->GetZaxis()->SetLabelFont(42);
   Graph_Graph58->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph58->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph58->GetZaxis()->SetTitleOffset(1);
   Graph_Graph58->GetZaxis()->SetTitleFont(42);
   graph->SetHistogram(Graph_Graph58);
   
   graph->Draw("c");
   
   Double_t Graph6_fx3001[1] = {
   3.96};
   Double_t Graph6_fy3001[1] = {
   1.0772};
   Double_t Graph6_felx3001[1] = {
   0.1};
   Double_t Graph6_fely3001[1] = {
   0.2647};
   Double_t Graph6_fehx3001[1] = {
   0.1};
   Double_t Graph6_fehy3001[1] = {
   0.2647};
   TGraphAsymmErrors *grae = new TGraphAsymmErrors(1,Graph6_fx3001,Graph6_fy3001,Graph6_felx3001,Graph6_fehx3001,Graph6_fely3001,Graph6_fehy3001);
   grae->SetName("Graph6");
   grae->SetTitle("Graph");

   ci = TColor::GetColor("#ffcccc");
   grae->SetFillColor(ci);
   grae->SetFillStyle(1000);

   ci = TColor::GetColor("#cc6666");
   grae->SetLineColor(ci);
   grae->SetLineWidth(4);

   ci = TColor::GetColor("#cc6666");
   grae->SetMarkerColor(ci);
   grae->SetMarkerStyle(29);
   grae->SetMarkerSize(2.5);
   
   TH1F *Graph_Graph63001 = new TH1F("Graph_Graph63001","Graph",100,3.84,4.08);
   Graph_Graph63001->SetMinimum(0.75956);
   Graph_Graph63001->SetMaximum(1.39484);
   Graph_Graph63001->SetDirectory(0);
   Graph_Graph63001->SetStats(0);
   Graph_Graph63001->SetLineWidth(2);
   Graph_Graph63001->SetMarkerStyle(20);
   Graph_Graph63001->SetMarkerSize(1.2);
   Graph_Graph63001->GetXaxis()->SetLabelFont(42);
   Graph_Graph63001->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph63001->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph63001->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph63001->GetXaxis()->SetTitleFont(42);
   Graph_Graph63001->GetYaxis()->SetLabelFont(42);
   Graph_Graph63001->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph63001->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph63001->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph63001->GetYaxis()->SetTitleFont(42);
   Graph_Graph63001->GetZaxis()->SetLabelFont(42);
   Graph_Graph63001->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph63001->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph63001->GetZaxis()->SetTitleOffset(1);
   Graph_Graph63001->GetZaxis()->SetTitleFont(42);
   grae->SetHistogram(Graph_Graph63001);
   
   grae->Draw("2");
   
   Double_t Graph7_fx1001[1] = {
   3.96};
   Double_t Graph7_fy1001[1] = {
   1.0772};
   Double_t Graph7_fex1001[1] = {
   0};
   Double_t Graph7_fey1001[1] = {
   0.1643};
   TGraphErrors *gre = new TGraphErrors(1,Graph7_fx1001,Graph7_fy1001,Graph7_fex1001,Graph7_fey1001);
   gre->SetName("Graph7");
   gre->SetTitle("Graph");
   gre->SetFillStyle(1000);

   ci = TColor::GetColor("#cc6666");
   gre->SetLineColor(ci);
   gre->SetLineWidth(4);

   ci = TColor::GetColor("#cc6666");
   gre->SetMarkerColor(ci);
   gre->SetMarkerStyle(29);
   gre->SetMarkerSize(2.5);
   
   TH1F *Graph_Graph71001 = new TH1F("Graph_Graph71001","Graph",100,3.86,5.06);
   Graph_Graph71001->SetMinimum(0.88004);
   Graph_Graph71001->SetMaximum(1.27436);
   Graph_Graph71001->SetDirectory(0);
   Graph_Graph71001->SetStats(0);
   Graph_Graph71001->SetLineWidth(2);
   Graph_Graph71001->SetMarkerStyle(20);
   Graph_Graph71001->SetMarkerSize(1.2);
   Graph_Graph71001->GetXaxis()->SetLabelFont(42);
   Graph_Graph71001->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph71001->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph71001->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph71001->GetXaxis()->SetTitleFont(42);
   Graph_Graph71001->GetYaxis()->SetLabelFont(42);
   Graph_Graph71001->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph71001->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph71001->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph71001->GetYaxis()->SetTitleFont(42);
   Graph_Graph71001->GetZaxis()->SetLabelFont(42);
   Graph_Graph71001->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph71001->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph71001->GetZaxis()->SetTitleOffset(1);
   Graph_Graph71001->GetZaxis()->SetTitleFont(42);
   gre->SetHistogram(Graph_Graph71001);
   
   gre->Draw("p");
   
   Double_t Graph8_fx1002[9] = {
   2.583333,
   3.125,
   3.375,
   3.625,
   3.875,
   4.25,
   4.75,
   5.5,
   7};
   Double_t Graph8_fy1002[9] = {
   1.853762,
   1.577412,
   1.449866,
   1.322319,
   1.194773,
   1.089611,
   1.006833,
   0.8826667,
   0.6343333};
   Double_t Graph8_fex1002[9] = {
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0};
   Double_t Graph8_fey1002[9] = {
   0.6454636,
   0.1177555,
   0.06851934,
   0.05566389,
   0.04170966,
   0.03544575,
   0.03332761,
   0.01970951,
   0.02262259};
   gre = new TGraphErrors(9,Graph8_fx1002,Graph8_fy1002,Graph8_fex1002,Graph8_fey1002);
   gre->SetName("Graph8");
   gre->SetTitle("Graph");
   gre->SetFillStyle(1000);

   ci = TColor::GetColor("#0000cc");
   gre->SetLineColor(ci);
   gre->SetLineWidth(4);

   ci = TColor::GetColor("#0000cc");
   gre->SetMarkerColor(ci);
   gre->SetMarkerStyle(20);
   gre->SetMarkerSize(2);
   
   TH1F *Graph_Graph81002 = new TH1F("Graph_Graph81002","Graph",100,2.141667,7.441667);
   Graph_Graph81002->SetMinimum(0.4229592);
   Graph_Graph81002->SetMaximum(2.687977);
   Graph_Graph81002->SetDirectory(0);
   Graph_Graph81002->SetStats(0);
   Graph_Graph81002->SetLineWidth(2);
   Graph_Graph81002->SetMarkerStyle(20);
   Graph_Graph81002->SetMarkerSize(1.2);
   Graph_Graph81002->GetXaxis()->SetLabelFont(42);
   Graph_Graph81002->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph81002->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph81002->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph81002->GetXaxis()->SetTitleFont(42);
   Graph_Graph81002->GetYaxis()->SetLabelFont(42);
   Graph_Graph81002->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph81002->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph81002->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph81002->GetYaxis()->SetTitleFont(42);
   Graph_Graph81002->GetZaxis()->SetLabelFont(42);
   Graph_Graph81002->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph81002->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph81002->GetZaxis()->SetTitleOffset(1);
   Graph_Graph81002->GetZaxis()->SetTitleFont(42);
   gre->SetHistogram(Graph_Graph81002);
   
   gre->Draw("p");
   
   Double_t Graph9_fx1003[9] = {
   2.583333,
   3.125,
   3.375,
   3.625,
   3.875,
   4.25,
   4.75,
   5.5,
   7};
   Double_t Graph9_fy1003[9] = {
   0.4738302,
   0.4534402,
   0.4425949,
   0.4293281,
   0.4172762,
   0.4028348,
   0.3699746,
   0.3312994,
   0.2644519};
   Double_t Graph9_fex1003[9] = {
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0};
   Double_t Graph9_fey1003[9] = {
   0.09934981,
   0.03462986,
   0.03020602,
   0.03596083,
   0.03452022,
   0.03395578,
   0.04127387,
   0.03644076,
   0.05541404};
   gre = new TGraphErrors(9,Graph9_fx1003,Graph9_fy1003,Graph9_fex1003,Graph9_fey1003);
   gre->SetName("Graph9");
   gre->SetTitle("Graph");
   gre->SetFillStyle(1000);
   gre->SetLineWidth(4);
   gre->SetMarkerStyle(21);
   gre->SetMarkerSize(2);
   
   TH1F *Graph_Graph91003 = new TH1F("Graph_Graph91003","Graph",100,2.141667,7.441667);
   Graph_Graph91003->SetMinimum(0.1726237);
   Graph_Graph91003->SetMaximum(0.6095942);
   Graph_Graph91003->SetDirectory(0);
   Graph_Graph91003->SetStats(0);
   Graph_Graph91003->SetLineWidth(2);
   Graph_Graph91003->SetMarkerStyle(20);
   Graph_Graph91003->SetMarkerSize(1.2);
   Graph_Graph91003->GetXaxis()->SetLabelFont(42);
   Graph_Graph91003->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph91003->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph91003->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph91003->GetXaxis()->SetTitleFont(42);
   Graph_Graph91003->GetYaxis()->SetLabelFont(42);
   Graph_Graph91003->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph91003->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph91003->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph91003->GetYaxis()->SetTitleFont(42);
   Graph_Graph91003->GetZaxis()->SetLabelFont(42);
   Graph_Graph91003->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph91003->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph91003->GetZaxis()->SetTitleOffset(1);
   Graph_Graph91003->GetZaxis()->SetTitleFont(42);
   gre->SetHistogram(Graph_Graph91003);
   
   gre->Draw("p");
   
   TLegend *leg = new TLegend(0.1998067,0.688525,0.8001561,0.8282708,NULL,"brNDC");
   leg->SetBorderSize(0);
   leg->SetLineColor(1);
   leg->SetLineStyle(1);
   leg->SetLineWidth(1);
   leg->SetFillColor(0);
   leg->SetFillStyle(1001);
   TLegendEntry *entry=leg->AddEntry("Graph9","6.2 pb^{-1} str. #it{p}+#it{p}","pl");
   entry->SetLineColor(1);
   entry->SetLineStyle(1);
   entry->SetLineWidth(4);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(2);
   entry->SetTextFont(42);
   entry=leg->AddEntry("Graph8","21 nb^{-1} rec. Au+Au, 0-10%","pl");

   ci = TColor::GetColor("#0000cc");
   entry->SetLineColor(ci);
   entry->SetLineStyle(1);
   entry->SetLineWidth(4);

   ci = TColor::GetColor("#0000cc");
   entry->SetMarkerColor(ci);
   entry->SetMarkerStyle(20);
   entry->SetMarkerSize(2);
   entry->SetTextFont(42);
   entry=leg->AddEntry("Graph6","STAR, Au+Au, 0-20%, PRL#bf{124}","plf");

   ci = TColor::GetColor("#ffcccc");
   entry->SetFillColor(ci);
   entry->SetFillStyle(1000);

   ci = TColor::GetColor("#cc6666");
   entry->SetLineColor(ci);
   entry->SetLineStyle(1);
   entry->SetLineWidth(4);

   ci = TColor::GetColor("#cc6666");
   entry->SetMarkerColor(ci);
   entry->SetMarkerStyle(29);
   entry->SetMarkerSize(2.5);
   entry->SetTextFont(42);
   leg->Draw();
   
   leg = new TLegend(0.04741459,0.8467278,0.7472027,0.8968254,NULL,"brNDC");
   leg->SetBorderSize(0);
   leg->SetLineColor(1);
   leg->SetLineStyle(1);
   leg->SetLineWidth(1);
   leg->SetFillColor(0);
   leg->SetFillStyle(0);
   entry=leg->AddEntry("NULL","#it{#bf{sPHENIX}} BUP 2022, Years 1-3","");
   entry->SetLineColor(1);
   entry->SetLineStyle(1);
   entry->SetLineWidth(1);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   entry->SetTextFont(42);
   leg->Draw();
   
   leg = new TLegend(0.35,0.5,0.94,0.65,NULL,"brNDC");
   leg->SetBorderSize(0);
   leg->SetLineColor(1);
   leg->SetLineStyle(1);
   leg->SetLineWidth(1);
   leg->SetFillColor(0);
   leg->SetFillStyle(1001);
   entry=leg->AddEntry("Graph5","Tsinghua: simul. coal, 10-80%","l");

   ci = TColor::GetColor("#66cc66");
   entry->SetLineColor(ci);
   entry->SetLineStyle(1);
   entry->SetLineWidth(2);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   entry->SetTextFont(42);
   entry=leg->AddEntry("Graph4","Tsinghua: seq. coal, 10-80%","l");

   ci = TColor::GetColor("#66cc66");
   entry->SetLineColor(ci);
   entry->SetLineStyle(2);
   entry->SetLineWidth(2);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   entry->SetTextFont(42);
   entry=leg->AddEntry("Graph1","Catania: coal only, 0-20%","l");
   entry->SetLineColor(4);
   entry->SetLineStyle(1);
   entry->SetLineWidth(2);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   entry->SetTextFont(42);
   entry=leg->AddEntry("Graph0","Catania: coal+frag, 0-20%","l");
   entry->SetLineColor(4);
   entry->SetLineStyle(2);
   entry->SetLineWidth(2);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   entry->SetTextFont(42);
   leg->Draw();
   
   leg = new TLegend(0.5,0.35,0.94,0.5,NULL,"brNDC");
   leg->SetBorderSize(0);
   leg->SetLineColor(1);
   leg->SetLineStyle(1);
   leg->SetLineWidth(1);
   leg->SetFillColor(0);
   leg->SetFillStyle(1001);
   entry=leg->AddEntry("Graph3","TAMU: di-quark 0-5%","l");
   entry->SetLineColor(2);
   entry->SetLineStyle(1);
   entry->SetLineWidth(2);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   entry->SetTextFont(42);
   entry=leg->AddEntry("Graph2","TAMU: 3-quark 0-5%","l");
   entry->SetLineColor(2);
   entry->SetLineStyle(2);
   entry->SetLineWidth(2);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   entry->SetTextFont(42);
   entry=leg->AddEntry("Graph_from_LctoD0_pt_whole_rebin","PYTHIA8 (CR)","l");
   entry->SetLineColor(1);
   entry->SetLineStyle(1);
   entry->SetLineWidth(2);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   entry->SetTextFont(42);
   entry=leg->AddEntry("Graph_from_LctoD0_pt_whole_rebin","PYTHIA8 (Monash)","l");
   entry->SetLineColor(1);
   entry->SetLineStyle(2);
   entry->SetLineWidth(2);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   entry->SetTextFont(42);
   leg->Draw();
   Lc_BUP2022_AuAu_3yr_1->Modified();
   Lc_BUP2022_AuAu_3yr->cd();
   Lc_BUP2022_AuAu_3yr->Modified();
   Lc_BUP2022_AuAu_3yr->cd();
   Lc_BUP2022_AuAu_3yr->SetSelected(Lc_BUP2022_AuAu_3yr);
}
