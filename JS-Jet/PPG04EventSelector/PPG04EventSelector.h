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

        // Do event selection on leading R=0.4 truth jet pT range
        void do_MCLeadingTruthJetpT_cut(float low, float high)
        {
            m_do_MCLeadingTruthJetpT_cut = true;
            m_MCLeadingTruthJetpT_range.first = low;
            m_MCLeadingTruthJetpT_range.second = high;
        }

        // MinBias event selection
        void do_minBias_cut(bool doCut) { m_do_minBias_cut = doCut; }

        // zvtx cut
        void do_zvtx_cut(float low, float high)
        {
            m_do_zvtx_cut = true;
            m_zvtx_range.first = low;
            m_zvtx_range.second = high;
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
        bool m_do_MCLeadingTruthJetpT_cut{false};
        std::pair<float, float> m_MCLeadingTruthJetpT_range{0.0,1000.0};
        bool MCLeadingTruthJetpT_cut(PHCompositeNode *topNode);

        // MinBias event selection
        bool m_do_minBias_cut{false};
        bool minBias_cut(PHCompositeNode *topNode);

        // zvtx cut
        bool m_do_zvtx_cut{false};
        std::pair<float, float> m_zvtx_range{-60.0,60.0};
        bool zvtx_cut(PHCompositeNode *topNode);



};

#endif // PPG04_EVENTSELECTOR_H
