#include "MBDStudy.h"

#include <phool/phool.h>
#include <phool/getClass.h>
#include <g4main/PHG4HitContainer.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4Hit.h>
#include <g4main/PHG4VtxPoint.h>
#include <fun4all/PHTFileServer.h>
#include <fun4all/Fun4AllServer.h>
#include <ffaobjects/EventHeaderv1.h>
#include <mbd/MbdOut.h>
#include <mbd/MbdPmtContainer.h>
#include <mbd/MbdPmtHit.h>
#include <mbd/MbdGeom.h>
#include <mbd/MbdDefs.h>

#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TString.h>
#include <TLorentzVector.h>
//#include <TMath.h>
#include <TDatabasePDG.h>
#include <TRandom3.h>
#include <TSystem.h>

#include <iostream>
#include <cmath>

using namespace std;
using namespace MbdDefs;

//const Double_t C = 29.9792458; // cm/ns

//____________________________________
MBDStudy::MBDStudy(const string &name) : SubsysReco(name),
    _tree( 0 ),
    f_evt( 0 ),
    _pdg( 0 ),
    _rndm( 0 ),
    _tres( 0.05 ),
    _savefname( "mbdstudy.root" ),
    _savefile( 0 )
{ 

}

//___________________________________
int MBDStudy::Init(PHCompositeNode *topNode)
{
  cout << PHWHERE << " Saving to file " << _savefname << endl;
  //PHTFileServer::get().open( _outfile, "RECREATE");
  _savefile = new TFile( _savefname.c_str(), "RECREATE" );

  // Global MBD variables
  _tree = new TTree("t","MBD Study");
  _tree->Branch("evt",&f_evt,"evt/I");
  _tree->Branch("bimp",&f_bimp,"bimp/F");
  _tree->Branch("ncoll",&f_ncoll,"ncoll/I");
  _tree->Branch("npart",&f_npart,"npart/I");
  _tree->Branch("vx",&f_vx,"vx/F");
  _tree->Branch("vy",&f_vy,"vy/F");
  _tree->Branch("vz",&f_vz,"vz/F");
  _tree->Branch("bns",&f_mbdn[0],"bns/S");
  _tree->Branch("bnn",&f_mbdn[1],"bnn/S");
  _tree->Branch("bqs",&f_mbdq[0],"bqs/F");
  _tree->Branch("bqn",&f_mbdq[1],"bqn/F");
  _tree->Branch("bts",&f_mbdt[0],"bts/F");
  _tree->Branch("btn",&f_mbdt[1],"btn/F");
  _tree->Branch("btes",&f_mbdte[0],"btes/F");
  _tree->Branch("bten",&f_mbdte[1],"bten/F");
  _tree->Branch("bz",&f_mbdz,"bz/F");
  _tree->Branch("bt0",&f_mbdt0,"bt0/F");

  _pdg = TDatabasePDG::Instance();  // database of PDG info on particles
  _rndm = new TRandom3(0);

  TString name, title;
  for (int ipmt=0; ipmt<MbdDefs::MBD_N_PMT; ipmt++)
  {
    name = "h_mbdq"; name += ipmt;
    title = "mbd charge, ch "; title += ipmt;
    h_mbdq[ipmt] = new TH1F(name,title,1200,0,120*90);

    /*
    name = "h_tdiff_ch"; name += ipmt;
    title = "tdiff, ch "; title += ipmt;
    h_tdiff_ch[ipmt] = new TH1F(name,title,600,-3,3);
    */
  }

  for (int iarm=0; iarm<2; iarm++)
  {
    name = "h_mbdqtot"; name += iarm;
    title = "mbd charge, arm "; title += iarm;
    h_mbdqtot[iarm] = new TH1F(name,title,1200,0,120*60); // npe for 1 mip = 120, and up to 30 mips are possible

    //
    name = "hevt_mbdt"; name += iarm;
    title = "mbd times, arm "; title += iarm;
    hevt_mbdt[iarm] = new TH1F(name,title,200,7.5,11.5);
    hevt_mbdt[iarm]->SetLineColor(4);
  }
  h2_mbdqtot = new TH2F("h2_mbdqtot","north MBDQ vs South MBDQ",300,0,120*900,300,0,120*900);

  h_ztrue = new TH1F("h_ztrue","true z-vtx",600,-30,30);
  h_tdiff = new TH1F("h_tdiff","dt (measured - true_time)",6000,-3,3);
  h2_tdiff_ch = new TH2F("h2_tdiff_ch","dt (measured - true time) vs ch",MBD_N_PMT,-0.5,MBD_N_PMT-0.5,200,-2,2);

  gaussian = new TF1("gaussian","gaus",0,20);
  gaussian->FixParameter(2,0.05);   // set sigma to 50 ps

  c_mbdt = new TCanvas("c_mbdt","MBD Times",1200,800);
  c_mbdt->Divide(1,2);

  return 0;
}

//___________________________________
int MBDStudy::InitRun(PHCompositeNode *topNode)
{
  _mbdgeom = new MbdGeom();
  GetNodes(topNode);

  return 0;
}

//__________________________________
//Call user instructions for every event
int MBDStudy::process_event(PHCompositeNode *topNode)
{
  nprocessed++;

  //GetNodes(topNode);

  f_evt = _evtheader->get_EvtSequence();
  if(f_evt%1==0) cout << PHWHERE << "Event " << f_evt << "\t" << ", nprocessed = " << nprocessed << endl;

  // Initialize Variables
  f_mbdn[0] = 0;
  f_mbdn[1] = 0;
  f_mbdq[0] = 0.;
  f_mbdq[1] = 0.;
  f_mbdt[0] = -9999.;
  f_mbdt[1] = -9999.;
  f_mbdte[0] = -9999.;
  f_mbdte[1] = -9999.;
  f_mbdz = NAN;
  f_mbdt0 = NAN;
  hevt_mbdt[0]->Reset();
  hevt_mbdt[1]->Reset();

  // Get Truth Centrality Info
  f_bimp = _evtheader->get_ImpactParameter();
  f_ncoll = _evtheader->get_ncoll();
  f_npart = _evtheader->get_npart();

  // Get Primaries AND Secondaries
  /*
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
  if ( f_evt < 20 )
  {
    cout << "Num primaries = " << nprimaries << "\t" << _truth_container->GetNumPrimaryVertexParticles() << endl;
  }
  */


  // Get True Vertex
  // NB: Currently PrimaryVertexIndex is always 1, need to figure out how to handle collision pile-up
  PHG4VtxPoint *vtxp = _truth_container->GetPrimaryVtx( _truth_container->GetPrimaryVertexIndex() );
  if ( vtxp != 0 )
  {
    f_vx = vtxp->get_x();
    f_vy = vtxp->get_y();
    f_vz = vtxp->get_z();
    f_vt = vtxp->get_t();

    if ( f_evt<20 )
    {
      cout << "VTXP " << "\t" << f_vx << "\t" << f_vy << "\t" << f_vz << "\t" << f_vt << endl;
    }

    h_ztrue->Fill( f_vz );

  }


  // Get All Vertices
  PHG4TruthInfoContainer::ConstVtxRange vtx_range = _truth_container->GetVtxRange();
  unsigned int nvtx = 0;
 
  /*
  for (auto vtx_iter = vtx_range.first; vtx_iter != vtx_range.second; ++vtx_iter)
  {
    PHG4VtxPoint *vtx = vtx_iter->second;
    double vx = vtx->get_x();
    double vy = vtx->get_y();
    double vz = vtx->get_z();
    double vt = vtx->get_t();

    // look at first few tracks
    // What are negative track id's?
    if ( abs(vtx->get_id()) < 8 && f_evt<20 )
    {
      cout << "vtx " << nvtx << "\t" << vtx->get_id() << "\t" << vt << "\t" << vx << "\t" << vy << "\t" << vz << endl;
    }
    nvtx++;
  }
  */

  if ( f_evt<20 )
  {
    cout << "Num Vertices = " << nvtx << "\t" << _truth_container->GetNumVertices() << endl;
    //cout << "Primary Vertex = " << _truth_container->GetPrimaryVertexIndex() << endl;
  }

  // Go through MBD hits to see what they look like

  float len[MbdDefs::MBD_N_PMT] = {0.};
  float edep[MbdDefs::MBD_N_PMT] = {0.};
  float first_time[MbdDefs::MBD_N_PMT];    // First hit time for each tube
  std::fill_n(first_time, MbdDefs::MBD_N_PMT, 1e12);


  unsigned int nhits = 0;

  TLorentzVector v4;
  //TLorentzVector hitpos;

  PHG4HitContainer::ConstRange mbd_hit_range = _mbdhits->getHits();
  for (auto hit_iter = mbd_hit_range.first; hit_iter != mbd_hit_range.second; hit_iter++)
  {
    PHG4Hit *this_hit = hit_iter->second;

    unsigned int ipmt = this_hit->get_layer();  // pmt channel
    int arm = ipmt/64;;                         // south=0, north=1

    int trkid = this_hit->get_trkid();
    if ( trkid>0 && f_evt<20 ) cout << "TRKID " << trkid << endl;

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

    // Determine time
    //hitpos.SetXYZT( this_hit->get_x(), this_hit->get_y(), this_hit->get_z(), this_hit->get_t() );

    float xsign = 1.;
    float zsign = -1.;
    if ( arm == 1 )
    {
      xsign = -1.;
      zsign = 1.;
    }

    float tube_x = _mbdgeom->get_x(ipmt);
    float tube_y = _mbdgeom->get_y(ipmt);
    float tube_z = zsign*253.;
    float flight_z = fabs(tube_z - vtxp->get_z());

    float flight_time = sqrt( tube_x*tube_x + tube_y*tube_y + flight_z*flight_z )/C;
    float tdiff = this_hit->get_t(1) - ( vtxp->get_t() + flight_time );

    // get the first time
    if ( this_hit->get_t(1) < first_time[ipmt] )
    {
      if ( fabs( this_hit->get_t(1) ) < 106.5 )
      {
        first_time[ipmt] = this_hit->get_t(1) - vtxp->get_t();
        Float_t dt = static_cast<float>( _rndm->Gaus( 0, _tres ) ); // get fluctuation in time
        first_time[ipmt] += dt;
      }
      else
      {
        cout << "BAD " << ipmt << "\t" << this_hit->get_t(1) << endl;
      }
    }

    if ( f_evt<10 )
    {
      cout << "hit " << ipmt << "\t" << trkid << "\t" << pid
        //<< "\t" << v4.M()
        << "\t" << beta
        << "\t" << this_hit->get_path_length()
        << "\t" << this_hit->get_edep()
        //<< "\t" << v4.Eta()
        //<< "\t" << v4.Pt()
        //<< "\t" << v4.P()
        << "\t" << this_hit->get_x(1)
        << "\t" << this_hit->get_y(1)
        << "\t" << this_hit->get_z(1)
        << "\t" << this_hit->get_t(1)
        << "\t" << tdiff
        << endl;

      //cout << "WWW " << first_time[ipmt] << endl;
    }

    edep[ipmt] += this_hit->get_edep();
 
    // get summed path length for particles that can create CKOV light
    // n.p.e. is determined from path length
    if ( beta > MbdDefs::v_ckov && charge != 0. )
    {
      len[ipmt] += this_hit->get_path_length();

      if ( trkid>0 )
      {
        h_tdiff->Fill( tdiff );
        h2_tdiff_ch->Fill( ipmt, tdiff );
      }

      _pids[pid] += 1;
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


  // process the data from each channel
  for (int iarm=0; iarm<2; iarm++)
  {
    f_mbdt[iarm] = 0.;
  }

  vector<float> hit_times[2];   // times of the hits in each [arm]

  for (int ipmt=0; ipmt<MbdDefs::MBD_N_PMT; ipmt++)
  {
    //cout << "ZZZ " << ipmt << "\t" << first_time[ipmt] << "\t" << edep[ipmt] << "\t" << len[ipmt] << endl;

    int arm = _mbdgeom->get_arm(ipmt); // ch 0-63 = south, ch 64-127 = north

    // Fill charge and time info
    if ( len[ipmt]>0. )
    {
      if ( f_evt<20 )
      {
        cout << "ipmt " << ipmt << "\t" << len[ipmt] << "\t" << edep[ipmt] << endl;
      }

      // Get charge in MBD tube
      float npe = len[ipmt]*(120/3.0);  // we get 120 p.e. per 3 cm
      float dnpe = static_cast<float>( _rndm->Gaus( 0, sqrt(npe) ) ); // get fluctuation in npe

      npe += dnpe;  // apply the fluctuations in npe

      f_mbdq[arm] += npe;

      h_mbdq[ipmt]->Fill( npe );
      h_mbdqtot[arm]->Fill( npe );

      // Now time
      if ( first_time[ipmt] < 9999. )
      {
        // Fill evt histogram
        hevt_mbdt[arm]->Fill( first_time[ipmt] );
        hit_times[arm].push_back( first_time[ipmt] );

        f_mbdt[arm] += first_time[ipmt];
        //cout << "XXX " << ipmt << "\t" << f_mbdt[arm] << "\t" << first_time[ipmt] << endl;

      }
      else  // should never happen
      {
        cout << "ERROR, have hit but no time" << endl;
      }

      // threshold should be > 0.
      ++f_mbdn[arm];
    }
  }

  // Get best t
  if ( f_mbdn[0]>0 && f_mbdn[1]> 0 )
  {
    for (int iarm=0; iarm<2; iarm++)
    {
      c_mbdt->cd(iarm+1);

      std::sort( hit_times[iarm].begin(), hit_times[iarm].end() );
      float earliest = hit_times[iarm][0];

      gaussian->SetParameter(0,5);
      gaussian->SetParameter(1, earliest);
      gaussian->SetRange(6, earliest+ 5*0.05);
      //gaussian->SetParameter(1,hevt_mbdt[iarm]->GetMean());
      //gaussian->SetRange(6,hevt_mbdt[iarm]->GetMean()+0.125);

      hevt_mbdt[iarm]->Fit(gaussian,"BLR");
      hevt_mbdt[iarm]->Draw();

      if ( f_mbdn[iarm] > 0 )
      {
        //f_mbdt[iarm] = f_mbdt[iarm] / f_mbdn[iarm];
        f_mbdt[iarm] = gaussian->GetParameter(1);
        f_mbdte[iarm] = earliest;
      }
    }

    // Now calculate zvtx, t0 from best times
    f_mbdz = (f_mbdt[0] - f_mbdt[1])*C/2.0;
    f_mbdt0 = (f_mbdt[0] + f_mbdt[1])/2.0;

  }

  c_mbdt->Modified();
  c_mbdt->Update();

  if ( fabs(f_mbdz-f_vz)>5.0 && f_mbdn[0]>1 && f_mbdn[1]>1 )
  {
    string response;
    cout << "Event " << f_evt << " ? ";
    //cin >> response;
    //if ( response[0] == 'q' )
    TString name = "evt_"; name += f_evt; name += ".png";
    c_mbdt->SaveAs( name );
  }

  h2_mbdqtot->Fill( f_mbdq[0], f_mbdq[1] );

  _tree->Fill();

  //CheckDST(topNode);

  return 0;
}

//___________________________________
void MBDStudy::GetNodes(PHCompositeNode *topNode)
{
  // Get the DST objects

  // Truth container
  _truth_container = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
  if(!_truth_container && f_evt<10) cout << PHWHERE << " PHG4TruthInfoContainer node not found on node tree" << endl;

  // MBD hit container
  _mbdhits = findNode::getClass<PHG4HitContainer> (topNode, "G4HIT_MBD");
  if(!_mbdhits && f_evt<10) cout << PHWHERE << " G4HIT_MBD node not found on node tree" << endl;

  _evtheader = findNode::getClass<EventHeaderv1>(topNode, "EventHeader");
  if (!_evtheader && f_evt<10) cout << PHWHERE << " G4HIT_MBD node not found on node tree" << endl;

}

//______________________________________
int MBDStudy::End(PHCompositeNode *topNode)
{
  _savefile->cd();
  _savefile->Write();
  _savefile->Close();

  // print out list of pids that hit MBD
  cout << "PIDs of Particles that hit MBD" << endl;
  unsigned int ipid = 0;
  double npart = 0;
  for (auto & pid : _pids)
  {
    npart += pid.second;
  }
  for (auto & pid : _pids)
  {
    cout << pid.first << "\t" << pid.second << "\t" << pid.second/npart << endl;
    ipid++;
  }
  cout << "There were " << ipid << " different particle types" << endl;

  return 0;
}

void MBDStudy::CheckDST(PHCompositeNode *topNode)
{
  // MbdOut
  _mbdout = findNode::getClass<MbdOut>(topNode, "MbdOut");
  if(!_mbdout && f_evt<4) cout << PHWHERE << " MbdOut node not found on node tree" << endl;

  // MbdOut
  _mbdpmts = findNode::getClass<MbdPmtContainer>(topNode, "MbdPmtContainer");
  if(!_mbdpmts && f_evt<4) cout << PHWHERE << " MbdPmtContainer node not found on node tree" << endl;

  Float_t mbdz = _mbdout->get_zvtx();
  if ( f_mbdz != mbdz )
  {
    cout << "ERROR, f_mbdz != mbdz, " << f_mbdz << "\t" << mbdz << endl;
  }

  for (int iarm=0; iarm<2; iarm++)
  {
    if ( f_mbdq[iarm] != _mbdout->get_q(iarm) )
    {
      cout << "ERROR, f_mbdq != mbdq, arm " << iarm << "\t" << f_mbdq[iarm] << "\t" << _mbdout->get_q(iarm) << endl;
    }
  }

}

