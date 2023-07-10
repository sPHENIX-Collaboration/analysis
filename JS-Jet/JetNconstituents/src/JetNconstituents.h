// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef JETNCONSTITUENTS_H
#define JETNCONSTITUENTS_H

#include <fun4all/SubsysReco.h>
#include <jetbase/Jetv1.h>
#include <TH2D.h>
#include <TH1D.h>

#include <string>
#include <vector>

class TH1;
class TH2;
class PHCompositeNode;


class JetNconstituents : public SubsysReco
{
 public:

  JetNconstituents(const std::string &recojetname = "AntiKt_Tower_r04",
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
    //! Input Node pointers
    std::string m_recoJetName;
    std::string m_outputFileName;
    std::pair<double, double> m_etaRange;
    std::pair<double, double> m_ptRange;
    //! Output Histos variables

    TH1 * h1d_nConsituents = nullptr;
    TH1 * h1d_nConsituents_IHCal = nullptr;
    TH1 * h1d_nConsituents_OHCal = nullptr;
    TH1 * h1d_nConsituents_EMCal = nullptr;
    TH1 * h1d_FracEnergy_EMCal = nullptr;
    TH1 * h1d_FracEnergy_IHCal = nullptr;
    TH1 * h1d_FracEnergy_OHCal = nullptr;
    

    TH2 * h2d_FracEnergy_vs_CaloLayer = nullptr;
    TH2 * h2d_nConstituent_vs_CaloLayer = nullptr;

};

#endif // JETNCONSTITUENTS_H
