//=====================================================================-*-C++-*-
// File and Version Information:
//      $Id: RooUnfoldTUnfold.cxx 309 2011-10-10 20:40:14Z T.J.Adye $
//
// Description:
//      Unfolding class using TUnfold from ROOT to do the actual unfolding.
//
// Authors: Richard Claridge <richard.claridge@stfc.ac.uk> & Tim Adye <T.J.Adye@rl.ac.uk>
//
//==============================================================================

//____________________________________________________________
/* BEGIN_HTML
<p>Uses the unfolding method implemented in ROOT's <a href="http://root.cern.ch/root/html/TUnfold.html">TUnfold</a> class
<p>Only included in ROOT versions 5.22 and higher
<p>Only able to reconstruct 1 dimensional distributions
<p>Can account for bin migration and smearing
<p>Errors come as a full covariance matrix. 
<p>Will sometimes warn of "unlinked" bins. These are bins with 0 entries and do not effect the results of the unfolding
<p>Regularisation parameter can be either optimised internally by plotting log10(chi2 squared) against log10(tau). The 'kink' in this curve is deemed the optimum tau value. This value can also be set manually (FixTau)
<p>The latest version (TUnfold 15 in ROOT 2.27.04) will not handle plots with an additional underflow bin. As a result overflows must be turned off
if v15 of TUnfold is used. ROOT versions 5.26 or below use v13 and so should be safe to use overflows.</ul>
END_HTML */

/////////////////////////////////////////////////////////////

#include "RooUnfoldTUnfold.h"

#include <iostream>

#include "TH1.h"
#include "TH2.h"
#include "TVectorD.h"
#include "TMatrixD.h"
#include "TUnfold.h"
#include "TGraph.h"

#include "RooUnfoldResponse.h"

using std::cout;
using std::cerr;
using std::endl;

ClassImp (RooUnfoldTUnfold);

RooUnfoldTUnfold::RooUnfoldTUnfold (const RooUnfoldTUnfold& rhs)
  : RooUnfold (rhs)
{
  // Copy constructor.
  Init();
  CopyData (rhs);
}

RooUnfoldTUnfold::RooUnfoldTUnfold (const RooUnfoldResponse* res, const TH1* meas, TUnfold::ERegMode reg,
                            const char* name, const char* title)
  : RooUnfold (res, meas, name, title),_reg_method(reg)
{
  // Constructor with response matrix object and measured unfolding input histogram.
  Init();
}

void
RooUnfoldTUnfold::Destroy()
{
    delete _unf;
}

RooUnfoldTUnfold*
RooUnfoldTUnfold::Clone (const char* newname) const
{
    //Clones object
  RooUnfoldTUnfold* unfold= new RooUnfoldTUnfold(*this);
  if (newname && strlen(newname)) unfold->SetName(newname);
  return unfold;
}

void
RooUnfoldTUnfold::Reset()
{
    //Resets all values
  Destroy();
  Init();
  RooUnfold::Reset();
}


void
RooUnfoldTUnfold::Assign (const RooUnfoldTUnfold& rhs)
{
  RooUnfold::Assign (rhs);
  CopyData (rhs);
}

void
RooUnfoldTUnfold::CopyData (const RooUnfoldTUnfold& rhs)
{
  tau_set=rhs.tau_set;
  _tau=rhs._tau;
  _reg_method=rhs._reg_method;
}


void
RooUnfoldTUnfold::Init()
{
    //Sets error matrix
    tau_set=false;
    _tau=0;
    _unf=0;
  GetSettings();
}

TUnfold*
RooUnfoldTUnfold::Impl()
{
    return _unf;
}


void
RooUnfoldTUnfold::Unfold()
{
    /* Does the unfolding. Uses the optimal value of the unfolding parameter unless a value has already been set using FixTau*/
       
  if (_nm<_nt)     cerr << "Warning: fewer measured bins than truth bins. TUnfold may not work correctly." << endl;
  if (_haveCovMes) cerr << "Warning: TUnfold does not account for bin-bin correlations on measured input"    << endl;

  Bool_t oldstat= TH1::AddDirectoryStatus();
  TH1::AddDirectory (kFALSE);
  TH1D* meas= HistNoOverflow (_meas, _overflow);
  TH2D* Hres=_res->HresponseNoOverflow();
  TH1::AddDirectory (oldstat);

  // Add inefficiencies to measured overflow bin
  TVectorD tru= _res->Vtruth();
  for (Int_t j= 1; j<=_nt; j++) {
    Double_t ntru= 0.0;
    for (Int_t i= 1; i<=_nm; i++) {
      ntru += Hres->GetBinContent(i,j);
    }
    Hres->SetBinContent (_nm+1, j, tru[j-1]-ntru);
  }

  // Subtract fakes from measured distribution
  if (_res->FakeEntries()) {
    TVectorD fakes= _res->Vfakes();
    Double_t fac= _res->Vmeasured().Sum();
    if (fac!=0.0) fac=  Vmeasured().Sum() / fac;
    if (_verbose>=1) cout << "Subtract " << fac*fakes.Sum() << " fakes from measured distribution" << endl;
    for (Int_t i= 1; i<=_nm; i++)
      meas->SetBinContent (i, meas->GetBinContent(i)-(fac*fakes[i-1]));
  }

  Int_t ndim= _meas->GetDimension();
  TUnfold::ERegMode reg= _reg_method;
  if (ndim == 2 || ndim == 3) reg= TUnfold::kRegModeNone;  // set explicitly

  _unf= new TUnfold(Hres,TUnfold::kHistMapOutputVert,reg);

  if        (ndim == 2) {
    Int_t nx= _meas->GetNbinsX(), ny= _meas->GetNbinsY();
    _unf->RegularizeBins2D (0, 1, nx, nx, ny, _reg_method);
  } else if (ndim == 3) {
    Int_t nx= _meas->GetNbinsX(), ny= _meas->GetNbinsY(), nz= _meas->GetNbinsZ(), nxy= nx*ny;
    for (Int_t i= 0; i<nx; i++) {
      _unf->RegularizeBins2D (    i, nx, ny, nxy, nz, _reg_method);
    }
    for (Int_t i= 0; i<ny; i++) {
      _unf->RegularizeBins2D ( nx*i,  1, nx, nxy, nz, _reg_method);
    }
    for (Int_t i= 0; i<nz; i++) {
      _unf->RegularizeBins2D (nxy*i,  1, nx,  nx, ny, _reg_method);
    }
  }

  Int_t nScan=30;
  // use automatic L-curve scan: start with taumin=taumax=0.0
  Double_t tauMin=0.0;
  Double_t tauMax=0.0;
  Int_t iBest;
  TSpline *logTauX,*logTauY;
  TGraph *lCurve;
  // this method scans the parameter tau and finds the kink in the L curve
  // finally, the unfolding is done for the best choice of tau
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,23,0)  /* TUnfold v6 (included in ROOT 5.22) didn't have setInput return value */
  Int_t stat= _unf->SetInput(meas);
  if(stat>=10000) {
    cerr<<"Unfolding result may be wrong: " << stat/10000 << " unconstrained output bins\n";
  }
#else
  _unf->SetInput(meas);
#endif
  //_unf->SetConstraint(TUnfold::kEConstraintArea);
  if (!tau_set){
    iBest=_unf->ScanLcurve(nScan,tauMin,tauMax,&lCurve,&logTauX,&logTauY);
    _tau=_unf->GetTau();  // save value, even if we don't use it unless tau_set
    cout <<"Lcurve scan chose tau= "<<_tau<<endl;
  }
  else{
    _unf->DoUnfold(_tau);
  }
  TH1* reco=0;
  _unf->GetOutput(reco);
  reco->SetName("_rec");
  reco->SetTitle("reconstructed dist");
  _rec.ResizeTo (_nt);
  for (int i=0;i<_nt;i++){
    _rec(i)=(reco->GetBinContent(i+1));
  }
  delete meas;
  delete Hres;
  delete reco;
  _unfolded= true;
  _haveCov=  false;
}

void
RooUnfoldTUnfold::GetCov()
{
  //Gets Covariance matrix
  if (!_unf) return;
  TH2D* ematrix=0;
  _unf->GetEmatrix(ematrix);
  ematrix->SetName("ematrix");
  ematrix->SetTitle("error matrix");
  _cov.ResizeTo (_nt,_nt);
  for (Int_t i= 0; i<_nt; i++) {
    for (Int_t j= 0; j<_nt; j++) {
      _cov(i,j)= ematrix->GetBinContent(i+1,j+1);
    }
  }
  delete ematrix;
  _haveCov= true;
}


void 
RooUnfoldTUnfold::FixTau(Double_t tau)
{
  // Fix regularisation parameter to a specified value
  _tau=tau;
  tau_set=true;
}

void
RooUnfoldTUnfold::SetRegMethod(TUnfold::ERegMode regmethod)
{
  /*
    Specifies the regularisation method:

      regemthod setting             regularisation
      ===========================   ==============
      TUnfold::kRegModeNone         none
      TUnfold::kRegModeSize         minimize the size of (x-x0)
      TUnfold::kRegModeDerivative   minimize the 1st derivative of (x-x0)
      TUnfold::kRegModeCurvature    minimize the 2nd derivative of (x-x0)
   */
  _reg_method=regmethod;
}

void
RooUnfoldTUnfold::OptimiseTau()
{
  // Choose optimal regularisation parameter
  tau_set=false;
}

void
RooUnfoldTUnfold::GetSettings()
{
    _minparm=0;
    _maxparm=1;
    _stepsizeparm=1e-2;
    _defaultparm=2;
}
