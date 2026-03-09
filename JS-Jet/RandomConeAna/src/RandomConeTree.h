// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef RANDOMCONETREE_H
#define RANDOMCONETREE_H

#include <fun4all/SubsysReco.h>

#include <memory>
#include <string>
#include <utility>  // std::pair, std::make_pair
#include <vector>
#include <array>

class PHCompositeNode;

class TTree;

class RandomConeTree : public SubsysReco
{
    public:

        // constructor
        RandomConeTree(const std::string &outputfilename = "conetree.root");
        ~RandomConeTree() override; // destructor

        
        // Standard Fun4All functions
        int Init(PHCompositeNode *topNode) override;
        int process_event(PHCompositeNode *topNode) override;
        int End(PHCompositeNode *topNode) override;
        int ResetEvent(PHCompositeNode *topNode) override;


        // add rho node
        void add_rho_node(const std::string &name) 
        {
            m_rho_nodes.push_back(name);
        }

        // add random cone node
        void add_random_cone_node(const std::string &name) 
        {
            m_random_cone_nodes.push_back(name);
        }

        // Global vertex cuts
        void add_gvtx_cut(const double low, const double high)
        {
            m_do_gvtx_cut = true;
            m_gvtx_cut.first = low;
            m_gvtx_cut.second = high;
        }

        // centrality
        void do_centrality_info(bool b = true) { m_do_cent_info = b; }


        // Data conveniences ========================================================
        void do_data(bool b = true) 
        { 
            // set to true if running on data   
            m_do_data = b; 

            // set MC event selection to false if running on data
            m_MC_do_event_selection = false;

            // set MC event weight to false if running on data
            m_MC_add_weight_to_ttree = false;
        }


        // MC conveniences ========================================================

        // Do event selection on leading R=0.4 truth jet pT range
        void do_event_selection_on_leading_truth_jet(float low, float high)
        {
            m_MC_do_event_selection = true;
            m_MC_event_selection_jetpT_range.first = low;
            m_MC_event_selection_jetpT_range.second = high;
        }
       
        // add event weight to output tree
        void add_weight_to_ttree(double w)
        { 
            m_MC_add_weight_to_ttree = true;
            m_MC_event_weight = w; 
        }

    

       
  
private:


        // output file
        std::string m_outputfile_name{"conetree.root"};

        std::vector<std::string> m_rho_nodes{};
        std::vector<std::string> m_random_cone_nodes{};

        // vertex cuts
        bool m_do_gvtx_cut{false};
        std::pair<double, double> m_gvtx_cut{-100, 100};

        // cent info
        bool m_do_cent_info{false};
        void GetCentInfo(PHCompositeNode *topNode);

        // event trigger (for simulation)
        bool m_MC_do_event_selection{false};
        std::pair<float, float> m_MC_event_selection_jetpT_range{-1,1000};

        // event weight
        bool m_MC_add_weight_to_ttree{false};
        double m_MC_event_weight{1.0};

        // is data
        bool m_do_data{false};

        bool GoodEvent(PHCompositeNode *topNode); // event selection (MC and data)

        //____________________________________________________________________________..
        // output trees
        TTree * m_run_info_tree{nullptr};
        TTree * m_event_tree{nullptr};

        int m_event_id{-1};
        
        int m_centrality;
        float m_impactparam;
        float m_zvtx;
        double m_mbd_NS;

        std::array<float, 10> m_rhos{{0,0,0,0,0,0,0,0,0,0}}; // max 10 rho methods
        std::array<float, 10> m_sigma_rhos{{0,0,0,0,0,0,0,0,0,0}}; // max 10 rho methods
        
        std::array<float, 10> m_cone_R{{0,0,0,0,0,0,0,0,0,0}}; // max 10 cones
        std::array<float, 10> m_cone_etas{{0,0,0,0,0,0,0,0,0,0}}; // max 10 cones
        std::array<float, 10> m_cone_phis{{0,0,0,0,0,0,0,0,0,0}}; // max 10 cones
        std::array<float, 10> m_cone_pts{{0,0,0,0,0,0,0,0,0,0}}; // max 10 cones
        std::array<int, 10> m_cone_nclustereds{{0,0,0,0,0,0,0,0,0,0}}; // max 10 cones 

        //____________________________________________________________________________..
};

#endif // RANDOMCONETREE_H
