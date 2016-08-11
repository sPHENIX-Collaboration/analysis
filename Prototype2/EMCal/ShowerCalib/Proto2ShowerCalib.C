#include "Proto2ShowerCalib.h"

#include <prototype2/RawTower_Temperature.h>
#include <prototype2/RawTower_Prototype2.h>
#include <g4cemc/RawTowerContainer.h>
#include <pdbcalbase/PdbParameterMap.h>
#include <g4detectors/PHG4Parameters.h>
#include <ffaobjects/EventHeader.h>

#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/PHTFileServer.h>
#include <phool/PHCompositeNode.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/getClass.h>
#include <fun4all/Fun4AllHistoManager.h>

#include <phool/PHCompositeNode.h>

#include <TNtuple.h>
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TVector3.h>
#include <TLorentzVector.h>

#include <exception>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <cassert>
#include <cmath>

using namespace std;

Proto2ShowerCalib::Proto2ShowerCalib(const std::string &filename) :
    SubsysReco("Proto2ShowerCalib"), _filename(filename), _ievent(0)
{

  verbosity = 1;

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

// help index files with TChain
  TTree * T_Index = new TTree("T_Index", "T_Index");
  assert(T_Index);
  T_Index->Write();

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

  fdata.open(_filename + ".dat",std::fstream::out);

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
  hm->registerHisto(hNormalization);

  hm->registerHisto(new TH1F("hCheck_Veto", "hCheck_Veto", 1000, -500, 500));
  hm->registerHisto(
      new TH1F("hCheck_Hodo_H", "hCheck_Hodo_H", 1000, -500, 500));
  hm->registerHisto(
      new TH1F("hCheck_Hodo_V", "hCheck_Hodo_V", 1000, -500, 500));

  hm->registerHisto(new TH1F("hBeam_Mom", "hBeam_Mom", 1200, -120, 120));

  hm->registerHisto(new TH2F("hEoP", "hEoP", 1000, 0, 1.5, 120, .5, 120.5));

  hm->registerHisto(new TH1F("hTemperature", "hTemperature", 500, 0, 50));

  return Fun4AllReturnCodes::EVENT_OK;
}

int
Proto2ShowerCalib::process_event(PHCompositeNode *topNode)
{

  if (verbosity > 2)
    cout << "Proto2ShowerCalib::process_event() entered" << endl;


  Fun4AllHistoManager *hm = get_HistoManager();
  assert(hm);

  PdbParameterMap *info = findNode::getClass<PdbParameterMap>(topNode,
      "RUN_INFO");

  assert(info);

  PHG4Parameters run_info_copy("RunInfo");
  run_info_copy.FillFrom(info);

  const double beam_mom = run_info_copy.get_double_param("beam_MTNRG_GeV");

  TH1F * hBeam_Mom = dynamic_cast<TH1F *>(hm->getHisto("hBeam_Mom"));
  assert(hBeam_Mom);

  hBeam_Mom->Fill(beam_mom);

  EventHeader* eventheader = findNode::getClass<EventHeader>(topNode,
      "EventHeader");
  assert(eventheader);

  // normalization
  TH1F * hNormalization = dynamic_cast<TH1F *>(hm->getHisto("hNormalization"));
  assert(hNormalization);

  hNormalization->Fill("ALL", 1);

  // other nodes
  RawTowerContainer* TOWER_CALIB_TRIGGER_VETO = findNode::getClass<
      RawTowerContainer>(topNode, "TOWER_CALIB_TRIGGER_VETO");
  assert(TOWER_CALIB_TRIGGER_VETO);

  RawTowerContainer* TOWER_CALIB_HODO_HORIZONTAL = findNode::getClass<
      RawTowerContainer>(topNode, "TOWER_CALIB_HODO_HORIZONTAL");
  assert(TOWER_CALIB_HODO_HORIZONTAL);
  RawTowerContainer* TOWER_CALIB_HODO_VERTICAL = findNode::getClass<
      RawTowerContainer>(topNode, "TOWER_CALIB_HODO_VERTICAL");
  assert(TOWER_CALIB_HODO_VERTICAL);

  RawTowerContainer* TOWER_RAW_CEMC = findNode::getClass<RawTowerContainer>(
      topNode, "TOWER_RAW_CEMC");
  assert(TOWER_RAW_CEMC);
  RawTowerContainer* TOWER_CALIB_CEMC = findNode::getClass<RawTowerContainer>(
      topNode, "TOWER_CALIB_CEMC");
  assert(TOWER_CALIB_CEMC);
  RawTowerContainer* TOWER_TEMPERATURE_EMCAL = findNode::getClass<
      RawTowerContainer>(topNode, "TOWER_TEMPERATURE_EMCAL");
  assert(TOWER_TEMPERATURE_EMCAL);

  RawTowerContainer* TOWER_CALIB_C1 = findNode::getClass<RawTowerContainer>(
      topNode, "TOWER_CALIB_C1");
  assert(TOWER_CALIB_C1);
  RawTowerContainer* TOWER_CALIB_C2 = findNode::getClass<RawTowerContainer>(
      topNode, "TOWER_CALIB_C2");
  assert(TOWER_CALIB_C2);

  // Cherenkov
  const double c2_in = TOWER_CALIB_C2->getTower(10)->get_energy();
  const double c2_out = TOWER_CALIB_C2->getTower(11)->get_energy();
  const double c1 = TOWER_CALIB_C1->getTower(0)->get_energy();

  bool cherekov_e = (c2_in + c2_out) > 100;
  hNormalization->Fill("C2-e", cherekov_e);

  TH2F * hCheck_Cherenkov = dynamic_cast<TH2F *>(hm->getHisto(
      "hCheck_Cherenkov"));
  assert(hCheck_Cherenkov);
  hCheck_Cherenkov->Fill(c1, "C1", 1);
  hCheck_Cherenkov->Fill(c2_in, "C2 in", 1);
  hCheck_Cherenkov->Fill(c2_out, "C2 out", 1);
  hCheck_Cherenkov->Fill(c2_in + c2_out, "C2 sum", 1);

  // veto
  TH1F * hCheck_Veto = dynamic_cast<TH1F *>(hm->getHisto("hCheck_Veto"));
  assert(hCheck_Veto);
  bool trigger_veto_pass = true;
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

  // hodoscope
  TH1F * hCheck_Hodo_H = dynamic_cast<TH1F *>(hm->getHisto("hCheck_Hodo_H"));
  assert(hCheck_Hodo_H);
  int hodo_h_count = 0;
    {
      auto range = TOWER_CALIB_HODO_HORIZONTAL->getTowers();
      for (auto it = range.first; it != range.second; ++it)
        {
          RawTower* tower = it->second;
          assert(tower);

          hCheck_Hodo_H->Fill(tower->get_energy());

          if (abs(tower->get_energy()) > 30)
            hodo_h_count++;
        }
    }
  const bool valid_hodo_h = hodo_h_count == 1;
  hNormalization->Fill("valid_hodo_h", valid_hodo_h);

  TH1F * hCheck_Hodo_V = dynamic_cast<TH1F *>(hm->getHisto("hCheck_Hodo_V"));
  assert(hCheck_Hodo_V);
  int hodo_v_count = 0;
    {
      auto range = TOWER_CALIB_HODO_HORIZONTAL->getTowers();
      for (auto it = range.first; it != range.second; ++it)
        {
          RawTower* tower = it->second;
          assert(tower);

          hCheck_Hodo_V->Fill(tower->get_energy());

          if (abs(tower->get_energy()) > 30)
            hodo_v_count++;
        }
    }
  const bool valid_hodo_v = hodo_v_count == 1;
  hNormalization->Fill("valid_hodo_v", valid_hodo_v);

  const bool good_e = valid_hodo_v and valid_hodo_h and cherekov_e
      and trigger_veto_pass;
  hNormalization->Fill("good_e", good_e);

  // tower
  double sum_energy_calib = 0;
  double sum_energy_T = 0;
  TH1F * hTemperature = dynamic_cast<TH1F *>(hm->getHisto("hTemperature"));
  assert(hTemperature);
  assert(fdata.is_open());

  if (good_e)
    fdata<<abs(beam_mom)<<"\t";


    {
      auto range = TOWER_CALIB_CEMC->getTowers();
      for (auto it = range.first; it != range.second; ++it)
        {

          RawTowerDefs::keytype key = it->first;
          RawTower* tower = it->second;
          assert(tower);

          const double energy_calib = tower->get_energy();
          sum_energy_calib += energy_calib;

          RawTower* tower_raw = TOWER_RAW_CEMC->getTower(key);
          assert(tower_raw);

          RawTower_Temperature * temp_t =
              dynamic_cast<RawTower_Temperature *>(TOWER_TEMPERATURE_EMCAL->getTower(
                  tower->get_row(), tower->get_column()));
          assert(temp_t);

          const double T = temp_t->get_temperature_from_time(
              eventheader->get_TimeStamp());
          hTemperature->Fill(T);

          const double energy_T = TemperatureCorrection::Apply(energy_calib, T);

          sum_energy_T += energy_T;

          if (good_e)
            fdata<<tower->get_energy()<<"\t";
//          fdata<<tower_raw->get_energy()<<"\t";
        }
    }
  const double EoP = sum_energy_T / abs(beam_mom);

  if (good_e)
    fdata<<endl;

  if (good_e)
    {
      if (verbosity>=3)
        cout << __PRETTY_FUNCTION__ << " sum_energy_calib = " << sum_energy_calib
            << " sum_energy_T = " << sum_energy_T << " beam_mom = " << beam_mom
            << endl;

      TH2F * hEoP = dynamic_cast<TH2F *>(hm->getHisto("hEoP"));
      assert(hEoP);

      hEoP->Fill(EoP, abs(beam_mom));
    }

  return Fun4AllReturnCodes::EVENT_OK;
}
