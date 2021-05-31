void D0_BUP2020pp_significance_3yr()
{
//=========Macro generated from canvas: D0_BUP2020pp_significance_3yr/D0_BUP2020pp_significance_3yr
//=========  (Wed May 12 23:46:18 2021) by ROOT version 6.22/02
   TCanvas *D0_BUP2020pp_significance_3yr = new TCanvas("D0_BUP2020pp_significance_3yr", "D0_BUP2020pp_significance_3yr",0,0,1100,800);
   gStyle->SetOptFit(1);
   gStyle->SetOptStat(0);
   gStyle->SetOptTitle(0);
   D0_BUP2020pp_significance_3yr->SetHighLightColor(2);
   D0_BUP2020pp_significance_3yr->Range(0,0,1,1);
   D0_BUP2020pp_significance_3yr->SetFillColor(0);
   D0_BUP2020pp_significance_3yr->SetBorderMode(0);
   D0_BUP2020pp_significance_3yr->SetBorderSize(2);
   D0_BUP2020pp_significance_3yr->SetTickx(1);
   D0_BUP2020pp_significance_3yr->SetTicky(1);
   D0_BUP2020pp_significance_3yr->SetLeftMargin(0.16);
   D0_BUP2020pp_significance_3yr->SetRightMargin(0.05);
   D0_BUP2020pp_significance_3yr->SetTopMargin(0.05);
   D0_BUP2020pp_significance_3yr->SetBottomMargin(0.16);
   D0_BUP2020pp_significance_3yr->SetFrameBorderMode(0);
  
// ------------>Primitives in pad: D0_BUP2020pp_significance_3yr_1
   TPad *D0_BUP2020pp_significance_3yr_1 = new TPad("D0_BUP2020pp_significance_3yr_1", "D0_BUP2020pp_significance_3yr_1",0.01,0.01,0.49,0.99);
   D0_BUP2020pp_significance_3yr_1->Draw();
   D0_BUP2020pp_significance_3yr_1->cd();
   D0_BUP2020pp_significance_3yr_1->Range(-2.43038,-74.97718,12.75949,393.6302);
   D0_BUP2020pp_significance_3yr_1->SetFillColor(0);
   D0_BUP2020pp_significance_3yr_1->SetBorderMode(0);
   D0_BUP2020pp_significance_3yr_1->SetBorderSize(2);
   D0_BUP2020pp_significance_3yr_1->SetTickx(1);
   D0_BUP2020pp_significance_3yr_1->SetTicky(1);
   D0_BUP2020pp_significance_3yr_1->SetLeftMargin(0.16);
   D0_BUP2020pp_significance_3yr_1->SetRightMargin(0.05);
   D0_BUP2020pp_significance_3yr_1->SetTopMargin(0.05);
   D0_BUP2020pp_significance_3yr_1->SetBottomMargin(0.16);
   D0_BUP2020pp_significance_3yr_1->SetFrameBorderMode(0);
   D0_BUP2020pp_significance_3yr_1->SetFrameBorderMode(0);
   
   TH1F *hframe__3 = new TH1F("hframe__3","",1000,0,12);
   hframe__3->SetMinimum(0);
   hframe__3->SetMaximum(370.1999);
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
   
   Double_t Graph0_fx9[20] = {
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
   Double_t Graph0_fy9[20] = {
   126.7968,
   195.6904,
   286.0247,
   308.4999,
   288.6705,
   247.1056,
   224.1003,
   176.4025,
   135.6341,
   102.1398,
   80.42071,
   62.31657,
   48.23249,
   37.48243,
   33.7777,
   26.18632,
   20.45656,
   16.1216,
   12.83019,
   10.28393};
   TGraph *graph = new TGraph(20,Graph0_fx9,Graph0_fy9);
   graph->SetName("Graph0");
   graph->SetTitle("Significance for N_Collision = 2.6040e+11 centrality_ncoll = 1.0000e+00, based on 60-80%, w/o TOF, 48B");
   graph->SetLineWidth(2);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#00cc00");
   graph->SetMarkerColor(ci);
   graph->SetMarkerStyle(20);
   graph->SetMarkerSize(1.2);
   
   TH1F *Graph_Graph09 = new TH1F("Graph_Graph09","Significance for N_Collision = 2.6040e+11 centrality_ncoll = 1.0000e+00, based on 60-80%, w/o TOF, 48B",100,0,10.7);
   Graph_Graph09->SetMinimum(18.29657);
   Graph_Graph09->SetMaximum(370.1999);
   Graph_Graph09->SetDirectory(0);
   Graph_Graph09->SetLineWidth(2);
   Graph_Graph09->SetMarkerStyle(20);
   Graph_Graph09->SetMarkerSize(1.2);
   Graph_Graph09->GetXaxis()->SetTitle(" p_{T} [GeV/c]");
   Graph_Graph09->GetXaxis()->SetRange(1,94);
   Graph_Graph09->GetXaxis()->SetLabelFont(42);
   Graph_Graph09->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph09->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph09->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph09->GetXaxis()->SetTitleFont(42);
   Graph_Graph09->GetYaxis()->SetTitle(" S / #sqrt{S + B}");
   Graph_Graph09->GetYaxis()->SetLabelFont(42);
   Graph_Graph09->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph09->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph09->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph09->GetYaxis()->SetTitleFont(42);
   Graph_Graph09->GetZaxis()->SetLabelFont(42);
   Graph_Graph09->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph09->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph09->GetZaxis()->SetTitleOffset(1);
   Graph_Graph09->GetZaxis()->SetTitleFont(42);
   graph->SetHistogram(Graph_Graph09);
   
   graph->Draw("p");
   D0_BUP2020pp_significance_3yr_1->Modified();
   D0_BUP2020pp_significance_3yr->cd();
  
// ------------>Primitives in pad: D0_BUP2020pp_significance_3yr_2
   TPad *D0_BUP2020pp_significance_3yr_2 = new TPad("D0_BUP2020pp_significance_3yr_2", "D0_BUP2020pp_significance_3yr_2",0.51,0.01,0.99,0.99);
   D0_BUP2020pp_significance_3yr_2->Draw();
   D0_BUP2020pp_significance_3yr_2->cd();
   D0_BUP2020pp_significance_3yr_2->Range(-2.43038,-3.949985,12.75949,20.73742);
   D0_BUP2020pp_significance_3yr_2->SetFillColor(0);
   D0_BUP2020pp_significance_3yr_2->SetBorderMode(0);
   D0_BUP2020pp_significance_3yr_2->SetBorderSize(2);
   D0_BUP2020pp_significance_3yr_2->SetTickx(1);
   D0_BUP2020pp_significance_3yr_2->SetTicky(1);
   D0_BUP2020pp_significance_3yr_2->SetLeftMargin(0.16);
   D0_BUP2020pp_significance_3yr_2->SetRightMargin(0.05);
   D0_BUP2020pp_significance_3yr_2->SetTopMargin(0.05);
   D0_BUP2020pp_significance_3yr_2->SetBottomMargin(0.16);
   D0_BUP2020pp_significance_3yr_2->SetFrameBorderMode(0);
   D0_BUP2020pp_significance_3yr_2->SetFrameBorderMode(0);
   
   TH1F *hframe__4 = new TH1F("hframe__4","",1000,0,12);
   hframe__4->SetMinimum(0);
   hframe__4->SetMaximum(19.50305);
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
   
   Double_t Graph0_fx10[10] = {
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
   Double_t Graph0_fy10[10] = {
   3.327242,
   6.760092,
   14.12846,
   16.25254,
   15.64292,
   12.50502,
   9.412885,
   6.369624,
   4.858984,
   3.656976};
   graph = new TGraph(10,Graph0_fx10,Graph0_fy10);
   graph->SetName("Graph0");
   graph->SetTitle("Significance for N_Collision = 2.6040e+11 centrality_ncoll = 1.0000e+00, based on 60-80%, w/o TOF, 48B");
   graph->SetLineWidth(2);

   ci = TColor::GetColor("#00cc00");
   graph->SetMarkerColor(ci);
   graph->SetMarkerStyle(20);
   graph->SetMarkerSize(1.2);
   
   TH1F *Graph_Graph010 = new TH1F("Graph_Graph010","Significance for N_Collision = 2.6040e+11 centrality_ncoll = 1.0000e+00, based on 60-80%, w/o TOF, 48B",100,0,10.4);
   Graph_Graph010->SetMinimum(4.022269);
   Graph_Graph010->SetMaximum(19.50305);
   Graph_Graph010->SetDirectory(0);
   Graph_Graph010->SetLineWidth(2);
   Graph_Graph010->SetMarkerStyle(20);
   Graph_Graph010->SetMarkerSize(1.2);
   Graph_Graph010->GetXaxis()->SetTitle(" p_{T} [GeV/c]");
   Graph_Graph010->GetXaxis()->SetRange(1,97);
   Graph_Graph010->GetXaxis()->SetLabelFont(42);
   Graph_Graph010->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph010->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph010->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph010->GetXaxis()->SetTitleFont(42);
   Graph_Graph010->GetYaxis()->SetTitle(" S / #sqrt{S + B}");
   Graph_Graph010->GetYaxis()->SetLabelFont(42);
   Graph_Graph010->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph010->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph010->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph010->GetYaxis()->SetTitleFont(42);
   Graph_Graph010->GetZaxis()->SetLabelFont(42);
   Graph_Graph010->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph010->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph010->GetZaxis()->SetTitleOffset(1);
   Graph_Graph010->GetZaxis()->SetTitleFont(42);
   graph->SetHistogram(Graph_Graph010);
   
   graph->Draw("p");
   D0_BUP2020pp_significance_3yr_2->Modified();
   D0_BUP2020pp_significance_3yr->cd();
   D0_BUP2020pp_significance_3yr->Modified();
   D0_BUP2020pp_significance_3yr->cd();
   D0_BUP2020pp_significance_3yr->SetSelected(D0_BUP2020pp_significance_3yr);
}
