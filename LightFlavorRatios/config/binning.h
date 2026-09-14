#ifndef BINNING_H
#define BINNING_H

#include "../util/HistogramTools.h"

namespace BinInfo
{
  //static const HistogramInfo final_pt_bins("pT","pT",10,0.2,3.,"pT [GeV/c]");
  // special bins to sync with Tony's tracking efficiency
  static const HistogramInfo final_pt_bins("pT","pT",{0.6,0.7,0.8,0.9,1.,1.1,1.2,1.3,1.4,1.5,1.8,2.1,2.4,2.7,3.,3.99999},"pT [GeV/c]");
  //  static const HistogramInfo final_pt_bins("pT","pT",{0.8,1.1,1.4,1.8,2.2,3.,3.9999},"pT [GeV/c]");
  static const HistogramInfo final_rapidity_bins("rapidity","rapidity",15,-0.8,0.8,"rapidity");
  static const HistogramInfo final_eta_bins("pseudorapidity","#eta",15,-0.8,0.8,"#eta");
  static const HistogramInfo final_phi_bins("phi","#phi",15,-M_PI,M_PI,"#phi");
  static const HistogramInfo final_ntrack_bins("ntrk","nTracks",makeLogBins(5,1.,20),"number of tracks");

  static const HistogramInfo pt_bins("pt","pT",100,0.,1.1,"pT [GeV/c]");
  static const HistogramInfo rapidity_bins("y","rapidity",100,-1.5,1.5,"y");
  static const HistogramInfo phi_bins("phi","#phi",100,-M_PI,M_PI,"#phi");
  static const HistogramInfo ntrack_bins("ntrk","nTracks",201,-0.5,200.5,"number of tracks");

  static const HistogramInfo K_S0_data_mass_bins("K_S0_mass","K^{0}_{S} mass",200,0.4,0.64,"mass [GeV/c^{2}]");
  static const HistogramInfo Lambda0_data_mass_bins("Lambda0_mass","#Lambda mass",200,1.09,1.18,"mass [GeV/c^{2}]");

  static const HistogramInfo K_S0_MC_mass_bins("K_S0_mass","K^{0}_{S} mass",300,0.42,0.58,"mass [GeV/c^{2}]");
  static const HistogramInfo Lambda0_MC_mass_bins("Lambda0_mass","#Lambda mass",300,1.1,1.14,"mass [GeV/c^{2}");

} // namespace BinInfo

#endif // BINNING_H
