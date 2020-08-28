void D0_BUP2020OO_significance_5yr()
{
//=========Macro generated from canvas: D0_BUP2020OO_significance_5yr/D0_BUP2020OO_significance_5yr
//=========  (Fri Aug 28 14:16:00 2020) by ROOT version 6.16/00
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
   
   TH1F *hframe__11 = new TH1F("hframe__11","",1000,0,12);
   hframe__11->SetMinimum(0);
   hframe__11->SetMaximum(458.0108);
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
   TGraph *graph = new TGraph(20,Graph0_fx11,Graph0_fy11);
   graph->SetName("Graph0");
   graph->SetTitle("Significance for N_Collision = 4.1519e+10 centrality_ncoll = 9.6000e+00, based on 60-80%, w/o TOF, 48B; p_{T} [GeV/c]; S / #sqrt{S + B}");
   graph->SetLineWidth(2);
   graph->SetMarkerStyle(20);
   graph->SetMarkerSize(1.2);
   
   TH1F *Graph_Graph011 = new TH1F("Graph_Graph011","Significance for N_Collision = 4.1519e+10 centrality_ncoll = 9.6000e+00, based on 60-80%, w/o TOF, 48B",100,0,10.7);
   Graph_Graph011->SetMinimum(18.29657);
   Graph_Graph011->SetMaximum(458.0108);
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
   
   TH1F *hframe__12 = new TH1F("hframe__12","",1000,0,12);
   hframe__12->SetMinimum(0);
   hframe__12->SetMaximum(24.12915);
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
   graph = new TGraph(10,Graph0_fx12,Graph0_fy12);
   graph->SetName("Graph0");
   graph->SetTitle("Significance for N_Collision = 4.1519e+10 centrality_ncoll = 9.6000e+00, based on 60-80%, w/o TOF, 48B; p_{T} [GeV/c]; S / #sqrt{S + B}");
   graph->SetLineWidth(2);
   graph->SetMarkerStyle(20);
   graph->SetMarkerSize(1.2);
   
   TH1F *Graph_Graph012 = new TH1F("Graph_Graph012","Significance for N_Collision = 4.1519e+10 centrality_ncoll = 9.6000e+00, based on 60-80%, w/o TOF, 48B",100,0,10.4);
   Graph_Graph012->SetMinimum(4.022269);
   Graph_Graph012->SetMaximum(24.12915);
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
   D0_BUP2020OO_significance_5yr_2->Modified();
   D0_BUP2020OO_significance_5yr->cd();
   D0_BUP2020OO_significance_5yr->Modified();
   D0_BUP2020OO_significance_5yr->cd();
   D0_BUP2020OO_significance_5yr->SetSelected(D0_BUP2020OO_significance_5yr);
}
