#include "BbcCheck.h"

#include <phool/phool.h>
#include <phool/getClass.h>
#include <phool/recoConsts.h>
#include <fun4all/Fun4AllServer.h>
#include <ffaobjects/EventHeaderv1.h>

#include <mbd/MbdDefs.h>
#include <mbd/MbdOut.h>
#include <mbd/MbdPmtContainer.h>
#include <mbd/MbdPmtHit.h>
#include <mbd/MbdGeom.h>

#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfo.h>
#include <globalvertex/GlobalVertexMap.h>
#include <globalvertex/GlobalVertex.h>

#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TGraphErrors.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TString.h>
#include <TLorentzVector.h>
//#include <TMath.h>
#include <TSystem.h>

#include <iostream>
#include <cmath>
#include <cstdio>

using namespace std;
using namespace MbdDefs;

/*
const Double_t index_refract = 1.4585;
const Double_t v_ckov = 1.0/index_refract;  // velocity threshold for CKOV
const Double_t C = 29.9792458; // cm/ns
*/

//____________________________________
BbcCheck::BbcCheck(const string &name) : SubsysReco(name),
    _tree( 0 ),
    f_evt( 0 ),
    _tres( 0.05 ),
    _savefname( "bbcstudy.root" ),
    _savefile( 0 )
{ 

}

//___________________________________
int BbcCheck::Init(PHCompositeNode *topNode)
{
  cout << PHWHERE << " Saving to file " << _savefname << endl;
  //PHTFileServer::get().open( _outfile, "RECREATE");
  _savefile = new TFile( _savefname.c_str(), "RECREATE" );

  _tree = new TTree("t","BbcCheck");
  _tree->Branch("run",&f_run,"run/I");
  _tree->Branch("ch",&f_ch,"ch/I");
  _tree->Branch("qmean",&f_qmean,"qmean/F");
  _tree->Branch("qmerr",&f_qmerr,"qmerr/F");

  TString name, title;
  for (int ipmt=0; ipmt<128; ipmt++)
  {
    name = "h_bbcq"; name += ipmt;
    title = "bbc charge, ch "; title += ipmt;
    h_bbcq[ipmt] = new TH1F(name,title,1200,0,60);

    // TGraph to track mean of bbcq distribution
    name = "g_bbcq"; name += ipmt;
    title = "mbdq, ch "; title += ipmt;
    g_bbcq[ipmt] = new TGraphErrors();
    g_bbcq[ipmt]->SetName(name);
    g_bbcq[ipmt]->SetTitle(name);

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
    h_bbcqtot[iarm] = new TH1F(name,title,1400,0,1400); // npe for 1 mip = 120, and up to 30 mips are possible

    //
    name = "hevt_bbct"; name += iarm;
    title = "bbc times, arm "; title += iarm;
    hevt_bbct[iarm] = new TH1F(name,title,200,7.5,11.5);
    hevt_bbct[iarm]->SetLineColor(4);
  }
  h_bbcqsum = new TH1F("h_bbcqsum","MBD/BBC north + south charge sum",3000,0.,3000.);
  h2_bbcqsum = new TH2F("h2_bbcqsum","north BBCQ vs South BBCQ",1400,0.,1400.,1400,0.,1400.);

  h_zdce = new TH1F("h_zdce","ZDC Energy",820,-50,4050);
  h_zdctimecut = new TH1F("h_zdctimecut", "zdctimecut", 50, -17.5 , 32.5);

  h_emcale = new TH1F("h_emcale","Emcal Energy",3000,-100,2900);
  h_emcaltimecut = new TH1F("h_emcaltimecut", "emcaltimecut", 50, -17.5 , 32.5);

  h_ohcale = new TH1F("h_ohcale","OHCAL Energy",1000,-100,900);
  h_ohcaltimecut = new TH1F("h_ohcaltimecut", "ohcaltimecut", 50, -17.5 , 32.5);

  h_ihcale = new TH1F("h_ihcale","IHCAL Energy",1000,-100,900);
  h_ihcaltimecut = new TH1F("h_ihcaltimecut", "ihcaltimecut", 50, -17.5 , 32.5);

  h_bz = new TH1F("h_bz","MBD z-vertex",1200,-300,300);

  gaussian = new TF1("gaussian","gaus",0,20);
  gaussian->FixParameter(2,0.05);   // set sigma to 50 ps

  c_bbct = new TCanvas("c_bbct","BBC Times",1200,800);
  c_bbct->Divide(1,2);

  return 0;
}

//___________________________________
int BbcCheck::InitRun(PHCompositeNode *topNode)
{
  recoConsts *rc = recoConsts::instance();
  f_run = rc->get_IntFlag("RUNNUMBER");

  GetNodes(topNode);

  return 0;
}

//__________________________________
//Call user instructions for every event
int BbcCheck::process_event(PHCompositeNode *topNode)
{
  nprocessed++;

  //f_evt = _evtheader->get_EvtSequence();
  //if(f_evt%1==0) cout << PHWHERE << "Event " << f_evt << "\t" << ", nprocessed = " << nprocessed << endl;

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
  //f_bimp = _evtheader->get_ImpactParameter();
  //f_ncoll = _evtheader->get_ncoll();
  //f_npart = _evtheader->get_npart();

  /*
  if ( f_evt<20 )
  {
    cout << "******** " << f_evt << " **************" << endl;
  }
  */


  CheckDST(topNode);

  return 0;
}

//___________________________________
void BbcCheck::GetNodes(PHCompositeNode *topNode)
{
  // Get the DST objects

  //_evtheader = findNode::getClass<EventHeaderv1>(topNode, "EventHeader");
  //if (!_evtheader && f_evt<10) cout << PHWHERE << " EventHeader node not found on node tree" << endl;

  // BbcOut
  _bbcout = findNode::getClass<MbdOut>(topNode, "MbdOut");
  if(!_bbcout && f_evt<4) cout << PHWHERE << " MbdOut node not found on node tree" << endl;

  // BbcPmt Info
  _bbcpmts = findNode::getClass<MbdPmtContainer>(topNode, "MbdPmtContainer");
  if(!_bbcpmts && f_evt<4) cout << PHWHERE << " MbdPmtContainer node not found on node tree" << endl;

}

//______________________________________
int BbcCheck::End(PHCompositeNode *topNode)
{
  _savefile->cd();

  Double_t nevents = h_bbcqsum->Integral();
  Double_t norm = 1.0/nevents;
  h_bbcqsum->Scale( norm );
  h2_bbcqsum->Scale( norm );

  h_bbcqtot[0]->Scale( norm );
  h_bbcqtot[1]->Scale( norm );

  for (int ipmt=0; ipmt<BBC_N_PMT; ipmt++)
  {
    h_bbcq[ipmt]->Scale( norm );

    // Fill info on q distribution
    f_ch = ipmt;
    f_qmean = h_bbcq[ipmt]->GetMean();
    f_qmerr = h_bbcq[ipmt]->GetMeanError();
    _tree->Fill();
    cout << f_run << "\t" << f_ch << "\t" << f_qmean << "\t" << f_qmerr << endl;

    g_bbcq[ipmt]->SetPoint(0,f_run,f_qmean);
    g_bbcq[ipmt]->SetPointError(0,0,f_qmerr);
    g_bbcq[ipmt]->Write();
  }

  cout << "Nevents processed integral " << nprocessed << "\t" << nevents << "\t" << nevents/nprocessed << endl;
  _savefile->Write();
  _savefile->Close();

  return 0;
}

void BbcCheck::CheckDST(PHCompositeNode *topNode)
{
  Float_t bqs = _bbcout->get_q(0);
  Float_t bqn = _bbcout->get_q(1);
  Float_t bz = _bbcout->get_zvtx();

  h_bz->Fill( bz );

  // Check the GlobalVertex
  GlobalVertexMap *vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
  if ( vertexmap && !vertexmap->empty() )
  {
    GlobalVertex *vtx = vertexmap->begin()->second;
    if ( vtx )
    {
      float vtx_z = vtx->get_z();
      if ( vtx_z != bz )
      {
        cout << "ERROR, vertices do not match " << vtx_z << "\t" << bz << endl;
      }
    }
  }
  else
  {
    static int counter = 0;
    if ( counter < 4 ) cout << "GlobalVertexMap not found or is empty" << endl;
    counter++;
  }

  if ( fabs(bz)>60. ) return;
  if ( bqn<10 && bqs>150 ) return;

  //Float_t r = (4.4049/4.05882);
  Float_t r = 1.0;

  h_bbcqsum->Fill( (bqn+bqs)*r );
  h_bbcqtot[0]->Fill( bqs*r );
  h_bbcqtot[1]->Fill( bqn*r );
  h2_bbcqsum->Fill( bqn*r, bqs*r );

  // Fill info from each PMT
  for (int ipmt=0; ipmt<_bbcpmts->get_npmt(); ipmt++)
  {
    Float_t q = _bbcpmts->get_pmt(ipmt)->get_q();
    Float_t t = _bbcpmts->get_pmt(ipmt)->get_time();
    //Float_t phi = mbdgeom->get_phi(ipmt);   // get phi angle

    if ( fabs(t) < 25. )
    {
      h_bbcq[ipmt]->Fill( q*r );
    }

    //cout << ipmt << ":\t" << q << "\t" << t << endl;
  }

  // Analyze other subsystems
  process_zdc( topNode );
  process_emcal( topNode );
  process_ohcal( topNode );
  process_ihcal( topNode );
}

int BbcCheck::Getpeaktime(TH1 * h)
{
  int getmaxtime, tcut = -1;

  for(int bin = 1; bin < h->GetNbinsX()+1; bin++)
  {
    double c = h->GetBinContent(bin);
    double max = h->GetMaximum();
    int bincenter = h->GetBinCenter(bin);
    if(max == c)
    {
      getmaxtime = bincenter;
      if(getmaxtime != -1) tcut = getmaxtime;
    }
  }

  return tcut;

}

void BbcCheck::process_zdc( PHCompositeNode *topNode )
{
  TowerInfoContainer* zdctowers = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_ZDC");
  if (zdctowers)
  {
    int max_zdc_t = Getpeaktime( h_zdctimecut );
    int range = 1;
    double zdc_etot = 0.;

    int size = zdctowers->size(); //online towers should be the same!
    for (int ich = 0; ich < size; ich++)
    {
      TowerInfo* zdctower = zdctowers->get_tower_at_channel(ich);
      float zdce = zdctower->get_energy();
      int zdct = zdctowers->get_tower_at_channel(ich)->get_time();
      h_zdctimecut->Fill( zdct );

      if (ich == 0 || ich == 2 || ich == 4 || ich == 8 || ich == 10 || ich == 12)
      {
        if( zdct  > (max_zdc_t - range) &&  zdct  < (max_zdc_t + range))
        {
          zdc_etot += zdce;
        }
      }
    }

    h_zdce->Fill( zdc_etot);
  }
}


void BbcCheck::process_emcal( PHCompositeNode *topNode )
{
  TowerInfoContainer* towers = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC");
  if (towers)
  {
    int max_emcal_t = Getpeaktime( h_emcaltimecut );
    int range = 1;
    double etot = 0.;

    int size = towers->size(); //online towers should be the same!
    for (int channel = 0; channel < size;channel++)
    {
      TowerInfo* tower = towers->get_tower_at_channel(channel);
      float energy = tower->get_energy();
      int t = towers->get_tower_at_channel(channel)->get_time();
      h_emcaltimecut->Fill(t);

      if( abs(t - max_emcal_t) <  range )
      {
        etot += energy;
      }
    }

    h_emcale->Fill(etot);
  }
}

void BbcCheck::process_ohcal( PHCompositeNode *topNode )
{
  TowerInfoContainer* towers = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT");
  if (towers)
  {
    int max_hcal_t = Getpeaktime( h_ohcaltimecut );
    int range = 1;
    double etot = 0.;

    int size = towers->size(); //online towers should be the same!
    for (int channel = 0; channel < size;channel++)
    {
      TowerInfo* tower = towers->get_tower_at_channel(channel);
      float energy = tower->get_energy();
      int t = towers->get_tower_at_channel(channel)->get_time();
      h_ohcaltimecut->Fill(t);

      if( abs(t - max_hcal_t) <  range )
      {
        etot += energy;
      }
    }

    h_ohcale->Fill(etot);
  }
}

void BbcCheck::process_ihcal( PHCompositeNode *topNode )
{
  TowerInfoContainer* towers = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALIN");
  if (towers)
  {
    int max_hcal_t = Getpeaktime( h_ihcaltimecut );
    int range = 1;
    double etot = 0.;

    int size = towers->size(); //online towers should be the same!
    for (int channel = 0; channel < size;channel++)
    {
      TowerInfo* tower = towers->get_tower_at_channel(channel);
      float energy = tower->get_energy();
      int t = towers->get_tower_at_channel(channel)->get_time();
      h_ihcaltimecut->Fill(t);

      if( abs(t - max_hcal_t) <  range )
      {
        etot += energy;
      }
    }

    h_ihcale->Fill(etot);
  }
}

