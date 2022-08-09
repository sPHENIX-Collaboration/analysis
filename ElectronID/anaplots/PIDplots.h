//
//	PIDplots.h
//	
//	Author :	Weihu Ma
//	Created:	04/2021
//	Version:	1.0
//

#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <math.h>

#include "TROOT.h"
#include "TApplication.h"
#include "TDirectory.h"
#include "TMath.h"
#include "TGraph.h"
#include "TGraph2D.h"
#include "TRandom.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TF3.h"
#include "TError.h"
#include "Fit/LogLikelihoodFCN.h"
#include "Fit/BasicFCN.h"
#include "Fit/BinData.h"
#include "Fit/UnBinData.h"
#include "Fit/FcnAdapter.h"
#include "Fit/FitConfig.h"
#include "Fit/FitResult.h"
#include "Fit/Fitter.h"
#include "Fit/Chi2FCN.h"
#include "Fit/PoissonLikelihoodFCN.h"
#include "TVirtualFitter.h"
#include "Math/Minimizer.h"
#include "Math/MinimizerOptions.h"
#include "Math/FitMethodFunction.h"
#include "Math/WrappedMultiTF1.h"
#include "Math/Error.h"
#include "Math/VirtualIntegrator.h"
#include "Math/GSLIntegrator.h"
#include "HFitInterface.h"
#include "Fit/FitExecutionPolicy.h"
#include "TF2.h"
#include "TF1.h"
#include "TGraphErrors.h" 
#include "TGraph.h" 
#include "TGaxis.h"
#include "TString.h"
#include "TSystem.h"
#include "TTree.h"
#include "TLegend.h"
#include "TText.h"
#include "TLatex.h"
#include "TFile.h"
#include "TAxis.h"
#include "TNtuple.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TFrame.h"
#include "TVector3.h"
#include "TFormula.h"

#include "TMVA/Factory.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Tools.h"
#include "TMVA/TMVAGui.h"


#include "omp.h"
using namespace std;

class PIDplots//A
{
 private:
  
  int Nfile;
  int Nevent;
  int Nmpnbin;
  int tag;
  
  float me;
  float mep;
  float men;
  float pi;
  float rYs[3];

  const char *input_file[400];
  char input_file_embed[400];

  const char * output_eID;
  const char * output_plot_eID;
  const char * output_plot_eID_eta;
  const char * output_plot_eID_pt;
  const char * output_plot_eID_HOM;
  const char * output_plot_eID_HOP;
  const char * output_plot_eID_EOP;
  
  //////////////////////////////////////////eID
  TFile *fHistogramFile_eID;
  TH1F *h1pt_e;
  TH1F *h1EOP;
  TH1F *h1EOP_e;  
  TH1F *h1HOM;
  TH1F *h1HOP;
  TH1F *h1dR;//dR=sqrt(deta*deta+dphi*dphi)
  TH1F *h1gflavor;
  
  int NEID_electron;

  int NEID_electron_eta[50];
  int NEID_electron_pt[50];
  int NEID_electron_HOP[50];
  int NEID_electron_HOM[50];
  int NEID_electron_EOP[50];
  
  
  int N_electron_eta[50];
  int N_electron_pt[50];
  int N_electron_HOM[50];
  int N_electron_HOP[50];
  int N_electron_EOP[50];
  
  int N_pion_eta[50];
  int N_K_eta[50];
  int N_antiproton_eta[50];
  int N_pion_pt[50];
  int N_K_pt[50];
  int N_antiproton_pt[50];
  int N_pion_HOM[50];
  int N_K_HOM[50];
  int N_antiproton_HOM[50];
  int N_pion_HOP[50];
  int N_K_HOP[50];
  int N_antiproton_HOP[50];
  int N_pion_EOP[50];
  int N_K_EOP[50];
  int N_antiproton_EOP[50];
  
  int NEID_pion_eta[50];
  int NEID_K_eta[50];
  int NEID_antiproton_eta[50];
  int NEID_pion_pt[50];
  int NEID_K_pt[50];
  int NEID_antiproton_pt[50];

  
  int NEID_pion;
  int NEID_K;
  int NEID_antiproton;
  float eta_point[50];
  float pt_point[50];
  float HOM_point[50];
  float HOP_point[50];
  float EOP_point[50];

public:
  
  void inputpars();
  void fhistogram_eID();
  void eID();
  void eIDoutput();
};

