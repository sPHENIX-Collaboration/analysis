#include "Proto4TowerCalib.h"

#include <calobase/RawTowerContainer.h>
#include <ffaobjects/EventHeader.h>
#include <pdbcalbase/PdbParameterMap.h>
#include <phparameter/PHParameters.h>
#include <prototype4/RawTower_Prototype4.h>
#include <prototype4/RawTower_Temperature.h>

#include <Event/EventTypes.h>
#include <fun4all/Fun4AllHistoManager.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/PHTFileServer.h>
#include <fun4all/SubsysReco.h>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include <phool/PHCompositeNode.h>

#include <g4main/PHG4Particle.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4VtxPoint.h>

#include <TFile.h>
#include <TString.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TLorentzVector.h>
#include <TNtuple.h>
#include <TVector3.h>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <exception>
#include <iostream>
#include <set>
#include <sstream>
#include <stdexcept>
#include <vector>

using namespace std;

ClassImp(Proto4TowerCalib::HCAL_Tower);
ClassImp(Proto4TowerCalib::Eval_Run);

Proto4TowerCalib::Proto4TowerCalib(const std::string &filename)
  : SubsysReco("Proto4TowerCalib")
  , _is_sim(true)
  , _filename(filename)
  , _ievent(0)
{
  Verbosity(1);

  _eval_run.reset();
  _tower.reset();
}

Proto4TowerCalib::~Proto4TowerCalib()
{
}

Fun4AllHistoManager *
Proto4TowerCalib::get_HistoManager()
{
  Fun4AllServer *se = Fun4AllServer::instance();
  Fun4AllHistoManager *hm = se->getHistoManager("Proto4TowerCalib_HISTOS");

  if (not hm)
  {
    cout
        << "Proto4TowerCalib::get_HistoManager - Making Fun4AllHistoManager Proto4TowerCalib_HISTOS"
        << endl;
    hm = new Fun4AllHistoManager("Proto4TowerCalib_HISTOS");
    se->registerHistoManager(hm);
  }

  assert(hm);

  return hm;
}

int Proto4TowerCalib::InitRun(PHCompositeNode *topNode)
{
  if (Verbosity())
    cout << "Proto4TowerCalib::InitRun" << endl;

  _ievent = 0;

  PHNodeIterator iter(topNode);
  PHCompositeNode *dstNode = static_cast<PHCompositeNode *>(iter.findFirst(
      "PHCompositeNode", "DST"));
  if (!dstNode)
  {
    std::cerr << PHWHERE << "DST Node missing, doing nothing." << std::endl;
    throw runtime_error(
        "Failed to find DST node in EmcRawTowerBuilder::CreateNodes");
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

int Proto4TowerCalib::End(PHCompositeNode *topNode)
{
  cout << "Proto4TowerCalib::End - write to " << _filename << endl;
  PHTFileServer::get().cd(_filename);

  Fun4AllHistoManager *hm = get_HistoManager();
  assert(hm);
  for (unsigned int i = 0; i < hm->nHistos(); i++)
    hm->getHisto(i)->Write();

  //  if (_T_EMCalTrk)
  //    _T_EMCalTrk->Write();

  return Fun4AllReturnCodes::EVENT_OK;
}

int Proto4TowerCalib::Init(PHCompositeNode *topNode)
{
  _ievent = 0;

  cout << "Proto4TowerCalib::get_HistoManager - Making PHTFileServer "
       << _filename << endl;
  PHTFileServer::get().open(_filename, "RECREATE");

  Fun4AllHistoManager *hm = get_HistoManager();
  assert(hm);

  //! Histogram of Cherenkov counters
  TH2F *hCheck_Cherenkov = new TH2F("hCheck_Cherenkov", "hCheck_Cherenkov",
                                    110, -200, 20000, 5, .5, 5.5);
  hCheck_Cherenkov->GetYaxis()->SetBinLabel(1, "C1");
  hCheck_Cherenkov->GetYaxis()->SetBinLabel(2, "C2 in");
  hCheck_Cherenkov->GetYaxis()->SetBinLabel(3, "C2 out");
  hCheck_Cherenkov->GetYaxis()->SetBinLabel(4, "C2 sum");
  hCheck_Cherenkov->GetXaxis()->SetTitle("Amplitude");
  hCheck_Cherenkov->GetYaxis()->SetTitle("Cherenkov Channel");
  hm->registerHisto(hCheck_Cherenkov);

  //! Envent nomalization
  TH1F *hNormalization = new TH1F("hNormalization", "hNormalization", 10, .5,
                                  10.5);
  hNormalization->GetXaxis()->SetBinLabel(1, "ALL");
  hNormalization->GetXaxis()->SetBinLabel(2, "C2-e");
  hNormalization->GetXaxis()->SetBinLabel(3, "trigger_veto_pass");
  hNormalization->GetXaxis()->SetBinLabel(4, "valid_hodo_h");
  hNormalization->GetXaxis()->SetBinLabel(5, "valid_hodo_v");
  hNormalization->GetXaxis()->SetBinLabel(6, "good_e");
  hNormalization->GetXaxis()->SetBinLabel(6, "good_anti_e");
  hNormalization->GetXaxis()->SetTitle("Cuts");
  hNormalization->GetYaxis()->SetTitle("Event count");
  hm->registerHisto(hNormalization);

  hm->registerHisto(new TH1F("hCheck_Veto", "hCheck_Veto", 1000, -500, 1500));
  hm->registerHisto(
      new TH1F("hCheck_Hodo_H", "hCheck_Hodo_H", 1000, -500, 1500));
  hm->registerHisto(
      new TH1F("hCheck_Hodo_V", "hCheck_Hodo_V", 1000, -500, 1500));

  hm->registerHisto(new TH1F("hBeam_Mom", "hBeam_Mom", 1200, -120, 120));

  // HCALIN LG
  TH1F *h_hcalin_tower_sim[16];
  TH1F *h_hcalin_lg_tower_raw[16];
  TH1F *h_hcalin_lg_tower_calib[16];
  for(int i_tower = 0; i_tower < 16; ++i_tower)
  {
    string HistName_sim = Form("h_hcalin_tower_%d_sim",i_tower);
    h_hcalin_tower_sim[i_tower] = new TH1F(HistName_sim.c_str(),HistName_sim.c_str(),500,0.5,100.5);
    hm->registerHisto(h_hcalin_tower_sim[i_tower]);

    string HistName_raw = Form("h_hcalin_lg_tower_%d_raw",i_tower);
    h_hcalin_lg_tower_raw[i_tower] = new TH1F(HistName_raw.c_str(),HistName_raw.c_str(),500,0.5,100.5);
    hm->registerHisto(h_hcalin_lg_tower_raw[i_tower]);

    string HistName_calib = Form("h_hcalin_lg_tower_%d_calib",i_tower);
    h_hcalin_lg_tower_calib[i_tower] = new TH1F(HistName_calib.c_str(),HistName_calib.c_str(),500,0.5,100.5);
    hm->registerHisto(h_hcalin_lg_tower_calib[i_tower]);
  }

  // HCALOUT LG
  TH1F *h_hcalout_tower_sim[16];
  TH1F *h_hcalout_lg_tower_raw[16];
  TH1F *h_hcalout_lg_tower_calib[16];
  for(int i_tower = 0; i_tower < 16; ++i_tower)
  {
    string HistName_sim = Form("h_hcalout_tower_%d_sim",i_tower);
    h_hcalout_tower_sim[i_tower] = new TH1F(HistName_sim.c_str(),HistName_sim.c_str(),500,0.5,100.5);
    hm->registerHisto(h_hcalout_tower_sim[i_tower]);

    string HistName_raw = Form("h_hcalout_lg_tower_%d_raw",i_tower);
    h_hcalout_lg_tower_raw[i_tower] = new TH1F(HistName_raw.c_str(),HistName_raw.c_str(),500,0.5,100.5);
    hm->registerHisto(h_hcalout_lg_tower_raw[i_tower]);

    string HistName_calib = Form("h_hcalout_lg_tower_%d_calib",i_tower);
    h_hcalout_lg_tower_calib[i_tower] = new TH1F(HistName_calib.c_str(),HistName_calib.c_str(),500,0.5,100.5);
    hm->registerHisto(h_hcalout_lg_tower_calib[i_tower]);
  }

  // HCALOUT HG
  TH1F *h_hcalout_hg_tower_raw[16];
  TH1F *h_hcalout_hg_tower_calib[16];
  for(int i_tower = 0; i_tower < 16; ++i_tower)
  {
    string HistName_raw = Form("h_hcalout_hg_tower_%d_raw",i_tower);
    h_hcalout_hg_tower_raw[i_tower] = new TH1F(HistName_raw.c_str(),HistName_raw.c_str(),500,0.5,100.5);
    hm->registerHisto(h_hcalout_hg_tower_raw[i_tower]);

    string HistName_calib = Form("h_hcalout_hg_tower_%d_calib",i_tower);
    h_hcalout_hg_tower_calib[i_tower] = new TH1F(HistName_calib.c_str(),HistName_calib.c_str(),500,0.5,100.5);
    hm->registerHisto(h_hcalout_hg_tower_calib[i_tower]);
  }

  /*
  TH1F *h_hcal_total_sim = new TH1F("h_hcal_total_sim","h_hcal_total_sim",500,-4.5,45.5);
  h_hcal_total_sim->GetXaxis()->SetTitle("Calib Energy (MeV)");
  h_hcal_total_sim->GetYaxis()->SetTitle("counts");
  hm->registerHisto(h_hcal_total_sim);

  TH1F *h_hcal_total_calib = new TH1F("h_hcal_total_calib","h_hcal_total_calib",500,-4.5,45.5);
  h_hcal_total_calib->GetXaxis()->SetTitle("Calib Energy (GeV)");
  h_hcal_total_calib->GetYaxis()->SetTitle("counts");
  hm->registerHisto(h_hcal_total_calib);
  */

  // help index files with TChain
  TTree *T = new TTree("T", "T");
  assert(T);
  hm->registerHisto(T);

  T->Branch("info", &_eval_run);
  T->Branch("sample", &_tower);

  return Fun4AllReturnCodes::EVENT_OK;
}

int Proto4TowerCalib::process_event(PHCompositeNode *topNode)
{
  if (Verbosity() > 2)
    cout << "Proto4TowerCalib::process_event() entered" << endl;

  // init eval objects
  _eval_run.reset();
  _tower.reset();

  Fun4AllHistoManager *hm = get_HistoManager();
  assert(hm);

  if (not _is_sim)
  {
    PdbParameterMap *info = findNode::getClass<PdbParameterMap>(topNode,
                                                                "RUN_INFO");

    assert(info);

    PHParameters run_info_copy("RunInfo");
    run_info_copy.FillFrom(info);

    _eval_run.beam_mom = run_info_copy.get_double_param("beam_MTNRG_GeV");

    TH1F *hBeam_Mom = dynamic_cast<TH1F *>(hm->getHisto("hBeam_Mom"));
    assert(hBeam_Mom);

    hBeam_Mom->Fill(_eval_run.beam_mom);

    _eval_run.beam_2CH_mm = run_info_copy.get_double_param("beam_2CH_mm");
    _eval_run.beam_2CV_mm = run_info_copy.get_double_param("beam_2CV_mm");
  }

  EventHeader *eventheader = findNode::getClass<EventHeader>(topNode,
                                                             "EventHeader");

  /*
  if (eventheader->get_EvtType() != DATAEVENT)
  {
    cout << __PRETTY_FUNCTION__
         << " disgard this event: " << endl;

    eventheader->identify();

    return Fun4AllReturnCodes::DISCARDEVENT;
  }
  */

  if (not _is_sim)
  {
    assert(eventheader);

    _eval_run.run = eventheader->get_RunNumber();
    if (Verbosity() > 4)
      cout << __PRETTY_FUNCTION__ << _eval_run.run << endl;

    _eval_run.event = eventheader->get_EvtSequence();
  }

  if (_is_sim)
  {
    PHG4TruthInfoContainer *truthInfoList = findNode::getClass<
        PHG4TruthInfoContainer>(topNode, "G4TruthInfo");

    assert(truthInfoList);

    _eval_run.run = -1;

    const PHG4Particle *p =
        truthInfoList->GetPrimaryParticleRange().first->second;
    assert(p);

    const PHG4VtxPoint *v = truthInfoList->GetVtx(p->get_vtx_id());
    assert(v);

    _eval_run.beam_mom = sqrt(
        p->get_px() * p->get_px() + p->get_py() * p->get_py() + p->get_pz() * p->get_pz());
    _eval_run.truth_y = v->get_y();
    _eval_run.truth_z = v->get_z();
  }

  // normalization
  TH1F *hNormalization = dynamic_cast<TH1F *>(hm->getHisto("hNormalization"));
  assert(hNormalization);

  hNormalization->Fill("ALL", 1);

  // get DST nodes

  // HCALIN LG information
  RawTowerContainer *TOWER_SIM_HCALIN = findNode::getClass<
      RawTowerContainer>(topNode, "TOWER_SIM_HCALIN");
  if(_is_sim)
  {
    assert(TOWER_SIM_HCALIN);
  }

  RawTowerContainer *TOWER_RAW_LG_HCALIN = findNode::getClass<
      RawTowerContainer>(topNode, "TOWER_RAW_LG_HCALIN");
  assert(TOWER_RAW_LG_HCALIN);

  RawTowerContainer *TOWER_CALIB_LG_HCALIN = findNode::getClass<
      RawTowerContainer>(topNode, "TOWER_CALIB_LG_HCALIN");
  assert(TOWER_CALIB_LG_HCALIN);

  // HCALOUT LG information
  RawTowerContainer *TOWER_SIM_HCALOUT = findNode::getClass<
      RawTowerContainer>(topNode, "TOWER_SIM_HCALOUT");
  if(_is_sim)
  {
    assert(TOWER_SIM_HCALOUT);
  }

  RawTowerContainer *TOWER_RAW_LG_HCALOUT = findNode::getClass<
      RawTowerContainer>(topNode, "TOWER_RAW_LG_HCALOUT");
  assert(TOWER_RAW_LG_HCALOUT);

  RawTowerContainer *TOWER_CALIB_LG_HCALOUT = findNode::getClass<
      RawTowerContainer>(topNode, "TOWER_CALIB_LG_HCALOUT");
  assert(TOWER_CALIB_LG_HCALOUT);

  // HCALOUT HG information
  RawTowerContainer *TOWER_RAW_HG_HCALOUT = findNode::getClass<
      RawTowerContainer>(topNode, "TOWER_RAW_HG_HCALOUT");
  assert(TOWER_RAW_HG_HCALOUT);

  RawTowerContainer *TOWER_CALIB_HG_HCALOUT = findNode::getClass<
      RawTowerContainer>(topNode, "TOWER_CALIB_HG_HCALOUT");
  assert(TOWER_CALIB_HG_HCALOUT);

  // other nodes
  RawTowerContainer *TOWER_CALIB_TRIGGER_VETO = findNode::getClass<
      RawTowerContainer>(topNode, "TOWER_CALIB_TRIGGER_VETO");
  if (not _is_sim)
  {
    assert(TOWER_CALIB_TRIGGER_VETO);
  }

  RawTowerContainer *TOWER_CALIB_HODO_HORIZONTAL = findNode::getClass<
      RawTowerContainer>(topNode, "TOWER_CALIB_HODO_HORIZONTAL");
  if (not _is_sim)
  {
    assert(TOWER_CALIB_HODO_HORIZONTAL);
  }
  RawTowerContainer *TOWER_CALIB_HODO_VERTICAL = findNode::getClass<
      RawTowerContainer>(topNode, "TOWER_CALIB_HODO_VERTICAL");
  if (not _is_sim)
  {
    assert(TOWER_CALIB_HODO_VERTICAL);
  }

  RawTowerContainer *TOWER_CALIB_C1 = findNode::getClass<RawTowerContainer>(
      topNode, "TOWER_CALIB_C1");
  if (not _is_sim)
  {
    assert(TOWER_CALIB_C1);
  }
  RawTowerContainer *TOWER_CALIB_C2 = findNode::getClass<RawTowerContainer>(
      topNode, "TOWER_CALIB_C2");
  if (not _is_sim)
  {
    assert(TOWER_CALIB_C2);
  }

  // Cherenkov
  bool cherekov_e = false;
  bool cherekov_anti_e = false;
  if (not _is_sim)
  {
    RawTower *t_c2_in = NULL;
    RawTower *t_c2_out = NULL;

    t_c2_in = TOWER_CALIB_C2->getTower(0);
    t_c2_out = TOWER_CALIB_C2->getTower(1);

    assert(t_c2_in);
    assert(t_c2_out);

    const double c2_in = t_c2_in->get_energy();
    const double c2_out = t_c2_out->get_energy();
    const double c1 = TOWER_CALIB_C1->getTower(0)->get_energy();

    _eval_run.C2_sum = c2_in + c2_out;
    _eval_run.C1 = c1;

    cherekov_e = (_eval_run.C2_sum) > (abs(_eval_run.beam_mom) >= 10 ? 1000 : 1500);
    cherekov_anti_e = (_eval_run.C2_sum) < 100;
    hNormalization->Fill("C2-e", cherekov_e);

    TH2F *hCheck_Cherenkov = dynamic_cast<TH2F *>(hm->getHisto(
        "hCheck_Cherenkov"));
    assert(hCheck_Cherenkov);
    hCheck_Cherenkov->Fill(c1, "C1", 1);
    hCheck_Cherenkov->Fill(c2_in, "C2 in", 1);
    hCheck_Cherenkov->Fill(c2_out, "C2 out", 1);
    hCheck_Cherenkov->Fill(c2_in + c2_out, "C2 sum", 1);
  }

  // veto
  TH1F *hCheck_Veto = dynamic_cast<TH1F *>(hm->getHisto("hCheck_Veto"));
  assert(hCheck_Veto);
  bool trigger_veto_pass = true;
  if (not _is_sim)
  {
    auto range = TOWER_CALIB_TRIGGER_VETO->getTowers();
    for (auto it = range.first; it != range.second; ++it)
    {
      RawTower *tower = it->second;
      assert(tower);

      hCheck_Veto->Fill(tower->get_energy());

      if (abs(tower->get_energy()) > 0.2)
        trigger_veto_pass = false;
    }
  }
  hNormalization->Fill("trigger_veto_pass", trigger_veto_pass);
  _eval_run.trigger_veto_pass = trigger_veto_pass;

  // hodoscope
  TH1F *hCheck_Hodo_H = dynamic_cast<TH1F *>(hm->getHisto("hCheck_Hodo_H"));
  assert(hCheck_Hodo_H);
  int hodo_h_count = 0;
  if (not _is_sim)
  {
    auto range = TOWER_CALIB_HODO_HORIZONTAL->getTowers();
    for (auto it = range.first; it != range.second; ++it)
    {
      RawTower *tower = it->second;
      assert(tower);

      hCheck_Hodo_H->Fill(tower->get_energy());

      if (abs(tower->get_energy()) >  0.5)
      {
        hodo_h_count++;
        _eval_run.hodo_h = tower->get_id();
      }
    }
  }
  const bool valid_hodo_h = hodo_h_count == 1;
  hNormalization->Fill("valid_hodo_h", valid_hodo_h);
  _eval_run.valid_hodo_h = valid_hodo_h;

  TH1F *hCheck_Hodo_V = dynamic_cast<TH1F *>(hm->getHisto("hCheck_Hodo_V"));
  assert(hCheck_Hodo_V);
  int hodo_v_count = 0;
  if (not _is_sim)
  {
    auto range = TOWER_CALIB_HODO_VERTICAL->getTowers();
    for (auto it = range.first; it != range.second; ++it)
    {
      RawTower *tower = it->second;
      assert(tower);

      hCheck_Hodo_V->Fill(tower->get_energy());

      if (abs(tower->get_energy()) >  0.5)
      {
        hodo_v_count++;
        _eval_run.hodo_v = tower->get_id();
      }
    }
  }
  const bool valid_hodo_v = hodo_v_count == 1;
  _eval_run.valid_hodo_v = valid_hodo_v;
  hNormalization->Fill("valid_hodo_v", valid_hodo_v);

  const bool good_e = (valid_hodo_v and valid_hodo_h and cherekov_e and trigger_veto_pass) and (not _is_sim);
  const bool good_anti_e = (valid_hodo_v and valid_hodo_h and cherekov_anti_e and trigger_veto_pass) and (not _is_sim);
  hNormalization->Fill("good_e", good_e);
  hNormalization->Fill("good_anti_e", good_anti_e);

  // simple clustering by matching to cluster of max energy
  // pair<int, int> max_tower = find_max(TOWER_CALIB_LG_HCALOUT, 1);

  // process HCALIN LG
  double hcalin_sum_e_sim = 0;
  double hcalin_sum_lg_e_raw = 0;
  double hcalin_sum_lg_e_calib = 0;
  {
    auto range_hcalin_sim = TOWER_SIM_HCALIN->getTowers(); // sim 
    for (auto it = range_hcalin_sim.first; it != range_hcalin_sim.second; ++it)
    {
      RawTower *tower = it->second;
      assert(tower);
      // cout << "HCALIN SIM: ";
      // tower->identify();

      const int col = tower->get_bineta();
      const int row = tower->get_binphi();

      if (col < 0 or col >= 4)
        continue;
      if (row < 0 or row >= 4)
        continue;

      const double energy_sim = tower->get_energy();
      hcalin_sum_e_sim += energy_sim;
      _tower.hcalin_twr_sim[row+4*col] = energy_sim;

      string HistName_sim = Form("h_hcalin_tower_%d_sim",row+4*col);
      TH1F *h_hcalin_sim = dynamic_cast<TH1F *>(hm->getHisto(HistName_sim.c_str()));
      assert(h_hcalin_sim);
      h_hcalin_sim->Fill(_tower.hcalin_twr_sim[row+4*col]*1000.0);
    }
    _tower.hcalin_e_sim = hcalin_sum_e_sim;

    auto range_hcalin_lg_raw = TOWER_RAW_LG_HCALIN->getTowers(); // raw
    for (auto it = range_hcalin_lg_raw.first; it != range_hcalin_lg_raw.second; ++it)
    {
      RawTower *tower = it->second;
      assert(tower);
      // cout << "HCALIN RAW: ";
      // tower->identify();

      const int col = tower->get_bineta();
      const int row = tower->get_binphi();

      if (col < 0 or col >= 4)
        continue;
      if (row < 0 or row >= 4)
        continue;

      const double energy_raw = tower->get_energy();
      hcalin_sum_lg_e_raw += energy_raw;
      _tower.hcalin_lg_twr_raw[row+4*col] = energy_raw;

      string HistName_raw = Form("h_hcalin_lg_tower_%d_raw",row+4*col);
      TH1F *h_hcalin_lg_raw = dynamic_cast<TH1F *>(hm->getHisto(HistName_raw.c_str()));
      assert(h_hcalin_lg_raw);
      h_hcalin_lg_raw->Fill(_tower.hcalin_lg_twr_raw[row+4*col]*1000.0);
    }
    _tower.hcalin_lg_e_raw = hcalin_sum_lg_e_raw;

    auto range_hcalin_lg_calib = TOWER_CALIB_LG_HCALIN->getTowers(); // calib
    for (auto it = range_hcalin_lg_calib.first; it != range_hcalin_lg_calib.second; ++it)
    {
      RawTower *tower = it->second;
      assert(tower);

      const int col = tower->get_bineta();
      const int row = tower->get_binphi();

      if (col < 0 or col >= 4)
        continue;
      if (row < 0 or row >= 4)
        continue;

      const double energy_calib = tower->get_energy();
      hcalin_sum_lg_e_calib += energy_calib;
      _tower.hcalin_lg_twr_calib[row+4*col] = energy_calib;

      string HistName_calib = Form("h_hcalin_lg_tower_%d_calib",row+4*col);
      TH1F *h_hcalin_lg_calib = dynamic_cast<TH1F *>(hm->getHisto(HistName_calib.c_str()));
      assert(h_hcalin_lg_calib);
      h_hcalin_lg_calib->Fill(_tower.hcalin_lg_twr_calib[row+4*col]*1000.0);
    }
    _tower.hcalin_lg_e_calib = hcalin_sum_lg_e_calib;
  }

  // process HCALOUT LG
  double hcalout_sum_e_sim = 0;
  double hcalout_sum_lg_e_raw = 0;
  double hcalout_sum_lg_e_calib = 0;
  {
    auto range_hcalout_sim = TOWER_SIM_HCALOUT->getTowers(); // sim 
    for (auto it = range_hcalout_sim.first; it != range_hcalout_sim.second; ++it)
    {
      RawTower *tower = it->second;
      assert(tower);
      // cout << "HCALOUT SIM: ";
      // tower->identify();

      const int col = tower->get_bineta();
      const int row = tower->get_binphi();

      if (col < 0 or col >= 4)
        continue;
      if (row < 0 or row >= 4)
        continue;

      const double energy_sim = tower->get_energy();
      hcalout_sum_e_sim += energy_sim;
      _tower.hcalout_twr_sim[row+4*col] = energy_sim;

      string HistName_sim = Form("h_hcalout_tower_%d_sim",row+4*col);
      TH1F *h_hcalout_sim = dynamic_cast<TH1F *>(hm->getHisto(HistName_sim.c_str()));
      assert(h_hcalout_sim);
      h_hcalout_sim->Fill(_tower.hcalout_twr_sim[row+4*col]*1000.0);
    }
    _tower.hcalout_e_sim = hcalout_sum_e_sim;

    auto range_hcalout_lg_raw = TOWER_RAW_LG_HCALOUT->getTowers(); // raw
    for (auto it = range_hcalout_lg_raw.first; it != range_hcalout_lg_raw.second; ++it)
    {
      RawTower *tower = it->second;
      assert(tower);
      // cout << "HCALOUT RAM: ";
      // tower->identify();

      const int col = tower->get_bineta();
      const int row = tower->get_binphi();

      if (col < 0 or col >= 4)
        continue;
      if (row < 0 or row >= 4)
        continue;

      const double energy_raw = tower->get_energy();
      hcalout_sum_lg_e_raw += energy_raw;
      _tower.hcalout_lg_twr_raw[row+4*col] = energy_raw;

      string HistName_raw = Form("h_hcalout_lg_tower_%d_raw",row+4*col);
      TH1F *h_hcalout_lg_raw = dynamic_cast<TH1F *>(hm->getHisto(HistName_raw.c_str()));
      assert(h_hcalout_lg_raw);
      h_hcalout_lg_raw->Fill(_tower.hcalout_lg_twr_raw[row+4*col]*1000.0);
    }
    _tower.hcalout_lg_e_raw = hcalout_sum_lg_e_raw;

    auto range_hcalout_lg_calib = TOWER_CALIB_LG_HCALOUT->getTowers(); // calib
    for (auto it = range_hcalout_lg_calib.first; it != range_hcalout_lg_calib.second; ++it)
    {
      RawTower *tower = it->second;
      assert(tower);

      const int col = tower->get_bineta();
      const int row = tower->get_binphi();

      if (col < 0 or col >= 4)
        continue;
      if (row < 0 or row >= 4)
        continue;

      const double energy_calib = tower->get_energy();
      hcalout_sum_lg_e_calib += energy_calib;
      _tower.hcalout_lg_twr_calib[row+4*col] = energy_calib;

      string HistName_calib = Form("h_hcalout_lg_tower_%d_calib",row+4*col);
      TH1F *h_hcalout_lg_calib = dynamic_cast<TH1F *>(hm->getHisto(HistName_calib.c_str()));
      assert(h_hcalout_lg_calib);
      h_hcalout_lg_calib->Fill(_tower.hcalout_lg_twr_calib[row+4*col]*1000.0);
    }
    _tower.hcalout_lg_e_calib = hcalout_sum_lg_e_calib;
  }

  // process HCALOUT HG
  double hcalout_sum_hg_e_raw = 0;
  double hcalout_sum_hg_e_calib = 0;
  {
    auto range_hcalout_hg_raw = TOWER_RAW_HG_HCALOUT->getTowers(); // raw
    for (auto it = range_hcalout_hg_raw.first; it != range_hcalout_hg_raw.second; ++it)
    {
      RawTower *tower = it->second;
      assert(tower);

      const int col = tower->get_bineta();
      const int row = tower->get_binphi();

      if (col < 0 or col >= 4)
        continue;
      if (row < 0 or row >= 4)
        continue;

      const double energy_raw = tower->get_energy();
      hcalout_sum_hg_e_raw += energy_raw;
      _tower.hcalout_hg_twr_raw[row+4*col] = energy_raw;

      string HistName_raw = Form("h_hcalout_hg_tower_%d_raw",row+4*col);
      TH1F *h_hcalout_hg_raw = dynamic_cast<TH1F *>(hm->getHisto(HistName_raw.c_str()));
      assert(h_hcalout_hg_raw);
      h_hcalout_hg_raw->Fill(_tower.hcalout_hg_twr_raw[row+4*col]*1000.0);
    }
    _tower.hcalout_hg_e_raw = hcalout_sum_hg_e_raw;

    auto range_hcalout_hg_calib = TOWER_CALIB_HG_HCALOUT->getTowers(); // calib
    for (auto it = range_hcalout_hg_calib.first; it != range_hcalout_hg_calib.second; ++it)
    {
      RawTower *tower = it->second;
      assert(tower);

      const int col = tower->get_bineta();
      const int row = tower->get_binphi();

      if (col < 0 or col >= 4)
        continue;
      if (row < 0 or row >= 4)
        continue;

      const double energy_calib = tower->get_energy();
      hcalout_sum_hg_e_calib += energy_calib;
      _tower.hcalout_hg_twr_calib[row+4*col] = energy_calib;

      string HistName_calib = Form("h_hcalout_hg_tower_%d_calib",row+4*col);
      TH1F *h_hcalout_hg_calib = dynamic_cast<TH1F *>(hm->getHisto(HistName_calib.c_str()));
      assert(h_hcalout_hg_calib);
      h_hcalout_hg_calib->Fill(_tower.hcalout_hg_twr_calib[row+4*col]*1000.0);
    }
    _tower.hcalout_hg_e_calib = hcalout_sum_hg_e_calib;
  }

  _tower.hcal_total_sim = hcalin_sum_e_sim + hcalout_sum_e_sim;
  _tower.hcal_total_raw = hcalin_sum_lg_e_raw + hcalout_sum_lg_e_raw;
  _tower.hcal_total_calib = hcalin_sum_lg_e_calib + hcalout_sum_lg_e_calib;

  _tower.hcal_asym_sim = (hcalin_sum_e_sim - hcalout_sum_e_sim)/(hcalin_sum_e_sim + hcalout_sum_e_sim);
  _tower.hcal_asym_raw = (hcalin_sum_lg_e_raw - hcalout_sum_lg_e_raw)/(hcalin_sum_lg_e_raw + hcalout_sum_lg_e_raw);
  _tower.hcal_asym_calib = (hcalin_sum_lg_e_calib - hcalout_sum_lg_e_calib)/(hcalin_sum_lg_e_calib + hcalout_sum_lg_e_calib);


  /*
  TH1F *h_hcal_total_sim = dynamic_cast<TH1F *>(hm->getHisto("h_hcal_total_sim"));
  assert(h_hcal_total_sim);
  h_hcal_total_sim->Fill(_tower.hcal_total_sim*1000.0);
  */

  _eval_run.good_e = good_e;
  _eval_run.good_anti_e = good_anti_e;

  TTree *T = dynamic_cast<TTree *>(hm->getHisto("T"));
  assert(T);
  T->Fill();

  return Fun4AllReturnCodes::EVENT_OK;
}

pair<int, int>
Proto4TowerCalib::find_max(RawTowerContainer *towers, int cluster_size)
{
  const int clus_edge_size = (cluster_size - 1) / 2;
  assert(clus_edge_size >= 0);

  pair<int, int> max(-1000, -1000);
  double max_e = 0;

  for (int col = 0; col < n_size; ++col)
    for (int row = 0; row < n_size; ++row)
    {
      double energy = 0;

      for (int dcol = col - clus_edge_size; dcol <= col + clus_edge_size;
           ++dcol)
        for (int drow = row - clus_edge_size; drow <= row + clus_edge_size;
             ++drow)
        {
          if (dcol < 0 or drow < 0)
            continue;

          RawTower *t = towers->getTower(dcol, drow);
          if (t)
            energy += t->get_energy();
        }

      if (energy > max_e)
      {
        max = make_pair(col, row);
        max_e = energy;
      }
    }

  return max;
}
