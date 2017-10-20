#include "RICHEvaluator.h"

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

// ROOT includes
#include <TTree.h>
#include <TFile.h>
#include <TString.h>

// other C++ includes
#include <cassert>

using namespace std;

RICHEvaluator::RICHEvaluator(std::string filename, std::string richname) :
  SubsysReco("RICHEvaluator" ),
  _ievent(0),
  _richhits_name(richname),
  _foutname(filename),
  _fout_root(nullptr)
{

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

  return 0;
}

int
RICHEvaluator::process_event(PHCompositeNode *topNode)
{
  _ievent ++;

  /* reset tree variables */
  reset_tree_vars();

  /*get data nodes */
  PHG4HitContainer* richhits =
    findNode::getClass<PHG4HitContainer>(topNode,_richhits_name);

  /* skip event if no RICH hits found */
  if ( !richhits )
    return 0;

  /* get truth info node */
  PHG4TruthInfoContainer* truthinfo =
    findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");

  /* abort if no truth info container found */
  assert(truthinfo);

  /* Loop over all G4Hits in container (i.e. RICH photons in event) */
  PHG4HitContainer::ConstRange rich_hits_begin_end = richhits->getHits();
  PHG4HitContainer::ConstIterator rich_hits_iter;

  for (rich_hits_iter = rich_hits_begin_end.first; rich_hits_iter !=  rich_hits_begin_end.second; ++rich_hits_iter)
    {
      PHG4Hit *hit_i = rich_hits_iter->second;

      /* Get matching truth particle */
      PHG4Particle* particle = NULL;
      PHG4Particle* parent = NULL;
      PHG4VtxPoint* vertex = NULL;
      if ( truthinfo )
        {
          // cout << "Found a Truth Info Container!" << endl;
          particle = truthinfo->GetParticle( hit_i->get_trkid() );
          parent = truthinfo->GetParticle( particle->get_parent_id() );
          vertex = truthinfo->GetVtx( particle->get_vtx_id() );
        }
      /*
        else
        cout << "Missing Truth Info Container!" << endl;

        if ( !particle )
        cout << "NO truth particle found!" << endl;

        if ( !parent )
        cout << "NO parent particle found!" << endl;
      */

      /* Variables that need to be filled for output tree */
      _hit_x0 =  hit_i->get_x(0);
      _hit_y0 =  hit_i->get_y(0);
      _hit_z0 =  hit_i->get_z(0);

      _hit_lx0 = _hit_x0;
      _hit_ly0 = _hit_y0;
      _hit_lz0 = _hit_z0;

      _emi_x = vertex->get_x();
      _emi_y = vertex->get_y();
      _emi_z = vertex->get_z();

      _track_px = particle->get_px();
      _track_py = particle->get_py();
      _track_pz = particle->get_pz();

      _mtrack_px = parent->get_px();
      _mtrack_py = parent->get_py();
      _mtrack_pz = parent->get_pz();

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
      _otrackid = _mtrackid;
      /* END of output tree variables */

      /* fill output TTree */
      _tree_rich->Fill();

      /* fill output IRT TTree */
      _tree_irt->Fill();

    }
  /* END Loop over all G4Hits in container (i.e. RICH photons in event) */


  /* LOOP OVER PLANE DETECTOR HITS */
  PHG4HitContainer* planehits =
    findNode::getClass<PHG4HitContainer>(topNode,"G4HIT_FWDDISC");

  if (planehits)
    {
      PHG4HitContainer::ConstRange plane_hits_begin_end = planehits->getHits();
      PHG4HitContainer::ConstIterator plane_hits_iter;

      for (plane_hits_iter = plane_hits_begin_end.first; plane_hits_iter !=  plane_hits_begin_end.second; ++plane_hits_iter)
        {
          PHG4Hit *phit = plane_hits_iter->second;
          PHG4Particle *particle = truthinfo->GetParticle( phit->get_trkid() );

          phitx = phit->get_x(0);
          phity = phit->get_y(0);
          phitz = phit->get_z(0);

          ppx = phit->get_px(0);
          ppy = phit->get_py(0);
          ppz = phit->get_pz(0);
          pp = sqrt(ppx*ppx + ppy*ppy + ppz*ppz);

          pid = particle->get_pid();
          vtx = particle->get_vtx_id();

          _tree_plane->Fill();
        }
    }
  else
    cout << "no planehits" << endl;

  return 0;
}


int
RICHEvaluator::End(PHCompositeNode *topNode)
{
  _fout_root->cd();
  _tree_rich->Write();
  _tree_plane->Write();
  _tree_irt->Write();
  _fout_root->Close();

  return 0;
}

void
RICHEvaluator::reset_tree_vars()
{
  _hit_x0 = -999;
  _hit_y0 = -999;
  _hit_z0 = -999;

  _hit_lx0 = -999;
  _hit_ly0 = -999;
  _hit_lz0 = -999;

  _emi_x = -999;
  _emi_y = -999;
  _emi_z = -999;

  _track_px = -999;
  _track_py = -999;
  _track_pz = -999;

  _mtrack_px = -999;
  _mtrack_py = -999;
  _mtrack_pz = -999;

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

  phitx = -999;
  phity = -999;
  phitz = -999;
  ppx = -999;
  ppy = -999;
  ppz = -999;
  pp = -999;
  pid = -999;
  vtx = -999;

  return;
}

int
RICHEvaluator::init_tree()
{
  _tree_rich = new TTree("tree_rich","RICH info");

  _tree_rich->Branch("event", &_ievent, "Event number /I");
  _tree_rich->Branch("hit_x", &_hit_x0, "Global x-hit /D");
  _tree_rich->Branch("hit_y", &_hit_y0, "Global y-hit /D");
  _tree_rich->Branch("hit_z", &_hit_z0, "Global z-hit /D");
  _tree_rich->Branch("lhit_x", &_hit_lx0, "Local x-hit /D");
  _tree_rich->Branch("lhit_y", &_hit_ly0, "Local y-hit /D");
  _tree_rich->Branch("lhit_z", &_hit_lz0, "Local z-hit /D");
  _tree_rich->Branch("emi_x", &_emi_x, "Emission x-position /D");
  _tree_rich->Branch("emi_y", &_emi_y, "Emission y-position /D");
  _tree_rich->Branch("emi_z", &_emi_z, "Emission z-position /D");
  _tree_rich->Branch("px", &_track_px, "Particle x-momentum /D");
  _tree_rich->Branch("py", &_track_py, "Particle y-momentum /D");
  _tree_rich->Branch("pz", &_track_pz, "Particle z-momentum /D");
  _tree_rich->Branch("mpx", &_mtrack_px, "Mother x-momentum /D");
  _tree_rich->Branch("mpy", &_mtrack_py, "Mother y-momentum /D");
  _tree_rich->Branch("mpz", &_mtrack_pz, "Mother z-momentum /D");
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
  _tree_rich->Branch("otrackid", &_otrackid, "Original track ID /I");

  _tree_plane = new TTree("tree_plane","Plane Detector info");

  _tree_plane->Branch("event", &_ievent, "event /I");
  _tree_plane->Branch("phitx", &phitx, "x hit /D");
  _tree_plane->Branch("phity", &phity, "y hit /D");
  _tree_plane->Branch("phitz", &phitz, "z hit /D");
  _tree_plane->Branch("ppx", &ppx, "px /D");
  _tree_plane->Branch("ppy", &ppy, "py /D");
  _tree_plane->Branch("ppz", &ppz, "pz /D");
  _tree_plane->Branch("pp", &pp, "p /D");
  _tree_plane->Branch("pid", &pid, "ID /I");
  _tree_plane->Branch("vtx", &vtx, "Vtx ID /I");

  _tree_irt = new TTree("tree_irt","IRT Algorithm Restricted Info");

  _tree_irt->Branch("event", &_ievent, "event /I");
  _tree_irt->Branch("hit_x", &_hit_x0, "Global x-hit /D");
  _tree_irt->Branch("hit_y", &_hit_y0, "Global y-hit /D");
  _tree_irt->Branch("hit_z", &_hit_z0, "Global z-hit /D");
  _tree_irt->Branch("mpx", &_mtrack_px, "Mother x-momentum /D");
  _tree_irt->Branch("mpy", &_mtrack_py, "Mother y-momentum /D");
  _tree_irt->Branch("mpz", &_mtrack_pz, "Mother z-momentum /D");
  _tree_irt->Branch("volumeid", &_volumeid, "Volume ID /I");

  return 0;
}
