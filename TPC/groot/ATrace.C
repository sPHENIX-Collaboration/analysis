#include "ATrace.h"  //always include class header
#include "TF1.h"
#include "TH1D.h"
#include "TH2D.h"
#include <iostream>

using namespace std;

int ATrace::numtraces=0;

bool ATrace::FastFits = true;

double ATrace::SingleEmean = 0;
double ATrace::SingleEsigma = 0;
double ATrace::DoubleEmean = 0;
double ATrace::DoubleEsigma = 0;

TH2D* heightCompare=0;
TH2D* timeCompare=0;

TH1D* heightDiff=0;
TH1D* timeDiff=0;


ATrace::ATrace()
{
  voltage.clear();
  height=0;
  time=0;
  W=0;
  dW=0;

  sprintf (fcn_name, "fermi%d", numtraces);
  fermi = new TF1(fcn_name, "[0]/(exp(-(x-[1])/[2])+1)+[3]", 0, 1024);

  sprintf (hst_name, "trace%d", numtraces);
  trace = new TH1D(hst_name, hst_name, 1024, -0.5, 1023.5);

  numtraces++;		   
}

ATrace::~ATrace()
{
  fermi->Delete();
  trace->Delete();
}

double ATrace::FindMaximum(int n)
{

  double MaxSum = 9999;
  int MaxMid = -1;
  for (int mid=1+n; mid<voltage.size()-n; mid++)
    {
      // We know roughly where true electron pulses should be in time - only look in that range
      if( mid < 420 || mid > 460 ) continue;
      double Sum=0;
      for (int bin=mid-n; bin<mid+n; bin++)
	{
	  Sum += voltage[bin];
	}
      if (Sum < MaxSum)
	{
	  MaxSum = Sum;
	  MaxMid = mid;
	}
    }

  return MaxSum/(2.0*n+1.0);
}

int ATrace::FindMaximumMiddle(int n)
{

  double MaxSum = 9999;
  int MaxMid = -1;
  for (int mid=1+n; mid<voltage.size()-n; mid++)
    {
      // We know roughly where true electron pulses should be in time -- only look in that range
      if( mid < 420 || mid > 460 ) continue;
      double Sum=0;
      for (int bin=mid-n; bin<mid+n; bin++)
	{
	  Sum += voltage[bin];
	}
      if (Sum < MaxSum)
	{
	  MaxSum = Sum;
	  MaxMid = mid;
	}
    }

  return MaxMid;
}


void ATrace::FitLeftEdge()
{
  MakeTrace(+1);
  int bin = trace->GetMaximumBin();
  height  = trace->GetBinContent(bin);
  time    = 0;

  // 1) get time and height from histogram
  if (FastFits) 
    {
      bool found = false;
      for(int i=bin;i>0;--i) 
	{
	  if(found) break;
	  if(trace->GetBinContent(i) < height/2.0) 
	    {
	      found = true;
	      time = trace->GetBinCenter(i);
	    }
	}
    } 
  // 2) get time and height from fit
  else 
    {
      if (!heightCompare) 
	{
	  heightCompare = new TH2D("heightCompare", "heightCompare", 1000, -0.5, 999.5, 1000, -0.5, 999.5);
	  timeCompare = new TH2D("timeCompare", "timeCompare", 1000, -0.5, 999.5, 1000, -0.5, 999.5);
	  heightDiff = new TH1D("heightDiff", "heightDiff", 1000, -499.5, 500.5);
	  timeDiff = new TH1D("timeDiff", "timeDiff", 1000, -499.5, 500.5);
	}
      double X = trace->GetBinCenter(bin);
    
      fermi->SetParameter(0,height);
      fermi->SetParameter(1,X-2);
      fermi->SetParameter(2,5);
      fermi->SetParameter(3,2);
      fermi->SetRange(5,X+2);
      trace->Fit(fermi,"WWR","",5,X+2);
    
      double ht = trace->GetFunction(fcn_name)->GetParameter(0);
      double tm = trace->GetFunction(fcn_name)->GetParameter(1);
      
      heightCompare->Fill(height, ht);
      timeCompare->Fill(time, tm);
      heightDiff->Fill(height-ht);
      timeDiff->Fill(time-tm);
      
      height = trace->GetFunction(fcn_name)->GetParameter(0);
      time   = trace->GetFunction(fcn_name)->GetParameter(1);
    }
}


int ATrace::NAboveThreshold( double thrup, double thrdown )
{

  int nAbove = 0;

  bool belowThreshold = true;

  for(int i=0; i<voltage.size(); i++)
    {
      if ( belowThreshold && -voltage[i] >= thrup )
	{
	  nAbove++;
	  belowThreshold = false;
	}

      else if ( !belowThreshold && -voltage[i] < thrdown )
	{
	  belowThreshold = true;
	}
    }

  return nAbove;

}

double ATrace::PulseWidth( double thrup, double thrdown )
{

  //  The results of this routine are ONLY valid 
  //  if NAbove is one.

  bool belowThreshold = true;

  int left = 0;
  int right = 0;

  for(int i=0; i<voltage.size(); i++)
    {
      if ( belowThreshold && voltage[i] >= thrup )
	{
	  left = i;
	  belowThreshold = false;
	}

      else if ( !belowThreshold && voltage[i] < thrdown )
	{
	  right = i;
	  belowThreshold = true;
	}
    }

  return right-left;

}

void ATrace::MakeTrace(int sign) 
{
  for(int i=0; i!=voltage.size(); ++i) 
    {
      double vol = sign*voltage[i];
      trace->SetBinContent(i+1,vol);
    }
}
