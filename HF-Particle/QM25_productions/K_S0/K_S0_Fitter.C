#include <string>
#include <iostream>
#include <sstream>
#include <cmath>
#ifndef __CINT__
#include <RooGlobalFunc.h>
#endif
#include <RooRealVar.h>
#include <RooDataSet.h>
#include <RooDataHist.h>
#include <RooGaussian.h>
#include <RooBifurGauss.h>
#include <RooHist.h>
#include <RooPlot.h>
#include <RooExponential.h>
#include <RooAddPdf.h>
#include <RooStats/SPlot.h>
#include <TH1F.h>
#include <TFile.h>
#include <TTree.h>
#include <TCut.h>
#include <TChain.h>
#include <TMath.h>
#include <TPad.h>
#include <TCanvas.h>
#include <TBranch.h>

using namespace RooFit;
using namespace std;

//https://stackoverflow.com/questions/997946/how-to-get-current-time-and-date-in-c
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

void K_S0_Fitter()
{
  int maxDigits = 3;
  TGaxis::SetMaxDigits(maxDigits);

  string path, inputFile, plotTitle, saveName, xAxisTitle, branch, dataType;
  double minMass = 0.400;
  double maxMass = 0.600;

  path = "./";
  inputFile = "../pipi_reco/outputKFParticle_pipi_reco_00053877_00000_00000.root";
  dataType = "pp";

  branch = "K_S0_mass"; 
  plotTitle = "";
  saveName = path + "testFit_Kshort"; 
  xAxisTitle = "m(#pi^{+}#pi^{-}) [GeV]"; 

  stringstream cutStream;
  cutStream << minMass << " <= " << branch << " && " << branch << " <= " << maxMass;
  TCut masscut = cutStream.str().c_str();
 
  /*
   * Get files and data sets
   */

  TFile* dataFile = new TFile(inputFile.c_str());
  TTree* dataTree = (TTree*)dataFile->Get("DecayTree");
  
  string datasWeight =  inputFile.substr(0, inputFile.size()-5) + "_weighted.root";
  TFile* sWeightedDataFile  = new TFile(datasWeight.c_str(), "RECREATE");
  TTree* dataSWTree = dataTree->CopyTree(masscut);
  TTree* sWeightedDataTree = dataSWTree->CloneTree(-1);

  RooRealVar mass(branch.c_str(), "mass", minMass, maxMass);      
  RooDataSet dataSet(branch.c_str(), "data", mass, Import(*sWeightedDataTree));

  /*
   * Signal Model
   */

  RooRealVar  mean("K_S0_mean", "mean", 0.5, 0.46, 0.54);

  //Base Gaussian model
  RooRealVar  sigma("sigma", "sigma", 0.01, 0.001, 0.02);
  RooGaussian signal("signal", "signal", mass, mean, sigma);

  RooRealVar  fSig("fSig", "fSig", 0.*dataSet.numEntries(), 0.1, 1*dataSet.numEntries());

  /*
   * Background Model
   */

  RooRealVar expConst("expConst", "expConst", -1., -1.e4, 0.);         //creating the decay constant
  RooExponential background("background", "background", mass, expConst);          //creating the exponential background signal
 
  /*
   * Fitting to the data
   */

  RooArgList fitModelList(signal, background), fitFracList(fSig);
  RooAddPdf model("model", "model", fitModelList, fitFracList); 
  model.fitTo(dataSet);

  RooPlot* frame = mass.frame(Title(plotTitle.c_str()));       //creating the frame

  RooBinning bins(minMass, maxMass);
  int nBins = 25;
  bins.addUniform(nBins, minMass, maxMass);

  dataSet.plotOn(frame, Binning(bins), XErrorSize(0), DataError(RooAbsData::SumW2));          //plotting the raw unbinned data on the fram
  model.plotOn(frame, Components(background), LineColor(kGray), DrawOption("F"), FillColor(kGray));
  model.plotOn(frame, Components(RooArgSet(signal, background)), LineColor(kAzure+8), DrawOption("F"), FillColor(kAzure+8), MoveToBack());
  model.plotOn(frame, LineColor(kBlack));        //plotting the fit onto the frame
  dataSet.plotOn(frame, DrawOption("PE1"), Binning(bins),XErrorSize(0), DataError(RooAbsData::SumW2));          //plotting the raw unbinned data on the fram

  // Create a new frame to draw the pull distribution and add the distribution to the frame
  RooHist* pull = frame->pullHist();
  RooPlot* frame2 = mass.frame(Title(""));
  frame2->addPlotable(pull,"PE1");

  TCanvas* c = new TCanvas("massFitCanvas", "massFitCanvas",800, 800);  

  TPad mainPad("mainPad", "mainPad", 0., 0.3, 1., 1.);
  mainPad.SetBottomMargin(0);
  mainPad.Draw();

  TPad pullPad("pullPad", "pullPad", 0., 0.0, 1., 0.3);
  pullPad.SetBottomMargin(0.5);
  pullPad.SetTopMargin(0);

  pullPad.Draw();

  mainPad.cd();

  frame->SetMarkerStyle(kCircle);
  frame->SetMarkerSize(0.02);
  frame->SetLineWidth(1);
  frame->GetXaxis()->SetTitleSize(0);
  frame->GetXaxis()->SetLabelSize(0);
  frame->GetYaxis()->SetTitleFont(42);
  frame->GetYaxis()->SetLabelFont(42);
  frame->GetYaxis()->SetRangeUser(1, 50);
  float binWidth = 1000.*(maxMass - minMass) / nBins;
  string yAxisTitle = "Candidates / (" + to_string_with_precision(binWidth, 0) + " MeV)";
  frame->GetYaxis()->SetTitle(yAxisTitle.c_str());
  frame->Draw();

  //Print signal yield
  string fittedSigYield = to_string_with_precision(dataSet.numEntries() * fSig.getValV(), 0);
  string fittedSigError = to_string_with_precision(dataSet.numEntries() * fSig.getError(), 0);
  //int nEvents = dataTree->GetEntries();
  int nEvents = sWeightedDataTree->GetEntries();
  cout << "*\n*\n* nEvents = " << nEvents << "\n* nSig = " << fittedSigYield << " +/- " << fittedSigError << endl;
  double chiSq = frame->chiSquare("model", "data", 4);
  cout << "* Fit chisq/ndf = " << chiSq << "\n*\n*" << endl;

  TPaveText *pt;
  pt = new TPaveText(0.59,0.40,0.89,0.56, "NDC");
  pt->SetFillColor(0);
  pt->SetFillStyle(0);
  pt->SetTextFont(42);
  TText *pt_LaTex = pt->AddText("#it{#bf{sPHENIX}} Internal");
  pt_LaTex = pt->AddText("#it{p}+#it{p} #sqrt{s} = 200 GeV");
  pt->SetBorderSize(0);
  pt->DrawClone();

  //date timestamp
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

  //frame->Print();
  TLegend *legend = new TLegend(0.57,0.60,0.85,0.85);
  legend->AddEntry(frame->findObject("h_K_S0_mass"),"Data","PE2");
  legend->AddEntry(frame->findObject("model_Norm[K_S0_mass]"),"Fit","L");
  legend->AddEntry(frame->findObject("model_Norm[K_S0_mass]_Comp[signal,background]"),"K_{S}^{0}#rightarrow#pi^{+}#pi^{-}","f");
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
  frame2->GetXaxis()->SetTitle(xAxisTitle.c_str());
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
  TF1 *plusThreeLine = new TF1("plusThreeLine",  "pol1", minMass, maxMass);
  plusThreeLine->SetParameters(3, 0);
  plusThreeLine->SetLineColor(1);
  plusThreeLine->SetLineStyle(2);
  plusThreeLine->SetLineWidth(2);
  TF1 *zeroLine = new TF1("zeroLine",  "pol1", minMass, maxMass);
  zeroLine->SetParameters(0, 0);
  zeroLine->SetLineColor(1);
  zeroLine->SetLineStyle(2);
  zeroLine->SetLineWidth(2);
  TF1 *minusThreeLine = new TF1("minusThreeLine", "pol1", minMass, maxMass);
  minusThreeLine->SetParameters(-3, 0);
  minusThreeLine->SetLineColor(1);
  minusThreeLine->SetLineStyle(2);
  minusThreeLine->SetLineWidth(2);
  frame2->Draw();  
  plusThreeLine->Draw("same");
  zeroLine->Draw("same");
  minusThreeLine->Draw("same");

  vector<string> extensions = {".C", ".pdf", ".png"};
  for (auto &extension : extensions)
  {
    string outputFile = saveName + "_" + dataType + extension;
    c->SaveAs(outputFile.c_str());
  }
}
