#include "Proto4ShowerCalib.h"

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
#include <TH1F.h>
#include <TH2F.h>
#include <TLorentzVector.h>
#include <TNtuple.h>
#include <TVector3.h>
#include <TChain.h>

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

ClassImp(Proto4ShowerCalib::HCAL_Tower);
ClassImp(Proto4ShowerCalib::Eval_Run);

Proto4ShowerCalib::Proto4ShowerCalib(const std::string &filename)
  : SubsysReco("Proto4ShowerCalib")
  , _is_sim(false)
  , _filename(filename)
  , _ievent(0)
{
  Verbosity(1);

  _eval_run.reset();
  _tower.reset();

  samplefrac_in = 0.09267;
  samplefrac_out = 0.02862;

  for(int i_row  = 0; i_row < 4; ++i_row)
  {
    for(int i_col = 0; i_col < 4; ++i_col)
    {
      int i_tower = i_row + 4*i_col;
      towercalib_in[i_tower] = 1.0;
      towercalib_out[i_tower] = 1.0;
    }
  }

  _mInPut_flag = 1;
  _mStartEvent = -1;
  _mStopEvent = -1;
}

Proto4ShowerCalib::~Proto4ShowerCalib()
{
}

Fun4AllHistoManager *
Proto4ShowerCalib::get_HistoManager()
{
  Fun4AllServer *se = Fun4AllServer::instance();
  Fun4AllHistoManager *hm = se->getHistoManager("Proto4ShowerCalib_HISTOS");

  if (not hm)
  {
    cout
        << "Proto4ShowerCalib::get_HistoManager - Making Fun4AllHistoManager Proto4ShowerCalib_HISTOS"
        << endl;
    hm = new Fun4AllHistoManager("Proto4ShowerCalib_HISTOS");
    se->registerHistoManager(hm);
  }

  assert(hm);

  return hm;
}

int Proto4ShowerCalib::InitRun(PHCompositeNode *topNode)
{
  if (Verbosity())
    cout << "Proto4ShowerCalib::InitRun" << endl;

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

int Proto4ShowerCalib::End(PHCompositeNode *topNode)
{
  cout << "Proto4ShowerCalib::End - write to " << _filename << endl;
  PHTFileServer::get().cd(_filename);

  Fun4AllHistoManager *hm = get_HistoManager();
  assert(hm);
  for (unsigned int i = 0; i < hm->nHistos(); i++)
    hm->getHisto(i)->Write();

  //  if (_T_EMCalTrk)
  //    _T_EMCalTrk->Write();

  return Fun4AllReturnCodes::EVENT_OK;
}

int Proto4ShowerCalib::Init(PHCompositeNode *topNode)
{
  _ievent = 0;

  cout << "Proto4ShowerCalib::get_HistoManager - Making PHTFileServer "
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
  hNormalization->GetXaxis()->SetBinLabel(7, "good_anti_e");
  hNormalization->GetXaxis()->SetTitle("Cuts");
  hNormalization->GetYaxis()->SetTitle("Event count");
  hm->registerHisto(hNormalization);

  hm->registerHisto(new TH1F("hCheck_Veto", "hCheck_Veto", 1000, -500, 1500));
  hm->registerHisto(
      new TH1F("hCheck_Hodo_H", "hCheck_Hodo_H", 1000, -500, 1500));
  hm->registerHisto(
      new TH1F("hCheck_Hodo_V", "hCheck_Hodo_V", 1000, -500, 1500));

  hm->registerHisto(new TH1F("hBeam_Mom", "hBeam_Mom", 1200, -120, 120));

  // SIM HCALIN/HCALOUT
  if(_is_sim)
  {
    TH1F *h_hcalin_tower_sim[16];
    TH1F *h_hcalout_tower_sim[16];
    for(int i_tower = 0; i_tower < 16; ++i_tower)
    {
      string HistName_sim;
      HistName_sim = Form("h_hcalin_tower_%d_sim",i_tower);
      h_hcalin_tower_sim[i_tower] = new TH1F(HistName_sim.c_str(),HistName_sim.c_str(),500,0.5,100.5);
      hm->registerHisto(h_hcalin_tower_sim[i_tower]);

      HistName_sim = Form("h_hcalout_tower_%d_sim",i_tower);
      h_hcalout_tower_sim[i_tower] = new TH1F(HistName_sim.c_str(),HistName_sim.c_str(),500,0.5,100.5);
      hm->registerHisto(h_hcalout_tower_sim[i_tower]);
    }

    TH2F *h_tower_energy_sim = new TH2F("h_tower_energy_sim","h_tower_energy_sim",100,-1.0,1.0,1000,-0.0,99.5);
    hm->registerHisto(h_tower_energy_sim);
  }

  // HCALIN LG
  TH1F *h_hcalin_lg_tower_raw[16];
  TH1F *h_hcalin_lg_tower_calib[16];
  for(int i_tower = 0; i_tower < 16; ++i_tower)
  {
    string HistName_raw = Form("h_hcalin_lg_tower_%d_raw",i_tower);
    h_hcalin_lg_tower_raw[i_tower] = new TH1F(HistName_raw.c_str(),HistName_raw.c_str(),40,0.5,8000.5);
    hm->registerHisto(h_hcalin_lg_tower_raw[i_tower]);

    string HistName_calib = Form("h_hcalin_lg_tower_%d_calib",i_tower);
    h_hcalin_lg_tower_calib[i_tower] = new TH1F(HistName_calib.c_str(),HistName_calib.c_str(),500,0.5,100.5);
    hm->registerHisto(h_hcalin_lg_tower_calib[i_tower]);
  }

  // HCALOUT LG
  TH1F *h_hcalout_lg_tower_raw[16];
  TH1F *h_hcalout_lg_tower_calib[16];
  for(int i_tower = 0; i_tower < 16; ++i_tower)
  {
    string HistName_raw = Form("h_hcalout_lg_tower_%d_raw",i_tower);
    h_hcalout_lg_tower_raw[i_tower] = new TH1F(HistName_raw.c_str(),HistName_raw.c_str(),40,0.5,8000.5);
    hm->registerHisto(h_hcalout_lg_tower_raw[i_tower]);

    string HistName_calib = Form("h_hcalout_lg_tower_%d_calib",i_tower);
    h_hcalout_lg_tower_calib[i_tower] = new TH1F(HistName_calib.c_str(),HistName_calib.c_str(),500,0.5,100.5);
    hm->registerHisto(h_hcalout_lg_tower_calib[i_tower]);
  }

  TH2F *h_tower_energy_raw = new TH2F("h_tower_energy_raw","h_tower_energy_raw",100,-1.0,1.0,1000,0.0,20000.0);
  hm->registerHisto(h_tower_energy_raw);

  TH2F *h_tower_energy_calib = new TH2F("h_tower_energy_calib","h_tower_energy_calib",100,-1.0,1.0,1000,0.0,20000.0);
  hm->registerHisto(h_tower_energy_calib);

  // HCALOUT HG
  TH1F *h_hcalout_hg_tower_raw[16];
  TH1F *h_hcalout_hg_tower_calib[16];
  for(int i_tower = 0; i_tower < 16; ++i_tower)
  {
    string HistName_raw = Form("h_hcalout_hg_tower_%d_raw",i_tower);
    h_hcalout_hg_tower_raw[i_tower] = new TH1F(HistName_raw.c_str(),HistName_raw.c_str(),5000,0.5,5000.5);
    hm->registerHisto(h_hcalout_hg_tower_raw[i_tower]);

    string HistName_calib = Form("h_hcalout_hg_tower_%d_calib",i_tower);
    h_hcalout_hg_tower_calib[i_tower] = new TH1F(HistName_calib.c_str(),HistName_calib.c_str(),500,0.5,100.5);
    hm->registerHisto(h_hcalout_hg_tower_calib[i_tower]);
  }


  // help index files with TChain
  TTree *T = new TTree("HCAL_Info", "HCAL_Info");
  assert(T);
  hm->registerHisto(T);

  T->Branch("info", &_eval_run);
  T->Branch("tower", &_tower);

  return Fun4AllReturnCodes::EVENT_OK;
}

int Proto4ShowerCalib::process_event(PHCompositeNode *topNode)
{
  if (Verbosity() > 2)
    cout << "Proto4ShowerCalib::process_event() entered" << endl;

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

  if (not _is_sim)
  {
    EventHeader *eventheader = findNode::getClass<EventHeader>(topNode,
	"EventHeader");

    if (eventheader->get_EvtType() != DATAEVENT)
    {
      cout << __PRETTY_FUNCTION__
	<< " disgard this event: " << endl;

      eventheader->identify();

      return Fun4AllReturnCodes::DISCARDEVENT;
    }

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

  // HCALIN/HCALOUT SIM information
  RawTowerContainer *TOWER_SIM_HCALIN = findNode::getClass<
      RawTowerContainer>(topNode, "TOWER_SIM_HCALIN");
  if(_is_sim)
  {
    assert(TOWER_SIM_HCALIN);
  }

  RawTowerContainer *TOWER_SIM_HCALOUT = findNode::getClass<
      RawTowerContainer>(topNode, "TOWER_SIM_HCALOUT");
  if(_is_sim)
  {
    assert(TOWER_SIM_HCALOUT);
  }

  // HCAL information
  RawTowerContainer *TOWER_RAW_LG_HCALIN = findNode::getClass<
      RawTowerContainer>(topNode, "TOWER_RAW_LG_HCALIN");
  assert(TOWER_RAW_LG_HCALIN);

  RawTowerContainer *TOWER_RAW_LG_HCALOUT = findNode::getClass<
      RawTowerContainer>(topNode, "TOWER_RAW_LG_HCALOUT");
  assert(TOWER_RAW_LG_HCALOUT);

  RawTowerContainer *TOWER_RAW_HG_HCALOUT = findNode::getClass<
      RawTowerContainer>(topNode, "TOWER_RAW_HG_HCALOUT");
  assert(TOWER_RAW_HG_HCALOUT);

  RawTowerContainer *TOWER_CALIB_LG_HCALIN = findNode::getClass<
      RawTowerContainer>(topNode, "TOWER_CALIB_LG_HCALIN");
  assert(TOWER_CALIB_LG_HCALIN);

  RawTowerContainer *TOWER_CALIB_LG_HCALOUT = findNode::getClass<
      RawTowerContainer>(topNode, "TOWER_CALIB_LG_HCALOUT");
  assert(TOWER_CALIB_LG_HCALOUT);

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

  // process SIM HCALIN/HCALOUT
  if(_is_sim)
  {
    double hcalin_sum_e_sim = 0;
    double hcalout_sum_e_sim = 0;
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
	hcalin_sum_e_sim += energy_sim/samplefrac_in;
	_tower.hcalin_twr_sim[row+4*col] = energy_sim/samplefrac_in;

	string HistName_sim = Form("h_hcalin_tower_%d_sim",row+4*col);
	TH1F *h_hcalin_sim = dynamic_cast<TH1F *>(hm->getHisto(HistName_sim.c_str()));
	assert(h_hcalin_sim);
	h_hcalin_sim->Fill(_tower.hcalin_twr_sim[row+4*col]);
      }
      _tower.hcalin_e_sim = hcalin_sum_e_sim;

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
	hcalout_sum_e_sim += energy_sim/samplefrac_out;
	_tower.hcalout_twr_sim[row+4*col] = energy_sim/samplefrac_out;

	string HistName_sim = Form("h_hcalout_tower_%d_sim",row+4*col);
	TH1F *h_hcalout_sim = dynamic_cast<TH1F *>(hm->getHisto(HistName_sim.c_str()));
	assert(h_hcalout_sim);
	h_hcalout_sim->Fill(_tower.hcalout_twr_sim[row+4*col]);
      }
      _tower.hcalout_e_sim = hcalout_sum_e_sim;
    }
    _tower.hcal_total_sim = hcalin_sum_e_sim + hcalout_sum_e_sim;
    _tower.hcal_asym_sim = (hcalin_sum_e_sim - hcalout_sum_e_sim)/(hcalin_sum_e_sim + hcalout_sum_e_sim);

    TH2F *h_tower_energy_sim = dynamic_cast<TH2F *>(hm->getHisto("h_tower_energy_sim"));
    assert(h_tower_energy_sim);
    h_tower_energy_sim->Fill(_tower.hcal_asym_sim,_tower.hcal_total_sim);
  }

  // process HCALIN LG
  double hcalin_sum_lg_e_raw = 0;
  double hcalin_sum_lg_e_calib = 0;
  {
    auto range_hcalin_lg_raw = TOWER_RAW_LG_HCALIN->getTowers(); // raw
    for (auto it = range_hcalin_lg_raw.first; it != range_hcalin_lg_raw.second; ++it)
    {
      RawTower *tower = it->second;
      assert(tower);
      // cout << "HCALIN RAW: ";
      // tower->identify();

      const int col = tower->get_bineta();
      const int row = tower->get_binphi();

      // RawTower_Prototype4 *tower_pro4 = dynamic_cast<RawTower_Prototype4 *>(TOWER_RAW_LG_HCALIN->getTower(col,row));
      // const int towerid = tower_pro4->get_HBD_channel_number();
      // cout << "towerid = " << towerid << ", col = " << col << ", row = " << row << endl;

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
      h_hcalin_lg_raw->Fill(_tower.hcalin_lg_twr_raw[row+4*col]);
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
      h_hcalin_lg_calib->Fill(_tower.hcalin_lg_twr_calib[row+4*col]);
    }
    _tower.hcalin_lg_e_calib = hcalin_sum_lg_e_calib;
  }

  // process HCALOUT LG
  double hcalout_sum_lg_e_raw = 0;
  double hcalout_sum_lg_e_calib = 0;
  {
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
      h_hcalout_lg_raw->Fill(_tower.hcalout_lg_twr_raw[row+4*col]);
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
      h_hcalout_lg_calib->Fill(_tower.hcalout_lg_twr_calib[row+4*col]);
    }
    _tower.hcalout_lg_e_calib = hcalout_sum_lg_e_calib;
  }

  _tower.hcal_total_raw = hcalin_sum_lg_e_raw + hcalout_sum_lg_e_raw;
  _tower.hcal_asym_raw = (hcalin_sum_lg_e_raw - hcalout_sum_lg_e_raw)/(hcalin_sum_lg_e_raw + hcalout_sum_lg_e_raw);
  TH2F *h_tower_energy_raw = dynamic_cast<TH2F *>(hm->getHisto("h_tower_energy_raw"));
  assert(h_tower_energy_raw);
  h_tower_energy_raw->Fill(_tower.hcal_asym_raw,_tower.hcal_total_raw); 
  // h_tower_energy_raw->Fill(_tower.hcal_asym_raw,_tower.hcal_total_raw*1000.0); // convert to MeV

  _tower.hcal_total_calib = hcalin_sum_lg_e_calib + hcalout_sum_lg_e_calib;
  _tower.hcal_asym_calib = (hcalin_sum_lg_e_calib - hcalout_sum_lg_e_calib)/(hcalin_sum_lg_e_calib + hcalout_sum_lg_e_calib);
  TH2F *h_tower_energy_calib = dynamic_cast<TH2F *>(hm->getHisto("h_tower_energy_calib"));
  assert(h_tower_energy_calib);
  h_tower_energy_calib->Fill(_tower.hcal_asym_calib,_tower.hcal_total_calib); 
  // h_tower_energy_calib->Fill(_tower.hcal_asym_calib,_tower.hcal_total_calib*1000.0); // convert to MeV

  // process HCALOUT HG
  {
    double hcalout_sum_hg_e_raw = 0;
    double hcalout_sum_hg_e_calib = 0;
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
      h_hcalout_hg_raw->Fill(_tower.hcalout_hg_twr_raw[row+4*col]);
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
      h_hcalout_hg_calib->Fill(_tower.hcalout_hg_twr_calib[row+4*col]);
    }
    _tower.hcalout_hg_e_calib = hcalout_sum_hg_e_calib;
  }

  _eval_run.sum_E_HCAL_IN = hcalin_sum_lg_e_calib;
  _eval_run.sum_E_HCAL_OUT = hcalout_sum_lg_e_calib;

  _eval_run.good_e = good_e;
  _eval_run.good_anti_e = good_anti_e;

  TTree *T = dynamic_cast<TTree *>(hm->getHisto("HCAL_Info"));
  assert(T);
  T->Fill();

  return Fun4AllReturnCodes::EVENT_OK;
}

pair<int, int>
Proto4ShowerCalib::find_max(RawTowerContainer *towers, int cluster_size)
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

int Proto4ShowerCalib::InitAna()
{
  if(_is_sim) _mList = "SIM";
  if(!_is_sim) _mList = "RAW";
  string inputdir = "/sphenix/user/xusun/software/data/beam/ShowerCalib/";
  string InPutList = Form("/sphenix/user/xusun/software/data/list/beam/ShowerCalib/%s/Proto4TowerInfo%s.list",_mList.c_str(),_mList.c_str());

  mChainInPut = new TChain("HCAL_Info");

  if (!InPutList.empty())   // if input file is ok
  { 
    cout << "Open input database file list: " << InPutList.c_str() << endl;
    ifstream in(InPutList.c_str());  // input stream
    if(in)
    { 
      cout << "input database file list is ok" << endl;
      char str[255];       // char array for each file name
      Long64_t entries_save = 0;
      while(in)
      { 
	in.getline(str,255);  // take the lines of the file list
	if(str[0] != 0)
	{ 
	  string addfile;
	  addfile = str;
	  addfile = inputdir+addfile;
	  mChainInPut->AddFile(addfile.c_str(),-1,"HCAL_Info");
	  long file_entries = mChainInPut->GetEntries();
	  cout << "File added to data chain: " << addfile.c_str() << " with " << (file_entries-entries_save) << " entries" << endl;
	  entries_save = file_entries;
	}
      }
    }
    else
    { 
      cout << "WARNING: input database file input is problemtic" << endl;
      _mInPut_flag = 0;
    }
  }

  // Set the input tree
  if (_mInPut_flag == 1 && !mChainInPut->GetBranch( "info" ))
  {
    cerr << "ERROR: Could not find branch 'info' in tree!" << endl;
  }

  _mInfo = new Proto4ShowerCalib::Eval_Run();
  _mTower = new Proto4ShowerCalib::HCAL_Tower();
  if(_mInPut_flag == 1)
  {
    mChainInPut->SetBranchAddress("info", &_mInfo);
    mChainInPut->SetBranchAddress("tower", &_mTower);

    long NumOfEvents = (long)mChainInPut->GetEntries();
    cout << "total number of events: " << NumOfEvents << endl;
    _mStartEvent = 0;
    _mStopEvent = NumOfEvents;

    cout << "New nStartEvent = " << _mStartEvent << ", new nStopEvent = " << _mStopEvent << endl;
  }

  if(_is_sim)  // temp solution => will share same Th2Fs
  {
    h_mAsymmEnergy_mixed = new TH2F("h_mAsymmEnergy_mixed","h_mAsymmEnergy_mixed",100,-1.0,1.0,500,0.5,100.5);
    // h_mAsymmEnergy_electron = new TH2F("h_mAsymmEnergy_electron","h_mAsymmEnergy_electron",100,-1.0,1.0,500,0.5,100.5);
    h_mAsymmEnergy_pion_leveling = new TH2F("h_mAsymmEnergy_pion_leveling","h_mAsymmEnergy_pion_leveling",100,-1.0,1.0,500,0.5,100.5);
    h_mAsymmEnergy_pion_scaling = new TH2F("h_mAsymmEnergy_pion_scaling","h_mAsymmEnergy_pion_scaling",100,-1.0,1.0,500,0.5,100.5);
    h_mAsymmEnergy_pion_ShowerCalib = new TH2F("h_mAsymmEnergy_pion_ShowerCalib","h_mAsymmEnergy_pion_ShowerCalib",100,-1.0,1.0,500,0.5,100.5);
  }
  if(!_is_sim) 
  {
    h_mAsymmEnergy_mixed = new TH2F("h_mAsymmEnergy_mixed","h_mAsymmEnergy_mixed",100,-1.0,1.0,500,0.5,20000.5);
    h_mAsymmEnergy_electron = new TH2F("h_mAsymmEnergy_electron","h_mAsymmEnergy_electron",100,-1.0,1.0,500,0.5,20000.5);
    h_mAsymmEnergy_pion_leveling = new TH2F("h_mAsymmEnergy_pion_leveling","h_mAsymmEnergy_pion_leveling",100,-1.0,1.0,500,0.5,20000.5);
    h_mAsymmEnergy_pion_scaling = new TH2F("h_mAsymmEnergy_pion_scaling","h_mAsymmEnergy_pion_scaling",100,-1.0,1.0,500,0.5,20000.5);
    h_mAsymmEnergy_pion_ShowerCalib = new TH2F("h_mAsymmEnergy_pion_ShowerCalib","h_mAsymmEnergy_pion_ShowerCalib",100,-1.0,1.0,500,0.5,20000.5);
  }

  return 0;
}

int Proto4ShowerCalib::MakeAna()
{
  cout << "Make()" << endl;
  unsigned long start_event_use = _mStartEvent;
  unsigned long stop_event_use = _mStopEvent;

  mChainInPut->SetBranchAddress("info", &_mInfo);
  mChainInPut->SetBranchAddress("tower", &_mTower);
  mChainInPut->GetEntry(0);

  for(unsigned long i_event = start_event_use; i_event < stop_event_use; ++i_event)
  // for(unsigned long i_event = 20; i_event < 40; ++i_event)
  {
    if (!mChainInPut->GetEntry( i_event )) // take the event -> information is stored in event
      break;  // end of data chunk

    if (i_event != 0  &&  i_event % 1000 == 0)
      cout << "." << flush;
    if (i_event != 0  &&  i_event % 10000 == 0)
    {
      if((stop_event_use-start_event_use) > 0)
      {
	double event_percent = 100.0*((double)(i_event-start_event_use))/((double)(stop_event_use-start_event_use));
	cout << " " << i_event-start_event_use << " (" << event_percent << "%) " << "\n" << "==> Processing data (ShowerCalib) " << flush;
      }
    }

    if(_is_sim) // beam test simulation
    {
      float energy = _mTower->hcal_total_sim;
      float asymm = _mTower->hcal_asym_sim;

      h_mAsymmEnergy_mixed->Fill(asymm,energy);
      // h_mAsymmEnergy_electron->Fill(asymm,energy);
      h_mAsymmEnergy_pion_leveling->Fill(asymm,energy);
      h_mAsymmEnergy_pion_scaling->Fill(asymm,energy);
      h_mAsymmEnergy_pion_ShowerCalib->Fill(asymm,energy);
    }
    if(!_is_sim) // beam test data
    {
      float energy = _mTower->hcal_total_raw;
      float asymm = _mTower->hcal_asym_raw;

      const bool good_electron = _mInfo->good_e;
      const bool good_pion = _mInfo->good_anti_e;
      const float energy_hcalin = _mTower->hcalin_lg_e_raw;
      const float energy_hcalout = _mTower->hcalout_lg_e_raw;

      h_mAsymmEnergy_mixed->Fill(asymm,energy);
      if(good_electron) h_mAsymmEnergy_electron->Fill(asymm,energy);
      if(good_pion && energy_hcalin > 50.0 && energy_hcalout > 50.0)
      {
	h_mAsymmEnergy_pion_leveling->Fill(asymm,energy);
	h_mAsymmEnergy_pion_scaling->Fill(asymm,energy);
	h_mAsymmEnergy_pion_ShowerCalib->Fill(asymm,energy);
      }
    }
  }

  cout << "." << flush;
  cout << " " << stop_event_use-start_event_use << "(" << 100 << "%)";
  cout << endl;

  return 1;
}

int Proto4ShowerCalib::FinishAna()
{
  cout << "Finish()" << endl;
  mFile_OutPut = new TFile(_filename.c_str(),"RECREATE");
  mFile_OutPut->cd();
  h_mAsymmEnergy_mixed->Write();
  if(!_is_sim) h_mAsymmEnergy_electron->Write();
  h_mAsymmEnergy_pion_leveling->Write();
  h_mAsymmEnergy_pion_scaling->Write();
  h_mAsymmEnergy_pion_ShowerCalib->Write();
  mFile_OutPut->Close();

  return 1;
}
