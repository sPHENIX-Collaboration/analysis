#include <iostream>

// --- header for this clase
#include <TreeMaker.h>

// --- basic sPhenix environment stuff
#include <fun4all/Fun4AllServer.h>
#include <phool/getClass.h>
#include <phool/PHCompositeNode.h>

// --- trigger
//#include <calotrigger/CaloTriggerInfo.h>

// --- truth information
//#include <g4main/PHG4TruthInfoContainer.h>
//#include <g4main/PHG4Particle.h>

// --- calorimeter towers
//#include <g4cemc/RawTowerGeom.h>
//#include <g4cemc/RawTower.h>
//#include <g4cemc/RawTowerContainer.h>
//#include <g4cemc/RawTowerGeomContainer_Cylinderv1.h>
//#include <g4cemc/RawTowerGeomContainer.h>
// --- calorimeter clusters
//#include <g4cemc/RawClusterContainer.h>
//#include <g4cemc/RawCluster.h>

// --- jet specific stuff
#include <TLorentzVector.h>
#include <g4jets/JetMap.h>
#include <g4jets/Jet.h>
#include <jetbackground/TowerBackground.h>


using std::cout;
using std::endl;

TreeMaker::TreeMaker(const std::string &name) : SubsysReco("TREEMAKER")
{
  foutname = name;
}


int TreeMaker::Init(PHCompositeNode *topNode)
{
  outfile = TFile::Open(foutname.c_str(),"RECREATE");
  InitializeTree();
  return 0;
}

int TreeMaker::process_event(PHCompositeNode *topNode)
{

  InitializeCounters();

  GetTruthJets(topNode);
  GetSeedJets(topNode);
  GetRecoJets(topNode);

  GetTowerBackgrounds(topNode);

  b_cluster_n = 0; // used only here
  b_particle_n = 0; // used only here



  // -----------------------------------------------------------------------------------------------------
  // --- all done!  fill the tree and move on to the next event
  // -----------------------------------------------------------------------------------------------------

  tree->Fill();

  return 0;
}



int TreeMaker::End(PHCompositeNode *topNode)
{
  outfile->Write();
  outfile->Close();
  cout << "All done!  Have a nice day!" << endl;
  return 0;
}

