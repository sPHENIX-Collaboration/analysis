// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME INTTvtxZF4AObjDict
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
#include "INTTvtxZF4AObj.h"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace ROOT {
   static void *new_INTTvtxZF4AObj(void *p = nullptr);
   static void *newArray_INTTvtxZF4AObj(Long_t size, void *p);
   static void delete_INTTvtxZF4AObj(void *p);
   static void deleteArray_INTTvtxZF4AObj(void *p);
   static void destruct_INTTvtxZF4AObj(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::INTTvtxZF4AObj*)
   {
      ::INTTvtxZF4AObj *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::INTTvtxZF4AObj >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("INTTvtxZF4AObj", ::INTTvtxZF4AObj::Class_Version(), "INTTvtxZF4AObj.h", 10,
                  typeid(::INTTvtxZF4AObj), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::INTTvtxZF4AObj::Dictionary, isa_proxy, 4,
                  sizeof(::INTTvtxZF4AObj) );
      instance.SetNew(&new_INTTvtxZF4AObj);
      instance.SetNewArray(&newArray_INTTvtxZF4AObj);
      instance.SetDelete(&delete_INTTvtxZF4AObj);
      instance.SetDeleteArray(&deleteArray_INTTvtxZF4AObj);
      instance.SetDestructor(&destruct_INTTvtxZF4AObj);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::INTTvtxZF4AObj*)
   {
      return GenerateInitInstanceLocal((::INTTvtxZF4AObj*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::INTTvtxZF4AObj*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr INTTvtxZF4AObj::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *INTTvtxZF4AObj::Class_Name()
{
   return "INTTvtxZF4AObj";
}

//______________________________________________________________________________
const char *INTTvtxZF4AObj::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::INTTvtxZF4AObj*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int INTTvtxZF4AObj::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::INTTvtxZF4AObj*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *INTTvtxZF4AObj::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::INTTvtxZF4AObj*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *INTTvtxZF4AObj::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::INTTvtxZF4AObj*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void INTTvtxZF4AObj::Streamer(TBuffer &R__b)
{
   // Stream an object of class INTTvtxZF4AObj.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(INTTvtxZF4AObj::Class(),this);
   } else {
      R__b.WriteClassBuffer(INTTvtxZF4AObj::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_INTTvtxZF4AObj(void *p) {
      return  p ? new(p) ::INTTvtxZF4AObj : new ::INTTvtxZF4AObj;
   }
   static void *newArray_INTTvtxZF4AObj(Long_t nElements, void *p) {
      return p ? new(p) ::INTTvtxZF4AObj[nElements] : new ::INTTvtxZF4AObj[nElements];
   }
   // Wrapper around operator delete
   static void delete_INTTvtxZF4AObj(void *p) {
      delete ((::INTTvtxZF4AObj*)p);
   }
   static void deleteArray_INTTvtxZF4AObj(void *p) {
      delete [] ((::INTTvtxZF4AObj*)p);
   }
   static void destruct_INTTvtxZF4AObj(void *p) {
      typedef ::INTTvtxZF4AObj current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::INTTvtxZF4AObj

namespace {
  void TriggerDictionaryInitialization_INTTvtxZF4AObjDict_Impl() {
    static const char* headers[] = {
"INTTvtxZF4AObj.h",
nullptr
    };
    static const char* includePaths[] = {
"/cvmfs/sphenix.sdcc.bnl.gov/gcc-12.1.0/opt/sphenix/core/root-6.26.06.p01/include/",
"/gpfs/mnt/gpfs02/sphenix/user/ChengWei/sPH_analysis_repo/INTTvtxZTrapezoidal/",
nullptr
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "INTTvtxZF4AObjDict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class __attribute__((annotate("$clingAutoload$INTTvtxZF4AObj.h")))  INTTvtxZF4AObj;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "INTTvtxZF4AObjDict dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
#include "INTTvtxZF4AObj.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"INTTvtxZF4AObj", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("INTTvtxZF4AObjDict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_INTTvtxZF4AObjDict_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_INTTvtxZF4AObjDict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_INTTvtxZF4AObjDict() {
  TriggerDictionaryInitialization_INTTvtxZF4AObjDict_Impl();
}
