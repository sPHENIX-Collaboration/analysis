#include "FindRejects.h"
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/RawTowerGeom.h>
#include <calotrigger/TriggerAnalyzer.h>
// Calo 
#include <calobase/TowerInfo.h>
#include <fun4all/Fun4AllHistoManager.h>
#include <jetbackground/TowerBackground.h>

#include <calobase/TowerInfoContainer.h>
#include <ffarawobjects/CaloPacketContainer.h>
#include <ffarawobjects/CaloPacket.h>
// jet includes
#include <jetbackground/TowerBackground.h>
#include <jetbase/Jet.h>
#include <jetbase/JetContainer.h>
#include <jetbase/JetContainerv1.h>
#include <jetbase/Jetv2.h>

#include <qautils/QAHistManagerDef.h>

#include <fun4all/Fun4AllHistoManager.h>
#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/getClass.h>
#include <phool/phool.h>  // for PHWHERE

#include <ffarawobjects/Gl1Packet.h>
#include <TNtuple.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TProfile2D.h>
#include <TSystem.h>

#include <boost/format.hpp>
#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/PHObject.h>
#include <phool/getClass.h>
#include <phool/phool.h>

#include <cassert>
#include <cmath>  // for log10, pow, sqrt, abs, M_PI
#include <cstdint>
#include <iostream>  // for operator<<, endl, basic_...
#include <limits>
#include <map>  // for operator!=, _Rb_tree_con...
#include <string>
#include <utility>  // for pair

FindRejects::FindRejects(const std::string& name, const std::string& outfile)
: SubsysReco(name)
, m_outfilename(outfile)

{						
  m_triggeranalyzer = new TriggerAnalyzer();
}

FindRejects::~FindRejects()
{
  
}

int FindRejects::Init(PHCompositeNode* /*unused*/)
{
  std::cout << "Leaving FindRejects::Init" << std::endl;

  tn = new TNtuple("tn","tn","event:trash:r1:energy:emcal_energy:spread");
  hm = new Fun4AllHistoManager("FindRejectsHistos");
  h_emcal = new TH2F("EMCAL","", 12, -0.5, 95.5, 32, -0.5, 255.5);
  h_hcalout = new TH2F("HCALOUT","", 24, -0.5, 23.5, 64, -0.5, 63.5);
  h_hcalout_time = new TH2F("HCALOUT_TIME","", 24, -0.5, 23.5, 64, -0.5, 63.5);
  return Fun4AllReturnCodes::EVENT_OK;
}

int FindRejects::process_event(PHCompositeNode* topNode)
{

  _eventcounter++;  
  std::cout << "In process_event" << std::endl;

  std::string recoJetName = "AntiKt_Tower_r04_Sub1";
  JetContainer *jets_4 = findNode::getClass<JetContainer>(topNode, recoJetName);
  TowerInfoContainer *hcalin_towers = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALIN");
  TowerInfoContainer *hcalout_towers = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT");
  TowerInfoContainer *emcal_towers = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC");
  TowerInfoContainer *emcalre_towers = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC_RETOWER");
  RawTowerGeomContainer *tower_geomIH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALIN");
  RawTowerGeomContainer *tower_geomOH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALOUT");

  int b_njet_4 = 0;
  std::vector<float> b_jet_et_4{};
    
  if (jets_4)
     {
       std::cout << "In jets" << std::endl;
       for (auto jet : *jets_4)
	 {
	   if (jet->get_pt() < 3) continue;
	   std::cout << "In b_njet " << b_njet_4 << std::endl;

	   float jet_total_eT = 0;

	   b_njet_4++;


	   for (auto comp : jet->get_comp_vec())
	     {
	       unsigned int channel = comp.second;
	       TowerInfo *tower;
	       float tower_eT = 0;
	       if (comp.first == 26 || comp.first == 30)
		 {  // IHcal
		   tower = hcalin_towers->get_tower_at_channel(channel);
		   if (!tower || !tower_geomIH)
		     {
		       continue;
		     }
		   if(!tower->get_isGood()) continue;

		   unsigned int calokey = hcalin_towers->encode_key(channel);

		   int ieta = hcalin_towers->getTowerEtaBin(calokey);
		   int iphi = hcalin_towers->getTowerPhiBin(calokey);
		   const RawTowerDefs::keytype key = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::HCALIN, ieta, iphi);
		   float tower_eta = tower_geomIH->get_tower_geometry(key)->get_eta();
		   tower_eT = tower->get_energy() / std::cosh(tower_eta);
		   jet_total_eT += tower_eT;
		 }
	       else if (comp.first == 27 || comp.first == 31)
		 {  // IHcal
		   tower = hcalout_towers->get_tower_at_channel(channel);

		   if (!tower || !tower_geomOH)
		     {
		       continue;
		     }
		   unsigned int calokey = hcalout_towers->encode_key(channel);
		   int ieta = hcalout_towers->getTowerEtaBin(calokey);
		   int iphi = hcalout_towers->getTowerPhiBin(calokey);
		   const RawTowerDefs::keytype key = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::HCALOUT, ieta, iphi);
		   float tower_eta = tower_geomOH->get_tower_geometry(key)->get_eta();
		   tower_eT = tower->get_energy() / std::cosh(tower_eta);
		   jet_total_eT += tower_eT;
		 }
	       else if (comp.first == 28 || comp.first == 29)
		 {  // IHcal
		   tower = emcalre_towers->get_tower_at_channel(channel);

		   if (!tower || !tower_geomIH)
		     {
		       continue;
		     }

		   unsigned int calokey = emcalre_towers->encode_key(channel);
		   int ieta = emcalre_towers->getTowerEtaBin(calokey);
		   int iphi = emcalre_towers->getTowerPhiBin(calokey);
		   const RawTowerDefs::keytype key = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::HCALIN, ieta, iphi);
		   float tower_eta = tower_geomIH->get_tower_geometry(key)->get_eta();
		   tower_eT = tower->get_energy() / std::cosh(tower_eta);
		   jet_total_eT += tower_eT;
		 }	       
	     }
	   b_jet_et_4.push_back(jet_total_eT);
	   std::cout << "   total et = " << jet_total_eT  << std::endl;
	 }
     }
   // find if there is a jet over 20 gev

   std::cout << b_njet_4 << " jets"<<std::endl;
   if (b_njet_4 < 1) return Fun4AllReturnCodes::EVENT_OK;

   auto is_greater_than_cut = [] (float et) { return et >= 15; };

   auto it = std::find_if(b_jet_et_4.begin(), b_jet_et_4.end(), is_greater_than_cut);

   if (it == b_jet_et_4.end())
     {
       return Fun4AllReturnCodes::EVENT_OK;
     }


   std::cout << " ET > 15 FOUND " << (*it) << " GeV" << std::endl;

   m_triggeranalyzer->decodeTriggers(topNode);

   if (m_triggeranalyzer->didTriggerFire(19))
     {
       return Fun4AllReturnCodes::EVENT_OK;
     }

   std::cout << " REJECT FOUND " << (*it) << " GeV" << std::endl;

   {
     h_emcal->Reset();
     int size = emcal_towers->size();
     for (int i = 0; i < size; i++)
       {
	 TowerInfo* tower = emcal_towers->get_tower_at_channel(i);
	 float offlineenergy = tower->get_energy();

	 unsigned int towerkey = emcal_towers->encode_key(i);
	 int ieta = emcal_towers->getTowerEtaBin(towerkey);
	 int iphi = emcal_towers->getTowerPhiBin(towerkey);
	 if (offlineenergy < 0.03) continue;
	 h_emcal->Fill(ieta, iphi, offlineenergy);
       }

     char hname[100];
     TH2F *h = (TH2F*) h_emcal->Clone();
     sprintf(hname, "h_emcal_%d", _eventcount);
     h->SetName(hname);
     hm->registerHisto(h);

     CaloPacketContainer *emcalcont = findNode::getClass<CaloPacketContainer>(topNode, "CEMCPackets");
     for (int packetid = 6001; packetid <= 6128; packetid++)
       {
	 CaloPacket *packet = emcalcont->getPacketbyId(packetid);
	 if (!packet)
	   return Fun4AllReturnCodes::ABORTRUN;
	  
	 for (int i = 0; i < 192; i++)
	   {
	     int cha = 192*(packetid-6001) + i;
	     if (emcal_towers->get_tower_at_channel(cha)->get_energy() < 0.03 ) continue;
	     char name[100];
	     sprintf(name, "ewave_%d_%d", _eventcount, cha);
	     auto *h_waveform = new TH1F(name, ";sample; adc", 12, -0.5, 11.5);
	      
	     int channel = i;
	     for (int is = 0; is < 12; is++)
	       {
		 if (packet->iValue(channel, "SUPPRESSED"))
		   {
		     if (is < 6)
		       {
			 h_waveform->Fill(is,packet->iValue(channel, "PRE"));
		       }
		     else
		       {
			 h_waveform->Fill(is,packet->iValue(channel, "POST"));
		       }
		   }
		 else
		   {
		     h_waveform->Fill(is,packet->iValue(is, channel));
		   }
		  
	       }
	     h_waveform->SetDirectory(nullptr);  
	     hm->registerHisto(h_waveform);
	   }
       }

   }
   {
     h_hcalout->Reset();

     int size = hcalout_towers->size();
     for (int i = 0; i < size; i++)
       {
	 TowerInfo* tower = hcalout_towers->get_tower_at_channel(i);
	 float offlineenergy = tower->get_energy();
	 float offlinetime = tower->get_time();
	 unsigned int towerkey = hcalout_towers->encode_key(i);
	 int ieta = hcalout_towers->getTowerEtaBin(towerkey);
	 int iphi = hcalout_towers->getTowerPhiBin(towerkey);
	 if (offlineenergy < 0.03) continue;
	 h_hcalout->Fill(ieta, iphi, offlineenergy);
	 h_hcalout_time->Fill(ieta, iphi, offlinetime);
       }

     char hname[100];
     TH2F *h = (TH2F*) h_hcalout->Clone();
     sprintf(hname, "h_hcalout_%d", _eventcount);
     h->SetName(hname);
     hm->registerHisto(h);
     TH2F *ht = (TH2F*) h_hcalout_time->Clone();
     sprintf(hname, "h_hcalout_time_%d", _eventcount);
     ht->SetName(hname);
     hm->registerHisto(ht);

     CaloPacketContainer *emcalcont = findNode::getClass<CaloPacketContainer>(topNode, "HCALPackets");
     for (int packetid = 8001; packetid <= 8008; packetid++)
       {
	 CaloPacket *packet = emcalcont->getPacketbyId(packetid);
	 if (!packet)
	   return Fun4AllReturnCodes::ABORTRUN;
      
	 std::cout << "FindRejects::Found one at " << _eventcounter <<std::endl;
	 for (int i = 0; i < 192; i++)
	   {
	     int cha = 192*(packetid-8001) + i;
	     if (hcalout_towers->get_tower_at_channel(cha)->get_energy() < 0.03 ) continue;
	     char name[100];
	     sprintf(name, "hwave_%d_%d", _eventcount, cha);
	     auto *h_waveform = new TH1F(name, ";sample; adc", 12, -0.5, 11.5);
	  
	     int channel = i;
	     for (int is = 0; is < 12; is++)
	       {
		 if (packet->iValue(channel, "SUPPRESSED"))
		   {
		     if (is < 6)
		       {
			 h_waveform->Fill(is,packet->iValue(channel, "PRE"));
		       }
		     else
		       {
			 h_waveform->Fill(is,packet->iValue(channel, "POST"));
		       }
		   }
		 else
		   {
		     h_waveform->Fill(is,packet->iValue(is, channel));
		   }
	      
	       }
	     h_waveform->SetDirectory(nullptr);  
	     hm->registerHisto(h_waveform);
	   }
       }
   }
   
   _eventcount++;
     if (_eventcount > 1000)
       {
	 return Fun4AllReturnCodes::ABORTRUN;
       }
     return Fun4AllReturnCodes::EVENT_OK;      
     
     
}

int FindRejects::End(PHCompositeNode* /*unused*/) 
{
  hm->dumpHistos(m_outfilename.c_str());
  TFile *fout = new TFile("failtest.root","recreate");
  tn->Write();
  fout->Close();
  return Fun4AllReturnCodes::EVENT_OK;
}
