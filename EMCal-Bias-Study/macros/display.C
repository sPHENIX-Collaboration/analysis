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
    Int_t readOffsets(const string &input);
    Int_t addHist(const string &fileName, const string &histName, unordered_map<string,TH1*> &histograms, const string &tag = "");
    void setEMCalDim(TH1* hist);

    unordered_map<string,TH1*> m_hists;
    map<string,Int_t> m_runOffset;

    Int_t m_nsector = 64;
    Int_t m_nchannel_per_sector = 384;
    Int_t m_nchannel_per_ib = 64;
    Int_t m_nphi = 256;
    Int_t m_neta = 96;
    Int_t m_nib = 384;
    Int_t m_nib_per_sector = 6;

    Int_t m_bins_offset = 25;
    Float_t m_offset_low  = -2000;
    Float_t m_offset_high = 500;

    // S49 IB5 range
    Int_t m_towPhi_low  = 136;
    Int_t m_towPhi_high = 143;

    Int_t m_towEta_low  = 0;
    Int_t m_towEta_high = 7;

    Int_t m_saturation = 16383;

    Int_t m_verbosity = 0;
    Bool_t m_saveFig = false;
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

Int_t myAnalysis::addHist(const string &fileName, const string &histName, unordered_map<string,TH1*> &histograms, const string &tag) {
    TFile *file = TFile::Open(fileName.c_str());
    if (!file || file->IsZombie()) {
      cout << "Error: Could not open ROOT file: " << fileName << endl;
      return 1;
    }

    TH1 *hist = (TH1 *) file->Get(histName.c_str());
    if (!hist) {
      cout << "Error: Histogram '" << histName << "' not found in file: " << fileName << endl;
      file->Close();
      return 1;
    }

    string histNewName = histName+tag;

    // Clone the histogram to avoid ownership issues when the file is closed
    TH1 *clonedHist = (TH1 *) hist->Clone(histNewName.c_str());

    histograms[histNewName] = clonedHist;
    if(m_verbosity) {
        cout << "Adding: " << clonedHist->GetName() << " to map." << endl;
    }
    file->Close();
    return 0;
}

Int_t myAnalysis::readOffsets(const string& input) {
    std::ifstream file(input);

    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + input);
    }

    string line;
    // Skip the header line if it exists (optional)
    if (std::getline(file, line)) {
        // You might want to process the header here if needed.
        // For example, you could check if the header is as expected.
    }

    while (std::getline(file, line)) {
        stringstream ss(line);
        string cell;
        string run;
        Int_t offset;

        // Extract the data from each cell, handling potential errors
        try {
            // serial
            if (std::getline(ss, cell, ',')) {
                    run = cell;
            } else {
                 throw std::runtime_error("Error parsing run");
            }

            // ib
            if (std::getline(ss, cell, ',')) {
                 try {
                    offset = std::stoi(cell);
                } catch (const std::invalid_argument& e) {
                    throw std::runtime_error("Invalid integer in offset: " + cell);
                } catch (const std::out_of_range& e) {
                    throw std::runtime_error("Integer out of range in offset: " + cell);
                }
            } else {
                 throw std::runtime_error("Error parsing offset");
            }

            m_runOffset[run] = offset;
        } catch (const std::runtime_error& e) {
            cout << "Error processing line: " << line << ".  Error: " << e.what() << std::endl;
            return 1;
            // You could choose to continue processing or exit here.  For now, continuing.
        }
    }

    file.close();
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
            cout << "File: " << fileName << ", Run: " << run << endl;
            addHist(line, "h2CEMC", m_hists, "_"+run);
            addHist(line, "hEvent", m_hists, "_"+run);

            stringstream name;
            for(Int_t iphi = m_towPhi_low; iphi <= m_towPhi_high; ++iphi) {
                for(Int_t ieta = m_towEta_low; ieta <= m_towEta_high; ++ieta) {
                    name.str("");
                    name << "waveform/h2adc_" << iphi << "_" << ieta;
                    addHist(line, name.str(), m_hists, "_"+run);
                }
            }
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
    fs::create_directories(outputDir);
    fs::create_directories(outputDir+"/images");

    // dummy hists for labeling
    m_hists["h2DummySector"] = new TH2F("h2DummySector","", m_nsector/2, 0, m_nsector/2, 2, 0, 2);
    m_hists["h2DummyIB"] = new TH2F("h2DummyIB","", m_nsector/2, 0, m_nsector/2, m_nib*2/m_nsector, 0, m_nib*2/m_nsector);

    // Individual IB channel response
    stringstream name;
    stringstream title;
    for(UInt_t iphi = m_towPhi_low; iphi <= m_towPhi_high; ++iphi) {
        for(UInt_t ieta = m_towEta_low; ieta <= m_towEta_high; ++ieta) {
            name.str("");
            title.str("");
            name << "hADC_" << iphi << "_" << ieta;
            title << "ADC: (" << iphi << "," << ieta << "); Offset [mV]; ADC";

            m_hists[name.str()] = new TH1F(name.str().c_str(), title.str().c_str(), m_bins_offset, m_offset_low, m_offset_high);
        }
    }

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

    c1->Print((output + "[").c_str(), "pdf portrait");

    for (const auto& [run, offset] : m_runOffset) {
        cout << "Run: " << run << ", Offset: " << offset << " mV" << endl;
        string histName = "h2CEMC_" + run;
        string histEventsName = "hEvent_" + run;
        title.str("");
        title << "EMCal [ADC], Run: " << run << ", Offset: " << offset << " mV";

        setEMCalDim(m_hists[histName]);

        cout << "Events: " << m_hists[histEventsName]->GetBinContent(1) << endl;
        m_hists[histName]->Scale(1./m_hists[histEventsName]->GetBinContent(1));

        m_hists[histName]->SetTitle(title.str().c_str());
        m_hists[histName]->Draw("COLZ");

        m_hists[histName]->SetMinimum(0);
        m_hists[histName]->SetMaximum(1.6e4);

        m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
        m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

        c1->Print(output.c_str(), "pdf portrait");
        if(m_saveFig) c1->Print((outputDir + "/images/" + histName + ".png").c_str());

        title << ", Sector 49 IB 5";

        m_hists[histName]->SetTitle(title.str().c_str());
        m_hists[histName]->GetXaxis()->SetRangeUser(136,144);
        m_hists[histName]->GetYaxis()->SetRangeUser(0,8);
        m_hists[histName]->GetXaxis()->SetNdivisions(8, false);
        m_hists[histName]->GetYaxis()->SetNdivisions(8, false);
        m_hists[histName]->Draw("COLZ");

        // zoom
        c1->SetCanvasSize(1500, 1000);
        c1->SetRightMargin(.18);
        // c1->SetLeftMargin(.06);
        // c1->SetTopMargin(.1);
        // c1->SetBottomMargin(.12);

        TPaletteAxis *palette = (TPaletteAxis*)m_hists[histName]->GetListOfFunctions()->FindObject("palette");

        // // the following lines move the palette. Choose the values you need for the position.
        Float_t xshift = 0.05;
        Float_t xlow = 0.83;
        palette->SetX1NDC(xlow);
        palette->SetX2NDC(xlow+xshift);
        // palette->SetY1NDC(0.2);
        // palette->SetY2NDC(0.8);
        gPad->Modified();
        gPad->Update();

        c1->Print(output.c_str(), "pdf portrait");
        if(m_saveFig) c1->Print((outputDir + "/images/" + histName + "-zoom.png").c_str());

        c1->SetCanvasSize(2900, 1000);
        c1->SetLeftMargin(.06);
        c1->SetRightMargin(.12);
        c1->SetTopMargin(.1);
        c1->SetBottomMargin(.12);

        for(UInt_t iphi = m_towPhi_low; iphi <= m_towPhi_high; ++iphi) {
            for(UInt_t ieta = m_towEta_low; ieta <= m_towEta_high; ++ieta) {
                name.str("");
                name << "hADC_" << iphi << "_" << ieta;
                Float_t adc = m_hists[histName]->GetBinContent(iphi+1, ieta+1);
                Float_t adcError = m_hists[histName]->GetBinError(iphi+1, ieta+1);
                Int_t bin_offset = m_hists[name.str()]->FindBin(offset);

                m_hists[name.str()]->SetBinContent(bin_offset, adc);
                m_hists[name.str()]->SetBinError(bin_offset, adcError);
            }
        }
    }

    c1->Print((output + "]").c_str(), "pdf portrait");

    // save plots to root file
    name.str("");
    name << outputDir << "/ADC-biasOffset.root";
    TFile tf(name.str().c_str(),"recreate");
    tf.cd();
    for(UInt_t iphi = m_towPhi_low; iphi <= m_towPhi_high; ++iphi) {
        for(UInt_t ieta = m_towEta_low; ieta <= m_towEta_high; ++ieta) {
            name.str("");
            name << "hADC_" << iphi << "_" << ieta;

            m_hists[name.str()]->Write();
        }
    }
    tf.Close();

    TCanvas* c2 = new TCanvas("c2");
    c2->SetTickx();
    c2->SetTicky();

    c2->SetCanvasSize(3000, 2000);
    // c2->SetLeftMargin(.15);
    c2->SetRightMargin(.1);
    // c2->SetTopMargin(.1);
    // c2->SetBottomMargin(.12);

    c2->DivideSquare(64,0,0);

    c2->Print((outputDir + "/ADC.pdf[").c_str(), "pdf portrait");

    Int_t ctr = 1;
    for(Int_t ieta = m_towEta_high; ieta >= m_towEta_low; --ieta) {
        for(Int_t iphi = m_towPhi_low; iphi <= m_towPhi_high; ++iphi) {
            c2->cd(ctr++);
            name.str("");
            title.str("");
            name << "hADC_" << iphi << "_" << ieta;
            title << "(" << iphi << "," << ieta << ")";
            m_hists[name.str()]->GetYaxis()->SetRangeUser(0,1.6e4);
            m_hists[name.str()]->GetXaxis()->SetTitleOffset(0.9);
            m_hists[name.str()]->GetYaxis()->SetTitleOffset(1.8);
            m_hists[name.str()]->GetXaxis()->SetLabelSize(0.07);
            m_hists[name.str()]->GetYaxis()->SetLabelSize(0.07);
            m_hists[name.str()]->SetTitle("");
            m_hists[name.str()]->Draw();

            TLatex latex;
            latex.SetTextSize(0.17);
            if(ieta == 0) {
                latex.DrawLatex(-600,6000,title.str().c_str());
            }
            else {
                latex.DrawLatex(-500,6000,title.str().c_str());
            }
        }
    }

    c2->Print((outputDir + "/ADC.pdf").c_str(), "pdf portrait");
    if(m_saveFig) c2->Print((outputDir + "/images/ADC.png").c_str());

    // Waveforms
    for (const auto& [run, offset] : m_runOffset) {
        ctr = 1;
        title.str("");
        title << offset << " mV";

        for(Int_t ieta = m_towEta_high; ieta >= m_towEta_low; --ieta) {
            for(Int_t iphi = m_towPhi_low; iphi <= m_towPhi_high; ++iphi) {
                c2->cd(ctr++);

                name.str("");
                name << "waveform/h2adc_" << iphi << "_" << ieta << "_" << run;

                m_hists[name.str()]->GetXaxis()->SetTitleOffset(0.9);
                m_hists[name.str()]->GetYaxis()->SetTitleOffset(1.8);
                m_hists[name.str()]->GetXaxis()->SetLabelSize(0.07);
                m_hists[name.str()]->GetYaxis()->SetLabelSize(0.07);
                m_hists[name.str()]->SetTitle("");
                m_hists[name.str()]->Draw("COL");

                TH1* px = ((TH2*)m_hists[name.str()])->ProfileX();
                px->SetLineColor(kRed);
                px->SetMarkerColor(kRed);
                px->Draw("same");

                TLine* l = new TLine(0, m_saturation, 12, m_saturation);
                l->SetLineColor(kMagenta);
                l->SetLineStyle(kDashed);

                l->Draw("same");

                if(iphi == 143 && ieta == 7) {
                    TLatex latex;
                    latex.SetTextSize(0.19);
                    latex.DrawLatex(5,2000,title.str().c_str());
                }
            }
        }
        c2->Print((outputDir + "/ADC.pdf").c_str(), "pdf portrait");
        if(m_saveFig) c2->Print((outputDir + "/images/adc-" + run + ".png").c_str());
    }

    c2->Print((outputDir + "/ADC.pdf]").c_str(), "pdf portrait");

    cout << "###############" << endl;
}

void display(const string &input,
             const string &input_offset,
             const string &output="plots.pdf") {
    cout << "#############################" << endl;
    cout << "Run Parameters" << endl;
    cout << "input: "  << input << endl;
    cout << "input offsets: "  << input_offset << endl;
    cout << "output: " << output << endl;
    cout << "#############################" << endl;

    // set sPHENIX plotting style
    SetsPhenixStyle();

    if(myAnalysis::readFile(input) || myAnalysis::readOffsets(input_offset)) return ;
    myAnalysis::analyze(output);
}

# ifndef __CINT__
Int_t main(Int_t argc, char* argv[]) {
if(argc < 3 || argc > 4){
        cout << "usage: ./display input input_offset [output]" << endl;
        cout << "input: input TFile list" << endl;
        cout << "input_offset: input runs with offsets csv" << endl;
        cout << "output: output pdf file" << endl;
        return 1;
    }

    string output = "plots.pdf";

    if(argc >= 4) {
        output = argv[3];
    }

    display(argv[1], argv[2], output);

    cout << "======================================" << endl;
    cout << "done" << endl;
    return 0;
}
# endif
