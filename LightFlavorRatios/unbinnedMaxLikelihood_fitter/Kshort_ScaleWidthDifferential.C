#ifndef KSHORT_SCALEWIDTHDIFFERENTIAL_C
#define KSHORT_SCALEWIDTHDIFFERENTIAL_C

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <format>
#include <functional>
#include <iomanip>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

#include <TCut.h>
#include <TSystem.h>

#include "./RDataframeToRoofit.C"
#include "./RDataframeToRoofit_SOnly.C"
#include "./fitutil.h"

namespace
{
inline void replace_all(std::string &s, const std::string &from, const std::string &to)
{
    if (from.empty())
        return;
    size_t pos = 0;
    while ((pos = s.find(from, pos)) != std::string::npos)
    {
        s.replace(pos, from.size(), to);
        pos += to.size();
    }
}

inline std::string to_hex_u64(std::uint64_t v)
{
    std::ostringstream oss;
    oss << std::hex << std::setw(16) << std::setfill('0') << v;
    return oss.str();
}

inline std::string shorten_tag_if_needed(const std::string &tag, std::size_t maxlen = 180)
{
    if (tag.size() <= maxlen)
        return tag;

    const std::uint64_t h = static_cast<std::uint64_t>(std::hash<std::string>{}(tag));
    const std::string suffix = "_h" + to_hex_u64(h);

    const std::size_t keep = (maxlen > suffix.size()) ? (maxlen - suffix.size()) : 0;
    return tag.substr(0, keep) + suffix;
}

inline std::string trim_number(const std::string &text)
{
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(6) << std::stod(text);

    std::string out = oss.str();
    while (out.size() > 1 && out.back() == '0')
        out.pop_back();
    if (!out.empty() && out.back() == '.')
        out.push_back('0');
    return out;
}

struct CutRangeText
{
    bool has_low = false;
    bool has_high = false;
    std::string low;
    std::string high;
    std::string low_bracket = "[";
    std::string high_bracket = ")";
};

inline CutRangeText parse_cut_range(const std::string &cut_string, const std::string &variable)
{
    CutRangeText range;
    const std::regex re(variable + R"(\s*(>=|<=|==|>|<)\s*([-+]?[0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?))");

    for (std::sregex_iterator it(cut_string.begin(), cut_string.end(), re), end; it != end; ++it)
    {
        const std::string op = (*it)[1].str();
        const std::string value = trim_number((*it)[2].str());

        if (op == ">=" || op == ">")
        {
            range.has_low = true;
            range.low = value;
            range.low_bracket = (op == ">=") ? "[" : "(";
        }
        else if (op == "<=" || op == "<")
        {
            range.has_high = true;
            range.high = value;
            range.high_bracket = (op == "<=") ? "]" : ")";
        }
        else if (op == "==")
        {
            range.has_low = true;
            range.has_high = true;
            range.low = value;
            range.high = value;
            range.low_bracket = "[";
            range.high_bracket = "]";
        }
    }

    return range;
}

inline void add_range_text(std::vector<std::string> &entries, const CutRangeText &range, const std::string &label, const std::string &separator, const std::string &unit)
{
    if (range.has_low && range.has_high)
    {
        entries.push_back(label + " = " + range.low_bracket + range.low + separator + range.high + range.high_bracket + unit);
    }
    else if (range.has_low)
    {
        entries.push_back(label + " #geq " + range.low + unit);
    }
    else if (range.has_high)
    {
        entries.push_back(label + " < " + range.high + unit);
    }
}

inline std::vector<std::string> make_cut_legend_entries(const std::string &cut_string)
{
    std::vector<std::string> entries;

    add_range_text(entries, parse_cut_range(cut_string, "K_S0_pT"), "p_{T}", "-", " GeV");
    add_range_text(entries, parse_cut_range(cut_string, "K_S0_pseudorapidity"), "#eta", ", ", "");
    add_range_text(entries, parse_cut_range(cut_string, "K_S0_phi"), "#phi", ", ", " rad");

    const std::regex min_track_pt_re(R"(min\s*\(\s*track_1_pT\s*,\s*track_2_pT\s*\)\s*(>=|<=|==|>|<)\s*([-+]?[0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?))");
    std::smatch min_track_pt_match;
    if (std::regex_search(cut_string, min_track_pt_match, min_track_pt_re))
    {
        const std::string op = min_track_pt_match[1].str();
        const std::string value = trim_number(min_track_pt_match[2].str());
        if (op == ">=")
            entries.push_back("min track p_{T} " + value + " GeV");
        else
            entries.push_back("min track p_{T} " + op + " " + value + " GeV");
    }

    return entries;
}
} // namespace

std::string modify(std::string str)
{
    replace_all(str, ">=", "geq");
    replace_all(str, "<=", "leq");
    replace_all(str, "==", "eq");
    replace_all(str, "&&", "AND");
    replace_all(str, "||", "OR");
    replace_all(str, ">", "gt");
    replace_all(str, "<", "lt");

    std::replace(str.begin(), str.end(), '.', 'p'); // decimal point
    std::replace(str.begin(), str.end(), '-', 'm'); // minus sign (negative numbers)
    std::replace(str.begin(), str.end(), '+', 'p'); // plus sign

    for (char &c : str)
    {
        const bool ok = (std::isalnum(static_cast<unsigned char>(c)) != 0) || (c == '_');
        if (!ok)
            c = '_';
    }

    std::string out;
    out.reserve(str.size());
    bool prev_us = false;
    for (char c : str)
    {
        if (c == '_')
        {
            if (!prev_us)
                out.push_back(c);
            prev_us = true;
        }
        else
        {
            out.push_back(c);
            prev_us = false;
        }
    }

    // Trim leading/trailing underscores
    while (!out.empty() && out.front() == '_')
        out.erase(out.begin());
    while (!out.empty() && out.back() == '_')
        out.pop_back();

    return out;
}

void configure_fitparam(const std::string cutstring, fitparam_config &fit_conf)
{
    if (cutstring == "K_S0_pT>=0.60&&K_S0_pT<0.70&&K_S0_phi>=1.00&&K_S0_phi<2.00")
    {
        // Mass window and binning
        fit_conf.minMass = 0.44;
        fit_conf.maxMass = 0.56;
        fit_conf.nBins = 60;

        // Signal starting params / ranges
        fit_conf.sigmodel = "Gaussian";
        fit_conf.mean = 0.5;
        fit_conf.mean_low = 0.485;
        fit_conf.mean_high = 0.51;

        fit_conf.sigma = 5e-3;
        fit_conf.sigma_low = 1e-3;
        fit_conf.sigma_high = 8e-3;

        // Background model
        fit_conf.bkgmodel = "Polynomial3";
        fit_conf.p1 = -1.6724e+00;
        fit_conf.p1_low = -10.;
        fit_conf.p1_high = 10.;

        // fit_conf.p2 = 0;
        fit_conf.p2_low = -10.;
        fit_conf.p2_high = 10.;

        // fit_conf.p3 = 0;
        fit_conf.p3_low = -10.;
        fit_conf.p3_high = 10.;

        fit_conf.nSig = 0.0;     // if > 0, will be used as fixed starting value
        fit_conf.nSig_low = 0.0; // if > 0, will be used as fixed starting value
        fit_conf.nBkg = 0.0;     // if > 0, will be used as fixed starting value
        fit_conf.nBkg_low = 0.0; // if > 0, will be used as fixed starting value
    }
    else if (cutstring == "K_S0_pT>=0.60&&K_S0_pT<0.70&&K_S0_phi>=2.00&&K_S0_phi<3.15")
    {
        // Mass window and binning
        fit_conf.minMass = 0.44;
        fit_conf.maxMass = 0.56;
        fit_conf.nBins = 60;

        // Signal starting params / ranges
        fit_conf.sigmodel = "Gaussian";
        fit_conf.mean = 0.488;
        fit_conf.mean_low = 0.48;
        fit_conf.mean_high = 0.51;

        fit_conf.sigma = 4e-3;
        fit_conf.sigma_low = 1e-3;
        fit_conf.sigma_high = 8e-3;

        // Background model
        fit_conf.bkgmodel = "Polynomial3";
        fit_conf.p1 = -1.6724e+00;
        fit_conf.p1_low = -10.;
        fit_conf.p1_high = 10.;

        // fit_conf.p2 = 0;
        fit_conf.p2_low = -10.;
        fit_conf.p2_high = 10.;

        // fit_conf.p3 = 0;
        fit_conf.p3_low = -10.;
        fit_conf.p3_high = 10.;

        fit_conf.nSig = 0.0;     // if > 0, will be used as fixed starting value
        fit_conf.nSig_low = 0.0; // if > 0, will be used as fixed starting value
        fit_conf.nBkg = 0.0;     // if > 0, will be used as fixed starting value
        fit_conf.nBkg_low = 0.0; // if > 0, will be used as fixed starting value
    }
    else if (cutstring == "K_S0_pT>=0.80&&K_S0_pT<0.90&&K_S0_phi>=-1.00&&K_S0_phi<0.00")
    {
        // Mass window and binning
        fit_conf.minMass = 0.44;
        fit_conf.maxMass = 0.56;
        fit_conf.nBins = 60;

        // Signal starting params / ranges
        fit_conf.sigmodel = "Gaussian";
        fit_conf.mean = 0.488;
        fit_conf.mean_low = 0.48;
        fit_conf.mean_high = 0.51;

        fit_conf.sigma = 4e-3;
        fit_conf.sigma_low = 1e-3;
        fit_conf.sigma_high = 8e-3;

        // Background model
        fit_conf.bkgmodel = "Polynomial3";
        fit_conf.p1 = -1.6724e+00;
        fit_conf.p1_low = -10.;
        fit_conf.p1_high = 10.;

        // fit_conf.p2 = 0;
        fit_conf.p2_low = -10.;
        fit_conf.p2_high = 10.;

        // fit_conf.p3 = 0;
        fit_conf.p3_low = -10.;
        fit_conf.p3_high = 10.;

        fit_conf.nSig = 0.0;     // if > 0, will be used as fixed starting value
        fit_conf.nSig_low = 0.0; // if > 0, will be used as fixed starting value
        fit_conf.nBkg = 0.0;     // if > 0, will be used as fixed starting value
        fit_conf.nBkg_low = 0.0; // if > 0, will be used as fixed starting value
    }
    else if (cutstring == "K_S0_pT>=0.90&&K_S0_pT<1.00&&K_S0_phi>=-2.00&&K_S0_phi<-1.00")
    {
        // Mass window and binning
        fit_conf.minMass = 0.44;
        fit_conf.maxMass = 0.56;
        fit_conf.nBins = 60;

        // Signal starting params / ranges
        fit_conf.sigmodel = "Gaussian";
        fit_conf.mean = 0.488;
        fit_conf.mean_low = 0.48;
        fit_conf.mean_high = 0.51;

        fit_conf.sigma = 4e-3;
        fit_conf.sigma_low = 1e-3;
        fit_conf.sigma_high = 8e-3;

        // Background model
        fit_conf.bkgmodel = "Polynomial3";
        fit_conf.p1 = -1.6724e+00;
        fit_conf.p1_low = -10.;
        fit_conf.p1_high = 10.;

        // fit_conf.p2 = 0;
        fit_conf.p2_low = -10.;
        fit_conf.p2_high = 10.;

        // fit_conf.p3 = 0;
        fit_conf.p3_low = -10.;
        fit_conf.p3_high = 10.;

        fit_conf.nSig = 0.0;     // if > 0, will be used as fixed starting value
        fit_conf.nSig_low = 0.0; // if > 0, will be used as fixed starting value
        fit_conf.nBkg = 0.0;     // if > 0, will be used as fixed starting value
        fit_conf.nBkg_low = 0.0; // if > 0, will be used as fixed starting value
    }
    else if (cutstring == "K_S0_pT>=1.50&&K_S0_pT<1.80&&K_S0_phi>=0.00&&K_S0_phi<1.00")
    {
        // Mass window and binning
        fit_conf.minMass = 0.44;
        fit_conf.maxMass = 0.56;
        fit_conf.nBins = 60;

        // Signal starting params / ranges
        fit_conf.sigmodel = "Gaussian";
        fit_conf.mean = 0.488;
        fit_conf.mean_low = 0.48;
        fit_conf.mean_high = 0.51;

        fit_conf.sigma = 4e-3;
        fit_conf.sigma_low = 1e-3;
        fit_conf.sigma_high = 15e-3;

        // Background model
        fit_conf.bkgmodel = "Polynomial3";
        fit_conf.p1 = -1.6724e+00;
        fit_conf.p1_low = -10.;
        fit_conf.p1_high = 10.;

        // fit_conf.p2 = 0;
        fit_conf.p2_low = -10.;
        fit_conf.p2_high = 10.;

        // fit_conf.p3 = 0;
        fit_conf.p3_low = -10.;
        fit_conf.p3_high = 10.;

        fit_conf.nSig = 0.0;     // if > 0, will be used as fixed starting value
        fit_conf.nSig_low = 0.0; // if > 0, will be used as fixed starting value
        fit_conf.nBkg = 0.0;     // if > 0, will be used as fixed starting value
        fit_conf.nBkg_low = 0.0; // if > 0, will be used as fixed starting value
    }
    else
    {
        // Mass window and binning
        fit_conf.minMass = 0.44;
        fit_conf.maxMass = 0.56;
        fit_conf.nBins = 60;

        // Signal starting params / ranges
        fit_conf.sigmodel = "Gaussian";
        fit_conf.mean = 0.5;
        fit_conf.mean_low = 0.485;
        fit_conf.mean_high = 0.51;

        fit_conf.sigma = 1e-2;
        fit_conf.sigma_low = 1e-3;
        fit_conf.sigma_high = 5e-1;

        // Tail params (used for CB/DoubleCB only)
        fit_conf.alpha1 = 0.5;
        fit_conf.alpha1_low = 1E-3;
        fit_conf.alpha1_high = 5.0;

        fit_conf.n1 = 10;
        fit_conf.n1_low = 5;
        fit_conf.n1_high = 1E3;

        fit_conf.alpha2 = 0.5;
        fit_conf.alpha2_low = 1E-3;
        fit_conf.alpha2_high = 5.0;

        fit_conf.n2 = 10;
        fit_conf.n2_low = 5;
        fit_conf.n2_high = 1E3;

        fit_conf.frac = 0.5;

        // Background model
        fit_conf.bkgmodel = "Polynomial3";
        fit_conf.p1 = -1.6724e+00;
        fit_conf.p1_low = -10.;
        fit_conf.p1_high = 10.;

        // fit_conf.p2 = 0;
        fit_conf.p2_low = -10.;
        fit_conf.p2_high = 10.;

        // fit_conf.p3 = 0;
        fit_conf.p3_low = -10.;
        fit_conf.p3_high = 10.;

        // fitconfig.nSig_fracTol = 0.7;
        // fitconfig.nSig_fracTol_low = 0.5;
        // fitconfig.nSig_fracTol_high = 1.00;
        // fitconfig.nBkg_fracTol = 1.0 - fitconfig.nSig_fracTol;
        // fitconfig.nBkg_fracTol_low = 1.0 - fitconfig.nSig_fracTol_high;
        // fitconfig.nBkg_fracTol_high = 1.0;
        fit_conf.nSig = 0.0;     // if > 0, will be used as fixed starting value
        fit_conf.nSig_low = 0.0; // if > 0, will be used as fixed starting value
        fit_conf.nBkg = 0.0;     // if > 0, will be used as fixed starting value
        fit_conf.nBkg_low = 0.0; // if > 0, will be used as fixed starting value
    }
}

void Kshort_ScaleWidthDifferential(                                                           //
    std::string additionalCuts = "K_S0_pT>=1.50&&K_S0_pT<1.80&&K_S0_phi>=0.00&&K_S0_phi<1.00" //
)
{
    const std::string figure_dir = "./figure_kshort_crosscheck/";
    system(("mkdir -p " + figure_dir).c_str());

    const bool isMC = false;

    // TCut KS0_optimizeCut = "K_S0_DIRA >= 0.985000 && K_S0_decayLength > 0.125000 && min(abs(track_1_IP_xy), abs(track_2_IP_xy)) >= 0.020000";
    const TCut KS0_optimizeCut = "min(track_1_pT, track_2_pT) >= 0.2";
    const TCut KSO_additionalCuts(additionalCuts.c_str());
    const std::vector<std::string> cut_legend_entries = make_cut_legend_entries(additionalCuts + "&&" + KS0_optimizeCut.GetTitle());

    const TCut totalselection = KS0_optimizeCut && KSO_additionalCuts;
    std::cout << "Selection: " << totalselection.GetTitle() << std::endl;

    std::string plotdir_prefix = modify(additionalCuts);
    plotdir_prefix = shorten_tag_if_needed(plotdir_prefix);
    std::cout << "Plotdir prefix: " << plotdir_prefix << std::endl;

    const std::string plotdir = figure_dir + "/" + plotdir_prefix;
    system(("mkdir -p " + plotdir).c_str());

    // Fit configuration defaults
    fitparam_config fitconfig;
    fitconfig.branch = "K_S0_mass";
    fitconfig.decaystring = "K_{S}^{0}#rightarrow#pi^{-}#pi^{+}";

    // Signal model
    // fitconfig.sigmodel = "DoubleCrystalBall";
    // fitconfig.sigmodel = "Gaussian";

    configure_fitparam(additionalCuts, fitconfig);

    // // Mass window and binning
    // fitconfig.minMass = (isMC) ? 0.45 : 0.44;
    // fitconfig.maxMass = (isMC) ? 0.54 : 0.56;
    // fitconfig.nBins = (isMC) ? 90 : 60;

    // // Signal starting params / ranges
    // fitconfig.mean = 0.5;
    // fitconfig.mean_low = 0.485;
    // fitconfig.mean_high = 0.51;

    // fitconfig.sigma = 1e-2;
    // fitconfig.sigma_low = 1e-3;
    // fitconfig.sigma_high = 5e-1;

    // // Tail params (used for CB/DoubleCB only)
    // fitconfig.alpha1 = 0.5;
    // fitconfig.alpha1_low = 1E-3;
    // fitconfig.alpha1_high = 5.0;

    // fitconfig.n1 = 10;
    // fitconfig.n1_low = 5;
    // fitconfig.n1_high = 1E3;

    // fitconfig.alpha2 = 0.5;
    // fitconfig.alpha2_low = 1E-3;
    // fitconfig.alpha2_high = 5.0;

    // fitconfig.n2 = 10;
    // fitconfig.n2_low = 5;
    // fitconfig.n2_high = 1E3;

    // fitconfig.frac = 0.5;

    // // Background model
    // fitconfig.bkgmodel = "Polynomial3";
    // fitconfig.p1 = -1.6724e+00;
    // fitconfig.p1_low = -10.;
    // fitconfig.p1_high = 10.;

    // // fitconfig.p2 = 0;
    // fitconfig.p2_low = -10.;
    // fitconfig.p2_high = 10.;

    // // fitconfig.p3 = 0;
    // fitconfig.p3_low = -10.;
    // fitconfig.p3_high = 10.;

    // // fitconfig.nSig_fracTol = 0.7;
    // // fitconfig.nSig_fracTol_low = 0.5;
    // // fitconfig.nSig_fracTol_high = 1.00;
    // // fitconfig.nBkg_fracTol = 1.0 - fitconfig.nSig_fracTol;
    // // fitconfig.nBkg_fracTol_low = 1.0 - fitconfig.nSig_fracTol_high;
    // // fitconfig.nBkg_fracTol_high = 1.0;
    // fitconfig.nSig = 0.0;     // if > 0, will be used as fixed starting value
    // fitconfig.nSig_low = 0.0; // if > 0, will be used as fixed starting value
    // fitconfig.nBkg = 0.0;     // if > 0, will be used as fixed starting value
    // fitconfig.nBkg_low = 0.0; // if > 0, will be used as fixed starting value

    // fitconfig.useStagedFit = false;

    if (!isMC)
    {
        const bool doSnapshot = true;
        const std::string snapshotName = std::format("./snapshot/filtered_tree_{}.root", plotdir_prefix);
        // const std::string inputFile = "/sphenix/user/hjheng/sPHENIXRepo/TrackingAnalysis/Physics_Val_TF/misc/KFParticle_merged/merged_pipi_reco_00079516.root";
        const std::string inputFile = "/sphenix/tg/tg01/hf/aopatton/SVLooseJun4/6RunsCombinedKShortSVLoose.root";

        const double significance = RDataframeToRoofit(doSnapshot, snapshotName, inputFile, totalselection, fitconfig, plotdir, cut_legend_entries);

        std::cout << "Returned significance: " << significance << std::endl;
    }
    else
    {
        const bool doSnapshot = false;
        const std::string snapshotName = "./snapshot/filtered_tree.root";
        const std::string inputFile = "/sphenix/user/hjheng/sPHENIXRepo/TrackingAnalysis/Physics_Val_TF/misc/KFParticle_merged/merged_pipi_reco_00079516.root";

        RDataframeToRoofit_SOnly(doSnapshot, snapshotName, inputFile, totalselection, fitconfig, plotdir);
    }
}

#endif // KSHORT_SCALEWIDTHDIFFERENTIAL_C
