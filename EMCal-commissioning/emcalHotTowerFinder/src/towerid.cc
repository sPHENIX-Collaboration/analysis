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

#include <cdbobjects/CDBTTree.h>

//________________________________
towerid::towerid(const std::string &name, const std::string &cdbtreename, float adccut_sg, float adccut_k, float sigmas_lo, float sigmas_hi, float SG_f, float Kur_f, float region_f): 
SubsysReco(name)
  ,T(nullptr)
  ,Outfile(name) 	
  ,cdbtreename(cdbtreename)
  ,adccut_sg(adccut_sg)		// The minimum ADC required for a tower with Saint Gobain fibers to register a hit
  ,adccut_k(adccut_k)		// Minimum ADC for Kurary fibers to register a hit
  ,sigmas_lo(sigmas_lo)		// # of standard deviations from the mode for a cold tower
  ,sigmas_hi(sigmas_hi)		// # of standard deviations from the mode for a hot tower
  ,SG_f(SG_f)			// Fiducial cut (artificial maximum) for Saint Gobain towers
  ,Kur_f(Kur_f)			// Fiducial cut for Kurary
  ,region_f(region_f)		// Fiducial cut for Sectors and IBs 
{
  std::cout << "towerid::towerid(const std::string &name) Calling ctor" << std::endl;
}
//__________________________________
towerid::~towerid()
{
  std::cout << "towerid::~towerid() Calling dtor" << std::endl;
}
//_____________________________
int towerid::Init(PHCompositeNode *topNode)
{
	//initialize output file with hot channels
  out = new TFile(Outfile.c_str(),"RECREATE");
  T = new TTree("T","T");
  T -> Branch("hot_channels",&m_hot_channels);

	//initialize tree with SG vs Kurary fiber information
  fchannels = new TFile("channels.root");
  channels = (TTree*)fchannels->Get("myTree");
  channels->SetBranchAddress("fiber_type",&fiber_type);

  std::cout << "towerid::Init(PHCompositeNode *topNode) Initializing" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;

}
//_____________________________
int towerid::InitRun(PHCompositeNode *topNode)
{
  std::cout << "towerid::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}
//_____________________________
int towerid::process_event(PHCompositeNode *topNode)
{

//Get TowerInfoContainer
TowerInfoContainer *emcTowerContainer;
emcTowerContainer =  findNode::getClass<TowerInfoContainer>(topNode,"TOWERS_CEMC");
 if(!emcTowerContainer)
    {
      std::cout << PHWHERE << "towerid::process_event Could not find node TOWERS_CEMC"  << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }
//iterate through all towers, incrementing their Frequency arrays if they record a hit

 bool goodevent = false;
 int tower_range = emcTowerContainer->size();
        for(int j = 0; j < tower_range; j++){
	
	//	unsigned int towerkey = emcTowerContainer->encode_key(j);
       // unsigned int ieta = TowerInfoDefs::getCaloTowerEtaBin(towerkey);
	//	unsigned int iphi = TowerInfoDefs::getCaloTowerPhiBin(towerkey);

	    double energy = emcTowerContainer -> get_tower_at_channel(j) -> get_energy();
		channels->GetEntry(j);
		if((fiber_type ==0) && (energy > adccut_sg)){
		towerF[j]++;
		goodevent = true; //counter of events with nonzero EMCal energy
                sectorF[j/384]++;
                ibF[j/64]++;
		}
		else if ((fiber_type ==1) &&( energy > adccut_k)){
		towerF[j]++;
		goodevent = true;
		sectorF[j/384]++;
		ibF[j/64]++;
		}
				
		//std::cout << energy << std::endl;
		towerE[j]+=energy;
		sectorE[j/384]+=energy;
		ibE[j/64]+=energy;
		
			
            /*if(energy > adccut){
                towerF[j]++;
            }*/
	}
	if(goodevent == true){goodevents++;}
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
	
 // initialize histograms: Initial frequency spectra, Energy spectrum, and Frequency spectrum with Fiducial cuts

  Fspeci->SetBins(goodevents,0,goodevents);
  Fspeci_SG->SetBins(goodevents,0,goodevents);
  Fspeci_K->SetBins(goodevents,0,goodevents);
  Fspeci_IB->SetBins(goodevents,0,goodevents);
  Fspeci_sector->SetBins(goodevents,0,goodevents);	

  Fspec->SetBins(goodevents,0,goodevents);
  Fspec_SG->SetBins(SG_f*goodevents,0,SG_f*goodevents);
  Fspec_K->SetBins(Kur_f*goodevents,0,Kur_f*goodevents);  
  Fspec_IB->SetBins(region_f*goodevents,0,region_f*goodevents);
  Fspec_sector->SetBins(region_f*goodevents,0,region_f*goodevents);
  
  Espec->SetBins(goodevents,0,goodevents);
  Espec_SG->SetBins(goodevents,0,goodevents);
  Espec_K->SetBins(goodevents,0,goodevents);
  Espec_IB->SetBins(goodevents,0,goodevents);
  Espec_sector->SetBins(goodevents,0,goodevents); 

 //fill histograms
 
 for(int i = 0; i < 24576; i++){

	Fspec->Fill(towerF[i]);
	Espec->Fill(towerE[i]);
	Fspeci->Fill(towerF[i]);

	channels->GetEntry(i);
	if(fiber_type == 0){
	Fspec_SG->Fill(towerF[i]);
	Espec_SG->Fill(towerE[i]);
	Fspeci_SG->Fill(towerF[i]);
	}
	else{
	Fspec_K->Fill(towerF[i]);
	Espec_K->Fill(towerE[i]);
	Fspeci_K->Fill(towerF[i]);
	}
	}
 for(int j = 0; j<384; j++){
	Fspec_IB->Fill(1.0*ibF[j]/64);
	Espec_IB->Fill(1.0*ibE[j]/64);
	Fspeci_IB->Fill(1.0*ibF[j]/64);
	}
 for(int k = 0; k<64; k++){
	Fspec_sector->Fill(1.0*sectorF[k]/384);
	Espec_sector->Fill(1.0*sectorE[k]/384);
	Fspeci_sector->Fill(1.0*sectorF[k]/384);
	}

  //kill zeros: 

  Fspec->SetBinContent(1,0);
  Fspec_SG->SetBinContent(1,0);
  Fspec_K->SetBinContent(1,0);
  Fspec_IB->SetBinContent(1,0);
  Fspec_sector->SetBinContent(1,0);
	
	float cutoffFreq_SG;
	float cutoffFreq_K;
	float cutoffFreq;
	
	float cutoffFreq_sector;
	float cutoffFreq_IB;

	float cutoffFreq_SG_lo;
        float cutoffFreq_K_lo;

        float cutoffFreq_sector_lo;
        float cutoffFreq_IB_lo;

	cutoffFreq_SG = Fspec_SG->GetStdDev()*sigmas_hi + Fspec_SG->GetXaxis()->GetBinCenter(Fspec_SG->GetMaximumBin());
	cutoffFreq_K = Fspec_K->GetStdDev()*sigmas_hi + Fspec_K->GetXaxis()->GetBinCenter(Fspec_K->GetMaximumBin());
	cutoffFreq = Fspec->GetStdDev()*sigmas_hi + Fspec->GetXaxis()->GetBinCenter(Fspec->GetMaximumBin());	
	cutoffFreq_IB = Fspec_IB->GetStdDev()*sigmas_hi + Fspec_IB->GetXaxis()->GetBinCenter(Fspec_IB->GetMaximumBin());
	cutoffFreq_sector = Fspec_sector->GetStdDev()*sigmas_hi + Fspec_sector->GetXaxis()->GetBinCenter(Fspec_sector->GetMaximumBin());


	cutoffFreq_SG_lo = -1*Fspec_SG->GetStdDev()*sigmas_lo + Fspec_SG->GetXaxis()->GetBinCenter(Fspec_SG->GetMaximumBin());
        cutoffFreq_K_lo = -1*Fspec_K->GetStdDev()*sigmas_lo + Fspec_K->GetXaxis()->GetBinCenter(Fspec_K->GetMaximumBin());
        cutoffFreq_IB_lo = -1*Fspec_IB->GetStdDev()*sigmas_lo + Fspec_IB->GetXaxis()->GetBinCenter(Fspec_IB->GetMaximumBin());
        cutoffFreq_sector_lo = -1*Fspec_sector->GetStdDev()*sigmas_lo + Fspec_sector->GetXaxis()->GetBinCenter(Fspec_sector->GetMaximumBin());
	
	std::cout << "towerid::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
  	std::cout << "Saint Gobain Cutoff Frequency: " << cutoffFreq_SG << std::endl;
  	std::cout <<  "Kurary Cutoff Frequency: " << cutoffFreq_K << std::endl;
  	std::cout << "Overall Tower Cutoff Frequency: " << cutoffFreq << std::endl;
  	std::cout << "IB Cutoff hit Frequency: " << cutoffFreq_IB << std::endl;
 	 std::cout <<  "Sector Cutoff hit Frequency: " << cutoffFreq_sector << std::endl;

    for(int i = 0; i < 24576; i++){

	channels->GetEntry(i);
	if(fiber_type == 0 && towerF[i] > cutoffFreq_SG){
		hottowers[i]++;
	}
	else if(fiber_type == 1 && towerF[i]> cutoffFreq_K){
		hottowers[i]++;
	}
	else if(towerF[i]==0){
		deadtowers[i]++;
	}
	/*
	if(towerF[i]> cutoffFreq){
	//	std::cout << i << std::endl;
		m_hot_channels = i;
		T->Fill();
	}	*/

	else if (towerF[i] < cutoffFreq_SG_lo && fiber_type == 0 && towerF[i]>0){
		coldtowers[i]++;
	}
	else if (towerF[i] < cutoffFreq_K_lo && fiber_type ==1 && towerF[i] > 0){
		coldtowers[i]++;	
	}
	}
    hot_regions = 0;
    cold_regions = 0;	
    for(int j = 0; j<384; j++){
	if((ibF[j]/64.0)>cutoffFreq_IB){
		hot_regions = 1;
		std::cout << "IB " << j << "is hot with ADC rate" << (ibF[j]/64.0) << std::endl;
		hotIB[j]++;
		for(int j1 = 0; j1<64; j1++ ){
			hottowers[j*64+j1]++;
			towerF[j*64+j1] = 0;
		
		}
		ibF[j] = 0;
	}
	}
    
    for(int k = 0; k<64; k++){
	if((sectorF[k]/384.0)>cutoffFreq_sector){
		hot_regions = 1;
		std::cout << "sector " << k << "is hot with ADC rate" << (sectorF[k]/384.0) << std::endl;
		hotsectors[k]++;
		for(int k1 = 0; k1 < 384; k1++){
			hottowers[k*384+k1]++;
			towerF[k*384+k1] = 0;
		}
		sectorF[k] = 0;
	}
	}

        for(int j = 0; j<384; j++){
        if((ibF[j]/64.0) < cutoffFreq_IB_lo && ibF[j]/64.0 > 0){
                cold_regions = 1;
                std::cout << "IB " << j << "is cold with ADC rate" << (ibF[j]/64.0) << std::endl;
                coldIB[j]++;
                for(int j1 = 0; j1<64; j1++ ){
                        coldtowers[j*64+j1]++;
                        towerF[j*64+j1] = 0;

                }
                ibF[j] = 0;
        }
        }
    
    for(int k = 0; k<64; k++){
        if((sectorF[k]/384.0)<cutoffFreq_sector_lo && sectorF[k] > 0){
                cold_regions = 1;
                std::cout << "sector " << k << "is cold  with ADC rate" << (sectorF[k]/384.0) << std::endl;
                coldsectors[k]++;
                for(int k1 = 0; k1 < 384; k1++){
                        coldtowers[k*384+k1]++;
                        towerF[k*384+k1] = 0;
                }
                sectorF[k] = 0;
        }
        }


	while(hot_regions == 1 || cold_regions ==1){
	std::cout << "hot/cold IB or sector detected. Running another pass for hot towers" << std::endl;
	Fspec->Reset();
        Fspec_SG->Reset();
        Fspec_K->Reset();
        Fspec_IB->Reset();
        Fspec_sector->Reset();

        Espec->Reset();
        Espec_SG->Reset();
        Espec_K->Reset();
        Espec_IB->Reset();
        Espec_sector->Reset();

	for(int i = 0; i < 24576; i++){

        	Fspec->Fill(towerF[i]);
        	Espec->Fill(towerE[i]);
        	channels->GetEntry(i);
        	if(fiber_type == 0){
        		Fspec_SG->Fill(towerF[i]);
        		Espec_SG->Fill(towerE[i]);
        		}
      	 	else{
        		Fspec_K->Fill(towerF[i]);
        		Espec_K->Fill(towerE[i]);
        		}
        	}

 	for(int j = 0; j<384; j++){
        	Fspec_IB->Fill(1.0*ibF[j]/64);
        	Espec_IB->Fill(1.0*ibE[j]/64);
        	}
 	for(int k = 0; k<64; k++){
        	Fspec_sector->Fill(1.0*sectorF[k]/384);
        	Espec_sector->Fill(1.0*sectorE[k]/384);
        	}

  	Fspec->SetBinContent(1,0);
  	Fspec_SG->SetBinContent(1,0);
  	Fspec_K->SetBinContent(1,0);
  	Fspec_IB->SetBinContent(1,0);
	  Fspec_sector->SetBinContent(1,0);


        cutoffFreq_SG = Fspec_SG->GetStdDev()*sigmas_hi + Fspec_SG->GetXaxis()->GetBinCenter(Fspec_SG->GetMaximumBin());
        cutoffFreq_K = Fspec_K->GetStdDev()*sigmas_hi + Fspec_K->GetXaxis()->GetBinCenter(Fspec_K->GetMaximumBin());
        cutoffFreq = Fspec->GetStdDev()*sigmas_hi + Fspec->GetXaxis()->GetBinCenter(Fspec->GetMaximumBin());
        cutoffFreq_IB = Fspec_IB->GetStdDev()*sigmas_hi + Fspec_IB->GetXaxis()->GetBinCenter(Fspec_IB->GetMaximumBin());
        cutoffFreq_sector = Fspec_sector->GetStdDev()*sigmas_hi + Fspec_sector->GetXaxis()->GetBinCenter(Fspec_sector->GetMaximumBin());
	
	cutoffFreq_SG_lo = -1*Fspec_SG->GetStdDev()*sigmas_lo + Fspec_SG->GetXaxis()->GetBinCenter(Fspec_SG->GetMaximumBin());
        cutoffFreq_K_lo = -1*Fspec_K->GetStdDev()*sigmas_lo + Fspec_K->GetXaxis()->GetBinCenter(Fspec_K->GetMaximumBin());
        cutoffFreq_IB_lo = -1*Fspec_IB->GetStdDev()*sigmas_lo + Fspec_IB->GetXaxis()->GetBinCenter(Fspec_IB->GetMaximumBin());
        cutoffFreq_sector_lo = -1*Fspec_sector->GetStdDev()*sigmas_lo + Fspec_sector->GetXaxis()->GetBinCenter(Fspec_sector->GetMaximumBin());


	for(int i = 0; i < 24576; i++){

        	channels->GetEntry(i);
        	if(fiber_type == 0 && towerF[i] > cutoffFreq_SG){
                	hottowers[i]++;
        	}
        	else if(fiber_type == 1 && towerF[i]> cutoffFreq_K){
                	hottowers[i]++;
        	}
		else if (towerF[i] < cutoffFreq_SG_lo && fiber_type == 0 && towerF[i]>0){
             		   coldtowers[i]++;
        	}
        	else if (towerF[i] < cutoffFreq_K_lo && fiber_type ==1 && towerF[i] > 0){
                		coldtowers[i]++;
        	}

        	}

    	hot_regions = 0;
	cold_regions = 0;
    	for(int j = 0; j<384; j++){
        	if((ibF[j]/64.0)>cutoffFreq_IB){
			std::cout << "IB " << j << "is hot with ADC rate " << (ibF[j]/64.0) <<std::endl;
                	hot_regions = 1;
                	hotIB[j]++;
                	for(int j1 = 0; j1<64; j1++ ){
                        	hottowers[j*64+j1]++;
                        	towerF[j*64+j1] = 0;
                	}
			ibF[j] = 0;
        	}
        }

    	for(int k = 0; k<64; k++){
        	if((sectorF[k]/384.0)>cutoffFreq_sector){
	                std::cout << "sector " << k  << "is hot with ADC rate " << (sectorF[k]/384.0)<< std::endl;	
			hot_regions = 1;
                	hotsectors[k]++;
                	for(int k1 = 0; k1 < 384; k1++){
                        	hottowers[k*384+k1]++;
                        	towerF[k*384+k1] = 0;
                	}
			sectorF[k] = 0;
        	}
        }
	        for(int j = 0; j<384; j++){
        if((ibF[j]/64.0) < cutoffFreq_IB_lo && ibF[j]/64.0 > 0){
                cold_regions = 1;
                std::cout << "IB " << j << "is cold with ADC rate" << (ibF[j]/64.0) << std::endl;
                coldIB[j]++;
                for(int j1 = 0; j1<64; j1++ ){
                        coldtowers[j*64+j1]++;
                        towerF[j*64+j1] = 0;

                }
                ibF[j] = 0;
        }
        }
    
    for(int k = 0; k<64; k++){
        if((sectorF[k]/384.0)<cutoffFreq_sector_lo && sectorF[k] > 0){
                cold_regions = 1;
                std::cout << "sector " << k << "is cold  with ADC rate" << (sectorF[k]/384.0) << std::endl;
                coldsectors[k]++;
                for(int k1 = 0; k1 < 384; k1++){
                        coldtowers[k*384+k1]++;
                        towerF[k*384+k1] = 0;
                }
                sectorF[k] = 0;
        }
        }
	}
  std::cout << "towerid::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
  std::cout << "Saint Gobain Cutoff Frequency: " << cutoffFreq_SG << std::endl;
  std::cout <<  "Kurary Cutoff Frequency: " << cutoffFreq_K << std::endl;
  std::cout << "Overall Tower Cutoff Frequency: " << cutoffFreq << std::endl;
  std::cout << "IB Cutoff hit Frequency: " << cutoffFreq_IB << std::endl;
  std::cout <<  "Sector Cutoff hit Frequency: " << cutoffFreq_sector << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int towerid::End(PHCompositeNode *topNode)
{
  std::cout << "towerid::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  
   CDBTTree*cdbttree = new CDBTTree(cdbtreename);
   std::string fieldname = "status";
   for(int i = 0; i<24576; i++){

                if(hottowers[i] >= 0.5){
                        m_hot_channels = 1;
                        T->Fill();
			cdbttree->SetIntValue(i,fieldname,2);
                }
		else if(deadtowers[i] >= 0.5){
			m_hot_channels = -1;
                        T->Fill();
			cdbttree->SetIntValue(i,fieldname,1);
		}
		else if(coldtowers[i] > 0.5){
			m_hot_channels = -1;
                        T->Fill();
			cdbttree->SetIntValue(i,fieldname,3);
		}
		else{
			m_hot_channels = 0;
                        T->Fill();
			cdbttree->SetIntValue(i,fieldname,0);
		}
        }

  fchannels->Close();
  delete fchannels;
  out -> cd();
  Fspec->Write();
  Fspec_K->Write();
  Fspec_SG->Write();
  Fspec_sector->Write();
  Fspec_IB->Write();
  Espec->Write();
  Espec_SG->Write();
  Espec_K->Write();
  Espec_sector->Write();
  Espec_IB->Write();
   Fspeci->Write();
  Fspeci_K->Write();
  Fspeci_SG->Write();
  Fspeci_sector->Write();
  Fspeci_IB->Write();
  T -> Write();
  out -> Close();
  delete out;
  cdbttree->Commit();
  cdbttree->Print();
  cdbttree->WriteCDBTTree();
  delete cdbttree;

  return Fun4AllReturnCodes::EVENT_OK;
 }

 //____________________________________________________________________________..
 int towerid::Reset(PHCompositeNode *topNode)
   {
 std::cout << "towerid::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
 return Fun4AllReturnCodes::EVENT_OK;
    }
//____________________________________________________
void towerid::Print(const std::string &what) const
{
  std::cout << "towerid::Print(const std::string &what) const Printing info for " << what << std::endl;
}

//____________________________________________________







