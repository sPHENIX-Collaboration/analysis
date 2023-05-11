
#ifndef _HCALCONTROLS_H_
#define _HCALCONTROLS_H_
#include <Riostream.h>
#include <TString.h>


enum OBJECTKINDS {
  CALOR,
  COUNTER
};
enum GAINS {
  HIGH,
  LOW
};
enum STACKS{
  EMC,         //  EMCal
  HINNER,      //  Inner HCal
  HOUTER,      //  Outer HCal
  HODO,        //  Scintillating Hodoscope
  SCINT,       //  Scintillators (Counters along the beam line)+Cherenkovs
  CHER         //  CHerenkov counters
};

// **************************************************************************

//  CURRENTLY OPERATING DAQ
static const    int READOUTCHANNELS   = 192; // four ADC stations (EMC/HCInner/HCOuter/Service)

static const    int TRIGGERCHANNELS   = 0;   // 4;

//  defaults
static const    int chinspected       = 32;  //  high and low gains in HCal Inner
static const    int triggerinsp       = 0;

//  HCAL 2016 EXTERNAL COUNTERS AND HODOSCOPES

//  T1044 CONFIGURATION
static const      int CALSTACKS         = 6;   
static const      int T1044TRIGGERCH    = 14;
static const      int T1044HODO         = 16;
static const      int NSAMPLES          = 24;
static const Double_t PEDESTAL          = 2048.;
//  thresholds below are in units ADC in selected TRIGGER gain range
//  all thresholds    are in 8 adc counts steps
//  mixed amplitudes are stored in LG units, so the actual trigger thresholds is computed as
//  8./(TRGAINRANGE==0?  HLGRATIO : 1)*TWRAMPTHR
//  8./(TRGAINRANGE==0?  HLGRATIO : 1)*STTOTAMPTHR
//  the 8 corresponds to lowerst 3 bits suppressed by Chi before thresholding
static       Int_t    TRGAINRANGE       = 0;   //  gain range used to trigger
static const Int_t    STTOTAMPTHR       = 10;  //  total sum threshold in daq.c
static const Int_t    TWRAMPTHR         = 1;   //  individual tower threshold in daq.c
static const Int_t    STHITMIN          = 3;   //  minimal number of hits above threshold in stack in daq.c
static const Int_t    STHITMAX          = 14;
//  next two thresholds are computed based upon pedestal widths in towers and stacks
//  Towers(RMS) ~ 1.2; Stacks (RMS) ~4.7 (sum, cleaned), 7.1 (Fit)  (in units of LG)
static const Double_t TWRZEROSUPTHR     = 1.;  //  tower zero suppression threshold (units of LG) 
static const Double_t STZEROSUPTHR      = 5.; 
static const Double_t HLGRATIO          = 16.;
static const Float_t  hlgratios[]       = {  1., 32., 16., 1., 1., 1.};
static Int_t          emcGainSelection  = 1;
//  HCAL PROTOTYPE
static const    int HCALTOWERS        = 16;
static const    int HCALROWS          = 4;
static const    int HCALCOLUMNS       = 4;
static const    int HCALGAINS         = 2;

//  EMC  2016 PROTOTYPE
static const    int EMCTOWERS         = 64;
static const    int EMCROWS           = 8;  
static const    int EMCCOLUMNS        = 8; 
static const    int EMCGAINS          = 1;

static          int CHTOTAL           = 0;   // all channels stored into ROOT file         
static          int ACTIVECHANNELS    = 0;   // total number of mapped channels
static          int detchannels       = 0;  
static          int hgDetChannels     = 0;
static          int lgDetChannels     = 0;
static const    int ovrflow           =  1990;  // 2048
static const    int undflow           = -1990;  //-2048
//  CALORIMETER CALIBRATION
//  GeV/ADC count (LG)
static const Double_t stECalib[]      = {0.008, 0.010, 0.005, 1., 1., 1.};  
//  HINNER has HGain dropped by x2. 
//  This kind of Scaling to be used only to get uncalibrated sums of amplitudes
static       Double_t stAScale[]      = {1., 2., 1., 1., 1., 1.};

//  TILE
static const    int TILEFIBERS        = 4;
static const    int TILECHANNELS      = 2*TILEFIBERS;

//  TILE CONTROLS
//static const    int NSAMPLES          = 24;
static const    int TILETRIGGERCH     = 8;
static const    int TRIGGERRES        = 3;
static const    int CHANNELTHRESHOLDS = 25;
static const    int HITMULTTHRESHOLDS = 8;
static const    int RISETIME          = 4;
static const    int FALLTIME          = 5;
static const    int NPARAMETERS       = 6;
static const    int DISPLAYX          = 4;   // maximum number of channels per display panel
// number of channels in detector (set by Helper, if left zero on the first call to hLabHelper
//  - it will be set to ACTIVECHANNELS-TRIGGERCHANNELS

// **************************************************************************
//  all informative ADC (TILE) channels (stored into .root file)
static const int chInUse[]       = {  115, 113, 119, 117, 123, 121, 127, 125 };

// **************************************************************************
//  DEFAULTS FOR INSPECTION AND PLOTTING
//static const int feechinsp[] = {64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,
//				80,81,82,83,84,85,86,87,88,89,90,91.92,93.94,95};
//  Mapping for HINNER 3/2/2016
static const int feechinsp[] = {64,72,80,88,66,74,82,90,68,76,84,92,70,78,86,94,
				65,73,81,89,67,75,83,91,69,77,85,93,71,79,87,95};
// **************************************************************************
//  2016 FNAL Beam Test Prototype
static const int hcalInnerCh[] = { 64, 72, 80, 88, 66, 74, 82, 90, 68, 76, 84, 92, 70, 78, 86, 94,
				   65, 73, 81, 89, 67, 75, 83, 91, 69, 77, 85, 93, 71, 79, 87, 95};
static const int hcalOuterCh[] = {112,120,128,136,114,122,130,138,116,124,132,140,118,126,134,142,
				  113,121,129,137,115,123,131,139,117,125,133,141,119,127,135,143};
// static const int hcalOuterCh[] = {16,24,32,40,18,26,34,42,20,28,36,44,22,30,38,46,
// 				  17,25,33,41,19,27,35,43,21,29,37,45,23,31,39,47};





static const int emcCh[]       = {   5,  4,  7,  6,  1,  0,  3,  2, 13, 12, 15, 14,  9,  8, 11, 10,
				    21, 20, 23, 22, 17, 16, 19, 18, 29, 28, 31, 30, 25, 24, 27, 26, 
				    37, 36, 39, 38, 33, 32, 35, 34, 45, 44, 47, 46, 41, 40, 43, 42,
				    53, 52, 55, 54, 49, 48, 51, 50, 61, 60, 63, 62, 57, 56, 59, 58};
static const int hodoCh[]      = {  96, 97, 98, 99,100,101,102,103,104,105,106,107,108,109,110,111};
static const int counters[]    = { 144,145,146,147,148,149,150,
				   160,161,162,163,164,165,166}; //  including cherenkov's

// **************************************************************************
//  SMART TILE VERSION 0

static const    Double_t  tileSizeX      = 25.;
static const    Double_t  tileSizeY      = 15.;
static const    Int_t     minProjEntries = 50;  // min # of hits to try fitting the light yield
//  TILECHANNELS
static const int feech1[]        = { 115, 119, 123, 127, 113, 117, 121, 125 };  //  HG channels only. Valid for earlier runs
static const int feech2[]        = { 115, 113, 119, 117, 123, 121, 127, 125 };  //  HG channels only. Valid starting Run 803
//  TRIGGERCHANNELS
//static const int trch[]          = { 134, 135, 128, 129, 130, 131, 132, 133 };

static const int trch1125[]        = {  128, 129, 130, 131 };  //  two of the four channels are used by trigger tags
static const int trch1152[]        = {   98,  99, 129, 131 };  //  first are two gain ranges for Tower then sc trigger tags
//  Calibration data at different times
static const Double_t sc_779[] = { 3.6,    5.2,   5.2,   4.5,   6.6,  5.3,  7.9,  4.2  };  //  single cell calibration - cosmic run 779
static const Double_t mu_779[] = { 72.7, 130.6, 141.5, 132.1, 132., 138., 135.5, 68.4 };   //  cosmic calibration Run 779

static const Double_t sc_900[]  = {  6.6, 12.3, 12.9, 12.3, 14.6, 14.2, 12.25, 6.42 };      //  cosmic run, low amp. signals

static const Double_t sc_1061[] = {  6.71, 12.74, 13.13, 12.76, 14.30, 14.35, 12.70, 6.56 };  //  cosmic run, low amp. signals, mode 1
//const    Double_t sc_1061[] = {  6.65, 12.68, 12.86, 12.55, 14.25 , 14.10, 12.23, 6.59 };  //  cosmic run, low amp. signals, mode 0

static const Double_t sc_1123[] = {  6.61, 11.83, 12.77, 12.41, 13.75, 13.55, 12.31, 6.44 };  //  cosmic run, low amp. signals, mode 0
//  this is just an educated guess for the time being. Estimate includes Gain difference at the same bias.

// **************************************************************************
//      SMART TILE DATA
//  data and ROOT files on RCF
//static const TString             RCFdataDir("/direct/phenix+user04/sphenix/bnl/buffer/");
//static const TString             RCFrootDir("/direct/phenix+user04/sphenix/bnl/buffer/rootf/");
//  data and ROOT files on sphnxdaq
//static const TString             HLABdataDir("/scratch/buffer/bnl/");
//static const TString             HLABrootDir("/scratch/buffer/bnl/rootf/");
// **************************************************************************

//      PROTOTYPE 2016 COSMIC & LED  DATA
//  data and ROOT files on RCF
//  static const TString             RCFdataDir("/gpfs/mnt/gpfs02/sphenix/sim/sim01/user04/sphenix/bnl/buffer/");
//  before April 1st 2016
// static const TString             RCFdataDir("/gpfs/mnt/gpfs02/sphenix/data/data01/t1044-2016a/cosmics/");
// static const TString             RCFrootDir("/gpfs/mnt/gpfs02/sphenix/sim/sim01/user04phenix/bnl/buffer/tbsest/");
//  Beginning April 1st, 2016
//static const TString             RCFdataDir("/sphenix/data/data01/t1044-2016a/fnal/cosmics/");
static const TString             RCFdataDir("/sphenix/data/data01/t1044-2016a/fnal/");
static const TString             RCFrootDir("/sphenix/data/data01/kistenev/rootf/");

//  data and ROOT files on sphnxdaq
static const TString             HLABdataDir("/scratch/buffer/bnl/");
static const TString             HLABrootDir("/scratch/buffer/bnl/rootf/");
// **************************************************************************

//  USEFUL CONSTANTS
#define   piMass     0.1356



#endif // _HCALCONTROLS_H_
