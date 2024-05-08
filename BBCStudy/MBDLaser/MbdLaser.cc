#include "MbdLaser.h"

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
MbdLaser::MbdLaser(const string &name) : SubsysReco(name),
    _tree( 0 ),
    f_evt( 0 ),
    _tres( 0.05 ),
    _savefname( "mbdlaser.root" ),
    _savefile( 0 )
{ 

}

//___________________________________
int MbdLaser::Init(PHCompositeNode *topNode)
{
  cout << PHWHERE << " Saving to file " << _savefname << endl;
  //PHTFileServer::get().open( _outfile, "RECREATE");
  _savefile = new TFile( _savefname.c_str(), "RECREATE" );

  _tree = new TTree("t","MbdLaser");
  _tree->Branch("run",&f_run,"run/I");
  _tree->Branch("ch",&f_ch,"ch/I");
  _tree->Branch("qmean",&f_qmean,"qmean/F");
  _tree->Branch("qmerr",&f_qmerr,"qmerr/F");

  TString name, title;
  for (int ipmt=0; ipmt<128; ipmt++)
  {
    name = "h_mbdq"; name += ipmt;
    title = "mbd charge, ch "; title += ipmt;
    h_mbdq[ipmt] = new TH1F(name,title,1600,0,16000);

    // TGraph to track mean of mbdq distribution
    name = "g_mbdq"; name += ipmt;
    title = "mbdq, ch "; title += ipmt;
    g_mbdq[ipmt] = new TGraphErrors();
    g_mbdq[ipmt]->SetName(name);
    g_mbdq[ipmt]->SetTitle(name);

    /*
    name = "h_tdiff_ch"; name += ipmt;
    title = "tdiff, ch "; title += ipmt;
    h_tdiff_ch[ipmt] = new TH1F(name,title,600,-3,3);
    */
  }

  
  c_mbdt = new TCanvas("c_mbdt","MBD Times",1200,800);
  c_mbdt->Divide(1,2);

  return 0;
}

//___________________________________
int MbdLaser::InitRun(PHCompositeNode *topNode)
{
  recoConsts *rc = recoConsts::instance();
  f_run = rc->get_IntFlag("RUNNUMBER");

  GetNodes(topNode);

  return 0;
}

//__________________________________
//Call user instructions for every event
int MbdLaser::process_event(PHCompositeNode *topNode)
{
  nprocessed++;

  
  // Initialize Variables
  f_mbdt[0] = -9999.;
  f_mbdt[1] = -9999.;
  f_mbdte[0] = -9999.;
  f_mbdte[1] = -9999.;
  f_mbdt0 = NAN;
  hevt_mbdt[0]->Reset();
  hevt_mbdt[1]->Reset();

 
  LaserDST(topNode);

  return 0;
}

//___________________________________
void MbdLaser::GetNodes(PHCompositeNode *topNode)
{
  // Get the DST objects

  //_evtheader = findNode::getClass<EventHeaderv1>(topNode, "EventHeader");
  //if (!_evtheader && f_evt<10) cout << PHWHERE << " EventHeader node not found on node tree" << endl;

  // MbdOut
  _mbdout = findNode::getClass<MbdOut>(topNode, "MbdOut");
  if(!_mbdout && f_evt<4) cout << PHWHERE << " MbdOut node not found on node tree" << endl;

  // MbdPmt Info
  _mbdpmts = findNode::getClass<MbdPmtContainer>(topNode, "MbdPmtContainer");
  if(!_mbdpmts && f_evt<4) cout << PHWHERE << " MbdPmtContainer node not found on node tree" << endl;

}

//______________________________________
int MbdLaser::End(PHCompositeNode *topNode)
{
  _savefile->cd();

  
  for (int ipmt=0; ipmt<MBD_N_PMT; ipmt++)
  {
    //h_mbdq[ipmt]->Scale( norm );

    // Fill info on q distribution
    f_ch = ipmt;
    f_qmean = h_mbdq[ipmt]->GetMean();
    f_qmerr = h_mbdq[ipmt]->GetMeanError();
    _tree->Fill();
    cout << "run =  "<< f_run << "\t" << " pmt =  "<< f_ch << "\t" <<" qmean =  "<< f_qmean << "\t" << " qmerr =  "<<f_qmerr << endl;

    g_mbdq[ipmt]->SetPoint(0,f_run,f_qmean);
    g_mbdq[ipmt]->SetPointError(0,0,f_qmerr);
    g_mbdq[ipmt]->Write();
  }

  //cout << "Nevents processed integral " << nprocessed << "\t" << nevents << "\t" << nevents/nprocessed << endl;
  _savefile->Write();
  _savefile->Close();

  return 0;
}

void MbdLaser::LaserDST(PHCompositeNode *topNode)
{
  


  
  //Float_t r = (4.4049/4.05882);
  Float_t r = 1.0;

  //Fill info from each PMT
  for (int ipmt=0; ipmt<_mbdpmts->get_npmt(); ipmt++)
  {
    Float_t q = _mbdpmts->get_pmt(ipmt)->get_q();
    Float_t t = _mbdpmts->get_pmt(ipmt)->get_time();
    //Float_t phi = mbdgeom->get_phi(ipmt);   // get phi angle

    if ( fabs(t) < 25. )
    {
      h_mbdq[ipmt]->Fill( q*r );
     // cout << ipmt << ":\t" << q << "\t" << t << endl;
    }

   // cout << ipmt << ":\t" << q << "\t" << t << endl;
  }

}

int MbdLaser::Getpeaktime(TH1 * h)
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

