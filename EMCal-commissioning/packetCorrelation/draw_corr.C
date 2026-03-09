#include "TFile.h"
#include "TTree.h"

#include "TStyle.h"
#include "TLatex.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TCanvas.h"

#include <iostream>
#include <iomanip>
#include <vector>
#include <tuple>
#include <string>
#include <cmath>
#include <algorithm>


int round8(float num) {
      int inum = static_cast<int>(num);
        return inum - inum % 8;
}

void draw_corr() {
    const char * run = "../9613-0000.root";
    const char * runNum = "EMCal run 9613";

    // Are your tree variables named correctly??

    TFile *f = new TFile(run,"READ");

    f->ls();

    TTree *t = (TTree*) f->Get("towerntup");
    const int entries = t->GetEntries();    


    t->Show(0);

    std::vector<float> *emcTowE = 0;
    std::vector<float> *emciEta = 0;
    std::vector<float> *emciPhi = 0;
  
    t->SetBranchAddress("energy",&emcTowE);
    t->SetBranchAddress("etabin",&emciEta);
    t->SetBranchAddress("phibin",&emciPhi);
   
    TH2D *EDist = new TH2D("EDist",";EMCal tower phi;EMCal tower eta",256,-.5,255.5,96,-.5,95.5);
    TH2D *EDist_gap = new TH2D("EDist_gap",";EMCal tower phi;EMCal tower eta",256,-.5,255.5,96,-.5,95.5);
    TH2D* packet_corrs[128][128];
    for (int i = 0; i < 128; i++) {
        for (int j = 0; j < 128; j++) {
            std::string corrIndex = std::to_string(i) + "-" + std::to_string(j);
            const char * ccorrIndex = corrIndex.c_str();
            packet_corrs[i][j] = new TH2D(ccorrIndex,";Energy;Energy",256,0,20000,256,0,20000);
        }
    }

    auto packetE = new float[entries][128];
    TH2D *correlations = new TH2D("correlations",";packets",128,0,128,128,0,128);

    float tower_E[256 * 96] = { 0 }; // variables used for average energies
    float tower_ET[256 * 96] = { 0 };
    
    TCanvas *tc = new TCanvas();
    gStyle->SetOptStat(0);
    TLatex l;
    l.SetNDC();
    l.SetTextSize(0.03);


    
    std::cout << "I see " << entries << " events!" << std::endl;

    for (int e = 0 ; e < entries; e++ ) {
      t->GetEntry( e );

      for (unsigned i = 0; i < emcTowE->size(); i++) {
           tower_E[i] += emcTowE->at(i);
      }

      if ( e % 1000 == 0 ) {
        std::cout << "\r" << static_cast<int>(static_cast<float>(e) / entries * 100) << "%" << std::flush;
      }
    }
    std::cout << endl;

    // Find the various hot/cold spots
    std::cout << endl << "Finding hot/cold spots..." << endl;

    TH1D *h_tower_E = new TH1D("h_tower_E",";Tower energy;counts",200,0,200000);
    TH1D *h_tower_E_top = new TH1D("h_tower_E_top",";Tower energy;counts",200,0,200000);
    
    float sumE = 0.0;
    float hottest = 0.0;
    for (int i = 0; i < 256 * 96; i++) {
        if (emciEta->at(i) >= 80) {
           h_tower_E_top->Fill(tower_E[i]);
        } 
        h_tower_E->Fill(tower_E[i]);
        sumE += tower_E[i];
        if (tower_E[i] > hottest) {
            hottest = tower_E[i];
        }
    }
    float aveE = sumE / (256 * 96);
    float stdDev = h_tower_E->GetStdDev();
    
    std::vector<int> hot_spots;
    std::vector<int> warm_spots;
    std::vector<int> cold_spots;
    std::vector<int> cool_spots;
    std::vector<int> normal_spots;
    
    float cool_tol = 0.65;

    for (int i = 0; i < 256 * 96; i++) {
       
        if (tower_E[i] > (aveE+stdDev*3) && tower_E[i + 1] < (aveE+stdDev*3) && tower_E[i - 1] < (aveE+stdDev*3)) {
           hot_spots.push_back(i);
        }
        else if (tower_E[i] > (aveE + stdDev*3)) {
           warm_spots.push_back(i);
        }
        else if (tower_E[i] < aveE * cool_tol && tower_E[i + 1] > aveE * cool_tol && tower_E[i - 1] > aveE * cool_tol) {
           cold_spots.push_back(i);
        }
        else if (tower_E[i] < aveE * cool_tol) {
           cool_spots.push_back(i);
        }
        else {
           normal_spots.push_back(i);
        }
    }

    gPad->SetLogy(1);
    h_tower_E->Draw();
    l.DrawLatex(0.15,0.95,"sPHENIX Internal");
    l.DrawLatex(0.15,0.91,runNum);
    tc->Print("tower_E_1D.pdf");

    h_tower_E_top->Draw();
    l.DrawLatex(0.15,0.95,"sPHENIX Internal");
    l.DrawLatex(0.15,0.91,runNum);
    tc->Print("tower_E_top_1D.pdf");
    gPad->SetLogy(0);


    // Print out the locations of the very hot/cold spots

     std::cout << "hot spots" << endl;
     for (unsigned i = 0; i < hot_spots.size(); i++) {
       std::cout << "(ieta, iphi) = (";
       std::cout << emciEta->at(hot_spots.at(i)) << ", " << emciPhi->at(hot_spots.at(i)) << ") " << endl;
     }
     std::cout << endl << "warm spots" << endl;
     for (unsigned i = 0; i < warm_spots.size(); i++ ) {
         std::cout << "(ieta, iphi) = (";
         std::cout << emciEta->at(warm_spots.at(i)) << ", " << emciPhi->at(warm_spots.at(i)) << ") " << endl;
     }
     std::cout << endl << "cold spots" << endl;
     for (unsigned i = 0; i < cold_spots.size(); i++) {
       std::cout << "(ieta, iphi) = (";
       std::cout << emciEta->at(cold_spots.at(i)) << ", " << emciPhi->at(cold_spots.at(i)) << ") " << endl;
     }
     std::cout << endl << "cool spots" << endl;
     std::vector<std::tuple<int,int>> cool_points;

     for (unsigned i = 0; i < cool_spots.size(); i++) {
       int rEta = round8(emciEta->at(cool_spots.at(i)));
       int rPhi = round8(emciPhi->at(cool_spots.at(i)));

       if (rEta != 0 &&
           std::find(cool_points.begin(), cool_points.end(), std::make_tuple(rEta,rPhi)) == cool_points.end()) {
         cool_points.push_back(std::make_tuple(rEta, rPhi));
         std::cout << "(ieta, iphi) = (" << rEta << ", " << rPhi << ")" << endl;
       }
     }

     bool skip = false;
     for (unsigned i = 0; i < emcTowE->size(); i++ ) {

       EDist->Fill( emciPhi->at(i), emciEta->at(i), tower_E[i] );

       for (unsigned j = 0; j < hot_spots.size(); j++ ) {
          if (i == static_cast<unsigned>(hot_spots.at(j))) {
             skip = true;
           }
        }
        for (unsigned j = 0; j < warm_spots.size(); j++ ) {
          if (i == static_cast<unsigned>(warm_spots.at(j))) {
             skip = true;
          }
        }

        if (!skip) {
          EDist_gap->Fill(emciPhi->at(i), emciEta->at(i), tower_E[i]);
        }
        skip = false;
    }

    gPad->SetLogz(1);
    EDist->Draw("colz");
    l.DrawLatex(0.15,0.95,"sPHENIX Internal");
    l.DrawLatex(0.15,0.91,runNum);
    l.SetTextSize(0.035);
    l.DrawLatex(0.65,0.92,"#Sigma_{evt} ADC");
    l.SetTextSize(0.03);
    tc->Print("EDist.pdf");
    gPad->SetLogz(0);

    EDist_gap->Draw("colz");
    l.DrawLatex(0.15,0.95,"sPHENIX Internal");
    l.DrawLatex(0.15,0.91,runNum);
    l.SetTextSize(0.035);
    l.DrawLatex(0.65,0.92,"#Sigma_{evt} ADC");
    l.SetTextSize(0.03);
    tc->Print("EDistGap.pdf");


    // packet correlations
    
    std::cout << endl << "Finding packet energies..." << endl;
    bool hot = false;
    int ieta = 0;
    int iphi = 0;
    int packet = 0;
    for (int e = 0; e < entries; e++) {

        t->GetEntry(e);

        for (unsigned i = 0; i < emcTowE->size(); i++) {
            ieta = emciEta->at(i);
            iphi = emciPhi->at(i);
            for (unsigned j = 0; j < hot_spots.size(); j++ ){
                if (ieta == emciEta->at(hot_spots.at(j)) && iphi == emciPhi->at(hot_spots.at(j))) {
                    hot = true;
                }
            }
            for (unsigned j = 0; j < warm_spots.size(); j++ ) {
                if (ieta == emciEta->at(warm_spots.at(j)) && iphi == emciPhi->at(warm_spots.at(j))) {
                    hot = true;
                }
            }

            packet += iphi / 8 * 4;
            if (ieta / 24 == 0) { packet += ieta / 24 + 1; }
            else if (ieta / 24 == 1) { packet += ieta / 24 - 1; }
            else { packet += ieta / 24; }

            if (!hot) { packetE[e][packet] += emcTowE->at(i); } // 24 = 96/4 and 8 = 256/32
            hot = false;
            packet = 0;
        }
        if ( e % 1000 == 0 ) {
           std::cout << "\r" << static_cast<int>(static_cast<float>(e) / entries * 100) << "%" << std::flush;
        }
    }
    std::cout << endl;

    std::cout << endl << "Finding correlations..." << endl;

    for (int e = 0; e < entries; e++ ) {
        for (int i = 0; i < 128; i++) {
            for (int j = 0; j < 128; j++) {
                packet_corrs[i][j]->Fill(packetE[e][i],packetE[e][j]);
                correlations->Fill(i,j,packet_corrs[i][j]->GetCorrelationFactor() / entries);
            }
        }
        if ( e % 1000 == 0 ) {
           std::cout << "\r" << static_cast<int>(static_cast<float>(e) / entries * 100) << "%" << std::flush;
        }
    }
    std::cout << endl;
    
   
    correlations->SetMinimum(.7);
    correlations->Draw("colz");
    l.DrawLatex(0.15,0.95,"sPHENIX Internal");
    l.DrawLatex(0.15,0.91,runNum);
    l.SetTextSize(0.035);
    l.DrawLatex(0.65,0.92,"Packet Correlations");
    l.SetTextSize(0.03);
    tc->Print("packet_correlations.pdf");

    bool stop = false;
    std::string quit = "";
    std::cout << "Would you like to see a specific correlation? (yes/no)" << endl;
    std::cin >> quit;
    if (quit == "no") { stop = true; }
    while (!stop) {
        std::cout << "Which correlation would you like to see? (i j)" << endl;
        int ans1 = 0;
        int ans2 = 0;
        std::cin >> ans1 >> ans2;

        std::string title = "Packet Correlations " + std::to_string(ans1) + "-" + std::to_string(ans2);
        std::cout << title << endl;
        const char * ctitle = title.c_str();
   
        packet_corrs[ans1][ans2]->Draw("colz");
        l.DrawLatex(0.15,0.95,"sPHENIX Internal");
        l.DrawLatex(0.15,0.91,runNum);
        l.SetTextSize(0.035);
        l.DrawLatex(0.65,0.92,ctitle);
        l.SetTextSize(0.03);
        tc->Print("specific_correlation.pdf");

        std::cout << "Would you like to keep going? (yes/no) " << endl;
        std::cin >> quit;
        if (quit == "no") { 
            stop = true; 
        }
    }
}
