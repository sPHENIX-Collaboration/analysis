#ifndef __HCAL_H__
#define __HCAL_H__
#include <TString.h>
#include <Event/fileEventiterator.h>
#include <Event/Event.h>
#include <map>
#include <vector>
class TFile;
class TTree;
class TString;
class TGraph;
class TCanvas;
class TH1F;
class TH2F;
class TF1;

class hcal
{
 public:
   hcal();
   ~hcal(){;}
   void Initialize();
   int evLoop(int nevt);
   int GetNextEvent();
   int collect(int);
   void setPrdf(TString & fin)
     { prdfName = fin; } 
   void setPrdflist(TString &fin);
   void set_display(bool b)
     { display = b; }
   void Display();
   int Clear();

   void SetCableMap(int _nch, int *_index)
    { NCH = _nch;
      channel_index.assign( _index, _index+_nch+1); }

   TGraph* getSignal(int ch)
     { return gpulse[ch]; }
   static double SignalShape(double *x, double *par);
   static double SignalShapeNegative(double *x, double *par);
   void fitShape(int);
   void Save(char*);
   void set_positive_polarity(bool b)
     { polarity_positive = b; }
   void set_verbosity(bool b)
     { verbosity = b; }
   void set_plot_max(float max)
     { plot_max = max; }
   void set_plot_min(float min)
     { plot_min = min; }
   void FitDisplay();
  public:
   int runnumber;
   int nevents;
   TString prdfName;
   int NCH;
   int NSAMPLES;
   int PEDESTAL;
   bool display;
   Eventiterator *it;
   Event *evt;
   TGraph *gpulse[100]; //Signal
   std::vector<int> channel_index;
   TF1 *fits[100];
   bool fit_analysis;
   TH2F *h2_fit_shape[100];
   TH1F *h_PulsePeaks[100];
   TH1F *h_PulseInt[100];
   TH2F *h2_Pulse_H_Int[100];
   TH2F *h_hilo[100];
   TH1F *h_gain[100];
   TH1F *h_tres[100];
   TH1F *h_ped_res[100];
   TH1F *h_saturation_rates;
   TH1F *h_info;
   TGraph *rate;
   TCanvas *c;
   float plot_max;
   float plot_min;
   TFile *fout;
   bool polarity_positive; //default yes
   bool verbosity;
   std::map<int,float> saturation_map;
};

#endif
