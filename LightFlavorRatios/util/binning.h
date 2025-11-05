#ifndef BINNING_H
#define BINNING_H

#include <map>

struct HistogramInfo
{
  std::string name;
  std::string title;
  std::string axis_label;
  int nBins;
  float min;
  float max;
  std::string cut_string;
  std::string bin_string() const
  {
    return std::to_string(nBins)+","+std::to_string(min)+","+std::to_string(max);
  }
};

TH1F* makeHistogram(std::string basename, std::string basetitle, HistogramInfo hinfo)
{
  TH1F* h = new TH1F((basename+hinfo.name).c_str(),(basetitle+hinfo.title).c_str(),hinfo.nBins,hinfo.min,hinfo.max);
  return h;
}

TH2F* make2DHistogram(std::string basename, std::string basetitle, HistogramInfo hinfo_x, HistogramInfo hinfo_y)
{
  TH2F* h = new TH2F((basename+hinfo_x.name+hinfo_y.name).c_str(),(basetitle+hinfo_x.title+hinfo_y.title+";"+hinfo_x.axis_label+";"+hinfo_y.axis_label).c_str(),hinfo_x.nBins,hinfo_x.min,hinfo_x.max,hinfo_y.nBins,hinfo_y.min,hinfo_y.max);
  return h;
}

namespace BinInfo
{
  static const HistogramInfo final_pt_bins = {
    .name = "_vspt",
    .title = " vs. pT",
    .axis_label = "pT [GeV]",
    .nBins = 10,
    .min = 0.,
    .max = 5.,
    .cut_string = ""
  };

  static const HistogramInfo final_rapidity_bins = {
    .name = "_vsy",
    .title = " vs. rapidity",
    .axis_label = "y",
    .nBins = 10,
    .min = -1.5,
    .max = 1.5,
    .cut_string = ""
  };

  static const HistogramInfo final_phi_bins = {
    .name = "_vsphi",
    .title = " vs. phi",
    .axis_label = "#{phi}",
    .nBins = 10,
    .min = -3.15,
    .max = 3.15,
    .cut_string = ""
  };

  static const HistogramInfo final_ntrack_bins = {
    .name = "_vsntrk",
    .title = " vs. nTracks",
    .axis_label = "number of tracks",
    .nBins = 11,
    .min = -0.5,
    .max = 100.5,
    .cut_string = ""
  };

  static const HistogramInfo pt_bins = {
    .name = "_vspt",
    .title = " vs. pT",
    .axis_label = "pT [GeV]",
    .nBins = 100,
    .min = 0.,
    .max = 5.,
    .cut_string = ""
  };

  static const HistogramInfo rapidity_bins = {
    .name = "_vsy",
    .title = " vs. rapidity",
    .axis_label = "y",
    .nBins = 100,
    .min = -1.5,
    .max = 1.5,
    .cut_string = ""
  };

  static const HistogramInfo phi_bins = {
    .name = "_vsphi",
    .title = " vs. phi",
    .axis_label = "#{phi}",
    .nBins = 100,
    .min = -3.15,
    .max = 3.15,
    .cut_string = ""
  };

  static const HistogramInfo ntrack_bins = {
    .name = "_vsntrk",
    .title = " vs. nTracks",
    .axis_label = "number of tracks",
    .nBins = 201,
    .min = -0.5,
    .max = 200.5,
    .cut_string = ""
  };

  static const std::map<std::string,HistogramInfo> mass_bins = {
    {"K_S0", { .name = "Kshort_mass", .title = "K^0_S mass", .axis_label = "mass [GeV/c^{2}]",
               .nBins = 100, .min = 0.4, .max = 0.6,
  /* QM cut string, no longer viable
               .cut_string = "track_1_MVTX_nStates>0 && track_2_MVTX_nStates>0 && "
                             "track_1_TPC_nHits>25 && track_2_TPC_nHits>25 && "
                             "(track_1_chi2/track_1_nDoF)<=100. && (track_2_chi2/track_2_nDoF)<=100. && "
                             "track_1_IP_xy>=0.05 && track_2_IP_xy>=0.05 && "
                             "track_1_track_2_DCA<=0.5 && "
                             "K_S0_DIRA>=0.999 && (K_S0_chi2/K_S0_nDoF)<=20."
  */ 
               .cut_string = "" } },
    {"phi",    { .name = "phi_mass", .title = "${phi} mass", .axis_label = "mass [GeV/c^{2}]",
                 .nBins = 100, .min = 0.95, .max = 1.1,
                 .cut_string = "track_1_MVTX_nStates>=2 && track_2_MVTX_nStates>=2 &&"
                               "track_1_IP_xy<=0.05 && track_2_IP_xy<=0.05 && "
                               "phi_decayLength<=0.05" } },
    {"Lambda0", { .name = "lambda_mass", .title = "${Lambda} mass", .axis_label = "mass [GeV/c^{2}]",
                  .nBins = 100, .min = 1.05, .max = 1.2,
  /* QM cut string, no longer viable                
                  .cut_string = "track_1_MVTX_nStates>0 && track_2_MVTX_nStates>0 &&"
                                "track_1_TPC_nHits>25 && track_2_TPC_nHits>25 &&"
                                "(track_1_chi2/track_1_nDoF)<=100 && (track_2_chi2/track_2_nDoF)<=100 &&"
                                "track_1_IP_xy>=0.05 && track_2_IP_xy>=0.05 &&"
                                "track_1_track_2_DCA<=0.5 &&"
                                "Lambda0_DIRA>=0.999 && (Lambda0_chi2/Lambda0_nDoF)<=20."
  */
                  .cut_string = "" } }
  };
} // namespace BinInfo
#endif // BINNING_H
