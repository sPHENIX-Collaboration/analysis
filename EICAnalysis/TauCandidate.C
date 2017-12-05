#include "TauCandidate.h"

//#include <cstdlib>

#include <g4jets/JetMap.h>

using namespace std;

/* Constructor */
TauCandidate::TauCandidate( Jet* jet ) :
  PHObject(),
  _jet_id( jet->get_id() ),
  _jet_eta( jet->get_eta() ),
  _jet_phi( jet->get_phi() ),
  _jet_etotal( jet->get_e() ),
  _jet_etrans( jet->get_et() ),
  _jet_ptotal( jet->get_p() ),
  _jet_ptrans( jet->get_pt() ),
  _jet_mass( jet->get_mass() ),
  _is_tau( false ),
  _is_uds( false ),
  _tracks_count(0),
  _tracks_chargesum(0),
  _tracks_rmax(0)
{

}

void
TauCandidate::identify(ostream& os) const
{
  cout << "Class " << this->ClassName() << endl;
  cout << "jet_id: " << get_jet_id()
       << ", jet_eta: " << get_jet_eta()
       << ", jet_phi: " << get_jet_phi()
       << ", jet_etotal: " << get_jet_etotal()
       << ", tracks_count: " << get_tracks_count()
       << ", is_tau: " << get_is_tau() << endl;
  return;
}
