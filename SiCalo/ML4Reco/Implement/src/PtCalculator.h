#ifndef PT_CALCULATOR_H
#define PT_CALCULATOR_H

#include <functional>
#include <string>
#include <variant>
#include <vector>
#include <optional>
#include <limits>
#include <array>
#include <memory>

namespace SiCaloPt {

// consistent return struct
struct PtResult 
{
    float pt_reco = std::numeric_limits<float>::quiet_NaN(); // reconstructed pt
    bool ok = false; // whether successful
    std::string err; // error message if any
};

// Five approaches enum
enum class Method 
{
    MethodEMD,   // EM Deflection Method
    MethodEproj, // Energy Projection Method
    MethodMLEMD, // ML EM Deflection Method
    MethodMLEproj, // ML Energy Projection Method
    MethodMLCombined // ML Combined Method
};

// Formula method input struct
struct InputEMD 
{
    float EMD_Angle = 0.f; // delta_phi - EM Deflection angle in rad is between two vectors, one vector connect inner INTT and outer INTT,  another one connect outer INTT and Calo cluster
    float EMD_Eta = 0.f; // track eta
    float EMD_Radius = 0.f; // EMCal Cluster radius in cm
};

struct InputEproj 
{
    float Energy_Calo = 0.f; // EMCal Cluster energy in GeV
    float Radius_Calo = 0.f; // EMCal Cluster radius in cm
    float Z_Calo = 0.f; // EMCal Cluster z in cm
    float Radius_vertex = 0.f; // Vertex radius in cm
    float Z_vertex = 0.f; // Vertex z in cm
};

// ML Model input struct, using vector is convenient for ONNX, length and order must be consistent with training
// Defination of length and order is in the tutorial file PtCalcMLTutorial.C
struct InputMLEMD 
{
    std::vector<float> features; // 2-d features:{ 1/dphi_EMD, eta_track}
};

struct InputMLEproj 
{
    std::vector<float> features; // 7-d input features:{ INTT 3/4 layer R, INTT 3/4 layer Z, INTT 5/4 layer R, INTT 5/4 layer Z, Calo cluster R, Calo cluster Z, Calo cluster Energy }
};

struct InputMLCombined 
{
    std::vector<float> features; // 2-d features:{pT reconstuct with deflection information, pT reconstuct with Calo energy information}
};

// consistent input variant for all methods
using AnyInput = std::variant<InputEMD, InputEproj, InputMLEMD, InputMLEproj, InputMLCombined>;

// Config(optional) for ML: ML paths, standardizer params, etc.
struct PtCalculatorConfig 
{
    std::optional<std::string> mlEMD_model_path;
    std::optional<std::string> mlEproj_model_path;
    std::optional<std::string> mlCombined_model_path;

    std::optional<std::string> mlEMD_scaler_json;
    std::optional<std::string> mlEproj_scaler_json;
    std::optional<std::string> mlCombined_scaler_json;
};

// PtCalculator MAIN CLASS 
class PtCalculator {
public:
    // Main Function. general func. for Pt calculation, dispatching according to method
    PtResult ComputePt(Method method, const AnyInput& input) const;

    // independent interfaces for finer control
    PtResult ComputeEMD(const InputEMD& in) const; // using electromagnetic deflection to calculate pT by analytic formula pT = C_eta × |Δφ|^(Power)
    PtResult ComputeEproj(const InputEproj& in) const; // project the Calo cluster energy to XY-plane to get pT by analytic formula pT = Energy_Calo × (ΔR / Distance) 
    PtResult ComputeMLEMD(const InputMLEMD& in) const; // machine learning model trained for the EMD method, the features are 2-d features: { 1/dphi_EMD, eta_track} please setting the eta_track = 0, cause I just add the dimension but not trained with the eta data now
    PtResult ComputeMLEproj(const InputMLEproj& in) const; // machine learning model trained for the Eproj method, the features are 7-d input features:{ INTT 3/4 layer R, INTT 3/4 layer Z, INTT 5/4 layer R, INTT 5/4 layer Z, Calo R, Calo Z, Calo Energy }
    PtResult ComputeMLCombined(const InputMLCombined& in) const; // machine learning model trained to combine the pT estimates obtained from two different methods based on two independent sets of information {deflection, energy}; the final combined pT is given by pT = w1 * pT(deflection) + w2 * pT(energy), where the weights w1 and w2 are predicted by the model for deflection and energy

    // EMD formula parameters setters
    void setParCeta(float v)  { m_par_Ceta = v; }
    void setParPower(float v) { m_par_Power = v; }

    // member functions
    PtCalculator() = default;
    explicit PtCalculator(const PtCalculatorConfig& cfg);

    // setting/configuration (does not auto-load external resources)
    void setConfig(const PtCalculatorConfig& cfg);

    // initialize (load models and scalers for ML methods)
    bool init(std::string* err = nullptr);

    // load ML infer, output is pt
    using InferFn = std::function<float(const std::vector<float>&)>;
    void setMLEMDInfer(InferFn fn);
    void setMLEprojInfer(InferFn fn);
    void setMLCombinedInfer(InferFn fn);

    //  optional: set standardizer for each ML model, or do standardization inside InferFn
    void setMLEMDStandardizer(std::vector<float> mean, std::vector<float> scale);
    void setMLEprojStandardizer(std::vector<float> mean, std::vector<float> scale);
    void setMLCombinedStandardizer(std::vector<float> mean, std::vector<float> scale);

private:
    static void applyStandardize(std::vector<float>& x,
                                 const std::vector<float>& mean,
                                 const std::vector<float>& scale);

    static bool LoadScalerJson(const std::string& path,
                                std::vector<float>& mean,
                                std::vector<float>& scale,
                                std::string* err = nullptr);

    static SiCaloPt::PtCalculator::InferFn MakeOnnxInfer(const std::string& onnx_path, std::string* err = nullptr);

private:
    PtCalculatorConfig m_cfg;

    // ML Model infer functions
    InferFn m_mlEMD_infer;
    InferFn m_mlEproj_infer;
    InferFn m_mlCombined_infer;

    // optional: ML Scaler params for standardization, not exist for EMD and Combined model now
    std::vector<float> m_mlEMD_mean, m_mlEMD_scale;
    std::vector<float> m_mlEproj_mean, m_mlEproj_scale; 
    std::vector<float> m_mlCombined_mean, m_mlCombined_scale;

    // EMD formula parameters
    mutable float m_par_Ceta = 0.2;
    mutable float m_par_Power = -1.0;

    bool consider_eta_dependence_on_EMDcompute = true;

    bool charge_of_track_is_negative = false;
    bool charge_of_track_is_positive = false;
};

} // namespace SiCaloPt

#endif // PT_CALCULATOR_H
