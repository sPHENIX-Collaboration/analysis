#include "caloTreeGen.h"

#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>

//Fun4All
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllHistoManager.h>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>
#include <phool/phool.h>
#include <ffaobjects/EventHeader.h>

//ROOT stuff
#include <TH1F.h>
#include <TH2F.h>
#include <TFile.h>
#include <TLorentzVector.h>

//For clusters and geometry
#include <calobase/RawCluster.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawClusterUtility.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/TowerInfoContainerv1.h>
#include <calobase/TowerInfov1.h>

//Tower stuff
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfo.h>

//____________________________________________________________________________..
caloTreeGen::caloTreeGen(const std::string &name, const std::string &name2):
  SubsysReco(name)
  ,T(nullptr)
  ,Outfile(name)
  ,Outfile2(name2)
  ,iEvent(0)
  ,min_towE(0)
  ,min_clusterECore(0)
  ,min_clusterEta(0)
  ,min_clusterPhi(0)
  ,min_clusterPt(0)
  ,min_clusterChi(0)
  ,max_towE(0)
  ,max_clusterECore(0)
  ,max_clusterEta(0)
  ,max_clusterPhi(0)
  ,max_clusterPt(0)
  ,max_clusterChi(0)
  ,max_NClusters(0)
{
  std::cout << "caloTreeGen::caloTreeGen(const std::string &name) Calling ctor" << std::endl;
}

//____________________________________________________________________________..
caloTreeGen::~caloTreeGen()
{
  std::cout << "caloTreeGen::~caloTreeGen() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
int caloTreeGen::Init(PHCompositeNode *topNode)
{
  out = new TFile(Outfile.c_str(),"RECREATE");

  hTowE = new TH1F("hTowE", "Tower Energy; Energy [GeV]; Counts", bins_towE, low_e, high_e);
  hClusterECore = new TH1F("hClusterECore", "Cluster ECore; Energy [GeV]; Counts", bins_e, low_e, high_e);
  hClusterPt = new TH1F("hClusterPt", "Cluster p_{T}; p_{T} [GeV]; Counts", bins_pt, low_pt, high_pt);
  hClusterChi = new TH1F("hClusterChi", "Cluster #chi^{2}; #chi^{2}; Counts", bins_chi, low_chi, high_chi);
  hClusterTime = new TH1F("hClusterTime", "Cluster Time; Time; Counts", bins_time, low_time, high_time);
  hNClusters = new TH1F("hNClusters", "Cluster; # of clusters per event; Counts", bins_n, low_n, high_n);
  hTotalMBD = new TH1F("hTotalMBD", "MBD Charge; MBD Charge; Counts", bins_totalmbd, low_totalmbd, high_totalmbd);
  hTotalCaloE = new TH1F("hTotalCaloE", "Total Calo E; Total Calo E [GeV]; Counts", bins_totalcaloE, low_totalcaloE, high_totalcaloE);

  h2ClusterEtaPhi = new TH2F("h2ClusterEtaPhi", "Cluster; #eta; #phi", bins_eta, low_eta, high_eta, bins_phi, low_phi, high_phi);
  h2ClusterEtaPhiWeighted = new TH2F("h2ClusterEtaPhiWeighted", "Cluster ECore; #eta; #phi", bins_eta, low_eta, high_eta, bins_phi, low_phi, high_phi);
  h2TowEtaPhiWeighted = new TH2F("h2TowEtaPhiWeighted", "Tower Energy; Towerid #eta; Towerid #phi",  bins_eta, 0, bins_eta, bins_phi, 0, bins_phi);
  h2TotalMBDCaloE = new TH2F("h2TotalMBDCaloE", "Total MBD Charge vs Total EMCAL Energy; Total EMCAL Energy [Arb]; Total MBD Charge [Arb]", 100, 0, 1, 100, 0, 1);

  out2 = new TFile(Outfile2.c_str(),"RECREATE");

  T = new TNtuple("T","T","totalCaloE:totalMBD:"
                           "clus_E:clus_eta:clus_phi:clus_chi:clus_time:"
                           "clus_E2:clus_eta2:clus_phi2:clus_chi2:clus_time2:"
                           "pi0_mass:pi0_pt:pi0_eta:pi0_phi");

  //so that the histos actually get written out
  Fun4AllServer *se = Fun4AllServer::instance();
  se -> Print("NODETREE"); 
  
  std::cout << "caloTreeGen::Init(PHCompositeNode *topNode) Initializing" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int caloTreeGen::InitRun(PHCompositeNode *topNode)
{
  std::cout << "caloTreeGen::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int caloTreeGen::process_event(PHCompositeNode *topNode)
{

  if(iEvent%500 == 0) std::cout << "Progress: " << iEvent << std::endl;
  iEvent++;

  //Information on clusters
  RawClusterContainer *clusterContainer = findNode::getClass<RawClusterContainer>(topNode,"CLUSTERINFO_POS_COR_CEMC");
  // RawClusterContainer *clusterContainer = findNode::getClass<RawClusterContainer>(topNode,"CLUSTERINFO_CEMC");
  if(!clusterContainer)
  {
    std::cout << PHWHERE << "caloTreeGen::process_event - Fatal Error - CLUSTERINFO_POS_COR_CEMC node is missing. " << std::endl;
    return 0;
  }
  
  //tower information
  TowerInfoContainer *emcTowerContainer;
  emcTowerContainer = findNode::getClass<TowerInfoContainer>(topNode,"TOWERINFO_CALIB_CEMC");
  if(!emcTowerContainer)
  {
    std::cout << PHWHERE << "caloTreeGen::process_event Could not find node TOWERS_CEMC"  << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }
  
  
  //Tower geometry node for location information
  RawTowerGeomContainer *towergeom = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_CEMC");
  if (!towergeom)
  {
    std::cout << PHWHERE << "caloTreeGen::process_event Could not find node TOWERGEOM_CEMC"  << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }
  
  float calib = 1.;

  Float_t totalMBD = 0;
  TowerInfoContainer* offlinetowers = findNode::getClass<TowerInfoContainerv1>(topNode, "TOWERS_MBD");
  if (offlinetowers) {
    int size = offlinetowers->size(); //online towers should be the same!
    for (int channel = 0; channel < size;channel++) {
      TowerInfo* offlinetower = offlinetowers->get_tower_at_channel(channel);
      float offlineenergy = offlinetower->get_energy();
      unsigned int towerkey = TowerInfoDefs::encode_mbd(channel);
      int type = TowerInfoDefs::get_mbd_type(towerkey);
      if (type == 1) {
        totalMBD += offlineenergy;
      }
    }
  }

  max_totalmbd = std::max(max_totalmbd, totalMBD);
  hTotalMBD->Fill(totalMBD);

  //grab all the towers and fill their energies. 
  unsigned int tower_range = emcTowerContainer->size();
  Float_t totalCaloE = 0;
  for(unsigned int iter = 0; iter < tower_range; iter++)
  {
    unsigned int towerkey = emcTowerContainer->encode_key(iter);
    unsigned int ieta = getCaloTowerEtaBin(towerkey);
    unsigned int iphi = getCaloTowerPhiBin(towerkey);

    double energy = emcTowerContainer -> get_tower_at_channel(iter) -> get_energy()/calib;
    totalCaloE += energy;
    // double time = emcTowerContainer -> get_tower_at_channel(iter) -> get_time();

    min_towE = std::min(min_towE, energy);
    max_towE = std::max(max_towE, energy);

    hTowE->Fill(energy);

    // phi is in range [0, 2pi] so we need map [pi, 2pi] to [-pi, 0]
    // double phi = towergeom -> get_phicenter(iphi);
    // if(phi >= M_PI) phi -= 2*M_PI;

    // double eta = towergeom -> get_etacenter(ieta);

    // check to make sure that ieta and iphi are in range
    if(ieta >= bins_eta || iphi >= bins_phi) std::cerr << "ieta: " << ieta << ", iphi: " << iphi << std::endl;

    h2TowEtaPhiWeighted->Fill(ieta, iphi, energy);
  }

  max_totalCaloE = std::max(max_totalCaloE, totalCaloE);
  hTotalCaloE->Fill(totalCaloE);

  h2TotalMBDCaloE->Fill(totalCaloE/high_totalcaloE, totalMBD/high_totalmbd);

  max_NClusters = std::max(max_NClusters, clusterContainer->size());
  hNClusters->Fill(clusterContainer->size());

  RawClusterContainer::ConstRange clusterEnd = clusterContainer -> getClusters();
  RawClusterContainer::ConstIterator clusterIter;
  RawClusterContainer::ConstIterator clusterIter2;
  for(clusterIter = clusterEnd.first; clusterIter != clusterEnd.second; clusterIter++)
  {
    RawCluster *recoCluster = clusterIter -> second;

    CLHEP::Hep3Vector vertex(0,0,0);
    CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetECoreVec(*recoCluster, vertex);
    // CLHEP::Hep3Vector E_vec_cluster_Full = RawClusterUtility::GetEVec(*recoCluster, vertex);

    float clusE = E_vec_cluster.mag();
    float clus_eta = E_vec_cluster.pseudoRapidity();
    float clus_phi = E_vec_cluster.phi();
    float clus_pt = E_vec_cluster.perp();
    float clus_chi = recoCluster -> get_chi2();
    // float nTowers = recoCluster ->getNTowers();
    // float maxTowerEnergy = getMaxTowerE(recoCluster,emcTowerContainer);

    min_clusterECore = std::min(min_clusterECore, clusE);
    min_clusterEta = std::min(min_clusterEta, clus_eta);
    min_clusterPhi = std::min(min_clusterPhi, clus_phi);
    min_clusterPt = std::min(min_clusterPt, clus_pt);
    min_clusterChi = std::min(min_clusterChi, clus_chi);

    max_clusterECore = std::max(max_clusterECore, clusE);
    max_clusterEta = std::max(max_clusterEta, clus_eta);
    max_clusterPhi = std::max(max_clusterPhi, clus_phi);
    max_clusterPt = std::max(max_clusterPt, clus_pt);
    max_clusterChi = std::max(max_clusterChi, clus_chi);

    if(clusE >= clusE_min) {
      hClusterChi->Fill(clus_chi);
    }

    if(clusE < clusE_min || clus_chi >= clus_chi_max) continue;

    Short_t clus_time = getMaxTowerTime(recoCluster, emcTowerContainer);

    hClusterECore->Fill(clusE);
    h2ClusterEtaPhi->Fill(clus_eta, clus_phi);
    h2ClusterEtaPhiWeighted->Fill(clus_eta, clus_phi, clusE);
    hClusterPt->Fill(clus_pt);
    hClusterTime->Fill(clus_time);

    TLorentzVector photon1;
    photon1.SetPtEtaPhiE(clus_pt, clus_eta, clus_phi, clusE);

    for(clusterIter2 = std::next(clusterIter); clusterIter2 != clusterEnd.second; clusterIter2++) {

      if(clusterIter2 == clusterIter) std::cout << "Error: clusterIter2 == clusterIter" << std::endl;

      RawCluster *recoCluster2 = clusterIter2 -> second;

      CLHEP::Hep3Vector vertex2(0,0,0);
      CLHEP::Hep3Vector E_vec_cluster2 = RawClusterUtility::GetECoreVec(*recoCluster2, vertex2);

      float clusE2 = E_vec_cluster2.mag();
      float clus_eta2 = E_vec_cluster2.pseudoRapidity();
      float clus_phi2 = E_vec_cluster2.phi();
      float clus_pt2 = E_vec_cluster2.perp();
      float clus_chi2 = recoCluster2->get_chi2();

      if(clusE2 < clusE_min || clus_chi2 >= clus_chi_max) continue;

      Short_t clus_time2 = getMaxTowerTime(recoCluster2, emcTowerContainer);

      TLorentzVector photon2;
      photon2.SetPtEtaPhiE(clus_pt2, clus_eta2, clus_phi2, clusE2);

      TLorentzVector pi0 = photon1 + photon2;

      Float_t pi0_mass = pi0.M();
      Float_t pi0_pt = pi0.Pt();
      Float_t pi0_eta = pi0.Eta();
      Float_t pi0_phi = pi0.Phi();

      // not storing cluster pT since we can derive pT = E/cosh(eta)
      Float_t pi0_data[] = {totalCaloE, totalMBD,
                            clusE,      clus_eta,  clus_phi,  clus_chi,  (Float_t)clus_time,
                            clusE2,     clus_eta2, clus_phi2, clus_chi2, (Float_t)clus_time2,
                            pi0_mass,   pi0_pt,    pi0_eta,   pi0_phi};

      T->Fill(pi0_data);
    }
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int caloTreeGen::ResetEvent(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int caloTreeGen::EndRun(const int runnumber)
{
  std::cout << "caloTreeGen::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int caloTreeGen::End(PHCompositeNode *topNode)
{

  std::cout << "max totalCaloE: " << max_totalCaloE << std::endl;
  std::cout << "max totalmbd: " << max_totalmbd << std::endl;
  std::cout << "min tower energy: " << min_towE << ", max tower energy: " << max_towE << std::endl;
  std::cout << "min clusterECore: " << min_clusterECore << ", max clusterECore: " << max_clusterECore << std::endl;
  std::cout << "min clusterEta: " << min_clusterEta << ", max clusterEta: " << max_clusterEta << std::endl;
  std::cout << "min clusterPhi: " << min_clusterPhi << ", max clusterPhi: " << max_clusterPhi << std::endl;
  std::cout << "min clusterPt: " << min_clusterPt << ", max clusterPt: " << max_clusterPt << std::endl;
  std::cout << "min clusterChi: " << min_clusterChi << ", max clusterChi: " << max_clusterChi << std::endl;
  std::cout << "max NClusters: " << max_NClusters << std::endl;
  std::cout << "pi0 cuts: cluster minimum energy: " << clusE_min << ", cluster maximum chi2: " << clus_chi_max << std::endl;

  std::cout << "caloTreeGen::End(PHCompositeNode *topNode) This is the End..." << std::endl;

  out -> cd();

  hTotalMBD->Write();
  hTotalCaloE->Write();
  hTowE->Write();
  hClusterECore->Write();
  hClusterPt->Write();
  hClusterChi->Write();
  hClusterTime->Write();
  hNClusters->Write();
  h2ClusterEtaPhi->Write();
  h2ClusterEtaPhiWeighted->Write();
  h2TowEtaPhiWeighted->Write();
  h2TotalMBDCaloE->Write();
  out -> Close();
  delete out;

  out2 -> cd();
  T->Write();
  out2 -> Close();
  delete out2;

  //hm -> dumpHistos(Outfile.c_str(), "UPDATE");
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int caloTreeGen::Reset(PHCompositeNode *topNode)
{
  std::cout << "caloTreeGen::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void caloTreeGen::Print(const std::string &what) const
{
  std::cout << "caloTreeGen::Print(const std::string &what) const Printing info for " << what << std::endl;
}
// convert from calorimeter key to phi bin
//____________________________________________________________________________..
unsigned int caloTreeGen::getCaloTowerPhiBin(const unsigned int key)
{
  unsigned int etabin = key >> 16U;
  unsigned int phibin = key - (etabin << 16U);
  return phibin;
}

//____________________________________________________________________________..
// convert from calorimeter key to eta bin
unsigned int caloTreeGen::getCaloTowerEtaBin(const unsigned int key)
{
  unsigned int etabin = key >> 16U;
  return etabin;
}
//____________________________________________________________________________..
float caloTreeGen::getMaxTowerE(RawCluster *cluster, TowerInfoContainer *towerContainer)
{
  TowerInfo* tower = getMaxTower(cluster, towerContainer);

  return tower->get_energy();
}
//____________________________________________________________________________..
std::vector<int> caloTreeGen::returnClusterTowEta(RawCluster *cluster, TowerInfoContainer *towerContainer)
{
  RawCluster::TowerConstRange towers = cluster -> get_towers();
  RawCluster::TowerConstIterator toweriter;
  
  std::vector<int> towerIDsEta;
  for(toweriter = towers.first; toweriter != towers.second; toweriter++) towerIDsEta.push_back(RawTowerDefs::decode_index1(toweriter -> first));

  return towerIDsEta;
}
//____________________________________________________________________________..
std::vector<int> caloTreeGen::returnClusterTowPhi(RawCluster *cluster, TowerInfoContainer *towerContainer)
{
  RawCluster::TowerConstRange towers = cluster -> get_towers();
  RawCluster::TowerConstIterator toweriter;
  
  std::vector<int> towerIDsPhi;
  for(toweriter = towers.first; toweriter != towers.second; toweriter++) towerIDsPhi.push_back(RawTowerDefs::decode_index2(toweriter -> first));
  return towerIDsPhi;
}
//____________________________________________________________________________..
std::vector<float> caloTreeGen::returnClusterTowE(RawCluster *cluster, TowerInfoContainer *towerContainer)
{
  RawCluster::TowerConstRange towers = cluster -> get_towers();
  RawCluster::TowerConstIterator toweriter;
  
  std::vector<float> towerE;
  for(toweriter = towers.first; toweriter != towers.second; toweriter++) towerE.push_back(toweriter -> second);
  
  return towerE;
}

TowerInfo* caloTreeGen::getTower(RawTowerDefs::keytype key, TowerInfoContainer *towerContainer) {
      if(!towerContainer) return nullptr; // check if towerContainer is null

      int ieta = RawTowerDefs::decode_index1(key);  // index1 is eta in CYL
      int iphi = RawTowerDefs::decode_index2(key);  // index2 is phi in CYL
      unsigned int towerkey = TowerInfoDefs::encode_emcal(ieta,iphi);

      return towerContainer->get_tower_at_key(towerkey);
}


TowerInfo* caloTreeGen::getMaxTower(RawCluster *cluster, TowerInfoContainer *towerContainer) {
  RawCluster::TowerConstRange towers = cluster -> get_towers();
  RawCluster::TowerConstIterator toweriter;

  float maxEnergy = 0;
  RawTowerDefs::keytype key = 0;
  for(toweriter = towers.first; toweriter != towers.second; toweriter++)
  {
    float towE = toweriter -> second;

    if(towE > maxEnergy) {
      maxEnergy = towE;
      key = toweriter->first;
    }
  }
  return getTower(key, towerContainer);
}

Short_t caloTreeGen::getMaxTowerTime(RawCluster *cluster, TowerInfoContainer *towerContainer) {
  TowerInfo* tower = getMaxTower(cluster, towerContainer);

  return tower->get_time();
}
