void D0_BUP2020_AuAu_v2_3yr_20wk_comp()
{
//=========Macro generated from canvas: D0_BUP2020_AuAu_v2_3yr_20wk_comp/D0_BUP2020_AuAu_v2_3yr_20wk_comp
//=========  (Wed May 12 23:46:26 2021) by ROOT version 6.22/02
   TCanvas *D0_BUP2020_AuAu_v2_3yr_20wk_comp = new TCanvas("D0_BUP2020_AuAu_v2_3yr_20wk_comp", "D0_BUP2020_AuAu_v2_3yr_20wk_comp",0,0,1100,800);
   gStyle->SetOptFit(1);
   gStyle->SetOptStat(0);
   gStyle->SetOptTitle(0);
   D0_BUP2020_AuAu_v2_3yr_20wk_comp->SetHighLightColor(2);
   D0_BUP2020_AuAu_v2_3yr_20wk_comp->Range(0,0,1,1);
   D0_BUP2020_AuAu_v2_3yr_20wk_comp->SetFillColor(0);
   D0_BUP2020_AuAu_v2_3yr_20wk_comp->SetBorderMode(0);
   D0_BUP2020_AuAu_v2_3yr_20wk_comp->SetBorderSize(2);
   D0_BUP2020_AuAu_v2_3yr_20wk_comp->SetTickx(1);
   D0_BUP2020_AuAu_v2_3yr_20wk_comp->SetTicky(1);
   D0_BUP2020_AuAu_v2_3yr_20wk_comp->SetLeftMargin(0.16);
   D0_BUP2020_AuAu_v2_3yr_20wk_comp->SetRightMargin(0.05);
   D0_BUP2020_AuAu_v2_3yr_20wk_comp->SetTopMargin(0.05);
   D0_BUP2020_AuAu_v2_3yr_20wk_comp->SetBottomMargin(0.16);
   D0_BUP2020_AuAu_v2_3yr_20wk_comp->SetFrameBorderMode(0);
  
// ------------>Primitives in pad: D0_BUP2020_AuAu_v2_3yr_20wk_comp_1
   TPad *D0_BUP2020_AuAu_v2_3yr_20wk_comp_1 = new TPad("D0_BUP2020_AuAu_v2_3yr_20wk_comp_1", "D0_BUP2020_AuAu_v2_3yr_20wk_comp_1",0.01,0.01,0.99,0.99);
   D0_BUP2020_AuAu_v2_3yr_20wk_comp_1->Draw();
   D0_BUP2020_AuAu_v2_3yr_20wk_comp_1->cd();
   D0_BUP2020_AuAu_v2_3yr_20wk_comp_1->Range(-2.227848,-0.1708861,11.6962,0.2721519);
   D0_BUP2020_AuAu_v2_3yr_20wk_comp_1->SetFillColor(0);
   D0_BUP2020_AuAu_v2_3yr_20wk_comp_1->SetBorderMode(0);
   D0_BUP2020_AuAu_v2_3yr_20wk_comp_1->SetBorderSize(2);
   D0_BUP2020_AuAu_v2_3yr_20wk_comp_1->SetTickx(1);
   D0_BUP2020_AuAu_v2_3yr_20wk_comp_1->SetTicky(1);
   D0_BUP2020_AuAu_v2_3yr_20wk_comp_1->SetLeftMargin(0.16);
   D0_BUP2020_AuAu_v2_3yr_20wk_comp_1->SetRightMargin(0.05);
   D0_BUP2020_AuAu_v2_3yr_20wk_comp_1->SetTopMargin(0.05);
   D0_BUP2020_AuAu_v2_3yr_20wk_comp_1->SetBottomMargin(0.16);
   D0_BUP2020_AuAu_v2_3yr_20wk_comp_1->SetFrameBorderMode(0);
   D0_BUP2020_AuAu_v2_3yr_20wk_comp_1->SetFrameBorderMode(0);
   
   TH1F *hframe__23 = new TH1F("hframe__23","",1000,0,11);
   hframe__23->SetMinimum(-0.1);
   hframe__23->SetMaximum(0.25);
   hframe__23->SetDirectory(0);
   hframe__23->SetStats(0);
   hframe__23->SetLineWidth(2);
   hframe__23->SetMarkerStyle(20);
   hframe__23->SetMarkerSize(1.2);
   hframe__23->GetXaxis()->SetTitle("#it{p}_{T} [GeV]");
   hframe__23->GetXaxis()->SetLabelFont(42);
   hframe__23->GetXaxis()->SetLabelSize(0.05);
   hframe__23->GetXaxis()->SetTitleSize(0.05);
   hframe__23->GetXaxis()->SetTitleOffset(1.4);
   hframe__23->GetXaxis()->SetTitleFont(42);
   hframe__23->GetYaxis()->SetTitle("v_{2}");
   hframe__23->GetYaxis()->SetLabelFont(42);
   hframe__23->GetYaxis()->SetLabelSize(0.05);
   hframe__23->GetYaxis()->SetTitleSize(0.05);
   hframe__23->GetYaxis()->SetTitleOffset(1.4);
   hframe__23->GetYaxis()->SetTitleFont(42);
   hframe__23->GetZaxis()->SetLabelFont(42);
   hframe__23->GetZaxis()->SetLabelSize(0.05);
   hframe__23->GetZaxis()->SetTitleSize(0.05);
   hframe__23->GetZaxis()->SetTitleOffset(1);
   hframe__23->GetZaxis()->SetTitleFont(42);
   hframe__23->Draw(" ");
   TLine *line = new TLine(0,-0,11,0);
   line->Draw();
   
   Double_t v2_B_fx31[20] = {
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
   Double_t v2_B_fy31[20] = {
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
   TGraph *graph = new TGraph(20,v2_B_fx31,v2_B_fy31);
   graph->SetName("v2_B");
   graph->SetTitle("Graph");

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#3333ff");
   graph->SetLineColor(ci);
   graph->SetLineWidth(2);
   graph->SetMarkerStyle(20);
   graph->SetMarkerSize(1.2);
   
   TH1F *Graph_v2_B31 = new TH1F("Graph_v2_B31","Graph",100,0,13.43505);
   Graph_v2_B31->SetMinimum(0);
   Graph_v2_B31->SetMaximum(0.1611869);
   Graph_v2_B31->SetDirectory(0);
   Graph_v2_B31->SetLineWidth(2);
   Graph_v2_B31->SetMarkerStyle(20);
   Graph_v2_B31->SetMarkerSize(1.2);
   Graph_v2_B31->GetXaxis()->SetLabelFont(42);
   Graph_v2_B31->GetXaxis()->SetLabelSize(0.05);
   Graph_v2_B31->GetXaxis()->SetTitleSize(0.05);
   Graph_v2_B31->GetXaxis()->SetTitleOffset(1.4);
   Graph_v2_B31->GetXaxis()->SetTitleFont(42);
   Graph_v2_B31->GetYaxis()->SetLabelFont(42);
   Graph_v2_B31->GetYaxis()->SetLabelSize(0.05);
   Graph_v2_B31->GetYaxis()->SetTitleSize(0.05);
   Graph_v2_B31->GetYaxis()->SetTitleOffset(1.4);
   Graph_v2_B31->GetYaxis()->SetTitleFont(42);
   Graph_v2_B31->GetZaxis()->SetLabelFont(42);
   Graph_v2_B31->GetZaxis()->SetLabelSize(0.05);
   Graph_v2_B31->GetZaxis()->SetTitleSize(0.05);
   Graph_v2_B31->GetZaxis()->SetTitleOffset(1);
   Graph_v2_B31->GetZaxis()->SetTitleFont(42);
   graph->SetHistogram(Graph_v2_B31);
   
   graph->Draw("");
   
   Double_t v2_D_fx32[20] = {
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
   Double_t v2_D_fy32[20] = {
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
   graph = new TGraph(20,v2_D_fx32,v2_D_fy32);
   graph->SetName("v2_D");
   graph->SetTitle("Graph");

   ci = TColor::GetColor("#000000");
   graph->SetLineColor(ci);
   graph->SetLineStyle(2);
   graph->SetLineWidth(2);
   graph->SetMarkerStyle(20);
   graph->SetMarkerSize(1.2);
   
   TH1F *Graph_v2_D32 = new TH1F("Graph_v2_D32","Graph",100,0,10.7);
   Graph_v2_D32->SetMinimum(0);
   Graph_v2_D32->SetMaximum(0.1611869);
   Graph_v2_D32->SetDirectory(0);
   Graph_v2_D32->SetLineWidth(2);
   Graph_v2_D32->SetMarkerStyle(20);
   Graph_v2_D32->SetMarkerSize(1.2);
   Graph_v2_D32->GetXaxis()->SetLabelFont(42);
   Graph_v2_D32->GetXaxis()->SetLabelSize(0.05);
   Graph_v2_D32->GetXaxis()->SetTitleSize(0.05);
   Graph_v2_D32->GetXaxis()->SetTitleOffset(1.4);
   Graph_v2_D32->GetXaxis()->SetTitleFont(42);
   Graph_v2_D32->GetYaxis()->SetLabelFont(42);
   Graph_v2_D32->GetYaxis()->SetLabelSize(0.05);
   Graph_v2_D32->GetYaxis()->SetTitleSize(0.05);
   Graph_v2_D32->GetYaxis()->SetTitleOffset(1.4);
   Graph_v2_D32->GetYaxis()->SetTitleFont(42);
   Graph_v2_D32->GetZaxis()->SetLabelFont(42);
   Graph_v2_D32->GetZaxis()->SetLabelSize(0.05);
   Graph_v2_D32->GetZaxis()->SetTitleSize(0.05);
   Graph_v2_D32->GetZaxis()->SetTitleOffset(1);
   Graph_v2_D32->GetZaxis()->SetTitleFont(42);
   graph->SetHistogram(Graph_v2_D32);
   
   graph->Draw("");
   
   Double_t Graph0_fx1013[8] = {
   2.5,
   3.5,
   4.5,
   5.5,
   6.5,
   7.5,
   8.5,
   9.5};
   Double_t Graph0_fy1013[8] = {
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0};
   Double_t Graph0_fex1013[8] = {
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0};
   Double_t Graph0_fey1013[8] = {
   0.02921846,
   0.01773862,
   0.01631992,
   0.01913001,
   0.02379811,
   0.03436945,
   0.04452312,
   0.06111874};
   TGraphErrors *gre = new TGraphErrors(8,Graph0_fx1013,Graph0_fy1013,Graph0_fex1013,Graph0_fey1013);
   gre->SetName("Graph0");
   gre->SetTitle("Graph");
   gre->SetFillStyle(1000);

   ci = TColor::GetColor("#0000cc");
   gre->SetLineColor(ci);
   gre->SetLineWidth(4);

   ci = TColor::GetColor("#0000cc");
   gre->SetMarkerColor(ci);
   gre->SetMarkerStyle(21);
   gre->SetMarkerSize(2);
   
   TH1F *Graph_Graph01013 = new TH1F("Graph_Graph01013","Graph",100,1.8,10.2);
   Graph_Graph01013->SetMinimum(-0.07334249);
   Graph_Graph01013->SetMaximum(0.07334249);
   Graph_Graph01013->SetDirectory(0);
   Graph_Graph01013->SetStats(0);
   Graph_Graph01013->SetLineWidth(2);
   Graph_Graph01013->SetMarkerStyle(20);
   Graph_Graph01013->SetMarkerSize(1.2);
   Graph_Graph01013->GetXaxis()->SetLabelFont(42);
   Graph_Graph01013->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph01013->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph01013->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph01013->GetXaxis()->SetTitleFont(42);
   Graph_Graph01013->GetYaxis()->SetLabelFont(42);
   Graph_Graph01013->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph01013->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph01013->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph01013->GetYaxis()->SetTitleFont(42);
   Graph_Graph01013->GetZaxis()->SetLabelFont(42);
   Graph_Graph01013->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph01013->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph01013->GetZaxis()->SetTitleOffset(1);
   Graph_Graph01013->GetZaxis()->SetTitleFont(42);
   gre->SetHistogram(Graph_Graph01013);
   
   gre->Draw("p");
   
   Double_t Graph1_fx1014[8] = {
   2.75,
   3.75,
   4.75,
   5.75,
   6.75,
   7.75,
   8.75,
   9.75};
   Double_t Graph1_fy1014[8] = {
   0.025,
   0.025,
   0.025,
   0.025,
   0.025,
   0.025,
   0.025,
   0.025};
   Double_t Graph1_fex1014[8] = {
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0};
   Double_t Graph1_fey1014[8] = {
   0.03884449,
   0.02358262,
   0.02169652,
   0.02543239,
   0.0316384,
   0.04569247,
   0.05919128,
   0.08125433};
   gre = new TGraphErrors(8,Graph1_fx1014,Graph1_fy1014,Graph1_fex1014,Graph1_fey1014);
   gre->SetName("Graph1");
   gre->SetTitle("Graph");
   gre->SetFillStyle(1000);

   ci = TColor::GetColor("#00cccc");
   gre->SetLineColor(ci);
   gre->SetLineWidth(4);

   ci = TColor::GetColor("#00cccc");
   gre->SetMarkerColor(ci);
   gre->SetMarkerStyle(20);
   gre->SetMarkerSize(2);
   
   TH1F *Graph_Graph11014 = new TH1F("Graph_Graph11014","Graph",100,2.05,10.45);
   Graph_Graph11014->SetMinimum(-0.07250519);
   Graph_Graph11014->SetMaximum(0.1225052);
   Graph_Graph11014->SetDirectory(0);
   Graph_Graph11014->SetStats(0);
   Graph_Graph11014->SetLineWidth(2);
   Graph_Graph11014->SetMarkerStyle(20);
   Graph_Graph11014->SetMarkerSize(1.2);
   Graph_Graph11014->GetXaxis()->SetLabelFont(42);
   Graph_Graph11014->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph11014->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph11014->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph11014->GetXaxis()->SetTitleFont(42);
   Graph_Graph11014->GetYaxis()->SetLabelFont(42);
   Graph_Graph11014->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph11014->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph11014->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph11014->GetYaxis()->SetTitleFont(42);
   Graph_Graph11014->GetZaxis()->SetLabelFont(42);
   Graph_Graph11014->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph11014->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph11014->GetZaxis()->SetTitleOffset(1);
   Graph_Graph11014->GetZaxis()->SetTitleFont(42);
   gre->SetHistogram(Graph_Graph11014);
   
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
   entry=leg->AddEntry("NULL","#it{B}#rightarrow#it{D}^{0}, Res(#Psi_{2})=0.5","");
   entry->SetLineColor(1);
   entry->SetLineStyle(1);
   entry->SetLineWidth(1);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   entry->SetTextFont(42);
   leg->Draw();
   
   leg = new TLegend(0.55,0.65,0.85,0.83,NULL,"brNDC");
   leg->SetBorderSize(0);
   leg->SetLineColor(1);
   leg->SetLineStyle(1);
   leg->SetLineWidth(1);
   leg->SetFillColor(0);
   leg->SetFillStyle(0);
   entry=leg->AddEntry("Graph","28wk: 21 nb^{-1} rec. ","lp");

   ci = TColor::GetColor("#0000cc");
   entry->SetLineColor(ci);
   entry->SetLineStyle(1);
   entry->SetLineWidth(4);

   ci = TColor::GetColor("#0000cc");
   entry->SetMarkerColor(ci);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(2);
   entry->SetTextFont(42);
   entry=leg->AddEntry("Graph","20wk: 12 nb^{-1} rec.","lp");

   ci = TColor::GetColor("#00cccc");
   entry->SetLineColor(ci);
   entry->SetLineStyle(1);
   entry->SetLineWidth(4);

   ci = TColor::GetColor("#00cccc");
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
   D0_BUP2020_AuAu_v2_3yr_20wk_comp_1->Modified();
   D0_BUP2020_AuAu_v2_3yr_20wk_comp->cd();
   D0_BUP2020_AuAu_v2_3yr_20wk_comp->Modified();
   D0_BUP2020_AuAu_v2_3yr_20wk_comp->cd();
   D0_BUP2020_AuAu_v2_3yr_20wk_comp->SetSelected(D0_BUP2020_AuAu_v2_3yr_20wk_comp);
}
