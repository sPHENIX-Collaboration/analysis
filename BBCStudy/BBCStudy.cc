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
#include <TLorentzVector.h>
#include <TDatabasePDG.h>


#include <iostream>

using namespace std;

const Double_t index_refract = 1.4585;
const Double_t v_ckov = 1.0/index_refract;  // velocity threshold for CKOV

//____________________________________
BBCStudy::BBCStudy(const string &name) : SubsysReco(name),
    _tree( 0 ),
    f_evt( 0 ),
    _pdg( 0 ),
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
  _tree->Branch("evt",&f_evt,"evt/I");
  _tree->Branch("bns",&f_bbcn[0],"bns/S");
  _tree->Branch("bnn",&f_bbcn[1],"bnn/S");
  _tree->Branch("bqs",&f_bbcq[0],"bqs/F");
  _tree->Branch("bqn",&f_bbcq[1],"bqn/F");
  _tree->Branch("bts",&f_bbct[0],"bts/F");
  _tree->Branch("btn",&f_bbct[1],"btn/F");
  _tree->Branch("bz",&f_bbcz,"bz/F");
  _tree->Branch("bt0",&f_bbct0,"bt0/F");

  _pdg = new TDatabasePDG();

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

  f_evt++;
  //if(f_evt%1000==0) cout << PHWHERE << "Events processed: " << f_evt << endl;
  if(f_evt%100==0) cout << PHWHERE << "Events processed: " << f_evt << endl;

  // Initialize Variables
  f_bbcn[0] = 0;
  f_bbcn[1] = 0;
  f_bbcq[0] = 0.;
  f_bbcq[1] = 0.;
  f_bbct[0] = -9999.;
  f_bbct[1] = -9999.;
  f_bbcz = -9999.;
  f_bbct0 = -9999.;

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

    if ( f_evt<20 )
    {
      cout << "VTXP " << "\t" << vt << "\t" << vx << "\t" << vy << "\t" << vz << endl;
    }
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
    if ( abs(vtx->get_id()) < 8 && f_evt<20 )
    {
      cout << "vtx " << nvtx << "\t" << vtx->get_id() << "\t" << vt << "\t" << vx << "\t" << vy << "\t" << vz << endl;
    }
    nvtx++;
  }

  if ( f_evt<20 )
  {
    cout << "Num Vertices = " << nvtx << "\t" << _truth_container->GetNumVertices() << endl;
    cout << "Primary Vertex = " << _truth_container->GetPrimaryVertexIndex() << endl;
  }

  // Go through hits to see what they look like

  float len[128] = {0.};
  float edep[128] = {0.};
  unsigned int nhits = 0;

  TLorentzVector v4;

  PHG4HitContainer::ConstRange bbc_hit_range = _bbchits->getHits();
  for (auto hit_iter = bbc_hit_range.first; hit_iter != bbc_hit_range.second; hit_iter++)
  {
    PHG4Hit *this_hit = hit_iter->second;

    unsigned int ch = this_hit->get_layer();  // pmt channel
    int arm = ch/64;;          // south=0, north=1

    int trkid = this_hit->get_trkid();
    if ( trkid>0 ) cout << "TRKID " << trkid << endl;

    PHG4Particle *part = _truth_container->GetParticle( trkid );
    v4.SetPxPyPzE( part->get_px(), part->get_py(), part->get_pz(), part->get_e() );

    int pid = part->get_pid();
    TParticlePDG *partinfo = _pdg->GetParticle( pid );
    Double_t charge = -9999.;
    if ( partinfo )
    {
      charge = partinfo->Charge()/3;  // PDG gives charge in 1/3 e
    }
    else if ( part->isIon() )
    {
      charge = part->get_IonCharge();
    }

    Double_t beta = v4.Beta();

    if ( f_evt<20 )
    {
      cout << "hit " << ch << "\t" << trkid << "\t" << pid
        << "\t" << v4.M()
        << "\t" << beta
        << "\t" << charge
        << "\t" << this_hit->get_path_length()
        << "\t" << this_hit->get_edep()
        << "\t" << v4.Eta()
        << "\t" << v4.Pt()
        << "\t" << v4.P()
        << endl;
    }

    edep[ch] += this_hit->get_edep();

    // get summed path length for particles that can create CKOV light
    if ( beta > v_ckov && charge != 0. )
    {
      len[ch] += this_hit->get_path_length();

      f_bbcq[arm] += this_hit->get_path_length()*(120/3.0);  // we get 120 p.e. per 3 cm
    }

    // sanity check
    if ( part->get_track_id() != trkid )
    {
      cout << "ERROR " << endl;
    }

    nhits++;
  }

  if ( f_evt<20 )
  {
    cout << "******** " << f_evt << " **************" << endl;
  }
  for (int ich=0; ich<128; ich++)
  {
    // threshold should be > 0.
    if ( len[ich]>0. )
    {
      if ( f_evt<20 )
      {
        cout << "ich " << ich << "\t" << len[ich] << "\t" << edep[ich] << endl;
      }

      int arm = ich/64;
      ++f_bbcn[arm];
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
  if(!_truth_container && f_evt<2) cout << PHWHERE << " PHG4TruthInfoContainer node not found on node tree" << endl;

  // BBC hit container
  _bbchits = findNode::getClass<PHG4HitContainer> (topNode, "G4HIT_BBC");
  if(!_bbchits && f_evt<2) cout << PHWHERE << " G4HIT_BBC node not found on node tree" << endl;

}

//______________________________________
int BBCStudy::End(PHCompositeNode *topNode)
{
  _savefile->cd();
  _savefile->Write();

  return 0;
}

