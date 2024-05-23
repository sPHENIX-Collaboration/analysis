/// ---------------------------------------------------------------------------
/*! \file   SCorrelatorResponseMaker.ana.h
 *  \author Derek Anderson
 *  \date   02.14.2023
 *
 * A module to match truth to reconstructed
 * jets/particles to derive corrections for
 * an n-point energy correlation strength
 * function.
 */
/// ---------------------------------------------------------------------------

#pragma once

using namespace std;



// analysis methods ===========================================================

namespace SColdQcdCorrelatorAnalysis {

  // --------------------------------------------------------------------------
  //! Match truth and reconstructed jets
  // --------------------------------------------------------------------------
  void SCorrelatorResponseMaker::DoMatching() {

    // print debug statement
    if (m_config.inDebugMode && (m_config.verbosity > 1)) {
      PrintDebug(10);
    }

    // for storing jet info
    //   - FIXME all bookkeeping containers should be made members
    vector<uint32_t> vecTrueJetID;
    vector<uint32_t> vecRecoJetID;
    vector<uint64_t> vecTrueNumCst;
    vector<uint64_t> vecRecoNumCst;
    vector<double>   vecTrueJetEne;
    vector<double>   vecRecoJetEne;
    vector<double>   vecTrueJetPt;
    vector<double>   vecRecoJetPt;
    vector<double>   vecTrueJetEta;
    vector<double>   vecRecoJetEta;
    vector<double>   vecTrueJetPhi;
    vector<double>   vecRecoJetPhi;
    vector<double>   vecTrueJetArea;
    vector<double>   vecRecoJetArea;

    // for jet matching
    pair<vector<uint32_t>, vector<uint32_t>> vecJetID;
    pair<vector<uint64_t>, vector<uint64_t>> vecJetNum;

    // for constituent matching
    pair<vector<int>,    vector<int>>    vecCstID;
    pair<vector<double>, vector<double>> vecCstZ;
    pair<vector<double>, vector<double>> vecCstDr;
    pair<vector<double>, vector<double>> vecCstEne;
    pair<vector<double>, vector<double>> vecCstJt;
    pair<vector<double>, vector<double>> vecCstEta;
    pair<vector<double>, vector<double>> vecCstPhi;

    // get no. of events
    const uint64_t                 nTrueEvts = m_inTrueTree -> GetEntriesFast();
    const uint64_t                 nRecoEvts = m_inRecoTree -> GetEntriesFast();
    const pair<uint64_t, uint64_t> nEvts     = {nTrueEvts, nRecoEvts};
    PrintMessage(6, 0, nEvts);

    // loop over true events
    uint64_t nTrueBytes = 0;
    uint64_t nRecoBytes = 0;
    for (uint64_t iTrueEvt = 0; iTrueEvt < nTrueEvts; iTrueEvt++) {

      // clear jet vectors
      //   - FIXME all of this should be consolidated into a method
      vecTrueJetID.clear();
      vecRecoJetID.clear();
      vecTrueNumCst.clear();
      vecRecoNumCst.clear();
      vecTrueJetEne.clear();
      vecRecoJetEne.clear();
      vecTrueJetPt.clear();
      vecRecoJetPt.clear();
      vecTrueJetEta.clear();
      vecRecoJetEta.clear();
      vecTrueJetPhi.clear();
      vecRecoJetPhi.clear();
      vecTrueJetArea.clear();
      vecRecoJetArea.clear();

      // clear constituent matching vectors
      vecCstID.first.clear();
      vecCstID.second.clear();
      vecCstZ.first.clear();
      vecCstZ.second.clear();
      vecCstDr.first.clear();
      vecCstDr.second.clear();
      vecCstEne.first.clear();
      vecCstEne.second.clear();
      vecCstJt.first.clear();
      vecCstJt.second.clear();
      vecCstEta.first.clear();
      vecCstEta.second.clear();
      vecCstPhi.first.clear();
      vecCstPhi.second.clear();

      // clear input/output addresses
      InitializeAddresses();

      // load true entry
      const uint64_t trueEntry = Interfaces::LoadTree(iTrueEvt, m_inTrueTree, m_fTrueCurrent);
      if (trueEntry < 0) break;

      const uint64_t trueBytes = Interfaces::GetEntry(iTrueEvt, m_inTrueTree);
      if (trueBytes < 0) {
        PrintError(4, iTrueEvt);
        break;
      } else {
        nTrueBytes += trueBytes;
        PrintMessage(7, iTrueEvt + 1, nEvts);
      }

      // load reco entry
      //   - FIXME should the reco event be located via an event ID first?
      const uint64_t recoEntry = Interfaces::LoadTree(iTrueEvt, m_inRecoTree, m_fRecoCurrent);
      if (recoEntry < 0) break;

      const uint64_t recoBytes = Interfaces::GetEntry(iTrueEvt, m_inRecoTree);
      if (recoBytes < 0) {
        PrintError(5, iTrueEvt);
        break;
      } else {
        nRecoBytes += recoBytes;
      }

      // set event info
      //   - FIXME move to output struct definition
      m_matchNumJets = make_pair(m_trueNumJets,     m_recoNumJets);
      m_matchNumTrks = make_pair(m_trueNumChrgPars, m_recoNumTrks);
      m_matchVtxX    = make_pair(m_trueVtxX,        m_recoVtxX);
      m_matchVtxY    = make_pair(m_trueVtxY,        m_recoVtxY);
      m_matchVtxZ    = make_pair(m_trueVtxZ,        m_trueVtxZ);

      // loop over true jets
      for (int iTrueJet = 0; iTrueJet < m_trueNumJets; iTrueJet++) {

        // store true jet values
        vecTrueJetID.push_back(m_trueJetID      -> at(iTrueJet));
        vecTrueNumCst.push_back(m_trueJetNumCst -> at(iTrueJet));
        vecTrueJetEne.push_back(m_trueJetEne    -> at(iTrueJet));
        vecTrueJetPt.push_back(m_trueJetPt      -> at(iTrueJet));
        vecTrueJetEta.push_back(m_trueJetEta    -> at(iTrueJet));
        vecTrueJetPhi.push_back(m_trueJetPhi    -> at(iTrueJet));
        vecTrueJetArea.push_back(m_trueJetArea  -> at(iTrueJet));

        // get constituent values
        const uint64_t nTrueCsts = m_trueJetNumCst -> at(iTrueJet);
        for (uint64_t iTrueCst = 0; iTrueCst < nTrueCsts; iTrueCst++) {

          // TODO store cst info
          /*
          vecTrueCstID.push_back((m_trueCstID   -> at(iTrueJet)).at(iTrueCst));
          vecTrueCstZ.push_back((m_trueCstZ     -> at(iTrueJet)).at(iTrueCst));
          vecTrueCstDr.push_back((m_trueCstDr   -> at(iTrueJet)).at(iTrueCst));
          vecTrueCstEne.push_back((m_trueCstEne -> at(iTrueJet)).at(iTrueCst));
          vecTrueCstJt.push_back((m_trueCstJt   -> at(iTrueJet)).at(iTrueCst));
          vecTrueCstEta.push_back((m_trueCstEta -> at(iTrueJet)).at(iTrueCst));
          vecTrueCstPhi.push_back((m_trueCstPhi -> at(iTrueJet)).at(iTrueCst));
          */

        }  // end true cst loop

        // loop over reco jets
        int      iMatchJet        = -1;
        bool     jetWasMatched    = false;
        double   bestFracCstMatch = 0.;
        uint64_t nMatchCst        = 0;
        for (int iRecoJet = 0; iRecoJet < m_recoNumJets; iRecoJet++) {

          /* TODO add an option to exclude jets if they're too far away in space/pt */

          // grab no. of constituents
          const uint64_t nRecoCsts = m_recoJetNumCst -> at(iRecoJet);

          // loop over true constituents
          for (uint64_t iTrueCst = 0; iTrueCst < nTrueCsts; iTrueCst++) {

            // grab true constituent ID
            const int trueCstID = (m_trueCstID -> at(iTrueJet)).at(iTrueCst);

            // loop over reco constituents
            for (uint64_t iRecoCst = 0; iRecoCst < nRecoCsts; iRecoCst++) {

              // check ids
              const int  recoCstID    = (m_recoCstMatchID -> at(iRecoJet)).at(iRecoCst);
              const bool isMatchedCst = (trueCstID == recoCstID);
              if (isMatchedCst) {
                /* TODO will need to store map from true-to-reco cst.s */
                ++nMatchCst;
                break;
              }
            }  // end reco cst loop
          }  // end true cst loop
          const double fracCstMatch = nMatchCst / nTrueCsts;

          // if jet is best match so far, store values
          const bool didJetMatch      = ((fracCstMatch > m_fracCstMatchRange.first) && (fracCstMatch > m_fracCstMatchRange.second));
          const bool isJetBetterMatch = (fracCstMatch > bestFracCstMatch);
          if (didJetMatch && isJetBetterMatch) {
            iMatchJet     = iRecoJet;
            jetWasMatched = true;
          }
        }  // end reco jet loop

        // add matched reco jet values to vectors
        if (jetWasMatched) {
          vecRecoJetID.push_back(m_recoJetID      -> at(iMatchJet));
          vecRecoNumCst.push_back(m_recoJetNumCst -> at(iMatchJet));
          vecRecoJetEne.push_back(m_recoJetEne    -> at(iMatchJet));
          vecRecoJetPt.push_back(m_recoJetPt      -> at(iMatchJet));
          vecRecoJetEta.push_back(m_recoJetEta    -> at(iMatchJet));
          vecRecoJetPhi.push_back(m_recoJetPhi    -> at(iMatchJet));
          vecRecoJetArea.push_back(m_recoJetArea  -> at(iMatchJet));
        } else {
          vecRecoJetID.push_back(9999);
          vecRecoNumCst.push_back(0);
          vecRecoJetEne.push_back(-9999.);
          vecRecoJetPt.push_back(-9999.);
          vecRecoJetEta.push_back(-9999.);
          vecRecoJetPhi.push_back(-9999.);
          vecRecoJetArea.push_back(-9999.);
        }
      }  // end true jet loop

      // set jet & cst info
      m_matchJetID     = make_pair(vecTrueJetID,   vecRecoJetID);
      m_matchJetNumCst = make_pair(vecTrueNumCst,  vecRecoNumCst);
      m_matchJetEne    = make_pair(vecTrueJetEne,  vecRecoJetEne);
      m_matchJetPt     = make_pair(vecTrueJetPt,   vecRecoJetPt);
      m_matchJetEta    = make_pair(vecTrueJetEta,  vecRecoJetEta);
      m_matchJetPhi    = make_pair(vecTrueJetPhi,  vecRecoJetPhi);
      m_matchJetArea   = make_pair(vecTrueJetArea, vecRecoJetArea);

      // fill tree
      m_matchTree -> Fill();

    }  // end true event loop
    PrintMessage(8);

  }  // end 'DoMatching()'

}  // end SColdQcdCorrelatorAnalysis namespace

// end ------------------------------------------------------------------------
