#ifndef STREAK_ANALYZER_H
#define STREAK_ANALYZER_H

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include <calobase/PhotonClusterContainer.h>
#include <calobase/PhotonCluster.h>

#include <jetbase/JetContainer.h>
#include <jetbase/Jet.h>

#include <TMath.h>
#include <string>
#include <vector>
#include <cmath>

namespace streak {

struct Config {
  // Node name
  std::string photon_node = "PHOTONCLUSTER_CEMC";
  std::vector<std::string> jet_nodes = {
  
   "AntiKt_unsubtracted_r04", //The one we want to use right now

   //may be used in future runs
   /*
    "AntiKt_Tower_r04_Sub1",
    "AntiKt_Tower_r03_Sub1",
    "AntiKt_Tower_r02_Sub1",
    "AntiKt_unsubtracted_r04",
    "AntiKt_unsubtracted_r03",
    "AntiKt_unsubtracted_r02"
    */
  };

  //The Streak event selection cuts

  // Cluster cuts
  float et_min   = 10.0f;  // GeV
  float weta_min = 0.7f;

  // Dijet veto
  bool  apply_dijet_veto      = true;
  float jet_pt_min            = 10.0f;        // GeV
  float back_to_back_min_dphi = TMath::Pi()/2; // rad

  bool  verbose = false;
};

class StreakAnalyzer {
public:
  explicit StreakAnalyzer(const Config& cfg = Config()) : m_cfg(cfg) {}
  bool isStreakEvent(PHCompositeNode* topNode, std::string* why = nullptr) const;

private:
  Config m_cfg;

  // Helper to calculate  dphi value
  static inline float dphi_abs(float a, float b) {
    float d = a - b;
    while (d >  TMath::Pi()) d -= 2.f*TMath::Pi();
    while (d <= -TMath::Pi()) d += 2.f*TMath::Pi();
    return std::fabs(d);
  }

  // Cluster and dijets streak tag
  bool pass_cluster_selection(PHCompositeNode* topNode, std::string* why) const;
  bool pass_dijet_veto(PHCompositeNode* topNode, std::string* why) const;
};

// -----------------------------------------------------------
//This the main function users will call to check if event is streak event
// -----------------------------------------------------------
inline bool isStreakEvent(PHCompositeNode* topNode, const Config& cfg = Config(), std::string* why = nullptr) {
  return StreakAnalyzer(cfg).isStreakEvent(topNode, why);
}

} // namespace streak

#endif
