#include "towerid.h"

#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>


//Fun4All
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllHistoManager.h>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>
#include <phool/phool.h>
#include <ffaobjects/EventHeader.h>

//ROOT
#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TH2F.h>

//Tower stuff
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfo.h>
#include <calobase/TowerInfoDefs.h>

//________________________________
towerid::towerid(const std::string &name, float adccut, float sigmas, int event_end): 
SubsysReco(name)
  ,T(nullptr)
  ,Outfile(name)
  ,adccut(adccut)
  ,sigmas(sigmas)
  ,event_end(event_end)
{
  std::cout << "caloTreeGen::caloTreeGen(const std::string &name) Calling ctor" << std::endl;
}
//__________________________________
towerid::~towerid()
{
  std::cout << "caloTreeGen::~caloTreeGen() Calling dtor" << std::endl;
}
//_____________________________
int towerid::Init(PHCompositeNode *topNode)
{

  out = new TFile(Outfile.c_str(),"RECREATE");

  T = new TTree("T","T");
  T -> Branch("hot_channels",&m_hot_channels);
  std::cout << "caloTreeGen::Init(PHCompositeNode *topNode) Initializing" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}
//_____________________________
int towerid::InitRun(PHCompositeNode *topNode)
{
  std::cout << "caloTreeGen::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}
//_____________________________
int towerid::process_event(PHCompositeNode *topNode)
{

TowerInfoContainer *emcTowerContainer;
emcTowerContainer =  findNode::getClass<TowerInfoContainer>(topNode,"TOWERS_CEMC");
 if(!emcTowerContainer)
    {
      std::cout << PHWHERE << "caloTreeGen::process_event Could not find node TOWERS_CEMC"  << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }
 int tower_range = emcTowerContainer->size();
        for(int j = 0; j < tower_range; j++){
	
	//	unsigned int towerkey = emcTowerContainer->encode_key(j);
        //unsigned int ieta = TowerInfoDefs::getCaloTowerEtaBin(towerkey);

	    double energy = emcTowerContainer -> get_tower_at_channel(j) -> get_energy();	
            if(energy > adccut){
                towerF[j]++;
            }
	}
  return Fun4AllReturnCodes::EVENT_OK;
}
//__________________________
int towerid::ResetEvent(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}


//__________________________
int towerid::EndRun(const int runnumber)
{

  if(event_end == 0){
	event_end = 1000000;
	}
  Fspec->SetBins(0.5*event_end,0,0.5*event_end);
     for(int i = 0; i < 24576; i++){
	Fspec->Fill(towerF[i]);
	}     
  Fspec->SetBinContent(1,0);

     std = Fspec->GetStdDev();
     binmax = Fspec->GetMaximumBin();
     mode = Fspec->GetXaxis()->GetBinCenter(binmax);
     cutoffFreq = std*sigmas+ mode;
     std::cout << cutoffFreq << std::endl;
     std::cout << Fspec->Integral(0,Fspec->FindBin(cutoffFreq))<< std::endl;	
    for(int q = 0; q < 24576; q++){
	std::cout << q << towerF[q] << std::endl;
        }	 
    for(int i = 0; i < 24576; i++){

	if(towerF[i]> cutoffFreq){
		std::cout << i << std::endl;
		m_hot_channels = i;
		T->Fill();
	}
	}

  std::cout << "caloTreeGen::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int towerid::End(PHCompositeNode *topNode)
{
  std::cout << "caloTreeGen::End(PHCompositeNode *topNode) This is the End..." << std::endl;

  out -> cd();
  Fspec->Write();
  T -> Write();
  out -> Close();
  delete out;

  //hm -> dumpHistos(Outfile.c_str(), "UPDATE");
  return Fun4AllReturnCodes::EVENT_OK;
 }

 //____________________________________________________________________________..
 int towerid::Reset(PHCompositeNode *topNode)
   {
 std::cout << "caloTreeGen::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
 return Fun4AllReturnCodes::EVENT_OK;
    }
//____________________________________________________
void towerid::Print(const std::string &what) const
{
  std::cout << "caloTreeGen::Print(const std::string &what) const Printing info for " << what << std::endl;
}

//____________________________________________________










