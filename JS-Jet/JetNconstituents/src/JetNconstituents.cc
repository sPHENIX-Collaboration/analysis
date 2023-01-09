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
#include <TStyle.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TLine.h>
#include <iostream>
#include <string>
#include <vector>

//____________________________________________________________________________..
JetNconstituents::JetNconstituents(const std::string &recojetnameR02, const std::string &recojetnameR04, const std::string &recojetnameR06, const std::string &outputfilename):

 SubsysReco("JetNconstituents_AllR")
  , m_recojetnameR02(recojetnameR02)
  , m_recojetnameR04(recojetnameR04) 
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
  h2d_nConstituent_vs_R = new TH2D("h2d_nConstituent_vs_R", "h2d_nConstituent_vs_R",2,0,2, 300, 0, 300);
  h2d_nConstituent_vs_R->GetXaxis()->SetTitle("Jet Radius");
  h2d_nConstituent_vs_R->GetYaxis()->SetTitle("Number of Constituents");
  // change the bin labels
  h2d_nConstituent_vs_R->GetXaxis()->SetBinLabel(1, "0.2");
  h2d_nConstituent_vs_R->GetXaxis()->SetBinLabel(2, "0.4");

  h2d_SumEnergyDebug_vs_R = new TH2D("h2d_SumEnergyDebug_vs_R", "h2d_SumEnergyDebug_vs_R",2,0,2, 300, 0, 300);
  h2d_SumEnergyDebug_vs_R->GetXaxis()->SetTitle("Jet Radius");
  h2d_SumEnergyDebug_vs_R->GetYaxis()->SetTitle("Sum Energy");
  // change the bin labels
  h2d_SumEnergyDebug_vs_R->GetXaxis()->SetBinLabel(1, "0.2");
  h2d_SumEnergyDebug_vs_R->GetXaxis()->SetBinLabel(2, "0.4");

  std::vector<float> m_jet_radii = {0.2, 0.4};
  h2d_FracEnergy_vs_CaloLayer_R02 = new TH2D("h2d_FracEnergy_vs_CaloLayer_R02", "h2d_FracEnergy_vs_CaloLayer_R02", 3, 0, 3, 100, 0, 1);
  h2d_FracEnergy_vs_CaloLayer_R02->GetXaxis()->SetTitle("Calorimeter Layer ID");
  h2d_FracEnergy_vs_CaloLayer_R02->GetYaxis()->SetTitle("Fraction of Jet Energy");
  h2d_FracEnergy_vs_CaloLayer_R02->GetXaxis()->SetBinLabel(1, "EMCal");
  h2d_FracEnergy_vs_CaloLayer_R02->GetXaxis()->SetBinLabel(2, "HCalIn");
  h2d_FracEnergy_vs_CaloLayer_R02->GetXaxis()->SetBinLabel(3, "HCalOut");
  h2d_FracEnergy_vs_CaloLayer_R04 = new TH2D("h2d_FracEnergy_vs_CaloLayer_R04", "h2d_FracEnergy_vs_CaloLayer_R04", 3, 0, 3, 100, 0, 1);
  h2d_FracEnergy_vs_CaloLayer_R04->GetXaxis()->SetTitle("Calorimeter Layer ID");
  h2d_FracEnergy_vs_CaloLayer_R04->GetYaxis()->SetTitle("Fraction of Jet Energy");
  h2d_FracEnergy_vs_CaloLayer_R04->GetXaxis()->SetBinLabel(1, "EMCal");
  h2d_FracEnergy_vs_CaloLayer_R04->GetXaxis()->SetBinLabel(2, "HCalIn");
  h2d_FracEnergy_vs_CaloLayer_R04->GetXaxis()->SetBinLabel(3, "HCalOut");

  h1d_nConstituent_R02 = new TH1D("h1d_nConstituent_R02", "h1d_nConstituent_R02", 200, 0, 200);
  h1d_nConstituent_R02->GetXaxis()->SetTitle("Number of Constituents");
  h1d_nConstituent_R02->GetYaxis()->SetTitle("Probability Density");
  h1d_nConstituent_R02->SetLineColor(kRed-1);
  h1d_nConstituent_R02->SetLineWidth(2);
  h1d_nConstituent_R02->SetFillColor(kRed-1);
  h1d_nConstituent_R02->SetFillStyle(3001);
  h1d_nConstituent_R02->SetMarkerColor(kRed-1);
  h1d_nConstituent_R02->SetMarkerStyle(kFullCircle);
  h1d_nConstituent_R02->SetMarkerSize(1.5);

  h1d_nConstituent_R04 = new TH1D("h1d_nConstituent_R04", "h1d_nConstituent_R04", 200, 0, 200);
  h1d_nConstituent_R04->GetXaxis()->SetTitle("Number of Constituents");
  h1d_nConstituent_R04->GetYaxis()->SetTitle("Probability Density");
  h1d_nConstituent_R04->SetLineColor(kAzure+2);
  h1d_nConstituent_R04->SetLineWidth(2);
  h1d_nConstituent_R04->SetFillColor(kAzure+2);
  h1d_nConstituent_R04->SetFillStyle(3001);
  h1d_nConstituent_R04->SetMarkerColor(kAzure+2);
  h1d_nConstituent_R04->SetMarkerStyle(kFullSquare);
  h1d_nConstituent_R04->SetMarkerSize(1.5);

  h1d_nConstituent_IHCal_R02 = new TH1D("h1d_nConstituent_IHCal_R02", "h1d_nConstituent_IHCal_R02", 200,0,200);
  h1d_nConstituent_IHCal_R02->GetXaxis()->SetTitle("Fraction of Jet Energy");
  h1d_nConstituent_IHCal_R02->GetYaxis()->SetTitle("Probability Density");
  h1d_nConstituent_IHCal_R02->SetLineColor(kRed-1);
  h1d_nConstituent_IHCal_R02->SetLineWidth(2);
  h1d_nConstituent_IHCal_R02->SetFillColor(kRed-1);
  h1d_nConstituent_IHCal_R02->SetFillStyle(3001);
  h1d_nConstituent_IHCal_R02->SetMarkerColor(kRed-1);
  h1d_nConstituent_IHCal_R02->SetMarkerStyle(kFullCircle);
  h1d_nConstituent_IHCal_R02->SetMarkerSize(1.5);

  h1d_nConstituent_IHCal_R04 = new TH1D("h1d_nConstituent_IHCal_R04", "h1d_nConstituent_IHCal_R04", 200,0,200);
  h1d_nConstituent_IHCal_R04->GetXaxis()->SetTitle("Fraction of Jet Energy");
  h1d_nConstituent_IHCal_R04->GetYaxis()->SetTitle("Probability Density");
  h1d_nConstituent_IHCal_R04->SetLineColor(kAzure+2);
  h1d_nConstituent_IHCal_R04->SetLineWidth(2);
  h1d_nConstituent_IHCal_R04->SetFillColor(kAzure+2);
  h1d_nConstituent_IHCal_R04->SetFillStyle(3001);
  h1d_nConstituent_IHCal_R04->SetMarkerColor(kAzure+2);
  h1d_nConstituent_IHCal_R04->SetMarkerStyle(kFullSquare);
  h1d_nConstituent_IHCal_R04->SetMarkerSize(1.5);

  h1d_nConstituent_OHCal_R02 = new TH1D("h1d_nConstituent_OHCal_R02", "h1d_nConstituent_OHCal_R02", 200,0,200);
  h1d_nConstituent_OHCal_R02->GetXaxis()->SetTitle("Fraction of Jet Energy");
  h1d_nConstituent_OHCal_R02->GetYaxis()->SetTitle("Probability Density");
  h1d_nConstituent_OHCal_R02->SetLineColor(kRed-1);
  h1d_nConstituent_OHCal_R02->SetLineWidth(2);
  h1d_nConstituent_OHCal_R02->SetFillColor(kRed-1);
  h1d_nConstituent_OHCal_R02->SetFillStyle(3001);
  h1d_nConstituent_OHCal_R02->SetMarkerColor(kRed-1);
  h1d_nConstituent_OHCal_R02->SetMarkerStyle(kFullCircle);
  h1d_nConstituent_OHCal_R02->SetMarkerSize(1.5);

  h1d_nConstituent_OHCal_R04 = new TH1D("h1d_nConstituent_OHCal_R04", "h1d_nConstituent_OHCal_R04", 200,0,200);
  h1d_nConstituent_OHCal_R04->GetXaxis()->SetTitle("Fraction of Jet Energy");
  h1d_nConstituent_OHCal_R04->GetYaxis()->SetTitle("Probability Density");
  h1d_nConstituent_OHCal_R04->SetLineColor(kAzure+2);
  h1d_nConstituent_OHCal_R04->SetLineWidth(2);
  h1d_nConstituent_OHCal_R04->SetFillColor(kAzure+2);
  h1d_nConstituent_OHCal_R04->SetFillStyle(3001);
  h1d_nConstituent_OHCal_R04->SetMarkerColor(kAzure+2);
  h1d_nConstituent_OHCal_R04->SetMarkerStyle(kFullSquare);
  h1d_nConstituent_OHCal_R04->SetMarkerSize(1.5);
  
  h1d_nConstituent_EMCal_R02 = new TH1D("h1d_nConstituent_EMCal_R02", "h1d_nConstituent_EMCal_R02", 200,0,200);
  h1d_nConstituent_EMCal_R02->GetXaxis()->SetTitle("Fraction of Jet Energy");
  h1d_nConstituent_EMCal_R02->GetYaxis()->SetTitle("Probability Density");
  h1d_nConstituent_EMCal_R02->SetLineColor(kRed-1);
  h1d_nConstituent_EMCal_R02->SetLineWidth(2);
  h1d_nConstituent_EMCal_R02->SetFillColor(kRed-1);
  h1d_nConstituent_EMCal_R02->SetFillStyle(3001);
  h1d_nConstituent_EMCal_R02->SetMarkerColor(kRed-1);
  h1d_nConstituent_EMCal_R02->SetMarkerStyle(kFullCircle);
  h1d_nConstituent_EMCal_R02->SetMarkerSize(1.5);
  
  h1d_nConstituent_EMCal_R04 = new TH1D("h1d_nConstituent_EMCal_R04", "h1d_nConstituent_EMCal_R04", 200,0,200);
  h1d_nConstituent_EMCal_R04->GetXaxis()->SetTitle("Fraction of Jet Energy");
  h1d_nConstituent_EMCal_R04->GetYaxis()->SetTitle("Probability Density");
  h1d_nConstituent_EMCal_R04->SetLineColor(kAzure+2);
  h1d_nConstituent_EMCal_R04->SetLineWidth(2);
  h1d_nConstituent_EMCal_R04->SetFillColor(kAzure+2);
  h1d_nConstituent_EMCal_R04->SetFillStyle(3001);
  h1d_nConstituent_EMCal_R04->SetMarkerColor(kAzure+2);
  h1d_nConstituent_EMCal_R04->SetMarkerStyle(kFullSquare);
  h1d_nConstituent_EMCal_R04->SetMarkerSize(1.5);

  h1d_FracEnergy_IHCal_R02 = new TH1D("h1d_FracEnergy_IHCal_R02", "h1d_FracEnergy_IHCal_R02", 100, 0, 1);
  h1d_FracEnergy_IHCal_R02->GetXaxis()->SetTitle("Fraction of Jet Energy");
  h1d_FracEnergy_IHCal_R02->GetYaxis()->SetTitle("Probability Density");
  h1d_FracEnergy_IHCal_R02->SetLineColor(kRed-1);
  h1d_FracEnergy_IHCal_R02->SetLineWidth(2);
  h1d_FracEnergy_IHCal_R02->SetFillColor(kRed-1);
  h1d_FracEnergy_IHCal_R02->SetFillStyle(3001);
  h1d_FracEnergy_IHCal_R02->SetMarkerColor(kRed-1);
  h1d_FracEnergy_IHCal_R02->SetMarkerStyle(kFullCircle);
  h1d_FracEnergy_IHCal_R02->SetMarkerSize(1.5);

  h1d_FracEnergy_IHCal_R04 = new TH1D("h1d_FracEnergy_IHCal_R04", "h1d_FracEnergy_IHCal_R04", 100, 0, 1);
  h1d_FracEnergy_IHCal_R04->GetXaxis()->SetTitle("Fraction of Jet Energy");
  h1d_FracEnergy_IHCal_R04->GetYaxis()->SetTitle("Probability Density");
  h1d_FracEnergy_IHCal_R04->SetLineColor(kAzure+2);
  h1d_FracEnergy_IHCal_R04->SetLineWidth(2);
  h1d_FracEnergy_IHCal_R04->SetFillColor(kAzure+2);
  h1d_FracEnergy_IHCal_R04->SetFillStyle(3001);
  h1d_FracEnergy_IHCal_R04->SetMarkerColor(kAzure+2);
  h1d_FracEnergy_IHCal_R04->SetMarkerStyle(kFullSquare);
  h1d_FracEnergy_IHCal_R04->SetMarkerSize(1.5);

  h1d_FracEnergy_OHCal_R02 = new TH1D("h1d_FracEnergy_OHCal_R02", "h1d_FracEnergy_OHCal_R02", 100, 0, 1);
  h1d_FracEnergy_OHCal_R02->GetXaxis()->SetTitle("Fraction of Jet Energy");
  h1d_FracEnergy_OHCal_R02->GetYaxis()->SetTitle("Probability Density");
  h1d_FracEnergy_OHCal_R02->SetLineColor(kRed-1);
  h1d_FracEnergy_OHCal_R02->SetLineWidth(2);
  h1d_FracEnergy_OHCal_R02->SetFillColor(kRed-1);
  h1d_FracEnergy_OHCal_R02->SetFillStyle(3001);
  h1d_FracEnergy_OHCal_R02->SetMarkerColor(kRed-1);
  h1d_FracEnergy_OHCal_R02->SetMarkerStyle(kFullCircle);
  h1d_FracEnergy_OHCal_R02->SetMarkerSize(1.5);

  h1d_FracEnergy_OHCal_R04 = new TH1D("h1d_FracEnergy_OHCal_R04", "h1d_FracEnergy_OHCal_R04", 100, 0, 1);
  h1d_FracEnergy_OHCal_R04->GetXaxis()->SetTitle("Fraction of Jet Energy");
  h1d_FracEnergy_OHCal_R04->GetYaxis()->SetTitle("Probability Density");
  h1d_FracEnergy_OHCal_R04->SetLineColor(kAzure+2);
  h1d_FracEnergy_OHCal_R04->SetLineWidth(2);
  h1d_FracEnergy_OHCal_R04->SetFillColor(kAzure+2);
  h1d_FracEnergy_OHCal_R04->SetFillStyle(3001);
  h1d_FracEnergy_OHCal_R04->SetMarkerColor(kAzure+2);
  h1d_FracEnergy_OHCal_R04->SetMarkerStyle(kFullSquare);
  h1d_FracEnergy_OHCal_R04->SetMarkerSize(1.5);
  
  h1d_FracEnergy_EMCal_R02 = new TH1D("h1d_FracEnergy_EMCal_R02", "h1d_FracEnergy_EMCal_R02", 100, 0, 1);
  h1d_FracEnergy_EMCal_R02->GetXaxis()->SetTitle("Fraction of Jet Energy");
  h1d_FracEnergy_EMCal_R02->GetYaxis()->SetTitle("Probability Density");
  h1d_FracEnergy_EMCal_R02->SetLineColor(kRed-1);
  h1d_FracEnergy_EMCal_R02->SetLineWidth(2);
  h1d_FracEnergy_EMCal_R02->SetFillColor(kRed-1);
  h1d_FracEnergy_EMCal_R02->SetFillStyle(3001);
  h1d_FracEnergy_EMCal_R02->SetMarkerColor(kRed-1);
  h1d_FracEnergy_EMCal_R02->SetMarkerStyle(kFullCircle);
  h1d_FracEnergy_EMCal_R02->SetMarkerSize(1.5);
  
  h1d_FracEnergy_EMCal_R04 = new TH1D("h1d_FracEnergy_EMCal_R04", "h1d_FracEnergy_EMCal_R04", 100, 0, 1);
  h1d_FracEnergy_EMCal_R04->GetXaxis()->SetTitle("Fraction of Jet Energy");
  h1d_FracEnergy_EMCal_R04->GetYaxis()->SetTitle("Probability Density");
  h1d_FracEnergy_EMCal_R04->SetLineColor(kAzure+2);
  h1d_FracEnergy_EMCal_R04->SetLineWidth(2);
  h1d_FracEnergy_EMCal_R04->SetFillColor(kAzure+2);
  h1d_FracEnergy_EMCal_R04->SetFillStyle(3001);
  h1d_FracEnergy_EMCal_R04->SetMarkerColor(kAzure+2);
  h1d_FracEnergy_EMCal_R04->SetMarkerStyle(kFullSquare);
  h1d_FracEnergy_EMCal_R04->SetMarkerSize(1.5);
  
  h1d_FracEnergy_IHCal_R02->Sumw2();
  h1d_FracEnergy_OHCal_R02->Sumw2();
  h1d_FracEnergy_EMCal_R02->Sumw2();
  h1d_FracEnergy_IHCal_R04->Sumw2();
  h1d_FracEnergy_OHCal_R04->Sumw2();
  h1d_FracEnergy_EMCal_R04->Sumw2();

  c_emcal = new TCanvas("c_emcal", "c_emcal", 800, 600);
  c_ihcal = new TCanvas("c_ihcal", "c_ihcal", 800, 600);
  c_ohcal = new TCanvas("c_ohcal", "c_ohcal", 800, 600);
  c_nconst = new TCanvas("c_nconst", "c_nconst", 800, 600);

  c_nconst_emcal = new TCanvas("c_nconst_emcal", "c_nconst_emcal", 800, 600);
  c_nconst_ihcal = new TCanvas("c_nconst_ihcal", "c_nconst_ihcal", 800, 600);
  c_nconst_ohcal = new TCanvas("c_nconst_ohcal", "c_nconst_ohcal", 800, 600);

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
  std::vector<std::string> m_recojetname_string = {m_recojetnameR02, m_recojetnameR04};
  std::vector<float> m_jet_radii = {0.2, 0.4};
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
   
    
      if(i_radii==0) h1d_nConstituent_R02->Fill(jet->size_comp());
      else if(i_radii==1) h1d_nConstituent_R04->Fill(jet->size_comp());
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
        h1d_FracEnergy_EMCal_R02->Fill(emcal_sum);
        h1d_FracEnergy_IHCal_R02->Fill(i_hcal_sum);
        h1d_FracEnergy_OHCal_R02->Fill(o_hcal_sum);
        h1d_nConstituent_IHCal_R02->Fill(1.0*n_constituents_ihcal);
        h1d_nConstituent_OHCal_R02->Fill(1.0*n_constituents_ohcal);
        h1d_nConstituent_EMCal_R02->Fill(1.0*n_constituents_emcal);
        h2d_SumEnergyDebug_vs_R->Fill(0., emcal_energy_sum+ihcal_energy_sum+ohcal_energy_sum);
      }
      else if(i_radii==1) {
        h2d_FracEnergy_vs_CaloLayer_R04->Fill(2., o_hcal_sum);
        h2d_FracEnergy_vs_CaloLayer_R04->Fill(0., emcal_sum);
        h2d_FracEnergy_vs_CaloLayer_R04->Fill(1., i_hcal_sum);
        h1d_FracEnergy_EMCal_R04->Fill(emcal_sum);
        h1d_FracEnergy_IHCal_R04->Fill(i_hcal_sum);
        h1d_FracEnergy_OHCal_R04->Fill(o_hcal_sum);
        h1d_nConstituent_IHCal_R04->Fill(1.0*n_constituents_ihcal);
        h1d_nConstituent_OHCal_R04->Fill(1.0*n_constituents_ohcal);
        h1d_nConstituent_EMCal_R04->Fill(1.0*n_constituents_emcal);
        h2d_SumEnergyDebug_vs_R->Fill(0., emcal_energy_sum+ihcal_energy_sum+ohcal_energy_sum);


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
  float ptmin = m_ptRange.first;
  float ptmax = m_ptRange.second;

  h2d_nConstituent_vs_R->Write();
  h1d_nConstituent_R02->Write();
  h1d_nConstituent_R04->Write();
  h2d_FracEnergy_vs_CaloLayer_R02->Write();
  h2d_FracEnergy_vs_CaloLayer_R04->Write();
  h1d_FracEnergy_EMCal_R02->Write();
  h1d_FracEnergy_IHCal_R02->Write();
  h1d_FracEnergy_OHCal_R02->Write();
  h1d_FracEnergy_EMCal_R04->Write();
  h1d_FracEnergy_IHCal_R04->Write();
  h1d_FracEnergy_OHCal_R04->Write();
  h1d_nConstituent_IHCal_R02->Write();
  h1d_nConstituent_OHCal_R02->Write();
  h1d_nConstituent_EMCal_R02->Write();
  h1d_nConstituent_IHCal_R04->Write();
  h1d_nConstituent_OHCal_R04->Write();
  h1d_nConstituent_EMCal_R04->Write();
  h2d_SumEnergyDebug_vs_R->Write();



  c_emcal->cd();
  h1d_FracEnergy_EMCal_R02->SetTitle(Form("EMCal Energy Fraction, (%.1f < p_{T} < %.1f GeV/c)",ptmin,ptmax));
  h1d_FracEnergy_EMCal_R02->SetStats(0);
  h1d_FracEnergy_EMCal_R02->GetXaxis()->CenterTitle();
  h1d_FracEnergy_EMCal_R02->GetYaxis()->CenterTitle();
  h1d_FracEnergy_EMCal_R02->Scale(1./h1d_FracEnergy_EMCal_R02->Integral());
  h1d_FracEnergy_EMCal_R02->GetYaxis()->SetRangeUser(0.,0.5);
  h1d_FracEnergy_EMCal_R02->Draw("hist");
  h1d_FracEnergy_EMCal_R04->Scale(1./h1d_FracEnergy_EMCal_R04->Integral());
  h1d_FracEnergy_EMCal_R04->SetStats(0);  
  h1d_FracEnergy_EMCal_R04->Draw("hist same");
  TLegend *leg = new TLegend(0.7,0.7,0.9,0.9);
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->AddEntry(h1d_FracEnergy_EMCal_R02,"R=0.2","lf");
  leg->AddEntry(h1d_FracEnergy_EMCal_R04,"R=0.4","lf");
  leg->Draw();
  c_emcal->Update();
  c_emcal->Write();

  c_ihcal->cd();
  h1d_FracEnergy_IHCal_R02->SetTitle(Form("IHCal Energy Fraction, (%.1f < p_{T} < %.1f GeV/c)",ptmin,ptmax));
  h1d_FracEnergy_IHCal_R02->SetStats(0);
  h1d_FracEnergy_IHCal_R02->GetXaxis()->CenterTitle();
  h1d_FracEnergy_IHCal_R02->GetYaxis()->CenterTitle();
  h1d_FracEnergy_IHCal_R02->Scale(1./h1d_FracEnergy_IHCal_R02->Integral());
  h1d_FracEnergy_IHCal_R02->GetYaxis()->SetRangeUser(0.,0.5);
  h1d_FracEnergy_IHCal_R02->Draw("hist");
  h1d_FracEnergy_IHCal_R04->Scale(1./h1d_FracEnergy_IHCal_R04->Integral());
  h1d_FracEnergy_IHCal_R04->SetStats(0);
  h1d_FracEnergy_IHCal_R04->Draw("hist same");
  TLegend *leg2 = new TLegend(0.7,0.7,0.9,0.9);
  leg2->SetFillStyle(0);
  leg2->SetBorderSize(0);
  leg2->AddEntry(h1d_FracEnergy_IHCal_R02,"R=0.2","lf");
  leg2->AddEntry(h1d_FracEnergy_IHCal_R04,"R=0.4","lf");
  leg2->Draw();
  c_ihcal->Update();
  c_ihcal->Write();

  c_ohcal->cd();
  h1d_FracEnergy_OHCal_R02->SetTitle(Form("OHCal Energy Fraction, (%.1f < p_{T} < %.1f GeV/c)",ptmin,ptmax));
  h1d_FracEnergy_OHCal_R02->SetStats(0);
  h1d_FracEnergy_OHCal_R02->GetXaxis()->CenterTitle();
  h1d_FracEnergy_OHCal_R02->GetYaxis()->CenterTitle();
  h1d_FracEnergy_OHCal_R02->Scale(1./h1d_FracEnergy_OHCal_R02->Integral());
  h1d_FracEnergy_OHCal_R02->GetYaxis()->SetRangeUser(0.,0.5);
  h1d_FracEnergy_OHCal_R02->Draw("hist");
  h1d_FracEnergy_OHCal_R04->Scale(1./h1d_FracEnergy_OHCal_R04->Integral());
  h1d_FracEnergy_OHCal_R04->SetStats(0);
  h1d_FracEnergy_OHCal_R04->Draw("hist same");
  TLegend *leg3 = new TLegend(0.7,0.7,0.9,0.9);
  leg3->SetFillStyle(0);
  leg3->SetBorderSize(0);
  leg3->AddEntry(h1d_FracEnergy_OHCal_R02,"R=0.2","lf");
  leg3->AddEntry(h1d_FracEnergy_OHCal_R04,"R=0.4","lf");
  leg3->Draw();
  c_ohcal->Update();
  c_ohcal->Write();

  c_nconst->cd();
  h1d_nConstituent_R02->SetTitle(Form("Number of Constituents (%.1f < p_{T} < %.1f GeV/c)",ptmin,ptmax));
  h1d_nConstituent_R02->SetStats(0);
  h1d_nConstituent_R02->GetXaxis()->CenterTitle();
  h1d_nConstituent_R02->GetYaxis()->CenterTitle();
  h1d_nConstituent_R02->Scale(1./h1d_nConstituent_R02->Integral());
  h1d_nConstituent_R02->GetYaxis()->SetRangeUser(0.,0.5);
  h1d_nConstituent_R02->Draw("hist");
  h1d_nConstituent_R04->Scale(1./h1d_nConstituent_R04->Integral());
  h1d_nConstituent_R04->SetStats(0);
  h1d_nConstituent_R04->Draw("hist same");
  TLegend *leg4 = new TLegend(0.7,0.7,0.9,0.9);
  leg4->SetFillStyle(0);
  leg4->SetBorderSize(0);
  leg4->AddEntry(h1d_nConstituent_R02,"R=0.2","lf");
  leg4->AddEntry(h1d_nConstituent_R04,"R=0.4","lf");
  leg4->Draw();
  c_nconst->Update();
  c_nconst->Write();

  c_nconst_emcal->cd();
  h1d_nConstituent_EMCal_R02->SetTitle(Form("Number of Constituents in EMCal (%.1f < p_{T} < %.1f GeV/c)",ptmin,ptmax));
  h1d_nConstituent_EMCal_R02->SetStats(0);
  h1d_nConstituent_EMCal_R02->GetXaxis()->CenterTitle();
  h1d_nConstituent_EMCal_R02->GetYaxis()->CenterTitle();
  h1d_nConstituent_EMCal_R02->Scale(1./h1d_nConstituent_EMCal_R02->Integral());
  h1d_nConstituent_EMCal_R02->GetYaxis()->SetRangeUser(0.,0.5);
  h1d_nConstituent_EMCal_R02->Draw("hist");
  h1d_nConstituent_EMCal_R04->Scale(1./h1d_nConstituent_EMCal_R04->Integral());
  h1d_nConstituent_EMCal_R04->SetStats(0);
  h1d_nConstituent_EMCal_R04->Draw("hist same");
  TLegend *leg5 = new TLegend(0.7,0.7,0.9,0.9);
  leg5->SetFillStyle(0);
  leg5->SetBorderSize(0);
  leg5->AddEntry(h1d_nConstituent_EMCal_R02,"R=0.2","lf");
  leg5->AddEntry(h1d_nConstituent_EMCal_R04,"R=0.4","lf");
  leg5->Draw();
  c_nconst_emcal->Update();
  c_nconst_emcal->Write();

  c_nconst_emcal->cd();
  h1d_nConstituent_EMCal_R02->SetTitle(Form("Number of Constituents in EMCal (%.1f < p_{T} < %.1f GeV/c)",ptmin,ptmax));
  h1d_nConstituent_EMCal_R02->SetStats(0);
  h1d_nConstituent_EMCal_R02->GetXaxis()->CenterTitle();
  h1d_nConstituent_EMCal_R02->GetYaxis()->CenterTitle();
  h1d_nConstituent_EMCal_R02->Scale(1./h1d_nConstituent_EMCal_R02->Integral());
  h1d_nConstituent_EMCal_R02->GetYaxis()->SetRangeUser(0.,0.5);
  h1d_nConstituent_EMCal_R02->Draw("hist");
  h1d_nConstituent_EMCal_R04->Scale(1./h1d_nConstituent_EMCal_R04->Integral());
  h1d_nConstituent_EMCal_R04->SetStats(0);
  h1d_nConstituent_EMCal_R04->Draw("hist same");
  TLegend *leg51 = new TLegend(0.7,0.7,0.9,0.9);
  leg51->SetFillStyle(0);
  leg51->SetBorderSize(0);
  leg51->AddEntry(h1d_nConstituent_EMCal_R02,"R=0.2","lf");
  leg51->AddEntry(h1d_nConstituent_EMCal_R04,"R=0.4","lf");
  leg51->Draw();
  c_nconst_emcal->Update();
  c_nconst_emcal->Write();

  c_nconst_ihcal->cd();
  h1d_nConstituent_IHCal_R02->SetTitle(Form("Number of Constituents in IHCal (%.1f < p_{T} < %.1f GeV/c)",ptmin,ptmax));
  h1d_nConstituent_IHCal_R02->SetStats(0);
  h1d_nConstituent_IHCal_R02->GetXaxis()->CenterTitle();
  h1d_nConstituent_IHCal_R02->GetYaxis()->CenterTitle();
  h1d_nConstituent_IHCal_R02->Scale(1./h1d_nConstituent_IHCal_R02->Integral());
  h1d_nConstituent_IHCal_R02->GetYaxis()->SetRangeUser(0.,0.5);
  h1d_nConstituent_IHCal_R02->Draw("hist");
  h1d_nConstituent_IHCal_R04->Scale(1./h1d_nConstituent_IHCal_R04->Integral());
  h1d_nConstituent_IHCal_R04->SetStats(0);
  h1d_nConstituent_IHCal_R04->Draw("hist same");
  TLegend *leg16 = new TLegend(0.7,0.7,0.9,0.9);
  leg16->SetFillStyle(0);
  leg16->SetBorderSize(0);
  leg16->AddEntry(h1d_nConstituent_IHCal_R02,"R=0.2","lf");
  leg16->AddEntry(h1d_nConstituent_IHCal_R04,"R=0.4","lf");
  leg16->Draw();
  c_nconst_ihcal->Update();
  c_nconst_ihcal->Write();

  c_nconst_ohcal->cd();
  h1d_nConstituent_OHCal_R02->SetTitle(Form("Number of Constituents in OHCal (%.1f < p_{T} < %.1f GeV/c)",ptmin,ptmax));
  h1d_nConstituent_OHCal_R02->SetStats(0);
  h1d_nConstituent_OHCal_R02->GetXaxis()->CenterTitle();
  h1d_nConstituent_OHCal_R02->GetYaxis()->CenterTitle();
  h1d_nConstituent_OHCal_R02->Scale(1./h1d_nConstituent_OHCal_R02->Integral());
  h1d_nConstituent_OHCal_R02->GetYaxis()->SetRangeUser(0.,0.5);
  h1d_nConstituent_OHCal_R02->Draw("hist");
  h1d_nConstituent_OHCal_R04->Scale(1./h1d_nConstituent_OHCal_R04->Integral());
  h1d_nConstituent_OHCal_R04->SetStats(0);
  h1d_nConstituent_OHCal_R04->Draw("hist same");
  TLegend *leg17 = new TLegend(0.7,0.7,0.9,0.9);
  leg17->SetFillStyle(0);
  leg17->SetBorderSize(0);
  leg17->AddEntry(h1d_nConstituent_OHCal_R02,"R=0.2","lf");
  leg17->AddEntry(h1d_nConstituent_OHCal_R04,"R=0.4","lf");
  leg17->Draw();
  c_nconst_ohcal->Update();
  c_nconst_ohcal->Write();
  
  


    
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
