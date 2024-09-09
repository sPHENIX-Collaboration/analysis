#include "InttDummyData.h"

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

/**
 * This class demonstrates the construction and use of an analysis module
 * within the sPHENIX Fun4All framework. It is intended to show how to
 * obtain physics objects from the analysis tree, and then write them out
 * to a ROOT tree and file for further offline analysis.
 */

/**
 * Constructor of module
 */
InttDummyData::InttDummyData(const std::string &name, const std::string &filename)
  : SubsysReco(name)
  , inttEvt_(nullptr)
  , ievent_(0)
{
  fname_ = filename;
}

/**
 * Destructor of module
 */
InttDummyData::~InttDummyData()
{
  if(inttEvt_!=nullptr) delete inttEvt_;
}

/**
 * Initialize the module and prepare looping over events
 */
int InttDummyData::Init(PHCompositeNode * /*topNode*/)
{
  if (Verbosity() > 5)
  {
    std::cout << "Beginning Init in InttDummyData" << std::endl;
  }

  return 0;
}

int InttDummyData::InitRun(PHCompositeNode * topNode)
{
  if (Verbosity() > 5)
  {
    std::cout << "Beginning InitRun in InttDummyData" << std::endl;
  }

  if(!topNode)
  {
	  std::cout << "InttDummyData::InitRun(PHCompositeNode* topNode)" << std::endl;
	  std::cout << "\tCould not retrieve topNode; doing nothing" << std::endl;

	  return -1;
  }

  PHNodeIterator dst_itr(topNode);
  PHCompositeNode* dst_node = dynamic_cast<PHCompositeNode*>(dst_itr.findFirst("PHCompositeNode", "DST"));
  if(!dst_node)
  {
	  if(Verbosity())std::cout << "InttDummyData::InitRun(PHCompositeNode* topNode)" << std::endl;
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
	  if(Verbosity())std::cout << "InttDummyData::InitRun(PHCompositeNode* topNode)" << std::endl;
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
int InttDummyData::process_event(PHCompositeNode *topNode)
{
	cout<<"InttDummyData process event"<<endl;
	if (Verbosity() > 5)
	{
		std::cout << "Beginning process_event in AnaTutorial" << std::endl;
	}

	TrkrHitSetContainer* trkr_hit_set_container = findNode::getClass<TrkrHitSetContainer>(topNode, "TRKR_HITSET");
	if(!trkr_hit_set_container)
	{
		std::cout << PHWHERE << std::endl;
		std::cout << "InttDummyDataDecoder::process_event(PHCompositeNode* topNode)" << std::endl;
		std::cout << "Could not get \"TRKR_HITSET\" from Node Tree" << std::endl;
		std::cout << "Exiting" << std::endl;
		exit(1);

		return Fun4AllReturnCodes::DISCARDEVENT;
	}


	/////////////////////
	if(ievent_>=120){
		cout<<"all events are already processed. quit InttDummyData"<<endl;
		return Fun4AllReturnCodes::EVENT_OK;
	}

	/*
	   offlineRawdata data[12];
	   data[0].set(3, 0,  1, 0, 5,  45, 0);
	   data[1].set(3, 0,  2, 0, 0, 111, 0);
	   data[2].set(3, 0,  2, 0, 3, 128, 0);
	   data[3].set(3, 2,  6, 0, 2,   8, 0);
	   data[4].set(3, 2,  6, 0, 2,   9, 0);
	   data[5].set(4, 0,  6, 0, 7,   6, 0);
	   data[6].set(5, 0,  1, 0, 6, 180, 0);
	   data[7].set(6, 0,  2, 0, 2, 216, 0);
	   data[8].set(6, 1,  2, 0, 4, 195, 0);
	   data[9].set(6, 2,  7, 0, 4, 217, 0);
	   */



        setDummyInttEvent(ievent_);

	int adc = 0;
	//int amp = 0;
	int bco = 0;

	TrkrDefs::hitsetkey hit_set_key = 0;
	TrkrDefs::hitkey hit_key = 0;
	TrkrHitSetContainer::Iterator hit_set_container_itr;
	TrkrHit* hit = nullptr;

	int nskip=0;
	for(int n = 0; n < inttEvt_->getNHits(); ++n)
	{
		InttHit* rawhit = inttEvt_->getHit(n);
                cout<<"inputhit : "<<rawhit->pid<<" "<<flush;
                rawhit->show();

		struct InttNameSpace::RawData_s rawdata;
		rawdata.felix_server  = rawhit->pid - 3001;
		rawdata.felix_channel = rawhit->module;
		rawdata.chip          = rawhit->chip_id;
		rawdata.channel       = rawhit->chan_id;

		adc = rawhit->adc;
		//amp = p->iValue(n, "AMPLITUE");
		bco = 0; // always 0 until time-in issue fixed; //-- rawhit->bco;

		struct InttNameSpace::Offline_s offline = InttNameSpace::ToOffline(rawdata);

                //--//------------------
                //--// change the ladder id by hand
                //--int nladder = (offline.layer<5) ? 12 : 16;
                //--//int offset  = 2;
                //--//if(     offline.layer==4) offset=3;
                //--//else if(offline.layer==5) offset=3;
                //--//else if(offline.layer==6) offset=4;
                //--
                //--int offset  = 3;
                //--if(     offline.layer<5)  offset=3;
                //--else                      offset=4;
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
int InttDummyData::End(PHCompositeNode * /*topNode*/)
{
  if (Verbosity() > 1)
  {
    std::cout << "Ending InttDummyData analysis package" << std::endl;
  }


  return 0;
}

void InttDummyData::setDummyInttEvent(const int ievent)
{
  struct inputdata {
    inputdata()
      :pid(0), module(0), chip_id(0), chan_id(0), adc(0)
     { }

    int pid;
    int module;
    int chip_id;
    int chan_id;
    int adc;

    void set(int Pid, int Mod, int Chip, int Chan, int Adc){
      pid     = Pid;  // 0-7
      module  = Mod;  // 0-13
      chip_id = Chip; // 1-26
      chan_id = Chan; // 0-127
      adc     = Adc;  // 0-7
    }

    void set(InttEvent* inttEvt){
      InttHit * hit = inttEvt->addHit();
      hit->pid     = pid; 
      hit->module  = module; 
      hit->chip_id = chip_id; 
      hit->chan_id = chan_id; 
      hit->adc     = adc; 
    }
  };

  // set dummy data
  inputdata input;
  // ievent:0-7 for felix check
  if(ievent<8){
    input.set(3001+ievent, 0, 1, 0, 0);
  }
  else if(ievent<8+14*8){
    int pid = (ievent-8)/14;
    int mod = (ievent-8)%14;
    input.set(3001+pid, mod, 1, 0, 0);
    cout<<"set : "<<ievent<<" : "<<pid<<" "<<mod<<endl;
  }

  inttEvt_->clear();
  input.set(inttEvt_);
}

