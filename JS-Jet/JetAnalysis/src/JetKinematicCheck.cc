//____________________________________________________________________________..

#include "JetKinematicCheck.h"

#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/Fun4AllServer.h>
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


#include <TFile.h>
#include <TH3D.h>
#include <TH2D.h>
#include <TH1D.h>
#include <TPad.h>
#include <TLegend.h>
#include <cmath>
#include <string>
#include<vector>

//____________________________________________________________________________..
JetKinematicCheck::JetKinematicCheck(const std::string& recojetnameR02, const std::string& recojetnameR04, const std::string& outputfilename):
SubsysReco("JetKinematicCheck")
 , m_recoJetNameR02(recojetnameR02)
 , m_recoJetNameR04(recojetnameR04)
 , m_outputFileName(outputfilename)
 , m_etaRange(-1.1, 1.1)
 , m_ptRange(10, 100)

{
  std::cout << "JetKinematicCheck::JetKinematicCheck(const std::string &name) Calling ctor" << std::endl;
}

//____________________________________________________________________________..
JetKinematicCheck::~JetKinematicCheck()
{
  std::cout << "JetKinematicCheck::~JetKinematicCheck() Calling dtor" << std::endl;
}

//centrality labels
std::vector<std::string>JetKinematicCheck::GetCentLabels()
{

  std::vector<std::string> cent_labels = { "Inclusive", "0-10%", "10-20%","20-30%","30-40%", "40-50%",
					   "50-60%", "60-70%", "70-80%", "80-90%", "90-100%"};
  return cent_labels;
}

//marker colors
std::vector<int>JetKinematicCheck::GetMarkerColors()
{
  std::vector<int> colors = { kBlack, kRed, kBlue, kGreen+2, kViolet, kCyan,
			      kOrange, kPink+2, kMagenta, kTeal+3, kRed+3};
  return colors;
}



//____________________________________________________________________________..
int JetKinematicCheck::Init(PHCompositeNode *topNode)
{

  PHTFileServer::get().open(m_outputFileName, "RECREATE");

  jet_spectra_r02 = new TH2D("h_spectra_r02", "", 15, 10, 100, 10, 0, 100);
  jet_spectra_r02->GetXaxis()->SetTitle("p_{T} [GeV/c]");
  jet_spectra_r04 = new TH2D("h_spectra_r04", "", 15, 10, 100, 10, 0, 100);
  jet_spectra_r04->GetXaxis()->SetTitle("p_{T} [GeV/c]");
  jet_eta_phi_r02 = new TH3D("h_eta_phi_r02","", 24, -1.1, 1.1, 64, -M_PI, M_PI, 10, 0, 100);
  jet_eta_phi_r02->GetXaxis()->SetTitle("#eta");
  jet_eta_phi_r02->GetYaxis()->SetTitle("#Phi");
  jet_eta_phi_r04 = new TH3D("h_eta_phi_r04","", 24, -1.1, 1.1, 64, -M_PI, M_PI, 10, 0, 100);
  jet_eta_phi_r04->GetXaxis()->SetTitle("#eta");
  jet_eta_phi_r04->GetYaxis()->SetTitle("#Phi");

  std::cout << "JetKinematicCheck::Init(PHCompositeNode *topNode) Initializing" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}


//____________________________________________________________________________..
int JetKinematicCheck::InitRun(PHCompositeNode *topNode)
{
  std::cout << "JetKinematicCheck::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}


//____________________________________________________________________________..
int JetKinematicCheck::process_event(PHCompositeNode *topNode)
{

  std::vector<std::string> m_recoJetName_array = {m_recoJetNameR02, m_recoJetNameR04};

  m_radii = {0.2, 0.4};
  int n_radii = m_radii.size();

  for(int i = 0; i < n_radii; i++){

    std::string recoJetName = m_recoJetName_array[i];

    // interface to reco jets
    JetMap* jets = findNode::getClass<JetMap>(topNode, recoJetName);
    if (!jets)
      {
	std::cout
	  << "MyJetAnalysis::process_event - Error can not find DST Reco JetMap node "
	  << m_recoJetNameR04 << std::endl;
	exit(-1);
      }

    //centrality
    CentralityInfo* cent_node = findNode::getClass<CentralityInfo>(topNode, "CentralityInfo");
    if (!cent_node)
      {
	std::cout
	  << "MyJetAnalysis::process_event - Error can not find centrality node "
	  << std::endl;
	exit(-1);
      }

    //get the event centrality
    m_centrality =  cent_node->get_centile(CentralityInfo::PROP::mbd_NS);

    for (JetMap::Iter iter = jets->begin(); iter != jets->end(); ++iter)
      {

	Jet* jet = iter->second;

	bool eta_cut = (jet->get_eta() >= m_etaRange.first) and (jet->get_eta() <= m_etaRange.second);
	bool pt_cut = (jet->get_pt() >= m_ptRange.first) and (jet->get_pt() <= m_ptRange.second);
	if ((not eta_cut) or (not pt_cut)) continue;
	if(jet->get_pt() < 1) continue; // to remove noise jets

	if(i == 0){
	  jet_spectra_r02->Fill(jet->get_pt(), m_centrality);
	  jet_eta_phi_r02->Fill(jet->get_eta(), jet->get_phi(), m_centrality);

	}

	else if(i == 1){
	  jet_spectra_r04->Fill(jet->get_pt(), m_centrality);
	  jet_eta_phi_r04->Fill(jet->get_eta(), jet->get_phi(), m_centrality);
	}
      }
  }
  std::cout << "JetKinematicCheck::process_event(PHCompositeNode *topNode) Processing Event" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

int count = 0;

//____________________________________________________________________________..
int JetKinematicCheck::ResetEvent(PHCompositeNode *topNode)
{
  std::cout << "JetKinematicCheck::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << std::endl;

  std:: cout << count << std::endl;
  count++;


  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetKinematicCheck::EndRun(const int runnumber)
{
  std::cout << "JetKinematicCheck::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetKinematicCheck::End(PHCompositeNode *topNode)
{

  std::cout << "JetKinematicCheck::End - Output to " << m_outputFileName << std::endl;
  PHTFileServer::get().cd(m_outputFileName);

  jet_spectra_r02->Write();
  jet_spectra_r04->Write();
  jet_eta_phi_r02->Write();
  jet_eta_phi_r04->Write();

  auto cent_labels = GetCentLabels();
  auto colors = GetMarkerColors();
  ncent = cent_labels.size();

  //projecting jet_spectra =_r02
  for(int i = 0; i < ncent; i++){
    TLegend *leg1 = new TLegend(.7,.9,.9,1);
    leg1->SetFillStyle(0);
    leg1->SetBorderSize(0);
    leg1->SetTextSize(0.06);
    leg1->AddEntry((TObject*)0, Form("%2.0f < p_{T} < %2.0f [GeV/c]", m_ptRange.first, m_ptRange.second),"");
    leg1->AddEntry((TObject*)0, Form("%1.1f < #eta < %1.1f", m_etaRange.first, m_etaRange.second),"");
    //for inclusive in centrality
    if(i==0){
      jet_spectra_r02_proj[i] = (TH1F*)jet_spectra_r02->ProjectionX(Form("Jet_Spectra_R02_%s",cent_labels[i].c_str()));
      jet_spectra_r02_proj[i]->SetMarkerStyle(kFullCircle);
      jet_spectra_r02_proj[i]->SetMarkerColor(colors[i]);
      jet_spectra_r02_proj[i]->SetLineColor(colors[i]);
      jet_spectra_r02_proj[i]->SetTitle(Form("Jet Spectra R02 [%s]", cent_labels[i].c_str()));
      jet_spectra_r02_proj[i]->GetYaxis()->SetTitle("Entries");
      jet_spectra_r02_proj[i]->GetListOfFunctions()->Add(leg1);
      jet_spectra_r02_proj[i]->SetStats(0);
      jet_spectra_r02_proj[i]->Write();
    }
    //for all other centrality
    else{
      jet_spectra_r02_proj[i] = (TH1F*)jet_spectra_r02->ProjectionX(Form("Jet_Spectra_R02_%s",cent_labels[i].c_str()), i, i);
      jet_spectra_r02_proj[i]->SetMarkerStyle(kFullCircle);
      jet_spectra_r02_proj[i]->SetMarkerColor(colors[i]);
      jet_spectra_r02_proj[i]->SetLineColor(colors[i]);
      jet_spectra_r02_proj[i]->SetTitle(Form("Jet Spectra R02 [%s]", cent_labels[i].c_str()));
      jet_spectra_r02_proj[i]->GetYaxis()->SetTitle("Entries");
      jet_spectra_r02_proj[i]->GetListOfFunctions()->Add(leg1);
      jet_spectra_r02_proj[i]->SetStats(0);
      jet_spectra_r02_proj[i]->Write();
    }
  }


  //projecting jet_spectra_r04
  for(int i = 0; i < ncent; i++){
    TLegend *leg2 = new TLegend(.7,.9,.9,1);
    leg2->SetFillStyle(0);
    leg2->SetBorderSize(0);
    leg2->SetTextSize(0.06);
    leg2->AddEntry((TObject*)0, Form("%2.0f < p_{T} < %2.0f [GeV/c]", m_ptRange.first, m_ptRange.second),"");
    leg2->AddEntry((TObject*)0, Form("%1.1f < #eta < %1.1f", m_etaRange.first, m_etaRange.second),"");
    if(i==0){
      jet_spectra_r04_proj[i] = (TH1F*)jet_spectra_r04->ProjectionX(Form("Jet_Spectra_R04_%s",cent_labels[i].c_str()));
      jet_spectra_r04_proj[i]->SetMarkerStyle(kFullCircle);
      jet_spectra_r04_proj[i]->SetMarkerColor(colors[i]);
      jet_spectra_r04_proj[i]->SetLineColor(colors[i]);
      jet_spectra_r04_proj[i]->SetTitle(Form("Jet Spectra R04 [%s]", cent_labels[i].c_str()));
      jet_spectra_r04_proj[i]->GetYaxis()->SetTitle("Entries");
      jet_spectra_r04_proj[i]->GetListOfFunctions()->Add(leg2);
      jet_spectra_r04_proj[i]->SetStats(0);
      jet_spectra_r04_proj[i]->Write();
    }
    else{
      jet_spectra_r04_proj[i] = (TH1F*)jet_spectra_r04->ProjectionX(Form("Jet_Spectra_R04_%s",cent_labels[i].c_str()), i, i);
      jet_spectra_r04_proj[i]->SetMarkerStyle(kFullCircle);
      jet_spectra_r04_proj[i]->SetMarkerColor(colors[i]);
      jet_spectra_r04_proj[i]->SetLineColor(colors[i]);
      jet_spectra_r04_proj[i]->SetTitle(Form("Jet Spectra R04 [%s]", cent_labels[i].c_str()));
      jet_spectra_r04_proj[i]->GetYaxis()->SetTitle("Entries");
      jet_spectra_r04_proj[i]->GetListOfFunctions()->Add(leg2);
      jet_spectra_r04_proj[i]->SetStats(0);
      jet_spectra_r04_proj[i]->Write();
    }
  }

  //projecting jet_eta_phi_r02
  for(int i = 0; i < ncent; i++){
    TLegend *leg3 = new TLegend(.7,.9,.9,1);
    leg3->SetFillStyle(0);
    leg3->SetBorderSize(0);
    leg3->SetTextSize(0.06);
    leg3->AddEntry((TObject*)0, Form("%2.0f < p_{T} < %2.0f [GeV/c]", m_ptRange.first, m_ptRange.second),"");
    leg3->AddEntry((TObject*)0, Form("%1.1f < #eta < %1.1f", m_etaRange.first, m_etaRange.second),"");
    //for inclusive in centrality
    if(i==0){
      jet_eta_phi_r02_proj[i] = new TH2D;
      jet_eta_phi_r02_proj[i] = (TH2D*)jet_eta_phi_r02->Project3D("yx");
      jet_eta_phi_r02_proj[i]->SetStats(0);
      jet_eta_phi_r02_proj[i]->SetTitle(Form("Jet Eta-Phi R02 [%s]", cent_labels[i].c_str()));
      jet_eta_phi_r02_proj[i]->GetListOfFunctions()->Add(leg3);
      jet_eta_phi_r02_proj[i]->Write(Form("Jet_Eta_Phi_R02_%s", cent_labels[i].c_str()));
    }
    //for all other centrality
    else{
      jet_eta_phi_r02->GetZaxis()->SetRange(i, i);
      jet_eta_phi_r02_proj[i] = new TH2D;
      jet_eta_phi_r02_proj[i] = (TH2D*)jet_eta_phi_r02->Project3D("yx");
      jet_eta_phi_r02_proj[i]->SetStats(0);
      jet_eta_phi_r02_proj[i]->SetTitle(Form("Jet Eta-Phi R02 [%s]", cent_labels[i].c_str()));
      jet_eta_phi_r02_proj[i]->GetListOfFunctions()->Add(leg3);
      jet_eta_phi_r02_proj[i]->Write(Form("Jet_Eta_Phi_R02_%s", cent_labels[i].c_str()));
    }
  }



  //projecting jet_eta_phi_r04
  for(int i = 0; i < ncent; i++){
    TLegend *leg4 = new TLegend(.7,.9,.9,1);
    leg4->SetFillStyle(0);
    leg4->SetBorderSize(0);
    leg4->SetTextSize(0.06);
    leg4->AddEntry((TObject*)0, Form("%2.0f < p_{T} < %2.0f [GeV/c]", m_ptRange.first, m_ptRange.second),"");
    leg4->AddEntry((TObject*)0, Form("%1.1f < #eta < %1.1f", m_etaRange.first, m_etaRange.second),"");
    if(i==0){
      jet_eta_phi_r04_proj[i] = new TH2D;
      jet_eta_phi_r04_proj[i] = (TH2D*)jet_eta_phi_r04->Project3D("yx");
      jet_eta_phi_r04_proj[i]->SetStats(0);
      jet_eta_phi_r04_proj[i]->SetTitle(Form("Jet Eta-Phi R04 [%s]", cent_labels[i].c_str()));
      jet_eta_phi_r04_proj[i]->GetListOfFunctions()->Add(leg4);
      jet_eta_phi_r04_proj[i]->Write(Form("Jet_Eta_Phi_R04_%s", cent_labels[i].c_str()));
    }
    else{
      jet_eta_phi_r04->GetZaxis()->SetRange(i, i);
      jet_eta_phi_r04_proj[i] = new TH2D;
      jet_eta_phi_r04_proj[i] = (TH2D*)jet_eta_phi_r04->Project3D("yx");
      jet_eta_phi_r04_proj[i]->SetStats(0);
      jet_eta_phi_r04_proj[i]->SetTitle(Form("Jet Eta-Phi R04 [%s]", cent_labels[i].c_str()));
      jet_eta_phi_r04_proj[i]->GetListOfFunctions()->Add(leg4);
      jet_eta_phi_r04_proj[i]->Write(Form("Jet_Eta_Phi_R04_%s", cent_labels[i].c_str()));
    }
  }


  std::cout << "JetKinematicCheck::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetKinematicCheck::Reset(PHCompositeNode *topNode)
{
 std::cout << "JetKinematicCheck::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void JetKinematicCheck::Print(const std::string &what) const
{
  std::cout << "JetKinematicCheck::Print(const std::string &what) const Printing info for " << what << std::endl;
}
