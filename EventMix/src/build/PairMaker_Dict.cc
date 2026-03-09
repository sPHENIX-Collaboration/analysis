// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME PairMaker_Dict
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

// The generated code does not explicitly qualifies STL entities
namespace std {} using namespace std;

// Header files passed as explicit arguments
#include "/sphenix/u/weihuma/analysis/EventMix/src/PairMaker.h"

// Header files passed via #pragma extra_include

namespace ROOT {
   static TClass *PairMaker_Dictionary();
   static void PairMaker_TClassManip(TClass*);
   static void *new_PairMaker(void *p = 0);
   static void *newArray_PairMaker(Long_t size, void *p);
   static void delete_PairMaker(void *p);
   static void deleteArray_PairMaker(void *p);
   static void destruct_PairMaker(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::PairMaker*)
   {
      ::PairMaker *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::PairMaker));
      static ::ROOT::TGenericClassInfo 
         instance("PairMaker", "", 20,
                  typeid(::PairMaker), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &PairMaker_Dictionary, isa_proxy, 3,
                  sizeof(::PairMaker) );
      instance.SetNew(&new_PairMaker);
      instance.SetNewArray(&newArray_PairMaker);
      instance.SetDelete(&delete_PairMaker);
      instance.SetDeleteArray(&deleteArray_PairMaker);
      instance.SetDestructor(&destruct_PairMaker);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::PairMaker*)
   {
      return GenerateInitInstanceLocal((::PairMaker*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::PairMaker*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *PairMaker_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::PairMaker*)0x0)->GetClass();
      PairMaker_TClassManip(theClass);
   return theClass;
   }

   static void PairMaker_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_PairMaker(void *p) {
      return  p ? new(p) ::PairMaker : new ::PairMaker;
   }
   static void *newArray_PairMaker(Long_t nElements, void *p) {
      return p ? new(p) ::PairMaker[nElements] : new ::PairMaker[nElements];
   }
   // Wrapper around operator delete
   static void delete_PairMaker(void *p) {
      delete ((::PairMaker*)p);
   }
   static void deleteArray_PairMaker(void *p) {
      delete [] ((::PairMaker*)p);
   }
   static void destruct_PairMaker(void *p) {
      typedef ::PairMaker current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::PairMaker

namespace {
  void TriggerDictionaryInitialization_PairMaker_Dict_Impl() {
    static const char* headers[] = {
"0",
0
    };
    static const char* includePaths[] = {
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "PairMaker_Dict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class PairMaker;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "PairMaker_Dict dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers

#ifndef __PAIRMAKER_H__
#define __PAIRMAKER_H__

#include <fun4all/SubsysReco.h>

#include "sPHElectronv1.h"

#include <vector>

class TFile;
class TH1D;
class TRandom;
class SvtxTrack;
class sPHElectronPairContainerv1;


class PairMaker: public SubsysReco {

public:

  PairMaker(const std::string &name = "PairMaker", const std::string &filename = "test.root");
  virtual ~PairMaker() {}

  int Init(PHCompositeNode *topNode);
  int InitRun(PHCompositeNode *topNode);
  int process_event(PHCompositeNode *topNode);
  int End(PHCompositeNode *topNode);

protected:

  int process_event_test(PHCompositeNode *topNode);
  int MakeMixedPairs(std::vector<sPHElectronv1> elepos, sPHElectronPairContainerv1* eePairs, unsigned int centbin);

  bool isElectron(SvtxTrack*);

  std::string outnodename;
  static const int NZ = 2;
  static const int NCENT = 2;
  std::vector<sPHElectronv1> _buffer[NZ][NCENT];
  unsigned int _min_buffer_depth;
  unsigned int _max_buffer_depth;
  double _ZMAX;
  double _ZMIN;
  std::vector<double> _multbins;

  TRandom* _rng;

  int EventNumber;

};

#endif


#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"PairMaker", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("PairMaker_Dict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_PairMaker_Dict_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_PairMaker_Dict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_PairMaker_Dict() {
  TriggerDictionaryInitialization_PairMaker_Dict_Impl();
}
