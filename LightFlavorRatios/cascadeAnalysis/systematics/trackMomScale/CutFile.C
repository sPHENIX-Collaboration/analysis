#include <string>
#include <iostream>
#include <sstream>
#include <cmath>
#include <TFile.h>
#include <TTree.h>
#include <TCut.h>
#include <TChain.h>
#include <TMath.h>
#include <TPad.h>
#include <TCanvas.h>
#include <TBranch.h>

using namespace std;

void CutFile()
{
  stringstream cutStream;
  //cutStream << "track_3_MVTX_nStates > 0 && track_3_INTT_nStates > 0 && ((Lambda0_track_1_MVTX_nStates == 0 && Lambda0_track_1_INTT_nStates == 0) || Lambda0_track_1_bunch_crossing == track_3_bunch_crossing) && ((Lambda0_track_2_MVTX_nStates == 0 && Lambda0_track_2_INTT_nStates == 0) || Lambda0_track_2_bunch_crossing == track_3_bunch_crossing) && track_1_track_2_DCA_xy < 0.05";
  //cutStream << "abs(K_S0_rapidity) <= 1.0 && track_1_pT >= 0.2 && track_2_pT >= 0.2 && track_1_bunch_crossing == track_2_bunch_crossing && track_1_bunch_crossing >= 0 && track_1_bunch_crossing < 350 && track_2_bunch_crossing >= 0 && track_2_bunch_crossing < 350 &&  abs(primary_vertex_z) <= 10";
  //cutStream << "track_1_MVTX_nStates > 0 && track_2_MVTX_nStates > 0 && track_1_INTT_nStates > 0 && track_2_INTT_nStates > 0 && track_1_TPC_nStates > 19 && track_2_TPC_nStates > 0 && track_1_chi2/track_1_nDoF <= 300 && track_2_chi2/track_2_nDoF <= 300 && abs(track_1_IP_xy) >= 0.05 && abs(track_2_IP_xy) >= 0.05 && track_1_track_2_DCA <= 0.5 && track_1_track_2_DCA <= 1.0 && K_S0_DIRA >= 0.99 && K_S0_chi2/K_S0_nDoF <= 20";
  cutStream << "abs(Lambda0_track_1_true_ID) == 211 && Sum$(abs(Lambda0_track_1_true_track_history_PDG_ID) == 3122) > 0 && Sum$(abs(Lambda0_track_1_true_track_history_PDG_ID) == 3312) > 0 && abs(Lambda0_track_2_true_ID) == 2212 && Sum$(abs(Lambda0_track_2_true_track_history_PDG_ID) == 3122) > 0 && Sum$(abs(Lambda0_track_2_true_track_history_PDG_ID) == 3312) > 0 && abs(track_3_true_ID) == 211 && Sum$(abs(track_3_true_track_history_PDG_ID) == 3312) > 0 && Sum$(abs(track_3_true_track_history_PDG_ID) == 3122) == 0 && Lambda0_track_1_MVTX_nHits > 0 && Lambda0_track_2_MVTX_nHits > 0 && track_3_MVTX_nHits > 0 && Lambda0_track_1_INTT_nHits > 0 && Lambda0_track_2_INTT_nHits > 0 && track_3_INTT_nHits > 0 && Lambda0_track_1_TPC_nHits > 19 && Lambda0_track_2_TPC_nHits > 19 && track_3_TPC_nHits > 19 && abs(Lambda0_mass - 1.1157) <= 0.01 && Ximinus_decayLength_xy >= 0.15 && Lambda0_decayLength_xy >= 0.01 && (Lambda0_track_1_chi2/Lambda0_track_1_nDoF) <= 400 && (Lambda0_track_2_chi2/Lambda0_track_2_nDoF) <= 400 && (track_3_chi2/track_3_nDoF) <= 400 && track_1_track_2_DCA <= 0.5 && track_1_track_3_DCA <= 0.5 && track_2_track_3_DCA <= 0.5 && Ximinus_chi2/Ximinus_nDoF <= 50 && Lambda0_chi2/Lambda0_nDoF <= 50 && abs(Ximinus_rapidity) <= 1.0";
  TCut masscut = cutStream.str().c_str();
 
  TFile* originalFile = new TFile("outputKFParticleXiminusSV.root");
  TTree* originalTree = (TTree*)originalFile->Get("DecayTree");

  TFile* tempFile = new TFile("outputKFParticleXiminusSV_filtered.root", "RECREATE");
  tempFile->cd();
  //TTree* dataTree_Cut = dataChain->CopyTree("");
  TTree* dataTree_Cut = originalTree->CopyTree(masscut);
  dataTree_Cut->Write();
  tempFile->Close();
}
