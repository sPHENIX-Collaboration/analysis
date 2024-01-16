// ----------------------------------------------------------------------------
// 'SCorrelatorUtilities.GenTools.h'
// Derek Anderson
// 10.30.2023
//
// Collection of frequent generator/MC-related methods utilized
// in the sPHENIX Cold QCD Energy-Energy Correlator analysis.
// ----------------------------------------------------------------------------

#pragma once

// make common namespaces implicit
using namespace std;
using namespace findNode;



namespace SColdQcdCorrelatorAnalysis {
  namespace SCorrelatorUtilities {

    // generator/mc methods ---------------------------------------------------

    int GetEmbedID(PHCompositeNode* topNode, const int iEvtToGrab) {

      // grab mc event map
      PHHepMCGenEventMap* mapMcEvts = findNode::getClass<PHHepMCGenEventMap>(topNode, "PHHepMCGenEventMap");
      if (!mapMcEvts) {
        cerr << PHWHERE
             << "PANIC: HEPMC event map node is missing!"
             << endl;
        assert(mapMcEvts);
      }

      // grab mc event & return embedding id
      PHHepMCGenEvent* mcEvtStart = mapMcEvts -> get(iEvtToGrab);
      if (!mcEvtStart) {
        cerr << PHWHERE
             << "PANIC: Couldn't grab start of mc events!"
             << endl;
        assert(mcEvtStart);
      }
      return mcEvtStart -> get_embedding_id();

    }  // end 'GetEmbedID(PHCompositeNode*, int)'


    HepMC::GenEvent* GetMcEvent(PHCompositeNode* topNode, const int iEvtToGrab) {

      // grab mc event map
      PHHepMCGenEventMap* mapMcEvts = findNode::getClass<PHHepMCGenEventMap>(topNode, "PHHepMCGenEventMap");
      if (!mapMcEvts) {
        cerr << PHWHERE
             << "PANIC: HEPMC event map node is missing!"
             << endl;
        assert(mapMcEvts);
      }

      // grab mc event & check if good
      PHHepMCGenEvent* mcEvtStart = mapMcEvts -> get(iEvtToGrab);
      if (!mcEvtStart) {
        cerr << PHWHERE
             << "PANIC: Couldn't grab start of mc events!"
             << endl;
        assert(mcEvtStart);
      }

      HepMC::GenEvent* mcEvt = mcEvtStart -> getEvent();
      if (!mcEvt) {
        cerr << PHWHERE
             << "PANIC: Couldn't grab HepMC event!"
             << endl;
        assert(mcEvt);
      }
      return mcEvt;

    }  // end 'GetMcEvent(PHCompositeNode*, int)'



    /* TODO
     *   - replace most of this with comparison b/n ParInfo objects
     */
    bool IsGoodParticle(HepMC::GenParticle* par, const bool ignoreCharge) {

      // check charge if needed
      const bool isJetCharged  = (m_jetType != 1);
      const bool doChargeCheck = (isJetCharged && !ignoreCharge);

      int   parID;
      bool  isGoodCharge;
      float parChrg;
      if (doChargeCheck) {
        parID        = par -> pdg_id();
        parChrg      = GetParticleCharge(parID);
        isGoodCharge = (parChrg != 0.);
      } else {
        isGoodCharge = true;
      }

      const double parEta       = par -> momentum().eta();
      const double parPx        = par -> momentum().px();
      const double parPy        = par -> momentum().py();
      const double parPt        = sqrt((parPx * parPx) + (parPy * parPy));
      const bool   isInPtRange  = ((parPt  > m_parPtRange[0])  && (parPt  < m_parPtRange[1]));
      const bool   isInEtaRange = ((parEta > m_parEtaRange[0]) && (parEta < m_parEtaRange[1]));
      const bool   isGoodPar    = (isGoodCharge && isInPtRange && isInEtaRange);
      return isGoodPar;

    }  // end 'IsGoodParticle(HepMC::GenParticle*, bool)'



    bool IsOutgoingParton(HepMC::GenParticle* par) {

      // grab particle info
      const int pid    = par -> pdg_id();
      const int status = par -> status();

      // check if is outgoing parton
      const bool isStatusGood     = ((status == 23) || (status == 24));
      const bool isLightQuark     = ((pid == 1)     || (pid == 2));
      const bool isStrangeQuark   = ((pid == 3)     || (pid == 4));
      const bool isHeavyQuark     = ((pid == 5)     || (pid == 6));
      const bool isGluon          = (pid == 21);
      const bool isParton         = (isLightQuark || isStrangeQuark || isHeavyQuark || isGluon);
      const bool isOutgoingParton = (isStatusGood && isParton);
      return isOutgoingParton;

    }  // end 'IsOutgoingParton(HepMC::GenParticle*)'



    /* TODO
     *   - replace switch-case w/ a map of PID onto charge
     */ 
    float GetParticleCharge(const int pid) {

      // particle charge
      float charge(-999.);

      switch (abs(pid)) {
        // e+/e-
        case 11:
          charge = 1.;
          break;
        // e neutrinos
        case 12:
          charge = 0.;
          break;
        // mu-/mu+
        case 13:
          charge = -1.;
          break;
        // mu neutrinos
        case 14:
          charge = 0.;
          break;
        // tau-/tau+
        case 15:
          charge = -1.;
          break;
        // tau neutrinos
        case 16:
          charge = 0.;
          break;
        // photon
        case 22:
          charge = 0.;
          break;
        // Z0
        case 23:
          charge = 0.;
          break;
        // W+/W-
        case 24:
          charge = 1.;
          break;
        // pi0
        case 111:
          charge = 0.;
          break;
        // pi+/pi-
        case 211:
          charge = 1.;
          break;
        // K0 (long)
        case 130:
          charge = 0.;
          break;
        // K0 (short)
        case 310:
          charge = 0.;
          break;
        // K+/K-
        case 321:
          charge = 1.;
          break;
        // D+/D-
        case 441:
          charge = 1.;
          break;
        // D0
        case 421:
          charge = 0.;
          break;
        // DS+/DS-
        case 431:
          charge = 1.;
          break;
        // eta
        case 221:
          charge = 0.;
          break;
        // proton/antiproton
        case 2212:
          charge = 1.;
          break;
        // neutron
        case 2112:
          charge = 0.;
          break;
        // lambda
        case 3122:
          charge = 0.;
          break;
        // sigma+/antisigma+
        case 3222:
          charge = 1.;
          break;
        // sigma0
        case 3212:
          charge = 0.;
          break;
        // sigma-/antisigma-
        case 3112:
          charge = -1.;
          break;
        // xi0
        case 3322:
          charge = 0.;
          break;
        // deuteron
        case 700201:
          charge = 0.;
          break;
        // alpha
        case 700202:
          charge = 2.;
          break;
        // triton
        case 700301:
          charge = 0.;
          break;
        // he3
        case 700302:
          charge = 3.;
          break;
        default:
          charge = 0.;
          break;
      }  // end switch (abs(pid))

      // if antiparticle, flip charge and return
      if (pid < 0) {
        charge *= -1.;
      }
      return charge;

    }  // end 'GetParticleCharge(int)'

  }  // end SCorrelatorUtilities namespace
}  // end SColdQcdCorrealtorAnalysis namespace

// end ------------------------------------------------------------------------
