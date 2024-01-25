// ----------------------------------------------------------------------------
// 'SLambdaJetHunter.h'
// Derek Anderson
// 01.25.2024
//
// A minimal analysis module to find lambda-tagged
// jets in pythia events.
// ----------------------------------------------------------------------------

#ifndef SLAMBDAJETHUNTER_H
#define SLAMBDAJETHUNTER_H

// c++ utilities
#include <string>
#include <vector>
// root libraries
#include <TFile.h>
#include <TTree.h>
// fastjet libraries
#include <fastjet/PseudoJet.hh>
#include <fastjet/JetDefinition.hh>
#include <fastjet/ClusterSequence.hh>
#include <fastjet/FunctionOfPseudoJet.hh>
// f4a utilities
#include <fun4all/SubsysReco.h>
// analysis utilities
#include "SLambdaJetHunterConfig.h"
#include "/sphenix/user/danderson/eec/SCorrelatorUtilities/EvtTools.h"
#include "/sphenix/user/danderson/eec/SCorrelatorUtilities/JetTools.h"
#include "/sphenix/user/danderson/eec/SCorrelatorUtilities/CstTools.h"
#include "/sphenix/user/danderson/eec/SCorrelatorUtilities/GenTools.h"

// make common namespaces implicit
using namespace std;
using namespace fastjet;
using namespace SColdQcdCorrelatorAnalysis::SCorrelatorUtilities;

// forward declarations
class PHCompositeNode;



namespace SColdQcdCorrelatorAnalysis {

  // SLambdaJetHunter definition ----------------------------------------------

  class SLambdaJetHunter : public SubsysReco {

     public:

       // jet algorithm options
       enum Algo   {AntiKt, Kt, CA};
       enum Recomb {E, Pt, Pt2, Et, Et2};

       // ctor/dtor
       SLambdaJetHunter(const string &name = "SLambdaJetHunter", const bool debug = false);
       SLambdaJetHunter(SLambdaJetHunterConfig& config);
       ~SLambdaJetHunter() override;

       // f4a methods
      int Init(PHCompositeNode *topNode)          override;
      int process_event(PHCompositeNode *topNode) override;
      int End(PHCompositeNode *topNode)           override;

      // setters
      void SetConfig(SLambdaJetHunterConfig& config) {m_config = config;}

      // getters
      SLambdaJetHunterConfig GetConfig() {return m_config;}

    private:

      // i/o members
      TFile* m_outFile = NULL;
      TTree* m_outTree = NULL;

      // module configuration
      SLambdaJetHunterConfig m_config;

      // output variables
      GenInfo                 m_genEvtInfo;
      vector<JetInfo>         m_jetInfo;
      vector<vector<CstInfo>> m_cstInfo;
      vector<vector<ParInfo>> m_lambdaInfo;

      // analysis methods (*.ana.h)
      /* TODO will go here */

      // system methods (*.sys.h)
      /* TODO will go here */

  };  // end SLambdaJetHunter

}  // end SColdQcdCorrelatorAnalysis namespace

#endif

// end ------------------------------------------------------------------------
