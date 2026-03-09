#include <cassert>
#include <iostream>

#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/PHTFileServer.h>

#include <phool/getClass.h>
#include <phool/PHCompositeNode.h>

//#include <calotrigger/TriggerAnalyzer.h>
#include <ffarawobjects/Gl1Packet.h>
#include <globalvertex/GlobalVertexMapv1.h>

#include <jetbase/JetContainerv1.h>
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfo.h>

#include <TSQLServer.h>
#include <TSQLResult.h>
#include <TSQLRow.h>

#include <jetbase/FastJetAlgo.h>

#include "TH1D.h"
#include "TH3F.h"
#include "TH2D.h"

//module components
#include "Ana_PPG09_Mod.h"


//____________________________________________________________________________..
Ana_PPG09_Mod::Ana_PPG09_Mod(const std::string &recojetname, const std::string& outputfilename):
 SubsysReco("PPG09_Mod_"+recojetname)
 , m_recoJetName(recojetname)
 , m_outputFileName(outputfilename)
{
  if(Verbosity() > 5){
     std::cout << "Ana_PPG09_Mod::Ana_PPG09_Mod(const std::string &name) Calling ctor" << std::endl;
  }
}

//____________________________________________________________________________..
Ana_PPG09_Mod::~Ana_PPG09_Mod()
{
  if(Verbosity() > 5){
     std::cout << "Ana_PPG09_Mod::~Ana_PPG09_Mod() Calling dtor" << std::endl;
  }
}

//____________________________________________________________________________..
int Ana_PPG09_Mod::Init(PHCompositeNode *topNode)
{
  std::cout << "Ana_PPG09_Mod::Init(PHCompositeNode *topNode) Initializing" << std::endl;
  PHTFileServer::get().open(m_outputFileName, "RECREATE");
  std::cout << "Ana_PPG09_Mod::Init - Output to " << m_outputFileName << std::endl;
  std::cout << "Chosen Jet Cuts: Lead Pt >= " << Lead_RPt_Cut << " GeV, All Pt >= " << All_RPt_Cut << " GeV, |Z-Vertex| <= " << ZVtx_Cut << ", Num. of Constits. > " << NComp_Cut << std::endl;

  char hname[99];


  //Constructing Histograms
  for(int i = 0; i < nTrigs; i++){
     //Jet Plots
     sprintf(hname,"h_Eta_Phi_Pt_%d",i);
     h_Eta_Phi_Pt_[i] = new TH3F(hname,hname,22,-1.1,1.1,64,-3.2,3.2,99,1,100);
     
     //OHCal Tower Plots
     sprintf(hname,"h_oHCal_CS_Eta_Phi_E_%d",i);
     h_oHCal_CS_Eta_Phi_E_[i] = new TH3F(hname,hname,24,-0.5,23.5,64,-0.5,63.5,200,-100,100);
     sprintf(hname,"h_oHCal_C_Eta_Phi_E_%d",i);
     h_oHCal_C_Eta_Phi_E_[i] = new TH3F(hname,hname,24,-0.5,23.5,64,-0.5,63.5,200,-100,100);
     sprintf(hname,"h_oHCal_Raw_Eta_Phi_E_%d",i);
     h_oHCal_Raw_Eta_Phi_E_[i] = new TH3F(hname,hname,24,-0.5,23.5,64,-0.5,63.5,200,-100,100);
     sprintf(hname,"h_oHCal_Jet_Eta_Phi_E_%d",i);
     h_oHCal_Jet_Eta_Phi_E_[i] = new TH3F(hname,hname,24,-0.5,23.5,64,-0.5,63.5,200,-100,100);
     sprintf(hname,"h_oHCal_TE_Sub_Eta_Phi_E_%d",i);
     h_oHCal_TE_Sub_Eta_Phi_E_[i] = new TH3F(hname,hname,24,-0.5,23.5,64,-0.5,63.5,12000,-600,600);

     //IHCal Tower Plots
     sprintf(hname,"h_iHCal_CS_Eta_Phi_E_%d",i);
     h_iHCal_CS_Eta_Phi_E_[i] = new TH3F(hname,hname,24,-0.5,23.5,64,-0.5,63.5,200,-100,100);
     sprintf(hname,"h_iHCal_C_Eta_Phi_E_%d",i);
     h_iHCal_C_Eta_Phi_E_[i] = new TH3F(hname,hname,24,-0.5,23.5,64,-0.5,63.5,200,-100,100);
     sprintf(hname,"h_iHCal_Raw_Eta_Phi_E_%d",i);
     h_iHCal_Raw_Eta_Phi_E_[i] = new TH3F(hname,hname,24,-0.5,23.5,64,-0.5,63.5,200,-100,100);
     sprintf(hname,"h_iHCal_Jet_Eta_Phi_E_%d",i);
     h_iHCal_Jet_Eta_Phi_E_[i] = new TH3F(hname,hname,24,-0.5,23.5,64,-0.5,63.5,200,-100,100);
     sprintf(hname,"h_iHCal_TE_Sub_Eta_Phi_E_%d",i);
     h_iHCal_TE_Sub_Eta_Phi_E_[i] = new TH3F(hname,hname,24,-0.5,23.5,64,-0.5,63.5,12000,-600,600);

     //EMCal Tower/Retower Plots
     sprintf(hname,"h_EMCal_CS_Eta_Phi_E_%d",i);
     h_EMCal_CS_Eta_Phi_E_[i] = new TH3F(hname,hname,24,-0.5,23.5,64,-0.5,63.5,200,-100,100);
     sprintf(hname,"h_EMCal_CR_Eta_Phi_E_%d",i);
     h_EMCal_CR_Eta_Phi_E_[i] = new TH3F(hname,hname,24,-0.5,23.5,64,-0.5,63.5,200,-100,100);
     sprintf(hname,"h_EMCal_C_Eta_Phi_E_%d",i);
     h_EMCal_C_Eta_Phi_E_[i] = new TH3F(hname,hname,96,-0.5,95.5,256,-0.5,255.5,200,-100,100);
     sprintf(hname,"h_EMCal_Raw_Eta_Phi_E_%d",i);
     h_EMCal_Raw_Eta_Phi_E_[i] = new TH3F(hname,hname,96,-0.5,95.5,256,-0.5,255.5,200,-100,100);
     sprintf(hname,"h_EMCal_Jet_Eta_Phi_E_%d",i);
     h_EMCal_Jet_Eta_Phi_E_[i] = new TH3F(hname,hname,24,-0.5,23.5,64,-0.5,63.5,200,-100,100);
     sprintf(hname,"h_EMCal_TE_Sub_Eta_Phi_E_%d",i);
     h_EMCal_TE_Sub_Eta_Phi_E_[i] = new TH3F(hname,hname,24,-0.5,23.5,64,-0.5,63.5,12000,-600,600);

     //Event Plots
     sprintf(hname,"h_ZVtx_%d",i);
     h_ZVtx_[i] = new TH1D(hname,hname,200,-100,100);
  }
  
  h_nJetsAboveThresh = new TH2D("h_nJetsAboveThresh","h_nJetsAboveThresh",50,0.,50.,2000,4000.,6000.);
  h_EventCount = new TH1D("h_EventCount","Events",5,-2.5,2.5);
  h_theJetSpectrum = new TH1F("h_JetSpectrum","h_JetSpectrum",nJetBins, jetBins);
  
  //grab all trigger information
  initializePrescaleInformationFromDB(m_runnumber);
  
  std::cout << "Ana_PPG09_Mod::init - Initialization completed" << std::endl;
  std::cout << "scaleDowns vector size: "<< scaleDowns.size() << std::endl;
  for(int i = 0; i < (int)scaleDowns.size(); i++)
    {
      std::cout << "scaleDowns[" << i << "]: " << scaleDowns[i] << std::endl;
    }
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int Ana_PPG09_Mod::process_event(PHCompositeNode *topNode)
{
  
  m_event++;
  if((m_event % 1000) == 0)  std::cout << "Ana_PPG09_Mod::process_event - Event number = " << m_event << std::endl;

  h_EventCount->Fill(-1);

  GlobalVertexMap *global_vtxmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
  if(!global_vtxmap || global_vtxmap->empty()){
     if(Verbosity() > 0){std::cout << "Aborted Event number = " << m_event << ", no global node" << std::endl;}  
     return Fun4AllReturnCodes::ABORTEVENT;
  }

  GlobalVertex *vtx = global_vtxmap->begin()->second;
  float vtx_z = NAN;
  if(!vtx){
     if(Verbosity() > 0){std::cout << "Aborted Event number = " << m_event << ", no z-vertex" << std::endl;}
     return Fun4AllReturnCodes::ABORTEVENT;
  }
  else{
     vtx_z = vtx->get_z();
     if(vtx_z < -ZVtx_Cut || vtx_z > ZVtx_Cut){
        if(Verbosity() > 0){std::cout << "Aborted Event number = " << m_event << ", z-vertex out of range" << std::endl;}
        return Fun4AllReturnCodes::ABORTEVENT;
     }
  }

  ///Recording Event Triggers
  //triggeranalyzer->decodeTriggers(topNode);
  std::vector<int> m_triggers;
  Gl1Packet *gl1Packet = findNode::getClass<Gl1Packet>(topNode, "GL1Packet");
  if(gl1Packet){
     auto scaled_vector = gl1Packet->getScaledVector();
     for(int i = 0; i < 32; i++){
        if((scaled_vector & (int)std::pow(2,i)) != 0){
           m_triggers.push_back(i);
        }
     }
  }
  else{
     std::cout << "gl1Packet not found" << std::endl;
  }
  

  JetContainer* jets_Cont = findNode::getClass<JetContainer>(topNode, m_recoJetName);
  if(!jets_Cont){std::cout << "Jets are Missing !" << std::endl;}

  //Checking 
  double Lead_Check = 0;
  int jetAboveThresh = 0;
  for(unsigned int ijet = 0; ijet < jets_Cont->size(); ++ijet){
     Jet* jet = jets_Cont->get_jet(ijet);
     if(jet->get_pt() > Lead_Check){Lead_Check = jet->get_pt();}
     if(jet->get_pt() > 8){jetAboveThresh++;}
  }

  h_nJetsAboveThresh -> Fill(m_runnumber,jetAboveThresh);
  if(Lead_Check < Lead_RPt_Cut){
     if(Verbosity() > 0){std::cout << "Aborted Event number = " << m_event << ", no jet above lead pt threshold" << std::endl;}
     return Fun4AllReturnCodes::ABORTEVENT;
  }

  int eventPrescale = getEventPrescale(Lead_Check, m_triggers);
  
  TowerInfoContainer* towersEM3 = findNode::getClass<TowerInfoContainer>(topNode, "TOWERS_CEMC");
  TowerInfoContainer* towersIH3 = findNode::getClass<TowerInfoContainer>(topNode, "TOWERS_HCALIN");
  TowerInfoContainer* towersOH3 = findNode::getClass<TowerInfoContainer>(topNode, "TOWERS_HCALOUT");
   if(!towersEM3 || !towersIH3 || !towersOH3){std::cout << "Raw Towers are Missing !" << std::endl;}

  /*
  TowerInfoContainer* towersEM3 = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC");
  TowerInfoContainer* towersIH3 = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALIN");
  TowerInfoContainer* towersOH3 = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT"); 
  */

  TowerInfoContainer* CtowersEM3 = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC");
  TowerInfoContainer* CtowersIH3 = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALIN");
  TowerInfoContainer* CtowersOH3 = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT");
  TowerInfoContainer* CRtowersEM3 = findNode::getClass<TowerInfoContainer>(topNode,"TOWERINFO_CALIB_CEMC_RETOWER");
  if(!CtowersEM3 || !CtowersIH3 || !CtowersOH3 || !CRtowersEM3){std::cout << "Calib Towers are Missing !" << std::endl;}

  TowerInfoContainer* CStowersEM3 = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC_RETOWER_SUB1");
  TowerInfoContainer* CStowersIH3 = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALIN_SUB1");
  TowerInfoContainer* CStowersOH3 = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT_SUB1");
  if(!CStowersEM3 || !CStowersIH3 || !CStowersOH3){std::cout << "Calib Sub. Towers are Missing !" << std::endl;}

  double Index_Plots[nTrigs] = {0};

  h_EventCount->Fill(1);

  
  
  for(unsigned int ijet = 0; ijet < jets_Cont->size(); ++ijet){
     Jet* jet = jets_Cont->get_jet(ijet);

     if(ijet == 0){
        for(int k = 0; k < (int)m_triggers.size(); k++){
           if(m_triggers.at(k) == 10){ 
              Index_Plots[0] = 1;
              h_ZVtx_[0]->Fill(vtx_z);
           }
           if(m_triggers.at(k) == 12){ 
              Index_Plots[1] = 1;
              h_ZVtx_[1]->Fill(vtx_z);
           }
           if(m_triggers.at(k) == 17){ 
              Index_Plots[2] = 1;
              h_ZVtx_[2]->Fill(vtx_z);
           }
           if(m_triggers.at(k) == 18){
              Index_Plots[3] = 1;
              h_ZVtx_[3]->Fill(vtx_z);
           } 
           if(m_triggers.at(k) == 19){ 
              Index_Plots[4] = 1;
              h_ZVtx_[4]->Fill(vtx_z);
           }
           if(m_triggers.at(k) == 21){ 
              Index_Plots[5] = 1;
              h_ZVtx_[5]->Fill(vtx_z);
           }
           if(m_triggers.at(k) == 22){ 
              Index_Plots[6] = 1;
              h_ZVtx_[6]->Fill(vtx_z);
	   }
           if(m_triggers.at(k) == 23){
              Index_Plots[7] = 1;
              h_ZVtx_[7]->Fill(vtx_z);
           }
        }

        for(int k = 0; k < nTrigs; k++){
           if(Index_Plots[k] != 1) continue;
	   //float scaleDown = scaleDowns[k];
	   for(int Det = 0; Det < 3; Det++){
              TowerInfoContainer* towersCal;
              TowerInfoContainer* towersSCal;
              TowerInfoContainer* towersRaw;
              TowerInfoContainer* towersRCal;
	      TH3F* RawTowers;
              TH3F* CalibTowers;
              TH3F* CalibSubTowers;
              TH3F* RCalibTowers;
	      TH3F* SubETowers; 

              if(Det == 0){
                 towersCal = CtowersEM3;
		 towersRCal = CRtowersEM3;
                 towersSCal = CStowersEM3;
                 towersRaw = towersEM3; 
                 RawTowers = h_EMCal_Raw_Eta_Phi_E_[k];
                 CalibTowers = h_EMCal_C_Eta_Phi_E_[k];
                 CalibSubTowers = h_EMCal_CS_Eta_Phi_E_[k];      
                 RCalibTowers = h_EMCal_CR_Eta_Phi_E_[k];
		 SubETowers = h_EMCal_TE_Sub_Eta_Phi_E_[k];
	      }          
              if(Det == 1){
                 towersCal = CtowersIH3;
                 towersSCal = CStowersIH3;
                 towersRaw = towersIH3;
                 RawTowers = h_iHCal_Raw_Eta_Phi_E_[k];
                 CalibTowers = h_iHCal_C_Eta_Phi_E_[k];
                 CalibSubTowers = h_iHCal_CS_Eta_Phi_E_[k];
                 SubETowers = h_iHCal_TE_Sub_Eta_Phi_E_[k];
	      }
              if(Det == 2){
                 towersCal = CtowersOH3;
                 towersSCal = CStowersOH3;
                 towersRaw = towersOH3;
                 RawTowers = h_oHCal_Raw_Eta_Phi_E_[k];
                 CalibTowers = h_oHCal_C_Eta_Phi_E_[k];
                 CalibSubTowers = h_oHCal_CS_Eta_Phi_E_[k];
		 SubETowers = h_oHCal_TE_Sub_Eta_Phi_E_[k];
              }              
 
              int Chan_Num = -1;
              if(Det == 0){Chan_Num = 24576;}
              else{Chan_Num = 1536;}
              
              for (int channel = 0; channel < Chan_Num; channel++){
                 //Raw Towers
                 float CTowerE = -99;
		 float CSTowerE = -99;
		      
		 TowerInfo *towerRaw = towersRaw->get_tower_at_channel(channel);
                 unsigned int channelkeyR = towersRaw->encode_key(channel);
                 int ietaR = towersRaw->getTowerEtaBin(channelkeyR);
                 int iphiR = towersRaw->getTowerPhiBin(channelkeyR);

                 //Calib Towers
                 TowerInfo *towerCal = towersCal->get_tower_at_channel(channel);
                 unsigned int channelkeyC = towersCal->encode_key(channel);
                 int ietaC = towersCal->getTowerEtaBin(channelkeyC);
                 int iphiC = towersCal->getTowerPhiBin(channelkeyC);
                 if(!Det == 0){CTowerE = towerCal->get_energy();}

                 //Calib Subtracted Towers
		 if(channel < 1536){
                    TowerInfo *towerSCal = towersSCal->get_tower_at_channel(channel);
                    unsigned int channelkeySC = towersSCal->encode_key(channel);
                    int ietaSC = towersSCal->getTowerEtaBin(channelkeySC);
                    int iphiSC = towersSCal->getTowerPhiBin(channelkeySC); 
                    CalibSubTowers->Fill(ietaSC,iphiSC,towerSCal->get_energy(),scaleDowns[k]);
                    CSTowerE = towerSCal->get_energy();

                    //Calib. Retowers
		    if(Det == 0){
	               TowerInfo *towerRCal = towersRCal->get_tower_at_channel(channel);
                       unsigned int channelkeyRC = towersRCal->encode_key(channel);
                       int ietaRC = towersRCal->getTowerEtaBin(channelkeyRC);
                       int iphiRC = towersRCal->getTowerPhiBin(channelkeyRC);
		       RCalibTowers->Fill(ietaRC,iphiRC,towerRCal->get_energy(),scaleDowns[k]); 
		       CTowerE = towerRCal->get_energy();
		    }
		 }
	
	         double SubE = CTowerE - CSTowerE;	 
		 SubETowers->Fill(ietaC,iphiC,SubE,scaleDowns[k]);
		 CalibTowers->Fill(ietaC,iphiC,towerCal->get_energy(),scaleDowns[k]);
                 RawTowers->Fill(ietaR,iphiR,towerRaw->get_energy(),scaleDowns[k]);
              }
           }
        }
     }
     
     bool Eta_Check = check_bad_jet_eta(jet->get_eta(), vtx_z, 0.4);

     if (jet->get_pt() < All_RPt_Cut || jet->size_comp() <= NComp_Cut || Eta_Check) continue;
     h_theJetSpectrum -> Fill(jet->get_pt(), eventPrescale);
     
     for(int k = 0; k < nTrigs; k++){
       //std::cout << "on trig index: " << k << "; Index_plots[k] = " << Index_Plots[k] << std::endl;
       
        if(Index_Plots[k] != 1) continue;
	//std::cout << "Scaledown value: " << scaleDowns[k] << std::endl;
        if(scaleDowns[k])h_Eta_Phi_Pt_[k]->Fill(jet->get_eta(),jet->get_phi(),jet->get_pt(),liveCounts[k]/scaleDowns[k]);
        
        auto comp_vec = jet->get_comp_vec();
        std::vector<fastjet::PseudoJet> pseudojets;
        for(auto comp = comp_vec.begin(); comp != comp_vec.end(); ++comp){
           TowerInfo *tower;
           unsigned int channel = comp->second;
           unsigned int calo = comp->first;
          
           if (calo == 15 || calo == 30 || calo == 26){
              tower = CStowersIH3->get_tower_at_channel(channel);
              unsigned int calokey = CStowersIH3->encode_key(channel);
              int ieta = CStowersIH3->getTowerEtaBin(calokey);
              int iphi = CStowersIH3->getTowerPhiBin(calokey);
              h_iHCal_Jet_Eta_Phi_E_[k]->Fill(ieta,iphi,tower->get_energy(),scaleDowns[k]);
           }
           if (calo == 16 || calo == 31 || calo == 27){
              tower = CStowersOH3->get_tower_at_channel(channel);
              unsigned int calokey = CStowersOH3->encode_key(channel);
              int ieta = CStowersOH3->getTowerEtaBin(calokey);
              int iphi = CStowersOH3->getTowerPhiBin(calokey);
              h_oHCal_Jet_Eta_Phi_E_[k]->Fill(ieta,iphi,tower->get_energy(),scaleDowns[k]);
           }
           if (calo == 14 || calo == 29 || calo == 28){
              tower = CStowersEM3->get_tower_at_channel(channel);
              unsigned int calokey = CStowersEM3->encode_key(channel);
              int ieta = CStowersEM3->getTowerEtaBin(calokey);
              int iphi = CStowersEM3->getTowerPhiBin(calokey);
              h_EMCal_Jet_Eta_Phi_E_[k]->Fill(ieta,iphi,tower->get_energy(),scaleDowns[k]);
           }
        }
     }
  }
 
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int Ana_PPG09_Mod::ResetEvent(PHCompositeNode *topNode)
{
  if(Verbosity() > 5){
     std::cout << "Ana_PPG09_Mod::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << std::endl;
  }
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int Ana_PPG09_Mod::End(PHCompositeNode *topNode)
{
  PHTFileServer::get().cd(m_outputFileName);
  std::cout << "Saving histograms" << std::endl;
  h_nJetsAboveThresh->Write();
  h_EventCount->Write();
  h_theJetSpectrum  -> Write();
  for(int k = 0; k < nTrigs; k++){
     h_Eta_Phi_Pt_[k]->Write();
     h_EMCal_Raw_Eta_Phi_E_[k]->Write();
     h_iHCal_Raw_Eta_Phi_E_[k]->Write();
     h_oHCal_Raw_Eta_Phi_E_[k]->Write();
     h_EMCal_CS_Eta_Phi_E_[k]->Write();
     h_iHCal_CS_Eta_Phi_E_[k]->Write();
     h_oHCal_CS_Eta_Phi_E_[k]->Write();
     h_EMCal_C_Eta_Phi_E_[k]->Write();
     h_EMCal_CR_Eta_Phi_E_[k]->Write();
     h_iHCal_C_Eta_Phi_E_[k]->Write();
     h_oHCal_C_Eta_Phi_E_[k]->Write();
     h_EMCal_Jet_Eta_Phi_E_[k]->Write();
     h_iHCal_Jet_Eta_Phi_E_[k]->Write();
     h_oHCal_Jet_Eta_Phi_E_[k]->Write();
     h_EMCal_TE_Sub_Eta_Phi_E_[k]->Write();
     h_iHCal_TE_Sub_Eta_Phi_E_[k]->Write();
     h_oHCal_TE_Sub_Eta_Phi_E_[k]->Write();
     h_ZVtx_[k]->Write();
  }

  scaleDowns.clear();
  liveCounts.clear();

  std::cout << "Ana_PPG09_Mod::End - Output to " << m_outputFileName << std::endl;
  if(Verbosity() > 5){
     std::cout << "Ana_PPG09_Mod::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  }
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int Ana_PPG09_Mod::Reset(PHCompositeNode *topNode)
{
  if(Verbosity() > 5){
     std::cout << "Ana_PPG09_Mod::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
  }
  return Fun4AllReturnCodes::EVENT_OK;
}
//____________________________________________________________________________..
void Ana_PPG09_Mod::initializePrescaleInformationFromDB(int runnumber)
{
  std::cout << "Starting trig DB lookup for run "<< runnumber << std::endl;
  TSQLServer* db = TSQLServer::Connect("pgsql://sphnxdaqdbreplica:5432/daq","phnxro","");
  if (!db || db->IsZombie())
    {
      std::cerr <<  "[ERROR] DB connection failed for run "
		<< runnumber << std::endl;
      if (db) delete db;
      return;
    }

  char query[512];
  snprintf(query, sizeof(query),
	   "SELECT s.index, t.triggername, s.live, s.scaled "
	   "FROM gl1_scalers s "
	   "JOIN gl1_triggernames t ON (s.index = t.index AND s.runnumber BETWEEN t.runnumber AND t.runnumber_last) "
	   "WHERE s.runnumber=%d ORDER BY s.index;", runnumber);

  auto *res = db->Query(query);
  if (!res)
    {
      std::cerr << "[ERROR] Query failed for run "
		<< runnumber  << std::endl;
      delete db;
      return;
    }
    
  //grab the prescales for each of our triggers of interest
  //for(int i = 0; i < (int)sizeof(trigNames)/sizeof(trigNames[0]); ++i)
  std::cout << "Looking for " << trigNames.size() << " triggers" << std::endl;
  for(int i = 0; i < (int)trigNames.size(); i++)
    {
      bool trigFound = false;
      std::cout << "Looking for trig " << trigNames[i] << std::endl;
      while (auto row = res->Next())
	{
	  const char* dbTrig = row->GetField(1);
	  const char* liveStr = row->GetField(2);
	  const char* scaledStr = row->GetField(3);

	  if (!dbTrig || !liveStr || !scaledStr) { delete row; continue; }

	  std::string trigName(dbTrig);
	  double live = std::atof(liveStr);
	  double scaled = std::atof(scaledStr);
	  // check map
        
	  //the trigger string matches one we're looking for
	  //NB if for some reason there are multiple triggers of the same
	  //name this would break
	  if (!strcmp(dbTrig,trigNames[i].c_str()))
	    {
	      scaleDowns.push_back(scaled);
	      liveCounts.push_back(live);
	      std::cout << "Scale for trig " << trigNames[i] << " found with value " << scaled << std::endl;
	      std::cout << "Live for trig " << trigNames[i] << " found with value " << live << std::endl;
	      trigFound = true;
	      delete row;
	      break;
	    }
	}
      
      if(!trigFound)
	{
	  std::cout << "No trigger information for trigger " << trigNames[i] << " in run " << m_runnumber << std::endl;
	  scaleDowns.push_back(0);
	  liveCounts.push_back(0);
	}
    }
  delete res; delete db;
}
//____________________________________________________________________________..
int Ana_PPG09_Mod::getEventPrescale(float leadJetPt, std::vector<int> triggerVector)
{
  int prescale = 0;  
  //need to return pre-scaled from least pre-scaled, efficient trigger
  int lowest_prescale = std::numeric_limits<int>::max();
  bool trigIsGood = false;
  int live = 0;
  for(int i = 0; i < (int)triggerVector.size(); ++i)
    {
      if(std::find(trigIndices.begin(), trigIndices.end(), triggerVector[i]) == trigIndices.end())
	{//this trigger is not in our menu
	  continue;
	}
      if(scaleDowns[trigToVecIndex[triggerVector[i]]] < lowest_prescale && isTrigEfficient(trigToVecIndex[triggerVector[i]],leadJetPt) && scaleDowns[trigToVecIndex[triggerVector[i]]] > 0)
	{
	  lowest_prescale = scaleDowns[trigToVecIndex[triggerVector[i]]];
	  live = liveCounts[trigToVecIndex[triggerVector[i]]];
	  trigIsGood = true;
	}
    }

  //a fully efficient trigger that fired during this event has been found, and its  pre-scale is the lowest, use its pre-scale
  if(lowest_prescale != std::numeric_limits<int>::max() && trigIsGood && lowest_prescale != 0) prescale = live/lowest_prescale;
  //else we should default to the MB pre-scale if the MBD trigger fired
  else if(std::find(triggerVector.begin(), triggerVector.end(), 10) != triggerVector.end() && scaleDowns[0] != 0) prescale = liveCounts[0]/scaleDowns[0];
  //else this is an event where some rare-probe trigger fired without coincidence with the MBD, so we toss it (for now)
  else prescale = 0;
  return prescale;
}
//____________________________________________________________________________..
bool Ana_PPG09_Mod::isTrigEfficient(int trigIndex, float leadJetPt)
{
  
  if(leadJetPt > trigCutOffs[trigIndex]) return true;
  return true;
}
