//=====================================================================-*-C++-*-
// File and Version Information:
//      $Id: RooUnfoldSvd.h 296 2011-09-30 00:46:54Z T.J.Adye $
//
// Description:
//      SVD unfolding. Just an interface to RooUnfHistoSvd.
//
// Author: Tim Adye <T.J.Adye@rl.ac.uk>
//
//==============================================================================

#ifndef ROOUNFOLDSVD_HH
#define ROOUNFOLDSVD_HH

#include "RooUnfold.h"

class RooUnfoldResponse;
class TH1;
class TH1D;
class TH2D;
class TSVDUnfold;

class RooUnfoldSvd : public RooUnfold {

public:

  // Standard methods

  RooUnfoldSvd(); // default constructor
  RooUnfoldSvd (const char*    name, const char*    title); // named constructor
  RooUnfoldSvd (const TString& name, const TString& title); // named constructor
  RooUnfoldSvd (const RooUnfoldSvd& rhs); // copy constructor
  virtual ~RooUnfoldSvd(); // destructor
  RooUnfoldSvd& operator= (const RooUnfoldSvd& rhs); // assignment operator
  virtual RooUnfoldSvd* Clone (const char* newname= 0) const;

  // Special constructors

  RooUnfoldSvd (const RooUnfoldResponse* res, const TH1* meas, Int_t kreg= 0, Int_t ntoyssvd= 1000,
                const char* name= 0, const char* title= 0);

  void SetKterm (Int_t kreg);
  void SetNtoysSVD (Int_t ntoyssvd);
  Int_t GetKterm() const;
  Int_t GetNtoysSVD() const;

  virtual void  SetRegParm (Double_t parm);
  virtual Double_t GetRegParm() const;
  virtual void Reset();
  TSVDUnfold* Impl();

protected:
  void Assign (const RooUnfoldSvd& rhs); // implementation of assignment operator
  virtual void Unfold();
  virtual void GetCov();
  virtual void GetSettings();

private:
  void Init();
  void Destroy();
  void CopyData (const RooUnfoldSvd& rhs);

protected:
  // instance variables
  TSVDUnfold* _svd;  //! Implementation in TSVDUnfold object (no streamer)
  Int_t _kreg;
  Int_t _nb;
  Int_t _ntoyssvd;

  TH1D *_meas1d, *_train1d, *_truth1d;
  TH2D *_reshist;

public:
  ClassDef (RooUnfoldSvd, 1) // SVD Unfolding (interface to TSVDUnfold)
};

// Inline method definitions

inline
RooUnfoldSvd::RooUnfoldSvd()
  : RooUnfold()
{
  // Default constructor. Use Setup() to prepare for unfolding.
  Init();
}

inline
RooUnfoldSvd::RooUnfoldSvd (const char* name, const char* title)
  : RooUnfold(name,title)
{
  // Basic named constructor. Use Setup() to prepare for unfolding.
  Init();
}

inline
RooUnfoldSvd::RooUnfoldSvd (const TString& name, const TString& title)
  : RooUnfold(name,title)
{
  // Basic named constructor. Use Setup() to prepare for unfolding.
  Init();
}

inline
RooUnfoldSvd& RooUnfoldSvd::operator= (const RooUnfoldSvd& rhs)
{
  // Assignment operator for copying RooUnfoldSvd settings.
  Assign(rhs);
  return *this;
}

inline
RooUnfoldSvd::~RooUnfoldSvd()
{
  Destroy();
}


inline
void RooUnfoldSvd::SetKterm (Int_t kreg)
{
  // Set regularisation parameter
  _kreg= kreg;
}

inline
void RooUnfoldSvd::SetNtoysSVD (Int_t ntoyssvd)
{
  // Set number of toys for error calculation
  _ntoyssvd= ntoyssvd;
}

inline
Int_t RooUnfoldSvd::GetKterm() const
{
  // Return regularisation parameter
  return _kreg;
}

inline
Int_t RooUnfoldSvd::GetNtoysSVD() const
{
  // Return number of toys for error calculation
  return _ntoyssvd;
}


inline
void  RooUnfoldSvd::SetRegParm (Double_t parm)
{
  // Set regularisation parameter
  SetKterm(Int_t(parm+0.5));
}

inline
Double_t RooUnfoldSvd::GetRegParm() const
{
  // Return regularisation parameter
  return GetKterm();
}

#endif
