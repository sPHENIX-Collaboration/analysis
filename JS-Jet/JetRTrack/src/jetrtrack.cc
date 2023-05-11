#include "jetrtrack.h"
#include <fun4all/SubsysReco.h>  // for SubsysReco

#include <fun4all/Fun4AllReturnCodes.h>
#include <TFile.h>
#include <TH1F.h>
#include <fun4all/Fun4AllHistoManager.h>

#include <phool/PHCompositeNode.h>
#include <g4jets/JetMap.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/PHTFileServer.h>
#include <g4main/PHG4VtxPoint.h>
#include <TTree.h>
#include <phool/PHCompositeNode.h>

#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>    // for PHIODataNode
#include <phool/PHNode.h>          // for PHNode
#include <phool/PHNodeIterator.h>  // for PHNodeIterator
#include <phool/PHObject.h>        // for PHObject
#include <phool/getClass.h>

#include <iostream>
#include <sstream>
#include <string>
#include <phool/phool.h>                    // for PHWHERE

#include <g4main/PHG4Particle.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <centrality/CentralityInfo.h>

#include <trackbase/TrkrCluster.h>
#include <trackbase/TrkrClusterv3.h>
#include <trackbase/TrkrClusterv4.h>
#include <trackbase/TrkrHit.h>
#include <trackbase/TrkrHitSetContainer.h>
#include <trackbase/TrkrDefs.h>
#include <trackbase/ActsGeometry.h>
#include <trackbase/ClusterErrorPara.h>

#include <trackbase/TpcDefs.h>

#include <trackbase/TrkrClusterContainer.h>
#include <trackbase/TrkrHitSet.h>
#include <trackbase/TrkrClusterHitAssoc.h>
#include <trackbase/TrkrClusterIterationMapv1.h>

#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/SvtxVertex.h>
#include <trackbase_historic/SvtxVertexMap.h>
#include <trackbase_historic/ActsTransformations.h>
#include <trackbase_historic/TrackSeed.h>
#include <g4main/PHG4Particle.h>

#include <g4main/PHG4TruthInfoContainer.h>



//____________________________________________________________________________..
jetrtrack::jetrtrack(const std::string &name):
 SubsysReco(name)
{
  std::cout << "jetrtrack::jetrtrack(const std::string &name) Calling ctor" << std::endl;
}

//____________________________________________________________________________..
jetrtrack::~jetrtrack()
{
  std::cout << "jetrtrack::~jetrtrack() Calling dtor" << std::endl;
}
//____________________________________________________________________________..

int jetrtrack::Init(PHCompositeNode *topNode)
{
  //-------------------------------------------------------------
  //print the list of nodes available in memory
  //-------------------------------------------------------------
  topNode->print();
  return Fun4AllReturnCodes::EVENT_OK;
}

Fun4AllHistoManager *jetrtrack::get_HistoManager()  //This enables the handling of the output object and eventual writing of it to file
{
  Fun4AllServer *se = Fun4AllServer::instance();
  Fun4AllHistoManager *hm = se->getHistoManager("jetrtrack_HISTOS");

  if (not hm)
  {
    std:: cout
        << "jetrtrack::get_HistoManager - Making Fun4AllHistoManager jetrtrack_HISTOS"
        << std::endl;
    hm = new Fun4AllHistoManager("jetrtrack_HISTOS");
    se->registerHistoManager(hm);
  }
  assert(hm);
  return hm;
}

//____________________________________________________________________________..
int jetrtrack::InitRun(PHCompositeNode *topNode)
{
  std::cout << "jetrtrack::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;
  
  //-------------------------------
  //Create the output file
  //-------------------------------
  PHTFileServer::get().open(m_outfilename.c_str(), "RECREATE");
  // PHTFileServer::get().open("jettree.root", "RECREATE");


  Fun4AllHistoManager *hm = get_HistoManager();
  assert(hm);

  //------------------------------------------------------------------------------------------------
  // create the output tree where jets, truth particles, and tracks will 
  // be stored and register it to the histogram manager
  //------------------------------------------------------------------------------------------------

  TTree* tree=  new TTree("tree","tree"); 
  tree->Print();

  // Truth jet kinematics
  tree->Branch("tjet_pt",&m_tjet_pt);
  tree->Branch("tjet_phi",&m_tjet_phi);
  tree->Branch("tjet_eta",&m_tjet_eta);
  tree->Branch("tjet_jcpt",&m_tjet_jcpt);

  //Reconstructed jet kinematics
  tree->Branch("rjet_pt",&m_rjet_pt);
  tree->Branch("rjet_phi",&m_rjet_phi);
  tree->Branch("rjet_eta",&m_rjet_eta);

  //Reconstructed track kinematics
  tree->Branch("trk_pt",&m_trk_pt);
  tree->Branch("trk_eta",&m_trk_eta);
  tree->Branch("trk_phi",&m_trk_phi);
  tree->Branch("trk_quality",&m_trk_qual);
  tree->Branch("nmvtx_hits", &m_nmvtxhits);

  //global variables
  tree->Branch("cent", &m_centrality);
  tree->Branch("zvtx", &m_zvtx);

  //truth charged particle information
  tree->Branch("tp_pt",&m_tp_pt);
  tree->Branch("tp_px",&m_tp_px);
  tree->Branch("tp_py",&m_tp_py);
  tree->Branch("tp_pz",&m_tp_pz);
  tree->Branch("tp_phi",&m_tp_phi);
  tree->Branch("tp_eta",&m_tp_eta);
  tree->Branch("tp_pid",&m_tp_pid);

  //Jet constituent information for truth jets
  tree->Branch("jc_pt",&m_jc_pt);
  tree->Branch("jc_phi",&m_jc_phi);
  tree->Branch("jc_eta",&m_jc_eta);
  tree->Branch("jc_tjet_index",&m_jc_index);

  hm->registerHisto(tree);

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int jetrtrack::process_event(PHCompositeNode *topNode)
{
  double pi = TMath::Pi();

  //-----------------------------------------------------------------------------------------------------------------
  // load in the containers which hold the truth and reconstructed information
  //-----------------------------------------------------------------------------------------------------------------

  JetMap* tjets = findNode::getClass<JetMap>(topNode, "AntiKt_Truth_r04");
  if (!tjets)
  {
    std::cout
        << "Jetrtrack::process_event - Error can not find DST JetMap node "
        << std::endl;
    exit(-1);
  }


  JetMap* rjets = findNode::getClass<JetMap>(topNode, "AntiKt_Tower_r04_Sub1");
  if (!rjets)
  {
    std::cout
        << "Jetrtrack::process_event - Error can not find DST JetMap node "
        << std::endl;
    exit(-1);
  }

  SvtxTrackMap* trackmap = findNode::getClass<SvtxTrackMap>(topNode, "SvtxTrackMap");
  if (!trackmap)
  {
    std::cout
        << "Jetrtrack::process_event - Error can not find DST SvtxTrackMap node "
        << std::endl;
    exit(-1);
  }


  CentralityInfo* cent_node = findNode::getClass<CentralityInfo>(topNode, "CentralityInfo");
  if (!cent_node)
    {
      std::cout
        << "Jetrtrack::process_event - Error can not find centrality node "
        << std::endl;
      exit(-1);
    }

  m_centrality =  cent_node->get_centile(CentralityInfo::PROP::bimp);  //Centrality as defined by the impact parameter



  SvtxVertexMap* vertexmap = findNode::getClass<SvtxVertexMap>(topNode, "SvtxVertexMap");
 if (!vertexmap)
    {
      std::cout
        << "Jetrtrack::process_event - Error can not find vertex map node "
        << std::endl;
      exit(-1);
    }

  PHG4TruthInfoContainer *truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
  if (!truthinfo)
    {
      std::cout
        << "Jetrtrack::process_event - Error can not find G4TruthInfo map node "
        << std::endl;
      exit(-1);
    }



  //-------------------------------------------------------------
  //Call the histogram manager in order to 
  //pull the tree into the perview of the event 
  //-------------------------------------------------------------

  Fun4AllHistoManager *hm = get_HistoManager();
  assert(hm);



  //----------------------------------------------------
  // loop over vertex information and 
  // extract z-vertex
  //----------------------------------------------------

  if (vertexmap)
    {
      if (!vertexmap->empty())
	{
        SvtxVertex* vertex = (vertexmap->begin()->second);
        m_zvtx = vertex->get_z();
      }
    }



  //---------------------------------------------------
  // loop over all truth jets in the event
  //---------------------------------------------------
  int chargedparticlespids[] = {11,211,321,2212}; // corresponds to electrons, charged pions, charged kaons, and protons

  int jetnumber = 0;
  //---------------------------------
  // loop over all truth jets
  //---------------------------------
  for (JetMap::Iter iter = tjets->begin(); iter != tjets->end(); ++iter)
  {
    Jet* tjet = iter->second;
    assert(tjet);
    float tjet_phi = tjet->get_phi();
    float tjet_eta = tjet->get_eta();
    float tjet_pt = tjet->get_pt();
    if (tjet_pt < 5){continue;} //Filter out  low pT jets to reduce file size


    //---------------------------------------------------------
    //loop over truth jet constituent particles
    //extract the jet constitutent kinematics 
    // from charged particles
    //---------------------------------------------------------

    float sumjcpt = 0;
    for (Jet::ConstIter comp = tjet->begin_comp(); comp != tjet->end_comp(); ++comp)
      {
	PHG4Particle* truth = truthinfo->GetParticle((*comp).second); 
	bool reject_particle = true;
	for (int k = 0 ; k < 4;k++)
	   {
	     if (abs(truth->get_pid()) == chargedparticlespids[k])
	       {
		 reject_particle = false;
	       }
	   }
	if (reject_particle) {continue;}
	float m_truthpx = truth->get_px();
	float m_truthpy = truth->get_py();
	float m_truthpz = truth->get_pz();
	float m_truthenergy = truth->get_e(); 
	float m_truthpt = sqrt(m_truthpx * m_truthpx + m_truthpy * m_truthpy);
	float m_truthphi = atan2(m_truthpy , m_truthpx);
	float m_trutheta = atanh(m_truthpz / m_truthenergy); 
	m_jc_pt.push_back(m_truthpt);
	m_jc_phi.push_back(m_truthphi);
	m_jc_eta.push_back(m_trutheta);
	m_jc_index.push_back(jetnumber);
	sumjcpt += m_truthpt;
      }
    jetnumber++;



    float drmin = 0.3;
    float matched_pt = -999;
    float matched_eta = -999;
    float matched_phi = -999;
    //---------------------------------------------------------------------------------------------------
    //loop over reconstructed jets and match each truth jet to the closest 
    //reconstructed jet
    //----------------------------------------------------------------------------------------------------
    for (JetMap::Iter riter = rjets->begin(); riter != rjets->end(); ++riter)
      {
	Jet* rjet = riter->second;
	float rjet_phi = rjet->get_phi();
	float rjet_eta = rjet->get_eta();
	float rjet_pt = rjet->get_pt();
	
        float deta = fabs(rjet_eta - tjet_eta);
	float dphi = fabs(rjet_phi - tjet_phi);
	if (dphi > pi)
	  {
	    dphi = 2*pi-dphi;
	  }

	float dr = TMath::Sqrt(dphi*dphi + deta*deta);
	if (dr < drmin)//truth-reco jet matching
	  {
	    matched_pt = rjet_pt;
	    matched_eta = rjet_eta;
	    matched_phi = rjet_phi;
	    drmin = dr;
	  }
      }
    //-------------------------------------------------------------------------
    //append the truth and matched reconstructed jet
    // to the end of the corresponding vectors
    //-------------------------------------------------------------------------
    if (tjet_pt > 0)
      {	

	m_tjet_pt.push_back(tjet_pt);
	m_tjet_jcpt.push_back(sumjcpt);
	m_tjet_phi.push_back(tjet_phi);
	m_tjet_eta.push_back(tjet_eta);
	m_rjet_pt.push_back(matched_pt);
	m_rjet_phi.push_back(matched_phi);
	m_rjet_eta.push_back(matched_eta);
	m_dr.push_back(drmin);
      }
    }
    
  //-------------------------------------------------------------
  //loop over reconstructed tracks to extract 
  //reconstructed track information
  //-------------------------------------------------------------

 if (trackmap)
    {
      for (SvtxTrackMap::Iter iter = trackmap->begin();
           iter != trackmap->end();
           ++iter)
	{
	  SvtxTrack* track = iter->second;
	  float quality = track->get_quality();
	  auto silicon_seed = track->get_silicon_seed();
	  int nmvtxhits = 0;
	  if (silicon_seed)
	    {
	      nmvtxhits = silicon_seed->size_cluster_keys();
	    }
	  if (quality < 10) //select on some minimal track quality as reccomended by tracking group
	  {
	    if (track->get_pt() < 1) {continue;} //exclude  low pT tracks to reduce file size
	    m_trk_pt.push_back(track->get_pt());
	    m_trk_eta.push_back(track->get_eta());
	    m_trk_phi.push_back(track->get_phi());
	    m_trk_qual.push_back(quality);
	    m_nmvtxhits.push_back(nmvtxhits);
	  }
      }
    }
 
 
 //------------------------------------------------------------------------------------------------------
 // loop over primary truth particles to extract truth particle information
 //------------------------------------------------------------------------------------------------------
 if (truthinfo)
   {
     PHG4TruthInfoContainer::Range range = truthinfo->GetPrimaryParticleRange();
     /// Loop over the G4 truth (stable) particles
     for (PHG4TruthInfoContainer::ConstIterator iter = range.first;
	  iter != range.second;
	  ++iter)
       {
	 /// Get this truth particle
	 const PHG4Particle *truth = iter->second;
	 

	 /// Get this particles momentum, etc.
	 float m_truthpx = truth->get_px();
	 float m_truthpy = truth->get_py();
	 float m_truthpz = truth->get_pz();
	 float m_truthenergy = truth->get_e();
	 
	 float m_truthpt = sqrt(m_truthpx * m_truthpx + m_truthpy * m_truthpy);
	 float m_truthphi = atan2(m_truthpy , m_truthpx);
	 float m_trutheta = atanh(m_truthpz / m_truthenergy);
	 int  m_truthpid = truth->get_pid();

	 if (m_truthpt < 1){continue;}
	 //--------------------------------------------------------------------------------------------------
	 //Filter truth particles to select those which are charged and stablish
	 //--------------------------------------------------------------------------------------------------
	 bool reject_particle = true;
	 for (int k = 0 ; k < 4;k++)
	   {
	     if (abs(truth->get_pid()) == chargedparticlespids[k])
	       {
		 reject_particle = false;
	       }
	   }
	 if (reject_particle) {continue;}

	 m_tp_pt.push_back(m_truthpt);
	 m_tp_px.push_back(m_truthpx);
	 m_tp_py.push_back(m_truthpy);
	 m_tp_pz.push_back(m_truthpz);
	 m_tp_phi.push_back(m_truthphi);
	 m_tp_eta.push_back(m_trutheta);
	 m_tp_pid.push_back(m_truthpid);
       }
   }
  //----------------------------------------------------------
  //Record the vectors of jet kinematic info
  //to the ttree.
  //----------------------------------------------------------

  TTree *tree = dynamic_cast<TTree *>(hm->getHisto("tree"));
  assert(tree);
  tree->Fill();


  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int jetrtrack::ResetEvent(PHCompositeNode *topNode)
{
  //-----------------------------------------------------
  //Clear every vector inbetween events
  // this is CRITICAL!! Otherwise you will 
  // have runaway behaviors
  //------------------------------------------------------
  m_tjet_pt.clear();
  m_tjet_jcpt.clear();
  m_tjet_phi.clear();
  m_tjet_eta.clear();
  m_nmvtxhits.clear();

  m_jc_index.clear();
  m_tp_pt.clear();
  m_tp_px.clear();
  m_tp_py.clear();
  m_tp_pz.clear();
  m_tp_phi.clear();
  m_tp_eta.clear();
  m_tp_pid.clear();


  m_jc_pt.clear();
  m_jc_phi.clear();
  m_jc_eta.clear();

  m_rjet_pt.clear();
  m_rjet_phi.clear();
  m_rjet_eta.clear();
  m_dr.clear();

  m_trk_pt.clear();
  m_trk_phi.clear();
  m_trk_eta.clear();
  m_trk_qual.clear();

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int jetrtrack::EndRun(const int runnumber)
{
  std::cout << "jetrtrack::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int jetrtrack::End(PHCompositeNode *topNode)
{
  std::cout << "jetrtrack::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  //----------------------------------------------------------------------
  //Enter the created output file and write the ttree 
  //----------------------------------------------------------------------

  PHTFileServer::get().cd(m_outfilename.c_str());
  // PHTFileServer::get().cd("jettree.root");

  Fun4AllHistoManager *hm = get_HistoManager();
  assert(hm);

  for (unsigned int i = 0; i < hm->nHistos(); i++)
    hm->getHisto(i)->Write();


  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int jetrtrack::Reset(PHCompositeNode *topNode)
{
 std::cout << "jetrtrack::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void jetrtrack::Print(const std::string &what) const
{
  std::cout << "jetrtrack::Print(const std::string &what) const Printing info for " << what << std::endl;
}
