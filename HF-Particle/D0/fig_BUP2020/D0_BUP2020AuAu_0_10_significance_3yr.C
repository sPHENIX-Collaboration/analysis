void D0_BUP2020AuAu_0_10_significance_3yr()
{
//=========Macro generated from canvas: D0_BUP2020AuAu_0_10_significance_3yr/D0_BUP2020AuAu_0_10_significance_3yr
//=========  (Fri Aug 28 09:59:07 2020) by ROOT version 6.16/00
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
   
   TH1F *hframe__5 = new TH1F("hframe__5","",1000,0,12);
   hframe__5->SetMinimum(0);
   hframe__5->SetMaximum(1583.104);
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
   
   Double_t Graph0_fx5[20] = {
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
   Double_t Graph0_fy5[20] = {
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
   TGraph *graph = new TGraph(20,Graph0_fx5,Graph0_fy5);
   graph->SetName("Graph0");
   graph->SetTitle("Significance for N_Collision = 1.4128e+11 centrality_ncoll = 9.6020e+01, based on 0-10%, w/o TOF, 24B; p_{T} [GeV/c]; S / #sqrt{S + B}");
   graph->SetLineWidth(2);
   graph->SetMarkerStyle(20);
   graph->SetMarkerSize(1.2);
   
   TH1F *Graph_Graph05 = new TH1F("Graph_Graph05","Significance for N_Collision = 1.4128e+11 centrality_ncoll = 9.6020e+01, based on 0-10%, w/o TOF, 24B",100,0,10.7);
   Graph_Graph05->SetMinimum(26.2875);
   Graph_Graph05->SetMaximum(1583.104);
   Graph_Graph05->SetDirectory(0);
   Graph_Graph05->SetLineWidth(2);
   Graph_Graph05->SetMarkerStyle(20);
   Graph_Graph05->SetMarkerSize(1.2);
   Graph_Graph05->GetXaxis()->SetTitle(" p_{T} [GeV/c]");
   Graph_Graph05->GetXaxis()->SetRange(1,94);
   Graph_Graph05->GetXaxis()->SetLabelFont(42);
   Graph_Graph05->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph05->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph05->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph05->GetXaxis()->SetTitleFont(42);
   Graph_Graph05->GetYaxis()->SetTitle(" S / #sqrt{S + B}");
   Graph_Graph05->GetYaxis()->SetLabelFont(42);
   Graph_Graph05->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph05->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph05->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph05->GetYaxis()->SetTitleFont(42);
   Graph_Graph05->GetZaxis()->SetLabelFont(42);
   Graph_Graph05->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph05->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph05->GetZaxis()->SetTitleOffset(1);
   Graph_Graph05->GetZaxis()->SetTitleFont(42);
   graph->SetHistogram(Graph_Graph05);
   
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
   
   TH1F *hframe__6 = new TH1F("hframe__6","",1000,0,12);
   hframe__6->SetMinimum(0);
   hframe__6->SetMaximum(70.9852);
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
   
   Double_t Graph0_fx6[10] = {
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
   Double_t Graph0_fy6[10] = {
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
   graph = new TGraph(10,Graph0_fx6,Graph0_fy6);
   graph->SetName("Graph0");
   graph->SetTitle("Significance for N_Collision = 1.4128e+11 centrality_ncoll = 9.6020e+01, based on 0-10%, w/o TOF, 24B; p_{T} [GeV/c]; S / #sqrt{S + B}");
   graph->SetLineWidth(2);
   graph->SetMarkerStyle(20);
   graph->SetMarkerSize(1.2);
   
   TH1F *Graph_Graph06 = new TH1F("Graph_Graph06","Significance for N_Collision = 1.4128e+11 centrality_ncoll = 9.6020e+01, based on 0-10%, w/o TOF, 24B",100,0,10.4);
   Graph_Graph06->SetMinimum(1.607636);
   Graph_Graph06->SetMaximum(70.9852);
   Graph_Graph06->SetDirectory(0);
   Graph_Graph06->SetLineWidth(2);
   Graph_Graph06->SetMarkerStyle(20);
   Graph_Graph06->SetMarkerSize(1.2);
   Graph_Graph06->GetXaxis()->SetTitle(" p_{T} [GeV/c]");
   Graph_Graph06->GetXaxis()->SetRange(1,97);
   Graph_Graph06->GetXaxis()->SetLabelFont(42);
   Graph_Graph06->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph06->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph06->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph06->GetXaxis()->SetTitleFont(42);
   Graph_Graph06->GetYaxis()->SetTitle(" S / #sqrt{S + B}");
   Graph_Graph06->GetYaxis()->SetLabelFont(42);
   Graph_Graph06->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph06->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph06->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph06->GetYaxis()->SetTitleFont(42);
   Graph_Graph06->GetZaxis()->SetLabelFont(42);
   Graph_Graph06->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph06->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph06->GetZaxis()->SetTitleOffset(1);
   Graph_Graph06->GetZaxis()->SetTitleFont(42);
   graph->SetHistogram(Graph_Graph06);
   
   graph->Draw("p");
   D0_BUP2020AuAu_0_10_significance_3yr_2->Modified();
   D0_BUP2020AuAu_0_10_significance_3yr->cd();
   D0_BUP2020AuAu_0_10_significance_3yr->Modified();
   D0_BUP2020AuAu_0_10_significance_3yr->cd();
   D0_BUP2020AuAu_0_10_significance_3yr->SetSelected(D0_BUP2020AuAu_0_10_significance_3yr);
}
