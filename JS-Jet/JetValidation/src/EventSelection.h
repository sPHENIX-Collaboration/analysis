// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef EVENTSELECTION_H
#define EVENTSELECTION_H

#include <fun4all/SubsysReco.h>
#include <jetbase/Jetv1.h>

#include <memory>
#include <string>
#include <utility>  // std::pair, std::make_pair
#include <vector>

#include <TFile.h>
class PHCompositeNode;
class TTree;

class EventSelection : public SubsysReco
{
public:
    // constructor
    EventSelection(const double jet_R = 0.4,
            const std::string &outputfilename = "/Run23745_ana399_DSTs/selectiontree.root");

    ~EventSelection() override; // destructor

    void setVzCut(double cut) { m_vtxZ_cut = cut; }
    //    void add_input(JetInput *input) { _inputs.push_back(input); }
    
    // Standard Fun4All functions
    int Init(PHCompositeNode *topNode) override;
    int process_event(PHCompositeNode *topNode) override;
    int End(PHCompositeNode *topNode) override;

private:

    // private variables
    double m_jet_R;
    std::string m_outputfilename;
    double m_vtxZ_cut;
    // std::vector<JetInput *> _inputs;
    //____________________________________________________________________________..
    // output tree variables
    int m_event;
     // output trees
    TTree *m_tree;
    TFile* outFile;
    std::vector<float> m_vertex_z;
    
};

#endif // EVENTSELECTION_H
