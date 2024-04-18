//our base code
#include "neutralMesonTSSA.h"

//Fun4all stuff
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
#include <TH3F.h>
#include <TFile.h>
#include <TLorentzVector.h>

//for emc clusters
#include <calobase/RawCluster.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawClusterUtility.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/RawTower.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/TowerInfo.h>
#include <calobase/TowerInfoContainer.h>

// Minimum Bias
/* #include <calotrigger/MinimumBiasInfo.h> */

//for vertex information
#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>

//truth information
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4VtxPoint.h>

neutralMesonTSSA::neutralMesonTSSA(const std::string &name, bool isMC):
 SubsysReco(name),
 isMonteCarlo(isMC),
 m_clusterContainer(0),
 m_truthInfo(0),
 gVtx(0),
 mcVtx(0),
 histfile(0),
 h_diphotonMass(0),
 h_pi0Mass(0),
 h_etaMass(0),
 min_clusterE(0.3),
 max_clusterChi2(4.0),
 goodclusters_E(0),
 goodclusters_Eta(0),
 goodclusters_Phi(0),
 goodclusters_Ecore(0),
 goodclusters_Chi2(0),
 min_mesonPt(0.5),
 pi0MassMean(0.142),
 pi0MassSigma(0.0168),
 etaMassMean(0.564),
 etaMassSigma(0.0200),
 pi0_E(0),
 pi0_M(0),
 pi0_Eta(0),
 pi0_Phi(0),
 pi0_Pt(0),
 pi0_xF(0),
 eta_E(0),
 eta_M(0),
 eta_Eta(0),
 eta_Phi(0),
 eta_Pt(0),
 eta_xF(0),
 n_events_total(0),
 n_events_with_vertex(0),
 n_events_with_good_vertex(0),
 n_events_positiveCaloE(0),
 N_pi0_up_left(0),
 N_pi0_up_right(0),
 N_pi0_down_left(0),
 N_pi0_down_right(0),
 N_eta_up_left(0),
 N_eta_up_right(0),
 N_eta_down_left(0),
 N_eta_down_right(0)
{
  std::cout << "neutralMesonTSSA::neutralMesonTSSA(const std::string &name) Calling ctor" << std::endl;
}

//____________________________________________________________________________..
neutralMesonTSSA::~neutralMesonTSSA()
{
  std::cout << "neutralMesonTSSA::~neutralMesonTSSA() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
int neutralMesonTSSA::Init(PHCompositeNode *topNode)
{
  std::cout << "neutralMesonTSSA::Init(PHCompositeNode *topNode) Initializing" << std::endl;

  min_pi0Mass = pi0MassMean - 2.5*pi0MassSigma;
  max_pi0Mass = pi0MassMean + 2.5*pi0MassSigma;
  min_etaMass = etaMassMean - 2.5*etaMassSigma;
  max_etaMass = etaMassMean + 2.5*etaMassSigma;

  histfile = new TFile("neutralMesonTSSA_hists.root", "RECREATE");
  histfile->cd();
  h_diphotonMass = new TH1F("h_diphotonMass", "Diphoton Mass Distribution;Diphoton Mass (GeV);Counts", 100, 0.0, 1.0);
  h_pi0Mass = new TH1F("h_pi0Mass", "#pi^{0} Mass Distribution;#pi^{0} Mass (GeV);Counts", 100, 0.0, 1.0);
  h_etaMass = new TH1F("h_etaMass", "#eta Mass Distribution;#eta Mass (GeV);Counts", 100, 0.0, 1.0);
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int neutralMesonTSSA::InitRun(PHCompositeNode *topNode)
{
  std::cout << "neutralMesonTSSA::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int neutralMesonTSSA::process_event(PHCompositeNode *topNode)
{
  /* std::cout << "neutralMesonTSSA::process_event(PHCompositeNode *topNode) Processing Event" << std::endl; */
  n_events_total++;

  // Information on clusters
  // Name of node is different in MC and RD
  if (isMonteCarlo) {
      m_clusterContainer = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_POS_COR_CEMC");
  }
  else {
      m_clusterContainer = findNode::getClass<RawClusterContainer>(topNode,"CLUSTERINFO_CEMC");
  }
  if(!m_clusterContainer)
  {
      if (isMonteCarlo) std::cout << PHWHERE << "neutralMesonTSSA::process_event - Fatal Error - CLUSTER_POS_COR_CEMC node is missing. " << std::endl;
      else std::cout << PHWHERE << "neutralMesonTSSA::process_event - Fatal Error - CLUSTERINFO_CEMC node is missing. " << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
  }

  // Raw tower information -- used for checking if total calo E > 0
  RawTowerContainer *towerContainer = nullptr;
  TowerInfoContainer *towerInfoContainer = nullptr;
  // again, node has different names in MC and RD
  if (isMonteCarlo) {
      towerContainer = findNode::getClass<RawTowerContainer>(topNode,"TOWER_CALIB_CEMC");
      if(!towerContainer) {
	  std::cout << PHWHERE << "neutralMesonTSSA::process_event Could not find node TOWER_CALIB_CEMC"  << std::endl;
	  return Fun4AllReturnCodes::ABORTEVENT;
      }
  }
  else {
      towerInfoContainer = findNode::getClass<TowerInfoContainer>(topNode,"TOWERINFO_CALIB_CEMC");
      if (!towerInfoContainer) {
	  std::cout << PHWHERE << "neutralMesonTSSA::process_event Could not find node TOWERINFO_CALIB_CEMC"  << std::endl;
	  return Fun4AllReturnCodes::ABORTEVENT;
      }
  }

  // Truth information
  /* PHG4TruthInfoContainer *truthinfo = nullptr; */
  if (isMonteCarlo) {
      m_truthInfo = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
      if(!m_truthInfo) {
	  std::cout << PHWHERE << "neutralMesonTSSA::process_event Could not find node G4TruthInfo"  << std::endl;
	  return Fun4AllReturnCodes::ABORTEVENT;
      }
  }

  //Vertex information
  // Problem is MC has a PrimaryVtx but no GlobalVertex, while RD has the opposite
  if (isMonteCarlo) {
      PHG4TruthInfoContainer::VtxRange vtx_range = m_truthInfo->GetPrimaryVtxRange();
      PHG4TruthInfoContainer::ConstVtxIterator vtxIter = vtx_range.first;
      mcVtx = vtxIter->second;
      n_events_with_vertex++;
      if (abs(mcVtx->get_z()) > 10.0) {
	  /* std::cout << "Greg info: vertex z is " << mcVtx->get_z() << "\n"; */
	  return Fun4AllReturnCodes::ABORTEVENT;
      }
      n_events_with_good_vertex++;
  }
  else {
      GlobalVertexMap *vtxContainer = findNode::getClass<GlobalVertexMap>(topNode,"GlobalVertexMap");
      if (!vtxContainer)
      {
	  std::cout << PHWHERE << "neutralMesonTSSA::process_event - Fatal Error - GlobalVertexMap node is missing. Please turn on the do_global flag in the main macro in order to reconstruct the global vertex." << std::endl;
	  assert(vtxContainer);  // force quit
	  return 0;
      }
      if (vtxContainer->empty())
      {
	  std::cout << PHWHERE << "neutralMesonTSSA::process_event - Fatal Error - GlobalVertexMap node is empty. Please turn on the do_global flag in the main macro in order to reconstruct the global vertex." << std::endl;
	  return Fun4AllReturnCodes::ABORTEVENT;
      }

      //More vertex information
      else {
	  gVtx = vtxContainer->begin()->second;
	  if(!gVtx)
	  {
	      std::cout << PHWHERE << "neutralMesonTSSA::process_event Could not find vtx from vtxContainer"  << std::endl;
	      return Fun4AllReturnCodes::ABORTEVENT;
	  }
	  n_events_with_vertex++;
	  // Require vertex to be within 10cm of 0
	  if (abs(gVtx->get_z()) > 10.0) {
	      return Fun4AllReturnCodes::ABORTEVENT;
	  }
	  n_events_with_good_vertex++;
      }
  }

  // Check that total calo E > 0
  if (isMonteCarlo) {
      RawTowerContainer::ConstRange tower_range = towerContainer -> getTowers();
      double totalCaloE = 0;
      for(RawTowerContainer::ConstIterator tower_iter = tower_range.first; tower_iter!= tower_range.second; tower_iter++)
      {
	  double energy = tower_iter -> second -> get_energy();
	  totalCaloE += energy;
      }
      if (totalCaloE < 0) {
	  return Fun4AllReturnCodes::ABORTEVENT;
      }
      n_events_positiveCaloE++;
  }
  else {
      unsigned int tower_range = towerInfoContainer->size();
      double totalCaloE = 0;
      for(unsigned int iter = 0; iter < tower_range; iter++)
      {
	  TowerInfo* tower = towerInfoContainer->get_tower_at_channel(iter);
	  // check if tower is good
	  if(!tower->get_isGood()) continue;
	  double energy = tower->get_energy();
	  totalCaloE += energy;
      }
      if (totalCaloE < 0) {
	  return Fun4AllReturnCodes::ABORTEVENT;
      }
      n_events_positiveCaloE++;
  }

  FindGoodClusters();
  FindMesons();
  CountMesons();

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int neutralMesonTSSA::ResetEvent(PHCompositeNode *topNode)
{
  /* std::cout << "neutralMesonTSSA::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << std::endl; */
  goodclusters_E.clear();
  goodclusters_Eta.clear();
  goodclusters_Phi.clear();
  goodclusters_Ecore.clear();
  goodclusters_Chi2.clear();
  pi0_E.clear();
  pi0_M.clear();
  pi0_Eta.clear();
  pi0_Phi.clear();
  pi0_Pt.clear();
  pi0_xF.clear();
  eta_E.clear();
  eta_M.clear();
  eta_Eta.clear();
  eta_Phi.clear();
  eta_Pt.clear();
  eta_xF.clear();
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int neutralMesonTSSA::EndRun(const int runnumber)
{
  std::cout << "neutralMesonTSSA::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int neutralMesonTSSA::End(PHCompositeNode *topNode)
{
  std::cout << "neutralMesonTSSA::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  std::cout << "Processed " << n_events_total << " total events.\n";
  std::cout << "\t" << n_events_with_vertex << " events with vertex.\n";
  std::cout << "\t" << n_events_with_good_vertex << " events with |z_vtx| < 10cm.\n";
  std::cout << "\t" << n_events_positiveCaloE << " events with positive calo E.\n";
  histfile->Write();
  histfile->Close();
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int neutralMesonTSSA::Reset(PHCompositeNode *topNode)
{
 std::cout << "neutralMesonTSSA::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void neutralMesonTSSA::Print(const std::string &what) const
{
  std::cout << "neutralMesonTSSA::Print(const std::string &what) const Printing info for " << what << std::endl;
}

float neutralMesonTSSA::get_min_clusterE()
{
    return min_clusterE;
}

void neutralMesonTSSA::set_min_clusterE(float Emin)
{
    min_clusterE = Emin;
}

float neutralMesonTSSA::get_max_clusterChi2()
{
    return max_clusterChi2;
}

void neutralMesonTSSA::set_max_clusterChi2(float Chi2max)
{
    max_clusterChi2 = Chi2max;
}

void neutralMesonTSSA::FindGoodClusters()
{
    RawClusterContainer::ConstRange clusterRange = m_clusterContainer -> getClusters();
    RawClusterContainer::ConstIterator clusterIter;
    /* std::cout << "\n\nBeginning cluster loop\n"; */
    for (clusterIter = clusterRange.first; clusterIter != clusterRange.second; clusterIter++)
    {
	RawCluster *recoCluster = clusterIter -> second;
	CLHEP::Hep3Vector vertex;
	if (isMonteCarlo) vertex = CLHEP::Hep3Vector(mcVtx->get_x(), mcVtx->get_y(), mcVtx->get_z());
	else {
	    if (gVtx) vertex = CLHEP::Hep3Vector(gVtx->get_x(), gVtx->get_y(), gVtx->get_z());
	    else vertex = CLHEP::Hep3Vector(0,0,0);
	}
	CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetECoreVec(*recoCluster, vertex);
	float clusE = recoCluster->get_energy();
	float clusEcore = recoCluster->get_ecore();
	float clus_eta = E_vec_cluster.pseudoRapidity();
	float clus_phi = E_vec_cluster.phi();
	float clus_chi2 = recoCluster->get_chi2();

	if (clusE < min_clusterE) continue;
	if (clus_chi2 > max_clusterChi2) continue;

	goodclusters_E.push_back(clusE);
	goodclusters_Eta.push_back(clus_eta);
	goodclusters_Phi.push_back(clus_phi);
	goodclusters_Ecore.push_back(clusEcore);
	goodclusters_Chi2.push_back(clus_chi2);
    }
    return;
}

void neutralMesonTSSA::FindMesons()
{
    int nDiphotons = 0;
    int nRecoPi0s = 0;
    int nRecoEtas = 0;
    float min_Ediphoton_cut = 0.5;
    float max_asymmetry_cut = 0.75; // leadE <= 7*subE
    float max_clusterE_cut = 999.9; // clusters should merge above 6-7 GeV, so higher energy clusters should not be paired with others
    float min_deltaR_cut = 0.0;
    float max_deltaR_cut = 999.9;

    int nClusters = goodclusters_E.size();
    if (nClusters < 2) return;

    for (int i=0; i<(nClusters-1); i++) {
	float E1 = goodclusters_E.at(i);
	if (E1 > max_clusterE_cut) continue;
	for (int j=(i+1); j<nClusters; j++) {
	    float E2 = goodclusters_E.at(j);
	    if (E2 > max_clusterE_cut) continue;
	    nDiphotons++;
	    int lead_idx = i;
	    int sub_idx = j;
	    if (E1 < E2) {
		lead_idx = j;
		sub_idx = i;
	    }

	    double leadE = goodclusters_E.at(lead_idx);
	    double leadEta = goodclusters_Eta.at(lead_idx);
	    double leadPhi = goodclusters_Phi.at(lead_idx);
	    /* double leadChi2 = goodclusters_Chi2.at(lead_idx); */
	    double subE = goodclusters_E.at(sub_idx);
	    double subEta = goodclusters_Eta.at(sub_idx);
	    double subPhi = goodclusters_Phi.at(sub_idx);
	    /* double subChi2 = goodclusters_Chi2.at(sub_idx); */
	    double asymmetry = (leadE - subE)/(leadE + subE);
	    if (asymmetry > max_asymmetry_cut) continue;

	    TLorentzVector lead, sub;
	    float leadpT = leadE/TMath::CosH(leadEta);
	    float subpT = subE/TMath::CosH(subEta);
	    lead.SetPtEtaPhiE(leadpT, leadEta, leadPhi, leadE);
	    sub.SetPtEtaPhiE(subpT, subEta, subPhi, subE);
	    TLorentzVector diphoton = lead + sub;
	    if (diphoton.E() < min_Ediphoton_cut) continue;
	    double xF = 2.0*diphoton.Pz()/200.0;
	    double dR = lead.DeltaR(sub);
	    if (dR < min_deltaR_cut) continue;
	    if (dR > max_deltaR_cut) continue;

	    h_diphotonMass->Fill(diphoton.M());
	    if (diphoton.M() > min_pi0Mass && diphoton.M() < max_pi0Mass) {
		nRecoPi0s++;
		pi0_E.push_back(diphoton.E());
		pi0_M.push_back(diphoton.M());
		pi0_Eta.push_back(diphoton.Eta());
		pi0_Phi.push_back(diphoton.Phi());
		pi0_Pt.push_back(diphoton.Perp());
		pi0_xF.push_back(xF);
		h_pi0Mass->Fill(diphoton.M());
	    }
	    if (diphoton.M() > min_etaMass && diphoton.M() < max_etaMass) {
		nRecoEtas++;
		eta_E.push_back(diphoton.E());
		eta_M.push_back(diphoton.M());
		eta_Eta.push_back(diphoton.Eta());
		eta_Phi.push_back(diphoton.Phi());
		eta_Pt.push_back(diphoton.Perp());
		eta_xF.push_back(xF);
		h_etaMass->Fill(diphoton.M());
	    }
	}
    } // end loop over pairs of clusters
    /* h_nDiphotons->Fill(nDiphotons); */
    /* h_nRecoPi0s->Fill(nRecoPi0s); */
    /* h_nRecoEtas->Fill(nRecoEtas); */
    return;
}

void neutralMesonTSSA::CountMesons()
{
    return;
}
