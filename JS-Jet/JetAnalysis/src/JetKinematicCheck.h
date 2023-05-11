// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef JETKINEMATICCHECK_H
#define JETKINEMATICCHECK_H

#include <fun4all/SubsysReco.h>
#include <g4jets/Jetv1.h>
#include <g4eval/PHG4DstCompressReco.h>

#include <string>
#include <vector>

class PHCompositeNode;
class TH1;
class TH2;
class TH3;

class JetKinematicCheck : public SubsysReco
{

 public:

  JetKinematicCheck(const std::string &recojetnameR02 = "AntiKt_Tower_r02",
		    const std::string &recojetnameR04 = "AntiKt_Tower_r04",
		    const std::string &outputfilename = "JetKinematicCheck");

  ~JetKinematicCheck() override;

 // set eta range
  void setEtaRange(double low, double high)
  {
    m_etaRange.first = low;
    m_etaRange.second = high;
  }

 // set pt range
  void setPtRange(double low, double high)
  {
    m_ptRange.first = low;
    m_ptRange.second = high;
  }


  std::vector<std::string>GetCentLabels();
  std::vector<int>GetMarkerColors();

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

  std::string m_recoJetNameR02;
  std::string m_recoJetNameR04;
  std::string m_outputFileName;
  std::pair<double, double> m_etaRange;
  std::pair<double, double> m_ptRange;
  int cent_N;


  // eventwise quantities
  int m_nJet;
  int m_centrality;
  float m_impactparam;
  int ncent;

  // reconstructed jets
  std::vector<int> m_id;
  std::vector<int> m_nComponent;
  std::vector<float> m_eta;
  std::vector<float> m_phi;
  std::vector<float> m_pt;
  std::vector<float> m_radii;

  // output histograms
  TH2 *jet_spectra_r02 = nullptr;
  TH1 *jet_spectra_r02_proj[11] = {nullptr};
  TH2 *jet_spectra_r04 = nullptr;
  TH1 *jet_spectra_r04_proj[11] = {nullptr};
  TH3 *jet_eta_phi_r02 = nullptr;
  TH2 *jet_eta_phi_r02_proj[11] = {nullptr};
  TH3 *jet_eta_phi_r04 = nullptr;
  TH2 *jet_eta_phi_r04_proj[11] = {nullptr};


};

#endif // JETKINEMATICCHECK_H
