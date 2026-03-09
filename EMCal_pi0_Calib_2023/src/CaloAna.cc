#include "CaloAna.h"

// G4Hits includes
#include <TLorentzVector.h>
#include <g4main/PHG4Hit.h>
#include <g4main/PHG4HitContainer.h>
#include <g4main/PHG4Particle.h>

// G4Cells includes
#include <g4detectors/PHG4Cell.h>
#include <g4detectors/PHG4CellContainer.h>

// Tower includes
#include <calobase/RawCluster.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawClusterUtility.h>
#include <calobase/RawTower.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/TowerInfo.h>
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfoDefs.h>

// Cluster includes
#include <calobase/RawCluster.h>
#include <calobase/RawClusterContainer.h>

#include <fun4all/Fun4AllHistoManager.h>
#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/getClass.h>

#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>

// MBD
#include <mbd/BbcGeom.h>
#include <mbd/MbdPmtContainerV1.h>
#include <mbd/MbdPmtHit.h>

#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TNtuple.h>
#include <TProfile2D.h>
#include <TTree.h>

#include <Event/Event.h>
#include <Event/packet.h>
#include <cassert>
#include <sstream>
#include <string>

#include <iostream>
#include <utility>
#include "TCanvas.h"
#include "TF1.h"
#include "TFile.h"
#include "TH1F.h"
#include"TRandom3.h"

#include <cdbobjects/CDBTTree.h>  // for CDBTTree
#include <ffamodules/CDBInterface.h>
#include <phool/recoConsts.h>

#include <g4main/PHG4TruthInfoContainer.h>

R__LOAD_LIBRARY(libLiteCaloEvalTowSlope.so)

using namespace std;

CaloAna::CaloAna(const std::string& name, const std::string& filename)
  : SubsysReco(name)
  , detector("HCALIN")
  , outfilename(filename)
{
  _eventcounter = 0;
}

CaloAna::~CaloAna()
{
  delete hm;
  delete g4hitntuple;
  delete g4cellntuple;
  delete towerntuple;
  delete clusterntuple;
}

int CaloAna::Init(PHCompositeNode*)
{
  hm = new Fun4AllHistoManager(Name());
  // create and register your histos (all types) here

  outfile = new TFile(outfilename.c_str(), "RECREATE");

  // correlation plots
  for (int i = 0; i < 96; i++)
  {
    h_mass_eta_lt[i] = new TH1F(Form("h_mass_eta_lt%d", i), "", 50, 0, 0.5);
    h_mass_eta_lt_rw[i] = new TH1F(Form("h_mass_eta_lt_rw%d", i), "", 50, 0, 0.5);
    h_pt_eta[i] = new TH1F(Form("h_pt_eta%d", i), "", 100, 0, 10);
    h_pt_eta_rw[i] = new TH1F(Form("h_pt_eta_rw%d", i), "", 100, 0, 10);
  }

  h_cemc_etaphi = new TH2F("h_cemc_etaphi", "", 96, 0, 96, 256, 0, 256);

  // 1D distributions
  h_InvMass = new TH1F("h_InvMass", "Invariant Mass", 500, 0, 1.0);
  h_InvMass_w = new TH1F("h_InvMass_w", "Invariant Mass", 500, 0, 1.0);
  h_InvMassMix = new TH1F("h_InvMassMix", "Invariant Mass", 120, 0, 1.2);

  // cluster QA
  h_etaphi_clus = new TH2F("h_etaphi_clus", "", 140, -1.2, 1.2, 64, -1 * TMath::Pi(), TMath::Pi());
  h_clusE = new TH1F("h_clusE", "", 100, 0, 10);

  h_emcal_e_eta = new TH1F("h_emcal_e_eta", "", 96, 0, 96);

  h_pt1 = new TH1F("h_pt1", "", 100, 0, 5);
  h_pt2 = new TH1F("h_pt2", "", 100, 0, 5);

  h_nclusters = new TH1F("h_nclusters", "", 1000, 0, 1000);

  h_truth_eta = new TH1F("h_truth_eta", "", 100, -1.2, 1.2);
  h_truth_e = new TH1F("h_truth_e", "", 100, 0, 10);
  h_truth_pt = new TH1F("h_truth_pt", "", 100, 0, 10);
  h_delR_recTrth = new TH1F("h_delR_recTrth", "", 500, 0, 5);
  h_matched_res = new TH2F("h_matched_res","",100,0,1.5,20,-1,1);


  //////////////////////////
  // pT rewieghting
  frw = new TFile("/sphenix/u/bseidlitz/work/analysis/EMCal_pi0_Calib_2023/macros/rw_pt.root");
  for(int i=0; i<96; i++) h_pt_rw[i] = (TH1F*) frw->Get(Form("h_pt_eta%d", i));

  rnd = new TRandom3(); 


  return 0;
}

int CaloAna::process_event(PHCompositeNode* topNode)
{
  _eventcounter++;

  process_towers(topNode);

  return Fun4AllReturnCodes::EVENT_OK;
}

int CaloAna::process_towers(PHCompositeNode* topNode)
{
  if ((_eventcounter % 1000) == 0) std::cout << _eventcounter << std::endl;

  // float emcaldownscale = 1000000 / 800;

  float emcal_hit_threshold = 0.5;  // GeV

  // cuts
  float maxDr = 1.1;
  float maxAlpha = 0.6;
  float clus_chisq_cut = 4;
  float nClus_ptCut = 0.5;
  int max_nClusCount = 300;

  //-----------------------get vertex----------------------------------------//

  float vtx_z = 0;
  if (getVtx)
  {
    GlobalVertexMap* vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
    if (!vertexmap)
    {
      // std::cout << PHWHERE << " Fatal Error - GlobalVertexMap node is missing"<< std::endl;
      std::cout << "CaloAna GlobalVertexMap node is missing" << std::endl;
      // return Fun4AllReturnCodes::ABORTRUN;
    }
    if (vertexmap && !vertexmap->empty())
    {
      GlobalVertex* vtx = vertexmap->begin()->second;
      if (vtx)
      {
        vtx_z = vtx->get_z();
      }
    }
  }



  //////////////////////////////////////////////
  //         towers 
  vector<float> ht_eta;
  vector<float> ht_phi;

  // if (!m_vtxCut || abs(vtx_z) > _vz)  return Fun4AllReturnCodes::EVENT_OK;

  TowerInfoContainer* towers = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC");
  if (towers)
  {
    int size = towers->size();  // online towers should be the same!
    for (int channel = 0; channel < size; channel++)
    {
      TowerInfo* tower = towers->get_tower_at_channel(channel);
      float offlineenergy = tower->get_energy();
      unsigned int towerkey = towers->encode_key(channel);
      int ieta = towers->getTowerEtaBin(towerkey);
      int iphi = towers->getTowerPhiBin(towerkey);
      bool isGood = !(tower->get_isBadChi2());
      if (!isGood && offlineenergy > 0.2)
      {
        ht_eta.push_back(ieta);
        ht_phi.push_back(iphi);
      }
      if (isGood) h_emcal_e_eta->Fill(ieta, offlineenergy);
      if (offlineenergy > emcal_hit_threshold)
      {
        h_cemc_etaphi->Fill(ieta, iphi);
      }
    }
  }

  RawClusterContainer* clusterContainer = findNode::getClass<RawClusterContainer>(topNode, "CLUSTER_POS_COR_CEMC");
  if (!clusterContainer)
  {
    std::cout << PHWHERE << "funkyCaloStuff::process_event - Fatal Error - CLUSTER_CEMC node is missing. " << std::endl;
    return 0;
  }

  float weight =1;

  /////////////////////////////////////////////////
  //// Truth info
  float wieght = 1;
  PHG4TruthInfoContainer* truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
  vector <TLorentzVector> truth_photons;
  if (truthinfo)
  {
    PHG4TruthInfoContainer::Range range = truthinfo->GetPrimaryParticleRange();
    for (PHG4TruthInfoContainer::ConstIterator iter = range.first; iter != range.second; ++iter)
    {
      // Get truth particle
      const PHG4Particle* truth = iter->second;
      if (!truthinfo->is_primary(truth)) continue;
      TLorentzVector myVector;
      myVector.SetXYZM(truth->get_px(), truth->get_py(), truth->get_pz(), 0.13497);

      float energy = myVector.E();
      h_truth_eta->Fill(myVector.Eta());
      h_truth_e->Fill(energy, wieght);
      weight = myVector.Pt()*TMath::Exp(-3*myVector.Pt());
      h_truth_pt->Fill(myVector.Pt(),weight);
      // int id =  truth->get_pid();
      if (debug) std::cout << "M=" <<  myVector.M()  << "   E=" << energy << "  pt=" << myVector.Pt() << "  eta=" << myVector.Eta() << std::endl;
    }

    PHG4TruthInfoContainer::Range second_range = truthinfo->GetSecondaryParticleRange();
    float m_g4 = 0;

    for (PHG4TruthInfoContainer::ConstIterator siter = second_range.first;
          siter != second_range.second; ++siter) {
      if (m_g4 >= 19999) break;
      // Get photons from pi0 decays 
      const PHG4Particle *truth = siter->second;

      if (truth->get_pid() == 22) {
        PHG4Particle *parent = truthinfo->GetParticle(truth->get_parent_id());
        if (parent->get_pid() == 111 && parent->get_track_id() > 0) {
          float phot_pt = sqrt(truth->get_px() * truth->get_px()
                            + truth->get_py() * truth->get_py());
          //float phot_pz = truth->get_pz();
          float phot_e = truth->get_e();
          float phot_phi = atan2(truth->get_py(), truth->get_px());
          float phot_eta = atanh(truth->get_pz() / sqrt(truth->get_px()*truth->get_px()+truth->get_py()*truth->get_py()+truth->get_pz()*truth->get_pz()));

          TLorentzVector photon = TLorentzVector();
          photon.SetPtEtaPhiE(phot_pt, phot_eta, phot_phi,phot_e);
          truth_photons.push_back(photon);

          if (debug) std::cout<< "pt=" <<  phot_pt <<   " e=" << phot_e << " phi=" << phot_phi << " eta=" << phot_eta << endl; 
       }
     }
   }
  }


  //////////////////////////////////////////
  // geometry for hot tower/cluster masking
  std::string towergeomnodename = "TOWERGEOM_CEMC";
  RawTowerGeomContainer* m_geometry = findNode::getClass<RawTowerGeomContainer>(topNode, towergeomnodename);
  if (!m_geometry)
  {
    std::cout << Name() << "::"
              << "CreateNodeTree"
              << ": Could not find node " << towergeomnodename << std::endl;
    throw std::runtime_error("failed to find TOWERGEOM node in RawClusterDeadHotMask::CreateNodeTree");
  }

  RawClusterContainer::ConstRange clusterEnd = clusterContainer->getClusters();
  RawClusterContainer::ConstIterator clusterIter;
  RawClusterContainer::ConstIterator clusterIter2;
  int nClusCount = 0;
  for (clusterIter = clusterEnd.first; clusterIter != clusterEnd.second; clusterIter++)
  {
    RawCluster* recoCluster = clusterIter->second;

    CLHEP::Hep3Vector vertex(0, 0, vtx_z);
    CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetECoreVec(*recoCluster, vertex);

    float clus_pt = E_vec_cluster.perp();
    float clus_chisq = recoCluster->get_chi2();

    if (clus_pt < nClus_ptCut) continue;
    if (clus_chisq > clus_chisq_cut) continue;

    nClusCount++;
  }

  h_nclusters->Fill(nClusCount);

  if (nClusCount > max_nClusCount) return Fun4AllReturnCodes::EVENT_OK;

  float ptMaxCut = 4;

  float pt1ClusCut = 1.3;  // 1.3
  float pt2ClusCut = 0.7;  // 0.7

  if (nClusCount > 30)
  {
    pt1ClusCut += 1.4 * (nClusCount - 29) / 200.0;
    pt2ClusCut += 1.4 * (nClusCount - 29) / 200.0;
  }

  float pi0ptcut = 1.22 * (pt1ClusCut + pt2ClusCut);

  vector<float> save_pt;
  vector<float> save_eta;
  vector<float> save_phi;
  vector<float> save_e;

  float smear = 0.00;

  for (clusterIter = clusterEnd.first; clusterIter != clusterEnd.second; clusterIter++)
  {
    RawCluster* recoCluster = clusterIter->second;

    CLHEP::Hep3Vector vertex(0, 0, vtx_z);
    CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetECoreVec(*recoCluster, vertex);

    float clusE = E_vec_cluster.mag();
    float clus_eta = E_vec_cluster.pseudoRapidity();
    float clus_phi = E_vec_cluster.phi();
    float clus_pt = E_vec_cluster.perp();
    clus_pt *= rnd->Gaus(1,smear);
    float clus_chisq = recoCluster->get_chi2();

    h_clusE->Fill(clusE);
    if (clusE < 0.2) continue;

    if (clus_chisq > clus_chisq_cut) continue;

    // loop over the towers in the cluster
    RawCluster::TowerConstRange towerCR = recoCluster->get_towers();
    RawCluster::TowerConstIterator toweriter;
    float lt_e = -1000;
    unsigned int lt_eta = -1;
    for (toweriter = towerCR.first; toweriter != towerCR.second; ++toweriter)
    {
      int towereta = m_geometry->get_tower_geometry(toweriter->first)->get_bineta();
      int towerphi = m_geometry->get_tower_geometry(toweriter->first)->get_binphi();
      unsigned int key = TowerInfoDefs::encode_emcal(towereta, towerphi);
      unsigned int channel = towers->decode_key(key);
      float energy = towers->get_tower_at_channel(channel)->get_energy();
      if (energy > lt_e)
      {
        lt_e = energy;
        lt_eta = towereta;
      }

    }

    if (lt_eta > 95) continue;

    h_pt_eta[lt_eta]->Fill(clus_pt);

   // float weight = getWeight(lt_eta,clus_pt);
    h_pt_eta_rw[lt_eta]->Fill(clus_pt,weight);


    h_etaphi_clus->Fill(clus_eta, clus_phi);

    TLorentzVector photon1;
    photon1.SetPtEtaPhiE(clus_pt, clus_eta, clus_phi, clusE);

    for (auto tr_phot : truth_photons){
      float delR = photon1.DeltaR(tr_phot);
      h_delR_recTrth->Fill(delR);
      if (debug) std::cout << delR << " ";
      if (delR < 0.015){
        float res = photon1.E()/tr_phot.E();
        h_matched_res->Fill(res,photon1.Eta());
      }
    }
    if (debug) std::cout << std::endl;

    if (clus_pt < pt1ClusCut || clus_pt > ptMaxCut) continue;

    for (clusterIter2 = clusterEnd.first; clusterIter2 != clusterEnd.second; clusterIter2++)
    {
      if (clusterIter == clusterIter2)
      {
        continue;
      }
      RawCluster* recoCluster2 = clusterIter2->second;

      CLHEP::Hep3Vector E_vec_cluster2 = RawClusterUtility::GetECoreVec(*recoCluster2, vertex);

      float clus2E = E_vec_cluster2.mag();
      float clus2_eta = E_vec_cluster2.pseudoRapidity();
      float clus2_phi = E_vec_cluster2.phi();
      float clus2_pt = E_vec_cluster2.perp();
      //clus2_pt *= rnd->Gaus(1,smear);
      float clus2_chisq = recoCluster2->get_chi2();

      if (clus2_pt < pt2ClusCut || clus2_pt > ptMaxCut) continue;
      if (clus2_chisq > clus_chisq_cut) continue;


      TLorentzVector photon2;
      photon2.SetPtEtaPhiE(clus2_pt, clus2_eta, clus2_phi, clus2E);

      if (fabs(clusE - clus2E) / (clusE + clus2E) > maxAlpha) continue;

      if (photon1.DeltaR(photon2) > maxDr) continue;

      TLorentzVector pi0 = photon1 + photon2;
      if (pi0.Pt() < pi0ptcut) continue;


      h_pt1->Fill(photon1.Pt());
      h_pt2->Fill(photon2.Pt());
      h_InvMass->Fill(pi0.M());
      h_InvMass_w->Fill(pi0.M(),weight);
      h_mass_eta_lt[lt_eta]->Fill(pi0.M());
      h_mass_eta_lt_rw[lt_eta]->Fill(pi0.M());
    }
  }  // clus1 loop


  ht_phi.clear();
  ht_eta.clear();

  return Fun4AllReturnCodes::EVENT_OK;
}


int CaloAna::End(PHCompositeNode* /*topNode*/)
{
  outfile->cd();

  outfile->Write();
  outfile->Close();
  delete outfile;
  hm->dumpHistos(outfilename, "UPDATE");
  return 0;
}

float CaloAna::getWeight(int ieta, float pt){
  if (ieta < 0 || ieta > 95) return 0;
  float val = h_pt_rw[ieta]->GetBinContent(h_pt_rw[ieta]->FindBin(pt));
  if (val==0) return 0;
  return 1/val;
}


TF1* CaloAna::fitHistogram(TH1* h)
{
  TF1* fitFunc = new TF1("fitFunc", "[0]/[2]/2.5*exp(-0.5*((x-[1])/[2])^2) + [3] + [4]*x + [5]*x^2 + [6]*x^3", h->GetXaxis()->GetXmin(), h->GetXaxis()->GetXmax());

  fitFunc->SetParameter(0, 5);
  fitFunc->SetParameter(1, target_pi0_mass);
  fitFunc->SetParameter(2, 0.01);
  fitFunc->SetParameter(3, 0.0);
  fitFunc->SetParameter(4, 0.0);
  fitFunc->SetParameter(5, 0.0);
  fitFunc->SetParameter(6, 100);

  fitFunc->SetParLimits(0, 0,10);
  fitFunc->SetParLimits(1, 0.113, 0.25);
  fitFunc->SetParLimits(2, 0.01, 0.04);
  fitFunc->SetParLimits(3,-2 ,1 );
  fitFunc->SetParLimits(4,0 ,40 );
  fitFunc->SetParLimits(5, -150,50 );
  fitFunc->SetParLimits(6, 0,200 );

  fitFunc->SetRange(0.05, 0.7);

  // Perform the fit
  h->Fit("fitFunc", "QN");

  return fitFunc;
}


void CaloAna::fitEtaSlices(const std::string& infile, const std::string& fitOutFile, const std::string& cdbFile)
{
  TFile* fin = new TFile(infile.c_str());
  std::cout << "getting hists" << std::endl;
  TH1F* h_peak_eta = new TH1F("h_peak_eta", "", 96, 0, 96);
  TH1F* h_sigma_eta = new TH1F("h_sigma_eta", "", 96, 0, 96);
  TH1F* h_p3_eta = new TH1F("h_p3_eta", "", 96, 0, 96);
  TH1F* h_p4_eta = new TH1F("h_p4_eta", "", 96, 0, 96);
  TH1F* h_p5_eta = new TH1F("h_p5_eta", "", 96, 0, 96);
  TH1F* h_p6_eta = new TH1F("h_p6_eta", "", 96, 0, 96);
  TH1F* h_p0_eta = new TH1F("h_p0_eta", "", 96, 0, 96);
  if (!fin)
  {
    std::cout << "pi0EtaByEta::fitEtaSlices null fin" << std::endl;
    exit(1);
  }
  TH1F* h_M_eta[96];
  for (int i = 0; i < 96; i++)
  {
    h_M_eta[i] = (TH1F*) fin->Get(Form("h_mass_eta_lt_rw%d", i));
    h_M_eta[i]->Scale(1./h_M_eta[i]->Integral(),"width");
  }

  TF1* fitFunOut[96];
  for (int i = 0; i < 96; i++)
  {
    if (!h_M_eta[i])
    {
      std::cout << "pi0EtaByEta::fitEtaSlices null hist" << std::endl;
    }

    fitFunOut[i] = fitHistogram(h_M_eta[i]);
    fitFunOut[i]->SetName(Form("f_pi0_eta%d",i));
    float mass_val_out = fitFunOut[i]->GetParameter(1);
    float mass_err_out = fitFunOut[i]->GetParError(1);
    h_peak_eta->SetBinContent(i + 1, mass_val_out);
    if (isnan(h_M_eta[i]->GetEntries())){
       h_peak_eta->SetBinError(i + 1, 0);
       continue;
    }
    h_peak_eta->SetBinError(i + 1, mass_err_out);
    h_sigma_eta->SetBinContent(i+1,fitFunOut[i]->GetParameter(2));
    h_sigma_eta->SetBinError(i+1,fitFunOut[i]->GetParError(2));
    h_p3_eta->SetBinContent(i+1,fitFunOut[i]->GetParameter(3));
    h_p3_eta->SetBinError(i+1,fitFunOut[i]->GetParError(3));
    h_p4_eta->SetBinContent(i+1,fitFunOut[i]->GetParameter(4));
    h_p4_eta->SetBinError(i+1,fitFunOut[i]->GetParError(4));
    h_p5_eta->SetBinContent(i+1,fitFunOut[i]->GetParameter(5));
    h_p5_eta->SetBinError(i+1,fitFunOut[i]->GetParError(5));
    h_p6_eta->SetBinContent(i+1,fitFunOut[i]->GetParameter(6));
    h_p6_eta->SetBinError(i+1,fitFunOut[i]->GetParError(6));
    h_p0_eta->SetBinContent(i+1,fitFunOut[i]->GetParameter(0));
    h_p0_eta->SetBinError(i+1,fitFunOut[i]->GetParError(0));
  }

  CDBTTree* cdbttree1 = new CDBTTree(cdbFile.c_str());
  CDBTTree* cdbttree2 = new CDBTTree(cdbFile.c_str());

  std::string m_fieldname = "Femc_datadriven_qm1_correction";

  for (int i = 0; i < 96; i++)
  {
    for (int j = 0; j < 256; j++)
    {
      float correction = target_pi0_mass / h_peak_eta->GetBinContent(i + 1);
      unsigned int key = TowerInfoDefs::encode_emcal(i, j);
      float val1 = cdbttree1->GetFloatValue(key, m_fieldname);
      cdbttree2->SetFloatValue(key, m_fieldname, val1 * correction);
    }
  }

  cdbttree2->Commit();
  cdbttree2->WriteCDBTTree();
  delete cdbttree2;
  delete cdbttree1;

  TFile* fit_out = new TFile(fitOutFile.c_str(), "recreate");
  fit_out->cd();
  for (auto& i : h_M_eta)
  {
    i->Write();
    delete i;
  }
  for (auto& i : fitFunOut)
  {
    i->Write();
    delete i;
  }

  h_p3_eta->Write();
  h_p4_eta->Write();
  h_p5_eta->Write();
  h_p6_eta->Write();
  h_p0_eta->Write();
  h_sigma_eta->Write();
  h_peak_eta->Write();
  fin->Close();

  std::cout << "finish fitting suc" << std::endl;

  return;
}
