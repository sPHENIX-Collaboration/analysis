#include <iomanip>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <ctime>

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <stdint.h>

#include <fileEventiterator.h>
#include <Event.h>
#include <packet.h>
#include <packet_hbd_fpgashort.h>
#include <TROOT.h>
#include <TF1.h>
#include <TH1.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TString.h>
#include <TApplication.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TGClient.h>
#include <TGWindow.h>

using namespace std;

//TROOT root("xxx","if you want to make a root app you can");

//  -----------------------------------------------------------------------------------------------

//const    int NCH      = 2*8;
const    int NCH      = 8;        //  HG channels only
//const    int NCH       = 12;
const    int NSAMPLES = 24;
const    int RISETIME    = 4;
const    int FALLTIME    = 5;
//const    int feech[]  = { 99, 98, 97, 96, 106, 107, 108, 109, 112, 113, 122, 123, 124, 125 };
//const    int feech[]  = { 115, 114, 113, 112, 119, 118, 117, 116, 123, 122, 121, 120, 127, 126, 125, 124 };
//const    int feech[]  = { 115, 113, 119, 117, 123, 121, 127, 125 };  //  HG channels only//  ORDERING : RIGHT/LEFT ARE TWO ENDS OF THE SAME FIBER
const    int feech[]  = { 115, 119, 123, 127, 113, 117, 121, 125 };  //  HG channels only

TGraph  * gpulse[NCH];
Double_t  shapeD[NCH][NSAMPLES];
TF1     * shapes[NCH];
TCanvas * ev_display, *fitFunc;
TH1     * fittime, * fitmax, * fitwidth, * fitintegral, * pedval, * pedslope;
TH1    ** fitPar[6];
int      nevents(0);         // event counter
int      channel(0);


//  -----------------------------------------------------------------------------------------------

void usage()
{
  cout << "evDisplay <prdf>" << endl;
}
//  -----------------------------------------------------------------------------------------------
//  Unipolar pulse (0.3164*pow(x,4)*exp(-x*1.5) - Unity integral, peak ~1/3 of integral)
Double_t PEDESTAL     = 2048;
Double_t par0[]       = {    0., 0.,                    4., 0.,   0.,   0.};
Double_t par0Max[]    = { 1000., NSAMPLES-FALLTIME+1.,  5., 1.,   2150., 0.2};
Double_t par0Min[]    = {    1., RISETIME-1,            3., 2.,   1950.,-0.2};
Double_t signal(0.), pedestal(0.);
//  -----------------------------------------------------------------------------------------------
Double_t signalShape(Double_t *x, Double_t * par){
  //  if(x[0]<par[1])  return PEDESTAL;
  pedestal = par[4]+x[0]*par[5];
  if(x[0]<par[1])   return pedestal;
  //  signal contribution
  signal   = par[0]*pow((x[0]-par[1]),par[2])*exp(-(x[0]-par[1])*par[3]);
  //  cout<<"EVENT  "<<nevents<<"  CHANNEL  "<<channel<<"  X  "<<x[0]<<"  signal "<<signal<<"  pedestal  "<<pedestal<<endl;
  return   signal+pedestal;
}
//  -----------------------------------------------------------------------------------------------

void init(){
  fittime     = new TH1F("fittime",    "Fitted pulse peak time",   100, 0.,   24.);
  fitmax      = new TH1F("fitmax",     "Fitted pulse amplitude",   500, 0.,   5000.);
  fitwidth    = new TH1F("fitwidth",   "Fitted pulse width",       100, 0.,   5.);
  pedval      = new TH1F("pedval",     "Pedestal at t==0",         100, 2000.,2100.);
  pedslope    = new TH1F("pedslope",   "Pedestal slope",           100, -1.,  1.);
  fitintegral = new TH1F("fitintegral","Integral of fitted curve (pedestal suppressed)", 100, 0., 10000.);
  //  fit parameters (for templating)
  
  
}
//  -----------------------------------------------------------------------------------------------

void fitShape(int chan){
  channel  = chan;
  TString fitName = feech[chan]%2? "HG_" : "LG_";
  fitName += chan;
  fitName += "(";
  fitName += feech[chan];
  fitName += ")";
  shapes[chan]= (TF1*) (gROOT->FindObject("fitName"));
  //  zero approximation to fit parameters
  //  use pulse data to find location of maximum
  if(shapes[chan]) delete shapes[chan];
  shapes[chan] = new TF1(fitName, &signalShape, 0., 24., 6);
  shapes[chan]->SetLineColor(2);
  int      peakPos = 0;
  Double_t peakVal = 0.;
  for (int iSample=0; iSample<NSAMPLES; iSample++) {
    if(shapeD[chan][iSample]>peakVal) {
      peakVal = shapeD[chan][iSample];
      peakPos = iSample;
    }
  }
  peakVal -= PEDESTAL;
  if(peakVal<0.) peakVal = 0.;
  par0[0] = peakVal/3.;
  par0[1] = peakPos-RISETIME;
  if(par0[1]<0.) par0[1] = 0.;
  par0[2] = 4.;
  par0[3] = 1.5;
  par0[4] = PEDESTAL;  //   we do not do pedestal subtrastion at this time
  par0[5] = 0;      //   slope of the pedestals is initially set to "0"
  shapes[chan]->SetParameters(par0);
  for(int parn=0; parn<6; parn++) shapes[chan]->SetParLimits(parn, par0Min[parn], par0Max[parn]);
  //  fitFunc->cd(chan+1);
  //  shapes[chan]->Draw();
  //  gPad->Modified();
  gpulse[chan]->Fit(shapes[chan], "", "RQ", 0., (Double_t)NSAMPLES);
  Double_t fMax = shapes[chan]->GetMaximum(par0Min[1], par0Max[1]);
  Double_t fPos = shapes[chan]->GetMaximumX(par0Min[1], par0Max[1]);


  cout<<"EVENT  "<<nevents<<" SHAPE  "<<(char*)(shapes[chan]->GetName())<<"  peakVal  "<<peakVal<<"  peakPos  "<<peakPos<<"  fitMax  "<<fMax<<"  fitPos  "<<fPos<<endl;
  ev_display->cd(chan+1);
  shapes[channel]->Draw("same");
  //  gPad->Modified();


}

//  -----------------------------------------------------------------------------------------------
		       
int  main(int argc, char **argv)
{
  if ( argc!=2 )
    {
      usage();
      exit(-1);
    }

  TString prdfname = argv[1];   // get prdf file name
  TString basename = prdfname;
  basename.ReplaceAll(".prdf","");
  Ssiz_t lastslash = basename.Last('/');
  basename.Remove(0,lastslash+1);
  //cout << basename << endl;
  basename.ReplaceAll("rc-","");
  //cout << basename << endl;

  char *file = argv[1];	// get prdf file name
  TApplication theApp("theApp",&argc,argv);

  //  gROOT->SetStyle("Plain");
/*
  gStyle->SetLabelSize(0.1,"xyz");
  gStyle->SetTitleSize(0.08,"xyz");
  gStyle->SetStatH(0.4);
*/

  //TCanvas *ev_display = new TCanvas("sphenix","sphenix display",800,800);
  //  This version of plotting is for High/Low gain preamplifier sending its outputs into two sequential readout channels
  int nx_c = 2;             
  int ny_c = NCH/2+NCH%2;
  ev_display = new TCanvas("sphenix","sphenix display",400*nx_c,200*ny_c);

  //  fitFunc   = new TCanvas("fitFunc","Fit Functions",  400*nx_c,200*ny_c);
  ev_display->SetEditable(kTRUE);
  //  ev_display->Divide(nx_c,ny_c);
  //  ev_display->SetBit(TPad::kClearAfterCR);
  //  fitFunc->SetEditable(kTRUE);
  //  fitFunc->Divide(nx_c,ny_c);

  TString name, title;
  for (int ich=0; ich<NCH; ich++)
  {
    gpulse[ich] = new TGraph(NSAMPLES);
    name = "gch"; name += ich;
    gpulse[ich]->SetName(name);
    gpulse[ich]->SetMarkerStyle(20);
    gpulse[ich]->SetMarkerSize(0.4);
    //    gpulse[ich]->SetMarkerColor(ich+1);
    //    gpulse[ich]->SetLineColor(ich+1);
  }

  // Open up PRDFF
  int status;

  Eventiterator *it =  new fileEventiterator(file, status);
  if (status)
  {
    cout << "Couldn't open input file " << file << endl;
    delete(it);
    exit(1);
  }

  Event *evt;

  // Loop over events
  while ( (evt = it->getNextEvent()) != 0 )
    {
      int eventseq = evt->getEvtSequence();

      if ( evt->getEvtType() != 1 ) {
	cout << "eventseq " << eventseq << " event type = " << evt->getEvtType() << endl;
	continue;
      }
      ev_display->Divide(nx_c,ny_c);
      ev_display->SetBit(TPad::kClearAfterCR);

      // Get sPHENIX Packet
      Packet_hbd_fpgashort *spacket = dynamic_cast<Packet_hbd_fpgashort*>( evt->getPacket(21101) );
      if ( spacket )
	{
	  spacket->setNumSamples( NSAMPLES );
	  int nmod_per_fem = spacket->iValue(0,"NRMODULES");
	  //cout << "nmod_per_fem " << nmod_per_fem << endl;
	  cout <<"EVENT "<<eventseq << "    modules per FEM "<<nmod_per_fem<<endl;
	  nevents++;
	  for (int ich=0; ich<NCH; ich++)
	    {
	      channel     = ich;
	      for (int isamp=0; isamp<NSAMPLES; isamp++)
		{
		  Short_t val = spacket->iValue(feech[ich],isamp);
		  gpulse[ich]->SetPoint(isamp,(Double_t)isamp,(Double_t)val);
		  shapeD[ich][isamp] = (Double_t)val;
		}
	      ev_display->cd(ich+1);
	      //gpulse[ich]->SetMaximum(4095);
	      //gpulse[ich]->SetMinimum(0);
	      //	      if(nevents==1) gpulse[ich]->Draw("acp");
	      gpulse[ich]->Draw("acp");
	      //	      gPad       ->Modified();
	      fitShape(ich);
	      //	      gPad       ->Modified();
	      //	      shapes[ich]->Draw("same");
	      //	      gPad       ->Update();
	    }
// 	  for (int ich=0; ich<NCH; ich++)
// 	    {
// 	      channel    = ich;
// 	      fitFunc    ->cd(ich+1);
// 	      shapes[ich]->Draw();
// 	    }
// 	  gPad       ->Modified();


	  cout<<"Updating canvas  "<<endl;
	  
	  ev_display->Update();
	  //	  fitFunc  ->Update();
	  //	  name = basename; name += "_"; name += eventseq; name += ".png";
	  //      ev_display->SaveAs(name);	  for(int loop=0; loop<1e+6; loop++) continue;
	  //	  TGWindow * mw = (TGWindow *)(gClient->GetRoot());
	  //	  gClient->WaitFor(mw);
// 	  if ( eventseq > 10 )
// 	    {
 	      // string junk;
	      // cout << "? ";
 	      // cin >> junk;
	      // if(junk=="q") goto ALLDONE;
// 	    }

	  delete spacket;
	  ev_display->Clear();
	}

      delete evt;
      //if (nevents++==10) break;
    }


 ALLDONE:
  cout<<"ALLDONE"<<endl;
  delete it;

  //  theApp.Run();
  exit(0);

}

