// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef trackEEC_H
#define trackEEC_H

#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"

#include <fastjet/PseudoJet.hh>
#include <fastjet/JetDefinition.hh>
#include <fastjet/Selector.hh>

#include <vector>
#include <format>
#include <string>

class PHCompositeNode;
class SvtxTrack;


class trackEEC : public SubsysReco
{
    public:
    
        trackEEC(const std::string &name = "trackEEC");
        ~trackEEC () override = default;

        int Init(PHCompositeNode *topNode) override;

        int process_event(PHCompositeNode *topNode) override;

        int End(PHCompositeNode *topNode) override;

        void SetOutfileName(std::string name){m_outfileName = name;};
  
    private:

        float m_pi{0.13957};

        TFile *outfile{nullptr};
        std::string m_outfileName{""};
        TH1D *h_nEvents{nullptr};
        TH1D *h_crossing{nullptr};
        TH1D *h_nintt{nullptr};
        TH1D *h_chi2_ndf{nullptr};
        TH1D *h_nClus{nullptr};
        TH1D *h_trackSpec{nullptr};
        TH2D *h_track_etaPhi{nullptr};
        TH1D *h_jetCons{nullptr};
        TH1D *h_jetSpec{nullptr};
        TH2D *h_jetConsSpec{nullptr};
        TH2D *h_jet_etaPhi{nullptr};
        TH1D *h_jetMass{nullptr};
        TH1D *h_inJetAll{nullptr};
        TH1D *h_inJetAll_Q2{nullptr};
        TH1D *h_inJet[5]{nullptr};
        TH1D *h_inJet_Q2[5]{nullptr};
        TH1D *h_wEEC{nullptr};
        TH1D *h_wEEC_jet{nullptr};
        TH1D *h_wEEC_jet_Q2{nullptr};
        TH1D *h_wEEC_dijet{nullptr};
        TH1D *h_wEEC_dijet_Q2{nullptr};

        float jet_pTs[6] = {10,20,30,40,60,1000};

        std::string m_trackMapName = "SvtxTrackMap";

        fastjet::JetDefinition jetDef{ fastjet::antikt_algorithm, 0.4};
        fastjet::Selector jetSel = fastjet::SelectorAbsEtaMax(1.1 - 0.4) && fastjet::SelectorPtMin(10.0);
};

#endif
