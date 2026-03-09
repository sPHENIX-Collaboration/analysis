#include <iostream>
#include <fstream>
#include <vector>
#include <utility>

#include "sPhenixStyle.h"
#include "sPhenixStyle.C"

using namespace std;


Int_t SeedPlots()
{

    // set sPHENIX style
    SetsPhenixStyle();
    TH1::SetDefaultSumw2();
    TH2::SetDefaultSumw2();

   
    // in/out file names
    TString input_file = "/sphenix/user/tmengel/JetPerformancePPG/RandomConeAna/condor/merging/rootfiles/Jet30Debug.root";
    TString output_file = "Jet30_SeedPlots.root";

    // open input file
   TFile *f = new TFile(input_file, "READ");
    if(!f->IsOpen() || f->IsZombie()){ std::cout << "File " << input_file << " is zombie" << std::endl;  return -1; }

    // get tree
    TTree *t = (TTree*)f->Get("T");
    if(!t){ std::cout << "Tree event_info not found in " << input_file << std::endl; return -1; }

    // input variables 
    std::vector<float> * m_rawseedEta = 0;
    std::vector<float> * m_rawseedPhi = 0;
    std::vector<float> * m_rawseedPt = 0;
    std::vector<int> * m_rawseedCut = 0;

    std::vector<float> * m_subseedEta = 0;
    std::vector<float> * m_subseedPhi = 0;
    std::vector<float> * m_subseedPt = 0;
    std::vector<int> * m_subseedCut = 0;

    std::vector<float> * m_tower_pt = 0;
    std::vector<float> * m_tower_iter_pt = 0;
    std::vector<float> * m_tower_rand_pt = 0;
    std::vector<float> * m_tower_iter_rand_pt = 0;

    t->SetBranchAddress("rawseedEta", &m_rawseedEta);
    t->SetBranchAddress("rawseedPhi", &m_rawseedPhi);
    t->SetBranchAddress("rawseedPt", &m_rawseedPt);
    t->SetBranchAddress("rawseedCut", &m_rawseedCut);
    t->SetBranchAddress("subseedEta", &m_subseedEta);
    t->SetBranchAddress("subseedPhi", &m_subseedPhi);
    t->SetBranchAddress("subseedPt", &m_subseedPt);
    t->SetBranchAddress("subseedCut", &m_subseedCut);
    t->SetBranchAddress("tower_pt", &m_tower_pt);
    t->SetBranchAddress("tower_iter_pt", &m_tower_iter_pt);
    t->SetBranchAddress("tower_pt_rand", &m_tower_rand_pt);
    t->SetBranchAddress("tower_iter_pt_rand", &m_tower_iter_rand_pt);
    
    int nentries = t->GetEntries();

    // histograms
    TH1F * h_rawseedPt = new TH1F("h_rawseedPt", "h_rawseedPt", 100, 0, 100);
    TH1F * h_subseedPt = new TH1F("h_subseedPt", "h_subseedPt", 100, 0, 100);
    TH1F * h_rawseedEta = new TH1F("h_rawseedEta", "h_rawseedEta", 100,-1.5,1.5);
    TH1F * h_subseedEta = new TH1F("h_subseedEta", "h_subseedEta", 100,-1.5,1.5);
    TH1F * h_rawseedPhi = new TH1F("h_rawseedPhi", "h_rawseedPhi", 100,-TMath::Pi(),TMath::Pi());
    TH1F * h_subseedPhi = new TH1F("h_subseedPhi", "h_subseedPhi", 100,-TMath::Pi(),TMath::Pi());
    TH1F * h_rawseedCut = new TH1F("h_rawseedCut", "h_rawseedCut", 10, 0, 10);
    TH1F * h_subseedCut = new TH1F("h_subseedCut", "h_subseedCut", 10, 0, 10);
    TH1F * h_tower_pt = new TH1F("h_tower_pt", "h_tower_pt", 100, -50, 50);
    TH1F * h_tower_iter_pt = new TH1F("h_tower_iter_pt", "h_tower_iter_pt", 100, -50, 50);
    TH1F * h_tower_rand_pt = new TH1F("h_tower_rand_pt", "h_tower_rand_pt", 100, -50, 50);
    TH1F * h_tower_iter_rand_pt = new TH1F("h_tower_iter_rand_pt", "h_tower_iter_rand_pt", 100, -50, 50);

    // n seed plots
    TH1F * h_nseed = new TH1F("h_nseed", "h_nseed", 100, 0, 100);
    TH1F * h_nsubseed = new TH1F("h_nsubseed", "h_nsubseed", 100, 0, 100);


    // loop over events
    for(int i = 0; i < nentries; ++i)
    {
        t->GetEntry(i);

      
        for(int j =0; j <  m_rawseedPt->size(); j++) h_rawseedPt->Fill(m_rawseedPt->at(j));
        for(int j =0; j<m_subseedPt->size(); j++) h_subseedPt->Fill(m_subseedPt->at(j));
        for(int j =0; j<m_rawseedEta->size(); j++) h_rawseedEta->Fill(m_rawseedEta->at(j));
        for(int j =0; j<m_subseedEta->size(); j++) h_subseedEta->Fill(m_subseedEta->at(j));
        for(int j =0; j<m_rawseedPhi->size(); j++) h_rawseedPhi->Fill(m_rawseedPhi->at(j));
        for(int j =0; j<m_subseedPhi->size(); j++) h_subseedPhi->Fill(m_subseedPhi->at(j));
        for(int j =0; j<m_rawseedCut->size(); j++) h_rawseedCut->Fill(m_rawseedCut->at(j));
        for(int j =0; j<m_subseedCut->size(); j++) h_subseedCut->Fill(m_subseedCut->at(j));
        for(int j =0; j<m_tower_pt->size(); j++) h_tower_pt->Fill(m_tower_pt->at(j));
        for(int j =0; j<m_tower_iter_pt->size(); j++) h_tower_iter_pt->Fill(m_tower_iter_pt->at(j));
        for(int j =0; j<m_tower_rand_pt->size(); j++) h_tower_rand_pt->Fill(m_tower_rand_pt->at(j));
        for(int j =0; j<m_tower_iter_rand_pt->size(); j++) h_tower_iter_rand_pt->Fill(m_tower_iter_rand_pt->at(j));
    


        h_nseed->Fill(m_rawseedPt->size());
        h_nsubseed->Fill(m_subseedPt->size());

    }

    // create output file
    TFile *fout = new TFile(output_file, "RECREATE");

    // write histograms
    h_rawseedPt->Write();
    h_subseedPt->Write();
    h_rawseedEta->Write();
    h_subseedEta->Write();
    h_rawseedPhi->Write();
    h_subseedPhi->Write();
    h_rawseedCut->Write();
    h_subseedCut->Write();
    h_tower_pt->Write();
    h_tower_iter_pt->Write();
    h_tower_rand_pt->Write();
    h_tower_iter_rand_pt->Write();
    h_nseed->Write();
    h_nsubseed->Write();

    // close input file
    f->Close();

    // close output file
    fout->Close();

      
    return 0;
}

