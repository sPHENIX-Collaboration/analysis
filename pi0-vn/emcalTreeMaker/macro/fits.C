// -- c++ includes --
#include <iostream>
#include <iomanip>
#include <fstream>
#include <filesystem>

// -- root includes --
#include <TH1F.h>
#include <TF1.h>
#include <TFile.h>
#include <TLine.h>
#include <TLatex.h>
#include <TFitResult.h>
#include <TCanvas.h>

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
    TFitResultPtr PerformFitting(TH1F* hPi0Mass, TF1** totalFit);
    Double_t CalculateSignalToBackgroundRatio(TH1F* hPi0Mass, TF1* polyFit, Double_t fitMean, Double_t fitSigma, Double_t &signalToBackgroundError);
    void process_fits(const string &i_input, const string &outputCSV, const string &outputDir, const string &tag, const Float_t sigmaMult);

    vector<string> centrality = {"40-60","20-40","0-20"};
    vector<string> pts         = {"2-2.5","2.5-3","3-3.5","3.5-4","4-4.5","4.5-5"};

    Double_t fitStart = 0.1;
    Double_t fitEnd   = 0.35;
    Double_t sigmaMult = 2;
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

TFitResultPtr myAnalysis::PerformFitting(TH1F* hPi0Mass, TF1** totalFit) {

    Double_t lowerSignalBoundEstimate = 0.1;
    Double_t upperSignalBoundEstimate = 0.2;

    Double_t sigmaEstimate = 0.024; // sigmaEstimate value

    // Define totalFit
    *totalFit = new TF1("totalFit", "gaus(0) + pol2(3)", fitStart, fitEnd);

    /*
     Give overestimation for amplitude to help fit converge on correct value in correct region
     Set mean estimate to this values bin center
     */
    Int_t bin1 = hPi0Mass->GetXaxis()->FindBin(lowerSignalBoundEstimate);
    Int_t bin2 = hPi0Mass->GetXaxis()->FindBin(upperSignalBoundEstimate);
    Int_t maxBin = bin1;
    Double_t maxBinContent = hPi0Mass->GetBinContent(bin1);
    for (Int_t i = bin1 + 1; i <= bin2; ++i) {
        if (hPi0Mass->GetBinContent(i) > maxBinContent) {
            maxBinContent = hPi0Mass->GetBinContent(i);
            maxBin = i;
        }
    }
    Double_t maxBinCenter = hPi0Mass->GetXaxis()->GetBinCenter(maxBin);

    (*totalFit)->SetParameter(0, maxBinContent);

    (*totalFit)->SetParameter(1, maxBinCenter);

    (*totalFit)->SetParameter(2, sigmaEstimate);

    TFitResultPtr fitResult = hPi0Mass->Fit("totalFit", "SQR+");

    return fitResult; // Return the fit result
}

Double_t myAnalysis::CalculateSignalToBackgroundRatio(TH1F* hPi0Mass, TF1* polyFit, Double_t fitMean, Double_t fitSigma, Double_t &signalToBackgroundError) {
    // Cloning histograms for signal and background
    TH1F *hSignal = (TH1F*)hPi0Mass->Clone("hSignal");
    TH1F *hBackground = (TH1F*)hPi0Mass->Clone("hBackground");

    Double_t low  = max(fitMean - sigmaMult*fitSigma, fitStart);
    Double_t high = min(fitMean + sigmaMult*fitSigma, fitEnd);

    Int_t firstBinSignal = hPi0Mass->FindBin(low);
    Int_t lastBinSignal  = hPi0Mass->FindBin(high);

    Double_t binCenter, binContent, bgContent, binError;
    for (Int_t i = firstBinSignal; i <= lastBinSignal; ++i) {
        binCenter = hPi0Mass->GetBinCenter(i);
        binContent = hPi0Mass->GetBinContent(i);
        binError = hPi0Mass->GetBinError(i);
        bgContent = polyFit->Eval(binCenter);
        bgContent = max(bgContent, 0.0);

        // Setting signal and background contents
        hSignal->SetBinContent(i, binContent - bgContent);
        hBackground->SetBinContent(i, bgContent);

        // Set bin errors
        hSignal->SetBinError(i, binError); // Error for signal
        hBackground->SetBinError(i, sqrt(bgContent)); // Error for background (Poisson statistics)
    }

    Double_t signalYield_, signalError_, backgroundYield, backgroundError;
    signalYield_ = hSignal->IntegralAndError(firstBinSignal, lastBinSignal, signalError_, "");
    backgroundYield = hBackground->IntegralAndError(firstBinSignal, lastBinSignal, backgroundError, "");

    Double_t signalToBackgroundRatio = signalYield_ / backgroundYield;

    // Error propagation for division
    signalToBackgroundError = signalToBackgroundRatio * sqrt(pow(signalError_ / signalYield_, 2) + pow(backgroundError / backgroundYield, 2));

    return signalToBackgroundRatio;
}

void myAnalysis::process_fits(const string &i_input,
                              const string &outputCSV,
                              const string &outputDir,
                              const string &tag,
                              const Float_t sigmaMult) {
    TFile input(i_input.c_str());

    stringstream s;
    stringstream t;
    stringstream u;

    TCanvas* c1 = new TCanvas();
    c1->SetTickx();
    c1->SetTicky();

    c1->SetCanvasSize(1500, 1000);
    c1->SetLeftMargin(.12);
    c1->SetRightMargin(.05);

    TLatex l1;
    l1.SetTextSize(0.05);

    ofstream output((outputDir + "/" + outputCSV).c_str());
    output << "Index,mean,sigma,S/B,chi2,EA,EB,asym" << endl;

    for(auto cut : cuts) {
        Int_t index = 0;

        t.str("");
        t << outputDir << "/" << tag << "_" << cut.e1 << "_" << cut.e2 << "_" << cut.asym << "_" << cut.deltaR_min << "_" << cut.deltaR_max << "_" << cut.chi;

        if (std::filesystem::exists(t.str()))
        {
          std::cout << "Directory '" << t.str() << "' already exists." << std::endl;
        }
        else
        {
          try
          {
            std::filesystem::create_directory(t.str());
            std::cout << "Directory '" << t.str() << "' created successfully." << std::endl;
          }
          catch (const std::filesystem::filesystem_error &e)
          {
            // Handle other potential errors
            std::cerr << "Error creating directory: " << e.what() << std::endl;
          }
        }

        u.str("");
        u << t.str() << "/" << cut.e1 << "_" << cut.e2 << "_" << cut.asym << "_" << cut.deltaR_min << "_" << cut.deltaR_max << "_" << cut.chi;
        c1->Print((u.str() + ".pdf[").c_str(), "pdf portrait");

        for(auto cent : centrality) {
            for(auto pt : pts) {
                s.str("");
                s << "results/" << cent << "/" << pt << "/"
                    << "hPi0Mass_" << cent << "_" << pt << "_"
                    << cut.e1 << "_" << cut.e2 << "_" << cut.asym << "_"
                    << cut.deltaR_min << "_" << cut.deltaR_max << "_" << cut.chi;

                auto h = (TH1F*)input.Get(s.str().c_str());
                Double_t numEntries = h->Integral(h->FindBin(fitStart), h->FindBin(fitEnd));

                // ensure there are enough entries before fitting
                if(numEntries < 20){
                    ++index;
                    continue;
                }

                TF1 *totalFit;

                // Call PerformFitting to execute the fitting procedure
                TFitResultPtr fitResult = PerformFitting(h, &totalFit);

                Double_t fitMean       = totalFit->GetParameter(1);
                Double_t fitMeanError  = totalFit->GetParError(1);
                Double_t fitSigma      = totalFit->GetParameter(2);
                Double_t fitSigmaError = totalFit->GetParError(2);
                Double_t chi2          = fitResult->Chi2();
                Int_t    ndf           = fitResult->Ndf();

                if(fitMean < 0) {
                    ++index;
                    continue;
                }

                TF1 *gaussFit = new TF1("gaussFit", "gaus", fitStart, fitEnd);
                TF1 *polyFit  = new TF1("polyFit", "pol2",  fitStart, fitEnd);
                gaussFit->SetParameter(0, totalFit->GetParameter(0));
                gaussFit->SetParameter(1, totalFit->GetParameter(1));
                gaussFit->SetParameter(2, totalFit->GetParameter(2));

                for (Int_t i = 3; i < 6; i++) {
                    polyFit->SetParameter(i - 3, totalFit->GetParameter(i));
                    polyFit->SetParError(i - 3, totalFit->GetParError(i));
                }

                Double_t signalToBackgroundError;
                Double_t signalToBackgroundRatio = CalculateSignalToBackgroundRatio(h, polyFit, fitMean, fitSigma, signalToBackgroundError);

                s.str("");
                s << index << "," << fitMean << "," << fitSigma << "," << signalToBackgroundRatio << "," << chi2 << "," << cut.e1 << "," << cut.e2 << "," << cut.asym << endl;

                output << s.str();

                // round to 4 decimal places
                fitMean                 = (Int_t)(fitMean*1e4)/1e4;
                fitSigma                = (Int_t)(fitSigma*1e4)/1e4;
                signalToBackgroundRatio = (Int_t)(signalToBackgroundRatio*1e4)/1e4;
                chi2                    = (Int_t)(chi2*1e2)/1e2;

                c1->cd();
                h->SetTitle("Reconstructed Diphoton, MB + Central Events");
                h->SetMarkerStyle(kFullDotLarge);
                h->Draw();
                h->SetStats();

                s.str("");
                s << "Centrality: " << cent << "%, "
                  << 2+fmod(index,6)*0.5 << " #leq p_{T} < " << 2.5+fmod(index,6)*0.5 << " GeV";

                l1.DrawLatexNDC(0.15,0.83, s.str().c_str());
                l1.SetTextSize(0.04);

                s.str("");
                s << "#splitline{Cuts (inclusive):}"
                  << "{#splitline{Asymmetry < " << cut.asym << "}"
                  << "{#splitline{#chi^{2} < " << cut.chi << "}"
                  << "{#splitline{Cluster E_{A} #geq " << cut.e1 << " GeV}"
                  << "{#splitline{Cluster E_{B} #geq " << cut.e2 << " GeV}"
                  << "{" << cut.deltaR_min << " #leq #Delta R < " << cut.deltaR_max << "}"
                  << "}}}}";

                l1.DrawLatexNDC(0.15,0.75, s.str().c_str());

                s.str("");
                s << "#splitline{Fit:}"
                  << "{#splitline{#mu: " << fitMean << " GeV}"
                  << "{#splitline{#sigma: " << fitSigma << " GeV}"
                  << "{#splitline{S/B: " << signalToBackgroundRatio << "}"
                  << "{#chi^{2} / ndf: " << chi2 << "/" << ndf << "}"
                  << "}}}";

                l1.DrawLatexNDC(0.45,0.75, s.str().c_str());

                Double_t y = h->GetBinContent(h->FindBin(fitMean));

                if(y > 0) h->GetYaxis()->SetRangeUser(0,2.5*y);

                Double_t signal_low  = max(fitMean-sigmaMult*fitSigma, fitStart);
                Double_t signal_high = min(fitMean+sigmaMult*fitSigma, fitEnd);

                TLine *line1 = new TLine(signal_low, 0, signal_low, 1.2*y);
                TLine *line2 = new TLine(signal_high, 0, signal_high, 1.2*y);
                line1->SetLineColor(kBlack);
                line1->SetLineStyle(1);
                line2->SetLineColor(kBlack);
                line2->SetLineStyle(1);
                line1->Draw("same");
                line2->Draw("same");

                gaussFit->SetLineColor(kOrange+2);
                gaussFit->SetLineStyle(2);
                gaussFit->Draw("SAME");
                polyFit->SetLineColor(kBlue);
                polyFit->SetLineWidth(3);
                polyFit->SetLineStyle(2);
                polyFit->Draw("SAME");

                s.str("");
                s << t.str() << "/" << cent << "_" << pt;

                c1->Print((s.str() + ".png").c_str());
                c1->Print((u.str() + ".pdf").c_str(), "pdf portrait");
                ++index;
            }
        }
        c1->Print((u.str() + ".pdf]").c_str(), "pdf portrait");
    }

    output.close();
    input.Close();
}

void fits(const string &i_input,
          const string &i_cuts,
          const string &outputCSV = "fitStats.csv",
          const string &outputDir = ".",
          const string &tag = "test",
          const Float_t sigmaMult = 2) {

    cout << "#############################" << endl;
    cout << "Run Parameters" << endl;
    cout << "inputFile: "        << i_input << endl;
    cout << "Cuts: "             << i_cuts << endl;
    cout << "output csv: "       << outputCSV << endl;
    cout << "output directory: " << outputDir << endl;
    cout << "tag: "              << tag << endl;
    cout << "sigmaMult: "        << sigmaMult << endl;
    cout << "#############################" << endl;

    myAnalysis::sigmaMult = sigmaMult;
    Int_t ret = myAnalysis::readCuts(i_cuts);
    if(ret != 0) return;

    if (std::filesystem::exists(outputDir))
    {
        std::cout << "Directory '" << outputDir << "' already exists." << std::endl;
    }
    else
    {
        try
        {
        std::filesystem::create_directory(outputDir);
        std::cout << "Directory '" << outputDir << "' created successfully." << std::endl;
        }
        catch (const std::filesystem::filesystem_error &e)
        {
        // Handle other potential errors
        std::cerr << "Error creating directory: " << e.what() << std::endl;
        }
    }

    myAnalysis::process_fits(i_input, outputCSV, outputDir, tag, sigmaMult);
}

# ifndef __CINT__
Int_t main(Int_t argc, char* argv[]) {
if(argc < 3 || argc > 7){
        cout << "usage: ./fits inputFile cuts [fitStats] [outputDir] [tag] [sigmaMult]" << endl;
        cout << "inputFile: input root file" << endl;
        cout << "cuts: csv file containing cuts" << endl;
        cout << "fitStats: csv file containing fit Stats" << endl;
        cout << "outputDir: location of output directory. Default: current directory." << endl;
        cout << "tag: tag for the output folder. Default: test." << endl;
        cout << "sigmaMult: controls the signal bounds. Default: 2." << endl;
        return 1;
    }

    string fitStats  = "fitStats.csv";
    string outputDir = ".";
    string tag = "test";
    Float_t sigmaMult = 2;

    if(argc >= 4) {
        fitStats = argv[3];
    }
    if(argc >= 5) {
        outputDir = argv[4];
    }
    if(argc >= 6) {
        tag = argv[5];
    }
    if(argc >= 7) {
        sigmaMult = atof(argv[6]);
    }

    fits(argv[1], argv[2], fitStats, outputDir, tag, sigmaMult);

    cout << "======================================" << endl;
    cout << "done" << endl;
    return 0;
}
# endif
