// 'SEnergyCorrelator.h'
// Derek Anderson
// 01.20.2023
//
// A module to implement Peter Komiske's
// EEC library in the sPHENIX software
// stack.

#ifndef SENERGYCORRELATOR_H
#define SENERGYCORRELATOR_H

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wpragmas"

// standard c includes
#include <string>
#include <vector>
#include <cassert>
#include <sstream>
// f4a includes
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/Fun4AllHistoManager.h>
// phool includes
#include <phool/phool.h>
#include <phool/getClass.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/PHCompositeNode.h>
// root includes
#include <TH1.h>
#include <TFile.h>
#include <TTree.h>
#include <TMath.h>
#include <TNtuple.h>
#include <TDirectory.h>
// fastjet includes
#include <fastjet/PseudoJet.hh>
// eec include
#include "/sphenix/user/danderson/eec/EnergyEnergyCorrelators/eec/include/EEC.hh"

#pragma GCC diagnostic pop

using namespace std;

// forward declarations
class TH1;
class TFile;
class TTree;
class PHCompositeNode;



// SEnergyCorrelator definition -----------------------------------------------

class SEnergyCorrelator : public SubsysReco {

  public:

    // ctor/dtor
    SEnergyCorrelator(const string &name = "SEnergyCorrelator", const bool isComplex = false, const bool doDebug = false);
    ~SEnergyCorrelator() override;

    // F4A methods
    int Init(PHCompositeNode *topNode)          override;
    int process_event(PHCompositeNode *topNode) override;
    int End(PHCompositeNode *topNode)           override;

    // standalone-only methods
    void Init();
    void Analyze();
    void End();

    // setters
    void SetVerbosity(const int verb)           {m_verbosity   = verb;}
    void SetInputFile(const string &iFileName)  {m_inFileName  = iFileName;}
    void SetInputNode(const string &iNodeName)  {m_inNodeName  = iNodeName;}
    void SetInputTree(const string &iTreeName)  {m_inTreeName  = iTreeName;}
    void SetOutputFile(const string &oFileName) {m_outFileName = oFileName;}

    // system getters
    int    GetVerbosity()        {return m_verbosity;}
    bool   GetInDebugMode()      {return m_inDebugMode;}
    bool   GetInComplexMode()    {return m_inComplexMode;}
    bool   GetInStandaloneMode() {return m_inStandaloneMode;}
    string GetInputFileName()    {return m_inFileName;}
    string GetInputNodeName()    {return m_inNodeName;}
    string GetInputTreeName()    {return m_inTreeName;}
    string GetOutputFileName()   {return m_outFileName;}

  private:

    // io methods
    void OpenInputFile();
    void GrabInputNode();
    void SaveOutput();

    // system methods
    void InitializeMembers();
    void InitializeHists();
    void InitializeCorrs();
    void InitializeTree();
    void PrintMessage(const uint32_t code);
    void PrintError(const uint32_t code);

    // io members
    TFile *m_outFile;
    TFile *m_inFile;
    TTree *m_inTree;

    // system members
    int    m_verbosity;
    bool   m_inDebugMode;
    bool   m_inComplexMode;
    bool   m_inStandaloneMode;
    string m_inFileName;
    string m_inNodeName;
    string m_inTreeName;
    string m_outFileName;

    // histogram members
    size_t         m_nJetPtBins;
    size_t         m_nOutHistBins;
    vector<double> m_histBinEdges;
    vector<double> m_outputHists;

};

#endif

// end ------------------------------------------------------------------------
