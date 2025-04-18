#include "FillHoughHist.h"
#include "TH1D.h"
#include "TH2D.h"
#include "ABlob.h"
#include "ATrack.h"
#include "groot.h"

#include "AZigzag.h"

#include <iostream>
#include <cmath>

TH2D* HoughHistMC  = 0; //  This is in relative coordinates to "tune" the Hough Space.
TH2D* HoughHistABS = 0; //  This is in absolute coordinates to "solve" the Hough Space.

using namespace std;

void FillHoughHist()
{
  groot* Tree=groot::instance();

  if (!HoughHistMC)
    {
      HoughHistMC = new TH2D("HoughHistMC", "HoughHistMC" , 10, (Med_inverseSlope - 10*MAD_inverseSlope),(Med_inverseSlope + 10*MAD_inverseSlope), 10 , (Med_Offset - 10*MAD_Offset), (Med_Offset + 10*MAD_Offset));


      double x00 = Tree->ZigzagMap2[0][0]->XCenter();
      double y00 = Tree->ZigzagMap2[0][0]->YCenter();
      double x11 = Tree->ZigzagMap2[Nr][Nphi]->XCenter();
      double y11 = Tree->ZigzagMap2[Nr][Nphi]->YCenter();

      double mi = abs(inverseSlope(x00, y00, x11, y11));
      double c  = abs(intercept(x00, y00, x11, y11));

      int NHOUGHX = 2*mi/(HFACTOR*MAD_inverseSlope);
      int NHOUGHY = 2*c/(HFACTOR*MAD_Offset);

      HoughHistABS = new TH2D("HoughHistABS", "HoughHistABS" , NHOUGHX, -mi, mi, NHOUGHY, -c, c);
    }
  HoughHistMC->Reset();
  HoughHistABS->Reset();
  int Npoints=0;
  for (int i=0; i<Nr;i++)
    {
      Npoints+=Tree->theBlobs[i].size();
    }
  
  //cout<<"checking............";
  double points[2][Npoints];
  for (int i=0; i<Npoints; i++)
    {
      points[0][i]=0;
      points[1][i]=0;
    }
  //cout<<"\n\n\n\t\t\t"<<Npoints<<"\n\n";
  int k=0;
  for (int i=0; i<Nr; i++)
    {
      for (int j=0; j< Tree->theBlobs[i].size(); j++)
	{
	  points[0][k] = (Tree->theBlobs[i])[j]->CentroidX(); //X value
	  points[1][k]= (Tree->theBlobs[i])[j]->CentroidY(); // Y value
	  //cout<<"\n"<<points[0][k]<<"\t\t"<<points[1][k];
	  k++;
	}
      
    }
    

  vector<double> MI;
  vector<double> C;
  double y1,y2,x1,x2;
  for (int i=0; i<k; i++)
    {
      for (int j=i+1; j<k; j++)
	{
	  x1=points[0][i];
	  x2=points[0][j];
	  y1=points[1][i];
	  y2=points[1][j];
	  double mi = inverseSlope(x1, y1, x2, y2);
	  double c  = intercept   (x1, y1, x2, y2);
	  HoughHistABS->Fill(mi, c);
	  MI.push_back(mi);
	  C.push_back(c);
	}
    }
  
  
  for (int i=0; i<k; i++)
    {
      for (int j=i+1; j<k; j++)
	{
	  x1=points[0][i];
	  x2=points[0][j];
	  y1=points[1][i];
	  y2=points[1][j];
	  double mi = inverseSlope(x1, y1, x2, y2);
	  double c  = intercept   (x1, y1, x2, y2);
	  HoughHistMC->Fill(mi, c);
	}
    }
  
  //ATrack *theTracks;
  //int binmax = HoughHistMC->GetMaximumBin();
  //double Var[2];
  //Var[0] = 1/(HoughHistMC->GetXaxis()->GetBinCenter(binmax));
  //Var[1] = HoughHistMC->GetYaxis()->GetBinCenter(binmax);
  //Tree->theTracks->SetSlope(Var[0]);
  //Tree->theTracks->SetOffset(Var[1]);
  // cout<<"\nFinal\t"<<Tree->theTracks.Slope<<"\t"<<Tree->theTracks.Offset;
  //cout <<"\ncheck........"<<(Tree->theTracks)->check();


}

double inverseSlope(double x1, double y1, double x2, double y2)
{
  double mi = (x2-x1)/(y2-y1);
  return mi;
}

double intercept   (double x1, double y1, double x2, double y2)
{
  double c= (y2*x1 - y1*x2)/(x1-x2);
  return c;
}
