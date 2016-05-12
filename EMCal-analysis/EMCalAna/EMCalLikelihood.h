// $Id: $                                                                                             

/*!
 * \file EMCalLikelihood.h
 * \brief 
 * \author Jin Huang <jhuang@bnl.gov>
 * \version $Revision:   $
 * \date $Date: $
 */

#ifndef EMCALLIKELIHOOD_H_
#define EMCALLIKELIHOOD_H_

#include <fun4all/SubsysReco.h>
#include <phool/PHCompositeNode.h>
#include <TNtuple.h>
#include <TFile.h>

#include <string>
#include <stdint.h>
#include <utility>      // std::pair, std::make_pair

class EMCalTrk;
class TH2;
class TH1;
class Fun4AllHistoManager;

/*!
 * \brief EMCalLikelihood
 */
class EMCalLikelihood : public SubsysReco
{
public:
  EMCalLikelihood(const std::string &filename);
  virtual
  ~EMCalLikelihood();

  int
  Init(PHCompositeNode *topNode);
  int
  InitRun(PHCompositeNode *topNode);
  int
  process_event(PHCompositeNode *topNode);
  int
  End(PHCompositeNode *topNode);

  Fun4AllHistoManager *
  get_HistoManager();

  void
  ApplyEMCalGanging(EMCalTrk * trk);

  void
  UpdateEnergyDeposition(EMCalTrk * trk);

  void
  UpdateEnergyDepositionLikelihood(EMCalTrk * trk);

  TH2*
  get_h2_Edep_Distribution_e() const
  {
    return h2_Edep_Distribution_e;
  }

  void
  set_h2_Edep_Distribution_e(TH2* h2EdepDistributionE)
  {
    h2_Edep_Distribution_e = h2EdepDistributionE;
  }

  TH2*
  get_h2_Edep_Distribution_pi() const
  {
    return h2_Edep_Distribution_pi;
  }

  void
  set_h2_Edep_Distribution_pi(TH2* h2EdepDistributionPi)
  {
    h2_Edep_Distribution_pi = h2EdepDistributionPi;
  }

  double
  get_center_cemc_ieta() const
  {
    return center_cemc_ieta;
  }

  void
  set_center_cemc_ieta(double centerCemcIeta)
  {
    center_cemc_ieta = centerCemcIeta;
  }

  double
  get_center_cemc_iphi() const
  {
    return center_cemc_iphi;
  }

  void
  set_center_cemc_iphi(double centerCemcIphi)
  {
    center_cemc_iphi = centerCemcIphi;
  }

  double
  get_center_hcalin_ieta() const
  {
    return center_hcalin_ieta;
  }

  void
  set_center_hcalin_ieta(double centerHcalinIeta)
  {
    center_hcalin_ieta = centerHcalinIeta;
  }

  double
  get_center_hcalin_iphi() const
  {
    return center_hcalin_iphi;
  }

  void
  set_center_hcalin_iphi(double centerHcalinIphi)
  {
    center_hcalin_iphi = centerHcalinIphi;
  }

  double
  get_width_cemc_ieta() const
  {
    return width_cemc_ieta;
  }

  void
  set_width_cemc_ieta(double widthCemcIeta)
  {
    width_cemc_ieta = widthCemcIeta;
  }

  double
  get_width_cemc_iphi() const
  {
    return width_cemc_iphi;
  }

  void
  set_width_cemc_iphi(double widthCemcIphi)
  {
    width_cemc_iphi = widthCemcIphi;
  }

  double
  get_width_hcalin_ieta() const
  {
    return width_hcalin_ieta;
  }

  void
  set_width_hcalin_ieta(double widthHcalinIeta)
  {
    width_hcalin_ieta = widthHcalinIeta;
  }

  double
  get_width_hcalin_iphi() const
  {
    return width_hcalin_iphi;
  }

  void
  set_width_hcalin_iphi(double widthHcalinIphi)
  {
    width_hcalin_iphi = widthHcalinIphi;
  }

  void do_ganging(unsigned int eta, unsigned int phi)
  {
    _do_ganging = true;
    _ganging_size.first= eta;
    _ganging_size.second= phi;
  }

private:
  std::string _filename;

  int _ievent;

  EMCalTrk * _trk;

  double center_cemc_iphi;
  double center_cemc_ieta;
  double center_hcalin_iphi;
  double center_hcalin_ieta;

  double width_cemc_iphi;
  double width_cemc_ieta;
  double width_hcalin_iphi;
  double width_hcalin_ieta;

  TH2 * h2_Edep_Distribution_e;
  TH2 * h2_Edep_Distribution_pi;
  TH1 * h1_ep_Distribution_e;
  TH1 * h1_ep_Distribution_pi;

  bool _do_ganging;

  //! eta and phi ganging
  std::pair<unsigned int, unsigned int> _ganging_size;
};

#endif /* EMCALLIKELIHOOD_H_ */
