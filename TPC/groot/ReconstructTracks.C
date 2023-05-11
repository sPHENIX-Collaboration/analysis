#include "ReconstructTracks.h"
#include "groot.h"
#include "ABlob.h"
#include "ABlob.h"

#include "ATrack.h"

#include "TH2D.h"
#include "TMath.h"

#include <iostream>
#include <cmath>

using namespace std;


static TH2D *trkDeltaOri; // accumalation of uncorrected data per run
static TH2D *trkDeltaRes; // accumalation of corrected and normalized data per run

void ReconstructTracks()
{
     groot* Tree = groot::instance();
     Tree->theTracks.clear();

     //
     //  OK Track Fans.  Now that we have some studies 
     //  done of the beam at FNAL, we find that the 
     //  TRUE beam is pretty well columnated.  For this reason,
     //  we can a priori reject blob combinations that
     //  do not follow the standard pointing.  
     //
     //  We shall apply hard-coded fits to the rough 
     //  correlation between the track directions and
     //  thereby be able to make cuts that will result in
     //  only pretty good tracks.
     //
     //  Wish us luck!
     //
     //                           TKH 10-12-2013
     //
     //

     static bool trkCreateOutput = true;
     double xmu, ymu, xsg, ysg, xysg;

     if(trkCreateOutput) { // create new containers. Once per running program
       trkDeltaOri = new TH2D("trkDeltaOri","trkDeltaOri;dX (cm);dY (cm)",768,-10,+10,768,-10,+10);
       trkDeltaRes = new TH2D("trkDeltaRes","trkDeltaRes;Normalized dX; Normalized dY",120,-6,+6,120,-6,+6);
       trkCreateOutput = false;
     }

     // gathering blob info
     const int mxcl1 = 40;
     const int mxcl2 = 90; //mxcl2>mxcl1!
     int cl1[mxcl1], cl2[mxcl2];
     int ncl1=0, ncl2=0;
     for( int i=0; i< Tree->theBlob.size(); i++) 
       {
	 if ( Tree->theBlob.at( i )->ID() == 0 )
	   cl1[ncl1++] = i;
	 if ( Tree->theBlob.at( i )->ID() == 1 )
	   cl2[ncl2++] = i;
	 if((ncl1+1)>mxcl1) 
	   {
	     printf("We found more than %d blobs!!\n",mxcl1);
	     break;
	   }
	 if((ncl2+1)>mxcl2) 
	   {
	     printf("We found more than %d blobs!!\n",mxcl2);
	     break;
	   }
       }
     // making all combinations
     xysg = ReconstructTracks_Ref(xmu,xsg,ymu,ysg);
     double var[mxcl1][mxcl2];
     double vc3[mxcl1][mxcl2];
     for(int i=0; i!=ncl1; ++i)
       for(int j=0; j<ncl2; j++) {
	 double x1 = Tree->theBlob[cl1[i]]->BestX();
	 double y1 = Tree->theBlob[cl1[i]]->BestY();
	 double x2 = Tree->theBlob[cl2[j]]->BestX();
	 double y2 = Tree->theBlob[cl2[j]]->BestY();
	 double c1 = Tree->theBlob[cl1[i]]->GetNSigma();
	 double c2 = Tree->theBlob[cl2[j]]->GetNSigma();
	 double dx = x2-x1;
	 double dy = y2-y1;
	 trkDeltaOri->Fill(dx/10000,dy/10000); //in cm
	 double nSigmaX = (dx-xmu)/xsg;
	 double nSigmaY = (dy-ymu)/ysg;
	 trkDeltaRes->Fill(nSigmaX,nSigmaY);
	 double c3 = nSigmaX*nSigmaX + nSigmaY*nSigmaY;
	 var[i][j] = c1*c1+c2*c2+c3;
	 vc3[i][j] = TMath::Sqrt(c3);
       }
     // sorting
     int nMAXtrk=TMath::Min(ncl1,ncl2);
     int trk[mxcl1][2];
     int ntrk=0;
     for(;ntrk!=nMAXtrk;++ntrk) {
       double min = 1e12; // to save time and hopping no nsigma is bigger than this =)
       int pi, pj;
       double c3;
       for(int i=0; i!=ncl1; ++i) {
	 bool skip = false;
	 for(int n=0; n!=ntrk; ++n) if(trk[n][0]==cl1[i]) skip=true; // skipping combinations with used blobs from TR1
	 if(skip) continue;
	 //cout << "loop " << cl1[i] << endl;
	 for(int j=0; j!=ncl2; ++j) {
	   skip=false;
	   for(int n=0; n!=ntrk; ++n) if(trk[n][1]==cl2[j]) skip=true; // skipping combinations with used blobs from TR2
	   if(skip) continue;
	   //cout << "  loop " << cl2[j] << " quality^2 " << var[i][j] << endl;
	   if(var[i][j]<min) {
	     min = var[i][j];
	     c3 = vc3[i][j];
	     pi = cl1[i];
	     pj = cl2[j];
	   }
	 }
       }
       trk[ntrk][0] = pi;
       trk[ntrk][1] = pj;
       vector<ABlob*> tempvec;
       tempvec.push_back( Tree->theBlob.at( pi ) ); //tr1
       tempvec.push_back( Tree->theBlob.at( pj ) ); //tr2
       Tree->theTracks.push_back ( new ATrack( tempvec, 
					      Tree->theBlob.at(pi)->GetNSigma(), 
					      Tree->theBlob.at(pj)->GetNSigma(),
					      c3) );
       //cout << "Found track " << ntrk << " made out of blobs " << pi << " and " << pj << " with quality " << Tree->theTracks.at(ntrk)->GetOverallQuality() << endl;
     }

}
