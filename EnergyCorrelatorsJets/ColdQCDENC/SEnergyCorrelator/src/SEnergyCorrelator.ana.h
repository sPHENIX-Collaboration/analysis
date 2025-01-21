/// ---------------------------------------------------------------------------
/*! \file    SEnergyCorrelator.ana.h
 *  \authors Derek Anderson, Alex Clarke
 *  \date    01.20.2023
 *
 *  A module to run ENC calculations in the sPHENIX
 *  software stack for the Cold QCD EEC analysis.
 */
/// ---------------------------------------------------------------------------

#pragma once

// make common namespaces implicit
using namespace std;
using namespace fastjet;
using namespace ROOT::Math;



namespace SColdQcdCorrelatorAnalysis {

  // analysis methods =========================================================

  // --------------------------------------------------------------------------
  //! Run ENC calculations
  // --------------------------------------------------------------------------
  void SEnergyCorrelator::RunCalculations() {

    // print debug statement
    if (m_config.isDebugOn) PrintDebug(31);

    // helper lambda to turn a PtEtaPhiEVector into a PseudoJet
    auto getPseudoJet = [](const PtEtaPhiEVector& vector) {
      PseudoJet pseudo(
        vector.Px(),
        vector.Py(),
        vector.Pz(),
        vector.E()
      );
      return pseudo;
    };

    //clear vector for global correlator
    m_jetCalcVec.clear();

    // jet loop
    double normGlobal = 0.;
    for (uint64_t iJet = 0; iJet < m_input.jets.size(); iJet++) {

      // select jet pt bin & apply jet cuts
      const bool isGoodJet = IsGoodJet( m_input.jets[iJet] );
      if (!isGoodJet) continue;

      // create jet 4-vector, smear if need be
      PtEtaPhiEVector pJetVector(
        m_input.jets[iJet].GetPT(),
        m_input.jets[iJet].GetEta(),
        m_input.jets[iJet].GetPhi(),
        m_input.jets[iJet].GetEne()
      );
      if (m_config.isInTreeTruth && m_config.doJetSmear) SmearJetMomentum(pJetVector);

      // create jet pseudojet
      PseudoJet jet = getPseudoJet(pJetVector);
      jet.set_user_index(iJet);

      // increment normalization & load global calc vector, if needed
      if (m_config.doGlobalCalc) {
        normGlobal += GetWeight(pJetVector, m_config.normOption);
        m_jetCalcVec.push_back(jet);
      }

      // run local calculation if needed
      if (m_config.doLocalCalc) {
        m_cstCalcVec.clear();
      } else {
        continue;
      }

      // constituent loop
      for (uint64_t iCst = 0; iCst < m_input.csts[iJet].size(); iCst++) {

        // if needed, apply constituent cuts
        const bool isGoodCst = IsGoodCst( m_input.csts[iJet][iCst] );
        if (!isGoodCst) continue;

        // create cst 4-vector, smear if need be
        PtEtaPhiEVector pCstVector(
          m_input.csts[iJet][iCst].GetPT(),
          m_input.csts[iJet][iCst].GetEta(),
          m_input.csts[iJet][iCst].GetPhi(),
          m_input.csts[iJet][iCst].GetEne()
        );
        if (m_config.isInTreeTruth && m_config.doCstSmear) SmearCstMomentum(pCstVector);

        // apply efficiency if need be
        if (m_config.doCstEff) {
          const bool survives = SurvivesEfficiency(pCstVector.Pt());
          if (!survives) continue;
        }

        // create cst pseudojet
        PseudoJet constituent = getPseudoJet(pCstVector);
        constituent.set_user_index(iCst);

        // add to list
        m_cstCalcVec.push_back(constituent);

      }  // end cst loop

      // run local eec computation(s)
      if (m_config.doPackageCalc) {
        DoLocalCalcWithPackage( pJetVector.Pt() );
      }
      if (m_config.doManualCalc) {
	DoLocalCalcManual( pJetVector );
      }
    }  // end jet loop

    // run global calculations
    if (m_config.doGlobalCalc) {
      DoGlobalCalcManual( normGlobal );
    }
    return;

  }  // end 'DoLocalCalculation()'



  // --------------------------------------------------------------------------
  //! Do global (jet-jet) ENC calculations
  // --------------------------------------------------------------------------
  void SEnergyCorrelator::DoGlobalCalcManual(const double normGlobal) {
    
    // print debug statement
    if (m_config.isDebugOn) PrintDebug(38);

    // 1st jet loop
    for (uint64_t iJetA = 0; iJetA < m_jetCalcVec.size(); ++iJetA) {

      // get weight for jet A
      const double weightA = GetWeight(
        PtEtaPhiEVector(
          m_jetCalcVec[iJetA].pt(),
          m_jetCalcVec[iJetA].eta(),
          m_jetCalcVec[iJetA].phi(),
          m_jetCalcVec[iJetA].E()
        ),
        m_config.momOption
      );

      // 2nd jet loop
      for (uint64_t iJetB = 0; iJetB < m_jetCalcVec.size(); ++iJetB) {

        // get weight for jet B
        const double weightB = GetWeight(
          PtEtaPhiEVector(
            m_jetCalcVec[iJetB].pt(),
            m_jetCalcVec[iJetB].eta(),
            m_jetCalcVec[iJetB].phi(),
            m_jetCalcVec[iJetB].E()
          ),
          m_config.momOption
        );

        // now calculate overall weight and delta-phi
        const double teec = (weightA * weightB) / (normGlobal * normGlobal);
        const double dphi = std::remainder(
          m_jetCalcVec[iJetA].phi() - m_jetCalcVec[iJetB].phi(),
          TMath::TwoPi()
        );
        const double cosdf = std::cos(dphi);

        // fill histograms
        // FIXME move to a dedicated histogram collection
        for (size_t iHtBin = 0; iHtBin < m_config.htEvtBins.size(); ++iHtBin) {
          const bool isInHtBin = (
            (normGlobal >= m_config.htEvtBins[iHtBin].first) &&
            (normGlobal < m_config.htEvtBins[iHtBin].second)
          );
          if (isInHtBin) {
            m_outGlobalHistDPhiAxis[iHtBin]  -> Fill(dphi, teec);
            m_outGlobalHistCosDFAxis[iHtBin] -> Fill(cosdf, teec);
          }
        } //end of HT bin loop

      }  // end jet loop B
    }  // end jet loop A
    return;

  }  // end 'DoGlobalCalcManual(double)'



  // --------------------------------------------------------------------------
  //! Run local (in-jet) calculations using P. T. Komiske's EEC package
  // --------------------------------------------------------------------------
  void SEnergyCorrelator::DoLocalCalcWithPackage(const double ptJet) {

    // print debug statement
    if (m_config.isDebugOn) PrintDebug(31);

    const int32_t iPtJetBin = GetJetPtBin( ptJet );
    const bool    foundBin  = (iPtJetBin >= 0);
    const bool    hasCsts   = (m_cstCalcVec.size() > 0); 
    if (foundBin && hasCsts) {
      m_eecLongSide.at(iPtJetBin) -> compute(m_cstCalcVec);
    }
    return;

  }  // end 'DoLocalCalcWithPackage(double)'



  // --------------------------------------------------------------------------
  //! Run local (in-jet) calculations manually
  // --------------------------------------------------------------------------
  void SEnergyCorrelator::DoLocalCalcManual(const PtEtaPhiEVector& normalization) {

    // print debug statement
    if (m_config.isDebugOn) PrintDebug(39);

    // get normalization for weights
    double norm = GetWeight(normalization, m_config.normOption);

    // 1s cst loop
    for (uint64_t iCst = 0; iCst < m_cstCalcVec.size(); iCst++) {

      // get weight for cst A
      const double weightA = GetWeight(
        PtEtaPhiEVector(
          m_cstCalcVec[iCst].pt(),
          m_cstCalcVec[iCst].eta(),
          m_cstCalcVec[iCst].phi(),
          m_cstCalcVec[iCst].E()
        ),
        m_config.momOption,
        normalization
      );

      // 2nd cst loop
      for (uint64_t jCst = 0; jCst < m_cstCalcVec.size(); jCst++){

        // get weight for cst B
        const double weightB = GetWeight(
	  PtEtaPhiEVector(
	    m_cstCalcVec[jCst].pt(),
            m_cstCalcVec[jCst].eta(),
	    m_cstCalcVec[jCst].phi(),
            m_cstCalcVec[jCst].E()
          ),
          m_config.momOption,
          normalization
        );

        // calculate distance b/n cst.s A-B
        const double drCstAB = std::hypot(
          m_cstCalcVec[iCst].rap() - m_cstCalcVec[jCst].rap(),
          std::remainder(
            m_cstCalcVec[iCst].phi() - m_cstCalcVec[jCst].phi(),
            TMath::TwoPi()
          )
        );

        // now calculate 2-point weight & fill relevant histograms
        // FIXME move to a dedicated histogram collection
        const double eecWeight = (weightA * weightB) / (norm * norm);
        for (size_t iPtBin = 0; iPtBin < m_config.ptJetBins.size(); iPtBin++) {
          const bool isInPtBin = (
            (normalization.Pt() >= m_config.ptJetBins[iPtBin].first) &&
            (normalization.Pt() < m_config.ptJetBins[iPtBin].second)
          );
          if (isInPtBin) {
            m_outManualHistErrDrAxis[iPtBin] -> Fill(drCstAB, eecWeight);
          }
        } //end of pT bin loop

        // if not doing 3-point, continue on in loop
        /* FIXME in prepping for multiple n per calc..
         *   - should make nPoints a set
         *   - might want to move this to a separate function...
         *   - also can remove the 3 point flag at that point, e.g.
         *       if (!m_config.nPoints.count(3)) continue;
         */  
        if (!m_config.doThreePoint) continue;

        // 3rd cst loop
        for (uint64_t kCst = 0; kCst < m_cstCalcVec.size(); kCst++) {

          const double weightC = GetWeight(
	    PtEtaPhiEVector(
	      m_cstCalcVec[kCst].pt(),
              m_cstCalcVec[kCst].eta(),
	      m_cstCalcVec[kCst].phi(),
              m_cstCalcVec[kCst].E()
            ),
            m_config.momOption,
            normalization
          );

          // calculate distances b/n cst.s A-C, B-C
          const double drCstAC = std::hypot(
            m_cstCalcVec[iCst].rap() - m_cstCalcVec[kCst].rap(),
            std::remainder(
              m_cstCalcVec[iCst].phi() - m_cstCalcVec[kCst].phi(),
              TMath::TwoPi()
            )
          );
          const double drCstBC = std::hypot(
            m_cstCalcVec[jCst].rap() - m_cstCalcVec[kCst].rap(),
            std::remainder(
              m_cstCalcVec[jCst].phi() - m_cstCalcVec[kCst].phi(),
              TMath::TwoPi()
            )
          );

          // calculate 3-point weight & RL/M/S
          const double e3cWeight = (weightA * weightB * weightC) / (norm * norm * norm);
          const double RL        = std::max(std::max(drCstAB, drCstAC), drCstBC);
          const double RS        = std::min(std::min(drCstAB, drCstAC), drCstBC);
	  
	  // fill Projected E3C histograms
	  // FIXME move to a dedicated histogram collection
	  for (size_t iPtBin = 0; iPtBin < m_config.ptJetBins.size(); iPtBin++) {
	    const bool isInPtBin = (
              (normalization.Pt() >= m_config.ptJetBins[iPtBin].first) &&
              (normalization.Pt() < m_config.ptJetBins[iPtBin].second)
            );
	    if (isInPtBin) {
              m_outProjE3C[iPtBin] -> Fill(RL, e3cWeight);
	    }
          }  //end of ptBin loop

          // grab RM
          const double RM = GetRM( std::make_tuple(drCstAB, drCstAC, drCstBC) );

	  // skip case where RS or RM are 0
	  if (RS == 0 || RM == 0) continue;

	  // calculate shape parameters
	  const double xi  = RS / RM;
	  const double phi = std::asin(sqrt(1. - pow(RL - RM, 2.) / (RS * RS)));

          // fill shape-dependent E3c histograms
          // FIXME move to a dedicated histogram collection
          for (size_t iPtBin = 0; iPtBin < m_config.ptJetBins.size(); iPtBin++) {

            // check if in pt bin
            const bool isInPtBin = (
              (normalization.Pt() >= m_config.ptJetBins[iPtBin].first) &&
              (normalization.Pt() < m_config.ptJetBins[iPtBin].second)
            );
            if (!isInPtBin) continue;

            // loop over RL bins
            for (size_t jRLBin = 0; jRLBin < m_config.rlBins.size(); jRLBin++) {

              // check if in RL bin
              const bool isInRLBin = (
                (RL >= m_config.rlBins[jRLBin].first) &&
                (RL < m_config.rlBins[jRLBin].second)
              );
              if (!isInRLBin) continue;

              // fill hist
              m_outE3C[iPtBin][jRLBin]->Fill(xi, phi, e3cWeight);

            }  // end of rl bin loop
          } // end of ptBin loop
        }  // end of 3rd cst loop
      }  // end of 2nd cst loop
    }  //end of 1st cst loop
    return;

  }  // end 'DoLocalCalcManual(vector<fastjet::PseudoJet>, PtEtaPhiEVector)'



  // --------------------------------------------------------------------------
  //! Extract histograms from ENC package
  // --------------------------------------------------------------------------
  /* FIXME move to a dedicated histogram collection */
  void SEnergyCorrelator::ExtractHistsFromCorr() {

    // print debug statement
    if (m_config.isDebugOn) PrintDebug(25);

    vector<double>                       drBinEdges;
    vector<double>                       lnDrBinEdges;
    pair<vector<double>, vector<double>> histContentAndVariance;
    pair<vector<double>, vector<double>> histContentAndError;

    for (size_t iPtBin = 0; iPtBin < m_config.ptJetBins.size(); iPtBin++) {

      // create names
      TString sPtJetBin("_ptJet");
      sPtJetBin += floor(m_config.ptJetBins[iPtBin].first);

      TString sVarDrAxisName("hPackageCorrelatorVarianceDrAxis");
      TString sErrDrAxisName("hPackageCorrelatorErrorDrAxis");
      TString sVarLnDrAxisName("hPackageCorrelatorVarianceLnDrAxis");
      TString sErrLnDrAxisName("hPackageCorrelatorErrorLnDrAxis");

      sVarDrAxisName.Append(sPtJetBin.Data());
      sErrDrAxisName.Append(sPtJetBin.Data());
      sVarLnDrAxisName.Append(sPtJetBin.Data());
      sErrLnDrAxisName.Append(sPtJetBin.Data());

      // clear vectors
      drBinEdges.clear();
      lnDrBinEdges.clear();
      histContentAndVariance.first.clear();
      histContentAndVariance.second.clear();
      histContentAndError.first.clear();
      histContentAndError.second.clear();

      // grab bin edges, content, and error
      drBinEdges             = m_eecLongSide[iPtBin] -> bin_edges();
      histContentAndVariance = m_eecLongSide[iPtBin] -> get_hist_vars();
      histContentAndError    = m_eecLongSide[iPtBin] -> get_hist_errs();

      // create ln(dr) bin edges and arrays
      const size_t nDrBinEdges = drBinEdges.size();
      for (size_t iDrEdge = 0; iDrEdge < nDrBinEdges; iDrEdge++) {
        const double drEdge   = drBinEdges.at(iDrEdge);
        const double lnDrEdge = log(drEdge);
        lnDrBinEdges.push_back(lnDrEdge);
      }

      // bin edge arrays for histograms
      double drBinEdgeArray[nDrBinEdges];
      double lnDrBinEdgeArray[nDrBinEdges];
      for (size_t iDrEdge = 0; iDrEdge < nDrBinEdges; iDrEdge++) {
        drBinEdgeArray[iDrEdge]   = drBinEdges.at(iDrEdge);
        lnDrBinEdgeArray[iDrEdge] = lnDrBinEdges.at(iDrEdge);
      }

      // make sure number of bin edges is reasonable
      const bool isNumBinEdgesGood = ((nDrBinEdges - 1) == m_config.nBinsDr);
      if (!isNumBinEdgesGood) {
        PrintError(12, nDrBinEdges);
        assert(isNumBinEdgesGood);
      }

      // create output histograms
      m_outPackageHistVarDrAxis[iPtBin]   = new TH1D(sVarDrAxisName.Data(),   "", m_config.nBinsDr, drBinEdgeArray);
      m_outPackageHistErrDrAxis[iPtBin]   = new TH1D(sErrDrAxisName.Data(),   "", m_config.nBinsDr, drBinEdgeArray);
      m_outPackageHistVarLnDrAxis[iPtBin] = new TH1D(sVarLnDrAxisName.Data(), "", m_config.nBinsDr, lnDrBinEdgeArray);
      m_outPackageHistErrLnDrAxis[iPtBin] = new TH1D(sErrLnDrAxisName.Data(), "", m_config.nBinsDr, lnDrBinEdgeArray);
      m_outPackageHistVarDrAxis[iPtBin]   -> Sumw2();
      m_outPackageHistErrDrAxis[iPtBin]   -> Sumw2();
      m_outPackageHistVarLnDrAxis[iPtBin] -> Sumw2();
      m_outPackageHistErrLnDrAxis[iPtBin] -> Sumw2();

      // set bin content
      for (size_t iDrEdge = 0; iDrEdge < m_config.nBinsDr; iDrEdge++) {
        const size_t iDrBin        = iDrEdge;
        const double binVarContent = histContentAndVariance.first.at(iDrEdge);
        const double binVarError   = histContentAndVariance.second.at(iDrEdge);
        const double binErrContent = histContentAndError.first.at(iDrEdge);
        const double binErrError   = histContentAndError.second.at(iDrEdge);

        // check if bin with variances is good & set content/error
        const bool areVarBinValuesNans = (isnan(binVarContent) || isnan(binVarError));
        if (areVarBinValuesNans) {
          PrintError(13, 0, iDrBin);
        } else {
          m_outPackageHistVarDrAxis[iPtBin]   -> SetBinContent(iDrBin, binVarContent);
          m_outPackageHistVarLnDrAxis[iPtBin] -> SetBinContent(iDrBin, binVarContent);
          m_outPackageHistVarDrAxis[iPtBin]   -> SetBinError(iDrBin, binVarError);
          m_outPackageHistVarLnDrAxis[iPtBin] -> SetBinError(iDrBin, binVarError);
        }

        // check if bin with errors is good & set content/error
        const bool areErrBinValuesNans = (isnan(binErrContent) || isnan(binErrError));
        if (areErrBinValuesNans) {
          PrintError(14, 0, iDrBin);
        } else {
          m_outPackageHistErrDrAxis[iPtBin]   -> SetBinContent(iDrBin, binErrContent);
          m_outPackageHistErrLnDrAxis[iPtBin] -> SetBinContent(iDrBin, binErrContent);
          m_outPackageHistErrDrAxis[iPtBin]   -> SetBinError(iDrBin, binErrError);
          m_outPackageHistErrLnDrAxis[iPtBin] -> SetBinError(iDrBin, binErrError);
        }
      }  // end dr bin edge loop
    }  // end jet pt bin loop

    PrintMessage(14);
    return;

  }  // end 'ExtractHistsFromCorr()'



  // --------------------------------------------------------------------------
  //! Smear jet momentum
  // --------------------------------------------------------------------------
  void SEnergyCorrelator::SmearJetMomentum(PtEtaPhiEVector& pJet) {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 7)) PrintDebug(35);

    // grab unsmeared jet pt, E & calculate mass
    const double ptOrig = pJet.Pt();
    const double mJet   = pJet.M();

    // apply smearing
    const double ptSmear = ptOrig + m_rando -> Gaus(0., m_config.ptJetSmear * ptOrig);
    const double eSmear  = sqrt( hypot(ptSmear, mJet) ); 

    // update 4-vector and exit
    pJet.SetPt(ptSmear);
    pJet.SetE(eSmear);
    return;

  }  // end 'SmearJetMomentum(PtEtaPhiEVector&)'



  // --------------------------------------------------------------------------
  //! Smear constituent momentum
  // --------------------------------------------------------------------------
  void SEnergyCorrelator::SmearCstMomentum(PtEtaPhiEVector& pCst) {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 7)) PrintDebug(36);

    // apply pt smearing, if need be
    const double ptOrig  = pCst.Pt();
    const double ptSmear = ptOrig + m_rando -> Gaus(0., m_config.ptCstSmear * ptOrig);
    if (m_config.doCstPtSmear) {
      pCst.SetPt(ptSmear);
    }

    // create TVector3 version of input
    //   - FIXME might be good to upgrade these to GenVectors
    TVector3 pCstVec3( pCst.X(), pCst.Y(), pCst.Z() );
    TVector3 pSmearVec3 = pCstVec3;

    // apply theta smearing, if need be
    const double thOrig  = pCstVec3.Theta();
    const double thSmear = thOrig + m_rando -> Gaus(0., m_config.thCstSmear);
    if (m_config.doCstThetaSmear) {
      pSmearVec3.SetTheta(thSmear);
    }

    // apply phi smearing, if need be
    const double phiRotate = m_rando -> Uniform(-TMath::Pi(), TMath::Pi());
    if (m_config.doCstPhiSmear) {
      pSmearVec3.Rotate(phiRotate, pCstVec3);
    }

    // update 4-vector and exit
    const double eSmear = hypot( pSmearVec3.Mag(), Const::MassPion() );
    if (m_config.doCstThetaSmear || m_config.doCstPhiSmear) {
      pCst.SetPt( pSmearVec3.Pt() );
      pCst.SetEta( pSmearVec3.Eta() );
      pCst.SetPhi( pSmearVec3.Phi() );
      pCst.SetE( eSmear );
    }
    return;

  }  // end 'SmearCstMomentum(PtEtaPhiEVector&)'



  // --------------------------------------------------------------------------
  //! Check if a jet satisfies cuts
  // --------------------------------------------------------------------------
  bool SEnergyCorrelator::IsGoodJet(const Types::JetInfo& jet) {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 7)) PrintDebug(26);

    const bool isGoodJet = jet.IsInAcceptance(m_config.jetAccept);
    return isGoodJet;

  }  // end 'IsGoodJet(Types::JetInfo&)'



  // --------------------------------------------------------------------------
  //! Check if a constituent satisfies cuts
  // --------------------------------------------------------------------------
  bool SEnergyCorrelator::IsGoodCst(const Types::CstInfo& cst) {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 7)) PrintDebug(27);

    // if analyzing truth tree and needed, check embedding ID
    bool isSubEvtGood = true;
    if (m_config.isInTreeTruth && m_config.selectSubEvts) {
      isSubEvtGood = Tools::IsSubEvtGood(
        cst.GetEmbedID(),
        m_config.subEvtOpt,
        m_config.isEmbed
      );
    }

    // if needed, apply cst. cuts
    bool isInCstAccept = true;
    if (m_config.applyCstCuts) {
      isInCstAccept = cst.IsInAcceptance(m_config.cstAccept);
    }

    const bool isGoodCst = (isSubEvtGood && isInCstAccept);
    return isGoodCst;

  }  // end 'IsGoodCst(Types::CstInfo&)'



  // --------------------------------------------------------------------------
  //! Check if a value (e.g. cst pt) passes efficiency
  // --------------------------------------------------------------------------
  bool SEnergyCorrelator::SurvivesEfficiency(const double value) {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 7)) PrintDebug(37);

    // apply efficiency if need be
    const float rand = m_rando          -> Uniform(0., 1.);
    const float eff  = m_config.funcEff -> Eval(value);

    // return if value passed
    return (rand <= eff);

  }  // end 'SurvivesEfficiency(double)'



  // --------------------------------------------------------------------------
  //! Get weight of a point (e.g. a constituent) wrt. a reference (e.g. a jet)
  // --------------------------------------------------------------------------
  /* FIXME this should be overloaded to accept both PtEtaPhiEVector and
   * PseudoJets...
   */
  double SEnergyCorrelator::GetWeight(
    const PtEtaPhiEVector& momentum,
    const int option,
    optional<PtEtaPhiEVector> reference
  ) {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 7)) PrintDebug(40);

    // calculate Et wrt reference if provided,
    // othwerwise use built-in value
    //   - FIXME might be good to swap out the TVector3
    //     for PtEtaPhiEVector
    double et = momentum.Et();
    if (reference.has_value()) {
      et = GetET(
        TVector3(
          momentum.Px(),
          momentum.Py(),
          momentum.Pz()
        ),
        TVector3(
          reference.value().Px(),
          reference.value().Py(),
          reference.value().Pz()
        )
      );
    }

    // select relevant component
    double weight = 1.;
    switch (option) {
      case Norm::Et:
        weight = et;
        break;
      case Norm::E:
        weight = momentum.E();
        break;
      case Norm::Pt:
        [[fallthrough]];
      default:
        weight = momentum.Pt();
        break;
    }
    return weight;

  }  // end 'GetWeight(PtEtaPhiEVector&, int&, optional<PtEtaPhiEVector>)'



  // --------------------------------------------------------------------------
  //! Get median distance from a triplet
  // --------------------------------------------------------------------------
  /* FIXME can probably replace this w/ clever use of some stl containers */
  double SEnergyCorrelator::GetRM(const tuple<double, double, double>& dists) {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 7)) PrintDebug(41);
  
    // check if first element is median
    if (
      ((get<0>(dists) >= get<1>(dists)) && (get<0>(dists) <= get<2>(dists))) ||
      ((get<0>(dists) <= get<1>(dists)) && (get<0>(dists) >= get<2>(dists)))
    ) {
      return get<0>(dists);
    }

    // check if second element is median
    if (
      ((get<1>(dists) >= get<0>(dists)) && (get<1>(dists) <= get<2>(dists))) ||
      ((get<1>(dists) <= get<0>(dists)) && (get<1>(dists) >= get<2>(dists)))
    ) {
      return get<1>(dists);
    }

    // check if third element is median
    if (
      ((get<2>(dists) >= get<0>(dists)) && (get<2>(dists) <= get<1>(dists))) ||
      ((get<2>(dists) <= get<0>(dists)) && (get<2>(dists) >= get<1>(dists)))
    ) {
      return get<2>(dists);
    }

    // by default return 1st element
    return get<0>(dists);

  }  // end 'GetRM(tuple<double, double, double>&)'



  // --------------------------------------------------------------------------
  //! Get energy transverse to a provided reference
  // --------------------------------------------------------------------------
  /* FIXME might be good to swap out the TVector3 for PtEtaPhiEVector */
  double SEnergyCorrelator::GetET(const TVector3& pMom, const TVector3& pRef) {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 7)) PrintDebug(42);

    // get component transverse to reference
    TVector3 pProduct    = ((pMom * pRef) / (pRef * pRef)) * pRef;
    TVector3 pTransverse = pMom - pProduct;

    // then return energy of transverse part
    return std::hypot(pTransverse.Mag(), Const::MassPion());

  }  // end 'GetET(TVector3&, TVector3&)'



  // --------------------------------------------------------------------------
  //! Get bin no. for a given jet pt
  // --------------------------------------------------------------------------
  /* FIXME move to a dedicated histogram collection */
  int32_t SEnergyCorrelator::GetJetPtBin(const double ptJet) {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 7)) PrintDebug(28);
 
    bool    isInPtBin = false;
    int32_t iJetPtBin = -1;
    for (size_t iPtBin = 0; iPtBin < m_config.ptJetBins.size(); iPtBin++) {
      isInPtBin = ((ptJet >= m_config.ptJetBins[iPtBin].first) && (ptJet < m_config.ptJetBins[iPtBin].second));
      if (isInPtBin) {
        iJetPtBin = iPtBin;
        break; 
      }
    }
    return iJetPtBin;

  }  // end 'GetJetPtBin(double)'

}  // end SColdQcdCorrelatorAnalysis namespace

// end ------------------------------------------------------------------------
