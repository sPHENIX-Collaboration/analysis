#include <algorithm>
#include <cmath>
#include <cctype>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include <RooArgList.h>
#include <RooFitResult.h>
#include <RooRealVar.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TGaxis.h>
#include <TH1.h>
#include <TH1D.h>
#include <TLatex.h>
#include <TSystem.h>

struct V0Config
{
    std::string name;
    std::string input_dir;
    std::string cut_prefix;
    std::string output_prefix;
    std::string plot_label;
};

void draw1Dhistogram(TH1 *hist, bool logx, bool logy, std::string xtitle, std::string ytitle, std::vector<std::string> addinfo, std::string filename)
{
    TCanvas *c = new TCanvas("c", "c", 800, 700);
    gPad->SetTopMargin(0.07);
    c->cd();
    c->SetLogx(logx);
    c->SetLogy(logy);
    hist->GetXaxis()->SetTitle(xtitle.c_str());
    hist->GetYaxis()->SetTitle(ytitle.c_str());
    hist->GetYaxis()->SetTitleOffset(1.6);
    hist->GetYaxis()->SetRangeUser((logy) ? hist->GetMinimum(0) * 0.5 : 0, (logy) ? hist->GetMaximum() * 50 : hist->GetMaximum() * 1.4);
    hist->Draw("e1");

    TLatex *latex = new TLatex();
    latex->SetTextSize(0.04);
    latex->SetTextAlign(12);
    latex->SetNDC();
    for (size_t i = 0; i < addinfo.size(); ++i)
    {
        latex->DrawLatex(0.2, 0.88 - i * 0.05, addinfo[i].c_str());
    }
    c->SaveAs(Form("%s.png", filename.c_str()));
    c->SaveAs(Form("%s.pdf", filename.c_str()));
    delete latex;
    delete c;
}

void replace_all(std::string &s, const std::string &from, const std::string &to)
{
    size_t pos = 0;
    while ((pos = s.find(from, pos)) != std::string::npos)
    {
        s.replace(pos, from.size(), to);
        pos += to.size();
    }
}

std::string cut_to_dir_tag(std::string cut)
{
    replace_all(cut, ">=", "geq");
    replace_all(cut, "<=", "leq");
    replace_all(cut, "==", "eq");
    replace_all(cut, "&&", "AND");
    replace_all(cut, "||", "OR");
    replace_all(cut, ">", "gt");
    replace_all(cut, "<", "lt");

    std::replace(cut.begin(), cut.end(), '.', 'p');
    std::replace(cut.begin(), cut.end(), '-', 'm');
    std::replace(cut.begin(), cut.end(), '+', 'p');
    return cut;
}

std::string pt_cut(const std::string &cut_prefix, double pt_min, double pt_max)
{
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2)
        << cut_prefix << "_pT>=" << pt_min << "&&" << cut_prefix << "_pT<" << pt_max;
    return oss.str();
}

bool read_nsig(const std::string &filename, double &yield, double &error)
{
    std::unique_ptr<TFile> file(TFile::Open(filename.c_str(), "READ"));
    if (!file || file->IsZombie())
    {
        std::cerr << "WARNING: cannot open " << filename << std::endl;
        return false;
    }

    RooFitResult *fitres = dynamic_cast<RooFitResult *>(file->Get("fitres"));
    if (!fitres)
    {
        std::cerr << "WARNING: fitres not found in " << filename << std::endl;
        return false;
    }

    const RooRealVar *nSig = dynamic_cast<const RooRealVar *>(fitres->floatParsFinal().find("nSig"));
    if (!nSig)
        nSig = dynamic_cast<const RooRealVar *>(fitres->constPars().find("nSig"));
    if (!nSig)
    {
        std::cerr << "WARNING: nSig not found in " << filename << std::endl;
        return false;
    }

    yield = nSig->getVal();
    error = nSig->getError();
    return true;
}

bool make_v0_config(std::string v0, V0Config &config)
{
    std::transform(v0.begin(), v0.end(), v0.begin(), [](unsigned char c) { return std::tolower(c); });

    if (v0 == "lambda" || v0 == "lambda0")
    {
        config = {"lambda", "./figure_lambda_crosscheck", "Lambda0", "lambda", "#Lambda^{0}"};
        return true;
    }
    if (v0 == "kshort" || v0 == "ks" || v0 == "ks0" || v0 == "k_s0")
    {
        config = {"kshort", "./figure_kshort_crosscheck", "K_S0", "kshort", "K_{S}^{0}"};
        return true;
    }

    std::cerr << "ERROR: unsupported V0 species \"" << v0 << "\". Use \"lambda\", \"kshort\", or \"both\"." << std::endl;
    return false;
}

void get_one_v0_raw_yield(const V0Config &config)
{
    const std::vector<double> pt_bins = {0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.8, 2.1, 2.4, 2.7, 3.0, 4.0};

    std::vector<std::string> subdirs;
    void *dir = gSystem->OpenDirectory(config.input_dir.c_str());
    if (!dir)
    {
        std::cerr << "ERROR: cannot open " << config.input_dir << std::endl;
        return;
    }

    const char *entry = nullptr;
    while ((entry = gSystem->GetDirEntry(dir)))
    {
        const std::string dirname = entry;
        if (dirname == "." || dirname == "..")
            continue;

        const std::string rootfile = config.input_dir + "/" + dirname + "/fitresults.root";
        if (!gSystem->AccessPathName(rootfile.c_str()))
            subdirs.push_back(dirname);
    }
    gSystem->FreeDirectory(dir);
    std::sort(subdirs.begin(), subdirs.end());

    TGaxis::SetMaxDigits(3);
    TH1D *hYield = new TH1D(Form("h%sRawYieldPt", config.output_prefix.c_str()), Form(";%s p_{T} [GeV];Raw %s yield", config.plot_label.c_str(), config.plot_label.c_str()), pt_bins.size() - 1, pt_bins.data());
    hYield->Sumw2();
    std::vector<int> fit_counts(pt_bins.size() - 1, 0);

    std::cout << "\nRaw-yield summary for " << config.name << std::endl;
    for (size_t i = 0; i < pt_bins.size() - 1; ++i)
    {
        const double pt_min = pt_bins[i];
        const double pt_max = pt_bins[i + 1];
        const std::string pt_tag = cut_to_dir_tag(pt_cut(config.cut_prefix, pt_min, pt_max));

        std::vector<std::string> matching_phi_dirs;
        std::vector<std::string> matching_pt_dirs;
        for (const std::string &dirname : subdirs)
        {
            if (dirname.find(pt_tag) != 0)
                continue;

            if (dirname.find(config.cut_prefix + "_phi") != std::string::npos)
                matching_phi_dirs.push_back(dirname);
            else
                matching_pt_dirs.push_back(dirname);
        }

        const std::vector<std::string> &dirs_to_sum = matching_phi_dirs.empty() ? matching_pt_dirs : matching_phi_dirs;
        double yield_sum = 0.0;
        double error2_sum = 0.0;

        for (const std::string &dirname : dirs_to_sum)
        {
            double yield = 0.0;
            double error = 0.0;
            const std::string rootfile = config.input_dir + "/" + dirname + "/fitresults.root";
            if (!read_nsig(rootfile, yield, error))
                continue;

            yield_sum += yield;
            error2_sum += error * error;
        }

        hYield->SetBinContent(i + 1, yield_sum);
        hYield->SetBinError(i + 1, std::sqrt(error2_sum));
        fit_counts[i] = dirs_to_sum.size();

        std::cout << std::fixed << std::setprecision(2)
                  << pt_min << " <= pT < " << pt_max << ": "
                  << yield_sum << " +/- " << std::sqrt(error2_sum)
                  << " from " << dirs_to_sum.size() << " fit result file(s)" << std::endl;
    }

    std::cout << "\n| V0 | pT bin [GeV] | Raw yield | Fit uncertainty |\n"
              << "|---|---:|---:|---:|\n";
    for (size_t i = 0; i < pt_bins.size() - 1; ++i)
    {
        std::cout << std::fixed << std::setprecision(2)
                  << "| " << config.name << " | [" << pt_bins[i] << ", " << pt_bins[i + 1] << ") | "
                  << hYield->GetBinContent(i + 1) << " | "
                  << hYield->GetBinError(i + 1) << " |\n ";
    }
    std::cout << std::endl;

    const std::string output_name = config.output_prefix + "_yield_pt";
    TFile outfile((config.input_dir + "/" + output_name + ".root").c_str(), "RECREATE");
    hYield->Write();
    outfile.Close();

    draw1Dhistogram(hYield, false, false, config.plot_label + " p_{T} [GeV]", "Raw " + config.plot_label + " yield", {}, config.input_dir + "/" + output_name);
}

void getV0RawYield(std::string v0 = "lambda")
{
    std::transform(v0.begin(), v0.end(), v0.begin(), [](unsigned char c) { return std::tolower(c); });
    if (v0 == "both" || v0 == "all")
    {
        V0Config lambda_config;
        V0Config kshort_config;
        if (make_v0_config("lambda", lambda_config))
            get_one_v0_raw_yield(lambda_config);
        if (make_v0_config("kshort", kshort_config))
            get_one_v0_raw_yield(kshort_config);
        return;
    }

    V0Config config;
    if (!make_v0_config(v0, config))
        return;

    get_one_v0_raw_yield(config);
}
