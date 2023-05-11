#include "pi0Efficiency.h"

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
#include <HepMC/GenEvent.h>
#include <HepMC/GenParticle.h>
#include <HepMC/GenVertex.h>
#include <HepMC/IteratorRange.h>
#include <HepMC/SimpleVector.h> 
#include <HepMC/GenParticle.h>
#include <CLHEP/Geometry/Point3D.h>

//____________________________________________________________________________..
pi0Efficiency::pi0Efficiency(const std::string &name, const std::string &outName):
SubsysReco(name),
  OutFile(outName)
{
  std::cout << "pi0Efficiency::pi0Efficiency(const std::string &name) Calling ctor" << std::endl;
 
 
  for(int j = 0; j < nEtaBins; j++) ePi0InvMassEcut[j] = 0;
    
  photonE = 0;

  for(int i = 0; i < nEtaBins; i++)prim_pi0_E[i] = 0;

  clusterE = 0;

  truthPi0EAsym = 0;

  truthPi0EDeltaR = 0;
  
  hMassRat = 0;

  pi0EScale = 0;
}

//____________________________________________________________________________..
pi0Efficiency::~pi0Efficiency()
{
  std::cout << "pi0Efficiency::~pi0Efficiency() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
int pi0Efficiency::Init(PHCompositeNode *topNode)
{
  std::cout << "pi0Efficiency::Init(PHCompositeNode *topNode) Initializing" << std::endl;

  
  
  for(int j = 0; j < nEtaBins; j++)
    {
      ePi0InvMassEcut[j] = new TH3F(Form("ePi0InvMassEcut_Eta%d",j),Form("Pi0 energy vs. Inv Mass vs. Min pho Energy Eta%d", j), 500,0,50,500,-0.1,1,200,0,20);
    }
    
  clusterE = new TH1F("clusterE","Cluster Energy",200,0,20);
  
  for(int i = 0; i < nEtaBins; i++)prim_pi0_E[i] = new TH1F(Form("primPi0E_Eta%d",i),"Primary pi0 Energy",200,0,20);
  
  photonE = new TH1F("photonE","Decay Photon Energy",200,0,20);
  
  pi0EScale = new TH2F("pi0EScale","Pi0 energy fraction",200,0,2,200,0,20);
  
  truthPi0EDeltaR = new TH2F("truthPi0EDeltaR","truth pi0 energy versus decay opening angle",200,0,20,100,0,.5);
  
  truthPi0EAsym = new TH2F("truthPi0EAsym","truth pi0 energy vs. decay energy asymmetry",200,0,20,200,0,1);
  
  hMassRat = new TH1F("hMassRat","ratio of pi0 mass reco from truth photons to primary mass",200,0,2);
 
  Fun4AllServer *se = Fun4AllServer::instance();
  se -> Print("NODETREE"); 
  hm = new Fun4AllHistoManager("MYHISTOS");
  
  se -> registerHistoManager(hm);
   
  se -> registerHisto(clusterE -> GetName(), clusterE);
  
  for(int i = 0; i < nEtaBins; i++)se -> registerHisto(prim_pi0_E[i] -> GetName(), prim_pi0_E[i]);
  
  se -> registerHisto(photonE -> GetName(), photonE);

  for(int j = 0; j < nEtaBins; j++) se -> registerHisto(ePi0InvMassEcut[j] -> GetName(), ePi0InvMassEcut[j]);
  
  se -> registerHisto(truthPi0EAsym->GetName(), truthPi0EAsym);
  
  se -> registerHisto(hMassRat -> GetName(), hMassRat);

  se -> registerHisto(truthPi0EDeltaR -> GetName(), truthPi0EDeltaR);

  out = new TFile(OutFile.c_str(),"RECREATE");
   
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int pi0Efficiency::InitRun(PHCompositeNode *topNode)
{
  std::cout << "pi0Efficiency::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int pi0Efficiency::process_event(PHCompositeNode *topNode)
{
  std::cout << "pi0Efficiency::process_event(PHCompositeNode *topNode) Processing Event" << std::endl;
  
 
  //Information on clusters
  RawClusterContainer *clusterContainer = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_POS_COR_CEMC");
  //RawClusterContainer *clusterContainer = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_CEMC");
  if(!clusterContainer)
    {
      std::cout << PHWHERE << "pi0Efficiency::process_event - Fatal Error - CLUSTER_POS_COR_CEMC node is missing. " << std::endl;

      return 0;
    }


  //Vertex information
  GlobalVertexMap *vtxContainer = findNode::getClass<GlobalVertexMap>(topNode,"GlobalVertexMap");
  if (!vtxContainer)
    {
      std::cout << PHWHERE << "pi0Efficiency::process_event - Fatal Error - GlobalVertexMap node is missing. Please turn on the do_global flag in the main macro in order to reconstruct the global vertex." << std::endl;
      assert(vtxContainer);  // force quit

      return 0;
    }

  if (vtxContainer->empty())
    {
      std::cout << PHWHERE << "pi0Efficiency::process_event - Fatal Error - GlobalVertexMap node is empty. Please turn on the do_global flag in the main macro in order to reconstruct the global vertex." << std::endl;
      return 0;
    }

  //More vertex information
  GlobalVertex *vtx = vtxContainer->begin()->second;
  if(!vtx)
    {

      std::cout << PHWHERE << "pi0Efficiency::process_event Could not find vtx from vtxContainer"  << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  //Tower geometry node for location information
  RawTowerGeomContainer *towergeom = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_CEMC");
  if (!towergeom)
    {
      std::cout << PHWHERE << "pi0Efficiency::process_event Could not find node TOWERGEOM_CEMC"  << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  //Raw tower information
  RawTowerContainer *towerContainer = findNode::getClass<RawTowerContainer>(topNode,"TOWER_CALIB_CEMC");
  if(!towerContainer)
    {
      std::cout << PHWHERE << "pi0Efficiency::process_event Could not find node TOWER_CALIB_CEMC"  << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  //truth particle information
  PHG4TruthInfoContainer *truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
  if(!truthinfo)
    {
      std::cout << PHWHERE << "pi0Efficiency::process_event Could not find node G4TruthInfo"  << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }
  
  int firstphotonflag = 0;
  int firstfirstphotonflag = 0;
  int secondphotonflag = 0;
  int secondsecondphotonflag = 0;
  
  PHG4TruthInfoContainer::Range truthRangeDecay1 = truthinfo->GetSecondaryParticleRange();
  PHG4TruthInfoContainer::ConstIterator truthIterDecay1;

  float photonEtaMax = 1.1;
  float mesonEtaMax = 0.3;
  TLorentzVector photon1;
  TLorentzVector photon2;
  
  for(truthIterDecay1 = truthRangeDecay1.first; truthIterDecay1 != truthRangeDecay1.second; truthIterDecay1++)
    {
      
      PHG4Particle *decay = truthIterDecay1 -> second;
      
      int dumtruthpid = decay -> get_pid();
      int dumparentid = decay -> get_parent_id();
      
      //if the parent is the pi0 and it's a photon and we haven't marked one yet
      if(dumparentid == 1 && dumtruthpid == 22 && !firstphotonflag)
	{
	  if(abs(getEta(decay)) > photonEtaMax) 
	    {
	      std::cout << "Photon 1 outside acceptance " << std::endl;
	      return 0; //decay product outside acceptance.
	    }
	  photon1.SetPxPyPzE(decay -> get_px(), decay -> get_py(), decay -> get_pz(), decay -> get_e());
	  
	  firstphotonflag = 1;
	}
      
      if(dumparentid == 1 && dumtruthpid == 22 && firstphotonflag && firstfirstphotonflag)
	{
	  if(abs(getEta(decay)) > photonEtaMax) 
	    {
	      std::cout << "Photon 2 outside acceptance " << std::endl;
	      return 0; //decay product outside acceptance
	    }
	  photon2.SetPxPyPzE(decay -> get_px(), decay -> get_py(), decay -> get_pz(), decay -> get_e()) ;
	  
	  secondphotonflag = 1;
	}

      //deal with dalitz decays
      if(dumparentid == 1 && firstphotonflag && secondphotonflag && secondsecondphotonflag)
	{
	  std::cout << "Dalitz decay, skipping event" << std::endl;
	  return 0;
	}
      
      

      //Need these extra flags, otherwise the dalitz 
      //check will always have first and second photon
      //flags marked true, so this allows the dalitz check
      //to occur after marking the second truth photon
      //as a decay component
      if(firstphotonflag) firstfirstphotonflag = 1;
      if(secondphotonflag) secondsecondphotonflag = 1;
    }
  photonE -> Fill(photon1.Energy());
  photonE -> Fill(photon2.Energy());
  float asym = abs(photon1.Energy() - photon2.Energy())/(photon1.Energy() + photon2.Energy());
  float deltaR = photon1.DeltaR(photon2);
  
  //Now we go and find all our truth pi0s
  PHG4TruthInfoContainer::Range truthRange = truthinfo->GetPrimaryParticleRange();
  PHG4TruthInfoContainer::ConstIterator truthIter;
  std::vector<int> mPi0Barcode;
  Double_t pi0Mass = 0;
  PHG4Particle *truthPar = NULL;
  TLorentzVector truePi0;

  for(truthIter = truthRange.first; truthIter != truthRange.second; truthIter++)
    {
       truthPar = truthIter->second;
      
      if(truthPar -> get_pid() == 111 && truthPar -> get_parent_id() == 0)//for single particle gun, this list has one particle, the 
                                                                          //one of interest, but best be safe out of habit
	{
	  if(getEta(truthPar) >= mesonEtaMax)
	    {
	      std::cout << "Parent outside allowed spatial limit" << std::endl;
	      return 0; 
	    }
	  int etaBin = -1;
	  if(photon1.Energy() >= photon2.Energy())etaBin = getEtaBin(photon1.PseudoRapidity());
	  else etaBin = getEtaBin(photon2.PseudoRapidity());
	  if(etaBin >= 0)prim_pi0_E[etaBin] -> Fill(truthPar -> get_e());
	  prim_pi0_E[nEtaBins-1] -> Fill(truthPar -> get_e());

	  mPi0Barcode.push_back(truthPar -> get_barcode());
	  truePi0.SetPxPyPzE(truthPar -> get_px(), truthPar -> get_py(), truthPar -> get_pz(), truthPar -> get_e()); 
	  pi0Mass = truePi0.M();//sanity check later to make sure when we reconstruct the pi0 from truth photons we get the pi0 mass back
	    
	}
  
    }

  truthPi0EDeltaR -> Fill(truePi0.Energy(), deltaR);
  truthPi0EAsym -> Fill(truePi0.Energy(), asym);

  //iterate over items in the container.
  RawClusterContainer::ConstRange clusterEnd = clusterContainer -> getClusters();
  RawClusterContainer::ConstIterator clusterIter;

  //remove noise with a 300MeV energy cut and store
  //good clusters in a vector for later. 
  std::vector<RawCluster*> goodRecoCluster;
  float minE = 0.3;
  CLHEP::Hep3Vector vertex(vtx->get_x(), vtx->get_y(), vtx->get_z());

  for(clusterIter = clusterEnd.first; clusterIter != clusterEnd.second; clusterIter++)
    {
      RawCluster *recoCluster = clusterIter -> second;

      CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetECoreVec(*recoCluster, vertex);
      float clusE = E_vec_cluster.mag();
      clusterE -> Fill(clusE);
      if(clusE < minE) continue;
     
      goodRecoCluster.push_back(recoCluster);
    }
   
  for(int i = 0; i < (int)goodRecoCluster.size(); ++i)
    {
      //grab the first good cluster. 
      CLHEP::Hep3Vector vertex(vtx->get_x(), vtx->get_y(), vtx->get_z());
      CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetECoreVec(*goodRecoCluster[i], vertex);

      TLorentzVector pho1, pho2, pi0; 
      pho1.SetPtEtaPhiE(E_vec_cluster.perp(), E_vec_cluster.pseudoRapidity(), E_vec_cluster.getPhi(),  E_vec_cluster.mag());
       
       for(int j = 0; j <(int)goodRecoCluster.size(); ++j)
	{

	  if(j <= i) continue; //make sure we don't get redundant pairs
	  CLHEP::Hep3Vector E_vec_cluster2 = RawClusterUtility::GetECoreVec(*goodRecoCluster[j], vertex);
	  
	  pho2.SetPtEtaPhiE(E_vec_cluster2.perp(), E_vec_cluster2.pseudoRapidity(), E_vec_cluster2.getPhi(),  E_vec_cluster2.mag());

	  pi0 = pho1 + pho2; 
	  
	  pi0EScale -> Fill(pi0.Energy()/truePi0.Energy(), truePi0.Energy());//check the pi0 energy scale. 
	  
 
	  //isolating spatial kinematics to make sure we're really measuring
	  //sPHENIX's ability to reconstruct pi0's within its volume
	  if(abs(pho1.PseudoRapidity()) < photonEtaMax && abs(pho2.PseudoRapidity()) < photonEtaMax && abs(pi0.PseudoRapidity()) < mesonEtaMax)
	    {
	      int etaBin = -1;
	      //determine psuedorapidity bin from lead photon
	      if(pho1.Energy() >= pho2.Energy()) etaBin = getEtaBin(pho1.PseudoRapidity());
	      else etaBin = getEtaBin(pho2.PseudoRapidity());
	      if(etaBin >= 0)ePi0InvMassEcut[etaBin] -> Fill(pi0.Energy(), pi0.M(),std::min(pho1.Energy(), pho2.Energy()));
	      ePi0InvMassEcut[nEtaBins-1] -> Fill(pi0.Energy(), pi0.M(),std::min(pho1.Energy(), pho2.Energy()));
	    }
	}
    }

 
   

  
  //make sure we aren't messing up the reconstruction :)
  TLorentzVector pi0fromTruPhoton = photon1 + photon2;
  hMassRat -> Fill(pi0fromTruPhoton.M()/pi0Mass);
  //std::cout << "Mass diff " << massdiff << " from truth reco pi0: " << pi0fromTruPhoton.M() << " and truth pi0 mass: " << pi0Mass << std::endl;
  
  /*This doesn't work on things that aren't primary particles. 
  //and here's where we do the sneaky business of finding out where the photons went. 
  if(!cluster1 || !cluster2)
    {
      if(!cluster1) 
	{
		  
	  unmatchedLocale -> Fill(photon1.PseudoRapidity(), photon1.Phi(), truthPar -> get_e());
	  unmatchedE -> Fill(photon1.E());
	}
      if(!cluster2) 
	{
		 
	  unmatchedLocale -> Fill(photon2.PseudoRapidity(), photon2.Phi(), truthPar -> get_e());
	  unmatchedE -> Fill(photon2.E());
	}
      ePi0InvMassEcut[1] -> Fill(truthPar -> get_e(), 0., std::min(photon1.Energy(), photon2.Energy())); 
    }
  else
    {

      if(cluster1 -> get_id() == cluster2 -> get_id()) return 0;

      TLorentzVector cpi0, c1, c2;
	      
      CLHEP::Hep3Vector eVecCluster1 = RawClusterUtility::GetECoreVec(*cluster1, vertex);
      CLHEP::Hep3Vector eVecCluster2 = RawClusterUtility::GetECoreVec(*cluster2, vertex);
	      
      c1.SetPtEtaPhiE(eVecCluster1.perp(), eVecCluster1.pseudoRapidity(), eVecCluster1.getPhi(), eVecCluster1.mag());
      c2.SetPtEtaPhiE(eVecCluster2.perp(), eVecCluster2.pseudoRapidity(), eVecCluster2.getPhi(), eVecCluster2.mag());

      cpi0 = c1 + c2;
	      
      ePi0InvMassEcut[1] -> Fill(cpi0.Energy(), cpi0.M(),std::min(c1.Energy(), c1.Energy())); 
		  
      invMassEPhi -> Fill(cpi0.M(), truthPar -> get_e(), cpi0.Phi());	
      invMassPhoPi0 -> Fill(cpi0.M(), cpi0.Energy()/truthPar -> get_e(), c1.Energy()/photon1.Energy());
      invMassPhoPi0 -> Fill(cpi0.M(), cpi0.Energy()/truthPar -> get_e(), c1.Energy()/photon2.Energy());
      }*/	  
  /*Supplanting with method developed by Joe Osborn above
  PHG4TruthInfoContainer::Range truthRangeDecay1 = truthinfo->GetSecondaryParticleRange();
  PHG4TruthInfoContainer::ConstIterator truthIterDecay1;
  
 
  //Collect truth decay photons
  std::vector<PHG4Particle*> pi0Pho;
  std::vector<int> pi0PhoBarcode;
  for(truthIterDecay1 = truthRangeDecay1.first; truthIterDecay1 != truthRangeDecay1.second; truthIterDecay1++)
    {
      PHG4Particle *truthDecay1 = truthIterDecay1 -> second;
      PHG4Particle *mother = truthinfo -> GetParticle(truthDecay1 -> get_parent_id());
      int mBarcode = mother -> get_barcode();
    
      if(truthDecay1 -> get_parent_id() != 0 && truthDecay1 -> get_pid() == 22 && getEta(truthDecay1) <= etaMax && (std::find(mPi0Barcode.begin(), mPi0Barcode.end(),mBarcode) != mPi0Barcode.end())) //want decay photons w/ strict matching back to a pi0
	{
	  pi0PhoBarcode.push_back(mBarcode);//this is for the actual pairing process later where we want to match two photons together.
	  photonE -> Fill(truthDecay1 -> get_e());
	  pi0Pho.push_back(truthDecay1);
	  
	  
	}
    }
  
  //Re-pair decay photons to extract their kinematic information, match to clusters, and hopefully 
  //see what causes inefficiencies. Apparently GEANT considers literally every particle 
  //created after the initial generation a secondary particle, you need to do this pairing and then
  //find the pair with the invariant mass closes to the pi0's (~O(10-9) difference)
  float massdiff = 10;
  
  for(int i = 0; i < (int)pi0Pho.size(); i++)
    {
      for(int j = 0; j < (int)pi0Pho.size(); j++)
	{
	  if(j <= i) continue; 
	  
	  if(pi0PhoBarcode.at(i) != pi0PhoBarcode.at(j)) continue;
	  	  
	  TLorentzVector tpi0, pho1, pho2;
	  pho1.SetPxPyPzE(pi0Pho.at(i) -> get_px(), pi0Pho.at(i) -> get_py(), pi0Pho.at(i) -> get_pz(), pi0Pho.at(i) -> get_e());
	  pho2.SetPxPyPzE(pi0Pho.at(j) -> get_px(), pi0Pho.at(j) -> get_py(), pi0Pho.at(j) -> get_pz(), pi0Pho.at(j) -> get_e());

	  tpi0 = pho1 + pho2;
	  if(std::floor(100000*tpi0.M()) == std::floor(100000*pi0Mass))
	    {
	      massdiff = abs(tpi0.M() - pi0Mass);
	      RawCluster *cluster1 = clustereval -> best_cluster_from(pi0Pho.at(i));
	      RawCluster *cluster2 = clustereval -> best_cluster_from(pi0Pho.at(j));
	      //.13497
	      if(!cluster1 || !cluster2)
		{
		  ePi0InvMassEcut[1] -> Fill(tpi0.Energy(), 0., std::min(pi0Pho.at(i) -> get_e(), pi0Pho.at(j) -> get_e())); 
		}
	      else
		{
		  TLorentzVector cpi0, c1, c2;
	      
		  CLHEP::Hep3Vector eVecCluster1 = RawClusterUtility::GetECoreVec(*cluster1, vertex);
		  CLHEP::Hep3Vector eVecCluster2 = RawClusterUtility::GetECoreVec(*cluster2, vertex);
	      
		  c1.SetPtEtaPhiE(eVecCluster1.perp(), eVecCluster1.pseudoRapidity(), eVecCluster1.getPhi(), eVecCluster1.mag());
		  c2.SetPtEtaPhiE(eVecCluster2.perp(), eVecCluster2.pseudoRapidity(), eVecCluster2.getPhi(), eVecCluster2.mag());

		  cpi0 = c1 + c2;
	      
		  ePi0InvMassEcut[1] -> Fill(cpi0.Energy(),cpi0.M(),std::min(c1.Energy(), c1.Energy())); 
		  
		  invMassEPhi -> Fill(cpi0.M(), tpi0.Energy(), cpi0.Phi());	
		  invMassPhoPi0 -> Fill(cpi0.M(), cpi0.Energy()/tpi0.Energy(), c1.Energy()/pho1.Energy());
		  invMassPhoPi0 -> Fill(cpi0.M(), cpi0.Energy()/tpi0.Energy(), c1.Energy()/pho2.Energy());
		  
		  
		}
	    }
	}
	}*/


	      
   
  goodRecoCluster.clear();
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int pi0Efficiency::ResetEvent(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int pi0Efficiency::EndRun(const int runnumber)
{
  std::cout << "pi0Efficiency::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int pi0Efficiency::End(PHCompositeNode *topNode)
{
  std::cout << "pi0Efficiency::End(PHCompositeNode *topNode) This is the End..." << std::endl;

  out -> cd();
 
 
  for(int j = 0; j < nEtaBins; j++) ePi0InvMassEcut[j] -> Write();
  
  clusterE -> Write();
  for(int i = 0; i < nEtaBins; i++)prim_pi0_E[i] -> Write();
  photonE -> Write();
  pi0EScale -> Write();
  ///unmatchedLocale -> Write();
  hMassRat -> Write();
  truthPi0EDeltaR -> Write();
  truthPi0EAsym -> Write();
  //unmatchedE -> Write();
  

  
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int pi0Efficiency::Reset(PHCompositeNode *topNode)
{
  std::cout << "pi0Efficiency::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void pi0Efficiency::Print(const std::string &what) const
{
  std::cout << "pi0Efficiency::Print(const std::string &what) const Printing info for " << what << std::endl;
}
//____________________________________________________________________________.. 
float pi0Efficiency::getEta(PHG4Particle *particle)
{
  float px = particle -> get_px();
  float py = particle -> get_py();
  float pz = particle -> get_pz();
  float p = sqrt(pow(px,2) + pow(py,2) + pow(pz,2));

  return 0.5*log((p+pz)/(p-pz));
}
//____________________________________________________________________________.. 
int pi0Efficiency::getEtaBin(float eta)
{
  for(int i = 0; i < nEtaBins-1; i++)
    {
      if(abs(eta) < (i+1)/(float)(nEtaBins-1) * 1.1 && abs(eta) >= (i+1-1)/(float)(nEtaBins-1) * 1.1) return i;
    }
 return -1;
}
