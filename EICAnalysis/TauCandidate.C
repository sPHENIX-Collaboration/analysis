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
    case  evtgen_pid:
      return make_pair("evtgen_pid",TauCandidate::type_int);

    case  evtgen_ptotal:
      return make_pair("evtgen_ptotal",TauCandidate::type_float);

    case  evtgen_etotal:
      return make_pair("evtgen_etotal",TauCandidate::type_float);

    case  evtgen_theta:
      return make_pair("evtgen_theta",TauCandidate::type_float);

    case  evtgen_phi:
      return make_pair("evtgen_phi",TauCandidate::type_float);

    case  evtgen_eta:
      return make_pair("evtgen_eta",TauCandidate::type_float);

    case  evtgen_charge:
      return make_pair("evtgen_charge",TauCandidate::type_int);

    case  evtgen_decay_prong:
      return make_pair("evtgen_decay_prong",TauCandidate::type_uint);

    case  evtgen_decay_hcharged:
      return make_pair("evtgen_decay_hcharged",TauCandidate::type_uint);

    case  evtgen_decay_lcharged:
      return make_pair("evtgen_decay_lcharged",TauCandidate::type_uint);

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

    case  tracks_count_r02:
      return make_pair("tracks_count_r02",TauCandidate::type_uint);

    case  tracks_count_r04:
      return make_pair("tracks_count_r04",TauCandidate::type_uint);

    case  tracks_count_R:
      return make_pair("tracks_count_R",TauCandidate::type_uint);

    case  tracks_rmax_r02:
      return make_pair("tracks_rmax_r02",TauCandidate::type_float);

    case  tracks_rmax_r04:
      return make_pair("tracks_rmax_r04",TauCandidate::type_float);

    case  tracks_rmax_R:
      return make_pair("tracks_rmax_R",TauCandidate::type_float);

    case  tracks_chargesum_r02:
      return make_pair("tracks_chargesum_r02",TauCandidate::type_int);

    case  tracks_chargesum_r04:
      return make_pair("tracks_chargesum_r04",TauCandidate::type_int);

    case  tracks_chargesum_R:
      return make_pair("tracks_chargesum_R",TauCandidate::type_int);

      // ------

      // EM Candidates:

    case  em_cluster_id:
      return make_pair("em_cluster_id",TauCandidate::type_uint);

    case  em_cluster_prob:
      return make_pair("em_cluster_prob",TauCandidate::type_float);

    case  em_cluster_posx:
      return make_pair("em_cluster_posx",TauCandidate::type_float);

    case  em_cluster_posy:
      return make_pair("em_cluster_posy",TauCandidate::type_float);

    case  em_cluster_posz:
      return make_pair("em_cluster_posz",TauCandidate::type_float);

    case  em_cluster_e:
      return make_pair("em_cluster_e",TauCandidate::type_float);

    case  em_cluster_ecore:
      return make_pair("em_cluster_ecore",TauCandidate::type_float);

    case  em_cluster_et_iso:
      return make_pair("em_cluster_et_iso",TauCandidate::type_float);

    case  em_cluster_theta:
      return make_pair("em_cluster_theta",TauCandidate::type_float);

    case  em_cluster_phi:
      return make_pair("em_cluster_phi",TauCandidate::type_float);

    case  em_cluster_pt:
      return make_pair("em_cluster_pt",TauCandidate::type_float);

    case  em_cluster_ntower:
      return make_pair("em_cluster_ntower",TauCandidate::type_uint);

    case  em_cluster_caloid:
      return make_pair("em_cluster_caloid",TauCandidate::type_uint);

      // ------

    case  em_track_id:
      return make_pair("em_track_id",TauCandidate::type_uint);

    case  em_track_quality:
      return make_pair("em_track_quality",TauCandidate::type_float);

    case  em_track_theta:
      return make_pair("em_track_theta",TauCandidate::type_float);

    case  em_track_phi:
      return make_pair("em_track_phi",TauCandidate::type_float);

    case  em_track_ptotal:
      return make_pair("em_track_ptotal",TauCandidate::type_float);

    case  em_track_ptrans:
      return make_pair("em_track_ptrans",TauCandidate::type_float);

    case  em_track_charge:
      return make_pair("em_track_charge",TauCandidate::type_int);

    case  em_track_dca:
      return make_pair("em_track_dca",TauCandidate::type_float);

    case  em_track_section:
      return make_pair("em_track_section",TauCandidate::type_uint);

    case  em_track_e3x3_cemc:
      return make_pair("em_track_e3x3_cemc",TauCandidate::type_float);

    case  em_track_e3x3_femc:
      return make_pair("em_track_e3x3_femc",TauCandidate::type_float);

    case  em_track_e3x3_eemc:
      return make_pair("em_track_e3x3_eemc",TauCandidate::type_float);

    case  em_track_e3x3_ihcal:
      return make_pair("em_track_e3x3_ihcal",TauCandidate::type_float);

    case  em_track_e3x3_ohcal:
      return make_pair("em_track_e3x3_ohcal",TauCandidate::type_float);

    case  em_track_e3x3_fhcal:
      return make_pair("em_track_e3x3_fhcal",TauCandidate::type_float);

    case  em_track_e3x3_ehcal:
      return make_pair("em_track_e3x3_ehcal",TauCandidate::type_float);

    case  em_track_cluster_dr:
      return make_pair("em_track_cluster_dr",TauCandidate::type_float);

      // ------

    case  em_evtgen_pid:
      return make_pair("em_evtgen_pid",TauCandidate::type_int);

    case  em_evtgen_ptotal:
      return make_pair("em_evtgen_ptotal",TauCandidate::type_float);

    case  em_evtgen_etotal:
      return make_pair("em_evtgen_etotal",TauCandidate::type_float);

    case  em_evtgen_theta:
      return make_pair("em_evtgen_theta",TauCandidate::type_float);

    case  em_evtgen_phi:
      return make_pair("em_evtgen_phi",TauCandidate::type_float);

    case  em_evtgen_eta:
      return make_pair("em_evtgen_eta",TauCandidate::type_float);

    case  em_evtgen_charge:
      return make_pair("em_evtgen_charge",TauCandidate::type_int);

      // ------

    case  em_reco_x_e:
      return make_pair("em_reco_x_e",TauCandidate::type_float);

    case  em_reco_y_e:
      return make_pair("em_reco_y_e",TauCandidate::type_float);

    case  em_reco_q2_e:
      return make_pair("em_reco_q2_e",TauCandidate::type_float);

    case  em_reco_w_e:
      return make_pair("em_reco_w_e",TauCandidate::type_float);

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
