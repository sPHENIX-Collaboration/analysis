// 'SEnergyCorrelator.h'
// Derek Anderson
// 01.20.2023
//
// A module to implement Peter Komiske's
// EEC library in the sPHENIX software
// stack.

#ifndef SENERGYCORRELATOR_H
#define SENERGYCORRELATOR_H

// standard c includes
#include <string>
#include <vector>
#include <cassert>
#include <sstream>
#include <cstdlib>
#include <utility>
// root includes
#include <TH1.h>
#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TMath.h>
#include <TNtuple.h>
#include <TDirectory.h>
// fastjet includes
#include <fastjet/PseudoJet.hh>
// eec include
#include "/sphenix/user/danderson/eec/EnergyEnergyCorrelators/eec/include/EECLongestSide.hh"

using namespace std;
using namespace fastjet;

// global constants
static const size_t NRange     = 2;
static const size_t NMaxPtBins = 10;



// SEnergyCorrelator definition -----------------------------------------------

//class SEnergyCorrelator : public SubsysReco {
class SEnergyCorrelator {

  public:

    // ctor/dtor
    SEnergyCorrelator(const string &name = "SEnergyCorrelator", const bool isComplex = false, const bool doDebug = false, const bool inBatch = false);
    //~SEnergyCorrelator() override;
    ~SEnergyCorrelator();

    // F4A methods
    /* TODO F4A methods will go here */

    // standalone-only methods
    void Init();
    void Analyze();
    void End();

    // setters (inline)
    void SetVerbosity(const int verb)           {m_verbosity   = verb;}
    void SetInputNode(const string &iNodeName)  {m_inNodeName  = iNodeName;}
    void SetInputFile(const string &iFileName)  {m_inFileName  = iFileName;}
    void SetOutputFile(const string &oFileName) {m_outFileName = oFileName;}

    // setters (*.io.h)
    void SetInputTree(const string &iTreeName, const bool isTruthTree = false);
    void SetCorrelatorParameters(const uint32_t nPointCorr, const uint64_t nBinsDr, const double minDr, const double maxDr);
    void SetPtJetBins(const vector<pair<double, double>> &pTjetBins);

    // system getters
    int      GetVerbosity()        {return m_verbosity;}
    bool     GetInDebugMode()      {return m_inDebugMode;}
    bool     GetInComplexMode()    {return m_inComplexMode;}
    bool     GetInStandaloneMode() {return m_inStandaloneMode;}
    string   GetInputFileName()    {return m_inFileName;}
    string   GetInputNodeName()    {return m_inNodeName;}
    string   GetInputTreeName()    {return m_inTreeName;}
    string   GetOutputFileName()   {return m_outFileName;}

    // correlator getters
    double   GetMinDrBin()   {return m_drBinRange[0];}
    double   GetMaxDrBin()   {return m_drBinRange[1];}
    size_t   GetNBinsJetPt() {return m_nBinsJetPt;}
    uint32_t GetNPointCorr() {return m_nPointCorr;}
    uint64_t GetNBinsDr()    {return m_nBinsDr;}
    

  private:

    // io methods (*.io.h)
    void GrabInputNode();
    void OpenInputFile();
    void OpenOutputFile();
    void SaveOutput();

    // system methods (*.sys.h)
    void    InitializeMembers();
    void    InitializeHists();
    void    InitializeCorrs();
    void    InitializeTree();
    void    PrintMessage(const uint32_t code, const uint64_t nEvts = 0, const uint64_t event = 0);
    void    PrintDebug(const uint32_t code);
    void    PrintError(const uint32_t code);
    bool    CheckCriticalParameters();
    int64_t LoadTree(const uint64_t entry);
    int64_t GetEntry(const uint64_t entry);

    // io members
    TFile         *m_outFile;
    TFile         *m_inFile;
    TTree         *m_inTree;
    vector<TH1D*>  m_outHist;

    // system members
    int    m_fCurrent;
    int    m_verbosity;
    bool   m_inDebugMode;
    bool   m_inBatchMode;
    bool   m_inComplexMode;
    bool   m_inStandaloneMode;
    bool   m_isInputTreeTruth;
    string m_moduleName;
    string m_inFileName;
    string m_inNodeName;
    string m_inTreeName;
    string m_outFileName;

    // correlator parameters
    uint32_t                     m_nPointCorr;
    uint64_t                     m_nBinsDr;
    size_t                       m_nBinsJetPt;
    double                       m_drBinRange[NRange];
    vector<pair<double, double>> m_ptJetBins;

    // correlators
    vector<contrib::eec::EECLongestSide<contrib::eec::hist::axis::log>*> m_eecLongSide;

    // input truth tree address members
    int    m_truParton3_ID;
    int    m_truParton4_ID;
    double m_truParton3_MomX;
    double m_truParton3_MomY;
    double m_truParton3_MomZ;
    double m_truParton4_MomX;
    double m_truParton4_MomY;
    double m_truParton4_MomZ;
    // input reco. tree address members
    int    m_recParton3_ID;
    int    m_recParton4_ID;
    double m_recParton3_MomX;
    double m_recParton3_MomY;
    double m_recParton3_MomZ;
    double m_recParton4_MomX;
    double m_recParton4_MomY;
    double m_recParton4_MomZ;

    // generic input tree address members
    int                     m_evtNumJets;
    vector<unsigned long>  *m_jetNumCst;
    vector<unsigned int>   *m_jetID;
    vector<unsigned int>   *m_jetTruthID;
    vector<double>         *m_jetEnergy;
    vector<double>         *m_jetPt;
    vector<double>         *m_jetEta;
    vector<double>         *m_jetPhi;
    vector<double>         *m_jetArea;
    vector<vector<double>> *m_cstZ;
    vector<vector<double>> *m_cstDr;
    vector<vector<double>> *m_cstEnergy;
    vector<vector<double>> *m_cstJt;
    vector<vector<double>> *m_cstEta;
    vector<vector<double>> *m_cstPhi;

    // input truth tree branch members
    TBranch *m_brTruParton3_ID;
    TBranch *m_brTruParton4_ID;
    TBranch *m_brTruParton3_MomX;
    TBranch *m_brTruParton3_MomY;
    TBranch *m_brTruParton3_MomZ;
    TBranch *m_brTruParton4_MomX;
    TBranch *m_brTruParton4_MomY;
    TBranch *m_brTruParton4_MomZ;
    // input reco. tree branch members
    TBranch *m_brRecParton3_ID;
    TBranch *m_brRecParton4_ID;
    TBranch *m_brRecParton3_MomX;
    TBranch *m_brRecParton3_MomY;
    TBranch *m_brRecParton3_MomZ;
    TBranch *m_brRecParton4_MomX;
    TBranch *m_brRecParton4_MomY;
    TBranch *m_brRecParton4_MomZ;

    // generic input tree branch members
    TBranch *m_brEvtNumJets;
    TBranch *m_brJetNumCst;
    TBranch *m_brJetID;
    TBranch *m_brJetTruthID;
    TBranch *m_brJetEnergy;
    TBranch *m_brJetPt;
    TBranch *m_brJetEta;
    TBranch *m_brJetPhi;
    TBranch *m_brJetArea;
    TBranch *m_brCstZ;
    TBranch *m_brCstDr;
    TBranch *m_brCstEnergy;
    TBranch *m_brCstJt;
    TBranch *m_brCstEta;
    TBranch *m_brCstPhi;

};

#endif

// end ------------------------------------------------------------------------
