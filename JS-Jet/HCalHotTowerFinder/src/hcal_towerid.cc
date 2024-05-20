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
#include <TStyle.h>
#include <TSystem.h>
#include <TPad.h>
#include <TLine.h>

//Tower stuff
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfo.h>
#include <calobase/TowerInfoDefs.h>

#include <cdbobjects/CDBTTree.h>

//________________________________
hcal_towerid::hcal_towerid(const std::string &name, const std::string &outfilename, const std::string &icdbtreename, const std::string &ocdbtreename, float adccut_i, float adccut_o, float sigmas_lo, float sigmas_hi, float inner_f,float outer_f): 
SubsysReco(name)
,Tinner(nullptr)
,Touter(nullptr)
,Outfile(outfilename) 	
,icdbtreename(icdbtreename)
  ,ocdbtreename(ocdbtreename)
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
//TowerInfoContainer *hcalTowerContainer_o;

  hcalTowerContainer_i =  findNode::getClass<TowerInfoContainer>(topNode,"TOWERS_HCALIN");
// adding HCALIN
  if(!hcalTowerContainer_i)
    {
      std::cout << PHWHERE << "towerid::process_event Could not find node TOWERS_HCALIN"  << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }
  
  TowerInfoContainer *hcalTowerContainer_o;
  hcalTowerContainer_o =  findNode::getClass<TowerInfoContainer>(topNode,"TOWERS_HCALOUT");
  if(!hcalTowerContainer_o)
    {
      std::cout << PHWHERE << "towerid::process_event Could not find node TOWERS_HCALOUT"  << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }
  //iterate through all towers, incrementing their Frequency arrays if they record a hit

  bool goodevent = false;
  int tower_range_i = hcalTowerContainer_i->size();
  int tower_range_o = hcalTowerContainer_o->size();
  iHCal_hist = new TH1F("iHCal_energy", "Inner HCal Energy Distribution", 100, 0, 100);
  oHCal_hist = new TH1F("oHCal_energy", "Outer HCal Energy Distribution", 100, 0, 100);
  // Return EVENT_OK if at least one tower had non-zero energy
   for(int j = 0; j < tower_range_i; j++){

    double energy = hcalTowerContainer_i -> get_tower_at_channel(j) -> get_energy();
    iHCal_hist->Fill(energy);
    if(energy > adccut_i){
      itowerF[j]++;
      goodevent = true; //counter of events with nonzero IHCal energy
    }				
    std::cout << energy << std::endl;
    itowerE[j]+=energy;
   }
  
  //	int tower_range_o = hcalTowerContainer_o->size();
  for(int j = 0; j < tower_range_o; j++){
    
    double energy = hcalTowerContainer_o -> get_tower_at_channel(j) -> get_energy();
    oHCal_hist->Fill(energy);
    if(energy > adccut_o){
      otowerF[j]++;
      goodevent = true; //counter of events with nonzero OHCal energy
    }
    std::cout << energy << std::endl;
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
void hcal_towerid::FillHistograms(const int runnumber, const int segment)
{
  const int nBins = 101;
  
  out = new TFile(Outfile.c_str(),"RECREATE");
 
  Fspec_i = new TH2F("Fspec_i","Fspec_i",nTowers+1,-0.5,nTowers+0.5,nBins,0.-inner_f/(2*nBins),inner_f+inner_f/(2*nBins));
  Fspec_o = new TH2F("Fspec_o","Fspec_o",nTowers+1,-0.5,nTowers+0.5,nBins,0.-outer_f/(2*nBins),outer_f+outer_f/(2*nBins));
  
  Fspeci_i = new TH2F("Fspeci_i","Fspeci_i",nTowers+1,-0.5,nTowers+0.5,nBins,0.-1/(2*nBins),1);
  Fspeci_o = new TH2F("Fspeci_o","Fspeci_o",nTowers+1,-0.5,nTowers+0.5,nBins,0.-1/(2*nBins),1);
  
  Espec_i = new TH2F("Espec_i","Espec_i",nTowers+1,-0.5,nTowers-0.5,nBins,0.-1/(2*nBins),1);
  Espec_o = new TH2F("Espec_o","Espec_o",nTowers+1,-0.5,nTowers-0.5,nBins,0.-1/(2*nBins),1);
  for(int i = 0; i < nTowers; i++){
    
    Fspec_i->Fill(i,itowerF[i]/goodevents);
    Espec_i->Fill(i,itowerE[i]/goodevents);
    Fspeci_i->Fill(i,itowerF[i]/goodevents);
    
    Fspec_o->Fill(i,otowerF[i]/goodevents);
    Espec_o->Fill(i,otowerE[i]/goodevents);
    Fspeci_o->Fill(i,otowerF[i]/goodevents);
  }

  out ->cd();
  
  Fspec_i->Write();
  delete Fspec_i;
  
  Fspeci_i->Write();
  delete Fspeci_i;
  
  Espec_i->Write();
  delete Espec_i;

  Fspec_o->Write();
  delete Fspec_o;

  Fspeci_o->Write();
  delete Fspeci_o;
  
  Espec_o->Write();
  delete Espec_o;

  iHCal_hist->Write();
  delete iHCal_hist;

  oHCal_hist->Write();
  delete oHCal_hist;
   
}
//__________________________

void hcal_towerid::CalculateCutOffs(const int runnumber)
{
  TFile *histsIn = new TFile(Form("output/DST_CALOR-%08d_badTowerMapTree.root",runnumber));
  //  TFile *histsIn = new TFile(Form("DST_CALOR_badTowerMapTree.root"));
  
  float cutoffFreq_i;
  float cutoffFreq_i_lo;
 
  float cutoffFreq_o;
  float cutoffFreq_o_lo;
  
  TH1F *dummyProj = NULL;
  gStyle -> SetOptStat(0);
  Fspec_i = (TH2F*)histsIn -> Get("Fspec_i");
  dummyProj = (TH1F*)Fspec_i -> ProjectionY("dummy");
  dummyProj -> SetBinContent(1,0);
  cutoffFreq_i = dummyProj->GetStdDev()*sigmas_hi + dummyProj->GetBinCenter(dummyProj->GetMaximumBin());
  cutoffFreq_i_lo = -1*dummyProj->GetStdDev()*sigmas_lo + dummyProj->GetBinCenter(dummyProj->GetMaximumBin());
  
  Fspec_o = (TH2F*)histsIn -> Get("Fspec_o");
  dummyProj = (TH1F*)Fspec_o -> ProjectionY("dummy");
  dummyProj -> SetBinContent(1,0);
  cutoffFreq_o = dummyProj->GetStdDev()*sigmas_hi + dummyProj->GetBinCenter(dummyProj->GetMaximumBin());
  cutoffFreq_o_lo = -1*Fspec_o->GetStdDev()*sigmas_lo + dummyProj->GetBinCenter(dummyProj->GetMaximumBin());
  
  Fspeci_i = (TH2F*)histsIn -> Get("Fspeci_i");
  Fspeci_i -> SetTitle("Tower ID; Hit Frequency");
  
  Fspeci_o = (TH2F*)histsIn -> Get("Fspeci_o");
  Fspeci_o -> SetTitle("Tower ID; Hit Frequency");
  
  for(int i = 0; i < nTowers; i++){ 

    TH1F *fSpecProj_i = (TH1F*)Fspeci_i->ProjectionY("dummyi",i+1,i+1);
  //  TH1F *fSpecProj_o = (TH1F*)Fspeci_o->ProjectionY("dummyo",i+1,i+1);
    
    if(fSpecProj_i->GetBinCenter(fSpecProj_i->GetMaximumBin()) > cutoffFreq_i){
      ihottowers[i]++;
    }
    else if(fSpecProj_i -> GetBinCenter(fSpecProj_i->GetMaximumBin()) == 0){
      ideadtowers[i]++;
  }
    else if (fSpecProj_i->GetBinCenter(fSpecProj_i->GetMaximumBin()) < cutoffFreq_i_lo &&  fSpecProj_i->GetBinCenter(fSpecProj_i->GetMaximumBin()) >0 ){
      icoldtowers[i]++;
    }
  }

  for(int i = 0; i < nTowers; i++){
    TH1F *fSpecProj_o = (TH1F*)Fspeci_o->ProjectionY("dummyo",i+1,i+1); 
    if(fSpecProj_o->GetBinCenter(fSpecProj_o->GetMaximumBin()) > cutoffFreq_o){
      ohottowers[i]++;
  }
    else if(fSpecProj_o -> GetBinCenter(fSpecProj_o->GetMaximumBin()) == 0){
      odeadtowers[i]++;
    }
    else if (fSpecProj_o->GetBinCenter(fSpecProj_o->GetMaximumBin()) < cutoffFreq_o_lo &&  fSpecProj_o->GetBinCenter(fSpecProj_o->GetMaximumBin()) >0 ){
      ocoldtowers[i]++;
    }
  }
  //adding
  hot_regions = 0;
  cold_regions = 0;
  std::cout << "removing hot/cold regions" << std::endl;
  while(hot_regions == 1 || cold_regions ==1){
    std::cout << "hot/cold tower detected. Running another pass for hot towers" << std::endl;
    dummyProj = (TH1F*)Fspec_i -> ProjectionY("dummy");
    dummyProj -> SetBinContent(1,0);
    cutoffFreq_i = dummyProj->GetStdDev()*sigmas_hi + dummyProj->GetBinCenter(dummyProj->GetMaximumBin());
    cutoffFreq_i_lo = -1*dummyProj->GetStdDev()*sigmas_lo + dummyProj->GetBinCenter(dummyProj->GetMaximumBin());
    
    dummyProj = (TH1F*)Fspec_o -> ProjectionY("dummy");
    dummyProj -> SetBinContent(1,0);
    cutoffFreq_o = dummyProj->GetStdDev()*sigmas_hi + dummyProj->GetBinCenter(dummyProj->GetMaximumBin());
    cutoffFreq_o_lo = -1*Fspec_o->GetStdDev()*sigmas_lo + dummyProj->GetBinCenter(dummyProj->GetMaximumBin());
    for(int i = 0; i < nTowers; i++){
      
      TH1F *fSpecProj_i = (TH1F*)Fspeci_i->ProjectionY("dummyi",i+1,i+1);
    //    TH1F *fSpecProj_o = (TH1F*)Fspeci_o->ProjectionY("dummyo",i+1,i+1);

      if(fSpecProj_i->GetBinCenter(fSpecProj_i->GetMaximumBin()) > cutoffFreq_i){
	ihottowers[i]++;
      }
      else if(fSpecProj_i -> GetBinCenter(fSpecProj_i->GetMaximumBin()) == 0){
	ideadtowers[i]++;
      }
      else if (fSpecProj_i->GetBinCenter(fSpecProj_i->GetMaximumBin()) < cutoffFreq_i_lo &&  fSpecProj_i->GetBinCenter(fSpecProj_i->GetMaximumBin()) >0 ){
	icoldtowers[i]++;
      }
    }
    for(int i = 0; i < nTowers; i++){
      TH1F *fSpecProj_o = (TH1F*)Fspeci_o->ProjectionY("dummyo",i+1,i+1);
      if(fSpecProj_o->GetBinCenter(fSpecProj_o->GetMaximumBin()) > cutoffFreq_o){
	ohottowers[i]++;
      }
      else if(fSpecProj_o -> GetBinCenter(fSpecProj_o->GetMaximumBin()) == 0){
	odeadtowers[i]++;
      }
      else if (fSpecProj_o->GetBinCenter(fSpecProj_o->GetMaximumBin()) < cutoffFreq_o_lo &&  fSpecProj_o->GetBinCenter(fSpecProj_o->GetMaximumBin()) >0 ){
	ocoldtowers[i]++;
      }
    }
    
    hot_regions = 0;
    cold_regions = 0;
  } 
    std::cout << "towerid::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
    std::cout << "Inner Cutoff Frequency: " << cutoffFreq_i << std::endl;
    std::cout << "Outer Cutoff Frequency: " << cutoffFreq_o << std::endl;
}
//__________________________
  void hcal_towerid::WriteCDBTree(const int runnumber)
  {
    //    TFile *histsIn = new TFile(Form("output/%d/DST_CALOR-%08d_badTowerMapTree.root",runnumber,runnumber));
    TFile *cdbOut = new TFile(Form("cdbMaps/DST_CALOR-%08d_badTowerMapCDBTTree.root",runnumber),"RECREATE");
    //  TFile *cdbOut = new TFile(Form("badTowerMapCDBTTree.root"),"RECREATE");
    //TFile *cdbOut = new TFile(Form("/sphenix/u/shumail0001/analysis/JS-Jet/HCalHotTowerFinder/macro/HCAL-%08d_badTowerMapCDBTTree.root",runnumber),"RECREATE");
    
    Tinner = new TTree("T_inner","T_inner");
    Tinner -> Branch("hot_channels_i",&hot_channels_i);
    Touter = new TTree("T_outer","T_outer");
    Touter -> Branch("hot_channels_o",&hot_channels_o);

    CDBTTree*icdbttree = new CDBTTree(icdbtreename);
    CDBTTree*ocdbttree = new CDBTTree(ocdbtreename);
    std::string fieldname = "status";
    for(int i = 0; i<nTowers; i++){

    unsigned int key = TowerInfoDefs::encode_hcal(i);
    
      if(ihottowers[i] >= 0.5){
	hot_channels_i = 2;
	Tinner->Fill();
       	icdbttree->SetIntValue(key,fieldname,2);
      }
      else if(ideadtowers[i] >= 0.5){
	hot_channels_i = 1;
	Tinner->Fill();
	icdbttree->SetIntValue(key,fieldname,1);
      }
      else if(icoldtowers[i] > 0.5){
	hot_channels_i = 3;
	Tinner->Fill();
       	icdbttree->SetIntValue(key,fieldname,3);
      }
      else{
	hot_channels_i = 0;
	Tinner->Fill();
       	icdbttree->SetIntValue(key,fieldname,0);
      }
    }
      for(int i = 0; i<nTowers; i++){

	unsigned int key1 = TowerInfoDefs::encode_hcal(i);

	if(ohottowers[i] >= 0.5){
	  hot_channels_o = 2;
	  Touter->Fill();
	  ocdbttree->SetIntValue(key1,fieldname,2);
	}
	else if(odeadtowers[i] >= 0.5){
	  hot_channels_o = 1;
	  Touter->Fill();
	  ocdbttree->SetIntValue(key1,fieldname,1);
	}
	else if(ocoldtowers[i] > 0.5){
	  hot_channels_o = 3;
	  Touter->Fill();
      	  ocdbttree->SetIntValue(key1,fieldname,3);
	}
	else{
	  hot_channels_o = 0;
	  Touter->Fill();
   	  ocdbttree->SetIntValue(key1,fieldname,0);
	}
      }


  
    cdbOut->cd();
    Tinner -> Write();
    icdbttree->Commit();
    //icdbttree->Print();
    icdbttree->WriteCDBTTree();
    delete icdbttree;
    //cdbOut->Close();

    Touter -> Write();
    ocdbttree->Commit();
    //ocdbttree->Print()
    ocdbttree->WriteCDBTTree();
    delete ocdbttree;
    cdbOut->Close();

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
 int hcal_towerid::End(PHCompositeNode *topNode)
 {
   std::cout << "towerid::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
   return Fun4AllReturnCodes::EVENT_OK;
 }
 //____________________________________________________
 int hcal_towerid::EndRun(int runnumber)
 {
   std::cout << "towerid::EndRun: this is the end of run: "<< runnumber << std::endl;
   return Fun4AllReturnCodes::EVENT_OK;
 }
 //____________________________________________________

