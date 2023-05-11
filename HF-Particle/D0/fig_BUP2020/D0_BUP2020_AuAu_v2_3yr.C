void D0_BUP2020_AuAu_v2_3yr()
{
//=========Macro generated from canvas: D0_BUP2020_AuAu_v2_3yr/D0_BUP2020_AuAu_v2_3yr
//=========  (Wed May 12 23:46:26 2021) by ROOT version 6.22/02
   TCanvas *D0_BUP2020_AuAu_v2_3yr = new TCanvas("D0_BUP2020_AuAu_v2_3yr", "D0_BUP2020_AuAu_v2_3yr",0,0,1100,800);
   gStyle->SetOptFit(1);
   gStyle->SetOptStat(0);
   gStyle->SetOptTitle(0);
   D0_BUP2020_AuAu_v2_3yr->SetHighLightColor(2);
   D0_BUP2020_AuAu_v2_3yr->Range(0,0,1,1);
   D0_BUP2020_AuAu_v2_3yr->SetFillColor(0);
   D0_BUP2020_AuAu_v2_3yr->SetBorderMode(0);
   D0_BUP2020_AuAu_v2_3yr->SetBorderSize(2);
   D0_BUP2020_AuAu_v2_3yr->SetTickx(1);
   D0_BUP2020_AuAu_v2_3yr->SetTicky(1);
   D0_BUP2020_AuAu_v2_3yr->SetLeftMargin(0.16);
   D0_BUP2020_AuAu_v2_3yr->SetRightMargin(0.05);
   D0_BUP2020_AuAu_v2_3yr->SetTopMargin(0.05);
   D0_BUP2020_AuAu_v2_3yr->SetBottomMargin(0.16);
   D0_BUP2020_AuAu_v2_3yr->SetFrameBorderMode(0);
  
// ------------>Primitives in pad: D0_BUP2020_AuAu_v2_3yr_1
   TPad *D0_BUP2020_AuAu_v2_3yr_1 = new TPad("D0_BUP2020_AuAu_v2_3yr_1", "D0_BUP2020_AuAu_v2_3yr_1",0.01,0.01,0.99,0.99);
   D0_BUP2020_AuAu_v2_3yr_1->Draw();
   D0_BUP2020_AuAu_v2_3yr_1->cd();
   D0_BUP2020_AuAu_v2_3yr_1->Range(-2.227848,-0.1708861,11.6962,0.2721519);
   D0_BUP2020_AuAu_v2_3yr_1->SetFillColor(0);
   D0_BUP2020_AuAu_v2_3yr_1->SetBorderMode(0);
   D0_BUP2020_AuAu_v2_3yr_1->SetBorderSize(2);
   D0_BUP2020_AuAu_v2_3yr_1->SetTickx(1);
   D0_BUP2020_AuAu_v2_3yr_1->SetTicky(1);
   D0_BUP2020_AuAu_v2_3yr_1->SetLeftMargin(0.16);
   D0_BUP2020_AuAu_v2_3yr_1->SetRightMargin(0.05);
   D0_BUP2020_AuAu_v2_3yr_1->SetTopMargin(0.05);
   D0_BUP2020_AuAu_v2_3yr_1->SetBottomMargin(0.16);
   D0_BUP2020_AuAu_v2_3yr_1->SetFrameBorderMode(0);
   D0_BUP2020_AuAu_v2_3yr_1->SetFrameBorderMode(0);
   
   TH1F *hframe__22 = new TH1F("hframe__22","",1000,0,11);
   hframe__22->SetMinimum(-0.1);
   hframe__22->SetMaximum(0.25);
   hframe__22->SetDirectory(0);
   hframe__22->SetStats(0);
   hframe__22->SetLineWidth(2);
   hframe__22->SetMarkerStyle(20);
   hframe__22->SetMarkerSize(1.2);
   hframe__22->GetXaxis()->SetTitle("#it{p}_{T} [GeV]");
   hframe__22->GetXaxis()->SetLabelFont(42);
   hframe__22->GetXaxis()->SetLabelSize(0.05);
   hframe__22->GetXaxis()->SetTitleSize(0.05);
   hframe__22->GetXaxis()->SetTitleOffset(1.4);
   hframe__22->GetXaxis()->SetTitleFont(42);
   hframe__22->GetYaxis()->SetTitle("v_{2}");
   hframe__22->GetYaxis()->SetLabelFont(42);
   hframe__22->GetYaxis()->SetLabelSize(0.05);
   hframe__22->GetYaxis()->SetTitleSize(0.05);
   hframe__22->GetYaxis()->SetTitleOffset(1.4);
   hframe__22->GetYaxis()->SetTitleFont(42);
   hframe__22->GetZaxis()->SetLabelFont(42);
   hframe__22->GetZaxis()->SetLabelSize(0.05);
   hframe__22->GetZaxis()->SetTitleSize(0.05);
   hframe__22->GetZaxis()->SetTitleOffset(1);
   hframe__22->GetZaxis()->SetTitleFont(42);
   hframe__22->Draw(" ");
   TLine *line = new TLine(0,-0,11,0);
   line->Draw();
   
   Double_t v2_B_fx29[20] = {
   0.4200009,
   1.246444,
   2.039168,
   2.791197,
   3.50468,
   4.185221,
   4.838745,
   5.470381,
   6.084274,
   6.683685,
   7.271172,
   7.848745,
   8.418,
   8.980208,
   9.5364,
   10.08741,
   10.63394,
   11.17655,
   11.71573,
   12.25187};
   Double_t v2_B_fy29[20] = {
   0.003230484,
   0.02045627,
   0.05875515,
   0.1035772,
   0.1351997,
   0.1468273,
   0.1426436,
   0.1296975,
   0.1135213,
   0.09734482,
   0.08267526,
   0.07001466,
   0.05936532,
   0.05052002,
   0.04320955,
   0.03716986,
   0.03216805,
   0.02800891,
   0.02453305,
   0.0216121};
   TGraph *graph = new TGraph(20,v2_B_fx29,v2_B_fy29);
   graph->SetName("v2_B");
   graph->SetTitle("Graph");

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#3333ff");
   graph->SetLineColor(ci);
   graph->SetLineWidth(2);
   graph->SetMarkerStyle(20);
   graph->SetMarkerSize(1.2);
   
   TH1F *Graph_v2_B29 = new TH1F("Graph_v2_B29","Graph",100,0,13.43505);
   Graph_v2_B29->SetMinimum(0);
   Graph_v2_B29->SetMaximum(0.1611869);
   Graph_v2_B29->SetDirectory(0);
   Graph_v2_B29->SetLineWidth(2);
   Graph_v2_B29->SetMarkerStyle(20);
   Graph_v2_B29->SetMarkerSize(1.2);
   Graph_v2_B29->GetXaxis()->SetLabelFont(42);
   Graph_v2_B29->GetXaxis()->SetLabelSize(0.05);
   Graph_v2_B29->GetXaxis()->SetTitleSize(0.05);
   Graph_v2_B29->GetXaxis()->SetTitleOffset(1.4);
   Graph_v2_B29->GetXaxis()->SetTitleFont(42);
   Graph_v2_B29->GetYaxis()->SetLabelFont(42);
   Graph_v2_B29->GetYaxis()->SetLabelSize(0.05);
   Graph_v2_B29->GetYaxis()->SetTitleSize(0.05);
   Graph_v2_B29->GetYaxis()->SetTitleOffset(1.4);
   Graph_v2_B29->GetYaxis()->SetTitleFont(42);
   Graph_v2_B29->GetZaxis()->SetLabelFont(42);
   Graph_v2_B29->GetZaxis()->SetLabelSize(0.05);
   Graph_v2_B29->GetZaxis()->SetTitleSize(0.05);
   Graph_v2_B29->GetZaxis()->SetTitleOffset(1);
   Graph_v2_B29->GetZaxis()->SetTitleFont(42);
   graph->SetHistogram(Graph_v2_B29);
   
   graph->Draw("");
   
   Double_t v2_D_fx30[20] = {
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
   Double_t v2_D_fy30[20] = {
   0.003230484,
   0.02045627,
   0.05875515,
   0.1035772,
   0.1351997,
   0.1468273,
   0.1426436,
   0.1296975,
   0.1135213,
   0.09734482,
   0.08267526,
   0.07001466,
   0.05936532,
   0.05052002,
   0.04320955,
   0.03716986,
   0.03216805,
   0.02800891,
   0.02453305,
   0.0216121};
   graph = new TGraph(20,v2_D_fx30,v2_D_fy30);
   graph->SetName("v2_D");
   graph->SetTitle("Graph");

   ci = TColor::GetColor("#000000");
   graph->SetLineColor(ci);
   graph->SetLineStyle(2);
   graph->SetLineWidth(2);
   graph->SetMarkerStyle(20);
   graph->SetMarkerSize(1.2);
   
   TH1F *Graph_v2_D30 = new TH1F("Graph_v2_D30","Graph",100,0,10.7);
   Graph_v2_D30->SetMinimum(0);
   Graph_v2_D30->SetMaximum(0.1611869);
   Graph_v2_D30->SetDirectory(0);
   Graph_v2_D30->SetLineWidth(2);
   Graph_v2_D30->SetMarkerStyle(20);
   Graph_v2_D30->SetMarkerSize(1.2);
   Graph_v2_D30->GetXaxis()->SetLabelFont(42);
   Graph_v2_D30->GetXaxis()->SetLabelSize(0.05);
   Graph_v2_D30->GetXaxis()->SetTitleSize(0.05);
   Graph_v2_D30->GetXaxis()->SetTitleOffset(1.4);
   Graph_v2_D30->GetXaxis()->SetTitleFont(42);
   Graph_v2_D30->GetYaxis()->SetLabelFont(42);
   Graph_v2_D30->GetYaxis()->SetLabelSize(0.05);
   Graph_v2_D30->GetYaxis()->SetTitleSize(0.05);
   Graph_v2_D30->GetYaxis()->SetTitleOffset(1.4);
   Graph_v2_D30->GetYaxis()->SetTitleFont(42);
   Graph_v2_D30->GetZaxis()->SetLabelFont(42);
   Graph_v2_D30->GetZaxis()->SetLabelSize(0.05);
   Graph_v2_D30->GetZaxis()->SetTitleSize(0.05);
   Graph_v2_D30->GetZaxis()->SetTitleOffset(1);
   Graph_v2_D30->GetZaxis()->SetTitleFont(42);
   graph->SetHistogram(Graph_v2_D30);
   
   graph->Draw("");
   
   Double_t Graph0_fx1011[20] = {
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
   Double_t Graph0_fy1011[20] = {
   0.025,
   0.025,
   0.025,
   0.025,
   0.025,
   0.025,
   0.025,
   0.025,
   0.025,
   0.025,
   0.025,
   0.025,
   0.025,
   0.025,
   0.025,
   0.025,
   0.025,
   0.025,
   0.025,
   0.025};
   Double_t Graph0_fex1011[20] = {
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
   Double_t Graph0_fey1011[20] = {
   0.002133623,
   0.001356161,
   0.0008824884,
   0.000720938,
   0.0007165389,
   0.0008040748,
   0.001092407,
   0.001410594,
   0.001853328,
   0.002409361,
   0.003082667,
   0.004021513,
   0.005262816,
   0.006880724,
   0.008858975,
   0.01154967,
   0.01498844,
   0.01921445,
   0.02463416,
   0.03110009};
   TGraphErrors *gre = new TGraphErrors(20,Graph0_fx1011,Graph0_fy1011,Graph0_fex1011,Graph0_fey1011);
   gre->SetName("Graph0");
   gre->SetTitle("Graph");
   gre->SetFillStyle(1000);

   ci = TColor::GetColor("#000000");
   gre->SetLineColor(ci);
   gre->SetLineWidth(4);

   ci = TColor::GetColor("#000000");
   gre->SetMarkerColor(ci);
   gre->SetMarkerStyle(20);
   gre->SetMarkerSize(2);
   
   TH1F *Graph_Graph01011 = new TH1F("Graph_Graph01011","Graph",100,0,10.7);
   Graph_Graph01011->SetMinimum(-0.0123201);
   Graph_Graph01011->SetMaximum(0.0623201);
   Graph_Graph01011->SetDirectory(0);
   Graph_Graph01011->SetStats(0);
   Graph_Graph01011->SetLineWidth(2);
   Graph_Graph01011->SetMarkerStyle(20);
   Graph_Graph01011->SetMarkerSize(1.2);
   Graph_Graph01011->GetXaxis()->SetLabelFont(42);
   Graph_Graph01011->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph01011->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph01011->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph01011->GetXaxis()->SetTitleFont(42);
   Graph_Graph01011->GetYaxis()->SetLabelFont(42);
   Graph_Graph01011->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph01011->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph01011->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph01011->GetYaxis()->SetTitleFont(42);
   Graph_Graph01011->GetZaxis()->SetLabelFont(42);
   Graph_Graph01011->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph01011->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph01011->GetZaxis()->SetTitleOffset(1);
   Graph_Graph01011->GetZaxis()->SetTitleFont(42);
   gre->SetHistogram(Graph_Graph01011);
   
   gre->Draw("p");
   
   Double_t Graph1_fx1012[8] = {
   2.5,
   3.5,
   4.5,
   5.5,
   6.5,
   7.5,
   8.5,
   9.5};
   Double_t Graph1_fy1012[8] = {
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0};
   Double_t Graph1_fex1012[8] = {
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0};
   Double_t Graph1_fey1012[8] = {
   0.02921846,
   0.01773862,
   0.01631992,
   0.01913001,
   0.02379811,
   0.03436945,
   0.04452312,
   0.06111874};
   gre = new TGraphErrors(8,Graph1_fx1012,Graph1_fy1012,Graph1_fex1012,Graph1_fey1012);
   gre->SetName("Graph1");
   gre->SetTitle("Graph");
   gre->SetFillStyle(1000);

   ci = TColor::GetColor("#0000cc");
   gre->SetLineColor(ci);
   gre->SetLineWidth(4);

   ci = TColor::GetColor("#0000cc");
   gre->SetMarkerColor(ci);
   gre->SetMarkerStyle(21);
   gre->SetMarkerSize(2);
   
   TH1F *Graph_Graph11012 = new TH1F("Graph_Graph11012","Graph",100,1.8,10.2);
   Graph_Graph11012->SetMinimum(-0.07334249);
   Graph_Graph11012->SetMaximum(0.07334249);
   Graph_Graph11012->SetDirectory(0);
   Graph_Graph11012->SetStats(0);
   Graph_Graph11012->SetLineWidth(2);
   Graph_Graph11012->SetMarkerStyle(20);
   Graph_Graph11012->SetMarkerSize(1.2);
   Graph_Graph11012->GetXaxis()->SetLabelFont(42);
   Graph_Graph11012->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph11012->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph11012->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph11012->GetXaxis()->SetTitleFont(42);
   Graph_Graph11012->GetYaxis()->SetLabelFont(42);
   Graph_Graph11012->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph11012->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph11012->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph11012->GetYaxis()->SetTitleFont(42);
   Graph_Graph11012->GetZaxis()->SetLabelFont(42);
   Graph_Graph11012->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph11012->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph11012->GetZaxis()->SetTitleOffset(1);
   Graph_Graph11012->GetZaxis()->SetTitleFont(42);
   gre->SetHistogram(Graph_Graph11012);
   
   gre->Draw("p");
   
   TLegend *leg = new TLegend(0,0.78,0.85,0.9,NULL,"brNDC");
   leg->SetBorderSize(0);
   leg->SetLineColor(1);
   leg->SetLineStyle(1);
   leg->SetLineWidth(1);
   leg->SetFillColor(0);
   leg->SetFillStyle(0);
   TLegendEntry *entry=leg->AddEntry("NULL","#it{#bf{sPHENIX}} Projection, 0-80% Au+Au, Years 1-3","");
   entry->SetLineColor(1);
   entry->SetLineStyle(1);
   entry->SetLineWidth(1);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   entry->SetTextFont(42);
   entry=leg->AddEntry("NULL","21 nb^{-1} rec. Au+Au, Res(#Psi_{2})=0.5","");
   entry->SetLineColor(1);
   entry->SetLineStyle(1);
   entry->SetLineWidth(1);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   entry->SetTextFont(42);
   leg->Draw();
   
   leg = new TLegend(0.65,0.6,0.9,0.77,NULL,"brNDC");
   leg->SetBorderSize(0);
   leg->SetLineColor(1);
   leg->SetLineStyle(1);
   leg->SetLineWidth(1);
   leg->SetFillColor(0);
   leg->SetFillStyle(0);
   entry=leg->AddEntry("Graph","#it{B}#rightarrow#it{D}^{0}","lp");

   ci = TColor::GetColor("#0000cc");
   entry->SetLineColor(ci);
   entry->SetLineStyle(1);
   entry->SetLineWidth(4);

   ci = TColor::GetColor("#0000cc");
   entry->SetMarkerColor(ci);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(2);
   entry->SetTextFont(42);
   entry=leg->AddEntry("Graph","Prompt #it{D}^{0}","lp");

   ci = TColor::GetColor("#000000");
   entry->SetLineColor(ci);
   entry->SetLineStyle(1);
   entry->SetLineWidth(4);

   ci = TColor::GetColor("#000000");
   entry->SetMarkerColor(ci);
   entry->SetMarkerStyle(20);
   entry->SetMarkerSize(2);
   entry->SetTextFont(42);
   leg->Draw();
   
   leg = new TLegend(0.2,0.2,0.7,0.3,NULL,"brNDC");
   leg->SetBorderSize(0);
   leg->SetLineColor(1);
   leg->SetLineStyle(1);
   leg->SetLineWidth(1);
   leg->SetFillColor(0);
   leg->SetFillStyle(0);
   entry=leg->AddEntry("v2_D","#it{D}-meson (fit to STAR PRL#bf{118})","l");

   ci = TColor::GetColor("#000000");
   entry->SetLineColor(ci);
   entry->SetLineStyle(2);
   entry->SetLineWidth(2);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   entry->SetTextFont(42);
   entry=leg->AddEntry("v2_B","#it{B}-meson (m_{T} scaling)","l");

   ci = TColor::GetColor("#3333ff");
   entry->SetLineColor(ci);
   entry->SetLineStyle(1);
   entry->SetLineWidth(2);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   entry->SetTextFont(42);
   leg->Draw();
   D0_BUP2020_AuAu_v2_3yr_1->Modified();
   D0_BUP2020_AuAu_v2_3yr->cd();
   D0_BUP2020_AuAu_v2_3yr->Modified();
   D0_BUP2020_AuAu_v2_3yr->cd();
   D0_BUP2020_AuAu_v2_3yr->SetSelected(D0_BUP2020_AuAu_v2_3yr);
}
