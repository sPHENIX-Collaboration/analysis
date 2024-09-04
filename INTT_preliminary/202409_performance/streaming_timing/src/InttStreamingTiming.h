// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#pragma once

// std libraries
#include <string>
#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>

// ROOT libraries
#include <TH2D.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TFile.h>

// Fun4All libraries
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>
#include <phool/recoConsts.h>

#include <trackbase/InttDefs.h>
//#include <trackbase/InttEventInfo.h>
#include <trackbase/InttEventInfov1.h>
//#include <trackbase/TrkrHitSetContainer.h>
#include <trackbase/TrkrHitSetContainerv1.h>
//#include <trackbase/TrkrHitSetContainerv2.h>
#include <trackbase/TrkrHitSetv1.h>
#include <trackbase/TrkrHitv1.h>
#include <trackbase/TrkrDefs.h>

#include <ffaobjects/FlagSavev1.h>
#include <ffarawobjects/InttRawHit.h>
#include <ffarawobjects/InttRawHitContainer.h>
#include <ffarawobjects/Gl1RawHitv2.h> // v2 <-- v1 <-- v0 : confirmed, it has to be so
#include <ffarawobjects/Gl1Packetv2.h>

#include <intt/InttMapping.h>
#include <intt/InttDacMap.h>

#include "InttQaCommon.h"

class PHCompositeNode;

class InttStreamingTiming : public SubsysReco
{
private:

  // general variables
  int run_num_ = 0;
  int event_counter_ = 0;
  int event_counter_by_myself_  = 0; // because the event counter is not reliable, I count it by myself for histogram normalization
  int required_trigger_bit_ = -1;
  
  // variables for the output
  std::string output_dir_ = "./results/";
  std::string output_basename_ = "InttStreamingTiming_run";
  std::string output_root_ = "";
  std::string output_txt_ = "";
  std::ofstream ofs_;
  TFile* tf_output_;
  
  // objects to be output
  TH1D* hist_fphx_bco_; // no ADC0
  TH1D* hist_fphx_bco_raw_; // includes all hits
  TH1D* hist_streaming_offset_; // INTT hit BCO - GL1 BCO = (INTT GTM BCO + FPHX BCO) - GL1 BCO
  
  int event_max_ = 10000;
  
  // nodes
  Gl1Packet* gl1_;
  InttRawHitContainer* node_inttrawhit_map_;
  TrkrHitSetContainer* node_trkrhitset_map_; // // TRKR_HITSET (IO,TrkrHitSetContainerv1)
  
  // functions
  void DrawHists();
  vector < InttRawHit* > GetRawHits();
  vector < InttRawHit* > GetRawHitsWithoutClone();
  std::vector < std::pair < InttNameSpace::Online_s, unsigned int > >
    GetHits( TrkrHitSetContainer::ConstRange hitsets ); //! Draw hits and save them into a PDF file
  std::vector < std::pair < uint16_t, int > >
  GetBcoEventCounter();
  std::vector < int > GetTriggerBits();
  bool IsSame( InttRawHit* hit1, InttRawHit* hit2 );
  
  int GetNodes(PHCompositeNode *topNode);

  string trigger_names_[32]
  = {
    "Clock"		     , "ZDC South"              , "ZDC North"               , "ZDC N&S"                 , "HCAL Singles",
    "HCAL Coincidence"	     , ""                       , ""                        , "MBD S>=1"                , "MBD N>=1",
    "MBD N&S>=1"	     , "MBD N&S>=2"             , "MBD N&S>=1, vtx<10cm"    , "MBD N&S>=1, vtx<30cm"    , "MBD N&S>=1, vtx<60cm",
    "HCAL Singles+MBD NS>=1" , "Jet 6GeV+MBD NS>=1"     , "Jet 8GeV+MBD NS>=1"      , "Jet 10GeV+MBD NS>=1"     , "Jet 12GeV+MBD NS>=1",
    "Jet 6GeV"		     , "Jet 8GeV"               , "Jet 10GeV"               , "Jet 12GeV"               , "Photon 2GeV+MBD NS>=1",
    "Photon 3GeV+MBD NS>=1"  , "Photon 4GeV+MBD NS>=1"  , "Photon 5GeV+MBD NS>=1"
  };

public:

  InttStreamingTiming(const std::string &name = "InttStreamingTiming" );

  ~InttStreamingTiming() override;

  int Init(PHCompositeNode *topNode) override;

  int InitRun(PHCompositeNode *topNode) override;

  int process_event(PHCompositeNode *topNode) override;

  /// Clean up internals after each event.
  int ResetEvent(PHCompositeNode *topNode) override;

  /// Called at the end of each run.
  int EndRun(const int runnumber) override;

  /// Called at the end of all processing.
  int End(PHCompositeNode *topNode) override;

  /// Reset
  int Reset(PHCompositeNode * /*topNode*/) override;

  void Print(const std::string &what = "ALL") const override;

  // 
  int GetEventMax(){ return event_max_;};

  void SetOutputDir( std::string dir = "" );
  void SetMaxEvent( int num ){ event_max_ = num;};
  void SetTriggerRequirement( int val ){ required_trigger_bit_ = val;};

};
