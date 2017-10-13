#include "RICHParticleID.h"
#include "dualrich_analyzer.h"

// Fun4All includes
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/getClass.h>
#include <phool/PHCompositeNode.h>

#include <g4main/PHG4Hit.h>
#include <g4main/PHG4HitContainer.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4VtxPoint.h>
#include "g4main/PHG4TruthInfoContainer.h"

#include <g4hough/SvtxTrackMap_v1.h>
#include <g4hough/SvtxTrack_FastSim.h>

// ROOT includes
#include <TTree.h>
#include <TFile.h>
#include <TString.h>
#include <TMath.h>

// other C++ includes
#include <cassert>
#include <algorithm>

using namespace std;

RICHParticleID::RICHParticleID(std::string tracksname, std::string richname, std::string filename) :
  SubsysReco("RICHParticleID" ),
  _ievent(0),
  _trackmap_name(tracksname),
  _richhits_name(richname),
  _foutname(filename),
  _fout_root(nullptr)
{

}


int
RICHParticleID::Init(PHCompositeNode *topNode)
{
  _verbose = false;
  _ievent = 0;

  /* create output file */
  _fout_root = new TFile(_foutname.c_str(), "RECREATE");

  /* create output tree */
  reset_tree_vars();
  init_tree();

  /* create analyzer object */
  _analyzer = new eic_dual_rich();

  return 0;
}


int
RICHParticleID::process_event(PHCompositeNode *topNode)
{
  _ievent ++;

  /* Get all photon hits in RICH for this event */
  PHG4HitContainer* richhits =
    findNode::getClass<PHG4HitContainer>(topNode,_richhits_name);

  /* Get track collection with all tracks in this event */
  SvtxTrackMap* trackmap =
    findNode::getClass<SvtxTrackMap>(topNode,_trackmap_name);
  /* Check if trackmap found */
  if (!trackmap) {
    cout << PHWHERE << "SvtxTrackMap node not found on node tree"
         << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  cout << "Track map size: " << trackmap->size() << endl;
  cout << "RICH hits size: " << richhits->size() << endl;

  /* get truth info node */
  //PHG4TruthInfoContainer* truthinfo =
  //findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");

  /* Create fake SvtxTrackMap from Truth Info container */
  //trackmap = fill_truth_trackmap(truthinfo, richhits);

  /* Loop over tracks */
  for (SvtxTrackMap::ConstIter track_itr = trackmap->begin();
       track_itr != trackmap->end(); track_itr++) {

    SvtxTrack_FastSim* track_j = dynamic_cast<SvtxTrack_FastSim*>(track_itr->second);

    /* Loop over all G4Hits in container (i.e. RICH photons in event) */
    PHG4HitContainer::ConstRange rich_hits_begin_end = richhits->getHits();
    PHG4HitContainer::ConstIterator rich_hits_iter;

    for (rich_hits_iter = rich_hits_begin_end.first; rich_hits_iter !=  rich_hits_begin_end.second; ++rich_hits_iter)
      {
        PHG4Hit *hit_i = rich_hits_iter->second;

        float theta_c = calculate_emission_angle( track_j, hit_i );

        /* Set reconstructed emission angle for output tree */
        _theta_reco = theta_c;

        /* Calculate true emission angle for output tree */
        _theta_true = 0;

        /* Fill tree */
        _tree_rich->Fill();

      } // END loop over photons

  } // END loop over tracks

  return 0;
}


double RICHParticleID::calculate_emission_angle( SvtxTrack_FastSim *track_j, PHG4Hit *hit_i )
{

  double momv[3] = {0.,0.,0.};
  double m_emi[3] = {0.,0.,0.};

  double mpx = track_j->get_px();
  double mpy = track_j->get_py();
  double mpz = track_j->get_pz();
  
  momv[0] = mpx/(mpx*mpx + mpy*mpy + mpz*mpz);
  momv[1] = mpy/(mpx*mpx + mpy*mpy + mpz*mpz);
  momv[2] = mpz/(mpx*mpx + mpy*mpy + mpz*mpz); 

  m_emi[0] = (220/momv[2])*momv[0]; // mean emission point
  m_emi[1] = (220/momv[2])*momv[1];
  m_emi[2] = (220/momv[2])*momv[2];

  int sec = hit_i->get_detid();

  /* Input parameters for indirect ray tracing algorithm */
  double Ex = m_emi[0];
  double Ey = m_emi[1];
  double Ez = m_emi[2];
  double Dx = hit_i->get_x(0);
  double Dy = hit_i->get_y(0);
  double Dz = hit_i->get_z(0);
  double vx = -18.5*TMath::Sin(sec*TMath::Pi()/4); // mirror center of each octant
  double vy = 18.5*TMath::Cos(sec*TMath::Pi()/4);
  double vz = 75;
  int select_radiator=0;

  /* Call algorithm to determine emission angle of photon i w.r.t. track j */
  float theta_c = _analyzer->ind_ray(Ex, Ey, Ez, Dx, Dy, Dz, vx, vy, vz, select_radiator); //Indirect Ray Tracing

  return theta_c;
}


int
RICHParticleID::End(PHCompositeNode *topNode)
{
  _fout_root->cd();
  _tree_rich->Write();
  _fout_root->Close();

  return 0;
}


void
RICHParticleID::reset_tree_vars()
{
  _theta_true = -1;
  _theta_reco = -1;

  return;
}


int
RICHParticleID::init_tree()
{
  _tree_rich = new TTree("pid_rich","RICH ParticleID info");

  _tree_rich->Branch("event", &_ievent, "Event number /I");
  _tree_rich->Branch("theta_true", &_theta_true, "True emission angle /F");
  _tree_rich->Branch("theta_reco", &_theta_reco, "Reconstructed emission angle /F");

  return 0;
}


//SvtxTrackMap*
//RICHParticleID::fill_truth_trackmap( PHG4TruthInfoContainer *truthinfo, PHG4HitContainer* richhits )
//{
//  SvtxTrackMap* trackmap = new SvtxTrackMap_v1();
//
//  vector< unsigned > v_id_used;
//
//  /* Loop over all G4Hits in container (i.e. RICH photons in event) */
//  PHG4HitContainer::ConstRange rich_hits_begin_end = richhits->getHits();
//  PHG4HitContainer::ConstIterator rich_hits_iter;
//
//  for (rich_hits_iter = rich_hits_begin_end.first; rich_hits_iter !=  rich_hits_begin_end.second; ++rich_hits_iter)
//    {
//      PHG4Hit *hit_i = rich_hits_iter->second;
//
//      /* Get matching truth particle */
//      PHG4Particle* particle = truthinfo->GetParticle( hit_i->get_trkid() );
//      PHG4Particle* parent = truthinfo->GetParticle( particle->get_parent_id() );
//      PHG4VtxPoint* vertex = truthinfo->GetVtx( parent->get_vtx_id() );
//
//      /* check that track has not been used yet in track container */
//      unsigned parent_id = parent->get_track_id();
//      if ( find( v_id_used.begin(), v_id_used.end(), parent_id ) != v_id_used.end())
//	continue;
//
//      /* create track object */
//      SvtxTrack_FastSim *track1 = new SvtxTrack_FastSim();
//
//      track1->set_x( vertex->get_x() );
//      track1->set_y( vertex->get_y() );
//      track1->set_z( vertex->get_z() );
//
//      track1->set_px( parent->get_px() );
//      track1->set_py( parent->get_py() );
//      track1->set_pz( parent->get_pz() );
//
//      trackmap->insert( track1 );
//
//      v_id_used.push_back( parent->get_track_id() );
//    }
//
//  return trackmap;
//}
