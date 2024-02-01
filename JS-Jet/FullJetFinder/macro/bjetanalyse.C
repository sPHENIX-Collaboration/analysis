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

#include "bjetanalyse.h"


R__LOAD_LIBRARY(libJetVertexTagging.so)


bjetanalyse::bjetanalyse(	const std::string &filename = "myjetanalysis.root"):
input_file_name_(filename)
  , m_reco_jet_pt()
  , m_reco_constituents_dxy()
  , m_reco_constituents_Sdxy()
  , m_reco_jet_Sdxy_1N()
  , m_reco_jet_Sdxy_2N()
  , m_reco_jet_Sdxy_3N()
  //, m_reco_jet_Sdxy_4N()
  , m_truth_jet_pt()
  , m_truth_jet_flavour()
{

}

bool bjetanalyse::CheckValue(ROOT::Internal::TTreeReaderValueBase& value) {
   if (value.GetSetupStatus() < 0) {
      std::cerr << "Error " << value.GetSetupStatus()
                << "setting up reader for " << value.GetBranchName() << '\n';
      return false;
   }
   return true;
}

// Analyze the tree "MyTree" in the file passed into the function.
// Returns false in case of errors.
bool bjetanalyse::analyse() {
   // Create a TTreeReader named "MyTree" from the given TDirectory.
   // The TTreeReader gives access to the TTree to the TTreeReaderValue and
   // TTreeReaderArray objects. It knows the current entry number and knows
   // how to iterate through the TTree.
  // std::cout<<"A"<<std::endl;
   TChain *tc = new TChain("AntiKt_r04/Data");
  // std::cout<<"B"<<std::endl;
   tc->Add("/sphenix/tg/tg01/hf/jkvapil/JET30_r8/myTestJets/outputData_0*.root");
   std::cout<<"entries: "<<tc->GetEntries()<<std::endl;
   TTreeReader reader(tc);
  // std::cout<<"D"<<std::endl;
  // TFile *f = new TFile(input_file_name_.data(),"READ");
  // TTreeReader reader("AntiKt_r04", f);

  //TTreeReaderValue<JetVertexTagging::Container> *jet_container_= nullptr;

  jet_container_ = new TTreeReaderValue<JetVertexTagging::Container>(reader,"JetContainer");

  TTreeReaderValue<JetVertexTagging::Container> jet_container_2 (reader,"JetContainer"); 


  
/*
  n_event_ = new TTreeReaderValue<int>(reader,"m_event");
  n_reco_jet_= new TTreeReaderValue<int>(reader,"m_reco_jet_n");
  n_truth_jet_= new TTreeReaderValue<int>(reader,"m_truth_jet_n");

  reco_jet_id_ = new TTreeReaderValue<std::vector<int>>(reader,"m_reco_jet_id");
  reco_jet_nConstituents_ = new TTreeReaderValue<std::vector<int>>(reader,"m_reco_jet_nConstituents");
  reco_jet_nChConstituents_ = new TTreeReaderValue<std::vector<int>>(reader,"m_reco_jet_nChConstituents");
  reco_jet_px_ = new TTreeReaderValue<std::vector<float>>(reader,"m_reco_jet_px");
  reco_jet_py_ = new TTreeReaderValue<std::vector<float>>(reader,"m_reco_jet_py");
  reco_jet_pz_ = new TTreeReaderValue<std::vector<float>>(reader,"m_reco_jet_pz");
  reco_jet_pt_ = new TTreeReaderValue<std::vector<float>>(reader,"m_reco_jet_pt");
  reco_jet_eta_ = new TTreeReaderValue<std::vector<float>>(reader,"m_reco_jet_eta");
  reco_jet_phi_ = new TTreeReaderValue<std::vector<float>>(reader,"m_reco_jet_phi");
  reco_jet_m_ = new TTreeReaderValue<std::vector<float>>(reader,"m_reco_jet_m");
  reco_jet_e_ = new TTreeReaderValue<std::vector<float>>(reader,"m_reco_jet_e");
  reco_constituents_dxy_ = new TTreeReaderValue<std::vector<std::vector<float>>>(reader,"m_reco_constituents_dxy");
  reco_constituents_dxy_unc_= new TTreeReaderValue<std::vector<std::vector<float>>>(reader,"m_reco_constituents_dxy_unc");
  reco_constituents_Sdxy_old_ = new TTreeReaderValue<std::vector<std::vector<float>>>(reader,"m_reco_constituents_Sdxy_old");
  reco_constituents_Sdxy_= new TTreeReaderValue<std::vector<std::vector<float>>>(reader,"m_reco_constituents_Sdxy");
  reco_constituents_dxy_cut_ = new TTreeReaderValue<std::vector<std::vector<float>>>(reader,"m_reco_constituents_dxy_cut");
  reco_constituents_dxy_unc_cut_= new TTreeReaderValue<std::vector<std::vector<float>>>(reader,"m_reco_constituents_dxy_unc_cut");
  reco_constituents_Sdxy_old_cut_ = new TTreeReaderValue<std::vector<std::vector<float>>>(reader,"m_reco_constituents_Sdxy_old_cut");
  reco_constituents_Sdxy_cut_= new TTreeReaderValue<std::vector<std::vector<float>>>(reader,"m_reco_constituents_Sdxy_cut");

  //! truth jets
  truth_jet_id_ = new TTreeReaderValue<std::vector<int>>(reader,"m_truth_jet_id");
  truth_jet_nConstituents_ = new TTreeReaderValue<std::vector<int>>(reader,"m_truth_jet_nConstituents");
  truth_jet_nChConstituents_ = new TTreeReaderValue<std::vector<int>>(reader,"m_truth_jet_nChConstituents");
  truth_jet_px_ = new TTreeReaderValue<std::vector<float>>(reader,"m_truth_jet_px");
  truth_jet_py_ = new TTreeReaderValue<std::vector<float>>(reader,"m_truth_jet_py");
  truth_jet_pz_ = new TTreeReaderValue<std::vector<float>>(reader,"m_truth_jet_pz");
  truth_jet_pt_ = new TTreeReaderValue<std::vector<float>>(reader,"m_truth_jet_pt");
  truth_jet_eta_ = new TTreeReaderValue<std::vector<float>>(reader,"m_truth_jet_eta");
  truth_jet_phi_ = new TTreeReaderValue<std::vector<float>>(reader,"m_truth_jet_phi");
  truth_jet_m_ = new TTreeReaderValue<std::vector<float>>(reader,"m_truth_jet_m");
  truth_jet_e_ = new TTreeReaderValue<std::vector<float>>(reader,"m_truth_jet_e");
  truth_constituents_PDG_ID_ = new TTreeReaderValue<std::vector<std::vector<int>>>(reader,"m_truth_constituents_PDG_ID");*/
/*
     TFile *fo = new TFile("/sphenix/tg/tg01/hf/jkvapil/JET10_new/out2.root","recreate");


  ttree_out_= new TTree("AntiKt_r04_o", "AntiKt_r04_o");
  ttree_out_->Branch("m_reco_jet_pt", &m_reco_jet_pt,"m_reco_jet_pt/F");
  ttree_out_->Branch("m_reco_jet_nConstituents", &m_reco_jet_nConstituents,"m_reco_jet_nConstituents/I");
  ttree_out_->Branch("m_reco_jet_nChConstituents", &m_reco_jet_nChConstituents,"m_reco_jet_nChConstituents/I");  
  ttree_out_->Branch("m_reco_constituents_dxy", &m_reco_constituents_dxy);
  ttree_out_->Branch("m_reco_constituents_dxy_unc", &m_reco_constituents_dxy_unc);
  ttree_out_->Branch("m_reco_constituents_Sdxy_old", &m_reco_constituents_Sdxy_old);
  ttree_out_->Branch("m_reco_constituents_Sdxy", &m_reco_constituents_Sdxy);
  ttree_out_->Branch("m_reco_constituents_dxy_cut", &m_reco_constituents_dxy_cut);
  ttree_out_->Branch("m_reco_constituents_dxy_unc_cut", &m_reco_constituents_dxy_unc_cut);
  ttree_out_->Branch("m_reco_constituents_Sdxy_old_cut", &m_reco_constituents_Sdxy_old_cut);
  ttree_out_->Branch("m_reco_constituents_Sdxy_cut", &m_reco_constituents_Sdxy_cut);
  ttree_out_->Branch("m_reco_jet_Sdxy_1N", &m_reco_jet_Sdxy_1N,"m_reco_jet_Sdxy_1N/F");
  ttree_out_->Branch("m_reco_jet_Sdxy_2N", &m_reco_jet_Sdxy_2N,"m_reco_jet_Sdxy_2N/F");
  ttree_out_->Branch("m_reco_jet_Sdxy_3N", &m_reco_jet_Sdxy_3N,"m_reco_jet_Sdxy_3N/F");
  ttree_out_->Branch("m_truth_jet_pt", &m_truth_jet_pt,"m_truth_jet_pt/F");
  ttree_out_->Branch("m_truth_jet_nConstituents", &m_truth_jet_nConstituents,"m_truth_jet_nConstituents/I");
  ttree_out_->Branch("m_truth_jet_nChConstituents", &m_truth_jet_nChConstituents,"m_truth_jet_nChConstituents/I");  
  ttree_out_->Branch("m_truth_jet_flavour", &m_truth_jet_flavour,"m_truth_jet_flavour/I");


  TH2D *d = new TH2D("d","d",300,0,30,300,0,30);
  d->GetXaxis()->SetTitle("pt_rec");
    d->GetYaxis()->SetTitle("pt_truth");

    TH1D *r = new TH1D("r_all","r_all",300,0,100);
    r->GetXaxis()->SetTitle("pt_rec");

    TH1D *t = new TH1D("t_all","t_all",300,0,100);
    t->GetXaxis()->SetTitle("pt_truth");

    TH1D *ru = new TH1D("r_unmatch","r_unmatch",300,0,100);
    ru->GetXaxis()->SetTitle("pt_rec");

    TH1D *rm = new TH1D("r_match","r_match",300,0,100);
    rm->GetXaxis()->SetTitle("pt_rec");

    TH1D *tm = new TH1D("t_match","t_match",300,0,100);
    tm->GetXaxis()->SetTitle("pt_truth");

    TH1D *phi = new TH1D("phi","phi",800,-4,4);
    phi->GetXaxis()->SetTitle("phi");
    TH1D *phim = new TH1D("phim","phim",800,-4,4);
    phim->GetXaxis()->SetTitle("phi_match");
*/
   /*  TH1D *rel = new TH1D("rel","rel",200,-20,20);
     TH1D *rec = new TH1D("rec","rec",200,-20,20);
     TH1D *reb = new TH1D("reb","reb",200,-20,20);*/
/*
   std::vector<std::vector<double>> binEdges;
   //axis 1 reco 
   std::vector<double> recoPt;
   for(int i = 0; i<81;i++){
      recoPt.push_back(10+i);
   }
   binEdges.push_back(recoPt);
    //axis 2 truth 
   std::vector<double> truthPt;
   for(int i = 0; i<51;i++){
      truthPt.push_back(30+i);
   }
   binEdges.push_back(truthPt);
   //axis 3 Sdxy
   std::vector<double> Sdzy;
   for(int i = 0; i<81;i++){
      Sdzy.push_back(-40+i);
   }
   binEdges.push_back(Sdzy);
   binEdges.push_back(Sdzy); //N1
   binEdges.push_back(Sdzy); //N2
   binEdges.push_back(Sdzy); //N3
   std::vector<double> flavour{-0.5,0.5,1.5,2.5,3.5};
   binEdges.push_back(flavour);
   std::vector<double> dpt;
   for(int i = 0; i<201;i++){
      dpt.push_back(-1+0.01*i);
   }
   binEdges.push_back(dpt);

   int nbins[]{80,50,80,80,80,80,4,200};

   //int nbin

   //THn (const char *name, const char *title, Int_t dim, const Int_t *nbins, const std::vector< std::vector< double > > &xbins)
   //THn_matrix = new THnF("THn_matrix","THn_matrix",8,nbins,binEdges);

   TH2D *dca_xy = new TH2D("dca_xy","dca_xy",90,10,100,100,-10,10);
   TH2D *dca_xy_unc = new TH2D("dca_xy_unc","dca_xy_unc",90,10,100,100,0,0.01);
   TH2D *dca_xy_cut = new TH2D("dca_xy_cut","dca_xy_cut",90,10,100,100,-10,10);
   TH2D *dca_xy_unc_cut = new TH2D("dca_xy_unc_cut","dca_xy_unc_cut",90,10,100,100,0,0.01);
   
   int num = 8;
   TH1D *rel[num];
   TH1D *rec[num];
   TH1D *reb[num];
    std::string tagName[8] = {"Sdxy_old","Sdxy_old_cut","Sdxy","Sdxy_cut","1_N","2_N","3_N","4_N"};//"dxy","dxy_unc",
    for(int i = 0; i<num;i++){ 
      TString name = "matching_"+tagName[i];
      rel[i] = new TH1D(name+"_LF",name+"_LF",100,-40,40);
      rec[i] = new TH1D(name+"_C",name+"_C",100,-40,40);
      reb[i] = new TH1D(name+"_B",name+"_B",100,-40,40);
      rel[i]->GetYaxis()->SetTitle("Probability distribution");
      rel[i]->GetXaxis()->SetTitle("2D Impact Parameter significance");
      //if(i==0)rel[i]->GetXaxis()->SetTitle("2D Impact Parameter");
      rec[i]->GetYaxis()->SetTitle("Probability distribution");
      rec[i]->GetXaxis()->SetTitle("2D Impact Parameter significance");
      reb[i]->GetYaxis()->SetTitle("Probability distribution");
      reb[i]->GetXaxis()->SetTitle("2D Impact Parameter significance");
    }

    r->GetXaxis()->SetTitle("pt_rec");
    t->GetXaxis()->SetTitle("pt_truth");
    TCanvas *c = new TCanvas("c","c",2400,800);
    c->Divide(6,1);

     TCanvas *c2 = new TCanvas("c2","c2",2400,800);
    c2->Divide(7,1);

    TH1I *mat[4];
    std::string matName[4] = {"total","LF","C","B"};
    for(int i = 0; i<4;i++){ 
      TString name = "matching_"+matName[i];
      mat[i]= new TH1I(name,name,5,0,5);
      mat[i]->GetXaxis()->SetBinLabel(1,"Uniquely matched");
      mat[i]->GetXaxis()->SetBinLabel(2,"Unmatched R");
      mat[i]->GetXaxis()->SetBinLabel(3,"Unmatched T");
      mat[i]->GetXaxis()->SetBinLabel(4,"2T -> 1R");
      mat[i]->GetXaxis()->SetBinLabel(5,"1T -> 2R");
    }
*/
//int countFD = 0;
   // Read a TriggerInfo object from the tree entries:
   // Now iterate through the TTree entries and fill a histogram.
//std::cout<<"E"<<std::endl;
  // int nentries = reader.GetEntries();
   int nentries = tc->GetEntries();
   std::cout<<"events "<<nentries<<std::endl;
//std::cout<<"F"<<std::endl;
   while (reader.Next()) {
 //     if (!CheckValue(*jet_container_)) return false;
      /*if (!CheckValue(*n_reco_jet_)) return false;
      if (!CheckValue(*n_truth_jet_)) return false;
      if (!CheckValue(*reco_jet_id_)) return false;
      if (!CheckValue(*reco_jet_nConstituents_)) return false;
      if (!CheckValue(*reco_jet_nChConstituents_)) return false;
      if (!CheckValue(*reco_jet_px_)) return false;
      if (!CheckValue(*reco_jet_py_)) return false;
      if (!CheckValue(*reco_jet_pz_)) return false;
      if (!CheckValue(*reco_jet_pt_)) return false;
      if (!CheckValue(*reco_jet_eta_)) return false;
      if (!CheckValue(*reco_jet_phi_)) return false;
      if (!CheckValue(*reco_jet_m_)) return false;
      if (!CheckValue(*reco_jet_e_)) return false;
      if (!CheckValue(*reco_constituents_dxy_)) return false;
      if (!CheckValue(*reco_constituents_dxy_unc_)) return false;
      if (!CheckValue(*reco_constituents_Sdxy_old_)) return false;
      if (!CheckValue(*reco_constituents_Sdxy_)) return false;
      if (!CheckValue(*reco_constituents_dxy_cut_)) return false;
      if (!CheckValue(*reco_constituents_dxy_unc_cut_)) return false;
      if (!CheckValue(*reco_constituents_Sdxy_old_cut_)) return false;
      if (!CheckValue(*reco_constituents_Sdxy_cut_)) return false;
      if (!CheckValue(*truth_jet_id_)) return false;
      if (!CheckValue(*truth_jet_nConstituents_)) return false;
      if (!CheckValue(*truth_jet_nChConstituents_)) return false;
      if (!CheckValue(*truth_jet_px_)) return false;
      if (!CheckValue(*truth_jet_py_)) return false;
      if (!CheckValue(*truth_jet_pz_)) return false;
      if (!CheckValue(*truth_jet_pt_)) return false;
      if (!CheckValue(*truth_jet_eta_)) return false;
      if (!CheckValue(*truth_jet_phi_)) return false;
      if (!CheckValue(*truth_jet_m_)) return false;
      if (!CheckValue(*truth_jet_e_)) return false;
      if (!CheckValue(*truth_constituents_PDG_ID_)) return false;*/
    //std::cout<<"AA"<<std::endl;  
 //     printProgress(reader.GetCurrentEntry(),nentries);    //reader.GetEntries();   printProgress(reader.GetCurrentEntry(),reader.GetEntries())
//std::cout<<"AB"<<std::endl;  JetIDT_map
      //main matching loop
    /*  std::vector<int> matchedR;
      std::vector<int> matchedT;
      std::vector<int> JetIDR;
      std::vector<int> JetIDT;
      std::map<int,int> JetIDR_map;
      std::map<int,int> JetIDT_map;*/

      auto jet_container = *jet_container_2;
      //auto truth_jets = (**jet_container_).truthjets;

      std::cout<<"R04 "<< jet_container.reco_jet_n <<std::endl;

      for (auto reco_jet : jet_container.recojets){
         std::cout<<"JET pt "<< reco_jet.pt<<std::endl;
         std::cout<<"JET E "<< reco_jet.e<<std::endl;
         for (auto constituents : reco_jet.constituents){
            std::cout<<"track pt "<< constituents.pt<<std::endl;
           // std::cout<<"track chi2 "<< constituents.chisq<<std::endl;
           // std::cout<<"track mvtx "<< constituents.n_mvtx<<std::endl;
          //  std::cout<<"track intt "<< constituents.n_intt<<std::endl;
           // std::cout<<"track tpc "<< constituents.n_tpc<<std::endl;

         }

         
      }

            
   



      /*

      //ID of reco jets
      for (int ijetR = 0; ijetR < **n_reco_jet_; ijetR++) {
         if((**reco_jet_pt_).at(ijetR) < 5.0) continue;
         if((**reco_jet_pt_).at(ijetR) > 120.0) continue;
         if(std::abs((**reco_jet_eta_).at(ijetR)) > 0.7) continue;
         JetIDR_map.insert({(**reco_jet_id_).at(ijetR), ijetR});
         JetIDR.push_back((**reco_jet_id_).at(ijetR));
         phi->Fill((**reco_jet_phi_).at(ijetR));
      }
      //ID of truth jet
      for (int ijetT = 0; ijetT < **n_truth_jet_; ijetT++) {
         if((**truth_jet_pt_).at(ijetT) < 10.0) continue;
         if((**truth_jet_pt_).at(ijetT) > 100.0) continue;
         if(std::abs((**truth_jet_eta_).at(ijetT)) > 0.7) continue;
         JetIDT_map.insert({(**truth_jet_id_).at(ijetT), ijetT});
         JetIDT.push_back((**truth_jet_id_).at(ijetT));
      }

      //main matching loop
      std::vector<std::pair<int,int>> matchedPair;
      for (int ijetR = 0; ijetR < **n_reco_jet_; ijetR++) {
         if((**reco_jet_pt_).at(ijetR) < 5.0) continue;
         if((**reco_jet_pt_).at(ijetR) > 120.0) continue;
         if(std::abs((**reco_jet_eta_).at(ijetR)) > 0.7) continue;
         for (int ijetT = 0; ijetT < **n_truth_jet_; ijetT++) {
            if((**truth_jet_pt_).at(ijetT) < 10.0) continue;
            if((**truth_jet_pt_).at(ijetT) > 100.0) continue;
            if(std::abs((**truth_jet_eta_).at(ijetT)) > 0.7) continue;
         
            double etaR = (**reco_jet_eta_).at(ijetR);
            double etaT = (**truth_jet_eta_).at(ijetT);
            double phiR = (**reco_jet_phi_).at(ijetR);
            double phiT = (**truth_jet_phi_).at(ijetT);
            double dR = std::sqrt((etaR-etaT)*(etaR-etaT)+(phiR-phiT)*(phiR-phiT));
            if (dR > 0.3){
               // unmatched
               continue;
            } 
            else{
               matchedPair.push_back(std::pair((**reco_jet_id_).at(ijetR),(**truth_jet_id_).at(ijetT)));
               matchedR.push_back((**reco_jet_id_).at(ijetR));
               matchedT.push_back((**truth_jet_id_).at(ijetT));
            }
         }
      }

      //uniquely matched jets
      for(auto &i : matchedPair){
         int pos = JetIDT_map.find(i.second)->second;
         if(!(((**truth_constituents_PDG_ID_).at(pos)).empty())){
            if(TString((TDatabasePDG::Instance()->GetParticle((**truth_constituents_PDG_ID_).at(pos).at(0)))->ParticleClass()) == "CharmedMeson" || TString((TDatabasePDG::Instance()->GetParticle((**truth_constituents_PDG_ID_).at(pos).at(0)))->ParticleClass()) == "CharmedBaryon")mat[2]->Fill(0.5);
            else if(TString((TDatabasePDG::Instance()->GetParticle((**truth_constituents_PDG_ID_).at(pos).at(0)))->ParticleClass()) == "B-Meson" || TString((TDatabasePDG::Instance()->GetParticle((**truth_constituents_PDG_ID_).at(pos).at(0)))->ParticleClass()) == "B-Baryon")mat[3]->Fill(0.5);
         }
         else mat[1]->Fill(0.5);
         mat[0]->Fill(0.5);
      }

      //are there unmatched jets?
      //unmatched reco
      std::vector<int> unmatchedR;
      std::set_difference(JetIDR.begin(), JetIDR.end(), matchedR.begin(), matchedR.end(), std::inserter(unmatchedR, unmatchedR.begin()));
      for (auto& s : unmatchedR){
         mat[0]->Fill(1.5);
      }

      //unmatched truth
      std::vector<int> unmatchedT;
      std::set_difference(JetIDT.begin(), JetIDT.end(), matchedT.begin(), matchedT.end(), std::inserter(unmatchedT, unmatchedT.begin()));
      for (auto& i : unmatchedT){
         int pos = JetIDT_map.find(i)->second;
         if(!(((**truth_constituents_PDG_ID_).at(pos)).empty())){
            if(TString((TDatabasePDG::Instance()->GetParticle((**truth_constituents_PDG_ID_).at(pos).at(0)))->ParticleClass()) == "CharmedMeson" || TString((TDatabasePDG::Instance()->GetParticle((**truth_constituents_PDG_ID_).at(pos).at(0)))->ParticleClass()) == "CharmedBaryon")mat[2]->Fill(2.5);
            else if(TString((TDatabasePDG::Instance()->GetParticle((**truth_constituents_PDG_ID_).at(pos).at(0)))->ParticleClass()) == "B-Meson" || TString((TDatabasePDG::Instance()->GetParticle((**truth_constituents_PDG_ID_).at(pos).at(0)))->ParticleClass()) == "B-Baryon")mat[3]->Fill(2.5);
         }
         else mat[1]->Fill(2.5);
         mat[0]->Fill(2.5);
      }

      //are there doubly matched jets?
      //2 reco -> 1 truth
      std::set<int> doublymatchedR = findDuplicates(matchedR);
      for (auto &s: doublymatchedR) {
         int index = 0;
         for(auto &m :matchedPair){
            if(m.first == s) index = m.second;
            int pos = JetIDT_map.find(index)->second;
            if(!(((**truth_constituents_PDG_ID_).at(pos)).empty())){
               if(TString((TDatabasePDG::Instance()->GetParticle((**truth_constituents_PDG_ID_).at(pos).at(0)))->ParticleClass()) == "CharmedMeson" || TString((TDatabasePDG::Instance()->GetParticle((**truth_constituents_PDG_ID_).at(pos).at(0)))->ParticleClass()) == "CharmedBaryon")mat[2]->Fill(3.5);
               else if(TString((TDatabasePDG::Instance()->GetParticle((**truth_constituents_PDG_ID_).at(pos).at(0)))->ParticleClass()) == "B-Meson" || TString((TDatabasePDG::Instance()->GetParticle((**truth_constituents_PDG_ID_).at(pos).at(0)))->ParticleClass()) == "B-Baryon")mat[3]->Fill(3.5);
            }
            else mat[1]->Fill(3.5);
         }
         mat[0]->Fill(3.5);
      }

      //truth -> 2 reco
      std::set<int> doublymatchedT = findDuplicates(matchedT);
      for (auto &i: doublymatchedT) {
         int pos = JetIDT_map.find(i)->second;
         if(!(((**truth_constituents_PDG_ID_).at(pos)).empty())){
            if(TString((TDatabasePDG::Instance()->GetParticle((**truth_constituents_PDG_ID_).at(pos).at(0)))->ParticleClass()) == "CharmedMeson" || TString((TDatabasePDG::Instance()->GetParticle((**truth_constituents_PDG_ID_).at(pos).at(0)))->ParticleClass()) == "CharmedBaryon")mat[2]->Fill(4.5);
            else if(TString((TDatabasePDG::Instance()->GetParticle((**truth_constituents_PDG_ID_).at(pos).at(0)))->ParticleClass()) == "B-Meson" || TString((TDatabasePDG::Instance()->GetParticle((**truth_constituents_PDG_ID_).at(pos).at(0)))->ParticleClass()) == "B-Baryon")mat[3]->Fill(4.5);
         }
         else mat[1]->Fill(4.5);
         mat[0]->Fill(4.5);
      }
//std::cout<<"BB"<<std::endl;  

      for (int ijetR = 0; ijetR < **n_reco_jet_; ijetR++) {
         r->Fill((**reco_jet_pt_).at(ijetR));
         if(std::find(unmatchedR.begin(), unmatchedR.end(),(**reco_jet_id_).at(ijetR)) != unmatchedR.end()){
            ru->Fill((**reco_jet_pt_).at(ijetR));
         }

      }
         for (int ijetT = 0; ijetT < **n_truth_jet_; ijetT++) { 
            t->Fill((**truth_jet_pt_).at(ijetT));
         }

      //Main analysis loop
      for (int ijetR = 0; ijetR < **n_reco_jet_; ijetR++) {
         for (int ijetT = 0; ijetT < **n_truth_jet_; ijetT++) {        
            double etaR = (**reco_jet_eta_).at(ijetR);
            double etaT = (**truth_jet_eta_).at(ijetT);
            double phiR = (**reco_jet_phi_).at(ijetR);
            double phiT = (**truth_jet_phi_).at(ijetT);
            double dR = std::sqrt((etaR-etaT)*(etaR-etaT)+(phiR-phiT)*(phiR-phiT));
            //find matched combination
            if ( std::find(matchedPair.begin(), matchedPair.end(), std::pair((**reco_jet_id_).at(ijetR),(**truth_jet_id_).at(ijetT))) != matchedPair.end() ){
               if(std::find(doublymatchedR.begin(), doublymatchedR.end(),(**reco_jet_id_).at(ijetR)) != doublymatchedR.end()){
                  //reject doubly matched reco jets
                  continue;
               }
               if(std::find(doublymatchedT.begin(), doublymatchedT.end(),(**truth_jet_id_).at(ijetT)) != doublymatchedT.end()){
                  //reject doubly matched truth jets
                  continue;
               }
               m_reco_constituents_Sdxy.clear();
               m_reco_constituents_Sdxy_cut.clear();
               phim->Fill((**reco_jet_phi_).at(ijetR));

               //uniquely matched jet pairs
               d->Fill((**reco_jet_pt_).at(ijetR),(**truth_jet_pt_).at(ijetT));
               rm->Fill((**reco_jet_pt_).at(ijetR));
               tm->Fill((**truth_jet_pt_).at(ijetT));
               m_reco_jet_pt = (**reco_jet_pt_).at(ijetR);
               m_truth_jet_pt = (**truth_jet_pt_).at(ijetT);

               //get flavout IDs 
               if(!(((**truth_constituents_PDG_ID_).at(ijetT)).empty())){
                  if(TString((TDatabasePDG::Instance()->GetParticle((**truth_constituents_PDG_ID_).at(ijetT).at(0)))->ParticleClass()) == "CharmedMeson" || TString((TDatabasePDG::Instance()->GetParticle((**truth_constituents_PDG_ID_).at(ijetT).at(0)))->ParticleClass()) == "CharmedBaryon"){
                     m_truth_jet_flavour = 1;     
                  }
                  else if(TString((TDatabasePDG::Instance()->GetParticle((**truth_constituents_PDG_ID_).at(ijetT).at(0)))->ParticleClass()) == "B-Meson" || TString((TDatabasePDG::Instance()->GetParticle((**truth_constituents_PDG_ID_).at(ijetT).at(0)))->ParticleClass()) == "B-Baryon"){
                     m_truth_jet_flavour = 2;
                  }
                  else
                     m_truth_jet_flavour = 3;
               }
               else{
                  m_truth_jet_flavour = 0;
               }

               //get flavout IDs 
               bool isLF = false;
               bool isC = false;
               bool isB = false;
               if(!(((**truth_constituents_PDG_ID_).at(ijetT)).empty())){
                  for(auto iconstitient : (**truth_constituents_PDG_ID_).at(ijetT)){
                     if(TString((TDatabasePDG::Instance()->GetParticle(iconstitient))->ParticleClass()) == "CharmedMeson" || TString((TDatabasePDG::Instance()->GetParticle(iconstitient))->ParticleClass()) == "CharmedBaryon"){
                        isC = true;    
                     }
                     else if(TString((TDatabasePDG::Instance()->GetParticle(iconstitient))->ParticleClass()) == "B-Meson" || TString((TDatabasePDG::Instance()->GetParticle(iconstitient))->ParticleClass()) == "B-Baryon"){
                        isB = true;
                     }
                  }
               }
               else
                  isLF = true;

*/

               //if(m_truth_jet_flavour == 0 && !isLF ) std::cout<<"old LF but not new LF"<<std::endl;

               //if(m_truth_jet_flavour == 1 && !isC ) std::cout<<"old C but not new C"<<std::endl;
              // if(m_truth_jet_flavour == 2 && !isB ) std::cout<<"old B but not new B"<<std::endl;
               /*if(isC && isB ){
                  countFD++;
                  std::cout<<"both C and B"<<std::endl;
                  if(!(((**truth_constituents_PDG_ID_).at(ijetT)).empty())){
                  for(auto iconstitient : (**truth_constituents_PDG_ID_).at(ijetT)){
                     std::cout<<TString((TDatabasePDG::Instance()->GetParticle(iconstitient))->ParticleClass())<<" ID: "<<iconstitient<<std::endl;

                     
                  }

               } 
               }*/
/*
               for( auto elem :(**reco_constituents_Sdxy_old_).at(ijetR)){
                  if (isC) rec[0]->Fill(elem);
                  if (isB) reb[0]->Fill(elem);
                  if (isLF) rel[0]->Fill(elem);
               }

               for( auto elem :(**reco_constituents_Sdxy_old_cut_).at(ijetR)){
                 if (isC) rec[1]->Fill(elem);
                 if (isB) reb[1]->Fill(elem);
                 if (isLF) rel[1]->Fill(elem);
               }

               for( auto elem :(**reco_constituents_Sdxy_).at(ijetR)){
                 if (isC) rec[2]->Fill(elem);
                 if (isB) reb[2]->Fill(elem);
                 if (isLF) rel[2]->Fill(elem);
               }

               for( auto elem :(**reco_constituents_Sdxy_cut_).at(ijetR)){
                 if (isC) rec[3]->Fill(elem);
                 if (isB) reb[3]->Fill(elem);
                 if (isLF) rel[3]->Fill(elem);
               }
                  
                  
                
 */      


             /*  for( auto elem :(**reco_constituents_dxy_).at(ijetR)){
                  if(!(((**truth_constituents_PDG_ID_).at(ijetT)).empty())){
                     m_reco_constituents_dxy.push_back(elem);
                     if(TString((TDatabasePDG::Instance()->GetParticle((**truth_constituents_PDG_ID_).at(ijetT).at(0)))->ParticleClass()) == "CharmedMeson" || TString((TDatabasePDG::Instance()->GetParticle((**truth_constituents_PDG_ID_).at(ijetT).at(0)))->ParticleClass()) == "CharmedBaryon"){
                        rec[0]->Fill(elem);//*std::max_element(((**reco_constituents_Sdxy_).at(ijetR)).begin(), ((**reco_constituents_Sdxy_).at(ijetR)).end()));
                        
                     }
                     else if(TString((TDatabasePDG::Instance()->GetParticle((**truth_constituents_PDG_ID_).at(ijetT).at(0)))->ParticleClass()) == "B-Meson" || TString((TDatabasePDG::Instance()->GetParticle((**truth_constituents_PDG_ID_).at(ijetT).at(0)))->ParticleClass()) == "B-Baryon"){
                        reb[0]->Fill(elem);//*std::max_element(((**reco_constituents_Sdxy_).at(ijetR)).begin(), ((**reco_constituents_Sdxy_).at(ijetR)).end()));

                     }
                  }
                  else{
                      rel[0]->Fill(elem);//*std::max_element(((**reco_constituents_Sdxy_).at(ijetR)).begin(), ((**reco_constituents_Sdxy_).at(ijetR)).end()));  

                  }
               }*/
               //std::cout<<"B"<<std::endl;
               /*for( auto elem :(**reco_constituents_Sdxy_).at(ijetR)){
                  if(!(((**truth_constituents_PDG_ID_).at(ijetT)).empty())){
                     m_reco_constituents_Sdxy.push_back(elem);
                     if(TString((TDatabasePDG::Instance()->GetParticle((**truth_constituents_PDG_ID_).at(ijetT).at(0)))->ParticleClass()) == "CharmedMeson" || TString((TDatabasePDG::Instance()->GetParticle((**truth_constituents_PDG_ID_).at(ijetT).at(0)))->ParticleClass()) == "CharmedBaryon")rec[1]->Fill(elem);//*std::max_element(((**reco_constituents_Sdxy_).at(ijetR)).begin(), ((**reco_constituents_Sdxy_).at(ijetR)).end()));
                     else if(TString((TDatabasePDG::Instance()->GetParticle((**truth_constituents_PDG_ID_).at(ijetT).at(0)))->ParticleClass()) == "B-Meson" || TString((TDatabasePDG::Instance()->GetParticle((**truth_constituents_PDG_ID_).at(ijetT).at(0)))->ParticleClass()) == "B-Baryon")reb[1]->Fill(elem);//*std::max_element(((**reco_constituents_Sdxy_).at(ijetR)).begin(), ((**reco_constituents_Sdxy_).at(ijetR)).end()));
                  }
                  else rel[1]->Fill(elem);//*std::max_element(((**reco_constituents_Sdxy_).at(ijetR)).begin(), ((**reco_constituents_Sdxy_).at(ijetR)).end()));  
               }*/
               //  std::cout<<"C"<<std::endl;
                     
               //if((((**reco_constituents_Sdxy_).at(ijetR)).size())> 3){
                            //std::vector<float> orig_vec=(**reco_constituents_Sdxy_).at(ijetR);
                          //  std::cout<<"jet"<<std::endl;

 /*              for( auto elem :(**reco_constituents_Sdxy_cut_).at(ijetR)){
                     if (std::abs(elem) < 40) m_reco_constituents_Sdxy_cut.push_back(elem);
               }

               if((((**reco_constituents_Sdxy_cut_).at(ijetR)).size())> 0){
                  std::nth_element((**reco_constituents_Sdxy_cut_).at(ijetR).begin(), (**reco_constituents_Sdxy_cut_).at(ijetR).begin()+1, (**reco_constituents_Sdxy_cut_).at(ijetR).end(), std::greater<double>());
                  int off = 0;
                  for(int tag_power = 0; tag_power < 4; tag_power++){
                     float dnd = -999;
                     //if(tag_power < (**reco_constituents_Sdxy_).at(ijetR).size()) dnd = *((**reco_constituents_Sdxy_).at(ijetR).begin()+tag_power);
                     if(tag_power + off < (**reco_constituents_Sdxy_cut_).at(ijetR).size()){
                        dnd = *((**reco_constituents_Sdxy_cut_).at(ijetR).begin()+tag_power+off);
                        if(std::abs(dnd) > 40){
                           off++;
                           if(tag_power + off < (**reco_constituents_Sdxy_cut_).at(ijetR).size()) dnd = *((**reco_constituents_Sdxy_cut_).at(ijetR).begin()+tag_power+off);
                           else dnd = -999;
                           if(std::abs(dnd) > 40){
                           off++;
                           if(tag_power + off < (**reco_constituents_Sdxy_cut_).at(ijetR).size()) dnd = *((**reco_constituents_Sdxy_cut_).at(ijetR).begin()+tag_power+off);
                           else dnd = -999;
                        } 
                        } 

                     } 
                     //if(tag_power < (**reco_constituents_Sdxy_cut_).at(ijetR).size()){
                     //   dnd = *((**reco_constituents_Sdxy_cut_).at(ijetR).begin()+tag_power);
                     //} 
                     if (tag_power == 0) m_reco_jet_Sdxy_1N = dnd;
                     if (tag_power == 1) m_reco_jet_Sdxy_2N = dnd;
                     if (tag_power == 2) m_reco_jet_Sdxy_3N = dnd;
                    // if (tag_power == 3) m_reco_jet_Sdxy_4N = dnd;
                     //std::cout<<tag_power<<" "<<dnd<<std::endl;
                     if(!(((**truth_constituents_PDG_ID_).at(ijetT)).empty())){
                        if(TString((TDatabasePDG::Instance()->GetParticle((**truth_constituents_PDG_ID_).at(ijetT).at(0)))->ParticleClass()) == "CharmedMeson" || TString((TDatabasePDG::Instance()->GetParticle((**truth_constituents_PDG_ID_).at(ijetT).at(0)))->ParticleClass()) == "CharmedBaryon")rec[4+tag_power]->Fill(dnd);//*std::max_element(((**reco_constituents_Sdxy_).at(ijetR)).begin(), ((**reco_constituents_Sdxy_).at(ijetR)).end()));
                        else if(TString((TDatabasePDG::Instance()->GetParticle((**truth_constituents_PDG_ID_).at(ijetT).at(0)))->ParticleClass()) == "B-Meson" || TString((TDatabasePDG::Instance()->GetParticle((**truth_constituents_PDG_ID_).at(ijetT).at(0)))->ParticleClass()) == "B-Baryon")reb[4+tag_power]->Fill(dnd);//*std::max_element(((**reco_constituents_Sdxy_).at(ijetR)).begin(), ((**reco_constituents_Sdxy_).at(ijetR)).end()));
                     }
                     else rel[4+tag_power]->Fill(dnd);
                  }    
               }

               */
               //std::cout<<"D"<<std::endl;       
   //            ttree_out_->Fill();
  //          }        
  //       }
  //    }


//std::cout<<"CC"<<std::endl;  
   


      /*for (int ijetT = 0; ijetT < **n_truth_jet_; ijetT++) {
         if((**truth_jet_pt_).at(ijetT) < 2.0) continue;
         for (int ijetR = 0; ijetR < **n_reco_jet_; ijetR++) {
            if((**reco_jet_pt_).at(ijetR) < 2.0) continue;
            double etaR = (**reco_jet_eta_).at(ijetR);
            double etaT = (**truth_jet_eta_).at(ijetT);
            double phiR = (**reco_jet_phi_).at(ijetR);
            double phiT = (**truth_jet_phi_).at(ijetT);
            double dR = std::sqrt((etaR-etaT)*(etaR-etaT)+(phiR-phiT)*(phiR-phiT));
            if (dR > 0.3) continue;
          //  if ((**truth_jet_pt_).at(ijetT)/(**reco_jet_pt_).at(ijetR) < 0.5 ||(**truth_jet_pt_).at(ijetT)/(**reco_jet_pt_).at(ijetR) > 1.5) continue;
            else{
               if ( std::find(matched.begin(), matched.end(), (**reco_jet_id_).at(ijetR)/*std::pair((**reco_jet_id_).at(ijetR),(**truth_jet_id_).at(ijetT))*//*) != matched.end() ){
                  std::cout<<"ERROR: This jet was already matched"<<std::endl;
                  ptr = 0;
                  ptt = 0;
                  d->Fill(0.0,0.0);
                  r->Fill(0);
                  t->Fill(0);
               }else{
                  matched.push_back(/*std::pair(*//*(**reco_jet_id_).at(ijetR)/*,(**truth_jet_id_).at(ijetT))*//*);
                  /*if((**truth_jet_pt_).at(ijetT) > ptt){
                     //if(((**truth_constituents_PDG_ID_).at(ijetT)).size() > 0){
                        ptr = (**reco_jet_pt_).at(ijetR);
                        ptt =(**truth_jet_pt_).at(ijetT);
                     //}
                  }*/
                  //r->Fill((**reco_jet_pt_).at(ijetR));
                  //t->Fill((**truth_jet_pt_).at(ijetT));
                 // std::cout<<"match: "<<(**reco_jet_id_).at(ijetR)<<" "<<(**truth_jet_id_).at(ijetT)<<" "<<dR<<" "<<(**reco_jet_pt_).at(ijetR)<<" "<< (**truth_jet_pt_).at(ijetT)<<std::endl;
                 //if(((**truth_constituents_PDG_ID_).at(ijetT)).size() > 0){
                  /*d->Fill((**reco_jet_pt_).at(ijetR),(**truth_jet_pt_).at(ijetT));
                  r->Fill((**reco_jet_pt_).at(ijetR));
                  t->Fill((**truth_jet_pt_).at(ijetT));
                // }
               }
  
               
              // std::cout<<"match: "<<(**reco_jet_id_).at(ijetR)<<" "<<(**truth_jet_id_).at(ijetT)<<" "<<dR<<" "<<(**reco_jet_pt_).at(ijetR)<<" "<< (**truth_jet_pt_).at(ijetT)<<std::endl;
            }
        
         }
      }*/

          /*  d->Fill(ptr,ptt);
                  r->Fill(ptr);
                  t->Fill(ptt);
                  re->Fill((ptr-ptt)/ptt);*/
     // matched.clear();



      



      //std::cout<<" rec "<<**n_reco_jet_;
   /*   for (int ijet = 0; ijet < **n_reco_jet_; ijet++) {
        if((**reco_jet_pt_).at(ijet) < 2.0) continue;
        std::cout<<"R pt: "<<(**reco_jet_pt_).at(ijet)<<" "<<(**reco_jet_eta_).at(ijet)<<" "<<(**reco_jet_phi_).at(ijet)<<" "<<(**reco_jet_id_).at(ijet)<<std::endl;
      }
      //std::cout<<" tru "<<**n_truth_jet_<<std::endl;
      for (int ijet = 0; ijet < **n_truth_jet_; ijet++) {
         if((**truth_jet_pt_).at(ijet) < 2.0) continue;
         //std::cout<<"cons: "<<((**truth_jet_nConstituents_).at(ijet))<<" "<<((**truth_constituents_PDG_ID_).at(ijet)).size()<<std::endl;
        std::cout<<"T pt: "<<(**truth_jet_pt_).at(ijet)<<" "<<(**truth_jet_eta_).at(ijet)<<" "<<(**truth_jet_phi_).at(ijet)<<" "<<(**truth_jet_id_).at(ijet)<<std::endl;;
        for(auto pdg : ((**truth_constituents_PDG_ID_).at(ijet))){
          std::cout<<"pdg "<<pdg<<" "<<std::endl;
        }
        //std::cout<<std::endl;
      }*/

     /* for (auto i_reco_jet_eta_: *reco_jet_eta_) {
         std::cout<<i_reco_jet_eta_<<std::endl;
      }*/

 /*     unmatchedR.clear();
      unmatchedT.clear();
      doublymatchedR.clear();
      doublymatchedT.clear();
      matchedPair.clear();
      matchedR.clear();
      matchedT.clear();
      JetIDR_map.clear();
      JetIDT_map.clear();*/
     
   } // TTree entry / event loop

  //   std::cout<<"FD count" <<countFD<<std::endl;


  /* ttree_out_->Write("", TObject::kOverwrite);
    for (int i = 0; i< 4; i++){
      mat[i]->Write();
   }
   r->Write();
   ru->Write();
   rm->Write();
   t->Write();
   tm->Write();*/

   
   //fo->Close();

   //std::cout<<"G"<<std::endl;  
 /*  c2->cd(1);
   c2->cd(1)->SetLogy();
   t->Draw();
   c2->cd(2);
   c2->cd(2)->SetLogy();
   r->Draw();*/



  /* for(int i=0;i<7;i++){
      c2->cd(i+1);
      c2->cd(i+1)->SetLogy();
      rel[i]->Scale(1./rel[i]->Integral());
      rec[i]->Scale(1./rec[i]->Integral());
      reb[i]->Scale(1./reb[i]->Integral());
      rel[i]->Scale(1,"width");
      rec[i]->Scale(1,"width");
      reb[i]->Scale(1,"width");
      rel[i]->SetMarkerColor(kBlue);
      rec[i]->SetMarkerColor(kGreen);
      reb[i]->SetMarkerColor(kRed);
      rel[i]->SetLineColor(kBlue);
      rec[i]->SetLineColor(kGreen);
      reb[i]->SetLineColor(kRed);
      rel[i]->Draw();
      reb[i]->Draw("same");
      rec[i]->Draw("same");
      rel[i]->Write();
      reb[i]->Write();
      rec[i]->Write();
   }*/



   /*c->cd(1);
   d->Draw("colz");
   c->cd(2);
   r->Draw();
   c->cd(3);
   t->Draw();
     c->cd(4);
     c->cd(4)->SetLogy();
   rel->Draw();
    reb->Draw("same");
     rec->Draw("same");
        c->cd(5);
        c->cd(5)->SetLogy();
   mat->Draw();*/
/*
   for (int i = 0; i< 4; i++){
      c->cd(i+1);
      mat[i]->Draw();
   }
   c->cd(5);
   phi->Draw();
   c->cd(6);
   phim->Draw();
*/

   return true;
}

std::set<int> bjetanalyse::findDuplicates(std::vector<int> vec){
   std::set<int> duplicates;
   std::sort(vec.begin(), vec.end());
   std::set<int> distinct(vec.begin(), vec.end());
   std::set_difference(vec.begin(), vec.end(), distinct.begin(), distinct.end(),
   std::inserter(duplicates, duplicates.end()));
   return duplicates;
}

void bjetanalyse::printProgress(int cur, int total){  
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

bool bjetanalyse::Plot() {

   TChain *tc = new TChain("AntiKt_r04");
   TFile *f = new TFile("/sphenix/tg/tg01/hf/jkvapil/JET30_new/out.root","READ");
   TTreeReader reader("AntiKt_r04_o", f);

  TTreeReaderValue<float> m_reco_jet_pt{reader,"m_reco_jet_pt"};
  //TTreeReaderValue<float> m_reco_constituents_dxy{reader,"m_reco_constituents_dxy"};
 // TTreeReaderValue<std::vector<float>> m_reco_constituents_Sdxy{reader,"m_reco_constituents_Sdxy"};
  TTreeReaderValue<float> m_reco_jet_Sdxy_1N{reader,"m_reco_jet_Sdxy_1N"};
  TTreeReaderValue<float> m_reco_jet_Sdxy_2N{reader,"m_reco_jet_Sdxy_2N"};
  TTreeReaderValue<float> m_reco_jet_Sdxy_3N{reader,"m_reco_jet_Sdxy_3N"};
  TTreeReaderValue<float> m_truth_jet_pt{reader,"m_truth_jet_pt"};
  TTreeReaderValue<int> m_truth_jet_flavour{reader,"m_truth_jet_flavour"};

  TH1D *purity[3][4][300];
  TH1D *eff[4]; //flavour, 
  double bins[]{10,70};
  TH2D *RM_all = new TH2D("RM_all","RM_all",200,10,110,200,5,105);
  TH2D *RM_b = new TH2D("RM_b","RM_b",40,10,50,45,5,50);
  for (int i = 0; i < 3 ; i++){
    for (int j = 0; j < 4 ; j++){
      for (int k = 0; k < 300 ; k++){
      TString name = "purity_power"+std::to_string(i)+"_flavor_"+std::to_string(j)+"_sdxy_"+  std::to_string(k);
      //std::cout<<"creating "<<name<<std::endl; 
      purity[i][j][k] = new TH1D(name,name,1,bins);
    }
  }
  }

  RM_all->GetXaxis()->SetTitle("pT truth");
  RM_all->GetYaxis()->SetTitle("pT reco");
  RM_b->GetXaxis()->SetTitle("pT truth");
  RM_b->GetYaxis()->SetTitle("pT reco");

   for (int k = 0; k < 4 ; k++){
      TString name = "efficiency_flavour_"+std::to_string(k);
      eff[k] = new TH1D(name,name,1,bins);
      
    }


   TH1D *resolution[10];
   for (int k = 0; k < 9 ; k++){
      std::string num_text = std::to_string(bins[k]);
      std::string rounded = num_text.substr(0, num_text.find(".")+2);
      std::string num_text2 = std::to_string(bins[k+1]);
      std::string rounded2 = num_text2.substr(0, num_text2.find(".")+2);
      TString name = "resolution_pt_truth_"+rounded+"_"+rounded2;
      resolution[k] = new TH1D(name,name,40,-1,1);
      resolution[k]->GetXaxis()->SetTitle("dPT = (reco-truth)/truth");
      resolution[k]->GetYaxis()->SetTitle("probability");
    }
  
        float tag[4]{0,2.5,5,10};

   while (reader.Next()) {
      //std::cout<<"checking"<<std::endl;
      if (!CheckValue(m_reco_jet_pt)) return false;
      //if (!CheckValue(m_reco_constituents_dxy)) return false;
      //if (!CheckValue(m_reco_constituents_Sdxy)) return false;
      if (!CheckValue(m_reco_jet_Sdxy_1N)) return false;
      if (!CheckValue(m_reco_jet_Sdxy_2N)) return false;
      if (!CheckValue(m_reco_jet_Sdxy_3N)) return false;
      if (!CheckValue(m_truth_jet_pt)) return false;
      if (!CheckValue(m_truth_jet_flavour)) return false;
//std::cout<<"pass"<<std::endl;
      /*if(*m_reco_jet_pt < 30) continue;
      if(*m_reco_jet_pt > 50) continue;
      if(*m_truth_jet_pt < 30) continue;
      if(*m_truth_jet_pt > 50) continue;*/


for (int k = 0; k < 300 ; k++){
      if(*m_reco_jet_Sdxy_1N > 0.1*k){
         if(*m_truth_jet_flavour == 0) purity[0][0][k]->Fill(*m_reco_jet_pt);
         if(*m_truth_jet_flavour == 1) purity[0][1][k]->Fill(*m_reco_jet_pt);
         if(*m_truth_jet_flavour == 2) purity[0][2][k]->Fill(*m_reco_jet_pt);
         purity[0][3][k]->Fill(*m_reco_jet_pt);
      }
      if(*m_reco_jet_Sdxy_2N > 0.1*k){
         if(*m_truth_jet_flavour == 0) purity[1][0][k]->Fill(*m_reco_jet_pt);
         if(*m_truth_jet_flavour == 1) purity[1][1][k]->Fill(*m_reco_jet_pt);
         if(*m_truth_jet_flavour == 2) purity[1][2][k]->Fill(*m_reco_jet_pt);
         purity[1][3][k]->Fill(*m_reco_jet_pt);
         /*if(*m_truth_jet_flavour == 2 && *m_reco_jet_Sdxy_1N > tag[1]){
            RM_b->Fill(*m_truth_jet_pt,*m_reco_jet_pt);
               for (int k = 0; k < 9 ; k++){
                  if(*m_reco_jet_pt > 5){
                  if(bins[k] < *m_truth_jet_pt && *m_truth_jet_pt < bins[k+1])
                     resolution[k]->Fill(((*m_reco_jet_pt)-(*m_truth_jet_pt))/(*m_truth_jet_pt));
                  }
               }

         }*/
      }
      if(*m_reco_jet_Sdxy_3N > 0.1*k){
         if(*m_truth_jet_flavour == 0) purity[2][0][k]->Fill(*m_reco_jet_pt);
         if(*m_truth_jet_flavour == 1) purity[2][1][k]->Fill(*m_reco_jet_pt);
         if(*m_truth_jet_flavour == 2) purity[2][2][k]->Fill(*m_reco_jet_pt);
         purity[2][3][k]->Fill(*m_reco_jet_pt);
      }
   }

      if(*m_truth_jet_flavour == 0) eff[0]->Fill(*m_reco_jet_pt);
      if(*m_truth_jet_flavour == 1) eff[1]->Fill(*m_reco_jet_pt);
      if(*m_truth_jet_flavour == 2) eff[2]->Fill(*m_reco_jet_pt);
      eff[3]->Fill(*m_reco_jet_pt);
      RM_all->Fill(*m_truth_jet_pt,*m_reco_jet_pt);
   }

   TH1D *eff2[3][3][300]; //power flavour sdxy
   for (int i = 0; i < 3 ; i++){
    for (int j = 0; j < 3 ; j++){
      for (int k = 0; k < 300 ; k++){
      TString name = "eff_power"+std::to_string(i)+"_flavor_"+std::to_string(j)+"_sdxy_"+std::to_string(k);
      //std::cout<<"cloning "<<name<<std::endl;
      eff2[i][j][k] = (TH1D*)(purity[i][j][k]->Clone(name));
      eff2[i][j][k]->Divide(eff2[i][j][k],eff[j],1,1,"B");
    }
  }
  }


   for (int i = 0; i < 3 ; i++){
      for (int j = 0; j < 3 ; j++){
         for (int k = 0; k < 300 ; k++){
         if(i==0) purity[i][j][k]->SetMarkerColor(kBlue);
         if(i==1)purity[i][j][k]->SetMarkerColor(kGreen);
         if(i==2)purity[i][j][k]->SetMarkerColor(kRed);
         if(i==0)purity[i][j][k]->SetLineColor(kBlue);
         if(i==1)purity[i][j][k]->SetLineColor(kGreen);
         if(i==2)purity[i][j][k]->SetLineColor(kRed);
         purity[i][j][k]->Divide(purity[i][j][k],purity[i][3][k],1.,1.,"B");
         }
      }
   }

      for (int i = 0; i < 3 ; i++){
      for (int j = 0; j < 3 ; j++){
         for (int k = 0; k < 300 ; k++){
         if(i==0) eff2[i][j][k]->SetMarkerColor(kBlue);
         if(i==1)eff2[i][j][k]->SetMarkerColor(kGreen);
         if(i==2)eff2[i][j][k]->SetMarkerColor(kRed);
         if(i==0)eff2[i][j][k]->SetLineColor(kBlue);
         if(i==1)eff2[i][j][k]->SetLineColor(kGreen);
         if(i==2)eff2[i][j][k]->SetLineColor(kRed);
         }
      }
   }

   /*for (int i = 0; i < 3 ; i++){
      for (int k = 0; k < 4 ; k++){
      for (int bin = 0; bin < purity[i][2][k]->GetNbinsX() ; bin++){
      purity[i][2][k]->SetBinError(bin+1,0);
      }
      }
   }*/

  /*   for (int i = 0; i < 3 ; i++){
      for (int k = 0; k < 4 ; k++){
      for (int bin = 0; bin < eff2[i][2][k]->GetNbinsX() ; bin++){
      eff2[i][2][k]->SetBinError(bin+1,0);
      }
      }
   }*/

   std::cout<<"H"<<std::endl;
 

   /*   double x[100], y[100];
   int n = 20;
   for (int i=0;i<n;i++) {
     x[i] = i*0.1;
     y[i] = 10*sin(x[i]+0.2);
   }
   auto g = new TGraph(n,x,y);
   g->SetTitle("Graph title;X title;Y title");
   g->Draw("AC*");*/

    double ee1[250], pp1[250];
     double ee2[250], pp2[250];
      double ee3[250], pp3[250];

   for (int i = 0; i < 3 ; i++){
      for (int k = 0; k < 250 ; k++){
         if(i==0){
            //std::cout<<"P: "<<purity[i][2][k]->GetBinContent(1)<<" E: "<<eff2[i][2][k]->GetBinContent(1)<<std::endl;
            pp1[k] = purity[i][2][k]->GetBinContent(1);
            ee1[k] = eff2[i][2][k]->GetBinContent(1);
         } 
         if(i==1){
            //std::cout<<"P: "<<purity[i][2][k]->GetBinContent(1)<<" E: "<<eff2[i][2][k]->GetBinContent(1)<<std::endl;
            pp2[k] = purity[i][2][k]->GetBinContent(1);
            ee2[k] = eff2[i][2][k]->GetBinContent(1);
         } 
         if(i==2){
            //std::cout<<"P: "<<purity[i][2][k]->GetBinContent(1)<<" E: "<<eff2[i][2][k]->GetBinContent(1)<<std::endl;
            pp3[k] = purity[i][2][k]->GetBinContent(1);
            ee3[k] = eff2[i][2][k]->GetBinContent(1);
         } 


      }
   }

   auto g1 = new TGraph(250,ee1,pp1);
   g1->SetTitle("N1;eff_b;purity_b");
   g1->SetMarkerColor(kGreen);
   g1->SetLineColor(kGreen);
      auto g2 = new TGraph(250,ee2,pp2);
   g2->SetTitle("N1;eff;purity");
   g2->SetMarkerColor(kRed);
   g2->SetLineColor(kRed);
      auto g3 = new TGraph(250,ee3,pp3);
   g3->SetTitle("N1;eff;purity");
   g3->SetMarkerColor(kBlue);
   g3->SetLineColor(kBlue);

   /*for (int k = 0; k < 9 ; k++){
      resolution[k]->Scale(1./resolution[k]->Integral());
      resolution[k]->Scale(1.,"width");
   }

      TCanvas *c5 = new TCanvas("c5","c5",2000,1200);
   c5->Divide(6,2);
   c5->cd(1);
   c5->cd(1)->SetLogz();
   RM_all->Draw("colz");
   c5->cd(2);
   c5->cd(2)->SetLogz();
   RM_b->Draw("colz");
   for (int k = 0; k < 9 ; k++){
      c5->cd(k+3);
      c5->cd(k+3)->SetLogy();
      resolution[k]->Draw();
   }*/

         TCanvas *c5 = new TCanvas("c5","c5",2000,1200);
   //c5->Divide(6,2);
   c5->cd(1);
   c5->cd(1)->SetLogz();
   RM_all->Draw("colz");
      
std::cout<<"I AM HERE"<<std::endl;
   TCanvas *cc = new TCanvas("cc","cc",2000,1200);
   cc->cd();
   //cc->SetLogy();
   g1->GetXaxis()->SetRangeUser(0,1);
   g1->GetYaxis()->SetRangeUser(0,1);
   g1->Draw("AC*");
   g2->Draw("C* same");
   g3->Draw("C* same");
   //cc->Divide(10,10);

      /*   for (int k = 0; k < 4 ; k++){
            cc->cd(4*i+k+1);
            //eff2[i][0][k]->Draw();
            //eff2[i][1][k]->Draw();
            eff2[i][2][k]->Draw("e0");

         }*/



 /*  for (int k = 0; k < 100 ; k++){
      cc->cd(k+1);
         eff2[0][2][k]->GetYaxis()->SetRangeUser(0,1);
         std::string num_text = std::to_string(tag[k]);
         std::string rounded = num_text.substr(0, num_text.find(".")+2);
TString name = "efficiency, Sdxy > "+ rounded;//std::to_string(tag[k]);
         eff2[0][2][k]->SetTitle(name);
   eff2[0][2][k]->GetYaxis()->SetTitle("b-jet efficiency");
   eff2[0][2][k]->GetXaxis()->SetTitle("pt jet reco");
      eff2[0][2][k]->Draw("e0");
      eff2[1][2][k]->Draw("same e0");
      eff2[2][2][k]->Draw("same e0");*/
   /*cc->cd(k+5);
   name = "purity, Sdxy > "+ rounded;
         purity[0][2][k]->SetTitle(name);
   purity[0][2][k]->GetYaxis()->SetRangeUser(0,0.15);
   purity[0][2][k]->GetYaxis()->SetTitle("b-jet purity");
   purity[0][2][k]->GetXaxis()->SetTitle("pt jet reco");
   purity[0][2][k]->Draw("e0");
   purity[1][2][k]->Draw("same e0");
   purity[2][2][k]->Draw("same e0");*/
  // }
/*
   cc->cd(1);
   purity[0][0]->GetYaxis()->SetRangeUser(0,1);
   purity[0][0]->Draw();
   purity[0][1]->Draw("same");
   purity[0][2]->Draw("same");
     cc->cd(2);
   purity[1][0]->GetYaxis()->SetRangeUser(0,1);
   purity[1][0]->Draw();
   purity[1][1]->Draw("same");
   purity[1][2]->Draw("same");
     cc->cd(2);
   purity[2][0]->GetYaxis()->SetRangeUser(0,1);
   purity[2][0]->Draw();
   purity[2][1]->Draw("same");
   purity[2][2]->Draw("same");*/

   /*for (unsigned int ii = 0; ii < 3; ii++){
     std::cout << ii << std::endl;
      cc->cd(ii+1);
      std::cout<<"plotting "<<ii<<" 0"<<std::endl;
       purity[ii][0]->GetYaxis()->SetRangeUser(0,1);
      purity[ii][0]->Draw();
       std::cout<<"plotting "<<ii<<" 1"<<std::endl;
      purity[ii][1]->Draw("same");
       std::cout<<"plotting "<<ii<<" 2"<<std::endl;
      purity[ii][2]->Draw("same");
     

   }*/


return true;
}

