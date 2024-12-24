#include "EmulatorHistos.h"

// Trigger includes
#include <calobase/TowerInfo.h>
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfoDefs.h>

#include <calotrigger/LL1Out.h>
#include <calotrigger/LL1Outv1.h>
#include <calotrigger/TriggerPrimitive.h>
#include <calotrigger/TriggerPrimitivev1.h>
#include <calotrigger/TriggerPrimitiveContainer.h>
#include <calotrigger/TriggerPrimitiveContainerv1.h>
#include <calotrigger/TriggerDefs.h>

#include <qautils/QAHistManagerDef.h>

#include <fun4all/Fun4AllHistoManager.h>
#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/getClass.h>
#include <phool/phool.h>  // for PHWHERE

#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TLorentzVector.h>
#include <TNtuple.h>
#include <TProfile2D.h>
#include <TSystem.h>
#include <TTree.h>

#include <cmath>     // for log10, pow, sqrt, abs, M_PI
#include <iostream>  // for operator<<, endl, basic_...
#include <limits>
#include <map>  // for operator!=, _Rb_tree_con...
#include <string>
#include <utility>  // for pair

EmulatorHistos::EmulatorHistos(const std::string& name)
: SubsysReco(name)
{
}

int EmulatorHistos::Init(PHCompositeNode* /*unused*/)
{
  auto hm = QAHistManagerDef::getHistoManager();
  assert(hm);
  // create and register your histos (all types) here

  if (m_debug)
    {
      std::cout << "In EmulatorHistos::Init" << std::endl;
    }

  TH1D *h = nullptr;
  TH2D *h2 = nullptr;
  
  // rejections by ;

  h = new TH1D("h_rejection_photon",";Emulated Threshold;Rejection Factor", 50, 0, 50);
  hm->registerHisto(h);
  h = new TH1D("h_rejection_jet",";Emulated Threshold;Rejection Factor", 50, 0, 50);
  hm->registerHisto(h);

  h2 = new TH2D("h_photon_lutsum",";Max Digital Sum Photon;Corresponding 0.2x0.2 sum E [GeV]", 256, -0.5, 255.5, 50, 0, 50);
  hm->registerHisto(h2);

  h2 = new TH2D("h_jet_lutsum",";Max Digital Sum Jet;Corresponding 0.8x0.8 sum E [GeV]", 256, -0.5, 255.5, 50, 0, 50);
  hm->registerHisto(h2);

  for (int i = 0; i < 50; i++)
    {
      h = new TH1D(Form("h_eT_photon_%d", i),";Energy;", 50, 0, 50);
      hm->registerHisto(h);
      h = new TH1D(Form("h_eT_jet_%d", i),";Energy;", 50, 0, 50);
      hm->registerHisto(h);
    }
  
  i_event = 0;
  h = new TH1D("h_nevent", "",1, 0, 2);
  hm->registerHisto(h);


  if (m_debug)
    {
      std::cout << "Leaving EmulatorHistos::Init" << std::endl;
    }
  return 0;
}

int EmulatorHistos::process_event(PHCompositeNode* topNode)
{
  i_event++;
  
  return process_towers(topNode);

}

int EmulatorHistos::process_towers(PHCompositeNode* topNode)
{


  TowerInfoContainer* towers_emcal = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC");

  TowerInfoContainer* towers_hcalin = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALIN");

  TowerInfoContainer* towers_hcalout = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT");

  LL1Out* ll1out_jet = findNode::getClass<LL1Out>(topNode, "LL1OUT_JET");

  LL1Out* ll1out_photon = findNode::getClass<LL1Out>(topNode, "LL1OUT_PHOTON");


  auto hm = QAHistManagerDef::getHistoManager();
  assert(hm);

   int ll1_photon[12][32];
   int ll1_jet[12][32];
   for (int i = 0; i < 12; i++)
     {
       for (int j = 0; j < 32; j++)
	 {
	   ll1_photon[i][j] = 0;
	   ll1_jet[i][j] = 0;
	 }
     }
   int max_photon = 0;
   int max_jet = 0;

  if (ll1out_photon)
    {
      auto triggered_sums = ll1out_photon->getTriggeredSums();

      for (auto &keypair : triggered_sums)
	{
	  TriggerDefs::TriggerSumKey key = keypair.first;
	  unsigned short bit = keypair.second;
	  int phi = TriggerDefs::getSumPhiId(key) + TriggerDefs::getPrimitivePhiId_from_TriggerSumKey(key)*2;
	  int eta = TriggerDefs::getSumEtaId(key);
	  ll1_photon[eta][phi] = static_cast<int>(bit);
	  if (static_cast<int>(bit) > max_photon)
	    max_photon = static_cast<int>(bit);
	}
    }
  if (ll1out_jet)
    {
      auto triggered_sums = ll1out_jet->getTriggeredSums();
      for (auto &keypair : triggered_sums)
	{
	  TriggerDefs::TriggerSumKey key = keypair.first;
	  unsigned short bit = keypair.second;
	  int phi = static_cast<int>(key & 0xffffU); 
	  int eta = static_cast<int>((key >> 16U) & 0xffffU); 
	  ll1_jet[eta][phi] = static_cast<int>(bit);
	  if (static_cast<int>(bit) > max_jet)
	    max_jet = static_cast<int>(bit);
	}
    }

  float emcal_energies[12][35]{0.0};
  float hcal_energies[12][35]{0.0};
  float max_energy_emcal = 0.0;
  int size;
  if (towers_emcal)
    {
      size = towers_emcal->size();
      for (int itower = 0; itower < size ; itower++)
	{
	  TowerInfo* tower = towers_emcal->get_tower_at_channel(itower);

	  float offlineenergy = tower->get_energy();
	  unsigned int towerkey = towers_emcal->encode_key(itower);
	  int ieta = towers_emcal->getTowerEtaBin(towerkey);
	  int iphi = towers_emcal->getTowerPhiBin(towerkey);
	  emcal_energies[ieta/8][iphi/8] += offlineenergy;
	}
      for (int i = 0; i < 12; i++)
	{
	  for (int j = 0; j < 32; j++)
	    {
	      if (emcal_energies[i][j] > max_energy_emcal)
		{
		  max_energy_emcal = emcal_energies[i][j];
		}
	    }
	}
    }

  if (towers_hcalin)
    {
      size = towers_hcalin->size();
      for (int itower = 0; itower < size ; itower++)
	{
	  TowerInfo* tower = towers_hcalin->get_tower_at_channel(itower);
	
	  float offlineenergy = tower->get_energy();
	  unsigned int towerkey = towers_hcalin->encode_key(itower);
	  int ieta = towers_hcalin->getTowerEtaBin(towerkey);
	  int iphi = towers_hcalin->getTowerPhiBin(towerkey);
	  hcal_energies[ieta/2][iphi/2] += offlineenergy;
	}
      
    }
  if (towers_hcalout)
    {
      size = towers_hcalout->size();
      for (int itower = 0; itower < size ; itower++)
	{
	  TowerInfo* tower = towers_hcalout->get_tower_at_channel(itower);

	  float offlineenergy = tower->get_energy();
	  unsigned int towerkey = towers_hcalout->encode_key(itower);
	  int ieta = towers_hcalout->getTowerEtaBin(towerkey);
	  int iphi = towers_hcalout->getTowerPhiBin(towerkey);
	  hcal_energies[ieta/2][iphi/2] += offlineenergy;
	}

    }

  
  float jet_energies[9][32]{};
  float max_energy_jetpatch = 0.0;

  for (int i = 0; i < 3; i++)
    {
      for (int j = 0; j < 12; j++)
	{
	  emcal_energies[j][i + 32] = emcal_energies[j][i];
	  hcal_energies[j][i + 32] = hcal_energies[j][i];
	}
    }
  for (int i = 0; i < 9; i++)
    {
      for (int j = 0; j < 32; j++)
	{
	  for (int k = 0; k < 16; k++)
	    {
	      jet_energies[i][j] += emcal_energies[i + k % 4][j + k / 4];
	      jet_energies[i][j] += hcal_energies[i + k % 4][j + k / 4];
	    }
	  if (jet_energies[i][j] > max_energy_jetpatch)
	    {
	      max_energy_jetpatch = jet_energies[i][j];
	    }
	}
    }

  auto h_rej_photon = dynamic_cast<TH1*>(hm->getHisto("h_rejection_photon"));
  for (int i = 0; i < max_photon; i++)
    {
      h_rej_photon->Fill(i);
    }

  auto h_rej_jet = dynamic_cast<TH1*>(hm->getHisto("h_rejection_jet"));
  for (int i = 0; i < max_jet; i++)
    {
      h_rej_jet->Fill(i);
    }

  auto h_photon_lutsum = dynamic_cast<TH2*>(hm->getHisto("h_photon_lutsum"));
  auto h_jet_lutsum = dynamic_cast<TH2*>(hm->getHisto("h_jet_lutsum"));
  for (int i = 0; i < 12; i++)
    {
      for (int j = 0; j < 32; j++)
	{
	  h_photon_lutsum->Fill(ll1_photon[i][j], emcal_energies[i][j]);
	  if (i < 9)
	    {
	      h_jet_lutsum->Fill(ll1_jet[i][j], jet_energies[i][j]);
	    }
	}
    }


  for ( int i = 0; i < 50; i++)
    {
      auto h_eT_photon = dynamic_cast<TH1*>(hm->getHisto(Form("h_eT_photon_%d", i)));
      if (max_photon >= i)
	h_eT_photon->Fill(max_energy_emcal);
      auto h_eT_jet = dynamic_cast<TH1*>(hm->getHisto(Form("h_eT_jet_%d", i)));
      if (max_jet >= i)
	h_eT_jet->Fill(max_energy_emcal);
    }
  auto h_nevent = dynamic_cast<TH1*>(hm->getHisto("h_nevent"));
  h_nevent->Fill(1);			     
  return Fun4AllReturnCodes::EVENT_OK;
}

int EmulatorHistos::End(PHCompositeNode*)
{
  // auto hm = QAHistManagerDef::getHistoManager();
  // assert(hm);
  
  // auto h_rej_photon = dynamic_cast<TH1*>(hm->getHisto("h_rejection_photon"));
  // for (int i = 0 ; i < h_rej_photon->GetXaxis()->GetNbins(); i++)
  //   {
  //     if (h_rej_photon->GetBinContent(i+1) == 0)
  // 	{
  // 	  h_rej_photon->SetBinContent(i+1, i_event);
  // 	}
  //     else
  // 	{
  // 	  h_rej_photon->SetBinContent(i+1, i_event/h_rej_photon->GetBinContent(i+1));
  // 	}
  //   }
  // auto h_rej_jet = dynamic_cast<TH1*>(hm->getHisto("h_rejection_jet"));
  // for (int i = 0 ; i < h_rej_jet->GetXaxis()->GetNbins(); i++)
  //   {
  //     if (h_rej_jet->GetBinContent(i+1) == 0)
  // 	{
  // 	  h_rej_jet->SetBinContent(i+1, i_event);
  // 	}
  //     else
  // 	{
  // 	  h_rej_jet->SetBinContent(i+1, i_event/h_rej_jet->GetBinContent(i+1));
  // 	}
  //   }
  return Fun4AllReturnCodes::EVENT_OK;
}
