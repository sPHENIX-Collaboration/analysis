#ifndef MULTIDETECTORMULTIPLICITY_H__
#define MULTIDETECTORMULTIPLICITY_H__

#include <fun4all/SubsysReco.h>

/// Class declarations for use in the analysis module
class PHCompositeNode;
class TFile;
class TTree;
class TH1;
class TH2;

/// Definition of this analysis module class
class MultiDetectorMultiplicity : public SubsysReco
{
 public:
  enum OBJECT
  {
    VERTEX = 0,
    CENTRALITY = 1,
    TRACKS = 2,
    MVTXCLUSTERS = 3,
    EMCALTOWERS = 4,
    IHCALTOWERS = 5,
    OHCALTOWERS = 6,
    EPDTOWERS = 7
  };

  /// Constructor
  MultiDetectorMultiplicity(const std::string &name = "MultiDetectorMultiplicity",
              const std::string &fname = "MultiDetectorMultiplicity.root");

  // Destructor
  virtual ~MultiDetectorMultiplicity();

  /// SubsysReco initialize processing method
  int Init(PHCompositeNode *);

  /// SubsysReco event processing method
  int process_event(PHCompositeNode *);

  /// SubsysReco end processing method
  int End(PHCompositeNode *);

  void initializeObjects();
  float getFilteredNtracks(PHCompositeNode *topNode);
  float getMVTXnclusters(PHCompositeNode *topNode);
  float getEMCalTotalEnergy(PHCompositeNode *topNode);
  float getIHCalTotalEnergy(PHCompositeNode *topNode);
  float getOHCalTotalEnergy(PHCompositeNode *topNode);
  float getsEPDTotalEnergy(PHCompositeNode *topNode);
  void setCentralityHistoLabel(TH2 *histo, bool doYaxisLabels = false);
  bool isVertexAccepted(PHCompositeNode *topNode);

 private:
  /// String to contain the outfile name containing the trees
  std::string _outfilename;
  const float _zVertexAcceptance = 10.; //cm
  const unsigned int _nlayers_maps = 3;
  const unsigned int _nlayers_intt = 4;
  const unsigned int _nlayers_tpc = 48;

  /// 2D Histograms
  TH2 *_tracks_EMCALenergy; // Track multiplicity (with selections) vs total EMCal energy
  TH2 *_tracks_IHCALenergy; // Track multiplicity (with selections) vs total IHCal energy
  TH2 *_tracks_OHCALenergy; // Track multiplicity (with selections) vs total OHCal energy
  TH2 *_tracks_AllCALenergy; // Track multiplicity (with selections) vs total EMCal+IHCal+OHCal energy
  TH2 *_tracks_MVTXclusters; // Track multiplicity (with selections) vs MVTX cluster multiplicity

  TH2 *_MVTXclusters_EMCALenergy; // MVTX cluster multiplicity vs total EMCal energy
  TH2 *_MVTXclusters_IHCALenergy; // MVTX cluster multiplicity vs total IHCal energy
  TH2 *_MVTXclusters_OHCALenergy; // MVTX cluster multiplicity vs total OHCal energy
  TH2 *_MVTXclusters_AllCALenergy; // MVTX cluster multiplicity vs total EMCal+IHCal+OHCal energy

  TH2 *_EMCALenergy_IHCALenergy; // Total EMCal energy vs total IHCal energy
  TH2 *_EMCALenergy_OHCALenergy; // Total EMCal energy vs total OHCal energy
  TH2 *_IHCALenergy_OHCALenergy; // Total IHCal energy vs total OHCal energy

  TH2 *_MBDcentrality_tracks; // Centrality percentile (MBD) vs track multiplicity (with selections)
  TH2 *_MBDcentrality_MVTXclusters; // Centrality percentile (MBD) vs MVTX cluster multiplicity
  TH2 *_MBDcentrality_EMCALenergy; // Centrality percentile (MBD) vs total EMCal energy
  TH2 *_MBDcentrality_IHCALenergy; // Centrality percentile (MBD) vs total IHCal energy
  TH2 *_MBDcentrality_OHCALenergy; // Centrality percentile (MBD) vs total OHCal energy
  TH2 *_MBDcentrality_AllCALenergy; // Centrality percentile (MBD) vs total EMCal+IHCal+OHCal energy
  TH2 *_MBDcentrality_EPDenergy; // Centrality percentile (MBD) vs total sEPD energy

  TH2 *_EPDcentrality_tracks; // Centrality percentile (sEPD) vs track multiplicity (with selections)
  TH2 *_EPDcentrality_MVTXclusters; // Centrality percentile (sEPD) vs MVTX cluster multiplicity
  TH2 *_EPDcentrality_EMCALenergy; // Centrality percentile (sEPD) vs total EMCal energy
  TH2 *_EPDcentrality_IHCALenergy; // Centrality percentile (sEPD) vs total IHCal energy
  TH2 *_EPDcentrality_OHCALenergy; // Centrality percentile (sEPD) vs total OHCal energy
  TH2 *_EPDcentrality_AllCALenergy; // Centrality percentile (sEPD) vs total EMCal+IHCal+OHCal energy
  TH2 *_EPDcentrality_EPDenergy; // Centrality percentile (sEPD) vs total sEPD energy

  TH2 *_EPDcentrality_MBDcentrality; // Centrality percentile (sEPD) vs centrality percentile (sEPD)

  TH2 *_EPDenergy_tracks; // Total sEPD energy vs track multiplicity (with selections)
  TH2 *_EPDenergy_MVTXclusters; // Total sEPD energy vs MVTX cluster multiplicity
  TH2 *_EPDenergy_EMCALenergy; // Total sEPD energy vs total EMCal energy
  TH2 *_EPDenergy_IHCALenergy; // Total sEPD energy vs total IHCal energy
  TH2 *_EPDenergy_OHCALenergy; // Total sEPD energy vs total OHCal energy
  TH2 *_EPDenergy_AllCALenergy; // Total sEPD energy vs total EMCal+IHCal+OHCal energy

  // This histo keeps track of the number of times certain objects were not found.
  // Ideally this histogram should be EMPTY!!!
  // For future updates including other objects, be sure to not reject the event before all objects are checked
  TH1 *_SubsystemObjectsFail;



  /// TFile to hold the following TTrees and histograms
  TFile *_outfile;
  float _mbd_centrality;
  float _epd_centrality;

  const int _nbins = 100;
  const float _track_bin_max = 1000.;
  const float _mvtx_bin_max = 20000.;
  const float _emcal_bin_max = 2000.;
  const float _ihcal_bin_max = 200.;
  const float _ohcal_bin_max = 500.;
  const float _allcal_bin_max = _emcal_bin_max+_ihcal_bin_max+_ohcal_bin_max;
  const float _epd_bin_max = 20000.;

};

#endif
