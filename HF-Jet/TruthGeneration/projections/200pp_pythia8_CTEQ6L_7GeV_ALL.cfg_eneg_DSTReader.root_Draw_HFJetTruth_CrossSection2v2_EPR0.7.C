{
//=========Macro generated from canvas: Draw_HFJetTruth_CrossSection2v2_EPR0.7/Draw_HFJetTruth_CrossSection2v2_EPR0.7
//=========  (Tue Oct 17 20:36:19 2017) by ROOT version5.34/36
   TCanvas *Draw_HFJetTruth_CrossSection2v2_EPR0.7 = new TCanvas("Draw_HFJetTruth_CrossSection2v2_EPR0.7", "Draw_HFJetTruth_CrossSection2v2_EPR0.7",23,135,700,600);
   gStyle->SetOptFit(1);
   gStyle->SetOptStat(0);
   gStyle->SetOptTitle(0);
   Draw_HFJetTruth_CrossSection2v2_EPR0.7->Range(0,0,1,1);
   Draw_HFJetTruth_CrossSection2v2_EPR0.7->SetFillColor(0);
   Draw_HFJetTruth_CrossSection2v2_EPR0.7->SetBorderMode(0);
   Draw_HFJetTruth_CrossSection2v2_EPR0.7->SetBorderSize(2);
   Draw_HFJetTruth_CrossSection2v2_EPR0.7->SetTickx(1);
   Draw_HFJetTruth_CrossSection2v2_EPR0.7->SetTicky(1);
   Draw_HFJetTruth_CrossSection2v2_EPR0.7->SetLeftMargin(0.16);
   Draw_HFJetTruth_CrossSection2v2_EPR0.7->SetRightMargin(0.05);
   Draw_HFJetTruth_CrossSection2v2_EPR0.7->SetTopMargin(0.05);
   Draw_HFJetTruth_CrossSection2v2_EPR0.7->SetBottomMargin(0.16);
   Draw_HFJetTruth_CrossSection2v2_EPR0.7->SetFrameBorderMode(0);
  
// ------------>Primitives in pad: Draw_HFJetTruth_CrossSection2v2_EPR0.7_1
   TPad *Draw_HFJetTruth_CrossSection2v2_EPR0.7_1 = new TPad("Draw_HFJetTruth_CrossSection2v2_EPR0.7_1", "Draw_HFJetTruth_CrossSection2v2_EPR0.7_1",0.01,0.01,0.99,0.99);
   Draw_HFJetTruth_CrossSection2v2_EPR0.7_1->Draw();
   Draw_HFJetTruth_CrossSection2v2_EPR0.7_1->cd();
   Draw_HFJetTruth_CrossSection2v2_EPR0.7_1->Range(9.936709,-0.1810127,41.58228,0.3253165);
   Draw_HFJetTruth_CrossSection2v2_EPR0.7_1->SetFillColor(0);
   Draw_HFJetTruth_CrossSection2v2_EPR0.7_1->SetBorderMode(0);
   Draw_HFJetTruth_CrossSection2v2_EPR0.7_1->SetBorderSize(2);
   Draw_HFJetTruth_CrossSection2v2_EPR0.7_1->SetTickx(1);
   Draw_HFJetTruth_CrossSection2v2_EPR0.7_1->SetTicky(1);
   Draw_HFJetTruth_CrossSection2v2_EPR0.7_1->SetLeftMargin(0.16);
   Draw_HFJetTruth_CrossSection2v2_EPR0.7_1->SetRightMargin(0.05);
   Draw_HFJetTruth_CrossSection2v2_EPR0.7_1->SetTopMargin(0.05);
   Draw_HFJetTruth_CrossSection2v2_EPR0.7_1->SetBottomMargin(0.16);
   Draw_HFJetTruth_CrossSection2v2_EPR0.7_1->SetFrameBorderMode(0);
   Draw_HFJetTruth_CrossSection2v2_EPR0.7_1->SetFrameBorderMode(0);
   
   TH1F *hframe__3 = new TH1F("hframe__3","",1000,15,40);
   hframe__3->SetMinimum(-0.1);
   hframe__3->SetMaximum(0.3);
   hframe__3->SetDirectory(0);
   hframe__3->SetStats(0);
   hframe__3->SetLineWidth(2);
   hframe__3->SetMarkerStyle(20);
   hframe__3->SetMarkerSize(1.2);
   hframe__3->GetXaxis()->SetTitle("Transverse Momentum [GeV/#it{c}]");
   hframe__3->GetXaxis()->SetLabelFont(42);
   hframe__3->GetXaxis()->SetLabelSize(0.05);
   hframe__3->GetXaxis()->SetTitleSize(0.05);
   hframe__3->GetXaxis()->SetTitleOffset(1.4);
   hframe__3->GetXaxis()->SetTitleFont(42);
   hframe__3->GetYaxis()->SetTitle("v_{2}");
   hframe__3->GetYaxis()->SetLabelFont(42);
   hframe__3->GetYaxis()->SetLabelSize(0.05);
   hframe__3->GetYaxis()->SetTitleSize(0.05);
   hframe__3->GetYaxis()->SetTitleOffset(1.4);
   hframe__3->GetYaxis()->SetTitleFont(42);
   hframe__3->GetZaxis()->SetLabelFont(42);
   hframe__3->GetZaxis()->SetLabelSize(0.05);
   hframe__3->GetZaxis()->SetTitleSize(0.05);
   hframe__3->GetZaxis()->SetTitleFont(42);
   hframe__3->Draw(" ");
   
   TGraphErrors *gre = new TGraphErrors(4);
   gre->SetName("ge_h_b_copyv21714636915");
   gre->SetTitle("Graph");
   gre->SetFillColor(1);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#000066");
   gre->SetLineColor(ci);
   gre->SetLineWidth(3);

   ci = TColor::GetColor("#000066");
   gre->SetMarkerColor(ci);
   gre->SetMarkerStyle(20);
   gre->SetMarkerSize(2);
   gre->SetPoint(0,16.8,0);
   gre->SetPointError(0,0,0.004303379);
   gre->SetPoint(1,21.8,0);
   gre->SetPointError(1,0,0.01034742);
   gre->SetPoint(2,26.8,0);
   gre->SetPointError(2,0,0.02364167);
   gre->SetPoint(3,31.8,0);
   gre->SetPointError(3,0,0.05203592);
   
   TH1F *Graph_ge_h_b_copyv217146369153 = new TH1F("Graph_ge_h_b_copyv217146369153","Graph",100,15.3,33.3);
   Graph_ge_h_b_copyv217146369153->SetMinimum(-0.0624431);
   Graph_ge_h_b_copyv217146369153->SetMaximum(0.0624431);
   Graph_ge_h_b_copyv217146369153->SetDirectory(0);
   Graph_ge_h_b_copyv217146369153->SetStats(0);
   Graph_ge_h_b_copyv217146369153->SetLineWidth(2);
   Graph_ge_h_b_copyv217146369153->SetMarkerStyle(20);
   Graph_ge_h_b_copyv217146369153->SetMarkerSize(1.2);
   Graph_ge_h_b_copyv217146369153->GetXaxis()->SetLabelFont(42);
   Graph_ge_h_b_copyv217146369153->GetXaxis()->SetLabelSize(0.05);
   Graph_ge_h_b_copyv217146369153->GetXaxis()->SetTitleSize(0.05);
   Graph_ge_h_b_copyv217146369153->GetXaxis()->SetTitleOffset(1.4);
   Graph_ge_h_b_copyv217146369153->GetXaxis()->SetTitleFont(42);
   Graph_ge_h_b_copyv217146369153->GetYaxis()->SetLabelFont(42);
   Graph_ge_h_b_copyv217146369153->GetYaxis()->SetLabelSize(0.05);
   Graph_ge_h_b_copyv217146369153->GetYaxis()->SetTitleSize(0.05);
   Graph_ge_h_b_copyv217146369153->GetYaxis()->SetTitleOffset(1.4);
   Graph_ge_h_b_copyv217146369153->GetYaxis()->SetTitleFont(42);
   Graph_ge_h_b_copyv217146369153->GetZaxis()->SetLabelFont(42);
   Graph_ge_h_b_copyv217146369153->GetZaxis()->SetLabelSize(0.05);
   Graph_ge_h_b_copyv217146369153->GetZaxis()->SetTitleSize(0.05);
   Graph_ge_h_b_copyv217146369153->GetZaxis()->SetTitleFont(42);
   gre->SetHistogram(Graph_ge_h_b_copyv217146369153);
   
   gre->Draw("pe");
   
   gre = new TGraphErrors(4);
   gre->SetName("ge_h_b_copyv21649760492");
   gre->SetTitle("Graph");
   gre->SetFillColor(1);

   ci = TColor::GetColor("#006633");
   gre->SetLineColor(ci);
   gre->SetLineWidth(3);

   ci = TColor::GetColor("#006633");
   gre->SetMarkerColor(ci);
   gre->SetMarkerStyle(33);
   gre->SetMarkerSize(2);
   gre->SetPoint(0,18.2,0);
   gre->SetPointError(0,0,0.003857505);
   gre->SetPoint(1,23.2,0);
   gre->SetPointError(1,0,0.009275319);
   gre->SetPoint(2,28.2,0);
   gre->SetPointError(2,0,0.02119215);
   gre->SetPoint(3,33.2,0);
   gre->SetPointError(3,0,0.04664447);
   
   TH1F *Graph_ge_h_b_copyv216497604924 = new TH1F("Graph_ge_h_b_copyv216497604924","Graph",100,16.7,34.7);
   Graph_ge_h_b_copyv216497604924->SetMinimum(-0.05597336);
   Graph_ge_h_b_copyv216497604924->SetMaximum(0.05597336);
   Graph_ge_h_b_copyv216497604924->SetDirectory(0);
   Graph_ge_h_b_copyv216497604924->SetStats(0);
   Graph_ge_h_b_copyv216497604924->SetLineWidth(2);
   Graph_ge_h_b_copyv216497604924->SetMarkerStyle(20);
   Graph_ge_h_b_copyv216497604924->SetMarkerSize(1.2);
   Graph_ge_h_b_copyv216497604924->GetXaxis()->SetLabelFont(42);
   Graph_ge_h_b_copyv216497604924->GetXaxis()->SetLabelSize(0.05);
   Graph_ge_h_b_copyv216497604924->GetXaxis()->SetTitleSize(0.05);
   Graph_ge_h_b_copyv216497604924->GetXaxis()->SetTitleOffset(1.4);
   Graph_ge_h_b_copyv216497604924->GetXaxis()->SetTitleFont(42);
   Graph_ge_h_b_copyv216497604924->GetYaxis()->SetLabelFont(42);
   Graph_ge_h_b_copyv216497604924->GetYaxis()->SetLabelSize(0.05);
   Graph_ge_h_b_copyv216497604924->GetYaxis()->SetTitleSize(0.05);
   Graph_ge_h_b_copyv216497604924->GetYaxis()->SetTitleOffset(1.4);
   Graph_ge_h_b_copyv216497604924->GetYaxis()->SetTitleFont(42);
   Graph_ge_h_b_copyv216497604924->GetZaxis()->SetLabelFont(42);
   Graph_ge_h_b_copyv216497604924->GetZaxis()->SetLabelSize(0.05);
   Graph_ge_h_b_copyv216497604924->GetZaxis()->SetTitleSize(0.05);
   Graph_ge_h_b_copyv216497604924->GetZaxis()->SetTitleFont(42);
   gre->SetHistogram(Graph_ge_h_b_copyv216497604924);
   
   gre->Draw("pe");
   
   TLegend *leg = new TLegend(0,0.78,0.85,0.93,NULL,"brNDC");
   leg->SetBorderSize(0);
   leg->SetLineColor(1);
   leg->SetLineStyle(1);
   leg->SetLineWidth(1);
   leg->SetFillColor(0);
   leg->SetFillStyle(0);
   TLegendEntry *entry=leg->AddEntry("NULL","#it{#bf{sPHENIX }} Simulation","");
   entry->SetLineColor(1);
   entry->SetLineStyle(1);
   entry->SetLineWidth(1);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   entry->SetTextFont(42);
   entry=leg->AddEntry("NULL","PYTHIA-8 #it{b}-jet, Anti-k_{T} R=0.4, |#eta|<0.7, CTEQ6L","");
   entry->SetLineColor(1);
   entry->SetLineStyle(1);
   entry->SetLineWidth(1);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   entry->SetTextFont(42);
   entry=leg->AddEntry("NULL","Au+Au: 240B col., 40% Eff., 40% Pur.","");
   entry->SetLineColor(1);
   entry->SetLineStyle(1);
   entry->SetLineWidth(1);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   entry->SetTextFont(42);
   leg->Draw();
   
   leg = new TLegend(0.19,0.55,1,0.78,NULL,"brNDC");
   leg->SetBorderSize(0);
   leg->SetLineColor(1);
   leg->SetLineStyle(1);
   leg->SetLineWidth(1);
   leg->SetFillColor(0);
   leg->SetFillStyle(0);
   entry=leg->AddEntry("NULL","#it{b}-jet v_{2} Projection, #it{R}_{AA, #it{b}-jet}=0.6, Res(#Psi_{2})=0.7","h");
   entry->SetLineColor(1);
   entry->SetLineStyle(1);
   entry->SetLineWidth(1);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   entry->SetTextFont(42);
   entry=leg->AddEntry("ge_h_b_copyv21714636915","Au+Au 0-10%C","pl");

   ci = TColor::GetColor("#000066");
   entry->SetLineColor(ci);
   entry->SetLineStyle(1);
   entry->SetLineWidth(3);

   ci = TColor::GetColor("#000066");
   entry->SetMarkerColor(ci);
   entry->SetMarkerStyle(20);
   entry->SetMarkerSize(2);
   entry->SetTextFont(42);
   entry=leg->AddEntry("ge_h_b_copyv21649760492","Au+Au 10-40%C","pl");

   ci = TColor::GetColor("#006633");
   entry->SetLineColor(ci);
   entry->SetLineStyle(1);
   entry->SetLineWidth(3);

   ci = TColor::GetColor("#006633");
   entry->SetMarkerColor(ci);
   entry->SetMarkerStyle(33);
   entry->SetMarkerSize(2);
   entry->SetTextFont(42);
   leg->Draw();
   Draw_HFJetTruth_CrossSection2v2_EPR0.7_1->Modified();
   Draw_HFJetTruth_CrossSection2v2_EPR0.7->cd();
   Draw_HFJetTruth_CrossSection2v2_EPR0.7->Modified();
   Draw_HFJetTruth_CrossSection2v2_EPR0.7->cd();
   Draw_HFJetTruth_CrossSection2v2_EPR0.7->SetSelected(Draw_HFJetTruth_CrossSection2v2_EPR0.7);
}
