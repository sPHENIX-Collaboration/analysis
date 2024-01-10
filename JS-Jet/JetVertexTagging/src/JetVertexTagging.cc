//module for producing a TTree with jet information for doing jet validation studies
// for questions/bugs please contact Virginia Bailey vbailey13@gsu.edu

#include "JetVertexTagging.h"

#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/PHTFileServer.h>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include <jetbase/JetContainer.h>
#include <jetbase/JetMap.h>
#include <jetbase/Jet.h>

#include <centrality/CentralityInfo.h>

#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>

#include <particleflowreco/ParticleFlowElementv1.h>
#include <particleflowreco/ParticleFlowElementContainer.h>

//#include <bbc/BbcPmtContainer.h>
//#include <bbc/BbcPmtContainer.h>

#define HomogeneousField
#include <KFParticle.h>

#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>

//#include <trackbase_historic/SvtxVertex.h>
#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/TrackAnalysisUtils.h>

#include <trackbase/MvtxDefs.h>

/// HEPMC truth includes
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <HepMC/GenEvent.h>
#include <HepMC/GenVertex.h>
#pragma GCC diagnostic pop

#include <phhepmc/PHHepMCGenEvent.h>
#include <phhepmc/PHHepMCGenEventMap.h>

#include <TDatabasePDG.h>

#include <TTree.h>
#include <TH1.h>

#include <cmath>



//____________________________________________________________________________..
JetVertexTagging::JetVertexTagging(const std::string& outputfilename, int n_inputs, std::vector<std::string> TreeNameCollection):
 SubsysReco("JetVertexTagging")
 , m_inputs(n_inputs)
 , m_TreeNameCollection(TreeNameCollection)
 , m_recoJetName()
 , m_truthJetName()
 , m_outputFileName(outputfilename)
 , m_etaRange(-1, 1)
 , m_ptRange(5, 100)
 , m_doTruthJets(0)
 , m_T()
 , m_event()
 , m_reco_jet_n()
 , m_truth_jet_n()
 , m_centrality()
 , m_impactparam()
 , m_reco_jet_id()
 , m_reco_jet_nConstituents()
 , m_reco_jet_nChConstituents()
 , m_reco_jet_px()
 , m_reco_jet_py()
 , m_reco_jet_pz()
 , m_reco_jet_pt()
 , m_reco_jet_eta()
 , m_reco_jet_phi()
 , m_reco_jet_m()
 , m_reco_jet_e()
 , m_reco_constituents_pt()
 , m_reco_constituents_dxy()
 , m_reco_constituents_dxy_unc()
 , m_reco_constituents_Sdxy_old()
 , m_reco_constituents_Sdxy()
 , m_reco_constituents_pt_cut()
 , m_reco_constituents_dxy_cut()
 , m_reco_constituents_dxy_unc_cut()
 , m_reco_constituents_Sdxy_old_cut()
 , m_reco_constituents_Sdxy_cut()
 , m_truth_jet_id()
 , m_truth_jet_nConstituents()
 , m_truth_jet_nChConstituents()
 , m_truth_jet_px()
 , m_truth_jet_py()
 , m_truth_jet_pz()
 , m_truth_jet_pt()
 , m_truth_jet_eta()
 , m_truth_jet_phi()
 , m_truth_jet_m()
 , m_truth_jet_e()
 , m_truth_constituents_PDG_ID()
 //, m_truth_constituents_Sdxy_2017()
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

  if (m_inputs == 0 || m_inputs > 10){
    std::cout << "MyJetAnalysis::Init - Error number of inputs outside (0,10> " << std::endl;
    exit(-1);
  }

  // configure Tree
  for(int i = 0 ; i < m_inputs; i++){
    m_T[i] = new TTree(m_TreeNameCollection.at(i).data(), m_TreeNameCollection.at(i).data());
    m_T[i]->Branch("m_event", &m_event[i], "event/I");
    m_T[i]->Branch("cent", &m_centrality[i]);
    m_T[i]->Branch("b", &m_impactparam[i]);

    m_T[i]->Branch("m_reco_jet_n", &m_reco_jet_n[i], "m_reco_jet_n/I");
    m_T[i]->Branch("m_reco_jet_id", &m_reco_jet_id[i]);
    m_T[i]->Branch("m_reco_jet_nConstituents", &m_reco_jet_nConstituents[i]);
    m_T[i]->Branch("m_reco_jet_nChConstituents", &m_reco_jet_nChConstituents[i]);
    m_T[i]->Branch("m_reco_jet_px", &m_reco_jet_px[i]);
    m_T[i]->Branch("m_reco_jet_py", &m_reco_jet_py[i]);
    m_T[i]->Branch("m_reco_jet_pz", &m_reco_jet_pz[i]);
    m_T[i]->Branch("m_reco_jet_pt", &m_reco_jet_pt[i]);
    m_T[i]->Branch("m_reco_jet_eta", &m_reco_jet_eta[i]);
    m_T[i]->Branch("m_reco_jet_phi", &m_reco_jet_phi[i]);
    m_T[i]->Branch("m_reco_jet_m", &m_reco_jet_m[i]);
    m_T[i]->Branch("m_reco_jet_e", &m_reco_jet_e[i]);
    m_T[i]->Branch("m_reco_constituents_pt", &m_reco_constituents_pt[i]);
    m_T[i]->Branch("m_reco_constituents_dxy", &m_reco_constituents_dxy[i]);
    m_T[i]->Branch("m_reco_constituents_dxy_unc", &m_reco_constituents_dxy_unc[i]);
    m_T[i]->Branch("m_reco_constituents_Sdxy_old", &m_reco_constituents_Sdxy_old[i]);
    m_T[i]->Branch("m_reco_constituents_Sdxy", &m_reco_constituents_Sdxy[i]);
    m_T[i]->Branch("m_reco_constituents_pt_cut", &m_reco_constituents_pt_cut[i]);
    m_T[i]->Branch("m_reco_constituents_dxy_cut", &m_reco_constituents_dxy_cut[i]);
    m_T[i]->Branch("m_reco_constituents_dxy_unc_cut", &m_reco_constituents_dxy_unc_cut[i]);
    m_T[i]->Branch("m_reco_constituents_Sdxy_old_cut", &m_reco_constituents_Sdxy_old_cut[i]);
    m_T[i]->Branch("m_reco_constituents_Sdxy_cut", &m_reco_constituents_Sdxy_cut[i]);
    //m_T->Branch("m_reco_jet_dR", &m_reco_jet_dR);m_reco_constituents_d0

    if(m_doTruthJets){
      m_T[i]->Branch("m_truth_jet_n", &m_truth_jet_n[i], "m_truth_jet_n/I");
      m_T[i]->Branch("m_truth_jet_id", &m_truth_jet_id[i]);
      m_T[i]->Branch("m_truth_jet_nConstituents", &m_truth_jet_nConstituents[i]);
      m_T[i]->Branch("m_truth_jet_nChConstituents", &m_truth_jet_nChConstituents[i]);
      m_T[i]->Branch("m_truth_jet_px", &m_truth_jet_px[i]);
      m_T[i]->Branch("m_truth_jet_py", &m_truth_jet_py[i]);
      m_T[i]->Branch("m_truth_jet_pz", &m_truth_jet_pz[i]);
      m_T[i]->Branch("m_truth_jet_pt", &m_truth_jet_pt[i]);
      m_T[i]->Branch("m_truth_jet_eta", &m_truth_jet_eta[i]);
      m_T[i]->Branch("m_truth_jet_phi", &m_truth_jet_phi[i]);
      m_T[i]->Branch("m_truth_jet_m", &m_truth_jet_m[i]);
      m_T[i]->Branch("m_truth_jet_e", &m_truth_jet_e[i]);
      m_T[i]->Branch("m_truth_constituents_PDG_ID", &m_truth_constituents_PDG_ID[i]);
      //m_T[i]->Branch("m_truth_constituents_dxy", &m_truth_constituents_dxy[i]);
      //m_T[i]->Branch("m_truth_constituents_dxy_unc", &m_truth_constituents_dxy_unc[i]);
      //m_T[i]->Branch("m_truth_constituents_Sdxy", &m_truth_constituents_Sdxy[i]);
      //m_T[i]->Branch("m_truth_constituents_Sdxy_2017", &m_truth_constituents_Sdxy_2017[i]);
    //  m_T->Branch("m_truth_jet_dR", &m_truth_jet_dR);
    }
    TString tmp = "";
    m_chi2ndf[i] = new TH1D(tmp +"chi2ndf_" + m_TreeNameCollection.at(i).data(),tmp +"chi2ndf_" + m_TreeNameCollection.at(i).data(),1000,0,100);
    m_mvtxcl[i] = new TH1I(tmp +"mvtxcl_" + m_TreeNameCollection.at(i).data(),tmp +"mvtxcl_" + m_TreeNameCollection.at(i).data(),6,0,6);
    m_inttcl[i] = new TH1I(tmp +"inttcl_" + m_TreeNameCollection.at(i).data(),tmp +"inttcl_" + m_TreeNameCollection.at(i).data(),6,0,6);
    m_mtpccl[i]= new TH1I(tmp +"tpccl_" + m_TreeNameCollection.at(i).data(),tmp +"tpccl_" + m_TreeNameCollection.at(i).data(),100,0,100);
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
  
    //centrality
  CentralityInfo* cent_node = findNode::getClass<CentralityInfo>(topNode, "CentralityInfo");
  if (!cent_node){
    std::cout << "MyJetAnalysis::process_event - Error can not find centrality node " << std::endl;
    exit(-1);
  }

  // interface to reco jets
  if ((m_recoJetName.size() != m_truthJetName.size()) || m_recoJetName.empty() || m_truthJetName.empty()){
    std::cout << "MyJetAnalysis::process_event - Error in m_recoJetName size !=  m_truthJetName.size() or empty" << std::endl;
    exit(-1);
  }

  if (m_recoJetName.size() != static_cast<long unsigned int>(m_inputs)){
    std::cout << "MyJetAnalysis::process_event - Error number of AddInput() calls does not match with number of inputs specified in constructor" << std::endl;
    exit(-1);
  }



  GlobalVertexMap *vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
  //BbcPmtContainer *vertexmap = findNode::getClass<BbcPmtContainer>(topNode, "BbcPmtContainer");
  if (!vertexmap)
  {
    std::cout << "MyJetAnalysis::process_event - Fatal Error - GlobalVertexMap node is missing. Please turn on the do_global flag in the main macro in order to reconstruct the global vertex." << std::endl;
    assert(vertexmap);  // force quit

    exit(-1);
  }

  if (vertexmap->empty())
  {
    std::cout << "MyJetAnalysis::process_event - Fatal Error - GlobalVertexMap node is empty. Please turn on the do_global flag in the main macro in order to reconstruct the global vertex." << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
    //exit(-1);
  }

  /*GlobalVertex *vtx = vertexmap->begin()->second;
  if (vtx == nullptr)
  {
    std::cout << "MyJetAnalysis::process_event - Fatal Error - Gvtx null." << std::endl;
    exit(-1);
  }

  Acts::Vector3 vtxActs(vtx->get_x(), vtx->get_y(), vtx->get_z());*/

 /* SvtxVertex *vtx2;
  vtx2->set_x(vtx->get_x());
  vtx2->set_y(vtx->get_y());
  vtx2->set_z(vtx->get_z());*/

  PHG4TruthInfoContainer *truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
  if (!truthinfo)
  {
    std::cout << PHWHERE << " ERROR: Can't find G4TruthInfo" << std::endl;
     exit(-1);
  }

  //std::cout<<"event: "<<m_event<<" vtx x: "<< vtx->get_x()<<" vtx y: "<< vtx->get_y()<<" vtx z: "<< vtx->get_z()<<std::endl;


  for(long unsigned int input = 0; input < m_recoJetName.size(); input++){
++m_event[input];



  //get the event centrality/impact parameter from HIJING
  m_centrality[input] =  cent_node->get_centile(CentralityInfo::PROP::bimp);
  m_impactparam[input] =  cent_node->get_quantity(CentralityInfo::PROP::bimp);

  // interface to reco jets
  JetContainer* jets = findNode::getClass<JetContainer>(topNode, m_recoJetName.at(input));
  if (!jets)
    {
      std::cout	<< "MyJetAnalysis::process_event - Error can not find DST Reco JetContainer node " << m_recoJetName.at(input) << std::endl;
      exit(-1);
    }

   /* JetMap* jets = findNode::getClass<JetMap>(topNode, m_recoJetName.at(input));
    if (!jets){
      std::cout	<< "MyJetAnalysis::process_event - Error can not find DST Reco JetMap node " << m_recoJetName.at(input) << std::endl;
      exit(-1);
    }*/

    //interface to truth jets
    JetMap* jetsMC = findNode::getClass<JetMap>(topNode, m_truthJetName.at(input));
    if (!jetsMC && m_doTruthJets){
      std::cout	<< "MyJetAnalysis::process_event - Error can not find DST Truth JetMap node "	<< m_truthJetName.at(input) << std::endl;
      exit(-1);
    }

      HepMC::GenEvent *hepMCGenEvent = nullptr;

  if(m_doTruthJets){

    PHHepMCGenEventMap *hepmceventmap = findNode::getClass<PHHepMCGenEventMap>(topNode, "PHHepMCGenEventMap");

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

  hepMCGenEvent = hepmcevent->getEvent();

  if(!hepMCGenEvent) return Fun4AllReturnCodes::ABORTEVENT;
  }

    ParticleFlowElementContainer *pflowContainer = findNode::getClass<ParticleFlowElementContainer>(topNode, "ParticleFlowElements");

  if(!pflowContainer)
  {
    std::cout << PHWHERE
         << "ParticleFlowElements node is missing, can't collect particles"
         << std::endl;
    return -1;
  }

  ParticleFlowElementContainer::ConstRange begin_end = pflowContainer->getParticleFlowElements();
  ParticleFlowElementContainer::ConstIterator rtiter;
  for (rtiter = begin_end.first; rtiter != begin_end.second; ++rtiter)
  {
    ParticleFlowElement *pflow = rtiter->second;

    if (!pflow)
    {
      continue;
    }

   // std::cout<< pflow->get_id()<<" x: "<< pflow->get_px()<<" y: "<< pflow->get_py()<<" z: "<< pflow->get_pz()<<std::endl;

  }

  //std::cout<<"NEW EVENT"<<std::endl;

 
  //get reco jets
    m_reco_jet_n[input] = 0;
    //for (JetMap::Iter iter = jets->begin(); iter != jets->end(); ++iter){
      for (Jet* jet : *jets){
      //Jet* jet = iter->second;
      
      if(jet->get_pt() < 5.0) continue; // to remove noise jets  input
      //bool eta_cut = (jet->get_eta() >= m_etaRange.first) and (jet->get_eta() <= m_etaRange.second);
      bool eta_cut = (std::abs(jet->get_eta()) <= 1.1-0.4);
      if (not eta_cut) continue;

      //std::cout<<"jet "<<std::endl;
      std::vector<float> jetdxy;
      std::vector<float> jetdxy_unc;
      std::vector<float> jetSdxy_old;
      std::vector<float> jetSdxy;
      std::vector<float> jetdxy_cut;
      std::vector<float> jetdxy_unc_cut;
      std::vector<float> jetSdxy_old_cut;
      std::vector<float> jetSdxy_cut;

      std::vector<float> jetpt;
      std::vector<float> jetpt_cut;
      int nChtracks = 0;
      
   
      //std::cout<<"NEW jet"<<std::endl;
      //for(auto citer = jet->begin_comp(); citer != jet->end_comp(); ++citer){
      for (const auto& comp : jet->get_comp_vec()){
	      //HepMC::GenParticle* constituent = hepMCGenEvent->barcode_to_particle(citer->second);
        //Jet::SRC source = citer->first;
        //std::cout<<source<<" "<<citer->second<<std::endl;
       // ParticleFlowElement *pflow = pflowContainer->getParticleFlowElement(comp->second);
        ParticleFlowElement *pflow = pflowContainer->getParticleFlowElement(comp.second);
        //std::cout<<"id: "<<citer->second<<" px: "<< pflow->get_px()<<" py: "<< pflow->get_py()<<" pz: "<< pflow->get_pz()<<std::endl;
        //std::cout<<"d0 line in 2D "<<vtx->get_x()*std::sin(pflow->get_phi()) - vtx->get_y()*std::cos(pflow->get_phi())<<std::endl;
        //jetd0.push_back(vtx->get_x()*std::sin(pflow->get_phi()) - vtx->get_y()*std::cos(pflow->get_phi()));
        /*if(pflow){
          std::cout<<"pflow exists"<<std::endl;
          pflow->identify();
        }else{
          std::cout<<"pflow does not exists"<<std::endl;
        }

        if(pflow->get_track()){
          std::cout<<"pflow track exists"<<std::endl;
        }else{
          std::cout<<"pflow track does not exists"<<std::endl;

          namespace TrackAnalysisUtils
{
  /// Returns DCA as .first and uncertainty on DCA as .second
  using DCA = std::pair<float, float>;
  using DCAPair = std::pair<DCA, DCA>;

  DCAPair get_dca(SvtxTrack* track, Acts::Vector3& vertex);

};
        }*/

  

 if(pflow->get_track()){
  
  //GlobalVertex *vtx = vertexmap->begin()->second;

  int id = pflow->get_track()->get_vertex_id();
  GlobalVertex *vtx = vertexmap->get(id);
  if (vtx == nullptr)
  {
    std::cout << "MyJetAnalysis::process_event - Fatal Error - Gvtx null." << std::endl;
    continue;
  }
  nChtracks++;

  Acts::Vector3 vtxActs(vtx->get_x(), vtx->get_y(), vtx->get_z());


        float val, err;
        std::pair<double,double> DCAxy;
        DCAxy = calcIP(pflow->get_track(),vtx,val,err);
        double dot = DCAxy.first * jet->get_px() + DCAxy.second * jet->get_py();
        double sign = int(dot/std::abs(dot));

        std::pair<std::pair<float, float>, std::pair<float, float>> DCApair;
        DCApair = TrackAnalysisUtils::get_dca(pflow->get_track(),vtxActs);
        double dot2 = (pflow->get_track()->get_x() - vtx->get_x()) * jet->get_px() + (pflow->get_track()->get_y() - vtx->get_y()) * jet->get_py();
        double sign2 = int(dot2/std::abs(dot2));

       jetpt.push_back(pflow->get_track()->get_pt());
       jetdxy.push_back(val);
       jetdxy_unc.push_back(err);
       jetSdxy_old.push_back(sign*std::abs(val/err));
       jetSdxy.push_back(sign2*std::abs(val/err));

      //std::cout<<pflow->get_track()->get_chisq()/pflow->get_track()->get_ndf()<<std::endl;

            
      int m_nmaps = 0;
      int m_nintt = 0;
      int m_ntpc = 0;
      int m_nmms = 0;

      for (const auto& ckey : get_cluster_keys(pflow->get_track())){
        switch (TrkrDefs::getTrkrId(ckey))
          {
          case TrkrDefs::mvtxId:
            //std::cout<<"mvtx layer: "<<TrkrDefs::getLayer(ckey)<<std::endl;
            //std::cout<<"mvtx stave: "<<MvtxDefs::getStaveId(ckey)<<std::endl;
            m_nmaps++;
            break;
          case TrkrDefs::inttId:
            m_nintt++;
            break;
          case TrkrDefs::tpcId:
            m_ntpc++;
            break;
          case TrkrDefs::micromegasId:
            m_nmms++;
            break;
          }
      }

     m_chi2ndf[input]->Fill(pflow->get_track()->get_chisq()/pflow->get_track()->get_ndf());
     m_mvtxcl[input]->Fill(m_nmaps);
     m_inttcl[input]->Fill(m_nintt);
     m_mtpccl[input]->Fill(m_ntpc);

     if((pflow->get_track()->get_chisq()/pflow->get_track()->get_ndf()) <= 3.0 && m_nmaps >= 3 && m_nintt >= 2 && m_ntpc >= 30){
      jetpt_cut.push_back(pflow->get_track()->get_pt());
      jetdxy_cut.push_back(val);
      jetdxy_unc_cut.push_back(err);
      jetSdxy_old_cut.push_back(sign*std::abs(val/err));
      jetSdxy_cut.push_back(sign2*std::abs(val/err));
     }

      //std::cout<<pflow->get_track()->get_id()<<" "<<m_nmaps<<" "<<m_nintt<<" "<<m_ntpc<<" "<<m_nmms<<std::setprecision(2)<<" chi2/ndof: "<<pflow->get_track()->get_chisq()/pflow->get_track()->get_ndf()<<" Sdxy "<<std::abs(val/err)<<std::endl;


      /* if((pflow->get_track()->get_chisq()/pflow->get_track()->get_ndf())){

       }*/

 }

	   // m_truthjet_const_px.push_back(constituent->momentum().px());
	    //m_truthjet_const_py.push_back(constituent->momentum().py());
	    //m_truthjet_const_pz.push_back(constituent->momentum().pz());
	    //m_truthjet_const_e.push_back(constituent->momentum().e());

      }

      m_reco_jet_id[input].push_back(jet->get_id());
      m_reco_jet_nConstituents[input].push_back(jet->size_comp());
      m_reco_jet_nChConstituents[input].push_back(nChtracks);
      m_reco_jet_px[input].push_back(jet->get_px());
      m_reco_jet_py[input].push_back(jet->get_py());
      m_reco_jet_pz[input].push_back(jet->get_pz());
      m_reco_jet_pt[input].push_back(jet->get_pt());
      m_reco_jet_eta[input].push_back(jet->get_eta());
      m_reco_jet_phi[input].push_back(jet->get_phi());
      m_reco_jet_m[input].push_back(jet->get_mass());
      m_reco_jet_e[input].push_back(jet->get_e());
      m_reco_constituents_pt[input].push_back(jetpt);
      m_reco_constituents_dxy[input].push_back(jetdxy);
      m_reco_constituents_dxy_unc[input].push_back(jetdxy_unc);
      m_reco_constituents_Sdxy_old[input].push_back(jetSdxy_old);
      m_reco_constituents_Sdxy[input].push_back(jetSdxy);
      m_reco_constituents_pt_cut[input].push_back(jetpt_cut);
      m_reco_constituents_dxy_cut[input].push_back(jetdxy_cut);
      m_reco_constituents_dxy_unc_cut[input].push_back(jetdxy_unc_cut);
      m_reco_constituents_Sdxy_old_cut[input].push_back(jetSdxy_old_cut);
      m_reco_constituents_Sdxy_cut[input].push_back(jetSdxy_cut);
      m_reco_jet_n[input]++;
    }
    //get truth jets
    if(m_doTruthJets){
      m_truth_jet_n[input] = 0;
      
      for (JetMap::Iter iter = jetsMC->begin(); iter != jetsMC->end(); ++iter){
        
      Jet* truthjet = iter->second;
  
      //bool eta_cut = (truthjet->get_eta() >= m_etaRange.first) and (truthjet->get_eta() <= m_etaRange.second);
      bool eta_cut = (std::abs(truthjet->get_eta()) <= 1.1-0.4);
      //bool pt_cut = (truthjet->get_pt() >= m_ptRange.first) and (truthjet->get_pt() <= m_ptRange.second);
      //if ((not eta_cut) or (not pt_cut)) continue;
      //bool pt_cut = (truthjet->get_pt() >= m_ptRange.first) and (truthjet->get_pt() <= m_ptRange.second);
      if (not eta_cut) continue;
      if(truthjet->get_pt() < 10.0) continue;

  //std::cout<<"new JET"<<std::endl;

      m_truth_jet_id[input].push_back(truthjet->get_id());
      m_truth_jet_nConstituents[input].push_back(truthjet->size_comp());
      m_truth_jet_px[input].push_back(truthjet->get_px());
      m_truth_jet_py[input].push_back(truthjet->get_py());
      m_truth_jet_pz[input].push_back(truthjet->get_pz());
      m_truth_jet_pt[input].push_back(truthjet->get_pt());
      m_truth_jet_eta[input].push_back(truthjet->get_eta());
      m_truth_jet_phi[input].push_back(truthjet->get_phi());
      m_truth_jet_m[input].push_back(truthjet->get_mass());
      m_truth_jet_e[input].push_back(truthjet->get_e());
      m_truth_jet_n[input]++;

      //std::vector<std::pair<HepMC::GenVertex*,int>> pdgid;
      std::vector<int> pdgid;
     /* for(auto citer = truthjet->begin_comp(); citer != truthjet->end_comp(); ++citer){
	      HepMC::GenParticle* constituent = hepMCGenEvent->barcode_to_particle(citer->second);
    	  TParticlePDG* pdg_p = TDatabasePDG::Instance()->GetParticle(constituent->pdg_id());
        if (pdg_p){
          //std::cout<<TString(pdg_p->ParticleClass())<<" pdgcode: "<<pdg_p->PdgCode()<<" S "<<pdg_p->Strangeness()<<" C "<<pdg_p->Charm()<<" B "<<pdg_p->Beauty()<<std::endl;
          pdgid.push_back(pdg_p->PdgCode());
          //if (TString(pdg_p->ParticleClass()).BeginsWith("B-"))
        }
	    }*/

      //PHG4Particle
   //   std::vector<int> vertexid;

     // std::cout<<"NEW JET: ";
      //int idv = 0;
//int constit = 0;
 /* HepMC::GenEvent *theEvent = m_genevt->getEvent();
  HepMC::GenParticle *prevParticle = nullptr;

  int forbiddenPDGIDs[] = {21, 22};  //Stop tracing history when we reach quarks, gluons and photons

  for (HepMC::GenEvent::particle_const_iterator p = theEvent->particles_begin(); p != theEvent->particles_end(); ++p)
  {
    if (((*p)->barcode() == g4particle->get_barcode()))
    {
      prevParticle = *p;
      while (!prevParticle->is_beam())
      {
        bool breakOut = false;
        bool taggedHF = false;
        for (HepMC::GenVertex::particle_iterator mother = prevParticle->production_vertex()->particles_begin(HepMC::parents);
             mother != prevParticle->production_vertex()->particles_end(HepMC::parents); ++mother)
        {
          if (std::find(std::begin(forbiddenPDGIDs), std::end(forbiddenPDGIDs),
                        abs((*mother)->pdg_id())) != std::end(forbiddenPDGIDs))
          {
            breakOut = true;
            break;
          }

          fillHepMCBranch((*mother), daughter_id);
          prevParticle = *mother;
        }
        if (breakOut) break;
      }
    }
  }*/

  TString taggedPDGIDs[] = {"B-Meson","CharmedMeson","CharmedBaryon", "B-Baryon"};
  int forbiddenPDGIDs[] = {1,2,3,4,5,6,7,8,21, 22}; 
  int nChTrackstruth = 0;

   for(auto citer = truthjet->begin_comp(); citer != truthjet->end_comp(); ++citer){
    //std::cout<<"new track"<<std::endl;
        PHG4Particle *g4part = truthinfo->GetPrimaryParticle(citer->second);
	      HepMC::GenParticle* constituent = hepMCGenEvent->barcode_to_particle(g4part->get_barcode());
        //std::cout<<(TDatabasePDG::Instance()->GetParticle((constituent)->pdg_id()))->Charge()<<std::endl;
        if(std::abs((TDatabasePDG::Instance()->GetParticle((constituent)->pdg_id()))->Charge()) > 10e-4) nChTrackstruth++;

        if (std::find(std::begin(taggedPDGIDs), std::end(taggedPDGIDs), TString((TDatabasePDG::Instance()->GetParticle((constituent)->pdg_id()))->ParticleClass())) != std::end(taggedPDGIDs)){
            //pdgid.push_back(std::pair(nullptr,(constituent)->pdg_id()));
            pdgid.push_back((constituent)->pdg_id());
            //std::cout<<"constituent is HF continuing "<<(constituent)->pdg_id()<<std::endl;
            continue;
        }

    	  while (!constituent->is_beam()){
          //std::cout<<"in while"<<std::endl;
          bool breakOut = false;
          bool taggedHF = false;
          for (HepMC::GenVertex::particle_iterator mother = constituent->production_vertex()->particles_begin(HepMC::parents); mother != constituent->production_vertex()->particles_end(HepMC::parents); ++mother){
            //std::cout<<"in for"<<std::endl;
            //found HF in parent tree
            if (std::find(std::begin(taggedPDGIDs), std::end(taggedPDGIDs), TString((TDatabasePDG::Instance()->GetParticle((*mother)->pdg_id()))->ParticleClass())) != std::end(taggedPDGIDs)){
              taggedHF = true;
              //std::cout<<(*mother)->pdg_id()<<std::endl;
              pdgid.push_back((*mother)->pdg_id());
              //std::cout<<"HF found breaking "<<(*mother)->pdg_id()<<std::endl;
              break;
            }
            //reached partonic level
            if (std::find(std::begin(forbiddenPDGIDs), std::end(forbiddenPDGIDs), abs((*mother)->pdg_id())) != std::end(forbiddenPDGIDs)){
              breakOut = true;
              //std::cout<<"quark found breaking"<<std::endl;
              break;
            }

            //pdgid.push_back(std::pair((*mother)->production_vertex(),(*mother)->pdg_id()));
            //std::cout<<"const = mother"<<std::endl;
            constituent = *mother;
          }
          //if (breakOut || taggedHF) std::cout<<"HF or quekr break"<<std::endl;
          if (breakOut || taggedHF) break;
      }

    }



     /* for(auto citer = truthjet->begin_comp(); citer != truthjet->end_comp(); ++citer){
        PHG4Particle *g4part = truthinfo->GetPrimaryParticle(citer->second);
	      HepMC::GenParticle* constituent = hepMCGenEvent->barcode_to_particle(g4part->get_barcode());
    	  TParticlePDG* pdg_p = TDatabasePDG::Instance()->GetParticle(constituent->pdg_id());
        if (pdg_p){
          //std::cout<<TString(pdg_p->ParticleClass())<<" pdgcode: "<<pdg_p->PdgCode()<<" S "<<pdg_p->Strangeness()<<" C "<<pdg_p->Charm()<<" B "<<pdg_p->Beauty()<<std::endl;
          pdgid.push_back(pdg_p->PdgCode());
          TString classPart = TString(pdg_p->ParticleClass());
         if(classPart != "Meson" && classPart != "GaugeBoson" &&classPart != "Baryon"   &&classPart != "Lepton") std::cout<<classPart<<std::endl;
          //if (TString(pdg_p->ParticleClass()).BeginsWith("B-"))
        }*/
/*
        GaugeBoson
B-Meson
NEW EVENT
NEW EVENT
CharmedMeson
CharmedMeson
NEW EVENT
NEW EVENT
NEW EVENT
NEW EVENT
NEW EVENT
NEW EVENT
NEW EVENT
NEW EVENT
NEW EVENT
NEW EVENT
NEW EVENT
CharmedMeson
CharmedMeson
NEW EVENT
NEW EVENT
CharmedBaryon

*/



      //  HepMC::GenVertex *TagVertex = constituent->production_vertex();
      //  int id = TagVertex->barcode();
        //TagVertex->parent_event()->print();
        //if ( std::find(vertexid.begin(), vertexid.end(), id) != vertexid.end() ){
       //   continue;
      //  }
       // else{
          //TagVertex->print();
         // std::cout<<"Vertex No: "<<idv<<std::endl;
         // idv++;
/*         std::cout<<"constituent "<<constit<<" constituent type: ";
         std::map<int, std::string>::iterator it3 = mapPDG.find(pdg_p->PdgCode());
        std::cout<<it3->second<<" ";
         
        std::cout<<" Vertex ID "<<id<<" Particles vertex in: ";
          for (HepMC::GenVertex::particles_in_const_iterator iter = TagVertex->particles_in_const_begin(); iter != TagVertex->particles_in_const_end(); ++iter){
              std::map<int, std::string>::iterator it2 = mapPDG.find((*iter)->pdg_id());
              std::cout<<it2->second<<" "<<(*iter)->barcode()<<" ";
          }
          std::cout<<" Particles vertex out: ";
           for (HepMC::GenVertex::particles_out_const_iterator iter = TagVertex->particles_out_const_begin(); iter != TagVertex->particles_out_const_end(); ++iter){
              std::map<int, std::string>::iterator it2 = mapPDG.find((*iter)->pdg_id());
              std::cout<<it2->second<<" "<<(*iter)->barcode()<<" ";
          }
          std::cout<<std::endl;
          constit++;
          std::cout<<" getting deeper particles in: ";
          for (HepMC::GenVertex::particles_in_const_iterator iter = TagVertex->particles_in_const_begin(); iter != TagVertex->particles_in_const_end(); ++iter){
              if((*iter)->production_vertex()){
              std::cout<<" vtx 1 bar "<<(*iter)->production_vertex()->barcode()<<" ";
              for (HepMC::GenVertex::particles_in_const_iterator iter2 = (*iter)->production_vertex()->particles_in_const_begin(); iter2 != (*iter)->production_vertex()->particles_in_const_end(); ++iter2){
                 std::map<int, std::string>::iterator it2 = mapPDG.find((*iter2)->pdg_id());
              std::cout<<it2->second<<" "<<(*iter2)->barcode()<<" ";
                if((*iter2)->production_vertex()){
              for (HepMC::GenVertex::particles_in_const_iterator iter3 = (*iter2)->production_vertex()->particles_in_const_begin(); iter3 != (*iter2)->production_vertex()->particles_in_const_end(); ++iter3){
std::cout<<" getting even deeper particles in: vtx bar "<<(*iter2)->production_vertex()->barcode()<<" ";
              std::map<int, std::string>::iterator it3 = mapPDG.find((*iter3)->pdg_id());
              std::cout<<it3->second<<" "<<(*iter3)->barcode()<<" ";
              }
              }
              }
          }
          }
*/



         // vertexid.push_back(id);
        //}
        
     /* for (HepMC::genvertex::particle_iterator it = TagVertex->particles_begin(HepMC::ancestors); it != TagVertex->particles_end(HepMC::ancestors); ++it)
      {
          print
  }*/
	   // }
     // vertexid.clear();

m_truth_jet_nChConstituents[input].push_back(nChTrackstruth);
 m_truth_constituents_PDG_ID[input].push_back(pdgid);



 

      }
    }
    m_T[input]->Fill();
    //fill the tree 
    //std::cout<<"same "<<same<<" diff "<<diff<<std::endl;
  }

  

  
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetVertexTagging::ResetEvent(PHCompositeNode *topNode)
{
  //std::cout << "JetVertexTagging::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << std::endl;
  for(int i = 0 ; i< m_inputs; i++){
  m_reco_jet_id[i].clear();
  m_reco_jet_nConstituents[i].clear();
  m_reco_jet_nChConstituents[i].clear();
  m_reco_jet_px[i].clear();
  m_reco_jet_py[i].clear();
  m_reco_jet_pz[i].clear();
  m_reco_jet_pt[i].clear();
  m_reco_jet_eta[i].clear();
  m_reco_jet_phi[i].clear();
  m_reco_jet_m[i].clear();
  m_reco_jet_e[i].clear();
  m_reco_constituents_pt[i].clear();
  m_reco_constituents_dxy[i].clear();
  m_reco_constituents_dxy_unc[i].clear();
  m_reco_constituents_Sdxy_old[i].clear();
  m_reco_constituents_Sdxy[i].clear();
  m_reco_constituents_pt_cut[i].clear();
  m_reco_constituents_dxy_cut[i].clear();
  m_reco_constituents_dxy_unc_cut[i].clear();
  m_reco_constituents_Sdxy_old_cut[i].clear();
  m_reco_constituents_Sdxy_cut[i].clear();


  m_truth_jet_id[i].clear();
  m_truth_jet_nConstituents[i].clear();
  m_truth_jet_nChConstituents[i].clear();
  m_truth_jet_px[i].clear();
  m_truth_jet_py[i].clear();
  m_truth_jet_pz[i].clear();
  m_truth_jet_pt[i].clear();
  m_truth_jet_eta[i].clear();
  m_truth_jet_phi[i].clear();
  m_truth_jet_m[i].clear();
  m_truth_jet_e[i].clear();
  m_truth_constituents_PDG_ID[i].clear();
  }

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

  for(int i = 0 ; i < m_inputs; i++){
    m_T[i]->Write();
    m_chi2ndf[i]->Write();
    m_mvtxcl[i]->Write();
    m_inttcl[i]->Write();
    m_mtpccl[i]->Write();
  }
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

std::pair<double,double> JetVertexTagging::calcIP(SvtxTrack *m_dst_track, GlobalVertex *m_dst_vertex,float &val, float &err)
{
 
  KFParticle::SetField(1.4e1);

  KFParticle kfp_particle;

  float f_trackParameters[6] = {m_dst_track->get_x(),
                                m_dst_track->get_y(),
                                m_dst_track->get_z(),
                                m_dst_track->get_px(),
                                m_dst_track->get_py(),
                                m_dst_track->get_pz()};

  float f_trackCovariance[21];
  unsigned int iterate = 0;
  for (unsigned int i = 0; i < 6; ++i)
    for (unsigned int j = 0; j <= i; ++j)
    {
      f_trackCovariance[iterate] = m_dst_track->get_error(i, j);
      ++iterate;
    }

  kfp_particle.Create(f_trackParameters, f_trackCovariance, (Int_t) m_dst_track->get_charge(), -1);
  kfp_particle.NDF() = m_dst_track->get_ndf();
  kfp_particle.Chi2() = m_dst_track->get_chisq();
  kfp_particle.SetId(m_dst_track->get_id());

  float f_vertexParameters[6] = {m_dst_vertex->get_x(),
                                 m_dst_vertex->get_y(),
                                 m_dst_vertex->get_z(), 0, 0, 0};

  float f_vertexCovariance[21];
  unsigned int iterate2 = 0;
  for (unsigned int i = 0; i < 3; ++i)
    for (unsigned int j = 0; j <= i; ++j)
    {
      f_vertexCovariance[iterate2] = m_dst_vertex->get_error(i, j);
      ++iterate2;
    }

  KFParticle kfp_vertex;
  kfp_vertex.Create(f_vertexParameters, f_vertexCovariance, 0, -1);
  kfp_vertex.NDF() = m_dst_vertex->get_ndof();
  kfp_vertex.Chi2() = m_dst_vertex->get_chisq();
  kfp_particle.GetDistanceFromVertexXY(kfp_vertex,val,err);

  float mP[8];
  float mC[36];
  float dsdr[6] = {0.f};
  float xyz[3];
  xyz[0] = kfp_vertex.GetX();
  xyz[1] = kfp_vertex.GetY();
  xyz[2] = kfp_vertex.GetZ();

  const float dS = kfp_particle.GetDStoPoint(xyz, dsdr);
  kfp_particle.Transport( dS, dsdr, mP, mC );  

  float dx = mP[0] - kfp_vertex.GetX();
  float dy = mP[1] - kfp_vertex.GetY();
  float px = mP[3];
  float py = mP[4];
  float pt = sqrt(px*px + py*py);
  float ex=0, ey=0;
  std::pair<double,double> DCAvector;

  if( pt<1.e-4 ){
    pt = 1.;
    val = 1.e4;
    DCAvector.first = val;
    DCAvector.second = val;
  } else{
    ex = px/pt;
    ey = py/pt;
    val = dy*ex - dx*ey;
    DCAvector.first = ex*val;
    DCAvector.second = ey*val;
  }

  

  //std::cout<<kfp_particle.GetDistanceFromVertexXY(kfp_vertex)<<std::endl;

  //std::cout<<kfp_particle.GetDistanceFromVertexXY(kfp_vertex)<<" "<<kfp_particle.GetDeviationFromVertexXY(kfp_vertex)<<std::endl;

  return DCAvector;//kfp_particle.GetDistanceFromVertexXY(kfp_vertex)/kfp_particle.GetDeviationFromVertexXY(kfp_vertex);
}


std::pair<double,double> JetVertexTagging::calcIP2(SvtxTrack *m_dst_track, GlobalVertex *m_dst_vertex){
  KFParticle::SetField(1.4e1);
  KFParticle kfp_particle;

  float f_trackParameters[6] = {m_dst_track->get_x(),
                                m_dst_track->get_y(),
                                m_dst_track->get_z(),
                                m_dst_track->get_px(),
                                m_dst_track->get_py(),
                                m_dst_track->get_pz()};

  float f_trackCovariance[21];
  unsigned int iterate = 0;
  for (unsigned int i = 0; i < 6; ++i)
    for (unsigned int j = 0; j <= i; ++j)
    {
      f_trackCovariance[iterate] = m_dst_track->get_error(i, j);
      ++iterate;
    }

  kfp_particle.Create(f_trackParameters, f_trackCovariance, (Int_t) m_dst_track->get_charge(), -1);
  kfp_particle.NDF() = m_dst_track->get_ndf();
  kfp_particle.Chi2() = m_dst_track->get_chisq();
  kfp_particle.SetId(m_dst_track->get_id());

  float f_vertexParameters[3] = {m_dst_vertex->get_x(),
                                 m_dst_vertex->get_y(),
                                 m_dst_vertex->get_z()};

  kfp_particle.TransportToPoint(f_vertexParameters);
  std::pair<double,double> DCAvector;
  //std::pair<double,double> DCAvector_unc;
  DCAvector.first = kfp_particle.GetX() - f_vertexParameters[0];
  DCAvector.second = kfp_particle.GetY() - f_vertexParameters[1];

  //kfp_particle.GetCovariance  

  //std::cout<<kfp_particle.GetDistanceFromVertexXY(kfp_vertex)<<std::endl;

  //std::cout<<kfp_particle.GetDistanceFromVertexXY(kfp_vertex)<<" "<<kfp_particle.GetDeviationFromVertexXY(kfp_vertex)<<std::endl;

  return DCAvector;//kfp_particle.GetDistanceFromVertexXY(kfp_vertex)/kfp_particle.GetDeviationFromVertexXY(kfp_vertex);
}


std::vector<TrkrDefs::cluskey> JetVertexTagging::get_cluster_keys(SvtxTrack* track)
  {
    std::vector<TrkrDefs::cluskey> out;
    for (const auto& seed : {track->get_silicon_seed(), track->get_tpc_seed()})
    {
      if (seed)
      {
        std::copy(seed->begin_cluster_keys(), seed->end_cluster_keys(), std::back_inserter(out));
      }
    }

    return out;
  }

