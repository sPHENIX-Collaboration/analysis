void D0_BUP2020AuAu_0_80_significance_5yr()
{
//=========Macro generated from canvas: D0_BUP2020AuAu_0_80_significance_5yr/D0_BUP2020AuAu_0_80_significance_5yr
//=========  (Fri Aug 28 10:29:54 2020) by ROOT version 6.16/00
   TCanvas *D0_BUP2020AuAu_0_80_significance_5yr = new TCanvas("D0_BUP2020AuAu_0_80_significance_5yr", "D0_BUP2020AuAu_0_80_significance_5yr",0,0,1100,800);
   gStyle->SetOptFit(1);
   gStyle->SetOptStat(0);
   gStyle->SetOptTitle(0);
   D0_BUP2020AuAu_0_80_significance_5yr->SetHighLightColor(2);
   D0_BUP2020AuAu_0_80_significance_5yr->Range(0,0,1,1);
   D0_BUP2020AuAu_0_80_significance_5yr->SetFillColor(0);
   D0_BUP2020AuAu_0_80_significance_5yr->SetBorderMode(0);
   D0_BUP2020AuAu_0_80_significance_5yr->SetBorderSize(2);
   D0_BUP2020AuAu_0_80_significance_5yr->SetTickx(1);
   D0_BUP2020AuAu_0_80_significance_5yr->SetTicky(1);
   D0_BUP2020AuAu_0_80_significance_5yr->SetLeftMargin(0.16);
   D0_BUP2020AuAu_0_80_significance_5yr->SetRightMargin(0.05);
   D0_BUP2020AuAu_0_80_significance_5yr->SetTopMargin(0.05);
   D0_BUP2020AuAu_0_80_significance_5yr->SetBottomMargin(0.16);
   D0_BUP2020AuAu_0_80_significance_5yr->SetFrameBorderMode(0);
  
// ------------>Primitives in pad: D0_BUP2020AuAu_0_80_significance_5yr_1
   TPad *D0_BUP2020AuAu_0_80_significance_5yr_1 = new TPad("D0_BUP2020AuAu_0_80_significance_5yr_1", "D0_BUP2020AuAu_0_80_significance_5yr_1",0.01,0.01,0.49,0.99);
   D0_BUP2020AuAu_0_80_significance_5yr_1->Draw();
   D0_BUP2020AuAu_0_80_significance_5yr_1->cd();
   D0_BUP2020AuAu_0_80_significance_5yr_1->Range(-2.43038,-833.3988,12.75949,4375.344);
   D0_BUP2020AuAu_0_80_significance_5yr_1->SetFillColor(0);
   D0_BUP2020AuAu_0_80_significance_5yr_1->SetBorderMode(0);
   D0_BUP2020AuAu_0_80_significance_5yr_1->SetBorderSize(2);
   D0_BUP2020AuAu_0_80_significance_5yr_1->SetTickx(1);
   D0_BUP2020AuAu_0_80_significance_5yr_1->SetTicky(1);
   D0_BUP2020AuAu_0_80_significance_5yr_1->SetLeftMargin(0.16);
   D0_BUP2020AuAu_0_80_significance_5yr_1->SetRightMargin(0.05);
   D0_BUP2020AuAu_0_80_significance_5yr_1->SetTopMargin(0.05);
   D0_BUP2020AuAu_0_80_significance_5yr_1->SetBottomMargin(0.16);
   D0_BUP2020AuAu_0_80_significance_5yr_1->SetFrameBorderMode(0);
   D0_BUP2020AuAu_0_80_significance_5yr_1->SetFrameBorderMode(0);
   
   TH1F *hframe__9 = new TH1F("hframe__9","",1000,0,12);
   hframe__9->SetMinimum(0);
   hframe__9->SetMaximum(4114.907);
   hframe__9->SetDirectory(0);
   hframe__9->SetStats(0);
   hframe__9->SetLineWidth(2);
   hframe__9->SetMarkerStyle(20);
   hframe__9->SetMarkerSize(1.2);
   hframe__9->GetXaxis()->SetTitle("p_{T} [GeV]");
   hframe__9->GetXaxis()->SetLabelFont(42);
   hframe__9->GetXaxis()->SetLabelSize(0.05);
   hframe__9->GetXaxis()->SetTitleSize(0.05);
   hframe__9->GetXaxis()->SetTitleOffset(1.4);
   hframe__9->GetXaxis()->SetTitleFont(42);
   hframe__9->GetYaxis()->SetTitle("Significance");
   hframe__9->GetYaxis()->SetLabelFont(42);
   hframe__9->GetYaxis()->SetLabelSize(0.05);
   hframe__9->GetYaxis()->SetTitleSize(0.05);
   hframe__9->GetYaxis()->SetTitleOffset(1.4);
   hframe__9->GetYaxis()->SetTitleFont(42);
   hframe__9->GetZaxis()->SetLabelFont(42);
   hframe__9->GetZaxis()->SetLabelSize(0.05);
   hframe__9->GetZaxis()->SetTitleSize(0.05);
   hframe__9->GetZaxis()->SetTitleOffset(1);
   hframe__9->GetZaxis()->SetTitleFont(42);
   hframe__9->Draw(" ");
   
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
   1151.598,
   1811.788,
   2784.259,
   3408.165,
   3429.089,
   3055.78,
   2249.232,
   1741.873,
   1325.764,
   1019.804,
   797.0617,
   610.9829,
   466.8747,
   357.0955,
   277.3544,
   212.74,
   163.9314,
   127.8765,
   99.74263,
   79.00544};
   TGraph *graph = new TGraph(20,Graph0_fx9,Graph0_fy9);
   graph->SetName("Graph0");
   graph->SetTitle("Significance for N_Collision = 3.4604e+11 centrality_ncoll = 1.9080e+02, based on 0-80%, w/o TOF, 192B; p_{T} [GeV/c]; S / #sqrt{S + B}");
   graph->SetLineWidth(2);
   graph->SetMarkerStyle(20);
   graph->SetMarkerSize(1.2);
   
   TH1F *Graph_Graph09 = new TH1F("Graph_Graph09","Significance for N_Collision = 3.4604e+11 centrality_ncoll = 1.9080e+02, based on 0-80%, w/o TOF, 192B",100,0,10.7);
   Graph_Graph09->SetMinimum(59.2166);
   Graph_Graph09->SetMaximum(4114.907);
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
   D0_BUP2020AuAu_0_80_significance_5yr_1->Modified();
   D0_BUP2020AuAu_0_80_significance_5yr->cd();
  
// ------------>Primitives in pad: D0_BUP2020AuAu_0_80_significance_5yr_2
   TPad *D0_BUP2020AuAu_0_80_significance_5yr_2 = new TPad("D0_BUP2020AuAu_0_80_significance_5yr_2", "D0_BUP2020AuAu_0_80_significance_5yr_2",0.51,0.01,0.99,0.99);
   D0_BUP2020AuAu_0_80_significance_5yr_2->Draw();
   D0_BUP2020AuAu_0_80_significance_5yr_2->cd();
   D0_BUP2020AuAu_0_80_significance_5yr_2->Range(-2.43038,-36.60958,12.75949,192.2003);
   D0_BUP2020AuAu_0_80_significance_5yr_2->SetFillColor(0);
   D0_BUP2020AuAu_0_80_significance_5yr_2->SetBorderMode(0);
   D0_BUP2020AuAu_0_80_significance_5yr_2->SetBorderSize(2);
   D0_BUP2020AuAu_0_80_significance_5yr_2->SetTickx(1);
   D0_BUP2020AuAu_0_80_significance_5yr_2->SetTicky(1);
   D0_BUP2020AuAu_0_80_significance_5yr_2->SetLeftMargin(0.16);
   D0_BUP2020AuAu_0_80_significance_5yr_2->SetRightMargin(0.05);
   D0_BUP2020AuAu_0_80_significance_5yr_2->SetTopMargin(0.05);
   D0_BUP2020AuAu_0_80_significance_5yr_2->SetBottomMargin(0.16);
   D0_BUP2020AuAu_0_80_significance_5yr_2->SetFrameBorderMode(0);
   D0_BUP2020AuAu_0_80_significance_5yr_2->SetFrameBorderMode(0);
   
   TH1F *hframe__10 = new TH1F("hframe__10","",1000,0,12);
   hframe__10->SetMinimum(0);
   hframe__10->SetMaximum(180.7598);
   hframe__10->SetDirectory(0);
   hframe__10->SetStats(0);
   hframe__10->SetLineWidth(2);
   hframe__10->SetMarkerStyle(20);
   hframe__10->SetMarkerSize(1.2);
   hframe__10->GetXaxis()->SetTitle("p_{T} [GeV]");
   hframe__10->GetXaxis()->SetLabelFont(42);
   hframe__10->GetXaxis()->SetLabelSize(0.05);
   hframe__10->GetXaxis()->SetTitleSize(0.05);
   hframe__10->GetXaxis()->SetTitleOffset(1.4);
   hframe__10->GetXaxis()->SetTitleFont(42);
   hframe__10->GetYaxis()->SetTitle("Significance");
   hframe__10->GetYaxis()->SetLabelFont(42);
   hframe__10->GetYaxis()->SetLabelSize(0.05);
   hframe__10->GetYaxis()->SetTitleSize(0.05);
   hframe__10->GetYaxis()->SetTitleOffset(1.4);
   hframe__10->GetYaxis()->SetTitleFont(42);
   hframe__10->GetZaxis()->SetLabelFont(42);
   hframe__10->GetZaxis()->SetLabelSize(0.05);
   hframe__10->GetZaxis()->SetTitleSize(0.05);
   hframe__10->GetZaxis()->SetTitleOffset(1);
   hframe__10->GetZaxis()->SetTitleFont(42);
   hframe__10->Draw(" ");
   
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
   17.92239,
   35.53991,
   84.13591,
   138.5858,
   150.6332,
   128.5061,
   103.299,
   71.52636,
   55.21449,
   40.22206};
   graph = new TGraph(10,Graph0_fx10,Graph0_fy10);
   graph->SetName("Graph0");
   graph->SetTitle("Significance for N_Collision = 3.4604e+11 centrality_ncoll = 1.9080e+02, based on 0-80%, w/o TOF, 192B; p_{T} [GeV/c]; S / #sqrt{S + B}");
   graph->SetLineWidth(2);
   graph->SetMarkerStyle(20);
   graph->SetMarkerSize(1.2);
   
   TH1F *Graph_Graph010 = new TH1F("Graph_Graph010","Significance for N_Collision = 3.4604e+11 centrality_ncoll = 1.9080e+02, based on 0-80%, w/o TOF, 192B",100,0,10.4);
   Graph_Graph010->SetMinimum(3.873636);
   Graph_Graph010->SetMaximum(180.7598);
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
   D0_BUP2020AuAu_0_80_significance_5yr_2->Modified();
   D0_BUP2020AuAu_0_80_significance_5yr->cd();
   D0_BUP2020AuAu_0_80_significance_5yr->Modified();
   D0_BUP2020AuAu_0_80_significance_5yr->cd();
   D0_BUP2020AuAu_0_80_significance_5yr->SetSelected(D0_BUP2020AuAu_0_80_significance_5yr);
}
