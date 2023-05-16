#include "ElectronID.h"

#include <cstdlib>
#include <cstdio>
#include <iomanip>
#include <fstream>

#include <phool/PHCompositeNode.h>
#include <phool/recoConsts.h>
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
#include <trackbase_historic/SvtxVertex.h>
#include <trackbase_historic/SvtxVertexMap.h>
#include <trackbase/TrkrDefs.h>

#include <calobase/RawClusterContainer.h>
#include <calobase/RawCluster.h>
#include <calobase/RawClusterv1.h>

#include <globalvertex/GlobalVertexMap.h>
#include <globalvertex/GlobalVertex.h>

#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4VtxPoint.h>

#include "trackpidassoc/TrackPidAssoc.h"

//TMVA class
#include <vector>
#include <iostream>
#include <map>
#include <string>
#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#include "TMVA/MethodCuts.h"
#include "TMVA/Factory.h"
#include "TMVA/DataLoader.h"


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

ElectronID::ElectronID(const std::string& name, const std::string &filename) : SubsysReco(name)
{
  OutputNtupleFile=nullptr;
  OutputFileName=filename;
  EventNumber=0;
  output_ntuple = true;

  /// default limits 
  EMOP_lowerlimit = 0.7;
  EMOP_higherlimit = 100.0;
  HOP_lowerlimit = 0.0;
  HinOEM_higherlimit = 100.0;
  Pt_lowerlimit = 0.0;
  Pt_higherlimit = 100.0;
  Nmvtx_lowerlimit = 2;
  Nintt_lowerlimit = 0;
  Ntpc_lowerlimit = 0;
  Nquality_higherlimit = 100;
  Ntpc_lowerlimit = 20;
  Nquality_higherlimit = 5.;
  PROB_cut = 0.;

  /// MVA
  BDT_cut_p = 0.0;
  BDT_cut_n = 0.0;
  ISUSE_BDT_p =0;
  ISUSE_BDT_n =0;

 // unsigned int _nlayers_maps = 3;
 // unsigned int _nlayers_intt = 4;
 // unsigned int _nlayers_tpc = 48;
}

ElectronID::~ElectronID() 
{
}

int ElectronID::Init(PHCompositeNode *topNode)
{

  if(output_ntuple) {

	OutputNtupleFile = new TFile(OutputFileName.c_str(),"RECREATE");
  	std::cout << "PairMaker::Init: output file " << OutputFileName.c_str() << " opened." << endl;
        ntpBDTresponse = new TNtuple("ntpbeforecut","","select_p:select_n");

	ntpbeforecut = new TNtuple("ntpbeforecut","","p:pt:cemce3x3:hcaline3x3:hcaloute3x3:cemce3x3overp:hcaline3x3overcemce3x3:hcale3x3overp:charge:pid:quality:e_cluster:EventNumber:z:vtxid:nmvtx:nintt:ntpc:cemc_prob:cemc_ecore:cemc_chi2");
        ntpcutEMOP = new TNtuple("ntpcutEMOP","","p:pt:cemce3x3:hcaline3x3:hcaloute3x3:charge:pid:quality:e_cluster:EventNumber:z:vtxid:cemc_prob:cemc_ecore");
	ntpcutHOP = new TNtuple("ntpcutHOP","","p:pt:cemce3x3:hcaline3x3:hcaloute3x3:charge:pid:quality:e_cluster:EventNumber:z:vtxid:cemc_prob:cemc_ecore");
	ntpcutEMOP_HinOEM = new TNtuple("ntpcutEMOP_HinOEM","","p:pt:cemce3x3:hcaline3x3:hcaloute3x3:charge:pid:quality:e_cluster:EventNumber:z:vtxid:cemc_prob:cemc_ecore");
	ntpcutEMOP_HinOEM_Pt = new TNtuple("ntpcutEMOP_HinOEM_Pt","","p:pt:cemce3x3:hcaline3x3:hcaloute3x3:charge:pid:quality:e_cluster:EventNumber:z:vtxid:cemc_prob:cemc_ecore");
        ntpcutEMOP_HinOEM_Pt_read = new TNtuple("ntpcutEMOP_HinOEM_Pt_read","","p:pt:cemce3x3:hcaline3x3:hcaloute3x3:charge:pid:quality:e_cluster:EventNumber:z:vtxid:trackid:cemc_prob:cemc_ecore");
        ntpcutBDT_read = new TNtuple("ntpcutBDT_read","","p:pt:cemce3x3:hcaline3x3:hcaloute3x3:charge:pid:quality:e_cluster:EventNumber:z:vtxid:trackid:cemc_prob:cemc_ecore:EOP:HOM:cemc_chi2");
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

int ElectronID::InitRun(PHCompositeNode* topNode)
{
  int ret = GetNodes(topNode);
  if (ret != Fun4AllReturnCodes::EVENT_OK) return ret;

  return Fun4AllReturnCodes::EVENT_OK;
}

int ElectronID::process_event(PHCompositeNode* topNode)
{
  EventNumber++;
  float ntp[30];

  SvtxTrackMap *trackmap = findNode::getClass<SvtxTrackMap>(topNode, "SvtxTrackMap");
  if(!trackmap) {
    cerr << PHWHERE << " ERROR: Can not find SvtxTrackMap node." << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  cout<<"EventNumber ===================== " << EventNumber-1 << endl;
  if(EventNumber==1) topNode->print();

  SvtxVertexMap *vtxmap = findNode::getClass<SvtxVertexMap>(topNode, "SvtxVertexMap");
  if(!vtxmap) {
      cout << "SvtxVertexMap node not found!" << endl;
      return Fun4AllReturnCodes::ABORTEVENT;
  }
  //cout << "Number of SvtxVertexMap entries = " << vtxmap->size() << endl;

  RawClusterContainer* cemc_cluster_container = findNode::getClass<RawClusterContainer>(topNode, "CLUSTER_CEMC");
  if(!cemc_cluster_container) {
    cerr << PHWHERE << " ERROR: Can not find CLUSTER_CEMC node." << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }


// Truth info
  PHG4TruthInfoContainer* truth_container = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
  if(!truth_container) {
    cerr << PHWHERE << " ERROR: Can not find G4TruthInfo node." << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }
  PHG4TruthInfoContainer::ConstRange range = truth_container->GetPrimaryParticleRange();
  cout << "number of MC particles = " << truth_container->size() << " " << truth_container->GetNumPrimaryVertexParticles() << endl;

    int mycount = 0;
    for (PHG4TruthInfoContainer::ConstIterator iter = range.first; iter != range.second; ++iter)
    {
      PHG4Particle* g4particle = iter->second;
      mycount++;
      int gflavor  = g4particle->get_pid();
      double gpx = g4particle->get_px();
      double gpy = g4particle->get_py();
      double gpz= g4particle->get_pz();
      double gpt = sqrt(gpx*gpx+gpy*gpy);
      double phi = atan2(gpy,gpx);
      double eta = asinh(gpz/gpt);
      int primid =  g4particle->get_primary_id();
      int parentid = g4particle->get_parent_id();
      int trackid = g4particle->get_track_id();
      if(trackid>truth_container->GetNumPrimaryVertexParticles()-50) cout << trackid << " " << parentid << " " << primid << " " << gflavor << " " << gpt << " " << phi << " " << eta << endl;
    }
    cout << "mycount = " << mycount << endl;
// end Truth


//MVA method setup**********************

  // This loads the library
   TMVA::Tools::Instance();

   // Default MVA methods with trained + tested
   std::map<std::string,int> Use_p;
   std::map<std::string,int> Use_n;
   // Boosted Decision Trees
   Use_p["BDT"] = ISUSE_BDT_p; // uses Adaptive Boost; for positive particles
   Use_n["BDT"] = ISUSE_BDT_n; // uses Adaptive Boost; for negative particles

  // Create the Reader object

   TMVA::Reader *reader_positive = new TMVA::Reader( "!Color:!Silent" );
   TMVA::Reader *reader_negative = new TMVA::Reader( "!Color:!Silent" );

   // Create a set of variables and declare them to the reader
   // - the variable names MUST corresponds in name and type to those given in the weight file(s) used
   Float_t var1_p, var2_p, var3_p;
   Float_t var1_n, var2_n, var3_n;
   reader_positive->AddVariable( "var1_p", &var1_p );
   reader_positive->AddVariable( "var2_p", &var2_p );
   reader_positive->AddVariable( "var3_p", &var3_p );

   reader_negative->AddVariable( "var1_n", &var1_n );
   reader_negative->AddVariable( "var2_n", &var2_n );
   reader_negative->AddVariable( "var3_n", &var3_n );

  // Book the MVA methods
   TString dir_p, dir_n;
   dir_p = "dataset/Weights_positive/"; // weights for positive particles
   dir_n = "dataset/Weights_negative/"; // weights for negative particles
   TString prefix = "TMVAClassification";

   // Book method(s)
   for (std::map<std::string,int>::iterator it = Use_p.begin(); it != Use_p.end(); it++) {
      if (it->second) {
         TString methodName_p = TString(it->first) + TString(" method");
         TString weightfile_p = dir_p + prefix + TString("_") + TString(it->first) + TString(".weights.xml");
         reader_positive->BookMVA( methodName_p, weightfile_p );
      }
   }
   
   for (std::map<std::string,int>::iterator it = Use_n.begin(); it != Use_n.end(); it++) {
      if (it->second) {
         TString methodName_n = TString(it->first) + TString(" method");
         TString weightfile_n = dir_n + prefix + TString("_") + TString(it->first) + TString(".weights.xml");
         reader_negative->BookMVA( methodName_n, weightfile_n );
      }
   }

//end of MVA method setup*******************

  int nmvtx = 0;
  int nintt = 0;
  int ntpc = 0;
  cout << _nlayers_maps << " " << _nlayers_intt << " " << _nlayers_tpc<<endl;
  // get the tracks
  for(SvtxTrackMap::Iter it = _track_map->begin(); it != _track_map->end(); ++it)
    {
      SvtxTrack *track = it->second;

      PHG4Particle* g4particle = findMCmatch(track, truth_container);
      int gflavor = g4particle->get_pid();
      if(gflavor==0) continue;

      nmvtx = 0;
      nintt = 0;
      ntpc = 0;

      for (SvtxTrack::ConstClusterKeyIter iter = track->begin_cluster_keys(); iter != track->end_cluster_keys(); ++iter)
      {
        TrkrDefs::cluskey cluser_key = *iter;
        int trackerid = TrkrDefs::getTrkrId(cluser_key);
      //  cout << "trackerid= " << trackerid << endl; 
        if(trackerid==0) nmvtx++;
        if(trackerid==1) nintt++;
        if(trackerid==2) ntpc++;
        //unsigned int layer = TrkrDefs::getLayer(cluser_key);
        //if (_nlayers_maps > 0 && layer < _nlayers_maps) nmvtx++;
        //if (_nlayers_intt > 0 && layer >= _nlayers_maps && layer < _nlayers_maps + _nlayers_intt) nintt++;
        //if (_nlayers_tpc > 0 && layer >= (_nlayers_maps + _nlayers_intt) && layer < (_nlayers_maps + _nlayers_intt + _nlayers_tpc)) ntpc++;
      }
 

      double px = track->get_px();
      double py = track->get_py();

      double z = track->get_z();

      unsigned int vtxid = track->get_vertex_id();

      double mom = track->get_p();
      double pt = sqrt(px*px + py*py);
      int charge = track->get_charge();
      int pid = it->first;
      float quality = track->get_quality();

      double e_cluster = track->get_cal_cluster_e(SvtxTrack::CAL_LAYER::CEMC);

      double e_cemc_3x3 = track->get_cal_energy_3x3(SvtxTrack::CAL_LAYER::CEMC);
      double e_hcal_in_3x3 = track->get_cal_energy_3x3(SvtxTrack::CAL_LAYER::HCALIN);
      double e_hcal_out_3x3 = track->get_cal_energy_3x3(SvtxTrack::CAL_LAYER::HCALOUT);

      unsigned int cemc_clusid = track->get_cal_cluster_id(SvtxTrack::CAL_LAYER::CEMC);
      double cemc_prob = 0.;
      double cemc_ecore = 0.;
      double cemc_chi2 = 99999.;
      if(cemc_clusid<99999) {
        RawCluster* cemc_cluster = cemc_cluster_container->getCluster(cemc_clusid);
        cemc_prob = cemc_cluster->get_prob();
        cemc_ecore = cemc_cluster->get_ecore();
        cemc_chi2 = cemc_cluster->get_chi2();
      }

      // CEMC E/p cut
      double cemceoverp = e_cemc_3x3 / mom;
      //double cemceoverp = cemc_ecore / mom;
      // HCaline/CEMCe cut
      double hcalineovercemce = e_hcal_in_3x3 / e_cemc_3x3;
      // HCal E/p cut
      double hcaleoverp = (e_hcal_in_3x3 + e_hcal_out_3x3) / mom;

    //MVA valuables
     if(cemceoverp>0.0 && cemceoverp<10.0 && hcalineovercemce>0.0 && hcalineovercemce<10.0){
         if(charge>0){
             var1_p = cemceoverp;
             var2_p = hcalineovercemce;
             var3_p = cemc_chi2;
          }
         if(charge<0){
             var1_n = cemceoverp;
             var2_n = hcalineovercemce;
             var3_n = cemc_chi2;
          }
      }
    
    if (Use_p["BDT"] && Use_n["BDT"] && quality < Nquality_higherlimit && nmvtx >= Nmvtx_lowerlimit && nintt >= Nintt_lowerlimit && ntpc >= Ntpc_lowerlimit && pt > Pt_lowerlimit && pt < Pt_higherlimit &&cemceoverp>0.0 && cemceoverp<10.0 && hcalineovercemce>0.0 && hcalineovercemce<10.0) {
          float select_p=reader_positive->EvaluateMVA("BDT method");
	  float select_n=reader_negative->EvaluateMVA("BDT method");
          ntp[0] = select_p;
          ntp[1] = select_n;
	  if(output_ntuple) { ntpBDTresponse -> Fill(ntp); }
          if(select_p>BDT_cut_p && select_n>BDT_cut_n){
              // add to the association map
	      _track_pid_assoc->addAssoc(TrackPidAssoc::electron, it->second->get_id());
          }

    }// end of BDT cut
    else{ // for traditional cuts

      ntp[0] = mom;
      ntp[1] = pt;
      ntp[2] = e_cemc_3x3;
      ntp[3] = e_hcal_in_3x3;
      ntp[4] = e_hcal_out_3x3;
      ntp[5] = cemceoverp;
      ntp[6] = hcalineovercemce;
      ntp[7] = hcaleoverp;
      ntp[8] = charge;
      ntp[9] = pid;
      ntp[10] = quality;
      ntp[11] = e_cluster;
      ntp[12] = EventNumber;
      ntp[13] = z;
      ntp[14] = vtxid;
      ntp[15] = nmvtx;
      ntp[16] = nintt;
      ntp[17] = ntpc;
      ntp[18] = cemc_prob;
      ntp[19] = cemc_ecore;
      ntp[20] = cemc_chi2;
      if(output_ntuple) { ntpbeforecut -> Fill(ntp); }

	//std::cout << " Pt_lowerlimit " << Pt_lowerlimit << " Pt_higherlimit " << Pt_higherlimit << " HOP_lowerlimit " << HOP_lowerlimit <<std::endl;
        //std::cout << " EMOP_lowerlimit " << EMOP_lowerlimit << " EMOP_higherlimit " << EMOP_higherlimit << " HinOEM_higherlimit " << HinOEM_higherlimit <<std::endl;

     ///////////////////////////////////////////////////////////////electrons
      if(cemceoverp > EMOP_lowerlimit && cemceoverp < EMOP_higherlimit && quality < Nquality_higherlimit && nmvtx >= Nmvtx_lowerlimit && nintt >= Nintt_lowerlimit && ntpc >= Ntpc_lowerlimit)
	{
	
	  ntp[0] = mom;
    	  ntp[1] = pt;
    	  ntp[2] = e_cemc_3x3;
    	  ntp[3] = e_hcal_in_3x3;
    	  ntp[4] = e_hcal_out_3x3;
    	  ntp[5] = charge;
    	  ntp[6] = pid;
	  ntp[7] = quality;
	  ntp[8] = e_cluster;
	  ntp[9] = EventNumber;
	  ntp[10] = z;
  	  ntp[11] = vtxid;
  	  ntp[12] = cemc_prob;
  	  ntp[13] = cemc_ecore;
  	  if(output_ntuple) { ntpcutEMOP -> Fill(ntp); }

	  if(hcalineovercemce < HinOEM_higherlimit)
	     {

		  ntp[0] = mom;
    		  ntp[1] = pt;
    		  ntp[2] = e_cemc_3x3;
    		  ntp[3] = e_hcal_in_3x3;
    		  ntp[4] = e_hcal_out_3x3;
    		  ntp[5] = charge;
    		  ntp[6] = pid;
		  ntp[7] = quality;
		  ntp[8] = e_cluster;
		  ntp[9] = EventNumber;
	  	  ntp[10] = z;
  		  ntp[11] = vtxid;
  		  ntp[12] = cemc_prob;
  		  ntp[13] = cemc_ecore;
  		  if(output_ntuple) { ntpcutEMOP_HinOEM -> Fill(ntp); }

		  if( pt > Pt_lowerlimit && pt < Pt_higherlimit)
	  	    {

			  ntp[0] = mom;
    			  ntp[1] = pt;
    			  ntp[2] = e_cemc_3x3;
    			  ntp[3] = e_hcal_in_3x3;
    			  ntp[4] = e_hcal_out_3x3;
    			  ntp[5] = charge;
    			  ntp[6] = pid;
			  ntp[7] = quality;
			  ntp[8] = e_cluster;
			  ntp[9] = EventNumber;
			  ntp[10] = z;
  			  ntp[11] = vtxid;
  			  ntp[12] = cemc_prob;
  			  ntp[13] = cemc_ecore;
  			  if(output_ntuple) { ntpcutEMOP_HinOEM_Pt -> Fill(ntp); }
   	 	
	 		  if(Verbosity() > 0) {
	 	 	   	std::cout << " Track " << it->first  << " identified as electron " << "    mom " << mom << " e_cemc_3x3 " << e_cemc_3x3  << " cemceoverp " << cemceoverp << " e_hcal_in_3x3 " << e_hcal_in_3x3 << " e_hcal_out_3x3 " << e_hcal_out_3x3 << std::endl; 
			   }
		  
			  // add to the association map
	 		  _track_pid_assoc->addAssoc(TrackPidAssoc::electron, it->second->get_id());
		   }
   	     }
	}
     }//end of BDT else
      
      /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////hadrons
     
     // if(hcaleoverp > HOP_lowerlimit && quality < 10)
      if(hcaleoverp > HOP_lowerlimit && quality < 5)
	{
      
	  ntp[0] = mom;
    	  ntp[1] = pt;
    	  ntp[2] = e_cemc_3x3;
    	  ntp[3] = e_hcal_in_3x3;
    	  ntp[4] = e_hcal_out_3x3;
    	  ntp[5] = charge;
    	  ntp[6] = pid;
	  ntp[7] = quality;
    	  ntp[8] = e_cluster;
	  ntp[9] = EventNumber;
	  ntp[10] = z;
  	  ntp[11] = vtxid;
  	  ntp[12] = cemc_prob;
  	  ntp[13] = cemc_ecore;
  	  if(output_ntuple) { ntpcutHOP -> Fill(ntp); }

	  if(Verbosity() > 0) {
		std::cout << " Track " << it->first  << " identified as hadron " << "    mom " << mom << " e_cemc_3x3 " << e_cemc_3x3  << " hcaleoverp " << hcaleoverp << " e_hcal_in_3x3 " << e_hcal_in_3x3 << " e_hcal_out_3x3 " << e_hcal_out_3x3 << std::endl; 
	    }

	  // add to the association map
	  _track_pid_assoc->addAssoc(TrackPidAssoc::hadron, it->second->get_id());
     	}
      

    }//end of evet loop.

  delete reader_positive;
  delete reader_negative;
  
  // Read back the association map
  if(Verbosity() > 1)
    std::cout << "Read back the association map electron entries" << std::endl;
  auto electrons = _track_pid_assoc->getTracks(TrackPidAssoc::electron);
  for(auto it = electrons.first; it != electrons.second; ++it)
    {
      SvtxTrack *tr = _track_map->get(it->second);

      double z = tr->get_z();
      unsigned int vtxid = tr->get_vertex_id();
      double mom = tr->get_p();
      double px = tr->get_px();
      double py = tr->get_py();
      double pt = sqrt(px*px + py*py);
      int charge = tr->get_charge();
      int pid = it->first;
      int tr_id = it->second;
      int quality = tr->get_quality();

      double e_cluster = tr->get_cal_cluster_e(SvtxTrack::CAL_LAYER::CEMC);

      double e_cemc_3x3 = tr->get_cal_energy_3x3(SvtxTrack::CAL_LAYER::CEMC);
      double e_hcal_in_3x3 = tr->get_cal_energy_3x3(SvtxTrack::CAL_LAYER::HCALIN);
      double e_hcal_out_3x3 = tr->get_cal_energy_3x3(SvtxTrack::CAL_LAYER::HCALOUT);

      double EOP = e_cemc_3x3/mom;
      double HOM = e_hcal_in_3x3/e_cemc_3x3;

 
      unsigned int cemc_clusid = tr->get_cal_cluster_id(SvtxTrack::CAL_LAYER::CEMC);
      double cemc_prob = 0.;
      double cemc_ecore = 0.;
      double cemc_chi2 = 99999.;
      if(cemc_clusid<99999) {
        RawCluster* cemc_cluster = cemc_cluster_container->getCluster(cemc_clusid);
        cemc_prob = cemc_cluster->get_prob();
        cemc_ecore = cemc_cluster->get_ecore();
        cemc_chi2 = cemc_cluster->get_chi2();
      }
     
      ntp[0] = mom;
      ntp[1] = pt;
      ntp[2] = e_cemc_3x3;
      ntp[3] = e_hcal_in_3x3;
      ntp[4] = e_hcal_out_3x3;
      ntp[5] = charge;
      ntp[6] = pid;
      ntp[7] = quality;
      ntp[8] = e_cluster;
      ntp[9] = EventNumber;
      ntp[10] = z;
      ntp[11] = vtxid;
      ntp[12] = tr_id;
      ntp[13] = cemc_prob;
      ntp[14] = cemc_ecore;
      ntp[15] = EOP;
      ntp[16] = HOM;
      ntp[17] = cemc_chi2;
      //if(output_ntuple) { ntpcutEMOP_HinOEM_Pt_read -> Fill(ntp); }
      if(output_ntuple) { ntpcutBDT_read -> Fill(ntp); }
      
      if(Verbosity() > 1)
	std::cout << " pid " << it->first << " track ID " << it->second << " mom " << mom << std::endl;
    }

  if(Verbosity() > 1)
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


int ElectronID::GetNodes(PHCompositeNode* topNode)
{
  _track_map = findNode::getClass<SvtxTrackMap>(topNode, "SvtxTrackMap");

  // if the TrackPidAssoc node is not present, create it
  _track_pid_assoc =  findNode::getClass<TrackPidAssoc>(topNode, "TrackPidAssoc");
  if(!_track_pid_assoc)
    {
      PHNodeIterator iter(topNode);      
      PHCompositeNode* dstNode = static_cast<PHCompositeNode*>(iter.findFirst("PHCompositeNode", "DST"));
      if (!dstNode)
	{
	  cerr << PHWHERE << "DST Node missing, quit!" << endl;
	  return Fun4AllReturnCodes::ABORTEVENT;
	}

     // Get the SVTX node
      PHNodeIterator iter_dst(dstNode);
      PHCompositeNode* tb_node =
	dynamic_cast<PHCompositeNode*>(iter_dst.findFirst("PHCompositeNode", "SVTX"));
      if (!tb_node)
	{
	  cout << PHWHERE << "SVTX node missing, quit!" << endl;
	  return Fun4AllReturnCodes::ABORTEVENT;
	}

      // now add the new node  
      _track_pid_assoc = new TrackPidAssoc;
      PHIODataNode<PHObject>* assoc_node = new PHIODataNode<PHObject>(_track_pid_assoc, "TrackPidAssoc", "PHObject");
      tb_node->addNode(assoc_node);
      if (Verbosity() > 0)
	cout << PHWHERE << "Svtx/TrackPidAssoc node added" << endl;
    }
  
  return Fun4AllReturnCodes::EVENT_OK;
}


PHG4Particle* ElectronID::findMCmatch(SvtxTrack* track, PHG4TruthInfoContainer* truth_container)
{
  double px = track->get_px();
  double py = track->get_py();
  double pz = track->get_pz();
  double pt = sqrt(px*px+py*py);
  double phi = atan2(py,px);
  double eta = asinh(pz/pt);
  PHG4TruthInfoContainer::ConstRange range = truth_container->GetPrimaryParticleRange();
//  cout << "number of MC particles = " << truth_container->size() << " " << truth_container->GetNumPrimaryVertexParticles() << endl;

  double thedistance = 9999.;
  PHG4Particle* matchedMC = NULL;

    for (PHG4TruthInfoContainer::ConstIterator iter = range.first; iter != range.second; ++iter)
    {
      PHG4Particle* g4particle = iter->second;
      int trackid = g4particle->get_track_id();
      if(trackid<=truth_container->GetNumPrimaryVertexParticles()-50) continue; // only embedded particles
      //int gflavor  = g4particle->get_pid();
      double gpx = g4particle->get_px();
      double gpy = g4particle->get_py();
      double gpz= g4particle->get_pz();
      double gpt = sqrt(gpx*gpx+gpy*gpy);
      double gphi = atan2(gpy,gpx);
      double geta = asinh(gpz/gpt);
      //int primid =  g4particle->get_primary_id();
      //int parentid = g4particle->get_parent_id();
      if(sqrt(pow(gphi-phi,2)+pow(geta-eta,2))<thedistance) {
        thedistance = sqrt(pow(gphi-phi,2)+pow(geta-eta,2));
        matchedMC = g4particle;
      }     
    }

  return matchedMC;
}


int ElectronID::End(PHCompositeNode * /*topNode*/)
{
if(output_ntuple) {
  OutputNtupleFile -> cd();
  OutputNtupleFile -> Write();
  OutputNtupleFile -> Close();
}

  cout << "************END************" << endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

