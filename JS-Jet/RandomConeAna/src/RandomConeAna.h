// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef RANDOMCONEANA_H
#define RANDOMCONEANA_H

#include <fun4all/SubsysReco.h>

#include <jetbase/Jetv1.h>

#include <fastjet/PseudoJet.hh>

#include <memory>
#include <string>
#include <utility>  // std::pair, std::make_pair
#include <vector>

#include <TRandom3.h> // for seed

class PHCompositeNode;

class TTree;
class TRandom3;

class JetInput;

class RandomConeAna : public SubsysReco
{
public:


    // constructor
    RandomConeAna(const std::string &outputfilename = "jettree.root");
    ~RandomConeAna() override; // destructor

    void setEventSelection(double low, double high){
        _doEventSelection = true;
        m_eventSelection.first = low;
        m_eventSelection.second = high;
    }

    void addWeight(double w) { 
        _doWeight = true;
        weight = w; 
    }

    void add_input(JetInput *input) { _inputs.push_back(input); }
    
    void add_iter_input(JetInput *input) { _iter_inputs.push_back(input); } 

    void set_user_seed(const unsigned int seed) { 
        m_seed = seed;
        m_user_set_seed = true; 
    }


    // Standard Fun4All functions
    int Init(PHCompositeNode *topNode) override;
    int process_event(PHCompositeNode *topNode) override;
    int End(PHCompositeNode *topNode) override;

private:

    // event trigger (for simulation)
    float LeadingR04TruthJet(PHCompositeNode *topNode);
    bool EventSelect(PHCompositeNode *topNode);

    // cent info
    void GetCentInfo(PHCompositeNode *topNode);

    std::pair<float,float> get_leading_jet_eta_phi(PHCompositeNode *topNode);
    std::vector<fastjet::PseudoJet> jets_to_pseudojets(std::vector<Jet*>& particles, bool randomize_etaphi);

    std::vector<fastjet::PseudoJet> get_psuedojets(PHCompositeNode *topNode);
    void EstimateRhos(PHCompositeNode *topNode);
    float _percentile(const std::vector<float> & sorted_vec, 
        const float percentile,
        const float nempty) const ;
    void _median_stddev(const std::vector<float> & vec, 
        float n_empty_jets,
        float & median, 
        float & std_dev) const ; 

    
    // output tree
    std::string m_outputfilename;
    bool _doEventSelection;
    std::pair<double, double> m_eventSelection;

    bool _doWeight;
    bool _doCentrality;

    bool m_user_set_seed;
    unsigned int m_seed;

    std::vector<JetInput *> _inputs;
    std::vector<JetInput *> _iter_inputs;

    TRandom3* m_random { nullptr };


    //____________________________________________________________________________..
    // output tree
    TTree *m_tree;

    int m_event;
    double weight;
    int m_centrality;

    float m_rho_area;
    float m_rho_area_sigma;
    float m_rho_mult;
    float m_rho_mult_sigma;

    float m_cone_area;
    int m_cone_nclustered;
    
    float m_cone_pt_raw;
    float m_cone_pt_iter_sub;

    float m_randomized_cone_pt_raw;
    float m_randomized_cone_pt_iter_sub;

    float m_avoid_leading_cone_pt_raw;
    float m_avoid_leading_cone_pt_iter_sub;

};

#endif // RANDOMCONEANA_H
