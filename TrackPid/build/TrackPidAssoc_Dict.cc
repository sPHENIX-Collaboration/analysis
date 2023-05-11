// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME TrackPidAssoc_Dict
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
#include "/sphenix/u/weihuma/analysis/TrackPid/src/TrackPidAssoc.h"

// Header files passed via #pragma extra_include

namespace ROOT {
   static void *new_TrackPidAssoc(void *p = 0);
   static void *newArray_TrackPidAssoc(Long_t size, void *p);
   static void delete_TrackPidAssoc(void *p);
   static void deleteArray_TrackPidAssoc(void *p);
   static void destruct_TrackPidAssoc(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::TrackPidAssoc*)
   {
      ::TrackPidAssoc *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::TrackPidAssoc >(0);
      static ::ROOT::TGenericClassInfo 
         instance("TrackPidAssoc", ::TrackPidAssoc::Class_Version(), "", 23,
                  typeid(::TrackPidAssoc), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::TrackPidAssoc::Dictionary, isa_proxy, 4,
                  sizeof(::TrackPidAssoc) );
      instance.SetNew(&new_TrackPidAssoc);
      instance.SetNewArray(&newArray_TrackPidAssoc);
      instance.SetDelete(&delete_TrackPidAssoc);
      instance.SetDeleteArray(&deleteArray_TrackPidAssoc);
      instance.SetDestructor(&destruct_TrackPidAssoc);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::TrackPidAssoc*)
   {
      return GenerateInitInstanceLocal((::TrackPidAssoc*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::TrackPidAssoc*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr TrackPidAssoc::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *TrackPidAssoc::Class_Name()
{
   return "TrackPidAssoc";
}

//______________________________________________________________________________
const char *TrackPidAssoc::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::TrackPidAssoc*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int TrackPidAssoc::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::TrackPidAssoc*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *TrackPidAssoc::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::TrackPidAssoc*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *TrackPidAssoc::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::TrackPidAssoc*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void TrackPidAssoc::Streamer(TBuffer &R__b)
{
   // Stream an object of class TrackPidAssoc.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(TrackPidAssoc::Class(),this);
   } else {
      R__b.WriteClassBuffer(TrackPidAssoc::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_TrackPidAssoc(void *p) {
      return  p ? new(p) ::TrackPidAssoc : new ::TrackPidAssoc;
   }
   static void *newArray_TrackPidAssoc(Long_t nElements, void *p) {
      return p ? new(p) ::TrackPidAssoc[nElements] : new ::TrackPidAssoc[nElements];
   }
   // Wrapper around operator delete
   static void delete_TrackPidAssoc(void *p) {
      delete ((::TrackPidAssoc*)p);
   }
   static void deleteArray_TrackPidAssoc(void *p) {
      delete [] ((::TrackPidAssoc*)p);
   }
   static void destruct_TrackPidAssoc(void *p) {
      typedef ::TrackPidAssoc current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::TrackPidAssoc

namespace ROOT {
   static TClass *multimaplEunsignedsPintcOunsignedsPintgR_Dictionary();
   static void multimaplEunsignedsPintcOunsignedsPintgR_TClassManip(TClass*);
   static void *new_multimaplEunsignedsPintcOunsignedsPintgR(void *p = 0);
   static void *newArray_multimaplEunsignedsPintcOunsignedsPintgR(Long_t size, void *p);
   static void delete_multimaplEunsignedsPintcOunsignedsPintgR(void *p);
   static void deleteArray_multimaplEunsignedsPintcOunsignedsPintgR(void *p);
   static void destruct_multimaplEunsignedsPintcOunsignedsPintgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const multimap<unsigned int,unsigned int>*)
   {
      multimap<unsigned int,unsigned int> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(multimap<unsigned int,unsigned int>));
      static ::ROOT::TGenericClassInfo 
         instance("multimap<unsigned int,unsigned int>", -2, "map", 99,
                  typeid(multimap<unsigned int,unsigned int>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &multimaplEunsignedsPintcOunsignedsPintgR_Dictionary, isa_proxy, 0,
                  sizeof(multimap<unsigned int,unsigned int>) );
      instance.SetNew(&new_multimaplEunsignedsPintcOunsignedsPintgR);
      instance.SetNewArray(&newArray_multimaplEunsignedsPintcOunsignedsPintgR);
      instance.SetDelete(&delete_multimaplEunsignedsPintcOunsignedsPintgR);
      instance.SetDeleteArray(&deleteArray_multimaplEunsignedsPintcOunsignedsPintgR);
      instance.SetDestructor(&destruct_multimaplEunsignedsPintcOunsignedsPintgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::MapInsert< multimap<unsigned int,unsigned int> >()));

      ::ROOT::AddClassAlternate("multimap<unsigned int,unsigned int>","std::multimap<unsigned int, unsigned int, std::less<unsigned int>, std::allocator<std::pair<unsigned int const, unsigned int> > >");
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const multimap<unsigned int,unsigned int>*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *multimaplEunsignedsPintcOunsignedsPintgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const multimap<unsigned int,unsigned int>*)0x0)->GetClass();
      multimaplEunsignedsPintcOunsignedsPintgR_TClassManip(theClass);
   return theClass;
   }

   static void multimaplEunsignedsPintcOunsignedsPintgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_multimaplEunsignedsPintcOunsignedsPintgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) multimap<unsigned int,unsigned int> : new multimap<unsigned int,unsigned int>;
   }
   static void *newArray_multimaplEunsignedsPintcOunsignedsPintgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) multimap<unsigned int,unsigned int>[nElements] : new multimap<unsigned int,unsigned int>[nElements];
   }
   // Wrapper around operator delete
   static void delete_multimaplEunsignedsPintcOunsignedsPintgR(void *p) {
      delete ((multimap<unsigned int,unsigned int>*)p);
   }
   static void deleteArray_multimaplEunsignedsPintcOunsignedsPintgR(void *p) {
      delete [] ((multimap<unsigned int,unsigned int>*)p);
   }
   static void destruct_multimaplEunsignedsPintcOunsignedsPintgR(void *p) {
      typedef multimap<unsigned int,unsigned int> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class multimap<unsigned int,unsigned int>

namespace {
  void TriggerDictionaryInitialization_TrackPidAssoc_Dict_Impl() {
    static const char* headers[] = {
"0",
0
    };
    static const char* includePaths[] = {
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "TrackPidAssoc_Dict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class TrackPidAssoc;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "TrackPidAssoc_Dict dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
/**
 * @file trackbase/TrkrClusterHitAssoc.h
 * @author D. McGlinchey
 * @date June 2018
 * @brief Class for associating clusters to the hits that went into them
 */
#ifndef TRACKPID_TRACKPIDASSOC_H
#define TRACKPID_TRACKPIDASSOC_H

#include <phool/PHObject.h>

#include <iostream>          // for cout, ostream
#include <map>
#include <utility>           // for pair

/**
 * @brief Class for associating particle ID categories to tracks 
 *
 * Store the associations between particle ID categories and tracks 
 */
class TrackPidAssoc : public PHObject
{
public:
  //! typedefs for convenience
  typedef std::multimap<unsigned int, unsigned int> MMap;
  typedef MMap::iterator Iterator;
  typedef MMap::const_iterator ConstIterator;
  typedef std::pair<Iterator, Iterator> Range;
  typedef std::pair<ConstIterator, ConstIterator> ConstRange;
  //! ctor
  TrackPidAssoc();
  //! dtor
  virtual ~TrackPidAssoc();

  void Reset();

  void identify(std::ostream &os = std::cout) const;

  /**
   * @brief Add association between particle ID and track
   * @param[in] PID index
   * @param[in] Index of the track
   */
  void addAssoc(unsigned int pid, unsigned int trid);

  /**
   * @brief Get all the tracks associated with PID index
   * @param[in] pid particle id index
   * @param[out] Range of tracks associated with pid
   */
  ConstRange getTracks(unsigned int pid);

  static const unsigned int electron = 1;
  static const unsigned int hadron = 2;


private:
  MMap m_map;
  ClassDef(TrackPidAssoc, 1);
};

#endif // TRACKPID_TRACKPIDASSOC_H

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"TrackPidAssoc", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("TrackPidAssoc_Dict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_TrackPidAssoc_Dict_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_TrackPidAssoc_Dict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_TrackPidAssoc_Dict() {
  TriggerDictionaryInitialization_TrackPidAssoc_Dict_Impl();
}
