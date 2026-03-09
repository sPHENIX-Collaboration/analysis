// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME Analyze_dict
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
#include "Analyze.h"
#include "InttEvent.h"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace ROOT {
   static void *new_InttHit(void *p = nullptr);
   static void *newArray_InttHit(Long_t size, void *p);
   static void delete_InttHit(void *p);
   static void deleteArray_InttHit(void *p);
   static void destruct_InttHit(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::InttHit*)
   {
      ::InttHit *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::InttHit >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("InttHit", ::InttHit::Class_Version(), "InttEvent.h", 9,
                  typeid(::InttHit), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::InttHit::Dictionary, isa_proxy, 4,
                  sizeof(::InttHit) );
      instance.SetNew(&new_InttHit);
      instance.SetNewArray(&newArray_InttHit);
      instance.SetDelete(&delete_InttHit);
      instance.SetDeleteArray(&deleteArray_InttHit);
      instance.SetDestructor(&destruct_InttHit);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::InttHit*)
   {
      return GenerateInitInstanceLocal((::InttHit*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::InttHit*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_InttEvent(void *p = nullptr);
   static void *newArray_InttEvent(Long_t size, void *p);
   static void delete_InttEvent(void *p);
   static void deleteArray_InttEvent(void *p);
   static void destruct_InttEvent(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::InttEvent*)
   {
      ::InttEvent *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::InttEvent >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("InttEvent", ::InttEvent::Class_Version(), "InttEvent.h", 52,
                  typeid(::InttEvent), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::InttEvent::Dictionary, isa_proxy, 4,
                  sizeof(::InttEvent) );
      instance.SetNew(&new_InttEvent);
      instance.SetNewArray(&newArray_InttEvent);
      instance.SetDelete(&delete_InttEvent);
      instance.SetDeleteArray(&deleteArray_InttEvent);
      instance.SetDestructor(&destruct_InttEvent);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::InttEvent*)
   {
      return GenerateInitInstanceLocal((::InttEvent*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::InttEvent*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr InttHit::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *InttHit::Class_Name()
{
   return "InttHit";
}

//______________________________________________________________________________
const char *InttHit::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::InttHit*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int InttHit::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::InttHit*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *InttHit::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::InttHit*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *InttHit::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::InttHit*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr InttEvent::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *InttEvent::Class_Name()
{
   return "InttEvent";
}

//______________________________________________________________________________
const char *InttEvent::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::InttEvent*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int InttEvent::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::InttEvent*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *InttEvent::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::InttEvent*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *InttEvent::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::InttEvent*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void InttHit::Streamer(TBuffer &R__b)
{
   // Stream an object of class InttHit.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(InttHit::Class(),this);
   } else {
      R__b.WriteClassBuffer(InttHit::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_InttHit(void *p) {
      return  p ? new(p) ::InttHit : new ::InttHit;
   }
   static void *newArray_InttHit(Long_t nElements, void *p) {
      return p ? new(p) ::InttHit[nElements] : new ::InttHit[nElements];
   }
   // Wrapper around operator delete
   static void delete_InttHit(void *p) {
      delete ((::InttHit*)p);
   }
   static void deleteArray_InttHit(void *p) {
      delete [] ((::InttHit*)p);
   }
   static void destruct_InttHit(void *p) {
      typedef ::InttHit current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::InttHit

//______________________________________________________________________________
void InttEvent::Streamer(TBuffer &R__b)
{
   // Stream an object of class InttEvent.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(InttEvent::Class(),this);
   } else {
      R__b.WriteClassBuffer(InttEvent::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_InttEvent(void *p) {
      return  p ? new(p) ::InttEvent : new ::InttEvent;
   }
   static void *newArray_InttEvent(Long_t nElements, void *p) {
      return p ? new(p) ::InttEvent[nElements] : new ::InttEvent[nElements];
   }
   // Wrapper around operator delete
   static void delete_InttEvent(void *p) {
      delete ((::InttEvent*)p);
   }
   static void deleteArray_InttEvent(void *p) {
      delete [] ((::InttEvent*)p);
   }
   static void destruct_InttEvent(void *p) {
      typedef ::InttEvent current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::InttEvent

namespace {
  void TriggerDictionaryInitialization_Analyze_dict_Impl() {
    static const char* headers[] = {
"Analyze.h",
"InttEvent.h",
nullptr
    };
    static const char* includePaths[] = {
"/cvmfs/sphenix.sdcc.bnl.gov/gcc-12.1.0/release/release_new/new.9/include",
"/cvmfs/sphenix.sdcc.bnl.gov/gcc-12.1.0/release/release_new/new.9/include",
"/cvmfs/sphenix.sdcc.bnl.gov/gcc-12.1.0/opt/sphenix/core/root-6.26.06.p01/include/",
"/gpfs/mnt/gpfs02/sphenix/user/hachiya/INTT/INTT/general_codes/hachiya/mbd_sync/",
nullptr
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "Analyze_dict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class __attribute__((annotate("$clingAutoload$InttEvent.h")))  InttHit;
class __attribute__((annotate("$clingAutoload$InttEvent.h")))  InttEvent;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "Analyze_dict dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
#include "Analyze.h"
#include "InttEvent.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"InitAnalysis", payloadCode, "@",
"InttEvent", payloadCode, "@",
"InttHit", payloadCode, "@",
"RunAnalysis", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("Analyze_dict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_Analyze_dict_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_Analyze_dict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_Analyze_dict() {
  TriggerDictionaryInitialization_Analyze_dict_Impl();
}
