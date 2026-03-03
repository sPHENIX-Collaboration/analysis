#include <iostream>
#include <limits>
#include <vector>

class Tower {
    public:
        Tower() = default;
        ~Tower();

        void CopyTo(Tower* tower);


        double px() { return Px; };
        void set_px(double tmpPx) { Px = tmpPx; };

        double py() { return Py; };
        void set_py(double tmpPy) { Py = tmpPy; };

        double pz() { return Pz; };
        void set_pz(double tmpPz) { Pz = tmpPz; };

        double e() { return E; };
        void set_e(double tmpE) { E = tmpE; };
        

        int get_calo() { return calo; };
        void set_calo(int tmpCalo) { calo = tmpCalo; };

    private:
        double Px;
        double Py;
        double Pz;
        double E;
        int calo; //0=EMCal, 1=retowered EMCal, 2=IHCal, 3=OHCal

        //ClassDef(Tower,1);

};
