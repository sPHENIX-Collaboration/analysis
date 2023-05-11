#ifndef PIDCANDIDATEv1_H__
#define PIDCANDIDATEv1_H__

#include "PidCandidate.h"

#ifdef __CINT__
#include <stdint.h>
#else
#include <cstdint>
#endif
#include <iostream>
#include <map>

class PidCandidatev1 : public PidCandidate
{
public:
  PidCandidatev1();
  explicit PidCandidatev1(const PidCandidate &tc);
  virtual ~PidCandidatev1() {}

  void identify(std::ostream& os  = std::cout) const;

  void Reset();

  void set_candidate_id(const int i) { _candidateid = i; }
  int get_candidate_id() const { return _candidateid; }

  virtual void print() const;

  bool  has_property(const PROPERTY prop_id) const;
  float get_property_float(const PROPERTY prop_id) const;
  int   get_property_int(const PROPERTY prop_id) const;
  unsigned int   get_property_uint(const PROPERTY prop_id) const;
  void  set_property(const PROPERTY prop_id, const float value);
  void  set_property(const PROPERTY prop_id, const int value);
  void  set_property(const PROPERTY prop_id, const unsigned int value);

protected:
  unsigned int get_property_nocheck(const PROPERTY prop_id) const;
  void set_property_nocheck(const PROPERTY prop_id,const unsigned int ui) {prop_map[prop_id]=ui;}
  int _candidateid;

  //! storage types for additional property
  typedef uint8_t prop_id_t;
  typedef uint32_t prop_storage_t;
  typedef std::map<prop_id_t, prop_storage_t> prop_map_t;

  //! convert between 32bit inputs and storage type prop_storage_t
  union u_property{
    float fdata;
    int32_t idata;
    uint32_t uidata;

    u_property(int32_t in): idata(in) {}
    u_property(uint32_t in): uidata(in) {}
    u_property(float in): fdata(in) {}
    u_property(): uidata(0) {}

    prop_storage_t get_storage() const {return uidata;}
  };

  //! container for additional property
  prop_map_t prop_map;

  ClassDef(PidCandidatev1,2)
};

#endif
