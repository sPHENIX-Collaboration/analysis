#ifndef MACRO_G4HIJETRECO_C
#define MACRO_G4HIJETRECO_C

#include <GlobalVariables.C>
#include <G4_Global.C>

#include <mbd/MbdReco.h>

#include <fstream>
#include <limits>
#include <iostream>

#include <globalvertex/GlobalVertexReco.h>

#include <fun4all/Fun4AllServer.h>
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllRunNodeInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllUtils.h>
 
#include <phool/PHRandomSeed.h>
#include <phool/recoConsts.h>

#include <ana_ppg09_mod/Ana_PPG09_Mod.h>

#include <jetbase/FastJetAlgo.h>
#include <jetbase/JetReco.h>
#include <jetbase/TowerJetInput.h>

#include <jetbackground/CopyAndSubtractJets.h>
#include <jetbackground/DetermineTowerBackground.h>
#include <jetbackground/FastJetAlgoSub.h>
#include <jetbackground/RetowerCEMC.h>
#include <jetbackground/SubtractTowers.h>
#include <jetbackground/SubtractTowersCS.h>

//Data
#include <ffamodules/CDBInterface.h>
#include <ffamodules/FlagHandler.h>
#include <ffamodules/HeadReco.h>
#include <ffamodules/SyncReco.h>

#include <calotreegen/caloTreeGen.h> 
#include <Calo_Calib.C>
#include <calovalid/CaloValid.h>
#include <calotrigger/TriggerRunInfoReco.h>
#include <calotrigger/TriggerRunInfov1.h>
#include <globalqa/GlobalQA.h>
#include <jetbackgroundcut/jetBackgroundCut.h>

R__LOAD_LIBRARY(libglobalQA.so)
R__LOAD_LIBRARY(libmbd.so)
R__LOAD_LIBRARY(libffarawobjects.so)
R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libcalotrigger.so)
R__LOAD_LIBRARY(libglobalvertex.so)
R__LOAD_LIBRARY(libAna_PPG09_Mod.so)
R__LOAD_LIBRARY(libjetbackgroundcut.so)
R__LOAD_LIBRARY(libjetbase.so)
R__LOAD_LIBRARY(libjetbackground.so)
R__LOAD_LIBRARY(libg4jets.so)

namespace HIJETS
{
  bool do_flow = false; // should be set to true once the EPD event plane correction is implemented
  bool do_CS = false;
  bool is_pp = true;  // turn off functionality only relevant for nucleon collisions
  std::string tower_prefix = "TOWERINFO_CALIB";
  bool do_vertex_type = true;
  GlobalVertex::VTXTYPE vertex_type = GlobalVertex::MBD;
}  // namespace HIJETS

void Fun4All_Ana_PPG09_Mod_Skim(
                  const char *filelistcalo = "Skim_Calo_Test.list",
                  const char *filelistjet = "Skim_Jet_Test.list",
                  const string &outname = "Test",
                  int n_skip = 0,
                  int n_event = 2000
	         )
   {
   gSystem->Load("libg4dst");
   gSystem->Load("libAna_PPG09_Mod");

  int verbosity = 0;

  //---------------
  // Fun4All server
  //---------------

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(verbosity);

  string inputFile; 
  string inputFile2;
  string run_num;
 
  if(outname == "Test"){ 
    run_num = "49659";
    inputFile = "DST_JETCALO_run2pp_ana450_2024p009-000"+run_num+"-00000.root";
    inputFile2 = "DST_JET_run2pp_ana450_2024p009-000"+run_num+"-00000.root";
  }
  else{
    int d = stoi(outname);
    int Counter = 0;
    string filePath = "listRun.txt";
    ifstream file(filePath);
    string line;
    cout << d << endl;
    while (getline(file, line)){
       if(Counter == d){
          cout << line << endl;
          run_num = line;
          cout << run_num << endl;
       }
       Counter += 1;
    }
    file.close();
    inputFile = "DST_JETCALO_run2pp_ana450_2024p009-000"+run_num+"-00000.root";
    inputFile2 = "DST_JET_run2pp_ana450_2024p009-000"+run_num+"-00000.root";
  }

  //Data
  recoConsts *rc = recoConsts::instance();
  
  pair<int, int> runseg = Fun4AllUtils::GetRunSegment(inputFile);
  int runnumber = runseg.first;
  int segment = runseg.second;

  rc->set_StringFlag("CDB_GLOBALTAG","ProdA_2024");
  rc->set_uint64Flag("TIMESTAMP",runnumber);
  CDBInterface::instance()->Verbosity(0); 

  Fun4AllInputManager *in = new Fun4AllDstInputManager("DSTCalo");
  in->AddListFile(filelistcalo,1);
  se->registerInputManager(in);

  Fun4AllInputManager *in2 = new Fun4AllDstInputManager("DSTjets");
  in2->AddListFile(filelistjet,1);
  se->registerInputManager(in2);

  Process_Calo_Calib();

  //NJA Added: Trigger Info Reco (Dan. Lis)
  TriggerRunInfoReco *triggerruninforeco = new TriggerRunInfoReco();
  se->registerSubsystem(triggerruninforeco);
  
  JetReco *towerjetreco = new JetReco();
  towerjetreco->add_input(new TowerJetInput(Jet::CEMC_TOWERINFO_RETOWER,HIJETS::tower_prefix));
  towerjetreco->add_input(new TowerJetInput(Jet::HCALIN_TOWERINFO,HIJETS::tower_prefix));
  towerjetreco->add_input(new TowerJetInput(Jet::HCALOUT_TOWERINFO,HIJETS::tower_prefix));
  towerjetreco->add_algo(new FastJetAlgoSub(Jet::ANTIKT, 0.2, verbosity), "AntiKt_Tower_r02");
  towerjetreco->add_algo(new FastJetAlgoSub(Jet::ANTIKT, 0.3, verbosity), "AntiKt_Tower_r03");
  towerjetreco->add_algo(new FastJetAlgoSub(Jet::ANTIKT, 0.4, verbosity), "AntiKt_Tower_r04");
  towerjetreco->add_algo(new FastJetAlgoSub(Jet::ANTIKT, 0.5, verbosity), "AntiKt_Tower_r05");
  towerjetreco->set_algo_node("ANTIKT");
  towerjetreco->set_input_node("TOWER");
  towerjetreco->Verbosity(verbosity);
  se->registerSubsystem(towerjetreco);

  /*
  jetBackgroundCut *jocl = new jetBackgroundCut("AntiKt_Tower_r04_Sub1","JOCL", 3, false);
  se->registerSubsystem(jocl);
  */

  //Check jet Input first line, see if subtracted or not Sub: AntiKt_Tower_r04_Sub1 Unsub: AntiKt_Tower_r04
  Ana_PPG09_Mod *APM9 = new Ana_PPG09_Mod("AntiKt_Tower_r04", "output/R04_Jets_PPG09_v5_Skim_Run" + run_num +"_" + outname + ".root");  
  se->registerSubsystem(APM9);
  
  //se->run(-1);
  se->run(n_event);

  se->End();
  
  gSystem->Exit(0);
  //return 0;
}
#endif
