#include <HIJetReco.C>

#include <jetkinematiccheck/JetKinematicCheck.h>

#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/SubsysReco.h>

#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllServer.h>

#include <phool/PHRandomSeed.h>
#include <phool/recoConsts.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libjetbase.so)
R__LOAD_LIBRARY(libjetbackground.so)
R__LOAD_LIBRARY(libJetKinematicCheck.so)
R__LOAD_LIBRARY(libg4centrality.so)
R__LOAD_LIBRARY(libg4dst.so)

void Fun4All_JetAna(const int nevnt = 0,
                    const std::string &filelistcalo = "dst_calo_cluster.list",
                    const std::string &filelistglobal = "dst_global.list",
                    const std::string &outname = "output.root")
{
  Fun4AllServer *se = Fun4AllServer::instance();
  int verbosity = 0;

  se->Verbosity(verbosity);
  recoConsts *rc = recoConsts::instance();

  HIJetReco();

  JetKinematicCheck *myJetKC = new JetKinematicCheck("AntiKt_Tower_r02_Sub1", "AntiKt_Tower_r03_Sub1", "AntiKt_Tower_r04_Sub1", outname);

  myJetKC->setPtRange(5, 100);
  myJetKC->setEtaRange(-1.1, 1.1);
  se->registerSubsystem(myJetKC);

  Fun4AllInputManager *in1 = new Fun4AllDstInputManager("DSTcalo");
  in1->AddListFile(filelistcalo, 1);
  se->registerInputManager(in1);

  Fun4AllInputManager *in2 = new Fun4AllDstInputManager("DSTglobal");
  in2->AddListFile(filelistglobal, 1);
  se->registerInputManager(in2);

  se->run(nevnt);
  se->End();

  gSystem->Exit(0);
  return 0;
}
