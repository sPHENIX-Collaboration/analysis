void D0_BUP2020ArAr_significance_5yr()
{
//=========Macro generated from canvas: D0_BUP2020ArAr_significance_5yr/D0_BUP2020ArAr_significance_5yr
//=========  (Fri Aug 28 00:10:47 2020) by ROOT version 6.16/00
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
   
   TH1F *hframe__13 = new TH1F("hframe__13","",1000,0,12);
   hframe__13->SetMinimum(0);
   hframe__13->SetMaximum(649.307);
   hframe__13->SetDirectory(0);
   hframe__13->SetStats(0);
   hframe__13->SetLineWidth(2);
   hframe__13->SetMarkerStyle(20);
   hframe__13->SetMarkerSize(1.2);
   hframe__13->GetXaxis()->SetTitle("p_{T} [GeV]");
   hframe__13->GetXaxis()->SetLabelFont(42);
   hframe__13->GetXaxis()->SetLabelSize(0.05);
   hframe__13->GetXaxis()->SetTitleSize(0.05);
   hframe__13->GetXaxis()->SetTitleOffset(1.4);
   hframe__13->GetXaxis()->SetTitleFont(42);
   hframe__13->GetYaxis()->SetTitle("Significance");
   hframe__13->GetYaxis()->SetLabelFont(42);
   hframe__13->GetYaxis()->SetLabelSize(0.05);
   hframe__13->GetYaxis()->SetTitleSize(0.05);
   hframe__13->GetYaxis()->SetTitleOffset(1.4);
   hframe__13->GetYaxis()->SetTitleFont(42);
   hframe__13->GetZaxis()->SetLabelFont(42);
   hframe__13->GetZaxis()->SetLabelSize(0.05);
   hframe__13->GetZaxis()->SetTitleSize(0.05);
   hframe__13->GetZaxis()->SetTitleOffset(1);
   hframe__13->GetZaxis()->SetTitleFont(42);
   hframe__13->Draw(" ");
   
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
   TGraph *graph = new TGraph(20,Graph0_fx13,Graph0_fy13);
   graph->SetName("Graph0");
   graph->SetTitle("Significance for N_Collision = 2.8108e+10 centrality_ncoll = 2.8500e+01, based on 60-80%, w/o TOF, 48B; p_{T} [GeV/c]; S / #sqrt{S + B}");
   graph->SetLineWidth(2);
   graph->SetMarkerStyle(20);
   graph->SetMarkerSize(1.2);
   
   TH1F *Graph_Graph013 = new TH1F("Graph_Graph013","Significance for N_Collision = 2.8108e+10 centrality_ncoll = 2.8500e+01, based on 60-80%, w/o TOF, 48B",100,0,10.7);
   Graph_Graph013->SetMinimum(18.29657);
   Graph_Graph013->SetMaximum(649.307);
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
   
   TH1F *hframe__14 = new TH1F("hframe__14","",1000,0,12);
   hframe__14->SetMinimum(0);
   hframe__14->SetMaximum(34.20711);
   hframe__14->SetDirectory(0);
   hframe__14->SetStats(0);
   hframe__14->SetLineWidth(2);
   hframe__14->SetMarkerStyle(20);
   hframe__14->SetMarkerSize(1.2);
   hframe__14->GetXaxis()->SetTitle("p_{T} [GeV]");
   hframe__14->GetXaxis()->SetLabelFont(42);
   hframe__14->GetXaxis()->SetLabelSize(0.05);
   hframe__14->GetXaxis()->SetTitleSize(0.05);
   hframe__14->GetXaxis()->SetTitleOffset(1.4);
   hframe__14->GetXaxis()->SetTitleFont(42);
   hframe__14->GetYaxis()->SetTitle("Significance");
   hframe__14->GetYaxis()->SetLabelFont(42);
   hframe__14->GetYaxis()->SetLabelSize(0.05);
   hframe__14->GetYaxis()->SetTitleSize(0.05);
   hframe__14->GetYaxis()->SetTitleOffset(1.4);
   hframe__14->GetYaxis()->SetTitleFont(42);
   hframe__14->GetZaxis()->SetLabelFont(42);
   hframe__14->GetZaxis()->SetLabelSize(0.05);
   hframe__14->GetZaxis()->SetTitleSize(0.05);
   hframe__14->GetZaxis()->SetTitleOffset(1);
   hframe__14->GetZaxis()->SetTitleFont(42);
   hframe__14->Draw(" ");
   
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
   graph = new TGraph(10,Graph0_fx14,Graph0_fy14);
   graph->SetName("Graph0");
   graph->SetTitle("Significance for N_Collision = 2.8108e+10 centrality_ncoll = 2.8500e+01, based on 60-80%, w/o TOF, 48B; p_{T} [GeV/c]; S / #sqrt{S + B}");
   graph->SetLineWidth(2);
   graph->SetMarkerStyle(20);
   graph->SetMarkerSize(1.2);
   
   TH1F *Graph_Graph014 = new TH1F("Graph_Graph014","Significance for N_Collision = 2.8108e+10 centrality_ncoll = 2.8500e+01, based on 60-80%, w/o TOF, 48B",100,0,10.4);
   Graph_Graph014->SetMinimum(4.022269);
   Graph_Graph014->SetMaximum(34.20711);
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
   D0_BUP2020ArAr_significance_5yr_2->Modified();
   D0_BUP2020ArAr_significance_5yr->cd();
   D0_BUP2020ArAr_significance_5yr->Modified();
   D0_BUP2020ArAr_significance_5yr->cd();
   D0_BUP2020ArAr_significance_5yr->SetSelected(D0_BUP2020ArAr_significance_5yr);
}
