#include <iostream>
#include <limits>
#include <vector>

class EventInfo {
    public:
        EventInfo() = default;
        virtual ~EventInfo() = default;

        double get_z_vtx() {return z_vtx;};
        void set_z_vtx(double vtx){ z_vtx = vtx; };

        double get_ZDC_rate() {return ZDC_rate;};
        void set_ZDC_rate(double rate){ ZDC_rate = rate; };

        bool is_dijet_event() { return dijet_event; };
        void set_dijet_event(bool dijet){ dijet_event = dijet; };

        float get_lead_pT() {return lead_pT;};
        void set_lead_pT(float pT){ lead_pT = pT; };

        float get_sublead_pT() {return sublead_pT;};
        void set_sublead_pT(float pT){ sublead_pT = pT; };

        bool is_dijetTruth_event() { return dijetTruth_event; };
        void set_dijetTruth_event(bool dijet){ dijetTruth_event = dijet; };

        float get_leadTruth_pT() {return leadTruth_pT;};
        void set_leadTruth_pT(float pT){ leadTruth_pT = pT; };

        float get_subleadTruth_pT() {return subleadTruth_pT;};
        void set_subleadTruth_pT(float pT){ subleadTruth_pT = pT; };

    private:
        double z_vtx{0.0};
        double ZDC_rate{0.0};
        bool dijet_event = false;
        float lead_pT{-999};
        float sublead_pT{-999};
        bool dijetTruth_event = true;
        float leadTruth_pT{-999};
        float subleadTruth_pT{-999};

    };