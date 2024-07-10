#include "CaloEmulatorTreeMaker.h"
//for emc clusters
#include <calobase/RawCluster.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawClusterUtility.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/RawTower.h>
#include <calobase/RawTowerContainer.h>
#include <mbd/MbdPmtContainerV1.h>
#include <mbd/MbdPmtHitV1.h>
#include <HepMC/SimpleVector.h> 
//for vetex information
#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>

#include <vector>

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
CaloEmulatorTreeMaker::CaloEmulatorTreeMaker(const std::string &name, const std::string &outfilename):
  SubsysReco(name)
  
{
  useCaloTowerBuilder = false;
  useLL1=false;
  _foutname = outfilename;  
}

//____________________________________________________________________________..
CaloEmulatorTreeMaker::~CaloEmulatorTreeMaker()
{

}

//____________________________________________________________________________..
int CaloEmulatorTreeMaker::Init(PHCompositeNode *topNode)
{

  m_ll1_nodename = "LL1OUT_" + _trigger;
  m_ll1_raw_nodename = "LL1OUT_RAW_" + _trigger;
  if (Verbosity()) std::cout << __FUNCTION__ << __LINE__<<std::endl;
  _f = new TFile( _foutname.c_str(), "RECREATE");

  std::cout << " making a file = " <<  _foutname.c_str() << " , _f = " << _f << std::endl;
  
  _tree = new TTree("ttree","a persevering date tree");

  if (useLL1)
    {
      _tree->Branch("trigger_sum_emcal",b_trigger_sum_emcal,"trigger_sum_emcal[6144]/i");
      _tree->Branch("trigger_sumkey_emcal",b_trigger_sumkey_emcal,"trigger_sumkey_emcal[6144]/i");
      _tree->Branch("trigger_sum_smpl_emcal",b_trigger_sum_smpl_emcal,"trigger_sum_smpl_emcal[6144]/i");
      _tree->Branch("trigger_sum_hcalin",b_trigger_sum_hcalin,"trigger_sum_hcalin[384]/i");
      _tree->Branch("trigger_sumkey_hcalin",b_trigger_sumkey_hcalin,"trigger_sumkey_hcalin[384]/i");
      _tree->Branch("trigger_sum_smpl_hcalin",b_trigger_sum_smpl_hcalin,"trigger_sum_smpl_hcalin[384]/i");
      _tree->Branch("trigger_sum_hcalout",b_trigger_sum_hcalout,"trigger_sum_hcalout[384]/i");
      _tree->Branch("trigger_sumkey_hcalout",b_trigger_sumkey_hcalout,"trigger_sumkey_hcalout[384]/i");
      _tree->Branch("trigger_sum_smpl_hcalout",b_trigger_sum_smpl_hcalout,"trigger_sum_smpl_hcalout[384]/i");
  

      _tree->Branch("trigger_sum_emcal_ll1",b_trigger_sum_emcal_ll1, "trigger_sum_emcal_ll1[384]/i");
      _tree->Branch("trigger_sumkey_emcal_ll1",b_trigger_sumkey_emcal_ll1, "trigger_sumkey_emcal_ll1[384]/i");
      _tree->Branch("trigger_sum_smpl_emcal_ll1",b_trigger_sum_smpl_emcal_ll1, "trigger_sum_smpl_emcal_ll1[384]/i");
      _tree->Branch("trigger_sum_hcal_ll1",b_trigger_sum_hcal_ll1, "trigger_sum_hcal_ll1[384]/i");
      _tree->Branch("trigger_sumkey_hcal_ll1",b_trigger_sumkey_hcal_ll1, "trigger_sumkey_hcal_ll1[384]/i");
      _tree->Branch("trigger_sum_smpl_hcal_ll1",b_trigger_sum_smpl_hcal_ll1, "trigger_sum_smpl_hcal_ll1[384]/i");

      _tree->Branch("triggered_sums",&b_triggered_sums, "triggered_sums/i");

      _tree->Branch("trigger_bits",&b_trigger_bits);
      _tree->Branch("trigger_raw_bits",&b_trigger_raw_bits);

      _tree->Branch("trigger_sum_jet",b_trigger_sum_jet, "trigger_sum_jet[288]/i");
      _tree->Branch("trigger_sum_smpl_jet",b_trigger_sum_smpl_jet, "trigger_sum_smpl_jet[288]/i");
      _tree->Branch("trigger_sumkey_jet",b_trigger_sumkey_jet, "trigger_sumkey_jet[288]/i");

      _tree->Branch("trigger_sum_smpl_jet_input",b_trigger_sum_smpl_jet_input, "trigger_sum_smpl_jet_input[384]/i");
      _tree->Branch("trigger_sum_jet_input",b_trigger_sum_jet_input, "trigger_sum_jet_input[384]/i");
      _tree->Branch("trigger_sumkey_jet_input",b_trigger_sumkey_jet_input, "trigger_sumkey_jet_input[384]/i");

      _tree->Branch("trigger_raw_sum_smpl_emcal",b_trigger_raw_sum_smpl_emcal, "trigger_raw_sum_smpl_emcal[6144]/i");
      _tree->Branch("trigger_raw_sum_emcal",b_trigger_raw_sum_emcal, "trigger_raw_sum_emcal[6144]/i");
      _tree->Branch("trigger_raw_sumkey_emcal",b_trigger_raw_sumkey_emcal, "trigger_raw_sumkey_emcal[6144]/i");

      _tree->Branch("trigger_raw_sum_smpl_emcal_ll1",b_trigger_raw_sum_smpl_emcal_ll1, "trigger_raw_sum_smpl_emcal_ll1[384]/i");
      _tree->Branch("trigger_raw_sum_emcal_ll1",b_trigger_raw_sum_emcal_ll1, "trigger_raw_sum_emcal[384]/i");
      _tree->Branch("trigger_raw_sumkey_emcal_ll1",b_trigger_raw_sumkey_emcal_ll1, "trigger_raw_sumkey_emcal[384]/i");

      _tree->Branch("trigger_raw_sum_smpl_jet",b_trigger_raw_sum_smpl_jet, "trigger_raw_sum_smpl_jet[288]/i");
      _tree->Branch("trigger_raw_sum_jet",b_trigger_raw_sum_jet, "trigger_raw_sum_jet[288]/i");
      _tree->Branch("trigger_raw_sumkey_jet",b_trigger_raw_sumkey_jet, "trigger_raw_sumkey_jet[288]/i");

      _tree->Branch("trigger_raw_sum_smpl_jet_input",b_trigger_raw_sum_smpl_jet_input, "trigger_raw_sum_smpl_jet_input[384]/i");
      _tree->Branch("trigger_raw_sum_jet_input",b_trigger_raw_sum_jet_input, "trigger_raw_sum_jet_input[384]/i");
      _tree->Branch("trigger_raw_sumkey_jet_input",b_trigger_raw_sumkey_jet_input, "trigger_raw_sumkey_jet_input[384]/i");
    }

  _i_event = 0;
  _tree->Branch("gl1_clock",&b_gl1_clock, "gl1_clock/l");
  _tree->Branch("gl1_scaled",b_gl1_scaled, "gl1_scaled[64]/l");
  _tree->Branch("gl1_live",b_gl1_live, "gl1_live[64]/l");
  _tree->Branch("gl1_raw",b_gl1_raw, "gl1_raw[64]/l");
  _tree->Branch("gl1_rawvec",&b_gl1_rawvec, "gl1_rawvec/l");
  _tree->Branch("gl1_livevec",&b_gl1_livevec, "gl1_livevec/l");
  _tree->Branch("gl1_scaledvec",&b_gl1_scaledvec, "gl1_scaledvec/l");

  _tree->Branch("emcal_good",&b_emcal_good);
  _tree->Branch("emcal_energy",&b_emcal_energy);
  _tree->Branch("emcal_time",&b_emcal_time);
  _tree->Branch("emcal_phibin",&b_emcal_phibin);
  _tree->Branch("emcal_etabin",&b_emcal_etabin);
  _tree->Branch("hcalin_good",&b_hcalin_good);
  _tree->Branch("hcalin_energy",&b_hcalin_energy);
  _tree->Branch("hcalin_time",&b_hcalin_time);
  _tree->Branch("hcalin_phibin",&b_hcalin_phibin);
  _tree->Branch("hcalin_etabin",&b_hcalin_etabin);
  _tree->Branch("hcalout_good",&b_hcalout_good);
  _tree->Branch("hcalout_energy",&b_hcalout_energy);
  _tree->Branch("hcalout_time",&b_hcalout_time);
  _tree->Branch("hcalout_phibin",&b_hcalout_phibin);
  _tree->Branch("hcalout_etabin",&b_hcalout_etabin);

  _tree->Branch("mbd_vertex_z", &b_vertex_z, "mbd_vertex_z/F");
  // _tree->Branch("mbd_vertex_y", &b_vertex_y, "mbd_vertex_y/F");
  // _tree->Branch("mbd_vertex_x", &b_vertex_x, "mbd_vertex_x/F");
  // _tree->Branch("mbd_charge", b_mbd_charge, "mbd_charge[128]/F");
  // _tree->Branch("mbd_time", b_mbd_time, "mbd_time[128]/F");
  // _tree->Branch("mbd_ipmt", b_mbd_ipmt, "mbd_ipmt[128]/I");
  // _tree->Branch("mbd_side", b_mbd_side, "mbd_side[128]/I");
  _tree->Branch("cluster_n",&b_cluster_n);
  _tree->Branch("cluster_ecore",&b_cluster_ecore);
  _tree->Branch("cluster_pt",&b_cluster_pt);
  _tree->Branch("cluster_prob",&b_cluster_prob);
  _tree->Branch("cluster_chi2",&b_cluster_chi2);
  _tree->Branch("cluster_phi",&b_cluster_phi);
  _tree->Branch("cluster_eta",&b_cluster_eta);
  _tree->Branch("cluster_iso",&b_cluster_iso);



  std::cout << "Done initing the treemaker"<<std::endl;  
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int CaloEmulatorTreeMaker::InitRun(PHCompositeNode *topNode)
{
  if (Verbosity()) std::cout << __FUNCTION__ << __LINE__<<std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..

void CaloEmulatorTreeMaker::SetVerbosity(int verbo){
  _verbosity = verbo;
  return;
}

void CaloEmulatorTreeMaker::reset_tree_vars()
{
  if (Verbosity()) std::cout << __FUNCTION__ << __LINE__<<std::endl;
  b_cluster_n = 0;
  b_cluster_prob.clear();
  b_cluster_chi2.clear();
  b_cluster_ecore.clear();
  b_cluster_pt.clear();
  b_cluster_phi.clear();
  b_cluster_eta.clear();
  b_cluster_iso.clear();

  b_emcal_good.clear();
  b_emcal_energy.clear();
  b_emcal_time.clear();
  b_emcal_phibin.clear();
  b_emcal_etabin.clear();
  b_hcalin_good.clear();
  b_hcalin_energy.clear();
  b_hcalin_time.clear();
  b_hcalin_phibin.clear();
  b_hcalin_etabin.clear();
  b_hcalout_good.clear();  
  b_hcalout_energy.clear();
  b_hcalout_time.clear();
  b_hcalout_phibin.clear();
  b_hcalout_etabin.clear();

  return;
}

int CaloEmulatorTreeMaker::process_event(PHCompositeNode *topNode)
{

  if (Verbosity()) std::cout << __FILE__ << " "<< __LINE__<<" "<<std::endl;
  int i;

  _i_event++;

  reset_tree_vars();

  _gl1_packet = findNode::getClass<Gl1Packet>(topNode, "GL1Packet");

  _ll1out_trigger = findNode::getClass<LL1Out>(topNode, m_ll1_nodename);

  _ll1out_raw_trigger = findNode::getClass<LL1Out>(topNode, m_ll1_raw_nodename);

  _trigger_primitives_raw_emcal = findNode::getClass<TriggerPrimitiveContainer>(topNode, "TRIGGERPRIMITIVES_RAW_EMCAL");

  _trigger_primitives_raw_emcal_ll1 = findNode::getClass<TriggerPrimitiveContainer>(topNode, "TRIGGERPRIMITIVES_RAW_EMCAL_LL1");

  _trigger_primitives_raw_trigger = findNode::getClass<TriggerPrimitiveContainer>(topNode, "TRIGGERPRIMITIVES_RAW_" + _trigger);

  _trigger_primitives_trigger = findNode::getClass<TriggerPrimitiveContainer>(topNode, "TRIGGERPRIMITIVES_" + _trigger);

  _trigger_primitives_emcal = findNode::getClass<TriggerPrimitiveContainer>(topNode, "TRIGGERPRIMITIVES_EMCAL");

  _trigger_primitives_emcal_ll1 = findNode::getClass<TriggerPrimitiveContainer>(topNode, "TRIGGERPRIMITIVES_EMCAL_LL1");

  _trigger_primitives_hcalin = findNode::getClass<TriggerPrimitiveContainer>(topNode, "TRIGGERPRIMITIVES_HCALIN");

  _trigger_primitives_hcalout = findNode::getClass<TriggerPrimitiveContainer>(topNode, "TRIGGERPRIMITIVES_HCALOUT");

  _trigger_primitives_hcal_ll1 = findNode::getClass<TriggerPrimitiveContainer>(topNode, "TRIGGERPRIMITIVES_HCAL_LL1");

  GlobalVertexMap* vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
  if (!vertexmap)
    {
      // std::cout << PHWHERE << " Fatal Error - GlobalVertexMap node is missing"<< std::endl;
      std::cout << "pi0EtaByEta GlobalVertexMap node is missing" << std::endl;
      // return Fun4AllReturnCodes::ABORTRUN;
    }
  float vtx_z = 0;
  b_vertex_z = -999;
  if (vertexmap && !vertexmap->empty())
    {
      GlobalVertex* vtx = vertexmap->begin()->second;
      if (vtx)
	{
	  vtx_z = vtx->get_z();
	  b_vertex_z = vtx_z;
	}
    }

  // b_vertex_x = -99;
  // b_vertex_y = -99;
  // b_vertex_z = -99;
  // MbdVertex *vtx = nullptr;
  // //Vertex information
  // MbdVertexMap *vtxContainer = findNode::getClass<MbdVertexMap>(topNode,"MbdVertexMap");
  // while (true)
  //   {
  //     if (!vtxContainer)
  // 	{
  // 	  break;
  // 	}
  //     if (vtxContainer->empty())
  // 	{
  // 	  break;
  // 	}

  //     //More vertex information
  //     vtx = vtxContainer->begin()->second;
  //     if(!vtx)
  // 	{
  // 	  break;
  // 	}
  //     b_vertex_x = vtx->get_x();
  //     b_vertex_y = vtx->get_y();
  //     b_vertex_z = vtx->get_z();
    
  //     break;
  //   }

  if (_gl1_packet)
    {
      b_gl1_clock = _gl1_packet->lValue(0, "BCO");
      b_gl1_rawvec = _gl1_packet->lValue(0, "TriggerInput");
      b_gl1_livevec = _gl1_packet->lValue(0, "TriggerVector");
      b_gl1_scaledvec = _gl1_packet->lValue(0, "ScaledVector");
      
      for (int i = 0; i < 64; i++)
	{
	  b_gl1_scaled[i] = _gl1_packet->lValue(i, 2);
	  b_gl1_raw[i] = _gl1_packet->lValue(i, 0);
	  b_gl1_live[i] = _gl1_packet->lValue(i, 1);
	}
    }

  std::vector<unsigned int>::iterator it;
  std::vector<unsigned int> *sum;
  TriggerPrimitiveContainerv1::Range range;
  TriggerPrimitivev1::Range srange;
  if (_trigger_primitives_emcal)
    {
      i = 0;      
      range = _trigger_primitives_emcal->getTriggerPrimitives();
      for (TriggerPrimitiveContainerv1::Iter iter = range.first ; iter != range.second ; ++iter)
	{
	  _trigger_primitive = (*iter).second;
	  srange = _trigger_primitive->getSums();
	  for (TriggerPrimitive::Iter siter = srange.first; siter != srange.second; ++siter)
	    {
	      sum = (*siter).second;
	      it = max_element(sum->begin(), sum->end());
	      unsigned int summ = 0;
	      unsigned int sumk = 0;
	      unsigned int sums = 0;
	      
	      if (it != sum->end())
		{
		  summ = (*it);
		  sumk = (*siter).first;
		  sums = std::distance(sum->begin(), it);
		}
	      b_trigger_sum_emcal[i] = summ;
	      b_trigger_sumkey_emcal[i] = sumk;
	      b_trigger_sum_smpl_emcal[i] = sums;
	      
	      i++;
	    }
	}
    }
    
 if (_trigger_primitives_hcalin)
    {
      i = 0;      
      range = _trigger_primitives_hcalin->getTriggerPrimitives();
      for (TriggerPrimitiveContainerv1::Iter iter = range.first ; iter != range.second ; ++iter)
	{
	  _trigger_primitive = (*iter).second;
	  srange = _trigger_primitive->getSums();
	  for (TriggerPrimitive::Iter siter = srange.first; siter != srange.second; ++siter)
	    {
	      sum = (*siter).second;
	      it = max_element(sum->begin(), sum->end());
	      unsigned int summ = 0;
	      unsigned int sumk = 0;
	      unsigned int sums = 0;
	      
	      if (it != sum->end())
		{
		  summ = (*it);
		  sumk = (*siter).first;
		  sums = std::distance(sum->begin(), it);
		}
	      b_trigger_sum_hcalin[i] = summ;
	      b_trigger_sumkey_hcalin[i] = sumk;
	      b_trigger_sum_smpl_hcalin[i] = sums;
		
	      i++;
	    }
	}
    }
   if (_trigger_primitives_hcalout)
    {
      i = 0;      
      range = _trigger_primitives_hcalout->getTriggerPrimitives();
      for (TriggerPrimitiveContainerv1::Iter iter = range.first ; iter != range.second ; ++iter)
	{
	  _trigger_primitive = (*iter).second;
	  srange = _trigger_primitive->getSums();
	  for (TriggerPrimitive::Iter siter = srange.first; siter != srange.second; ++siter)
	    {
	      sum = (*siter).second;
	      it = max_element(sum->begin(), sum->end());
	      unsigned int summ = 0;
	      unsigned int sumk = 0;
	      unsigned int sums = 0;
	      
	      if (it != sum->end())
		{
		  summ = (*it);
		  sumk = (*siter).first;
		  sums = std::distance(sum->begin(), it);
		}
	      b_trigger_sum_hcalout[i] = summ;
	      b_trigger_sumkey_hcalout[i] = sumk;
	      b_trigger_sum_smpl_hcalout[i] = sums;
	
	      i++;
	    }
	}
    }

   if (_trigger_primitives_emcal_ll1)
    {
      i = 0;      
      range = _trigger_primitives_emcal_ll1->getTriggerPrimitives();
      for (TriggerPrimitiveContainerv1::Iter iter = range.first ; iter != range.second ; ++iter)
	{
	  _trigger_primitive = (*iter).second;
	  srange = _trigger_primitive->getSums();
	  for (TriggerPrimitive::Iter siter = srange.first; siter != srange.second; ++siter)
	    {
	      sum = (*siter).second;
	      it = max_element(sum->begin(), sum->end());
	      unsigned int summ = 0;
	      unsigned int sumk = 0;
	      unsigned int sums = 0;
	      
	      if (it != sum->end())
		{
		  summ = (*it);
		  sumk = (*siter).first;
		  sums = std::distance(sum->begin(), it);
		}
	      b_trigger_sum_emcal_ll1[i] = summ;
	      b_trigger_sumkey_emcal_ll1[i] = sumk;
	      b_trigger_sum_smpl_emcal_ll1[i] = sums;
		
	      i++;
	    }
	}
    }
    
 if (_trigger_primitives_hcal_ll1)
    {
      i = 0;      
      range = _trigger_primitives_hcal_ll1->getTriggerPrimitives();
      for (TriggerPrimitiveContainerv1::Iter iter = range.first ; iter != range.second ; ++iter)
	{
	  _trigger_primitive = (*iter).second;
	  srange = _trigger_primitive->getSums();
	  for (TriggerPrimitive::Iter siter = srange.first; siter != srange.second; ++siter)
	    {
	      sum = (*siter).second;
	      it = max_element(sum->begin(), sum->end());
	      unsigned int summ = 0;
	      unsigned int sumk = 0;
	      unsigned int sums = 0;
	      
	      if (it != sum->end())
		{
		  summ = (*it);
		  sumk = (*siter).first;
		  sums = std::distance(sum->begin(), it);
		}
	      b_trigger_sum_hcal_ll1[i] = summ;
	      b_trigger_sumkey_hcal_ll1[i] = sumk;
	      b_trigger_sum_smpl_hcal_ll1[i] = sums;
	
	      i++;
	    }
	}
    }

  if (_ll1out_trigger)
    {
      i = 0;      
      b_triggered_sums.clear();
      b_triggered_sums = _ll1out_trigger->getTriggeredSums();

      LL1Outv1::Range wrange = _ll1out_trigger->getTriggerWords();
      for (LL1Outv1::Iter iter = wrange.first ; iter != wrange.second ; ++iter)
	{
	  std::vector<unsigned int> *trigger_word = (*iter).second;

	  it = max_element(trigger_word->begin(), trigger_word->end());
	  unsigned int summ = 0;
	  unsigned int sumk = 0;
	  unsigned int sums = 0;
	      
	  if (it != trigger_word->end())
	    {
	      summ = (*it);
	      sumk = (*iter).first;
	      sums = std::distance(trigger_word->begin(), it);
	    }
	  b_trigger_sum_jet[i] = summ;
	  b_trigger_sumkey_jet[i] = sumk;
	  b_trigger_sum_smpl_jet[i] = sums;
	  
	  i++;
	}
	
      b_trigger_bits = 0;
      for (unsigned int tbit = 1; tbit <= 4; tbit++)
	{
	  b_trigger_bits |= (_ll1out_trigger->passesThreshold(tbit) ? 1U << (tbit - 0x1U): 0);
	}
    }

  if (_trigger_primitives_raw_emcal)
    {
      i = 0;      
      range = _trigger_primitives_raw_emcal->getTriggerPrimitives();
      for (TriggerPrimitiveContainerv1::Iter iter = range.first ; iter != range.second ; ++iter)
	{
	  _trigger_primitive = (*iter).second;
	  srange = _trigger_primitive->getSums();
	  for (TriggerPrimitive::Iter siter = srange.first; siter != srange.second; ++siter)
	    {
	      sum = (*siter).second;
	      it = max_element(sum->begin(), sum->end());
	      unsigned int summ = 0;
	      unsigned int sumk = 0;
	      unsigned int sums = 0;
	      
	      if (it != sum->end())
		{
		  summ = (*it);
		  sumk = (*siter).first;
		  sums = std::distance(sum->begin(), it);
		}
	      b_trigger_raw_sum_emcal[i] = summ;
	      b_trigger_raw_sumkey_emcal[i] = sumk;
	      b_trigger_raw_sum_smpl_emcal[i] = sums;
	
	      i++;
	    }
	}
    }
  if (_trigger_primitives_raw_emcal_ll1)
    {
      i = 0;      
      range = _trigger_primitives_raw_emcal_ll1->getTriggerPrimitives();
      for (TriggerPrimitiveContainerv1::Iter iter = range.first ; iter != range.second ; ++iter)
	{
	  _trigger_primitive = (*iter).second;
	  srange = _trigger_primitive->getSums();
	  for (TriggerPrimitive::Iter siter = srange.first; siter != srange.second; ++siter)
	    {
	      sum = (*siter).second;
	      it = max_element(sum->begin(), sum->end());
	      unsigned int summ = 0;
	      unsigned int sumk = 0;
	      unsigned int sums = 0;
	      
	      if (it != sum->end())
		{
		  summ = (*it);
		  sumk = (*siter).first;
		  sums = std::distance(sum->begin(), it);
		}
	      b_trigger_raw_sum_emcal_ll1[i] = summ;
	      b_trigger_raw_sumkey_emcal_ll1[i] = sumk;
	      b_trigger_raw_sum_smpl_emcal_ll1[i] = sums;

	      i++;
	    }
	}
    }

  if (_trigger_primitives_trigger)
    {
      i = 0;      
      range = _trigger_primitives_trigger->getTriggerPrimitives();
      for (TriggerPrimitiveContainerv1::Iter iter = range.first ; iter != range.second ; ++iter)
	{
	  _trigger_primitive = (*iter).second;
	  srange = _trigger_primitive->getSums();
	  for (TriggerPrimitive::Iter siter = srange.first; siter != srange.second; ++siter)
	    {
	      sum = (*siter).second;
	      it = max_element(sum->begin(), sum->end());
	      unsigned int summ = 0;
	      unsigned int sumk = 0;
	      unsigned int sums = 0;
	      
	      if (it != sum->end())
		{
		  summ = (*it);
		  sumk = (*siter).first;
		  sums = std::distance(sum->begin(), it);
		}
	      b_trigger_sum_jet_input[i] = summ;
	      b_trigger_sumkey_jet_input[i] = sumk;
	      b_trigger_sum_smpl_jet_input[i] = sums;

	      i++;
	    }
	}
    }
  if (_trigger_primitives_raw_trigger)
    {
      i = 0;      
      range = _trigger_primitives_raw_trigger->getTriggerPrimitives();
      for (TriggerPrimitiveContainerv1::Iter iter = range.first ; iter != range.second ; ++iter)
	{
	  _trigger_primitive = (*iter).second;
	  srange = _trigger_primitive->getSums();
	  for (TriggerPrimitive::Iter siter = srange.first; siter != srange.second; ++siter)
	    {
	      sum = (*siter).second;
	      it = max_element(sum->begin(), sum->end());
	      unsigned int summ = 0;
	      unsigned int sumk = 0;
	      unsigned int sums = 0;
	      
	      if (it != sum->end())
		{
		  summ = (*it);
		  sumk = (*siter).first;
		  sums = std::distance(sum->begin(), it);
		}
	      b_trigger_raw_sum_jet_input[i] = summ;
	      b_trigger_raw_sumkey_jet_input[i] = sumk;
	      b_trigger_raw_sum_smpl_jet_input[i] = sums;

	      i++;
	    }
	}
    }

  if (_ll1out_raw_trigger)
    {
      i = 0;      
      LL1Outv1::Range wrange = _ll1out_raw_trigger->getTriggerWords();
      for (LL1Outv1::Iter iter = wrange.first ; iter != wrange.second ; ++iter)
	{
	  if (i >= 288)
	    {
	      std::cout <<"bad!!"<<std::endl;
	    }
	  std::vector<unsigned int> *trigger_word = (*iter).second;

	  it = max_element(trigger_word->begin(), trigger_word->end());
	  unsigned int summ = 0;
	  unsigned int sumk = 0;
	  unsigned int sums = 0;
	      
	  if (it != trigger_word->end())
	    {
	      summ = (*it);
	      sumk = (*iter).first;
	      sums = std::distance(trigger_word->begin(), it);
	    }
	  b_trigger_raw_sum_jet[i] = summ;
	  b_trigger_raw_sumkey_jet[i] = sumk;
	  b_trigger_raw_sum_smpl_jet[i] = sums;
	  
	  i++;
	}
      b_trigger_raw_bits = 0;
      for (unsigned int tbit = 0; tbit < 4; tbit++)
	{
	  b_trigger_raw_bits |= _ll1out_raw_trigger->passesThreshold(tbit);
	}

    }
   
   if (useCaloTowerBuilder)
    {

      MbdPmtContainer *_pmts_mbd = findNode::getClass<MbdPmtContainer>(topNode, "MbdPmtContainer");

      if (_pmts_mbd)
	{

	  int npmt = 128;//_pmts_mbd->get_npmt();
	  for ( int ipmt = 0; ipmt < npmt; ipmt++)
	    {
	      MbdPmtHit *_tmp_pmt = _pmts_mbd->get_pmt(ipmt);

	      b_mbd_charge[ipmt] = _tmp_pmt->get_q();
	      b_mbd_time[ipmt] = _tmp_pmt->get_time();
	      b_mbd_side[ipmt] = ipmt/64;
	      b_mbd_ipmt[ipmt] = ipmt;
	    }

	}
      _towers = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALIN");

      int size;
      if (_towers)
	{

	  size = _towers->size(); //online towers should be the same!
	  for (int channel = 0; channel < size;channel++)
	    {
	      _tower = _towers->get_tower_at_channel(channel);
	      float energy = _tower->get_energy();
	      float time = _tower->get_time();	  
	      short good = (_tower->get_isGood() ? 1:0);
	      unsigned int towerkey = _towers->encode_key(channel);
	      int ieta = _towers->getTowerEtaBin(towerkey);
	      int iphi = _towers->getTowerPhiBin(towerkey);

	      b_hcalin_good.push_back(good);
	      b_hcalin_energy.push_back(energy);
	      b_hcalin_time.push_back(time);
	      b_hcalin_etabin.push_back(ieta);
	      b_hcalin_phibin.push_back(iphi);
	    }
	}
      _towers = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT");
      if (_towers)
	{

	  size = _towers->size(); //online towers should be the same!
	  for (int channel = 0; channel < size;channel++)
	    {
	      _tower = _towers->get_tower_at_channel(channel);
	      float energy = _tower->get_energy();
	      float time = _tower->get_time();	  
	      unsigned int towerkey = _towers->encode_key(channel);
	      int ieta = _towers->getTowerEtaBin(towerkey);
	      int iphi = _towers->getTowerPhiBin(towerkey);
	      short good = (_tower->get_isGood() ? 1:0);
	      b_hcalout_good.push_back(good);
	      b_hcalout_energy.push_back(energy);
	      b_hcalout_time.push_back(time);
	      b_hcalout_etabin.push_back(ieta);
	      b_hcalout_phibin.push_back(iphi);
	    }
	}
      _towers = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC");
      if (_towers)
	{
	  size = _towers->size(); //online towers should be the same!
	  for (int channel = 0; channel < size;channel++)
	    {
	      _tower = _towers->get_tower_at_channel(channel);
	      float energy = _tower->get_energy();
	      float time = _tower->get_time();
	      unsigned int towerkey = _towers->encode_key(channel);
	      int ieta = _towers->getTowerEtaBin(towerkey);
	      int iphi = _towers->getTowerPhiBin(towerkey);
	      short good = (_tower->get_isGood() ? 1:0);
	      b_emcal_good.push_back(good);
	      b_emcal_energy.push_back(energy);
	      b_emcal_time.push_back(time);
	      b_emcal_etabin.push_back(ieta);
	      b_emcal_phibin.push_back(iphi);
	    }
	}
    }
   {   

     RawClusterContainer* clusterContainer = findNode::getClass<RawClusterContainer>(topNode, "CLUSTERINFO_CEMC");
     if (!clusterContainer)
       {
	 std::cout << PHWHERE << "funkyCaloStuff::process_event - Fatal Error - CLUSTER_CEMC node is missing. " << std::endl;
	 return 0;
       }

     //////////////////////////////////////////
     // geometry for hot tower/cluster masking
     std::string towergeomnodename = "TOWERGEOM_CEMC";
     RawTowerGeomContainer* m_geometry = findNode::getClass<RawTowerGeomContainer>(topNode, towergeomnodename);
     if (!m_geometry)
       {
	 std::cout << Name() << "::"
		   << "CreateNodeTree"
		   << ": Could not find node " << towergeomnodename << std::endl;
	 throw std::runtime_error("failed to find TOWERGEOM node in RawClusterDeadHotMask::CreateNodeTree");
       }

     RawClusterContainer::ConstRange clusterEnd = clusterContainer->getClusters();
     RawClusterContainer::ConstIterator clusterIter;
     RawClusterContainer::ConstIterator clusterIter2;
     
     for (clusterIter = clusterEnd.first; clusterIter != clusterEnd.second; clusterIter++)
       {
	 RawCluster* recoCluster = clusterIter->second;

	 CLHEP::Hep3Vector vertex(0, 0, vtx_z);
	 CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetECoreVec(*recoCluster, vertex);


	 
	 double e   = E_vec_cluster.mag();
	 double phi = E_vec_cluster.phi();
	 double eta   = E_vec_cluster.pseudoRapidity();
	 double pt  = E_vec_cluster.perp();
	 float prob = recoCluster->get_prob();
	 float chi2 = recoCluster->get_chi2();
	 float iso = recoCluster->get_et_iso();
	 if (pt < 0.3) continue;
	 b_cluster_n++;	 
	 b_cluster_ecore.push_back(e);
	 b_cluster_pt.push_back(pt);
	 b_cluster_phi.push_back(phi);
	 b_cluster_eta.push_back(eta);
	 b_cluster_prob.push_back(prob);
	 b_cluster_chi2.push_back(chi2);
	 b_cluster_iso.push_back(iso);
       }
   }
   
   if (Verbosity()) std::cout << __FILE__ << " "<< __LINE__<<" "<<std::endl;
   
   _tree->Fill();
   
   return Fun4AllReturnCodes::EVENT_OK;
}



void CaloEmulatorTreeMaker::GetNodes(PHCompositeNode* topNode)
{


}

int CaloEmulatorTreeMaker::ResetEvent(PHCompositeNode *topNode)
{
  if (Verbosity() > 0)
    {
      std::cout << "CaloEmulatorTreeMaker::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << std::endl;
    }


  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int CaloEmulatorTreeMaker::EndRun(const int runnumber)
{
  if (Verbosity() > 0)
    {
      std::cout << "CaloEmulatorTreeMaker::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
    }
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int CaloEmulatorTreeMaker::End(PHCompositeNode *topNode)
{
  if (Verbosity() > 0)
    {
      std::cout << "CaloEmulatorTreeMaker::End(PHCompositeNode *topNode) This is the End..." << std::endl;
    }
  std::cout<<"Total events: "<<_i_event<<std::endl;
  _f->Write();
  _f->Close();

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int CaloEmulatorTreeMaker::Reset(PHCompositeNode *topNode)
{
  if (Verbosity() > 0)
    {
      std::cout << "CaloEmulatorTreeMaker::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
    }
  return Fun4AllReturnCodes::EVENT_OK;
}

