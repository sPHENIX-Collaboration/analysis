#ifndef MACRO_HIJETRECO_C
#define MACRO_HIJETRECO_C

#include <GlobalVariables.C>

#include <g4jets/TruthJetInput.h>

#include <globalvertex/GlobalVertex.h>

#include <jetbackground/CopyAndSubtractJets.h>
#include <jetbackground/DetermineEventRho.h>
#include <jetbackground/DetermineTowerBackground.h>
#include <jetbackground/DetermineTowerBackgroundv1.h>
#include <jetbackground/DetermineTowerRho.h>
#include <jetbackground/FastJetAlgoSub.h>
#include <jetbackground/RetowerCEMC.h>
#include <jetbackground/SubtractTowers.h>
#include <jetbackground/SubtractTowersCS.h>
#include <jetbackground/SubtractTowersRho.h>
#include <jetbackground/TowerRho.h>

#include <jetbase/FastJetOptions.h>
#include <jetbase/JetReco.h>
#include <jetbase/TowerJetInput.h>
#include <jetbase/TrackJetInput.h>
#include <jetbase/JetCalib.h>

#include <particleflowreco/ParticleFlowJetInput.h>

#include <eventplaneinfo/EventPlaneReco.h>
#include <eventplaneinfo/Eventplaneinfo.h>

#include <fun4all/Fun4AllServer.h>

R__LOAD_LIBRARY(libg4jets.so)
R__LOAD_LIBRARY(libglobalvertex.so)
R__LOAD_LIBRARY(libjetbackground.so)
R__LOAD_LIBRARY(libjetbase.so)
R__LOAD_LIBRARY(libparticleflow.so)
R__LOAD_LIBRARY(libeventplaneinfo.so)

// ----------------------------------------------------------------------------
//! General options for background-subtracted jet reconstruction
// ----------------------------------------------------------------------------
namespace Enable
{
  bool HIJETS = false;        ///< do HI jet reconstruction
  int HIJETS_VERBOSITY = 0;   ///< verbosity
  bool HIJETS_MC = false;     ///< is simulation
  bool HIJETS_TRUTH = false;  ///< make truth jets
  bool HIJETS_TOWER = true;   ///< make tower jets
  bool HIJETS_TOWER_MULTSUB = false; ///< make multi-sub tower jets
  bool HIJETS_TOWER_NOBKG = false;   ///< make unsubtracted (raw) tower jets
  bool HIJETS_TOWER_CALIB = true;    ///< apply jet calibration
  bool HIJETS_TRACK = false;  ///< make track jets
  bool HIJETS_PFLOW = false;  ///< make particle flow jets
}  // namespace Enable

// ----------------------------------------------------------------------------
//! Options specific to background-subtracted jet reconstruction
// ----------------------------------------------------------------------------
namespace HIJETS
{
  // do_flow = 0 --noflow
  // do_flow = 1 --psi2 derived from calo
  // do_flow = 2 --psi2 derived from HIJING
  // do_flow = 3 --psi2 derived from sEPD
  int do_flow = 0;

  ///! Configures event plane reco
  std::string eventplane_node_input = "TOWERINFO_CALIB_SEPD";
  std::string eventplane_node_output = "EventplaneinfoMap";
  std::string eventplane_custom_calib = "default";

  ///! do constituent subtraction
  bool do_CS = false;

  ///! turn on/off functionality only relevant for
  ///! nucleon collisions
  bool is_pp = true;

  ///! sets prefix of nodes to use as tower jet
  ///! input
  std::string tower_prefix = "TOWERINFO_CALIB";

  ///! turn on/off vertex specification
  bool do_vertex_type = true;

  ///! if specifying vertex, set vertex
  ///! to use to this one
  GlobalVertex::VTXTYPE vertex_type = GlobalVertex::MBD;

  ///! Base fastjet options to use. Note that the
  ///! resolution parameter will be overwritten
  ///! to R = 0.2, 0.3, 0.4, and 0.5
  FastJetOptions fj_opts({Jet::ANTIKT, JET_R, 0.4F, VERBOSITY, static_cast<float>(Enable::HIJETS_VERBOSITY)});

  ///! kt fastjet options for seed jets
  FastJetOptions kt_fj_opts({Jet::KT, JET_R, 0.4F, VERBOSITY, static_cast<float>(Enable::HIJETS_VERBOSITY)});

  ///! sets jet node name
  std::string jet_node = "ANTIKT";

  ///! sets prefix of nodes to store jets
  std::string algo_prefix = "AntiKt";

  ///! sets the embedding g4 flag used to determine which particles are classified as 'truth' in MakeHITruthJets
  ///! if you don't know what this is, you probably don't need to change it
  ///! 0 = all particles (use for all g4particles !not useful for HIJING+Pythia samples)
  ///! 1 = pythia/herwig particles only (use for pythia/herwig jets in HIJING+Pythia samples)
  ///! 2 = pythia particles from the HIJING+Pythia samples (use for pythia jets in HIJING+Pythia samples)
  ///! negative values are typically background particles (HIJING particles)
  int embedding_flag = 1;

  ///! enumerates reconstructed resolution
  ///! parameters
  enum Res
  {
    R02 = 0,
    R03 = 1,
    R04 = 2,
    R05 = 3
  };

  // --------------------------------------------------------------------------
  //! Helper method to generate releveant FastJet algorithms
  // --------------------------------------------------------------------------
  FastJetAlgoSub *GetFJAlgo(const float reso)
  {
    // grab current options & update
    // reso parameter
    FastJetOptions opts = fj_opts;
    opts.jet_R = reso;

    // create new algorithm
    return new FastJetAlgoSub(opts);

  }  // end 'GetFJAlgo()'
}  // namespace HIJETS

// ----------------------------------------------------------------------------
//! Make jets out of appropriate truth particles
// ----------------------------------------------------------------------------
void MakeHITruthJets()
{
  // set verbosity
  unsigned int verbosity = static_cast<unsigned int>(std::max(Enable::VERBOSITY, Enable::HIJETS_VERBOSITY));

  //---------------
  // Fun4All server
  //---------------
  Fun4AllServer *se = Fun4AllServer::instance();

  // if making track jets, make truth jets out of only charged particles
  if (Enable::HIJETS_TRACK)
  {
    // configure truth jet input for charged particles
    TruthJetInput *ctji = new TruthJetInput(Jet::SRC::CHARGED_PARTICLE);
    ctji->add_embedding_flag(HIJETS::embedding_flag);  // changes depending on signal vs. embedded

    // book jet reconstruction on chargedparticles
    JetReco *chargedtruthjetreco = new JetReco();
    chargedtruthjetreco->add_input(ctji);
    chargedtruthjetreco->add_algo(HIJETS::GetFJAlgo(0.2F), HIJETS::algo_prefix + "_ChargedTruth_r02");
    chargedtruthjetreco->add_algo(HIJETS::GetFJAlgo(0.3F), HIJETS::algo_prefix + "_ChargedTruth_r03");
    chargedtruthjetreco->add_algo(HIJETS::GetFJAlgo(0.4F), HIJETS::algo_prefix + "_ChargedTruth_r04");
    chargedtruthjetreco->add_algo(HIJETS::GetFJAlgo(0.5F), HIJETS::algo_prefix + "_ChargedTruth_r05");
    chargedtruthjetreco->set_algo_node(HIJETS::jet_node);
    chargedtruthjetreco->set_input_node("TRUTH");
    chargedtruthjetreco->Verbosity(verbosity);
    se->registerSubsystem(chargedtruthjetreco);
  }

  // if making tower or pflow jets, make truth jets out of all particles
  if (Enable::HIJETS_TOWER || Enable::HIJETS_PFLOW)
  {
    // configure truth jet input for all particles
    TruthJetInput *tji = new TruthJetInput(Jet::PARTICLE);
    tji->add_embedding_flag(HIJETS::embedding_flag);  // changes depending on signal vs. embedded

    // book jet reconstruction on all particles
    JetReco *truthjetreco = new JetReco();
    truthjetreco->add_input(tji);
    truthjetreco->add_algo(HIJETS::GetFJAlgo(0.2F), HIJETS::algo_prefix + "_Truth_r02");
    truthjetreco->add_algo(HIJETS::GetFJAlgo(0.3F), HIJETS::algo_prefix + "_Truth_r03");
    truthjetreco->add_algo(HIJETS::GetFJAlgo(0.4F), HIJETS::algo_prefix + "_Truth_r04");
    truthjetreco->add_algo(HIJETS::GetFJAlgo(0.5F), HIJETS::algo_prefix + "_Truth_r05");
    truthjetreco->set_algo_node(HIJETS::jet_node);
    truthjetreco->set_input_node("TRUTH");
    truthjetreco->Verbosity(verbosity);
    se->registerSubsystem(truthjetreco);
  }

  // exit back to HIJetReco()
  return;
}

// ----------------------------------------------------------------------------
//! Common setup for tower jet reconstruction
// ----------------------------------------------------------------------------
void HIJetRecoCommon()
{
  unsigned int verbosity = static_cast<unsigned int>(std::max(Enable::VERBOSITY, Enable::HIJETS_VERBOSITY));
  Fun4AllServer *se = Fun4AllServer::instance();

  if (HIJETS::do_flow == 3)
  {
    EventPlaneReco *epreco = new EventPlaneReco();
    if (HIJETS::eventplane_custom_calib != "default")
    {
      epreco->set_directURL_EventPlaneCalib(HIJETS::eventplane_custom_calib);
    }
    epreco->set_inputNode(HIJETS::eventplane_node_input);
    epreco->set_EventPlaneInfoNodeName(HIJETS::eventplane_node_output);
    se->registerSubsystem(epreco);
  }

  RetowerCEMC *rcemc = new RetowerCEMC();
  rcemc->Verbosity(verbosity);
  rcemc->set_towerinfo(true);
  rcemc->set_frac_cut(1);  // fraction of retower that must be masked to mask the full retower. 1=do not apply extra masking
  rcemc->set_do_rescale(false); // scale the retowered towers up to account for dead area?
  rcemc->set_towerNodePrefix(HIJETS::tower_prefix);
  se->registerSubsystem(rcemc);
}

// ----------------------------------------------------------------------------
//! Make jets out of subtracted towers
// ----------------------------------------------------------------------------
void MakeHITowerJets()
{
  unsigned int verbosity = static_cast<unsigned int>(std::max(Enable::VERBOSITY, Enable::HIJETS_VERBOSITY));

  //---------------
  // Fun4All server
  //---------------
  Fun4AllServer *se = Fun4AllServer::instance();

  JetReco *towerjetreco = new JetReco();
  TowerJetInput *incemc = new TowerJetInput(Jet::CEMC_TOWERINFO_RETOWER, HIJETS::tower_prefix);
  TowerJetInput *inihcal = new TowerJetInput(Jet::HCALIN_TOWERINFO, HIJETS::tower_prefix);
  TowerJetInput *inohcal = new TowerJetInput(Jet::HCALOUT_TOWERINFO, HIJETS::tower_prefix);
  if (HIJETS::do_vertex_type)
  {
    incemc->set_GlobalVertexType(HIJETS::vertex_type);
    inihcal->set_GlobalVertexType(HIJETS::vertex_type);
    inohcal->set_GlobalVertexType(HIJETS::vertex_type);
  }
  towerjetreco->add_input(incemc);
  towerjetreco->add_input(inihcal);
  towerjetreco->add_input(inohcal);
  towerjetreco->add_algo(HIJETS::GetFJAlgo(0.2F), HIJETS::algo_prefix + "_TowerInfo_HIRecoSeedsRaw_r02");
  towerjetreco->set_algo_node(HIJETS::jet_node);
  towerjetreco->set_input_node("TOWER");
  towerjetreco->Verbosity(verbosity);
  se->registerSubsystem(towerjetreco);

  DetermineTowerBackground *dtb = new DetermineTowerBackground("DetermineTowerBackground_Sub1");
  dtb->SetBackgroundOutputName("TowerInfoBackground_Sub1");
  dtb->SetFlow(HIJETS::do_flow);
  dtb->SetSeedType(0);
  dtb->SetSeedJetD(3);
  // dtb->set_towerinfo(true);
  dtb->Verbosity(verbosity);
  dtb->set_towerNodePrefix(HIJETS::tower_prefix);
  dtb->set_EventPlaneInfoNodeName(HIJETS::eventplane_node_output);
  se->registerSubsystem(dtb);

  CopyAndSubtractJets *casj = new CopyAndSubtractJets();
  casj->SetFlowModulation(HIJETS::do_flow);
  casj->Verbosity(verbosity);
  casj->set_towerinfo(true);
  casj->set_towerNodePrefix(HIJETS::tower_prefix);
  se->registerSubsystem(casj);

  DetermineTowerBackground *dtb2 = new DetermineTowerBackground("DetermineTowerBackground_Sub2");
  dtb2->SetBackgroundOutputName("TowerInfoBackground_Sub2");
  dtb2->SetFlow(HIJETS::do_flow);
  dtb2->SetSeedType(1);
  dtb2->SetSeedJetPt(7);
  dtb2->Verbosity(verbosity);
  // dtb2->set_towerinfo(true);
  dtb2->set_towerNodePrefix(HIJETS::tower_prefix);
  dtb2->set_EventPlaneInfoNodeName(HIJETS::eventplane_node_output);
  se->registerSubsystem(dtb2);

  SubtractTowers *st = new SubtractTowers();
  st->SetFlowModulation(HIJETS::do_flow);
  st->Verbosity(verbosity);
  st->set_towerinfo(true);
  st->set_towerNodePrefix(HIJETS::tower_prefix);
  se->registerSubsystem(st);

  towerjetreco = new JetReco();
  incemc = new TowerJetInput(Jet::CEMC_TOWERINFO_SUB1, HIJETS::tower_prefix);
  inihcal = new TowerJetInput(Jet::HCALIN_TOWERINFO_SUB1, HIJETS::tower_prefix);
  inohcal = new TowerJetInput(Jet::HCALOUT_TOWERINFO_SUB1, HIJETS::tower_prefix);
  if (HIJETS::do_vertex_type)
  {
    incemc->set_GlobalVertexType(HIJETS::vertex_type);
    inihcal->set_GlobalVertexType(HIJETS::vertex_type);
    inohcal->set_GlobalVertexType(HIJETS::vertex_type);
  }
  towerjetreco->add_input(incemc);
  towerjetreco->add_input(inihcal);
  towerjetreco->add_input(inohcal);
  towerjetreco->add_algo(HIJETS::GetFJAlgo(0.2F), HIJETS::algo_prefix + "_Tower_r02_Sub1");
  towerjetreco->add_algo(HIJETS::GetFJAlgo(0.3F), HIJETS::algo_prefix + "_Tower_r03_Sub1");
  towerjetreco->set_algo_node(HIJETS::jet_node);
  towerjetreco->set_input_node("TOWER");
  towerjetreco->Verbosity(verbosity);
  se->registerSubsystem(towerjetreco);

  if (Enable::HIJETS_TOWER_CALIB)
  {
    for (const auto & R : {0.2, 0.3})
    {
      int r_int = static_cast<int>(R * 10);
      JetCalib* jetCalib = new JetCalib(Form("JetCalib_Tower_r0%d_Sub1", r_int));
      jetCalib->set_InputNode(Form("%s_Tower_r0%d_Sub1", HIJETS::algo_prefix.c_str(), r_int));
      jetCalib->set_OutputNode(Form("%s_Tower_r0%d_Sub1_calib", HIJETS::algo_prefix.c_str(), r_int));
      jetCalib->set_JetRadius(R);
      jetCalib->set_ZvrtxNode("GlobalVertexMap");
      jetCalib->set_ApplyZvrtxDependentCalib(true);
      jetCalib->set_ApplyEtaDependentCalib(true);
      se->registerSubsystem(jetCalib);
    }
  }

  return;
}

// ----------------------------------------------------------------------------
//! Make jets out of subtracted towers with mult-embedded background subtraction
// ----------------------------------------------------------------------------
void MakeHITowerJetsMultSub()
{
  unsigned int verbosity = static_cast<unsigned int>(std::max(Enable::VERBOSITY, Enable::HIJETS_VERBOSITY));

  //---------------
  // Fun4All server
  //---------------
  Fun4AllServer *se = Fun4AllServer::instance();

  auto GetTowerInput = [](const Jet::SRC src, const std::string & prefix = "TOWERINFO_CALIB") {
    auto * input = new TowerJetInput(src, prefix);
    if (HIJETS::do_vertex_type) input->set_GlobalVertexType(HIJETS::vertex_type);
    return input;
  };

  std::string seed_jet_name = Form("Kt_TowerInfo_HIRecoSeedsRaw_r0%d", static_cast<int>(HIJETS::kt_fj_opts.jet_R * 10));

  JetReco *towerjetreco = new JetReco("TowerJetReco_Seed");
  for (const auto & src : { Jet::CEMC_TOWERINFO_RETOWER, Jet::HCALIN_TOWERINFO, Jet::HCALOUT_TOWERINFO })
  {
    towerjetreco->add_input(GetTowerInput(src, HIJETS::tower_prefix));
  }
  towerjetreco->add_algo(new FastJetAlgoSub(HIJETS::kt_fj_opts), seed_jet_name);
  towerjetreco->set_algo_node(HIJETS::jet_node);
  towerjetreco->set_input_node("TOWER");
  towerjetreco->Verbosity(verbosity);
  se->registerSubsystem(towerjetreco);

  DetermineTowerRho *dt_rho = new DetermineTowerRho("DetermineTowerRho_CEMC_MultEmb");
  dt_rho->add_method(TowerRho::Method::MULT, "TowerRho_MULT_CEMC");
  dt_rho->add_tower_input(GetTowerInput(Jet::CEMC_TOWERINFO_RETOWER, HIJETS::tower_prefix));
  dt_rho->Verbosity(verbosity);
  se->registerSubsystem(dt_rho);

  dt_rho = new DetermineTowerRho("DetermineTowerRho_HCALIN_MultEmb");
  dt_rho->add_method(TowerRho::Method::MULT, "TowerRho_MULT_HCALIN");
  dt_rho->add_tower_input(GetTowerInput(Jet::HCALIN_TOWERINFO, HIJETS::tower_prefix));
  dt_rho->Verbosity(verbosity);
  se->registerSubsystem(dt_rho);

  dt_rho = new DetermineTowerRho("DetermineTowerRho_HCALOUT_MultEmb");
  dt_rho->add_method(TowerRho::Method::MULT, "TowerRho_MULT_HCALOUT");
  dt_rho->add_tower_input(GetTowerInput(Jet::HCALOUT_TOWERINFO, HIJETS::tower_prefix));
  dt_rho->Verbosity(verbosity);
  se->registerSubsystem(dt_rho);

  SubtractTowersRho *sub_rho = new SubtractTowersRho("SubtractTowersRho_CEMC_MultEmb");
  sub_rho->set_rhoNode("TowerRho_MULT_CEMC");
  sub_rho->add_targetTowerNode(HIJETS::tower_prefix + "_CEMC_RETOWER");
  sub_rho->set_subSuffix("MULTSUB");
  if (HIJETS::do_vertex_type) sub_rho->set_globalVertexType(HIJETS::vertex_type);
  sub_rho->Verbosity(verbosity);
  se->registerSubsystem(sub_rho);

  sub_rho = new SubtractTowersRho("SubtractTowersRho_HCALIN_MultEmb");
  sub_rho->set_rhoNode("TowerRho_MULT_HCALIN");
  sub_rho->add_targetTowerNode(HIJETS::tower_prefix + "_HCALIN");
  sub_rho->set_subSuffix("MULTSUB");
  if (HIJETS::do_vertex_type) sub_rho->set_globalVertexType(HIJETS::vertex_type);
  sub_rho->Verbosity(verbosity);
  se->registerSubsystem(sub_rho);

  sub_rho = new SubtractTowersRho("SubtractTowersRho_HCALOUT_MultEmb");
  sub_rho->set_rhoNode("TowerRho_MULT_HCALOUT");
  sub_rho->add_targetTowerNode(HIJETS::tower_prefix + "_HCALOUT");
  sub_rho->set_subSuffix("MULTSUB");
  if (HIJETS::do_vertex_type) sub_rho->set_globalVertexType(HIJETS::vertex_type);
  sub_rho->Verbosity(verbosity);
  se->registerSubsystem(sub_rho);

  DetermineTowerBackgroundv1 *dtb = new DetermineTowerBackgroundv1("DetermineTowerBackgroundv1_Emb");
  dtb->SetBackgroundOutputName("TowerInfoBackground_MultSub2");
  if (HIJETS::do_flow == 0)
  {
    dtb->SetFlowMode(DetermineTowerBackgroundv1::FlowMode::NoFlow);
    dtb->SetPsi2Mode(DetermineTowerBackgroundv1::Psi2Mode::NoPsi2);
  }
  else
  {
    dtb->SetFlowMode(DetermineTowerBackgroundv1::FlowMode::EBE);
    if (HIJETS::do_flow == 1) dtb->SetPsi2Mode(DetermineTowerBackgroundv1::Psi2Mode::Calo);
    else if (HIJETS::do_flow == 2) dtb->SetPsi2Mode(DetermineTowerBackgroundv1::Psi2Mode::Truth);
    else if (HIJETS::do_flow == 3) dtb->SetPsi2Mode(DetermineTowerBackgroundv1::Psi2Mode::sEPD);
  }
  dtb->SetSeedJetName(seed_jet_name);
  dtb->SetCEMC_RhoNode("TowerRho_MULT_CEMC");
  dtb->SetIHCAL_RhoNode("TowerRho_MULT_HCALIN");
  dtb->SetOHCAL_RhoNode("TowerRho_MULT_HCALOUT");
  dtb->SetNOmitSeeds(2);
  dtb->Verbosity(verbosity);
  dtb->UseReweighting(true);
  se->registerSubsystem(dtb);

  SubtractTowers *st = new SubtractTowers("SubtractTowers_Emb");
  st->set_towerNodePrefix("MULTSUB_" + HIJETS::tower_prefix);
  st->set_inputTowerBackgroundNode("TowerInfoBackground_MultSub2");
  st->SetFlowModulation(HIJETS::do_flow);
  st->Verbosity(verbosity);
  st->set_towerinfo(true);
  se->registerSubsystem(st);

  towerjetreco = new JetReco("TowerJetReco_Emb_Sub1");
  for (const auto & src : { Jet::CEMC_TOWERINFO_SUB1, Jet::HCALIN_TOWERINFO_SUB1, Jet::HCALOUT_TOWERINFO_SUB1 })
  {
    towerjetreco->add_input(GetTowerInput(src, "MULTSUB_" + HIJETS::tower_prefix));
  }
  for (const auto & R : {0.2, 0.3})
  {
    towerjetreco->add_algo(HIJETS::GetFJAlgo(R), Form("%s_Tower_r0%d_MultSub1", HIJETS::algo_prefix.c_str(), static_cast<int>(R * 10)));
  }
  towerjetreco->set_algo_node(HIJETS::jet_node);
  towerjetreco->set_input_node("TOWER");
  towerjetreco->Verbosity(verbosity);
  se->registerSubsystem(towerjetreco);

  // towerjetreco = new JetReco("TowerJetReco_Rho");
  // for (const auto & src : { Jet::CEMC_TOWERINFO_RETOWER, Jet::HCALIN_TOWERINFO, Jet::HCALOUT_TOWERINFO })
  // {
  //   towerjetreco->add_input(GetTowerInput(src, "MULTSUB_" + HIJETS::tower_prefix));
  // }
  // for (const auto & R : { 0.3 })
  // {
  //   towerjetreco->add_algo(HIJETS::GetFJAlgo(R), Form("%s_TowerInfo_r0%d_Rho", HIJETS::algo_prefix.c_str(), static_cast<int>(R * 10)));
  // }
  // towerjetreco->set_algo_node(HIJETS::jet_node);
  // towerjetreco->set_input_node("TOWER");
  // towerjetreco->Verbosity(verbosity);
  // se->registerSubsystem(towerjetreco);

  if (Enable::HIJETS_TOWER_CALIB)
  {
    for (const auto & R : {0.2, 0.3})
    {
      int r_int = static_cast<int>(R * 10);
      JetCalib* jetCalib = new JetCalib(Form("JetCalib_Tower_r0%d_MultSub1", r_int));
      jetCalib->set_InputNode(Form("%s_Tower_r0%d_MultSub1", HIJETS::algo_prefix.c_str(), r_int));
      jetCalib->set_OutputNode(Form("%s_Tower_r0%d_MultSub1_calib", HIJETS::algo_prefix.c_str(), r_int));
      jetCalib->set_JetRadius(R);
      jetCalib->set_ZvrtxNode("GlobalVertexMap");
      jetCalib->set_ApplyZvrtxDependentCalib(true);
      jetCalib->set_ApplyEtaDependentCalib(true);
      se->registerSubsystem(jetCalib);
    }
  }

  return;
}

// ----------------------------------------------------------------------------
//! Make unsubtracted (raw) tower jets
// ----------------------------------------------------------------------------
void MakeHITowerJetsNoBkg()
{
  unsigned int verbosity = static_cast<unsigned int>(std::max(Enable::VERBOSITY, Enable::HIJETS_VERBOSITY));
  Fun4AllServer *se = Fun4AllServer::instance();

  auto GetTowerInput = [](const Jet::SRC src, const std::string & prefix = "TOWERINFO_CALIB") {
    auto * input = new TowerJetInput(src, prefix);
    if (HIJETS::do_vertex_type) input->set_GlobalVertexType(HIJETS::vertex_type);
    return input;
  };

  JetReco *towerjetreco = new JetReco("TowerJetReco_Raw");
  for (const auto & src : { Jet::CEMC_TOWERINFO_RETOWER, Jet::HCALIN_TOWERINFO, Jet::HCALOUT_TOWERINFO })
  {
    towerjetreco->add_input(GetTowerInput(src, HIJETS::tower_prefix));
  }
  for (const auto & R : {0.2, 0.3})
  {
    towerjetreco->add_algo(HIJETS::GetFJAlgo(R), Form("%s_TowerInfo_r0%d", HIJETS::algo_prefix.c_str(), static_cast<int>(R * 10)));
  }
  towerjetreco->set_algo_node(HIJETS::jet_node);
  towerjetreco->set_input_node("TOWER");
  towerjetreco->Verbosity(verbosity);
  se->registerSubsystem(towerjetreco);

  if (Enable::HIJETS_TOWER_CALIB)
  {
    for (const auto & R : {0.2, 0.3})
    {
      int r_int = static_cast<int>(R * 10);
      JetCalib* jetCalib = new JetCalib(Form("JetCalib_TowerInfo_r0%d", r_int));
      jetCalib->set_InputNode(Form("%s_TowerInfo_r0%d", HIJETS::algo_prefix.c_str(), r_int));
      jetCalib->set_OutputNode(Form("%s_TowerInfo_r0%d_calib", HIJETS::algo_prefix.c_str(), r_int));
      jetCalib->set_JetRadius(R);
      jetCalib->set_ZvrtxNode("GlobalVertexMap");
      jetCalib->set_ApplyZvrtxDependentCalib(true);
      jetCalib->set_ApplyEtaDependentCalib(true);
      se->registerSubsystem(jetCalib);
    }
  }

  return;
}

// ----------------------------------------------------------------------------
//! Make jets out of tracks with background subtraction
// ----------------------------------------------------------------------------
void MakeHITrackJets()
{
  // set verbosity
  unsigned int verbosity = static_cast<unsigned int>(std::max(Enable::VERBOSITY, Enable::HIJETS_VERBOSITY));

  //---------------
  // Fun4All server
  //---------------
  Fun4AllServer *se = Fun4AllServer::instance();

  // emit warning: background sub will be added later
  std::cerr << "WARNING: Background subtraction for track jets is still in development!\n"
            << "  If you want to do jet reco without background subtraction, please\n"
            << "  use NoBkgdSubJetReco()"
            << std::endl;

  // book jet reconstruction routines on tracks
  JetReco *trackjetreco = new JetReco();
  trackjetreco->add_input(new TrackJetInput(Jet::SRC::TRACK));
  trackjetreco->add_algo(HIJETS::GetFJAlgo(0.2F), HIJETS::algo_prefix + "_Track_r02");
  trackjetreco->add_algo(HIJETS::GetFJAlgo(0.3F), HIJETS::algo_prefix + "_Track_r03");
  trackjetreco->add_algo(HIJETS::GetFJAlgo(0.4F), HIJETS::algo_prefix + "_Track_r04");
  trackjetreco->add_algo(HIJETS::GetFJAlgo(0.5F), HIJETS::algo_prefix + "_Track_r05");
  trackjetreco->set_algo_node(HIJETS::jet_node);
  trackjetreco->set_input_node("TRACK");
  trackjetreco->Verbosity(verbosity);
  se->registerSubsystem(trackjetreco);

  // exit back to HIJetReco()
  return;
}

// ----------------------------------------------------------------------------
//! Make jets out of particle-flow elements with background subtraction
// ----------------------------------------------------------------------------
void MakeHIPFlowJets()
{
  // set verbosity
  unsigned int verbosity = static_cast<unsigned int>(std::max(Enable::VERBOSITY, Enable::HIJETS_VERBOSITY));

  //---------------
  // Fun4All server
  //---------------
  Fun4AllServer *se = Fun4AllServer::instance();

  // emit warning: background sub will be added later
  std::cerr << "WARNING: Background subtraction for particle-flow jets is still in development!\n"
            << "  If you want to do jet reco without background subtraction, please\n"
            << "  use NoBkgdSubJetReco.C"
            << std::endl;

  // book jet reconstruction routines on pflow elements
  JetReco *pflowjetreco = new JetReco();
  pflowjetreco->add_input(new ParticleFlowJetInput());
  pflowjetreco->add_algo(HIJETS::GetFJAlgo(0.2F), HIJETS::algo_prefix + "_ParticleFlow_r02");
  pflowjetreco->add_algo(HIJETS::GetFJAlgo(0.3F), HIJETS::algo_prefix + "_ParticleFlow_r03");
  pflowjetreco->add_algo(HIJETS::GetFJAlgo(0.4F), HIJETS::algo_prefix + "_ParticleFlow_r04");
  pflowjetreco->add_algo(HIJETS::GetFJAlgo(0.5F), HIJETS::algo_prefix + "_ParticleFlow_r05");
  pflowjetreco->set_algo_node(HIJETS::jet_node);
  pflowjetreco->set_input_node("ELEMENT");
  pflowjetreco->Verbosity(verbosity);
  se->registerSubsystem(pflowjetreco);

  // exit back to HIJetReco()
  return;
}

// ----------------------------------------------------------------------------
//! Run background-subtracted jet reconstruction
// ----------------------------------------------------------------------------
void HIJetReco()
{
  // if simulation, make appropriate truth jets
  if (Enable::HIJETS_MC && Enable::HIJETS_TRUTH)
  {
    MakeHITruthJets();
  }

  // run approriate jet reconstruction routines
  if (Enable::HIJETS_TOWER || Enable::HIJETS_TOWER_MULTSUB || Enable::HIJETS_TOWER_NOBKG)
  {
    HIJetRecoCommon();
  }

  if (Enable::HIJETS_TOWER)
  {
    MakeHITowerJets();
  }
  if (Enable::HIJETS_TOWER_MULTSUB)
  {
    MakeHITowerJetsMultSub();
  }
  if (Enable::HIJETS_TOWER_NOBKG)
  {
    MakeHITowerJetsNoBkg();
  }
  if (Enable::HIJETS_TRACK)
  {
    MakeHITrackJets();
  }
  if (Enable::HIJETS_PFLOW)
  {
    MakeHIPFlowJets();
  }
}

// ----------------------------------------------------------------------------
//! Determine rho from tower input to jet reco (necessary for jet QA)
// ----------------------------------------------------------------------------
void DoRhoCalculation()
{
  // set verbosity
  //  int verbosity = std::max(Enable::VERBOSITY, Enable::HIJETS_VERBOSITY);

  //---------------
  // Fun4All server
  //---------------
  Fun4AllServer *se = Fun4AllServer::instance();

  // run rho calculations w/ towers
  if (Enable::HIJETS_TOWER)
  {
    DetermineTowerRho *towRhoCalc = new DetermineTowerRho();
    towRhoCalc->add_method(TowerRho::Method::AREA);
    towRhoCalc->add_method(TowerRho::Method::MULT);
    TowerJetInput *rho_incemc = new TowerJetInput(Jet::CEMC_TOWERINFO_RETOWER, HIJETS::tower_prefix);
    TowerJetInput *rho_inihcal = new TowerJetInput(Jet::HCALIN_TOWERINFO, HIJETS::tower_prefix);
    TowerJetInput *rho_inohcal = new TowerJetInput(Jet::HCALOUT_TOWERINFO, HIJETS::tower_prefix);
    if (HIJETS::do_vertex_type)
    {
      rho_incemc->set_GlobalVertexType(HIJETS::vertex_type);
      rho_inihcal->set_GlobalVertexType(HIJETS::vertex_type);
      rho_inohcal->set_GlobalVertexType(HIJETS::vertex_type);
    }
    towRhoCalc->add_tower_input(rho_incemc);
    towRhoCalc->add_tower_input(rho_inihcal);
    towRhoCalc->add_tower_input(rho_inohcal);
    se->registerSubsystem(towRhoCalc);
  }

  // run rho calculations w/ tracks
  if (Enable::HIJETS_TRACK)
  {
    DetermineEventRho *trkRhoCalc = new DetermineEventRho();
    trkRhoCalc->add_method(EventRho::Method::AREA, "EventRho_AREA");
    trkRhoCalc->add_method(EventRho::Method::MULT, "EventRho_MULT");
    trkRhoCalc->add_input(new TrackJetInput(Jet::SRC::TRACK));
    se->registerSubsystem(trkRhoCalc);
  }

  // exit back to main macro
  return;
}

#endif
