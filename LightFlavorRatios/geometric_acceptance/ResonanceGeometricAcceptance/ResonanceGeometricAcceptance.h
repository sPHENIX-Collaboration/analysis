// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef RESONANCE_GEOMETRICACCEPTANCE_H
#define RESONANCE_GEOMETRICACCEPTANCE_H

#include <fun4all/SubsysReco.h>

#include "../../util/binning.h"

#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/PHG4ParticleSvtxMap.h>
#include <trackbase_historic/SvtxPHG4ParticleMap.h>

#include <globalvertex/SvtxVertexMap.h>

#include <g4main/PHG4TruthInfoContainer.h>

#include <KFParticle.h>

#include <TFile.h>
#include <string>

class PHCompositeNode;

class ResonanceGeometricAcceptance : public SubsysReco
{
 public:

  ResonanceGeometricAcceptance(const std::string &name = "ResonanceGeometricAcceptance");

  ~ResonanceGeometricAcceptance() override;

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

  void setTrackMapName(const std::string& name)
  {
    m_trackmap_name = name;
  }

  void setVertexMapName(const std::string& name)
  {
    m_vertexmap_name = name;
  }

  void setTruthContainerName(const std::string& name)
  {
    m_truth_name = name;
  }

  void setOutputFilename(const std::string& name)
  {
    m_outfile_name = name;
  }

  void setMotherName(const std::string& name)
  {
    m_mother_name = name;
  }

  void setMotherPDGID(const int pdgid)
  {
    m_mother_pdgid = pdgid;
  }

  void setDaughterPDGIDs(const std::vector<int>& daughter_pdgids)
  {
    m_daughter_pdgids = daughter_pdgids;
  }

  void includeConjugate(const bool include = true)
  {
    m_include_conjugate = include;
  }

  void setTruthWeightThreshold(const int threshold)
  {
    m_truth_weight_threshold = threshold;
  }

  void SetVertexZLimits(const std::pair<float,float> limits)
  {
    zvertex_limits = limits;
  }

  void setPtBins(const HistogramInfo& hinfo)
  {
    pt_bins = hinfo;
  }

  void setEtaBins(const HistogramInfo& hinfo)
  {
    eta_bins = hinfo;
  }

  void setPhiBins(const HistogramInfo& hinfo)
  {
    phi_bins = hinfo;
  }

  void setRapidityBins(const HistogramInfo& hinfo)
  {
    rapidity_bins = hinfo;
  }

  void setPtLimits(float vmin, float vmax)
  {
    pt_limits = {vmin, vmax};
  }

  void setEtaLimits(float vmin, float vmax)
  {
    eta_limits = {vmin, vmax};
  }

  void setPhiLimits(float vmin, float vmax)
  {
    phi_limits = {vmin, vmax};
  }

  void setRapidityLimits(float vmin, float vmax)
  {
    rapidity_limits = {vmin, vmax};
  }

 private:

  int get_best_reco_vertex(KFParticle mother, const int crossing) const;
  KFParticle make_KFParticle_Vertex(SvtxVertex* vertex) const;
  KFParticle make_KFParticle(PHG4Particle* particle, SvtxTrack* track) const;
  std::set<SvtxTrack*> get_best_reco_matches(const PHG4Particle* particle) const;
  std::set<SvtxTrack*> get_reco_matches_with_silicon_hits(const PHG4Particle* particle) const;
  std::vector<std::vector<SvtxTrack*>> get_all_reco_combinations(const std::vector<PHG4Particle*>& daughters) const;
  bool has_all_daughters(const std::vector<PHG4Particle*>& daughters) const;
  bool has_daughter(const int pdgid, const std::vector<PHG4Particle*>& daughters) const;
  bool check_charge_swaps(const std::vector<PHG4Particle*>& daughters, const std::vector<SvtxTrack*>& reco_daughters) const;
  bool check_crossings_match(const std::vector<SvtxTrack*>& reco_daughters) const;
  void identify(KFParticle kfp) const;
  int get_mother_PDGID(const std::vector<PHG4Particle*>& daughters) const;
  //bool truth_track_is_best_match_to_reco_track(const PHG4Particle* particle) const;

  HistogramInfo pt_bins = BinInfo::final_pt_bins;
  HistogramInfo eta_bins = BinInfo::final_eta_bins;
  HistogramInfo phi_bins = BinInfo::final_phi_bins;
  HistogramInfo rapidity_bins = BinInfo::final_rapidity_bins;

  std::pair<float,float> zvertex_limits = {-10.,10.}; // cm

  std::pair<float,float> pt_limits = {BinInfo::final_pt_bins.bins.front(),BinInfo::final_pt_bins.bins.back()};
  std::pair<float,float> eta_limits = {BinInfo::final_eta_bins.bins.front(),BinInfo::final_eta_bins.bins.back()};
  std::pair<float,float> phi_limits = {BinInfo::final_phi_bins.bins.front(),BinInfo::final_phi_bins.bins.back()};
  std::pair<float,float> rapidity_limits = {BinInfo::final_rapidity_bins.bins.front(),BinInfo::final_rapidity_bins.bins.back()};

  TH1F* all_candidates_vspt;
  TH1F* all_candidates_vseta;
  TH1F* all_candidates_vsphi;
  TH1F* all_candidates_vsrapidity;

  TH1F* passing_candidates_vspt;
  TH1F* passing_candidates_vseta;
  TH1F* passing_candidates_vsphi;
  TH1F* passing_candidates_vsrapidity;

  TFile* outfile;

  int m_mother_pdgid = 0;
  std::vector<int> m_daughter_pdgids;
  bool m_include_conjugate = false;
  int m_truth_weight_threshold = 20;

  std::string m_mother_name = "Particle";
  std::string m_trackmap_name = "SvtxTrackMap";
  std::string m_vertexmap_name = "SvtxVertexMap";
  std::string m_truth_name = "G4TruthInfo";
  std::string m_outfile_name = "geometric_acceptance.root";

  SvtxTrackMap* m_trackmap;
  SvtxVertexMap* m_vertexmap;
  PHG4TruthInfoContainer* m_truth;
  PHG4ParticleSvtxMap* m_truth_reco_map;
  SvtxPHG4ParticleMap* m_reco_truth_map;
};

#endif // GEOMETRICACCEPTANCE_H
