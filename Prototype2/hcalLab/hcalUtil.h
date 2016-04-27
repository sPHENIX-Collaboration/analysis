#ifndef __HCALUTIL_H__
#define __HCALUTIL_H__
//#include <TString.h>
#include <list>
#include <algorithm>

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TGMsgBox.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TString.h>
#include <TText.h>
#include <TApplication.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TGClient.h>
#include <TGWindow.h>
#include <TH1.h>
#include <TH1F.h>
#include <TH1D.h>
#include <TProfile.h>
#include <TF1.h>
#include <TH2.h>
#include <TH3.h>
#include <TMath.h>
#include <TSpectrum.h>
#include "TVirtualFitter.h"
#include <TSystemDirectory.h>
#include <TSystemFile.h>

class hcalUtil;
class hLabHelper;
class hcalHelper;
class hcalTree;
class tileHelper;
class tileTree;
class hcal;
class stack;
class tower;


#include <hcalControls.h>

// **************************************************************************

extern "C" Double_t erfunc(Double_t *x, Double_t * par); 
extern "C" Double_t erf_g(Double_t *x, Double_t * par);
extern "C" Double_t powerFun(Double_t *x, Double_t * par);
extern "C" Double_t power_g(Double_t *x, Double_t * par);
extern "C" Double_t ps( Double_t *adc, Double_t *par);
extern "C" Double_t sipmPeaks(Double_t *x, Double_t *par);
extern "C" Int_t    sipmCalib(TString & hName, int mode=0);
extern "C" Double_t AsymToX(Double_t asym);
//extern "C" void     pcPattern(Int_t nx, Int_t ny, Int_t run=0, Int_t mode=1);
extern "C" Double_t signalShape(Double_t *x, Double_t * par);
//extern "C" void     fitShape(int chan);
//extern "C" void     status();
//extern "C" Int_t    evLoop(int run, int nevents=0, int nfiles=1);
//extern "C" void     closeEvent();
//extern "C" void     closeLoop();

// **************************************************************************

//#ifndef __CINT__
//   ====================================================================================
  
struct rootfile{
  rootfile(TString n, Int_t rn, TString rid) : name(n) ,runnumber(rn), runid(rid) {;}
  rootfile(const rootfile & last){
    this->name          = last.name;
    this->runnumber     = last.runnumber;
    this->runid         = last.runid;
  }  
  void print(){
    cout<<"<rootfile>  "<<name<<"  Run # "<<runnumber<<"  RunId  "<<runid<<endl;  
  }
  TString    name;
  Int_t      runnumber;
  TString    runid;
    
};
//  bool    operator()  runnumberequal(rootfile & rf, Int_t run) const {return run==rf.runnumber;}
//   ====================================================================================

struct prdffile{
  prdffile(TString n, Int_t rn, TString rid) : name(n) ,runnumber(rn), runid(rid) {;}
  prdffile(const prdffile & last){
    this->name          = last.name;
    this->runnumber     = last.runnumber;
    this->runid         = last.runid;
  }
  void print(){
    cout<<"<prdffile>  "<<name<<"  Run # "<<runnumber<<"  RunId  "<<runid<<endl;  
  }
  TString name;
  Int_t   runnumber;
  TString runid;
};
//  bool    operator()  runnumberequal(prdffile & pf, Int_t run) const {return run==pf.runnumber;}

//   ====================================================================================

class hLabHelper{

protected:
  hLabHelper();
  virtual ~hLabHelper(){;}
  static   hLabHelper * single;

  
public:
  static hLabHelper * getInstance(){if(!single) single = new hLabHelper(); return single;}
  static void         deleteInstance(){if(single) delete single;} 

  bool    evDisplay(Int_t run);

  void    getFileLists();
  void    decoderun(TString & fname, Int_t & rn, TString & rid);
  TFile * attachrootrun(Int_t rn);
  void    setRunKind(TString rK = "beam"); 
  Bool_t  setPRDFRun(int run, Bool_t bookH = kTRUE);
  Bool_t  setROOTRun(int run = 0);
  Int_t   runToRootFile(Int_t run = 0);
  Int_t   runToPRDFFile(Int_t run = 0);
  void    renameAndCloseRF();
  void    makeCanvasDirectory();
  void    updateMap();
  void    updateCalibration();
  void    initPRDFRun(Bool_t  bookH = kTRUE);
  void    fitShape(int chan, Double_t & peakVal, Int_t & peakPos, Int_t mode);
  void    getDetectorTiming();
  void    displaySumEvent();
  void    dofixes();      
  void    collect(Bool_t fitshape = kFALSE);
  Bool_t  reject();

  Int_t       runnumber;
  TString     runKind;
  Int_t       rfiles;
  TString     host;
  TString     rootTempFileName;
  TString     rootfName;
  TString     prdfName;
  TString     rootdirname;
  TString     cDirectory;         //  directory to store TCanvases for Run#
  list<rootfile>  roots;
  Int_t       pfiles;
  TString     prdfdirname;
  list<prdffile> prdfs;
  Int_t        eventseq;
  Int_t        eventsread;

  Int_t      * active;            //  list of active ADC channels  
  Float_t   ** adc; 
  Float_t    * pedestal;  
  Float_t    * rawPeak,    * rawTime;

  Double_t  ** fitResults;
  Float_t    * fitPeak,    * fitTime,    * fitInt,  * fitChi2;
  Float_t    * fitPeakRMS, * fitTimeRMS;

  Float_t    * calibPeak;    //  units of SiPM pixels

  Double_t  ** shapeD;

  TF1       ** shapes;
  TF1       ** sigFit;
  TF1       ** rvsc, **fmsc;                       //  Single cell calibration
  TGraph    ** gpulse;                             //  Graph of the pulse in indivuidual channel
  TH1       ** ft, ** fm, ** fw, ** fint, ** fpd, **fchi2;
  TH1      *** fitPar;                             //  Actual fit parameters in every channel
  TH1       ** rpeak, ** rtm;
  TH2        * rdata;
 
  //  Event produced by separately summing up all detector data in High and Low gains
  Double_t   * evtadcsum[2];
  Double_t   * evtfitpar[2];
  Double_t   evtpedestal[2];
  Double_t   evttime[2];
  Double_t   evtpeak[2];
  Double_t   tChi2[2];               //  Chi2 from signal fit of the total ampl sum
  TF1        * evtShape[2], * evtSignal[2];
  TGraph     * evtGraph[2];
  //
  TFile   * fhcl;
  TTree   * thcl;

};

//   ====================================================================================

class hcalHelper{
protected:
  hcalHelper();
  static hcalHelper * single;
  virtual ~hcalHelper(){ ; }

public:
  static hcalHelper * getInstance(){if(!single) single = new hcalHelper(); return single;}
  static void         deleteInstance(){if(single) delete single;} 
  //  HCAL implementation for test beam

  hcal   * t1044;
  
  void     setCTriggerOn(Bool_t  ON = kTRUE);   //  program runs on cosmic data
  void     setCTriggerGRange(Int_t gain = 1);   //  Gain range used to trigger on cosmic
  void     setDisplayMode(Int_t mode = 3);      //  defult: show only summary
  void     setRunKind(TString rK = "beam"); 
  Int_t    evLoop(int run, int evToProcess=0, int fToProcess=1);
  Int_t    collectRaw();
  void     updateCalibration();
  void     hcalTrigger();
  void     hcalPattern(Int_t nx, Int_t ny, Int_t run, Int_t mod = 0);
  void     hcalImpact();
  void     fitHCalSignal();
  void     hcalDisplay();
  void     dofixes();
  Int_t    reject();

  Double_t     trThresholdBin;
  Double_t     twrThreshold;
  Double_t     stckSumThreshold;

  Int_t        runnumber;
  Int_t        eventseq;
  Int_t        eventsread;
  Int_t        displayMode;      //  amount and form of graphic data presented 
  Int_t        triggerOn;        //  if kFALSE - cosmic trigger is disabled  
  Int_t        triggerGain;      //  preamp range used for triggering if enabled
  Int_t        rejectRaw;
  Int_t        eventReject;
  Int_t        eventTrigger;
  Int_t        channels;
  Int_t        samples;
  Int_t        parameters;
  //  Energy sums from collectTileSummary()
  Double_t     uSum, cSum;
  Double_t     YF,  YU,   YC,   XU,    XC;
  Double_t     sumFU,      sumFC;
  Double_t     muxU, muxC, muxFU, muxFC;
  Int_t        muxUCh,     muxCCh,       muxUFiber,   muxCFiber;

  Int_t     ** adc;
  Int_t      * activeCh;
  //  processing run summaries
  TH2        * rdata;
};

//   ====================================================================================

  
class hcal{
  hcal();
  ~hcal();
  friend class hcalHelper;
public:
  //  currently number of stacks with reject codes
  Int_t     rejectEvent;   //  Event must be rejected 
  Bool_t    readyForEvent;
  Int_t     maxStacks;     //  may include scintillators between or upfront or what not
  Int_t     activeStacks;
  Int_t     activeCalStacks;
  Bool_t *  alive;
  Int_t  *  kinds;         //  for now is just StackId
  //  Stacks belong to hcal and towers belong to stacks 
  stack **  stacks;
  void      setCalibration();
  Int_t     getStackTiming();
  Int_t     update(Int_t displayMode = 3, Bool_t fitShapes=kFALSE);  //  returns reject 
  Int_t     collectTrPrimitives();
  void      displayRaw(Int_t mode = 0);
  void      displaySummary(Int_t mode = 0);
  void      clean();          //  clean all related objects for new event
  //  Summary
  Double_t  amplTotal;
  Double_t  eTotal;
  Double_t  calLCG;
  Double_t  scintLCG;
  TH1     * totalamp, * totale, * totallcg, * scintlcg; 
};

//   ====================================================================================
  
class stack{
  friend class hcal;
  friend class hcalHelper;

public:

  stack();

  stack(const Int_t kind, const Int_t chnls, const Int_t xch, const Int_t ych);
  ~stack();
  void  displayEvent(Int_t mode = 0);
  void  displayADCSum();
  void  displayTowerSummary(Int_t mode = 3);
  void  displayStackSummary(Int_t mode = 3);
  void  print();
  void  updateMap(Int_t stckloc);          //  Map Update in Towers is done Helper->HCal->Stack->Tower
  Int_t update(Bool_t fitShapes = kFALSE); //  EVent update. HCal, Stacks and Towers are updated from HCal
  Int_t getStackTime();
  Int_t getStackImpact(); 
  Int_t getStackTrack();//  returns reject code
  Int_t getTrigger();
  void  clean();          //  get ready for new event

  Int_t    reject;
  //   single energy scale calibration [GeV/ADC count [LG]) (copied into towers by setCalibration()). May became 
  //   different later

  Double_t stackECalib; 
  Int_t    triggerFlag;
  Int_t    triggerHits;
  Int_t    triggerSum;
  Int_t    eventsSeen;
  
  Int_t    stackId;      //   from enum )HINNER etc)
  Int_t    stackKind;    //   Stack kind (CALOR, COUNTER)
  Int_t    gains;        //   number of gain ranges
  Int_t    twrsInStack;  //   Maximum number of active towers in this stack
  Int_t    stackLoc;     //   first tower location in activeCh lookup table
  Int_t    nTwrsX;       //   Stack geometry in units of Towers
  Int_t    nTwrsY;       //   
  Int_t    key;          //   (stackId*10+gains)
  Bool_t   mapUpdated;
  Double_t totPed;
  Double_t totAmpl;      //   total sum of amplitudes in stack (inits LG)
  Double_t totCorAmpl;   //   fully corrected amplitude sum
  Double_t avTwrTime;
  Double_t rmsTwrTime;
  Double_t E;            //   Fully calibrated energy in Stack [GeV]
  Double_t xImpact;      //   x - Impact position in stack
  Double_t yImpact;      //   y - Impact position in stack
  tower ** towers;
 
  //  Event produced by separately summing up all detector data in High and Low gains
  Int_t        gainToUse;
  Int_t        twrsUsed[2];
  Double_t   * adcsum[2];
  Double_t   * fitPar[2];
  Double_t     fitPed[2];
  //  fit results for a total ADC sums (HIGH and LOW gains) in stack (exclude overflow towers)
  Double_t     fitTime[2];
  Double_t     fitPeak[2];
  Double_t     fitChi2[2];               //  Chi2 from signal fit of the total ampl sum
  TF1        * shape[2], * signal[2];
  TGraph     * graph[2];
  //  Muon Track in the stack
  //  counted towers with less then 3 samples in sat. region and above TWRAMPTHR
  Int_t        stackHits;
  Int_t        trackHits;
  Double_t     crossing, slope, trchi2, trackAmpl;
  //  Summaries
  TH1        * stFGR;
  TH1        * stFPed, * stFAmpl, * stFTime, * stChi2;
  TH1        * stSPed, * stSAmpl, * stSE,    * stAvT,  * stTRMS;

  TH2        * hitCG;
  //  cosmics summary
  TH1        * trAmp,  * trChi2,  * trCr,    * trSl;
  TH2        * trAmpCh2, * trAstH,* trASl,   * trATwr;

  //  saturation study
  TH1       *  satProb[2];  //  High and Low gains
   
};

//   ====================================================================================
    //  towers are created and updated by evloop in hcalHelper as readout objects, 
    //  converted into physics entities by stacks when added to stacks but still - 
    //  remain property of hcalHelper which updates them when new event is coming
  
class tower{
  friend class hcalHelper;
public:
  tower(){;}
  ~tower(){;}
  tower(Int_t stack, Int_t stkLoc, Int_t xSt, Int_t ySt);
  void  setChannel(Int_t channel, Int_t gain);
  Int_t update(Bool_t fitShape=kFALSE);  //   channel energies, times and gain selection in new event
  void  clean();           //   clean tower (we keep whole configuration stable, just update data)
  void  print();
  void  display();

  Int_t    reject;        //   for now just a signal of overflow in both (or single) ranges  
  Double_t twrECalib;     //   calibration coefficient (set by hcal->setCalibration()). Common to stack
  Double_t twrEScale;     //   equalization coefficient
  Int_t    stackId;       //   stack which owns this tower
  Int_t    index;         //   index in the stack y*HCALCOLUMNS + x  
  Int_t    key;           //   ((stackId*1000+index)*10)+gainToUse
  Int_t    x;
  Int_t    y;
  Int_t    gains;
  Int_t    gainToUse;
  Int_t    adcChannel[2]; //   location in activeCh[] and adc[][] storage areas (-stackLoc)
  Int_t    satFlag[2];
  Double_t rawPed[2];     //   2048 (PEDESTAL) subtructed
  Double_t rawAmpl[2], rawTime[2];
  Bool_t   attached;      //   attached to muon track when calibrating

  Double_t rped, rampl, rtime;   //  best raw values for amplitude and time (units LG)
  Double_t cped, campl, ctime;   //  mixed & corrected peds&Amplitude (in units of LG) and time
  Double_t E;                    //  fully calibrated energy in Tower [GeV]
  TF1        * shape[2], * signal[2];
  TGraph     * graph[2];
  // data accumulated since beginning of processing run
  TH1        * twrPed, * twrAmpl, * twrTime, * twrE;

  // Double_t     fitAmpl[2], fitTime[2];


};

//   ====================================================================================

class hcalTree
{

protected:
  hcalTree();
  virtual ~hcalTree(){;}
  static   hcalTree * single;


public:
  static hcalTree * getInstance(){if(!single) single = new hcalTree(); return single;}
  static void       deleteInstance(){if(single) delete single;} 


  //  switching output file if so required:   TBD

  void updateRootFile();
 
  int runnumber;
  int eventseq;
  int eventsread;
  int channels;
  int samples;

  //   1      overflows in calorimeter amplitudes
  //   10     tags outside range
  //   100    HCal time is unreliable
  //   1000   
  //   10000  Total energy in event is consistent with ZERO
  // 
  int rejectCode;

};


//   ====================================================================================
  
class tileHelper{
protected:
  tileHelper();
  static tileHelper * single;
  virtual ~tileHelper(){
    delete [] tileCalib;
    delete [] tileCalPeak;
  }


public:
  static tileHelper * getInstance(){if(!single) single = new tileHelper(); return single;}
  static void         deleteInstance(){if(single) delete single;} 

  // **************************************************************************
  //  event data for a smartTile

  struct eventtilesummary{
    eventtilesummary(){
      evtGraph  = new TGraph(NSAMPLES);
      TString fitName  = "det adc sum";
      evtShape  = new TF1(fitName, &signalShape, 0., (Double_t)NSAMPLES, NPARAMETERS);
      fitName   = "det signal";
      evtSignal = new TF1(fitName, &signalShape, 0., (Double_t)NSAMPLES, NPARAMETERS);
    }
    Double_t    times[TILECHANNELS];
    Double_t    chi2[TILECHANNELS];
    Double_t    evtadcsum[NSAMPLES];
    Double_t    evtfitpar[NPARAMETERS];
    Double_t    evtpedestal;
    Double_t    evttime;
    Double_t    evtpeak;
    Double_t    tChi2;                   //  Chi2 from signal fit of the tile total amplitude
    Double_t    evtpc;                   //  event total pixel count (sum of calibrated counts in individual tile channels)
    Int_t       hitfiber;                //  fiber with highest signal
    Double_t    hitfiberpc;              //  pixel count in the hit fiber
    Double_t    hitfibertd;              //  time delta between hit fiber ends
    Double_t    yChi2;                   //  CHi2 from fitting Y coordinate in the tile
    Double_t    ySigma;
    TGraph    * evtGraph;
    TF1       * evtShape;
    TF1       * evtSignal;
    Int_t       rejectCode;

    void reset(){
      for (int is  = 0; is  < NSAMPLES;     is++)  evtadcsum[is] = 0.;
      for (int tch = 0; tch < TILECHANNELS; tch++) times[tch]    = 0.;
      evttime    = 0.;
      rejectCode = 0;
    }  
  };

  // **************************************************************************
  //  Run data for a smartTile

  struct runtilesummary{
    runtilesummary();

    // --------------------------------------------------------------------------
    //  Tile data (when calibrated - in units of fired cells)
    TH2     * rmax;     //  [TILECHANNELS][2050]; //  maximum uncalibrated amplitudes
    TH2     * rfsum;    //  [TILEFIBERS][2050];   //  raw fiber sum (only max fiber)
    TH2     * rtsum;    //  [TILEFIBERS][2050];   //  raw total sum (only max fiber)
    TH2     * rfasym;   //  [TILEFIBERS][-1:+1]   //  raw asymmetry per fiber
    TH1     * rtasym;   //  [-1 : +1]  right-left asymmetry  (using uncalibrated sums}
    TH1     * rx;       //  uncalibrated X
    TH1     * ry;       //  uncalibrated Y 
    TH2     * rimp;
    TH2     * cdata;    //  [TILECHANNELS][2000];  //  all calibrated data for further unalisis
    TH2     * cmax;     //  [TILECHANNELS][2000];  //  maximum calibrated amplitudes
    TH2     * cfsum;    //                         //  calobrated fiber sum
    TH2     * ctsum;    //  [TILECHANNELS][2000];  //  calobrated total sum
    TH2     * cfasym;   //  [TILEFIBERS][-1.-1.]   //  calibrated asymmetry per fiber
    TH1     * ctasym;   //  [-1 : +1]  right-left asymmetry (using calibrated sums)
    TH1     * cx;       //  calibrated X
    TH1     * cy;       //  calibrated Y 
    TH1     * fy;       //  fitted Y 
    TH2     * cimp;
    TH2     * cx_pc;    //  calibrated total asymmetry (X)  vs total pixel count
    TH2     * cy_pc;    //  calibrated weighted Y  vs total pixel count
    TH2     * fy_pc;    //  fitted Y  vs total pixel count
    TH2     * cimpW;    //  Weighted (by total pixel count) impact points 

    //  Trigger related
    TH1     * trhits;   //  Raw total sum with binning similar to "hits_tpc"
    TH2     * hits_tpc; //  Number of triggerable hits (above threshold) vs total pixel count

    // --------------------------------------------------------------------------
    //   accumulated in the processing phase and must be written to .root file
    TH3     * treff;    //  [HITMULTTHRESHOLDS][CHANNELTHRESHOLDS][0, 200];    //  total pixel count (histogramm) vs cuts on total number of hits and hit amplitude in individual channels
    TH3     * pc_imp;   //  total pixel count (histogramm) vs impact position
    TH2     * pc_pat;   //  fitted positions of maxiuma in pc_imp pixel distributions
    TH3     * pc_fimp;  //  total pixel count (histogramm) vs impact position (fitted Y)
    TH2     * pc_fpat;  //  fitted positions of maxiuma in pc_imp pixel distributions (fitted Y)

    // --------------------------------------------------------------------------
    //   debugging muon impact computations
    TH2        * XvsSl,    * XY;                 //  X value vs assumption about slope in the X vs Asymmetry 
    TH1        * Y0, * Y1, * Y2;        //  Parameters from the tile-Y fit
    TH2        * y_yChi2,  * s_y_Chi2;  // Y in the tile vs Chi2 of the Y fit and Tile signal Chi2 vs Chi2 of Y fit
    TH2        * y_ys,     * yc_rcode;
    TH1        * yCleaned, * yKept;
    TH1        * trcode;

    // --------------------------------------------------------------------------
    //   attempts on timing resolution in tiles and towers
    TH2        * td_r_l;            //  Time difference R/L vs total pixel count in the fiber
    TH3        * td_t_tw;           //  Time difference tile to tower vs tile pc and tower ampl

    
  };

  // **************************************************************************

  
  Int_t    evLoop(Int_t run, Int_t evToProcess, Int_t fToProcess);
  void     status();
  void     updateMap();
  void     updateCalibration();
  void     evreset();                 //  reset last event data
  void     tileTrigger();
  void     tileTiming();
  void     tileDisplay();
  void     tileTriggerDisplay();
  void     tilePattern(Int_t nx, Int_t ny, Int_t run, Int_t mod = 0);
  void     collectTileSummary();
  Double_t getYFit();
  void     tileImpact();
  void     fitTileSignal();
  Int_t    reject();
  eventtilesummary evtsum;
  runtilesummary   runsum;
  
  Int_t        channels;
  Int_t        samples;
  Int_t        fibers;
  Int_t        xdivisions,  ydivisions;
  Int_t        parameters;
  //  Energy sums from collectTileSummary()
  Double_t     uSum, cSum, luSum, lcSum, ruSum, rcSum;
  Double_t     YF,  YU,   YC,   XU,    XC;
  Double_t     sumFU,      sumFC;
  Double_t     muxU, muxC, muxFU, muxFC;
  int          muxUCh,     muxCCh,       muxUFiber,   muxCFiber;
  Double_t     ras,  cas,  rtas,  ctas;
  Double_t   * fiberY;
  Double_t   * fiberLY;         //  Calibrated Light Yields from individual fibers
  Double_t   * tileCalib;       //  Calibration coefficients for SiPM's currently in use (earlier measurements)
  Double_t   * tileCalPeak;     //  Fitted peak values scaled by tileCalib  (tile channels)
  int          hitcnt[CHANNELTHRESHOLDS]; 

  TCanvas    * fiberDisplay;
  TCanvas    * triggerDisplay;
  TCanvas    * impactDisplay;
  TCanvas    * lyPattern,  * lyFits;
  TF1        * yFit;
  TGraph     * fLY;

};

//   ====================================================================================


class tileLightYield{
  tileLightYield();
  void  setrootfile(rootfile & rf){
    //    hcalHelper * hHelper = hcalHelper ::getInstance();
    rfile = new rootfile(rf);
  }
  void setdivisions(Int_t nx, Int_t ny){
    xdivisions  = nx;
    ydivisions  = ny;
    lightyield  = new Double_t * [ny];
    for(int iy =0; iy<ny; iy++) lightyield[iy] = new Double_t [nx];
      
  }
  rootfile   * rfile;
  Int_t        xdivisions, ydivisions;
  Double_t  ** lightyield;
};

//   ====================================================================================
class tileTree
{

protected:
  tileTree();
  virtual ~tileTree(){;}
  static   tileTree * single;


public:
  static tileTree * getInstance(){if(!single) single = new tileTree(); return single;}
  static void       deleteInstance(){if(single) delete single;} 


  //  switching output file if so required:   TBD

  void updateRootFile();
 

  //   1      overflows in calorimeter amplitudes
  //   10     tags outside range
  //   100    HCal time is unreliable
  //   1000   
  //   10000  Total energy in event is consistent with ZERO
  // 
  int rejectCode;
};

//   ====================================================================================

class hcalUtil{
protected:
  hcalUtil ();
  virtual ~hcalUtil(){;}
  static  hcalUtil * single;

public:
  static hcalUtil * getInstance(){if(!single) single=new hcalUtil(); return single;}
  static void       deleteInstance(){if(single) delete single;} 

  Float_t xPeak(TF1* f, Float_t x1, Float_t x2);

  //  Float_t xPeak(TF1* f, Float_t x1, Float_t x2);
  Float_t width(TF1* f, Float_t x1, Float_t x2, Float_t peak);

  bool  shapeFit(TH1D * proj, Double_t * pE, Float_t & maxE, Double_t * pG, bool fine, bool MIP);

  bool  emcFit(TH1D * proj,  bool fine, bool sing, Double_t * pE, Float_t & maxE, Double_t * pG, bool MIP);
  bool  emcFit(TH1D * proj,  bool sing, Double_t * pE, Float_t & maxE, Double_t * pG);
  bool  emcFit(TH1D * proj,  bool sing, Double_t * pE, Float_t & maxE, Double_t * pG, Float_t minx, Float_t maxx);
  bool  twrMipFit(TH1D * proj,  Double_t * pE, Float_t & maxE, Double_t * pG);
  bool  twrTimeFit(TH1D * proj,  Double_t * pE, Float_t & maxE, Double_t * pG);

};

// hcalUtil    * hcalUtil    :: single = 0;
// hLabHelper  * hLabHelper  :: single = 0;
// hcalHelper  * hcalHelper  :: single = 0;
// hcalTree    * hcalTree    :: single = 0;
// tileHelper  * tileHelper  :: single = 0;
// tileTree    * tileTree    :: single = 0;


//#endif   //  CINT
  
#endif   //  HCALUTIL_H_

