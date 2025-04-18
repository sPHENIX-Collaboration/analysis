// ----------------------------------------------------------------------------
// 'TwoPointCorrelatorQA.h'
// Derek Anderson
// 03.25.2024
//
// A Fun4All QA module to produce an EEC spectra
// for jets constructed from an arbitrary source.
// ----------------------------------------------------------------------------

#ifndef TWOPOINTCORRELATORQA_H
#define TWOPOINTCORRELATORQA_H

// c++ utilities
#include <string>
#include <utility>
#include <vector>
// f4a libraries
#include <fun4all/SubsysReco.h>
// module configuration
#include "TwoPointCorrelatorQAConfig.h"

// forward declarations
class Fun4AllHistoManager;
class JetContainer;
class PHCompositeNode;
class TH1;



// TwoPointCorrelatorQA definition --------------------------------------------

class TwoPointCorrelatorQA : public SubsysReco {

  public:

    // ctor
    TwoPointCorrelatorQA(const std::string &name = "TwoPointCorrelatorQA");
    ~TwoPointCorrelatorQA() override;

    // inline public methods
    void Configure(TwoPointCorrelatorQAConfig config) {m_config = config;}

    // f4a methods
    int Init(PHCompositeNode *topNode)          override;
    int process_event(PHCompositeNode *topNode) override;
    int End(PHCompositeNode *topNode)           override;

  private:

    // private methods
    void BuildHistograms();
    void GrabNodes(PHCompositeNode* topNode);

    // f4a memebers
    std::shared_ptr<Fun4AllHistManager> m_manager = NULL;
    std::shared_ptr<JetContainer>       m_jetMap  = NULL;

    // output histograms
    std::vector<std::unique_ptr<TH1D>> m_vecHist;

    // module configuration
    TwoPointCorrelatorQAConfig m_config;

};

#endif

// end ------------------------------------------------------------------------
