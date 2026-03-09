void D0_BUP2020AuAu_0_10_significance_3yr()
{
//=========Macro generated from canvas: D0_BUP2020AuAu_0_10_significance_3yr/D0_BUP2020AuAu_0_10_significance_3yr
//=========  (Wed May 12 23:46:20 2021) by ROOT version 6.22/02
   TCanvas *D0_BUP2020AuAu_0_10_significance_3yr = new TCanvas("D0_BUP2020AuAu_0_10_significance_3yr", "D0_BUP2020AuAu_0_10_significance_3yr",0,0,1100,800);
   gStyle->SetOptFit(1);
   gStyle->SetOptStat(0);
   gStyle->SetOptTitle(0);
   D0_BUP2020AuAu_0_10_significance_3yr->SetHighLightColor(2);
   D0_BUP2020AuAu_0_10_significance_3yr->Range(0,0,1,1);
   D0_BUP2020AuAu_0_10_significance_3yr->SetFillColor(0);
   D0_BUP2020AuAu_0_10_significance_3yr->SetBorderMode(0);
   D0_BUP2020AuAu_0_10_significance_3yr->SetBorderSize(2);
   D0_BUP2020AuAu_0_10_significance_3yr->SetTickx(1);
   D0_BUP2020AuAu_0_10_significance_3yr->SetTicky(1);
   D0_BUP2020AuAu_0_10_significance_3yr->SetLeftMargin(0.16);
   D0_BUP2020AuAu_0_10_significance_3yr->SetRightMargin(0.05);
   D0_BUP2020AuAu_0_10_significance_3yr->SetTopMargin(0.05);
   D0_BUP2020AuAu_0_10_significance_3yr->SetBottomMargin(0.16);
   D0_BUP2020AuAu_0_10_significance_3yr->SetFrameBorderMode(0);
  
// ------------>Primitives in pad: D0_BUP2020AuAu_0_10_significance_3yr_1
   TPad *D0_BUP2020AuAu_0_10_significance_3yr_1 = new TPad("D0_BUP2020AuAu_0_10_significance_3yr_1", "D0_BUP2020AuAu_0_10_significance_3yr_1",0.01,0.01,0.49,0.99);
   D0_BUP2020AuAu_0_10_significance_3yr_1->Draw();
   D0_BUP2020AuAu_0_10_significance_3yr_1->cd();
   D0_BUP2020AuAu_0_10_significance_3yr_1->Range(-2.43038,-320.6286,12.75949,1683.3);
   D0_BUP2020AuAu_0_10_significance_3yr_1->SetFillColor(0);
   D0_BUP2020AuAu_0_10_significance_3yr_1->SetBorderMode(0);
   D0_BUP2020AuAu_0_10_significance_3yr_1->SetBorderSize(2);
   D0_BUP2020AuAu_0_10_significance_3yr_1->SetTickx(1);
   D0_BUP2020AuAu_0_10_significance_3yr_1->SetTicky(1);
   D0_BUP2020AuAu_0_10_significance_3yr_1->SetLeftMargin(0.16);
   D0_BUP2020AuAu_0_10_significance_3yr_1->SetRightMargin(0.05);
   D0_BUP2020AuAu_0_10_significance_3yr_1->SetTopMargin(0.05);
   D0_BUP2020AuAu_0_10_significance_3yr_1->SetBottomMargin(0.16);
   D0_BUP2020AuAu_0_10_significance_3yr_1->SetFrameBorderMode(0);
   D0_BUP2020AuAu_0_10_significance_3yr_1->SetFrameBorderMode(0);
   
   TH1F *hframe__7 = new TH1F("hframe__7","",1000,0,12);
   hframe__7->SetMinimum(0);
   hframe__7->SetMaximum(1583.104);
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
   
   Double_t Graph0_fx13[20] = {
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
   Double_t Graph0_fy13[20] = {
   452.7213,
   696.0787,
   1083.052,
   1319.253,
   1306.403,
   1142.592,
   832.6952,
   630.6431,
   468.637,
   354.0432,
   271.5012,
   203.7491,
   153.3749,
   114.7844,
   86.90932,
   65.23378,
   50.12622,
   37.89479,
   29.06995,
   22.41011};
   TGraph *graph = new TGraph(20,Graph0_fx13,Graph0_fy13);
   graph->SetName("Graph0");
   graph->SetTitle("Significance for N_Collision = 1.4128e+11 centrality_ncoll = 9.6020e+01, based on 0-10%, w/o TOF, 24B");
   graph->SetLineWidth(2);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#cc0000");
   graph->SetMarkerColor(ci);
   graph->SetMarkerStyle(20);
   graph->SetMarkerSize(1.2);
   
   TH1F *Graph_Graph013 = new TH1F("Graph_Graph013","Significance for N_Collision = 1.4128e+11 centrality_ncoll = 9.6020e+01, based on 0-10%, w/o TOF, 24B",100,0,10.7);
   Graph_Graph013->SetMinimum(26.2875);
   Graph_Graph013->SetMaximum(1583.104);
   Graph_Graph013->SetDirectory(0);
   Graph_Graph013->SetLineWidth(2);
   Graph_Graph013->SetMarkerStyle(20);
   Graph_Graph013->SetMarkerSize(1.2);
   Graph_Graph013->GetXaxis()->SetTitle(" p_{T} [GeV/c]");
   Graph_Graph013->GetXaxis()->SetRange(1,94);
   Graph_Graph013->GetXaxis()->SetLabelFont(42);
   Graph_Graph013->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph013->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph013->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph013->GetXaxis()->SetTitleFont(42);
   Graph_Graph013->GetYaxis()->SetTitle(" S / #sqrt{S + B}");
   Graph_Graph013->GetYaxis()->SetLabelFont(42);
   Graph_Graph013->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph013->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph013->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph013->GetYaxis()->SetTitleFont(42);
   Graph_Graph013->GetZaxis()->SetLabelFont(42);
   Graph_Graph013->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph013->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph013->GetZaxis()->SetTitleOffset(1);
   Graph_Graph013->GetZaxis()->SetTitleFont(42);
   graph->SetHistogram(Graph_Graph013);
   
   graph->Draw("p");
   D0_BUP2020AuAu_0_10_significance_3yr_1->Modified();
   D0_BUP2020AuAu_0_10_significance_3yr->cd();
  
// ------------>Primitives in pad: D0_BUP2020AuAu_0_10_significance_3yr_2
   TPad *D0_BUP2020AuAu_0_10_significance_3yr_2 = new TPad("D0_BUP2020AuAu_0_10_significance_3yr_2", "D0_BUP2020AuAu_0_10_significance_3yr_2",0.51,0.01,0.99,0.99);
   D0_BUP2020AuAu_0_10_significance_3yr_2->Draw();
   D0_BUP2020AuAu_0_10_significance_3yr_2->cd();
   D0_BUP2020AuAu_0_10_significance_3yr_2->Range(-2.43038,-14.37675,12.75949,75.47793);
   D0_BUP2020AuAu_0_10_significance_3yr_2->SetFillColor(0);
   D0_BUP2020AuAu_0_10_significance_3yr_2->SetBorderMode(0);
   D0_BUP2020AuAu_0_10_significance_3yr_2->SetBorderSize(2);
   D0_BUP2020AuAu_0_10_significance_3yr_2->SetTickx(1);
   D0_BUP2020AuAu_0_10_significance_3yr_2->SetTicky(1);
   D0_BUP2020AuAu_0_10_significance_3yr_2->SetLeftMargin(0.16);
   D0_BUP2020AuAu_0_10_significance_3yr_2->SetRightMargin(0.05);
   D0_BUP2020AuAu_0_10_significance_3yr_2->SetTopMargin(0.05);
   D0_BUP2020AuAu_0_10_significance_3yr_2->SetBottomMargin(0.16);
   D0_BUP2020AuAu_0_10_significance_3yr_2->SetFrameBorderMode(0);
   D0_BUP2020AuAu_0_10_significance_3yr_2->SetFrameBorderMode(0);
   
   TH1F *hframe__8 = new TH1F("hframe__8","",1000,0,12);
   hframe__8->SetMinimum(0);
   hframe__8->SetMaximum(70.9852);
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
   
   Double_t Graph0_fx14[10] = {
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
   Double_t Graph0_fy14[10] = {
   6.498993,
   12.81415,
   30.68814,
   52.24624,
   59.15433,
   53.23591,
   44.80608,
   31.86806,
   25.21887,
   18.33553};
   graph = new TGraph(10,Graph0_fx14,Graph0_fy14);
   graph->SetName("Graph0");
   graph->SetTitle("Significance for N_Collision = 1.4128e+11 centrality_ncoll = 9.6020e+01, based on 0-10%, w/o TOF, 24B");
   graph->SetLineWidth(2);

   ci = TColor::GetColor("#cc0000");
   graph->SetMarkerColor(ci);
   graph->SetMarkerStyle(20);
   graph->SetMarkerSize(1.2);
   
   TH1F *Graph_Graph014 = new TH1F("Graph_Graph014","Significance for N_Collision = 1.4128e+11 centrality_ncoll = 9.6020e+01, based on 0-10%, w/o TOF, 24B",100,0,10.4);
   Graph_Graph014->SetMinimum(1.607636);
   Graph_Graph014->SetMaximum(70.9852);
   Graph_Graph014->SetDirectory(0);
   Graph_Graph014->SetLineWidth(2);
   Graph_Graph014->SetMarkerStyle(20);
   Graph_Graph014->SetMarkerSize(1.2);
   Graph_Graph014->GetXaxis()->SetTitle(" p_{T} [GeV/c]");
   Graph_Graph014->GetXaxis()->SetRange(1,97);
   Graph_Graph014->GetXaxis()->SetLabelFont(42);
   Graph_Graph014->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph014->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph014->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph014->GetXaxis()->SetTitleFont(42);
   Graph_Graph014->GetYaxis()->SetTitle(" S / #sqrt{S + B}");
   Graph_Graph014->GetYaxis()->SetLabelFont(42);
   Graph_Graph014->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph014->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph014->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph014->GetYaxis()->SetTitleFont(42);
   Graph_Graph014->GetZaxis()->SetLabelFont(42);
   Graph_Graph014->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph014->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph014->GetZaxis()->SetTitleOffset(1);
   Graph_Graph014->GetZaxis()->SetTitleFont(42);
   graph->SetHistogram(Graph_Graph014);
   
   graph->Draw("p");
   D0_BUP2020AuAu_0_10_significance_3yr_2->Modified();
   D0_BUP2020AuAu_0_10_significance_3yr->cd();
   D0_BUP2020AuAu_0_10_significance_3yr->Modified();
   D0_BUP2020AuAu_0_10_significance_3yr->cd();
   D0_BUP2020AuAu_0_10_significance_3yr->SetSelected(D0_BUP2020AuAu_0_10_significance_3yr);
}
