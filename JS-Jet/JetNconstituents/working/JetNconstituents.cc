//____________________________________________________________________________..

#include "JetNconstituents.h"

#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/PHTFileServer.h>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include <g4jets/JetMap.h>
#include <g4jets/Jetv1.h>

#include <centrality/CentralityInfo.h>

#include <calobase/RawTower.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTowerGeomContainer.h>

#include <jetbackground/TowerBackground.h>

#include <TH2D.h>
#include <TH1D.h>

#include <iostream>
#include <string>
#include <vector>

//____________________________________________________________________________..
JetNconstituents::JetNconstituents(const std::string &recojetnameR02, const std::string &recojetnameR04, const std::string &recojetnameR06, const std::string &outputfilename):

 SubsysReco("JetNconstituents_AllR")
  , m_recojetnameR02(recojetnameR02)
  , m_recojetnameR04(recojetnameR04)
  , m_recojetnameR06(recojetnameR06)  
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
 // configure Tree
  h2d_nConstituent_vs_R = new TH2D("h2d_nConstituent_vs_R", "h2d_nConstituent_vs_R", 3, 0, 3, 300, 0, 300);
  // replace x-axis labels with jet radii
  h2d_nConstituent_vs_R->GetXaxis()->SetBinLabel(1, "R=0.2");
  h2d_nConstituent_vs_R->GetXaxis()->SetBinLabel(2, "R=0.4");
  h2d_nConstituent_vs_R->GetXaxis()->SetBinLabel(3, "R=0.6");
  h2d_nConstituent_vs_R->GetXaxis()->SetTitle("Jet Radius");
  h2d_nConstituent_vs_R->GetYaxis()->SetTitle("Number of Constituents");
  h2d_FracEnergy_vs_CaloLayer_R02 = new TH2D("h2d_FracEnergy_vs_CaloLayer_R02", "h2d_FracEnergy_vs_CaloLayer_R02", 3, 0, 3, 100, 0, 1);
  h2d_FracEnergy_vs_CaloLayer_R02->GetXaxis()->SetTitle("Calorimeter Layer ID");
  h2d_FracEnergy_vs_CaloLayer_R02->GetYaxis()->SetTitle("Fraction of Jet Energy");
  // replace x-axis labels with calorimeter layer IDs
  h2d_FracEnergy_vs_CaloLayer_R02->GetXaxis()->SetBinLabel(1, "EMCal");
  h2d_FracEnergy_vs_CaloLayer_R02->GetXaxis()->SetBinLabel(2, "HCalIn");
  h2d_FracEnergy_vs_CaloLayer_R02->GetXaxis()->SetBinLabel(3, "HCalOut");
  h2d_FracEnergy_vs_CaloLayer_R04 = new TH2D("h2d_FracEnergy_vs_CaloLayer_R04", "h2d_FracEnergy_vs_CaloLayer_R04", 5, 0, 3, 100, 0, 1);
  h2d_FracEnergy_vs_CaloLayer_R04->GetXaxis()->SetTitle("Calorimeter Layer ID");
  h2d_FracEnergy_vs_CaloLayer_R04->GetYaxis()->SetTitle("Fraction of Jet Energy");
  // replace x-axis labels with calorimeter layer IDs
  h2d_FracEnergy_vs_CaloLayer_R04->GetXaxis()->SetBinLabel(1, "EMCal");
  h2d_FracEnergy_vs_CaloLayer_R04->GetXaxis()->SetBinLabel(2, "HCalIn");
  h2d_FracEnergy_vs_CaloLayer_R04->GetXaxis()->SetBinLabel(3, "HCalOut");
  h2d_FracEnergy_vs_CaloLayer_R06 = new TH2D("h2d_FracEnergy_vs_CaloLayer_R06", "h2d_FracEnergy_vs_CaloLayer_R06",  5, 0, 3, 100, 0, 1);
  h2d_FracEnergy_vs_CaloLayer_R06->GetXaxis()->SetTitle("Calorimeter Layer ID");
  h2d_FracEnergy_vs_CaloLayer_R06->GetYaxis()->SetTitle("Fraction of Jet Energy");
  // replace x-axis labels with calorimeter layer IDs
  h2d_FracEnergy_vs_CaloLayer_R06->GetXaxis()->SetBinLabel(1, "EMCal");
  h2d_FracEnergy_vs_CaloLayer_R06->GetXaxis()->SetBinLabel(2, "HCalIn");
  h2d_FracEnergy_vs_CaloLayer_R06->GetXaxis()->SetBinLabel(3, "HCalOut");


  h1d_nConstituent_R02 = new TH1D("h1d_nConstituent_R02", "h1d_nConstituent_R02", 350, 0, 350);
  h1d_nConstituent_R02->GetXaxis()->SetTitle("Number of Constituents");
  h1d_nConstituent_R02->GetYaxis()->SetTitle("Number of Jets");
  h1d_nConstituent_R04 = new TH1D("h1d_nConstituent_R04", "h1d_nConstituent_R04", 350, 0, 350);
  h1d_nConstituent_R04->GetXaxis()->SetTitle("Number of Constituents");
  h1d_nConstituent_R04->GetYaxis()->SetTitle("Number of Jets");
  h1d_nConstituent_R06 = new TH1D("h1d_nConstituent_R06", "h1d_nConstituent_R06", 350, 0, 350);
  h1d_nConstituent_R06->GetXaxis()->SetTitle("Number of Constituents");
  h1d_nConstituent_R06->GetYaxis()->SetTitle("Number of Jets");

 
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
  std::vector<std::string> m_recojetname_string = {m_recojetnameR02, m_recojetnameR04, m_recojetnameR06};
  std::vector<float> m_jet_radii = {0.2, 0.4, 0.6};
  int n_radii = m_jet_radii.size();
  RawTowerContainer *towersEM3 = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_CEMC_RETOWER");
  RawTowerContainer *towersIH3 = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_HCALIN");
  RawTowerContainer *towersOH3 = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_HCALOUT");
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

  for(int i_radii =0; i_radii<n_radii; i_radii++){
    // get reco jet name
    std::string recojetname = m_recojetname_string[i_radii];
    //std::cout << recojetname << std::endl;
    // get DST reco jet map
    JetMap* jets = findNode::getClass<JetMap>(topNode, recojetname);
    if(!jets){
      std::cout << "JetNconstituents::process_event - Error can not find DST Reco JetMap node " << recojetname << std::endl;
      continue;
    }
    // loop over jets
   for(JetMap::Iter iter = jets->begin(); iter != jets->end(); ++iter){
      // get jet object 
      
      Jet* jet = iter->second;
      // apply eta and pt cuts
      bool eta_cut = (jet->get_eta() >= m_etaRange.first) and (jet->get_eta() <= m_etaRange.second);
	    bool pt_cut = (jet->get_pt() >= m_ptRange.first) and (jet->get_pt() <= m_ptRange.second);
      if ((not eta_cut) or (not pt_cut)) continue;
      // to remove noise jets
      if(jet->get_pt() < 1) continue; 

      // fill nConstituent vs R
      if(i_radii==0) h2d_nConstituent_vs_R->Fill(0., jet->size_comp());
      else if(i_radii==1) h2d_nConstituent_vs_R->Fill(1., jet->size_comp());
      else if(i_radii==2) h2d_nConstituent_vs_R->Fill(2., jet->size_comp());
   
    
      if(i_radii==0) h1d_nConstituent_R02->Fill(jet->size_comp());
      else if(i_radii==1) h1d_nConstituent_R04->Fill(jet->size_comp());
      else if(i_radii==2) h1d_nConstituent_R06->Fill(jet->size_comp());
      //int n_constituents = 0;
      float ohcal_energy_sum = 0;
      float ihcal_energy_sum = 0;
      float emcal_energy_sum = 0;
      float jet_energy = jet->get_e();
      int n_constituents_emcal = 0;
      int n_constituents_ihcal = 0;
      int n_constituents_ohcal = 0;
      for (Jet::ConstIter comp = jet->begin_comp(); comp != jet->end_comp(); ++comp)
      {
        RawTower *tower;
        if ((*comp).first == 15){
          tower = towersIH3->getTower((*comp).second);
          if(!tower || !tower_geom){
            continue;
          }
          ihcal_energy_sum += tower->get_energy();
          n_constituents_ihcal++;
        }
        else if ((*comp).first == 16){
          tower = towersOH3->getTower((*comp).second);
		      if(!tower || !tower_geomOH)
          {
            continue;
          } 
          ohcal_energy_sum += tower->get_energy();
          n_constituents_ohcal++;
        }
        else if ((*comp).first == 14){
          tower = towersEM3->getTower((*comp).second);
          if(!tower || !tower_geom){
            continue;
          }
          emcal_energy_sum += tower->get_energy();
          n_constituents_emcal++;
        }
    
      }
      float o_hcal_sum = ohcal_energy_sum/jet_energy;
      float i_hcal_sum = ihcal_energy_sum/jet_energy;
      float emcal_sum = emcal_energy_sum/jet_energy;
      if(i_radii==0) {
        h2d_FracEnergy_vs_CaloLayer_R02->Fill(2., o_hcal_sum);
        h2d_FracEnergy_vs_CaloLayer_R02->Fill(0., emcal_sum);
        h2d_FracEnergy_vs_CaloLayer_R02->Fill(1., i_hcal_sum);
      }
      else if(i_radii==1) {
        h2d_FracEnergy_vs_CaloLayer_R04->Fill(2., o_hcal_sum);
        h2d_FracEnergy_vs_CaloLayer_R04->Fill(0., emcal_sum);
        h2d_FracEnergy_vs_CaloLayer_R04->Fill(1., i_hcal_sum);
      }
      else if(i_radii==2){
        h2d_FracEnergy_vs_CaloLayer_R06->Fill(2., o_hcal_sum);
        h2d_FracEnergy_vs_CaloLayer_R06->Fill(0., emcal_sum);
        h2d_FracEnergy_vs_CaloLayer_R06->Fill(1., i_hcal_sum);
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

  h2d_nConstituent_vs_R->Write();
  h1d_nConstituent_R02->Write();
  h1d_nConstituent_R04->Write();
  h1d_nConstituent_R06->Write();
  h2d_FracEnergy_vs_CaloLayer_R02->Write();
  h2d_FracEnergy_vs_CaloLayer_R04->Write();
  h2d_FracEnergy_vs_CaloLayer_R06->Write();
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
