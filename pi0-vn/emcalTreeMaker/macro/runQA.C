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
using std::ofstream;

namespace myAnalysis {
    vector<pair<string,TFile*>> inputs; // run, file

    Int_t init(const string &i_input);
    Int_t readFiles(const string &i_input);

    void process_event(const string &outputFile);
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

void myAnalysis::process_event(const string &outputFile) {

    UInt_t ctr[4] = {0};

    cout << "Run,|z| < 10,|z| < 10 and MB,|z| < 10 and MB and totalCaloE > 0,Bad PMTs" << endl;

    ofstream output(outputFile);
    output << "Run,|z| < 10,|z| < 10 and MB,|z| < 10 and MB and totalCaloE > 0,Bad PMTs" << endl;
    stringstream s;

    for(auto input : inputs) {
        auto h = (TH1F*)(input.second->Get("hVtxZ"));
        Int_t low = h->FindBin(-10);
        Int_t high = h->FindBin(10)-1;

        Int_t events = h->Integral(low, high);

        h = (TH1F*)(input.second->Get("hVtxZv2"));
        Int_t events_mb = h->Integral(low, high);

        auto h2 = (TH2F*)(input.second->Get("h2TotalMBDCaloE"));
        Int_t events_mb_caloE = h2->GetEntries();

        h = (TH1F*)(input.second->Get("hBadPMTs"));

        Int_t totalBadPMTs = 0;
        for(UInt_t i = 1; i <= h->GetNbinsX(); ++i) {
            totalBadPMTs += h->GetBinLowEdge(i)*h->GetBinContent(i);
        }

        s.str("");
        s << input.first << "," << events << "," << events_mb << "," << events_mb_caloE << "," << totalBadPMTs << endl;
        output << s.str();
        cout << s.str();

        ctr[0] += events;
        ctr[1] += events_mb;
        ctr[2] += events_mb_caloE;
        ctr[3] += totalBadPMTs;
    }

    output.close();

    cout << "Total" << endl;
    cout << "z| < 10: " << ctr[0]
         << ", |z| < 10 and MB: " << ctr[1]
         << ", |z| < 10 and MB and totalCaloE > 0: " << ctr[2]
         << ", Total Bad PMTs: " << ctr[3]
         << endl;
}

void myAnalysis::finalize() {}

void runQA(const string &i_input,const string &output = "qa.csv") {

    cout << "#############################" << endl;
    cout << "Run Parameters" << endl;
    cout << "inputFile: "  << i_input << endl;
    cout << "outputFile: " << output << endl;
    cout << "#############################" << endl;

    Int_t ret = myAnalysis::init(i_input);
    if(ret != 0) return;

    myAnalysis::process_event(output);
    myAnalysis::finalize();
}

# ifndef __CINT__
Int_t main(Int_t argc, char* argv[]) {
if(argc < 2 || argc > 3){
        cout << "usage: ./runQA inputFile [outputFile]" << endl;
        cout << "inputFile: containing list of root file paths" << endl;
        cout << "outputFile: run stats. Default: qa.csv" << endl;
        return 1;
    }

    string outputFile = "qa.csv";

    if(argc >= 3) {
       outputFile = argv[2];
    }

    runQA(argv[1], outputFile);

    cout << "======================================" << endl;
    cout << "done" << endl;
    return 0;
}
# endif
