#ifndef RANDOMCONES_RANDOMCONERECO_H
#define RANDOMCONES_RANDOMCONERECO_H

//===========================================================
/// \file RandomConeReco.h
/// \brief UE background rho calculator
/// \author Tanner Mengel
//===========================================================

#include "RandomCone.h"
#include "RandomConev1.h"

// fun4all includes
#include <fun4all/SubsysReco.h>

// system includes
#include <string>
#include <vector>
#include <ostream> 

#include <jetbase/Jet.h>
#include <jetbase/JetAlgo.h>
#include <jetbase/JetInput.h>

#include <fastjet/JetDefinition.hh>
#include <fastjet/PseudoJet.hh>

#include <TRandom3.h>


// forward declarations
class PHCompositeNode;


/// \class RandomConeReco
///
/// \brief lll
///
/// 
///


class RandomConeReco : public SubsysReco
{
  public:
    
    RandomConeReco(const std::string &name = "RandomCone");
    ~RandomConeReco() override;

    // standard Fun4All methods
    int Init(PHCompositeNode *topNode) override;
    int process_event(PHCompositeNode *topNode) override;

    // print settings
    void print_settings(std::ostream& os = std::cout) const;
   
    //****************** Required variables **********************************
    /*
        Required variables:

            -> input for the random cone (required)

            -> cone radius (default = 0.4, i.e. R = 0.4)

            -> output node name (default = "RandomCone_<cone_type>_r<int(R*10):2>")
    
    */

    // add input for the random cone (required)
    void add_input(JetInput *input){ m_inputs.push_back(input); }

    // set cone radius (default = 0.4, i.e. R = 0.4)
    void set_cone_radius(const float R) { m_R = R; }

    // output node name (default = "RandomCone_<cone_type>_r<int(R*10):2>")
    void set_output_node_name(const std::string &name) { m_output_node_name = name; }

    // **************** Optional variables ***********************************
    /*

      Optional variables:

        -> set eta range input objects (default = 1.1)

        -> set eta range for cone acceptance (cone is contsrained to eta range min + R < |eta| < max - R)

        -> user-defined seed (default = 0, i.e. use time as seed)

        -> set min pT of input objects (default = 0.0)
                   
    */

    // set eta range input objects (default = 1.1)
    void set_input_max_abs_eta(const float max_abs_eta){ m_input_max_abs_eta = max_abs_eta; }

    // set eta range for cone acceptance (max_abs_eta - R) by default
    void set_cone_max_abs_eta(const float max_abs_eta){ m_cone_max_abs_eta = max_abs_eta; }

    // user-defined seed (default = 0, i.e. use time as seed)
    void set_seed(const unsigned int seed){  m_seed = seed; }

    // set min pT of input objects (default = 0.0)
    void set_input_min_pT(const float min_pT){ m_input_min_pT = min_pT; }

    // tower cut
    void do_tower_cut(bool b = true) { m_do_tower_cut = b; }

    // add threshold for input objects
    void set_tower_threshold(const float threshold)
    {
        m_tower_threshold = threshold; 
    }

    
    // **************** Random Cone reconstruction options **********************
    /*
    
        Random Cone reconstruction options:

            -> randomize eta, phi of the input objects (default = false)
            
            -> avoid leading jet (default = false)
                - minimum dR between the leading jet and the random cone (default = 1.4)
                - lead jet can be from jetmap, jet container or lead jet input
                    - (jetmap) add_lead_jet_map
                    - (jet container) add_lead_jet_container
                    - (lead jet input) add_lead_jet_input 
                - if no additonal input method is set (jetmap, jet container or lead jet input), 
                    the leading jet is reconstructed from the inputs used to reconstruct the random cone
            
            -> basic random cone reconstruction (default = true)
        
        All can be true at the same time, but at least one must be true.
        
    */

    // randomize eta, phi of the input objects
    void do_randomize_etaphi(const bool b) { m_do_randomize_etaphi_of_inputs = b; }
        
    // **************** Avoid Leading Jet Options

    // avoid leading jet stuff (setters for leading jet options)
    void do_avoid_leading_jet(const bool b) 
    { 
        // avoid leading jet of user-defined node
        m_do_avoid_leading_jet = b; 

        // defaults to using inputs to reconstruct jet
        if(m_do_avoid_leading_jet)
        {
            m_leading_jet_from_inputs = true; // leading jet is reconstructed from inputs (default)
            m_leading_jet_from_jetmap = false;
            m_leading_jet_from_jetcont = false;
        }
    }

    // minimum dR between the leading jet and the random cone (default = 1.0 + R)
    void set_lead_jet_dR(const float dR) 
    { 
        //if not already set to avoid leading jet, set to true
        if(!m_do_avoid_leading_jet){ m_do_avoid_leading_jet = true; }
        m_lead_jet_dR = dR; 
    }
    
    // set jet pT threshold for leading jet
    void set_lead_jet_pT_threshold(const float pT) 
    { 
        //if not already set to avoid leading jet, set to true
        if(!m_do_avoid_leading_jet){ m_do_avoid_leading_jet = true; }
        m_lead_jet_pT_threshold = pT;
    }

    // add jet container for leading jet (if not set, the leading jet is reconstructed from the inputs used to reconstruct the random cone)
    void add_lead_jet_container(const std::string &name) 
    {  
        // avoid leading jet of user-defined node
        m_leading_jet_node_name = name; 

        // set true if not already set
        if(!m_do_avoid_leading_jet){ m_do_avoid_leading_jet = true; }
        
        // set type of leading jet
        m_leading_jet_from_inputs = false;
        m_leading_jet_from_jetmap = false;
        m_leading_jet_from_jetcont = true; // leading jet is from jet container


    }
    // add jetmap for leading jet (if not set, the leading jet is reconstructed from the inputs used to reconstruct the random cone)
    void add_lead_jet_map(const std::string &name) 
    {  
        
        // avoid leading jet of user-defined node (jetmap)
        m_leading_jet_node_name = name;

        // set true if not already set
        if(!m_do_avoid_leading_jet){ m_do_avoid_leading_jet = true; }

        // set type of leading jet
        m_leading_jet_from_inputs = false;
        m_leading_jet_from_jetmap = true; // leading jet is from jetmap
        m_leading_jet_from_jetcont = false;

    }
  
    // **************** Basic Random Cone Reconstruction

    // normal random cone reconstruction (default = true)
    void do_basic_reconstruction(const bool b) { m_do_basic_reconstruction = b; } // default is already true





  private:

    // ============================================================
    // required variables
    // ============================================================

    std::vector<JetInput *> m_inputs{}; // input for the random cone

    std::string m_output_node_name{"RandomCone"}; // output node name

    std::string m_output_basic_node{"_basic_"}; // output node name for basic random cone reconstruction
    std::string m_output_randomize_etaphi_node{"_randomize_etaphi_"}; // output node name for randomize eta, phi of the input objects
    std::string m_output_avoid_leading_jet_node{"_avoid_lead_jet_"}; // output node name for avoid leading jet

    float m_R{0.4}; // cone radius

    // ============================================================
    // optional variables
    // ============================================================

    float m_input_max_abs_eta{1.1}; // eta range input objects
    float m_cone_max_abs_eta{5.0}; // eta range for cone acceptance (5.0 is dummy value) default is max_abs_eta - R

    unsigned int m_seed{0}; // user-defined seed (default = 0, i.e. use time as seed)

    float m_input_min_pT{10.0}; // min pT of input objects (default = 0.0)

    // add threshold for input objects
    bool m_do_tower_cut{false}; // tower cut
    float m_tower_threshold{0.0}; // threshold for input objects
    // ============================================================
    // Random Cone reconstruction options
    // ============================================================

    bool m_do_basic_reconstruction{true}; // basic random cone reconstruction (default = true)
    
    bool m_do_randomize_etaphi_of_inputs{false}; // randomize eta, phi of the input objects

    // avoid leading jet stuff
    bool m_do_avoid_leading_jet{false}; // avoid leading jet
    float m_lead_jet_dR{5.0}; // minimum dR between the leading jet and the random cone (default = 1.0 + R)
    float m_lead_jet_pT_threshold{0.0}; // jet pT threshold for leading jet

    // leading jet options
    std::string m_leading_jet_node_name{""};
    bool m_leading_jet_from_inputs{true}; // leading jet is reconstructed from inputs (default)
    bool m_leading_jet_from_jetmap{false}; // leading jet is from jetmap
    bool m_leading_jet_from_jetcont{false}; // leading jet is from jet container


    

    // ============================================================
    // internal variables
    // ============================================================

    // random number generator
    TRandom3 *m_random{nullptr};

    // ============================================================



    // Nodes ==========================================
    int CreateNode(PHCompositeNode *topNode);

      // helper functions ==========================================
    Jet * GetLeadJet(PHCompositeNode *topNode); // get leading jet from user-defined method

    std::vector<fastjet::PseudoJet> JetsToPseudojets(const std::vector<Jet *> &particles, bool randomize_etaphi=false); // convert jet objects to pseudojets

    void GetConeAxis(PHCompositeNode *topNode, float &cone_eta, float &cone_phi, bool avoid_lead_jet=false); // get random cone axis
};

#endif // RHOBASE_DETERMINETOWERRHO_H
