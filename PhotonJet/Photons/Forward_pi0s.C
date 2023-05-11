#include "Forward_pi0s.h"

#include <fun4all/Fun4AllServer.h>
#include <g4main/PHG4Hit.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include <TLorentzVector.h>
#include <calobase/RawTower.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTowerGeomContainer.h>
#include <g4jets/Jet.h>
#include <g4jets/JetMap.h>
#include <iostream>

#include <calobase/RawCluster.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawClusterUtility.h>
#include <g4detectors/PHG4ScintillatorSlatContainer.h>
#include <g4eval/JetEvalStack.h>
#include <g4eval/SvtxEvalStack.h>

#include <g4vertex/GlobalVertex.h>
#include <g4vertex/GlobalVertexMap.h>
#include <sstream>

#include <HepMC/GenEvent.h>
#include <HepMC/GenVertex.h>
#include <phhepmc/PHHepMCGenEvent.h>
using namespace std;

#include <cassert>
#include <iostream>

Forward_pi0s::Forward_pi0s(const std::string &name)
  : SubsysReco("FORWARD_PI0S")
{
  outfilename = name;

  //add other initializers here
  //default use isocone algorithm
  use_isocone = 1;

  //default central arm
  _etalow = -1;
  _etahi = 1;

  //default to only central arm
  _useforwardarm = 0;

  mincluspt = 0.3;

  nevents = 0;
  //default use 0.3 jet cone
  jet_cone_size = 0.3;

}

int Forward_pi0s::Init(PHCompositeNode *topnode)
{
  file = new TFile(outfilename.c_str(), "RECREATE");

  histo = new TH1F("histo", "histo", 100, -3, 3);

  tree = new TTree("tree", "a tree");
  tree->Branch("nevents", &nevents, "nevents/I");

  Set_Tree_Branches();
  initialize_things();

  return 0;
}

int Forward_pi0s::process_event(PHCompositeNode *topnode)
{
  nevents++;
  cout << "at event number " << nevents << endl;
  
  //reset the truth variables
  tphote2 = -999;
  tphotpx2 = -999;
  tphotpy2 = -999;
  tphotpz2 = -999;
  tphotpid2 = -999;
  tphotparentid2 = -999;
  tphotpt2 = -999;
  tphotphi2 = -999;
  tphoteta2 = -999;

  tphote1 = -999;
  tphotpx1 = -999;
  tphotpy1 = -999;
  tphotpz1 = -999;
  tphotpid1 = -999;
  tphotparentid1 = -999;
  tphotpt1 = -999;
  tphotphi1 = -999;
  tphoteta1 = -999;

  tpi0e = -999;
  tpi0px = -999;
  tpi0py = -999;
  tpi0pz = -999;
  tpi0pid = -999;
  tpi0pt = -999;
  tpi0phi = -999;
  tpi0eta = -999;

  //reset cluster variables
  fclusenergy = -999;
  fclus_eta = -999;
  fclus_phi = -999;
  fclus_theta = -999;
  fclus_pt = -999;
  fclus_px = -999;
  fclus_py = -999;
  fclus_pz = -999;


  clus_energy = -999;
  clus_eta = -999;
  clus_theta = -999;
  clus_pt = -999;
  clus_phi = -999;
  clus_px = -999;
  clus_py = -999;
  clus_pz = -999;

 




  //get the nodes from the NodeTree

  PHG4TruthInfoContainer *truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topnode, "G4TruthInfo");
  RawClusterContainer *clusters = findNode::getClass<RawClusterContainer>(topnode, "CLUSTER_CEMC");
  RawClusterContainer *fclusters = 0;
  if (_useforwardarm)
  {
    fclusters = findNode::getClass<RawClusterContainer>(topnode, "CLUSTER_FEMC");
  }
  PHG4TruthInfoContainer::Range range = truthinfo->GetPrimaryParticleRange();

  GlobalVertexMap *vertexmap = findNode::getClass<GlobalVertexMap>(topnode, "GlobalVertexMap");
  if (!vertexmap)
  {
    cout << "Forward_pi0s::process_event - Fatal Error - GlobalVertexMap node is missing. Please turn on the do_global flag in the main macro in order to reconstruct the global vertex." << endl;
    assert(vertexmap);  // force quit

    return 0;
  }

  if (vertexmap->empty())
  {
    cout << "Forward_pi0s::process_event - Fatal Error - GlobalVertexMap node is empty. Please turn on the do_global flag in the main macro in order to reconstruct the global vertex." << endl;
    return 0;
  }

  GlobalVertex *vtx = vertexmap->begin()->second;
  if (vtx == nullptr) return 0;


  if (!fclusters && _useforwardarm)
  {
    cout << "not forward cluster info" << endl;
    return 0;
  }
  if (!truthinfo)
  {
    cout << "no truth track info" << endl;
    return 0;
  }

  if (!clusters)
  {
    cout << "no cluster info" << endl;
    return 0;
  }


  for (PHG4TruthInfoContainer::ConstIterator iter = range.first; iter != range.second; ++iter)
  {
    PHG4Particle *truth = iter->second;

    truthpx = truth->get_px();
    truthpy = truth->get_py();
    truthpz = truth->get_pz();
    truthp = sqrt(truthpx * truthpx + truthpy * truthpy + truthpz * truthpz);
    truthenergy = truth->get_e();

    TLorentzVector vec;
    vec.SetPxPyPzE(truthpx, truthpy, truthpz, truthenergy);

    truthpt = sqrt(truthpx * truthpx + truthpy * truthpy);

    truthphi = vec.Phi();
    trutheta = vec.Eta();
    truthpid = truth->get_pid();

    //get the pi0 information
    if (truthpid == 111)
    {
      tpi0e = truth->get_e();
      tpi0px = truth->get_px();
      tpi0py = truth->get_py();
      tpi0pz = truth->get_pz();
      tpi0pid = truth->get_pid();
      tpi0pt = sqrt(tpi0px * tpi0px + tpi0py * tpi0py);
      TLorentzVector pi0;
      pi0.SetPxPyPzE(tpi0px, tpi0py, tpi0pz, tpi0e);
      tpi0phi = pi0.Phi();
      tpi0eta = pi0.Eta();
    }

    truth_g4particles->Fill();
  }

  if (Verbosity() > 1)
    cout << "Truth pi0 energy is :" << tpi0e << endl;

  int firstphotonflag = 0;
  int firstfirstphotonflag = 0;
  int secondphotonflag = 0;
  int secondsecondphotonflag = 0;
  PHG4TruthInfoContainer::Range ranger = truthinfo->GetSecondaryParticleRange();
  //This sets the global variables in the if statements to the pion first decay photon truth values and second decay photon truth values
  for (PHG4TruthInfoContainer::ConstIterator iter = ranger.first; iter != ranger.second; ++iter)
  {
    PHG4Particle *truth = iter->second;

    int dumtruthpid = truth->get_pid();
    int dumparentid = truth->get_parent_id();


    //if the parent is the pi0 and its a photon and we haven't marked one yet
    if (dumparentid == 1 && dumtruthpid == 22 && !firstphotonflag)
    {
      tphote1 = truth->get_e();
      tphotpx1 = truth->get_px();
      tphotpy1 = truth->get_py();
      tphotpz1 = truth->get_pz();
      tphotpid1 = truth->get_pid();
      tphotparentid1 = truth->get_parent_id();
      tphotpt1 = sqrt(tphotpx1 * tphotpx1 + tphotpy1 * tphotpy1);

      TLorentzVector vec;
      vec.SetPxPyPzE(tphotpx1, tphotpy1, tphotpz1, tphote1);

      tphotphi1 = vec.Phi();
      tphoteta1 = vec.Eta();
      firstphotonflag = 1;
    }

    //if the parent is the pi0 and its a photon and we have
    //marked the first photon but not the second photon
    if (dumparentid == 1 &&
        dumtruthpid == 22 &&
        firstphotonflag && firstfirstphotonflag)
    {
      tphote2 = truth->get_e();
      tphotpx2 = truth->get_px();
      tphotpy2 = truth->get_py();
      tphotpz2 = truth->get_pz();
      tphotpid2 = truth->get_pid();
      tphotparentid2 = truth->get_parent_id();
      tphotpt2 = sqrt(tphotpx2 * tphotpx2 + tphotpy2 * tphotpy2);

      TLorentzVector vec;
      vec.SetPxPyPzE(tphotpx2, tphotpy2, tphotpz2, tphote2);

      tphotphi2 = vec.Phi();
      tphoteta2 = vec.Eta();
      secondphotonflag = 1;
    }

    //this is a decay to photon + ee pair i.e. dalitz decay, lets just skip it
    if (dumparentid == 1 &&
        firstphotonflag && secondphotonflag && secondsecondphotonflag)
    {
      cout << "I AM GOING TO SKIP THIS EVENT BECAUSE IT APPEARS TO BE A DALITZ DECAY" << endl;
      cout << "THERE WERE 3 PARTICLES MARKED WITH PARENT ID == 1" << endl;

      return 0;
    }

    //we need these extra flags because otherwise the dalitz check
    //will always have first and secondphoton flags marked as true
    //so this allows the dalitz check to occur after marking the
    //second truth photon as a decay component
    if (firstphotonflag)
      firstfirstphotonflag = 1;
    if (secondphotonflag)
      secondsecondphotonflag = 1;
  }
  if (Verbosity() > 1)
  {
    cout << "truth decay photon 1 energy | phi | eta " << tphote1
         << "  |   " << tphotphi1 << "   |   " << tphoteta1 << endl;
    cout << "truth decay photon 2 energy | phi | eta " << tphote2
         << "  |   " << tphotphi2 << "   |   " << tphoteta2 << endl;
  }






  /***********************************************

  GET THE FORWARD EMCAL CLUSTERS

  ************************************************/

  if (_useforwardarm)
  {
    RawClusterContainer::ConstRange fclus = fclusters->getClusters();
    RawClusterContainer::ConstIterator fclusiter;

    for (fclusiter = fclus.first; fclusiter != fclus.second; ++fclusiter)
    {
      RawCluster *cluster = fclusiter->second;

      CLHEP::Hep3Vector vertex(vtx->get_x(), vtx->get_y(), vtx->get_z());
      CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetEVec(*cluster, vertex);
      fclusenergy = E_vec_cluster.mag();
      fclus_eta = E_vec_cluster.pseudoRapidity();
      fclus_theta = E_vec_cluster.getTheta();
      fclus_pt = E_vec_cluster.perp();
      fclus_phi = E_vec_cluster.getPhi();
     
   
      if (fclusenergy < mincluspt)
        continue;
    

      fclus_px = fclus_pt * TMath::Cos(fclus_phi);
      fclus_py = fclus_pt * TMath::Sin(fclus_phi);
      fclus_pz = fclusenergy * TMath::Cos(fclus_theta);

      //reset second cluster variables
      fclusenergy2 = -999;
      fclus_eta2 = -999;
      fclus_phi2 = -999;
      fclus_theta2 = -999;
      fclus_pt2 = -999;
      fclus_px2 = -999;
      fclus_py2 = -999;
      fclus_pz2 = -999;


      if(Verbosity() > 1)
	{
	  cout<<"Found one good cluster"<<endl;
	  cout<<"energy | phi | eta: "<<fclusenergy<<" | "<<fclus_phi<<" | "<<fclus_eta<<endl;
	}

      //found a first good cluster, lets look for a second
      RawClusterContainer::ConstRange fclus2 = fclusters->getClusters();
      RawClusterContainer::ConstIterator fclusiter2;
      for (fclusiter2 = fclus2.first; fclusiter2 != fclus2.second; ++fclusiter2)
      {
        RawCluster *cluster2 = fclusiter2->second;

        CLHEP::Hep3Vector E_vec_cluster2 = RawClusterUtility::GetEVec(*cluster2, vertex);
        fclusenergy2 = E_vec_cluster2.mag();
	//just got the same photon, skip
        if (fclusenergy == fclusenergy2)
	  {
	    //reset it
	    fclusenergy2 = -999;
	    continue;
	  }

        fclus_eta2 = E_vec_cluster2.pseudoRapidity();
        fclus_theta2 = E_vec_cluster2.getTheta();
        fclus_pt2 = E_vec_cluster2.perp();
        fclus_phi2 = E_vec_cluster2.getPhi();

	  

        if (fclusenergy2 < mincluspt)
          continue;

	//avoid double counting, leading photon should always be highest energy
        if (fclusenergy < fclusenergy2)
          continue;

        if (Verbosity() > 1)
        {
          cout << "identified reco photon 1 energy | phi | eta:  "
               << fclusenergy << "  |  " << fclus_phi << "  |  " << fclus_eta << endl;
          cout << "identified reco photon 2 energy | phi | eta:  "
               << fclusenergy2 << "  |  " << fclus_phi2 << "  |  " << fclus_eta2 << endl;
        }

        fclus_px2 = fclus_pt2 * TMath::Cos(fclus_phi2);
        fclus_py2 = fclus_pt2 * TMath::Sin(fclus_phi2);
        fclus_pz2 = fclusenergy2 * TMath::Cos(fclus_theta2);

        TLorentzVector phot1, phot2;
        phot1.SetPtEtaPhiE(fclus_pt, fclus_eta, fclus_phi, fclusenergy);
        phot2.SetPtEtaPhiE(fclus_pt2, fclus_eta2, fclus_phi2, fclusenergy2);

        TLorentzVector pi0;
        pi0 = phot1 + phot2;

	//get the pi0 invmass
        invmass = pi0.M();

        if (Verbosity() > 1)
          cout << "pi0 reco invmass is " << invmass << endl;
      }


      if(Verbosity() > 1)
	{
	  cout<<"Final clusters found and writing to tree"<<endl;
	  cout<<"clus 1 - energy | phi | eta: "<<fclusenergy<<" | "<<fclus_phi
	      <<" | "<<fclus_eta<<endl;
	  cout<<"clus 2 - energy | phi | eta: "<<fclusenergy2<<" | "<<fclus_phi2
	      <<" | "<<fclus_eta2<<endl;

	}
      fcluster_tree->Fill();
    }
  }






  /***********************************************

  GET THE CENTRAL ARM EMCAL CLUSTERS

  ************************************************/

  RawClusterContainer::ConstRange begin_end = clusters->getClusters();
  RawClusterContainer::ConstIterator clusiter;

  for (clusiter = begin_end.first; clusiter != begin_end.second; ++clusiter)
  {
    RawCluster *cluster = clusiter->second;

    CLHEP::Hep3Vector vertex(vtx->get_x(), vtx->get_y(), vtx->get_z());
    CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetECoreVec(*cluster, vertex);
    clus_energy = E_vec_cluster.mag();
    clus_eta = E_vec_cluster.pseudoRapidity();
    clus_theta = E_vec_cluster.getTheta();
    clus_pt = E_vec_cluster.perp();
    clus_phi = E_vec_cluster.getPhi();

    if (clus_pt < mincluspt)
      continue;

    TLorentzVector *clus = new TLorentzVector();
    clus->SetPtEtaPhiE(clus_pt, clus_eta, clus_phi, clus_energy);

    float dumarray[4];
    clus->GetXYZT(dumarray);
    clus_x = dumarray[0];
    clus_y = dumarray[1];
    clus_z = dumarray[2];
    clus_t = dumarray[3];

    clus_px = clus_pt * TMath::Cos(clus_phi);
    clus_py = clus_pt * TMath::Sin(clus_phi);
    clus_pz = sqrt(clus_energy * clus_energy - clus_px * clus_px - clus_py * clus_py);




    //reset second cluster variables
    clus_energy2 = -999;
    clus_eta2 = -999;
    clus_theta2 = -999;
    clus_pt2 = -999;
    clus_phi2 = -999;
    clus_px2 = -999;
    clus_py2 = -999;
    clus_pz2 = -999;





    //found a first good cluster, lets look for a second
    RawClusterContainer::ConstRange begin_end2 = clusters->getClusters();
    RawClusterContainer::ConstIterator clusiter2;

    for (clusiter2 = begin_end2.first; clusiter2 != begin_end2.second; ++clusiter2)
      {
	RawCluster *cluster2 = clusiter2->second;
	CLHEP::Hep3Vector vertex(vtx->get_x(), vtx->get_y(), vtx->get_z());
	CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetECoreVec(*cluster2, vertex);
	clus_energy2 = E_vec_cluster.mag();
	if(clus_energy == clus_energy2)
	  {
	    //reset it
	    clus_energy2 = -999;
	    continue;
	  }
	clus_eta2 = E_vec_cluster.pseudoRapidity();
	clus_theta2 = E_vec_cluster.getTheta();
	clus_pt2 = E_vec_cluster.perp();
	clus_phi2 = E_vec_cluster.getPhi();
	
	if(clus_energy == clus_energy2 and clus_eta == clus_eta2 and clus_phi == clus_phi2)
	  continue;
	
	if(clus_energy2 < mincluspt)
	  continue;
	
	//avoid double counting
	if(clus_energy < clus_energy2)
	  continue;
	
	if (Verbosity() > 1)
        {
	  cout << "CENTRAL ARM ID: "<<endl;
          cout << "identified reco photon 1 energy | phi | eta:  "
               << clus_energy << "  |  " << clus_phi << "  |  " << clus_eta << endl;
          cout << "identified reco photon 2 energy | phi | eta:  "
               << clus_energy2 << "  |  " << clus_phi2 << "  |  " << clus_eta2 << endl;
        }
 
	clus_px2 = clus_pt2 * TMath::Cos(clus_phi2);
	clus_py2 = clus_pt2 * TMath::Sin(clus_phi2);
	clus_pz2 = sqrt(clus_energy2 * clus_energy2 - clus_px2 * clus_px2 - clus_py2 * clus_py2);

	
	TLorentzVector phot1, phot2;
        phot1.SetPtEtaPhiE(clus_pt, clus_eta, clus_phi, clus_energy);
        phot2.SetPtEtaPhiE(clus_pt2, clus_eta2, clus_phi2, clus_energy2);

        TLorentzVector pi0;
        pi0 = phot1 + phot2;

	//get the pi0 invmass
        cent_invmass = pi0.M();

        if (Verbosity() > 1)
          cout << "Central pi0 reco invmass is " << cent_invmass << endl;
      

	cluster_tree->Fill();
      }
    
  }

  tree->Fill();

 

  return 0;
}

int Forward_pi0s::End(PHCompositeNode *topnode)
{
  std::cout << " DONE PROCESSING " << endl;

  file->Write();
  file->Close();
  return 0;
}

void Forward_pi0s::Set_Tree_Branches()
{
  inhcal_tree = new TTree("hcalclustree", "a tree with inner hcal cluster information");
  inhcal_tree->Branch("hcal_energy", &hcal_energy, "hcal_energy/F");
  inhcal_tree->Branch("hcal_eta", &hcal_eta, "hcal_eta/F");
  inhcal_tree->Branch("hcal_phi", &hcal_phi, "hcal_phi/F");
  inhcal_tree->Branch("hcal_pt", &hcal_pt, "hcal_pt/F");
  inhcal_tree->Branch("hcal_theta", &hcal_theta, "hcal_theta/F");
  inhcal_tree->Branch("hcal_x", &hcal_x, "hcal_x/F");
  inhcal_tree->Branch("hcal_y", &hcal_y, "hcal_y/F");
  inhcal_tree->Branch("hcal_z", &hcal_z, "hcal_z/F");
  inhcal_tree->Branch("hcal_t", &hcal_t, "hcal_t/F");
  inhcal_tree->Branch("hcal_px", &hcal_px, "hcal_px/F");
  inhcal_tree->Branch("hcal_py", &hcal_py, "hcal_py/F");
  inhcal_tree->Branch("hcal_pz", &hcal_pz, "hcal_pz/F");
  inhcal_tree->Branch("nevents", &nevents, "nevents/I");
  inhcal_tree->Branch("hclustruthpx", &hclustruthpx, "hclustruthpx/F");
  inhcal_tree->Branch("hclustruthpy", &hclustruthpy, "hclustruthpy/F");
  inhcal_tree->Branch("hclustruthpz", &hclustruthpz, "hclustruthpz/F");
  inhcal_tree->Branch("hclustruthenergy", &hclustruthenergy, "hclustruthenergy/F");
  inhcal_tree->Branch("hclustruthpt", &hclustruthpt, "hclustruthpt/F");
  inhcal_tree->Branch("hclustruthphi", &hclustruthphi, "hclustruthphi/F");
  inhcal_tree->Branch("hclustrutheta", &hclustrutheta, "hclustrutheta/F");

  cluster_tree = new TTree("clustertree", "A tree with EMCal cluster information");
  cluster_tree->Branch("clus_energy", &clus_energy, "clus_energy/F");
  cluster_tree->Branch("clus_eta", &clus_eta, "clus_eta/F");
  cluster_tree->Branch("clus_phi", &clus_phi, "clus_phi/F");
  cluster_tree->Branch("clus_pt", &clus_pt, "clus_pt/F");
  cluster_tree->Branch("clus_theta", &clus_theta, "clus_theta/F");
  cluster_tree->Branch("clus_x", &clus_x, "clus_x/F");
  cluster_tree->Branch("clus_y", &clus_y, "clus_y/F");
  cluster_tree->Branch("clus_z", &clus_z, "clus_z/F");
  cluster_tree->Branch("clus_t", &clus_t, "clus_t/F");
  cluster_tree->Branch("clus_px", &clus_px, "clus_px/F");
  cluster_tree->Branch("clus_py", &clus_py, "clus_py/F");
  cluster_tree->Branch("clus_pz", &clus_pz, "clus_pz/F");
  cluster_tree->Branch("nevents", &nevents, "nevents/I");

  cluster_tree->Branch("clus_px2", &clus_px2, "clus_px2/F");
  cluster_tree->Branch("clus_py2", &clus_py2, "clus_py2/F");
  cluster_tree->Branch("clus_pz2", &clus_pz2, "clus_pz2/F");
  cluster_tree->Branch("clus_energy2", &clus_energy2, "clus_energy2/F");
  cluster_tree->Branch("clus_eta2", &clus_eta2, "clus_eta2/F");
  cluster_tree->Branch("clus_phi2", &clus_phi2, "clus_phi2/F");
  cluster_tree->Branch("clus_pt2", &clus_pt2, "clus_pt2/F");
  cluster_tree->Branch("clus_theta2", &clus_theta2, "clus_theta2/F");
  cluster_tree->Branch("Cent_invmass", &cent_invmass,"cent_invmass/F");

  cluster_tree->Branch("tpi0e", &tpi0e, "tpi0e/F");
  cluster_tree->Branch("tpi0px", &tpi0px, "tpi0px/F");
  cluster_tree->Branch("tpi0py", &tpi0py, "tpi0py/F");
  cluster_tree->Branch("tpi0pz", &tpi0pz, "tpi0pz/F");
  cluster_tree->Branch("tpi0pid", &tpi0pid, "tpi0pid/F");
  cluster_tree->Branch("tpi0pt", &tpi0pt, "tpi0pt/F");
  cluster_tree->Branch("tpi0phi", &tpi0phi, "tpi0phi/F");
  cluster_tree->Branch("tpi0eta", &tpi0eta, "tpi0eta/F");
  cluster_tree->Branch("tphote1", &tphote1, "tphote1/F");
  cluster_tree->Branch("tphotpx1", &tphotpx1, "tphotpx1/F");
  cluster_tree->Branch("tphotpy1", &tphotpy1, "tphotpy1/F");
  cluster_tree->Branch("tphotpz1", &tphotpz1, "tphotpz1/F");
  cluster_tree->Branch("tphotpt1", &tphotpt1, "tphotpt1/F");
  cluster_tree->Branch("tphotpid1", &tphotpid1, "tphotpid1/I");
  cluster_tree->Branch("tphotparentid1", &tphotparentid1, "tphotparentid1/I");
  cluster_tree->Branch("tphotphi1", &tphotphi1, "tphotphi1/F");
  cluster_tree->Branch("tphoteta1", &tphoteta1, "tphoteta1/F");

  cluster_tree->Branch("tphote2", &tphote2, "tphote2/F");
  cluster_tree->Branch("tphotpx2", &tphotpx2, "tphotpx2/F");
  cluster_tree->Branch("tphotpy2", &tphotpy2, "tphotpy2/F");
  cluster_tree->Branch("tphotpz2", &tphotpz2, "tphotpz2/F");
  cluster_tree->Branch("tphotpt2", &tphotpt2, "tphotpt2/F");
  cluster_tree->Branch("tphotpid2", &tphotpid2, "tphotpid2/I");
  cluster_tree->Branch("tphotparentid2", &tphotparentid2, "tphotparentid2/I");
  cluster_tree->Branch("tphotphi2", &tphotphi2, "tphotphi2/F");
  cluster_tree->Branch("tphoteta2", &tphoteta2, "tphoteta2/F");



  fcluster_tree = new TTree("fclustertree", "A tree with FEMCal cluster information");
  fcluster_tree->Branch("invmass", &invmass, "invmass/F");
  fcluster_tree->Branch("fclusenergy", &fclusenergy, "fclusenergy/F");
  fcluster_tree->Branch("fclus_eta", &fclus_eta, "fclus_eta/F");
  fcluster_tree->Branch("fclus_phi", &fclus_phi, "fclus_phi/F");
  fcluster_tree->Branch("fclus_pt", &fclus_pt, "fclus_pt/F");
  fcluster_tree->Branch("fclus_theta", &fclus_theta, "fclus_theta/F");
  fcluster_tree->Branch("fclus_px", &fclus_px, "fclus_px/F");
  fcluster_tree->Branch("fclus_py", &fclus_py, "fclus_py/F");
  fcluster_tree->Branch("fclus_pz", &fclus_pz, "fclus_pz/F");
  fcluster_tree->Branch("nevents", &nevents, "nevents/I");
  fcluster_tree->Branch("fclusenergy2", &fclusenergy2, "fclusenergy2/F");
  fcluster_tree->Branch("fclus_eta2", &fclus_eta2, "fclus_eta2/F");
  fcluster_tree->Branch("fclus_phi2", &fclus_phi2, "fclus_phi2/F");
  fcluster_tree->Branch("fclus_pt2", &fclus_pt2, "fclus_pt2/F");
  fcluster_tree->Branch("fclus_theta2", &fclus_theta2, "fclus_theta2/F");
  fcluster_tree->Branch("fclus_px2", &fclus_px2, "fclus_px2/F");
  fcluster_tree->Branch("fclus_py2", &fclus_py2, "fclus_py2/F");
  fcluster_tree->Branch("fclus_pz2", &fclus_pz2, "fclus_pz2/F");

  fcluster_tree->Branch("tpi0e", &tpi0e, "tpi0e/F");
  fcluster_tree->Branch("tpi0px", &tpi0px, "tpi0px/F");
  fcluster_tree->Branch("tpi0py", &tpi0py, "tpi0py/F");
  fcluster_tree->Branch("tpi0pz", &tpi0pz, "tpi0pz/F");
  fcluster_tree->Branch("tpi0pid", &tpi0pid, "tpi0pid/F");
  fcluster_tree->Branch("tpi0pt", &tpi0pt, "tpi0pt/F");
  fcluster_tree->Branch("tpi0phi", &tpi0phi, "tpi0phi/F");
  fcluster_tree->Branch("tpi0eta", &tpi0eta, "tpi0eta/F");
  fcluster_tree->Branch("tphote1", &tphote1, "tphote1/F");
  fcluster_tree->Branch("tphotpx1", &tphotpx1, "tphotpx1/F");
  fcluster_tree->Branch("tphotpy1", &tphotpy1, "tphotpy1/F");
  fcluster_tree->Branch("tphotpz1", &tphotpz1, "tphotpz1/F");
  fcluster_tree->Branch("tphotpt1", &tphotpt1, "tphotpt1/F");
  fcluster_tree->Branch("tphotpid1", &tphotpid1, "tphotpid1/I");
  fcluster_tree->Branch("tphotparentid1", &tphotparentid1, "tphotparentid1/I");
  fcluster_tree->Branch("tphotphi1", &tphotphi1, "tphotphi1/F");
  fcluster_tree->Branch("tphoteta1", &tphoteta1, "tphoteta1/F");

  fcluster_tree->Branch("tphote2", &tphote2, "tphote2/F");
  fcluster_tree->Branch("tphotpx2", &tphotpx2, "tphotpx2/F");
  fcluster_tree->Branch("tphotpy2", &tphotpy2, "tphotpy2/F");
  fcluster_tree->Branch("tphotpz2", &tphotpz2, "tphotpz2/F");
  fcluster_tree->Branch("tphotpt2", &tphotpt2, "tphotpt2/F");
  fcluster_tree->Branch("tphotpid2", &tphotpid2, "tphotpid2/I");
  fcluster_tree->Branch("tphotparentid2", &tphotparentid2, "tphotparentid2/I");
  fcluster_tree->Branch("tphotphi2", &tphotphi2, "tphotphi2/F");
  fcluster_tree->Branch("tphoteta2", &tphoteta2, "tphoteta2/F");

  truth_g4particles = new TTree("truthtree_g4", "a tree with all truth g4 particles");
  truth_g4particles->Branch("truthpx", &truthpx, "truthpx/F");
  truth_g4particles->Branch("truthpy", &truthpy, "truthpy/F");
  truth_g4particles->Branch("truthpz", &truthpz, "truthpz/F");
  truth_g4particles->Branch("truthp", &truthp, "truthp/F");
  truth_g4particles->Branch("truthenergy", &truthenergy, "truthenergy/F");
  truth_g4particles->Branch("truthphi", &truthphi, "truthphi/F");
  truth_g4particles->Branch("trutheta", &trutheta, "trutheta/F");
  truth_g4particles->Branch("truthpt", &truthpt, "truthpt/F");
  truth_g4particles->Branch("truthpid", &truthpid, "truthpid/I");
  truth_g4particles->Branch("nevents", &nevents, "nevents/I");

  truth_g4particles->Branch("tphote1", &tphote1, "tphote1/F");
  truth_g4particles->Branch("tphotpx1", &tphotpx1, "tphotpx1/F");
  truth_g4particles->Branch("tphotpy1", &tphotpy1, "tphotpy1/F");
  truth_g4particles->Branch("tphotpz1", &tphotpz1, "tphotpz1/F");
  truth_g4particles->Branch("tphotpt1", &tphotpt1, "tphotpt1/F");
  truth_g4particles->Branch("tphotpid1", &tphotpid1, "tphotpid1/I");
  truth_g4particles->Branch("tphotparentid1", &tphotparentid1, "tphotparentid1/I");
  truth_g4particles->Branch("tphotphi1", &tphotphi1, "tphotphi1/F");
  truth_g4particles->Branch("tphoteta1", &tphoteta1, "tphoteta1/F");

  truth_g4particles->Branch("tphote2", &tphote2, "tphote2/F");
  truth_g4particles->Branch("tphotpx2", &tphotpx2, "tphotpx2/F");
  truth_g4particles->Branch("tphotpy2", &tphotpy2, "tphotpy2/F");
  truth_g4particles->Branch("tphotpz2", &tphotpz2, "tphotpz2/F");
  truth_g4particles->Branch("tphotpt2", &tphotpt2, "tphotpt2/F");
  truth_g4particles->Branch("tphotpid2", &tphotpid2, "tphotpid2/I");
  truth_g4particles->Branch("tphotparentid2", &tphotparentid2, "tphotparentid2/I");
  truth_g4particles->Branch("tphotphi2", &tphotphi2, "tphotphi2/F");
  truth_g4particles->Branch("tphoteta2", &tphoteta2, "tphoteta2/F");
}

void Forward_pi0s::initialize_things()
{
  hcal_energy = -999;
  hcal_eta = -999;
  hcal_phi = -999;
  hcal_pt = -999;
  hcal_px = -999;
  hcal_py = -999;
  hcal_pz = -999;
  hcal_theta = -999;
  hcal_x = -999;
  hcal_y = -999;
  hcal_z = -999;
  hcal_t = -999;

  clus_energy = -999;
  clus_eta = -999;
  clus_phi = -999;
  clus_pt = -999;
  clus_px = -999;
  clus_py = -999;
  clus_pz = -999;
  clus_theta = -999;
  clus_x = -999;
  clus_y = -999;
  clus_z = -999;
  clus_t = -999;

  fclusenergy2 = -999;
  fclus_eta2 = -999;
  fclus_phi2 = -999;
  fclus_theta2 = -999;
  fclus_pt2 = -999;
  fclus_px2 = -999;
  fclus_py2 = -999;
  fclus_pz2 = -999;
  invmass = -999;

  tphote1 = -999;
  tphotpx1 = -999;
  tphotpy1 = -999;
  tphotpz1 = -999;
  tphotpid1 = -999;
  tphotparentid1 = -999;
  tphotpt1 = -999;
  tphotphi1 = -999;
  tphoteta1 = -999;
  tphote2 = -999;
  tphotpx2 = -999;
  tphotpy2 = -999;
  tphotpz2 = -999;
  tphotpid2 = -999;
  tphotparentid2 = -999;
  tphotpt2 = -999;
  tphotphi2 = -999;
  tphoteta2 = -999;
  fclusenergy = -999;
  fclus_eta = -999;
  fclus_phi = -999;
  fclus_theta = -999;
  fclus_pt = -999;
  fclustruthpid = -999;
  fclustruthpx = -999;
  fclustruthpy = -999;
  fclustruthpz = -999;
  fclustruthenergy = -999;
  fclustruthpt = -999;
  fclustruthphi = -999;
  fclustrutheta = -999;
  fclus_px = -999;
  fclus_py = -999;
  fclus_pz = -999;

  tpi0e = -999;
  tpi0px = -999;
  tpi0py = -999;
  tpi0pz = -999;
  tpi0pid = -999;
  tpi0pt = -999;
  tpi0phi = -999;
  tpi0eta = -999;
}
