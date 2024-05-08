// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME MbdLaser_Dict
#define R__NO_DEPRECATION

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "RConfig.h"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// Header files passed as explicit arguments
#include "../MbdLaser.h"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace {
  void TriggerDictionaryInitialization_MbdLaser_Dict_Impl() {
    static const char* headers[] = {
"0",
nullptr
    };
    static const char* includePaths[] = {
nullptr
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "MbdLaser_Dict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "MbdLaser_Dict dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
#ifndef __MBDLASER_H__
#define __MBDLASER_H__

#include <fun4all/SubsysReco.h>
#include <string>
#include <map>
#include <TFile.h>

//Forward declerations
class PHCompositeNode;
class EventHeader;
class MbdOut;
class MbdPmtContainer;
class TFile;
class TTree;
class TDatabasePDG;
class TRandom3;
class TH1;
class TH2;
class TGraphErrors;
class TF1;
class TCanvas;


//Brief: basic TTree and histogram creation for sim evaluation
class MbdLaser: public SubsysReco
{
public: 

  //Default constructor
  MbdLaser(const std::string &name="MbdLaser");

  //Initialization, called for initialization
  int Init(PHCompositeNode *);

  //Initialization at start of every run
  int InitRun(PHCompositeNode *);

  //Process Event, called for each event
  int process_event(PHCompositeNode *);

  //End, write and close files
  int End(PHCompositeNode *);

  //Change output filename
  void set_savefile(const char *f) { _savefname = f; }

  void set_run(const int r) { f_run = r; }

private:

  //
  void LaserDST(PHCompositeNode *topNode);

  int  Getpeaktime(TH1 *h);

  //output filename
  std::string _savefname;
  TFile* _savefile;
   
  int nprocessed{0};     // num events processed

  //Output
  TTree* _tree;
  Int_t    f_run;
  Int_t    f_evt;
  Int_t    f_ch;
  Float_t  f_qmean;
  Float_t  f_qmerr;

  Float_t  f_mbdt[2]; // time in arm
  Float_t  f_mbdte[2]; // earliest hit time in arm
  Float_t  f_mbdt0;   // start time

  TH1 *h_mbdq[128];   // q in each tube
  TGraphErrors *g_mbdq[128];   // q in each tube
  
  
  TCanvas *c_mbdt;    // Canvas to 
  TH1 *hevt_mbdt[2];  // time in each mbd, per event
  TF1 *gaussian;

  std::map<int,int> _pids;  // PIDs of tracks in the MBD

  //
  TRandom3*     _rndm;
  Float_t       _tres;    // time resolution of one channel

  //Get all the nodes
  void GetNodes(PHCompositeNode *);
  
  //Node pointers
  EventHeader* _evtheader;
  MbdOut* _mbdout;
  MbdPmtContainer* _mbdpmts;

};

#endif //* __MBDLASER_H__ *//


#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("MbdLaser_Dict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_MbdLaser_Dict_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_MbdLaser_Dict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_MbdLaser_Dict() {
  TriggerDictionaryInitialization_MbdLaser_Dict_Impl();
}
