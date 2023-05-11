{
//=========Macro generated from canvas: Draw_HFJetTruth_CrossSection2RAA_Ratio_3yr_deta0.70/Draw_HFJetTruth_CrossSection2RAA_Ratio_3yr_deta0.70
//=========  (Tue Apr  9 10:39:18 2019) by ROOT version5.34/38
   TCanvas *Draw_HFJetTruth_CrossSection2RAA_Ratio_3yr_deta0.70 = new TCanvas("Draw_HFJetTruth_CrossSection2RAA_Ratio_3yr_deta0.70", "Draw_HFJetTruth_CrossSection2RAA_Ratio_3yr_deta0.70",8,65,700,600);
   gStyle->SetOptFit(1);
   gStyle->SetOptStat(0);
   gStyle->SetOptTitle(0);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_3yr_deta0.70->Range(0,0,1,1);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_3yr_deta0.70->SetFillColor(0);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_3yr_deta0.70->SetBorderMode(0);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_3yr_deta0.70->SetBorderSize(2);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_3yr_deta0.70->SetTickx(1);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_3yr_deta0.70->SetTicky(1);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_3yr_deta0.70->SetLeftMargin(0.16);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_3yr_deta0.70->SetRightMargin(0.05);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_3yr_deta0.70->SetTopMargin(0.05);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_3yr_deta0.70->SetBottomMargin(0.16);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_3yr_deta0.70->SetFrameBorderMode(0);
  
// ------------>Primitives in pad: Draw_HFJetTruth_CrossSection2RAA_Ratio_3yr_deta0.70_1
   TPad *Draw_HFJetTruth_CrossSection2RAA_Ratio_3yr_deta0.70_1 = new TPad("Draw_HFJetTruth_CrossSection2RAA_Ratio_3yr_deta0.70_1", "Draw_HFJetTruth_CrossSection2RAA_Ratio_3yr_deta0.70_1",0.01,0.01,0.99,0.99);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_3yr_deta0.70_1->Draw();
   Draw_HFJetTruth_CrossSection2RAA_Ratio_3yr_deta0.70_1->cd();
   Draw_HFJetTruth_CrossSection2RAA_Ratio_3yr_deta0.70_1->Range(-20.25316,-0.4204901,106.3291,2.207573);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_3yr_deta0.70_1->SetFillColor(0);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_3yr_deta0.70_1->SetBorderMode(0);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_3yr_deta0.70_1->SetBorderSize(2);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_3yr_deta0.70_1->SetTickx(1);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_3yr_deta0.70_1->SetTicky(1);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_3yr_deta0.70_1->SetLeftMargin(0.16);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_3yr_deta0.70_1->SetRightMargin(0.05);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_3yr_deta0.70_1->SetTopMargin(0.05);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_3yr_deta0.70_1->SetBottomMargin(0.16);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_3yr_deta0.70_1->SetFrameBorderMode(0);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_3yr_deta0.70_1->SetFrameBorderMode(0);
   
   TH1F *h_b_copy846930886 = new TH1F("h_b_copy846930886","",20,0,100);
   h_b_copy846930886->SetBinContent(4,1);
   h_b_copy846930886->SetBinContent(5,1);
   h_b_copy846930886->SetBinContent(6,1);
   h_b_copy846930886->SetBinContent(7,1);
   h_b_copy846930886->SetBinContent(8,1);
   h_b_copy846930886->SetBinContent(9,1);
   h_b_copy846930886->SetBinContent(10,1);
   h_b_copy846930886->SetBinError(4,0.008925437);
   h_b_copy846930886->SetBinError(5,0.02146109);
   h_b_copy846930886->SetBinError(6,0.04903408);
   h_b_copy846930886->SetBinError(7,0.1079253);
   h_b_copy846930886->SetBinError(8,0.2635595);
   h_b_copy846930886->SetBinError(9,0.5893369);
   h_b_copy846930886->SetBinError(10,0.9773048);
   h_b_copy846930886->SetEntries(58360);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#ff0000");
   h_b_copy846930886->SetLineColor(ci);
   h_b_copy846930886->SetLineWidth(2);
   h_b_copy846930886->SetMarkerStyle(20);
   h_b_copy846930886->SetMarkerSize(1.2);
   h_b_copy846930886->GetXaxis()->SetTitle("p_{T} [GeV/c]");
   h_b_copy846930886->GetXaxis()->SetRange(1,100);
   h_b_copy846930886->GetXaxis()->SetLabelFont(42);
   h_b_copy846930886->GetXaxis()->SetLabelSize(0.05);
   h_b_copy846930886->GetXaxis()->SetTitleSize(0.05);
   h_b_copy846930886->GetXaxis()->SetTitleOffset(1.4);
   h_b_copy846930886->GetXaxis()->SetTitleFont(42);
   h_b_copy846930886->GetYaxis()->SetTitle("Relative Cross Section and Uncertainty");
   h_b_copy846930886->GetYaxis()->SetLabelFont(42);
   h_b_copy846930886->GetYaxis()->SetLabelSize(0.05);
   h_b_copy846930886->GetYaxis()->SetTitleSize(0.05);
   h_b_copy846930886->GetYaxis()->SetTitleOffset(1.4);
   h_b_copy846930886->GetYaxis()->SetTitleFont(42);
   h_b_copy846930886->GetZaxis()->SetLabelFont(42);
   h_b_copy846930886->GetZaxis()->SetLabelSize(0.05);
   h_b_copy846930886->GetZaxis()->SetTitleSize(0.05);
   h_b_copy846930886->GetZaxis()->SetTitleFont(42);
   h_b_copy846930886->Draw("");
   
   TH1F *h_b_copy1681692777 = new TH1F("h_b_copy1681692777","",20,0,100);
   h_b_copy1681692777->SetBinContent(4,0.6);
   h_b_copy1681692777->SetBinContent(5,0.6);
   h_b_copy1681692777->SetBinContent(6,0.6);
   h_b_copy1681692777->SetBinContent(7,0.6);
   h_b_copy1681692777->SetBinContent(8,0.6);
   h_b_copy1681692777->SetBinContent(9,0.6);
   h_b_copy1681692777->SetBinContent(10,0.6);
   h_b_copy1681692777->SetBinError(4,0.00334669);
   h_b_copy1681692777->SetBinError(5,0.008047071);
   h_b_copy1681692777->SetBinError(6,0.01838587);
   h_b_copy1681692777->SetBinError(7,0.04046775);
   h_b_copy1681692777->SetBinError(8,0.09882451);
   h_b_copy1681692777->SetBinError(9,0.2209783);
   h_b_copy1681692777->SetBinError(10,0.3664511);
   h_b_copy1681692777->SetEntries(58360);

   ci = TColor::GetColor("#0000ff");
   h_b_copy1681692777->SetLineColor(ci);
   h_b_copy1681692777->SetLineWidth(2);
   h_b_copy1681692777->SetMarkerStyle(20);
   h_b_copy1681692777->SetMarkerSize(1.2);
   h_b_copy1681692777->GetXaxis()->SetTitle("p_{T} [GeV/c]");
   h_b_copy1681692777->GetXaxis()->SetRange(1,100);
   h_b_copy1681692777->GetXaxis()->SetLabelFont(42);
   h_b_copy1681692777->GetXaxis()->SetLabelSize(0.05);
   h_b_copy1681692777->GetXaxis()->SetTitleSize(0.05);
   h_b_copy1681692777->GetXaxis()->SetTitleOffset(1.4);
   h_b_copy1681692777->GetXaxis()->SetTitleFont(42);
   h_b_copy1681692777->GetYaxis()->SetTitle("Relative Cross Section and Uncertainty");
   h_b_copy1681692777->GetYaxis()->SetLabelFont(42);
   h_b_copy1681692777->GetYaxis()->SetLabelSize(0.05);
   h_b_copy1681692777->GetYaxis()->SetTitleSize(0.05);
   h_b_copy1681692777->GetYaxis()->SetTitleOffset(1.4);
   h_b_copy1681692777->GetYaxis()->SetTitleFont(42);
   h_b_copy1681692777->GetZaxis()->SetLabelFont(42);
   h_b_copy1681692777->GetZaxis()->SetLabelSize(0.05);
   h_b_copy1681692777->GetZaxis()->SetTitleSize(0.05);
   h_b_copy1681692777->GetZaxis()->SetTitleFont(42);
   h_b_copy1681692777->Draw("same");
   Draw_HFJetTruth_CrossSection2RAA_Ratio_3yr_deta0.70_1->Modified();
   Draw_HFJetTruth_CrossSection2RAA_Ratio_3yr_deta0.70->cd();
   Draw_HFJetTruth_CrossSection2RAA_Ratio_3yr_deta0.70->Modified();
   Draw_HFJetTruth_CrossSection2RAA_Ratio_3yr_deta0.70->cd();
   Draw_HFJetTruth_CrossSection2RAA_Ratio_3yr_deta0.70->SetSelected(Draw_HFJetTruth_CrossSection2RAA_Ratio_3yr_deta0.70);
}
