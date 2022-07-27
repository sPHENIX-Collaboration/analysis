void D0_BUP2020_AN_5yr_comparison()
{
//=========Macro generated from canvas: D0_BUP2020_AN_5yr_comparison/D0_BUP2020OOArArO_AN_5yr_comparison
//=========  (Wed May 12 23:46:28 2021) by ROOT version 6.22/02
   TCanvas *D0_BUP2020_AN_5yr_comparison = new TCanvas("D0_BUP2020_AN_5yr_comparison", "D0_BUP2020OOArArO_AN_5yr_comparison",0,0,1100,800);
   gStyle->SetOptFit(1);
   gStyle->SetOptStat(0);
   gStyle->SetOptTitle(0);
   D0_BUP2020_AN_5yr_comparison->SetHighLightColor(2);
   D0_BUP2020_AN_5yr_comparison->Range(0,0,1,1);
   D0_BUP2020_AN_5yr_comparison->SetFillColor(0);
   D0_BUP2020_AN_5yr_comparison->SetBorderMode(0);
   D0_BUP2020_AN_5yr_comparison->SetBorderSize(2);
   D0_BUP2020_AN_5yr_comparison->SetTickx(1);
   D0_BUP2020_AN_5yr_comparison->SetTicky(1);
   D0_BUP2020_AN_5yr_comparison->SetLeftMargin(0.16);
   D0_BUP2020_AN_5yr_comparison->SetRightMargin(0.05);
   D0_BUP2020_AN_5yr_comparison->SetTopMargin(0.05);
   D0_BUP2020_AN_5yr_comparison->SetBottomMargin(0.16);
   D0_BUP2020_AN_5yr_comparison->SetFrameBorderMode(0);
  
// ------------>Primitives in pad: D0_BUP2020_AN_5yr_comparison_1
   TPad *D0_BUP2020_AN_5yr_comparison_1 = new TPad("D0_BUP2020_AN_5yr_comparison_1", "D0_BUP2020_AN_5yr_comparison_1",0.01,0.01,0.99,0.99);
   D0_BUP2020_AN_5yr_comparison_1->Draw();
   D0_BUP2020_AN_5yr_comparison_1->cd();
   D0_BUP2020_AN_5yr_comparison_1->Range(-1.012658,-0.0371519,5.316456,0.03879747);
   D0_BUP2020_AN_5yr_comparison_1->SetFillColor(0);
   D0_BUP2020_AN_5yr_comparison_1->SetBorderMode(0);
   D0_BUP2020_AN_5yr_comparison_1->SetBorderSize(2);
   D0_BUP2020_AN_5yr_comparison_1->SetTickx(1);
   D0_BUP2020_AN_5yr_comparison_1->SetTicky(1);
   D0_BUP2020_AN_5yr_comparison_1->SetLeftMargin(0.16);
   D0_BUP2020_AN_5yr_comparison_1->SetRightMargin(0.05);
   D0_BUP2020_AN_5yr_comparison_1->SetTopMargin(0.05);
   D0_BUP2020_AN_5yr_comparison_1->SetBottomMargin(0.16);
   D0_BUP2020_AN_5yr_comparison_1->SetFrameBorderMode(0);
   D0_BUP2020_AN_5yr_comparison_1->SetFrameBorderMode(0);
   
   TH1F *hframe__28 = new TH1F("hframe__28","",1000,0,5);
   hframe__28->SetMinimum(-0.025);
   hframe__28->SetMaximum(0.035);
   hframe__28->SetDirectory(0);
   hframe__28->SetStats(0);
   hframe__28->SetLineWidth(2);
   hframe__28->SetMarkerStyle(20);
   hframe__28->SetMarkerSize(1.2);
   hframe__28->GetXaxis()->SetTitle("#it{p}_{T} [GeV]");
   hframe__28->GetXaxis()->SetLabelFont(42);
   hframe__28->GetXaxis()->SetLabelSize(0.05);
   hframe__28->GetXaxis()->SetTitleSize(0.05);
   hframe__28->GetXaxis()->SetTitleOffset(1.4);
   hframe__28->GetXaxis()->SetTitleFont(42);
   hframe__28->GetYaxis()->SetTitle("A_{N}");
   hframe__28->GetYaxis()->SetLabelFont(42);
   hframe__28->GetYaxis()->SetLabelSize(0.05);
   hframe__28->GetYaxis()->SetTitleSize(0.05);
   hframe__28->GetYaxis()->SetTitleOffset(1.4);
   hframe__28->GetYaxis()->SetTitleFont(42);
   hframe__28->GetZaxis()->SetLabelFont(42);
   hframe__28->GetZaxis()->SetLabelSize(0.05);
   hframe__28->GetZaxis()->SetTitleSize(0.05);
   hframe__28->GetZaxis()->SetTitleOffset(1);
   hframe__28->GetZaxis()->SetTitleFont(42);
   hframe__28->Draw(" ");
   TLine *line = new TLine(0,-0,5,0);
   line->Draw();
   
   Double_t Graph0_fx37[30] = {
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
   Double_t Graph0_fy37[30] = {
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
   TGraph *graph = new TGraph(30,Graph0_fx37,Graph0_fy37);
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
   
   TH1F *Graph_Graph037 = new TH1F("Graph_Graph037","Graph",100,0.71,4.19);
   Graph_Graph037->SetMinimum(-0.0009172203);
   Graph_Graph037->SetMaximum(-0.0002893771);
   Graph_Graph037->SetDirectory(0);
   Graph_Graph037->SetStats(0);
   Graph_Graph037->SetLineWidth(2);
   Graph_Graph037->SetMarkerStyle(20);
   Graph_Graph037->SetMarkerSize(1.2);
   Graph_Graph037->GetXaxis()->SetLabelFont(42);
   Graph_Graph037->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph037->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph037->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph037->GetXaxis()->SetTitleFont(42);
   Graph_Graph037->GetYaxis()->SetLabelFont(42);
   Graph_Graph037->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph037->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph037->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph037->GetYaxis()->SetTitleFont(42);
   Graph_Graph037->GetZaxis()->SetLabelFont(42);
   Graph_Graph037->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph037->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph037->GetZaxis()->SetTitleOffset(1);
   Graph_Graph037->GetZaxis()->SetTitleFont(42);
   graph->SetHistogram(Graph_Graph037);
   
   graph->Draw("l");
   
   Double_t Graph1_fx38[30] = {
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
   Double_t Graph1_fy38[30] = {
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
   graph = new TGraph(30,Graph1_fx38,Graph1_fy38);
   graph->SetName("Graph1");
   graph->SetTitle("Graph");
   graph->SetFillStyle(1000);

   ci = TColor::GetColor("#000099");
   graph->SetLineColor(ci);
   graph->SetLineStyle(2);
   graph->SetLineWidth(5);
   graph->SetMarkerStyle(20);
   graph->SetMarkerSize(1.2);
   
   TH1F *Graph_Graph138 = new TH1F("Graph_Graph138","Graph",100,0.71,4.19);
   Graph_Graph138->SetMinimum(-0.0196607);
   Graph_Graph138->SetMaximum(-0.009327811);
   Graph_Graph138->SetDirectory(0);
   Graph_Graph138->SetStats(0);
   Graph_Graph138->SetLineWidth(2);
   Graph_Graph138->SetMarkerStyle(20);
   Graph_Graph138->SetMarkerSize(1.2);
   Graph_Graph138->GetXaxis()->SetLabelFont(42);
   Graph_Graph138->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph138->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph138->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph138->GetXaxis()->SetTitleFont(42);
   Graph_Graph138->GetYaxis()->SetLabelFont(42);
   Graph_Graph138->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph138->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph138->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph138->GetYaxis()->SetTitleFont(42);
   Graph_Graph138->GetZaxis()->SetLabelFont(42);
   Graph_Graph138->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph138->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph138->GetZaxis()->SetTitleOffset(1);
   Graph_Graph138->GetZaxis()->SetTitleFont(42);
   graph->SetHistogram(Graph_Graph138);
   
   graph->Draw("l");
   
   Double_t Graph2_fx1023[10] = {
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
   Double_t Graph2_fy1023[10] = {
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
   Double_t Graph2_fex1023[10] = {
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
   Double_t Graph2_fey1023[10] = {
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
   TGraphErrors *gre = new TGraphErrors(10,Graph2_fx1023,Graph2_fy1023,Graph2_fex1023,Graph2_fey1023);
   gre->SetName("Graph2");
   gre->SetTitle("Graph");
   gre->SetFillStyle(1000);

   ci = TColor::GetColor("#999999");
   gre->SetLineColor(ci);
   gre->SetLineWidth(4);

   ci = TColor::GetColor("#999999");
   gre->SetMarkerColor(ci);
   gre->SetMarkerStyle(24);
   gre->SetMarkerSize(2);
   
   TH1F *Graph_Graph21023 = new TH1F("Graph_Graph21023","Graph",100,0,10.4);
   Graph_Graph21023->SetMinimum(-0.0905336);
   Graph_Graph21023->SetMaximum(0.0905336);
   Graph_Graph21023->SetDirectory(0);
   Graph_Graph21023->SetStats(0);
   Graph_Graph21023->SetLineWidth(2);
   Graph_Graph21023->SetMarkerStyle(20);
   Graph_Graph21023->SetMarkerSize(1.2);
   Graph_Graph21023->GetXaxis()->SetLabelFont(42);
   Graph_Graph21023->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph21023->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph21023->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph21023->GetXaxis()->SetTitleFont(42);
   Graph_Graph21023->GetYaxis()->SetLabelFont(42);
   Graph_Graph21023->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph21023->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph21023->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph21023->GetYaxis()->SetTitleFont(42);
   Graph_Graph21023->GetZaxis()->SetLabelFont(42);
   Graph_Graph21023->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph21023->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph21023->GetZaxis()->SetTitleOffset(1);
   Graph_Graph21023->GetZaxis()->SetTitleFont(42);
   gre->SetHistogram(Graph_Graph21023);
   
   gre->Draw("p");
   
   Double_t Graph3_fx1024[20] = {
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
   Double_t Graph3_fy1024[20] = {
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
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
   Double_t Graph3_fex1024[20] = {
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
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
   Double_t Graph3_fey1024[20] = {
   0.002623881,
   0.001700133,
   0.001163185,
   0.001078443,
   0.001152524,
   0.001346386,
   0.001484601,
   0.001886026,
   0.00245292,
   0.003257298,
   0.00413699,
   0.005338863,
   0.006897833,
   0.008876149,
   0.009849684,
   0.01270509,
   0.01626371,
   0.02063689,
   0.025931,
   0.03235142};
   gre = new TGraphErrors(20,Graph3_fx1024,Graph3_fy1024,Graph3_fex1024,Graph3_fey1024);
   gre->SetName("Graph3");
   gre->SetTitle("Graph");
   gre->SetFillStyle(1000);

   ci = TColor::GetColor("#000000");
   gre->SetLineColor(ci);
   gre->SetLineWidth(4);

   ci = TColor::GetColor("#000000");
   gre->SetMarkerColor(ci);
   gre->SetMarkerStyle(20);
   gre->SetMarkerSize(2);
   
   TH1F *Graph_Graph31024 = new TH1F("Graph_Graph31024","Graph",100,0,10.7);
   Graph_Graph31024->SetMinimum(-0.0388217);
   Graph_Graph31024->SetMaximum(0.0388217);
   Graph_Graph31024->SetDirectory(0);
   Graph_Graph31024->SetStats(0);
   Graph_Graph31024->SetLineWidth(2);
   Graph_Graph31024->SetMarkerStyle(20);
   Graph_Graph31024->SetMarkerSize(1.2);
   Graph_Graph31024->GetXaxis()->SetLabelFont(42);
   Graph_Graph31024->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph31024->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph31024->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph31024->GetXaxis()->SetTitleFont(42);
   Graph_Graph31024->GetYaxis()->SetLabelFont(42);
   Graph_Graph31024->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph31024->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph31024->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph31024->GetYaxis()->SetTitleFont(42);
   Graph_Graph31024->GetZaxis()->SetLabelFont(42);
   Graph_Graph31024->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph31024->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph31024->GetZaxis()->SetTitleOffset(1);
   Graph_Graph31024->GetZaxis()->SetTitleFont(42);
   gre->SetHistogram(Graph_Graph31024);
   
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

   ci = TColor::GetColor("#999999");
   entry->SetLineColor(ci);
   entry->SetLineStyle(1);
   entry->SetLineWidth(4);

   ci = TColor::GetColor("#999999");
   entry->SetMarkerColor(ci);
   entry->SetMarkerStyle(24);
   entry->SetMarkerSize(2);
   entry->SetTextFont(42);
   entry=leg->AddEntry("Graph","86 pb^{-1} str. #it{p}+#it{p}, Years 1-5","lp");

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
   D0_BUP2020_AN_5yr_comparison_1->Modified();
   D0_BUP2020_AN_5yr_comparison->cd();
   D0_BUP2020_AN_5yr_comparison->Modified();
   D0_BUP2020_AN_5yr_comparison->cd();
   D0_BUP2020_AN_5yr_comparison->SetSelected(D0_BUP2020_AN_5yr_comparison);
}
