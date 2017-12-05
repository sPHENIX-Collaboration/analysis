#ifndef TAUCANDIDATE_H__
#define TAUCANDIDATE_H__

#include <phool/PHObject.h>

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
  void set_is_tau( bool id ) { _is_tau = id; }
  void set_is_uds( bool id ) { _is_uds = id; }

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

  bool get_is_tau() const { return _is_tau; }
  bool get_is_uds() const { return _is_uds; }

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

  /** Does this jet come from a 'truth' tau from hard scattering? */
  bool _is_tau;

  /** Does this jet come from a 'truth' u-, d-, or s-quark from hard scattering? */
  bool _is_uds;

  /** Number of tracks within cone around jet axis */
  int _tracks_count;

  /** Sum of charges of tracks within cone around jet axis */
  int _tracks_chargesum;

  /** Track with maximum angle w.r.t. jet axis but still within search cone around jet axis */
  int _tracks_rmax;

};

#endif
