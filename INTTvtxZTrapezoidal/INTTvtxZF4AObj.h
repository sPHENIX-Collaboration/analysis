#ifndef INTTVTXZF4AOBJ_H
#define INTTVTXZF4AOBJ_H

#include <phool/PHObject.h>
#include <phool/phool.h>

#include <iostream>
#include <cmath>

class INTTvtxZF4AObj : public PHObject
{
    public:
        INTTvtxZF4AObj();
        
        // void identify(std::ostream& os = std::cout) const override;
        void Reset() override;
        int isValid() const override;

        double INTTvtxZ;
        double INTTvtxZError;
        double NgroupTrapezoidal;
        double NgroupCoarse;
        double TrapezoidalFitWidth;
        double TrapezoidalFWHM;
        long long NClusAll;
        long long NClusAllInner;
    private:
        ClassDefOverride(INTTvtxZF4AObj, 1);
};

#endif // INTTVTXZF4AOBJ_H