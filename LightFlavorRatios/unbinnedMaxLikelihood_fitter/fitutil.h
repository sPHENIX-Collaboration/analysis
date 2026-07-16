#ifndef FITUTIL_H
#define FITUTIL_H

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <TString.h>

#include <RooAbsArg.h>
#include <RooAbsPdf.h>
#include <RooAddPdf.h>
#include <RooArgList.h>
#include <RooArgSet.h>
#include <RooFormulaVar.h>
#include <RooRealVar.h>

#include <RooGaussian.h>
#include <RooVoigtian.h>
#include <RooCBShape.h>

#include <RooPolynomial.h>
#include <RooArgusBG.h>

struct fitparam_config
{
    std::string branch;
    std::string decaystring;

    int nBins = 60;

    std::string sigmodel;
    double minMass = 0.0, maxMass = 1.0;

    double nSig = 0.0;
    double nSig_low = 0.0, nSig_high = 1e12;
    double nSig_fracTol = 0.35; // fraction of total entries
    double nSig_fracTol_low = 0.0, nSig_fracTol_high = 1.0;

    double mean = 0.0, mean_low = 0.0, mean_high = 1.0;
    double sigma = 0.01, sigma_low = 1e-6, sigma_high = 0.2;
    double width = 0.002, width_low = 1e-6, width_high = 0.1;

    double alpha1 = 1.0, alpha1_low = 1e-3, alpha1_high = 100.0;
    double n1 = 5.0, n1_low = 0.1, n1_high = 200.0;
    double alpha2 = 1.0, alpha2_low = 1e-3, alpha2_high = 100.0;
    double n2 = 5.0, n2_low = 0.1, n2_high = 200.0;
    double frac = 0.5;

    std::string bkgmodel;

    double nBkg = 0.0;
    double nBkg_low = 0.0, nBkg_high = 1e12;
    double nBkg_fracTol = 1.0 - nSig_fracTol; // fraction of total entries
    double nBkg_fracTol_low = 0.0, nBkg_fracTol_high = 1.0;

    // Polynomial coeffs (up to 10)
    double p1 = 0.0, p1_low = -100.0, p1_high = 100.0;
    double p2 = 0.0, p2_low = -100.0, p2_high = 100.0;
    double p3 = 0.0, p3_low = -100.0, p3_high = 100.0;
    double p4 = 0.0, p4_low = -100.0, p4_high = 100.0;
    double p5 = 0.0, p5_low = -100.0, p5_high = 100.0;
    double p6 = 0.0, p6_low = -100.0, p6_high = 100.0;
    double p7 = 0.0, p7_low = -100.0, p7_high = 100.0;
    double p8 = 0.0, p8_low = -100.0, p8_high = 100.0;
    double p9 = 0.0, p9_low = -100.0, p9_high = 100.0;
    double p10 = 0.0, p10_low = -100.0, p10_high = 100.0;

    // Argus
    double k = -10.0, k_low = -200.0, k_high = -1e-3;
    double edp = 0.0, edp_low = -10.0, edp_high = 10.0;
    double argusShift = 0.0;
    double argusfrac = 0.5;

    bool useStagedFit = false; 
    double stagedGapNSigma = 3.0;
};

namespace FitParams
{
std::string branch = "Lambda0_mass";
double minMass = 2.1;
double maxMass = 2.4;

RooRealVar mass("mass", "mass", 0.0, -10.0, 10.0);

RooRealVar nSig("nSig", "nSig", 10000.0, 0.0, 1e12);
RooRealVar nBkg("nBkg", "nBkg", 100000.0, 0.0, 1e12);

RooRealVar mean("mean", "mean", 0.0, -10.0, 10.0);
RooRealVar sigma("sigma", "sigma", 0.01, 1e-7, 1.0);
RooRealVar width("width", "width", 0.002, 1e-6, 0.2);
RooRealVar alpha1("alpha1", "alpha1", 1.0, 1e-3, 100.0);
RooRealVar n1("n1", "n1", 5.0, 0.1, 200.0);
RooRealVar alpha2("alpha2", "alpha2", 1.0, 1e-3, 100.0);
RooRealVar n2("n2", "n2", 5.0, 0.1, 200.0);
RooRealVar frac("frac", "frac", 0.5, 0.0, 1.0);

RooRealVar p1("p1", "coeff #1", 0.0, -100.0, 100.0);
RooRealVar p2("p2", "coeff #2", 0.0, -100.0, 100.0);
RooRealVar p3("p3", "coeff #3", 0.0, -100.0, 100.0);
RooRealVar p4("p4", "coeff #4", 0.0, -100.0, 100.0);
RooRealVar p5("p5", "coeff #5", 0.0, -100.0, 100.0);
RooRealVar p6("p6", "coeff #6", 0.0, -100.0, 100.0);
RooRealVar p7("p7", "coeff #7", 0.0, -100.0, 100.0);
RooRealVar p8("p8", "coeff #8", 0.0, -100.0, 100.0);
RooRealVar p9("p9", "coeff #9", 0.0, -100.0, 100.0);
RooRealVar p10("p10", "coeff #10", 0.0, -100.0, 100.0);

RooRealVar k("k", "k", -10.0, -200.0, -1e-3);
RooRealVar edp("edp", "edp", 0.0, -10.0, 10.0);
double ArgusShift = 0.0;
RooRealVar frac_argus("frac_argus", "Fraction of Argus", 0.5, 0.0, 1.0);

static std::vector<std::unique_ptr<RooAbsArg>> _owned_nodes;

inline void BeginFit() { _owned_nodes.clear(); }

template <typename T, typename... Args> inline T *Own(Args &&...args)
{
    auto up = std::make_unique<T>(std::forward<Args>(args)...);
    T *ptr = up.get();
    _owned_nodes.emplace_back(std::move(up));
    return ptr;
}

inline RooArgList PolyCoeffList(int order)
{
    RooArgList coeffs;
    if (order >= 1)
        coeffs.add(p1);
    if (order >= 2)
        coeffs.add(p2);
    if (order >= 3)
        coeffs.add(p3);
    if (order >= 4)
        coeffs.add(p4);
    if (order >= 5)
        coeffs.add(p5);
    if (order >= 6)
        coeffs.add(p6);
    if (order >= 7)
        coeffs.add(p7);
    if (order >= 8)
        coeffs.add(p8);
    if (order >= 9)
        coeffs.add(p9);
    if (order >= 10)
        coeffs.add(p10);
    return coeffs;
}

inline int ParsePolyOrder(const std::string &modelType)
{
    const std::string prefix = "Polynomial";
    if (modelType.rfind(prefix, 0) != 0)
        return -1;
    const std::string suffix = modelType.substr(prefix.size());
    if (suffix.empty())
        return -1;
    try
    {
        int n = std::stoi(suffix);
        if (n < 1 || n > 10)
            return -1;
        return n;
    }
    catch (...)
    {
        return -1;
    }
}
} // namespace FitParams

inline RooAbsPdf *signalModel(const std::string &modelType, RooRealVar &mass)
{
    using namespace FitParams;

    if (modelType == "Gaussian")
    {
        return Own<RooGaussian>("signal", "Gaussian", mass, mean, sigma);
    }
    else if (modelType == "Voigtian")
    {
        return Own<RooVoigtian>("signal", "Voigtian", mass, mean, sigma, width);
    }
    else if (modelType == "CrystalBall")
    {
        return Own<RooCBShape>("signal", "Crystal Ball", mass, mean, sigma, alpha1, n1);
    }
    else if (modelType == "DoubleCrystalBall")
    {
        return Own<RooDoubleCB>("signal", "Double Crystal Ball", mass, mean, sigma, alpha1, n1, alpha2, n2, frac);
    }

    std::cerr << "[fitutil.h] ERROR: Unknown signal model type: " << modelType << " (returning nullptr)\n";
    return nullptr;
}

inline RooAbsPdf *backgroundModel(const std::string &modelType, RooRealVar &mass)
{
    using namespace FitParams;

    const int polyOrder = ParsePolyOrder(modelType);
    if (polyOrder >= 1)
    {
        RooArgList coeffs = PolyCoeffList(polyOrder);
        return Own<RooPolynomial>("background", modelType.c_str(), mass, coeffs);
    }

    if (modelType == "Argus")
    {
        const TString formula = TString::Format("-%s+2*%.17g", mass.GetName(), ArgusShift);

        RooArgList deps(mass);
        auto *shifted = Own<RooFormulaVar>("shifted", "shifted", formula.Data(), deps);

        auto *argus = Own<RooArgusBG>("Argus", "Argus", *shifted, edp, k);

        RooArgList polyCoeffs = PolyCoeffList(8);
        auto *poly = Own<RooPolynomial>("Polynomial", "Polynomial", mass, polyCoeffs);

        RooArgList pdfs(*argus, *poly);
        RooArgList fracs(frac_argus);
        return Own<RooAddPdf>("background", "Composite Background", pdfs, fracs);
    }

    std::cerr << "[fitutil.h] ERROR: Unknown background model type: " << modelType << " (returning nullptr)\n";
    return nullptr;
}

#endif // FITUTIL_H
