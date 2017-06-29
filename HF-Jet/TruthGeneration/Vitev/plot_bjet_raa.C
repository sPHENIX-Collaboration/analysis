/*
Here are preliminary results for the RAA of b-jets
I have used the parameterization of Cesar (had convert it to d^2pT)
I have used 10% g and 90 % b quarks
 3 radii  0.2, 0.4 and 0.6  - there are names 2, 4, 6 in the name of files
two different couplings g= 2  (labeled g41.0)  and g=2.2 labeled g41.5
I suggest using g=2.2 as default
since ichi is exponential parameterizations and things will start falling steeply,
I sugest not using results beyond  60 GeV
This should give you a good sense of the expected suppression and p-T dependence
as a function of radius and coupling between the jet and the media,
Ivan Vitev, May 12, 2017
 */
void plot_bjet_raa()
{
  TGraph* gdef = new TGraph("R_SigR4.W0.0g41.5.SPEHIXdat");
  TGraph* ggeq2 = new TGraph("R_SigR4.W0.0g41.0.SPEHIXdat");
  gdef->SetLineColor(kBlack);
  gdef->SetLineWidth(4);
  ggeq2->SetLineWidth(4);
  ggeq2->SetLineColor(kBlue);
  TCanvas *c1 = new TCanvas("c1", "c1",10,45,700,502);
  gStyle->SetOptTitle(1);
  c1->SetHighLightColor(0);
  c1->Range(-9.413793,-0.2360656,62.7931,1.321311);
  c1->SetFillColor(0);
  c1->SetBorderMode(0);
  c1->SetBorderSize(0);
  c1->SetLeftMargin(0.1303725);
  c1->SetRightMargin(0.03868195);
  c1->SetTopMargin(0.07789474);
  c1->SetBottomMargin(0.1515789);
  c1->SetFrameBorderMode(0);

  TH2F* hframe = new TH2F("hframe","Jet radii = 0.4;p_{T} [GeV/c];R_{AA}",
			  100,0,60,100,0,1.2);
  hframe->GetXaxis()->SetLabelFont(132);
  hframe->GetXaxis()->SetLabelSize(0.06);
  hframe->GetXaxis()->SetTitleSize(0.07);
  hframe->GetXaxis()->SetTitleOffset(0.9);
  hframe->GetXaxis()->SetTitleFont(132);
  hframe->GetYaxis()->SetTitle("R_{AA}");
  hframe->GetYaxis()->SetNdivisions(310);
  hframe->GetYaxis()->SetLabelFont(132);
  hframe->GetYaxis()->SetLabelSize(0.06);
  hframe->GetYaxis()->SetTitleSize(0.08);
  hframe->GetYaxis()->SetTitleOffset(0.65);
  hframe->GetYaxis()->SetTitleFont(132);
  hframe->SetTitleFont(132);
  hframe->SetTitleSize(0.06315789);
  hframe->Draw();

  gdef->Draw("L");
  ggeq2->Draw("L");

  TLegend* l = new TLegend(0.3323782,0.6221053,0.6332378,0.9231579, 
			   "Au+Au#rightarrow b-jet @ #sqrt{s_{NN}}=200 GeV");
  l->SetBorderSize(0);
  l->SetFillStyle(0);
  l->SetTextFont(132);
  l->SetTextSize(0.06315789);
  l->AddEntry(ggeq2,"coupling g=2.0","L");
  l->AddEntry(gdef,"coupling g=2.2","L");
  l->Draw();

}
