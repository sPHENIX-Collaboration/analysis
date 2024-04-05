// ----------------------------------------------------------------------------
// 'TrackQAMaker.h'
// Derek Anderson
// 03.25.2024
//
// A submodule for the TrackJetQAMaker module
// to generate QA plots for tracks, inclusive
// and in jets
// ----------------------------------------------------------------------------

#ifndef TRACKJETQAMAKER_TRACKQAMAKER_H
#define TRACKJETQAMAKER_TRACKQAMAKER_H

// c++ utilities
#include <limits>
#include <string>
#include <vector>
#include <cassert>
#include <utility>
#include <optional>
// root libraries
#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
#include <TDirectory.h>
// f4a libraries
#include <phool/PHCompositeNode.h>
// phool libraries
#include <phool/phool.h>
#include <phool/getClass.h>
// tracking libraries
#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrackMap.h>
// module tools
#include "TrackJetQAMakerHelper.h"
#include "TrackJetQAMakerHistDef.h"



// TrackQAMaker definition ------------------------------------------------------

class TrackQAMaker {

  public:

    // histogram accessors
    //   - TODO split tracks into seed types
    enum Type {All};
    enum H1D  {Eta, Phi, Pt, Qual};
    enum H2D  {EtaVsPhi, PtVsQual};

    // histogram content
    struct TrackQAContent {
      double eta  = std::numeric_limits<double>::max();
      double phi  = std::numeric_limits<double>::max();
      double pt   = std::numeric_limits<double>::max();
      double qual = std::numeric_limits<double>::max();
    };

    // ctor/dtor
    TrackQAMaker() {};
    ~TrackQAMaker() {};

    // public methods
    void Init(TrackJetQAMakerHistDef& hist, TrackJetQAMakerHelper& help);
    void Process(PHCompositeNode* topNode);
    void End(TFile* outFile, std::string outDirName);

  private:

    // private methods
    void BuildHistograms();
    void FillHistograms(Type type, TrackQAContent& content);

    // necessary dst nodes
    SvtxTrackMap* m_trkMap = NULL;

    // histograms
    std::vector<std::vector<TH1D*>> vecHist1D;
    std::vector<std::vector<TH2D*>> vecHist2D;

    // module utilities
    TrackJetQAMakerHelper  m_help;
    TrackJetQAMakerHistDef m_hist;

};  // end TrackQAMaker

#endif

// end ------------------------------------------------------------------------

