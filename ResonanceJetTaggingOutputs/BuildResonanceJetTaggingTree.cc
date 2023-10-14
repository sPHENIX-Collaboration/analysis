#include "BuildResonanceJetTaggingTree.h"

#include <resonancejettagging/ResonanceJetTagging.h>

#include <phool/phool.h>

/// Jet includes
#include <jetbase/JetMap.h>
#include <jetbase/Jetv1.h>

/// Tracking includes
#include <trackbase_historic/SvtxPHG4ParticleMap_v1.h>
#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrackMap.h>

#include <g4eval/SvtxEvalStack.h>   // for SvtxEvalStack
#include <g4eval/SvtxTrackEval.h>   // for SvtxTrackEval

/// HEPMC truth includes
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <HepMC/GenEvent.h>
#include <HepMC/GenVertex.h>
#pragma GCC diagnostic pop

#include <phhepmc/PHHepMCGenEvent.h>
#include <phhepmc/PHHepMCGenEventMap.h>
#include <phhepmc/PHGenIntegral.h>

/// Fun4All includes
#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include <g4main/PHG4Particle.h>            // for PHG4Particle
#include <g4main/PHG4TruthInfoContainer.h>  // for PHG4TruthInfoContainer
#include <g4main/PHG4VtxPoint.h>            // for PHG4VtxPoint

#include <kfparticle_sphenix/KFParticle_truthAndDetTools.h>

#include <KFParticle.h>
#include <kfparticle_sphenix/KFParticle_Container.h>

/// ROOT includes
#include <TFile.h>
#include <TTree.h>
#include <TH1I.h>

/// C++ includes
#include <cassert>
#include <sstream>
#include <string>

/**
 * BuildResonanceJetTaggingTree is a class developed to reconstruct jets containing a D-meson
 * The class can be adapted to tag jets using any kind of particle
 * Author: Antonio Silva (antonio.sphenix@gmail.com)
 * Contributor: Jakub Kvapil (jakub.kvapil@cern.ch)
 */

/**
 * Constructor of module
 */
BuildResonanceJetTaggingTree::BuildResonanceJetTaggingTree(const std::string &name, const std::string &filename, const ResonanceJetTagging::TAG tag)
  : SubsysReco(name)
  , m_outfilename(filename)
  , m_tagcontainer_name("")
  , m_jetcontainer_name("")
  , m_truth_jetcontainer_name("")
  , m_dorec(true)
  , m_dotruth(false)
  , m_nDaughters(0)
  , m_svtx_evalstack(nullptr)
  , m_trackeval(nullptr)
  , m_tag_particle(tag)
  , m_tag_pdg(0)
  , m_outfile(nullptr)
  , m_eventcount_h(nullptr)
  , m_taggedjettree(nullptr)
{
  /// Initialize variables and trees so we don't accidentally access
  /// memory that was never allocated
  initializeVariables();
  initializeTrees();

  switch (m_tag_particle) {
    case ResonanceJetTagging::TAG::D0:
      m_tag_pdg = 421;
      m_nDaughters = 2;
      break;
    case ResonanceJetTagging::TAG::D0TOK3PI:
      m_tag_pdg = 421;
      m_nDaughters = 4;
      break;
    case ResonanceJetTagging::TAG::DPLUS:
      m_tag_pdg = 411;
      m_nDaughters = 3;
      break;
    case ResonanceJetTagging::TAG::DSTAR:
      m_tag_pdg = 413;
      m_nDaughters = 0;
      break;
    case ResonanceJetTagging::TAG::JPSY:
      m_tag_pdg = 433;
      m_nDaughters = 0;
      break;
    case ResonanceJetTagging::TAG::K0:
      m_tag_pdg = 311;
      m_nDaughters = 0;
      break;
    case ResonanceJetTagging::TAG::GAMMA:
      m_tag_pdg = 22;
      m_nDaughters = 0;
      break;
    case ResonanceJetTagging::TAG::ELECTRON:
      m_tag_pdg = 11;
      m_nDaughters = 0;
      break;
    case ResonanceJetTagging::TAG::LAMBDAC:
      m_tag_pdg = 4122;
      m_nDaughters = 3;
      break;
  }
}

/**
 * Destructor of module
 */
BuildResonanceJetTaggingTree::~BuildResonanceJetTaggingTree()
{

}

/**
 * Initialize the module and prepare looping over events
 */
int BuildResonanceJetTaggingTree::Init(PHCompositeNode */*topNode*/)
{
  if (Verbosity() > 5)
  {
    std::cout << "Beginning Init in BuildResonanceJetTaggingTree" << std::endl;
  }

  return 0;
}

/**
 * Main workhorse function where each event is looped over and
 * data from each event is collected from the node tree for analysis
 */
int BuildResonanceJetTaggingTree::process_event(PHCompositeNode *topNode)
{
  if (Verbosity() > 5)
  {
    std::cout << "Beginning process_event in BuildResonanceJetTaggingTree" << std::endl;
  }

  if(m_nDaughters == 0)
  {
    std::cout<<"ERROR: Number of Decay Daughters Not Set, ABORTING!";
    return Fun4AllReturnCodes::ABORTRUN;
  }

  m_eventcount_h->Fill(0);

  switch (m_tag_particle) {
    case ResonanceJetTagging::TAG::D0:
      [[fallthrough]];
    case ResonanceJetTagging::TAG::D0TOK3PI:
      [[fallthrough]];
    case ResonanceJetTagging::TAG::DPLUS:
      [[fallthrough]];
    case ResonanceJetTagging::TAG::LAMBDAC:
      return loopHFHadronic(topNode);
      break;
    default:
      std::cout<<"ERROR: Fill Tree Function Not Set, ABORTING!";
      return Fun4AllReturnCodes::ABORTRUN;
      break;
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

/**
 * End the module and finish any data collection. Clean up any remaining
 * loose ends
 */
int BuildResonanceJetTaggingTree::End(PHCompositeNode *topNode)
{
  if (Verbosity() > 1)
  {
    std::cout << "Ending BuildResonanceJetTaggingTree analysis package" << std::endl;
  }

  /// Change to the outfile
  m_outfile->cd();

  if(m_dotruth) {
    PHGenIntegral *phgen = findNode::getClass<PHGenIntegral>(topNode, "PHGenIntegral");
    if(phgen)
      {
	m_intlumi = phgen->get_Integrated_Lumi();
	m_nprocessedevents = phgen->get_N_Processed_Event();
	m_nacceptedevents = phgen->get_N_Generator_Accepted_Event();
	m_xsecprocessedevents = phgen->get_CrossSection_Processed_Event();
	m_xsecacceptedevents = phgen->get_CrossSection_Generator_Accepted_Event();
	m_runinfo->Fill();
      }
    m_runinfo->Write();
  }

  m_taggedjettree->Write();
  m_eventcount_h->Write();

  m_outfile->Close();

  delete m_outfile;

  if (Verbosity() > 1)
  {
    std::cout << "Finished BuildResonanceJetTaggingTree analysis package" << std::endl;
  }

  return 0;
}

int BuildResonanceJetTaggingTree::loopHFHadronic(PHCompositeNode *topNode)
{
  KFParticle_Container* kfContainer = nullptr;

  if(m_dorec)
  {
    m_taggedJetMap = getJetMapFromNode(topNode, m_jetcontainer_name);
    if(!m_taggedJetMap) return Fun4AllReturnCodes::ABORTEVENT;

    kfContainer = getKFParticleContainerFromNode(topNode, m_tagcontainer_name);
    if(!kfContainer) return Fun4AllReturnCodes::ABORTEVENT;
  }

  HepMC::GenEvent *hepMCGenEvent = nullptr;

  if(m_dotruth)
  {
    m_truth_taggedJetMap = getJetMapFromNode(topNode, m_truth_jetcontainer_name);
    if(!m_truth_taggedJetMap) return Fun4AllReturnCodes::ABORTEVENT;

    hepMCGenEvent = getGenEventFromNode(topNode, "PHHepMCGenEventMap");
    if(!hepMCGenEvent) return Fun4AllReturnCodes::ABORTEVENT;

    if (!m_svtx_evalstack)
    {
      m_svtx_evalstack = new SvtxEvalStack(topNode);

      m_trackeval = m_svtx_evalstack->get_track_eval();
    }
    else
    {
      m_svtx_evalstack->next_event(topNode);
    }

  }

  m_eventcount_h->Fill(1);

  Jet *recTagJet = nullptr;
  Jet *genTagJet = nullptr;

  KFParticle *recTag = nullptr;
  HepMC::GenParticle *genTag = nullptr;

  std::vector<int> recDaughtersID(m_nDaughters);
  std::vector<int> recJetIndex;

  if(m_dorec)
  {
    for (JetMap::Iter iter = m_taggedJetMap->begin(); iter != m_taggedJetMap->end(); ++iter)
    {
      recTagJet = iter->second;

      if(!recTagJet) continue;

      Jet::Iter recTagIter = recTagJet->find(Jet::SRC::VOID);

      if(recTagIter == recTagJet->end_comp()) continue;

      recTag = kfContainer->get(recTagIter->second);

      if(!recTag) continue;

      for (int iDaughter = 0; iDaughter < m_nDaughters; iDaughter++){
        recDaughtersID[iDaughter] = (kfContainer->get(recTagIter->second + iDaughter +1))->Id();
      }

      resetTreeVariables();

      m_reco_tag_px = recTag->Px();
      m_reco_tag_py = recTag->Py();
      m_reco_tag_pz = recTag->Pz();
      m_reco_tag_pt = recTag->GetPt();
      m_reco_tag_eta = recTag->GetEta();
      m_reco_tag_phi = recTag->GetPhi();
      m_reco_tag_m = recTag->GetMass();
      m_reco_tag_e = recTag->E();

      m_reco_jet_px = recTagJet->get_px();
      m_reco_jet_py = recTagJet->get_py();
      m_reco_jet_pz = recTagJet->get_pz();
      m_reco_jet_pt = recTagJet->get_pt();
      m_reco_jet_eta = recTagJet->get_eta();
      m_reco_jet_phi = recTagJet->get_phi();
      m_reco_jet_m = recTagJet->get_mass();
      m_reco_jet_e = recTagJet->get_e();

      genTagJet = nullptr;
      genTag = nullptr;

      if(m_dotruth)
      {
        findMatchedTruthD0(topNode, genTagJet, genTag, recDaughtersID);

        if((genTagJet) && (genTag))
        {
          recJetIndex.push_back(genTagJet->get_id());

          m_truth_tag_px = genTag->momentum().px();
          m_truth_tag_py = genTag->momentum().py();
          m_truth_tag_pz = genTag->momentum().pz();
          m_truth_tag_pt = std::sqrt(m_truth_tag_px * m_truth_tag_px + m_truth_tag_py * m_truth_tag_py);
          m_truth_tag_eta = genTag->momentum().pseudoRapidity();
          m_truth_tag_phi = atan2(m_truth_tag_py, m_truth_tag_px);
	  m_truth_tag_m = genTag->momentum().m();
	  m_truth_tag_e = genTag->momentum().e();

          m_truth_jet_px = genTagJet->get_px();
          m_truth_jet_py = genTagJet->get_py();
          m_truth_jet_pz = genTagJet->get_pz();
          m_truth_jet_pt = genTagJet->get_pt();
          m_truth_jet_eta = genTagJet->get_eta();
          m_truth_jet_phi = genTagJet->get_phi();
          m_truth_jet_m = genTagJet->get_mass();
          m_truth_jet_e = genTagJet->get_e();

        }
      }

      m_taggedjettree->Fill();
    }
  }

  if(m_dotruth)
  {
    resetTreeVariables();

    for (JetMap::Iter iter = m_truth_taggedJetMap->begin(); iter != m_truth_taggedJetMap->end(); ++iter)
    {
      genTagJet = iter->second;

      if(!genTagJet) continue;

      //Check if truth was matched to reconstructed
      if(m_dorec) {
	if(isReconstructed(genTagJet->get_id(), recJetIndex)) continue;
      }

      Jet::Iter genTagIter = genTagJet->find(Jet::SRC::VOID);

      genTag = hepMCGenEvent->barcode_to_particle(genTagIter->second);

      m_truth_tag_px = genTag->momentum().px();
      m_truth_tag_py = genTag->momentum().py();
      m_truth_tag_pz = genTag->momentum().pz();
      m_truth_tag_pt = std::sqrt(m_truth_tag_px * m_truth_tag_px + m_truth_tag_py * m_truth_tag_py);
      m_truth_tag_eta = genTag->momentum().pseudoRapidity();
      m_truth_tag_phi = atan2(m_truth_tag_py, m_truth_tag_px);
      m_truth_tag_m = genTag->momentum().m();
      m_truth_tag_e = genTag->momentum().e();

      m_truth_jet_px = genTagJet->get_px();
      m_truth_jet_py = genTagJet->get_py();
      m_truth_jet_pz = genTagJet->get_pz();
      m_truth_jet_pt = genTagJet->get_pt();
      m_truth_jet_eta = genTagJet->get_eta();
      m_truth_jet_phi = genTagJet->get_phi();
      m_truth_jet_m = genTagJet->get_mass();
      m_truth_jet_e = genTagJet->get_e();

      /// iterate over all constituents and add them to the tree
      for(auto citer = genTagJet->begin_comp(); citer != genTagJet->end_comp();
	  ++citer)
	{
	  HepMC::GenParticle* constituent = hepMCGenEvent->barcode_to_particle(citer->second);
	  /// Don't include the tagged particle
	  if(constituent == genTag)
	    {
	      continue;
	    }

	  m_truthjet_const_px.push_back(constituent->momentum().px());
	  m_truthjet_const_py.push_back(constituent->momentum().py());
	  m_truthjet_const_pz.push_back(constituent->momentum().pz());
	  m_truthjet_const_e.push_back(constituent->momentum().e());

	}

      m_taggedjettree->Fill();
    }
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

void BuildResonanceJetTaggingTree::findMatchedTruthD0(PHCompositeNode *topNode, Jet *&mcTagJet, HepMC::GenParticle *&mcTag, std::vector<int> decays)
{
  m_truth_taggedJetMap = getJetMapFromNode(topNode, m_truth_jetcontainer_name);
  if(!m_truth_taggedJetMap) return;

  HepMC::GenEvent *hepMCGenEvent = getGenEventFromNode(topNode, "PHHepMCGenEventMap");
  if(!hepMCGenEvent) return;

  PHG4Particle *g4particle = nullptr;
  PHG4Particle *g4parent = nullptr;
  std::vector<HepMC::GenParticle*> mcTags(m_nDaughters);

  PHG4TruthInfoContainer *truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");

  // Truth map
  SvtxPHG4ParticleMap_v1 *dst_reco_truth_map = findNode::getClass<SvtxPHG4ParticleMap_v1>(topNode, "SvtxPHG4ParticleMap");

  if (dst_reco_truth_map)
  {
    for (int idecay = 0; idecay < m_nDaughters; idecay++)
    {
      std::map<float, std::set<int>> truth_set = dst_reco_truth_map->get(decays[idecay]);
      const auto &best_weight = truth_set.rbegin();
      int best_truth_id = *best_weight->second.rbegin();
      g4particle = truthinfo->GetParticle(best_truth_id);
      mcTags[idecay] = getMother(topNode, g4particle);
      if (mcTags[idecay] == nullptr)
      {
        return;
      }
    }
  }
  else
  {
    SvtxTrackMap *trackmap = findNode::getClass<SvtxTrackMap>(topNode, "SvtxTrackMap");
    if(!trackmap) return;

    for (int idecay = 0; idecay < m_nDaughters; idecay++)
    {
      SvtxTrack *track = trackmap->get(decays[idecay]);
      if(!track) return;
      g4particle = m_trackeval->max_truth_particle_by_nclusters(track);

      if(!g4particle)
      {
        return;
      }

      g4parent = truthinfo->GetParticle(g4particle->get_primary_id());

      if(g4parent == nullptr)
      {
        return;
      }

      mcTags[idecay] = hepMCGenEvent->barcode_to_particle(g4parent->get_barcode());
      if(mcTags[idecay] == nullptr)
      {
        return;
      }
    }
  }

  // check is decays are from the same mother, otherwise it is background
  for (int idecay = 1; idecay < m_nDaughters; idecay++)
  {
    if (mcTags[idecay]->barcode() != mcTags[idecay - 1]->barcode())
    {
      return;
    }
  }

  mcTag = mcTags[0];

  for (JetMap::Iter iter = m_truth_taggedJetMap->begin(); iter != m_truth_taggedJetMap->end(); ++iter)
  {
    mcTagJet = iter->second;

    Jet::Iter mcTagIter = mcTagJet->find(Jet::SRC::VOID);

    if(int(mcTagIter->second) != mcTag->barcode())
    {
      mcTagJet = nullptr;
      continue;
    }
    else
    {
      break;
    }
  }
  return;
}

HepMC::GenParticle *BuildResonanceJetTaggingTree::getMother(PHCompositeNode *topNode, PHG4Particle *g4daughter)
{
  PHHepMCGenEventMap *hepmceventmap = findNode::getClass<PHHepMCGenEventMap>(topNode, "PHHepMCGenEventMap");

  /// If the node was not properly put on the tree, return
  if (!hepmceventmap)
  {
    std::cout << PHWHERE
              << "HEPMC event map node is missing, can't collected HEPMC truth particles"
              << std::endl;
    return nullptr;
  }

  PHHepMCGenEvent *hepmcevent = hepmceventmap->get(1);
  if (!hepmcevent)
  {
    std::cout << "no hepmcevent!!!" << std::endl;
    return nullptr;
  }

  HepMC::GenEvent *hepMCGenEvent = hepmcevent->getEvent();
  if (!hepMCGenEvent)
  {
    return nullptr;
  }

  HepMC::GenParticle *mcDaughter = nullptr;

  if (g4daughter->get_barcode() > 0)
  {
    mcDaughter = hepMCGenEvent->barcode_to_particle(g4daughter->get_barcode());
  }
  if (!mcDaughter)
  {
    return nullptr;
  }

  HepMC::GenVertex *TagVertex = mcDaughter->production_vertex();
  for (HepMC::GenVertex::particle_iterator it = TagVertex->particles_begin(HepMC::ancestors); it != TagVertex->particles_end(HepMC::ancestors); ++it)
  {
    if (std::abs((*it)->pdg_id()) == m_tag_pdg)
    {
      return (*it);
    }
  }

  return nullptr;
}

bool BuildResonanceJetTaggingTree::isReconstructed(int index, std::vector<int> indexRecVector)
{
  for(auto indexRec : indexRecVector)
  {
    if(index == indexRec) return true;
  }
  return false;
}

void BuildResonanceJetTaggingTree::initializeTrees()
{
  delete m_runinfo;
  m_runinfo = new TTree("m_runinfo","A tree with the run information");
  m_runinfo->Branch("m_intlumi",&m_intlumi, "m_intlumi/F");
  m_runinfo->Branch("m_nprocessedevents", &m_nprocessedevents, "m_nprocessedevents/F");
  m_runinfo->Branch("m_nacceptedevents", &m_nacceptedevents, "m_nacceptedevents/F");
  m_runinfo->Branch("m_xsecprocessedevents", &m_xsecprocessedevents, "m_xsecprocessedevents/F");
  m_runinfo->Branch("m_xsecacceptedevents", &m_xsecacceptedevents, "m_xsecacceptedevents/F");

  delete m_taggedjettree;
  m_taggedjettree = new TTree("m_taggedjettree", "A tree with Tagged-Jet info");
  m_taggedjettree->Branch("m_reco_tag_px", &m_reco_tag_px, "m_reco_tag_px/F");
  m_taggedjettree->Branch("m_reco_tag_py", &m_reco_tag_py, "m_reco_tag_py/F");
  m_taggedjettree->Branch("m_reco_tag_pz", &m_reco_tag_pz, "m_reco_tag_pz/F");
  m_taggedjettree->Branch("m_reco_tag_pt", &m_reco_tag_pt, "m_reco_tag_pt/F");
  m_taggedjettree->Branch("m_reco_tag_eta", &m_reco_tag_eta, "m_reco_tag_eta/F");
  m_taggedjettree->Branch("m_reco_tag_phi", &m_reco_tag_phi, "m_reco_tag_phi/F");
  m_taggedjettree->Branch("m_reco_tag_m", &m_reco_tag_m, "m_reco_tag_m/F");
  m_taggedjettree->Branch("m_reco_tag_e", &m_reco_tag_e, "m_reco_tag_e/F");
  m_taggedjettree->Branch("m_reco_jet_px", &m_reco_jet_px, "m_reco_jet_px/F");
  m_taggedjettree->Branch("m_reco_jet_py", &m_reco_jet_py, "m_reco_jet_py/F");
  m_taggedjettree->Branch("m_reco_jet_pz", &m_reco_jet_pz, "m_reco_jet_pz/F");
  m_taggedjettree->Branch("m_reco_jet_pt", &m_reco_jet_pt, "m_reco_jet_pt/F");
  m_taggedjettree->Branch("m_reco_jet_eta", &m_reco_jet_eta, "m_reco_jet_eta/F");
  m_taggedjettree->Branch("m_reco_jet_phi", &m_reco_jet_phi, "m_reco_jet_phi/F");
  m_taggedjettree->Branch("m_reco_jet_m", &m_reco_jet_m, "m_reco_jet_m/F");
  m_taggedjettree->Branch("m_reco_jet_e", &m_reco_jet_e, "m_reco_jet_e/F");

  m_taggedjettree->Branch("m_truth_tag_px", &m_truth_tag_px, "m_truth_tag_px/F");
  m_taggedjettree->Branch("m_truth_tag_py", &m_truth_tag_py, "m_truth_tag_py/F");
  m_taggedjettree->Branch("m_truth_tag_pz", &m_truth_tag_pz, "m_truth_tag_pz/F");
  m_taggedjettree->Branch("m_truth_tag_pt", &m_truth_tag_pt, "m_truth_tag_pt/F");
  m_taggedjettree->Branch("m_truth_tag_eta", &m_truth_tag_eta, "m_truth_tag_eta/F");
  m_taggedjettree->Branch("m_truth_tag_phi", &m_truth_tag_phi, "m_truth_tag_phi/F");
  m_taggedjettree->Branch("m_truth_tag_m", &m_truth_tag_m, "m_truth_tag_m/F");
  m_taggedjettree->Branch("m_truth_tag_e", &m_truth_tag_e, "m_truth_tag_e/F");
  m_taggedjettree->Branch("m_truth_jet_px", &m_truth_jet_px, "m_truth_jet_px/F");
  m_taggedjettree->Branch("m_truth_jet_py", &m_truth_jet_py, "m_truth_jet_py/F");
  m_taggedjettree->Branch("m_truth_jet_pz", &m_truth_jet_pz, "m_truth_jet_pz/F");
  m_taggedjettree->Branch("m_truth_jet_pt", &m_truth_jet_pt, "m_truth_jet_pt/F");
  m_taggedjettree->Branch("m_truth_jet_eta", &m_truth_jet_eta, "m_truth_jet_eta/F");
  m_taggedjettree->Branch("m_truth_jet_phi", &m_truth_jet_phi, "m_truth_jet_phi/F");
  m_taggedjettree->Branch("m_truth_jet_m", &m_truth_jet_m, "m_truth_jet_m/F");
  m_taggedjettree->Branch("m_truth_jet_e", &m_truth_jet_e, "m_truth_jet_e/F");
  m_taggedjettree->Branch("m_truthjet_const_px", &m_truthjet_const_px);
  m_taggedjettree->Branch("m_truthjet_const_py", &m_truthjet_const_py);
  m_taggedjettree->Branch("m_truthjet_const_pz", &m_truthjet_const_pz);
  m_taggedjettree->Branch("m_truthjet_const_e", &m_truthjet_const_e);

}
void BuildResonanceJetTaggingTree::initializeVariables()
{
  delete m_outfile;
  m_outfile = new TFile(m_outfilename.c_str(), "RECREATE");

  delete m_eventcount_h;
  m_eventcount_h = new TH1I("eventcount_h", "Event Count", 2, -0.5, 1.5);
  m_eventcount_h->GetXaxis()->SetBinLabel(1,"N raw ev anal");
  m_eventcount_h->GetXaxis()->SetBinLabel(2,"N ev anal");
}

void BuildResonanceJetTaggingTree::resetTreeVariables()
{
  // Tagged-Jet reconstructed variables
  m_reco_tag_px = NAN;
  m_reco_tag_py = NAN;
  m_reco_tag_pz = NAN;
  m_reco_tag_pt = NAN;
  m_reco_tag_eta = NAN;
  m_reco_tag_phi = NAN;
  m_reco_tag_m = NAN;
  m_reco_tag_e = NAN;
  m_reco_jet_px = NAN;
  m_reco_jet_py = NAN;
  m_reco_jet_pz = NAN;
  m_reco_jet_pt = NAN;
  m_reco_jet_eta = NAN;
  m_reco_jet_phi = NAN;
  m_reco_jet_m = NAN;
  m_reco_jet_e = NAN;
  //Truth info
  m_truth_tag_px = NAN;
  m_truth_tag_py = NAN;
  m_truth_tag_pz = NAN;
  m_truth_tag_pt = NAN;
  m_truth_tag_eta = NAN;
  m_truth_tag_phi = NAN;
  m_truth_tag_m = NAN;
  m_truth_tag_e = NAN;
  m_truth_jet_px = NAN;
  m_truth_jet_py = NAN;
  m_truth_jet_pz = NAN;
  m_truth_jet_pt = NAN;
  m_truth_jet_eta = NAN;
  m_truth_jet_phi = NAN;
  m_truth_jet_m = NAN;
  m_truth_jet_e = NAN;

  m_truthjet_const_px.clear();
  m_truthjet_const_py.clear();
  m_truthjet_const_pz.clear();
  m_truthjet_const_e.clear();
}

JetMapv1* BuildResonanceJetTaggingTree::getJetMapFromNode(PHCompositeNode *topNode, const std::string &name)
{
  JetMapv1 *jetmap = findNode::getClass<JetMapv1>(topNode, name);

  if (!jetmap)
  {
    std::cout << PHWHERE
         << "JetMap node is missing, can't collect jets"
         << std::endl;
    return 0;
  }

  return jetmap;
}
KFParticle_Container* BuildResonanceJetTaggingTree::getKFParticleContainerFromNode(PHCompositeNode *topNode, const std::string &name)
{
  KFParticle_Container *cont = findNode::getClass<KFParticle_Container>(topNode, name);

  if (!cont)
  {
    std::cout << PHWHERE
         << "KFParticle_Container node is missing, can't collect HF particles"
         << std::endl;
    return 0;
  }

  return cont;
}
HepMC::GenEvent* BuildResonanceJetTaggingTree::getGenEventFromNode(PHCompositeNode *topNode, const std::string &name)
{
  PHHepMCGenEventMap *hepmceventmap = findNode::getClass<PHHepMCGenEventMap>(topNode, name);

  /// If the node was not properly put on the tree, return
  if (!hepmceventmap)
  {
    std::cout << PHWHERE
         << "HEPMC event map node is missing, can't collected HEPMC truth particles"
         << std::endl;
    return 0;
  }

  PHHepMCGenEvent *hepmcevent = hepmceventmap->get(1);

  if (!hepmcevent)
  {
    std::cout << PHWHERE
         << "PHHepMCGenEvent node is missing, can't collected HEPMC truth particles"
         << std::endl;
    return 0;
  }

  HepMC::GenEvent *hepMCGenEvent = hepmcevent->getEvent();

  if (!hepmceventmap)
  {
    std::cout << PHWHERE
         << "HepMC::GenEvent node is missing, can't collected HEPMC truth particles"
         << std::endl;
    return 0;
  }

  return hepMCGenEvent;

}
