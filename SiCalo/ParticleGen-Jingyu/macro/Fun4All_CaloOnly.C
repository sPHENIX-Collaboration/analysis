#ifndef MACRO_FUN4ALL_CALOTREEGEN_C
#define MACRO_FUN4ALL_CALOTREEGEN_C

#include <ffamodules/CDBInterface.h>
#include <fun4all/Fun4AllUtils.h>

#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/SubsysReco.h>

#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllServer.h>

#include <phool/PHRandomSeed.h>
#include <phool/recoConsts.h>

#include <Calo_Calib.C>

#include <tutorial/CaloOnly.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libffarawobjects.so)
// R__LOAD_LIBRARY(libcaloHistGen.so)
R__LOAD_LIBRARY( libtutorial.so )

void Fun4All_CaloOnly(const int nEvents = 1, const std::string &fnameCalib = "DST_CALO_run2pp_ana450_2024p009-00053018-00001.root", const std::string &fnameRaw = "DST_CALOFITTING_run2pp_ana446_2024p007-00053018-00001.root",const std::string &outName = "calotest.root", const std::string &dbtag = "ProdA_2024", const std::string &inputfile = "/sphenix/user/jzhang1/testcode4all/PhotonConv/macro/runList/dst_calo_run2pp-00053018.list ")
{
    Fun4AllServer *se = Fun4AllServer::instance();
    se->Verbosity(1);

    recoConsts *rc = recoConsts::instance();
    
    pair<int, int> runseg = Fun4AllUtils::GetRunSegment(fnameCalib);
    int runnumber = runseg.first;

    // Enable::CDB = true;
    rc -> set_StringFlag("CDB_GLOBALTAG",dbtag);
    rc -> set_uint64Flag("TIMESTAMP",runnumber);
    CDBInterface::instance() -> Verbosity(1);

    gSystem->Load("libg4dst");
    
    Fun4AllInputManager *inRaw = new Fun4AllDstInputManager("DSTRaw");
    inRaw->AddFile(fnameRaw);
    se->registerInputManager(inRaw);

    Fun4AllInputManager *inCalib = new Fun4AllDstInputManager("DSTCalib");
    inCalib->AddFile(fnameCalib);
    se->registerInputManager(inCalib);

    // Fun4AllInputManager *infile = new Fun4AllDstInputManager("DSTin");
    // std::cout << "Including file " << inputfile << std::endl;
    // infile->AddListFile(inputfile);
    // se->registerInputManager(infile);

    Process_Calo_Calib();
        
    CaloOnly *calo = new CaloOnly("CaloOnly", outName);
    se->registerSubsystem(calo);

    se->run(nEvents);
    se->End();
    se->PrintTimer();
    std::cout << "All done!" << std::endl;

    gSystem->Exit(0);
}

#endif