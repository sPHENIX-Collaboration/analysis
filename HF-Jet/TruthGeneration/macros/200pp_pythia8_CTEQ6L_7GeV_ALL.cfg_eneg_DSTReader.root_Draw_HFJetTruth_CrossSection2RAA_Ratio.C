{
//=========Macro generated from canvas: Draw_HFJetTruth_CrossSection2RAA_Ratio/Draw_HFJetTruth_CrossSection2RAA_Ratio
//=========  (Sun Jun 11 17:15:32 2017) by ROOT version5.34/34
   TCanvas *Draw_HFJetTruth_CrossSection2RAA_Ratio = new TCanvas("Draw_HFJetTruth_CrossSection2RAA_Ratio", "Draw_HFJetTruth_CrossSection2RAA_Ratio",20,135,700,600);
   gStyle->SetOptFit(1);
   gStyle->SetOptStat(0);
   gStyle->SetOptTitle(0);
   Draw_HFJetTruth_CrossSection2RAA_Ratio->Range(0,0,1,1);
   Draw_HFJetTruth_CrossSection2RAA_Ratio->SetFillColor(0);
   Draw_HFJetTruth_CrossSection2RAA_Ratio->SetBorderMode(0);
   Draw_HFJetTruth_CrossSection2RAA_Ratio->SetBorderSize(2);
   Draw_HFJetTruth_CrossSection2RAA_Ratio->SetTickx(1);
   Draw_HFJetTruth_CrossSection2RAA_Ratio->SetTicky(1);
   Draw_HFJetTruth_CrossSection2RAA_Ratio->SetLeftMargin(0.16);
   Draw_HFJetTruth_CrossSection2RAA_Ratio->SetRightMargin(0.05);
   Draw_HFJetTruth_CrossSection2RAA_Ratio->SetTopMargin(0.05);
   Draw_HFJetTruth_CrossSection2RAA_Ratio->SetBottomMargin(0.16);
   Draw_HFJetTruth_CrossSection2RAA_Ratio->SetFrameBorderMode(0);
  
// ------------>Primitives in pad: Draw_HFJetTruth_CrossSection2RAA_Ratio_1
   TPad *Draw_HFJetTruth_CrossSection2RAA_Ratio_1 = new TPad("Draw_HFJetTruth_CrossSection2RAA_Ratio_1", "Draw_HFJetTruth_CrossSection2RAA_Ratio_1",0.01,0.01,0.99,0.99);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_1->Draw();
   Draw_HFJetTruth_CrossSection2RAA_Ratio_1->cd();
   Draw_HFJetTruth_CrossSection2RAA_Ratio_1->Range(-20.25316,-0.3173624,106.3291,1.666153);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_1->SetFillColor(0);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_1->SetBorderMode(0);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_1->SetBorderSize(2);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_1->SetTickx(1);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_1->SetTicky(1);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_1->SetLeftMargin(0.16);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_1->SetRightMargin(0.05);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_1->SetTopMargin(0.05);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_1->SetBottomMargin(0.16);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_1->SetFrameBorderMode(0);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_1->SetFrameBorderMode(0);
   
   TH1F *h_b_copy1714636915 = new TH1F("h_b_copy1714636915","",20,0,100);
   h_b_copy1714636915->SetBinContent(4,1);
   h_b_copy1714636915->SetBinContent(5,1);
   h_b_copy1714636915->SetBinContent(6,1);
   h_b_copy1714636915->SetBinContent(7,1);
   h_b_copy1714636915->SetBinContent(8,1);
   h_b_copy1714636915->SetBinContent(9,1);
   h_b_copy1714636915->SetBinContent(10,1);
   h_b_copy1714636915->SetBinError(4,0.00449657);
   h_b_copy1714636915->SetBinError(5,0.01081194);
   h_b_copy1714636915->SetBinError(6,0.02470301);
   h_b_copy1714636915->SetBinError(7,0.05437196);
   h_b_copy1714636915->SetBinError(8,0.1327794);
   h_b_copy1714636915->SetBinError(9,0.2969037);
   h_b_copy1714636915->SetBinError(10,0.492359);
   h_b_copy1714636915->SetEntries(58360);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#ff0000");
   h_b_copy1714636915->SetLineColor(ci);
   h_b_copy1714636915->SetLineWidth(2);
   h_b_copy1714636915->SetMarkerStyle(20);
   h_b_copy1714636915->SetMarkerSize(1.2);
   h_b_copy1714636915->GetXaxis()->SetTitle("p_{T} [GeV/c]");
   h_b_copy1714636915->GetXaxis()->SetRange(1,100);
   h_b_copy1714636915->GetXaxis()->SetLabelFont(42);
   h_b_copy1714636915->GetXaxis()->SetLabelSize(0.05);
   h_b_copy1714636915->GetXaxis()->SetTitleSize(0.05);
   h_b_copy1714636915->GetXaxis()->SetTitleOffset(1.4);
   h_b_copy1714636915->GetXaxis()->SetTitleFont(42);
   h_b_copy1714636915->GetYaxis()->SetTitle("Relative Cross Section and Uncertainty");
   h_b_copy1714636915->GetYaxis()->SetLabelFont(42);
   h_b_copy1714636915->GetYaxis()->SetLabelSize(0.05);
   h_b_copy1714636915->GetYaxis()->SetTitleSize(0.05);
   h_b_copy1714636915->GetYaxis()->SetTitleOffset(1.4);
   h_b_copy1714636915->GetYaxis()->SetTitleFont(42);
   h_b_copy1714636915->GetZaxis()->SetLabelFont(42);
   h_b_copy1714636915->GetZaxis()->SetLabelSize(0.05);
   h_b_copy1714636915->GetZaxis()->SetTitleSize(0.05);
   h_b_copy1714636915->GetZaxis()->SetTitleFont(42);
   h_b_copy1714636915->Draw("");
   
   TH1F *h_b_copy1957747793 = new TH1F("h_b_copy1957747793","",20,0,100);
   h_b_copy1957747793->SetBinContent(4,0.6);
   h_b_copy1957747793->SetBinContent(5,0.6);
   h_b_copy1957747793->SetBinContent(6,0.6);
   h_b_copy1957747793->SetBinContent(7,0.6);
   h_b_copy1957747793->SetBinContent(8,0.6);
   h_b_copy1957747793->SetBinContent(9,0.6);
   h_b_copy1957747793->SetBinContent(10,0.6);
   h_b_copy1957747793->SetBinError(4,0.001688489);
   h_b_copy1957747793->SetBinError(5,0.004059949);
   h_b_copy1957747793->SetBinError(6,0.009276129);
   h_b_copy1957747793->SetBinError(7,0.02041699);
   h_b_copy1957747793->SetBinError(8,0.04985944);
   h_b_copy1957747793->SetBinError(9,0.1114891);
   h_b_copy1957747793->SetBinError(10,0.1848837);
   h_b_copy1957747793->SetEntries(58360);

   ci = TColor::GetColor("#0000ff");
   h_b_copy1957747793->SetLineColor(ci);
   h_b_copy1957747793->SetLineWidth(2);
   h_b_copy1957747793->SetMarkerStyle(20);
   h_b_copy1957747793->SetMarkerSize(1.2);
   h_b_copy1957747793->GetXaxis()->SetTitle("p_{T} [GeV/c]");
   h_b_copy1957747793->GetXaxis()->SetRange(1,100);
   h_b_copy1957747793->GetXaxis()->SetLabelFont(42);
   h_b_copy1957747793->GetXaxis()->SetLabelSize(0.05);
   h_b_copy1957747793->GetXaxis()->SetTitleSize(0.05);
   h_b_copy1957747793->GetXaxis()->SetTitleOffset(1.4);
   h_b_copy1957747793->GetXaxis()->SetTitleFont(42);
   h_b_copy1957747793->GetYaxis()->SetTitle("Relative Cross Section and Uncertainty");
   h_b_copy1957747793->GetYaxis()->SetLabelFont(42);
   h_b_copy1957747793->GetYaxis()->SetLabelSize(0.05);
   h_b_copy1957747793->GetYaxis()->SetTitleSize(0.05);
   h_b_copy1957747793->GetYaxis()->SetTitleOffset(1.4);
   h_b_copy1957747793->GetYaxis()->SetTitleFont(42);
   h_b_copy1957747793->GetZaxis()->SetLabelFont(42);
   h_b_copy1957747793->GetZaxis()->SetLabelSize(0.05);
   h_b_copy1957747793->GetZaxis()->SetTitleSize(0.05);
   h_b_copy1957747793->GetZaxis()->SetTitleFont(42);
   h_b_copy1957747793->Draw("same");
   Draw_HFJetTruth_CrossSection2RAA_Ratio_1->Modified();
   Draw_HFJetTruth_CrossSection2RAA_Ratio->cd();
   Draw_HFJetTruth_CrossSection2RAA_Ratio->Modified();
   Draw_HFJetTruth_CrossSection2RAA_Ratio->cd();
   Draw_HFJetTruth_CrossSection2RAA_Ratio->SetSelected(Draw_HFJetTruth_CrossSection2RAA_Ratio);
}
