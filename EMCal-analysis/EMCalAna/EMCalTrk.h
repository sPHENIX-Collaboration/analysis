// $Id: $                                                                                             

/*!
 * \file EMCalTrk.h
 * \brief 
 * \author Jin Huang <jhuang@bnl.gov>
 * \version $Revision:   $
 * \date $Date: $
 */

#ifndef EMCALTRK_H_
#define EMCALTRK_H_

#include <phool/PHObject.h>
#include <phool/phool.h>

/*!
 * \brief EMCalTrk
 */
class EMCalTrk : public PHObject
{
public:
  EMCalTrk();
  virtual
  ~EMCalTrk();

  virtual void
  Clear(Option_t *option = "");

  virtual void
  Reset();

  int trackID;
  int charge;
  float quality;
  float chisq;
  float ndf;
  float nhits;
  unsigned int layers;

  float dca;
  float dca2d;
  float dca2dsigma;
  float px;
  float py;
  float pz;
  float pcax;
  float pcay;
  float pcaz;

  float presdphi;
  float presdeta;
  float prese3x3;
  float prese;

  float cemcdphi;
  float cemcdeta;
  float cemce3x3;
  float cemce;

  float hcalindphi;
  float hcalindeta;
  float hcaline3x3;
  float hcaline;

  float hcaloutdphi;
  float hcaloutdeta;
  float hcaloute3x3;
  float hcaloute;

  int gtrackID;
  int gflavor;
  int ng4hits;

  float gpx;
  float gpy;
  float gpz;
  float gvx;
  float gvy;
  float gvz;
  float gfpx;
  float gfpy;
  float gfpz;
  float gfx;
  float gfy;
  float gfz;
  int gembed;

  int nfromtruth;

  enum
  {
    Max_N_Tower = 11
  };

  float cemc_ieta[Max_N_Tower][Max_N_Tower];
  float cemc_iphi[Max_N_Tower][Max_N_Tower];
  float cemc_energy[Max_N_Tower][Max_N_Tower];
  float cemc_radius[Max_N_Tower][Max_N_Tower];

  float hcalin_ieta[Max_N_Tower][Max_N_Tower];
  float hcalin_iphi[Max_N_Tower][Max_N_Tower];
  float hcalin_energy[Max_N_Tower][Max_N_Tower];
  float hcalin_radius[Max_N_Tower][Max_N_Tower];

  float cemc_sum_energy;
  int cemc_sum_n_tower;
  float hcalin_sum_energy;
  int hcalin_sum_n_tower;

  //! e/p based on CEMC and tracking
  double
  get_ep() const;

  //! log-likelihood electron - ep matching
  float ll_ep_e;

  //! log-likelihood hadron - ep matching
  float ll_ep_h;

  //! log-likelihood electron - emcal + inner HCal energy deposition
  float ll_edep_e;

  //! log-likelihood hadron - emcal + inner HCal energy deposition
  float ll_edep_h;

  //! log-likelihood electron - shower shape
  float ll_shape_e;

  //! log-likelihood hadron - shower shape
  float ll_shape_h;

ClassDef(EMCalTrk,10)
};

#endif /* EMCALTRK_H_ */
