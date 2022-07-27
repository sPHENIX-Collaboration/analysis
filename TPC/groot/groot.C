#include "groot.h"

//Important padplane headers
#include "ATrace.h"
#include "ABlob.h"
#include "ATrack.h"
#include "AZigzag.h"

#include "Event.h"
#include "Quiver.h"
#include "TFile.h"
#include "TH1.h"

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>  // Used to read $MYCALIB environment variable...

#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost;
groot* groot::__instance=0;

groot::groot()
{
  cout << "I am GROOT!";
  cout << endl;

  //  There are 4 readouts from the DRS4
  for(int i=0; i<4; i++)
    {
      theTraces.push_back(new ATrace());
    }

  //  Used for neighboring cuts...
  for (int i=0; i<Nsrs; i++)
    {
      ZigzagMap[i]=0;  
      ZWaveMap[i]=0;  
    }

  // Radius by Angle matrix of all the pads
  for (int i=0; i<Nr; i++)
    {
      for (int j=0; j<Nphi; j++)
	{ 
	  ZigzagMap2[i][j]=0;
	}
    }

  //  Used to understand APV25 decoder order...
  for (int i = 0; i < CHhybrid; i++)
   {
     int channel = 32 * ( i % 4 ) + 8 * ( i >> 2) - 31 * ( i >> 4);
     demapper[channel]=i;
     remapper[i]=channel;
   }

  CreateZigzags();         //  This makes the set of zigzags that form the TPC module.
  FindNeighbors();         //  Find trackers neighboring channels.
  ClearTheDetector();      //  Just to be sure that hexes are "born clear".

  AZigzag::ReadCalibration();
}


groot::~groot()
{
  for( unsigned int i=0; i<theTraces.size(); i++ ) { delete theTraces[i]; }

  for( unsigned int i=0; i< theTracks.size(); i++ ) { delete theTracks[i]; }

  for( unsigned int i=0; i< theZigzags.size(); i++ ) { delete theZigzags[i]; }  

  for( unsigned int i=0; i<Nr; i++ )
    {
      for ( unsigned int j=0; i< theBlobs[i].size(); j++ ) { delete (theBlobs[i])[j]; }

      theBlobs[i].clear();
    }
}


void groot::Report()
{
  cout << "I am groot!" << endl;

  cout << "TRACES -------------------------------------------------------" << endl;
  for (unsigned int i=0; i< theTraces.size(); i++)
    {
      cout << " Trace " << i;
      cout << " Maximum: " << theTraces[i]->FindMaximum(1) << "  " <<endl;
    }
  
  cout << "ZIGZAGS -------------------------------------------------------" << endl;
  int totalHit=0;
  for (unsigned int i=0; i< theZigzags.size(); i++)
    {
      /*
      if ( theZigzags[i]->IsHit() )
	{
	  cout << "Zigzag " << i << "  " << endl;
	  theZigzags[i]->Report();
	  
	  int j;
	  if (i%64 == 0) cin >> j;
	  
	  totalHit++;
	}
      cout << "Total Pads Hit: " << totalHit << endl;
      */
    }
 
  cout << " BLOBS --------------------------------------------------------------" << endl;
  for(int i=0; i<Nr; i++)
    {
      cout << "*** ROW: " << i+1 << endl;
      for (int j=0; j<theBlobs[i].size(); j++)
	{
	  theBlobs[i][j]->Report();
	}
    }
}


void groot::ClearTheDetector()
{
  //  clear the voltage vector in each trace.
  for (int i=0; i<4; i++)
    {
      theTraces[i]->voltage.clear();
    }

  //  AZigzag static variables...
  for (int i=0; i<Nsrs; i++)
    {
      AZigzag::Raw[i].clear();
      AZigzag::Cal[i].clear();
    }

   //  destroy all the old blobs...
  for(unsigned int i=0; i<Nr; i++)
    {
      for ( unsigned int j=0; j< theBlobs[i].size(); j++ )
	{
	  delete (theBlobs[i])[j];
	}
    theBlobs[i].clear();
    }

    //  destroy all the old tracks...	  
  for ( unsigned int i=0; i< theTracks.size(); i++ )
    {
      delete theTracks[i];
    }  
  theTracks.clear();
}


void groot::FindNeighbors()
{
  cout << "waveform neighbors" << endl;
  //  Determine the waveform neighbors...
  for (int j=0; j<Nhybrid; j++)
    {
      for (int k=0; k<CHhybrid; k++)
	{
	  int i = CHhybrid*j + k;
	  if (ZWaveMap[i] !=0)
	    {
	      if (k>0)          ZWaveMap[i]->PreWaveform  = ZWaveMap[i-1];
	      if (k<CHhybrid-1) ZWaveMap[i]->PostWaveform = ZWaveMap[i+1];
	    }
	}
    }

  cout << "logical neighbors" << endl;
  //  Determine the logical neighbors...
  for (int j=0; j<Nr; j++)
    {
      for (int k=0; k<Nphi; k++)
	{
	  if (ZigzagMap2[j][k] !=0)
	    {
       	      if (k>0)      ZigzagMap2[j][k]->PreLogical  = ZigzagMap2[j][k-1];
	      if (k<Nphi-1) ZigzagMap2[j][k]->PostLogical = ZigzagMap2[j][k+1]; 
	    }
	}
    }
}


void groot::FillZigzagMaps()
{
  cout << "Filling the Zigzag Maps..." << endl;

  string dir( getenv("MYCALIB") );
  string file("PadMap.txt");
  string result = dir + file;
  cout << "Looking for PadMap.txt in " << result << endl;
  ifstream fin(result.c_str());

  if (!fin.is_open())
    {
      cout <<  "Groot could not open the file PadMap.txt." << endl;
      cout <<  "Please initialize the variable $MYCALIB in your .login." << endl;
      cout <<  "Then move the PadMap.txt file from the groot source to there." << endl;
      cout <<  "Only after that will you be allowed to continue analysis." << endl;
      return;
    }
  cout << "FOUND!" << endl;

  string line;
  while (!fin.eof())
    {
      fin >> line;

      vector<string> items;
      boost::split(items, line, [](char c){return c == ',';});

      Padplane_NameToConnector[items[0]] = items[1];
      Padplane_NameToSamPin[items[0]] = items[2];
    }
  fin.close();

  //map<string, string>::iterator it1;
  // for (it1 = Padplane_NameToConnector.begin(); it1 != Padplane_NameToConnector.end(); it1++)
  //   {
  //     cout << " " << it1->first;
  //     cout << " " << it1->second;
  //     cout << endl;
  //   }

  dir = getenv("MYCALIB");
  file = "PanSamMap.txt";
  result = dir + file;
  cout << "Looking for PadMap.txt in " << result << endl;
  fin.open(result.c_str());

  if (!fin.is_open())
    {
      cout <<  "You could not open the file PanSamMap.txt." << endl;
      cout <<  "Please initialize the variable $MYCALIB in your .login." << endl;
      cout <<  "Then move the PanSamMap.txt file from the groot source to there." << endl;
      cout <<  "Only after that will you be allowed to continue analysis." << endl;
      return;
    }
  cout << "FOUND!" << endl;

  while (!fin.eof())
    {
      fin >> line;
  
      vector<string> items;
      boost::split(items, line, [](char c){return c == ',';});

      PanSam_SamPinToPanPin[items[0]] = atoi(items[1].c_str());
    }
  fin.close();

  //map<string, int>::iterator it2;
  //for (it2 = PanSam_SamPinToPanPin.begin(); it2 != PanSam_SamPinToPanPin.end(); it2++)
  //  {
  //    cout << " " << it2->first;
  //    cout << " " << it2->second;
  //    cout << endl;
  //  }
}


int groot::PanPinToChannel(int PanPin)
{
  if (PanPin%2 == 0)
    {
      return 132-PanPin-2;  // the -2 makes channel instead of pin
    }
  else
    {
      return 130-PanPin-2;  // the -2 makes channel instead of pin
    }
}


int groot::ConnectorToHybrid(string Conn)
{
  // FIrst get rid of the U$ business..
  string ConnNumber = Conn.substr(2);
  int Number = atoi(ConnNumber.c_str());

  int hybrid;
  if (Number%2 == 0)
    {
      hybrid = Number-2;
    }
  else
    {
      hybrid = Number;
    }

  //cout << " " << Conn;
  //cout << " " << ConnNumber;
  //cout << " " << Number;
  //cout << " " << hybrid;
  //cout << endl;

  return hybrid;
}


void groot::CreateZigzags()
{
  FillZigzagMaps();

  string dir( getenv("MYCALIB") );
  string file("OUTPUT_trim.XML");
  string result = dir + file;
  cout << "Looking for OUTPUT_trim.XML in " << result << endl;
  ifstream fin(result.c_str());

  if (!fin.is_open())
    {
      cout <<  "You could not open the file OUTPUT_trim.XML." << endl;
      cout <<  "Please initialize the variable $MYCALIB in your .login." << endl;
      cout <<  "Then move the OUTPUT_trim.XML file from the groot source to there." << endl;
      cout <<  "Only after that will you be allowed to continue analysis." << endl;
      return;
    }
  cout << "FOUND!" << endl;

  double tmp;
  CheveronPad_t thispad;

  for(;;) 
    {
    fin >> tmp;
    if(!fin.good()) break;
    if(tmp==99999) // begin of polygon detected
      { 
      fin >> thispad.fName;
      fin >> thispad.fPolyWidth;
      fin >> thispad.fPolyLayer;
      for(;;) 
	{
	  fin >> tmp;
	  if(tmp==999999) break;
	  thispad.fX.push_back(tmp);
	  double x = tmp;
	  fin >> tmp;
	  thispad.fY.push_back(tmp);
	  double y = tmp;
	  double R = TMath::Sqrt(x*x+y*y);
	  double Phi = TMath::Pi()+TMath::ATan2(-y,-x);
	  thispad.r.push_back( R );
	  thispad.phi.push_back( Phi );
	}
      //cout << thispad.fName.Data();
      //cout << " size of points " << thispad.fX.size() << endl;
      fin >> tmp; thispad.fVia.push_back( tmp ); //0
      fin >> tmp; thispad.fVia.push_back( tmp ); //
      fin >> tmp; thispad.fVia.push_back( tmp ); //2
      fin >> tmp; thispad.fVia.push_back( tmp ); //
      fin >> tmp; thispad.fVia.push_back( tmp ); //4
      fin >> tmp; thispad.fVia.push_back( tmp ); //
      fin >> tmp; thispad.fVia.push_back( tmp ); //6
      fin >> tmp; thispad.fVia.push_back( tmp ); //

      fin >> tmp; thispad.fWire.push_back( tmp ); //0
      fin >> tmp; thispad.fWire.push_back( tmp ); //
      fin >> tmp; thispad.fWire.push_back( tmp ); //2
      fin >> tmp; thispad.fWire.push_back( tmp ); //
      fin >> tmp; thispad.fWire.push_back( tmp ); //4
      fin >> tmp; thispad.fWire.push_back( tmp ); //

      int hybrid = ConnectorToHybrid(Padplane_NameToConnector[thispad.fName]);
      int channel = PanPinToChannel(PanSam_SamPinToPanPin[Padplane_NameToSamPin[thispad.fName]]);
      int index = CHhybrid*hybrid + channel;

      //  Because some runs did not use all known hybrids...
      if (hybrid >= Nhybrid) continue;

      //  OK, looks like this is a valid hybrid.
      //  Make the hybrid and set it up (mostly)...
      AZigzag* thisZigzag = new AZigzag(thispad);
      thisZigzag->SetMyID(index);
      thisZigzag->MyHybrid = hybrid;
      thisZigzag->MyChannel= channel;
      thisZigzag->MyWaveIndex = demapper[channel];
      theZigzags.push_back( thisZigzag );
      
      //  Now setup the utility arrays for the Mapping of neighbors...
      vector<string> items;
      split(items, thispad.fName, [](char c){return c == '.';});
      int ir   = atoi(items[1].c_str());
      int iphi = atoi(items[2].c_str());
      thisZigzag->iR   = ir;
      thisZigzag->iPhi = iphi;
      ZigzagMap [index]    = thisZigzag;
      ZigzagMap2[ir][iphi] = thisZigzag;
      ZWaveMap[hybrid*128+demapper[channel]] = thisZigzag;

      // Print out some stuff...
      // cout << " " << thispad.fName;
      // cout << " " << Padplane_NameToConnector[thispad.fName];
      // cout << " " << ConnectorToHybrid(Padplane_NameToConnector[thispad.fName]);
      // cout << " " << Padplane_NameToSamPin[thispad.fName];
      // cout << " " << PanSam_SamPinToPanPin[Padplane_NameToSamPin[thispad.fName]];
      // cout << " " << PanPinToChannel(PanSam_SamPinToPanPin[Padplane_NameToSamPin[thispad.fName]]);
      // cout << " ind " << index;
      // cout << " ir " << ir;
      // cout << " ip " << iphi;
      // cout << " hyb+de " << hybrid*128+demapper[channel] ;
      // cout << " " << index;
      // cout << endl;
    }
    thispad.fX.clear();
    thispad.fY.clear();
    thispad.fVia.clear();
    thispad.fWire.clear();
    thispad.r.clear();
    thispad.phi.clear();
  }
fin.close();
}

void groot::SaveTheHistograms(string filename)
{
  TFile *theFile = new TFile(filename.c_str(),"RECREATE");

  for (int i=0; i<theHistograms.size(); i++)
    {
      theHistograms[i]->Write();
    }

  theFile->Close();
}

void groot::FillAccessBlobs()
{
  // zero out any and all old data...
  for (int i=0; i<Nr; i++)
    {
      for (int j=0; j<10; j++)
	{
	  AccessBlobs[i][j] = 0;
	}
    }

  // Fill em up...
  for (int i=0; i<Nr; i++)
    {
      BlobCount[i] = theBlobs[i].size();
      int MAX = (( theBlobs[i].size() < 10) ?  theBlobs[i].size() : 10);
      for (int j=0; j<MAX; j++)
	{
	  AccessBlobs[i][j] = theBlobs[i][j];
	}
    }

}
