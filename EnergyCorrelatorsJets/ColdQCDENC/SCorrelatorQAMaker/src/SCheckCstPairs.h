// ----------------------------------------------------------------------------
// 'SCheckCstPairs.h'
// Derek Anderson
// 01.19.2024
//
// SCorrelatorQAMaker plugin to iterate through all pairs of constituents in
// an event and fill tuples/histograms comparing them.
//
// This is similar to the `SCheckTrackPairs` plugin, which specifically looks
// at pairs of tracks off the node tree. This plugin compares constituents
// of any type off the correlator jet tree.
// ----------------------------------------------------------------------------

#ifndef SCORRELATORQAMAKER_SCHECKCSTPAIRS_H
#define SCORRELATORQAMAKER_SCHECKCSTPAIRS_H

// c++ utilities
#include <string>
#include <vector>
#include <utility>
// root libraries
#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
#include <TChain.h>
#include <TBranch.h>
// analysis utilities
#include <scorrelatorutilities/Types.h>
#include <scorrelatorutilities/Constants.h>
#include <scorrelatorutilities/Interfaces.h>
// plugin definitions
#include "SBaseQAPlugin.h"
#include "SCheckCstPairsConfig.h"

// make common namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {

  // SCheckCstPairs definition ------------------------------------------------

  class SCheckCstPairs : public SBaseQAPlugin<SCheckCstPairsConfig> {

    public:

      // ctor/dtor
      SCheckCstPairs() {};
      ~SCheckCstPairs() {};

      // analysis methods
      void Init();
      void Analyze();
      void End();

    private:

      // internal methods
      void InitInput();
      void InitHists();
      void SaveOutput();
      void CloseInput();
      void DoDoubleCstLoop();
      bool IsGoodJet();
      bool IsGoodCst();

      // io members
      TFile*  m_fInput = NULL;
      TChain* m_cInput = NULL;

      // output histograms
      TH2D* hCstPtOneVsDr;
      TH2D* hCstPtTwoVsDr;
      TH2D* hCstPtFracVsDr;
      TH2D* hCstPhiOneVsDr;
      TH2D* hCstPhiTwoVsDr;
      TH2D* hCstEtaOneVsDr;
      TH2D* hCstEtaTwoVsDr;
      TH2D* hDeltaPhiOneVsDr;
      TH2D* hDeltaPhiTwoVsDr;
      TH2D* hDeltaEtaOneVsDr;
      TH2D* hDeltaEtaTwoVsDr;
      TH2D* hJetPtFracOneVsDr;
      TH2D* hJetPtFracTwoVsDr;
      TH2D* hCstPairWeightVsDr;

      // input truth tree addresses
      //   - FIXME swap out for utlity types when ready
      int                  m_evtNumChrgPars = -999;
      double               m_evtSumPar      = -999.;
      pair<int, int>       m_partonID       = {-999,  -999};
      pair<double, double> m_partonMomX     = {-999., -999.};
      pair<double, double> m_partonMomY     = {-999., -999.};
      pair<double, double> m_partonMomZ     = {-999., -999.};
      vector<vector<int>>* m_cstID          = NULL;
      vector<vector<int>>* m_cstEmbedID     = NULL;
      // input reco. tree addresses
      int                  m_evtNumTrks = -999;
      double               m_evtSumECal = -999.;
      double               m_evtSumHCal = -999.;
      vector<vector<int>>* m_cstMatchID = NULL;

      // generic input tree address members
      //   - FIXME swap out for utlity types when ready
      int                     m_evtNumJets = -999;
      double                  m_evtVtxX    = -999.;
      double                  m_evtVtxY    = -999.;
      double                  m_evtVtxZ    = -999.;
      vector<unsigned long>*  m_jetNumCst  = NULL;
      vector<unsigned int>*   m_jetID      = NULL;
      vector<unsigned int>*   m_jetTruthID = NULL;
      vector<double>*         m_jetEnergy  = NULL;
      vector<double>*         m_jetPt      = NULL;
      vector<double>*         m_jetEta     = NULL;
      vector<double>*         m_jetPhi     = NULL;
      vector<double>*         m_jetArea    = NULL;
      vector<vector<double>>* m_cstZ       = NULL;
      vector<vector<double>>* m_cstDr      = NULL;
      vector<vector<double>>* m_cstEnergy  = NULL;
      vector<vector<double>>* m_cstPt      = NULL;
      vector<vector<double>>* m_cstEta     = NULL;
      vector<vector<double>>* m_cstPhi     = NULL;

      // input truth tree branch members
      //   - FIXME swap out for utlity types when ready
      TBranch*                 m_brEvtSumPar  = NULL;
      TBranch*                 m_brCstID      = NULL;
      TBranch*                 m_brCstEmbedID = NULL;
      pair<TBranch*, TBranch*> m_brPartonID   = {NULL, NULL};
      pair<TBranch*, TBranch*> m_brPartonMomX = {NULL, NULL};
      pair<TBranch*, TBranch*> m_brPartonMomY = {NULL, NULL};
      pair<TBranch*, TBranch*> m_brPartonMomZ = {NULL, NULL};
      // input reco. tree branch members
      TBranch* m_brEvtNumTrks = NULL;
      TBranch* m_brEvtSumECal = NULL;
      TBranch* m_brEvtSumHCal = NULL;
      TBranch* m_brCstMatchID = NULL;

      // generic input tree branch members
      //   - FIXME swap out for utlity types when ready
      TBranch* m_brEvtNumJets = NULL;
      TBranch* m_brEvtVtxX    = NULL;
      TBranch* m_brEvtVtxY    = NULL;
      TBranch* m_brEvtVtxZ    = NULL;
      TBranch* m_brJetNumCst  = NULL;
      TBranch* m_brJetID      = NULL;
      TBranch* m_brJetEnergy  = NULL;
      TBranch* m_brJetPt      = NULL;
      TBranch* m_brJetEta     = NULL;
      TBranch* m_brJetPhi     = NULL;
      TBranch* m_brJetArea    = NULL;
      TBranch* m_brCstZ       = NULL;
      TBranch* m_brCstDr      = NULL;
      TBranch* m_brCstPt      = NULL;
      TBranch* m_brCstEnergy  = NULL;
      TBranch* m_brCstEta     = NULL;
      TBranch* m_brCstPhi     = NULL;

  };  // end SCheckCstPairs

}  // end SColdQcdCorrelatorAnalysis namespace

#endif

// end ------------------------------------------------------------------------
