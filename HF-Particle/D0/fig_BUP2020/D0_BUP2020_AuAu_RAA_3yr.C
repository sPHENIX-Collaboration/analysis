void D0_BUP2020_AuAu_RAA_3yr()
{
//=========Macro generated from canvas: D0_BUP2020_AuAu_RAA_3yr/D0_BUP2020_AuAu_RAA_3yr
//=========  (Wed May 12 23:46:24 2021) by ROOT version 6.22/02
   TCanvas *D0_BUP2020_AuAu_RAA_3yr = new TCanvas("D0_BUP2020_AuAu_RAA_3yr", "D0_BUP2020_AuAu_RAA_3yr",0,0,1100,800);
   gStyle->SetOptFit(1);
   gStyle->SetOptStat(0);
   gStyle->SetOptTitle(0);
   D0_BUP2020_AuAu_RAA_3yr->SetHighLightColor(2);
   D0_BUP2020_AuAu_RAA_3yr->Range(0,0,1,1);
   D0_BUP2020_AuAu_RAA_3yr->SetFillColor(0);
   D0_BUP2020_AuAu_RAA_3yr->SetBorderMode(0);
   D0_BUP2020_AuAu_RAA_3yr->SetBorderSize(2);
   D0_BUP2020_AuAu_RAA_3yr->SetTickx(1);
   D0_BUP2020_AuAu_RAA_3yr->SetTicky(1);
   D0_BUP2020_AuAu_RAA_3yr->SetLeftMargin(0.16);
   D0_BUP2020_AuAu_RAA_3yr->SetRightMargin(0.05);
   D0_BUP2020_AuAu_RAA_3yr->SetTopMargin(0.05);
   D0_BUP2020_AuAu_RAA_3yr->SetBottomMargin(0.16);
   D0_BUP2020_AuAu_RAA_3yr->SetFrameBorderMode(0);
  
// ------------>Primitives in pad: D0_BUP2020_AuAu_RAA_3yr_1
   TPad *D0_BUP2020_AuAu_RAA_3yr_1 = new TPad("D0_BUP2020_AuAu_RAA_3yr_1", "D0_BUP2020_AuAu_RAA_3yr_1",0.01,0.01,0.99,0.99);
   D0_BUP2020_AuAu_RAA_3yr_1->Draw();
   D0_BUP2020_AuAu_RAA_3yr_1->cd();
   D0_BUP2020_AuAu_RAA_3yr_1->Range(-2.227848,-0.3240506,11.6962,1.701266);
   D0_BUP2020_AuAu_RAA_3yr_1->SetFillColor(0);
   D0_BUP2020_AuAu_RAA_3yr_1->SetBorderMode(0);
   D0_BUP2020_AuAu_RAA_3yr_1->SetBorderSize(2);
   D0_BUP2020_AuAu_RAA_3yr_1->SetTickx(1);
   D0_BUP2020_AuAu_RAA_3yr_1->SetTicky(1);
   D0_BUP2020_AuAu_RAA_3yr_1->SetLeftMargin(0.16);
   D0_BUP2020_AuAu_RAA_3yr_1->SetRightMargin(0.05);
   D0_BUP2020_AuAu_RAA_3yr_1->SetTopMargin(0.05);
   D0_BUP2020_AuAu_RAA_3yr_1->SetBottomMargin(0.16);
   D0_BUP2020_AuAu_RAA_3yr_1->SetFrameBorderMode(0);
   D0_BUP2020_AuAu_RAA_3yr_1->SetFrameBorderMode(0);
   
   TH1F *hframe__19 = new TH1F("hframe__19","",1000,0,11);
   hframe__19->SetMinimum(0);
   hframe__19->SetMaximum(1.6);
   hframe__19->SetDirectory(0);
   hframe__19->SetStats(0);
   hframe__19->SetLineWidth(2);
   hframe__19->SetMarkerStyle(20);
   hframe__19->SetMarkerSize(1.2);
   hframe__19->GetXaxis()->SetTitle("#it{p}_{T} [GeV]");
   hframe__19->GetXaxis()->SetLabelFont(42);
   hframe__19->GetXaxis()->SetLabelSize(0.05);
   hframe__19->GetXaxis()->SetTitleSize(0.05);
   hframe__19->GetXaxis()->SetTitleOffset(1.4);
   hframe__19->GetXaxis()->SetTitleFont(42);
   hframe__19->GetYaxis()->SetTitle("#it{R}_{AA}");
   hframe__19->GetYaxis()->SetLabelFont(42);
   hframe__19->GetYaxis()->SetLabelSize(0.05);
   hframe__19->GetYaxis()->SetTitleSize(0.05);
   hframe__19->GetYaxis()->SetTitleOffset(1.4);
   hframe__19->GetYaxis()->SetTitleFont(42);
   hframe__19->GetZaxis()->SetLabelFont(42);
   hframe__19->GetZaxis()->SetLabelSize(0.05);
   hframe__19->GetZaxis()->SetTitleSize(0.05);
   hframe__19->GetZaxis()->SetTitleOffset(1);
   hframe__19->GetZaxis()->SetTitleFont(42);
   hframe__19->Draw(" ");
   
   Double_t RAA_pi_fx25[21] = {
   1.25,
   1.75,
   2.25,
   2.75,
   3.25,
   3.75,
   4.25,
   5.25,
   5.75,
   6.25,
   7.5,
   8.5,
   9.5,
   12.5,
   17.5,
   22.5,
   27.5,
   32.5,
   37.5,
   42.5,
   47.5};
   Double_t RAA_pi_fy25[21] = {
   0.364,
   0.4123,
   0.3958,
   0.3159,
   0.2556,
   0.2272,
   0.2103,
   0.1855,
   0.1854,
   0.1911,
   0.2108347,
   0.2248045,
   0.2379755,
   0.2730735,
   0.3189244,
   0.3522533,
   0.376236,
   0.3934406,
   0.4059054,
   0.4152112,
   0.4225492};
   TGraph *graph = new TGraph(21,RAA_pi_fx25,RAA_pi_fy25);
   graph->SetName("RAA_pi");
   graph->SetTitle("Graph");

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#009900");
   graph->SetLineColor(ci);
   graph->SetLineWidth(2);
   graph->SetMarkerStyle(20);
   graph->SetMarkerSize(1.2);
   
   TH1F *Graph_RAA_pi25 = new TH1F("Graph_RAA_pi25","Graph",100,1.125,52.125);
   Graph_RAA_pi25->SetMinimum(0.1616851);
   Graph_RAA_pi25->SetMaximum(0.4462641);
   Graph_RAA_pi25->SetDirectory(0);
   Graph_RAA_pi25->SetLineWidth(2);
   Graph_RAA_pi25->SetMarkerStyle(20);
   Graph_RAA_pi25->SetMarkerSize(1.2);
   Graph_RAA_pi25->GetXaxis()->SetLabelFont(42);
   Graph_RAA_pi25->GetXaxis()->SetLabelSize(0.05);
   Graph_RAA_pi25->GetXaxis()->SetTitleSize(0.05);
   Graph_RAA_pi25->GetXaxis()->SetTitleOffset(1.4);
   Graph_RAA_pi25->GetXaxis()->SetTitleFont(42);
   Graph_RAA_pi25->GetYaxis()->SetLabelFont(42);
   Graph_RAA_pi25->GetYaxis()->SetLabelSize(0.05);
   Graph_RAA_pi25->GetYaxis()->SetTitleSize(0.05);
   Graph_RAA_pi25->GetYaxis()->SetTitleOffset(1.4);
   Graph_RAA_pi25->GetYaxis()->SetTitleFont(42);
   Graph_RAA_pi25->GetZaxis()->SetLabelFont(42);
   Graph_RAA_pi25->GetZaxis()->SetLabelSize(0.05);
   Graph_RAA_pi25->GetZaxis()->SetTitleSize(0.05);
   Graph_RAA_pi25->GetZaxis()->SetTitleOffset(1);
   Graph_RAA_pi25->GetZaxis()->SetTitleFont(42);
   graph->SetHistogram(Graph_RAA_pi25);
   
   graph->Draw("");
   
   Double_t RAA_B_fx26[23] = {
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
   6.25,
   7.25,
   8.25,
   9.25,
   12.5,
   17.5,
   22.5,
   27.5,
   32.5,
   37.5,
   42.5,
   47.5};
   Double_t RAA_B_fy26[23] = {
   0.7919363,
   0.8851137,
   0.9790749,
   1.034256,
   1.085165,
   1.075569,
   1.047355,
   0.9999001,
   0.9357845,
   0.8760597,
   0.8136347,
   0.7157307,
   0.6335103,
   0.5680183,
   0.5241536,
   0.4155918,
   0.3573316,
   0.3401377,
   0.3305569,
   0.3272414,
   0.3282991,
   0.332189,
   0.337686};
   graph = new TGraph(23,RAA_B_fx26,RAA_B_fy26);
   graph->SetName("RAA_B");
   graph->SetTitle("Graph");

   ci = TColor::GetColor("#3333ff");
   graph->SetLineColor(ci);
   graph->SetLineWidth(2);
   graph->SetMarkerStyle(20);
   graph->SetMarkerSize(1.2);
   
   TH1F *Graph_RAA_B26 = new TH1F("Graph_RAA_B26","Graph",100,0.225,52.225);
   Graph_RAA_B26->SetMinimum(0.2514491);
   Graph_RAA_B26->SetMaximum(1.160957);
   Graph_RAA_B26->SetDirectory(0);
   Graph_RAA_B26->SetLineWidth(2);
   Graph_RAA_B26->SetMarkerStyle(20);
   Graph_RAA_B26->SetMarkerSize(1.2);
   Graph_RAA_B26->GetXaxis()->SetLabelFont(42);
   Graph_RAA_B26->GetXaxis()->SetLabelSize(0.05);
   Graph_RAA_B26->GetXaxis()->SetTitleSize(0.05);
   Graph_RAA_B26->GetXaxis()->SetTitleOffset(1.4);
   Graph_RAA_B26->GetXaxis()->SetTitleFont(42);
   Graph_RAA_B26->GetYaxis()->SetLabelFont(42);
   Graph_RAA_B26->GetYaxis()->SetLabelSize(0.05);
   Graph_RAA_B26->GetYaxis()->SetTitleSize(0.05);
   Graph_RAA_B26->GetYaxis()->SetTitleOffset(1.4);
   Graph_RAA_B26->GetYaxis()->SetTitleFont(42);
   Graph_RAA_B26->GetZaxis()->SetLabelFont(42);
   Graph_RAA_B26->GetZaxis()->SetLabelSize(0.05);
   Graph_RAA_B26->GetZaxis()->SetTitleSize(0.05);
   Graph_RAA_B26->GetZaxis()->SetTitleOffset(1);
   Graph_RAA_B26->GetZaxis()->SetTitleFont(42);
   graph->SetHistogram(Graph_RAA_B26);
   
   graph->Draw("");
   
   Double_t RAA_D_fx27[23] = {
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
   6.25,
   7.25,
   8.25,
   9.25,
   12.5,
   17.5,
   22.5,
   27.5,
   32.5,
   37.5,
   42.5,
   47.5};
   Double_t RAA_D_fy27[23] = {
   0.5375912,
   0.7987257,
   1.078158,
   1.162469,
   1.098154,
   0.8777246,
   0.6481578,
   0.5014529,
   0.3921144,
   0.3353954,
   0.3012415,
   0.2515049,
   0.2276756,
   0.2284476,
   0.2313802,
   0.2505994,
   0.2746772,
   0.2927324,
   0.306397,
   0.3169513,
   0.3253756,
   0.3323967,
   0.3385319};
   graph = new TGraph(23,RAA_D_fx27,RAA_D_fy27);
   graph->SetName("RAA_D");
   graph->SetTitle("Graph");

   ci = TColor::GetColor("#000000");
   graph->SetLineColor(ci);
   graph->SetLineStyle(2);
   graph->SetLineWidth(2);
   graph->SetMarkerStyle(20);
   graph->SetMarkerSize(1.2);
   
   TH1F *Graph_RAA_D27 = new TH1F("Graph_RAA_D27","Graph",100,0.225,52.225);
   Graph_RAA_D27->SetMinimum(0.1341963);
   Graph_RAA_D27->SetMaximum(1.255948);
   Graph_RAA_D27->SetDirectory(0);
   Graph_RAA_D27->SetLineWidth(2);
   Graph_RAA_D27->SetMarkerStyle(20);
   Graph_RAA_D27->SetMarkerSize(1.2);
   Graph_RAA_D27->GetXaxis()->SetLabelFont(42);
   Graph_RAA_D27->GetXaxis()->SetLabelSize(0.05);
   Graph_RAA_D27->GetXaxis()->SetTitleSize(0.05);
   Graph_RAA_D27->GetXaxis()->SetTitleOffset(1.4);
   Graph_RAA_D27->GetXaxis()->SetTitleFont(42);
   Graph_RAA_D27->GetYaxis()->SetLabelFont(42);
   Graph_RAA_D27->GetYaxis()->SetLabelSize(0.05);
   Graph_RAA_D27->GetYaxis()->SetTitleSize(0.05);
   Graph_RAA_D27->GetYaxis()->SetTitleOffset(1.4);
   Graph_RAA_D27->GetYaxis()->SetTitleFont(42);
   Graph_RAA_D27->GetZaxis()->SetLabelFont(42);
   Graph_RAA_D27->GetZaxis()->SetLabelSize(0.05);
   Graph_RAA_D27->GetZaxis()->SetTitleSize(0.05);
   Graph_RAA_D27->GetZaxis()->SetTitleOffset(1);
   Graph_RAA_D27->GetZaxis()->SetTitleFont(42);
   graph->SetHistogram(Graph_RAA_D27);
   
   graph->Draw("");
   
   Double_t RAA_D0_B_fx28[27] = {
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
   9.75,
   10.5,
   11.5,
   12.5,
   13.5,
   14.5,
   15.5,
   30};
   Double_t RAA_D0_B_fy28[27] = {
   0.9838097,
   1.002943,
   0.9880626,
   0.9700977,
   0.9535339,
   0.9354688,
   0.885242,
   0.8401878,
   0.7672503,
   0.7206252,
   0.6574549,
   0.621044,
   0.5733797,
   0.5464847,
   0.5120443,
   0.4925487,
   0.4675513,
   0.453502,
   0.4338281,
   0.4226899,
   0.4031777,
   0.3852578,
   0.3725463,
   0.3635795,
   0.3550752,
   0.3492906,
   0.3288992};
   graph = new TGraph(27,RAA_D0_B_fx28,RAA_D0_B_fy28);
   graph->SetName("RAA_D0_B");
   graph->SetTitle("Graph");

   ci = TColor::GetColor("#0000cc");
   graph->SetLineColor(ci);
   graph->SetLineStyle(2);
   graph->SetLineWidth(2);
   graph->SetMarkerStyle(20);
   graph->SetMarkerSize(1.2);
   
   TH1F *Graph_RAA_D0_B28 = new TH1F("Graph_RAA_D0_B28","Graph",100,0,32.975);
   Graph_RAA_D0_B28->SetMinimum(0.2614948);
   Graph_RAA_D0_B28->SetMaximum(1.070347);
   Graph_RAA_D0_B28->SetDirectory(0);
   Graph_RAA_D0_B28->SetLineWidth(2);
   Graph_RAA_D0_B28->SetMarkerStyle(20);
   Graph_RAA_D0_B28->SetMarkerSize(1.2);
   Graph_RAA_D0_B28->GetXaxis()->SetLabelFont(42);
   Graph_RAA_D0_B28->GetXaxis()->SetLabelSize(0.05);
   Graph_RAA_D0_B28->GetXaxis()->SetTitleSize(0.05);
   Graph_RAA_D0_B28->GetXaxis()->SetTitleOffset(1.4);
   Graph_RAA_D0_B28->GetXaxis()->SetTitleFont(42);
   Graph_RAA_D0_B28->GetYaxis()->SetLabelFont(42);
   Graph_RAA_D0_B28->GetYaxis()->SetLabelSize(0.05);
   Graph_RAA_D0_B28->GetYaxis()->SetTitleSize(0.05);
   Graph_RAA_D0_B28->GetYaxis()->SetTitleOffset(1.4);
   Graph_RAA_D0_B28->GetYaxis()->SetTitleFont(42);
   Graph_RAA_D0_B28->GetZaxis()->SetLabelFont(42);
   Graph_RAA_D0_B28->GetZaxis()->SetLabelSize(0.05);
   Graph_RAA_D0_B28->GetZaxis()->SetTitleSize(0.05);
   Graph_RAA_D0_B28->GetZaxis()->SetTitleOffset(1);
   Graph_RAA_D0_B28->GetZaxis()->SetTitleFont(42);
   graph->SetHistogram(Graph_RAA_D0_B28);
   
   graph->Draw("");
   
   Double_t Graph0_fx1001[20] = {
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
   Double_t Graph0_fy1001[20] = {
   0.5375912,
   0.7987257,
   1.078158,
   1.162469,
   1.098154,
   0.8777246,
   0.6481578,
   0.5014529,
   0.3921144,
   0.3353954,
   0.3012415,
   0.2763732,
   0.2515049,
   0.2395902,
   0.2276756,
   0.2280616,
   0.2284476,
   0.2299139,
   0.2313802,
   0.234337};
   Double_t Graph0_fex1001[20] = {
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
   Double_t Graph0_fey1001[20] = {
   0.004402939,
   0.004239807,
   0.003898691,
   0.00386979,
   0.003895942,
   0.00363414,
   0.002995177,
   0.002951778,
   0.003009618,
   0.00341761,
   0.003906693,
   0.004637783,
   0.005466189,
   0.006724237,
   0.007231593,
   0.009384692,
   0.0120616,
   0.01549817,
   0.01971241,
   0.02507147};
   TGraphErrors *gre = new TGraphErrors(20,Graph0_fx1001,Graph0_fy1001,Graph0_fex1001,Graph0_fey1001);
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
   
   TH1F *Graph_Graph01001 = new TH1F("Graph_Graph01001","Graph",100,0,10.7);
   Graph_Graph01001->SetMinimum(0.1135582);
   Graph_Graph01001->SetMaximum(1.262046);
   Graph_Graph01001->SetDirectory(0);
   Graph_Graph01001->SetStats(0);
   Graph_Graph01001->SetLineWidth(2);
   Graph_Graph01001->SetMarkerStyle(20);
   Graph_Graph01001->SetMarkerSize(1.2);
   Graph_Graph01001->GetXaxis()->SetLabelFont(42);
   Graph_Graph01001->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph01001->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph01001->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph01001->GetXaxis()->SetTitleFont(42);
   Graph_Graph01001->GetYaxis()->SetLabelFont(42);
   Graph_Graph01001->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph01001->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph01001->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph01001->GetYaxis()->SetTitleFont(42);
   Graph_Graph01001->GetZaxis()->SetLabelFont(42);
   Graph_Graph01001->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph01001->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph01001->GetZaxis()->SetTitleOffset(1);
   Graph_Graph01001->GetZaxis()->SetTitleFont(42);
   gre->SetHistogram(Graph_Graph01001);
   
   gre->Draw("p");
   
   Double_t Graph1_fx1002[8] = {
   2.5,
   3.5,
   4.5,
   5.5,
   6.5,
   7.5,
   8.5,
   9.5};
   Double_t Graph1_fy1002[8] = {
   0.9445014,
   0.8627149,
   0.7439378,
   0.6392494,
   0.5599322,
   0.5022965,
   0.4605267,
   0.428259};
   Double_t Graph1_fex1002[8] = {
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0};
   Double_t Graph1_fey1002[8] = {
   0.07359554,
   0.05559087,
   0.04919221,
   0.05251078,
   0.06078421,
   0.08041788,
   0.09652157,
   0.1194139};
   gre = new TGraphErrors(8,Graph1_fx1002,Graph1_fy1002,Graph1_fex1002,Graph1_fey1002);
   gre->SetName("Graph1");
   gre->SetTitle("Graph");
   gre->SetFillStyle(1000);

   ci = TColor::GetColor("#000099");
   gre->SetLineColor(ci);
   gre->SetLineWidth(4);

   ci = TColor::GetColor("#0000cc");
   gre->SetMarkerColor(ci);
   gre->SetMarkerStyle(21);
   gre->SetMarkerSize(2);
   
   TH1F *Graph_Graph11002 = new TH1F("Graph_Graph11002","Graph",100,1.8,10.2);
   Graph_Graph11002->SetMinimum(0.2379199);
   Graph_Graph11002->SetMaximum(1.089022);
   Graph_Graph11002->SetDirectory(0);
   Graph_Graph11002->SetStats(0);
   Graph_Graph11002->SetLineWidth(2);
   Graph_Graph11002->SetMarkerStyle(20);
   Graph_Graph11002->SetMarkerSize(1.2);
   Graph_Graph11002->GetXaxis()->SetLabelFont(42);
   Graph_Graph11002->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph11002->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph11002->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph11002->GetXaxis()->SetTitleFont(42);
   Graph_Graph11002->GetYaxis()->SetLabelFont(42);
   Graph_Graph11002->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph11002->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph11002->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph11002->GetYaxis()->SetTitleFont(42);
   Graph_Graph11002->GetZaxis()->SetLabelFont(42);
   Graph_Graph11002->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph11002->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph11002->GetZaxis()->SetTitleOffset(1);
   Graph_Graph11002->GetZaxis()->SetTitleFont(42);
   gre->SetHistogram(Graph_Graph11002);
   
   gre->Draw("p");
   
   TLegend *leg = new TLegend(0,0.78,0.85,0.9,NULL,"brNDC");
   leg->SetBorderSize(0);
   leg->SetLineColor(1);
   leg->SetLineStyle(1);
   leg->SetLineWidth(1);
   leg->SetFillColor(0);
   leg->SetFillStyle(0);
   TLegendEntry *entry=leg->AddEntry("NULL","#it{#bf{sPHENIX}} Projection, 0-10% Au+Au, Years 1-3","");
   entry->SetLineColor(1);
   entry->SetLineStyle(1);
   entry->SetLineWidth(1);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   entry->SetTextFont(42);
   entry=leg->AddEntry("NULL","6.2 pb^{-1} str. #it{p}+#it{p}, 21 nb^{-1} rec. Au+Au","");
   entry->SetLineColor(1);
   entry->SetLineStyle(1);
   entry->SetLineWidth(1);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   entry->SetTextFont(42);
   leg->Draw();
   
   leg = new TLegend(0.65,0.52,0.9,0.77,NULL,"brNDC");
   leg->SetBorderSize(0);
   leg->SetLineColor(1);
   leg->SetLineStyle(1);
   leg->SetLineWidth(1);
   leg->SetFillColor(0);
   leg->SetFillStyle(0);
   entry=leg->AddEntry("RAA_B","#it{B}-meson","l");

   ci = TColor::GetColor("#3333ff");
   entry->SetLineColor(ci);
   entry->SetLineStyle(1);
   entry->SetLineWidth(2);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   entry->SetTextFont(42);
   entry=leg->AddEntry("Graph","#it{B}#rightarrow#it{D}^{0}","lp");

   ci = TColor::GetColor("#000099");
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
   entry=leg->AddEntry("RAA_pi","#pi","l");

   ci = TColor::GetColor("#009900");
   entry->SetLineColor(ci);
   entry->SetLineStyle(1);
   entry->SetLineWidth(2);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   entry->SetTextFont(42);
   leg->Draw();
   D0_BUP2020_AuAu_RAA_3yr_1->Modified();
   D0_BUP2020_AuAu_RAA_3yr->cd();
   D0_BUP2020_AuAu_RAA_3yr->Modified();
   D0_BUP2020_AuAu_RAA_3yr->cd();
   D0_BUP2020_AuAu_RAA_3yr->SetSelected(D0_BUP2020_AuAu_RAA_3yr);
}
