#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TCanvas.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>
#include <vector>
#include <iostream>
#include <TDatabasePDG.h>
#include <TString.h>
#include <set>
#include <TChain.h>
#include <TGraph.h>
#include <TLegend.h>
#include <TMath.h>

//#include "sPhenixStyle.h"
//#include "sPhenixStyle.C"

#include "JetAnalysis.h"
#include <ranges>



//JetAnalysis::JetAnalysis(){
//}

std::tuple<TCanvas*, TPad**, TH1D**> PrepareCanvas4Pad(UInt_t xAxisBins, Double_t *xAxis);

bool CheckValue(ROOT::Internal::TTreeReaderValueBase& value) {
   if (value.GetSetupStatus() < 0) {
      std::cerr << "Error " << value.GetSetupStatus()
                << "setting up reader for " << value.GetBranchName() << '\n';
      return false;
   }
   return true;
}

/*void JetAnalysis::MatchedJetContainer::Reset()
{
    vtxtype = GlobalVertex::VTXTYPE::UNDEFINED;
    reco_pt = NAN;
    reco_jet_nChConstituents = NAN;
    reco_jet_nConstituents = NAN;
    reco_constituents_Sdxy.clear();
    reco_jet_Sdxy_1N = NAN;
    reco_jet_Sdxy_2N = NAN;
    reco_jet_Sdxy_3N = NAN;
    reco_jet_Sdxy_4N = NAN;
    truth_jet_flavour = {};
    truth_pt = NAN;
    truth_jet_nChConstituents = NAN;
    truth_jet_nConstituents = NAN;
}*/

bool JetAnalysis::CheckValue(ROOT::Internal::TTreeReaderValueBase& value) {
   if (value.GetSetupStatus() < 0) {
      std::cerr << "Error " << value.GetSetupStatus()
                << "setting up reader for " << value.GetBranchName() << '\n';
      return false;
   }
   return true;
}

// Analyze the tree "MyTree" in the file passed into the function.
// Returns false in case of errors.
bool JetAnalysis::processCondor(const std::string &dataFiles) {
   TChain *tc = new TChain("AntiKt_r04/Data");
   tc->Add(dataFiles.data());
   TTreeReader reader(tc);
   
   jet_container_ = new TTreeReaderValue<FullJetFinder::Container>(reader,"JetContainer");
   TString filename = work_dir_ + "outtree_v2.root";
   //TFile *f = new TFile(filename,"RECREATE");
   //TTree *ttree_out = new TTree("Data", "Data");
   //MatchedJetContainer matched_jets_out;
   //ttree_out->Branch( "matched_jets", &matched_jets_out );


   TH1D *h_reco_all = new TH1D("h_reco_all","reco_all",400,0,100);
   h_reco_all->GetXaxis()->SetTitle("pt_jet_reco");

   TH1D *h_truth_all = new TH1D("h_truth_all","truth_all",400,0,100);
   h_truth_all->GetXaxis()->SetTitle("pt_jet^truth");

   TH1D *h_reco_unmatch = new TH1D("h_reco_unmatch","reco_unmatch",400,0,100);
   h_reco_unmatch->GetXaxis()->SetTitle("pt_jet^reco");

   TH1D *h_reco_match = new TH1D("h_reco_match","reco_match",400,0,100);
   h_reco_match->GetXaxis()->SetTitle("pt_jet^reco");

   TH1D *h_truth_match = new TH1D("h_truth_match","truth_match",400,0,100);
   h_truth_match->GetXaxis()->SetTitle("pt_jet^truth");

   TH1D *h_phi_all = new TH1D("h_phi_all","phi_all",800,-4,4);
   h_phi_all->GetXaxis()->SetTitle("phi");

   TH1D *h_phi_match = new TH1D("h_phi_match","phi_match",800,-4,4);
   h_phi_match->GetXaxis()->SetTitle("phi_match");

   TH1D *h_eta_all = new TH1D("h_eta_all","eta_all",300,-1.5,1.5);
   h_eta_all->GetXaxis()->SetTitle("eta");

   TH1D *h_eta_match = new TH1D("h_eta_match","eta_match",300,-1.5,1.5);
   h_eta_match->GetXaxis()->SetTitle("eta_match");

   TH1D *h_phi_truth_all = new TH1D("h_phi_truth_all","phi_truth_all",800,-4,4);
   h_phi_truth_all->GetXaxis()->SetTitle("phi");

   TH1D *h_phi_truth_match = new TH1D("h_phi_truth_match","phi_truth_match",800,-4,4);
   h_phi_truth_match->GetXaxis()->SetTitle("phi_match");

   TH1D *h_eta_truth_all = new TH1D("h_eta_truth_all","eta_truth_all",300,-1.5,1.5);
   h_eta_truth_all->GetXaxis()->SetTitle("eta");

   TH1D *h_eta_truth_match = new TH1D("h_eta_truth_match","eta_truth_match",300,-1.5,1.5);
   h_eta_match->GetXaxis()->SetTitle("eta_match");


   TH1D *h_eta_track = new TH1D("h_eta_track","h_eta_track",300,-1.5,1.5);
   h_eta_track->GetXaxis()->SetTitle("eta_match");

   TH1D *h_phi_track = new TH1D("h_phi_track","h_phi_track",800,-4,4);
   h_phi_track->GetXaxis()->SetTitle("phi");

   TH1D *h_eta_track_match = new TH1D("h_eta_track_match","h_eta_track_match",300,-1.5,1.5);
   h_eta_track_match->GetXaxis()->SetTitle("eta_match");

   TH1D *h_phi_track_match = new TH1D("h_phi_track_match","h_phi_track_match",800,-4,4);
   h_phi_track_match->GetXaxis()->SetTitle("phi");

   TH2D *h_phi_pt_track = new TH2D("h_phi_pt_track","h_phi_pt_track",60,0,30,800,-4,4);
   TH2D *h_dca_xy = new TH2D("h_dca_xy","dca_xy",60,0,30,100,-2,2);
   TH2D *h_dca_xy_unc = new TH2D("dca_xy_unc","dca_xy_unc",60,0,30,100,0,0.02);
   TH2D *h_nmvtx = new TH2D("h_nmvtx","h_nmvtx_all",60,0,30,6,0,6);
   TH2D *h_nintt = new TH2D("h_nintt","h_nintt_all",60,0,30,5,0,5);
   TH2D *h_ntpc = new TH2D("h_ntpc","h_ntpc_all",60,0,30,50,0,50);
   TH2D *h_chi2ndof = new TH2D("h_chi2ndof","h_chi2ndof_all",60,0,30,80,0,20);

   h_phi_pt_track->GetXaxis()->SetTitle("Track p_T");
   h_dca_xy->GetXaxis()->SetTitle("Track p_T");
   h_dca_xy_unc->GetXaxis()->SetTitle("Track p_T");
   h_nmvtx->GetXaxis()->SetTitle("Track p_T");
   h_nintt->GetXaxis()->SetTitle("Track p_T");
   h_ntpc->GetXaxis()->SetTitle("Track p_T");
   h_chi2ndof->GetXaxis()->SetTitle("Track p_T");

   TH2D *h_jet_area = new TH2D("h_jet_area","h_jet_area",60,0,1.2,60,0,1.2);
   h_jet_area->GetXaxis()->SetTitle("area_jet_truth");
   h_jet_area->GetYaxis()->SetTitle("area_jet_reco");
   
   int num = 5;
   TH1D *h_sDCA[num][3]; //variables, flavour (0 - lf, 1 - C, 2 - B)
    std::string tagName[5] = {"Sdxy","1_N","2_N","3_N","4_N"};//"dxy","dxy_unc",
    for(int i = 0; i<num;i++){ 
      TString name = "matching_"+tagName[i];
      h_sDCA[i][0] = new TH1D(name+"_LF",name+"_LF",100,-40,40);
      h_sDCA[i][1] = new TH1D(name+"_C",name+"_C",100,-40,40);
      h_sDCA[i][2] = new TH1D(name+"_B",name+"_B",100,-40,40);
      for(int j = 0; j<3;j++){ 
         h_sDCA[i][j]->GetYaxis()->SetTitle("Probability distribution");
         h_sDCA[i][j]->GetXaxis()->SetTitle("2D Impact Parameter significance");
      }
    }

   TCanvas *c2 = new TCanvas("c2","c2",2400,800);
   c2->Divide(5,1);

    TH1I *h_matching[4];
    std::string matName[4] = {"total","LF","C","B"};
    for(int i = 0; i<4;i++){ 
      TString name = "matching_"+matName[i];
      h_matching[i]= new TH1I(name,name,5,0,5);
      h_matching[i]->GetXaxis()->SetBinLabel(1,"Uniquely matched");
      h_matching[i]->GetXaxis()->SetBinLabel(2,"Unmatched R");
      h_matching[i]->GetXaxis()->SetBinLabel(3,"Unmatched T");
      h_matching[i]->GetXaxis()->SetBinLabel(4,"2T -> 1R");
      h_matching[i]->GetXaxis()->SetBinLabel(5,"1T -> 2R");
    }

    TH1I *h_pflow[4];
    std::string matName2[4] = {"Uniquely_matched","Unmatched_R","2T_to_1R","1T_to_2R"};
    for(int i = 0; i<4;i++){ 
      TString name = "h_pflow_"+matName2[i];
      h_pflow[i]= new TH1I(name,name,6,-1.5,4.5);
      h_pflow[i]->GetXaxis()->SetBinLabel(1,"UNASSIGNED");
      h_pflow[i]->GetXaxis()->SetBinLabel(2,"MATCHED_CHARGED_HADRON");
      h_pflow[i]->GetXaxis()->SetBinLabel(3,"UNMATCHED_CHARGED_HADRON");
      h_pflow[i]->GetXaxis()->SetBinLabel(4,"UNMATCHED_EM_PARTICLE");
      h_pflow[i]->GetXaxis()->SetBinLabel(5,"UNMATCHED_NEUTRAL_HADRON");
      h_pflow[i]->GetXaxis()->SetBinLabel(6,"LEFTOVER_EM_PARTICLE");
    }

   TCanvas *c3 = new TCanvas("c3","c3",2400,800);
    c3->Divide(8,1);

   TH1I *h_primvtx[6];
   std::string matName3[6] = {"All","z<10cm","matched_jets_to_vtx","c-jets","b-jets","unmatched R"};
   for(int i = 0; i<6;i++){ 
      TString name = "h_primvtx_"+matName3[i];
      h_primvtx[i]= new TH1I(name,name,3,-0.5,2.5);
      h_primvtx[i]->GetXaxis()->SetBinLabel(1,"SVTX_MBD");
      h_primvtx[i]->GetXaxis()->SetBinLabel(2,"SVTX");
      h_primvtx[i]->GetXaxis()->SetBinLabel(3,"MBD");
   }

   TH1I *h_n_vtx = new TH1I("h_n_vtx","number of primary vertices in event",5,0,5);
   TH1I *h_n_vtx_jet = new TH1I("h_n_vtx_jet","number of primary vertices inside jet",5,0,5);

   // Read a TriggerInfo object from the tree entries:
   // Now iterate through the TTree entries and fill a histogram.

  // int nentries = reader.GetEntries();
   int nentries = tc->GetEntries();
   std::cout<<"events "<<nentries<<std::endl;


   // Create a new ROOT file
TFile *file = new TFile("/sphenix/tg/tg01/hf/jkvapil/JET30_R22_npile_full/HF_matched_jets.root", "RECREATE");

// Create a new TTree
TTree *tree = new TTree("Data", "A tree with various branches");

// Define variables for branches
float id;
float vertex_x, vertex_y, vertex_z;
float vertex_x_unc, vertex_y_unc, vertex_z_unc;
float vertex_chisq;
int vertex_ndf;

float reco_area;
int reco_num_ChConstituents;
float reco_px, reco_py, reco_pz, reco_pt, reco_eta, reco_phi, reco_m, reco_e;
float truth_area;
int truth_num_ChConstituents;
float truth_px, truth_py, truth_pz, truth_pt, truth_eta, truth_phi, truth_m, truth_e;
int truth_flavour;

// Define vectors for leaves
std::vector<float> track_e, track_eta, track_phi, track_px, track_py, track_pz, track_pt;
std::vector<int> track_charge;
std::vector<float> track_DCA_xy, track_DCA_xy_unc, track_sDCA_xy, track_DCA3d, track_sDCA3d;
std::vector<int> track_n_mvtx, track_n_intt, track_n_tpc;
std::vector<float> track_chisq;
std::vector<int> track_ndf;

// Create branches
tree->Branch("vertex_x", &vertex_x, "vertex_x/F");
tree->Branch("vertex_y", &vertex_y, "vertex_y/F");
tree->Branch("vertex_z", &vertex_z, "vertex_z/F");
tree->Branch("vertex_x_unc", &vertex_x_unc, "vertex_x_unc/F");
tree->Branch("vertex_y_unc", &vertex_y_unc, "vertex_y_unc/F");
tree->Branch("vertex_z_unc", &vertex_z_unc, "vertex_z_unc/F");
tree->Branch("vertex_chisq", &vertex_chisq, "vertex_chisq/F");
tree->Branch("vertex_ndf", &vertex_ndf, "vertex_ndf/I");
tree->Branch("id", &id, "id/F");
tree->Branch("reco_area", &reco_area, "reco_area/F");
tree->Branch("reco_num_ChConstituents", &reco_num_ChConstituents, "reco_num_ChConstituents/I");
tree->Branch("reco_px", &reco_px, "reco_px/F");
tree->Branch("reco_py", &reco_py, "reco_py/F");
tree->Branch("reco_pz", &reco_pz, "reco_pz/F");
tree->Branch("reco_pt", &reco_pt, "reco_pt/F");
tree->Branch("reco_eta", &reco_eta, "reco_eta/F");
tree->Branch("reco_phi", &reco_phi, "reco_phi/F");
tree->Branch("reco_m", &reco_m, "reco_m/F");
tree->Branch("reco_e", &reco_e, "reco_e/F");
tree->Branch("reco_sdxy_1N", &reco_sdxy_1N, "reco_sdxy_1N/F");
tree->Branch("reco_sdxy_2N", &reco_sdxy_2N, "reco_sdxy_2N/F");
tree->Branch("reco_sdxy_3N", &reco_sdxy_3N, "reco_sdxy_3N/F");
tree->Branch("truth_area", &truth_area, "truth_area/F");
tree->Branch("truth_num_ChConstituents", &truth_num_ChConstituents, "truth_num_ChConstituents/I");
tree->Branch("truth_px", &truth_px, "truth_px/F");
tree->Branch("truth_py", &truth_py, "truth_py/F");
tree->Branch("truth_pz", &truth_pz, "truth_pz/F");
tree->Branch("truth_pt", &truth_pt, "truth_pt/F");
tree->Branch("truth_eta", &truth_eta, "truth_eta/F");
tree->Branch("truth_phi", &truth_phi, "truth_phi/F");
tree->Branch("truth_m", &truth_m, "truth_m/F");
tree->Branch("truth_e", &truth_e, "truth_e/F");
tree->Branch("truth_flavour", &truth_flavour, "truth_flavour/I");


// Create leaves
tree->Branch("track_e","std::vector<float>", &track_e);
tree->Branch("track_eta", &track_eta);
tree->Branch("track_phi", &track_phi);
tree->Branch("track_px", &track_px);
tree->Branch("track_py", &track_py);
tree->Branch("track_pz", &track_pz);
tree->Branch("track_pt", &track_pt);
tree->Branch("track_charge", &track_charge);
tree->Branch("track_DCA_xy", &track_DCA_xy);
tree->Branch("track_DCA_xy_unc", &track_DCA_xy_unc);
tree->Branch("track_sDCA_xy", &track_sDCA_xy);
tree->Branch("track_DCA3d", &track_DCA3d);
tree->Branch("track_sDCA3d", &track_sDCA3d);
tree->Branch("track_n_mvtx", &track_n_mvtx);
tree->Branch("track_n_intt", &track_n_intt);
tree->Branch("track_n_tpc", &track_n_tpc);
tree->Branch("track_chisq", &track_chisq);
tree->Branch("track_ndf", &track_ndf);

   //loop over events
   while (reader.Next()) {
      if (!CheckValue(*jet_container_)) return false;
      printProgress(reader.GetCurrentEntry(),nentries);

      //vectors to hold jet ID for matching
      std::vector<int> matchedR;
      std::vector<int> matchedT;
      std::vector<int> JetIDR;
      std::vector<int> JetIDT;

      //if jet does not pass a selection it will not reach here and then can have single jet with id 1 while missing id 0
      //this is a map between jetid and vector position
      std::map<int,int> JetIDR_map;
      std::map<int,int> JetIDT_map;

      //std::cout<<"recojetid: ";

      //IDs of reco jets
      int local_index = -1;
      for (auto reco_jet : (**jet_container_).recojets) {
         local_index++;
         if(reco_jet.pt < 10.0 || reco_jet.pt > 120.0) continue;
         if(std::abs(reco_jet.eta) > 0.7) continue;
         if(reco_jet.area < 3.1415*0.4*0.4*0.6) continue;
         JetIDR.push_back(reco_jet.id);
         JetIDR_map.insert(std::pair<int, int>(reco_jet.id, local_index));  

         h_reco_all->Fill(reco_jet.pt);
         h_eta_all->Fill(reco_jet.eta);
         h_phi_all->Fill(reco_jet.phi);
         for (auto chtrk : reco_jet.chConstituents){
            h_phi_track->Fill(chtrk.phi);
            h_eta_track->Fill(chtrk.eta);
         }
         
        // std::cout<<reco_jet.id<<" ";
      }
     // std::cout<<std::endl;
     // std::cout<<"truthjetid: ";

      //IDs of truth jet
      local_index = -1;
      for (auto truth_jet : (**jet_container_).truthjets) {
         local_index++;
         if(truth_jet.pt < 30.0 || truth_jet.pt > 120.0) continue;
         if(std::abs(truth_jet.eta) > 0.7) continue;
         if(truth_jet.area < 3.1415*0.4*0.4*0.6) continue;
         JetIDT.push_back(truth_jet.id);
         JetIDT_map.insert(std::pair<int, int>(truth_jet.id, local_index));
         h_truth_all->Fill(truth_jet.pt);
         h_eta_truth_all->Fill(truth_jet.eta);
         h_phi_truth_all->Fill(truth_jet.phi);
         //std::cout<<truth_jet.id<<" ";
      }

     // std::cout<<std::endl;

     
            




      //main matching loop
      std::vector<std::pair<int,int>> matchedPair;
      for (auto reco_jet : (**jet_container_).recojets) {
         if(reco_jet.pt < 10.0 || reco_jet.pt > 120.0) continue;
         if(std::abs(reco_jet.eta) > 0.7) continue;
         if(reco_jet.area < 3.1415*0.4*0.4*0.6) continue;
         for (auto truth_jet : (**jet_container_).truthjets) {
            if(truth_jet.pt < 30.0 || truth_jet.pt > 120.0) continue;
            if(std::abs(truth_jet.eta) > 0.7) continue;
            if(truth_jet.area < 3.1415*0.4*0.4*0.6) continue;
         
            double etaR = reco_jet.eta;
            double etaT = truth_jet.eta;
            double phiR = reco_jet.phi;
            double phiT = truth_jet.phi;
            double dR = std::sqrt((etaR-etaT)*(etaR-etaT)+(phiR-phiT)*(phiR-phiT));
            if (dR > 0.3){
               // unmatched
               continue;
            } 
            else{
               matchedPair.push_back(std::pair(reco_jet.id,truth_jet.id));
               matchedR.push_back(reco_jet.id);
               matchedT.push_back(truth_jet.id);
            }
         }
      }

      //are there unmatched reco jets?
      std::vector<int> unmatchedR;
      std::set_difference(JetIDR.begin(), JetIDR.end(), matchedR.begin(), matchedR.end(), std::inserter(unmatchedR, unmatchedR.begin()));
      //unmatched truth
      std::vector<int> unmatchedT;
      std::set_difference(JetIDT.begin(), JetIDT.end(), matchedT.begin(), matchedT.end(), std::inserter(unmatchedT, unmatchedT.begin()));
      //are there doubly matched jets?
      //2 reco -> 1 truth
      std::set<int> doublymatchedR = findDuplicates(matchedR);
      //truth -> 2 reco
      std::set<int> doublymatchedT = findDuplicates(matchedT);

      /*for (auto truth_jet : (**jet_container_).truthjets){  
         Flavour flavour = getFlavour(truth_jet);
         if(std::find(unmatchedT.begin(), unmatchedT.end(), truth_jet.id)!=unmatchedT.end()){
            h_matching[0]->Fill(2.5);
            if(flavour.isLF) h_matching[1]->Fill(2.5);
            if(flavour.isC & !flavour.isB) h_matching[2]->Fill(2.5);
            if(flavour.isB) h_matching[3]->Fill(2.5);
         }
      }*/
//std::cout<<"matched pair: "<<std::endl;
      //matching stats
      for(auto &id : matchedPair){
         if(std::find(doublymatchedR.begin(), doublymatchedR.end(),id.first) != doublymatchedR.end()) continue;
         if(std::find(doublymatchedT.begin(), doublymatchedT.end(),id.second) != doublymatchedT.end()) continue;
         //std::cout<<"<"<<id.first<<","<<id.second<<"> ";
         Flavour flavour = getFlavour((**jet_container_).truthjets.at(JetIDT_map[id.second]));  
         h_matching[0]->Fill(0.5);
         if(flavour.isLF) h_matching[1]->Fill(0.5);
         if(flavour.isC & !flavour.isB) h_matching[2]->Fill(0.5);
         if(flavour.isB) h_matching[3]->Fill(0.5);
         h_reco_match->Fill((**jet_container_).recojets.at(JetIDR_map[id.first]).pt);
         h_truth_match->Fill((**jet_container_).truthjets.at(JetIDT_map[id.second]).pt);
         h_phi_match->Fill((**jet_container_).recojets.at(JetIDR_map[id.first]).phi);
         h_eta_match->Fill((**jet_container_).recojets.at(JetIDR_map[id.first]).eta);
         h_phi_truth_match->Fill((**jet_container_).truthjets.at(JetIDT_map[id.second]).phi);
         h_eta_truth_match->Fill((**jet_container_).truthjets.at(JetIDT_map[id.second]).eta);
      }
//std::cout<<std::endl<<"Unmatch R ID "<<std::endl;
      for (auto& id : unmatchedR){
         //std::cout<<id<<" ";
         h_matching[0]->Fill(1.5);
         h_reco_unmatch->Fill((**jet_container_).recojets.at(JetIDR_map[id]).pt);
         //if((**jet_container_).recojets.at(JetIDR_map[id]).chConstituents.size() > 0){
            /*if((**jet_container_).primaryVertex.at((**jet_container_).recojets.at(JetIDR_map[id]).chConstituents.at(0).vtx_id).vtxtype == GlobalVertex::VTXTYPE::SVTX_MBD){
                  h_primvtx[5]->Fill(0);
               } 
               if((**jet_container_).primaryVertex.at((**jet_container_).recojets.at(JetIDR_map[id]).chConstituents.at(0).vtx_id).vtxtype == GlobalVertex::VTXTYPE::SVTX){
                  h_primvtx[5]->Fill(1);
               } 
               if((**jet_container_).primaryVertex.at((**jet_container_).recojets.at(JetIDR_map[id]).chConstituents.at(0).vtx_id).vtxtype == GlobalVertex::VTXTYPE::MBD){
                  h_primvtx[5]->Fill(2);
            } */
         //}
         for(auto con : (**jet_container_).recojets.at(JetIDR_map[id]).chConstituents){
            h_pflow[1]->Fill(static_cast<int>(con.pflowtype));
            h_phi_track_match->Fill(con.phi);
            h_eta_track_match->Fill(con.eta);
            h_phi_pt_track->Fill(con.pt,con.phi);
         } 
         for(auto con : (**jet_container_).recojets.at(JetIDR_map[id]).neConstituents) h_pflow[1]->Fill(static_cast<int>(con.pflowtype));
      }
//std::cout<<std::endl<<"Unmatch T ID "<<std::endl;
      for(auto& id : unmatchedT){
         //std::cout<<id<<" ";
         Flavour flavour = getFlavour((**jet_container_).truthjets.at(JetIDT_map[id]));
         h_matching[0]->Fill(2.5);
         if(flavour.isLF) h_matching[1]->Fill(2.5);
         if(flavour.isC & !flavour.isB) h_matching[2]->Fill(2.5);
         if(flavour.isB) h_matching[3]->Fill(2.5);
      }
//std::cout<<std::endl<<"Doubly R ID "<<std::endl;   
      for (auto &id: doublymatchedR) {
         for(auto id2 :matchedPair){
            if(id2.first == id){
               //std::cout<<id2.second<<" ";
               Flavour flavour = getFlavour((**jet_container_).truthjets.at(JetIDT_map[id2.second]));
               h_matching[0]->Fill(3.5);
               if(flavour.isLF) h_matching[1]->Fill(3.5);
               if(flavour.isC & !flavour.isB) h_matching[2]->Fill(3.5);
               if(flavour.isB) h_matching[3]->Fill(3.5);          
            }
         }
      }
//std::cout<<std::endl<<"Doubly T ID "<<std::endl;  
      for (auto &id: doublymatchedT) {
         for(auto id2 :matchedPair){
            if(id2.second == id){
               //std::cout<<id2.second<<" ";
               Flavour flavour = getFlavour((**jet_container_).truthjets.at(JetIDT_map[id2.second]));
               h_matching[0]->Fill(4.5);
               if(flavour.isLF) h_matching[1]->Fill(4.5);
               if(flavour.isC & !flavour.isB) h_matching[2]->Fill(4.5);
               if(flavour.isB) h_matching[3]->Fill(4.5);          
            }
         }
      }    
      //std::cout<<std::endl;
/*
         for (auto reco_jet : (**jet_container_).recojets){            
            if(std::find(unmatchedR.begin(), unmatchedR.end(),reco_jet.id) != unmatchedR.end()){
               for(auto con : reco_jet.chConstituents){
                   pflow[1]->Fill(static_cast<int>(con.pflowtype));
                  if((**jet_container_).primaryVertex.at(con.vtx_id).vtxtype == GlobalVertex::VTXTYPE::SVTX_MBD) primvtx[5]->Fill(0);
                  if((**jet_container_).primaryVertex.at(con.vtx_id).vtxtype == GlobalVertex::VTXTYPE::SVTX) primvtx[5]->Fill(1);
                  if((**jet_container_).primaryVertex.at(con.vtx_id).vtxtype == GlobalVertex::VTXTYPE::MBD) primvtx[5]->Fill(2);

               }
               for(auto con : reco_jet.neConstituents) pflow[1]->Fill(static_cast<int>(con.pflowtype));
               
            } 
         }*/


//std::cout<<"B"<<std::endl;
      
     
      

      /*
      for (int ijetR = 0; ijetR < **n_reco_jet_; ijetR++) {
         r->Fill((**reco_jet_pt_).at(ijetR));
         if(std::find(unmatchedR.begin(), unmatchedR.end(),(**reco_jet_id_).at(ijetR)) != unmatchedR.end()){
            ru->Fill((**reco_jet_pt_).at(ijetR));
         }

      }
         for (int ijetT = 0; ijetT < **n_truth_jet_; ijetT++) { 
            t->Fill((**truth_jet_pt_).at(ijetT));
         }
      */

      //get vertex info
      //int num_vtx = 0;
      /*for(auto vertex : (**jet_container_).primaryVertex){
         num_vtx++;
         if(vertex.vtxtype == GlobalVertex::VTXTYPE::SVTX_MBD) h_primvtx[0]->Fill(0);
         if(vertex.vtxtype == GlobalVertex::VTXTYPE::SVTX) h_primvtx[0]->Fill(1);
         if(vertex.vtxtype == GlobalVertex::VTXTYPE::MBD) h_primvtx[0]->Fill(2);
         if(abs(vertex.z) < 10){
            if(vertex.vtxtype == GlobalVertex::VTXTYPE::SVTX_MBD) h_primvtx[1]->Fill(0);
            if(vertex.vtxtype == GlobalVertex::VTXTYPE::SVTX) h_primvtx[1]->Fill(1);
            if(vertex.vtxtype == GlobalVertex::VTXTYPE::MBD) h_primvtx[1]->Fill(2);
         }
      }*/
      //h_n_vtx ->Fill(num_vtx);

//std::cout<<"C"<<std::endl;
      //Main analysis loop
      int iid = 0;
      for (auto truth_jet : (**jet_container_).truthjets){
         Flavour flavour = getFlavour(truth_jet);  
         for (auto reco_jet : (**jet_container_).recojets){            
   //std::cout<<"D"<<std::endl;         
            //find matched combination
            if (std::find(matchedPair.begin(), matchedPair.end(), std::pair(reco_jet.id,truth_jet.id)) == matchedPair.end()) continue;
            //reject doubly matched reco jets  
            if(std::find(doublymatchedR.begin(), doublymatchedR.end(),reco_jet.id) != doublymatchedR.end()){
               //for(auto con : reco_jet.chConstituents) h_pflow[2]->Fill(static_cast<int>(con.pflowtype));
               //for(auto con : reco_jet.neConstituents) h_pflow[2]->Fill(static_cast<int>(con.pflowtype));
               continue;
            } 
            //reject doubly matched truth jets
            if(std::find(doublymatchedT.begin(), doublymatchedT.end(),truth_jet.id) != doublymatchedT.end()){
               //for(auto con : reco_jet.chConstituents) h_pflow[3]->Fill(static_cast<int>(con.pflowtype));
               //for(auto con : reco_jet.neConstituents) h_pflow[3]->Fill(static_cast<int>(con.pflowtype));
               continue;
            } 

            //bool good_vertex = false;
            //for( auto chtrk : reco_jet.chConstituents){
            //   if(abs((**jet_container_).primaryVertex.at(chtrk.vtx_id).z) < 10) good_vertex = true;
           // }
            //if(!good_vertex) continue;

            

       

          /*  int n_vtx_in_jet = 0;
            int vtx_1_id_tmp = -1;
            int vtx_2_id_tmp = -1;

            for(auto con : reco_jet.chConstituents){
               if(vtx_1_id_tmp == -1){
                  vtx_1_id_tmp = con.vtx_id;
                  n_vtx_in_jet++;
               }
               else if ((vtx_1_id_tmp != con.vtx_id) && vtx_2_id_tmp == -1){
                  vtx_2_id_tmp = con.vtx_id;
                  n_vtx_in_jet++;
               }
               else if((vtx_1_id_tmp != con.vtx_id) && (vtx_2_id_tmp != con.vtx_id)){
                  n_vtx_in_jet++;
               }
            }

            h_n_vtx_jet->Fill(n_vtx_in_jet);

            if(n_vtx_in_jet != 1) continue;*/

          /*  if((**jet_container_).primaryVertex.at(reco_jet.chConstituents.at(0).vtx_id).vtxtype == GlobalVertex::VTXTYPE::SVTX_MBD){
               h_primvtx[2]->Fill(0);
               if (flavour.isC & !flavour.isB) h_primvtx[3]->Fill(0);
               if (flavour.isB) h_primvtx[4]->Fill(0);
            } 
            if((**jet_container_).primaryVertex.at(reco_jet.chConstituents.at(0).vtx_id).vtxtype == GlobalVertex::VTXTYPE::SVTX){
               h_primvtx[2]->Fill(1);
               if (flavour.isC & !flavour.isB) h_primvtx[3]->Fill(1);
               if (flavour.isB) h_primvtx[4]->Fill(1);
            } 
            if((**jet_container_).primaryVertex.at(reco_jet.chConstituents.at(0).vtx_id).vtxtype == GlobalVertex::VTXTYPE::MBD){
               h_primvtx[2]->Fill(2);
               if (flavour.isC & !flavour.isB) h_primvtx[3]->Fill(2);
               if (flavour.isB) h_primvtx[4]->Fill(2);
            } */

            h_jet_area->Fill(truth_jet.area, reco_jet.area);

            //for(auto con : reco_jet.chConstituents) h_pflow[0]->Fill(static_cast<int>(con.pflowtype));
            //for(auto con : reco_jet.neConstituents) h_pflow[0]->Fill(static_cast<int>(con.pflowtype));
//std::cout<<"E"<<std::endl;
            //reset output container
            //matched_jets_out.Reset();


            
            vertex_x = (**jet_container_).primaryVertex.x;
            vertex_y = (**jet_container_).primaryVertex.y;
            vertex_z = (**jet_container_).primaryVertex.z;
            vertex_x_unc = (**jet_container_).primaryVertex.x_unc;
            vertex_y_unc = (**jet_container_).primaryVertex.y_unc;
            vertex_z_unc = (**jet_container_).primaryVertex.z_unc;
            vertex_chisq = (**jet_container_).primaryVertex.chisq;
            vertex_ndf = (**jet_container_).primaryVertex.ndf;
            id = iid;
            iid++;

            reco_area = reco_jet.area;
            reco_num_ChConstituents = reco_jet.num_ChConstituents;
            reco_px = reco_jet.px; 
            reco_py = reco_jet.py; 
            reco_pz = reco_jet.pz; 
            reco_pt = reco_jet.pt; 
            reco_eta = reco_jet.eta; 
            reco_phi = reco_jet.phi; 
            reco_m = reco_jet.m; 
            reco_e = reco_jet.e;
            truth_area = truth_jet.area;
            truth_num_ChConstituents = truth_jet.num_ChConstituents;
            truth_px = truth_jet.px; 
            truth_py = truth_jet.py; 
            truth_pz = truth_jet.pz; 
            truth_pt = truth_jet.pt; 
            truth_eta = truth_jet.eta; 
            truth_phi = truth_jet.phi; 
            truth_m = truth_jet.m; 
            truth_e = truth_jet.e; 

            Flavour fl = getFlavour(truth_jet);
            if(fl.isB) truth_flavour = 2;
            else if(fl.isC) truth_flavour = 1;
            else truth_flavour = 0;

            track_e.clear();
               track_eta.clear();
               track_phi.clear();
               track_px.clear();
               track_py.clear();
               track_pz.clear();
               track_pt.clear();
               track_charge.clear();
               track_DCA_xy.clear();
               track_DCA_xy_unc.clear();
               track_sDCA_xy.clear();
               track_DCA3d.clear();
               track_sDCA3d.clear();
               track_n_mvtx.clear();
               track_n_intt.clear();
               track_n_tpc.clear();
               track_chisq.clear();
               track_ndf.clear();

                     

            std::vector<double> sDCA_cut;
            
            for( auto chtrk : reco_jet.chConstituents){
               h_dca_xy->Fill(chtrk.pt,chtrk.DCA_xy);
               h_dca_xy_unc->Fill(chtrk.pt,chtrk.DCA_xy_unc);
               h_nmvtx->Fill(chtrk.pt,chtrk.n_mvtx);
               h_nintt->Fill(chtrk.pt,chtrk.n_intt);
               h_ntpc->Fill(chtrk.pt,chtrk.n_tpc);
               h_chi2ndof->Fill(chtrk.pt,chtrk.chisq/chtrk.ndf);


               track_e.push_back(chtrk.e);
               track_eta.push_back(chtrk.eta);
               track_phi.push_back(chtrk.phi);
               track_px.push_back(chtrk.pt * TMath::Cos(chtrk.phi));
               track_py.push_back(chtrk.pt * TMath::Sin(chtrk.phi));
               track_pz.push_back(chtrk.pt * TMath::SinH(chtrk.eta));
               track_pt.push_back(chtrk.pt);
               track_charge.push_back(chtrk.charge);
               track_DCA_xy.push_back(chtrk.DCA_xy);
               track_DCA_xy_unc.push_back(chtrk.DCA_xy_unc);
               track_sDCA_xy.push_back(chtrk.sDCA_xy);
               track_DCA3d.push_back(chtrk.DCA3d);
               track_sDCA3d.push_back(chtrk.sDCA3d);
               track_n_mvtx.push_back(chtrk.n_mvtx);
               track_n_intt.push_back(chtrk.n_intt);
               track_n_tpc.push_back(chtrk.n_tpc);
               track_chisq.push_back(chtrk.chisq);
               track_ndf.push_back(chtrk.ndf);

               if(abs(chtrk.sDCA3d) < 40){
                  //track quality parameters
                  if (chtrk.n_mvtx < 3) continue;
                  if (chtrk.n_intt < 2) continue;
                  if (chtrk.n_tpc < 40) continue;
                  if (chtrk.chisq/chtrk.ndf > 3.0) continue;
                  if (chtrk.pt < 0.5) continue;
                  if (chtrk.DCA_xy_unc > 3.0) continue;

                  sDCA_cut.push_back(chtrk.sDCA3d);  
                  //matched_jets_out.reco_constituents_Sdxy.push_back(chtrk.sDCA3d);
                  if (flavour.isLF) h_sDCA[0][0]->Fill(chtrk.sDCA3d);
                  if (flavour.isC & !flavour.isB) h_sDCA[0][1]->Fill(chtrk.sDCA3d);
                  if (flavour.isB) h_sDCA[0][2]->Fill(chtrk.sDCA3d);         
               }  
            }

            reco_sdxy_1N = -999;
            reco_sdxy_2N = -999;
            reco_sdxy_3N = -999;

            //sort from greater to lowest
            std::nth_element(sDCA_cut.begin(), sDCA_cut.begin()+1, sDCA_cut.end(), std::greater<double>());

            //most displaced track
            if(sDCA_cut.size() >= 1){
               //matched_jets_out.reco_jet_Sdxy_1N = *(sDCA_cut.begin());
               reco_sdxy_1N = *(sDCA_cut.begin());
               if (flavour.isLF) h_sDCA[1][0]->Fill(*(sDCA_cut.begin()));
               if (flavour.isC & !flavour.isB) h_sDCA[1][1]->Fill(*(sDCA_cut.begin()));
               if (flavour.isB) h_sDCA[1][2]->Fill(*(sDCA_cut.begin()));   
            } 
            //secnd most displaced track
            if(sDCA_cut.size() >= 2){
               reco_sdxy_2N = *(sDCA_cut.begin()+1);
               //matched_jets_out.reco_jet_Sdxy_2N = *(sDCA_cut.begin()+1);
               if (flavour.isLF) h_sDCA[1][0]->Fill(*(sDCA_cut.begin()+1));
               if (flavour.isC & !flavour.isB) h_sDCA[1][1]->Fill(*(sDCA_cut.begin()+1));
               if (flavour.isB) h_sDCA[1][2]->Fill(*(sDCA_cut.begin()+1));   
            } 
            //third most displaced track
            if(sDCA_cut.size() >= 3){
               reco_sdxy_3N = *(sDCA_cut.begin()+2);
               //matched_jets_out.reco_jet_Sdxy_3N = *(sDCA_cut.begin()+2);
               if (flavour.isLF) h_sDCA[1][0]->Fill(*(sDCA_cut.begin()+2));
               if (flavour.isC & !flavour.isB) h_sDCA[1][1]->Fill(*(sDCA_cut.begin()+2));
               if (flavour.isB) h_sDCA[1][2]->Fill(*(sDCA_cut.begin()+2));   
            } 
            //fourth most dispalced track
            if(sDCA_cut.size() >= 4){
               //matched_jets_out.reco_jet_Sdxy_4N = *(sDCA_cut.begin()+3);
               if (flavour.isLF) h_sDCA[1][0]->Fill(*(sDCA_cut.begin()));
               if (flavour.isC & !flavour.isB) h_sDCA[1][1]->Fill(*(sDCA_cut.begin()+3));
               if (flavour.isB) h_sDCA[1][2]->Fill(*(sDCA_cut.begin()+3));   
            } 

           /* matched_jets_out.reco_pt = reco_jet.pt;
            matched_jets_out.reco_jet_nChConstituents = reco_jet.num_ChConstituents;
            matched_jets_out.reco_jet_nConstituents = reco_jet.num_Constituents;
            matched_jets_out.truth_jet_flavour = flavour;
            matched_jets_out.truth_pt = truth_jet.pt;
            matched_jets_out.truth_jet_nChConstituents = truth_jet.num_ChConstituents;
            matched_jets_out.truth_jet_nConstituents = truth_jet.num_Constituents;*/

            //ttree_out->Fill();
            tree->Fill();
         }
      }
   } // TTree entry / event loop

  /* std::string makeDirectory = "mkdir -p " + work_dir_ + "QA_histo";
   system(makeDirectory.c_str());
   TString qapath = work_dir_ + "QA_histo/";

   h_reco_all->SaveAs(qapath + h_reco_all->GetName()+".png");
   h_truth_all->SaveAs(qapath + h_truth_all->GetName()+".png");
   h_reco_unmatch->SaveAs(qapath + h_reco_unmatch->GetName()+".png");
   h_reco_match->SaveAs(qapath + h_reco_match->GetName()+".png");
   h_truth_match->SaveAs(qapath + h_truth_match->GetName()+".png");
   h_phi_all->SaveAs(qapath + h_phi_all->GetName()+".png");
   h_phi_match->SaveAs(qapath + h_phi_match->GetName()+".png");
   h_eta_all->SaveAs(qapath + h_eta_all->GetName()+".png");
   h_eta_match->SaveAs(qapath + h_eta_match->GetName()+".png");
   h_phi_truth_all->SaveAs(qapath + h_phi_truth_all->GetName()+".png");
   h_phi_truth_match->SaveAs(qapath + h_phi_truth_match->GetName()+".png");
   h_eta_truth_all->SaveAs(qapath + h_eta_truth_all->GetName()+".png");
   h_eta_truth_match->SaveAs(qapath + h_eta_truth_match->GetName()+".png");
   h_dca_xy->SaveAs(qapath + h_dca_xy->GetName()+".png");
   h_dca_xy_unc->SaveAs(qapath + h_dca_xy_unc->GetName()+".png");
   h_nmvtx->SaveAs(qapath + h_nmvtx->GetName()+".png");
   h_nintt->SaveAs(qapath + h_nintt->GetName()+".png");
   h_ntpc->SaveAs(qapath + h_ntpc->GetName()+".png");
   h_chi2ndof->SaveAs(qapath + h_chi2ndof->GetName()+".png");
   for(int i = 0; i<num;i++){ 
      for(int j = 0; j<3;j++){ 
         h_sDCA[i][j]->SaveAs(qapath + h_sDCA[i][j]->GetName()+".png");
      }
   }
   for(int i = 0; i<4;i++){ 
      h_matching[i]->SaveAs(qapath + h_matching[i]->GetName()+".png");
   }
   for(int i = 0; i<4;i++){ 
      h_pflow[i]->SaveAs(qapath + h_pflow[i]->GetName()+".png");
   }
   for(int i = 0; i<6;i++){ 
      h_primvtx[i]->SaveAs(qapath + h_primvtx[i]->GetName()+".png");
   }
   h_n_vtx->SaveAs(qapath + h_n_vtx->GetName()+".png");
   h_n_vtx_jet->SaveAs(qapath + h_n_vtx_jet->GetName()+".png");
   h_jet_area->SaveAs(qapath + h_jet_area->GetName()+".png");*/


   TDirectory *qadir = file->mkdir("QA_histo");
   qadir->cd();
   h_reco_all->Write();
   h_truth_all->Write();
   h_reco_unmatch->Write();
   h_reco_match->Write();
   h_truth_match->Write();
   h_phi_all->Write();
   h_phi_match->Write();
   h_eta_all->Write();
   h_eta_match->Write();
   h_phi_truth_all->Write();
   h_phi_truth_match->Write();
   h_eta_truth_all->Write();
   h_eta_truth_match->Write();
   h_phi_track->Write();
   h_eta_track->Write();
   h_phi_track_match->Write();
   h_eta_track_match->Write();
   h_phi_pt_track->Write();
   h_dca_xy->Write();
   h_dca_xy_unc->Write();
   h_nmvtx->Write();
   h_nintt->Write();
   h_ntpc->Write();
   h_chi2ndof->Write();
   for(int i = 0; i<num;i++){ 
      for(int j = 0; j<3;j++){ 
         h_sDCA[i][j]->Write();
      }
   }
   for(int i = 0; i<4;i++){ 
      h_matching[i]->Write();
   }
   for(int i = 0; i<4;i++){ 
      h_pflow[i]->Write();
   }
   for(int i = 0; i<6;i++){ 
      h_primvtx[i]->Write();
   }
   h_n_vtx->Write();
   h_n_vtx_jet->Write();
   h_jet_area->Write();

   file->cd();
   tree->Write("Data", TObject::kOverwrite);
   file->Close();
   return true;
}

JetAnalysis::Flavour JetAnalysis::getFlavour(FullJetFinder::TruthJets truth_jet){
   Flavour flavour = {false, false, false};
   if(!(truth_jet.constituents_PDG_ID.empty())){
      for(auto iPDGid : truth_jet.constituents_PDG_ID){
         if(TString((TDatabasePDG::Instance()->GetParticle(iPDGid))->ParticleClass()) == "CharmedMeson" || TString((TDatabasePDG::Instance()->GetParticle(iPDGid))->ParticleClass()) == "CharmedBaryon"){
            flavour.isC = true;    
         }
         else if(TString((TDatabasePDG::Instance()->GetParticle(iPDGid))->ParticleClass()) == "B-Meson" || TString((TDatabasePDG::Instance()->GetParticle(iPDGid))->ParticleClass()) == "B-Baryon"){
            flavour.isB = true;
         }
      }
   }
   else
      flavour.isLF = true;

   return flavour;
}

std::set<int> JetAnalysis::findDuplicates(std::vector<int> vec){
   std::set<int> duplicates;
   std::sort(vec.begin(), vec.end());
   std::set<int> distinct(vec.begin(), vec.end());
   std::set_difference(vec.begin(), vec.end(), distinct.begin(), distinct.end(),
   std::inserter(duplicates, duplicates.end()));
   return duplicates;
}

void JetAnalysis::printProgress(int cur, int total){  
   if((cur % (total/100))==0){
      float frac = float(cur)/float(total) + 0.01;
      int barWidth = 70;
      std::cout << "[";
      int pos = barWidth * frac;
      for (int i = 0; i < barWidth; ++i) {
         if (i < pos) std::cout << "=";
         else if (i == pos) std::cout << ">";
         else std::cout << " ";
      }
      std::cout << "] " << int(frac * 100.0) << " %\r";
      std::cout.flush();
      if(cur >= total*0.99) std::cout<<std::endl;
   }
}

/*

bool JetAnalysis::scanPurityEfficiency() {
   //load TTree produced by processing processCondor
   TString filename = work_dir_ + "outtree_v2.root";
   TFile *f = new TFile(filename,"READ");
   if(!f){
      std::cout<<filename<<" does not exist. Run processCondor function first or verify path"<<std::endl;
      return false;
   }
   TTreeReader reader("Data", f);
   TTreeReaderValue<JetAnalysis::MatchedJetContainer> *jet_container3 = new TTreeReaderValue<JetAnalysis::MatchedJetContainer>(reader,"matched_jets");

   const int n_points = 250;
   //algo power: 1st, 2nd 3rd ; flavour: 0 -b, 1- c, 2- lf, 3- inclusive; number of points in graphs
   TH1D *purity[3][4][n_points];
   TH1D *efficiency[3][4][n_points];
   TH1D *efficiency_flavour[4]; //flavour: 0 -b, 1- c, 2- lf, 3- inclusive
   double bins[]{10,120};
   //TH2D *RM_all = new TH2D("RM_all","RM_all",200,10,110,200,5,105);
   //TH2D *RM_b = new TH2D("RM_b","RM_b",40,10,50,45,5,50);
   for (int ipower = 0; ipower < 3; ipower++){
      for (int iflavour = 0; iflavour < 4; iflavour++){
         for (int ipoint = 0; ipoint < n_points; ipoint++){
            TString name = "purity_power"+std::to_string(ipower)+"_flavor_"+std::to_string(iflavour)+"_sdxy_"+std::to_string(ipoint);
            purity[ipower][iflavour][ipoint] = new TH1D(name,name,1,bins);
         }
      }
   }
   for (int iflavour = 0; iflavour < 4; iflavour++){
      TString name = "efficiency_flavour_"+std::to_string(iflavour);
      efficiency_flavour[iflavour] = new TH1D(name,name,1,bins);   
   }

  //RM_all->GetXaxis()->SetTitle("pT truth");
  //RM_all->GetYaxis()->SetTitle("pT reco");
  //RM_b->GetXaxis()->SetTitle("pT truth");
  //RM_b->GetYaxis()->SetTitle("pT reco");

  


   //TH1D *resolution[10];
   //for (int k = 0; k < 1 ; k++){
   //   std::string num_text = std::to_string(bins[k]);
   //   std::string rounded = num_text.substr(0, num_text.find(".")+2);
   //   std::string num_text2 = std::to_string(bins[k+1]);
   //   std::string rounded2 = num_text2.substr(0, num_text2.find(".")+2);
   //   TString name = "resolution_pt_truth_"+rounded+"_"+rounded2;
   //   resolution[k] = new TH1D(name,name,40,-1,1);
   //   resolution[k]->GetXaxis()->SetTitle("dPT = (reco-truth)/truth");
   //   resolution[k]->GetYaxis()->SetTitle("probability");
   // }
  
      //  float tag[4]{0,2.5,5,10};


   TCanvas *canvas;
    TPad **Pad;
    TH1D **placeholder;
  UInt_t nbins = 80+2;
  Double_t axis[81+2];
  for (int i = 1; i < 82 ; i++){
     axis[i] = -40 + i;
  }
  axis[0] = -45;
  axis[82] = 45;

    UInt_t bins2 = 80;
  Double_t axis2[81];
  for (int i = 0; i < 81 ; i++){
     axis2[i] = -40 + i;
  }

  TH1D *Signi[3][4];

  for (int i = 0; i < 3 ; i++){
    for (int j = 0; j < 4 ; j++){
      TString name = "flavour"+std::to_string(i)+"_histo_"+std::to_string(j);
      std::cout<<"creating "<<name<<std::endl; 
      Signi[i][j] = new TH1D(name,name,bins2,axis2);
  }
  }

  int c_count = 0;
  int b_count = 0;
  int lf_count = 0;
  int cb_count = 0;


   while (reader.Next()) {
      if (!CheckValue(*jet_container3)) return false;
      auto jets = (**jet_container3);

      for (int ipoint = 0; ipoint < n_points ; ipoint++){
         if(jets.reco_jet_Sdxy_1N > 0.1*ipoint){
            if(jets.truth_jet_flavour.isB) purity[0][0][ipoint]->Fill(jets.reco_pt);
            if(jets.truth_jet_flavour.isC && !jets.truth_jet_flavour.isB) purity[0][1][ipoint]->Fill(jets.reco_pt);
            if(jets.truth_jet_flavour.isLF) purity[0][2][ipoint]->Fill(jets.reco_pt);
            purity[0][3][ipoint]->Fill(jets.reco_pt);
         }
         if(jets.reco_jet_Sdxy_2N > 0.1*ipoint){
            if(jets.truth_jet_flavour.isB) purity[1][0][ipoint]->Fill(jets.reco_pt);
            if(jets.truth_jet_flavour.isC && !jets.truth_jet_flavour.isB) purity[1][1][ipoint]->Fill(jets.reco_pt);
            if(jets.truth_jet_flavour.isLF) purity[1][2][ipoint]->Fill(jets.reco_pt);
            purity[1][3][ipoint]->Fill(jets.reco_pt);
            //if(*m_truth_jet_flavour == 2 && *m_reco_jet_Sdxy_1N > tag[1]){
            //RM_b->Fill(*m_truth_jet_pt,*m_reco_jet_pt);
            //for (int k = 0; k < 9 ; k++){
            //if(*m_reco_jet_pt > 5){
            //if(bins[k] < *m_truth_jet_pt && *m_truth_jet_pt < bins[k+1])
            //resolution[k]->Fill(((*m_reco_jet_pt)-(*m_truth_jet_pt))/(*m_truth_jet_pt));
            //}
           // }

            //}
         }
         if(jets.reco_jet_Sdxy_3N > 0.1*ipoint){
            if(jets.truth_jet_flavour.isB) purity[2][0][ipoint]->Fill(jets.reco_pt);
            if(jets.truth_jet_flavour.isC && !jets.truth_jet_flavour.isB) purity[2][1][ipoint]->Fill(jets.reco_pt);
            if(jets.truth_jet_flavour.isLF) purity[2][2][ipoint]->Fill(jets.reco_pt);
            purity[2][3][ipoint]->Fill(jets.reco_pt);
         }
      }

      if(jets.truth_jet_flavour.isB) efficiency_flavour[0]->Fill(jets.reco_pt);
      if(jets.truth_jet_flavour.isC && !jets.truth_jet_flavour.isB) efficiency_flavour[1]->Fill(jets.reco_pt);
      if(jets.truth_jet_flavour.isLF) efficiency_flavour[2]->Fill(jets.reco_pt);
      efficiency_flavour[3]->Fill(jets.reco_pt);
      //RM_all->Fill(jets.truth_pt,jets.reco_pt);

      if(jets.truth_jet_flavour.isLF) lf_count++;
      if(jets.truth_jet_flavour.isB) b_count++;
      if(jets.truth_jet_flavour.isC) c_count++;
      if(jets.truth_jet_flavour.isC && jets.truth_jet_flavour.isB) cb_count++;


      //lf
      if(jets.truth_jet_flavour.isLF){
        for( auto elem : jets.reco_constituents_Sdxy)Signi[0][0]->Fill(elem);
        Signi[0][1]->Fill(jets.reco_jet_Sdxy_1N);
        Signi[0][2]->Fill(jets.reco_jet_Sdxy_2N);
        Signi[0][3]->Fill(jets.reco_jet_Sdxy_3N);
      }
      if(jets.truth_jet_flavour.isC && !jets.truth_jet_flavour.isB){
        for( auto elem : jets.reco_constituents_Sdxy)Signi[1][0]->Fill(elem);
        Signi[1][1]->Fill(jets.reco_jet_Sdxy_1N);
        Signi[1][2]->Fill(jets.reco_jet_Sdxy_2N);
        Signi[1][3]->Fill(jets.reco_jet_Sdxy_3N);
      }
      if(jets.truth_jet_flavour.isB){
        for( auto elem : jets.reco_constituents_Sdxy)Signi[2][0]->Fill(elem);
        Signi[2][1]->Fill(jets.reco_jet_Sdxy_1N);
        Signi[2][2]->Fill(jets.reco_jet_Sdxy_2N);
        Signi[2][3]->Fill(jets.reco_jet_Sdxy_3N);
      }


   } // end event loop while (reader.Next()) {

      std::cout<<" C: "<<c_count<<" B: "<<b_count<<" LF: "<<lf_count<<" CB: "<<cb_count<<std::endl;

   
   for (int ipower = 0; ipower < 3; ipower++){
      for (int iflavour = 0; iflavour < 3; iflavour++){
         for (int ipoint = 0; ipoint < n_points; ipoint++){
            TString name = "eff_power"+std::to_string(ipower)+"_flavor_"+std::to_string(iflavour)+"_sdxy_"+std::to_string(ipoint);
            efficiency[ipower][iflavour][ipoint] = (TH1D*)(purity[ipower][iflavour][ipoint]->Clone(name));
            //if(!efficiency[ipower][iflavour][ipoint])std::cout<<name<<std::endl;
            efficiency[ipower][iflavour][ipoint]->Divide(efficiency[ipower][iflavour][ipoint],efficiency_flavour[iflavour],1,1,"B");
            purity[ipower][iflavour][ipoint]->Divide(purity[ipower][iflavour][ipoint],purity[ipower][3][ipoint],1.,1.,"B");
         }
      }
   }

   for (int ipower = 0; ipower < 3; ipower++){
      for (int iflavour = 0; iflavour < 3; iflavour++){
         for (int ipoint = 0; ipoint < n_points; ipoint++){
            if(ipower==0){
               purity[ipower][iflavour][ipoint]->SetMarkerColor(kBlue);
               purity[ipower][iflavour][ipoint]->SetLineColor(kBlue);
               efficiency[ipower][iflavour][ipoint]->SetMarkerColor(kBlue);
               efficiency[ipower][iflavour][ipoint]->SetLineColor(kBlue);
            }
            if(ipower==1){
               purity[ipower][iflavour][ipoint]->SetMarkerColor(kGreen);
               purity[ipower][iflavour][ipoint]->SetLineColor(kGreen);
               efficiency[ipower][iflavour][ipoint]->SetMarkerColor(kGreen);
               efficiency[ipower][iflavour][ipoint]->SetLineColor(kGreen);
            }
            if(ipower==2){
               purity[ipower][iflavour][ipoint]->SetMarkerColor(kRed);
               purity[ipower][iflavour][ipoint]->SetLineColor(kRed);
               efficiency[ipower][iflavour][ipoint]->SetMarkerColor(kRed);
               efficiency[ipower][iflavour][ipoint]->SetLineColor(kRed);
            }   
         }
      }
   }



   double eff_1_b[n_points], pur_1_b[n_points];
   double eff_2_b[n_points], pur_2_b[n_points];
   double eff_3_b[n_points], pur_3_b[n_points];
   double eff_1_c[n_points], pur_1_c[n_points];
   double eff_2_c[n_points], pur_2_c[n_points];
   double eff_3_c[n_points], pur_3_c[n_points];
   double eff_1_lf[n_points], eff_2_lf[n_points], eff_3_lf[n_points];

   for (int ipoint = 0; ipoint < n_points ; ipoint++){
      eff_1_b[ipoint] = efficiency[0][0][ipoint]->GetBinContent(1);
      eff_2_b[ipoint] = efficiency[1][0][ipoint]->GetBinContent(1);
      eff_3_b[ipoint] = efficiency[2][0][ipoint]->GetBinContent(1);
      eff_1_c[ipoint] = efficiency[0][1][ipoint]->GetBinContent(1);
      eff_2_c[ipoint] = efficiency[1][1][ipoint]->GetBinContent(1);
      eff_3_c[ipoint] = efficiency[2][1][ipoint]->GetBinContent(1);
      eff_1_lf[ipoint] = efficiency[0][2][ipoint]->GetBinContent(1);
      eff_2_lf[ipoint] = efficiency[1][2][ipoint]->GetBinContent(1);
      eff_3_lf[ipoint] = efficiency[2][2][ipoint]->GetBinContent(1);

      pur_1_b[ipoint] = purity[0][0][ipoint]->GetBinContent(1);
      pur_2_b[ipoint] = purity[1][0][ipoint]->GetBinContent(1);
      pur_3_b[ipoint] = purity[2][0][ipoint]->GetBinContent(1);
      pur_1_c[ipoint] = purity[0][1][ipoint]->GetBinContent(1);
      pur_2_c[ipoint] = purity[1][1][ipoint]->GetBinContent(1);
      pur_3_c[ipoint] = purity[2][1][ipoint]->GetBinContent(1);
   }

   auto g_1_eff_b_pur_b = new TGraph(n_points,eff_1_b,pur_1_b);
   g_1_eff_b_pur_b->SetTitle("Beauty Eff vs Purity;eff_b;pur_b");
   g_1_eff_b_pur_b->SetMarkerColor(kGreen);
   g_1_eff_b_pur_b->SetLineColor(kGreen);
   auto g_2_eff_b_pur_b = new TGraph(n_points,eff_2_b,pur_2_b);
   g_2_eff_b_pur_b->SetTitle("Eff vs Purity;eff_b;pur_b");
   g_2_eff_b_pur_b->SetMarkerColor(kRed);
   g_2_eff_b_pur_b->SetLineColor(kRed);
   auto g_3_eff_b_pur_b = new TGraph(n_points,eff_3_b,pur_3_b);
   g_3_eff_b_pur_b->SetTitle("Eff vs Purity;eff_b;pur_b");
   g_3_eff_b_pur_b->SetMarkerColor(kBlue);
   g_3_eff_b_pur_b->SetLineColor(kBlue);

   auto g_1_eff_c_pur_c = new TGraph(n_points,eff_1_c,pur_1_c);
   g_1_eff_c_pur_c->SetTitle("Charm Eff vs Purity;eff_c;pur_c");
   g_1_eff_c_pur_c->SetMarkerColor(kGreen);
   g_1_eff_c_pur_c->SetLineColor(kGreen);
   auto g_2_eff_c_pur_c = new TGraph(n_points,eff_2_c,pur_2_c);
   g_2_eff_c_pur_c->SetTitle("Eff vs Purity;eff_c;pur_c");
   g_2_eff_c_pur_c->SetMarkerColor(kRed);
   g_2_eff_c_pur_c->SetLineColor(kRed);
   auto g_3_eff_c_pur_c = new TGraph(n_points,eff_3_c,pur_3_c);
   g_3_eff_c_pur_c->SetTitle("Eff vs Purity;eff_c;pur_c");
   g_3_eff_c_pur_c->SetMarkerColor(kBlue);
   g_3_eff_c_pur_c->SetLineColor(kBlue);

   auto g_1_eff_b_eff_c = new TGraph(n_points,eff_1_b,eff_1_c);
   g_1_eff_b_eff_c->SetTitle("Eff charm vs Eff beauty;eff_b;eff_c");
   g_1_eff_b_eff_c->SetMarkerColor(kGreen);
   g_1_eff_b_eff_c->SetLineColor(kGreen);
   auto g_2_eff_b_eff_c = new TGraph(n_points,eff_2_b,eff_2_c);
   g_2_eff_b_eff_c->SetTitle("Eff vs Eff;eff_b;eff_c");
   g_2_eff_b_eff_c->SetMarkerColor(kRed);
   g_2_eff_b_eff_c->SetLineColor(kRed);
   auto g_3_eff_b_eff_c = new TGraph(n_points,eff_3_b,eff_3_c);
   g_3_eff_b_eff_c->SetTitle("Eff vs Eff;eff_b;eff_c");
   g_3_eff_b_eff_c->SetMarkerColor(kBlue);
   g_3_eff_b_eff_c->SetLineColor(kBlue);

   auto g_1_eff_b_eff_lf = new TGraph(n_points,eff_1_b,eff_1_lf);
   g_1_eff_b_eff_lf->SetTitle("Eff LF vs Eff beauty;eff_b;eff_lf");
   g_1_eff_b_eff_lf->SetMarkerColor(kGreen);
   g_1_eff_b_eff_lf->SetLineColor(kGreen);
   auto g_2_eff_b_eff_lf = new TGraph(n_points,eff_2_b,eff_2_lf);
   g_2_eff_b_eff_lf->SetTitle("Eff vs Eff;eff_b;eff_lf");
   g_2_eff_b_eff_lf->SetMarkerColor(kRed);
   g_2_eff_b_eff_lf->SetLineColor(kRed);
   auto g_3_eff_b_eff_lf = new TGraph(n_points,eff_3_b,eff_3_lf);
   g_3_eff_b_eff_lf->SetTitle("Eff vs Eff;eff_b;eff_lf");
   g_3_eff_b_eff_lf->SetMarkerColor(kBlue);
   g_3_eff_b_eff_lf->SetLineColor(kBlue);

   TLegend *leg = new TLegend(0.65,0.35,0.9,0.55);
   leg->SetHeader("sDCA_xy: 0-25 (0.1 step)","C");
   leg->AddEntry(g_1_eff_b_pur_b,"1st most displaced track","lp");
   leg->AddEntry(g_2_eff_b_pur_b,"2nd most displaced track","lp");
   leg->AddEntry(g_3_eff_b_pur_b,"3rd most displaced track","lp");



   //for (int k = 0; k < 9 ; k++){
   //   resolution[k]->Scale(1./resolution[k]->Integral());
   //   resolution[k]->Scale(1.,"width");
   //}

   //   TCanvas *c5 = new TCanvas("c5","c5",2000,1200);
   //c5->Divide(6,2);
   //c5->cd(1);
   //c5->cd(1)->SetLogz();
   //RM_all->Draw("colz");
   //c5->cd(2);
   //c5->cd(2)->SetLogz();
   //RM_b->Draw("colz");
   //for (int k = 0; k < 9 ; k++){
   //   c5->cd(k+3);
   //   c5->cd(k+3)->SetLogy();
   //   resolution[k]->Draw();
   //}

      //   TCanvas *c5 = new TCanvas("c5","c5",2000,1200);
   //c5->Divide(6,2);
  // c5->cd(1);
  // c5->cd(1)->SetLogz();
 //  RM_all->Draw("colz");
      
std::cout<<"I AM HERE"<<std::endl;


   TCanvas *c_1 = new TCanvas("c_1","c_1",1600,1200);
   c_1->cd();
   g_1_eff_b_pur_b->GetXaxis()->SetRangeUser(0,1);
   g_1_eff_b_pur_b->GetYaxis()->SetRangeUser(0,1);
   g_1_eff_b_pur_b->Draw("AC*");
   g_2_eff_b_pur_b->Draw("C* same");
   g_3_eff_b_pur_b->Draw("C* same");
   leg->Draw();


   TCanvas *c_2 = new TCanvas("c_2","c_2",1600,1200);
   c_2->cd();
   g_1_eff_c_pur_c->GetXaxis()->SetRangeUser(0,1);
   g_1_eff_c_pur_c->GetYaxis()->SetRangeUser(0,1);
   g_1_eff_c_pur_c->Draw("AC*");
   g_2_eff_c_pur_c->Draw("C* same");
   g_3_eff_c_pur_c->Draw("C* same");
   leg->Draw();

   TCanvas *c_3 = new TCanvas("c_3","c_3",1600,1200);
   c_3->cd();
   c_3->SetLogy();
   g_1_eff_b_eff_c->GetXaxis()->SetRangeUser(0,1);
   g_1_eff_b_eff_c->GetYaxis()->SetRangeUser(1E-3,1);
   g_1_eff_b_eff_c->Draw("AC*");
   g_2_eff_b_eff_c->Draw("C* same");
   g_3_eff_b_eff_c->Draw("C* same");
   leg->Draw();

    TCanvas *c_4 = new TCanvas("c_4","c_4",1600,1200);
   c_4->cd();
   c_4->SetLogy();
   g_1_eff_b_eff_lf->GetXaxis()->SetRangeUser(0,1);
   g_1_eff_b_eff_lf->GetYaxis()->SetRangeUser(1E-5,1);
   g_1_eff_b_eff_lf->Draw("AC*");
   g_2_eff_b_eff_lf->Draw("C* same");
   g_3_eff_b_eff_lf->Draw("C* same");
   leg->Draw();


for (int i = 0; i < 3 ; i++){
    for (int j = 0; j < 4 ; j++){
      Signi[i][j]->Scale(1./Signi[i][j]->Integral());
      Signi[i][j]->Scale(1,"width");
      Signi[i][j]->SetLineWidth(1.2);
      Signi[0][j]->SetMarkerColor(kBlue);
      Signi[1][j]->SetMarkerColor(kGreen);
      Signi[2][j]->SetMarkerColor(kRed);
      Signi[0][j]->SetLineColor(kBlue);
      Signi[1][j]->SetLineColor(kGreen);
      Signi[2][j]->SetLineColor(kRed);
    }
  }

  for (int j = 0; j < 4 ; j++){
      Signi[0][j]->SetMarkerStyle(25);
      Signi[1][j]->SetMarkerStyle(20);
      Signi[2][j]->SetMarkerStyle(21);
    }




  std::tie(canvas,Pad,placeholder) =PrepareCanvas4Pad(nbins,axis);

    std::cout<<"A"<<std::endl;

  TLegend *leg6 = new TLegend(.02,.7,.95,.95);
  leg6->SetFillStyle(0);
  leg6->AddEntry("","#it{#bf{sPHENIX}} Internal","");
  leg6->AddEntry("","PYTHIA 8 + GEANT 4","");
  leg6->AddEntry("","p+p #sqrt{s} = 200 GeV with no pileup","");
  Pad[0]->cd();
  leg6->Draw("same");
  Signi[0][0]->Draw("same");
  Signi[1][0]->Draw("same");
  Signi[2][0]->Draw("same");

  std::cout<<"B"<<std::endl;

  TLegend *leg2 = new TLegend(-0.1,.6,.95,.95);
  leg2->SetFillStyle(0);
  leg2->AddEntry("","Full jets, Anti-#it{k}_{T}","");
  leg2->AddEntry("","#it{R}=0.4, |#it{#eta}_{jet}| < 0.7","");
  leg2->AddEntry("","#it{p}_{T,jet}^{rec} > 10 GeV/#it{c}","");
  leg2->AddEntry("","#it{p}_{T,jet}^{truth} > 30 GeV/#it{c}","");
  Pad[1]->cd();
  leg2->Draw("same");
  Signi[0][1]->Draw("same");
  Signi[1][1]->Draw("same");
  Signi[2][1]->Draw("same");
  TLegend *leg4 = new TLegend(.57,.6,.97,.95);
  leg4->SetFillStyle(0);
  leg4->AddEntry(Signi[2][1],"b-jet","p");
   leg4->AddEntry(Signi[1][1],"c-jet","p");
  leg4->AddEntry(Signi[0][1],"light-flavour-jet","p");
   leg4->Draw("same");

  std::cout<<"C"<<std::endl;

  TLegend *leg3 = new TLegend(-0.12,.52,.95,.95);
  leg3->SetFillStyle(0);
  leg3->AddEntry("","Track Quality:","");
  leg3->AddEntry("","  #it{p}_{T} > 500 MeV/#it{c}","");
  leg3->AddEntry("","  #it{#chi^{2}}/#it{nDOF} < 3","");
  leg3->AddEntry("","  #it{Clusters}_{MVTX} > 3","");
  leg3->AddEntry("","  #it{Clusters}_{INTT} > 2","");
  leg3->AddEntry("","  #it{Clusters}_{TPC} > 40","");
  Pad[2]->cd();
  leg3->Draw("same");
  Signi[0][2]->Draw("same");
  Signi[1][2]->Draw("same");
  Signi[2][2]->Draw("same");

std::cout<<"D"<<std::endl;

 
  

  Pad[3]->cd();
  Signi[0][3]->Draw("same");
  Signi[1][3]->Draw("same");
  Signi[2][3]->Draw("same");
std::cout<<"E"<<std::endl;
  
  //TCanvas *c = new TCanvas("c", "c", 685, 630);
  //gPad->SetLogz();
  //gPad->SetRightMargin(0.15);

  //TLegend *leg = new TLegend(.12,.78,.4,.9);
  //leg->SetFillStyle(0);
  //leg->AddEntry("","#it{#bf{sPHENIX}} Preliminary","");
  //leg->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");

  //TH2D *h = new TH2D("", "", 100, 0, 1, 100, 0, 1);
  //h->GetYaxis()->SetNdivisions(405);
  //h->GetXaxis()->SetNdivisions(405);
  //h->SetXTitle("x-axis title [arb. units]");
  //h->SetYTitle("y-axis title [arb. units]");
  //h->Draw("col z");
  //leg->Draw("same");
  //TLatex l;
  //l.SetNDC();
  //l.SetTextFont(43);
  //l.SetTextSize(25);
  //l.DrawLatex(0.7, 0.965, "#it{7/21/2023}");

  canvas->Print("plot1.pdf");
  canvas->Print("plot1.ps");
  canvas->Print("plot1.png");
   //cc->Divide(10,10);

      //   for (int k = 0; k < 4 ; k++){
       //     cc->cd(4*i+k+1);
       //     //eff2[i][0][k]->Draw();
       //     //eff2[i][1][k]->Draw();
       //     eff2[i][2][k]->Draw("e0");

       //  }



//   for (int k = 0; k < 100 ; k++){
//      cc->cd(k+1);
//         eff2[0][2][k]->GetYaxis()->SetRangeUser(0,1);
//         std::string num_text = std::to_string(tag[k]);
//         std::string rounded = num_text.substr(0, num_text.find(".")+2);
//TString name = "efficiency, Sdxy > "+ rounded;//std::to_string(tag[k]);
//         eff2[0][2][k]->SetTitle(name);
//   eff2[0][2][k]->GetYaxis()->SetTitle("b-jet efficiency");
//   eff2[0][2][k]->GetXaxis()->SetTitle("pt jet reco");
//      eff2[0][2][k]->Draw("e0");
//      eff2[1][2][k]->Draw("same e0");
//      eff2[2][2][k]->Draw("same e0");
//   cc->cd(k+5);
//   name = "purity, Sdxy > "+ rounded;
//         purity[0][2][k]->SetTitle(name);
//   purity[0][2][k]->GetYaxis()->SetRangeUser(0,0.15);
//   purity[0][2][k]->GetYaxis()->SetTitle("b-jet purity");
//   purity[0][2][k]->GetXaxis()->SetTitle("pt jet reco");
//   purity[0][2][k]->Draw("e0");
//   purity[1][2][k]->Draw("same e0");
//   purity[2][2][k]->Draw("same e0");
  // }

//   cc->cd(1);
//   purity[0][0]->GetYaxis()->SetRangeUser(0,1);
//   purity[0][0]->Draw();
//   purity[0][1]->Draw("same");
//   purity[0][2]->Draw("same");
//     cc->cd(2);
//   purity[1][0]->GetYaxis()->SetRangeUser(0,1);
//   purity[1][0]->Draw();
//   purity[1][1]->Draw("same");
 //  purity[1][2]->Draw("same");
 //    cc->cd(2);
//   purity[2][0]->GetYaxis()->SetRangeUser(0,1);
//   purity[2][0]->Draw();
//   purity[2][1]->Draw("same");
//   purity[2][2]->Draw("same");

//   for (unsigned int ii = 0; ii < 3; ii++){
//     std::cout << ii << std::endl;
//      cc->cd(ii+1);
//      std::cout<<"plotting "<<ii<<" 0"<<std::endl;
//       purity[ii][0]->GetYaxis()->SetRangeUser(0,1);
//      purity[ii][0]->Draw();
//       std::cout<<"plotting "<<ii<<" 1"<<std::endl;
//      purity[ii][1]->Draw("same");
//       std::cout<<"plotting "<<ii<<" 2"<<std::endl;
//      purity[ii][2]->Draw("same");
     

//   }


return true;
}
*/
/*
void JetAnalysis::printHiisto(TH1* h){
   TCanvas c1;
   c1.Print("hsimple.ps[");
   while ((key = (TKey*)keyList())) {
      TClass *cl = gROOT->GetClass(key->GetClassName());
      if (!cl->InheritsFrom("TH1")) continue;
      TH1 *h = (TH1*)key->ReadObj();
      h->Draw();
      c1.Print("hsimple.ps");
   }
   c1.Print("hsimple.ps]");

}
*/


std::tuple<TCanvas*, TPad**, TH1D**> PrepareCanvas4Pad(UInt_t xAxisBins, Double_t *xAxis){
    //SetsPhenixStyle();
    //prepare main canvas
    TCanvas *FinalSpectrum = new TCanvas("FinalSpectrum3", "FinalSpectrum3",0,45,(685+3*630),630);
    //gStyle->SetOptStat(0);
    //gStyle->SetOptTitle(0);
    FinalSpectrum->SetHighLightColor(2);
    FinalSpectrum->Range(0,0,1,1);
    FinalSpectrum->SetFillColor(0);
    FinalSpectrum->SetBorderMode(0);
    FinalSpectrum->SetBorderSize(2);
    FinalSpectrum->SetFrameBorderMode(0);
    TPad **pad = new TPad*[static_cast<ULong_t>(4)];
    //0  1   2   3

    Double_t marginLeft=0.05;
    Double_t marginRight=0.02;
    Double_t innerPadWidth=(1-marginLeft-marginRight)/4.;
    Double_t marginLeftForXAxis=0.02;

    Double_t marginTop=0.04;
    Double_t marginBottom=0.14;
    Double_t innerPadHeight=(1-marginTop-marginBottom)/1.;
    //Double_t marginBottomForYAxis=0.0;

    //pads for the plots
    auto xbaseL = [innerPadWidth,marginLeft,marginLeftForXAxis](int m) ->Double_t {return (m*innerPadWidth+marginLeft-marginLeftForXAxis);};
    auto xbaseR = [innerPadWidth,marginLeft](int m) ->Double_t {return (m*innerPadWidth+marginLeft);};
    //auto ybaseL = [innerPadHeight,marginBottom,marginBottomForYAxis](int m) ->Double_t {return (m*innerPadHeight+marginBottom-marginBottomForYAxis);};
    //auto ybaseR = [innerPadHeight,marginBottom](int m) ->Double_t {return (m*innerPadHeight+marginBottom);};

    //pads for the plots
    for(Int_t ipad = 0;ipad < 4;ipad++){
        std::cout<<"padID: "<<ipad<<std::endl;
        std::cout<<xbaseL(ipad)<<" "<<xbaseR(ipad)<<std::endl;
        if(ipad ==0) pad[ipad] = new TPad("pad0", "pad0",0,         0 ,xbaseR(1) ,1);
        if(ipad ==1) pad[ipad] = new TPad("pad1", "pad1",xbaseL(1), 0 ,xbaseR(2) ,1);
        if(ipad ==2) pad[ipad] = new TPad("pad2", "pad2",xbaseL(2), 0 ,xbaseR(3) ,1);
        if(ipad ==3) pad[ipad] = new TPad("pad3", "pad3",xbaseL(3), 0 ,1         ,1);

        pad[ipad]->Draw();
        pad[ipad]->cd();
        pad[ipad]->SetLogy();

        //left column
         if(ipad ==0){
            pad[ipad]->SetLeftMargin(static_cast<Float_t>(marginLeft/(marginLeft+innerPadWidth)));
            pad[ipad]->SetRightMargin(0);
            pad[ipad]->Modified();
            pad[ipad]->SetFillStyle(0);
        }
        //right column
        else if (ipad ==1 || ipad ==2){
            pad[ipad]->SetLeftMargin(static_cast<Float_t>(marginLeftForXAxis/(innerPadWidth+marginLeftForXAxis)));
            pad[ipad]->SetRightMargin(0);
            pad[ipad]->Modified();
            pad[ipad]->SetFillStyle(0);
        }
        else if (ipad ==3){
            pad[ipad]->SetLeftMargin(static_cast<Float_t>(marginLeftForXAxis/(innerPadWidth+marginLeftForXAxis+marginRight)));
            pad[ipad]->SetRightMargin(static_cast<Float_t>(marginRight/(innerPadWidth+marginRight)));
            pad[ipad]->Modified();
            pad[ipad]->SetFillStyle(0);
        }

        pad[ipad]->SetBottomMargin(marginBottom/(marginBottom+innerPadHeight));
        pad[ipad]->SetTopMargin(marginTop/(2*innerPadHeight+marginTop));

        pad[ipad]->SetFillStyle(0);
        pad[ipad]->Modified();
        pad[ipad]->Update();
        FinalSpectrum->cd();
    }

    //ratio Y-axis title placeholder
    TH1D **placeholder = new TH1D*[static_cast<ULong_t>(4)];


    //other axis placeholders
    for(Int_t ipad = 0;ipad <4;ipad++){
            pad[ipad]->cd();
            placeholder[ipad] = new TH1D(Form("placeholder2%d",ipad),"Central Values",static_cast<Int_t>(xAxisBins), xAxis);
            //set ranges
    
            placeholder[ipad]->SetMinimum(1E-5);
            placeholder[ipad]->SetMaximum(100);

            //if(ipad>3)placeholder[ipad]->GetXaxis()->SetTitle("#it{p}_{T,ch. jet} (GeV/#it{c})");
            //if(ipad<2)placeholder[ipad]->GetYaxis()->SetTitle("#frac{d^{2}#it{#sigma}^{#it{R}=0.2}}{d#it{p}_{T,ch. jet}d#it{#eta}_{ch. jet}}/#frac{d^{2}#it{#sigma}^{#it{R}=#it{X}}}{d#it{p}_{T,ch. jet}d#it{#eta}_{ch. jet}}");
            //else if(ipad==4)placeholder[ipad]->GetYaxis()->SetTitle("MC/Data");

            //TMathText l;
            //l.
            TString tt = "\\mathscr{S} DCA_{xy}";
            //tt + "\\mathscr{S}";
            //tt + "DCA_{xy}";

            if(ipad==0)placeholder[ipad]->GetYaxis()->SetTitle("Probability");
            if(ipad==0)placeholder[ipad]->GetXaxis()->SetTitle("#it{S}DCA_{xy}");
            if(ipad==1)placeholder[ipad]->GetXaxis()->SetTitle("1^{ st} largest #it{S}DCA_{xy}");
            if(ipad==2)placeholder[ipad]->GetXaxis()->SetTitle("2^{nd} largest #it{S}DCA_{xy}");
            if(ipad==3)placeholder[ipad]->GetXaxis()->SetTitle("3^{rd} largest #it{S}DCA_{xy}");

            /*placeholder[ipad]->GetYaxis()->SetTitleFont(43);
            placeholder[ipad]->GetXaxis()->SetTitleFont(43);
            placeholder[ipad]->GetYaxis()->SetLabelFont(43);
            placeholder[ipad]->GetXaxis()->SetLabelFont(43);
            placeholder[ipad]->GetYaxis()->SetTitleSize(0.05);
            placeholder[ipad]->GetXaxis()->SetTitleSize(0.05);
            placeholder[ipad]->GetYaxis()->SetLabelSize(0.05);
            placeholder[ipad]->GetXaxis()->SetLabelSize(0.05);*/

            placeholder[ipad]->GetYaxis()->SetTitleOffset(1.2f);//0.95*(gPad->GetHNDC())/scaleHeightPads/resizeTextFactor);
            placeholder[ipad]->GetXaxis()->SetTitleOffset(1.2f);// not number before, no number was optimized before font 43 was introduced
            if(ipad>0){
            placeholder[ipad]->GetYaxis()->SetTitleSize(0);
            placeholder[ipad]->GetYaxis()->SetLabelSize(0);
            }

            placeholder[ipad]->GetXaxis()->SetNdivisions(405);
            placeholder[ipad]->GetYaxis()->SetNdivisions(405);
            /*else placeholder[ipad]->GetYaxis()->SetDecimals();
            if(ipad>1)placeholder[ipad]->GetYaxis()->SetNdivisions(5, 5, 0, kTRUE);
            //because root is so advanced it decided to put different ticks lenght for 2 particular plots - lets just force it to look same
            if(ipad==4 || ipad==5) placeholder[ipad]->GetYaxis()->SetTickLength(0.04f);
            if(ipad>=2) placeholder[ipad]->GetXaxis()->SetTickLength(0.06);*/

            placeholder[ipad]->Draw();
        }

    return std::make_tuple(FinalSpectrum,pad, placeholder);

}
