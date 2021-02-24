#include "ElectronPid.h"

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

ElectronPid::ElectronPid(const std::string& name)
  : SubsysReco(name)
{
}

ElectronPid::~ElectronPid() 
{
}

int ElectronPid::InitRun(PHCompositeNode* topNode)
{
  int ret = GetNodes(topNode);
  if (ret != Fun4AllReturnCodes::EVENT_OK) return ret;

  return Fun4AllReturnCodes::EVENT_OK;
}

int ElectronPid::process_event(PHCompositeNode* topNode)
{
  // get the tracks
  for(SvtxTrackMap::Iter it = _track_map->begin(); it != _track_map->end(); ++it)
    {

      SvtxTrack *track = it->second;
      //double tr_px = track->get_px();
     // double tr_py = track->get_py();
     // double tr_pz = track->get_pz();
      double mom = track->get_p();
     // double tr_pt = sqrt(tr_px * tr_px + tr_py * tr_py);
      double e_cemc = track->get_cal_energy_3x3(SvtxTrack::CAL_LAYER::CEMC);
      double e_hcal_in = track->get_cal_energy_3x3(SvtxTrack::CAL_LAYER::HCALIN);
      double e_hcal_out = track->get_cal_energy_3x3(SvtxTrack::CAL_LAYER::HCALOUT);

	// CEMC E/p cut
      double eoverp = e_cemc / mom;
/*
      PID_tr_p = mom;
      PID_tr_pt = tr_pt;
      PID_cemcdphi = track->get_cal_dphi(SvtxTrack::CEMC);
      PID_cemcdeta = track->get_cal_deta(SvtxTrack::CEMC); 
      PID_cemce3x3 = e_cemc;
      
      PID_hcalindphi = track->get_cal_dphi(SvtxTrack::HCALIN);
      PID_hcalindeta = track->get_cal_deta(SvtxTrack::HCALIN);
      PID_hcaline3x3 = e_hcal_in;
      
      PID_hcaloute3x3 = e_hcal_out;

      PID_EcemcOP = PID_cemce3x3 / PID_tr_p;
*/
      if(eoverp > EOP_lowerlimit && eoverp < EOP_higherlimit)// 0.7<eoverp<1.5
	{
	 // PID_EcemcOP_cut = PID_cemce3x3 / PID_tr_p;

	  if(Verbosity() > 0)
	    std::cout << " Track " << it->first  << " identified as electron " << "    mom " << mom << " e_cemc " << e_cemc  << " eoverp " << eoverp 
		      << " e_hcal_in " << e_hcal_in << " e_hcal_out " << e_hcal_out << std::endl;
	  // add to the association map
	  _track_pid_assoc->addAssoc(TrackPidAssoc::electron, it->second->get_id());
	}
      
      // HCal E/p cut
      eoverp = (e_hcal_in + e_hcal_out) / mom;
     
     // PID_EhcalOP = (PID_hcaline3x3 + PID_hcaloute3x3) / PID_tr_p;

      if(eoverp > HOP_higherlimit)// eoverp>0.5
	{
        //  PID_EhcalOP_cut = (PID_hcaline3x3 + PID_hcaloute3x3) / PID_tr_p;

	  if(Verbosity() > 0)
	    std::cout << " Track " << it->first  << " identified as hadron " << "    mom " << mom << " e_cemc " << e_cemc  << " eoverp " << eoverp 
		      << " e_hcal_in " << e_hcal_in << " e_hcal_out " << e_hcal_out << std::endl;

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

      if(Verbosity() > 0)
	std::cout << " pid " << it->first << " track ID " << it->second << " mom " << p << std::endl;
    }

  if(Verbosity() > 0)
    std::cout << "Read back the association map hadron entries" << std::endl;
  auto hadrons = _track_pid_assoc->getTracks(TrackPidAssoc::hadron);
  for(auto it = hadrons.first; it != hadrons.second; ++it)
    {
      SvtxTrack *tr = _track_map->get(it->second);
      double p = tr->get_p();

      if(Verbosity() > 0)
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


