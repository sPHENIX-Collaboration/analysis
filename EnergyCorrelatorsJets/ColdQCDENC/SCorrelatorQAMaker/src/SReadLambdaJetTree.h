// ----------------------------------------------------------------------------
// 'ReadLambdaJetTree.h'
// Derek Anderson
// 02.19.2024
//
// SCorrelatorQAMaker plugin to read lambda-tagged jet
// trees and draw plots and calculate the total no.
// of lambdas.
// ----------------------------------------------------------------------------

#ifndef SCORRELATORQAMAKER_SREADLAMBDAJETTREE_H
#define SCORRELATORQAMAKER_SREADLAMBDAJETTREE_H

// c++ utilities
#include <cmath>
#include <string>
#include <vector>
#include <cassert>
#include <utility>
#include <iostream>
// root libraries
#include <TH1.h>
#include <TH2.h>
#include <TMath.h>
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
// plugin definitions
#include "SBaseQAPlugin.h"
#include "SReadLambdaJetTreeConfig.h"
#include "SReadLambdaJetTreeHistDef.h"

// make common namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {

  // SReadLambdaJetTree definition --------------------------------------------

  class SReadLambdaJetTree : public SBaseQAPlugin<SReadLambdaJetTreeConfig> {

    public:

      // histogram accessors
      enum Evt {
        NJet, 
        NTagJet,
        NLeadJet,
        NMultiJet,
        NLam,
        NLamJet,
        NLeadLam
      };
      enum Type {
        Lam,
        LLam,
        MLJetLam,
        Jet,
        LJet,
        LLJet,
        MLJet,
        HJet
      };
      enum Var {
        Eta,
        Ene,
        Pt,
        DPhi,
        DEta,
        Dr,
        Z,
        NL,
        NC,
        RLC
      };
      enum Mod {
        VsEta,
        VsEne,
        VsPt,
        VsDPhi,
        VsDEta
      };

      // ctor/dtor
      SReadLambdaJetTree()  {};
      ~SReadLambdaJetTree() {};

      // public methods
      void Init();
      void Analyze();
      void End();

      // plugin-specific setters
      void SetHistDef(SReadLambdaJetTreeHistDef& def) {m_hist = def;}

    private:

      // internal methods
      void   InitInput();
      void   InitTree();
      void   InitHists();
      void   DoAnalysis();
      void   SetHistogramStyles();
      void   SaveOutput();
      void   CloseInput();
      void   FillHist1D(const int type, Hist hist);
      void   FillHist2D(const int type, Hist hist, VsVar vs);
      bool   IsGoodJet(const double pt, const double eta);
      bool   IsGoodLambda(const double pt, const double eta);
      bool   IsLeadingLambda(const double z);
      bool   IsAssociatedLambda(const int idLam, const int idJet);
      double GetDeltaPhi(const double phiA, const double phiB);
      double GetDeltaEta(const double etaA, const double etaB);

      // inputs
      TFile* m_fInput = NULL;
      TTree* m_tInput = NULL;

      // histograms
      vector<TH1D*>                 vecHistEvt;
      vector<vector<TH1D*>>         vecHist1D;
      vector<vector<vector<TH2D*>>> vecHist2D;

      // histogram definitions
      SReadLambdaJetTreeHistDef m_hist;

      // event-level input leaves
      int    m_evtNJets;
      int    m_evtNLambdas;
      int    m_evtNTaggedJets;
      int    m_evtNChrgPars;
      int    m_evtNNeuPars;
      double m_evtSumEPar;
      double m_evtVtxX;
      double m_evtVtxY;
      double m_evtVtxZ;
      // parton input leaves
      int    m_partonA_ID;
      int    m_partonB_ID;
      double m_partonA_Px;
      double m_partonA_Py;
      double m_partonA_Pz;
      double m_partonA_E;
      double m_partonB_Px;
      double m_partonB_Py;
      double m_partonB_Pz;
      double m_partonB_E;
      // lambda input leaves
      vector<int>*    m_lambdaID      = NULL;
      vector<int>*    m_lambdaPID     = NULL;
      vector<int>*    m_lambdaJetID   = NULL;
      vector<int>*    m_lambdaEmbedID = NULL;
      vector<double>* m_lambdaZ       = NULL;
      vector<double>* m_lambdaDr      = NULL;
      vector<double>* m_lambdaEnergy  = NULL;
      vector<double>* m_lambdaPt      = NULL;
      vector<double>* m_lambdaEta     = NULL;
      vector<double>* m_lambdaPhi     = NULL;
      // jet input leaves
      vector<bool>*     m_jetHasLambda = NULL;
      vector<uint64_t>* m_jetNCst      = NULL;
      vector<uint64_t>* m_jetID        = NULL;
      vector<double>*   m_jetE         = NULL;
      vector<double>*   m_jetPt        = NULL;
      vector<double>*   m_jetEta       = NULL;
      vector<double>*   m_jetPhi       = NULL;
      // constituent input leaves
      vector<vector<int>>*    m_cstID      = NULL;
      vector<vector<int>>*    m_cstPID     = NULL;
      vector<vector<int>>*    m_cstJetID   = NULL;
      vector<vector<int>>*    m_cstEmbedID = NULL;
      vector<vector<double>>* m_cstZ       = NULL;
      vector<vector<double>>* m_cstDr      = NULL;
      vector<vector<double>>* m_cstEnergy  = NULL;
      vector<vector<double>>* m_cstPt      = NULL;
      vector<vector<double>>* m_cstEta     = NULL;
      vector<vector<double>>* m_cstPhi     = NULL;

      // intput branches
      TBranch* m_brEvtNJets       = NULL;
      TBranch* m_brEvtNLambdas    = NULL;
      TBranch* m_brEvtNTaggedJets = NULL;
      TBranch* m_brEvtNChrgPars   = NULL;
      TBranch* m_brEvtNNeuPars    = NULL;
      TBranch* m_brEvtSumEPar     = NULL;
      TBranch* m_brEvtVtxX        = NULL;
      TBranch* m_brEvtVtxY        = NULL;
      TBranch* m_brEvtVtxZ        = NULL;
      TBranch* m_brPartonA_ID     = NULL;
      TBranch* m_brPartonB_ID     = NULL;
      TBranch* m_brPartonA_Px     = NULL;
      TBranch* m_brPartonA_Py     = NULL;
      TBranch* m_brPartonA_Pz     = NULL;
      TBranch* m_brPartonA_E      = NULL;
      TBranch* m_brPartonB_Px     = NULL;
      TBranch* m_brPartonB_Py     = NULL;
      TBranch* m_brPartonB_Pz     = NULL;
      TBranch* m_brPartonB_E      = NULL;
      TBranch* m_brLambdaID       = NULL;
      TBranch* m_brLambdaPID      = NULL;
      TBranch* m_brLambdaJetID    = NULL;
      TBranch* m_brLambdaEmbedID  = NULL;
      TBranch* m_brLambdaZ        = NULL;
      TBranch* m_brLambdaDr       = NULL;
      TBranch* m_brLambdaEnergy   = NULL;
      TBranch* m_brLambdaPt       = NULL;
      TBranch* m_brLambdaEta      = NULL;
      TBranch* m_brLambdaPhi      = NULL;
      TBranch* m_brJetHasLambda   = NULL;
      TBranch* m_brJetNCst        = NULL;
      TBranch* m_brJetID          = NULL;
      TBranch* m_brJetE           = NULL;
      TBranch* m_brJetPt          = NULL;
      TBranch* m_brJetEta         = NULL;
      TBranch* m_brJetPhi         = NULL;
      TBranch* m_brCstID          = NULL;
      TBranch* m_brCstPID         = NULL;
      TBranch* m_brCstJetID       = NULL;
      TBranch* m_brCstEmbedID     = NULL;
      TBranch* m_brCstZ           = NULL;
      TBranch* m_brCstDr          = NULL;
      TBranch* m_brCstEnergy      = NULL;
      TBranch* m_brCstPt          = NULL;
      TBranch* m_brCstEta         = NULL;
      TBranch* m_brCstPhi         = NULL;

      // class-wide constants
      struct Const {
        double minDPhi;
        double maxDPhi;
      } m_const = {
        -1.*TMath::Pi(),
        TMath::Pi()
      }; 

  };  // end SReadLambdaJetTree

}  // end SColdQcdCorrelatorAnalysis namespace

#endif

// end ------------------------------------------------------------------------
