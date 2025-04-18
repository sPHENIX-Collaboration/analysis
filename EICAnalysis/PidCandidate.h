#ifndef PIDCANDIDATE_H__
#define PIDCANDIDATE_H__

#include <phool/PHObject.h>
#include <cmath>
#include <climits>

class PidCandidate: public PHObject
{
public:
  PidCandidate() {}
  virtual ~PidCandidate() {}

  virtual void identify(std::ostream& os = std::cout) const;

  virtual void Copy(PidCandidate const &tc);

  friend std::ostream &operator<<(std::ostream & stream, const PidCandidate * tc);

  virtual void Reset();

  virtual void print() const {std::cout<<"PidCandidate base class - print() not implemented"<<std::endl;}

  //! Procedure to add a new PROPERTY tag:
  //! 1.add new tag below with unique value,
  //! 2.add a short name to PidCandidate::get_property_info
  enum PROPERTY
    {//

      //-- Truth properties: 1 - 100  --
      evtgen_pid = 1,
      evtgen_ptotal = 2,
      evtgen_etotal = 3,
      evtgen_theta = 4,
      evtgen_phi = 5,
      evtgen_eta = 6,
      evtgen_charge = 7,
      evtgen_decay_prong = 8,
      evtgen_decay_hcharged = 9,
      evtgen_decay_lcharged = 10,

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
      tracks_count_R = 212,
      
      tracks_rmax_r02 = 215,
      tracks_rmax_r04 = 216,
      tracks_rmax_R = 217,

      tracks_chargesum_r02 = 220,
      tracks_chargesum_r04 = 221,
      tracks_chargesum_R = 222,
      tracks_vertex = 223,

      //-- EM Candidates Cluster
      em_cluster_id = 301,
      em_cluster_prob = 302,
      em_cluster_posx = 303,
      em_cluster_posy = 304,
      em_cluster_posz = 305,
      em_cluster_e = 306,
      em_cluster_ecore = 307,
      em_cluster_et_iso = 308,
      em_cluster_theta = 309,
      em_cluster_phi = 310,
      em_cluster_eta = 311,
      em_cluster_pt = 312,
      em_cluster_ntower = 313,
      em_cluster_caloid = 314,

      //-- EM Candidates Tracks
      em_track_id = 350,
      em_track_quality = 351,
      em_track_theta = 352,
      em_track_phi = 353,
      em_track_eta = 354,
      em_track_ptotal = 355,
      em_track_ptrans = 356,
      em_track_charge = 357,
      em_track_dca = 358,
      em_track_section = 359,
      em_track_e3x3_cemc = 360,
      em_track_e3x3_femc = 361,
      em_track_e3x3_eemc = 362,
      em_track_e3x3_ihcal = 363,
      em_track_e3x3_ohcal = 364,
      em_track_e3x3_fhcal = 365,
      em_track_e3x3_ehcal = 366,
      em_track_cluster_dr = 367,
      em_track_theta2cluster = 368,
      em_track_eta2cluster = 369,
      em_track_phi2cluster = 370,
      em_track_p2cluster = 371,
      em_track_x2cluster = 372,
      em_track_y2cluster = 373,
      em_track_z2cluster = 374,

      //-- EM PID probabilities
      em_pid_prob_electron = 391,
      em_pid_prob_pion = 392,
      em_pid_prob_kaon = 393,
      em_pid_prob_proton = 394,

      //-- EM Candidates Truth
      em_evtgen_pid = 401,
      em_evtgen_ptotal = 402,
      em_evtgen_etotal = 403,
      em_evtgen_theta = 404,
      em_evtgen_phi = 405,
      em_evtgen_eta = 406,
      em_evtgen_charge = 407,
      em_evtgen_is_scattered_lepton = 408,

      //-- Event kinematics reconstructed from candidate
      em_reco_x_e = 410,
      em_reco_y_e = 411,
      em_reco_q2_e = 412,
      em_reco_w_e = 413,

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

  ClassDef(PidCandidate,1)
};

#endif
