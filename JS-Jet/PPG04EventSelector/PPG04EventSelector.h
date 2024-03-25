// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef PPG04_EVENTSELECTOR_H
#define PPG04_EVENTSELECTOR_H

#include <fun4all/SubsysReco.h>

#include <string>
#include <utility> 

// Simple module to select events based on various criteria
/// template for cut:
// 1. setter for cut
// 2. bool for cut
// 3. value for cut
// 4. function to apply cut (return bool)

class PHCompositeNode;

class PPG04EventSelector : public SubsysReco
{
    public:

        PPG04EventSelector(const std::string &name = "PPG04EventSelector") : SubsysReco(name) {};
        ~PPG04EventSelector() override {};

        int process_event(PHCompositeNode *topNode) override;

        // template for cut
        void do_A_cut(double value_criteria)
        {
            m_do_A_cut = true;
            m_A_cut = value_criteria;
        }

        // bad chi2
        void do_badChi2_cut(bool doCut) { m_do_badChi2_cut = doCut; }

        / Do event selection on leading R=0.4 truth jet pT range
        void do_MC_event_select_lead_truth_jet(float low, float high)
        {
            m_MC_do_event_select = true;
            m_MC_event_select_jetpT_range.first = low;
            m_MC_event_select_jetpT_range.second = high;
        }

    private:

        // template for cut
        bool m_do_A_cut{false};
        double m_A_cut{0};
        bool A_cut(PHCompositeNode *topNode);

        // chi2 cut
        bool m_do_badChi2_cut{false};
        bool badChi2_cut(PHCompositeNode *topNode);

        // event trigger (for simulation)
        bool m_MC_do_event_select{false};
        std::pair<float, float> m_MC_event_select_jetpT_range{-1,1000};
        bool MC_event_select(PHCompositeNode *topNode);

};

#endif // BACKGROUNDANABASE_EVENTSELECTOR_H
