// $Id: $                                                                                             

/*!
 * \file EMCalTrk.C
 * \brief 
 * \author Jin Huang <jhuang@bnl.gov>
 * \version $Revision:   $
 * \date $Date: $
 */

#include "EMCalTrk.h"
#include <cmath>

using namespace std;

ClassImp(EMCalTrk)

EMCalTrk::EMCalTrk()
{
  Reset();
}

EMCalTrk::~EMCalTrk()
{
  // TODO Auto-generated destructor stub
}

void
EMCalTrk::Reset()
{

  for (int ieta = 0; ieta < Max_N_Tower; ++ieta)
    {
      for (int iphi = 0; iphi < Max_N_Tower; ++iphi)
        {
          cemc_ieta[ieta][iphi] = -9999;
          cemc_iphi[ieta][iphi] = -9999;
          hcalin_ieta[ieta][iphi] = -9999;
          hcalin_iphi[ieta][iphi] = -9999;
          cemc_energy[ieta][iphi] = -0;
          hcalin_energy[ieta][iphi] = -0;
          cemc_radius[ieta][iphi] = 9999;
          hcalin_radius[ieta][iphi] = 9999;
        }
    }

  trackID = -1;
  charge = -999;
  quality = NAN;
  chisq = NAN;
  ndf = NAN;
  nhits = NAN;
  layers = 0;

  dca2d = NAN;
  dca2dsigma = NAN;
  px = NAN;
  py = NAN;
  pz = NAN;
  pcax = NAN;
  pcay = NAN;
  pcaz = NAN;

  presdphi = NAN;
  presdeta = NAN;
  prese3x3 = NAN;
  prese = NAN;

  cemcdphi = NAN;
  cemcdeta = NAN;
  cemce3x3 = NAN;
  cemce = NAN;

  hcalindphi = NAN;
  hcalindeta = NAN;
  hcaline3x3 = NAN;
  hcaline = NAN;

  hcaloutdphi = NAN;
  hcaloutdeta = NAN;
  hcaloute3x3 = NAN;
  hcaloute = NAN;

  gtrackID = -1;
  gflavor = -1;
  ng4hits = -1;
  gpx = NAN;
  gpy = NAN;
  gpz = NAN;
  gvx = NAN;
  gvy = NAN;
  gvz = NAN;
  gfpx = NAN;
  gfpy = NAN;
  gfpz = NAN;
  gfx = NAN;
  gfy = NAN;
  gfz = NAN;
  gembed = -1;

  nfromtruth = -1;

  ll_ep_e = NAN;
  ll_ep_h = NAN;

  ll_edep_e = NAN;
  ll_edep_h = NAN;

  ll_shape_e = NAN;
  ll_shape_h = NAN;

  cemc_sum_energy = 0;
  cemc_sum_n_tower = 0;
  hcalin_sum_energy = 0;
  hcalin_sum_n_tower = 0;

}

void
EMCalTrk::Clear(Option_t *option)
{
  PHObject::Clear(option);

  Reset();
}

//! e/p based on CEMC and tracking
double
EMCalTrk::get_ep() const
{
  const double p2 = px * px + py * py + pz * pz;
  return cemc_sum_energy / sqrt(p2);
}
