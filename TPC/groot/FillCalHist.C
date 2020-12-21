////////////////////////////////////
// Fillcalhist.C                    //
////////////////////////////////////

//  This makes a histogram that can show the raw hits.

#include "FillCalHist.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TF1.h"
#include "AZigzag.h"

#include "groot.h"

#include <iostream>
#include <cmath>

TH2D* CalHist=0;
TH1D* CalDist[Nsrs];

TH1D* CalHistQ=0;
TH1D* CalHistT=0;
TH1D* CalHistX=0;
TH1D* CalHistY=0;

TH1D* CalibratedMeans=0;
TH1D* CalibratedSigmas=0;

using namespace std;

void FillCalHist()
{

  int nTime = AZigzag::Raw[0].size();
  double left = -0.5;
  double right = (double)nTime - 0.5;

  if (!CalHist)
    {
      CalHist = new TH2D("CalHist", "CalHist", Nsrs, -0.5, (float)Nsrs-0.5, nTime, left, right);

      CalHistQ = new TH1D("CalHistQ", "CalHistQ", Nsrs, -0.5, (float)Nsrs-0.5);
      CalHistT = new TH1D("CalHistT", "CalHistT", Nsrs, -0.5, (float)Nsrs-0.5);

      CalHistX = new TH1D("CalHistX", "CalHistX", Nsrs, -0.5, (float)Nsrs-0.5);
      CalHistY = new TH1D("CalHistY", "CalHistY", Nsrs, -0.5, (float)Nsrs-0.5);

      char name[500];
      for (int i=0; i<Nsrs; i++)
	{
	  sprintf(name,"CalDist%04d",i);
	  CalDist[i] = new TH1D(name, name, 101, -50.5, 50.5);

	}

    }

  CalHist->Reset();
  CalHistQ->Reset();
  CalHistT->Reset();
  CalHistX->Reset();
  CalHistY->Reset();

  groot* Tree=groot::instance();
  for (int i=0; i<Nsrs; i++)
    {
      for (int j=0; j<AZigzag::Cal[i].size(); j++)
	{
	  CalHist->SetBinContent(i+1,j+1,AZigzag::Cal[i][j]);
	  CalDist[i]->Fill(AZigzag::Cal[i][j]);
	}
      if (Tree->theZigzags[i])
	{
	  int ID = Tree->theZigzags[i]->MyID();
	  CalHistQ->SetBinContent(i+1,Tree->theZigzags[i]->Q());
	  CalHistT->SetBinContent(i+1,Tree->theZigzags[i]->T());
	  //CalHistX->SetBinContent(i+1,Tree->theZigzags[i]->XCenter());
	  //CalHistY->SetBinContent(i+1,Tree->theZigzags[i]->YCenter());
	}
    }
}


void FitCalHist()
{
  if (!CalibratedMeans)
    {
      CalibratedMeans = new TH1D("CalibratedMeans", "CalibratedMeans", Nsrs, -0.5, Nsrs-0.5);
      CalibratedSigmas = new TH1D("CalibratedSigmas", "CalibratedSigmas", Nsrs, -0.5, Nsrs-0.5);
    }
  
  CalibratedMeans->Reset();
  CalibratedSigmas->Reset();
  
  for (int i=0; i<Nsrs; i++)
    {
      CalDist[i]->Fit("gaus","Q");
      CalibratedMeans->SetBinContent(i, CalDist[i]->GetFunction("gaus")->GetParameter(1));
      CalibratedSigmas->SetBinContent(i, CalDist[i]->GetFunction("gaus")->GetParameter(2));
    }
  
}
