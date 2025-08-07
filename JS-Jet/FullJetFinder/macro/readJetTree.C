/**
 * @file readJetTree.C
 *
 * @brief An example macro how to proccess TTrees from FullJetFinder.cc
 *
 * @ingroup FullJetFinder
 *
 * @author Jakub Kvapil
 * Contact: jakub.kvapil@cern.ch
 *
 */

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
#include <string>
#include <vector>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wundefined-internal"
#include <FullJetFinder.h>
#pragma GCC diagnostic pop

R__LOAD_LIBRARY(libFullJetFinder.so)

bool CheckValue(ROOT::Internal::TTreeReaderValueBase& value) {
   if (value.GetSetupStatus() < 0) {
      std::cerr << "Error " << value.GetSetupStatus()
                << "setting up reader for " << value.GetBranchName() << '\n';
      return false;
   }
   return true;
}

void printProgress(int cur, int total){  
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

// Analyze TTree "AntiKt_r04/Data" in the file passed into the function.
// Returns false in case of errors.
//takes list of datafiles as entry
bool readJetTree(const std::string &dataFiles = "/sphenix/tg/tg01/hf/jkvapil/JET10_r22_npile_full/condorJob/myTestJets/outputData_000*.root"){
   TChain *tc = new TChain("AntiKt_r04/Data"); //TTRee name
   tc->Add(dataFiles.data());
   int n_events = tc->GetEntries(); //gen total number of events
   tc->LoadTree(-1); //previous commands detached the Tree head, reset back to supress warning
   TTreeReader reader(tc);
   TTreeReaderValue<FullJetFinder::Container> jet_container(reader,"JetContainer");

   TH1I *h_jet_n = new TH1I("h_jet_n","h_jet_n",2,-0.5,1.5);
   TH1F *h_reco_jet_pt = new TH1F("h_reco_jet_pt","h_reco_jet_pt",100,0,100);
   TH1F *h_reco_track_pt = new TH1F("h_reco_track_pt","h_reco_track_pt",100,0,100);
   TH1F *h_dca3d = new TH1F("h_dca3d","h_dca3d",100,-10,10);
 
   std::cout<<"Total number of events: "<<n_events<<std::endl;

   //main event loop
   while (reader.Next()){
      if (!CheckValue(jet_container)) return false; //NULL guards
      printProgress(reader.GetCurrentEntry(),n_events);

      h_jet_n->Fill(0.0,jet_container->reco_jet_n);
      h_jet_n->Fill(1.0,jet_container->truth_jet_n);

      //reco jet loop
      for (auto reco_jet : jet_container->recojets){
         h_reco_jet_pt->Fill(reco_jet.pt);
         //track inside jet loop
         for (auto chConstituent : reco_jet.chConstituents){
            h_reco_track_pt->Fill(chConstituent.pt);
	   h_dca3d->Fill(chConstituent.sDCA3d);
         }  // for (auto chConstituent : reco_jet.chConstituents)     
      }//f or (auto reco_jet : jet_container->recojets)
   } //while (reader.Next())

   //plotting
   TCanvas *c = new TCanvas("c","c",2400,800);
   c->Divide(4,1);
   c->cd(1);
   h_jet_n->Draw();
   c->cd(2);
   h_reco_jet_pt->Draw();
   c->cd(3);
   h_reco_track_pt->Draw();
c->cd(4);
h_dca3d->Draw();
   return true;
}
