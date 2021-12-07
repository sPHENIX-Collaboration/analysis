// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME sPHElectronPairv1_Dict
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
#include "/sphenix/u/weihuma/analysis/EventMix/src/sPHElectronPairv1.h"

// Header files passed via #pragma extra_include

namespace ROOT {
   static void *new_sPHElectronPairv1(void *p = 0);
   static void *newArray_sPHElectronPairv1(Long_t size, void *p);
   static void delete_sPHElectronPairv1(void *p);
   static void deleteArray_sPHElectronPairv1(void *p);
   static void destruct_sPHElectronPairv1(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::sPHElectronPairv1*)
   {
      ::sPHElectronPairv1 *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::sPHElectronPairv1 >(0);
      static ::ROOT::TGenericClassInfo 
         instance("sPHElectronPairv1", ::sPHElectronPairv1::Class_Version(), "", 13,
                  typeid(::sPHElectronPairv1), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::sPHElectronPairv1::Dictionary, isa_proxy, 4,
                  sizeof(::sPHElectronPairv1) );
      instance.SetNew(&new_sPHElectronPairv1);
      instance.SetNewArray(&newArray_sPHElectronPairv1);
      instance.SetDelete(&delete_sPHElectronPairv1);
      instance.SetDeleteArray(&deleteArray_sPHElectronPairv1);
      instance.SetDestructor(&destruct_sPHElectronPairv1);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::sPHElectronPairv1*)
   {
      return GenerateInitInstanceLocal((::sPHElectronPairv1*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::sPHElectronPairv1*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr sPHElectronPairv1::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *sPHElectronPairv1::Class_Name()
{
   return "sPHElectronPairv1";
}

//______________________________________________________________________________
const char *sPHElectronPairv1::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::sPHElectronPairv1*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int sPHElectronPairv1::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::sPHElectronPairv1*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *sPHElectronPairv1::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::sPHElectronPairv1*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *sPHElectronPairv1::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::sPHElectronPairv1*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void sPHElectronPairv1::Streamer(TBuffer &R__b)
{
   // Stream an object of class sPHElectronPairv1.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(sPHElectronPairv1::Class(),this);
   } else {
      R__b.WriteClassBuffer(sPHElectronPairv1::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_sPHElectronPairv1(void *p) {
      return  p ? new(p) ::sPHElectronPairv1 : new ::sPHElectronPairv1;
   }
   static void *newArray_sPHElectronPairv1(Long_t nElements, void *p) {
      return p ? new(p) ::sPHElectronPairv1[nElements] : new ::sPHElectronPairv1[nElements];
   }
   // Wrapper around operator delete
   static void delete_sPHElectronPairv1(void *p) {
      delete ((::sPHElectronPairv1*)p);
   }
   static void deleteArray_sPHElectronPairv1(void *p) {
      delete [] ((::sPHElectronPairv1*)p);
   }
   static void destruct_sPHElectronPairv1(void *p) {
      typedef ::sPHElectronPairv1 current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::sPHElectronPairv1

namespace {
  void TriggerDictionaryInitialization_sPHElectronPairv1_Dict_Impl() {
    static const char* headers[] = {
"0",
0
    };
    static const char* includePaths[] = {
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "sPHElectronPairv1_Dict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class sPHElectronPairv1;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "sPHElectronPairv1_Dict dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
#ifndef SPHELECTRONPAIRV1_H
#define SPHELECTRONPAIRV1_H

#include <climits>
#include <cmath>
#include <iostream>

#include "sPHElectronPair.h"
#include "sPHElectronv1.h"

class sPHElectronPairv1 : public sPHElectronPair
{
 public:
  sPHElectronPairv1();
  sPHElectronPairv1(sPHElectronv1* e1, sPHElectronv1* e2);
  virtual ~sPHElectronPairv1() {}

  virtual void identify(std::ostream& os = std::cout) const
     { os << "sPHElectronPairv1 object class" << std::endl; }
  virtual void Reset() {}
  virtual int isValid() const { return 1; }
  virtual PHObject* CloneMe() const { return new sPHElectronPairv1(*this); }

  virtual sPHElectron* get_first()  { return &_e1; }
  virtual sPHElectron* get_second() { return &_e2; }

  virtual int get_id() const { return _id; }
  virtual int get_type() const { return _type; }
  virtual double get_mass() const;
  virtual double get_pt()   const;
  virtual double get_eta()  const;
  virtual double get_phiv() const;
  virtual double get_min_mass() const { return _min_mass; }
 
  virtual void set_id(int id) { _id = id;}
  virtual void set_type(int type) { _type = type;}
  virtual void set_min_mass(double mm) { _min_mass = mm;}

 protected:

  int _id;
  int _type;
  double _min_mass;
  sPHElectronv1 _e1;
  sPHElectronv1 _e2;

  ClassDef(sPHElectronPairv1, 1)
};

#endif 

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"sPHElectronPairv1", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("sPHElectronPairv1_Dict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_sPHElectronPairv1_Dict_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_sPHElectronPairv1_Dict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_sPHElectronPairv1_Dict() {
  TriggerDictionaryInitialization_sPHElectronPairv1_Dict_Impl();
}
