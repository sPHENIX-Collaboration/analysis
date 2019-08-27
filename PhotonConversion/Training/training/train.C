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
  Factory *factory = new Factory(jobname,targetFile,"Correlations=True");
  DataLoader *loader = new DataLoader();
  loader->AddSignalTree(signalTree,1.0);
  loader->AddBackgroundTree(backTree,1.0);
  if(bgTree2){
    loader->AddBackgroundTree(bgTree2,1.0);
  }
  loader->AddVariable("track_deta",'F');
  /*loader->AddVariable("track_dlayer",'I');
  loader->AddVariable("track_layer",'I');*/
  loader->AddVariable("track_pT",'F');
  //loader->AddVariable("track_dca",'F');
  loader->AddVariable("approach_dist",'F'); //idk why this is off
  loader->AddVariable("vtx_radius",'F');
  loader->AddVariable("vtx_chi2",'F'); 
  //loader->AddVariable("vtxTrackRZ_dist",'F');
  //loader->AddVariable("abs(vtxTrackRPhi_dist-vtxTrackRZ_dist)",'F',"space diff");
  //loader->AddVariable("photon_m",'F');
  //loader->AddVariable("photon_pT",'F');
  loader->AddVariable("cluster_prob",'F');

  string track_pT_cut = "track_pT>0";
  string vtx_radius_cut = "vtx_radius>0";
  string em_prob_cut = "cluster_prob>=0";
  //string layer_cuts = "track_dlayer>=0&&track_layer>=0";
  string dist_cuts = "vtxTrackRZ_dist>=0&&vtxTrackRPhi_dist>=0&&approach_dist>=0";
  //do I need photon cuts? 
  string tCutInitializer = em_prob_cut+"&&"+ vtx_radius_cut+"&&"+track_pT_cut+"&&"+dist_cuts;
  TCut preTraingCuts(tCutInitializer.c_str());


  loader->PrepareTrainingAndTestTree(preTraingCuts,"nTrain_Signal=0:nTrain_Background=0:nTest_Signal=0:nTest_Background=0");
  factory->BookMethod( loader,TMVA::Types::kLikelihood, "LikelihoodD",
      "!H:!V:!TransformOutput:PDFInterpol=Spline2:NSmoothSig[0]=20:NSmoothBkg[0]=20:NSmooth=5:NAvEvtPerBin=50:VarTransform=Decorrelate" );
  factory->BookMethod(loader,Types::kCuts,"Cuts","");
  /*factory->BookMethod( Types::kKNN, "kNN", "" ); //>100k events
  factory->BookMethod( Types::kPDERS, "PDERS", "" );//>100k events*/
  /*factory->BookMethod( Types::kPDEFoam, "PDEFoam", "VolFrac=.0588i:SigBgSeparate=True" );//>10k events
  factory->BookMethod( Types::kFisher, "Fisher", "" );
  factory->BookMethod( Types::kLD, "LD" );*/
  /*would need to have the options tuned
   * factory->BookMethod( Types::kFDA, "FDA", "Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:\
      ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):\
      FitMethod=MINUIT:\
      ErrorLevel=1:PrintLevel=-1:FitStrategy=2:UseImprove:UseMinos:SetBatch" );*/
/* nerual network that would need to be worked on 
 * factory->BookMethod( Types::kMLP, "MLP_ANN", "<options>" );*/
  factory->TrainAllMethods();
  factory->TestAllMethods();
  factory->EvaluateAllMethods();
  targetFile->Write();
  targetFile->Close();
}


int train(){
  using namespace std;
  string treePath = "/sphenix/user/vassalli/gammasample/conversiononlineanalysis";
  string treeExtension = ".root";
  string outname = "cutTrainA.root";
  unsigned int nFiles=200;

  TChain *signalTree = handleFile(treePath,treeExtension,"cutTreeSignal",nFiles);
  TChain *backHTree = handleFile(treePath,treeExtension,"cutTreeBackh",nFiles);
  TChain *backETree = handleFile(treePath,treeExtension,"cutTreeBacke",nFiles);
  makeFactory(signalTree,backHTree,outname,"aback",backETree);
/*  outname="cutTrainE.root";
  makeFactory(signalTree,backETree,outname,"eback");*/
  return 0;
}
