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
#include <ffaobjects/EventHeaderv1.h>
#include <bbc/BbcOut.h>

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

const Double_t index_refract = 1.4585;
const Double_t v_ckov = 1.0/index_refract;  // velocity threshold for CKOV
const Double_t C = 29.9792458; // cm/ns

// kludge where we have the hardcoded positions of the tubes
// These are the x,y for the south BBC (in cm).
// The north inverts the x coordinate (x -> -x)
const float TubeLoc[64][2] = {
    { -12.2976,	4.26 },
    { -12.2976,	1.42 },
    { -9.83805,	8.52 },
    { -9.83805,	5.68 },
    { -9.83805,	2.84 },
    { -7.37854,	9.94 },
    { -7.37854,	7.1 },
    { -7.37854,	4.26 },
    { -7.37854,	1.42 },
    { -4.91902,	11.36 },
    { -4.91902,	8.52 },
    { -4.91902,	5.68 },
    { -2.45951,	12.78 },
    { -2.45951,	9.94 },
    { -2.45951,	7.1 },
    { 0,	11.36 },
    { 0,	8.52 },
    { 2.45951,	12.78 },
    { 2.45951,	9.94 },
    { 2.45951,	7.1 },
    { 4.91902,	11.36 },
    { 4.91902,	8.52 },
    { 4.91902,	5.68 },
    { 7.37854,	9.94 },
    { 7.37854,	7.1 },
    { 7.37854,	4.26 },
    { 7.37854,	1.42 },
    { 9.83805,	8.52 },
    { 9.83805,	5.68 },
    { 9.83805,	2.84 },
    { 12.2976,	4.26 },
    { 12.2976,	1.42 },
    { 12.2976,	-4.26 },
    { 12.2976,	-1.42 },
    { 9.83805,	-8.52 },
    { 9.83805,	-5.68 },
    { 9.83805,	-2.84 },
    { 7.37854,	-9.94 },
    { 7.37854,	-7.1 },
    { 7.37854,	-4.26 },
    { 7.37854,	-1.42 },
    { 4.91902,	-11.36 },
    { 4.91902,	-8.52 },
    { 4.91902,	-5.68 },
    { 2.45951,	-12.78 },
    { 2.45951,	-9.94 },
    { 2.45951,	-7.1 },
    { 0,	-11.36 },
    { 0,	-8.52 },
    { -2.45951,	-12.78 },
    { -2.45951,	-9.94 },
    { -2.45951,	-7.1 },
    { -4.91902,	-11.36 },
    { -4.91902,	-8.52 },
    { -4.91902,	-5.68 },
    { -7.37854,	-9.94 },
    { -7.37854,	-7.1 },
    { -7.37854,	-4.26 },
    { -7.37854,	-1.42 },
    { -9.83805,	-8.52 },
    { -9.83805,	-5.68 },
    { -9.83805,	-2.84 },
    { -12.2976,	-4.26 },
    { -12.2976,	-1.42 }
};    



//____________________________________
BBCStudy::BBCStudy(const string &name) : SubsysReco(name),
    _tree( 0 ),
    f_evt( 0 ),
    _pdg( 0 ),
    _rndm( 0 ),
    _tres( 0.05 ),
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

  // Global BBC variables
  _tree = new TTree("t","BBC Study");
  _tree->Branch("evt",&f_evt,"evt/I");
  _tree->Branch("bimp",&f_bimp,"bimp/F");
  _tree->Branch("ncoll",&f_ncoll,"ncoll/I");
  _tree->Branch("npart",&f_npart,"npart/I");
  _tree->Branch("vx",&f_vx,"vx/F");
  _tree->Branch("vy",&f_vy,"vy/F");
  _tree->Branch("vz",&f_vz,"vz/F");
  _tree->Branch("bns",&f_bbcn[0],"bns/S");
  _tree->Branch("bnn",&f_bbcn[1],"bnn/S");
  _tree->Branch("bqs",&f_bbcq[0],"bqs/F");
  _tree->Branch("bqn",&f_bbcq[1],"bqn/F");
  _tree->Branch("bts",&f_bbct[0],"bts/F");
  _tree->Branch("btn",&f_bbct[1],"btn/F");
  _tree->Branch("btes",&f_bbcte[0],"btes/F");
  _tree->Branch("bten",&f_bbcte[1],"bten/F");
  _tree->Branch("bz",&f_bbcz,"bz/F");
  _tree->Branch("bt0",&f_bbct0,"bt0/F");

  _pdg = TDatabasePDG::Instance();  // database of PDG info on particles
  _rndm = new TRandom3(0);

  TString name, title;
  for (int ipmt=0; ipmt<128; ipmt++)
  {
    name = "h_bbcq"; name += ipmt;
    title = "bbc charge, ch "; title += ipmt;
    h_bbcq[ipmt] = new TH1F(name,title,1200,0,120*90);

    /*
    name = "h_tdiff_ch"; name += ipmt;
    title = "tdiff, ch "; title += ipmt;
    h_tdiff_ch[ipmt] = new TH1F(name,title,600,-3,3);
    */
  }

  for (int iarm=0; iarm<2; iarm++)
  {
    name = "h_bbcqtot"; name += iarm;
    title = "bbc charge, arm "; title += iarm;
    h_bbcqtot[iarm] = new TH1F(name,title,1200,0,120*60); // npe for 1 mip = 120, and up to 30 mips are possible

    //
    name = "hevt_bbct"; name += iarm;
    title = "bbc times, arm "; title += iarm;
    hevt_bbct[iarm] = new TH1F(name,title,200,7.5,11.5);
    hevt_bbct[iarm]->SetLineColor(4);
  }
  h2_bbcqtot = new TH2F("h2_bbcqtot","north BBCQ vs South BBCQ",300,0,120*900,300,0,120*900);

  h_ztrue = new TH1F("h_ztrue","true z-vtx",600,-30,30);
  h_tdiff = new TH1F("h_tdiff","dt (measured - true_time)",6000,-3,3);
  h2_tdiff_ch = new TH2F("h2_tdiff_ch","dt (measured - true time) vs ch",128,-0.5,127.5,200,-2,2);

  gaussian = new TF1("gaussian","gaus",0,20);
  gaussian->FixParameter(2,0.05);   // set sigma to 50 ps

  c_bbct = new TCanvas("c_bbct","BBC Times",1200,800);
  c_bbct->Divide(1,2);

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
  nprocessed++;

  //GetNodes(topNode);

  f_evt = _evtheader->get_EvtSequence();
  if(f_evt%1==0) cout << PHWHERE << "Event " << f_evt << "\t" << ", nprocessed = " << nprocessed << endl;

  // Initialize Variables
  f_bbcn[0] = 0;
  f_bbcn[1] = 0;
  f_bbcq[0] = 0.;
  f_bbcq[1] = 0.;
  f_bbct[0] = -9999.;
  f_bbct[1] = -9999.;
  f_bbcte[0] = -9999.;
  f_bbcte[1] = -9999.;
  f_bbcz = NAN;
  f_bbct0 = NAN;
  hevt_bbct[0]->Reset();
  hevt_bbct[1]->Reset();

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

  // Go through BBC hits to see what they look like

  float len[128] = {0.};
  float edep[128] = {0.};
  float first_time[128];    // First hit time for each tube
  std::fill_n(first_time, 128, 1e12);


  unsigned int nhits = 0;

  TLorentzVector v4;
  //TLorentzVector hitpos;

  PHG4HitContainer::ConstRange bbc_hit_range = _bbchits->getHits();
  for (auto hit_iter = bbc_hit_range.first; hit_iter != bbc_hit_range.second; hit_iter++)
  {
    PHG4Hit *this_hit = hit_iter->second;

    unsigned int ch = this_hit->get_layer();  // pmt channel
    int arm = ch/64;;                         // south=0, north=1

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

    float tube_x = xsign*TubeLoc[ch%64][0];
    float tube_y = TubeLoc[ch%64][1];
    float tube_z = zsign*253.;
    float flight_z = fabs(tube_z - vtxp->get_z());

    float flight_time = sqrt( tube_x*tube_x + tube_y*tube_y + flight_z*flight_z )/C;
    float tdiff = this_hit->get_t(1) - ( vtxp->get_t() + flight_time );

    // get the first time
    if ( this_hit->get_t(1) < first_time[ch] )
    {
      if ( fabs( this_hit->get_t(1) ) < 106.5 )
      {
        first_time[ch] = this_hit->get_t(1) - vtxp->get_t();
        Float_t dt = static_cast<float>( _rndm->Gaus( 0, _tres ) ); // get fluctuation in time
        first_time[ch] += dt;
      }
      else
      {
        cout << "BAD " << ch << "\t" << this_hit->get_t(1) << endl;
      }
    }

    if ( f_evt<10 )
    {
      cout << "hit " << ch << "\t" << trkid << "\t" << pid
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

      //cout << "WWW " << first_time[ch] << endl;
    }

    edep[ch] += this_hit->get_edep();
 
    // get summed path length for particles that can create CKOV light
    // n.p.e. is determined from path length
    if ( beta > v_ckov && charge != 0. )
    {
      len[ch] += this_hit->get_path_length();

      if ( trkid>0 )
      {
        h_tdiff->Fill( tdiff );
        h2_tdiff_ch->Fill( ch, tdiff );
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
    f_bbct[iarm] = 0.;
  }

  vector<float> hit_times[2];   // times of the hits in each [arm]

  for (int ich=0; ich<128; ich++)
  {
    //cout << "ZZZ " << ich << "\t" << first_time[ich] << "\t" << edep[ich] << "\t" << len[ich] << endl;

    int arm = ich/64; // ch 0-63 = south, ch 64-127 = north

    // Fill charge and time info
    if ( len[ich]>0. )
    {
      if ( f_evt<20 )
      {
        cout << "ich " << ich << "\t" << len[ich] << "\t" << edep[ich] << endl;
      }

      // Get charge in BBC tube
      float npe = len[ich]*(120/3.0);  // we get 120 p.e. per 3 cm
      float dnpe = static_cast<float>( _rndm->Gaus( 0, sqrt(npe) ) ); // get fluctuation in npe

      npe += dnpe;  // apply the fluctuations in npe

      f_bbcq[arm] += npe;

      h_bbcq[ich]->Fill( npe );
      h_bbcqtot[arm]->Fill( npe );

      // Now time
      if ( first_time[ich] < 9999. )
      {
        // Fill evt histogram
        hevt_bbct[arm]->Fill( first_time[ich] );
        hit_times[arm].push_back( first_time[ich] );

        f_bbct[arm] += first_time[ich];
        //cout << "XXX " << ich << "\t" << f_bbct[arm] << "\t" << first_time[ich] << endl;

      }
      else  // should never happen
      {
        cout << "ERROR, have hit but no time" << endl;
      }

      // threshold should be > 0.
      ++f_bbcn[arm];
    }
  }

  // Get best t
  if ( f_bbcn[0]>0 && f_bbcn[1]> 0 )
  {
    for (int iarm=0; iarm<2; iarm++)
    {
      c_bbct->cd(iarm+1);

      std::sort( hit_times[iarm].begin(), hit_times[iarm].end() );
      float earliest = hit_times[iarm][0];

      gaussian->SetParameter(0,5);
      gaussian->SetParameter(1, earliest);
      gaussian->SetRange(6, earliest+ 5*0.05);
      //gaussian->SetParameter(1,hevt_bbct[iarm]->GetMean());
      //gaussian->SetRange(6,hevt_bbct[iarm]->GetMean()+0.125);

      hevt_bbct[iarm]->Fit(gaussian,"BLR");
      hevt_bbct[iarm]->Draw();

      if ( f_bbcn[iarm] > 0 )
      {
        //f_bbct[iarm] = f_bbct[iarm] / f_bbcn[iarm];
        f_bbct[iarm] = gaussian->GetParameter(1);
        f_bbcte[iarm] = earliest;
      }
    }

    // Now calculate zvtx, t0 from best times
    f_bbcz = (f_bbct[0] - f_bbct[1])*C/2.0;
    f_bbct0 = (f_bbct[0] + f_bbct[1])/2.0;

  }

  c_bbct->Modified();
  c_bbct->Update();

  if ( fabs(f_bbcz-f_vz)>5.0 && f_bbcn[0]>1 && f_bbcn[1]>1 )
  {
    string response;
    cout << "Event " << f_evt << " ? ";
    //cin >> response;
    //if ( response[0] == 'q' )
    TString name = "evt_"; name += f_evt; name += ".png";
    c_bbct->SaveAs( name );
  }

  h2_bbcqtot->Fill( f_bbcq[0], f_bbcq[1] );

  _tree->Fill();

  //CheckDST(topNode);

  return 0;
}

//___________________________________
void BBCStudy::GetNodes(PHCompositeNode *topNode)
{
  // Get the DST objects

  // Truth container
  _truth_container = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
  if(!_truth_container && f_evt<10) cout << PHWHERE << " PHG4TruthInfoContainer node not found on node tree" << endl;

  // BBC hit container
  _bbchits = findNode::getClass<PHG4HitContainer> (topNode, "G4HIT_BBC");
  if(!_bbchits && f_evt<10) cout << PHWHERE << " G4HIT_BBC node not found on node tree" << endl;

  _evtheader = findNode::getClass<EventHeaderv1>(topNode, "EventHeader");
  if (!_evtheader && f_evt<10) cout << PHWHERE << " G4HIT_BBC node not found on node tree" << endl;

}

//______________________________________
int BBCStudy::End(PHCompositeNode *topNode)
{
  _savefile->cd();
  _savefile->Write();
  _savefile->Close();

  // print out list of pids that hit BBC
  cout << "PIDs of Particles that hit BBC" << endl;
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

void BBCStudy::CheckDST(PHCompositeNode *topNode)
{
  // BbcOut
  BbcOut *_bbcout = findNode::getClass<BbcOut>(topNode, "BbcOut");
  if(!_bbcout && f_evt<10) cout << PHWHERE << " BbcOut node not found on node tree" << endl;

  Float_t bbcz = _bbcout->get_VertexPoint();
  if ( f_bbcz != bbcz )
  {
    cout << "ERROR, f_bbcz != bbcz, " << f_bbcz << "\t" << bbcz << endl;
  }

  for (int iarm=0; iarm<2; iarm++)
  {
    if ( f_bbcq[iarm] != _bbcout->get_nCharge(iarm) )
    {
      cout << "ERROR, f_bbcq != bbcq, arm " << iarm << "\t" << f_bbcq[iarm] << "\t" << _bbcout->get_nCharge(iarm) << endl;
    }
  }

}
