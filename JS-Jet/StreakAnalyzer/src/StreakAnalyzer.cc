
#include "StreakAnalyzer.h"

using namespace streak;

// -----------------------------------------------------------
//Getting the Photon Node: PHOTONCLUSTER_CEMC and calcualating physics variables
//-----------------------------------------------------------------------
bool StreakAnalyzer::pass_cluster_selection(PHCompositeNode* topNode, std::string* why) const
{
  auto* phCont = findNode::getClass<PhotonClusterContainer>(topNode, m_cfg.photon_node);
  if (!phCont) {
    if (m_cfg.verbose && why) (*why) += "PhotonClusterContainer '" + m_cfg.photon_node + "' not found.\n";
    return false;
  }

  const auto range = phCont->getClusters();
  int n_total = 0, n_pass = 0;

  for (auto it = range.first; it != range.second; ++it) {
    const PhotonCluster* ph = it->second;
    if (!ph) continue;
    ++n_total;

    //Getting the photon cluster eta, E and Et from the PhotonClusterBuilder
    const float eta = ph->get_shower_shape_parameter("cluster_eta");
    const float E   = ph->get_shower_shape_parameter("e77");
    const float Et  = (std::isfinite(eta) && std::cosh(eta) != 0.0) ? (E / std::cosh(eta)) : 0.f;

    //Getting weta_Center-of-gravity in x direction
    float weta = ph->get_shower_shape_parameter("weta_cogx");
    if (!std::isfinite(weta) || weta <= 0.f){

    // backup if 'weta_cogx' is not found, so we can take 'weta_cog' instead
      weta = ph->get_shower_shape_parameter("weta_cog"); 
    }

      //cuts for Et and weta to identify streak clusters
    const bool pass = (Et >= m_cfg.et_min) && (weta >= m_cfg.weta_min);
    if (pass) ++n_pass;

    if (m_cfg.verbose && why) {
      (*why) += "Cluster: Et=" + std::to_string(Et)
             + " GeV, weta=" + std::to_string(weta)
             + (pass ? " [PASS]\n" : " [FAIL]\n");
    }
  }

  //If the event has been rejected due to no clusters passing, provide info
  if (m_cfg.verbose && why) {
    (*why) += "Clusters passing = " + std::to_string(n_pass) + " / " + std::to_string(n_total) + "\n";
  }
  return (n_pass > 0);
}

// -----------------------------------------------------------
//Dijet veto: looking for back-to-back jets in specified jet containers
// -----------------------------------------------------------
bool StreakAnalyzer::pass_dijet_veto(PHCompositeNode* topNode, std::string* why) const
{
  if (!m_cfg.apply_dijet_veto) return true;

  // find first available jet container from preference list
  JetContainer* jets = nullptr;
  std::string picked;

  for (const auto& name : m_cfg.jet_nodes) {
 
    if (auto* found = findNode::getClass<JetContainer>(topNode, name)) {
      jets = const_cast<JetContainer*>(found); 
      picked = name;
      break;
    }
  }

  // If no jets found, cannot veto
  if (!jets) {
    if (m_cfg.verbose && why) (*why) += "Dijet veto: no JetContainer found \n";
    return true;
  }

  // Collect jet phi of jets above threshold
  std::vector<float> phi;
  phi.reserve(jets->size());

  
  for (Jet::IterJetTCA it = jets->begin(); it != jets->end(); ++it) {
    Jet* j = *it;
    if (!j) continue;
    if (j->get_pt() >= m_cfg.jet_pt_min) phi.push_back(j->get_phi());
  }

  // If not enough jets above threshold, cannot veto
  if (m_cfg.verbose && why) {
    (*why) += "Dijet veto: picked '" + picked + "', jets above "
           + std::to_string(m_cfg.jet_pt_min) + " GeV = "
           + std::to_string(phi.size()) + "\n";
  }

  // veto of back-to-back jets
  const int n = static_cast<int>(phi.size());
  for (int i = 0; i < n; ++i) {
    for (int k = i + 1; k < n; ++k) {
      if (dphi_abs(phi[i], phi[k]) >= m_cfg.back_to_back_min_dphi) {
        if (m_cfg.verbose && why)
          (*why) += "Dijet veto: back-to-back jets found -> FAIL.\n";
        return false; 
      }
    }
  }
  return true; // when there is  no veto
}

// -----------------------------------------------------------
//Main function to determine if event is streak event
// -----------------------------------------------------------
bool StreakAnalyzer::isStreakEvent(PHCompositeNode* topNode, std::string* why) const
{


  if (m_cfg.verbose && why) {
  (*why) += "Streak config: photon_node='" + m_cfg.photon_node
         + "', et_min="   + std::to_string(m_cfg.et_min)
         + " GeV, weta_min=" + std::to_string(m_cfg.weta_min)
         + ", dijet_veto=" + std::string(m_cfg.apply_dijet_veto ? "on" : "off")
         + ", jet_pt_min=" + std::to_string(m_cfg.jet_pt_min)
         + ", dphi_min="   + std::to_string(m_cfg.back_to_back_min_dphi) + "\n";
}


  if (!topNode) return false;
  if (!pass_cluster_selection(topNode, why)) return false;
  if (!pass_dijet_veto(topNode, why))        return false;
  return true;
}
