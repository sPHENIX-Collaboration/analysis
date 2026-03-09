#include <iostream>
#include <limits>
#include <vector>

class JetInfo {
    public:
        JetInfo() = default;
        ~JetInfo();

        double px() {return Px;};
        void set_px(double tpx) {Px = tpx;};
        
        double py() {return Py;};
        void set_py(double tpy) {Py = tpy;};

        double pz() {return Pz;};
        void set_pz(double tpz) {Pz = tpz;};

        double e() {return E;};
        void set_e(double te) {E = te;};

        std::vector<int> get_constituents() { return constituents; };
        void set_constituents (std::vector<int> jetCons) { constituents = jetCons; };

        void CopyTo(JetInfo *jet);
    private:
        double Px;
        double Py;
        double Pz;
        double E;
        std::vector<int> constituents;
};