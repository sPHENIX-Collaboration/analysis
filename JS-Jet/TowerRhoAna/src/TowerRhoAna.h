// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef TOWERRHOANA_H
#define TOWERRHOANA_H

#include <fun4all/SubsysReco.h>

#include <string>
#include <utility>  // std::pair, std::make_pair
#include <vector>

class PHCompositeNode;

class TH1;


/// \class TowerRhoAna
/// \brief SubsysReco module that plots tower rho information
/// \author: Tanner Mengel

class TowerRhoAna : public SubsysReco
{

    public:

        TowerRhoAna(const std::string &outputfilename = "TowerRhoAna.root");
        ~TowerRhoAna() override {};

        void add_mult_rho_node(const std::string &name)
        {
            m_do_mult_rho = true;
            m_mult_rho_node = name;
        }
        void add_area_rho_node(const std::string &name)
        {
            m_do_area_rho = true;
            m_area_rho_node = name;
        }

        // standard Fun4All functions
        int Init(PHCompositeNode *topNode) override;
        int process_event(PHCompositeNode *topNode) override;
        int End(PHCompositeNode *topNode) override;


    private:

        //! Input Node strings
        std::string m_outputFileName{ "TowerRhoAna.root"};

        bool m_do_mult_rho{false};
        bool m_do_area_rho{false};
        std::string m_mult_rho_node{};
        std::string m_area_rho_node{};


        //histos
        TH1 * h1_mult_rho{nullptr};
        TH1 * h1_mult_rho_sigma{nullptr};
        TH1 * h1_area_rho{nullptr};
        TH1 * h1_area_rho_sigma{nullptr};



};

#endif // TOWERRHOANA_H
