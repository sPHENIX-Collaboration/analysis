#include "ElectronPid.h"

#include <cstdlib>
#include <cstdio>
#include <iomanip>
#include <fstream>

#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>                         // for PHIODataNode
#include <phool/PHNode.h>                               // for PHNode
#include <phool/PHNodeIterator.h>
#include <phool/PHObject.h>                             // for PHObject
#include <phool/getClass.h>
#include <phool/phool.h>  // for PHWHERE
#include <phool/PHRandomSeed.h>
#include <phool/getClass.h>

#include <fun4all/Fun4AllReturnCodes.h>

#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/SvtxTrack.h>

// gsl
#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>

#include <TVector3.h>
#include <TMatrixFfwd.h>    // for TMatrixF

#include <iostream>                            // for operator<<, basic_ostream
#include <set>                                 // for _Rb_tree_iterator, set
#include <utility>                             // for pair

class PHCompositeNode;

using namespace std;

ElectronPid::ElectronPid(const std::string& name, const std::string &filename) : SubsysReco(name)
{
  OutputNtupleFile=nullptr;
  OutputFileName=filename;
  EventNumber=0;
  output_ntuple = true;

  /// default limits 
  EMOP_lowerlimit = 0.0;
  EMOP_higherlimit = 100.0;
  HOP_lowerlimit = 0.0;
  HinOEM_higherlimit = 100.0;
  Pt_lowerlimit = 0.0;
  Pt_lowerlimit = 100.0;
}

ElectronPid::~ElectronPid() 
{
}

int ElectronPid::Init(PHCompositeNode *topNode)
{

  if(output_ntuple) {

	OutputNtupleFile = new TFile(OutputFileName.c_str(),"RECREATE");
  	std::cout << "PairMaker::Init: output file " << OutputFileName.c_str() << " opened." << endl;

	ntpbeforecut = new TNtuple("ntpbeforecut","","p:pt:cemce3x3:hcaline3x3:hcaloute3x3:cemce3x3overp:hcale3x3overp:charge:pid");
        ntpcutEMOP = new TNtuple("ntpcutEMOP","","p:pt:cemce3x3:hcaline3x3:hcaloute3x3:charge:pid");
	ntpcutHOP = new TNtuple("ntpcutHOP","","p:pt:cemce3x3:hcaline3x3:hcaloute3x3:charge:pid");
	ntpcutEMOP_HinOEM = new TNtuple("ntpcutEMOP_HinOEM","","p:pt:cemce3x3:hcaline3x3:hcaloute3x3:charge:pid");
	ntpcutEMOP_HinOEM_Pt = new TNtuple("ntpcutEMOP_HinOEM_Pt","","p:pt:cemce3x3:hcaline3x3:hcaloute3x3:charge:pid");
  }
  else {
	PHNodeIterator iter(topNode);
	PHCompositeNode *dstNode = dynamic_cast<PHCompositeNode *>(iter.findFirst("PHCompositeNode", "DST"));
	if (!dstNode)
	{
  	  cerr << PHWHERE << " ERROR: Can not find DST node." << endl;
    	  return Fun4AllReturnCodes::ABORTEVENT;
	}
  }
  
  return Fun4AllReturnCodes::EVENT_OK;
  
}

int ElectronPid::InitRun(PHCompositeNode* topNode)
{
  int ret = GetNodes(topNode);
  if (ret != Fun4AllReturnCodes::EVENT_OK) return ret;

  return Fun4AllReturnCodes::EVENT_OK;
}

int ElectronPid::process_event(PHCompositeNode* topNode)
{
  EventNumber++;
  float ntp[30];

  cout<<"EventNumber ===================== " << EventNumber-1 << endl;
  if(EventNumber==1) topNode->print();

  // get the tracks
  for(SvtxTrackMap::Iter it = _track_map->begin(); it != _track_map->end(); ++it)
    {

      SvtxTrack *track = it->second;

      double px = track->get_px();
      double py = track->get_py();

      double mom = track->get_p();
      double pt = sqrt(px*px + py*py);
      int charge = track->get_charge();
      int pid = it->first;

      double e_cemc = track->get_cal_energy_3x3(SvtxTrack::CAL_LAYER::CEMC);
      double e_hcal_in = track->get_cal_energy_3x3(SvtxTrack::CAL_LAYER::HCALIN);
      double e_hcal_out = track->get_cal_energy_3x3(SvtxTrack::CAL_LAYER::HCALOUT);

      // CEMC E/p cut
      double cemceoverp = e_cemc / mom;
      // HCaline/CEMCe cut
      double hcalineovercemce = e_hcal_in / e_cemc;
      // HCal E/p cut
      double hcaleoverp = (e_hcal_in + e_hcal_out) / mom;

      ntp[0] = mom;
      ntp[1] = pt;
      ntp[2] = e_cemc;
      ntp[3] = e_hcal_in;
      ntp[4] = e_hcal_out;
      ntp[5] = cemceoverp;
      ntp[6] = hcaleoverp;
      ntp[7] = charge;
      ntp[8] = pid;
      if(output_ntuple) { ntpbeforecut -> Fill(ntp); }

      /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////electrons
      if(cemceoverp > EMOP_lowerlimit && cemceoverp < EMOP_higherlimit)
	{
	
	  ntp[0] = mom;
    	  ntp[1] = pt;
    	  ntp[2] = e_cemc;
    	  ntp[3] = e_hcal_in;
    	  ntp[4] = e_hcal_out;
    	  ntp[5] = charge;
    	  ntp[6] = pid;
	  ntp[7] = cemceoverp;
    	  ntp[8] = hcaleoverp;
  	  if(output_ntuple) { ntpcutEMOP -> Fill(ntp); }

	  if(hcalineovercemce < HinOEM_higherlimit)
	     {

		  ntp[0] = mom;
    		  ntp[1] = pt;
    		  ntp[2] = e_cemc;
    		  ntp[3] = e_hcal_in;
    		  ntp[4] = e_hcal_out;
    		  ntp[5] = charge;
    		  ntp[6] = pid;
  		  if(output_ntuple) { ntpcutEMOP_HinOEM -> Fill(ntp); }

		  if( pt > Pt_lowerlimit && pt < Pt_higherlimit)
	  	    {

			  ntp[0] = mom;
    			  ntp[1] = pt;
    			  ntp[2] = e_cemc;
    			  ntp[3] = e_hcal_in;
    			  ntp[4] = e_hcal_out;
    			  ntp[5] = charge;
    			  ntp[6] = pid;
  			  if(output_ntuple) { ntpcutEMOP_HinOEM -> Fill(ntp); }
   	 	
	 		  if(Verbosity() > 0) {
			   	std::cout << " Pt_lowerlimit " << Pt_lowerlimit << " Pt_higherlimit " << Pt_higherlimit <<std::endl;
	 	 	   	//std::cout << " Track " << it->first  << " identified as electron " << "    mom " << mom << " e_cemc " << e_cemc  << " cemceoverp " << cemceoverp << " e_hcal_in " << e_hcal_in << " e_hcal_out " << e_hcal_out << std::endl; 
			   }
		  
			  // add to the association map
	 		  _track_pid_assoc->addAssoc(TrackPidAssoc::electron, it->second->get_id());
		   }
   	    }
	}
      
      /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////hadrons
     
      if(hcaleoverp > HOP_lowerlimit)// hcaleoverp>0.5
	{
      
	  ntp[0] = mom;
    	  ntp[1] = pt;
    	  ntp[2] = e_cemc;
    	  ntp[3] = e_hcal_in;
    	  ntp[4] = e_hcal_out;
    	  ntp[5] = charge;
    	  ntp[6] = pid;
    	  
  	  if(output_ntuple) { ntpcutHOP -> Fill(ntp); }

	  if(Verbosity() > 0) {
		std::cout << " HOP_lowerlimit " << HOP_lowerlimit <<std::endl;
		//std::cout << " Track " << it->first  << " identified as hadron " << "    mom " << mom << " e_cemc " << e_cemc  << " hcaleoverp " << hcaleoverp << " e_hcal_in " << e_hcal_in << " e_hcal_out " << e_hcal_out << std::endl; 
	    }

	  // add to the association map
	  _track_pid_assoc->addAssoc(TrackPidAssoc::hadron, it->second->get_id());
     	}
      

    }
  
  // Read back the association map

  if(Verbosity() > 0)
    std::cout << "Read back the association map electron entries" << std::endl;
  auto electrons = _track_pid_assoc->getTracks(TrackPidAssoc::electron);
  for(auto it = electrons.first; it != electrons.second; ++it)
    {
      SvtxTrack *tr = _track_map->get(it->second);
      double p = tr->get_p();

      if(Verbosity() > 1)
	std::cout << " pid " << it->first << " track ID " << it->second << " mom " << p << std::endl;
    }

  if(Verbosity() > 0)
    std::cout << "Read back the association map hadron entries" << std::endl;
  auto hadrons = _track_pid_assoc->getTracks(TrackPidAssoc::hadron);
  for(auto it = hadrons.first; it != hadrons.second; ++it)
    {
      SvtxTrack *tr = _track_map->get(it->second);
      double p = tr->get_p();

      if(Verbosity() > 1)
	std::cout << " pid " << it->first << " track ID " << it->second << " mom " << p << std::endl;
    }

  
  return Fun4AllReturnCodes::EVENT_OK;
}


int ElectronPid::GetNodes(PHCompositeNode* topNode)
{
  _track_map = findNode::getClass<SvtxTrackMap>(topNode, "SvtxTrackMap");

  // if the TrackPidAssoc node is not present, create it
  _track_pid_assoc =  findNode::getClass<TrackPidAssoc>(topNode, "TrackPidAssoc");
  if(!_track_pid_assoc)
    {
      PHNodeIterator iter(topNode);      
      PHCompositeNode* dstNode = static_cast<PHCompositeNode*>(iter.findFirst(
									      "PHCompositeNode", "DST"));
      if (!dstNode)
	{
	  cerr << PHWHERE << "DST Node missing, quit!" << endl;
	  return Fun4AllReturnCodes::ABORTEVENT;
	}

     // Get the SVTX node
      PHNodeIterator iter_dst(dstNode);
      PHCompositeNode* tb_node =
	dynamic_cast<PHCompositeNode*>(iter_dst.findFirst("PHCompositeNode",
							  "SVTX"));
      if (!tb_node)
	{
	  cout << PHWHERE << "SVTX node missing, quit!" << endl;
	  return Fun4AllReturnCodes::ABORTEVENT;
	}

      // now add the new node  
      _track_pid_assoc = new TrackPidAssoc;
      PHIODataNode<PHObject>* assoc_node = new PHIODataNode<PHObject>(
								      _track_pid_assoc, "TrackPidAssoc", "PHObject");
      tb_node->addNode(assoc_node);
      if (Verbosity() > 0)
	cout << PHWHERE << "Svtx/TrackPidAssoc node added" << endl;
    }
  
  return Fun4AllReturnCodes::EVENT_OK;
}

int ElectronPid::End(PHCompositeNode * /*topNode*/)
{
if(output_ntuple) {
  OutputNtupleFile -> cd();
  OutputNtupleFile -> Write();
  OutputNtupleFile -> Close();
}

  cout << "************END************" << endl;
  return Fun4AllReturnCodes::EVENT_OK;
}


/*
void ElectronPid::initializeTrees()
{
  PID_tracktree = new TTree("tracktree", "A tree with svtx tracks for particle identification");
  
  PID_tracktree->Branch("PID_tr_p", &PID_tr_p, "PID_tr_p/D");
  PID_tracktree->Branch("PID_tr_pt", &PID_tr_pt, "PID_tr_pt/D");
  
  PID_tracktree->Branch("PID_cemcdphi", &PID_cemcdphi, "PID_cemcdphi/D");
  PID_tracktree->Branch("PID_cemcdeta", &PID_cemcdeta, "PID_cemcdeta/D");
  PID_tracktree->Branch("PID_cemce3x3", &PID_cemce3x3, "PID_cemce3x3/D");
   
  PID_tracktree->Branch("PID_hcalindphi", &PID_hcalindphi, "PID_hcalindphi/D");
  PID_tracktree->Branch("PID_hcalindeta", &PID_hcalindeta, "PID_hcalindeta/D");
  PID_tracktree->Branch("PID_hcaline3x3", &PID_hcaline3x3, "PID_hcaline3x3/D");

  PID_tracktree->Branch("PID_hcaloute3x3", &PID_hcaloute3x3, "PID_hcaloute3x3/D");

  PID_tracktree->Branch("PID_EcemcOP", &PID_EcemcOP, "PID_EcemcOP/D");
  PID_tracktree->Branch("PID_EcemcOP_cut", &PID_EcemcOP_cut, "PID_EcemcOP_cut/D");
    
  PID_tracktree->Branch("PID_EhcalOP", &PID_EhcalOP, "PID_EhcalOP/D");
  PID_tracktree->Branch("PID_EhcalOP_cut", &PID_EhcalOP_cut, "PID_EhcalOP_cut/D");

}

void ElectronPid::initializeVariables()
{
  PID_tr_p = -99;
  PID_tr_pt = -99;

  PID_cemcdphi = -99;
  PID_cemcdeta = -99;
  PID_cemce3x3 = -99;

  PID_hcalindphi = -99;
  PID_hcalindeta = -99;
  PID_hcaline3x3 = -99;

  PID_hcaloute3x3 = -99;
  
  PID_EcemcOP = -99;
  PID_EcemcOP_cut = -99;

  PID_EhcalOP = -99;
  PID_EhcalOP_cut = -99;
}
*/


