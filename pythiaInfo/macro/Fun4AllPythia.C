#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllSyncManager.h>
#include <fun4all/Fun4AllUtils.h>


#include <phool/PHRandomSeed.h>
#include <phool/recoConsts.h>

#include <stdlib.h>

#include <g4main/PHG4Reco.h>
#include <g4main/PHG4TruthSubsystem.h>

#include <GlobalVariables.C>

#include <G4_Input.C>
#include <phpythia8/PHPy8JetTrigger.h>

#include <jethistogrammer/jetHistogrammer.h>

#include <g4jets/FastJetAlgo.h>
#include <g4jets/JetReco.h>
#include <g4jets/TruthJetInput.h>
#include <time.h>

R__LOAD_LIBRARY(libjetHistogrammer.so)

void Fun4AllPythia(int nEvents = 1,
		   const string &Jet_Trigger = "Jet10", // or "PhotonJet"
		   int doCrossSection = 1,
		   const string &outname = "out"
)
{

  clock_t tStart = clock();

  
  Fun4AllServer *se = Fun4AllServer::instance();
  se -> Verbosity(0);
  
  Input::PYTHIA8 = true;
  Input::VERBOSITY = 0;


  if (Jet_Trigger == "PhotonJet")
    {
      PYTHIA8::config_file = "phpythia8_JS_GJ_MDC2.cfg";
    }
  else if (Jet_Trigger == "Jet10")
    {
      PYTHIA8::config_file = "phpythia8_15GeV_JS_MDC2.cfg";
    }
  else if (Jet_Trigger == "Jet30")
    {
      PYTHIA8::config_file = "phpythia8_30GeV_JS_MDC2.cfg";
    }
  else if(Jet_Trigger == "MB")
    {
      PYTHIA8::config_file = "phpythia8_minBias_MDC2.cfg";
    }
  else
    {
      std::cout << "Invalid jet trigger " << Jet_Trigger << std::endl;
      gSystem->Exit(1);
    }
  
  InputInit();

  if(Jet_Trigger != "MB")
    {
      PHPy8JetTrigger *p8_js_signal_trigger = new PHPy8JetTrigger();
      p8_js_signal_trigger->SetEtaHighLow(1.5,-1.5); // Set eta acceptance for particles into the jet between +/- 1.5
      p8_js_signal_trigger->SetJetR(0.4);      //Set the radius for the trigger jet
      p8_js_signal_trigger->Verbosity(0);;
      if (Jet_Trigger == "Jet10")
	{
	  p8_js_signal_trigger->SetMinJetPt(10); // require a 10 GeV minimum pT jet in the event
	}
      else if (Jet_Trigger == "Jet30")
	{
	  p8_js_signal_trigger->SetMinJetPt(30); // require a 30 GeV minimum pT jet in the event
	}
      else if (Jet_Trigger == "PhotonJet")
	{
	  delete p8_js_signal_trigger;
	  p8_js_signal_trigger = nullptr;
	  cout << "no cut for PhotonJet" << endl;
	}
      else
	{
	  cout << "invalid jet trigger: " << Jet_Trigger << endl;
	  gSystem->Exit(1);
	}
  
      if (p8_js_signal_trigger)
	{
	  INPUTGENERATOR::Pythia8->register_trigger(p8_js_signal_trigger);
	  INPUTGENERATOR::Pythia8->set_trigger_AND();
	}
    }
  Input::ApplysPHENIXBeamParameter(INPUTGENERATOR::Pythia8);
  INPUTGENERATOR::Pythia8 -> Verbosity(doCrossSection*10);

  InputRegister();
  InputManagers();
  PHG4Reco *reco = new PHG4Reco();
  reco -> set_field(0);
  reco -> SetWorldMaterial("G4_Galactic");
  reco -> SetWorldSizeX(100);
  reco -> SetWorldSizeY(100);
  reco -> SetWorldSizeZ(100);
  reco -> save_DST_geometry(false);
  
  PHG4TruthSubsystem *truth = new PHG4TruthSubsystem();
  reco -> registerSubsystem(truth);
  se -> registerSubsystem(reco);

  JetReco *truthJetReco = new JetReco();
  TruthJetInput *jetInput = new TruthJetInput(Jet::PARTICLE);
  truthJetReco -> add_input(jetInput);
  truthJetReco->add_algo(new FastJetAlgo(Jet::ANTIKT, 0.4), "AntiKt_Truth_r04");
  truthJetReco->set_algo_node("ANTIKT");
  truthJetReco->set_input_node("TRUTH");
  truthJetReco->Verbosity(0);
  
  se -> registerSubsystem(truthJetReco);

  string out = outname + Jet_Trigger + ".root";
  jetHistogrammer *jetHistos = new jetHistogrammer("jetHistogrammer",out.c_str());
  se -> registerSubsystem(jetHistos);
 
 
  se -> run(nEvents);
  
  se -> End();
  std::cout << "All done" << std::endl;

  
  delete se;
  std::cout << "Total runtime: " << double(clock() - tStart) / (double)CLOCKS_PER_SEC << std::endl;;

  gSystem -> Exit(0);
  return 0;
}
  
  
