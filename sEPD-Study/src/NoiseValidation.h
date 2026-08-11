#ifndef NOISEVALIDATION_H
#define NOISEVALIDATION_H

#include <fun4all/SubsysReco.h>

#include <string>
#include <memory>
#include <limits>

class PHCompositeNode;
class TH1;
class TH2;
class TriggerAnalyzer;

class NoiseValidation : public SubsysReco
{
 public:
  explicit NoiseValidation(const std::string &name = "NoiseValidation");
  ~NoiseValidation() override;

  // -- Rule of Five: explicitly delete copy and move semantics --
  NoiseValidation(const NoiseValidation &) = delete;                 // Delete copy constructor
  NoiseValidation &operator=(const NoiseValidation &) = delete;      // Delete copy assignment
  NoiseValidation(NoiseValidation &&) = delete;                      // Delete move constructor
  NoiseValidation &operator=(NoiseValidation &&) = delete;           // Delete move assignment

  int Init(PHCompositeNode *topNode) override;
  int process_event(PHCompositeNode *topNode) override;
  int ResetEvent(PHCompositeNode *topNode) override;
  int End(PHCompositeNode *topNode) override;

  void set_adc_bins(int bins, double min, double max)
  {
    m_adc_bins = bins;
    m_adc_min = min;
    m_adc_max = max;
  }

  void set_trigger_bit(int trig) { m_trig_bit = trig; }

 private:
  std::unique_ptr<TriggerAnalyzer> m_triggerAnalyzer;

  // Event counters
  int m_total_events{0};
  int m_clock_trig_events{0};

  // Detector channel configuration
  const int m_nchannels_hcal{1536};
  const int m_nchannels_sepd{744};

  // Trigger configuration
  int m_trig_bit{0};

  // Histogram binning configuration
  int m_adc_bins{400};
  double m_adc_min{-200.0};
  double m_adc_max{200.0};

  // Histograms
  TH1 *hEvent{nullptr};
  TH2 *h2_ihcal_noise{nullptr};
  TH2 *h2_ohcal_noise{nullptr};
  TH2 *h2_sepd_noise{nullptr};

  // Min and max ADC for each detector
  double m_ihcal_min_adc{std::numeric_limits<double>::max()};
  double m_ihcal_max_adc{std::numeric_limits<double>::lowest()};

  double m_ohcal_min_adc{std::numeric_limits<double>::max()};
  double m_ohcal_max_adc{std::numeric_limits<double>::lowest()};

  double m_sepd_min_adc{std::numeric_limits<double>::max()};
  double m_sepd_max_adc{std::numeric_limits<double>::lowest()};
};

#endif // NOISEVALIDATION_H
