#include "EliminateBackground.h"
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/RawTowerGeom.h>
// Calo 
#include "TMath.h"
#include "TrashInfo.h"
#include "TrashInfov1.h"

#include <calobase/TowerInfo.h>
#include <calobase/TowerInfoDefs.h>
#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/PHObject.h>
#include <phool/getClass.h>
#include <phool/phool.h>

#include <TH1.h>
#include <TH2.h>

#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfoContainerv4.h>

#include <fun4all/Fun4AllReturnCodes.h>


EliminateBackground::EliminateBackground(const std::string& name)
  : SubsysReco(name)
{						

}

EliminateBackground::~EliminateBackground()
{
  
}

int EliminateBackground::InitRun(PHCompositeNode* topNode)
{
  hcal_phi_consec = new TH1F("hcal_phi_consec", "", 64, -0.5, 63.5); 
  hcal_phi = new TH1F("hcal_phi", "", 64, -0.5, 63.5);
  hcal = new TH2F("hcal", "", 24, -0.5, 23.5, 64, -0.5, 63.5);

  PHNodeIterator iter(topNode);

  // Looking for the DST node
  PHCompositeNode *dstNode = dynamic_cast<PHCompositeNode *>(iter.findFirst("PHCompositeNode", "DST"));
  if (!dstNode)
    {
      std::cout << PHWHERE << "DST Node missing, doing nothing." << std::endl;
      return Fun4AllReturnCodes::ABORTRUN;
    }

  // store the GLOBAL stuff under a sub-node directory
  PHCompositeNode *globalNode = dynamic_cast<PHCompositeNode *>(iter.findFirst("PHCompositeNode", "GLOBAL"));
  if (!globalNode)
    {
      globalNode = new PHCompositeNode("GLOBAL");
      dstNode->addNode(globalNode);
    }

  // create the GlobalVertexMap
  TrashInfo *trash = findNode::getClass<TrashInfo>(topNode, "TrashInfo");
  if (!trash)
    {
      trash = new TrashInfov1();
      PHIODataNode<PHObject> *TrashNode = new PHIODataNode<PHObject>(trash, "TrashInfo", "PHObject");
      globalNode->addNode(TrashNode);
    }
  return Fun4AllReturnCodes::EVENT_OK;


}
int EliminateBackground::Init(PHCompositeNode* /*unused*/)
{

  
  return Fun4AllReturnCodes::EVENT_OK;
}

int EliminateBackground::process_event(PHCompositeNode* topNode)
{

  TowerInfoContainer *hcalout_towers = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT");
  //  TowerInfoContainer *emcal_towers = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC");
  TrashInfo *trash = findNode::getClass<TrashInfo>(topNode, "TrashInfo");

  if (Verbosity()) std::cout << __LINE__ <<std::endl;
  hcal_phi->Reset();
  hcal_phi_consec->Reset();
  hcal->Reset();
  int size = hcalout_towers->size();
  trash->setIsTrash(false);
  for (int channel = 0; channel < size;channel++)
    {
      TowerInfo *tower = hcalout_towers->get_tower_at_channel(channel);
      float energy = tower->get_energy();
      unsigned int towerkey = hcalout_towers->encode_key(channel);
      int ieta = hcalout_towers->getTowerEtaBin(towerkey);
      int iphi = hcalout_towers->getTowerPhiBin(towerkey);
      //short good = (tower->get_isGood() ? 1:0);
      //      if (!good) continue;
      if (energy > HCAL_TOWER_ENERGY_CUT)
	{
	  hcal->Fill(ieta, iphi, energy);
	  hcal_phi->Fill(iphi);
	}
    }

  TH1F *hcal_proj = (TH1F*) hcal->ProjectionY();

  for (int iphi = 0 ; iphi < 64; iphi++)
    {
      int maxlength = 0; 
      int length = 0;
      for (int ieta = 0 ; ieta < 24; ieta++)
	{
	  int bin = hcal->GetBin(ieta+1, iphi+1);
	  if (hcal->GetBinContent(bin) > CONSECUTIVE_ENERGY_CUT)
	    {
	      length++;
	    }
	  else
	    {
	      length = 0;
	    }
	  if (maxlength < length) maxlength = length;
	}
      hcal_phi_consec->Fill(iphi, maxlength);
    }
  int bmax = hcal_phi_consec->GetMaximumBin();
  trash->setSpread(hcal_phi->GetBinContent(bmax));
  trash->setR1(hcal_phi_consec->GetBinContent(bmax));
  trash->setEnergy(bmax);

  if (hcal_phi_consec->GetBinContent(bmax) >= CONSECUTIVE_COUNT_CUT)
    {
      if (hcal_proj->GetMaximumBin() != bmax)
	{
	  std::cout << " SIKE not the max " <<std::endl;
	  return Fun4AllReturnCodes::EVENT_OK;
	}
      if (Verbosity()) std::cout << "EVENT ELIMINATED" << std::endl;
      if (m_keep)
	{
	  trash->setIsTrash(true);
	  return Fun4AllReturnCodes::EVENT_OK;
	}
      else
	{
	  return Fun4AllReturnCodes::ABORTEVENT;
	}
    }
  if (hcal_phi->GetBinContent(bmax) >= COUNT_CUT)
    {
      if (hcal_proj->GetMaximumBin() != bmax)
	{
	  std::cout << " SIKE not the max " <<std::endl;
	  return Fun4AllReturnCodes::EVENT_OK;
	}

      if (Verbosity()) std::cout << "EVENT ELIMINATED" << std::endl;
      if (m_keep)
	{	  
	  trash->setIsTrash(true);
	  return Fun4AllReturnCodes::EVENT_OK;
	}
      else
	{
	  return Fun4AllReturnCodes::ABORTEVENT;
	}
    }
  int binup = (bmax == 64  ?  1 : bmax + 1);
  int bindown = (bmax == 1  ?  64 : bmax - 1);
  bool nextdoor_consec = (hcal_phi_consec->GetBinContent(binup) < 3 && hcal_phi_consec->GetBinContent(bindown) < 3);
  bool nextdoor_total = (hcal_phi->GetBinContent(binup) < 4 && hcal_phi->GetBinContent(bindown) < 4);
  bool bad = (hcal_phi_consec->GetBinContent(bmax) >= 5 &&  nextdoor_total && nextdoor_consec);

  if (bad)
    {
      if (hcal_proj->GetMaximumBin() != bmax)
	{
	  trash->setIsTrash(false);
	  std::cout << " SIKE not the max " <<std::endl;
	  return Fun4AllReturnCodes::EVENT_OK;
	}
      trash->setIsTrash(true);
      if (!m_keep) return Fun4AllReturnCodes::ABORTEVENT;
    }
  return Fun4AllReturnCodes::EVENT_OK;
}

int EliminateBackground::End(PHCompositeNode* /*unused*/) 
{
  return Fun4AllReturnCodes::EVENT_OK;
}
