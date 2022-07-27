#include "PidCandidate.h"

#include <cstdlib>

using namespace std;

ClassImp(PidCandidate)

void
PidCandidate::Copy(PidCandidate const &tc)
{
  for (unsigned char ic = 0; ic < UCHAR_MAX; ic++)
    {
      PROPERTY prop_id = static_cast<PidCandidate::PROPERTY> (ic);
      if (tc.has_property(prop_id))
        {
          set_property_nocheck(prop_id,tc.get_property_nocheck(prop_id));
        }
    }
}


void
PidCandidate::identify(ostream& os) const
{
  cout << "Class " << this->ClassName() << endl;
  return;
}

ostream& operator<<(ostream& stream, const PidCandidate * hit){
  stream <<endl<< "streaming information" <<endl;
  return stream;
}

void
PidCandidate::Reset()
{
  cout << "Reset not implemented by daughter class" << endl;
  return;
}


std::pair<const std::string,PidCandidate::PROPERTY_TYPE>
PidCandidate::get_property_info(const PROPERTY prop_id)
{
  switch (prop_id)
    {
    case  evtgen_pid:
      return make_pair("evtgen_pid",PidCandidate::type_int);

    case  evtgen_ptotal:
      return make_pair("evtgen_ptotal",PidCandidate::type_float);

    case  evtgen_etotal:
      return make_pair("evtgen_etotal",PidCandidate::type_float);

    case  evtgen_theta:
      return make_pair("evtgen_theta",PidCandidate::type_float);

    case  evtgen_phi:
      return make_pair("evtgen_phi",PidCandidate::type_float);

    case  evtgen_eta:
      return make_pair("evtgen_eta",PidCandidate::type_float);

    case  evtgen_charge:
      return make_pair("evtgen_charge",PidCandidate::type_int);

    case  evtgen_decay_prong:
      return make_pair("evtgen_decay_prong",PidCandidate::type_uint);

    case  evtgen_decay_hcharged:
      return make_pair("evtgen_decay_hcharged",PidCandidate::type_uint);

    case  evtgen_decay_lcharged:
      return make_pair("evtgen_decay_lcharged",PidCandidate::type_uint);

      // ------

    case  jet_id:
      return make_pair("jet_id",PidCandidate::type_uint);

    case  jet_eta:
      return make_pair("jet_eta",PidCandidate::type_float);

    case  jet_phi:
      return make_pair("jet_phi",PidCandidate::type_float);

    case  jet_etotal:
      return make_pair("jet_etotal",PidCandidate::type_float);

    case  jet_etrans:
      return make_pair("jet_etrans",PidCandidate::type_float);

    case  jet_ptotal:
      return make_pair("jet_ptotal",PidCandidate::type_float);

    case  jet_ptrans:
      return make_pair("jet_ptrans",PidCandidate::type_float);

    case  jet_minv:
      return make_pair("jet_minv",PidCandidate::type_float);

    case  jet_mtrans:
      return make_pair("jet_mtrans",PidCandidate::type_float);

    case  jet_ncomp:
      return make_pair("jet_ncomp",PidCandidate::type_uint);

    case  jet_ncomp_above_0p1:
      return make_pair("jet_ncomp_above_0p1",PidCandidate::type_uint);

    case  jet_ncomp_above_1:
      return make_pair("jet_ncomp_above_1",PidCandidate::type_uint);

    case  jet_ncomp_above_10:
      return make_pair("jet_ncomp_above_10",PidCandidate::type_uint);

    case  jet_ncomp_emcal:
      return make_pair("jet_ncomp_emcal",PidCandidate::type_uint);

      // ------

    case  jetshape_radius:
      return make_pair("jetshape_radius",PidCandidate::type_float);

    case  jetshape_rms:
      return make_pair("jetshape_rms",PidCandidate::type_float);

    case  jetshape_r90:
      return make_pair("jetshape_r90",PidCandidate::type_float);

    case  jetshape_econe_r01:
      return make_pair("jetshape_econe_r01",PidCandidate::type_float);

    case  jetshape_econe_r02:
      return make_pair("jetshape_econe_r02",PidCandidate::type_float);

    case  jetshape_econe_r03:
      return make_pair("jetshape_econe_r03",PidCandidate::type_float);

    case  jetshape_econe_r04:
      return make_pair("jetshape_econe_r04",PidCandidate::type_float);

    case  jetshape_econe_r05:
      return make_pair("jetshape_econe_r05",PidCandidate::type_float);

      // ------

    case  jetshape_emcal_radius:
      return make_pair("jetshape_emcal_radius",PidCandidate::type_float);

    case  jetshape_emcal_rms:
      return make_pair("jetshape_emcal_rms",PidCandidate::type_float);

    case  jetshape_emcal_r90:
      return make_pair("jetshape_emcal_r90",PidCandidate::type_float);

    case  jetshape_emcal_econe_r01:
      return make_pair("jetshape_emcal_econe_r01",PidCandidate::type_float);

    case  jetshape_emcal_econe_r02:
      return make_pair("jetshape_emcal_econe_r02",PidCandidate::type_float);

    case  jetshape_emcal_econe_r03:
      return make_pair("jetshape_emcal_econe_r03",PidCandidate::type_float);

    case  jetshape_emcal_econe_r04:
      return make_pair("jetshape_emcal_econe_r04",PidCandidate::type_float);

    case  jetshape_emcal_econe_r05:
      return make_pair("jetshape_emcal_econe_r05",PidCandidate::type_float);

      // ------

    case  tracks_count_r02:
      return make_pair("tracks_count_r02",PidCandidate::type_uint);

    case  tracks_count_r04:
      return make_pair("tracks_count_r04",PidCandidate::type_uint);

    case  tracks_count_R:
      return make_pair("tracks_count_R",PidCandidate::type_uint);

    case  tracks_rmax_r02:
      return make_pair("tracks_rmax_r02",PidCandidate::type_float);

    case  tracks_rmax_r04:
      return make_pair("tracks_rmax_r04",PidCandidate::type_float);

    case  tracks_rmax_R:
      return make_pair("tracks_rmax_R",PidCandidate::type_float);

    case  tracks_chargesum_r02:
      return make_pair("tracks_chargesum_r02",PidCandidate::type_int);

    case  tracks_chargesum_r04:
      return make_pair("tracks_chargesum_r04",PidCandidate::type_int);

    case  tracks_chargesum_R:
      return make_pair("tracks_chargesum_R",PidCandidate::type_int);

      // ------

      // EM Candidates:

    case  em_cluster_id:
      return make_pair("em_cluster_id",PidCandidate::type_uint);

    case  em_cluster_prob:
      return make_pair("em_cluster_prob",PidCandidate::type_float);

    case  em_cluster_posx:
      return make_pair("em_cluster_posx",PidCandidate::type_float);

    case  em_cluster_posy:
      return make_pair("em_cluster_posy",PidCandidate::type_float);

    case  em_cluster_posz:
      return make_pair("em_cluster_posz",PidCandidate::type_float);

    case  em_cluster_e:
      return make_pair("em_cluster_e",PidCandidate::type_float);

    case  em_cluster_ecore:
      return make_pair("em_cluster_ecore",PidCandidate::type_float);

    case  em_cluster_et_iso:
      return make_pair("em_cluster_et_iso",PidCandidate::type_float);

    case  em_cluster_theta:
      return make_pair("em_cluster_theta",PidCandidate::type_float);

    case  em_cluster_phi:
      return make_pair("em_cluster_phi",PidCandidate::type_float);

    case  em_cluster_eta:
      return make_pair("em_cluster_eta",PidCandidate::type_float);

    case  em_cluster_pt:
      return make_pair("em_cluster_pt",PidCandidate::type_float);

    case  em_cluster_ntower:
      return make_pair("em_cluster_ntower",PidCandidate::type_uint);

    case  em_cluster_caloid:
      return make_pair("em_cluster_caloid",PidCandidate::type_uint);

      // ------

    case  em_track_id:
      return make_pair("em_track_id",PidCandidate::type_uint);

    case  em_track_quality:
      return make_pair("em_track_quality",PidCandidate::type_float);

    case  em_track_theta:
      return make_pair("em_track_theta",PidCandidate::type_float);

    case  em_track_phi:
      return make_pair("em_track_phi",PidCandidate::type_float);

    case  em_track_eta:
      return make_pair("em_track_eta",PidCandidate::type_float);

    case  em_track_ptotal:
      return make_pair("em_track_ptotal",PidCandidate::type_float);

    case  em_track_ptrans:
      return make_pair("em_track_ptrans",PidCandidate::type_float);

    case  em_track_charge:
      return make_pair("em_track_charge",PidCandidate::type_int);

    case  em_track_dca:
      return make_pair("em_track_dca",PidCandidate::type_float);

    case  em_track_section:
      return make_pair("em_track_section",PidCandidate::type_uint);

    case  em_track_e3x3_cemc:
      return make_pair("em_track_e3x3_cemc",PidCandidate::type_float);

    case  em_track_e3x3_femc:
      return make_pair("em_track_e3x3_femc",PidCandidate::type_float);

    case  em_track_e3x3_eemc:
      return make_pair("em_track_e3x3_eemc",PidCandidate::type_float);

    case  em_track_e3x3_ihcal:
      return make_pair("em_track_e3x3_ihcal",PidCandidate::type_float);

    case  em_track_e3x3_ohcal:
      return make_pair("em_track_e3x3_ohcal",PidCandidate::type_float);

    case  em_track_e3x3_fhcal:
      return make_pair("em_track_e3x3_fhcal",PidCandidate::type_float);

    case  em_track_e3x3_ehcal:
      return make_pair("em_track_e3x3_ehcal",PidCandidate::type_float);

    case  em_track_cluster_dr:
      return make_pair("em_track_cluster_dr",PidCandidate::type_float);

    case  em_track_theta2cluster:
      return make_pair("em_track_theta2cluster",PidCandidate::type_float);
    
    case  em_track_eta2cluster:
      return make_pair("em_track_eta2cluster",PidCandidate::type_float);
      
    case  em_track_phi2cluster:
      return make_pair("em_track_phi2cluster",PidCandidate::type_float);

    case  em_track_p2cluster:
      return make_pair("em_track_p2cluster",PidCandidate::type_float);
      
    case  em_track_x2cluster:
      return make_pair("em_track_x2cluster",PidCandidate::type_float);

    case  em_track_y2cluster:
      return make_pair("em_track_y2cluster",PidCandidate::type_float);

    case  em_track_z2cluster:
      return make_pair("em_track_z2cluster",PidCandidate::type_float);

      // ------

    case  em_pid_prob_electron:
      return make_pair("em_pid_prob_electron",PidCandidate::type_float);

    case  em_pid_prob_pion:
      return make_pair("em_pid_prob_pion",PidCandidate::type_float);

    case  em_pid_prob_kaon:
      return make_pair("em_pid_prob_kaon",PidCandidate::type_float);

    case  em_pid_prob_proton:
      return make_pair("em_pid_prob_proton",PidCandidate::type_float);

      // ------

    case  em_evtgen_pid:
      return make_pair("em_evtgen_pid",PidCandidate::type_int);

    case  em_evtgen_ptotal:
      return make_pair("em_evtgen_ptotal",PidCandidate::type_float);

    case  em_evtgen_etotal:
      return make_pair("em_evtgen_etotal",PidCandidate::type_float);

    case  em_evtgen_theta:
      return make_pair("em_evtgen_theta",PidCandidate::type_float);

    case  em_evtgen_phi:
      return make_pair("em_evtgen_phi",PidCandidate::type_float);

    case  em_evtgen_eta:
      return make_pair("em_evtgen_eta",PidCandidate::type_float);

    case  em_evtgen_charge:
      return make_pair("em_evtgen_charge",PidCandidate::type_int);

    case  em_evtgen_is_scattered_lepton:
      return make_pair("em_evtgen_is_scattered_lepton",PidCandidate::type_uint);

      // ------

    case  em_reco_x_e:
      return make_pair("em_reco_x_e",PidCandidate::type_float);

    case  em_reco_y_e:
      return make_pair("em_reco_y_e",PidCandidate::type_float);

    case  em_reco_q2_e:
      return make_pair("em_reco_q2_e",PidCandidate::type_float);

    case  em_reco_w_e:
      return make_pair("em_reco_w_e",PidCandidate::type_float);

      // ------

    default:
      cout << "PidCandidate::get_property_info - Fatal Error - unknown index " << prop_id << endl;
      exit(1);
    }
}


bool
PidCandidate::check_property(const PROPERTY prop_id, const PROPERTY_TYPE prop_type)
{
  pair<const string,PROPERTY_TYPE> property_info = get_property_info(prop_id);
  if (property_info.second != prop_type)
    {
      return false;
    }
  return true;
}


string
PidCandidate::get_property_type(const PROPERTY_TYPE prop_type)
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
