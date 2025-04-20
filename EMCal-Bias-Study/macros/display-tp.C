// -- c++ includes --
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <unordered_map>
#include <map>
#include <stdexcept> // For exception handling

// -- root includes --
#include <TH2F.h>
#include <TF1.h>
#include <TFile.h>
#include <TProfile.h>
#include <TLine.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TCanvas.h>
#include <TColor.h>
#include <TPaletteAxis.h>

// -- sPHENIX Style
#include "sPhenixStyle.C"

// common utils
#include "myUtils.C"

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::to_string;
using std::vector;
using std::stringstream;
using std::min;
using std::max;
using std::ofstream;
using std::pair;
using std::make_pair;
using std::unordered_map;
using std::map;
namespace fs = std::filesystem;

namespace myAnalysis {
    void analyze(const string &output);
    Int_t processDir(const string &input);
    Int_t readADC(const string &input);
    unordered_map<string,TH1*> m_hists;
    unordered_map<string,vector<pair<Int_t,Int_t>>> m_data; // key: sector_ib, data: (tpwidth,adc)

    Int_t m_bins_tpwidth = 51;
    Int_t m_tpwidth_low = 10;
    Int_t m_tpwidth_high = 61;

    Int_t m_bins_adc = 160;
    Int_t m_adc_low = -2000;
    Int_t m_adc_high = 14000;

    Int_t m_verbosity = 0;
    Bool_t m_saveFig = true;
}

Int_t myAnalysis::processDir(const string &input) {
    try
    {
        // Make sure the path exists and is a directory
        if (!fs::exists(input) || !fs::is_directory(input))
        {
            cout << "Error: “" << input << "” is not a valid directory" << endl;
            return 1;
        }

        // Iterate over each entry in the directory
        for (auto const& entry : fs::directory_iterator{input})
        {
            // Only process regular files (skip subdirectories, symlinks, etc.)
            if (entry.is_regular_file())
            {
                 if(m_verbosity) cout << "File: " << entry.path().filename() << " (full path: " << entry.path() << ")" << endl;
                 readADC(entry.path());
            }
        }
    }
    catch (fs::filesystem_error const& ex)
    {
        cout << "Filesystem error: " << ex.what() << endl;
        return 2;
    }

    if(m_verbosity) {
        cout << endl;
        cout << "Loaded vector size" << endl;
        for (const auto& [key, value] : m_data) {
            cout << "key: " << key << ", size: " << value.size() << endl;
        }
        cout << endl;
    }

    return 0;
}


Int_t myAnalysis::readADC(const string& input) {
    std::ifstream file(input);
    string basename = fs::absolute(input).filename().stem().string();

    if(m_verbosity > 1) {
        cout << "basename: " << basename << endl;
    }

    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + input);
    }

    string line;
    // Skip the header line if it exists (optional)
    if (std::getline(file, line)) {
        // You might want to process the header here if needed.
        // For example, you could check if the header is as expected.
    }

    while (std::getline(file, line)) {
        stringstream ss(line);
        string cell;
        Int_t tpwidth;
        Int_t adc;

        // Extract the data from each cell, handling potential errors
        try {
            // tpwidth
            if (std::getline(ss, cell, ',')) {
                 try {
                    tpwidth = std::stoi(cell);
                } catch (const std::invalid_argument& e) {
                    throw std::runtime_error("Invalid integer in offset: " + cell);
                } catch (const std::out_of_range& e) {
                    throw std::runtime_error("Integer out of range in offset: " + cell);
                }
            } else {
                 throw std::runtime_error("Error parsing offset");
            }

            // adc
            if (std::getline(ss, cell, ',')) {
                 try {
                    adc = std::stoi(cell);
                } catch (const std::invalid_argument& e) {
                    throw std::runtime_error("Invalid integer in offset: " + cell);
                } catch (const std::out_of_range& e) {
                    throw std::runtime_error("Integer out of range in offset: " + cell);
                }
            } else {
                 throw std::runtime_error("Error parsing offset");
            }

            if(m_verbosity > 2) cout << "tpwidth: " << tpwidth << ", adc: " << adc << endl;
            m_data[basename].push_back(make_pair(tpwidth, adc));

        } catch (const std::runtime_error& e) {
            cout << "Error processing line: " << line << ".  Error: " << e.what() << std::endl;
            return 1;
            // You could choose to continue processing or exit here.  For now, continuing.
        }
    }

    file.close();
    return 0;
}

void myAnalysis::analyze(const string &output) {
    cout << "###############" << endl;
    cout << "Analysis" << endl;
    string outputDir = fs::absolute(output).parent_path().string();
    fs::create_directories(outputDir);
    fs::create_directories(outputDir+"/images");

    TCanvas* c1 = new TCanvas("c1");
    c1->SetTickx();
    c1->SetTicky();

    c1->SetCanvasSize(1400, 1000);
    c1->SetLeftMargin(.15);
    c1->SetRightMargin(.12);
    c1->SetTopMargin(.1);
    c1->SetBottomMargin(.12);

    gStyle->SetOptTitle(1);
    gStyle->SetTitleStyle(0);
    gStyle->SetTitleFontSize(0.08);
    gStyle->SetTitleW(1);
    gStyle->SetTitleH(0.08);
    gStyle->SetTitleFillColor(0);
    gStyle->SetTitleBorderSize(0);

    c1->Print((output + "[").c_str(), "pdf portrait");

    stringstream name;
    stringstream title;
    for (const auto& [key, m_vec] : m_data) {
        name.str("");
        title.str("");

        vector<string> tokens = myUtils::split(key,'_');
        string sector = tokens[2];
        string ib     = tokens[3];

        name << "h2_" << sector << "_" << ib;
        title << "Sector: " << sector << ", IB: " << ib << "; LED Pulse Width [ns]; Average ADC";

        m_hists[name.str()] = new TH2F(name.str().c_str(), title.str().c_str(),
                                       m_bins_tpwidth, m_tpwidth_low, m_tpwidth_high,
                                       m_bins_adc, m_adc_low, m_adc_high);

        for(const auto& x : m_vec) {
            Int_t tpwidth = x.first;
            Int_t adc     = x.second;

            static_cast<TH2*>(m_hists[name.str()])->Fill(tpwidth, adc);
        }

        m_hists[name.str()]->SetMaximum(m_hists[name.str()]->GetMaximum()+5);
        m_hists[name.str()]->GetXaxis()->SetTitleOffset(1);
        m_hists[name.str()]->Draw("COLZ1");

        TH1* px = static_cast<TH2*>(m_hists[name.str()])->ProfileX();
        px->SetLineColor(kRed);
        px->SetMarkerColor(kRed);
        px->Draw("same");

        Int_t m_ADC_target_low  = 2500;
        Int_t m_ADC_target_high = 3500;

        TLine* l1 = new TLine(10, m_ADC_target_low, 61, m_ADC_target_low);
        TLine* l2 = new TLine(10, m_ADC_target_high, 61, m_ADC_target_high);

        l1->SetLineStyle(kDashed);
        l2->SetLineStyle(kDashed);

        l1->Draw("same");
        l2->Draw("same");

        c1->Print(output.c_str(), "pdf portrait");
        c1->Print((outputDir + "/images/" + name.str() + ".png").c_str());
    }

    c1->Print((output + "]").c_str(), "pdf portrait");
}

void display_tp(const string &input,
             const string &output="plots.pdf",
             Int_t verbosity = 0) {
    cout << "#############################" << endl;
    cout << "Run Parameters" << endl;
    cout << "input: "  << input << endl;
    cout << "output: " << output << endl;
    cout << "verbosity: " << verbosity << endl;
    cout << "#############################" << endl;

    // set sPHENIX plotting style
    SetsPhenixStyle();

    // sets the verbosity level
    myAnalysis::m_verbosity = verbosity;

    if(myAnalysis::processDir(input)) return;
    myAnalysis::analyze(output);
}

# ifndef __CINT__
Int_t main(Int_t argc, char* argv[]) {
if(argc < 2 || argc > 4){
        cout << "usage: ./display input [output] [verbosity]" << endl;
        cout << "input: input directory" << endl;
        cout << "output: output pdf file" << endl;
        cout << "verbosity: set the verbosity level" << endl;
        return 1;
    }

    string output = "plots.pdf";
    Int_t verbosity = 0;

    if(argc >= 3) {
        output = argv[2];
    }
    if(argc >= 4) {
        verbosity = atoi(argv[3]);
    }

    display_tp(argv[1], output, verbosity);

    cout << "======================================" << endl;
    cout << "done" << endl;
    return 0;
}
# endif
