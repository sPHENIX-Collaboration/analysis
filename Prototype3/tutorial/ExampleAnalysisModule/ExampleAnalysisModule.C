#include "ExampleAnalysisModule.h"
#include "TemperatureCorrection.h"

#include <prototype3/RawTower_Temperature.h>
#include <prototype3/RawTower_Prototype3.h>
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

ClassImp(ExampleAnalysisModule::Eval_Cluster);
ClassImp(ExampleAnalysisModule::Eval_Run);

ExampleAnalysisModule::ExampleAnalysisModule(const std::string &filename) :
    SubsysReco("ExampleAnalysisModule"), _is_sim(false), _filename(filename), _ievent(
        0)
{

  verbosity = 1;

  _eval_run.reset();
  _eval_5x5_CEMC.reset();
}

ExampleAnalysisModule::~ExampleAnalysisModule()
{
}

Fun4AllHistoManager *
ExampleAnalysisModule::get_HistoManager()
{

  Fun4AllServer *se = Fun4AllServer::instance();
  Fun4AllHistoManager *hm = se->getHistoManager("ExampleAnalysisModule_HISTOS");

  if (not hm)
    {
      cout
          << "ExampleAnalysisModule::get_HistoManager - Making Fun4AllHistoManager ExampleAnalysisModule_HISTOS"
          << endl;
      hm = new Fun4AllHistoManager("ExampleAnalysisModule_HISTOS");
      se->registerHistoManager(hm);
    }

  assert(hm);

  return hm;
}

int
ExampleAnalysisModule::InitRun(PHCompositeNode *topNode)
{
  if (verbosity)
    cout << "ExampleAnalysisModule::InitRun" << endl;

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
ExampleAnalysisModule::End(PHCompositeNode *topNode)
{
  cout << "ExampleAnalysisModule::End - write to " << _filename << endl;
  PHTFileServer::get().cd(_filename);

  Fun4AllHistoManager *hm = get_HistoManager();
  assert(hm);
  for (unsigned int i = 0; i < hm->nHistos(); i++)
    hm->getHisto(i)->Write();

//  if (_T_EMCalTrk)
//    _T_EMCalTrk->Write();

  return Fun4AllReturnCodes::EVENT_OK;
}

int
ExampleAnalysisModule::Init(PHCompositeNode *topNode)
{

  _ievent = 0;

  cout << "ExampleAnalysisModule::get_HistoManager - Making PHTFileServer "
      << _filename << endl;
  PHTFileServer::get().open(_filename, "RECREATE");

  Fun4AllHistoManager *hm = get_HistoManager();
  assert(hm);

  //! Histogram of Cherenkov counters
  TH2F * hCheck_Cherenkov = new TH2F("hCheck_Cherenkov", "hCheck_Cherenkov",
      110, -20, 2000, 5, .5, 5.5);
  hCheck_Cherenkov->GetYaxis()->SetBinLabel(1, "C1");
  hCheck_Cherenkov->GetYaxis()->SetBinLabel(2, "C2 in");
  hCheck_Cherenkov->GetYaxis()->SetBinLabel(3, "C2 out");
  hCheck_Cherenkov->GetYaxis()->SetBinLabel(4, "C2 sum");
  hCheck_Cherenkov->GetXaxis()->SetTitle("Amplitude");
  hCheck_Cherenkov->GetYaxis()->SetTitle("Cherenkov Channel");
  hm->registerHisto(hCheck_Cherenkov);

  //! Envent nomalization
  TH1F * hNormalization = new TH1F("hNormalization", "hNormalization", 10, .5,
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

  hm->registerHisto(new TH1F("hCheck_Veto", "hCheck_Veto", 1000, -500, 500));
  hm->registerHisto(
      new TH1F("hCheck_Hodo_H", "hCheck_Hodo_H", 1000, -500, 500));
  hm->registerHisto(
      new TH1F("hCheck_Hodo_V", "hCheck_Hodo_V", 1000, -500, 500));

  hm->registerHisto(new TH1F("hBeam_Mom", "hBeam_Mom", 1200, -120, 120));

  // help index files with TChain
  TTree * T = new TTree("T", "T");
  assert(T);
  hm->registerHisto(T);

  T->Branch("info", &_eval_run);
  T->Branch("clus_5x5_CEMC", &_eval_5x5_CEMC);

  return Fun4AllReturnCodes::EVENT_OK;
}

int
ExampleAnalysisModule::process_event(PHCompositeNode *topNode)
{

  if (verbosity > 2)
    cout << "ExampleAnalysisModule::process_event() entered" << endl;

  // init eval objects
  _eval_run.reset();
  _eval_5x5_CEMC.reset();

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

      _eval_run.beam_2CH_mm = run_info_copy.get_double_param("beam_2CH_mm");
      _eval_run.beam_2CV_mm = run_info_copy.get_double_param("beam_2CV_mm");

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

      const PHG4Particle * p =
          truthInfoList->GetPrimaryParticleRange().first->second;
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

  // get DST nodes

  RawTowerContainer* TOWER_RAW_CEMC = findNode::getClass<RawTowerContainer>(
      topNode, _is_sim ? "TOWER_RAW_LG_CEMC" : "TOWER_RAW_CEMC");
  assert(TOWER_RAW_CEMC);

  RawTowerContainer* TOWER_CALIB_CEMC = findNode::getClass<RawTowerContainer>(
      topNode, _is_sim ? "TOWER_CALIB_LG_CEMC" : "TOWER_CALIB_CEMC");
  assert(TOWER_CALIB_CEMC);

  RawTowerContainer* TOWER_CALIB_LG_HCALIN = findNode::getClass<
      RawTowerContainer>(topNode, "TOWER_CALIB_LG_HCALIN");
  assert(TOWER_CALIB_LG_HCALIN);

  RawTowerContainer* TOWER_CALIB_LG_HCALOUT = findNode::getClass<
      RawTowerContainer>(topNode, "TOWER_CALIB_LG_HCALOUT");
  assert(TOWER_CALIB_LG_HCALOUT);

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
  bool cherekov_anti_e = false;
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
      _eval_run.C1 = c1;

      cherekov_e = (_eval_run.C2_sum)
          > (abs(_eval_run.beam_mom) >= 10 ? 100 : 240);
      cherekov_anti_e = (_eval_run.C2_sum) < 10;
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
  const bool good_anti_e = (valid_hodo_v and valid_hodo_h and cherekov_anti_e
      and trigger_veto_pass) and (not _is_sim);
  hNormalization->Fill("good_e", good_e);
  hNormalization->Fill("good_anti_e", good_anti_e);

  // simple clustering by matching to cluster of max energy
  pair<int, int> max_5x5 = find_max(TOWER_CALIB_CEMC, 5);

  _eval_5x5_CEMC.max_col = max_5x5.first;
  _eval_5x5_CEMC.max_row = max_5x5.second;

  // process EMCals
    {
      double sum_energy_calib = 0;

      auto range = TOWER_CALIB_CEMC->getTowers();
      for (auto it = range.first; it != range.second; ++it)
        {

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

          // cluster 5x5
          if (col >= max_5x5.first - 2 and col <= max_5x5.first + 2)
            if (row >= max_5x5.second - 2 and row <= max_5x5.second + 2)
              {
                // in cluster

                _eval_5x5_CEMC.average_col += energy_calib * col;
                _eval_5x5_CEMC.average_row += energy_calib * row;
                _eval_5x5_CEMC.sum_E += energy_calib;

              }
        } //       for (auto it = range.first; it != range.second; ++it)
      _eval_5x5_CEMC.reweight_clus_pol();

      _eval_run.sum_E_CEMC = sum_energy_calib;
    } // process EMCals

  // process inner HCAL
    {
      double sum_energy_calib = 0;

      auto range = TOWER_CALIB_LG_HCALIN->getTowers();
      for (auto it = range.first; it != range.second; ++it)
        {

          RawTower* tower = it->second;
          assert(tower);

          const int col = tower->get_bineta();
          const int row = tower->get_binphi();

          if (col < 0 or col >= 4)
            continue;
          if (row < 0 or row >= 4)
            continue;

          const double energy_calib = tower->get_energy();
          sum_energy_calib += energy_calib;

        } //       for (auto it = range.first; it != range.second; ++it)
      _eval_run.sum_E_HCAL_IN = sum_energy_calib;
    } // process inner HCAL

  // process outer HCAL
    {
      double sum_energy_calib = 0;

      auto range = TOWER_CALIB_LG_HCALOUT->getTowers();
      for (auto it = range.first; it != range.second; ++it)
        {

          RawTower* tower = it->second;
          assert(tower);

          const int col = tower->get_bineta();
          const int row = tower->get_binphi();

          if (col < 0 or col >= 4)
            continue;
          if (row < 0 or row >= 4)
            continue;

          const double energy_calib = tower->get_energy();
          sum_energy_calib += energy_calib;

        } //       for (auto it = range.first; it != range.second; ++it)
      _eval_run.sum_E_HCAL_OUT = sum_energy_calib;
    } // process outer HCAL

  _eval_run.good_e = good_e;
  _eval_run.good_anti_e = good_anti_e;

  TTree * T = dynamic_cast<TTree *>(hm->getHisto("T"));
  assert(T);
  T->Fill();

  return Fun4AllReturnCodes::EVENT_OK;
}

pair<int, int>
ExampleAnalysisModule::find_max(RawTowerContainer* towers, int cluster_size)
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

