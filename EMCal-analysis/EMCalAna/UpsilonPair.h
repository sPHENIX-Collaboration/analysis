// $Id: $                                                                                             

/*!
 * \file UpsilonPair.h
 * \brief 
 * \author Jin Huang <jhuang@bnl.gov>
 * \version $Revision:   $
 * \date $Date: $
 */

#ifndef UPSILONRECO_H_
#define UPSILONRECO_H_

#include <phool/PHObject.h>
#include <phool/phool.h>

class TClonesArray;
class EMCalTrk;

/*!
 * \brief UpsilonPair
 */
class UpsilonPair : public PHObject
{
public:
  UpsilonPair();
  virtual
  ~UpsilonPair();
  virtual void
  Reset();

  EMCalTrk *
  get_trk( int i);

public:

  float gmass;
  float mass;
  bool  good_upsilon;

  enum {N_trk = 2};

//  EMCalTrk trk[N_trk];
  TClonesArray * trk;

  ClassDef(UpsilonPair,1)
};

#endif /* UPSILONRECO_H_ */
