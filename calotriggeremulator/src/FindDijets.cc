#include "FindDijets.h"
//for emc clusters
#include <TH1.h>
#include <TH2.h>
#include <TH2D.h>
#include <TH1D.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>
#include <TMath.h>
#include <calotrigger/TriggerRunInfov1.h>

#include <calobase/RawTowerGeom.h>
#include <jetbackground/TowerBackground.h>
#include <jetbase/Jet.h>
#include <jetbase/JetContainer.h>
#include <jetbase/JetContainerv1.h>
#include <jetbase/Jetv2.h>
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfoContainerv1.h>
#include <calobase/TowerInfoContainerv2.h>
#include <calobase/TowerInfoContainerv3.h>
#include <calobase/TowerInfo.h>
#include <calobase/TowerInfov1.h>
#include <calobase/TowerInfov2.h>
#include <calobase/TowerInfov3.h>

#include <calobase/RawCluster.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawClusterUtility.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/RawTower.h>
#include <calobase/RawTowerContainer.h>
#include <fun4all/Fun4AllHistoManager.h>
#include <HepMC/SimpleVector.h> 
//for vetex information
#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>
#include "TrashInfov1.h"
#include <vector>

#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/PHObject.h>
#include <phool/getClass.h>
// G4Cells includes

#include <iostream>

#include <map>

//____________________________________________________________________________..
FindDijets::FindDijets(const std::string &name, const std::string &outfilename):
  SubsysReco(name)
  
{
  isSim = false;
  _foutname = outfilename;  
  m_calo_nodename = "TOWERINFO_CALIB";
  m_jet_triggernames = {"Jet 6 GeV + MBD NS >= 1",
			"Jet 8 GeV + MBD NS >= 1",
			"Jet 10 GeV + MBD NS >= 1",
			"Jet 12 GeV + MBD NS >= 1",
			"Jet 6 GeV",
			"Jet 8 GeV",
			"Jet 10 GeV",
			"Jet 12 GeV"};
}

//____________________________________________________________________________..
FindDijets::~FindDijets()
{

}

//____________________________________________________________________________..
int FindDijets::Init(PHCompositeNode *topNode)
{

  triggeranalyzer = new TriggerAnalyzer();
  hm = new Fun4AllHistoManager("DIJET_HISTOGRAMS");

  TH1D *h;
  TH2D *h2;

  h2 = new TH2D("h_pt1_pt2",";p_{T1};p_{T2}", 50, 0, 50, 50, 0, 50);
  hm->registerHisto(h2);
  h2 = new TH2D("h_et1_et2",";e_{T1};e_{T2}", 50, 0, 50, 50, 0, 50);
  hm->registerHisto(h2);

  if (!isSim)
    {
      h2 = new TH2D("h_pt1_pt2_mbd",";p_{T1};p_{T2}", 50, 0, 50, 50, 0, 50);
      hm->registerHisto(h2);
      h2 = new TH2D("h_et1_et2_mbd",";e_{T1};e_{T2}", 50, 0, 50, 50, 0, 50);
      hm->registerHisto(h2);

      for (int j = 0; j < 8; j++)
	{
	  h2 = new TH2D(Form("h_pt1_pt2_gl1_%d", j),";p_{T1};p_{T2}", 50, 0, 50, 50, 0, 50);
	  hm->registerHisto(h2);
	  h2 = new TH2D(Form("h_et1_et2_gl1_%d", j),";e_{T1};e_{T2}", 50, 0, 50, 50, 0, 50);
	  hm->registerHisto(h2);
	}
    }

  for (int i = 0; i < 3; i++)
    {
      h = new TH1D(Form("h_Aj_et_%d", i), ";A_j;N", 20, 0, 1);
      hm->registerHisto(h);
      h = new TH1D(Form("h_xj_et_%d", i), ";x_j;N", 20, 0, 1);
      hm->registerHisto(h);

      h = new TH1D(Form("h_dphi_et_%d", i), ";#Delta #phi;N", 32, TMath::Pi()/2., TMath::Pi());
      hm->registerHisto(h);

      h = new TH1D(Form("h_Aj_%d", i), ";A_j;N", 20, 0, 1);
      hm->registerHisto(h);
      h = new TH1D(Form("h_xj_%d", i), ";x_j;N", 20, 0, 1);
      hm->registerHisto(h);

      h = new TH1D(Form("h_dphi_%d", i), ";#Delta #phi;N", 32, TMath::Pi()/2., TMath::Pi());
      hm->registerHisto(h);

      if (!isSim)
	{
	  h = new TH1D(Form("h_Aj_et_mb_%d", i), ";A_j;N", 20, 0, 1);
	  hm->registerHisto(h);
	  h = new TH1D(Form("h_xj_et_mb_%d", i), ";x_j;N", 20, 0, 1);
	  hm->registerHisto(h);
	  h = new TH1D(Form("h_Aj_mb_%d", i), ";A_j;N", 20, 0, 1);
	  hm->registerHisto(h);
	  h = new TH1D(Form("h_xj_mb_%d", i), ";x_j;N", 20, 0, 1);
	  hm->registerHisto(h);

	  h = new TH1D(Form("h_dphi_mb_%d", i), ";#Delta #phi;N", 32, TMath::Pi()/2., TMath::Pi());
	  hm->registerHisto(h);
	  h = new TH1D(Form("h_dphi_et_mb_%d", i), ";#Delta #phi;N", 32, TMath::Pi()/2., TMath::Pi());
	  hm->registerHisto(h);

	  for (int j = 0; j < 8; j++)
	    {
	      h = new TH1D(Form("h_Aj_et_gl1_%d_%d", j, i), ";A_j;N", 20, 0, 1);
	      hm->registerHisto(h);
	      h = new TH1D(Form("h_xj_et_gl1_%d_%d", j, i), ";x_j;N", 20, 0, 1);
	      hm->registerHisto(h);
	      h = new TH1D(Form("h_Aj_gl1_%d_%d", j, i), ";A_j;N", 20, 0, 1);
	      hm->registerHisto(h);
	      h = new TH1D(Form("h_xj_gl1_%d_%d", j, i), ";x_j;N", 20, 0, 1);
	      hm->registerHisto(h);
	      h = new TH1D(Form("h_dphi_gl1_%d_%d", j, i), ";#Delta #phi;N", 32, TMath::Pi()/2., TMath::Pi());
	      hm->registerHisto(h);
	      h = new TH1D(Form("h_dphi_et_gl1_%d_%d", j, i), ";#Delta #phi;N", 32, TMath::Pi()/2., TMath::Pi());
	      hm->registerHisto(h);
	    }
	}
    }
  

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int FindDijets::InitRun(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..

int FindDijets::process_event(PHCompositeNode *topNode)
{


  _i_event++;

  if (!isSim)
    {
      triggeranalyzer->decodeTriggers(topNode);
    }
  RawTowerGeomContainer *tower_geomIH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALIN");
  RawTowerGeomContainer *tower_geomOH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALOUT");


  std::string recoJetName = "AntiKt_Tower_r04_Sub1";

  JetContainer *jets_4 = findNode::getClass<JetContainer>(topNode, recoJetName);

  TowerInfoContainer *hcalin_towers = findNode::getClass<TowerInfoContainer>(topNode, m_calo_nodename + "_HCALIN");
  TowerInfoContainer *hcalout_towers = findNode::getClass<TowerInfoContainer>(topNode, m_calo_nodename + "_HCALOUT");
  TowerInfoContainer *emcalre_towers = findNode::getClass<TowerInfoContainer>(topNode, m_calo_nodename + "_CEMC_RETOWER");

  float background_v2 = 0;
  float background_Psi2 = 0;
  bool has_tower_background = false;
  TowerBackground *towBack = findNode::getClass<TowerBackground>(topNode, "TowerInfoBackground_Sub2");
  if (!towBack)
    {

    }
  else
    {
      has_tower_background = true;
      background_v2 = towBack->get_v2();
      background_Psi2 = towBack->get_Psi2();
    }

  if (!jets_4)
    {
      return Fun4AllReturnCodes::ABORTRUN;
    }

  std::vector<jetty> jets{};
  for (auto jet : *jets_4)
    {
      float jet_total_eT = 0;
      float eFrac_ihcal = 0;
      float eFrac_ohcal = 0;
      float eFrac_emcal = 0;
      
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

	      if (comp.first == 30)
		{  // is sub1
		  if (has_tower_background)
		    {
		      float UE = towBack->get_UE(1).at(ieta);
		      float tower_UE = UE * (1 + 2 * background_v2 * std::cos(2 * (tower_phi - background_Psi2)));
		      tower_eT = (tower->get_energy() - tower_UE) / std::cosh(tower_eta);
		    }
		}

	      eFrac_ihcal += tower_eT;
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
	      float tower_phi = tower_geomOH->get_tower_geometry(key)->get_phi();
	      float tower_eta = tower_geomOH->get_tower_geometry(key)->get_eta();
	      tower_eT = tower->get_energy() / std::cosh(tower_eta);

	      if (comp.first == 31)
		{  // is sub1
		  if (has_tower_background)
		    {
		      float UE = towBack->get_UE(2).at(ieta);
		      float tower_UE = UE * (1 + 2 * background_v2 * std::cos(2 * (tower_phi - background_Psi2)));
		      tower_eT = (tower->get_energy() - tower_UE) / std::cosh(tower_eta);
		    }
		}

	      eFrac_ohcal += tower_eT;
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
	      float tower_phi = tower_geomIH->get_tower_geometry(key)->get_phi();
	      float tower_eta = tower_geomIH->get_tower_geometry(key)->get_eta();
	      tower_eT = tower->get_energy() / std::cosh(tower_eta);

	      if (comp.first == 29)
		{  // is sub1
		  if (has_tower_background)
		    {
		      float UE = towBack->get_UE(0).at(ieta);
		      float tower_UE = UE * (1 + 2 * background_v2 * std::cos(2 * (tower_phi - background_Psi2)));
		      tower_eT = (tower->get_energy() - tower_UE) / std::cosh(tower_eta);
		    }
		}


	      eFrac_emcal += tower_eT;
	      jet_total_eT += tower_eT;
	    }	       
	}
      struct jetty myjet;
      myjet.pt = jet->get_pt();
      myjet.eta = jet->get_eta();
      myjet.phi = jet->get_phi();
      myjet.et = jet_total_eT;
      myjet.emcal = eFrac_emcal;
      myjet.ihcal = eFrac_ihcal;
      myjet.ohcal = eFrac_ohcal;
      if (myjet.et < 4 ) continue;
      jets.push_back(myjet);
    }

  if (jets.size() < 2) 
    return Fun4AllReturnCodes::EVENT_OK;


  auto jets_pt_ordered = jets;
  std::sort(jets.begin(), jets.end(), [](auto a, auto b) { return a.et > b.et; });
  std::sort(jets_pt_ordered.begin(), jets_pt_ordered.end(), [](auto a, auto b) { return a.pt > b.pt; });
  

  const RawTowerDefs::keytype bottomkey = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::HCALIN, 0, 0);
  const RawTowerDefs::keytype topkey    = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::HCALIN, 23, 0);
  float tower_eta_low = tower_geomIH->get_tower_geometry(bottomkey)->get_eta() + 0.4;
  float tower_eta_high = tower_geomIH->get_tower_geometry(topkey)->get_eta() - 0.4;

  // are top two within fiducail range.
  for (int ij = 0; ij < 2; ij++)
    {
      if (jets.at(ij).eta > tower_eta_high || jets.at(ij).eta < tower_eta_low) 
	return Fun4AllReturnCodes::EVENT_OK;
    }

  float dphi = jets.at(0).phi - jets.at(1).phi;
  if (dphi > TMath::Pi())
    {
      dphi = dphi - 2.*TMath::Pi();
    }
  if (dphi <= -1*TMath::Pi())
    {
      dphi = dphi + 2. * TMath::Pi();
    }
  
  dphi = fabs(dphi);

  if (dphi > TMath::Pi()) 
    return Fun4AllReturnCodes::EVENT_OK;

  if (dphi < TMath::Pi()/2.)
    return Fun4AllReturnCodes::EVENT_OK;


    

  auto h_pt1_pt2 = dynamic_cast<TH2*>(hm->getHisto("h_pt1_pt2"));
  h_pt1_pt2->Fill(jets.at(0).pt, jets.at(1).pt);
  h_pt1_pt2->Fill(jets.at(1).pt, jets.at(0).pt);
  auto h_et1_et2 = dynamic_cast<TH2*>(hm->getHisto("h_et1_et2"));
  h_et1_et2->Fill(jets.at(0).et, jets.at(1).et);
  h_et1_et2->Fill(jets.at(1).et, jets.at(0).et);

  if (!isSim)
    {
      if (triggeranalyzer->didTriggerFire("MBD N&S >= 1"))
	{
	  auto h_pt1_pt2_mbd = dynamic_cast<TH2*>(hm->getHisto("h_pt1_pt2_mbd"));
	  h_pt1_pt2_mbd->Fill(jets.at(0).pt, jets.at(1).pt);
	  h_pt1_pt2_mbd->Fill(jets.at(1).pt, jets.at(0).pt);
	  auto h_et1_et2_mbd = dynamic_cast<TH2*>(hm->getHisto("h_et1_et2_mbd"));
	  h_et1_et2_mbd->Fill(jets.at(0).et, jets.at(1).et);
	  h_et1_et2_mbd->Fill(jets.at(1).et, jets.at(0).et);
	}
      for (int i = 0; i < 8; i++)
	{
	  if (triggeranalyzer->didTriggerFire(m_jet_triggernames[i]))
	    {
	      auto h_pt1_pt2_gl1 = dynamic_cast<TH2*>(hm->getHisto(Form("h_pt1_pt2_gl1_%d", i)));
	      h_pt1_pt2_gl1->Fill(jets.at(0).pt, jets.at(1).pt);
	      h_pt1_pt2_gl1->Fill(jets.at(1).pt, jets.at(0).pt);
	      auto h_et1_et2_gl1 = dynamic_cast<TH2*>(hm->getHisto(Form("h_et1_et2_gl1_%d", i)));
	      h_et1_et2_gl1->Fill(jets.at(0).et, jets.at(1).et);
	      h_et1_et2_gl1->Fill(jets.at(1).et, jets.at(0).et);
	    }
	}
    }


  float Aj_pt = (jets_pt_ordered.at(0).pt - jets_pt_ordered.at(1).pt)/(jets_pt_ordered.at(0).pt + jets_pt_ordered.at(1).pt);
  float Aj_et = (jets.at(0).et - jets.at(1).et)/(jets.at(0).et + jets.at(1).et);
  float xj_pt = (jets_pt_ordered.at(1).pt / jets_pt_ordered.at(0).pt);
  float xj_et = (jets.at(1).et / jets.at(0).et);

  if (Verbosity())
    {

      std::cout << "Leading Jet" << std::endl;
      int jl = 0;
      std::cout << "    pt   = " << jets.at(jl).pt << std::endl;
      std::cout << "    et   = " << jets.at(jl).et << std::endl;
      std::cout << "    eta  = " << jets.at(jl).eta << std::endl;
      std::cout << "    phi  = " << jets.at(jl).phi << std::endl;
      std::cout << "Subleading Jet" << std::endl;
      jl++;
      std::cout << "    pt   = " << jets.at(jl).pt << std::endl;
      std::cout << "    et   = " << jets.at(jl).et << std::endl;
      std::cout << "    eta  = " << jets.at(jl).eta << std::endl;
      std::cout << "    phi  = " << jets.at(jl).phi << std::endl;
      std::cout << " " << std::endl;
      std::cout << "Calculated: " << std::endl;
      std::cout << "    A_j   = " << Aj_pt << " ( " << Aj_et << " ) "<< std::endl; 
      std::cout << "    x_j   = " << xj_pt << " ( " << xj_et << " ) "<< std::endl; 
      std::cout << "    dPhi  = " << dphi << std::endl;
      std::cout << " ------------------------ " << std::endl;
    }
  for (int i = 0; i < 3; i++)
    {
      if (jets.at(0).pt > pt_cut1[i] && jets.at(1).pt > pt_cut2[i])
	{
	  auto h_Aj = dynamic_cast<TH1*>(hm->getHisto(Form("h_Aj_%d", i)));
	  h_Aj->Fill(Aj_pt);
	  auto h_xj = dynamic_cast<TH1*>(hm->getHisto(Form("h_xj_%d", i)));
	  h_xj->Fill(xj_pt);
	  auto h_dphi = dynamic_cast<TH1*>(hm->getHisto(Form("h_dphi_%d", i)));
	  h_dphi->Fill(dphi);

	  if (!isSim)
	    {
	      if (triggeranalyzer->didTriggerFire("MBD N&S >= 1"))
		{
		  auto h_Aj_mb = dynamic_cast<TH1*>(hm->getHisto(Form("h_Aj_mb_%d", i)));
		  h_Aj_mb->Fill(Aj_pt);
		  auto h_xj_mb = dynamic_cast<TH1*>(hm->getHisto(Form("h_xj_mb_%d", i)));
		  h_xj_mb->Fill(xj_pt);
		  auto h_dphi_mb = dynamic_cast<TH1*>(hm->getHisto(Form("h_dphi_mb_%d", i)));
		  h_dphi_mb->Fill(dphi);
		}
	    }
	  for (int j = 0; j < 8; j++)
	    {
	      if (triggeranalyzer->didTriggerFire(m_jet_triggernames[j]))
		{
		  auto h_Aj_gl1 = dynamic_cast<TH1*>(hm->getHisto(Form("h_Aj_gl1_%d_%d", j, i)));
		  h_Aj_gl1->Fill(Aj_pt);
		  auto h_xj_gl1 = dynamic_cast<TH1*>(hm->getHisto(Form("h_xj_gl1_%d_%d", j, i)));
		  h_xj_gl1->Fill(xj_pt);
		  auto h_dphi_gl1 = dynamic_cast<TH1*>(hm->getHisto(Form("h_dphi_gl1_%d_%d", j, i)));
		  h_dphi_gl1->Fill(dphi);
		}
	    }
	  
	}
      if (jets.at(0).et > pt_cut1[i] && jets.at(1).et > pt_cut2[i])
	{
	  auto h_Aj = dynamic_cast<TH1*>(hm->getHisto(Form("h_Aj_et_%d", i)));
	  h_Aj->Fill(Aj_et);
	  auto h_xj = dynamic_cast<TH1*>(hm->getHisto(Form("h_xj_et_%d", i)));
	  h_xj->Fill(xj_et);
	  auto h_dphi = dynamic_cast<TH1*>(hm->getHisto(Form("h_dphi_et_%d", i)));
	  h_dphi->Fill(dphi);

	  if (!isSim)
	    {
	      if (triggeranalyzer->didTriggerFire("MBD N&S >= 1"))
		{
		  auto h_Aj_et_mb = dynamic_cast<TH1*>(hm->getHisto(Form("h_Aj_et_mb_%d", i)));
		  h_Aj_et_mb->Fill(Aj_et);
		  auto h_xj_et_mb = dynamic_cast<TH1*>(hm->getHisto(Form("h_xj_et_mb_%d", i)));
		  h_xj_et_mb->Fill(xj_et);
		  auto h_dphi_et_mb = dynamic_cast<TH1*>(hm->getHisto(Form("h_dphi_et_mb_%d", i)));
		  h_dphi_et_mb->Fill(dphi);
		}
	    }
	  for (int j = 0; j < 8; j++)
	    {
	      if (triggeranalyzer->didTriggerFire(m_jet_triggernames[j]))
		{
		  auto h_Aj_et_gl1 = dynamic_cast<TH1*>(hm->getHisto(Form("h_Aj_et_gl1_%d_%d", j, i)));
		  h_Aj_et_gl1->Fill(Aj_et);
		  auto h_xj_et_gl1 = dynamic_cast<TH1*>(hm->getHisto(Form("h_xj_et_gl1_%d_%d", j, i)));
		  h_xj_et_gl1->Fill(xj_et);
		  auto h_dphi_et_gl1 = dynamic_cast<TH1*>(hm->getHisto(Form("h_dphi_et_gl1_%d_%d", j, i)));
		  h_dphi_et_gl1->Fill(dphi);
		}
	    }

	}
    }
  
  return Fun4AllReturnCodes::EVENT_OK;
}



void FindDijets::GetNodes(PHCompositeNode* topNode)
{


}

int FindDijets::ResetEvent(PHCompositeNode *topNode)
{

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int FindDijets::EndRun(const int runnumber)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int FindDijets::End(PHCompositeNode *topNode)
{
  if (Verbosity() > 0)
    {
      std::cout << "FindDijets::End(PHCompositeNode *topNode) This is the End..." << std::endl;
    }
  std::cout<<"Total events: "<<_i_event<<std::endl;
  hm->dumpHistos(_foutname.c_str());

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int FindDijets::Reset(PHCompositeNode *topNode)
{
  if (Verbosity() > 0)
    {
      std::cout << "FindDijets::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
    }
  return Fun4AllReturnCodes::EVENT_OK;
}
