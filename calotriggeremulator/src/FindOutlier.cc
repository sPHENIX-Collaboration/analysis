#include "FindOutlier.h"
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/RawTowerGeom.h>
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
#include"TrashInfov1.h"

#include <cassert>
#include <cmath>  // for log10, pow, sqrt, abs, M_PI
#include <cstdint>
#include <iostream>  // for operator<<, endl, basic_...
#include <limits>
#include <map>  // for operator!=, _Rb_tree_con...
#include <string>
#include <utility>  // for pair

FindOutlier::FindOutlier(const std::string& name, const std::string& outfile)
: SubsysReco(name)
, m_outfilename(outfile)

{						

}

FindOutlier::~FindOutlier()
{
  
}

int FindOutlier::Init(PHCompositeNode* /*unused*/)
{
  std::cout << "Leaving FindOutlier::Init" << std::endl;

  tn = new TNtuple("tn","tn","event:trash:r1:energy:emcal_energy:spread");
  hm = new Fun4AllHistoManager("FindOutlierHistos");
  h_emcal = new TH2F("EMCAL","", 12, -0.5, 95.5, 32, -0.5, 255.5);
  h_hcalout = new TH2F("HCALOUT","", 24, -0.5, 23.5, 64, -0.5, 63.5);
  h_hcalout_time = new TH2F("HCALOUT_TIME","", 24, -0.5, 23.5, 64, -0.5, 63.5);
  return Fun4AllReturnCodes::EVENT_OK;
}

int FindOutlier::process_event(PHCompositeNode* topNode)
{

  _eventcounter++;  
  if (Verbosity())  std::cout << "In process_event" << std::endl;

  Gl1Packet *gl1PacketInfo = findNode::getClass<Gl1Packet>(topNode, "GL1Packet");
  if (!gl1PacketInfo)
  {
    std::cout << PHWHERE << "FindOutlier::process_event: GL1Packet node is missing" << std::endl;
    return Fun4AllReturnCodes::EVENT_OK;
  }

  trashinfo = findNode::getClass<TrashInfo>(topNode, "TrashInfo");
  if (!trashinfo)
  {
    std::cout << PHWHERE << "FindOutlier::process_event: Trashinfo node is missing" << std::endl;
    return Fun4AllReturnCodes::EVENT_OK;
  }
  //  if (!trashinfo->isTrash()) return Fun4AllReturnCodes::EVENT_OK;
  
  TowerInfoContainer* emcal_towers = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC");
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
  if (Verbosity())  std::cout << "emcal" << std::endl;
  char hname[100];
  TH2F *h = (TH2F*) h_emcal->Clone();
  sprintf(hname, "h_emcal_%d_%d", (trashinfo->isTrash()? 1 : 0), _eventcounter);
  h->SetName(hname);
  hm->registerHisto(h);

  if (useWave)
    {  
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
	      sprintf(name, "wave_%d_%d", _eventcounter, cha);
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
  
  TowerInfoContainer* hcalout_towers = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT");
  h_hcalout->Reset();
  h_hcalout_time->Reset();
  size = hcalout_towers->size();
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
  if (Verbosity())  std::cout << "hcal" << std::endl;  
  h = (TH2F*) h_hcalout->Clone();
  sprintf(hname, "h_hcalout_%d_%d", (trashinfo->isTrash()? 1 : 0), _eventcounter);
  h->SetName(hname);
  hm->registerHisto(h);
  TH2F *ht = (TH2F*) h_hcalout_time->Clone();
  sprintf(hname, "h_hcalout_time_%d_%d", (trashinfo->isTrash()? 1 : 0), _eventcounter);
  ht->SetName(hname);
  hm->registerHisto(ht);

  tn->Fill(_eventcounter, (trashinfo->isTrash()? 1:0), trashinfo->getR1(), trashinfo->getEnergy(), trashinfo->getEMCALEnergy(), trashinfo->getSpread());

  if (useWave)
    {

      CaloPacketContainer *emcalcont = findNode::getClass<CaloPacketContainer>(topNode, "HCALPackets");
      for (int packetid = 8001; packetid <= 8008; packetid++)
	{
	  CaloPacket *packet = emcalcont->getPacketbyId(packetid);
	  if (!packet)
	    return Fun4AllReturnCodes::ABORTRUN;
      
	  std::cout << "FindOutlier::Found one at " << _eventcounter <<std::endl;
	  for (int i = 0; i < 192; i++)
	    {
	      int cha = 192*(packetid-8001) + i;
	      if (hcalout_towers->get_tower_at_channel(cha)->get_energy() < 0.03 ) continue;
	      char name[100];
	      sprintf(name, "wave_%d_%d", _eventcounter, cha);
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
  // uint64_t triggervec = gl1PacketInfo->getScaledVector();
      // bool mbd_event = (((triggervec >> 10U) & 0x1U) == 0x1U);
      // if (!mbd_event)   return Fun4AllReturnCodes::EVENT_OK;


  // uint64_t rawvec = gl1PacketInfo->lValue(0, "TriggerInput");
  // bool rare_event = ((rawvec >> 18U) > 0);
  // if (rare_event)   return Fun4AllReturnCodes::EVENT_OK;

  // // calculate max
  // TowerInfoContainer* towers = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC");
  // int size = towers->size();  // online towers should be the same!
  // h_emcal->Reset();

  // for (int i = 0; i < size; i++)
  //   {
  //       TowerInfo* tower = towers->get_tower_at_channel(i);
  //       float offlineenergy = tower->get_energy();
  //       unsigned int towerkey = towers->encode_key(i);
  //       int ieta = towers->getTowerEtaBin(towerkey);
  //       int iphi = towers->getTowerPhiBin(towerkey);
  //       bool isGood = tower->get_isGood();
  // 	if (!isGood) continue;
  // 	h_emcal->Fill(ieta, iphi, offlineenergy);
  // 	if (offlineenergy > max_cut) {
	  
  // 	  std::cout << i << " " <<ieta<< " " << iphi <<std::endl;
  // 	}
  //   }
  // int binnumber = h_emcal->GetMaximumBin();
  // if (h_emcal->GetBinContent(binnumber) < max_cut)  return Fun4AllReturnCodes::EVENT_OK;

  // int binx = -1;
  // int binz = -1;
  // int biny = -1;
  // h_emcal->GetBinXYZ(binnumber, binx, biny, binz);
  // if (binx < 0)
  //   return Fun4AllReturnCodes::ABORTRUN;

  // binx--;
  // biny--;
  // int packetid = 4 * biny;
  // int channeloffset = 0;
  // if (binx < 3)
  //   {
  //     channeloffset = 2 - binx;
  //     packetid += 1;
  //   }
  // else if (binx < 6)
  //   {
  //     channeloffset = 2 - (binx%3);
  //     packetid += 0;
  //   }
  // else if (binx < 9)
  //   {
  //     channeloffset = (binx%3);
  //     packetid += 2;
  //   }
  // else
  //   {
  //     channeloffset = (binx%3);
  //     packetid += 3;
  //   }
  // channeloffset *= 64;
  // packetid += 6001;
    
  // CaloPacketContainer *emcalcont = findNode::getClass<CaloPacketContainer>(topNode, "CEMCPackets");
  // CaloPacket *packet = emcalcont->getPacketbyId(packetid);
  // if (!packet)
  //   return Fun4AllReturnCodes::ABORTRUN;

  // std::cout << "FindOutlier::Found one at " << _eventcounterer <<" ch " << binx << " " << biny << " " << std::endl;
  // for (int i = 0; i < 192; i++)
  //   {
  //     char name[100];
  //     sprintf(name, "wave_%d_%d", i, _eventcounteryer);
  //     auto *h_waveform = new TH1F(name, ";sample; adc", 12, -0.5, 11.5);

  //     int channel = i;
  //     for (int is = 0; is < 12; is++)
  // 	{
  // 	  if (packet->iValue(channel, "SUPPRESSED"))
  // 	    {
  // 	      if (is < 6)
  // 		{
  // 		  h_waveform->Fill(is,packet->iValue(channel, "PRE"));
  // 		}
  // 	      else
  // 		{
  // 		  h_waveform->Fill(is,packet->iValue(channel, "POST"));
  // 		}
  // 	    }
  // 	  else
  // 	    {
  // 	      h_waveform->Fill(is,packet->iValue(is, channel));
  // 	    }
      
  // 	}
  //     h_waveform->SetDirectory(nullptr);  
  //     hm->registerHisto(h_waveform);
  //   }


  return Fun4AllReturnCodes::EVENT_OK;      

  
}

int FindOutlier::End(PHCompositeNode* /*unused*/) 
{
  hm->dumpHistos(m_outfilename.c_str());
  std::string fail=m_outfilename;
  auto it = fail.find("HIST");
  fail.replace(it, 4, "NTUPLE");
  TFile *fout = new TFile(fail.c_str(),"recreate");
  tn->Write();
  fout->Close();
  return Fun4AllReturnCodes::EVENT_OK;
}
