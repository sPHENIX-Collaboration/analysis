#include "sPhenixStyle.h"
#include "sPhenixStyle.C"
#include <TROOT.h>
#include <TColor.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TH1F.h>
#include <TF1.h>
#include <TLatex.h>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include "TSystem.h"
#include "TSystemDirectory.h"
#include "TList.h"
#include "TSystemFile.h"
#include <sys/stat.h>

struct CutValues {
    float clusEA, clusEB, asymmetry, deltaRMin, deltaRMax, chi;
};

CutValues globalCutValues;

// Global variable
std::string globalDataPath = "/Users/patsfan753/Desktop/p015/InvMassOutput/CSV/";
std::string globalPlotOutput = "/Users/patsfan753/Desktop/p015/InvMassOutput/Plots/"; //Note: Folder auto created with name of cut variation to this
std::string globalFilename = "/Users/patsfan753/Desktop/p015/hPi0Mass_EA1_EB1_Asym0point5_DelrMin0_DelrMax1_Chi4.root";

int histIndex = 0;
double globalFitStart;
double globalFitEnd;

TFitResultPtr PerformFitting(TH1F* hPi0Mass, TF1*& totalFit, double& fitStart, double& fitEnd) {
    ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit2");
    // Increase number of allowed function calls (e.g., from default 500 to 5000)
    ROOT::Math::MinimizerOptions::SetDefaultMaxFunctionCalls(5000);

    fitStart = 0.1;
    fitEnd = 0.35;
    
    globalFitStart = fitStart;
    globalFitEnd = fitEnd;

    double sigmaEstimate = 0.025; // sigmaEstimate value

    
    // Define totalFit
    totalFit = new TF1("totalFit", "gaus(0) + pol2(3)", fitStart, fitEnd);
    totalFit->SetLineColor(kRed);

    
    double amplitudeEstimate = hPi0Mass->GetBinContent(hPi0Mass->GetXaxis()->FindBin(0.185));
    totalFit->SetParameter(0, amplitudeEstimate);
    
    totalFit->SetParameter(1, 0.185);
    
    totalFit->SetParameter(2, sigmaEstimate);

    TFitResultPtr fitResult = hPi0Mass->Fit("totalFit", "SR+");
    return fitResult; // Return the fit result
}
/*
 Function to parse the filename and extract cut values to be propagated throughout macro for automation of cut variation analysis
 */
CutValues parseFileName() {
    CutValues cuts = {0, 0, 0, 0}; // Initialize cut values to zero
    std::regex re("hPi0Mass_EA([0-9]+(?:point[0-9]*)?)_EB([0-9]+(?:point[0-9]*)?)_Asym([0-9]+(?:point[0-9]*)?)_DelrMin([0-9]+(?:point[0-9]*)?)_DelrMax([0-9]+(?:point[0-9]*)?)_Chi([0-9]+(?:point[0-9]*)?)\\.root");

    std::smatch match; // Object to store the results of regex search

    if (std::regex_search(globalFilename, match, re) && match.size() > 4) {
        // Lambda function to convert a string with 'point' to a float
        auto convert = [](const std::string& input) -> float {
            // Create a temporary string to hold the modified input
            std::string temp = input;
            // Find the position of 'point' in the string
            size_t pointPos = temp.find("point");
            // If 'point' is found, replace it with a decimal point '.'
            // The 'point' substring has a length of 5 characters
            if (pointPos != std::string::npos) {
                temp.replace(pointPos, 5, ".");
            }
            // Attempt to convert the modified string to a float
            // Use a try-catch block to handle any exceptions that might occur during conversion
            try {
                return std::stof(temp);
            } catch (const std::exception&) {
                return 0.0f;
            }
        };
        // Diagnostic prints
        std::cout << "Matched Filename: " << globalFilename << std::endl;
        for (size_t i = 1; i < match.size(); ++i) {
            std::cout << "Match[" << i << "]: " << match[i].str() << " --> " << convert(match[i].str()) << std::endl;
        }
        cuts.clusEA = convert(match[1].str()); //minimum cluster energy cut 1
        cuts.clusEB = convert(match[2].str()); //minimum cluster energy cut 2
        cuts.asymmetry = convert(match[3].str()); //Asym Cut
        cuts.deltaRMin = convert(match[4].str()); //Delta R cut minimum
        cuts.deltaRMax = convert(match[5].str()); //Delta R cut max --really just is 1
        cuts.chi = convert(match[6].str()); //chi2 cut
    } else {
        // If regex does not match, log an error message
        std::cout << "Regex did not match the filename: " << globalFilename << std::endl;
    }

    return cuts;// Return the populated cuts structure
}

struct Range {
    double ptLow, ptHigh, mbdLow, mbdHigh;
};
/*
 For automatic labelling of pT and centrality bins
 */
Range ranges[] = {
    //40-60 percent centrality
    {2.0, 2.5, 40, 60},       // index 0
    {2.5, 3.0, 40, 60},       // index 1
    {3.0, 3.5, 40, 60},       // index 2
    {3.5, 4.0, 40, 60},       // index 3
    {4.0, 4.5, 40, 60},       // index 4
    {4.5, 5.0, 40, 60},       // index 5
    
    //20 - 40 percent centrality
    {2.0, 2.5, 20, 40},       // index 6
    {2.5, 3.0, 20, 40},       // index 7
    {3.0, 3.5, 20, 40},       // index 8
    {3.5, 4.0, 20, 40},       // index 9
    {4.0, 4.5, 20, 40},       // index 10
    {4.5, 5.0, 20, 40},       // index 11
    
    //0 - 20 percent centrality
    {2.0, 2.5, 0, 20},       // index 12
    {2.5, 3.0, 0, 20},       // index 13
    {3.0, 3.5, 0, 20},       // index 14
    {3.5, 4.0, 0, 20},       // index 15
    {4.0, 4.5, 0, 20},       // index 16
    {4.5, 5.0, 0, 20},       // index 17

};
/*
 Signal to background ratio calculation
 */
struct SignalBackgroundRatio {
    std::map<double, double> ratios;
    std::map<double, double> errors;
};
SignalBackgroundRatio CalculateSignalToBackgroundRatio(TH1F* hPi0Mass, TF1* polyFit, double fitMean, double fitSigma, double fitStart, double fitEnd) {
    std::vector<double> sigmaMultipliers = {1.25, 1.5, 1.75, 2.0, 2.25};
    SignalBackgroundRatio sbRatios;
    std::cout << "\033[1;31mEntering CalculateSignalToBackgroundRatio\033[0m" << std::endl;
    std::cout << "\033[1;31mfitMean: " << fitMean << ", fitSigma: " << fitSigma << ", fitStart: " << fitStart << ", fitEnd: " << fitEnd << "\033[0m" << std::endl;
    
    
    
    for (double multiplier : sigmaMultipliers) {
        // Move the cloning and cleanup of histograms outside the sigmaMultipliers loop
        TH1F *hSignal = (TH1F*)hPi0Mass->Clone("hSignal");
        TH1F *hBackground = (TH1F*)hPi0Mass->Clone("hBackground");
        
        int firstBinSignal = hPi0Mass->FindBin(std::max(fitMean - multiplier * fitSigma, fitStart));
        int lastBinSignal = hPi0Mass->FindBin(std::min(fitMean + multiplier * fitSigma, fitEnd));
        
        std::cout << "Multiplier: " << multiplier << ", First bin: " << firstBinSignal << ", Last bin: " << lastBinSignal << std::endl;

        double binCenter, binContent, bgContent, binError;
        for (int i = firstBinSignal; i <= lastBinSignal; ++i) {
            binCenter = hPi0Mass->GetBinCenter(i);
            binContent = hPi0Mass->GetBinContent(i);
            binError = hPi0Mass->GetBinError(i);
            bgContent = polyFit->Eval(binCenter);
            bgContent = std::max(bgContent, 0.0);
            
            // Setting signal and background contents
            hSignal->SetBinContent(i, binContent - bgContent);
            hBackground->SetBinContent(i, bgContent);
            // Set bin errors
            hSignal->SetBinError(i, binError); // Error for signal
            hBackground->SetBinError(i, sqrt(bgContent)); // Error for background (Poisson statistics)
        }
        double signalYield, signalError, backgroundYield, backgroundError;
        signalYield = hSignal->IntegralAndError(firstBinSignal, lastBinSignal, signalError, "");// Option "" means error is computed using sqrt(N) for each bin
        backgroundYield = hBackground->IntegralAndError(firstBinSignal, lastBinSignal, backgroundError, "");

        // Protect against division by zero
        double signalToBackgroundRatio = backgroundYield > 0 ? signalYield / backgroundYield : 0;
        double signalToBackgroundError = signalToBackgroundRatio > 0 ? signalToBackgroundRatio * sqrt(pow(signalError / signalYield, 2) + pow(backgroundError / backgroundYield, 2)) : 0;

        sbRatios.ratios[multiplier] = signalToBackgroundRatio;
        sbRatios.errors[multiplier] = signalToBackgroundError;

        std::cout << "Signal yield: " << signalYield << ", Background yield: " << backgroundYield << std::endl;
        
        std::cout << "S/B Ratio: " << signalToBackgroundRatio << ", Error: " << signalToBackgroundError << std::endl;
        
        sbRatios.ratios[multiplier] = signalToBackgroundRatio;
        sbRatios.errors[multiplier] = signalToBackgroundError;
        delete hSignal;
        delete hBackground;
        
        
    }
    std::cout << "\033[1;31mExiting CalculateSignalToBackgroundRatio\033[0m" << std::endl;
    return sbRatios;
}
/*
 Calculation for signal yield and error, simiarly to above method outputs to text file upon user input in terminal when happy with fit
 */
double CalculateSignalYieldAndError(TH1F* hPi0Mass, TF1* polyFit, double fitMean, double fitSigma, double& signalError, double fitStart, double fitEnd) {
    TH1F *hSignal = (TH1F*)hPi0Mass->Clone("hSignal");
    double binCenter, binContent, bgContent, binError;
    int firstBinSignal = hPi0Mass->FindBin(std::max(fitMean - 2 * fitSigma, fitStart));
    int lastBinSignal = hPi0Mass->FindBin(std::min(fitMean + 2 * fitSigma, fitEnd));
    for (int i = firstBinSignal; i <= lastBinSignal; ++i) {
        binCenter = hPi0Mass->GetBinCenter(i);
        binContent = hPi0Mass->GetBinContent(i);
        binError = hPi0Mass->GetBinError(i);
        bgContent = polyFit->Eval(binCenter);
        bgContent = std::max(bgContent, 0.0);
        hSignal->SetBinContent(i, binContent - bgContent);
        hSignal->SetBinError(i, binError);
    }
    double signalYield = hSignal->IntegralAndError(firstBinSignal, lastBinSignal, signalError, "");
    delete hSignal; // Clean up
    return signalYield; // Return the yield
}

/*
 Automatic update of canvas cut values, fit information, and analysis bin
 */
void DrawCanvasText(TLatex& latex, const Range& selectedRange, double fitMean, double fitSigma, const SignalBackgroundRatio& sbRatios, double signalYield, double signalYieldError, double numEntries, double chi2, double NDF) {
    // Drawing text related to the range and cuts
    std::ostringstream mbdStream, ptStream;
    mbdStream << std::fixed << std::setprecision(0) << "Centrality: " << selectedRange.mbdLow << " - " << selectedRange.mbdHigh << "%";
    ptStream << std::fixed << std::setprecision(2) << selectedRange.ptLow << " #leq Diphoton p_{T} < " << selectedRange.ptHigh << " GeV";

    latex.SetTextSize(0.038);
    latex.DrawLatex(0.55, 0.76, mbdStream.str().c_str());
    latex.DrawLatex(0.55, 0.71, ptStream.str().c_str());
    latex.DrawLatex(0.55, 0.66, Form("Entries = %.0f", numEntries));
    
    latex.DrawLatex(0.2, 0.9, Form("Asymmetry < %.2f", globalCutValues.asymmetry));
    latex.DrawLatex(0.2, 0.85, Form("Cluster #chi^{2} < %.2f", globalCutValues.chi));
    latex.DrawLatex(0.2, 0.8, Form("Cluster E_{A} #geq %.2f GeV", globalCutValues.clusEA));
    latex.DrawLatex(0.2, 0.75, Form("Cluster E_{B} #geq %.2f GeV", globalCutValues.clusEB));
    latex.DrawLatex(0.2, 0.7, Form("%.2f #leq #Delta R < %.2f", globalCutValues.deltaRMin, globalCutValues.deltaRMax));

    double ratioFor2Sigma = sbRatios.ratios.at(2.0);
    double errorFor2Sigma = sbRatios.errors.at(2.0);
//    latex.DrawLatex(0.67, 0.4, Form("#mu_{Gaussian} = %.2f GeV", fitMean));
//    latex.DrawLatex(0.67, 0.35, Form("#sigma_{Gaussian} = %.2f GeV", fitSigma));
//    latex.DrawLatex(0.67, 0.3, Form("S/B = %.2f", ratioFor2Sigma));
//    latex.DrawLatex(0.67, 0.25, Form("Signal Yield = %.2f", signalYield));
//    latex.DrawLatex(0.67, .2, Form("Fit #chi^{2}/NDF: %.2f/%.0f", chi2, NDF));
    
    
    latex.DrawLatex(0.45, 0.54, Form("#mu_{Gaussian} = %.2f GeV", fitMean));
    latex.DrawLatex(0.45, 0.49, Form("#sigma_{Gaussian} = %.2f GeV", fitSigma));
    latex.DrawLatex(0.67, 0.57, Form("S/B = %.2f", ratioFor2Sigma));
    latex.DrawLatex(0.67, 0.52, Form("Signal Yield = %.2f", signalYield));
    latex.DrawLatex(0.67, .47, Form("Fit #chi^{2}/NDF: %.2f/%.0f", chi2, NDF));
}
/*
Track Relevant Output in CSV tracking via indices and unique cut combinations
 */
void WriteDataToCSV(int histIndex, const CutValues& cutValues, double fitMean, double fitMeanError, double fitSigma, double fitSigmaError, const SignalBackgroundRatio& sbRatios, double signalYield, double signalError, double numEntries, double chi2) {


    std::string csv_filename = globalDataPath + "PlotByPlotOutput_4_2_variedSignalBounds.csv";
    std::ifstream checkFile(csv_filename);
    bool fileIsEmpty = checkFile.peek() == std::ifstream::traits_type::eof();
    checkFile.close();

    std::ofstream file(csv_filename, std::ios::app); // Open the file in append mode

    if (!file.is_open()) {
        std::cerr << "Unable to open CSV file for writing." << std::endl;
        return;
    }
    // Define the sigma multipliers
    std::vector<double> sigmaMultipliers = {1.25, 1.5, 1.75, 2.0, 2.25};
    // Write column headers if file is empty, including signal bounds headers at the correct position
    if (fileIsEmpty) {
        file << "Index,EnergyA,EnergyB,Asymmetry,Chi2,DeltaRMin,DeltaRMax,GaussMean,GaussMeanError,GaussSigma,GaussSigmaError";
        for (const auto& pair : sbRatios.ratios) {
            file << ",S/B_" << pair.first << "sigma,S/BError_" << pair.first << "sigma";
        }
        file << ",NumEntry,Yield,YieldError,RelativeSignalError,chi2";
        for (double multiplier : sigmaMultipliers) {
            file << ",LowerBound_" << multiplier << "sigma,UpperBound_" << multiplier << "sigma";
        }
        file << "\n";
    }
    double relativeSignalError = signalYield != 0 ? signalError / signalYield : 0;

    file << histIndex << ","
         << cutValues.clusEA << ","
         << cutValues.clusEB << ","
         << cutValues.asymmetry << ","
         << cutValues.chi << ","
         << cutValues.deltaRMin << ","
         << cutValues.deltaRMax << ","
         << fitMean << ","
         << fitMeanError << ","
         << fitSigma << ","
         << fitSigmaError;

    // Iterating over each signal to background ratio
    for (const auto& pair : sbRatios.ratios) {
        file << "," << pair.second << "," << sbRatios.errors.at(pair.first);
    }

    file << "," << numEntries << ","
         << signalYield << ","
         << signalError << ","
         << relativeSignalError << ","
         << chi2;
    
    // Output lower and upper bounds for each sigma multiplier
    for (double multiplier : sigmaMultipliers) {
        double lowerBound = fitMean - multiplier * fitSigma;
        double upperBound = fitMean + multiplier * fitSigma;
        file << "," << lowerBound << "," << upperBound;
    }
    file << "\n";
    file.close();
}
/*
 Sets y axis depending on if background is highest or signal is highest
 */
double CalculateDynamicYAxisMax(TH1F* h, double& lineHeight, bool& isBackgroundHigher) {
    // Get the max value in the peak region
    double peakMaxValue = 0;
    for (int i = h->FindBin(0.1); i <= h->FindBin(0.2); ++i) {
        peakMaxValue = std::max(peakMaxValue, h->GetBinContent(i));
    }

    // Get the content at the far right to simulate the background noise level
    double backgroundValue = h->GetBinContent(h->FindBin(0.6));

    // Determine if the background noise level is higher than the signal peak
    isBackgroundHigher = (backgroundValue > peakMaxValue);

    double chosenMax;
    double yAxisMaxBuffer;
    double marginFactor;

    // Use empirical margin factors and buffers to avoid overlapping
    if (isBackgroundHigher) {
        chosenMax = backgroundValue;
        marginFactor = 0.005; //  margin for background
        yAxisMaxBuffer = 1.5; // Buffer to fit under the stats box
    } else {
        chosenMax = peakMaxValue;
        marginFactor = 0.3; //  margin for peak
        yAxisMaxBuffer = 1.5; // Buffer to provide space above TLatex
    }

    lineHeight = chosenMax * (1 + marginFactor);
    
    // Return the y-axis max, scaled with an additional buffer
    return lineHeight * yAxisMaxBuffer;
}

void AnalyzePi0() {
    gROOT->LoadMacro("sPhenixStyle.C");
    SetsPhenixStyle();
    // Open the ROOT file once
    TFile *file = new TFile(globalFilename.c_str(), "READ");
    // Initialize global cut values from the file name
    globalCutValues = parseFileName();

    std::vector<int> indices;
    for (int i = 0; i <= 17; ++i) {
        indices.push_back(i);
    }

    for (int currentIndex : indices) {
        // Fetch histogram based on index
        std::string histName = "hPi0Mass_" + std::to_string(currentIndex);
        TH1F *hPi0Mass = (TH1F*)file->Get(histName.c_str());
        if (!hPi0Mass) {
            continue; // Skip this histogram
        }
        bool isBackgroundHigher;
        double lineHeight;
        double dynamicYAxisMax = CalculateDynamicYAxisMax(hPi0Mass, lineHeight, isBackgroundHigher);

        hPi0Mass->GetYaxis()->SetRangeUser(0, dynamicYAxisMax);
        
        hPi0Mass->SetTitle("Reconstructed Diphotons");
        
        // Declare variables for fit parameters
        TF1 *totalFit;
        
        double fitStart, fitEnd;
        
        // Call PerformFitting to execute the fitting procedure
        TFitResultPtr fitResult = PerformFitting(hPi0Mass, totalFit, fitStart, fitEnd);
    
        
        std::string canvasName = "canvas_" + std::to_string(currentIndex);
        TCanvas *canvas = new TCanvas(canvasName.c_str(), "Pi0 Mass Distribution", 900, 600);
        hPi0Mass->SetMarkerStyle(20);
        hPi0Mass->SetMarkerSize(1.0);
        hPi0Mass->SetMarkerColor(kBlack);
        hPi0Mass->SetStats(0);
        hPi0Mass->Draw("PE");
        
        double fitMean = totalFit->GetParameter(1);
        double fitMeanError = totalFit->GetParError(1);
        double fitSigma = totalFit->GetParameter(2);
        double fitSigmaError = totalFit->GetParError(2);
        double numEntries = hPi0Mass->GetEntries();
        
        
        TF1 *gaussFit = new TF1("gaussFit", "gaus", fitStart, fitEnd);
        TF1 *polyFit = new TF1("polyFit", "pol2", fitStart, fitEnd);
        gaussFit->SetParameter(0, totalFit->GetParameter(0));
        gaussFit->SetParameter(1, totalFit->GetParameter(1));
        gaussFit->SetParameter(2, totalFit->GetParameter(2));
        
        for (int i = 3; i < 6; i++) {
            polyFit->SetParameter(i - 3, totalFit->GetParameter(i));
            polyFit->SetParError(i - 3, totalFit->GetParError(i));
        }
        SignalBackgroundRatio sbRatios = CalculateSignalToBackgroundRatio(hPi0Mass, polyFit, fitMean, fitSigma, fitStart, fitEnd);
        
        gaussFit->SetLineColor(kOrange+2);
        gaussFit->SetLineStyle(2);
        gaussFit->Draw("SAME");
        polyFit->SetLineColor(kBlue);
        polyFit->SetLineWidth(3);
        polyFit->SetLineStyle(2);
        polyFit->Draw("SAME");
        
        TLatex latex;
        latex.SetNDC();
        
        
        double amplitude = totalFit->GetParameter(0);
        
        // Calculate lowerSignalBound and upperSignalBound
        double lowerSignalBound = std::max(fitMean - 2 * fitSigma, fitStart);
        double upperSignalBound = std::min(fitMean + 2 * fitSigma, fitEnd);
   
        double chi2 = fitResult->Chi2(); // to retrieve the fit chi2
        
        double NDF = fitResult->Ndf(); // to retrieve the fit chi2
        
        
        
        TLegend *leg = new TLegend(0.55,.82,0.75,.92);
        leg->SetFillStyle(0);
        leg->AddEntry("","#it{#bf{sPHENIX}} Internal","");
        leg->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
        leg->Draw("same");
        
        double signalError_; // Declare once outside the function call, ensuring it's not redeclared in the local scope where it's used
        double signalYield_ = CalculateSignalYieldAndError(hPi0Mass, polyFit, fitMean, fitSigma, signalError_, fitStart, fitEnd);
        
        
        DrawCanvasText(latex, ranges[currentIndex], fitMean, fitSigma, sbRatios, signalYield_, signalError_, numEntries, chi2, NDF);
        
        // Call the new method to write to CSV if the fit is good
        WriteDataToCSV(currentIndex, globalCutValues, fitMean, fitMeanError, fitSigma, fitSigmaError, sbRatios, signalYield_, signalError_, numEntries, chi2);
        
        TLine *line1 = new TLine(upperSignalBound, 0, upperSignalBound, lineHeight);
        TLine *line2 = new TLine(lowerSignalBound, 0, lowerSignalBound, lineHeight);
        line1->SetLineColor(kBlack);
        line1->SetLineStyle(1);
        line2->SetLineColor(kBlack);
        line2->SetLineStyle(1);
        line1->Draw("same");
        line2->Draw("same");

        if (currentIndex >= 11 && currentIndex <= 11) {
            std::ostringstream dirPathStream;
            dirPathStream << globalPlotOutput // Use the global variable here
                          << "EA" << globalCutValues.clusEA
                          << "_EB" << globalCutValues.clusEB
                          << "_Asym" << globalCutValues.asymmetry
                          << "_DelrMin" << globalCutValues.deltaRMin
                          << "_DelrMax" << globalCutValues.deltaRMax
                          << "_Chi" << globalCutValues.chi;
            std::string dirPath = dirPathStream.str();


            // Now construct the filename
            std::ostringstream pngFilenameStream;
            pngFilenameStream << globalPlotOutput << "/hPi0Mass_"
                              << "Index" << currentIndex
                              << "_fit.png"; // You can adjust this naming scheme
            std::string pngFilename = pngFilenameStream.str();

            // Save the canvas to the constructed filename
            canvas->SaveAs(pngFilename.c_str());
        }
    }
}
