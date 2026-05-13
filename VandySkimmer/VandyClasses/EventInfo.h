#include <iostream>
#include <limits>
#include <vector>

class EventInfo {
    public:
        EventInfo() = default;
        virtual ~EventInfo() = default;

        float get_z_vtx() {return z_vtx;};
        void set_z_vtx(float vtx){ z_vtx = vtx; };

        float get_z_vtx_truth() {return z_vtx_truth;};
        void set_z_vtx_truth(float vtx){ z_vtx_truth = vtx; };        

        float get_ZDC_rate() {return ZDC_rate;};
        void set_ZDC_rate(float rate){ ZDC_rate = rate; };

        float get_cross_section() {return cross_section;};
        void set_cross_section(float cs){ cross_section = cs; };

        bool is_dijet_event(int jetR_index) { return dijet_event[jetR_index]; };
        void set_dijet_event(int jetR_index, bool dijet){ dijet_event[jetR_index] = dijet; };

        float get_lead_pT(int jetR_index) {return lead_pT[jetR_index];};
        void set_lead_pT(int jetR_index, float pT){ lead_pT[jetR_index] = pT; };

        float get_sublead_pT(int jetR_index) {return sublead_pT[jetR_index];};
        void set_sublead_pT(int jetR_index, float pT){ sublead_pT[jetR_index] = pT; };

        bool is_dijetTruth_event(int jetR_index) { return dijetTruth_event[jetR_index]; };
        void set_dijetTruth_event(int jetR_index, bool dijet){ dijetTruth_event[jetR_index] = dijet; };

        float get_leadTruth_pT(int jetR_index) {return leadTruth_pT[jetR_index];};
        void set_leadTruth_pT(int jetR_index, float pT){ leadTruth_pT[jetR_index] = pT; };

        float get_subleadTruth_pT(int jetR_index) {return subleadTruth_pT[jetR_index];};
        void set_subleadTruth_pT(int jetR_index, float pT){ subleadTruth_pT[jetR_index] = pT; };
          
	float get_dijetDeltatTruth(int jetR_index) { return dijetDeltatTruth[jetR_index]; };
        void set_dijetDeltatTruth(int jetR_index, float dijet){ dijetDeltatTruth[jetR_index] = dijet; };

        bool is_dijetDeltatPass() { return dijetDeltatPass; };
        void set_dijetDeltatPass(bool dijet){ dijetDeltatPass = dijet; };
       
	bool is_dijetDeltatTruthPass(int jetR_index) { return dijetDeltatTruthPass[jetR_index]; };
        void set_dijetDeltatTruthPass(int jetR_index, bool dijet){ dijetDeltatTruthPass[jetR_index] = dijet; };

        float get_dijetDeltaPhi(int jetR_index) { return dijetDeltaPhi[jetR_index]; };
        void set_dijetDeltaPhi(int jetR_index, float dijet){ dijetDeltaPhi[jetR_index] = dijet; };
       
	float get_dijetDeltaPhiTruth(int jetR_index) { return dijetDeltaPhiTruth[jetR_index]; };
        void set_dijetDeltaPhiTruth(int jetR_index, float dijet){ dijetDeltaPhiTruth[jetR_index] = dijet; };

        float get_leadJetTime() { return leadJetTime; };
        void set_leadJetTime(float time){ leadJetTime = time; };

        float get_subJetTime() { return subJetTime; };
        void set_subJetTime(float time){ subJetTime = time; };

        float get_MBDTime() { return MBDTime; };
        void set_MBDTime(float time){ MBDTime = time; };

        bool get_leadJetTimePass() { return leadJetTimePass; };
        void set_leadJetTimePass(bool timePass){ leadJetTimePass = timePass; };

        bool get_leadJetMBDDeltatPass() { return leadJetMBDDeltatPass; };
        void set_leadJetMBDDeltatPass(bool timePass){ leadJetMBDDeltatPass = timePass; };

    
    private:
        float 	z_vtx{0.0};
        float 	z_vtx_truth{0.0};
        float 	ZDC_rate{0.0};
         
	bool 	dijet_event[5] = {false, false, false, false, false};
        bool 	dijetTruth_event[5] = {false, false, false, false, false};
	
        float   leadJetTime = -999;
        float   subJetTime = -999;
        float   MBDTime = -999;
	float 	dijetDeltatTruth[5] {-999, -999, -999, -999, -999};
	bool	leadJetTimePass = false;
	bool	leadJetMBDDeltatPass = false;
	bool	dijetDeltatPass = false;
	bool 	dijetDeltatTruthPass[5] {false, false, false, false, false};

	float 	dijetDeltaPhi[5] {-999, -999, -999, -999};
	float 	dijetDeltaPhiTruth[5] {-999, -999, -999, -999, -999};

        float 	cross_section{0.0};
       
       	float 	lead_pT[5] = {-999, -999, -999, -999, -999};
        float 	sublead_pT[5] = {-999, -999, -999, -999, -999};
        float 	leadTruth_pT[5] = {-999, -999, -999, -999, -999};
        float 	subleadTruth_pT[5] = {-999, -999, -999, -999, -999};
};
