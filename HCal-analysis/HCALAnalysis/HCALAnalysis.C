//  General sPHENIX tools
#include "phool/PHCompositeNode.h"
#include "phool/phool.h"
#include "fun4all/getClass.h"
#include "HCALAnalysis.h"

#include "g4main/PHG4HitContainer.h"
#include "g4main/PHG4Hit.h"

//  Root classes
#include "TH1.h"
#include "TH2.h"
#include "TPythia6.h"
#include "TMath.h"
#include "TFile.h"
#include "TNtuple.h"
#include <TLorentzVector.h>

#include <assert.h>


HCALAnalysis::HCALAnalysis(const char *outfile) : SubsysReco("HCAL Analysis Development Code")
{
  OutFileName = outfile;

  return;
}

int HCALAnalysis::Init(PHCompositeNode *topNode)
{

  outputfile = new TFile(OutFileName,"RECREATE");

  calenergy = new TNtuple("calenergy","calenergy","e_hcin:e_hcout:e_cemc:ea_hcin:ea_hcout:ea_cemc:ev_hcin:ev_hcout:ev_cemc:e_magnet:e_bh:e_emcelect:e_hcalin_spt"); 

  return 0; 
}

int HCALAnalysis::process_event(PHCompositeNode *topNode)
{
  GetNodes(topNode);

  /*
  std::cout << " This event has " << _hcalout_hit_container->size() << " HCALOUT G4 hits" << std::endl; 
  std::cout << " This event has " << _hcalin_hit_container->size() << " HCALIN G4 hits" << std::endl; 
  std::cout << " This event has " << _cemc_hit_container->size() << " CEMC G4 hits" << std::endl; 

  std::cout << " This event has " << _hcalout_abs_hit_container->size() << " ABSORBER_HCALOUT G4 hits" << std::endl; 
  std::cout << " This event has " << _hcalin_abs_hit_container->size() << " ABSORBER_HCALIN G4 hits" << std::endl; 
  std::cout << " This event has " << _cemc_abs_hit_container->size() << " ABSORBER_CEMC G4 hits" << std::endl; 
  */

  // Sum up energy in scintillator, absorber, and light yield for 
  // the three calorimeter sections. 

  float e_hcin = 0.0, e_hcout = 0.0, e_cemc = 0.0; 
  float ea_hcin = 0.0, ea_hcout = 0.0, ea_cemc = 0.0; 
  float ev_hcin = 0.0, ev_hcout = 0.0, ev_cemc = 0.0; 
  float e_magnet = 0.0, e_bh = 0.0; 
  float e_emcelect = 0.0, e_hcalin_spt = 0.0; 

  PHG4HitContainer::ConstRange hcalout_hit_range = _hcalout_hit_container->getHits();
  for (PHG4HitContainer::ConstIterator hit_iter = hcalout_hit_range.first;
       hit_iter != hcalout_hit_range.second; hit_iter++){

    PHG4Hit *this_hit =  hit_iter->second;
    assert(this_hit); 

    e_hcout += this_hit->get_edep(); 
    ev_hcout += this_hit->get_light_yield();

  }

  PHG4HitContainer::ConstRange hcalin_hit_range = _hcalin_hit_container->getHits();
  for (PHG4HitContainer::ConstIterator hit_iter = hcalin_hit_range.first;
       hit_iter != hcalin_hit_range.second; hit_iter++){

    PHG4Hit *this_hit =  hit_iter->second;
    assert(this_hit); 

    e_hcin += this_hit->get_edep(); 
    ev_hcin += this_hit->get_light_yield();

  }

  PHG4HitContainer::ConstRange cemc_hit_range = _cemc_hit_container->getHits();
  for (PHG4HitContainer::ConstIterator hit_iter = cemc_hit_range.first;
       hit_iter != cemc_hit_range.second; hit_iter++){

    PHG4Hit *this_hit =  hit_iter->second;
    assert(this_hit); 

    e_cemc += this_hit->get_edep(); 
    ev_cemc += this_hit->get_light_yield();

  }

  PHG4HitContainer::ConstRange hcalout_abs_hit_range = _hcalout_abs_hit_container->getHits();
  for (PHG4HitContainer::ConstIterator hit_iter = hcalout_abs_hit_range.first;
       hit_iter != hcalout_abs_hit_range.second; hit_iter++){

    PHG4Hit *this_hit =  hit_iter->second;
    assert(this_hit); 

    ea_hcout += this_hit->get_edep(); 

  }

  PHG4HitContainer::ConstRange hcalin_abs_hit_range = _hcalin_abs_hit_container->getHits();
  for (PHG4HitContainer::ConstIterator hit_iter = hcalin_abs_hit_range.first;
       hit_iter != hcalin_abs_hit_range.second; hit_iter++){

    PHG4Hit *this_hit =  hit_iter->second;
    assert(this_hit); 

    ea_hcin += this_hit->get_edep(); 

  }

  PHG4HitContainer::ConstRange cemc_abs_hit_range = _cemc_abs_hit_container->getHits();
  for (PHG4HitContainer::ConstIterator hit_iter = cemc_abs_hit_range.first;
       hit_iter != cemc_abs_hit_range.second; hit_iter++){

    PHG4Hit *this_hit =  hit_iter->second;
    assert(this_hit); 

    ea_cemc += this_hit->get_edep(); 

  }

  PHG4HitContainer::ConstRange magnet_hit_range = _magnet_hit_container->getHits();
  for (PHG4HitContainer::ConstIterator hit_iter = magnet_hit_range.first;
       hit_iter != magnet_hit_range.second; hit_iter++){

    PHG4Hit *this_hit =  hit_iter->second;
    assert(this_hit); 

    e_magnet += this_hit->get_edep(); 

  }

  PHG4HitContainer::ConstRange bh_hit_range = _bh_hit_container->getHits();
  for (PHG4HitContainer::ConstIterator hit_iter = bh_hit_range.first;
       hit_iter != bh_hit_range.second; hit_iter++){

    PHG4Hit *this_hit =  hit_iter->second;
    assert(this_hit); 

    e_bh += this_hit->get_edep(); 

  }

  PHG4HitContainer::ConstRange cemc_electronics_hit_range = _cemc_electronics_hit_container->getHits();
  for (PHG4HitContainer::ConstIterator hit_iter = cemc_electronics_hit_range.first;
       hit_iter != cemc_electronics_hit_range.second; hit_iter++){

    PHG4Hit *this_hit =  hit_iter->second;
    assert(this_hit); 

    e_emcelect += this_hit->get_edep(); 

  }

  PHG4HitContainer::ConstRange hcalin_spt_hit_range = _hcalin_spt_hit_container->getHits();
  for (PHG4HitContainer::ConstIterator hit_iter = hcalin_spt_hit_range.first;
       hit_iter != hcalin_spt_hit_range.second; hit_iter++){

    PHG4Hit *this_hit =  hit_iter->second;
    assert(this_hit); 

    e_hcalin_spt += this_hit->get_edep(); 

  }

  // Fill the ntuple

  float ntdata[13]; 
 
  ntdata[0] = e_hcout; 
  ntdata[1] = e_hcin; 
  ntdata[2] = e_cemc; 
  ntdata[3] = ea_hcout; 
  ntdata[4] = ea_hcin; 
  ntdata[5] = ea_cemc; 
  ntdata[6] = ev_hcout; 
  ntdata[7] = ev_hcin; 
  ntdata[8] = ev_cemc; 
  ntdata[9] = e_magnet; 
  ntdata[10] = e_bh; 
  ntdata[11] = e_emcelect; 
  ntdata[12] = e_hcalin_spt; 

  calenergy->Fill(ntdata); 
  
  // any other return code might lead to aborting the event or analysis for everyone
  return 0;
}

int HCALAnalysis::End(PHCompositeNode *topNode)
{

  outputfile->Write();
  outputfile->Close();

  return 0;
}

void HCALAnalysis::GetNodes(PHCompositeNode *topNode)
{

  // store top node
  _topNode = topNode;

  // get DST objects
  _hcalout_hit_container = findNode::getClass<PHG4HitContainer> (topNode, "G4HIT_HCALOUT");
  if(!_hcalout_hit_container) { std::cout << PHWHERE << ":: No G4HIT_HCALOUT! No sense continuing" << std::endl; exit(1);}

  _hcalin_hit_container = findNode::getClass<PHG4HitContainer> (topNode, "G4HIT_HCALIN");
  if(!_hcalin_hit_container) { std::cout << PHWHERE << ":: No G4HIT_HCALIN! No sense continuing" << std::endl; exit(1);}

  _cemc_hit_container = findNode::getClass<PHG4HitContainer> (topNode, "G4HIT_CEMC");
  if(!_cemc_hit_container) { std::cout << PHWHERE << ":: No G4HIT_CEMC! No sense continuing" << std::endl; exit(1);}

  _hcalout_abs_hit_container = findNode::getClass<PHG4HitContainer> (topNode, "G4HIT_ABSORBER_HCALOUT");
  if(!_hcalout_abs_hit_container) { std::cout << PHWHERE << ":: No G4HIT_HCALOUT! No sense continuing" << std::endl; exit(1);}

  _hcalin_abs_hit_container = findNode::getClass<PHG4HitContainer> (topNode, "G4HIT_ABSORBER_HCALIN");
  if(!_hcalin_abs_hit_container) { std::cout << PHWHERE << ":: No G4HIT_HCALIN! No sense continuing" << std::endl; exit(1);}

  _cemc_abs_hit_container = findNode::getClass<PHG4HitContainer> (topNode, "G4HIT_ABSORBER_CEMC");
  if(!_cemc_abs_hit_container) { std::cout << PHWHERE << ":: No G4HIT_CEMC! No sense continuing" << std::endl; exit(1);}

  _magnet_hit_container = findNode::getClass<PHG4HitContainer> (topNode, "G4HIT_MAGNET_0");
  if(!_magnet_hit_container) { std::cout << PHWHERE << ":: No G4HIT_MAGNET_0! No sense continuing" << std::endl; exit(1);}

  _bh_hit_container = findNode::getClass<PHG4HitContainer> (topNode, "G4HIT_BH_1");
  if(!_bh_hit_container) { std::cout << PHWHERE << ":: No G4HIT_BH_1! No sense continuing" << std::endl; exit(1);}

  _cemc_electronics_hit_container = findNode::getClass<PHG4HitContainer> (topNode, "G4HIT_EMCELECTRONICS_0");
  if(!_cemc_electronics_hit_container) { std::cout << PHWHERE << ":: No G4HIT_EMCELECTRONICS_0! No sense continuing" << std::endl; exit(1);}

  _hcalin_spt_hit_container = findNode::getClass<PHG4HitContainer> (topNode, "G4HIT_HCALIN_SPT");
  if(!_hcalin_spt_hit_container) { std::cout << PHWHERE << ":: No G4HIT_HCALIN_SPT! No sense continuing" << std::endl; exit(1);}

  return;
}

