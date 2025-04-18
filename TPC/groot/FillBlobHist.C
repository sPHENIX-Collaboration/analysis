////////////////////////////////////
//        FillBlobHist.C          //
////////////////////////////////////

// This makes a BH (Blob Bistogram) that can show the raw hits.

#include "FillBlobHist.h"
#include "TH1D.h"
#include "TH2D.h"
#include "ABlob.h"
#include "AZigzag.h"

#include "groot.h"

#include <iostream>
#include <cmath>
#include <string>

TH1D* BH_Q = 0;       // How many blobs of a certain charge
TH1D* BH_Size = 0;    // How many blobs made of a certain # of zig-zag pads
TH2D* BH_QvsSize = 0; // Charge of a blob vs. how many pads it's made of

TH1D* BH_PhiTic = 0;  // Just the phi location of each pad
TH2D* BH_PhivsR = 0;  // Phi vs. Radius

TH1D* BH_Tmax = 0; // This is the time of the zig with the most charge
TH1D* BH_Tdiff = 0;

TH1D* BH_5Residual = 0; // Minor cheating based on arc-length for a quick preliminary resolution
TH2D* BH_5Residual_vsPhi = 0;
 TH1D* BH_5Residual_PC = 0;
 TH2D* BH_5Residual_vsPhi_PC = 0;
  TH1D* BH_5Efficiency = 0; // Checks if a blob even exists on the 5th radii if 4 & 6 are hit


// Histograming ALL Radii through an array
char name[100];
TH1D* BH_Residual_[16]; // Initialize rows 0 and 15 (in case they're useful later)
TH2D* BH_Residual_vsPhi_[16];
 TH1D* BH_Residual_PC_[16]; // PC = PhiCut (efficiency/track attempt before Hough)      
 TH2D* BH_Residual_vsPhi_PC_[16];
  TH1D* BH_Efficiency_[16];


using namespace std;

void FillBlobHist()
{
  groot* Tree = groot::instance();

  // phi=90 and phi=91 are instrumented (connected to hybrids)
  double phi90 = Tree->ZigzagMap2[0][90]->PCenter();
  double phi91 = Tree->ZigzagMap2[0][91]->PCenter();

  double phi0   = 91.0*phi90 - 90.0*phi91;
  double phi127 = phi0 + 127.0*(phi91-phi90);

  double BIN_FACTOR = 10.0;
  double dPhi_BIN   = (phi91-phi90)/BIN_FACTOR;

  double min = phi0 - dPhi_BIN/2.0;
  double max = phi127 + dPhi_BIN/2.0;
  int  N_BIN = 127*BIN_FACTOR;

  if (!BH_Q)
    {
      BH_Q       = new TH1D("BH_Q", "BH_Q",              256, -0.5, 2047.5);
      BH_Size    = new TH1D("BH_Size", "BH_Size",         21, -0.5, 20.5);
      BH_QvsSize = new TH2D("BH_QvsSize", "BH_QvsSize",  256, -0.5, 2047.5, 21, -0.5, 20.5);
      BH_PhiTic  = new TH1D("BH_PhiTic", "BH_PhiTic",    2*N_BIN, min, max); // "normalized" tics
      BH_PhivsR  = new TH2D("BH_PhivsR", "BH_PhivsR",      N_BIN, min, max, 16, -0.5, 15.5);
      BH_Tmax    = new TH1D("BH_Tmax", "BH_Tmax",        300, -5.0, 30.0); // "normalized" tics
      BH_Tdiff   = new TH1D("BH_Tdiff", "BH_Tdiff",      20000, -30.0, 30.0); // "normalized" tic difference
  
      // Resolution-study Histograms
      BH_5Residual          = new TH1D("BH_5Residual", "BH_5Residual", 200, -1000.0, 1000.0); //arc length in microns
      BH_5Residual_vsPhi    = new TH2D("BH_5Residual_vsPhi", "BH_5Residual_vsPhi", (int)(0.2/dPhi_BIN), 1.50, 1.66, 200, -1300.0, 1300.0);
      BH_5Residual_PC       = new TH1D("BH_5Residual_PC", "BH_5Residual_PC", 200, -1000.0, 1000.0); //arc length in microns
      BH_5Residual_vsPhi_PC = new TH2D("BH_5Residual_vsPhi_PC", "BH_5Residual_vsPhi_PC", (int)(0.2/dPhi_BIN), 1.53, 1.63, 200, -1000.0, 1000.0);
      BH_5Efficiency        = new TH1D("BH_5Efficiency", "BH_5Efficiency", 2, 0, 2);
     

      // Histograming ALL Radii through an array
      for (int i=1; i<15; i++) // Can't do radii 0 or 15 since we don't have radii -1 or 16 for comparison
	{
	  sprintf(name, "BH_Residual_%d", i);
	                 BH_Residual_[i]          = new TH1D(name, name, 200,-1000.0, 1000.0);
	  sprintf(name, "BH_Residual_vsPhi_%d", i);
	                 BH_Residual_vsPhi_[i]    = new TH2D(name, name, (int)(0.2/dPhi_BIN), 1.50, 1.66, 200, -1300.0, 1300.0);
	  sprintf(name, "BH_Residual_PC_%d", i);
	                 BH_Residual_PC_[i]       = new TH1D(name, name, 200,-1000.0, 1000.0);
	  sprintf(name, "BH_Residual_vsPhi_PC_%d", i);
	                 BH_Residual_vsPhi_PC_[i] = new TH2D(name, name, (int)(0.2/dPhi_BIN), 1.50, 1.66, 200, -1300.0, 1300.0);
	  sprintf(name, "BH_Efficiency_%d", i);
	                 BH_Efficiency_[i]        = new TH1D(name, name, 2, 0, 2);
	}


      // Used for Condor
      Tree->theHistograms.push_back(BH_Q);
      Tree->theHistograms.push_back(BH_Size);
      Tree->theHistograms.push_back(BH_QvsSize);
      Tree->theHistograms.push_back(BH_PhiTic );
      Tree->theHistograms.push_back(BH_PhivsR);
      Tree->theHistograms.push_back(BH_Tmax);
      Tree->theHistograms.push_back(BH_Tdiff);
      Tree->theHistograms.push_back(BH_5Residual);
      Tree->theHistograms.push_back(BH_5Residual_PC);
      Tree->theHistograms.push_back(BH_5Residual_vsPhi);
      Tree->theHistograms.push_back(BH_5Residual_vsPhi_PC);
      Tree->theHistograms.push_back(BH_5Efficiency);
      for (int i=1; i<15; i++) 
	{
	  Tree->theHistograms.push_back(BH_Residual_[i]);
	  Tree->theHistograms.push_back(BH_Residual_vsPhi_[i]);
	  Tree->theHistograms.push_back(BH_Residual_PC_[i]);
	  Tree->theHistograms.push_back(BH_Residual_vsPhi_PC_[i]);
	  Tree->theHistograms.push_back(BH_Efficiency_[i]);
	}
    } // End of creating histograms


  if ( BH_PhiTic->GetMaximum() != 1) //Gets the positions of the pads. Hist hight 0 to give better BH_->Scale(int) control
    {
      for (int i=0; i<Nphi; i++)
	{
	  if (Tree->ZigzagMap2[0][i] ) { BH_PhiTic->Fill(Tree->ZigzagMap2[0][i]->PCenter()); }
	}
    }

  // All radii are in mm. Only convert to microns in resolution since all x,y values are also in mm
  double RadiusValue[16];
  for (int k=0; k<16; k++) { RadiusValue[k] = Tree->theZigzags[k]->RCenter(); }


  for (int i=0; i<Nr; i++)
    {
      for (int j=0; j< Tree->theBlobs[i].size(); j++)
	{
	  double Q   = Tree->theBlobs[i][j]->Q();
	  double Phi = Tree->theBlobs[i][j]->CentroidP();
	  int Radius = i;

	  int numZigs = Tree->theBlobs[i][j]->numZigs(); 

	  if ( Tree->theBlobs[i].size()==1 ) // Only 1 blob per radius
	    {
	      BH_Q->Fill(Q);
	      BH_Size->Fill(numZigs);
	      BH_QvsSize->Fill(Q, numZigs);	      

	      BH_PhivsR->Fill(Phi, Radius);

	      double TMAX = Tree->theBlobs[i][0]->maxT();
	      BH_Tmax->Fill(TMAX);
	      for (int k=0; k< Tree->theBlobs[i][0]->manyZigs.size(); k++)
		{
		  BH_Tdiff->Fill( Tree->theBlobs[i][0]->manyZigs[k]->T() - TMAX );
		}
	    }
	}
    } // End of priliminary histograms



  // Original full Resolution studies (over all angles)
  if ( Tree->theBlobs[3].size()==1 && Tree->theBlobs[4].size()==1 && Tree->theBlobs[5].size()==1 ) 
    { 
      // Calculation to handle polar coordinates correctly
      double X4 = Tree->theBlobs[3][0]->CentroidX(), Y4 = Tree->theBlobs[3][0]->CentroidY();
      double X6 = Tree->theBlobs[5][0]->CentroidX(), Y6 = Tree->theBlobs[5][0]->CentroidY(); 
	      
      double x5a, y5a, x5b, y5b; //line must intersect the circle in 2 points
      int found = line_circle(X4, Y4, X6, Y6, RadiusValue[4], x5a, y5a, x5b, y5b); //takes 9 arguments
	
      if (found == 2) // Nonesense unless both are found
	{
	  double X5p = x5a, Y5p = y5a;
	  if (y5b > 0)
	    {
	      X5p = x5b; Y5p = y5b;
	    }
	
	  double Phi5p = atan2(Y5p, X5p);                   // predicted/expected angle, should be around 1.58 [rad]
	  double Phi5a = Tree->theBlobs[4][0]->CentroidP(); // actual angle
	  double Residual5 = Phi5a - Phi5p;                 // difference in angles
	
	  BH_5Residual->Fill(RadiusValue[4]*Residual5*1000.0); //angle * radius * conversion = arc length [microns]
	  BH_5Residual_vsPhi->Fill(Phi5p, RadiusValue[4]*Residual5*1000.0); //Fill (predicted/expected angle, arc length[microns]);
	} // End of filling actual histographs
    } // End of making sure only one blob per radii
    

  // New efficiency histograms with a Phi cut
  if ( Tree->theBlobs[3].size()==1 && Tree->theBlobs[5].size()==1 )
    {
      if ( (Tree->theBlobs[3][0]->CentroidP() >= 1.53) && (Tree->theBlobs[3][0]->CentroidP() <= 1.63) &&
	   (Tree->theBlobs[5][0]->CentroidP() >= 1.53) && (Tree->theBlobs[5][0]->CentroidP() <= 1.63) )
	{
	  if ( Tree->theBlobs[4].size()==1 )
	    {
	      BH_5Efficiency->Fill(1.5); 

	      // Calculation to handle polar coordinates correctly
	      double X4 = Tree->theBlobs[3][0]->CentroidX(), Y4 = Tree->theBlobs[3][0]->CentroidY();
	      double X6 = Tree->theBlobs[5][0]->CentroidX(), Y6 = Tree->theBlobs[5][0]->CentroidY(); 
	      
	      double x5a, y5a, x5b, y5b; //line must intersect the circle in 2 points
	      int found = line_circle(X4, Y4, X6, Y6, RadiusValue[4], x5a, y5a, x5b, y5b); //takes 9 arguments
	      
	      if (found == 2) // Nonesense unless both are found
		{
		  double X5p = x5a, Y5p = y5a;
		  if (y5b > 0)
		    {
		      X5p = x5b; Y5p = y5b;
		    }
		  
		  double Phi5p = atan2(Y5p, X5p);                   // predicted/expected angle, should be around 1.58 [rad]
		  double Phi5a = Tree->theBlobs[4][0]->CentroidP(); // actual angle
		  double Residual5 = Phi5a - Phi5p;                 // difference in angles
		  
		  BH_5Residual_PC->Fill(RadiusValue[4]*Residual5*1000.0); //angle * radius * conversion = arc length [microns]
		  BH_5Residual_vsPhi_PC->Fill(Phi5p, RadiusValue[4]*Residual5*1000.0); //Fill (predicted/expected angle, arc length[microns]);
		} // End of filling actual histographs
	    } // End of one blob on 5
	  else { BH_5Efficiency->Fill(0.5); }
	} // End of one blob on 4 & 6 within angle cut
    } // End of one blob per adjacent radii








//---------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------|ALL RESOLUTIONS|----------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------

  for (int i=1; i<15; i++)
    {
      // Original full Resolution studies (over all angles)
      if ( Tree->theBlobs[i-1].size()==1 && Tree->theBlobs[i].size()==1 && Tree->theBlobs[i+1].size()==1 ) 
	{ 
	  // Calculation to handle polar coordinates correctly
	  double Xpre  = Tree->theBlobs[i-1][0]->CentroidX(), Ypre  = Tree->theBlobs[i-1][0]->CentroidY();
	  double Xpost = Tree->theBlobs[i+1][0]->CentroidX(), Ypost = Tree->theBlobs[i+1][0]->CentroidY(); 
	      
	  double xia, yia, xib, yib; //line must intersect the circle in 2 points
	  int found = line_circle(Xpre, Ypre, Xpost, Ypost, RadiusValue[i], xia, yia, xib, yib); //takes 9 arguments
	
	  if (found == 2) // Nonesense unless both are found
	    {
	      double Xip = xia, Yip = yia;
	      if (yib > 0) { Xip = xib; Yip = yib; }
	
	      double Phi_ip = atan2(Yip, Xip);                   // predicted/expected angle, should be around 1.58 [rad]
	      double Phi_ia = Tree->theBlobs[i][0]->CentroidP(); // actual angle
	      double Residuali = Phi_ia - Phi_ip;                // difference in angles
	
	      BH_Residual_[i]->Fill(RadiusValue[i]*Residuali*1000.0); //angle * radius * conversion = arc length [microns]
	      BH_Residual_vsPhi_[i]->Fill(Phi_ip, RadiusValue[i]*Residuali*1000.0); //Fill (predicted/expected angle, arc length[microns]);
	    } // End of filling actual histographs
	} // End of making sure only one blob per radii
    

      // New efficiency histograms with a Phi cut
      if ( Tree->theBlobs[i-1].size()==1 && Tree->theBlobs[i+1].size()==1 )
	{
	  double PC_LOW  = 1.53;
	  double PC_HIGH = 1.63;
	  if ( (Tree->theBlobs[i-1][0]->CentroidP() >= PC_LOW) && (Tree->theBlobs[i-1][0]->CentroidP() <= PC_HIGH) &&
	       (Tree->theBlobs[i+1][0]->CentroidP() >= PC_LOW) && (Tree->theBlobs[i+1][0]->CentroidP() <= PC_HIGH) )
	    {
	      if ( Tree->theBlobs[i].size()==1 )
		{
		  BH_Efficiency_[i]->Fill(1.5); 

		  // Calculation to handle polar coordinates correctly
		  double Xpre  = Tree->theBlobs[i-1][0]->CentroidX(), Ypre  = Tree->theBlobs[i-1][0]->CentroidY();
		  double Xpost = Tree->theBlobs[i+1][0]->CentroidX(), Ypost = Tree->theBlobs[i+1][0]->CentroidY(); 
	      
		  double xia, yia, xib, yib; //line must intersect the circle in 2 points
		  int found = line_circle(Xpre, Ypre, Xpost, Ypost, RadiusValue[i], xia, yia, xib, yib); //takes 9 arguments
	      
		  if (found == 2) // Nonesense unless both are found
		    {
		      double Xip = xia, Yip = yia;
		      if (yib > 0) { Xip = xib; Yip = yib; }
		  
		      double Phi_ip = atan2(Yip, Xip);                   // predicted/expected angle, should be around 1.58 [rad]
		      double Phi_ia = Tree->theBlobs[i][0]->CentroidP(); // actual angle
		      double Residuali = Phi_ia - Phi_ip;                // difference in angles
		  
		      BH_Residual_PC_[i]->Fill(RadiusValue[i]*Residuali*1000.0); //angle * radius * conversion = arc length [microns]
		      BH_Residual_vsPhi_PC_[i]->Fill(Phi_ip, RadiusValue[i]*Residuali*1000.0); //Fill (predicted/expected angle, arc length[microns]);
		    } // End of filling actual histographs
		} // End of one blob on 5
	      else { BH_Efficiency_[i]->Fill(0.5); }
	    } // End of one blob on 4 & 6 within angle cut
	} // End of one blob per adjacent radii
    
    } // end of ARRAY of histograms

//---------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------|ALL RESOLUTIONS END|------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------

}






int line_circle(double m, double b, double r, double& Xi1, double& Yi1, double& Xi2, double& Yi2)
{
  return line_circle( 0.0, b, 1.0, (m+b), r, Xi1, Yi1, Xi2, Yi2);
}


int line_circle(double X1, double Y1, double X2, double Y2, double r,  double& Xi1, double& Yi1, double& Xi2, double& Yi2) //takes 9 arguments
{
  double dx = X2 - X1;
  double dy = Y2 - Y1;
  double dr = sqrt(dx*dx + dy*dy);
  double D  = X1*Y2 - X2*Y1;

  double DELTA = r*r*dr*dr - D*D;
  if (DELTA < 0) return 0;

  if (DELTA == 0)
    {
      double sign = (dy > 0) - (dy < 0); // Implements the sign() function
      Xi1 =  D*dy/(dr*dr);
      Yi1 = -D*dx/(dr*dr);
      return 1;
    }

  double sign = (dy > 0) - (dy < 0); // Implements the sign() function
  Xi1 = ( D*dy + sign*dx*sqrt(DELTA))/(dr*dr);
  Yi1 = (-D*dx + abs(dy)*sqrt(DELTA))/(dr*dr);
  Xi2 = ( D*dy - sign*dx*sqrt(DELTA))/(dr*dr);
  Yi2 = (-D*dx - abs(dy)*sqrt(DELTA))/(dr*dr);
  return 2;
}
