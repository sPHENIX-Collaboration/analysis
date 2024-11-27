#include "BbcCheck.h"

#include <phool/phool.h>
#include <phool/getClass.h>
#include <phool/recoConsts.h>
#include <fun4all/Fun4AllServer.h>
#include <ffaobjects/EventHeaderv1.h>
#include <ffarawobjects/Gl1Packet.h>
#include <fun4all/Fun4AllReturnCodes.h>

#include <mbd/MbdDefs.h>
#include <mbd/MbdOut.h>
#include <mbd/MbdPmtContainer.h>
#include <mbd/MbdPmtHit.h>
#include <mbd/MbdGeom.h>

#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfo.h>
#include <globalvertex/GlobalVertexMap.h>
#include <globalvertex/GlobalVertex.h>
#include <globalvertex/MbdVertexMap.h>
#include <globalvertex/MbdVertex.h>

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

//____________________________________
BbcCheck::BbcCheck(const string &name) : SubsysReco(name),
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

  _tree2 = new TTree("t2","BbcCheck Event Tree");
  _tree2->Branch("run",&f_run,"run/I");
  _tree2->Branch("evt",&f_evt,"evt/I");
  _tree2->Branch("cross",&f_cross,"cross/S");
  _tree2->Branch("rtrig",&f_rtrig,"rtrig/l");
  _tree2->Branch("ltrig",&f_ltrig,"ltrig/l");
  _tree2->Branch("strig",&f_strig,"strig/l");
  _tree2->Branch("bz",&f_bz,"bz/F");

  TString name, title;
  for (int ipmt=0; ipmt<128; ipmt++)
  {
    name = "h_q"; name += ipmt;
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

    name = "h2_bbcqtot_bz"; name += iarm;
    title = "bbc charge, arm "; title += iarm; title += " vs z";
    h2_bbcqtot_bz[iarm] = new TH2F(name,title,20,-50.,50.,1000,0.,3000.);

    //
    name = "hevt_bbct"; name += iarm;
    title = "bbc times, arm "; title += iarm;
    hevt_bbct[iarm] = new TH1F(name,title,200,7.5,11.5);
    hevt_bbct[iarm]->SetLineColor(4);
  }
  h_bbcqsum = new TH1F("h_bbcqsum","MBD/BBC north + south charge sum",3000,0.,3000.);
  h2_bbcqsum = new TH2F("h2_bbcqsum","north BBCQ vs South BBCQ",1400,0.,1400.,1400,0.,1400.);

  h2_bbcqsum_bz = new TH2F("h2_bbcqsum_bz","BBCQsum vs z",20,-50.,50.,1000,0.,3000.);

  h_zdce = new TH1F("h_zdce","ZDC Energy",820,-50,4050);
  h_zdcse = new TH1F("h_zdcse","ZDC.S Energy",500,0,250);
  h_zdcne = new TH1F("h_zdcne","ZDC.N Energy",500,0,250);
  h_zdctimecut = new TH1F("h_zdctimecut", "zdctimecut", 50, -17.5 , 32.5);

  h_emcale = new TH1F("h_emcale","Emcal Energy",3000,-100,2900);
  h_emcaltimecut = new TH1F("h_emcaltimecut", "emcaltimecut", 50, -17.5 , 32.5);

  h_ohcale = new TH1F("h_ohcale","OHCAL Energy",1000,-100,900);
  h_ohcaltimecut = new TH1F("h_ohcaltimecut", "ohcaltimecut", 50, -17.5 , 32.5);

  h_ihcale = new TH1F("h_ihcale","IHCAL Energy",1000,-100,900);
  h_ihcaltimecut = new TH1F("h_ihcaltimecut", "ihcaltimecut", 50, -17.5 , 32.5);

  h_bz = new TH1F("h_bz","MBD z-vertex",1200,-300,300);
  h_bz->SetXTitle("z_{VTX} [cm]");
  for (int itrig=0; itrig<5; itrig++)
  {
    name = "h_bz"; name += itrig;
    title = "MBD z-vertex, trig "; title += itrig;
    h_bztrig[itrig] = new TH1F(name,title,1200,-300,300);
    h_bztrig[itrig]->SetXTitle("z_{VTX} [cm]");
  }

  h_bpmt_bad = new TH1F("h_bpmt_bad","PMT for BAD MBD z-vertex",128,0,128);

  
  for (int ipmt=0; ipmt<128; ipmt++)
  {
    name = "h2_slew"; name += ipmt;
    title = "slew curve, ch "; title += ipmt;
    h2_slew[ipmt] = new TH2F(name,title,4000,0.,100,1100,-5,6);
  }
  h2_tq = new TH2F("h2_tq","ch vs tq",900,-150,150,128,-0.5,128-0.5);
  h2_tt = new TH2F("h2_tt","ch vs tt",900,-150,150,128,-0.5,128-0.5);

  gaussian = new TF1("gaussian","gaus",0,20);
  gaussian->FixParameter(2,0.05);   // set sigma to 50 ps

  c_bbct = new TCanvas("c_bbct","BBC Times",1200,800);
  c_bbct->Divide(1,2);

  // MBD triggers
  mbdtrigbits.push_back(0x0400);    // MBDNS>=1(pp) or 2(AuAu)
  mbdtrigbits.push_back(0x0800);    // MBDNS>=2(pp) or 1(AuAu)
  mbdtrigbits.push_back(0x1000);    // MBD60 (or MBD10)
  mbdtrigbits.push_back(0x2000);    // MBD30 (or MBD30)
  mbdtrigbits.push_back(0x4000);    // MBD10 (or MBD60)

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
  // Get the raw gl1 data from event combined DST
  _gl1raw = findNode::getClass<Gl1Packet>(topNode, "GL1Packet");
  if(!_gl1raw && f_evt<4) cout << PHWHERE << " Gl1Packet node not found on node tree" << endl;

  nprocessed++;

  f_evt = _evtheader->get_EvtSequence();
  if (f_evt%1000==0) cout << PHWHERE << "Event " << f_evt << "\t" << ", nprocessed = " << nprocessed << endl;

  // Only use MBDNS triggered events
  if ( _gl1raw != nullptr )
  {
    const uint64_t MBDTRIGS = 0x7c00;  // MBDNS trigger bits
    const uint64_t ZDCNS = 0x8;        // ZDCNS trigger bits

    f_cross = _gl1raw->getBunchNumber();
    f_rtrig = _gl1raw->getTriggerVector();
    f_ltrig = _gl1raw->getLiveVector();
    f_strig = _gl1raw->getScaledVector();

    if ( (f_strig&MBDTRIGS) == 0 )
    {
      return Fun4AllReturnCodes::ABORTEVENT;
    }

    if ( nprocessed<10 )
    {
      std::cout << "Trig " << nprocessed << std::endl;
      std::cout << hex;
      std::cout << "Raw\t" << f_rtrig << std::endl;
      std::cout << "Live\t" << f_ltrig << std::endl;
      std::cout << "Scaled\t" << f_strig << std::endl;
      std::cout << "AND\t" << (f_strig&MBDTRIGS) << std::endl;
      std::cout << dec;

    }
    /*
    // before some run, there was only the trigger vector...
    uint64_t trigvec = _gl1raw->getTriggerVector();  // raw trigger only
    if ( (trigvec&MBDTRIGS) == 0 )
    {
      return Fun4AllReturnCodes::ABORTEVENT;
    }
    */
  }

  // Initialize Variables
  f_bbcn[0] = 0;
  f_bbcn[1] = 0;
  f_bbcq[0] = 0.;
  f_bbcq[1] = 0.;
  f_bbct[0] = -9999.;
  f_bbct[1] = -9999.;
  f_bbcte[0] = -9999.;
  f_bbcte[1] = -9999.;
  f_bz = NAN;
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

  _tree2->Fill();
  return 0;
}

//___________________________________
void BbcCheck::GetNodes(PHCompositeNode *topNode)
{
  // Get the DST objects

  _evtheader = findNode::getClass<EventHeaderv1>(topNode, "EventHeader");
  if (!_evtheader && f_evt<10) cout << PHWHERE << " EventHeader node not found on node tree" << endl;

  // MbdOut
  _mbdout = findNode::getClass<MbdOut>(topNode, "MbdOut");
  if(!_mbdout && f_evt<4) cout << PHWHERE << " MbdOut node not found on node tree" << endl;

  // MbdPmt Info
  _mbdpmts = findNode::getClass<MbdPmtContainer>(topNode, "MbdPmtContainer");
  if(!_mbdpmts && f_evt<4) cout << PHWHERE << " MbdPmtContainer node not found on node tree" << endl;


}

//______________________________________
int BbcCheck::End(PHCompositeNode *topNode)
{
  _savefile->cd();

  Double_t nevents = h_bbcqsum->Integral();
  h_bbcqsum->Fill(-1000,nevents); // underflow bin keeps track of nevents
  h_bbcqtot[0]->Fill(-1000,nevents); // underflow bin keeps track of nevents
  h_bbcqtot[1]->Fill(-1000,nevents); // underflow bin keeps track of nevents

  /*
  Double_t norm = 1.0/nevents;
  h_bbcqsum->Scale( norm );
  h2_bbcqsum->Scale( norm );

  h_bbcqtot[0]->Scale( norm );
  h_bbcqtot[1]->Scale( norm );
  */

  for (int ipmt=0; ipmt<BBC_N_PMT; ipmt++)
  {
    //h_bbcq[ipmt]->Scale( norm );

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
  //cout << "In CheckDST" << endl;
  //Float_t bqs = _mbdout->get_q(0)/120.;
  //Float_t bqn = _mbdout->get_q(1)/120.;
  Float_t bqs = _mbdout->get_q(0);
  Float_t bqn = _mbdout->get_q(1);
  f_bz = _mbdout->get_zvtx();
  Float_t btarm[2];
  btarm[0] = _mbdout->get_time(0);
  btarm[1] = _mbdout->get_time(1);

//  h_bz->Fill( f_bz );

  // Check the MbdVertexMap
  MbdVertexMap *mbdvtxmap = findNode::getClass<MbdVertexMap>(topNode, "MbdVertexMap");
  if ( mbdvtxmap && !mbdvtxmap->empty() )
  {
    MbdVertex *vtx = mbdvtxmap->begin()->second;
    if ( vtx )
    {
      float vtx_z = vtx->get_z();
      if ( !isnan(f_bz) && vtx_z != f_bz )
      {
        cout << "ERROR, mbdvertexmap does not match " << vtx_z << "\t" << f_bz << endl;
      }
      else
      {
        static int counter = 0;
        if ( counter<3 )
        {
          cout << "GOOD, mbdvertexmap vertex matches " << vtx_z << "\t" << f_bz << endl;
          counter++;
        }
      }
    }
  }
  else
  {
    static int counter = 0;
    if ( counter < 4 )
    {
      if ( mbdvtxmap && mbdvtxmap->empty() )
      {
        cout << "MbdVertexMap is empty" << endl;
      }
      else
      {
        cout << "MbdVertexMap not found" << endl;
      }
      counter++;
    }
  }

  // Check the GlobalVertex
  GlobalVertexMap *vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
  if ( vertexmap && !vertexmap->empty() )
  {
    GlobalVertex *vtx = vertexmap->begin()->second;
    if ( vtx )
    {
      float vtx_z = vtx->get_z();
      if ( vtx_z != f_bz )
      {
        cout << "ERROR, vertices do not match " << vtx_z << "\t" << f_bz << endl;
      }
      else
      {
        static int counter = 0;
        if ( counter<3 )
        {
          cout << "GOOD, globalvertexmap vertex matches " << vtx_z << "\t" << f_bz << endl;
          counter++;
        }
      }
    }
  }
  else
  {
    static int counter = 0;
    if ( counter < 4 )
    {
      if ( vertexmap && vertexmap->empty() )
      {
        cout << "GlobalVertexMap is empty" << endl;
      }
      else
      {
        cout << "GlobalVertexMap not found" << endl;
      }
      counter++;
    }
  }

  //cout << "bz " << f_bz << endl;
  if ( fabs(f_bz)>3000. ) return;
  //if ( bqn<10 && bqs>150 ) return;

  //Float_t r = (4.4049/4.05882);
  Float_t r = 1.0;

  h_bbcqsum->Fill( (bqn+bqs)*r );
  h_bbcqtot[0]->Fill( bqs*r );
  h_bbcqtot[1]->Fill( bqn*r );
  h2_bbcqsum->Fill( bqn*r, bqs*r );

  h2_bbcqtot_bz[0]->Fill( f_bz, bqs );
  h2_bbcqtot_bz[1]->Fill( f_bz, bqn );
  h2_bbcqsum_bz->Fill( f_bz, bqn+bqs );

  //cout << "npmts " << _mbdpmts->get_npmt() << endl;
  // Fill info from each PMT
  //for (int ipmt=0; ipmt<_mbdpmts->get_npmt(); ipmt++)
  for (int ipmt=0; ipmt<128; ipmt++)
  {
    int arm = ipmt/64;
    //Float_t q = _mbdpmts->get_pmt(ipmt)->get_q()/120.;
    Float_t q = _mbdpmts->get_pmt(ipmt)->get_q();
    Float_t t = _mbdpmts->get_pmt(ipmt)->get_time();
    Float_t tt = _mbdpmts->get_pmt(ipmt)->get_tt();
    Float_t tq = _mbdpmts->get_pmt(ipmt)->get_tq();
    //Float_t phi = mbdgeom->get_phi(ipmt);   // get phi angle

    if ( fabs(t) < 25. )
    {
      h_bbcq[ipmt]->Fill( q*r );
    }

    h2_tt->Fill( t, ipmt );
    h2_tq->Fill( tq, ipmt );

    h2_slew[ipmt]->Fill( q, t - btarm[arm] );
    //cout << ipmt << ":\t" << q << "\t" << t << endl;
 
  }

  h_bz->Fill( f_bz );
  for (size_t itrig=0; itrig<mbdtrigbits.size(); itrig++)
  {
    if ( f_ltrig&mbdtrigbits[itrig] )
    {
      h_bztrig[itrig]->Fill( f_bz );
    }
  }

  // Analyze other subsystems
  //process_gl1( topNode );
  //process_zdc( topNode );
  /*
  process_emcal( topNode );
  process_ohcal( topNode );
  process_ihcal( topNode );
  */
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

void BbcCheck::process_gl1( PHCompositeNode *topNode )
{
  TowerInfoContainer* zdctowers = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_ZDC");
  if (zdctowers)
  {
    int max_zdc_t = Getpeaktime( h_zdctimecut );
    int range = 1;
    double zdc_etot = 0.;
    double zdc_e[2] {0.,0.};

    int size = zdctowers->size(); //online towers should be the same!
    for (int ich = 0; ich < size; ich++)
    {
      TowerInfo* zdctower = zdctowers->get_tower_at_channel(ich);
      float zdce = zdctower->get_energy();
      int zdct = zdctowers->get_tower_at_channel(ich)->get_time();
      h_zdctimecut->Fill( zdct );

      if ( (zdct  < (max_zdc_t - range)) ||  (zdct  > (max_zdc_t + range)) )
      {
        continue;
      }

      //
      if (ich==0||ich==2||ich==4)
      {
        zdc_e[0] += zdce;
      }
      else if (ich == 8 || ich == 10 || ich == 12)
      {
        zdc_e[1] += zdce;
      }

    }

    h_zdcse->Fill( zdc_e[0] );
    h_zdcne->Fill( zdc_e[1] );
    zdc_etot = zdc_e[0] + zdc_e[1];
    h_zdce->Fill( zdc_etot );
  }
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

