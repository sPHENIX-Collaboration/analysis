{
//=========Macro generated from canvas: Draw_HFJetTruth_CrossSection2v2_EPR0.4_deta0.45/Draw_HFJetTruth_CrossSection2v2_EPR0.4_deta0.45
//=========  (Mon Dec  4 09:14:07 2017) by ROOT version5.34/36
   TCanvas *Draw_HFJetTruth_CrossSection2v2_EPR0.4_deta0.45 = new TCanvas("Draw_HFJetTruth_CrossSection2v2_EPR0.4_deta0.45", "Draw_HFJetTruth_CrossSection2v2_EPR0.4_deta0.45",24,157,700,600);
   gStyle->SetOptFit(1);
   gStyle->SetOptStat(0);
   gStyle->SetOptTitle(0);
   Draw_HFJetTruth_CrossSection2v2_EPR0.4_deta0.45->Range(0,0,1,1);
   Draw_HFJetTruth_CrossSection2v2_EPR0.4_deta0.45->SetFillColor(0);
   Draw_HFJetTruth_CrossSection2v2_EPR0.4_deta0.45->SetBorderMode(0);
   Draw_HFJetTruth_CrossSection2v2_EPR0.4_deta0.45->SetBorderSize(2);
   Draw_HFJetTruth_CrossSection2v2_EPR0.4_deta0.45->SetTickx(1);
   Draw_HFJetTruth_CrossSection2v2_EPR0.4_deta0.45->SetTicky(1);
   Draw_HFJetTruth_CrossSection2v2_EPR0.4_deta0.45->SetLeftMargin(0.16);
   Draw_HFJetTruth_CrossSection2v2_EPR0.4_deta0.45->SetRightMargin(0.05);
   Draw_HFJetTruth_CrossSection2v2_EPR0.4_deta0.45->SetTopMargin(0.05);
   Draw_HFJetTruth_CrossSection2v2_EPR0.4_deta0.45->SetBottomMargin(0.16);
   Draw_HFJetTruth_CrossSection2v2_EPR0.4_deta0.45->SetFrameBorderMode(0);
  
// ------------>Primitives in pad: Draw_HFJetTruth_CrossSection2v2_EPR0.4_deta0.45_1
   TPad *Draw_HFJetTruth_CrossSection2v2_EPR0.4_deta0.45_1 = new TPad("Draw_HFJetTruth_CrossSection2v2_EPR0.4_deta0.45_1", "Draw_HFJetTruth_CrossSection2v2_EPR0.4_deta0.45_1",0.01,0.01,0.99,0.99);
   Draw_HFJetTruth_CrossSection2v2_EPR0.4_deta0.45_1->Draw();
   Draw_HFJetTruth_CrossSection2v2_EPR0.4_deta0.45_1->cd();
   Draw_HFJetTruth_CrossSection2v2_EPR0.4_deta0.45_1->Range(9.936709,-0.1810127,41.58228,0.3253165);
   Draw_HFJetTruth_CrossSection2v2_EPR0.4_deta0.45_1->SetFillColor(0);
   Draw_HFJetTruth_CrossSection2v2_EPR0.4_deta0.45_1->SetBorderMode(0);
   Draw_HFJetTruth_CrossSection2v2_EPR0.4_deta0.45_1->SetBorderSize(2);
   Draw_HFJetTruth_CrossSection2v2_EPR0.4_deta0.45_1->SetTickx(1);
   Draw_HFJetTruth_CrossSection2v2_EPR0.4_deta0.45_1->SetTicky(1);
   Draw_HFJetTruth_CrossSection2v2_EPR0.4_deta0.45_1->SetLeftMargin(0.16);
   Draw_HFJetTruth_CrossSection2v2_EPR0.4_deta0.45_1->SetRightMargin(0.05);
   Draw_HFJetTruth_CrossSection2v2_EPR0.4_deta0.45_1->SetTopMargin(0.05);
   Draw_HFJetTruth_CrossSection2v2_EPR0.4_deta0.45_1->SetBottomMargin(0.16);
   Draw_HFJetTruth_CrossSection2v2_EPR0.4_deta0.45_1->SetFrameBorderMode(0);
   Draw_HFJetTruth_CrossSection2v2_EPR0.4_deta0.45_1->SetFrameBorderMode(0);
   
   TH1F *hframe__4 = new TH1F("hframe__4","",1000,15,40);
   hframe__4->SetMinimum(-0.1);
   hframe__4->SetMaximum(0.3);
   hframe__4->SetDirectory(0);
   hframe__4->SetStats(0);
   hframe__4->SetLineWidth(2);
   hframe__4->SetMarkerStyle(20);
   hframe__4->SetMarkerSize(1.2);
   hframe__4->GetXaxis()->SetTitle("Transverse Momentum [GeV/#it{c}]");
   hframe__4->GetXaxis()->SetLabelFont(42);
   hframe__4->GetXaxis()->SetLabelSize(0.05);
   hframe__4->GetXaxis()->SetTitleSize(0.05);
   hframe__4->GetXaxis()->SetTitleOffset(1.4);
   hframe__4->GetXaxis()->SetTitleFont(42);
   hframe__4->GetYaxis()->SetTitle("v_{2}");
   hframe__4->GetYaxis()->SetLabelFont(42);
   hframe__4->GetYaxis()->SetLabelSize(0.05);
   hframe__4->GetYaxis()->SetTitleSize(0.05);
   hframe__4->GetYaxis()->SetTitleOffset(1.4);
   hframe__4->GetYaxis()->SetTitleFont(42);
   hframe__4->GetZaxis()->SetLabelFont(42);
   hframe__4->GetZaxis()->SetLabelSize(0.05);
   hframe__4->GetZaxis()->SetTitleSize(0.05);
   hframe__4->GetZaxis()->SetTitleFont(42);
   hframe__4->Draw(" ");
   
   TGraphErrors *gre = new TGraphErrors(4);
   gre->SetName("ge_h_b_copyv21025202362");
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
   gre->SetPointError(0,0,0.0093927);
   gre->SetPoint(1,21.8,0);
   gre->SetPointError(1,0,0.02258462);
   gre->SetPoint(2,26.8,0);
   gre->SetPointError(2,0,0.05160111);
   gre->SetPoint(3,31.8,0);
   gre->SetPointError(3,0,0.1135753);
   
   TH1F *Graph_ge_h_b_copyv210252023625 = new TH1F("Graph_ge_h_b_copyv210252023625","Graph",100,15.3,33.3);
   Graph_ge_h_b_copyv210252023625->SetMinimum(-0.1362904);
   Graph_ge_h_b_copyv210252023625->SetMaximum(0.1362904);
   Graph_ge_h_b_copyv210252023625->SetDirectory(0);
   Graph_ge_h_b_copyv210252023625->SetStats(0);
   Graph_ge_h_b_copyv210252023625->SetLineWidth(2);
   Graph_ge_h_b_copyv210252023625->SetMarkerStyle(20);
   Graph_ge_h_b_copyv210252023625->SetMarkerSize(1.2);
   Graph_ge_h_b_copyv210252023625->GetXaxis()->SetLabelFont(42);
   Graph_ge_h_b_copyv210252023625->GetXaxis()->SetLabelSize(0.05);
   Graph_ge_h_b_copyv210252023625->GetXaxis()->SetTitleSize(0.05);
   Graph_ge_h_b_copyv210252023625->GetXaxis()->SetTitleOffset(1.4);
   Graph_ge_h_b_copyv210252023625->GetXaxis()->SetTitleFont(42);
   Graph_ge_h_b_copyv210252023625->GetYaxis()->SetLabelFont(42);
   Graph_ge_h_b_copyv210252023625->GetYaxis()->SetLabelSize(0.05);
   Graph_ge_h_b_copyv210252023625->GetYaxis()->SetTitleSize(0.05);
   Graph_ge_h_b_copyv210252023625->GetYaxis()->SetTitleOffset(1.4);
   Graph_ge_h_b_copyv210252023625->GetYaxis()->SetTitleFont(42);
   Graph_ge_h_b_copyv210252023625->GetZaxis()->SetLabelFont(42);
   Graph_ge_h_b_copyv210252023625->GetZaxis()->SetLabelSize(0.05);
   Graph_ge_h_b_copyv210252023625->GetZaxis()->SetTitleSize(0.05);
   Graph_ge_h_b_copyv210252023625->GetZaxis()->SetTitleFont(42);
   gre->SetHistogram(Graph_ge_h_b_copyv210252023625);
   
   gre->Draw("pe");
   
   gre = new TGraphErrors(4);
   gre->SetName("ge_h_b_copyv22044897763");
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
   gre->SetPointError(0,0,0.00841952);
   gre->SetPoint(1,23.2,0);
   gre->SetPointError(1,0,0.02024462);
   gre->SetPoint(2,28.2,0);
   gre->SetPointError(2,0,0.0462547);
   gre->SetPoint(3,33.2,0);
   gre->SetPointError(3,0,0.1018078);
   
   TH1F *Graph_ge_h_b_copyv220448977636 = new TH1F("Graph_ge_h_b_copyv220448977636","Graph",100,16.7,34.7);
   Graph_ge_h_b_copyv220448977636->SetMinimum(-0.1221693);
   Graph_ge_h_b_copyv220448977636->SetMaximum(0.1221693);
   Graph_ge_h_b_copyv220448977636->SetDirectory(0);
   Graph_ge_h_b_copyv220448977636->SetStats(0);
   Graph_ge_h_b_copyv220448977636->SetLineWidth(2);
   Graph_ge_h_b_copyv220448977636->SetMarkerStyle(20);
   Graph_ge_h_b_copyv220448977636->SetMarkerSize(1.2);
   Graph_ge_h_b_copyv220448977636->GetXaxis()->SetLabelFont(42);
   Graph_ge_h_b_copyv220448977636->GetXaxis()->SetLabelSize(0.05);
   Graph_ge_h_b_copyv220448977636->GetXaxis()->SetTitleSize(0.05);
   Graph_ge_h_b_copyv220448977636->GetXaxis()->SetTitleOffset(1.4);
   Graph_ge_h_b_copyv220448977636->GetXaxis()->SetTitleFont(42);
   Graph_ge_h_b_copyv220448977636->GetYaxis()->SetLabelFont(42);
   Graph_ge_h_b_copyv220448977636->GetYaxis()->SetLabelSize(0.05);
   Graph_ge_h_b_copyv220448977636->GetYaxis()->SetTitleSize(0.05);
   Graph_ge_h_b_copyv220448977636->GetYaxis()->SetTitleOffset(1.4);
   Graph_ge_h_b_copyv220448977636->GetYaxis()->SetTitleFont(42);
   Graph_ge_h_b_copyv220448977636->GetZaxis()->SetLabelFont(42);
   Graph_ge_h_b_copyv220448977636->GetZaxis()->SetLabelSize(0.05);
   Graph_ge_h_b_copyv220448977636->GetZaxis()->SetTitleSize(0.05);
   Graph_ge_h_b_copyv220448977636->GetZaxis()->SetTitleFont(42);
   gre->SetHistogram(Graph_ge_h_b_copyv220448977636);
   
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
   entry=leg->AddEntry("NULL","PYTHIA-8 #it{b}-jet, Anti-k_{T} R=0.4, |#eta|<0.45, CTEQ6L","");
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
   entry=leg->AddEntry("NULL","#it{b}-jet v_{2} Projection, #it{R}_{AA, #it{b}-jet}=0.6, Res(#Psi_{2})=0.4","h");
   entry->SetLineColor(1);
   entry->SetLineStyle(1);
   entry->SetLineWidth(1);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   entry->SetTextFont(42);
   entry=leg->AddEntry("ge_h_b_copyv21025202362","Au+Au 0-10%C","pl");

   ci = TColor::GetColor("#000066");
   entry->SetLineColor(ci);
   entry->SetLineStyle(1);
   entry->SetLineWidth(3);

   ci = TColor::GetColor("#000066");
   entry->SetMarkerColor(ci);
   entry->SetMarkerStyle(20);
   entry->SetMarkerSize(2);
   entry->SetTextFont(42);
   entry=leg->AddEntry("ge_h_b_copyv22044897763","Au+Au 10-40%C","pl");

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
   Draw_HFJetTruth_CrossSection2v2_EPR0.4_deta0.45_1->Modified();
   Draw_HFJetTruth_CrossSection2v2_EPR0.4_deta0.45->cd();
   Draw_HFJetTruth_CrossSection2v2_EPR0.4_deta0.45->Modified();
   Draw_HFJetTruth_CrossSection2v2_EPR0.4_deta0.45->cd();
   Draw_HFJetTruth_CrossSection2v2_EPR0.4_deta0.45->SetSelected(Draw_HFJetTruth_CrossSection2v2_EPR0.4_deta0.45);
}
