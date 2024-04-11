// ----------------------------------------------------------------------------
// 'TrackJetQAMaker.h'
// Derek Anderson
// 03.25.2024
//
// A small Fun4All module to produce QA plots for tracks,
// hits, and more.
// ----------------------------------------------------------------------------

#ifndef TRACKJETQAMAKER_H
#define TRACKJETQAMAKER_H

// c++ utilities
#include <string>
#include <vector>
#include <utility>
#include <optional>
// root libraries
#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
// fun4all libraries
#include <fun4all/SubsysReco.h>
#include <phool/PHCompositeNode.h>
#include <fun4all/Fun4AllReturnCodes.h>
// module tools
#include "TrackJetQAMakerConfig.h"
#include "TrackJetQAMakerHelper.h"
#include "TrackJetQAMakerHistDef.h"
// submodule definitions
#include "HitQAMaker.h"
#include "ClustQAMaker.h"
#include "TrackQAMaker.h"
#include "JetQAMaker.h"



// TrackJetQAMaker definition --------------------------------------------

class TrackJetQAMaker : public SubsysReco {

  public:

    // ctor/dtor
    TrackJetQAMaker(const std::string &name = "TrackJetQAMaker", const std::string& sOutFileName = "tracksinjets.root");
    ~TrackJetQAMaker() override;

    // public methods
    void Configure(TrackJetQAMakerConfig config, std::optional<TrackJetQAMakerHistDef> hist = std::nullopt);

    // f4a methods
    int Init(PHCompositeNode* topNode)          override;
    int process_event(PHCompositeNode* topNode) override;
    int End(PHCompositeNode* topNode)           override;

  private:

    // io members
    TFile*      m_outFile     = NULL;
    std::string m_outFileName = "";

    // submodules to run
    HitQAMaker*   m_hitMaker   = NULL;
    ClustQAMaker* m_clustMaker = NULL;
    TrackQAMaker* m_trackMaker = NULL;
    JetQAMaker*   m_jetMaker   = NULL;

    // module utilities
    TrackJetQAMakerConfig  m_config;
    TrackJetQAMakerHelper  m_help;
    TrackJetQAMakerHistDef m_hist;

};  // end TrackJetQAMaker

#endif

// end ------------------------------------------------------------------------
