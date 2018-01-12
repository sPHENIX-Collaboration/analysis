#ifndef TAUCANDIDATE_H__
#define TAUCANDIDATE_H__

#include <phool/PHObject.h>
#include <cmath>
#include <climits>

class TauCandidate: public PHObject
{
public:
  TauCandidate() {}
  virtual ~TauCandidate() {}

  virtual void identify(std::ostream& os = std::cout) const;

  virtual void Copy(TauCandidate const &tc);

  friend std::ostream &operator<<(std::ostream & stream, const TauCandidate * tc);

  virtual void Reset();

  virtual void print() const {std::cout<<"TauCandidate base class - print() not implemented"<<std::endl;}

  //! Procedure to add a new PROPERTY tag:
  //! 1.add new tag below with unique value,
  //! 2.add a short name to TauCandidate::get_property_info
  enum PROPERTY
    {//

      //-- Truth properties: 1 - 100  --
      evtgen_is_tau = 1,
      evtgen_tau_etotal = 2,
      evtgen_tau_eta = 3,
      evtgen_tau_phi = 4,
      evtgen_tau_decay_prong = 5,
      evtgen_tau_decay_hcharged = 6,
      evtgen_tau_decay_lcharged = 7,

      evtgen_is_uds = 11,
      evtgen_uds_etotal = 12,
      evtgen_uds_eta = 13,
      evtgen_uds_phi = 14,

      evtgen_is_ele = 21,
      evtgen_ele_etotal = 22,
      evtgen_ele_eta = 23,
      evtgen_ele_phi = 24,

      //-- Jet properties: 101 - ?  --
      jet_id = 101,
      jet_eta = 102,
      jet_phi = 103,
      jet_etotal = 104,
      jet_etrans = 105,
      jet_ptotal = 106,
      jet_ptrans = 107,
      jet_minv   = 108,
      jet_mtrans = 109,
      jet_ncomp  = 110,
      jet_ncomp_above_0p1  = 111,
      jet_ncomp_above_1    = 112,
      jet_ncomp_above_10   = 113,
      jet_ncomp_emcal      = 115,

      //-- Jet shape in calorimeter: 120 - 200  --
      jetshape_radius = 121,
      jetshape_rms = 122,
      jetshape_r90 = 123,

      jetshape_econe_r01 = 131,
      jetshape_econe_r02 = 132,
      jetshape_econe_r03 = 133,
      jetshape_econe_r04 = 134,
      jetshape_econe_r05 = 135,

      jetshape_emcal_radius = 161,
      jetshape_emcal_rms = 162,
      jetshape_emcal_r90 = 163,

      jetshape_emcal_econe_r01 = 171,
      jetshape_emcal_econe_r02 = 172,
      jetshape_emcal_econe_r03 = 173,
      jetshape_emcal_econe_r04 = 174,
      jetshape_emcal_econe_r05 = 175,

      //-- Track information: 201 - 250  --
      tracks_count_r02 = 210,
      tracks_count_r04 = 211,

      tracks_rmax_r02 = 215,
      tracks_rmax_r04 = 216,

      tracks_chargesum_r02 = 220,
      tracks_chargesum_r04 = 221,

      //-- 1-track properties: 251 - 300  --
      track_id = 251,
      track_quality = 252,
      track_eta = 253,
      track_phi = 254,
      track_ptotal = 255,
      track_ptrans = 256,
      track_charge = 257,
      track_e3x3_cemc = 260,
      track_e3x3_femc = 261,
      track_e3x3_eemc = 262,
      track_e3x3_ihcal = 263,
      track_e3x3_ohcal = 264,
      track_e3x3_fhcal = 265,
      track_e3x3_ehcal = 266,

      //! max limit in order to fit into 8 bit unsigned number
      prop_MAX_NUMBER = UCHAR_MAX
    };

  enum PROPERTY_TYPE
    {//
      type_int = 1,
      type_uint = 2,
      type_float = 3,
      type_unknown = -1
    };

  virtual bool  has_property(const PROPERTY prop_id) const {return false;}
  virtual float get_property_float(const PROPERTY prop_id) const {return NAN;}
  virtual int   get_property_int(const PROPERTY prop_id) const {return INT_MIN;}
  virtual unsigned int   get_property_uint(const PROPERTY prop_id) const {return UINT_MAX;}
  virtual void  set_property(const PROPERTY prop_id, const float value) {return;}
  virtual void  set_property(const PROPERTY prop_id, const int value) {return;}
  virtual void  set_property(const PROPERTY prop_id, const unsigned int value) {return;}
  static std::pair<const std::string,PROPERTY_TYPE> get_property_info(PROPERTY prop_id);
  static bool check_property(const PROPERTY prop_id, const PROPERTY_TYPE prop_type);
  static std::string get_property_type(const PROPERTY_TYPE prop_type);

protected:
  virtual unsigned int get_property_nocheck(const PROPERTY prop_id) const {return UINT_MAX;}
  virtual void set_property_nocheck(const PROPERTY prop_id,const unsigned int) {return;}
  //ClassDef(TauCandidate,1)
};

#endif
