#include <iostream>
#include <TH2D.h>
#include <TH1D.h>
#include <TChain.h>
#include <TMath.h>
#include <TTree.h>
#include <TFile.h>
#include <sstream> //std::ostringstsream
#include <fstream> //std::ifstream
#include <iostream> //std::cout, std::endl
#include <cmath>
#include <TGraphErrors.h>
#include <TGraph.h>
#include <TSpectrum.h>
#include <TF1.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>
#include <string>
#include <set>
#include <TVector3.h>
#include <map>
#include <vector>
#include <TDatabasePDG.h>
#include <tuple>
#include <TProfile.h>
#include <TProfile2D.h>
#include "TH1D.h"

using namespace std;

std::vector<int> run_numbers;
std::vector<double> collision_rates;
std::vector<double> pileup_rates;
//std::vector<double> pileup_bins = {0.0,0.015};
std::vector<double> pileup_bins = {0.0,0.015,0.02,0.028,0.05};

void get_pileup_rate() {

    std::ifstream infile("0mrad_collision_rates.txt");
    if (!infile) {
        std::cerr << "Error: Unable to open input file!" << std::endl;
        return;
    }
    std::string line;
    while (std::getline(infile, line)) {
        std::istringstream iss(line);
        int run_number;
        double collision_rate;  
        if (!(iss >> run_number >> collision_rate)) {
            std::cerr << "Error: Malformed line in input file!" << std::endl;
            continue;
        }
        std::cout << "run_number " << run_number << " collision_rate " << collision_rate << std::endl;
        run_numbers.push_back(run_number);
        collision_rates.push_back(collision_rate);

        // Calculate lambda
        double denominator = 78000.0 * 111.0;
        double lambda = collision_rate / denominator;
        double exp_neg_lambda = exp(-lambda);
        
        // Calculate pk
        double pk = 1 - lambda * exp_neg_lambda - exp_neg_lambda;
        
        // Calculate pileup rate
        double pileup_rate = pk / (pk + lambda * exp_neg_lambda);
        pileup_rates.push_back(pileup_rate);
    }
    infile.close();

    TH1F* h_pileup = new TH1F("h_pileup","",100,0,0.12);

    for (auto p: pileup_rates) {
    	h_pileup->Fill(p);
    }

    TCanvas* canvas = new TCanvas("canvas","",600,500);
    h_pileup->Draw("hist");
    canvas->Draw();

    for (int i = 0; i < pileup_rates.size(); i++) {
    	std::cout << "run_number " << run_numbers[i] << " pileup_rate " << pileup_rates[i] << std::endl;
    }

    
}
