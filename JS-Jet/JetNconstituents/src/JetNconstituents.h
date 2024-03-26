// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef JETNCONSTITUENTS_H
#define JETNCONSTITUENTS_H

#include <fun4all/SubsysReco.h>
#include <jetbase/Jet.h>

#include <string>
#include <utility>  // std::pair, std::make_pair
#include <vector>

class PHCompositeNode;

class TH1;
class TH2;
class JetInput;


/// \class JetNconstituents
/// \brief SubsysReco module that plots the number of constituents in the reco jets per calorimeter layer and the energy fraction of the reco jets per calorimeter layer
/// \author: Tanner Mengel

class JetNconstituents : public SubsysReco
{

    public:

        JetNconstituents(const std::string &outputfilename = "JetNconstituents.root");
        ~JetNconstituents() override {};

        void setEtaRange(double low, double high)
        { // set the eta range for the reco jets
            m_etaRange.first = low;
            m_etaRange.second = high;
        }

        void setPtRange(double low, double high)
        { // set the pt range for the reco jets
            m_ptRange.first = low;
            m_ptRange.second = high;
        }

        void setRecoJetNodeName(const std::string &name)
        { // set the name of the node containing the reco jets
            m_recoJetName = name;
        }

        // standard Fun4All functions
        int Init(PHCompositeNode *topNode) override;
        int process_event(PHCompositeNode *topNode) override;
        int End(PHCompositeNode *topNode) override;


    private:

        //! Input Node strings
        std::string m_outputFileName{ "JetNconstituents.root"};


        // ! Kinematic cuts and reco jet node name
        std::pair<double, double> m_etaRange = std::make_pair(-1.1, 1.1);
        std::pair<double, double> m_ptRange = std::make_pair(1.0, 1000.0);
        std::string m_recoJetName { "AntiKt_Tower_r04"};

        // Jet N constituents
        TH1 * h1_jetNconstituents_total{nullptr};
        TH1 * h1_jetNconstituents_IHCAL{nullptr};
        TH1 * h1_jetNconstituents_OHCAL{nullptr};
        TH1 * h1_jetNconstituents_CEMC{nullptr};
        TH2 * h2_jetNconstituents_vs_caloLayer{nullptr};

        // Jet E fraction
        TH1 * h1_jetFracE_IHCAL{nullptr};
        TH1 * h1_jetFracE_OHCAL{nullptr};
        TH1 * h1_jetFracE_CEMC{nullptr};
        TH2 * h2_jetFracE_vs_caloLayer{nullptr};



};

#endif // JETNCONSTITUENTS_H
