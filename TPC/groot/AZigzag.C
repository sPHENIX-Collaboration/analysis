#include<iostream>

#include "AZigzag.h"

#include "TObject.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TBox.h"
#include "TMath.h"
#include "TSpectrum.h"
#include "TPolyLine.h"
#include "Quiver.h"
#include "groot.h"

#include <cmath>   //don't put this in class header.
#include <math.h>
#include <algorithm>
#include <fstream>
#include <string>
#include "TPolyLine.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TF1.h"
#include <algorithm>
using namespace std;

int AZigzag::nextID=0;

bool AZigzag::FastQ = false; // FastQ is bad....
bool AZigzag::UseSigma = true;
std::vector<int> AZigzag::Raw[Nsrs];
std::vector<double> AZigzag::Cal[Nsrs];

double AZigzag::Pedestals[Nsrs] = {0};
double AZigzag::Sigmas[Nsrs] = {0};
double AZigzag::Gains[Nsrs] = {0};

TRandom AZigzag::Randy;

std::vector<double> AZigzag::CommonMode[Nhybrid];

TH1D* AZigzag::Pulse=0; /* @TODO Do we need this? */
TF1*  AZigzag::blue=0; /* @TODO Do we need this? */
TH1D* AZigzag::commy[Nhybrid] = {0};

double AZigzag::SigmaCut=4;
double AZigzag::PulseCut=100;
string AZigzag::CommonModeMethod="median";

AZigzag::AZigzag( CheveronPad_t paddef )
{
  fPads = paddef;

  double minR = *min_element(fPads.r.begin(),fPads.r.end());
  double maxR = *max_element(fPads.r.begin(),fPads.r.end());
  double minPhi = *min_element(fPads.phi.begin(),fPads.phi.end());
  double maxPhi = *max_element(fPads.phi.begin(),fPads.phi.end());
  
  myR = (minR+maxR)/2.0;
  myPhi = (minPhi+maxPhi)/2.0;

  // Set the complex mappig pointers as null at creation.
  PreLogical  = 0;
  PostLogical = 0;
  PreWaveform = 0;
  PostWaveform= 0;

  myID = nextID++;

  Randy.SetSeed();
}


void AZigzag::Draw(double MAX) 
{
  int N = fPads.fX.size();
  TPolyLine* Ziggy = new TPolyLine(N, &(fPads.fX[0]), &(fPads.fY[0]));
  
  /*cout << " N" << N;
  cout << " x" << fPads.fX[0];
  cout << " y" << fPads.fY[0];
  cout << endl;
  */
  Ziggy->SetFillStyle(1001);
  Ziggy->SetFillColor(18);
  

  //  51-100 looks like a ROYGBIV scale in root.
  if ( IsHit() ) 
    {
      double f = q/MAX;
      if (f>1) f=1;
      int CLR = 51 + f*49;
      Ziggy->SetFillColor(CLR);
    }

  Ziggy->SetLineStyle(1);
  Ziggy->SetLineWidth(1);
  Ziggy->SetLineColor(18);
  
  Ziggy->Draw("f");
}

int AZigzag::color(int) {}


void AZigzag::WriteCalibration()
{
  string dir( getenv("MYCALIB") );
  string file("ZIGZAG_CALIBRATIONS.TXT");
  string result = dir + file;
  cout << "Looking to write Zigzag Calibrations in " << result << endl;
  ofstream fin(result.c_str());

  if (!fin.is_open())
    {
      cout <<  "Groot could not open the file ZIGZAG_CALIBRATIONS.TXT." << endl;
      return;
    }

  
  for (int i=0; i<Nsrs; i++)
    {
      fin << Pedestals[i] << " " << Sigmas[i] << " " << Gains[i] << endl;
      cout << Pedestals[i] << " " << Sigmas[i] << " " << Gains[i] << endl;
    }
  fin.close();
}

void AZigzag::ReadCalibration()
{
  cout << "Reading ZIGZAG calibrations..." << endl;

  string dir( getenv("MYCALIB") );
  string file("ZIGZAG_CALIBRATIONS.TXT");
  string result = dir + file;
  cout << "Looking to read Zigzag Calibrations in " << result << endl;
  ifstream fin(result.c_str());
  
  if (!fin.is_open())
    {
      cout <<  "Groot could not open the file ZIGZAG_CALIBRATIONS.TXT." << endl;
      cout <<  "Please initialize the variable $MYCALIB in your .login." << endl;
      cout <<  "Then move the ZIGZAG_CALIBRATIONS.TXT file to there." << endl;
      cout <<  "Here we will make fake calibrations..." << endl;
      for ( int i=0; i < Nsrs; i++ )
	{
	  Pedestals[i]  = 0;
	  Sigmas[i]     = 0;
	  Gains[i]      = 1;
	}
      cout << "FAKED!" << endl;
      return;
    }

  cout << "FOUND!  Reading Calibrations..." << endl;
  for ( int i=0; i < Nsrs; i++ )
    {
      fin >> Pedestals[i] >> Sigmas[i] >> Gains[i];
      //cout << Pedestals[i] << " " << Sigmas[i] << " " << Gains[i] << endl;
    }
  fin.close();

  cout << "DONE." << endl;
}

void AZigzag::DetermineCommonMode()
{
  groot* Tree = groot::instance();

  string method = CommonModeMethod;

  for (int i=0; i<Nhybrid; i++)
    {
      CommonMode[i].clear();
    }

  if (method == "off")
    {
      for (int i=0; i<Nhybrid; i++)
	{
	  for (int j=0; j<Raw[0].size(); j++)
	    {
	      CommonMode[i].push_back(0);
	    }
	}
    }
  else if (method == "median")
    {
      vector<double> pedheights[Nhybrid]; // one vector of doubles for the non-fired chans from every hybrid.      

      //  Loop over the time slices...
      for (int i=0; i<Raw[0].size(); i++)
	{

	  //  The pedheights[hybrid] is reused in every timeslice i, gotta clear it...
	  for (int q=0; q<Nhybrid; q++)
	    {
	      pedheights[q].clear();
	    }
	  
	  // loop over all zigzags for this timeslice add value to pedheight vector.
	  for (int j=0; j< Tree->theZigzags.size(); j++)
	    {
	      int which = (Tree->theZigzags[j])->MyHybrid;
	      double hit = Pedestals[(Tree->theZigzags[j])->MyID()] - Raw[(Tree->theZigzags[j])->MyID()][i];

	      pedheights[which].push_back(hit);
 	    }

	  // Now all zigzags have made entries to pedheights vectors.
	  // Determine the medians of each such pedheights distribution.
	  // Save these as the CommonMode Value...
	  for (int q=0; q<Nhybrid; q++)
	    {
	      if (pedheights[q].size()>10)
		{
		  sort(pedheights[q].begin(),pedheights[q].end());
		  int index = pedheights[q].size()/2;
		  CommonMode[q].push_back(pedheights[q][index]);
		}
	      else
		{
		  CommonMode[q].push_back(0);
		}
      	    }
	}
    }

  else if (method == "gauss" || method == "spectrum")
    {
      for (int i=0; i<Nhybrid; i++)
	{
	  char name[500];
	  sprintf(name,"commy%02d",i);
	  commy[i] = new TH1D(name,name,40,-330,70);
	}
      
      TF1* gss = new TF1("gss", "[0]*exp(-((x-[1])*(x-[1])/(2*[2]*[2])))", -100, 100);
      gss->SetParLimits(0,0,128);
      gss->SetParLimits(1,-330,70);
      gss->SetParLimits(2,7,40);
      
      for (int i=0; i<Raw[0].size(); i++)
	{
	  
	  /* loop over all zigzag types */
	  for (int j=0; j< Tree->theZigzags.size(); j++)
	    {
	      int which = (Tree->theZigzags[j])->MyHybrid;
	      commy[which]->Fill(Pedestals[(Tree->theZigzags[j])->MyID()] - Raw[(Tree->theZigzags[j])->MyID()][i]);
	    }
	  
	  for (int k=0; k<Nhybrid; k++)
	    {
	      if (commy[k]->Integral(1,commy[k]->GetNbinsX())>64)
		{
		  if ( method == "gauss" )
		    {
		      int bin = commy[k]->GetMaximumBin();
		      
		      gss->SetParameter(0,commy[k]->GetBinContent(bin));
		      gss->SetParameter(1,commy[k]->GetBinCenter(bin));
		      gss->SetParameter(2,17);
		      commy[k]->Fit(gss,"WWQM0");

		      //CommonCompare->Fill( CommonMode[k][i],  commy[k]->GetFunction("gss")->GetParameter(1));
		      //cout << "timeslice: " << k;
		      //cout << " vector method = " << CommonMode[k][i];
		      //cout << " gauss method = " << commy[k]->GetFunction("gss")->GetParameter(1);
		      //cout << endl;
		      CommonMode[k].push_back(commy[k]->GetFunction("gss")->GetParameter(1));
		      
		      if (i==7)
			{
			  char name[50];
			  sprintf(name,"cmm%d",k);
			  commy[k]->Clone(name);
			}
		      
		    }
		  else // method == "spectrum"
		    {
		      TSpectrum sspec(10);
		      int nPeaks = sspec.Search( commy[k] , 2 , "goff" );
		      
		      float peak_max_x = 0;
		      float peak_max_y = 0;
		      
		      float *xpeaks = sspec.GetPositionX();
		      float *ypeaks = sspec.GetPositionY();
		      
		      for ( int peak_j = 0; peak_j < nPeaks; peak_j++ )
			{
			  //cout << "Found peak at x = " << xpeaks[peak_j] << " , y = " << ypeaks[peak_j] << endl;
			  if (  ypeaks[peak_j] > peak_max_y )
			    {
			      peak_max_x = xpeaks[peak_j];
			      peak_max_y = ypeaks[peak_j];
			    }
			}
		      
		      CommonMode[k].push_back( peak_max_x );
		      //cout << "**using peak at x = " << peak_max_x << " , y = " << peak_max_y << endl;
		    }
		  commy[k]->Reset();
		}
	      else
		{
		  CommonMode[k].push_back(0);
		}
	    }
	}
      for (int m=0; m<Nhybrid; m++)
	{
	  commy[m]->Delete();
	}
      gss->Delete();
    }
  
  else
    {
      cerr << "ERROR:  AZigzag::DetermineCommonMode -> Unknown method " << method << endl; 
    }
  
}

void AZigzag::ApplyCalibration()
{
  groot* Tree = groot::instance();

  for (int i=0; i<Nsrs; i++) //loop over all zigzag types
    {
      Cal[i].clear();
      if (Tree->ZigzagMap[i])
	{
	  for(int j=0; j<Raw[i].size(); j++)
	    {
	      int which = Tree->ZigzagMap[i]->MyHybrid;
	      Cal[i].push_back(Gains[i]*(Pedestals[i]-Raw[i][j]-CommonMode[which][j]));
	    }
	}
    }  
}

void AZigzag::DetermineQ(double Mintime, double Maxtime)
{
  if (FastQ)
    {
      maxq = -100;
      maxt = -100;
      for (int i=0; i<Cal[myID].size(); i++)
     	{
	  if (Cal[myID][i] > maxq)
	    {
	      maxq = Cal[myID][i];
	      maxt = i;
	    }
      	}
      q = maxq;
      t = maxt;

      return;
    }
    

  //  After some analysis of pulser data, we have 
  //  determined a fit function that does a good job at extracting the charge.  
  //
  //  The function in question is a fermi function multipled by a gaussian.
  //  The initial trial was a fermi function multiplied by an exponential,
  //  but that choice poorly described the tail.
  //
  //                                                   MB & TKH 3-8-2013
  if (!blue)
    {
      Pulse = new TH1D("Pulse", "Pulse", Cal[myID].size(), -0.5, (double)Cal[myID].size()-0.5);
      blue  = new TF1("blue", "[0]/(exp(-(x-[1])/[2])+1)*exp(-((x-[1])^2/[3]))", 0, 30);
      
      //blue->FixParameter(1,7);  // Expt on 3-8-2013
      blue->FixParameter(2,0.42);  // Expt on 3-8-2013
      blue->FixParameter(3,53.7);  // Expt on 3-8-2013
    }


  //Here we are just averaging over time slices as a first approximation...
  //NOTE:  root will treat errors on each bin as sqrt(content). This is 
  //not true nor appropriate for an ADC. Instead we should treat them as constant errors.
  //I will choose the sigma of the channel in question as the error!
  for (int i=0; i<Cal[myID].size(); i++)
    {
      Pulse->SetBinContent(i+1,Cal[myID][i]);
      Pulse->SetBinError(i+1,Sigmas[myID]);
    }
  double max = Pulse->GetBinContent(Pulse->GetMaximumBin());
  double min = Pulse->GetBinContent(Pulse->GetMinimumBin());
  double t0  = Pulse->GetBinCenter(Pulse->GetMaximumBin())-2.0;

  blue->SetParameter(0,max);
  blue->SetParameter(1,(Mintime+Maxtime)/2.0);

  //  Note:  The above-parameterized curve does NOT peak at a.
  //  It peaks at a*AFACTOR !!!
  double AFACTOR = 0.933;
  blue->SetParLimits(0, min/AFACTOR-2.0*Sigmas[myID], max/AFACTOR+2.0*Sigmas[myID]);
  blue->SetParLimits(1, Mintime, Maxtime);

  Pulse->Fit(blue,"Q0");

  q = blue->GetParameter(0)*AFACTOR;
  t = blue->GetParameter(1);
}


void AZigzag::Report()
{
  //cout << "Q: " << q;
  //cout << "\tT: " << t;
  //cout << "\tID: " << myID;
  //cout << "\tHBD: " << MyHybrid;
  //cout << "\tiR: " << iR+1;
  //cout << "\tiPhi: " << iPhi;
  //if (IsHit()) cout << " **HIT** " << endl;
}


double AZigzag::XCenter() { return myR*cos(myPhi); }

double AZigzag::YCenter() { return myR*sin(myPhi); }

double AZigzag::ZCenter() { return 0; }

double AZigzag::RCenter() { return myR; }

double AZigzag::PCenter() { return myPhi; }
