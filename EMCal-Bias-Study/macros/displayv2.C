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
#include <TDirectory.h>
#include <TList.h>
#include <TProfile.h>
#include <TLine.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TCanvas.h>
#include <TColor.h>
#include <TPaletteAxis.h>

// -- sPHENIX Style
#include "sPhenixStyle.C"

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
    Int_t readFile(const string &input);
    Int_t addHist(TFile* file, const string &histName, unordered_map<string,TH1*> &histograms, const string &tag = "");
    void setEMCalDim(TH1* hist);

    pair<Int_t, Int_t> getSectorIB(Int_t iphi, Int_t ieta);
    pair<Int_t, Int_t> getSectorIB(Int_t towerIndex);
    pair<Int_t, Int_t> getSectorIBCorner(Int_t sector, Int_t ib);

    unordered_map<string,TH1*> m_hists;
    vector<string> m_run;

    Int_t m_nsector = 64;
    Int_t m_nchannel_per_sector = 384;
    Int_t m_nchannel_per_ib = 64;
    Int_t m_nphi = 256;
    Int_t m_neta = 96;
    Int_t m_nib = 384;
    Int_t m_nib_per_sector = 6;

    Int_t m_ntowIBSide = 8;

    Int_t m_saturation = 16383;

    Int_t m_bins_ADC = 1640;
    Double_t m_ADC_low = 0;
    Double_t m_ADC_high = 16400;

    Int_t m_verbosity = 0;
    Bool_t m_saveFig = false;
}

pair<Int_t, Int_t> myAnalysis::getSectorIBCorner(Int_t sector, Int_t ib) {
    Int_t phi = sector % 32 * m_ntowIBSide;
    Int_t eta = (sector > 31) ? (5 - ib) * m_ntowIBSide : ib * m_ntowIBSide + 48;

    return make_pair(phi, eta);
}

pair<Int_t, Int_t> myAnalysis::getSectorIB(Int_t iphi, Int_t ieta) {
    Int_t k = iphi / m_ntowIBSide;

    Int_t sector = (ieta < 48) ? k + 32 : k;
    Int_t ib = (ieta < 48) ? m_nib_per_sector - ieta / m_ntowIBSide - 1: (ieta - 48) / m_ntowIBSide;

    return make_pair(sector, ib);
}

pair<Int_t, Int_t> myAnalysis::getSectorIB(Int_t towerIndex) {
    Int_t k = towerIndex / m_nchannel_per_sector;

    Int_t sector = (k % 2) ? (k - 1) / 2 : k / 2 + 32;
    Int_t ib = (k % m_nchannel_per_sector) / m_nchannel_per_ib;

    return make_pair(sector, ib);
}

void myAnalysis::setEMCalDim(TH1* hist) {
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

Int_t myAnalysis::addHist(TFile* file, const string &histName, unordered_map<string,TH1*> &histograms, const string &tag) {
    TH1 *hist = (TH1*) file->Get(histName.c_str());
    if (!hist) {
      cout << "Error: Histogram '" << histName << "' not found in file: " << file->GetName() << endl;
      return 1;
    }

    string histNewName = histName+tag;

    // Clone the histogram to avoid ownership issues when the file is closed
    TH1 *clonedHist = (TH1 *) hist->Clone(histNewName.c_str());

    histograms[histNewName] = clonedHist;
    if(m_verbosity) {
        cout << "Adding: " << clonedHist->GetName() << " to map." << endl;
    }
    return 0;
}

Int_t myAnalysis::readFile(const string &input) {
    cout << "###############" << endl;
    cout << "Read File: " << input << endl;
    std::ifstream file(input);
    string line;

    TH1::AddDirectory(kFALSE);

    if (file.is_open()) {
        while (std::getline(file, line)) {
            // Process each line here
            string fileName = fs::path(line).filename().string();
            // Assumes "name-RUN.root" format
            string run = fileName.substr(fileName.find("-")+1,fileName.find(".")-fileName.find("-")-1);

            TFile* tfile = TFile::Open(line.c_str());
            if (!tfile || tfile->IsZombie()) {
              cout << "Error: Could not open ROOT file: " << fileName << endl;
              return 1;
            }

            m_run.push_back(run);
            cout << "File: " << fileName << ", Run: " << run << endl;
            addHist(tfile, "h2CEMC", m_hists, "_"+run);
            addHist(tfile, "hEvent", m_hists, "_"+run);

            TList* keysWaveform = ((TDirectory*)tfile->Get("waveform"))->GetListOfKeys();
            for(UInt_t i = 0; i < keysWaveform->GetSize(); ++i) {
                string name  = "waveform/" + string(keysWaveform->At(i)->GetName());
                addHist(tfile, name, m_hists, "_"+run);
            }

            tfile->Close();
        }
        file.close();
    } else {
        cout << "Unable to open file" << endl;
        return 1;
    }
    cout << "###############" << endl;
    return 0;
}

void myAnalysis::analyze(const string &output) {
    cout << "###############" << endl;
    cout << "Analysis" << endl;
    string outputDir = fs::absolute(output).parent_path().string();
    string outputStem = fs::absolute(output).filename().stem().string();
    fs::create_directories(outputDir);
    fs::create_directories(outputDir+"/images");

    // dummy hists for labeling
    m_hists["h2DummySector"] = new TH2F("h2DummySector","", m_nsector/2, 0, m_nsector/2, 2, 0, 2);
    m_hists["h2DummyIB"] = new TH2F("h2DummyIB","", m_nsector/2, 0, m_nsector/2, m_nib*2/m_nsector, 0, m_nib*2/m_nsector);

    // Individual IB channel response
    stringstream name;
    stringstream title;
    // for (const auto& [phi, eta] : m_nphi_neta_low) {
    //     for(Int_t iphi = phi; iphi < phi+m_ntowIBSide; ++iphi) {
    //         for(Int_t ieta = eta; ieta < eta+m_ntowIBSide; ++ieta) {
    //             name.str("");
    //             title.str("");
    //             name << "hADC_" << iphi << "_" << ieta;
    //             title << "ADC: (" << iphi << "," << ieta << "); Offset [mV]; ADC";

    //             m_hists[name.str()] = new TH1F(name.str().c_str(), title.str().c_str(), m_bins_offset, m_offset_low, m_offset_high);
    //         }
    //     }
    // }

    for(UInt_t i = 0; i < m_nsector; ++i) {
        Int_t x = i % (m_nsector / 2) + 1;
        Int_t y = (i < (m_nsector / 2)) ? 2 : 1;
        m_hists["h2DummySector"]->SetBinContent(x,y,i);
    }

    for(UInt_t i = 0; i < m_nib; ++i) {
        Int_t val = i % m_nib_per_sector;
        Int_t x = (i / m_nib_per_sector) % (m_nsector / 2) + 1;
        Int_t y = (i < (m_nib / 2)) ? m_nib_per_sector - val : m_nib_per_sector + val + 1;
        m_hists["h2DummyIB"]->SetBinContent(x,y,val);
    }

    setEMCalDim(m_hists["h2DummySector"]);
    setEMCalDim(m_hists["h2DummyIB"]);

    TCanvas* c1 = new TCanvas("c1");
    c1->SetTickx();
    c1->SetTicky();

    c1->SetCanvasSize(2900, 1000);
    c1->SetLeftMargin(.06);
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

    gPad->SetGrid();

    for (const auto& run : m_run) {

        string outputFile = outputDir + "/" + outputStem + "-" + run + ".pdf";
        c1->Print((outputFile + "[").c_str(), "pdf portrait");

        cout << "Run: " << run << endl;
        string histName = "h2CEMC_" + run;
        string hist1DName = "hCEMC_" + run;
        string histEventsName = "hEvent_" + run;
        title.str("");
        title << "EMCal [ADC], Run: " << run;
        m_hists[histName]->SetTitle(title.str().c_str());

        title.str("");
        title << "EMCal [ADC], Run: " << run << "; ADC; Counts";

        m_hists[hist1DName] = new TH1F(hist1DName.c_str(), title.str().c_str(), m_bins_ADC, m_ADC_low, m_ADC_high);

        setEMCalDim(m_hists[histName]);

        cout << "Events: " << m_hists[histEventsName]->GetBinContent(1) << endl;
        m_hists[histName]->Scale(1./m_hists[histEventsName]->GetBinContent(1));

        for(Int_t iphi = 1; iphi <= m_nphi; ++iphi) {
            for(Int_t ieta = 1; ieta <= m_neta; ++ieta) {
                Double_t val = m_hists[histName]->GetBinContent(iphi,ieta);
                m_hists[hist1DName]->Fill(val);
            }
        }

        m_hists[histName]->Draw("COLZ");

        m_hists[histName]->SetMinimum(0);
        m_hists[histName]->SetMaximum(1.6e4);

        m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
        m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

        c1->Print(outputFile.c_str(), "pdf portrait");
        c1->Print((outputDir + "/images/" + histName + ".png").c_str());

        m_hists[histName]->SetMaximum(6.5e3);

        c1->Print(outputFile.c_str(), "pdf portrait");
        c1->Print((outputDir + "/images/" + histName + "-zoom.png").c_str());

        c1->SetCanvasSize(1500, 1000);
        c1->SetRightMargin(.04);
        c1->SetLeftMargin(.1);
        gPad->SetLogy();
        gPad->SetGrid(0,0);

        m_hists[hist1DName]->GetXaxis()->SetTitleOffset(1);
        m_hists[hist1DName]->GetYaxis()->SetTitleOffset(1);
        m_hists[hist1DName]->Draw();
        c1->Print(outputFile.c_str(), "pdf portrait");
        c1->Print((outputDir + "/images/" + hist1DName + ".png").c_str());

        c1->SetCanvasSize(2900, 1000);
        c1->SetLeftMargin(.06);
        c1->SetRightMargin(.12);
        c1->SetTopMargin(.1);
        c1->SetBottomMargin(.12);
        gPad->SetLogy(0);
        gPad->SetGrid();

        c1->Print((outputFile + "]").c_str(), "pdf portrait");
    }

    stringstream imgName;

    TCanvas* c2 = new TCanvas("c2");
    c2->SetTickx();
    c2->SetTicky();

    c2->SetCanvasSize(3000, 2000);
    // c2->SetLeftMargin(.15);
    c2->SetRightMargin(.1);
    // c2->SetTopMargin(.1);
    // c2->SetBottomMargin(.12);

    c2->DivideSquare(64,0,0);

    // Waveforms
    for (const auto &run : m_run)
    {
        string outputFile = outputDir + "/waveforms-" + run + ".pdf";
        c2->Print((outputFile + "[").c_str(), "pdf portrait");
        for (Int_t sector = 0; sector < m_nsector; ++sector)
        {
            cout << "Sector: " << sector << endl;
            for (Int_t ib = 0; ib < m_nib_per_sector; ++ib)
            {
                pair<Int_t, Int_t> phi_eta = getSectorIBCorner(sector, ib);
                Int_t phi = phi_eta.first;
                Int_t eta = phi_eta.second;
                Int_t ctr = 1;
                imgName.str("");
                imgName << outputDir + "/images/adc_" << phi << "_" << eta << "_" << run << ".png";

                for (Int_t ieta = eta + m_ntowIBSide - 1; ieta >= eta; --ieta)
                {
                  for (Int_t iphi = phi; iphi < phi + m_ntowIBSide; ++iphi)
                  {
                    c2->cd(ctr++);

                    name.str("");
                    name << "waveform/h2adc_" << iphi << "_" << ieta << "_" << run;

                    m_hists[name.str()]->GetXaxis()->SetTitleOffset(0.9);
                    m_hists[name.str()]->GetYaxis()->SetTitleOffset(1.8);
                    m_hists[name.str()]->GetXaxis()->SetLabelSize(0.07);
                    m_hists[name.str()]->GetYaxis()->SetLabelSize(0.07);
                    m_hists[name.str()]->SetTitle("");
                    m_hists[name.str()]->Draw("COL");

                    TH1 *px = ((TH2 *) m_hists[name.str()])->ProfileX();
                    px->SetLineColor(kRed);
                    px->SetMarkerColor(kRed);
                    px->Draw("same");

                    TLine *l = new TLine(0, m_saturation, 12, m_saturation);
                    l->SetLineColor(kMagenta);
                    l->SetLineStyle(kDashed);

                    l->Draw("same");

                    if (iphi == phi && ieta == eta + m_ntowIBSide - 1)
                    {
                      title.str("");
                      title << "S" << sector << " IB" << ib;

                      TLatex latex;
                      latex.SetTextSize(0.19);
                      latex.DrawLatex(5, 12000, title.str().c_str());
                    }
                  }
                }
                c2->Print(outputFile.c_str(), "pdf portrait");
                if(m_saveFig) c2->Print(imgName.str().c_str());
            }
        }
        c2->Print((outputFile + "]").c_str(), "pdf portrait");
    }

    cout << "###############" << endl;
}

void displayv2(const string &input,
             const string &output="plots.pdf") {
    cout << "#############################" << endl;
    cout << "Run Parameters" << endl;
    cout << "input: "  << input << endl;
    cout << "output: " << output << endl;
    cout << "#############################" << endl;

    // set sPHENIX plotting style
    SetsPhenixStyle();

    if(myAnalysis::readFile(input)) return ;
    myAnalysis::analyze(output);
}

# ifndef __CINT__
Int_t main(Int_t argc, char* argv[]) {
if(argc < 2 || argc > 3){
        cout << "usage: ./displayv2 input [output]" << endl;
        cout << "input: input TFile list" << endl;
        cout << "output: output pdf file" << endl;
        return 1;
    }

    string output = "plots.pdf";

    if(argc >= 3) {
        output = argv[2];
    }

    displayv2(argv[1], output);

    cout << "======================================" << endl;
    cout << "done" << endl;
    return 0;
}
# endif
