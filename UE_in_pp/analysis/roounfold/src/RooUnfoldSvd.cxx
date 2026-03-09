//=====================================================================-*-C++-*-
// File and Version Information:
//      $Id: RooUnfoldSvd.cxx 298 2011-09-30 19:14:46Z T.J.Adye $
//
// Description:
//      SVD unfolding. Just an interface to TSVDUnfold.
//
// Author: Tim Adye <T.J.Adye@rl.ac.uk>
//
//==============================================================================

//____________________________________________________________
/* BEGIN_HTML
<p>Links to TSVDUnfold class which unfolds using Singular Value Decomposition (SVD).</p>
<p>Regularisation parameter defines the level at which values are deemed to be due to statistical fluctuations and are cut out. (Default= number of bins/2)
<p>Returns errors as a full matrix of covariances
<p>Error processing is much the same as with the kCovToy setting with 1000 toys. This is quite slow but can be switched off.
<p>Can only handle 1 dimensional distributions
<p>True and measured distributions must have the same binning
<p>Can account for both smearing and biasing
<p>Returns near singular covariance matrices, again leading to very large chi squared values
END_HTML */

/////////////////////////////////////////////////////////////

#include "RooUnfoldSvd.h"

#include <iostream>
#include <iomanip>

#include "TClass.h"
#include "TNamed.h"
#include "TH1.h"
#include "TH2.h"
#include "TVectorD.h"
#include "TMatrixD.h"
#if defined(HAVE_TSVDUNFOLD) || ROOT_VERSION_CODE < ROOT_VERSION(5,28,0)
#include "TSVDUnfold_local.h"  /* Use local copy of TSVDUnfold.h */
#else
#include "TSVDUnfold.h"
#endif

#include "RooUnfoldResponse.h"

using std::cout;
using std::cerr;
using std::endl;

ClassImp (RooUnfoldSvd);

RooUnfoldSvd::RooUnfoldSvd (const RooUnfoldSvd& rhs)
  : RooUnfold (rhs)
{
  // Copy constructor.
  Init();
  CopyData (rhs);
}

RooUnfoldSvd::RooUnfoldSvd (const RooUnfoldResponse* res, const TH1* meas, Int_t kreg, Int_t ntoyssvd,
                            const char* name, const char* title)
  : RooUnfold (res, meas, name, title), _kreg(kreg ? kreg : res->GetNbinsTruth()/2), _ntoyssvd(ntoyssvd)
{
  // Constructor with response matrix object and measured unfolding input histogram.
  // The regularisation parameter is kreg.
  Init();
}

RooUnfoldSvd*
RooUnfoldSvd::Clone (const char* newname) const
{
  RooUnfoldSvd* unfold= new RooUnfoldSvd(*this);
  if (newname && strlen(newname)) unfold->SetName(newname);
  return unfold;
}

void
RooUnfoldSvd::Reset()
{
  Destroy();
  Init();
  RooUnfold::Reset();
}

void
RooUnfoldSvd::Destroy()
{
  delete _svd;
  delete _meas1d;
  delete _train1d;
  delete _truth1d;
  delete _reshist;
}

void
RooUnfoldSvd::Init()
{
  _svd= 0;
  _meas1d= _train1d= _truth1d= 0;
  _reshist= 0;
  GetSettings();
}

void
RooUnfoldSvd::Assign (const RooUnfoldSvd& rhs)
{
  RooUnfold::Assign (rhs);
  CopyData (rhs);
}

void
RooUnfoldSvd::CopyData (const RooUnfoldSvd& rhs)
{
  _kreg= rhs._kreg;
  _ntoyssvd= rhs._ntoyssvd;
}

TSVDUnfold*
RooUnfoldSvd::Impl()
{
  return _svd;
}

void
RooUnfoldSvd::Unfold()
{
  if (_res->GetDimensionTruth() != 1 || _res->GetDimensionMeasured() != 1) {
    cerr << "RooUnfoldSvd may not work very well for multi-dimensional distributions" << endl;
  }
  if (_kreg < 0) {
    cerr << "RooUnfoldSvd invalid kreg: " << _kreg << endl;
    return;
  }

  if (_res->FakeEntries()) {
    _nb= _nt+1;
    if (_nm>_nb) _nb= _nm;
  } else {
    _nb= _nm > _nt ? _nm : _nt;
  }

  if (_kreg > _nb) {
    cerr << "RooUnfoldSvd invalid kreg=" << _kreg << " with " << _nb << " bins" << endl;
    return;
  }

  Bool_t oldstat= TH1::AddDirectoryStatus();
  TH1::AddDirectory (kFALSE);
  _meas1d=  HistNoOverflow (_meas,             _overflow);
  _train1d= HistNoOverflow (_res->Hmeasured(), _overflow);
  _truth1d= HistNoOverflow (_res->Htruth(),    _overflow);
  _reshist= _res->HresponseNoOverflow();
  Resize (_meas1d,  _nb);
  Resize (_train1d, _nb);
  Resize (_truth1d, _nb);
  Resize (_reshist, _nb, _nb);
  if (_res->FakeEntries()) {
    TVectorD fakes= _res->Vfakes();
    Double_t nfakes= fakes.Sum();
    if (_verbose>=1) cout << "Add truth bin for " << nfakes << " fakes" << endl;
    for (Int_t i= 0; i<_nm; i++) _reshist->SetBinContent(i+1,_nt+1,fakes[i]);
    _truth1d->SetBinContent(_nt+1,nfakes);
  }

  if (_verbose>=1) cout << "SVD init " << _reshist->GetNbinsX() << " x " << _reshist->GetNbinsY()
                        << " bins, kreg=" << _kreg << endl;
  _svd= new TSVDUnfold (_meas1d, _train1d, _truth1d, _reshist);

  TH1D* rechist= _svd->Unfold (_kreg);

  _rec.ResizeTo (_nt);
  for (Int_t i= 0; i<_nt; i++) {
    _rec[i]= rechist->GetBinContent(i+1);
  }

  delete rechist;
  TH1::AddDirectory (oldstat);

  _unfolded= true;
  _haveCov=  false;
}

void
RooUnfoldSvd::GetCov()
{
  if (!_svd) return;
  Bool_t oldstat= TH1::AddDirectoryStatus();
  TH1::AddDirectory (kFALSE);
  TH2D* meascov= new TH2D ("meascov", "meascov", _nb, 0.0, 1.0, _nb, 0.0, 1.0);
  const TMatrixD& cov= GetMeasuredCov();
  for (Int_t i= 0; i<_nm; i++)
    for (Int_t j= 0; j<_nm; j++)
      meascov->SetBinContent (i+1, j+1, cov(i,j));

  //Get the covariance matrix for statistical uncertainties on the measured distribution
  TH2D* unfoldedCov= _svd->GetUnfoldCovMatrix (meascov, _ntoyssvd);
  //Get the covariance matrix for statistical uncertainties on the response matrix
  TH2D* adetCov=     _svd->GetAdetCovMatrix   (_ntoyssvd);

  _cov.ResizeTo (_nt, _nt);
  for (Int_t i= 0; i<_nt; i++) {
    for (Int_t j= 0; j<_nt; j++) {
      _cov(i,j)= unfoldedCov->GetBinContent(i+1,j+1) + adetCov->GetBinContent(i+1,j+1);
    }
  }

  delete adetCov;
  delete unfoldedCov;
  delete meascov;
  TH1::AddDirectory (oldstat);

  _haveCov= true;
}

void
RooUnfoldSvd::GetSettings(){
    _minparm=0;
    _maxparm= _meas ? _meas->GetNbinsX() : 0;
    _stepsizeparm=1;
    _defaultparm=_maxparm/2;
}

void RooUnfoldSvd::Streamer (TBuffer &R__b)
{
  // Stream an object of class RooUnfoldSvd.
  if (R__b.IsReading()) {
    // Don't add our histograms to the currect directory.
    // We own them and we don't want them to disappear when the file is closed.
    Bool_t oldstat= TH1::AddDirectoryStatus();
    TH1::AddDirectory (kFALSE);
    RooUnfoldSvd::Class()->ReadBuffer  (R__b, this);
    TH1::AddDirectory (oldstat);
  } else {
    RooUnfoldSvd::Class()->WriteBuffer (R__b, this);
  }
}
