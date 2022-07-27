#include "FillHoughHist.h"
#include "TH1D.h"
#include "TH2D.h"
#include "ABlob.h"
#include "ATrack.h"
#include "groot.h"

#include <iostream>
#include <cmath>

TH2D* HoughHistMC = 0;

using namespace std;

void FillHoughHist()
{
  if (!HoughHistMC)
    {
      HoughHistMC = new TH2D("HoughHistMC", "HoughHistMC" , 15 , -2.5 , 2.5, 12, 200, 2500);
    }
  //HoughHistMC->Reset();
  groot* Tree=groot::instance();
  int Npoints=0, k1=0; //number of points/blobs in an event. considering all tracks.
  for (int i=0; i<Nr;i++)
    {
      Npoints+=Tree->theBlobs[i].size();
      if (Tree->theBlobs[i].size()==1)
	k1++;
    }
  
  cout<<"checking............";
  double points[2][k1];
  for (int i=0; i<k1+1; i++)
    {
      points[0][i]=0;
      points[1][i]=0;
    }
  cout<<"\n\n\n\t\t\t"<<Npoints<<"\n\n";
  int k=0;
  for (int i=0; i<Nr; i++)
    {
      for (int j=0; j< Tree->theBlobs[i].size(); j++)
	{ if (Tree->theBlobs[i].size()==1)
	    {
	      points[0][k] = (Tree->theBlobs[i])[j]->CentroidX(); //X value
	      points[1][k]= (Tree->theBlobs[i])[j]->CentroidY(); // Y value
	      //cout<<"\n"<<points[0][k]<<"\t\t"<<points[1][k];
	      k++;
	    }
	  
	}
    }

 
  cout <<"\n N" << Npoints <<"\t k1"<<k1<<"\t K"<<k;
  double m, c;
  
  static std::vector <double> inverse_slope;
  static std::vector <double> offset;
  double y1,y2,x1,x2;
  for (int i=0; i<k; i++)
    {
      for (int j=i+1; j<k; j++)
	{
	  x1=points[0][i];
	  x2=points[0][j];
	  y1=points[1][i];
	  y2=points[1][j];
	  m= (y2-y1)/(x2-x1);
	  c= (y2*x1 - y1*x2)/(x1-x2);
	  inverse_slope.push_back(1/m);
	  offset.push_back(c);
	  //HoughHistMC->Fill(1/m,c);
	  //cout<<"\n"<<m<<"\t"<<c;
	}
    }
  int size = static_cast<int>(inverse_slope.size());
  cout <<"\n Size "<< size;

  //Calculating Median of Slope and Offset for Bin Size determination.
  double Median_InverseSlope, Median_Offset;
  sort(inverse_slope.begin(), inverse_slope.end());  
  if (size  % 2 != 0)
    Median_InverseSlope = inverse_slope[(size+1)/2];
  else  
    Median_InverseSlope =  (inverse_slope[(size)/2] + inverse_slope[(size+2)/2])/2.0 ;
  cout <<"\n Median_ Inverse Slope"<<Median_InverseSlope;

  sort(offset.begin(), offset.end());  
  if (size  % 2 != 0)
    Median_Offset = offset[(size+1)/2];
  else  
    Median_Offset =  (offset[(size)/2] + offset[(size+2)/2])/2.0 ;
  cout <<"\nMedian_Offset"<<Median_Offset <<"\n";
  /*
  for(int i=0; i<size; ++i)
  std::cout <<inverse_slope[i] << ' ';*/

  
  //Calculating Std.devaition of Slope and Offset for Bin Size determination.



  
  std::vector <double> accum_IS;
  std::for_each (std::begin(inverse_slope), std::end(inverse_slope), [&](const double d) {
      accum_IS.push_back(abs(d - Median_InverseSlope));
    });
  cout<<"\n Size of accum vector without static" << accum_IS.size();
  /*
  cout<<"\n the median deviation list\n";
  for(int i=0; i<size; ++i)
  std::cout <<accum[i] << ' ';*/
  
  double MAD_InverseSlope = 0;  
  sort(accum_IS.begin(), accum_IS.end());  
  if (size  % 2 != 0)
    MAD_InverseSlope = accum_IS[(size+1)/2];
  else
    MAD_InverseSlope = accum_IS[size/2];
    //Med_dev =  (accum[(size)/2] + accum[(size+2)/2])/2.0 ;

  cout <<"\n MAD_InverseSlope"<< MAD_InverseSlope;


  std::vector <double> accum_O;
  std::for_each (std::begin(inverse_slope), std::end(inverse_slope), [&](const double d) {
      accum_O.push_back(abs(d - Median_Offset));
    });

  cout<<"\n Size of accum vector with static" << accum_O.size();
  /*
  cout<<"\n the median deviation list\n";
  for(int i=0; i<size; ++i)
  std::cout <<accum[i] << ' ';*/
  
  double MAD_Offset = 0;  
  sort(accum_O.begin(), accum_O.end());  
  if (size  % 2 != 0)
    MAD_Offset = accum_O[(size+1)/2];
  else
    MAD_Offset = accum_O[size/2];
    //Med_dev =  (accum[(size)/2] + accum[(size+2)/2])/2.0 ;

  cout <<"\n MAD_Offset"<< MAD_Offset;


	


  
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
