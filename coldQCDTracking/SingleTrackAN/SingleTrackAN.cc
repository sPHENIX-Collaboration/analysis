#include "SingleTrackAN.h"

#include <uspin/SpinDBContent.h>
#include <uspin/SpinDBContentv1.h>

#include <calotrigger/TriggerRunInfo.h>
#include <ffarawobjects/Gl1Packet.h>

#include <trackbase_historic/SvtxTrack.h> 
#include <trackbase_historic/SvtxTrackMap.h>

#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include <ffaobjects/RunHeader.h>
#include <ffaobjects/EventHeader.h>
#include <ffarawobjects/Gl1Packet.h>

#include <phool/PHNodeIterator.h> 

#include <bcolumicount/StreamingBcoInfo.h>

#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TString.h>
#include <TTree.h>
#include <TVector3.h>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>

//____________________________________________________________________________..
SingleTrackAN::SingleTrackAN(const std::string &name):
 SubsysReco(name)
{
  std::cout << "SingleTrackAN::SingleTrackAN(const std::string &name) Calling ctor" << std::endl;
}

//____________________________________________________________________________..
SingleTrackAN::~SingleTrackAN()
{
  std::cout << "SingleTrackAN::~SingleTrackAN() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
int SingleTrackAN::Init([[maybe_unused]] PHCompositeNode *topNode)
{
  std::cout << "SingleTrackAN::Init(PHCompositeNode *topNode) Initializing" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}







//____________________________________________________________________________..
int SingleTrackAN::InitRun([[maybe_unused]] PHCompositeNode *topNode)
{
  std::cout << "SingleTrackAN::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;

  outfile = new TFile(m_output_filename.c_str(), "RECREATE");
  outfile->cd();

  SpinDBContent* spindb = findNode::getClass<SpinDBContent>(topNode, "SpinDBContent");
  if (!spindb)
  {
    std::cout << PHWHERE << ":: SpinDB node missing! Skipping run" << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }
  
  for (int i = 0; i < 120; i++)
  {
    spinPatternBlue[i] = -1*spindb->GetSpinPatternBlue(i); //-1 for IP12 -> IP8
    spinPatternYellow[i] = -1*spindb->GetSpinPatternYellow(i); //-1 for IP12 -> IP8

    std::cout << spinPatternBlue[i] << " " << spinPatternYellow[i] << std::endl;
  }

  xingshift = spindb->GetCrossingShift();
  spindb->GetPolarizationBlue(0,bluepolarization,bluepolarizationerr);
  spindb->GetPolarizationYellow(0,yellowpolarization,yellowpolarizationerr);

  hbunchnum = new TH1F("hbunchnum", "Bunch Number", 120, 0, 120);
  hsphnxbunchnum = new TH1F("hsphnxbunchnum", "Sphenix Bunch Number", 120, 0, 120);
  hmultiplicity = new TH1F("hmultiplicity", "Multiplicity", 100, 0, 100);
  hcrossing = new TH1F("hcrossing", "Crossing", 550, -150, 400);
  htrackBunch = new TH1F("htrackBunch", "Track Bunch", 120, 0, 120);
  hbspin = new TH1F("hbspin", "Blue Spin", 3, -1, 2);
  hyspin = new TH1F("hyspin", "Yellow Spin", 3, -1, 2);

  singleTrackTree = new TTree("singleTrackTree", "Single Track Analysis Tree");
  singleTrackTree->SetDirectory(outfile);
  singleTrackTree->Branch("bunchnum", &bunchnum, "bunchnum/I");
  singleTrackTree->Branch("sphnxbunchnum", &sphnxbunchnum, "sphnxbunchnum/I");
  singleTrackTree->Branch("multiplicity", &multiplicity, "multiplicity/I");
  singleTrackTree->Branch("crossing", &crossing, "crossing/I");
  singleTrackTree->Branch("trackBunch", &trackBunch, "trackBunch/I");
  singleTrackTree->Branch("bspin", &bspin, "bspin/I");
  singleTrackTree->Branch("yspin", &yspin, "yspin/I");
  singleTrackTree->Branch("bco", &bco, "bco/l");
  singleTrackTree->Branch("bcowindowlow", &bcowindowlow, "bcowindowlow/l");
  singleTrackTree->Branch("bcowindowhigh", &bcowindowhigh, "bcowindowhigh/l");
  singleTrackTree->Branch("usable_bco_tag", &usable_bco_tag, "usable_bco_tag/O");
  singleTrackTree->Branch("doublecount", &doublecount, "doublecount/O");
  singleTrackTree->Branch("p", &p, "p/F");
  singleTrackTree->Branch("pt", &pt, "pt/F");
  singleTrackTree->Branch("eta", &eta, "eta/F");
  singleTrackTree->Branch("phi", &phi, "phi/F");
  singleTrackTree->Branch("quality", &quality, "quality/F");
  singleTrackTree->Branch("charge", &charge, "charge/F");

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int SingleTrackAN::process_event([[maybe_unused]] PHCompositeNode *topNode)
{


  Gl1Packet* gl1packet = findNode::getClass<Gl1Packet>(topNode, "GL1RAWHIT");
  bunchnum = gl1packet->getBunchNumber(); // this will get me triggered crossing bunch number
  sphnxbunchnum = (bunchnum + xingshift) % 120;

  StreamingBcoInfo* bcoinfo = findNode::getClass<StreamingBcoInfo>(topNode, "STREAMINGBCOINFO");
  bco = bcoinfo->get_bco();
  usable_bco_tag = bcoinfo->get_usable_bco_tag();
  auto window = bcoinfo->get_bco_streaming_window();
  bcowindowlow = window.first;
  bcowindowhigh = window.second;


  std::cout << "BCO: " << bco << std::endl;
  std::cout << "Usable BCO Tag: " << usable_bco_tag << std::endl;
  std::cout << "BCO Streaming Window: " << bcowindowlow << " to " << bcowindowhigh << std::endl;




  SvtxTrackMap* trackmap = findNode::getClass<SvtxTrackMap>(topNode, "SvtxTrackMap");
  if (!trackmap)
  {
    trackmap = findNode::getClass<SvtxTrackMap>(topNode, "TrackMap");
    if (!trackmap)
    {
      std::cout
          << "TrackSpin::process_event - Error can not find DST trackmap node SvtxTrackMap" << std::endl;
      exit(-1);
    }
  }
  multiplicity = trackmap->size();
  for (SvtxTrackMap::Iter iter = trackmap->begin(); iter != trackmap->end(); ++iter)
  {
    
    SvtxTrack* track = iter->second;
    crossing = track->get_crossing();
    if (crossing > int(bcowindowhigh - bco))
    {
      doublecount = true;
    }
    else
    {
      doublecount = false;
    }
    charge = track->get_charge();
    quality = track->get_quality();
    p = track->get_p();
    pt = track->get_pt();
    eta = track->get_eta();
    phi = track->get_phi();

    trackBunch = crossing + sphnxbunchnum;
    while (trackBunch < 0)
    {
      trackBunch += NBUNCHES;
    }
    trackBunch %= NBUNCHES;
    
    bspin = spinPatternBlue[trackBunch];
    yspin = spinPatternYellow[trackBunch];


    singleTrackTree->Fill();
    
  }


    std::cout << "SingleTrackAN::process_event(PHCompositeNode *topNode) Processing Event" << std::endl;
    return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int SingleTrackAN::ResetEvent([[maybe_unused]] PHCompositeNode *topNode)
{
  std::cout << "SingleTrackAN::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int SingleTrackAN::EndRun(const int runnumber)
{
  std::cout << "SingleTrackAN::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int SingleTrackAN::End([[maybe_unused]] PHCompositeNode *topNode)
{
  std::cout << "SingleTrackAN::End(PHCompositeNode *topNode) This is the End..." << std::endl;  


  outfile->cd();
  singleTrackTree->Write();
  outfile->Close();



  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int SingleTrackAN::Reset([[maybe_unused]] PHCompositeNode *topNode)
{
 std::cout << "SingleTrackAN::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void SingleTrackAN::Print(const std::string &what) const
{
  std::cout << "SingleTrackAN::Print(const std::string &what) const Printing info for " << what << std::endl;
}



