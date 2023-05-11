#include "Proto2ShowerCalib.h"
#include "TemperatureCorrection.h"

#include <prototype2/RawTower_Temperature.h>
#include <prototype2/RawTower_Prototype2.h>
#include <calobase/RawTowerContainer.h>
#include <pdbcalbase/PdbParameterMap.h>
#include <phparameter/PHParameters.h>
#include <ffaobjects/EventHeader.h>

#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/PHTFileServer.h>
#include <phool/PHCompositeNode.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/getClass.h>
#include <fun4all/Fun4AllHistoManager.h>

#include <phool/PHCompositeNode.h>

#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4VtxPoint.h>

#include <TNtuple.h>
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TVector3.h>
#include <TLorentzVector.h>

#include <exception>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <vector>
#include <set>
#include <algorithm>
#include <cassert>
#include <cmath>

using namespace std;

ClassImp(Proto2ShowerCalib::Eval_Cluster);
ClassImp(Proto2ShowerCalib::Eval_Run);

Proto2ShowerCalib::Proto2ShowerCalib(const std::string &filename) :
    SubsysReco("Proto2ShowerCalib"), _is_sim(false), _filename(filename), _ievent(
        0)
{

  verbosity = 1;

  _eval_run.reset();
  _eval_3x3_raw.reset();
  _eval_5x5_raw.reset();
  _eval_3x3_prod.reset();
  _eval_5x5_prod.reset();
  _eval_3x3_temp.reset();
  _eval_5x5_temp.reset();
  _eval_3x3_recalib.reset();
  _eval_5x5_recalib.reset();

  for (int col = 0; col < n_size; ++col)
    for (int row = 0; row < n_size; ++row)
      {
        _recalib_const[make_pair(col, row)] = 0;
      }

}

Proto2ShowerCalib::~Proto2ShowerCalib()
{
}

Fun4AllHistoManager *
Proto2ShowerCalib::get_HistoManager()
{

  Fun4AllServer *se = Fun4AllServer::instance();
  Fun4AllHistoManager *hm = se->getHistoManager("Proto2ShowerCalib_HISTOS");

  if (not hm)
    {
      cout
          << "Proto2ShowerCalib::get_HistoManager - Making Fun4AllHistoManager Proto2ShowerCalib_HISTOS"
          << endl;
      hm = new Fun4AllHistoManager("Proto2ShowerCalib_HISTOS");
      se->registerHistoManager(hm);
    }

  assert(hm);

  return hm;
}

int
Proto2ShowerCalib::InitRun(PHCompositeNode *topNode)
{
  if (verbosity)
    cout << "Proto2ShowerCalib::InitRun" << endl;

  _ievent = 0;

  PHNodeIterator iter(topNode);
  PHCompositeNode *dstNode = static_cast<PHCompositeNode*>(iter.findFirst(
      "PHCompositeNode", "DST"));
  if (!dstNode)
    {
      std::cerr << PHWHERE << "DST Node missing, doing nothing." << std::endl;
      throw runtime_error(
          "Failed to find DST node in EmcRawTowerBuilder::CreateNodes");
    }

  return Fun4AllReturnCodes::EVENT_OK;
}

int
Proto2ShowerCalib::End(PHCompositeNode *topNode)
{
  cout << "Proto2ShowerCalib::End - write to " << _filename << endl;
  PHTFileServer::get().cd(_filename);

  Fun4AllHistoManager *hm = get_HistoManager();
  assert(hm);
  for (unsigned int i = 0; i < hm->nHistos(); i++)
    hm->getHisto(i)->Write();

//  if (_T_EMCalTrk)
//    _T_EMCalTrk->Write();

  fdata.close();

  return Fun4AllReturnCodes::EVENT_OK;
}

int
Proto2ShowerCalib::Init(PHCompositeNode *topNode)
{

  _ievent = 0;

  cout << "Proto2ShowerCalib::get_HistoManager - Making PHTFileServer "
      << _filename << endl;
  PHTFileServer::get().open(_filename, "RECREATE");

  fdata.open(_filename + ".dat", std::fstream::out);

  Fun4AllHistoManager *hm = get_HistoManager();
  assert(hm);

  TH2F * hCheck_Cherenkov = new TH2F("hCheck_Cherenkov", "hCheck_Cherenkov",
      1000, -2000, 2000, 5, .5, 5.5);
  hCheck_Cherenkov->GetYaxis()->SetBinLabel(1, "C1");
  hCheck_Cherenkov->GetYaxis()->SetBinLabel(2, "C2 in");
  hCheck_Cherenkov->GetYaxis()->SetBinLabel(3, "C2 out");
  hCheck_Cherenkov->GetYaxis()->SetBinLabel(4, "C2 sum");
  hm->registerHisto(hCheck_Cherenkov);

  TH1F * hNormalization = new TH1F("hNormalization", "hNormalization", 10, .5,
      10.5);
  hCheck_Cherenkov->GetXaxis()->SetBinLabel(1, "ALL");
  hCheck_Cherenkov->GetXaxis()->SetBinLabel(2, "C2-e");
  hCheck_Cherenkov->GetXaxis()->SetBinLabel(3, "trigger_veto_pass");
  hCheck_Cherenkov->GetXaxis()->SetBinLabel(4, "valid_hodo_h");
  hCheck_Cherenkov->GetXaxis()->SetBinLabel(5, "valid_hodo_v");
  hCheck_Cherenkov->GetXaxis()->SetBinLabel(6, "good_e");
  hCheck_Cherenkov->GetXaxis()->SetBinLabel(7, "good_data");
  hm->registerHisto(hNormalization);

  hm->registerHisto(new TH1F("hCheck_Veto", "hCheck_Veto", 1000, -500, 500));
  hm->registerHisto(
      new TH1F("hCheck_Hodo_H", "hCheck_Hodo_H", 1000, -500, 500));
  hm->registerHisto(
      new TH1F("hCheck_Hodo_V", "hCheck_Hodo_V", 1000, -500, 500));

  hm->registerHisto(new TH1F("hBeam_Mom", "hBeam_Mom", 1200, -120, 120));

  hm->registerHisto(new TH2F("hEoP", "hEoP", 1000, 0, 1.5, 120, .5, 120.5));

  hm->registerHisto(new TH1F("hTemperature", "hTemperature", 500, 0, 50));

  // help index files with TChain
  TTree * T = new TTree("T", "T");
  assert(T);
  hm->registerHisto(T);

  T->Branch("info", &_eval_run);
  T->Branch("clus_3x3_raw", &_eval_3x3_raw);
  T->Branch("clus_5x5_raw", &_eval_5x5_raw);
  T->Branch("clus_3x3_prod", &_eval_3x3_prod);
  T->Branch("clus_5x5_prod", &_eval_5x5_prod);
  T->Branch("clus_3x3_temp", &_eval_3x3_temp);
  T->Branch("clus_5x5_temp", &_eval_5x5_temp);
  T->Branch("clus_3x3_recalib", &_eval_3x3_recalib);
  T->Branch("clus_5x5_recalib", &_eval_5x5_recalib);

  return Fun4AllReturnCodes::EVENT_OK;
}

int
Proto2ShowerCalib::process_event(PHCompositeNode *topNode)
{

  if (verbosity > 2)
    cout << "Proto2ShowerCalib::process_event() entered" << endl;

  // init eval objects
  _eval_run.reset();
  _eval_3x3_raw.reset();
  _eval_5x5_raw.reset();
  _eval_3x3_prod.reset();
  _eval_5x5_prod.reset();
  _eval_3x3_temp.reset();
  _eval_5x5_temp.reset();
  _eval_3x3_recalib.reset();
  _eval_5x5_recalib.reset();

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

      TH1F * hBeam_Mom = dynamic_cast<TH1F *>(hm->getHisto("hBeam_Mom"));
      assert(hBeam_Mom);

      hBeam_Mom->Fill(_eval_run.beam_mom);
    }

  EventHeader* eventheader = findNode::getClass<EventHeader>(topNode,
      "EventHeader");
  if (not _is_sim)
    {
      assert(eventheader);

      _eval_run.run = eventheader->get_RunNumber();
      if (verbosity > 4)
        cout << __PRETTY_FUNCTION__ << _eval_run.run << endl;

      _eval_run.event = eventheader->get_EvtSequence();
    }

  if (_is_sim)
    {

      PHG4TruthInfoContainer* truthInfoList = findNode::getClass<
          PHG4TruthInfoContainer>(topNode, "G4TruthInfo");

      assert(truthInfoList);

      _eval_run.run = -1;

      const PHG4Particle * p = truthInfoList->GetPrimaryParticleRange().first->second;
      assert(p);

      const PHG4VtxPoint * v = truthInfoList->GetVtx(p->get_vtx_id());
      assert(v);

      _eval_run.beam_mom = sqrt(
          p->get_px() * p->get_px() + p->get_py() * p->get_py()
              + p->get_pz() * p->get_pz());
      _eval_run.truth_y = v->get_y();
      _eval_run.truth_z = v->get_z();

    }

  // normalization
  TH1F * hNormalization = dynamic_cast<TH1F *>(hm->getHisto("hNormalization"));
  assert(hNormalization);

  hNormalization->Fill("ALL", 1);

  RawTowerContainer* TOWER_RAW_CEMC = findNode::getClass<RawTowerContainer>(
      topNode, _is_sim ? "TOWER_RAW_LG_CEMC" : "TOWER_RAW_CEMC");
  assert(TOWER_RAW_CEMC);
  RawTowerContainer* TOWER_CALIB_CEMC = findNode::getClass<RawTowerContainer>(
      topNode, _is_sim ? "TOWER_CALIB_LG_CEMC" : "TOWER_CALIB_CEMC");
  assert(TOWER_CALIB_CEMC);

  // other nodes
  RawTowerContainer* TOWER_CALIB_TRIGGER_VETO = findNode::getClass<
      RawTowerContainer>(topNode, "TOWER_CALIB_TRIGGER_VETO");
  if (not _is_sim)
    {
      assert(TOWER_CALIB_TRIGGER_VETO);
    }

  RawTowerContainer* TOWER_CALIB_HODO_HORIZONTAL = findNode::getClass<
      RawTowerContainer>(topNode, "TOWER_CALIB_HODO_HORIZONTAL");
  if (not _is_sim)
    {
      assert(TOWER_CALIB_HODO_HORIZONTAL);
    }
  RawTowerContainer* TOWER_CALIB_HODO_VERTICAL = findNode::getClass<
      RawTowerContainer>(topNode, "TOWER_CALIB_HODO_VERTICAL");
  if (not _is_sim)
    {
      assert(TOWER_CALIB_HODO_VERTICAL);
    }

  RawTowerContainer* TOWER_TEMPERATURE_EMCAL = findNode::getClass<
      RawTowerContainer>(topNode, "TOWER_TEMPERATURE_EMCAL");
  if (not _is_sim)
    {
      assert(TOWER_TEMPERATURE_EMCAL);
    }

  RawTowerContainer* TOWER_CALIB_C1 = findNode::getClass<RawTowerContainer>(
      topNode, "TOWER_CALIB_C1");
  if (not _is_sim)
    {
      assert(TOWER_CALIB_C1);
    }
  RawTowerContainer* TOWER_CALIB_C2 = findNode::getClass<RawTowerContainer>(
      topNode, "TOWER_CALIB_C2");
  if (not _is_sim)
    {
      assert(TOWER_CALIB_C2);
    }

  // Cherenkov
  bool cherekov_e = false;
  if (not _is_sim)
    {
      RawTower * t_c2_in = NULL;
      RawTower * t_c2_out = NULL;

      assert(eventheader);
      if (eventheader->get_RunNumber() >= 2105)
        {
          t_c2_in = TOWER_CALIB_C2->getTower(10);
          t_c2_out = TOWER_CALIB_C2->getTower(11);
        }
      else
        {
          t_c2_in = TOWER_CALIB_C2->getTower(0);
          t_c2_out = TOWER_CALIB_C2->getTower(1);
        }
      assert(t_c2_in);
      assert(t_c2_out);

      const double c2_in = t_c2_in->get_energy();
      const double c2_out = t_c2_out->get_energy();
      const double c1 = TOWER_CALIB_C1->getTower(0)->get_energy();

      _eval_run.C2_sum = c2_in + c2_out;
      cherekov_e = (_eval_run.C2_sum) > 100;
      hNormalization->Fill("C2-e", cherekov_e);

      TH2F * hCheck_Cherenkov = dynamic_cast<TH2F *>(hm->getHisto(
          "hCheck_Cherenkov"));
      assert(hCheck_Cherenkov);
      hCheck_Cherenkov->Fill(c1, "C1", 1);
      hCheck_Cherenkov->Fill(c2_in, "C2 in", 1);
      hCheck_Cherenkov->Fill(c2_out, "C2 out", 1);
      hCheck_Cherenkov->Fill(c2_in + c2_out, "C2 sum", 1);
    }

  // veto
  TH1F * hCheck_Veto = dynamic_cast<TH1F *>(hm->getHisto("hCheck_Veto"));
  assert(hCheck_Veto);
  bool trigger_veto_pass = true;
  if (not _is_sim)
    {
      auto range = TOWER_CALIB_TRIGGER_VETO->getTowers();
      for (auto it = range.first; it != range.second; ++it)
        {
          RawTower* tower = it->second;
          assert(tower);

          hCheck_Veto->Fill(tower->get_energy());

          if (abs(tower->get_energy()) > 15)
            trigger_veto_pass = false;
        }
    }
  hNormalization->Fill("trigger_veto_pass", trigger_veto_pass);
  _eval_run.trigger_veto_pass = trigger_veto_pass;

  // hodoscope
  TH1F * hCheck_Hodo_H = dynamic_cast<TH1F *>(hm->getHisto("hCheck_Hodo_H"));
  assert(hCheck_Hodo_H);
  int hodo_h_count = 0;
  if (not _is_sim)
    {
      auto range = TOWER_CALIB_HODO_HORIZONTAL->getTowers();
      for (auto it = range.first; it != range.second; ++it)
        {
          RawTower* tower = it->second;
          assert(tower);

          hCheck_Hodo_H->Fill(tower->get_energy());

          if (abs(tower->get_energy()) > 30)
            {
              hodo_h_count++;
              _eval_run.hodo_h = tower->get_id();
            }
        }
    }
  const bool valid_hodo_h = hodo_h_count == 1;
  hNormalization->Fill("valid_hodo_h", valid_hodo_h);
  _eval_run.valid_hodo_h = valid_hodo_h;

  TH1F * hCheck_Hodo_V = dynamic_cast<TH1F *>(hm->getHisto("hCheck_Hodo_V"));
  assert(hCheck_Hodo_V);
  int hodo_v_count = 0;
  if (not _is_sim)
    {
      auto range = TOWER_CALIB_HODO_VERTICAL->getTowers();
      for (auto it = range.first; it != range.second; ++it)
        {
          RawTower* tower = it->second;
          assert(tower);

          hCheck_Hodo_V->Fill(tower->get_energy());

          if (abs(tower->get_energy()) > 30)
            {
              hodo_v_count++;
              _eval_run.hodo_v = tower->get_id();
            }
        }
    }
  const bool valid_hodo_v = hodo_v_count == 1;
  _eval_run.valid_hodo_v = valid_hodo_v;
  hNormalization->Fill("valid_hodo_v", valid_hodo_v);

  const bool good_e = (valid_hodo_v and valid_hodo_h and cherekov_e
      and trigger_veto_pass) and (not _is_sim);
  hNormalization->Fill("good_e", good_e);

  // simple clustering
  pair<int, int> max_3x3 = find_max(TOWER_CALIB_CEMC, 3);
  pair<int, int> max_5x5 = find_max(TOWER_CALIB_CEMC, 5);

  _eval_3x3_raw.max_col = max_3x3.first;
  _eval_3x3_raw.max_row = max_3x3.second;
  _eval_3x3_prod.max_col = max_3x3.first;
  _eval_3x3_prod.max_row = max_3x3.second;
  _eval_3x3_temp.max_col = max_3x3.first;
  _eval_3x3_temp.max_row = max_3x3.second;
  _eval_3x3_recalib.max_col = max_3x3.first;
  _eval_3x3_recalib.max_row = max_3x3.second;

  _eval_5x5_raw.max_col = max_5x5.first;
  _eval_5x5_raw.max_row = max_5x5.second;
  _eval_5x5_prod.max_col = max_5x5.first;
  _eval_5x5_prod.max_row = max_5x5.second;
  _eval_5x5_temp.max_col = max_5x5.first;
  _eval_5x5_temp.max_row = max_5x5.second;
  _eval_5x5_recalib.max_col = max_5x5.first;
  _eval_5x5_recalib.max_row = max_5x5.second;

  // tower
  bool good_temp = true;
  double sum_energy_calib = 0;
  double sum_energy_T = 0;
  TH1F * hTemperature = dynamic_cast<TH1F *>(hm->getHisto("hTemperature"));
  assert(hTemperature);

  stringstream sdata;

  if (good_e)
    sdata << abs(_eval_run.beam_mom) << "\t";

  // tower temperature and recalibration
    {
      auto range = TOWER_CALIB_CEMC->getTowers();
      for (auto it = range.first; it != range.second; ++it)
        {

          RawTowerDefs::keytype key = it->first;
          RawTower* tower = it->second;
          assert(tower);

          const int col = tower->get_bineta();
          const int row = tower->get_binphi();

          if (col < 0 or col >= 8)
            continue;
          if (row < 0 or row >= 8)
            continue;

          const double energy_calib = tower->get_energy();
          sum_energy_calib += energy_calib;

          RawTower* tower_raw = TOWER_RAW_CEMC->getTower(key);
          assert(tower_raw);

          double energy_T = 0;
          if (not _is_sim)
            {
              RawTower_Temperature * temp_t =
                  dynamic_cast<RawTower_Temperature *>(TOWER_TEMPERATURE_EMCAL->getTower(
                      tower->get_row(), tower->get_column())); // note swap of col/row in temperature storage
              assert(temp_t);

              const double T = temp_t->get_temperature_from_time(
                  eventheader->get_TimeStamp());
              hTemperature->Fill(T);

              if (T < 25 or T > 35)
                good_temp = false;

              energy_T = TemperatureCorrection::Apply(energy_calib, T);
            }

          // recalibration
          assert(
              _recalib_const.find(make_pair(col, row)) != _recalib_const.end());
          const double energy_recalib = energy_T
              * _recalib_const[make_pair(col, row)];

          // energy sums
          sum_energy_T += energy_T;

          // calibration file
//          sdata << tower->get_energy() << "\t";
          // calibration file - only output 5x5 towers
          if (col >= max_5x5.first - 2 and col <= max_5x5.first + 2
              and row >= max_5x5.second - 2 and row <= max_5x5.second + 2)
            {
              sdata << tower->get_energy() << "\t";
            }
          else
            {
              sdata << 0 << "\t";
            }

          // cluster 3x3
          if (col >= max_3x3.first - 1 and col <= max_3x3.first + 1)
            if (row >= max_3x3.second - 1 and row <= max_3x3.second + 1)
              {
                // in cluster

                _eval_3x3_raw.average_col += abs(tower_raw->get_energy()) * col;
                _eval_3x3_raw.average_row += abs(tower_raw->get_energy()) * row;
                _eval_3x3_raw.sum_E += abs(tower_raw->get_energy());

                _eval_3x3_prod.average_col += energy_calib * col;
                _eval_3x3_prod.average_row += energy_calib * row;
                _eval_3x3_prod.sum_E += energy_calib;

                _eval_3x3_temp.average_col += energy_T * col;
                _eval_3x3_temp.average_row += energy_T * row;
                _eval_3x3_temp.sum_E += energy_T;

                _eval_3x3_recalib.average_col += energy_recalib * col;
                _eval_3x3_recalib.average_row += energy_recalib * row;
                _eval_3x3_recalib.sum_E += energy_recalib;
              }

          // cluster 5x5
          if (col >= max_5x5.first - 2 and col <= max_5x5.first + 2)
            if (row >= max_5x5.second - 2 and row <= max_5x5.second + 2)
              {
                // in cluster

                _eval_5x5_raw.average_col += abs(tower_raw->get_energy()) * col;
                _eval_5x5_raw.average_row += abs(tower_raw->get_energy()) * row;
                _eval_5x5_raw.sum_E += abs(tower_raw->get_energy());

                _eval_5x5_prod.average_col += energy_calib * col;
                _eval_5x5_prod.average_row += energy_calib * row;
                _eval_5x5_prod.sum_E += energy_calib;

                _eval_5x5_temp.average_col += energy_T * col;
                _eval_5x5_temp.average_row += energy_T * row;
                _eval_5x5_temp.sum_E += energy_T;

                _eval_5x5_recalib.average_col += energy_recalib * col;
                _eval_5x5_recalib.average_row += energy_recalib * row;
                _eval_5x5_recalib.sum_E += energy_recalib;
              }
        }
    }

  _eval_3x3_raw.reweight_clus_pol();
  _eval_5x5_raw.reweight_clus_pol();
  _eval_3x3_prod.reweight_clus_pol();
  _eval_5x5_prod.reweight_clus_pol();
  _eval_3x3_temp.reweight_clus_pol();
  _eval_5x5_temp.reweight_clus_pol();
  _eval_3x3_recalib.reweight_clus_pol();
  _eval_5x5_recalib.reweight_clus_pol();

  const double EoP = sum_energy_T / abs(_eval_run.beam_mom);
  hNormalization->Fill("good_temp", good_temp);

  bool good_data = good_e and good_temp;
  hNormalization->Fill("good_data", good_data);

  _eval_run.good_temp = good_temp;
  _eval_run.good_e = good_e;
  _eval_run.good_data = good_data;
  _eval_run.sum_energy_T = sum_energy_T;
  _eval_run.EoP = EoP;

  // E/p
  if (good_data)
    {
      if (verbosity >= 3)
        cout << __PRETTY_FUNCTION__ << " sum_energy_calib = "
            << sum_energy_calib << " sum_energy_T = " << sum_energy_T
            << " _eval_run.beam_mom = " << _eval_run.beam_mom << endl;

      TH2F * hEoP = dynamic_cast<TH2F *>(hm->getHisto("hEoP"));
      assert(hEoP);

      hEoP->Fill(EoP, abs(_eval_run.beam_mom));
    }

  // calibration file
  if (good_data and abs(_eval_run.beam_mom) >= 4
      and abs(_eval_run.beam_mom) <= 8)
    {
      assert(fdata.is_open());

      fdata << sdata.str();

      fdata << endl;
    }

  TTree * T = dynamic_cast<TTree *>(hm->getHisto("T"));
  assert(T);
  T->Fill();

  return Fun4AllReturnCodes::EVENT_OK;
}

pair<int, int>
Proto2ShowerCalib::find_max(RawTowerContainer* towers, int cluster_size)
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

              RawTower * t = towers->getTower(dcol, drow);
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

int
Proto2ShowerCalib::LoadRecalibMap(const std::string & file)
{
  if (verbosity)
    {
      cout << __PRETTY_FUNCTION__ << " - input recalibration constant from "
          << file << endl;
    }

  ifstream fcalib(file);

  assert(fcalib.is_open());

  string line;

  while (not fcalib.eof())
    {
      getline(fcalib, line);

      if (verbosity > 10)
        {
          cout << __PRETTY_FUNCTION__ << " get line " << line << endl;
        }
      istringstream sline(line);

      int col = -1;
      int row = -1;
      double calib = 0;

      sline >> col >> row >> calib;

      if (not sline.fail())
        {
          if (verbosity)
            {
              cout << __PRETTY_FUNCTION__ << " - recalibration constant  "
                  << col << "," << row << " = " << calib << endl;
            }

          _recalib_const[make_pair(col, row)] = calib;
        }

    }

  return _recalib_const.size();
}

