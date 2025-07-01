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
//#include <RooDoubleCB.h>
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

void D0_fitter_QM25_noPulls()
{
  int maxDigits = 3;
  TGaxis::SetMaxDigits(maxDigits);

  string path, inputFile, plotTitle, saveName, xAxisTitle, branch, dataType;
  double minMass = 1.7;
  double maxMass = 2.0;

  path = "/sphenix/user/cdean/public/QM25/";
 
  inputFile = path + "output_D0_oldKFP_ana475p017_optimized.root";
  dataType = "pp";

  branch = "D0_mass"; 
  plotTitle = "";
  saveName = path + "fits/D02Kpi_ana475p01_noPull";
  xAxisTitle = "m(K^{#mp}#pi^{#pm}) [GeV]"; 

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
  TTree* dataCopyTree = dataTree->CopyTree(masscut);
  TTree* dataCloneTree = dataCopyTree->CloneTree(-1);

  RooRealVar mass(branch.c_str(), "mass", minMass, maxMass);      
  RooDataSet dataSet(branch.c_str(), "data", mass, Import(*dataCloneTree));

  /*
   * Signal Model
   */

  RooRealVar  mean("D0_mean", "mean", 1.864, 1.855, 1.875);

  //Base Gaussian model
  RooRealVar  sigma("sigma", "sigma", 0.01, 0.000, 0.02);
  RooGaussian signal("signal", "signal", mass, mean, sigma);

  RooRealVar  fSig("fSig", "fSig", 0.*dataSet.numEntries(), 0., 0.1*dataSet.numEntries());
  RooRealVar  fBkg("fBkg", "fBkg", 0.*dataSet.numEntries(), 0., 1*dataSet.numEntries());

  /*
   * Background Model
   */

  RooRealVar expConstOne("expConstOne", "expConstOne", -10, -1e2, 0.);
  RooExponential background("background", "background", mass, expConstOne);
 
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

  TPad mainPad("mainPad", "mainPad", 0., 0., 1., 1.);
  mainPad.Draw();

  mainPad.cd();

  frame->SetMarkerStyle(kCircle);
  frame->SetMarkerSize(0.02);
  frame->SetLineWidth(1);
  frame->GetXaxis()->SetTitle(xAxisTitle.c_str());
  frame->GetXaxis()->SetNdivisions(505);
  frame->GetYaxis()->SetNdivisions(505);
  frame->GetYaxis()->SetTitleFont(42);
  frame->GetYaxis()->SetLabelFont(42);
  frame->GetYaxis()->SetRangeUser(181, 580);
  float binWidth = 1000.*(maxMass - minMass) / nBins;
  string yAxisTitle = "Candidates / (" + to_string_with_precision(binWidth, 0) + " MeV)";
  frame->GetYaxis()->SetTitle(yAxisTitle.c_str());
  frame->Draw();

  //Print signal yield
  string fitSigYield = to_string_with_precision(dataSet.numEntries() * fSig.getValV(), 0);
  string fitSigError = to_string_with_precision(dataSet.numEntries() * fSig.getError(), 0);
  string fitYield = "Yield = " + fitSigYield + "#pm " + fitSigError;
  double significance = fSig.getValV()/fSig.getError();
  string fitSignificance = "Significance = " + to_string_with_precision(significance, 1);
  int nEvents = dataCloneTree->GetEntries();
  cout << "*\n*\n* nEvents = " << nEvents << "\n* nSig = " << fitSigYield << " +/- " << fitSigError << endl;
  double chiSq = frame->chiSquare("model", "data", 4);
  cout << "* Fit chisq/ndf = " << chiSq << "\n*\n*" << endl;

  //Mean and width
  string fitMeanVal = to_string_with_precision(1e3 * mean.getValV(), 1);
  string fitMeanErr = to_string_with_precision(1e3 * mean.getError(), 1);
  string fitMean = "#mu = " + fitMeanVal + "#pm " + fitMeanErr + " MeV";
  string fitWidthVal = to_string_with_precision(1e3 * sigma.getValV(), 1);
  string fitWidthErr = to_string_with_precision(1e3 * sigma.getError(), 1);
  string fitWidth = "#sigma = " + fitWidthVal + "#pm " + fitWidthErr + " MeV";

  TPaveText *pt;
  pt = new TPaveText(0.19,0.20,0.52,0.45, "NDC"); //For 4 GeV
  pt->SetFillColor(0);
  pt->SetFillStyle(0);
  pt->SetTextFont(42);
  TText *pt_LaTex = pt->AddText("#it{#bf{sPHENIX}} Preliminary");
  pt_LaTex = pt->AddText("#it{p}+#it{p} #sqrt{s} = 200 GeV");
  pt_LaTex = pt->AddText("Early Calibration");
  pt_LaTex = pt->AddText(fitMean.c_str());
  pt_LaTex = pt->AddText(fitWidth.c_str());
  pt_LaTex = pt->AddText(fitYield.c_str());
  //pt_LaTex = pt->AddText(fitSignificance.c_str());
  pt->SetBorderSize(0);
  pt->DrawClone();

  //date timestamp
  TPaveText *ptDate;
  ptDate = new TPaveText(0.67,0.92,1.0,0.97, "NDC");
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
  TLegend *legend = new TLegend(0.53,0.68,0.83,0.88);
  legend->AddEntry(frame->findObject("h_D0_mass"),"Data","PE2");
  legend->AddEntry(frame->findObject("model_Norm[D0_mass]"),"Fit","L");
  legend->AddEntry(frame->findObject("model_Norm[D0_mass]_Comp[signal,background]"),"D^{0}#rightarrow K^{-}#pi^{+} + c.c.","f");
  legend->AddEntry(frame->findObject("model_Norm[D0_mass]_Comp[background]"),"Comb. Bkg.","f");
  legend->SetFillColor(0);
  legend->SetFillStyle(0);
  legend->SetBorderSize(0);
  legend->SetTextSize(0.05);
  legend->Draw();

  gPad->Modified();

  vector<string> extensions = {".C", ".pdf", ".png"};
  for (auto &extension : extensions)
  {
    string outputFile = saveName + "_" + dataType + extension;
    c->SaveAs(outputFile.c_str());
  }
}
