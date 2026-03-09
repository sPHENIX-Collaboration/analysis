#include "StreakEventsIdentifier.h"


#include <fun4all/Fun4AllReturnCodes.h>


// Fun4All
#include <ffaobjects/EventHeader.h>
#include <fun4all/Fun4AllHistoManager.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/Fun4AllServer.h>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>
#include <phool/phool.h>


// ROOT stuff
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TH3F.h>
#include <TLorentzVector.h>
#include <TTree.h>



#include <fun4all/SubsysReco.h>



// Event / Run
#include <ffaobjects/EventHeader.h>
#include <ffaobjects/RunHeader.h>

// Calo / towers / clusters / jets
#include <calobase/RawCluster.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawClusterUtility.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/TowerInfo.h>
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfoDefs.h>
#include <jetbase/Jet.h>
#include <jetbase/JetContainer.h>

// For clusters and geometry

#include <calobase/RawTower.h>

// ROOT
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TCanvas.h>
#include <TStyle.h>

#include <calotrigger/TriggerRunInfo.h>


// GL1 Information
#include <ffarawobjects/Gl1Packet.h>

// for cluster vertex correction
#include <CLHEP/Geometry/Point3D.h>

// for the vertex
#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>
#include <globalvertex/MbdVertex.h>
#include <globalvertex/MbdVertexMap.h>

#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4VtxPoint.h>
#include <g4main/PHG4Shower.h>


#include <mbd/MbdGeom.h>
#include <mbd/MbdPmtContainer.h>
#include <mbd/MbdPmtHit.h>

#include <phhepmc/PHHepMCGenEvent.h>
#include <phhepmc/PHHepMCGenEventMap.h>
#include <ffaobjects/RunHeader.h>

#include <calotrigger/TriggerRunInfo.h>

#include <jetbase/Jetv1.h>
#include <jetbase/Jetv2.h>
#include <jetbase/JetContainer.h>

#include <set>                          // for std::set in fillClusterWindows
#include <CLHEP/Vector/ThreeVector.h>   // for CLHEP::Hep3Vector
#include <calobase/RawTowerDefs.h>      // for RawTowerDefs::decode_index*
#include <phhepmc/PHGenIntegral.h>


#include <TSystem.h>
#include <TMath.h>
#include <iomanip>
#include <fstream>

#include <ios>

#include <TCanvas.h>
#include <TLatex.h>

#include <TVector2.h>



// -----------------------------------------------------------
StreakEventsIdentifier::StreakEventsIdentifier(
  const std::string& name,
  const std::string& outprefix)
: SubsysReco(name), m_outprefix(outprefix) {}

StreakEventsIdentifier::~StreakEventsIdentifier() = default;

// -----------------------------------------------------------
int StreakEventsIdentifier::Init(PHCompositeNode* topNode) {

  std::cout.setf(std::ios::unitbuf); 
  std::cout << "[Init] Setting ROOT style and booking histos...\n";
  gStyle->SetOptStat(0);
  bookHistos();
  
  if (Verbosity()>0) {
    std::cout << "[Init] Booking histos. Output prefix='" << m_outprefix << "'\n"
              << "       Cuts: Et_min=" << m_et_min
              << " GeV, weta_min=" << m_weta_min
              << ", |t|<" << m_abs_time_cut_ns << " ns, tRMS>=" << m_time_spread_min << " ns\n"
              << "       Time weight Ethresh=" << m_time_weight_Ethresh
              << " GeV, tower E for shapes=" << m_min_tower_E_for_shapes << " GeV" << std::endl;
  }
  const std::string ofn = m_outprefix + "_results.root";
  std::cout << "[Init] Opening output ROOT file: " << ofn << "\n";
  m_out.reset(TFile::Open(ofn.c_str(), "RECREATE"));
  return 0;
}

// -----------------------------------------------------------
int StreakEventsIdentifier::InitRun(PHCompositeNode* topNode) {
  if (auto* rh = findNode::getClass<RunHeader>(topNode, "RunHeader")) {
    m_runnumber = rh->get_RunNumber();
    std::cout << "[InitRun] RunHeader found. Run = " << m_runnumber << "\n";
  } else {
    std::cout << "[InitRun][WARN] RunHeader node not found.\n";
  }
  return 0;
}


// -----------------------------------------------------------
/////GEtting the nodes needed towers and clusters
// -----------------------------------------------------------
RawClusterContainer* StreakEventsIdentifier::getCemcClusterContainer(PHCompositeNode* topNode) const {
  const char* candidates[] = {
    "CLUSTERINFO_CEMC",
    nullptr
  };
  for (const char** p = candidates; *p; ++p) {
    if (auto* c = findNode::getClass<RawClusterContainer>(topNode, *p)) {
      std::cout << "[getCemcClusterContainer] Using node: " << *p << "\n";
      return c;
    }
  }
  std::cout << "[getCemcClusterContainer][WARN] No EMCal cluster container found.\n";
  return nullptr;
}


//Raw towers
TowerInfoContainer* StreakEventsIdentifier::getCemcRawTowers(PHCompositeNode* topNode) const {
  auto* t = findNode::getClass<TowerInfoContainer>(topNode, "TOWERS_CEMC");
  std::cout << (t ? "[getCemcRawTowers] Found TOWERS_CEMC.\n"
                  : "[getCemcRawTowers][INFO] No RAW tower container (continuing).\n");
  return t;
}
  
//Calibrated towers
TowerInfoContainer* StreakEventsIdentifier::getCemcCalibTowers(PHCompositeNode* topNode) const {
  const char* names[] = {
    "TOWERINFO_CALIB_CEMC_RETOWER",    
    "TOWERINFO_CALIB_CEMC_RETOWER_SUB1",       
    nullptr
  };
  for (const char** n = names; *n; ++n) {
    if (auto* t = findNode::getClass<TowerInfoContainer>(topNode, *n)) {
      std::cout << "[getCemcCalibTowers] Found " << *n << "\n";
      return t;
    }
  }
  std::cout << "[getCemcCalibTowers][WARN] No calibrated CEMC TowerInfo container found.\n";
  return nullptr;
}

RawTowerGeomContainer* StreakEventsIdentifier::getCemcGeom(PHCompositeNode* topNode) const {
  auto* g = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_CEMC");
  std::cout << (g ? "[getCemcGeom] Found TOWERGEOM_CEMC.\n"
                  : "[getCemcGeom][WARN] Missing TOWERGEOM_CEMC.\n");
  return g;
}


// --- IHCal ---
TowerInfoContainer* StreakEventsIdentifier::getIhcalCalibTowers(PHCompositeNode* topNode) const {
  auto* t = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALIN");
  std::cout << (t ? "[getIhcalCalibTowers] Found TOWERINFO_CALIB_HCALIN.\n"
                  : "[getIhcalCalibTowers][WARN] Missing TOWERINFO_CALIB_HCALIN.\n");
  return t;
}
TowerInfoContainer* StreakEventsIdentifier::getIhcalRawTowers(PHCompositeNode* topNode) const {
  auto* t = findNode::getClass<TowerInfoContainer>(topNode, "TOWERS_HCALIN");
  std::cout << (t ? "[getIhcalRawTowers] Found TOWERS_HCALIN.\n"
                  : "[getIhcalRawTowers][INFO] No RAW IHCal tower container (continuing).\n");
  return t;
}
RawTowerGeomContainer* StreakEventsIdentifier::getIhcalGeom(PHCompositeNode* topNode) const {
  auto* g = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALIN");
  std::cout << (g ? "[getIhcalGeom] Found TOWERGEOM_HCALIN.\n"
                  : "[getIhcalGeom][WARN] Missing TOWERGEOM_HCALIN.\n");
  return g;
}

// --- OHCal ---
TowerInfoContainer* StreakEventsIdentifier::getOhcalCalibTowers(PHCompositeNode* topNode) const {
  auto* t = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT");
  std::cout << (t ? "[getOhcalCalibTowers] Found TOWERINFO_CALIB_HCALOUT.\n"
                  : "[getOhcalCalibTowers][WARN] Missing TOWERINFO_CALIB_HCALOUT.\n");
  return t;
}
TowerInfoContainer* StreakEventsIdentifier::getOhcalRawTowers(PHCompositeNode* topNode) const {
  auto* t = findNode::getClass<TowerInfoContainer>(topNode, "TOWERS_HCALOUT");
  std::cout << (t ? "[getOhcalRawTowers] Found TOWERS_HCALOUT.\n"
                  : "[getOhcalRawTowers][INFO] No RAW OHCal tower container (continuing).\n");
  return t;
}
RawTowerGeomContainer* StreakEventsIdentifier::getOhcalGeom(PHCompositeNode* topNode) const {
  auto* g = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALOUT");
  std::cout << (g ? "[getOhcalGeom] Found TOWERGEOM_HCALOUT.\n"
                  : "[getOhcalGeom][WARN] Missing TOWERGEOM_HCALOUT.\n");
  return g;
}


JetContainer* StreakEventsIdentifier::getJetContainer(PHCompositeNode* topNode) const {
  if (!m_jet_container_hint.empty()) {
    if (auto* jc = findNode::getClass<JetContainer>(topNode, m_jet_container_hint)) {
      std::cout << "[getJetContainer] Using hinted jet container: " << m_jet_container_hint << "\n";
      return jc;
    } else {
      std::cout << "[getJetContainer][INFO] Hint '" << m_jet_container_hint << "' not found.\n";
    }
  }
  const char* cands[] = { "AntiKt_Tower_r04", "AntiKt_Tower_r04_Sub1", "AntiKt_Tower_r04_Combined", nullptr };
  for (const char** p = cands; *p; ++p) {
    if (auto* jc = findNode::getClass<JetContainer>(topNode, *p)) {
      std::cout << "[getJetContainer] Using jet container: " << *p << "\n";
      return jc;
    }
  }
  std::cout << "[getJetContainer][INFO] No jet container found.\n";
  return nullptr;
}

float StreakEventsIdentifier::getVertexZ(PHCompositeNode* topNode) const {
  if (auto* mbd = findNode::getClass<MbdVertexMap>(topNode, "MbdVertexMap")) {
    if (!mbd->empty()) {
      const float vz = mbd->begin()->second->get_z();
      std::cout << "[getVertexZ] Using MBD z = " << vz << "\n";
      return vz;
    }
  }
  std::cout << "[getVertexZ][INFO] No MBD vertex. Using z=0.\n";
  return 0.f;
}

// -----------------------------------------------------------
/////Shower shape windows from CaloAna24
// -----------------------------------------------------------

  bool StreakEventsIdentifier::fillClusterWindows(
    const RawCluster* cl,
    TowerInfoContainer* emcCalib,
    TowerInfoContainer* emcRaw,
    ClusterWindows& win,
    int& maxieta, int& maxiphi,
    float& cog_eta, float& cog_phi) const
{
  if (!cl) return false;

  // tower source (96×256)
  auto is_full = [](TowerInfoContainer* c){ return c && c->size() >= 96*256; };
  TowerInfoContainer* src = nullptr;
  if (is_full(emcCalib)) src = emcCalib;
  else if (is_full(emcRaw)) src = emcRaw;
  else {
    std::cout << " No full 96 *256 tower container (calib="
              << (emcCalib?emcCalib->size():-1) << ", raw=" << (emcRaw?emcRaw->size():-1) << ")\n";
    return false;
  }

  //  max-energy tower from the cluster towermap
  float emax = -1.f; int seed_i=-1, seed_j=-1;
  for (const auto& kv : cl->get_towermap()) {
    const int i = RawTowerDefs::decode_index1(kv.first);
    const int j = RawTowerDefs::decode_index2(kv.first);
    const unsigned int key = TowerInfoDefs::encode_emcal(i,j);
    if (auto* tw = src->get_tower_at_key(key)) {
      const float e = tw->get_energy();
      if (e > emax) { emax = e; seed_i = i; seed_j = j; }
    }
  }
  if (seed_i < 0) {
    std::cout << "  [fillClusterWindows][WARN] Towermap had no usable towers for this cluster.\n";
    return false;
  }
  maxieta = seed_i;  // seed center
  maxiphi = seed_j;

  //  use shapes for a fractional CoG  
  float frac_eta = 0.f, frac_phi = 0.f;
  {
    std::vector<float> ss = cl->get_shower_shapes(0.070f);
    if (ss.size() >= 6) {

      // Here we only keep the fractional part in [-0.5, +0.5] as a sub-tower
      float eta_like = ss[4]; 
      float phi_like = ss[5]; 
      frac_eta = (eta_like - std::floor(eta_like)) - 0.5f;  
      frac_phi = (phi_like - std::floor(phi_like)) - 0.5f;
      std::cout << "  [fillClusterWindows] shapes present: frac_eta=" << frac_eta
                << " frac_phi=" << frac_phi << "\n";
    } else {
      std::cout << "  [fillClusterWindows] no usable shapes; using energy-only CoG.\n";
    }
  }

  // Building 7×7 window 
  auto wrap_phi = [](int& iphi){ iphi = (iphi%256 + 256)%256; };
  auto in_eta = [](int i){ return (0 <= i && i < 96); };

  float sumE=0.f, sumEi=0.f, sumEj=0.f;
  std::set<unsigned int> owned;
  for (const auto& it : cl->get_towermap()) {
    owned.insert( TowerInfoDefs::encode_emcal(RawTowerDefs::decode_index1(it.first),
                                             RawTowerDefs::decode_index2(it.first)) );
  }

  for (int di=-3; di<=+3; ++di) {
    for (int dj=-3; dj<=+3; ++dj) {
      int i = maxieta + di;
      int j = maxiphi + dj;
      if (!in_eta(i)) continue;        // truncate at eta edges
      wrap_phi(j);                      // wrap phi

      const int I = di + 3;
      const int J = dj + 3;

      const unsigned int key = TowerInfoDefs::encode_emcal(i,j);
      TowerInfo* tw = src->get_tower_at_key(key);
      
      if (!tw || !tw->get_isGood()) continue;

      const float e = tw ? tw->get_energy() : 0.f;
      const float t = tw ? tw->get_time() : 0.f;

      if (e < 0.f) continue; // skip negative energy towers
      if (!t) continue; // skip no-time towers
      //if (t < -999.f) continue; // skip invalid time towers


      win.E77[I][J]   = (e > m_min_tower_E_for_shapes) ? e : 0.f;
      win.Own77[I][J] = owned.count(key) ? 1 : 0;
      win.T77[I][J]   = tw ? tw->get_time() : 0.f;

      if (win.E77[I][J] > 0.f) {
        sumE  += win.E77[I][J];
        sumEi += win.E77[I][J] * I;
        sumEj += win.E77[I][J] * J;
      }
    }
  }

  if (sumE <= 0.f) {
    std::cout << "  [fillClusterWindows][WARN] No energy in 7 X 7 window.\n";
    return false;
  }

  // CoG: energy-weighted from the window + fractional from shapes
  cog_eta = (sumEi / sumE) + frac_eta; 
  cog_phi = (sumEj / sumE) + frac_phi;

  return true;
}


void StreakEventsIdentifier::computeWidths(
    const ClusterWindows& win, float cog_eta, float cog_phi,
    float& weta, float& weta_cogx, float& wphi, float& wphi_cogx) const  
{
  const bool owned_only = true;

  float sumE = 0.f, sumE_eta2 = 0.f, sumE_phi2 = 0.f;
  float sumE_excl = 0.f, sumE_eta2_excl = 0.f, sumE_phi2_excl = 0.f;

  for (int i = 0; i < 7; ++i) {
    for (int j = 0; j < 7; ++j) {
      const float E = win.E77[i][j];
      if (E <= 0.f) continue;
      if (owned_only && !win.Own77[i][j]) continue;

      const float di = float(i) - cog_eta;
      const float dj = float(j) - cog_phi;
      
      sumE       += E;
      sumE_eta2  += E * di * di;
      sumE_phi2  += E * dj * dj;  

      if (!(i == 3 && j == 3)) {
        sumE_excl       += E;
        sumE_eta2_excl  += E * di * di;
        sumE_phi2_excl  += E * dj * dj;  
      }
    }
  }

  weta       = (sumE      > 0.f) ? (sumE_eta2      / sumE)      : 0.f;
  weta_cogx  = (sumE_excl > 0.f) ? (sumE_eta2_excl / sumE_excl) : 0.f;
  wphi       = (sumE      > 0.f) ? (sumE_phi2      / sumE)      : 0.f;
  wphi_cogx  = (sumE_excl > 0.f) ? (sumE_phi2_excl / sumE_excl) : 0.f; 
}

// -----------------------------------------------------------

void StreakEventsIdentifier::computeTimeMoments(
    const ClusterWindows& w, float& t_avg, float& t_rms) const {
  float sumE = 0.f, sumEt = 0.f, sumEt2 = 0.f;
  for (int i=0;i<7;++i) for (int j=0;j<7;++j) {
    const float E = w.E77[i][j];
    const float t = w.T77[i][j];

    if (E < m_time_weight_Ethresh) continue; // weight threshold
    sumE   += E;
    sumEt  += E * t;
    sumEt2 += E * t * t;
  }
  if (sumE>0) {
    t_avg = sumEt / sumE;
    const float var = std::max(0.f, (sumEt2/sumE) - t_avg*t_avg);
    t_rms = std::sqrt(var);
  } else {
    t_avg = -999.f; t_rms = -999.f;
  }
  std::cout << "    [computeTimeMoments] t_avg=" << t_avg << " t_rms=" << t_rms
            << " (sumE=" << sumE << ")\n";
}


// extractCaloTiming ---Extract weighted average timing from calorimeter
std::pair<float, float> StreakEventsIdentifier::extractCaloTiming(
    TowerInfoContainer* towers, float energy_threshold) const {
    
  if (!towers) {
    std::cout << "    [extractCaloTiming] No tower container provided\n";
    return {-999.f, -999.f};
  }
  
  float sum_time = 0.f;
  float sum_weight = 0.f;
  std::vector<float> times;
  
  // Loop over all towers in the calorimeter
  const int ntowers = towers->size();
  for (int i = 0; i < ntowers; ++i) {
    TowerInfo* tw = towers->get_tower_at_channel(i);
    if (!tw || !tw->get_isGood()) continue;
    
    float energy = tw->get_energy();
    if (energy < energy_threshold) continue;
    
    float time = tw->get_time();
    if (time < -900.f) continue;  // Skip invalid times
    
    sum_time += time * energy;
    sum_weight += energy;
    times.push_back(time);
  }
  
  if (sum_weight > 0.f && !times.empty()) {
    float mean = sum_time / sum_weight;
    
    // Calculate RMS
    float sum_sq = 0.f;
    for (float t : times) {
      sum_sq += (t - mean) * (t - mean);
    }
    float rms = std::sqrt(sum_sq / times.size());
    
    std::cout << "    [extractCaloTiming] mean=" << mean << " ns, rms=" << rms 
              << " ns (from " << times.size() << " towers, sum_weight=" 
              << sum_weight << " GeV)\n";
    
    return {mean, rms};
  }
  
  std::cout << "    [extractCaloTiming] No valid timing data found\n";
  return {-999.f, -999.f};
}

// -----------------------------------------------------------
//STARting the event processing
// -----------------------------------------------------------

int StreakEventsIdentifier::process_event(PHCompositeNode* topNode) {
  ++m_evt_processed;

  if (Verbosity()>2 && (m_evt_processed<=15 || m_evt_processed%500==1))
  std::cout << "\n[Event] idx=" << m_evt_processed << std::endl;

  if (m_evt_processed % 100 == 1) {
    std::cout << "\n=== [Event " << m_evt_processed << "] Starting processing ===" << std::endl;
  }

  // event header
  int eventnumber = 0;
  if (auto* eh = findNode::getClass<EventHeader>(topNode, "EventHeader")) {
    eventnumber = eh->get_EvtSequence();
    std::cout << "[process_event] Run " << eh->get_RunNumber()
              << "  Event " << eventnumber << "\n";
  } else {
    std::cout << "[process_event][WARN] No EventHeader node.\n";
  }

   // ===== Triggers Selection =====

   Gl1Packet* gl1PacketInfo = findNode::getClass<Gl1Packet>(topNode, "GL1Packet");
  
  if (!gl1PacketInfo) {
    std::cout << "[process_event][WARN] GL1Packet node is missing. "
              << "Skipping trigger selection for event " << m_eventnumber << "\n";
  }

    if (gl1PacketInfo) {
      // getting the trigger vectors
      uint64_t triggervec = gl1PacketInfo->getScaledVector();
      uint64_t triggervecraw = gl1PacketInfo->getLiveVector();
      
      // Processing   all 64 trigger bits
      for (int i = 0; i < 64; i++) {
        bool trig_decision = ((triggervec & 0x1U) == 0x1U);
        bool trig_decision_raw = ((triggervecraw & 0x1U) == 0x1U);

        if (i < 64) {
          // Reset and set trigger flags
          m_scaledtrigger[i] = false;
          m_scaledtrigger[i] = trig_decision;
          
          m_livetrigger[i] = false;
          m_livetrigger[i] = trig_decision_raw;
          
          // Count triggers
          if (trig_decision) m_nscaledtrigger[i]++;
          if (trig_decision_raw) m_nlivetrigger[i]++;
          
          // Initialize scalers on first event
          if (!m_initilized) {
            for (int j = 0; j < 3; j++) {
              m_initscaler[i][j] = gl1PacketInfo->lValue(i, j);
            }
          }
          
          // Store current scalers
          for (int j = 0; j < 3; j++) {
            m_currentscaler[i][j] = gl1PacketInfo->lValue(i, j);
            if (j == 0) m_currentscaler_raw[i] = m_currentscaler[i][j];
            if (j == 1) m_currentscaler_live[i] = m_currentscaler[i][j];
            if (j == 2) m_currentscaler_scaled[i] = m_currentscaler[i][j];
          }
        }

        // Shift trigger vectors for next bit
        triggervec = (triggervec >> 1U);
        triggervecraw = (triggervecraw >> 1U);
      }
      m_initilized = true;
      
      // Get trigger prescales
      TriggerRunInfo* trigRunInfo = findNode::getClass<TriggerRunInfo>(topNode, "TriggerRunInfo");
      if (trigRunInfo) {
        for (int i = 0; i < 32; i++) {
          m_trigger_prescale[i] = trigRunInfo->getPrescaleByBit(i);
        }
      }
      
      // Apply trigger selection
      if (!m_using_trigger_bits.empty()) {
        bool trigger_passed = false;
        
        for (int bit : m_using_trigger_bits) {
          if (bit >= 0 && bit < 64 && m_scaledtrigger[bit]) {
            trigger_passed = true;
            std::cout << "  [Trigger] Event " << m_eventnumber 
                      << " PASSED trigger bit " << bit << "\n";
            break;  
          }
        }
        
        if (!trigger_passed) {
          std::cout << "  [Trigger] Event " << m_eventnumber 
                    << " REJECTED - does not pass required trigger bits (";
          for (size_t i = 0; i < m_using_trigger_bits.size(); i++) {
            std::cout << m_using_trigger_bits[i];
            if (i < m_using_trigger_bits.size() - 1) std::cout << ", ";
          }
          std::cout << ")\n";
          return Fun4AllReturnCodes::ABORTEVENT;
        }
      } else {
        std::cout << "  [Trigger] No trigger bits specified - accepting all events\n";
      }
    }
   
  // ===== End of trigger selection =====



  // Get tower containers
  auto* emcTowerContainer = getCemcCalibTowers(topNode);
  auto* ihcalTowerContainer = getIhcalCalibTowers(topNode);
  auto* ohcalTowerContainer = getOhcalCalibTowers(topNode);
  
  // Calculate total calo energies
  m_totalEMCal_energy = 0.f;
  m_totalIHCal_energy = 0.f;
  m_totalOHCal_energy = 0.f;
  
  // EMCal towers Loop
  if (emcTowerContainer) {
    const int emcsize = emcTowerContainer->size();
    for (int i = 0; i < emcsize; i++) {
      TowerInfo* towerinfo = emcTowerContainer->get_tower_at_channel(i);
      if (towerinfo && towerinfo->get_isGood()) {
        m_totalEMCal_energy += towerinfo->get_energy();
      }
    }
    std::cout << "  [EMCal] Total energy: " << m_totalEMCal_energy << " GeV\n";
  }
  
  // IHCal tower loop
  if (ihcalTowerContainer) {
    const int ihsize = ihcalTowerContainer->size();
    for (int i = 0; i < ihsize; i++) {
      TowerInfo* towerinfo = ihcalTowerContainer->get_tower_at_channel(i);
      if (towerinfo && towerinfo->get_isGood()) {
        m_totalIHCal_energy += towerinfo->get_energy();
      }
    }
    std::cout << "  [IHCal] Total energy: " << m_totalIHCal_energy << " GeV\n";
  }
  
  // Loop  OHCal towers
  if (ohcalTowerContainer) {
    const int ohsize = ohcalTowerContainer->size();
    for (int i = 0; i < ohsize; i++) {
      TowerInfo* towerinfo = ohcalTowerContainer->get_tower_at_channel(i);
      if (towerinfo && towerinfo->get_isGood()) {
        m_totalOHCal_energy += towerinfo->get_energy();
      }
    }
    std::cout << "  [OHCal] Total energy: " << m_totalOHCal_energy << " GeV\n";
  }

  // ============================================================================

  // Get event header
  auto* eventheader = findNode::getClass<EventHeader>(topNode, "EventHeader");
  m_eventnumber = eventheader ? eventheader->get_EvtSequence() : 0;

  // run header
  if (auto* rh = findNode::getClass<RunHeader>(topNode, "RunHeader")) {
    m_runnumber = rh->get_RunNumber();
  }
  m_run_total[m_runnumber]++;

  // Nodes
  auto* clusters = getCemcClusterContainer(topNode);
  auto* emcCalib = getCemcCalibTowers(topNode);
  auto* emcRaw   = getCemcRawTowers(topNode);
  auto* jets     = getJetContainer(topNode);

  
  if (!clusters || (!emcCalib && !emcRaw)) {
  std::cout << "[process_event][SKIP] Missing clusters or any CEMC tower container.\n";
  return Fun4AllReturnCodes::EVENT_OK;
  }

  if (Verbosity()>1) {
  std::cout << "  nodes:"
            << " clusters=" << (clusters? "OK":"NULL")
            << " calibTowers=" << (emcCalib? "OK":"NULL")
            << " rawTowers="   << (emcRaw? "OK":"NULL")
            << " jets="        << (jets? "OK":"NULL") << std::endl;
  }

  if (!clusters || !emcCalib) {
    std::cout << "[process_event][SKIP] Missing clusters or calib towers. clusters="
              << (clusters?"yes":"no") << " calib="
              << (emcCalib?"yes":"no") << "\n";
    return Fun4AllReturnCodes::EVENT_OK;
  }

  // Z vertex from MBDvertexMap

  float vertexz = -9999.0f;
  auto* vertexmap = findNode::getClass<MbdVertexMap>(topNode, "MbdVertexMap");
  
  if (!vertexmap || vertexmap->empty()) {
    if (Verbosity() > 1) {
      std::cout << "[process_event][SKIP] No MBD vertex map" << std::endl;
    }
    return Fun4AllReturnCodes::EVENT_OK;
  }
  
  MbdVertex* mbd_vtx = vertexmap->begin()->second;
  if (!mbd_vtx) {
    if (Verbosity() > 1) {
      std::cout << "[process_event][SKIP] Null vertex pointer" << std::endl;
    }
    return Fun4AllReturnCodes::EVENT_OK;
  }
  
  vertexz = mbd_vtx->get_z();
  
  // Check for NaN vertex
  if (vertexz != vertexz) {  // NaN check
    if (Verbosity() > 1) {
      std::cout << "[process_event][SKIP] NaN vertex" << std::endl;
    }
    return Fun4AllReturnCodes::EVENT_OK;
  }
  
  // Apply vertex cut
  if (std::fabs(vertexz) > m_vertex_cut) {
    if (Verbosity() > 1) {
      std::cout << "[process_event][SKIP] Vertex |z| = " << std::fabs(vertexz) 
                << " cm exceeds cut of " << m_vertex_cut << " cm" << std::endl;
    }
    return Fun4AllReturnCodes::EVENT_OK;
  }
  
  if (Verbosity() > 2) {
    std::cout << "[process_event] Using vertex z = " << vertexz << " cm" << std::endl;
  }
  // ============================================================================

  const CLHEP::Hep3Vector vertex(0,0,vertexz);

    // Gettin MBD timing
  float mbd_time = -999.f;
  bool mbd_time_valid = false;
  if (auto* mbdpmts = findNode::getClass<MbdPmtContainer>(topNode, "MbdPmtContainer")) {
    // Calculating average MBD timing from both sides
    float sum_time = 0.f;
    int n_valid = 0;
    for (int i = 0; i < mbdpmts->get_npmt(); ++i) {
      auto* pmt = mbdpmts->get_pmt(i);
      if (pmt && pmt->get_q() > 0.4) {  // charge threshold
        sum_time += pmt->get_time();
        ++n_valid;
      }
    }
    if (n_valid > 0) {
      mbd_time = sum_time / n_valid;
      mbd_time_valid = true;
      std::cout << "  [MBD] Average time = " << mbd_time << " ns (from " << n_valid << " PMTs)\n";
    }
  } else {
    std::cout << "  [MBD][INFO] No MbdPmtContainer found.\n";
  }


  int njet_seen = 0;
  if (jets && Verbosity()>2) {
  for (const auto* j : *jets) { if (j) ++njet_seen; }
  std::cout << "  jets: n=" << njet_seen << std::endl;
  }

  // --- Collect jets (for veto and QA) ---
  bool has_back_to_back = false;
  std::vector<float> jet_pt, jet_phi, jet_E;
  if (jets) {
    for (const auto* j : *jets) {
      if (!j) continue;
      jet_pt .push_back(j->get_pt());
      jet_phi.push_back(j->get_phi());
      jet_E  .push_back(j->get_e());
      if (h_jet_pt_vs_phi) h_jet_pt_vs_phi->Fill(j->get_phi(), j->get_pt());
    }
    std::cout << "[process_event] Njets=" << jet_pt.size() << "\n";

    const int njet = static_cast<int>(jet_pt.size());
    // dijet veto -- both jets pT>10, dphi > pi/2
    for (int a = 0; a < njet; ++a) {
      if (jet_pt[a] <= 10.f) continue;
      for (int b = a + 1; b < njet; ++b) {
        if (jet_pt[b] <= 10.f) continue;
        const float dphi = std::fabs(std::acos(std::cos(jet_phi[a] - jet_phi[b])));
        if (h_jet_dphi) h_jet_dphi->Fill(dphi);
        if (jet_pt[b] > 0.f && h_dphi_vs_pt_ratio) {
          h_dphi_vs_pt_ratio->Fill(dphi, jet_pt[a] / jet_pt[b]);
        }
        if (dphi > TMath::Pi() / 2.0) {
          has_back_to_back = true;
          std::cout << "  [Jets] Back-to-back found: a=" << a << " b=" << b
                    << " pt(a)=" << jet_pt[a] << " pt(b)=" << jet_pt[b]
                    << " dphi=" << dphi << " (>pi/2) -> veto\n";
          break;
        }
      }
      if (has_back_to_back) break;
    }

    // Jet hemisphere asymmetry
    float jet_e_plus=0.f, jet_e_minus=0.f;
    for (int i = 0; i < njet; ++i) {
      ((std::cos(jet_phi[i])>0) ? jet_e_plus : jet_e_minus) += jet_E[i];
    }
    const float jet_asym = safe_div(jet_e_plus-jet_e_minus, jet_e_plus+jet_e_minus);
    if (h_jet_asym) h_jet_asym->Fill(jet_asym);
    std::cout << "  [Jets] has_back_to_back=" << has_back_to_back
              << " jet_asym=" << jet_asym << "\n";
  }

  // --- Loop clusters ---
  bool cluster_condition_met = false;
  float e_plus=0.f, e_minus=0.f; // cluster hemisphere 
  int ncl = 0;

  RawClusterContainer::ConstRange range = clusters->getClusters();
  for (auto it = range.first; it != range.second; ++it) {
    const RawCluster* cl = it->second;
    if (!cl) continue;
    ++ncl;

    const float E  = cl->get_energy();
    const float eta = RawClusterUtility::GetPseudorapidity(*cl, vertex);
    const float phi = RawClusterUtility::GetAzimuthAngle(*cl, vertex);
    const float Et = E / std::cosh(eta);
    if (Et < 0.5f) continue; 

    ClusterWindows w; int maxieta=0, maxiphi=0; float cog_eta=0, cog_phi=0;
    const bool ok = fillClusterWindows(cl, emcCalib, emcRaw, w, maxieta, maxiphi, cog_eta, cog_phi);
    if (!ok) {
      std::cout << "  [Cluster] skip: could not build 7x7. Et=" << Et
                << " eta=" << eta << " phi=" << phi << "\n";
      continue;
    }

    float weta=0.f, weta_cog=0.f, weta_cogx=0.f, wphi_cog=0.f, wphi_cogx=0.f;
    computeWidths(w, cog_eta, cog_phi, weta_cog, weta_cogx, wphi_cog, wphi_cogx); 

    float tavg=0, trms=0;
    computeTimeMoments(w, tavg, trms);

    bool this_cluster_has_timing = (tavg > -900.f);

    // QA fills (all events)
    h_eta_phi_all->Fill(phi, eta);
    h_phi_Et_all->Fill(phi, Et);
    h_weta_all->Fill(weta);
    h_weta_all_x->Fill(weta_cogx);
    h_wphi_all->Fill(wphi_cogx);
    h_cluster_Et_all->Fill(Et);
    if (tavg>-998.f) h_cluster_time_all->Fill(tavg);
    if (trms>-998.f) h_time_spread_all->Fill(trms);

    // Hemisphere energy (clusters)
    (std::cos(phi)>0 ? e_plus : e_minus) += Et;


  ////////////////////////////STREAK Events SELECTION////////////////////////////
    // Now using: (Weta + Et + timing)
   bool passes_weta = (weta_cogx > m_weta_min);
    bool passes_et = (Et >= m_et_min);
    bool passes_timing = !m_require_valid_timing || this_cluster_has_timing;
    
    if (passes_weta && passes_et && passes_timing)
      cluster_condition_met = true;

    std::cout << "  [Cluster] Et=" << Et
              << " eta=" << eta << " phi=" << phi
              << " weta(CoG)=" << weta_cog
              << " weta_cogx=" << weta_cogx
              << " tavg=" << tavg << " trms=" << trms
              << " -> pass_timing=" << passes_timing  << "\n";
  }
  std::cout << "[process_event] Nclusters(looped)=" << ncl << "\n";


  const float e_asym = safe_div(e_plus - e_minus, e_plus + e_minus);
  h_asymmetry->Fill(e_asym);
  std::cout << "  [Clusters] e_asym=" << e_asym
            << " cluster_condition_met=" << cluster_condition_met << "\n";

  // Event-level streak tag
  if (cluster_condition_met && !has_back_to_back) {

  auto [emcal_mean, emcal_rms] = extractCaloTiming(emcTowerContainer, 0.1);
  auto [ihcal_mean, ihcal_rms] = extractCaloTiming(ihcalTowerContainer, 0.1);
  auto [ohcal_mean, ohcal_rms] = extractCaloTiming(ohcalTowerContainer, 0.1);
  
  // Require at least one calorimeter has valid timing
  bool has_timing = (emcal_mean > -900.f || ihcal_mean > -900.f || ohcal_mean > -900.f);
  
  std::cout << "  [Timing Check] EMCal: " << emcal_mean << " ns (E=" << m_totalEMCal_energy << " GeV), "
            << "IHCal: " << ihcal_mean << " ns (E=" << m_totalIHCal_energy << " GeV), "
            << "OHCal: " << ohcal_mean << " ns (E=" << m_totalOHCal_energy << " GeV), "
            << "has_timing=" << has_timing << "\n";

    // Check for valid timing if required
    if (m_require_valid_timing && !has_timing) {
    std::cout << "  [Timing] Event REJECTED - no valid timing in any calorimeter\n";
    return Fun4AllReturnCodes::EVENT_OK;
  }

    // Check MBD timing cut
    bool mbd_cut_pass = true;
    if (mbd_time_valid) {
      mbd_cut_pass = (mbd_time >= m_mbd_time_min && mbd_time <= m_mbd_time_max);
      std::cout << "  [MBD] Timing check: " << mbd_time << " ns, cut=[" 
                << m_mbd_time_min << ", " << m_mbd_time_max << "], pass=" 
                << mbd_cut_pass << "\n";
    }
    
    if (mbd_cut_pass) {
      ++m_streak_count;
      m_run_streak[m_runnumber]++;
      m_streakEvents.emplace_back(m_runnumber, eventnumber);
      std::cout << "  [TAG] Event marked as STREAK. run=" << m_runnumber
                << " evt=" << eventnumber << " (streak_count=" << m_streak_count << ")\n";

      // If exclude , skip further processing
      if (m_exclude_streaks) {
        ++m_excluded_count;
        std::cout << "  ***** Event " << eventnumber 
                  << " is EXCLUDED - it is a streak *****\n";
        return Fun4AllReturnCodes::DISCARDEVENT;  // Skip this event
      }

      // Fill streak-specific shapes ( if not excluding)
      for (auto it = range.first; it != range.second; ++it) {
        const RawCluster* cl = it->second; if (!cl) continue;
        const float E  = cl->get_energy();
        const float eta = RawClusterUtility::GetPseudorapidity(*cl, vertex);
        const float phi = RawClusterUtility::GetAzimuthAngle(*cl, vertex);
        const float Et = E / std::cosh(eta);
        if (Et < 0.5f) continue;

        ClusterWindows w; int maxieta=0, maxiphi=0; float cog_eta=0, cog_phi=0;
        if (!fillClusterWindows(cl, emcCalib, emcRaw, w, maxieta, maxiphi, cog_eta, cog_phi)) continue;


        float weta=0.f, weta_cog=0.f, weta_cogx=0.f, wphi_cog=0.f, wphi_cogx=0.f;
        computeWidths(w, cog_eta, cog_phi, weta_cog, weta_cogx, wphi_cog, wphi_cogx);

        float tavg=-999.f, trms=-999.f; computeTimeMoments(w, tavg, trms);

        h_eta_phi_streak->Fill(phi, eta);
        h_phi_Et_streak->Fill(phi, Et);
        h_weta_streak->Fill(weta);
        h_weta_streak_x->Fill(weta_cogx);
        h_wphi_streak->Fill(wphi_cogx);
        h_cluster_Et_streak->Fill(Et);

        if (tavg>-998.f) h_cluster_time_streak->Fill(tavg);
        if (trms>-998.f) h_time_spread_streak->Fill(trms);
      }
    } else {
      std::cout << "  [TAG] Event NOT a streak (MBD timing failed).\n";
    }
  } else {
    std::cout << "  [TAG] Event NOT a streak."
              << " cluster_condition_met=" << cluster_condition_met
              << " has_back_to_back=" << has_back_to_back << "\n";
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

// -----------------------------------------------------------
int StreakEventsIdentifier::End(PHCompositeNode* topNode) {
  std::cout << "[End] Writing outputs...\n";
  if (m_out) {
    m_out->cd();
    // write histos
    h_eta_phi_all->Write(); h_eta_phi_streak->Write();
    h_phi_Et_all->Write();  h_phi_Et_streak->Write();
    h_weta_all->Write();    h_weta_streak->Write();
    h_weta_all_x->Write();    h_weta_streak_x->Write();
    h_wphi_all->Write();    h_wphi_streak->Write(); 
    h_cluster_Et_all->Write(); h_cluster_Et_streak->Write();
    h_asymmetry->Write(); h_jet_dphi->Write(); h_jet_asym->Write();
    h_dphi_vs_pt_ratio->Write(); h_jet_pt_vs_phi->Write();
    h_cluster_time_all->Write(); h_cluster_time_streak->Write();
    h_time_spread_all->Write();  h_time_spread_streak->Write();

  }

  // Event list
  {
    const std::string evlist = m_outprefix + "_event_list.txt";
    std::ofstream fout(evlist);
    fout << "# run event totalInRun\n";
    for (auto& ev : m_streakEvents) {
      const int run = ev.first; const int evt = ev.second;
      const int tot = m_run_total[run];
      fout << run << " " << evt << " " << tot << "\n";
    }
    std::cout << "[End] Wrote event list: " << evlist << " (N=" << m_streakEvents.size() << ")\n";
  }

  // Run summary
  {
    const std::string runsum = m_outprefix + std::string("_run_summary.txt");
    std::ofstream rs(runsum);
    rs << "# RunNumber TotalEvents StreakEvents Rate(%)\n";
    for (auto& kv : m_run_total) {
      const int run = kv.first; const int tot = kv.second;
      const int stk = m_run_streak[run];
      const double rate = tot>0 ? 100.0*double(stk)/double(tot) : 0.0;
      rs << run << " " << tot << " " << stk << " " << std::fixed << std::setprecision(3) << rate << "\n";
    }
    std::cout << "[End] Wrote run summary: " << runsum << "\n";
  }

  // PNGs
  if (!m_png_dir.empty()) {
    std::cout << "[End] Saving PNGs into: " << m_png_dir << "\n";
    gSystem->mkdir(m_png_dir.c_str(), kTRUE);
    savePNGs();
  } else {
    std::cout << "[End] PNG directory empty; skipping PNGs.\n";
  }

  std::cout << "\n=== StreakEventsIdentifier Summary ===" << std::endl;
  std::cout << "  Total events processed: " << m_evt_processed << std::endl;
  std::cout << "  Total streak events:    " << m_streak_count << std::endl;
  if (m_exclude_streaks) {
    std::cout << "  Events EXCLUDED:        " << m_excluded_count << std::endl;
  }
  std::cout << "  Output written to:      " << m_outprefix << "_results.root" << std::endl;
  std::cout << "=============================================\n" << std::endl;

  if (m_out) { m_out->Write(); m_out->Close(); }
  return 0;
}

// -----------------------------------------------------------
void StreakEventsIdentifier::bookHistos() {
  auto mk1 = [&](const char* n,const char* t,int nb,double lo,double hi){ auto* h=new TH1F(n,t,nb,lo,hi); h->SetDirectory(nullptr); return h; };
  auto mk2 = [&](const char* n,const char* t,int nbx,double xlo,double xhi,int nby,double ylo,double yhi){ auto* h=new TH2F(n,t,nbx,xlo,xhi,nby,ylo,yhi); h->SetDirectory(nullptr); return h; };

  std::cout << "[bookHistos] Booking histograms...\n";
  h_eta_phi_all     = mk2("h_eta_phi_all"   ,"Eta vs Phi (All);#phi;#eta",64,-3.2,3.2,64,-1.1,1.1);
  h_eta_phi_streak  = mk2("h_eta_phi_streak","Eta vs Phi (Streak);#phi;#eta",64,-3.2,3.2,64,-1.1,1.1);
  h_phi_Et_all      = mk2("h_phi_Et_all"    ,"Cluster E_{T} vs #phi (All);#phi;E_{T} [GeV]",64,-3.2,3.2,100,0,50);
  h_phi_Et_streak   = mk2("h_phi_Et_streak" ,"Cluster E_{T} vs #phi (Streak);#phi;E_{T} [GeV]",64,-3.2,3.2,100,0,50);

  h_weta_all_x    = mk1("h_weta_all_x",   "Cluster w_{#eta} (All, CoGx);w_{#eta}^{CoGx};Counts",100,0,3.0);
  h_weta_streak_x = mk1("h_weta_streak_x","Cluster w_{#eta} (Streak, CoGx);w_{#eta}^{CoGx};Counts",100,0,3.0);
 
  h_weta_all    = mk1("h_weta_all",   "Cluster w_{#eta} (All);w_{#eta};Counts",100,0,3.0);
  h_weta_streak = mk1("h_weta_streak","Cluster w_{#eta} (Streak);w_{#eta};Counts",100,0,3.0);
  
  h_wphi_all    = mk1("h_wphi_all",   "Cluster w_{#phi} (All, CoGx);w_{#phi}^{CoGx};Counts",100,0,3.0);
  h_wphi_streak = mk1("h_wphi_streak","Cluster w_{#phi} (Streak, CoGx);w_{#phi}^{CoGx};Counts",100,0,3.0);

  h_cluster_Et_all  = mk1("h_cluster_Et_all","Cluster E_{T} (All);E_{T} [GeV];Counts",100,0,100);
  h_cluster_Et_streak= mk1("h_cluster_Et_streak","Cluster E_{T} (Streak);E_{T} [GeV];Counts",100,0,100);
  h_asymmetry       = mk1("h_asymmetry"     ,"Hemisphere Energy Asymmetry;E_{diff};Counts",100,-1,1);
  h_jet_dphi        = mk1("h_jet_dphi"      ,"#Delta#phi between leading jets;#Delta#phi;Counts",64,0,M_PI);
  h_jet_asym        = mk1("h_jet_asym"      ,"Jet energy hemisphere asymmetry;Asymmetry;Counts",100,-1,1);
  h_dphi_vs_pt_ratio= mk2("h_dphi_vs_pt_ratio","#Delta#phi vs p_{T} Ratio;#Delta#phi;p_{T,lead}/p_{T,sublead}",64,0,M_PI,100,0,10);
  h_jet_pt_vs_phi   = mk2("h_jet_pt_vs_phi" ,"Jet p_{T} vs #phi;#phi;p_{T} [GeV]",64,-M_PI,M_PI,100,0,100);
  h_cluster_time_all   = mk1("h_cluster_time_all"  ,"Cluster Timing (All);Time [ns];Counts",100,-50,50);
  h_cluster_time_streak= mk1("h_cluster_time_streak","Cluster Timing (Streak);Time [ns];Counts",100,-50,50);
  h_time_spread_all    = mk1("h_time_spread_all"   ,"Cluster Time Spread (All);Time RMS [ns];Counts",100,0,20);
  h_time_spread_streak = mk1("h_time_spread_streak","Cluster Time Spread (Streak);Time RMS [ns];Counts",100,0,20);
  std::cout << "[bookHistos] Done.\n";
}

void StreakEventsIdentifier::savePNGs() const {
  std::cout << "[savePNGs] Producing PNGs...\n";
  auto Save1D = [&](TH1* h,const std::string& tag,bool logy=false){ TCanvas c("c1","",800,700); if (logy) c.SetLogy(); h->Draw("HIST"); c.SaveAs((m_png_dir+"/"+tag+".png").c_str()); };
  auto Save2D = [&](TH2* h,const std::string& tag){ TCanvas c("c2","",900,800); h->SetContour(60); h->Draw("COLZ"); c.SaveAs((m_png_dir+"/"+tag+".png").c_str()); };

  Save2D(h_eta_phi_all, "eta_phi_all");
  Save2D(h_eta_phi_streak, "eta_phi_streak");
  Save2D(h_phi_Et_all, "phi_vs_Et_all");
  Save2D(h_phi_Et_streak, "phi_vs_Et_streak");
  Save1D(h_weta_all, "weta_all");
  Save1D(h_weta_streak, "weta_streak");
  Save1D(h_weta_all_x, "weta_all_x");
  Save1D(h_weta_streak_x, "weta_streak_x");
  Save1D(h_wphi_all, "wphi_all");        
  Save1D(h_wphi_streak, "wphi_streak");  
  Save1D(h_jet_dphi, "jet_dphi");
  Save1D(h_asymmetry, "hemi_asym_all");
  Save1D(h_jet_asym, "jet_hemi_asym_all");
  Save1D(h_cluster_time_all, "cluster_time_all");
  Save1D(h_cluster_time_streak, "cluster_time_streak");
  Save1D(h_time_spread_all, "time_spread_all");
  Save1D(h_time_spread_streak, "time_spread_streak");
  std::cout << "[savePNGs] Done.\n";
}
// -----------------------------------------------------------
