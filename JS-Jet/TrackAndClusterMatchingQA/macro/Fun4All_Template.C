#include <trackandclustermatchingqa/TrackAndClusterMatchingQA.h>

#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllServer.h>

#include <jetbackground/RetowerCEMC.h>

#include <G4_Global.C>

#include <phool/recoConsts.h>

#include <caloreco/RawClusterBuilderTopo.h>
#include <particleflowreco/ParticleFlowReco.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libtrackandclustermatchingqa.so)
R__LOAD_LIBRARY(libjetbackground.so)

using namespace std;

void Fun4All_Template(vector<string> myInputLists = {"productionFiles-JET_30GEV-dst_tracks-00000.list","productionFiles-JET_30GEV-dst_vertex-00000.list","productionFiles-JET_30GEV-dst_truth-00000.list","productionFiles-JET_30GEV-dst_trkr_g4hit-00000.list", "productionFiles-JET_30GEV-dst_trkr_hit-00000.list","productionFiles-JET_30GEV-dst_trackseeds-00000.list","productionFiles-JET_30GEV-dst_trkr_cluster-00000.list","productionFiles-JET_30GEV-dst_calo_cluster-00000.list","productionFiles-JET_30GEV-dst_bbc_g4hit-00000.list"}, const int nEvents = 100)
{
  int verbosity = 0;

  gSystem->Load("libg4dst.so");
  recoConsts *rc = recoConsts::instance();
  Enable::CDB = true;
  // global tag
  rc->set_StringFlag("CDB_GLOBALTAG",CDB::global_tag);
  // 64 bit timestamp
  rc->set_uint64Flag("TIMESTAMP",CDB::timestamp);


  //The next set of lines figures out folder revisions, file numbers etc
  string outDir = "./";
  if (outDir.substr(outDir.size() - 1, 1) != "/") outDir += "/";
  outDir +=  "Output/";

  string fileNumber = myInputLists[0];
  size_t findLastDash = fileNumber.find_last_of("-");
  if (findLastDash != string::npos) fileNumber.erase(0, findLastDash + 1);
  string remove_this = ".list";
  size_t pos = fileNumber.find(remove_this);
  if (pos != string::npos) fileNumber.erase(pos, remove_this.length());
  string outputFileName = "outputData_" + fileNumber + ".root";

  string outputRecoDir = outDir + "/inReconstruction/";
  string makeDirectory = "mkdir -p " + outputRecoDir;
  system(makeDirectory.c_str());
  string outputRecoFile = outputRecoDir + outputFileName;

  //Create the server
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(verbosity);

  //Add all required input files
  for (unsigned int i = 0; i < myInputLists.size(); ++i)
  {
    Fun4AllInputManager *infile = new Fun4AllDstInputManager("DSTin_" + to_string(i));
    infile->AddListFile(myInputLists[i]);
    se->registerInputManager(infile);
  }

  Global_Reco();

  RetowerCEMC *rcemc = new RetowerCEMC();
  rcemc->set_towerinfo(true);
  rcemc->Verbosity(verbosity);
  se->registerSubsystem(rcemc);

  TrackAndClusterMatchingQA *module = new TrackAndClusterMatchingQA("module", outputRecoFile);
  module->set_EM_clustersName("CLUSTER_POS_COR_CEMC");
  module->set_EM_retoweredTowersName("TOWERINFO_CALIB_CEMC_RETOWER");
  module->Verbosity(verbosity);
  se->registerSubsystem(module);

  se->run(nEvents);
  se->End();

  ifstream file(outputRecoFile.c_str());
  if (file.good())
  {
    string moveOutput = "mv " + outputRecoFile + " " + outDir;
    system(moveOutput.c_str());
  }


  std::cout << "All done" << std::endl;
  delete se;
  gSystem->Exit(0);

  return;
}
