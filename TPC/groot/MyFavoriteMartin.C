#include <iostream>
#include <cmath>
#include <pmonitor.h>
#include "MyFavoriteMartin.h"

#include "groot.h"
#include "ATrace.h"
#include "AZigzag.h"
#include "FindBlobs.h"
#include "FillBlobHist.h"
#include "FillHoughHist.h"

#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TNtuple.h"
#include "TFile.h"

#include "prdfoStream.h"
#include "Quiver.h"

#include "params.h"

//#include <stdlib.h>
#include <cstdlib>
#include <ctime>
#include <map>

prdfoStream *os = 0;
int init_done = 0;

//KD
int first_event_nr = 0;
//KD

using namespace std;

//  These are the waveforms...
//  They are the most basic monitor of the SRS system.
TH1F *h1a; 
TH1F *h1b; 
TH1F *h1c; 
TH1F *h1d; 

TH1F *h2a; 
TH1F *h2b; 
TH1F *h2c; 
TH1F *h2d; 

TH1F *h3a; 
TH1F *h3b; 
TH1F *h3c; 
TH1F *h3d; 

TH1F *h4a; 
TH1F *h4b; 
TH1F *h4c; 
TH1F *h4d; 

//  These "scope" histograms are the most basic monitor of the DRS4 system
TH1F *scope0, *scope1, *scope2, *scope3;

//  These hnl_D histograms are used to phase the PLL...
TH1F *h1a_D; 
TH1F *h1b_D; 
TH1F *h1c_D; 
TH1F *h1d_D; 
TH1F *h2a_D; 
TH1F *h2b_D; 
TH1F *h2c_D; 
TH1F *h2d_D; 
TH1F *h3a_D; 
TH1F *h3b_D; 
TH1F *h3c_D; 
TH1F *h3d_D; 
TH1F *h4a_D; 
TH1F *h4b_D; 
TH1F *h4c_D; 
TH1F *h4d_D; 

int pinit()
{
  // Here we will book all the histograms, but NONE of the 
  // Ntuples.  The reason is that we only want to make the NTuples
  // when we've made an output file.

  // Generate the First groot and initialize the Zigzags.
  groot *Tree = groot::instance();

  scope0 = new TH1F ( "scope0","waveform Ch 0", 1024, -0.5, 1023.5); 
  scope1 = new TH1F ( "scope1","waveform Ch 1", 1024, -0.5, 1023.5); 
  scope2 = new TH1F ( "scope2","waveform Ch 2", 1024, -0.5, 1023.5); 
  scope3 = new TH1F ( "scope3","waveform Ch 3", 1024, -0.5, 1023.5); 

  scope0->SetLineColor(kYellow+2);
  scope1->SetLineColor(kBlue);
  scope2->SetLineColor(kViolet);
  scope3->SetLineColor(kGreen);

  h1a = new TH1F( "h1a", "Waveform", 4500, -0.5, 4499.5);
  h1b = new TH1F( "h1b", "Waveform", 4500, -0.5, 4499.5);
  h1c = new TH1F( "h1c", "Waveform", 4500, -0.5, 4499.5);
  h1d = new TH1F( "h1d", "Waveform", 4500, -0.5, 4499.5);

  h2a = new TH1F( "h2a", "Waveform", 4500, -0.5, 4499.5);
  h2b = new TH1F( "h2b", "Waveform", 4500, -0.5, 4499.5);
  h2c = new TH1F( "h2c", "Waveform", 4500, -0.5, 4499.5);
  h2d = new TH1F( "h2d", "Waveform", 4500, -0.5, 4499.5);

  h3a = new TH1F( "h3a", "Waveform", 4500, -0.5, 4499.5);
  h3b = new TH1F( "h3b", "Waveform", 4500, -0.5, 4499.5);
  h3c = new TH1F( "h3c", "Waveform", 4500, -0.5, 4499.5);
  h3d = new TH1F( "h3d", "Waveform", 4500, -0.5, 4499.5);
  
  h4a = new TH1F( "h4a", "Waveform", 4500, -0.5, 4499.5);
  h4b = new TH1F( "h4b", "Waveform", 4500, -0.5, 4499.5);
  h4c = new TH1F( "h4c", "Waveform", 4500, -0.5, 4499.5);
  h4d = new TH1F( "h4d", "Waveform", 4500, -0.5, 4499.5);

  h1a_D = new TH1F( "h1a_D", "Waveform", 4001, -0.5, 4000.5);
  h1b_D = new TH1F( "h1b_D", "Waveform", 4001, -0.5, 4000.5);
  h1c_D = new TH1F( "h1c_D", "Waveform", 4001, -0.5, 4000.5);
  h1d_D = new TH1F( "h1d_D", "Waveform", 4001, -0.5, 4000.5);
  h2a_D = new TH1F( "h2a_D", "Waveform", 4001, -0.5, 4000.5);
  h2b_D = new TH1F( "h2b_D", "Waveform", 4001, -0.5, 4000.5);
  h2c_D = new TH1F( "h2c_D", "Waveform", 4001, -0.5, 4000.5);
  h2d_D = new TH1F( "h2d_D", "Waveform", 4001, -0.5, 4000.5);
  h3a_D = new TH1F( "h3a_D", "Waveform", 4001, -0.5, 4000.5);
  h3b_D = new TH1F( "h3b_D", "Waveform", 4001, -0.5, 4000.5);
  h3c_D = new TH1F( "h3c_D", "Waveform", 4001, -0.5, 4000.5);
  h3d_D = new TH1F( "h3d_D", "Waveform", 4001, -0.5, 4000.5);
  h4a_D = new TH1F( "h4a_D", "Waveform", 4001, -0.5, 4000.5);
  h4b_D = new TH1F( "h4b_D", "Waveform", 4001, -0.5, 4000.5);
  h4c_D = new TH1F( "h4c_D", "Waveform", 4001, -0.5, 4000.5);
  h4d_D = new TH1F( "h4d_D", "Waveform", 4001, -0.5, 4000.5);

  return 0;
}


int process_event (Event * e)
{  
  // Set the run number if event is from new run...
  int thisRun = e->getRunNumber();
  if( Quiver::RunNumber != thisRun ) Quiver::RunNumber = thisRun;
  
  // Line below is used to put NON-DATA events into output stream
  if ( os && e->getEvtType() >7 ) os->addEvent(e);

  //  Don't run physics analysis code on NON-DATA events!
  if (e->getEvtType() > 7) return 0;

  //Use nr-th event as starting point of process_event
  // and skip every event before that
  static int skipentries=0;
  skipentries++;
  static bool SetSkipEntry = true;
  if(SetSkipEntry)
    {
      cout << " Skipping " << first_event_nr << " events." 
	   << endl; 
      SetSkipEntry = false;
    }
  if(skipentries%100==0 && skipentries<first_event_nr)
    cout << " Skipped " << skipentries << " events." << endl;
  if(e->getEvtSequence() < first_event_nr && e->getEvtType()==1)
    return 0;

  // Here is a data integrity check for the SRS system
  // Use this for offline analysis at a mature level only.
  static bool PriorPassed = true;
  Packet *p = e->getPacket(1010);
  if (p)
    {
      int nhb = p->iValue(0,"NHYBRIDS");

      //  DIagnostic...
      // cout << "Number of Hybrids:  " << nhb << endl;
      // cout << "Number of Samples:  " << endl;
      // for (int i=0; i<nhb; i++)
      // 	{
      // 	  int nsmp = p->iValue(i,"NSAMPLES");
      // 	  cout << "  " << nsmp;
      // 	}
      // cout << endl;
      
      //  Check that the data has the right number of hybrids.
      if (nhb != Nhybrid)
	{
	  if (PriorPassed)
	    {
	      cout << "Wrong number of hybrids: " 
		   << nhb << " found but expected " 
		   << Nhybrid << ", abort event." 
		   << endl;
	    }
	  else
	    {
	      cout << ".";
	    }
	  delete p;
	  PriorPassed = false;
	  return 0;
	}
      
      //  Check that all hybrids have the same data length as the first one.
      for (int i=1; i<nhb-1; i++)
	{
	  if (p->iValue(0,"NSAMPLES") != p->iValue(i,"NSAMPLES"))
	    {
	      if (PriorPassed)
		{
		  cout << "Data length mismatch: " 
		       << p->iValue(i,"NSAMPLES")
		       << " out of " << p->iValue(0,"NSAMPLES")
		       << " on hybrid " << i
		       << " abort event." 
		       << endl;
		}
	      else
		{
		  cout << ".";
		}
	      delete p;
	      PriorPassed = false;
	      return 0;
	    }      
	}
      if (!PriorPassed) cout << endl;
      PriorPassed = true;
    }
  
  // Fancy event counter written to the screen...
  static int entries=0;
  entries++;
  if (entries%10==0)
    {
      cout << "Processed " << entries;
      time_t now = time(0);
      char* dt = ctime(&now);      
      cout << " at " << dt << endl;
    }

  groot *Tree = groot::instance();
  Tree->ClearTheDetector();
  Tree->event = e;

  scope0->Reset();
  scope1->Reset();
  scope2->Reset();
  scope3->Reset();

  h1a->Reset();
  h1b->Reset();
  h1c->Reset();
  h1d->Reset();
  
  h2a->Reset();
  h2b->Reset();
  h2c->Reset();
  h2d->Reset();
  
  h3a->Reset();
  h3b->Reset();
  h3c->Reset();
  h3d->Reset();

  h4a->Reset();
  h4b->Reset();
  h4c->Reset();
  h4d->Reset();

  // Reading the PSI 4-channel "oscilloscope"...
  Packet *p1 = e->getPacket(1020);
  if (p1 && Quiver::AnalyzeScope)
    {
      int samples = p1->iValue(0, "SAMPLES"); 
      for ( int i = 0; i < samples; i++)
	{
	  
	  scope0->Fill ( i, -p1->rValue(i,0) );
	  scope1->Fill ( i, -p1->rValue(i,1) );
	  scope2->Fill ( i, -p1->rValue(i,2) );
	  scope3->Fill ( i, -p1->rValue(i,3) );
	  
	  Tree->theTraces[0]->voltage.push_back(-p1->rValue(i,0));
	  Tree->theTraces[1]->voltage.push_back(-p1->rValue(i,1));
	  Tree->theTraces[2]->voltage.push_back(-p1->rValue(i,2));
	  Tree->theTraces[3]->voltage.push_back(-p1->rValue(i,3));

	}
    }
  if (p1) delete p1;

  //  Now monitor and fill SRS to groot.
  if (p && Quiver::AnalyzeSRS) 
    {

      int nhb = p->iValue(0,"NHYBRIDS");
      for (int i=0; i<nhb; i++)
	{
	  if (p->iValue(i,"NSAMPLES")<24)
	    {
	      cout << "ERROR in Sample Length, "<< p->iValue(i,"NSAMPLES") <<" abort event" << endl;
	      return 0;
	    }
	}

      static bool FirstEvent = true;
      if (FirstEvent)
	{
	  cout << "Reports for " << nhb << " Hybrids:" << endl;
	  for (int i=0; i<nhb; i++)
	    {
	      cout << i << ": " << p->iValue(i,"NSAMPLES") << endl;
	    }
	  FirstEvent = false;
	}
      

      //  Check the hybrid count for sensibility...
      if (nhb>=1)
	{
	  int idigi;
	  for (idigi = 0; idigi< 4500; idigi++)
	    {
	      h1a->Fill( idigi, p->iValue(idigi, 0, "RAWSAMPLES"));
	      h1b->Fill( idigi, p->iValue(idigi, 1, "RAWSAMPLES"));
	      h1c->Fill( idigi, p->iValue(idigi, 2, "RAWSAMPLES"));
	      h1d->Fill( idigi, p->iValue(idigi, 3, "RAWSAMPLES"));
	      
	      h2a->Fill( idigi, p->iValue(idigi, 4, "RAWSAMPLES"));
	      h2b->Fill( idigi, p->iValue(idigi, 5, "RAWSAMPLES"));
	      h2c->Fill( idigi, p->iValue(idigi, 6, "RAWSAMPLES"));
	      h2d->Fill( idigi, p->iValue(idigi, 7, "RAWSAMPLES"));
	      
	      h3a->Fill( idigi, p->iValue(idigi, 8, "RAWSAMPLES"));
	      h3b->Fill( idigi, p->iValue(idigi, 9, "RAWSAMPLES"));
	      h3c->Fill( idigi, p->iValue(idigi,10, "RAWSAMPLES"));
	      h3d->Fill( idigi, p->iValue(idigi,11, "RAWSAMPLES"));

	      h4a->Fill( idigi, p->iValue(idigi, 12, "RAWSAMPLES"));
	      h4b->Fill( idigi, p->iValue(idigi, 13, "RAWSAMPLES"));
	      h4c->Fill( idigi, p->iValue(idigi, 14, "RAWSAMPLES"));
	      h4d->Fill( idigi, p->iValue(idigi, 15, "RAWSAMPLES"));

	      if (idigi>500 && idigi<3500)
		{
		  h1a_D->Fill( p->iValue(idigi, 0, "RAWSAMPLES"));
		  h1b_D->Fill( p->iValue(idigi, 1, "RAWSAMPLES"));
		  h1c_D->Fill( p->iValue(idigi, 2, "RAWSAMPLES"));
		  h1d_D->Fill( p->iValue(idigi, 3, "RAWSAMPLES"));
		  h2a_D->Fill( p->iValue(idigi, 4, "RAWSAMPLES"));
		  h2b_D->Fill( p->iValue(idigi, 5, "RAWSAMPLES"));
		  h2c_D->Fill( p->iValue(idigi, 6, "RAWSAMPLES"));
		  h2d_D->Fill( p->iValue(idigi, 7, "RAWSAMPLES"));
		  h3a_D->Fill( p->iValue(idigi, 8, "RAWSAMPLES"));
		  h3b_D->Fill( p->iValue(idigi, 9, "RAWSAMPLES"));
		  h3c_D->Fill( p->iValue(idigi,10, "RAWSAMPLES"));
		  h3d_D->Fill( p->iValue(idigi,11, "RAWSAMPLES"));
		  h4a_D->Fill( p->iValue(idigi,12, "RAWSAMPLES"));
		  h4b_D->Fill( p->iValue(idigi,13, "RAWSAMPLES"));
		  h4c_D->Fill( p->iValue(idigi,14, "RAWSAMPLES"));
		  h4d_D->Fill( p->iValue(idigi,15, "RAWSAMPLES"));
		}
	    }
	  
	  // Read the tracker raw data into the raw array...
	  int rindex=0;
	  for (int JINX =0; JINX<nhb; JINX++)
	    {
	      for ( int j = 0; j< CHhybrid; j++)
		{
		  for (int i=0; i<min(p->iValue(JINX,"NSAMPLES"),24); i++) // limit to 24 samples...
		    {
		      //  For now we'll use a trivial routine to analyze waveform.
		      //  We'll just sum across the samples...
		      AZigzag::Raw[rindex].push_back(p->iValue(j,i,JINX));
		    }
		  rindex++;
		}	      
	    }
	  AZigzag::DetermineCommonMode();
	  AZigzag::ApplyCalibration();
	  for (int i=0; i<Tree->theZigzags.size(); i++)
	    {
	      Tree->theZigzags[i]->DetermineQ();
	    }
	  FindBlobs();
	  FillBlobHist();
	  //FillHoughHist();
	}
      else
	{
	  cout << "ERROR " << nhb << " Hybrids Found:" << endl;
	  for (int i=0; i<nhb; i++)
	    {
	      cout << i << ": " << p->iValue(i,"NSAMPLES") << endl;
	    }	  
	}
    }


  // Here select whether to write the event to the output stream.
  static int nSaved=0;
  if ( os && Tree->theTracks.size() >= 1 ) 
    {
      nSaved++;
      os->addEvent(e);
      if (nSaved%100==0) 
	{
	  cout << "Saved " << nSaved;
	  cout << " percentage " 
	       << 100.0*(double)nSaved/(double)entries;
	  cout << endl;
	}
    }
  if (p) delete p;

  return 0;  
}



int prdfopen ( const char *filename)
{
  if (os) 
    {
      cout << "file is already open " << endl;
      return -1;
    }

  os = new prdfoStream (filename);
  if ( os->is_defunct() )
    {
      delete os;
      os = 0;
      return -1;
    }

  return 0;
}

int prdfclose ()
{
  if ( !os) 
    {
      cout << "no file open " << endl;
      return -1;
    }
  delete os;
  os = 0;
  return 0;
}



//Function that allows to start prun() from the nr-th event
int getFirstEventNr() { return first_event_nr; }

void setFirstEventNr(const int nr) { first_event_nr = nr; }

// Binary number to GrayCode (courtesy of Martin)
int b2G(int num) { return (num >> 1) ^ num; }
