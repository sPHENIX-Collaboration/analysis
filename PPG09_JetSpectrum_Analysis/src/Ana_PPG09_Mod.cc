#include <cassert>
#include <iostream>

#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/PHTFileServer.h>

#include <phool/getClass.h>
#include <phool/PHCompositeNode.h>

#include <calotrigger/TriggerAnalyzer.h>
#include <globalvertex/GlobalVertexMapv1.h>

#include <jetbase/JetContainerv1.h>
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfo.h>

#include <jetbase/FastJetAlgo.h>

#include "TH1D.h"
#include "TH3F.h"

//module components
#include "Ana_PPG09_Mod.h"

float radius_OH = 225.87;
 float mineta_OH = -1.1;
 float maxeta_OH = 1.1;
 float minz_OH = -301.683;
 float maxz_OH = 301.683;

 float radius_IH = 127.503;
 float mineta_IH = -1.1;
 float maxeta_IH = 1.1;
 float minz_IH = -170.299;
 float maxz_IH = 170.299;

 float radius_EM = 93.5;
 float mineta_EM = -1.13381;
 float maxeta_EM = 1.13381;
 float minz_EM = -130.23;
 float maxz_EM = 130.23;

float get_ohcal_mineta_zcorrected(float zvertex) {
  float z = minz_OH - zvertex;
  float eta_zcorrected = asinh(z / (float)radius_OH);
  return eta_zcorrected;
}

float get_ohcal_maxeta_zcorrected(float zvertex) {
  float z = maxz_OH - zvertex;
  float eta_zcorrected = asinh(z / (float)radius_OH);
  return eta_zcorrected;
}

float get_ihcal_mineta_zcorrected(float zvertex) {
  float z = minz_IH - zvertex;
  float eta_zcorrected = asinh(z / (float)radius_IH);
  return eta_zcorrected;
}

float get_ihcal_maxeta_zcorrected(float zvertex) {
  float z = maxz_IH - zvertex;
  float eta_zcorrected = asinh(z / (float)radius_IH);
  return eta_zcorrected;
}

float get_emcal_mineta_zcorrected(float zvertex) {
  float z = minz_EM - zvertex;
  float eta_zcorrected = asinh(z / (float)radius_EM);
  return eta_zcorrected;
}

float get_emcal_maxeta_zcorrected(float zvertex) {
  float z = maxz_EM - zvertex;
  float eta_zcorrected = asinh(z / (float)radius_EM);
  return eta_zcorrected;
}

bool check_bad_jet_eta(float jet_eta, float zertex, float jet_radius) {
  float emcal_mineta = get_emcal_mineta_zcorrected(zertex);
  float emcal_maxeta = get_emcal_maxeta_zcorrected(zertex);
  float ihcal_mineta = get_ihcal_mineta_zcorrected(zertex);
  float ihcal_maxeta = get_ihcal_maxeta_zcorrected(zertex);
  float ohcal_mineta = get_ohcal_mineta_zcorrected(zertex);
  float ohcal_maxeta = get_ohcal_maxeta_zcorrected(zertex);
  float minlimit = emcal_mineta;
  if (ihcal_mineta > minlimit) minlimit = ihcal_mineta;
  if (ohcal_mineta > minlimit) minlimit = ohcal_mineta;
  float maxlimit = emcal_maxeta;
  if (ihcal_maxeta < maxlimit) maxlimit = ihcal_maxeta;
  if (ohcal_maxeta < maxlimit) maxlimit = ohcal_maxeta;
  minlimit += jet_radius;
  maxlimit -= jet_radius;
  return jet_eta < minlimit || jet_eta > maxlimit;
}

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
  std::cout << "Chosen Jet Cuts: Pt - " << RPt_Cut << " GeV, Z-Vertex - " << ZVtx_Cut << ", Num. of Constits. - " << NComp_Cut << std::endl;

  char hname[99];
  //Constructing Histograms
  for(int i = 0; i < 8; i++){
     //Jet Plots
     sprintf(hname,"h_Eta_Phi_Pt_%d",i);
     h_Eta_Phi_Pt_[i] = new TH3F(hname,hname,22,-1.1,1.1,64,-3.2,3.2,95,5,100);
     
     //OHCal Tower Plots
     sprintf(hname,"h_oHCal_CS_Eta_Phi_E_%d",i);
     h_oHCal_CS_Eta_Phi_E_[i] = new TH3F(hname,hname,24,-0.5,23.5,64,-0.5,63.5,200,-100,100);
     sprintf(hname,"h_oHCal_C_Eta_Phi_E_%d",i);
     h_oHCal_C_Eta_Phi_E_[i] = new TH3F(hname,hname,24,-0.5,23.5,64,-0.5,63.5,200,-100,100);
     sprintf(hname,"h_oHCal_Raw_Eta_Phi_E_%d",i);
     h_oHCal_Raw_Eta_Phi_E_[i] = new TH3F(hname,hname,24,-0.5,23.5,64,-0.5,63.5,200,-100,100);
     sprintf(hname,"h_oHCal_Jet_Eta_Phi_E_%d",i);
     h_oHCal_Jet_Eta_Phi_E_[i] = new TH3F(hname,hname,24,-0.5,23.5,64,-0.5,63.5,200,-100,100);
         
 
     //IHCal Tower Plots
     sprintf(hname,"h_iHCal_CS_Eta_Phi_E_%d",i);
     h_iHCal_CS_Eta_Phi_E_[i] = new TH3F(hname,hname,24,-0.5,23.5,64,-0.5,63.5,200,-100,100);
     sprintf(hname,"h_iHCal_C_Eta_Phi_E_%d",i);
     h_iHCal_C_Eta_Phi_E_[i] = new TH3F(hname,hname,24,-0.5,23.5,64,-0.5,63.5,200,-100,100);
     sprintf(hname,"h_iHCal_Raw_Eta_Phi_E_%d",i);
     h_iHCal_Raw_Eta_Phi_E_[i] = new TH3F(hname,hname,24,-0.5,23.5,64,-0.5,63.5,200,-100,100);
     sprintf(hname,"h_iHCal_Jet_Eta_Phi_E_%d",i);
     h_iHCal_Jet_Eta_Phi_E_[i] = new TH3F(hname,hname,24,-0.5,23.5,64,-0.5,63.5,200,-100,100);

     //EMCal Tower/Retower Plots
     sprintf(hname,"h_EMCal_CS_Eta_Phi_E_%d",i);
     h_EMCal_CS_Eta_Phi_E_[i] = new TH3F(hname,hname,24,-0.5,23.5,64,-0.5,63.5,200,-100,100);
     sprintf(hname,"h_EMCal_C_Eta_Phi_E_%d",i);
     h_EMCal_C_Eta_Phi_E_[i] = new TH3F(hname,hname,96,-0.5,95.5,256,-0.5,255.5,200,-100,100);
     sprintf(hname,"h_EMCal_Raw_Eta_Phi_E_%d",i);
     h_EMCal_Raw_Eta_Phi_E_[i] = new TH3F(hname,hname,96,-0.5,95.5,256,-0.5,255.5,200,-100,100);
     sprintf(hname,"h_EMCal_Jet_Eta_Phi_E_%d",i);
     h_EMCal_Jet_Eta_Phi_E_[i] = new TH3F(hname,hname,24,-0.5,23.5,64,-0.5,63.5,200,-100,100);

     //Event Plots
     sprintf(hname,"h_ZVtx_%d",i);
     h_ZVtx_[i] = new TH1D(hname,hname,200,-100,100);
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int Ana_PPG09_Mod::process_event(PHCompositeNode *topNode)
{
  if(Verbosity() > 1){
     m_event++;
     if((m_event % 100) == 0) std::cout << "Ana_PPG09_Mod::process_event - Event number = " << m_event << std::endl;
  }
 
  GlobalVertexMap *global_vtxmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
  if(!global_vtxmap || global_vtxmap->empty()){
     return Fun4AllReturnCodes::ABORTEVENT;
  }

  GlobalVertex *vtx = global_vtxmap->begin()->second;
  float vtx_z = NAN;
  if(!vtx){
     return Fun4AllReturnCodes::ABORTEVENT;
  }
  else{
     vtx_z = vtx->get_z();
     if(vtx_z < -ZVtx_Cut || vtx_z > ZVtx_Cut){return Fun4AllReturnCodes::ABORTEVENT;}
  }

  //Recording Event Triggers
  //triggeranalyzer->decodeTriggers(topNode);
  std::vector<int> m_triggers;
  Gl1Packet *gl1Packet = findNode::getClass<Gl1Packet>(topNode, "GL1Packet");
  if(gl1Packet){
     auto scaled_vector = gl1Packet->getScaledVector();
     for(int i = 0; i < 64; i++){
        if((scaled_vector & (int)std::pow(2,i)) != 0){
           m_triggers.push_back(i);
        }
     }
  }
  else{
     std::cout << "gl1Packet not found" << std::endl;
  }
  
  JetContainer* jets_Cont = findNode::getClass<JetContainer>(topNode, m_recoJetName);
  
  TowerInfoContainer* towersEM3 = findNode::getClass<TowerInfoContainer>(topNode, "TOWERS_CEMC");
  TowerInfoContainer* towersIH3 = findNode::getClass<TowerInfoContainer>(topNode, "TOWERS_HCALIN");
  TowerInfoContainer* towersOH3 = findNode::getClass<TowerInfoContainer>(topNode, "TOWERS_HCALOUT");
 
  TowerInfoContainer* CtowersEM3 = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC");
  TowerInfoContainer* CtowersIH3 = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALIN");
  TowerInfoContainer* CtowersOH3 = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT");
 
  TowerInfoContainer* CStowersEM3 = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC_RETOWER_SUB1");
  TowerInfoContainer* CStowersIH3 = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALIN_SUB1");
  TowerInfoContainer* CStowersOH3 = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT_SUB1");

  double Index_Plots[8] = {0};
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
           if(m_triggers.at(k) == 33){ 
              Index_Plots[5] = 1;
              h_ZVtx_[5]->Fill(vtx_z);
           }
           if(m_triggers.at(k) == 34){ 
              Index_Plots[6] = 1;
              h_ZVtx_[6]->Fill(vtx_z);
           }
           if(m_triggers.at(k) == 35){
              Index_Plots[7] = 1;
              h_ZVtx_[7]->Fill(vtx_z);
           }
        }
  
        for(int k = 0; k < 8; k++){
           if(Index_Plots[k] != 1) continue;
           for(int Det = 0; Det < 9; Det++){
              TowerInfoContainer* towersCal;
              if(Det == 0){towersCal = towersEM3;}          
              if(Det == 1){towersCal = towersIH3;}
              if(Det == 2){towersCal = towersOH3;}
              if(Det == 3){towersCal = CStowersEM3;}
              if(Det == 4){towersCal = CStowersIH3;}
              if(Det == 5){towersCal = CStowersOH3;}
              if(Det == 6){towersCal = CtowersEM3;}
              if(Det == 7){towersCal = CtowersIH3;}
              if(Det == 8){towersCal = CtowersOH3;}
               
              int Chan_Num = -1;
              if(Det == 0 || Det == 6){Chan_Num = 24576;}
              else{Chan_Num = 1536;}
              
              for (int channel = 0; channel < Chan_Num; channel++){
                 TowerInfo *tower = towersCal->get_tower_at_channel(channel);
                 unsigned int channelkey = towersCal->encode_key(channel);
                 int ieta = towersCal->getTowerEtaBin(channelkey);
                 int iphi = towersCal->getTowerPhiBin(channelkey);
                 if(Det == 0){h_EMCal_Raw_Eta_Phi_E_[k]->Fill(ieta,iphi,tower->get_energy());}
                 if(Det == 1){h_iHCal_Raw_Eta_Phi_E_[k]->Fill(ieta,iphi,tower->get_energy());}
                 if(Det == 2){h_oHCal_Raw_Eta_Phi_E_[k]->Fill(ieta,iphi,tower->get_energy());}
                 if(Det == 3){h_EMCal_CS_Eta_Phi_E_[k]->Fill(ieta,iphi,tower->get_energy());}
                 if(Det == 4){h_iHCal_CS_Eta_Phi_E_[k]->Fill(ieta,iphi,tower->get_energy());}
                 if(Det == 5){h_oHCal_CS_Eta_Phi_E_[k]->Fill(ieta,iphi,tower->get_energy());}
                 if(Det == 6){h_EMCal_C_Eta_Phi_E_[k]->Fill(ieta,iphi,tower->get_energy());}
                 if(Det == 7){h_iHCal_C_Eta_Phi_E_[k]->Fill(ieta,iphi,tower->get_energy());}
                 if(Det == 8){h_oHCal_C_Eta_Phi_E_[k]->Fill(ieta,iphi,tower->get_energy());}
              }
           }
        }
     }
     
     bool Eta_Check = check_bad_jet_eta(jet->get_eta(), vtx_z, 0.4);

     //Filling Jet Plots  
     if (jet->get_pt() < RPt_Cut || jet->size_comp() <= NComp_Cut || Eta_Check) continue;

     for(int k = 0; k < 8; k++){
        if(Index_Plots[k] != 1) continue;
        h_Eta_Phi_Pt_[k]->Fill(jet->get_eta(),jet->get_phi(),jet->get_pt());
        
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
              h_iHCal_Jet_Eta_Phi_E_[k]->Fill(ieta,iphi,tower->get_energy());
           }
           if (calo == 16 || calo == 31 || calo == 27){
              tower = CStowersOH3->get_tower_at_channel(channel);
              unsigned int calokey = CStowersOH3->encode_key(channel);
              int ieta = CStowersOH3->getTowerEtaBin(calokey);
              int iphi = CStowersOH3->getTowerPhiBin(calokey);
              h_oHCal_Jet_Eta_Phi_E_[k]->Fill(ieta,iphi,tower->get_energy());
           }
           if (calo == 14 || calo == 29 || calo == 28){
              tower = CStowersEM3->get_tower_at_channel(channel);
              unsigned int calokey = CStowersEM3->encode_key(channel);
              int ieta = CStowersEM3->getTowerEtaBin(calokey);
              int iphi = CStowersEM3->getTowerPhiBin(calokey);
              h_EMCal_Jet_Eta_Phi_E_[k]->Fill(ieta,iphi,tower->get_energy());
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
  for(int k = 0; k < 8; k++){
     h_Eta_Phi_Pt_[k]->Write();
     h_EMCal_Raw_Eta_Phi_E_[k]->Write();
     h_iHCal_Raw_Eta_Phi_E_[k]->Write();
     h_oHCal_Raw_Eta_Phi_E_[k]->Write();
     h_EMCal_CS_Eta_Phi_E_[k]->Write();
     h_iHCal_CS_Eta_Phi_E_[k]->Write();
     h_oHCal_CS_Eta_Phi_E_[k]->Write();
     h_EMCal_C_Eta_Phi_E_[k]->Write();
     h_iHCal_C_Eta_Phi_E_[k]->Write();
     h_oHCal_C_Eta_Phi_E_[k]->Write();
     h_EMCal_Jet_Eta_Phi_E_[k]->Write();
     h_iHCal_Jet_Eta_Phi_E_[k]->Write();
     h_oHCal_Jet_Eta_Phi_E_[k]->Write();
     h_ZVtx_[k]->Write();
  }

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
