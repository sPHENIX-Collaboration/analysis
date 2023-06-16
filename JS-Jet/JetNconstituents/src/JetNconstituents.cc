//____________________________________________________________________________..

#include "JetNconstituents.h"

#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/PHTFileServer.h>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include <jetbase/JetMap.h>
#include <jetbase/Jetv1.h>

#include <centrality/CentralityInfo.h>

#include <calobase/RawTower.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfo.h>

#include <jetbackground/TowerBackground.h>

#include <TH2D.h>
#include <TH1D.h>

#include <iostream>
#include <string>
#include <vector>

//____________________________________________________________________________..
JetNconstituents::JetNconstituents(const std::string &recojetname, const std::string &outputfilename):

 SubsysReco("JetNconstituents_" + recojetname)
  , m_recoJetName(recojetname)
  , m_outputFileName(outputfilename)
  , m_etaRange(-1.1, 1.1)
  , m_ptRange(5, 100)
{
  std::cout << "JetNconstituents::JetNconstituents(const std::string& recojetname, const std::string& outputfilename) Calling ctor" << std::endl;
}

//____________________________________________________________________________..
JetNconstituents::~JetNconstituents()
{
  std::cout << "JetNconstituents::~JetNconstituents() Calling dtor" << std::endl;
}


//____________________________________________________________________________..
int JetNconstituents::Init(PHCompositeNode *topNode)
{
  std::cout << "JetNconstituents::Init(PHCompositeNode *topNode) Initializing" << std::endl;
  PHTFileServer::get().open(m_outputFileName, "RECREATE");
  std::cout << "JetValidation::Init - Output to " << m_outputFileName << std::endl;

  // configure histograms
  h1d_nConsituents = new TH1D("h1d_nConsituents", "h1d_nConsituents", 300, 0, 300);
  h1d_nConsituents->GetXaxis()->SetTitle("N_{Consts}");
  h1d_nConsituents->GetYaxis()->SetTitle("Jet Counts");

  h1d_nConsituents_IHCal = new TH1D("h1d_nConsituents_IHCal", "h1d_nConsituents_IHCal", 300, 0, 300);
  h1d_nConsituents_IHCal->GetXaxis()->SetTitle("N_{Consts}");
  h1d_nConsituents_IHCal->GetYaxis()->SetTitle("Jet Counts");

  h1d_nConsituents_OHCal = new TH1D("h1d_nConsituents_OHCal", "h1d_nConsituents_OHCal", 300, 0, 300);
  h1d_nConsituents_OHCal->GetXaxis()->SetTitle("N_{Consts}");
  h1d_nConsituents_OHCal->GetYaxis()->SetTitle("Jet Counts");

  h1d_nConsituents_EMCal = new TH1D("h1d_nConsituents_EMCal", "h1d_nConsituents_EMCal", 300, 0, 300);
  h1d_nConsituents_EMCal->GetXaxis()->SetTitle("N_{Consts}");
  h1d_nConsituents_EMCal->GetYaxis()->SetTitle("Jet Counts");

  h1d_FracEnergy_EMCal = new TH1D("h1d_FracEnergy_EMCal", "h1d_FracEnergy_EMCal", 100, 0, 1);
  h1d_FracEnergy_EMCal->GetXaxis()->SetTitle("Fraction of Jet Energy");
  h1d_FracEnergy_EMCal->GetYaxis()->SetTitle("Jet Counts");

  h1d_FracEnergy_IHCal = new TH1D("h1d_FracEnergy_IHCal", "h1d_FracEnergy_IHCal", 100, 0, 1);
  h1d_FracEnergy_IHCal->GetXaxis()->SetTitle("Fraction of Jet Energy");
  h1d_FracEnergy_IHCal->GetYaxis()->SetTitle("Jet Counts");

  h1d_FracEnergy_OHCal = new TH1D("h1d_FracEnergy_OHCal", "h1d_FracEnergy_OHCal", 100, 0, 1);
  h1d_FracEnergy_OHCal->GetXaxis()->SetTitle("Fraction of Jet Energy");
  h1d_FracEnergy_OHCal->GetYaxis()->SetTitle("Jet Counts");
  
  h2d_FracEnergy_vs_CaloLayer = new TH2D("h2d_FracEnergy_vs_CaloLayer", "h2d_FracEnergy_vs_CaloLayer", 3, 0, 3, 100, 0, 1);
  h2d_FracEnergy_vs_CaloLayer->GetXaxis()->SetTitle("Calorimeter Layer ID");
  h2d_FracEnergy_vs_CaloLayer->GetYaxis()->SetTitle("Fraction of Jet Energy");
  h2d_FracEnergy_vs_CaloLayer->GetXaxis()->SetBinLabel(1, "EMCal");
  h2d_FracEnergy_vs_CaloLayer->GetXaxis()->SetBinLabel(2, "HCalIn");
  h2d_FracEnergy_vs_CaloLayer->GetXaxis()->SetBinLabel(3, "HCalOut");

  h2d_nConstituent_vs_CaloLayer = new TH2D("h2d_nConstituent_vs_CaloLayer", "h2d_nConstituent_vs_CaloLayer", 3, 0, 3, 200, 0, 200);
  h2d_nConstituent_vs_CaloLayer->GetXaxis()->SetTitle("Calorimeter Layer ID");
  h2d_nConstituent_vs_CaloLayer->GetYaxis()->SetTitle("Number of Constituents");
  h2d_nConstituent_vs_CaloLayer->GetXaxis()->SetBinLabel(1, "EMCal");
  h2d_nConstituent_vs_CaloLayer->GetXaxis()->SetBinLabel(2, "HCalIn");
  h2d_nConstituent_vs_CaloLayer->GetXaxis()->SetBinLabel(3, "HCalOut");
  
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetNconstituents::InitRun(PHCompositeNode *topNode)
{
  std::cout << "JetNconstituents::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetNconstituents::process_event(PHCompositeNode *topNode)
{
  //std::cout << "JetValidation::process_event(PHCompositeNode *topNode) Processing Event" << std::endl;
  
  // get the jets
  JetMap* jets = findNode::getClass<JetMap>(topNode, m_recoJetName);
  if(!jets){
      std::cout << "JetNconstituents::process_event - Error can not find DST Reco JetMap node " << m_recoJetName << std::endl;
      exit(-1);
  }

  TowerInfoContainer *towersEM3 = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC_RETOWER");
  TowerInfoContainer *towersIH3 = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALIN");
  TowerInfoContainer *towersOH3 = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT");
  RawTowerGeomContainer *tower_geom = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALIN");
  RawTowerGeomContainer *tower_geomOH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALOUT");
  if(!towersEM3 || !towersIH3 || !towersOH3){
    std::cout
      <<"MyJetAnalysis::process_event - Error can not find raw tower node "
      << std::endl;
    exit(-1);
  }

  if(!tower_geom || !tower_geomOH){
    std::cout
      <<"MyJetAnalysis::process_event - Error can not find raw tower geometry "
      << std::endl;
    exit(-1);
  }

  // loop over jets
  for(JetMap::Iter iter = jets->begin(); iter != jets->end(); ++iter){
      
      Jet* jet = iter->second;

      // apply eta and pt cuts
      bool eta_cut = (jet->get_eta() >= m_etaRange.first) and (jet->get_eta() <= m_etaRange.second);
	    bool pt_cut = (jet->get_pt() >= m_ptRange.first) and (jet->get_pt() <= m_ptRange.second);
      if ((not eta_cut) or (not pt_cut)) continue;
      // to remove noise jets
      if(jet->get_pt() < 1) continue; 

      // fill histograms
      h1d_nConsituents->Fill(jet->size_comp());

      float ohcal_energy_sum = 0;
      float ihcal_energy_sum = 0;
      float emcal_energy_sum = 0;
      // float jet_energy = jet->get_e();


      float jet_energy_sum = 0;
      int n_constituents_emcal = 0;
      int n_constituents_ihcal = 0;
      int n_constituents_ohcal = 0;
      int nconst = 0;

      for (Jet::ConstIter comp = jet->begin_comp(); comp != jet->end_comp(); ++comp)
	    {
	      TowerInfo *tower;
	      nconst++;
        unsigned int channel = (*comp).second;
	      
	      if ((*comp).first == 15 ||  (*comp).first == 30)
		    {
		      tower = towersIH3->get_tower_at_channel(channel);
		      if(!tower || !tower_geom){ continue; }
          ihcal_energy_sum += tower->get_energy();
          n_constituents_ihcal++;
          jet_energy_sum += tower->get_energy();
        }
        else if ((*comp).first == 16 ||  (*comp).first == 31)
        {
          tower = towersOH3->get_tower_at_channel(channel);
          if(!tower || !tower_geomOH){ continue; }
          ohcal_energy_sum += tower->get_energy();
          n_constituents_ohcal++;
          jet_energy_sum += tower->get_energy();
        }
        else if ((*comp).first == 14 || (*comp).first == 29){
          tower = towersEM3->get_tower_at_channel(channel);
          if(!tower || !tower_geom){ continue; }
          emcal_energy_sum += tower->get_energy();
          n_constituents_emcal++;
          jet_energy_sum += tower->get_energy();
        }
      }

      float eFrac_emcal = emcal_energy_sum/jet_energy_sum;
      float eFrac_ihcal = ihcal_energy_sum/jet_energy_sum;
      float eFrac_ohcal = ohcal_energy_sum/jet_energy_sum;

      // fill histograms
      h1d_nConsituents_IHCal->Fill(n_constituents_ihcal);
      h1d_nConsituents_OHCal->Fill(n_constituents_ohcal);
      h1d_nConsituents_EMCal->Fill(n_constituents_emcal);
      h1d_nConsituents->Fill(nconst);
      h1d_FracEnergy_EMCal->Fill(eFrac_emcal);
      h1d_FracEnergy_IHCal->Fill(eFrac_ihcal);
      h1d_FracEnergy_OHCal->Fill(eFrac_ohcal);

      for (int ibin =1; ibin< h2d_FracEnergy_vs_CaloLayer->GetNbinsX()+1; ibin++){
        float bin_center = h2d_FracEnergy_vs_CaloLayer->GetXaxis()->GetBinCenter(ibin);
        if(ibin == 1){
          h2d_FracEnergy_vs_CaloLayer->Fill(bin_center, eFrac_emcal);
          h2d_nConstituent_vs_CaloLayer->Fill(bin_center, n_constituents_emcal);
        }
        else if(ibin == 2){
          h2d_FracEnergy_vs_CaloLayer->Fill(bin_center, eFrac_ihcal);
          h2d_nConstituent_vs_CaloLayer->Fill(bin_center, n_constituents_ihcal);
        }
        else if(ibin == 3){
          h2d_FracEnergy_vs_CaloLayer->Fill(bin_center, eFrac_ohcal);
          h2d_nConstituent_vs_CaloLayer->Fill(bin_center, n_constituents_ohcal);
        }
      }

  }


 
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetNconstituents::ResetEvent(PHCompositeNode *topNode)
{
 // std::cout << "JetNconstituents::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << std::endl;

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetNconstituents::EndRun(const int runnumber)
{
  std::cout << "JetNconstituents::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
  std::cout << "JetValidation::End - Output to " << m_outputFileName << std::endl;
  PHTFileServer::get().cd(m_outputFileName);
  // float ptmin = m_ptRange.first;
  // float ptmax = m_ptRange.second;

  // write histograms to root file
  h1d_nConsituents->Write();
  h1d_nConsituents_IHCal->Write();
  h1d_nConsituents_OHCal->Write();
  h1d_nConsituents_EMCal->Write();
  h1d_FracEnergy_EMCal->Write();
  h1d_FracEnergy_IHCal->Write();
  h1d_FracEnergy_OHCal->Write();
  h2d_FracEnergy_vs_CaloLayer->Write();
  h2d_nConstituent_vs_CaloLayer->Write();



    
  std::cout << "JetValidation::End(PHCompositeNode *topNode) This is the End..." << std::endl;

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetNconstituents::End(PHCompositeNode *topNode)
{
  std::cout << "JetNconstituents::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetNconstituents::Reset(PHCompositeNode *topNode)
{
 //std::cout << "JetNconstituents::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void JetNconstituents::Print(const std::string &what) const
{
  std::cout << "JetNconstituents::Print(const std::string &what) const Printing info for " << what << std::endl;
}
