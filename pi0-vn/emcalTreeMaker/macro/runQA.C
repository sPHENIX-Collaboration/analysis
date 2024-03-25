// -- c++ includes --
#include <iostream>
#include <iomanip>
#include <fstream>

// -- root includes --
#include <TH2F.h>
#include <TFile.h>
#include <TMath.h>

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::to_string;
using std::vector;
using std::map;
using std::pair;
using std::make_pair;
using std::min;
using std::max;
using std::left;
using std::setw;
using std::stringstream;
using std::sin;
using std::cos;

namespace myAnalysis {
    vector<pair<string,TFile*>> inputs; // run, file

    Int_t init(const string &i_input);
    Int_t readFiles(const string &i_input);

    void process_event();
    void finalize();

    vector<string> cent_key = {"40-60", "20-40", "0-20"};
    vector<string> pt_key   = {"2-2.5", "2.5-3", "3-3.5", "3.5-4", "4-4.5", "4.5-5"};

    TH1F* pt_dum_vec   = new TH1F("pt_dum_vec","",6,2,5);
    TH1F* cent_dum_vec = new TH1F("cent_dum_vec","", 3, 0, 0.6);
}

Int_t myAnalysis::init(const string &i_input) {
    return readFiles(i_input);
}

Int_t myAnalysis::readFiles(const string &i_input) {
    // Create an input stream
    std::ifstream file(i_input);

    // Check if the file was successfully opened
    if (!file.is_open()) {
        cerr << "Failed to open root file list: " << i_input << endl;
        return 1;
    }

    cout << "Reading in Files" << endl;
    cout << "======================================" << endl;
    string line;
    while (std::getline(file, line)) {
        // Find the position of the last '.'
        Int_t dot_pos = line.find_last_of('.');
        string run    = line.substr(line.rfind('-')+1, dot_pos-line.rfind('-')-1);
        // Process each line here
        inputs.push_back(make_pair(run, TFile::Open(line.c_str())));
    }

    // Close the file
    file.close();

    return 0;
}

void myAnalysis::process_event() {

    UInt_t ctr[2] = {0};

    cout << "Run,Events |z| < 10,Events: |z| < 10 and MB" << endl;
    for(auto input : inputs) {
        auto h = (TH1F*)(input.second->Get("hVtxZ"));
        Int_t low = h->FindBin(-10);
        Int_t high = h->FindBin(10)-1;

        Int_t events = h->Integral(low, high);

        h = (TH1F*)(input.second->Get("hVtxZv2"));
        Int_t events_mb = h->Integral(low, high);

        cout << input.first << "," << events << "," << events_mb << endl;

        ctr[0] += events;
        ctr[1] += events_mb;
    }

    cout << "Total" << endl;
    cout << "Events |z| < 10: " << ctr[0] << ", Events: |z| < 10 and MB: " << ctr[1] << endl;
}

void myAnalysis::finalize() {}

void runQA(const string &i_input) {

    cout << "#############################" << endl;
    cout << "Run Parameters" << endl;
    cout << "inputFile: "  << i_input << endl;
    cout << "#############################" << endl;

    Int_t ret = myAnalysis::init(i_input);
    if(ret != 0) return;

    myAnalysis::process_event();
    myAnalysis::finalize();
}

# ifndef __CINT__
Int_t main(Int_t argc, char* argv[]) {
if(argc < 2 || argc > 2){
        cout << "usage: ./runQA inputFile" << endl;
        cout << "inputFile: containing list of root file paths" << endl;
        return 1;
    }

    runQA(argv[1]);

    cout << "======================================" << endl;
    cout << "done" << endl;
    return 0;
}
# endif
