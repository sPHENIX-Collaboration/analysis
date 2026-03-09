/*
 * This macro shows a minimum working example of running the tracking
 * hit unpackers with some basic seeding algorithms to try to put together
 * tracks. There are some analysis modules run at the end which package
 * hits, clusters, and clusters on tracks into trees for analysis.
 */

#include <fun4all/Fun4AllUtils.h>

#include <ffamodules/CDBInterface.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllRunNodeInputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <GlobalVariables.C>

#include <phool/recoConsts.h>

#include <laserqa/laserQA.h>

#include <stdio.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libtpc.so)
R__LOAD_LIBRARY(libLaserQA.so)
void Fun4All_laserQA(
    const int nEvents = 10,
    const std::string fileList = "/sphenix/tg/tg01/jets/bkimelman/BenProduction/Aug19_2024/",
    const std::string outdir = "/sphenix/tg/tg01/jets/bkimelman/BenProduction/Aug19_2024/laserClusterQA/",
    int runnumber = 53534,
    const std::string prod = "new_2025p000_v000",
    const std::string type = "run3auau")
{


  TString outfile = Form("%s/laserQA_%s_%s-%08d.root",outdir.c_str(),type.c_str(), prod.c_str(), runnumber);
  std::string theOutfile = outfile.Data();
  auto se = Fun4AllServer::instance();
  se->Verbosity(2);
  auto rc = recoConsts::instance();
  rc->set_IntFlag("RUNNUMBER", runnumber);

  Enable::CDB = true;
  rc->set_StringFlag("CDB_GLOBALTAG", "ProdA_2025");
  rc->set_uint64Flag("TIMESTAMP", runnumber);
  std::string geofile = CDBInterface::instance()->getUrl("Tracking_Geometry");


  auto infile = new Fun4AllDstInputManager("InputManager");
  //infile->fileopen(inputLaserDST);
  infile->AddListFile(fileList);
  se->registerInputManager(infile);

  auto lQA = new laserQA("laserQA");
  lQA->setOutfile(theOutfile);
  se->registerSubsystem(lQA);

  se->run(nEvents);
  se->End();
  se->PrintTimer();

  delete se;
  std::cout << "Finished" << std::endl;
  gSystem->Exit(0);
}
