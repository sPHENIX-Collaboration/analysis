#include "CaloEmulatorTreeMaker.h"
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
  _tree->Branch("gl1_clock",&b_gl1_clock, "gl1_clock/l");
  _tree->Branch("gl1_triggervec",&b_gl1_triggervec, "gl1_triggervec/l");

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


  _i_event = 0;

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
  b_emcal_energy.clear();
  b_emcal_time.clear();
  b_emcal_phibin.clear();
  b_emcal_etabin.clear();
  b_hcalin_energy.clear();
  b_hcalin_time.clear();
  b_hcalin_phibin.clear();
  b_hcalin_etabin.clear();
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


  if (_gl1_packet)
    {
      b_gl1_clock = _gl1_packet->lValue(0, "BCO");
      b_gl1_triggervec = _gl1_packet->lValue(0, "TriggerVector");
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

