void SetHist(TH2* h, string xt ="", string yt ="",int color = 4, int marker = 20,int width = 3, float size = 1.0);
void SetHist(TH2* h, int color = 4);
void SetLeg(TLegend* l,float txtsize=0.03);


void DrawUECentralityPlots(string infile = "UEvsEtaCentrality.root"){

  gStyle->SetOptStat(0);

  TFile *_file0 = TFile::Open(infile.c_str());
    
  TH2*  hv2_cent = (TH2*)gROOT->FindObject("hv2_cent");
  TH2*  hPsi2_cent = (TH2*)gROOT->FindObject("hPsi2_cent");
  TCanvas* can1 = new TCanvas("can1","",800,400);
  can1->Divide(2,1,0,0);

  can1->cd(1);
  gPad->SetLeftMargin(0.15);
  gPad->SetRightMargin(.15);
  gPad->SetTopMargin(.07);
  gPad->SetBottomMargin(.10);
  SetHist(hv2_cent,"Centrality %","v2 value",1);
  hv2_cent->SetTitle("v2 vs Centrality");
  hv2_cent->Draw("ColZ");
    
  TProfile* v2profile = hv2_cent->ProfileX("Mean Values of v2");
    
  gStyle->SetTitleTextColor(1);
  v2profile->Draw("same");

  can1->cd(2);
  gPad->SetLeftMargin(0.15);
  gPad->SetRightMargin(.15);
  gPad->SetTopMargin(.07);
  gPad->SetBottomMargin(.10);
  SetHist(hPsi2_cent,"Centrality %","Psi2 value",1);
  hPsi2_cent->SetTitle("Psi2 vs Centrality");
  hPsi2_cent->Draw("ColZ");

  TProfile* Psi2profile = hPsi2_cent->ProfileX("Mean Values of Psi2");

  gStyle->SetTitleTextColor(1);
  Psi2profile->Draw("same");
 
  TH2*  hUEiHcalEta_Cent0_20 = (TH2*)gROOT->FindObject("hUEiHcalEta_Cent0_20");
  TH2*  hUEoHcalEta_Cent0_20 = (TH2*)gROOT->FindObject("hUEoHcalEta_Cent0_20");
  TH2*  hUEemcalEta_Cent0_20 = (TH2*)gROOT->FindObject("hUEemcalEta_Cent0_20");

  TH2*  hUEiHcalEta_Cent20_50 = (TH2*)gROOT->FindObject("hUEiHcalEta_Cent20_50");                                        
  TH2*  hUEoHcalEta_Cent20_50 = (TH2*)gROOT->FindObject("hUEoHcalEta_Cent20_50");
  TH2*  hUEemcalEta_Cent20_50 = (TH2*)gROOT->FindObject("hUEemcalEta_Cent20_50");
  
  TH2*  hUEiHcalEta_Cent50_100 = (TH2*)gROOT->FindObject("hUEiHcalEta_Cent50_100");                                         
  TH2*  hUEoHcalEta_Cent50_100 = (TH2*)gROOT->FindObject("hUEoHcalEta_Cent50_100");
  TH2*  hUEemcalEta_Cent50_100 = (TH2*)gROOT->FindObject("hUEemcalEta_Cent50_100");
  
  TCanvas* can2 = new TCanvas("can2","",1200,400);
  can2->Divide(3,1,0,0);
  
  can2->cd(1);
  gPad->SetLeftMargin(0.15);
  gPad->SetRightMargin(.10);
  gPad->SetTopMargin(.07);
  gPad->SetBottomMargin(.10);
  SetHist(hUEiHcalEta_Cent0_20,"Eta","UE Size/Tower (GeV)",1);
  hUEiHcalEta_Cent0_20->SetTitle("UE Size vs Eta 0 - 20 % Centrality Range iHCal");
  hUEiHcalEta_Cent0_20->Draw("ColZ");
  hUEiHcalEta_Cent0_20->ProfileX()->Draw("same");

  can2->cd(2);
  gPad->SetLeftMargin(0.15);
  gPad->SetRightMargin(.15);
  gPad->SetTopMargin(.07);
  gPad->SetBottomMargin(.10);
  SetHist(hUEiHcalEta_Cent20_50,"Eta","UE Size/Tower (GeV)",1);                                                                  
  hUEiHcalEta_Cent20_50->SetTitle("UE Size vs Eta 20 - 50 % Centrality Range iHCal");
  hUEiHcalEta_Cent20_50->GetYaxis()->SetRangeUser(0.0,0.05);
  hUEiHcalEta_Cent20_50->Draw("ColZ");
  hUEiHcalEta_Cent20_50->ProfileX()->Draw("same");

  can2->cd(3);
  gPad->SetLeftMargin(0.15);
  gPad->SetRightMargin(.10);
  gPad->SetTopMargin(.07);
  gPad->SetBottomMargin(.10);
  SetHist(hUEiHcalEta_Cent50_100,"Eta","UE Size/Tower (GeV)",1);                                                         
  hUEiHcalEta_Cent50_100->SetTitle("UE Size vs Eta 50 - 100 % Centrality Range iHCal");
  hUEiHcalEta_Cent50_100->GetYaxis()->SetRangeUser(0.0,0.03);
  hUEiHcalEta_Cent50_100->Draw("ColZ");
  hUEiHcalEta_Cent50_100->ProfileX()->Draw("same"); 

  TCanvas* can3 = new TCanvas("can3","",1200,400);
  can3->Divide(3,1,0,0);
  
  can3->cd(1);
  gPad->SetLeftMargin(0.15);
  gPad->SetRightMargin(.10);
  gPad->SetTopMargin(.07);
  gPad->SetBottomMargin(.10);
  SetHist(hUEoHcalEta_Cent0_20,"Eta","UE Size/Tower (GeV)",1);
  hUEoHcalEta_Cent0_20->SetTitle("UE Size vs Eta 0 - 20 % Centrality Range oHCal");                                      
  hUEoHcalEta_Cent0_20->Draw("ColZ");
  hUEoHcalEta_Cent0_20->ProfileX()->Draw("same");  

  can3->cd(2);
  gPad->SetLeftMargin(0.15);
  gPad->SetRightMargin(.15);
  gPad->SetTopMargin(.07);
  gPad->SetBottomMargin(.10);
  SetHist(hUEoHcalEta_Cent20_50,"Eta","UE Size/Tower (GeV)",1);                                                              
  hUEoHcalEta_Cent20_50->SetTitle("UE Size vs Eta 20 - 50 % Centrality Range oHCal");
  hUEoHcalEta_Cent20_50->GetYaxis()->SetRangeUser(0.0,0.25);
  hUEoHcalEta_Cent20_50->Draw("ColZ");
  hUEoHcalEta_Cent20_50->ProfileX()->Draw("same");
  
  can3->cd(3);
  gPad->SetLeftMargin(0.15);
  gPad->SetRightMargin(.10);
  gPad->SetTopMargin(.07);
  gPad->SetBottomMargin(.10);
  SetHist(hUEoHcalEta_Cent50_100,"Eta","UE Size/Tower (GeV)",1);                                                             
  hUEoHcalEta_Cent50_100->SetTitle("UE Size vs Eta 50 - 100 % Centrality Range oHCal");
  hUEoHcalEta_Cent50_100->GetYaxis()->SetRangeUser(0.0,0.15);
  hUEoHcalEta_Cent50_100->Draw("ColZ");
  hUEoHcalEta_Cent50_100->ProfileX()->Draw("same");

  TCanvas* can4 = new TCanvas("can4","",1200,400);
  can4->Divide(3,1,0,0);

  
  can4->cd(1);
  gPad->SetLeftMargin(0.15);
  gPad->SetRightMargin(.10);
  gPad->SetTopMargin(.07);
  gPad->SetBottomMargin(.10);
  SetHist(hUEemcalEta_Cent0_20,"Eta","UE Size/Tower (GeV)",1);                                                           
  hUEemcalEta_Cent0_20->SetTitle("UE Size vs Eta 0 - 20 % Centrality Range EMCal");
  hUEemcalEta_Cent0_20->Draw("ColZ");
  hUEemcalEta_Cent0_20->ProfileX()->Draw("same");  

  can4->cd(2);
  gPad->SetLeftMargin(0.15);
  gPad->SetRightMargin(.15);
  gPad->SetTopMargin(.07);
  gPad->SetBottomMargin(.10);
  SetHist(hUEemcalEta_Cent20_50,"Eta","UE Size/Tower (GeV)",1);
  hUEemcalEta_Cent20_50->SetTitle("UE Size vs Eta 20 - 50 % Centrality Range EMCal");
  hUEemcalEta_Cent20_50->GetYaxis()->SetRangeUser(0.0,0.60);
  hUEemcalEta_Cent20_50->Draw("ColZ");
  hUEemcalEta_Cent20_50->ProfileX()->Draw("same");
   
  can4->cd(3);
  gPad->SetLeftMargin(0.15);
  gPad->SetRightMargin(.10);
  gPad->SetTopMargin(.07);
  gPad->SetBottomMargin(.10);
  SetHist(hUEemcalEta_Cent50_100,"Eta","UE Size/Tower (GeV)",1);
  hUEemcalEta_Cent50_100->SetTitle("UE Size vs Eta 50 - 100 % Centrality Range EMCal");
  hUEemcalEta_Cent50_100->GetYaxis()->SetRangeUser(0.0,0.30);
  hUEemcalEta_Cent50_100->Draw("ColZ");
  hUEemcalEta_Cent50_100->ProfileX()->Draw("same");
  
  
  TProfile *IH0_20 = hUEiHcalEta_Cent0_20->ProfileX();                                                                   
  TProfile *IH20_50 = hUEiHcalEta_Cent20_50->ProfileX();                                                                 
  TProfile *IH50_100 = hUEiHcalEta_Cent50_100->ProfileX();
  TProfile *OH0_20 = hUEoHcalEta_Cent0_20->ProfileX();                                                                 
  TProfile *OH20_50 = hUEoHcalEta_Cent20_50->ProfileX();                                                               
  TProfile *OH50_100 = hUEoHcalEta_Cent50_100->ProfileX();
  TProfile *EM0_20 = hUEemcalEta_Cent0_20->ProfileX();
  TProfile *EM20_50 = hUEemcalEta_Cent20_50->ProfileX();
  TProfile *EM50_100 = hUEemcalEta_Cent50_100->ProfileX();

  TCanvas* can5 = new TCanvas("can5","",1200,400);
  can5->Divide(3,1,0,0);
  can5->cd(1);
  gPad->SetLeftMargin(0.15);
  gPad->SetRightMargin(.10);
  gPad->SetTopMargin(.07);
  gPad->SetBottomMargin(.10);
  IH0_20->SetTitle("Mean Values of UE Size vs Eta iHCal");                                                               
  IH0_20->GetYaxis()->SetTitle("UE Size/Tower (GeV)");                                                                   
  IH0_20->GetYaxis()->SetRangeUser(0.0,0.08);
  IH0_20->GetXaxis()->SetTitle("Eta");
  IH0_20->Draw();
  IH20_50->SetMarkerColor(1);
  IH50_100->SetMarkerColor(2);
  IH20_50->Draw("same");
  IH50_100->Draw("same");
  
  TLegend* leg1 = new TLegend(0.4,0.7,0.7,0.85);
  SetLeg(leg1);
  leg1->AddEntry(IH0_20,"0-20 % Centrality","P");
  leg1->AddEntry(IH20_50,"20-50 % Centrality","P");
  leg1->AddEntry(IH50_100,"50-100 % Centrality","P");
  leg1->Draw();


  can5->cd(2);
  gPad->SetLeftMargin(0.15);
  gPad->SetRightMargin(.10);
  gPad->SetTopMargin(.07);
  gPad->SetBottomMargin(.10);
  OH0_20->SetTitle("Mean Values of UE Size vs Eta oHCal");
  OH0_20->GetYaxis()->SetTitle("UE Size/Tower (GeV)");
  OH0_20->GetXaxis()->SetTitle("Eta");
  OH0_20->GetYaxis()->SetRangeUser(0.0,0.30);
  OH0_20->Draw();
  OH20_50->SetMarkerColor(1);
  OH50_100->SetMarkerColor(2);
  OH20_50->Draw("same");
  OH50_100->Draw("same");

  TLegend* leg2 = new TLegend(0.4,0.7,0.7,0.85);
  SetLeg(leg2);
  leg2->AddEntry(OH0_20,"0-20 % Centrality","P");                                                                        leg2->AddEntry(OH20_50,"20-50 % Centrality","P");                                                                      leg2->AddEntry(OH50_100,"50-100 % Centrality","P");                                                                    leg2->Draw();

  can5->cd(3);
  gPad->SetLeftMargin(0.15);
  gPad->SetRightMargin(.10);
  gPad->SetTopMargin(.07);
  gPad->SetBottomMargin(.10);
  EM0_20->SetTitle("Mean Values of UE Size vs Eta EMCal");                                                               
  EM0_20->GetYaxis()->SetTitle("UE Size/Tower (GeV)");                                                                   
  EM0_20->GetXaxis()->SetTitle("Eta");
  EM0_20->GetYaxis()->SetRangeUser(0.0,0.70);
  EM0_20->Draw();
  EM20_50->SetMarkerColor(1);
  EM50_100->SetMarkerColor(2);
  EM20_50->Draw("same");
  EM50_100->Draw("same");
  

  TLegend* leg3 = new TLegend(0.4,0.7,0.7,0.85);
  SetLeg(leg3);
  leg3->AddEntry(EM0_20,"0-20 % Centrality","P");                                                                        leg3->AddEntry(EM20_50,"20-50 % Centrality","P");                                                                      leg3->AddEntry(EM50_100,"50-100 % Centrality","P");                                                                    leg3->Draw();
  }

void SetHist(TH2* h, string xt ="", string yt ="",int color = 4, int marker = 24,int width = 3, float size = 1.0)
{
  h->SetLineWidth(width);  
  h->SetLineColor(color);
  h->SetMarkerColor(3);
  h->SetMarkerSize(0.5);
  h->SetMarkerStyle(marker);
  h->GetYaxis()->SetTitle(yt.c_str());
  h->GetYaxis()->SetTitleOffset(1.6);
  h->GetXaxis()->SetTitle(xt.c_str());
}

void SetHist(TH2* h, int color = 4)
{
  h->SetLineWidth(3);
  h->SetLineColor(color);
  h->SetMarkerColor(color);
  h->SetMarkerSize(0.25);
  h->GetYaxis()->SetTitleOffset(1.6);
    
}


void SetLeg(TLegend* l,float txtsize=0.04){
  l->SetBorderSize(1);
  l->SetFillColor(0);
  l->SetTextSize(txtsize);
}
