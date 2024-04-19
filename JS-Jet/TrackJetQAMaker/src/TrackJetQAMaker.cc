// ----------------------------------------------------------------------------
// 'TrackJetQAMaker.cc'
// Derek Anderson
// 03.25.2024
//
// A small Fun4All module to produce QA plots for tracks,
// hits, and more.
// ----------------------------------------------------------------------------

#define TRACKJETQAMAKER_CC

// module defintion
#include "TrackJetQAMaker.h"



// ctor/dtor ------------------------------------------------------------------

TrackJetQAMaker::TrackJetQAMaker(const std::string& name, const std::string& outFileName) : SubsysReco(name) {

  // print debug message
  if (m_config.doDebug && (m_config.verbose > 4)) {
    std::cout << "TrackJetQAMaker::TrackJetQAMaker(const std::string& name, const std::string& outFileName) Calling ctor" << std::endl;
  }

  // initialize output file
  m_outFile = new TFile(outFileName.data(), "RECREATE");
  if (!m_outFile) {
    std::cerr << PHWHERE << ": PANIC: couldn't create output file!" << std::endl;
    assert(m_outFile); 
  }

}  // end ctor(std::string, st::string)



TrackJetQAMaker::~TrackJetQAMaker() {

  // print debug messages
  if (m_config.doDebug && (m_config.verbose > 4)) {
    std::cout << "TrackJetQAMaker::~TrackJetQAMaker() Calling dtor" << std::endl;
  }

  // clean up any dangling pointers
  if (m_outFile)    delete m_outFile;
  if (m_hitMaker)   delete m_hitMaker;
  if (m_clustMaker) delete m_clustMaker;
  if (m_trackMaker) delete m_trackMaker;
  if (m_jetMaker)   delete m_jetMaker;

}  // end dtor



// public methods -------------------------------------------------------------

void TrackJetQAMaker::Configure(TrackJetQAMakerConfig config, std::optional<TrackJetQAMakerHistDef> hist) {

  // print debug messages
  if (m_config.doDebug && (m_config.verbose > 3)) {
    std::cout << "TrackJetQAMaker::~TrackJetQAMaker() Calling dtor" << std::endl;
  }

  m_config = config;
  if (hist.has_value()) {
    m_hist = hist.value();
  }
  return;

}  // end 'Configure(TrackJetQAMakerConfig, std::optional<TrackJetQAMakerHistDef>)'



// fun4all methods ------------------------------------------------------------

int TrackJetQAMaker::Init(PHCompositeNode* topNode) {

  // print debug message
  if (m_config.doDebug && (m_config.verbose > 0)) {
    std::cout << "TrackJetQAMaker::Init(PHCompositeNode* topNode) Initializing" << std::endl;
  }

  // instantiate needed submodules
  if (m_config.doHitQA)   m_hitMaker   = new HitQAMaker();
  if (m_config.doClustQA) m_clustMaker = new ClustQAMaker();
  if (m_config.doTrackQA) m_trackMaker = new TrackQAMaker();
  if (m_config.doJetQA)   m_jetMaker   = new JetQAMaker();

  // intialize submodules
  if (m_config.doHitQA)   m_hitMaker   -> Init(m_hist, m_help);
  if (m_config.doClustQA) m_clustMaker -> Init(m_hist, m_help);
  if (m_config.doTrackQA) m_trackMaker -> Init(m_hist, m_help);
  if (m_config.doJetQA)   m_jetMaker   -> Init(m_hist, m_help);
  return Fun4AllReturnCodes::EVENT_OK;

}  // end 'Init(PHCompositeNode*)'



int TrackJetQAMaker::process_event(PHCompositeNode* topNode) {

  // print debug message
  if (m_config.doDebug && (m_config.verbose > 2)) {
    std::cout << "TrackJetQAMaker::process_event(PHCompositeNode* topNode) Processing Event" << std::endl;
  }

  // run submodules
  if (m_config.doHitQA)   m_hitMaker   -> Process(topNode);
  if (m_config.doClustQA) m_clustMaker -> Process(topNode);
  if (m_config.doTrackQA) m_trackMaker -> Process(topNode);
  if (m_config.doJetQA)   m_jetMaker   -> Process(topNode);
  return Fun4AllReturnCodes::EVENT_OK;

}  // end 'process_event(PHCompositeNode*)'



int TrackJetQAMaker::End(PHCompositeNode* topNode) {

  // print debug message
  if (m_config.doDebug && (m_config.verbose > 0)) {
    std::cout << "TrackJetQAMaker::End(PHCompositeNode* topNode) This is the End..." << std::endl;
  }

  // terminate submodules
  if (m_config.doHitQA)   m_hitMaker   -> End(m_outFile, m_config.hitOutDir);
  if (m_config.doClustQA) m_clustMaker -> End(m_outFile, m_config.clustOutDir);
  if (m_config.doTrackQA) m_trackMaker -> End(m_outFile, m_config.trackOutDir);
  if (m_config.doJetQA)   m_jetMaker   -> End(m_outFile, m_config.jetOutDir, m_config.cstOutDir);

  // close file
  m_outFile -> cd();
  m_outFile -> Close();
  return Fun4AllReturnCodes::EVENT_OK;

}  // end 'End(PHCompositeNode*)'

// end ------------------------------------------------------------------------
