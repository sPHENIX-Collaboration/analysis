#include "ABlob.h"
#include "AZigzag.h"
#include <iostream>

#include "groot.h"

#include "TH1D.h"
#include "TF1.h"

#include "TMarker.h"

using namespace std;

bool ABlob::RecalibrateCharge = true;
bool ABlob::GaussPosition = false;
TH1* ABlob::BlobPos   = 0;
TH1* ABlob::BlobSigma = 0;
TF1* ABlob::BlobFit   = 0;

ABlob::ABlob(std::vector<AZigzag*> MANYZIGZAGS)
{
  manyZigs = MANYZIGZAGS;

  correctedq = 0;
  correctedcentroid = 0;

  if (RecalibrateCharge) FixTheCharges();

  Precalc_PHI = GetPHI();
  Precalc_R   = manyZigs[0]->RCenter();
}


double ABlob::CentroidX() { return Precalc_R * cos(Precalc_PHI); }

double ABlob::CentroidY() { return Precalc_R * sin(Precalc_PHI); }

double ABlob::CentroidR() { return Precalc_R; }

double ABlob::CentroidP() { return Precalc_PHI; }

double ABlob::maxT()      { return manyZigs[0]->T(); } //time of zigzag with the max charge


double ABlob::Q()
{
  double den = 0;
  for(int i=0; i<manyZigs.size(); i++)
    {
      den += manyZigs[i]->Q();
    }

  return den;
}

double ABlob::CentroidZ()
{
  if (Q() == 0)
    {
      cout << "ABlob:: Error: Blob has zero total charge...no Z centroid" << endl;
      return -9999;
    }

  double num = 0;
  for(int i=0; i<manyZigs.size(); i++)
    {
      num += manyZigs[i]->Q() * manyZigs[i]->ZCenter();
    }

  return num/Q();
}


double ABlob::GetPHI() //Angle Phi
{
  if (Q() == 0)
    {
      cout << "ABlob:: Error: Blob has zero total charge...no Phi centroid" << endl;
      return -9999;
    }

  if (!GaussPosition)
    {
      double num = 0;
      for(int i=0; i<manyZigs.size(); i++)
	{
	  num += manyZigs[i]->Q() * manyZigs[i]->PCenter();
	}
      
      return num/Q();
    }
  else
    {
      groot* Tree = groot::instance();
      if (!BlobPos)
	{
	  double Minphi =  9999;
	  double Maxphi = -9999;
	  for (int i=0; i<Nphi; i++)
	    {
	      if (Tree->ZigzagMap2[0][i])
		{
		  double phi = Tree->ZigzagMap2[0][i]->PCenter();
		  if (phi < Minphi) Minphi = phi;
		  if (phi > Maxphi) Maxphi = phi;
		}
	    }
	  double dPhi = abs( Tree->ZigzagMap2[0][90]->PCenter() - Tree->ZigzagMap2[0][91]->PCenter() );
	  int NBINS = int( (Maxphi - Minphi)/dPhi + 1.5 );
	  double left  = Minphi - 0.5*dPhi;
	  double right = Maxphi + 0.5*dPhi;

	  BlobPos   = new TH1D("BlobPos", "BlobPos", NBINS, left, right);
	  BlobSigma = new TH1D("BlobSigma", "BlobSigma", 250, 0.0, 8.0E-3);
	  BlobFit   = new TF1("BlobFit", "[0]*exp(-(x-[1])*(x-[1])/(2*[2]*[2]))", left, right);
	  Tree->theHistograms.push_back(BlobSigma);
	}
      
      BlobPos->Reset();
      
      for(int i=0; i<manyZigs.size(); i++)
	{
	  double phi = manyZigs[i]->PCenter();
	  double Q   = manyZigs[i]->Q();
	  BlobPos->Fill(phi,Q);
	}

      for (int i=1; i<=BlobPos->GetNbinsX(); i++)
	{
	  BlobPos->SetBinError( i, 9.0 );
	}

      double dPhi  = abs( Tree->ZigzagMap2[0][90]->PCenter() - Tree->ZigzagMap2[0][91]->PCenter() );
      double max   = BlobPos->GetMaximum();
      double mean  = manyZigs[0]->PCenter();
      double sigma = 0.003;
      if (sigma < dPhi/3.0) sigma = dPhi/3.0;
      if (sigma > 5.0*dPhi) sigma = 5.0*dPhi;
      
      BlobFit->SetParameter(0,max);
      BlobFit->SetParameter(1,mean);
      BlobFit->SetParameter(2,sigma);

      BlobFit->SetParLimits(0, max/2.0, 3.0*max);
      BlobFit->SetParLimits(1, mean-1.0*dPhi, mean+1.0*dPhi);
      BlobFit->SetParLimits(2, 0.0007, 0.0055);

      BlobPos->Fit(BlobFit,"Q0");

      BlobSigma->Fill(BlobFit->GetParameter(2));

      return BlobFit->GetParameter(1);;
    }
}

void ABlob::Draw()
{
  // color zigzag according to charge fraction of total deposited charge
  int shape = 8;
  double X = CentroidX();
  double Y = CentroidY();

  TMarker* ablob = new TMarker(X,Y,shape);
  
  ablob->SetMarkerSize(1);
  ablob->SetMarkerColor(6);
  
  ablob->Draw();
}

void ABlob::Report()
{
  //cout << "~~BLOB REPORT~~" << endl;
  //cout << "Q:" << Q();
  //cout << " \tX:" << CentroidX();
  //cout << " \tY:" << CentroidY();
  //cout << " \tZ:" << CentroidZ();
  cout << " \tR:" << CentroidR();
  cout << " \tP:" << CentroidP();
  //cout << " \tT:" << maxT();
  //cout << " \tsize:" << manyZigs.size();
  cout << endl;
}

void ABlob::FixTheCharges()
{
  //  This routine is an attempt to improve the values of charge 
  //  from the "edge" zigzags of the blob.  It is premised upon the
  //  following assumptions:
  //
  //  1) When the charge hit is big, the Zigzag does a fine job
  //     of determining both Q and T.
  //  2) When the charge hit is small, the fit can be drawn away 
  //     from the correct Q by following a fluctuation at the wrong 
  //     time.
  //  3) Since every blob should have the same time for all zigzags, 
  //     we can pull a trick for the outlying pads as follows:
  //     a:  Determine a reference time by accessing the time from the
  //         pad with the most charge.
  //     b:  Perform a NEW FIT of the charge on every pad by restricting 
  //         the time fit range to be "near" to that of the reference time.
  //
  //  In this way outlier pads will always use the "in time" charge instead
  //  of the "highest found" charge and hopefully improve the overall 
  //  blob position resolution.
  //
  //                                       TKH 10/10/2018
  ///////////////////////////////////////////////////////////////////////

  double Reftime = maxT();
  double Mintime = Reftime - 2.0;
  double Maxtime = Reftime + 2.0;

  for(int i=0; i<manyZigs.size(); i++)
    {
      manyZigs[i]->DetermineQ(Mintime,Maxtime);
    }

}
