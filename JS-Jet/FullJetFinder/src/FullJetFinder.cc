/**
 * @file FullJetFinder.cc
 *
 * @brief module for producing a TTree with full jets (tracks + calos) studies
 *        Based on JetValidation macro and HF group macros
 *
 * @ingroup FullJetFinder
 *
 * @author Jakub Kvapil
 * Contact: jakub.kvapil@cern.ch
 *
 */

#include "FullJetFinder.h"

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

#define HomogeneousField
#include <KFParticle.h>

#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>

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
FullJetFinder::FullJetFinder(const std::string& outputfilename, FullJetFinder::TYPE jet_type):
 SubsysReco("FullJetFinder")
 , m_recoJetName()
 , m_truthJetName()
 , m_outputFileName(outputfilename)
 , m_ptRangeReco(0, 100)
 , m_ptRangeTruth(0, 100)
 , m_doTruthJets(0)
 , m_T()
 , m_jet_type(jet_type)
{
  std::cout << "FullJetFinder::FullJetFinder(const std::string &name) Calling ctor" << std::endl;
}

//____________________________________________________________________________..
FullJetFinder::~FullJetFinder()
{
  std::cout << "FullJetFinder::~FullJetFinder() Calling dtor" << std::endl;
}

//_________________________________________________________
void FullJetFinder::Container::Reset()
{
  reco_jet_n = -1;
  truth_jet_n = -1;
  centrality = -1;
  impactparam = -1;
  recojets.clear();
  truthjets.clear();
  //primaryVertex.clear();
}

//____________________________________________________________________________..
int FullJetFinder::Init(PHCompositeNode *topNode)
{
  std::cout << "FullJetFinder::Init(PHCompositeNode *topNode) Initializing" << std::endl;
  PHTFileServer::get().open(m_outputFileName, "RECREATE");
  std::cout << "FullJetFinder::Init - Output to " << m_outputFileName << std::endl;

  if (m_inputs == 0 || m_inputs > 10){
    std::cout << "MyJetAnalysis::Init - Error number of inputs outside (0,10> " << std::endl;
    exit(-1);
  }

  // configure Tree
  for(int i = 0 ; i < m_inputs; i++){
    //m_T[i] = new TTree(m_TreeNameCollection.at(i).data(), m_TreeNameCollection.at(i).data());
    m_T[i] = new TTree("Data", "Data");
    m_container[i] = new Container;
    m_T[i]->Branch( "JetContainer", &m_container[i] );
  
    TString tmp = "";
    m_chi2ndf[i] = new TH1D(tmp +"chi2ndf_" + m_TreeNameCollection.at(i).data(),tmp +"chi2ndf_" + m_TreeNameCollection.at(i).data(),1000,0,100);
    m_mvtxcl[i] = new TH1I(tmp +"mvtxcl_" + m_TreeNameCollection.at(i).data(),tmp +"mvtxcl_" + m_TreeNameCollection.at(i).data(),6,0,6);
    m_inttcl[i] = new TH1I(tmp +"inttcl_" + m_TreeNameCollection.at(i).data(),tmp +"inttcl_" + m_TreeNameCollection.at(i).data(),6,0,6);
    m_mtpccl[i]= new TH1I(tmp +"tpccl_" + m_TreeNameCollection.at(i).data(),tmp +"tpccl_" + m_TreeNameCollection.at(i).data(),100,0,100);
  }

  m_stat = new TH1I("event_stat","event_stat",10,-0.5,9.5);
  m_stat->GetXaxis()->SetBinLabel(1,"n_events");
  m_stat->GetXaxis()->SetBinLabel(2,"GV_exists");
  m_stat->GetXaxis()->SetBinLabel(3,"GV_notempty");
  m_stat->GetXaxis()->SetBinLabel(4,"GV_SVTX_vtx");
  m_stat->GetXaxis()->SetBinLabel(5,"GV_in_10cm");



  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int FullJetFinder::InitRun(PHCompositeNode *topNode)
{
  std::cout << "FullJetFinder::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int FullJetFinder::process_event(PHCompositeNode *topNode)
{  
    //std::cout<<"NEW EVENT"<<std::endl;
  //std::cout<<"NEW Fun4All EVENT"<<std::endl<<std::endl;
  m_stat->Fill(0);
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
  if (!vertexmap){
    std::cout << "MyJetAnalysis::process_event - Fatal Error - GlobalVertexMap node is missing. Please turn on the do_global flag in the main macro in order to reconstruct the global vertex." << std::endl;
    assert(vertexmap);  // force quit
    exit(-1);
  }

  m_stat->Fill(1);

  if (vertexmap->empty()){
    std::cout << "MyJetAnalysis::process_event - Fatal Error - GlobalVertexMap node is empty. Please turn on the do_global flag in the main macro in order to reconstruct the global vertex." << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  m_stat->Fill(2);

  PHG4TruthInfoContainer *truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
  if (!truthinfo)
  {
    std::cout << PHWHERE << " ERROR: Can't find G4TruthInfo" << std::endl;
     exit(-1);
  }

  //loop over multiple JetReco->add_algo()
  for(long unsigned int input = 0; input < m_recoJetName.size(); input++){
    //reset per event container
    m_container[input]->Reset();
    
    //get the event centrality/impact parameter from HIJING
    m_container[input]->centrality =  cent_node->get_centile(CentralityInfo::PROP::bimp);
    m_container[input]->impactparam =  cent_node->get_quantity(CentralityInfo::PROP::bimp);

    

    // interface to reco jets
    JetContainer* jets = findNode::getClass<JetContainer>(topNode, m_recoJetName.at(input));
    if (!jets){
      std::cout	<< "MyJetAnalysis::process_event - Error can not find DST Reco JetContainer node " << m_recoJetName.at(input) << std::endl;
      exit(-1);
    }

    //interface to truth jets
    //JetMap* jetsMC = findNode::getClass<JetMap>(topNode, m_truthJetName.at(input));
    JetContainer* jetsMC = findNode::getClass<JetContainer>(topNode, m_truthJetName.at(input));
    if (!jetsMC && m_doTruthJets){
      std::cout	<< "MyJetAnalysis::process_event - Error can not find DST Truth JetContainer node "	<< m_truthJetName.at(input) << std::endl;
      exit(-1);
    }

    //get associated hepMCGenEvent
    HepMC::GenEvent *hepMCGenEvent = nullptr;
    if(m_doTruthJets){
      PHHepMCGenEventMap *hepmceventmap = findNode::getClass<PHHepMCGenEventMap>(topNode, "PHHepMCGenEventMap");

      /// If the node was not properly put on the tree, return
      if (!hepmceventmap){
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

    //get Particle Flow container
    ParticleFlowElementContainer *pflowContainer = findNode::getClass<ParticleFlowElementContainer>(topNode, "ParticleFlowElements");

    if(!pflowContainer && m_jet_type==TYPE::FULLJET){
      std::cout << PHWHERE
          << "ParticleFlowElements node is missing, can't collect particles"
          << std::endl;
      return -1;
    }
  

    SvtxTrackMap *trackmap = findNode::getClass<SvtxTrackMap>(topNode, "SvtxTrackMap");
  if (!trackmap)
  {
    std::cout << PHWHERE
          << "SvtxTrackMap node is missing, can't collect particles"
          << std::endl;
      return -1;
  }

  GlobalVertex *prim_vtx = nullptr;

    for(auto vertex : *vertexmap){
    //std::cout<<"map entry"<<std::endl;
    PrimaryVertex primary;
    std::vector<GlobalVertex::VTXTYPE> source;
    source.clear();
    primary.id = vertex.second->get_id();
    primary.x = vertex.second->get_x();
    primary.y = vertex.second->get_y();
    primary.z = vertex.second->get_z();
    primary.x_unc = sqrt(vertex.second->get_error(0,0));
    primary.y_unc = sqrt(vertex.second->get_error(1,1));
    primary.z_unc = sqrt(vertex.second->get_error(2,2));
    primary.chisq = vertex.second->get_chisq();
    primary.ndf = vertex.second->get_ndof();
    
     //std::cout<<std::endl<<"A "<<vertex.second->get_x()<<" "<<vertex.second->get_y()<<" "<<vertex.second->get_z()<<" "<<vertex.second->get_id()<<" "<<vertex.first<<std::endl;
     //for(auto vx :vertex.second->begin_vertexes()->second){
     //   std::cout<<"vx "<<vx->get_x()<<" "<<vx->get_y()<<" "<<vx->get_z()<<" "<<vx->get_chisq()<<vx->get_chisq()<<std::endl;
     //}

    for (auto iter = vertex.second->begin_vertexes();   iter != vertex.second->end_vertexes();   ++iter){
      source.push_back(iter->first);
      GlobalVertex::VertexVector vtx = iter->second;
      //std::cout<<"vertex source "<<iter->first<<std::endl;
      if(iter->first == 400){
        
        prim_vtx = vertex.second;
      } 

      
      //for(auto vx :vtx){
      //  std::cout<<"vx "<<vx->get_x()<<" "<<vx->get_y()<<" "<<vx->get_z()<<" "<<vx->get_chisq()<<std::endl;

        
      //}
    }
  
  if(std::find(source.begin(), source.end(), GlobalVertex::VTXTYPE::SVTX) == source.end()) return Fun4AllReturnCodes::ABORTEVENT;
  m_stat->Fill(3);

    //if((std::find(source.begin(), source.end(), GlobalVertex::VTXTYPE::SVTX) != source.end()) && (std::find(source.begin(), source.end(), GlobalVertex::VTXTYPE::MBD) != source.end())) primary.vtxtype = GlobalVertex::VTXTYPE::SVTX_MBD;
    if(std::find(source.begin(), source.end(), GlobalVertex::VTXTYPE::SVTX) != source.end()) primary.vtxtype = GlobalVertex::VTXTYPE::SVTX;
    else if(std::find(source.begin(), source.end(), GlobalVertex::VTXTYPE::MBD) != source.end()) primary.vtxtype = GlobalVertex::VTXTYPE::MBD;
    m_container[input]->primaryVertex = primary;
  }

  if(prim_vtx->get_z() > 10) return Fun4AllReturnCodes::ABORTEVENT;
 m_stat->Fill(4);

  

    int nrecojet = -1;

    Jet::PROPERTY recojet_area_index = jets->property_index(Jet::PROPERTY::prop_area);

    for (Jet* jet : *jets){
      if(jet->get_pt() < m_ptRangeReco.first || jet->get_pt() > m_ptRangeReco.second) continue;
      if (not (std::abs(jet->get_eta()) <= 1.1 - (doFiducial?jetR.at(input):0))) continue;

      //std::cout<<"jet pt "<<jet->get_pt()<<std::endl;
      nrecojet++;

      int nChtracks = 0;

      RecoJets recojet;

      //loop over jet constituents
      for (const auto& comp : jet->get_comp_vec()){
        ParticleFlowElement *pflow = nullptr;

        SvtxTrack *trk = nullptr;

        if(m_jet_type==TYPE::FULLJET){
          pflow = pflowContainer->getParticleFlowElement(comp.second);
          trk = pflow->get_track();
        }
        else if(m_jet_type==TYPE::CHARGEJET){
          trk = trackmap->get(comp.second);
        }

         //for (SvtxTrackMap::ConstIter iter = trackmap->begin(); iter != trackmap->end(); ++iter){
            //const SvtxTrack *track = iter->second;

            //std::cout<<iter->first<<" "<<track->get_id()<<std::endl;
          //}

        //if charged track
        if(trk){
          chConstituent track_properties;

             int n_mvtx_hits = 0;
          int n_intt_hits = 0;
          int n_tpc_hits = 0;
//std::cout<<"C"<<std::endl;
          for (const auto& ckey : TrackAnalysisUtils::get_cluster_keys(trk)){
            switch (TrkrDefs::getTrkrId(ckey)){
              case TrkrDefs::mvtxId:
                n_mvtx_hits++;
                break;
              case TrkrDefs::inttId:
                n_intt_hits++;
                break;
              case TrkrDefs::tpcId:
                n_tpc_hits++;
                break;
            }
          }

          //std::cout << "x:"<<trk->get_x()<< " y:"<<trk->get_y()<< " z:"<<trk->get_z()<< " eta:"<<trk->get_eta()<< " pt:"<<trk->get_pt()<< " chi2:"<<trk->get_chisq()/trk->get_ndf()<<" "<<n_mvtx_hits<<" "<<n_intt_hits<<" "<<n_tpc_hits<<"charge: "<<trk->get_charge();

          //get vertex associated to tarck
          int id = trk->get_vertex_id();
          //std::cout<<id<<std::endl;
          GlobalVertex *vtx = prim_vtx;//vertexmap->get(id);
          //skip if track without vertex


          float DCA_xy = -999;
          float DCA_xy_unc = -999;
          float DCA_3d = -999;
          float chi2_3d = 1;
          double sign = -999;
          double sign_3d= -999;

          if (vtx == nullptr){
            std::cout << "MyJetAnalysis::process_event - Track does not have assigned vertex" << std::endl;
            //std::cout << "x:"<<pflow->get_track()->get_x()<< " y:"<<pflow->get_track()->get_y()<< " z:"<<pflow->get_track()->get_z()<< " eta:"<<pflow->get_track()->get_eta()<< " pt:"<<pflow->get_track()->get_pt()<<std::endl;
            //continue;
          } else if(n_mvtx_hits > 0 && n_intt_hits > 0){
              GetDistanceFromVertex(trk,vtx,DCA_xy,DCA_xy_unc,DCA_3d,chi2_3d);
              double dot = (trk->get_x() - vtx->get_x()) * jet->get_px() + (trk->get_y() - vtx->get_y()) * jet->get_py();
              sign = int(dot/std::abs(dot));

              double dot_3d = (trk->get_x() - vtx->get_x()) * jet->get_px() + (trk->get_y() - vtx->get_y()) * jet->get_py() + (trk->get_z() - vtx->get_z()) * jet->get_pz();
              sign_3d = int(dot_3d/std::abs(dot_3d));

              //std::cout<<" signdca:"<< sign*std::abs(DCA_xy/DCA_xy_unc)<<std::endl;
          }
          nChtracks++;
//std::cout<<"B"<<std::endl;
          
           //rerutn DCA_XY vector, val ||DCA|| + unc DCA
          

          //tarcking team way - no uncertainty of vertex in DCA calculation
          /*Acts::Vector3 vtxActs(vtx->get_x(), vtx->get_y(), vtx->get_z());
          std::pair<std::pair<float, float>, std::pair<float, float>> DCApair;
          DCApair = TrackAnalysisUtils::get_dca(pflow->get_track(),vtxActs);
          double dot2 = (pflow->get_track()->get_x() - vtx->get_x()) * jet->get_px() + (pflow->get_track()->get_y() - vtx->get_y()) * jet->get_py();
          double sign2 = int(dot2/std::abs(dot2));*/
      
          //get some track quality values
       
//std::cout<<"D"<<std::endl;

          /*m_chi2ndf[input]->Fill(pflow->get_track()->get_chisq()/pflow->get_track()->get_ndf());
          m_mvtxcl[input]->Fill(m_nmaps);
          m_inttcl[input]->Fill(m_nintt);
          m_mtpccl[input]->Fill(m_ntpc);*/
          if(m_jet_type==TYPE::FULLJET)track_properties.pflowtype = pflow->get_type();
          track_properties.vtx_id = id;
          if(m_jet_type==TYPE::FULLJET)track_properties.e = pflow->get_e();
          track_properties.eta = trk->get_eta();
          track_properties.phi = trk->get_phi();
          track_properties.pt = trk->get_pt();
          track_properties.charge = trk->get_charge();
          track_properties.DCA_xy = DCA_xy;
          track_properties.DCA_xy_unc = DCA_xy_unc;
          track_properties.sDCA_xy = sign*std::abs(DCA_xy/DCA_xy_unc);
          track_properties.DCA3d = DCA_3d;
          track_properties.sDCA3d = sign_3d*std::sqrt(chi2_3d);
          track_properties.n_mvtx = n_mvtx_hits;
          track_properties.n_intt = n_intt_hits;
          track_properties.n_tpc = n_tpc_hits;
          track_properties.chisq = trk->get_chisq();
          track_properties.ndf = trk->get_ndf();
//std::cout<<"E"<<std::endl;
          recojet.chConstituents.push_back(track_properties);
        } // end if(pflow->get_track())
        else if(m_jet_type==TYPE::FULLJET){ //neutral tracl
          neConstituent neutral_properties;
          neutral_properties.pflowtype = pflow->get_type();
          neutral_properties.e = pflow->get_e();
          neutral_properties.eta = pflow->get_eta();
          neutral_properties.phi = pflow->get_phi();
          recojet.neConstituents.push_back(neutral_properties);
        }
      } // end for (const auto& comp : jet->get_comp_vec())
//std::cout<<"F"<<std::endl;
      //recojet.id = jet->get_id();
      recojet.id = nrecojet;
      recojet.area = jet->get_property(recojet_area_index);
      recojet.num_Constituents = static_cast<int>(jet->get_comp_vec().size());
      recojet.num_ChConstituents = nChtracks;
      recojet.px = jet->get_px();
      recojet.py = jet->get_py();
      recojet.pz = jet->get_pz();
      recojet.pt = jet->get_pt();
      recojet.eta = jet->get_eta();
      recojet.phi = jet->get_phi();
      recojet.m = jet->get_mass();
      recojet.e = jet->get_e();
//std::cout<<"G"<<std::endl;
      m_container[input]->recojets.push_back(recojet);
    } // end for (Jet* jet : *jets)
    m_container[input]->reco_jet_n = static_cast<int>(nrecojet+1);

//std::cout<<"H"<<std::endl;
  
    //get truth jets
    if(m_doTruthJets){
      int ntruthjet = -1;
      TruthJets mtruthjet;
      Jet::PROPERTY truthjet_area_index = jetsMC->property_index(Jet::PROPERTY::prop_area);

      //Jet::PROPERTY recojet_area_index = jetsMC->property_index(Jet::PROPERTY::prop_area);
      
      //truth jet loop
      //for (JetMap::Iter iter = jetsMC->begin(); iter != jetsMC->end(); ++iter){   
      
      for (Jet* truthjet : *jetsMC){
        
        //Jet* truthjet = iter->second;
        if(truthjet->get_pt() < m_ptRangeTruth.first || truthjet->get_pt() > m_ptRangeTruth.second) continue;
        if (not (std::abs(truthjet->get_eta()) <= 1.1 - (doFiducial?jetR.at(input):0))) continue;
        if(truthjet->get_pt() < 10.0) continue;
        ntruthjet++;
        //std::cout<<"new Truth Jet "<<std::endl;


        //mtruthjet.id = truthjet->get_id();
        mtruthjet.id = ntruthjet;
        mtruthjet.area = truthjet->get_property(truthjet_area_index);
        mtruthjet.num_Constituents = truthjet->size_comp();
        mtruthjet.px = truthjet->get_px();
        mtruthjet.py = truthjet->get_py();
        mtruthjet.pz = truthjet->get_pz();
        mtruthjet.pt = truthjet->get_pt();
        mtruthjet.eta = truthjet->get_eta();
        mtruthjet.phi = truthjet->get_phi();
        mtruthjet.m = truthjet->get_mass();
        mtruthjet.e = truthjet->get_e();


        //jet-tarck PDG identification
        TString taggedPDGIDs[] = {"B-Meson","CharmedMeson","CharmedBaryon", "B-Baryon"};
        int forbiddenPDGIDs[] = {1,2,3,4,5,6,7,8,21,22}; 
        int nChTrackstruth = 0;

        //loop over jet constituents
        //for(auto citer = truthjet->begin_comp(); citer != truthjet->end_comp(); ++citer){
          
        for (const auto& comp : truthjet->get_comp_vec()){
          //std::cout<<"constituent: "<<std::endl;
          //PHG4Particle *g4part = truthinfo->GetPrimaryParticle(citer->second);
          PHG4Particle *g4part = truthinfo->GetPrimaryParticle(comp.second);       
	        HepMC::GenParticle* constituent = hepMCGenEvent->barcode_to_particle(g4part->get_barcode());
          if(std::abs((TDatabasePDG::Instance()->GetParticle((constituent)->pdg_id()))->Charge()) > 10e-4) nChTrackstruth++;

          //mother track tagged, stop search
          if (std::find(std::begin(taggedPDGIDs), std::end(taggedPDGIDs), TString((TDatabasePDG::Instance()->GetParticle((constituent)->pdg_id()))->ParticleClass())) != std::end(taggedPDGIDs)){
            //std::cout<<"mother tagged, pushing pdg id: "<<(constituent)->pdg_id()<<std::endl;
              mtruthjet.constituents_PDG_ID.push_back((constituent)->pdg_id());
              //continue; keep searching
          }
          //std::cout<<"recursive search: "<<std::endl;
          int i = -1;
          //not yet tagged, begin history search
    	    while (!constituent->is_beam()){
            i++;
            //std::cout<<"i: "<<i<<std::endl;
            bool breakOut = false;
            bool taggedHF = false;
            for (HepMC::GenVertex::particle_iterator mother = constituent->production_vertex()->particles_begin(HepMC::parents); mother != constituent->production_vertex()->particles_end(HepMC::parents); ++mother){
              //std::cout<<"inside for"<<std::endl;
              //found HF in parent tree
              if (std::find(std::begin(taggedPDGIDs), std::end(taggedPDGIDs), TString((TDatabasePDG::Instance()->GetParticle((*mother)->pdg_id()))->ParticleClass())) != std::end(taggedPDGIDs)){
                //taggedHF = true;
                //std::cout<<"taggedHF, pushing pdg id: "<<(*mother)->pdg_id()<<std::endl;
                mtruthjet.constituents_PDG_ID.push_back((*mother)->pdg_id());
                //break;
              }
              //reached partonic level, break search
              if (std::find(std::begin(forbiddenPDGIDs), std::end(forbiddenPDGIDs), abs((*mother)->pdg_id())) != std::end(forbiddenPDGIDs)){
                //std::cout<<"partonic reached, pdg id: "<<(*mother)->pdg_id()<<std::endl;
                //(*mother)->production_vertex()->print();
                /*for (HepMC::GenVertex::particle_iterator mother2 = (*mother)->production_vertex()->particles_begin(HepMC::parents); mother2 != constituent->production_vertex()->particles_end(HepMC::parents); ++mother2){
                    (*mother2)->production_vertex()->print();
                }*/
                if(abs((*mother)->pdg_id()) == 4 || abs((*mother)->pdg_id()) == 5){
                  quark HFquark;
                  HFquark.vtx_barcode = (*mother)->production_vertex()->barcode();
                  HFquark.pdgid = (*mother)->pdg_id();  
                  HFquark.px = (*mother)->momentum().px(); 
                  HFquark.py = (*mother)->momentum().py(); 
                  HFquark.pz = (*mother)->momentum().pz(); 
                  HFquark.e = (*mother)->momentum().e(); 
                  mtruthjet.constituents_origin_quark.push_back(HFquark);
                //(*mother)->production_vertex()->print();
                //(*mother)->print();
                //std::cout<<"v "<<(*mother)->production_vertex()->barcode()<<" "<<(*mother)->momentum().e()<<" "<<(*mother)->momentum().px()<<" "<<(*mother)->momentum().py()<<" "<<(*mother)->momentum().pz()<<std::endl;
                }
                breakOut = true;
                break;
              }
              //if(constituent->is_beam()) break;
              //std::cout<<"no HF or partonic, pdgid: "<<(*mother)->pdg_id()<<" continue search in history"<<std::endl;
              
              constituent = *mother;
            }
            //if (taggedHF)std::cout<<"TAGGED HF"<<std::endl;
            if (breakOut || taggedHF){
              //std::cout<<"breaking breakOut: "<<(breakOut?"true":"false")<<" taggedHF "<<(taggedHF?"true":"false")<<std::endl;
              break;
            } 
          }
        }// end loop over constituents

        mtruthjet.num_ChConstituents = nChTrackstruth ;
        m_container[input]->truthjets.push_back(mtruthjet);
      }// end jet loop
      m_container[input]->truth_jet_n = static_cast<int>(ntruthjet+1);
    }//end if do truth
    //fill tree
    m_T[input]->Fill();
  }//end input loop

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int FullJetFinder::ResetEvent(PHCompositeNode *topNode)
{
  //std::cout << "FullJetFinder::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int FullJetFinder::EndRun(const int runnumber)
{
  std::cout << "FullJetFinder::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int FullJetFinder::End(PHCompositeNode *topNode)
{
  std::cout << "FullJetFinder::End - Output to " << m_outputFileName << std::endl;
  
   /*CutSelection cutselection;
    cutselection.jetptmin = 5;
    cutselection.jetptmin = 30;*/

  if(PHTFileServer::get().cd(m_outputFileName)){
    m_stat->Write();
  }

  for(int i = 0 ; i < m_inputs; i++){
    if(m_T[i] ){
      if(PHTFileServer::get().cd(m_outputFileName)){
        TDirectory *cdtof = gDirectory->mkdir(m_TreeNameCollection.at(i).data());
        cdtof->cd();
        //cdtof-><CutSelection>WriteObject(cutselection, "Selection");
        //cutselection.Write("Selection");
        m_T[i]->Write();
      }
    }
  }
  std::cout << "FullJetFinder::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int FullJetFinder::Reset(PHCompositeNode *topNode)
{
 std::cout << "FullJetFinder::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void FullJetFinder::Print(const std::string &what) const
{
  std::cout << "FullJetFinder::Print(const std::string &what) const Printing info for " << what << std::endl;
}

void FullJetFinder::GetDistanceFromVertex(SvtxTrack *m_dst_track, GlobalVertex *m_dst_vertex,float &val_xy, float &err_xy, float &val_3d, float &chi2_3d){
 
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
  kfp_particle.GetDistanceFromVertexXY(kfp_vertex,val_xy,err_xy);
  val_3d = kfp_particle.GetDistanceFromVertex(kfp_vertex);
  chi2_3d = kfp_particle.GetDeviationFromVertex(kfp_vertex);
}

