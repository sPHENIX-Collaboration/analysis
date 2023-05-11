#ifndef PJTRANSLATOR_H
#define PJTRANSLATOR_H

#include <fun4all/SubsysReco.h>
#include "phool/PHObject.h"
#include <fun4all/Fun4AllReturnCodes.h>

#include <map>
#include <set>
#include <string>
#include <vector>

#ifndef __CINT__
#include <fastjet/PseudoJet.hh>
class PJContainer : public PHObject
{
 public : 
  std::vector<fastjet::PseudoJet> data;
};
#endif // __CINT__


class PHCompositeNode;
class RawClusterContainer;
class SvtxTrackMap;
class TF1;

class PJTranslator: public SubsysReco 
{
 public:

  PJTranslator(const std::string &name = "PJTranslator");

  virtual ~PJTranslator();

  int Init(PHCompositeNode *);

  int process_event(PHCompositeNode *);

  int get_matched(double clus_energy, double track_energy);

  private:

#ifndef __CINT__
  PJContainer *particles;
#endif// __CINT__
			 
  //Sampling Fractions
  static const float sfEMCAL = 0.03;
  static const float sfHCALIN = 0.071;
  static const float sfHCALOUT = 0.04;
			 
  //Tolerance for cluster-track matching
  TF1 *match_tolerance_low;
  TF1 *match_tolerance_high;

};


#endif // PJTRANSLATOR_H
