#ifndef __groot_H__
#define __groot_H__


//
//  Hello TPC Fans :P
//
//    Welcome to groot.  groot is the code to analyze SLAC
//  test beam data.  It gets its name form the highly non-uniform
//  namture of the MPC-EX and also the test beam apparatus.
//
//    If this code were written for PHENIX, data objects 
//  (Blobs, Clusters, etc) would be <vector> located in a PHENIX "Node Tree" 
//  (linked list).  Since TESTBEAM is not PHENIX, we have  TEMPORARILY(!!!) located 
//  these objects inside groot.
//
//    This file principally declares vectors of object types that are essential to the 
//  pattern recognition itself.
//
//    The thing that distinguishes groot from legolas is the inclusion of
//  the two trackers but exclusion of the RICH.  The trackers will be used to define 
//  the vectors coming from the tracks and thereby define the places of shower
//  centers and also the number of electrons. 
//
//
//                                                TKH
//                                                2-27-2014
//

#include <vector>
#include <string>
#include <map>
#include "TRandom.h"

#include "params.h"

// Forward declarations...
class ATrace;
class ABlob;
class ATrack;
class AZigzag;
class Event;
class TH1;

class groot
{
 public:
  static groot *instance()
  {
    if (__instance) 
      {
	return __instance;
      }
    else
      {
	__instance = new groot();
	return __instance;
      }
  }
  virtual ~groot();

  /* BEAMLINE Counters */
  std::vector<ATrace*>   theTraces;

  /* Tracker */
  std::vector<AZigzag*>  theZigzags;
  std::vector<ABlob*>    theBlobs[Nr];

  //  To allow easy access at the prompt...
  ABlob* AccessBlobs[Nr][10];
  int    BlobCount[Nr];
  void   FillAccessBlobs();


  /* Tracker combined */
  std::vector<ATrack*>   theTracks;

  Event* event;

  std::vector<TH1*> theHistograms;

  // Utility Functions...
  void Report();
  void ClearTheDetector();
  void SaveTheHistograms(std::string filename);

  TRandom Randy;

  // ZZM[r][phi] is an ORDERED set of pointers to the Zigzags
  // WM[WaveformIndex] is a pointer to a Zigzag in waveform order (used to suppress crosstalk).
  AZigzag* ZigzagMap2[Nr][Nphi];
  AZigzag* ZigzagMap[Nsrs];
  AZigzag* ZWaveMap[Nsrs];  

protected:
  groot();
  static groot *__instance;

  void CreateZigzags();
  void FillZigzagMaps();
  void CreateZigzagMaps();
  void FindNeighbors();

  //  demapper[MartinIndex] = WaveformIndex
  //  remapper[WaveformIndex] = MartinIndex
  int demapper[128];
  int remapper[128];

  std::map<std::string,std::string> Padplane_NameToConnector;
  std::map<std::string,std::string> Padplane_NameToSamPin;

  std::map<std::string,int> PanSam_SamPinToPanPin;

  //  Accepts SBU panasonic numbering as input.
  //  Returns true APV channel number...
  int PanPinToChannel(int PanPin);

  //  This takes a connector like U$1 and tells you the hybrid index...
  int ConnectorToHybrid(std::string Conn);

};

#endif /* __groot_H__ */
