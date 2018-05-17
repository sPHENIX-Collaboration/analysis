#include "TROOT.h"
#include "TClass.h"
#include "TGraph.h"
#include "TF1.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TH3D.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TBranch.h"
#include "Riostream.h"
#include "TStyle.h"
#include "TFile.h"
#include "TString.h"
#include "TLegend.h"
#include "TRandom3.h"
#include "TMath.h"
#include "math.h"
#include "TColor.h"
#include <vector>
#include <sstream>
#include <algorithm>

#include <cstdlib>
#include "TMath.h"
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <cmath>
#include "TGraph.h"
#include "TGraph2D.h"
#include <algorithm>


/* 
   Write out jet variables to csv file 
   to be used for machine learning 

   written by sean.jeffas@stonybrook.edu
*/

int MachineLearning_CSV()
{

  
  ofstream myfile;	  
   
  string filename;

  // Names to loop over different types of files  
  string seed[10] = {"1","2","3","4","5","6","7","8","9","10"};
  string type[2] = {"3pion","SM"};

  filename = "./data/JetSummary_p250_e20_1000events_r05.csv";
  myfile.open(filename.c_str());
  
  // Loop over all LQ, NC, and CC geant files
  for(int a = 0; a<10; a++){
    for(int b=0; b<2; b++){
      
      const std::string inFile = "LeptoAna_p250_e20_1000events_"+seed[a]+"seed_"+type[b]+"_r05.root";    
      const std::string inDirectory = "/gpfs/mnt/gpfs02/phenix/scratch/spjeffas/data/";
      std::string inputFile = inDirectory+inFile;
      
      TFile *f = TFile::Open(inputFile.c_str());
      TTree *t = (TTree*)f->Get("event");
      
      const int Nevent = t->GetEntries();
      
      // Variables for different jet characteristics
      vector<float> * tracks_rmax;
      vector<float> * tracks_count;
      vector<float> * tracks_chargesum;
      vector<float> * tracks_vertex;
      vector<float> * jetshape_radius;
      vector<float> * jetshape_econe_1;
      vector<float> * jetshape_econe_2;
      vector<float> * jetshape_econe_5;
      vector<float> * jet_eta;
      vector<float> * jet_minv;
      vector<float> * jet_etotal;
      vector<float> * jet_ptrans;
      vector<int> * evtgen_pid;
      
      //point to variables in tree
      t->SetBranchAddress("tracks_rmax_R",&tracks_rmax);
      t->SetBranchAddress("tracks_count_R",&tracks_count);
      t->SetBranchAddress("tracks_chargesum_R",&tracks_chargesum);
      t->SetBranchAddress("tracks_vertex",&tracks_vertex);
      t->SetBranchAddress("jetshape_radius",&jetshape_radius);
      t->SetBranchAddress("jetshape_econe_r01",&jetshape_econe_1);
      t->SetBranchAddress("jetshape_econe_r02",&jetshape_econe_2);
      t->SetBranchAddress("jetshape_econe_r05",&jetshape_econe_5);
      t->SetBranchAddress("jet_eta",&jet_eta);
      t->SetBranchAddress("jet_minv",&jet_minv);
      t->SetBranchAddress("jet_etotal",&jet_etotal);
      t->SetBranchAddress("evtgen_pid",&evtgen_pid);
      t->SetBranchAddress("jet_ptrans",&jet_ptrans);
      
      
      //loop over all events
      for(int i = 0; i < Nevent; i++)
	{
	  //Get entry for each event
	  t->GetEntry(i);
	  
	  for(int l=0; l < tracks_rmax->size(); l++){	
	    
	    // Get variables for each jet in event
	    double rmax = (*tracks_rmax)[l];
	    int count = (*tracks_count)[l];
	    int chargesum = (*tracks_chargesum)[l];
	    double vertex = (*tracks_vertex)[l];
	    double radius = (*jetshape_radius)[l];
	    double econe_1 = (*jetshape_econe_1)[l];
	    double econe_2 = (*jetshape_econe_2)[l];
	    double econe_5 = (*jetshape_econe_5)[l];
	    double eta = (*jet_eta)[l];
	    double minv = (*jet_minv)[l];
	    double etotal = (*jet_etotal)[l];
	    double ptrans = (*jet_ptrans)[l];
	    int pid = (*evtgen_pid)[l];
	
	    if(ptrans < 5) continue;
    
	    // If LQ then tag as LQ in csv file
	    if(b == 0 && pid == 15 && vertex == vertex) myfile << count << "," << chargesum << "," << eta << "," << vertex << "," << "tau" << endl;
	    
	    //If SM then tag as SM in csv file
	    if(b != 0 && pid != 15 && pid != 11 && vertex == vertex) myfile << count << "," << chargesum << "," << eta << "," << vertex << "," << "DIS" << endl;
	  }
	}
    }
  }
  
  myfile.close();
      



  return 0;
}
