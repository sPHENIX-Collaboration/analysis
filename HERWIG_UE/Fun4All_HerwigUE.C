// Fun4All_HerwigUE.C
//
// Run the HERWIG (Nashville) vs STAR UE cross-check over sPHENIX
// generator-level DSTs containing PHHepMCGenEventMap.
//
//   root -b -q 'Fun4All_HerwigUE.C("herwig_dst.list","herwig_ue_check.root",0)'
//
// filelist: text file with one DST path per line
// nevents : 0 = all

#include <fun4all/Fun4AllBase.h>
#include <fun4all/Fun4AllUtils.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/SubsysReco.h>
#include <ffamodules/CDBInterface.h>

#include <phool/recoConsts.h>


#include <herwigstaruecheck/HerwigSTARUECheck.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libphool.so)
R__LOAD_LIBRARY(libHerwigSTARUECheck.so)

void Fun4All_HerwigUE(const int nevents = -1,
                      const std::string &filelist = "herwig_dst.list",
                      const std::string &outfile = "herwig_ue_check.root")
{
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(1);

  auto rc = recoConsts::instance();
  rc->set_StringFlag("CDB_GLOBALTAG", "MDC2");
  rc->set_uint64Flag("TIMESTAMP", 0);
  CDBInterface::instance()->Verbosity(0);

  std::string simSample = filelist.substr(filelist.find("/")+1, filelist.find("_") - (filelist.find("/")+1));
  if(simSample.rfind("Pythia",0) == 0)
  {
    simSample = simSample.substr(6);
  }
  std::cout << "simSample: " << simSample << std::endl;

  HerwigSTARUECheck *ana = new HerwigSTARUECheck("HerwigSTARUECheck", outfile);
  // STAR PRD 101, 052004 defaults are already set; example variations:
  // ana->set_include_neutrinos(false);
  // ana->set_embedding_id(0);
  ana->SetSimSample(simSample);
  ana->useCSWeights(true);
  se->registerSubsystem(ana);

  Fun4AllDstInputManager *in = new Fun4AllDstInputManager("DSTin");
  in->AddListFile(filelist);
  se->registerInputManager(in);

  se->run(nevents);
  se->End();

  delete se;
  gSystem->Exit(0);
}
