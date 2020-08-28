void D0_BUP2020AuAu_0_80_significance_3yr()
{
//=========Macro generated from canvas: D0_BUP2020AuAu_0_80_significance_3yr/D0_BUP2020AuAu_0_80_significance_3yr
//=========  (Fri Aug 28 14:16:00 2020) by ROOT version 6.16/00
   TCanvas *D0_BUP2020AuAu_0_80_significance_3yr = new TCanvas("D0_BUP2020AuAu_0_80_significance_3yr", "D0_BUP2020AuAu_0_80_significance_3yr",0,0,1100,800);
   gStyle->SetOptFit(1);
   gStyle->SetOptStat(0);
   gStyle->SetOptTitle(0);
   D0_BUP2020AuAu_0_80_significance_3yr->SetHighLightColor(2);
   D0_BUP2020AuAu_0_80_significance_3yr->Range(0,0,1,1);
   D0_BUP2020AuAu_0_80_significance_3yr->SetFillColor(0);
   D0_BUP2020AuAu_0_80_significance_3yr->SetBorderMode(0);
   D0_BUP2020AuAu_0_80_significance_3yr->SetBorderSize(2);
   D0_BUP2020AuAu_0_80_significance_3yr->SetTickx(1);
   D0_BUP2020AuAu_0_80_significance_3yr->SetTicky(1);
   D0_BUP2020AuAu_0_80_significance_3yr->SetLeftMargin(0.16);
   D0_BUP2020AuAu_0_80_significance_3yr->SetRightMargin(0.05);
   D0_BUP2020AuAu_0_80_significance_3yr->SetTopMargin(0.05);
   D0_BUP2020AuAu_0_80_significance_3yr->SetBottomMargin(0.16);
   D0_BUP2020AuAu_0_80_significance_3yr->SetFrameBorderMode(0);
  
// ------------>Primitives in pad: D0_BUP2020AuAu_0_80_significance_3yr_1
   TPad *D0_BUP2020AuAu_0_80_significance_3yr_1 = new TPad("D0_BUP2020AuAu_0_80_significance_3yr_1", "D0_BUP2020AuAu_0_80_significance_3yr_1",0.01,0.01,0.49,0.99);
   D0_BUP2020AuAu_0_80_significance_3yr_1->Draw();
   D0_BUP2020AuAu_0_80_significance_3yr_1->cd();
   D0_BUP2020AuAu_0_80_significance_3yr_1->Range(-2.43038,-532.5177,12.75949,2795.718);
   D0_BUP2020AuAu_0_80_significance_3yr_1->SetFillColor(0);
   D0_BUP2020AuAu_0_80_significance_3yr_1->SetBorderMode(0);
   D0_BUP2020AuAu_0_80_significance_3yr_1->SetBorderSize(2);
   D0_BUP2020AuAu_0_80_significance_3yr_1->SetTickx(1);
   D0_BUP2020AuAu_0_80_significance_3yr_1->SetTicky(1);
   D0_BUP2020AuAu_0_80_significance_3yr_1->SetLeftMargin(0.16);
   D0_BUP2020AuAu_0_80_significance_3yr_1->SetRightMargin(0.05);
   D0_BUP2020AuAu_0_80_significance_3yr_1->SetTopMargin(0.05);
   D0_BUP2020AuAu_0_80_significance_3yr_1->SetBottomMargin(0.16);
   D0_BUP2020AuAu_0_80_significance_3yr_1->SetFrameBorderMode(0);
   D0_BUP2020AuAu_0_80_significance_3yr_1->SetFrameBorderMode(0);
   
   TH1F *hframe__7 = new TH1F("hframe__7","",1000,0,12);
   hframe__7->SetMinimum(0);
   hframe__7->SetMaximum(2629.306);
   hframe__7->SetDirectory(0);
   hframe__7->SetStats(0);
   hframe__7->SetLineWidth(2);
   hframe__7->SetMarkerStyle(20);
   hframe__7->SetMarkerSize(1.2);
   hframe__7->GetXaxis()->SetTitle("p_{T} [GeV]");
   hframe__7->GetXaxis()->SetLabelFont(42);
   hframe__7->GetXaxis()->SetLabelSize(0.05);
   hframe__7->GetXaxis()->SetTitleSize(0.05);
   hframe__7->GetXaxis()->SetTitleOffset(1.4);
   hframe__7->GetXaxis()->SetTitleFont(42);
   hframe__7->GetYaxis()->SetTitle("Significance");
   hframe__7->GetYaxis()->SetLabelFont(42);
   hframe__7->GetYaxis()->SetLabelSize(0.05);
   hframe__7->GetYaxis()->SetTitleSize(0.05);
   hframe__7->GetYaxis()->SetTitleOffset(1.4);
   hframe__7->GetYaxis()->SetTitleFont(42);
   hframe__7->GetZaxis()->SetLabelFont(42);
   hframe__7->GetZaxis()->SetLabelSize(0.05);
   hframe__7->GetZaxis()->SetTitleSize(0.05);
   hframe__7->GetZaxis()->SetTitleOffset(1);
   hframe__7->GetZaxis()->SetTitleFont(42);
   hframe__7->Draw(" ");
   
   Double_t Graph0_fx7[20] = {
   0.25,
   0.75,
   1.25,
   1.75,
   2.25,
   2.75,
   3.25,
   3.75,
   4.25,
   4.75,
   5.25,
   5.75,
   6.25,
   6.75,
   7.25,
   7.75,
   8.25,
   8.75,
   9.25,
   9.75};
   Double_t Graph0_fy7[20] = {
   735.8376,
   1157.68,
   1779.061,
   2177.719,
   2191.089,
   1952.555,
   1437.194,
   1113.006,
   847.1249,
   651.6253,
   509.2994,
   390.4004,
   298.3194,
   228.1737,
   177.2215,
   135.9347,
   104.7474,
   81.70937,
   63.73266,
   50.48219};
   TGraph *graph = new TGraph(20,Graph0_fx7,Graph0_fy7);
   graph->SetName("Graph0");
   graph->SetTitle("Significance for N_Collision = 1.4128e+11 centrality_ncoll = 1.9080e+02, based on 0-80%, w/o TOF, 192B; p_{T} [GeV/c]; S / #sqrt{S + B}");
   graph->SetLineWidth(2);
   graph->SetMarkerStyle(20);
   graph->SetMarkerSize(1.2);
   
   TH1F *Graph_Graph07 = new TH1F("Graph_Graph07","Significance for N_Collision = 1.4128e+11 centrality_ncoll = 1.9080e+02, based on 0-80%, w/o TOF, 192B",100,0,10.7);
   Graph_Graph07->SetMinimum(59.2166);
   Graph_Graph07->SetMaximum(2629.306);
   Graph_Graph07->SetDirectory(0);
   Graph_Graph07->SetLineWidth(2);
   Graph_Graph07->SetMarkerStyle(20);
   Graph_Graph07->SetMarkerSize(1.2);
   Graph_Graph07->GetXaxis()->SetTitle(" p_{T} [GeV/c]");
   Graph_Graph07->GetXaxis()->SetRange(1,94);
   Graph_Graph07->GetXaxis()->SetLabelFont(42);
   Graph_Graph07->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph07->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph07->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph07->GetXaxis()->SetTitleFont(42);
   Graph_Graph07->GetYaxis()->SetTitle(" S / #sqrt{S + B}");
   Graph_Graph07->GetYaxis()->SetLabelFont(42);
   Graph_Graph07->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph07->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph07->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph07->GetYaxis()->SetTitleFont(42);
   Graph_Graph07->GetZaxis()->SetLabelFont(42);
   Graph_Graph07->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph07->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph07->GetZaxis()->SetTitleOffset(1);
   Graph_Graph07->GetZaxis()->SetTitleFont(42);
   graph->SetHistogram(Graph_Graph07);
   
   graph->Draw("p");
   D0_BUP2020AuAu_0_80_significance_3yr_1->Modified();
   D0_BUP2020AuAu_0_80_significance_3yr->cd();
  
// ------------>Primitives in pad: D0_BUP2020AuAu_0_80_significance_3yr_2
   TPad *D0_BUP2020AuAu_0_80_significance_3yr_2 = new TPad("D0_BUP2020AuAu_0_80_significance_3yr_2", "D0_BUP2020AuAu_0_80_significance_3yr_2",0.51,0.01,0.99,0.99);
   D0_BUP2020AuAu_0_80_significance_3yr_2->Draw();
   D0_BUP2020AuAu_0_80_significance_3yr_2->cd();
   D0_BUP2020AuAu_0_80_significance_3yr_2->Range(-2.43038,-23.39246,12.75949,122.8104);
   D0_BUP2020AuAu_0_80_significance_3yr_2->SetFillColor(0);
   D0_BUP2020AuAu_0_80_significance_3yr_2->SetBorderMode(0);
   D0_BUP2020AuAu_0_80_significance_3yr_2->SetBorderSize(2);
   D0_BUP2020AuAu_0_80_significance_3yr_2->SetTickx(1);
   D0_BUP2020AuAu_0_80_significance_3yr_2->SetTicky(1);
   D0_BUP2020AuAu_0_80_significance_3yr_2->SetLeftMargin(0.16);
   D0_BUP2020AuAu_0_80_significance_3yr_2->SetRightMargin(0.05);
   D0_BUP2020AuAu_0_80_significance_3yr_2->SetTopMargin(0.05);
   D0_BUP2020AuAu_0_80_significance_3yr_2->SetBottomMargin(0.16);
   D0_BUP2020AuAu_0_80_significance_3yr_2->SetFrameBorderMode(0);
   D0_BUP2020AuAu_0_80_significance_3yr_2->SetFrameBorderMode(0);
   
   TH1F *hframe__8 = new TH1F("hframe__8","",1000,0,12);
   hframe__8->SetMinimum(0);
   hframe__8->SetMaximum(115.5003);
   hframe__8->SetDirectory(0);
   hframe__8->SetStats(0);
   hframe__8->SetLineWidth(2);
   hframe__8->SetMarkerStyle(20);
   hframe__8->SetMarkerSize(1.2);
   hframe__8->GetXaxis()->SetTitle("p_{T} [GeV]");
   hframe__8->GetXaxis()->SetLabelFont(42);
   hframe__8->GetXaxis()->SetLabelSize(0.05);
   hframe__8->GetXaxis()->SetTitleSize(0.05);
   hframe__8->GetXaxis()->SetTitleOffset(1.4);
   hframe__8->GetXaxis()->SetTitleFont(42);
   hframe__8->GetYaxis()->SetTitle("Significance");
   hframe__8->GetYaxis()->SetLabelFont(42);
   hframe__8->GetYaxis()->SetLabelSize(0.05);
   hframe__8->GetYaxis()->SetTitleSize(0.05);
   hframe__8->GetYaxis()->SetTitleOffset(1.4);
   hframe__8->GetYaxis()->SetTitleFont(42);
   hframe__8->GetZaxis()->SetLabelFont(42);
   hframe__8->GetZaxis()->SetLabelSize(0.05);
   hframe__8->GetZaxis()->SetTitleSize(0.05);
   hframe__8->GetZaxis()->SetTitleOffset(1);
   hframe__8->GetZaxis()->SetTitleFont(42);
   hframe__8->Draw(" ");
   
   Double_t Graph0_fx8[10] = {
   0.5,
   1.5,
   2.5,
   3.5,
   4.5,
   5.5,
   6.5,
   7.5,
   8.5,
   9.5};
   Double_t Graph0_fy8[10] = {
   11.45189,
   22.70898,
   53.76041,
   88.55232,
   96.25025,
   82.11165,
   66.0051,
   45.70327,
   35.28046,
   25.70073};
   graph = new TGraph(10,Graph0_fx8,Graph0_fy8);
   graph->SetName("Graph0");
   graph->SetTitle("Significance for N_Collision = 1.4128e+11 centrality_ncoll = 1.9080e+02, based on 0-80%, w/o TOF, 192B; p_{T} [GeV/c]; S / #sqrt{S + B}");
   graph->SetLineWidth(2);
   graph->SetMarkerStyle(20);
   graph->SetMarkerSize(1.2);
   
   TH1F *Graph_Graph08 = new TH1F("Graph_Graph08","Significance for N_Collision = 1.4128e+11 centrality_ncoll = 1.9080e+02, based on 0-80%, w/o TOF, 192B",100,0,10.4);
   Graph_Graph08->SetMinimum(3.873636);
   Graph_Graph08->SetMaximum(115.5003);
   Graph_Graph08->SetDirectory(0);
   Graph_Graph08->SetLineWidth(2);
   Graph_Graph08->SetMarkerStyle(20);
   Graph_Graph08->SetMarkerSize(1.2);
   Graph_Graph08->GetXaxis()->SetTitle(" p_{T} [GeV/c]");
   Graph_Graph08->GetXaxis()->SetRange(1,97);
   Graph_Graph08->GetXaxis()->SetLabelFont(42);
   Graph_Graph08->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph08->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph08->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph08->GetXaxis()->SetTitleFont(42);
   Graph_Graph08->GetYaxis()->SetTitle(" S / #sqrt{S + B}");
   Graph_Graph08->GetYaxis()->SetLabelFont(42);
   Graph_Graph08->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph08->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph08->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph08->GetYaxis()->SetTitleFont(42);
   Graph_Graph08->GetZaxis()->SetLabelFont(42);
   Graph_Graph08->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph08->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph08->GetZaxis()->SetTitleOffset(1);
   Graph_Graph08->GetZaxis()->SetTitleFont(42);
   graph->SetHistogram(Graph_Graph08);
   
   graph->Draw("p");
   D0_BUP2020AuAu_0_80_significance_3yr_2->Modified();
   D0_BUP2020AuAu_0_80_significance_3yr->cd();
   D0_BUP2020AuAu_0_80_significance_3yr->Modified();
   D0_BUP2020AuAu_0_80_significance_3yr->cd();
   D0_BUP2020AuAu_0_80_significance_3yr->SetSelected(D0_BUP2020AuAu_0_80_significance_3yr);
}
