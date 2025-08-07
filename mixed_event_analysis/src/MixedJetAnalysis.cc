#include "TMath.h"
#include "TLorentzVector.h"
#include "MixedJetAnalysis.h"
#include <eventplaneinfo/Eventplaneinfo.h>
#include <eventplaneinfo/EventplaneinfoMap.h>
#include <centrality/CentralityInfov2.h>
#include <calotrigger/MinimumBiasInfov1.h>
//for emc clusters
#include <TMath.h>
#include <jetbackground/TowerBackground.h>
//for vetex information
#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>
#include <vector>
#include <mbd/MbdPmtContainer.h>
#include <mbd/MbdPmtHit.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/PHObject.h>
#include <phool/getClass.h>
// G4Cells includes

#include <iostream>

#include <map>

//____________________________________________________________________________..
MixedJetAnalysis::MixedJetAnalysis(const std::string &name, const std::string &outfilename):
SubsysReco(name)
  
{
  _foutname = outfilename;  
}

//____________________________________________________________________________..
MixedJetAnalysis::~MixedJetAnalysis()
{

}

//____________________________________________________________________________..
int MixedJetAnalysis::Init(PHCompositeNode * /*topNode*/)
{



  if (Verbosity()) std::cout << __FUNCTION__ << __LINE__<<std::endl;
  _f = new TFile( _foutname.c_str(), "RECREATE");

  std::cout << " making a file = " <<  _foutname.c_str() << " , _f = " << _f << std::endl;
  _tree_end = new TTree("ttree_end","run stats");
  _tree_end->Branch("runnumber", &b_runnumber);
  _tree_end->Branch("segment", &b_segment);     
  _tree_end->Branch("scaled_scalers", &b_scaled_scalers, "scaled_scalers[64]/l");
  _tree_end->Branch("live_scalers", &b_live_scalers, "live_scalers[64]/l");
  _tree_end->Branch("raw_scalers", &b_raw_scalers, "raw_scalers[64]/l");
  _tree_end->Branch("scaled_last", &last_scaled, "scaled_last[64]/l");
  _tree_end->Branch("live_last", &last_live, "live_last[64]/l");
  _tree_end->Branch("raw_last", &last_raw, "raw_last[64]/l");
  _tree_end->Branch("scaled_first", &first_scaled, "scaled_first[64]/l");
  _tree_end->Branch("live_first", &first_live, "live_first[64]/l");
  _tree_end->Branch("raw_first", &first_raw, "raw_first[64]/l");

  _tree = new TTree("ttree","a persevering date tree");

  _tree->Branch("gl1_scaled", &b_gl1_scaled, "gl1_scaled/l");
  _tree->Branch("gl1_live", &b_gl1_live, "gl1_live/l");
  _tree->Branch("gl1_min_bias", &b_gl1_raw, "gl1_min_bias/l");
  
  _i_event = 0;

  _tree->Branch("centrality",&b_centrality);
  _tree->Branch("psi",&b_psi);
  _tree->Branch("v2",&b_v2);
  _tree->Branch("flow_fail",&b_flow_fail);
  _tree->Branch("minbias",&b_ismb);
  
  _tree->Branch("mbtd_npt", &b_mbd_npmt);
  _tree->Branch("mbd_time", &b_mbd_time);
  _tree->Branch("mbd_charge", &b_mbd_charge);
  _tree->Branch("mbd_side", &b_mbd_side);
  _tree->Branch("mbd_ipmt", &b_mbd_ipmt);


  _tree->Branch("mbd_vertex_z", &b_vertex_z, "mbd_vertex_z/F");
  _tree->Branch("time_zero", &b_time_zero, "time_zero/F");

  for (auto cone_size : m_cone_sizes)
    {
      int coneindex = cone_size.first;
      int cone = cone_size.second.first;
      bool bkg = cone_size.second.second;
      std::cout << " Cone size : " << cone << std::endl;
      b_njet.push_back(0);
      b_jet_pt.push_back(new std::vector<float>());
      b_jet_eta.push_back(new std::vector<float>());
      b_jet_phi.push_back(new std::vector<float>());

      b_njet_mix.push_back(0);
      b_jet_mix_pt.push_back(new std::vector<float>());
      b_jet_mix_eta.push_back(new std::vector<float>());
      b_jet_mix_phi.push_back(new std::vector<float>());

      _tree->Branch(Form("njet_%d%s", cone, (bkg?"_sub":"")), &b_njet.at(coneindex), Form("njet_%d%s/I", cone,(bkg?"_sub":"")));
      _tree->Branch(Form("jet_pt_%d%s", cone, (bkg?"_sub":"")), b_jet_pt.at(coneindex));
      _tree->Branch(Form("jet_eta_%d%s", cone, (bkg?"_sub":"")), b_jet_eta.at(coneindex));
      _tree->Branch(Form("jet_phi_%d%s", cone, (bkg?"_sub":"")), b_jet_phi.at(coneindex));

      _tree->Branch(Form("njet_mix_%d%s", cone, (bkg?"_sub":"")), &b_njet_mix.at(coneindex), Form("njet_mix_%d%s/I", cone,(bkg?"_sub":"")));
      _tree->Branch(Form("jet_mix_pt_%d%s", cone, (bkg?"_sub":"")), b_jet_mix_pt.at(coneindex));
      _tree->Branch(Form("jet_mix_eta_%d%s", cone, (bkg?"_sub":"")), b_jet_mix_eta.at(coneindex));
      _tree->Branch(Form("jet_mix_phi_%d%s", cone, (bkg?"_sub":"")), b_jet_mix_phi.at(coneindex));

      std::map<unsigned int, std::vector<std::vector<struct mixedjet>>*> empty_map;
      unsigned int ncentbins = (m_use_psi?20:100);
      for (unsigned int i = 0; i < ncentbins; i++)
	{
	  for (unsigned int j = 0; j < 12; j++)
	    {
	      if (m_use_psi)
		{
		  for (unsigned int k = 0; k < 8; k++)
		    {
		      unsigned int total_bin = (i & 0x1f) + ((j & 0xf) << 0x5U) + ((k & 0x7) << 0x9U);
		      
		      empty_map[total_bin] = new std::vector<std::vector<struct mixedjet>>;;
		    }
		}
	      else
		{
		  unsigned int total_bin = (i & 0xff) + ((j & 0xf) << 0x8U);
		      
		  empty_map[total_bin] = new std::vector<std::vector<struct mixedjet>>;;

		}
	    }
	}      
      m_mixed_jet_bank.push_back(empty_map);      
    }
  std::cout << "Done initing the treemaker"<<std::endl;  
  return Fun4AllReturnCodes::EVENT_OK;
}



void MixedJetAnalysis::reset_tree_vars()
{
  if (Verbosity()) std::cout << __FUNCTION__ << __LINE__<<std::endl;

  b_ismb = 0;
  b_centrality = -99;
  b_mbd_npmt = 0;
  b_mbd_charge.clear();
  b_mbd_time.clear();
  b_mbd_ipmt.clear();
  b_mbd_side.clear();

  for (int i = 0; i < m_n_cone_sizes; i++)
    {
      b_njet.at(i) = 0;
      b_jet_pt.at(i)->clear();
      b_jet_eta.at(i)->clear();
      b_jet_phi.at(i)->clear();

      b_njet_mix.at(i) = 0;
      b_jet_mix_pt.at(i)->clear();
      b_jet_mix_eta.at(i)->clear();
      b_jet_mix_phi.at(i)->clear();
    }

  b_gl1_scaled = 0x0;
  b_gl1_live = 0x0;
  b_gl1_raw = 0x0;


  return;
}

int MixedJetAnalysis::process_event(PHCompositeNode *topNode)
{

  if (Verbosity()) std::cout << __FILE__ << " "<< __LINE__<<" Event : " << _i_event <<std::endl;

  reset_tree_vars();

  Gl1Packet* gl1p = findNode::getClass<Gl1Packet>(topNode, 14001);
  b_gl1_scaled = gl1p->getScaledVector();
  b_gl1_live = gl1p->getLiveVector();
  b_gl1_raw = gl1p->lValue(10, 1);

  if (_i_event == 0)
    {
      for (int i = 0; i < 64; i++)
	{
	  first_raw[i] = gl1p->lValue(i, 0);
	  first_live[i] = gl1p->lValue(i, 1);
	  first_scaled[i] = gl1p->lValue(i, 2);
	}
    }
  else
    {
      for (int i = 0; i < 64; i++)
	{
	  last_raw[i] = gl1p->lValue(i, 0);
	  last_live[i] = gl1p->lValue(i, 1);
	  last_scaled[i] = gl1p->lValue(i, 2);
	}
    }
  
  _i_event++;

  // if (!(((b_gl1_scaled >> 10) & 0x1) == 0x1 ||
  // 	((b_gl1_scaled >> 12) & 0x1) == 0x1 ||
  // 	((b_gl1_scaled >> 13) & 0x1) == 0x1 ||
  // 	((b_gl1_scaled >> 14) & 0x1) == 0x1)) return Fun4AllReturnCodes::EVENT_OK;
      
  m_minimumbiasinfo = findNode::getClass<MinimumBiasInfo>(topNode, "MinimumBiasInfo");
  m_towback = findNode::getClass<TowerBackground>(topNode, "TowerInfoBackground_Sub2");
  m_central = findNode::getClass<CentralityInfov2>(topNode, "CentralityInfo");
  m_evpmap = findNode::getClass<EventplaneinfoMap>(topNode, "EventplaneinfoMap");  

  b_centrality = 0;
  b_ismb = 0;
  b_psi = 0;
  b_v2 = -99;
  if (!m_evpmap && m_use_psi)
    {
      exit(-1);
    }
  if (m_use_psi)
    {
      if(!(m_evpmap->empty()))
	{
	  
	  auto EPDNS = m_evpmap->get(EventplaneinfoMap::sEPDNS);
	  b_psi = EPDNS->get_shifted_psi(2);
	}
    }
  if (m_central)
    b_centrality = (m_central->has_centile(CentralityInfo::PROP::mbd_NS)?m_central->get_centrality_bin(CentralityInfo::PROP::mbd_NS) : -99);
  if (m_minimumbiasinfo)
    b_ismb = (m_minimumbiasinfo->isAuAuMinimumBias()? 1 : 0);
  if (m_towback)
    {
      b_v2 = m_towback->get_v2();
      b_flow_fail = m_towback->get_flow_failure_flag();
    }
  GlobalVertexMap* vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");


  float vtx_z = 0;
  b_vertex_z = -999;
  if (vertexmap && !vertexmap->empty())
    {
      GlobalVertex* vtx = vertexmap->begin()->second;
      if (vtx)
	{
	  vtx_z = vtx->get_z();
	  b_vertex_z = vtx_z;
	  b_time_zero = vtx->get_t();
	}
    }
  if (Verbosity()) std::cout << "Mbd Vertex = " << b_vertex_z << std::endl;

  if (!b_ismb) return Fun4AllReturnCodes::EVENT_OK;

  if (Verbosity()) std::cout << "MINBIAS event"<< std::endl;

  unsigned int centrality_bin = b_centrality/5;

  unsigned int vertex_bin = 0;

  for (unsigned int i = 0; i < m_nbin_vertex; i++)
    {
      if (b_vertex_z < m_vertex_bin_edges[i+1])
	{
	  vertex_bin = i;
	  break;
	}
    }
  unsigned int psi_bin = 0;
  if (m_use_psi)
    {
      for (unsigned int i = 0; i < m_nbin_psi; i++)
	{
	  if (b_psi < (-1*TMath::Pi()/2. + (i+1)+TMath::Pi()/8.))
	    {
	      psi_bin = i;
	      break;
	    }
	}
      m_total_bin = (centrality_bin & 0x1f) + ((vertex_bin & 0xf) << 0x5U) + ((psi_bin & 0x7) << 9U);
    }
  else
    {
      m_total_bin = (centrality_bin & 0xff) + ((vertex_bin & 0xf) << 0x8U);
    }
  //m_total_bin = (centrality_bin & 0xff) + ((vertex_bin & 0xf) << 0x8U);

  if (Verbosity()) std::cout << " total bin : " << m_total_bin << std::endl;
  bool keep_candidate = false;
  for (int i = 0; i < m_n_cone_sizes; i++)
    {
      bool jet_candidate = process_jets(i, topNode);
      
      if (jet_candidate)
	{
	  if (Verbosity()) std::cout << "Found good Jet" << std::endl;
	  keep_candidate = true;
	  get_mixed_events(i);
	}
      if (Verbosity()) std::cout << "Now storing jets vector" << std::endl;
      store_mixed_events(i);

      if (Verbosity())
	{
	  show_queue(i);
	}
    }

  if (!keep_candidate) return Fun4AllReturnCodes::EVENT_OK;
  
  MbdPmtContainer *pmts_mbd = findNode::getClass<MbdPmtContainer>(topNode, "MbdPmtContainer");

  if (pmts_mbd)
    {

      for (int i = 0 ; i < 128; i++)
	{
	  MbdPmtHit *tmp_pmt = pmts_mbd->get_pmt(i);
	  
	  float charge = tmp_pmt->get_q();
	  float time = tmp_pmt->get_time();
	  if (fabs(time) < 25. && charge > 0.4)
	    {
	      b_mbd_charge.push_back(charge);
	      b_mbd_time.push_back(time);
	      b_mbd_ipmt.push_back(i);
	      b_mbd_side.push_back(i/64);
	      b_mbd_npmt++;
	    }
	}
    }

  _tree->Fill();
   
  return Fun4AllReturnCodes::EVENT_OK;
}

int MixedJetAnalysis::process_jets(int cone_index, PHCompositeNode* topNode)
{

  bool isbkg = m_cone_sizes[cone_index].second;
  int cone_size = m_cone_sizes[cone_index].first;

  std::string recoJetName = Form("AntiKt_Tower_r%02d", cone_size);

  bool jet_candidate = false;
  
  if (isbkg)
    {
      recoJetName = Form("AntiKt_Tower_r%02d_Sub1", cone_size);
    }

  JetContainer *jets = findNode::getClass<JetContainer>(topNode, recoJetName);
  if (jets)
    {
      // zero out counters
      for (auto jet : *jets)
	{

	  if (jet->get_pt() < pt_cut) continue;

	  if (jet->get_pt() >= signal_pt_cut) jet_candidate = true;
	  
	  b_njet.at(cone_index)++;
	  b_jet_pt.at(cone_index)->push_back(jet->get_pt());
	  b_jet_eta.at(cone_index)->push_back(jet->get_eta());
	  b_jet_phi.at(cone_index)->push_back(jet->get_phi());

	}
    }
  
  return jet_candidate;

}

int MixedJetAnalysis::store_mixed_events(int cone_index)
{
  
  auto mixed_jet_vector = m_mixed_jet_bank.at(cone_index)[m_total_bin];
  if (Verbosity()) std::cout << "Storing Mixed events" << std::endl;
  if (!mixed_jet_vector) return 1;
  if (Verbosity()) std::cout << "entering Mixed event" << std::endl;
  
  int njet = b_njet.at(cone_index);
  //if (njet == 0) return 1;
  if (mixed_jet_vector->size() > m_max_length_buffer) return 0;
  std::vector<struct mixedjet> moxies = {};
  for (int ij = 0 ; ij < njet ; ij++)
    {
      struct mixedjet moxy;
      moxy.pt = b_jet_pt.at(cone_index)->at(ij);
      moxy.eta = b_jet_eta.at(cone_index)->at(ij);
      moxy.phi = b_jet_phi.at(cone_index)->at(ij);
      moxies.push_back(moxy);
    }

  mixed_jet_vector->push_back(moxies);
  
  return 0;

}

int MixedJetAnalysis::get_mixed_events(int cone_index)
{

  auto mixed_jet_vector = m_mixed_jet_bank.at(cone_index)[m_total_bin];
  if (Verbosity()) std::cout << "Getting Mixed events" << std::endl;
  if (!mixed_jet_vector) return 1;
  if (Verbosity()) std::cout << "Got Mixed event" << std::endl;
  int njet = mixed_jet_vector->size();
  if (Verbosity()) std::cout << "Found " << njet << " Jet vectors" << std::endl;

  if (njet == 0)
    {
      b_njet_mix.at(cone_index) = -1;
      return 1;
    }
  auto moxies = mixed_jet_vector->at(0);

  b_njet_mix.at(cone_index) = 0;
  for (auto moxy : moxies)
    {
      if (Verbosity()) std::cout << " getting mixed jet: " << moxy.pt << " / " << moxy.eta << " / " << moxy.phi << std::endl;
      b_njet_mix.at(cone_index)++;
      b_jet_mix_pt.at(cone_index)->push_back(moxy.pt);
      b_jet_mix_eta.at(cone_index)->push_back(moxy.eta);
      b_jet_mix_phi.at(cone_index)->push_back(moxy.phi);
    }
  if (Verbosity()) std::cout << "Now erasing vector" << std::endl;
  mixed_jet_vector->erase(mixed_jet_vector->begin());
  
  return 0;

}

int MixedJetAnalysis::show_queue(int cone_index)
{
  for (unsigned int i = 0; i < 100; i++)
    {
      std::cout << i << "\t | ";
      for (unsigned int j = 0; j < 1; j++)
	{
	  unsigned int total_bin = (i & 0xff) + ((j & 0xff) << 0x8U);
	  int number_in_queue = m_mixed_jet_bank.at(cone_index)[total_bin]->size();
	  std::cout <<  number_in_queue << " \t";	  	 
	}
      std::cout <<  " | " << std::endl;
    }
  return 0;
}
//____________________________________________________________________________..
int MixedJetAnalysis::End(PHCompositeNode */*topNode*/)
{
  if (Verbosity() > 0)
    {
      std::cout << "MixedJetAnalysis::End(PHCompositeNode *topNode) This is the End..." << std::endl;
    }
  std::cout<<"Total events: "<<_i_event<<std::endl;

  for (int i = 0; i < 64; i++)
    {
      b_scaled_scalers[i] = (last_scaled[i] - first_scaled[i]);
      b_live_scalers[i] = (last_live[i] - first_live[i]);
      b_raw_scalers[i] = (last_raw[i] - first_raw[i]);
    }

  _tree_end->Fill();
  
  _f->Write();
  _f->Close();

  return Fun4AllReturnCodes::EVENT_OK;
}

