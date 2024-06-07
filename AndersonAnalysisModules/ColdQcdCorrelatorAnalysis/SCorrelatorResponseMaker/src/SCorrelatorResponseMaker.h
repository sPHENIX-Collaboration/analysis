/// ---------------------------------------------------------------------------
/*! \file   SCorrelatorResponseMaker.h
 *  \author Derek Anderson
 *  \date   04.30.2023
 *
 * A module to match truth to reconstructed
 * jets/particles to derive corrections for
 * an n-point energy correlation strength
 * function.
 */
/// ---------------------------------------------------------------------------

#ifndef SCORRELATORRESPONSEMAKER_H
#define SCORRELATORRESPONSEMAKER_H

// c++ utilities
#include <cmath>
#include <string>
#include <vector>
#include <cassert>
#include <sstream>
#include <cstdlib>
#include <utility>
// root libraries
#include <TH1.h>
#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TMath.h>
#include <TString.h>
#include <TDirectory.h>
#include <Math/Vector3D.h>
// f4a libraries
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/Fun4AllHistoManager.h>
// phool utilities
#include <phool/phool.h>
#include <phool/getClass.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/PHCompositeNode.h>
// analysis utilities
#include <scorrelatorutilities/Tools.h>
#include <scorrelatorutilities/Types.h>
#include <scorrelatorutilities/Constants.h>
#include <scorrelatorutilities/Interfaces.h>
// analysis definitions
#include "SCorrelatorResponseMakerConfig.h"
#include "SCorrelatorResponseMakerInput.h"
#include "SCorrelatorResponseMakerOutput.h"

// make common namespaces
using namespace std;



namespace SColdQcdCorrelatorAnalysis {

  // --------------------------------------------------------------------------
  // Response tree maker
  // --------------------------------------------------------------------------
  /*! A module to match truth-to-reco level jets/constituents
   *  to derive corrections for ENC spectra from.  Produces a
   *  "Response Tree", a tree mapping all truth jets and their
   *  constituents onto reconstructed jets/constituents.
   */
  class SCorrelatorResponseMaker : public SubsysReco {

    public:

      // ctor/dtor
      SCorrelatorResponseMaker(const string &name = "SCorrelatorResponseMaker", const bool debug = false, const bool standalone = true);
      SCorrelatorResponseMaker(SCorrelatorResponseMakerConfig& config);
      ~SCorrelatorResponseMaker() override;

      // F4A methods
      int Init(PHCompositeNode*)          override;
      int process_event(PHCompositeNode*) override;
      int End(PHCompositeNode*)           override;

      // standalone-only methods
      void Init();
      void Analyze();
      void End();

      // setters
      void SetConfig(const SCorrelatorResponseMakerConfig& config) {m_config = config;}

      // getters
      SCorrelatorResponseMakerConfig GetConfig() {return m_config;}

    private:

      // io methods (*.io.h)
      void GrabInputNodes();
      void OpenInputFiles();
      void OpenOutputFile();
      void OpenFile(const string& fileName, TFile*& file);
      void SaveOutput();

      // analysis methods (*.ana.h)
      void DoMatching();

      // system methods (*.sys.h)
      void InitializeTrees();
      void PrintMessage(const uint32_t code, const uint64_t iEvt = 0, const pair<uint64_t, uint64_t> nEvts = {0, 0});
      void PrintDebug(const uint32_t code);
      void PrintError(const uint32_t code, const uint64_t iEvt = 0);

      // io members
      TFile* m_outFile    = NULL;
      TFile* m_inTrueFile = NULL;
      TFile* m_inRecoFile = NULL;
      TTree* m_inTrueTree = NULL;
      TTree* m_inRecoTree = NULL;
      TTree* m_matchTree  = NULL;

      // system members
      int m_fTrueCurrent = 0;
      int m_fRecoCurrent = 0;

      // configuration
      SCorrelatorResponseMakerConfig m_config; 

      // inputs
      SCorrelatorResponseMakerRecoInput        m_recoInput;
      SCorrelatorResponseMakerTruthInput       m_trueInput;
      SCorrelatorResponseMakerLegacyRecoInput  m_recoLegacy;
      SCorrelatorResponseMakerLegacyTruthInput m_trueLegacy;

      // outputs
      SCorrelatorResponseMakerOutput m_output;
      SCorrelatorResponseMakerOutput m_outLegacy;

  };  // end SCorrelatorResponseMaker

}  // end SColdQcdCorrelatorAnalysis namespace

#endif

// end ------------------------------------------------------------------------
