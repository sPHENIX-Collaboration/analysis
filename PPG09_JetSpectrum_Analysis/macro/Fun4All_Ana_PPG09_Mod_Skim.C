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

R__LOAD_LIBRARY(libglobalQA.so)
R__LOAD_LIBRARY(libmbd.so)
R__LOAD_LIBRARY(libffarawobjects.so)
R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libcalotrigger.so)
R__LOAD_LIBRARY(libglobalvertex.so)
R__LOAD_LIBRARY(libAna_PPG09_Mod.so)

void Fun4All_Ana_PPG09_Mod_Skim(
                  const char *filelistcalo = "Skim_Calo_Test.list",
                  const char *filelistjet = "Skim_Jet_Test.list",
                  const string &outname = "Test",
                  int n_skip = 0,
                  int n_event = 300
	         )
   {
   gSystem->Load("libg4dst");
   gSystem->Load("libAna_PPG09_Mod");

  int verbosity = 0;

  //---------------
  // Fun4All server
  //--------------1-

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
  
  //Check jet Input first line, see if subtracted or not Sub: AntiKt_Tower_r04_Sub1 Unsub: AntiKt_Tower_r04
  Ana_PPG09_Mod *APM9 = new Ana_PPG09_Mod("AntiKt_Tower_r04_Sub1", "output/R04_Jets_PPG09_v1_Skim_Run" + run_num +"_" + outname + ".root");  
  se->registerSubsystem(APM9);
  
  //se->run(-1);
  se->run(n_event);

  se->End();
  
  gSystem->Exit(0);
  return 0;
}
#endif
