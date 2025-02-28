#include <GlobalVariables.C>
#include <G4_Magnet.C>
#include <G4_ActsGeom.C>
// #include <Trkr_Clustering.C>
#include <Trkr_Reco_Cosmics.C>

#include <fun4all/Fun4AllRunNodeInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <ffamodules/CDBInterface.h>

#include <fun4allraw/Fun4AllStreamingInputManager.h>
// #include <fun4allraw/SingleGl1PoolInput.h>
#include <fun4allraw/SingleTpcPoolInput.h>
#include <ffarawmodules/TpcCheck.h>

#include <phool/recoConsts.h>
#include <ffamodules/FlagHandler.h>
#include <ffamodules/HeadReco.h>
#include <ffamodules/SyncReco.h>

#include <tpc/TpcCombinedRawDataUnpacker.h>

#include <trackingdiagnostics/TrkrNtuplizer.h>
// #include <trackingdiagnostics/TrackResiduals.h>

#include <tpcgaindebug/TPCGainDebug.h>

// #include <g4detectors/PHG4TpcCylinderGeom.h>
// #include <g4detectors/PHG4TpcCylinderGeomContainer.h>

#include <stdio.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libtpc.so)
R__LOAD_LIBRARY(libphool.so)
R__LOAD_LIBRARY(libTrackingDiagnostics.so)
R__LOAD_LIBRARY(libtpcgaindebug.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libffarawmodules.so)

void Fun4All_TPCGainDebug_mergedEBDC_useCombined(
    const std::string outdir = "/sphenix/tg/tg01/hf/yeonjugo/"
    , const int runnumber = 32420
    , bool saveAllhits = true
    , float AllHitADCThreshold = -9999
    , const std::string runType = "cosmics" //"calib" 
    , bool useListFiles = false
    , bool useCombinedFiles = true
    // , const string combinedFileName = "/sphenix/lustre01/sphnxpro/commissioning/aligned_streaming_tpc_3/cosmics-00030909-0005.root"
    , const string combinedFileName = "/sphenix/lustre01/sphnxpro/commissioning/aligned_streaming_tpc_3/cosmics-00032420-0002.root"
    // , const string combinedFileName = "/sphenix/lustre01/sphnxpro/commissioning/slurp/tpccosmics/run_00036200_00036300/DST_COSMICS_run2pp_new_2023p013-00036251-0004.root"
    , const string prefix = "02"
    , bool doSharkFin = false 
    , const int initevt = 172
    , const int endevt = 172
)
{
  // string tempCap = saveAllhits==1 ? "_allhit" : "";
  // if(!(initevt==0 && endevt==99999)) tempCap = 
  const int nEvents = (initevt==0 && endevt==99999) ? -1 : endevt-initevt+1; 
  string tempCap = (prefix == "") ? "" : "_" + prefix; 
  // string tempCap = (prefix == "") ? tempCap : tempCap + "_" + prefix; 
  tempCap += (nEvents==-1) ? "_allEvents" : Form("_evt%dto%d", initevt, endevt);
  tempCap = tempCap + ( saveAllhits==1 ? "_allhit" : "");

  if(doSharkFin)
    tempCap += "_sharkFin"; 

  string outfilename = runType + "_run" + std::to_string(runnumber) + tempCap +  ".root";

  // /sphenix/lustre01/sphnxpro/commissioning/tpc/cosmics/TPC_ebdc05_cosmics-00029746-0000.evt
  vector<string> tpc_infile;
  for(int i=0; i<24;i++){
    if(useListFiles){
      std::string filePath = Form("/direct/sphenix+tg+tg01/hf/yeonjugo/TPCGainDebug/inputlist/TPC_ebdc%02d_%s-000%d.list", i,     runType.data(), runnumber);
      tpc_infile.push_back(filePath);
    } else{
      // std::string filePath = Form("/sphenix/lustre01/sphnxpro/commissioning/tpc/%s/TPC_ebdc%02d_%s-%08d-000%s.evt", runType.data(), i, runType.data(), runnumber, prefix.data());
      std::string filePath = "/sphenix/lustre01/sphnxpro/commissioning/tpc/" + runType + "/TPC_ebdc" + std::string(i < 10 ? "0" : "") + std::to_string(i) + "_" + runType + "-000" + std::to_string(runnumber) + "-000" + prefix + ".evt";
      tpc_infile.push_back(filePath);

      // vector<string> gl1_infile;
      // gl1_infile.push_back(input_gl1file);
    }
  }

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(1);
  recoConsts *rc = recoConsts::instance();
  rc->set_IntFlag("RUNNUMBER",runnumber);

  HeadReco *head = new HeadReco();
  se->registerSubsystem(head);

  // FlagHandler *flag = new FlagHandler();
  // se->registerSubsystem(flag);

  Fun4AllDstInputManager* hitsin;
  Fun4AllStreamingInputManager *in;
  Fun4AllOutputManager *out;
  if(useCombinedFiles){
    hitsin = new Fun4AllDstInputManager("InputManager");
    hitsin->fileopen(combinedFileName.c_str());
    se->registerInputManager(hitsin);
  } else{
    in = new Fun4AllStreamingInputManager("Comb");
    int i = 0;
    for (auto iter : tpc_infile)
    {
      cout << iter << endl;
      SingleTpcPoolInput *tpc_sngl = new SingleTpcPoolInput("TPC_" + to_string(i));
      tpc_sngl->SetBcoRange(130);
      if(useListFiles){
        tpc_sngl->AddListFile(iter);
      } else{
        tpc_sngl->AddFile(iter);
      }
      in->registerStreamingInput(tpc_sngl, InputManagerType::TPC);
      i++;
    }
    se->registerInputManager(in);

    bool saveStreaming = false;
    if(saveStreaming){
      string streamingfilename = outdir + "/streaming_" + outfilename;
      out = new Fun4AllDstOutputManager("out", streamingfilename);
      se->registerOutputManager(out);
    }
  }
  
  Enable::CDB = true;
  rc->set_StringFlag("CDB_GLOBALTAG","ProdA_2023");
  rc->set_uint64Flag("TIMESTAMP",6);
  auto cdb = CDBInterface::instance();
  cdb->Verbosity(2);
  std::string geofile = CDBInterface::instance()->getUrl("Tracking_Geometry");
 
  Fun4AllRunNodeInputManager *ingeo = new Fun4AllRunNodeInputManager("GeoIn");
  ingeo->AddFile(geofile);
  se->registerInputManager(ingeo); 

  G4MAGNET::magfield = "0.01";
  G4MAGNET::magfield_rescale = 1;
  ACTSGEOM::ActsGeomInit();

  string tpcunpackerfilename = outdir + "/tpcUnpacker_" + outfilename;
  auto tpcunpacker = new TpcCombinedRawDataUnpacker("TpcCombinedRawDataUnpacker", tpcunpackerfilename);
  tpcunpacker->do_zero_suppression(false);
  // tpcunpacker->save_allhits(saveAllhits);
  tpcunpacker->Verbosity(0);
  tpcunpacker->event_range(initevt, endevt);
  // tpcunpacker->skipNevent(89);
  se->registerSubsystem(tpcunpacker);

  string tpcgaindebugfilename = outdir + "/tpcGainDebug_" + outfilename;
  auto ntp = new TPCGainDebug("TPCGainDebug",tpcgaindebugfilename, runnumber);
  ntp->Verbosity(1);
  ntp->event_range(initevt, endevt);
  ntp->set_eventOffset(stoi(prefix)*1000);
  if(doSharkFin){
    ntp->save_ntuple(0);
    ntp->debugNtuple_adcThreshold(-9999);
    ntp->save_sharkFin(1);
    // ntp->skipNevent(89);
    ntp->do_gain(0);
  }
  else {
    ntp->save_ntuple(saveAllhits);
    ntp->debugNtuple_adcThreshold(AllHitADCThreshold);
  }
  se->registerSubsystem(ntp);

  // se->skip(10);
    // se->run(nEvents);
  if(nEvents>0)
    se->run(endevt+1);
  else 
    se->run();
  se->End();
  se->PrintTimer();

  delete se;
  std::cout << "Finished" <<std::endl;
  gSystem->Exit(0);

}
