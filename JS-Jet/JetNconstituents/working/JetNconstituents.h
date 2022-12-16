// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef JETNCONSTITUENTS_H
#define JETNCONSTITUENTS_H

#include <fun4all/SubsysReco.h>
#include <g4jets/Jetv1.h>
#include <TH2D.h>
#include <TH1D.h>

#include <iostream>
#include <string>
#include <vector>

class TH1;
class TH2;
class PHCompositeNode;

class JetNconstituents : public SubsysReco
{
 public:

  JetNconstituents(const std::string &recojetnameR02 = "AntiKt_Tower_r02",
   const std::string &recojetnameR04 ="AntiKt_Tower_r04",
   const std::string &recojetnameR06 = "AntiKt_Tower_r06",
   const std::string &outputfilename = "JetNconstituents.root");

  ~JetNconstituents() override;

    void setEtaRange(double low, double high)
    {
        m_etaRange.first = low;
        m_etaRange.second = high;
    }
    void setPtRange(double low, double high)
    {
        m_ptRange.first = low;
        m_ptRange.second = high;
    }
  /** Called during initialization.
      Typically this is where you can book histograms, and e.g.
      register them to Fun4AllServer (so they can be output to file
      using Fun4AllServer::dumpHistos() method).
   */
  int Init(PHCompositeNode *topNode) override;

  /** Called for first event when run number is known.
      Typically this is where you may want to fetch data from
      database, because you know the run number. A place
      to book histograms which have to know the run number.
   */
  int InitRun(PHCompositeNode *topNode) override;

  /** Called for each event.
      This is where you do the real work.
   */
  int process_event(PHCompositeNode *topNode) override;

  /// Clean up internals after each event.
  int ResetEvent(PHCompositeNode *topNode) override;

  /// Called at the end of each run.
  int EndRun(const int runnumber) override;

  /// Called at the end of all processing.
  int End(PHCompositeNode *topNode) override;

  /// Reset
  int Reset(PHCompositeNode * /*topNode*/) override;

  void Print(const std::string &what = "ALL") const override;

 private:
    std::string m_recojetnameR02;
    std::string m_recojetnameR04;
    std::string m_recojetnameR06;
    std::string m_outputFileName;
    std::pair<double, double> m_etaRange;
    std::pair<double, double> m_ptRange;
    //! Output Histos variables
    // 2d nConstituent histograms
    TH2 * h2d_nConstituent_vs_R = nullptr;
    TH1 * h1d_nConstituent_R02 = nullptr;
    TH1 * h1d_nConstituent_R04 = nullptr;
    TH1 * h1d_nConstituent_R06 = nullptr;
    // 2d fraction histograms
    TH2 * h2d_FracEnergy_vs_CaloLayer_R02 = nullptr;
    TH2 * h2d_FracEnergy_vs_CaloLayer_R04 = nullptr;
    TH2 * h2d_FracEnergy_vs_CaloLayer_R06 = nullptr;
    // 1d fraction histograms
    TH1 * h1d_FracEnergy_EMCal_R02 = nullptr;
    TH1 * h1d_FracEnergy_EMCal_R04 = nullptr;
    TH1 * h1d_FracEnergy_EMCal_R06 = nullptr;
    TH1 * h1d_FracEnergy_IHCal_R02 = nullptr;
    TH1 * h1d_FracEnergy_IHCal_R04 = nullptr;
    TH1 * h1d_FracEnergy_IHCal_R06 = nullptr;
    TH1 * h1d_FracEnergy_OHCal_R02 = nullptr;
    TH1 * h1d_FracEnergy_OHCal_R04 = nullptr;
    TH1 * h1d_FracEnergy_OHCal_R06 = nullptr;
    // 1d nConstituents histograms
    TH1 * h1d_nConstituents_EMCal_R02 = nullptr;
    TH1 * h1d_nConstituents_EMCal_R04 = nullptr;
    TH1 * h1d_nConstituents_EMCal_R06 = nullptr;
    TH1 * h1d_nConstituents_IHCal_R02 = nullptr;
    TH1 * h1d_nConstituents_IHCal_R04 = nullptr;
    TH1 * h1d_nConstituents_IHCal_R06 = nullptr;
    TH1 * h1d_nConstituents_OHCal_R02 = nullptr;
    TH1 * h1d_nConstituents_OHCal_R04 = nullptr;
    TH1 * h1d_nConstituents_OHCal_R06 = nullptr;


};

#endif // JETNCONSTITUENTS_H
