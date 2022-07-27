// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME sPHElectron_Dict
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
#include "/sphenix/u/weihuma/analysis/EventMix/src/sPHElectron.h"

// Header files passed via #pragma extra_include

namespace ROOT {
   static void delete_sPHElectron(void *p);
   static void deleteArray_sPHElectron(void *p);
   static void destruct_sPHElectron(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::sPHElectron*)
   {
      ::sPHElectron *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::sPHElectron >(0);
      static ::ROOT::TGenericClassInfo 
         instance("sPHElectron", ::sPHElectron::Class_Version(), "", 12,
                  typeid(::sPHElectron), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::sPHElectron::Dictionary, isa_proxy, 4,
                  sizeof(::sPHElectron) );
      instance.SetDelete(&delete_sPHElectron);
      instance.SetDeleteArray(&deleteArray_sPHElectron);
      instance.SetDestructor(&destruct_sPHElectron);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::sPHElectron*)
   {
      return GenerateInitInstanceLocal((::sPHElectron*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::sPHElectron*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr sPHElectron::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *sPHElectron::Class_Name()
{
   return "sPHElectron";
}

//______________________________________________________________________________
const char *sPHElectron::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::sPHElectron*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int sPHElectron::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::sPHElectron*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *sPHElectron::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::sPHElectron*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *sPHElectron::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::sPHElectron*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void sPHElectron::Streamer(TBuffer &R__b)
{
   // Stream an object of class sPHElectron.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(sPHElectron::Class(),this);
   } else {
      R__b.WriteClassBuffer(sPHElectron::Class(),this);
   }
}

namespace ROOT {
   // Wrapper around operator delete
   static void delete_sPHElectron(void *p) {
      delete ((::sPHElectron*)p);
   }
   static void deleteArray_sPHElectron(void *p) {
      delete [] ((::sPHElectron*)p);
   }
   static void destruct_sPHElectron(void *p) {
      typedef ::sPHElectron current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::sPHElectron

namespace {
  void TriggerDictionaryInitialization_sPHElectron_Dict_Impl() {
    static const char* headers[] = {
"0",
0
    };
    static const char* includePaths[] = {
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "sPHElectron_Dict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class sPHElectron;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "sPHElectron_Dict dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
#ifndef SPHELECTRON_H
#define SPHELECTRON_H

#include <phool/PHObject.h>

#include <climits>
#include <cmath>
#include <iostream>

class sPHElectron : public PHObject
{
 public:
  virtual ~sPHElectron() {}

  virtual void identify(std::ostream& os = std::cout) const
     { os << "sPHElectron base class" << std::endl; }
  virtual void Reset() {}
  virtual int isValid() const { return 0; }
  virtual PHObject* CloneMe() const { return nullptr; }

  virtual unsigned int get_id()  const { return 99999; }
  virtual int get_charge()       const { return 0; }
  virtual double get_px()        const { return NAN;}
  virtual double get_py()        const { return NAN;}
  virtual double get_pz()        const { return NAN;}
  virtual double get_dphi()      const { return NAN;}
  virtual double get_deta()      const { return NAN;}
  virtual double get_emce()      const { return NAN;}
  virtual double get_e3x3()      const { return NAN;}
  virtual double get_e5x5()      const { return NAN;}

  virtual double get_chi2()      const { return NAN;}
  virtual unsigned int get_ndf() const { return 99999;}
  virtual double get_zvtx()      const { return NAN;}
  virtual double get_dca2d()     const { return NAN;}
  virtual double get_dca2d_error()      const { return NAN;}
  virtual double get_dca3d_xy()  const { return NAN;}
  virtual double get_dca3d_z()   const { return NAN;}

  virtual int get_nmvtx()   const { return -1;}
  virtual int get_ntpc()   const { return -1;}

  virtual double get_cemc_ecore()   const { return NAN;}
  virtual double get_cemc_chi2()   const { return NAN;}
  virtual double get_cemc_prob()   const { return NAN;}
  virtual double get_cemc_dphi()   const { return NAN;}
  virtual double get_cemc_deta()   const { return NAN;}
  virtual double get_hcalin_e()   const { return NAN;}
  virtual double get_hcalin_dphi()   const { return NAN;}
  virtual double get_hcalin_deta()   const { return NAN;}

  virtual void set_id(unsigned int id) { }
  virtual void set_charge(int charge)  { }
  virtual void set_px(double px)       { }
  virtual void set_py(double px)       { }
  virtual void set_pz(double pz)       { }
  virtual void set_dphi(double dphi)   { }
  virtual void set_deta(double deta)   { }
  virtual void set_emce(double emce)   { }
  virtual void set_e3x3(double e3x3)   { }
  virtual void set_e5x5(double e5x5)   { }

  virtual void set_chi2(double a)      { }
  virtual void set_ndf(unsigned int a) { }
  virtual void set_zvtx(double a)      { }
  virtual void set_dca2d(double a)     { }
  virtual void set_dca2d_error(double a)   { }
  virtual void set_dca3d_xy(double a)  { }
  virtual void set_dca3d_z(double a)   { }

  virtual void set_nmvtx(int i)   { }
  virtual void set_ntpc(int i)   { }

  virtual void set_cemc_ecore(double a)   { }
  virtual void set_cemc_chi2(double a)   { }
  virtual void set_cemc_prob(double a)   { }
  virtual void set_cemc_dphi(double a)   { }
  virtual void set_cemc_deta(double a)   { }
  virtual void set_hcalin_e(double a)   { }
  virtual void set_hcalin_dphi(double a)   { }
  virtual void set_hcalin_deta(double a)   { }

 protected:
  sPHElectron() {}
  ClassDef(sPHElectron, 1)
};

#endif 

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"sPHElectron", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("sPHElectron_Dict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_sPHElectron_Dict_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_sPHElectron_Dict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_sPHElectron_Dict() {
  TriggerDictionaryInitialization_sPHElectron_Dict_Impl();
}
