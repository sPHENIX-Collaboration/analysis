// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME sPHElectronPairContainerv1_Dict
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
#include "/sphenix/u/weihuma/analysis/EventMix/src/sPHElectronPairContainerv1.h"

// Header files passed via #pragma extra_include

namespace ROOT {
   static void *new_sPHElectronPairContainerv1(void *p = 0);
   static void *newArray_sPHElectronPairContainerv1(Long_t size, void *p);
   static void delete_sPHElectronPairContainerv1(void *p);
   static void deleteArray_sPHElectronPairContainerv1(void *p);
   static void destruct_sPHElectronPairContainerv1(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::sPHElectronPairContainerv1*)
   {
      ::sPHElectronPairContainerv1 *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::sPHElectronPairContainerv1 >(0);
      static ::ROOT::TGenericClassInfo 
         instance("sPHElectronPairContainerv1", ::sPHElectronPairContainerv1::Class_Version(), "", 13,
                  typeid(::sPHElectronPairContainerv1), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::sPHElectronPairContainerv1::Dictionary, isa_proxy, 4,
                  sizeof(::sPHElectronPairContainerv1) );
      instance.SetNew(&new_sPHElectronPairContainerv1);
      instance.SetNewArray(&newArray_sPHElectronPairContainerv1);
      instance.SetDelete(&delete_sPHElectronPairContainerv1);
      instance.SetDeleteArray(&deleteArray_sPHElectronPairContainerv1);
      instance.SetDestructor(&destruct_sPHElectronPairContainerv1);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::sPHElectronPairContainerv1*)
   {
      return GenerateInitInstanceLocal((::sPHElectronPairContainerv1*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::sPHElectronPairContainerv1*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr sPHElectronPairContainerv1::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *sPHElectronPairContainerv1::Class_Name()
{
   return "sPHElectronPairContainerv1";
}

//______________________________________________________________________________
const char *sPHElectronPairContainerv1::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::sPHElectronPairContainerv1*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int sPHElectronPairContainerv1::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::sPHElectronPairContainerv1*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *sPHElectronPairContainerv1::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::sPHElectronPairContainerv1*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *sPHElectronPairContainerv1::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::sPHElectronPairContainerv1*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void sPHElectronPairContainerv1::Streamer(TBuffer &R__b)
{
   // Stream an object of class sPHElectronPairContainerv1.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(sPHElectronPairContainerv1::Class(),this);
   } else {
      R__b.WriteClassBuffer(sPHElectronPairContainerv1::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_sPHElectronPairContainerv1(void *p) {
      return  p ? new(p) ::sPHElectronPairContainerv1 : new ::sPHElectronPairContainerv1;
   }
   static void *newArray_sPHElectronPairContainerv1(Long_t nElements, void *p) {
      return p ? new(p) ::sPHElectronPairContainerv1[nElements] : new ::sPHElectronPairContainerv1[nElements];
   }
   // Wrapper around operator delete
   static void delete_sPHElectronPairContainerv1(void *p) {
      delete ((::sPHElectronPairContainerv1*)p);
   }
   static void deleteArray_sPHElectronPairContainerv1(void *p) {
      delete [] ((::sPHElectronPairContainerv1*)p);
   }
   static void destruct_sPHElectronPairContainerv1(void *p) {
      typedef ::sPHElectronPairContainerv1 current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::sPHElectronPairContainerv1

namespace ROOT {
   static TClass *maplEunsignedsPintcOsPHElectronPairv1mUgR_Dictionary();
   static void maplEunsignedsPintcOsPHElectronPairv1mUgR_TClassManip(TClass*);
   static void *new_maplEunsignedsPintcOsPHElectronPairv1mUgR(void *p = 0);
   static void *newArray_maplEunsignedsPintcOsPHElectronPairv1mUgR(Long_t size, void *p);
   static void delete_maplEunsignedsPintcOsPHElectronPairv1mUgR(void *p);
   static void deleteArray_maplEunsignedsPintcOsPHElectronPairv1mUgR(void *p);
   static void destruct_maplEunsignedsPintcOsPHElectronPairv1mUgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const map<unsigned int,sPHElectronPairv1*>*)
   {
      map<unsigned int,sPHElectronPairv1*> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(map<unsigned int,sPHElectronPairv1*>));
      static ::ROOT::TGenericClassInfo 
         instance("map<unsigned int,sPHElectronPairv1*>", -2, "map", 100,
                  typeid(map<unsigned int,sPHElectronPairv1*>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &maplEunsignedsPintcOsPHElectronPairv1mUgR_Dictionary, isa_proxy, 0,
                  sizeof(map<unsigned int,sPHElectronPairv1*>) );
      instance.SetNew(&new_maplEunsignedsPintcOsPHElectronPairv1mUgR);
      instance.SetNewArray(&newArray_maplEunsignedsPintcOsPHElectronPairv1mUgR);
      instance.SetDelete(&delete_maplEunsignedsPintcOsPHElectronPairv1mUgR);
      instance.SetDeleteArray(&deleteArray_maplEunsignedsPintcOsPHElectronPairv1mUgR);
      instance.SetDestructor(&destruct_maplEunsignedsPintcOsPHElectronPairv1mUgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::MapInsert< map<unsigned int,sPHElectronPairv1*> >()));

      ::ROOT::AddClassAlternate("map<unsigned int,sPHElectronPairv1*>","std::map<unsigned int, sPHElectronPairv1*, std::less<unsigned int>, std::allocator<std::pair<unsigned int const, sPHElectronPairv1*> > >");
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const map<unsigned int,sPHElectronPairv1*>*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *maplEunsignedsPintcOsPHElectronPairv1mUgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const map<unsigned int,sPHElectronPairv1*>*)0x0)->GetClass();
      maplEunsignedsPintcOsPHElectronPairv1mUgR_TClassManip(theClass);
   return theClass;
   }

   static void maplEunsignedsPintcOsPHElectronPairv1mUgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_maplEunsignedsPintcOsPHElectronPairv1mUgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) map<unsigned int,sPHElectronPairv1*> : new map<unsigned int,sPHElectronPairv1*>;
   }
   static void *newArray_maplEunsignedsPintcOsPHElectronPairv1mUgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) map<unsigned int,sPHElectronPairv1*>[nElements] : new map<unsigned int,sPHElectronPairv1*>[nElements];
   }
   // Wrapper around operator delete
   static void delete_maplEunsignedsPintcOsPHElectronPairv1mUgR(void *p) {
      delete ((map<unsigned int,sPHElectronPairv1*>*)p);
   }
   static void deleteArray_maplEunsignedsPintcOsPHElectronPairv1mUgR(void *p) {
      delete [] ((map<unsigned int,sPHElectronPairv1*>*)p);
   }
   static void destruct_maplEunsignedsPintcOsPHElectronPairv1mUgR(void *p) {
      typedef map<unsigned int,sPHElectronPairv1*> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class map<unsigned int,sPHElectronPairv1*>

namespace {
  void TriggerDictionaryInitialization_sPHElectronPairContainerv1_Dict_Impl() {
    static const char* headers[] = {
"0",
0
    };
    static const char* includePaths[] = {
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "sPHElectronPairContainerv1_Dict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class sPHElectronPairContainerv1;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "sPHElectronPairContainerv1_Dict dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
#ifndef SPHELECTRONPAIRCONTAINER_V1_H
#define SPHELECTRONPAIRCONTAINER_V1_H

#include "sPHElectronPair.h"
#include "sPHElectronPairv1.h"
#include "sPHElectronPairContainer.h"

#include <cstddef>        // for size_t
#include <iostream>        // for cout, ostream

class sPHElectronPairContainerv1 : public sPHElectronPairContainer
{
public:

  typedef std::map<unsigned int, sPHElectronPairv1*> PairMap;
  typedef std::map<unsigned int, sPHElectronPairv1*>::const_iterator ConstIter;
  typedef std::map<unsigned int, sPHElectronPairv1*>::iterator Iter;

  sPHElectronPairContainerv1();
  sPHElectronPairContainerv1(const sPHElectronPairContainerv1& container);
  sPHElectronPairContainerv1& operator=(const sPHElectronPairContainerv1& container);

  virtual ~sPHElectronPairContainerv1();

  void identify(std::ostream& os = std::cout) const;
  void Reset();
  void clear() { Reset(); }
  int isValid() const { return 1; }
  size_t size() const { return _map.size(); }
  bool empty() const { return _map.empty(); }
  PHObject* CloneMe() const { return new sPHElectronPairContainerv1(*this); }

  const sPHElectronPairv1* get(unsigned int idkey) const;
  sPHElectronPairv1* get(unsigned int idkey);
  void insert(const sPHElectronPairv1* pair);
  size_t erase(unsigned int idkey)
  {
    delete _map[idkey];
    return _map.erase(idkey);
  }

  ConstIter begin() const { return _map.begin(); }
  ConstIter find(unsigned int idkey) const { return _map.find(idkey); }
  ConstIter end() const { return _map.end(); }

  Iter begin() { return _map.begin(); }
  Iter find(unsigned int idkey) { return _map.find(idkey); }
  Iter end() { return _map.end(); }

protected:
  PairMap _map;
  ClassDef(sPHElectronPairContainerv1, 1);

};

#endif


#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"sPHElectronPairContainerv1", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("sPHElectronPairContainerv1_Dict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_sPHElectronPairContainerv1_Dict_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_sPHElectronPairContainerv1_Dict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_sPHElectronPairContainerv1_Dict() {
  TriggerDictionaryInitialization_sPHElectronPairContainerv1_Dict_Impl();
}
