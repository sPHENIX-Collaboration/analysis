//our base code
#include "cemcReco.h"

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

//caloEvalStack for cluster to truth matching
#include <g4eval/CaloEvalStack.h>
#include <g4eval/CaloRawClusterEval.h>

//for vetex information
#include <g4vertex/GlobalVertex.h>
#include <g4vertex/GlobalVertexMap.h>

//tracking
#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/SvtxVertex.h>
#include <trackbase_historic/SvtxVertexMap.h>

//truth information
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>
#include </gpfs/mnt/gpfs02/sphenix/user/ahodges/macros_git/coresoftware/generators/phhepmc/PHHepMCGenEvent.h>
#include </gpfs/mnt/gpfs02/sphenix/user/ahodges/macros_git/coresoftware/generators/phhepmc/PHHepMCGenEventMap.h>
#include <g4main/PHG4VtxPoint.h>
//#include <phhepmc/PHHepMCParticleSelectorDecayProductChain.h>
#include <HepMC/GenEvent.h>
#include <HepMC/GenParticle.h>
#include <HepMC/GenVertex.h>
#include <HepMC/IteratorRange.h>
#include <HepMC/SimpleVector.h> 
#include <HepMC/GenParticle.h>
#include <CLHEP/Geometry/Point3D.h>



//____________________________________________________________________________..
cemcReco::cemcReco(const std::string &name, const std::string &outName):
SubsysReco(name),
  nEvent(0),
  Outfile(outName),
  trackErrorCount(0)
{
  std::cout << "cemcReco::cemcReco(const std::string &name) Constructor" << std::endl;  
  out = 0;

  //histos
  photonE = 0;
  clusterDisp = 0;
  clusterChi2 = 0;
  clusterProbPhoton = 0;
  isoPhoE = 0;
  isoPhoChi2 = 0;
  isoPhoProb = 0;
  deltaR_E_invMass = 0;
  tsp_E = 0;
  tsp_E_iso = 0;
  for(int i = 0; i < nEtaBins; i++) truth_pi0_E[i] = 0;
  truth_eta_E = 0;
  truth_dpho_E = 0; 
  pi0E_truth_reco = 0;
  invMass_eta = 0;
  eFrac_dr_primary = 0;
  eFrac_dr_secondary = 0;
 
  for(int i = 0; i < 2; i++)
    {
      for(int j = 0; j < nEtaBins; j++)
	{
	  ePi0InvMassEcut[i][j] = 0;
	}
    }
  dPhoChi2 = 0;
  dPhoProb = 0;
  pi0Chi2 = 0;
  pi0Prob = 0;
  etaChi2 = 0;
  etaProb = 0;
  electronChi2 = 0;
  electronProb = 0; 
  hadronChi2 = 0;
  hadronProb = 0;
  etaFrac = 0;   
  pi0Frac = 0;
  unmatchedLocale = 0;
  unmatchedE = 0;
  invMassPhoPi0 = 0;
  invMassEPhi = 0;
  //caloevalstack = NULL;
}

//____________________________________________________________________________..
cemcReco::~cemcReco()
{
  std::cout << "cemcReco::~cemcReco() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
int cemcReco::Init(PHCompositeNode *topNode)
{
  std::cout << "cemcReco::Init(PHCompositeNode *topNode) Initializing Histos" << std::endl;

  //histo initialization
  photonE = new TH1F("photonE_Reco","Reco Energy",200,0,20);

  clusterChi2 = new TH2F("clusterChi2","Cluster chi2",100,0,20,200,0,20);

  clusterProbPhoton = new TH2F("clusterProbPhoton","Cluster template probability",100,0,1,200,0,20);

  isoPhoE = new TH1F("isoPhotonE_Reco_Tru","Isolated Photon Energy",200,0,20);

  isoPhoChi2 = new TH2F("isoPhotonChi2","Isolated Photon Chi2 v. E",100,0,20,200,0,20);

  isoPhoProb = new TH2F("isoPhotonProb2","Isolated Photon Prob v. E",100,0,1,200,0,20);

  deltaR_E_invMass = new TH3F("deltaREinvMass","Opening Angle v. E v. InvMass",100,0,1,200,0,20,100,0,1);

  tsp_E = new TH2F("tspE","Transverse Shower Profile v. E",100,0,1,200,0,20);

  tsp_E_iso = new TH2F("tspEiso","Transverse Shower Profile Iso v. E",100,0,1,200,0,20);

  asym_E_invMass = new TH3F("asymEinvMass","Asymmetry v. E v. Invariant Mass",100,0,1,200,0,20,100,0,1);

  for(int i = 0; i < nEtaBins; i++)truth_pi0_E[i] = new TH1F(Form("truthPi0E_Eta%d",i),"truth pi0 energy",200,0,20);

  truth_eta_E = new TH1F("truthEtaE","truth eta energy",200,0,20);

  truth_dpho_E = new TH1F("truthDphoE","truth direct photon energy",200,0,20); 

  deltaR_E_truth_pi0 = new TH2F("deltaREtruthpi0","Opening angle truth pi0",100,0,1,500,0,50);

  asym_E_truth_pi0 = new TH2F("asymEtruthpi0","Photon asymmetry truth pi0",100,0,1,200,0,20);

  pi0E_truth_reco = new TH1F("pi0ETruthReco","Reco pi0/truth",100,0,2);

  invMass_eta = new TH3F("invMassEtaE","Invariant Mass vs. psuedorapidity bin",100,0,1,200,-1.2,1.2,200,0,20);

  eFrac_dr_primary = new TH2F("eFrac_dr_primary","Reco/Truth vs dr. primary",100,0,1,100,0,4);

  eFrac_dr_secondary = new TH2F("eFrac_dr_secondary","Reco/Truth vs dr. secondary",100,0,1,100,0,4);

  //ePi0InvMassEcut[0] = new TH3F("ePi0InvMassEcut0","Pi0 energy vs. Inv Mass vs. Min pho Energy Raw",500,0,50,500,-0.1,1,200,0,20);
  //ePi0InvMassEcut[1] = new TH3F("ePi0InvMassEcut1","Pi0 energy vs. Inv Mass vs. Min pho Energy Matched",500,0,50,500,-0.1,1,200,0,20);
  
  for(int i = 0; i < 2; i++)
    {
      for(int j = 0; j < nEtaBins; j++)
	{
	  ePi0InvMassEcut[i][j] = new TH3F(Form("ePi0InvMassEcut_%dMatch_Eta%d",i,j),Form("Pi0 energy vs. Inv Mass vs. Min pho Energy %dMatched Eta%d",i, j), 500,0,50,500,-0.1,1,200,0,20);
	}
    }
  
  dPhoChi2 = new TH3F("dphoChi2","Direct Photon Chi2 vs. Cluster Energy vs. Mother energy",100,0,20,200,0,20,200,0,20);
  dPhoProb = new TH3F("dphoProb","Direct Photon Prob vs. Cluster Energy vs. Mother energy",100,0,1,200,0,20,200,0,20);

  pi0Chi2 = new TH3F("pi0Chi2","Pi0 Daughter Chi2 vs. Cluster Energy vs. Mother energy",100,0,20,200,0,20,200,0,20);
  pi0Prob = new TH3F("pi0Prob","Pi0 Daughter Prob vs. Cluster Energy vs. Mother energy",100,0,1,200,0,20,200,0,20);

  etaChi2 = new TH3F("etaChi2","Eta Daughter Chi2 vs. Cluster Energy vs. Mother energy",100,0,20,200,0,20,200,0,20);
  etaProb = new TH3F("etaProb","Eta Daughter Prob vs. Cluster Energy vs. Mother energy",100,0,1,200,0,20,200,0,20);

  electronChi2 = new TH3F("eChi2","Electron  Chi2 vs. Cluster Energy vs. Mother energy",100,0,20,200,0,20,200,0,20);
  electronProb = new TH3F("eProb","Electron  Prob vs. Cluster Energy vs. Mother energy",100,0,1,200,0,20,200,0,20);

  hadronChi2 = new TH3F("hChi2","Hadron  Chi2 vs. Cluster Energy vs. Mother energy",100,0,20,200,0,20,200,0,20);
  hadronProb = new TH3F("hProb","Hadron  Prob vs. Cluster Energy vs. Mother energy",100,0,1,200,0,20,200,0,20);
   
  etaFrac = new TH2F("etaClusterEFrac","clusters from eta fractional energy",200,0,1.5,500,0,50);
   
  pi0Frac = new TH2F("pi0ClusterEFrac","clusters from pi0 fractional energy",200,0,1.5,500,0,50);

  invMassEPhi = new TH3F("invMassEPhi","invariant mass vs. Truth Energy vs. phi",500,-0.1,1,500,0,50,200,-M_PI,M_PI);
  
  unmatchedLocale = new TH3F("unmatchedLocale","location of unmatched truth photons",200,-5,5,180,-M_PI,M_PI,500,0,50);
  unmatchedE = new TH1F("unmatchedE","energy of unmatched truth photons",250,0,50);
  
  invMassPhoPi0 = new TH3F("invMassPhoPi0","invariant mass vs. Photon energy scale vs. pi0 energy scale",500,-0.1,1,100,0,2,100,0,2);
  
  //so that the histos actually get written out
  Fun4AllServer *se = Fun4AllServer::instance();
  se -> Print("NODETREE"); 
  hm = new Fun4AllHistoManager("MYHISTOS");

  se -> registerHistoManager(hm);

  se -> registerHisto(photonE -> GetName(),photonE);
  se -> registerHisto(clusterChi2 -> GetName(), clusterChi2);
  se -> registerHisto(clusterProbPhoton -> GetName(), clusterProbPhoton);
  se -> registerHisto(isoPhoE -> GetName(), isoPhoE);
  se -> registerHisto(isoPhoChi2 -> GetName(), isoPhoChi2);
  se -> registerHisto(isoPhoProb -> GetName(), isoPhoProb);
  se -> registerHisto(deltaR_E_invMass -> GetName(), deltaR_E_invMass);
  se -> registerHisto(asym_E_invMass -> GetName(), asym_E_invMass);
  se -> registerHisto(pi0E_truth_reco -> GetName(), pi0E_truth_reco);
  se -> registerHisto(invMass_eta -> GetName(), invMass_eta);
  //se -> registerHisto(ePi0InvMassEcut[0] -> GetName(), ePi0InvMassEcut[0]);
  //se -> registerHisto(ePi0InvMassEcut[1] -> GetName(), ePi0InvMassEcut[1]);
  for(int i = 0; i < 2; i++)
    {
      for(int j = 0; j < 2; j++)
	{
	  se -> registerHisto(ePi0InvMassEcut[i][j] -> GetName(), ePi0InvMassEcut[i][j]);
	}
    }
  se -> registerHisto(eFrac_dr_secondary -> GetName(), eFrac_dr_secondary);
  se -> registerHisto(eFrac_dr_primary -> GetName(), eFrac_dr_primary);
  se -> registerHisto(dPhoChi2 -> GetName(), dPhoChi2);
  se -> registerHisto(dPhoProb -> GetName(), dPhoProb);
  se -> registerHisto(pi0Chi2 -> GetName(), pi0Chi2);
  se -> registerHisto(pi0Prob -> GetName(), pi0Prob);
  se -> registerHisto(etaChi2 -> GetName(), etaChi2);
  se -> registerHisto(etaProb -> GetName(), etaProb);
  se -> registerHisto(electronChi2 -> GetName(), electronChi2);
  se -> registerHisto(electronProb -> GetName(), electronProb);
  se -> registerHisto(hadronChi2 -> GetName(), hadronChi2);
  se -> registerHisto(hadronProb -> GetName(), hadronProb);
  se -> registerHisto(unmatchedLocale -> GetName(), unmatchedLocale);
  se -> registerHisto(unmatchedE -> GetName(), unmatchedE);
  
  se -> registerHisto(invMassPhoPi0 -> GetName(), invMassPhoPi0);
  se -> registerHisto(invMassEPhi -> GetName(), invMassEPhi);
  
  
  out = new TFile(Outfile.c_str(),"RECREATE");


  //Call sumw2() on everything
  //letsSumw2();
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int cemcReco::InitRun(PHCompositeNode *topNode)
{
  std::cout << "cemcReco::InitRun(PHCompositeNode *topNode) No run dependence, doing nothing." << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int cemcReco::process_event(PHCompositeNode *topNode)
{


  if(!nEvent%100)
    {
      std::cout << "Processing Event: " << nEvent << std::endl;
    }

  //event-level information
  EventHeader *evtInfo = findNode::getClass<EventHeader>(topNode,"EventHeader");
  if(!evtInfo)
    {
      std::cout << PHWHERE << "cemc::process_event: EventHeader not in node tree" << std::endl;
      return 0;
    }

  //Information on clusters
  RawClusterContainer *clusterContainer = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_POS_COR_CEMC");
  //RawClusterContainer *clusterContainer = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_CEMC");
  if(!clusterContainer)
    {
      std::cout << PHWHERE << "cemc::process_event - Fatal Error - CLUSTER_POS_COR_CEMC node is missing. " << std::endl;

      return 0;
    }


  //Vertex information
  GlobalVertexMap *vtxContainer = findNode::getClass<GlobalVertexMap>(topNode,"GlobalVertexMap");
  if (!vtxContainer)
    {
      std::cout << PHWHERE << "cemc::process_event - Fatal Error - GlobalVertexMap node is missing. Please turn on the do_global flag in the main macro in order to reconstruct the global vertex." << std::endl;
      assert(vtxContainer);  // force quit

      return 0;
    }

  if (vtxContainer->empty())
    {
      std::cout << PHWHERE << "cemc::process_event - Fatal Error - GlobalVertexMap node is empty. Please turn on the do_global flag in the main macro in order to reconstruct the global vertex." << std::endl;
      return 0;
    }

  //More vertex information
  GlobalVertex *vtx = vtxContainer->begin()->second;
  if(!vtx)
    {

      std::cout << PHWHERE << "cemc::process_event Could not find vtx from vtxContainer"  << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  //Tracks for the isolation cone
  SvtxTrackMap *trackmap = findNode::getClass<SvtxTrackMap>(topNode, "SvtxTrackMap");
  if(!trackmap) 
    {
      if(trackErrorCount < 4)std::cout << PHWHERE << "cemc::process_event Could not find node SvtxTrackMap, not aborting, but reco direct photon information will not be available"  << std::endl;
      if(trackErrorCount == 4)std::cout << PHWHERE << "cemc::process_event Could not find node SvtxTrackMap for four events, it's probably missing from your file list, this message will no longer display" << std::endl;
      trackErrorCount++;
      //return Fun4AllReturnCodes::ABORTEVENT;
    }

  //Tower geometry node for location information
  RawTowerGeomContainer *towergeom = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_CEMC");
  if (!towergeom)
    {
      std::cout << PHWHERE << "cemc::process_event Could not find node TOWERGEOM_CEMC"  << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  //Raw tower information
  RawTowerContainer *towerContainer = findNode::getClass<RawTowerContainer>(topNode,"TOWER_CALIB_CEMC");
  if(!towerContainer)
    {
      std::cout << PHWHERE << "cemc::process_event Could not find node TOWER_CALIB_CEMC"  << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  //truth particle information
  PHG4TruthInfoContainer *truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
  if(!truthinfo)
    {
      std::cout << PHWHERE << "cemc::process_event Could not find node G4TruthInfo"  << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  //For pythia ancestory information
  PHHepMCGenEventMap *genEventMap = findNode::getClass<PHHepMCGenEventMap>(topNode, "PHHepMCGenEventMap");
  if(!genEventMap)
    {
      std::cout << PHWHERE << "cemc::process_event Could not find PHHepMCGenEventMap"  << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }
  //event level information of the above
  PHHepMCGenEvent *genEvent = genEventMap -> get(1);
  if(!genEvent)
    {
      std::cout << PHWHERE << "cemc::process_event Could not find PHHepMCGenEvent"  << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }
   
  HepMC::GenEvent *theEvent = genEvent -> getEvent();

  CaloEvalStack caloevalstack(topNode, "CEMC");
  CaloRawClusterEval *clustereval = caloevalstack.get_rawcluster_eval();
  if(!clustereval)
    {
      std::cout << PHWHERE << "cemc::process_event cluster eval not available"  << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    } 
  
  float mesonEtaMax = 0.3;
  float photonEtaMax = 1.1;
  PHG4Particle *maxPrimary = NULL;
  //This is how we iterate over items in the container.
  RawClusterContainer::ConstRange clusterEnd = clusterContainer -> getClusters();
  RawClusterContainer::ConstIterator clusterIter;

  //general pass over photons for info with only a mild energy cut to reduce noise
  std::vector<RawCluster*> goodRecoCluster;
  float minE = 0.3;
  float minDirpT = 4.;
  float isoConeRadius = 3;
  for(clusterIter = clusterEnd.first; clusterIter != clusterEnd.second; clusterIter++)
    {
      RawCluster *recoCluster = clusterIter -> second;

      CLHEP::Hep3Vector vertex(vtx->get_x(), vtx->get_y(), vtx->get_z());
      CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetECoreVec(*recoCluster, vertex);
      float clusE = E_vec_cluster.mag();
      float clus_eta = E_vec_cluster.pseudoRapidity();
      float clus_pt = E_vec_cluster.perp();
      //float clus_phi = E_vec_cluster.getPhi();

      if(clusE < minE) continue;
       
      maxPrimary = clustereval -> max_truth_primary_particle_by_energy(recoCluster);
      if(maxPrimary) 
	{
	  
	  if(maxPrimary -> get_pid() == 11 || maxPrimary -> get_pid() == -11)
	    {
	      electronChi2 -> Fill(recoCluster -> get_chi2(), clusE, maxPrimary -> get_e());
	      electronProb -> Fill(recoCluster -> get_prob(), clusE, maxPrimary -> get_e());
	    }
	  else if(maxPrimary -> get_pid() == 211 || maxPrimary -> get_pid() == -211)
	    {
	      hadronChi2 -> Fill(recoCluster -> get_chi2(), clusE, maxPrimary -> get_e());
	      hadronProb -> Fill(recoCluster -> get_prob(), clusE, maxPrimary -> get_e());
	    }
	  if(maxPrimary -> get_pid() == 22)
	    {
	      for(HepMC::GenEvent::particle_const_iterator p = theEvent -> particles_begin(); p != theEvent -> particles_end(); ++p)
		{
		  assert(*p);
	   
		  if((*p) -> barcode() == maxPrimary -> get_barcode())
		    {
		      for(HepMC::GenVertex::particle_iterator mother = (*p)->production_vertex()->particles_begin(HepMC::parents);
			  mother != (*p)->production_vertex()->particles_end(HepMC::parents); ++mother)
			{
			  HepMC::FourVector moMomentum = (*mother) -> momentum();
			  float e = moMomentum.e();
			  //float eta = moMomentum.pseudoRapidity();
			  if((*mother) -> pdg_id() == 22)
			    {
			      dPhoChi2 -> Fill(recoCluster -> get_chi2(), clusE, e);
			      dPhoProb -> Fill(recoCluster -> get_prob(), clusE, e);
			    }
			  else if((*mother) -> pdg_id() == 111)
			    {
			      pi0Chi2 -> Fill(recoCluster -> get_chi2(), clusE, e);
			      pi0Prob -> Fill(recoCluster -> get_prob(), clusE, e);
			      pi0Frac -> Fill(clusE/e, e);
			    }
			  else if((*mother) -> pdg_id() == 221)
			    {
			      etaChi2 -> Fill(recoCluster -> get_chi2(), clusE, e);
			      etaProb -> Fill(recoCluster -> get_prob(), clusE, e);
			      etaFrac -> Fill(clusE/e, e);

			    }
		       
			}
		    }
		}
	    }
	}
					    
		       

      photonE -> Fill(clusE);
      //clusterChi2 -> Fill(recoCluster -> get_chi2(), clusE);
      //clusterProbPhoton -> Fill(recoCluster -> get_prob(), clusE);

      //Calculate the transverse shower profile, how spread out the energy in the cluster is
      //more spread out -> Most likely pi0 or other meson decay
      //less spread out -> iso photon 
      float tsp = calculateTSP(recoCluster, clusterContainer, towerContainer, towergeom, vtx);

      tsp_E -> Fill(tsp, clusE);


      //check to see if this is a high p_T isolate photon -> possibly a direct photon
      if(clus_pt > minDirpT) 
	{


	  //make sure entire isocone is contained within sPHENIX acceptance
	  if((fabs(clus_eta) < 1.0 - isoConeRadius) && trackmap)
	    {
	      float energyInCone = coneSum(recoCluster,clusterContainer, trackmap, isoConeRadius, vtx);

	      if(energyInCone < 0.1*clusE)
		{
		  isoPhoChi2 -> Fill(recoCluster -> get_chi2(), clusE);
		  isoPhoProb -> Fill(recoCluster -> get_prob(), clusE);
		  isoPhoE -> Fill(clusE);
		  tsp_E_iso -> Fill(tsp, clusE);
		  //isIso = true;
		}
	    }
	}
      goodRecoCluster.push_back(recoCluster);
    }

  //And now we'll loop over all the good clusters to reconstruct pi0's and etas
  CLHEP::Hep3Vector vertex(vtx->get_x(), vtx->get_y(), vtx->get_z());
  for(int i = 0; i < (int)goodRecoCluster.size(); i++)
    {

      
      CLHEP::Hep3Vector E_vec_cluster1 = RawClusterUtility::GetECoreVec(*goodRecoCluster[i], vertex);

      TLorentzVector pho1, pho2, pi0; 
      pho1.SetPtEtaPhiE(E_vec_cluster1.perp(), E_vec_cluster1.pseudoRapidity(), E_vec_cluster1.getPhi(),  E_vec_cluster1.mag());



      for(int j = 0; j <(int)goodRecoCluster.size(); j++)
	{
	 
	  if(j <= i) continue; 
	  CLHEP::Hep3Vector E_vec_cluster2 = RawClusterUtility::GetECoreVec(*goodRecoCluster[j], vertex);

	  pho2.SetPtEtaPhiE(E_vec_cluster2.perp(), E_vec_cluster2.pseudoRapidity(), E_vec_cluster2.getPhi(),  E_vec_cluster2.mag());

	  pi0 = pho1 + pho2; 

	  deltaR_E_invMass -> Fill(pho1.DeltaR(pho2), pi0.Energy(), pi0.M());

	  float asym = abs(pho1.Energy() - pho2.Energy())/pi0.Energy();

	  asym_E_invMass -> Fill(asym, pi0.Energy(), pi0.M());

	  invMass_eta -> Fill(pi0.M(), pi0.Eta(), pi0.Energy());

	  if(abs(pho2.PseudoRapidity()) < photonEtaMax && abs(pho1.PseudoRapidity()) < photonEtaMax &&  pi0.PseudoRapidity() < mesonEtaMax)
	    {
	      int etaBin = -1;
	      if(pho1.Energy() >= pho2.Energy()) etaBin = getEtaBin(pho1.PseudoRapidity());
	      else etaBin = getEtaBin(pho2.PseudoRapidity());
	      if(etaBin>=0)ePi0InvMassEcut[0][etaBin] -> Fill(pi0.Energy(), pi0.M(),std::min(pho1.Energy(), pho2.Energy()));
	      ePi0InvMassEcut[0][nEtaBins-1] -> Fill(pi0.Energy(), pi0.M(),std::min(pho1.Energy(), pho2.Energy()));
	    }
	}
    }

  //truth pi0 reconstruction handled via the PHHepMCGenEventMap
  //This accounts for/bypasses the fact that pi0 decay is handled
  //in general by the event generator. This method is fine for things like
  //purity and kinematic studies, but if you need to get the *total* 
  //yield of pi0's, it isn't enough, see the primary and secondary 
  //particle loops below


  std::vector<PHG4Particle*> phoPi0;
  std::vector<PHG4Particle*> phoEta;

  std::vector<int> mBarCodePi0;
  std::vector<HepMC::GenVertex*> vtxIDpi0;

  std::vector<int> mBarCodeEta;
  std::vector<HepMC::GenVertex*> vtxIDEta;

  PHG4TruthInfoContainer::Range truthRange = truthinfo->GetPrimaryParticleRange();
  PHG4TruthInfoContainer::ConstIterator truthIter;
  //Here's where we link decay photons categorized as "primary" to their parent pi0's
  //from the HepMC event log
  for(truthIter = truthRange.first; truthIter != truthRange.second; truthIter++)
    {
      PHG4Particle *truthPar = truthIter->second;
      if(truthPar -> get_pid() != 22) continue;
      if(truthinfo -> isEmbeded(truthPar -> get_track_id()) != 1) continue;
    
      for(HepMC::GenEvent::particle_const_iterator p = theEvent -> particles_begin(); p != theEvent -> particles_end(); ++p)
	{
	  assert(*p);
	  if((*p) -> pdg_id()  != 22) continue;
	      
	  if((*p) -> barcode() == truthPar -> get_barcode())
	    {
	        
	      for (HepMC::GenVertex::particle_iterator mother = (*p)->production_vertex()->particles_begin(HepMC::parents);
		   mother != (*p)->production_vertex()->particles_end(HepMC::parents); ++mother)
		{
		   
		  HepMC::FourVector moMomentum = (*mother) -> momentum();
		  float e = moMomentum.e();
		  float eta = moMomentum.pseudoRapidity();
		  if((*mother) -> pdg_id() == 22 && abs(eta) < photonEtaMax) 
		    {
		      
		      truth_dpho_E -> Fill(e);
		      
		    }
		  else if((*mother) -> pdg_id() == 111 )
		    {

		      phoPi0.push_back(truthPar);//these photons will be unique
		      vtxIDpi0.push_back((*mother) -> production_vertex());
		      
		      mBarCodePi0.push_back((*mother) -> barcode());//barcodes will not be unique, as they'll be added per photon
		    }
		  else if((*mother) -> pdg_id() == 221 )
		    {
		      
		      if(abs(eta) < mesonEtaMax &&  abs(getEta(truthPar)) < photonEtaMax && !checkBarcode((*mother) -> barcode(),mBarCodeEta))truth_eta_E -> Fill(e);
		      phoEta.push_back(truthPar);
		      vtxIDEta.push_back((*mother) -> production_vertex());
		      mBarCodeEta.push_back((*mother) -> barcode());
		    }
		  
		}
	    }
	}
    }

  //And then, we have to loop over the photons in the secondary particle list and trace them back to a pi0
  truthRange = truthinfo -> GetSecondaryParticleRange();
  for(truthIter = truthRange.first; truthIter != truthRange.second; truthIter++)
    {
      PHG4Particle *truthPar = truthIter -> second;
      
      while(truthPar -> get_parent_id() != 0)
	{
	  
	  PHG4Particle *parent = truthinfo -> GetParticle(truthPar -> get_parent_id());
	  if(parent -> get_parent_id() == 0) break; //this is the decay product we want, so we don't want to reassign it
	  truthPar = parent;
	}
	
      if(truthPar -> get_pid() != 22 ) 
       	{
       	  //std::cout << "Killed by final photon pid cut" << std::endl;
       	  continue;
       	}
      PHG4Particle *pi0 = truthinfo -> GetParticle(truthPar -> get_parent_id());
      if(pi0 -> get_pid() != 111)
	{
	  //std::cout << "Killed by pi0 pid cut" << std::endl;
	  continue; //secondary not descendant from pi0
	}
      if(abs(getEta(pi0)) > mesonEtaMax) 
	{
	  //std::cout << "Killed by pi0 eta cut" << std::endl;
	  continue;//go ahead and make pi0 kinematic cut
	}
      if(abs(getEta(truthPar)) > photonEtaMax) continue; //don't cut on photons just yet, save for reconstruction
      
      //check to see if we've already gotten this photon and we're just at another shower element
     
      if(checkBarcode(truthPar -> get_barcode(), phoPi0)) 
	{
	  //std::cout << "Killed by photon barcode cut" << std::endl;
	  continue;
	}
      //sneaky overload
      if(checkBarcode(pi0 -> get_barcode(), mBarCodePi0)) continue;
      
      mBarCodePi0.push_back(pi0 -> get_barcode());
      phoPi0.push_back(truthPar);
    }
      

  
  //Loop over our recovered decay photons and 
  //extract kinematic information.
  //First for the pi0's
  for(int i = 0; i < (int)phoPi0.size(); i++)
    {
      for(int j = 0; j < (int)phoPi0.size(); j++)
	{
	  if(j <= i) 
	    {
	      continue;
	    }
	  if(mBarCodePi0.at(i) != mBarCodePi0.at(j))//match mother particles
	    {
	      continue;
	    }
	  if(abs(getEta(phoPi0.at(i))) >= photonEtaMax || abs(getEta(phoPi0.at(j))) >= photonEtaMax)
	    {
	      continue;
	    }

	  
	  TLorentzVector e1Vec, e2Vec;
	  e1Vec.SetPxPyPzE(phoPi0.at(i) -> get_px(),phoPi0.at(i) -> get_py(), phoPi0.at(i) -> get_pz(), phoPi0.at(i) -> get_e());
	  e2Vec.SetPxPyPzE(phoPi0.at(j) -> get_px(),phoPi0.at(j) -> get_py(), phoPi0.at(j) -> get_pz(), phoPi0.at(j) -> get_e());
	  
	  
	  float dr = e1Vec.DeltaR(e2Vec); 
	    
	  float e1, e2;
	  e1 = phoPi0.at(i) -> get_e();
	  e2 = phoPi0.at(j) -> get_e();
	  
	  float asym = abs(e1-e2)/(e1+e2);
	  
	  TLorentzVector pi0 = e1Vec + e2Vec;
	  if(abs(pi0.PseudoRapidity()) >= mesonEtaMax) 
	    {
	      continue;
	    }
	  int etaBin = -1;
	  if(e1Vec.Energy() >= e2Vec.Energy()) etaBin = getEtaBin(e1Vec.PseudoRapidity());
	  else etaBin = getEtaBin(e2Vec.PseudoRapidity());
	  if(etaBin>=0)truth_pi0_E[etaBin] -> Fill(pi0.Energy());//fill individual eta bin
	  truth_pi0_E[nEtaBins-1] -> Fill(pi0.Energy());//fill one for all eta bins

	  asym_E_truth_pi0 -> Fill(asym, pi0.E());
	  deltaR_E_truth_pi0 -> Fill(dr, pi0.E());
	  
	  RawCluster *best_cluster1 = clustereval -> best_cluster_from(phoPi0.at(i));
	  RawCluster *best_cluster2 = clustereval -> best_cluster_from(phoPi0.at(j));
	  
	  if(!best_cluster1 || !best_cluster2)
	    {
	      if(!best_cluster1) 
		{
		  
		  unmatchedLocale -> Fill(getEta(phoPi0.at(i)), getPhi(phoPi0.at(i)), pi0.E());
		  unmatchedE -> Fill(phoPi0.at(i) -> get_e());
		}
	      if(!best_cluster2) 
		{
		 
		  unmatchedLocale -> Fill(getEta(phoPi0.at(j)), getPhi(phoPi0.at(j)), pi0.E());
		  unmatchedE -> Fill(phoPi0.at(j) -> get_e());
		}
	      int etaBin = -1;
	      if(e1Vec.Energy() >= e2Vec.Energy()) etaBin = getEtaBin(e1Vec.PseudoRapidity());
	      else etaBin = getEtaBin(e2Vec.PseudoRapidity());
	      if(etaBin>=0)ePi0InvMassEcut[1][etaBin] -> Fill(pi0.Energy(), 0., std::min(e1Vec.Energy(), e2Vec.Energy()));
	      ePi0InvMassEcut[1][nEtaBins-1] -> Fill(pi0.Energy(), 0., std::min(e1Vec.Energy(), e2Vec.Energy()));
	    }
	  else
	    {
	      if(best_cluster1 -> get_id() == best_cluster2 -> get_id()) continue; 
		
	      
	      CLHEP::Hep3Vector E_vec_cluster1 = RawClusterUtility::GetECoreVec(*best_cluster1, vertex);
	      CLHEP::Hep3Vector E_vec_cluster2 = RawClusterUtility::GetECoreVec(*best_cluster2, vertex);
	  
	      TLorentzVector clusE1, clusE2, clusPi0;
	      clusE1.SetPtEtaPhiE(E_vec_cluster1.perp(), E_vec_cluster1.pseudoRapidity(), E_vec_cluster1.getPhi(),  E_vec_cluster1.mag());
	      clusE2.SetPtEtaPhiE(E_vec_cluster2.perp(), E_vec_cluster2.pseudoRapidity(), E_vec_cluster2.getPhi(),  E_vec_cluster2.mag());

	      clusPi0 = clusE1 + clusE2;
	      
	      int etaBin = -1;
	      if(clusE1.Energy() >= clusE2.Energy()) etaBin = getEtaBin(clusE1.PseudoRapidity());
	      else etaBin = getEtaBin(clusE2.PseudoRapidity());
	      if(etaBin>=0)ePi0InvMassEcut[1][etaBin] -> Fill(pi0.Energy(), clusPi0.M(),std::min(clusE1.Energy(), clusE2.Energy()));
	      ePi0InvMassEcut[1][nEtaBins-1] -> Fill(pi0.Energy(), clusPi0.M(),std::min(clusE1.Energy(), clusE2.Energy()));
	      
	      invMassEPhi -> Fill(clusPi0.M(), pi0.Energy(), clusE1.Phi());
	      invMassEPhi -> Fill(clusPi0.M(), pi0.Energy(), clusE2.Phi());	
	
	      invMassPhoPi0 -> Fill(clusPi0.M(), clusPi0.Energy()/pi0.Energy(), clusE1.Energy()/e1Vec.Energy());
	      invMassPhoPi0 -> Fill(clusPi0.M(), clusPi0.Energy()/pi0.Energy(), clusE2.Energy()/e2Vec.Energy());
		      
	    }
	}
    }
  
  //collecting pi0's that enter GEANT's volume. This is actually now handled by 
  //collecting photons from the secondary photon list.
  /*truthRange = truthinfo->GetPrimaryParticleRange();
  for(truthIter = truthRange.first; truthIter != truthRange.second; truthIter++)
    {
      PHG4Particle *truthPar = truthIter->second;
      if(truthPar -> get_pid() == 111 && truthPar -> get_parent_id() == 0)//it's a primary pi0, let's take a look
	{
	  if(abs(getEta(truthPar)) < mesonEtaMax)
	    {
	      //truth_pi0_E -> Fill(truthPar -> get_e());
	    }
	}
	}*/

  PHG4TruthInfoContainer::Range truthRangeDecay1 = truthinfo->GetSecondaryParticleRange();
  PHG4TruthInfoContainer::ConstIterator truthIterDecay1;
  
 
  //Get decays from other particles to pi0's
  for(truthIterDecay1 = truthRangeDecay1.first; truthIterDecay1 != truthRangeDecay1.second; truthIterDecay1++)
    {
      PHG4Particle *truthDecay1 = truthIterDecay1 -> second;

      if(truthDecay1 -> get_parent_id() != 0 && truthDecay1 -> get_pid() == 111 && abs(getEta(truthDecay1)) < mesonEtaMax) //want decay pi0's
	{
	  //truth_pi0_E -> Fill(truthDecay1 -> get_e());
	}
    }
	
  	  


  nEvent++;
  goodRecoCluster.clear();
  phoPi0.clear();
  phoEta.clear();
  mBarCodePi0.clear();
  vtxIDpi0.clear();
  mBarCodeEta.clear();
 
  vtxIDEta.clear();
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int cemcReco::ResetEvent(PHCompositeNode *topNode)
{
  //std::cout << "cemcReco::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int cemcReco::EndRun(const int runnumber)
{
  std::cout << "cemcReco::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int cemcReco::End(PHCompositeNode *topNode)
{
  std::cout << "cemcReco::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  out -> cd();
  
  photonE -> Write();
  clusterChi2 -> Write();
  clusterProbPhoton -> Write();
  isoPhoE -> Write();
  isoPhoChi2 -> Write();
  isoPhoProb -> Write();
  deltaR_E_invMass -> Write();
  tsp_E -> Write();
  tsp_E_iso -> Write();
  asym_E_truth_pi0 -> Write();
  deltaR_E_truth_pi0 -> Write();
  for(int i = 0; i < nEtaBins; i++) truth_pi0_E[i] -> Write();
  truth_eta_E -> Write();
  truth_dpho_E -> Write();
  asym_E_invMass -> Write();
  pi0E_truth_reco -> Write();
  invMass_eta -> Write();
  eFrac_dr_secondary -> Write();
  eFrac_dr_primary -> Write();
  //ePi0InvMassEcut[0] -> Write();
  //ePi0InvMassEcut[1] -> Write();
  for(int i = 0; i < 2; i++)
    {
      for(int j = 0; j < nEtaBins; j++)
	{
	  ePi0InvMassEcut[i][j] -> Write();
	}
    }

  dPhoChi2 -> Write();
  dPhoProb -> Write();
  pi0Chi2 -> Write();
  pi0Prob -> Write();
  etaChi2 -> Write();
  etaProb -> Write();
  electronChi2 -> Write();
  electronProb -> Write(); 
  hadronChi2 -> Write();
  hadronProb -> Write();
  pi0Frac -> Write();
  etaFrac -> Write();
  unmatchedLocale -> Write();
  unmatchedE -> Write();
  invMassPhoPi0 -> Write();
  invMassEPhi -> Write();

  //out -> Close();
  
  //delete out;
  //delete caloevalstack;

  //hm -> dumpHistos(Outfile.c_str(),"UPDATE");
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int cemcReco::Reset(PHCompositeNode *topNode)
{
  std::cout << "cemcReco::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void cemcReco::Print(const std::string &what) const
{
  std::cout << "cemcReco::Print(const std::string &what) const Printing info for " << what << std::endl;
}
//____________________________________________________________________________..
float cemcReco::coneSum(RawCluster *cluster,
			RawClusterContainer *cluster_container,
			SvtxTrackMap *trackmap,
			float coneradius,
			GlobalVertex *vtx)
{
  float energyptsum = 0;

  CLHEP::Hep3Vector vertex(vtx->get_x(), vtx->get_y(), vtx->get_z());
  CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetECoreVec(*cluster, vertex);

  RawClusterContainer::ConstRange begin_end = cluster_container->getClusters();
  RawClusterContainer::ConstIterator clusiter;

  for (clusiter = begin_end.first;
       clusiter != begin_end.second;
       ++clusiter)
    {
      RawCluster *conecluster = clusiter->second;

      //check to make sure that the candidate isolated photon isn't being counted in the energy sum
      if (conecluster->get_energy() == cluster->get_energy())
	if (conecluster->get_phi() == cluster->get_phi())
	  if (conecluster->get_z() == cluster->get_z())
	    continue;

      CLHEP::Hep3Vector E_vec_conecluster = RawClusterUtility::GetECoreVec(*conecluster, vertex);

      float cone_pt = E_vec_conecluster.perp();

      if (cone_pt < 0.2)
	continue;

      float cone_e = conecluster->get_energy();
      float cone_eta = E_vec_conecluster.pseudoRapidity();
      float cone_phi = E_vec_conecluster.getPhi();

      float dphi = cluster->get_phi() - cone_phi;
      if (dphi < -1 * pi)
	dphi += 2. * pi;
      if (dphi > pi)
	dphi -= 2. * pi;

      float deta = E_vec_cluster.pseudoRapidity() - cone_eta;

      float radius = sqrt(dphi * dphi + deta * deta);

      if (radius < coneradius)
	{
	  energyptsum += cone_e;
	}
    }

  for (SvtxTrackMap::Iter iter = trackmap->begin(); iter != trackmap->end(); ++iter)
    {
      SvtxTrack *track = iter->second;

      float trackpx = track->get_px();
      float trackpy = track->get_py();
      float trackpt = sqrt(trackpx * trackpx + trackpy * trackpy);
      if (trackpt < 0.2)
	continue;
      float trackphi = track->get_phi();
      float tracketa = track->get_eta();
      float dphi = E_vec_cluster.getPhi() - trackphi;
      float deta = E_vec_cluster.pseudoRapidity() - tracketa;
      float radius = sqrt(dphi * dphi + deta * deta);

      if (radius < coneradius)
	{
	  energyptsum += trackpt;
	}
    }

  return energyptsum;
}

//____________________________________________________________________________..
float cemcReco::calculateTSP(RawCluster *cluster,
			     RawClusterContainer *clusterContainer, 
			     RawTowerContainer *towerContainer,
			     RawTowerGeomContainer *towerGeo,
			     GlobalVertex *vtx
			     )
{
  
  CLHEP::Hep3Vector vertex(vtx->get_x(), vtx->get_y(), vtx->get_z());
  CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetECoreVec(*cluster, vertex);
  float clusE = E_vec_cluster.mag();
  float clusEta = E_vec_cluster.pseudoRapidity();
  //float clus_pt = E_vec_cluster.perp();
  float clusPhi = E_vec_cluster.getPhi();

  RawCluster::TowerConstRange towers = cluster->get_towers();
  RawCluster::TowerConstIterator toweriter;

  float denom = 0;
  for (toweriter = towers.first; toweriter != towers.second; ++toweriter)
    {
      
      RawTower *tower = towerContainer->getTower(toweriter->first);

      //int towereta = tower->get_bineta();
      //int towerphi = tower->get_binphi();
      double towerEnergy = tower->get_energy();

      //Get eta and phi bin from the rawTowerContainer
      //then transform it to coordinates with the tower geometry object
      int phibin = tower->get_binphi();
      int etabin = tower->get_bineta();
      double phi = towerGeo->get_phicenter(phibin);
      double eta = towerGeo->get_etacenter(etabin);

      float r = sqrt(pow(clusEta - eta,2) + pow(clusPhi - phi,2));
      denom += towerEnergy*pow(r,1.5);
      
    }

  return clusE/denom;
}
//____________________________________________________________________________..
/*void cemcReco::letsSumw2()
{
  photonE -> Sumw2();
  clusterChi2 -> Sumw2();
  clusterProbPhoton -> Sumw2();
  isoPhoE -> Sumw2();
  isoPhoChi2 -> Sumw2();
  isoPhoProb -> Sumw2();
  deltaR_E_invMass -> Sumw2();
  asym_E_invMass -> Sumw2();
  tsp_E -> Sumw2();
  tsp_E_iso -> Sumw2();
  truth_pi0_E -> Sumw2();
  truth_eta_E -> Sumw2();
  truth_dpho_E -> Sumw2(); 
  asym_E_truth_pi0 -> Sumw2();
  deltaR_E_truth_pi0  -> Sumw2();
  pi0E_truth_reco -> Sumw2();
  eFrac_dr_primary -> Sumw2();
  eFrac_dr_secondary -> Sumw2();
  // ePi0InvMassEcut[0] -> Sumw2();
  etaFrac -> Sumw2();
  pi0Frac -> Sumw2();
  }*/
//____________________________________________________________________________..
bool cemcReco::compareVertices(HepMC::FourVector hepMCvtx, PHG4VtxPoint *g4vtx)
{
  float g4vtxz, g4vtxy, g4vtxx;//geant vertices
  g4vtxx =  g4vtx -> get_x();
  g4vtxy =  g4vtx -> get_y();
  g4vtxz =  g4vtx -> get_z();
  std::cout << "g4 x: " << g4vtxx << "; g4 y: " << g4vtxy << "; g4 z: " << g4vtxz << std::endl;
  
  float hepVtxx, hepVtxy, hepVtxz;
  hepVtxx = hepMCvtx.x();
  hepVtxy = hepMCvtx.y();
  hepVtxz = hepMCvtx.z();
  std::cout << "hep x: " << hepVtxx << "; hep y: " << hepVtxy << "; hep z: " << hepVtxz << std::endl;


  if(g4vtxx != hepVtxx  || g4vtxy != hepVtxy  || g4vtxz != hepVtxz ) return false;
  
  return true;  
  
}
//____________________________________________________________________________..
/*float getpT(PHGParticle *particle)
  {
  float px = particle -> get_px();
  float py = particle -> get_py();
  
  float pt = sqrt(pow(px,2) + pow(py,2));
  return pt;
  }*/
//____________________________________________________________________________..
float cemcReco::getEta(PHG4Particle *particle)
{
  float px = particle -> get_px();
  float py = particle -> get_py();
  float pz = particle -> get_pz();
  float p = sqrt(pow(px,2) + pow(py,2) + pow(pz,2));

  return 0.5*log((p+pz)/(p-pz));
}
//____________________________________________________________________________.. 
float cemcReco::getPhi(PHG4Particle *particle)
{
  float phi = atan2(particle -> get_py(),particle -> get_px());
  return phi;
}
//____________________________________________________________________________.. 
bool cemcReco::checkBarcode(int motherBarcode, std::vector<int> &motherBarcodes)
{
  bool isRepeated = false;
  for(int i = 0; i < (int)motherBarcodes.size(); i++)
    {
      if(motherBarcode == motherBarcodes.at(i)) isRepeated = true;
    }
  
  return isRepeated;
}
//____________________________________________________________________________.. 
bool cemcReco::checkBarcode(int motherBarcode, std::vector<PHG4Particle*> &motherBarcodes)
{
  bool isRepeated = false;
  
  for(int i = 0; i < (int)motherBarcodes.size(); i++)
    {
      if(motherBarcode == motherBarcodes.at(i) -> get_barcode()) isRepeated = true;
    }
  
  return isRepeated;
}
//____________________________________________________________________________.. 
int cemcReco::getEtaBin(float eta)
{
  for(int i = 0; i < nEtaBins-1; i++)//only go up to nEtaBins - 1 because the last bin is for all eta
    {
      if(abs(eta) < (i+1)/(float)(nEtaBins-1) * 1.1 && abs(eta) >= (i+1-1)/(float)(nEtaBins-1) * 1.1) 
	{
	  return i;
	}
    }
 return -1;
}
  
