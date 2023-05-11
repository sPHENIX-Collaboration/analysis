#include <iostream>
#include "JetSeedCount.h"

#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/PHTFileServer.h>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include <g4jets/JetMap.h>
#include <g4jets/Jetv1.h>

#include <centrality/CentralityInfo.h>
#include <ffaobjects/EventHeader.h>
#include <ffaobjects/EventHeaderv1.h>

#include <calobase/RawTower.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTowerGeomContainer.h>

#include <jetbackground/TowerBackground.h>

//#include <TTree.h> 
#include <TCanvas.h>
#include <TH2.h>

JetSeedCount::JetSeedCount(const std::string& recojetname, const std::string& truthjetname, const std::string& outputfilename):
 SubsysReco("JetSeedCount_" + recojetname + " " + truthjetname)
 , m_recoJetName(recojetname)
 , m_truthJetName(truthjetname)
 , m_outputFileName(outputfilename)
 , m_etaRange(-1,1)
 , m_ptRange(5, 100)
{
  //std::cout << "JetSeedCount::JetSeedCount(const std::string &name) Calling ctor" << std::endl;
}

JetSeedCount::~JetSeedCount()
{
  //std::cout << "JetSeedCount::~JetSeedCount() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
int JetSeedCount::Init(PHCompositeNode *topNode)
{
  std::cout << "JetSeedCount::Init(PHCompositeNode *topNode) Initializing" << std::endl;
  std::cout << "Opening output file named " << m_outputFileName << std::endl;
  PHTFileServer::get().open(m_outputFileName, "RECREATE");
   
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetSeedCount::InitRun(PHCompositeNode *topNode)
{
  std::cout << "JetSeedCount::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetSeedCount::process_event(PHCompositeNode *topNode)
{
  //std::cout << "JetSeedCount::process_event(PHCompositeNode *topNode) Processing Event" << std::endl;
  ++m_event;
  /*
  if((m_event % 100) == 0){
     std::cout << "Event number = " << m_event << std::endl;
  }
  
  JetMap* jets = findNode::getClass<JetMap>(topNode, m_recoJetName);
  if(!jets){
     std::cout << "JetSeedCount::process_event - Error can not find jets" << std::endl;
     exit(-1);
  }
  */

  // Calling Raw Jet Seeds  
  JetMap* seedjetsraw = findNode::getClass<JetMap>(topNode, "AntiKt_TowerInfo_HIRecoSeedsRaw_r02");
  if (!seedjetsraw){
     std::cout << "JetSeedCount::process_event - Error can not find DST raw seed jets" << std::endl;
     exit(-1);
  }
  
  // Calling Sub jet Seeds
  JetMap* seedjetssub = findNode::getClass<JetMap>(topNode, "AntiKt_TowerInfo_HIRecoSeedsSub_r02");
  if (!seedjetssub){
     std::cout << "JetSeedCount::process_event - Error can not find DST sub seed jets" << std::endl;
     exit(-1);
  }
 
  // Calling Centrality Info 
  CentralityInfo* cent_node = findNode::getClass<CentralityInfo>(topNode, "CentralityInfo");
  if (!cent_node){
     std::cout << "JetSeedCount::process_event - Error can not find CentralityInfo" << std::endl;
     exit(-1);
  }
  
  // Saving Centrality Info
  //float cent_epd = cent_node->get_centile(CentralityInfo::PROP::epd_NS);
  float cent_mbd = cent_node->get_centile(CentralityInfo::PROP::mbd_NS);  

  m_centrality.push_back(cent_mbd);
  //m_centrality_diff.push_back(cent_mbd);
  //std::cout << cent_node->get_centile(CentralityInfo::PROP::mbd_NS) << std::endl; 
 
   //Raw Seed Count
  m_seed_raw = 0;
  for (JetMap::Iter iter = seedjetsraw->begin(); iter != seedjetsraw->end(); ++iter){
     Jet* jet = iter->second;
     int passesCut = jet->get_property(Jet::PROPERTY::prop_SeedItr);
     if (passesCut == 1){
        m_rawenergy.push_back(jet->get_e());
        m_rawcent.push_back(cent_mbd);
        m_seed_raw++;
     }
  }
  m_raw_counts.push_back(m_seed_raw); 
 
  //Sub Seed Count
  m_seed_sub = 0;
  for (JetMap::Iter iter = seedjetssub->begin(); iter != seedjetssub->end(); ++iter){
     Jet* jet = iter->second;
     int passesCut = jet->get_property(Jet::PROPERTY::prop_SeedItr);
     if (passesCut == 2){
        m_subenergy.push_back(jet->get_e());
        m_subcent.push_back(cent_mbd);
        m_seed_sub++;
     }
  }
  
  //Checks
  /*
  if(m_seed_raw < m_seed_sub){
    std::cout<<"Event " << m_event << " has more raw seeds then sub seeds" << std::endl;
    std::cout<<"This Event has " << m_seed_raw << " raw seeds and " << m_seed_sub << " sub seeds" << std::endl;
    std::cout<<"The Centrality of this Event is " << cent_mbd << std::endl;
  }
  
  if(m_seed_sub > 10){
    std::cout<<"Event " << m_event << " has more than 10 sub seeds" << std::endl;
    std::cout<<"This Event has " << m_seed_raw << " raw seeds and " << m_seed_sub << " sub seeds" << std::endl;
    //std::cout<<"The Centrality of this Event is " << cent_mbd << std::endl;
  }
  */ 
  m_sub_counts.push_back(m_seed_sub);
  return Fun4AllReturnCodes::EVENT_OK;  
}
  

//____________________________________________________________________________..
int JetSeedCount::ResetEvent(PHCompositeNode *topNode)
{
  //std::cout << "JetSeedCount::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetSeedCount::EndRun(const int runnumber)
{
  //std::cout << "JetSeedCount::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetSeedCount::End(PHCompositeNode *topNode)
{
  std::cout << "JetSeedCount::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  PHTFileServer::get().cd(m_outputFileName);
  
  // Saving Histos
  TH1F *hRawSeedCount = new TH1F("hRawSeedCount", "Raw Seed Count per Event", 100, 0.00, 50.00);
  hRawSeedCount->GetXaxis()->SetTitle("Raw Seed Count per Event");
  hRawSeedCount->GetYaxis()->SetTitle("Number of Entries");
  for (int j = 0; j < (int)m_raw_counts.size(); j++){
     hRawSeedCount->Fill(m_raw_counts.at(j));
  }

  TH1F *hSubSeedCount = new TH1F("hSubSeedCount", "Sub Seed Count per Event", 100, 0.00, 50.00);
  hSubSeedCount->GetXaxis()->SetTitle("Sub Seed Count per Event");
  hSubSeedCount->GetYaxis()->SetTitle("Number of Entries");
  for (int j = 0; j < (int)m_sub_counts.size(); j++){
     hSubSeedCount->Fill(m_sub_counts.at(j));
  }

  TH2F *hRawSeedEnergyVsCent = new TH2F("hRawSeedEnergyVsCent", "Raw Seed Energy Vs Centrality", 10.00, 0.00, 100.00, 100, 0.00, 50.00);
  hRawSeedEnergyVsCent->GetXaxis()->SetTitle("Centrality");
  hRawSeedEnergyVsCent->GetYaxis()->SetTitle("RawSeedEnergy");
  for (int j = 0; j < (int)m_rawenergy.size(); j++){
     hRawSeedEnergyVsCent->Fill(m_rawcent.at(j), m_rawenergy.at(j));
  }

  TH2F *hSubSeedEnergyVsCent = new TH2F("hSubSeedEnergyVsCent", "Sub Seed Energy Vs Centrality", 10.00, 0.00, 100.00, 100, 0.00, 50.00);
  hSubSeedEnergyVsCent->GetXaxis()->SetTitle("Centrality");
  hSubSeedEnergyVsCent->GetYaxis()->SetTitle("SubSeedEnergy");
  for (int j = 0; j < (int)m_subenergy.size(); j++){
     hSubSeedEnergyVsCent->Fill(m_subcent.at(j), m_subenergy.at(j));
  }
  /*
  TH1F *hCentEpd = new TH1F("hCentEpd", "hCentEpd", 10, 0.00, 100.00);
  hCentEpd->GetXaxis()->SetTitle("Centrality (Epd)");
  hCentEpd->GetYaxis()->SetTitle("Number of Entries");
  for (int j = 0; j < (int)m_centrality.size(); j++){
     hCentEpd->Fill(m_centrality.at(j));
  }
 */
 TH1F *hCentMbd = new TH1F("hCentMbd", "hCentMbd", 10, 0.00, 100.00);
  hCentMbd->GetXaxis()->SetTitle("Centrality (Mbd)");
  hCentMbd->GetYaxis()->SetTitle("Number of Entries");
  for (int j = 0; j < (int)m_centrality.size(); j++){
     hCentMbd->Fill(m_centrality.at(j));
  } 

  TH2F *hRawSeedVsCent = new TH2F("hRawSeedVsCent", "Raw Seed Vs Centrality", 10, 0.00, 100.00, 10, 0.00, 100.00);
  hRawSeedVsCent->GetXaxis()->SetTitle("Centrality");
  hRawSeedVsCent->GetYaxis()->SetTitle("Raw Seed Count");
  //std::cout << (int)m_centrality.size() << std::endl;
  for (int j = 0; j < (int)m_raw_counts.size(); j++){ 
     //std::cout << m_centrality.at(j) << " " << m_raw_counts.at(j) << std::endl;
     hRawSeedVsCent->Fill(m_centrality.at(j), m_raw_counts.at(j));
  }
 
  /*
  TH2F *hCentEpdVsCentMbd = new TH2F("hCentEpdVsCentMbd", "Cent Epd Vs Cent Mbd", 10, 0.00, 100.00, 10, 0.00, 100.00);
  hCentEpdVsCentMbd->GetXaxis()->SetTitle("Centrality Epd");
  hCentEpdVsCentMbd->GetYaxis()->SetTitle("Centrality Mbd");
  for (int j = 0; j < (int)m_centrality.size(); j++){
     //std::cout << m_centrality.at(j) << " " << m_sub_counts.at(j) << std::endl;
     hCentEpdVsCentMbd->Fill(m_centrality.at(j), m_centrality_diff.at(j));
  }
  */

  TH2F *hSubSeedVsCent = new TH2F("hSubSeedVsCent", "Sub Seed Vs Centrality", 10, 0.00, 100.00, 100, 0.00, 100.00);
  hSubSeedVsCent->GetXaxis()->SetTitle("Centrality");
  hSubSeedVsCent->GetYaxis()->SetTitle("Sub Seed Count");
  for (int j = 0; j < (int)m_sub_counts.size(); j++){
     //std::cout << m_centrality.at(j) << " " << m_sub_counts.at(j) << std::endl;
     hSubSeedVsCent->Fill(m_centrality.at(j), m_sub_counts.at(j));
  }
  
  hRawSeedCount->Write();
  hSubSeedCount->Write();
  hRawSeedEnergyVsCent->Write();
  hSubSeedEnergyVsCent->Write();
  hCentMbd->Write();
  //hCentEpd->Write();
  hRawSeedVsCent->Write();
  hSubSeedVsCent->Write();
  //hCentEpdVsCentMbd->Write();

  return Fun4AllReturnCodes::EVENT_OK;
  
}

//____________________________________________________________________________..
int JetSeedCount::Reset(PHCompositeNode *topNode)
{
 //std::cout << "JetSeedCount::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void JetSeedCount::Print(const std::string &what) const
{ 
  //std::cout << "JetSeedCount::Print(const std::string &what) const Printing info for " << what << std::endl;
}
