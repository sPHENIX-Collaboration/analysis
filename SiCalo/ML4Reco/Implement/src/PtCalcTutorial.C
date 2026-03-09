#include <TSystem.h>
#include <iostream>
#include "PtCalculator.h"

void PtCalcTutorial() 
{
    // 先加载你的 .so
    gSystem->Load("libPtCalc.so");

    // ===== 下面保持你的原逻辑不变 =====
    SiCaloPt::PtCalculatorConfig cfg;        // 如果你的头文件里实际叫 PtCalculator::Config，请改成那种写法
    SiCaloPt::PtCalculator       calc(cfg);
    std::string err;
    if (!calc.init(&err)) {
        std::cout << "init failed: " << err << std::endl;
    }

    // EMD
    {
        SiCaloPt::InputEMD in;
        in.EMD_Angle  = 0.1f;
        in.EMD_Eta    = 0.0f;
        in.EMD_Radius = 93.5f;
        auto r = calc.ComputePt(SiCaloPt::Method::MethodEMD, SiCaloPt::AnyInput{in});
        std::cout << "[EMD] ok=" << r.ok << "  pt=" << r.pt_reco << "  err=\"" << r.err << "\"\n";
    }

    // Eproj
    {
        SiCaloPt::InputEproj in;
        in.Energy_Calo   = 8.f;
        in.Radius_Calo   = 100.f;
        in.Z_Calo        = 0.0f;
        in.Radius_vertex = 0.f;
        in.Z_vertex      = 100.0f;
        auto r = calc.ComputePt(SiCaloPt::Method::MethodEproj, SiCaloPt::AnyInput{in});
        std::cout << "[Eproj] ok=" << r.ok << "  pt=" << r.pt_reco << "  err=\"" << r.err << "\"\n";
    }
}
