//sPHENIX Sector Mapping of modules R1 R2 and R3 May 19, 2023
//Jennifer James
#include "TString.h"
#include "TFile.h"
#include "TSystem.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH3I.h"
#include "TCanvas.h"
#include "TNtuple.h"
#include <iostream>

// forward declaration
void Ana( TNtuple*, TNtuple*, TNtuple*, char[2], int, double tot[24], double dead[24], int );

void SectorMap_Display( )
{

  char name[100];
  int run_num;

  cout << "Input run number: ";
  cin >> run_num;

  //string runNumber = "pedestal-00010616";

  const char* run_str[24] =  { "00", "01", "02", "03" ,"04", "05", "06", "07", "08", "09", "10" , "11", "12" , "13" , "14", "15" , "16" , "17", "18" , "19", "20", "21" , "22", "23"  };
  double tot[24] = {0};
  double dead[24] = {0};

  double tot_ult[24]={0};
  double dead_ult[24]={0};

  sprintf(name, "pedestal-%d-outfile.root", run_num);

  //TFile h_outfile10305("pedestal-10616-outfile.root", "RECREATE");
  TFile h_outfile10305(name, "RECREATE");
  TNtuple *h_Alive=new TNtuple("h_Alive","Location Tntuple Cuts","chan_id:fee_id:module_id:pedMean:pedStdi:sec_id");
  TNtuple *h_AliveTot=new TNtuple("h_AliveTot","Location Tntuple No Cuts","chan_id:fee_id:module_id:pedMean:pedStdi:sec_id");
  TNtuple *h_AliveNoise=new TNtuple("h_AliveNoise","Location Tntuple Cuts","chan_id:fee_id:module_id:pedMean:pedStdi:sec_id:noise");

  for( int i = 0; i < 24; i++ )
  {
    char run[2]; //define character string
    std::sprintf(run,run_str[i]); //print run_str values indexed at i<72

    Ana( h_Alive, h_AliveTot,h_AliveNoise, run, run_num, tot, dead, i); //create Ana function

    tot_ult[i] += tot[i]; //find total channels
    dead_ult[i] += dead[i]; //find dead channels
  }

  h_outfile10305.cd();
  h_Alive->Write();
  h_AliveTot->Write();
  h_outfile10305.Close();

 }

// Define Ana function 

void Ana(TNtuple *Alive, TNtuple *AliveTot, TNtuple *AliveNoise, char run[2] = "11", int runNumber = 00000, double tot[24]={0}, double dead[24]={0}, int j = 0)
{ // Ana starts     
     
  //const TString filename2( Form( "/sphenix/u/llegnosky/Livechan_Pedestals_Noise/outputfile_TPC_ebdc%s_pedestal-00010616.root", run) );
  const TString filename2( Form( "/sphenix/u/llegnosky/Livechan_Pedestals_Noise/run_%d/outputfile_TPC_ebdc%s_pedestal-000%d.root", runNumber,run,runNumber) );

  TFile* infile2 = TFile::Open(filename2);

  if(!infile2) return;

  TTree *treesd;  
  treesd = (TTree*) infile2->Get("outputTree");

  Int_t isAlive, chan, fee, module, slot;
  Float_t pedMean, pedStdi;

  treesd->SetBranchAddress("isAlive",&isAlive);
  treesd->SetBranchAddress("chan",&chan);
  treesd->SetBranchAddress("fee",&fee);
  treesd->SetBranchAddress("module",&module);
  treesd->SetBranchAddress("slot", &slot);
  treesd->SetBranchAddress("pedMean",&pedMean);
  treesd->SetBranchAddress("pedStdi",&pedStdi);

  std::cout << "sector = "<< run <<std::endl;

  UInt_t nEntries = treesd->GetEntries();
  std::string fs(run);
  float sec = std::stof(fs); 

  std::cout << "sector from char = " << sec << std::endl;  
 
  // defining a cut of the mean and standard deviation
   for (int i=0;i<nEntries;i++){
     treesd->GetEntry(i); 

     double pedestal = pedMean;
     double noise = pedStdi;
     
      AliveTot->Fill(1.0*chan,1.0*fee,1.0*module,pedMean,pedStdi,1.0*sec);

      if (isAlive == 1){
	Alive->Fill(1.0*chan,1.0*fee,1.0*module,pedMean, pedStdi, 1.0*sec);            
      }
   }
}
