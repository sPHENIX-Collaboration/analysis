//=====================================================================-*-C++-*-
// File and Version Information:
//      $Id: RooUnfoldInvert.cxx 296 2011-09-30 00:46:54Z T.J.Adye $
//
// Description:
//      Unfolding class using inversion of the response matrix. This does not produce
//      good results and is designed to illustrate the need for more sophisticated
//      unfolding techniques
//
// Authors: Richard Claridge <richard.claridge@stfc.ac.uk> & Tim Adye <T.J.Adye@rl.ac.uk>
//
//==============================================================================

//____________________________________________________________
/* BEGIN_HTML
<p>The simplest method of unfolding works by simply inverting the response matrix.</p> 
<p>This is not accurate for small matrices and produces inaccurate unfolded distributions.</p>
<p>The inversion method is included largely to illustrate the necessity of a more effective method of unfolding</p>
END_HTML */

/////////////////////////////////////////////////////////////

#include "RooUnfoldInvert.h"

#include <iostream>

#include "TH1.h"
#include "TH2.h"
#include "TVectorD.h"
#include "TMatrixD.h"
#include "TDecompSVD.h"

#include "RooUnfoldResponse.h"

using std::cout;
using std::cerr;
using std::endl;

ClassImp (RooUnfoldInvert);

RooUnfoldInvert::RooUnfoldInvert (const RooUnfoldInvert& rhs)
  : RooUnfold (rhs)
{
  // Copy constructor.
  Init();
}

RooUnfoldInvert::RooUnfoldInvert (const RooUnfoldResponse* res, const TH1* meas,
                                  const char* name, const char* title)
  : RooUnfold (res, meas, name, title)
{
  // Constructor with response matrix object and measured unfolding input histogram.
  Init();
}

RooUnfoldInvert*
RooUnfoldInvert::Clone (const char* newname) const
{
  RooUnfoldInvert* unfold= new RooUnfoldInvert(*this);
  if (newname && strlen(newname)) unfold->SetName(newname);
  return unfold;
}


RooUnfoldInvert::~RooUnfoldInvert()
{
  delete _svd;
}

void
RooUnfoldInvert::Init()
{
  _svd= 0;
  GetSettings();
}

void
RooUnfoldInvert::Reset()
{
  delete _svd;
  Init();
  RooUnfold::Reset();
}

TDecompSVD*
RooUnfoldInvert::Impl()
{
  return _svd;
}

void
RooUnfoldInvert::Unfold()
{
  if (_nt>_nm) {
    cerr << "More truth bins ("<<_nt<<") than measured bin ("<<_nm<<") - cannot invert using TDecompSVD"<<endl;
    return;
  }
  _svd= new TDecompSVD (_res->Mresponse());
  if (_svd->Condition()<0){
    cerr <<"Warning: response matrix bad condition= "<<_svd->Condition()<<endl;
  }

  _rec.ResizeTo(_nm);
  _rec= Vmeasured();

  if (_res->FakeEntries()) {
    TVectorD fakes= _res->Vfakes();
    Double_t fac= _res->Vmeasured().Sum();
    if (fac!=0.0) fac=  Vmeasured().Sum() / fac;
    if (_verbose>=1) cout << "Subtract " << fac*fakes.Sum() << " fakes from measured distribution" << endl;
    fakes *= fac;
    _rec -= fakes;
  }

  Bool_t ok= _svd->Solve (_rec);
  _rec.ResizeTo(_nt);
  if (!ok) {
    cerr << "Response matrix Solve failed" << endl;
    return;
  }

  _unfolded= true;
  _haveCov=  false;
}

void
RooUnfoldInvert::GetCov()
{
    if (!_svd) return;
    TMatrixD resinv(_nt,_nm);
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,13,4)  /* TDecompSVD::Invert() didn't have ok status before 5.13/04. */
    Bool_t ok;
    resinv=_svd->Invert(ok);
    if (!ok) {
      cerr << "response matrix inversion failed" << endl;
      return;
    }
#else
    resinv=_svd->Invert();
#endif

    _cov.ResizeTo(_nt,_nt);
    ABAT (resinv, GetMeasuredCov(), _cov);
    _haveCov= true;
}

void
RooUnfoldInvert::GetSettings(){
    _minparm=0;
    _maxparm=0;
    _stepsizeparm=0;
    _defaultparm=0;
}
