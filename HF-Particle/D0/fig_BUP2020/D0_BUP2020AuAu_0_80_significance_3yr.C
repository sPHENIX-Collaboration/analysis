void D0_BUP2020AuAu_0_80_significance_3yr()
{
//=========Macro generated from canvas: D0_BUP2020AuAu_0_80_significance_3yr/D0_BUP2020AuAu_0_80_significance_3yr
//=========  (Wed May 12 23:46:21 2021) by ROOT version 6.22/02
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
   
   TH1F *hframe__11 = new TH1F("hframe__11","",1000,0,12);
   hframe__11->SetMinimum(0);
   hframe__11->SetMaximum(2629.306);
   hframe__11->SetDirectory(0);
   hframe__11->SetStats(0);
   hframe__11->SetLineWidth(2);
   hframe__11->SetMarkerStyle(20);
   hframe__11->SetMarkerSize(1.2);
   hframe__11->GetXaxis()->SetTitle("p_{T} [GeV]");
   hframe__11->GetXaxis()->SetLabelFont(42);
   hframe__11->GetXaxis()->SetLabelSize(0.05);
   hframe__11->GetXaxis()->SetTitleSize(0.05);
   hframe__11->GetXaxis()->SetTitleOffset(1.4);
   hframe__11->GetXaxis()->SetTitleFont(42);
   hframe__11->GetYaxis()->SetTitle("Significance");
   hframe__11->GetYaxis()->SetLabelFont(42);
   hframe__11->GetYaxis()->SetLabelSize(0.05);
   hframe__11->GetYaxis()->SetTitleSize(0.05);
   hframe__11->GetYaxis()->SetTitleOffset(1.4);
   hframe__11->GetYaxis()->SetTitleFont(42);
   hframe__11->GetZaxis()->SetLabelFont(42);
   hframe__11->GetZaxis()->SetLabelSize(0.05);
   hframe__11->GetZaxis()->SetTitleSize(0.05);
   hframe__11->GetZaxis()->SetTitleOffset(1);
   hframe__11->GetZaxis()->SetTitleFont(42);
   hframe__11->Draw(" ");
   
   Double_t Graph0_fx17[20] = {
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
   Double_t Graph0_fy17[20] = {
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
   TGraph *graph = new TGraph(20,Graph0_fx17,Graph0_fy17);
   graph->SetName("Graph0");
   graph->SetTitle("Significance for N_Collision = 1.4128e+11 centrality_ncoll = 1.9080e+02, based on 0-80%, w/o TOF, 192B");
   graph->SetLineWidth(2);
   graph->SetMarkerStyle(20);
   graph->SetMarkerSize(1.2);
   
   TH1F *Graph_Graph017 = new TH1F("Graph_Graph017","Significance for N_Collision = 1.4128e+11 centrality_ncoll = 1.9080e+02, based on 0-80%, w/o TOF, 192B",100,0,10.7);
   Graph_Graph017->SetMinimum(59.2166);
   Graph_Graph017->SetMaximum(2629.306);
   Graph_Graph017->SetDirectory(0);
   Graph_Graph017->SetLineWidth(2);
   Graph_Graph017->SetMarkerStyle(20);
   Graph_Graph017->SetMarkerSize(1.2);
   Graph_Graph017->GetXaxis()->SetTitle(" p_{T} [GeV/c]");
   Graph_Graph017->GetXaxis()->SetRange(1,94);
   Graph_Graph017->GetXaxis()->SetLabelFont(42);
   Graph_Graph017->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph017->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph017->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph017->GetXaxis()->SetTitleFont(42);
   Graph_Graph017->GetYaxis()->SetTitle(" S / #sqrt{S + B}");
   Graph_Graph017->GetYaxis()->SetLabelFont(42);
   Graph_Graph017->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph017->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph017->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph017->GetYaxis()->SetTitleFont(42);
   Graph_Graph017->GetZaxis()->SetLabelFont(42);
   Graph_Graph017->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph017->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph017->GetZaxis()->SetTitleOffset(1);
   Graph_Graph017->GetZaxis()->SetTitleFont(42);
   graph->SetHistogram(Graph_Graph017);
   
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
   
   TH1F *hframe__12 = new TH1F("hframe__12","",1000,0,12);
   hframe__12->SetMinimum(0);
   hframe__12->SetMaximum(115.5003);
   hframe__12->SetDirectory(0);
   hframe__12->SetStats(0);
   hframe__12->SetLineWidth(2);
   hframe__12->SetMarkerStyle(20);
   hframe__12->SetMarkerSize(1.2);
   hframe__12->GetXaxis()->SetTitle("p_{T} [GeV]");
   hframe__12->GetXaxis()->SetLabelFont(42);
   hframe__12->GetXaxis()->SetLabelSize(0.05);
   hframe__12->GetXaxis()->SetTitleSize(0.05);
   hframe__12->GetXaxis()->SetTitleOffset(1.4);
   hframe__12->GetXaxis()->SetTitleFont(42);
   hframe__12->GetYaxis()->SetTitle("Significance");
   hframe__12->GetYaxis()->SetLabelFont(42);
   hframe__12->GetYaxis()->SetLabelSize(0.05);
   hframe__12->GetYaxis()->SetTitleSize(0.05);
   hframe__12->GetYaxis()->SetTitleOffset(1.4);
   hframe__12->GetYaxis()->SetTitleFont(42);
   hframe__12->GetZaxis()->SetLabelFont(42);
   hframe__12->GetZaxis()->SetLabelSize(0.05);
   hframe__12->GetZaxis()->SetTitleSize(0.05);
   hframe__12->GetZaxis()->SetTitleOffset(1);
   hframe__12->GetZaxis()->SetTitleFont(42);
   hframe__12->Draw(" ");
   
   Double_t Graph0_fx18[10] = {
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
   Double_t Graph0_fy18[10] = {
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
   graph = new TGraph(10,Graph0_fx18,Graph0_fy18);
   graph->SetName("Graph0");
   graph->SetTitle("Significance for N_Collision = 1.4128e+11 centrality_ncoll = 1.9080e+02, based on 0-80%, w/o TOF, 192B");
   graph->SetLineWidth(2);
   graph->SetMarkerStyle(20);
   graph->SetMarkerSize(1.2);
   
   TH1F *Graph_Graph018 = new TH1F("Graph_Graph018","Significance for N_Collision = 1.4128e+11 centrality_ncoll = 1.9080e+02, based on 0-80%, w/o TOF, 192B",100,0,10.4);
   Graph_Graph018->SetMinimum(3.873636);
   Graph_Graph018->SetMaximum(115.5003);
   Graph_Graph018->SetDirectory(0);
   Graph_Graph018->SetLineWidth(2);
   Graph_Graph018->SetMarkerStyle(20);
   Graph_Graph018->SetMarkerSize(1.2);
   Graph_Graph018->GetXaxis()->SetTitle(" p_{T} [GeV/c]");
   Graph_Graph018->GetXaxis()->SetRange(1,97);
   Graph_Graph018->GetXaxis()->SetLabelFont(42);
   Graph_Graph018->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph018->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph018->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph018->GetXaxis()->SetTitleFont(42);
   Graph_Graph018->GetYaxis()->SetTitle(" S / #sqrt{S + B}");
   Graph_Graph018->GetYaxis()->SetLabelFont(42);
   Graph_Graph018->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph018->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph018->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph018->GetYaxis()->SetTitleFont(42);
   Graph_Graph018->GetZaxis()->SetLabelFont(42);
   Graph_Graph018->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph018->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph018->GetZaxis()->SetTitleOffset(1);
   Graph_Graph018->GetZaxis()->SetTitleFont(42);
   graph->SetHistogram(Graph_Graph018);
   
   graph->Draw("p");
   D0_BUP2020AuAu_0_80_significance_3yr_2->Modified();
   D0_BUP2020AuAu_0_80_significance_3yr->cd();
   D0_BUP2020AuAu_0_80_significance_3yr->Modified();
   D0_BUP2020AuAu_0_80_significance_3yr->cd();
   D0_BUP2020AuAu_0_80_significance_3yr->SetSelected(D0_BUP2020AuAu_0_80_significance_3yr);
}
