// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef JETMULTSUB_H
#define JETMULTSUB_H

#include <fun4all/SubsysReco.h>

#include <string>

class PHCompositeNode;

/// \class JetMultSub
///
/// \brief Subtracts reco jets using multiplicity method

class JetMultSub : public SubsysReco
{
public:
    // Constructor
    JetMultSub(const std::string &name = "JetMultSub");

    // Destructor
    ~JetMultSub() override;

    // setters 
    void setEtaRange(double low, double high){
        m_etaRange.first = low;
        m_etaRange.second = high;
    }
    
    void setPtRange(double low, double high){
        m_ptRange.first = low;
        m_ptRange.second = high;
    }


    // add input(s)
    void add_reco_input(const std::string &recoinput){m_reco_input = recoinput;}
    void add_kt_input(const std::string &ktinput){m_kt_input = ktinput;}
    // add output
    void set_output_name(const std::string &outputname){m_subtracted_output = outputname;}


    // Standard Fun4All functions
    int Init(PHCompositeNode *topNode) override;
    int InitRun(PHCompositeNode *topNode) override;
    int process_event(PHCompositeNode *topNode) override;
    int ResetEvent(PHCompositeNode *topNode) override;
    int EndRun(const int runnumber) override;
    int End(PHCompositeNode *topNode) override;
    int Reset(PHCompositeNode * /*topNode*/) override;
    void Print(const std::string &what = "ALL") const override;

    
private:

    // private variables
    std::string m_reco_input;
    std::string m_kt_input;
    std::string m_subtracted_output;
    std::pair<double, double> m_etaRange;
    std::pair<double, double> m_ptRange;

    /// private methods
    int CreateNode(PHCompositeNode *topNode); // create node if it doesn't exist
    float EstimateRho(PHCompositeNode *topNode); // estimate rho using kT jets-> returns rho
    float GetMultiplicityCorrection(float pt); //  get multiplicity correction for given pt from vector of corrections


};

#endif // JETMULTSUB_H
