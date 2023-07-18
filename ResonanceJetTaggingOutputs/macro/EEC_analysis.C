#include "TTree.h"
#include "TH1.h"
#include <vector>
#include <cmath>
#include "TFile.h"

void EEC_analysis()
{

  // Histograms
  //
  // reco
  TH1F *EEC = new TH1F("EEC", ";R_{L};EEC",50, 0.001, 0.5);
  EEC->Sumw2();
  TH1F *rD = new TH1F("rD", ";r_{D};",50, 0.0, 0.5);
  rD->Sumw2();
  TH1F *rDcons = new TH1F("rDcons", ";r_{Dcons};",50, 0.0, 1.0);
  rDcons->Sumw2();
  TH1F *rcons = new TH1F("rcons",";r_{cons};",50,0.0,4.0);
  rcons->Sumw2();
  TH1F *ptD = new TH1F("ptD", ";p_{T}^{D};", 50, 0., 30.);
  ptD->Sumw2();
  TH1F *ptjet = new TH1F("ptjet",";p_{T}^{jet};",50, 0., 40.);
  ptjet->Sumw2();
  TH1F *ptcons = new TH1F("ptcons",";p_{T}^{cons};",50, 0., 10.);
  ptcons->Sumw2();
  TH1F *mD = new TH1F("mD", ";m_{D};", 50, 1.7, 2.);
  mD->Sumw2();
  //
  //truth
  TH1F *truthEEC = new TH1F("truthEEC", ";R_{L};EEC",50, 0.001, 0.5);
  truthEEC->Sumw2();
  TH1F *truthrD = new TH1F("truthrD", ";r_{D};",50, 0.0, 0.5);
  truthrD->Sumw2();
  TH1F *truthrDcons = new TH1F("truthrDcons", ";r_{Dcons};",50, 0.0, 1.0);
  truthrDcons->Sumw2();
  TH1F *truthrcons = new TH1F("truthrcons",";r_{cons};",50,0.0,4.0);
  truthrcons->Sumw2();
  TH1F *truthptD = new TH1F("truthptD", ";p_{T}^{D};", 50, 0., 30.);
  truthptD->Sumw2();
  TH1F *truthptjet = new TH1F("truthptjet",";p_{T}^{jet};",50, 0., 40.);
  truthptjet->Sumw2();
  TH1F *truthptcons = new TH1F("truthptcons",";p_{T}^{cons};",50, 0., 10.);
  truthptcons->Sumw2();
  TH1F *truthmD = new TH1F("truthmD", ";m_{D};", 50, 1.7, 2.);
  truthmD->Sumw2();

  //resolution
  TH1F *D_eta_res = new TH1F("D_eta_res", "; #Delta #eta^{D};", 50, -0.02, 0.02);
  D_eta_res->Sumw2();
  TH1F *D_phi_res = new TH1F("D_phi_res", "; #Delta #phi^{D};", 50, -0.02, 0.02);
  D_phi_res->Sumw2();
  TH1F *D_p_res = new TH1F("D_p_res", "; #Delta p^{D};", 50, -0.5, 0.5);
  D_p_res->Sumw2();
  TH1F *D_pt_res = new TH1F("D_pt_res", "; #Delta p_{T}^{D};", 50, -0.5, 0.5);
  D_pt_res->Sumw2();
  TH1F *jet_eta_res = new TH1F("jet_eta_res", "; #Delta #eta^{jet};", 50, -0.2, 0.2);
  jet_eta_res->Sumw2();
  TH1F *jet_phi_res = new TH1F("jet_phi_res", "; #Delta #phi^{jet};", 50, -0.2, 0.2);
  jet_phi_res->Sumw2();
  TH1F *jet_p_res = new TH1F("jet_p_res", "; #Delta p^{jet};", 50, -5., 5.);
  jet_p_res->Sumw2();
  TH1F *jet_pt_res = new TH1F("jet_pt_res", "; #Delta p_{T}^{jet};", 50, -5., 5.);
  jet_pt_res->Sumw2();

  TH2F *D_etaphi_res_widerange = new TH2F("D_eta_phi_widerange", ";#Delta #phi^{D}; #Delta #eta^{D}",50, -0.5, 0.5, 50, -0.3, 0.3);
    TH2F *D_etaphi_res = new TH2F("D_eta_phi", ";#Delta #phi^{D}; #Delta #eta^{D}",50, -0.02, 0.02, 50, -0.02, 0.02);

  // I/O
  TString infilename = "myTestReco_D0/D0JetTree_dst_tracks_5k.root";
  TFile *infile = new TFile(infilename);
  TFile *outfile = new TFile("Djet_histos.root", "RECREATE");


  // Read input Tree
  TTree *Djettree = (TTree*)infile->Get("m_taggedjettree");
  
  float reco_tag_px, reco_tag_py, reco_tag_pz, reco_tag_e, reco_tag_m;
  float reco_jet_px, reco_jet_py, reco_jet_pz, reco_jet_e;
  float truth_tag_px, truth_tag_py, truth_tag_pz, truth_tag_e, truth_tag_m;
  float truth_jet_px, truth_jet_py, truth_jet_pz, truth_jet_e;
  std::vector<float>* reco_const_px = nullptr;
  std::vector<float>* reco_const_py = nullptr;
  std::vector<float>* reco_const_pz = nullptr;
  std::vector<float>* reco_const_e = nullptr;
  std::vector<float>* truth_const_px = nullptr;
  std::vector<float>* truth_const_py = nullptr;
  std::vector<float>* truth_const_pz = nullptr;
  std::vector<float>* truth_const_e = nullptr;

  Djettree->SetBranchAddress("m_reco_tag_px", &reco_tag_px);
  Djettree->SetBranchAddress("m_reco_tag_py", &reco_tag_py);
  Djettree->SetBranchAddress("m_reco_tag_pz", &reco_tag_pz);
  Djettree->SetBranchAddress("m_reco_tag_e",  &reco_tag_e);
  Djettree->SetBranchAddress("m_reco_tag_m",  &reco_tag_m);

  Djettree->SetBranchAddress("m_reco_jet_px", &reco_jet_px);
  Djettree->SetBranchAddress("m_reco_jet_py", &reco_jet_py);
  Djettree->SetBranchAddress("m_reco_jet_pz", &reco_jet_pz);
  Djettree->SetBranchAddress("m_reco_jet_e",  &reco_jet_e);

  Djettree->SetBranchAddress("m_recojet_const_px", &reco_const_px);
  Djettree->SetBranchAddress("m_recojet_const_py", &reco_const_py);
  Djettree->SetBranchAddress("m_recojet_const_pz", &reco_const_pz);
  Djettree->SetBranchAddress("m_recojet_const_e",  &reco_const_e);

  Djettree->SetBranchAddress("m_truth_tag_px", &truth_tag_px);
  Djettree->SetBranchAddress("m_truth_tag_py", &truth_tag_py);
  Djettree->SetBranchAddress("m_truth_tag_pz", &truth_tag_pz);
  Djettree->SetBranchAddress("m_truth_tag_e",  &truth_tag_e);
  Djettree->SetBranchAddress("m_truth_tag_m",  &truth_tag_m);

  Djettree->SetBranchAddress("m_truth_jet_px", &truth_jet_px);
  Djettree->SetBranchAddress("m_truth_jet_py", &truth_jet_py);
  Djettree->SetBranchAddress("m_truth_jet_pz", &truth_jet_pz);
  Djettree->SetBranchAddress("m_truth_jet_e",  &truth_jet_e);

  Djettree->SetBranchAddress("m_truthjet_const_px", &truth_const_px);
  Djettree->SetBranchAddress("m_truthjet_const_py", &truth_const_py);
  Djettree->SetBranchAddress("m_truthjet_const_pz", &truth_const_pz);
  Djettree->SetBranchAddress("m_truthjet_const_e",  &truth_const_e);

  double weight = 0;
  double truthweight = 0;
  int numEntires = Djettree->GetEntries();
  for (int i=0; i<numEntires; ++i)
    {
      Djettree->GetEntry(i);

      if (!isnan(reco_tag_px) && !isnan(reco_jet_px) && !isnan(truth_tag_px) && !isnan(truth_jet_px)) 
	{
	  //std::cout << "tagged D0 mass : " << reco_tag_m << std::endl; \
	  //std::cout << "jet 4 vec : ( " << reco_jet_px << ", " << reco_jet_py << ", " << reco_jet_pz << ", " << reco_jet_e << ")" << std::endl;

	  // reco 
	  TLorentzVector tag4vec(reco_tag_px, reco_tag_py, reco_tag_pz, reco_tag_e);
	  TLorentzVector jet4vec(reco_jet_px, reco_jet_py, reco_jet_pz, reco_jet_e);

	  // truth
	  TLorentzVector truthtag4vec(truth_tag_px, truth_tag_py, truth_tag_pz, truth_tag_e);
	  TLorentzVector truthjet4vec(truth_jet_px, truth_jet_py, truth_jet_pz, truth_jet_e);
	  
	  //std::cout << "number of constituents : " << reco_const_px->size() << std::endl;
	  // reco
	  if (jet4vec.DeltaR(tag4vec,false) > 0.4) {continue;}
	  rD->Fill(jet4vec.DeltaR(tag4vec, false));
	  ptD->Fill(tag4vec.Pt());
	  ptjet->Fill(jet4vec.Pt());
	  mD->Fill(reco_tag_m);

	  // truth
	  truthrD->Fill(truthjet4vec.DeltaR(truthtag4vec, false));
	  truthptD->Fill(truthtag4vec.Pt());
	  truthptjet->Fill(truthjet4vec.Pt());
	  truthmD->Fill(truth_tag_m);

	  // res
	  D_eta_res->Fill(tag4vec.Eta() - truthtag4vec.Eta());
	  D_phi_res->Fill(tag4vec.Phi() - truthtag4vec.Phi());
	  D_p_res->Fill(tag4vec.P() - truthtag4vec.P());
	  D_pt_res->Fill(tag4vec.Pt() - truthtag4vec.Pt());

	  D_etaphi_res_widerange->Fill(tag4vec.Phi() - truthtag4vec.Phi(), tag4vec.Eta() - truthtag4vec.Eta());
	  D_etaphi_res->Fill(tag4vec.Phi() - truthtag4vec.Phi(), tag4vec.Eta() - truthtag4vec.Eta());

	  jet_eta_res->Fill(jet4vec.Eta() - truthjet4vec.Eta());
	  jet_phi_res->Fill(jet4vec.Phi() - truthjet4vec.Phi());
	  jet_p_res->Fill(jet4vec.P() - truthjet4vec.P());
	  jet_pt_res->Fill(jet4vec.Pt() - truthjet4vec.Pt());

	  // reco
	  for(int j=0; j<reco_const_px->size(); ++j)
	    {
	      TLorentzVector con4vec(reco_const_px->at(j), reco_const_py->at(j), reco_const_pz->at(j), reco_const_e->at(j));
	      ptcons->Fill(con4vec.Pt());
	      double angle = tag4vec.Angle(con4vec.Vect());
	      //std::cout <<" cos angle " << cos(angle) << std::endl;
	      //std::cout << "angle " << angle << std::endl;
	      double RL = tag4vec.DeltaR(con4vec, false);
	      rcons->Fill(jet4vec.DeltaR(con4vec,false));
	      if (jet4vec.DeltaR(con4vec,false) > 0.4) {continue;}
	      weight = con4vec.Pt()*tag4vec.Pt()/(jet4vec.Pt()*jet4vec.Pt());
	      //std::cout << "weight : " << weight << std::endl;
	      EEC->Fill(RL,weight);
	      rDcons->Fill(tag4vec.DeltaR(con4vec,false));

	    }

	  // truth
	  for(int j=0; j<truth_const_px->size(); ++j)
	    {
	      TLorentzVector truthcon4vec(truth_const_px->at(j), truth_const_py->at(j), truth_const_pz->at(j), truth_const_e->at(j));
	      truthptcons->Fill(truthcon4vec.Pt());
	      double truthangle = truthtag4vec.Angle(truthcon4vec.Vect());
	      //std::cout <<" cos angle " << cos(angle) << std::endl;
	      //std::cout << "angle " << angle << std::endl;
	      double truthRL = truthtag4vec.DeltaR(truthcon4vec, false);
	      truthrcons->Fill(truthjet4vec.DeltaR(truthcon4vec,false));
	      truthweight = truthcon4vec.Pt()*truthtag4vec.Pt()/(truthjet4vec.Pt()*truthjet4vec.Pt());
	      //std::cout << "weight : " << weight << std::endl;
	      truthEEC->Fill(truthRL,truthweight);
	      truthrDcons->Fill(truthtag4vec.DeltaR(truthcon4vec,false));
	    }
	}
    }
  TCanvas *c1 = new TCanvas("c1");
  EEC->Draw();
  truthEEC->SetLineColor(kRed);
  truthEEC->Draw("same");
  TCanvas *c2 = new TCanvas("c2");
  rcons->Draw();
  truthrcons->SetLineColor(kRed);
  truthrcons->Draw("same");
  TCanvas *c3 = new TCanvas("c3");
  rD->Draw();
  truthrD->SetLineColor(kRed);
  truthrD->Draw("same");
  TCanvas *c4 = new TCanvas("c4");
  rDcons->Draw();
  truthrDcons->SetLineColor(kRed);
  truthrDcons->Draw("same");
  TCanvas *c5 = new TCanvas("c5");
  ptD->Draw();
  truthptD->SetLineColor(kRed);
  truthptD->Draw("same");
  TCanvas *c6 = new TCanvas("c6");
  ptjet->Draw();
  truthptjet->SetLineColor(kRed);
  truthptjet->Draw("same");
  TCanvas *c7 = new TCanvas("c7");
  ptcons->Draw();
  truthptcons->SetLineColor(kRed);
  truthptcons->Draw("same");
  TCanvas *c8 = new TCanvas("c8");
  truthmD->SetLineColor(kRed);
  truthmD->Draw();
  mD->Draw("same");
  TCanvas *c9 = new TCanvas("c9");
  mD->Draw();
  TCanvas *c10 = new TCanvas("c10");
  D_phi_res->Draw();
  D_phi_res->Write();
  TCanvas *c11 = new TCanvas("c11");
  D_eta_res->Draw();
  D_eta_res->Write();
  TCanvas *c12 = new TCanvas("c12");
  D_p_res->Draw();
  D_p_res->Write();
  TCanvas *c13 = new TCanvas("c13");
  D_pt_res->Draw();
  D_pt_res->Write();
  TCanvas *c14 = new TCanvas("c14");
  jet_phi_res->Draw();
  jet_phi_res->Write();
  TCanvas *c15 = new TCanvas("c15");
  jet_eta_res->Draw();
  jet_eta_res->Write();
  TCanvas *c16 = new TCanvas("c16");
  jet_p_res->Draw();
  jet_p_res->Write();
  TCanvas *c17 = new TCanvas("c17");
  jet_pt_res->Draw();
  jet_pt_res->Write();
  TCanvas *c18 = new TCanvas("c18");
  D_etaphi_res_widerange->Draw("colz");
  D_etaphi_res_widerange->Write();
  TCanvas *c19 = new TCanvas("c19");
  D_etaphi_res->Draw("colz");
  D_etaphi_res->Write();
  
  THStack *EECstack = new THStack("EECstack",";R_{L};EEC");
  EECstack->Add(EEC);
  EECstack->Add(truthEEC);
  EECstack->Write();
  THStack *rconsstack = new THStack("rconsstack", ";r_{cons};");
  rconsstack->Add(rcons);
  rconsstack->Add(truthrcons);
  rconsstack->Write();
  THStack *rDstack = new THStack("rDstack", ";r_{D};");
  rDstack->Add(rD);
  rDstack->Add(truthrD);
  rDstack->Write();
  THStack *rDconsstack = new THStack("rDconsstack", ";r_{Dcons};");
  rDconsstack->Add(rDcons);
  rDconsstack->Add(truthrDcons);
  rDconsstack->Write();
  THStack *ptDstack = new THStack("ptDstack", ";p_{T}^{D};");
  ptDstack->Add(ptD);
  ptDstack->Add(truthptD);
  ptDstack->Write();
  THStack *ptjetstack = new THStack("ptjetstack", ";p_{T}^{jet};");
  ptjetstack->Add(ptjet);
  ptjetstack->Add(truthptjet);
  ptjetstack->Write();
  THStack *ptconsstack = new THStack("ptconsstack", ";p_{T}^{cons};");
  ptconsstack->Add(ptcons);
  ptconsstack->Add(truthptcons);
  ptconsstack->Write();
  THStack *mDstack = new THStack("mDstack", ";m_{D};");
  mDstack->Add(truthmD);
  mDstack->Add(mD);
  mDstack->Write();
  
  EEC->Write();
  truthEEC->Write();
  rcons->Write();
  truthrcons->Write();
  rD->Write();
  truthrD->Write();
  
  
  outfile->Write();
  outfile->Close();
}
