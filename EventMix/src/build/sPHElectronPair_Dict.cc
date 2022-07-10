// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME sPHElectronPair_Dict
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
#include "/sphenix/u/weihuma/analysis/EventMix/src/sPHElectronPair.h"

// Header files passed via #pragma extra_include

namespace ROOT {
   static void delete_sPHElectronPair(void *p);
   static void deleteArray_sPHElectronPair(void *p);
   static void destruct_sPHElectronPair(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::sPHElectronPair*)
   {
      ::sPHElectronPair *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::sPHElectronPair >(0);
      static ::ROOT::TGenericClassInfo 
         instance("sPHElectronPair", ::sPHElectronPair::Class_Version(), "", 14,
                  typeid(::sPHElectronPair), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::sPHElectronPair::Dictionary, isa_proxy, 4,
                  sizeof(::sPHElectronPair) );
      instance.SetDelete(&delete_sPHElectronPair);
      instance.SetDeleteArray(&deleteArray_sPHElectronPair);
      instance.SetDestructor(&destruct_sPHElectronPair);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::sPHElectronPair*)
   {
      return GenerateInitInstanceLocal((::sPHElectronPair*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::sPHElectronPair*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr sPHElectronPair::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *sPHElectronPair::Class_Name()
{
   return "sPHElectronPair";
}

//______________________________________________________________________________
const char *sPHElectronPair::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::sPHElectronPair*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int sPHElectronPair::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::sPHElectronPair*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *sPHElectronPair::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::sPHElectronPair*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *sPHElectronPair::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::sPHElectronPair*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void sPHElectronPair::Streamer(TBuffer &R__b)
{
   // Stream an object of class sPHElectronPair.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(sPHElectronPair::Class(),this);
   } else {
      R__b.WriteClassBuffer(sPHElectronPair::Class(),this);
   }
}

namespace ROOT {
   // Wrapper around operator delete
   static void delete_sPHElectronPair(void *p) {
      delete ((::sPHElectronPair*)p);
   }
   static void deleteArray_sPHElectronPair(void *p) {
      delete [] ((::sPHElectronPair*)p);
   }
   static void destruct_sPHElectronPair(void *p) {
      typedef ::sPHElectronPair current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::sPHElectronPair

namespace {
  void TriggerDictionaryInitialization_sPHElectronPair_Dict_Impl() {
    static const char* headers[] = {
"0",
0
    };
    static const char* includePaths[] = {
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "sPHElectronPair_Dict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class sPHElectronPair;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "sPHElectronPair_Dict dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
#ifndef SPHELECTRONPAIR_H
#define SPHELECTRONPAIR_H

#include <phool/PHObject.h>

#include <climits>
#include <cmath>
#include <iostream>

class sPHElectron;

class sPHElectronPair : public PHObject
{
 public:
  virtual ~sPHElectronPair() {}

  virtual void identify(std::ostream& os = std::cout) const
     { os << "sPHElectronPair base class" << std::endl; }
  virtual void Reset() {}
  virtual int isValid() const { return 0; }
  virtual PHObject* CloneMe() const { return nullptr; }

  virtual sPHElectron* get_first()  { return nullptr; }
  virtual sPHElectron* get_second() { return nullptr; }

  virtual int get_id()    const { return -99999; }
  virtual int get_type()    const { return 0; }
  virtual double get_mass() const { return NAN; }
  virtual double get_pt()   const { return NAN; }
  virtual double get_eta()  const { return NAN; }
  virtual double get_phiv() const { return NAN; }
  virtual double get_min_mass() const { return NAN; }

  virtual void set_id(int id) { }
  virtual void set_type(int type) { }
  virtual void set_min_mass(double mm) { }

 protected:
  sPHElectronPair() {}
  ClassDef(sPHElectronPair, 1)
};

#endif 

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"sPHElectronPair", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("sPHElectronPair_Dict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_sPHElectronPair_Dict_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_sPHElectronPair_Dict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_sPHElectronPair_Dict() {
  TriggerDictionaryInitialization_sPHElectronPair_Dict_Impl();
}
