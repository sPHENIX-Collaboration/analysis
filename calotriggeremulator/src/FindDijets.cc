#include "FindDijets.h"
//for emc clusters
#include <TH1.h>
#include <TH2.h>
#include <TH2D.h>
#include <TFile.h>
#include <TF1.h>
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
#include "DijetEventDisplay.h"
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
  // if (h_jes)
  //   {
  //     delete h_jes;
  //   }
  // if (h_jer)
  //   {
  //     delete h_jer;
  //   }
}

//____________________________________________________________________________..
int FindDijets::Init(PHCompositeNode *topNode)
{

  // TFile *fnew = new TFile("/sphenix/user/dlis/Projects/CaloTriggerEmulator/analysis/calotriggeremulator/src/jesjer.root","r");
  // h_jes = (TH1D*) fnew->Get("h_jes");
  // h_jer = (TH1D*) fnew->Get("h_jer");

  // fsmear = new TF1("smearfunction","gaus", 0, 2);

  std::string outfile_display = _foutname;
  auto it =  outfile_display.find("DIJET");
  outfile_display.replace(it, 5, "DIJETEVT");
  if (!isSim)
    {
      if (drawDijets) dijeteventdisplay = new DijetEventDisplay(outfile_display.c_str());
      triggeranalyzer = new TriggerAnalyzer();
    }
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
  else
    {
      h2 = new TH2D("h_truth_pt1_pt2",";p_{T1};p_{T2}", 50, 0, 50, 50, 0, 50);
      hm->registerHisto(h2);
      h2 = new TH2D("h_truth_smear_pt1_pt2",";p_{T1};p_{T2}", 50, 0, 50, 50, 0, 50);
      hm->registerHisto(h2);

    }
  for (int i = 0; i < 3; i++)
    {

      h = new TH1D(Form("h_dphi_et_%d", i), ";#Delta #phi;N", 64, 0, TMath::Pi());
      hm->registerHisto(h);
      h = new TH1D(Form("h_dphi_%d", i), ";#Delta #phi;N", 64, 0, TMath::Pi());
      hm->registerHisto(h);
      if (!isSim)
	{
	  h = new TH1D(Form("h_dphi_mb_%d", i), ";#Delta #phi;N", 64, 0, TMath::Pi());
	  hm->registerHisto(h);
	  h = new TH1D(Form("h_dphi_et_mb_%d", i), ";#Delta #phi;N", 64, 0, TMath::Pi());
	  hm->registerHisto(h);

	}
      else
	{
	  h = new TH1D(Form("h_truth_dphi_%d", i), ";#Delta #phi;N", 64, 0, TMath::Pi());
	  hm->registerHisto(h);
	  h = new TH1D(Form("h_truth_smear_dphi_%d", i), ";#Delta #phi;N", 64, 0, TMath::Pi());
	  hm->registerHisto(h);

	}
      for (int ip = 0; ip < 3; ip++)
	{

	  h = new TH1D(Form("h_Aj_et_%d_dp%d", i, ip), ";A_j;N", 20, 0, 1);
	  hm->registerHisto(h);
	  h = new TH1D(Form("h_xj_et_%d_dp%d", i, ip), ";x_j;N", 20, 0, 1);
	  hm->registerHisto(h);
	  
	  h = new TH1D(Form("h_Aj_%d_dp%d", i, ip), ";A_j;N", 20, 0, 1);
	  hm->registerHisto(h);
	  h = new TH1D(Form("h_xj_%d_dp%d", i, ip), ";x_j;N", 20, 0, 1);
	  hm->registerHisto(h);
	  for (int j = 0; j < 2; j++)
	    {
	      h = new TH1D(Form("h_Aj_et_iso%d_%d_dp%d", j, i, ip), ";A_j;N", 20, 0, 1);
	      hm->registerHisto(h);
	      h = new TH1D(Form("h_xj_et_iso%d_%d_dp%d", j, i, ip), ";x_j;N", 20, 0, 1);
	      hm->registerHisto(h);
	      
	      h = new TH1D(Form("h_Aj_iso%d_%d_dp%d", j, i, ip), ";A_j;N", 20, 0, 1);
	      hm->registerHisto(h);
	      h = new TH1D(Form("h_xj_iso%d_%d_dp%d", j, i, ip), ";x_j;N", 20, 0, 1);
	      hm->registerHisto(h);
	    }

	  if (!isSim)
	    {
	      h = new TH1D(Form("h_Aj_et_mb_%d_dp%d", i, ip), ";A_j;N", 20, 0, 1);
	      hm->registerHisto(h);
	      h = new TH1D(Form("h_xj_et_mb_%d_dp%d", i, ip), ";x_j;N", 20, 0, 1);
	      hm->registerHisto(h);
	      h = new TH1D(Form("h_Aj_mb_%d_dp%d", i, ip), ";A_j;N", 20, 0, 1);
	      hm->registerHisto(h);
	      h = new TH1D(Form("h_xj_mb_%d_dp%d", i, ip), ";x_j;N", 20, 0, 1);
	      hm->registerHisto(h);


	      for (int j = 0; j < 8; j++)
		{
		  h = new TH1D(Form("h_Aj_et_gl1_%d_%d_dp%d", j, i, ip), ";A_j;N", 20, 0, 1);
		  hm->registerHisto(h);
		  h = new TH1D(Form("h_xj_et_gl1_%d_%d_dp%d", j, i, ip), ";x_j;N", 20, 0, 1);
		  hm->registerHisto(h);
		  h = new TH1D(Form("h_Aj_gl1_%d_%d_dp%d", j, i, ip), ";A_j;N", 20, 0, 1);
		  hm->registerHisto(h);
		  h = new TH1D(Form("h_xj_gl1_%d_%d_dp%d", j, i, ip), ";x_j;N", 20, 0, 1);
		  hm->registerHisto(h);
		  if (ip == 0)
		    {
		      h = new TH1D(Form("h_dphi_gl1_%d_%d", j, i), ";#Delta #phi;N", 64, 0, TMath::Pi());
		      hm->registerHisto(h);
		      h = new TH1D(Form("h_dphi_et_gl1_%d_%d", j, i), ";#Delta #phi;N", 64, 0, TMath::Pi());
		      hm->registerHisto(h);
		    }
		}
	    }
	  else
	    {
	      
	      h = new TH1D(Form("h_truth_Aj_%d_dp%d", i, ip), ";A_j;N", 20, 0, 1);
	      hm->registerHisto(h);
	      h = new TH1D(Form("h_truth_xj_%d_dp%d", i, ip), ";x_j;N", 20, 0, 1);
	      hm->registerHisto(h);
	      h = new TH1D(Form("h_truth_smear_Aj_%d_dp%d", i, ip), ";A_j;N", 20, 0, 1);
	      hm->registerHisto(h);
	      h = new TH1D(Form("h_truth_smear_xj_%d_dp%d", i, ip), ";x_j;N", 20, 0, 1);
	      hm->registerHisto(h);

	    }
	}
    }
  // trash events here
  if (!isSim)
    {
      h2 = new TH2D("h_trash_pt1_pt2",";p_{T1};p_{T2}", 50, 0, 50, 50, 0, 50);
      hm->registerHisto(h2);
      h2 = new TH2D("h_trash_et1_et2",";e_{T1};e_{T2}", 50, 0, 50, 50, 0, 50);
      hm->registerHisto(h2);

      h2 = new TH2D("h_trash_pt1_pt2_mb",";p_{T1};p_{T2}", 50, 0, 50, 50, 0, 50);
      hm->registerHisto(h2);
      h2 = new TH2D("h_trash_et1_et2_mb",";e_{T1};e_{T2}", 50, 0, 50, 50, 0, 50);
      hm->registerHisto(h2);

      for (int j = 0; j < 8; j++)
	{
	  h2 = new TH2D(Form("h_trash_pt1_pt2_gl1_%d", j),";p_{T1};p_{T2}", 50, 0, 50, 50, 0, 50);
	  hm->registerHisto(h2);
	  h2 = new TH2D(Form("h_trash_et1_et2_gl1_%d",j),";e_{T1};e_{T2}", 50, 0, 50, 50, 0, 50);
	  hm->registerHisto(h2);

	}
      for (int i = 0; i < 3; i++)
	{

	  h = new TH1D(Form("h_trash_dphi_et_%d", i), ";#Delta #phi;N", 64, 0, TMath::Pi());
	  hm->registerHisto(h);
	  h = new TH1D(Form("h_trash_dphi_%d", i), ";#Delta #phi;N", 64, 0, TMath::Pi());
	  hm->registerHisto(h);

	  h = new TH1D(Form("h_trash_dphi_mb_%d", i), ";#Delta #phi;N", 64, 0, TMath::Pi());
	  hm->registerHisto(h);
	  h = new TH1D(Form("h_trash_dphi_et_mb_%d", i), ";#Delta #phi;N", 64, 0, TMath::Pi());
	  hm->registerHisto(h);
	  
	  for (int ip = 0; ip < 3; ip++)
	    {

	      h = new TH1D(Form("h_trash_Aj_et_%d_dp%d", i, ip), ";A_j;N", 20, 0, 1);
	      hm->registerHisto(h);
	      h = new TH1D(Form("h_trash_xj_et_%d_dp%d", i, ip), ";x_j;N", 20, 0, 1);
	      hm->registerHisto(h);
	      h = new TH1D(Form("h_trash_Aj_%d_dp%d", i, ip), ";A_j;N", 20, 0, 1);
	      hm->registerHisto(h);
	      h = new TH1D(Form("h_trash_xj_%d_dp%d", i, ip), ";x_j;N", 20, 0, 1);
	      hm->registerHisto(h);
      
	      h = new TH1D(Form("h_trash_Aj_et_mb_%d_dp%d", i, ip), ";A_j;N", 20, 0, 1);
	      hm->registerHisto(h);
	      h = new TH1D(Form("h_trash_xj_et_mb_%d_dp%d", i, ip), ";x_j;N", 20, 0, 1);
	      hm->registerHisto(h);
	      h = new TH1D(Form("h_trash_Aj_mb_%d_dp%d", i, ip), ";A_j;N", 20, 0, 1);
	      hm->registerHisto(h);
	      h = new TH1D(Form("h_trash_xj_mb_%d_dp%d", i, ip), ";x_j;N", 20, 0, 1);
	      hm->registerHisto(h);
	  
	      for (int j = 0; j < 8; j++)
		{

		  h = new TH1D(Form("h_trash_Aj_et_gl1_%d_%d_dp%d", j, i, ip), ";A_j;N", 20, 0, 1);
		  hm->registerHisto(h);
		  h = new TH1D(Form("h_trash_xj_et_gl1_%d_%d_dp%d", j, i, ip), ";x_j;N", 20, 0, 1);
		  hm->registerHisto(h);
		  h = new TH1D(Form("h_trash_Aj_gl1_%d_%d_dp%d", j, i, ip), ";A_j;N", 20, 0, 1);
		  hm->registerHisto(h);
		  h = new TH1D(Form("h_trash_xj_gl1_%d_%d_dp%d", j, i, ip), ";x_j;N", 20, 0, 1);
		  hm->registerHisto(h);
		  if (ip == 0)
		    {
		      h = new TH1D(Form("h_trash_dphi_gl1_%d_%d", j, i), ";#Delta #phi;N", 64, 0, TMath::Pi());
		      hm->registerHisto(h);
		      h = new TH1D(Form("h_trash_dphi_et_gl1_%d_%d", j, i), ";#Delta #phi;N", 64, 0, TMath::Pi());
		      hm->registerHisto(h);
		    }
		}
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
void FindDijets::process_truth_jets(JetContainer *jets)
{

  fsmear = new TF1("fg","gaus", 0, 2);
  float jes = 1;
  float jer = 0.18;
  fsmear->SetParameters(1, jes, jer);
  std::vector<jetty> truth_jets{};
  std::vector<jetty> truth_smear_jets{};
  if (Verbosity()) jets->identify(std::cout);
  for (auto jet : *jets)
    {
      if (Verbosity()) std::cout << __LINE__ << std::endl;
      struct jetty myjet;
      myjet.pt = jet->get_pt();
      myjet.eta = jet->get_eta();
      myjet.phi = jet->get_phi();
      truth_jets.push_back(myjet);

      float pt = jet->get_pt() * fsmear->GetRandom();

      struct jetty myjetsmear;
      myjetsmear.pt = pt;
      myjetsmear.eta = jet->get_eta();
      myjetsmear.phi = jet->get_phi();
      truth_smear_jets.push_back(myjetsmear);

    }	 
  if ((int) truth_jets.size() >= 2)
    {
      std::sort(truth_jets.begin(), truth_jets.end(), [] (auto a, auto b){ return a.pt > b.pt; });
      float eta1 = truth_jets.at(0).eta;
      float phi1 = truth_jets.at(0).phi;
      float eta2 = truth_jets.at(1).eta;
      float phi2 = truth_jets.at(1).phi;
      if (fabs(eta1) < 0.7 && fabs(eta2) < 0.7)
	{

	  float dphi = phi1 - phi2;

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
	    return;

	  if (Verbosity())
	    {
	      std::cout << "Truth Dijet" << std::endl;
	      std::cout << "Leading Jet" << std::endl;
	      int jl = 0;
	      std::cout << "    pt   = " << truth_jets.at(jl).pt << std::endl;
	      std::cout << "    eta  = " << truth_jets.at(jl).eta << std::endl;
	      std::cout << "    phi  = " << truth_jets.at(jl).phi << std::endl;
	      std::cout << "Subleading Jet" << std::endl;
	      jl++;
	      std::cout << "    pt   = " << truth_jets.at(jl).pt << std::endl;
	      std::cout << "    eta  = " << truth_jets.at(jl).eta << std::endl;
	      std::cout << "    phi  = " << truth_jets.at(jl).phi << std::endl;
	      std::cout << " " << std::endl;
	    }  


	  if (truth_jets.at(0).pt > pt_cut1[0] && truth_jets.at(1).pt > pt_cut2[0])
	    {
	      auto h_pt1_pt2 = dynamic_cast<TH2*>(hm->getHisto("h_truth_pt1_pt2"));
	      h_pt1_pt2->Fill(truth_jets.at(0).pt, truth_jets.at(1).pt);
	      h_pt1_pt2->Fill(truth_jets.at(1).pt, truth_jets.at(0).pt);
	    }

	  float Aj_pt = (truth_jets.at(0).pt - truth_jets.at(1).pt)/(truth_jets.at(0).pt + truth_jets.at(1).pt);
	  float xj_pt = (truth_jets.at(1).pt / truth_jets.at(0).pt);

	  if (Verbosity())
	    {
	      std::cout << "---- DIJET ---- " << std::endl;
	      std::cout << "Calculated: " << std::endl;
	      std::cout << "    A_j   = " << Aj_pt << std::endl; 
	      std::cout << "    x_j   = " << xj_pt << std::endl; 
	      std::cout << "    dPhi  = " << dphi << std::endl;
	      std::cout << " ------------------------ " << std::endl;
	    }

	  for (int i = 0; i < 3; i++)
	    {
	      if (truth_jets.at(0).pt > pt_cut1[i] && truth_jets.at(1).pt > pt_cut2[i])
		{
		  auto h_dphi = dynamic_cast<TH1*>(hm->getHisto(Form("h_truth_dphi_%d", i)));
		  h_dphi->Fill(dphi);

		  for (int ip = 0; ip < 3; ip++)
		    {

		      if (dphi <= dphi_cut[ip])
			break;

		      auto h_Aj = dynamic_cast<TH1*>(hm->getHisto(Form("h_truth_Aj_%d_dp%d", i, ip)));
		      h_Aj->Fill(Aj_pt);
		      auto h_xj = dynamic_cast<TH1*>(hm->getHisto(Form("h_truth_xj_%d_dp%d", i, ip)));
		      h_xj->Fill(xj_pt);
	      
		    }
		}
	    }
	}
    }
  if ((int) truth_smear_jets.size() >= 2)
    {
      std::sort(truth_smear_jets.begin(), truth_smear_jets.end(), [] (auto a, auto b){ return a.pt > b.pt; });
      float eta1 = truth_smear_jets.at(0).eta;
      float phi1 = truth_smear_jets.at(0).phi;
      float eta2 = truth_smear_jets.at(1).eta;
      float phi2 = truth_smear_jets.at(1).phi;
      if (fabs(eta1) < 0.7 && fabs(eta2) < 0.7)
	{

	  float dphi = phi1 - phi2;

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
	    return;

	  if (Verbosity())
	    {
	      std::cout << "Truth Dijet" << std::endl;
	      std::cout << "Leading Jet" << std::endl;
	      int jl = 0;
	      std::cout << "    pt   = " << truth_smear_jets.at(jl).pt << std::endl;
	      std::cout << "    eta  = " << truth_smear_jets.at(jl).eta << std::endl;
	      std::cout << "    phi  = " << truth_smear_jets.at(jl).phi << std::endl;
	      std::cout << "Subleading Jet" << std::endl;
	      jl++;
	      std::cout << "    pt   = " << truth_smear_jets.at(jl).pt << std::endl;
	      std::cout << "    eta  = " << truth_smear_jets.at(jl).eta << std::endl;
	      std::cout << "    phi  = " << truth_smear_jets.at(jl).phi << std::endl;
	      std::cout << " " << std::endl;
	    }  


	  if (truth_smear_jets.at(0).pt > pt_cut1[0] && truth_smear_jets.at(1).pt > pt_cut2[0])
	    {
	      auto h_pt1_pt2 = dynamic_cast<TH2*>(hm->getHisto("h_truth_smear_pt1_pt2"));
	      h_pt1_pt2->Fill(truth_smear_jets.at(0).pt, truth_smear_jets.at(1).pt);
	      h_pt1_pt2->Fill(truth_smear_jets.at(1).pt, truth_smear_jets.at(0).pt);
	    }

	  float Aj_pt = (truth_smear_jets.at(0).pt - truth_smear_jets.at(1).pt)/(truth_smear_jets.at(0).pt + truth_smear_jets.at(1).pt);
	  float xj_pt = (truth_smear_jets.at(1).pt / truth_smear_jets.at(0).pt);

	  if (Verbosity())
	    {
	      std::cout << "---- DIJET ---- " << std::endl;
	      std::cout << "Calculated: " << std::endl;
	      std::cout << "    A_j   = " << Aj_pt << std::endl; 
	      std::cout << "    x_j   = " << xj_pt << std::endl; 
	      std::cout << "    dPhi  = " << dphi << std::endl;
	      std::cout << " ------------------------ " << std::endl;
	    }

	  for (int i = 0; i < 3; i++)
	    {
	      if (truth_smear_jets.at(0).pt > pt_cut1[i] && truth_smear_jets.at(1).pt > pt_cut2[i])
		{
		  auto h_dphi = dynamic_cast<TH1*>(hm->getHisto(Form("h_truth_smear_dphi_%d", i)));
		  h_dphi->Fill(dphi);

		  for (int ip = 0; ip < 3; ip++)
		    {

		      if (dphi <= dphi_cut[ip])
			break;
		      auto h_Aj = dynamic_cast<TH1*>(hm->getHisto(Form("h_truth_smear_Aj_%d_dp%d", i, ip)));
		      h_Aj->Fill(Aj_pt);
		      auto h_xj = dynamic_cast<TH1*>(hm->getHisto(Form("h_truth_smear_xj_%d_dp%d", i, ip)));
		      h_xj->Fill(xj_pt);
	      
		    }
		}
	    }
	}
    }
  return;
}
int FindDijets::process_event(PHCompositeNode *topNode)
{


  _i_event++;

  if (!isSim)
    {
      triggeranalyzer->decodeTriggers(topNode);
      std::cout << triggeranalyzer->getTriggerName(17) << " : " << triggeranalyzer->getTriggerScalers(m_jet_triggernames[1]) << std::endl;
    }
  RawTowerGeomContainer *tower_geomIH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALIN");
  RawTowerGeomContainer *tower_geomOH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALOUT");


  std::string recoJetName = "AntiKt_Tower_r04_Sub1";

  JetContainer *jets_4 = findNode::getClass<JetContainer>(topNode, recoJetName);
  recoJetName = "AntiKt_Truth_r04";
  JetContainer *jets_truth_4 = findNode::getClass<JetContainer>(topNode, recoJetName);

  if (jets_truth_4)
    {
      if (Verbosity()) std::cout << __LINE__ << std::endl;
      process_truth_jets(jets_truth_4);
      if (Verbosity()) std::cout << __LINE__ << std::endl;
      
    }

  TrashInfo *trashinfo = findNode::getClass<TrashInfo>(topNode, "TrashInfo");
  if (!jets_4)
    {
      std::cout << " Nothing in the jets"<<std::endl;
      return Fun4AllReturnCodes::ABORTRUN;
    }
  int trash = 0;
  if (trashinfo)
    {
      trash = (trashinfo->isTrash()? 1: 0);
    }

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
      jets.push_back(myjet);
    }

  if (jets.size() < 2) 
    return Fun4AllReturnCodes::EVENT_OK;


  auto pt_ordered = jets;
  std::sort(jets.begin(), jets.end(), [](auto a, auto b) { return a.et > b.et; });
  std::sort(pt_ordered.begin(), pt_ordered.end(), [](auto a, auto b) { return a.pt > b.pt; });
  if (pt_ordered.at(0).pt < 10 && jets.at(0).et < 10)      return Fun4AllReturnCodes::EVENT_OK;

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

  
  // Get the iso
  bool isopt  = true;
  bool isoet  = true;
  if (jets.size() > 2)
    {
      for (int ijet = 0; ijet < (int) jets.size() - 2; ijet++)
	{
	  if (jets.at(ijet+2).pt < 2 ) continue;
	  
	  for (int ij = 0; ij < 2; ij++)
	    {
	      double dR = get_Dr(jets.at(ij), jets.at(ijet+2));
	      isoet &= (dR > isocut);
	    }
	}
    }

  if (pt_ordered.size() > 2)
    {
      for (int ijet = 0; ijet < (int) pt_ordered.size() - 2; ijet++)
	{
	  if (pt_ordered.at(ijet+2).pt < 2 ) continue;
	  
	  for (int ij = 0; ij < 2; ij++)
	    {
	      double dR = get_Dr(pt_ordered.at(ij), pt_ordered.at(ijet+2));
	      isopt &= (dR > isocut);
	    }
	}
    }
  if (dphi > TMath::Pi()) 
    return Fun4AllReturnCodes::EVENT_OK;

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
      // std::cout << "Calculated: " << std::endl;
      // std::cout << "    A_j   = " << Aj_pt << " ( " << Aj_et << " ) "<< std::endl; 
      // std::cout << "    x_j   = " << xj_pt << " ( " << xj_et << " ) "<< std::endl; 
      // std::cout << "    dPhi  = " << dphi << std::endl;
      // std::cout << " ------------------------ " << std::endl;
    }

  if (trash)
    {
      if (pt_ordered.at(0).pt > pt_cut1[0] && pt_ordered.at(1).pt > pt_cut2[0])
	{	  
	  auto h_trash_pt1_pt2 = dynamic_cast<TH2*>(hm->getHisto("h_trash_pt1_pt2"));
	  h_trash_pt1_pt2->Fill(pt_ordered.at(0).pt, pt_ordered.at(1).pt);
	  h_trash_pt1_pt2->Fill(pt_ordered.at(1).pt, pt_ordered.at(0).pt);
	}
      if (jets.at(0).et > pt_cut1[0] && jets.at(1).et > pt_cut2[0])
	{	  
	  auto h_trash_et1_et2 = dynamic_cast<TH2*>(hm->getHisto("h_trash_et1_et2"));
	  h_trash_et1_et2->Fill(jets.at(0).et, jets.at(1).et);
	  h_trash_et1_et2->Fill(jets.at(1).et, jets.at(0).et);
	}
      if (triggeranalyzer->didTriggerFire("MBD N&S >= 1"))
	{
	  if (pt_ordered.at(0).pt > pt_cut1[0] && pt_ordered.at(1).pt > pt_cut2[0])
	    {	  
	      auto h_trash_pt1_pt2_mbd = dynamic_cast<TH2*>(hm->getHisto("h_trash_pt1_pt2_mbd"));
	      h_trash_pt1_pt2_mbd->Fill(pt_ordered.at(0).pt, pt_ordered.at(1).pt);
	      h_trash_pt1_pt2_mbd->Fill(pt_ordered.at(1).pt, pt_ordered.at(0).pt);
	    }
	  if (jets.at(0).et > pt_cut1[0] && jets.at(1).et > pt_cut2[0])
	    {	  
	      auto h_trash_et1_et2_mbd = dynamic_cast<TH2*>(hm->getHisto("h_trash_et1_et2_mbd"));
	      h_trash_et1_et2_mbd->Fill(jets.at(0).et, jets.at(1).et);
	      h_trash_et1_et2_mbd->Fill(jets.at(1).et, jets.at(0).et);
	    }
	}
      for (int i = 0; i < 8; i++)
	{
	  if (triggeranalyzer->didTriggerFire(m_jet_triggernames[i]))
	    {
	      if (pt_ordered.at(0).pt > pt_cut1[0] && pt_ordered.at(1).pt > pt_cut2[0])
		{	  		      
		  auto h_trash_pt1_pt2_gl1 = dynamic_cast<TH2*>(hm->getHisto(Form("h_trash_pt1_pt2_gl1_%d", i)));
		  h_trash_pt1_pt2_gl1->Fill(pt_ordered.at(0).pt, pt_ordered.at(1).pt);
		  h_trash_pt1_pt2_gl1->Fill(pt_ordered.at(1).pt, pt_ordered.at(0).pt);
		}
	      if (jets.at(0).et > pt_cut1[0] && jets.at(1).et > pt_cut2[0])
		{	  
		  auto h_trash_et1_et2_gl1 = dynamic_cast<TH2*>(hm->getHisto(Form("h_trash_et1_et2_gl1_%d", i)));
		  h_trash_et1_et2_gl1->Fill(jets.at(0).et, jets.at(1).et);
		  h_trash_et1_et2_gl1->Fill(jets.at(1).et, jets.at(0).et);
		}
	    }
	}
	
      float Aj_pt = (pt_ordered.at(0).pt - pt_ordered.at(1).pt)/(pt_ordered.at(0).pt + pt_ordered.at(1).pt);
      float Aj_et = (jets.at(0).et - jets.at(1).et)/(jets.at(0).et + jets.at(1).et);
      float xj_pt = (pt_ordered.at(1).pt / pt_ordered.at(0).pt);
      float xj_et = (jets.at(1).et / jets.at(0).et);

      if (Verbosity())
	{
	  std::cout << "---- TRASH ---- " << std::endl;
	  std::cout << "Calculated: " << std::endl;
	  std::cout << "    A_j   = " << Aj_pt << " ( " << Aj_et << " ) "<< std::endl; 
	  std::cout << "    x_j   = " << xj_pt << " ( " << xj_et << " ) "<< std::endl; 
	  std::cout << "    dPhi  = " << dphi << std::endl;
	  std::cout << " ------------------------ " << std::endl;
	}
      bool trashalreadyfilled = false;
      for (int i = 0; i < 3; i++)
	{
	  if (pt_ordered.at(0).pt > pt_cut1[i] && pt_ordered.at(1).pt > pt_cut2[i])
	    {
	      auto h_trash_dphi = dynamic_cast<TH1*>(hm->getHisto(Form("h_trash_dphi_%d", i)));
	      h_trash_dphi->Fill(dphi);
	      if (!isSim)
		{
		  if (triggeranalyzer->didTriggerFire("MBD N&S >= 1"))
		    {
		      auto h_trash_dphi_mb = dynamic_cast<TH1*>(hm->getHisto(Form("h_trash_dphi_mb_%d", i)));
		      h_trash_dphi_mb->Fill(dphi);
		    }
		
		  for (int j = 0; j < 8; j++)
		    {
		      if (triggeranalyzer->didTriggerFire(m_jet_triggernames[j]))
			{
			  auto h_trash_dphi_gl1 = dynamic_cast<TH1*>(hm->getHisto(Form("h_trash_dphi_gl1_%d_%d", j, i)));
			  h_trash_dphi_gl1->Fill(dphi);
			}
		    }
		}
	      
	      if (i == 0 && drawDijets)
		{
		  trashalreadyfilled = true;
		  std::cout << " FOUND " << std::endl;
		  dijeteventdisplay->FillEvent(topNode, Aj_et, dphi);
		}
	      for (int ip = 0; ip < 3; ip++)
		{
		  if (dphi <= dphi_cut[ip])
		    break;
		  auto h_trash_Aj = dynamic_cast<TH1*>(hm->getHisto(Form("h_trash_Aj_%d_dp%d", i, ip)));
		  h_trash_Aj->Fill(Aj_pt);
		  auto h_trash_xj = dynamic_cast<TH1*>(hm->getHisto(Form("h_trash_xj_%d_dp%d", i, ip)));
		  h_trash_xj->Fill(xj_pt);
		  
		  if (!isSim)
		    {
		      if (triggeranalyzer->didTriggerFire("MBD N&S >= 1"))
			{
			  auto h_trash_Aj_mb = dynamic_cast<TH1*>(hm->getHisto(Form("h_trash_Aj_mb_%d_dp%d", i, ip)));
			  h_trash_Aj_mb->Fill(Aj_pt);
			  auto h_trash_xj_mb = dynamic_cast<TH1*>(hm->getHisto(Form("h_trash_xj_mb_%d_dp%d", i, ip)));
			  h_trash_xj_mb->Fill(xj_pt);
			}
		    }
		  for (int j = 0; j < 8; j++)
		    {
		      if (triggeranalyzer->didTriggerFire(m_jet_triggernames[j]))
			{
			  auto h_trash_Aj_gl1 = dynamic_cast<TH1*>(hm->getHisto(Form("h_trash_Aj_gl1_%d_%d_dp%d", j, i, ip)));
			  h_trash_Aj_gl1->Fill(Aj_pt);
			  auto h_trash_xj_gl1 = dynamic_cast<TH1*>(hm->getHisto(Form("h_trash_xj_gl1_%d_%d_dp%d", j, i, ip)));
			  h_trash_xj_gl1->Fill(xj_pt);
			}
		    }
	  
		}
	    }
	  if (jets.at(0).et > pt_cut1[i] && jets.at(1).et > pt_cut2[i])
	    {
	      auto h_trash_dphi = dynamic_cast<TH1*>(hm->getHisto(Form("h_trash_dphi_et_%d", i)));
	      h_trash_dphi->Fill(dphi);

		  if (!isSim)
		    {
		      if (triggeranalyzer->didTriggerFire("MBD N&S >= 1"))
			{
       	
			  auto h_trash_dphi_et_mb = dynamic_cast<TH1*>(hm->getHisto(Form("h_trash_dphi_et_mb_%d", i)));
			  h_trash_dphi_et_mb->Fill(dphi);
			}
		      for (int j = 0; j < 8; j++)
			{
			  if (triggeranalyzer->didTriggerFire(m_jet_triggernames[j]))
			    {
			      auto h_trash_dphi_et_gl1 = dynamic_cast<TH1*>(hm->getHisto(Form("h_trash_dphi_et_gl1_%d_%d", j, i)));
			      h_trash_dphi_et_gl1->Fill(dphi);
			    }
			}
		    }

	      if (i == 0 && !trashalreadyfilled && drawDijets)
		{
		  std::cout << " FOUND " << std::endl;
		  dijeteventdisplay->FillEvent(topNode, Aj_et, dphi);
		}
	      for (int ip = 0 ; ip < 3; ip++)
		{
		  if (dphi <= dphi_cut[ip]) break;
		  auto h_trash_Aj = dynamic_cast<TH1*>(hm->getHisto(Form("h_trash_Aj_et_%d_dp%d", i, ip)));
		  h_trash_Aj->Fill(Aj_et);
		  auto h_trash_xj = dynamic_cast<TH1*>(hm->getHisto(Form("h_trash_xj_et_%d_dp%d", i, ip)));
		  h_trash_xj->Fill(xj_et);

		  if (!isSim)
		    {
		      if (triggeranalyzer->didTriggerFire("MBD N&S >= 1"))
			{
			  auto h_trash_Aj_et_mb = dynamic_cast<TH1*>(hm->getHisto(Form("h_trash_Aj_et_mb_%d_dp%d", i, ip)));
			  h_trash_Aj_et_mb->Fill(Aj_et);
			  auto h_trash_xj_et_mb = dynamic_cast<TH1*>(hm->getHisto(Form("h_trash_xj_et_mb_%d_dp%d", i, ip)));
			  h_trash_xj_et_mb->Fill(xj_et);
			}
		      for (int j = 0; j < 8; j++)
			{
			  if (triggeranalyzer->didTriggerFire(m_jet_triggernames[j]))
			    {
			      auto h_trash_Aj_et_gl1 = dynamic_cast<TH1*>(hm->getHisto(Form("h_trash_Aj_et_gl1_%d_%d_dp%d", j, i, ip)));
			      h_trash_Aj_et_gl1->Fill(Aj_et);
			      auto h_trash_xj_et_gl1 = dynamic_cast<TH1*>(hm->getHisto(Form("h_trash_xj_et_gl1_%d_%d_dp%d", j, i, ip)));
			      h_trash_xj_et_gl1->Fill(xj_et);
			    }
			}
		    }
		}
	    }
	}
      return Fun4AllReturnCodes::EVENT_OK;
    }
  
  if (pt_ordered.at(0).pt > pt_cut1[0] && pt_ordered.at(1).pt > pt_cut2[0])
    {
      auto h_pt1_pt2 = dynamic_cast<TH2*>(hm->getHisto("h_pt1_pt2"));
      h_pt1_pt2->Fill(pt_ordered.at(0).pt, pt_ordered.at(1).pt);
      h_pt1_pt2->Fill(pt_ordered.at(1).pt, pt_ordered.at(0).pt);
    }
  if (jets.at(0).et > pt_cut1[0] && jets.at(1).et > pt_cut2[0])
    {
      auto h_et1_et2 = dynamic_cast<TH2*>(hm->getHisto("h_et1_et2"));
      h_et1_et2->Fill(jets.at(0).et, jets.at(1).et);
      h_et1_et2->Fill(jets.at(1).et, jets.at(0).et);
    }
  if (!isSim)
    {
      if (triggeranalyzer->didTriggerFire("MBD N&S >= 1"))
	{
	  if (pt_ordered.at(0).pt > pt_cut1[0] && pt_ordered.at(1).pt > pt_cut2[0])
	    {	      
	      auto h_pt1_pt2_mbd = dynamic_cast<TH2*>(hm->getHisto("h_pt1_pt2_mbd"));
	      h_pt1_pt2_mbd->Fill(pt_ordered.at(0).pt, pt_ordered.at(1).pt);
	      h_pt1_pt2_mbd->Fill(pt_ordered.at(1).pt, pt_ordered.at(0).pt);
	    }
	  if (jets.at(0).et > pt_cut1[0] && jets.at(1).et > pt_cut2[0])
	    {
	      auto h_et1_et2_mbd = dynamic_cast<TH2*>(hm->getHisto("h_et1_et2_mbd"));
	      h_et1_et2_mbd->Fill(jets.at(0).et, jets.at(1).et);
	      h_et1_et2_mbd->Fill(jets.at(1).et, jets.at(0).et);
	    }
	}
      for (int i = 0; i < 8; i++)
	{
	  if (triggeranalyzer->didTriggerFire(m_jet_triggernames[i]))
	    {
	      if (pt_ordered.at(0).pt > pt_cut1[0] && pt_ordered.at(1).pt > pt_cut2[0])
		{
		  auto h_pt1_pt2_gl1 = dynamic_cast<TH2*>(hm->getHisto(Form("h_pt1_pt2_gl1_%d", i)));
		  h_pt1_pt2_gl1->Fill(pt_ordered.at(0).pt, pt_ordered.at(1).pt);
		  h_pt1_pt2_gl1->Fill(pt_ordered.at(1).pt, pt_ordered.at(0).pt);
		}
	      if (jets.at(0).et > pt_cut1[0] && jets.at(1).et > pt_cut2[0])
		{
		  auto h_et1_et2_gl1 = dynamic_cast<TH2*>(hm->getHisto(Form("h_et1_et2_gl1_%d", i)));
		  h_et1_et2_gl1->Fill(jets.at(0).et, jets.at(1).et);
		  h_et1_et2_gl1->Fill(jets.at(1).et, jets.at(0).et);
		}
	    }
	}
    }

  float Aj_pt = (pt_ordered.at(0).pt - pt_ordered.at(1).pt)/(pt_ordered.at(0).pt + pt_ordered.at(1).pt);
  float Aj_et = (jets.at(0).et - jets.at(1).et)/(jets.at(0).et + jets.at(1).et);
  float xj_pt = (pt_ordered.at(1).pt / pt_ordered.at(0).pt);
  float xj_et = (jets.at(1).et / jets.at(0).et);

  if (Verbosity())
    {
      std::cout << "---- DIJET ---- " << std::endl;
      std::cout << "Calculated: " << std::endl;
      std::cout << "    A_j   = " << Aj_pt << " ( " << Aj_et << " ) "<< std::endl; 
      std::cout << "    x_j   = " << xj_pt << " ( " << xj_et << " ) "<< std::endl; 
      std::cout << "    dPhi  = " << dphi << std::endl;
      std::cout << " ------------------------ " << std::endl;
    }
  bool alreadyfilled = false;
  for (int i = 0; i < 3; i++)
    {
      if (pt_ordered.at(0).pt > pt_cut1[i] && pt_ordered.at(1).pt > pt_cut2[i])
	{
	  auto h_dphi = dynamic_cast<TH1*>(hm->getHisto(Form("h_dphi_%d", i)));
	  h_dphi->Fill(dphi);

	  if (!isSim)
	    {
	      if (i == 0 && drawDijets)
		{
		  alreadyfilled = true;
		  std::cout << " FOUND " << std::endl;
		  dijeteventdisplay->FillEvent(topNode, Aj_et, dphi);
		}
	      if (triggeranalyzer->didTriggerFire("MBD N&S >= 1"))
		{
		  auto h_dphi_mb = dynamic_cast<TH1*>(hm->getHisto(Form("h_dphi_mb_%d", i)));
		  h_dphi_mb->Fill(dphi);
		}
	    
	      for (int j = 0; j < 8; j++)
		{
		  if (triggeranalyzer->didTriggerFire(m_jet_triggernames[j]))
		    {
		      auto h_dphi_gl1 = dynamic_cast<TH1*>(hm->getHisto(Form("h_dphi_gl1_%d_%d", j, i)));
		      h_dphi_gl1->Fill(dphi);
		    }
		}
	    }
	  for (int ip = 0; ip < 3; ip++)
	    {
	      if (dphi <= dphi_cut[ip]) break;
	      
	      auto h_Aj = dynamic_cast<TH1*>(hm->getHisto(Form("h_Aj_%d_dp%d", i, ip)));
	      h_Aj->Fill(Aj_pt);
	      auto h_xj = dynamic_cast<TH1*>(hm->getHisto(Form("h_xj_%d_dp%d", i, ip)));
	      h_xj->Fill(xj_pt);

	      auto h_Ajiso = dynamic_cast<TH1*>(hm->getHisto(Form("h_Aj_iso%d_%d_dp%d", (isopt?1:0), i, ip)));
	      h_Ajiso->Fill(Aj_pt);
	      auto h_xjiso = dynamic_cast<TH1*>(hm->getHisto(Form("h_xj_iso%d_%d_dp%d", (isopt?1:0), i, ip)));
	      h_xjiso->Fill(xj_pt);

	      if (!isSim)
		{
		  if (i == 0 && drawDijets)
		    {
		      alreadyfilled = true;
		      std::cout << " FOUND " << std::endl;
		      dijeteventdisplay->FillEvent(topNode, Aj_et, dphi);
		    }
		  if (triggeranalyzer->didTriggerFire("MBD N&S >= 1"))
		    {
		      auto h_Aj_mb = dynamic_cast<TH1*>(hm->getHisto(Form("h_Aj_mb_%d_dp%d", i, ip)));
		      h_Aj_mb->Fill(Aj_pt);
		      auto h_xj_mb = dynamic_cast<TH1*>(hm->getHisto(Form("h_xj_mb_%d_dp%d", i, ip)));
		      h_xj_mb->Fill(xj_pt);
		    }
	    
		  for (int j = 0; j < 8; j++)
		    {
		      if (triggeranalyzer->didTriggerFire(m_jet_triggernames[j]))
			{
			  auto h_Aj_gl1 = dynamic_cast<TH1*>(hm->getHisto(Form("h_Aj_gl1_%d_%d_dp%d", j, i, ip)));
			  h_Aj_gl1->Fill(Aj_pt);
			  auto h_xj_gl1 = dynamic_cast<TH1*>(hm->getHisto(Form("h_xj_gl1_%d_%d_dp%d", j, i, ip)));
			  h_xj_gl1->Fill(xj_pt);
			}
		    }
		}
	    }
	}
      if (jets.at(0).et > pt_cut1[i] && jets.at(1).et > pt_cut2[i])
	{
	  auto h_dphi = dynamic_cast<TH1*>(hm->getHisto(Form("h_dphi_et_%d", i)));
	  h_dphi->Fill(dphi);
	  if (!isSim)
	    {
	      auto h_dphi_et_mb = dynamic_cast<TH1*>(hm->getHisto(Form("h_dphi_et_mb_%d", i)));
	      h_dphi_et_mb->Fill(dphi);
	      for (int j = 0; j < 8; j++)
		{
		  if (triggeranalyzer->didTriggerFire(m_jet_triggernames[j]))
		    {
		      auto h_dphi_et_gl1 = dynamic_cast<TH1*>(hm->getHisto(Form("h_dphi_et_gl1_%d_%d", j, i)));
		      h_dphi_et_gl1->Fill(dphi);
		    }
		}

	    }
	  for (int ip = 0 ; ip < 3; ip++)
	    {
	      if (dphi <= dphi_cut[ip]) break;

	      auto h_Aj = dynamic_cast<TH1*>(hm->getHisto(Form("h_Aj_et_%d_dp%d", i, ip)));
	      h_Aj->Fill(Aj_et);
	      auto h_xj = dynamic_cast<TH1*>(hm->getHisto(Form("h_xj_et_%d_dp%d", i, ip)));
	      h_xj->Fill(xj_et);
	      auto h_Ajiso = dynamic_cast<TH1*>(hm->getHisto(Form("h_Aj_iso%d_%d_dp%d", (isoet?1:0), i, ip)));
	      h_Ajiso->Fill(Aj_et);
	      auto h_xjiso = dynamic_cast<TH1*>(hm->getHisto(Form("h_xj_iso%d_%d_dp%d", (isoet?1:0), i, ip)));
	      h_xjiso->Fill(xj_et);

	      if (!isSim)
		{
		  if (i == 0 && !alreadyfilled && drawDijets)
		    {
		      std::cout << " FOUND " << std::endl;
		      dijeteventdisplay->FillEvent(topNode, Aj_et, dphi);
		    }
		  if (triggeranalyzer->didTriggerFire("MBD N&S >= 1"))
		    {
		      auto h_Aj_et_mb = dynamic_cast<TH1*>(hm->getHisto(Form("h_Aj_et_mb_%d_dp%d", i, ip)));
		      h_Aj_et_mb->Fill(Aj_et);
		      auto h_xj_et_mb = dynamic_cast<TH1*>(hm->getHisto(Form("h_xj_et_mb_%d_dp%d", i, ip)));
		      h_xj_et_mb->Fill(xj_et);
		    }
	      
		  for (int j = 0; j < 8; j++)
		    {
		      if (triggeranalyzer->didTriggerFire(m_jet_triggernames[j]))
			{
			  auto h_Aj_et_gl1 = dynamic_cast<TH1*>(hm->getHisto(Form("h_Aj_et_gl1_%d_%d_dp%d", j, i, ip)));
			  h_Aj_et_gl1->Fill(Aj_et);
			  auto h_xj_et_gl1 = dynamic_cast<TH1*>(hm->getHisto(Form("h_xj_et_gl1_%d_%d_dp%d", j, i, ip)));
			  h_xj_et_gl1->Fill(xj_et);
			}
		    }
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

double FindDijets::get_Dr(struct jetty jet1, struct jetty jet2)
{
  float dphi = jet1.phi - jet2.phi;

  if (dphi > TMath::Pi())
    {
      dphi = dphi - 2.*TMath::Pi();
    }
  if (dphi <= -1*TMath::Pi())
    {
      dphi = dphi + 2. * TMath::Pi();
    }

  return sqrt(TMath::Power(dphi, 2) + TMath::Power(jet1.eta - jet2.eta, 2));
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
  if (!isSim && drawDijets) dijeteventdisplay->Dump();
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
