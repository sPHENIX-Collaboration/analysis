/// ---------------------------------------------------------------------------
/*! \file    SEnergyCorrelator.h
 *  \authors Derek Anderson, Alex Clarke
 *  \date    01.20.2023
 *
 *  A module to run ENC calculations in the sPHENIX
 *  software stack for the Cold QCD EEC analysis.
 */
/// ---------------------------------------------------------------------------

#ifndef SENERGYCORRELATOR_H
#define SENERGYCORRELATOR_H

// c++ utilities
#include <map>
#include <set>
#include <limits>
#include <string>
#include <vector>
#include <cassert>
#include <utility>
#include <iostream>
#include <optional>
#include <algorithm>
// root libraries
#include <TF1.h>
#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
#include <TChain.h>
#include <TDatime.h>
#include <TString.h>
#include <TRandom2.h>
#include <TVector3.h>
#include <TDirectory.h>
#include <Math/Vector4D.h>
// phool utilities
#include <phool/phool.h>
#include <phool/getClass.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/PHCompositeNode.h>
// f4a utilities
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllReturnCodes.h>
// fastjet libraries
#include <fastjet/PseudoJet.hh>
// eec library
#include <eec/include/EECLongestSide.hh>
// analysis utilities
#include <scorrelatorutilities/Tools.h>
#include <scorrelatorutilities/Types.h>
#include <scorrelatorutilities/Constants.h>
#include <scorrelatorutilities/Interfaces.h>
// analysis definitions
#include "SEnergyCorrelatorInput.h"
#include "SEnergyCorrelatorConfig.h"

// make common namespaces implicit
using namespace std;
using namespace ROOT::Math;



namespace SColdQcdCorrelatorAnalysis {

  // --------------------------------------------------------------------------
  //! N-point energy correlator calculator
  // --------------------------------------------------------------------------
  /*! A module to run various N-point energy correlator (ENC) calculations.
   *  Can run calculations either manually or via P. T. Komiske's EnergyEnergy-
   *  Correlator package. Also can run either local (in-jet) or global (jet-jet
   *  or particle-particle) calculations.
   */
  class SEnergyCorrelator : public SubsysReco {

    public:

      // options for ENC weight normalization
      enum Norm {Pt, Et, E};

      // ctor/dtor
      SEnergyCorrelator(SEnergyCorrelatorConfig& config);
      ~SEnergyCorrelator() override;

      // public methods
      void Init();
      void Analyze();
      void End();

      // setters
      void SetConfig(const SEnergyCorrelatorConfig& config) {m_config = config;}

      // getters
      SEnergyCorrelatorConfig GetConfig() {return m_config;}

    private:

      // io methods (*.io.h)
      void OpenInputFiles();
      void OpenOutputFile();
      void SaveOutput();
      void CloseOutputFile();

      // system methods (*.sys.h)
      void InitializeMembers();
      void InitializeHists();
      void InitializeCorrs();
      void InitializeTree();
      void InitializeSmearing();
      void PrintMessage(const uint32_t code, const uint64_t nEvts = 0, const uint64_t event = 0);
      void PrintDebug(const uint32_t code);
      void PrintError(const uint32_t code, const size_t nDrBinEdges = 0, const size_t iDrBin = 0, const string sInFileName = "");

      // analysis methods (*.ana.h)
      void    RunCalculations();
      void    DoGlobalCalcManual(const double normGlobal);  // TODO
      void    DoLocalCalcWithPackage(const double ptJet);
      void    DoLocalCalcManual(const PtEtaPhiEVector& normalization);
      void    ExtractHistsFromCorr();
      void    SmearJetMomentum(PtEtaPhiEVector& pJet);
      void    SmearCstMomentum(PtEtaPhiEVector& pCst);
      bool    IsGoodJet(const Types::JetInfo& jet);
      bool    IsGoodCst(const Types::CstInfo& cst);
      bool    SurvivesEfficiency(const double value);
      double  GetWeight(const PtEtaPhiEVector& momentum, const int option, optional<PtEtaPhiEVector> reference = nullopt);
      double  GetRM(const tuple<double, double, double>& dists);
      double  GetET(const TVector3& pMom, const TVector3& pRef);
      int32_t GetJetPtBin(const double ptJet);

      // configuration
      SEnergyCorrelatorConfig m_config;

      // io members
      TFile*  m_outFile = NULL;
      TFile*  m_inFile  = NULL;
      TChain* m_inChain = NULL;

      // smearing members
      TRandom2* m_rando  = NULL;

      // system members
      int m_fCurrent = 0;

      // for correlator calculations
      vector<fastjet::PseudoJet> m_jetCalcVec;
      vector<fastjet::PseudoJet> m_cstCalcVec;

      // local package output histograms
      //   - FIXME move these to a dedicated histogram manager
      vector<TH1D*> m_outPackageHistVarDrAxis;
      vector<TH1D*> m_outPackageHistErrDrAxis;
      vector<TH1D*> m_outPackageHistVarLnDrAxis;
      vector<TH1D*> m_outPackageHistErrLnDrAxis;

      // local manual output histograms
      //   - FIXME move these to a dedicated histogram manager
      vector<TH1D*>         m_outManualHistErrDrAxis;
      vector<TH1D*>         m_outProjE3C;
      vector<vector<TH2D*>> m_outE3C;

      // global output histograms
      //   - FIXME move these to a dedicated histogram manager
      //   - FIXME also add pi-phi versions
      vector<TH1D*> m_outGlobalHistDPhiAxis;
      vector<TH1D*> m_outGlobalHistCosDFAxis;

      // correlators
      vector<fastjet::contrib::eec::EECLongestSide<fastjet::contrib::eec::hist::axis::log>*> m_eecLongSide;

      // inputs
      SEnergyCorrelatorInput    m_input;
      SCorrelatorInputInterface m_interface;

  };  // end SEnergyCorrelator

}  // end SColdQcdCorrelatorAnalysis namespace

#endif

// end ------------------------------------------------------------------------
