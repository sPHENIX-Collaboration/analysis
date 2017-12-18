#include <iostream>

// --- header for this clase
#include <TreeMaker.h>

// --- basic sPhenix environment stuff
#include <fun4all/Fun4AllServer.h>
#include <phool/getClass.h>
#include <phool/PHCompositeNode.h>


using std::cout;
using std::endl;



TreeMaker::TreeMaker(const std::string &name) : SubsysReco("TREEMAKER")
{
  cout << "TreeMaker::TreeMaker called" << endl;
  foutname = name;
}


TreeMaker::~TreeMaker()
{
  cout << "TreeMaker::~TreeMaker called" << endl;
}


int TreeMaker::Init(PHCompositeNode *topNode)
{
  cout << "TreeMaker::Init called" << endl;
  outfile = TFile::Open(foutname.c_str(),"RECREATE");
  InitializeTree();
  return 0;
}



int TreeMaker::process_event(PHCompositeNode *topNode)
{

  // --- Dennis stuff

  InitializeCounters();

  GetTruthJets(topNode);
  GetSeedJets(topNode);
  GetRecoJets(topNode);

  GetTowerBackgrounds(topNode);

  b_cluster_n = 0; // used only here
  b_particle_n = 0; // used only here

  // --- new stuff

  CopyAndMakeJets(topNode);
  CopyAndMakeClusters(topNode);



  // -----------------------------------------------------------------------------------------------------
  // --- all done!  fill the tree and move on to the next event
  // -----------------------------------------------------------------------------------------------------

  tree->Fill();

  return 0;
}



int TreeMaker::End(PHCompositeNode *topNode)
{
  cout << "TreeMaker::End called" << endl;
  outfile->Write();
  outfile->Close();
  cout << "All done!  Have a nice day!" << endl;
  return 0;
}

