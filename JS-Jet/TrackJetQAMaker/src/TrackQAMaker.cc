// ----------------------------------------------------------------------------
// 'TrackQAMaker.cc'
// Derek Anderson
// 03.25.2024
//
// A submodule for the TrackJetQAMaker module
// to generate QA plots for track hits
// ----------------------------------------------------------------------------

#define TRACKJETQAMAKER_TRACKQAMAKER_CC

// submodule definition
#include "TrackQAMaker.h"



// public methods -------------------------------------------------------------

void TrackQAMaker::Init(TrackJetQAMakerHistDef& hist, TrackJetQAMakerHelper& help) {

  // grab module utilities
  m_help = help;
  m_hist = hist;

  // make histograms
  BuildHistograms();
  return;

}  // end 'Init(TrackJetQAMakerHistDef&, TrackJetQAMakerHelper&)'



void TrackQAMaker::Process(PHCompositeNode* topNode) {

  // grab track map off the node tree
  m_trkMap = findNode::getClass<SvtxTrackMap>(topNode, "SvtxTrackMap");
  if (!m_trkMap) {
    std::cerr << PHWHERE << ": PANIC: couldn't grab track map from node tree!" << std::endl;
    assert(m_trkMap);
  }

  // loop over tracks
  for (
    SvtxTrackMap::Iter itTrk = m_trkMap -> begin();
    itTrk != m_trkMap -> end();
    ++itTrk
  ) {

    // grab track
    SvtxTrack* track = itTrk -> second;

    // collect track info
    TrackQAContent content = {
      .eta  = track -> get_eta(),
      .phi  = track -> get_phi(),
      .pt   = track -> get_pt(),
      .qual = track -> get_quality()
    };

    // fill histograms
    FillHistograms(Type::All, content);

  }  // end track loop
  return;

}  // end 'Process(PHCompositeNode*)'



void TrackQAMaker::End(TFile* outFile, std::string outDirName) {

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

}  // end 'End(TFile*, std::string)'



// private methods ------------------------------------------------------------

void TrackQAMaker::BuildHistograms() {

  // grab binning schemes
  std::vector<BinDef> vecBins = m_hist.GetVecHistBins();

  // histogram labels/definitions
  const std::vector<std::string> vecHistTypes  = {
    "All"
  };
  const std::vector<HistDef1D> vecHistDef1D = {
    std::make_tuple( "TrackEta",  vecBins.at(TrackJetQAMakerHistDef::Var::Eta) ),
    std::make_tuple( "TrackPhi",  vecBins.at(TrackJetQAMakerHistDef::Var::Phi) ),
    std::make_tuple( "TrackPt",   vecBins.at(TrackJetQAMakerHistDef::Var::Ene) ),
    std::make_tuple( "TrackQual", vecBins.at(TrackJetQAMakerHistDef::Var::Qual) )
  };
  const std::vector<HistDef2D> vecHistDef2D = {
    std::make_tuple(
      "TrackEtaVsPhi",
      vecBins.at(TrackJetQAMakerHistDef::Var::Phi),
      vecBins.at(TrackJetQAMakerHistDef::Var::Eta)
    ),
    std::make_tuple(
      "TrackPtVsQual",
      vecBins.at(TrackJetQAMakerHistDef::Var::Qual),
      vecBins.at(TrackJetQAMakerHistDef::Var::Ene)
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



void TrackQAMaker::FillHistograms(Type type, TrackQAContent& content) {

  // fill 1d histograms
  vecHist1D.at(type).at(H1D::Eta)  -> Fill(content.eta);
  vecHist1D.at(type).at(H1D::Phi)  -> Fill(content.phi);
  vecHist1D.at(type).at(H1D::Pt)   -> Fill(content.pt);
  vecHist1D.at(type).at(H1D::Qual) -> Fill(content.qual);

  // fill 2d histograms
  vecHist2D.at(type).at(H2D::EtaVsPhi) -> Fill(content.phi, content.eta);
  vecHist2D.at(type).at(H2D::PtVsQual) -> Fill(content.qual, content.pt);
  return;

}  //  end 'FillHistograms(Type, TrackQAContent&)'

// end ------------------------------------------------------------------------
