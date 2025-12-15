#include <TSystem.h>
#include <TStopwatch.h>
#include <iostream>
#include <vector>

// #include "PtCalculator.h"  // SiCaloPt::PtCalculator & friends
#include "/sphenix/user/jzhang1/testcode4all/INTT-EMCAL/InttSeedingTrackDev/ML4Reco/Implement/src/PtCalculator.h"  // SiCaloPt::PtCalculator & friends
R__LOAD_LIBRARY(/sphenix/user/jzhang1/testcode4all/INTT-EMCAL/InttSeedingTrackDev/ML4Reco/Implement/src/libPtCalc.so)

// ---- Weights(onnx) and Scalers(json) Path ---------------------------
struct DemoPaths
{
    std::string emd_onnx          = "/sphenix/user/jzhang1/testcode4all/INTT-EMCAL/InttSeedingTrackDev/ML4Reco/Implement/ML_Weight_Scaler/model_MLEMD.onnx"; 
    std::string emd_scaler_json   = "";

    std::string eproj_onnx        = "/sphenix/user/jzhang1/testcode4all/INTT-EMCAL/InttSeedingTrackDev/ML4Reco/Implement/ML_Weight_Scaler/model_MLEproj.onnx"; 
    std::string eproj_scaler_json = "/sphenix/user/jzhang1/testcode4all/INTT-EMCAL/InttSeedingTrackDev/ML4Reco/Implement/ML_Weight_Scaler/scaler_MLEproj.json"; 

    std::string combined_onnx         = "/sphenix/user/jzhang1/testcode4all/INTT-EMCAL/InttSeedingTrackDev/ML4Reco/Implement/ML_Weight_Scaler/model_MLCombined.onnx"; 
    std::string combined_scaler_json  = "";
};

// ---- turn string into optional<string> for Config ------------------------
template<typename Opt>
Opt make_opt(const std::string& s) 
{
    if (s.empty()) return std::nullopt;
    return s;
}

// ---- PtCalc Tutorial -------------------------------------
void PtCalcMLTutorial()
{
    // Load PtCalc shared library
    // gSystem->Load("libPtCalc.so");
    // gSystem->Load("/sphenix/user/jzhang1/testcode4all/INTT-EMCAL/InttSeedingTrackDev/ML4Reco/Implement/src/libPtCalc.so");

    // Use appropriate paths in your environment, the default "DemoPaths" setup is correct here for mine
    DemoPaths WS_Path;  

    // SiCaloPt::PtCalculatorConfig setting(if you want to use the ML models, Formula-method donnot need these)
    SiCaloPt::PtCalculatorConfig cfg;
    cfg.mlEMD_model_path        = make_opt<decltype(cfg.mlEMD_model_path)>(WS_Path.emd_onnx);
    cfg.mlEMD_scaler_json       = make_opt<decltype(cfg.mlEMD_scaler_json)>(WS_Path.emd_scaler_json);
    cfg.mlEproj_model_path      = make_opt<decltype(cfg.mlEproj_model_path)>(WS_Path.eproj_onnx);
    cfg.mlEproj_scaler_json     = make_opt<decltype(cfg.mlEproj_scaler_json)>(WS_Path.eproj_scaler_json);
    cfg.mlCombined_model_path   = make_opt<decltype(cfg.mlCombined_model_path)>(WS_Path.combined_onnx);
    cfg.mlCombined_scaler_json  = make_opt<decltype(cfg.mlCombined_scaler_json)>(WS_Path.combined_scaler_json);

    // PtCalculator instance
    SiCaloPt::PtCalculator calcTutorial(cfg);
    
    // initialize (load models and scalers for ML methods)
    std::string err;
    if (!calcTutorial.init(&err)) 
    {
        std::cout << "[init] failed: " << err << std::endl;
        return;
    }
    std::cout << "[init] OK\n";
 
    // ======================== EMD Formula ==========================
    {
        SiCaloPt::InputEMD in;
        in.EMD_Angle  = 0.025;  // delta_phi - EM Deflection angle in rad
        in.EMD_Eta    = 0.00;   // EMCal Cluster eta
        in.EMD_Radius = 93.5;   // EMCal Cluster radius in cm

        calcTutorial.setParCeta(0.2);   // set C_eta parameter if needed
        calcTutorial.setParPower(-1.0); // set Power parameter if needed

        auto r = calcTutorial.ComputePt(SiCaloPt::Method::MethodEMD, SiCaloPt::AnyInput{in});
        std::cout << "[EMD-analytic] ok=" << r.ok
                  << "  pt=" << r.pt_reco
                  << "  err=\"" << r.err << "\"\n";
    }

    // ======================== Eproj Formula ==========================
    {
        SiCaloPt::InputEproj in;
        in.Energy_Calo   = 1.8;   // EMCal Cluster energy in GeV
        in.Radius_Calo   = 93.5;  // EMCal Cluster radius in cm
        in.Z_Calo        = 0.0;   // EMCal Cluster z in cm
        in.Radius_vertex = 0.0;   // Vertex radius in cm
        in.Z_vertex      = 0.0;   // Vertex z in cm

        auto r = calcTutorial.ComputePt(SiCaloPt::Method::MethodEproj, SiCaloPt::AnyInput{in});
        std::cout << "[Eproj-analytic] ok=" << r.ok
                  << "  pt=" << r.pt_reco
                  << "  err=\"" << r.err << "\"\n";
    }

    // ============= ML：MLEMD (2-d input: 1/dphi_EMD, eta_track = 0 now) ===============
    {
        // 2-d input features:{ dphi_EMD, eta_track }
        std::vector<float> featsMLEMD = {15, 0};

        SiCaloPt::InputMLEMD in{featsMLEMD};
        auto r = calcTutorial.ComputePt(SiCaloPt::Method::MethodMLEMD, SiCaloPt::AnyInput{in});
        std::cout << "[MLEMD-2D] ok=" << r.ok
                << "  pt=" << r.pt_reco
                << "  err=\"" << r.err << "\"\n";
    }

    // ====== ML：MLEproj (7-d input: INTT 3/4 layer R,Z, INTT 5/4 layer R,Z, Calo R,Z,Energy) ======
    {
        // 7-d input features:{ INTT 3/4 layer R, INTT 3/4 layer Z, INTT 5/4 layer R, INTT 5/4 layer Z, Calo R, Calo Z, Calo Energy }
        std::vector<float> featsMLEproj = { 10.0,  5.0,   // INTT 3/4 layer
                                            15.0,  7.5,   // INTT 5/4 layer
                                            100.0, 50.0, 8.0 }; // Calo R,Z,Energy

        SiCaloPt::InputMLEproj in{featsMLEproj};
        auto r = calcTutorial.ComputePt(SiCaloPt::Method::MethodMLEproj, SiCaloPt::AnyInput{in});
        std::cout << "[MLEproj-7D] ok=" << r.ok
                << "  pt=" << r.pt_reco
                << "  err=\"" << r.err << "\"\n";
    }

    // ============ ML：Combined/Gate (2-d input: pt_from_MLEMD, pt_from_MLEproj) ===================
    {
        // 2-d input features:{ pt_from_MLEMD, pt_from_MLEproj }
        std::vector<float> featsMLCombined = {8.0, 9.5};

        SiCaloPt::InputMLCombined in{featsMLCombined};
        auto r = calcTutorial.ComputePt(SiCaloPt::Method::MethodMLCombined, SiCaloPt::AnyInput{in});
        std::cout << "[MLCombined] ok=" << r.ok
                << "  pt=" << r.pt_reco
                << "  err=\"" << r.err << "\"\n";
    }

    // // ======================== Batch example ========================
    // {
    //     std::vector<SiCaloPt::InputEproj> batch(1000);
    //     for (size_t i = 0; i < batch.size(); ++i) {
    //     auto& x = batch[i];
    //     x.Energy_Calo   = 1.0 + 0.001 * i;
    //     x.Radius_Calo   = 93.5;
    //     x.Z_Calo        = 0.0;
    //     x.Radius_vertex = 0.0;
    //     x.Z_vertex      = 0.0;
    //     }

    //     TStopwatch sw; sw.Start();
    //     double sum_pt = 0.0; size_t ok_cnt = 0;
    //     for (auto& x : batch) {
    //     auto r = calcTutorial.ComputePt(SiCaloPt::Method::MethodEproj, SiCaloPt::AnyInput{x});
    //     if (r.ok) { sum_pt += r.pt_reco; ++ok_cnt; }
    //     }
    //     sw.Stop();

    //     std::cout << "[Batch Eproj] ok=" << ok_cnt << "/" << batch.size()
    //             << "  avg_pt=" << (ok_cnt ? sum_pt/ok_cnt : 0.0)
    //             << "  time=" << sw.RealTime() << " s\n";
    // }
}
