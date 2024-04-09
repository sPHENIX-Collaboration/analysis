// ----------------------------------------------------------------------------
// 'JetQAMaker.cc'
// Derek Anderson
// 03.25.2024
//
// A submodule for the TrackJetQAMaker module
// to generate QA plots for track jets
// ----------------------------------------------------------------------------

#define TRACKJETQAMAKER_JETQAMAKER_CC

// submodule definition
#include "JetQAMaker.h"



// public methods -------------------------------------------------------------

void JetQAMaker::Init(TrackJetQAMakerHistDef& hist, TrackJetQAMakerHelper& help) {

  // grab module utilities
  m_help = help;
  m_hist = hist;

  // make histograms
  BuildHistograms();
  return;

}  // end 'Init(TrackJetQAMakerHistDef&, TrackJetQAMakerHelper&)'



void JetQAMaker::Process(PHCompositeNode* topNode) {

  // grab jet map from node tree
  m_jetMap = findNode::getClass<JetContainer>(topNode, "AntiKt_Track_r04");
  if (!m_jetMap) {
    std::cerr << PHWHERE << ": PANIC: couldn't grab jet map from node tree!" << std::endl;
    assert(m_jetMap);
  }

  // grab track map off the node tree
  m_trkMap = findNode::getClass<SvtxTrackMap>(topNode, "SvtxTrackMap");
  if (!m_trkMap) {
    std::cerr << PHWHERE << ": PANIC: couldn't grab track map from node tree!" << std::endl;
    assert(m_trkMap);
  }

  // loop over jets
  for (
    uint64_t iJet = 0;
    iJet < m_jetMap -> size();
    ++iJet
  ) {

    // grab jet
    Jet* jet = m_jetMap -> get_jet(iJet);

    // loop over jet constituents
    auto csts = jet -> get_comp_vec();
    for (
      auto cst = csts.begin();
      cst != csts.end();
      ++cst
    ) {

      // skip if cst is not a track
      uint32_t src = cst -> first;
      if (src != Jet::SRC::TRACK) continue;

      // grab track
      SvtxTrack* track = m_trkMap -> get(cst -> second);

      // collect cst info
      CstQAContent cstContent {
        .ptCst   = track -> get_pt(),
        .qualCst = track -> get_quality()
      };

      // fill histograms
      FillHistograms(Type::All, cstContent);

    }  // end cst loop

    // collect jet info
    JetQAContent jetContent {
      .etaJet = jet -> get_eta(),
      .phiJet = jet -> get_phi(),
      .ptJet  = jet -> get_pt(),
      .ptSum  = std::numeric_limits<double>::max()  // TODO put in calc
    };

    // fill histograms
    FillHistograms(Type::All, jetContent);

  }  // end jet loop
  return;

}  // end 'Process(PHCompositeNode*)'



void JetQAMaker::End(TFile* outFile, std::string jetDirName, std::string cstDirName) {

  TDirectory* jetDir = outFile -> mkdir(jetDirName.data());
  if (!jetDir) {
    std::cerr << PHWHERE << ": PANIC: unable to make jet output directory!" << std::endl;
    assert(jetDir);
  }

  TDirectory* cstDir = outFile -> mkdir(cstDirName.data());
  if (!cstDir) {
    std::cerr << PHWHERE << ": PANIC: unable to make constituent output directory!" << std::endl;
    assert(cstDir);
  }

  // save jet histograms
  jetDir -> cd();
  for (auto hist1Ds : vecJetHist1D) {
    for (TH1D* hist1D : hist1Ds) {
      hist1D -> Write();
    }
  }
  for (auto hist2Ds : vecJetHist2D) {
    for (TH2D* hist2D : hist2Ds) {
      hist2D -> Write();
    }
  }

  // save constituent histograms
  cstDir -> cd();
  for (auto hist1Ds : vecCstHist1D) {
    for (TH1D* hist1D : hist1Ds) {
      hist1D -> Write();
    }
  }
  for (auto hist2Ds : vecCstHist2D) {
    for (TH2D* hist2D : hist2Ds) {
      hist2D -> Write();
    }
  }
  return;

}  // end 'End(TFile*, std::string, std::string)'



// private methods ------------------------------------------------------------

void JetQAMaker::BuildHistograms() {

  // grab binning schemes
  std::vector<BinDef> vecBins = m_hist.GetVecHistBins();

  // histogram labels
  const std::vector<std::string> vecHistTypes  = {
    "All"
  };

  // 1d histogram definitions
  const std::vector<HistDef1D> vecJetHistDef1D = {
    std::make_tuple( "JetEta", vecBins.at(TrackJetQAMakerHistDef::Var::Eta) ),
    std::make_tuple( "JetPhi", vecBins.at(TrackJetQAMakerHistDef::Var::Phi) ),
    std::make_tuple( "JetPt",  vecBins.at(TrackJetQAMakerHistDef::Var::Ene) ),
    std::make_tuple( "SumPt",  vecBins.at(TrackJetQAMakerHistDef::Var::Ene) )
  };
  const std::vector<HistDef1D> vecCstHistDef1D = {
    std::make_tuple( "CstPt",   vecBins.at(TrackJetQAMakerHistDef::Var::Ene) ),
    std::make_tuple( "CstQual", vecBins.at(TrackJetQAMakerHistDef::Var::Qual) )
  };

  // 2d histogram definitions
  const std::vector<HistDef2D> vecJetHistDef2D = {
    std::make_tuple(
      "JetPtVsEta",
      vecBins.at(TrackJetQAMakerHistDef::Var::Eta),
      vecBins.at(TrackJetQAMakerHistDef::Var::Ene)
    ),
    std::make_tuple(
      "JetVsSumPt",
      vecBins.at(TrackJetQAMakerHistDef::Var::Ene),
      vecBins.at(TrackJetQAMakerHistDef::Var::Ene)
    )
  };
  const std::vector<HistDef2D> vecCstHistDef2D = {
    std::make_tuple(
      "CstQualVsPt",
      vecBins.at(TrackJetQAMakerHistDef::Var::Ene),
      vecBins.at(TrackJetQAMakerHistDef::Var::Qual)
    ),
  };

  // build 1d histograms
  vecJetHist1D.resize( vecHistTypes.size() );
  vecCstHist1D.resize( vecHistTypes.size() );
  for (size_t iType = 0; iType < vecHistTypes.size(); iType++) {
 
    // build jet histograms
    for (HistDef1D jetHistDef1D : vecJetHistDef1D) {

      // make name
      std::string sHistName("h");
      sHistName += vecHistTypes.at(iType);
      sHistName += std::get<0>(jetHistDef1D);

      // create histogram
      vecJetHist1D.at(iType).push_back(
        new TH1D(
          sHistName.data(),
          "",
          std::get<1>(jetHistDef1D).first,
          std::get<1>(jetHistDef1D).second.first,
          std::get<1>(jetHistDef1D).second.second
        )
      );
    }  // end jet hist loop

    // build constituent histograms
    for (HistDef1D cstHistDef1D : vecCstHistDef1D) {

      // make name
      std::string sHistName("h");
      sHistName += vecHistTypes.at(iType);
      sHistName += std::get<0>(cstHistDef1D);

      // create histogram
      vecCstHist1D.at(iType).push_back(
        new TH1D(
          sHistName.data(),
          "",
          std::get<1>(cstHistDef1D).first,
          std::get<1>(cstHistDef1D).second.first,
          std::get<1>(cstHistDef1D).second.second
        )
      );
    }  // end cst hist loop
  }  // end type loop

  // build 2d histograms
  vecJetHist2D.resize( vecHistTypes.size() );
  vecCstHist2D.resize( vecHistTypes.size() );
  for (size_t iType = 0; iType < vecHistTypes.size(); iType++) {

    // build jet histograms
    for (HistDef2D jetHistDef2D : vecJetHistDef2D) {

      // make name
      std::string sHistName("h");
      sHistName += vecHistTypes.at(iType);
      sHistName += std::get<0>(jetHistDef2D);

      // create histogram
      vecJetHist2D.at(iType).push_back(
        new TH2D(
          sHistName.data(),
          "",
          std::get<1>(jetHistDef2D).first,
          std::get<1>(jetHistDef2D).second.first,
          std::get<1>(jetHistDef2D).second.second,
          std::get<2>(jetHistDef2D).first,
          std::get<2>(jetHistDef2D).second.first,
          std::get<2>(jetHistDef2D).second.second
        )
      );
    }  // end jet hist loop

    // build constituent histograms
    for (HistDef2D cstHistDef2D : vecCstHistDef2D) {

      // make name
      std::string sHistName("h");
      sHistName += vecHistTypes.at(iType);
      sHistName += std::get<0>(cstHistDef2D);

      // create histogram
      vecCstHist2D.at(iType).push_back(
        new TH2D(
          sHistName.data(),
          "",
          std::get<1>(cstHistDef2D).first,
          std::get<1>(cstHistDef2D).second.first,
          std::get<1>(cstHistDef2D).second.second,
          std::get<2>(cstHistDef2D).first,
          std::get<2>(cstHistDef2D).second.first,
          std::get<2>(cstHistDef2D).second.second
        )
      );
    }  // end cst hist loop
  }  // end type loop
  return;

}  // end 'BuildHistograms()'



void JetQAMaker::FillHistograms(Type type, JetQAContent& content) {

  // fill 1d histograms
  vecJetHist1D.at(type).at(J1D::JetEta) -> Fill(content.etaJet);
  vecJetHist1D.at(type).at(J1D::JetPhi) -> Fill(content.phiJet);
  vecJetHist1D.at(type).at(J1D::JetPt)  -> Fill(content.ptJet);
  vecJetHist1D.at(type).at(J1D::SumPt)  -> Fill(content.ptSum);

  // fill 2d histograms
  vecJetHist2D.at(type).at(J2D::JetPtVsEta) -> Fill(content.etaJet, content.ptJet);
  vecJetHist2D.at(type).at(J2D::JetVsSumPt) -> Fill(content.ptSum, content.ptJet);
  return;

}  //  end 'FillHistograms(Type, JetQAContent&)'



void JetQAMaker::FillHistograms(Type type, CstQAContent& content) {

  // fill 1d histograms
  vecCstHist1D.at(type).at(C1D::CstPt)   -> Fill(content.ptCst);
  vecCstHist1D.at(type).at(C1D::CstQual) -> Fill(content.qualCst);

  // fill 2d histograms
  vecCstHist2D.at(type).at(C2D::CstQualVsPt) -> Fill(content.ptCst, content.qualCst);
  return;

}  //  end 'FillHistograms(Type, CstQAContent&)'

// end ------------------------------------------------------------------------


