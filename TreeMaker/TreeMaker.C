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


int TreeMaker::InitRun(PHCompositeNode *topNode)
{
  cout << "TreeMaker::InitRun called" << endl;
  int cn_status = CreateNode(topNode);
  cout << "TreeMaker::InitRun : CreateNode returned " << cn_status << endl;
  return 0;
}



int TreeMaker::process_event(PHCompositeNode *topNode)
{

  // --- initialize the counters
  InitializeCounters();

  // --- copy the old jets/clusters and make a new container for modified jets/clusters
  CopyAndMakeJets(topNode);
  CopyAndMakeClusters(topNode);

  // --- get the jets and set the tree variables
  GetTruthJets(topNode);
  GetRecoJets(topNode);

  // --- get the clusters and do some analysis
  GetClusters(topNode);

  // --- do some stuff with fast jet
  UseFastJet(topNode);

  // --- fill the tree
  tree->Fill();

  // --- all done!
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

