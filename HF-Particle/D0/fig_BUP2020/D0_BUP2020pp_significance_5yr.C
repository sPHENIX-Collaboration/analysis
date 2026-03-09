void D0_BUP2020pp_significance_5yr()
{
//=========Macro generated from canvas: D0_BUP2020pp_significance_5yr/D0_BUP2020pp_significance_5yr
//=========  (Wed May 12 23:46:19 2021) by ROOT version 6.22/02
   TCanvas *D0_BUP2020pp_significance_5yr = new TCanvas("D0_BUP2020pp_significance_5yr", "D0_BUP2020pp_significance_5yr",0,0,1100,800);
   gStyle->SetOptFit(1);
   gStyle->SetOptStat(0);
   gStyle->SetOptTitle(0);
   D0_BUP2020pp_significance_5yr->SetHighLightColor(2);
   D0_BUP2020pp_significance_5yr->Range(0,0,1,1);
   D0_BUP2020pp_significance_5yr->SetFillColor(0);
   D0_BUP2020pp_significance_5yr->SetBorderMode(0);
   D0_BUP2020pp_significance_5yr->SetBorderSize(2);
   D0_BUP2020pp_significance_5yr->SetTickx(1);
   D0_BUP2020pp_significance_5yr->SetTicky(1);
   D0_BUP2020pp_significance_5yr->SetLeftMargin(0.16);
   D0_BUP2020pp_significance_5yr->SetRightMargin(0.05);
   D0_BUP2020pp_significance_5yr->SetTopMargin(0.05);
   D0_BUP2020pp_significance_5yr->SetBottomMargin(0.16);
   D0_BUP2020pp_significance_5yr->SetFrameBorderMode(0);
  
// ------------>Primitives in pad: D0_BUP2020pp_significance_5yr_1
   TPad *D0_BUP2020pp_significance_5yr_1 = new TPad("D0_BUP2020pp_significance_5yr_1", "D0_BUP2020pp_significance_5yr_1",0.01,0.01,0.49,0.99);
   D0_BUP2020pp_significance_5yr_1->Draw();
   D0_BUP2020pp_significance_5yr_1->cd();
   D0_BUP2020pp_significance_5yr_1->Range(-2.43038,-279.5676,12.75949,1467.73);
   D0_BUP2020pp_significance_5yr_1->SetFillColor(0);
   D0_BUP2020pp_significance_5yr_1->SetBorderMode(0);
   D0_BUP2020pp_significance_5yr_1->SetBorderSize(2);
   D0_BUP2020pp_significance_5yr_1->SetTickx(1);
   D0_BUP2020pp_significance_5yr_1->SetTicky(1);
   D0_BUP2020pp_significance_5yr_1->SetLeftMargin(0.16);
   D0_BUP2020pp_significance_5yr_1->SetRightMargin(0.05);
   D0_BUP2020pp_significance_5yr_1->SetTopMargin(0.05);
   D0_BUP2020pp_significance_5yr_1->SetBottomMargin(0.16);
   D0_BUP2020pp_significance_5yr_1->SetFrameBorderMode(0);
   D0_BUP2020pp_significance_5yr_1->SetFrameBorderMode(0);
   
   TH1F *hframe__5 = new TH1F("hframe__5","",1000,0,12);
   hframe__5->SetMinimum(0);
   hframe__5->SetMaximum(1380.365);
   hframe__5->SetDirectory(0);
   hframe__5->SetStats(0);
   hframe__5->SetLineWidth(2);
   hframe__5->SetMarkerStyle(20);
   hframe__5->SetMarkerSize(1.2);
   hframe__5->GetXaxis()->SetTitle("p_{T} [GeV]");
   hframe__5->GetXaxis()->SetLabelFont(42);
   hframe__5->GetXaxis()->SetLabelSize(0.05);
   hframe__5->GetXaxis()->SetTitleSize(0.05);
   hframe__5->GetXaxis()->SetTitleOffset(1.4);
   hframe__5->GetXaxis()->SetTitleFont(42);
   hframe__5->GetYaxis()->SetTitle("Significance");
   hframe__5->GetYaxis()->SetLabelFont(42);
   hframe__5->GetYaxis()->SetLabelSize(0.05);
   hframe__5->GetYaxis()->SetTitleSize(0.05);
   hframe__5->GetYaxis()->SetTitleOffset(1.4);
   hframe__5->GetYaxis()->SetTitleFont(42);
   hframe__5->GetZaxis()->SetLabelFont(42);
   hframe__5->GetZaxis()->SetLabelSize(0.05);
   hframe__5->GetZaxis()->SetTitleSize(0.05);
   hframe__5->GetZaxis()->SetTitleOffset(1);
   hframe__5->GetZaxis()->SetTitleFont(42);
   hframe__5->Draw(" ");
   
   Double_t Graph0_fx11[20] = {
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
   Double_t Graph0_fy11[20] = {
   472.7875,
   729.6713,
   1066.501,
   1150.304,
   1076.367,
   921.3835,
   835.6036,
   657.7524,
   505.7394,
   380.8489,
   299.865,
   232.36,
   179.8446,
   139.7609,
   125.947,
   97.64101,
   76.27645,
   60.11264,
   47.83997,
   38.34571};
   TGraph *graph = new TGraph(20,Graph0_fx11,Graph0_fy11);
   graph->SetName("Graph0");
   graph->SetTitle("Significance for N_Collision = 3.6204e+12 centrality_ncoll = 1.0000e+00, based on 60-80%, w/o TOF, 48B");
   graph->SetLineWidth(2);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#00cc00");
   graph->SetMarkerColor(ci);
   graph->SetMarkerStyle(20);
   graph->SetMarkerSize(1.2);
   
   TH1F *Graph_Graph011 = new TH1F("Graph_Graph011","Significance for N_Collision = 3.6204e+12 centrality_ncoll = 1.0000e+00, based on 60-80%, w/o TOF, 48B",100,0,10.7);
   Graph_Graph011->SetMinimum(18.29657);
   Graph_Graph011->SetMaximum(1380.365);
   Graph_Graph011->SetDirectory(0);
   Graph_Graph011->SetLineWidth(2);
   Graph_Graph011->SetMarkerStyle(20);
   Graph_Graph011->SetMarkerSize(1.2);
   Graph_Graph011->GetXaxis()->SetTitle(" p_{T} [GeV/c]");
   Graph_Graph011->GetXaxis()->SetRange(1,94);
   Graph_Graph011->GetXaxis()->SetLabelFont(42);
   Graph_Graph011->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph011->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph011->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph011->GetXaxis()->SetTitleFont(42);
   Graph_Graph011->GetYaxis()->SetTitle(" S / #sqrt{S + B}");
   Graph_Graph011->GetYaxis()->SetLabelFont(42);
   Graph_Graph011->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph011->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph011->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph011->GetYaxis()->SetTitleFont(42);
   Graph_Graph011->GetZaxis()->SetLabelFont(42);
   Graph_Graph011->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph011->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph011->GetZaxis()->SetTitleOffset(1);
   Graph_Graph011->GetZaxis()->SetTitleFont(42);
   graph->SetHistogram(Graph_Graph011);
   
   graph->Draw("p");
   D0_BUP2020pp_significance_5yr_1->Modified();
   D0_BUP2020pp_significance_5yr->cd();
  
// ------------>Primitives in pad: D0_BUP2020pp_significance_5yr_2
   TPad *D0_BUP2020pp_significance_5yr_2 = new TPad("D0_BUP2020pp_significance_5yr_2", "D0_BUP2020pp_significance_5yr_2",0.51,0.01,0.99,0.99);
   D0_BUP2020pp_significance_5yr_2->Draw();
   D0_BUP2020pp_significance_5yr_2->cd();
   D0_BUP2020pp_significance_5yr_2->Range(-2.43038,-14.72832,12.75949,77.32368);
   D0_BUP2020pp_significance_5yr_2->SetFillColor(0);
   D0_BUP2020pp_significance_5yr_2->SetBorderMode(0);
   D0_BUP2020pp_significance_5yr_2->SetBorderSize(2);
   D0_BUP2020pp_significance_5yr_2->SetTickx(1);
   D0_BUP2020pp_significance_5yr_2->SetTicky(1);
   D0_BUP2020pp_significance_5yr_2->SetLeftMargin(0.16);
   D0_BUP2020pp_significance_5yr_2->SetRightMargin(0.05);
   D0_BUP2020pp_significance_5yr_2->SetTopMargin(0.05);
   D0_BUP2020pp_significance_5yr_2->SetBottomMargin(0.16);
   D0_BUP2020pp_significance_5yr_2->SetFrameBorderMode(0);
   D0_BUP2020pp_significance_5yr_2->SetFrameBorderMode(0);
   
   TH1F *hframe__6 = new TH1F("hframe__6","",1000,0,12);
   hframe__6->SetMinimum(0);
   hframe__6->SetMaximum(72.72108);
   hframe__6->SetDirectory(0);
   hframe__6->SetStats(0);
   hframe__6->SetLineWidth(2);
   hframe__6->SetMarkerStyle(20);
   hframe__6->SetMarkerSize(1.2);
   hframe__6->GetXaxis()->SetTitle("p_{T} [GeV]");
   hframe__6->GetXaxis()->SetLabelFont(42);
   hframe__6->GetXaxis()->SetLabelSize(0.05);
   hframe__6->GetXaxis()->SetTitleSize(0.05);
   hframe__6->GetXaxis()->SetTitleOffset(1.4);
   hframe__6->GetXaxis()->SetTitleFont(42);
   hframe__6->GetYaxis()->SetTitle("Significance");
   hframe__6->GetYaxis()->SetLabelFont(42);
   hframe__6->GetYaxis()->SetLabelSize(0.05);
   hframe__6->GetYaxis()->SetTitleSize(0.05);
   hframe__6->GetYaxis()->SetTitleOffset(1.4);
   hframe__6->GetYaxis()->SetTitleFont(42);
   hframe__6->GetZaxis()->SetLabelFont(42);
   hframe__6->GetZaxis()->SetLabelSize(0.05);
   hframe__6->GetZaxis()->SetTitleSize(0.05);
   hframe__6->GetZaxis()->SetTitleOffset(1);
   hframe__6->GetZaxis()->SetTitleFont(42);
   hframe__6->Draw(" ");
   
   Double_t Graph0_fx12[10] = {
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
   Double_t Graph0_fy12[10] = {
   12.4063,
   25.20637,
   52.68082,
   60.6009,
   58.32782,
   46.62752,
   35.09785,
   23.75044,
   18.11771,
   13.63578};
   graph = new TGraph(10,Graph0_fx12,Graph0_fy12);
   graph->SetName("Graph0");
   graph->SetTitle("Significance for N_Collision = 3.6204e+12 centrality_ncoll = 1.0000e+00, based on 60-80%, w/o TOF, 48B");
   graph->SetLineWidth(2);

   ci = TColor::GetColor("#00cc00");
   graph->SetMarkerColor(ci);
   graph->SetMarkerStyle(20);
   graph->SetMarkerSize(1.2);
   
   TH1F *Graph_Graph012 = new TH1F("Graph_Graph012","Significance for N_Collision = 3.6204e+12 centrality_ncoll = 1.0000e+00, based on 60-80%, w/o TOF, 48B",100,0,10.4);
   Graph_Graph012->SetMinimum(4.022269);
   Graph_Graph012->SetMaximum(72.72108);
   Graph_Graph012->SetDirectory(0);
   Graph_Graph012->SetLineWidth(2);
   Graph_Graph012->SetMarkerStyle(20);
   Graph_Graph012->SetMarkerSize(1.2);
   Graph_Graph012->GetXaxis()->SetTitle(" p_{T} [GeV/c]");
   Graph_Graph012->GetXaxis()->SetRange(1,97);
   Graph_Graph012->GetXaxis()->SetLabelFont(42);
   Graph_Graph012->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph012->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph012->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph012->GetXaxis()->SetTitleFont(42);
   Graph_Graph012->GetYaxis()->SetTitle(" S / #sqrt{S + B}");
   Graph_Graph012->GetYaxis()->SetLabelFont(42);
   Graph_Graph012->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph012->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph012->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph012->GetYaxis()->SetTitleFont(42);
   Graph_Graph012->GetZaxis()->SetLabelFont(42);
   Graph_Graph012->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph012->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph012->GetZaxis()->SetTitleOffset(1);
   Graph_Graph012->GetZaxis()->SetTitleFont(42);
   graph->SetHistogram(Graph_Graph012);
   
   graph->Draw("p");
   D0_BUP2020pp_significance_5yr_2->Modified();
   D0_BUP2020pp_significance_5yr->cd();
   D0_BUP2020pp_significance_5yr->Modified();
   D0_BUP2020pp_significance_5yr->cd();
   D0_BUP2020pp_significance_5yr->SetSelected(D0_BUP2020pp_significance_5yr);
}
