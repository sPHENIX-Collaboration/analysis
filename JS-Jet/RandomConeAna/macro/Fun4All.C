#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)

#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>

#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllOutputManager.h>

#include <phool/PHRandomSeed.h>
#include <phool/recoConsts.h>

#include <g4centrality/PHG4CentralityReco.h>

#include <jetbackground/RetowerCEMC.h>
#include <jetbackground/FastJetAlgoSub.h>

#include <jetbase/JetReco.h>
#include <jetbase/TowerJetInput.h>
#include <jetbase/FastJetAlgo.h>
#include <g4jets/TruthJetInput.h>

#include <HIJetReco.C>

#include <randomconetree/RandomConeTree.h>

#include <towerrho/DetermineTowerRho.h>
#include <towerrho/TowerRho.h>

#include <randomcones/RandomConeReco.h>



// #include <randomconeana/RandomConeAna.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libg4jets.so)
R__LOAD_LIBRARY(libjetbackground.so)
R__LOAD_LIBRARY(libjetbase.so)
R__LOAD_LIBRARY(libg4centrality.so)
R__LOAD_LIBRARY(libg4dst.so)
R__LOAD_LIBRARY(libRandomConeTree.so)
R__LOAD_LIBRARY(libtowerrho.so)
R__LOAD_LIBRARY(librandomcones.so)

// R__LOAD_LIBRARY(libRandomConeAna.so)

#endif


void Fun4All(
  const char *filelistcalo = "dst_calo_cluster.list",
	const char *filelistglobal = "dst_global.list",
  const char * outputfile = "output.root",
  const char * outputfile_debug = "output_debug.root"
)
{

  // analysis parameters
  //-----------------------------------
  // global
  bool do_centrality = true;
  double eta_max = 1.1;
  double cone_jet_eta_max = 0.65;

  double tower_pT_min = 0.0;
  double tower_threshold = 0.05;
  bool do_tower_cut = true;

  Enable::HIJETS_TRUTH=false;

  double zvrtx_abs_cut = 10.0;

  //-----------------------------------  
  // rho estimation
  bool do_rho_Area = true;
  bool do_rho_Mult = true;

  Jet::ALGO background_algo = Jet::ALGO::KT;
  double background_R = 0.4;
  int omit_nhardest = 2;
  double ghost_area = 0.01;
  double jet_min_pT = 0.0;

  std::vector<TowerJetInput*> background_inputs = 
  {
    new TowerJetInput(Jet::CEMC_TOWERINFO_RETOWER),
    new TowerJetInput(Jet::HCALIN_TOWERINFO),
    new TowerJetInput(Jet::HCALOUT_TOWERINFO)
  };
  std::vector<std::string> rho_node_names;
  if(do_rho_Area) rho_node_names.push_back("TowerRho_AREA");
  if(do_rho_Mult) rho_node_names.push_back("TowerRho_MULT");

  //-----------------------------------
  // random cone reco
  double cone_radius = 0.4;
  int random_seed = 42;

  bool do_basic_reconstruction = true;
  bool do_randomize_etaphi = true;
  
  bool do_avoid_leading_jet = false;
  double lead_jet_dR = 1.4;
  double lead_jet_pT_threshold = 5.0; // GeV

  std::string output_node_name = "RandomCone_Tower";
  std::vector<TowerJetInput*> random_cone_inputs = 
  {
    new TowerJetInput(Jet::CEMC_TOWERINFO_RETOWER),
    new TowerJetInput(Jet::HCALIN_TOWERINFO),
    new TowerJetInput(Jet::HCALOUT_TOWERINFO)
  };
  std::vector<std::string> random_cone_node_names;
  if(do_basic_reconstruction) random_cone_node_names.push_back( output_node_name + "_basic_r0" + std::to_string(int(cone_radius*10)) );
  if(do_randomize_etaphi) random_cone_node_names.push_back( output_node_name + "_randomize_etaphi_r0" + std::to_string(int(cone_radius*10)) );
  if(do_avoid_leading_jet) random_cone_node_names.push_back( output_node_name + "_avoid_lead_jet_r0" + std::to_string(int(cone_radius*10)) );

  std::string output_node_name_sub1 = "RandomCone_Tower_Sub1";
  std::vector<TowerJetInput*> random_cone_inputs_sub1 = 
  {
    new TowerJetInput(Jet::CEMC_TOWERINFO_SUB1),
    new TowerJetInput(Jet::HCALIN_TOWERINFO_SUB1),
    new TowerJetInput(Jet::HCALOUT_TOWERINFO_SUB1)
  };
  if(do_basic_reconstruction) random_cone_node_names.push_back( output_node_name_sub1 + "_basic_r0" + std::to_string(int(cone_radius*10)) );
  if(do_randomize_etaphi) random_cone_node_names.push_back( output_node_name_sub1 + "_randomize_etaphi_r0" + std::to_string(int(cone_radius*10)) );
  if(do_avoid_leading_jet) random_cone_node_names.push_back( output_node_name_sub1 + "_avoid_lead_jet_r0" + std::to_string(int(cone_radius*10)) );

  //-----------------------------------
  

  //-----------------------------------
  // Fun4All server initialization
  //-----------------------------------

  // create fun4all server
  Fun4AllServer *se = Fun4AllServer::instance();
  int verbosity = 0;
  se->Verbosity(verbosity);
  recoConsts *rc = recoConsts::instance();

  //-----------------------------------
  //  Centrality
  //-----------------------------------

  if(do_centrality)
  {
    PHG4CentralityReco *cent = new PHG4CentralityReco();
    cent->Verbosity(0);
    cent->GetCalibrationParameters().ReadFromFile("centrality", "xml", 0, 0, string(getenv("CALIBRATIONROOT")) + string("/Centrality/"));
    se->registerSubsystem( cent );
  }


  //-----------------------------------
  // Jet reco
  //-----------------------------------

  HIJetReco();

  //-----------------------------------
  // Estimating rho
  //-----------------------------------
  DetermineTowerRho *dtr = new DetermineTowerRho();
  if(do_rho_Area) dtr->add_method(TowerRho::Method::AREA); // do area method
  if(do_rho_Mult) dtr->add_method(TowerRho::Method::MULT); // do multiplicy method
  dtr->set_algo(background_algo); // default = KT
  dtr->set_par(background_R); // default = 0.4
  dtr->set_tower_abs_eta(eta_max); // default=1.1
  dtr->set_jet_abs_eta(cone_jet_eta_max); //default is tower_abs_eta - R
  dtr->set_omit_nhardest(omit_nhardest); // default=2
  dtr->set_ghost_area(ghost_area); // default=0.01
  dtr->set_jet_min_pT(jet_min_pT); // default=0.0
  dtr->set_tower_min_pT(tower_pT_min); // default=0.0
  dtr->do_tower_cut(do_tower_cut); // default=false
  dtr->set_tower_threshold(tower_threshold); // default=0.00
  for(auto input : background_inputs) dtr->add_tower_input(input); // add inputs to estimate rho
  dtr->Verbosity(verbosity);
  se->registerSubsystem(dtr);


  //-----------------------------------
  // Random Cone Reco
  //-----------------------------------

  RandomConeReco *rcr = new RandomConeReco();
  rcr->set_cone_radius(cone_radius); // default = 0.4
  rcr->set_seed(random_seed); // seed for random number generator (default is 0 (time ))
  rcr->set_output_node_name(output_node_name); // sets the output node prefix (default is "RandomCone")
  rcr->set_input_max_abs_eta(eta_max); // default (max eta for input towers)
  rcr->set_cone_max_abs_eta(cone_jet_eta_max); //max eta for random cone (default is input_max_abs_eta - cone_radius)
  rcr->set_input_min_pT(tower_pT_min); // default (min pT for input towers)
  rcr->do_tower_cut(do_tower_cut); // (default is false)
  rcr->set_tower_threshold(tower_threshold); // default (threshold for input towers)
  rcr->do_basic_reconstruction(do_basic_reconstruction); // (default is true)
  rcr->do_randomize_etaphi(do_randomize_etaphi); //  (default is false)
  if(do_avoid_leading_jet)
  {
    rcr->do_avoid_leading_jet(do_avoid_leading_jet); // (default is false)
    rcr->set_lead_jet_dR(lead_jet_dR); // defaults to 1.0 + cone_radius
    rcr->set_lead_jet_pT_threshold(lead_jet_pT_threshold); // defaults to 10.0
  
  }
  for(auto input : random_cone_inputs) rcr->add_input(input); // add inputs for random cone reco
  rcr->Verbosity(1);
  se->registerSubsystem(rcr);

  //-----------------------------------
  // same thing but for sub1
  rcr = new RandomConeReco();
  rcr->set_cone_radius(cone_radius); // default = 0.4
  rcr->set_seed(random_seed); // seed for random number generator (default is 0 (time ))
  rcr->set_output_node_name(output_node_name_sub1); // sets the output node prefix (default is "RandomCone")
  rcr->set_input_max_abs_eta(eta_max); // default (max eta for input towers)
  rcr->set_cone_max_abs_eta(cone_jet_eta_max); //max eta for random cone (default is input_max_abs_eta - cone_radius)
  rcr->set_input_min_pT(tower_pT_min); // default (min pT for input towers)
  rcr->do_tower_cut(false); // (default is false)
  rcr->set_tower_threshold(0.0); // default (threshold for input towers)
  rcr->do_basic_reconstruction(do_basic_reconstruction); // (default is true)
  rcr->do_randomize_etaphi(do_randomize_etaphi); //  (default is false)
  if(do_avoid_leading_jet)
  {
    rcr->do_avoid_leading_jet(do_avoid_leading_jet); // (default is false)
    rcr->set_lead_jet_dR(lead_jet_dR); // defaults to 1.0 + cone_radius
    rcr->set_lead_jet_pT_threshold(lead_jet_pT_threshold); // defaults to 10.0
  }
  for(auto input : random_cone_inputs_sub1) rcr->add_input(input); // add inputs for random cone reco
  rcr->Verbosity(1);
  se->registerSubsystem(rcr);




  //-----------------------------------


  // Random Cone Analysis
  //-----------------------------------
  RandomConeTree * rca = new RandomConeTree(outputfile);
  for(auto node_name : random_cone_node_names) rca->add_random_cone_node(node_name); // add random cone node to output tree
  for(auto rho_node_name : rho_node_names) rca->add_rho_node(rho_node_name); // add rho node to output tree
  rca->add_gvtx_cut(-zvrtx_abs_cut, zvrtx_abs_cut); // default is no cut
  rca->do_centrality_info(do_centrality); // default is false (centrality info)
  // rca->do_event_selection_on_leading_truth_jet(30,1000); // default is false (event selection on leading truth jet) MC
  // rca->add_weight_to_ttree(1.0) // add weight to output tree
  rca->do_data(false); // default is false (data)
  rca->Verbosity(1);
  se->registerSubsystem(rca);


  // manual reco
  // RandomConeAna *myJetTree = new RandomConeAna(outputfile_debug);
  // myJetTree->add_input(new TowerJetInput(Jet::CEMC_TOWERINFO_RETOWER));
  // myJetTree->add_input(new TowerJetInput(Jet::HCALIN_TOWERINFO));
  // myJetTree->add_input(new TowerJetInput(Jet::HCALOUT_TOWERINFO));
  // myJetTree->add_iter_input(new TowerJetInput(Jet::CEMC_TOWERINFO_SUB1));
  // myJetTree->add_iter_input(new TowerJetInput(Jet::HCALIN_TOWERINFO_SUB1));
  // myJetTree->add_iter_input(new TowerJetInput(Jet::HCALOUT_TOWERINFO_SUB1));
  // myJetTree->set_user_seed(42);
  // // myJetTree->setEventSelection(30,1000);
  // // myJetTree->addWeight(2.178e-9);
  // myJetTree->Verbosity(verbosity);
  // se->registerSubsystem(myJetTree);









  //-----------------------------------
  // Input managers
  //-----------------------------------

  Fun4AllInputManager *in2 = new Fun4AllDstInputManager("DSTcalo");
  in2->AddListFile(filelistcalo,1);
  se->registerInputManager(in2);

  Fun4AllInputManager *in3 = new Fun4AllDstInputManager("DSTglobal");
  in3->AddListFile(filelistglobal,1);
  se->registerInputManager(in3);

  //-----------------------------------
  // Run the analysis
  //-----------------------------------
  
  se->run(10);
  se->End();

  gSystem->Exit(0);
  return 0;

}
