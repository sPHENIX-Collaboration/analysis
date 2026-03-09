// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME structureDict
#define R__NO_DEPRECATION

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "ROOT/RConfig.hxx"
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
#include "structure.h"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace ROOT {
   static void *new_pair_str(void *p = nullptr);
   static void *newArray_pair_str(Long_t size, void *p);
   static void delete_pair_str(void *p);
   static void deleteArray_pair_str(void *p);
   static void destruct_pair_str(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::pair_str*)
   {
      ::pair_str *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::pair_str >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("pair_str", ::pair_str::Class_Version(), "structure.h", 39,
                  typeid(::pair_str), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::pair_str::Dictionary, isa_proxy, 4,
                  sizeof(::pair_str) );
      instance.SetNew(&new_pair_str);
      instance.SetNewArray(&newArray_pair_str);
      instance.SetDelete(&delete_pair_str);
      instance.SetDeleteArray(&deleteArray_pair_str);
      instance.SetDestructor(&destruct_pair_str);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::pair_str*)
   {
      return GenerateInitInstanceLocal(static_cast<::pair_str*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::pair_str*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr pair_str::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *pair_str::Class_Name()
{
   return "pair_str";
}

//______________________________________________________________________________
const char *pair_str::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::pair_str*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int pair_str::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::pair_str*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *pair_str::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::pair_str*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *pair_str::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::pair_str*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void pair_str::Streamer(TBuffer &R__b)
{
   // Stream an object of class pair_str.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(pair_str::Class(),this);
   } else {
      R__b.WriteClassBuffer(pair_str::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_pair_str(void *p) {
      return  p ? new(p) ::pair_str : new ::pair_str;
   }
   static void *newArray_pair_str(Long_t nElements, void *p) {
      return p ? new(p) ::pair_str[nElements] : new ::pair_str[nElements];
   }
   // Wrapper around operator delete
   static void delete_pair_str(void *p) {
      delete (static_cast<::pair_str*>(p));
   }
   static void deleteArray_pair_str(void *p) {
      delete [] (static_cast<::pair_str*>(p));
   }
   static void destruct_pair_str(void *p) {
      typedef ::pair_str current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::pair_str

namespace ROOT {
   static TClass *vectorlEpair_strgR_Dictionary();
   static void vectorlEpair_strgR_TClassManip(TClass*);
   static void *new_vectorlEpair_strgR(void *p = nullptr);
   static void *newArray_vectorlEpair_strgR(Long_t size, void *p);
   static void delete_vectorlEpair_strgR(void *p);
   static void deleteArray_vectorlEpair_strgR(void *p);
   static void destruct_vectorlEpair_strgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<pair_str>*)
   {
      vector<pair_str> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<pair_str>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<pair_str>", -2, "vector", 428,
                  typeid(vector<pair_str>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEpair_strgR_Dictionary, isa_proxy, 4,
                  sizeof(vector<pair_str>) );
      instance.SetNew(&new_vectorlEpair_strgR);
      instance.SetNewArray(&newArray_vectorlEpair_strgR);
      instance.SetDelete(&delete_vectorlEpair_strgR);
      instance.SetDeleteArray(&deleteArray_vectorlEpair_strgR);
      instance.SetDestructor(&destruct_vectorlEpair_strgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<pair_str> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("vector<pair_str>","std::vector<pair_str, std::allocator<pair_str> >"));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const vector<pair_str>*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEpair_strgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const vector<pair_str>*>(nullptr))->GetClass();
      vectorlEpair_strgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEpair_strgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEpair_strgR(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<pair_str> : new vector<pair_str>;
   }
   static void *newArray_vectorlEpair_strgR(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<pair_str>[nElements] : new vector<pair_str>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEpair_strgR(void *p) {
      delete (static_cast<vector<pair_str>*>(p));
   }
   static void deleteArray_vectorlEpair_strgR(void *p) {
      delete [] (static_cast<vector<pair_str>*>(p));
   }
   static void destruct_vectorlEpair_strgR(void *p) {
      typedef vector<pair_str> current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class vector<pair_str>

namespace {
  void TriggerDictionaryInitialization_structureDict_Impl() {
    static const char* headers[] = {
"structure.h",
nullptr
    };
    static const char* includePaths[] = {
"/cvmfs/sphenix.sdcc.bnl.gov/alma9.2-gcc-14.2.0/opt/sphenix/core/root-6.32.06/include/",
"/gpfs/mnt/gpfs02/sphenix/user/ChengWei/INTT/INTT_dNdeta_repo/NewCode2024/EvtVtxZTracklet/",
nullptr
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "structureDict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
struct __attribute__((annotate("$clingAutoload$structure.h")))  pair_str;
namespace std{template <typename _Tp> class __attribute__((annotate("$clingAutoload$bits/allocator.h")))  __attribute__((annotate("$clingAutoload$string")))  allocator;
}
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "structureDict dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
#include "structure.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"pair_str", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("structureDict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_structureDict_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_structureDict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_structureDict() {
  TriggerDictionaryInitialization_structureDict_Impl();
}
