void D0_BUP2020_AN_3yr()
{
//=========Macro generated from canvas: D0_BUP2020_AN_3yr/D0_BUP2020OOArArO_AN_3yr
//=========  (Fri Aug 28 23:39:55 2020) by ROOT version 6.16/00
   TCanvas *D0_BUP2020_AN_3yr = new TCanvas("D0_BUP2020_AN_3yr", "D0_BUP2020OOArArO_AN_3yr",0,0,1100,800);
   gStyle->SetOptFit(1);
   gStyle->SetOptStat(0);
   gStyle->SetOptTitle(0);
   D0_BUP2020_AN_3yr->SetHighLightColor(2);
   D0_BUP2020_AN_3yr->Range(0,0,1,1);
   D0_BUP2020_AN_3yr->SetFillColor(0);
   D0_BUP2020_AN_3yr->SetBorderMode(0);
   D0_BUP2020_AN_3yr->SetBorderSize(2);
   D0_BUP2020_AN_3yr->SetTickx(1);
   D0_BUP2020_AN_3yr->SetTicky(1);
   D0_BUP2020_AN_3yr->SetLeftMargin(0.16);
   D0_BUP2020_AN_3yr->SetRightMargin(0.05);
   D0_BUP2020_AN_3yr->SetTopMargin(0.05);
   D0_BUP2020_AN_3yr->SetBottomMargin(0.16);
   D0_BUP2020_AN_3yr->SetFrameBorderMode(0);
  
// ------------>Primitives in pad: D0_BUP2020_AN_3yr_1
   TPad *D0_BUP2020_AN_3yr_1 = new TPad("D0_BUP2020_AN_3yr_1", "D0_BUP2020_AN_3yr_1",0.01,0.01,0.99,0.99);
   D0_BUP2020_AN_3yr_1->Draw();
   D0_BUP2020_AN_3yr_1->cd();
   D0_BUP2020_AN_3yr_1->Range(-1.012658,-0.0371519,5.316456,0.03879747);
   D0_BUP2020_AN_3yr_1->SetFillColor(0);
   D0_BUP2020_AN_3yr_1->SetBorderMode(0);
   D0_BUP2020_AN_3yr_1->SetBorderSize(2);
   D0_BUP2020_AN_3yr_1->SetTickx(1);
   D0_BUP2020_AN_3yr_1->SetTicky(1);
   D0_BUP2020_AN_3yr_1->SetLeftMargin(0.16);
   D0_BUP2020_AN_3yr_1->SetRightMargin(0.05);
   D0_BUP2020_AN_3yr_1->SetTopMargin(0.05);
   D0_BUP2020_AN_3yr_1->SetBottomMargin(0.16);
   D0_BUP2020_AN_3yr_1->SetFrameBorderMode(0);
   D0_BUP2020_AN_3yr_1->SetFrameBorderMode(0);
   
   TH1F *hframe__24 = new TH1F("hframe__24","",1000,0,5);
   hframe__24->SetMinimum(-0.025);
   hframe__24->SetMaximum(0.035);
   hframe__24->SetDirectory(0);
   hframe__24->SetStats(0);
   hframe__24->SetLineWidth(2);
   hframe__24->SetMarkerStyle(20);
   hframe__24->SetMarkerSize(1.2);
   hframe__24->GetXaxis()->SetTitle("#it{p}_{T} [GeV]");
   hframe__24->GetXaxis()->SetLabelFont(42);
   hframe__24->GetXaxis()->SetLabelSize(0.05);
   hframe__24->GetXaxis()->SetTitleSize(0.05);
   hframe__24->GetXaxis()->SetTitleOffset(1.4);
   hframe__24->GetXaxis()->SetTitleFont(42);
   hframe__24->GetYaxis()->SetTitle("A_{N}");
   hframe__24->GetYaxis()->SetLabelFont(42);
   hframe__24->GetYaxis()->SetLabelSize(0.05);
   hframe__24->GetYaxis()->SetTitleSize(0.05);
   hframe__24->GetYaxis()->SetTitleOffset(1.4);
   hframe__24->GetYaxis()->SetTitleFont(42);
   hframe__24->GetZaxis()->SetLabelFont(42);
   hframe__24->GetZaxis()->SetLabelSize(0.05);
   hframe__24->GetZaxis()->SetTitleSize(0.05);
   hframe__24->GetZaxis()->SetTitleOffset(1);
   hframe__24->GetZaxis()->SetTitleFont(42);
   hframe__24->Draw(" ");
   TLine *line = new TLine(0,-0,5,0);
   line->Draw();
   
   Double_t Graph0_fx27[30] = {
   1,
   1.1,
   1.2,
   1.3,
   1.4,
   1.5,
   1.6,
   1.7,
   1.8,
   1.9,
   2,
   2.1,
   2.2,
   2.3,
   2.4,
   2.5,
   2.6,
   2.7,
   2.8,
   2.9,
   3,
   3.1,
   3.2,
   3.3,
   3.4,
   3.5,
   3.6,
   3.7,
   3.8,
   3.9};
   Double_t Graph0_fy27[30] = {
   -0.0003416974,
   -0.0003446386,
   -0.0003737235,
   -0.0003766647,
   -0.0003796059,
   -0.0003825471,
   -0.000423376,
   -0.000447253,
   -0.0004501941,
   -0.0004531353,
   -0.0004560765,
   -0.0004821873,
   -0.0005190058,
   -0.000522962,
   -0.0005266647,
   -0.0005394398,
   -0.0005668878,
   -0.0005917628,
   -0.0006011655,
   -0.0006393142,
   -0.0006619588,
   -0.0006768133,
   -0.0007164591,
   -0.0007296059,
   -0.0007325471,
   -0.0007447225,
   -0.000784316,
   -0.0008001941,
   -0.0008463742,
   -0.0008649};
   TGraph *graph = new TGraph(30,Graph0_fx27,Graph0_fy27);
   graph->SetName("Graph0");
   graph->SetTitle("Graph");
   graph->SetFillStyle(1000);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#006666");
   graph->SetLineColor(ci);
   graph->SetLineWidth(5);
   graph->SetMarkerStyle(20);
   graph->SetMarkerSize(1.2);
   
   TH1F *Graph_Graph027 = new TH1F("Graph_Graph027","Graph",100,0.71,4.19);
   Graph_Graph027->SetMinimum(-0.0009172203);
   Graph_Graph027->SetMaximum(-0.0002893771);
   Graph_Graph027->SetDirectory(0);
   Graph_Graph027->SetStats(0);
   Graph_Graph027->SetLineWidth(2);
   Graph_Graph027->SetMarkerStyle(20);
   Graph_Graph027->SetMarkerSize(1.2);
   Graph_Graph027->GetXaxis()->SetLabelFont(42);
   Graph_Graph027->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph027->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph027->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph027->GetXaxis()->SetTitleFont(42);
   Graph_Graph027->GetYaxis()->SetLabelFont(42);
   Graph_Graph027->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph027->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph027->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph027->GetYaxis()->SetTitleFont(42);
   Graph_Graph027->GetZaxis()->SetLabelFont(42);
   Graph_Graph027->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph027->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph027->GetZaxis()->SetTitleOffset(1);
   Graph_Graph027->GetZaxis()->SetTitleFont(42);
   graph->SetHistogram(Graph_Graph027);
   
   graph->Draw("l");
   
   Double_t Graph1_fx28[30] = {
   1,
   1.1,
   1.2,
   1.3,
   1.4,
   1.5,
   1.6,
   1.7,
   1.8,
   1.9,
   2,
   2.1,
   2.2,
   2.3,
   2.4,
   2.5,
   2.6,
   2.7,
   2.8,
   2.9,
   3,
   3.1,
   3.2,
   3.3,
   3.4,
   3.5,
   3.6,
   3.7,
   3.8,
   3.9};
   Double_t Graph1_fy28[30] = {
   -0.01879963,
   -0.01869631,
   -0.0185394,
   -0.01832259,
   -0.01809963,
   -0.01782545,
   -0.01752092,
   -0.01718923,
   -0.01673616,
   -0.01626995,
   -0.01585206,
   -0.0153734,
   -0.01496127,
   -0.01456491,
   -0.01422932,
   -0.01387782,
   -0.01353845,
   -0.01321867,
   -0.01285432,
   -0.01258897,
   -0.01229654,
   -0.01199905,
   -0.01177594,
   -0.01148847,
   -0.01129233,
   -0.01101149,
   -0.01081373,
   -0.01058588,
   -0.01039728,
   -0.01018889};
   graph = new TGraph(30,Graph1_fx28,Graph1_fy28);
   graph->SetName("Graph1");
   graph->SetTitle("Graph");
   graph->SetFillStyle(1000);

   ci = TColor::GetColor("#000099");
   graph->SetLineColor(ci);
   graph->SetLineStyle(2);
   graph->SetLineWidth(5);
   graph->SetMarkerStyle(20);
   graph->SetMarkerSize(1.2);
   
   TH1F *Graph_Graph128 = new TH1F("Graph_Graph128","Graph",100,0.71,4.19);
   Graph_Graph128->SetMinimum(-0.0196607);
   Graph_Graph128->SetMaximum(-0.009327811);
   Graph_Graph128->SetDirectory(0);
   Graph_Graph128->SetStats(0);
   Graph_Graph128->SetLineWidth(2);
   Graph_Graph128->SetMarkerStyle(20);
   Graph_Graph128->SetMarkerSize(1.2);
   Graph_Graph128->GetXaxis()->SetLabelFont(42);
   Graph_Graph128->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph128->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph128->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph128->GetXaxis()->SetTitleFont(42);
   Graph_Graph128->GetYaxis()->SetLabelFont(42);
   Graph_Graph128->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph128->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph128->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph128->GetYaxis()->SetTitleFont(42);
   Graph_Graph128->GetZaxis()->SetLabelFont(42);
   Graph_Graph128->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph128->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph128->GetZaxis()->SetTitleOffset(1);
   Graph_Graph128->GetZaxis()->SetTitleFont(42);
   graph->SetHistogram(Graph_Graph128);
   
   graph->Draw("l");
   
   Double_t Graph2_fx1017[10] = {
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
   Double_t Graph2_fy1017[10] = {
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0};
   Double_t Graph2_fex1017[10] = {
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0};
   Double_t Graph2_fey1017[10] = {
   0.005320126,
   0.0029488,
   0.003264666,
   0.00434972,
   0.007306244,
   0.01219333,
   0.02030859,
   0.02902563,
   0.04762935,
   0.07544466};
   TGraphErrors *gre = new TGraphErrors(10,Graph2_fx1017,Graph2_fy1017,Graph2_fex1017,Graph2_fey1017);
   gre->SetName("Graph2");
   gre->SetTitle("Graph");
   gre->SetFillStyle(1000);

   ci = TColor::GetColor("#000000");
   gre->SetLineColor(ci);
   gre->SetLineWidth(4);

   ci = TColor::GetColor("#000000");
   gre->SetMarkerColor(ci);
   gre->SetMarkerStyle(20);
   gre->SetMarkerSize(2);
   
   TH1F *Graph_Graph21017 = new TH1F("Graph_Graph21017","Graph",100,0,10.4);
   Graph_Graph21017->SetMinimum(-0.0905336);
   Graph_Graph21017->SetMaximum(0.0905336);
   Graph_Graph21017->SetDirectory(0);
   Graph_Graph21017->SetStats(0);
   Graph_Graph21017->SetLineWidth(2);
   Graph_Graph21017->SetMarkerStyle(20);
   Graph_Graph21017->SetMarkerSize(1.2);
   Graph_Graph21017->GetXaxis()->SetLabelFont(42);
   Graph_Graph21017->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph21017->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph21017->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph21017->GetXaxis()->SetTitleFont(42);
   Graph_Graph21017->GetYaxis()->SetLabelFont(42);
   Graph_Graph21017->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph21017->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph21017->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph21017->GetYaxis()->SetTitleFont(42);
   Graph_Graph21017->GetZaxis()->SetLabelFont(42);
   Graph_Graph21017->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph21017->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph21017->GetZaxis()->SetTitleOffset(1);
   Graph_Graph21017->GetZaxis()->SetTitleFont(42);
   gre->SetHistogram(Graph_Graph21017);
   
   gre->Draw("p");
   
   TLegend *leg = new TLegend(0,0.8,0.83,0.95,NULL,"brNDC");
   leg->SetBorderSize(0);
   leg->SetLineColor(1);
   leg->SetLineStyle(1);
   leg->SetLineWidth(1);
   leg->SetFillColor(0);
   leg->SetFillStyle(0);
   TLegendEntry *entry=leg->AddEntry("NULL","#it{#bf{sPHENIX}} Projection, #it{p}^{#uparrow}+#it{p}#rightarrowD^{0}/#bar{D}^{0}+X, P=0.57","");
   entry->SetLineColor(1);
   entry->SetLineStyle(1);
   entry->SetLineWidth(1);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   entry->SetTextFont(42);
   leg->Draw();
   
   leg = new TLegend(0.2,0.58,0.85,0.83,NULL,"brNDC");
   leg->SetBorderSize(0);
   leg->SetLineColor(1);
   leg->SetLineStyle(1);
   leg->SetLineWidth(1);
   leg->SetFillColor(0);
   leg->SetFillStyle(0);
   entry=leg->AddEntry("Graph","6.2 pb^{-1} str. #it{p}+#it{p}, Years 1-3","pl");

   ci = TColor::GetColor("#000000");
   entry->SetLineColor(ci);
   entry->SetLineStyle(1);
   entry->SetLineWidth(4);

   ci = TColor::GetColor("#000000");
   entry->SetMarkerColor(ci);
   entry->SetMarkerStyle(20);
   entry->SetMarkerSize(2);
   entry->SetTextFont(42);
   entry=leg->AddEntry("Graph0","Kang, PRD#bf{78}, #lambda_{f} = #lambda_{d} = 0","l");

   ci = TColor::GetColor("#006666");
   entry->SetLineColor(ci);
   entry->SetLineStyle(1);
   entry->SetLineWidth(5);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   entry->SetTextFont(42);
   entry=leg->AddEntry("Graph1","Kang, PRD#bf{78}, #lambda_{f} = -#lambda_{d} = 70 MeV","l");

   ci = TColor::GetColor("#000099");
   entry->SetLineColor(ci);
   entry->SetLineStyle(2);
   entry->SetLineWidth(5);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   entry->SetTextFont(42);
   leg->Draw();
   D0_BUP2020_AN_3yr_1->Modified();
   D0_BUP2020_AN_3yr->cd();
   D0_BUP2020_AN_3yr->Modified();
   D0_BUP2020_AN_3yr->cd();
   D0_BUP2020_AN_3yr->SetSelected(D0_BUP2020_AN_3yr);
}
