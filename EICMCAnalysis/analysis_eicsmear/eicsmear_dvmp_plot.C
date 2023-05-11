#include <vector>
int eicsmear_dvmp_plot()
{

  /* PRELIMINARY ROOT STUFF */


  /* Loading libraries and setting sphenix style */
  gSystem->Load("libeicsmear");
  gROOT->LoadMacro("/sphenix/user/gregtom3/SBU/research/macros/macros/sPHENIXStyle/sPhenixStyle.C");
  SetsPhenixStyle();

  /* Open TFile */
  TFile *input = new TFile("/sphenix/user/gregtom3/data/Fall2018/LOI_smear_studies/DVMP.root","READ");

  /* Open histograms in TFile */
  TH1F *h1_invariant_mass = (TH1F*)input->Get("h1_invariant_mass"); 
  TH1F *h1_invariant_mass_comb = (TH1F*)input->Get("h1_invariant_mass_comb");
  TH1F *h1_invariant_mass_alone = (TH1F*)input->Get("h1_invariant_mass_alone");

  
  TCanvas *c_inv = new TCanvas("c_inv","",700,500);
  h1_invariant_mass_comb->SetLineColor(kGray);
  h1_invariant_mass_comb->SetFillColor(kGray);
  h1_invariant_mass->GetXaxis()->SetTitle("e^{-}e^{+} Invariant Mass [GeV/c^{2}]");
  h1_invariant_mass->GetYaxis()->SetTitle("Entries");

  //Scale for Luminosity
  h1_invariant_mass->Scale(10/1.55);
  h1_invariant_mass_comb->Scale(10/1.55);
  h1_invariant_mass_alone->Scale(10/1.55);

  
  

  // Legend text
  auto legend = new TLegend(0.6,0.65,0.8,0.75);
  legend->SetTextSize(0.03);
  legend->AddEntry(h1_invariant_mass,"All e^{+}e^{-} pairs","l");
  legend->AddEntry(h1_invariant_mass_comb,"Combinatorial background","l");
  
  //h1_invariant_mass->Draw();
  //h1_invariant_mass_comb->Draw("same");
  //h1_invariant_mass->Draw("same");
  legend->Draw("same");
  gPad->SetLogy();
  gPad->RedrawAxis();
  c_inv->SaveAs("inv_mass_poly.eps");

  TCanvas *c_sing = new TCanvas("c_sing","",700,500);
  double x[100]; double y[100]; double ex[100]; double ey[100];
  for(int entry = 0; entry<100; entry++)
    {
      x[entry] = h1_invariant_mass->GetXaxis()->GetBinCenter(entry);
      if(entry==0) 
	y[entry]=h1_invariant_mass->GetBinContent(1);
      else
	y[entry] = h1_invariant_mass->GetBinContent(entry);
      ey[entry] = sqrt(h1_invariant_mass->GetBinContent(entry));
      ex[entry] = 0;
     
    }
  
  auto gr = new TGraph(100,x,y);
  auto gr_error = new TGraphErrors(100,x,y,ex,ey);
  gr_error->SetMarkerSize(1);
  gr_error->SetMarkerColor(kBlack);
  gr->SetMarkerSize(.5);
  gr->SetMarkerColor(kBlue);
 
  gr->Draw("AP");

  gr->GetXaxis()->SetTitle("e^{+}e^{-} Invariant Mass [GeV/c^{2}]");
  gr->GetYaxis()->SetTitle("Entries");
  //gr->GetYaxis()->SetTitleOffset(1.6);
  auto th2 = new TLatex(3.16,38000,"Luminosity 10 fb^{-1}");
  th2->SetTextSize(0.04);
  th2->Draw("same");
  h1_invariant_mass_comb->Draw("same");
  c_sing->SaveAs("inv_mass_sing.eps");

  TCanvas *c_nils = new TCanvas("c_nils","",700,500);
  //gr_error->Draw("ap");
  h1_invariant_mass_alone->SetMarkerColor(kWhite);
  h1_invariant_mass_alone->SetLineColor(kWhite);
  gr_error->GetXaxis()->SetRangeUser(2.95,3.25);
  gr_error->GetXaxis()->SetTitle("e^{+}e^{-} Invariant Mass [GeV/c^{2}]");
  gr_error->GetYaxis()->SetTitle("Entries");
  gr_error->GetYaxis()->SetNdivisions(505);
  gr_error->Draw("aP");
  // h1_invariant_mass_alone->GetYaxis()->SetRangeUser(0,50000);
  /*h1_invariant_mass_alone->Draw();
  h1_invariant_mass_comb->SetLineWidth(0);
  h1_invariant_mass_comb->Draw("same");
  gr->Draw("p");
  gPad->RedrawAxis();
  */
  //h1_invariant_mass_alone->Draw();
  using namespace RooFit;
  //double hmin = h1_invariant_mass_alone->GetXaxis()->GetXmin();
  //double hmax = h1_invariant_mass_alone->GetXaxis()->GetXmax();

  double hmin = 3.075;
  double hmax = 3.125;
  // Declare observable x
  RooRealVar xx("xx","e^{+}e^{-} Invariant Mass [GeV/c^{2}]",hmin,hmax);
  RooDataHist dh("dh","Entries",xx,h1_invariant_mass);
  RooPlot* frame = xx.frame("Z mass");
  //h1_invariant_mass->Draw();
  h1_invariant_mass->GetYaxis()->SetNdivisions(505);
  //frame->Draw("same");
  //dh.plotOn(frame,MarkerColor(4),MarkerSize(2),MarkerStyle(20));  //this will show histogram data points on canvas 
  //dh.statOn(frame);  //this will display hist stat on canvas

  RooRealVar mean("mean","mean",3.096, 3, 1200.0);
  RooRealVar width("width","width",0.0926, 0.0900, 0.1);
  RooRealVar sigma("sigma","sigma",0.02, 0.0, 100.0);
  RooRealVar alpha("alpha","alpha",0.02, -1000.0, 1000.0);
  RooRealVar n("n","n",3, -5, 5);
  RooGaussian gauss("gauss","gauss",xx,mean,sigma);
  //RooCBShape gauss("gauss","gauss",xx,mean,sigma,alpha,n);
  //RooBreitWigner gauss("gauss","gauss",xx,mean,sigma);
  //RooVoigtian gauss("gauss","gauss",xx,mean,width,sigma);
  //h1_invariant_mass->SetLineColor(kWhite);
  //h1_invariant_mass->Draw("same");
  RooFitResult* filters = gauss.fitTo(dh,"qr");
  //h1_invariant_mass_alone->SetLineColor(kWhite);
  //h1_invariant_mass_alone->Draw("same");
  frame->GetYaxis()->SetTitle("Entries");
  frame->GetYaxis()->SetNdivisions(505);
  
  
  TF1 *gfit = new TF1("Gaussian","gaus",hmin,hmax); // Create the fit function
  h1_invariant_mass->Fit("Gaussian","RQ0"); //
  gfit->SetLineColor(kBlue);
  gfit->Draw("same");

  //gauss.plotOn(frame,LineColor(4));//this will show fit overlay on canvas 
  //gauss.paramOn(frame); //this will display the fit parameters on canvas
  dh.plotOn(frame,MarkerColor(4),MarkerSize(1),MarkerStyle(20));  //this will show histogram data points on canvas 
  //frame->Draw();
  //h1_invariant_mass_comb->Draw("same");
  frame->Draw("same");
  auto legend = new TLegend(0.2,0.79,0.4,0.89);
  legend->SetTextSize(0.04);
  legend->AddEntry(h1_invariant_mass,"All e^{+}e^{-} pairs","l");
  legend->AddEntry(gr,"Bin points","p");
  legend->AddEntry(gfit,"Fitted Gaussian","l");
  //legend->AddEntry(h1_invariant_mass_comb,"Combinatorial Background","f");
  //legend->Draw("same");
  // Luminosity Text
  auto th1 = new TLatex(3.14,130000,"Luminosity 10 fb^{-1}");
  th1->SetTextSize(0.04);
  th1->Draw("same");

  auto th3 = new TLatex(3.14,150000,"18x275 e+p");
  th3->SetTextSize(0.04);
  th3->Draw("same");
  

  auto th4 = new TLatex(3.14,140000,"DVMP Sartre");
  th4->SetTextSize(0.04);
  th4->Draw("same");
  
  auto th5 = new TLatex(3.14,160000,"#bf{#it{sPHENIX}} Simulation");
  th5->SetTextSize(0.04);
  th5->Draw("same");


  
  c_nils->SaveAs("dvmp_gaussian.eps");


  return 0;
}

Double_t mybw(Double_t* x, Double_t* par)
{
  Double_t arg1 = 14.0/22.0; // 2 over pi
  Double_t arg2 = par[1]*par[1]*par[2]*par[2]; //Gamma=par[1]  M=par[2]
  Double_t arg3 = ((x[0]*x[0]) - (par[2]*par[2]))*((x[0]*x[0]) - (par[2]*par[2]));
  Double_t arg4 = x[0]*x[0]*x[0]*x[0]*((par[1]*par[1])/(par[2]*par[2]));
  return par[0]*arg1*arg2/(arg3 + arg4);
}
