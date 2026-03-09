#include "MbdLaser.h"

#include <phool/phool.h>
#include <phool/getClass.h>
#include <phool/recoConsts.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <ffaobjects/EventHeader.h>

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

//---------------------------------------------------------------->
MbdLaser::MbdLaser(const string &name) : SubsysReco(name)
{ 
  _savefname = "mbdlaser.root";
}

//----------------------------------------------------------------->
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
  _tree->Branch("tmean",&f_tmean,"tmean/F");
  _tree->Branch("tmerr",&f_tmerr,"tmerr/F");

  TString name, title;

  //charge 
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
  //Time 
  for (int ipmt=0; ipmt<128; ipmt++)
  { 
    //Time A for each ch filled with (t)
    name = "h_mbdt"; name += ipmt;
    title = "mbd Time, ch "; title += ipmt;
    h_mbdt[ipmt] = new TH1F(name,title,128,-0.5,127.5);


    // TGraph to track mean of mbdq distribution
    name = "g_mbdt"; name += ipmt;
    title = "mbdt, ch "; title += ipmt;
    g_mbdt[ipmt] = new TGraphErrors();
    g_mbdt[ipmt]->SetName(name);
    g_mbdt[ipmt]->SetTitle(name);


  }

  for (int iarm=0; iarm<2; iarm++)
  {
    //
    name = "hevt_mbdt"; name += iarm;
    title = "mbd times, arm "; title += iarm;
    hevt_mbdt[iarm] = new TH1F(name,title,200,7.5,11.5);
    hevt_mbdt[iarm]->SetLineColor(4);
  }

  // Time vs ch filled with (btt,pmt)
  h2_mbdbtt = new TH2F("h2_mbdtt","Time[ADC] vs Ch",128,-0.5,127.5,250,0.,25.);
  h2_mbdbtt->GetXaxis()->SetTitle("ch");
  h2_mbdbtt->GetYaxis()->SetTitle("Time");

  //charge A vs ch D-2 filled with(bq,pmt)
  h2_mbdbq = new TH2F("h2_mbdbq","Charge[ADC] vs Ch", 128,-0.5,127.5,1600,0.,16000.);
  h2_mbdbq->GetXaxis()->SetTitle("ch");
  h2_mbdbq->GetYaxis()->SetTitle("Charge");





  c_mbdt = new TCanvas("c_mbdt","MBD Times",1200,800);
  c_mbdt->Divide(1,2);

  return 0;
}

//------------------------------------------------------------------>
int MbdLaser::InitRun(PHCompositeNode *topNode)
{
  recoConsts *rc = recoConsts::instance();
  f_run = rc->get_IntFlag("RUNNUMBER");

  GetNodes(topNode);

  return 0;
}

//-------------------------------------------------------------------->
//Call user instructions for every event
int MbdLaser::process_event(PHCompositeNode *topNode)
{
  nprocessed++;

  // skip the first 100 events
  // these are bad since the laser takes time to ramp up
  f_evt = _evtheader->get_EvtSequence();
  if ( f_evt<100 )
  {
    return Fun4AllReturnCodes::DISCARDEVENT;
  }

  //if(f_evt%1==0) cout << PHWHERE << "Event " << f_evt << "\t" << ", nprocessed = " << nprocessed << endl;

  // Initialize Variables
  f_mbdt[0] = -9999.;
  f_mbdt[1] = -9999.;
  f_mbdte[0] = -9999.;
  f_mbdte[1] = -9999.;
  f_mbdt0 = NAN;
  hevt_mbdt[0]->Reset();
  hevt_mbdt[1]->Reset();

  for (int ipmt=0; ipmt<_mbdpmts->get_npmt(); ipmt++)
  {
    Float_t q = _mbdpmts->get_pmt(ipmt)->get_q();
    Float_t t = _mbdpmts->get_pmt(ipmt)->get_time();
    //Float_t phi = mbdgeom->get_phi(ipmt);   // get phi angle

    h_mbdq[ipmt]->Fill( q );
    //cout << ipmt << ":\t" << q << "\t" << t << endl;

    h_mbdt[ipmt]->Fill( t );
    // cout << ipmt << ":\t" << q << "\t" << t << endl;

    // declear variables 
    Float_t bq   = _mbdpmts->get_pmt(ipmt)->get_q();
    Float_t btt  = _mbdpmts->get_pmt(ipmt)->get_time();
    Short_t pmt =_mbdpmts->get_pmt(ipmt)->get_pmt();
    //cout << " bq = " << bq << ":\t" << "btt = " <<btt << ":\t" << "pmt = " <<pmt<<endl;


    //Fill 2D charge vs ch and Time vs ch
    h2_mbdbtt->Fill(pmt,btt);
    h2_mbdbq->Fill(pmt,bq);

  }


  // LaserDST(topNode);

  return 0;
}

//-------------------------------------------------------------------->
void MbdLaser::GetNodes(PHCompositeNode *topNode)
{
  // Get the DST objects

  _evtheader = findNode::getClass<EventHeader>(topNode, "EventHeader");
  if (!_evtheader && f_evt<10) cout << PHWHERE << " EventHeader node not found on node tree" << endl;

  // MbdOut
  _mbdout = findNode::getClass<MbdOut>(topNode, "MbdOut");
  if(!_mbdout && f_evt<4) cout << PHWHERE << " MbdOut node not found on node tree" << endl;

  // MbdPmt Info
  _mbdpmts = findNode::getClass<MbdPmtContainer>(topNode, "MbdPmtContainer");
  if(!_mbdpmts && f_evt<4) cout << PHWHERE << " MbdPmtContainer node not found on node tree" << endl;

}

//------------------------------------------------------------------->
int MbdLaser::End(PHCompositeNode *topNode)
{
  _savefile->cd();

  //Filling g_mbdq and g_mbdt
  for (int ipmt=0; ipmt<MBD_N_PMT; ipmt++)
  {

    // Fill info on q distribution and t
    f_ch = ipmt;
    f_qmean = h_mbdq[ipmt]->GetMean();
    f_qmerr = h_mbdq[ipmt]->GetMeanError();
    f_tmean = h_mbdt[ipmt]->GetMean();
    f_tmerr = h_mbdt[ipmt]->GetMeanError();

    _tree->Fill();

    //cout << "run =  "<< f_run << "\t" << " pmt =  "<< f_ch << "\t" <<" qmean =  "<< f_qmean << "\t" << " qmerr =  "<<f_qmerr << "\t"<< "tmean =  "<< f_tmean << "\t" << "tmerr = " << f_tmerr<< endl;;

    g_mbdq[ipmt]->SetPoint(0,f_run,f_qmean);
    g_mbdq[ipmt]->SetPointError(0,0,f_qmerr);
    g_mbdq[ipmt]->Write();

    g_mbdt[ipmt]->SetPoint(0,f_run,f_tmean);
    g_mbdt[ipmt]->SetPointError(0,0,f_tmerr);
    g_mbdt[ipmt]->Write();

  }

  _savefile->Write();
  _savefile->Close();

  return 0;
}

void MbdLaser::LaserDST(PHCompositeNode *topNode)
{

  //Float_t r = (4.4049/4.05882);
  Float_t r = 1.0;

  // Fill info from each PMT
  for (int ipmt=0; ipmt<_mbdpmts->get_npmt(); ipmt++)
  {
    Float_t q = _mbdpmts->get_pmt(ipmt)->get_q();
    Float_t t = _mbdpmts->get_pmt(ipmt)->get_time();
    //Float_t phi = mbdgeom->get_phi(ipmt);   // get phi angle

    if ( fabs(t) < 25. )
    {
      h_mbdq[ipmt]->Fill( q*r );
      h_mbdt[ipmt]->Fill(t);
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


