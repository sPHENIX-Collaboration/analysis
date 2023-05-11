#ifndef __HCALANALYSIS_H__
#define __HCALANALYSIS_H__

#include <fun4all/SubsysReco.h>
#include <string>

class PHCompositeNode;
class PHG4HitContainer;
class TFile;
class TNtuple; 

class HCALAnalysis: public SubsysReco
{
 public:
  HCALAnalysis(const char *outfile = "ntuple.root");
  virtual ~HCALAnalysis() {}

  //  For this analysis we only use Init, process_event;
  int Init         (PHCompositeNode *topNode);
  int process_event(PHCompositeNode *topNode);
  int End          (PHCompositeNode *topNode);

 protected:

  const char *OutFileName;

  void GetNodes(PHCompositeNode *topNode);

  PHCompositeNode* _topNode;
  PHG4HitContainer* _hcalout_hit_container; 
  PHG4HitContainer* _hcalin_hit_container; 
  PHG4HitContainer* _cemc_hit_container; 
  PHG4HitContainer* _hcalout_abs_hit_container; 
  PHG4HitContainer* _hcalin_abs_hit_container; 
  PHG4HitContainer* _cemc_abs_hit_container; 
  PHG4HitContainer* _magnet_hit_container; 
  PHG4HitContainer* _bh_hit_container; 
  PHG4HitContainer* _bh_plus_hit_container; 
  PHG4HitContainer* _bh_minus_hit_container; 
  PHG4HitContainer* _cemc_electronics_hit_container; 
  PHG4HitContainer* _hcalin_spt_hit_container; 
  PHG4HitContainer* _svtx_hit_container; 
  PHG4HitContainer* _svtx_support_hit_container; 
  
  TFile *outputfile; 
  TNtuple *calenergy; 

};

#endif /* __HCALANALYSIS_H__ */
