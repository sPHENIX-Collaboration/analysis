// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME BJetModule_Dict
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
#include "../BJetModule.h"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace ROOT {
   static TClass *BJetModule_Dictionary();
   static void BJetModule_TClassManip(TClass*);
   static void *new_BJetModule(void *p = 0);
   static void *newArray_BJetModule(Long_t size, void *p);
   static void delete_BJetModule(void *p);
   static void deleteArray_BJetModule(void *p);
   static void destruct_BJetModule(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::BJetModule*)
   {
      ::BJetModule *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::BJetModule));
      static ::ROOT::TGenericClassInfo 
         instance("BJetModule", "BJetModule.h", 24,
                  typeid(::BJetModule), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &BJetModule_Dictionary, isa_proxy, 1,
                  sizeof(::BJetModule) );
      instance.SetNew(&new_BJetModule);
      instance.SetNewArray(&newArray_BJetModule);
      instance.SetDelete(&delete_BJetModule);
      instance.SetDeleteArray(&deleteArray_BJetModule);
      instance.SetDestructor(&destruct_BJetModule);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::BJetModule*)
   {
      return GenerateInitInstanceLocal((::BJetModule*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::BJetModule*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *BJetModule_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::BJetModule*)0x0)->GetClass();
      BJetModule_TClassManip(theClass);
   return theClass;
   }

   static void BJetModule_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_BJetModule(void *p) {
      return  p ? new(p) ::BJetModule : new ::BJetModule;
   }
   static void *newArray_BJetModule(Long_t nElements, void *p) {
      return p ? new(p) ::BJetModule[nElements] : new ::BJetModule[nElements];
   }
   // Wrapper around operator delete
   static void delete_BJetModule(void *p) {
      delete ((::BJetModule*)p);
   }
   static void deleteArray_BJetModule(void *p) {
      delete [] ((::BJetModule*)p);
   }
   static void destruct_BJetModule(void *p) {
      typedef ::BJetModule current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::BJetModule

namespace {
  void TriggerDictionaryInitialization_BJetModule_Dict_Impl() {
    static const char* headers[] = {
"../BJetModule.h",
0
    };
    static const char* includePaths[] = {
"..",
"/sphenix/u/cdean/sPHENIX/install/include",
"/include",
"/cvmfs/sphenix.sdcc.bnl.gov/gcc-8.3/release/release_new/new.1/include",
"/cvmfs/sphenix.sdcc.bnl.gov/gcc-8.3/opt/sphenix/core/root-6.24.06/include",
"/cvmfs/sphenix.sdcc.bnl.gov/gcc-8.3/opt/sphenix/core/root-6.24.06/include/",
"/gpfs/mnt/gpfs02/sphenix/user/cdean/software/analysis/HF-Jet/HighDCATrackCounting/build/",
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "BJetModule_Dict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class __attribute__((annotate("$clingAutoload$../BJetModule.h")))  BJetModule;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "BJetModule_Dict dictionary payload"

#ifndef PACKAGE_NAME
  #define PACKAGE_NAME ""
#endif
#ifndef PACKAGE_TARNAME
  #define PACKAGE_TARNAME ""
#endif
#ifndef PACKAGE_VERSION
  #define PACKAGE_VERSION ""
#endif
#ifndef PACKAGE_STRING
  #define PACKAGE_STRING ""
#endif
#ifndef PACKAGE_BUGREPORT
  #define PACKAGE_BUGREPORT ""
#endif
#ifndef PACKAGE_URL
  #define PACKAGE_URL ""
#endif
#ifndef PACKAGE
  #define PACKAGE "g4picoDst"
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
// Inline headers
#include "../BJetModule.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"BJetModule", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("BJetModule_Dict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_BJetModule_Dict_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_BJetModule_Dict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_BJetModule_Dict() {
  TriggerDictionaryInitialization_BJetModule_Dict_Impl();
}
