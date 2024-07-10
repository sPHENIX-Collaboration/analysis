//our base code
#include "neutralMesonTSSA.h"
#include "binnedhistset/BinnedHistSet.h"

//Fun4all stuff
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllHistoManager.h>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>
#include <phool/phool.h>
/* #include <ffaobjects/EventHeader.h> */
#include <ffaobjects/RunHeader.h>
#include <ffarawobjects/Gl1Packet.h>

//ROOT stuff
#include <TH1F.h>
#include <TH2F.h>
#include <TFile.h>
#include <TLorentzVector.h>
#include <TString.h>

//for emc clusters
#include <calobase/RawCluster.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawClusterUtility.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/RawTower.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/TowerInfo.h>
#include <calobase/TowerInfoContainer.h>

//spin database stuff
#include <uspin/SpinDBContent.h>
#include <uspin/SpinDBOutput.h>

// Minimum Bias
/* #include <calotrigger/MinimumBiasInfo.h> */

//for vertex information
#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>
#include <globalvertex/MbdVertex.h>
#include <globalvertex/MbdVertexMap.h>

//truth information
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4VtxPoint.h>

neutralMesonTSSA::neutralMesonTSSA(const std::string &name, std::string histname, bool isMC):
 SubsysReco(name),
 isMonteCarlo(isMC),
 outfilename(histname)
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

  outfile = new TFile(outfilename.c_str(), "RECREATE");
  outfile->cd();

  MakeAllHists();
  MakeVectors();

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int neutralMesonTSSA::InitRun(PHCompositeNode *topNode)
{
  std::cout << "neutralMesonTSSA::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;

  // Run header
  runHeader = findNode::getClass<RunHeader>(topNode, "RunHeader");
  if (!runHeader)
  {
    std::cout << PHWHERE << ":: RunHeader node missing! Skipping run XXX" << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  GetRunNum();
  int good_spin = GetSpinInfo();
  if (good_spin)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int neutralMesonTSSA::process_event(PHCompositeNode *topNode)
{
  /* std::cout << "neutralMesonTSSA::process_event(PHCompositeNode *topNode) Processing Event" << std::endl; */
  n_events_total++;
  if (n_events_total%10000 == 0) std::cout << "Event " << n_events_total << std::endl;
  /* if (n_events_total < 1000) return Fun4AllReturnCodes::ABORTEVENT; */
  /* std::cout << "Greg info: starting process_event. n_events_total = " << n_events_total << std::endl; */

  // First populate all the data containers

  // GL1
  gl1Packet = findNode::getClass<Gl1Packet>(topNode, "GL1Packet");
  if (!gl1Packet)
  {
    std::cout << PHWHERE << ":: GL1Packet node missing! Skipping run " << runNum << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  // Check for MBDNS coincidence trigger
  GetTrigger();
  if (!mbdtrigger) return Fun4AllReturnCodes::ABORTEVENT;

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
  MbdVertexMap *MBDvtxContainer = findNode::getClass<MbdVertexMap>(topNode,"MbdVertexMap");
  if (MBDvtxContainer) {
      if (!MBDvtxContainer->empty()) {
	  MbdVertex *MBDVertex= MBDvtxContainer->begin()->second;
	  if (MBDVertex) {
	      mbdvertex = true;
	      n_events_with_mbdvertex++;
	      if (mbdtrigger) n_events_mbdvtx_with_mbdtrig++;
	      if (!mbdtrigger) n_events_mbdvtx_without_mbdtrig++;
	      if (first_mbdvtx == 0) first_mbdvtx = n_events_total - 1;
	      if (n_events_total < 1000) n_events_mbdvtx_first1k++;
	  }
      }
  }

  // Problem is MC has a PrimaryVtx but no GlobalVertex, while RD has the opposite
  if (isMonteCarlo) {
      PHG4TruthInfoContainer::VtxRange vtx_range = m_truthInfo->GetPrimaryVtxRange();
      PHG4TruthInfoContainer::ConstVtxIterator vtxIter = vtx_range.first;
      mcVtx = vtxIter->second;
      n_events_with_globalvertex++;
      if (abs(mcVtx->get_z()) > 30.0) {
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
      /* std::cout << "Global vertex map has size " << vtxContainer->size() << std::endl; */
      if (vtxContainer->empty())
      {
	  // Final version:
	  /* std::cout << PHWHERE << "neutralMesonTSSA::process_event - Fatal Error - GlobalVertexMap node is empty. Please turn on the do_global flag in the main macro in order to reconstruct the global vertex." << std::endl; */
	  return Fun4AllReturnCodes::ABORTEVENT;

	  // For testing
	  // Use (0,0,0) as the vertex
	  /* n_events_with_globalvertex++; */
	  /* n_events_with_good_vertex++; */
      }

      //More vertex information
      else {
	  gVtx = vtxContainer->begin()->second;
	  if (!gVtx)
	  {
	      /* std::cout << PHWHERE << "neutralMesonTSSA::process_event Could not find vtx from vtxContainer"  << std::endl; */
	      return Fun4AllReturnCodes::ABORTEVENT;
	  }
	  globalvertex = true;
	  n_events_with_globalvertex++;
	  if (mbdtrigger) n_events_globalvtx_with_mbdtrig++;
	  if (!mbdtrigger) n_events_globalvtx_without_mbdtrig++;
	  if (mbdvertex) n_events_globalvtx_with_mbdvtx++;
	  if (!mbdvertex) n_events_globalvtx_without_mbdvtx++;
	  if (first_globalvtx == 0) first_globalvtx = n_events_total - 1;
	  if (n_events_total < 1000) n_events_globalvtx_first1k++;
	  // Require vertex to be within 10cm of 0
	  if (abs(gVtx->get_z()) > 30.0) {
	      /* std::cout << PHWHERE << ":: Vertex |z| > 10cm, skipping event" << std::endl; */
	      /* return Fun4AllReturnCodes::ABORTEVENT; */
	  }
	  if (abs(gVtx->get_z()) < 10.0) {
	      n_events_with_vtx10++;
	  }
	  if (abs(gVtx->get_z()) < 30.0) {
	      n_events_with_vtx30++;
	      n_events_with_good_vertex++;
	  }
	  if (abs(gVtx->get_z()) < 50.0) {
	      n_events_with_vtx50++;
	  }
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
	  /* std::cout << PHWHERE << ":: Total EMCal energy < 0, skipping event" << std::endl; */
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
	  /* std::cout << PHWHERE << ":: Total EMCal energy < 0, skipping event" << std::endl; */
	  return Fun4AllReturnCodes::ABORTEVENT;
      }
      n_events_positiveCaloE++;
  }

  // Event is passes all checks. Now call the functions to do the analysis
  /* std::cout << "Greg info: Getting bunch number" << std::endl; */
  GetBunchNum();
  /* std::cout << "Greg info: Getting spins" << std::endl; */
  GetSpins();
  /* std::cout << "Greg info: Getting good clusters. n_events_total = " << n_events_total << std::endl; */
  FindGoodClusters();
  /* std::cout << "Greg info: Getting diphotons" << std::endl; */
  FindDiphotons();
  /* std::cout << "Greg info: Filling phi hists" << std::endl; */
  FillAllPhiHists();

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int neutralMesonTSSA::ResetEvent(PHCompositeNode *topNode)
{
  /* std::cout << "neutralMesonTSSA::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << std::endl; */
  
  // Clear the data containers and vertex info
  m_clusterContainer = nullptr;
  m_truthInfo = nullptr;
  gVtx = nullptr;
  mcVtx = nullptr;
  gl1Packet = nullptr;
  mbdNtrigger = false;
  mbdStrigger = false;
  mbdtrigger = false;
  mbdvertex = false;
  globalvertex = false;

  // Clear the vectors
  ClearVectors();

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int neutralMesonTSSA::EndRun(const int runnumber)
{
  std::cout << "neutralMesonTSSA::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;

  // Clear the RunHeader
  runHeader = nullptr;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int neutralMesonTSSA::End(PHCompositeNode *topNode)
{
  std::cout << "neutralMesonTSSA::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  std::cout << "Processed " << n_events_total << " total events. Skipping first 1000!\n";
  std::cout << "" << n_events_mbdtrigger << " events with MBDN&S trigger\n";
  std::cout << "\t(" << mbdcoinc_withoutNandS << " *without* MBDN AND MBDS individually)\n";
  std::cout << "\t" << n_events_mbdtrigger_vtx1 << " with MBD trigger, |z_vtx| < T1\n";
  std::cout << "\t" << n_events_mbdtrigger_vtx2 << " with MBD trigger, |z_vtx| < T2\n";
  std::cout << "\t" << n_events_mbdtrigger_vtx3 << " events with MBD trigger, |z_vtx| < T3\n\n";
  std::cout << "" << n_events_with_mbdvertex << " events with MBDVertex (first event is " << first_mbdvtx << ")\n";
  std::cout << "\t" << n_events_mbdvtx_with_mbdtrig << " with MBDN&S trigger\n";
  std::cout << "\t" << n_events_mbdvtx_without_mbdtrig << " *without* MBDN&S trigger\n";
  std::cout << "" << n_events_with_globalvertex << " events with GlobalVertex (first event is " << first_globalvtx << ")\n";
  std::cout << "\t" << n_events_globalvtx_with_mbdvtx << " with MbdVertex\n";
  std::cout << "\t" << n_events_globalvtx_without_mbdvtx << " *without* MbdVertex\n";
  std::cout << "\t" << n_events_globalvtx_with_mbdtrig << " with MBDN&S trigger\n";
  std::cout << "\t" << n_events_globalvtx_without_mbdtrig << " *without* MBDN&S trigger\n";
  /* std::cout << "" << n_events_with_good_vertex << " events with |z_vtx| < 30cm.\n"; */
  std::cout << "" << n_events_with_vtx10 << " events with GlobalVertex |z| < 10cm.\n";
  std::cout << "" << n_events_with_vtx30 << " events with GlobalVertex |z| < 30cm.\n";
  std::cout << "" << n_events_with_vtx50 << " events with GlobalVertex |z| < 50cm.\n";
  std::cout << "" << n_events_positiveCaloE << " events with positive calo E.\n";

  outfile->Write();
  outfile->Close();

  DeleteStuff();

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

bool neutralMesonTSSA::InRange(float mass, std::pair<float, float> range)
{
    bool ret = false;
    if ( (mass > range.first) && (mass < range.second) ) ret = true;
    return ret;
}

void neutralMesonTSSA::MakePhiHists(std::string which)
{
    PhiHists* hists = new PhiHists();
    std::string nameprefix, titlewhich, titlebeam;
    nameprefix = "h_" + which + "_";

    if (which == "pi0") {
	titlewhich = "#pi^{0}";
	pi0Hists = hists;
    }
    else if (which == "eta") {
	titlewhich = "#eta";
	etaHists = hists;
    }
    else if (which == "pi0bkgr") {
	titlewhich = "#pi^{0} Background";
	pi0BkgrHists = hists;
    }
    else if (which == "etabkgr") {
	titlewhich = "#eta Background";
	etaBkgrHists = hists;
    }
    else {
	std::cout << PHWHERE << ":: Invalid arguments!" << std::endl;
	return;
    }

    std::vector<double> pTbins;
    std::vector<double> xFbins;
    for (int i=0; i<nBins_pT; i++) {
	pTbins.push_back(i*(bhs_max_pT/nBins_pT));
    }
    for (int i=0; i<nBins_xF; i++) {
	xFbins.push_back(2*bhs_max_xF*i/nBins_xF - bhs_max_xF);
    }
    pTbins.push_back(bhs_max_pT);
    xFbins.push_back(bhs_max_xF);

    hists->phi_pT = new BinnedHistSet(Form("%sphi_pT", nameprefix.c_str()), Form("%s #phi Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins);
    hists->phi_pT_blue_up = new BinnedHistSet(Form("%sphi_pT_blue_up", nameprefix.c_str()), Form("%s Blue Beam Spin-Up #phi^{B} Distribution;#phi^{B} (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins);
    hists->phi_pT_blue_down = new BinnedHistSet(Form("%sphi_pT_blue_down", nameprefix.c_str()), Form("%s Blue Beam Spin-Down #phi^{B} Distribution;#phi^{B} (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins);
    hists->phi_pT_yellow_up = new BinnedHistSet(Form("%sphi_pT_yellow_up", nameprefix.c_str()), Form("%s Yellow Beam Spin-Up #phi^{Y} Distribution;#phi^{Y} (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins);
    hists->phi_pT_yellow_down = new BinnedHistSet(Form("%sphi_pT_yellow_down", nameprefix.c_str()), Form("%s Yellow Beam Spin-Down #phi^{Y} Distribution;#phi^{Y} (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "p_{T} (GeV)", pTbins);

    hists->phi_xF = new BinnedHistSet(Form("%sphi_xF", nameprefix.c_str()), Form("%s #phi Distribution;#phi (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "x_{F}", xFbins);
    hists->phi_xF_blue_up = new BinnedHistSet(Form("%sphi_xF_blue_up", nameprefix.c_str()), Form("%s Blue Beam Spin-Up #phi^{B} Distribution;#phi^{B} (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "x_{F}", xFbins);
    hists->phi_xF_blue_down = new BinnedHistSet(Form("%sphi_xF_blue_down", nameprefix.c_str()), Form("%s Blue Beam Spin-Down #phi^{B} Distribution;#phi^{B} (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "x_{F}", xFbins);
    hists->phi_xF_yellow_up = new BinnedHistSet(Form("%sphi_xF_yellow_up", nameprefix.c_str()), Form("%s Yellow Beam Spin-Up #phi^{Y} Distribution;#phi^{Y} (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "x_{F}", xFbins);
    hists->phi_xF_yellow_down = new BinnedHistSet(Form("%sphi_xF_yellow_down", nameprefix.c_str()), Form("%s Yellow Beam Spin-Down #phi^{Y} Distribution;#phi^{Y} (rad);Counts", titlewhich.c_str()), nHistBins_phi, -1.0*PI, PI, "x_{F}", xFbins);

    hists->phi_pT->MakeHists();
    hists->phi_pT_blue_up->MakeHists();
    hists->phi_pT_blue_down->MakeHists();
    hists->phi_pT_yellow_up->MakeHists();
    hists->phi_pT_yellow_down->MakeHists();

    hists->phi_xF->MakeHists();
    hists->phi_xF_blue_up->MakeHists();
    hists->phi_xF_blue_down->MakeHists();
    hists->phi_xF_yellow_up->MakeHists();
    hists->phi_xF_yellow_down->MakeHists();
}

void neutralMesonTSSA::MakeAllHists()
{
    // clusters
    int nbins_etaphi = 100;
    double eta_upper = 1.15;
    double phi_upper = PI;
    int nbins_pT = 100;
    int nbins_xF = 100;
    double pT_upper = 15.0;
    double xF_upper = 0.15;
    h_clusterE = new TH1F("h_clusterE", "Cluster Energy Distribution;Cluster E (GeV);Counts", nbins_pT, 0.0, pT_upper);
    h_clusterEta = new TH1F("h_clusterEta", "Cluster #eta Distribution;Cluster #eta;Counts", nbins_etaphi, -eta_upper, eta_upper);
    h_clusterPhi = new TH1F("h_clusterPhi", "Cluster #phi Distribution;Cluster #phi;Counts", nbins_etaphi, -phi_upper, phi_upper);
    h_clusterEta_Phi = new TH2F("h_clusterEta_Phi", "Cluster Position;Cluster #eta;Cluster #phi (rad)", nbins_etaphi, -eta_upper, eta_upper, nbins_etaphi, -phi_upper, phi_upper);
    h_clusterpT = new TH1F("h_clusterpT", "Cluster Transverse Momentum Distribution;Cluster p_{T} (GeV);Counts", nbins_pT, 0.0, pT_upper);
    h_clusterxF = new TH1F("h_clusterxF", "Cluster x_{F} Distribution;Cluster x_{F};Counts", nbins_xF, -1.0*xF_upper, xF_upper);
    h_clusterChi2 = new TH1F("h_clusterChi2", "Cluster #chi^{2} Distribution;Cluster #chi^{2};Counts", 200, 0.0, 30.0);
    h_clusterChi2zoomed = new TH1F("h_clusterChi2zoomed", "Cluster #chi^{2} Distribution;Cluster #chi^{2};Counts", 200, 0.0, 5.0);

    // diphotons
    int nbins_mass = 100;
    h_diphotonMass = new TH1F("h_diphotonMass", "Diphoton Mass Distribution;Diphoton Mass (GeV);Counts", nbins_mass, 0.0, 1.0);
    h_diphotonpT = new TH1F("h_diphotonpT", "Diphoton p_{T} Distribution;Diphoton p_{T} (GeV);Counts", nbins_pT, 0.0, pT_upper);
    h_diphotonxF = new TH1F("h_diphotonxF", "Diphoton x_{F} Distribution;Diphoton x_{F};Counts", nbins_xF, -1.0*xF_upper, xF_upper);

    std::vector<double> pTbins;
    std::vector<double> xFbins;
    int nbins_bhs = 25;
    for (int i=0; i<nbins_bhs; i++) {
	pTbins.push_back(i*(pT_upper/nbins_bhs));
	xFbins.push_back(2*xF_upper*i/nbins_bhs - xF_upper);
    }
    pTbins.push_back(pT_upper);
    xFbins.push_back(xF_upper);

    bhs_diphotonMass_pT = new BinnedHistSet("h_diphotonMass_pT", "Diphoton Mass;Mass (GeV);Counts", nbins_mass, 0.0, 1.0, "p_{T} (GeV)", pTbins);
    bhs_diphotonMass_xF = new BinnedHistSet("h_diphotonMass_xF", "Diphoton Mass;Mass (GeV);Counts", nbins_mass, 0.0, 1.0, "x_{F}", xFbins);
    bhs_diphotonMass_pT->MakeHists();
    bhs_diphotonMass_xF->MakeHists();

    MakePhiHists("pi0");
    MakePhiHists("eta");
    MakePhiHists("pi0bkgr");
    MakePhiHists("etabkgr");
}

void neutralMesonTSSA::MakeVectors()
{
    goodclusters_E = new std::vector<float>;
    goodclusters_Eta = new std::vector<float>;
    goodclusters_Phi = new std::vector<float>;
    goodclusters_Ecore = new std::vector<float>;
    goodclusters_Chi2 = new std::vector<float>;

    pi0s = new std::vector<Diphoton>;
    etas = new std::vector<Diphoton>;
    pi0Bkgr = new std::vector<Diphoton>;
    etaBkgr = new std::vector<Diphoton>;
}

void neutralMesonTSSA::GetRunNum()
{
    if (!runHeader)
    {
	std::cout << PHWHERE << ":: Missing RunHeader! Exiting!" << std::endl;
	exit(1);
    }
    runNum = runHeader->get_RunNumber();
    /* std::cout << "Run number is " << runNum << std::endl; */
}

int neutralMesonTSSA::GetSpinInfo()
{
    int spinDB_status = 0;
    // Get the spin patterns from the spin DB
    //  0xffff is the qa_level from XingShiftCal //////
    unsigned int qa_level = 0xffff;
    SpinDBContent spin_cont;
    SpinDBOutput spin_out("phnxrc");

    spin_out.StoreDBContent(runNum,runNum,qa_level);
    spin_out.GetDBContentStore(spin_cont,runNum);

    // Get crossing shift
    crossingShift = spin_cont.GetCrossingShift();
    if (crossingShift == -999) crossingShift = 0;
    std::cout << "Crossing shift: " << crossingShift << std::endl;

    // Get spin patterns
    std::cout << "Blue spin pattern: [";
    for (int i = 0; i < NBUNCHES; i++)
    {
	spinPatternBlue[i] = spin_cont.GetSpinPatternBlue(i);
	std::cout << spinPatternBlue[i];
	if (i < 119)std::cout << ", ";
    }
    std::cout << "]" << std::endl;

    std::cout << "Yellow spin pattern: [";
    for (int i = 0; i < NBUNCHES; i++)
    {
	spinPatternYellow[i] = spin_cont.GetSpinPatternYellow(i);
	std::cout << spinPatternYellow[i];
	if (i < 119)std::cout << ", ";
    }
    std::cout << "]" << std::endl;

    // Get beam polarizations
    float bpolerr, ypolerr;
    spin_cont.GetPolarizationBlue(0, polBlue, bpolerr);
    spin_cont.GetPolarizationYellow(0, polYellow, ypolerr);
    
    // Get crossing shift
    crossingShift = spin_cont.GetCrossingShift();

    // Get GL1P scalers
    std::cout << "MBDNS GL1p scalers: [";
    for (int i = 0; i < NBUNCHES; i++)
    {
        gl1pScalers[i] = spin_cont.GetScalerMbdNoCut(i);
	std::cout << gl1pScalers[i];
	if (i < 119) std::cout << ", ";
    }
    std::cout << "]" << std::endl;
    
    // Calculate luminosities
    for (int i = 0; i < NBUNCHES; i++)
    {
        int bsp = spinPatternBlue[i];
        int ysp = spinPatternYellow[i];
	if (bsp == 1) lumiUpBlue += gl1pScalers[i];
	if (bsp == -1) lumiDownBlue += gl1pScalers[i];
	if (ysp == 1) lumiUpYellow += gl1pScalers[i];
	if (ysp == -1) lumiDownYellow += gl1pScalers[i];
    }

    // Print run number, relative luminosity & polarization, total luminosity
    std::cout << "Luminosity info: run#,bPol,bLumiUp,bLumiDown,yPol,yLumiUp,yLumiDown" << std::endl;
    std::cout << Form("%d,%f,%f,%f,%f,%f,%f", runNum, polBlue/100.0, lumiUpBlue, lumiDownBlue, polYellow/100.0, lumiUpYellow, lumiDownYellow) << std::endl;

    // Check if spin info is valid
    if (spinPatternYellow[0] == -999) spinDB_status = 1;

    if (spinDB_status)
    {
	std::cout << PHWHERE << ":: Run number " << runNum << " not found in spin DB! Skipping this run!" << std::endl;
	/* std::cout << PHWHERE << ":: Run number " << runNum << " not found in spin DB! Exiting!" << std::endl; */
	/* exit(1); */
    }
    return spinDB_status;
}

void neutralMesonTSSA::GetBunchNum()
{
  if (gl1Packet)
  {
    bunchNum = gl1Packet->getBunchNumber();
    sphenixBunch = (bunchNum + crossingShift)%NBUNCHES;
  }
  else
  {
    std::cout << "GL1 missing!" << std::endl;
    exit(1);
  }
}

void neutralMesonTSSA::GetSpins()
{
    bspin = spinPatternBlue[sphenixBunch];
    yspin = spinPatternYellow[sphenixBunch];
}

/* void neutralMesonTSSA::CountLumi() */
/* { */
/*     float bLumi = 0; // Replace with GL1 Scaler value */
/*     if (bspin == 1) lumiUpBlue += bLumi; */
/*     if (bspin == -1) lumiDownBlue += bLumi; */
/*     float yLumi = 0; // Replace with GL1 Scaler value */
/*     if (yspin == 1) lumiUpYellow += yLumi; */
/*     if (yspin == -1) lumiDownYellow += yLumi; */
/* } */

void neutralMesonTSSA::FindGoodClusters()
{
    RawClusterContainer::ConstRange clusterRange = m_clusterContainer -> getClusters();
    RawClusterContainer::ConstIterator clusterIter;
    /* std::cout << "\n\nBeginning cluster loop\n"; */
    /* std::cout << "cluster container size is " << m_clusterContainer->size() << std::endl; */
    for (clusterIter = clusterRange.first; clusterIter != clusterRange.second; clusterIter++)
    {
	/* std::cout << "clusterIter->first = " << clusterIter->first << std::endl; */
	RawCluster *recoCluster = clusterIter -> second;
	/* std::cout << "recoCluster = " << recoCluster << std::endl; */
	CLHEP::Hep3Vector vertex;
	if (isMonteCarlo) {
	    /* std::cout << "Setting MC vertex, mcVtx = " << mcVtx << std::endl; */
	    vertex = CLHEP::Hep3Vector(mcVtx->get_x(), mcVtx->get_y(), mcVtx->get_z());
	}
	else {
	    /* std::cout << "Setting RD vertex, gVtx = " << gVtx << ", identify():" << std::endl; */
	    if (gVtx) {
		/* int isValid = gVtx->isValid(); */
		/* std::cout << "Vertex isValid = " << isValid << std::endl; */
		/* float vtx_z = gVtx->get_z(); */
		/* std::cout << "Vertex z = " << vtx_z << std::endl; */
		/* gVtx->identify(); */
	        vertex = CLHEP::Hep3Vector(gVtx->get_x(), gVtx->get_y(), gVtx->get_z());
	    }
	    else vertex = CLHEP::Hep3Vector(0,0,0);
	    /* std::cout << "Done setting RD vertex, gVtx = " << gVtx << std::endl; */
	}
	/* std::cout << "Got vertex" << std::endl; */
	CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetECoreVec(*recoCluster, vertex);
	/* std::cout << "E_vec_cluster" << std::endl; */
	float clusE = recoCluster->get_energy();
	float clusEcore = recoCluster->get_ecore();
	float clus_eta = E_vec_cluster.pseudoRapidity();
	float clus_phi = E_vec_cluster.phi();
	float clus_chi2 = recoCluster->get_chi2();

	h_clusterE->Fill(clusE);
	h_clusterEta->Fill(clus_eta);
	h_clusterPhi->Fill(clus_phi);
	h_clusterEta_Phi->Fill(clus_eta, clus_phi);
	h_clusterpT->Fill(clusE/TMath::CosH(clus_eta));
	h_clusterxF->Fill(2.0*(clusE*TMath::TanH(clus_eta))/200.0);
	h_clusterChi2->Fill(clus_chi2);
	h_clusterChi2zoomed->Fill(clus_chi2);

	/* std::cout << "Applying cuts" << std::endl; */
	if (clusE < min_clusterE) continue;
	if (clusE > max_clusterE) continue;
	if (clus_chi2 > max_clusterChi2) continue;

	/* std::cout << "Populating goodcluster vectors" << std::endl; */
	goodclusters_E->push_back(clusE);
	goodclusters_Eta->push_back(clus_eta);
	goodclusters_Phi->push_back(clus_phi);
	goodclusters_Ecore->push_back(clusEcore);
	goodclusters_Chi2->push_back(clus_chi2);
	/* std::cout << "Done" << std::endl; */
    }
    return;
}

void neutralMesonTSSA::FindDiphotons()
{
    int nDiphotons = 0;
    int nRecoPi0s = 0;
    int nRecoEtas = 0;

    int nClusters = goodclusters_E->size();
    if (nClusters < 2) return;

    for (int i=0; i<(nClusters-1); i++) {
	float E1 = goodclusters_E->at(i);
	for (int j=(i+1); j<nClusters; j++) {
	    float E2 = goodclusters_E->at(j);
	    nDiphotons++;
	    int lead_idx = i;
	    int sub_idx = j;
	    if (E1 < E2) {
		lead_idx = j;
		sub_idx = i;
	    }

	    double leadE = goodclusters_E->at(lead_idx);
	    double leadEta = goodclusters_Eta->at(lead_idx);
	    double leadPhi = goodclusters_Phi->at(lead_idx);
	    /* double leadChi2 = goodclusters_Chi2->at(lead_idx); */
	    double subE = goodclusters_E->at(sub_idx);
	    double subEta = goodclusters_Eta->at(sub_idx);
	    double subPhi = goodclusters_Phi->at(sub_idx);
	    /* double subChi2 = goodclusters_Chi2->at(sub_idx); */
	    double asymmetry = (leadE - subE)/(leadE + subE);
	    if (asymmetry > max_asym) continue;

	    TLorentzVector lead, sub;
	    float leadpT = leadE/TMath::CosH(leadEta);
	    float subpT = subE/TMath::CosH(subEta);
	    lead.SetPtEtaPhiE(leadpT, leadEta, leadPhi, leadE);
	    sub.SetPtEtaPhiE(subpT, subEta, subPhi, subE);
	    TLorentzVector diphoton = lead + sub;
	    if (diphoton.Perp() < min_diphotonPt) continue;
	    double xF = 2.0*diphoton.Pz()/200.0;
	    double dR = lead.DeltaR(sub);
	    if (dR < min_deltaR) continue;
	    if (dR > max_deltaR) continue;

	    h_diphotonMass->Fill(diphoton.M());
	    bhs_diphotonMass_pT->FillHists(diphoton.Perp(), diphoton.M());
	    bhs_diphotonMass_xF->FillHists(xF, diphoton.M());
	    h_diphotonpT->Fill(diphoton.Perp());
	    h_diphotonxF->Fill(xF);

	    Diphoton d;
	    d.mass = diphoton.M();
	    d.E = diphoton.E();
	    d.eta = diphoton.Eta();
	    d.phi = diphoton.Phi();
	    d.pT = diphoton.Perp();
	    d.xF = xF;

	    std::vector<Diphoton>* vec = nullptr;
	    if (InRange(diphoton.M(), pi0MassRange)) {
		nRecoPi0s++;
		vec = pi0s;
	    }
	    if (InRange(diphoton.M(), etaMassRange)) {
		nRecoEtas++;
		vec = etas;
	    }
	    if (InRange(diphoton.M(), pi0BkgrLowRange) || InRange(diphoton.M(), pi0BkgrHighRange)) {
		vec = pi0Bkgr;
	    }
	    if (InRange(diphoton.M(), etaBkgrLowRange) || InRange(diphoton.M(), etaBkgrHighRange)) {
		vec = etaBkgr;
	    }

	    if (vec) {
		vec->push_back(d);
	    }
	}
    } // end loop over pairs of clusters
    /* h_nDiphotons->Fill(nDiphotons); */
    /* h_nRecoPi0s->Fill(nRecoPi0s); */
    /* h_nRecoEtas->Fill(nRecoEtas); */
    return;
}

void neutralMesonTSSA::FillPhiHists(std::string which, int index)
{
    std::vector<Diphoton>* vec = nullptr;
    PhiHists* hists = nullptr;

    if (which == "pi0") {
	vec = pi0s;
	hists = pi0Hists;
    }
    if (which == "eta") {
	vec = etas;
	hists = etaHists;
    }
    if (which == "pi0bkgr") {
	vec = pi0Bkgr;
	hists = pi0BkgrHists;
    }
    if (which == "etabkgr") {
	vec = etaBkgr;
	hists = etaBkgrHists;
    }

    if (!vec || !hists) {
	std::cout << PHWHERE << ":: Invalid arguments!" << std::endl;
	return;
    }

    Diphoton d = vec->at(index);
    float phi = d.phi;
    float phiblue = 999.9;
    float phiyellow = 999.9;
    phiblue = phi - PI/2.0;
    if (phiblue < -PI) phiblue += 2.0*PI;
    phiyellow = phi + PI/2.0;
    if (phiyellow > PI) phiyellow -= 2.0*PI;

    hists->phi_pT->FillHists(d.pT, phi);
    if (bspin == 1) {
	hists->phi_pT_blue_up->FillHists(d.pT, phiblue);
    }
    if (bspin == -1) {
	hists->phi_pT_blue_down->FillHists(d.pT, phiblue);
    }
    if (yspin == 1) {
	hists->phi_pT_yellow_up->FillHists(d.pT, phiyellow);
    }
    if (yspin == -1) {
	hists->phi_pT_yellow_down->FillHists(d.pT, phiyellow);
    }

    // xF is defined such that forward is to the north. For yellow beam, invert this
    float xFblue = d.xF;
    float xFyellow = -1.0*d.xF;
    hists->phi_xF->FillHists(d.xF, phi);
    if (bspin == 1) {
	hists->phi_xF_blue_up->FillHists(xFblue, phiblue);
    }
    if (bspin == -1) {
	hists->phi_xF_blue_down->FillHists(xFblue, phiblue);
    }
    if (yspin == 1) {
	hists->phi_xF_yellow_up->FillHists(xFyellow, phiyellow);
    }
    if (yspin == -1) {
	hists->phi_xF_yellow_down->FillHists(xFyellow, phiyellow);
    }
}

void neutralMesonTSSA::FillAllPhiHists()
{
    for (unsigned int i=0; i<pi0s->size(); i++) {
	FillPhiHists("pi0", i);
    }
    for (unsigned int i=0; i<etas->size(); i++) {
	FillPhiHists("eta", i);
    }
    for (unsigned int i=0; i<pi0Bkgr->size(); i++) {
	FillPhiHists("pi0bkgr", i);
    }
    for (unsigned int i=0; i<etaBkgr->size(); i++) {
	FillPhiHists("etabkgr", i);
    }
}

void neutralMesonTSSA::ClearVectors()
{
    goodclusters_E->clear();
    goodclusters_Ecore->clear();
    goodclusters_Eta->clear();
    goodclusters_Phi->clear();
    goodclusters_Chi2->clear();

    pi0s->clear();
    etas->clear();
    pi0Bkgr->clear();
    etaBkgr->clear();
}

void neutralMesonTSSA::DeleteStuff()
{
    delete pi0Hists; delete etaHists; delete pi0BkgrHists; delete etaBkgrHists;
    delete goodclusters_E; delete goodclusters_Eta; delete goodclusters_Phi; delete goodclusters_Ecore; delete goodclusters_Chi2;
    delete pi0s; delete etas; delete pi0Bkgr; delete etaBkgr;
}

void neutralMesonTSSA::GetTrigger()
{
    if (gl1Packet)
    {
	uint64_t trig = gl1Packet->getTriggerVector();
	unsigned int mbdtrignum = 10;
	if (((trig >> mbdtrignum ) & 0x1U ) == 0x1U)
	{
	    mbdtrigger = true;
	}
    }
}

void neutralMesonTSSA::PrintTrigger()
{
    if (gl1Packet)
    {
	uint64_t trig = gl1Packet->getTriggerVector();
	uint64_t live = gl1Packet->getLiveVector();
	uint64_t scaled = gl1Packet->getScaledVector();

	std::bitset<64> trigbits(trig);
	std::bitset<64> livebits(live);
	std::bitset<64> scaledbits(scaled);

	/* std::cout << "trig = " << trig << "\ntrigbits = " << trigbits << std::endl; */
	for (unsigned int i=0; i<64; i++)
	{
	    if (((trig >> i ) & 0x1U ) == 0x1U)
	    {
		/* std::cout << "Trigger: bit " << i << " = true" << std::endl; */
		if (i == 8) mbdStrigger = true;
		if (i == 9) mbdNtrigger = true;
		if (i == 10) {
		    n_events_mbdtrigger++;
		    mbdtrigger = true;
		    if (!(mbdStrigger && mbdNtrigger)) mbdcoinc_withoutNandS++;
		}
		if (i == 12) n_events_mbdtrigger_vtx1++;
		if (i == 13) n_events_mbdtrigger_vtx2++;
		if (i == 14) n_events_mbdtrigger_vtx3++;
	    }
	}
	/*
	std::cout << "live = " << live << "\nlivebits = " << livebits << std::endl;
	for (unsigned int i=0; i<64; i++)
	{
	    if (((live >> i ) & 0x1U ) == 0x1U)
	    {
		std::cout << "Live: bit " << i << " = true" << std::endl;
	    }
	}
	*/
	/*
	std::cout << "scaled = " << scaled << "\nscaledbits = " << scaledbits << std::endl;
	for (unsigned int i=0; i<64; i++)
	{
	    if (((scaled >> i ) & 0x1U ) == 0x1U)
	    {
		std::cout << "Scaled: bit " << i << " = true" << std::endl;
	    }
	}
	*/
    }
}
