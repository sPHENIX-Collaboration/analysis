#include "EMCalAna.h"
#include "UpsilonPair.h"

#include <fun4all/getClass.h>
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/PHTFileServer.h>
#include <phool/PHCompositeNode.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/getClass.h>
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

#include <g4eval/CaloEvalStack.h>
#include <g4eval/CaloRawClusterEval.h>
#include <g4eval/CaloRawTowerEval.h>
#include <g4eval/CaloTruthEval.h>
#include <g4eval/SvtxEvalStack.h>

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

EMCalAna::EMCalAna(const std::string &filename, EMCalAna::enu_flags flags) :
    SubsysReco("EMCalAna"), _eval_stack(NULL), _T_EMCalTrk(NULL), _filename(
        filename), _flags(flags), _ievent(0)
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

EMCalAna::~EMCalAna()
{
  if (_eval_stack)
    {
      delete _eval_stack;
    }
}

int
EMCalAna::InitRun(PHCompositeNode *topNode)
{
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

  if (flag(kProcessUpslisonTrig))
    {
      UpsilonPair * upsilon_pair = findNode::getClass<UpsilonPair>(dstNode,
          "UpsilonPair");

      if (not upsilon_pair)
        {
          upsilon_pair = new UpsilonPair();

          PHIODataNode<PHObject> *node = new PHIODataNode<PHObject>(
              upsilon_pair, "UpsilonPair", "PHObject");
          dstNode->addNode(node);
        }

      assert(upsilon_pair);
    }

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

  // help index files with TChain
  TTree * T_Index = new TTree("T_Index", "T_Index");
  assert(T_Index);
  T_Index->Write();

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
  if (flag(kProcessUpslisonTrig))
    {
      cout << "EMCalAna::Init - Process kProcessUpslisonTrig" << endl;
      Init_UpslisonTrig(topNode);
    }

  return Fun4AllReturnCodes::EVENT_OK;
}

int
EMCalAna::process_event(PHCompositeNode *topNode)
{

  if (verbosity > 2)
    cout << "EMCalAna::process_event() entered" << endl;

  if (_eval_stack)
    _eval_stack->next_event(topNode);

  if (flag(kProcessUpslisonTrig))
    {
      int ret = process_event_UpslisonTrig(topNode);
      if (ret != Fun4AllReturnCodes::EVENT_OK)
        return ret;
    }
  if (flag(kProcessSF))
    process_event_SF(topNode);
  if (flag(kProcessTower))
    process_event_Tower(topNode);
  if (flag(kProcessTrk))
    process_event_Trk(topNode);

  return Fun4AllReturnCodes::EVENT_OK;
}

int
EMCalAna::Init_UpslisonTrig(PHCompositeNode *topNode)
{

  return Fun4AllReturnCodes::EVENT_OK;
}

int
EMCalAna::process_event_UpslisonTrig(PHCompositeNode *topNode)
{

  if (verbosity > 2)
    cout << "EMCalAna::process_event_UpslisonTrig() entered" << endl;
  _trk.Clear();

  UpsilonPair * upsilon_pair = findNode::getClass<UpsilonPair>(topNode,
      "UpsilonPair");
  assert(upsilon_pair);
  static const double upsilon_mass = 9460.30e-3;

  if (!_eval_stack)
    _eval_stack = new SvtxEvalStack(topNode);
//
//  SvtxVertexEval* vertexeval = svtxevalstack.get_vertex_eval();
//  SvtxTrackEval* trackeval = svtxevalstack.get_track_eval();
//  SvtxClusterEval* clustereval = svtxevalstack.get_cluster_eval();
//  SvtxHitEval* hiteval = svtxevalstack.get_hit_eval();
//  SvtxTruthEval* trutheval = svtxevalstack.get_truth_eval();
//
//  SvtxTrackMap* trackmap = findNode::getClass<SvtxTrackMap>(topNode,
//      "SvtxTrackMap");
//  assert(trackmap);
  PHG4TruthInfoContainer* truthinfo =
      findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
  assert(truthinfo);
  PHG4TruthInfoContainer::Map primary_map = truthinfo->GetPrimaryMap();

  set<int> e_pos_candidate;
  set<int> e_neg_candidate;

  for (PHG4TruthInfoContainer::Iterator iter = primary_map.begin();
      iter != primary_map.end(); ++iter)
    {
      PHG4Particle * particle = iter->second;
      assert(particle);

      if (particle->get_pid() == 11)
        e_neg_candidate.insert(particle->get_track_id());
      if (particle->get_pid() == -11)
        e_pos_candidate.insert(particle->get_track_id());
    }

  map<double, pair<int, int> > mass_diff_id_map;
  for (set<int>::const_iterator i_e_pos = e_pos_candidate.begin();
      i_e_pos != e_pos_candidate.end(); ++i_e_pos)
    for (set<int>::const_iterator i_e_neg = e_neg_candidate.begin();
        i_e_neg != e_neg_candidate.end(); ++i_e_neg)
      {
        TLorentzVector vpos(primary_map[*i_e_pos]->get_px(),
            primary_map[*i_e_pos]->get_py(), primary_map[*i_e_pos]->get_pz(),
            primary_map[*i_e_pos]->get_e());
        TLorentzVector vneg(primary_map[*i_e_neg]->get_px(),
            primary_map[*i_e_neg]->get_py(), primary_map[*i_e_neg]->get_pz(),
            primary_map[*i_e_neg]->get_e());

        TLorentzVector invar = vpos + vneg;

        const double mass = invar.M();

        const double mass_diff = fabs(mass - upsilon_mass);

        mass_diff_id_map[mass_diff] = make_pair<int, int>(*i_e_pos, *i_e_neg);
      }

  if (mass_diff_id_map.size() <= 0)
    return Fun4AllReturnCodes::DISCARDEVENT;
  else
    {
      const pair<int, int> best_upsilon_pair = mass_diff_id_map.begin()->second;

      //truth mass
      TLorentzVector gvpos(primary_map[best_upsilon_pair.first]->get_px(),
          primary_map[best_upsilon_pair.first]->get_py(),
          primary_map[best_upsilon_pair.first]->get_pz(),
          primary_map[best_upsilon_pair.first]->get_e());
      TLorentzVector gvneg(primary_map[best_upsilon_pair.second]->get_px(),
          primary_map[best_upsilon_pair.second]->get_py(),
          primary_map[best_upsilon_pair.second]->get_pz(),
          primary_map[best_upsilon_pair.second]->get_e());
      TLorentzVector ginvar = gvpos + gvneg;
      upsilon_pair->gmass = ginvar.M();

      eval_trk(primary_map[best_upsilon_pair.first], *upsilon_pair->get_trk(0),
          topNode);
      eval_trk(primary_map[best_upsilon_pair.second], *upsilon_pair->get_trk(1),
          topNode);

      //calculated mass
      TLorentzVector vpos(upsilon_pair->get_trk(0)->px,
          upsilon_pair->get_trk(0)->py, upsilon_pair->get_trk(0)->pz, 0);
      TLorentzVector vneg(upsilon_pair->get_trk(1)->px,
          upsilon_pair->get_trk(1)->py, upsilon_pair->get_trk(1)->pz, 0);
      vpos.SetE(vpos.P());
      vneg.SetE(vneg.P());
      TLorentzVector invar = vpos + vneg;
      upsilon_pair->mass = invar.M();

      // cuts
      const bool eta_pos_cut = fabs(gvpos.Eta())<1.0;
      const bool eta_neg_cut = fabs(gvneg.Eta())<1.0;
      const bool reco_upsilon_mass = fabs( upsilon_pair->mass - upsilon_mass )<0.2; // two sigma cuts
      upsilon_pair->good_upsilon = eta_pos_cut and eta_neg_cut and reco_upsilon_mass;

      if (not upsilon_pair->good_upsilon )
        {
          return Fun4AllReturnCodes::DISCARDEVENT;
        }

    }

  return Fun4AllReturnCodes::EVENT_OK;
}

void
EMCalAna::eval_trk(PHG4Particle * g4particle, EMCalTrk & trk,
    PHCompositeNode *topNode)
{
  assert(g4particle);

  if (!_eval_stack)
    _eval_stack = new SvtxEvalStack(topNode);
//  SvtxVertexEval* vertexeval = _eval_stack->get_vertex_eval();
  SvtxTrackEval* trackeval = _eval_stack->get_track_eval();
//  SvtxClusterEval* clustereval = _eval_stack->get_cluster_eval();
//  SvtxHitEval* hiteval = _eval_stack->get_hit_eval();
  SvtxTruthEval* trutheval = _eval_stack->get_truth_eval();

  PHG4TruthInfoContainer* truthinfo =
      findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
  assert(truthinfo);

  SvtxClusterMap* clustermap = findNode::getClass<SvtxClusterMap>(topNode,
      "SvtxClusterMap");
  assert(clustermap);

  int gtrackID = g4particle->get_track_id();
  int gflavor = g4particle->get_pid();

  std::set<PHG4Hit*> g4hits = trutheval->all_truth_hits(g4particle);
  int ng4hits = g4hits.size();
  float gpx = g4particle->get_px();
  float gpy = g4particle->get_py();
  float gpz = g4particle->get_pz();

  PHG4VtxPoint* vtx = trutheval->get_vertex(g4particle);
  float gvx = vtx->get_x();
  float gvy = vtx->get_y();
  float gvz = vtx->get_z();

  float gfpx = NULL;
  float gfpy = NULL;
  float gfpz = NULL;
  float gfx = NULL;
  float gfy = NULL;
  float gfz = NULL;

  PHG4Hit* outerhit = trutheval->get_outermost_truth_hit(g4particle);
  if (outerhit)
    {
      gfpx = outerhit->get_px(1);
      gfpy = outerhit->get_py(1);
      gfpz = outerhit->get_pz(1);
      gfx = outerhit->get_x(1);
      gfy = outerhit->get_y(1);
      gfz = outerhit->get_z(1);
    }

  int gembed = trutheval->get_embed(g4particle);

  SvtxTrack* track = trackeval->best_track_from(g4particle);

  float trackID = NAN;
  float charge = NAN;
  float quality = NAN;
  float chisq = NAN;
  float ndf = NAN;
  float nhits = NAN;
  unsigned int layers = 0x0;
  float dca = NAN;
  float dca2d = NAN;
  float dca2dsigma = NAN;
  float px = NAN;
  float py = NAN;
  float pz = NAN;
  float pcax = NAN;
  float pcay = NAN;
  float pcaz = NAN;

  int nfromtruth = -1;

  if (track)
    {
      trackID = track->get_id();
      charge = track->get_charge();
      quality = track->get_quality();
      chisq = track->get_chisq();
      ndf = track->get_ndf();
      nhits = track->size_clusters();

      for (SvtxTrack::ConstClusterIter iter = track->begin_clusters();
          iter != track->end_clusters(); ++iter)
        {
          unsigned int cluster_id = *iter;
          SvtxCluster* cluster = clustermap->get(cluster_id);
          unsigned int layer = cluster->get_layer();
          if (layer < 32)
            layers |= (0x1 << layer);
        }

      dca = track->get_dca();
      dca2d = track->get_dca2d();
      dca2dsigma = track->get_dca2d_error();
      px = track->get_px();
      py = track->get_py();
      pz = track->get_pz();
      pcax = track->get_x();
      pcay = track->get_y();
      pcaz = track->get_z();

      nfromtruth = trackeval->get_nclusters_contribution(track, g4particle);
    }

  trk.gtrackID = gtrackID;
  trk.gflavor = gflavor;
  trk.ng4hits = ng4hits;
  trk.gpx = gpx;
  trk.gpy = gpy;
  trk.gpz = gpz;
  trk.gvx = gvx;
  trk.gvy = gvy;
  trk.gvz = gvz;
  trk.gfpx = gfpx;
  trk.gfpy = gfpy;
  trk.gfpz = gfpz;
  trk.gfx = gfx;
  trk.gfy = gfy;
  trk.gfz = gfz;
  trk.gembed = gembed;
//  trk.gprimary = gprimary;
  trk.trackID = trackID;
  trk.px = px;
  trk.py = py;
  trk.pz = pz;
  trk.charge = charge;
  trk.quality = quality;
  trk.chisq = chisq;
  trk.ndf = ndf;
  trk.nhits = nhits;
  trk.layers = layers;
  trk.dca2d = dca2d;
  trk.dca2dsigma = dca2dsigma;
  trk.pcax = pcax;
  trk.pcay = pcay;
  trk.pcaz = pcaz;
  trk.nfromtruth = nfromtruth;

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
  _trk.Clear();

//  SvtxEvalStack svtxevalstack(topNode);
//
//  SvtxVertexEval* vertexeval = svtxevalstack.get_vertex_eval();
//  SvtxTrackEval* trackeval = svtxevalstack.get_track_eval();
//  SvtxClusterEval* clustereval = svtxevalstack.get_cluster_eval();
//  SvtxHitEval* hiteval = svtxevalstack.get_hit_eval();
//  SvtxTruthEval* trutheval = svtxevalstack.get_truth_eval();
//
//  SvtxTrackMap* trackmap = findNode::getClass<SvtxTrackMap>(topNode,
//      "SvtxTrackMap");
//  assert(trackmap);
//  PHG4TruthInfoContainer* truthinfo =
//      findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
//  assert(truthinfo);
//  PHG4TruthInfoContainer::Map map = truthinfo->GetPrimaryMap();
//
//  for (PHG4TruthInfoContainer::ConstIterator iter = map.begin();
//      iter != map.end(); ++iter)
//    {
//
//    }

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

  hm->registerHisto(new TH2F("EMCalAna_h_CEMC_RZ", //
      "EMCalAna_h_CEMC_RZ;Z (cm);R (cm)", 1200, -300, 300, 600, -000, 300));
  hm->registerHisto(new TH2F("EMCalAna_h_HCALIN_RZ", //
      "EMCalAna_h_HCALIN_RZ;Z (cm);R (cm)", 1200, -300, 300, 600, -000, 300));
  hm->registerHisto(new TH2F("EMCalAna_h_HCALOUT_RZ", //
      "EMCalAna_h_HCALOUT_RZ;Z (cm);R (cm)", 1200, -300, 300, 600, -000, 300));

  hm->registerHisto(new TH2F("EMCalAna_h_CEMC_XY", //
      "EMCalAna_h_CEMC_XY;X (cm);Y (cm)", 1200, -300, 300, 1200, -300, 300));
  hm->registerHisto(new TH2F("EMCalAna_h_HCALIN_XY", //
      "EMCalAna_h_HCALIN_XY;X (cm);Y (cm)", 1200, -300, 300, 1200, -300, 300));
  hm->registerHisto(new TH2F("EMCalAna_h_HCALOUT_XY", //
      "EMCalAna_h_HCALOUT_XY;X (cm);Y (cm)", 1200, -300, 300, 1200, -300, 300));

  return Fun4AllReturnCodes::EVENT_OK;
}

int
EMCalAna::process_event_SF(PHCompositeNode *topNode)
{

  if (verbosity > 2)
    cout << "EMCalAna::process_event_SF() entered" << endl;

  TH1F* h = NULL;

  Fun4AllHistoManager *hm = get_HistoManager();
  assert(hm);

  double e_hcin = 0.0, e_hcout = 0.0, e_cemc = 0.0;
  double ev_hcin = 0.0, ev_hcout = 0.0, ev_cemc = 0.0;
  double ea_hcin = 0.0, ea_hcout = 0.0, ea_cemc = 0.0;

  if (_hcalout_hit_container)
    {
      TH2F * hrz = (TH2F*) hm->getHisto("EMCalAna_h_HCALOUT_RZ");
      assert(hrz);
      TH2F * hxy = (TH2F*) hm->getHisto("EMCalAna_h_HCALOUT_XY");
      assert(hxy);

      PHG4HitContainer::ConstRange hcalout_hit_range =
          _hcalout_hit_container->getHits();
      for (PHG4HitContainer::ConstIterator hit_iter = hcalout_hit_range.first;
          hit_iter != hcalout_hit_range.second; hit_iter++)
        {

          PHG4Hit *this_hit = hit_iter->second;
          assert(this_hit);

          e_hcout += this_hit->get_edep();
          ev_hcout += this_hit->get_light_yield();

          const TVector3 hit(this_hit->get_avg_x(), this_hit->get_avg_y(),
              this_hit->get_avg_z());
          hrz->Fill(hit.Z(), hit.Perp(), this_hit->get_light_yield());
          hxy->Fill(hit.X(), hit.Y(), this_hit->get_light_yield());
        }
    }

  if (_hcalin_hit_container)
    {
      TH2F * hrz = (TH2F*) hm->getHisto("EMCalAna_h_HCALIN_RZ");
      assert(hrz);
      TH2F * hxy = (TH2F*) hm->getHisto("EMCalAna_h_HCALIN_XY");
      assert(hxy);

      PHG4HitContainer::ConstRange hcalin_hit_range =
          _hcalin_hit_container->getHits();
      for (PHG4HitContainer::ConstIterator hit_iter = hcalin_hit_range.first;
          hit_iter != hcalin_hit_range.second; hit_iter++)
        {

          PHG4Hit *this_hit = hit_iter->second;
          assert(this_hit);

          e_hcin += this_hit->get_edep();
          ev_hcin += this_hit->get_light_yield();

          const TVector3 hit(this_hit->get_avg_x(), this_hit->get_avg_y(),
              this_hit->get_avg_z());
          hrz->Fill(hit.Z(), hit.Perp(), this_hit->get_light_yield());
          hxy->Fill(hit.X(), hit.Y(), this_hit->get_light_yield());

        }
    }

  if (_cemc_hit_container)
    {
      TH2F * hrz = (TH2F*) hm->getHisto("EMCalAna_h_CEMC_RZ");
      assert(hrz);
      TH2F * hxy = (TH2F*) hm->getHisto("EMCalAna_h_CEMC_XY");
      assert(hxy);

      PHG4HitContainer::ConstRange cemc_hit_range =
          _cemc_hit_container->getHits();
      for (PHG4HitContainer::ConstIterator hit_iter = cemc_hit_range.first;
          hit_iter != cemc_hit_range.second; hit_iter++)
        {

          PHG4Hit *this_hit = hit_iter->second;
          assert(this_hit);

          e_cemc += this_hit->get_edep();
          ev_cemc += this_hit->get_light_yield();

          const TVector3 hit(this_hit->get_avg_x(), this_hit->get_avg_y(),
              this_hit->get_avg_z());
          hrz->Fill(hit.Z(), hit.Perp(), this_hit->get_light_yield());
          hxy->Fill(hit.X(), hit.Y(), this_hit->get_light_yield());
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

