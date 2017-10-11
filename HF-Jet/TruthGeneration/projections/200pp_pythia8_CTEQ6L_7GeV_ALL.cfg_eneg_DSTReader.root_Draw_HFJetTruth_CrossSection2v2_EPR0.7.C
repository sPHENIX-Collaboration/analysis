{
//=========Macro generated from canvas: Draw_HFJetTruth_CrossSection2v2_EPR0.7/Draw_HFJetTruth_CrossSection2v2_EPR0.7
//=========  (Thu Aug 17 22:26:37 2017) by ROOT version5.34/36
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
   gre->SetName("ge_h_b_copyv2424238335");
   gre->SetTitle("Graph");
   gre->SetFillColor(1);

   ci = TColor::GetColor("#003366");
   gre->SetLineColor(ci);
   gre->SetLineWidth(3);

   ci = TColor::GetColor("#003366");
   gre->SetMarkerColor(ci);
   gre->SetMarkerStyle(21);
   gre->SetMarkerSize(2);
   gre->SetPoint(0,17.5,0);
   gre->SetPointError(0,0,0.0054304);
   gre->SetPoint(1,22.5,0);
   gre->SetPointError(1,0,0.01305732);
   gre->SetPoint(2,27.5,0);
   gre->SetPointError(2,0,0.02983324);
   gre->SetPoint(3,32.5,0);
   gre->SetPointError(3,0,0.06566371);
   
   TH1F *Graph_ge_h_b_copyv24242383354 = new TH1F("Graph_ge_h_b_copyv24242383354","Graph",100,16,34);
   Graph_ge_h_b_copyv24242383354->SetMinimum(-0.07879645);
   Graph_ge_h_b_copyv24242383354->SetMaximum(0.07879645);
   Graph_ge_h_b_copyv24242383354->SetDirectory(0);
   Graph_ge_h_b_copyv24242383354->SetStats(0);
   Graph_ge_h_b_copyv24242383354->SetLineWidth(2);
   Graph_ge_h_b_copyv24242383354->SetMarkerStyle(20);
   Graph_ge_h_b_copyv24242383354->SetMarkerSize(1.2);
   Graph_ge_h_b_copyv24242383354->GetXaxis()->SetLabelFont(42);
   Graph_ge_h_b_copyv24242383354->GetXaxis()->SetLabelSize(0.05);
   Graph_ge_h_b_copyv24242383354->GetXaxis()->SetTitleSize(0.05);
   Graph_ge_h_b_copyv24242383354->GetXaxis()->SetTitleOffset(1.4);
   Graph_ge_h_b_copyv24242383354->GetXaxis()->SetTitleFont(42);
   Graph_ge_h_b_copyv24242383354->GetYaxis()->SetLabelFont(42);
   Graph_ge_h_b_copyv24242383354->GetYaxis()->SetLabelSize(0.05);
   Graph_ge_h_b_copyv24242383354->GetYaxis()->SetTitleSize(0.05);
   Graph_ge_h_b_copyv24242383354->GetYaxis()->SetTitleOffset(1.4);
   Graph_ge_h_b_copyv24242383354->GetYaxis()->SetTitleFont(42);
   Graph_ge_h_b_copyv24242383354->GetZaxis()->SetLabelFont(42);
   Graph_ge_h_b_copyv24242383354->GetZaxis()->SetLabelSize(0.05);
   Graph_ge_h_b_copyv24242383354->GetZaxis()->SetTitleSize(0.05);
   Graph_ge_h_b_copyv24242383354->GetZaxis()->SetTitleFont(42);
   gre->SetHistogram(Graph_ge_h_b_copyv24242383354);
   
   gre->Draw("pe");
   
   gre = new TGraphErrors(4);
   gre->SetName("ge_h_b_copyv2719885386");
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
   gre->SetPointError(0,0,0.005480619);
   gre->SetPoint(1,23.2,0);
   gre->SetPointError(1,0,0.01317807);
   gre->SetPoint(2,28.2,0);
   gre->SetPointError(2,0,0.03010913);
   gre->SetPoint(3,33.2,0);
   gre->SetPointError(3,0,0.06627095);
   
   TH1F *Graph_ge_h_b_copyv27198853865 = new TH1F("Graph_ge_h_b_copyv27198853865","Graph",100,16.7,34.7);
   Graph_ge_h_b_copyv27198853865->SetMinimum(-0.07952514);
   Graph_ge_h_b_copyv27198853865->SetMaximum(0.07952514);
   Graph_ge_h_b_copyv27198853865->SetDirectory(0);
   Graph_ge_h_b_copyv27198853865->SetStats(0);
   Graph_ge_h_b_copyv27198853865->SetLineWidth(2);
   Graph_ge_h_b_copyv27198853865->SetMarkerStyle(20);
   Graph_ge_h_b_copyv27198853865->SetMarkerSize(1.2);
   Graph_ge_h_b_copyv27198853865->GetXaxis()->SetLabelFont(42);
   Graph_ge_h_b_copyv27198853865->GetXaxis()->SetLabelSize(0.05);
   Graph_ge_h_b_copyv27198853865->GetXaxis()->SetTitleSize(0.05);
   Graph_ge_h_b_copyv27198853865->GetXaxis()->SetTitleOffset(1.4);
   Graph_ge_h_b_copyv27198853865->GetXaxis()->SetTitleFont(42);
   Graph_ge_h_b_copyv27198853865->GetYaxis()->SetLabelFont(42);
   Graph_ge_h_b_copyv27198853865->GetYaxis()->SetLabelSize(0.05);
   Graph_ge_h_b_copyv27198853865->GetYaxis()->SetTitleSize(0.05);
   Graph_ge_h_b_copyv27198853865->GetYaxis()->SetTitleOffset(1.4);
   Graph_ge_h_b_copyv27198853865->GetYaxis()->SetTitleFont(42);
   Graph_ge_h_b_copyv27198853865->GetZaxis()->SetLabelFont(42);
   Graph_ge_h_b_copyv27198853865->GetZaxis()->SetLabelSize(0.05);
   Graph_ge_h_b_copyv27198853865->GetZaxis()->SetTitleSize(0.05);
   Graph_ge_h_b_copyv27198853865->GetZaxis()->SetTitleFont(42);
   gre->SetHistogram(Graph_ge_h_b_copyv27198853865);
   
   gre->Draw("pe");
   
   gre = new TGraphErrors(3);
   gre->SetName("ge_h_b_copyv21649760492");
   gre->SetTitle("Graph");
   gre->SetFillColor(1);

   ci = TColor::GetColor("#669933");
   gre->SetLineColor(ci);
   gre->SetLineWidth(3);

   ci = TColor::GetColor("#669933");
   gre->SetMarkerColor(ci);
   gre->SetMarkerStyle(34);
   gre->SetMarkerSize(2);
   gre->SetPoint(0,18.9,0);
   gre->SetPointError(0,0,0.009725491);
   gre->SetPoint(1,23.9,0);
   gre->SetPointError(1,0,0.02338481);
   gre->SetPoint(2,28.9,0);
   gre->SetPointError(2,0,0.05342938);
   
   TH1F *Graph_ge_h_b_copyv216497604926 = new TH1F("Graph_ge_h_b_copyv216497604926","Graph",100,17.9,29.9);
   Graph_ge_h_b_copyv216497604926->SetMinimum(-0.06411525);
   Graph_ge_h_b_copyv216497604926->SetMaximum(0.06411525);
   Graph_ge_h_b_copyv216497604926->SetDirectory(0);
   Graph_ge_h_b_copyv216497604926->SetStats(0);
   Graph_ge_h_b_copyv216497604926->SetLineWidth(2);
   Graph_ge_h_b_copyv216497604926->SetMarkerStyle(20);
   Graph_ge_h_b_copyv216497604926->SetMarkerSize(1.2);
   Graph_ge_h_b_copyv216497604926->GetXaxis()->SetLabelFont(42);
   Graph_ge_h_b_copyv216497604926->GetXaxis()->SetLabelSize(0.05);
   Graph_ge_h_b_copyv216497604926->GetXaxis()->SetTitleSize(0.05);
   Graph_ge_h_b_copyv216497604926->GetXaxis()->SetTitleOffset(1.4);
   Graph_ge_h_b_copyv216497604926->GetXaxis()->SetTitleFont(42);
   Graph_ge_h_b_copyv216497604926->GetYaxis()->SetLabelFont(42);
   Graph_ge_h_b_copyv216497604926->GetYaxis()->SetLabelSize(0.05);
   Graph_ge_h_b_copyv216497604926->GetYaxis()->SetTitleSize(0.05);
   Graph_ge_h_b_copyv216497604926->GetYaxis()->SetTitleOffset(1.4);
   Graph_ge_h_b_copyv216497604926->GetYaxis()->SetTitleFont(42);
   Graph_ge_h_b_copyv216497604926->GetZaxis()->SetLabelFont(42);
   Graph_ge_h_b_copyv216497604926->GetZaxis()->SetLabelSize(0.05);
   Graph_ge_h_b_copyv216497604926->GetZaxis()->SetTitleSize(0.05);
   Graph_ge_h_b_copyv216497604926->GetZaxis()->SetTitleFont(42);
   gre->SetHistogram(Graph_ge_h_b_copyv216497604926);
   
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
   entry=leg->AddEntry("ge_h_b_copyv2424238335","Au+Au 10-20%C","pl");

   ci = TColor::GetColor("#003366");
   entry->SetLineColor(ci);
   entry->SetLineStyle(1);
   entry->SetLineWidth(3);

   ci = TColor::GetColor("#003366");
   entry->SetMarkerColor(ci);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(2);
   entry->SetTextFont(42);
   entry=leg->AddEntry("ge_h_b_copyv2719885386","Au+Au 20-40%C","pl");

   ci = TColor::GetColor("#006633");
   entry->SetLineColor(ci);
   entry->SetLineStyle(1);
   entry->SetLineWidth(3);

   ci = TColor::GetColor("#006633");
   entry->SetMarkerColor(ci);
   entry->SetMarkerStyle(33);
   entry->SetMarkerSize(2);
   entry->SetTextFont(42);
   entry=leg->AddEntry("ge_h_b_copyv21649760492","Au+Au 40-60%C","pl");

   ci = TColor::GetColor("#669933");
   entry->SetLineColor(ci);
   entry->SetLineStyle(1);
   entry->SetLineWidth(3);

   ci = TColor::GetColor("#669933");
   entry->SetMarkerColor(ci);
   entry->SetMarkerStyle(34);
   entry->SetMarkerSize(2);
   entry->SetTextFont(42);
   leg->Draw();
   Draw_HFJetTruth_CrossSection2v2_EPR0.7_1->Modified();
   Draw_HFJetTruth_CrossSection2v2_EPR0.7->cd();
   Draw_HFJetTruth_CrossSection2v2_EPR0.7->Modified();
   Draw_HFJetTruth_CrossSection2v2_EPR0.7->cd();
   Draw_HFJetTruth_CrossSection2v2_EPR0.7->SetSelected(Draw_HFJetTruth_CrossSection2v2_EPR0.7);
}
