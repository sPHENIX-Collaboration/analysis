//
//	PIDplots.cc
//	
//	Author :	Weihu Ma
//	Created:	04/2021
//	Version:	1.0
//
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>

#include "omp.h"
#include "PIDplots.h"
#include "sPhenixStyle.h"
#include "sPhenixStyle.C"

using namespace std;
//_________________________________________________________________
//////////////////////////////////////////////////////////////////////////////////////////////////////int main
int main(int argc, char **argv){
  printf("Starting the program, numargs = %d\n",argc);
  for(Int_t i=0;i<argc;i++) printf("arg #%d = %s\n",i,argv[i]);
  TROOT amdv_dens("amdv_dens", "amdv_dens");
  TApplication theApp("App", &argc, argv);
  
  const bool run_eID=true;
  
  PIDplots pidplots;
  
  pidplots.inputpars();
  
  pidplots.eID();
    
  printf("end.\n");
  
  return 0;
}
   
//////////////////////////
void PIDplots::inputpars(){
  me=0.511*0.001;//Gev electron/positron mass
  mep=0.511*0.001;//Gev electron mass
  men=0.511*0.001;//Gev positron mass
  pi=TMath::Pi();
  
  tag=1;
  //tag=1 for upsilon/embed run; 
  //tag=2 for electron,pion,antiproton/embed run; 
  //tag=3 for single particles identification and efficiency;
 //////////////////////////////////////////////input and output
  if(tag==1){//tag=1 for upsilon run
    Nfile=13; //input number of files
    Nevent=2000; //event number of each Nfile (maximum number for ifiles)
    Nmpnbin=400;//bin number for plot; Nmpnbin=200 or 400; Nmpnbin<=400.
    
    rYs[0]=9755.0/13680.0;//Yield ratio of Upsilon 1S corresponding to the input_file[0]
    rYs[1]=2540.0/13680.0;//Yield ratio of Upsilon 2S corresponding to the input_file[1]
    rYs[2]=1400.0/13680.0;//Yield ratio of Upsilon 3S corresponding to the input_file[2]
    
    //single-Upsilon decay electrons
    input_file[0] ="/sphenix/u/weihuma/RunOutput/G4sPHENIX_upsilon_1s_2000_POSCOR_g4svtx_eval.root";//2000 events
    //input_file[1] ="/sphenix/u/weihuma/RunOutput/G4sPHENIX_upsilon_2s_2000_POSCOR_g4svtx_eval.root";//2000 events
    //input_file[2] ="/sphenix/u/weihuma/RunOutput/G4sPHENIX_upsilon_3s_2000_POSCOR_g4svtx_eval.root";//2000 events

    input_file[1] ="/sphenix/u/weihuma/RunOutput/G4sPHENIX_pi-_1000_0_POSCOR_g4svtx_eval.root";//1000 events
    input_file[2] ="/sphenix/u/weihuma/RunOutput/G4sPHENIX_pi-_1000_1_POSCOR_g4svtx_eval.root";//1000 events
    input_file[3] ="/sphenix/u/weihuma/RunOutput/G4sPHENIX_pi-_1000_2_POSCOR_g4svtx_eval.root";//1000 events
    input_file[4] ="/sphenix/u/weihuma/RunOutput/G4sPHENIX_pi-_1000_3_POSCOR_g4svtx_eval.root";//1000 events
    input_file[5] ="/sphenix/u/weihuma/RunOutput/G4sPHENIX_K-_1000_0_POSCOR_g4svtx_eval.root";//1000 events
    input_file[6] ="/sphenix/u/weihuma/RunOutput/G4sPHENIX_K-_1000_1_POSCOR_g4svtx_eval.root";//1000 events
    input_file[7] ="/sphenix/u/weihuma/RunOutput/G4sPHENIX_K-_1000_2_POSCOR_g4svtx_eval.root";//1000 events
    input_file[8] ="/sphenix/u/weihuma/RunOutput/G4sPHENIX_K-_1000_3_POSCOR_g4svtx_eval.root";//1000 events
    input_file[9] ="/sphenix/u/weihuma/RunOutput/G4sPHENIX_antiproton_0_1000_POSCOR_g4svtx_eval.root";//1000 events
    input_file[10] ="/sphenix/u/weihuma/RunOutput/G4sPHENIX_antiproton_1000_1_POSCOR_g4svtx_eval.root";//1000 events
    input_file[11] ="/sphenix/u/weihuma/RunOutput/G4sPHENIX_antiproton_1000_2_POSCOR_g4svtx_eval.root";//1000 events
    input_file[12] ="/sphenix/u/weihuma/RunOutput/G4sPHENIX_antiproton_1000_3_POSCOR_g4svtx_eval.root";//1000 events
    
    //embed-Upsilon decay electrons
    
    
    output_eID ="/sphenix/u/weihuma/analysis/TrackPid/anaplots/output/upsilon/eID_embed_upsilon.root";
    output_plot_eID ="/sphenix/u/weihuma/analysis/TrackPid/anaplots/output/upsilon/eID_plot_e+e-_embed_upsilon";
    output_plot_eID_eta ="/sphenix/u/weihuma/analysis/TrackPid/anaplots/output/upsilon/efficiency_eta_e+e-_embed_upsilon";
    output_plot_eID_pt ="/sphenix/u/weihuma/analysis/TrackPid/anaplots/output/upsilon/efficiency_pt_e+e-_embed_upsilon";
    output_plot_eID_HOM ="/sphenix/u/weihuma/analysis/TrackPid/anaplots/output/upsilon/efficiency_HOM_e+e-_embed_upsilon";
    output_plot_eID_HOP ="/sphenix/u/weihuma/analysis/TrackPid/anaplots/output/upsilon/efficiency_HOP_e+e-_embed_upsilon";
    output_plot_eID_EOP ="/sphenix/u/weihuma/analysis/TrackPid/anaplots/output/upsilon/efficiency_EOP_e+e-_embed_upsilon";
  }
  if(tag==2){//tag=2 for embed run
    Nfile=180; //input number of files
    Nevent=2000; //event number of each Nfile (maximum number for ifiles)
    Nmpnbin=400;//bin number for plot; Nmpnbin=200 or 400; Nmpnbin<=400.

    output_eID ="/sphenix/u/weihuma/analysis/TrackPid/anaplots/output/embed/eID_embed.root";
    output_plot_eID ="/sphenix/u/weihuma/analysis/TrackPid/anaplots/output/embed/eID_plot_e+e-_embed";
    output_plot_eID_eta ="/sphenix/u/weihuma/analysis/TrackPid/anaplots/output/embed/efficiency_eta_e+e-_embed";
    output_plot_eID_pt ="/sphenix/u/weihuma/analysis/TrackPid/anaplots/output/embed/efficiency_pt_e+e-_embed";
    output_plot_eID_HOM ="/sphenix/u/weihuma/analysis/TrackPid/anaplots/output/embed/efficiency_HOM_e+e-_embed";
    output_plot_eID_HOP ="/sphenix/u/weihuma/analysis/TrackPid/anaplots/output/embed/efficiency_HOP_e+e-_embed";
    output_plot_eID_EOP ="/sphenix/u/weihuma/analysis/TrackPid/anaplots/output/embed/efficiency_EOP_e+e-_embed";
  }
  
  if(tag==3){//tag=3 for single particle identification and efficiency
    Nfile=16; //input number of files
    Nevent=1000; //event number of each Nfile (maximum number for ifiles)
    Nmpnbin=200;//bin number for plot; Nmpnbin=200 or 400; Nmpnbin<=400. 
  
    input_file[0] ="/sphenix/u/weihuma/RunOutput/G4sPHENIX_e-_1000_0_POSCOR_g4svtx_eval.root";//1000 events
    input_file[1] ="/sphenix/u/weihuma/RunOutput/G4sPHENIX_e-_1000_1_POSCOR_g4svtx_eval.root";//1000 events
    input_file[2] ="/sphenix/u/weihuma/RunOutput/G4sPHENIX_e-_1000_2_POSCOR_g4svtx_eval.root";//1000 events
    input_file[3] ="/sphenix/u/weihuma/RunOutput/G4sPHENIX_e-_1000_3_POSCOR_g4svtx_eval.root";//1000 events
    input_file[4] ="/sphenix/u/weihuma/RunOutput/G4sPHENIX_pi-_1000_0_POSCOR_g4svtx_eval.root";//1000 events
    input_file[5] ="/sphenix/u/weihuma/RunOutput/G4sPHENIX_pi-_1000_1_POSCOR_g4svtx_eval.root";//1000 events
    input_file[6] ="/sphenix/u/weihuma/RunOutput/G4sPHENIX_pi-_1000_2_POSCOR_g4svtx_eval.root";//1000 events
    input_file[7] ="/sphenix/u/weihuma/RunOutput/G4sPHENIX_pi-_1000_3_POSCOR_g4svtx_eval.root";//1000 events
    input_file[8] ="/sphenix/u/weihuma/RunOutput/G4sPHENIX_K-_1000_0_POSCOR_g4svtx_eval.root";//1000 events
    input_file[9] ="/sphenix/u/weihuma/RunOutput/G4sPHENIX_K-_1000_1_POSCOR_g4svtx_eval.root";//1000 events
    input_file[10] ="/sphenix/u/weihuma/RunOutput/G4sPHENIX_K-_1000_2_POSCOR_g4svtx_eval.root";//1000 events
    input_file[11] ="/sphenix/u/weihuma/RunOutput/G4sPHENIX_K-_1000_3_POSCOR_g4svtx_eval.root";//1000 events
    input_file[12] ="/sphenix/u/weihuma/RunOutput/G4sPHENIX_antiproton_0_1000_POSCOR_g4svtx_eval.root";//1000 events
    input_file[13] ="/sphenix/u/weihuma/RunOutput/G4sPHENIX_antiproton_1000_1_POSCOR_g4svtx_eval.root";//1000 events
    input_file[14] ="/sphenix/u/weihuma/RunOutput/G4sPHENIX_antiproton_1000_2_POSCOR_g4svtx_eval.root";//1000 events
    input_file[15] ="/sphenix/u/weihuma/RunOutput/G4sPHENIX_antiproton_1000_3_POSCOR_g4svtx_eval.root";//1000 events
    
    output_eID ="/sphenix/u/weihuma/analysis/TrackPid/anaplots/output/single/eID_single.root";
    output_plot_eID ="/sphenix/u/weihuma/analysis/TrackPid/anaplots/output/single/eID_plot_single_antiproton";
    output_plot_eID_eta ="/sphenix/u/weihuma/analysis/TrackPid/anaplots/output/single/efficiency_eta_single";
    output_plot_eID_pt ="/sphenix/u/weihuma/analysis/TrackPid/anaplots/output/single/efficiency_pt_single";
    output_plot_eID_HOM ="/sphenix/u/weihuma/analysis/TrackPid/anaplots/output/single/efficiency_HOM_single";
    output_plot_eID_HOP ="/sphenix/u/weihuma/analysis/TrackPid/anaplots/output/single/efficiency_HOP_single";
    output_plot_eID_EOP ="/sphenix/u/weihuma/analysis/TrackPid/anaplots/output/single/efficiency_EOP_single";
  }
 
}

void PIDplots::fhistogram_eID(){  
  h1pt_e= new TH1F("h1pt_e","pt",200,0,20);
  h1EOP= new TH1F("h1EOP","cemce3x3/p",200,0,10);
  h1EOP_e= new TH1F("h1EOP_e","cemce3x3/p",200,0,10);
  h1HOM= new TH1F("h1HOM","hcalin3x3/cemce3x3",200,0,10);
  h1HOP= new TH1F("h1HOP","hcalin3x3+hcalout3x3/cemce3x3",200,0,10);
  
  h1dR= new TH1F("h1dR","dR",400,0,0.2);//dR=sqrt(deta*deta+dphi*dphi)
  h1gflavor= new TH1F("h1gflavor","Eop>0.8&Eop<1.2&hcaline3x3<0.2",2000,-1000,1000);
  
}

///////////////////////////////////////////////////////////////////////////////////////////void PIDplots::eID()
void PIDplots::eID(){
  fHistogramFile_eID = new TFile(output_eID,"RECREATE");
  fHistogramFile_eID->cd();
  fhistogram_eID();
  
  for(int i=0;i<50;i++){
      
	NEID_electron_eta[i]=0;
	NEID_electron_pt[i]=0;
	NEID_electron_HOM[i]=0;
	NEID_electron_EOP[i]=0;
  	NEID_electron_HOP[i]=0;
	N_electron_eta[i]=0;
	N_electron_pt[i]=0;
	N_electron_HOM[i]=0;
	N_electron_EOP[i]=0;
  	N_electron_HOP[i]=0;
	
	eta_point[i]=0.0;
	pt_point[i]=0.0;
	HOM_point[i]=0.0;
   	HOP_point[i]=0.0;
	EOP_point[i]=0.0;
    
	N_pion_eta[i]=0;
	N_K_eta[i]=0;
	N_antiproton_eta[i]=0;
    
	N_pion_pt[i]=0;
	N_K_pt[i]=0;
	N_antiproton_pt[i]=0;
    
	N_pion_HOM[i]=0;
	N_K_HOM[i]=0;
	N_antiproton_HOM[i]=0;
    
  	N_pion_HOP[i]=0;
	N_K_HOP[i]=0;
	N_antiproton_HOP[i]=0;
    
	N_pion_EOP[i]=0;
	N_K_EOP[i]=0;
	N_antiproton_EOP[i]=0;
    
   	NEID_pion_eta[i]=0;
    	NEID_K_eta[i]=0;
    	NEID_antiproton_eta[i]=0;
	
	NEID_pion_pt[i]=0;
	NEID_K_pt[i]=0;
	NEID_antiproton_pt[i]=0;
	
  }
  NEID_electron=0; 
  NEID_pion=0;
  NEID_K=0; 
  NEID_antiproton=0;
  

  for(int ifile=0;ifile<Nfile;ifile++){
      TNtuple *ntp_vertex=0;
      TNtuple *ntp_gtrack=0;
      TNtuple *ntp_track=0;
      
      TFile *file3;
     // file3 = new TFile(input_file[ifile]);
      if(tag==1 or 3){
        file3 = new TFile(input_file[ifile]);
        file3->GetObject("ntp_vertex",ntp_vertex);
        file3->GetObject("ntp_gtrack",ntp_gtrack);
        file3->GetObject("ntp_track",ntp_track);
      }
      //////////
      int temifile=0;
      if(ifile<60) temifile=ifile+40;
      if(ifile>=60 && ifile<120) temifile=ifile+40-60;
      if(ifile>=120 && ifile<180) temifile=ifile+40-120;
      if(ifile<60)sprintf(input_file_embed,"/sphenix/sim/sim01/sphnxpro/MDC1/embedpion/embedelectronsDST_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000001-031%d_g4svtx_eval.root",temifile);
      if(ifile>=60 && ifile<120)sprintf(input_file_embed,"/sphenix/sim/sim01/sphnxpro/MDC1/embedpion/embedpionsDST_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000001-043%d_g4svtx_eval.root",temifile);
      if(ifile>=120 && ifile<180){
            sprintf(input_file_embed,"/sphenix/sim/sim01/sphnxpro/MDC1/embedpion/data/embedantiprotonsDST_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000001-021%d_g4svtx_eval.root",temifile);
            if(ifile==153)sprintf(input_file_embed,"/sphenix/sim/sim01/sphnxpro/MDC1/embedpion/data/embedantiprotonsDST_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000001-02139_g4svtx_eval.root",temifile);
      }
      if(gSystem->AccessPathName(input_file_embed)) {
            printf("no data existing for %s\n",input_file_embed);
            continue;
      }
      TFile file2(input_file_embed);
      if(tag==2){
        file2.GetObject("ntp_vertex",ntp_vertex);
        file2.GetObject("ntp_gtrack",ntp_gtrack);
        file2.GetObject("ntp_track",ntp_track);
        printf("inputfile: %s\n",input_file_embed);
      }
      
      //cout<<"ntp_vertex->GetEntries=  "<<ntp_vertex->GetEntries()<<endl;
     // cout<<"ntp_gtrack->GetEntries=  "<<ntp_gtrack->GetEntries()<<endl;
      cout<<"ntp_track->GetEntries=  "<<ntp_track->GetEntries()<<endl;
      
	
      for(int ientry=0;ientry<ntp_track->GetEntries();ientry++) {//ntp_track
         ntp_track->GetEntry(ientry);
         float *content2=ntp_track->GetArgs();  
         int event2=0,trackID2=0;
         float px2=0.0,py2=0.0,pz2=0.0,pt2=0.0,gpt2=0.0,eta2=0.0,phi2=0.0,charge2=0.0,quality2=0.0,nhits2=0.0,nmaps2=0.0,nintt2=0.0,ntpc2=0.0;
         float cemcdphi2=0.0,cemcdeta2=0.0,cemce3x32=0.0,cemce2=0.0,hcalindphi2=0.0,hcalindeta2=0.0,hcaline3x32=0.0,hcaline2=0.0,hcaloutdphi2=0.0,hcaloutdeta2=0.0,hcaloute3x32=0.0,hcaloute2=0.0;
         float gflavor2=0;

            event2=content2[0];
            trackID2=content2[2];
            px2=content2[3];
            py2=content2[4];
            pz2=content2[5];
            pt2=content2[6];
            eta2=content2[7];
            phi2=content2[8];
            charge2=content2[12];
            quality2=content2[13];
            nhits2=content2[16];
            nmaps2=content2[17];
            nintt2=content2[18];
            ntpc2=content2[19];
            cemcdphi2=content2[43];
            cemcdeta2=content2[44];
            cemce3x32=content2[45];
            cemce2=content2[46];
            hcalindphi2=content2[47];
            hcalindeta2=content2[48];
            hcaline3x32=content2[49];
            hcaline2=content2[50];
            hcaloutdphi2=content2[51];
            hcaloutdeta2=content2[52];
            hcaloute3x32=content2[53];
            hcaloute2=content2[54];
            gflavor2=content2[56];
            gpt2=content2[69];
         
        
         float p2=TMath::Sqrt(px2*px2+py2*py2+pz2*pz2);
         float E2=TMath::Sqrt(me*me+p2*p2);
         float EOP=cemce3x32/p2;// E/p
         float HOP=(hcaline3x32+hcaloute3x32)/p2;// H/p
         float HOM=hcaline3x32/cemce3x32;// EHcalin/EEmcal
         float dR=TMath::Sqrt(cemcdphi2*cemcdphi2+cemcdeta2*cemcdeta2);
         float pt=TMath::Sqrt(px2*px2+py2*py2);
       

       if(nmaps2>0 && nintt2>0 && ntpc2>20 && quality2<10){
           
         if(TMath::Abs(gflavor2)==11 && (TMath::Abs(eta2)>0.0 && TMath::Abs(eta2)<1.1) && (gpt2>0.0 && gpt2<20.0) && quality2<20) NEID_electron=NEID_electron+1;
         if(TMath::Abs(gflavor2)==211 && (TMath::Abs(eta2)>0.0 && TMath::Abs(eta2)<1.1) && (gpt2>0.0 && gpt2<20.0) && quality2<20) NEID_pion=NEID_pion+1;
         if(TMath::Abs(gflavor2)==321 && (TMath::Abs(eta2)>0.0 && TMath::Abs(eta2)<1.1) && (gpt2>0.0 && gpt2<20.0) && quality2<20) NEID_K=NEID_K+1;
         if(TMath::Abs(gflavor2)==2212 && (TMath::Abs(eta2)>0.0 && TMath::Abs(eta2)<1.1) && (gpt2>0.0 && gpt2<20.0) && quality2<20) NEID_antiproton=NEID_antiproton+1;
      	 cout<<"NEID_electron= "<<NEID_electron<<"NEID_pion= "<<NEID_pion<<"NEID_electron= "<<NEID_K<<"NEID_electron= "<<NEID_antiproton<<endl;

         if((TMath::Abs(eta2)>0.0 && TMath::Abs(eta2)<1.1) && (gpt2>0.0 && gpt2<20.0)){
            if(TMath::Abs(gflavor2)==11)h1pt_e->Fill(pt2);
            h1dR->Fill(dR);
            h1EOP->Fill(EOP);
            if(TMath::Abs(gflavor2)==11)h1EOP_e->Fill(EOP);
            h1HOM->Fill(HOM);
            h1HOP->Fill(HOP);
            h1gflavor->Fill(gflavor2);
         }
    
         for(int i=0;i<11;i++){
            eta_point[i]=i*1.0/10+0.05;
            if((TMath::Abs(eta2)>(eta_point[i]-0.05)) && (TMath::Abs(eta2)<(eta_point[i]+0.05)) && (gpt2>0.0 & gpt2<20.0)){
                if(TMath::Abs(gflavor2)==11) N_electron_eta[i]=N_electron_eta[i]+1;
                if(TMath::Abs(gflavor2)==211) N_pion_eta[i]=N_pion_eta[i]+1;
                if(TMath::Abs(gflavor2)==321) N_K_eta[i]=N_K_eta[i]+1;
                if(TMath::Abs(gflavor2)==2212) N_antiproton_eta[i]=N_antiproton_eta[i]+1;
            
                if(EOP>0.7 && EOP<1.5 && HOM<0.2 && pt>2.0 ){
				    if(TMath::Abs(gflavor2)==11) NEID_electron_eta[i]=NEID_electron_eta[i]+1;               
				    if(TMath::Abs(gflavor2)==211) NEID_pion_eta[i]=NEID_pion_eta[i]+1;
				    if(TMath::Abs(gflavor2)==321) NEID_K_eta[i]=NEID_K_eta[i]+1;
				    if(TMath::Abs(gflavor2)==2212) NEID_antiproton_eta[i]=NEID_antiproton_eta[i]+1;
				    cout<<"NEID_pion_eta= "<<NEID_pion_eta[i]<<endl;
                }
            }
         }

         for(int i=0;i<16;i++){
            pt_point[i]=i*10.0/10+0.5;
            if(pt>(pt_point[i]-0.5) && pt<(pt_point[i]+0.5) && (TMath::Abs(eta2)>0.0 && TMath::Abs(eta2)<1.1) && (gpt2>0.0 && gpt2<20.0)){
                if(TMath::Abs(gflavor2)==11) N_electron_pt[i]=N_electron_pt[i]+1;
                if(TMath::Abs(gflavor2)==211) N_pion_pt[i]=N_pion_pt[i]+1;
                if(TMath::Abs(gflavor2)==321) N_K_pt[i]=N_K_pt[i]+1;
                if(TMath::Abs(gflavor2)==2212) N_antiproton_pt[i]=N_antiproton_pt[i]+1;

                if(EOP>0.7 && EOP<1.5 && HOM<0.2){
                    if(TMath::Abs(gflavor2)==11) NEID_electron_pt[i]=NEID_electron_pt[i]+1;     
                    if(TMath::Abs(gflavor2)==211) NEID_pion_pt[i]=NEID_pion_pt[i]+1;
                    if(TMath::Abs(gflavor2)==321) NEID_K_pt[i]=NEID_K_pt[i]+1;
                    if(TMath::Abs(gflavor2)==2212) NEID_antiproton_pt[i]=NEID_antiproton_pt[i]+1;
                    cout<<"NEID_pion_pt= "<<NEID_pion_pt[i]<<endl;
                }
            }
         }

         for(int i=0;i<20;i++){
            HOM_point[i]=i*0.5/10+0.05;
            if(HOM<HOM_point[i] && (TMath::Abs(eta2)>0.0 && TMath::Abs(eta2)<1.1) && (gpt2>0.0 && gpt2<20.0)){
                if(TMath::Abs(gflavor2)==11) N_electron_HOM[i]=N_electron_HOM[i]+1;
                if(TMath::Abs(gflavor2)==211) N_pion_HOM[i]=N_pion_HOM[i]+1;
                if(TMath::Abs(gflavor2)==321) N_K_HOM[i]=N_K_HOM[i]+1;
                if(TMath::Abs(gflavor2)==2212) N_antiproton_HOM[i]=N_antiproton_HOM[i]+1;
            }
        }
        
         for(int i=0;i<20;i++){
            HOP_point[i]=i*0.5/10+0.05;
            if(HOP>HOP_point[i] && (TMath::Abs(eta2)>0.0 && TMath::Abs(eta2)<1.1) && (gpt2>0.0 && gpt2<20.0)){
                if(TMath::Abs(gflavor2)==11) N_electron_HOP[i]=N_electron_HOP[i]+1;
                if(TMath::Abs(gflavor2)==211) N_pion_HOP[i]=N_pion_HOP[i]+1;
                if(TMath::Abs(gflavor2)==321) N_K_HOP[i]=N_K_HOP[i]+1;
                if(TMath::Abs(gflavor2)==2212) N_antiproton_HOP[i]=N_antiproton_HOP[i]+1;
            }
         }
	  
         for(int i=0;i<9;i++){
            EOP_point[i]=1.0-i*1.0/10;
            if(EOP>EOP_point[i] && EOP<1.5 && (TMath::Abs(eta2)>0.0 && TMath::Abs(eta2)<1.1) && (gpt2>0.0 && gpt2<20.0)){
                if(TMath::Abs(gflavor2)==11) N_electron_EOP[i]=N_electron_EOP[i]+1;
                if(TMath::Abs(gflavor2)==211) N_pion_EOP[i]=N_pion_EOP[i]+1;
                if(TMath::Abs(gflavor2)==321) N_K_EOP[i]=N_K_EOP[i]+1;
                if(TMath::Abs(gflavor2)==2212) N_antiproton_EOP[i]=N_antiproton_EOP[i]+1;
            }
         }
         
       }//if(nmaps2>1 && nintt2>1 && ntpc2>25){
	
    }//for(int ientry=0;ientry<ntp_track->GetEntries();ientry++) { 
  }//for(int ifile=0;ifile<1;ifile++){
    
  eIDoutput();
  fHistogramFile_eID->Write();
  fHistogramFile_eID->Close();
  
}//end of PIDplots::eID()

void PIDplots::eIDoutput(){  
  float aa_eta[20],cc_eta[20],ee_eta[20],ff_eta[20],hh_eta[20];
  float aa_pt[20],cc_pt[20],ee_pt[20],ff_pt[20],hh_pt[20];
  float aa_HOM[20],cc_HOM[20],ee_HOM[20],ff_HOM[20],hh_HOM[20];
  float aa_HOP[20],cc_HOP[20],ee_HOP[20],ff_HOP[20],hh_HOP[20];
  float aa_EOP[20],cc_EOP[20],ee_EOP[20],ff_EOP[20],hh_EOP[20];
  float ree_eta[20],rff_eta[20],rhh_eta[20],ree_pt[20],rff_pt[20],rhh_pt[20];
  float ree_EOP[20],rff_EOP[20],rhh_EOP[20],ree_HOM[20],rff_HOM[20],rhh_HOM[20],ree_HOP[20],rff_HOP[20],rhh_HOP[20];
  
  float err_aa_eta[20],err_cc_eta[20],err_aa_pt[20],err_cc_pt[20],err_aa_HOM[20],err_cc_HOM[20],err_aa_HOP[20],err_cc_HOP[20],err_aa_EOP[20],err_cc_EOP[20];
  float err_ee_eta[20],err_ff_eta[20],err_hh_eta[20],err_ee_pt[20],err_ff_pt[20],err_hh_pt[20];
  float err_ee_HOM[20],err_ff_HOM[20],err_hh_HOM[20],err_ee_HOP[20],err_ff_HOP[20],err_hh_HOP[20],err_ee_EOP[20],err_ff_EOP[20],err_hh_EOP[20];
  float err_ree_eta[20],err_rff_eta[20],err_rhh_eta[20],err_ree_pt[20],err_rff_pt[20],err_rhh_pt[20];
  float err_ree_HOM[20],err_rff_HOM[20],err_rhh_HOM[20],err_ree_HOP[20],err_rff_HOP[20],err_rhh_HOP[20],err_ree_EOP[20],err_rff_EOP[20],err_rhh_EOP[20];
  
  for(int i=0;i<20;i++){
	aa_eta[i]=0.0;
    	cc_eta[i]=0.0;
	ee_eta[i]=0.0;
	ff_eta[i]=0.0;
	hh_eta[i]=0.0;
    	ree_eta[i]=0.0;
	rff_eta[i]=0.0;
	rhh_eta[i]=0.0;
    	err_ree_eta[i]=0.0;
	err_rff_eta[i]=0.0;
	err_rhh_eta[i]=0.0;
    
	aa_pt[i]=0.0;
    	cc_pt[i]=0.0;
	ee_pt[i]=0.0;
	ff_pt[i]=0.0;
	hh_pt[i]=0.0;
    	ree_pt[i]=0.0;
	rff_pt[i]=0.0;
	rhh_pt[i]=0.0;
    	err_ree_pt[i]=0.0;
	err_rff_pt[i]=0.0;
	err_rhh_pt[i]=0.0;
    
	aa_HOM[i]=0.0;
	cc_HOM[i]=0.0;
	ee_HOM[i]=0.0;
	ff_HOM[i]=0.0;
	hh_HOM[i]=0.0;
    	ree_HOM[i]=0.0;
	rff_HOM[i]=0.0;
	rhh_HOM[i]=0.0;
    	err_ree_HOM[i]=0.0;
	err_rff_HOM[i]=0.0;
	err_rhh_HOM[i]=0.0;
    
    	aa_HOP[i]=0.0;
	cc_HOP[i]=0.0;
	ee_HOP[i]=0.0;
	ff_HOP[i]=0.0;
	hh_HOP[i]=0.0;
    	ree_HOP[i]=0.0;
	rff_HOP[i]=0.0;
	rhh_HOP[i]=0.0;
    	err_ree_HOP[i]=0.0;
	err_rff_HOP[i]=0.0;
	err_rhh_HOP[i]=0.0;
    
	aa_EOP[i]=0.0;
	cc_EOP[i]=0.0;
	ee_EOP[i]=0.0;
	ff_EOP[i]=0.0;
	hh_EOP[i]=0.0;
    	ree_EOP[i]=0.0;
	rff_EOP[i]=0.0;
	rhh_EOP[i]=0.0;
    	err_ree_EOP[i]=0.0;
	err_rff_EOP[i]=0.0;
	err_rhh_EOP[i]=0.0;
	
	
	err_aa_eta[i]=0.0;
    	err_cc_eta[i]=0.0;
	err_ee_eta[i]=0.0;
	err_ff_eta[i]=0.0;
	err_hh_eta[i]=0.0;
	
	err_aa_pt[i]=0.0;
	err_cc_pt[i]=0.0;
	err_ee_pt[i]=0.0;
	err_ff_pt[i]=0.0;
	err_hh_pt[i]=0.0;
	
	err_aa_HOM[i]=0.0;
	err_cc_HOM[i]=0.0;
	err_ee_HOM[i]=0.0;
	err_ff_HOM[i]=0.0;
	err_hh_HOM[i]=0.0;
    
    	err_aa_HOP[i]=0.0;
	err_cc_HOP[i]=0.0;
	err_ee_HOP[i]=0.0;
	err_ff_HOP[i]=0.0;
	err_hh_HOP[i]=0.0;

	
	err_aa_EOP[i]=0.0;
	err_cc_EOP[i]=0.0;
	err_ee_EOP[i]=0.0;
	err_ff_EOP[i]=0.0;
	err_hh_EOP[i]=0.0;

    
	
  }
  //////////////////////////////////////////////////////////////eta
  FILE *outfile1;
  TString fname1;
  fname1 = Form("%s.dat",output_plot_eID_eta);
  outfile1 =fopen(fname1.Data(),"w");
  int Nplot_electron_eta=0,Nplot_pion_eta=0,Nplot_K_eta=0,Nplot_antiproton_eta=0;
  for(int i=0;i<11;i++){
   // if(N_electron_eta[i]==0 or N_pion_eta[i]==0 or N_K_eta[i]==0 or N_antiproton_eta[i]==0)continue;
   // if(NEID_electron_eta[i]==0 or NEID_pion_eta[i]==0 or NEID_K_eta[i]==0 or NEID_antiproton_eta[i]==0)continue;
    
    aa_eta[i]=eta_point[i];
    err_aa_eta[i]=0.05;
    if(N_electron_eta[i]>0 && NEID_electron_eta[i]>0){
        cc_eta[i]=1.0*NEID_electron_eta[i]/N_electron_eta[i];
        err_cc_eta[i]=1.0*TMath::Sqrt((1.0/NEID_electron_eta[i]+1.0/N_electron_eta[i]))*cc_eta[i];
        Nplot_electron_eta=Nplot_electron_eta+1;
    }
    if(N_pion_eta[i]>0 && NEID_pion_eta[i]>0){
        ee_eta[i]=1.0*NEID_pion_eta[i]/N_pion_eta[i];
        err_ee_eta[i]=1.0*TMath::Sqrt((1.0/NEID_pion_eta[i]+1.0/N_pion_eta[i]))*ee_eta[i];
        Nplot_pion_eta=Nplot_pion_eta+1;
    }
    if(N_K_eta[i]>0 && NEID_K_eta[i]>0){
        ff_eta[i]=1.0*NEID_K_eta[i]/N_K_eta[i];
        err_ff_eta[i]=1.0*TMath::Sqrt((1.0/NEID_K_eta[i]+1.0/N_K_eta[i]))*ff_eta[i];
        Nplot_K_eta=Nplot_K_eta+1;
    }
    if(N_antiproton_eta[i]>0 && NEID_antiproton_eta[i]>0){
        hh_eta[i]=1.0*NEID_antiproton_eta[i]/N_antiproton_eta[i];
        err_hh_eta[i]=1.0*TMath::Sqrt((1.0/NEID_antiproton_eta[i]+1.0/N_antiproton_eta[i]))*hh_eta[i];
        Nplot_antiproton_eta=Nplot_antiproton_eta+1;
    }
	  
	float aa,cc,ee,ff,hh;
    float err_aa,err_cc,err_ee,err_ff,err_hh;
	aa=aa_eta[i];
	cc=cc_eta[i];
	ee=ee_eta[i];
	ff=ff_eta[i];
	hh=hh_eta[i];
    
    if(ee_eta[i]>0.0){
        ree_eta[i]=1.0/ee_eta[i];
        err_ree_eta[i]=ree_eta[i]*ree_eta[i]*err_ee_eta[i];
    }
   //cout<<cc_eta[i]<<"; "<<ree_eta[i]<<"; "<<err_ree_eta[i]<<endl;
    if(ff_eta[i]>0.0){
        rff_eta[i]=1.0/ff_eta[i];
        err_rff_eta[i]=rff_eta[i]*rff_eta[i]*err_ff_eta[i];
    }
    if(hh_eta[i]>0.0){
        rhh_eta[i]=1.0/hh_eta[i];
        err_rhh_eta[i]=rhh_eta[i]*rhh_eta[i]*err_hh_eta[i];
    }
	 
    fprintf(outfile1,"eta:  %f    %f    %f    %f    %f\n",aa,cc,ee,ff,hh);
     
  }
  
  //////////////////////////////////////////////////////////////pt
  FILE *outfile2;
  TString fname2;
  fname2 = Form("%s.dat",output_plot_eID_pt);
  outfile2 =fopen(fname2.Data(),"w");
  int Nplot_electron_pt=0,Nplot_pion_pt=0,Nplot_K_pt=0,Nplot_antiproton_pt=0;
  for(int i=0;i<16;i++){
   // if(N_electron_pt[i]==0 or N_pion_pt[i]==0 or N_K_pt[i]==0 or N_antiproton_pt[i]==0)continue;
   // if(NEID_electron_pt[i]==0 or NEID_pion_pt[i]==0 or NEID_K_pt[i]==0 or NEID_antiproton_pt[i]==0)continue;
    
	aa_pt[i]=pt_point[i];
	err_aa_pt[i]=0.5;
    if(N_electron_pt[i]>0 && NEID_electron_pt[i]>0){
        cc_pt[i]=1.0*NEID_electron_pt[i]/N_electron_pt[i];
        err_cc_pt[i]=1.0*TMath::Sqrt((1.0/NEID_electron_pt[i]+1.0/N_electron_pt[i]))*cc_pt[i];
        Nplot_electron_pt=Nplot_electron_pt+1;
       // cout<<cc_pt[i]<<"; "<<err_cc_pt[i]<<endl;
    }
    if(N_pion_pt[i]>0 && NEID_pion_pt[i]>0){
        ee_pt[i]=1.0*NEID_pion_pt[i]/N_pion_pt[i];
        err_ee_pt[i]=1.0*TMath::Sqrt((1.0/NEID_pion_pt[i]+1.0/N_pion_pt[i]))*ee_pt[i];
        Nplot_pion_pt=Nplot_pion_pt+1;
        //cout<<ee_pt[i]<<"; "<<err_ee_pt[i]<<endl;
    }
    if(N_K_pt[i]>0 && NEID_K_pt[i]>0){
        ff_pt[i]=1.0*NEID_K_pt[i]/N_K_pt[i];
        err_ff_pt[i]=1.0*TMath::Sqrt((1.0/NEID_K_pt[i]+1.0/N_K_pt[i]))*ff_pt[i];
        Nplot_K_pt=Nplot_K_pt+1;
    }
    if(N_antiproton_pt[i]>0 && NEID_antiproton_pt[i]>0){
        hh_pt[i]=1.0*NEID_antiproton_pt[i]/N_antiproton_pt[i];
        err_hh_pt[i]=1.0*TMath::Sqrt((1.0/NEID_antiproton_pt[i]+1.0/N_antiproton_pt[i]))*hh_pt[i];
        Nplot_antiproton_pt=Nplot_antiproton_pt+1;
    }
	  
	float aa,cc,ee,ff,hh;
    float err_aa,err_cc,err_ee,err_ff,err_hh;
	aa=aa_pt[i];
	cc=cc_pt[i];
	ee=ee_pt[i];
	ff=ff_pt[i];
	hh=hh_pt[i];
    
    if(ee_pt[i]>0.0){
        ree_pt[i]=1.0/ee_pt[i];
        err_ree_pt[i]=ree_pt[i]*ree_pt[i]*err_ee_pt[i];
       // cout<<cc_pt[i]<<"; "<<ree_pt[i]<<"; "<<err_ree_pt[i]<<endl;
    }
    cout<<cc_pt[i]<<"; "<<ree_pt[i]<<"; "<<err_ree_pt[i]<<endl;
    if(ff_pt[i]>0.0){
        rff_pt[i]=1.0/ff_pt[i];
        err_rff_pt[i]=rff_pt[i]*rff_pt[i]*err_ff_pt[i];
    }
    if(hh_pt[i]>0.0){
        rhh_pt[i]=1.0/hh_pt[i];
        err_rhh_pt[i]=rhh_pt[i]*rhh_pt[i]*err_hh_pt[i];
    }
	cout<<cc_pt[i]<<"; "<<rhh_pt[i]<<"; "<<err_rhh_pt[i]<<endl;
	fprintf(outfile2,"pt:  %f    %f    %f    %f    %f\n",aa,cc,ee,ff,hh);
  }
 
  
//////////////////////////////////////////////////////////////HOM
  FILE *outfile3;
  TString fname3;
  fname3 = Form("%s.dat",output_plot_eID_HOM);
  outfile3 =fopen(fname3.Data(),"w");
  int Nplot_electron_HOM=0,Nplot_pion_HOM=0,Nplot_K_HOM=0,Nplot_antiproton_HOM=0;
  for(int i=0;i<20;i++){
	//if(NEID_electron==0 or NEID_pion==0 or NEID_K==0 or NEID_antiproton==0)continue;
    //if(N_electron_HOM[i]==0 or N_pion_HOM[i]==0 or N_K_HOM[i]==0 or N_antiproton_HOM[i]==0)continue;
      
	aa_HOM[i]=HOM_point[i];
	err_aa_HOM[i]=0.0;
    if(NEID_electron>0 & N_electron_HOM[i]>0){
        cc_HOM[i]=1.0*N_electron_HOM[i]/NEID_electron;
        err_cc_HOM[i]=1.0*TMath::Sqrt((1.0/N_electron_HOM[i]+1.0/NEID_electron))*cc_HOM[i];
        Nplot_electron_HOM=Nplot_electron_HOM+1;
    }
	if(NEID_pion>0 & N_pion_HOM[i]>0){
        ee_HOM[i]=1.0*N_pion_HOM[i]/NEID_pion;
        err_ee_HOM[i]=1.0*TMath::Sqrt((1.0/N_pion_HOM[i]+1.0/NEID_pion))*ee_HOM[i];
        Nplot_pion_HOM=Nplot_pion_HOM+1;
    }
    if(NEID_K>0 & N_K_HOM[i]>0){
        ff_HOM[i]=1.0*N_K_HOM[i]/NEID_K;
        err_ff_HOM[i]=1.0*TMath::Sqrt((1.0/N_K_HOM[i]+1.0/NEID_K))*ff_HOM[i];
        Nplot_K_HOM=Nplot_K_HOM+1;
    }
    if(NEID_antiproton>0 & N_antiproton_HOM[i]>0){
        hh_HOM[i]=1.0*N_antiproton_HOM[i]/NEID_antiproton;
        err_hh_HOM[i]=1.0*TMath::Sqrt((1.0/N_antiproton_HOM[i]+1.0/NEID_antiproton))*hh_HOM[i];
        Nplot_antiproton_HOM=Nplot_antiproton_HOM+1;
    }
		
    float aa,cc,ee,ff,hh;
    float err_aa,err_cc,err_ee,err_ff,err_hh;
	aa=aa_HOM[i];
	cc=cc_HOM[i];
	ee=ee_HOM[i];
	ff=ff_HOM[i];
	hh=hh_HOM[i];
    
    if(ee_HOM[i]>0.0){
        ree_HOM[i]=1.0/ee_HOM[i];
        err_ree_HOM[i]=ree_HOM[i]*ree_HOM[i]*err_ee_HOM[i];
    }
    if(ff_HOM[i]>0.0){
        rff_HOM[i]=1.0/ff_HOM[i];
        err_rff_HOM[i]=rff_HOM[i]*rff_HOM[i]*err_ff_HOM[i];
    }
    if(hh_HOM[i]>0.0){
        rhh_HOM[i]=1.0/hh_HOM[i];
        err_rhh_HOM[i]=rhh_HOM[i]*rhh_HOM[i]*err_hh_HOM[i];
    }
	 
    
    	err_aa=err_aa_HOM[i];
	err_cc=err_cc_HOM[i];
	err_ee=err_ee_HOM[i];
	err_ff=err_ff_HOM[i];
	err_hh=err_hh_HOM[i];
	
	fprintf(outfile3,"   %f    %f    %f    %f    %f    %f    %f    %f    %f    %f\n",aa,err_aa,cc,err_cc,ee,err_ee,ff,err_ff,hh,err_hh);
  }
  
  //////////////////////////////////////////////////////////////EOP
  FILE *outfile4;
  TString fname4;
  fname4 = Form("%s.dat",output_plot_eID_EOP);
  outfile4 =fopen(fname4.Data(),"w");
  int Nplot_electron_EOP=0,Nplot_pion_EOP=0,Nplot_K_EOP=0,Nplot_antiproton_EOP=0;
  for(int i=0;i<9;i++){
	//if(NEID_electron==0 or NEID_pion==0 or NEID_K==0 or NEID_antiproton==0)continue;
   // if(N_electron_EOP[i]==0 or N_pion_EOP[i]==0 or N_K_EOP[i]==0 or N_antiproton_EOP[i]==0)continue;
    
	aa_EOP[i]=EOP_point[i];
	err_aa_EOP[i]=0.0;
    if(NEID_electron>0 & N_electron_EOP[i]>0){
        cc_EOP[i]=1.0*N_electron_EOP[i]/NEID_electron;
        err_cc_EOP[i]=1.0*TMath::Sqrt((1.0/N_electron_EOP[i]+1.0/NEID_electron))*cc_EOP[i];
        Nplot_electron_EOP=Nplot_electron_EOP+1;
       // cout<<cc_EOP[i]<<"; "<<err_cc_EOP[i]<<endl;
    }
	if(NEID_pion>0 & N_pion_EOP[i]>0){
        ee_EOP[i]=1.0*N_pion_EOP[i]/NEID_pion;
        err_ee_EOP[i]=1.0*TMath::Sqrt((1.0/N_pion_EOP[i]+1.0/NEID_pion))*ee_EOP[i];
        Nplot_pion_EOP=Nplot_pion_EOP+1;
    }
    if(NEID_K>0 & N_K_EOP[i]>0){
        ff_EOP[i]=1.0*N_K_EOP[i]/NEID_K;
        err_ff_EOP[i]=1.0*TMath::Sqrt((1.0/N_K_EOP[i]+1.0/NEID_K))*ff_EOP[i];
        Nplot_K_EOP=Nplot_K_EOP+1;
    }
    if(NEID_antiproton>0 & N_antiproton_EOP[i]>0){
        hh_EOP[i]=1.0*N_antiproton_EOP[i]/NEID_antiproton;
        err_hh_EOP[i]=1.0*TMath::Sqrt((1.0/N_antiproton_EOP[i]+1.0/NEID_antiproton))*hh_EOP[i];
        Nplot_antiproton_EOP=Nplot_antiproton_EOP+1;
    }
		
    float aa,cc,ee,ff,hh;
    float err_aa,err_cc,err_ee,err_ff,err_hh;
	aa=aa_EOP[i];
	cc=cc_EOP[i];
	ee=ee_EOP[i];
	ff=ff_EOP[i];
	hh=hh_EOP[i];
    
    if(ee_EOP[i]>0.0){
        ree_EOP[i]=1.0/ee_EOP[i];
        err_ree_EOP[i]=ree_EOP[i]*ree_EOP[i]*err_ee_EOP[i];
       // cout<<ree_EOP[i]<<"; "<<err_ree_EOP[i]<<endl;
    }
    if(ff_EOP[i]>0.0){
        rff_EOP[i]=1.0/ff_EOP[i];
        err_rff_EOP[i]=rff_EOP[i]*rff_EOP[i]*err_ff_EOP[i];
    }
    if(hh_EOP[i]>0.0){
        rhh_EOP[i]=1.0/hh_EOP[i];
        err_rhh_EOP[i]=rhh_EOP[i]*rhh_EOP[i]*err_hh_EOP[i];
    }
    
   	err_aa=err_aa_EOP[i];
	err_cc=err_cc_EOP[i];
	err_ee=err_ee_EOP[i];
	err_ff=err_ff_EOP[i];
	err_hh=err_hh_EOP[i];
	
	fprintf(outfile4,"   %f    %f    %f    %f    %f    %f    %f    %f    %f    %f\n",aa,err_aa,cc,err_cc,ee,err_ee,ff,err_ff,hh,err_hh);
  }
  
  //////////////////////////////////////////////////////////////HOP
  FILE *outfile5;
  TString fname5;
  fname5 = Form("%s.dat",output_plot_eID_HOP);
  outfile5 =fopen(fname5.Data(),"w");
  int Nplot_electron_HOP=0,Nplot_pion_HOP=0,Nplot_K_HOP=0,Nplot_antiproton_HOP=0;
  for(int i=0;i<20;i++){
	//if(NEID_electron==0 or NEID_pion==0 or NEID_K==0 or NEID_antiproton==0)continue;
    //if(N_electron_HOP[i]==0 or N_pion_HOP[i]==0 or N_K_HOP[i]==0 or N_antiproton_HOP[i]==0)continue;
      
	aa_HOP[i]=HOP_point[i];
	err_aa_HOP[i]=0.0;
    if(NEID_electron>0 & N_electron_HOP[i]>0){
        cc_HOP[i]=1.0*N_electron_HOP[i]/NEID_electron;
        err_cc_HOP[i]=1.0*TMath::Sqrt((1.0/N_electron_HOP[i]+1.0/NEID_electron))*cc_HOP[i];
        Nplot_electron_HOP=Nplot_electron_HOP+1;
    }
	if(NEID_pion>0 & N_pion_HOP[i]>0){
        ee_HOP[i]=1.0*N_pion_HOP[i]/NEID_pion;
        err_ee_HOP[i]=1.0*TMath::Sqrt((1.0/N_pion_HOP[i]+1.0/NEID_pion))*ee_HOP[i];
        Nplot_pion_HOP=Nplot_pion_HOP+1;
    }
    if(NEID_K>0 & N_K_HOP[i]>0){
        ff_HOP[i]=1.0*N_K_HOP[i]/NEID_K;
        err_ff_HOP[i]=1.0*TMath::Sqrt((1.0/N_K_HOP[i]+1.0/NEID_K))*ff_HOP[i];
        Nplot_K_HOP=Nplot_K_HOP+1;
    }
    if(NEID_antiproton>0 & N_antiproton_HOP[i]>0){
        hh_HOP[i]=1.0*N_antiproton_HOP[i]/NEID_antiproton;
        err_hh_HOP[i]=1.0*TMath::Sqrt((1.0/N_antiproton_HOP[i]+1.0/NEID_antiproton))*hh_HOP[i];
        Nplot_antiproton_HOP=Nplot_antiproton_HOP+1;
    }
		
    float aa,cc,ee,ff,hh;
    float err_aa,err_cc,err_ee,err_ff,err_hh;
    aa=aa_HOP[i];
    cc=cc_HOP[i];
    ee=ee_HOP[i];
    ff=ff_HOP[i];
    hh=hh_HOP[i];
    
    if(ee_HOP[i]>0.0){
        ree_HOP[i]=1.0/ee_HOP[i];
        err_ree_HOP[i]=ree_HOP[i]*ree_HOP[i]*err_ee_HOP[i];
    }
    if(ff_HOP[i]>0.0){
        rff_HOP[i]=1.0/ff_HOP[i];
        err_rff_HOP[i]=rff_HOP[i]*rff_HOP[i]*err_ff_HOP[i];
    }
    if(hh_HOP[i]>0.0){
        rhh_HOP[i]=1.0/hh_HOP[i];
        err_rhh_HOP[i]=rhh_HOP[i]*rhh_HOP[i]*err_hh_HOP[i];
    }
    
    err_aa=err_aa_HOP[i];
	err_cc=err_cc_HOP[i];
	err_ee=err_ee_HOP[i];
	err_ff=err_ff_HOP[i];
	err_hh=err_hh_HOP[i];
	
	fprintf(outfile5,"   %f    %f    %f    %f    %f    %f    %f    %f    %f    %f\n",aa,err_aa,cc,err_cc,ee,err_ee,ff,err_ff,hh,err_hh);
  }
  

  /////////////////////////////
  gROOT->LoadMacro("sPhenixStyle.C");
  SetsPhenixStyle();
  TCanvas *canv= new TCanvas("canv","Cali Canvas",1800,4750);
  canv->Divide(2,5);
  
 ///////////////////////////////////////////////////////// canv->cd(1);
  canv->cd(1);
  TPad *pad1 = new TPad("pad1","pad1",0,0,0.99,0.99);
  pad1->Draw();    
  pad1->cd();
  //pad1->SetFrameLineWidth(2);
  //pad1->SetFrameLineColor(1);
  Float_t Xmin,Xmax;
  Float_t Ymin,Ymax;
  Ymin=-0.05;
  if(tag==2)Ymax=2.0;
  if(tag==1 or 3)Ymax=2.0;
  Xmin=0.0;
  Xmax=1.1; 
  
  
  
  TH1F *hframe0;
  hframe0=gPad->DrawFrame(Xmin,Ymin,Xmax,Ymax);
  hframe0->GetXaxis()->SetTitle("|#eta|"); 
  //hframe0->GetXaxis()->CenterTitle();	
  hframe0->GetYaxis()->SetTitle("Survival probability (%)");
  //hframe0->GetYaxis()->CenterTitle();
 
  TGraphErrors *g1,*g11,*g12,*g13;  
  g1=new TGraphErrors(11,aa_eta,cc_eta,err_aa_eta,err_cc_eta);
  g1->SetMarkerStyle(26);
  g1->SetMarkerColor(1);
  g1->SetLineColor(1);
  g1->SetLineStyle(1);
  g1->SetLineWidth(1.2);
  g1->SetMarkerSize(3.0);
  g1->Draw("p");
  g11=new TGraphErrors(11,aa_eta,ee_eta,err_aa_eta,err_ee_eta);
  g11->SetMarkerStyle(26);
  g11->SetMarkerColor(2);
  g11->SetLineColor(2);
  g11->SetLineStyle(1);
  g11->SetLineWidth(1.2);
  g11->SetMarkerSize(3.0);
  g11->Draw("p");
  g12=new TGraphErrors(11,aa_eta,ff_eta,err_aa_eta,err_ff_eta);
  g12->SetMarkerStyle(26);
  g12->SetMarkerColor(3);
  g12->SetLineColor(3);
  g12->SetLineStyle(1);
  g12->SetLineWidth(1.2);
  g12->SetMarkerSize(3.0);
  if(tag==1 or 3) g12->Draw("p");
  g13=new TGraphErrors(11,aa_eta,hh_eta,err_aa_eta,err_hh_eta);
  g13->SetMarkerStyle(26);
  g13->SetMarkerColor(4);
  g13->SetLineColor(4);
  g13->SetLineStyle(1);
  g13->SetLineWidth(1.2);
  g13->SetMarkerSize(3.0);
  g13->Draw("p");
  
  TLegend *legtitle1 =new TLegend(0.20,0.85,0.35,0.92,"#it{#bf{sPHENIX}} Simulation Internal");
  legtitle1->SetTextSize(0.035);
  legtitle1->Draw();
  
  TLegend *legtitle11;
  if(tag==2)legtitle11 =new TLegend(0.20,0.82,0.35,0.85,"Embed");
  if(tag==1 or 3)legtitle11 =new TLegend(0.20,0.82,0.35,0.85,"Single particle");
  legtitle11->SetTextSize(0.030);
  legtitle11->Draw();
  
  TLegend *legtitle12 =new TLegend(0.20,0.78,0.35,0.80,"@:0.7<E_{EMCal}/p<1.5 & E_{iHCal}/E_{EMCal}<0.2");
  legtitle12->SetTextSize(0.028);
  legtitle12->Draw();
  
  TLegend *legtitle13 =new TLegend(0.20,0.72,0.35,0.76,"@:p_{t}>2GeV");
  legtitle13->SetTextSize(0.028);
  legtitle13->Draw();
   
  TLegend *leg1 =new TLegend(0.65,0.63,0.90,0.92);
  leg1->AddEntry(g1," e^{+} & e^{-}","lep");
  leg1->AddEntry(g11," #pi^{+} & #pi^{-}","lep");
  if(tag==1 or 3)leg1->AddEntry(g12," K^{+} & K^{-}","lep");
  leg1->AddEntry(g13," p & #bar{p}","lep");
  leg1->Draw();
   
 ///////////////////////////////////////////////////////// canv->cd(2); 
  canv->cd(2);
  TPad *pad2 = new TPad("pad2","pad2",0,0,0.99,0.99);
  pad2->Draw();    
  pad2->cd();
 // pad2->SetFrameLineWidth(2);
 // pad2->SetFrameLineColor(1);

  Ymin=-0.05;
  if(tag==2)Ymax=2.0;
  if(tag==1 or 3)Ymax=2.0;
  Xmin=0.0;
  Xmax=16.0;  
 
  
  hframe0=gPad->DrawFrame(Xmin,Ymin,Xmax,Ymax);
  hframe0->GetXaxis()->SetTitle("p_{t} (GeV)"); 
//  hframe0->GetXaxis()->CenterTitle();	
  hframe0->GetYaxis()->SetTitle("Survival probability (%)");
//  hframe0->GetYaxis()->CenterTitle();
  
  
  TGraphErrors *g2,*g21,*g22,*g23;  
  g2=new TGraphErrors(16,aa_pt,cc_pt,err_aa_pt,err_cc_pt);
  g2->SetMarkerStyle(26);
  g2->SetMarkerColor(1);
  g2->SetLineColor(1);
  g2->SetLineStyle(1);
  g2->SetLineWidth(1.2);
  g2->SetMarkerSize(3.0);
  g2->Draw("p");
  g21=new TGraphErrors(16,aa_pt,ee_pt,err_aa_pt,err_ee_pt);
  g21->SetMarkerStyle(26);
  g21->SetMarkerColor(2);
  g21->SetLineColor(2);
  g21->SetLineStyle(1);
  g21->SetLineWidth(1.2);
  g21->SetMarkerSize(3.0);
  g21->Draw("p");
  g22=new TGraphErrors(16,aa_pt,ff_pt,err_aa_pt,err_ff_pt);
  g22->SetMarkerStyle(26);
  g22->SetMarkerColor(3);
  g22->SetLineColor(3);
  g22->SetLineStyle(1);
  g22->SetLineWidth(1.2);
  g22->SetMarkerSize(3.0);
  if(tag==1 or 3) g22->Draw("p");
  g23=new TGraphErrors(16,aa_pt,hh_pt,err_aa_pt,err_hh_pt);
  g23->SetMarkerStyle(26);
  g23->SetMarkerColor(4);
  g23->SetLineColor(4);
  g23->SetLineStyle(1);
  g23->SetLineWidth(1.2);
  g23->SetMarkerSize(3.0);
  g23->Draw("p");
  
  TLegend *legtitle2 =new TLegend(0.20,0.85,0.35,0.92,"#it{#bf{sPHENIX}} Simulation Internal");
  legtitle2->SetTextSize(0.035);
  legtitle2->Draw();
  
  TLegend *legtitle21;
  if(tag==2)legtitle21 =new TLegend(0.20,0.82,0.35,0.85,"Embed");
  if(tag==1 or 3)legtitle21 =new TLegend(0.20,0.82,0.35,0.85,"Single particle");
  legtitle21->SetTextSize(0.030);
  legtitle21->Draw();
  
  TLegend *legtitle22 =new TLegend(0.20,0.78,0.35,0.80,"@:0.7<E_{EMCal}/p<1.5 & E_{iHCal}/E_{EMCal}<0.2");
  legtitle22->SetTextSize(0.025);
  legtitle22->Draw();
  
  TLegend *leg2 =new TLegend(0.65,0.63,0.90,0.92);
  leg2->AddEntry(g2," e^{+} & e^{-}","lep");
  leg2->AddEntry(g21," #pi^{+} & #pi^{-}","lep");
  if(tag==1 or 3) leg2->AddEntry(g22," K^{+} & K^{-}","lep");
  leg2->AddEntry(g23," p & #bar{p}","lep");
  leg2->Draw();
  
 ///////////////////////////////////////////////////////// canv->cd(3);
  canv->cd(3);
  TPad *pad3 = new TPad("pad3","pad3",0,0,0.99,0.99);
  pad3->Draw();    
  pad3->cd();

  Ymin=0;
  Ymax=1.2;
  Xmin=0.0;
  Xmax=1.2;  
    
  hframe0=gPad->DrawFrame(Xmin,Ymin,Xmax,Ymax);
  hframe0->GetXaxis()->SetTitle("E_{iHCal}/E_{EMCal}<x_{cut}"); 
 // hframe0->GetXaxis()->CenterTitle();	
  hframe0->GetYaxis()->SetTitle("Survival probability (%)");
 // hframe0->GetYaxis()->CenterTitle();
 
  
  TGraphErrors *g3,*g31,*g32,*g33;  
  g3=new TGraphErrors(20,aa_HOM,cc_HOM,err_aa_HOM,err_cc_HOM);
  g3->SetMarkerStyle(26);
  g3->SetMarkerColor(1);
  g3->SetLineColor(1);
  g3->SetLineStyle(1);
  g3->SetLineWidth(1.2);
  g3->SetMarkerSize(3.0);
  g3->Draw("pc");
  g31=new TGraphErrors(20,aa_HOM,ee_HOM,err_aa_HOM,err_ee_HOM);
  g31->SetMarkerStyle(26);
  g31->SetMarkerColor(2);
  g31->SetLineColor(2);
  g31->SetLineStyle(1);
  g31->SetLineWidth(1.2);
  g31->SetMarkerSize(3.0);
  g31->Draw("pc");
  g32=new TGraphErrors(20,aa_HOM,ff_HOM,err_aa_HOM,err_ff_HOM);
  g32->SetMarkerStyle(26);
  g32->SetMarkerColor(3);
  g32->SetLineColor(3);
  g32->SetLineStyle(1);
  g32->SetLineWidth(1.2);
  g32->SetMarkerSize(3.0);
  if(tag==1 or 3) g32->Draw("pc");
  g33=new TGraphErrors(20,aa_HOM,hh_HOM,err_aa_HOM,err_hh_HOM);
  g33->SetMarkerStyle(26);
  g33->SetMarkerColor(4);
  g33->SetLineColor(4);
  g33->SetLineStyle(1);
  g33->SetLineWidth(1.2);
  g33->SetMarkerSize(3.0);
  g33->Draw("pc");
  
  TLegend *legtitle3 =new TLegend(0.20,0.85,0.35,0.92,"#it{#bf{sPHENIX}} Simulation Internal");
  legtitle3->SetTextSize(0.035);
  legtitle3->Draw();
  
  TLegend *legtitle31;
  if(tag==2)legtitle31 =new TLegend(0.20,0.82,0.35,0.85,"Embed");
  if(tag==1 or 3)legtitle31 =new TLegend(0.20,0.82,0.35,0.85,"Single particle");
  legtitle31->SetTextSize(0.030);
  legtitle31->Draw();
  
  TLegend *leg3;
 // if(tag==2) leg3 =new TLegend(0.65,0.60,0.90,0.90);
  if(tag==3 or 2) leg3 =new TLegend(0.70,0.28,0.78,0.58);
  leg3->AddEntry(g3," e^{+} & e^{-}","ep");
  leg3->AddEntry(g31," #pi^{+} & #pi^{-}","ep");
  if(tag==1 or 3) leg3->AddEntry(g32," K^{+} & K^{-}","ep");
  leg3->AddEntry(g33," p & #bar{p}","ep");
  leg3->Draw();

 ///////////////////////////////////////////////////////// canv->cd(4);
  canv->cd(4);
  TPad *pad4 = new TPad("pad4","pad4",0,0,0.99,0.99);
  pad4->Draw();    
  pad4->cd();

  Ymin=0;
  Ymax=1.8;
  Xmin=0.0;
  Xmax=1.2;  
   
  hframe0=gPad->DrawFrame(Xmin,Ymin,Xmax,Ymax);
  hframe0->GetXaxis()->SetTitle("x_{cut}<E_{EMCal}/p<1.5"); 
  //hframe0->GetXaxis()->CenterTitle();	
  hframe0->GetYaxis()->SetTitle("Survival probability (%)");
  //hframe0->GetYaxis()->CenterTitle();
   
  TGraphErrors *g4,*g41,*g42,*g43;  
  g4=new TGraphErrors(9,aa_EOP,cc_EOP,err_aa_EOP,err_cc_EOP);
  g4->SetMarkerStyle(26);
  g4->SetMarkerColor(1);
  g4->SetLineColor(1);
  g4->SetLineStyle(1);
  g4->SetLineWidth(1.2);
  g4->SetMarkerSize(3.0);
  g4->Draw("pc");
  g41=new TGraphErrors(9,aa_EOP,ee_EOP,err_aa_EOP,err_ee_EOP);
  g41->SetMarkerStyle(26);
  g41->SetMarkerColor(2);
  g41->SetLineColor(2);
  g41->SetLineStyle(1);
  g41->SetLineWidth(1.2);
  g41->SetMarkerSize(3.0);
  g41->Draw("pc");
  g42=new TGraphErrors(9,aa_EOP,ff_EOP,err_aa_EOP,err_ff_EOP);
  g42->SetMarkerStyle(26);
  g42->SetMarkerColor(3);
  g42->SetLineColor(3);
  g42->SetLineStyle(1);
  g42->SetLineWidth(1.2);
  g42->SetMarkerSize(3.0);
  if(tag==1 or 3) g42->Draw("pc");
  g43=new TGraphErrors(9,aa_EOP,hh_EOP,err_aa_EOP,err_hh_EOP);
  g43->SetMarkerStyle(26);
  g43->SetMarkerColor(4);
  g43->SetLineColor(4);
  g43->SetLineStyle(1);
  g43->SetLineWidth(1.2);
  g43->SetMarkerSize(3.0);
  g43->Draw("pc");
  
  TLegend *legtitle4 =new TLegend(0.20,0.85,0.35,0.92,"#it{#bf{sPHENIX}} Simulation Internal");
  legtitle4->SetTextSize(0.035);
  legtitle4->Draw();
  
  TLegend *legtitle41;
  if(tag==2)legtitle41 =new TLegend(0.20,0.82,0.35,0.85,"Embed");
  if(tag==1 or 3)legtitle41 =new TLegend(0.20,0.82,0.35,0.85,"Single particle");
  legtitle41->SetTextSize(0.030);
  legtitle41->Draw();
  
  TLegend *leg4 =new TLegend(0.65,0.60,0.90,0.90);
  leg4->AddEntry(g4," e^{+} & e^{-}","ep");
  leg4->AddEntry(g41," #pi^{+} & #pi^{-}","ep");
  if(tag==1 or 3) leg4->AddEntry(g42," K^{+} & K^{-}","ep");
  leg4->AddEntry(g43," p & #bar{p}","ep");
  leg4->Draw();
  
 ///////////////////////////////////////////////////////// canv->cd(5); 
  canv->cd(5);
  TPad *pad5 = new TPad("pad5","pad5",0,0,0.99,0.99);
  pad5->Draw();    
  pad5->cd();
 // pad5->SetFrameLineWidth(2);
 // pad5->SetFrameLineColor(1);

  Ymin=0.0;
  Ymax=1.3;
  Xmin=0.0;
  Xmax=1.2;  
  
  hframe0=gPad->DrawFrame(Xmin,Ymin,Xmax,Ymax);
  hframe0->GetXaxis()->SetTitle("(E_{iHCal}+E_{oHCal})/E_{EMCal}>x_{cut}"); 
 // hframe0->GetXaxis()->CenterTitle();
  hframe0->GetYaxis()->SetTitle("Survival probability (%)");
 // hframe0->GetYaxis()->CenterTitle();
  
  TGraphErrors *g5,*g51,*g52,*g53;  
  g5=new TGraphErrors(20,aa_HOP,cc_HOP,err_aa_HOP,err_cc_HOP);
  g5->SetMarkerStyle(26);
  g5->SetMarkerColor(1);
  g5->SetLineColor(1);
  g5->SetLineStyle(1);
  g5->SetLineWidth(1.2);
  g5->SetMarkerSize(3.0);
  g5->Draw("pc");
  g51=new TGraphErrors(20,aa_HOP,ee_HOP,err_aa_HOP,err_ee_HOP);
  g51->SetMarkerStyle(26);
  g51->SetMarkerColor(2);
  g51->SetLineColor(2);
  g51->SetLineStyle(1);
  g51->SetLineWidth(1.2);
  g51->SetMarkerSize(3.0);
  g51->Draw("pc");
  g52=new TGraphErrors(20,aa_HOP,ff_HOP,err_aa_HOP,err_ff_HOP);
  g52->SetMarkerStyle(26);
  g52->SetMarkerColor(3);
  g52->SetLineColor(3);
  g52->SetLineStyle(1);
  g52->SetLineWidth(1.2);
  g52->SetMarkerSize(3.0);
  if(tag==1 or 3) g52->Draw("pc");
  g53=new TGraphErrors(20,aa_HOP,hh_HOP,err_aa_HOP,err_hh_HOP);
  g53->SetMarkerStyle(26);
  g53->SetMarkerColor(4);
  g53->SetLineColor(4);
  g53->SetLineStyle(1);
  g53->SetLineWidth(1.2);
  g53->SetMarkerSize(3.0);
  g53->Draw("pc");
  
  TLegend *legtitle5 =new TLegend(0.20,0.85,0.35,0.92,"#it{#bf{sPHENIX}} Simulation Internal");
  legtitle5->SetTextSize(0.035);
  legtitle5->Draw();
  
  TLegend *legtitle51;
  if(tag==2)legtitle51 =new TLegend(0.20,0.82,0.35,0.85,"Embed");
  if(tag==1 or 3)legtitle51 =new TLegend(0.20,0.82,0.35,0.85,"Single particle");
  legtitle51->SetTextSize(0.030);
  legtitle51->Draw();
  
  TLegend *leg5 =new TLegend(0.65,0.60,0.90,0.90);
  leg5->AddEntry(g5," e^{+} & e^{-}","ep");
  leg5->AddEntry(g51," #pi^{+} & #pi^{-}","ep");
  if(tag==1 or 3) leg5->AddEntry(g52," K^{+} & K^{-}","ep");
  leg5->AddEntry(g53," p & #bar{p}","ep");
  leg5->Draw();
  
  ///////////////////////////////////////////////////////// canv->cd(6); 
  canv->cd(6);
  TPad *pad6 = new TPad("pad6","pad6",0,0,0.99,0.99);
  pad6->Draw();    
  pad6->cd();
 
  if(tag==2){
    Ymin=1.0;
    Ymax=10000.0;
    Xmin=0.6;
    Xmax=1.0;  
  }
  if(tag==1 or 3){
    Ymin=1.0;
    Ymax=100000.0;
    Xmin=0.45;
    Xmax=1.0;  
  }
  gPad->SetLogy();
   
  hframe0=gPad->DrawFrame(Xmin,Ymin,Xmax,Ymax);
  hframe0->GetXaxis()->SetTitle("eID-Eff.-E_{EMCal}/p(%)"); 
 // hframe0->GetXaxis()->CenterTitle();	
  hframe0->GetYaxis()->SetTitle("Rejection");
 // hframe0->GetYaxis()->CenterTitle();
 
  
  TGraphErrors *g61,*g62,*g63;  
  g61=new TGraphErrors(9,cc_EOP,ree_EOP,err_cc_EOP,err_ree_EOP);
  g61->SetMarkerStyle(26);
  g61->SetMarkerColor(2);
  g61->SetLineColor(2);
  g61->SetLineStyle(1);
  g61->SetLineWidth(1.2);
  g61->SetMarkerSize(3.0);
  g61->Draw("pl");
  g62=new TGraphErrors(9,cc_EOP,rff_EOP,err_cc_EOP,err_rff_EOP);
  g62->SetMarkerStyle(26);
  g62->SetMarkerColor(3);
  g62->SetLineColor(3);
  g62->SetLineStyle(1);
  g62->SetLineWidth(1.2);
  g62->SetMarkerSize(3.0);
  if(tag==1 or 3) g62->Draw("pl");
  g63=new TGraphErrors(9,cc_EOP,rhh_EOP,err_cc_EOP,err_rhh_EOP);
  g63->SetMarkerStyle(26);
  g63->SetMarkerColor(4);
  g63->SetLineColor(4);
  g63->SetLineStyle(1);
  g63->SetLineWidth(1.2);
  g63->SetMarkerSize(3.0);
  g63->Draw("pl");
  
  TLegend *legtitle6 =new TLegend(0.20,0.85,0.35,0.92,"#it{#bf{sPHENIX}} Simulation Internal");
  legtitle6->SetTextSize(0.035);
  legtitle6->Draw();
  
  TLegend *legtitle61;
  if(tag==2)legtitle61 =new TLegend(0.20,0.82,0.35,0.85,"Embed");
  if(tag==1 or 3)legtitle61 =new TLegend(0.20,0.82,0.35,0.85,"Single particle");
  legtitle61->SetTextSize(0.030);
  legtitle61->Draw();
  
  TLegend *leg6 =new TLegend(0.65,0.65,0.90,0.90);
  leg6->AddEntry(g61," #pi^{+} & #pi^{-}","lep");
  if(tag==1 or 3) leg6->AddEntry(g62," K^{+} & K^{-}","lep");
  leg6->AddEntry(g63," p & #bar{p}","lep");
  leg6->Draw();
  
  ///////////////////////////////////////////////////////// canv->cd(7); 
  canv->cd(7);
  TPad *pad7 = new TPad("pad7","pad7",0,0,0.99,0.99);
  pad7->Draw();    
  pad7->cd();
  if(tag==1){
    Ymin=1.0;
    Ymax=50.0;
    Xmin=0.88;
    Xmax=1.0;  
  }
  if(tag==2){
    Ymin=1.0;
    Ymax=50;
    Xmin=0.7;
    Xmax=1.0;  
  }
  if(tag==3){
    Ymin=1.0;
    Ymax=50.0;
    Xmin=0.85;
    Xmax=0.93;  
  }
  gPad->SetLogy();
  
  hframe0=gPad->DrawFrame(Xmin,Ymin,Xmax,Ymax);
  hframe0->GetXaxis()->SetTitle("eID-Eff.-E_{iHCal}/E_{EMCal}(%)"); 
 // hframe0->GetXaxis()->CenterTitle();	
  hframe0->GetYaxis()->SetTitle("Rejection");
 // hframe0->GetYaxis()->CenterTitle();
 
  
  TGraphErrors *g71,*g72,*g73;  
  g71=new TGraphErrors(20,cc_HOM,ree_HOM,err_cc_HOM,err_ree_HOM);
  g71->SetMarkerStyle(26);
  g71->SetMarkerColor(2);
  g71->SetLineColor(2);
  g71->SetLineStyle(1);
  g71->SetLineWidth(1.2);
  g71->SetMarkerSize(3.0);
  g71->Draw("pl");
  g72=new TGraphErrors(20,cc_HOM,rff_HOM,err_cc_HOM,err_rff_HOM);
  g72->SetMarkerStyle(26);
  g72->SetMarkerColor(3);
  g72->SetLineColor(3);
  g72->SetLineStyle(1);
  g72->SetLineWidth(1.2);
  g72->SetMarkerSize(3.0);
  if(tag==1 or 3) g72->Draw("pl");
  g73=new TGraphErrors(20,cc_HOM,rhh_HOM,err_cc_HOM,err_rhh_HOM);
  g73->SetMarkerStyle(26);
  g73->SetMarkerColor(4);
  g73->SetLineColor(4);
  g73->SetLineStyle(1);
  g73->SetLineWidth(1.2);
  g73->SetMarkerSize(3.0);
  g73->Draw("pl");
  
  TLegend *legtitle7 =new TLegend(0.20,0.85,0.35,0.92,"#it{#bf{sPHENIX}} Simulation Internal");
  legtitle7->SetTextSize(0.035);
  legtitle7->Draw();
  
  TLegend *legtitle71;
  if(tag==2)legtitle71 =new TLegend(0.20,0.82,0.35,0.85,"Embed");
  if(tag==1 or 3)legtitle71 =new TLegend(0.20,0.82,0.35,0.85,"Single particle");
  legtitle71->SetTextSize(0.030);
  legtitle71->Draw();
  
  TLegend *leg7 =new TLegend(0.65,0.65,0.90,0.90);
  leg7->SetBorderSize(0);
  leg7->AddEntry(g71," #pi^{+} & #pi^{-}","lep");
  if(tag==1 or 3) leg7->AddEntry(g72," K^{+} & K^{-}","lep");
  leg7->AddEntry(g73," p & #bar{p}","lep");
  leg7->Draw();    
  
  ///////////////////////////////////////////////////////// canv->cd(8); 
  canv->cd(8);
  TPad *pad8 = new TPad("pad8","pad8",0,0,0.99,0.99);
  pad8->Draw();    
  pad8->cd();
  if(tag==2){
    Ymin=1.0;
    Ymax=20.0;
    Xmin=0.0;
    Xmax=0.4;  
  }
  if(tag==1 or 3){
    Ymin=1.0;
    Ymax=500.0;
    Xmin=0.0;
    Xmax=0.06;  
  }
  gPad->SetLogy();
    
  hframe0=gPad->DrawFrame(Xmin,Ymin,Xmax,Ymax);
  hframe0->GetXaxis()->SetTitle("eID-Eff.-(E_{iHCal}+E_{oHCal})/E_{EMCal}(%)"); 
 // hframe0->GetXaxis()->CenterTitle();	
  hframe0->GetYaxis()->SetTitle("Rejection");
 // hframe0->GetYaxis()->CenterTitle();
 
  
  TGraphErrors *g81,*g82,*g83;  
  g81=new TGraphErrors(20,cc_HOP,ree_HOP,err_cc_HOP,err_ree_HOP);
  g81->SetMarkerStyle(26);
  g81->SetMarkerColor(2);
  g81->SetLineColor(2);
  g81->SetLineStyle(1);
  g81->SetLineWidth(1.2);
  g81->SetMarkerSize(3.0);
  g81->Draw("pl");
  g82=new TGraphErrors(20,cc_HOP,rff_HOP,err_cc_HOP,err_rff_HOP);
  g82->SetMarkerStyle(26);
  g82->SetMarkerColor(3);
  g82->SetLineColor(3);
  g82->SetLineStyle(1);
  g82->SetLineWidth(1.2);
  g82->SetMarkerSize(3.0);
  if(tag==1 or 3) g82->Draw("pl");
  g83=new TGraphErrors(20,cc_HOP,rhh_HOP,err_cc_HOP,err_rhh_HOP);
  g83->SetMarkerStyle(26);
  g83->SetMarkerColor(4);
  g83->SetLineColor(4);
  g83->SetLineStyle(1);
  g83->SetLineWidth(1.2);
  g83->SetMarkerSize(3.0);
  g83->Draw("pl");
  
  TLegend *legtitle8 =new TLegend(0.20,0.85,0.35,0.92,"#it{#bf{sPHENIX}} Simulation Internal");
  legtitle8->SetTextSize(0.035);
  legtitle8->Draw();
  
  TLegend *legtitle81;
  if(tag==2)legtitle81 =new TLegend(0.20,0.82,0.35,0.85,"Embed");
  if(tag==1 or 3)legtitle81 =new TLegend(0.20,0.82,0.35,0.85,"Single particle");
  legtitle81->SetTextSize(0.030);
  legtitle81->Draw();
  
  TLegend *leg8 =new TLegend(0.65,0.65,0.90,0.90);
  leg8->AddEntry(g81," #pi^{+} & #pi^{-}","lep");
  if(tag==1 or 3) leg8->AddEntry(g82," K^{+} & K^{-}","lep");
  leg8->AddEntry(g83," p & #bar{p}","lep");
  leg8->Draw(); 
  
  ///////////////////////////////////////////////////////// canv->cd(9); 
  canv->cd(9);
  TPad *pad9 = new TPad("pad9","pad9",0,0,0.99,0.99);
  pad9->Draw();    
  pad9->cd();
  if(tag==2){
    Ymin=1.0;
    Ymax=5000.0;
    Xmin=0.0;
    Xmax=1.0;  
  }
  if(tag==1 or 3){
    Ymin=1.0;
    Ymax=30000.0;
    Xmin=0.0;
    Xmax=1.0;  
  } 
  gPad->SetLogy();
   
  hframe0=gPad->DrawFrame(Xmin,Ymin,Xmax,Ymax);
  hframe0->GetXaxis()->SetTitle("eID-Eff.(%)"); 
 // hframe0->GetXaxis()->CenterTitle();	
  hframe0->GetYaxis()->SetTitle("Rejection");
 // hframe0->GetYaxis()->CenterTitle();
 
  
  TGraphErrors *g91,*g92,*g93;  
  if(tag==1 or 3)g91=new TGraphErrors(11,cc_eta,ree_eta,err_cc_eta,err_ree_eta);
  if(tag==2)g91=new TGraphErrors(10,cc_eta,ree_eta,err_cc_eta,err_ree_eta);
  g91->SetMarkerStyle(26);
  g91->SetMarkerColor(2);
  g91->SetLineColor(2);
  g91->SetLineStyle(1);
  g91->SetLineWidth(1.2);
  g91->SetMarkerSize(3.0);
  g91->Draw("p");
  g92=new TGraphErrors(11,cc_eta,rff_eta,err_cc_eta,err_rff_eta);
  g92->SetMarkerStyle(26);
  g92->SetMarkerColor(3);
  g92->SetLineColor(3);
  g92->SetLineStyle(1);
  g92->SetLineWidth(1.2);
  g92->SetMarkerSize(3.0);
  if(tag==1 or 3) g92->Draw("p");
  if(tag==1 or 3)g93=new TGraphErrors(11,cc_eta,rhh_eta,err_cc_eta,err_rhh_eta);
  if(tag==2)g93=new TGraphErrors(10,cc_eta,rhh_eta,err_cc_eta,err_rhh_eta);
  g93->SetMarkerStyle(26);
  g93->SetMarkerColor(4);
  g93->SetLineColor(4);
  g93->SetLineStyle(1);
  g93->SetLineWidth(1.2);
  g93->SetMarkerSize(3.0);
  g93->Draw("p");
  
  TLegend *legtitle9 =new TLegend(0.20,0.85,0.35,0.92,"#it{#bf{sPHENIX}} Simulation Internal");
  legtitle9->SetTextSize(0.035);
  legtitle9->Draw();
  
  TLegend *legtitle91;
  if(tag==2)legtitle91 =new TLegend(0.20,0.82,0.35,0.85,"Embed");
  if(tag==1 or 3)legtitle91 =new TLegend(0.20,0.82,0.35,0.85,"Single particle");
  legtitle91->SetTextSize(0.030);
  legtitle91->Draw();
  
  TLegend *legtitle92 =new TLegend(0.20,0.78,0.35,0.80,"@:0.7<E_{EMCal}/p<1.5 & E_{iHCal}/E_{EMCal}<0.2");
  legtitle92->SetTextSize(0.028);
  legtitle92->Draw();
  
  TLegend *legtitle93 =new TLegend(0.20,0.72,0.35,0.76,"@:p_{t}>2GeV");
  legtitle93->SetTextSize(0.028);
  legtitle93->Draw();
  
  TLegend *leg9 =new TLegend(0.65,0.65,0.90,0.90);
  leg9->AddEntry(g91," #pi^{+} & #pi^{-}","lep");
  if(tag==1 or 3) leg9->AddEntry(g92," K^{+} & K^{-}","lep");
  leg9->AddEntry(g93," p & #bar{p}","lep");
  leg9->Draw();
  
  ///////////////////////////////////////////////////////// canv->cd(10); 
  canv->cd(10);
  TPad *pad10 = new TPad("pad10","pad10",0,0,0.99,0.99);
  pad10->Draw();    
  pad10->cd();
  if(tag==2){
    Ymin=1.0;
    Ymax=5000.0;
    Xmin=0.0;
    Xmax=1.0;  
  }
  if(tag==1 or 3){
    Ymin=1.0;
    Ymax=30000.0;
    Xmin=0.3;
    Xmax=1.0;  
  } 
  gPad->SetLogy();
   
  hframe0=gPad->DrawFrame(Xmin,Ymin,Xmax,Ymax);
  hframe0->GetXaxis()->SetTitle("eID-Eff.-p_{t}(%)"); 
 // hframe0->GetXaxis()->CenterTitle();	
  hframe0->GetYaxis()->SetTitle("Rejection");
 // hframe0->GetYaxis()->CenterTitle();
 
  
  TGraphErrors *g101,*g102,*g103;  
  if(tag==1 or 3)g101=new TGraphErrors(16,cc_pt,ree_pt,err_cc_pt,err_ree_pt);
  if(tag==2)g101=new TGraphErrors(14,cc_pt,ree_pt,err_cc_pt,err_ree_pt);
  g101->SetMarkerStyle(26);
  g101->SetMarkerColor(2);
  g101->SetLineColor(2);
  g101->SetLineStyle(1);
  g101->SetLineWidth(1.2);
  g101->SetMarkerSize(3.0);
  g101->Draw("p");
  g102=new TGraphErrors(16,cc_pt,rff_pt,err_cc_pt,err_rff_pt);
  g102->SetMarkerStyle(26);
  g102->SetMarkerColor(3);
  g102->SetLineColor(3);
  g102->SetLineStyle(1);
  g102->SetLineWidth(1.2);
  g102->SetMarkerSize(3.0);
  if(tag==1 or 3) g102->Draw("p");
  if(tag==1 or 3)g103=new TGraphErrors(16,cc_pt,rhh_pt,err_cc_pt,err_rhh_pt);
  if(tag==2)g103=new TGraphErrors(14,cc_pt,rhh_pt,err_cc_pt,err_rhh_pt);
  g103->SetMarkerStyle(26);
  g103->SetMarkerColor(4);
  g103->SetLineColor(4);
  g103->SetLineStyle(1);
  g103->SetLineWidth(1.2);
  g103->SetMarkerSize(3.0);
  g103->Draw("p");
  
  TLegend *legtitle10 =new TLegend(0.20,0.85,0.35,0.92,"#it{#bf{sPHENIX}} Simulation Internal");
  legtitle10->SetTextSize(0.035);
  legtitle10->Draw();
  
  TLegend *legtitle101;
  if(tag==2)legtitle101 =new TLegend(0.20,0.82,0.35,0.85,"Embed");
  if(tag==1 or 3)legtitle101 =new TLegend(0.20,0.82,0.35,0.85,"Single particle");
  legtitle101->SetTextSize(0.030);
  legtitle101->Draw();
  
  TLegend *legtitle102 =new TLegend(0.20,0.78,0.35,0.80,"@:0.7<E_{EMCal}/p<1.5 & E_{iHCal}/E_{EMCal}<0.2");
  legtitle102->SetTextSize(0.028);
  legtitle102->Draw();
  
  TLegend *leg10 =new TLegend(0.65,0.65,0.90,0.90);
  leg10->AddEntry(g101," #pi^{+} & #pi^{-}","lep");
  if(tag==1 or 3) leg10->AddEntry(g102," K^{+} & K^{-}","lep");
  leg10->AddEntry(g103," p & #bar{p}","lep");
  leg10->Draw();
  
  
  canv->RedrawAxis();
  TString psname1;
  psname1=Form("%s.pdf",output_plot_eID);
  canv->Print(psname1);
  
  
  /////////////////////////////////////////////////////////////////plot only one separately 
  TCanvas *cv_EOP= new TCanvas("cv_EOP","cv_EOP Canvas",800,800);
  cv_EOP->cd();
  TPad *pad = new TPad("pad","pad",0,0,0.99,0.99);
  pad->Draw();    
  pad->cd();
  
  Ymin=0;
  Ymax=1.8;
  Xmin=0.0;
  Xmax=1.2; 
  
  hframe0=gPad->DrawFrame(Xmin,Ymin,Xmax,Ymax);
  hframe0->GetXaxis()->SetTitle("x_{cut}<E_{EMCal}/p<1.5"); 
  hframe0->GetYaxis()->SetTitle("Survival probability (%)");
  
  g4->Draw("pc");
  g41->Draw("pc");
  if(tag==1 or 3)g42->Draw("pc");
  g43->Draw("pc");
  
  legtitle4->Draw();
  legtitle41->Draw();
  leg4->Draw();
  
  cv_EOP->RedrawAxis();
  TString psname2,psname3;
  psname2=Form("%s_EOP.pdf",output_plot_eID);
  psname3=Form("%s_EOP.png",output_plot_eID);
  cv_EOP->Print(psname2);
  cv_EOP->Print(psname3);
  
}

