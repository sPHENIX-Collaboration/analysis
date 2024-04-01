// -- c++ includes --
#include <iostream>
#include <iomanip>
#include <fstream>

// -- root includes --
#include <TH1F.h>
#include <TF1.h>
#include <TFile.h>
#include <TGraphErrors.h>
#include <TCanvas.h>

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::to_string;
using std::vector;
using std::stringstream;

namespace myAnalysis {
    Int_t readVN(const string &input);
    Int_t readFitStats(const string &input);
    void  plot(const string &output);

    vector<string> centrality = {"40-60","20-40","0-20"};
    vector<string> pt         = {"2-2.5","2.5-3","3-3.5","3.5-4","4-4.5","4.5-5"};

    Int_t indices = 18;

    vector<Float_t> v2(indices);
    vector<Float_t> v2_err(indices);

    Float_t EA;
    Float_t EB;
}

Int_t myAnalysis::readVN(const string &input) {
    // Create an input stream
    std::ifstream file(input);

    // Check if the file was successfully opened
    if (!file.is_open()) {
        cerr << "Failed to open file: " << input << endl;
        return 1;
    }

    // 1: v2
    // 2: v2_err
    Int_t col[2] = {1,2};
    // Int_t col[2] = {3,4}; // type_3
    // Int_t col[2] = {5,6}; // type_4

    std::string line;
    Int_t idx = 0;

    // skip header
    std::getline(file, line);

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string token;

        Int_t colIdx = 0;
        while (std::getline(ss, token, ',')) { // Assuming comma-separated values
            if (colIdx == col[0]) {
                v2[idx] = stof(token);
            }
            else if (colIdx == col[1]) {
                v2_err[idx] = stof(token);
            }
            colIdx++;
        }
        ++idx;
    }

    cout << endl;
    for(Int_t i = 0; i < centrality.size(); ++i) {
        cout << "cent: " << centrality[i] << endl;

        for(Int_t j = 0; j < pt.size(); ++j) {
            Int_t idx = i*pt.size()+j;

            cout << "pt: " << pt[j]
                 << ", v2: "     << v2[idx]
                 << ", v2_err: " << v2_err[idx]
                 << endl;
        }
        cout << endl;
    }

    return 0;
}

Int_t myAnalysis::readFitStats(const string &input) {
    // Create an input stream
    std::ifstream file(input);

    // Check if the file was successfully opened
    if (!file.is_open()) {
        cerr << "Failed to open file: " << input << endl;
        return 1;
    }

    // 5: EA
    // 6: EB
    Int_t col[2] = {5,6};

    std::string line;
    Int_t idx = 0;

    // skip header
    std::getline(file, line);

    std::getline(file, line);

    std::stringstream ss(line);
    std::string token;

    Int_t colIdx = 0;
    while (std::getline(ss, token, ',')) { // Assuming comma-separated values
        if (colIdx == col[0]) {
            EA = stof(token);
        }
        else if (colIdx == col[1]) {
            EB = stof(token);
        }
        colIdx++;
    }

    cout << "EA: " << EA << ", EB: " << EB << endl;

    return 0;
}

void myAnalysis::plot(const string &output) {
    TCanvas* c1 = new TCanvas();
    c1->SetTickx();
    c1->SetTicky();

    c1->SetCanvasSize(1500, 1000);
    c1->SetLeftMargin(.12);
    c1->SetRightMargin(.05);

    stringstream s;

    for(Int_t i = 0; i < centrality.size(); ++i) {
        Float_t x[pt.size()];
        Float_t y[pt.size()];
        Float_t ey[pt.size()];

        for(Int_t j = 0; j < pt.size(); ++j) {
            Int_t idx = i*pt.size()+j;
            x[j]  = 2.25 + 0.5*j;
            y[j]  = v2[idx];
            ey[j] = v2_err[idx];
        }

        c1->cd();
        auto graph = new TGraphErrors(pt.size(), x, y, 0, ey);

        s.str("");
        s << "Centrality: " << centrality[i] << "%, E_{A} #geq "
          << EA << " GeV, E_{B} #geq " << EB
          << " GeV; p_{T} [GeV]; v_{2}";

        graph->SetTitle(s.str().c_str());
        graph->GetYaxis()->SetRangeUser(-1.5, 1);
        graph->SetMarkerColor(4);
        graph->SetMarkerStyle(21);
        graph->Draw("AP");

        auto tf = new TF1("tf","0",2,5);
        tf->SetLineColor(kBlack);
        tf->SetLineStyle(kDashed);
        tf->Draw("same");

        c1->Print((output + "-" + centrality[i] + ".png").c_str());
    }
}

void plot_vn(const string &i_input,
             const string &fitStats,
             const string &i_outputDir = ".") {

    cout << "#############################" << endl;
    cout << "Run Parameters" << endl;
    cout << "inputFile: "  << i_input << endl;
    cout << "fitStats: "  << fitStats << endl;
    cout << "output directory: " << i_outputDir << endl;
    cout << "#############################" << endl;

    // Find the position of the last dot ('.')
    size_t dot_pos = i_input.find_last_of('.');

    // Extract the name (everything before the dot)
    string output = i_input.substr(0, dot_pos);

    Int_t ret = myAnalysis::readVN(i_input);
    if(ret != 0) return;

    ret = myAnalysis::readFitStats(fitStats);
    if(ret != 0) return;

    myAnalysis::plot(output);
}

# ifndef __CINT__
Int_t main(Int_t argc, char* argv[]) {
if(argc < 3 || argc > 4){
        cout << "usage: ./plot-vn inputFile fitStats [outputDir]" << endl;
        cout << "inputFile: input vn file" << endl;
        cout << "inputFile: input fitStats file" << endl;
        cout << "outputDir: location of output directory. Default: current directory." << endl;
        return 1;
    }

    string outputDir = ".";

    if(argc >= 4) {
        outputDir = argv[3];
    }

    plot_vn(argv[1], argv[2], outputDir);

    cout << "======================================" << endl;
    cout << "done" << endl;
    return 0;
}
# endif
