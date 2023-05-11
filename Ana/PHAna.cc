#include "PHAna.h"

#include <phool/phool.h>
#include <phool/getClass.h>
#include <g4main/PHG4HitContainer.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4Hit.h>
#include <g4main/PHG4VtxPoint.h>
#include <fun4all/PHTFileServer.h>
#include <fun4all/Fun4AllServer.h>

#include <TNtuple.h>
#include <TH1.h>

#include <iostream>

using namespace std;

//____________________________________
PHAna::PHAna(const string &name):
    SubsysReco(name),
    _flags( NONE )
{ 
 //initialize
 _event = 0;
 _outfile = "phana.root";
 _truth = 0;
 _sf = 0;
 _hcalin_towers = 0;
}

//___________________________________
int PHAna::Init(PHCompositeNode *topNode)
{
 cout << PHWHERE << " Openning file " << _outfile << endl;
 PHTFileServer::get().open( _outfile, "RECREATE");

 if(_flags & TRUTH) _truth = new TNtuple("truth","truth","event:Energy:Rapidity:Pt:Phi:PID");
 if(_flags & HIST ) create_histos();
 if(_flags & SF ) _sf = new TNtuple("sf","sf","event:sfhcalin:sfhcalout:sfcemc");
 return 0;
}

//__________________________________
//Call user instructions for every event
int PHAna::process_event(PHCompositeNode *topNode)
{
 _event++;
 if(_event%1000==0) cout << PHWHERE << "Events processed: " << _event << endl;
 GetNodes(topNode);
 if(_flags & TRUTH) fill_truth_ntuple(topNode);
 
 if(_flags & HIST) fill_histos(topNode);

 if(_flags & SF) fill_sf_ntuple(topNode);

 return 0;
}

//__________________________________
void PHAna::fill_histos(PHCompositeNode *topNode)
{
 Fun4AllServer *se = Fun4AllServer::instance();
 Fun4AllHistoManager *hm = se->getHistoManager("HISTOS");

 if(_truth_container)
 {
  PHG4VtxPoint *point = _truth_container->GetPrimaryVtx( _truth_container->GetPrimaryVertexIndex() );
  TH1F *h = (TH1F*) hm->getHisto("zvertex");
  if(h) h->Fill( point->get_z() );
  h = (TH1F*) hm->getHisto("xvertex");
  if(h) h->Fill( point->get_x() );
  h = (TH1F*) hm->getHisto("yvertex");
  if(h) h->Fill( point->get_y() );
 } 

}

//_____________________________________
void PHAna::fill_truth_ntuple(PHCompositeNode *topNode)
{
 map<int, PHG4Particle*>::const_iterator particle_iter;
 float ntvars[7];

 PHG4TruthInfoContainer::ConstRange primary_range =
     _truth_container->GetPrimaryParticleRange();

 for (PHG4TruthInfoContainer::ConstIterator particle_iter = primary_range.first;
     particle_iter != primary_range.second; ++particle_iter)
  {
   PHG4Particle *particle = particle_iter->second;
   ntvars[0] = _event;
   ntvars[1] = particle->get_e();
   ntvars[2] = 0.5*log((particle->get_e()+particle->get_pz())/
	   	       (particle->get_e()-particle->get_pz()));
   ntvars[3] = sqrt(Square(particle->get_px())+Square(particle->get_py()));
   ntvars[4] = atan2(particle->get_py(), particle->get_px());
   ntvars[5] = particle->get_pid();
  _truth->Fill( ntvars );
  }

}

//_____________________________________
void PHAna::fill_sf_ntuple(PHCompositeNode *topNode)
{
  //Total Outer HCal energy deposited and light yield
  float e_hcout = 0.;
  float ev_hcout = 0.;
  PHG4HitContainer::ConstRange hcalout_hit_range = _hcalout_hit_container->getHits();
   for(PHG4HitContainer::ConstIterator hit_iter = hcalout_hit_range.first;
       hit_iter != hcalout_hit_range.second; hit_iter++)
    {
     PHG4Hit *this_hit =  hit_iter->second;
     e_hcout += this_hit->get_edep();
     ev_hcout += this_hit->get_light_yield();
    }

  //Total Outer HCal absorbed energy
  float ea_hcout = 0.;
  PHG4HitContainer::ConstRange hcalout_abs_hit_range = _hcalout_abs_hit_container->getHits();
  for (PHG4HitContainer::ConstIterator hit_iter = hcalout_abs_hit_range.first;
       hit_iter != hcalout_abs_hit_range.second; hit_iter++)
   {
    PHG4Hit *this_hit =  hit_iter->second;
    ea_hcout += this_hit->get_edep();
   }
  
  //Total Inner HCal energy deposit and light yield
  float e_hcin =0.;
  float ev_hcin = 0.;
  PHG4HitContainer::ConstRange hcalin_hit_range = _hcalin_hit_container->getHits();
  for (PHG4HitContainer::ConstIterator hit_iter = hcalin_hit_range.first;
       hit_iter != hcalin_hit_range.second; hit_iter++)
   {
    PHG4Hit *this_hit =  hit_iter->second;
    e_hcin += this_hit->get_edep();
    ev_hcin += this_hit->get_light_yield();
   }

  //Total Inner HCal absorbed energy
  float ea_hcin = 0.;
  PHG4HitContainer::ConstRange hcalin_abs_hit_range = _hcalin_abs_hit_container->getHits();
  for (PHG4HitContainer::ConstIterator hit_iter = hcalin_abs_hit_range.first;
       hit_iter != hcalin_abs_hit_range.second; hit_iter++)
   {
    PHG4Hit *this_hit =  hit_iter->second;

    ea_hcin += this_hit->get_edep();
   }

  PHG4HitContainer::ConstRange hcalin_spt_hit_range = _hcalin_spt_hit_container->getHits();
  for (PHG4HitContainer::ConstIterator hit_iter = hcalin_spt_hit_range.first;
       hit_iter != hcalin_spt_hit_range.second; hit_iter++)
   {
    PHG4Hit *this_hit =  hit_iter->second;
    ea_hcin += this_hit->get_edep();
   }

  //Total EMCal deposited energy and light yield
  float e_cemc = 0.;
  float ev_cemc = 0.;
  PHG4HitContainer::ConstRange cemc_hit_range = _cemc_hit_container->getHits();
  for (PHG4HitContainer::ConstIterator hit_iter = cemc_hit_range.first;
       hit_iter != cemc_hit_range.second; hit_iter++)
   {
    PHG4Hit *this_hit =  hit_iter->second;

    e_cemc += this_hit->get_edep();
    ev_cemc += this_hit->get_light_yield();
   }

   //Total EMCal absorbed energy
   float ea_cemc = 0.;
   PHG4HitContainer::ConstRange cemc_abs_hit_range = _cemc_abs_hit_container->getHits();
  for (PHG4HitContainer::ConstIterator hit_iter = cemc_abs_hit_range.first;
       hit_iter != cemc_abs_hit_range.second; hit_iter++)
   {
    PHG4Hit *this_hit =  hit_iter->second;
    ea_cemc += this_hit->get_edep();
   }

  PHG4HitContainer::ConstRange cemc_electronics_hit_range = _cemc_electronics_hit_container->getHits();
  for (PHG4HitContainer::ConstIterator hit_iter = cemc_electronics_hit_range.first;
       hit_iter != cemc_electronics_hit_range.second; hit_iter++)
   {
    PHG4Hit *this_hit =  hit_iter->second;

    ea_cemc += this_hit->get_edep();
   }


  float ntvars[4];
  ntvars[0] = _event;
  ntvars[1] = ev_hcin/(e_hcin+ea_hcin);
  ntvars[2] = ev_hcout/(e_hcout+ea_hcout);
  ntvars[3] = ev_cemc/(e_cemc+ea_cemc);

  _sf->Fill( ntvars );
}

//___________________________________
void PHAna::create_histos()
{
 Fun4AllServer *se = Fun4AllServer::instance();
 Fun4AllHistoManager *hm = se->getHistoManager("HISTOS");
 if (!hm)
 {
  hm = new Fun4AllHistoManager("HISTOS");
  se->registerHistoManager(hm);
 }
 hm->registerHisto( new TH1F("zvertex","zvertex",120,-30,30) ); 
 hm->registerHisto( new TH1F("xvertex","xvertex",120,-5,5) ); 
 hm->registerHisto( new TH1F("yvertex","yvertex",120,-5,5) ); 
 
}

//___________________________________
void PHAna::GetNodes(PHCompositeNode *topNode)
{
 //DST objects
 //Truth container
  _truth_container = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
  if(!_truth_container && _event<2) cout << PHWHERE << " PHG4TruthInfoContainer node not found on node tree" << endl;

 //Outer HCal hit container
  _hcalout_hit_container = findNode::getClass<PHG4HitContainer> (topNode, "G4HIT_HCALOUT");
  if(!_hcalout_hit_container && _event<2) cout << PHWHERE << " G4HIT_HCALOUT node not found on node tree" << endl;

  //Outer HCal absorber hit container
    _hcalout_abs_hit_container = findNode::getClass<PHG4HitContainer> (topNode, "G4HIT_ABSORBER_HCALOUT");
   if(!_hcalout_abs_hit_container && _event<2) cout << PHWHERE << " G4HIT_ABSORBER_HCALOUT node not found on node tree" << endl;
  
 //Inner HCal hit container
   _hcalin_hit_container = findNode::getClass<PHG4HitContainer> (topNode, "G4HIT_HCALIN");
    if(!_hcalin_hit_container && _event<2) cout << PHWHERE << " G4HIT_HCALIN node not found on node tree" << endl;

 //Inner HCal absorber hit container
  _hcalin_abs_hit_container = findNode::getClass<PHG4HitContainer> (topNode, "G4HIT_ABSORBER_HCALIN");
   if(!_hcalin_abs_hit_container && _event<2) cout << PHWHERE << " G4HIT_ABSORBER_HCALIN node not found on node tree" << endl;

  //Inner HCal support structure hit container
     _hcalin_spt_hit_container = findNode::getClass<PHG4HitContainer> (topNode, "G4HIT_HCALIN_SPT");
   if(!_hcalin_spt_hit_container && _event<2) cout << PHWHERE << " G4HIT_HCALIN_SPT node not found on node tree" << endl;

  //EMCAL hit container
    _cemc_hit_container = findNode::getClass<PHG4HitContainer> (topNode, "G4HIT_CEMC");
   if(!_cemc_hit_container && _event<2) cout << PHWHERE << " G4HIT_CEMC node not found on node tree" << endl;

  //EMCal absorber hit container
     _cemc_abs_hit_container = findNode::getClass<PHG4HitContainer> (topNode, "G4HIT_ABSORBER_CEMC");
   if(!_cemc_abs_hit_container && _event<2) cout << PHWHERE << " G4HIT_ABSORBER_CEMC node not found on node tree" << endl;

  //EMCal Electronics hit container
   _cemc_electronics_hit_container = findNode::getClass<PHG4HitContainer> (topNode, "G4HIT_CEMC_ELECTRONICS");
   if(!_cemc_electronics_hit_container && _event<2) cout << PHWHERE << " G4HIT_CEMC_EMCELECTRONICS node not found on node tree" << endl;

}

//______________________________________
int PHAna::End(PHCompositeNode *topNode)
{
 PHTFileServer::get().cd( _outfile );
 if( _truth ) _truth->Write();
 if( _sf ) _sf->Write();
 if(_flags & HIST)
 {
 Fun4AllServer *se = Fun4AllServer::instance();
 Fun4AllHistoManager *hm = se->getHistoManager("HISTOS");
 for(unsigned int i=0; i<hm->nHistos(); i++) hm->getHisto(i)->Write();
 }
 return 0;
}

