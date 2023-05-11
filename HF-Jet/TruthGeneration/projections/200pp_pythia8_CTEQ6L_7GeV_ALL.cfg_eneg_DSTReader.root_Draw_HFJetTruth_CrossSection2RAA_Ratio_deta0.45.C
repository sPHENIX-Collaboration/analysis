{
//=========Macro generated from canvas: Draw_HFJetTruth_CrossSection2RAA_Ratio_deta0.45/Draw_HFJetTruth_CrossSection2RAA_Ratio_deta0.45
//=========  (Mon Dec  4 09:14:02 2017) by ROOT version5.34/36
   TCanvas *Draw_HFJetTruth_CrossSection2RAA_Ratio_deta0.45 = new TCanvas("Draw_HFJetTruth_CrossSection2RAA_Ratio_deta0.45", "Draw_HFJetTruth_CrossSection2RAA_Ratio_deta0.45",8,69,700,600);
   gStyle->SetOptFit(1);
   gStyle->SetOptStat(0);
   gStyle->SetOptTitle(0);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_deta0.45->Range(0,0,1,1);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_deta0.45->SetFillColor(0);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_deta0.45->SetBorderMode(0);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_deta0.45->SetBorderSize(2);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_deta0.45->SetTickx(1);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_deta0.45->SetTicky(1);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_deta0.45->SetLeftMargin(0.16);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_deta0.45->SetRightMargin(0.05);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_deta0.45->SetTopMargin(0.05);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_deta0.45->SetBottomMargin(0.16);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_deta0.45->SetFrameBorderMode(0);
  
// ------------>Primitives in pad: Draw_HFJetTruth_CrossSection2RAA_Ratio_deta0.45_1
   TPad *Draw_HFJetTruth_CrossSection2RAA_Ratio_deta0.45_1 = new TPad("Draw_HFJetTruth_CrossSection2RAA_Ratio_deta0.45_1", "Draw_HFJetTruth_CrossSection2RAA_Ratio_deta0.45_1",0.01,0.01,0.99,0.99);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_deta0.45_1->Draw();
   Draw_HFJetTruth_CrossSection2RAA_Ratio_deta0.45_1->cd();
   Draw_HFJetTruth_CrossSection2RAA_Ratio_deta0.45_1->Range(-20.25316,-0.3422642,106.3291,1.796887);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_deta0.45_1->SetFillColor(0);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_deta0.45_1->SetBorderMode(0);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_deta0.45_1->SetBorderSize(2);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_deta0.45_1->SetTickx(1);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_deta0.45_1->SetTicky(1);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_deta0.45_1->SetLeftMargin(0.16);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_deta0.45_1->SetRightMargin(0.05);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_deta0.45_1->SetTopMargin(0.05);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_deta0.45_1->SetBottomMargin(0.16);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_deta0.45_1->SetFrameBorderMode(0);
   Draw_HFJetTruth_CrossSection2RAA_Ratio_deta0.45_1->SetFrameBorderMode(0);
   
   TH1F *h_b_copy846930886 = new TH1F("h_b_copy846930886","",20,0,100);
   h_b_copy846930886->SetBinContent(4,1);
   h_b_copy846930886->SetBinContent(5,1);
   h_b_copy846930886->SetBinContent(6,1);
   h_b_copy846930886->SetBinContent(7,1);
   h_b_copy846930886->SetBinContent(8,1);
   h_b_copy846930886->SetBinContent(9,1);
   h_b_copy846930886->SetBinContent(10,1);
   h_b_copy846930886->SetBinError(4,0.005565988);
   h_b_copy846930886->SetBinError(5,0.01338334);
   h_b_copy846930886->SetBinError(6,0.03057812);
   h_b_copy846930886->SetBinError(7,0.06730322);
   h_b_copy846930886->SetBinError(8,0.1643582);
   h_b_copy846930886->SetBinError(9,0.3675161);
   h_b_copy846930886->SetBinError(10,0.6094566);
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
   h_b_copy1681692777->SetBinError(4,0.003187988);
   h_b_copy1681692777->SetBinError(5,0.007665474);
   h_b_copy1681692777->SetBinError(6,0.017514);
   h_b_copy1681692777->SetBinError(7,0.03854875);
   h_b_copy1681692777->SetBinError(8,0.09413819);
   h_b_copy1681692777->SetBinError(9,0.2104994);
   h_b_copy1681692777->SetBinError(10,0.3490738);
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
   Draw_HFJetTruth_CrossSection2RAA_Ratio_deta0.45_1->Modified();
   Draw_HFJetTruth_CrossSection2RAA_Ratio_deta0.45->cd();
   Draw_HFJetTruth_CrossSection2RAA_Ratio_deta0.45->Modified();
   Draw_HFJetTruth_CrossSection2RAA_Ratio_deta0.45->cd();
   Draw_HFJetTruth_CrossSection2RAA_Ratio_deta0.45->SetSelected(Draw_HFJetTruth_CrossSection2RAA_Ratio_deta0.45);
}
