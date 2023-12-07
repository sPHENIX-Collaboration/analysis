// -- c++ includes --
#include <iostream>
#include <iomanip>
#include <fstream>

// -- root includes --
#include <TH1F.h>
#include <TFile.h>

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::to_string;
using std::vector;
using std::stringstream;

namespace myAnalysis {
    struct Cut {
        Float_t e;      // min cluster energy
        Float_t e_asym;   // max cluster pair energy asymmetry: |E1-E2|/(E1+E2), where E1 and E2 is cluster ecore
        Float_t deltaR; // min cluster pair deltaR
        Float_t chi;    // max cluster chi2
    };

    vector<Cut> cuts;

    Int_t readCuts(const string &i_cuts);
    string parseFloat(Float_t value);

    vector<string> centrality = {"60-100","40-60","20-40","0-20"};
    vector<string> pt         = {"2-2.5","3-4","4-5"};
}

Int_t myAnalysis::readCuts(const string &i_cuts) {
    // Create an input stream
    std::ifstream file(i_cuts);

    // Check if the file was successfully opened
    if (!file.is_open()) {
        cerr << "Failed to open cuts file: " << i_cuts << endl;
        return 1;
    }

    string line;
    while (std::getline(file, line)) {
        std::istringstream lineStream(line);
        string cell;
        char comma;

        Cut cut;

        if (lineStream >> cut.e      >> comma
                       >> cut.e_asym >> comma
                       >> cut.deltaR >> comma
                       >> cut.chi) {
            cuts.push_back(cut);
        }
        else {
            cerr << "Failed to parse line: " << line << endl;
            return 1;
        }
    }

    // Close the file
    file.close();

    return 0;
}

string myAnalysis::parseFloat(Float_t value) {
    stringstream s;
    s << value;
    size_t dotPos = s.str().find('.');
    if (dotPos != std::string::npos) {
        // Replace '.' with "point"
        s.str(s.str().substr(0, dotPos) + "point" + s.str().substr(dotPos + 1));
    }
    return s.str();
}

void convert(const string &i_input,
             const string &i_cuts,
             const string &i_outputDir = ".") {

    cout << "#############################" << endl;
    cout << "Run Parameters" << endl;
    cout << "inputFile: "  << i_input << endl;
    cout << "Cuts: "       << i_cuts << endl;
    cout << "output directory: " << i_outputDir << endl;
    cout << "#############################" << endl;

    Int_t ret = myAnalysis::readCuts(i_cuts);
    if(ret != 0) return;

    TFile input(i_input.c_str());

    stringstream s;
    TH1F* h;
    for(auto cut : myAnalysis::cuts) {
        string outfname = i_outputDir + "/hPi0Mass_E" + myAnalysis::parseFloat(cut.e) +
                       "_Asym"      + myAnalysis::parseFloat(cut.e_asym) +
                       "_Delr"      + myAnalysis::parseFloat(cut.deltaR) +
                       "_Chi"       + myAnalysis::parseFloat(cut.chi) +
                       ".root";
        Int_t index = 0;

        TFile output(outfname.c_str(),"recreate");
        output.cd();
        for(auto cent : myAnalysis::centrality) {
            for(auto pt : myAnalysis::pt) {
                s.str("");
                s << "results/" << cent << "/" << pt << "/"
                  << "hPi0Mass_" << cent << "_" << pt << "_"
                  << cut.e << "_" << cut.e_asym << "_" << cut.deltaR << "_" << cut.chi;

                h = (TH1F*)input.Get(s.str().c_str());
                if(h) {
                    h = (TH1F*)h->Clone(("hPi0Mass_"+to_string(index++)).c_str());
                    h->Write();
                }
            }
        }
        output.Close();
        cout << "Written: " << outfname << endl;
    }

    input.Close();
}

# ifndef __CINT__
Int_t main(Int_t argc, char* argv[]) {
if(argc < 3 || argc > 4){
        cout << "usage: ./convert inputFile cuts [outputDir]" << endl;
        cout << "inputFile: input root file" << endl;
        cout << "cuts: csv file containing cuts" << endl;
        cout << "outputDir: location of output directory. Default: current directory." << endl;
        return 1;
    }

    string outputDir = ".";

    if(argc >= 4) {
        outputDir = argv[3];
    }

    convert(argv[1], argv[2], outputDir);

    cout << "======================================" << endl;
    cout << "done" << endl;
    return 0;
}
# endif
