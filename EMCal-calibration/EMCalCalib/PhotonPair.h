// $Id: $                                                                                             

/*!
 * \file PhotonPair.h
 * \brief 
 * \author Jin Huang <jhuang@bnl.gov>
 * \version $Revision:   $
 * \date $Date: $
 */

#ifndef PhotonPair_H_
#define PhotonPair_H_

#include <phool/PHObject.h>
#include <phool/phool.h>

/*!
 * \brief PhotonPair
 */
class PhotonPair : public PHObject
{
public:
  PhotonPair();
  virtual
  ~PhotonPair();

  virtual void
  Clear(Option_t *option = "");

  virtual void
  Reset();

  float gmass;
  float mass;
  bool  good_upsilon;

ClassDef(PhotonPair,9)
};

#endif /* PhotonPair_H_ */
