// -- c++ includes --
#include <iostream>
#include <iomanip>
#include <fstream>

// -- root includes --
#include <TH2F.h>
#include <TFile.h>
#include <TMath.h>
#include <TCanvas.h>

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

    struct Data {
        string  readtime;
        Int_t   sector;
        Int_t   ib;
        Float_t imeas;
    };

    vector<Data> dataset;

    void analyze(const string &i_output, const string& outputPDF);
    Int_t readData(const string &input);

    // there are 6 interface boards for north and south (total 12 along eta)
    UInt_t  bins_eta = 12;
    Float_t eta_low  = -6;
    Float_t eta_high = 6;

    // there are 32 sectors in phi
    UInt_t  bins_phi = 32;
    Float_t phi_low  = 0;
    Float_t phi_high = 2*M_PI;

    UInt_t nIBs = 384;

    Int_t rows = 0;

    // tune the number of samples to process
    Int_t process_samples = 60;

    // the maximum time separation between any two consecutive samples
    Int_t nSeconds_separation = 60;
}

Int_t myAnalysis::readData(const string &input) {
    // Create an input stream
    std::ifstream file(input);

    // Check if the file was successfully opened
    if (!file.is_open()) {
        cerr << "Failed to open cuts file: " << input << endl;
        return 1;
    }

    string  line;
    Float_t temp;

    // skip header
    std::getline(file, line);

    while (std::getline(file, line)) {
        char comma;
        Data data;

        // Create a string stream from the line
        stringstream ss(line);
        getline(ss, data.readtime, ',');

        if (ss >> data.sector   >> comma
               >> data.ib       >> comma
               >> temp          >> comma
               >> temp          >> comma
               >> data.imeas) {
            dataset.push_back(data);
        }
        else {
            cerr << "Failed to parse line: " << line << endl;
            return 1;
        }
        ++rows;
    }

    // Close the file
    file.close();

    cout << "Data Preview" << endl;
    UInt_t i = 0;
    for(auto data : dataset) {
        cout << left << "readtime: " << setw(8) << data.readtime
                     << ", sector: " << setw(2) << data.sector
                     << ", ib: "     << setw(2) << data.ib
                     << ", imeas: "  << setw(2) << data.imeas << endl;
        ++i;
        if(i == 5) break;
    }

    return 0;
}

void myAnalysis::analyze(const string &i_output, const string &outputPDF) {

    auto hPhiDummy = new TH1F("hPhiDummy","",bins_phi, phi_low, 32);

    vector<TH2F*> h2Current;

    std::tm t = {};
    std::istringstream ss(dataset[0].readtime);

    stringstream time;
    ULong_t      seconds;

    if (ss >> std::get_time(&t, "%Y-%m-%d %H:%M:%S")) {
        time << std::put_time(&t, "%c");
        seconds = std::mktime(&t);
    }
    else cout << "Parse failed for row: 0" << endl;

    string name  = "h2Current_0";
    string title = "Current [#mu A]: " + time.str() + "; #phi; IB #eta Index";

    TH2F* h2 = new TH2F(name.c_str(), title.c_str(), bins_phi, phi_low, phi_high, bins_eta, eta_low, eta_high);

    h2Current.push_back(h2);

    Int_t samples = 1;
    Int_t counts = 0;

    for(UInt_t i = 0; i < rows; ++i) {
        t = {};
        ss.str("");
        ss.str(dataset[i].readtime);

        stringstream time2;
        ULong_t      seconds2;

        if (ss >> std::get_time(&t, "%Y-%m-%d %H:%M:%S")) {
            time2 << std::put_time(&t, "%c");
            seconds2 = std::mktime(&t);
        }
        else cout << "Parse failed for row: " << i << endl; ;

        Int_t   sector = dataset[i].sector;
        Int_t   ib     = dataset[i].ib;
        Float_t imeas  = dataset[i].imeas;

        if(seconds-seconds2 >= nSeconds_separation) {
            name  = "h2Current_"+to_string(samples);
            title = "Current [#mu A]: " + time2.str() + "; #phi; IB #eta Index";

            TH2F* h2 = new TH2F(name.c_str(), title.c_str(), bins_phi, phi_low, phi_high, bins_eta, eta_low, eta_high);

            h2Current.push_back(h2);

            cout << time.str() << ": " << counts << endl;

            counts  = 0;
            time.str(time2.str());
            seconds = seconds2;

            ++samples;
            if(samples == process_samples+1) break;
        }

        Int_t phi = (sector > bins_phi-1) ? sector-bins_phi : sector;
        Int_t eta = (sector > bins_phi-1) ? -ib-1 : ib;

        Int_t iphi = hPhiDummy->FindBin(phi);
        Int_t ieta = h2Current[samples-1]->GetYaxis()->FindBin(eta);

        ++counts;

        if(counts > nIBs) {
            cout << "ERROR: Counts: " << counts << endl;
            continue;
        }

        h2Current[samples-1]->SetBinContent(iphi, ieta, imeas);
    }
    cout << time.str() << ": " << counts << endl;

    TFile output(i_output.c_str(), "recreate");

    output.cd();

    TCanvas* c1 = new TCanvas();
    c1->SetTickx();
    c1->SetTicky();

    c1->SetCanvasSize(1500, 1000);
    c1->SetLeftMargin(.08);
    c1->SetRightMargin(.12);

    c1->Print((outputPDF + "[").c_str(), "pdf portrait");

    cout << "Samples: " << h2Current.size() << endl;
    for(UInt_t i = 0; i < h2Current.size(); ++i) {

        h2Current[i]->Write();

        c1->cd();
        h2Current[i]->SetStats(0);
        h2Current[i]->Draw("COLZ1");
        c1->Print((outputPDF).c_str(), "pdf portrait");
        c1->Print((string(h2Current[i]->GetName()) + ".png").c_str());

        if(i == process_samples-1) break;
    }

    c1->Print((outputPDF + "]").c_str(), "pdf portrait");

    output.Close();
}

void analyze_current(const string &input,
                     const string &output = "test.root",
                     const string &outputPDF = "test.pdf") {

    cout << "#############################" << endl;
    cout << "Run Parameters" << endl;
    cout << "inputFile: "    << input << endl;
    cout << "outputFile: "   << output << endl;
    cout << "outputPDF: "    << outputPDF << endl;
    cout << "#############################" << endl;

    Int_t ret = myAnalysis::readData(input);
    if(ret != 0) return;

    myAnalysis::analyze(output, outputPDF);
}
