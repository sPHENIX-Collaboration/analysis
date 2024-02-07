// ----------------------------------------------------------------------------
// 'STrackMatcherComparator.h'
// Derek Anderson
// 01.30.2024
//
// Small module to produce plots from the output of the
// SvtxEvaluator and FillMatchedClusTree modules.
// ----------------------------------------------------------------------------

#ifndef STRACKMATCHERCOMPARATOR_H
#define STRACKMATCHERCOMPARATOR_H

// c++ utilities
#include <string>
#include <optional>
// root libraries
#include <TFile.h>
#include <TTree.h>
#include <TNtuple.h>
#include <TDirectory.h>
// analysis utilities
#include "STrackMatcherComparatorConfig.h"
#include "STrackMatcherComparatorHistDef.h"

// make common namespaces implicit
using namespace std;

// forward declarations
class PHCompositeNode;



// STrackMatcherComparator definition -----------------------------------------

class STrackMatcherComparator {

  public:

    // ctor/dtor
    STrackMatcherComparator(optional<STrackMatcherComparatorConfig> config = nullopt);
    ~STrackMatcherComparator();

    // public methods
    void Init();
    void Analyze();
    void End();

     // setters
     void SetConfig(STrackMatcherComparatorConfig& config)  {m_config = config;}
     void SetHistDef(STrackMatcherComparatorHistDef& histo) {m_hist = histo;}

     // getters
     STrackMatcherComparatorConfig  GetConfig()  {return m_config;}
     STrackMatcherComparatorHistDef GetHistDef() {return m_hist;}

   private:

    // internal methods
    void OpenOutput();
    void OpenInput();
    void InitHists();
    void GetNewTreeHists();
    void GetNewTupleHists();
    void GetOldTupleHists();
    void MakeRatiosAndPlots(const vector<vector<TH1D*>> vecNewHists1D, const vector<vector<vector<TH2D*>>> vecNewHists2D, const int iDir, const string sLabel);
    void SaveHistograms();
    void CloseInput();
    void CloseOutput();

    // configuration & histogram info
    STrackMatcherComparatorConfig  m_config;
    STrackMatcherComparatorHistDef m_hist;

    // i/o files
    TFile* m_outFile         = NULL;
    TFile* m_treeInFileTrue  = NULL;
    TFile* m_treeInFileReco  = NULL;
    TFile* m_tupleInFileTrue = NULL;
    TFile* m_tupleInFileReco = NULL;
    TFile* m_oldInFileTrue   = NULL;
    TFile* m_oldInFileReco   = NULL;

    // input trees/tuples
    TTree*   m_tTreeTrue   = NULL;
    TTree*   m_tTreeReco   = NULL;
    TNtuple* m_ntTupleTrue = NULL;
    TNtuple* m_ntTupleReco = NULL;
    TNtuple* m_ntOldTrue   = NULL;
    TNtuple* m_ntOldReco   = NULL;

    // output directories
    vector<TDirectory*> m_vecHistDirs;
    vector<TDirectory*> m_vecRatioDirs;
    vector<TDirectory*> m_vecPlotDirs;

    // 1d histogram vectors
    vector<vector<TH1D*>> m_vecTreeHists1D;
    vector<vector<TH1D*>> m_vecTupleHists1D;
    vector<vector<TH1D*>> m_vecOldHists1D;

    // 2d histogram vectors
    vector<vector<vector<TH2D*>>> m_vecTreeHists2D;
    vector<vector<vector<TH2D*>>> m_vecTupleHists2D;
    vector<vector<vector<TH2D*>>> m_vecOldHists2D;

    // class-wide constants
    struct Consts {
      size_t nDirHist;
      size_t nDirRatio;
      size_t nDirPlot;
      size_t nVtx;
      size_t nSide;
      size_t nAxes;
    } m_const = {3, 2, 2, 4, 4, 3};

    // accessors
    enum Src {
      NewTree,
      NewTuple,
      OldTuple
    };
    enum Var {
      NTot,
      NIntt,
      NMvtx,
      NTpc,
      RTot,
      RIntt,
      RMvtx,
      RTpc,
      Phi,
      Eta,
      Pt,
      Frac,
      Qual,
      PtErr,
      EtaErr,
      PhiErr,
      PtRes,
      EtaRes,
      PhiRes
    };
    enum Type {
      Truth,
      Track,
      Weird,
      Normal
    };
    enum Comp {
      VsTruthPt,
      VsNumTpc
    };

};

#endif

// end ------------------------------------------------------------------------
