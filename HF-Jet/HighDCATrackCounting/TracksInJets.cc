
#include "TracksInJets.h"

#include <fun4all/Fun4AllReturnCodes.h>
#include <Acts/Definitions/Algebra.hpp>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include <g4main/PHG4Hit.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4VtxPoint.h>

#include <g4jets/Jet.h>
#include <g4jets/JetMap.h>

#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/SvtxVertex.h>
#include <trackbase_historic/SvtxVertexMap.h>

#include <particleflowreco/ParticleFlowElementContainer.h>
#include <particleflowreco/ParticleFlowElement.h>

#include <g4eval/JetEvalStack.h>
#include <g4eval/SvtxEvalStack.h>

#include <HepMC/GenEvent.h>
#include <HepMC/GenVertex.h>
#include <phhepmc/PHHepMCGenEvent.h>
#include <phhepmc/PHHepMCGenEventMap.h>


//____________________________________________________________________________..
TracksInJets::TracksInJets(const std::string& name,
			   const std::string& out):
 SubsysReco(name)
 , m_outfilename(out)
{
}

//____________________________________________________________________________..
TracksInJets::~TracksInJets()
{
}

//____________________________________________________________________________..
int TracksInJets::Init(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int TracksInJets::InitRun(PHCompositeNode *topNode)
{
  m_outfile = new TFile(m_outfilename.c_str(), "RECREATE");
  m_tree = new TTree("tree","A tree with tracks in jets");
  setBranches();

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int TracksInJets::process_event(PHCompositeNode *topNode)
{
  PHHepMCGenEventMap *geneventmap = findNode::getClass<PHHepMCGenEventMap>(
      topNode, "PHHepMCGenEventMap");
  if (!geneventmap)
  {
    std::cout << PHWHERE
              << " - Fatal error - missing node PHHepMCGenEventMap"
              << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  PHHepMCGenEvent *genevt = geneventmap->get(m_embeddingid);
  if (!genevt)
  {
    std::cout << PHWHERE
              << " - Fatal error - node PHHepMCGenEventMap missing subevent with embedding ID "
              << m_embeddingid;
    std::cout << ". Print PHHepMCGenEventMap:";
    geneventmap->identify();
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  JetEvalStack* jetevalstack = new JetEvalStack(topNode, m_recojetmapname, m_truthjetmapname);
  SvtxEvalStack* svtxevalstack = new SvtxEvalStack(topNode);
  if(!jetevalstack or !svtxevalstack)
    {
      std::cout << "No eval stacks, can't continue." << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  svtxevalstack->next_event(topNode);
  //SvtxTrackEval* trackeval = svtxevalstack->get_track_eval();

  JetMap *truth_jets = findNode::getClass<JetMap>(topNode, m_truthjetmapname);
  JetMap *reco_jets = findNode::getClass<JetMap>(topNode, m_recojetmapname);

  JetTruthEval* jettrutheval = jetevalstack->get_truth_eval();

  ParticleFlowElementContainer* pflowcontainer = findNode::getClass<ParticleFlowElementContainer>(topNode, "ParticleFlowElements");
  if(!pflowcontainer)
    {
      std::cout << "No particle flow elements on node tree, can't continue."
		<< std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  //HepMC::GenEvent *theEvent = genevt->getEvent();
 

  PHG4TruthInfoContainer *truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");

  PHG4VtxPoint *first_point = truthinfo->GetPrimaryVtx(truthinfo->GetPrimaryVertexIndex());
  
  float truth_vx = first_point->get_x();
  float truth_vy = first_point->get_y();
  float truth_vz = first_point->get_z();


  for (JetMap::Iter iter = truth_jets->begin(); iter != truth_jets->end();
       ++iter)
  {
    Jet *truthjet = iter->second;
    if(truthjet->get_pt() < m_minjettruthpt) 
      { continue; }
    if(fabs(truthjet->get_eta()) > 2)
      { continue; }

    m_truthjetpx = truthjet->get_px();
    m_truthjetpy = truthjet->get_py();
    m_truthjetpz = truthjet->get_pz();
    m_truthjete = truthjet->get_e();

    std::set<PHG4Particle*> truthjetconst =
      jettrutheval->all_truth_particles(truthjet);
    m_truthjetconst = 0;
    for(const PHG4Particle* truthpart : truthjetconst)
      {
	int truthpid = truthpart->get_pid();
	int fabstruthpid = fabs(truthpid);
	if(! (fabstruthpid == 211 or fabstruthpid == 321 or fabstruthpid == 2212 or fabstruthpid == 11 or fabstruthpid == 13))
	  { continue; }
	m_truthjettrackpx.push_back(truthpart->get_px());
	m_truthjettrackpy.push_back(truthpart->get_py());
	m_truthjettrackpz.push_back(truthpart->get_pz());
	m_truthjettrackvx.push_back(truth_vx);
	m_truthjettrackvy.push_back(truth_vy);
	m_truthjettrackvz.push_back(truth_vz);
	
	m_truthjetconst++;
      }

    Jet* matchedrecojet = nullptr;
    float mindr = std::numeric_limits<float>::max();
    for (JetMap::Iter riter = reco_jets->begin(); riter != reco_jets->end();
	 ++riter)
      {
	Jet* mjet = riter->second;
	if(mjet->get_pt() < 5)
	  { continue; }
	float dr = dR(mjet->get_eta(), truthjet->get_eta(),
		      mjet->get_phi(), truthjet->get_phi());
	if(dr < mindr)
	  {
	    mindr = dr;
	    matchedrecojet = mjet;
	  }
      }

    if(!matchedrecojet)
      {
	/// no good jet found, continue
	m_recojetpx = std::numeric_limits<float>::max();
	m_recojetpy = std::numeric_limits<float>::max();
	m_recojetpz = std::numeric_limits<float>::max();
	m_recojete = std::numeric_limits<float>::max();
	m_recojetconst = 0;
	m_tree->Fill();
	continue;
      }

    /// matchedrecojet is the reco jet matched to truthjet
    m_recojetpx = matchedrecojet->get_px();
    m_recojetpy = matchedrecojet->get_py();
    m_recojetpz = matchedrecojet->get_pz();
    m_recojete = matchedrecojet->get_e();

    m_recojetconst = matchedrecojet->size_comp();
    for(auto citer = matchedrecojet->begin_comp(); citer != matchedrecojet->end_comp(); ++citer)
      {
	ParticleFlowElement *pflow = pflowcontainer->getParticleFlowElement(citer->second);
	ParticleFlowElement::PFLOWTYPE type = pflow->get_type();
	
	if(!(type == ParticleFlowElement::PFLOWTYPE::MATCHED_CHARGED_HADRON or
	     type == ParticleFlowElement::PFLOWTYPE::UNMATCHED_CHARGED_HADRON))
	  { continue; }
	
	m_recojettrackpx.push_back(pflow->get_px());
	m_recojettrackpy.push_back(pflow->get_py());
	m_recojettrackpz.push_back(pflow->get_pz());
	m_recojettracke.push_back(pflow->get_e());
	m_recojettracktype.push_back(pflow->get_type());
      }

    m_tree->Fill();

  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int TracksInJets::ResetEvent(PHCompositeNode *topNode)
{
  m_truthjettrackpx.clear();
  m_truthjettrackpy.clear();
  m_truthjettrackpz.clear();
  m_truthjettrackpid.clear();
  m_truthjettrackvx.clear();
  m_truthjettrackvy.clear();
  m_truthjettrackvz.clear();
  m_recojettrackpx.clear();
  m_recojettrackpy.clear();
  m_recojettrackpz.clear();
  m_recojettracke.clear();
  m_recojettrackvx.clear();
  m_recojettrackvy.clear();
  m_recojettrackvz.clear();
  m_recojettracktype.clear();
  m_recojettrackpcax.clear();
  m_recojettrackpcay.clear();
  m_recojettrackpcaz.clear();
  m_recojettracktype.clear();

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int TracksInJets::End(PHCompositeNode *topNode)
{
  m_outfile->Write();
  m_outfile->Close();
  return Fun4AllReturnCodes::EVENT_OK;
}

void TracksInJets::setBranches()
{
  m_tree->Branch("truthjetpx", &m_truthjetpx, "truthjetpx/F");
  m_tree->Branch("truthjetpy", &m_truthjetpy, "truthjetpy/F");
  m_tree->Branch("truthjetpz", &m_truthjetpz, "truthjetpz/F");
  m_tree->Branch("truthjete", &m_truthjete, "truthjete/F");
  m_tree->Branch("recojetpx", &m_recojetpx, "recojetpx/F");
  m_tree->Branch("recojetpy", &m_recojetpy, "recojetpy/F");
  m_tree->Branch("recojetpz", &m_recojetpz, "recojetpz/F");
  m_tree->Branch("recojete", &m_recojete, "recojete/F");
  m_tree->Branch("truthjetconst", &m_truthjetconst, "truthjetconst/I");
  m_tree->Branch("recojetconst", &m_recojetconst, "recojetconst/I");

  m_tree->Branch("truthjettrackpx",&m_truthjettrackpx);
  m_tree->Branch("truthjettrackpy", &m_truthjettrackpy);
  m_tree->Branch("truthjettrackpz", &m_truthjettrackpz);
  m_tree->Branch("truthjettrackpid", &m_truthjettrackpid);
  m_tree->Branch("truthjettrackvx", &m_truthjettrackvx);
  m_tree->Branch("truthjettrackvy", &m_truthjettrackvy);
  m_tree->Branch("truthjettrackvz", &m_truthjettrackvz);

  m_tree->Branch("recojettrackpx",&m_recojettrackpx);
  m_tree->Branch("recojettrackpy", &m_recojettrackpy);
  m_tree->Branch("recojettrackpz", &m_recojettrackpz);
  m_tree->Branch("recojettracke", &m_recojettracke);
  m_tree->Branch("recojettrackvx", &m_recojettrackvx);
  m_tree->Branch("recojettrackvy", &m_recojettrackvy);
  m_tree->Branch("recojettrackvz", &m_recojettrackvz);
  m_tree->Branch("recojettrackpcax", &m_recojettrackpcax);
  m_tree->Branch("recojettrackpcay", &m_recojettrackpcay);
  m_tree->Branch("recojettrackpcaz", &m_recojettrackpcaz);
  m_tree->Branch("recojettracktype", &m_recojettracktype);


}


float TracksInJets::dR(const float& eta1, const float& eta2,  
		       const float& phi1, const float& phi2)
{
  float deta = eta1-eta2;
  float dphi = phi1-phi2;
  if(dphi > M_PI)
    { dphi -= 2. * M_PI; }
  if(dphi < -M_PI)
    { dphi += 2. * M_PI; }

  return sqrt(pow(deta,2) + pow(dphi,2));

}
