// $Id: $                                                                                             

/*!
 * \file PhotonPair.C
 * \brief 
 * \author Jin Huang <jhuang@bnl.gov>
 * \version $Revision:   $
 * \date $Date: $
 */

#include "PhotonPair.h"
#include <cmath>

using namespace std;

ClassImp(PhotonPair)

PhotonPair::PhotonPair()
{
  Reset();
}

PhotonPair::~PhotonPair()
{
  // TODO Auto-generated destructor stub
}

void
PhotonPair::Reset()
{

  gmass = NAN;
  mass = NAN;
  good_upsilon = false;

}

void
PhotonPair::Clear(Option_t *option)
{
  PHObject::Clear(option);

  Reset();
}
