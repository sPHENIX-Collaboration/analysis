// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME MBDStudy_Dict
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
#include "../MBDStudy.h"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace ROOT {
   static TClass *MBDStudy_Dictionary();
   static void MBDStudy_TClassManip(TClass*);
   static void *new_MBDStudy(void *p = nullptr);
   static void *newArray_MBDStudy(Long_t size, void *p);
   static void delete_MBDStudy(void *p);
   static void deleteArray_MBDStudy(void *p);
   static void destruct_MBDStudy(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::MBDStudy*)
   {
      ::MBDStudy *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::MBDStudy));
      static ::ROOT::TGenericClassInfo 
         instance("MBDStudy", "", 28,
                  typeid(::MBDStudy), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &MBDStudy_Dictionary, isa_proxy, 4,
                  sizeof(::MBDStudy) );
      instance.SetNew(&new_MBDStudy);
      instance.SetNewArray(&newArray_MBDStudy);
      instance.SetDelete(&delete_MBDStudy);
      instance.SetDeleteArray(&deleteArray_MBDStudy);
      instance.SetDestructor(&destruct_MBDStudy);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::MBDStudy*)
   {
      return GenerateInitInstanceLocal((::MBDStudy*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::MBDStudy*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *MBDStudy_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::MBDStudy*)nullptr)->GetClass();
      MBDStudy_TClassManip(theClass);
   return theClass;
   }

   static void MBDStudy_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_MBDStudy(void *p) {
      return  p ? new(p) ::MBDStudy : new ::MBDStudy;
   }
   static void *newArray_MBDStudy(Long_t nElements, void *p) {
      return p ? new(p) ::MBDStudy[nElements] : new ::MBDStudy[nElements];
   }
   // Wrapper around operator delete
   static void delete_MBDStudy(void *p) {
      delete ((::MBDStudy*)p);
   }
   static void deleteArray_MBDStudy(void *p) {
      delete [] ((::MBDStudy*)p);
   }
   static void destruct_MBDStudy(void *p) {
      typedef ::MBDStudy current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::MBDStudy

namespace ROOT {
   static TClass *maplEintcOintgR_Dictionary();
   static void maplEintcOintgR_TClassManip(TClass*);
   static void *new_maplEintcOintgR(void *p = nullptr);
   static void *newArray_maplEintcOintgR(Long_t size, void *p);
   static void delete_maplEintcOintgR(void *p);
   static void deleteArray_maplEintcOintgR(void *p);
   static void destruct_maplEintcOintgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const map<int,int>*)
   {
      map<int,int> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(map<int,int>));
      static ::ROOT::TGenericClassInfo 
         instance("map<int,int>", -2, "map", 100,
                  typeid(map<int,int>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &maplEintcOintgR_Dictionary, isa_proxy, 0,
                  sizeof(map<int,int>) );
      instance.SetNew(&new_maplEintcOintgR);
      instance.SetNewArray(&newArray_maplEintcOintgR);
      instance.SetDelete(&delete_maplEintcOintgR);
      instance.SetDeleteArray(&deleteArray_maplEintcOintgR);
      instance.SetDestructor(&destruct_maplEintcOintgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::MapInsert< map<int,int> >()));

      ::ROOT::AddClassAlternate("map<int,int>","std::map<int, int, std::less<int>, std::allocator<std::pair<int const, int> > >");
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const map<int,int>*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *maplEintcOintgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const map<int,int>*)nullptr)->GetClass();
      maplEintcOintgR_TClassManip(theClass);
   return theClass;
   }

   static void maplEintcOintgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_maplEintcOintgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) map<int,int> : new map<int,int>;
   }
   static void *newArray_maplEintcOintgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) map<int,int>[nElements] : new map<int,int>[nElements];
   }
   // Wrapper around operator delete
   static void delete_maplEintcOintgR(void *p) {
      delete ((map<int,int>*)p);
   }
   static void deleteArray_maplEintcOintgR(void *p) {
      delete [] ((map<int,int>*)p);
   }
   static void destruct_maplEintcOintgR(void *p) {
      typedef map<int,int> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class map<int,int>

namespace {
  void TriggerDictionaryInitialization_MBDStudy_Dict_Impl() {
    static const char* headers[] = {
"0",
nullptr
    };
    static const char* includePaths[] = {
nullptr
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "MBDStudy_Dict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class MBDStudy;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "MBDStudy_Dict dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
#ifndef __MBDSTUDY_H__
#define __MBDSTUDY_H__

#include <fun4all/SubsysReco.h>
#include <string>
#include <map>
#include <TFile.h>

//Forward declerations
class PHCompositeNode;
class PHG4HitContainer;
class PHG4TruthInfoContainer;
class EventHeader;
class MbdOut;
class TFile;
class TTree;
class TDatabasePDG;
class TRandom3;
class TH1;
class TH2;
class TF1;
class TCanvas;


//Brief: basic ntuple and histogram creation for sim evaluation
class MBDStudy: public SubsysReco
{
public: 

  //Default constructor
  MBDStudy(const std::string &name="MBDStudy");

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

  void set_tres(const Float_t tr) { _tres = tr; }

private:

  //
  void CheckDST(PHCompositeNode *topNode);

  //output filename
  std::string _savefname;
  TFile* _savefile;
   
  int nprocessed{0};     // num events processed

  //Output
  TTree* _tree;
  Int_t    f_evt;
  Float_t  f_bimp;    // impact parameter
  Int_t    f_ncoll;   // number n-n collisions
  Int_t    f_npart;   // number participants
  Float_t  f_vx;      // true x vertex of collision point
  Float_t  f_vy;      // true y vertex
  Float_t  f_vz;      // true z vertex
  Float_t  f_vt;       // true start time
  Short_t  f_mbdn[2]; // num hits for each arm (north and south)
  Float_t  f_mbdq[2]; // total charge (currently npe) in each arm
  Float_t  f_mbdt[2]; // time in arm
  Float_t  f_mbdte[2]; // earliest hit time in arm
  Float_t  f_mbdz;    // z-vertex
  Float_t  f_mbdt0;   // start time

  TH1* h_mbdq[128];   // q in each tube
  TH1* h_mbdqtot[2];  // total q in mbd arms
  TH2* h2_mbdqtot;    // north q vs south q
  TH1* h_ztrue;       // true z-vertex
  TH1* h_tdiff;       // time diff between estimated and real time
  TH2* h2_tdiff_ch;   // time diff by channel

  TCanvas *c_mbdt;    // Canvas to 
  TH1 *hevt_mbdt[2];  // time in each mbd, per event
  TF1 *gaussian;

  std::map<int,int> _pids;  // PIDs of tracks in the MBD

  //
  TDatabasePDG* _pdg;
  TRandom3*     _rndm;
  Float_t       _tres;    // time resolution of one channel

  //Get all the nodes
  void GetNodes(PHCompositeNode *);
  
  //Node pointers
  PHG4TruthInfoContainer* _truth_container;
  PHG4HitContainer* _mbdhits;
  EventHeader* _evtheader;

};

#endif //* __MBDSTUDY_H__ *//

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"MBDStudy", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("MBDStudy_Dict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_MBDStudy_Dict_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_MBDStudy_Dict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_MBDStudy_Dict() {
  TriggerDictionaryInitialization_MBDStudy_Dict_Impl();
}
