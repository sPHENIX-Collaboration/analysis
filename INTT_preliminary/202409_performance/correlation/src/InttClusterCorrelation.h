// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#pragma once

// std libraries
#include <string>
#include <iostream>
#include <iomanip>
#include <vector>

// ROOT libraries
#include "TFile.h"
#include "TH3I.h"
#include "TH2I.h"
#include "TH1I.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TProfile2D.h"
#include "TPaveStats.h"
#include "TPaletteAxis.h"

// Fun4All libraries
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>
#include <phool/recoConsts.h>

#include <trackbase/InttDefs.h>
#include <trackbase/InttEventInfo.h>
#include <trackbase/InttEventInfov1.h>
#include <trackbase/TrkrHitSetContainer.h>
#include <trackbase/TrkrHitSetContainerv1.h>
//#include <trackbase/TrkrHitSetContainerv2.h>
#include <trackbase/TrkrHitSetv1.h>
#include <trackbase/TrkrHitSet.h>
#include <trackbase/TrkrHitv1.h>
#include <trackbase/TrkrHit.h>
#include <trackbase/TrkrDefs.h>
#include <trackbase/TrkrClusterv4.h>
#include <trackbase/TrkrClusterContainerv4.h>
#include <trackbase/TrkrClusterHitAssocv3.h>
#include <trackbase/TrkrClusterCrossingAssocv1.h>

#include <ffaobjects/FlagSavev1.h>
#include <ffarawobjects/InttRawHit.h>
#include <ffarawobjects/InttRawHitContainer.h>

#include <intt/InttMapping.h>
#include <intt/InttDacMap.h>

#include "InttQaCommon.h"

class PHCompositeNode;

class InttClusterCorrelation : public SubsysReco
{
private:

  // general variables
  int run_num_ = 0;
  int year_ = 2024;
  int is_official_ = true;
  int fphx_bco_in_use_ = -1;

  int event_counter_by_myself_  = 0; // because the event counter is not reliable, I count it by myself for histogram normalization
  
  // variables for the output
  std::string output_dir_ = "./results/";
  std::string output_basename_ = "InttClusterCorrelation_run";
  std::string output_root_ = "";
  int chunk_num_ = 0;
  
  TFile* tf_output_;
  
  // objects to be output
  TH2D* hist_barrel_correlation_; // #hit correlation b/w the inner and the outer barrels
  TH2D* hist_barrel_correlation_no_adc0_; // #hit correlation b/w the inner and the outer barrels
  
  // nodes
  InttRawHitContainer*    node_inttrawhit_map_;
  // TRKR_HITSET (IO,TrkrHitSetContainerv1)
  TrkrHitSetContainer* node_trkrhitset_map_;
  TrkrClusterContainerv4* node_cluster_map_;

  // functions
  void DrawHists();

  void InitPaths();
  
  int GetNodes(PHCompositeNode *topNode);  

public:

  InttClusterCorrelation(const std::string &name = "InttClusterCorrelation", bool is_official = true );

  ~InttClusterCorrelation() override;

  /** Called during initialization.
      Typically this is where you can book histograms, and e.g.
      register them to Fun4AllServer (so they can be output to file
      using Fun4AllServer::dumpHistos() method).
   */
  int Init(PHCompositeNode *topNode) override;

  /** Called for first event when run number is known.
      Typically this is where you may want to fetch data from
      database, because you know the run number. A place
      to book histograms which have to know the run number.
   */
  int InitRun(PHCompositeNode *topNode) override;

  /** Called for each event.
      This is where you do the real work.
   */
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

  void SetChunkNum( int num ){ chunk_num_ = num;};
  void SetFphxBco( int val = -1 ){ fphx_bco_in_use_ = val;};
  void SetOutputDir( std::string dir = "" );
  void SetYear( int year ){ year_ = year;};
};
