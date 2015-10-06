// $Id: $                                                                                             

/*!
 * \file EMCalTrk.C
 * \brief 
 * \author Jin Huang <jhuang@bnl.gov>
 * \version $Revision:   $
 * \date $Date: $
 */

#include "EMCalTrk.h"

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
        }
    }

}

void
EMCalTrk::Clear(Option_t *option)
{
  PHObject::Clear(option);

  Reset();
}
