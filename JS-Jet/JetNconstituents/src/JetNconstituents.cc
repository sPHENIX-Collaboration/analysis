#include "JetNconstituents.h"

// fun4all includes
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/PHTFileServer.h>

// phool includes
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

// jet includes
#include <jetbase/Jet.h>
#include <jetbase/Jetv2.h>
#include <jetbase/JetContainer.h>
#include <jetbase/JetContainerv1.h>

// calobase includes
#include <calobase/RawTower.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfo.h>

// jetbackground includes
#include <jetbackground/TowerBackground.h>

#include <TH2D.h>
#include <TH1D.h>

#include <iostream>
#include <string>
#include <vector>

JetNconstituents::JetNconstituents(const std::string &outputfilename)
  : SubsysReco("JetNconstituents")
  , m_outputFileName(outputfilename)
  // these are all initialized but included here for clarity
  , m_etaRange(-1.1, 1.1)
  , m_ptRange(1.0, 1000)
  , m_recoJetName("AntiKt_Tower_r04")
  , h1_jetNconstituents_total(nullptr)
  , h1_jetNconstituents_IHCAL(nullptr)
  , h1_jetNconstituents_OHCAL(nullptr)
  , h1_jetNconstituents_CEMC(nullptr)
  , h2_jetNconstituents_vs_caloLayer(nullptr)
  , h1_jetFracE_IHCAL(nullptr)
  , h1_jetFracE_OHCAL(nullptr)
  , h1_jetFracE_CEMC(nullptr)
  , h2_jetFracE_vs_caloLayer(nullptr)
{
}


int JetNconstituents::Init(PHCompositeNode *topNode)
{
  if (Verbosity() > 0)
  {
    std::cout << "JetNconstituents::Init - Output to " << m_outputFileName << std::endl;
  } 

  // create output file
  PHTFileServer::get().open(m_outputFileName, "RECREATE");


  // Initialize histograms
  const int N_const = 200;
  Double_t N_const_bins[N_const+1];
  for (int i = 0; i <= N_const; i++)
  {
    N_const_bins[i] = 1.0*i;
  }

  const int N_frac = 100;
  double frac_max = 1.0;
  Double_t frac_bins[N_frac+1];
  for (int i = 0; i <= N_frac; i++)
  {
    frac_bins[i] = (frac_max/N_frac)*i;
  }

  const int N_calo_layers = 3;
  Double_t calo_layers_bins[N_calo_layers+1] = {0.5, 1.5, 2.5, 3.5}; // emcal, ihcal, ohcal

  h1_jetNconstituents_total = new TH1D("h1_jetNconstituents_total", "Jet N constituents", N_const, N_const_bins);
  h1_jetNconstituents_IHCAL = new TH1D("h1_jetNconstituents_IHCAL", "Jet N constituents in IHCal", N_const, N_const_bins);
  h1_jetNconstituents_OHCAL = new TH1D("h1_jetNconstituents_OHCAL", "Jet N constituents in OHCal", N_const, N_const_bins);
  h1_jetNconstituents_CEMC = new TH1D("h1_jetNconstituents_CEMC", "Jet N constituents in CEMC", N_const, N_const_bins);
  h2_jetNconstituents_vs_caloLayer = new TH2D("h2_jetNconstituents_vs_caloLayer", "Jet N constituents vs Calo Layer", N_calo_layers, calo_layers_bins, N_const, N_const_bins);

  h1_jetFracE_IHCAL = new TH1D("h1_jetFracE_IHCAL", "Jet E fraction in IHCal", N_frac, frac_bins);
  h1_jetFracE_OHCAL = new TH1D("h1_jetFracE_OHCAL", "Jet E fraction in OHCal", N_frac, frac_bins);
  h1_jetFracE_CEMC = new TH1D("h1_jetFracE_CEMC", "Jet E fraction in CEMC", N_frac, frac_bins);
  h2_jetFracE_vs_caloLayer = new TH2D("h2_jetFracE_vs_caloLayer", "Jet E fraction vs Calo Layer", N_calo_layers, calo_layers_bins, N_frac, frac_bins);

  if(Verbosity() > 0)
  {
    std::cout << "JetNconstituents::Init - Histograms created" << std::endl;
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

int JetNconstituents::process_event(PHCompositeNode *topNode)
{
  
  if(Verbosity() > 1)
  {
    std::cout << "JetNconstituents::process_event - Process event..." << std::endl;
  }

  // get the jets
  JetContainer* jets = findNode::getClass<JetContainer>(topNode, m_recoJetName);
  if(!jets)
  {
    std::cout <<"JetNconstituents::process_event - Error can not find jet node " << m_recoJetName << std::endl;
    exit(-1); // fatal
  }

  // get unsub towers
  TowerInfoContainer *towersEM3 = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC_RETOWER");
  TowerInfoContainer *towersIH3 = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALIN");
  TowerInfoContainer *towersOH3 = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT"); 
  if(!towersEM3 || !towersIH3 || !towersOH3)
  {
    std::cout <<"JetNconstituents::process_event - Error can not find tower node " << std::endl;
    exit(-1); // fatal
  }

  // get tower geometry
  RawTowerGeomContainer *tower_geomIH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALIN");
  RawTowerGeomContainer *tower_geomOH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALOUT");
  if(!tower_geomIH || !tower_geomOH)
  {
    std::cout <<"JetNconstituents::process_event - Error can not find tower geometry node " << std::endl;
    exit(-1); // fatal
  }

  // get underlying event
  float background_v2 = 0;
  float background_Psi2 = 0;
  bool has_tower_background = false;
  TowerBackground *towBack = findNode::getClass<TowerBackground>(topNode, "TowerInfoBackground_Sub2");
  if(!towBack)
  {
    std::cout <<"JetNconstituents::process_event - Error can not find tower background node " << std::endl;  
  }
  else
  {
    has_tower_background = true;
    background_v2 = towBack->get_v2();
    background_Psi2 = towBack->get_Psi2();
  }

 
  // loop over jets
  for (auto jet : *jets)
  {    

    // remove noise
    if(jet->get_pt() < 1) continue;

    // apply eta and pt cuts
    bool eta_cut = (jet->get_eta() >= m_etaRange.first) and (jet->get_eta() <= m_etaRange.second);
    bool pt_cut = (jet->get_pt() >= m_ptRange.first) and (jet->get_pt() <= m_ptRange.second);
    if ((not eta_cut) or (not pt_cut)) continue;
  

    // zero out counters
    int n_comp_total = 0;
    int n_comp_ihcal = 0;
    int n_comp_ohcal = 0;
    int n_comp_emcal = 0;

    float jet_total_eT = 0;
    float eFrac_ihcal = 0;
    float eFrac_ohcal = 0;
    float eFrac_emcal = 0;
    
    // loop over jet constituents
    for (auto comp: jet->get_comp_vec())
	  {

      // get tower
      unsigned int channel = comp.second;
      TowerInfo *tower;
      
      float tower_eT = 0;

      // get tower info
      if(comp.first == 26 || comp.first == 30)
      { // IHcal 

        tower = towersIH3->get_tower_at_channel(channel);
        
        if(!tower || !tower_geomIH){ continue; }
        
        unsigned int calokey = towersIH3->encode_key(channel);
        int ieta = towersIH3->getTowerEtaBin(calokey);
	      int iphi = towersIH3->getTowerPhiBin(calokey);
	      const RawTowerDefs::keytype key = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::HCALIN, ieta, iphi);
	      float tower_phi = tower_geomIH->get_tower_geometry(key)->get_phi();
	      float tower_eta = tower_geomIH->get_tower_geometry(key)->get_eta();
        tower_eT = tower->get_energy()/cosh(tower_eta);

        if(comp.first == 30)
        { // is sub1
          if(has_tower_background)
          {
            float UE = towBack->get_UE(1).at(ieta);
            float tower_UE = UE *( 1 + 2 * background_v2 * cos(2 * (tower_phi - background_Psi2)));
            tower_eT = (tower->get_energy() - tower_UE)/cosh(tower_eta);
          }
        }

        eFrac_ihcal+= tower_eT;
        jet_total_eT += tower_eT;
        n_comp_ihcal++;
        n_comp_total++;
        
      }
      else if(comp.first == 27 || comp.first == 31)
      { // OHcal 
       
        tower = towersOH3->get_tower_at_channel(channel);
       
        if(!tower || !tower_geomOH){ continue; }

        unsigned int calokey = towersOH3->encode_key(channel);
        int ieta = towersOH3->getTowerEtaBin(calokey);
        int iphi = towersOH3->getTowerPhiBin(calokey);
        const RawTowerDefs::keytype key = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::HCALOUT, ieta, iphi);
        float tower_phi = tower_geomOH->get_tower_geometry(key)->get_phi();
        float tower_eta = tower_geomOH->get_tower_geometry(key)->get_eta();
        tower_eT = tower->get_energy()/cosh(tower_eta);

        if(comp.first == 31)
        { // is sub1
          if(has_tower_background)
          {
            float UE = towBack->get_UE(2).at(ieta);
            float tower_UE = UE *( 1 + 2 * background_v2 * cos(2 * (tower_phi - background_Psi2)));
            tower_eT = (tower->get_energy() - tower_UE)/cosh(tower_eta);
          }
        }

        eFrac_ohcal+= tower_eT;
        jet_total_eT += tower_eT;
        n_comp_ohcal++;
        n_comp_total++;

      }
      else if(comp.first == 28 || comp.first == 29)
      { // EMCal (retowered)
       
        tower = towersEM3->get_tower_at_channel(channel);
        
        if(!tower || !tower_geomIH){ continue; }
      

        unsigned int calokey = towersEM3->encode_key(channel);
        int ieta = towersEM3->getTowerEtaBin(calokey);
        int iphi = towersEM3->getTowerPhiBin(calokey);
        const RawTowerDefs::keytype key = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::HCALIN, ieta, iphi);
        float tower_phi = tower_geomIH->get_tower_geometry(key)->get_phi();
        float tower_eta = tower_geomIH->get_tower_geometry(key)->get_eta();
        tower_eT = tower->get_energy()/cosh(tower_eta);

        if(comp.first == 29)
        { // is sub1
          if(has_tower_background)
          {
            float UE = towBack->get_UE(0).at(ieta);
            float tower_UE = UE *( 1 + 2 * background_v2 * cos(2 * (tower_phi - background_Psi2)));
            tower_eT = (tower->get_energy() - tower_UE)/cosh(tower_eta);
          }
        }

        eFrac_emcal+= tower_eT;
        jet_total_eT += tower_eT;
        n_comp_emcal++;
        n_comp_total++;
      }


    }

    // normalize energy fractions
    eFrac_ihcal/= jet_total_eT;
    eFrac_ohcal/= jet_total_eT;
    eFrac_emcal/= jet_total_eT;

    // fill histograms
    h1_jetNconstituents_total->Fill(1.0*n_comp_total);
    h1_jetNconstituents_IHCAL->Fill(1.0*n_comp_ihcal);
    h1_jetNconstituents_OHCAL->Fill(1.0*n_comp_ohcal);
    h1_jetNconstituents_CEMC->Fill(1.0*n_comp_emcal);
    h2_jetNconstituents_vs_caloLayer->Fill(1.0, 1.0*n_comp_emcal);
    h2_jetNconstituents_vs_caloLayer->Fill(2.0, 1.0*n_comp_ihcal);
    h2_jetNconstituents_vs_caloLayer->Fill(3.0, 1.0*n_comp_ohcal);

    h1_jetFracE_IHCAL->Fill(eFrac_ihcal);
    h1_jetFracE_OHCAL->Fill(eFrac_ohcal);
    h1_jetFracE_CEMC->Fill(eFrac_emcal);

    h2_jetFracE_vs_caloLayer->Fill(1.0, eFrac_emcal);
    h2_jetFracE_vs_caloLayer->Fill(2.0, eFrac_ihcal);
    h2_jetFracE_vs_caloLayer->Fill(3.0, eFrac_ohcal);




  }

  if(Verbosity() > 1)
  {
    std::cout << "JetNconstituents::process_event - Event processed" << std::endl;
  }
 
  return Fun4AllReturnCodes::EVENT_OK;
}


//____________________________________________________________________________..
int JetNconstituents::End(PHCompositeNode *topNode)
{
  
  if(Verbosity() > 0)
  {
    std::cout << "JetNconstituents::EndRun - End run " << std::endl;
    std::cout << "JetNconstituents::EndRun - Writing to " << m_outputFileName << std::endl;
  }

  PHTFileServer::get().cd(m_outputFileName);
  
  // write histograms to root file
  h1_jetNconstituents_total->Write();
  h1_jetNconstituents_IHCAL->Write();
  h1_jetNconstituents_OHCAL->Write();
  h1_jetNconstituents_CEMC->Write();
  h2_jetNconstituents_vs_caloLayer->Write();

  h1_jetFracE_IHCAL->Write();
  h1_jetFracE_OHCAL->Write();
  h1_jetFracE_CEMC->Write();
  h2_jetFracE_vs_caloLayer->Write();  
 
  if(Verbosity() > 0)
  {
    std::cout << "JetNconstituents::EndRun - Done" << std::endl;
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

