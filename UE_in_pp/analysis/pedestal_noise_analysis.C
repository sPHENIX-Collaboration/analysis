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
#include <TProfile2D.h>

using namespace std;

const int cemcSize = 24576;
const int ihcalSize = 1536;
const int ohcalSize = 1536;
const int mbdSize = 256;
const int g4Size = 100000;
const int vtxSize = 3;

void pedestal_noise_analysis(int i = 0) {

	string filename = "pedestal_output/pedestal_noise_analysis_run54256_" + to_string(i) + ".root";
	TFile *out = new TFile(filename.c_str(),"RECREATE");

	TH1F* h_emcal = new TH1F("h_emcal","",2000,-20,20);
	TH1F* h_ihcal = new TH1F("h_ihcal","",2000,-5,5);
	TH1F* h_ohcal = new TH1F("h_ohcal","",2000,-20,20);

  	TProfile2D* h_2D_ihcal = new TProfile2D("h_2D_ihcal","",24,0.,24.,64,0.,64.);
  	TProfile2D* h_2D_ohcal = new TProfile2D("h_2D_ohcal","",24,0.,24.,64,0.,64.);
  	TProfile2D* h_2D_emcal = new TProfile2D("h_2D_emcal","",96,0.,96.,256,0.,256.);
  	TProfile2D* h_2D_emcal_chi2 = new TProfile2D("h_2D_emcal_chi2","",96,0.,96.,256,0.,256.);
  	TProfile2D* h_2D_ihcal_chi2 = new TProfile2D("h_2D_ihcal_chi2","",24,0.,24.,64,0.,64.);
  	TProfile2D* h_2D_ohcal_chi2 = new TProfile2D("h_2D_ohcal_chi2","",24,0.,24.,64,0.,64.);

  	h_2D_ihcal->SetErrorOption("s");
  	h_2D_ohcal->SetErrorOption("s");
  	h_2D_emcal->SetErrorOption("s");
  	h_2D_ihcal_chi2->SetErrorOption("s");
  	h_2D_ohcal_chi2->SetErrorOption("s");
  	h_2D_emcal_chi2->SetErrorOption("s");
	  
    TChain chain("ttree");

    const char* inputDirectory = "/sphenix/tg/tg01/commissioning/CaloCalibWG/egm2153/detdeta_run24auau/";
	TString wildcardPath = TString::Format("%sevents_54256_pedestal_unc_%d.root", inputDirectory, i);
	chain.Add(wildcardPath);

    int m_simtwrmult_cemc;
    float m_simtwr_cemc_e[cemcSize];
    float m_simtwr_cemc_zs_e[cemcSize];
    int m_simtwr_cemc_ieta[cemcSize];
    int m_simtwr_cemc_iphi[cemcSize];
    float m_simtwr_cemc_eta[cemcSize];
    int m_simtwr_cemc_adc[cemcSize];
    int m_simtwr_cemc_zs_adc[cemcSize];
    float m_simtwr_cemc_time[cemcSize];
    float m_simtwr_cemc_chi2[cemcSize];
    int m_simtwrmult_ihcal;
    float m_simtwr_ihcal_e[ihcalSize];
    float m_simtwr_ihcal_zs_e[ihcalSize];
    int m_simtwr_ihcal_ieta[ihcalSize];
    int m_simtwr_ihcal_iphi[ihcalSize];
    float m_simtwr_ihcal_eta[ihcalSize];
    int m_simtwr_ihcal_adc[ihcalSize];
    int m_simtwr_ihcal_zs_adc[ihcalSize];
    float m_simtwr_ihcal_time[ihcalSize];
    float m_simtwr_ihcal_chi2[ihcalSize];
    int m_simtwrmult_ohcal;
    float m_simtwr_ohcal_e[ohcalSize];
    float m_simtwr_ohcal_zs_e[ohcalSize];
    int m_simtwr_ohcal_ieta[ohcalSize];
    int m_simtwr_ohcal_iphi[ohcalSize];
    float m_simtwr_ohcal_eta[ohcalSize];
    int m_simtwr_ohcal_adc[ohcalSize];
    int m_simtwr_ohcal_zs_adc[ohcalSize];
    float m_simtwr_ohcal_time[ohcalSize];
    float m_simtwr_ohcal_chi2[ohcalSize];

     // Set branch addresses
    int use_emcal = 1;
    int use_hcal = 1;
    if (use_emcal) {
	    chain.SetBranchAddress("sectorem", &m_simtwrmult_cemc);
	    chain.SetBranchAddress("emcalen", m_simtwr_cemc_e);
	    chain.SetBranchAddress("emcaletabin", m_simtwr_cemc_ieta);
	    chain.SetBranchAddress("emcalphibin", m_simtwr_cemc_iphi);
		chain.SetBranchAddress("emchi2", m_simtwr_cemc_chi2);
	}
	if (use_hcal) {
	    chain.SetBranchAddress("sectorih", &m_simtwrmult_ihcal);
	    chain.SetBranchAddress("ihcalen", m_simtwr_ihcal_e);
	    chain.SetBranchAddress("ihcaletabin", m_simtwr_ihcal_ieta);
	    chain.SetBranchAddress("ihcalphibin", m_simtwr_ihcal_iphi);
	    chain.SetBranchAddress("ihchi2", m_simtwr_ihcal_chi2);

	    chain.SetBranchAddress("sectoroh", &m_simtwrmult_ohcal);
	    chain.SetBranchAddress("ohcalen", m_simtwr_ohcal_e);
	    chain.SetBranchAddress("ohcaletabin", m_simtwr_ohcal_ieta);
	    chain.SetBranchAddress("ohcalphibin", m_simtwr_ohcal_iphi);
	    chain.SetBranchAddress("ohchi2", m_simtwr_ohcal_chi2);
	}

	int eventnumber = 0;
	float totalweights = 0.0;

    Long64_t nEntries = chain.GetEntries();
    std::cout << nEntries << std::endl;
    for (Long64_t entry = 0; entry < nEntries; ++entry) {
        chain.GetEntry(entry);
    	if (eventnumber % 1000 == 0) cout << "event " << eventnumber << endl;

  		eventnumber++;
		if (use_emcal) {
			for (int i = 0; i < m_simtwrmult_cemc; i++) {
				h_emcal->Fill(m_simtwr_cemc_e[i]);
				h_2D_emcal->Fill(m_simtwr_cemc_ieta[i], m_simtwr_cemc_iphi[i], m_simtwr_cemc_e[i]);
				h_2D_emcal_chi2->Fill(m_simtwr_cemc_ieta[i], m_simtwr_cemc_iphi[i], m_simtwr_cemc_chi2[i]);
			}
		}
		if (use_hcal) {
			for (int i = 0; i < m_simtwrmult_ihcal; i++) {
				h_ihcal->Fill(m_simtwr_ihcal_e[i]);
				h_2D_ihcal->Fill(m_simtwr_ihcal_ieta[i], m_simtwr_ihcal_iphi[i], m_simtwr_ihcal_e[i]);
				h_2D_ihcal_chi2->Fill(m_simtwr_ihcal_ieta[i], m_simtwr_ihcal_iphi[i], m_simtwr_ihcal_chi2[i]);
			}

			for (int i = 0; i < m_simtwrmult_ohcal; i++) {
				h_ohcal->Fill(m_simtwr_ohcal_e[i]);
		    	h_2D_ohcal->Fill(m_simtwr_ohcal_ieta[i], m_simtwr_ohcal_iphi[i], m_simtwr_ohcal_e[i]);
				h_2D_ohcal_chi2->Fill(m_simtwr_ohcal_ieta[i], m_simtwr_ohcal_iphi[i], m_simtwr_ohcal_chi2[i]);
			}
		}
		
	}
	
	out->Write();
	out->Close();


}
