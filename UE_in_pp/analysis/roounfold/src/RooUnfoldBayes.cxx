//=====================================================================-*-C++-*-
// File and Version Information:
//      $Id: RooUnfoldBayes.cxx 296 2011-09-30 00:46:54Z T.J.Adye $
//
// Description:
//      Bayesian unfolding.
//
// Author: Tim Adye <T.J.Adye@rl.ac.uk>
//
//==============================================================================

//____________________________________________________________
/* BEGIN_HTML
<p>Links to the RooUnfoldBayesImpl class which uses Bayesian unfolding to reconstruct the truth distribution.</p>
<p>Works for 2 and 3 dimensional distributions
<p>Returned errors can be either as a diagonal matrix or as a full matrix of covariances
<p>Regularisation parameter sets the number of iterations used in the unfolding (default=4)
<p>Is able to account for bin migration and smearing
<p>Can unfold if test and measured distributions have different binning.
<p>Returns covariance matrices with conditions approximately that of the machine precision. This occasionally leads to very large chi squared values
END_HTML */

/////////////////////////////////////////////////////////////

//#define OLDERRS   // restore old (incorrect) error calculation

#include "RooUnfoldBayes.h"

#include <iostream>
#include <iomanip>
#include <math.h>

#include "TNamed.h"
#include "TH1.h"
#include "TH2.h"

#include "RooUnfoldResponse.h"

using std::min;
using std::cerr;
using std::endl;
using std::cout;
using std::setw;
using std::left;
using std::right;

ClassImp (RooUnfoldBayes);

bool RooUnfoldBayes::UseOldErrors=true;

RooUnfoldBayes::RooUnfoldBayes (const RooUnfoldBayes& rhs)
  : RooUnfold (rhs)
{
  // Copy constructor.
  Init();
  CopyData (rhs);
}

RooUnfoldBayes::RooUnfoldBayes (const RooUnfoldResponse* res, const TH1* meas, Int_t niter, Bool_t smoothit,
                                const char* name, const char* title)
  : RooUnfold (res, meas, name, title), _niter(niter), _smoothit(smoothit)
{
  // Constructor with response matrix object and measured unfolding input histogram.
  // The regularisation parameter is niter (number of iterations).
  Init();
}

RooUnfoldBayes* RooUnfoldBayes::Clone (const char* newname) const
{
  // Creates a copy of the RooUnfoldBayes object
  RooUnfoldBayes* unfold= new RooUnfoldBayes(*this);
  if (newname && strlen(newname)) unfold->SetName(newname);
  return unfold;
}

void RooUnfoldBayes::Init()
{
  _nc= _ne= 0;
  GetSettings();
}

void RooUnfoldBayes::Reset()
{
  Init();
  RooUnfold::Reset();
}

void RooUnfoldBayes::Assign (const RooUnfoldBayes& rhs)
{
  RooUnfold::Assign (rhs);
  CopyData (rhs);
}

void RooUnfoldBayes::CopyData (const RooUnfoldBayes& rhs)
{
  _niter=    rhs._niter;
  _smoothit= rhs._smoothit;
}

void RooUnfoldBayes::Unfold()
{
  setup();
  if (verbose() >= 2) Print();
  if (verbose() >= 1) cout << "Now unfolding..." << endl;
  train();
  if (verbose() >= 2) Print();
  _rec.ResizeTo(_nc);
  _rec = _nbarCi;
  _rec.ResizeTo(_nt);  // drop fakes in final bin
  _unfolded= true;
  _haveCov=  false;
}

void RooUnfoldBayes::GetCov()
{
  getCovariance();
  _cov.ResizeTo (_nt, _nt);  // drop fakes in final bin
  _haveCov= true;
}

void RooUnfoldBayes::GetSettings()
{
  _minparm=1;
  _maxparm=15;
  _stepsizeparm=1;
  _defaultparm=4;
}

TMatrixD& RooUnfoldBayes::H2M (const TH2* h, TMatrixD& m, Bool_t overflow)
{
  // TH2 -> TMatrixD
  if (!h) return m;
  Int_t first= overflow ? 0 : 1;
  Int_t nm= m.GetNrows(), nt= m.GetNcols();
  for (Int_t j= 0; j < nt; j++)
    for (Int_t i= 0; i < nm; i++)
      m(i,j)= h->GetBinContent(i+first,j+first);
  return m;
}

//-------------------------------------------------------------------------
void RooUnfoldBayes::setup()
{
  _nc = _nt;
  _ne = _nm;

  _nEstj.ResizeTo(_ne);
  _nEstj= Vmeasured();

  _nCi.ResizeTo(_nt);
  _nCi= _res->Vtruth();

  _Nji.ResizeTo(_ne,_nt);
  H2M (_res->Hresponse(), _Nji, _overflow);   // don't normalise, which is what _res->Mresponse() would give us

  if (_res->FakeEntries()) {
    TVectorD fakes= _res->Vfakes();
    Double_t nfakes= fakes.Sum();
    if (verbose()>=0) cout << "Add truth bin for " << nfakes << " fakes" << endl;
    _nc++;
    _nCi.ResizeTo(_nc);
    _nCi[_nc-1]= nfakes;
    _Nji.ResizeTo(_ne,_nc);
    for (Int_t i= 0; i<_nm; i++) _Nji(i,_nc-1)= fakes[i];
  }

  _nbarCi.ResizeTo(_nc);
  _efficiencyCi.ResizeTo(_nc);
  _Mij.ResizeTo(_nc,_ne);
  _dnCidnEj.ResizeTo(_nc,_ne);
}

//-------------------------------------------------------------------------
void RooUnfoldBayes::train()
{
  // After accumulating the training sample, calculate the unfolding matrix.
  // _niter = number of iterations to perform (3 by default).
  // _smoothit = smooth the matrix in between iterations (default false).

  Double_t ntrue = _nCi.Sum();

  // Initial distribution
  TVectorD P0C(_nCi);
  P0C *= 1.0/ntrue;

  cout << "intitial distribution generated" << endl;

  TMatrixD PEjCi(_ne,_nc), PEjCiEff(_ne,_nc);
  for (Int_t i = 0 ; i < _nc ; i++) {
    if (_nCi[i] <= 0.0) { _efficiencyCi[i] = 0.0; continue; }
    Double_t eff = 0.0;
    for (Int_t j = 0 ; j < _ne ; j++) {
      Double_t response = _Nji(j,i) / _nCi[i];
      PEjCi(j,i) = PEjCiEff(j,i) = response;  // efficiency of detecting the cause Ci in Effect Ej
      eff += response;
    }
    _efficiencyCi[i] = eff;
    Double_t effinv = eff > 0.0 ? 1.0/eff : 0.0;   // reset PEjCiEff if eff=0
    for (Int_t j = 0 ; j < _ne ; j++) PEjCiEff(j,i) *= effinv;
  }



  for (Int_t kiter = 0 ; kiter < _niter; kiter++) {

    if (verbose()>=1) cout << "Iteration : " << kiter << endl;

    TVectorD UjInv(_ne);
    for (Int_t j = 0 ; j < _ne ; j++) {
      Double_t Uj = 0.0;
      for (Int_t i = 0 ; i < _nc ; i++)
	Uj += PEjCi(j,i) * P0C[i];
      UjInv[j] = Uj > 0.0 ? 1.0/Uj : 0.0;
    }
    
    // Unfolding matrix M
    _nbartrue = 0.0;
    for (Int_t i = 0 ; i < _nc ; i++) {
      Double_t nbarC = 0.0;
      for (Int_t j = 0 ; j < _ne ; j++) {
        Double_t Mij = UjInv[j] * PEjCiEff(j,i) * P0C[i];
        _Mij(i,j) = Mij;
        nbarC += Mij * _nEstj[j];
      }
      _nbarCi[i] = nbarC;
      _nbartrue += nbarC;  // best estimate of true number of events
    }


    // new estimate of true distribution
    TVectorD PbarCi(_nbarCi);
    PbarCi *= 1.0/_nbartrue;


    if(!UseOldErrors)
    {
      if (kiter <= 0) {
	_dnCidnEj= _Mij;
      } else {
	TVectorD ksum(_ne);
	for (Int_t j = 0 ; j < _ne ; j++) {
	  for (Int_t k = 0 ; k < _ne ; k++) {
	    Double_t sum = 0.0;
	    for (Int_t l = 0 ; l < _nc ; l++) {
	      if (P0C[l]>0.0) sum += _efficiencyCi[l]*_Mij(l,k)*_dnCidnEj(l,j)/P0C[l];
	    }
	    ksum[k]= sum;
	  }
	  for (Int_t i = 0 ; i < _nc ; i++) {
	    Double_t dsum = P0C[i]>0 ? _dnCidnEj(i,j)*_nbarCi[i]/P0C[i] : 0.0;
	    for (Int_t k = 0 ; k < _ne ; k++) {
	      dsum -= _Mij(i,k)*_nEstj[k]*ksum[k];
	    }
	    // update dnCidnEj. Note that we can do this in-place due to the ordering of the accesses.
	    _dnCidnEj(i,j) = _Mij(i,j) + dsum/ntrue;
	  }
	}
      }
    }


    // no need to smooth the last iteraction
    if (_smoothit && kiter < (_niter-1)) smooth(PbarCi);



    // Chi2 based on Poisson errors
    Double_t chi2 = getChi2(PbarCi, P0C, _nbartrue);
    if (verbose()>=1) cout << "Chi^2 of change " << chi2 << endl;


    //Total Chi2 calculated by tim based on poisson errors:
    Double_t totchi2 = getChi2Val(PbarCi,_nbartrue);
    // cout << totchi2<< endl;

    // replace P0C
    P0C = PbarCi;
    ntrue = _nbartrue;

    // and repeat
  }
}

//-------------------------------------------------------------------------
void RooUnfoldBayes::getCovariance(Bool_t doUnfoldSystematic)
{
  if (verbose()>=1) cout << "Calculating covariances due to number of measured events" << endl;

  // Create the covariance matrix of result from that of the measured distribution
  _cov.ResizeTo (_nc, _nc);
  if(UseOldErrors)  ABAT (_Mij,      GetMeasuredCov(), _cov);
  else  ABAT (_dnCidnEj, GetMeasuredCov(), _cov);


  // error due to uncertainty on unfolding matrix M
  // This is disabled by default: I'm not sure it is correct, it is very slow, and
  // the effect should be small with good MC statistics.
  if (!doUnfoldSystematic) return;

  if (verbose()>=0 && _nc*_ne >= 50625)
    cout << "getCovariance (this takes some time with " << _nc << " x " << _ne << " bins)." << endl;

  if (verbose()>=1) cout << "Calculating covariance due to unfolding matrix..." << endl;

  // Pre-compute some numbers
  TVectorD inv_nCi(_nCi);
  TMatrixD inv_npec(_nc,_ne);  // automatically zeroed
  for (Int_t k = 0 ; k < _nc ; k++) {
    if (inv_nCi[k] != 0) {inv_nCi[k] = 1.0 / inv_nCi[k];}
    for (Int_t i = 0 ; i < _ne ; i++) {
      if (inv_nCi[k] == 0) continue;
      Double_t pec  = _Nji(i,k) / _nCi[k];
      Double_t temp = inv_nCi[k] / pec;
      if (pec !=0) {inv_npec(k,i) = temp; }
    }
  }
  //
  TMatrixD M_tmp(_nc,_ne);  // automatically zeroed
  for (Int_t i = 0 ; i < _ne ; i++) {
    Double_t temp = 0.0;
    // diagonal element
    for (Int_t u = 0 ; u < _nc ; u++) {
      temp = _Mij(i,u) * _Mij(i,u) * _efficiencyCi[u] * _efficiencyCi[u]
        * (inv_npec(u,i) - inv_nCi[u]);
      M_tmp(i, i) += temp;
    }

    // off-diagonal element
    for (Int_t j = i+1 ; j < _ne ; j++) {
      for (Int_t u = 0 ; u < _nc ; u++) {
        temp = -1.0 * _Mij(i,u) * _Mij(j,u) * _efficiencyCi[u] * _efficiencyCi[u]
          * inv_nCi[u];
        M_tmp(j, i) += temp;
      }
      M_tmp(i, j) =  M_tmp(j, i); // symmetric matrix
    }
  }

  // now calculate covariance
  TMatrixD Vc1(_nc,_nc);
  Double_t neff_inv = 0.0;
  for (Int_t k = 0 ; k < _nc ; k++) {
    (_efficiencyCi[k] != 0) ? neff_inv = inv_nCi[k]  / _efficiencyCi[k] : neff_inv = 0;
    for (Int_t l = k ; l < _nc ; l++) {
      for (Int_t i = 0 ; i < _ne ; i++) {
        for (Int_t j = 0 ; j < _ne ; j++) {
          Double_t covM = _Mij(i,l) * inv_nCi[l] +
            _Mij(j,k) * inv_nCi[k] + M_tmp(j,i);
          if (k==l) {
            covM -= neff_inv;
            if (i==j) {covM += inv_npec(k,i);}
          }
          if (i==j) {
            covM -=  (_Mij(i,l) * _efficiencyCi[l] * inv_npec(l,i) +
                      _Mij(i,k) * _efficiencyCi[k] * inv_npec(k,i) );
          }
          covM +=  _Mij(i,k) * _Mij(j,l);

          Double_t temp = _nEstj[i] * _nEstj[j] * covM;
          Vc1(l, k) += temp;
        } // j...
      } // i...
      Vc1(k, l) =  Vc1(l,k);
    } // l...
  } // k...

    // to get complete covariance add together
  Vc1 *= 1.0/(_nbartrue*_nbartrue);  // divide by _nbartrue*_nbartrue to get probability covariance matrix
  _cov += Vc1;

}

//-------------------------------------------------------------------------
void RooUnfoldBayes::smooth(TVectorD& PbarCi) const
{
  // Smooth unfolding distribution. PbarCi is the array of proababilities
  // to be smoothed PbarCi; nevts is the numbers of events
  // (needed to calculate suitable errors for the smearing).
  // PbarCi is returned with the smoothed distribution.

  if (_res->GetDimensionTruth() != 1) {
    cerr << "Smoothing only implemented for 1-D distributions" << endl;
    return;
  }
  if (verbose()>=1) cout << "Smoothing." << endl;
  TH1::SmoothArray (_nc, PbarCi.GetMatrixArray(), 1);
  return;
}

//-------------------------------------------------------------------------
Double_t RooUnfoldBayes::getChi2Val(const TVectorD& prob1, Double_t nevents) const
{
  // added by tim and is seemingly meaningless!
  Double_t chi2 = 0.0;
  Int_t n=prob1.GetNrows();
  for (int i = 0;i < n;i++)
    {
      chi2 = chi2 + prob1[i]*nevents;
    }
  return(chi2);
}



Double_t RooUnfoldBayes::getChi2(const TVectorD& prob1,
                                 const TVectorD& prob2,
                                 Double_t nevents) const
{
  // calculate the chi^2. prob1 and prob2 are the probabilities
  // and nevents is the number of events used to calculate the probabilities
  Double_t chi2= 0.0;
  Int_t n= prob1.GetNrows();
  if (verbose()>=2) cout << "chi2 " << n << " " << nevents << endl;
  for (Int_t i = 0 ; i < n ; i++) {
    Double_t psum  = (prob1[i] + prob2[i])*nevents;
    Double_t pdiff = (prob1[i] - prob2[i])*nevents;
    if (psum > 1.0) {
      chi2 = chi2 + (pdiff*pdiff)/psum;
    } else {
      chi2 = chi2 + (pdiff*pdiff);
    }
    
    // chi2 = chi2+(pdiff);
    
  }
  return(chi2);
}

//-------------------------------------------------------------------------
void RooUnfoldBayes::Print(Option_t* option) const
{
  RooUnfold::Print (option);
  if (_nc<=0 || _ne<=0) return;

  // Print out some useful info of progress so far

  cout << "-------------------------------------------" << endl;
  cout << "Unfolding Algorithm" << endl;
  cout << "Generated (Training):" << endl;
  cout << "  Total Number of bins   : " << _nc << endl;
  Double_t ntrue = _nCi.Sum();
  cout << "  Total Number of events : " << ntrue << endl;

  cout << "Measured (Training):" << endl;
  cout << "  Total Number of bins   : " << _ne << endl;

  cout << "Input (for unfolding):" << endl;
  cout << "  Total Number of events : " << _nEstj.Sum() << endl;

  cout << "Output (unfolded):" << endl;
  cout << "  Total Number of events : " << _nbarCi.Sum() <<endl;

  cout << "-------------------------------------------\n" << endl;

  if (((_nEstj.Sum())!=0) || ((_nCi.Sum())!=0)) {
    Int_t iend = min(_nCi.GetNrows(),_nEstj.GetNrows());
    cout << "    \tTrain \tTest\tUnfolded"<< endl;
    cout << "Bin \tTruth \tInput\tOutput"<< endl;
    Int_t ir=0, ic=0;
    for (Int_t i=0; i < iend ; i++) {
      ic = i / _ne;
      ir = i - (ic*_ne);
      if ((_nCi[i] == 0) && (_nEstj[i] == 0) &&
          (_nEstj[i] == 0) && (_nbarCi[i]==0)) continue;
      cout << i << "\t" << _nCi[i]                                      \
           << "\t " << _nEstj[i] << "\t " << _nbarCi[i] << endl;
    }

    // if the number of bins is different
    if (_nCi.GetNrows() > _nEstj.GetNrows() ) {
      for (Int_t i=iend; i < _nCi.GetNrows() ; i++) {
        cout << i << "\t " << _nCi[i] << endl;
      }
    }

    cout << "--------------------------------------------------------" << endl;
    cout << " \t" << (_nCi.Sum())
         << "\t " << (_nEstj.Sum()) << "\t " << (_nbarCi.Sum()) << endl;
    cout << "--------------------------------------------------------\n" << endl;
  }
}
