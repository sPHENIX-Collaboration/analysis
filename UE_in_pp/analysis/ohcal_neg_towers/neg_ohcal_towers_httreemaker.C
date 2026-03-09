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
#include <assert.h>

using namespace std;

void neg_ohcal_towers_httreemaker() {

	const char *outfile = "neg_ohcal_towers_49219_0_httree.root";
	TTree* hot_towers = new TTree("hot_towers","");
	std::vector<int>ht_channel;
	hot_towers->Branch("ht_channel", &ht_channel);
	ht_channel = {1311, 1184, 574, 405, 331, 602, 382, 994, 1073, 26};
	hot_towers->Fill();
	TFile *out = new TFile(outfile,"RECREATE");
	out->cd();
	hot_towers->Write();
	out->Close();
}
