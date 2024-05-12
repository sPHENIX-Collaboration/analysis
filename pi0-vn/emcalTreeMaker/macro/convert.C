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
        Float_t e1;         // min cluster energy
        Float_t e2;         // min cluster 2 energy
        Float_t asym;       // max cluster pair energy asymmetry: |E1-E2|/(E1+E2), where E1 and E2 is cluster ecore
        Float_t deltaR_min; // min cluster pair deltaR
        Float_t deltaR_max; // max cluster pair deltaR
        Float_t chi;        // max cluster chi2
    };

    vector<Cut> cuts;

    Int_t readCuts(const string &i_cuts);
    string parseFloat(Float_t value);

    vector<string> cent_key;
    vector<string> cent_key1 = {"40-60","20-40","0-20"};
    vector<string> cent_key2 = {"50-60", "40-50", "30-40","20-30","10-20","0-10"};

    // Impact parameter bin edges taken from: https://wiki.sphenix.bnl.gov/index.php/MDC2_2022
    vector<string>  b_key1 = {"9.71-11.84", "6.81-9.71", "0-6.81"}; /*fm*/
    vector<string>  b_key2 = {"10.81-11.84","9.71-10.81","8.40-9.71","6.81-8.40","4.88-6.81","0-4.88"}; /*fm*/

    vector<string> pt_key;
    vector<string> pt_key1 = {"2-2.5","2.5-3","3-3.5","3.5-4","4-4.5","4.5-5"};
    vector<string> pt_key2 = {"2-5"};
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
    // skip header
    std::getline(file, line);
    while (std::getline(file, line)) {
        std::istringstream lineStream(line);
        string cell;
        char comma;

        Cut cut;

        if (lineStream >> cut.e1         >> comma
                       >> cut.e2         >> comma
                       >> cut.asym       >> comma
                       >> cut.deltaR_min >> comma
                       >> cut.deltaR_max >> comma
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
             const string &i_outputDir = ".",
                   Bool_t  isSim       = false,
                   Int_t   anaType     = 0) {

    cout << "#############################" << endl;
    cout << "Run Parameters" << endl;
    cout << "inputFile: "        << i_input << endl;
    cout << "Cuts: "             << i_cuts << endl;
    cout << "output directory: " << i_outputDir << endl;
    cout << "isSim: "            << isSim << endl;
    cout << "anaType: "          << anaType << endl;
    cout << "#############################" << endl;

    Int_t ret = myAnalysis::readCuts(i_cuts);
    if(ret != 0) return;

    TFile input(i_input.c_str());

    if(isSim) {
        myAnalysis::cent_key = (anaType == 0) ? myAnalysis::b_key1 : myAnalysis::b_key2;
    }
    else {
        myAnalysis::cent_key = (anaType == 0) ? myAnalysis::cent_key1 : myAnalysis::cent_key2;
    }

    myAnalysis::pt_key   = (anaType == 0) ? myAnalysis::pt_key1   : myAnalysis::pt_key2;

    stringstream s;
    TH1F* h;
    for(auto cut : myAnalysis::cuts) {
        string outfname = i_outputDir + "/hPi0Mass_EA" + myAnalysis::parseFloat(cut.e1) +
                                         "_EB"         + myAnalysis::parseFloat(cut.e2) +
                                         "_Asym"       + myAnalysis::parseFloat(cut.asym) +
                                         "_DelrMin"    + myAnalysis::parseFloat(cut.deltaR_min) +
                                         "_DelrMax"    + myAnalysis::parseFloat(cut.deltaR_max) +
                                         "_Chi"        + myAnalysis::parseFloat(cut.chi) +
                                         ".root";
        Int_t index = 0;

        TFile output(outfname.c_str(),"recreate");
        output.cd();
        for(auto cent : myAnalysis::cent_key) {
            for(auto pt : myAnalysis::pt_key) {
                s.str("");
                s << "results/" << cent << "/" << pt << "/"
                  << "hPi0Mass_" << cent << "_" << pt << "_"
                  << cut.e1 << "_" << cut.e2 << "_" << cut.asym << "_"
                  << cut.deltaR_min << "_" << cut.deltaR_max << "_" << cut.chi;

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
if(argc < 3 || argc > 6){
        cout << "usage: ./convert inputFile cuts [outputDir] [isSim] [anaType]" << endl;
        cout << "inputFile: input root file" << endl;
        cout << "cuts: csv file containing cuts" << endl;
        cout << "outputDir: location of output directory. Default: current directory." << endl;
        cout << "isSim: Simulation. Default: False" << endl;
        cout << "anaType: analysis type. Default: 0." << endl;
        return 1;
    }

    string outputDir = ".";
    Bool_t isSim     = false;
    Int_t  anaType   = 0;

    if(argc >= 4) {
        outputDir = argv[3];
    }
    if(argc >= 5) {
        isSim = atoi(argv[4]);
    }
    if(argc >= 6) {
        anaType = atoi(argv[5]);
    }

    convert(argv[1], argv[2], outputDir, isSim, anaType);

    cout << "======================================" << endl;
    cout << "done" << endl;
    return 0;
}
# endif
