#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TLegend.h>
#include <TPaveText.h>
#include <TLine.h>
#include <TGraphAsymmErrors.h>
#include <RooRealVar.h>
#include <RooGaussian.h>
#include <RooChebychev.h>
#include <RooPolynomial.h>
#include <RooAddPdf.h>
#include <RooDataSet.h>
#include <RooFitResult.h>
#include <RooPlot.h>
#include <RooArgList.h>
#include <TMath.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>

using namespace RooFit;

std::string getDate()
{
    std::time_t t = std::time(0);   // get time now
    std::tm* now = std::localtime(&t);

    std::stringstream date;
    date << (now->tm_mon + 1) << '/'
         <<  now->tm_mday << '/'
         << (now->tm_year + 1900);
    return date.str();
}

template <typename T>
string to_string_with_precision(const T a_value, const int n = 2)
{
    ostringstream out;
    out.precision(n);
    out << fixed << a_value;
    return out.str();
}

std::pair<double,double> GetKS0Yield(TTree* inTree, bool savePlots, std::string binType, int binNumber)
{
  RooRealVar mass("K_S0_mass", "mass", 0.45, 0.55);
  RooDataSet dataSet("K_S0_mass", "data", mass, Import(*inTree));

  // DCB Signal
  /*
  RooRealVar mean_KS0("KS0_mean", "KS0_mean", 0.494, 0.48, 0.50);
  RooRealVar sigma_KS0("sigma_KS0", "sigma_KS0", 0.0032, 1e-3, 0.015);
  RooRealVar aLow("aL", "aLow", 1.6135); aLow.setConstant(true);
  RooRealVar nLow("nL", "nLow", 4.9563); nLow.setConstant(true);
  RooRealVar aHigh("aH", "aHigh", 1.3797); aHigh.setConstant(true);
  RooRealVar nHigh("nH", "nHigh", 2.9996); nHigh.setConstant(true);
  RooRealVar frac("fDCBcore", "fDCBcore", 0.80260); frac.setConstant(true);
  RooDoubleCB signal_KS0("signal_KS0", "signal_KS0", mass, mean_KS0, sigma_KS0, aLow, nLow, aHigh, nHigh, frac);
  RooRealVar fSig_KS0("fSig_KS0", "fSig_KS0", 0.5*dataSet.numEntries(), 0., 1.2*dataSet.numEntries()); 
  */
  // Triple Gaussian Signal

  RooRealVar  mean1_KS0("KS0_mean1", "KS0_mean1", 0.49, 0.48, 0.50);
  RooRealVar  sigma1_KS0("sigma1_KS0", "sigma1_KS0", 0.01, 0.0005, 0.5);
  RooGaussian gaus1("gaus1", "gaus1", mass, mean1_KS0, sigma1_KS0);
  RooRealVar  mean2_KS0("KS0_mean2", "KS0_mean2", 0.49, 0.48, 0.50);
  RooRealVar  sigma2_KS0("sigma2_KS0", "sigma2_KS0", 0.01, 0.0005, 0.5);
  RooGaussian gaus2("gaus2", "gaus2", mass, mean2_KS0, sigma2_KS0);
  RooRealVar  mean3_KS0("KS0_mean3", "KS0_mean3", 0.49, 0.48, 0.50);
  RooRealVar  sigma3_KS0("sigma3_KS0", "sigma3_KS0", 0.01, 0.0005, 0.5);
  RooGaussian gaus3("gaus3", "gaus3", mass, mean3_KS0, sigma3_KS0);
  RooRealVar f1("f1","f1",0.6,0.0,1.0);
  RooRealVar f2("f2","f2",0.3,0.0,1.0);
  RooAddPdf signal_KS0("signal_KS0","signal_KS0",RooArgList(gaus1,gaus2,gaus3),RooArgList(f1,f2));
  RooRealVar  fSig_KS0("fSig_KS0", "fSig_KS0", 0.9*dataSet.numEntries(), 0.5*dataSet.numEntries(), 1.2*dataSet.numEntries());
  

  // Linear Background
  RooRealVar p1("p1","coeff #1", 0., -100., 100.);
  RooPolynomial background("background","background", mass, RooArgList(p1));

  // Quadratic Chebyshev Polynomial Background
  //RooRealVar c1("c1", "c1", 0.0, -1.0, 1.0);  
  //RooRealVar c2("c2", "c2", 0.0, -1.0, 1.0);
  //RooChebychev background("background","background", mass, RooArgList(c1, c2));  

  RooRealVar fBkg("fBkg", "fBkg", 0.5*dataSet.numEntries(), 0., dataSet.numEntries());

  RooArgList fitModelList(signal_KS0, background), fitFracList(fSig_KS0, fBkg);
  RooAddPdf model("model", "model", fitModelList, fitFracList);
  RooFitResult* r  = model.fitTo(dataSet, Save(true), Extended(true), PrintLevel(-1));
  r->Print();

  double fitYield = fSig_KS0.getValV();
  double fitYieldErr = fSig_KS0.getError();

  if (savePlots)
  {
    RooPlot* frame = mass.frame(Title(""));       //creating the frame

    RooBinning bins(0.45, 0.55);
    int nBins = 40;
    bins.addUniform(nBins, 0.45, 0.55);
   
    dataSet.plotOn(frame, Binning(bins), XErrorSize(0), DataError(RooAbsData::SumW2));
    model.plotOn(frame, Components(background), LineColor(kGray), DrawOption("F"), FillColor(kGray));
    model.plotOn(frame, Components(RooArgSet(signal_KS0, background)), LineColor(kAzure+8), DrawOption("F"), FillColor(kAzure+8), MoveToBack());
    model.plotOn(frame, LineColor(kBlack));        //plotting the fit onto the frame
    dataSet.plotOn(frame, DrawOption("PE1"), Binning(bins),XErrorSize(0), DataError(RooAbsData::SumW2));
    RooHist* pull = frame->pullHist();
    RooPlot* frame2 = mass.frame(Title(""));
    frame2->addPlotable(pull,"PE1");

    TCanvas* c = new TCanvas("massFitCanvas", "massFitCanvas",800, 800);

    TPad mainPad("mainPad", "mainPad", 0., 0.3, 1., 1.);
    mainPad.SetBottomMargin(0.0);

    mainPad.Draw();

    TPad pullPad("pullPad", "pullPad", 0., 0.0, 1., 0.3);
    pullPad.SetBottomMargin(0.5);
    pullPad.SetTopMargin(0);

    pullPad.Draw();

    mainPad.cd();

    frame->SetMarkerStyle(kCircle);
    frame->SetMarkerSize(0.02);
    frame->SetLineWidth(1);
    frame->GetXaxis()->SetNdivisions(505);
    frame->GetXaxis()->SetTitle("m(#pi^{+}#pi^{-}) [GeV/c^{2}]");
    frame->GetXaxis()->SetTitleOffset(0.9);
    frame->GetXaxis()->SetTitleFont(42);
    frame->GetXaxis()->SetLabelFont(42);
    frame->GetYaxis()->SetTitleFont(42);
    frame->GetYaxis()->SetLabelFont(42);
    double maxVal = frame->GetMaximum();
    frame->GetYaxis()->SetRangeUser(1.0, maxVal*1.5);
    float binWidth = 1000.*(0.55 - 0.45) / nBins;
    string yAxisTitle = "Candidates / (" + to_string_with_precision(binWidth, 0) + " MeV)";
    frame->GetYaxis()->SetTitle(yAxisTitle.c_str());
    frame->Draw();

    //string fitWidthVal_KS0 = to_string_with_precision(1e3 * sigma_KS0.getValV(), 1);
    //string fitWidthErr_KS0 = to_string_with_precision(1e3 * sigma_KS0.getError(), 1);
    //string fitWidth_KS0 = "#sigma(K_{S}^{0}) = " + fitWidthVal_KS0 + "#pm " + fitWidthErr_KS0 + " MeV";

    TPaveText *pt;
    pt = new TPaveText(0.52,0.59,0.90,0.90, "NDC"); //For 4 GeV
    pt->SetFillColor(0);
    pt->SetFillStyle(0);
    pt->SetTextFont(42);
    TText *pt_LaTex = pt->AddText("#it{#bf{sPHENIX}} Internal");
    pt_LaTex = pt->AddText("#it{p}+#it{p} #sqrt{s} = 200 GeV");
    //pt_LaTex = pt->AddText(fitWidth_KS0.c_str());
    pt->SetBorderSize(0);
    pt->DrawClone();

    TPaveText *ptDate;
    ptDate = new TPaveText(0.67,0.92,1.05,1.00, "NDC");
    ptDate->SetFillColor(0);
    ptDate->SetFillStyle(0);
    ptDate->SetTextFont(42);
    std::string compilation_date = getDate();
    TText *pt_LaTexDate = ptDate->AddText(compilation_date.c_str());
    ptDate->SetBorderSize(0);
    ptDate->Draw();
    gPad->Modified();

    gPad->SetTopMargin(0.08);

    frame->Print();
    TLegend *legend = new TLegend(0.19,0.70,0.52,0.90);
    legend->AddEntry(frame->findObject("h_K_S0_mass"),"Data","PE2");
    legend->AddEntry(frame->findObject("model_Norm[K_S0_mass]"),"Fit","L");
    legend->AddEntry(frame->findObject("model_Norm[K_S0_mass]_Comp[signal_KS0,background]"),"K_{S}^{0}#rightarrow#pi^{+}#pi^{-}","f");
    legend->AddEntry(frame->findObject("model_Norm[K_S0_mass]_Comp[background]"),"Comb. Bkg.","f");
    legend->SetFillColor(0);
    legend->SetFillStyle(0);
    legend->SetBorderSize(0);
    legend->SetTextSize(0.05);
    legend->Draw();

    gPad->Modified();
    pullPad.cd();

    frame2->SetMarkerStyle(kCircle);
    frame2->SetMarkerSize(0.02);
    frame2->SetTitle("");
    frame2->GetXaxis()->SetNdivisions(505);
    frame2->GetXaxis()->SetTitle("m(#pi^{+}#pi^{-}) [GeV/c^{2}]");
    frame2->GetXaxis()->SetTitleOffset(0.9);
    frame2->GetXaxis()->SetTitleFont(42);
    frame2->GetXaxis()->SetTitleSize(0.12);
    frame2->GetXaxis()->SetLabelFont(42);
    frame2->GetXaxis()->SetLabelSize(0.12);
    frame2->GetYaxis()->SetTitle("Pull");
    frame2->GetYaxis()->SetTitleOffset(0.55);
    frame2->GetYaxis()->SetTitleFont(42);
    frame2->GetYaxis()->SetTitleSize(0.12);
    frame2->GetYaxis()->SetLabelFont(42);
    frame2->GetYaxis()->SetLabelSize(0.12);
    frame2->GetYaxis()->SetRangeUser(-6, 6);
    frame2->GetYaxis()->SetNdivisions(5);
    TF1 *plusThreeLine = new TF1("plusThreeLine",  "pol1", 0.45, 0.55);
    plusThreeLine->SetParameters(3, 0);
    plusThreeLine->SetLineColor(1);
    plusThreeLine->SetLineStyle(2);
    plusThreeLine->SetLineWidth(2);
    TF1 *zeroLine = new TF1("zeroLine",  "pol1",0.45, 0.55);
    zeroLine->SetParameters(0, 0);
    zeroLine->SetLineColor(1);
    zeroLine->SetLineStyle(2);
    zeroLine->SetLineWidth(2);

    TF1 *minusThreeLine = new TF1("minusThreeLine", "pol1", 0.45, 0.55);
    minusThreeLine->SetParameters(-3, 0);
    minusThreeLine->SetLineColor(1);
    minusThreeLine->SetLineStyle(2);
    minusThreeLine->SetLineWidth(2);
    frame2->Draw();
    plusThreeLine->Draw("same");
    zeroLine->Draw("same");
    minusThreeLine->Draw("same");

    string outputFile = "./yield_fits/KS0_yield_" + binType + "_bin_" + std::to_string(binNumber) + ".png";
    c->SaveAs(outputFile.c_str());
  }

  return {fitYield, fitYieldErr};
}

std::pair<double,double> GetXiYield(TTree* inTree, bool savePlots, std::string binType, int binNumber)
{
  //RooRealVar mass("Ximinus_mass", "mass", 1.3, 1.34);
  //RooDataSet dataSet("Ximinus_mass", "data", mass, Import(*inTree));

  // DCB Signal
  /*
  RooRealVar mean_Xi("Xi_mean", "Xi_mean", 1.32, 1.31, 1.33);
  RooRealVar sigma_Xi("sigma_Xi", "sigma_Xi", 0.002, 1e-3, 0.005);
  RooRealVar aLow("aL", "aLow", 1.882); aLow.setConstant(true);
  RooRealVar nLow("nL", "nLow", 1.5159); nLow.setConstant(true);
  RooRealVar aHigh("aH", "aHigh", 1.4578); aHigh.setConstant(true);
  RooRealVar nHigh("nH", "nHigh", 2.5526); nHigh.setConstant(true);
  RooRealVar frac("fDCBcore", "fDCBcore", 0.53753); frac.setConstant(true);
  RooDoubleCB signal_Xi("signal_Xi", "signal_Xi", mass, mean_Xi, sigma_Xi, aLow, nLow, aHigh, nHigh, frac);
  RooRealVar fSig_Xi("fSig_Xi", "fSig_Xi", 0.5*dataSet.numEntries(), 0., 1.2*dataSet.numEntries()); 
  */

  // Triple Gaussian signal
  /*
  RooRealVar  mean1_Xi("Xi_mean1", "Xi_mean1", 1.32, 1.315, 1.325);
  RooRealVar  sigma1_Xi("sigma1_Xi", "sigma1_Xi", 0.001, 0.0005, 0.002);
  RooGaussian gaus1("gaus1", "gaus1", mass, mean1_Xi, sigma1_Xi);
  //RooRealVar  mean2_Xi("Xi_mean2", "Xi_mean2", 1.32, 1.315, 1.325);
  RooRealVar  sigma2_Xi("sigma2_Xi", "sigma2_Xi", 0.002, 0.001, 0.003);
  RooGaussian gaus2("gaus2", "gaus2", mass, mean1_Xi, sigma2_Xi);
  //RooRealVar  mean3_Xi("Xi_mean3", "Xi_mean3", 1.32, 1.315, 1.325);
  //RooRealVar  sigma3_Xi("sigma3_Xi", "sigma3_Xi", 0.007, 0.005, 0.01);
  //RooGaussian gaus3("gaus3", "gaus3", mass, mean1_Xi, sigma3_Xi);
  RooRealVar f1("f1","f1",0.6,0.0,1.0);
  RooRealVar f2("f2","f2",0.3,0.0,1.0);
  //RooAddPdf signal_Xi("signal_Xi","signal_Xi",RooArgList(gaus1,gaus2,gaus3),RooArgList(f1,f2));
  RooAddPdf signal_Xi("signal_Xi","signal_Xi",RooArgList(gaus1,gaus2),RooArgList(f1,f2));
  RooRealVar  fSig_Xi("fSig_Xi", "fSig_Xi", 0.9*dataSet.numEntries(), 0.5*dataSet.numEntries(), 1.2*dataSet.numEntries());
  */

  /*
  // Linear Background
  RooRealVar p1("p1","coeff #1", -10, -1000., 0.);
  RooPolynomial background("background","background", mass, RooArgList(p1));
  
  // Quadratic Chebyshev Polynomial Background
  //RooRealVar c1("c1", "c1", 0.0, -1.0, 1.0);  
  //RooRealVar c2("c2", "c2", 0.0, -1.0, 1.0);
  //RooChebychev background("background","background", mass, RooArgList(c1, c2));  
  
  RooRealVar fBkg("fBkg", "fBkg", 0.5*dataSet.numEntries(), 0., dataSet.numEntries());

  RooArgList fitModelList(signal_Xi, background), fitFracList(fSig_Xi, fBkg);
  RooAddPdf model("model", "model", fitModelList, fitFracList);
  RooFitResult* r  = model.fitTo(dataSet, Save(true), Extended(true), PrintLevel(-1));
  r->Print();

  double fitYield = fSig_Xi.getValV();
  double fitYieldErr = fSig_Xi.getError();
  */

  TH1F* h_Xi_mass = new TH1F("h_Xi_mass", ";m(#Lambda^{0}#pi^{-}) [GeV/c^{2}];Candidates / (1 MeV)", 40, 1.3, 1.34);
  inTree->Draw("Ximinus_mass>>h_Xi_mass", "", "goff");

  TF1* fitFunc = new TF1("gausPlusLinear","[0]*exp(-0.5*((x-[1])/[2])^2) + [3] + [4]*x",1.3, 1.34);
  fitFunc->SetLineColor(kRed);
  fitFunc->SetParName(0, "Const"); fitFunc->SetParameter(0, 100); fitFunc->SetParLimits(0, 0, 1e4);
  fitFunc->SetParName(1, "Mean"); fitFunc->SetParameter(1, 1.32); fitFunc->SetParLimits(1, 1.31, 1.33);
  fitFunc->SetParName(2, "Width"); fitFunc->SetParameter(2, 0.002); fitFunc->SetParLimits(2, 0, 0.01);
  fitFunc->SetParName(3, "Intercept"); fitFunc->SetParameter(3, 100.); fitFunc->SetParLimits(3, 0., 1e4);
  fitFunc->SetParName(4, "Slope");     fitFunc->SetParameter(4, -10.); fitFunc->SetParLimits(4, -1e4, 0.);

  double binWidth = h_Xi_mass->GetBinWidth(1);

  TCanvas *c1  = new TCanvas("myCanvas", "myCanvas",800,800);

  h_Xi_mass->Draw("PE1");
  h_Xi_mass->Fit(fitFunc, "ER");

  double fitMean = fitFunc->GetParameter(1);
  double fitMeanErr = fitFunc->GetParError(1);
  double fitWidth = fitFunc->GetParameter(2);
  double fitWidthErr = fitFunc->GetParError(2);

  std::cout << "Mean : " << fitMean << "+/-" << fitMeanErr << std::endl;
  std::cout << "Width : " << fitWidth << "+/-" << fitWidthErr << std::endl;

  double A     = fitFunc->GetParameter(0);
  double sigma = fitFunc->GetParameter(2);
  double Aerr  = fitFunc->GetParError(0);
  double sigerr = fitFunc->GetParError(2);

  double fitYield    = (A * sigma * sqrt(2 * TMath::Pi()))/binWidth;
  double fitYieldErr = sqrt(2 * TMath::Pi()) / binWidth
                  * sqrt(pow(sigma * Aerr, 2) + pow(A * sigerr, 2));

  string outputFile = "./yield_fits/Xi_yield_" + binType + "_bin_" + std::to_string(binNumber) + ".png";
  c1->SaveAs(outputFile.c_str());

  if (savePlots)
  {
    /*
    RooPlot* frame = mass.frame(Title(""));       //creating the frame

    RooBinning bins(1.3, 1.34);
    int nBins = 40;
    bins.addUniform(nBins, 1.3, 1.34);
   
    dataSet.plotOn(frame, Binning(bins), XErrorSize(0), DataError(RooAbsData::SumW2));
    model.plotOn(frame, Components(background), LineColor(kGray), DrawOption("F"), FillColor(kGray));
    model.plotOn(frame, Components(RooArgSet(signal_Xi, background)), LineColor(kAzure+8), DrawOption("F"), FillColor(kAzure+8), MoveToBack());
    model.plotOn(frame, LineColor(kBlack));        //plotting the fit onto the frame
    dataSet.plotOn(frame, DrawOption("PE1"), Binning(bins),XErrorSize(0), DataError(RooAbsData::SumW2));
    RooHist* pull = frame->pullHist();
    RooPlot* frame2 = mass.frame(Title(""));
    frame2->addPlotable(pull,"PE1");

    TCanvas* c = new TCanvas("massFitCanvas", "massFitCanvas",800, 800);

    TPad mainPad("mainPad", "mainPad", 0., 0.3, 1., 1.);
    mainPad.SetBottomMargin(0.0);

    mainPad.Draw();

    TPad pullPad("pullPad", "pullPad", 0., 0.0, 1., 0.3);
    pullPad.SetBottomMargin(0.5);
    pullPad.SetTopMargin(0);

    pullPad.Draw();

    mainPad.cd();

    frame->SetMarkerStyle(kCircle);
    frame->SetMarkerSize(0.02);
    frame->SetLineWidth(1);
    frame->GetXaxis()->SetNdivisions(505);
    frame->GetXaxis()->SetTitle("m(#Lambda^{0}#pi^{-}) [GeV/c^{2}]");
    frame->GetXaxis()->SetTitleOffset(0.9);
    frame->GetXaxis()->SetTitleFont(42);
    frame->GetXaxis()->SetLabelFont(42);
    frame->GetYaxis()->SetTitleFont(42);
    frame->GetYaxis()->SetLabelFont(42);
    double maxVal = frame->GetMaximum();
    frame->GetYaxis()->SetRangeUser(1.0, maxVal*1.5);
    float binWidth = 1000.*(1.34 - 1.30) / nBins;
    string yAxisTitle = "Candidates / (" + to_string_with_precision(binWidth, 0) + " MeV)";
    frame->GetYaxis()->SetTitle(yAxisTitle.c_str());
    frame->Draw();

    //string fitWidthVal_Xi = to_string_with_precision(1e3 * sigma_Xi.getValV(), 1);
    //string fitWidthErr_Xi = to_string_with_precision(1e3 * sigma_Xi.getError(), 1);
    //string fitWidth_Xi = "#sigma(#Xi^{-}) = " + fitWidthVal_Xi + "#pm " + fitWidthErr_Xi + " MeV";

    TPaveText *pt;
    pt = new TPaveText(0.52,0.59,0.90,0.90, "NDC"); //For 4 GeV
    pt->SetFillColor(0);
    pt->SetFillStyle(0);
    pt->SetTextFont(42);
    TText *pt_LaTex = pt->AddText("#it{#bf{sPHENIX}} Internal");
    pt_LaTex = pt->AddText("#it{p}+#it{p} #sqrt{s} = 200 GeV");
    //pt_LaTex = pt->AddText(fitWidth_Xi.c_str());
    pt->SetBorderSize(0);
    pt->DrawClone();

    TPaveText *ptDate;
    ptDate = new TPaveText(0.67,0.92,1.05,1.00, "NDC");
    ptDate->SetFillColor(0);
    ptDate->SetFillStyle(0);
    ptDate->SetTextFont(42);
    std::string compilation_date = getDate();
    TText *pt_LaTexDate = ptDate->AddText(compilation_date.c_str());
    ptDate->SetBorderSize(0);
    ptDate->Draw();
    gPad->Modified();

    gPad->SetTopMargin(0.08);

    frame->Print();
    TLegend *legend = new TLegend(0.19,0.70,0.52,0.90);
    legend->AddEntry(frame->findObject("h_Ximinus_mass"),"Data","PE2");
    legend->AddEntry(frame->findObject("model_Norm[Ximinus_mass]"),"Fit","L");
    legend->AddEntry(frame->findObject("model_Norm[Ximinus_mass]_Comp[signal_Xi,background]"),"#Xi^{-}#rightarrow#Lambda^{0}#pi^{-}","f");
    legend->AddEntry(frame->findObject("model_Norm[Ximinus_mass]_Comp[background]"),"Comb. Bkg.","f");
    legend->SetFillColor(0);
    legend->SetFillStyle(0);
    legend->SetBorderSize(0);
    legend->SetTextSize(0.05);
    legend->Draw();

    gPad->Modified();
    pullPad.cd();

    frame2->SetMarkerStyle(kCircle);
    frame2->SetMarkerSize(0.02);
    frame2->SetTitle("");
    frame2->GetXaxis()->SetNdivisions(505);
    frame2->GetXaxis()->SetTitle("m(#Lambda^{0}#pi^{-}) [GeV/c^{2}]");
    frame2->GetXaxis()->SetTitleOffset(0.9);
    frame2->GetXaxis()->SetTitleFont(42);
    frame2->GetXaxis()->SetTitleSize(0.12);
    frame2->GetXaxis()->SetLabelFont(42);
    frame2->GetXaxis()->SetLabelSize(0.12);
    frame2->GetYaxis()->SetTitle("Pull");
    frame2->GetYaxis()->SetTitleOffset(0.55);
    frame2->GetYaxis()->SetTitleFont(42);
    frame2->GetYaxis()->SetTitleSize(0.12);
    frame2->GetYaxis()->SetLabelFont(42);
    frame2->GetYaxis()->SetLabelSize(0.12);
    frame2->GetYaxis()->SetRangeUser(-6, 6);
    frame2->GetYaxis()->SetNdivisions(5);
    TF1 *plusThreeLine = new TF1("plusThreeLine",  "pol1", 1.3, 1.34);
    plusThreeLine->SetParameters(3, 0);
    plusThreeLine->SetLineColor(1);
    plusThreeLine->SetLineStyle(2);
    plusThreeLine->SetLineWidth(2);
    TF1 *zeroLine = new TF1("zeroLine",  "pol1",1.3, 1.34);
    zeroLine->SetParameters(0, 0);
    zeroLine->SetLineColor(1);
    zeroLine->SetLineStyle(2);
    zeroLine->SetLineWidth(2);

    TF1 *minusThreeLine = new TF1("minusThreeLine", "pol1", 1.3, 1.34);
    minusThreeLine->SetParameters(-3, 0);
    minusThreeLine->SetLineColor(1);
    minusThreeLine->SetLineStyle(2);
    minusThreeLine->SetLineWidth(2);
    frame2->Draw();
    plusThreeLine->Draw("same");
    zeroLine->Draw("same");
    minusThreeLine->Draw("same");

    string outputFile = "./yield_fits/Xi_yield_" + binType + "_bin_" + std::to_string(binNumber) + ".png";
    c->SaveAs(outputFile.c_str());
    */
  }

  return {fitYield, fitYieldErr};
}

void XiKS0_SingleBin_Calculator(std::string binType = "pT", int binNumber = 0, bool saveYieldPlots = false)
{
  TFile* KS0DataFile = new TFile("/sphenix/user/rosstom/TrackingAnalysis/Physics_Val_TF/data/analysisMacros/KShort_fullDataset_finalCuts_0p2pTCut_rapidity1p0Cut_BCOcut_.root");
  TTree* KS0DataTree = (TTree*)KS0DataFile->Get("DecayTree");
  
  TFile* XiDataFile = new TFile("/sphenix/user/rosstom/TrackingAnalysis/Physics_Val_TF/data/analysisMacros/Ximinus_fullDataset_finalCuts_0p2pTCut_rapidity1p0Cut_BCOcut_.root");
  TTree* XiDataTree = (TTree*)XiDataFile->Get("DecayTree");

  const int nBins_pT = 4;
  double pTbins[nBins_pT + 1] = {0.75, 1.07, 1.25, 1.49, 2.0};
  const int nBins_phi = 6;
  //double phibins[nBins_phi + 1] = {-M_PI,-2.14,-1.37,-0.61,0.35,2.21,M_PI};
  double phibins[nBins_phi + 1] = {-M_PI,-2.09439,-1.04719,0.0,1.04721,2.09441,M_PI};
  const int nBins_eta = 6;
  //double etabins[nBins_eta + 1] = {-1.1,-0.82,-0.56,-0.20,0.30,0.72,1.1};
  double etabins[nBins_eta + 1] = {-1.1,-0.733333,-0.366666,0.0,0.366668,0.733335,1.1};
  const int nBins_rap = 6;
  //double rapbins[nBins_rap + 1] = {-1.0,-0.64,-0.41,-0.15,0.21,0.55,1.0};
  double rapbins[nBins_rap + 1] = {-1.0,-0.66,-0.33,0.0,0.33,0.66,1.0};

  double KS02pipi_BR_PDG = 0.6920;
  double KS02pipi_BR_uncert_PDG = 0.0005;
  double Xi2Lambdapi_BR_PDG = 0.99887;
  double Xi2Lambdapi_BR_uncert_PDG = 0.00035;
  double Lambda2ppi_BR_PDG = 0.641;
  double Lambda2ppi_BR_uncert_PDG = 0.005;

  std::string geomAccept_dir = "/sphenix/user/rosstom/analysis/LightFlavorRatios/geometric_acceptance/analysis/plots_evaluator/";
  std::string cutEff_dir = "/sphenix/user/rosstom/TrackingAnalysis/Physics_Val_TF/sim/pTComp/";
  std::string trackAccept_dir = "/sphenix/user/rosstom/analysis/LightFlavorRatios/bachelor_pi_efficiency/analysis/";

  std::string tempFileString;

  double ratioValue;
  double ratioUncertainty;
  if (binType == "pT")
  {
    if (binNumber >= nBins_pT)
    {
      std::cout << "ERROR: Bin value out of range for this bin type" << std::endl;
      return;
    }
    std::string binCut_KS0;
    std::string binCut_Xi;
    if (binNumber == nBins_pT - 1)
    {
      binCut_KS0 = Form("K_S0_pT >= %f && K_S0_pT <= %f", pTbins[binNumber],pTbins[binNumber+1]);
      binCut_Xi = Form("Ximinus_pT >= %f && Ximinus_pT <= %f && Lambda0_track_1_charge == track_3_charge", pTbins[binNumber],pTbins[binNumber+1]);
    }
    else
    {
      binCut_KS0 = Form("K_S0_pT >= %f && K_S0_pT < %f", pTbins[binNumber],pTbins[binNumber+1]);
      binCut_Xi = Form("Ximinus_pT >= %f && Ximinus_pT < %f && Lambda0_track_1_charge == track_3_charge", pTbins[binNumber],pTbins[binNumber+1]);
    }

    tempFileString = "temp_cut_trees_trash_" + binType + "_" + std::to_string(binNumber) + ".root";
    TFile* tempFile = new TFile(tempFileString.c_str(), "RECREATE"); 
    TTree* KS0DataTree_cut = KS0DataTree->CopyTree(binCut_KS0.c_str());
    TTree* XiDataTree_cut = XiDataTree->CopyTree(binCut_Xi.c_str());

    std::cout << "Extracting KS0 yield" << std::endl;
    std::pair<double,double> KS0_yield_uncert = GetKS0Yield(KS0DataTree_cut, saveYieldPlots, binType, binNumber);
    std::cout << "KS0 yield = " << KS0_yield_uncert.first << " +/- " << KS0_yield_uncert.second << std::endl;
    std::cout << "Extracting Xi yield" << std::endl;
    std::pair<double,double> Xi_yield_uncert = GetXiYield(XiDataTree_cut, saveYieldPlots, binType, binNumber);
    std::cout << "Xi yield = " << Xi_yield_uncert.first << " +/- " << Xi_yield_uncert.second << std::endl;

    tempFile->Close();

    // raw ratio value
    ratioValue = Xi_yield_uncert.first/(2*KS0_yield_uncert.first);
    ratioUncertainty = std::sqrt(std::pow((Xi_yield_uncert.second/(2*KS0_yield_uncert.first)),2) + 
                                 std::pow(((KS0_yield_uncert.second*Xi_yield_uncert.first)/(2*std::pow(KS0_yield_uncert.first,2))),2));
    std::cout << "Raw ratio value : " << ratioValue << "+/-" << ratioUncertainty << std::endl;
   
    std::string outfile_name = "Xi_KS0_ratio_TriGauss_Linear_" + binType + "_bin_" + std::to_string(binNumber)  + ".txt";
    std::ofstream outFile(outfile_name.c_str());
    outFile << KS0_yield_uncert.first << "\n";
    outFile << KS0_yield_uncert.second << "\n";
    outFile << Xi_yield_uncert.first << "\n";
    outFile << Xi_yield_uncert.second << "\n";
    outFile.close();

    //correct for geometric acceptance and efficiency
    std::string geoAccFile = geomAccept_dir + "Ximinus_to_KS0_geometric_acceptance_ratio_onlyPrimaries_pT.root";
    TFile* f_geoAccFile = TFile::Open(geoAccFile.c_str(), "READ");
    TCanvas* c_geoAccFile = (TCanvas*)f_geoAccFile->Get("myCanvas");
    TH1F* h_geoAccFile = (TH1F*)c_geoAccFile->GetPrimitive("Ximinus_inGeo_pT");
    double binContent = h_geoAccFile->GetBinContent(binNumber+1);
    double binError   = h_geoAccFile->GetBinError(binNumber+1); 
    ratioValue = ratioValue/binContent;
    ratioUncertainty = ratioUncertainty/binContent;
    std::cout << "Ratio with geometric acceptance correction : " << ratioValue << "+/-" << ratioUncertainty << std::endl;
    f_geoAccFile->Close();

    //correct for cut efficiency
    std::string cutEffFile = cutEff_dir + "XiKS0ratio_pT_cutEff.root";
    TFile* f_cutEffFile = TFile::Open(cutEffFile.c_str(), "READ");
    TCanvas* c_cutEffFile = (TCanvas*)f_cutEffFile->Get("myCanvas");
    TGraphAsymmErrors* g_cutEffFile = (TGraphAsymmErrors*)c_cutEffFile->GetPrimitive("Graph");
    double binCenter;
    g_cutEffFile->GetPoint(binNumber, binCenter, binContent);
    binError   = g_cutEffFile->GetErrorYhigh(binNumber);
    ratioValue = ratioValue/binContent;
    ratioUncertainty = ratioUncertainty/binContent;
    std::cout << "Ratio with cut efficiency correction : " << ratioValue << "+/-" << ratioUncertainty << std::endl;
    f_cutEffFile->Close();

    //correct for tracking acceptance and efficiency 
    std::string trackAccFile = trackAccept_dir + "trackingAcceptanceEfficiencyCorrections_withSystematics.root";
    TFile* f_trackAccFile = TFile::Open(trackAccFile.c_str(), "READ");
    TH1F* h_trackAccFile = (TH1F*)f_trackAccFile->Get("h_pT_corrections");
    binContent = h_trackAccFile->GetBinContent(binNumber+1);
    binError   = h_trackAccFile->GetBinError(binNumber+1);
    ratioValue = ratioValue/binContent;
    ratioUncertainty = ratioUncertainty/binContent;
    std::cout << "Ratio with tracking acceptance and efficiency correction : " << ratioValue << "+/-" << ratioUncertainty << std::endl;
    f_trackAccFile->Close();

    //correct for branching ratios
    ratioValue = ratioValue*(KS02pipi_BR_PDG/(Xi2Lambdapi_BR_PDG*Lambda2ppi_BR_PDG));
    ratioUncertainty = ratioUncertainty*(KS02pipi_BR_PDG/(Xi2Lambdapi_BR_PDG*Lambda2ppi_BR_PDG));
    std::cout << "Ratio with branching ratio correction : " << ratioValue << "+/-" << ratioUncertainty << std::endl;
  }
  else if (binType == "eta")
  {
    if (binNumber >= nBins_eta)
    {
      std::cout << "ERROR: Bin value out of range for this bin type" << std::endl;
      return;
    }
    std::string binCut_KS0;
    std::string binCut_Xi;
    if (binNumber == nBins_eta - 1)
    {
      binCut_KS0 = Form("K_S0_pseudorapidity >= %f && K_S0_pseudorapidity <= %f", etabins[binNumber],etabins[binNumber+1]);
      binCut_Xi = Form("Ximinus_pseudorapidity >= %f && Ximinus_pseudorapidity <= %f && Lambda0_track_1_charge == track_3_charge", etabins[binNumber],etabins[binNumber+1]);
    }
    else
    {
      binCut_KS0 = Form("K_S0_pseudorapidity >= %f && K_S0_pseudorapidity < %f", etabins[binNumber],etabins[binNumber+1]);
      binCut_Xi = Form("Ximinus_pseudorapidity >= %f && Ximinus_pseudorapidity < %f && Lambda0_track_1_charge == track_3_charge", etabins[binNumber],etabins[binNumber+1]);
    }
 
    tempFileString = "temp_cut_trees_trash_" + binType + "_" + std::to_string(binNumber) + ".root";
    TFile* tempFile = new TFile(tempFileString.c_str(), "RECREATE");
    TTree* KS0DataTree_cut = KS0DataTree->CopyTree(binCut_KS0.c_str());
    TTree* XiDataTree_cut = XiDataTree->CopyTree(binCut_Xi.c_str());

    std::cout << "Extracting KS0 yield" << std::endl;
    std::pair<double,double> KS0_yield_uncert = GetKS0Yield(KS0DataTree_cut, saveYieldPlots, binType, binNumber);
    std::cout << "KS0 yield = " << KS0_yield_uncert.first << " +/- " << KS0_yield_uncert.second << std::endl;
    std::cout << "Extracting Xi yield" << std::endl;
    std::pair<double,double> Xi_yield_uncert = GetXiYield(XiDataTree_cut, saveYieldPlots, binType, binNumber);
    std::cout << "Xi yield = " << Xi_yield_uncert.first << " +/- " << Xi_yield_uncert.second << std::endl;

    tempFile->Close();

    // raw ratio value
    ratioValue = Xi_yield_uncert.first/(2*KS0_yield_uncert.first);
    ratioUncertainty = std::sqrt(std::pow((Xi_yield_uncert.second/(2*KS0_yield_uncert.first)),2) + 
                                 std::pow(((KS0_yield_uncert.second*Xi_yield_uncert.first)/(2*std::pow(KS0_yield_uncert.first,2))),2));
    std::cout << "Raw ratio value : " << ratioValue << "+/-" << ratioUncertainty << std::endl;

    std::string outfile_name = "Xi_KS0_ratio_TriGauss_Linear_" + binType + "_bin_" + std::to_string(binNumber)  + ".txt";
    std::ofstream outFile(outfile_name.c_str());
    outFile << KS0_yield_uncert.first << "\n";
    outFile << KS0_yield_uncert.second << "\n";
    outFile << Xi_yield_uncert.first << "\n";
    outFile << Xi_yield_uncert.second << "\n";
    outFile.close();

    //correct for geometric acceptance and efficiency
    std::string geoAccFile = geomAccept_dir + "Ximinus_to_KS0_geometric_acceptance_ratio_onlyPrimaries_eta.root";
    TFile* f_geoAccFile = TFile::Open(geoAccFile.c_str(), "READ");
    TCanvas* c_geoAccFile = (TCanvas*)f_geoAccFile->Get("myCanvas");
    TH1F* h_geoAccFile = (TH1F*)c_geoAccFile->GetPrimitive("Ximinus_inGeo_#eta");
    double binContent = h_geoAccFile->GetBinContent(binNumber+1);
    double binError   = h_geoAccFile->GetBinError(binNumber+1); 
    ratioValue = ratioValue/binContent;
    ratioUncertainty = ratioUncertainty/binContent;
    std::cout << "Ratio with geometric acceptance correction : " << ratioValue << "+/-" << ratioUncertainty << std::endl;
    f_geoAccFile->Close();

    //correct for cut efficiency
    std::string cutEffFile = cutEff_dir + "XiKS0ratio_eta_cutEff.root";
    TFile* f_cutEffFile = TFile::Open(cutEffFile.c_str(), "READ");
    TCanvas* c_cutEffFile = (TCanvas*)f_cutEffFile->Get("myCanvas");
    TGraphAsymmErrors* g_cutEffFile = (TGraphAsymmErrors*)c_cutEffFile->GetPrimitive("Graph");
    double binCenter;
    g_cutEffFile->GetPoint(binNumber, binCenter, binContent);
    binError   = g_cutEffFile->GetErrorYhigh(binNumber);
    ratioValue = ratioValue/binContent;
    ratioUncertainty = ratioUncertainty/binContent;
    std::cout << "Ratio with cut efficiency correction : " << ratioValue << "+/-" << ratioUncertainty << std::endl;
    f_cutEffFile->Close();

    //correct for tracking acceptance and efficiency 
    std::string trackAccFile = trackAccept_dir + "trackingAcceptanceEfficiencyCorrections_withSystematics.root";
    TFile* f_trackAccFile = TFile::Open(trackAccFile.c_str(), "READ");
    TH1F* h_trackAccFile = (TH1F*)f_trackAccFile->Get("h_eta_corrections");
    binContent = h_trackAccFile->GetBinContent(binNumber+1);
    binError   = h_trackAccFile->GetBinError(binNumber+1);
    ratioValue = ratioValue/binContent;
    ratioUncertainty = ratioUncertainty/binContent;
    std::cout << "Ratio with tracking acceptance and efficiency correction : " << ratioValue << "+/-" << ratioUncertainty << std::endl;
    f_trackAccFile->Close();

    //correct for branching ratios
    ratioValue = ratioValue*(KS02pipi_BR_PDG/(Xi2Lambdapi_BR_PDG*Lambda2ppi_BR_PDG));
    ratioUncertainty = ratioUncertainty*(KS02pipi_BR_PDG/(Xi2Lambdapi_BR_PDG*Lambda2ppi_BR_PDG));
    std::cout << "Ratio with branching ratio correction : " << ratioValue << "+/-" << ratioUncertainty << std::endl;
  }
  else if (binType == "phi")
  {
    if (binNumber >= nBins_phi)
    {
      std::cout << "ERROR: Bin value out of range for this bin type" << std::endl;
      return;
    }
    std::string binCut_KS0;
    std::string binCut_Xi;
    if (binNumber == nBins_phi - 1)
    {
      binCut_KS0 = Form("K_S0_phi >= %f && K_S0_phi <= %f", phibins[binNumber],phibins[binNumber+1]);
      binCut_Xi = Form("Ximinus_phi >= %f && Ximinus_phi <= %f && Lambda0_track_1_charge == track_3_charge", phibins[binNumber],phibins[binNumber+1]);
    }
    else
    {
      binCut_KS0 = Form("K_S0_phi >= %f && K_S0_phi < %f", phibins[binNumber],phibins[binNumber+1]);
      binCut_Xi = Form("Ximinus_phi >= %f && Ximinus_phi < %f && Lambda0_track_1_charge == track_3_charge", phibins[binNumber],phibins[binNumber+1]);
    }
 
    tempFileString = "temp_cut_trees_trash_" + binType + "_" + std::to_string(binNumber) + ".root";
    TFile* tempFile = new TFile(tempFileString.c_str(), "RECREATE");
    TTree* KS0DataTree_cut = KS0DataTree->CopyTree(binCut_KS0.c_str());
    TTree* XiDataTree_cut = XiDataTree->CopyTree(binCut_Xi.c_str());

    std::cout << "Extracting KS0 yield" << std::endl;
    std::pair<double,double> KS0_yield_uncert = GetKS0Yield(KS0DataTree_cut, saveYieldPlots, binType, binNumber);
    std::cout << "KS0 yield = " << KS0_yield_uncert.first << " +/- " << KS0_yield_uncert.second << std::endl;
    std::cout << "Extracting Xi yield" << std::endl;
    std::pair<double,double> Xi_yield_uncert = GetXiYield(XiDataTree_cut, saveYieldPlots, binType, binNumber);
    std::cout << "Xi yield = " << Xi_yield_uncert.first << " +/- " << Xi_yield_uncert.second << std::endl;

    tempFile->Close();

    // raw ratio value
    ratioValue = Xi_yield_uncert.first/(2*KS0_yield_uncert.first);
    ratioUncertainty = std::sqrt(std::pow((Xi_yield_uncert.second/(2*KS0_yield_uncert.first)),2) + 
                                 std::pow(((KS0_yield_uncert.second*Xi_yield_uncert.first)/(2*std::pow(KS0_yield_uncert.first,2))),2));
    std::cout << "Raw ratio value : " << ratioValue << "+/-" << ratioUncertainty << std::endl;

    std::string outfile_name = "Xi_KS0_ratio_TriGauss_Linear_" + binType + "_bin_" + std::to_string(binNumber)  + ".txt";
    std::ofstream outFile(outfile_name.c_str());
    outFile << KS0_yield_uncert.first << "\n";
    outFile << KS0_yield_uncert.second << "\n";
    outFile << Xi_yield_uncert.first << "\n";
    outFile << Xi_yield_uncert.second << "\n";
    outFile.close();

    //correct for geometric acceptance and efficiency
    std::string geoAccFile = geomAccept_dir + "Ximinus_to_KS0_geometric_acceptance_ratio_onlyPrimaries_phi.root";
    TFile* f_geoAccFile = TFile::Open(geoAccFile.c_str(), "READ");
    TCanvas* c_geoAccFile = (TCanvas*)f_geoAccFile->Get("myCanvas");
    TH1F* h_geoAccFile = (TH1F*)c_geoAccFile->GetPrimitive("Ximinus_inGeo_#phi");
    double binContent = h_geoAccFile->GetBinContent(binNumber+1);
    double binError   = h_geoAccFile->GetBinError(binNumber+1); 
    ratioValue = ratioValue/binContent;
    ratioUncertainty = ratioUncertainty/binContent;
    std::cout << "Ratio with geometric acceptance correction : " << ratioValue << "+/-" << ratioUncertainty << std::endl;
    f_geoAccFile->Close();

    //correct for cut efficiency
    std::string cutEffFile = cutEff_dir + "XiKS0ratio_phi_cutEff.root";
    TFile* f_cutEffFile = TFile::Open(cutEffFile.c_str(), "READ");
    TCanvas* c_cutEffFile = (TCanvas*)f_cutEffFile->Get("myCanvas");
    TGraphAsymmErrors* g_cutEffFile = (TGraphAsymmErrors*)c_cutEffFile->GetPrimitive("Graph");
    double binCenter;
    g_cutEffFile->GetPoint(binNumber, binCenter, binContent);
    binError   = g_cutEffFile->GetErrorYhigh(binNumber);
    ratioValue = ratioValue/binContent;
    ratioUncertainty = ratioUncertainty/binContent;
    std::cout << "Ratio with cut efficiency correction : " << ratioValue << "+/-" << ratioUncertainty << std::endl;
    f_cutEffFile->Close();

    //correct for tracking acceptance and efficiency 
    std::string trackAccFile = trackAccept_dir + "trackingAcceptanceEfficiencyCorrections_withSystematics.root";
    TFile* f_trackAccFile = TFile::Open(trackAccFile.c_str(), "READ");
    TH1F* h_trackAccFile = (TH1F*)f_trackAccFile->Get("h_phi_corrections");
    binContent = h_trackAccFile->GetBinContent(binNumber+1);
    binError   = h_trackAccFile->GetBinError(binNumber+1);
    ratioValue = ratioValue/binContent;
    ratioUncertainty = ratioUncertainty/binContent;
    std::cout << "Ratio with tracking acceptance and efficiency correction : " << ratioValue << "+/-" << ratioUncertainty << std::endl;
    f_trackAccFile->Close();

    //correct for branching ratios
    ratioValue = ratioValue*(KS02pipi_BR_PDG/(Xi2Lambdapi_BR_PDG*Lambda2ppi_BR_PDG));
    ratioUncertainty = ratioUncertainty*(KS02pipi_BR_PDG/(Xi2Lambdapi_BR_PDG*Lambda2ppi_BR_PDG));
    std::cout << "Ratio with branching ratio correction : " << ratioValue << "+/-" << ratioUncertainty << std::endl;
  }
  else if (binType == "rap")
  {
    if (binNumber >= nBins_rap)
    {
      std::cout << "ERROR: Bin value out of range for this bin type" << std::endl;
      return;
    }
    std::string binCut_KS0;
    std::string binCut_Xi;
    if (binNumber == nBins_rap - 1)
    {
      binCut_KS0 = Form("K_S0_rapidity >= %f && K_S0_rapidity <= %f", rapbins[binNumber],rapbins[binNumber+1]);
      binCut_Xi = Form("Ximinus_rapidity >= %f && Ximinus_rapidity <= %f && Lambda0_track_1_charge == track_3_charge", rapbins[binNumber],rapbins[binNumber+1]);
    }
    else
    {
      binCut_KS0 = Form("K_S0_rapidity >= %f && K_S0_rapidity < %f", rapbins[binNumber],rapbins[binNumber+1]);
      binCut_Xi = Form("Ximinus_rapidity >= %f && Ximinus_rapidity < %f && Lambda0_track_1_charge == track_3_charge", rapbins[binNumber],rapbins[binNumber+1]);
    }
 
    tempFileString = "temp_cut_trees_trash_" + binType + "_" + std::to_string(binNumber) + ".root";
    TFile* tempFile = new TFile(tempFileString.c_str(), "RECREATE");
    TTree* KS0DataTree_cut = KS0DataTree->CopyTree(binCut_KS0.c_str());
    TTree* XiDataTree_cut = XiDataTree->CopyTree(binCut_Xi.c_str());

    std::cout << "Extracting KS0 yield" << std::endl;
    std::pair<double,double> KS0_yield_uncert = GetKS0Yield(KS0DataTree_cut, saveYieldPlots, binType, binNumber);
    std::cout << "KS0 yield = " << KS0_yield_uncert.first << " +/- " << KS0_yield_uncert.second << std::endl;
    std::cout << "Extracting Xi yield" << std::endl;
    std::pair<double,double> Xi_yield_uncert = GetXiYield(XiDataTree_cut, saveYieldPlots, binType, binNumber);
    std::cout << "Xi yield = " << Xi_yield_uncert.first << " +/- " << Xi_yield_uncert.second << std::endl;

    tempFile->Close();

    // raw ratio value
    ratioValue = Xi_yield_uncert.first/(2*KS0_yield_uncert.first);
    ratioUncertainty = std::sqrt(std::pow((Xi_yield_uncert.second/(2*KS0_yield_uncert.first)),2) + 
                                 std::pow(((KS0_yield_uncert.second*Xi_yield_uncert.first)/(2*std::pow(KS0_yield_uncert.first,2))),2));
    std::cout << "Raw ratio value : " << ratioValue << "+/-" << ratioUncertainty << std::endl;

    std::string outfile_name = "Xi_KS0_ratio_TriGauss_Linear_" + binType + "_bin_" + std::to_string(binNumber)  + ".txt";
    std::ofstream outFile(outfile_name.c_str());
    outFile << KS0_yield_uncert.first << "\n";
    outFile << KS0_yield_uncert.second << "\n";
    outFile << Xi_yield_uncert.first << "\n";
    outFile << Xi_yield_uncert.second << "\n";
    outFile.close();

    //correct for geometric acceptance and efficiency
    std::string geoAccFile = geomAccept_dir + "Ximinus_to_KS0_geometric_acceptance_ratio_onlyPrimaries_rap.root";
    TFile* f_geoAccFile = TFile::Open(geoAccFile.c_str(), "READ");
    TCanvas* c_geoAccFile = (TCanvas*)f_geoAccFile->Get("myCanvas");
    TH1F* h_geoAccFile = (TH1F*)c_geoAccFile->GetPrimitive("Ximinus_inGeo_y");
    double binContent = h_geoAccFile->GetBinContent(binNumber+1);
    double binError   = h_geoAccFile->GetBinError(binNumber+1); 
    ratioValue = ratioValue/binContent;
    ratioUncertainty = ratioUncertainty/binContent;
    std::cout << "Ratio with geometric acceptance correction : " << ratioValue << "+/-" << ratioUncertainty << std::endl;
    f_geoAccFile->Close();

    //correct for cut efficiency
    std::string cutEffFile = cutEff_dir + "XiKS0ratio_y_cutEff.root";
    TFile* f_cutEffFile = TFile::Open(cutEffFile.c_str(), "READ");
    TCanvas* c_cutEffFile = (TCanvas*)f_cutEffFile->Get("myCanvas");
    TGraphAsymmErrors* g_cutEffFile = (TGraphAsymmErrors*)c_cutEffFile->GetPrimitive("Graph");
    double binCenter;
    g_cutEffFile->GetPoint(binNumber, binCenter, binContent);
    binError   = g_cutEffFile->GetErrorYhigh(binNumber);
    ratioValue = ratioValue/binContent;
    ratioUncertainty = ratioUncertainty/binContent;
    std::cout << "Ratio with cut efficiency correction : " << ratioValue << "+/-" << ratioUncertainty << std::endl;
    f_cutEffFile->Close();

    //correct for tracking acceptance and efficiency 
    std::string trackAccFile = trackAccept_dir + "trackingAcceptanceEfficiencyCorrections_withSystematics.root";
    TFile* f_trackAccFile = TFile::Open(trackAccFile.c_str(), "READ");
    TH1F* h_trackAccFile = (TH1F*)f_trackAccFile->Get("h_rap_corrections");
    binContent = h_trackAccFile->GetBinContent(binNumber+1);
    binError   = h_trackAccFile->GetBinError(binNumber+1);
    ratioValue = ratioValue/binContent;
    ratioUncertainty = ratioUncertainty/binContent;
    std::cout << "Ratio with tracking acceptance and efficiency correction : " << ratioValue << "+/-" << ratioUncertainty << std::endl;
    f_trackAccFile->Close();

    //correct for branching ratios
    ratioValue = ratioValue*(KS02pipi_BR_PDG/(Xi2Lambdapi_BR_PDG*Lambda2ppi_BR_PDG));
    ratioUncertainty = ratioUncertainty*(KS02pipi_BR_PDG/(Xi2Lambdapi_BR_PDG*Lambda2ppi_BR_PDG));
    std::cout << "Ratio with branching ratio correction : " << ratioValue << "+/-" << ratioUncertainty << std::endl;
  }
  //std::string outfile_name = "Xi_KS0_ratio_TriGauss_Cheb_" + binType + "_bin_" + std::to_string(binNumber)  + ".txt";
  //std::ofstream outFile(outfile_name.c_str());
  //outFile << ratioValue << "\n";
  //outFile << ratioUncertainty << "\n";
  //outFile.close();

  gSystem->Unlink(tempFileString.c_str());
}
