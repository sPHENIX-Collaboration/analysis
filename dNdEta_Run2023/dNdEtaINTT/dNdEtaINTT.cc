//____________________________________________________________________________..
//
// This is a template for a Fun4All SubsysReco module with all methods from the
// $OFFLINE_MAIN/include/fun4all/SubsysReco.h baseclass
// You do not have to implement all of them, you can just remove unused methods
// here and in dNdEtaINTT.h.
//
// dNdEtaINTT(const std::string &name = "dNdEtaINTT")
// everything is keyed to dNdEtaINTT, duplicate names do work but it makes
// e.g. finding culprits in logs difficult or getting a pointer to the module
// from the command line
//
// dNdEtaINTT::~dNdEtaINTT()
// this is called when the Fun4AllServer is deleted at the end of running. Be
// mindful what you delete - you do loose ownership of object you put on the
// node tree
//
// int dNdEtaINTT::Init(PHCompositeNode *topNode)
// This method is called when the module is registered with the Fun4AllServer.
// You can create historgrams here or put objects on the node tree but be aware
// that modules which haven't been registered yet did not put antyhing on the
// node tree
//
// int dNdEtaINTT::InitRun(PHCompositeNode *topNode)
// This method is called when the first event is read (or generated). At
// this point the run number is known (which is mainly interesting for raw data
// processing). Also all objects are on the node tree in case your module's
// action depends on what else is around. Last chance to put nodes under the DST
// Node We mix events during readback if branches are added after the first
// event
//
// int dNdEtaINTT::process_event(PHCompositeNode *topNode)
// called for every event. Return codes trigger actions, you find them in
// $OFFLINE_MAIN/include/fun4all/Fun4AllReturnCodes.h
//   everything is good:
//     return Fun4AllReturnCodes::EVENT_OK
//   abort event reconstruction, clear everything and process next event:
//     return Fun4AllReturnCodes::ABORT_EVENT;
//   proceed but do not save this event in output (needs output manager
//   setting):
//     return Fun4AllReturnCodes::DISCARD_EVENT;
//   abort processing:
//     return Fun4AllReturnCodes::ABORT_RUN
// all other integers will lead to an error and abort of processing
//
// int dNdEtaINTT::ResetEvent(PHCompositeNode *topNode)
// If you have internal data structures (arrays, stl containers) which needs
// clearing after each event, this is the place to do that. The nodes under the
// DST node are cleared by the framework
//
// int dNdEtaINTT::EndRun(const int runnumber)
// This method is called at the end of a run when an event from a new run is
// encountered. Useful when analyzing multiple runs (raw data). Also called at
// the end of processing (before the End() method)
//
// int dNdEtaINTT::End(PHCompositeNode *topNode)
// This is called at the end of processing. It needs to be called by the macro
// by Fun4AllServer::End(), so do not forget this in your macro
//
// int dNdEtaINTT::Reset(PHCompositeNode *topNode)
// not really used - it is called before the dtor is called
//
// void dNdEtaINTT::Print(const std::string &what) const
// Called from the command line - useful to print information when you need it
//
//____________________________________________________________________________..

#include "dNdEtaINTT.h"

#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/PHTFileServer.h>
#include <intt/CylinderGeomIntt.h>
#include <phool/PHCompositeNode.h>
#include <trackbase/TrkrHitv2.h>

#include <limits>
#include <sstream>

namespace
{
  template <class T>
  void CleanVec(std::vector<T> &v)
  {
    std::vector<T>().swap(v);
    v.shrink_to_fit();
  }
}  // namespace

//____________________________________________________________________________..
dNdEtaINTT::dNdEtaINTT(const std::string &name, const std::string &outputfile, const bool &isData)
  : SubsysReco(name)
  , _get_hepmc_info(true)
  , _get_truth_cluster(true)
  , _get_reco_cluster(true)
  , _get_centrality(false)
  , _get_intt_data(true)
  , _get_inttrawhit(true)
  , _get_trkr_hit(true)
  , _get_phg4_info(true)
  , _outputFile(outputfile)
  , IsData(isData)
  , eventheader(nullptr)
  , m_geneventmap(nullptr)
  , m_genevt(nullptr)
  , svtx_evalstack(nullptr)
  , truth_eval(nullptr)
  , clustereval(nullptr)
  , hiteval(nullptr)
  , dst_clustermap(nullptr)
  , clusterhitmap(nullptr)
  , inttrawhitcontainer(nullptr)
  , hitsets(nullptr)
  , _tgeometry(nullptr)
  , _intt_geom_container(nullptr)
  , m_truth_info(nullptr)
  , m_CentInfo(nullptr)
{
  if (Verbosity() >= VERBOSITY_MORE) std::cout << "dNdEtaINTT::dNdEtaINTT(const std::string &name) Calling ctor" << std::endl;
}

//____________________________________________________________________________..
dNdEtaINTT::~dNdEtaINTT()
{
  if (Verbosity() >= VERBOSITY_MORE) std::cout << "dNdEtaINTT::~dNdEtaINTT() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
int dNdEtaINTT::Init(PHCompositeNode *topNode)
{
  std::cout << "dNdEtaINTT::Init(PHCompositeNode *topNode) Initializing" << std::endl
            << "Running on Data or simulation? -> IsData = " << IsData << std::endl;

  PHTFileServer::get().open(_outputFile, "RECREATE");
  outtree = new TTree("EventTree", "EventTree");

  if (_get_centrality)
  {
    if (!IsData)
    {
      outtree->Branch("ncoll", &ncoll_);
      outtree->Branch("npart", &npart_);
      outtree->Branch("centrality_bimp", &centrality_bimp_);
      outtree->Branch("centrality_impactparam", &centrality_impactparam_);
    }
    outtree->Branch("event", &event_);
    outtree->Branch("clk", &clk);
    outtree->Branch("femclk", &femclk);
    outtree->Branch("is_min_bias", &is_min_bias);
    outtree->Branch("MBD_centrality", &centrality_mbd_);
    outtree->Branch("MBD_z_vtx", &mbd_z_vtx);
    outtree->Branch("MBD_south_charge_sum", &mbd_south_charge_sum);
    outtree->Branch("MBD_north_charge_sum", &mbd_north_charge_sum);
    outtree->Branch("MBD_charge_sum", &mbd_charge_sum);
    outtree->Branch("MBD_charge_asymm", &mbd_charge_asymm);
    if (_get_pmt_info)
    {
      for (unsigned int i = 0; i < 128; i++)
      {
        std::ostringstream pmtNumber;
        pmtNumber << std::setfill('0') << std::setw(3) << std::to_string(i);
        std::string branchName = "MBD_PMT" + pmtNumber.str() + "_charge";
        outtree->Branch(branchName.c_str(), &m_pmt_q[i]);
      }
    }
  }
  if (_get_intt_data)
  {
    outtree->Branch("event_counter", &event_);
    outtree->Branch("INTT_BCO", &intt_bco);

    if (!IsData)
    {
      outtree->Branch("NTruthVtx", &NTruthVtx_);
      outtree->Branch("TruthPV_trig_x", &TruthPV_trig_x_);
      outtree->Branch("TruthPV_trig_y", &TruthPV_trig_y_);
      outtree->Branch("TruthPV_trig_z", &TruthPV_trig_z_);
      // HepMC::GenParticle informaiton (final states, after boost and rotation)
      outtree->Branch("NHepMCFSPart", &NHepMCFSPart_);
      outtree->Branch("signal_process_id", &signal_process_id_);
      outtree->Branch("HepMCFSPrtl_Pt", &HepMCFSPrtl_Pt_);
      outtree->Branch("HepMCFSPrtl_Eta", &HepMCFSPrtl_Eta_);
      outtree->Branch("HepMCFSPrtl_Phi", &HepMCFSPrtl_Phi_);
      outtree->Branch("HepMCFSPrtl_E", &HepMCFSPrtl_E_);
      outtree->Branch("HepMCFSPrtl_PID", &HepMCFSPrtl_PID_);
      outtree->Branch("HepMCFSPrtl_prodx", &HepMCFSPrtl_prodx_);
      outtree->Branch("HepMCFSPrtl_prody", &HepMCFSPrtl_prody_);
      outtree->Branch("HepMCFSPrtl_prodz", &HepMCFSPrtl_prodz_);
      // Primary PHG4Particle information
      outtree->Branch("NPrimaryG4P", &NPrimaryG4P_);
      outtree->Branch("PrimaryG4P_Pt", &PrimaryG4P_Pt_);
      outtree->Branch("PrimaryG4P_Eta", &PrimaryG4P_Eta_);
      outtree->Branch("PrimaryG4P_Phi", &PrimaryG4P_Phi_);
      outtree->Branch("PrimaryG4P_E", &PrimaryG4P_E_);
      outtree->Branch("PrimaryG4P_PID", &PrimaryG4P_PID_);
      // Truth cluster information & matching with reco clusters
      outtree->Branch("NTruthLayers", &NTruthLayers_);
      outtree->Branch("ClusTruthCKeys", &ClusTruthCKeys_);
      outtree->Branch("ClusNG4Particles", &ClusNG4Particles_);
      outtree->Branch("ClusNPrimaryG4Particles", &ClusNPrimaryG4Particles_);
      outtree->Branch("TruthClusPhiSize", &TruthClusPhiSize_);
      outtree->Branch("TruthClusZSize", &TruthClusZSize_);
      outtree->Branch("TruthClusNRecoClus", &TruthClusNRecoClus_);
      outtree->Branch("PrimaryTruthClusPhiSize", &PrimaryTruthClusPhiSize_);
      outtree->Branch("PrimaryTruthClusZSize", &PrimaryTruthClusZSize_);
      outtree->Branch("PrimaryTruthClusNRecoClus", &PrimaryTruthClusNRecoClus_);
    }

    // InttRawHit information
    if (_get_inttrawhit)
    {
      outtree->Branch("NInttRawHits", &NInttRawHits_);
      outtree->Branch("InttRawHit_bco", &InttRawHit_bco_);
      outtree->Branch("InttRawHit_packetid", &InttRawHit_packetid_);
      outtree->Branch("InttRawHit_word", &InttRawHit_word_);
      outtree->Branch("InttRawHit_fee", &InttRawHit_fee_);
      outtree->Branch("InttRawHit_channel_id", &InttRawHit_channel_id_);
      outtree->Branch("InttRawHit_chip_id", &InttRawHit_chip_id_);
      outtree->Branch("InttRawHit_adc", &InttRawHit_adc_);
      outtree->Branch("InttRawHit_FPHX_BCO", &InttRawHit_FPHX_BCO_);
      outtree->Branch("InttRawHit_full_FPHX", &InttRawHit_full_FPHX_);
      outtree->Branch("InttRawHit_full_ROC", &InttRawHit_full_ROC_);
      outtree->Branch("InttRawHit_amplitude", &InttRawHit_amplitude_);
    }
    // TrkrHit informatio
    if (_get_trkr_hit)
    {
      outtree->Branch("NTrkrhits", &NTrkrhits_);
      outtree->Branch("TrkrHitRow", &TrkrHitRow_);
      outtree->Branch("TrkrHitColumn", &TrkrHitColumn_);
      outtree->Branch("TrkrHitLadderZId", &TrkrHitLadderZId_);
      outtree->Branch("TrkrHitLadderPhiId", &TrkrHitLadderPhiId_);
      outtree->Branch("TrkrHitLayer", &TrkrHitLayer_);
      outtree->Branch("TrkrHitADC", &TrkrHitADC_);
    }
    // TrkrCluster information
    if (_get_reco_cluster)
    {
      outtree->Branch("NClus_Layer1", &NClus_Layer1_);
      outtree->Branch("NClus", &NClus_);
      outtree->Branch("ClusLayer", &ClusLayer_);
      outtree->Branch("ClusX", &ClusX_);
      outtree->Branch("ClusY", &ClusY_);
      outtree->Branch("ClusZ", &ClusZ_);
      outtree->Branch("ClusR", &ClusR_);
      outtree->Branch("ClusPhi", &ClusPhi_);
      outtree->Branch("ClusEta", &ClusEta_);
      outtree->Branch("ClusAdc", &ClusAdc_);
      outtree->Branch("ClusPhiSize", &ClusPhiSize_);
      outtree->Branch("ClusZSize", &ClusZSize_);
      outtree->Branch("ClusLadderZId", &ClusLadderZId_);
      outtree->Branch("ClusLadderPhiId", &ClusLadderPhiId_);
      outtree->Branch("ClusTrkrHitSetKey", &ClusTrkrHitSetKey_);
      outtree->Branch("ClusTimeBucketId", &ClusTimeBucketId_);
    }
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int dNdEtaINTT::InitRun(PHCompositeNode *topNode)
{
  if (Verbosity() >= VERBOSITY_MORE) std::cout << "dNdEtaINTT::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int dNdEtaINTT::process_event(PHCompositeNode *topNode)
{
  // std::cout << "dNdEtaINTT::process_event(PHCompositeNode *topNode) Processing Event" << InputFileListIndex * NEvtPerFile + eventNum << std::endl;
  if (Verbosity() >= VERBOSITY_MORE) std::cout << "dNdEtaINTT::process_event(PHCompositeNode *topNode) Processing Event " << eventNum << std::endl;

  PHNodeIterator nodeIter(topNode);

  if (!IsData)
  {
    if (!svtx_evalstack)
    {
      svtx_evalstack = new SvtxEvalStack(topNode);
      clustereval = svtx_evalstack->get_cluster_eval();
      hiteval = svtx_evalstack->get_hit_eval();
      truth_eval = svtx_evalstack->get_truth_eval();
    }

    svtx_evalstack->next_event(topNode);
  }

  if (_get_centrality)
  {
    GetCentralityInfo(topNode);
  }

  if (_get_intt_data)
  {
    if (!IsData)
    {
      std::cout << "Running on simulation" << std::endl;

      if (_get_phg4_info)
        GetPHG4Info(topNode);

      if (_get_hepmc_info)
        GetHEPMCInfo(topNode);

      if (_get_truth_cluster)
        GetTruthClusterInfo(topNode);
    }

    if (_get_inttrawhit)
      GetInttRawHitInfo(topNode);

    if (_get_trkr_hit)
      GetTrkrHitInfo(topNode);

    if (_get_reco_cluster)
      GetRecoClusterInfo(topNode);
  }

  if (IsData)
  {
    if (_get_intt_data)
    {
      intteventinfo = findNode::getClass<InttEventInfo>(topNode, "INTTEVENTHEADER");
      if (!intteventinfo)
      {
        std::cout << "The INTT event header is missing, you will have no BCO information fro syncing" << std::endl;
      }
      
      intt_bco = intteventinfo->get_bco_full();
    }
  }

  if (_get_centrality)
  {
    eventheader = findNode::getClass<EventHeader>(topNode, "EventHeader");
  }
  // event_ = InputFileListIndex * NEvtPerFile + eventNum;
  event_ = _get_centrality ? eventheader->get_EvtSequence() : eventNum;
  outtree->Fill();
  eventNum++;

  ResetVectors();

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int dNdEtaINTT::ResetEvent(PHCompositeNode *topNode)
{
  if (Verbosity() >= VERBOSITY_MORE) std::cout << "dNdEtaINTT::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int dNdEtaINTT::EndRun(const int runnumber)
{
  if (Verbosity() >= VERBOSITY_MORE) std::cout << "dNdEtaINTT::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int dNdEtaINTT::End(PHCompositeNode *topNode)
{
  if (Verbosity() >= VERBOSITY_MORE) std::cout << "dNdEtaINTT::End(PHCompositeNode *topNode) This is the End - Output to " << _outputFile << std::endl;

  PHTFileServer::get().cd(_outputFile);
  outtree->Write("", TObject::kOverwrite);

  delete svtx_evalstack;

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int dNdEtaINTT::Reset(PHCompositeNode *topNode)
{
  if (Verbosity() >= VERBOSITY_MORE) std::cout << "dNdEtaINTT::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void dNdEtaINTT::Print(const std::string &what) const { std::cout << "dNdEtaINTT::Print(const std::string &what) const Printing info for " << what << std::endl; }

//____________________________________________________________________________..
void dNdEtaINTT::GetHEPMCInfo(PHCompositeNode *topNode)
{
  std::cout << "Get HEPMC info." << std::endl;

  // HEPMC info
  m_geneventmap = findNode::getClass<PHHepMCGenEventMap>(topNode, "PHHepMCGenEventMap");
  PHHepMCGenHelper *phhepmcgenhlpr = new PHHepMCGenHelper();
  phhepmcgenhlpr->PHHepMCGenHelper_Verbosity(1);

  if (m_geneventmap)
  {
    PHHepMCGenEventMap::ConstIter iter_beg = m_geneventmap->begin();
    PHHepMCGenEventMap::ConstIter iter_end = m_geneventmap->end();
    for (PHHepMCGenEventMap::ConstIter iter = iter_beg; iter != iter_end; ++iter)
    {
      PHHepMCGenEvent *genevt = iter->second;
      // HepMC::ThreeVector initv_boostbeta = genevt->get_boost_beta_vector(); HepMC::ThreeVector
      // initv_rotation = genevt->get_rotation_vector();
      // std::cout << "Initial boost beta vector: " << initv_boostbeta.x() << " " << initv_boostbeta.y() << " " << initv_boostbeta.z() << std::endl;
      // std::cout << "Initial rotation vector: " << initv_rotation.x() << " " << initv_rotation.y() << " " << initv_rotation.z() << std::endl;
      // genevt->identify();

      if (genevt->get_embedding_id() != 0)
        continue;

      HepMC::GenEvent *evt = genevt->getEvent();

      // Get the Lorentz rotation and boost
      const CLHEP::HepLorentzRotation lortentz_rotation(genevt->get_LorentzRotation_EvtGen2Lab());
      const double mom_factor = HepMC::Units::conversion_factor(evt->momentum_unit(), HepMC::Units::GEV);

      if (evt)
      {
        // genevt->PrintEvent();
        std::cout << "Signal process id " << evt->signal_process_id() << std::endl;
        std::cout << "Embedding id " << genevt->get_embedding_id() << std::endl;
        std::cout << "is simulated " << genevt->is_simulated() << std::endl;
        std::cout << "Collision vertex x (PHHepMCGenEvent): " << genevt->get_collision_vertex().x() << std::endl;
        std::cout << "Collision vertex y (PHHepMCGenEvent): " << genevt->get_collision_vertex().y() << std::endl;
        std::cout << "Collision vertex z (PHHepMCGenEvent): " << genevt->get_collision_vertex().z() << std::endl;
        std::cout << "Collision vertex t (PHHepMCGenEvent): " << genevt->get_collision_vertex().t() << std::endl;

        signal_process_id_ = evt->signal_process_id();

        for (HepMC::GenEvent::particle_const_iterator p = evt->particles_begin(); p != evt->particles_end(); ++p)
        {
          HepMC::GenParticle *particle = *p;
          HepMC::GenVertex *vtx_decay = particle->end_vertex();
          if (vtx_decay == nullptr && particle->status() == 1)
          {
            // Lorentz rotation and boost for the beam crossing and shifted vertex
            CLHEP::HepLorentzVector lv_momentum(particle->momentum().px(), particle->momentum().py(), particle->momentum().pz(), particle->momentum().e());
            lv_momentum = lortentz_rotation(lv_momentum);

            TLorentzVector hepmcp;
            hepmcp.SetPxPyPzE(lv_momentum.px() * mom_factor, lv_momentum.py() * mom_factor, lv_momentum.pz() * mom_factor, lv_momentum.e() * mom_factor);

            HepMCFSPrtl_Pt_.push_back(hepmcp.Pt());
            HepMCFSPrtl_Eta_.push_back(hepmcp.Eta());
            HepMCFSPrtl_Phi_.push_back(hepmcp.Phi());
            HepMCFSPrtl_E_.push_back(hepmcp.E());
            HepMCFSPrtl_PID_.push_back(particle->pdg_id());

            HepMC::GenVertex *vtx_prod = particle->production_vertex();
            if (vtx_prod)
            {
              HepMCFSPrtl_prodx_.push_back(vtx_prod->position().x());
              HepMCFSPrtl_prody_.push_back(vtx_prod->position().y());
              HepMCFSPrtl_prodz_.push_back(vtx_prod->position().z());
            }
          }
        }

        NHepMCFSPart_ = HepMCFSPrtl_PID_.size();
      }
      else
      {
        std::cout << "No HepMC event" << std::endl;
        continue;
      }
    }
  }
  else
  {
    std::cout << PHWHERE << "Error, can't find PHHepMCGenEventMap" << std::endl;
    exit(1);
  }
}
//____________________________________________________________________________..
void dNdEtaINTT::GetCentralityInfo(PHCompositeNode *topNode)
{
  if (Verbosity() >= VERBOSITY_MORE) std::cout << "Get centrality info." << std::endl;

  m_CentInfo = findNode::getClass<CentralityInfo>(topNode, "CentralityInfo");
  if (!m_CentInfo)
  {
    std::cout << PHWHERE << "Error, can't find CentralityInfov1" << std::endl;
    exit(1);
  }

  _minimumbiasinfo = findNode::getClass<MinimumBiasInfo>(topNode, "MinimumBiasInfo");
  if (!_minimumbiasinfo)
  {
    std::cout << "Error, can't find MinimumBiasInfo" << std::endl;
    exit(1);
  }

  m_mbdout = findNode::getClass<MbdOut>(topNode, "MbdOut");
  if (!m_mbdout)
  {
    std::cout << "Error, can't find MbdOut" << std::endl;
    exit(1);
  }

  if (_get_pmt_info)
  {
    m_mbdpmtcontainer = findNode::getClass<MbdPmtContainer>(topNode, "MbdPmtContainer");
    if (!m_mbdpmtcontainer)
    {
      std::cout << "Error, can't find MbdPmtContainer" << std::endl;
      exit(1);
    }
  }

  m_glbvtxmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
  if (!m_glbvtxmap)
  {
    std::cout << "Error, can't find GlobalVertexMap" << std::endl;
    exit(1);
  }

  if (!IsData)
  {
    centrality_bimp_ = m_CentInfo->get_centile(CentralityInfo::PROP::bimp);
    centrality_impactparam_ = m_CentInfo->get_quantity(CentralityInfo::PROP::bimp);

    // Glauber parameter information
    ncoll_ = eventheader->get_ncoll();
    npart_ = eventheader->get_npart();

    // IsMinBias_ = Minimumbiasinfo->isAuAuMinimumBias(); // TODO: uncomment
    // this when the minimum bias info is available

    // std::cout << "Is minimum bias? " << IsMinBias_ << std::endl; // TODO:
    // uncomment this when the minimum bias info is available
    std::cout << "Centrality: (bimp,impactparam) = (" << centrality_bimp_ << ", " << centrality_impactparam_ << "); (mbd,mbdquantity) = (" << centrality_mbd_ << ", " << centrality_mbdquantity_ << ")"
              << std::endl;
    std::cout << "Glauber parameter information: (ncoll,npart) = (" << ncoll_ << ", " << npart_ << ")" << std::endl;
  }

  clk = m_mbdout->get_clock();
  femclk = m_mbdout->get_femclock();
  mbd_south_charge_sum = m_mbdout->get_q(0);
  mbd_north_charge_sum = m_mbdout->get_q(1);
  mbd_charge_sum = mbd_south_charge_sum + mbd_north_charge_sum;
  mbd_charge_asymm = mbd_charge_sum == 0 ? std::numeric_limits<float>::quiet_NaN() : (float) (mbd_south_charge_sum - mbd_north_charge_sum) / mbd_charge_sum;
  centrality_mbd_ = m_CentInfo->has_centile(CentralityInfo::PROP::mbd_NS) ? m_CentInfo->get_centile(CentralityInfo::PROP::mbd_NS) : std::numeric_limits<float>::quiet_NaN();
  is_min_bias = _minimumbiasinfo->isAuAuMinimumBias();

  if (_get_pmt_info)
  {
    for (int i = 0; i < 128; i++)
    {
      m_pmt_q[i] = m_mbdpmtcontainer->get_pmt(i)->get_q();
    }
  }

  mbd_z_vtx = std::numeric_limits<float>::quiet_NaN();
  // Now get MBD z vertex value
  for (GlobalVertexMap::ConstIter iter = m_glbvtxmap->begin(); iter != m_glbvtxmap->end(); ++iter)
  {
    m_glbvtx = iter->second;
    auto mbdvtxiter = m_glbvtx->find_vertexes(GlobalVertex::MBD);
    // check that it contains a track vertex
    if (mbdvtxiter == m_glbvtx->end_vertexes())
    {
      continue;
    }

    auto mbdvertexvector = mbdvtxiter->second;

    for (auto &vertex : mbdvertexvector)
    {
      mbd_z_vtx = vertex->get_z();
      // m_mbdvtx = m_dst_vertexmap->find(vertex->get_id())->second;
    }
  }
}
//____________________________________________________________________________..
void dNdEtaINTT::GetInttRawHitInfo(PHCompositeNode *topNode)
{
  std::cout << "Get InttRawHit info." << std::endl;

  inttrawhitcontainer = findNode::getClass<InttRawHitContainer>(topNode, "INTTRAWHIT");
  if (!inttrawhitcontainer)
  {
    std::cout << PHWHERE << "Error, can't find INTTRAWHIT" << std::endl;
    exit(1);
  }

  NInttRawHits_ = inttrawhitcontainer->get_nhits();

  for (unsigned int i = 0; i < inttrawhitcontainer->get_nhits(); i++)
  {
    InttRawHit *intthit = inttrawhitcontainer->get_hit(i);

    InttRawHit_bco_.push_back(intthit->get_bco());
    InttRawHit_packetid_.push_back(intthit->get_packetid());
    InttRawHit_word_.push_back(intthit->get_word());
    InttRawHit_fee_.push_back(intthit->get_fee());
    InttRawHit_channel_id_.push_back(intthit->get_channel_id());
    InttRawHit_chip_id_.push_back(intthit->get_chip_id());
    InttRawHit_adc_.push_back(intthit->get_adc());
    InttRawHit_FPHX_BCO_.push_back(intthit->get_FPHX_BCO());
    InttRawHit_full_FPHX_.push_back(intthit->get_full_FPHX());
    InttRawHit_full_ROC_.push_back(intthit->get_full_ROC());
    InttRawHit_amplitude_.push_back(intthit->get_amplitude());
  }
}
//____________________________________________________________________________..
void dNdEtaINTT::GetTrkrHitInfo(PHCompositeNode *topNode)
{
  if (Verbosity() >= VERBOSITY_MORE) std::cout << "Get TrkrHit info." << std::endl;

  hitsets = findNode::getClass<TrkrHitSetContainer>(topNode, "TRKR_HITSET");
  if (!hitsets)
  {
    std::cout << PHWHERE << "Error, can't find cluster-hit map" << std::endl;
    exit(1);
  }

  TrkrHitSetContainer::ConstRange hitset_range = hitsets->getHitSets(TrkrDefs::TrkrId::inttId);
  for (TrkrHitSetContainer::ConstIterator hitset_iter = hitset_range.first; hitset_iter != hitset_range.second; ++hitset_iter)
  {
    TrkrHitSet::ConstRange hit_range = hitset_iter->second->getHits();
    for (TrkrHitSet::ConstIterator hit_iter = hit_range.first; hit_iter != hit_range.second; ++hit_iter)
    {
      TrkrDefs::hitkey hitKey = hit_iter->first;
      TrkrDefs::hitsetkey hitSetKey = hitset_iter->first;
      TrkrHitRow_.push_back(InttDefs::getRow(hitKey));
      TrkrHitColumn_.push_back(InttDefs::getCol(hitKey));
      TrkrHitLadderZId_.push_back(InttDefs::getLadderZId(hitSetKey));
      TrkrHitLadderPhiId_.push_back(InttDefs::getLadderPhiId(hitSetKey));
      TrkrHitLayer_.push_back(TrkrDefs::getLayer(hitSetKey));
      TrkrHitADC_.push_back(hit_iter->second->getAdc());
    }
  }
  NTrkrhits_ = TrkrHitRow_.size();
}
//____________________________________________________________________________..
void dNdEtaINTT::GetRecoClusterInfo(PHCompositeNode *topNode)
{
  if (Verbosity() >= VERBOSITY_MORE) std::cout << "Get reconstructed cluster info." << std::endl;

  dst_clustermap = findNode::getClass<TrkrClusterContainerv4>(topNode, "TRKR_CLUSTER");
  if (!dst_clustermap)
  {
    std::cout << PHWHERE << "Error, can't find trkr clusters" << std::endl;
    exit(1);
  }

  _tgeometry = findNode::getClass<ActsGeometry>(topNode, "ActsGeometry");
  if (!_tgeometry)
  {
    std::cout << PHWHERE << "Error, can't find Acts geometry" << std::endl;
    exit(1);
  }

  std::vector<int> _NClus = {0, 0};
  // std::map<int, unsigned int> AncG4P_cluster_count;
  // AncG4P_cluster_count.clear();

  // Reference:
  // https://github.com/sPHENIX-Collaboration/coresoftware/blob/master/simulation/g4simulation/g4eval/SvtxEvaluator.cc#L1731-L1984
  // for (const auto &hitsetkey : dst_clustermap->getHitSetKeys())
  for (const auto &hitsetkey : dst_clustermap->getHitSetKeys(TrkrDefs::inttId))
  {
    auto range = dst_clustermap->getClusters(hitsetkey);
    for (auto iter = range.first; iter != range.second; ++iter)
    {
      // std::cout << "----------" << std::endl;
      TrkrDefs::cluskey ckey = iter->first;
      TrkrCluster *cluster = dst_clustermap->findCluster(ckey);
      unsigned int trkrId = TrkrDefs::getTrkrId(ckey);
      if (trkrId != TrkrDefs::inttId)
        continue;  // we want only INTT clusters
      int layer = (TrkrDefs::getLayer(ckey) == 3 || TrkrDefs::getLayer(ckey) == 4) ? 0 : 1;
      _NClus[layer]++;
      if (cluster == nullptr)
      {
        std::cout << "cluster is nullptr, ckey=" << ckey << std::endl;
      }
      auto globalpos = _tgeometry->getGlobalPosition(ckey, cluster);
      ClusLayer_.push_back(TrkrDefs::getLayer(ckey));
      ClusX_.push_back(globalpos(0));
      ClusY_.push_back(globalpos(1));
      ClusZ_.push_back(globalpos(2));
      ClusAdc_.push_back(cluster->getAdc());
      TVector3 pos(globalpos(0), globalpos(1), globalpos(2));
      ClusR_.push_back(pos.Perp());
      ClusPhi_.push_back(pos.Phi());
      ClusEta_.push_back(pos.Eta());
      // ClusPhiSize is a signed char (-127-127), we should convert it to an unsigned char (0-255)
      // ClusPhiSize is a signed char (-127-127), we should convert it to
      // an unsigned char (0-255)
      int phisize = cluster->getPhiSize();
      if (phisize <= 0)
        phisize += 256;
      ClusPhiSize_.push_back(phisize);
      ClusZSize_.push_back(cluster->getZSize());
      ClusLadderZId_.push_back(InttDefs::getLadderZId(ckey));
      ClusLadderPhiId_.push_back(InttDefs::getLadderPhiId(ckey));
      ClusTrkrHitSetKey_.push_back(hitsetkey);
      ClusTimeBucketId_.push_back(InttDefs::getTimeBucketId(ckey));
      if (!IsData)
      {
        // truth cluster association
        std::vector<int32_t> truth_ckeys;
        std::set<PHG4Particle *> truth_particles = clustereval->all_truth_particles(ckey);
        int Nprimary = 0;
        for (auto &p : truth_particles)
        {
          // must be primary truth particle
          if (p->get_parent_id() == 0)
          {
            Nprimary++;
            std::map<TrkrDefs::cluskey, std::shared_ptr<TrkrCluster>> truth_clusters = truth_eval->all_truth_clusters(p);
            for (auto &c : truth_clusters)
            {
              // only take at most 1 truth cluster per truth particle
              // only take at most 1 truth cluster per truth
              // particle
              bool found = false;
              if (TrkrDefs::getLayer(c.first) == TrkrDefs::getLayer(ckey))
              {
                if (!found)
                {
                  found = true;
                  truth_ckeys.push_back(c.first);
                }
              }
            }
          }
        }
        ClusTruthCKeys_.push_back(truth_ckeys.size());
        ClusNG4Particles_.push_back(truth_particles.size());
        ClusNPrimaryG4Particles_.push_back(Nprimary);
      }
    }
  }

  NClus_ = _NClus[0] + _NClus[1];
  NClus_Layer1_ = _NClus[0];
  if (Verbosity() >= VERBOSITY_MORE) std::cout << "Number of clusters (total,0,1)=(" << NClus_ << "," << _NClus[0] << "," << _NClus[1] << ")" << std::endl;
}
//____________________________________________________________________________..
void dNdEtaINTT::GetTruthClusterInfo(PHCompositeNode *topNode)
{
  std::cout << "Get truth cluster info." << std::endl;

  _intt_geom_container = findNode::getClass<PHG4CylinderGeomContainer>(topNode, "CYLINDERGEOM_INTT");
  if (!_intt_geom_container)
  {
    std::cout << PHWHERE << "Error, can't find INTT geometry container" << std::endl;
    exit(1);
  }

  _tgeometry = findNode::getClass<ActsGeometry>(topNode, "ActsGeometry");
  if (!_tgeometry)
  {
    std::cout << PHWHERE << "Error, can't find Acts geometry" << std::endl;
    exit(1);
  }

  auto prange = m_truth_info->GetParticleRange();

  NTruthLayers_ = 0;

  for (auto iter = prange.first; iter != prange.second; ++iter)
  {
    PHG4Particle *truth_particle = iter->second;

    // count number of layers passed through by truth particles
    bool layer_filled[4] = {false, false, false, false};
    std::map<TrkrDefs::cluskey, std::shared_ptr<TrkrCluster>> truth_clusters = truth_eval->all_truth_clusters(truth_particle);
    for (auto c_iter = truth_clusters.begin(); c_iter != truth_clusters.end(); c_iter++)
    {
      int layer = TrkrDefs::getLayer(c_iter->first);
      if (layer >= 3 && layer <= 6)
        layer_filled[layer - 3] = true;
    }
    for (int i = 0; i < 4; i++)
      if (layer_filled[i])
        NTruthLayers_++;

    // compute phisize and zsize of truth clusters
    std::set<PHG4Hit *> truth_hits = truth_eval->all_truth_hits(truth_particle);
    // just as with truth clustering in SvtxTruthEval, cluster together all
    // G4Hits in a given layer
    std::array<std::vector<PHG4Hit *>, 4> clusters;
    for (auto h_iter = truth_hits.begin(); h_iter != truth_hits.end(); ++h_iter)
    {
      PHG4Hit *hit = *h_iter;
      int layer = hit->get_layer();
      if (layer >= 3 && layer <= 6)
      {
        clusters[layer - 3].push_back(hit);
      }
    }

    // since SvtxClusterEval doesn't correctly compute cluster size,
    // and since none of the relevant truth objects store their association
    // with detector elements. we have to do that association ourselves
    // through CylinderGeomIntt
    std::vector<CylinderGeomIntt *> layergeom;

    for (int layer = 3; layer <= 6; layer++)
    {
      layergeom.push_back(dynamic_cast<CylinderGeomIntt *>(_intt_geom_container->GetLayerGeom(layer)));
    }

    for (int i = 0; i < 4; i++)
    {
      // we break clusters up by TrkrHitset, in the same way as in
      // InttClusterizer
      std::map<TrkrDefs::hitsetkey, std::vector<PHG4Hit *>> clusters_by_hitset;

      for (auto &hit : clusters[i])
      {
        double entry_point[3] = {hit->get_x(0), hit->get_y(0), hit->get_z(0)};
        double exit_point[3] = {hit->get_x(1), hit->get_y(1), hit->get_z(1)};

        // find ladder z and phi id
        int entry_ladder_z_id, entry_ladder_phi_id;
        int exit_ladder_z_id, exit_ladder_phi_id;
        layergeom[i]->find_indices_from_world_location(entry_ladder_z_id, entry_ladder_phi_id, entry_point);
        layergeom[i]->find_indices_from_world_location(exit_ladder_z_id, exit_ladder_phi_id, exit_point);

        // fix a rounding error where you can sometimes get a phi index
        // out of range, which causes a segfault when the geometry
        // container can't find a surface
        if (i == 0 || i == 1)
        {
          if (entry_ladder_phi_id == 12)
            entry_ladder_phi_id = 0;
          if (exit_ladder_phi_id == 12)
            exit_ladder_phi_id = 0;
        }
        if (i == 2 || i == 3)
        {
          if (entry_ladder_phi_id == 16)
            entry_ladder_phi_id = 0;
          if (exit_ladder_phi_id == 16)
            exit_ladder_phi_id = 0;
        }

        // get hitset key so we can retrieve surface
        TrkrDefs::hitsetkey entry_hskey = InttDefs::genHitSetKey(i + 3, entry_ladder_z_id, entry_ladder_phi_id, 0);
        TrkrDefs::hitsetkey exit_hskey = InttDefs::genHitSetKey(i + 3, exit_ladder_z_id, exit_ladder_phi_id, 0);

        // if entry and exit ladder z id are different, then we have a
        // truth G4Hit that will be split in reco (since InttClusterizer
        // cannot cross TrkrHitsets) for now, just print a warning if
        // this ever happens (should be very rare) and discard that hit
        if (entry_hskey != exit_hskey)
        {
          std::cout << "!!! WARNING !!! PHG4Hit crosses TrkrHitsets, "
                       "discarding!"
                    << std::endl;
          std::cout << "Discarded PHG4Hit info: " << std::endl;
          std::cout << "entry point: ladder (phi, z) ID = (" << entry_ladder_phi_id << ", " << entry_ladder_z_id << ")" << std::endl;
          std::cout << "exit point: ladder (phi, z) ID = (" << exit_ladder_phi_id << ", " << exit_ladder_z_id << ")" << std::endl;
          continue;
        }

        clusters_by_hitset[entry_hskey].push_back(hit);
      }

      for (auto chs_iter = clusters_by_hitset.begin(); chs_iter != clusters_by_hitset.end(); chs_iter++)
      {
        TrkrDefs::hitsetkey hskey = chs_iter->first;
        std::vector<PHG4Hit *> hits = chs_iter->second;

        auto surface = _tgeometry->maps().getSiliconSurface(hskey);

        int ladder_z_id = InttDefs::getLadderZId(hskey);

        // just as in InttClusterizer, cluster size = number of unique
        // strip IDs
        std::set<int> phibins;
        std::set<int> zbins;

        std::set<TrkrDefs::cluskey> associated_reco_clusters;

        for (auto &hit : hits)
        {
          double entry_point[3] = {hit->get_x(0), hit->get_y(0), hit->get_z(0)};
          double exit_point[3] = {hit->get_x(1), hit->get_y(1), hit->get_z(1)};

          // get local coordinates on surface
          TVector3 entry_local = layergeom[i]->get_local_from_world_coords(surface, _tgeometry, entry_point);
          TVector3 exit_local = layergeom[i]->get_local_from_world_coords(surface, _tgeometry, exit_point);

          // get strip z and phi id (which we needed local coordinates
          // for)
          int entry_strip_phi_id, entry_strip_z_id;
          int exit_strip_phi_id, exit_strip_z_id;
          layergeom[i]->find_strip_index_values(ladder_z_id, entry_local[1], entry_local[2], entry_strip_phi_id, entry_strip_z_id);
          layergeom[i]->find_strip_index_values(ladder_z_id, exit_local[1], exit_local[2], exit_strip_phi_id, exit_strip_z_id);

          // insert one phi and z ID entry for every strip between the
          // entry and exit point (this implicitly assumes that there
          // will be a non-negligible amount of energy deposited along
          // the whole path)
          int min_z_id = std::min(entry_strip_z_id, exit_strip_z_id);
          int max_z_id = std::max(entry_strip_z_id, exit_strip_z_id);
          int min_phi_id = std::min(entry_strip_phi_id, exit_strip_phi_id);
          int max_phi_id = std::max(entry_strip_phi_id, exit_strip_phi_id);
          for (int z_id = min_z_id; z_id <= max_z_id; z_id++)
          {
            zbins.insert(z_id);
          }
          for (int phi_id = min_phi_id; phi_id <= max_phi_id; phi_id++)
          {
            phibins.insert(phi_id);
          }

          // find all reco clusters associated with this PHG4Hit
          std::set<TrkrDefs::cluskey> reco_clusters_g4hit = clustereval->all_clusters_from(hit);
          associated_reco_clusters.insert(reco_clusters_g4hit.begin(), reco_clusters_g4hit.end());
        }
        if (phibins.size() > 0)
          TruthClusPhiSize_.push_back(phibins.size());
        if (phibins.size() > 0 && truth_particle->get_parent_id() == 0)
          PrimaryTruthClusPhiSize_.push_back(phibins.size());
        if (zbins.size() > 0)
          TruthClusZSize_.push_back(zbins.size());
        if (zbins.size() > 0 && truth_particle->get_parent_id() == 0)
          PrimaryTruthClusZSize_.push_back(zbins.size());
        TruthClusNRecoClus_.push_back(associated_reco_clusters.size());
        if (truth_particle->get_parent_id() == 0)
          PrimaryTruthClusNRecoClus_.push_back(associated_reco_clusters.size());
      }
    }
  }
}
//____________________________________________________________________________..
void dNdEtaINTT::GetPHG4Info(PHCompositeNode *topNode)
{
  std::cout << "Get PHG4 info.: truth primary vertex" << std::endl;
  m_truth_info = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
  if (!m_truth_info)
  {
    std::cout << PHWHERE << "Error, can't find G4TruthInfo" << std::endl;
    exit(1);
  }
  // Truth vertex
  auto vrange = m_truth_info->GetPrimaryVtxRange();
  int NTruthPV = 0, NTruthPV_Embeded0 = 0;
  for (auto iter = vrange.first; iter != vrange.second; ++iter)  // process all primary vertices
  {
    const int point_id = iter->first;
    PHG4VtxPoint *point = iter->second;
    if (point)
    {
      if (m_truth_info->isEmbededVtx(point_id) == 0)
      {
        TruthPV_trig_x_ = point->get_x();
        TruthPV_trig_y_ = point->get_y();
        TruthPV_trig_z_ = point->get_z();
        // std::cout << "TruthVtx " << NTruthPV_Embeded0 << ", vertex: (x,y,z,t)=(" << point->get_x() << "," << point->get_y() << "," << point->get_z() << "," << point->get_t() << ")" <<
        // std::endl;
        NTruthPV_Embeded0++;
      }
      NTruthPV++;
    }
  }
  // print out the truth vertex information
  std::cout << "Number of truth vertices: " << NTruthPV << std::endl;
  std::cout << "Number of truth vertices with isEmbededVtx=0: " << NTruthPV_Embeded0 << std::endl;
  std::cout << "Final truth vertex: (x,y,z)=(" << TruthPV_trig_x_ << "," << TruthPV_trig_y_ << "," << TruthPV_trig_z_ << ")" << std::endl;
  NTruthVtx_ = NTruthPV;
  // PHG4Particle
  std::cout << "Get PHG4 info.: truth primary G4Particle" << std::endl;
  const auto prange = m_truth_info->GetPrimaryParticleRange();
  // const auto prange = m_truth_info->GetParticleRange();
  for (auto iter = prange.first; iter != prange.second; ++iter)
  {
    PHG4Particle *ptcl = iter->second;
    // particle->identify();
    if (ptcl)
    {
      PrimaryG4P_PID_.push_back(ptcl->get_pid());
      TLorentzVector p;
      p.SetPxPyPzE(ptcl->get_px(), ptcl->get_py(), ptcl->get_pz(), ptcl->get_e());
      PrimaryG4P_E_.push_back(ptcl->get_e());
      PrimaryG4P_Pt_.push_back(p.Pt());
      PrimaryG4P_Eta_.push_back(p.Eta());
      PrimaryG4P_Phi_.push_back(p.Phi());
    }
  }
  NPrimaryG4P_ = PrimaryG4P_PID_.size();
}
//____________________________________________________________________________..
void dNdEtaINTT::ResetVectors()
{
  CleanVec(ClusLayer_);
  CleanVec(ClusX_);
  CleanVec(ClusY_);
  CleanVec(ClusZ_);
  CleanVec(ClusR_);
  CleanVec(ClusPhi_);
  CleanVec(ClusEta_);
  CleanVec(ClusAdc_);
  CleanVec(ClusPhiSize_);
  CleanVec(ClusZSize_);
  CleanVec(ClusLadderZId_);
  CleanVec(ClusLadderPhiId_);
  CleanVec(ClusTrkrHitSetKey_);
  CleanVec(ClusTimeBucketId_);
  CleanVec(ClusTruthCKeys_);
  CleanVec(ClusNG4Particles_);
  CleanVec(ClusNPrimaryG4Particles_);
  CleanVec(TruthClusPhiSize_);
  CleanVec(TruthClusZSize_);
  CleanVec(TruthClusNRecoClus_);
  CleanVec(PrimaryTruthClusPhiSize_);
  CleanVec(PrimaryTruthClusZSize_);
  CleanVec(PrimaryTruthClusNRecoClus_);
  CleanVec(InttRawHit_bco_);
  CleanVec(InttRawHit_packetid_);
  CleanVec(InttRawHit_word_);
  CleanVec(InttRawHit_fee_);
  CleanVec(InttRawHit_channel_id_);
  CleanVec(InttRawHit_chip_id_);
  CleanVec(InttRawHit_adc_);
  CleanVec(InttRawHit_FPHX_BCO_);
  CleanVec(InttRawHit_full_FPHX_);
  CleanVec(InttRawHit_full_ROC_);
  CleanVec(InttRawHit_amplitude_);
  CleanVec(TrkrHitRow_);
  CleanVec(TrkrHitColumn_);
  CleanVec(TrkrHitLadderZId_);
  CleanVec(TrkrHitLadderPhiId_);
  CleanVec(TrkrHitLayer_);
  CleanVec(TrkrHitADC_);
  CleanVec(HepMCFSPrtl_Pt_);
  CleanVec(HepMCFSPrtl_Eta_);
  CleanVec(HepMCFSPrtl_Phi_);
  CleanVec(HepMCFSPrtl_E_);
  CleanVec(HepMCFSPrtl_PID_);
  CleanVec(HepMCFSPrtl_prodx_);
  CleanVec(HepMCFSPrtl_prody_);
  CleanVec(HepMCFSPrtl_prodz_);
  CleanVec(PrimaryG4P_Pt_);
  CleanVec(PrimaryG4P_Eta_);
  CleanVec(PrimaryG4P_Phi_);
  CleanVec(PrimaryG4P_E_);
  CleanVec(PrimaryG4P_PID_);
}
