void D0_BUP2020OO_significance_5yr()
{
//=========Macro generated from canvas: D0_BUP2020OO_significance_5yr/D0_BUP2020OO_significance_5yr
//=========  (Wed May 12 23:46:22 2021) by ROOT version 6.22/02
   TCanvas *D0_BUP2020OO_significance_5yr = new TCanvas("D0_BUP2020OO_significance_5yr", "D0_BUP2020OO_significance_5yr",0,0,1100,800);
   gStyle->SetOptFit(1);
   gStyle->SetOptStat(0);
   gStyle->SetOptTitle(0);
   D0_BUP2020OO_significance_5yr->SetHighLightColor(2);
   D0_BUP2020OO_significance_5yr->Range(0,0,1,1);
   D0_BUP2020OO_significance_5yr->SetFillColor(0);
   D0_BUP2020OO_significance_5yr->SetBorderMode(0);
   D0_BUP2020OO_significance_5yr->SetBorderSize(2);
   D0_BUP2020OO_significance_5yr->SetTickx(1);
   D0_BUP2020OO_significance_5yr->SetTicky(1);
   D0_BUP2020OO_significance_5yr->SetLeftMargin(0.16);
   D0_BUP2020OO_significance_5yr->SetRightMargin(0.05);
   D0_BUP2020OO_significance_5yr->SetTopMargin(0.05);
   D0_BUP2020OO_significance_5yr->SetBottomMargin(0.16);
   D0_BUP2020OO_significance_5yr->SetFrameBorderMode(0);
  
// ------------>Primitives in pad: D0_BUP2020OO_significance_5yr_1
   TPad *D0_BUP2020OO_significance_5yr_1 = new TPad("D0_BUP2020OO_significance_5yr_1", "D0_BUP2020OO_significance_5yr_1",0.01,0.01,0.49,0.99);
   D0_BUP2020OO_significance_5yr_1->Draw();
   D0_BUP2020OO_significance_5yr_1->cd();
   D0_BUP2020OO_significance_5yr_1->Range(-2.43038,-92.76168,12.75949,486.9989);
   D0_BUP2020OO_significance_5yr_1->SetFillColor(0);
   D0_BUP2020OO_significance_5yr_1->SetBorderMode(0);
   D0_BUP2020OO_significance_5yr_1->SetBorderSize(2);
   D0_BUP2020OO_significance_5yr_1->SetTickx(1);
   D0_BUP2020OO_significance_5yr_1->SetTicky(1);
   D0_BUP2020OO_significance_5yr_1->SetLeftMargin(0.16);
   D0_BUP2020OO_significance_5yr_1->SetRightMargin(0.05);
   D0_BUP2020OO_significance_5yr_1->SetTopMargin(0.05);
   D0_BUP2020OO_significance_5yr_1->SetBottomMargin(0.16);
   D0_BUP2020OO_significance_5yr_1->SetFrameBorderMode(0);
   D0_BUP2020OO_significance_5yr_1->SetFrameBorderMode(0);
   
   TH1F *hframe__15 = new TH1F("hframe__15","",1000,0,12);
   hframe__15->SetMinimum(0);
   hframe__15->SetMaximum(458.0108);
   hframe__15->SetDirectory(0);
   hframe__15->SetStats(0);
   hframe__15->SetLineWidth(2);
   hframe__15->SetMarkerStyle(20);
   hframe__15->SetMarkerSize(1.2);
   hframe__15->GetXaxis()->SetTitle("p_{T} [GeV]");
   hframe__15->GetXaxis()->SetLabelFont(42);
   hframe__15->GetXaxis()->SetLabelSize(0.05);
   hframe__15->GetXaxis()->SetTitleSize(0.05);
   hframe__15->GetXaxis()->SetTitleOffset(1.4);
   hframe__15->GetXaxis()->SetTitleFont(42);
   hframe__15->GetYaxis()->SetTitle("Significance");
   hframe__15->GetYaxis()->SetLabelFont(42);
   hframe__15->GetYaxis()->SetLabelSize(0.05);
   hframe__15->GetYaxis()->SetTitleSize(0.05);
   hframe__15->GetYaxis()->SetTitleOffset(1.4);
   hframe__15->GetYaxis()->SetTitleFont(42);
   hframe__15->GetZaxis()->SetLabelFont(42);
   hframe__15->GetZaxis()->SetLabelSize(0.05);
   hframe__15->GetZaxis()->SetTitleSize(0.05);
   hframe__15->GetZaxis()->SetTitleOffset(1);
   hframe__15->GetZaxis()->SetTitleFont(42);
   hframe__15->Draw(" ");
   
   Double_t Graph0_fx21[20] = {
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
   Double_t Graph0_fy21[20] = {
   156.8728,
   242.1079,
   353.8694,
   381.6757,
   357.1429,
   305.7188,
   277.2567,
   218.2449,
   167.8064,
   126.3672,
   99.49641,
   77.09799,
   59.67318,
   46.37322,
   41.78973,
   32.39768,
   25.30884,
   19.94562,
   15.8735,
   12.72326};
   TGraph *graph = new TGraph(20,Graph0_fx21,Graph0_fy21);
   graph->SetName("Graph0");
   graph->SetTitle("Significance for N_Collision = 4.1519e+10 centrality_ncoll = 9.6000e+00, based on 60-80%, w/o TOF, 48B");
   graph->SetLineWidth(2);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#00cc00");
   graph->SetMarkerColor(ci);
   graph->SetMarkerStyle(20);
   graph->SetMarkerSize(1.2);
   
   TH1F *Graph_Graph021 = new TH1F("Graph_Graph021","Significance for N_Collision = 4.1519e+10 centrality_ncoll = 9.6000e+00, based on 60-80%, w/o TOF, 48B",100,0,10.7);
   Graph_Graph021->SetMinimum(18.29657);
   Graph_Graph021->SetMaximum(458.0108);
   Graph_Graph021->SetDirectory(0);
   Graph_Graph021->SetLineWidth(2);
   Graph_Graph021->SetMarkerStyle(20);
   Graph_Graph021->SetMarkerSize(1.2);
   Graph_Graph021->GetXaxis()->SetTitle(" p_{T} [GeV/c]");
   Graph_Graph021->GetXaxis()->SetRange(1,94);
   Graph_Graph021->GetXaxis()->SetLabelFont(42);
   Graph_Graph021->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph021->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph021->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph021->GetXaxis()->SetTitleFont(42);
   Graph_Graph021->GetYaxis()->SetTitle(" S / #sqrt{S + B}");
   Graph_Graph021->GetYaxis()->SetLabelFont(42);
   Graph_Graph021->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph021->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph021->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph021->GetYaxis()->SetTitleFont(42);
   Graph_Graph021->GetZaxis()->SetLabelFont(42);
   Graph_Graph021->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph021->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph021->GetZaxis()->SetTitleOffset(1);
   Graph_Graph021->GetZaxis()->SetTitleFont(42);
   graph->SetHistogram(Graph_Graph021);
   
   graph->Draw("p");
   D0_BUP2020OO_significance_5yr_1->Modified();
   D0_BUP2020OO_significance_5yr->cd();
  
// ------------>Primitives in pad: D0_BUP2020OO_significance_5yr_2
   TPad *D0_BUP2020OO_significance_5yr_2 = new TPad("D0_BUP2020OO_significance_5yr_2", "D0_BUP2020OO_significance_5yr_2",0.51,0.01,0.99,0.99);
   D0_BUP2020OO_significance_5yr_2->Draw();
   D0_BUP2020OO_significance_5yr_2->cd();
   D0_BUP2020OO_significance_5yr_2->Range(-2.43038,-4.886916,12.75949,25.65631);
   D0_BUP2020OO_significance_5yr_2->SetFillColor(0);
   D0_BUP2020OO_significance_5yr_2->SetBorderMode(0);
   D0_BUP2020OO_significance_5yr_2->SetBorderSize(2);
   D0_BUP2020OO_significance_5yr_2->SetTickx(1);
   D0_BUP2020OO_significance_5yr_2->SetTicky(1);
   D0_BUP2020OO_significance_5yr_2->SetLeftMargin(0.16);
   D0_BUP2020OO_significance_5yr_2->SetRightMargin(0.05);
   D0_BUP2020OO_significance_5yr_2->SetTopMargin(0.05);
   D0_BUP2020OO_significance_5yr_2->SetBottomMargin(0.16);
   D0_BUP2020OO_significance_5yr_2->SetFrameBorderMode(0);
   D0_BUP2020OO_significance_5yr_2->SetFrameBorderMode(0);
   
   TH1F *hframe__16 = new TH1F("hframe__16","",1000,0,12);
   hframe__16->SetMinimum(0);
   hframe__16->SetMaximum(24.12915);
   hframe__16->SetDirectory(0);
   hframe__16->SetStats(0);
   hframe__16->SetLineWidth(2);
   hframe__16->SetMarkerStyle(20);
   hframe__16->SetMarkerSize(1.2);
   hframe__16->GetXaxis()->SetTitle("p_{T} [GeV]");
   hframe__16->GetXaxis()->SetLabelFont(42);
   hframe__16->GetXaxis()->SetLabelSize(0.05);
   hframe__16->GetXaxis()->SetTitleSize(0.05);
   hframe__16->GetXaxis()->SetTitleOffset(1.4);
   hframe__16->GetXaxis()->SetTitleFont(42);
   hframe__16->GetYaxis()->SetTitle("Significance");
   hframe__16->GetYaxis()->SetLabelFont(42);
   hframe__16->GetYaxis()->SetLabelSize(0.05);
   hframe__16->GetYaxis()->SetTitleSize(0.05);
   hframe__16->GetYaxis()->SetTitleOffset(1.4);
   hframe__16->GetYaxis()->SetTitleFont(42);
   hframe__16->GetZaxis()->SetLabelFont(42);
   hframe__16->GetZaxis()->SetLabelSize(0.05);
   hframe__16->GetZaxis()->SetTitleSize(0.05);
   hframe__16->GetZaxis()->SetTitleOffset(1);
   hframe__16->GetZaxis()->SetTitleFont(42);
   hframe__16->Draw(" ");
   
   Double_t Graph0_fx22[10] = {
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
   Double_t Graph0_fy22[10] = {
   4.116461,
   8.363578,
   17.47971,
   20.10763,
   19.35341,
   15.4712,
   11.64561,
   7.880492,
   6.01153,
   4.524406};
   graph = new TGraph(10,Graph0_fx22,Graph0_fy22);
   graph->SetName("Graph0");
   graph->SetTitle("Significance for N_Collision = 4.1519e+10 centrality_ncoll = 9.6000e+00, based on 60-80%, w/o TOF, 48B");
   graph->SetLineWidth(2);

   ci = TColor::GetColor("#00cc00");
   graph->SetMarkerColor(ci);
   graph->SetMarkerStyle(20);
   graph->SetMarkerSize(1.2);
   
   TH1F *Graph_Graph022 = new TH1F("Graph_Graph022","Significance for N_Collision = 4.1519e+10 centrality_ncoll = 9.6000e+00, based on 60-80%, w/o TOF, 48B",100,0,10.4);
   Graph_Graph022->SetMinimum(4.022269);
   Graph_Graph022->SetMaximum(24.12915);
   Graph_Graph022->SetDirectory(0);
   Graph_Graph022->SetLineWidth(2);
   Graph_Graph022->SetMarkerStyle(20);
   Graph_Graph022->SetMarkerSize(1.2);
   Graph_Graph022->GetXaxis()->SetTitle(" p_{T} [GeV/c]");
   Graph_Graph022->GetXaxis()->SetRange(1,97);
   Graph_Graph022->GetXaxis()->SetLabelFont(42);
   Graph_Graph022->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph022->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph022->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph022->GetXaxis()->SetTitleFont(42);
   Graph_Graph022->GetYaxis()->SetTitle(" S / #sqrt{S + B}");
   Graph_Graph022->GetYaxis()->SetLabelFont(42);
   Graph_Graph022->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph022->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph022->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph022->GetYaxis()->SetTitleFont(42);
   Graph_Graph022->GetZaxis()->SetLabelFont(42);
   Graph_Graph022->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph022->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph022->GetZaxis()->SetTitleOffset(1);
   Graph_Graph022->GetZaxis()->SetTitleFont(42);
   graph->SetHistogram(Graph_Graph022);
   
   graph->Draw("p");
   D0_BUP2020OO_significance_5yr_2->Modified();
   D0_BUP2020OO_significance_5yr->cd();
   D0_BUP2020OO_significance_5yr->Modified();
   D0_BUP2020OO_significance_5yr->cd();
   D0_BUP2020OO_significance_5yr->SetSelected(D0_BUP2020OO_significance_5yr);
}
