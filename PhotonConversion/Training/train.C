#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TChain.h>


#if not defined(__CINT__) || defined(__MAKECINT__)
// needs to be included when makecint runs (ACLIC)
#include "TMVA/Factory.h"
#include "TMVA/Tools.h"
#endif



TChain* handleFile(string name, string extension, string treename, int filecount){
  TChain *all = new TChain(treename.c_str());
  string temp;
  for (int i = 0; i < filecount; ++i)
  {

    ostringstream s;
    s<<i;
    temp = name+string(s.str())+extension;
    all->Add(temp.c_str());
  }
  return all;
}


void makeFactory(TTree* signalTree, TTree* backTree,std::string outfile,std::string factoryname, TTree* bgTree2=NULL)
{
  using namespace TMVA;
  TString jobname(factoryname.c_str());
  TFile *targetFile = new TFile(outfile.c_str(),"RECREATE");
  Factory *factory = new Factory(jobname,targetFile);
  factory->AddSignalTree(signalTree,1.0);
  factory->AddBackgroundTree(backTree,1.0);
  if(bgTree2){
    factory->AddBackgroundTree(bgTree2,1.0);
  }
  factory->AddSpectator("track_layer",'I');
  factory->AddSpectator("track_pT",'F');
  factory->AddSpectator("track_dca",'F');
  factory->AddSpectator("cluster_prob",'F');
  factory->AddSpectator("abs(track_deta)",'F');
  factory->AddSpectator("abs(cluster_deta)",'F');
  factory->AddSpectator("abs(cluster_dphi)",'F');
  factory->AddSpectator("abs(track_dlayer)",'I');
  factory->AddSpectator("approach_dist",'F');
  factory->AddVariable("vtx_radius",'F');
//  factory->AddVariable("vtx_chi2",'F'); 
  //factory->AddVariable("vtxTrackRZ_dist",'F');
  //factory->AddVariable("abs(vtxTrackRPhi_dist-vtxTrackRZ_dist)",'F');
  factory->AddVariable("photon_m",'F');
  factory->AddVariable("photon_pT",'F');

  string track_layer_cut = "track_layer>-1.";
  string track_pT_cut = "track_pT>2.0";
  string track_dca_cut = "50>track_dca>0";
  string em_prob_cut = "cluster_prob>-0.1";
  string track_deta_cut = ".0082>=track_deta";
  string track_dlayer_cut = "2>=abs(track_dlayer)";
  string approach_dist_cut = "69.34>approach_dist>0";
  string vtx_radius_cut = "vtx_radius>0";
  //do I need photon cuts? 
  string tCutInitializer = track_pT_cut+"&&"+em_prob_cut+"&&"+track_layer_cut;//+"&&"+track_deta_cut+"&&"+track_dlayer_cut+"&&"+approach_dist_cut+"&&"+vtx_radius_cut;
  TCut preTraingCuts(tCutInitializer.c_str());

  factory->PrepareTrainingAndTestTree(preTraingCuts,"nTrain_Signal=0:nTrain_Background=0:nTest_Signal=0:nTest_Background=0");
  //for track training
  //factory->BookMethod(Types::kCuts,"Cuts");
  //for pair training
  //factory->BookMethod(Types::kCuts,"Cuts","CutRangeMin[0]=0:CutRangeMax[0]=1:CutRangeMin[1]=-100:CutRangeMax[1]=100:CutRangeMin[2]=0:CutRangeMax[2]=100");
  //for vtx training
  factory->BookMethod(Types::kCuts,"Cuts");
  factory->TrainAllMethods();
  factory->TestAllMethods();
  factory->EvaluateAllMethods();
  targetFile->Write();
  targetFile->Close();
}


int train(){
  using namespace std;
  string treePath = "/sphenix/user/vassalli/RecoConversionTests/truthconversionembededonlineanalysis";
  string treeExtension = ".root";
  string outname = "cutTrainA.root";
  unsigned int nFiles=100;
  
  /*TChain *backVtxTree = new TChain("vtxBackTree");
  TChain *signalTree = new TChain("cutTreeSignal");
  backVtxTree->Add(treePath.c_str());
  signalTree->Add(treePath.c_str());*/
  TChain *signalTree = handleFile(treePath,treeExtension,"cutTreeSignal",nFiles);
  TChain *backtrackTree = handleFile(treePath,treeExtension,"trackBackTree",nFiles);
  TChain *backpairTree = handleFile(treePath,treeExtension,"pairBackTree",nFiles);
  TChain *backVtxTree = handleFile(treePath,treeExtension,"vtxBackTree",nFiles);
  //makeFactory(signalTree,backtrackTree,outname,"trackback");
  //makeFactory(signalTree,backpairTree,outname,"pairback");
  makeFactory(signalTree,backVtxTree,outname,"vtxback");
/*  outname="cutTrainE.root";
  makeFactory(signalTree,backETree,outname,"eback");*/
}
