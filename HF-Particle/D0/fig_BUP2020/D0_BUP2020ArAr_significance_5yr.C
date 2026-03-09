void D0_BUP2020ArAr_significance_5yr()
{
//=========Macro generated from canvas: D0_BUP2020ArAr_significance_5yr/D0_BUP2020ArAr_significance_5yr
//=========  (Wed May 12 23:46:24 2021) by ROOT version 6.22/02
   TCanvas *D0_BUP2020ArAr_significance_5yr = new TCanvas("D0_BUP2020ArAr_significance_5yr", "D0_BUP2020ArAr_significance_5yr",0,0,1100,800);
   gStyle->SetOptFit(1);
   gStyle->SetOptStat(0);
   gStyle->SetOptTitle(0);
   D0_BUP2020ArAr_significance_5yr->SetHighLightColor(2);
   D0_BUP2020ArAr_significance_5yr->Range(0,0,1,1);
   D0_BUP2020ArAr_significance_5yr->SetFillColor(0);
   D0_BUP2020ArAr_significance_5yr->SetBorderMode(0);
   D0_BUP2020ArAr_significance_5yr->SetBorderSize(2);
   D0_BUP2020ArAr_significance_5yr->SetTickx(1);
   D0_BUP2020ArAr_significance_5yr->SetTicky(1);
   D0_BUP2020ArAr_significance_5yr->SetLeftMargin(0.16);
   D0_BUP2020ArAr_significance_5yr->SetRightMargin(0.05);
   D0_BUP2020ArAr_significance_5yr->SetTopMargin(0.05);
   D0_BUP2020ArAr_significance_5yr->SetBottomMargin(0.16);
   D0_BUP2020ArAr_significance_5yr->SetFrameBorderMode(0);
  
// ------------>Primitives in pad: D0_BUP2020ArAr_significance_5yr_1
   TPad *D0_BUP2020ArAr_significance_5yr_1 = new TPad("D0_BUP2020ArAr_significance_5yr_1", "D0_BUP2020ArAr_significance_5yr_1",0.01,0.01,0.49,0.99);
   D0_BUP2020ArAr_significance_5yr_1->Draw();
   D0_BUP2020ArAr_significance_5yr_1->cd();
   D0_BUP2020ArAr_significance_5yr_1->Range(-2.43038,-131.5052,12.75949,690.4023);
   D0_BUP2020ArAr_significance_5yr_1->SetFillColor(0);
   D0_BUP2020ArAr_significance_5yr_1->SetBorderMode(0);
   D0_BUP2020ArAr_significance_5yr_1->SetBorderSize(2);
   D0_BUP2020ArAr_significance_5yr_1->SetTickx(1);
   D0_BUP2020ArAr_significance_5yr_1->SetTicky(1);
   D0_BUP2020ArAr_significance_5yr_1->SetLeftMargin(0.16);
   D0_BUP2020ArAr_significance_5yr_1->SetRightMargin(0.05);
   D0_BUP2020ArAr_significance_5yr_1->SetTopMargin(0.05);
   D0_BUP2020ArAr_significance_5yr_1->SetBottomMargin(0.16);
   D0_BUP2020ArAr_significance_5yr_1->SetFrameBorderMode(0);
   D0_BUP2020ArAr_significance_5yr_1->SetFrameBorderMode(0);
   
   TH1F *hframe__17 = new TH1F("hframe__17","",1000,0,12);
   hframe__17->SetMinimum(0);
   hframe__17->SetMaximum(649.307);
   hframe__17->SetDirectory(0);
   hframe__17->SetStats(0);
   hframe__17->SetLineWidth(2);
   hframe__17->SetMarkerStyle(20);
   hframe__17->SetMarkerSize(1.2);
   hframe__17->GetXaxis()->SetTitle("p_{T} [GeV]");
   hframe__17->GetXaxis()->SetLabelFont(42);
   hframe__17->GetXaxis()->SetLabelSize(0.05);
   hframe__17->GetXaxis()->SetTitleSize(0.05);
   hframe__17->GetXaxis()->SetTitleOffset(1.4);
   hframe__17->GetXaxis()->SetTitleFont(42);
   hframe__17->GetYaxis()->SetTitle("Significance");
   hframe__17->GetYaxis()->SetLabelFont(42);
   hframe__17->GetYaxis()->SetLabelSize(0.05);
   hframe__17->GetYaxis()->SetTitleSize(0.05);
   hframe__17->GetYaxis()->SetTitleOffset(1.4);
   hframe__17->GetYaxis()->SetTitleFont(42);
   hframe__17->GetZaxis()->SetLabelFont(42);
   hframe__17->GetZaxis()->SetLabelSize(0.05);
   hframe__17->GetZaxis()->SetTitleSize(0.05);
   hframe__17->GetZaxis()->SetTitleOffset(1);
   hframe__17->GetZaxis()->SetTitleFont(42);
   hframe__17->Draw(" ");
   
   Double_t Graph0_fx23[20] = {
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
   Double_t Graph0_fy23[20] = {
   222.3935,
   343.2285,
   501.669,
   541.0891,
   506.3098,
   433.4075,
   393.0577,
   309.3987,
   237.8936,
   179.1467,
   141.0528,
   109.2993,
   84.59671,
   65.7418,
   59.24393,
   45.92914,
   35.87951,
   28.27625,
   22.50334,
   18.03735};
   TGraph *graph = new TGraph(20,Graph0_fx23,Graph0_fy23);
   graph->SetName("Graph0");
   graph->SetTitle("Significance for N_Collision = 2.8108e+10 centrality_ncoll = 2.8500e+01, based on 60-80%, w/o TOF, 48B");
   graph->SetLineWidth(2);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#00cc00");
   graph->SetMarkerColor(ci);
   graph->SetMarkerStyle(20);
   graph->SetMarkerSize(1.2);
   
   TH1F *Graph_Graph023 = new TH1F("Graph_Graph023","Significance for N_Collision = 2.8108e+10 centrality_ncoll = 2.8500e+01, based on 60-80%, w/o TOF, 48B",100,0,10.7);
   Graph_Graph023->SetMinimum(18.29657);
   Graph_Graph023->SetMaximum(649.307);
   Graph_Graph023->SetDirectory(0);
   Graph_Graph023->SetLineWidth(2);
   Graph_Graph023->SetMarkerStyle(20);
   Graph_Graph023->SetMarkerSize(1.2);
   Graph_Graph023->GetXaxis()->SetTitle(" p_{T} [GeV/c]");
   Graph_Graph023->GetXaxis()->SetRange(1,94);
   Graph_Graph023->GetXaxis()->SetLabelFont(42);
   Graph_Graph023->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph023->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph023->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph023->GetXaxis()->SetTitleFont(42);
   Graph_Graph023->GetYaxis()->SetTitle(" S / #sqrt{S + B}");
   Graph_Graph023->GetYaxis()->SetLabelFont(42);
   Graph_Graph023->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph023->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph023->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph023->GetYaxis()->SetTitleFont(42);
   Graph_Graph023->GetZaxis()->SetLabelFont(42);
   Graph_Graph023->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph023->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph023->GetZaxis()->SetTitleOffset(1);
   Graph_Graph023->GetZaxis()->SetTitleFont(42);
   graph->SetHistogram(Graph_Graph023);
   
   graph->Draw("p");
   D0_BUP2020ArAr_significance_5yr_1->Modified();
   D0_BUP2020ArAr_significance_5yr->cd();
  
// ------------>Primitives in pad: D0_BUP2020ArAr_significance_5yr_2
   TPad *D0_BUP2020ArAr_significance_5yr_2 = new TPad("D0_BUP2020ArAr_significance_5yr_2", "D0_BUP2020ArAr_significance_5yr_2",0.51,0.01,0.99,0.99);
   D0_BUP2020ArAr_significance_5yr_2->Draw();
   D0_BUP2020ArAr_significance_5yr_2->cd();
   D0_BUP2020ArAr_significance_5yr_2->Range(-2.43038,-6.928021,12.75949,36.37211);
   D0_BUP2020ArAr_significance_5yr_2->SetFillColor(0);
   D0_BUP2020ArAr_significance_5yr_2->SetBorderMode(0);
   D0_BUP2020ArAr_significance_5yr_2->SetBorderSize(2);
   D0_BUP2020ArAr_significance_5yr_2->SetTickx(1);
   D0_BUP2020ArAr_significance_5yr_2->SetTicky(1);
   D0_BUP2020ArAr_significance_5yr_2->SetLeftMargin(0.16);
   D0_BUP2020ArAr_significance_5yr_2->SetRightMargin(0.05);
   D0_BUP2020ArAr_significance_5yr_2->SetTopMargin(0.05);
   D0_BUP2020ArAr_significance_5yr_2->SetBottomMargin(0.16);
   D0_BUP2020ArAr_significance_5yr_2->SetFrameBorderMode(0);
   D0_BUP2020ArAr_significance_5yr_2->SetFrameBorderMode(0);
   
   TH1F *hframe__18 = new TH1F("hframe__18","",1000,0,12);
   hframe__18->SetMinimum(0);
   hframe__18->SetMaximum(34.20711);
   hframe__18->SetDirectory(0);
   hframe__18->SetStats(0);
   hframe__18->SetLineWidth(2);
   hframe__18->SetMarkerStyle(20);
   hframe__18->SetMarkerSize(1.2);
   hframe__18->GetXaxis()->SetTitle("p_{T} [GeV]");
   hframe__18->GetXaxis()->SetLabelFont(42);
   hframe__18->GetXaxis()->SetLabelSize(0.05);
   hframe__18->GetXaxis()->SetTitleSize(0.05);
   hframe__18->GetXaxis()->SetTitleOffset(1.4);
   hframe__18->GetXaxis()->SetTitleFont(42);
   hframe__18->GetYaxis()->SetTitle("Significance");
   hframe__18->GetYaxis()->SetLabelFont(42);
   hframe__18->GetYaxis()->SetLabelSize(0.05);
   hframe__18->GetYaxis()->SetTitleSize(0.05);
   hframe__18->GetYaxis()->SetTitleOffset(1.4);
   hframe__18->GetYaxis()->SetTitleFont(42);
   hframe__18->GetZaxis()->SetLabelFont(42);
   hframe__18->GetZaxis()->SetLabelSize(0.05);
   hframe__18->GetZaxis()->SetTitleSize(0.05);
   hframe__18->GetZaxis()->SetTitleOffset(1);
   hframe__18->GetZaxis()->SetTitleFont(42);
   hframe__18->Draw(" ");
   
   Double_t Graph0_fx24[10] = {
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
   Double_t Graph0_fy24[10] = {
   5.835771,
   11.85677,
   24.78042,
   28.50592,
   27.43669,
   21.93302,
   16.5096,
   11.17191,
   8.522349,
   6.414102};
   graph = new TGraph(10,Graph0_fx24,Graph0_fy24);
   graph->SetName("Graph0");
   graph->SetTitle("Significance for N_Collision = 2.8108e+10 centrality_ncoll = 2.8500e+01, based on 60-80%, w/o TOF, 48B");
   graph->SetLineWidth(2);

   ci = TColor::GetColor("#00cc00");
   graph->SetMarkerColor(ci);
   graph->SetMarkerStyle(20);
   graph->SetMarkerSize(1.2);
   
   TH1F *Graph_Graph024 = new TH1F("Graph_Graph024","Significance for N_Collision = 2.8108e+10 centrality_ncoll = 2.8500e+01, based on 60-80%, w/o TOF, 48B",100,0,10.4);
   Graph_Graph024->SetMinimum(4.022269);
   Graph_Graph024->SetMaximum(34.20711);
   Graph_Graph024->SetDirectory(0);
   Graph_Graph024->SetLineWidth(2);
   Graph_Graph024->SetMarkerStyle(20);
   Graph_Graph024->SetMarkerSize(1.2);
   Graph_Graph024->GetXaxis()->SetTitle(" p_{T} [GeV/c]");
   Graph_Graph024->GetXaxis()->SetRange(1,97);
   Graph_Graph024->GetXaxis()->SetLabelFont(42);
   Graph_Graph024->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph024->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph024->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph024->GetXaxis()->SetTitleFont(42);
   Graph_Graph024->GetYaxis()->SetTitle(" S / #sqrt{S + B}");
   Graph_Graph024->GetYaxis()->SetLabelFont(42);
   Graph_Graph024->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph024->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph024->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph024->GetYaxis()->SetTitleFont(42);
   Graph_Graph024->GetZaxis()->SetLabelFont(42);
   Graph_Graph024->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph024->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph024->GetZaxis()->SetTitleOffset(1);
   Graph_Graph024->GetZaxis()->SetTitleFont(42);
   graph->SetHistogram(Graph_Graph024);
   
   graph->Draw("p");
   D0_BUP2020ArAr_significance_5yr_2->Modified();
   D0_BUP2020ArAr_significance_5yr->cd();
   D0_BUP2020ArAr_significance_5yr->Modified();
   D0_BUP2020ArAr_significance_5yr->cd();
   D0_BUP2020ArAr_significance_5yr->SetSelected(D0_BUP2020ArAr_significance_5yr);
}
