///////////////////////////////////////////////////////////
//         Jennifer James, Charles Hughes                //
//          derived from files created by                //
//         Aditya Dash and Thomas Marshall               //
//                  May 19, 2023                         //
///////////////////////////////////////////////////////////

// includes
#ifndef __CINT__
#include <algorithm>
#include <cmath>
#include "TCanvas.h"
#include "TApplication.h"
#include "TH1D.h"
#include "TH1F.h"
#include "TH2D.h"
#include "TH2F.h"
#include "TH3D.h"
#include "TH3F.h"
#include "TString.h"
#include "TStyle.h"
#include "TFile.h"
#include "TGraphPolar.h"
#include "TGraphPolargram.h"
#include "TAxis.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TApplication.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TVirtualFitter.h"
#include "Math/MinimizerOptions.h"
#include "TFitResultPtr.h"
#include "TFitResult.h"
#include "TF1.h"
#include "TPaveLabel.h"
#include <string.h>
#include <vector>
#include <iostream>
#include <RtypesCore.h>
#endif

using namespace std;

void Locate(Int_t id, Bool_t is_ASIDE, Double_t *rbin, Double_t *thbin);


void Noise_ModuleDisplay(){

  //  std::vector <pair<int,int>> vec1;

  bool skip_empty_fees = true;

  std::vector<pair<int,int>> vec; 

  char name[100];
  int run_num;

  cout << "Input run number: ";
  cin >> run_num;

  const TString filename3( Form( "/sphenix/u/llegnosky/Livechan_Pedestals_Noise/run_%d/pedestal-%d-outfile.root",run_num,run_num) );
  //const TString filename3( Form( "/sphenix/u/jamesj3j3/workfest_Charles_mistake/sPHENIXProjects/pedestal-10131-outfile.root") );

  //    std::cout << "Analyze - filename2: " << filename2 << std::endl;
  //
  TFile *infile3 = TFile::Open(filename3);
  //        TFile* infile2 = TFile::Open(filename2);
  //
  if(!infile3) return;

  TNtuple * liveTntuple ;
  liveTntuple = (TNtuple*) infile3->Get("h_Alive");

  TNtuple * totTntuple ;
  totTntuple = (TNtuple*) infile3->Get("h_AliveTot");
  
  TH3F * dm2 = new TH3F("dm2","TPC map",26, -0.5, 25.5, 24, -0.5, 23.5, 3, 0.5, 3.5 );
  liveTntuple->Draw("module_id:sec_id:fee_id>>dm2","",""); //x axis = FEE ID, y axis = SEC ID, z axis = MODULE ID

  TH3F * h_AlivePedestalNoise = new TH3F("h_AlivePedestalNoise","TPC Alive Pedestal Noise of Std. Dev,",26, -0.5, 25.5, 24, -0.5, 23.5, 3, 0.5, 3.5 );
  liveTntuple->Draw("module_id:sec_id:fee_id>>h_AlivePedestalNoise","pedStdi * (pedStdi < 20)",""); //x axis = FEE ID, y axis = SEC ID, z axis = MODULE ID 

  TH3F * tot = new TH3F("tot"," TPC Tot Map", 26, -0.5, 25.5, 24, -0.5, 23.5, 3, 0.5, 3.5 );
  totTntuple->Draw("module_id:sec_id:fee_id>>tot","",""); //x axis = FEE ID, y axis = SEC ID, z axis = MODULE ID
  //h3->Draw("colz");

  dm2->Print();
  tot->Print(); 

  // std::vector<Float_t> frac_val;
  std::vector<Float_t> sub_arrA;
  std::vector<Float_t> sub_arrC;

  Float_t frac[dm2->GetNbinsX()][dm2->GetNbinsY()]; // array to store fractions                                                         
  std::vector<Float_t> module_std_dev; 
  
  for (Int_t i = 0; i < dm2->GetNbinsY(); i++) { // i is looping over sec ID                                                           
    //    if(fee->GetBinContent(i)<1){
    // continue;
    // }
    for (Int_t j = 0; j < dm2->GetNbinsZ(); j++) { // j is looping over Module ID 

      Float_t num=0; // numerator (live)                                      
      Float_t denom=0; // denominator (total)   
      Float_t sum_noise=0;

      for (Int_t k = 0; k < dm2->GetNbinsX(); k++) { // k is looping over FEE ID                                                      
        
        if( skip_empty_fees){ //if you want to skip empty fees
          if( dm2->GetBinContent(k+1,i+1,j+1) >= 1){ //only add to numerator and denominator if FEE has at least 1 live channel
            num+=dm2->GetBinContent(k+1,i+1,j+1);
            denom+=tot->GetBinContent(k+1,i+1,j+1);
            sum_noise+=h_AlivePedestalNoise->GetBinContent(k+1,i+1,j+1);
          }
        }
        else { //if you don't want to skip empty fees
	  num+=dm2->GetBinContent(k+1,i+1,j+1);
	  denom+=tot->GetBinContent(k+1,i+1,j+1); 
	  sum_noise+=h_AlivePedestalNoise->GetBinContent(k+1,i+1,j+1);
	}
      }      
                                                           
      Float_t frac_val= ( num / denom) * 100.0; // calculate the fraction 
      Float_t noise_value = sum_noise/num;
                                                               
      frac[i+1][j+1] = frac_val; // store fraction in array                                                                              
      std::cout << "Sec ID = " << i+1 << ", Module ID = " << j+1 << ", Live fraction = " << frac_val << "%" << std::endl;
      if (i < 12) {
	sub_arrC.push_back(noise_value);
	//std::cout <<  " Live fraction C = " << sub_arrC.size() << " %" << std::endl;
	//std::cout << "sub_arrC[" << i << "] = " << frac_val << std::endl;
      }else {
	sub_arrA.push_back(noise_value);
	//std::cout << "sub_arrA[" << i << "] = " << frac_val << std::endl;
	//std::cout <<  " Size = " << sub_arrA.size() <<" %" << std::endl;
      }
    }
  }

  //////////////////////////////////////////////////////////////////////// 
  //       A Side   South Label Conventions                                  //  
  ///////////////////////////////////////////////////////////////////////  
  //   1 - 12_R1   16 - 17_R1   31 - 22_R1    
  //   2 - 12_R2   17 - 17_R2   32 - 22_R2   
  //   3 - 12_R3   18 - 17_R3   33 - 22_R3 
  //   4 - 13_R1   19 - 18_R1   34 - 23_R1
  //   5 - 13_R2   20 - 18_R2   35 - 23_R2
  //   6 - 13_R3   21 - 18_R3   36 - 23_R3
  //   7 - 14_R1   22 - 19_R1
  //   8 - 14_R2   23 - 19_R2
  //   9 - 14_R3   24 - 19_R3
  //  10 - 15_R1   25 - 20_R1
  //  11 - 15_R2   26 - 20_R2
  //  12 - 15_R3   27 - 20_R3
  //  13 - 16_R1   28 - 21_R1
  //  14 - 16_R2   29 - 21_R2
  //  15 - 16_R3   30 - 21_R3
  
  ////////////////////////////////////////////////////////////////////////
  //       C Side North Label Conventions                              //
  ///////////////////////////////////////////////////////////////////////
  //   1 - 00_R1   16 - 05_R1   31 - 10_R1    
  //   2 - 00_R2   17 - 05_R2   32 - 10_R2   
  //   3 - 00_R3   18 - 05_R3   33 - 10_R3 
  //   4 - 01_R1   19 - 06_R1   34 - 11_R1
  //   5 - 01_R2   20 - 06_R2   35 - 11_R2
  //   6 - 01_R3   21 - 06_R3   36 - 11_R3
  //   7 - 02_R1   22 - 07_R1
  //   8 - 02_R2   23 - 07_R2
  //   9 - 02_R3   24 - 07_R3
  //  10 - 03_R1   25 - 08_R1
  //  11 - 03_R2   26 - 08_R2
  //  12 - 03_R3   27 - 08_R3
  //  13 - 04_R1   28 - 09_R1
  //  14 - 04_R2   29 - 09_R2
  //  15 - 04_R3   30 - 09_R3


  gStyle->SetOptStat(0);
   
  // if have lists inside input file - create pointer to list
 
  /* // Open up the histograms from the infile and give new names
  THnSparseD* sparse = (THnSparseD *)infile1->Get("histsparse"); //get the sparse object and store it in memory
  if(!sparse){
  cout<<"sparse does not exist in "<<input1<<endl; //double check that it's in the file it should be !
    return;
  }
  sparse->THnSparse::GetAxis(2)->SetRange(1,15); //limit to ONLY real trips
  */ 
  //____________________________________________________________________________________________________// 

  // Heat map Visualization

  const Int_t N_rBins = 4; //(one inner bin NOT to be filled, 2nd bin is R1, 3rd bin is R2, 4th bin is R3)
  const Int_t N_thBins = 12; //(12 theta bins of uniform angle (360/12 = 30 degrees = TMath::Pi()/6 ~= 0.523 rad) )

  Double_t rBin_edges[N_rBins + 1] = {0.0, 0.256, 0.504, 0.752, 1.00}; //variable edges for the radial dimensions

  TGraphPolargram* polardummy1 = new TGraphPolargram("polardummy1",0,1,0,2.*TMath::Pi()); //dummy plots to get the canvas right (not to be filled)
  polardummy1->SetToGrad();
  polardummy1->SetNdivPolar(N_thBins);
  polardummy1->SetLineColor(0);
  polardummy1->SetRadialLabelSize(0);

  TGraphPolargram* polardummy2 = new TGraphPolargram("polardummy2",0,1,0,2.*TMath::Pi());
  polardummy2->SetToGrad();
  polardummy2->SetNdivPolar( N_thBins);
  polardummy2->SetLineColor(0);
  polardummy2->SetRadialLabelSize(0);

  for(Int_t i = 0 ; i < 12 ; i++){ //setting the axis label (CCW from horizontal right axis)
    char labelstr1[128];
    char labelstr2[128];
    if(i<=9){ // i -> [0:9]
      sprintf(labelstr2,"C0%d",i);

      if(i<=6){ // i -> [0:6] (halfway)
	sprintf(labelstr1,"A%d",18-i);
      }
      else if(i>6){ // i -> [7:9]
	sprintf(labelstr1,"A%d",30-i);
      }

    } 
    else { // i -> [10:11]
      sprintf(labelstr2,"C%d",i);
      sprintf(labelstr1,"A%d",30-i);
    }
    polardummy1->SetPolarLabel(i,labelstr1);
    polardummy2->SetPolarLabel(i,labelstr2);
  }

  TH2D* ErrASide = new TH2D( "ASide" , "ADC Counts North Side" , N_thBins, -TMath::Pi()/12. , 23.*TMath::Pi()/12. , N_rBins , rBin_edges ); // X maps to theta, Y maps to R

  TH2D* ErrCSide = new TH2D( "CSide" , "ADC Counts South Side" , N_thBins, -TMath::Pi()/12. , 23.*TMath::Pi()/12. , N_rBins , rBin_edges ); // X maps to theta, Y maps to R


  Double_t r, theta;
  Int_t trip_count_total = 0;
  Bool_t is_ASIDE = true;

  for (Int_t i = 0; i < 36; i++) {
    Locate(i + 1, true, &r, &theta);
    ErrASide->Fill(theta, r, sub_arrA[i]);
    // cout<<"Region # A "<<(i)<<" Alive Fraction = "<<sub_arrA[i]<<endl;   
  }

  for (Int_t i = 0; i < 36; i++) {
    Locate(i + 1,false, &r, &theta);
    ErrCSide->Fill(theta, r, sub_arrC[i]);
    // cout<<"Region # C "<<(i)<<" Alive Fraction = "<<sub_arrC[i]<<endl;   
  }
   
  sprintf(name, "%d-Noise map in ADC unit North Side",run_num);
  TH2D* dummy_his1 = new TH2D("dummy1", name, 100, -1.5, 1.5, 100, -1.5, 1.5);
  //TH2D* dummy_his1 = new TH2D("dummy1", "10616-Noise map in ADC unit North Side", 100, -1.5, 1.5, 100, -1.5, 1.5); //dummy histos for titles
  sprintf(name, "%d-Noise map in ADC unit South Side",run_num);
  TH2D* dummy_his2 = new TH2D("dummy2", name, 100, -1.5, 1.5, 100, -1.5, 1.5);
  //TH2D* dummy_his2 = new TH2D("dummy2", "10616-Noise map in ADC unit South Side", 100, -1.5, 1.5, 100, -1.5, 1.5);
  //TPaveLabels for sector labels
  TPaveLabel* A00 = new TPaveLabel( 1.046586,-0.1938999,1.407997,0.2144871, "18" );
  TPaveLabel* A01 = new TPaveLabel( 0.962076,0.4382608,1.323487,0.8466479 , "17" );
  TPaveLabel* A02 = new TPaveLabel( 0.4801947,0.8802139,0.8416056,1.288601 , "16" );
  TPaveLabel* A03 = new TPaveLabel( -0.1823921,1.011681,0.1790189,1.425662, "15" );
  TPaveLabel* A04 = new TPaveLabel( -0.8449788,0.8690253,-0.4835679,1.288601 , "14" );
  TPaveLabel* A05 = new TPaveLabel( -1.30879,0.441058,-0.9473786,0.8550394 , "13" );
  TPaveLabel* A06 = new TPaveLabel( -1.411009,-0.2050886,-1.049598,0.2144871, "12" );
  TPaveLabel* A07 = new TPaveLabel( -1.302585,-0.7757116,-0.9471979,-0.3561359 , "23" );
  TPaveLabel* A08 = new TPaveLabel( -0.8449788,-1.309971,-0.4835679,-0.8848013 , "22" );
  TPaveLabel* A09 = new TPaveLabel( -0.1823921,-1.426557,0.1790189,-1.006982 , "21" );
  TPaveLabel* A10 = new TPaveLabel( 0.4801947,-1.309076,0.8416056,-0.8839062 , "20" );
  TPaveLabel* A11 = new TPaveLabel( 0.9622567,-0.7785088,1.323668,-0.3533387 , "19" );

  TPaveLabel* C00 = new TPaveLabel( 1.046586,-0.1938999,1.407997,0.2144871, "00" );
  TPaveLabel* C01 = new TPaveLabel( 0.962076,0.4382608,1.323487,0.8466479 , "01" );
  TPaveLabel* C02 = new TPaveLabel( 0.4801947,0.8802139,0.8416056,1.288601 , "02" );
  TPaveLabel* C03 = new TPaveLabel( -0.1823921,1.011681,0.1790189,1.425662, "03" );
  TPaveLabel* C04 = new TPaveLabel( -0.8449788,0.8690253,-0.4835679,1.288601 , "04" );
  TPaveLabel* C05 = new TPaveLabel( -1.30879,0.441058,-0.9473786,0.8550394 , "05" );
  TPaveLabel* C06 = new TPaveLabel( -1.411009,-0.2050886,-1.049598,0.2144871, "06" );
  TPaveLabel* C07 = new TPaveLabel( -1.302585,-0.7757116,-0.9471979,-0.3561359 , "07" );
  TPaveLabel* C08 = new TPaveLabel( -0.8449788,-1.309971,-0.4835679,-0.8848013 , "08" );
  TPaveLabel* C09 = new TPaveLabel( -0.1823921,-1.426557,0.1790189,-1.006982 , "09" );
  TPaveLabel* C10 = new TPaveLabel( 0.4801947,-1.309076,0.8416056,-0.8839062 , "10" );
  TPaveLabel* C11 = new TPaveLabel( 0.9622567,-0.7785088,1.323668,-0.3533387 , "11" );

  A00->SetFillColor(0);
  A01->SetFillColor(0);
  A02->SetFillColor(0);
  A03->SetFillColor(0);
  A04->SetFillColor(0);
  A05->SetFillColor(0);
  A06->SetFillColor(0);
  A07->SetFillColor(0);
  A08->SetFillColor(0);
  A09->SetFillColor(0);
  A10->SetFillColor(0);
  A11->SetFillColor(0);

  C00->SetFillColor(0);
  C01->SetFillColor(0);
  C02->SetFillColor(0);
  C03->SetFillColor(0);
  C04->SetFillColor(0);
  C05->SetFillColor(0);
  C06->SetFillColor(0);
  C07->SetFillColor(0);
  C08->SetFillColor(0);
  C09->SetFillColor(0);
  C10->SetFillColor(0);
  C11->SetFillColor(0);

  gStyle->SetPalette(kBird);

  TCanvas *Error_Viz = new TCanvas("Error_Viz", "Error_Viz", 1248, 598);
  Error_Viz->Divide(2,1);
  Error_Viz->cd(1);
  dummy_his1->Draw("");
  //polardummy1->Draw("same");
  ErrCSide->Draw("colpolzsame0");
  C00->Draw("same");
  C01->Draw("same");
  C02->Draw("same");
  C03->Draw("same");
  C04->Draw("same");
  C05->Draw("same");
  C06->Draw("same");
  C07->Draw("same");
  C08->Draw("same");
  C09->Draw("same");
  C10->Draw("same");
  C11->Draw("same");  
  Error_Viz->cd(2);
  dummy_his2->Draw("");
  //polardummy2->Draw("");
  ErrASide->Draw("colpolzsame0");
  A00->Draw("same");
  A01->Draw("same");
  A02->Draw("same");
  A03->Draw("same");
  A04->Draw("same");
  A05->Draw("same");
  A06->Draw("same");
  A07->Draw("same");
  A08->Draw("same");
  A09->Draw("same");
  A10->Draw("same");
  A11->Draw("same");

  ErrCSide->SetMaximum(4);
  ErrASide->SetMaximum(4);

  ErrCSide->SetMinimum(0);
  ErrASide->SetMinimum(0);
  

  //Set Same Scale for A and C side displays

  Double_t Maxval = TMath::Max(ErrASide->GetBinContent(ErrASide->GetMaximumBin()),ErrCSide->GetBinContent(ErrCSide->GetMaximumBin()));
  //ErrASide->SetMaximum(Maxval);
  //ErrCSide->SetMaximum(Maxval);

  //ErrASide->SetMinimum(0);
  //ErrCSide->SetMinimum(0);

  //  std::cout <<"A Side Entries=" << ErrASide->GetBinContent() << std::endl;
  // std::cout <<"C Side Entries=" << ErrCSide->GetBinContent() << std::endl;
  // std::cout <<" Min A Side Bin" << ErrASide->GetMinimumBin() << std::endl;
  // std::cout <<" Min C Side Bin" << ErrCSide->GetMinimumBin() << std::endl;

  //____________________________________________________________________________________________________// 


  TFile *outf = new TFile("Trip_Histos.root","RECREATE");
  Error_Viz->Write();
  //Trip_per_stack_dist->Scale(1./trip_count_total);
  //Trip_per_stack_dist->Fit("PoissonFit","ERS");
  //cout<< "\nThe total trips = " << trip_count_total << endl;
  //delete sparsforVIZ;

  sprintf(name,"/sphenix/u/llegnosky/Livechan_Pedestals_Noise/run_%d/run_%d_Noise.png",run_num,run_num);
  Error_Viz->Print(name);

  outf->Write();
}

void Locate(Int_t id, Bool_t is_ASIDE, Double_t *rbin, Double_t *thbin) {
  
  Double_t ASIDE_angle_bins[12] = { 0.1*2.*TMath::Pi()/12 , 1.1*2.*TMath::Pi()/12 , 2.1*2.*TMath::Pi()/12 , 3.1*2.*TMath::Pi()/12 , 4.1*2.*TMath::Pi()/12 , 5.1*2.*TMath::Pi()/12 , 6.1*2.*TMath::Pi()/12 , 7.1*2.*TMath::Pi()/12 , 8.1*2.*TMath::Pi()/12 , 9.1*2.*TMath::Pi()/12 , 10.1*2.*TMath::Pi()/12 , 11.1*2.*TMath::Pi()/12 }; //CCW from x = 0 (RHS horizontal)
  /*
    Double_t CSIDE_angle_bins[12] = { 0.1*2.*TMath::Pi()/12 , 1.1*2.*TMath::Pi()/12 , 2.1*2.*TMath::Pi()/12 , 3.1*2.*TMath::Pi()/12 , 4.1*2.*TMath::Pi()/12 , 5.1*2.*TMath::Pi()/12 , 6.1*2.*TMath::Pi()/12 , 7.1*2.*TMath::Pi()/12 , 8.1*2.*TMath::Pi()/12 , 9.1*2.*TMath::Pi()/12 , 10.1*2.*TMath::Pi()/12 , 11.1*2.*TMath::Pi()/12 }; // CW from x = 0 (RHS horizontal)*/
                 
  Double_t CSIDE_angle_bins[12] = { 6.1*2.*TMath::Pi()/12 , 5.1*2.*TMath::Pi()/12 , 4.1*2.*TMath::Pi()/12 , 3.1*2.*TMath::Pi()/12 , 2.1*2.*TMath::Pi()/12 , 1.1*2.*TMath::Pi()/12 , 0.1*2.*TMath::Pi()/12 , 11.1*2.*TMath::Pi()/12 , 10.1*2.*TMath::Pi()/12 , 9.1*2.*TMath::Pi()/12 , 8.1*2.*TMath::Pi()/12 , 7.1*2.*TMath::Pi()/12  }; //CCW from x = 0 (RHS horizontal)
  /*
    Double_t ASIDE_angle_bins[12] = { 6.1*2.*TMath::Pi()/12 , 5.1*2.*TMath::Pi()/12 , 4.1*2.*TMath::Pi()/12 , 3.1*2.*TMath::Pi()/12 , 2.1*2.*TMath::Pi()/12 , 1.1*2.*TMath::Pi()/12 , 0.1*2.*TMath::Pi()/12 , 11.1*2.*TMath::Pi()/12 , 10.1*2.*TMath::Pi()/12 , 9.1*2.*TMath::Pi()/12 , 8.1*2.*TMath::Pi()/12 , 7.1*2.*TMath::Pi()/12  }; //CCW from x = 0 (RHS horizontal)
  */ 
  Int_t modid3 = id % 3;

  switch(modid3) {
  case 1:
    *rbin = 0.4; //R1
    break;
  case 2:
    *rbin = 0.6; //R2
    break;
  case 0:
    *rbin = 0.8; //R3
    break;
  }


  if( is_ASIDE ){
    *thbin = CSIDE_angle_bins[TMath::FloorNint((id-1)/3)];
  }
  else{
    *thbin = ASIDE_angle_bins[TMath::FloorNint((id-1)/3)];
  }

}
