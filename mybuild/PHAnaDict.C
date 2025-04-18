// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME PHAnaDict

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

// Since CINT ignores the std namespace, we need to do so in this file.
namespace std {} using namespace std;

// Header files passed as explicit arguments
#include "/sphenix/u/weihuma/analysis/Ana/PHAna.h"

// Header files passed via #pragma extra_include

namespace ROOT {
   static TClass *PHAna_Dictionary();
   static void PHAna_TClassManip(TClass*);
   static void *new_PHAna(void *p = 0);
   static void *newArray_PHAna(Long_t size, void *p);
   static void delete_PHAna(void *p);
   static void deleteArray_PHAna(void *p);
   static void destruct_PHAna(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::PHAna*)
   {
      ::PHAna *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::PHAna));
      static ::ROOT::TGenericClassInfo 
         instance("PHAna", "PHAna.h", 16,
                  typeid(::PHAna), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &PHAna_Dictionary, isa_proxy, 3,
                  sizeof(::PHAna) );
      instance.SetNew(&new_PHAna);
      instance.SetNewArray(&newArray_PHAna);
      instance.SetDelete(&delete_PHAna);
      instance.SetDeleteArray(&deleteArray_PHAna);
      instance.SetDestructor(&destruct_PHAna);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::PHAna*)
   {
      return GenerateInitInstanceLocal((::PHAna*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::PHAna*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *PHAna_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::PHAna*)0x0)->GetClass();
      PHAna_TClassManip(theClass);
   return theClass;
   }

   static void PHAna_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_PHAna(void *p) {
      return  p ? new(p) ::PHAna : new ::PHAna;
   }
   static void *newArray_PHAna(Long_t nElements, void *p) {
      return p ? new(p) ::PHAna[nElements] : new ::PHAna[nElements];
   }
   // Wrapper around operator delete
   static void delete_PHAna(void *p) {
      delete ((::PHAna*)p);
   }
   static void deleteArray_PHAna(void *p) {
      delete [] ((::PHAna*)p);
   }
   static void destruct_PHAna(void *p) {
      typedef ::PHAna current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::PHAna

namespace {
  void TriggerDictionaryInitialization_PHAnaDict_Impl() {
    static const char* headers[] = {
"/sphenix/u/weihuma/analysis/Ana/PHAna.h",
0
    };
    static const char* includePaths[] = {
"/sphenix/u/weihuma/analysis/Ana",
"/usr/local/include",
"/cvmfs/sphenix.sdcc.bnl.gov/x8664_sl7/release/release_new/new.4/include",
"/cvmfs/sphenix.sdcc.bnl.gov/x8664_sl7/opt/sphenix/core/root-6.16.00/include",
"/cvmfs/sphenix.sdcc.bnl.gov/x8664_sl7/opt/sphenix/core/root-6.16.00/include",
"/direct/sphenix+u/weihuma/analysis/mybuild/",
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "PHAnaDict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_Autoloading_Map;
class __attribute__((annotate("$clingAutoload$/sphenix/u/weihuma/analysis/Ana/PHAna.h")))  PHAna;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "PHAnaDict dictionary payload"

#ifndef G__VECTOR_HAS_CLASS_ITERATOR
  #define G__VECTOR_HAS_CLASS_ITERATOR 1
#endif
#ifndef PACKAGE_NAME
  #define PACKAGE_NAME "PHAna"
#endif
#ifndef PACKAGE_TARNAME
  #define PACKAGE_TARNAME "phana"
#endif
#ifndef PACKAGE_VERSION
  #define PACKAGE_VERSION "1.00"
#endif
#ifndef PACKAGE_STRING
  #define PACKAGE_STRING "PHAna 1.00"
#endif
#ifndef PACKAGE_BUGREPORT
  #define PACKAGE_BUGREPORT ""
#endif
#ifndef PACKAGE_URL
  #define PACKAGE_URL ""
#endif
#ifndef PACKAGE
  #define PACKAGE "phana"
#endif
#ifndef VERSION
  #define VERSION "1.00"
#endif
#ifndef STDC_HEADERS
  #define STDC_HEADERS 1
#endif
#ifndef HAVE_SYS_TYPES_H
  #define HAVE_SYS_TYPES_H 1
#endif
#ifndef HAVE_SYS_STAT_H
  #define HAVE_SYS_STAT_H 1
#endif
#ifndef HAVE_STDLIB_H
  #define HAVE_STDLIB_H 1
#endif
#ifndef HAVE_STRING_H
  #define HAVE_STRING_H 1
#endif
#ifndef HAVE_MEMORY_H
  #define HAVE_MEMORY_H 1
#endif
#ifndef HAVE_STRINGS_H
  #define HAVE_STRINGS_H 1
#endif
#ifndef HAVE_INTTYPES_H
  #define HAVE_INTTYPES_H 1
#endif
#ifndef HAVE_STDINT_H
  #define HAVE_STDINT_H 1
#endif
#ifndef HAVE_UNISTD_H
  #define HAVE_UNISTD_H 1
#endif
#ifndef HAVE_DLFCN_H
  #define HAVE_DLFCN_H 1
#endif
#ifndef LT_OBJDIR
  #define LT_OBJDIR ".libs/"
#endif

#define _BACKWARD_BACKWARD_WARNING_H
#include "/sphenix/u/weihuma/analysis/Ana/PHAna.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[]={
"PHAna", payloadCode, "@",
nullptr};

    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("PHAnaDict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_PHAnaDict_Impl, {}, classesHeaders, /*has no C++ module*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_PHAnaDict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_PHAnaDict() {
  TriggerDictionaryInitialization_PHAnaDict_Impl();
}
