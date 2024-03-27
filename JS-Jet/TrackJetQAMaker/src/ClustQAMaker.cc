// ----------------------------------------------------------------------------
// 'ClustQAMaker.cc'
// Derek Anderson
// 03.25.2024
//
// A submodule for the TrackJetQAMaker module
// to generate QA plots for track clusters
// ----------------------------------------------------------------------------

#define TRACKJETQAMAKER_CLUSTQAMAKER_CC

// submodule definition
#include "ClustQAMaker.h"



// public methods -------------------------------------------------------------

void ClustQAMaker::Init(TrackJetQAMakerHistDef& hist, TrackJetQAMakerHelper& help) {

  // grab module utilities
  m_help = help;
  m_hist = hist;

  // make histograms
  BuildHistograms();
  return;

}  // end 'Init(TrackJetQAMakerHistDef&, TrackJetQAMakerHelper&)'



void ClustQAMaker::Process(PHCompositeNode* topNode) {

  // grab acts geometry from node tree
  m_actsGeom = findNode::getClass<ActsGeometry>(topNode, "ActsGeometry");
  if (!m_actsGeom) {
    std::cerr << PHWHERE << ": PANIC: couldn't grab ACTS geometry from node tree!" << std::endl;
    assert(m_actsGeom);
  }

  // grab hit map from node tree
  m_hitMap = findNode::getClass<TrkrHitSetContainer>(topNode, "TRKR_HITSET");
  if (!m_hitMap) {
    std::cerr << PHWHERE << ": PANIC: couldn't grab hit map from node tree!" << std::endl;
    assert(m_hitMap);
  }

  m_clustMap = findNode::getClass<TrkrClusterContainer>(topNode, "TRKR_CLUSTER");
  if (!m_clustMap) {
    std::cerr << PHWHERE << ": PANIC: couldn't grab cluster map from node tree!" << std::endl;
    assert(m_clustMap);
  }

  // loop over hit sets
  TrkrHitSetContainer::ConstRange hitSets = m_hitMap -> getHitSets();
  for (
    TrkrHitSetContainer::ConstIterator itSet = hitSets.first;
    itSet != hitSets.second;
    ++itSet
  ) {

    // loop over clusters associated w/ hit set
    TrkrDefs::hitsetkey              setKey   = itSet      -> first;
    TrkrClusterContainer::ConstRange clusters = m_clustMap -> getClusters(setKey);
    for (
      TrkrClusterContainer::ConstIterator itClust = clusters.first;
      itClust != clusters.second;
      ++itClust
    ) {

      // grab cluster
      TrkrDefs::cluskey clustKey = itClust    -> first;
      TrkrCluster*      cluster  = m_clustMap -> findCluster(clustKey);

      // check which subsystem cluster is in
      const uint16_t layer  = TrkrDefs::getLayer(clustKey);
      const bool     isMvtx = m_help.IsInMvtx(layer);
      const bool     isIntt = m_help.IsInIntt(layer);
      const bool     isTpc  = m_help.IsInTpc(layer);

      // get cluster position
      Acts::Vector3 actsPos = m_actsGeom -> getGlobalPosition(clustKey, cluster);

      // collect cluster infor
      ClustQAContent content {
        .x = actsPos(0),
        .y = actsPos(1),
        .z = actsPos(2),
        .r = std::hypot( actsPos(0), actsPos(1) )
      };

      // fill histograms
      FillHistograms(Type::All, content);
      if (isMvtx) {
        FillHistograms(Type::Mvtx, content);
      } else if (isIntt) {
        FillHistograms(Type::Intt, content);
      } else if (isTpc) {
        FillHistograms(Type::Tpc, content);
      }
    }  // end cluster loop
  }  // end hit set loop
  return;

}  // end 'Process(PHCompositeNode*)'



void ClustQAMaker::End(TFile* outFile, std::string outDirName) {

  TDirectory* outDir = outFile -> mkdir(outDirName.data());
  if (!outDir) {
    std::cerr << PHWHERE << ": PANIC: unable to make output directory!" << std::endl;
    assert(outDir);
  }

  outDir -> cd();
  for (auto hist1Ds : vecHist1D) {
    for (TH1D* hist1D : hist1Ds) {
      hist1D -> Write();
    }
  }
  for (auto hist2Ds : vecHist2D) {
    for (TH2D* hist2D : hist2Ds) {
      hist2D -> Write();
    }
  }
  return;

}  // end 'End()'



// private methods ------------------------------------------------------------

void ClustQAMaker::BuildHistograms() {

  // grab binning schemes
  std::vector<BinDef> vecBins = m_hist.GetVecHistBins();

  // histogram labels/definitions
  const std::vector<std::string> vecHistTypes  = {
    "Mvtx",
    "Intt",
    "Tpc",
    "All"
  };
  const std::vector<HistDef1D> vecHistDef1D = {
    std::make_tuple( "ClustPosX", vecBins.at(TrackJetQAMakerHistDef::Var::PosXY) ),
    std::make_tuple( "ClustPosY", vecBins.at(TrackJetQAMakerHistDef::Var::PosXY) ),
    std::make_tuple( "ClustPosZ", vecBins.at(TrackJetQAMakerHistDef::Var::PosZ) ),
    std::make_tuple( "ClustPosR", vecBins.at(TrackJetQAMakerHistDef::Var::PosR) )
  };
  const std::vector<HistDef2D> vecHistDef2D = {
    std::make_tuple(
      "ClustPosYvsX",
      vecBins.at(TrackJetQAMakerHistDef::Var::PosXY),
      vecBins.at(TrackJetQAMakerHistDef::Var::PosXY)
    ),
    std::make_tuple(
      "ClustPosRvsZ",
      vecBins.at(TrackJetQAMakerHistDef::Var::PosZ),
      vecBins.at(TrackJetQAMakerHistDef::Var::PosR)
    )
  };

  // build 1d histograms
  vecHist1D.resize( vecHistTypes.size() );
  for (size_t iType = 0; iType < vecHistTypes.size(); iType++) {
    for (HistDef1D histDef1D : vecHistDef1D) {

      // make name
      std::string sHistName("h");
      sHistName += vecHistTypes.at(iType);
      sHistName += std::get<0>(histDef1D);

      // create histogram
      vecHist1D.at(iType).push_back(
        new TH1D(
          sHistName.data(),
          "",
          std::get<1>(histDef1D).first,
          std::get<1>(histDef1D).second.first,
          std::get<1>(histDef1D).second.second
        )
      );
    }  // end hist loop
  }  // end type loop

  // build 2d histograms
  vecHist2D.resize( vecHistTypes.size() );
  for (size_t iType = 0; iType < vecHistTypes.size(); iType++) {
    for (HistDef2D histDef2D : vecHistDef2D) {

      // make name
      std::string sHistName("h");
      sHistName += vecHistTypes.at(iType);
      sHistName += std::get<0>(histDef2D);

      // create histogram
      vecHist2D.at(iType).push_back(
        new TH2D(
          sHistName.data(),
          "",
          std::get<1>(histDef2D).first,
          std::get<1>(histDef2D).second.first,
          std::get<1>(histDef2D).second.second,
          std::get<2>(histDef2D).first,
          std::get<2>(histDef2D).second.first,
          std::get<2>(histDef2D).second.second
        )
      );
    }  // end hist loop
  }  // end type loop
  return;

}  // end 'BuildHistograms()'



void ClustQAMaker::FillHistograms(Type type, ClustQAContent& content) {

  // fill 1d histograms
  vecHist1D.at(type).at(H1D::PosX) -> Fill(content.x);
  vecHist1D.at(type).at(H1D::PosY) -> Fill(content.y);
  vecHist1D.at(type).at(H1D::PosZ) -> Fill(content.z);
  vecHist1D.at(type).at(H1D::PosR) -> Fill(content.r);

  // fill 2d histograms
  vecHist2D.at(type).at(H2D::PosYvsX) -> Fill(content.x, content.y);
  vecHist2D.at(type).at(H2D::PosRvsZ) -> Fill(content.z, content.r);
  return;

}  //  end 'FillHistograms(Type, ClustQAContent&)'

// end ------------------------------------------------------------------------
