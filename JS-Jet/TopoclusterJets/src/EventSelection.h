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
  EventSelection( const std::string &recojetname = "AntiKt_Tower_r04_Sub1",
		   const std::string &outputfilename = "selecteventtest.root");

    ~EventSelection() override; // destructor

    void setVzCut(double cut) { m_vtxZ_cut = cut; }
    
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
    
    std::string m_outputfilename;

    TFile *outFile;
    TTree *m_tree;

    double m_vtxZ_cut;
    int m_event;
    float m_vertex_z;
    
};

#endif // EVENTSELECTION_H
