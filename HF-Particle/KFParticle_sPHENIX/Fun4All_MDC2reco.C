#include "HFReco.C"

#include <g4main/Fun4AllDstPileupInputManager.h>

#include <G4_Global.C>
#include <G4_Magnet.C>
#include <G4_ActsGeom.C>
#include <G4_TrkrSimulation.C>

#include <Trkr_Clustering.C>
#include <Trkr_Reco.C>
#include <Trkr_RecoInit.C>

#include <FROG.h>
#include <decayfinder/DecayFinder.h>
#include <hftrackefficiency/HFTrackEfficiency.h>

#include <fun4all/Fun4AllDstInputManager.h>

#include <g4eval/SvtxEvaluator.h>
#include <g4eval/SvtxTruthRecoTableEval.h>

#include <ffamodules/CDBInterface.h>
#include <phool/recoConsts.h>
#include <globalvertex/GlobalVertexReco.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libdecayfinder.so)
R__LOAD_LIBRARY(libhftrackefficiency.so)
R__LOAD_LIBRARY(libg4vertex.so)
R__LOAD_LIBRARY(libglobalvertex.so)

using namespace std;
using namespace HeavyFlavorReco;

/****************************/
/*     MDC2 Reco for MDC2   */
/* Cameron Dean, LANL, 2021 */
/*      cdean@bnl.gov       */
/****************************/

void Fun4All_MDC2reco(vector<string> myInputLists = {"condorJob/fileLists/productionFiles-CHARM-dst_tracks-00000.list"}, const int nEvents = 0)
{
  bool runDemo = true; //Run the pure Bs2JpsiKs0 reco

  gSystem->Load("libg4dst.so");
  gSystem->Load("libFROG.so");
  FROG *fr = new FROG();

  string outDir = "./";
  if (!runDemo)
  {
    //The next set of lines figures out folder revisions, file numbers etc
    if (outDir.substr(outDir.size() - 1, 1) != "/") outDir += "/";
    outDir += reconstructionName + "/";

    string fileNumber = myInputLists[0];
    size_t findLastDash = fileNumber.find_last_of("-");
    if (findLastDash != string::npos) fileNumber.erase(0, findLastDash + 1);
    string remove_this = ".list";
    size_t pos = fileNumber.find(remove_this);
    if (pos != string::npos) fileNumber.erase(pos, remove_this.length());
    string outputFileName = "outputFile_" + reconstructionName + "_" + fileNumber + ".root";
    string outputHFEffFileName = "outputHFEff_" + reconstructionName + "_" + fileNumber + ".root";

    string outputRecoDir = outDir + "inReconstruction/";
    string makeDirectory = "mkdir -p " + outputRecoDir;
    system(makeDirectory.c_str());
    outputRecoFile = outputRecoDir + outputFileName;
    if (runTrackEff) outputHFEffFile = outputRecoDir + outputHFEffFileName;
  }

  recoConsts *rc = recoConsts::instance();
  // global tag
  rc->set_StringFlag("CDB_GLOBALTAG", "MDC2");
  // 64 bit timestamp
  rc->set_uint64Flag("TIMESTAMP", 6);

  //Create the server
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(1);
  if (runDemo)
  {
    Fun4AllInputManager *infile = new Fun4AllDstInputManager("DSTin");
    infile->AddListFile("demo.list");
    se->registerInputManager(infile);

    myDemoReco();
  }
  else
  {
    //Add all required input files
    for (unsigned int i = 0; i < myInputLists.size(); ++i)
    {
      Fun4AllInputManager *infile = new Fun4AllDstInputManager("DSTin_" + to_string(i));
      infile->AddListFile(myInputLists[i]);
      se->registerInputManager(infile);
    }
  
    // Runs decay finder to trigger on your decay. Useful for signal cleaning
    if (runTruthTrigger)
    {
      DecayFinder *myFinder = new DecayFinder("myFinder");
      myFinder->Verbosity(INT_MAX);
      myFinder->setDecayDescriptor(decayDescriptor);
      myFinder->saveDST(1);
      myFinder->allowPi0(0);
      myFinder->allowPhotons(1);
      myFinder->triggerOnDecay(1);
      myFinder->setPTmin(0.2); //Note: sPHENIX min pT is 0.2 GeV for tracking
      myFinder->setEtaRange(-1.1, 1.1); //Note: sPHENIX acceptance is |eta| <= 1.1
      se->registerSubsystem(myFinder);
    } 

    //Run the tracking if not already done
    if (runTracking)
    {
      Enable::MICROMEGAS=true;

      G4MAGNET::magfield_rescale = 1.;
      MagnetInit();
      MagnetFieldInit();

      Mvtx_Cells();
      Intt_Cells();
      TPC_Cells();
      Micromegas_Cells();

      TrackingInit();

      Mvtx_Clustering();
      Intt_Clustering();
      TPC_Clustering();
      Micromegas_Clustering();

      Tracking_Reco();
    
      Global_Reco();
    }

    if (buildTruthTable)
    {
      SvtxTruthRecoTableEval *tables = new SvtxTruthRecoTableEval();
      tables->Verbosity(VERBOSITY);
      se->registerSubsystem(tables);
    }

    if (runTrackEff)
    {
      HFTrackEfficiency *myTrackEff = new HFTrackEfficiency("myTrackEff");
      myTrackEff->Verbosity(1);
      myTrackEff->setDFNodeName("myFinder");
      myTrackEff->triggerOnDecay(1);
      myTrackEff->writeSelectedTrackMap(true);
      myTrackEff->writeOutputFile(true);
      myTrackEff->setOutputFileName(outputHFEffFile);
      se->registerSubsystem(myTrackEff);
    }

    //Now run the actual reconstruction
    myHeavyFlavorReco();
  }

  se->run(nEvents);
  se->End();

  if (!runDemo)
  {
    ifstream file(outputRecoFile.c_str());
    if (file.good())
    {
      string moveOutput = "mv " + outputRecoFile + " " + outDir;
      system(moveOutput.c_str());
    }

    if (runTrackEff)
    {
      ifstream evalfile(outputHFEffFile.c_str());
      if (evalfile.good())
      {
        string moveOutput = "mv " + outputHFEffFile + " " + outDir;
        system(moveOutput.c_str());
      }
    }
  }

  std::cout << "All done" << std::endl;
  delete se;
  gSystem->Exit(0);

  return;
}
