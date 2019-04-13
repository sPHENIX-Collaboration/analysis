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

  _mInPut_flag = 1;
  _mStartEvent = -1;
  _mStopEvent = -1;

  setTowerCalibParas();
  _mRunID = "0422";
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

  float histo_range = find_range();
  // cout << "histo_range = " << histo_range << endl;
  // float histo_range = 49.5;

  // SIM HCALIN/HCALOUT
  if(_is_sim)
  {
    TH1F *h_hcalin_tower_sim[16];
    TH1F *h_hcalout_tower_sim[16];
    for(int i_tower = 0; i_tower < 16; ++i_tower)
    {
      string HistName_sim;
      HistName_sim = Form("h_hcalin_tower_%d_sim",i_tower);
      // h_hcalin_tower_sim[i_tower] = new TH1F(HistName_sim.c_str(),HistName_sim.c_str(),100,-0.5,9.5);
      h_hcalin_tower_sim[i_tower] = new TH1F(HistName_sim.c_str(),HistName_sim.c_str(),100,-0.5,histo_range);
      hm->registerHisto(h_hcalin_tower_sim[i_tower]);

      HistName_sim = Form("h_hcalout_tower_%d_sim",i_tower);
      // h_hcalout_tower_sim[i_tower] = new TH1F(HistName_sim.c_str(),HistName_sim.c_str(),100,-0.5,9.5);
      h_hcalout_tower_sim[i_tower] = new TH1F(HistName_sim.c_str(),HistName_sim.c_str(),100,-0.5,histo_range);
      hm->registerHisto(h_hcalout_tower_sim[i_tower]);
    }

    TH2F *h_tower_energy_sim = new TH2F("h_tower_energy_sim","h_tower_energy_sim",100,-1.0,1.0,100,-0.5,histo_range);
    hm->registerHisto(h_tower_energy_sim);
  }

  // HCALIN LG
  TH1F *h_hcalin_lg_tower_raw[16];
  TH1F *h_hcalin_lg_tower_calib[16];
  for(int i_tower = 0; i_tower < 16; ++i_tower)
  {
    string HistName_raw = Form("h_hcalin_lg_tower_%d_raw",i_tower);
    h_hcalin_lg_tower_raw[i_tower] = new TH1F(HistName_raw.c_str(),HistName_raw.c_str(),100,-0.5,histo_range*20.0);
    hm->registerHisto(h_hcalin_lg_tower_raw[i_tower]);

    string HistName_calib = Form("h_hcalin_lg_tower_%d_calib",i_tower);
    h_hcalin_lg_tower_calib[i_tower] = new TH1F(HistName_calib.c_str(),HistName_calib.c_str(),100,-0.5,histo_range);
    hm->registerHisto(h_hcalin_lg_tower_calib[i_tower]);
  }

  // HCALOUT LG
  TH1F *h_hcalout_lg_tower_raw[16];
  TH1F *h_hcalout_lg_tower_calib[16];
  for(int i_tower = 0; i_tower < 16; ++i_tower)
  {
    string HistName_raw = Form("h_hcalout_lg_tower_%d_raw",i_tower);
    h_hcalout_lg_tower_raw[i_tower] = new TH1F(HistName_raw.c_str(),HistName_raw.c_str(),100,-0.5,histo_range*20.0);
    hm->registerHisto(h_hcalout_lg_tower_raw[i_tower]);

    string HistName_calib = Form("h_hcalout_lg_tower_%d_calib",i_tower);
    h_hcalout_lg_tower_calib[i_tower] = new TH1F(HistName_calib.c_str(),HistName_calib.c_str(),100,-0.5,histo_range);
    hm->registerHisto(h_hcalout_lg_tower_calib[i_tower]);
  }

  TH2F *h_tower_energy_raw = new TH2F("h_tower_energy_raw","h_tower_energy_raw",110,-1.1,1.1,100,-0.5,histo_range*640.0);
  hm->registerHisto(h_tower_energy_raw);

  TH2F *h_tower_energy_calib = new TH2F("h_tower_energy_calib","h_tower_energy_calib",110,-1.1,1.1,100,-0.5,histo_range);
  hm->registerHisto(h_tower_energy_calib);

  // HCALOUT HG
  TH1F *h_hcalout_hg_tower_raw[16];
  TH1F *h_hcalout_hg_tower_calib[16];
  for(int i_tower = 0; i_tower < 16; ++i_tower)
  {
    string HistName_raw = Form("h_hcalout_hg_tower_%d_raw",i_tower);
    h_hcalout_hg_tower_raw[i_tower] = new TH1F(HistName_raw.c_str(),HistName_raw.c_str(),100,-0.5,histo_range*20.0);
    hm->registerHisto(h_hcalout_hg_tower_raw[i_tower]);

    string HistName_calib = Form("h_hcalout_hg_tower_%d_calib",i_tower);
    h_hcalout_hg_tower_calib[i_tower] = new TH1F(HistName_calib.c_str(),HistName_calib.c_str(),100,-0.5,histo_range);
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
	const int chanNum = getChannelNumber(row,col);

	if (col < 0 or col >= 4)
	  continue;
	if (row < 0 or row >= 4)
	  continue;

	const double energy_sim = tower->get_energy()/samplefrac_in; // apply samplefrac_in
	hcalin_sum_e_sim += energy_sim;
	_tower.hcalin_twr_sim[chanNum] = energy_sim;

	string HistName_sim = Form("h_hcalin_tower_%d_sim",chanNum);
	TH1F *h_hcalin_sim = dynamic_cast<TH1F *>(hm->getHisto(HistName_sim.c_str()));
	assert(h_hcalin_sim);
	h_hcalin_sim->Fill(_tower.hcalin_twr_sim[chanNum]);
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
	const int chanNum = getChannelNumber(row,col);

	if (col < 0 or col >= 4)
	  continue;
	if (row < 0 or row >= 4)
	  continue;

	const double energy_sim = tower->get_energy()/samplefrac_out; // apply samplefrac_out
	hcalout_sum_e_sim += energy_sim;
	_tower.hcalout_twr_sim[chanNum] = energy_sim;

	string HistName_sim = Form("h_hcalout_tower_%d_sim",chanNum);
	TH1F *h_hcalout_sim = dynamic_cast<TH1F *>(hm->getHisto(HistName_sim.c_str()));
	assert(h_hcalout_sim);
	h_hcalout_sim->Fill(_tower.hcalout_twr_sim[chanNum]);
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
      const int chanNum = getChannelNumber(row,col);

      if (col < 0 or col >= 4)
        continue;
      if (row < 0 or row >= 4)
        continue;

      // const double energy_raw = tower->get_energy()*towercalib_lg_in[chanNum]; // manual calibration
      const double energy_raw = tower->get_energy(); // raw
      hcalin_sum_lg_e_raw += energy_raw;
      _tower.hcalin_lg_twr_raw[chanNum] = energy_raw;

      string HistName_raw = Form("h_hcalin_lg_tower_%d_raw",chanNum);
      TH1F *h_hcalin_lg_raw = dynamic_cast<TH1F *>(hm->getHisto(HistName_raw.c_str()));
      assert(h_hcalin_lg_raw);
      h_hcalin_lg_raw->Fill(_tower.hcalin_lg_twr_raw[chanNum]);
    }
    _tower.hcalin_lg_e_raw = hcalin_sum_lg_e_raw;

    auto range_hcalin_lg_calib = TOWER_CALIB_LG_HCALIN->getTowers(); // calib
    for (auto it = range_hcalin_lg_calib.first; it != range_hcalin_lg_calib.second; ++it)
    {
      RawTower *tower = it->second;
      assert(tower);

      const int col = tower->get_bineta();
      const int row = tower->get_binphi();
      const int chanNum = getChannelNumber(row,col);

      if (col < 0 or col >= 4)
        continue;
      if (row < 0 or row >= 4)
        continue;

      const double energy_calib = tower->get_energy();
      hcalin_sum_lg_e_calib += energy_calib;
      _tower.hcalin_lg_twr_calib[chanNum] = energy_calib;

      string HistName_calib = Form("h_hcalin_lg_tower_%d_calib",chanNum);
      TH1F *h_hcalin_lg_calib = dynamic_cast<TH1F *>(hm->getHisto(HistName_calib.c_str()));
      assert(h_hcalin_lg_calib);
      h_hcalin_lg_calib->Fill(_tower.hcalin_lg_twr_calib[chanNum]);
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
      const int chanNum = getChannelNumber(row,col);

      if (col < 0 or col >= 4)
        continue;
      if (row < 0 or row >= 4)
        continue;

      // const double energy_raw = tower->get_energy()*towercalib_lg_out[chanNum]; // manual calibration
      const double energy_raw = tower->get_energy(); // raw
      hcalout_sum_lg_e_raw += energy_raw;
      _tower.hcalout_lg_twr_raw[chanNum] = energy_raw;

      string HistName_raw = Form("h_hcalout_lg_tower_%d_raw",chanNum);
      TH1F *h_hcalout_lg_raw = dynamic_cast<TH1F *>(hm->getHisto(HistName_raw.c_str()));
      assert(h_hcalout_lg_raw);
      h_hcalout_lg_raw->Fill(_tower.hcalout_lg_twr_raw[chanNum]);
    }
    _tower.hcalout_lg_e_raw = hcalout_sum_lg_e_raw;

    auto range_hcalout_lg_calib = TOWER_CALIB_LG_HCALOUT->getTowers(); // calib
    for (auto it = range_hcalout_lg_calib.first; it != range_hcalout_lg_calib.second; ++it)
    {
      RawTower *tower = it->second;
      assert(tower);

      const int col = tower->get_bineta();
      const int row = tower->get_binphi();
      const int chanNum = getChannelNumber(row,col);

      if (col < 0 or col >= 4)
        continue;
      if (row < 0 or row >= 4)
        continue;

      const double energy_calib = tower->get_energy();
      hcalout_sum_lg_e_calib += energy_calib;
      _tower.hcalout_lg_twr_calib[chanNum] = energy_calib;

      string HistName_calib = Form("h_hcalout_lg_tower_%d_calib",chanNum);
      TH1F *h_hcalout_lg_calib = dynamic_cast<TH1F *>(hm->getHisto(HistName_calib.c_str()));
      assert(h_hcalout_lg_calib);
      h_hcalout_lg_calib->Fill(_tower.hcalout_lg_twr_calib[chanNum]);
    }
    _tower.hcalout_lg_e_calib = hcalout_sum_lg_e_calib;
  }

  _tower.hcal_total_raw = hcalin_sum_lg_e_raw + hcalout_sum_lg_e_raw;
  _tower.hcal_asym_raw = (hcalin_sum_lg_e_raw - hcalout_sum_lg_e_raw)/(hcalin_sum_lg_e_raw + hcalout_sum_lg_e_raw);
  TH2F *h_tower_energy_raw = dynamic_cast<TH2F *>(hm->getHisto("h_tower_energy_raw"));
  assert(h_tower_energy_raw);
  h_tower_energy_raw->Fill(_tower.hcal_asym_raw,_tower.hcal_total_raw); 

  _tower.hcal_total_calib = hcalin_sum_lg_e_calib + hcalout_sum_lg_e_calib;
  _tower.hcal_asym_calib = (hcalin_sum_lg_e_calib - hcalout_sum_lg_e_calib)/(hcalin_sum_lg_e_calib + hcalout_sum_lg_e_calib);
  TH2F *h_tower_energy_calib = dynamic_cast<TH2F *>(hm->getHisto("h_tower_energy_calib"));
  assert(h_tower_energy_calib);
  h_tower_energy_calib->Fill(_tower.hcal_asym_calib,_tower.hcal_total_calib); 

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
      const int chanNum = getChannelNumber(row,col);

      if (col < 0 or col >= 4)
        continue;
      if (row < 0 or row >= 4)
        continue;

      // const double energy_raw = tower->get_energy()*towercalib_hg_out[chanNum]; // manual calibration
      const double energy_raw = tower->get_energy(); // raw
      hcalout_sum_hg_e_raw += energy_raw;
      _tower.hcalout_hg_twr_raw[chanNum] = energy_raw;

      string HistName_raw = Form("h_hcalout_hg_tower_%d_raw",chanNum);
      TH1F *h_hcalout_hg_raw = dynamic_cast<TH1F *>(hm->getHisto(HistName_raw.c_str()));
      assert(h_hcalout_hg_raw);
      h_hcalout_hg_raw->Fill(_tower.hcalout_hg_twr_raw[chanNum]);
    }
    _tower.hcalout_hg_e_raw = hcalout_sum_hg_e_raw;

    auto range_hcalout_hg_calib = TOWER_CALIB_HG_HCALOUT->getTowers(); // calib
    for (auto it = range_hcalout_hg_calib.first; it != range_hcalout_hg_calib.second; ++it)
    {
      RawTower *tower = it->second;
      assert(tower);

      const int col = tower->get_bineta();
      const int row = tower->get_binphi();
      const int chanNum = getChannelNumber(row,col);

      if (col < 0 or col >= 4)
        continue;
      if (row < 0 or row >= 4)
        continue;

      const double energy_calib = tower->get_energy();
      hcalout_sum_hg_e_calib += energy_calib;
      _tower.hcalout_hg_twr_calib[chanNum] = energy_calib;

      string HistName_calib = Form("h_hcalout_hg_tower_%d_calib",chanNum);
      TH1F *h_hcalout_hg_calib = dynamic_cast<TH1F *>(hm->getHisto(HistName_calib.c_str()));
      assert(h_hcalout_hg_calib);
      h_hcalout_hg_calib->Fill(_tower.hcalout_hg_twr_calib[chanNum]);
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
  string inputdir = "/sphenix/user/xusun/TestBeam/ShowerCalib/";
  string InPutList = Form("/direct/phenix+u/xusun/WorkSpace/sPHENIX/analysis/Prototype4/HCal/macros/list/ShowerCalib/Proto4TowerInfo%s_%s.list",_mList.c_str(),_mRunID.c_str());

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

  float histo_range = find_range();

  if(_is_sim)
  {
    // h_mAsymmEnergy_mixed_sim_wo_cut = new TH2F("h_mAsymmEnergy_mixed_sim_wo_cut","h_mAsymmEnergy_mixed_sim_wo_cut",100,-1.0,1.0,100,-0.5,histo_range);
    h_mAsymmEnergy_pion_sim_wo_cut = new TH2F("h_mAsymmEnergy_pion_sim_wo_cut","h_mAsymmEnergy_pion_sim_wo_cut",100,-1.0,1.0,100,-0.5,histo_range);

    // h_mAsymmEnergy_mixed_sim = new TH2F("h_mAsymmEnergy_mixed_sim","h_mAsymmEnergy_mixed_sim",100,-1.0,1.0,100,-0.5,histo_range);
    h_mAsymmEnergy_pion_sim = new TH2F("h_mAsymmEnergy_pion_sim","h_mAsymmEnergy_pion_sim",100,-1.0,1.0,100,-0.5,histo_range);
  }
  if(!_is_sim)
  {
    h_mAsymmAdc_mixed = new TH2F("h_mAsymmAdc_mixed","h_mAsymmAdc_mixed",110,-1.1,1.1,100,-0.5,histo_range*640.0);
    h_mAsymmAdc_electron = new TH2F("h_mAsymmAdc_electron","h_mAsymmAdc_electron",110,-1.1,1.1,100,-0.5,histo_range*640.0);
    h_mAsymmAdc_pion = new TH2F("h_mAsymmAdc_pion","h_mAsymmAdc_pion",110,-1.1,1.1,100,-0.5,histo_range*640.0);

    h_mAsymmEnergy_mixed_wo_cut = new TH2F("h_mAsymmEnergy_mixed_wo_cut","h_mAsymmEnergy_mixed_wo_cut",110,-1.1,1.1,100,-0.5,0.5*histo_range);
    h_mAsymmEnergy_electron_wo_cut = new TH2F("h_mAsymmEnergy_electron_wo_cut","h_mAsymmEnergy_electron_wo_cut",110,-1.1,1.1,100,-0.5,0.5*histo_range);
    h_mAsymmEnergy_pion_wo_cut = new TH2F("h_mAsymmEnergy_pion_wo_cut","h_mAsymmEnergy_pion_wo_cut",110,-1.1,1.1,100,-0.5,0.5*histo_range);

    h_mAsymmEnergy_mixed = new TH2F("h_mAsymmEnergy_mixed","h_mAsymmEnergy_mixed",110,-1.1,1.1,100,-0.5,0.5*histo_range);
    h_mAsymmEnergy_electron = new TH2F("h_mAsymmEnergy_electron","h_mAsymmEnergy_electron",110,-1.1,1.1,100,-0.5,0.5*histo_range);
    h_mAsymmEnergy_pion = new TH2F("h_mAsymmEnergy_pion","h_mAsymmEnergy_pion",110,-1.1,1.1,100,-0.5,0.5*histo_range);

    h_mAsymmEnergy_mixed_balancing = new TH2F("h_mAsymmEnergy_mixed_balancing","h_mAsymmEnergy_mixed_balancing",110,-1.1,1.1,100,-0.5,0.5*histo_range);
    h_mAsymmEnergy_electron_balancing = new TH2F("h_mAsymmEnergy_electron_balancing","h_mAsymmEnergy_electron_balancing",110,-1.1,1.1,100,-0.5,0.5*histo_range);
    h_mAsymmEnergy_pion_balancing = new TH2F("h_mAsymmEnergy_pion_balancing","h_mAsymmEnergy_pion_balancing",110,-1.1,1.1,100,-0.5,0.5*histo_range);

    h_mAsymmEnergy_mixed_leveling = new TH2F("h_mAsymmEnergy_mixed_leveling","h_mAsymmEnergy_mixed_leveling",110,-1.1,1.1,100,-0.5,0.5*histo_range);
    h_mAsymmEnergy_electron_leveling = new TH2F("h_mAsymmEnergy_electron_leveling","h_mAsymmEnergy_electron_leveling",110,-1.1,1.1,100,-0.5,0.5*histo_range);
    h_mAsymmEnergy_pion_leveling = new TH2F("h_mAsymmEnergy_pion_leveling","h_mAsymmEnergy_pion_leveling",110,-1.1,1.1,100,-0.5,0.5*histo_range);

    h_mAsymmEnergy_mixed_ShowerCalib = new TH2F("h_mAsymmEnergy_mixed_ShowerCalib","h_mAsymmEnergy_mixed_ShowerCalib",110,-1.1,1.1,100,-0.5,histo_range+15.0);
    h_mAsymmEnergy_electron_ShowerCalib = new TH2F("h_mAsymmEnergy_electron_ShowerCalib","h_mAsymmEnergy_electron_ShowerCalib",110,-1.1,1.1,100,-0.5,histo_range+15.0);
    h_mAsymmEnergy_pion_ShowerCalib = new TH2F("h_mAsymmEnergy_pion_ShowerCalib","h_mAsymmEnergy_pion_ShowerCalib",110,-1.1,1.1,100,-0.5,histo_range+15.0);

    // Outer HCal only study
    h_mAsymmEnergy_mixed_MIP = new TH2F("h_mAsymmEnergy_mixed_MIP","h_mAsymmEnergy_mixed_MIP",110,-1.1,1.1,100,-0.5,0.5*histo_range);
    h_mEnergyOut_electron = new TH1F("h_mEnergyOut_electron","h_mEnergyOut_electron",100,-0.5,0.5*histo_range);
    h_mEnergyOut_pion = new TH1F("h_mEnergyOut_pion","h_mEnergyOut_pion",100,-0.5,0.5*histo_range);

    h_mEnergyOut_electron_ShowerCalib = new TH1F("h_mEnergyOut_electron_ShowerCalib","h_mEnergyOut_electron_ShowerCalib",100,-0.5,histo_range);
    h_mEnergyOut_pion_ShowerCalib = new TH1F("h_mEnergyOut_pion_ShowerCalib","h_mEnergyOut_pion_ShowerCalib",100,-0.5,histo_range);
  }

  return 0;
}

int Proto4ShowerCalib::MakeAna()
{
  cout << "Make()" << endl;

  // const int mBeamEnergy[6] = {4, 8, 12, 16, 24, 30};
  const float c_in[5]  = {0.905856, 0.943406, 0.964591, 1.0, 0.962649};
  const float c_out[5] = {1.115980, 1.063820, 1.038110, 1.0, 1.040370};
  // const float c_in[5]  = {0.942372, 0.932490, 0.955448, 0.962649, 0.962649};
  // const float c_out[5] = {1.065140, 1.078050, 1.048910, 1.040370, 1.040370};
  const int mEnergyBin = find_energy();
  const float c_in_leveling = c_in[mEnergyBin];
  const float c_out_leveling = c_out[mEnergyBin];
  // cout << "mEnergyBin = " << mEnergyBin << ", c_in = " << c_in[mEnergyBin] << ", c_out = " << c_out[mEnergyBin] << endl;

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
      float energy_sim = _mTower->hcal_total_sim;
      float asymm_sim = _mTower->hcal_asym_sim;
      const float energy_hcalin_sim = _mTower->hcalin_e_sim; // sim
      const float energy_hcalout_sim = _mTower->hcalout_e_sim;

      // h_mAsymmEnergy_mixed_sim_wo_cut->Fill(asymm_sim,energy_sim);
      h_mAsymmEnergy_pion_sim_wo_cut->Fill(asymm_sim,energy_sim);
      if(energy_hcalin_sim > 0.2 && energy_hcalout_sim > 0/2)
      {
	// h_mAsymmEnergy_mixed_sim->Fill(asymm_sim,energy_sim);
	h_mAsymmEnergy_pion_sim->Fill(asymm_sim,energy_sim);
      }
    }
    if(!_is_sim) // beam test data
    {
      const bool good_electron = _mInfo->good_e;
      const bool good_pion = _mInfo->good_anti_e;

      float energy_raw = _mTower->hcal_total_raw; // raw
      float asymm_raw = _mTower->hcal_asym_raw;

      h_mAsymmAdc_mixed->Fill(asymm_raw,energy_raw);
      if(good_electron) h_mAsymmAdc_electron->Fill(asymm_raw,energy_raw);
      if(good_pion) h_mAsymmAdc_pion->Fill(asymm_raw,energy_raw);

      float energy_calib = _mTower->hcal_total_calib; // calib
      float asymm_calib = _mTower->hcal_asym_calib;
      const float energy_hcalin_calib = _mTower->hcalin_lg_e_calib;
      const float energy_hcalout_calib = _mTower->hcalout_lg_e_calib;

      h_mAsymmEnergy_mixed_wo_cut->Fill(asymm_calib,energy_calib);
      if(good_electron) h_mAsymmEnergy_electron_wo_cut->Fill(asymm_calib,energy_calib);
      if(good_pion) h_mAsymmEnergy_pion_wo_cut->Fill(asymm_calib,energy_calib);

      if(energy_hcalin_calib > 0.001 && energy_hcalout_calib > 0.001) // remove ped
      { // extract MIP
	h_mAsymmEnergy_mixed->Fill(asymm_calib,energy_calib);
	if(good_electron) h_mAsymmEnergy_electron->Fill(asymm_calib,energy_calib);
	if(good_pion) h_mAsymmEnergy_pion->Fill(asymm_calib,energy_calib);
      }

      const double MIP_cut = MIP_mean+MIP_width; // 1 sigma MIP energy cut
      if(energy_hcalin_calib <= MIP_cut && energy_hcalout_calib > 0.001 && energy_calib > MIP_cut)
      { // OHCal with MIP event in IHCal
	h_mAsymmEnergy_mixed_MIP->Fill(asymm_calib,energy_calib);
	if(good_electron) h_mEnergyOut_electron->Fill(energy_hcalout_calib);
	if(good_pion) h_mEnergyOut_pion->Fill(energy_hcalout_calib);

	// showercalib
	if(good_electron) h_mEnergyOut_electron_ShowerCalib->Fill(energy_hcalout_calib*showercalib_ohcal);
	if(good_pion) h_mEnergyOut_pion_ShowerCalib->Fill(energy_hcalout_calib*showercalib_ohcal);
      }
      const double MIP_energy_cut = MIP_mean+3.0*MIP_width; // 3 sigma MIP energy cut
      if(energy_hcalin_calib > 0.001 && energy_hcalout_calib > 0.001 && energy_calib > MIP_energy_cut)
      { // balancing without muon
	h_mAsymmEnergy_mixed_balancing->Fill(asymm_calib,energy_calib);
	if(good_electron) h_mAsymmEnergy_electron_balancing->Fill(asymm_calib,energy_calib);
	if(good_pion) h_mAsymmEnergy_pion_balancing->Fill(asymm_calib,energy_calib);

	// apply leveling
	const float energy_leveling = c_in_leveling*energy_hcalin_calib + c_out_leveling*energy_hcalout_calib;
	const float asymm_leveling = (c_in_leveling*energy_hcalin_calib - c_out_leveling*energy_hcalout_calib)/energy_leveling;
	h_mAsymmEnergy_mixed_leveling->Fill(asymm_leveling,energy_leveling);
	if(good_electron) h_mAsymmEnergy_electron_leveling->Fill(asymm_leveling,energy_leveling);
	if(good_pion) h_mAsymmEnergy_pion_leveling->Fill(asymm_leveling,energy_leveling);

	// apply shower calibration
	const float energy_showercalib = showercalib*c_in_leveling*energy_hcalin_calib + showercalib*c_out_leveling*energy_hcalout_calib;
	const float asymm_showercalib = (showercalib*c_in_leveling*energy_hcalin_calib - showercalib*c_out_leveling*energy_hcalout_calib)/energy_showercalib;
	// cout << "energy_leveling = " << energy_leveling << ", energy_showercalib = " << energy_showercalib << endl;
	// cout << "asymm_leveling = " << asymm_leveling << ", asymm_showercalib = " << asymm_showercalib << endl;
	h_mAsymmEnergy_mixed_ShowerCalib->Fill(asymm_showercalib,energy_showercalib);
	if(good_electron) h_mAsymmEnergy_electron_ShowerCalib->Fill(asymm_showercalib,energy_showercalib);
	if(good_pion) h_mAsymmEnergy_pion_ShowerCalib->Fill(asymm_showercalib,energy_showercalib);
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
  if(_is_sim) // beam test simulation
  {
    // h_mAsymmEnergy_mixed_sim_wo_cut->Write();
    h_mAsymmEnergy_pion_sim_wo_cut->Write();
    // h_mAsymmEnergy_mixed_sim->Write();
    h_mAsymmEnergy_pion_sim->Write();
  }
  if(!_is_sim) // beam test data
  {
    h_mAsymmAdc_mixed->Write();
    h_mAsymmAdc_electron->Write();
    h_mAsymmAdc_pion->Write();

    h_mAsymmEnergy_mixed_wo_cut->Write();
    h_mAsymmEnergy_electron_wo_cut->Write();
    h_mAsymmEnergy_pion_wo_cut->Write();

    h_mAsymmEnergy_mixed->Write();
    h_mAsymmEnergy_electron->Write();
    h_mAsymmEnergy_pion->Write();

    h_mAsymmEnergy_mixed_balancing->Write();
    h_mAsymmEnergy_electron_balancing->Write();
    h_mAsymmEnergy_pion_balancing->Write();

    h_mAsymmEnergy_mixed_leveling->Write();
    h_mAsymmEnergy_electron_leveling->Write();
    h_mAsymmEnergy_pion_leveling->Write();

    h_mAsymmEnergy_mixed_ShowerCalib->Write();
    h_mAsymmEnergy_electron_ShowerCalib->Write();
    h_mAsymmEnergy_pion_ShowerCalib->Write();

    h_mAsymmEnergy_mixed_MIP->Write();
    h_mEnergyOut_electron->Write();
    h_mEnergyOut_pion->Write();

    h_mEnergyOut_electron_ShowerCalib->Write();
    h_mEnergyOut_pion_ShowerCalib->Write();
  }
  mFile_OutPut->Close();

  return 1;
}

int Proto4ShowerCalib::getChannelNumber(int row, int column)
{
  if(!_is_sim)
  {
    int hbdchanIHC[4][4] = {{4, 8, 12, 16},
                            {3, 7, 11, 15},
                            {2, 6, 10, 14},
                            {1, 5,  9, 13}};

    return hbdchanIHC[row][column] - 1;
  }

  if(_is_sim)
  {
    int hbdchanIHC[4][4] = {{13,  9, 5, 1},
                            {14, 10, 6, 2},
                            {15, 11, 7, 3},
                            {16, 12, 8, 4}};

    return hbdchanIHC[row][column] - 1;
  }

  return -1;
}

int Proto4ShowerCalib::setTowerCalibParas()
{
  const double energy_in[16] = {0.00763174, 0.00692298, 0.00637355, 0.0059323, 0.00762296, 0.00691832, 0.00636611, 0.0059203, 0.00762873, 0.00693594, 0.00637791, 0.00592433, 0.00762898, 0.00691679, 0.00636373, 0.00592433};
  const double adc_in[16] = {2972.61, 2856.43, 2658.19, 2376.10, 3283.39, 2632.81, 2775.77, 2491.68, 2994.11, 3385.70, 3258.01, 2638.31, 3479.97, 3081.41, 2768.36, 2626.77};
  const double gain_factor_in = 32.0;

  const double energy_out[16] = {0.00668176, 0.00678014, 0.00687082, 0.00706854, 0.00668973, 0.00678279, 0.00684794, 0.00705448, 0.00668976, 0.0068013, 0.00685931, 0.00704985, 0.0066926, 0.00678282, 0.00684403, 0.00704143};
  // const double adc_out[16] = {666.378, 1488.15, 1493.36, 1816.82, 666.378, 1488.15, 1493.36, 1816.82, 666.378, 1488.15, 1493.36, 1816.82, 666.378, 1488.15, 1493.36, 1816.82}; // use 1st column for whole HCALOUT
  const double adc_out[16] = {276.9, 290.0, 280.7, 272.1, 309.5, 304.8, 318.5, 289.6, 289.9, 324.2, 297.9, 294.6, 292.7, 310.5, 302.3, 298.5}; // Songkyo's number
  const double adc_amp[16] = {2.505, 5.330, 5.330, 6.965, 2.505, 5.330, 5.330, 6.965, 2.505, 5.330, 5.330, 6.965, 2.505, 5.330, 5.330, 6.965}; // amplify from 2017 to 2018
  const double gain_factor_out = 16.0;

  // const double tower_in[16] = {3.98902E-05,3.76295E-05,3.75111E-05,3.8131E-05,3.84078E-05,3.66107E-05,4.20291E-05,3.61503E-05,3.67743E-05,3.22359E-05,3.27909E-05,3.89949E-05,3.55663E-05,3.62068E-05,3.55327E-05,3.34781E-05}; // from Songkyo
  // const double tower_out[16] ={0.000144539,0.000136337,0.000129978,0.00013414,0.000135812,0.000120164,0.000123676,0.000119989,0.000133497,0.000128479,0.000116275,0.000128108,0.000131765,0.000126611,0.000121401,0.000126879};

  for(int i_tower = 0; i_tower < 16; ++i_tower)
  {
    towercalib_lg_in[i_tower] = gain_factor_in*energy_in[i_tower]/(adc_in[i_tower]*samplefrac_in);
    towercalib_lg_out[i_tower] = gain_factor_out*energy_out[i_tower]/(adc_amp[i_tower]*adc_out[i_tower]*samplefrac_out);
    towercalib_hg_out[i_tower] = energy_out[i_tower]/(adc_amp[i_tower]*adc_out[i_tower]*samplefrac_out);
    // towercalib_lg_out[i_tower] = gain_factor_out*energy_out[i_tower]/(adc_out[i_tower]*samplefrac_out);
    // towercalib_hg_out[i_tower] = energy_out[i_tower]/(adc_out[i_tower]*samplefrac_out);

    // towercalib_lg_in[i_tower] = tower_in[i_tower]*gain_factor_in;
    // towercalib_lg_out[i_tower] = tower_out[i_tower]*gain_factor_out;
    // towercalib_hg_out[i_tower] = tower_out[i_tower];

    // cout << "i_tower = " << i_tower << ", towercalib_lg_in = " << towercalib_lg_in[i_tower] << ", towercalib_lg_out = " << towercalib_lg_out[i_tower] << endl;
  }

  return 1;
}

float Proto4ShowerCalib::find_range()
{
  float range = 99.5;

  std::string hcal_120GeV[2] = {"0498","0762"};
  std::string hcal_60GeV[2] = {"0820","0821"};
  std::string hcal_12GeV[2] = {"0570","0571"};
  std::string hcal_8GeV[3] = {"0421","0422","1245"};
  std::string hcal_5GeV[3] = {"1087","1089","1091"};

  // cout << "_mRunID = " << _mRunID.c_str() << ", compare = " << _mRunID.compare(hcal_4GeV[0]) << endl;

  for(int i_run = 0; i_run < 2; ++i_run) // 120 GeV
  {
    if(_mRunID.compare(hcal_120GeV[i_run]) == 0) 
    {
      range = 99.5;
      return range;
    }
  }
  for(int i_run = 0; i_run < 2; ++i_run) // 60 GeV
  {
    if(_mRunID.compare(hcal_60GeV[i_run]) == 0) 
    {
      range = 79.5;
      return range;
    }
  }
  for(int i_run = 0; i_run < 2; ++i_run) // 12 GeV
  {
    if(_mRunID.compare(hcal_12GeV[i_run]) == 0)
    {
      range = 24.5;
      return range;
    }
  }
  for(int i_run = 0; i_run < 3; ++i_run) // 8 GeV
  {
    if(_mRunID.compare(hcal_8GeV[i_run]) == 0)
    {
      range = 16.5;
      return range;
    }
  }
  for(int i_run = 0; i_run < 3; ++i_run) // 5 GeV
  {
    if(_mRunID.compare(hcal_5GeV[i_run]) == 0) 
    {
      range = 9.5;
      return range;
    }
  }

  return range;
}

int Proto4ShowerCalib::find_energy()
{
  int energy = -1;

  std::string hcal_120GeV[2] = {"0498","0762"};
  std::string hcal_60GeV[2] = {"0820","0821"};
  std::string hcal_12GeV[2] = {"0570","0571"};
  std::string hcal_8GeV[3] = {"0421","0422","1245"};
  std::string hcal_5GeV[3] = {"1087","1089","1091"};

  // cout << "_mRunID = " << _mRunID.c_str() << ", compare = " << _mRunID.compare(hcal_4GeV[0]) << endl;

  for(int i_run = 0; i_run < 2; ++i_run) // 120 GeV
  {
    if(_mRunID.compare(hcal_120GeV[i_run]) == 0) 
    {
      energy = 4;
      return energy;
    }
  }
  for(int i_run = 0; i_run < 2; ++i_run) // 60 GeV
  {
    if(_mRunID.compare(hcal_60GeV[i_run]) == 0) 
    {
      energy = 3;
      return energy;
    }
  }
  for(int i_run = 0; i_run < 2; ++i_run) // 12 GeV
  {
    if(_mRunID.compare(hcal_12GeV[i_run]) == 0)
    {
      energy = 2;
      return energy;
    }
  }
  for(int i_run = 0; i_run < 3; ++i_run) // 8 GeV
  {
    if(_mRunID.compare(hcal_8GeV[i_run]) == 0)
    {
      energy = 1;
      return energy;
    }
  }
  for(int i_run = 0; i_run < 3; ++i_run) // 5 GeV
  {
    if(_mRunID.compare(hcal_5GeV[i_run]) == 0) 
    {
      energy = 0;
      return energy;
    }
  }

  return energy;
}
