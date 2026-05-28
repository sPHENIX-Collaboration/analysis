#include <globalvertex/GlobalVertexReco.h>

#include <centrality/CentralityReco.h>

#include <calotrigger/MinimumBiasClassifier.h>

#include <ffamodules/CDBInterface.h>
#include <ffamodules/FlagHandler.h>

#include <fun4all/Fun4AllBase.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllUtils.h>

#include <phool/recoConsts.h>

#include <eventplaneinfo/EventPlaneReco.h>

#include <sepdvalidation/sEPD_MC_Validation.h>

// root includes --
#include <TSystem.h>

// c++ includes --
#include <fstream>
#include <iostream>
#include <string>

R__LOAD_LIBRARY(libg4detectors_io.so)
R__LOAD_LIBRARY(libphfield_io.so)
R__LOAD_LIBRARY(libphgeom_io.so)
R__LOAD_LIBRARY(libmvtx_io.so)
R__LOAD_LIBRARY(libintt_io.so)
R__LOAD_LIBRARY(libmicromegas_io.so)
R__LOAD_LIBRARY(libsEPDValidation.so)

void Fun4All_sEPD_MC(int nEvents = 0,
                    const std::string& flist = "files.list",
                    const std::string& output = "test.root",
                    const std::string& dbtag = "MDC2")
{
  std::cout << "########################" << std::endl;
  std::cout << "Run Parameters" << std::endl;
  std::cout << "input list: " << flist << std::endl;
  std::cout << "output: " << output << std::endl;
  std::cout << "nEvents: " << nEvents << std::endl;
  std::cout << "dbtag: " << dbtag << std::endl;
  std::cout << "########################" << std::endl;

  Fun4AllServer* se = Fun4AllServer::instance();

  std::ifstream infile_stream;
  infile_stream.open(flist, std::ios_base::in);
  std::string filepath;
  getline(infile_stream, filepath);
  std::pair<int, int> runseg = Fun4AllUtils::GetRunSegment(filepath);
  int runnumber = runseg.first;
  infile_stream.close();

  recoConsts* rc = recoConsts::instance();

  // conditions DB flags and timestamp
  rc->set_StringFlag("CDB_GLOBALTAG", dbtag);
  rc->set_uint64Flag("TIMESTAMP", runnumber);
  CDBInterface::instance()->Verbosity(Fun4AllBase::VERBOSITY_SOME);

  SubsysReco* flag = new FlagHandler();
  se->registerSubsystem(flag);

  // Official vertex storage
  SubsysReco* gvertex = new GlobalVertexReco();
  se->registerSubsystem(gvertex);

  // Minimum Bias Classifier
  MinimumBiasClassifier* mb = new MinimumBiasClassifier();
  mb->setIsSim(true);
  mb->setOverwriteScale("/sphenix/user/dlis/Projects/centrality/cdb/calibrations/scales/cdb_centrality_scale_1.root");
  mb->setOverwriteVtx("/sphenix/user/dlis/Projects/centrality/cdb/calibrations/vertexscales/cdb_centrality_vertex_scale_1.root");
  mb->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  se->registerSubsystem(mb);

  // Centrality
  CentralityReco* cent = new CentralityReco();
  cent->setOverwriteScale("/sphenix/user/dlis/Projects/centrality/cdb/calibrations/scales/cdb_centrality_scale_1.root");
  cent->setOverwriteVtx("/sphenix/user/dlis/Projects/centrality/cdb/calibrations/vertexscales/cdb_centrality_vertex_scale_1.root");
  cent->setOverwriteDivs("/sphenix/user/dlis/Projects/centrality/cdb/calibrations/divs/cdb_centrality_1.root");
  se->registerSubsystem(cent);

  EventPlaneReco* epreco = new EventPlaneReco();
  epreco->set_inputNode("TOWERINFO_CALIB_EPD");
  se->registerSubsystem(epreco);

  Fun4AllInputManager* In = new Fun4AllDstInputManager("in");
  In->AddListFile(flist);
  se->registerInputManager(In);

  // sEPD QA
  sEPD_MC_Validation* sepd_validation = new sEPD_MC_Validation();
  sepd_validation->set_print_interval(1000);
  se->registerSubsystem(sepd_validation);

  se->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  se->run(nEvents);
  se->End();

  se->dumpHistos(output);

  CDBInterface::instance()->Print();  // print used DB files
  se->PrintTimer();
  delete se;
  std::cout << "All done!" << std::endl;
  gSystem->Exit(0);
}
