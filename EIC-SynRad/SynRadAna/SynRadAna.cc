

#include "SynRadAna.h"

#include <fun4all/Fun4AllReturnCodes.h>

#include <fun4all/Fun4AllHistoManager.h>
#include <fun4all/Fun4AllServer.h>

#include <phhepmc/PHHepMCGenEvent.h>
#include <phhepmc/PHHepMCGenEventMap.h>

#include <g4eval/CaloEvalStack.h>
#include <g4eval/CaloRawClusterEval.h>
#include <g4eval/SvtxEvalStack.h>

#include <g4main/PHG4Hit.h>
#include <g4main/PHG4HitContainer.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4VtxPoint.h>

#include <calobase/RawCluster.h>
#include <calobase/RawTower.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeomContainer.h>

#include <mvtx/CylinderGeom_Mvtx.h>
#include <mvtx/MvtxDefs.h>
//#include <mvtx/MvtxHit.h>
#include <trackbase/TrkrDefs.h>
#include <trackbase/TrkrHit.h>  // for TrkrHit
#include <trackbase/TrkrHitSet.h>
#include <trackbase/TrkrHitSetContainer.h>
#include <trackbase/TrkrHitTruthAssoc.h>
#include <trackbase_historic/SvtxTrack.h>

#include <g4eval/SvtxTrackEval.h>  // for SvtxTrackEval
#include <g4eval/SvtxTruthEval.h>  // for SvtxTruthEval

#include <fun4all/Fun4AllHistoManager.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/SubsysReco.h>

#include <phool/PHCompositeNode.h>
#include <phool/PHDataNode.h>
#include <phool/PHNode.h>  // for PHNode
#include <phool/PHNodeIterator.h>
#include <phool/PHObject.h>
#include <phool/PHRandomSeed.h>
#include <phool/getClass.h>
#include <phool/phool.h>
#include <phool/recoConsts.h>

#include <HepMC/GenEvent.h>
#include <HepMC/GenParticle.h>
#include <HepMC/GenVertex.h>
#include <HepMC/IteratorRange.h>
#include <HepMC/SimpleVector.h>
#include <HepMC/Units.h>

#include <gsl/gsl_const.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>

#include <TAxis.h>
#include <TDatabasePDG.h>
#include <TH1.h>
#include <TH2.h>
#include <TNamed.h>
#include <TString.h>
#include <TVector3.h>

#include <phool/PHCompositeNode.h>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <list>
#include <utility>

using namespace std;

//____________________________________________________________________________..
SynRadAna::SynRadAna(const std::string &fname)
  : SubsysReco("SynRadAna")
  , _embedding_id(1)
  , m_eventWeight(0)
  , do_photon(true)
  , do_MVTXHits(true)
  , do_TPCHits(true)
  , m_outputFIle(fname)
{
  if (Verbosity())
    cout << "SynRadAna::SynRadAna(const std::string &name) Calling ctor" << endl;
}

//____________________________________________________________________________..
SynRadAna::~SynRadAna()
{
  if (Verbosity())
    cout << "SynRadAna::~SynRadAna() Calling dtor" << endl;
}

//____________________________________________________________________________..
int SynRadAna::Init(PHCompositeNode *topNode)
{
  if (Verbosity())
    cout << "SynRadAna::Init(PHCompositeNode *topNode) Initializing" << endl;

  Fun4AllHistoManager *hm = getHistoManager();
  assert(hm);

  {
    TH1 *h(nullptr);
    // n events and n tracks histogram
    h = new TH1D(TString(get_histo_prefix()) + "Normalization",
                 TString(get_histo_prefix()) + " Normalization;Items;Count", 10, .5, 10.5);
    int i = 1;
    h->GetXaxis()->SetBinLabel(i++, "Event");
    h->GetXaxis()->SetBinLabel(i++, "Photon");
    h->GetXaxis()->SetBinLabel(i++, "Flux");
    h->GetXaxis()->SetBinLabel(i++, "G4Particle");
    h->GetXaxis()->SetBinLabel(i++, "G4PrimaryParticle");
    h->GetXaxis()->SetBinLabel(i++, "G4Vertex");
    h->GetXaxis()->LabelsOption("v");
    hm->registerHisto(h);
  }

  for (const auto &hitnode : _node_postfix)
  {
    TH2D *h2(nullptr);

    h2 = new TH2D(TString(get_histo_prefix()) + hitnode + "_nHit",
                  "Hit sum;Sum number of hits", 2000, -.5, 2000 - .5, 2, .5, 2.5);
    //  QAHistManagerDef::useLogBins(h->GetXaxis());
    h2->GetYaxis()->SetBinLabel(1, "Flux");
    h2->GetYaxis()->SetBinLabel(2, "Photon");
    hm->registerHisto(h2);

    h2 = new TH2D(TString(get_histo_prefix()) + hitnode + "_sumEdep",
                  "Hit sum energy distribution;Ionizing energy deposition [keV]", 2000, 0, 100, 2, .5, 2.5);
    //  QAHistManagerDef::useLogBins(h->GetXaxis());
    h2->GetYaxis()->SetBinLabel(1, "Flux");
    h2->GetYaxis()->SetBinLabel(2, "Photon");
    hm->registerHisto(h2);

    h2 = new TH2D(TString(get_histo_prefix()) + hitnode + "_photonEnergy",
                  "Hit source photon;Photon energy [keV]", 2000, 0, 100, 2, .5, 2.5);
    //  QAHistManagerDef::useLogBins(h->GetXaxis());
    h2->GetYaxis()->SetBinLabel(1, "Flux");
    h2->GetYaxis()->SetBinLabel(2, "Photon");
    hm->registerHisto(h2);

    h2 = new TH2D(TString(get_histo_prefix()) + hitnode + "_photonZ",
                  "#gamma z crossing interface facet [cm]", 800, -400, 400, 2, .5, 2.5);
    //  QAHistManagerDef::useLogBins(h->GetXaxis());
    h2->GetYaxis()->SetBinLabel(1, "Flux");
    h2->GetYaxis()->SetBinLabel(2, "Photon");
    hm->registerHisto(h2);
  }

  for (vector<string>::const_iterator it = _tower_postfix.begin();
       it != _tower_postfix.end(); ++it)
  {
  }

  if (do_photon)
  {
    TH2D *h2 = new TH2D(TString(get_histo_prefix()) + "photonEnergy",
                        "Source photon;Photon energy [keV]", 2000, 0, 100, 2, .5, 2.5);
    //  QAHistManagerDef::useLogBins(h->GetXaxis());
    h2->GetYaxis()->SetBinLabel(1, "Flux");
    h2->GetYaxis()->SetBinLabel(2, "Photon");
    hm->registerHisto(h2);

    h2 = new TH2D(TString(get_histo_prefix()) + "photonZ",
                  "#gamma z crossing interface facet [cm]", 800, -400, 400, 2, .5, 2.5);
    //  QAHistManagerDef::useLogBins(h->GetXaxis());
    h2->GetYaxis()->SetBinLabel(1, "Flux");
    h2->GetYaxis()->SetBinLabel(2, "Photon");
    hm->registerHisto(h2);
  }

  if (do_MVTXHits)
  {
    TH2D *h2(nullptr);

    h2 = new TH2D(TString(get_histo_prefix()) + "MVTXHit" + "_nHit",
                  "Hit sum;Sum number of hits", 2000, -.5, 2000 - .5, 2, .5, 2.5);
    //  QAHistManagerDef::useLogBins(h->GetXaxis());
    h2->GetYaxis()->SetBinLabel(1, "Flux");
    h2->GetYaxis()->SetBinLabel(2, "Photon");
    hm->registerHisto(h2);

    h2 = new TH2D(TString(get_histo_prefix()) + "MVTXHit" + "_nHit_Layer",
                  "Hit sum;Sum number of hits;layer", 2000, -.5, 2000 - .5, 3, -.5, 2.5);
    hm->registerHisto(h2);
  }

  if (do_TPCHits)
  {
    TH2D *h2(nullptr);

    h2 = new TH2D(TString(get_histo_prefix()) + "TPCHit" + "_nHit",
                  "Hit sum;Sum number of hits", 2000, -.5, 2000 - .5, 2, .5, 2.5);
    //  QAHistManagerDef::useLogBins(h->GetXaxis());
    h2->GetYaxis()->SetBinLabel(1, "Flux");
    h2->GetYaxis()->SetBinLabel(2, "Photon");
    hm->registerHisto(h2);

    h2 = new TH2D(TString(get_histo_prefix()) + "TPCHit" + "_nHit_Layer",
                  "Hit sum;Sum number of hits;layer", 2000, -.5, 2000 - .5, 61, -.5, 60.5);
    hm->registerHisto(h2);
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int SynRadAna::InitRun(PHCompositeNode *topNode)
{
  if (Verbosity())
    cout << "SynRadAna::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << endl;

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int SynRadAna::process_event(PHCompositeNode *topNode)
{
  if (Verbosity())
    cout << "SynRadAna::process_event(PHCompositeNode *topNode) Processing Event" << endl;

  static const double GeV2keV = 1e6;

  // histogram manager
  Fun4AllHistoManager *hm = getHistoManager();
  assert(hm);
  // n events and n tracks histogram
  TH1 *h_norm = dynamic_cast<TH1 *>(hm->getHisto(get_histo_prefix() + "Normalization"));
  assert(h_norm);
  h_norm->Fill("Event", 1);

  PHG4TruthInfoContainer *truthInfoList = findNode::getClass<
      PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
  assert(truthInfoList);
  h_norm->Fill("G4Particle", truthInfoList->size());
  h_norm->Fill("G4Vertex", truthInfoList->GetNumVertices());
  h_norm->Fill("G4PrimaryParticle", truthInfoList->GetNumPrimaryVertexParticles());

  // For pile-up simulation: define GenEventMap
  PHHepMCGenEventMap *genevtmap = findNode::getClass<PHHepMCGenEventMap>(topNode, "PHHepMCGenEventMap");
  assert(genevtmap);
  PHHepMCGenEvent *genev = genevtmap->get(_embedding_id);
  assert(genev);
  HepMC::GenEvent *hepmc_evt = genev->getEvent();
  assert(hepmc_evt);

  if (Verbosity() >= 2)
  {
    hepmc_evt->print();
  }

  //weight calculation/normalization
  const auto &weightcontainer = hepmc_evt->weights();
  assert(weightcontainer.size() >= 2);
  h_norm->Fill("Flux", weightcontainer[0]);
  h_norm->Fill("Photon", weightcontainer[1]);
  if (weightcontainer[1] != 1)
  {
    cout << __PRETTY_FUNCTION__ << "- warning - weightcontainer[1] = " << weightcontainer[1]
         << " != 1. Can not precisely pin point each photon's flux!"
         << endl;
  }
  m_eventWeight = weightcontainer[0] / weightcontainer[1];

  // initial photons
  if (do_photon)
  {
    TH2 *h_photonEnergy = dynamic_cast<TH2 *>(hm->getHisto(string(get_histo_prefix()) + "photonEnergy"));
    assert(h_photonEnergy);
    TH2 *h_photonZ = dynamic_cast<TH2 *>(hm->getHisto(string(get_histo_prefix()) + "photonZ"));
    assert(h_photonZ);

    PHG4TruthInfoContainer::ConstRange primary_range =
        truthInfoList->GetPrimaryParticleRange();

    for (PHG4TruthInfoContainer::ConstIterator particle_iter =
             primary_range.first;
         particle_iter != primary_range.second;
         ++particle_iter)
    {
      const auto particle = particle_iter->second;

      assert(particle);
      if (particle->get_pid() == 22)
      {
        const double photon_e_keV = particle->get_e() * GeV2keV;
        h_photonEnergy->Fill(photon_e_keV, "Flux", m_eventWeight);
        h_photonEnergy->Fill(photon_e_keV, "Photon", 1);

        PHG4VtxPoint *vtx = truthInfoList->GetVtx(particle->get_vtx_id());
        assert(vtx);

        const double photon_z = vtx->get_z();
        h_photonZ->Fill(photon_z, "Flux", m_eventWeight);
        h_photonZ->Fill(photon_z, "Photon", 1);
      }
      else
      {
        cout << __PRETTY_FUNCTION__ << "- warning - non-photon source!";
        particle->identify();
      }
    }  //          if (_load_all_particle) else
  }

  for (const auto &hitnode : _node_postfix)
  {
    PHG4HitContainer *hits = findNode::getClass<PHG4HitContainer>(topNode,
                                                                  string("G4HIT_" + hitnode));
    if (!hits)
    {
      cout << __PRETTY_FUNCTION__ << " - Fatal Error - missing " << string("G4HIT_" + hitnode) << endl;
    }

    assert(hits);
    PHG4HitContainer::ConstRange hit_range = hits->getHits();

    if (Verbosity() >= 2)
      cout << "SynRadAna::process_event - processing "
           << hitnode << " and received " << hits->size() << " hits"
           << endl;

    int nhit(0);
    double sumEdep_keV(0);

    map<PHG4Particle *, pair<double, double> > primary_photon_map;
    for (PHG4HitContainer::ConstIterator hit_iter = hit_range.first;
         hit_iter != hit_range.second; hit_iter++)
    {
      PHG4Hit *hit = hit_iter->second;
      assert(hit);

      ++nhit;

      const double eion_keV = hit->get_eion() * GeV2keV;
      sumEdep_keV += eion_keV;

      PHG4Particle *hit_particle =
          truthInfoList->GetParticle(hit->get_trkid());
      assert(hit_particle);

      PHG4Particle *primary_particle = truthInfoList->GetParticle(hit_particle->get_primary_id());
      assert(primary_particle);
      if (primary_particle->get_pid() != 22)
      {
        cout << "SynRadAna::process_event - WARNING - unexpected primary particle that is not photon: ";
        primary_particle->identify();

        continue;
      }
      const double photon_e_keV = primary_particle->get_e() * GeV2keV;

      PHG4VtxPoint *vtx = truthInfoList->GetVtx(primary_particle->get_vtx_id());
      assert(vtx);
      const double photon_z = vtx->get_z();

      primary_photon_map.insert(
          make_pair(primary_particle,
                    make_pair(photon_e_keV, photon_z)));
    }

    TH2 *h_nHit = dynamic_cast<TH2 *>(hm->getHisto(string(get_histo_prefix()) + hitnode + "_nHit"));
    assert(h_nHit);

    TH2 *h_sumEdep = dynamic_cast<TH2 *>(hm->getHisto(string(get_histo_prefix()) + hitnode + "_sumEdep"));
    assert(h_sumEdep);

    TH2 *h_photonEnergy = dynamic_cast<TH2 *>(hm->getHisto(string(get_histo_prefix()) + hitnode + "_photonEnergy"));
    assert(h_photonEnergy);

    TH2 *h_photonZ = dynamic_cast<TH2 *>(hm->getHisto(string(get_histo_prefix()) + hitnode + "_photonZ"));
    assert(h_photonZ);

    h_nHit->Fill(nhit, "Flux", m_eventWeight);
    h_nHit->Fill(nhit, "Photon", 1);

    if (nhit > 0)
    {
      if (primary_photon_map.size() != 1)
      {
        cout << "SynRadAna::process_event - WARNING - primary_photon_map.size() = " << primary_photon_map.size() << endl;

        continue;
      }

      for (auto &pair : primary_photon_map)
      {
        const std::pair<double, double> & e_z = pair.second;

        h_photonEnergy->Fill(e_z.first, "Flux", m_eventWeight);
        h_photonEnergy->Fill(e_z.first, "Photon", 1);

        h_photonZ->Fill(e_z.second, "Flux", m_eventWeight);
        h_photonZ->Fill(e_z.second, "Photon", 1);
      }

      h_sumEdep->Fill(sumEdep_keV, "Flux", m_eventWeight);
      h_sumEdep->Fill(sumEdep_keV, "Photon", 1);
    }

  }  //  for (const auto &hitnode : _node_postfix)

  if (do_MVTXHits)
  {
    // Get the TrkrHitSetContainer node
    TrkrHitSetContainer *trkrhitsetcontainer = findNode::getClass<TrkrHitSetContainer>(topNode, "TRKR_HITSET");
    if (!trkrhitsetcontainer)
    {
      cout << "Could not locate TRKR_HITSET node, quit! " << endl;
      exit(1);
    }

    int nhit(0);
    map<int, int> layer_nhit{{0, 0}, {0, 0}, {0, 0}};
    // We want all hitsets for the Mvtx
    TrkrHitSetContainer::ConstRange hitset_range = trkrhitsetcontainer->getHitSets(TrkrDefs::TrkrId::mvtxId);
    for (TrkrHitSetContainer::ConstIterator hitset_iter = hitset_range.first;
         hitset_iter != hitset_range.second;
         ++hitset_iter)
    {
      // we have an itrator to one TrkrHitSet for the mvtx from the trkrHitSetContainer
      // get the hitset key so we can find the layer
      TrkrDefs::hitsetkey hitsetkey = hitset_iter->first;
      int layer = TrkrDefs::getLayer(hitsetkey);
      if (Verbosity() >= 2) cout << __PRETTY_FUNCTION__ << ": found MVTX hitset with key: " << hitsetkey << " in layer " << layer << endl;

      TrkrHitSet *hitset = hitset_iter->second;
      TrkrHitSet::ConstRange hit_range = hitset->getHits();
      for (TrkrHitSet::ConstIterator hit_iter = hit_range.first;
           hit_iter != hit_range.second;
           ++hit_iter)
      {
        TrkrHit *hit = hit_iter->second;
        assert(hit);
        if (Verbosity() >= 2)
        {
          cout << hit->getAdc() << "ADC hit: ";
          hit->identify();
        }

        if (hit->getAdc())
        {
          ++nhit;

          layer_nhit[layer] += 1;
        }
      }
    }

    TH2 *h_nHit = dynamic_cast<TH2 *>(hm->getHisto(string(get_histo_prefix()) + "MVTXHit" + "_nHit"));
    assert(h_nHit);

    TH2 *h_nHit_Layer = dynamic_cast<TH2 *>(hm->getHisto(string(get_histo_prefix()) + "MVTXHit" + "_nHit_Layer"));
    assert(h_nHit_Layer);

    h_nHit->Fill(nhit, "Flux", m_eventWeight);
    h_nHit->Fill(nhit, "Photon", 1);

    for (auto &pair : layer_nhit)
    {
      if (Verbosity() >= 2)
        cout << __PRETTY_FUNCTION__ << ": MVTX Summary: found " << pair.second << " hits in layer " << pair.first << endl;

      h_nHit_Layer->Fill(pair.second, pair.first, m_eventWeight);
    }

  }  //  if (do_MVTXHits)

  if (do_TPCHits)
  {
    // Get the TrkrHitSetContainer node
    TrkrHitSetContainer *trkrhitsetcontainer = findNode::getClass<TrkrHitSetContainer>(topNode, "TRKR_HITSET");
    if (!trkrhitsetcontainer)
    {
      cout << "Could not locate TRKR_HITSET node, quit! " << endl;
      exit(1);
    }

    int nhit(0);
    map<int, int> layer_nhit{};
    // We want all hitsets for the TPC
    TrkrHitSetContainer::ConstRange hitset_range = trkrhitsetcontainer->getHitSets(TrkrDefs::TrkrId::tpcId);
    for (TrkrHitSetContainer::ConstIterator hitset_iter = hitset_range.first;
         hitset_iter != hitset_range.second;
         ++hitset_iter)
    {
      // we have an itrator to one TrkrHitSet for the mvtx from the trkrHitSetContainer
      // get the hitset key so we can find the layer
      TrkrDefs::hitsetkey hitsetkey = hitset_iter->first;
      int layer = TrkrDefs::getLayer(hitsetkey);
      if (Verbosity() >= 2) cout << __PRETTY_FUNCTION__ << ": found TPC hitset with key: " << hitsetkey << " in layer " << layer << endl;

      TrkrHitSet *hitset = hitset_iter->second;
      TrkrHitSet::ConstRange hit_range = hitset->getHits();
      for (TrkrHitSet::ConstIterator hit_iter = hit_range.first;
           hit_iter != hit_range.second;
           ++hit_iter)
      {
        TrkrHit *hit = hit_iter->second;
        assert(hit);
        if (Verbosity() >= 2)
        {
          cout << hit->getAdc() << " ADC hit. " << endl;
        }

        if (hit->getAdc())
        {
          ++nhit;

          if (layer_nhit.find(layer) == layer_nhit.end())
            layer_nhit.insert(make_pair(layer, 0));

          layer_nhit[layer] += 1;
        }
      }
    }

    TH2 *h_nHit = dynamic_cast<TH2 *>(hm->getHisto(string(get_histo_prefix()) + "TPCHit" + "_nHit"));
    assert(h_nHit);

    TH2 *h_nHit_Layer = dynamic_cast<TH2 *>(hm->getHisto(string(get_histo_prefix()) + "TPCHit" + "_nHit_Layer"));
    assert(h_nHit_Layer);

    h_nHit->Fill(nhit, "Flux", m_eventWeight);
    h_nHit->Fill(nhit, "Photon", 1);

    for (auto &pair : layer_nhit)
    {
      if (Verbosity() >= 2)
        cout << __PRETTY_FUNCTION__ << ": TPC summary:  found " << pair.second << " hits in layer " << pair.first << endl;

      h_nHit_Layer->Fill(pair.second, pair.first, m_eventWeight);
    }

  }  //  if (do_TPCHits)

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int SynRadAna::ResetEvent(PHCompositeNode *topNode)
{
  if (Verbosity())
    cout << "SynRadAna::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int SynRadAna::EndRun(const int runnumber)
{
  if (Verbosity())
    cout << "SynRadAna::EndRun(const int runnumber) Ending Run for Run " << runnumber << endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int SynRadAna::End(PHCompositeNode *topNode)
{
  if (Verbosity())
    cout << "SynRadAna::End(PHCompositeNode *topNode) This is the End..." << endl;

  getHistoManager()->dumpHistos(m_outputFIle);

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int SynRadAna::Reset(PHCompositeNode *topNode)
{
  if (Verbosity())
    cout << "SynRadAna::Reset(PHCompositeNode *topNode) being Reset" << endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void SynRadAna::Print(const std::string &what) const
{
  cout << "SynRadAna::Print(const std::string &what) const Printing info for " << what << endl;
}

//! Get a pointer to the default hist manager for QA modules
Fun4AllHistoManager *SynRadAna::
    getHistoManager()
{
  static const string HistoManagerName("HistoManager_SynRadAna");

  Fun4AllServer *se = Fun4AllServer::instance();
  Fun4AllHistoManager *hm = se->getHistoManager(HistoManagerName);

  if (not hm)
  {
    //        cout
    //            << "QAHistManagerDef::get_HistoManager - Making Fun4AllHistoManager EMCalAna_HISTOS"
    //            << endl;
    hm = new Fun4AllHistoManager(HistoManagerName);
    se->registerHistoManager(hm);
  }

  assert(hm);

  return hm;
}

string
SynRadAna::get_histo_prefix()
{
  return string("h_") + Name() + string("_");
}
