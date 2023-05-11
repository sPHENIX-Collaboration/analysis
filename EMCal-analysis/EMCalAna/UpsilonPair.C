// $Id: $                                                                                             

/*!
 * \file UpsilonPair.C
 * \brief 
 * \author Jin Huang <jhuang@bnl.gov>
 * \version $Revision:   $
 * \date $Date: $
 */

#include "UpsilonPair.h"
#include "EMCalTrk.h"
#include <cmath>
#include <cassert>

#include <TClonesArray.h>

ClassImp(UpsilonPair)

using namespace std;

UpsilonPair::UpsilonPair()
{

  trk = new TClonesArray("EMCalTrk", N_trk);

  Reset();
}

UpsilonPair::~UpsilonPair()
{
  assert(trk);
  trk->Clear();
  delete trk;
}

void
UpsilonPair::Reset()
{
  gmass = NAN;
  mass = NAN;
  good_upsilon = false;

  assert(trk);
  trk->Clear();
  trk->ExpandCreate(N_trk);

  for (int i = 0; i < N_trk; ++i)
    {
      new ((*trk)[i]) EMCalTrk();

      EMCalTrk * t = dynamic_cast<EMCalTrk *>(trk->At(i));
      assert(t);
    }

}

EMCalTrk *
UpsilonPair::get_trk(int i)
{
  assert(i>=0);
  assert(i<N_trk);

  EMCalTrk * t = dynamic_cast<EMCalTrk *>(trk->At(i));
  assert(t);

  return t;
}
