#ifndef CUTS_H
#define CUTS_H

#include "binning.h"

struct CutSettings
{
  std::string name = "";
  std::string mother_name = "";
  bool isData = false;

  int track_1_min_MVTX_nHits = -1;
  int track_2_min_MVTX_nHits = -1;
  int track_1_min_INTT_nHits = -1;
  int track_2_min_INTT_nHits = -1;
  int track_1_min_TPC_nHits = -1;
  int track_2_min_TPC_nHits = -1;

  float primary_vertex_max_z = FLT_MAX;
  float track_1_min_pT = -1.;
  float track_2_min_pT = -1.;
  float track_1_max_quality = FLT_MAX;
  float track_2_max_quality = FLT_MAX;

  float track_1_PV_min_DCA_xy = -1.;
  float track_2_PV_min_DCA_xy = -1.;
  float track_1_track_2_max_DCA = FLT_MAX;
  float track_1_track_2_max_DCA_xy = FLT_MAX;

  float min_DIRA = -FLT_MAX;

  int max_background_order = 1;
  std::pair<float,float> left_sideband = {-FLT_MAX,FLT_MAX};
  std::pair<float,float> right_sideband = {-FLT_MAX,FLT_MAX};

  std::string misc_cutstring = "";
};

CutSettings add_misc_cutstring(const CutSettings& c, std::string newname, std::string newmisc_cutstring)
{
  CutSettings copy = c;
  copy.name = newname;
  copy.misc_cutstring = newmisc_cutstring;
  return copy;
}

std::string generate_fiducial_cuts(const std::string& particle_name,const std::vector<HistogramInfo> variables)
{
  std::string cut;
  for(size_t i=0;i<variables.size();i++)
  {
    cut += particle_name+"_"+variables[i].name+">="+std::to_string(variables[i].bins.front())
           +" && "+particle_name+"_"+variables[i].name+"<="+std::to_string(variables[i].bins.back());
    if(i<variables.size()-1) cut += " && ";
  }
  return cut;
}

std::string generate_selection_cutstring(const CutSettings& settings,const std::vector<HistogramInfo> variables)
{
  std::string daughter_pt_cuts = "track_1_pT>"+std::to_string(settings.track_1_min_pT)+" && track_2_pT>"+std::to_string(settings.track_2_min_pT);
  std::string fiducial_cuts = generate_fiducial_cuts(settings.mother_name,variables);

  // in data, basically all of this is handled by KFParticle settings
  if(settings.isData)
  {
    std::string crossing_cuts = "track_1_bunch_crossing>0 && track_1_bunch_crossing<360 && track_2_bunch_crossing>0 && track_2_bunch_crossing<360";
    std::string full_cuts = crossing_cuts+" && "+daughter_pt_cuts+" && "+fiducial_cuts;
    if(!settings.misc_cutstring.empty()) full_cuts += " && "+settings.misc_cutstring;
    return full_cuts;
  }

  // in MC, KFParticle applies no cuts, and also only populates crossing 0
  else
  {
    std::string MC_PVz_cut = "fabs(primary_vertex_z)<="+std::to_string(settings.primary_vertex_max_z);

    std::string MC_nHits_cuts = "track_1_MVTX_nHits>"+std::to_string(settings.track_1_min_MVTX_nHits)
                              +" && track_2_MVTX_nHits>"+std::to_string(settings.track_2_min_MVTX_nHits)
                              +" && track_1_INTT_nHits>"+std::to_string(settings.track_1_min_INTT_nHits)
                              +" && track_2_INTT_nHits>"+std::to_string(settings.track_2_min_INTT_nHits)
                              +" && track_1_TPC_nHits>"+std::to_string(settings.track_1_min_TPC_nHits)
                              +" && track_2_TPC_nHits>"+std::to_string(settings.track_2_min_TPC_nHits);

    std::string MC_daughter_selection_cuts = "fabs(track_1_PV_DCA_xy)>="+std::to_string(settings.track_1_PV_min_DCA_xy)
                                           +" && fabs(track_2_PV_DCA_xy)>="+std::to_string(settings.track_2_PV_min_DCA_xy)
                                           +" && (track_1_chi2/track_1_nDoF)<="+std::to_string(settings.track_1_max_quality)
                                           +" && (track_2_chi2/track_2_nDoF)<="+std::to_string(settings.track_2_max_quality);

    std::string MC_mother_selection_cuts = settings.mother_name+"_DIRA>="+std::to_string(settings.min_DIRA)
                                         +" && track_1_track_2_DCA_xy<="+std::to_string(settings.track_1_track_2_max_DCA_xy)
                                         +" && track_1_track_2_DCA<="+std::to_string(settings.track_1_track_2_max_DCA);

    std::string full_cuts = daughter_pt_cuts+" && "+fiducial_cuts+" && "+MC_PVz_cut+" && "+MC_nHits_cuts+" && "+MC_daughter_selection_cuts+" && "+MC_mother_selection_cuts;
    if(!settings.misc_cutstring.empty()) full_cuts += " && "+settings.misc_cutstring;
    return full_cuts;
  }
}

namespace StandardCuts
{

  // (Lambda+anti-Lambda)/2Ks data
  CutSettings data_K_S0_cuts =
  {
    .name = "data_K_S0_cuts",
    .mother_name = "K_S0",
    .isData = true,
    .track_1_min_pT = 0.2,
    .track_2_min_pT = 0.2,

    .max_background_order = 6,
    .left_sideband = {0.4,0.42},
    .right_sideband = {0.58,0.64}
  };
  CutSettings data_Lambda0_cuts =
  {
    .name = "data_Lambda0_cuts",
    .mother_name = "Lambda0",
    .isData = true,
    .track_1_min_pT = 0.2,
    .track_2_min_pT = 0.2,

    .max_background_order = 6,
    .left_sideband = {1.09,1.105},
    .right_sideband = {1.13,1.18}
  };

  // Lambda/Ks data
  CutSettings data_Lambda0_pos_cuts = add_misc_cutstring(data_Lambda0_cuts,"data_Lambda0_pos_cuts","track_1_charge==-1 && track_2_charge==1");

  // anti-Lambda/Ks data
  CutSettings data_Lambda0_neg_cuts = add_misc_cutstring(data_Lambda0_cuts,"data_Lambda0_neg_cuts","track_1_charge==1 && track_2_charge==-1");

  // (Lambda+anti-Lambda)/2Ks MC
  CutSettings MC_K_S0_cuts =
  {
    .name = "MC_K_S0_cuts",
    .mother_name = "K_S0",
    .isData = false,

    .track_1_min_MVTX_nHits = 0,
    .track_2_min_MVTX_nHits = 0,
    .track_1_min_INTT_nHits = 0,
    .track_2_min_INTT_nHits = 0,
    .track_1_min_TPC_nHits = 19,
    .track_2_min_TPC_nHits = 19,

    .primary_vertex_max_z = 10.,
    .track_1_min_pT = 0.2,
    .track_2_min_pT = 0.2,
    .track_1_max_quality = 300.,
    .track_2_max_quality = 300.,

    .track_1_PV_min_DCA_xy = 0.01,
    .track_2_PV_min_DCA_xy = 0.01,
    .track_1_track_2_max_DCA = 0.75,
    .track_1_track_2_max_DCA_xy = 1.25,

    .min_DIRA = 0.98,

    .max_background_order = 8,
    .left_sideband = {0.42,0.46},
    .right_sideband = {0.52,0.58}
  };

  CutSettings MC_Lambda0_cuts =
  {
    .name = "MC_Lambda0_cuts",
    .mother_name = "Lambda0",
    .isData = false,

    .track_1_min_MVTX_nHits = 0,
    .track_2_min_MVTX_nHits = 0,
    .track_1_min_INTT_nHits = 0,
    .track_2_min_INTT_nHits = 0,
    .track_1_min_TPC_nHits = 19,
    .track_2_min_TPC_nHits = 19,

    .primary_vertex_max_z = 10.,
    .track_1_min_pT = 0.2,
    .track_2_min_pT = 0.2,
    .track_1_max_quality = 300.,
    .track_2_max_quality = 300.,

    .track_1_PV_min_DCA_xy = 0.01,
    .track_2_PV_min_DCA_xy = 0.01,
    .track_1_track_2_max_DCA = 0.75,
    .track_1_track_2_max_DCA_xy = 1.25,

    .min_DIRA = 0.98,

    .max_background_order = 8,
    .left_sideband = {1.1,1.105},
    .right_sideband = {1.125,1.14}
  };

  // Lambda/Ks MC 
  CutSettings MC_Lambda0_pos_cuts = add_misc_cutstring(MC_Lambda0_cuts,"MC_Lambda0_pos_cuts","track_1_charge==-1 && track_2_charge==1");

  // anti-Lambda/Ks MC
  CutSettings MC_Lambda0_neg_cuts = add_misc_cutstring(MC_Lambda0_cuts,"MC_Lambda0_neg_cuts","track_1_charge==1 && track_2_charge==-1");

} // namespace StandardCuts

#endif // CUTS_H
