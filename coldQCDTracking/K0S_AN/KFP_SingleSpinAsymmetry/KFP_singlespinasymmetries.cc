

#include "KFP_singlespinasymmetries.h"

#include <fun4all/Fun4AllReturnCodes.h>

#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/PHObject.h> 
#include <phool/getClass.h>

#include <uspin/SpinDBContent.h>
#include <uspin/SpinDBContentv1.h>

#include <calotrigger/TriggerRunInfo.h>
#include <ffarawobjects/Gl1Packet.h>


#include <globalvertex/SvtxVertexMap.h>
#include <globalvertex/SvtxVertex.h>

#include <trackbase/TrkrCluster.h>
#include <trackbase/TrkrClusterContainer.h>
#include <trackbase/TrkrClusterCrossingAssocv1.h>
#include <trackbase/TrkrDefs.h>
#include <trackbase/TrkrHitSetContainer.h>
#include <trackbase/TrkrHitSet.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/SvtxTrackState_v1.h>
#include <trackbase_historic/TrackSeedContainer.h>
#include <trackbase_historic/TrackSeed.h>
#include <trackbase_historic/TrackAnalysisUtils.h>
#include <trackreco/ActsPropagator.h>


#include <CLHEP/Vector/ThreeVector.h>



#include <kfparticle_sphenix/KFParticle_Container.h>
#include <kfparticle_sphenix/KFParticle_truthAndDetTools.h>
#include <KFParticle.h>

#include <TTree.h>
#include <TFile.h>
#include <math.h>
#include <vector>
#include <TH1F.h>
#include <TH2F.h>
#include <TVector3.h>

KFP_singlespinasymmetries::KFP_singlespinasymmetries(const std::string &name):
 SubsysReco(name)
{
}

KFP_singlespinasymmetries::~KFP_singlespinasymmetries()
{
}

int KFP_singlespinasymmetries::Init(PHCompositeNode */*topNode*/)
{
  mother_mass = 0.;
  mother_pt = 0.;
  bluespin = 0;
  yellspin = 0;

  std::string s_treename = input_mother_particle + "_Tree";
  ks_tree = new TTree(s_treename.c_str(),s_treename.c_str());
  ks_tree->SetDirectory(0);

  // mother
  ks_tree->Branch("mother_pdg",                     &mother_pdg,              "mother pdg/I");
  ks_tree->Branch("mother_mass",                   &mother_mass,             "mother mass/F");
  ks_tree->Branch("mother_pt",                       &mother_pt,               "mother pt/F");
  ks_tree->Branch("mother_pt_err",               &mother_pt_err,         "mother pt error/F");
  ks_tree->Branch("mother_pz",                       &mother_pz,               "mother pz/F");
  ks_tree->Branch("mother_xf",                       &mother_xf,               "mother xf/F");
  ks_tree->Branch("mother_e",                         &mother_e,           "mother energy/F");
  ks_tree->Branch("mother_eta",                     &mother_eta,              "mother eta/F");
  ks_tree->Branch("mother_eta_err",             &mother_eta_err,        "mother eta error/F");
  ks_tree->Branch("mother_phi",                     &mother_phi,              "mother phi/F");
  ks_tree->Branch("mother_phi_err",             &mother_phi_err,        "mother phi error/F");
  ks_tree->Branch("mother_chi2",                   &mother_chi2,             "mother chi2/F");
  ks_tree->Branch("mother_ndf",                     &mother_ndf,              "mother ndf/I");
  ks_tree->Branch("mother_decaylength",     &mother_decaylength,      "mother decaylength/F");
  ks_tree->Branch("mother_decaylengthxy", &mother_decaylengthxy,    "mother decaylengthxy/F");

  // daughter 1
  ks_tree->Branch("daughter_1_id",               &daughter_1_id,      "daughter 1 track_id/I");
  ks_tree->Branch("daughter_1_pt",               &daughter_1_pt,            "daughter 1 pt/F");
  ks_tree->Branch("daughter_1_pz",               &daughter_1_pz,            "daughter 1 pz/F");
  // ks_tree->Branch("daughter_1_xf",            &daughter_1_xf,            "daughter 1 xf/F");
  ks_tree->Branch("daughter_1_eta",             &daughter_1_eta,           "daughter 1 eta/F");
  ks_tree->Branch("daughter_1_phi",             &daughter_1_phi,           "daughter 1 phi/F");
  ks_tree->Branch("daughter_1_chi2",           &daughter_1_chi2,          "daughter 1 chi2/F");
  ks_tree->Branch("daughter_1_ndf",             &daughter_1_ndf,           "daughter 1 ndf/I");
  ks_tree->Branch("daughter_1_charge",       &daughter_1_charge,        "daughter 1 charge/I");
  ks_tree->Branch("daughter_1_dca_XY",       &daughter_1_dca_XY,        "daughter 1 dca XY/F");
  ks_tree->Branch("daughter_1_nMVTX",         &daughter_1_nMVTX, "daughter 1 nClus on MVTX/I");
  ks_tree->Branch("daughter_1_nINTT",         &daughter_1_nINTT, "daughter 1 nClus on INTT/I");
  ks_tree->Branch("daughter_1_nTPC",           &daughter_1_nTPC,  "daughter 1 nClus on TPC/I");
  // ks_tree->Branch("daughter_1_mass",           &daughter_1_mass,         "daughter 1 mass/F");
  // ks_tree->Branch("daughter_1_pt_err",       &daughter_1_pt_err,     "daughter 1 pt error/F");
  // ks_tree->Branch("daughter_1_e",                 &daughter_1_e,       "daughter 1 energy/F");
  // ks_tree->Branch("daughter_1_eta_err",     &daughter_1_eta_err,    "daughter 1 eta error/F");
  // ks_tree->Branch("daughter_1_phi_err",     &daughter_1_phi_err,    "daughter 1 phi error/F");

  // daughter 2
  ks_tree->Branch("daughter_2_id",               &daughter_2_id,     "daughter 2 track_id/I");
  ks_tree->Branch("daughter_2_pt",               &daughter_2_pt,           "daughter 2 pt/F");
  ks_tree->Branch("daughter_2_pz",               &daughter_2_pz,           "daughter 2 pz/F");
  // ks_tree->Branch("daughter_2_xf",            &daughter_2_xf,           "daughter 2 xf/F");
  ks_tree->Branch("daughter_2_eta",             &daughter_2_eta,          "daughter 2 eta/F");
  ks_tree->Branch("daughter_2_phi",             &daughter_2_phi,          "daughter 2 phi/F");
  ks_tree->Branch("daughter_2_chi2",           &daughter_2_chi2,         "daughter 2 chi2/F");
  ks_tree->Branch("daughter_2_ndf",             &daughter_2_ndf,          "daughter 2 ndf/I");
  ks_tree->Branch("daughter_2_charge",       &daughter_2_charge,       "daughter 2 charge/I");
  ks_tree->Branch("daughter_2_dca_XY",       &daughter_2_dca_XY,       "daughter 2 dca XY/F");
  ks_tree->Branch("daughter_2_nMVTX",         &daughter_2_nMVTX, "daughter 2 nClus on MVTX/I");
  ks_tree->Branch("daughter_2_nINTT",         &daughter_2_nINTT, "daughter 2 nClus on INTT/I");
  ks_tree->Branch("daughter_2_nTPC",           &daughter_2_nTPC,  "daughter 2 nClus on TPC/I");

  // spin staff
  ks_tree->Branch("candidate_crossing",     &candidate_crossing,      "candidate_crossing/I");
  ks_tree->Branch("bluespin",                         &bluespin,                "bluespin/I");
  ks_tree->Branch("yellspin",                         &yellspin,                "yellspin/I");
  ks_tree->Branch("bluepol",                  &bluepolarization,       "blue polarization/F");
  ks_tree->Branch("yellpol",                &yellowpolarization,       "yell polarization/F");
  ks_tree->Branch("bluepolerr",            &bluepolarizationerr, "blue polarization error/F");
  ks_tree->Branch("yellpolerr",          &yellowpolarizationerr, "yell polarization error/F");

  // daughter tree
  // daughter_tree = new TTree("daughter_Tree","daughter_Tree");
  return Fun4AllReturnCodes::EVENT_OK;
}

int KFP_singlespinasymmetries::InitRun(PHCompositeNode *topNode)
{
  SpinDBContent* spindb = findNode::getClass<SpinDBContent>(topNode, "SpinDBContent");
  if (!spindb)
  {
    std::cout << PHWHERE << ":: SpinDB node missing! Skipping run" << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }
  
  for (int i = 0; i < 120; i++)
  {
    bluepattern[i] = -1*spindb->GetSpinPatternBlue(i); //-1 for IP12 -> IP8
    yellowpattern[i] = -1*spindb->GetSpinPatternYellow(i); //-1 for IP12 -> IP8
  }

  xingshift = spindb->GetCrossingShift();
  spindb->GetPolarizationBlue(0,bluepolarization,bluepolarizationerr);
  spindb->GetPolarizationYellow(0,yellowpolarization,yellowpolarizationerr);

  return Fun4AllReturnCodes::EVENT_OK;
}

int KFP_singlespinasymmetries::process_event(PHCompositeNode *topNode)
{
  //--------------------//
  //  get bunch number  //
  //--------------------//
  Gl1Packet* gl1packet = findNode::getClass<Gl1Packet>(topNode, "GL1RAWHIT");
  int bunchnum = gl1packet->getBunchNumber(); // this will get me triggered crossing bunch number
  int sphnxbunchnum = (bunchnum + xingshift) % 120;
  // std::cout << "trigerred bunchnum: " << bunchnum << std::endl;
  std::cout << Form("triggered bunch number: %03d", bunchnum) << std::endl;



  //------------------//
  //  tracking thing  //
  //------------------//
  SvtxTrackMap *thisTrackMap = findNode::getClass<SvtxTrackMap>(topNode, "SvtxTrackMap");
  TrkrClusterContainer *trkrContainer = findNode::getClass<TrkrClusterContainer>(topNode, "TRKR_CLUSTER_SEED");


  
  // ====== trigger run info ========
  // auto triggerruninfo = findNode::getClass<TriggerRunInfo>(topNode, "TriggerRunInfo");

  KFParticle_Container* kfp_container = findNode::getClass<KFParticle_Container>(topNode, m_kfpcontainername.c_str());

  KFParticle_truthAndDetTools kfp_truthanddettools;


  
  //-------------//
  //  dca thing  //
  //-------------//
  CLHEP::Hep3Vector vertex(0., 0., 0.);
  SvtxVertex *svtx_vtx = nullptr;
  SvtxVertexMap *vertexMap = findNode::getClass<SvtxVertexMap>(topNode, "SvtxVertexMap");
  if(!vertexMap->empty())
  {
    svtx_vtx = vertexMap->begin()->second;
    if(svtx_vtx)
    {
      vertex.setX(svtx_vtx->get_x());
      vertex.setY(svtx_vtx->get_y());
      vertex.setZ(svtx_vtx->get_z());
    }
    else
    {std::cout << "SvtxVertexMap is empty!" << std::endl;}
  }
  Acts::Vector3 acts_vertex(vertex.x(), vertex.y(), vertex.z());



  for (auto &kfp : *kfp_container)
  {
    mother_pdg = kfp.second->GetPDG(); 

    //-----------//
    //  pdg cut  //
    //-----------//
    if(mother_pdg != input_mother_particle_pdg) continue; //D0 PDG: 421 //Ks PDG: 310 // omega(782): 223 //rho(770) neutral: 113

    //----------------//
    //  mother stuff  //
    //----------------//
    mother_mass    = kfp.second->GetMass();
    mother_pt      = kfp.second->GetPt();
    mother_pt_err  = kfp.second->GetErrPt();
    mother_pz      = kfp.second->GetPz();
    mother_xf      = 2*mother_pz/200;
    mother_e       = kfp.second->GetE();
    mother_eta     = kfp.second->GetEta();
    mother_eta_err = kfp.second->GetErrEta();
    mother_phi     = kfp.second->GetPhi();
    mother_phi_err = kfp.second->GetErrPhi();
    mother_chi2    = kfp.second->GetChi2();
    mother_ndf     = kfp.second->GetNDF();
    mother_decaylength   = kfp.second->GetDecayLength();
    mother_decaylengthxy = kfp.second->GetDecayLengthXY();
    
    
    
    //------------------//
    //  daughter stuff  //
    //------------------//
    // get crossing number from daughters
    std::vector<int> daughterIds = kfp.second->DaughterIds();
    int just_check_crossing_number_1 = -123; int just_check_crossing_number_2 = -456;
    for (int id : daughterIds)
    {
      SvtxTrack *thisTrack = kfp_truthanddettools.getTrack(id, thisTrackMap);
      if (!thisTrack)
      {
        continue;
      }


      
      //-----------------------------------------------//
      //  get # of clusters from silicon and tpc seed  //
      //-----------------------------------------------//
      int nMVTX = 0; int nINTT = 0; int nTPC = 0; // number of clusters
      
      /*
      std::cout << "one" << std::endl;
      auto cluster_range = trkrContainer->getClusters();
      std::cout << "two" << std::endl;
      for (auto clus_iter = cluster_range.first; clus_iter != cluster_range.second; ++clus_iter)
      {
            auto cluskey = clus_iter->first;
            std::cout << "three" << std::endl;
            // auto clus = clus_iter->second;
            if(TrkrDefs::getTrkrId(cluskey) == TrkrDefs::TrkrId::mvtxId){nMVTX++;}
            if(TrkrDefs::getTrkrId(cluskey) == TrkrDefs::TrkrId::inttId){nINTT++;}
            if(TrkrDefs::getTrkrId(cluskey) == TrkrDefs::TrkrId::tpcId){nTPC++;}
      }
      */
      
      TrackSeed   *silicon_seed = thisTrack->get_silicon_seed(); 
      TrackSeed   *tpc_seed     = thisTrack->get_tpc_seed();
      TrkrCluster *trkrCluster  = nullptr;
      // std::cout << "one" << std::endl;

      if(silicon_seed)
      {
        for(auto key_iter = silicon_seed->begin_cluster_keys(); key_iter != silicon_seed->end_cluster_keys(); ++key_iter)
        {
          const auto& cluster_key = *key_iter;
          trkrCluster = trkrContainer->findCluster(cluster_key);
          // std::cout << "two" << std::endl;
          if(!trkrCluster) continue;

          if(TrkrDefs::getTrkrId(cluster_key) == TrkrDefs::TrkrId::mvtxId){nMVTX++;}
          if(TrkrDefs::getTrkrId(cluster_key) == TrkrDefs::TrkrId::inttId){nINTT++;}
        }
      }

      if(tpc_seed)
      {
        for(auto key_iter = tpc_seed->begin_cluster_keys(); key_iter != tpc_seed->end_cluster_keys(); ++key_iter)
        {
          const auto& cluster_key = *key_iter;
          trkrCluster = trkrContainer->findCluster(cluster_key);
          // std::cout << "three" << std::endl;
          if(!trkrCluster) continue;

          if(TrkrDefs::getTrkrId(cluster_key) == TrkrDefs::TrkrId::tpcId){nTPC++;}
        }
      }
      
      

      
      int daughter_charge = thisTrack->get_charge();
      auto dcapair = TrackAnalysisUtils::get_dca(thisTrack, acts_vertex);

      if(daughter_charge < 0)
      {
        daughter_1_id     = thisTrack->get_id();
        daughter_1_pt     = thisTrack->get_pt();
        daughter_1_eta    = thisTrack->get_eta();
        daughter_1_phi    = thisTrack->get_phi();
        daughter_1_pz     = thisTrack->get_pz();
        daughter_1_chi2   = thisTrack->get_chisq();
        daughter_1_ndf    = thisTrack->get_ndf();
        daughter_1_charge = thisTrack->get_charge();

        daughter_1_dca_XY = dcapair.first.first;

        daughter_1_nMVTX = nMVTX;
        daughter_1_nINTT = nINTT;
        daughter_1_nTPC  = nTPC; 

        just_check_crossing_number_1 = thisTrack->get_crossing();
      }

      if(daughter_charge > 0)
      {
        daughter_2_id     = thisTrack->get_id();
        daughter_2_pt     = thisTrack->get_pt();
        daughter_2_eta    = thisTrack->get_eta();
        daughter_2_phi    = thisTrack->get_phi();
        daughter_2_pz     = thisTrack->get_pz();
        daughter_2_chi2   = thisTrack->get_chisq();
        daughter_2_ndf    = thisTrack->get_ndf();
        daughter_2_charge = thisTrack->get_charge();

        daughter_2_dca_XY = dcapair.first.first;

        daughter_2_nMVTX = nMVTX;
        daughter_2_nINTT = nINTT;
        daughter_2_nTPC  = nTPC; 

        just_check_crossing_number_2 = thisTrack->get_crossing();
      }
      

      candidate_crossing = (sphnxbunchnum + thisTrack->get_crossing()) % 120;
    }

    // just curious
    if(just_check_crossing_number_1 != just_check_crossing_number_2)
    {
      std::cout << Form("WHAT?????? crossing_1: %3d; crossing_2: %3d", just_check_crossing_number_1, just_check_crossing_number_2);
    }

    // should do more here, like DIRA, IPchi2 etc. for heavy flavor
    
    
    //--------------//
    //  spin stuff  //
    //--------------//
    bluespin = bluepattern[candidate_crossing];
    yellspin = yellowpattern[candidate_crossing];
    ks_tree->Fill();
    
  }
  return Fun4AllReturnCodes::EVENT_OK;
}

int KFP_singlespinasymmetries::ResetEvent(PHCompositeNode */*topNode*/)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

int KFP_singlespinasymmetries::End(PHCompositeNode */*topNode*/)
{

  //  calculate single spin asymmetries here
  


  TFile *outfile = new TFile(m_outputfilename.c_str(),"RECREATE");
  ks_tree->Write();
  outfile->Delete();
  delete ks_tree;

  return Fun4AllReturnCodes::EVENT_OK;
}

int KFP_singlespinasymmetries::Reset(PHCompositeNode */*topNode*/)
{
  return Fun4AllReturnCodes::EVENT_OK;
}