#include "EMCalCalib.h"
#include "PhotonPair.h"

#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/PHTFileServer.h>
#include <phool/PHCompositeNode.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/getClass.h>
#include <fun4all/Fun4AllHistoManager.h>

#include <phool/PHCompositeNode.h>

#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4VtxPoint.h>
#include <g4main/PHG4Particle.h>

#include <g4hough/SvtxVertexMap.h>

#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/RawTower.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawCluster.h>

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

EMCalCalib::EMCalCalib(const std::string &filename, EMCalCalib::enu_flags flags) :
    SubsysReco("EMCalCalib"), _eval_stack(NULL), _T_EMCalTrk(NULL), _mc_photon(
        NULL), //
    _magfield(1.5), //
    _filename(filename), _flags(flags), _ievent(0)
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

EMCalCalib::~EMCalCalib()
{
  if (_eval_stack)
    {
      delete _eval_stack;
    }
}

int
EMCalCalib::InitRun(PHCompositeNode *topNode)
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
//      PhotonPair * photon_pair = findNode::getClass<PhotonPair>(dstNode,
//          "PhotonPair");
//
//      if (not photon_pair)
//        {
//          photon_pair = new PhotonPair();
//
//          PHIODataNode<PHObject> *node = new PHIODataNode<PHObject>(
//              photon_pair, "PhotonPair", "PHObject");
//          dstNode->addNode(node);
//        }
//
//      assert(photon_pair);
    }
  if (flag(kProcessMCPhoton))
    {
      _mc_photon = findNode::getClass<MCPhoton>(dstNode, "MCPhoton");

      if (not _mc_photon)
        {
          _mc_photon = new MCPhoton();

          PHIODataNode<PHObject> *node = new PHIODataNode<PHObject>(_mc_photon,
              "MCPhoton", "PHObject");
          dstNode->addNode(node);
        }

      assert(_mc_photon);
    }

  return Fun4AllReturnCodes::EVENT_OK;
}

int
EMCalCalib::End(PHCompositeNode *topNode)
{
  cout << "EMCalCalib::End - write to " << _filename << endl;
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
EMCalCalib::Init(PHCompositeNode *topNode)
{

  _ievent = 0;

  cout << "EMCalCalib::get_HistoManager - Making PHTFileServer " << _filename
      << endl;
  PHTFileServer::get().open(_filename, "RECREATE");

  if (flag(kProcessSF))
    {
      cout << "EMCalCalib::Init - Process sampling fraction" << endl;
      Init_SF(topNode);
    }
  if (flag(kProcessTower))
    {
      cout << "EMCalCalib::Init - Process tower occupancies" << endl;
      Init_Tower(topNode);
    }
  if (flag(kProcessMCPhoton))
    {
      cout << "EMCalCalib::Init - Process trakcs" << endl;
      Init_MCPhoton(topNode);
    }
  if (flag(kProcessUpslisonTrig))
    {
      cout << "EMCalCalib::Init - Process kProcessUpslisonTrig" << endl;
      Init_UpslisonTrig(topNode);
    }

  return Fun4AllReturnCodes::EVENT_OK;
}

int
EMCalCalib::process_event(PHCompositeNode *topNode)
{

  if (verbosity > 2)
    cout << "EMCalCalib::process_event() entered" << endl;

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
  if (flag(kProcessMCPhoton))
    process_event_MCPhoton(topNode);

  return Fun4AllReturnCodes::EVENT_OK;
}

int
EMCalCalib::Init_UpslisonTrig(PHCompositeNode *topNode)
{

  return Fun4AllReturnCodes::EVENT_OK;
}

int
EMCalCalib::process_event_UpslisonTrig(PHCompositeNode *topNode)
{

  if (verbosity > 2)
    cout << "EMCalCalib::process_event_UpslisonTrig() entered" << endl;

//  PhotonPair * photon_pair = findNode::getClass<PhotonPair>(topNode,
//      "PhotonPair");
//  assert(photon_pair);
//  static const double upsilon_mass = 9460.30e-3;

//  if (!_eval_stack)
//    {
//      _eval_stack = new SvtxEvalStack(topNode);
//      _eval_stack->set_strict(false);
//    }
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
//  PHG4TruthInfoContainer::Map particle_map = truthinfo->GetMap();
//  PHG4TruthInfoContainer::ConstRange primary_range =
//      truthinfo->GetPrimaryParticleRange();
//
//  set<int> e_pos_candidate;
//  set<int> e_neg_candidate;
//
//  for (PHG4TruthInfoContainer::ConstIterator iter = primary_range.first;
//      iter != primary_range.second; ++iter)
//    {
//
//      PHG4Particle * particle = iter->second;
//      assert(particle);
//
//      if (particle->get_pid() == 11)
//        e_neg_candidate.insert(particle->get_track_id());
//      if (particle->get_pid() == -11)
//        e_pos_candidate.insert(particle->get_track_id());
//    }
//
//  map<double, pair<int, int> > mass_diff_id_map;
//  for (set<int>::const_iterator i_e_pos = e_pos_candidate.begin();
//      i_e_pos != e_pos_candidate.end(); ++i_e_pos)
//    for (set<int>::const_iterator i_e_neg = e_neg_candidate.begin();
//        i_e_neg != e_neg_candidate.end(); ++i_e_neg)
//      {
//        TLorentzVector vpos(particle_map[*i_e_pos]->get_px(),
//            particle_map[*i_e_pos]->get_py(), particle_map[*i_e_pos]->get_pz(),
//            particle_map[*i_e_pos]->get_e());
//        TLorentzVector vneg(particle_map[*i_e_neg]->get_px(),
//            particle_map[*i_e_neg]->get_py(), particle_map[*i_e_neg]->get_pz(),
//            particle_map[*i_e_neg]->get_e());
//
//        TLorentzVector invar = vpos + vneg;
//
//        const double mass = invar.M();
//
//        const double mass_diff = fabs(mass - upsilon_mass);
//
//        mass_diff_id_map[mass_diff] = make_pair<int, int>(*i_e_pos, *i_e_neg);
//      }
//
//  if (mass_diff_id_map.size() <= 0)
//    return Fun4AllReturnCodes::DISCARDEVENT;
//  else
//    {
//      const pair<int, int> best_upsilon_pair = mass_diff_id_map.begin()->second;
//
//      //truth mass
//      TLorentzVector gvpos(particle_map[best_upsilon_pair.first]->get_px(),
//          particle_map[best_upsilon_pair.first]->get_py(),
//          particle_map[best_upsilon_pair.first]->get_pz(),
//          particle_map[best_upsilon_pair.first]->get_e());
//      TLorentzVector gvneg(particle_map[best_upsilon_pair.second]->get_px(),
//          particle_map[best_upsilon_pair.second]->get_py(),
//          particle_map[best_upsilon_pair.second]->get_pz(),
//          particle_map[best_upsilon_pair.second]->get_e());
//      TLorentzVector ginvar = gvpos + gvneg;
//      photon_pair->gmass = ginvar.M();
//
//      eval_photon(particle_map[best_upsilon_pair.first], *photon_pair->get_trk(0),
//          topNode);
//      eval_photon(particle_map[best_upsilon_pair.second],
//          *photon_pair->get_trk(1), topNode);
//
//      //calculated mass
//      TLorentzVector vpos(photon_pair->get_trk(0)->px,
//          photon_pair->get_trk(0)->py, photon_pair->get_trk(0)->pz, 0);
//      TLorentzVector vneg(photon_pair->get_trk(1)->px,
//          photon_pair->get_trk(1)->py, photon_pair->get_trk(1)->pz, 0);
//      vpos.SetE(vpos.P());
//      vneg.SetE(vneg.P());
//      TLorentzVector invar = vpos + vneg;
//      photon_pair->mass = invar.M();
//
//      // cuts
//      const bool eta_pos_cut = fabs(gvpos.Eta()) < 1.0;
//      const bool eta_neg_cut = fabs(gvneg.Eta()) < 1.0;
//      const bool reco_upsilon_mass = fabs(photon_pair->mass - upsilon_mass)
//          < 0.2; // two sigma cuts
//      photon_pair->good_upsilon = eta_pos_cut and eta_neg_cut
//          and reco_upsilon_mass;
//
//      if (not photon_pair->good_upsilon)
//        {
//          return Fun4AllReturnCodes::DISCARDEVENT;
//        }
//
//    }

  return Fun4AllReturnCodes::EVENT_OK;
}

//! project a photon to calorimeters and collect towers around it
void
EMCalCalib::eval_photon(PHG4Particle * g4particle, MCPhoton & mc_photon,
    PHCompositeNode *topNode)
{
  assert(g4particle);

  if (!_eval_stack)
    {
      _eval_stack = new SvtxEvalStack(topNode);
      _eval_stack->set_strict(false);
    }
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

      mc_photon.presdphi = track->get_cal_dphi(SvtxTrack::PRES);
      mc_photon.presdeta = track->get_cal_deta(SvtxTrack::PRES);
      mc_photon.prese3x3 = track->get_cal_energy_3x3(SvtxTrack::PRES);
      mc_photon.prese = track->get_cal_cluster_e(SvtxTrack::PRES);

      mc_photon.cemcdphi = track->get_cal_dphi(SvtxTrack::CEMC);
      mc_photon.cemcdeta = track->get_cal_deta(SvtxTrack::CEMC);
      mc_photon.cemce3x3 = track->get_cal_energy_3x3(SvtxTrack::CEMC);
      mc_photon.cemce = track->get_cal_cluster_e(SvtxTrack::CEMC);

      mc_photon.hcalindphi = track->get_cal_dphi(SvtxTrack::HCALIN);
      mc_photon.hcalindeta = track->get_cal_deta(SvtxTrack::HCALIN);
      mc_photon.hcaline3x3 = track->get_cal_energy_3x3(SvtxTrack::HCALIN);
      mc_photon.hcaline = track->get_cal_cluster_e(SvtxTrack::HCALIN);

      mc_photon.hcaloutdphi = track->get_cal_dphi(SvtxTrack::HCALOUT);
      mc_photon.hcaloutdeta = track->get_cal_deta(SvtxTrack::HCALOUT);
      mc_photon.hcaloute3x3 = track->get_cal_energy_3x3(SvtxTrack::HCALOUT);
      mc_photon.hcaloute = track->get_cal_cluster_e(SvtxTrack::HCALOUT);

    }

  eval_photon_proj( //
      /*PHG4Particle **/g4particle, //
      /*MCPhoton &*/mc_photon,
      /*enu_calo*/kCEMC, //
      /*const double */gvz,
      /*PHCompositeNode **/topNode //
      );
  eval_photon_proj( //
      /*PHG4Particle **/g4particle, //
      /*MCPhoton &*/mc_photon,
      /*enu_calo*/kHCALIN, //
      /*const double */gvz,
      /*PHCompositeNode **/topNode //
      );

  mc_photon.gtrackID = gtrackID;
  mc_photon.gflavor = gflavor;
  mc_photon.ng4hits = ng4hits;
  mc_photon.gpx = gpx;
  mc_photon.gpy = gpy;
  mc_photon.gpz = gpz;
  mc_photon.gvx = gvx;
  mc_photon.gvy = gvy;
  mc_photon.gvz = gvz;
  mc_photon.gfpx = gfpx;
  mc_photon.gfpy = gfpy;
  mc_photon.gfpz = gfpz;
  mc_photon.gfx = gfx;
  mc_photon.gfy = gfy;
  mc_photon.gfz = gfz;
  mc_photon.gembed = gembed;
//  trk.gprimary = gprimary;
  mc_photon.trackID = trackID;
  mc_photon.px = px;
  mc_photon.py = py;
  mc_photon.pz = pz;
  mc_photon.charge = charge;
  mc_photon.quality = quality;
  mc_photon.chisq = chisq;
  mc_photon.ndf = ndf;
  mc_photon.nhits = nhits;
  mc_photon.layers = layers;
  mc_photon.dca2d = dca2d;
  mc_photon.dca2dsigma = dca2dsigma;
  mc_photon.pcax = pcax;
  mc_photon.pcay = pcay;
  mc_photon.pcaz = pcaz;
  mc_photon.nfromtruth = nfromtruth;

}

void
EMCalCalib::eval_photon_proj( //
    PHG4Particle * g4particle, //
    MCPhoton & trk, enu_calo calo_id, //
    const double gvz, PHCompositeNode *topNode //
    )
// Track projections
{
  // hard coded radius
  string detector = "InvalidDetector";
  double radius = 110;
  if (calo_id == kCEMC)
    {
      detector = "CEMC";
      radius = 105;
    }
  if (calo_id == kHCALIN)
    {
      detector = "HCALIN";
      radius = 125;
    }

  if (!_eval_stack)
    {
      _eval_stack = new SvtxEvalStack(topNode);
      _eval_stack->set_strict(false);
    }

  // pull the tower geometry
  string towergeonodename = "TOWERGEOM_" + detector;
  RawTowerGeomContainer *cemc_towergeo = findNode::getClass<
      RawTowerGeomContainer>(topNode, towergeonodename.c_str());
  assert(cemc_towergeo);

  if (verbosity > 1)
    {
      cemc_towergeo->identify();
    }
  // pull the towers
  string towernodename = "TOWER_CALIB_" + detector;
  RawTowerContainer *cemc_towerList = findNode::getClass<RawTowerContainer>(
      topNode, towernodename.c_str());
  assert(cemc_towerList);

  // curved tracks inside mag field
  // straight projections thereafter
  std::vector<double> point;
//  point.assign(3, -9999.);
//  _hough.projectToRadius(track, _magfield, radius, point);

  const double pt = sqrt(
      g4particle->get_px() * g4particle->get_px()
          + g4particle->get_py() * g4particle->get_py());

  double x = g4particle->get_px() / pt * radius;
  double y = g4particle->get_py() / pt * radius;
  double z = g4particle->get_pz() / pt * radius + gvz;

  double phi = atan2(y, x);
  double eta = asinh(z / sqrt(x * x + y * y));

  // calculate 3x3 tower energy
  int binphi = cemc_towergeo->get_phibin(phi);
  int bineta = cemc_towergeo->get_etabin(eta);

  double etabin_width = cemc_towergeo->get_etabounds(bineta).second
      - cemc_towergeo->get_etabounds(bineta).first;
  if (bineta > 1 and bineta < cemc_towergeo->get_etabins() - 1)
    etabin_width = (cemc_towergeo->get_etacenter(bineta + 1)
        - cemc_towergeo->get_etacenter(bineta - 1)) / 2.;

  double phibin_width = cemc_towergeo->get_phibounds(binphi).second
      - cemc_towergeo->get_phibounds(binphi).first;

  assert(etabin_width>0);
  assert(phibin_width>0);

  const double etabin_shift = (cemc_towergeo->get_etacenter(bineta) - eta)
      / etabin_width;
  const double phibin_shift = (fmod(
      cemc_towergeo->get_phicenter(binphi) - phi + 5 * M_PI, 2 * M_PI) - M_PI)
      / phibin_width;

  if (verbosity > 1)
    cout << __PRETTY_FUNCTION__ << " - info - handling track proj. (" << x
        << ", " << y << ", " << z << ")" << ", eta " << eta << ", phi" << phi
        << ", bineta " << bineta << " - ["
        << cemc_towergeo->get_etabounds(bineta).first << ", "
        << cemc_towergeo->get_etabounds(bineta).second << "], etabin_shift = "
        << etabin_shift << ", binphi " << binphi << " - ["
        << cemc_towergeo->get_phibounds(binphi).first << ", "
        << cemc_towergeo->get_phibounds(binphi).second << "], phibin_shift = "
        << phibin_shift << endl;

  const int bin_search_range = (trk.Max_N_Tower - 1) / 2;
  for (int iphi = binphi - bin_search_range; iphi <= binphi + bin_search_range;
      ++iphi)

    for (int ieta = bineta - bin_search_range;
        ieta <= bineta + bin_search_range; ++ieta)
      {

        // wrap around
        int wrapphi = iphi;
        if (wrapphi < 0)
          {
            wrapphi = cemc_towergeo->get_phibins() + wrapphi;
          }
        if (wrapphi >= cemc_towergeo->get_phibins())
          {
            wrapphi = wrapphi - cemc_towergeo->get_phibins();
          }

        // edges
        if (ieta < 0)
          continue;
        if (ieta >= cemc_towergeo->get_etabins())
          continue;

        if (verbosity > 1)
          cout << __PRETTY_FUNCTION__ << " - info - processing tower"
              << ", bineta " << ieta << " - ["
              << cemc_towergeo->get_etabounds(ieta).first << ", "
              << cemc_towergeo->get_etabounds(ieta).second << "]" << ", binphi "
              << wrapphi << " - ["
              << cemc_towergeo->get_phibounds(wrapphi).first << ", "
              << cemc_towergeo->get_phibounds(wrapphi).second << "]" << endl;

        RawTower* tower = cemc_towerList->getTower(ieta, wrapphi);
        double energy = 0;
        if (tower)
          {
            if (verbosity > 1)
              cout << __PRETTY_FUNCTION__ << " - info - tower " << ieta << " "
                  << wrapphi << " energy = " << tower->get_energy() << endl;

            energy = tower->get_energy();
          }

        if (calo_id == kCEMC)
          {
            trk.cemc_ieta //
            [ieta - (bineta - bin_search_range)] //
            [iphi - (binphi - bin_search_range)] //
            = ieta - bineta + etabin_shift;
            trk.cemc_iphi //
            [ieta - (bineta - bin_search_range)] //
            [iphi - (binphi - bin_search_range)] //
            = iphi - binphi + phibin_shift;
            trk.cemc_energy //
            [ieta - (bineta - bin_search_range)] //
            [iphi - (binphi - bin_search_range)] //
            = energy;
          }
        if (calo_id == kHCALIN)
          {
            trk.hcalin_ieta //
            [ieta - (bineta - bin_search_range)] //
            [iphi - (binphi - bin_search_range)] //
            = ieta - bineta + etabin_shift;
            trk.hcalin_iphi //
            [ieta - (bineta - bin_search_range)] //
            [iphi - (binphi - bin_search_range)] //
            = iphi - binphi + phibin_shift;
            trk.hcalin_energy //
            [ieta - (bineta - bin_search_range)] //
            [iphi - (binphi - bin_search_range)] //
            = energy;
          }

      } //            for (int ieta = bineta-1; ieta < bineta+2; ++ieta) {

} //       // Track projections

int
EMCalCalib::Init_MCPhoton(PHCompositeNode *topNode)
{
//  static const int BUFFER_SIZE = 32000;
//  _T_EMCalTrk = new TTree("T_EMCalTrk", "T_EMCalTrk");
//
//  _T_EMCalTrk->Branch("trk.", _trk.ClassName(), &_trk, BUFFER_SIZE);
//  _T_EMCalTrk->Branch("event", &_ievent, "event/I", BUFFER_SIZE);

  return Fun4AllReturnCodes::EVENT_OK;
}

int
EMCalCalib::process_event_MCPhoton(PHCompositeNode *topNode)
{

  if (verbosity > 2)
    cout << "EMCalCalib::process_event_MCPhoton() entered" << endl;

  if (!_eval_stack)
    {
      _eval_stack = new SvtxEvalStack(topNode);
      _eval_stack->set_strict(false);
    }

  _mc_photon = findNode::getClass<MCPhoton>(topNode, "MCPhoton");
  assert(_mc_photon);

  PHG4TruthInfoContainer* truthinfo =
      findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
  assert(truthinfo);
  PHG4TruthInfoContainer::ConstRange range =
      truthinfo->GetPrimaryParticleRange();

  assert(range.first != range.second);
  // make sure there is at least one primary partcle

  eval_photon(range.first->second, *_mc_photon, topNode);

  return Fun4AllReturnCodes::EVENT_OK;
}

Fun4AllHistoManager *
EMCalCalib::get_HistoManager()
{

  Fun4AllServer *se = Fun4AllServer::instance();
  Fun4AllHistoManager *hm = se->getHistoManager("EMCalAna_HISTOS");

  if (not hm)
    {
      cout
          << "EMCalCalib::get_HistoManager - Making Fun4AllHistoManager EMCalAna_HISTOS"
          << endl;
      hm = new Fun4AllHistoManager("EMCalAna_HISTOS");
      se->registerHistoManager(hm);
    }

  assert(hm);

  return hm;
}

int
EMCalCalib::Init_SF(PHCompositeNode *topNode)
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
EMCalCalib::process_event_SF(PHCompositeNode *topNode)
{

  if (verbosity > 2)
    cout << "EMCalCalib::process_event_SF() entered" << endl;

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
EMCalCalib::Init_Tower(PHCompositeNode *topNode)
{

  Fun4AllHistoManager *hm = get_HistoManager();
  assert(hm);

  hm->registerHisto(new TH1F("EMCalAna_h_CEMC_TOWER_1x1", //
      "h_CEMC_TOWER_1x1", 5000, 0, 50));
  hm->registerHisto(new TH1F("EMCalAna_h_CEMC_TOWER_1x1_max", //
      "EMCalAna_h_CEMC_TOWER_1x1_max", 5000, 0, 50));
  hm->registerHisto(new TH1F("EMCalAna_h_CEMC_TOWER_1x1_max_trigger_ADC", //
      "EMCalAna_h_CEMC_TOWER_1x1_max_trigger_ADC", 5000, 0, 50));

  hm->registerHisto(new TH1F("EMCalAna_h_CEMC_TOWER_2x2", //
      "h_CEMC_TOWER_2x2", 5000, 0, 50));
  hm->registerHisto(new TH1F("EMCalAna_h_CEMC_TOWER_2x2_max", //
      "EMCalAna_h_CEMC_TOWER_2x2_max", 5000, 0, 50));
  hm->registerHisto(new TH1F("EMCalAna_h_CEMC_TOWER_2x2_max_trigger_ADC", //
      "EMCalAna_h_CEMC_TOWER_2x2_max_trigger_ADC", 5000, 0, 50));
  hm->registerHisto(new TH1F("EMCalAna_h_CEMC_TOWER_2x2_slide2_max_trigger_ADC", //
      "EMCalAna_h_CEMC_TOWER_2x2_slide2_max_trigger_ADC", 5000, 0, 50));

  hm->registerHisto(new TH1F("EMCalAna_h_CEMC_TOWER_3x3", //
      "h_CEMC_TOWER_3x3", 5000, 0, 50));
  hm->registerHisto(new TH1F("EMCalAna_h_CEMC_TOWER_3x3_max", //
      "EMCalAna_h_CEMC_TOWER_3x3_max", 5000, 0, 50));
  hm->registerHisto(new TH1F("EMCalAna_h_CEMC_TOWER_3x3_max_trigger_ADC", //
      "EMCalAna_h_CEMC_TOWER_3x3_max_trigger_ADC", 5000, 0, 50));

  hm->registerHisto(new TH1F("EMCalAna_h_CEMC_TOWER_4x4", //
      "h_CEMC_TOWER_4x4", 5000, 0, 50));
  hm->registerHisto(new TH1F("EMCalAna_h_CEMC_TOWER_4x4_max", //
      "EMCalAna_h_CEMC_TOWER_4x4_max", 5000, 0, 50));
  hm->registerHisto(new TH1F("EMCalAna_h_CEMC_TOWER_4x4_max_trigger_ADC", //
      "EMCalAna_h_CEMC_TOWER_4x4_max_trigger_ADC", 5000, 0, 50));
  hm->registerHisto(new TH1F("EMCalAna_h_CEMC_TOWER_4x4_slide2_max_trigger_ADC", //
      "EMCalAna_h_CEMC_TOWER_4x4_slide2_max_trigger_ADC", 5000, 0, 50));

  hm->registerHisto(new TH1F("EMCalAna_h_CEMC_TOWER_5x5", //
      "h_CEMC_TOWER_4x4", 5000, 0, 50));
  hm->registerHisto(new TH1F("EMCalAna_h_CEMC_TOWER_5x5_max", //
      "EMCalAna_h_CEMC_TOWER_5x5_max", 5000, 0, 50));
  hm->registerHisto(new TH1F("EMCalAna_h_CEMC_TOWER_5x5_max_trigger_ADC", //
      "EMCalAna_h_CEMC_TOWER_5x5_max_trigger_ADC", 5000, 0, 50));

  return Fun4AllReturnCodes::EVENT_OK;
}

int
EMCalCalib::process_event_Tower(PHCompositeNode *topNode)
{
  const string detector("CEMC");

  if (verbosity > 2)
    cout << "EMCalCalib::process_event_SF() entered" << endl;

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
  RawTowerGeomContainer *towergeom = findNode::getClass<RawTowerGeomContainer>(
      topNode, towergeomnodename.c_str());
  if (!towergeom)
    {
      cout << PHWHERE << ": Could not find node " << towergeomnodename.c_str()
          << endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  static const double trigger_ADC_bin = 45. / 256.; //8-bit ADC max to 45 GeV
  static const int max_size = 5;
  map<int, string> size_label;
  size_label[1] = "1x1";
  size_label[2] = "2x2";
  size_label[3] = "3x3";
  size_label[4] = "4x4";
  size_label[5] = "5x5";
  map<int, double> max_energy;
  map<int, double> max_energy_trigger_ADC;
  map<int, double> slide2_max_energy_trigger_ADC;
  map<int, TH1F*> energy_hist_list;
  map<int, TH1F*> max_energy_hist_list;
  map<int, TH1F*> max_energy_trigger_ADC_hist_list;
  map<int, TH1F*> slide2_max_energy_trigger_ADC_hist_list;

  Fun4AllHistoManager *hm = get_HistoManager();
  assert(hm);
  for (int size = 1; size <= max_size; ++size)
    {
      max_energy[size] = 0;
      max_energy_trigger_ADC[size] = 0;

      TH1F* h = NULL;

      h = (TH1F*) hm->getHisto("EMCalAna_h_CEMC_TOWER_" + size_label[size]);
      assert(h);
      energy_hist_list[size] = h;
      h = (TH1F*) hm->getHisto(
          "EMCalAna_h_CEMC_TOWER_" + size_label[size] + "_max");
      assert(h);
      max_energy_hist_list[size] = h;

      h = (TH1F*) hm->getHisto(
          "EMCalAna_h_CEMC_TOWER_" + size_label[size] + "_max_trigger_ADC");
      assert(h);
      max_energy_trigger_ADC_hist_list[size] = h;

      if (size == 2 or size == 4)
        {
          // sliding window made from 2x2 sums
          slide2_max_energy_trigger_ADC[size] = 0;

          h = (TH1F*) hm->getHisto(
              "EMCalAna_h_CEMC_TOWER_" + size_label[size]
                  + "_slide2_max_trigger_ADC");
          assert(h);
          slide2_max_energy_trigger_ADC_hist_list[size] = h;

        }

    }

  for (int binphi = 0; binphi < towergeom->get_phibins(); ++binphi)
    {
      for (int bineta = 0; bineta < towergeom->get_etabins(); ++bineta)
        {
          for (int size = 1; size <= max_size; ++size)
            {
              double energy = 0;
              double energy_trigger_ADC = 0;
              double slide2_energy_trigger_ADC = 0;

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
                          const double e_intput = tower->get_energy();

                          const double e_trigger_ADC = round(
                              e_intput / trigger_ADC_bin) * trigger_ADC_bin;

                          energy += e_intput;
                          energy_trigger_ADC += e_trigger_ADC;

                          if ((size == 2 or size == 4) and (binphi % 2 == 0)
                              and (bineta % 2 == 0))
                            {
                              // sliding window made from 2x2 sums

                              slide2_energy_trigger_ADC += e_trigger_ADC;
                            }
                        }
                    }
                }

              energy_hist_list[size]->Fill(energy);

              if (energy > max_energy[size])
                max_energy[size] = energy;
              if (energy_trigger_ADC > max_energy_trigger_ADC[size])
                max_energy_trigger_ADC[size] = energy_trigger_ADC;

              if ((size == 2 or size == 4) and (binphi % 2 == 0)
                  and (bineta % 2 == 0))
                {
                  // sliding window made from 2x2 sums

                  if (slide2_energy_trigger_ADC
                      > slide2_max_energy_trigger_ADC[size])
                    slide2_max_energy_trigger_ADC[size] =
                        slide2_energy_trigger_ADC;
                }

            } //          for (int size = 1; size <= 4; ++size)
        }
    }

  for (int size = 1; size <= max_size; ++size)
    {
      max_energy_hist_list[size]->Fill(max_energy[size]);
      max_energy_trigger_ADC_hist_list[size]->Fill(
          max_energy_trigger_ADC[size]);

      if (size == 2 or size == 4)
        {
          // sliding window made from 2x2 sums
          slide2_max_energy_trigger_ADC_hist_list[size]->Fill(
              slide2_max_energy_trigger_ADC[size]);
        }
    }
  return Fun4AllReturnCodes::EVENT_OK;
}
