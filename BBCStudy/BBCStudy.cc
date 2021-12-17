#include "BBCStudy.h"

#include <phool/phool.h>
#include <phool/getClass.h>
#include <g4main/PHG4HitContainer.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4Hit.h>
#include <g4main/PHG4VtxPoint.h>
#include <fun4all/PHTFileServer.h>
#include <fun4all/Fun4AllServer.h>

#include <TFile.h>
#include <TTree.h>
#include <TH1.h>

#include <iostream>

using namespace std;

//____________________________________
BBCStudy::BBCStudy(const string &name) : SubsysReco(name),
    _tree( 0 ),
    _evt( 0 ),
    _savefname( "bbcstudy.root" ),
    _savefile( 0 )
{ 

}

//___________________________________
int BBCStudy::Init(PHCompositeNode *topNode)
{
  cout << PHWHERE << " Saving to file " << _savefname << endl;
  //PHTFileServer::get().open( _outfile, "RECREATE");
  _savefile = new TFile( _savefname.c_str(), "RECREATE" );

  _tree = new TTree("t","BBC Study");
  _tree->Branch("evt",&_evt,"evt/I");

  return 0;
}

//___________________________________
int BBCStudy::InitRun(PHCompositeNode *topNode)
{
  GetNodes(topNode);

  return 0;
}

//__________________________________
//Call user instructions for every event
int BBCStudy::process_event(PHCompositeNode *topNode)
{
  //GetNodes(topNode);

  _evt++;
  //if(_evt%1000==0) cout << PHWHERE << "Events processed: " << _evt << endl;
  if(_evt%1==0) cout << PHWHERE << "Events processed: " << _evt << endl;

  // Get Primaries
  PHG4TruthInfoContainer::ConstRange primary_range = _truth_container->GetPrimaryParticleRange();
  int nprimaries = 0;

  for (auto part_iter = primary_range.first; part_iter != primary_range.second; ++part_iter)
  {
    PHG4Particle *particle = part_iter->second;
    Float_t e = particle->get_e();
    Float_t px = particle->get_px();
    Float_t py = particle->get_py();
    Float_t pz = particle->get_pz();
    Float_t eta = 0.5*log((particle->get_e()+particle->get_pz())/ (particle->get_e()-particle->get_pz()));
    Float_t pt = sqrt( px*px + py*py );
    Float_t phi = atan2(particle->get_py(), particle->get_px());
    Float_t pid = particle->get_pid();

    nprimaries++;
  }
  cout << "Num primaries = " << nprimaries << "\t" << _truth_container->GetNumPrimaryVertexParticles() << endl;

  // Get True Vertex
  PHG4VtxPoint *vtxp = _truth_container->GetPrimaryVtx( _truth_container->GetPrimaryVertexIndex() );
  if ( vtxp != 0 )
  {
    double vt = vtxp->get_t();
    double vx = vtxp->get_x();
    double vy = vtxp->get_y();
    double vz = vtxp->get_z();
    cout << "VTXP " << "\t" << vt << "\t" << vx << "\t" << vy << "\t" << vz << endl;
  }


  // Get All Vertices
  PHG4TruthInfoContainer::ConstVtxRange vtx_range = _truth_container->GetVtxRange();
  unsigned int nvtx = 0;
 
  for (auto vtx_iter = vtx_range.first; vtx_iter != vtx_range.second; ++vtx_iter)
  {
    PHG4VtxPoint *vtx = vtx_iter->second;
    double vx = vtx->get_x();
    double vy = vtx->get_y();
    double vz = vtx->get_z();
    double vt = vtx->get_t();

    //if ( nvtx < 10 )
    if ( abs(vtx->get_id()) < 8 )
    {
      cout << "vtx " << nvtx << "\t" << vtx->get_id() << "\t" << vt << "\t" << vx << "\t" << vy << "\t" << vz << endl;
    }
    nvtx++;
  }

  cout << "Num Vertices = " << nvtx << "\t" << _truth_container->GetNumVertices() << endl;
  cout << "Primary Vertex = " << _truth_container->GetPrimaryVertexIndex() << endl;


  float len[128] = {0.};
  float edep[128] = {0.};
  unsigned int nhits = 0;

  PHG4HitContainer::ConstRange bbc_hit_range = _bbchits->getHits();
  for (auto hit_iter = bbc_hit_range.first; hit_iter != bbc_hit_range.second; hit_iter++)
  {
    PHG4Hit *this_hit = hit_iter->second;

    unsigned int ch = this_hit->get_layer();  // pmt channel

    len[ch] += this_hit->get_path_length();

    edep[ch] += this_hit->get_edep();

    int trkid = this_hit->get_trkid();
    if ( trkid>0 ) cout << "TRKID " << trkid << endl;

    PHG4Particle *part = _truth_container->GetParticle( trkid );
    cout << "hit " << ch << "\t" << trkid << "\t" << part->get_pid() << endl;

    if ( part->get_track_id() != trkid )
    {
      cout << "ERROR " << endl;
    }

    nhits++;
  }

  cout << "******** " << _evt << " **************" << endl;
  for (int ich=0; ich<128; ich++)
  {
    if ( len[ich]>0. )
    {
      cout << "ich " << ich << "\t" << len[ich] << "\t" << edep[ich] << endl;
    }
  }

  _tree->Fill();

  return 0;
}

//___________________________________
void BBCStudy::GetNodes(PHCompositeNode *topNode)
{
  // Get the DST objects

  // Truth container
  _truth_container = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
  if(!_truth_container && _evt<2) cout << PHWHERE << " PHG4TruthInfoContainer node not found on node tree" << endl;

  // BBC hit container
  _bbchits = findNode::getClass<PHG4HitContainer> (topNode, "G4HIT_BBC");
  if(!_bbchits && _evt<2) cout << PHWHERE << " G4HIT_BBC node not found on node tree" << endl;

}

//______________________________________
int BBCStudy::End(PHCompositeNode *topNode)
{
  _savefile->cd();
  _savefile->Write();

  return 0;
}

