#include "TauCandidate.h"

#include <cstdlib>

using namespace std;

ClassImp(TauCandidate)

void
TauCandidate::Copy(TauCandidate const &tc)
{
  for (unsigned char ic = 0; ic < UCHAR_MAX; ic++)
    {
      PROPERTY prop_id = static_cast<TauCandidate::PROPERTY> (ic);
      if (tc.has_property(prop_id))
        {
          set_property_nocheck(prop_id,tc.get_property_nocheck(prop_id));
        }
    }
}


void
TauCandidate::identify(ostream& os) const
{
  cout << "Class " << this->ClassName() << endl;
  return;
}

ostream& operator<<(ostream& stream, const TauCandidate * hit){
  stream <<endl<< "streaming information" <<endl;
  return stream;
}

void
TauCandidate::Reset()
{
  cout << "Reset not implemented by daughter class" << endl;
  return;
}


std::pair<const std::string,TauCandidate::PROPERTY_TYPE>
TauCandidate::get_property_info(const PROPERTY prop_id)
{
  switch (prop_id)
    {
    case  evtgen_is_tau:
      return make_pair("evtgen_is_tau",TauCandidate::type_uint);

    case  evtgen_tau_etotal:
      return make_pair("evtgen_tau_etotal",TauCandidate::type_float);

    case  evtgen_tau_eta:
      return make_pair("evtgen_tau_eta",TauCandidate::type_float);

    case  evtgen_tau_phi:
      return make_pair("evtgen_tau_phi",TauCandidate::type_float);

    case  evtgen_tau_decay_prong:
      return make_pair("evtgen_tau_decay_prong",TauCandidate::type_uint);

    case  evtgen_tau_decay_hcharged:
      return make_pair("evtgen_tau_decay_hcharged",TauCandidate::type_uint);

    case  evtgen_tau_decay_lcharged:
      return make_pair("evtgen_tau_decay_lcharged",TauCandidate::type_uint);

      // ------

    case  evtgen_is_uds:
      return make_pair("evtgen_is_uds",TauCandidate::type_uint);

    case  evtgen_uds_etotal:
      return make_pair("evtgen_uds_etotal",TauCandidate::type_float);

    case  evtgen_uds_eta:
      return make_pair("evtgen_uds_eta",TauCandidate::type_float);

    case  evtgen_uds_phi:
      return make_pair("evtgen_uds_phi",TauCandidate::type_float);

      // ------

    case  evtgen_pid:
      return make_pair("evtgen_pid",TauCandidate::type_int);

    case  evtgen_etotal:
      return make_pair("evtgen_etotal",TauCandidate::type_float);

    case  evtgen_eta:
      return make_pair("evtgen_eta",TauCandidate::type_float);

    case  evtgen_phi:
      return make_pair("evtgen_phi",TauCandidate::type_float);

      // ------

    case  jet_id:
      return make_pair("jet_id",TauCandidate::type_uint);

    case  jet_eta:
      return make_pair("jet_eta",TauCandidate::type_float);

    case  jet_phi:
      return make_pair("jet_phi",TauCandidate::type_float);

    case  jet_etotal:
      return make_pair("jet_etotal",TauCandidate::type_float);

    case  jet_etrans:
      return make_pair("jet_etrans",TauCandidate::type_float);

    case  jet_ptotal:
      return make_pair("jet_ptotal",TauCandidate::type_float);

    case  jet_ptrans:
      return make_pair("jet_ptrans",TauCandidate::type_float);

    case  jet_minv:
      return make_pair("jet_minv",TauCandidate::type_float);

    case  jet_mtrans:
      return make_pair("jet_mtrans",TauCandidate::type_float);

    case  jet_ncomp:
      return make_pair("jet_ncomp",TauCandidate::type_uint);

    case  jet_ncomp_above_0p1:
      return make_pair("jet_ncomp_above_0p1",TauCandidate::type_uint);

    case  jet_ncomp_above_1:
      return make_pair("jet_ncomp_above_1",TauCandidate::type_uint);

    case  jet_ncomp_above_10:
      return make_pair("jet_ncomp_above_10",TauCandidate::type_uint);

    case  jet_ncomp_emcal:
      return make_pair("jet_ncomp_emcal",TauCandidate::type_uint);

      // ------

    case  jetshape_radius:
      return make_pair("jetshape_radius",TauCandidate::type_float);

    case  jetshape_rms:
      return make_pair("jetshape_rms",TauCandidate::type_float);

    case  jetshape_r90:
      return make_pair("jetshape_r90",TauCandidate::type_float);

    case  jetshape_econe_r01:
      return make_pair("jetshape_econe_r01",TauCandidate::type_float);

    case  jetshape_econe_r02:
      return make_pair("jetshape_econe_r02",TauCandidate::type_float);

    case  jetshape_econe_r03:
      return make_pair("jetshape_econe_r03",TauCandidate::type_float);

    case  jetshape_econe_r04:
      return make_pair("jetshape_econe_r04",TauCandidate::type_float);

    case  jetshape_econe_r05:
      return make_pair("jetshape_econe_r05",TauCandidate::type_float);

      // ------

    case  jetshape_emcal_radius:
      return make_pair("jetshape_emcal_radius",TauCandidate::type_float);

    case  jetshape_emcal_rms:
      return make_pair("jetshape_emcal_rms",TauCandidate::type_float);

    case  jetshape_emcal_r90:
      return make_pair("jetshape_emcal_r90",TauCandidate::type_float);

    case  jetshape_emcal_econe_r01:
      return make_pair("jetshape_emcal_econe_r01",TauCandidate::type_float);

    case  jetshape_emcal_econe_r02:
      return make_pair("jetshape_emcal_econe_r02",TauCandidate::type_float);

    case  jetshape_emcal_econe_r03:
      return make_pair("jetshape_emcal_econe_r03",TauCandidate::type_float);

    case  jetshape_emcal_econe_r04:
      return make_pair("jetshape_emcal_econe_r04",TauCandidate::type_float);

    case  jetshape_emcal_econe_r05:
      return make_pair("jetshape_emcal_econe_r05",TauCandidate::type_float);

      // ------

    case  track_id:
      return make_pair("track_id",TauCandidate::type_uint);

    case  track_quality:
      return make_pair("track_quality",TauCandidate::type_float);

    case  track_eta:
      return make_pair("track_eta",TauCandidate::type_float);

    case  track_phi:
      return make_pair("track_phi",TauCandidate::type_float);

    case  track_ptotal:
      return make_pair("track_ptotal",TauCandidate::type_float);

    case  track_ptrans:
      return make_pair("track_ptrans",TauCandidate::type_float);

    case  track_charge:
      return make_pair("track_charge",TauCandidate::type_int);

    case  track_e3x3_cemc:
      return make_pair("track_e3x3_cemc",TauCandidate::type_float);

    case  track_e3x3_femc:
      return make_pair("track_e3x3_femc",TauCandidate::type_float);

    case  track_e3x3_eemc:
      return make_pair("track_e3x3_eemc",TauCandidate::type_float);

    case  track_e3x3_ihcal:
      return make_pair("track_e3x3_ihcal",TauCandidate::type_float);

    case  track_e3x3_ohcal:
      return make_pair("track_e3x3_ohcal",TauCandidate::type_float);

    case  track_e3x3_fhcal:
      return make_pair("track_e3x3_fhcal",TauCandidate::type_float);

    case  track_e3x3_ehcal:
      return make_pair("track_e3x3_ehcal",TauCandidate::type_float);

      // ------

    case  tracks_count_r02:
      return make_pair("tracks_count_r02",TauCandidate::type_uint);

    case  tracks_count_r04:
      return make_pair("tracks_count_r04",TauCandidate::type_uint);

    case  tracks_rmax_r02:
      return make_pair("tracks_rmax_r02",TauCandidate::type_float);

    case  tracks_rmax_r04:
      return make_pair("tracks_rmax_r04",TauCandidate::type_float);

    case  tracks_chargesum_r02:
      return make_pair("tracks_chargesum_r02",TauCandidate::type_int);

    case  tracks_chargesum_r04:
      return make_pair("tracks_chargesum_r04",TauCandidate::type_int);

      // ------

    case  cluster_id:
      return make_pair("cluster_id",TauCandidate::type_uint);

    case  cluster_energy:
      return make_pair("cluster_energy",TauCandidate::type_float);

    case  cluster_ecore:
      return make_pair("cluster_ecore",TauCandidate::type_float);

    case  cluster_et_iso:
      return make_pair("cluster_et_iso",TauCandidate::type_float);

    case  cluster_prob:
      return make_pair("cluster_prob",TauCandidate::type_float);

    case  cluster_eta:
      return make_pair("cluster_eta",TauCandidate::type_float);

    case  cluster_phi:
      return make_pair("cluster_phi",TauCandidate::type_float);

    case  cluster_ntower:
      return make_pair("cluster_ntower",TauCandidate::type_uint);

      // ------

    default:
      cout << "TauCandidate::get_property_info - Fatal Error - unknown index " << prop_id << endl;
      exit(1);
    }
}


bool
TauCandidate::check_property(const PROPERTY prop_id, const PROPERTY_TYPE prop_type)
{
  pair<const string,PROPERTY_TYPE> property_info = get_property_info(prop_id);
  if (property_info.second != prop_type)
    {
      return false;
    }
  return true;
}


string
TauCandidate::get_property_type(const PROPERTY_TYPE prop_type)
{
  switch(prop_type)
    {
    case type_int:
      return "int";
    case type_uint:
      return "unsigned int";
    case type_float:
      return "float";
    default:
      return "unkown";
    }
}
