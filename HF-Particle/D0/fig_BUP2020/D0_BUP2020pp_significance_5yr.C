void D0_BUP2020pp_significance_5yr()
{
//=========Macro generated from canvas: D0_BUP2020pp_significance_5yr/D0_BUP2020pp_significance_5yr
//=========  (Fri Aug 28 00:10:45 2020) by ROOT version 6.16/00
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
   
   TH1F *hframe__3 = new TH1F("hframe__3","",1000,0,12);
   hframe__3->SetMinimum(0);
   hframe__3->SetMaximum(1380.365);
   hframe__3->SetDirectory(0);
   hframe__3->SetStats(0);
   hframe__3->SetLineWidth(2);
   hframe__3->SetMarkerStyle(20);
   hframe__3->SetMarkerSize(1.2);
   hframe__3->GetXaxis()->SetTitle("p_{T} [GeV]");
   hframe__3->GetXaxis()->SetLabelFont(42);
   hframe__3->GetXaxis()->SetLabelSize(0.05);
   hframe__3->GetXaxis()->SetTitleSize(0.05);
   hframe__3->GetXaxis()->SetTitleOffset(1.4);
   hframe__3->GetXaxis()->SetTitleFont(42);
   hframe__3->GetYaxis()->SetTitle("Significance");
   hframe__3->GetYaxis()->SetLabelFont(42);
   hframe__3->GetYaxis()->SetLabelSize(0.05);
   hframe__3->GetYaxis()->SetTitleSize(0.05);
   hframe__3->GetYaxis()->SetTitleOffset(1.4);
   hframe__3->GetYaxis()->SetTitleFont(42);
   hframe__3->GetZaxis()->SetLabelFont(42);
   hframe__3->GetZaxis()->SetLabelSize(0.05);
   hframe__3->GetZaxis()->SetTitleSize(0.05);
   hframe__3->GetZaxis()->SetTitleOffset(1);
   hframe__3->GetZaxis()->SetTitleFont(42);
   hframe__3->Draw(" ");
   
   Double_t Graph0_fx3[20] = {
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
   Double_t Graph0_fy3[20] = {
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
   TGraph *graph = new TGraph(20,Graph0_fx3,Graph0_fy3);
   graph->SetName("Graph0");
   graph->SetTitle("Significance for N_Collision = 3.6204e+12 centrality_ncoll = 1.0000e+00, based on 60-80%, w/o TOF, 48B; p_{T} [GeV/c]; S / #sqrt{S + B}");
   graph->SetLineWidth(2);
   graph->SetMarkerStyle(20);
   graph->SetMarkerSize(1.2);
   
   TH1F *Graph_Graph03 = new TH1F("Graph_Graph03","Significance for N_Collision = 3.6204e+12 centrality_ncoll = 1.0000e+00, based on 60-80%, w/o TOF, 48B",100,0,10.7);
   Graph_Graph03->SetMinimum(18.29657);
   Graph_Graph03->SetMaximum(1380.365);
   Graph_Graph03->SetDirectory(0);
   Graph_Graph03->SetLineWidth(2);
   Graph_Graph03->SetMarkerStyle(20);
   Graph_Graph03->SetMarkerSize(1.2);
   Graph_Graph03->GetXaxis()->SetTitle(" p_{T} [GeV/c]");
   Graph_Graph03->GetXaxis()->SetRange(1,94);
   Graph_Graph03->GetXaxis()->SetLabelFont(42);
   Graph_Graph03->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph03->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph03->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph03->GetXaxis()->SetTitleFont(42);
   Graph_Graph03->GetYaxis()->SetTitle(" S / #sqrt{S + B}");
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
   
   TH1F *hframe__4 = new TH1F("hframe__4","",1000,0,12);
   hframe__4->SetMinimum(0);
   hframe__4->SetMaximum(72.72108);
   hframe__4->SetDirectory(0);
   hframe__4->SetStats(0);
   hframe__4->SetLineWidth(2);
   hframe__4->SetMarkerStyle(20);
   hframe__4->SetMarkerSize(1.2);
   hframe__4->GetXaxis()->SetTitle("p_{T} [GeV]");
   hframe__4->GetXaxis()->SetLabelFont(42);
   hframe__4->GetXaxis()->SetLabelSize(0.05);
   hframe__4->GetXaxis()->SetTitleSize(0.05);
   hframe__4->GetXaxis()->SetTitleOffset(1.4);
   hframe__4->GetXaxis()->SetTitleFont(42);
   hframe__4->GetYaxis()->SetTitle("Significance");
   hframe__4->GetYaxis()->SetLabelFont(42);
   hframe__4->GetYaxis()->SetLabelSize(0.05);
   hframe__4->GetYaxis()->SetTitleSize(0.05);
   hframe__4->GetYaxis()->SetTitleOffset(1.4);
   hframe__4->GetYaxis()->SetTitleFont(42);
   hframe__4->GetZaxis()->SetLabelFont(42);
   hframe__4->GetZaxis()->SetLabelSize(0.05);
   hframe__4->GetZaxis()->SetTitleSize(0.05);
   hframe__4->GetZaxis()->SetTitleOffset(1);
   hframe__4->GetZaxis()->SetTitleFont(42);
   hframe__4->Draw(" ");
   
   Double_t Graph0_fx4[10] = {
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
   Double_t Graph0_fy4[10] = {
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
   graph = new TGraph(10,Graph0_fx4,Graph0_fy4);
   graph->SetName("Graph0");
   graph->SetTitle("Significance for N_Collision = 3.6204e+12 centrality_ncoll = 1.0000e+00, based on 60-80%, w/o TOF, 48B; p_{T} [GeV/c]; S / #sqrt{S + B}");
   graph->SetLineWidth(2);
   graph->SetMarkerStyle(20);
   graph->SetMarkerSize(1.2);
   
   TH1F *Graph_Graph04 = new TH1F("Graph_Graph04","Significance for N_Collision = 3.6204e+12 centrality_ncoll = 1.0000e+00, based on 60-80%, w/o TOF, 48B",100,0,10.4);
   Graph_Graph04->SetMinimum(4.022269);
   Graph_Graph04->SetMaximum(72.72108);
   Graph_Graph04->SetDirectory(0);
   Graph_Graph04->SetLineWidth(2);
   Graph_Graph04->SetMarkerStyle(20);
   Graph_Graph04->SetMarkerSize(1.2);
   Graph_Graph04->GetXaxis()->SetTitle(" p_{T} [GeV/c]");
   Graph_Graph04->GetXaxis()->SetRange(1,97);
   Graph_Graph04->GetXaxis()->SetLabelFont(42);
   Graph_Graph04->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph04->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph04->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph04->GetXaxis()->SetTitleFont(42);
   Graph_Graph04->GetYaxis()->SetTitle(" S / #sqrt{S + B}");
   Graph_Graph04->GetYaxis()->SetLabelFont(42);
   Graph_Graph04->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph04->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph04->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph04->GetYaxis()->SetTitleFont(42);
   Graph_Graph04->GetZaxis()->SetLabelFont(42);
   Graph_Graph04->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph04->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph04->GetZaxis()->SetTitleOffset(1);
   Graph_Graph04->GetZaxis()->SetTitleFont(42);
   graph->SetHistogram(Graph_Graph04);
   
   graph->Draw("p");
   D0_BUP2020pp_significance_5yr_2->Modified();
   D0_BUP2020pp_significance_5yr->cd();
   D0_BUP2020pp_significance_5yr->Modified();
   D0_BUP2020pp_significance_5yr->cd();
   D0_BUP2020pp_significance_5yr->SetSelected(D0_BUP2020pp_significance_5yr);
}
