gSystem->Load("libPtCalc.so");
#include "PtCalculator.h"

void PtCalcTutorial() 
{
    // 构造 & init
    SiCaloPt::PtCalculatorConfig cfg;
    SiCaloPt::PtCalculator       calc(cfg);
    std::string err;
    if (!calc.init(&err)) {
        std::cout << "init failed: " << err << std::endl;
    }

    // EMD
    {
        SiCaloPt::InputEMD in;
        in.EMD_Angle  = 0.03f;
        in.EMD_Eta    = 0.0f;
        in.EMD_Radius = 93.5f;
        auto r = calc.ComputePt(SiCaloPt::Method::MethodEMD, SiCaloPt::AnyInput{in});
        std::cout << "[EMD] ok=" << r.ok << "  pt=" << r.pt_reco << "  err=\"" << r.err << "\"\n";
    }

    // Eproj
    {
        SiCaloPt::InputEproj in;
        in.Energy_Calo   = 1.5f;
        in.Radius_Calo   = 93.5f;
        in.Z_Calo        = 0.0f;
        in.Radius_vertex = 0.0f;
        in.Z_vertex      = 0.0f;
        auto r = calc.ComputePt(SiCaloPt::Method::MethodEproj, SiCaloPt::AnyInput{in});
        std::cout << "[Eproj] ok=" << r.ok << "  pt=" << r.pt_reco << "  err=\"" << r.err << "\"\n";
    }
}
