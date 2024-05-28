#include "../utils/sPhenixStyle.h"
#include "../utils/sPhenixStyle.C"
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
std::string basePath = "/Users/patsfan753/Desktop/p015/MonteCarlo/Integrated_OverPT/p015/InvMass/";
std::string globalDataPath = basePath + "CSV/";
std::string globalPlotOutput = basePath + "Plots/";
std::string globalFilename = basePath + "hPi0Mass_EA1_EB1_Asym0point5_DelrMin0_DelrMax1_Chi4.root";
std::string csv_filename = globalDataPath + "PlotByPlotOutput_P015_IntegratedOverPT.csv";

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

    double meanEstimate = 0.2;
    
    double amplitudeEstimate = hPi0Mass->GetBinContent(hPi0Mass->GetXaxis()->FindBin(meanEstimate));
    totalFit->SetParameter(0, amplitudeEstimate);
    
    totalFit->SetParameter(1, meanEstimate);
    
    totalFit->SetParameter(2, sigmaEstimate);

    TFitResultPtr fitResult = hPi0Mass->Fit("totalFit", "SR+");
    return fitResult; // Return the fit result
}
/*
 Function to parse the filename and extract cut values to be propagated throughout macro for automation of cut variation analysis--applies use of regular expressions (regex)
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
    {2.0, 5.0, 50, 60},       // index 0
    {2.0, 5.0, 40, 50},       // index 1
    {2.0, 5.0, 30, 40},        // index 2
    {2.0, 5.0, 20, 30},        // index 3
    {2.0, 5.0, 10, 20},       // index 4
    {2.0, 5.0, 0, 10},       // index 5
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
        
        sbRatios.ratios[multiplier] = signalToBackgroundRatio;
        sbRatios.errors[multiplier] = signalToBackgroundError;
        delete hSignal;
        delete hBackground;
        
        
    }
    std::cout << "\033[1;31mExiting CalculateSignalToBackgroundRatio\033[0m" << std::endl;
    return sbRatios;
}
/*
 Calculation for signal yield and error
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
void DrawCanvasText(TLatex& latex, const Range& selectedRange, double fitMean, double fitSigma, const SignalBackgroundRatio& sbRatios, double signalYield, double signalYieldError, double numEntries, double chi2, double NDF, int currentIndex) {
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
    
    //change legend depending on background size to not overlap data
//    if (currentIndex == 5) {
//        latex.DrawLatex(0.45, 0.54, Form("#mu_{Gaussian} = %.2f GeV", fitMean));
//        latex.DrawLatex(0.45, 0.49, Form("#sigma_{Gaussian} = %.2f GeV", fitSigma));
//        latex.DrawLatex(0.67, 0.57, Form("S/B = %.2f", ratioFor2Sigma));
//        latex.DrawLatex(0.67, 0.52, Form("Signal Yield = %.2f", signalYield));
//        latex.DrawLatex(0.67, .47, Form("Fit #chi^{2}/NDF: %.2f/%.0f", chi2, NDF));
//    } else {
        latex.DrawLatex(0.67, 0.4, Form("#mu_{Gaussian} = %.2f GeV", fitMean));
        latex.DrawLatex(0.67, 0.35, Form("#sigma_{Gaussian} = %.2f GeV", fitSigma));
        latex.DrawLatex(0.67, 0.3, Form("S/B = %.2f", ratioFor2Sigma));
        latex.DrawLatex(0.67, 0.25, Form("Signal Yield = %.2f", signalYield));
        latex.DrawLatex(0.67, .2, Form("Fit #chi^{2}/NDF: %.2f/%.0f", chi2, NDF));
//    }
    
//    latex.DrawLatex(0.67, 0.4, Form("#mu_{Gaussian} = %.2f GeV", fitMean));
//    latex.DrawLatex(0.67, 0.35, Form("#sigma_{Gaussian} = %.2f GeV", fitSigma));
//    latex.DrawLatex(0.67, 0.3, Form("S/B = %.2f", ratioFor2Sigma));
//    latex.DrawLatex(0.67, 0.25, Form("Signal Yield = %.2f", signalYield));
//    latex.DrawLatex(0.67, .2, Form("Fit #chi^{2}/NDF: %.2f/%.0f", chi2, NDF));
}
/*
Track Relevant Output in CSV tracking via indices and unique cut combinations
 */
void WriteDataToCSV(int histIndex, const CutValues& cutValues, double fitMean, double fitMeanError, double fitSigma, double fitSigmaError, const SignalBackgroundRatio& sbRatios, double signalYield, double signalError, double numEntries, double chi2) {

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
struct Data {
    std::vector<double> fitMean_0_10, fitMean_0_10_Errors;
    std::vector<double> fitMean_10_20, fitMean_10_20_Errors;
    std::vector<double> fitMean_20_30, fitMean_20_30_Errors;
    std::vector<double> fitMean_30_40, fitMean_30_40_Errors;
    std::vector<double> fitMean_40_50, fitMean_40_50_Errors;
    std::vector<double> fitMean_50_60, fitMean_50_60_Errors;
    
    std::vector<double> fitSigma_0_10, fitSigma_0_10_Errors;
    std::vector<double> fitSigma_10_20, fitSigma_10_20_Errors;
    std::vector<double> fitSigma_20_30, fitSigma_20_30_Errors;
    std::vector<double> fitSigma_30_40, fitSigma_30_40_Errors;
    std::vector<double> fitSigma_40_50, fitSigma_40_50_Errors;
    std::vector<double> fitSigma_50_60, fitSigma_50_60_Errors;
    
    std::vector<double> SBratio_0_10, SBratio_0_10_Errors;
    std::vector<double> SBratio_10_20, SBratio_10_20_Errors;
    std::vector<double> SBratio_20_30, SBratio_20_30_Errors;
    std::vector<double> SBratio_30_40, SBratio_30_40_Errors;
    std::vector<double> SBratio_40_50, SBratio_40_50_Errors;
    std::vector<double> SBratio_50_60, SBratio_50_60_Errors;
    
};
void Read_DataSet(const std::string& filePath, Data& data) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return;
    }
    std::string line;
    std::getline(file, line); // Skip the header line

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::vector<std::string> rowData;
        std::string cell;

        while (std::getline(ss, cell, ',')) {
            rowData.push_back(cell);
        }
        
        double fitMean = std::stod(rowData.at(7));
        double fitMean_Error = std::stod(rowData.at(8));
        
        double fitSigma = std::stod(rowData.at(9));
        double fitSigma_Error = std::stod(rowData.at(10));
        
        double SBratio = std::stod(rowData.at(17));
        double SBratio_Error = std::stod(rowData.at(18));

        
    
        int index = std::stoi(rowData[0]); //index of analysis bin, first column of CSV
        if (index == 0) { //index 0 50-60% Centrality
            data.fitMean_50_60.push_back(fitMean);
            data.fitMean_50_60_Errors.push_back(fitMean_Error);
            
            data.fitSigma_50_60.push_back(fitSigma);
            data.fitSigma_50_60_Errors.push_back(fitSigma_Error);
            
            data.SBratio_50_60.push_back(SBratio);
            data.SBratio_50_60_Errors.push_back(SBratio_Error);


        } else if (index == 1) { //index 1 is 40-50
            data.fitMean_40_50.push_back(fitMean);
            data.fitMean_40_50_Errors.push_back(fitMean_Error);
            
            data.fitSigma_40_50.push_back(fitSigma);
            data.fitSigma_40_50_Errors.push_back(fitSigma_Error);
            
            data.SBratio_40_50.push_back(SBratio);
            data.SBratio_40_50_Errors.push_back(SBratio_Error);
            

        } else if (index == 2) { //index 2 is 30-40
            data.fitMean_30_40.push_back(fitMean);
            data.fitMean_30_40_Errors.push_back(fitMean_Error);
            
            data.fitSigma_30_40.push_back(fitSigma);
            data.fitSigma_30_40_Errors.push_back(fitSigma_Error);
            
            data.SBratio_30_40.push_back(SBratio);
            data.SBratio_30_40_Errors.push_back(SBratio_Error);
            
        } else if (index == 3) { //index 3 is 20-30
            data.fitMean_20_30.push_back(fitMean);
            data.fitMean_20_30_Errors.push_back(fitMean_Error);
            
            data.fitSigma_20_30.push_back(fitSigma);
            data.fitSigma_20_30_Errors.push_back(fitSigma_Error);
            
            data.SBratio_20_30.push_back(SBratio);
            data.SBratio_20_30_Errors.push_back(SBratio_Error);
            
        } else if (index == 4) { //index 4 is 10-20
            data.fitMean_10_20.push_back(fitMean);
            data.fitMean_10_20_Errors.push_back(fitMean_Error);
            
            data.fitSigma_10_20.push_back(fitSigma);
            data.fitSigma_10_20_Errors.push_back(fitSigma_Error);
            
            data.SBratio_10_20.push_back(SBratio);
            data.SBratio_10_20_Errors.push_back(SBratio_Error);

        } else if (index == 5) { //index 5 is 0-10
            data.fitMean_0_10.push_back(fitMean);
            data.fitMean_0_10_Errors.push_back(fitMean_Error);
            
            data.fitSigma_0_10.push_back(fitSigma);
            data.fitSigma_0_10_Errors.push_back(fitSigma_Error);
            
            data.SBratio_0_10.push_back(SBratio);
            data.SBratio_0_10_Errors.push_back(SBratio_Error);
        }
    }
    file.close();
}
std::vector<double> centralityCenters = {5, 15, 25, 35, 45, 55}; // Mid-points of centrality ranges for plots

void plotMean(Data& data1, Data& data2) {
    // Prepare the graph
    TGraphErrors* graph_1 = new TGraphErrors();
    int pointIndex = 0;

    // Initialize values and errors vector lists
    std::vector<std::vector<double>*> values_1;
    values_1.push_back(&data1.fitMean_0_10);
    values_1.push_back(&data1.fitMean_10_20);
    values_1.push_back(&data1.fitMean_20_30);
    values_1.push_back(&data1.fitMean_30_40);
    values_1.push_back(&data1.fitMean_40_50);
    values_1.push_back(&data1.fitMean_50_60);

    std::vector<std::vector<double>*> errors_1;
    errors_1.push_back(&data1.fitMean_0_10_Errors);
    errors_1.push_back(&data1.fitMean_10_20_Errors);
    errors_1.push_back(&data1.fitMean_20_30_Errors);
    errors_1.push_back(&data1.fitMean_30_40_Errors);
    errors_1.push_back(&data1.fitMean_40_50_Errors);
    errors_1.push_back(&data1.fitMean_50_60_Errors);

    
    TGraphErrors* graph_2 = new TGraphErrors();
    // Initialize values and errors vector lists
    std::vector<std::vector<double>*> values_2;
    values_2.push_back(&data2.fitMean_0_10);
    values_2.push_back(&data2.fitMean_10_20);
    values_2.push_back(&data2.fitMean_20_30);
    values_2.push_back(&data2.fitMean_30_40);
    values_2.push_back(&data2.fitMean_40_50);
    values_2.push_back(&data2.fitMean_50_60);

    std::vector<std::vector<double>*> errors_2;
    errors_2.push_back(&data2.fitMean_0_10_Errors);
    errors_2.push_back(&data2.fitMean_10_20_Errors);
    errors_2.push_back(&data2.fitMean_20_30_Errors);
    errors_2.push_back(&data2.fitMean_30_40_Errors);
    errors_2.push_back(&data2.fitMean_40_50_Errors);
    errors_2.push_back(&data2.fitMean_50_60_Errors);
    
    double minY = 0, maxY = 0;
    bool firstPoint = true;
    // Loop over each centrality range
    for (size_t i = 0; i < centralityCenters.size(); ++i) {
        for (size_t j = 0; j < values_1[i]->size(); ++j) {
            double y = (*values_1[i])[j];
            double error = (*errors_1[i])[j];
            graph_1->SetPoint(pointIndex, centralityCenters[i] - 0.8, y);
            graph_1->SetPointError(pointIndex, 0, error);
            
            
            double y2 = (*values_2[i])[j];
            double error2 = (*errors_2[i])[j];
            
            graph_2->SetPoint(pointIndex, centralityCenters[i] + 0.8, y2);
            graph_2->SetPointError(pointIndex, 0, error2);

            if (firstPoint) {
                minY = y - error;
                maxY = y + error;
                firstPoint = false;
            } else {
                if (y - error < minY) minY = y - error;
                if (y + error > maxY) maxY = y + error;
                if (y2 - error2 < minY) minY = y2 - error2;
                if (y2 + error2 > maxY) maxY = y2 + error2;
            }
            
            pointIndex++;
        }
    }
    double range = maxY - minY;
    double buffer = range * 0.15; // 15% buffer on each side

    minY -= buffer;
    maxY += buffer;
    
    graph_1->GetYaxis()->SetRangeUser(0, 0.2);

    // Graph properties
    graph_1->SetMarkerStyle(20);
    graph_1->SetMarkerColor(kBlue);
    graph_1->SetLineColor(kBlue);
    
    graph_1->GetXaxis()->SetLimits(0.0, 60.0);
    
    graph_2->SetMarkerStyle(20);
    graph_2->SetMarkerColor(kRed);
    graph_2->SetLineColor(kRed);

    
    graph_1->GetYaxis()->SetTitle("#mu_{Gauss} [GeV]");
    graph_1->GetXaxis()->SetTitle("Centrality [%]");
    
//    graph_1->GetYaxis()->SetRangeUser(0.14, .2);
    
    // Create canvas
    TCanvas* c_1GaussMean = new TCanvas("c_1GaussMean", "Centrality Plot", 900, 600);
    graph_1->Draw("AP");
    graph_2->Draw("P SAME");
    c_1GaussMean->Update();
    graph_1->GetXaxis()->SetRangeUser(0, 70);
    
    TLegend *legend = new TLegend(0.2, 0.18, 0.3, 0.33);
    legend->SetBorderSize(0);
    legend->SetTextSize(0.028);
    legend->AddEntry(graph_1, "Data", "pe");
    legend->AddEntry(graph_2, "Hijing", "pe");

    legend->Draw("SAME");
    
    c_1GaussMean->Update();

    // Save the canvas
    c_1GaussMean->SaveAs("/Users/patsfan753/Desktop/p015/MonteCarlo/Integrated_OverPT/p015/InvMass/Plots/GaussMean.png");
}
void plotSigma(Data& data1, Data& data2) {
    // Prepare the graph
    TGraphErrors* graph_1 = new TGraphErrors();
    int pointIndex = 0;

    // Initialize values and errors vector lists
    std::vector<std::vector<double>*> values_1;
    values_1.push_back(&data1.fitSigma_0_10);
    values_1.push_back(&data1.fitSigma_10_20);
    values_1.push_back(&data1.fitSigma_20_30);
    values_1.push_back(&data1.fitSigma_30_40);
    values_1.push_back(&data1.fitSigma_40_50);
    values_1.push_back(&data1.fitSigma_50_60);

    std::vector<std::vector<double>*> errors_1;
    errors_1.push_back(&data1.fitSigma_0_10_Errors);
    errors_1.push_back(&data1.fitSigma_10_20_Errors);
    errors_1.push_back(&data1.fitSigma_20_30_Errors);
    errors_1.push_back(&data1.fitSigma_30_40_Errors);
    errors_1.push_back(&data1.fitSigma_40_50_Errors);
    errors_1.push_back(&data1.fitSigma_50_60_Errors);

    
    TGraphErrors* graph_2 = new TGraphErrors();
    // Initialize values and errors vector lists
    std::vector<std::vector<double>*> values_2;
    values_2.push_back(&data2.fitSigma_0_10);
    values_2.push_back(&data2.fitSigma_10_20);
    values_2.push_back(&data2.fitSigma_20_30);
    values_2.push_back(&data2.fitSigma_30_40);
    values_2.push_back(&data2.fitSigma_40_50);
    values_2.push_back(&data2.fitSigma_50_60);

    std::vector<std::vector<double>*> errors_2;
    errors_2.push_back(&data2.fitSigma_0_10_Errors);
    errors_2.push_back(&data2.fitSigma_10_20_Errors);
    errors_2.push_back(&data2.fitSigma_20_30_Errors);
    errors_2.push_back(&data2.fitSigma_30_40_Errors);
    errors_2.push_back(&data2.fitSigma_40_50_Errors);
    errors_2.push_back(&data2.fitSigma_50_60_Errors);
    
    double minY = 0, maxY = 0;
    bool firstPoint = true;
    // Loop over each centrality range
    for (size_t i = 0; i < centralityCenters.size(); ++i) {
        for (size_t j = 0; j < values_1[i]->size(); ++j) {
            double y = (*values_1[i])[j];
            double error = (*errors_1[i])[j];
            graph_1->SetPoint(pointIndex, centralityCenters[i] - 0.8, y);
            graph_1->SetPointError(pointIndex, 0, error);
            
            
            double y2 = (*values_2[i])[j];
            double error2 = (*errors_2[i])[j];
            
            graph_2->SetPoint(pointIndex, centralityCenters[i] + 0.8, y2);
            graph_2->SetPointError(pointIndex, 0, error2);

            if (firstPoint) {
                minY = y - error;
                maxY = y + error;
                firstPoint = false;
            } else {
                if (y - error < minY) minY = y - error;
                if (y + error > maxY) maxY = y + error;
                if (y2 - error2 < minY) minY = y2 - error2;
                if (y2 + error2 > maxY) maxY = y2 + error2;
            }
            
            pointIndex++;
        }
    }
    double range = maxY - minY;
    double buffer = range * 0.4; // 15% buffer on each side

    minY -= buffer;
    maxY += buffer;
    
    graph_1->GetYaxis()->SetRangeUser(0, 0.03);

    // Graph properties
    graph_1->SetMarkerStyle(20);
    graph_1->SetMarkerColor(kBlue);
    graph_1->SetLineColor(kBlue);
    
    graph_1->GetXaxis()->SetLimits(0.0, 60.0);
    
    graph_2->SetMarkerStyle(20);
    graph_2->SetMarkerColor(kRed);
    graph_2->SetLineColor(kRed);

    
    graph_1->GetYaxis()->SetTitle("#sigma_{Gauss} [GeV]");
    graph_1->GetXaxis()->SetTitle("Centrality [%]");
    
//    graph_1->GetYaxis()->SetRangeUser(0.14, .2);
    
    // Create canvas
    TCanvas* c_1GaussSigma = new TCanvas("c_1GaussSigma", "Centrality Plot", 900, 600);
    graph_1->Draw("AP");
    graph_2->Draw("P SAME");
    c_1GaussSigma->Update();
    graph_1->GetXaxis()->SetRangeUser(0, 70);
    
    TLegend *legend = new TLegend(0.2, 0.18, 0.3, 0.33);
    legend->SetBorderSize(0);
    legend->SetTextSize(0.028);
    legend->AddEntry(graph_1, "Data", "pe");
    legend->AddEntry(graph_2, "Hijing", "pe");

    legend->Draw("SAME");
    
    c_1GaussSigma->Update();

    // Save the canvas
    c_1GaussSigma->SaveAs("/Users/patsfan753/Desktop/p015/MonteCarlo/Integrated_OverPT/p015/InvMass/Plots/GaussSigma.png");
}

void plotSB(Data& data1, Data& data2) {
    // Prepare the graph
    TGraphErrors* graph_1 = new TGraphErrors();
    int pointIndex = 0;

    // Initialize values and errors vector lists
    std::vector<std::vector<double>*> values_1;
    values_1.push_back(&data1.SBratio_0_10);
    values_1.push_back(&data1.SBratio_10_20);
    values_1.push_back(&data1.SBratio_20_30);
    values_1.push_back(&data1.SBratio_30_40);
    values_1.push_back(&data1.SBratio_40_50);
    values_1.push_back(&data1.SBratio_50_60);

    std::vector<std::vector<double>*> errors_1;
    errors_1.push_back(&data1.SBratio_0_10_Errors);
    errors_1.push_back(&data1.SBratio_10_20_Errors);
    errors_1.push_back(&data1.SBratio_20_30_Errors);
    errors_1.push_back(&data1.SBratio_30_40_Errors);
    errors_1.push_back(&data1.SBratio_40_50_Errors);
    errors_1.push_back(&data1.SBratio_50_60_Errors);

    
    TGraphErrors* graph_2 = new TGraphErrors();
    // Initialize values and errors vector lists
    std::vector<std::vector<double>*> values_2;
    values_2.push_back(&data2.SBratio_0_10);
    values_2.push_back(&data2.SBratio_10_20);
    values_2.push_back(&data2.SBratio_20_30);
    values_2.push_back(&data2.SBratio_30_40);
    values_2.push_back(&data2.SBratio_40_50);
    values_2.push_back(&data2.SBratio_50_60);

    std::vector<std::vector<double>*> errors_2;
    errors_2.push_back(&data2.SBratio_0_10_Errors);
    errors_2.push_back(&data2.SBratio_10_20_Errors);
    errors_2.push_back(&data2.SBratio_20_30_Errors);
    errors_2.push_back(&data2.SBratio_30_40_Errors);
    errors_2.push_back(&data2.SBratio_40_50_Errors);
    errors_2.push_back(&data2.SBratio_50_60_Errors);
    
    double minY = 0, maxY = 0;
    bool firstPoint = true;
    // Loop over each centrality range
    for (size_t i = 0; i < centralityCenters.size(); ++i) {
        for (size_t j = 0; j < values_1[i]->size(); ++j) {
            double y = (*values_1[i])[j];
            double error = (*errors_1[i])[j];
            graph_1->SetPoint(pointIndex, centralityCenters[i] - 0.8, y);
            graph_1->SetPointError(pointIndex, 0, error);
            
            
            double y2 = (*values_2[i])[j];
            double error2 = (*errors_2[i])[j];
            
            graph_2->SetPoint(pointIndex, centralityCenters[i] + 0.8, y2);
            graph_2->SetPointError(pointIndex, 0, error2);

            if (firstPoint) {
                minY = y - error;
                maxY = y + error;
                firstPoint = false;
            } else {
                if (y - error < minY) minY = y - error;
                if (y + error > maxY) maxY = y + error;
                if (y2 - error2 < minY) minY = y2 - error2;
                if (y2 + error2 > maxY) maxY = y2 + error2;
            }
            
            pointIndex++;
        }
    }
    double range = maxY - minY;
    double buffer = range * 0.4; // 15% buffer on each side

    minY -= buffer;
    maxY += buffer;
    
    graph_1->GetYaxis()->SetRangeUser(0, 1.3);

    // Graph properties
    graph_1->SetMarkerStyle(20);
    graph_1->SetMarkerColor(kBlue);
    graph_1->SetLineColor(kBlue);
    
    graph_1->GetXaxis()->SetLimits(0.0, 60.0);
    
    graph_2->SetMarkerStyle(20);
    graph_2->SetMarkerColor(kRed);
    graph_2->SetLineColor(kRed);

    
    graph_1->GetYaxis()->SetTitle("S/B");
    graph_1->GetXaxis()->SetTitle("Centrality [%]");
    
//    graph_1->GetYaxis()->SetRangeUser(0.14, .2);
    
    // Create canvas
    TCanvas* c_1GaussSigma = new TCanvas("c_1GaussSigma", "Centrality Plot", 900, 600);
    graph_1->Draw("AP");
    graph_2->Draw("P SAME");
    c_1GaussSigma->Update();
    graph_1->GetXaxis()->SetRangeUser(0, 70);
    
    TLegend *legend = new TLegend(0.2, 0.75, 0.3, 0.9);
    legend->SetBorderSize(0);
    legend->SetTextSize(0.028);
    legend->AddEntry(graph_1, "Data", "pe");
    legend->AddEntry(graph_2, "Hijing", "pe");

    legend->Draw("SAME");
    
    c_1GaussSigma->Update();

    // Save the canvas
    c_1GaussSigma->SaveAs("/Users/patsfan753/Desktop/p015/MonteCarlo/Integrated_OverPT/p015/InvMass/Plots/SBratio.png");
}
void AnalyzePi0_IntegratedOverPT() {
    gROOT->LoadMacro("../utils/sPhenixStyle.C");
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
        
        
        DrawCanvasText(latex, ranges[currentIndex], fitMean, fitSigma, sbRatios, signalYield_, signalError_, numEntries, chi2, NDF, currentIndex);
        
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
    Data plotByplot, simulationPlotByPlot;
    std::string PlotByPlotPath = "/Users/patsfan753/Desktop/p015/IntegratedOverPt/InvMass/CSV/PlotByPlotOutput_P015_IntegratedOverPT.csv";
    std::string PlotByPlotPath_Simulation = "/Users/patsfan753/Desktop/p015/MonteCarlo/Integrated_OverPT/p015/InvMass/CSV/PlotByPlotOutput_P015_IntegratedOverPT.csv";
    Read_DataSet(PlotByPlotPath, plotByplot);
    Read_DataSet(PlotByPlotPath_Simulation, simulationPlotByPlot);
    plotMean(plotByplot, simulationPlotByPlot);
    plotSigma(plotByplot, simulationPlotByPlot);
    plotSB(plotByplot, simulationPlotByPlot);
}

