#pragma once

#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllUtils.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4allraw/Fun4AllPrdfInputManager.h>

#include <TString.h>

#include <smdhistgen/SmdHistGen.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libSmdHistGen.so)

void Fun4All_SmdHistGen(
		     std::string mode = "dst",
                     int nEvents = 1,
                     const char *infilelist = "42200.list",
		     std::string outfilename = "")
{
  // this convenience library knows all our i/o objects so you don't
  // have to figure out what is in each dst type
  gSystem->Load("libg4dst.so");

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(0);  // set it to 1 if you want event printouts

  Fun4AllInputManager *in_smd = new Fun4AllDstInputManager("DSTSMD");
  in_smd->AddListFile(infilelist);
  se->registerInputManager(in_smd);

  const char* outname;
  if (outfilename != "") outname = outfilename.c_str();
  else
  {
    std::list<std::string> inputfiles = in_smd->GetFileList();
    std::string inputFile = inputfiles.front();
    std::pair<int, int> runseg = Fun4AllUtils::GetRunSegment(inputFile);
    int runnumber = runseg.first;
    outname = Form("SmdHists-%d.root", runnumber);
  }
  std::cout << "Output file will be " << outname << std::endl;

  SmdHistGen *eval = new SmdHistGen("SmdHistGen", mode, outname);
  se -> registerSubsystem(eval);
  
  se->run(nEvents);
  se->End();
  
  delete se;
  cout << "Analysis Completed" << endl;
  
  gSystem->Exit(0);
}
