// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef JETBKGDSUB_H
#define JETBKGDSUB_H

#include <fun4all/SubsysReco.h>
#include <jetbase/Jetv1.h>

#include <memory>
#include <string>
#include <utility>  // std::pair, std::make_pair
#include <vector>

class PHCompositeNode;
class TTree;
class JetInput;
class JetAlgo;
class JetEvalStack;

class JetBkgdSub : public SubsysReco
{
public:
    // constructor
    JetBkgdSub(const float jet_R = 0.4,
            const std::string &recojetname = "AntiKt_Tower_r04_Sub1",
            const std::string &rawjetname = "AntiKt_Tower_r04",
            const std::string &truthjetname = "AntiKt_Truth_r04",
            const std::string &outputfilename = "jettree.root");

    ~JetBkgdSub() override; // destructor

    // setters
    void setEtaRange(double low, double high)
    {
        m_etaRange.first = low;
        m_etaRange.second = high;
    }

    void setPtRange(double low, double high)
    {
        m_ptRange.first = low;
        m_ptRange.second = high;
    }

    void add_input(JetInput *input) { _inputs.push_back(input); }
    
    // event trigger
    float LeadingR04TruthJet(PHCompositeNode *topNode);
    
    // Standard Fun4All functions
    int Init(PHCompositeNode *topNode) override;
    int process_event(PHCompositeNode *topNode) override;
    int ResetEvent(PHCompositeNode *topNode) override;
    int End(PHCompositeNode *topNode) override;

    void EstimateRhoMult(PHCompositeNode *topNode);
    double GetMedian(std::vector<double> &v);

private:

    // private variables
    float m_jet_R;
     std::string m_reco_input;
    std::string m_raw_input; 
    std::string m_truth_input;
    std::string m_outputfilename;
    std::pair<double, double> m_etaRange;
    std::pair<double, double> m_ptRange;
    std::vector<JetInput *> _inputs;

    //____________________________________________________________________________..
    // output tree variables
    int m_event;
    int m_rhoA_jets;
    int m_raw_jets;
    int m_reco_jets;
    int m_truth_jets;
    int m_centrality;
    double m_rho_area;
    double m_rho_area_sigma;  
    double m_rho_mult;

    float m_event_leading_truth_pt;

     // output trees
    TTree *m_tree;
    //sub1 jet variables
    std::vector<float> m_reco_eta;
    std::vector<float> m_reco_phi;
    std::vector<float> m_reco_pt;
    std::vector<float> m_reco_pt_unsub;

    // rhoA jet variables
    std::vector<float> m_rhoA_eta;
    std::vector<float> m_rhoA_phi;
    std::vector<float> m_rhoA_pt;
    std::vector<float> m_rhoA_area;
    std::vector<float> m_rhoA_pt_unsub; 

    // mult jet variables
    std::vector<int> m_raw_ncomponent;
    std::vector<float> m_raw_eta;
    std::vector<float> m_raw_phi;
    std::vector<float> m_raw_pt;

    // truth jet variables
    std::vector<int> m_truth_ncomponent;
    std::vector<float> m_truth_eta;
    std::vector<float> m_truth_phi;
    std::vector<float> m_truth_pt;
    
};

#endif // JETBKGDSUB_H
