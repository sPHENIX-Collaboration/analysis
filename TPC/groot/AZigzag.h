#ifndef __AZIGZAG_H__
#define __AZIGZAG_H__

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "params.h"


class TPolyLine;
class TH2D;
class TH1D;
class TF1;
class TRandom;

class AZigzag
{
public:
  AZigzag(CheveronPad_t paddef);
  virtual ~AZigzag() {}
  
  //Read the zig zag pad coordinates from XML file
  //static void ReadXML();
  //virtual int fileopen(const std::string &filenam);
  //virtual int fileclose();
  
  // zig zag strip in x and y from XML file
  CheveronPad_t fPads;    
  
  //Draw
  virtual void Draw(double MAX);
  int color(int);
  
  void Clear() {q=0;}
  virtual void Report();

  // sets...
  void SetQ(double Q) {q = Q;}
  void SetT(double T) {t = T;} /* @TODO Do we need this? */
  void AddQ(double Q) {q += Q;}

  // gets...
  double Q            ()  {return q;}
  double T            ()  {return t;} /* @TODO Do we need this? */
  double MAXQ         ()  {return maxq;}
  double MAXT         ()  {return maxt;}
  int MyID            ()  {return myID;}
  void SetMyID(int ID)    {myID = ID;}
  static int NCreated ()  {return nextID;}

  //  Readout Space Coordinates for utility
  int MyHybrid;
  int MyChannel;
  int MyWaveIndex;
  int iR;
  int iPhi;

  double XCenter();
  double YCenter();
  double ZCenter();
  double RCenter();
  double PCenter();

  double myR, myPhi;


  //  Charge Handling Variables
  double NumSigma() {return q/Gains[myID]/Sigmas[myID];}
  bool IsHit() 
  {
    if (UseSigma) 
      return (NumSigma()>SigmaCut);
    else 
      return (q>PulseCut);
  }
  void DetermineQ(double Mintime=-2.0, double Maxtime=30.0);  // This covers the full APV space...

  //  Neighbor Handling Pointers.
  bool IsIsolated();
  AZigzag* PreLogical;
  AZigzag* PostLogical;
  AZigzag* PreWaveform;
  AZigzag* PostWaveform;
    
  //  OK..we're going for it...
  //  We shall be using the very same sytle of manipulations
  //  for the strips as we developed for the Hexes.  This means
  //  That we shall make a set of static member variables that
  //  treat the common data that all Hexes need to access.
  //
  //  This includes calibrations (one copy for all), cuts for
  //  hit definitions, Raw and Cal "striped" data.
  
  static TRandom Randy;

  //  Flags for calibration operation...
  static bool FastQ;
  static bool UseSigma;
  static double SigmaCut;
  static double PulseCut;
  static std::string CommonModeMethod;
  
  //  Collective Data...
  static std::vector<int>    Raw[Nsrs];
  static std::vector<double> Cal[Nsrs];  //array OF vectors
  
  //  Calibration storage...
  static double Pedestals[Nsrs];
  static double Sigmas[Nsrs];
  static double Gains[Nsrs];
  static std::vector<double> CommonMode[Nhybrid];
  
  //  Methods for applying calibrations...
  static void WriteCalibration();
  static void ReadCalibration();
  static void DetermineCommonMode();
  static void ApplyCalibration();
  
  static TH1D* commy[Nhybrid];
  static TH1D* Pulse; /* @TODO Do we need this? */
  static TF1*  blue; /* @TODO Do we need this? */
    
  //Protected
  double xinit;
  double yinit;
  double rinit;
  double phinit;
  
  double t;  /* @TODO Do we need this? */
  double q;
  double maxq;
  double maxt;

  static int nextID;
  int myID;

};

#endif
