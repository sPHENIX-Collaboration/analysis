//=====================================================================-*-C++-*-
// File and Version Information:
//      $Id: RooUnfoldTestHarness.h 298 2011-09-30 19:14:46Z T.J.Adye $
//
// Description:
//      Test Harness class for the RooUnfold package using toy MC generated
//      according to PDFs defined in RooUnfoldTestPdf.icc or RooUnfoldTestPdfRooFit.icc.
//
// Authors: Tim Adye <T.J.Adye@rl.ac.uk> and Fergus Wilson <fwilson@slac.stanford.edu>
//
//==============================================================================

#ifndef ROOUNFOLDTESTHARNESS_HH
#define ROOUNFOLDTESTHARNESS_HH

#if !defined(__CINT__) || defined(__MAKECINT__)
#include "TNamed.h"
#include "TString.h"
#include "RooUnfold.h"
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,0,0)
#include "TVectorDfwd.h"
#include "TMatrixDfwd.h"
#else
class TVectorD;
class TMatrixD;
#endif
#endif

#if ((defined(__CINT__) && !defined(__MAKECINT__)) || defined(MAKEBUILD)) && !defined(NOTUNFOLD)
#define USE_TUNFOLD_H 1   // can include TUnfold.h (don't know for ACLiC)
#endif

#ifdef __CINT__
#include "TPad.h"    // Why is this necessary (at least in ROOT 5.18+)?
#include "ArgVars.h"
#else
class ArgVars;
#endif

class TCanvas;
class TPostScript;
class TLegend;
class TH1;
class TH1D;
class TH2D;
class TNtuple;
class RooUnfoldResponse;
class RooUnfold;
class RooUnfoldErrors;
class RooUnfoldParms;

class RooUnfoldTestHarness : public TNamed {
public:
  // Parameters
  Int_t    method, stage, ftrainx, ftestx, ntx, ntest, ntrain, wpaper, hpaper, regmethod;
  Int_t    ntoyssvd, nmx, onepage, doerror, dim, overflow, addbias, nbPDF, verbose, dodraw;
  Int_t    ntoys, ploterrors, plotparms;
  Double_t xlo, xhi, mtrainx, wtrainx, btrainx, mtestx, wtestx, btestx, mscalex, bincorr;
  Double_t regparm, effxlo, effxhi, xbias, xsmear, fakexlo, fakexhi, minparm, maxparm, stepsize;
  TString  rootfile;

  // Data
  Int_t              error, ipad, ntbins, nmbins;
  TCanvas*           canvas;
  TPostScript*       ps;
  TLegend            *lTrain, *lTest, *lErrors;
  TH1                *hTrain, *hTrainTrue, *hTrainFake, *hTrue, *hMeas, *hReco, *hFake, *hRes, *hPulls;
  TH1                *hUnfErr, *hToyErr, *hParmChi2, *hParmErr, *hParmRes, *hParmRms;
  TH1D               *hPDFx, *hTestPDFx;
  TH2D               *hResmat, *hCorr, *hMeasCorr;
  TNtuple            *ntChi2;
  RooUnfoldResponse* response;
  RooUnfold*         unfold;
  RooUnfoldErrors*   errors;
  RooUnfoldParms*    parms;
  // Constructors
  RooUnfoldTestHarness (const char* name= "RooUnfoldTest");
  RooUnfoldTestHarness (const char* name, const char* args);
  RooUnfoldTestHarness (const char* name, int argc, const char* const* argv);
  virtual ~RooUnfoldTestHarness();

  // Methods and functions
  virtual void     Parms (ArgVars& args);
  virtual Int_t    Run();
  virtual void     SetupCanvas();
  virtual Int_t    RunTests();
  virtual Int_t    Train();
  virtual void     TrainResults();
  virtual Int_t    Test();
  virtual void     SetMeasuredCov();
  virtual Int_t    Unfold();
  virtual void     Results();
  virtual void     PlotErrors();
  virtual void     PlotParms();
  virtual TH1D*    Generate (TVectorD& x, const char* name, const char* title, Int_t nt, Int_t fpdf,
                             Int_t nx, Double_t xlo, Double_t xhi, Double_t bkg, Double_t mean, Double_t width,
                             Double_t bias, Double_t smear);
  virtual Int_t    Fakes    (TVectorD& x, Int_t nm, Int_t nx, Double_t lo, Double_t hi, Double_t fakelo, Double_t fakehi) const;
  virtual bool     Eff      (Double_t x,           Double_t xlo, Double_t xhi, Double_t efflo, Double_t effhi) const;
  virtual Double_t Smear    (Double_t x, Int_t nt, Double_t xlo, Double_t xhi, Double_t bias,  Double_t smear) const;
  virtual Double_t Overflow (Double_t x, Int_t nt, Double_t xlo, Double_t xhi) const;
  virtual void     Reset();
  virtual void     SetDefaults();
  virtual int      SetArgs  (int argc, const char* const* argv, bool split= false);
  virtual void     Init();
  virtual Int_t    CheckParms();
  virtual void     PrintParms (std::ostream& o) const;
  static  void     setmax   (TH1* h, const TH1* h1= 0, const TH1* h2= 0, const TH1* h3= 0,
                             const TH1* h4= 0, const TH1* h5= 0, const TH1* h6= 0);
  static  void     Legend (TLegend*& legend, TH1* pdf, TH1* truth, TH1* fake, TH1* meas, TH1* reco= 0);
  static  TH2D*    CorrelationHist (const TMatrixD& cov,
                                    const char* name="rho", const char* title="Correlation matrix",
                                    Double_t lo=0.0, Double_t hi=1.0);
  static  void     PrintMatrix (const TMatrixD& m, const char* format= 0,
                                const char* name= "matrix", Int_t cols_per_sheet= 0);
};
#ifndef NOINLINE
#include "RooUnfoldTestHarness.icc"
#endif

#endif
