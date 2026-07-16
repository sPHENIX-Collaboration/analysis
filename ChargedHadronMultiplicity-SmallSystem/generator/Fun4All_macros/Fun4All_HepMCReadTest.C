// Fun4All_HepMCReadTest.C
// Minimal validation: read a HepMC2 ASCII file through the standard sPHENIX
// input manager and dump the PHHepMCGenEventMap node it fills.
// Usage:  root -b -q 'Fun4All_HepMCReadTest.C("oo200_hepmc2.hepmc", 10)'

#include <TSystem.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <nodedump/Dumper.h>
#include <phhepmc/Fun4AllHepMCInputManager.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libphhepmc.so)
R__LOAD_LIBRARY(libphnodedump.so)

void Fun4All_HepMCReadTest(                                                                                                           //
    const std::string &infile = "/sphenix/user/hjheng/sPHENIXRepo/generator_OO200GeV/Pythia_Angantyr/pythia8317/examples/test.hepmc", //
    const int nEvents = 10                                                                                                            //
)
{
    Fun4AllServer *se = Fun4AllServer::instance();
    se->Verbosity(INT_MAX);

    // Same construction as the official G4_Input.C. The manager itself creates
    // the PHHepMCGenEventMap node and inserts one PHHepMCGenEvent per event.
    Fun4AllHepMCInputManager *in = new Fun4AllHepMCInputManager("HEPMCin");
    in->Verbosity(2); // set to 4 to print every parsed HepMC event in full
    in->AddFile(infile);
    se->registerInputManager(in);

    // Zero-custom-code content check: the standard NodeDump module, restricted
    // to the one node we care about. Writes one dump file per event to OutDir.
    Dumper *dumper = new Dumper("HEPMCDUMP");
    dumper->Select("PHHepMCGenEventMap");
    dumper->SetOutDir(".");
    se->registerSubsystem(dumper);

    se->run(nEvents);
    se->End();
    delete se;

    std::cout << "Fun4All_HepMCReadTest: done." << std::endl;

    gSystem->Exit(0);
}