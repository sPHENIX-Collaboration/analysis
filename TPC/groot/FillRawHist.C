////////////////////////////////////
// FillRawHist.C                    //
////////////////////////////////////

//  This makes a histogram that can show the raw hits.

#include "FillRawHist.h"
#include "TF1.h"
#include "TH2D.h"
#include "AZigzag.h"

#include "ATrace.h"
#include "groot.h"

#include <iostream>

TH2D* RawHist=0;
TH1D* RawDist[Nsrs];

using namespace std;

void FillRawHist()
{

  int nTime = AZigzag::Raw[0].size();
  double left = -0.5;
  double right = (double)nTime - 0.5;


  //  NOTE:  Raw is the histogram into which we place the raw data.
  //  but AZigzag::Raw is the raw data itsChi.


  if (!RawHist)
    {
      RawHist = new TH2D("RawHist","RawHist",Nsrs,-0.5,((float)Nsrs-0.5),nTime,left,right);
      cout << "Making the Raw histogram" << endl;

      char name[500];
      for (int i=0; i<Nsrs; i++)
	{
	  sprintf(name,"RawDist%04d",i);
	  RawDist[i] = new TH1D(name,name,2001,1999.5,4000.5);
	}
    }

  RawHist->Reset();

  for (int i=0; i<Nsrs; i++)
    {
      for (int j=0; j<AZigzag::Raw[i].size(); j++)
	{
	  RawHist->SetBinContent(i+1,j+1,AZigzag::Raw[i][j]);
	  RawDist[i]->Fill(AZigzag::Raw[i][j]);
	}
    }

}

void FitPedestals()
{
  for (int i=0; i<Nsrs; i++)
    {
      RawDist[i]->Fit("gaus","Q");
      AZigzag::Pedestals[i] = RawDist[i]->GetFunction("gaus")->GetParameter(1);
      AZigzag::Sigmas[i] = RawDist[i]->GetFunction("gaus")->GetParameter(2);
      AZigzag::Gains[i] = 1.0;
    }
}
