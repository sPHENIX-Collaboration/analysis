//____________________________________________________________________________..
//
// This is a template for a Fun4All SubsysReco module with all methods from the
// $OFFLINE_MAIN/include/fun4all/SubsysReco.h baseclass
// You do not have to implement all of them, you can just remove unused methods
// here and in CalibrateQVec.h.
//
// CalibrateQVec(const std::string &name = "CalibrateQVec")
// everything is keyed to CalibrateQVec, duplicate names do work but it makes
// e.g. finding culprits in logs difficult or getting a pointer to the module
// from the command line
//
// CalibrateQVec::~CalibrateQVec()
// this is called when the Fun4AllServer is deleted at the end of running. Be
// mindful what you delete - you do loose ownership of object you put on the node tree
//
// int CalibrateQVec::Init(PHCompositeNode *topNode)
// This method is called when the module is registered with the Fun4AllServer. You
// can create historgrams here or put objects on the node tree but be aware that
// modules which haven't been registered yet did not put antyhing on the node tree
//
// int CalibrateQVec::InitRun(PHCompositeNode *topNode)
// This method is called when the first event is read (or generated). At
// this point the run number is known (which is mainly interesting for raw data
// processing). Also all objects are on the node tree in case your module's action
// depends on what else is around. Last chance to put nodes under the DST Node
// We mix events during readback if branches are added after the first event
//
// int CalibrateQVec::process_event(PHCompositeNode *topNode)
// called for every event. Return codes trigger actions, you find them in
// $OFFLINE_MAIN/include/fun4all/Fun4AllReturnCodes.h
//   everything is good:
//     return Fun4AllReturnCodes::EVENT_OK
//   abort event reconstruction, clear everything and process next event:
//     return Fun4AllReturnCodes::ABORT_EVENT;
//   proceed but do not save this event in output (needs output manager setting):
//     return Fun4AllReturnCodes::DISCARD_EVENT;
//   abort processing:
//     return Fun4AllReturnCodes::ABORT_RUN
// all other integers will lead to an error and abort of processing
//
// int CalibrateQVec::ResetEvent(PHCompositeNode *topNode)
// If you have internal data structures (arrays, stl containers) which needs clearing
// after each event, this is the place to do that. The nodes under the DST node are cleared
// by the framework
//
// int CalibrateQVec::EndRun(const int runnumber)
// This method is called at the end of a run when an event from a new run is
// encountered. Useful when analyzing multiple runs (raw data). Also called at
// the end of processing (before the End() method)
//
// int CalibrateQVec::End(PHCompositeNode *topNode)
// This is called at the end of processing. It needs to be called by the macro
// by Fun4AllServer::End(), so do not forget this in your macro
//
// int CalibrateQVec::Reset(PHCompositeNode *topNode)
// not really used - it is called before the dtor is called
//
// void CalibrateQVec::Print(const std::string &what) const
// Called from the command line - useful to print information when you need it
//
//____________________________________________________________________________..

#include "CalibrateQVec.h"

#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/getClass.h>

#include <calobase/TowerInfo.h>
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfoDefs.h>

// -- sEPD
#include <epd/EpdGeom.h>

// -- Centrality
#include <centrality/CentralityInfo.h>

// -- ROOT
#include <TProfile.h>

#include <cmath>  // std::cos and std::sin
#include <format>
#include <stdexcept>

//____________________________________________________________________________..
CalibrateQVec::CalibrateQVec(const std::string& name)
  : SubsysReco(name)
  , m_CalibQVecParams(name)
{
  std::cout << "CalibrateQVec::CalibrateQVec(const std::string &name) Calling ctor" << std::endl;
}

//____________________________________________________________________________..
int CalibrateQVec::Init(PHCompositeNode* topNode)
{
  // Read QVecCalib
  int ret = read_QVecCalib();
  if (ret)
  {
    return ret;
  }

  print_correction_data();

  // Create Space on NodeTree to save Minimum Bias Params
  PHNodeIterator parIter(topNode);
  m_parNode = dynamic_cast<PHCompositeNode*>(parIter.findFirst("PHCompositeNode", "PAR"));
  if (!m_parNode)
  {
    std::cout << "No RUN node found; cannot create PHParameters. Aborting run!";
    return Fun4AllReturnCodes::ABORTRUN;
  }

  m_CalibQVecParams.SaveToNodeTree(m_parNode, "CalibQVec");

  // hists
  hCentrality = std::make_unique<TH1F>("hCentrality", "", m_bins_cent, -0.5, m_bins_cent*10-0.5);

  std::cout << "CalibrateQVec::Init(PHCompositeNode *topNode) Initializing" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

int CalibrateQVec::read_QVecCalib()
{
  TH1::AddDirectory(kFALSE);

  auto file = std::unique_ptr<TFile>(TFile::Open(m_input_QVecCalib.c_str()));

  // Check if the file was opened successfully.
  if (!file || file->IsZombie())
  {
    std::cout << std::format("Could not open file '{}'\n", m_input_QVecCalib);
    return Fun4AllReturnCodes::ABORTRUN;
  }

  auto* h_sEPD_Bad_Channels = dynamic_cast<TH1*>(file->Get("h_sEPD_Bad_Channels"));

  if (!h_sEPD_Bad_Channels)
  {
    std::cout << std::format("Could not find histogram 'h_sEPD_Bad_Channels' in file '{}'\n", m_input_QVecCalib);
    return Fun4AllReturnCodes::ABORTRUN;
  }

  std::cout << std::format("Bad sEPD Channels\n");
  std::map<std::string, int> ctr;
  // Load Bad Channels
  for (int channel = 0; channel < h_sEPD_Bad_Channels->GetNbinsX(); ++channel)
  {
    // sEPD Channel Status
    // 0: good
    // non-zero: bad

    int status = static_cast<int>(h_sEPD_Bad_Channels->GetBinContent(channel + 1));
    if (status)
    {
      std::string badType;
      if (status == 1)
      {
        badType = "Dead";
      }
      if (status == 2)
      {
        badType = "Hot";
      }
      if (status == 3)
      {
        badType = "Cold";
      }

      m_bad_channels.insert(channel);
      std::cout << std::format("Channel: {:3d}, Type: {}\n", channel, badType);

      ++ctr[badType];
    }
  }
  std::cout << std::format("Total Bad sEPD Channels: {}, Dead: {}, Hot: {}, Cold: {}\n", m_bad_channels.size(), ctr["Dead"], ctr["Hot"], ctr["Cold"]);
  std::cout << std::format("{:#<20}\n", "");

  try
  {
    load_correction_data(file.get());
  }
  catch (const std::exception& e)
  {
    std::cout << std::format("An exception occurred: {}", e.what()) << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

void CalibrateQVec::load_correction_data(TFile* file)
{
  const int n = 2;

  std::string S_x_avg_name = std::format("h_sEPD_Q_S_x_{}_avg", n);
  std::string S_y_avg_name = std::format("h_sEPD_Q_S_y_{}_avg", n);
  std::string N_x_avg_name = std::format("h_sEPD_Q_N_x_{}_avg", n);
  std::string N_y_avg_name = std::format("h_sEPD_Q_N_y_{}_avg", n);

  auto* h_sEPD_Q_S_x_avg = dynamic_cast<TProfile*>(file->Get(S_x_avg_name.c_str()));
  auto* h_sEPD_Q_S_y_avg = dynamic_cast<TProfile*>(file->Get(S_y_avg_name.c_str()));
  auto* h_sEPD_Q_N_x_avg = dynamic_cast<TProfile*>(file->Get(N_x_avg_name.c_str()));
  auto* h_sEPD_Q_N_y_avg = dynamic_cast<TProfile*>(file->Get(N_y_avg_name.c_str()));

  if (!h_sEPD_Q_S_x_avg)
  {
    throw std::runtime_error(std::format("Could not find histogram '{}' in file '{}'", S_x_avg_name, m_input_QVecCalib));
  }
  if (!h_sEPD_Q_S_y_avg)
  {
    throw std::runtime_error(std::format("Could not find histogram '{}' in file '{}'", S_y_avg_name, m_input_QVecCalib));
  }
  if (!h_sEPD_Q_N_x_avg)
  {
    throw std::runtime_error(std::format("Could not find histogram '{}' in file '{}'", N_x_avg_name, m_input_QVecCalib));
  }
  if (!h_sEPD_Q_N_y_avg)
  {
    throw std::runtime_error(std::format("Could not find histogram '{}' in file '{}'", N_y_avg_name, m_input_QVecCalib));
  }

  std::string S_xx_avg_name = std::format("h_sEPD_Q_S_xx_{}_avg", n);
  std::string S_yy_avg_name = std::format("h_sEPD_Q_S_yy_{}_avg", n);
  std::string S_xy_avg_name = std::format("h_sEPD_Q_S_xy_{}_avg", n);
  std::string N_xx_avg_name = std::format("h_sEPD_Q_N_xx_{}_avg", n);
  std::string N_yy_avg_name = std::format("h_sEPD_Q_N_yy_{}_avg", n);
  std::string N_xy_avg_name = std::format("h_sEPD_Q_N_xy_{}_avg", n);

  auto* h_sEPD_Q_S_xx_avg = dynamic_cast<TProfile*>(file->Get(S_xx_avg_name.c_str()));
  auto* h_sEPD_Q_S_yy_avg = dynamic_cast<TProfile*>(file->Get(S_yy_avg_name.c_str()));
  auto* h_sEPD_Q_S_xy_avg = dynamic_cast<TProfile*>(file->Get(S_xy_avg_name.c_str()));
  auto* h_sEPD_Q_N_xx_avg = dynamic_cast<TProfile*>(file->Get(N_xx_avg_name.c_str()));
  auto* h_sEPD_Q_N_yy_avg = dynamic_cast<TProfile*>(file->Get(N_yy_avg_name.c_str()));
  auto* h_sEPD_Q_N_xy_avg = dynamic_cast<TProfile*>(file->Get(N_xy_avg_name.c_str()));

  if (!h_sEPD_Q_S_xx_avg)
  {
    throw std::runtime_error(std::format("Could not find histogram '{}' in file '{}'", S_xx_avg_name, m_input_QVecCalib));
  }
  if (!h_sEPD_Q_S_yy_avg)
  {
    throw std::runtime_error(std::format("Could not find histogram '{}' in file '{}'", S_yy_avg_name, m_input_QVecCalib));
  }
  if (!h_sEPD_Q_S_xy_avg)
  {
    throw std::runtime_error(std::format("Could not find histogram '{}' in file '{}'", S_xy_avg_name, m_input_QVecCalib));
  }
  if (!h_sEPD_Q_N_xx_avg)
  {
    throw std::runtime_error(std::format("Could not find histogram '{}' in file '{}'", N_xx_avg_name, m_input_QVecCalib));
  }
  if (!h_sEPD_Q_N_yy_avg)
  {
    throw std::runtime_error(std::format("Could not find histogram '{}' in file '{}'", N_yy_avg_name, m_input_QVecCalib));
  }
  if (!h_sEPD_Q_N_xy_avg)
  {
    throw std::runtime_error(std::format("Could not find histogram '{}' in file '{}'", N_xy_avg_name, m_input_QVecCalib));
  }

  size_t south_idx = static_cast<size_t>(Subdetector::S);
  size_t north_idx = static_cast<size_t>(Subdetector::N);

  for (size_t cent_bin = 0; cent_bin < m_bins_cent; ++cent_bin)
  {
    int bin = static_cast<int>(cent_bin) + 1;

    double Q_S_x_avg = h_sEPD_Q_S_x_avg->GetBinContent(bin);
    double Q_S_y_avg = h_sEPD_Q_S_y_avg->GetBinContent(bin);
    double Q_N_x_avg = h_sEPD_Q_N_x_avg->GetBinContent(bin);
    double Q_N_y_avg = h_sEPD_Q_N_y_avg->GetBinContent(bin);

    double Q_S_xx_avg = h_sEPD_Q_S_xx_avg->GetBinContent(bin);
    double Q_S_yy_avg = h_sEPD_Q_S_yy_avg->GetBinContent(bin);
    double Q_S_xy_avg = h_sEPD_Q_S_xy_avg->GetBinContent(bin);
    double Q_N_xx_avg = h_sEPD_Q_N_xx_avg->GetBinContent(bin);
    double Q_N_yy_avg = h_sEPD_Q_N_yy_avg->GetBinContent(bin);
    double Q_N_xy_avg = h_sEPD_Q_N_xy_avg->GetBinContent(bin);

    // Recentering Params
    m_correction_data[cent_bin][south_idx].avg_Q = {Q_S_x_avg, Q_S_y_avg};
    m_correction_data[cent_bin][north_idx].avg_Q = {Q_N_x_avg, Q_N_y_avg};

    // Flattening Params
    m_correction_data[cent_bin][south_idx].avg_Q_xx = Q_S_xx_avg;
    m_correction_data[cent_bin][south_idx].avg_Q_yy = Q_S_yy_avg;
    m_correction_data[cent_bin][south_idx].avg_Q_xy = Q_S_xy_avg;

    m_correction_data[cent_bin][north_idx].avg_Q_xx = Q_N_xx_avg;
    m_correction_data[cent_bin][north_idx].avg_Q_yy = Q_N_yy_avg;
    m_correction_data[cent_bin][north_idx].avg_Q_xy = Q_N_xy_avg;

    // ----
    // South
    // ----

    double D_term_S_arg = (Q_S_xx_avg * Q_S_yy_avg) - (Q_S_xy_avg * Q_S_xy_avg);
    if (D_term_S_arg <= 0)
    {
      throw std::runtime_error(std::format(
          "Invalid calibration data: D-term sqrt argument is non-positive ({}) "
          "for harmonic n={}, centrality bin {}, subdetector S. "
          "Check calibration file statistics.",
          D_term_S_arg, n, cent_bin));
    }
    double D_term_S = std::sqrt(D_term_S_arg);

    double N_term_S = D_term_S * (Q_S_xx_avg + Q_S_yy_avg + (2 * D_term_S));
    if (N_term_S <= 0)
    {
      throw std::runtime_error(std::format(
          "Invalid calibration data: N-term denominator sqrt argument is non-positive ({}) "
          "for harmonic n={}, centrality bin {}, subdetector S. "
          "Check calibration file statistics.",
          N_term_S, n, cent_bin));
    }

    auto& X_S_matrix = m_correction_data[cent_bin][south_idx].X_matrix;
    X_S_matrix[0][0] = (1. / std::sqrt(N_term_S)) * (Q_S_yy_avg + D_term_S);
    X_S_matrix[0][1] = (-1. / std::sqrt(N_term_S)) * Q_S_xy_avg;
    X_S_matrix[1][0] = X_S_matrix[0][1];
    X_S_matrix[1][1] = (1. / std::sqrt(N_term_S)) * (Q_S_xx_avg + D_term_S);

    // ----
    // North
    // ----

    double D_term_N_arg = (Q_N_xx_avg * Q_N_yy_avg) - (Q_N_xy_avg * Q_N_xy_avg);
    if (D_term_N_arg <= 0)
    {
      throw std::runtime_error(std::format(
          "Invalid calibration data: D-term sqrt argument is non-positive ({}) "
          "for harmonic n={}, centrality bin {}, subdetector N. "
          "Check calibration file statistics.",
          D_term_N_arg, n, cent_bin));
    }
    double D_term_N = std::sqrt(D_term_N_arg);

    double N_term_N = D_term_N * (Q_N_xx_avg + Q_N_yy_avg + (2 * D_term_N));
    if (N_term_N <= 0)
    {
      throw std::runtime_error(std::format(
          "Invalid calibration data: N-term denominator sqrt argument is non-positive ({}) "
          "for harmonic n={}, centrality bin {}, subdetector N. "
          "Check calibration file statistics.",
          N_term_N, n, cent_bin));
    }

    auto& X_N_matrix = m_correction_data[cent_bin][north_idx].X_matrix;
    X_N_matrix[0][0] = (1. / std::sqrt(N_term_N)) * (Q_N_yy_avg + D_term_N);
    X_N_matrix[0][1] = (-1. / std::sqrt(N_term_N)) * Q_N_xy_avg;
    X_N_matrix[1][0] = X_N_matrix[0][1];
    X_N_matrix[1][1] = (1. / std::sqrt(N_term_N)) * (Q_N_xx_avg + D_term_N);
  }
}

void CalibrateQVec::print_correction_data()
{
  std::string line_separator(70, '=');

  // Format specifier for consistency (e.g., fixed-point, 6 decimal places)
  static constexpr const char* double_fmt = "{:.6f}";

  // Outer loop: Iterate over the m_bins_cent array
  for (size_t i = 0; i < m_correction_data.size(); ++i)
  {
    std::cout << std::format("\n{} \n", line_separator);
    std::cout << std::format("### Centrality Index {} of {} ###\n", i, m_bins_cent);

    // Inner loop: Iterate over the inner array of 2 CorrectionData structs
    for (size_t j = 0; j < m_correction_data[i].size(); ++j)
    {
      const auto& cd = m_correction_data[i][j];
      std::string detector = (j) ? "North" : "South";
      std::cout << std::format("\n  --- Correction Data Set {} ---\n", detector);

      // 1. Format and print QVec
      std::cout << std::format("    avg_Q: QVec(x={}, y={})\n",
                               std::format(double_fmt, cd.avg_Q.x),
                               std::format(double_fmt, cd.avg_Q.y));

      // 2. Format and print other average doubles
      std::cout << std::format("    avg_Q_xx: {} | avg_Q_yy: {} | avg_Q_xy: {}\n",
                               std::format(double_fmt, cd.avg_Q_xx),
                               std::format(double_fmt, cd.avg_Q_yy),
                               std::format(double_fmt, cd.avg_Q_xy));

      // 3. Format and print Correction Matrix (X_matrix)
      std::cout << "    X_matrix:\n";
      // Innermost loop for the 2x2 matrix rows
      std::cout << std::format("      [ {} {} ]\n",
                               std::format(double_fmt, cd.X_matrix[0][0]),
                               std::format(double_fmt, cd.X_matrix[0][1]));
      std::cout << std::format("      [ {} {} ]\n",
                               std::format(double_fmt, cd.X_matrix[1][0]),
                               std::format(double_fmt, cd.X_matrix[1][1]));
    }
  }
  std::cout << std::format("{}\n", line_separator);
}

//____________________________________________________________________________..
int CalibrateQVec::process_event(PHCompositeNode* topNode)
{
  int ret = process_centrality(topNode);
  if (ret)
  {
    return ret;
  }

  ret = process_sEPD(topNode);
  if (ret)
  {
    return ret;
  }

  // Calibrate Q Vectors
  correct_QVecs();

  // Propogate Corrected Psi 2
  double psi2_S = std::atan2(m_Q[0].y, m_Q[0].x) / 2;
  double psi2_N = std::atan2(m_Q[1].y, m_Q[1].x) / 2;

  m_CalibQVecParams.set_double_param("Psi2_S", psi2_S);
  m_CalibQVecParams.set_double_param("Psi2_N", psi2_N);
  m_CalibQVecParams.UpdateNodeTree(m_parNode, "CalibQVec");

  if(Verbosity())
  {
    std::cout << std::format("{:#<20}\n", "");
    std::cout << std::format("South: (Qx,Qy) = ({},{}), Psi: {} pi\n", m_Q[0].x, m_Q[0].y, psi2_S / std::numbers::pi);
    std::cout << std::format("North: (Qx,Qy) = ({},{}), Psi: {} pi\n", m_Q[1].x, m_Q[1].y, psi2_N / std::numbers::pi);
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int CalibrateQVec::process_centrality(PHCompositeNode *topNode)
{
  auto* centInfo = findNode::getClass<CentralityInfo>(topNode, "CentralityInfo");
  if (!centInfo)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }

  m_cent = centInfo->get_centile(CentralityInfo::PROP::mbd_NS) * 100;

  if (!std::isfinite(m_cent))
  {
    m_cent = 0;
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int CalibrateQVec::process_sEPD(PHCompositeNode* topNode)
{
  TowerInfoContainer* towerinfosEPD = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_SEPD");
  if (!towerinfosEPD)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }

  EpdGeom* epdgeom = findNode::getClass<EpdGeom>(topNode, "TOWERGEOM_EPD");
  if (!epdgeom)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }

  // sepd
  unsigned int nchannels_epd = towerinfosEPD->size();

  double sepd_total_charge_south = 0;
  double sepd_total_charge_north = 0;

  for (unsigned int channel = 0; channel < nchannels_epd; ++channel)
  {
    unsigned int key = TowerInfoDefs::encode_epd(channel);

    TowerInfo* tower = towerinfosEPD->get_tower_at_channel(channel);

    double charge = tower->get_energy();
    bool isZS = tower->get_isZS();
    double phi = epdgeom->get_phi(key);

    // Ensure phi is in [-pi, pi]
    if (phi > std::numbers::pi)
    {
      phi -= 2 * std::numbers::pi;
    }

    // exclude ZS
    // expecting Nmips
    if (isZS || charge < m_sepd_charge_threshold || m_bad_channels.contains(channel))
    {
      continue;
    }

    QVec q_2 = {charge * std::cos(2 * phi), charge * std::sin(2 * phi)};

    // sepd charge sums
    unsigned int arm = TowerInfoDefs::get_epd_arm(key);

    // arm = 0: South
    // arm = 1: North
    double& sepd_total_charge = (arm == 0) ? sepd_total_charge_south : sepd_total_charge_north;

    // Compute total charge for the respective sEPD arm
    sepd_total_charge += charge;

    m_Q[arm].x += q_2.x;
    m_Q[arm].y += q_2.y;
  }

  // ensure both total charges are nonzero
  if (sepd_total_charge_south == 0 || sepd_total_charge_north == 0)
  {
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  m_Q[0].x /= sepd_total_charge_south;
  m_Q[0].y /= sepd_total_charge_south;

  m_Q[1].x /= sepd_total_charge_north;
  m_Q[1].y /= sepd_total_charge_north;

  return Fun4AllReturnCodes::EVENT_OK;
}

void CalibrateQVec::correct_QVecs()
{
  size_t cent_bin = static_cast<size_t>(std::min(static_cast<int>(m_bins_cent), hCentrality->FindBin(m_cent)) - 1);

  size_t south_idx = static_cast<size_t>(Subdetector::S);
  size_t north_idx = static_cast<size_t>(Subdetector::N);

  double Q_S_x_avg = m_correction_data[cent_bin][south_idx].avg_Q.x;
  double Q_S_y_avg = m_correction_data[cent_bin][south_idx].avg_Q.y;
  double Q_N_x_avg = m_correction_data[cent_bin][north_idx].avg_Q.x;
  double Q_N_y_avg = m_correction_data[cent_bin][north_idx].avg_Q.y;

  QVec q_S = m_Q[south_idx];
  QVec q_N = m_Q[north_idx];

  // Apply Recentering
  QVec q_S_corr = {q_S.x - Q_S_x_avg, q_S.y - Q_S_y_avg};
  QVec q_N_corr = {q_N.x - Q_N_x_avg, q_N.y - Q_N_y_avg};

  const auto& X_S = m_correction_data[cent_bin][south_idx].X_matrix;
  const auto& X_N = m_correction_data[cent_bin][north_idx].X_matrix;

  // Apply Flattening
  double Q_S_x_corr2 = X_S[0][0] * q_S_corr.x + X_S[0][1] * q_S_corr.y;
  double Q_S_y_corr2 = X_S[1][0] * q_S_corr.x + X_S[1][1] * q_S_corr.y;
  double Q_N_x_corr2 = X_N[0][0] * q_N_corr.x + X_N[0][1] * q_N_corr.y;
  double Q_N_y_corr2 = X_N[1][0] * q_N_corr.x + X_N[1][1] * q_N_corr.y;

  QVec q_S_corr2 = {Q_S_x_corr2, Q_S_y_corr2};
  QVec q_N_corr2 = {Q_N_x_corr2, Q_N_y_corr2};

  m_Q[south_idx] = q_S_corr2;
  m_Q[north_idx] = q_N_corr2;
}

//____________________________________________________________________________..
int CalibrateQVec::ResetEvent([[maybe_unused]] PHCompositeNode* topNode)
{
  // Reset Q Vectors
  std::fill(m_Q.begin(), m_Q.end(), QVec{0.0, 0.0});

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int CalibrateQVec::End([[maybe_unused]] PHCompositeNode* topNode)
{
  std::cout << "CalibrateQVec::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}
