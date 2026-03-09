#include "DijetEventDisplay.h"
#include <phool/PHNode.h>
#include <phool/getClass.h>
#include <ffaobjects/EventHeader.h>
#include <ffaobjects/EventHeaderv1.h>
#include <ffaobjects/EventHeaderv2.h>
DijetEventDisplay::DijetEventDisplay(const std::string f)
  : file{f}
{
  hm = new Fun4AllHistoManager("DIJETEVENTDISPLAY");
}
int DijetEventDisplay::FillEvent(PHCompositeNode *topNode, float aj, float dphi)
{
  EventHeader *eventheader = findNode::getClass<EventHeader>(topNode, "EventHeader");
  RawTowerGeomContainer *tower_geomIH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALIN");
  RawTowerGeomContainer *tower_geomOH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALOUT");
  TowerInfoContainer *hcalin_towers = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALIN");
  TowerInfoContainer *hcalout_towers = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT");
  TowerInfoContainer *emcalre_towers = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC_RETOWER");

  JetContainer *jets_4 = findNode::getClass<JetContainer>(topNode, "AntiKt_Tower_r04_Sub1");
  int eventnumber = eventheader->get_EvtSequence();

  std::string aj_name = "h_aj_" + std::to_string(eventnumber);
  std::string dphi_name = "h_dphi_" + std::to_string(eventnumber);
  TH1F *haj = new TH1F(aj_name.c_str(), "", 1, -1, 1);
  haj->SetBinContent(1, aj);
  hm->registerHisto(haj);
  TH1F *hdphi = new TH1F(dphi_name.c_str(), "", 1, -1, 1);
  hdphi->SetBinContent(1, dphi);
  hm->registerHisto(hdphi);
  float binedges_eta_in[25];
  float binedges_eta_out[25];
  float binedges_phi_in[65];
  float binedges_phi_out[65];
  for (int ieta = 0; ieta < 24; ieta++)
    {
      const RawTowerDefs::keytype key = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::HCALIN, ieta, 0);     
      float tower_eta_in = tower_geomIH->get_tower_geometry(key)->get_eta();
      const RawTowerDefs::keytype keyo = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::HCALOUT, ieta, 0);
      float tower_eta_out = tower_geomOH->get_tower_geometry(keyo)->get_eta();
      binedges_eta_in[ieta] = tower_eta_in;
      binedges_eta_out[ieta] = tower_eta_out;
    }

  binedges_eta_in[24] = binedges_eta_in[23] + (binedges_eta_in[23] - binedges_eta_in[22]);
  binedges_eta_out[24] = binedges_eta_out[23] + (binedges_eta_out[23] - binedges_eta_out[22]);
  for (int ieta = 0; ieta < 25; ieta++)
    {
      binedges_eta_in[ieta] = binedges_eta_in[ieta] - (binedges_eta_in[1] - binedges_eta_in[0])/2.;
      binedges_eta_out[ieta] = binedges_eta_out[ieta] - (binedges_eta_out[1] - binedges_eta_out[0])/2.;
    }
  std::vector<float> binedge_phi_in;
  std::vector<float> binedge_phi_out;
  for (int iphi = 0; iphi < 64; iphi++)
    {
      const RawTowerDefs::keytype key = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::HCALIN, 0, iphi);
      float tower_phi_in = tower_geomIH->get_tower_geometry(key)->get_phi();
      const RawTowerDefs::keytype keyo = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::HCALOUT, 0, iphi);
      float tower_phi_out = tower_geomOH->get_tower_geometry(keyo)->get_phi();

      binedge_phi_in.push_back(tower_phi_in);
      binedge_phi_out.push_back(tower_phi_out);     

    }
  std::sort(binedge_phi_in.begin(), binedge_phi_in.end());
  std::sort(binedge_phi_out.begin(), binedge_phi_out.end());
  for (int iphi = 0; iphi < 64; iphi++)
    {
      binedges_phi_in[iphi] = binedge_phi_in.at(iphi);
      binedges_phi_out[iphi] = binedge_phi_out.at(iphi);
    }
  binedges_phi_in[64] = binedges_phi_in[63] + (binedges_phi_in[63] - binedges_phi_in[62]);
  binedges_phi_out[64] = binedges_phi_out[63] + (binedges_phi_out[63] - binedges_phi_out[62]);
  for (int iphi = 0; iphi < 65; iphi++)
    {

      binedges_phi_in[iphi] = binedges_phi_in[iphi] - (binedges_phi_in[1] - binedges_phi_in[0])/2.;
      binedges_phi_out[iphi] = binedges_phi_out[iphi] - (binedges_phi_out[1] - binedges_phi_out[0])/2.;
    }  
  int ijet = 0;
  
  for (auto jet : *jets_4)
    {
      if (jet->get_pt() < 1) continue;

      std::string emcal_name = "h_emcal_dijet_" + std::to_string(eventnumber) + "_jet" + std::to_string(ijet);
      std::string hcalin_name = "h_hcalin_dijet_" + std::to_string(eventnumber) + "_jet" + std::to_string(ijet);
      std::string hcalout_name = "h_hcalout_dijet_" + std::to_string(eventnumber) + "_jet" + std::to_string(ijet);
      std::string all_name = "h_all_dijet_" + std::to_string(eventnumber) + "_jet" + std::to_string(ijet);

      ijet++;
      
      h_emcal = new TH2D(emcal_name.c_str(), ";#eta;#phi", 24, binedges_eta_in, 64, binedges_phi_in);
      h_hcalin = new TH2D(hcalin_name.c_str(), ";#eta;#phi", 24, binedges_eta_in, 64, binedges_phi_in);
      h_hcalout = new TH2D(hcalout_name.c_str(), ";#eta;#phi", 24, binedges_eta_out, 64, binedges_phi_out);
      h_all = new TH2D(all_name.c_str(), ";#eta;#phi", 24, binedges_eta_out, 64, binedges_phi_out);
      
      hm->registerHisto(h_all);
      hm->registerHisto(h_emcal);
      hm->registerHisto(h_hcalin);
      hm->registerHisto(h_hcalout);

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
	      float tower_phi = tower_geomIH->get_tower_geometry(key)->get_phi();
	      float tower_eta = tower_geomIH->get_tower_geometry(key)->get_eta();
	      tower_eT = tower->get_energy() / std::cosh(tower_eta);

	      h_hcalin->Fill(tower_eta, tower_phi, tower_eT);
	      h_all->Fill(tower_eta, tower_phi, tower_eT);
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
	      float tower_phi = tower_geomOH->get_tower_geometry(key)->get_phi();
	      float tower_eta = tower_geomOH->get_tower_geometry(key)->get_eta();
	      tower_eT = tower->get_energy() / std::cosh(tower_eta);

	      h_hcalout->Fill(tower_eta, tower_phi, tower_eT);
	      h_all->Fill(tower_eta, tower_phi, tower_eT);
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
	      float tower_phi = tower_geomIH->get_tower_geometry(key)->get_phi();
	      float tower_eta = tower_geomIH->get_tower_geometry(key)->get_eta();
	      tower_eT = tower->get_energy() / std::cosh(tower_eta);
	      
	      h_emcal->Fill(tower_eta, tower_phi, tower_eT);
	      h_all->Fill(tower_eta, tower_phi, tower_eT);
	    }	       
	}
    }
  std::string emcal_namefull = "h_emcal_full_" + std::to_string(eventnumber);
  std::string hcalin_namefull = "h_hcalin_full_" + std::to_string(eventnumber);
  std::string hcalout_namefull = "h_hcalout_full_" + std::to_string(eventnumber);
  std::string all_namefull = "h_all_full_" + std::to_string(eventnumber);

  ijet++;
      
  h_emcal = new TH2D(emcal_namefull.c_str(), ";#eta;#phi", 24, binedges_eta_in, 64, binedges_phi_in);
  h_hcalin = new TH2D(hcalin_namefull.c_str(), ";#eta;#phi", 24, binedges_eta_in, 64, binedges_phi_in);
  h_hcalout = new TH2D(hcalout_namefull.c_str(), ";#eta;#phi", 24, binedges_eta_out, 64, binedges_phi_out);
  h_all = new TH2D(all_namefull.c_str(), ";#eta;#phi", 24, binedges_eta_out, 64, binedges_phi_out);
      
  hm->registerHisto(h_all);
  hm->registerHisto(h_emcal);
  hm->registerHisto(h_hcalin);
  hm->registerHisto(h_hcalout);

  int size;
  size = emcalre_towers->size();
  for (int channel = 0; channel < size; channel++)
    {
      TowerInfo *tower = emcalre_towers->get_tower_at_channel(channel);

      unsigned int calokey = emcalre_towers->encode_key(channel);
      int ieta = emcalre_towers->getTowerEtaBin(calokey);
      int iphi = emcalre_towers->getTowerPhiBin(calokey);
      const RawTowerDefs::keytype key = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::HCALIN, ieta, iphi);
      float tower_phi = tower_geomIH->get_tower_geometry(key)->get_phi();
      float tower_eta = tower_geomIH->get_tower_geometry(key)->get_eta();
      float tower_eT = tower->get_energy() / std::cosh(tower_eta);
	      
      h_emcal->Fill(tower_eta, tower_phi, tower_eT);
      h_all->Fill(tower_eta, tower_phi, tower_eT);
    }
  size = hcalin_towers->size();
  for (int channel = 0; channel < size; channel++)
    {
      TowerInfo *tower = hcalin_towers->get_tower_at_channel(channel);

      unsigned int calokey = hcalin_towers->encode_key(channel);
      int ieta = hcalin_towers->getTowerEtaBin(calokey);
      int iphi = hcalin_towers->getTowerPhiBin(calokey);
      const RawTowerDefs::keytype key = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::HCALIN, ieta, iphi);
      float tower_phi = tower_geomIH->get_tower_geometry(key)->get_phi();
      float tower_eta = tower_geomIH->get_tower_geometry(key)->get_eta();
      float tower_eT = tower->get_energy() / std::cosh(tower_eta);
	      
      h_hcalin->Fill(tower_eta, tower_phi, tower_eT);
      h_all->Fill(tower_eta, tower_phi, tower_eT);
    }
  size = hcalout_towers->size();
  for (int channel = 0; channel < size; channel++)
    {
      TowerInfo *tower = hcalout_towers->get_tower_at_channel(channel);

      unsigned int calokey = hcalout_towers->encode_key(channel);
      int ieta = hcalout_towers->getTowerEtaBin(calokey);
      int iphi = hcalout_towers->getTowerPhiBin(calokey);
      const RawTowerDefs::keytype key = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::HCALOUT, ieta, iphi);
      float tower_phi = tower_geomOH->get_tower_geometry(key)->get_phi();
      float tower_eta = tower_geomOH->get_tower_geometry(key)->get_eta();
      float tower_eT = tower->get_energy() / std::cosh(tower_eta);
	      
      h_hcalout->Fill(tower_eta, tower_phi, tower_eT);
      h_all->Fill(tower_eta, tower_phi, tower_eT);
    }
  return 0;  
}

void DijetEventDisplay::Dump()
{
  hm->dumpHistos(file);
}
