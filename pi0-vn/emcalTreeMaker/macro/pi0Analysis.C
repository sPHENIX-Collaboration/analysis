// -- c++ includes --
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

// -- root includes --
#include <TH2F.h>
#include <TFile.h>
#include <TChain.h>

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::to_string;
using std::vector;

namespace myAnalysis {
    TChain* T;

    struct Cut {
        float pi0_pt_min;
        float pi0_pt_max;
        float e1;
        float e2;
        float e_asym;
        float deltaR;
        float chi1;
        float chi2;
    };

    vector<Cut> cuts;

    int init(const string &i_input, const string &i_cuts);
    int readFiles(const string &i_input);
    int readCuts(const string &i_cuts);
    void init_hists();

    void process_event(UInt_t events = 0);
    void finalize(const string &i_output = "test.root");
}

int myAnalysis::init(const string &i_input, const string &i_cuts) {
    int ret = readFiles(i_input);
    if(ret != 0) return ret;

    ret = readCuts(i_cuts);
    if(ret != 0) return ret;

    init_hists();

    return 0;
}

int myAnalysis::readFiles(const string &i_input) {
    T = new TChain("T");

    // Create an input stream
    std::ifstream file(i_input);

    // Check if the file was successfully opened
    if (!file.is_open()) {
        cerr << "Failed to open the file." << endl;
        return 1;
    }

    cout << "Reading in TNtuples" << endl;
    cout << "======================================" << endl;
    string line;
    int entries = 0;
    while (std::getline(file, line)) {
        // Process each line here
        T->Add(line.c_str());

        cout << line << ", entries: " << T->GetEntries()-entries << endl;
        entries = T->GetEntries();
    }
    cout << "======================================" << endl;
    cout << "Total Entries: " << T->GetEntries() << endl;
    cout << "======================================" << endl;

    // Close the file
    file.close();

    return 0;
}

int myAnalysis::readCuts(const string &i_cuts) {
    // Create an input stream
    std::ifstream file(i_cuts);

    // Check if the file was successfully opened
    if (!file.is_open()) {
        cerr << "Failed to open the file." << endl;
        return 1;
    }

    string line;
    while (std::getline(file, line)) {
        std::istringstream lineStream(line);
        string cell;
        char comma;

        Cut cut;

        if (lineStream >> cut.pi0_pt_min >> comma
                       >> cut.pi0_pt_max >> comma
                       >> cut.e1 >> comma
                       >> cut.e2 >> comma
                       >> cut.e_asym >> comma
                       >> cut.deltaR >> comma
                       >> cut.chi1 >> comma
                       >> cut.chi2) cuts.push_back(cut);
        else {
            cerr << "Failed to parse line: " << line << endl;
            return 1;
        }
    }

    // Close the file
    file.close();

    cout << "Cuts" << endl;
    // Now, you can work with the data in the 'data' vector
    for(auto cut : cuts) {
        cout << "pi0_pt_min: " << cut.pi0_pt_min
           << ", pi0_pt_max: " << cut.pi0_pt_max
           << ", e1: "         << cut.e1
           << ", e2: "         << cut.e2
           << ", e_asym: "     << cut.e_asym
           << ", deltaR: "     << cut.deltaR
           << ", chi1: "       << cut.chi1
           << ", chi2: "       << cut.chi2 << endl;
    }

    return 0;
}


void myAnalysis::init_hists() {

}

void myAnalysis::process_event(UInt_t events) {

}


void myAnalysis::finalize(const string &i_output) {
}

void pi0Analysis(const string &i_input,
                  const string &i_cuts,
                  UInt_t        events   = 0,
                  const string &i_output = "test.root") {

    cout << "#############################" << endl;
    cout << "Run Parameters" << endl;
    cout << "inputFile: "  << i_input << endl;
    cout << "Cuts: "       << i_cuts << endl;
    cout << "events: "     << events << endl;
    cout << "outputFile: " << i_output << endl;
    cout << "#############################" << endl;

    int ret = myAnalysis::init(i_input, i_cuts);
    if(ret != 0) return;

    myAnalysis::process_event(events);
    myAnalysis::finalize(i_output);
}

# ifndef __CINT__
int main(int argc, char* argv[]) {
if(argc < 3 || argc > 5){
        cout << "usage: ./pi0_analysis inputFile cuts [events] [outputFile]" << endl;
        cout << "inputFile: containing list of root file paths" << endl;
        cout << "cuts: csv file containing cuts" << endl;
        cout << "events: Number of events to analyze. Default: 0 (to run over all entries)." << endl;
        cout << "outputFile: location of output file. Default: test.root." << endl;
        return 1;
    }

    UInt_t events     = 0;
    string outputFile = "test.root";

    if(argc >= 4) {
        events = atoi(argv[3]);
    }
    if(argc >= 5) {
        outputFile = argv[4];
    }

    pi0Analysis(argv[1], argv[2], events, outputFile);

    cout << "===============================" << endl;
    cout << "done" << endl;
    return 0;

}
# endif
