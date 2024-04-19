// ----------------------------------------------------------------------------
// 'HitQAMaker.cc'
// Derek Anderson
// 03.25.2024
//
// A submodule for the TrackJetQAMaker module
// to generate QA plots for track hits
// ----------------------------------------------------------------------------

#define TRACKJETQAMAKER_HITQAMAKER_CC

// submodule definition
#include "HitQAMaker.h"



// public methods -------------------------------------------------------------

void HitQAMaker::Init(TrackJetQAMakerHistDef& hist, TrackJetQAMakerHelper& help) {

  // grab module utilities
  m_help = help;
  m_hist = hist;

  // make histograms
  BuildHistograms();
  return;

}  // end 'Init(TrackJetQAMakerHistDef&, TrackJetQAMakerHelper&)'



void HitQAMaker::Process(PHCompositeNode* topNode) {

  /* TODO grab TPC geometry here */

  // grab hit map from node tree
  m_hitMap = findNode::getClass<TrkrHitSetContainer>(topNode, "TRKR_HITSET");
  if (!m_hitMap) {
    std::cerr << PHWHERE << ": PANIC: couldn't grab hit map from node tree!" << std::endl;
    assert(m_hitMap);
  }

  // loop over hit sets
  TrkrHitSetContainer::ConstRange hitSets = m_hitMap -> getHitSets();
  for (
    TrkrHitSetContainer::ConstIterator itSet = hitSets.first;
    itSet != hitSets.second;
    ++itSet
  ) {

    // grab hit set
    TrkrDefs::hitsetkey setKey = itSet -> first;
    TrkrHitSet*         set    = itSet -> second;

    // loop over all hits in hit set
    TrkrHitSet::ConstRange hits = set -> getHits();
    for (
      TrkrHitSet::ConstIterator itHit = hits.first;
      itHit != hits.second;
      ++itHit
    ) {

      // grab hit
      TrkrDefs::hitkey hitKey = itHit -> first;
      TrkrHit*         hit    = itHit -> second;

      // check which subsystem hit is in
      const uint16_t layer  = TrkrDefs::getLayer(setKey);
      const bool     isMvtx = m_help.IsInMvtx(layer);
      const bool     isIntt = m_help.IsInIntt(layer);
      const bool     isTpc  = m_help.IsInTpc(layer);

      // get phi and z values
      //   - FIXME should be more explicit about
      //     row/column vs. z/phi...
      uint16_t phiBin = std::numeric_limits<uint16_t>::max();
      uint16_t zBin   = std::numeric_limits<uint16_t>::max();
      if (isMvtx) {
        phiBin = MvtxDefs::getCol(hitKey);
        zBin   = MvtxDefs::getRow(hitKey);
      } else if (isIntt) {
        phiBin = InttDefs::getCol(hitKey);
        zBin   = InttDefs::getRow(hitKey);
      } else if (isTpc) {
        phiBin = TpcDefs::getPad(hitKey);
        /* TODO put in z calculation */
      }

      // collect hit info
      HitQAContent content {
        .ene    = hit -> getEnergy(),
        .adc    = hit -> getAdc(),
        .layer  = layer,
        .phiBin = phiBin,
        .zBin   = zBin
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
    }  // end hit loop
  }  // end hit set loop
  return;

}  // end 'Process(PHCompositeNode*)'



void HitQAMaker::End(TFile* outFile, std::string outDirName) {

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

void HitQAMaker::BuildHistograms() {

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
    std::make_tuple( "HitEne",    vecBins.at(TrackJetQAMakerHistDef::Var::Ene) ),
    std::make_tuple( "HitAdc",    vecBins.at(TrackJetQAMakerHistDef::Var::Adc) ),
    std::make_tuple( "HitLayer",  vecBins.at(TrackJetQAMakerHistDef::Var::Layer) ),
    std::make_tuple( "HitPhiBin", vecBins.at(TrackJetQAMakerHistDef::Var::PhiBin) ),
    std::make_tuple( "HitZBin",   vecBins.at(TrackJetQAMakerHistDef::Var::ZBin) )
  };
  const std::vector<HistDef2D> vecHistDef2D = {
    std::make_tuple(
      "HitEneVsLayer",
      vecBins.at(TrackJetQAMakerHistDef::Var::Layer),
      vecBins.at(TrackJetQAMakerHistDef::Var::Ene)
    ),
    std::make_tuple(
      "HitEneVsADC",
      vecBins.at(TrackJetQAMakerHistDef::Var::Adc),
      vecBins.at(TrackJetQAMakerHistDef::Var::Ene)
    ),
    std::make_tuple(
      "HitPhiVsZBin",
      vecBins.at(TrackJetQAMakerHistDef::Var::ZBin),
      vecBins.at(TrackJetQAMakerHistDef::Var::PhiBin)
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



void HitQAMaker::FillHistograms(Type type, HitQAContent& content) {

  // fill 1d histograms
  vecHist1D.at(type).at(H1D::Ene)    -> Fill(content.ene);
  vecHist1D.at(type).at(H1D::ADC)    -> Fill(content.adc);
  vecHist1D.at(type).at(H1D::Layer)  -> Fill(content.layer);
  vecHist1D.at(type).at(H1D::PhiBin) -> Fill(content.phiBin);
  vecHist1D.at(type).at(H1D::ZBin)   -> Fill(content.zBin);

  // fill 2d histograms
  vecHist2D.at(type).at(H2D::EneVsLayer) -> Fill(content.layer, content.ene);
  vecHist2D.at(type).at(H2D::EneVsADC)   -> Fill(content.adc, content.ene);
  vecHist2D.at(type).at(H2D::PhiVsZBin)  -> Fill(content.zBin, content.phiBin);
  return;

}  //  end 'FillHistograms(Type, HitQAContent&)'

// end ------------------------------------------------------------------------
