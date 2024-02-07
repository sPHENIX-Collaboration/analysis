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
    JetBkgdSub(const double jet_R = 0.4,
            const std::string &outputfilename = "jettree.root");

    ~JetBkgdSub() override; // destructor

    // setters
    void doIterative(bool b = true) { _doIterative = b; }
    void doAreaSub(bool b = true) { _doAreaSub = b; }
    void doMultSub(bool b = true) { _doMultSub = b; }
    void doTruth(bool b = true) { _doTruth = b; }
    void doData(bool b = true) { _doData = b; }
    void doEmbed(bool b = true) { _doEmbed = b;}
    void doTowerECut(bool b = true) { _doTowerECut = b;}

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

    void setMinRecoPt(double pt) { _minrecopT = pt; }

    void setVzCut(double cut) { m_vtxZ_cut = cut; }

    void setTowerThreshold(double threshold) { m_towerThreshold = threshold;}

    void add_input(JetInput *input) { _inputs.push_back(input); }
    
    // event trigger (for simulation)
    float LeadingR04TruthJet(PHCompositeNode *topNode);
    
    // Standard Fun4All functions
    int Init(PHCompositeNode *topNode) override;
    int process_event(PHCompositeNode *topNode) override;
    int ResetEvent(PHCompositeNode *topNode) override;
    int End(PHCompositeNode *topNode) override;

    void EstimateRhoMult(PHCompositeNode *topNode);
    double GetMedian(std::vector<double> &v);
    void GetCentInfo(PHCompositeNode *topNode);
    float NSignalCorrection(float jet_pt, int cent);

private:

    // private variables
    double m_jet_R;
    std::string m_iter_input;
    std::string m_raw_input; 
    std::string m_truth_input;
    std::string m_outputfilename;
    std::pair<double, double> m_etaRange;
    std::pair<double, double> m_ptRange;
    double _minrecopT;
    double m_vtxZ_cut;
    double m_towerThreshold;
    std::vector<JetInput *> _inputs;
    bool _doIterative;
    bool _doAreaSub;
    bool _doMultSub;
    bool _doTruth;
    bool _doData;
    bool _doEmbed;
    bool _doTowerECut;

    //____________________________________________________________________________..
    // output tree variables
    int m_event;
    int m_rhoA_jets;
    int m_mult_jets;
    int m_iter_jets;
    int m_truth_jets;
    int m_centrality;
    double m_mbd_NS;
    double m_rho_area;
    double m_rho_area_sigma;  
    double m_rho_area_CEMC;
    double m_rho_area_sigma_CEMC;  
    double m_rho_area_IHCAL;
    double m_rho_area_sigma_IHCAL;  
    double m_rho_area_OHCAL;
    double m_rho_area_sigma_OHCAL;  
    double m_rho_mult;

    float m_event_leading_truth_pt;

     // output trees
    TTree *m_tree;
    //sub1 jet variables
    std::vector<float> m_iter_eta;
    std::vector<float> m_iter_phi;
    std::vector<float> m_iter_pt;
    std::vector<float> m_iter_pt_unsub;

    // rhoA jet variables
    std::vector<float> m_rhoA_eta;
    std::vector<float> m_rhoA_phi;
    std::vector<float> m_rhoA_pt;
    std::vector<float> m_rhoA_area;
    std::vector<float> m_rhoA_pt_unsub; 

    // mult jet variables
    std::vector<int> m_mult_ncomponent;
    std::vector<float> m_mult_nsignal;
    std::vector<float> m_mult_eta;
    std::vector<float> m_mult_phi;
    std::vector<float> m_mult_pt;
    std::vector<float> m_mult_pt_unsub; 

    // truth jet variables
    std::vector<int> m_truth_ncomponent;
    std::vector<float> m_truth_eta;
    std::vector<float> m_truth_phi;
    std::vector<float> m_truth_pt;
    
};

#endif // JETBKGDSUB_H
