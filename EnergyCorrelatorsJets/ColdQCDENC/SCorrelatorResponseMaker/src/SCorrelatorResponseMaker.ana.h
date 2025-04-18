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

    // get no. of events
    const uint64_t                 nTrueEvts = m_inTrueTree -> GetEntriesFast();
    const uint64_t                 nRecoEvts = m_inRecoTree -> GetEntriesFast();
    const pair<uint64_t, uint64_t> nEvts     = {nTrueEvts, nRecoEvts};
    PrintMessage(6, 0, nEvts);

    // loop over true events
    uint64_t nTrueBytes = 0;
    uint64_t nRecoBytes = 0;
    for (uint64_t iTrueEvt = 0; iTrueEvt < nTrueEvts; iTrueEvt++) {

      // load true entry
      //   - FIXME event loop should go into the top-level analysis method
      const uint64_t trueEntry = Interfaces::LoadTree(m_inTrueTree, iTrueEvt, m_fTrueCurrent);
      if (trueEntry < 0) break;

      const uint64_t trueBytes = Interfaces::GetEntry(m_inTrueTree, iTrueEvt);
      if (trueBytes < 0) {
        PrintError(4, iTrueEvt);
        break;
      } else {
        nTrueBytes += trueBytes;
        PrintMessage(7, iTrueEvt + 1, nEvts);
      }

      // load reco entry
      //   - FIXME should the reco event be located via an event ID first?
      const uint64_t recoEntry = Interfaces::LoadTree(m_inRecoTree, iTrueEvt, m_fRecoCurrent);
      if (recoEntry < 0) break;

      const uint64_t recoBytes = Interfaces::GetEntry(m_inRecoTree, iTrueEvt);
      if (recoBytes < 0) {
        PrintError(5, iTrueEvt);
        break;
      } else {
        nRecoBytes += recoBytes;
      }

      // is uing legacy input, fill container
      if (m_config.isLegacyIO) {
        m_recoLegacy.SetInput(m_recoInput);
        m_trueLegacy.SetInput(m_trueInput);
      }

      // clear output addresses
      ResetOutVariables();

      /* TODO grab event-level info here */

      // loop over true jets
      for (size_t iTrueJet = 0; iTrueJet < m_trueInput.jets.size(); ++iTrueJet) {

        /* TODO grab truth jet-level info here */

        // get constituent values
        const uint64_t nTrueCsts = m_trueInput.csts.at(iTrueJet).size();
        for (Types::CstInfo& genCst : m_trueInput.csts.at(iTrueJet)) {
          m_output.cstGen.at(iTrueJet).push_back( genCst );
        }  // end true cst loop

        // loop over reco jets
        //   - TODO need bookkeeping container to keep track of matched
        //     reco jets...
        bool     jetWasMatched    = false;
        double   bestFracCstMatch = 0.;
        uint64_t nMatchCst        = 0;
        for (size_t iRecoJet = 0; iRecoJet < m_recoInput.jets.size(); ++iRecoJet) {

          /* TODO add an option to exclude jets if they're too far away in space/pt */

          // loop over true constituents
          //   - TODO need bookkeeping container to keep track of matched
          //     reco constituents...
          for (Types::CstInfo& genCst : m_trueInput.csts.at(iTrueJet)) {

            // loop over reco constituents
            for (Types::CstInfo& recCst : m_recoInput.csts.at(iRecoJet)) {

              // check ids
              const bool isMatchedCst = (genCst.GetCstID() == recCst.GetCstID());
              if (isMatchedCst) {

                /* TODO will need to store map from true-to-reco cst.s */
                ++nMatchCst;
                break;

              }
            }  // end reco cst loop
          }  // end true cst loop

          // if jet is best match so far, store values
          const double fracCstMatch = nMatchCst / nTrueCsts;
          const bool   isBetterMatch = IsBetterMatch(fracCstMatch, bestFracCstMatch);
          if (isBetterMatch) {
            jetWasMatched = true;
          }
        }  // end reco jet loop

        // add matched reco jet values to vectors
        if (jetWasMatched) {
          /* TODO set matched reco jet values here */
        } else {
          /* TODO set placeholder values if not matched */
        }
      }  // end true jet loop

      // fill output tree
      FillTree();

    }  // end true event loop
    PrintMessage(8);

  }  // end 'DoMatching()'



  // --------------------------------------------------------------------------
  //! Check if current match is best so far
  // --------------------------------------------------------------------------
  bool SCorrelatorResponseMaker::IsBetterMatch(const double frac, const double best) {

    // print debug statement
    if (m_config.inDebugMode && (m_config.verbosity > 2)) {
      PrintDebug(15);
    }

    const bool isMatch  = ((frac > m_config.fracCstMatchRange.first) && (frac < m_config.fracCstMatchRange.second));
    const bool isBetter = (frac > best);
    return (isMatch && isBetter);

  }  // end 'IsBetterMatch(double, double)'

}  // end SColdQcdCorrelatorAnalysis namespace

// end ------------------------------------------------------------------------
