#ifndef __Proto2ShowerCalib_H__
#define __Proto2ShowerCalib_H__

#include <fun4all/SubsysReco.h>
#include <TNtuple.h>
#include <TFile.h>
#include <string>
#include <stdint.h>
#include <fstream>

class PHCompositeNode;
class PHG4HitContainer;
class Fun4AllHistoManager;
class TH1F;
class TTree;
class SvtxEvalStack;
class PHG4Particle;
class RawTowerGeom;
class RawTowerContainer;
class SvtxTrack;

/// \class Proto2ShowerCalib
class Proto2ShowerCalib : public SubsysReco
{

public:

  Proto2ShowerCalib(const std::string &filename = "cemc_ana.root");
  virtual
  ~Proto2ShowerCalib();

  int
  Init(PHCompositeNode *topNode);
  int
  InitRun(PHCompositeNode *topNode);
  int
  process_event(PHCompositeNode *topNode);
  int
  End(PHCompositeNode *topNode);

private:

  Fun4AllHistoManager *
  get_HistoManager();

  std::string _filename;

  unsigned long _ievent;

  class TemperatureCorrection
  {
  public:

    static
    double
    Apply(const double Amplitude, const double T)
    {
      //! Change of gain VS T
      // https://indico.bnl.gov/conferenceDisplay.py?confId=2293 : Joey Smiga
      static const double Slope = -.589/16.76; // m / b
      static const double T0 = 28.86; // reference temperature

      return Amplitude / (1. + Slope * (T - T0));
    }

  };

  fstream fdata;
};

#endif // __Proto2ShowerCalib_H__
