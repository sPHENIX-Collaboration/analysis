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

    add_range_text(entries, parse_cut_range(cut_string, "Lambda0_pT"), "p_{T}", "-", " GeV");
    add_range_text(entries, parse_cut_range(cut_string, "Lambda0_pseudorapidity"), "#eta", ", ", "");
    add_range_text(entries, parse_cut_range(cut_string, "Lambda0_phi"), "#phi", ", ", " rad");

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
    if (cutstring == "Lambda0_pT>=0.50&&Lambda0_pT<0.60")
    {
        fit_conf.minMass = 1.113;
        fit_conf.maxMass = 1.15;
        fit_conf.nBins = 37; // ~5 MeV bins

        fit_conf.sigmodel = "Gaussian";
        fit_conf.mean = 1.1160e+00;
        fit_conf.mean_low = 1.113;
        fit_conf.mean_high = 1.125;

        fit_conf.sigma = 2.7158e-03;
        fit_conf.sigma_low = 1e-4;
        fit_conf.sigma_high = 0.005;

        fit_conf.bkgmodel = "Polynomial3";
        fit_conf.p1 = -8.5720e+00;
        fit_conf.p1_low = -10.;
        fit_conf.p1_high = 10.;

        fit_conf.p2 = 1.4117e+00;
        fit_conf.p2_low = -10.;
        fit_conf.p2_high = 10.;

        fit_conf.p3 = 4.9278e+00;
        fit_conf.p3_low = -10.;
        fit_conf.p3_high = 10.;

        fit_conf.nSig = 1.3395e+03;  // if > 0, will be used as fixed starting value
        fit_conf.nSig_low = 1300.0;  // if > 0, will be used as fixed starting value
        fit_conf.nBkg_high = 1500.0; // if > 0, will be used as fixed starting value
        fit_conf.nBkg = 0.0;
    }
    else if (cutstring == "Lambda0_pT>=0.60&&Lambda0_pT<0.70")
    {
        fit_conf.minMass = 1.105;
        fit_conf.maxMass = 1.15;
        fit_conf.nBins = 45; // ~5 MeV bins

        fit_conf.sigmodel = "Gaussian";
        fit_conf.mean = 1.1160e+00;
        fit_conf.mean_low = 1.113;
        fit_conf.mean_high = 1.125;

        fit_conf.sigma = 2.7158e-03;
        fit_conf.sigma_low = 1e-4;
        fit_conf.sigma_high = 0.005;

        fit_conf.bkgmodel = "Polynomial3";
        fit_conf.p1 = -8.5720e+00;
        fit_conf.p1_low = -10.;
        fit_conf.p1_high = 10.;

        fit_conf.p2 = 1.4117e+00;
        fit_conf.p2_low = -10.;
        fit_conf.p2_high = 10.;

        fit_conf.p3 = 4.9278e+00;
        fit_conf.p3_low = -10.;
        fit_conf.p3_high = 10.;

        fit_conf.nSig = 0;          // if > 0, will be used as fixed starting value
        fit_conf.nSig_low = 1300.0; // if > 0, will be used as fixed starting value
        fit_conf.nBkg_high = 0;     // if > 0, will be used as fixed starting value
        fit_conf.nBkg = 0.0;
    }
    else if (cutstring.find("Lambda0_pT>=0.70&&Lambda0_pT<0.80") != std::string::npos)
    {
        fit_conf.minMass = 1.1;
        fit_conf.maxMass = 1.15;
        fit_conf.nBins = 50; // ~5 MeV bins

        fit_conf.sigmodel = "Gaussian";
        fit_conf.mean = 1.1160e+00;
        fit_conf.mean_low = 1.113;
        fit_conf.mean_high = 1.125;

        fit_conf.sigma = 2.7158e-03;
        fit_conf.sigma_low = 1e-4;
        fit_conf.sigma_high = 0.01;

        if (cutstring.find("Lambda0_phi>=2.00&&Lambda0_phi<3.15") != std::string::npos)
        {
            fit_conf.mean = 1.1130e+00;
            fit_conf.mean_low = 1.11;
            fit_conf.mean_high = 1.115;

            fit_conf.sigma = 2.87e-03;
            fit_conf.sigma_low = 1.5e-3;
            fit_conf.sigma_high = 3.0e-3;
        }
        else if (cutstring.find("Lambda0_phi>=-2.00&&Lambda0_phi<-1.00") != std::string::npos)
        {
            fit_conf.mean = 1.1140e+00;
            fit_conf.mean_low = 1.11;
            fit_conf.mean_high = 1.115;

            fit_conf.sigma = 3e-03;
            fit_conf.sigma_low = 1.5e-3;
            fit_conf.sigma_high = 3.0e-3;
        }

        fit_conf.bkgmodel = "Polynomial3";
        fit_conf.p1 = -8.5720e+00;
        fit_conf.p1_low = -10.;
        fit_conf.p1_high = 10.;

        fit_conf.p2 = 1.4117e+00;
        fit_conf.p2_low = -10.;
        fit_conf.p2_high = 10.;

        fit_conf.p3 = 4.9278e+00;
        if (cutstring.find("Lambda0_phi>=-2.00&&Lambda0_phi<-1.00") != std::string::npos)
        {
            fit_conf.p3 = 0;
        }
        fit_conf.p3_low = -10.;
        fit_conf.p3_high = 10.;

        fit_conf.nSig = 0;          // if > 0, will be used as fixed starting value
        fit_conf.nSig_low = 1300.0; // if > 0, will be used as fixed starting value
        fit_conf.nBkg_high = 0;     // if > 0, will be used as fixed starting value
        fit_conf.nBkg = 0.0;
        if (cutstring.find("Lambda0_phi>=-2.00&&Lambda0_phi<-1.00") != std::string::npos)
        {
            fit_conf.nSig = 4500;       // if > 0, will be used as fixed starting value
            fit_conf.nSig_low = 1300.0; // if > 0, will be used as fixed starting value
            fit_conf.nBkg_high = 0;     // if > 0, will be used as fixed starting value
            fit_conf.nBkg = 0.0;
        }
    }
    else if (cutstring.find("Lambda0_pT>=0.80&&Lambda0_pT<0.90") != std::string::npos)
    {
        fit_conf.minMass = 1.095;
        fit_conf.maxMass = 1.14;
        fit_conf.nBins = 45; // ~5 MeV bins

        fit_conf.sigmodel = "Gaussian";
        fit_conf.mean = 1.1160e+00;
        fit_conf.mean_low = 1.113;
        fit_conf.mean_high = 1.125;
        if (cutstring.find("Lambda0_phi>=-2.00&&Lambda0_phi<-1.00") != std::string::npos)
        {
            fit_conf.mean = 1.1160e+00;
            fit_conf.mean_low = 1.113;
            fit_conf.mean_high = 1.12;
        }

        fit_conf.sigma = 2.7158e-03;
        fit_conf.sigma_low = 1e-4;
        fit_conf.sigma_high = 0.01;
        if (cutstring.find("Lambda0_phi>=-2.00&&Lambda0_phi<-1.00") != std::string::npos)
        {
            fit_conf.sigma = 2.7158e-03;
            fit_conf.sigma_low = 1e-4;
            fit_conf.sigma_high = 0.005;
        }
        else if (cutstring.find("Lambda0_phi>=1.00&&Lambda0_phi<2.00") != std::string::npos)
        {
            fit_conf.sigma = 2.5e-03;
            fit_conf.sigma_low = 0.002;
            fit_conf.sigma_high = 5e-3;
        }
        else if (cutstring.find("Lambda0_phi>=2.00&&Lambda0_phi<3.15") != std::string::npos)
        {
            fit_conf.sigma = 2.5e-03;
            fit_conf.sigma_low = 0.002;
            fit_conf.sigma_high = 5e-3;
        }

        fit_conf.bkgmodel = "Polynomial3";
        fit_conf.p1 = -8.5720e+00;
        fit_conf.p1_low = -10.;
        fit_conf.p1_high = 10.;

        fit_conf.p2 = 1.4117e+00;
        fit_conf.p2_low = -10.;
        fit_conf.p2_high = 10.;

        fit_conf.p3 = 4.9278e+00;
        fit_conf.p3_low = -10.;
        fit_conf.p3_high = 10.;

        fit_conf.nSig = 0;          // if > 0, will be used as fixed starting value
        fit_conf.nSig_low = 1300.0; // if > 0, will be used as fixed starting value
        fit_conf.nBkg_high = 0;     // if > 0, will be used as fixed starting value
        fit_conf.nBkg = 0.0;
    }
    else if (cutstring.find("Lambda0_pT>=0.90&&Lambda0_pT<1.00") != std::string::npos)
    {
        std::cout << "Using custom fit config for Lambda0_pT>=0.90&&Lambda0_pT<1.00" << std::endl;
        fit_conf.minMass = 1.095;
        fit_conf.maxMass = 1.14;
        fit_conf.nBins = 45; // ~5 MeV bins

        fit_conf.sigmodel = "Gaussian";
        fit_conf.mean = 1.1180e+00;
        fit_conf.mean_low = 1.113;
        fit_conf.mean_high = 1.123;

        fit_conf.sigma = 2.61e-03;
        fit_conf.sigma_low = 1e-4;
        fit_conf.sigma_high = 5.0e-03;
        if (cutstring.find("Lambda0_phi>=-1.00&&Lambda0_phi<0.00") != std::string::npos)
        {
            fit_conf.mean = 1.1150e+00;
            fit_conf.mean_low = 1.11;
            fit_conf.mean_high = 1.118;

            fit_conf.sigma = 2.5e-03;
            fit_conf.sigma_low = 0.002;
            fit_conf.sigma_high = 0.01;
        }
        else if (cutstring.find("Lambda0_phi>=2.00&&Lambda0_phi<3.15") != std::string::npos)
        {
            fit_conf.mean = 1.1180e+00;
            fit_conf.mean_low = 1.113;
            fit_conf.mean_high = 1.123;

            fit_conf.sigma = 2.5e-03;
            fit_conf.sigma_low = 0.002;
            fit_conf.sigma_high = 0.01;
        }
        else if (cutstring.find("Lambda0_phi>=-2.00&&Lambda0_phi<-1.00") != std::string::npos)
        {
            fit_conf.mean = 1.1160e+00;
            fit_conf.mean_low = 1.113;
            fit_conf.mean_high = 1.12;

            fit_conf.sigma = 2.7158e-03;
            fit_conf.sigma_low = 1e-4;
            fit_conf.sigma_high = 4.5e-3;
        }

        fit_conf.bkgmodel = "Polynomial3";
        fit_conf.p1 = -8.5720e+00;
        fit_conf.p1_low = -10.;
        fit_conf.p1_high = 10.;

        fit_conf.p2 = 1.4117e+00;
        fit_conf.p2_low = -10.;
        fit_conf.p2_high = 10.;

        fit_conf.p3 = 4.9278e+00;
        fit_conf.p3_low = -10.;
        fit_conf.p3_high = 10.;

        fit_conf.nSig = 0;          // if > 0, will be used as fixed starting value
        fit_conf.nSig_low = 1300.0; // if > 0, will be used as fixed starting value
        fit_conf.nBkg_high = 0;     // if > 0, will be used as fixed starting value
        fit_conf.nBkg = 0.0;
    }
    else if (cutstring.find("Lambda0_pT>=1.00&&Lambda0_pT<1.10") != std::string::npos)
    {
        fit_conf.minMass = 1.09;
        fit_conf.maxMass = 1.14;
        fit_conf.nBins = 50; // ~5 MeV bins

        fit_conf.sigmodel = "Gaussian";
        fit_conf.mean = 1.1160e+00;
        fit_conf.mean_low = 1.113;
        fit_conf.mean_high = 1.125;
        fit_conf.sigma = 2.7158e-03;
        fit_conf.sigma_low = 1e-4;
        fit_conf.sigma_high = 0.005;
        if (cutstring.find("Lambda0_phi>=-1.00&&Lambda0_phi<0.00") != std::string::npos)
        {
            fit_conf.mean = 1.1130e+00;
            fit_conf.mean_low = 1.11;
            fit_conf.mean_high = 1.115;

            fit_conf.sigma = 2.87e-03;
            fit_conf.sigma_low = 0.0023;
            fit_conf.sigma_high = 0.003;
        }

        fit_conf.bkgmodel = "Polynomial3";
        fit_conf.p1 = -8.5720e+00;
        fit_conf.p1_low = -10.;
        fit_conf.p1_high = 10.;

        fit_conf.p2 = 1.4117e+00;
        fit_conf.p2_low = -10.;
        fit_conf.p2_high = 10.;

        fit_conf.p3 = 4.9278e+00;
        if (cutstring.find("Lambda0_phi>=-1.00&&Lambda0_phi<0.00") != std::string::npos)
        {
            fit_conf.p3 = 0;
        }
        fit_conf.p3_low = -10.;
        fit_conf.p3_high = 10.;

        fit_conf.nSig = 0.0;     // if > 0, will be used as fixed starting value
        fit_conf.nSig_low = 0.0; // if > 0, will be used as fixed starting value
        fit_conf.nBkg = 0.0;     // if > 0, will be used as fixed starting value
        fit_conf.nBkg_low = 0.0; // if > 0, will be used as fixed starting value
        if (cutstring.find("Lambda0_phi>=-1.00&&Lambda0_phi<0.00") != std::string::npos)
        {
            fit_conf.nSig = 8500.0;     // if > 0, will be used as fixed starting value
            fit_conf.nSig_low = 3000.0; // if > 0, will be used as fixed starting value
        }
    }
    else if (cutstring.find("Lambda0_pT>=1.10&&Lambda0_pT<1.20&&Lambda0_phi>=0.00&&Lambda0_phi<1.00") != std::string::npos)
    {
        fit_conf.minMass = 1.09;
        fit_conf.maxMass = 1.14;
        fit_conf.nBins = 50; // ~5 MeV bins

        fit_conf.sigmodel = "Gaussian";
        fit_conf.mean = 1.1160e+00;
        fit_conf.mean_low = 1.113;
        fit_conf.mean_high = 1.125;
        fit_conf.sigma = 2.7158e-03;
        fit_conf.sigma_low = 1e-4;
        fit_conf.sigma_high = 0.005;

        fit_conf.bkgmodel = "Polynomial3";
        fit_conf.p1 = -8.5720e+00;
        fit_conf.p1_low = -10.;
        fit_conf.p1_high = 10.;

        fit_conf.p2 = 1.4117e+00;
        fit_conf.p2_low = -10.;
        fit_conf.p2_high = 10.;

        fit_conf.p3 = 0;
        fit_conf.p3_low = -10.;
        fit_conf.p3_high = 10.;

        fit_conf.nSig = 0.0;     // if > 0, will be used as fixed starting value
        fit_conf.nSig_low = 0.0; // if > 0, will be used as fixed starting value
        fit_conf.nBkg = 0.0;     // if > 0, will be used as fixed starting value
        fit_conf.nBkg_low = 0.0; // if > 0, will be used as fixed starting value
    }
    else if (cutstring.find("Lambda0_pT>=1.20&&Lambda0_pT<1.30") != std::string::npos)
    {
        fit_conf.minMass = 1.09;
        fit_conf.maxMass = 1.14;
        fit_conf.nBins = 50; // ~5 MeV bins

        fit_conf.sigmodel = "Gaussian";
        fit_conf.mean = 1.1160e+00;
        fit_conf.mean_low = 1.113;
        fit_conf.mean_high = 1.125;
        fit_conf.sigma = 2.7158e-03;
        fit_conf.sigma_low = 1e-4;
        fit_conf.sigma_high = 0.005;
        if (cutstring.find("Lambda0_phi>=-1.00&&Lambda0_phi<0.00") != std::string::npos)
        {
            fit_conf.mean = 1.1130e+00;
            fit_conf.mean_low = 1.112;
            fit_conf.mean_high = 1.115;

            fit_conf.sigma = 2.87e-03;
            fit_conf.sigma_low = 0.0023;
            fit_conf.sigma_high = 5e-3;
        }

        fit_conf.bkgmodel = "Polynomial3";
        fit_conf.p1 = -8.5720e+00;
        fit_conf.p1_low = -10.;
        fit_conf.p1_high = 10.;

        fit_conf.p2 = 1.4117e+00;
        fit_conf.p2_low = -10.;
        fit_conf.p2_high = 10.;

        fit_conf.p3 = 4.9278e+00;
        fit_conf.p3_low = -10.;
        fit_conf.p3_high = 10.;

        fit_conf.nSig = 0.0;     // if > 0, will be used as fixed starting value
        fit_conf.nSig_low = 0.0; // if > 0, will be used as fixed starting value
        fit_conf.nBkg = 0.0;     // if > 0, will be used as fixed starting value
        fit_conf.nBkg_low = 0.0; // if > 0, will be used as fixed starting value
        if (cutstring.find("Lambda0_phi>=-1.00&&Lambda0_phi<0.00") != std::string::npos)
        {
            fit_conf.nSig = 3500.0;         // if > 0, will be used as fixed starting value
            fit_conf.nSig_low = 3000.0;     // if > 0, will be used as fixed starting value
            fit_conf.nBkg = 2e+03;          // if > 0, will be used as fixed starting value
            fit_conf.nBkg_low = 1.3967e+03; // if > 0,
        }
    }
    else if (cutstring.find("Lambda0_pT>=1.30&&Lambda0_pT<1.40&&Lambda0_phi>=-1.00&&Lambda0_phi<0.00") != std::string::npos)
    {
        fit_conf.minMass = 1.09;
        fit_conf.maxMass = 1.14;
        fit_conf.nBins = 50; // ~5 MeV bins

        fit_conf.sigmodel = "Gaussian";
        fit_conf.mean = 1.1160e+00;
        fit_conf.mean_low = 1.113;
        fit_conf.mean_high = 1.125;
        fit_conf.sigma = 2.7158e-03;
        fit_conf.sigma_low = 1e-4;
        fit_conf.sigma_high = 0.005;

        fit_conf.bkgmodel = "Polynomial3";
        fit_conf.p1 = -8.5720e+00;
        fit_conf.p1_low = -10.;
        fit_conf.p1_high = 10.;

        fit_conf.p2 = 1.4117e+00;
        fit_conf.p2_low = -10.;
        fit_conf.p2_high = 10.;

        fit_conf.p3 = 4.9278e+00;
        fit_conf.p3_low = -10.;
        fit_conf.p3_high = 10.;

        fit_conf.nSig = 0.0;     // if > 0, will be used as fixed starting value
        fit_conf.nSig_low = 0.0; // if > 0, will be used as fixed starting value
        fit_conf.nBkg = 0.0;     // if > 0, will be used as fixed starting value
        fit_conf.nBkg_low = 0.0; // if > 0, will be used as fixed starting value
    }
    else if (cutstring.find("Lambda0_pT>=1.50&&Lambda0_pT<1.80") != std::string::npos)
    {
        fit_conf.minMass = 1.09;
        fit_conf.maxMass = 1.14;
        fit_conf.nBins = 50;

        fit_conf.sigmodel = "Gaussian";
        fit_conf.mean = 1.1160e+00;
        fit_conf.mean_low = 1.113;
        fit_conf.mean_high = 1.125;
        fit_conf.sigma = 2.7158e-03;
        fit_conf.sigma_low = 1e-4;
        fit_conf.sigma_high = 0.005;
        if (cutstring.find("Lambda0_phi>=-3.15&&Lambda0_phi<-2.00") != std::string::npos)
        {
            fit_conf.mean = 1.1130e+00;
            fit_conf.mean_low = 1.11;
            fit_conf.mean_high = 1.115;

            fit_conf.sigma = 2.2e-03;
            fit_conf.sigma_low = 0.002;
            fit_conf.sigma_high = 0.0023;
        }
        else if (cutstring.find("Lambda0_phi>=2.00&&Lambda0_phi<3.15") != std::string::npos)
        {
            fit_conf.mean = 1.1130e+00;
            fit_conf.mean_low = 1.11;
            fit_conf.mean_high = 1.115;

            fit_conf.sigma = 2.87e-03;
            fit_conf.sigma_low = 0.0023;
            fit_conf.sigma_high = 5e-3;
        }

        fit_conf.bkgmodel = "Polynomial3";
        fit_conf.p1 = -8.5720e+00;
        fit_conf.p1_low = -10.;
        fit_conf.p1_high = 10.;

        fit_conf.p2 = 1.4117e+00;
        fit_conf.p2_low = -10.;
        fit_conf.p2_high = 10.;

        fit_conf.p3 = 4.9278e+00;
        fit_conf.p3_low = -10.;
        fit_conf.p3_high = 10.;

        fit_conf.nSig = 0.0;     // if > 0, will be used as fixed starting value
        fit_conf.nSig_low = 0.0; // if > 0, will be used as fixed starting value
        fit_conf.nBkg = 0.0;     // if > 0, will be used as fixed starting value
        fit_conf.nBkg_low = 0.0; // if > 0, will be used as fixed starting value
    }
    else
    {
        fit_conf.minMass = 1.09;
        fit_conf.maxMass = 1.14;
        fit_conf.nBins = 50;

        fit_conf.sigmodel = "Gaussian";
        fit_conf.mean = 1.1160e+00;
        fit_conf.mean_low = 1.113;
        fit_conf.mean_high = 1.125;
        fit_conf.sigma = 2.7158e-03;
        fit_conf.sigma_low = 1e-4;
        fit_conf.sigma_high = 0.08;

        fit_conf.bkgmodel = "Polynomial3";
        fit_conf.p1 = -8.5720e+00;
        fit_conf.p1_low = -10.;
        fit_conf.p1_high = 10.;

        fit_conf.p2 = 1.4117e+00;
        fit_conf.p2_low = -10.;
        fit_conf.p2_high = 10.;

        fit_conf.p3 = 4.9278e+00;
        fit_conf.p3_low = -10.;
        fit_conf.p3_high = 10.;

        fit_conf.nSig = 0.0;     // if > 0, will be used as fixed starting value
        fit_conf.nSig_low = 0.0; // if > 0, will be used as fixed starting value
        fit_conf.nBkg = 0.0;     // if > 0, will be used as fixed starting value
        fit_conf.nBkg_low = 0.0; // if > 0, will be used as fixed starting value
    }
}

// void Lambda0_ScaleWidthDifferential(                                                                                                                  //
//     std::string additionalCuts = "Lambda0_pT>=0.50&&Lambda0_pT<0.60&&Lambda0_pseudorapidity>=0.00&&Lambda0_pseudorapidity<0.50&&Lambda0_phi>=1.00&&Lambda0_phi<2.00" //
// )
void Lambda0_ScaleWidthDifferential(                                                                      //
    std::string additionalCuts = "Lambda0_pT>=1.10&&Lambda0_pT<1.20&&Lambda0_phi>=0.00&&Lambda0_phi<1.00" //
)
{
    const std::string figure_dir = "./figure_lambda_crosscheck/";
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
    fitconfig.branch = "Lambda0_mass";
    fitconfig.decaystring = "#Lambda^{0}#rightarrow p#pi^{-} + (c.c.)";

    // Signal model
    // fitconfig.sigmodel = "DoubleCrystalBall";
    fitconfig.sigmodel = "Gaussian";

    configure_fitparam(additionalCuts, fitconfig);

    if (!isMC)
    {
        const bool doSnapshot = true;
        const std::string snapshotName = std::format("./snapshot/filtered_tree_{}.root", plotdir_prefix);
        const std::string inputFile = "/sphenix/tg/tg01/hf/aopatton/SVLooseJun4/6RunsCombinedLambdaSVLoose.root";

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
