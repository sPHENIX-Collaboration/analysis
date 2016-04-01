////////////////////////////////////////////////////////////////////////////////
//
// This module is desgined to grab svtx tracks and put truth and cluster
// information into a TTree for GenFit testing
//
////////////////////////////////////////////////////////////////////////////////
//
// Darren McGlinchey
// 1 Apr 2016
//
////////////////////////////////////////////////////////////////////////////////


#include "AnaSvtxTracksForGenFit.h"

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

//----------------------------------------------------------------------------//
//-- Constructor:
//--  simple initialization
//----------------------------------------------------------------------------//
AnaSvtxTracksForGenFit::AnaSvtxTracksForGenFit(const string &name):
  SubsysReco(name),
  _flags( NONE )
{
//initialize
  _event = 0;
  _outfile = "AnaSvtxTracksForGenFit.root";
  _truth = 0;
  _sf = 0;
  _hcalin_towers = 0;
}

//----------------------------------------------------------------------------//
//-- Init():
//--   Intialize all histograms, trees, and ntuples
//----------------------------------------------------------------------------//
int AnaSvtxTracksForGenFit::Init(PHCompositeNode *topNode)
{
  cout << PHWHERE << " Openning file " << _outfile << endl;
  PHTFileServer::get().open( _outfile, "RECREATE");


  // create TTree
  _tracks = new TTree("tracks", "Svtx Tracks");
  _tracks->Branch("event", &event, "event/I");
  _tracks->Branch("gtrackID", &gtrackID, "gtrackID/I");
  _tracks->Branch("gflavor", &gflavor, "gflavor/I");
  _tracks->Branch("gpx", &gpx, "gpx/F");
  _tracks->Branch("gpy", &gpy, "gpy/F");
  _tracks->Branch("gpz", &gpz, "gpz/F");
  _tracks->Branch("gvx", &gvx, "gvx/F");
  _tracks->Branch("gvy", &gvy, "gvy/F");
  _tracks->Branch("gvz", &gvz, "gvz/F");
  _tracks->Branch("trackID", &trackID, "trackID/I");
  _tracks->Branch("charge", &charge, "charge/I");
  _tracks->Branch("nhits", &nhits, "nhits/I");
  _tracks->Branch("px", &px, "px/F");
  _tracks->Branch("py", &py, "py/F");
  _tracks->Branch("pz", &pz, "pz/F");
  _tracks->Branch("clusterID", &clusterID, "clusterID[nhits]/I");
  _tracks->Branch("layer", &layer, "layer[nhits]/I");
  _tracks->Branch("x", &x, "x[nhits]/F");
  _tracks->Branch("y", &y, "y[nhits]/F");
  _tracks->Branch("z", &z, "z[nhits]/F");
  _tracks->Branch("size_dphi", &size_dphi, "size_dphi[nhits]/F");
  _tracks->Branch("size_dz", &size_dz, "size_dz[nhits]/F");


  return 0;
}

//----------------------------------------------------------------------------//
//-- process_event():
//--   Call user instructions for every event.
//--   This function contains the analysis structure.
//----------------------------------------------------------------------------//
int AnaSvtxTracksForGenFit::process_event(PHCompositeNode *topNode)
{
  _event++;
  if (_event % 1000 == 0) cout << PHWHERE << "Events processed: " << _event << endl;
  GetNodes(topNode);
  if (_flags & TRUTH) fill_truth_ntuple(topNode);

  if (_flags & HIST) fill_histos(topNode);

  if (_flags & SF) fill_sf_ntuple(topNode);

  return 0;
}

//----------------------------------------------------------------------------//
//-- End():
//--   End method, wrap everything up
//----------------------------------------------------------------------------//
int AnaSvtxTracksForGenFit::End(PHCompositeNode *topNode)
{
  PHTFileServer::get().cd( _outfile );
  if ( _truth ) _truth->Write();
  if ( _sf ) _sf->Write();
  if (_flags & HIST)
  {
    Fun4AllServer *se = Fun4AllServer::instance();
    Fun4AllHistoManager *hm = se->getHistoManager("HISTOS");
    for (unsigned int i = 0; i < hm->nHistos(); i++) hm->getHisto(i)->Write();
  }
  return 0;
}


//----------------------------------------------------------------------------//
//-- fill_tree():
//--   Fill the trees with truth, track fit, and cluster information
//----------------------------------------------------------------------------//
void AnaSvtxTracksForGenFit::fill_tree(PHCompositeNode *topNode)
{

}

//----------------------------------------------------------------------------//
//-- reset_variables():
//--   Reset all the tree variables to their default values.
//--   Needs to be called at the start of every event
//----------------------------------------------------------------------------//
void AnaSvtxTracksForGenFit::reset_variables()
{
  event = -9999;

  //-- truth
  gtrackID = -9999;
  gflavor = -9999;
  gpx = -9999;
  gpy = -9999;
  gpz = -9999;
  gvx = -9999;
  gvy = -9999;
  gvz = -9999;

  //-- reco
  trackID = -9999;
  charge = -9999;
  nhits = -9999;
  px = -9999;
  py = -9999;
  pz = -9999;

  //-- clusters
  for (int i = 0; i < 7; i++)
  {
    clusterID[i] = -9999;
    layer[i] = -9999;
    x[i] = -9999;
    y[i] = -9999;
    z[i] = -9999;
    size_dphi[i] = -9999;
    size_dz[i] = -9999;
  }

}

//----------------------------------------------------------------------------//
//-- GetNodes():
//--   Get all the all the required nodes off the node tree
//----------------------------------------------------------------------------//
void AnaSvtxTracksForGenFit::GetNodes(PHCompositeNode *topNode)
{
//DST objects
//Truth container
  _truth_container = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
  if (!_truth_container && _event < 2) cout << PHWHERE << " PHG4TruthInfoContainer node not found on node tree" << endl;

//Outer HCal hit container
  _hcalout_hit_container = findNode::getClass<PHG4HitContainer> (topNode, "G4HIT_HCALOUT");
  if (!_hcalout_hit_container && _event < 2) cout << PHWHERE << " G4HIT_HCALOUT node not found on node tree" << endl;

  //Outer HCal absorber hit container
  _hcalout_abs_hit_container = findNode::getClass<PHG4HitContainer> (topNode, "G4HIT_ABSORBER_HCALOUT");
  if (!_hcalout_abs_hit_container && _event < 2) cout << PHWHERE << " G4HIT_ABSORBER_HCALOUT node not found on node tree" << endl;

//Inner HCal hit container
  _hcalin_hit_container = findNode::getClass<PHG4HitContainer> (topNode, "G4HIT_HCALIN");
  if (!_hcalin_hit_container && _event < 2) cout << PHWHERE << " G4HIT_HCALIN node not found on node tree" << endl;

//Inner HCal absorber hit container
  _hcalin_abs_hit_container = findNode::getClass<PHG4HitContainer> (topNode, "G4HIT_ABSORBER_HCALIN");
  if (!_hcalin_abs_hit_container && _event < 2) cout << PHWHERE << " G4HIT_ABSORBER_HCALIN node not found on node tree" << endl;

  //Inner HCal support structure hit container
  _hcalin_spt_hit_container = findNode::getClass<PHG4HitContainer> (topNode, "G4HIT_HCALIN_SPT");
  if (!_hcalin_spt_hit_container && _event < 2) cout << PHWHERE << " G4HIT_HCALIN_SPT node not found on node tree" << endl;

  //EMCAL hit container
  _cemc_hit_container = findNode::getClass<PHG4HitContainer> (topNode, "G4HIT_CEMC");
  if (!_cemc_hit_container && _event < 2) cout << PHWHERE << " G4HIT_CEMC node not found on node tree" << endl;

  //EMCal absorber hit container
  _cemc_abs_hit_container = findNode::getClass<PHG4HitContainer> (topNode, "G4HIT_ABSORBER_CEMC");
  if (!_cemc_abs_hit_container && _event < 2) cout << PHWHERE << " G4HIT_ABSORBER_CEMC node not found on node tree" << endl;

  //EMCal Electronics hit container
  _cemc_electronics_hit_container = findNode::getClass<PHG4HitContainer> (topNode, "G4HIT_CEMC_ELECTRONICS");
  if (!_cemc_electronics_hit_container && _event < 2) cout << PHWHERE << " G4HIT_CEMC_EMCELECTRONICS node not found on node tree" << endl;

}

//_____________________________________
void AnaSvtxTracksForGenFit::fill_truth_ntuple(PHCompositeNode *topNode)
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
    ntvars[2] = 0.5 * log((particle->get_e() + particle->get_pz()) /
                          (particle->get_e() - particle->get_pz()));
    ntvars[3] = sqrt(Square(particle->get_px()) + Square(particle->get_py()));
    ntvars[4] = atan2(particle->get_py(), particle->get_px());
    ntvars[5] = particle->get_pid();
    _truth->Fill( ntvars );
  }

}



