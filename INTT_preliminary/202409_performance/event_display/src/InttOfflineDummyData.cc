#include "InttOfflineDummyData.h"

/// Cluster/Calorimeter includes

/// Fun4All includes
#include <fun4all/Fun4AllHistoManager.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include <trackbase/InttDefs.h>
#include <trackbase/TrkrHitv2.h>
#include <trackbase/TrkrHitSet.h>
#include <trackbase/TrkrHitSetContainerv1.h>

/// ROOT includes
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TNtuple.h>
#include <TTree.h>

/// C++ includes
#include <cassert>
#include <cmath>
#include <sstream>
#include <string>

#include <intt/InttMapping.h>

#include "InttEvent.h"

using namespace std;

std::vector<InttNameSpace::Offline_s> setDummyInttEvent(const int ievent);

/**
 * This class demonstrates the construction and use of an analysis module
 * within the sPHENIX Fun4All framework. It is intended to show how to
 * obtain physics objects from the analysis tree, and then write them out
 * to a ROOT tree and file for further offline analysis.
 */

/**
 * Constructor of module
 */
InttOfflineDummyData::InttOfflineDummyData(const std::string &name, const std::string &filename)
  : SubsysReco(name)
  , inttEvt_(nullptr)
  , ievent_(0)
{
  fname_ = filename;
}

/**
 * Destructor of module
 */
InttOfflineDummyData::~InttOfflineDummyData()
{
  if(inttEvt_!=nullptr) delete inttEvt_;
}

/**
 * Initialize the module and prepare looping over events
 */
int InttOfflineDummyData::Init(PHCompositeNode * /*topNode*/)
{
  if (Verbosity() > 5)
  {
    std::cout << "Beginning Init in InttOfflineDummyData" << std::endl;
  }

  return 0;
}

int InttOfflineDummyData::InitRun(PHCompositeNode * topNode)
{
  if (Verbosity() > 5)
  {
    std::cout << "Beginning InitRun in InttOfflineDummyData" << std::endl;
  }

  if(!topNode)
  {
	  std::cout << "InttOfflineDummyData::InitRun(PHCompositeNode* topNode)" << std::endl;
	  std::cout << "\tCould not retrieve topNode; doing nothing" << std::endl;

	  return -1;
  }

  PHNodeIterator dst_itr(topNode);
  PHCompositeNode* dst_node = dynamic_cast<PHCompositeNode*>(dst_itr.findFirst("PHCompositeNode", "DST"));
  if(!dst_node)
  {
	  if(Verbosity())std::cout << "InttOfflineDummyData::InitRun(PHCompositeNode* topNode)" << std::endl;
	  if(Verbosity())std::cout << "\tCould not retrieve dst_node; doing nothing" << std::endl;

	  return -1;
  }

  PHNodeIterator trkr_itr(dst_node);
  PHCompositeNode* trkr_node = dynamic_cast<PHCompositeNode*>(trkr_itr.findFirst("PHCompositeNode", "TRKR"));
  if(!trkr_node)
  {
	  trkr_node = new PHCompositeNode("TRKR");
	  dst_node->addNode(trkr_node);
  }

  TrkrHitSetContainer* trkr_hit_set_container = findNode::getClass<TrkrHitSetContainer>(topNode, "TRKR_HITSET");
  if(!trkr_hit_set_container)
  {
	  if(Verbosity())std::cout << "InttOfflineDummyData::InitRun(PHCompositeNode* topNode)" << std::endl;
	  if(Verbosity())std::cout << "\tMaking TrkrHitSetContainer" << std::endl;

	  trkr_hit_set_container = new TrkrHitSetContainerv1;
	  PHIODataNode<PHObject>* new_node = new PHIODataNode<PHObject>(trkr_hit_set_container, "TRKR_HITSET", "PHObject");
	  trkr_node->addNode(new_node);
  }


  inttEvt_ = new InttEvent();
  ievent_ = 0;

  return Fun4AllReturnCodes::EVENT_OK;
}

/**
 * Main workhorse function where each event is looped over and
 * data from each event is collected from the node tree for analysis
 */
int InttOfflineDummyData::process_event(PHCompositeNode *topNode)
{
	cout<<"InttOfflineDummyData process event"<<endl;
	if (Verbosity() > 5)
	{
		std::cout << "Beginning process_event in AnaTutorial" << std::endl;
	}

	TrkrHitSetContainer* trkr_hit_set_container = findNode::getClass<TrkrHitSetContainer>(topNode, "TRKR_HITSET");
	if(!trkr_hit_set_container)
	{
		std::cout << PHWHERE << std::endl;
		std::cout << "InttOfflineDummyDataDecoder::process_event(PHCompositeNode* topNode)" << std::endl;
		std::cout << "Could not get \"TRKR_HITSET\" from Node Tree" << std::endl;
		std::cout << "Exiting" << std::endl;
		exit(1);

		return Fun4AllReturnCodes::DISCARDEVENT;
	}


	/////////////////////
	//if(ievent_>=1000){
	//	cout<<"all events are already processed. quit InttOfflineDummyData"<<endl;
	//	return Fun4AllReturnCodes::EVENT_OK;
	//}



	vector<InttNameSpace::Offline_s> vdata = setDummyInttEvent(ievent_);

	int adc = 0;
	//int amp = 0;
	int bco = 0;

	TrkrDefs::hitsetkey hit_set_key = 0;
	TrkrDefs::hitkey hit_key = 0;
	TrkrHitSetContainer::Iterator hit_set_container_itr;
	TrkrHit* hit = nullptr;

	int nskip=0;
	for(uint n = 0; n < vdata.size(); ++n)
	{

		adc = 0;
		bco = 0; // always 0 until time-in issue fixed; //-- rawhit->bco;

		struct InttNameSpace::Offline_s offline = vdata[n];

                //--//------------------
                //--// change the ladder id by hand
                //--int nladder = (offline.layer<5) ? 12 : 16;
                //--int offset  = 2;
                //--if(     offline.layer==4) offset=3;
                //--else if(offline.layer==5) offset=3;
                //--else if(offline.layer==6) offset=4;
                //--offline.ladder_phi -= offset; 
                //--if(offline.ladder_phi<0) offline.ladder_phi += nladder;
                //--//------------------

		hit_set_key = InttDefs::genHitSetKey(offline.layer,  offline.ladder_z, offline.ladder_phi, bco); 
		hit_set_container_itr = trkr_hit_set_container->findOrAddHitSet(hit_set_key);
                //cout<<"offline layer : "<<offline.layer<<endl;

		hit_key = InttDefs::genHitKey(offline.strip_y, offline.strip_x);
		hit     = hit_set_container_itr->second->getHit(hit_key);
		if(hit) {
			nskip++;
			continue;
		}

		hit = new TrkrHitv2;
		hit->setAdc(adc);
		hit_set_container_itr->second->addHitSpecificKey(hit_key, hit);
	}
	cout<<"Nskip of copyhit : "<<nskip<<endl;


	ievent_++;
	return Fun4AllReturnCodes::EVENT_OK;
}

/**
 * End the module and finish any data collection. Clean up any remaining
 * loose ends
 */
int InttOfflineDummyData::End(PHCompositeNode * /*topNode*/)
{
  if (Verbosity() > 1)
  {
    std::cout << "Ending InttOfflineDummyData analysis package" << std::endl;
  }


  return 0;
}


//		hit_set_key = InttDefs::genHitSetKey(offline.layer,  offline.ladder_z, offline.ladder_phi, bco); 
//		hit_key = InttDefs::genHitKey(offline.strip_y, offline.strip_x);
void SetOfflineHit(InttNameSpace::Offline_s* hit, int layer, int ladder_z, int ladder_phi, int strip_y, int strip_x) 
{
  hit->layer      = layer;
  hit->ladder_z   = ladder_z;
  hit->ladder_phi = ladder_phi;
  hit->strip_y    = strip_y;
  hit->strip_x    = strip_x;

}

std::vector<InttNameSpace::Offline_s> setDummyInttEvent(const int ievent)
{
  vector<InttNameSpace::Offline_s> vdata;

  // ievent:0-7 for felix check
  if(ievent==0){
	   InttNameSpace::Offline_s hit0[10];
	   SetOfflineHit(&hit0[0], 3, 0,  1, 5,  45);
	   SetOfflineHit(&hit0[1], 3, 0,  2, 0, 111);
	   SetOfflineHit(&hit0[2], 3, 0,  2, 3, 128);
	   SetOfflineHit(&hit0[3], 3, 2,  6, 2,   8);
	   SetOfflineHit(&hit0[4], 3, 2,  6, 2,   9);
	   SetOfflineHit(&hit0[5], 4, 0,  6, 7,   6);
	   SetOfflineHit(&hit0[6], 5, 0,  1, 6, 180);
	   SetOfflineHit(&hit0[7], 6, 0,  2, 2, 216);
	   SetOfflineHit(&hit0[8], 6, 1,  2, 4, 195);
	   SetOfflineHit(&hit0[9], 6, 2,  7, 4, 217);
      for(int i=0; i<10; i++) vdata.push_back(hit0[i]);
  }
  else if(ievent<25){
	int layer      = (ievent-1)/12;
	int ladder_phi = (ievent-1)%12;

	InttNameSpace::Offline_s hit1;
	SetOfflineHit(&hit1, layer+3, 0, ladder_phi, 0, 0);
	vdata.push_back(hit1);
	cout<<"set : "<<ievent<<" : "<<layer<<" "<<ladder_phi<<endl;
  }
  else if(ievent<57){
	int layer      = (ievent-25)/16;
	int ladder_phi = (ievent-25)%16;

	InttNameSpace::Offline_s hit1;
	SetOfflineHit(&hit1, layer+5, 0, ladder_phi, 0, 0);
	vdata.push_back(hit1);
	cout<<"set : "<<ievent<<" : "<<layer<<" "<<ladder_phi<<endl;
  }
  else if(ievent<(57+56*26)){
	//0-25
	int seed_lad  = (ievent-57);
        int ladder_idx = seed_lad/26; // 0-55
        int layer=0, lad_phi=0; 
        if(ladder_idx<24){
          layer   = ladder_idx/12;
          lad_phi = ladder_idx%12;
        }
        else if(ladder_idx<56){
          layer   = (ladder_idx-24)/16 + 2;
          lad_phi = (ladder_idx-24)%16;
        }
	else { cout<<"exceed range2 : "<<ievent<<endl; }

        // same for all ladders
        int seed  = seed_lad%26;
	int lad_z = 0;
	int sen_z = 0;
	if(seed<8) {
	  lad_z = 0;
	  sen_z = seed;
	}
	else if(seed<13) {
	  lad_z = 1;
	  sen_z = seed-8;
	}
	else if(seed<21) {
	  lad_z = 2;
	  sen_z = seed-13;
	}
	else if(seed<26) {
	  lad_z = 3;
	  sen_z = seed-21;
	}
	else { cout<<"exceed range"<<endl; }

	InttNameSpace::Offline_s hit1;
	SetOfflineHit(&hit1, layer+3, lad_z, lad_phi, sen_z, 0);
	vdata.push_back(hit1);
	cout<<"set : "<<ievent<<" : "<<lad_z<<" "<<sen_z<<" "<<lad_phi<<" "<<layer<<endl;
  }
  else if(ievent<(57+56*26+256*2)){
	//0-25
	int seed = (ievent-(57+56*26));

        int lad = seed/256;

        int ch = seed%256;


        int layer=0, lad_z=0, lad_phi=0, sen_z=0;

	InttNameSpace::Offline_s hit1;
	SetOfflineHit(&hit1, layer+3, 2*lad, lad_phi, sen_z, ch);
	vdata.push_back(hit1);
	cout<<"set : "<<ievent<<" : "<<lad_z<<" "<<sen_z<<" "<<lad_phi<<" "<<layer<<" "<<ch<<endl;
  }


  return vdata;
}

