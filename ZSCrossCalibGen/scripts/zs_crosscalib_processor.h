#ifndef ZS_CROSSCALIB_PROCESSOR_H
#define ZS_CROSSCALIB_PROCESSOR_H

#include <calobase/TowerInfoDefs.h>
#include <cdbobjects/CDBTTree.h>
#include <TFile.h>
#include <TProfile2D.h>

#include <cmath>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

class ZSCrossCalibWorker
{
 public:
  std::string m_detector;      // "CEMC", "HCALIN", "HCALOUT"
  std::string m_hist_det;      // "cemc", "ihcal", "ohcal"
  int m_neta{0};
  int m_nphi{0};
  std::vector<unsigned int> m_keys;
  std::vector<float> m_scale_main;
  std::vector<float> m_scale_minus5;
  std::vector<float> m_scale_plus5;
  int m_zero_ref_count{0};
  int m_total_channels{0};
  int m_n_fallback_minus5{-1};
  int m_n_fallback_plus5{-1};

  ZSCrossCalibWorker(const std::string &detector)
    : m_detector(detector)
  {
    if (m_detector == "CEMC")
    {
      m_hist_det = "cemc";
      m_neta = 96;
      m_nphi = 256;
    }
    else if (m_detector == "HCALIN")
    {
      m_hist_det = "ihcal";
      m_neta = 24;
      m_nphi = 64;
    }
    else if (m_detector == "HCALOUT")
    {
      m_hist_det = "ohcal";
      m_neta = 24;
      m_nphi = 64;
    }
    else
    {
      std::cerr << "Unknown detector: " << m_detector << std::endl;
      return;
    }

    m_total_channels = m_neta * m_nphi;
    m_keys.resize(m_total_channels);
    m_scale_main.resize(m_total_channels, 0.0f);
    m_scale_minus5.resize(m_total_channels, 0.0f);
    m_scale_plus5.resize(m_total_channels, 0.0f);

    size_t idx = 0;
    for (int ie = 0; ie < m_neta; ++ie)
    {
      for (int ip = 0; ip < m_nphi; ++ip)
      {
        if (m_detector == "CEMC")
        {
          m_keys[idx] = TowerInfoDefs::encode_emcal(ie, ip);
        }
        else
        {
          m_keys[idx] = TowerInfoDefs::encode_hcal(ie, ip);
        }
        ++idx;
      }
    }
  }

  bool initCandidate(const std::string &hist_file, const std::string &ref_cdb_file)
  {
    m_zero_ref_count = 0;

    TFile *f_hist = TFile::Open(hist_file.c_str(), "READ");
    if (!f_hist || f_hist->IsZombie())
    {
      std::cerr << "Error opening hist file: " << hist_file << std::endl;
      if (f_hist) delete f_hist;
      return false;
    }

    std::string name_main = "h_CaloFittingQA_" + m_hist_det + "_etaphi_ZScrosscalib_main";
    std::string name_minus5 = "h_CaloFittingQA_" + m_hist_det + "_etaphi_ZScrosscalib_minus5";
    std::string name_plus5 = "h_CaloFittingQA_" + m_hist_det + "_etaphi_ZScrosscalib_plus5";

    TProfile2D *h_main = dynamic_cast<TProfile2D *>(f_hist->Get(name_main.c_str()));
    TProfile2D *h_minus5 = dynamic_cast<TProfile2D *>(f_hist->Get(name_minus5.c_str()));
    TProfile2D *h_plus5 = dynamic_cast<TProfile2D *>(f_hist->Get(name_plus5.c_str()));

    if (!h_main || !h_minus5 || !h_plus5)
    {
      std::cerr << "Missing one or more histograms in " << hist_file << ":\n"
                << "  - " << name_main << ": " << (h_main ? "found" : "MISSING") << "\n"
                << "  - " << name_minus5 << ": " << (h_minus5 ? "found" : "MISSING") << "\n"
                << "  - " << name_plus5 << ": " << (h_plus5 ? "found" : "MISSING") << std::endl;
      delete f_hist;
      return false;
    }

    CDBTTree ref_cdb(ref_cdb_file);
    ref_cdb.LoadCalibrations();

    size_t idx = 0;
    for (int ie = 0; ie < m_neta; ++ie)
    {
      for (int ip = 0; ip < m_nphi; ++ip)
      {
        unsigned int key = m_keys[idx];
        float ref_ratio = ref_cdb.GetFloatValue(key, "ratio");

        float val_main = h_main->GetBinContent(ie + 1, ip + 1);
        float val_minus5 = h_minus5->GetBinContent(ie + 1, ip + 1);
        float val_plus5 = h_plus5->GetBinContent(ie + 1, ip + 1);

        if (ref_ratio > 0.0f)
        {
          m_scale_main[idx] = val_main / ref_ratio;
          m_scale_minus5[idx] = val_minus5 / ref_ratio;
          m_scale_plus5[idx] = val_plus5 / ref_ratio;
        }
        else
        {
          m_scale_main[idx] = 0.0f;
          m_scale_minus5[idx] = 0.0f;
          m_scale_plus5[idx] = 0.0f;
          ++m_zero_ref_count;
        }
        ++idx;
      }
    }

    delete f_hist;
    return true;
  }

  bool processRun(unsigned int /*run*/, const std::string &target_cdb_file, const std::string &out_file)
  {
    std::filesystem::path p(out_file);
    if (p.has_parent_path())
    {
      std::error_code ec;
      std::filesystem::create_directories(p.parent_path(), ec);
      if (ec)
      {
        std::cerr << "Failed to create directory " << p.parent_path() << ": " << ec.message() << std::endl;
        return false;
      }
    }

    CDBTTree in_cdb(target_cdb_file);
    in_cdb.LoadCalibrations();

    CDBTTree out_cdb(out_file);
    for (size_t i = 0; i < m_keys.size(); ++i)
    {
      unsigned int key = m_keys[i];
      float orig_ratio = in_cdb.GetFloatValue(key, "ratio");

      float r_main = m_scale_main[i] * orig_ratio;
      float r_minus5 = m_scale_minus5[i] * orig_ratio;
      float r_plus5 = m_scale_plus5[i] * orig_ratio;

      out_cdb.SetFloatValue(key, "ratio", orig_ratio);
      out_cdb.SetFloatValue(key, "ratio_main", r_main);
      out_cdb.SetFloatValue(key, "ratio_minus5", r_minus5);
      out_cdb.SetFloatValue(key, "ratio_plus5", r_plus5);
    }

    out_cdb.Commit();
    out_cdb.WriteCDBTTree();
    return true;
  }

  // Same as processRun, but the ±5 ns scale per tower is taken from the given vectors (e.g. a raw-time fit
  // prediction). Towers whose override is not finite or <= 0 fall back to the candidate scale.
  // Fallback tower counts per window are available afterwards via getFallbackMinus5 / getFallbackPlus5 (-1 on failure).
  bool processRunWithOverride(unsigned int run, const std::string &target_cdb_file, const std::string &out_file,
                              const std::vector<float> &scale_minus5, const std::vector<float> &scale_plus5)
  {
    m_n_fallback_minus5 = -1;
    m_n_fallback_plus5 = -1;
    if (scale_minus5.size() != m_keys.size() || scale_plus5.size() != m_keys.size())
    {
      std::cerr << "Override scale size mismatch for run " << run << ": expected " << m_keys.size()
                << ", got " << scale_minus5.size() << " / " << scale_plus5.size() << std::endl;
      return false;
    }

    std::filesystem::path p(out_file);
    if (p.has_parent_path())
    {
      std::error_code ec;
      std::filesystem::create_directories(p.parent_path(), ec);
      if (ec)
      {
        std::cerr << "Failed to create directory " << p.parent_path() << ": " << ec.message() << std::endl;
        return false;
      }
    }

    CDBTTree in_cdb(target_cdb_file);
    in_cdb.LoadCalibrations();

    m_n_fallback_minus5 = 0;
    m_n_fallback_plus5 = 0;
    CDBTTree out_cdb(out_file);
    for (size_t i = 0; i < m_keys.size(); ++i)
    {
      unsigned int key = m_keys[i];
      float orig_ratio = in_cdb.GetFloatValue(key, "ratio");

      bool use_m5 = std::isfinite(scale_minus5[i]) && scale_minus5[i] > 0.0f;
      bool use_p5 = std::isfinite(scale_plus5[i]) && scale_plus5[i] > 0.0f;
      if (!use_m5) ++m_n_fallback_minus5;
      if (!use_p5) ++m_n_fallback_plus5;

      float r_main = m_scale_main[i] * orig_ratio;
      float r_minus5 = (use_m5 ? scale_minus5[i] : m_scale_minus5[i]) * orig_ratio;
      float r_plus5 = (use_p5 ? scale_plus5[i] : m_scale_plus5[i]) * orig_ratio;

      out_cdb.SetFloatValue(key, "ratio", orig_ratio);
      out_cdb.SetFloatValue(key, "ratio_main", r_main);
      out_cdb.SetFloatValue(key, "ratio_minus5", r_minus5);
      out_cdb.SetFloatValue(key, "ratio_plus5", r_plus5);
    }

    out_cdb.Commit();
    out_cdb.WriteCDBTTree();
    return true;
  }

  // Candidate scale (H_win / ref ratio) per tower in eta-major order; 0 where the reference ratio is 0.
  const std::vector<float> &getScale(const std::string &window) const
  {
    if (window == "minus5") return m_scale_minus5;
    if (window == "plus5") return m_scale_plus5;
    return m_scale_main;
  }

  int getFallbackMinus5() const { return m_n_fallback_minus5; }
  int getFallbackPlus5() const { return m_n_fallback_plus5; }

  int getZeroRefCount() const { return m_zero_ref_count; }
  int getTotalChannels() const { return m_total_channels; }
};

#endif
