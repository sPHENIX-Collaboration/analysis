// ----------------------------------------------------------------------------
// 'SEnergyCorrelator.ana.h'
// Derek Anderson
// 02.14.2023
//
// A module to implement Peter Komiske's EEC library
// in the sPHENIX software stack for the Cold QCD
// Energy-Energy Correlator analysis.
// ----------------------------------------------------------------------------

#pragma once

// make common namespaces implicit
using namespace std;
using namespace fastjet;



namespace SColdQcdCorrelatorAnalysis {

  // analysis methods ---------------------------------------------------------

  void SEnergyCorrelator::DoLocalCalculation() {

    TF1* fEff = new TF1("fEff", "[0]*(1.0-TMath::Exp(-1.0*[1]*x))", 0, 100.0);
    fEff->SetParameter(0, m_config.effVal);
    fEff->SetParameter(1, 0.9);
    TDatime *date = new TDatime();
    TRandom2 *shift = new TRandom2(date->GetDate()*date->GetTime());
    
    // print debug statement
    if (m_config.isDebugOn) PrintDebug(31);

    // jet loop
    for (uint64_t iJet = 0; iJet < m_input.jets.size(); iJet++) {

      // clear vector for correlator
      m_jetCstVector.clear();

      // select jet pt bin & apply jet cuts
      const bool isGoodJet = IsGoodJet( m_input.jets[iJet] );
      if (!isGoodJet) continue;

      //get Jet info for manual calculations
      double jet_pT = m_input.jets[iJet].GetPT();
      const double jet_Eta = m_input.jets[iJet].GetEta();
      const double jet_Phi = m_input.jets[iJet].GetPhi();
      double jet_Ene = m_input.jets[iJet].GetEne();
      if(m_config.jetPtSmear != 0 && m_config.modJets){
	double mass = sqrt((jet_Ene*jet_Ene) - (jet_pT*jet_pT));
	jet_pT += shift->Gaus(0, m_config.jetPtSmear*jet_pT);
	jet_Ene = sqrt((jet_pT*jet_pT) + (mass*mass));
      }
      ROOT::Math::PtEtaPhiEVector VecJet(jet_pT, jet_Eta, jet_Phi, jet_Ene);

      // constituent loop
      for (uint64_t iCst = 0; iCst < m_input.csts[iJet].size(); iCst++) {

        // create cst 4-vector
        ROOT::Math::PtEtaPhiMVector pVecCst(
          m_input.csts[iJet][iCst].GetPT(),
          m_input.csts[iJet][iCst].GetEta(),
          m_input.csts[iJet][iCst].GetPhi(),
          Const::MassPion()
        );

	//Components to use for pseudoJet
	float Px = pVecCst.Px();
	float Py = pVecCst.Py();
	float Pz = pVecCst.Pz();
	float cstE = pVecCst.E();
	float skipCst = false;
	
	//Create pseudoJet for original cst info
	PseudoJet CstPseudo(Px, Py, Pz, cstE);

	//Apply smearing if necessary
	if(m_config.modCsts){
	  //Apply efficiency if needed
	  float rando = shift->Uniform(0,1.);
	  float eff = fEff->Eval(m_input.csts[iJet][iCst].GetPT());
	  if(rando>eff && m_config.effVal!=1) skipCst = true;
	  //Apply pT smearing if needed
	  float newpT = m_input.csts[iJet][iCst].GetPT();
	  if(m_config.pTSmear != 0) newpT+=shift->Gaus(0, m_config.pTSmear*m_input.csts[iJet][iCst].GetPT());
	  ROOT::Math::PtEtaPhiMVector rVecCstCopy(newpT, m_input.csts[iJet][iCst].GetEta(), m_input.csts[iJet][iCst].GetPhi(), Const::MassPion());
	  TVector3 p(rVecCstCopy.X(), rVecCstCopy.Y(), rVecCstCopy.Z());
	  TVector3 rotation_axis(rVecCstCopy.X(), rVecCstCopy.Y(), rVecCstCopy.Z());
	  //Apply angular smearing if needed
	  if(m_config.theta != 0){
	    float Theta0 = p.Theta();
	    float deltaTheta = shift->Gaus(0, m_config.theta);
	    float deltaPhi = shift->Uniform(-TMath::Pi(), TMath::Pi());
	    p.SetTheta(Theta0+deltaTheta);
	    p.Rotate(deltaPhi, rotation_axis);
	  }
	  //Change values to use in pseudoJet
	  Px = p.X();
	  Py = p.Y();
	  Pz = p.Z();
	  cstE = rVecCstCopy.E(); 
	}
	if(skipCst) continue;

        // if needed, apply constituent cuts
        const bool isGoodCst = IsGoodCst( m_input.csts[iJet][iCst] );
        if (!isGoodCst) continue;

        // create pseudojet
        PseudoJet constituent(Px, Py, Pz, cstE);
        constituent.set_user_index(iCst);

        // add to list
        m_jetCstVector.push_back(constituent);

      }  // end cst loop

      // run eec computation(s)
      if (m_config.doPackageCalc) {
        DoLocalCalcWithPackage( jet_pT  );
      }
      if(m_config.doManualCalc) {
	DoLocalCalcManual(m_jetCstVector, VecJet);
      }
    }  // end jet loop
    return;

  }  // end 'DoLocalCalculation()'

  double SEnergyCorrelator::GetWeight(ROOT::Math::PtEtaPhiEVector momentum, int option, optional<ROOT::Math::PtEtaPhiEVector> reference){
    double weight = 1;
    double Et = 1;
    if(reference.has_value()){
      TVector3 pRef(reference.value().X(), reference.value().Y(), reference.value().Z());
      TVector3 pMom(momentum.X(), momentum.Y(), momentum.Z());
      TVector3 pEt = pMom - (pMom*pRef/(pRef*pRef))*pRef;
      Et = pow(pEt.Mag2()+pow(Const::MassPion(),2),0.5);
    }
    switch(option){
      case Norm::Et:
	weight = Et;
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
  }

  void SEnergyCorrelator::DoLocalCalcWithPackage(const double ptJet) {

    // print debug statement
    if (m_config.isDebugOn) PrintDebug(31);
    const uint32_t iPtJetBin = GetJetPtBin( ptJet );
    const bool     foundBin  = (iPtJetBin >= 0);
    const bool     hasCsts   = (m_jetCstVector.size() > 0); 
    if (foundBin && hasCsts) {
      m_eecLongSide[iPtJetBin] -> compute(m_jetCstVector);
    }
    return;

  }  // end 'DoLocalCalcWithPackage(double)'

  void SEnergyCorrelator::DoLocalCalcManual(const vector<fastjet::PseudoJet> momentum, ROOT::Math::PtEtaPhiEVector normalization){
    //Get norm
    double norm = GetWeight(normalization, m_config.norm_option);
    //Loop over csts
    for(uint64_t iCst = 0; iCst < momentum.size(); iCst++){
      //Get weightA
      ROOT::Math::PtEtaPhiEVector cstVec(
	  momentum[iCst].pt(),
          momentum[iCst].eta(),
	  momentum[iCst].phi(),
          pow(pow(momentum[iCst].pt(), 2) + pow(Const::MassPion(), 2), 0.5)
        );
      double weightA = GetWeight(cstVec, m_config.mom_option, normalization);
      //Start second cst loop
      for(uint64_t jCst = 0; jCst < momentum.size(); jCst++){
        //Get weightB
	ROOT::Math::PtEtaPhiEVector cstVecB(
	  momentum[jCst].pt(),
          momentum[jCst].eta(),
	  momentum[jCst].phi(),
          pow(pow(momentum[jCst].pt(), 2) + pow(Const::MassPion(), 2), 0.5)
        );
	double weightB = GetWeight(cstVecB, m_config.mom_option, normalization);
	const double dhCstAB = (momentum[iCst].rap()-momentum[jCst].rap());
	double dfCstAB = std::fabs(momentum[iCst].phi()-momentum[jCst].phi());
	if(dfCstAB > TMath::Pi()) dfCstAB = 2*TMath::Pi() - dfCstAB;
	const double drCstAB  = sqrt((dhCstAB * dhCstAB) + (dfCstAB * dfCstAB));
	const double eecWeight = (weightA*weightB)/(norm*norm);

	//Fill manual eecs
	for(size_t iPtBin = 0; iPtBin < m_config.ptJetBins.size(); iPtBin++){
	  bool isInPtBin = ((normalization.Pt() >= m_config.ptJetBins[iPtBin].first) && (normalization.Pt() < m_config.ptJetBins[iPtBin].second));
	  if(isInPtBin){
	    m_outManualHistErrDrAxis[iPtBin]->Fill(drCstAB, eecWeight);
	  }
	}//end of pT bin loop
      }//end of second cst loop
    }//end of first cst loop
  }

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

    if (m_config.isStandalone) PrintMessage(14);
    return;

  }  // end 'ExtractHistsFromCorr()'



  bool SEnergyCorrelator::IsGoodJet(const Types::JetInfo& jet) {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 7)) PrintDebug(26);

    const bool isGoodJet = jet.IsInAcceptance(m_config.jetAccept);
    return isGoodJet;

  }  // end 'IsGoodJet(double, double)'



  bool SEnergyCorrelator::IsGoodCst(const Types::CstInfo& cst) {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 7)) PrintDebug(27);

    // if analyzing truth tree and needed, check embedding ID
    bool isSubEvtGood = true;
    if (m_config.isInTreeTruth && m_config.selectSubEvts) {
      isSubEvtGood = Tools::IsSubEvtGood( cst.GetEmbedID(), m_config.subEvtOpt, m_config.isEmbed );
    }

    // if needed, apply cst. cuts
    bool isInCstAccept = true;
    if (m_config.applyCstCuts) {
      isInCstAccept = cst.IsInAcceptance(m_config.cstAccept);
    }

    const bool isGoodCst = (isSubEvtGood && isInCstAccept);
    return isGoodCst;

  }  // end 'IsGoodCst(double, double)'



  int32_t SEnergyCorrelator::GetJetPtBin(const double ptJet) {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 7)) PrintDebug(28);
 
    bool    isInPtBin(false);
    int32_t iJetPtBin(-1);
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
