#ifndef __ATRACE_H__
#define __ATRACE_H__

//
//  Hello ATrace Fans,
//       
//      This class will represent a trace from the (ADC) Oscilloscope. 
//  In the initial implementation, it will hold the timebins of the ADC.
//  It will have the methods of finding its maximum (averaged over +/- n
//  channels). 
//                                                      ~TKH and MAB

#include <vector>

class TF1;
class TH1D;

class ATrace
{
 public:
  
  ATrace();
  ~ATrace();

  std::vector<double> voltage;

  double FindMaximum(int n);  
  int    FindMaximumMiddle(int n);  
  void   FitLeftEdge();
  double PulseHeight(){return height;}
  double PulseTime(){return time;}
  double GetW() {return W;}
  double GetdW() {return dW;}
  int    NAboveThreshold( double thrup, double thrdown );

  double PulseWidth( double thrup, double thrdown );

  void MakeTrace(int sign=-1);

  static bool FastFits;
  static double SingleEmean;
  static double SingleEsigma;
  static double DoubleEmean;
  static double DoubleEsigma;


 protected:
  
  static int numtraces;
  int mytrace;

  double height;
  double time;
  double W;
  double dW;
  TH1D* trace;
  TF1* fermi;

  char fcn_name[500];
  char hst_name[500];
  
};

#endif /* __ATRACE_H__ */
