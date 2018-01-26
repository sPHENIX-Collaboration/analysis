#include "Proto3ShowerCalib.h"

#include <prototype3/RawTower_Prototype3.h>
#include <calobase/RawTowerContainer.h>
#include <pdbcalbase/PdbParameterMap.h>
#include <phparameter/PHParameters.h>
#include <ffaobjects/EventHeader.h>
#include <g4detectors/PHG4ScintillatorSlat.h>
#include <g4detectors/PHG4ScintillatorSlatContainer.h>
#include <g4detectors/PHG4ScintillatorSlatDefs.h>


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
#include <TF1.h>
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

ClassImp(Proto3ShowerCalib::HCAL_shower);
ClassImp(Proto3ShowerCalib::Eval_Run);
ClassImp(Proto3ShowerCalib::Time_Samples);

Proto3ShowerCalib::Proto3ShowerCalib(const std::string &filename) :
    SubsysReco("Proto3ShowerCalib"), _filename(filename), _ievent(0)
{

  verbosity = 1;
  _is_sim = false;
  _fill_tower_samples = true;
  _fill_time_samples = false;
  _fill_slats = false;
  _eval_run.reset();
  _shower.reset();
  _time_samples.reset();
  _slats.reset();

  for (int col = 0; col < n_size_emcal; ++col)
    for (int row = 0; row < n_size_emcal; ++row)
      {
        _emcal_recalib_const[make_pair(col, row)] = 0;
      }

  for (int col = 0; col < n_size_hcalin; ++col)
    for (int row = 0; row < n_size_hcalin; ++row)
     {
        _hcalin_recalib_const[make_pair(col, row)] = 0;
        _hcalout_recalib_const[make_pair(col, row)] = 0;
     }

  smearing = new TF1("smearing","gaus(0)", 0, 2);
  // gaus(0) = [0]*exp(-0.5((x-[1])/[2])**2)
  // par 0 => Set to 1
  smearing->SetParameter(0, 1.);
  // par1 => Set to 0
  smearing->SetParameter(1, 1.);
  // par2 => Set to 1
  smearing->SetParameter(2, 0.3);
}

void Proto3ShowerCalib::fill_tower_samples(const bool b)
{
  _fill_tower_samples = b;
}

void Proto3ShowerCalib::fill_time_samples(const bool b)
{
  _fill_time_samples = b;
}

void Proto3ShowerCalib::fill_slat_values(const bool b)
{
  _fill_slats = b;
}


Proto3ShowerCalib::~Proto3ShowerCalib()
{
}

Fun4AllHistoManager *
Proto3ShowerCalib::get_HistoManager()
{

  Fun4AllServer *se = Fun4AllServer::instance();
  Fun4AllHistoManager *hm = se->getHistoManager("Proto3ShowerCalib_HISTOS");

  if (not hm)
    {
      cout
          << "Proto3ShowerCalib::get_HistoManager - Making Fun4AllHistoManager Proto3ShowerCalib_HISTOS"
          << endl;
      hm = new Fun4AllHistoManager("Proto3ShowerCalib_HISTOS");
      se->registerHistoManager(hm);
    }

  assert(hm);

  return hm;
}

int
Proto3ShowerCalib::InitRun(PHCompositeNode *topNode)
{
  if (verbosity)
    cout << "Proto3ShowerCalib::InitRun" << endl;

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
Proto3ShowerCalib::End(PHCompositeNode *topNode)
{
  cout << "Proto3ShowerCalib::End - write to " << _filename << endl;
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
Proto3ShowerCalib::Init(PHCompositeNode *topNode)
{

  _ievent = 0;

  cout << "Proto3ShowerCalib::get_HistoManager - Making PHTFileServer "
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
  hCheck_Cherenkov->GetXaxis()->SetBinLabel(3, "C2-h");
  hCheck_Cherenkov->GetXaxis()->SetBinLabel(4, "trigger_veto_pass");
  hCheck_Cherenkov->GetXaxis()->SetBinLabel(5, "valid_hodo_h");
  hCheck_Cherenkov->GetXaxis()->SetBinLabel(6, "valid_hodo_v");
  hCheck_Cherenkov->GetXaxis()->SetBinLabel(7, "good_e");
  hCheck_Cherenkov->GetXaxis()->SetBinLabel(8, "good_h");
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
  T->Branch("shower", &_shower);
  if(_fill_time_samples) T->Branch("time", &_time_samples);
  cout << _fill_slats << endl;
  if(_fill_slats && _is_sim) T->Branch("Slats", &_slats);

  return Fun4AllReturnCodes::EVENT_OK;
}

int
Proto3ShowerCalib::process_event(PHCompositeNode *topNode)
{

  if (verbosity > 2)
    cout << "Proto3ShowerCalib::process_event() entered" << endl;

  // init eval objects
  _eval_run.reset();
  _shower.reset();
  _slats.reset();
  Fun4AllHistoManager *hm = get_HistoManager();
  assert(hm);

  PdbParameterMap *info = findNode::getClass<PdbParameterMap>(topNode,
      "RUN_INFO");

  //if(!_is_sim) assert(info);

  if(info)
  {
   PHParameters run_info_copy("RunInfo");
   run_info_copy.FillFrom(info);

   _eval_run.beam_mom = run_info_copy.get_double_param("beam_MTNRG_GeV");

   TH1F * hBeam_Mom = dynamic_cast<TH1F *>(hm->getHisto("hBeam_Mom"));
   assert(hBeam_Mom);

   hBeam_Mom->Fill(_eval_run.beam_mom);
  }

  EventHeader* eventheader = findNode::getClass<EventHeader>(topNode,
      "EventHeader");
  //if(!_is_sim) assert(eventheader);

  if( eventheader )
  {
   _eval_run.run = eventheader->get_RunNumber();
   if (verbosity > 4)
    cout << __PRETTY_FUNCTION__ << _eval_run.run << endl;

   _eval_run.event = eventheader->get_EvtSequence();
  }
  // normalization
  TH1F * hNormalization = dynamic_cast<TH1F *>(hm->getHisto("hNormalization"));
  assert(hNormalization);

  hNormalization->Fill("ALL", 1);

  // other nodes
  RawTowerContainer* TOWER_CALIB_TRIGGER_VETO = findNode::getClass<
      RawTowerContainer>(topNode, "TOWER_CALIB_TRIGGER_VETO");
  //if(!_is_sim) assert(TOWER_CALIB_TRIGGER_VETO);

  RawTowerContainer* TOWER_CALIB_HODO_HORIZONTAL = findNode::getClass<
      RawTowerContainer>(topNode, "TOWER_CALIB_HODO_HORIZONTAL");
  //if(!_is_sim) assert(TOWER_CALIB_HODO_HORIZONTAL);

  RawTowerContainer* TOWER_CALIB_HODO_VERTICAL = findNode::getClass<
      RawTowerContainer>(topNode, "TOWER_CALIB_HODO_VERTICAL");
  //if(!_is_sim) assert(TOWER_CALIB_HODO_VERTICAL);

 PHG4ScintillatorSlatContainer* hcalin_slats = 0;
 PHG4ScintillatorSlatContainer* hcalout_slats = 0;
 if( _is_sim and _fill_slats)
 {
  std::string cellnodename = "G4CELL_HCALIN";
  hcalin_slats = findNode::getClass<PHG4ScintillatorSlatContainer>(topNode, cellnodename.c_str());
 if(!hcalin_slats) cout << "HCALIN slats not found" << endl;

  cellnodename = "G4CELL_HCALOUT";
  hcalout_slats = findNode::getClass<PHG4ScintillatorSlatContainer>(topNode, cellnodename.c_str());
 if(!hcalout_slats) cout << "HCALOUT slats not found" << endl;
 }

 RawTowerContainer* TOWER_RAW_CEMC;
   TOWER_RAW_CEMC = findNode::getClass<RawTowerContainer>(
      topNode, "TOWER_RAW_CEMC");
 assert(TOWER_RAW_CEMC);

 RawTowerContainer* TOWER_CALIB_CEMC;
 if(!_is_sim)
 {
  TOWER_CALIB_CEMC = findNode::getClass<RawTowerContainer>(
      topNode, "TOWER_CALIB_CEMC");
 } else {
  TOWER_CALIB_CEMC = findNode::getClass<RawTowerContainer>(
      topNode, "TOWER_SIM_CEMC"); //"TOWER_CALIB_LG_CEMC");
 }
  assert(TOWER_CALIB_CEMC);

  RawTowerContainer* TOWER_RAW_LG_HCALIN = 0;
  RawTowerContainer* TOWER_RAW_HG_HCALIN = 0;
  if(!_is_sim)
  {
   TOWER_RAW_LG_HCALIN = findNode::getClass<RawTowerContainer>(
      topNode, "TOWER_RAW_LG_HCALIN");
   TOWER_RAW_HG_HCALIN= findNode::getClass<RawTowerContainer>(
      topNode, "TOWER_RAW_HG_HCALIN");
   assert(TOWER_RAW_LG_HCALIN);
   assert(TOWER_RAW_HG_HCALIN);
  }

  RawTowerContainer* TOWER_CALIB_HCALIN;
  if(!_is_sim)
  {
   TOWER_CALIB_HCALIN = findNode::getClass<RawTowerContainer>(
      topNode, "TOWER_CALIB_LG_HCALIN");
  } else {
   TOWER_CALIB_HCALIN = findNode::getClass<RawTowerContainer>(
      topNode, "TOWER_SIM_HCALIN"); //"TOWER_CALIB_LG_HCALIN");
  }
  assert(TOWER_CALIB_HCALIN);

  RawTowerContainer* TOWER_RAW_LG_HCALOUT = 0;
  RawTowerContainer* TOWER_RAW_HG_HCALOUT = 0;
  if(!_is_sim)
  {
   TOWER_RAW_LG_HCALOUT = findNode::getClass<RawTowerContainer>(
      topNode, "TOWER_RAW_LG_HCALOUT");
   TOWER_RAW_HG_HCALOUT = findNode::getClass<RawTowerContainer>(
      topNode, "TOWER_RAW_HG_HCALOUT");
   assert(TOWER_RAW_LG_HCALOUT);
   assert(TOWER_RAW_HG_HCALOUT);
  }

  RawTowerContainer* TOWER_CALIB_HCALOUT;
  if(!_is_sim)
    {
      TOWER_CALIB_HCALOUT = findNode::getClass<RawTowerContainer>(
       topNode, "TOWER_CALIB_LG_HCALOUT");
    } else {
      TOWER_CALIB_HCALOUT = findNode::getClass<RawTowerContainer>(
       topNode, "TOWER_SIM_HCALOUT");// "TOWER_CALIB_LG_HCALOUT");
    }
  assert(TOWER_CALIB_HCALOUT);

  RawTowerContainer* TOWER_TEMPERATURE_EMCAL = findNode::getClass<
      RawTowerContainer>(topNode, "TOWER_TEMPERATURE_EMCAL");
  if(!_is_sim) assert(TOWER_TEMPERATURE_EMCAL);

  RawTowerContainer* TOWER_CALIB_C1 = findNode::getClass<RawTowerContainer>(
      topNode, "TOWER_CALIB_C1");
  //if(!_is_sim) assert(TOWER_CALIB_C1);
  RawTowerContainer* TOWER_CALIB_C2 = findNode::getClass<RawTowerContainer>(
      topNode, "TOWER_CALIB_C2");
  //if(!_is_sim) assert(TOWER_CALIB_C2);

  if(!_is_sim && TOWER_CALIB_C2 && TOWER_CALIB_C1 && eventheader)
  {
  // Cherenkov
  RawTower * t_c2_in = NULL;
  RawTower * t_c2_out = NULL;

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
  bool cherekov_e = (_eval_run.C2_sum) > 100;
  hNormalization->Fill("C2-e", cherekov_e);

  bool cherekov_h = (_eval_run.C2_sum) < 20;
  hNormalization->Fill("C2-h", cherekov_h);

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
  _eval_run.trigger_veto_pass = trigger_veto_pass;

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

  const bool good_e = valid_hodo_v and valid_hodo_h and cherekov_e and trigger_veto_pass;

  const bool good_h = valid_hodo_v and valid_hodo_h and cherekov_h and trigger_veto_pass;

  hNormalization->Fill("good_e", good_e);
  hNormalization->Fill("good_h", good_h);

  _eval_run.good_temp = 1;
  _eval_run.good_e = good_e;
  _eval_run.good_h = good_h;

  }
  // tower
  double emcal_sum_energy_calib = 0;
  double emcal_sum_energy_recalib = 0;

  double hcalin_sum_energy_calib = 0;
  double hcalin_sum_energy_recalib = 0;

  double hcalout_sum_energy_calib = 0;
  double hcalout_sum_energy_recalib = 0;

  stringstream sdata;

   //EMCAL towers
    {
      auto range = TOWER_CALIB_CEMC->getTowers();
      for (auto it = range.first; it != range.second; ++it)
        {

          //RawTowerDefs::keytype key = it->first;
          RawTower* tower = it->second;
          assert(tower);

          const double energy_calib = tower->get_energy();
          emcal_sum_energy_calib += energy_calib;

          if(_is_sim) continue;
          const int col = tower->get_column();
          const int row = tower->get_row();
          if(_fill_tower_samples) _shower.emcal_twr_e[row+4*col] = energy_calib;
          // recalibration
          assert(
              _emcal_recalib_const.find(make_pair(col, row)) != _emcal_recalib_const.end());
          const double energy_recalib = energy_calib
              * _emcal_recalib_const[make_pair(col, row)];

          // energy sums
          emcal_sum_energy_recalib += energy_recalib;

         }
      }
  //HCALIN cells
  if( _is_sim and _fill_slats)
  {
  double hcalin_slats_edep = 0.;
  double hcalin_slats_ly = 0.;
  double hcalout_slats_edep = 0.;
  double hcalout_slats_ly = 0.;

  auto range = hcalin_slats->getScintillatorSlats();
  for(auto it=range.first; it!=range.second; ++it)
  {
   PHG4ScintillatorSlat *cell = it->second;
   hcalin_slats_edep += smearing->GetRandom()*cell->get_edep();
   hcalin_slats_ly += smearing->GetRandom()*cell->get_light_yield();
   }

  range = hcalout_slats->getScintillatorSlats();
  for(auto it=range.first; it!=range.second; ++it)
  {
   PHG4ScintillatorSlat *cell = it->second;
   hcalout_slats_edep += smearing->GetRandom()*cell->get_edep();
   hcalout_slats_ly += smearing->GetRandom()*cell->get_light_yield();
   }

   _slats.hcalin_edep = hcalin_slats_edep;
   _slats.hcalin_lightyield = hcalin_slats_ly;
   _slats.hcalout_edep = hcalout_slats_edep;
   _slats.hcalout_lightyield = hcalout_slats_ly;
  }

  //HCALIN towers
  {
      auto range = TOWER_CALIB_HCALIN->getTowers();
      for (auto it = range.first; it != range.second; ++it)
        {
          RawTower* tower = it->second;
          assert(tower);

          const double energy_calib = tower->get_energy();
          hcalin_sum_energy_calib += energy_calib;

          if(_is_sim) continue;
          const int col = tower->get_column();
          const int row = tower->get_row();
          if(_fill_tower_samples) _shower.hcalin_twr_e[row+4*col] = energy_calib;

          assert(
              _hcalin_recalib_const.find(make_pair(col, row)) != _hcalin_recalib_const.end());
          const double energy_recalib = energy_calib
              * _hcalin_recalib_const[make_pair(col, row)];

          hcalin_sum_energy_recalib += energy_recalib;
         }
     }

   //HCALOUT towers
   {
      auto range = TOWER_CALIB_HCALOUT->getTowers();
      for (auto it = range.first; it != range.second; ++it)
        {
          RawTower* tower = it->second;
          assert(tower);

          const double energy_calib = tower->get_energy();
          hcalout_sum_energy_calib += energy_calib;

          if(_is_sim) continue;
          const int col = tower->get_column();
          const int row = tower->get_row();
          if(_fill_tower_samples) _shower.hcalout_twr_e[row+4*col] = energy_calib;

          assert(
              _hcalout_recalib_const.find(make_pair(col, row)) != _hcalout_recalib_const.end());
          const double energy_recalib = energy_calib
              * _hcalout_recalib_const[make_pair(col, row)];

              hcalout_sum_energy_recalib += energy_recalib;
         }
     }


  const double EoP = emcal_sum_energy_recalib / abs(_eval_run.beam_mom);
  _eval_run.EoP = EoP;

  // E/p
  if (_eval_run.good_e)
    {
      if (verbosity >= 3)
        cout << __PRETTY_FUNCTION__ << " sum_energy_calib = "
            << emcal_sum_energy_calib << " sum_energy_recalib = " << emcal_sum_energy_recalib
            << " _eval_run.beam_mom = " << _eval_run.beam_mom << endl;

      TH2F * hEoP = dynamic_cast<TH2F *>(hm->getHisto("hEoP"));
      assert(hEoP);

      hEoP->Fill(EoP, abs(_eval_run.beam_mom));
    }

  // calibration file
   assert(fdata.is_open());
   sdata << emcal_sum_energy_calib << "; " << hcalin_sum_energy_calib << "; " << hcalout_sum_energy_calib << "; " << _eval_run.good_e << "; " << _eval_run.good_h;
   fdata << sdata.str();
   fdata << endl;

  _shower.emcal_e = emcal_sum_energy_calib;
  _shower.hcalin_e = hcalin_sum_energy_calib;
  _shower.hcalout_e = hcalout_sum_energy_calib;
  _shower.sum_e = emcal_sum_energy_calib + hcalin_sum_energy_calib + hcalout_sum_energy_calib ;
  _shower.hcal_asym = (hcalin_sum_energy_calib - hcalout_sum_energy_calib)/(hcalin_sum_energy_calib + hcalout_sum_energy_calib);

  _shower.emcal_e_recal = emcal_sum_energy_recalib;
  _shower.hcalin_e_recal = hcalin_sum_energy_recalib;
  _shower.hcalout_e_recal = hcalout_sum_energy_recalib;
  _shower.sum_e_recal = emcal_sum_energy_recalib + hcalin_sum_energy_recalib + hcalout_sum_energy_recalib ;


  if(_fill_time_samples && !_is_sim)
  {
   //HCALIN
   {
    auto range = TOWER_RAW_LG_HCALIN->getTowers();
    for (auto it = range.first; it != range.second; ++it)
    {
      RawTower_Prototype3* tower = dynamic_cast<RawTower_Prototype3 *>(it->second);
      assert(tower);

      int col = tower->get_column();
      int row = tower->get_row();
      int towerid = row + 4*col;
      for(int isample=0; isample<24; isample++)
       _time_samples.hcalin_time_samples[towerid][isample] =
                tower->get_signal_samples(isample);

     }
    }

   //HCALOUT
   {
    auto range = TOWER_RAW_LG_HCALOUT->getTowers();
    for (auto it = range.first; it != range.second; ++it)
    {
      RawTower_Prototype3* tower = dynamic_cast<RawTower_Prototype3 *>(it->second);
      assert(tower);

      int col = tower->get_column();
      int row = tower->get_row();
      int towerid = row + 4*col;
      for(int isample=0; isample<24; isample++)
       _time_samples.hcalout_time_samples[towerid][isample] =
		tower->get_signal_samples(isample);

     }
    }

   //EMCAL
   {
    auto range = TOWER_RAW_CEMC->getTowers();
    for (auto it = range.first; it != range.second; ++it)
    {
      RawTower_Prototype3* tower = dynamic_cast<RawTower_Prototype3 *>(it->second);
      assert(tower);

      int col = tower->get_column();
      int row = tower->get_row();
      int towerid = row + 8*col;
      for(int isample=0; isample<24; isample++)
       _time_samples.emcal_time_samples[towerid][isample] =
                tower->get_signal_samples(isample);

     }
    }
  }

  TTree * T = dynamic_cast<TTree *>(hm->getHisto("T"));
  assert(T);
  T->Fill();

  return Fun4AllReturnCodes::EVENT_OK;
}

int
Proto3ShowerCalib::LoadRecalibMap(const std::string & file)
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

          _emcal_recalib_const[make_pair(col, row)] = calib;
        }

    }

  return _emcal_recalib_const.size();
}

