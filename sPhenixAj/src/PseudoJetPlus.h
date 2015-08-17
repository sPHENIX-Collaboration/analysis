/** @file PseudoJetPlus.hh
    @author Kauder:Kolja
    @version Revision 0.1
    @brief Light wrapper to hand tracks, towers, etc. to fastjet 
    while retaining information
    @date Jul 30, 2015
*/

#ifndef PSEUDOJETPLUS_H
#define  PSEUDOJETPLUS_H

#include <fastjet/PseudoJet.hh>
#include "TString.h"


/**
   Userinfo base, for all kinds of constituents.
   Should have a type identifier, to ascertain what info is available,
   plus some things that should be defined for all kinds.
 */
class PseudoJetPlusUserInfoBase: public fastjet::PseudoJet::UserInfoBase {
public:
  /// Standard Constructor
 PseudoJetPlusUserInfoBase( int qc, TString Type) : 
  quarkcharge( qc ),
    Type ( Type  )
    {  };
  
  /// Get charge in units of e/3
  int GetQuarkCharge() const { return quarkcharge; };
  /// Set charge in units of e/3
  void SetQuarkCharge( const int qc ) { quarkcharge=qc; };

  /// Set charge in units of e
  double GetCharge() const { return quarkcharge / 3.0; };
  /// Set charge in units of e
  void SetCharge( const double c ) { quarkcharge = 3*c; };

  /// Type. Using, e.g., TString::Contains() 
  /// one can find out what more info is available
  TString GetType() { return Type; };

 private:
  int quarkcharge;   ///< Charge in units of e/3
  const TString Type; ///< Could use something like an enum instead
};

/**
   Userinfo for tracks
 */
class PseudoJetPlusUserInfoTrack : public PseudoJetPlusUserInfoBase {
public:
  /// Standard Constructor
 PseudoJetPlusUserInfoTrack() : 
  PseudoJetPlusUserInfoBase( -999, "TRACK" )
    {  };
  
 private:
  /// @todo: should have things like NFit, DCA, chi^2, ...
};


class PseudoJetPlus : public fastjet::PseudoJet
{
 public:
  PseudoJetPlus();
  
 private:
  
};



#endif //  PSEUDOJETPLUS_H
