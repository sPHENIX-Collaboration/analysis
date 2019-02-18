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

ClassImp(Proto4TowerCalib::HCAL_Tower);

Proto4TowerCalib::Proto4TowerCalib(const std::string &filename)
  : SubsysReco("Proto4TowerCalib")
  , _is_sim(true)
  , _filename(filename)
  , _ievent(0)
{
  Verbosity(1);

  _tower.reset();

  _mInPut_flag = 1;
  _mStartEvent = -1;
  _mStopEvent = -1;
  _mDet = "HCALIN";
  _mCol = 0;
  _mPedestal = 100.0;
  reset_pedestal();
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

  //! Envent nomalization
  TH1F *hNormalization = new TH1F("hNormalization", "hNormalization", 1, 0.5, 1.5);
  hNormalization->GetXaxis()->SetBinLabel(1, "ALL");
  hNormalization->GetXaxis()->SetTitle("Cuts");
  hNormalization->GetYaxis()->SetTitle("Event count");
  hm->registerHisto(hNormalization);

  // SIM HCALIN/HCALOUT
  if(_is_sim)
  {
    TH1F *h_hcalin_tower_sim[16];
    TH1F *h_hcalout_tower_sim[16];
    for(int i_tower = 0; i_tower < 16; ++i_tower)
    {
      string HistName_sim;
      HistName_sim = Form("h_hcalin_tower_%d_sim",i_tower);
      h_hcalin_tower_sim[i_tower] = new TH1F(HistName_sim.c_str(),HistName_sim.c_str(),500,-0.5,99.5);
      hm->registerHisto(h_hcalin_tower_sim[i_tower]);

      HistName_sim = Form("h_hcalout_tower_%d_sim",i_tower);
      h_hcalout_tower_sim[i_tower] = new TH1F(HistName_sim.c_str(),HistName_sim.c_str(),500,-0.5,99.5);
      hm->registerHisto(h_hcalout_tower_sim[i_tower]);
    }
  }

  // HCALIN LG
  TH1F *h_hcalin_lg_tower_raw[16];
  TH1F *h_hcalin_lg_tower_calib[16];
  for(int i_tower = 0; i_tower < 16; ++i_tower)
  {
    string HistName_raw = Form("h_hcalin_lg_tower_%d_raw",i_tower);
    h_hcalin_lg_tower_raw[i_tower] = new TH1F(HistName_raw.c_str(),HistName_raw.c_str(),40,0.5,16000.5);
    hm->registerHisto(h_hcalin_lg_tower_raw[i_tower]);

    string HistName_calib = Form("h_hcalin_lg_tower_%d_calib",i_tower);
    h_hcalin_lg_tower_calib[i_tower] = new TH1F(HistName_calib.c_str(),HistName_calib.c_str(),100,-0.5,19.5);
    hm->registerHisto(h_hcalin_lg_tower_calib[i_tower]);
  }

  // HCALOUT LG
  TH1F *h_hcalout_lg_tower_raw[16];
  TH1F *h_hcalout_lg_tower_calib[16];
  for(int i_tower = 0; i_tower < 16; ++i_tower)
  {
    string HistName_raw = Form("h_hcalout_lg_tower_%d_raw",i_tower);
    h_hcalout_lg_tower_raw[i_tower] = new TH1F(HistName_raw.c_str(),HistName_raw.c_str(),40,0.5,16000.5);
    hm->registerHisto(h_hcalout_lg_tower_raw[i_tower]);

    string HistName_calib = Form("h_hcalout_lg_tower_%d_calib",i_tower);
    h_hcalout_lg_tower_calib[i_tower] = new TH1F(HistName_calib.c_str(),HistName_calib.c_str(),100,-0.5,19.5);
    hm->registerHisto(h_hcalout_lg_tower_calib[i_tower]);
  }

  // HCALOUT HG
  TH1F *h_hcalout_hg_tower_raw[16];
  TH1F *h_hcalout_hg_tower_calib[16];
  for(int i_tower = 0; i_tower < 16; ++i_tower)
  {
    string HistName_raw = Form("h_hcalout_hg_tower_%d_raw",i_tower);
    h_hcalout_hg_tower_raw[i_tower] = new TH1F(HistName_raw.c_str(),HistName_raw.c_str(),40,0.5,16000.5);
    hm->registerHisto(h_hcalout_hg_tower_raw[i_tower]);

    string HistName_calib = Form("h_hcalout_hg_tower_%d_calib",i_tower);
    h_hcalout_hg_tower_calib[i_tower] = new TH1F(HistName_calib.c_str(),HistName_calib.c_str(),100,-0.5,19.5);
    hm->registerHisto(h_hcalout_hg_tower_calib[i_tower]);
  }

  // help index files with TChain
  TTree *T = new TTree("HCAL_Info", "HCAL_Info");
  assert(T);
  hm->registerHisto(T);

  T->Branch("TowerCalib", &_tower);

  return Fun4AllReturnCodes::EVENT_OK;
}

int Proto4TowerCalib::process_event(PHCompositeNode *topNode)
{
  if (Verbosity() > 2)
    cout << "Proto4TowerCalib::process_event() entered" << endl;

  // init eval objects
  _tower.reset();

  Fun4AllHistoManager *hm = get_HistoManager();
  assert(hm);

  // EventHeader *eventheader = findNode::getClass<EventHeader>(topNode, "EventHeader");

  /*
  if (eventheader->get_EvtType() != DATAEVENT)
  {
    cout << __PRETTY_FUNCTION__
         << " disgard this event: " << endl;

    eventheader->identify();

    return Fun4AllReturnCodes::DISCARDEVENT;
  }
  */

  if (_is_sim)
  {
    PHG4TruthInfoContainer *truthInfoList = findNode::getClass<
        PHG4TruthInfoContainer>(topNode, "G4TruthInfo");

    assert(truthInfoList);


    const PHG4Particle *p =
        truthInfoList->GetPrimaryParticleRange().first->second;
    assert(p);

    const PHG4VtxPoint *v = truthInfoList->GetVtx(p->get_vtx_id());
    assert(v);
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

  // HCALIN LG information
  RawTowerContainer *TOWER_RAW_LG_HCALIN = findNode::getClass<
      RawTowerContainer>(topNode, "TOWER_RAW_LG_HCALIN");
  assert(TOWER_RAW_LG_HCALIN);

  RawTowerContainer *TOWER_CALIB_LG_HCALIN = findNode::getClass<
      RawTowerContainer>(topNode, "TOWER_CALIB_LG_HCALIN");
  assert(TOWER_CALIB_LG_HCALIN);

  // HCALOUT LG information
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

	const int col = tower->get_bineta();
	const int row = tower->get_binphi();
	const int chanNum = getChannelNumber(row,col);

	// cout << "HCALIN SIM: ";
	// tower->identify();
	// cout << "getSimChannelNumber = " << chanNum << endl;

	if (col < 0 or col >= 4)
	  continue;
	if (row < 0 or row >= 4)
	  continue;

	const double energy_sim = tower->get_energy();
	hcalin_sum_e_sim += energy_sim;
	_tower.hcalin_twr_sim[chanNum] = energy_sim;

	string HistName_sim = Form("h_hcalin_tower_%d_sim",chanNum);
	TH1F *h_hcalin_sim = dynamic_cast<TH1F *>(hm->getHisto(HistName_sim.c_str()));
	assert(h_hcalin_sim);
	h_hcalin_sim->Fill(_tower.hcalin_twr_sim[chanNum]*1000.0); // convert to MeV
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

	const double energy_sim = tower->get_energy();
	hcalout_sum_e_sim += energy_sim;
	_tower.hcalout_twr_sim[chanNum] = energy_sim;

	string HistName_sim = Form("h_hcalout_tower_%d_sim",chanNum);
	TH1F *h_hcalout_sim = dynamic_cast<TH1F *>(hm->getHisto(HistName_sim.c_str()));
	assert(h_hcalout_sim);
	h_hcalout_sim->Fill(_tower.hcalout_twr_sim[chanNum]*1000.0); // convert to MeV
      }
      _tower.hcalout_e_sim = hcalout_sum_e_sim;
    }
    _tower.hcal_total_sim = hcalin_sum_e_sim + hcalout_sum_e_sim;
    _tower.hcal_asym_sim = (hcalin_sum_e_sim - hcalout_sum_e_sim)/(hcalin_sum_e_sim + hcalout_sum_e_sim);
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
      const int col = tower->get_bineta();
      const int row = tower->get_binphi();
      const int chanNum = getChannelNumber(row,col);

      // cout << "HCALIN RAW: ";
      // tower->identify();
      // cout << "getDataChannelNumber = " << chanNum << endl;

      if (col < 0 or col >= 4)
        continue;
      if (row < 0 or row >= 4)
        continue;

      const double energy_raw = tower->get_energy();
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

      const int col = tower->get_bineta();
      const int row = tower->get_binphi();
      const int chanNum = getChannelNumber(row,col);

      // cout << "HCALOUT LG RAW: ";
      // tower->identify();
      // cout << "getDataChannelNumber = " << chanNum << endl;

      if (col < 0 or col >= 4)
        continue;
      if (row < 0 or row >= 4)
        continue;

      const double energy_raw = tower->get_energy();
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
      const int chanNum = getChannelNumber(row,col);

      // cout << "HCALOUT HG RAW: ";
      // tower->identify();
      // cout << "getDataChannelNumber = " << chanNum << endl;

      if (col < 0 or col >= 4)
        continue;
      if (row < 0 or row >= 4)
        continue;

      const double energy_raw = tower->get_energy();
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

  _tower.hcal_total_raw = hcalin_sum_lg_e_raw + hcalout_sum_lg_e_raw;
  _tower.hcal_total_calib = hcalin_sum_lg_e_calib + hcalout_sum_lg_e_calib;

  _tower.hcal_asym_raw = (hcalin_sum_lg_e_raw - hcalout_sum_lg_e_raw)/(hcalin_sum_lg_e_raw + hcalout_sum_lg_e_raw);
  _tower.hcal_asym_calib = (hcalin_sum_lg_e_calib - hcalout_sum_lg_e_calib)/(hcalin_sum_lg_e_calib + hcalout_sum_lg_e_calib);

  TTree *T = dynamic_cast<TTree *>(hm->getHisto("HCAL_Info"));
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

int Proto4TowerCalib::getChannelNumber(int row, int column)
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

int Proto4TowerCalib::InitAna()
{
  if(_is_sim) _mList = "SIM";
  if(!_is_sim) _mList = "RAW";
  string inputdir = "/sphenix/user/xusun/TestBeam/TowerCalib/";
  string InPutList = Form("/direct/phenix+u/xusun/WorkSpace/sPHENIX/analysis/Prototype4/HCal/macros/list/Proto4TowerInfo%s_%s_%d.list",_mList.c_str(),_mDet.c_str(),_mCol);

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
  if (_mInPut_flag == 1 && !mChainInPut->GetBranch( "TowerCalib" ))
  {
    cerr << "ERROR: Could not find branch 'HCAL_Tower' in tree!" << endl;
  }

  _mTower = new Proto4TowerCalib::HCAL_Tower();
  if(_mInPut_flag == 1)
  {
    mChainInPut->SetBranchAddress("TowerCalib", &_mTower);

    long NumOfEvents = (long)mChainInPut->GetEntries();
    cout << "total number of events: " << NumOfEvents << endl;
    _mStartEvent = 0;
    _mStopEvent = NumOfEvents;
  }

  // initialize TH1Fs for energy/adc spectra
  for(int i_tower = 0; i_tower < 16; ++i_tower)
  {
    std::string HistName = Form("h_m%s_%s_twr_%d",_mDet.c_str(),_mList.c_str(),i_tower);
    if(_is_sim) h_mHCAL[i_tower] = new TH1F(HistName.c_str(),HistName.c_str(),500,0.5,100.5);
    if(!_is_sim) h_mHCAL[i_tower] = new TH1F(HistName.c_str(),HistName.c_str(),80,0.5,16000.5);
    // if(!_is_sim) h_mHCAL[i_tower] = new TH1F(HistName.c_str(),HistName.c_str(),500,0.5,100.5);
  }

  return 0;
}

int Proto4TowerCalib::MakeAna()
{
  cout << "Make()" << endl;
  unsigned long start_event_use = _mStartEvent;
  unsigned long stop_event_use = _mStopEvent;

  mChainInPut->SetBranchAddress("TowerCalib", &_mTower);
  mChainInPut->GetEntry(0);

  for(unsigned long i_event = start_event_use; i_event < stop_event_use; ++i_event)
  // for(unsigned long i_event = 20; i_event < 40; ++i_event)
  {
    if (!mChainInPut->GetEntry( i_event )) // take the event -> information is stored in event
      break;  // end of data chunk

    if(_is_sim) // cosmic simulation
    {
      for(int i_tower = 0; i_tower < 16; ++i_tower)
      {
	if(_mDet == "HCALIN")  // HCALIN
	{
	  hcal_twr[i_tower] = _mTower->hcalin_twr_sim[i_tower];
	  h_mHCAL[i_tower]->Fill(hcal_twr[i_tower]*1000.0); // convert to MeV
	}
	if(_mDet == "HCALOUT")  // HCALOUT
	{
	  hcal_twr[i_tower] = _mTower->hcalout_twr_sim[i_tower];
	  h_mHCAL[i_tower]->Fill(hcal_twr[i_tower]*1000.0); // convert to MeV
	}
      }
    }
    if(!_is_sim) // cosmic data
    {
      for(int i_tower = 0; i_tower < 16; ++i_tower) // 1st loop to decide pedestal
      {
	if(_mDet == "HCALIN") // HCALIN 
	{
	  hcal_twr[i_tower] = _mTower->hcalin_lg_twr_raw[i_tower];
	  if( hcal_twr[i_tower] > _mPedestal ) _is_sig[i_tower] = true;
	}
	if(_mDet == "HCALOUT") // HCALOUT
	{
	  hcal_twr[i_tower] = _mTower->hcalout_hg_twr_raw[i_tower];
	  if(hcal_twr[i_tower] > _mPedestal) _is_sig[i_tower] = true;
	}
      }
      if( is_sig(_mCol) ) 
      {
	// cout << "i_event = " << i_event << ", is_sig " << is_sig(_mCol) << endl;
	fill_sig(_mCol);
      }
      reset_pedestal();
    }
  }

  return 1;
}

int Proto4TowerCalib::FinishAna()
{
  cout << "Finish()" << endl;
  mFile_OutPut = new TFile(_filename.c_str(),"RECREATE");
  mFile_OutPut->cd();
  for(int i_tower = 0; i_tower < 16; ++i_tower)
  {
    h_mHCAL[i_tower]->Write();
  }
  mFile_OutPut->Close();

  return 1;
}

bool Proto4TowerCalib::is_sig(int colID)
{
  int twr_0 = getChannelNumber(0,colID);
  int twr_1 = getChannelNumber(1,colID);
  int twr_2 = getChannelNumber(2,colID);
  int twr_3 = getChannelNumber(3,colID);

  if(_is_sig[twr_0] && _is_sig[twr_1] && _is_sig[twr_2]) return true;
  if(_is_sig[twr_0] && _is_sig[twr_1] && _is_sig[twr_3]) return true;
  if(_is_sig[twr_0] && _is_sig[twr_2] && _is_sig[twr_3]) return true;
  if(_is_sig[twr_1] && _is_sig[twr_2] && _is_sig[twr_3]) return true;

  return false;
}

int Proto4TowerCalib::fill_sig(int colID)
{
  for(int i_row = 0; i_row < 4; ++i_row)
  {
    // fill adc spectra for cosmic
    int i_tower = getChannelNumber(i_row,colID);
    h_mHCAL[i_tower]->Fill(hcal_twr[i_tower]);
  }

  return 1;
}

int Proto4TowerCalib::reset_pedestal()
{
  for(int i_tower = 0; i_tower < 16; ++i_tower) 
  {
    hcal_twr[i_tower] = -1.0; // set tower energy
    _is_sig[i_tower] = false; // set pedestal cut 
  }

  return 1;
}
