//module for producing a TTree with jet information for doing jet validation studies
// for questions/bugs please contact Virginia Bailey vbailey13@gsu.edu

#include "JetVertexTagging.h"

#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/PHTFileServer.h>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include <g4jets/JetMap.h>
#include <g4jets/Jetv1.h>

#include <centrality/CentralityInfo.h>

#include <TTree.h>

//____________________________________________________________________________..
JetVertexTagging::JetVertexTagging(const std::string& recojetname, const std::string& truthjetname, const std::string& outputfilename):
 SubsysReco("JetVertexTagging_" + recojetname + "_" + truthjetname)
 , m_recoJetName(recojetname)
 , m_truthJetName(truthjetname)
 , m_outputFileName(outputfilename)
 , m_etaRange(-1, 1)
 , m_ptRange(5, 100)
 , m_doTruthJets(0)
 , m_T(nullptr)
 , m_event(-1)
 , m_reco_jet_n(-1)
 , m_truth_jet_n(-1)
 , m_reco_jet_id()
 , m_reco_jet_nConstituents()
 , m_reco_jet_px()
 , m_reco_jet_py()
 , m_reco_jet_pz()
 , m_reco_jet_pt()
 , m_reco_jet_eta()
 , m_reco_jet_phi()
 , m_reco_jet_m()
 , m_reco_jet_e()
 , m_truth_jet_id()
 , m_truth_jet_nConstituents()
 , m_truth_jet_px()
 , m_truth_jet_py()
 , m_truth_jet_pz()
 , m_truth_jet_pt()
 , m_truth_jet_eta()
 , m_truth_jet_phi()
 , m_truth_jet_m()
 , m_truth_jet_e()
{
  std::cout << "JetVertexTagging::JetVertexTagging(const std::string &name) Calling ctor" << std::endl;
}

//____________________________________________________________________________..
JetVertexTagging::~JetVertexTagging()
{
  std::cout << "JetVertexTagging::~JetVertexTagging() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
int JetVertexTagging::Init(PHCompositeNode *topNode)
{
  std::cout << "JetVertexTagging::Init(PHCompositeNode *topNode) Initializing" << std::endl;
  PHTFileServer::get().open(m_outputFileName, "RECREATE");
  std::cout << "JetVertexTagging::Init - Output to " << m_outputFileName << std::endl;

  // configure Tree
  m_T = new TTree("T", "MyJetAnalysis Tree");
  m_T->Branch("m_event", &m_event, "event/I");
  m_T->Branch("cent", &m_centrality);
  m_T->Branch("b", &m_impactparam);

  m_T->Branch("m_reco_jet_n", &m_reco_jet_n, "m_reco_jet_n/I");
  m_T->Branch("m_reco_jet_id", &m_reco_jet_id);
  m_T->Branch("m_reco_jet_nConstituents", &m_reco_jet_nConstituents);
  m_T->Branch("m_reco_jet_px", &m_reco_jet_px);
  m_T->Branch("m_reco_jet_py", &m_reco_jet_py);
  m_T->Branch("m_reco_jet_pz", &m_reco_jet_pz);
  m_T->Branch("m_reco_jet_pt", &m_reco_jet_pt);
  m_T->Branch("m_reco_jet_eta", &m_reco_jet_eta);
  m_T->Branch("m_reco_jet_phi", &m_reco_jet_phi);
  m_T->Branch("m_reco_jet_m", &m_reco_jet_m);
  m_T->Branch("m_reco_jet_e", &m_reco_jet_e);
  //m_T->Branch("m_reco_jet_dR", &m_reco_jet_dR);

  if(m_doTruthJets){
    m_T->Branch("m_truth_jet_n", &m_truth_jet_n, "m_truth_jet_n/I");
    m_T->Branch("m_truth_jet_id", &m_truth_jet_id);
    m_T->Branch("m_truth_jet_nConstituents", &m_truth_jet_nConstituents);
    m_T->Branch("m_truth_jet_px", &m_truth_jet_px);
    m_T->Branch("m_truth_jet_py", &m_truth_jet_py);
    m_T->Branch("m_truth_jet_pz", &m_truth_jet_pz);
    m_T->Branch("m_truth_jet_pt", &m_truth_jet_pt);
    m_T->Branch("m_truth_jet_eta", &m_truth_jet_eta);
    m_T->Branch("m_truth_jet_phi", &m_truth_jet_phi);
    m_T->Branch("m_truth_jet_m", &m_truth_jet_m);
    m_T->Branch("m_truth_jet_e", &m_truth_jet_e);
  //  m_T->Branch("m_truth_jet_dR", &m_truth_jet_dR);
  }



  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetVertexTagging::InitRun(PHCompositeNode *topNode)
{
  std::cout << "JetVertexTagging::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetVertexTagging::process_event(PHCompositeNode *topNode)
{
  //std::cout << "JetVertexTagging::process_event(PHCompositeNode *topNode) Processing Event" << std::endl;
  ++m_event;

  // interface to reco jets
  JetMap* jets = findNode::getClass<JetMap>(topNode, m_recoJetName);
  if (!jets){
    std::cout	<< "MyJetAnalysis::process_event - Error can not find DST Reco JetMap node " << m_recoJetName << std::endl;
    exit(-1);
  }

  //interface to truth jets
  JetMap* jetsMC = findNode::getClass<JetMap>(topNode, m_truthJetName);
  if (!jetsMC && m_doTruthJets){
    std::cout	<< "MyJetAnalysis::process_event - Error can not find DST Truth JetMap node "	<< m_truthJetName << std::endl;
    exit(-1);
  }
  
  //centrality
  CentralityInfo* cent_node = findNode::getClass<CentralityInfo>(topNode, "CentralityInfo");
  if (!cent_node){
    std::cout << "MyJetAnalysis::process_event - Error can not find centrality node " << std::endl;
    exit(-1);
  }

  //get the event centrality/impact parameter from HIJING
  m_centrality =  cent_node->get_centile(CentralityInfo::PROP::bimp);
  m_impactparam =  cent_node->get_quantity(CentralityInfo::PROP::bimp);

  //get reco jets
  m_reco_jet_n = 0;
  for (JetMap::Iter iter = jets->begin(); iter != jets->end(); ++iter){
    Jet* jet = iter->second;
    
    if(jet->get_pt() < 1) continue; // to remove noise jets

    m_reco_jet_id.push_back(jet->get_id());
    m_reco_jet_nConstituents.push_back(jet->size_comp());
    m_reco_jet_px.push_back(jet->get_px());
    m_reco_jet_py.push_back(jet->get_py());
    m_reco_jet_pz.push_back(jet->get_pz());
    m_reco_jet_pt.push_back(jet->get_pt());
    m_reco_jet_eta.push_back(jet->get_eta());
    m_reco_jet_phi.push_back(jet->get_phi());
    m_reco_jet_m.push_back(jet->get_mass());
    m_reco_jet_e.push_back(jet->get_e());
    m_reco_jet_n++;
  }

  //get truth jets
  if(m_doTruthJets){
    m_truth_jet_n = 0;
    for (JetMap::Iter iter = jetsMC->begin(); iter != jetsMC->end(); ++iter){
	  Jet* truthjet = iter->second;

	  bool eta_cut = (truthjet->get_eta() >= m_etaRange.first) and (truthjet->get_eta() <= m_etaRange.second);
	  bool pt_cut = (truthjet->get_pt() >= m_ptRange.first) and (truthjet->get_pt() <= m_ptRange.second);
    if ((not eta_cut) or (not pt_cut)) continue;

    m_truth_jet_id.push_back(truthjet->get_id());
    m_truth_jet_nConstituents.push_back(truthjet->size_comp());
    m_truth_jet_px.push_back(truthjet->get_px());
    m_truth_jet_py.push_back(truthjet->get_py());
    m_truth_jet_pz.push_back(truthjet->get_pz());
    m_truth_jet_pt.push_back(truthjet->get_pt());
    m_truth_jet_eta.push_back(truthjet->get_eta());
    m_truth_jet_phi.push_back(truthjet->get_phi());
    m_truth_jet_m.push_back(truthjet->get_mass());
    m_truth_jet_e.push_back(truthjet->get_e());
	  m_truth_jet_n++;
	  }
  }
  
  //fill the tree
  m_T->Fill();

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetVertexTagging::ResetEvent(PHCompositeNode *topNode)
{
  //std::cout << "JetVertexTagging::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << std::endl;
  m_reco_jet_id.clear();
  m_reco_jet_nConstituents.clear();
  m_reco_jet_px.clear();
  m_reco_jet_py.clear();
  m_reco_jet_pz.clear();
  m_reco_jet_pt.clear();
  m_reco_jet_eta.clear();
  m_reco_jet_phi.clear();
  m_reco_jet_m.clear();
  m_reco_jet_e.clear();

  m_truth_jet_id.clear();
  m_truth_jet_nConstituents.clear();
  m_truth_jet_px.clear();
  m_truth_jet_py.clear();
  m_truth_jet_pz.clear();
  m_truth_jet_pt.clear();
  m_truth_jet_eta.clear();
  m_truth_jet_phi.clear();
  m_truth_jet_m.clear();
  m_truth_jet_e.clear();

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetVertexTagging::EndRun(const int runnumber)
{
  std::cout << "JetVertexTagging::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetVertexTagging::End(PHCompositeNode *topNode)
{
  std::cout << "JetVertexTagging::End - Output to " << m_outputFileName << std::endl;
  PHTFileServer::get().cd(m_outputFileName);

  m_T->Write();
  std::cout << "JetVertexTagging::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetVertexTagging::Reset(PHCompositeNode *topNode)
{
 std::cout << "JetVertexTagging::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void JetVertexTagging::Print(const std::string &what) const
{
  std::cout << "JetVertexTagging::Print(const std::string &what) const Printing info for " << what << std::endl;
}

