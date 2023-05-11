#include "FindBlobs.h"
#include "groot.h"
#include "ABlob.h"
#include "AZigzag.h"

#include "Quiver.h"

#include "Riostream.h"
#include <iostream>

void FindBlobs()
{
  groot* Tree = groot::instance();
  
  /* clear blobs */
  for(int i=0; i<Nr; i++)
    {
      Tree->theBlobs[i].clear();
    }
 
  vector<AZigzag*> ziggy;  

  //cout << "R"; 
  //cout << "\tPhi"; 
  //cout << "\tStart Q";
  //cout << "\tRight Q";
  //cout << "\tLeft Q";
  //cout << "\tX-cen";
  //cout << "\tY-cen";
  //cout << "\tZ-cen";
  //cout << "\tR-cen";
  //cout << "\tPhi-cen" << endl;
 
  for(int i=0; i<Nr; i++)
    {
      for(int j=1; j<Nphi-1; j++)
	{
	  AZigzag* start = Tree->ZigzagMap2[i][j];
	  AZigzag* right = Tree->ZigzagMap2[i][j-1];
	  AZigzag* left  = Tree->ZigzagMap2[i][j+1];
	  
	  if( start && left && right ) //checks if Pads exist, only
	    {
	      if( start->IsHit() && start->T()>1.0 && start->T()<15.0 ) // Fits unstable if near edge
		{
		  if( (start->Q() >  right->Q()) && 
		      (start->Q() >= left->Q()) ) //finds center Zig
		    {
		      double TMAX = start->T();
		      int PADDING = -1;
		      int k=j;
		      while (PADDING < Quiver::PaddingLimit && k>=0) //adds all hit Pads to the RIGHT, and PadLimit right-most empty Pads
			{
			  if( Tree->ZigzagMap2[i][k] )
			    {
			      bool TIME_OK = true;
			      if (Quiver::BlobTimeCut)
				{
				  double TIME = Tree->ZigzagMap2[i][k]->T();
				  double MINT = -1.1;
				  double MAXT =  0.3;
				  if ( (TIME-TMAX) < MINT) TIME_OK = false;
				  if ( (TIME-TMAX) > MAXT) TIME_OK = false;
				}
			      if( Tree->ZigzagMap2[i][k]->IsHit() && TIME_OK)
				{
				  ziggy.push_back( Tree->ZigzagMap2[i][k] );
				}
			      else
				{
				  PADDING++;
				  if ( PADDING < Quiver::PaddingLimit  && TIME_OK ) ziggy.push_back( Tree->ZigzagMap2[i][k] );
				}
			    }
			  k--;
			}

		      PADDING = -1;
		      k = j+1;
		      while (PADDING < Quiver::PaddingLimit && k<Nphi) //adds all hit Pads to the LEFT, and right-most empty Pad
			{
			  if( Tree->ZigzagMap2[i][k] )
			    {
			      bool TIME_OK = true;
			      if (Quiver::BlobTimeCut)
				{
				  double TIME = Tree->ZigzagMap2[i][k]->T();
				  double MINT = -1.1;
				  double MAXT =  0.3;
				  if ( (TIME-TMAX) < MINT) TIME_OK = false;
				  if ( (TIME-TMAX) > MAXT) TIME_OK = false;
				}
			      if( Tree->ZigzagMap2[i][k]->IsHit() && TIME_OK)
				{
				  ziggy.push_back( Tree->ZigzagMap2[i][k] );
				}
			      else
				{
				  PADDING++;
				  if ( PADDING < Quiver::PaddingLimit  && TIME_OK ) ziggy.push_back( Tree->ZigzagMap2[i][k] );
				}
			      
			    }
			  k++;
			}
		      //cout << ziggy.size() << endl;;
		      if (ziggy.size()>0) Tree->theBlobs[i].push_back( new ABlob( ziggy ) );
		      ziggy.clear();
		      //cout << "got one" << endl;
		    }
		  

		  //cout << "~~FIND-BLOBS REPORT~~" << endl;
		  //cout << i; 
		  //cout << "\t" << j; 
		  //cout << "\t" << start->Q();
		  //cout << "\t" << right->Q();
		  //cout << "\t" << left->Q();
		  //cout << "\t" << start->XCenter();
		  //cout << "\t" << start->YCenter();
		  //cout << "\t" << start->ZCenter();
		  //cout << "\t" << start->RCenter();
		  //cout << "\t" << start->PCenter();
		  //cout << endl;

		  //cout << Tree->theBlobs[i].size() << endl;
		}
	    }
	}
    }
  //cout << "Blobs Done." << endl;
}
