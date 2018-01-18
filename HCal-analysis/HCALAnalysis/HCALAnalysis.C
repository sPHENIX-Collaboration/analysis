//  General sPHENIX tools

#include "HCALAnalysis.h"

#include <calobase/RawTowerContainer.h>
#include <pdbcalbase/PdbParameterMap.h>
#include <phparameter/PHParameters.h>
#include <ffaobjects/EventHeader.h>
#include <g4detectors/PHG4ScintillatorSlat.h>
#include <g4detectors/PHG4ScintillatorSlatContainer.h>
#include <g4detectors/PHG4ScintillatorSlatDefs.h>

#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/PHTFileServer.h>
#include <phool/PHCompositeNode.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/getClass.h>
#include <fun4all/Fun4AllHistoManager.h>
#include <phool/getClass.h>


#include <g4main/PHG4HitContainer.h>
#include <g4main/PHG4Hit.h>

//  Root classes
#include <TH1.h>
#include <TH2.h>
#include <TPythia6.h>
#include <TMath.h>
#include <TFile.h>
#include <TNtuple.h>
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

  calenergy = new TNtuple("calenergy","calenergy","e_hcin:e_hcout:e_cemc:ea_hcin:ea_hcout:ea_cemc:ev_hcin:ev_hcout:ev_cemc:e_magnet:e_bh:e_emcelect:e_hcalin_spt:sfemc:sfihcal:sfohcal:sfvemc:sfvihcal:sfvohcal:LCG:emc_LCG:hcalin_LCG:hcalout_LCG:e_svtx:e_svtx_support:e_bh_plus:e_bh_minus"); 

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
  float e_svtx = 0.0, e_svtx_support = 0.0; 
  float e_bh_plus = 0.0, e_bh_minus = 0.0; 

  float lcg = 0.0; //longitudinal center of gravity
  float emc_lcg = 0.0; 
  float hcalin_lcg = 0.0; 
  float hcalout_lcg = 0.0; 

  PHG4HitContainer::ConstRange hcalout_hit_range = _hcalout_hit_container->getHits();
  for (PHG4HitContainer::ConstIterator hit_iter = hcalout_hit_range.first;
       hit_iter != hcalout_hit_range.second; hit_iter++){

    PHG4Hit *this_hit =  hit_iter->second;
    assert(this_hit); 

    e_hcout += this_hit->get_edep(); 
    ev_hcout += this_hit->get_light_yield();

    float rin =  sqrt(pow(this_hit->get_x(0),2)+pow(this_hit->get_y(0),2)); 
    float rout =  sqrt(pow(this_hit->get_x(1),2)+pow(this_hit->get_y(1),2)); 
    
    lcg += this_hit->get_edep()*(rin + ((rout-rin)/2.0)); 
    hcalout_lcg += this_hit->get_edep()*(rin + ((rout-rin)/2.0)); 
    

  }

  PHG4HitContainer::ConstRange hcalin_hit_range = _hcalin_hit_container->getHits();
  for (PHG4HitContainer::ConstIterator hit_iter = hcalin_hit_range.first;
       hit_iter != hcalin_hit_range.second; hit_iter++){

    PHG4Hit *this_hit =  hit_iter->second;
    assert(this_hit); 

    e_hcin += this_hit->get_edep(); 
    ev_hcin += this_hit->get_light_yield();

    float rin =  sqrt(pow(this_hit->get_x(0),2)+pow(this_hit->get_y(0),2)); 
    float rout =  sqrt(pow(this_hit->get_x(1),2)+pow(this_hit->get_y(1),2)); 
    
    lcg += this_hit->get_edep()*(rin + ((rout-rin)/2.0)); 
    hcalin_lcg += this_hit->get_edep()*(rin + ((rout-rin)/2.0)); 

  }

  PHG4HitContainer::ConstRange cemc_hit_range = _cemc_hit_container->getHits();
  for (PHG4HitContainer::ConstIterator hit_iter = cemc_hit_range.first;
       hit_iter != cemc_hit_range.second; hit_iter++){

    PHG4Hit *this_hit =  hit_iter->second;
    assert(this_hit); 

    e_cemc += this_hit->get_edep(); 
    ev_cemc += this_hit->get_light_yield();

    float rin =  sqrt(pow(this_hit->get_x(0),2)+pow(this_hit->get_y(0),2)); 
    float rout =  sqrt(pow(this_hit->get_x(1),2)+pow(this_hit->get_y(1),2)); 
    
    lcg += this_hit->get_edep()*(rin + ((rout-rin)/2.0)); 
    emc_lcg += this_hit->get_edep()*(rin + ((rout-rin)/2.0)); 

  }

  PHG4HitContainer::ConstRange hcalout_abs_hit_range = _hcalout_abs_hit_container->getHits();
  for (PHG4HitContainer::ConstIterator hit_iter = hcalout_abs_hit_range.first;
       hit_iter != hcalout_abs_hit_range.second; hit_iter++){

    PHG4Hit *this_hit =  hit_iter->second;
    assert(this_hit); 

    ea_hcout += this_hit->get_edep(); 

    float rin =  sqrt(pow(this_hit->get_x(0),2)+pow(this_hit->get_y(0),2)); 
    float rout =  sqrt(pow(this_hit->get_x(1),2)+pow(this_hit->get_y(1),2)); 
    
    lcg += this_hit->get_edep()*(rin + ((rout-rin)/2.0)); 
    hcalout_lcg += this_hit->get_edep()*(rin + ((rout-rin)/2.0)); 

  }

  PHG4HitContainer::ConstRange hcalin_abs_hit_range = _hcalin_abs_hit_container->getHits();
  for (PHG4HitContainer::ConstIterator hit_iter = hcalin_abs_hit_range.first;
       hit_iter != hcalin_abs_hit_range.second; hit_iter++){

    PHG4Hit *this_hit =  hit_iter->second;
    assert(this_hit); 

    ea_hcin += this_hit->get_edep(); 

    float rin =  sqrt(pow(this_hit->get_x(0),2)+pow(this_hit->get_y(0),2)); 
    float rout =  sqrt(pow(this_hit->get_x(1),2)+pow(this_hit->get_y(1),2)); 
    
    lcg += this_hit->get_edep()*(rin + ((rout-rin)/2.0)); 
    hcalin_lcg += this_hit->get_edep()*(rin + ((rout-rin)/2.0)); 

  }

  PHG4HitContainer::ConstRange cemc_abs_hit_range = _cemc_abs_hit_container->getHits();
  for (PHG4HitContainer::ConstIterator hit_iter = cemc_abs_hit_range.first;
       hit_iter != cemc_abs_hit_range.second; hit_iter++){

    PHG4Hit *this_hit =  hit_iter->second;
    assert(this_hit); 

    ea_cemc += this_hit->get_edep(); 

    float rin =  sqrt(pow(this_hit->get_x(0),2)+pow(this_hit->get_y(0),2)); 
    float rout =  sqrt(pow(this_hit->get_x(1),2)+pow(this_hit->get_y(1),2)); 
    
    lcg += this_hit->get_edep()*(rin + ((rout-rin)/2.0)); 
    emc_lcg += this_hit->get_edep()*(rin + ((rout-rin)/2.0)); 

  }

  PHG4HitContainer::ConstRange magnet_hit_range = _magnet_hit_container->getHits();
  for (PHG4HitContainer::ConstIterator hit_iter = magnet_hit_range.first;
       hit_iter != magnet_hit_range.second; hit_iter++){

    PHG4Hit *this_hit =  hit_iter->second;
    assert(this_hit); 

    e_magnet += this_hit->get_edep(); 

    float rin =  sqrt(pow(this_hit->get_x(0),2)+pow(this_hit->get_y(0),2)); 
    float rout =  sqrt(pow(this_hit->get_x(1),2)+pow(this_hit->get_y(1),2)); 
    
    lcg += this_hit->get_edep()*(rin + ((rout-rin)/2.0)); 

  }

  PHG4HitContainer::ConstRange bh_hit_range = _bh_hit_container->getHits();
  for (PHG4HitContainer::ConstIterator hit_iter = bh_hit_range.first;
       hit_iter != bh_hit_range.second; hit_iter++){

    PHG4Hit *this_hit =  hit_iter->second;
    assert(this_hit); 

    e_bh += this_hit->get_edep(); 

  }

  PHG4HitContainer::ConstRange bh_plus_hit_range = _bh_plus_hit_container->getHits();
  for (PHG4HitContainer::ConstIterator hit_iter = bh_plus_hit_range.first;
       hit_iter != bh_plus_hit_range.second; hit_iter++){

    PHG4Hit *this_hit =  hit_iter->second;
    assert(this_hit); 

    e_bh_plus += this_hit->get_edep(); 

  }

  PHG4HitContainer::ConstRange bh_minus_hit_range = _bh_minus_hit_container->getHits();
  for (PHG4HitContainer::ConstIterator hit_iter = bh_minus_hit_range.first;
       hit_iter != bh_minus_hit_range.second; hit_iter++){

    PHG4Hit *this_hit =  hit_iter->second;
    assert(this_hit); 

    e_bh_minus += this_hit->get_edep(); 

  }

  PHG4HitContainer::ConstRange cemc_electronics_hit_range = _cemc_electronics_hit_container->getHits();
  for (PHG4HitContainer::ConstIterator hit_iter = cemc_electronics_hit_range.first;
       hit_iter != cemc_electronics_hit_range.second; hit_iter++){

    PHG4Hit *this_hit =  hit_iter->second;
    assert(this_hit); 

    e_emcelect += this_hit->get_edep(); 

    float rin =  sqrt(pow(this_hit->get_x(0),2)+pow(this_hit->get_y(0),2)); 
    float rout =  sqrt(pow(this_hit->get_x(1),2)+pow(this_hit->get_y(1),2)); 
    
    lcg += this_hit->get_edep()*(rin + ((rout-rin)/2.0)); 

  }

  PHG4HitContainer::ConstRange hcalin_spt_hit_range = _hcalin_spt_hit_container->getHits();
  for (PHG4HitContainer::ConstIterator hit_iter = hcalin_spt_hit_range.first;
       hit_iter != hcalin_spt_hit_range.second; hit_iter++){

    PHG4Hit *this_hit =  hit_iter->second;
    assert(this_hit); 

    e_hcalin_spt += this_hit->get_edep(); 

    float rin =  sqrt(pow(this_hit->get_x(0),2)+pow(this_hit->get_y(0),2)); 
    float rout =  sqrt(pow(this_hit->get_x(1),2)+pow(this_hit->get_y(1),2)); 
    
    lcg += this_hit->get_edep()*(rin + ((rout-rin)/2.0)); 

  }

  // SVTX

  PHG4HitContainer::ConstRange svtx_hit_range = _svtx_hit_container->getHits();
  for (PHG4HitContainer::ConstIterator hit_iter = svtx_hit_range.first;
       hit_iter != svtx_hit_range.second; hit_iter++){

    PHG4Hit *this_hit =  hit_iter->second;
    assert(this_hit); 

    e_svtx += this_hit->get_edep();    

  }

  PHG4HitContainer::ConstRange svtx_support_hit_range = _svtx_support_hit_container->getHits();
  for (PHG4HitContainer::ConstIterator hit_iter = svtx_support_hit_range.first;
       hit_iter != svtx_support_hit_range.second; hit_iter++){

    PHG4Hit *this_hit =  hit_iter->second;
    assert(this_hit); 

    e_svtx_support += this_hit->get_edep();    

  }

  //normalize the LCG
  
  lcg = lcg/(e_cemc+ea_cemc+e_emcelect+e_hcin+ea_hcin+e_hcalin_spt+e_hcout+ea_hcout+e_magnet);
  hcalin_lcg = hcalin_lcg/(e_hcin+ea_hcin);
  hcalout_lcg = hcalout_lcg/(e_hcout+ea_hcout);
  emc_lcg = emc_lcg/(e_cemc+ea_cemc);

  // Fill the ntuple

  float ntdata[27]; 
 
  ntdata[0] = e_hcin; 
  ntdata[1] = e_hcout; 
  ntdata[2] = e_cemc; 
  ntdata[3] = ea_hcin; 
  ntdata[4] = ea_hcout; 
  ntdata[5] = ea_cemc; 
  ntdata[6] = ev_hcin; 
  ntdata[7] = ev_hcout; 
  ntdata[8] = ev_cemc; 
  ntdata[9] = e_magnet; 
  ntdata[10] = e_bh; 
  ntdata[11] = e_emcelect; 
  ntdata[12] = e_hcalin_spt; 

  ntdata[13] = e_cemc/(e_cemc + ea_cemc + e_emcelect);
  ntdata[14] = e_hcin/(e_hcin + ea_hcin + e_hcalin_spt);
  ntdata[15] = e_hcout/(e_hcout + ea_hcout);

  ntdata[16] = ev_cemc/(e_cemc + ea_cemc + e_emcelect);
  ntdata[17] = ev_hcin/(e_hcin + ea_hcin + e_hcalin_spt);
  ntdata[18] = ev_hcout/(e_hcout + ea_hcout);

  ntdata[19] = lcg; 

  ntdata[20] = emc_lcg; 
  ntdata[21] = hcalin_lcg; 
  ntdata[22] = hcalout_lcg; 

  ntdata[23] = e_svtx; 
  ntdata[24] = e_svtx_support; 

  ntdata[25] = e_bh_plus; 
  ntdata[26] = e_bh_minus; 

  calenergy->Fill(ntdata); 
  
  // crosscheck

    //std::cout << std::endl; 
    //std::cout << "ev_cemc = " << ev_cemc << " e_cemc + ea_cemc + e_emcelect = " << e_cemc + ea_cemc + e_emcelect << " ev_cemc/sfvemc " << ntdata[8]/ntdata[16] << std::endl; 
    //std::cout << "ev_hcin = " << ev_hcin << " e_hcin + ea_hcin + e_hcalin_spt = " << e_hcin + ea_hcin + e_hcalin_spt << " ev_hcin/sfihcal " << ntdata[7]/ntdata[17] << std::endl; 
    //std::cout << "ev_hcout = " << ev_hcout << " e_hcout + ea_hcout = " << e_hcout + ea_hcout << " ev_hcout/sfohcal " << ntdata[6]/ntdata[18] << std::endl; 
    //std::cout << std::endl; 

  
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

  _magnet_hit_container = findNode::getClass<PHG4HitContainer> (topNode, "G4HIT_MAGNET");
  if(!_magnet_hit_container) { std::cout << PHWHERE << ":: No G4HIT_MAGNET! No sense continuing" << std::endl; exit(1);}

  _bh_hit_container = findNode::getClass<PHG4HitContainer> (topNode, "G4HIT_BH_1");
  if(!_bh_hit_container) { std::cout << PHWHERE << ":: No G4HIT_BH_1! No sense continuing" << std::endl; exit(1);}

  _cemc_electronics_hit_container = findNode::getClass<PHG4HitContainer> (topNode, "G4HIT_CEMC_ELECTRONICS");
  if(!_cemc_electronics_hit_container) { std::cout << PHWHERE << ":: No G4HIT_EMCELECTRONICS_0! No sense continuing" << std::endl; exit(1);}

  _hcalin_spt_hit_container = findNode::getClass<PHG4HitContainer> (topNode, "G4HIT_HCALIN_SPT");
  if(!_hcalin_spt_hit_container) { std::cout << PHWHERE << ":: No G4HIT_HCALIN_SPT! No sense continuing" << std::endl; exit(1);}

  _svtx_hit_container = findNode::getClass<PHG4HitContainer> (topNode, "G4HIT_SVTX");
  if(!_svtx_hit_container) { std::cout << PHWHERE << ":: No G4HIT_SVTX! No sense continuing" << std::endl; exit(1);}

  _svtx_support_hit_container = findNode::getClass<PHG4HitContainer> (topNode, "G4HIT_SVTXSUPPORT");
  if(!_svtx_support_hit_container) { std::cout << PHWHERE << ":: No G4HIT_SVTX_SUPPORT! No sense continuing" << std::endl; exit(1);}

  _bh_plus_hit_container = findNode::getClass<PHG4HitContainer> (topNode, "G4HIT_BH_FORWARD_PLUS");
  if(!_bh_plus_hit_container) { std::cout << PHWHERE << ":: No G4HIT_BH_FORWARD_PLUS! No sense continuing" << std::endl; exit(1);}

  _bh_minus_hit_container = findNode::getClass<PHG4HitContainer> (topNode, "G4HIT_BH_FORWARD_NEG");
  if(!_bh_minus_hit_container) { std::cout << PHWHERE << ":: No G4HIT_BH_FORWARD_NEG! No sense continuing" << std::endl; exit(1);}

  return;
}

