#ifndef TAUCANDIDATE_H__
#define TAUCANDIDATE_H__

#include <phool/PHObject.h>

#include <map>

class Jet;

/**
 * This class combines information from a jet, the energy distributio nwithin that jet, and charged tracks to
 * collect properties that allow the identification of tau->hadrons decays. */
class TauCandidate: public PHObject
{
public:

  /** Default constructor. */
  TauCandidate( Jet* );

  /** Default destructor. */
  ~TauCandidate() {}

  /** Print some information about this tau candidate (for debugging purposes) */
  void identify(std::ostream& os = std::cout) const;

  /** Manually set properties of this tau candidate */
  void set_jetshape_econe( float r1, float er1 ) { _jetshape_econe.insert( std::make_pair( r1, er1 ) ); }
  void set_jetshape_r90( float r90 ) { _jetshape_r90 = r90; }
  void set_jetshape_rms( float rms ) { _jetshape_rms = rms; }
  void set_jetshape_radius( float radius ) { _jetshape_radius = radius; }

  void set_is_tau( bool id ) { _is_tau = id; }
  void set_is_uds( bool id ) { _is_uds = id; }

  void set_tau_etotal( float etotal ) { _tau_etotal = etotal; }
  void set_tau_eta( float eta ) { _tau_eta = eta; }
  void set_tau_phi( float phi ) { _tau_phi = phi; }

  void set_tau_decay_prong( int prong ) { _tau_decay_prong = prong; }
  void set_tau_decay_hcharged( int hcharged ) { _tau_decay_hcharged = hcharged; }
  void set_tau_decay_lcharged( int lcharged ) { _tau_decay_lcharged = lcharged; }

  void set_uds_etotal( float etotal ) { _uds_etotal = etotal; }
  void set_uds_eta( float eta ) { _uds_eta = eta; }
  void set_uds_phi( float phi ) { _uds_phi = phi; }

  void set_tracks_count( int ntracks ) { _tracks_count = ntracks; }
  void set_tracks_chargesum( int qtracks ) { _tracks_chargesum = qtracks; }
  void set_tracks_rmax( float rmax ) { _tracks_rmax = rmax; }

  /** Retrieve properties of this tau candidate */
  unsigned int get_jet_id() const { return _jet_id; }
  float get_jet_eta() const { return _jet_eta; }
  float get_jet_phi() const { return _jet_phi; }
  float get_jet_etotal() const { return _jet_etotal; }
  float get_jet_etrans() const { return _jet_etrans; }
  float get_jet_ptotal() const { return _jet_ptotal; }
  float get_jet_ptrans() const { return _jet_ptrans; }
  float get_jet_mass() const { return _jet_mass; }

  float get_jetshape_econe( float r1 ) const { return _jetshape_econe.find( r1 )->second; }
  float get_jetshape_r90() const { return _jetshape_r90; }
  float get_jetshape_rms() const { return _jetshape_rms; }
  float get_jetshape_radius() const { return _jetshape_radius; }

  bool get_is_tau() const { return _is_tau; }
  bool get_is_uds() const { return _is_uds; }

  float get_tau_etotal() const { return _tau_etotal; }
  float get_tau_eta() const { return _tau_eta; }
  float get_tau_phi() const { return _tau_phi; }
  int   get_tau_decay_prong() const { return _tau_decay_prong; }
  int   get_tau_decay_hcharged() const { return _tau_decay_hcharged; }
  int   get_tau_decay_lcharged() const { return _tau_decay_lcharged; }

  float get_uds_etotal() const { return _uds_etotal; }
  float get_uds_eta() const { return _uds_eta; }
  float get_uds_phi() const { return _uds_phi; }

  int get_tracks_count() const { return _tracks_count; }
  int get_tracks_chargesum() const { return _tracks_chargesum; }
  float get_tracks_rmax() const { return _tracks_rmax; }

protected:

  /**
   * Save properties of the jet that serves as basis for this tau candidate.
   * These are const to preserve the information of the jet that was initially considered tau candidate. */
  const unsigned int _jet_id;
  const float _jet_eta;
  const float _jet_phi;
  const float _jet_etotal;
  const float _jet_etrans;
  const float _jet_ptotal;
  const float _jet_ptrans;
  const float _jet_mass;

  /** Jet structure properties */
  std::map< float, float > _jetshape_econe;
  float _jetshape_r90;
  float _jetshape_rms;
  float _jetshape_radius;

  /** Does this jet come from a 'truth' tau from hard scattering? */
  bool _is_tau;

  /** Does this jet come from a 'truth' u-, d-, or s-quark from hard scattering? */
  bool _is_uds;

  /** Properites of event generator "true" tau */
  float _tau_etotal;
  float _tau_eta;
  float _tau_phi;

  int _tau_decay_prong;
  int _tau_decay_hcharged;
  int _tau_decay_lcharged;

  /** Properites of event generator "true" quark from hard scattering */
  float _uds_etotal;
  float _uds_eta;
  float _uds_phi;

  /** Number of tracks within cone around jet axis */
  int _tracks_count;

  /** Sum of charges of tracks within cone around jet axis */
  int _tracks_chargesum;

  /** Track with maximum angle w.r.t. jet axis but still within search cone around jet axis */
  float _tracks_rmax;

};

#endif
