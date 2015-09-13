#include "EMCalAna.h"

#include <g4eval/CaloEvalStack.h>
#include <g4eval/CaloRawClusterEval.h>
#include <g4eval/CaloRawTowerEval.h>
#include <g4eval/CaloTruthEval.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/getClass.h>
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
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

EMCalAna::EMCalAna(const string &name, const string &caloname,
    const string &filename) :
    SubsysReco(name), _caloname(caloname), _ievent(0)
{
  verbosity = 0;
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
EMCalAna::Init(PHCompositeNode *topNode)
{

  _ievent = 0;

  Fun4AllServer *se = Fun4AllServer::instance();
  Fun4AllHistoManager *hm = se->getHistoManager("HISTOS");
  if (!hm)
    {
      hm = new Fun4AllHistoManager("HISTOS");
      se->registerHistoManager(hm);
    }

  hm->registerHisto(new TH1F("EMCalAna_h_CEMC_SF", //
      "_h_CEMC_SF", 1000, 0, 1));
  hm->registerHisto(new TH1F("EMCalAna_h_HCALIN_SF", //
      "_h_HCALIN_SF", 1000, 0, 1));
  hm->registerHisto(new TH1F("EMCalAna_h_HCALOUT_SF", //
      "_h_HCALOUT_SF", 1000, 0, 1));
  hm->registerHisto(new TH1F("EMCalAna_h_CEMC_VSF", //
      "_h_CEMC_VSF", 1000, 0, 1));
  hm->registerHisto(new TH1F("EMCalAna_h_HCALIN_VSF", //
      "_h_HCALIN_VSF", 1000, 0, 1));
  hm->registerHisto(new TH1F("EMCalAna_h_HCALOUT_VSF", //
      "_h_HCALOUT_VSF", 1000, 0, 1));

  return Fun4AllReturnCodes::EVENT_OK;
}

int
EMCalAna::InitRun(PHCompositeNode *topNode)
{

  // get DST objects
  _hcalout_hit_container = findNode::getClass<PHG4HitContainer>(topNode,
      "G4HIT_HCALOUT");
  if (!_hcalout_hit_container)
    {
      std::cout << PHWHERE << ":: No G4HIT_HCALOUT! No sense continuing"
          << std::endl;
      exit(1);
    }

  _hcalin_hit_container = findNode::getClass<PHG4HitContainer>(topNode,
      "G4HIT_HCALIN");
  if (!_hcalin_hit_container)
    {
      std::cout << PHWHERE << ":: No G4HIT_HCALIN! No sense continuing"
          << std::endl;
      exit(1);
    }

  _cemc_hit_container = findNode::getClass<PHG4HitContainer>(topNode,
      "G4HIT_CEMC");
  if (!_cemc_hit_container)
    {
      std::cout << PHWHERE << ":: No G4HIT_CEMC! No sense continuing"
          << std::endl;
      exit(1);
    }

  _hcalout_abs_hit_container = findNode::getClass<PHG4HitContainer>(topNode,
      "G4HIT_ABSORBER_HCALOUT");
  if (!_hcalout_abs_hit_container)
    {
      std::cout << PHWHERE << ":: No G4HIT_HCALOUT! No sense continuing"
          << std::endl;
      exit(1);
    }

  _hcalin_abs_hit_container = findNode::getClass<PHG4HitContainer>(topNode,
      "G4HIT_ABSORBER_HCALIN");
  if (!_hcalin_abs_hit_container)
    {
      std::cout << PHWHERE << ":: No G4HIT_HCALIN! No sense continuing"
          << std::endl;
      exit(1);
    }

  _cemc_abs_hit_container = findNode::getClass<PHG4HitContainer>(topNode,
      "G4HIT_ABSORBER_CEMC");
  if (!_cemc_abs_hit_container)
    {
      std::cout << PHWHERE << ":: No G4HIT_CEMC! No sense continuing"
          << std::endl;
      exit(1);
    }

  return Fun4AllReturnCodes::EVENT_OK;
}

int
EMCalAna::process_event(PHCompositeNode *topNode)
{

  if (verbosity > 2)
    cout << "EMCalAna::process_event() entered" << endl;

//  CaloEvalStack caloevalstack(topNode, _caloname);
//  CaloRawClusterEval* clustereval = caloevalstack.get_rawcluster_eval();
//  CaloRawTowerEval* towereval = caloevalstack.get_rawtower_eval();
//  CaloTruthEval* trutheval = caloevalstack.get_truth_eval();

  double e_hcin = 0.0, e_hcout = 0.0, e_cemc = 0.0;
  double ev_hcin = 0.0, ev_hcout = 0.0, ev_cemc = 0.0;
  double ea_hcin = 0.0, ea_hcout = 0.0, ea_cemc = 0.0;

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

  PHG4HitContainer::ConstRange cemc_hit_range = _cemc_hit_container->getHits();
  for (PHG4HitContainer::ConstIterator hit_iter = cemc_hit_range.first;
      hit_iter != cemc_hit_range.second; hit_iter++)
    {

      PHG4Hit *this_hit = hit_iter->second;
      assert(this_hit);

      e_cemc += this_hit->get_edep();
      ev_cemc += this_hit->get_light_yield();

    }

  PHG4HitContainer::ConstRange hcalout_abs_hit_range =
      _hcalout_abs_hit_container->getHits();
  for (PHG4HitContainer::ConstIterator hit_iter = hcalout_abs_hit_range.first;
      hit_iter != hcalout_abs_hit_range.second; hit_iter++)
    {

      PHG4Hit *this_hit = hit_iter->second;
      assert(this_hit);

      ea_hcout += this_hit->get_edep();

    }

  PHG4HitContainer::ConstRange hcalin_abs_hit_range =
      _hcalin_abs_hit_container->getHits();
  for (PHG4HitContainer::ConstIterator hit_iter = hcalin_abs_hit_range.first;
      hit_iter != hcalin_abs_hit_range.second; hit_iter++)
    {

      PHG4Hit *this_hit = hit_iter->second;
      assert(this_hit);

      ea_hcin += this_hit->get_edep();

    }

  PHG4HitContainer::ConstRange cemc_abs_hit_range =
      _cemc_abs_hit_container->getHits();
  for (PHG4HitContainer::ConstIterator hit_iter = cemc_abs_hit_range.first;
      hit_iter != cemc_abs_hit_range.second; hit_iter++)
    {

      PHG4Hit *this_hit = hit_iter->second;
      assert(this_hit);

      ea_cemc += this_hit->get_edep();

    }

  Fun4AllServer *se = Fun4AllServer::instance();
  Fun4AllHistoManager *hm = se->getHistoManager("HISTOS");
  assert(hm);

  TH1F* h = NULL;

  h = (TH1F*) hm->getHisto("EMCalAna_h_CEMC_SF");
  assert(h);
  h->Fill(e_cemc / (e_cemc + ea_cemc));
  h = (TH1F*) hm->getHisto("EMCalAna_h_CEMC_VSF");
  assert(h);
  h->Fill(ev_cemc / (e_cemc + ea_cemc));

  h = (TH1F*) hm->getHisto("EMCalAna_h_HCALOUT_SF");
  assert(h);
  h->Fill(e_hcout / (e_hcout + ea_hcout));
  h = (TH1F*) hm->getHisto("EMCalAna_h_HCALOUT_VSF");
  assert(h);
  h->Fill(ev_hcout / (e_hcout + ea_hcout));

  h = (TH1F*) hm->getHisto("EMCalAna_h_HCALIN_SF");
  assert(h);
  h->Fill(e_hcin / (e_hcin + ea_hcin));
  h = (TH1F*) hm->getHisto("EMCalAna_h_HCALIN_VSF");
  assert(h);
  h->Fill(ev_hcin / (e_hcin + ea_hcin));

  ++_ievent;

  return Fun4AllReturnCodes::EVENT_OK;
}

int
EMCalAna::End(PHCompositeNode *topNode)
{

  return Fun4AllReturnCodes::EVENT_OK;
}

