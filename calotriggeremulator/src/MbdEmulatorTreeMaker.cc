#include "MbdEmulatorTreeMaker.h"
//for emc clusters
#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexv1.h>
#include <globalvertex/GlobalVertexMap.h>
#include <globalvertex/GlobalVertexMapv1.h>
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
MbdEmulatorTreeMaker::MbdEmulatorTreeMaker(const std::string &name, const std::string &outfilename):
  SubsysReco(name)
  
{
  _foutname = outfilename;  
}

//____________________________________________________________________________..
MbdEmulatorTreeMaker::~MbdEmulatorTreeMaker()
{

}

//____________________________________________________________________________..
int MbdEmulatorTreeMaker::Init(PHCompositeNode *topNode)
{

  if (Verbosity()) std::cout << __FUNCTION__ << __LINE__<<std::endl;
  _f = new TFile( _foutname.c_str(), "RECREATE");

  std::cout << " making a file = " <<  _foutname.c_str() << " , _f = " << _f << std::endl;
  
  _tree = new TTree("ttree","a persevering date tree");


  _tree->Branch("trigger_charge_sum",b_trigger_charge_sum,"trigger_charge_sum[32]/i");
  _tree->Branch("trigger_nhit",b_trigger_nhit,"trigger_nhit[4]/i");
  _tree->Branch("trigger_time_sum",b_trigger_time_sum,"trigger_time_sum[16]/i");
  _tree->Branch("trigger_south_tavg",&b_trigger_south_tavg,"trigger_south_tavg/i");
  _tree->Branch("trigger_south_nhit",&b_trigger_south_nhit,"trigger_south_nhit/i");
  _tree->Branch("trigger_north_tavg",&b_trigger_north_tavg,"trigger_north_tavg/i");
  _tree->Branch("trigger_north_nhit",&b_trigger_north_nhit,"trigger_north_nhit/i");
  _tree->Branch("trigger_vertex_add",&b_trigger_vertex_add,"trigger_vertex_add/i");
  _tree->Branch("trigger_vertex_sub",&b_trigger_vertex_sub,"trigger_vertex_sub/i");
  
  _i_event = 0;
  _tree->Branch("gl1_clock",&b_gl1_clock, "gl1_clock/l");
  _tree->Branch("gl1_scaled",b_gl1_scaled, "gl1_scaled[64]/l");
  _tree->Branch("gl1_live",b_gl1_live, "gl1_live[64]/l");
  _tree->Branch("gl1_raw",b_gl1_raw, "gl1_raw[64]/l");
  _tree->Branch("gl1_rawvec",&b_gl1_rawvec, "gl1_rawvec/l");
  _tree->Branch("gl1_livevec",&b_gl1_livevec, "gl1_livevec/l");
  _tree->Branch("gl1_scaledvec",&b_gl1_scaledvec, "gl1_scaledvec/l");

  _tree->Branch("mbd_vertex_z", &b_vertex_z, "mbd_vertex_z/F");

  std::cout << "Done initing the treemaker"<<std::endl;  
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int MbdEmulatorTreeMaker::InitRun(PHCompositeNode *topNode)
{
  if (Verbosity()) std::cout << __FUNCTION__ << __LINE__<<std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..

void MbdEmulatorTreeMaker::SetVerbosity(int verbo){
  _verbosity = verbo;
  return;
}


int MbdEmulatorTreeMaker::process_event(PHCompositeNode *topNode)
{


  int i;

  _i_event++;

  if (Verbosity()) std::cout << __FILE__ << " "<< __LINE__<<" "<<std::endl;
  _gl1_packet = findNode::getClass<Gl1Packet>(topNode, "GL1Packet");

  _ll1out_mbd = findNode::getClass<LL1Out>(topNode, "LL1OUT_MBD");

  _trigger_primitives_mbd = findNode::getClass<TriggerPrimitiveContainer>(topNode, "TRIGGERPRIMITIVES_MBD");


  if (Verbosity()) std::cout << __FILE__ << " "<< __LINE__<<" "<<std::endl;
  GlobalVertexMap* vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
  float vertex_z = 0;
  b_vertex_z = -999;
  if (Verbosity()) std::cout << __FILE__ << " "<< __LINE__<<" "<<std::endl;

  if (vertexmap)
    {
      if (Verbosity()) std::cout << __FILE__ << " "<< __LINE__<<" "<<std::endl;
      if (!vertexmap->empty())
	{
	  if (Verbosity()) std::cout << __FILE__ << " "<< __LINE__<<" "<<std::endl;
	  GlobalVertex* vtx = vertexmap->begin()->second;
	  if (vtx)
	    {
	      if (Verbosity()) std::cout << __FILE__ << " "<< __LINE__<<" "<<std::endl;
	      vertex_z = vtx->get_z();
	      b_vertex_z = vertex_z;
	    }
	}
      if (Verbosity()) std::cout << __FILE__ << " "<< __LINE__<<" "<<std::endl;
    }
  if (Verbosity()) std::cout << __FILE__ << " "<< __LINE__<<" "<<std::endl;
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
  if (Verbosity()) std::cout << __FILE__ << " "<< __LINE__<<" "<<std::endl;
  std::vector<unsigned int>::iterator it;
  std::vector<unsigned int> *sum;
  TriggerPrimitiveContainerv1::Range range;
  TriggerPrimitivev1::Range srange;
  if (_trigger_primitives_mbd)
    {
      i = 0;      
      if (Verbosity()) std::cout << __FILE__ << " "<< __LINE__<<" "<<std::endl;
      range = _trigger_primitives_mbd->getTriggerPrimitives();
      for (TriggerPrimitiveContainerv1::Iter iter = range.first ; iter != range.second ; ++iter)
	{
	  if (Verbosity()) std::cout << __FILE__ << " "<< __LINE__<<" "<<std::endl;
	  _trigger_primitive = (*iter).second;
	  if (!_trigger_primitive)
	    {
	      continue;
	    }
	  if (Verbosity()) std::cout << __FILE__ << " "<< __LINE__<<" "<<std::endl;
	  srange = _trigger_primitive->getSums();
	  for (TriggerPrimitive::Iter siter = srange.first; siter != srange.second; ++siter)
	    {

	      sum = (*siter).second;
	      if (!sum->size())
		{
		  return Fun4AllReturnCodes::ABORTEVENT;
		}
	      unsigned int val = sum->at(0);
	      int ii = i%13;
	      int ij = i/13;
	      if ( ii < 9)
		{
		  b_trigger_charge_sum[ij*8 + ii] = val;
		}
	      else if (ii == 9)
		{
		  b_trigger_nhit[ij] = val;
		}
	      else
		{
		  b_trigger_time_sum[ij*4 + ii - 10] = val;
		}
	      i++;
	    }
	  if (Verbosity()) std::cout << __FILE__ << " "<< __LINE__<<" "<<std::endl;
	}
    }
    
  if (Verbosity()) std::cout << __FILE__ << " "<< __LINE__<<" "<<std::endl;
  if (_ll1out_mbd)
    {
      i = 0;      

      
      LL1Outv1::Range wrange = _ll1out_mbd->getTriggerWords();
      for (LL1Outv1::Iter iter = wrange.first ; iter != wrange.second ; ++iter)
	{

	  std::vector<unsigned int> *trigger_word = (*iter).second;
	  if (!trigger_word->size())
	    {
	      return Fun4AllReturnCodes::ABORTEVENT;
	    }
	  if (i == 0)
	    {
	      b_trigger_south_tavg = trigger_word->at(0);
	    }
	  else if (i==1)
	    {
	      b_trigger_north_tavg = trigger_word->at(0);
	    }
	  else if (i==2)
	    {
	      b_trigger_south_nhit = trigger_word->at(0);
	    }
	  else if (i==3)
	    {
	      b_trigger_north_nhit = trigger_word->at(0);
	    }
	  else if (i==6)
	    {
	      b_trigger_vertex_sub = trigger_word->at(0);
	    }
	  else if (i==7)
	    {
	      b_trigger_vertex_add = trigger_word->at(0);
	    }
	  i++;
	}
    }

   if (Verbosity()) std::cout << __FILE__ << " "<< __LINE__<<" "<<std::endl;
   
   _tree->Fill();
   
   return Fun4AllReturnCodes::EVENT_OK;
}



void MbdEmulatorTreeMaker::GetNodes(PHCompositeNode* topNode)
{


}

int MbdEmulatorTreeMaker::ResetEvent(PHCompositeNode *topNode)
{
  if (Verbosity() > 0)
    {
      std::cout << "MbdEmulatorTreeMaker::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << std::endl;
    }


  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int MbdEmulatorTreeMaker::EndRun(const int runnumber)
{
  if (Verbosity() > 0)
    {
      std::cout << "MbdEmulatorTreeMaker::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
    }
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int MbdEmulatorTreeMaker::End(PHCompositeNode *topNode)
{
  if (Verbosity() > 0)
    {
      std::cout << "MbdEmulatorTreeMaker::End(PHCompositeNode *topNode) This is the End..." << std::endl;
    }
  std::cout<<"Total events: "<<_i_event<<std::endl;
  _f->Write();
  _f->Close();

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int MbdEmulatorTreeMaker::Reset(PHCompositeNode *topNode)
{
  if (Verbosity() > 0)
    {
      std::cout << "MbdEmulatorTreeMaker::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
    }
  return Fun4AllReturnCodes::EVENT_OK;
}

