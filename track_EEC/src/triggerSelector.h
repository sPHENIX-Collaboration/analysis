// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef triggerSelector_H
#define triggerSelector_H

#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllReturnCodes.h>

//trigger
#include <ffarawobjects/Gl1Packetv2.h>
#include <ffarawobjects/Gl1Packetv1.h>
#include <calotrigger/TriggerAnalyzer.h>
#include <calotrigger/TriggerRunInfov1.h>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include <vector>
#include <string>

class PHCompositeNode;
class TriggerAnalyzer; 

class triggerSelector : public SubsysReco
{
    public:
    
        triggerSelector(const std::string &name = "VandyJetDSTSkimmer");
        ~triggerSelector () override = default;

        int Init(PHCompositeNode *topNode) override;

        int process_event(PHCompositeNode *topNode) override;

        int End(PHCompositeNode *topNode) override;

        void setTriggerName(std::string name){m_validTrigNames.push_back(name); };
  
    private:

        std::vector<std::string> m_validTrigNames;

};

#endif
