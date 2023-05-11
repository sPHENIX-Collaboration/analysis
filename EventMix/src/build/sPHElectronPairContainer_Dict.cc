// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME sPHElectronPairContainer_Dict
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
#include "/sphenix/u/weihuma/analysis/EventMix/src/sPHElectronPairContainer.h"

// Header files passed via #pragma extra_include

namespace ROOT {
   static void delete_sPHElectronPairContainer(void *p);
   static void deleteArray_sPHElectronPairContainer(void *p);
   static void destruct_sPHElectronPairContainer(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::sPHElectronPairContainer*)
   {
      ::sPHElectronPairContainer *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::sPHElectronPairContainer >(0);
      static ::ROOT::TGenericClassInfo 
         instance("sPHElectronPairContainer", ::sPHElectronPairContainer::Class_Version(), "", 12,
                  typeid(::sPHElectronPairContainer), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::sPHElectronPairContainer::Dictionary, isa_proxy, 4,
                  sizeof(::sPHElectronPairContainer) );
      instance.SetDelete(&delete_sPHElectronPairContainer);
      instance.SetDeleteArray(&deleteArray_sPHElectronPairContainer);
      instance.SetDestructor(&destruct_sPHElectronPairContainer);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::sPHElectronPairContainer*)
   {
      return GenerateInitInstanceLocal((::sPHElectronPairContainer*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::sPHElectronPairContainer*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr sPHElectronPairContainer::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *sPHElectronPairContainer::Class_Name()
{
   return "sPHElectronPairContainer";
}

//______________________________________________________________________________
const char *sPHElectronPairContainer::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::sPHElectronPairContainer*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int sPHElectronPairContainer::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::sPHElectronPairContainer*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *sPHElectronPairContainer::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::sPHElectronPairContainer*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *sPHElectronPairContainer::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::sPHElectronPairContainer*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void sPHElectronPairContainer::Streamer(TBuffer &R__b)
{
   // Stream an object of class sPHElectronPairContainer.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(sPHElectronPairContainer::Class(),this);
   } else {
      R__b.WriteClassBuffer(sPHElectronPairContainer::Class(),this);
   }
}

namespace ROOT {
   // Wrapper around operator delete
   static void delete_sPHElectronPairContainer(void *p) {
      delete ((::sPHElectronPairContainer*)p);
   }
   static void deleteArray_sPHElectronPairContainer(void *p) {
      delete [] ((::sPHElectronPairContainer*)p);
   }
   static void destruct_sPHElectronPairContainer(void *p) {
      typedef ::sPHElectronPairContainer current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::sPHElectronPairContainer

namespace {
  void TriggerDictionaryInitialization_sPHElectronPairContainer_Dict_Impl() {
    static const char* headers[] = {
"0",
0
    };
    static const char* includePaths[] = {
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "sPHElectronPairContainer_Dict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class sPHElectronPairContainer;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "sPHElectronPairContainer_Dict dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
#ifndef __SPHELECTRONPAIRCONTAINER_H__
#define __SPHELECTRONPAIRCONTAINER_H__

#include <phool/PHObject.h>
#include <iostream>
#include <map>

#include "sPHElectronPair.h"

class sPHElectronPairContainer : public PHObject
{
public:
//  typedef std::map<unsigned int, sPHElectronPair*> PairMap;
//  typedef std::map<unsigned int, sPHElectronPair*>::const_iterator ConstIter;
//  typedef std::map<unsigned int, sPHElectronPair*>::iterator Iter;

  virtual ~sPHElectronPairContainer() {}

  virtual void identify(std::ostream& os = std::cout) const 
    { os << "sPHElectronPairContainer base class" << std::endl; }

  virtual void Reset() {}
  virtual void clear() {} 
  virtual int isValid() const { return 0; }
  virtual size_t size() const { return 0; }
  virtual bool empty() const  { return true; } // returns true if container empty
  virtual PHObject* CloneMe() const { return nullptr; }

//  virtual const sPHElectronPair* get(unsigned int id) const    {return nullptr;}
//  virtual sPHElectronPair* get(unsigned int id)                {return nullptr;}
//  virtual void insert(const sPHElectronPair* pair) {return nullptr;}
//  virtual size_t erase(unsigned int id)                        {return 0;}

//  virtual ConstIter begin() const { return PairMap().end(); }
//  virtual ConstIter find(unsigned int id) const { return PairMap().end(); }
//  virtual ConstIter end() const { return PairMap().end(); }

//  virtual Iter begin() { return PairMap().end(); }
//  virtual Iter find(unsigned int id) { return PairMap().end(); }
//  virtual Iter end() { return PairMap().end(); }

protected:
  sPHElectronPairContainer() {}
  ClassDef(sPHElectronPairContainer, 1);
};

#endif


#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"sPHElectronPairContainer", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("sPHElectronPairContainer_Dict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_sPHElectronPairContainer_Dict_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_sPHElectronPairContainer_Dict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_sPHElectronPairContainer_Dict() {
  TriggerDictionaryInitialization_sPHElectronPairContainer_Dict_Impl();
}
