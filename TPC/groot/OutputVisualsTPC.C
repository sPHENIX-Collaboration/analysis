//////////////////////////////////////////////
//      Histogram output.        	    //
//         Cherenkov Detector Software	    //
//		   Marie Blatnik            //
//                 Thomas Videbaek	    //
//	       Dr. Thomas Hemmick	    //
//                                          //
//      An AHex Readout.	            //
//		   Version 2.0              //
//		Thursday, 2-28-2013	    //
//////////////////////////////////////////////

#include <iostream>
#include <cmath>
#include <vector>

#include "groot.h"
#include "AZigzag.h"
#include "ABlob.h"
#include "ATrack.h"
#include "TCanvas.h"
#include "TPad.h"
#include "Quiver.h"


using namespace std;

void OutputVisualsTPC(double MAX=2000)
{
  groot* Tree = groot::instance();
 
  /* get pixel resolution ratio for screen to make sure trackers are diplayed as squares */
  double screenratio_y_x = 1280. / 1024.;

  /* create one canvas for each tracker */
  static TCanvas* CanvasTPC = new TCanvas("CanvasTPC", "sPHENIX TPC", 0,0, 800, (int)800.*screenratio_y_x); //pixels
  CanvasTPC->Range(-150,400,150,600); //x1,y1,x2,y2
  CanvasTPC->SetWindowPosition(660,0);
  
  /* clear previous event from canvas */
  CanvasTPC->Clear();
  CanvasTPC->cd();

  /* draw zigzags */
  if(Quiver::DrawZigzags)
    {
      for(int i=0; i< Tree->theZigzags.size(); i++)
	{
	  Tree->theZigzags[i]->Draw(MAX);
	}
    }

  /* draw blobs */
  if(Quiver::DrawBlobs)
    {
      for(int i=0; i<Nr; i++)
	{
	  for(int j=0; j< Tree->theBlobs[i].size(); j++)  
	    {
	      (Tree->theBlobs[i])[j]->Draw();
	    }
	}
    }

  CanvasTPC->Update();
  CanvasTPC->cd();
}
