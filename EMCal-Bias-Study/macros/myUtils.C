#include <string>

// root includes --
#include <TH1.h>

using std::string;
using std::pair;
using std::make_pair;
using std::vector;
using std::stringstream;

namespace myUtils {
    void setEMCalDim(TH1* hist);

    pair<Int_t, Int_t> getSectorIB(Int_t iphi, Int_t ieta);
    pair<Int_t, Int_t> getSectorIB(Int_t towerIndex);
    vector<string> split(const string &s, const char delimiter);

    Int_t m_nsector = 64;
    Int_t m_nchannel_per_sector = 384;
    Int_t m_nchannel_per_ib = 64;
    Int_t m_nphi = 256;
    Int_t m_neta = 96;
    Int_t m_nib = 384;
    Int_t m_nib_per_sector = 6;
    Int_t m_ntowIBSide = 8;
}

vector<string> myUtils::split(const string &s, const char delimiter) {
    vector<string> result;

    stringstream ss(s);
    string temp;

    while(getline(ss,temp,delimiter)) {
        if(!temp.empty()) {
            result.push_back(temp);
        }
    }

    return result;
}

void myUtils::setEMCalDim(TH1* hist) {
    hist->GetXaxis()->SetLimits(0,256);
    hist->GetXaxis()->SetNdivisions(32, false);
    hist->GetXaxis()->SetLabelSize(0.04);
    hist->GetXaxis()->SetTickSize(0.01);
    hist->GetYaxis()->SetTickSize(0.01);
    hist->GetYaxis()->SetLabelSize(0.04);
    hist->GetYaxis()->SetLimits(0,96);
    hist->GetYaxis()->SetNdivisions(12, false);
    hist->GetYaxis()->SetTitleOffset(0.5);
    hist->GetXaxis()->SetTitleOffset(1);
}

pair<Int_t, Int_t> myUtils::getSectorIB(Int_t iphi, Int_t ieta) {
    Int_t k = iphi / m_ntowIBSide;

    Int_t sector = (ieta < 48) ? k + 32 : k;
    Int_t ib = (ieta < 48) ? m_nib_per_sector - ieta / m_ntowIBSide - 1: (ieta - 48) / m_ntowIBSide;

    return make_pair(sector, ib);
}

pair<Int_t, Int_t> myUtils::getSectorIB(Int_t towerIndex) {
    Int_t k = towerIndex / m_nchannel_per_sector;

    Int_t sector = (k % 2) ? (k - 1) / 2 : k / 2 + 32;
    Int_t ib = (k % m_nchannel_per_sector) / m_nchannel_per_ib;

    return make_pair(sector, ib);
}
