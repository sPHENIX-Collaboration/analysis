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
// eec include
#include </sphenix/user/danderson/eec/EnergyEnergyCorrelators/eec/include/EEC.hh>

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
    SEnergyCorrelator(const std::string &name = "SEnergyCorrelator", const bool isComplex = false, const bool doDebug = false);
    ~SEnergyCorrelator() override;

    // F4A methods
    int Init(PHCompositeNode *topNode)          override;
    int process_event(PHCompositeNode *topNode) override;
    int End(PHCompositeNode *topNode)           override;

    // standalone-only methods
    int Analyze();

    // setters
    void SetVerbosity(const int verb) {m_verbosity = verb;}

    // system getters
    int  GetVerbosity()        {return m_verbosity;}
    bool GetInDebugMode()      {return m_inDebugMode;}
    bool GetInComplexMode()    {return m_inComplexMode;}
    bool GetInStandaloneMode() {return m_inStandaloneMode;}

  private:

    // io methods
    void OpenInputFile();
    void GrabInputNode();
    void SaveOutput();

    // system methods
    void InitializeMembers();
    void InitializeHists();
    void PrintMessage();
    void PrintError();

    // io members
    TFile *m_inFile;   
    TFile *m_outFile;

    // system members
    int  m_verbosity;
    bool m_inDebugMode;
    bool m_inComplexMode;
    bool m_inStandaloneMode;

};

#endif

// end ------------------------------------------------------------------------
