// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef CALIBRATEQVEC_H
#define CALIBRATEQVEC_H

#include <fun4all/SubsysReco.h>
#include <phparameter/PHParameters.h>

// -- ROOT
#include <TFile.h>
#include <TH1.h>

// -- c++
#include <array>
#include <memory>
#include <string>
#include <unordered_set>

class PHCompositeNode;

class CalibrateQVec : public SubsysReco
{
 public:
  explicit CalibrateQVec(const std::string &name = "CalibrateQVec");

  /** Called during initialization.
      Typically this is where you can book histograms, and e.g.
      register them to Fun4AllServer (so they can be output to file
      using Fun4AllServer::dumpHistos() method).
   */
  int Init(PHCompositeNode *topNode) override;

  /** Called for each event.
      This is where you do the real work.
   */
  int process_event(PHCompositeNode *topNode) override;

  /// Clean up internals after each event.
  int ResetEvent(PHCompositeNode *topNode) override;

  /// Called at the end of all processing.
  int End(PHCompositeNode *topNode) override;

  void set_input_QVecCalib(const std::string &input_QVecCalib)
  {
    m_input_QVecCalib = input_QVecCalib;
  }

 private:
  enum class Subdetector
  {
    S,
    N
  };

  enum class QComponent
  {
    X,
    Y
  };

  struct QVec
  {
    double x{0.0};
    double y{0.0};
  };

  struct CorrectionData
  {
    // Averages of Qx, Qy, Qx^2, Qy^2, Qxy
    QVec avg_Q{};
    double avg_Q_xx{0.0};
    double avg_Q_yy{0.0};
    double avg_Q_xy{0.0};

    // Correction matrix
    std::array<std::array<double, 2>, 2> X_matrix{};
  };

  static constexpr size_t m_bins_cent = 8;

  // Holds all correction data
  // key: [Cent][Subdetector]
  // Subdetectors {S,N} -> 2 elements
  std::array<std::array<CorrectionData, 2>, m_bins_cent> m_correction_data;

  // Minimum sEPD Channel Charge
  double m_sepd_charge_threshold{0.2};

  // sEPD Bad Channels
  std::unordered_set<int> m_bad_channels;

  // sEPD Q Vectors
  // key: [Subdetector]
  // Subdetectors {S,N} -> 2 elements
  std::array<QVec, 2> m_Q;
  std::array<QVec, 2> m_Q_raw;
  std::array<QVec, 2> m_Q_recentered;

  std::string m_input_QVecCalib{"none"};

  double m_cent{0.0};

  // hists
  std::unique_ptr<TH1> hCentrality{nullptr};

  int read_QVecCalib();
  void load_correction_data(TFile *file);
  void print_correction_data();

  int process_centrality(PHCompositeNode *topNode);
  int process_sEPD(PHCompositeNode *topNode);
  void correct_QVecs();

  PHParameters m_CalibQVecParams;
  PHCompositeNode *m_parNode{nullptr};
};

#endif  // CALIBRATEQVEC_H
