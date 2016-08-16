#include <iostream>
#include "hcal.h"
#include <algorithm>
#include <cmath>
#include <TMath.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TFile.h>
#include <TROOT.h>
#include <TStyle.h>
#include <Event/EventTypes.h>
#include <Event/packetConstants.h>
#include <Event/packet.h>
#include <Event/packet_hbd_fpgashort.h>

using namespace std;

//_________________________________________
hcal::hcal()
{
 //Default values
 display = false;
 verbosity = false;
 polarity_positive = true; 
 PEDESTAL = 2048;
 plot_max = PEDESTAL + 100.;
 plot_min = 0.;
 fit_analysis = true;
}

//________________________________________
void hcal::Initialize()
{
  TString name;
  cout << "Reading " << NCH << " Channels." << endl; 
  NSAMPLES = 24; 
  double max = 3000;
  for(int ich=0; ich<NCH; ich++)
    {
      gpulse[ich] = new TGraph(NSAMPLES);
      name = "gch"; name += ich;
      gpulse[ich]->SetName(name);
      gpulse[ich]->SetMarkerStyle(20);
      gpulse[ich]->SetMarkerSize(0.4);

      name = "Signal_"; 
      name+= ich;
      h_PulsePeaks[ich] = new TH1F(name, name, (int)4*max, 0., max);
      h_PulsePeaks[ich]->SetStats(kFALSE);

      //Pulse Int
      name = "SignalInt_";
      name+= ich;
      double int_max = max*24./6. ;
      h_PulseInt[ich] = new TH1F(name, name, (int)int_max, 0., int_max );

      //2D Integrated signal vs signal height
      name = "Signal_Height_vs_Int_";
      name += ich;
      h2_Pulse_H_Int[ich] = new TH2F(name, name, (int) max, 0., max, (int)int_max, 0., int_max);

      //Fits
      name = "SignalFit_";
      name+= ich;
      if(polarity_positive) fits[ich] = new TF1(name,hcal::SignalShape, 0., 24., 6);
      else fits[ich] = new TF1(name,hcal::SignalShapeNegative, 0., 24., 6);
    
      //Fit analysis
      if(fit_analysis)
      {
       name = "Fit_analyze_";
       name += ich;
       h2_fit_shape[ich] = new TH2F(name,name,48,0,24,2100,0,2100);
      }

      //Timing resolution
      name = "TResolution_";
      name += ich;
      h_tres[ich] = new TH1F(name, name, 24, 0, 24);

      //Pedestal 
      name = "Ped_Resolution_";
      name += ich;
      h_ped_res[ich] = new TH1F(name, name, 500,2000,2200);

      /*if(ich%2)
      {
       name = "HiLoGainRatio_";
       name += (int)(ich/2);
       h_gain[(int)ich/2] = new TH1F(name,name, 200, 0, 100);
       name = "HiLo2D_";
       name += (int)(ich/2);
       h_hilo[(int)ich/2] = new TH2F(name,name, 500, 0, 3000, 500, 0, 300);
      }*/
    }

  name = "Info";
  h_info = new TH1F(name, name, 10, 0, 10);
  //1st bin = Runnumber
  //2nd bin = Number of events ran
  //3rd bin = Start time
  //4th bin = End time
  // More to add later

  name = "Saturation_rates_vs_ch";
  h_saturation_rates = new TH1F(name, name, NCH, 0, NCH);

  cout << "Reading below HBD channels:  " << endl;
  for(int ich=0; ich<NCH; ich++) cout << channel_index[ich] << ", ";
  cout << endl;

  //Save the trigger rate
  rate = new TGraph();
  rate->SetNameTitle("Rate","Trigger Rate");
}

//____________________________________
int hcal::evLoop(int nevts=0)
{
 Initialize();
 runnumber = 0;
 nevents = 0;
 int OK;
 if(verbosity) cout << "hcal::evLoop" << endl;
 //Event iterator
 cout << "Reading file " << prdfName << endl;
 it =  new fileEventiterator(prdfName, OK);
 if (OK)
 {
  cout << "Couldn't open input file " << prdfName << endl;
  delete(it);
  exit(1);
 }

 double time_begin = 0.;
 double time_end = 0.;
 while(GetNextEvent())
 {
  if(verbosity) cout << "Event " << nevents << endl;
  if(display) Display();
  if(nevts>0&&nevents==nevts) break;
  if(nevents%100==0) 
  {
   time_end = (double) evt->getTime();
   if(nevents>100) rate->SetPoint((nevents/100)-2, nevents, 100./(time_end-time_begin));
   time_begin = time_end;
  }
 }
 cout << "Total number of events processed " << nevents << endl;
 h_info->SetBinContent(2, nevents);

 cout << "Saturation summary " << endl;
 for( int ich=0; ich<NCH; ich++)
 {
  cout << "Channel " << ich << " => " << 100*saturation_map[ich]/nevents << " % " <<endl;
  h_saturation_rates->Fill( h_saturation_rates->FindBin(ich), 100*saturation_map[ich]/nevents );
 }

 
 return 0;
}

//_____________________________________
int hcal::Clear()
{
 if(verbosity) cout << "hcal::Clear()" << endl;
  for (int ich=0; ich<NCH; ich++)
  {
    gpulse[ich]->Set(0);
    //if(fits[ich]) delete fits[ich];
  }
 return 0;
}

//______________________________________
int hcal::GetNextEvent()
{
  if(verbosity) cout << "hcal::GetNextEvent " << nevents << endl;
  nevents++; 
  //Reset the graphs
  Clear();
  //Get the event 
  evt=it->getNextEvent();
  if(!evt) return 0;
  if(evt->getEvtType()==9) 
  {
    cout << "Event " << nevents << endl;
    cout << "Start Time " << evt->getTime() << endl;
    cout << "Start Date " << evt->getDate() << endl;
    //Get Runnumber
    h_info->SetBinContent(1, evt->getRunNumber() );
    //Get Start Time
    h_info->SetBinContent(3, evt->getTime() ); 
  }
  else if(evt->getEvtType()==12)
  {
    //Get End Time
    h_info->SetBinContent(4, evt->getTime() );
  } else {
   // Get sPHENIX Packet
   Packet_hbd_fpgashort *spacket = dynamic_cast<Packet_hbd_fpgashort*>( evt->getPacket(21101) );
   if ( spacket )
   {
     spacket->setNumSamples( NSAMPLES );
     if(nevents%500==0)  cout <<"RUN  "<<runnumber<<"  EVENT "<< nevents <<endl;
     for(int ich=0; ich<NCH; ich++)
     {
      int hbd_channel = channel_index[ich];
      for (int isamp=0; isamp<NSAMPLES; isamp++)
      {
       //Short_t val = spacket->iValue(feech[ich],isamp);
       //gpulse[ich]->SetPoint(isamp,(Double_t)isamp,(Double_t)val);
        Short_t val = spacket->iValue(hbd_channel,isamp);
        gpulse[ich]->SetPoint(isamp,(Double_t)isamp,(Double_t)val);
      }
       
       if(!polarity_positive && TMath::MinElement(24,gpulse[ich]->GetY())==0)
       {
         //display = true;
         saturation_map[ich]++;
         if(verbosity) cout << "Saturated channel " << ich << endl;
       }
       else 
       { 
        //display = false;
        fitShape(ich);
        collect(ich);
       }
      }
       delete spacket;
    } else {
     if( nevents>1) cout << "Event: " << nevents << " Packet not found " << endl;
   }
  }
 return 1;
}

//____________________________________________
void hcal::fitShape(int ich)
{
 int peakPos = 0.;
 double peakval;
 if(polarity_positive) peakval = 0.;
 else peakval = 99999.;
 double pedestal = gpulse[ich]->GetY()[0]; //(double) PEDESTAL;
 double risetime = 4;
 for(int iSample=0; iSample<NSAMPLES; iSample++)
 {

   if(!polarity_positive && gpulse[ich]->GetY()[iSample]<peakval){
   //if(gpulse[ich]->GetY()[iSample]>peakval){
     peakval = gpulse[ich]->GetY()[iSample];
     peakPos = iSample;
   } else if(polarity_positive && gpulse[ich]->GetY()[iSample]>peakval)
   {
     peakval = gpulse[ich]->GetY()[iSample];
     peakPos = iSample;
   }
 }
  
 //peakval = pedestal - peakval;
 if(polarity_positive) peakval -= pedestal;
 else peakval = pedestal - peakval;
 if(peakval<0.) peakval = 0.;
 double par[6];
 par[0] = peakval; // /3.;
 par[1] = peakPos - risetime;
 if(par[1]<0.) par[1] = 0.;
 par[2] = 4.;
 par[3] = 1.5;
 par[4] = pedestal;
 par[5] = 0;
 fits[ich]->SetParameters(par);
 fits[ich]->SetParLimits(0,peakval*0.98,peakval*1.02);
 fits[ich]->SetParLimits(1,0,24);
 fits[ich]->SetParLimits(2, 2, 4.);
 fits[ich]->SetParLimits(3,1.,2.);
 fits[ich]->SetParLimits(4, pedestal-30, pedestal+30);
 fits[ich]->SetParLimits(5, 1, -1);
 gpulse[ich]->Fit( fits[ich], "MQR", "goff", 0., (double) NSAMPLES);
 //gpulse[ich]->Fit( fits[ich], "M0RQ", "goff", 0., (double) NSAMPLES);
 //cout << "CH " << ich << " sig: " << peakval << " Par 2: " << fits[ich]->GetParameter(2) << endl;
 //fits[ich]->Print();
}

//__________________________________________
int hcal::collect(int ich)
{ 
 if(nevents<10) return 0; //Leave first 10 events
 double min = fits[ich]->GetMinimum();
 double minx = fits[ich]->GetMinimumX();
 //double ped_val = fits[ich]->GetParameter(4)+minx*fits[ich]->GetParameter(5);
 //double peakvalue = ped_val - min;

 double max = fits[ich]->GetMaximum();
 double maxx = fits[ich]->GetMaximumX();
 double ped_val =0;
 if(polarity_positive) ped_val = fits[ich]->GetParameter(4)+maxx*fits[ich]->GetParameter(5);
 else ped_val = fits[ich]->GetParameter(4)+minx*fits[ich]->GetParameter(5);

 double peakvalue = 0;
 if(polarity_positive) peakvalue = max  - ped_val;
 else peakvalue = ped_val - min;

 double integral = 24*ped_val - fits[ich]->Integral(0,24);
 if(verbosity) cout << "Channel " << ich << " " << h_PulsePeaks[ich]->GetName() << " PED: " << ped_val << " Signal " << peakvalue << " Integrated signal: " << integral << endl;

 //cout << "peakpos " << fits[ich]->GetMinimumX() << endl;
 //cout << "Maximum " << fits[ich]->GetMaximum() << endl;
 h_PulsePeaks[ich]->Fill( peakvalue );
 h_PulseInt[ich]->Fill( integral );
 h2_Pulse_H_Int[ich]->Fill( peakvalue, integral );
 h_tres[ich]->Fill( (polarity_positive)?maxx:minx );
 h_ped_res[ich]->Fill( ped_val );
 /*if(ich%2)
 {
  double hi_peak = fits[ich-1]->GetParameter(4)+fits[ich-1]->GetMinimumX()*fits[ich-1]->GetParameter(5) - fits[ich-1]->GetMinimum();
  double lo_peak = fits[ich-1]->GetMaximum() - fits[ich-1]->GetParameter(4)+fits[ich-1]->GetMaximumX()*fits[ich-1]->GetParameter(5);
  if(polarity_positive)
  {
   h_gain[(int)ich/2]->Fill( peakvalue/lo_peak );
   h_hilo[(int)ich/2]->Fill( peakvalue, lo_peak );
  } else {
   h_gain[(int)ich/2]->Fill( hi_peak/peakvalue );
   h_hilo[(int)ich/2]->Fill( hi_peak, peakvalue );
  }
  //cout << "Channel " << ich << " " << (int)ich/2 << "  " << hi_peak/peakvalue << endl;
 }*/

 return 0;
}

//_________________________________________
void hcal::Display()
{
 if(nevents==1) return;
 if(gpulse[0]->GetN()==0) return;

 if(fit_analysis)
 {
  for(int ich=0; ich<NCH; ich++)
  {
   if(verbosity) fits[ich]->Print();
   fits[ich]->SetParameter(1,4.0);
   for(float x=0; x<48; x=x+0.5)
   {
    h2_fit_shape[ich]->Fill( x, fits[ich]->Eval(x) );
   }
  }
 }

 if(!c) 
 {
  int nx_c = 4;
  int ny_c = ceil( NCH/4 );
  c = new TCanvas("sphenix","sphenix display", 300*nx_c, 200*ny_c );
  c->Divide(nx_c,ny_c);
  //int ncd[]={13,9,5,1,14,10,6,2,15,11,7,3,16,12,8,4};
  for(int ich=0; ich<NCH; ich++)
  {
   //c->cd(ncd[ich]); //ich+1);
   c->cd(ich+1);
   gpulse[ich]->SetMaximum( plot_max ); //PEDESTAL+50 );
   gpulse[ich]->SetMinimum( plot_min ); //PEDESTAL-2000 );
   gpulse[ich]->Draw("ACP");
  }

  //cout << "Double click on the canvas to go to next event." << endl;
  cout << "Kill the process or 'killall root.exe' to stop the canvas" << endl;
 }

 for(int ich=0; ich<NCH; ich++) c->cd(ich+1)->Modified();
 //c->WaitPrimitive();
 c->Modified();
 c->Update();
 //c->Print("print.gif+");
}

//_____________________________________
double hcal::SignalShape(double *x, double *par)
{
 double pedestal = par[4] + x[0]*par[5];
 if( x[0]<par[1]) return pedestal;
 //double  signal = (-1)*par[0]*pow((x[0]-par[1]),par[2])*exp(-(x[0]-par[1])*par[3]);
 double  signal = par[0]*pow((x[0]-par[1]),par[2])*exp(-(x[0]-par[1])*par[3]);
 return pedestal + signal  ;
}

//______________________________________
double hcal::SignalShapeNegative(double *x, double *par)
{
 double pedestal = par[4] + x[0]*par[5];
 if( x[0]<par[1]) return pedestal;
 double  signal = par[0]*pow((x[0]-par[1]),par[2])*exp(-(x[0]-par[1])*par[3]);
 return pedestal-signal  ;
}


//________________________________________
void hcal::Save(char *filename)
{
  fout = TFile::Open( filename, "RECREATE" );
  for(int ich=0; ich<NCH; ich++)
  {
    h_PulsePeaks[ich]->Write();
    h_tres[ich]->Write();
    h_ped_res[ich]->Write();
    h2_fit_shape[ich]->Write();
    //h_hilo[ich/2]->Write();
    //if(ich%2) h_gain[(int)ich/2]->Write();
    h_PulseInt[ich]->Write();
    h2_Pulse_H_Int[ich]->Write();
  }
  h_saturation_rates->Write();
  rate->Write();
  h_info->Write();
  fout->Close();
}

