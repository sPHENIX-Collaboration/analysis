// -- c++ includes --
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <unordered_map>
#include <stdexcept> // For exception handling

// -- root includes --
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>

// TowerInfo
#include <calobase/TowerInfoDefs.h>

// -- common utils
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
namespace fs = std::filesystem;

namespace myAnalysis {
    Int_t analyze(const string &input, const string &outputDir);
    void fillGaps(UInt_t groupSize);

    unordered_map<string,TH1*> m_hists;

    string cosmicHistName = "h2CosmicMPV";
    Double_t cosmicMPVMax = 750;
}

void myAnalysis::fillGaps(UInt_t groupSize) {
    // compute averge block MPV and fill in the empty spots
    for(UInt_t i = 0; i < myUtils::m_neta / groupSize; ++i) {
        for(UInt_t j = 0; j < myUtils::m_nphi / groupSize; ++j) {
            Double_t avgMPV = 0;
            UInt_t tower_ctr = 0;

            UInt_t lowEta = i*groupSize;
            UInt_t lowPhi = j*groupSize;

            for(UInt_t k = lowEta; k < lowEta+groupSize; ++k) {
                for(UInt_t l = lowPhi; l < lowPhi+groupSize; ++l) {
                    Double_t mpv = static_cast<TH2*>(m_hists[cosmicHistName])->GetBinContent(l+1,k+1);
                    if(mpv > 0 && mpv < cosmicMPVMax) {
                      avgMPV += mpv;
                      ++tower_ctr;
                    }
                }
            }

            if(!tower_ctr) {
                cout << "ERROR: Block contains no cosmic MPV: " << lowPhi << ", " << lowEta << endl;
                continue;
            }

            // compute average MPV
            avgMPV /= tower_ctr;

            for(UInt_t k = lowEta; k < lowEta+groupSize; ++k) {
                for(UInt_t l = lowPhi; l < lowPhi+groupSize; ++l) {
                    Double_t mpv = static_cast<TH2*>(m_hists[cosmicHistName])->GetBinContent(l+1,k+1);
                    if(mpv <= 0 || mpv >= cosmicMPVMax) {
                        static_cast<TH2*>(m_hists[cosmicHistName])->SetBinContent(l+1,k+1, avgMPV);
                    }
                }
            }
        }
    }
}

Int_t myAnalysis::analyze(const string &input, const string &outputDir) {
    // Read Hist from input
    TFile* tfile = TFile::Open(input.c_str());
    if (!tfile || tfile->IsZombie()) {
        cout << "Error: Could not open ROOT file: " << input << endl;
        return 1;
    }
    m_hists[cosmicHistName] = static_cast<TH2*>(tfile->Get(cosmicHistName.c_str()));

    if (!m_hists[cosmicHistName]) {
      cout << "Error: Histogram '" << cosmicHistName << "' not found in file: " << tfile->GetName() << endl;
      return 1;
    }
    tfile->Close();

    string cosmicHistNameV0 = cosmicHistName + "v0";
    m_hists[cosmicHistNameV0] = static_cast<TH2*>(m_hists[cosmicHistName]->Clone(cosmicHistNameV0.c_str()));

    fillGaps(2);

    cout << "Filled in Block Gaps" << endl;

    string cosmicHistNameV1 = cosmicHistName + "v1";
    m_hists[cosmicHistNameV1] = static_cast<TH2*>(m_hists[cosmicHistName]->Clone(cosmicHistNameV1.c_str()));

    fillGaps(8);

    cout << "Filled in Interface Board Gaps" << endl;

    string cosmicHistNameV2 = cosmicHistName + "v2";
    m_hists[cosmicHistNameV2] = static_cast<TH2*>(m_hists[cosmicHistName]->Clone(cosmicHistNameV2.c_str()));

    string output = outputDir + "/updated-Offset-info.root";
    // save plots to root file
    TFile tf(output.c_str(),"recreate");
    tf.cd();

    m_hists[cosmicHistNameV0]->Write();
    m_hists[cosmicHistNameV1]->Write();
    m_hists[cosmicHistNameV2]->Write();

    tf.Close();

    return 0;
}

void write_Biasv3(const string &input,
                  const string &outputDir=".") {

    cout << "#############################" << endl;
    cout << "Run Parameters" << endl;
    cout << "input: "  << input << endl;
    cout << "outputDir: " << outputDir << endl;
    cout << "#############################" << endl;

    fs::create_directories(outputDir);
    TH1::AddDirectory(kFALSE);

    myAnalysis::analyze(input, outputDir);
}

# ifndef __CINT__
Int_t main(Int_t argc, char* argv[]) {
if(argc < 2 || argc > 3){
        cout << "usage: ./write-Biasv3 input [outputDir]" << endl;
        cout << "input: input root file" << endl;
        cout << "outputDir: output directory" << endl;
        return 1;
    }

    string outputDir = ".";

    if(argc >= 3) {
        outputDir = argv[2];
    }

    write_Biasv3(argv[1], outputDir);

    cout << "======================================" << endl;
    cout << "done" << endl;
    return 0;
}
# endif
