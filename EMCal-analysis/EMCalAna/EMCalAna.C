#include "EMCalAna.h"

#include <g4eval/CaloEvalStack.h>
#include <g4eval/CaloRawClusterEval.h>
#include <g4eval/CaloRawTowerEval.h>
#include <g4eval/CaloTruthEval.h>
#include <g4eval/SvtxEvalStack.h>

#include <fun4all/getClass.h>
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/PHTFileServer.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/Fun4AllHistoManager.h>

#include <phool/PHCompositeNode.h>

#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4VtxPoint.h>
#include <g4main/PHG4Particle.h>

#include <g4hough/SvtxVertexMap.h>

#include <g4cemc/RawTowerContainer.h>
#include <g4cemc/RawTowerGeom.h>
#include <g4cemc/RawTower.h>
#include <g4cemc/RawClusterContainer.h>
#include <g4cemc/RawCluster.h>

#include <TNtuple.h>
#include <TFile.h>
#include <TH1F.h>

#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <cmath>

using namespace std;

EMCalAna::EMCalAna(const std::string &filename, EMCalAna::enu_flags flags) :
    SubsysReco("EMCalAna"), _filename(filename), _flags(flags), _ievent(0)
{

  verbosity = 1;

  _hcalout_hit_container = NULL;
  _hcalin_hit_container = NULL;
  _cemc_hit_container = NULL;
  _hcalout_abs_hit_container = NULL;
  _hcalin_abs_hit_container = NULL;
  _cemc_abs_hit_container = NULL;
  _magnet_hit_container = NULL;
  _bh_hit_container = NULL;
  _bh_plus_hit_container = NULL;
  _bh_minus_hit_container = NULL;
  _cemc_electronics_hit_container = NULL;
  _hcalin_spt_hit_container = NULL;
  _svtx_hit_container = NULL;
  _svtx_support_hit_container = NULL;

}

int
EMCalAna::InitRun(PHCompositeNode *topNode)
{
  _ievent = 0;

  // get DST objects
  _hcalout_hit_container = findNode::getClass<PHG4HitContainer>(topNode,
      "G4HIT_HCALOUT");
  _hcalin_hit_container = findNode::getClass<PHG4HitContainer>(topNode,
      "G4HIT_HCALIN");

  _cemc_hit_container = findNode::getClass<PHG4HitContainer>(topNode,
      "G4HIT_CEMC");

  _hcalout_abs_hit_container = findNode::getClass<PHG4HitContainer>(topNode,
      "G4HIT_ABSORBER_HCALOUT");

  _hcalin_abs_hit_container = findNode::getClass<PHG4HitContainer>(topNode,
      "G4HIT_ABSORBER_HCALIN");

  _cemc_abs_hit_container = findNode::getClass<PHG4HitContainer>(topNode,
      "G4HIT_ABSORBER_CEMC");

  return Fun4AllReturnCodes::EVENT_OK;
}

int
EMCalAna::End(PHCompositeNode *topNode)
{
  cout << "EMCalAna::End - write to " << _filename << endl;
  PHTFileServer::get().cd(_filename);

  Fun4AllHistoManager *hm = get_HistoManager();
  assert(hm);
  for (unsigned int i = 0; i < hm->nHistos(); i++)
    hm->getHisto(i)->Write();

  if (_T_EMCalTrk)
    _T_EMCalTrk->Write();

  return Fun4AllReturnCodes::EVENT_OK;
}

int
EMCalAna::Init(PHCompositeNode *topNode)
{

  _ievent = 0;

  cout << "EMCalAna::get_HistoManager - Making PHTFileServer " << _filename
      << endl;
  PHTFileServer::get().open(_filename, "RECREATE");

  if (flag(kProcessSF))
    {
      cout << "EMCalAna::Init - Process sampling fraction" << endl;
      Init_SF(topNode);
    }
  if (flag(kProcessTower))
    {
      cout << "EMCalAna::Init - Process tower occupancies" << endl;
      Init_Tower(topNode);
    }
  if (flag(kProcessTrk))
    {
      cout << "EMCalAna::Init - Process trakcs" << endl;
      Init_Trk(topNode);
    }

  return Fun4AllReturnCodes::EVENT_OK;
}

int
EMCalAna::process_event(PHCompositeNode *topNode)
{

  if (verbosity > 2)
    cout << "EMCalAna::process_event() entered" << endl;

  if (flag(kProcessSF))
    process_event_SF(topNode);
  if (flag(kProcessTower))
    process_event_Tower(topNode);
  if (flag(kProcessTrk))
    process_event_Trk(topNode);

  return Fun4AllReturnCodes::EVENT_OK;
}

int
EMCalAna::Init_Trk(PHCompositeNode *topNode)
{
  static const int BUFFER_SIZE = 32000;
  _T_EMCalTrk = new TTree("T_EMCalTrk", "T_EMCalTrk");

  _T_EMCalTrk->Branch("trk.", _trk.ClassName(), &_trk, BUFFER_SIZE);
  _T_EMCalTrk->Branch("event", &_ievent, "event/I", BUFFER_SIZE);

  return Fun4AllReturnCodes::EVENT_OK;
}

int
EMCalAna::process_event_Trk(PHCompositeNode *topNode)
{

  if (verbosity > 2)
    cout << "EMCalAna::process_event_Trk() entered" << endl;

  SvtxEvalStack svtxevalstack(topNode);

  SvtxVertexEval* vertexeval = svtxevalstack.get_vertex_eval();
  SvtxTrackEval* trackeval = svtxevalstack.get_track_eval();
  SvtxClusterEval* clustereval = svtxevalstack.get_cluster_eval();
  SvtxHitEval* hiteval = svtxevalstack.get_hit_eval();
  SvtxTruthEval* trutheval = svtxevalstack.get_truth_eval();

  SvtxTrackMap* trackmap = findNode::getClass<SvtxTrackMap>(topNode,
      "SvtxTrackMap");
  assert(trackmap);
  PHG4TruthInfoContainer* truthinfo =
      findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
  assert(truthinfo);
  PHG4TruthInfoContainer::Map map = truthinfo->GetPrimaryMap();

  for (PHG4TruthInfoContainer::ConstIterator iter = map.begin();
      iter != map.end(); ++iter)
    {

    }

  return Fun4AllReturnCodes::EVENT_OK;
}

Fun4AllHistoManager *
EMCalAna::get_HistoManager()
{

  Fun4AllServer *se = Fun4AllServer::instance();
  Fun4AllHistoManager *hm = se->getHistoManager("EMCalAna_HISTOS");

  if (not hm)
    {
      cout
          << "EMCalAna::get_HistoManager - Making Fun4AllHistoManager EMCalAna_HISTOS"
          << endl;
      hm = new Fun4AllHistoManager("EMCalAna_HISTOS");
      se->registerHistoManager(hm);
    }

  assert(hm);

  return hm;
}

int
EMCalAna::Init_SF(PHCompositeNode *topNode)
{

  Fun4AllHistoManager *hm = get_HistoManager();
  assert(hm);

  hm->registerHisto(new TH1F("EMCalAna_h_CEMC_SF", //
      "_h_CEMC_SF", 1000, 0, .2));
  hm->registerHisto(new TH1F("EMCalAna_h_HCALIN_SF", //
      "_h_HCALIN_SF", 1000, 0, .2));
  hm->registerHisto(new TH1F("EMCalAna_h_HCALOUT_SF", //
      "_h_HCALOUT_SF", 1000, 0, .2));
  hm->registerHisto(new TH1F("EMCalAna_h_CEMC_VSF", //
      "_h_CEMC_VSF", 1000, 0, .2));
  hm->registerHisto(new TH1F("EMCalAna_h_HCALIN_VSF", //
      "_h_HCALIN_VSF", 1000, 0, .2));
  hm->registerHisto(new TH1F("EMCalAna_h_HCALOUT_VSF", //
      "_h_HCALOUT_VSF", 1000, 0, .2));

  return Fun4AllReturnCodes::EVENT_OK;
}

int
EMCalAna::process_event_SF(PHCompositeNode *topNode)
{

  if (verbosity > 2)
    cout << "EMCalAna::process_event_SF() entered" << endl;

//  CaloEvalStack caloevalstack(topNode, _caloname);
//  CaloRawClusterEval* clustereval = caloevalstack.get_rawcluster_eval();
//  CaloRawTowerEval* towereval = caloevalstack.get_rawtower_eval();
//  CaloTruthEval* trutheval = caloevalstack.get_truth_eval();

  double e_hcin = 0.0, e_hcout = 0.0, e_cemc = 0.0;
  double ev_hcin = 0.0, ev_hcout = 0.0, ev_cemc = 0.0;
  double ea_hcin = 0.0, ea_hcout = 0.0, ea_cemc = 0.0;

  if (_hcalout_hit_container)
    {
      PHG4HitContainer::ConstRange hcalout_hit_range =
          _hcalout_hit_container->getHits();
      for (PHG4HitContainer::ConstIterator hit_iter = hcalout_hit_range.first;
          hit_iter != hcalout_hit_range.second; hit_iter++)
        {

          PHG4Hit *this_hit = hit_iter->second;
          assert(this_hit);

          e_hcout += this_hit->get_edep();
          ev_hcout += this_hit->get_light_yield();

        }
    }

  if (_hcalin_hit_container)
    {
      PHG4HitContainer::ConstRange hcalin_hit_range =
          _hcalin_hit_container->getHits();
      for (PHG4HitContainer::ConstIterator hit_iter = hcalin_hit_range.first;
          hit_iter != hcalin_hit_range.second; hit_iter++)
        {

          PHG4Hit *this_hit = hit_iter->second;
          assert(this_hit);

          e_hcin += this_hit->get_edep();
          ev_hcin += this_hit->get_light_yield();

        }
    }

  if (_cemc_hit_container)
    {
      PHG4HitContainer::ConstRange cemc_hit_range =
          _cemc_hit_container->getHits();
      for (PHG4HitContainer::ConstIterator hit_iter = cemc_hit_range.first;
          hit_iter != cemc_hit_range.second; hit_iter++)
        {

          PHG4Hit *this_hit = hit_iter->second;
          assert(this_hit);

          e_cemc += this_hit->get_edep();
          ev_cemc += this_hit->get_light_yield();

        }
    }

  if (_hcalout_abs_hit_container)
    {
      PHG4HitContainer::ConstRange hcalout_abs_hit_range =
          _hcalout_abs_hit_container->getHits();
      for (PHG4HitContainer::ConstIterator hit_iter =
          hcalout_abs_hit_range.first; hit_iter != hcalout_abs_hit_range.second;
          hit_iter++)
        {

          PHG4Hit *this_hit = hit_iter->second;
          assert(this_hit);

          ea_hcout += this_hit->get_edep();

        }
    }

  if (_hcalin_abs_hit_container)
    {
      PHG4HitContainer::ConstRange hcalin_abs_hit_range =
          _hcalin_abs_hit_container->getHits();
      for (PHG4HitContainer::ConstIterator hit_iter = hcalin_abs_hit_range.first;
          hit_iter != hcalin_abs_hit_range.second; hit_iter++)
        {

          PHG4Hit *this_hit = hit_iter->second;
          assert(this_hit);

          ea_hcin += this_hit->get_edep();

        }
    }

  if (_cemc_abs_hit_container)
    {
      PHG4HitContainer::ConstRange cemc_abs_hit_range =
          _cemc_abs_hit_container->getHits();
      for (PHG4HitContainer::ConstIterator hit_iter = cemc_abs_hit_range.first;
          hit_iter != cemc_abs_hit_range.second; hit_iter++)
        {

          PHG4Hit *this_hit = hit_iter->second;
          assert(this_hit);

          ea_cemc += this_hit->get_edep();

        }
    }

  Fun4AllHistoManager *hm = get_HistoManager();
  assert(hm);

  TH1F* h = NULL;

  h = (TH1F*) hm->getHisto("EMCalAna_h_CEMC_SF");
  assert(h);
  h->Fill(e_cemc / (e_cemc + ea_cemc) + 1e-9);
  h = (TH1F*) hm->getHisto("EMCalAna_h_CEMC_VSF");
  assert(h);
  h->Fill(ev_cemc / (e_cemc + ea_cemc) + 1e-9);

  h = (TH1F*) hm->getHisto("EMCalAna_h_HCALOUT_SF");
  assert(h);
  h->Fill(e_hcout / (e_hcout + ea_hcout) + 1e-9);
  h = (TH1F*) hm->getHisto("EMCalAna_h_HCALOUT_VSF");
  assert(h);
  h->Fill(ev_hcout / (e_hcout + ea_hcout) + 1e-9);

  h = (TH1F*) hm->getHisto("EMCalAna_h_HCALIN_SF");
  assert(h);
  h->Fill(e_hcin / (e_hcin + ea_hcin) + 1e-9);
  h = (TH1F*) hm->getHisto("EMCalAna_h_HCALIN_VSF");
  assert(h);
  h->Fill(ev_hcin / (e_hcin + ea_hcin) + 1e-9);

  return Fun4AllReturnCodes::EVENT_OK;
}

int
EMCalAna::Init_Tower(PHCompositeNode *topNode)
{

  Fun4AllHistoManager *hm = get_HistoManager();
  assert(hm);

  hm->registerHisto(new TH1F("EMCalAna_h_CEMC_TOWER_1x1", //
      "h_CEMC_TOWER_1x1", 5000, 0, 50));
  hm->registerHisto(new TH1F("EMCalAna_h_CEMC_TOWER_1x1_max", //
      "h_CEMC_TOWER_1x1_max", 5000, 0, 50));

  hm->registerHisto(new TH1F("EMCalAna_h_CEMC_TOWER_2x2", //
      "h_CEMC_TOWER_2x2", 5000, 0, 50));
  hm->registerHisto(new TH1F("EMCalAna_h_CEMC_TOWER_2x2_max", //
      "h_CEMC_TOWER_2x2_max", 5000, 0, 50));

  hm->registerHisto(new TH1F("EMCalAna_h_CEMC_TOWER_3x3", //
      "h_CEMC_TOWER_3x3", 5000, 0, 50));
  hm->registerHisto(new TH1F("EMCalAna_h_CEMC_TOWER_3x3_max", //
      "h_CEMC_TOWER_3x3_max", 5000, 0, 50));

  hm->registerHisto(new TH1F("EMCalAna_h_CEMC_TOWER_4x4", //
      "h_CEMC_TOWER_4x4", 5000, 0, 50));
  hm->registerHisto(new TH1F("EMCalAna_h_CEMC_TOWER_4x4_max", //
      "h_CEMC_TOWER_4x4_max", 5000, 0, 50));

  hm->registerHisto(new TH1F("EMCalAna_h_CEMC_TOWER_5x5", //
      "h_CEMC_TOWER_4x4", 5000, 0, 50));
  hm->registerHisto(new TH1F("EMCalAna_h_CEMC_TOWER_5x5_max", //
      "h_CEMC_TOWER_4x4_max", 5000, 0, 50));

  return Fun4AllReturnCodes::EVENT_OK;
}

int
EMCalAna::process_event_Tower(PHCompositeNode *topNode)
{
  const string detector("CEMC");

  if (verbosity > 2)
    cout << "EMCalAna::process_event_SF() entered" << endl;

  string towernodename = "TOWER_CALIB_" + detector;
  // Grab the towers
  RawTowerContainer* towers = findNode::getClass<RawTowerContainer>(topNode,
      towernodename.c_str());
  if (!towers)
    {
      std::cout << PHWHERE << ": Could not find node " << towernodename.c_str()
          << std::endl;
      return Fun4AllReturnCodes::DISCARDEVENT;
    }
  string towergeomnodename = "TOWERGEOM_" + detector;
  RawTowerGeom *towergeom = findNode::getClass<RawTowerGeom>(topNode,
      towergeomnodename.c_str());
  if (!towergeom)
    {
      cout << PHWHERE << ": Could not find node " << towergeomnodename.c_str()
          << endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  static const int max_size = 5;
  map<int, string> size_label;
  size_label[1] = "1x1";
  size_label[2] = "2x2";
  size_label[3] = "3x3";
  size_label[4] = "4x4";
  size_label[5] = "5x5";
  map<int, double> max_energy;
  map<int, TH1F*> energy_hist_list;
  map<int, TH1F*> max_energy_hist_list;

  Fun4AllHistoManager *hm = get_HistoManager();
  assert(hm);
  for (int size = 1; size <= max_size; ++size)
    {
      max_energy[size] = 0;

      TH1F* h = NULL;

      h = (TH1F*) hm->getHisto("EMCalAna_h_CEMC_TOWER_" + size_label[size]);
      assert(h);
      energy_hist_list[size] = h;
      h = (TH1F*) hm->getHisto(
          "EMCalAna_h_CEMC_TOWER_" + size_label[size] + "_max");
      assert(h);
      max_energy_hist_list[size] = h;
    }

  for (int binphi = 0; binphi < towergeom->get_phibins(); ++binphi)
    {
      for (int bineta = 0; bineta < towergeom->get_etabins(); ++bineta)
        {
          for (int size = 1; size <= max_size; ++size)
            {
              double energy = 0;

              for (int iphi = binphi; iphi < binphi + size; ++iphi)
                {
                  for (int ieta = bineta; ieta < bineta + size; ++ieta)
                    {
                      if (ieta > towergeom->get_etabins())
                        continue;

                      // wrap around
                      int wrapphi = iphi;
                      assert(wrapphi >= 0);
                      if (wrapphi >= towergeom->get_phibins())
                        {
                          wrapphi = wrapphi - towergeom->get_phibins();
                        }

                      RawTower* tower = towers->getTower(ieta, wrapphi);
                      if (tower)
                        {
                          energy += tower->get_energy();
                        }
                    }
                }

              energy_hist_list[size]->Fill(energy);

              if (energy > max_energy[size])
                max_energy[size] = energy;

            } //          for (int size = 1; size <= 4; ++size)
        }
    }

  for (int size = 1; size <= max_size; ++size)
    {
      max_energy_hist_list[size]->Fill(max_energy[size]);
    }
  return Fun4AllReturnCodes::EVENT_OK;
}

