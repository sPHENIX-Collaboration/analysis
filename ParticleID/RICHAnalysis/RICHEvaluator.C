#include "RICHEvaluator.h"
#include "dualrich_analyzer.h"
#include "TrackProjectorPid.h"
#include "SetupDualRICHAnalyzer.h"

// Fun4All includes
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/getClass.h>
#include <phool/PHCompositeNode.h>

#include <g4main/PHG4Hit.h>
#include <g4main/PHG4HitContainer.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4VtxPoint.h>
#include <g4main/PHG4TruthInfoContainer.h>

#include <g4hough/SvtxTrackMap.h>
#include <g4hough/SvtxTrack.h>
#include <g4hough/SvtxTrackState.h>

// ROOT includes
#include <TTree.h>
#include <TFile.h>
#include <TString.h>
#include <TMath.h>
#include <TDatabasePDG.h>
#include <TH1.h>

// other C++ includes
#include <cassert>
#include <algorithm>

using namespace std;

RICHEvaluator::RICHEvaluator(std::string tracksname, std::string richname, std::string filename) :
  SubsysReco("RICHEvaluator" ),
  _ievent(0),
  _detector(richname),
  _trackmap_name(tracksname),
  _refractive_index(1),
  _foutname(filename),
  _fout_root(nullptr),
  _trackproj(nullptr),
  _acquire(nullptr),
  _pdg(nullptr),
  _radius(220.)
{
  _richhits_name = "G4HIT_" + _detector;
}


int
RICHEvaluator::Init(PHCompositeNode *topNode)
{
  _verbose = false;
  _ievent = 0;

  /* create output file */
  _fout_root = new TFile(_foutname.c_str(), "RECREATE");

  /* create output tree */
  reset_tree_vars();
  init_tree();
  init_tree_small();

  /* access to PDG databse information */
  _pdg = new TDatabasePDG();

  /* Throw warning if refractive index is not greater than 1 */
  if ( _refractive_index <= 1 )
    cout << PHWHERE << " WARNING: Refractive index for radiator volume is " << _refractive_index << endl;

  return 0;
}


int
RICHEvaluator::InitRun(PHCompositeNode *topNode)
{
  /* create track projector object */
  _trackproj = new TrackProjectorPid( topNode );

  /* create acquire object */
  _acquire = new SetupDualRICHAnalyzer();

  return 0;
}


int
RICHEvaluator::process_event(PHCompositeNode *topNode)
{
  _ievent ++;


  /* Get truth information */
  PHG4TruthInfoContainer* truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");

  /* Get all photon hits in RICH for this event */
  PHG4HitContainer* richhits = findNode::getClass<PHG4HitContainer>(topNode,_richhits_name);

  /* Get track collection with all tracks in this event */
  SvtxTrackMap* trackmap = findNode::getClass<SvtxTrackMap>(topNode,_trackmap_name);

  /* Check if collections found */
  if (!truthinfo) {
    cout << PHWHERE << "PHG4TruthInfoContainer not found on node tree" << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }
  if (!richhits) {
    cout << PHWHERE << "PHG4HitContainer not found on node tree" << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }
  if (!trackmap) {
    cout << PHWHERE << "SvtxTrackMap node not found on node tree" << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }


  /* Loop over tracks */
  for (SvtxTrackMap::ConstIter track_itr = trackmap->begin(); track_itr != trackmap->end(); track_itr++)
    {

      bool use_reconstructed_momentum = false;
      bool use_truth_momentum = false;
      bool use_emission_momentum = true;

      bool use_reconstructed_point = false;
      bool use_approximate_point = true;


      /* Store angles to get RMS value */
      TH1F *ch_ang = new TH1F("","",1000,0.0,1.0);
      
      SvtxTrack* track_j = dynamic_cast<SvtxTrack*>(track_itr->second);

      /* Check if track_j is a null pointer. */
      if (track_j == NULL)
        continue;


      /* Fill momv object which is the normalized momentum vector of the track in the RICH (i.e. its direction) */
      double momv[3] = {0.,0.,0.};

      if (use_reconstructed_momentum) {
	/* 'Continue' with next track if RICH projection not found for this track */
	if ( ! _trackproj->get_projected_momentum( track_j, momv, TrackProjectorPid::SPHERE, _radius ) )
	  {
	    cout << "RICH track projection momentum NOT FOUND; next iteration" << endl;
	    continue;
	  }
      }
      if (use_truth_momentum) {
	/* Fill with truth momentum instead of reco */
	if ( ! _acquire->get_true_momentum( truthinfo, track_j, momv) )
          {
            cout << "No truth momentum found for track; next iteration" << endl;
            continue;
          }
      }
      if (use_emission_momentum) {
        /* Fill with vector constructed from emission points (from truth) */
        if ( ! _acquire->get_emission_momentum( truthinfo, richhits, track_j, momv) )
          {
            cout << "No truth momentum from emission points found for track; next iteration" << endl;
            continue;
          }
      }

      double momv_norm = sqrt( momv[0]*momv[0] + momv[1]*momv[1] + momv[2]*momv[2] );
      momv[0] /= momv_norm;
      momv[1] /= momv_norm;
      momv[2] /= momv_norm;


      /* Get mean emission point from track in RICH */
      double m_emi[3] = {0.,0.,0.};

      if (use_reconstructed_point) {
	/* 'Continue' with next track if RICH projection not found for this track */
	if ( ! _trackproj->get_projected_position( track_j, m_emi, TrackProjectorPid::SPHERE, _radius  ) )
	  {
	    cout << "RICH track projection position NOT FOUND; next iteration" << endl;
	    continue;
	  }
      }
      if (use_approximate_point) {
        m_emi[0] = ((_radius)/momv[2])*momv[0];
        m_emi[1] = ((_radius)/momv[2])*momv[1];
        m_emi[2] = ((_radius)/momv[2])*momv[2];
      }


      /* 'Continue' with next track if track doesn't pass through RICH */
      if ( ! _trackproj->is_in_RICH( momv ) )
	continue;
      

      /* Calculate truth emission angle and truth mass */
      if (truthinfo)
	{
	  _theta_true = calculate_true_emission_angle( truthinfo , track_j , _refractive_index );
	}
      
      /* Loop over all G4Hits in container (i.e. RICH photons in event) */
      PHG4HitContainer::ConstRange rich_hits_begin_end = richhits->getHits();
      PHG4HitContainer::ConstIterator rich_hits_iter;
      
      for (rich_hits_iter = rich_hits_begin_end.first; rich_hits_iter !=  rich_hits_begin_end.second; ++rich_hits_iter)
	{
	  PHG4Hit *hit_i = rich_hits_iter->second;
	  PHG4Particle* particle = NULL;
	  PHG4Particle* parent = NULL;
	  PHG4VtxPoint* vertex = NULL;
	 
	  if ( truthinfo )
	    {  
	      particle = truthinfo->GetParticle( hit_i->get_trkid() );
	      parent = truthinfo->GetParticle( particle->get_parent_id() );
	      vertex = truthinfo->GetVtx( particle->get_vtx_id() );
	    }
 	  
	  _hit_x0 =  hit_i->get_x(0);
	  _hit_y0 =  hit_i->get_y(0);
	  _hit_z0 =  hit_i->get_z(0);
	  
	  _emi_x = vertex->get_x();
	  _emi_y = vertex->get_y();
	  _emi_z = vertex->get_z();
	  
	  _track_px = particle->get_px();
	  _track_py = particle->get_py();
	  _track_pz = particle->get_pz();
	  
	  _mtrack_px = parent->get_px();
	  _mtrack_py = parent->get_py();
	  _mtrack_pz = parent->get_pz();
	  _mtrack_ptot = sqrt( _mtrack_px*_mtrack_px + _mtrack_py*_mtrack_py + _mtrack_pz*_mtrack_pz );
	  
	  _track_e = particle->get_e();
	  _mtrack_e = parent->get_e();
	  _edep = hit_i->get_edep();
	  
	  _bankid = 0;
	  _volumeid = hit_i->get_detid();
	  _hitid = hit_i->get_hit_id();
	  _pid = particle->get_pid();
	  _mpid = parent->get_pid();
	  _trackid = particle->get_track_id();
	  _mtrackid = parent->get_track_id();
	  _otrackid = track_j->get_id();
	  
	  /* Set reconstructed emission angle and reconstructed mass for output tree */
	  _theta_reco = _acquire->calculate_emission_angle( m_emi, momv, hit_i );
	  ch_ang->Fill(_theta_reco);
	  
	  /* Fill tree */
	  _tree_rich->Fill();

	  
	} // END loop over photons


      _theta_rms = ch_ang->GetRMS();  
      _theta_mean = ch_ang->GetMean();
      
      /* Fill condensed tree after every track */
      _tree_rich_small->Fill();
  
      
    } // END loop over tracks


  return 0;
}


double RICHEvaluator::calculate_true_emission_angle( PHG4TruthInfoContainer* truthinfo, SvtxTrack * track, double index )
{
  /* Get truth particle associated with track */
  PHG4Particle* particle = truthinfo->GetParticle( track->get_truth_track_id() );
 
  /* Get particle ID */
  int pid = particle->get_pid();

  /* Get particle mass */
  double mass = _pdg->GetParticle(pid)->Mass();

  /* Get particle total momentum */
  double ptotal = sqrt( particle->get_px() * particle->get_px() +
                        particle->get_py() * particle->get_py() +
                        particle->get_pz() * particle->get_pz() );

  /* Calculate beta = v/c */
  double beta = ptotal / sqrt( mass * mass + ptotal * ptotal );

  /* Calculate emission angle for Cerenkov light */
  double theta_c = acos( 1 / ( index * beta ) );

  return theta_c;
}

int
RICHEvaluator::End(PHCompositeNode *topNode)
{
  _fout_root->cd();
  _tree_rich->Write();
  _tree_rich_small->Write();
  _fout_root->Close();

  return 0;
}


void
RICHEvaluator::reset_tree_vars()
{
  _hit_x0 = -999;
  _hit_y0 = -999;
  _hit_z0 = -999;

  _emi_x = -999;
  _emi_y = -999;
  _emi_z = -999;

  _track_px = -999;
  _track_py = -999;
  _track_pz = -999;

  _mtrack_px = -999;
  _mtrack_py = -999;
  _mtrack_pz = -999;
  _mtrack_ptot = -999;

  _track_e = -999;
  _mtrack_e = -999;
  _edep = -999;

  _bankid = -999;
  _volumeid = -999;
  _hitid = -999;
  _pid = -999;
  _mpid = -999;
  _trackid = -999;
  _mtrackid = -999;
  _otrackid = -999;

  _theta_true = -999;
  _theta_reco = -999;
  _theta_mean = -999;
  _theta_rms = -999;

  return;
}


int
RICHEvaluator::init_tree()
{
  _tree_rich = new TTree("eval_rich","RICH Evaluator info");

  _tree_rich->Branch("event", &_ievent, "Event number /I");
  _tree_rich->Branch("hit_x", &_hit_x0, "Global x-hit /D");
  _tree_rich->Branch("hit_y", &_hit_y0, "Global y-hit /D");
  _tree_rich->Branch("hit_z", &_hit_z0, "Global z-hit /D");
  _tree_rich->Branch("emi_x", &_emi_x, "Photon emission x-coord. /D");  
  _tree_rich->Branch("emi_y", &_emi_y, "Photon emission y-coord. /D");
  _tree_rich->Branch("emi_z", &_emi_z, "Photon emission z-coord. /D");
  _tree_rich->Branch("px", &_track_px, "Particle x-momentum /D");
  _tree_rich->Branch("py", &_track_py, "Particle y-momentum /D");
  _tree_rich->Branch("pz", &_track_pz, "Particle z-momentum /D");
  _tree_rich->Branch("mpx", &_mtrack_px, "Mother x-momentum /D");
  _tree_rich->Branch("mpy", &_mtrack_py, "Mother y-momentum /D");
  _tree_rich->Branch("mpz", &_mtrack_pz, "Mother z-momentum /D");
  _tree_rich->Branch("mptot", &_mtrack_ptot, "Mother total momentum /D");
  _tree_rich->Branch("e", &_track_e, "Track energy /D");
  _tree_rich->Branch("me", &_mtrack_e, "Mother track energy /D");
  _tree_rich->Branch("edep", &_edep, "Energy deposited in material /D");
  _tree_rich->Branch("bankid", &_bankid, "Bank ID /I");
  _tree_rich->Branch("volumeid", &_volumeid, "Volume ID /I");
  _tree_rich->Branch("hitid", &_hitid, "Hit ID /I");
  _tree_rich->Branch("pid", &_pid, "Particle ID /I");
  _tree_rich->Branch("mpid", &_mpid, "Mother ID /I");
  _tree_rich->Branch("trackid", &_trackid, "Track ID /I");
  _tree_rich->Branch("mtrackid", &_mtrackid, "Mother track ID /I");
  _tree_rich->Branch("otrackid", &_otrackid, "Ordered track ID /I");

  _tree_rich->Branch("theta_true", &_theta_true, "True emission angle /D");
  _tree_rich->Branch("theta_reco", &_theta_reco, "Reconstructed emission angle /D");

  return 0;
}


int
RICHEvaluator::init_tree_small()
{
  /* Condensed ROOT tree, 1 entry per track */
  _tree_rich_small = new TTree("eval_rich_small","RICH Evaluator info condensed");

  _tree_rich_small->Branch("event", &_ievent, "Event number /I");
  _tree_rich_small->Branch("otrackid", &_otrackid, "Ordered track ID /I");
  _tree_rich_small->Branch("mptot", &_mtrack_ptot, "Total momentum /D");
  _tree_rich_small->Branch("theta_true", &_theta_true, "True emission angle /D");
  _tree_rich_small->Branch("theta_mean", &_theta_mean, "Reconstructed angle mean /D");
  _tree_rich_small->Branch("theta_rms", &_theta_rms, "Reconstructed angle spread /D");

  return 0;
}
