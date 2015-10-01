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

#include <TObject.h>

/*!
 * \brief EMCalTrk
 */
class EMCalTrk : public TObject
{
public:
  EMCalTrk();
  virtual
  ~EMCalTrk();

  virtual void        Clear(Option_t * /*option*/ ="") ;


  float trackID;
  float charge;
  float quality;
  float chisq;
  float ndf;
  float nhits;

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

  float gtrackID;
  float gflavor;
  float ng4hits;
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
  float gembed;
  float gprimary;

ClassDef(EMCalTrk,1)
};

#endif /* EMCALTRK_H_ */
