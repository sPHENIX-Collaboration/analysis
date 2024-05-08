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

// MBD
#include <mbd/MbdPmtContainerV1.h>
#include <mbd/MbdPmtHit.h>
#include <mbd/MbdGeom.h>

//Tower stuff
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfo.h>

// z-vertex stuff
#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>

// Centrality
#include <centrality/CentralityInfo.h>

// Minimum Bias
#include <calotrigger/MinimumBiasInfo.h>

//____________________________________________________________________________..
caloTreeGen::caloTreeGen(const std::string &name):
  SubsysReco(name)
  ,T(nullptr)
  ,out(nullptr)
  ,out2(nullptr)
{
  std::cout << "caloTreeGen::caloTreeGen(const std::string &name) Calling ctor" << std::endl;
}

//____________________________________________________________________________..
caloTreeGen::~caloTreeGen()
{
  std::cout << "caloTreeGen::~caloTreeGen() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
Int_t caloTreeGen::Init(PHCompositeNode *topNode)
{
  out = new TFile(Outfile.c_str(),"RECREATE");

  hVtxZ = new TH1F("hVtxZ", "Event z-vertex; z [cm]; Counts", bins_vtx_z, low_vtx_z, high_vtx_z);
  hVtxZv2 = new TH1F("hVtxZv2", "Event z-vertex with MinBias only; z [cm]; Counts", bins_vtx_z, low_vtx_z, high_vtx_z);
  hTowE = new TH1F("hTowE", "Tower Energy; Energy [GeV]; Counts", bins_towE, low_towE, high_towE);
  hClusterECore = new TH1F("hClusterECore", "Cluster ECore; Energy [GeV]; Counts", bins_e, low_e, high_e);
  hClusterPt = new TH1F("hClusterPt", "Cluster p_{T}; p_{T} [GeV]; Counts", bins_pt, low_pt, high_pt);
  hClusterChi = new TH1F("hClusterChi", "Cluster #chi^{2}; #chi^{2}; Counts", bins_chi, low_chi, high_chi);
  // hClusterTime = new TH1F("hClusterTime", "Cluster Time; Time; Counts", bins_time, low_time, high_time);
  hNClusters = new TH1F("hNClusters", "Cluster; # of clusters per event; Counts", bins_n, low_n, high_n);
  hTotalMBD = new TH1F("hTotalMBD", "MBD Charge; MBD Charge; Counts", bins_totalmbd, low_totalmbd, high_totalmbd);
  hCentrality = new TH1F("hCentrality", "Centrality; Centrality; Counts", bins_cent, low_cent, high_cent);
  hTotalCaloE = new TH1F("hTotalCaloE", "Total Calo E; Total Calo E [GeV]; Counts", bins_totalcaloE, low_totalcaloE, high_totalcaloE);
  hBadPMTs = new TH1F("hBadPMTs", "PMTs with charge nan; # of nan PMTs per Event; Counts", bins_nPMTs, low_nPMTs, high_nPMTs);

  h2ClusterEtaPhi = new TH2F("h2ClusterEtaPhi", "Cluster; #eta; #phi", bins_eta, low_eta, high_eta, bins_phi, low_phi, high_phi);
  h2ClusterEtaPhiWeighted = new TH2F("h2ClusterEtaPhiWeighted", "Cluster ECore; #eta; #phi", bins_eta, low_eta, high_eta, bins_phi, low_phi, high_phi);
  h2TowEtaPhiWeighted = new TH2F("h2TowEtaPhiWeighted", "Tower Energy; Towerid #eta; Towerid #phi",  bins_eta, 0, bins_eta, bins_phi, 0, bins_phi);
  h2TotalMBDCaloE = new TH2F("h2TotalMBDCaloE", "Total MBD Charge vs Total EMCAL Energy; Total EMCAL Energy [Arb]; Total MBD Charge [Arb]", 100, 0, 1, 100, 0, 1);
  h2TotalMBDCentrality = new TH2F("h2TotalMBDCentrality", "Total MBD Charge vs Centrality; Centrality; Total MBD Charge [Arb]", bins_cent, low_cent, high_cent, 100, 0, 1);
  h2TotalMBDCaloEv2 = new TH2F("h2TotalMBDCaloEv2", "Total MBD Charge vs Total EMCAL Energy; Total EMCAL Energy; Total MBD Charge", bins_totalcaloEv2, low_totalcaloEv2, high_totalcaloEv2,
                                                                                                                                    bins_totalmbdv2, low_totalmbdv2, high_totalmbdv2);
  if(isSim) {
    hImpactPar            = new TH1F("hImpactPar", "Impact Parameter; b [fm]; Counts", bins_b, low_b, high_b);
    h2ImpactParCentrality = new TH2F("h2ImpactParCentrality", "Impact Parameter vs Centrality; Centrality; b [fm]", bins_cent, low_cent, high_cent, bins_b, low_b, high_b);
  }

  out2 = new TFile(Outfile2.c_str(),"RECREATE");

  T = new TTree("T", "T");
  T->Branch("run",       &run,      "run/I");
  T->Branch("event",     &event,    "event/I");
  T->Branch("totalMBD",  &totalMBD, "totalMBD/F");
  T->Branch("centrality",&cent,     "centrality/F");
  T->Branch("vtx_z",     &vtx_z,    "vtx_z/F");
  T->Branch("Q2_S_x",    &Q2_S_x,   "Q2_S_x/F");
  T->Branch("Q2_S_y",    &Q2_S_y,   "Q2_S_y/F");
  T->Branch("Q2_N_x",    &Q2_N_x,   "Q2_N_x/F");
  T->Branch("Q2_N_y",    &Q2_N_y,   "Q2_N_y/F");
  T->Branch("Q3_S_x",    &Q3_S_x,   "Q3_S_x/F");
  T->Branch("Q3_S_y",    &Q3_S_y,   "Q3_S_y/F");
  T->Branch("Q3_N_x",    &Q3_N_x,   "Q3_N_x/F");
  T->Branch("Q3_N_y",    &Q3_N_y,   "Q3_N_y/F");
  T->Branch("pi0_phi",   &pi0_phi_vec);
  T->Branch("pi0_eta",   &pi0_eta_vec);
  T->Branch("pi0_pt",    &pi0_pt_vec);
  T->Branch("pi0_mass",  &pi0_mass_vec);
  T->Branch("asym",      &asym_vec);
  T->Branch("deltaR",    &deltaR_vec);
  T->Branch("ecore1",    &ecore1_vec);
  T->Branch("ecore2",    &ecore2_vec);
  T->Branch("chi2_max",  &chi2_max_vec);
  T->Branch("isFarNorth",&isFarNorth_vec);

  // keep track of impact parameter in simulation
  if(isSim) {
    T->Branch("b", &b, "b/F");
  }

  //so that the histos actually get written out
  Fun4AllServer *se = Fun4AllServer::instance();
  se -> Print("NODETREE"); 
  
  std::cout << "caloTreeGen::Init(PHCompositeNode *topNode) Initializing" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
Int_t caloTreeGen::InitRun(PHCompositeNode *topNode)
{
  std::cout << "caloTreeGen::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
Int_t caloTreeGen::process_event(PHCompositeNode *topNode)
{
  EventHeader* eventInfo = findNode::getClass<EventHeader>(topNode,"EventHeader");
  if(!eventInfo)
  {
    std::cout << PHWHERE << "caloTreeGen::process_event - Fatal Error - EventHeader node is missing. " << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  event = eventInfo->get_EvtSequence();
  run   = eventInfo->get_RunNumber();
  // std::cout << "Event: " << event->get_EvtSequence() << ", Run: " << event->get_RunNumber() << std::endl;

  if(iEvent%20 == 0) std::cout << "Progress: " << iEvent << std::endl;
  iEvent++;

  //Information on clusters
  RawClusterContainer *clusterContainer = findNode::getClass<RawClusterContainer>(topNode,clusterNode.c_str());

  if(!clusterContainer)
  {
    std::cout << PHWHERE << "caloTreeGen::process_event Could not find node: " <<  clusterNode << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }
  
  //tower information
  TowerInfoContainer *emcTowerContainer;
  emcTowerContainer = findNode::getClass<TowerInfoContainer>(topNode,towerNode.c_str());
  if(!emcTowerContainer)
  {
    std::cout << PHWHERE << "caloTreeGen::process_event Could not find node: " <<  towerNode << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  //Tower geometry node for location information
  RawTowerGeomContainer *towergeom = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_CEMC");
  if (!towergeom)
  {
    std::cout << PHWHERE << "caloTreeGen::process_event Could not find node TOWERGEOM_CEMC"  << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }
  
  MbdPmtContainer *mbdpmts = findNode::getClass<MbdPmtContainer>(topNode,"MbdPmtContainer"); // mbd info
  if(!mbdpmts)
  {
    std::cout << PHWHERE << "caloTreeGen::process_event: Could not find node MbdPmtContainer" << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  MbdGeom *mbdgeom = findNode::getClass<MbdGeom>(topNode,"MbdGeom"); // mbd geometry
  if(!mbdgeom)
  {
    std::cout << PHWHERE << "caloTreeGen::process_event: Could not find node MbdGeom" << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  CentralityInfo *centInfo = findNode::getClass<CentralityInfo>(topNode,"CentralityInfo");
  if(!centInfo)
  {
    std::cout << PHWHERE << "caloTreeGen::process_event: Could not find node CentralityInfo" << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  // simulation gives centrality values in 0 - 100
  // data gives centrality values in 0 - 1
  cent = (isSim) ? centInfo->get_centile(CentralityInfo::PROP::mbd_NS)/100. :
                   centInfo->get_centile(CentralityInfo::PROP::mbd_NS);

  // grab impact parameter in MC
  if(isSim) {
    b = centInfo->get_quantity(CentralityInfo::PROP::bimp);
  }

  //----------------------------------vertex------------------------------------------------------//
  GlobalVertexMap* vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
  if (!vertexmap || vertexmap->empty() || !(vertexmap->begin()->second))
  {
    if(!vertexmap)              std::cout << PHWHERE << "caloTreeGen::process_event: Could not find node GlobalVertexMap" << std::endl;
    else if(vertexmap->empty()) std::cout << PHWHERE << "caloTreeGen::process_event: GlobalVertexMap is empty" << std::endl;
    else                        std::cout << PHWHERE << "caloTreeGen::process_event: GlobalVertex is null" << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  GlobalVertex* vtx = vertexmap->begin()->second;
  vtx_z = vtx->get_z();
  min_vtx_z = std::min(min_vtx_z, vtx_z);
  max_vtx_z = std::max(max_vtx_z, vtx_z);
  hVtxZ->Fill(vtx_z);
  //----------------------------------vertex------------------------------------------------------//

  MinimumBiasInfo *minBiasInfo = findNode::getClass<MinimumBiasInfo>(topNode,"MinimumBiasInfo");
  Bool_t isMinBias = (minBiasInfo) ? minBiasInfo->isAuAuMinimumBias() : false;
  if(!isMinBias)
  {
    std::cout << PHWHERE << "caloTreeGen::process_event: " << event << " is not MinimumBias" << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  hVtxZv2->Fill(vtx_z);

  // reject events that do not pass the z-vertex cuts
  if(abs(vtx_z) >= vtx_z_max) {
    std::cout << PHWHERE << "|z-vertex| >= " << vtx_z_max << ": " << vtx_z << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  Float_t charge_S = 0;
  Float_t charge_N = 0;

  Int_t nPMTs = (!isSim) ? mbdpmts -> get_npmt() : 128; //size (should always be 128)

  Int_t badPMTs = 0;
  for(Int_t i = 0; i < nPMTs; ++i) {
    MbdPmtHit* mbdpmt = mbdpmts->get_pmt(i);
    Float_t charge    = mbdpmt->get_q();     //pmt charge

    // skip PMT if the charge is nan
    if(std::isnan(charge)) {
      ++badPMTs;
      continue;
    }

    Float_t phi       = mbdgeom->get_phi(i); //pmt phi
    Float_t z         = mbdgeom->get_z(i);   //pmt z ~ eta

    Float_t x2 = charge*std::cos(2*phi);
    Float_t y2 = charge*std::sin(2*phi);
    Float_t x3 = charge*std::cos(3*phi);
    Float_t y3 = charge*std::sin(3*phi);

    if(z < 0) {
      Q2_S_x   += x2;
      Q2_S_y   += y2;
      Q3_S_x   += x3;
      Q3_S_y   += y3;
      charge_S += charge;
    }
    else {
      Q2_N_x   += x2;
      Q2_N_y   += y2;
      Q3_N_x   += x3;
      Q3_N_y   += y3;
      charge_N += charge;
    }

    totalMBD += charge;
  }

  if(badPMTs) {
    std::cout << "Bad PMT in Event: " << iEvent << std::endl;
  }

  Q2_S_x = (charge_S) ? Q2_S_x/charge_S : 0;
  Q2_S_y = (charge_S) ? Q2_S_y/charge_S : 0;
  Q2_N_x = (charge_N) ? Q2_N_x/charge_N : 0;
  Q2_N_y = (charge_N) ? Q2_N_y/charge_N : 0;
  Q3_S_x = (charge_S) ? Q3_S_x/charge_S : 0;
  Q3_S_y = (charge_S) ? Q3_S_y/charge_S : 0;
  Q3_N_x = (charge_N) ? Q3_N_x/charge_N : 0;
  Q3_N_y = (charge_N) ? Q3_N_y/charge_N : 0;

  max_totalmbd = std::max(max_totalmbd, totalMBD);
  hTotalMBD->Fill(totalMBD);

  //grab all the towers and fill their energies. 
  UInt_t tower_range = emcTowerContainer->size();
  Float_t totalCaloE = 0;
  Int_t goodTowerCtr = 0;
  for(UInt_t iter = 0; iter < tower_range; iter++)
  {
    UInt_t towerkey = emcTowerContainer->encode_key(iter);
    UInt_t ieta = getCaloTowerEtaBin(towerkey);
    UInt_t iphi = getCaloTowerPhiBin(towerkey);

    TowerInfo* tower = emcTowerContainer -> get_tower_at_channel(iter);
    double energy = tower -> get_energy();

    // check if tower is good
    if(!tower->get_isGood()) continue;

    ++goodTowerCtr;

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
    if(ieta >= bins_eta || iphi >= bins_phi) std::cout << "ieta: " << ieta << ", iphi: " << iphi << std::endl;

    if(!isSim || (isSim && ieta >= 8)) h2TowEtaPhiWeighted->Fill(ieta, iphi, energy);
  }

  avg_goodTowers += goodTowerCtr*1./tower_range;

  max_totalCaloE = std::max(max_totalCaloE, totalCaloE);
  min_totalCaloE = std::min(min_totalCaloE, totalCaloE);

  if(totalCaloE <= 0) {
    max_totalmbd2 = std::max(max_totalmbd2, totalMBD);
    h2TotalMBDCaloEv2->Fill(totalCaloE, totalMBD);
    std::cout << PHWHERE << "caloTreeGen::process_event totalCaloE <= 0: " << totalCaloE  << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  if(isSim) {
    hImpactPar->Fill(b);
    h2ImpactParCentrality->Fill(cent, b);
  }

  hTotalCaloE->Fill(totalCaloE);
  h2TotalMBDCaloE->Fill(totalCaloE/high_totalcaloE, totalMBD/high_totalmbd);
  h2TotalMBDCentrality->Fill(cent, totalMBD/high_totalmbd);
  hCentrality->Fill(cent);
  hBadPMTs->Fill(badPMTs);

  min_cent = std::min(min_cent, cent);
  max_cent = std::max(max_cent, cent);

  min_b = std::min(min_b, b);
  max_b = std::max(max_b, b);

  if(cent < 0 || cent > 1) std::cout << "Centrality: " << cent << ", totalMBD: " << totalMBD << std::endl;

  max_NClusters = std::max(max_NClusters, clusterContainer->size());
  hNClusters->Fill(clusterContainer->size());

  RawClusterContainer::ConstRange clusterEnd = clusterContainer -> getClusters();
  RawClusterContainer::ConstIterator clusterIter;
  RawClusterContainer::ConstIterator clusterIter2;

  CLHEP::Hep3Vector vertex(0,0,vtx_z);
  // loop over all clusters in event
  for(clusterIter = clusterEnd.first; clusterIter != clusterEnd.second; clusterIter++)
  {
    RawCluster *recoCluster = clusterIter -> second;

    CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetECoreVec(*recoCluster, vertex);
    // CLHEP::Hep3Vector E_vec_cluster_Full = RawClusterUtility::GetEVec(*recoCluster, vertex);

    Float_t clusE = E_vec_cluster.mag();
    Float_t clus_eta = E_vec_cluster.pseudoRapidity();
    Float_t clus_phi = E_vec_cluster.phi();
    Float_t clus_pt = E_vec_cluster.perp();
    Float_t clus_chi = recoCluster -> get_chi2();
    // Float_t nTowers = recoCluster ->getNTowers();
    // Float_t maxTowerEnergy = getMaxTowerE(recoCluster,emcTowerContainer);

    min_clusterECore = std::min(min_clusterECore, clusE);
    min_clusterEta   = std::min(min_clusterEta, clus_eta);
    min_clusterPhi   = std::min(min_clusterPhi, clus_phi);
    min_clusterPt    = std::min(min_clusterPt, clus_pt);
    min_clusterChi   = std::min(min_clusterChi, clus_chi);

    max_clusterECore = std::max(max_clusterECore, clusE);
    max_clusterEta   = std::max(max_clusterEta, clus_eta);
    max_clusterPhi   = std::max(max_clusterPhi, clus_phi);
    max_clusterPt    = std::max(max_clusterPt, clus_pt);
    max_clusterChi   = std::max(max_clusterChi, clus_chi);

    if(clusE >= clusE_min) {
      hClusterChi->Fill(clus_chi);
    }

    if(clusE < clusE_min || clus_chi >= clus_chi_max) continue;

    // in simulation exclude cluster that comes from uninstrumented IB
    if(isSim && hasTowerFar(recoCluster, isSim)) continue;

    // Float_t clus_time = getMaxTowerTime(recoCluster, emcTowerContainer);

    hClusterECore->Fill(clusE);
    h2ClusterEtaPhi->Fill(clus_eta, clus_phi);
    h2ClusterEtaPhiWeighted->Fill(clus_eta, clus_phi, clusE);
    hClusterPt->Fill(clus_pt);
    // hClusterTime->Fill(clus_time);

    if(!do_pi0_ana) continue;

    TLorentzVector photon1;
    photon1.SetPtEtaPhiE(clus_pt, clus_eta, clus_phi, clusE);

    for(clusterIter2 = std::next(clusterIter); clusterIter2 != clusterEnd.second; clusterIter2++) {

      if(clusterIter2 == clusterIter) std::cout << "Error: clusterIter2 == clusterIter" << std::endl;

      RawCluster *recoCluster2 = clusterIter2 -> second;

      CLHEP::Hep3Vector E_vec_cluster2 = RawClusterUtility::GetECoreVec(*recoCluster2, vertex);

      Float_t clusE2 = E_vec_cluster2.mag();
      Float_t clus_eta2 = E_vec_cluster2.pseudoRapidity();
      Float_t clus_phi2 = E_vec_cluster2.phi();
      Float_t clus_pt2 = E_vec_cluster2.perp();
      Float_t clus_chi2 = recoCluster2->get_chi2();

      if(clusE2 < clusE_min || clus_chi2 >= clus_chi_max) continue;

      // Float_t clus_time2 = getMaxTowerTime(recoCluster2, emcTowerContainer);

      TLorentzVector photon2;
      photon2.SetPtEtaPhiE(clus_pt2, clus_eta2, clus_phi2, clusE2);

      TLorentzVector pi0 = photon1 + photon2;

      Float_t pi0_mass = pi0.M();
      Float_t pi0_pt = pi0.Pt();
      Float_t pi0_eta = pi0.Eta();
      Float_t pi0_phi = pi0.Phi();

      // exclude pi0 mass if it's more than 1 GeV
      // saves storage
      if(pi0_mass >= 0.7) continue;

      // in simulation exclude cluster that comes from uninstrumented IB
      if(isSim && hasTowerFar(recoCluster2, isSim)) continue;

      Float_t asym      = abs(clusE-clusE2)/(clusE+clusE2);
      Float_t chi2_max  = std::max(clus_chi, clus_chi2);
      Float_t deltaR    = E_vec_cluster.deltaR(E_vec_cluster2);

      // check if either cluster has a tower that is in the last IB board: i.e ieta >= 88
      Bool_t isFarNorth = hasTowerFar(recoCluster) || hasTowerFar(recoCluster2);

      pi0_phi_vec.push_back(pi0_phi);
      pi0_eta_vec.push_back(pi0_eta);
      pi0_pt_vec.push_back(pi0_pt);
      pi0_mass_vec.push_back(pi0_mass);
      asym_vec.push_back(asym);
      deltaR_vec.push_back(deltaR);
      ecore1_vec.push_back(clusE);
      ecore2_vec.push_back(clusE2);
      chi2_max_vec.push_back(chi2_max);
      isFarNorth_vec.push_back(isFarNorth);
    }
  }

  T->Fill();

  ++iEventGood;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
Int_t caloTreeGen::ResetEvent(PHCompositeNode *topNode)
{
  totalMBD = 0;
  Q2_S_x   = 0;
  Q2_S_y   = 0;
  Q2_N_x   = 0;
  Q2_N_y   = 0;
  Q3_S_x   = 0;
  Q3_S_y   = 0;
  Q3_N_x   = 0;
  Q3_N_y   = 0;

  pi0_phi_vec.clear();
  pi0_eta_vec.clear();
  pi0_pt_vec.clear();
  pi0_mass_vec.clear();
  asym_vec.clear();
  deltaR_vec.clear();
  ecore1_vec.clear();
  ecore2_vec.clear();
  chi2_max_vec.clear();
  isFarNorth_vec.clear();

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
Int_t caloTreeGen::EndRun(const Int_t runnumber)
{
  std::cout << "caloTreeGen::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
Int_t caloTreeGen::End(PHCompositeNode *topNode)
{

  std::cout << "Total Events: " << iEvent << ", Accepted Events: " << iEventGood << ", " << iEventGood*100./iEvent << " %" << std::endl;
  std::cout << "Bad PMT Events: " << hBadPMTs->Integral(2,bins_nPMTs) << std::endl;
  std::cout << "min z-vertex: " << min_vtx_z << ", max z-vertex: " << max_vtx_z << std::endl;
  std::cout << "min centrality: " << min_cent << ", max centrality: " << max_cent << std::endl;
  std::cout << "min b [fm]: " << min_b << ", max b [fm]: " << max_b << std::endl;
  std::cout << "min totalCaloE: " << min_totalCaloE << ", max totalCaloE: " << max_totalCaloE << std::endl;
  std::cout << "max totalmbd: " << max_totalmbd << ", max totalmbd (for totalCaloE < 0): " << max_totalmbd2 << std::endl;
  std::cout << "min tower energy: " << min_towE << ", max tower energy: " << max_towE << std::endl;
  std::cout << "min clusterECore: " << min_clusterECore << ", max clusterECore: " << max_clusterECore << std::endl;
  std::cout << "min clusterEta: " << min_clusterEta << ", max clusterEta: " << max_clusterEta << std::endl;
  std::cout << "min clusterPhi: " << min_clusterPhi << ", max clusterPhi: " << max_clusterPhi << std::endl;
  std::cout << "min clusterPt: " << min_clusterPt << ", max clusterPt: " << max_clusterPt << std::endl;
  std::cout << "min clusterChi: " << min_clusterChi << ", max clusterChi: " << max_clusterChi << std::endl;
  std::cout << "max NClusters: " << max_NClusters << std::endl;
  std::cout << "avg_goodTowers: " << avg_goodTowers*100./iEventGood << " %" << std::endl;
  std::cout << "pi0 cuts: cluster minimum energy: " << clusE_min << ", cluster maximum chi2: " << clus_chi_max << std::endl;

  std::cout << "caloTreeGen::End(PHCompositeNode *topNode) This is the End..." << std::endl;

  out -> cd();

  hBadPMTs->Write();
  hVtxZ->Write();
  hVtxZv2->Write();
  hTotalMBD->Write();
  hCentrality->Write();
  if(isSim) {
    hImpactPar->Write();
    h2ImpactParCentrality->Write();
  }
  hTotalCaloE->Write();
  hTowE->Write();
  hClusterECore->Write();
  hClusterPt->Write();
  hClusterChi->Write();
  // hClusterTime->Write();
  hNClusters->Write();
  h2ClusterEtaPhi->Write();
  h2ClusterEtaPhiWeighted->Write();
  h2TowEtaPhiWeighted->Write();
  h2TotalMBDCaloE->Write();
  h2TotalMBDCentrality->Write();
  h2TotalMBDCaloEv2->Write();
  out -> Close();
  delete out;

  out2 -> cd();
  // T->BuildIndex("run", "event");
  T->Write();
  out2 -> Close();
  delete out2;

  //hm -> dumpHistos(Outfile.c_str(), "UPDATE");
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
Int_t caloTreeGen::Reset(PHCompositeNode *topNode)
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
UInt_t caloTreeGen::getCaloTowerPhiBin(const UInt_t key)
{
  UInt_t etabin = key >> 16U;
  UInt_t phibin = key - (etabin << 16U);
  return phibin;
}

//____________________________________________________________________________..
// convert from calorimeter key to eta bin
UInt_t caloTreeGen::getCaloTowerEtaBin(const UInt_t key)
{
  UInt_t etabin = key >> 16U;
  return etabin;
}
//____________________________________________________________________________..
Float_t caloTreeGen::getMaxTowerE(RawCluster *cluster, TowerInfoContainer *towerContainer)
{
  TowerInfo* tower = getMaxTower(cluster, towerContainer);

  return tower->get_energy();
}
//____________________________________________________________________________..
Bool_t caloTreeGen::hasTowerFar(RawCluster *cluster, Bool_t isSim)
{
  RawCluster::TowerConstRange towers = cluster -> get_towers();
  RawCluster::TowerConstIterator toweriter;

  std::vector<Int_t> towerIDsEta;
  for(toweriter = towers.first; toweriter != towers.second; toweriter++) {
    Int_t ieta = RawTowerDefs::decode_index1(toweriter -> first);
    // check if the tower in part of the last IB
    if(!isSim && ieta >= 88) return true;
    if(isSim && ieta <= 8) return true;
  }

  return false;
}
//____________________________________________________________________________..
std::vector<Int_t> caloTreeGen::returnClusterTowEta(RawCluster *cluster)
{
  RawCluster::TowerConstRange towers = cluster -> get_towers();
  RawCluster::TowerConstIterator toweriter;
  
  std::vector<Int_t> towerIDsEta;
  for(toweriter = towers.first; toweriter != towers.second; toweriter++) towerIDsEta.push_back(RawTowerDefs::decode_index1(toweriter -> first));

  return towerIDsEta;
}
//____________________________________________________________________________..
std::vector<Int_t> caloTreeGen::returnClusterTowPhi(RawCluster *cluster)
{
  RawCluster::TowerConstRange towers = cluster -> get_towers();
  RawCluster::TowerConstIterator toweriter;
  
  std::vector<Int_t> towerIDsPhi;
  for(toweriter = towers.first; toweriter != towers.second; toweriter++) towerIDsPhi.push_back(RawTowerDefs::decode_index2(toweriter -> first));
  return towerIDsPhi;
}
//____________________________________________________________________________..
std::vector<Float_t> caloTreeGen::returnClusterTowE(RawCluster *cluster)
{
  RawCluster::TowerConstRange towers = cluster -> get_towers();
  RawCluster::TowerConstIterator toweriter;
  
  std::vector<Float_t> towerE;
  for(toweriter = towers.first; toweriter != towers.second; toweriter++) towerE.push_back(toweriter -> second);
  
  return towerE;
}

TowerInfo* caloTreeGen::getTower(RawTowerDefs::keytype key, TowerInfoContainer *towerContainer) {
      if(!towerContainer) return nullptr; // check if towerContainer is null

      Int_t ieta = RawTowerDefs::decode_index1(key);  // index1 is eta in CYL
      Int_t iphi = RawTowerDefs::decode_index2(key);  // index2 is phi in CYL
      UInt_t towerkey = TowerInfoDefs::encode_emcal(ieta,iphi);

      return towerContainer->get_tower_at_key(towerkey);
}


TowerInfo* caloTreeGen::getMaxTower(RawCluster *cluster, TowerInfoContainer *towerContainer) {
  RawCluster::TowerConstRange towers = cluster -> get_towers();
  RawCluster::TowerConstIterator toweriter;

  Float_t maxEnergy = 0;
  RawTowerDefs::keytype key = 0;
  for(toweriter = towers.first; toweriter != towers.second; toweriter++)
  {
    Float_t towE = toweriter -> second;

    if(towE > maxEnergy) {
      maxEnergy = towE;
      key = toweriter->first;
    }
  }
  return getTower(key, towerContainer);
}

Float_t caloTreeGen::getMaxTowerTime(RawCluster *cluster, TowerInfoContainer *towerContainer) {
  TowerInfo* tower = getMaxTower(cluster, towerContainer);

  return tower->get_time_float();
}
