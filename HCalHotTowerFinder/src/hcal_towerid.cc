#include "hcal_towerid.h"
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

#include <cdbobjects/CDBTTree.h>

//________________________________
hcal_towerid::hcal_towerid(const std::string &name, const std::string &cdbtreename_i, const std::string &cdbtreename_o, float adccut_i, float adccut_o, float sigmas_lo, float sigmas_hi, float inner_f,float outer_f): 
SubsysReco(name)
  ,T(nullptr)
  ,Outfile(name) 	
  ,cdbtreename_i(cdbtreename_i)
  ,cdbtreename_o(cdbtreename_o)
  ,adccut_i(adccut_i)
  ,adccut_o(adccut_o)		// Minimum ADC for Kurary fibers to register a hit
  ,sigmas_lo(sigmas_lo)		// # of standard deviations from the mode for a cold tower
  ,sigmas_hi(sigmas_hi)		// # of standard deviations from the mode for a hot tower
  ,inner_f(inner_f)
  ,outer_f(outer_f)		// Fiducial cut for Sectors and IBs 
{
  std::cout << "towerid::towerid(const std::string &name) Calling ctor" << std::endl;
}
//__________________________________
hcal_towerid::~hcal_towerid()
{
  std::cout << "towerid::~towerid() Calling dtor" << std::endl;
}
//_____________________________
int hcal_towerid::Init(PHCompositeNode *topNode)
{
	//initialize output file with hot channels
  out = new TFile(Outfile.c_str(),"RECREATE");
  T = new TTree("T_inner","T_inner");
  T2 = new TTree("T_outer","T_outer");
  T -> Branch("hot_channels",&m_hot_channels_i);
  T2-> Branch("hot_channels",&m_hot_channels_o);

  std::cout << "towerid::Init(PHCompositeNode *topNode) Initializing" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;

}
//_____________________________
int hcal_towerid::InitRun(PHCompositeNode *topNode)
{
  std::cout << "towerid::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}
//_____________________________
int hcal_towerid::process_event(PHCompositeNode *topNode)
{

//Get TowerInfoContainer
TowerInfoContainer *hcalTowerContainer_i;
TowerInfoContainer *hcalTowerContainer_o;

hcalTowerContainer_i =  findNode::getClass<TowerInfoContainer>(topNode,"TOWERS_HCALIN");
hcalTowerContainer_o =  findNode::getClass<TowerInfoContainer>(topNode,"TOWERS_HCALOUT");
 if(!hcalTowerContainer_o)
    {
      std::cout << PHWHERE << "towerid::process_event Could not find node TOWERS_CEMC"  << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }
//iterate through all towers, incrementing their Frequency arrays if they record a hit

 bool goodevent = false;
 int tower_range_i = hcalTowerContainer_i->size();
 int tower_range_o = hcalTowerContainer_o->size();
        for(int j = 0; j < tower_range_i; j++){
	
	    double energy = hcalTowerContainer_i -> get_tower_at_channel(j) -> get_energy();
		if(energy > adccut_i){
		itowerF[j]++;
		goodevent = true; //counter of events with nonzero EMCal energy
		}
				
		//std::cout << energy << std::endl;
		itowerE[j]+=energy;
		
	}
	for(int j = 0; j < tower_range_o; j++){

            double energy = hcalTowerContainer_o -> get_tower_at_channel(j) -> get_energy();
                if(energy > adccut_o){
                otowerF[j]++;
                goodevent = true; //counter of events with nonzero EMCal energy
                }

                //std::cout << energy << std::endl;
                otowerE[j]+=energy;
                
        }
	if(goodevent == true){goodevents++;}
  return Fun4AllReturnCodes::EVENT_OK;
}
//__________________________
int hcal_towerid::ResetEvent(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}


//__________________________
int hcal_towerid::EndRun(const int runnumber)
{	
	
 // initialize histograms: Initial frequency spectra, Energy spectrum, and Frequency spectrum with Fiducial cuts

  Fspeci_i->SetBins(goodevents,0,goodevents);
  Fspeci_o->SetBins(goodevents,0,goodevents);

  Fspec_i->SetBins(goodevents,0,inner_f*goodevents);
  Fspec_o->SetBins(goodevents,0,outer_f*goodevents);
  
  Espec_i->SetBins(goodevents,0,goodevents);
  Espec_o->SetBins(goodevents,0,goodevents);
 //fill histograms
 
 for(int i = 0; i < 1536; i++){

	Fspec_i->Fill(itowerF[i]);
	Espec_i->Fill(itowerE[i]);
	Fspeci_i->Fill(itowerF[i]);

	Fspec_o->Fill(otowerF[i]);
        Espec_o->Fill(otowerE[i]);
        Fspeci_o->Fill(otowerF[i]);
	}

  //kill zeros: 

  Fspec_i->SetBinContent(1,0);
  Fspec_o->SetBinContent(1,0);	

	float cutoffFreq_i;
	float cutoffFreq_i_lo;	
	float cutoffFreq_o;
	float cutoffFreq_o_lo;

	cutoffFreq_i = Fspec_i->GetStdDev()*sigmas_hi + Fspec_i->GetXaxis()->GetBinCenter(Fspec_i->GetMaximumBin());	
	cutoffFreq_i_lo = -1*Fspec_i->GetStdDev()*sigmas_lo + Fspec_i->GetXaxis()->GetBinCenter(Fspec_i->GetMaximumBin());	
	cutoffFreq_o = Fspec_o->GetStdDev()*sigmas_hi + Fspec_o->GetXaxis()->GetBinCenter(Fspec_o->GetMaximumBin());
	cutoffFreq_o_lo = -1*Fspec_o->GetStdDev()*sigmas_lo + Fspec_o->GetXaxis()->GetBinCenter(Fspec_o->GetMaximumBin());	

	std::cout << "towerid::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
	std::cout << "Inner hot tower cutoff: " << cutoffFreq_i << std::endl;
	std::cout << "Inner cold tower cutoff: " << cutoffFreq_i_lo << std::endl;    

	std::cout << "Outer hot tower cutoff: " << cutoffFreq_o << std::endl;
        std::cout << "OUter cold tower cutoff: " << cutoffFreq_o_lo << std::endl;

  for(int i = 0; i < 1536; i++){

	if(itowerF[i] > cutoffFreq_i){
		ihottowers[i]++;
	}
	if(otowerF[i]> cutoffFreq_o){
		ohottowers[i]++;
	}
	if(itowerF[i]==0){
		ideadtowers[i]++;
	}
	if(otowerF[i]==0){
		odeadtowers[i]++;
	}
	if(itowerF[i] < cutoffFreq_i_lo && itowerF[i]>0){
		icoldtowers[i]++;
	}
	if (otowerF[i] < cutoffFreq_o_lo && otowerF[i] > 0){
		ocoldtowers[i]++;	
	}
	}
    
  std::cout << "towerid::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int hcal_towerid::End(PHCompositeNode *topNode)
{
  std::cout << "towerid::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  
   CDBTTree*icdbttree = new CDBTTree(cdbtreename_i);
   CDBTTree*ocdbttree = new CDBTTree(cdbtreename_o);
   std::string fieldname = "status";
   for(int i = 0; i<1536; i++){

                if(ihottowers[i] >= 0.5){
                        m_hot_channels_i = 2;
                        T->Fill();
			icdbttree->SetIntValue(i,fieldname,2);
                }
	       else if(ideadtowers[i] >= 0.5){
			m_hot_channels_i = 1;
                        T->Fill();
			icdbttree->SetIntValue(i,fieldname,1);
		 }
		else if(icoldtowers[i] > 0.5){
			m_hot_channels_i = 3;
                        T->Fill();
			icdbttree->SetIntValue(i,fieldname,3);
		}
		else{
			m_hot_channels_i = 0;
                        T->Fill();
			icdbttree->SetIntValue(i,fieldname,0);
		}
        }
	
  for(int j = 0; j<1536; j++){

                if(ohottowers[j] >= 0.5){
                        m_hot_channels_o = 2;
                        T2->Fill();
                        ocdbttree->SetIntValue(j,fieldname,2);
                }
               else if(odeadtowers[j] >= 0.5){
                        m_hot_channels_o = 1;
                        T2->Fill();
                        ocdbttree->SetIntValue(j,fieldname,1);
                 }
                else if(ocoldtowers[j] > 0.5){
                        m_hot_channels_i = 3;
                        T2->Fill();
                        ocdbttree->SetIntValue(j,fieldname,3);
                }
                else{
                        m_hot_channels_i = 0;
                        T2->Fill();
                        ocdbttree->SetIntValue(j,fieldname,0);
                }
        }

  out -> cd();
  Fspec_i->Write();
  Fspeci_i->Write();
  Espec_i->Write();
  Fspec_o->Write();
  Fspeci_o->Write();
  Espec_o->Write();
  T -> Write();
  T2->Write();
  out -> Close();
  delete out;
  icdbttree->Commit();
  //icdbttree->Print();
  icdbttree->WriteCDBTTree();
  delete icdbttree;

  ocdbttree->Commit();
  //ocdbttree->Print();
  ocdbttree->WriteCDBTTree();
  delete ocdbttree;

  return Fun4AllReturnCodes::EVENT_OK;
 }

 //____________________________________________________________________________..
 int hcal_towerid::Reset(PHCompositeNode *topNode)
   {
 std::cout << "towerid::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
 return Fun4AllReturnCodes::EVENT_OK;
    }
//____________________________________________________
void hcal_towerid::Print(const std::string &what) const
{
  std::cout << "towerid::Print(const std::string &what) const Printing info for " << what << std::endl;
}

//____________________________________________________







