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
std::string userPath;
std::string basePath;
std::string globalDataPath;
std::string globalPlotOutput;
std::string globalFilename;
std::string csv_filename;
//std::string userPath = "/Users/patsfan753/Desktop";
// Global variable


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

    double meanEstimate = 0.15;
    
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
    mbdStream << std::fixed << std::setprecision(0) << "#bf{Centrality:} " << selectedRange.mbdLow << " - " << selectedRange.mbdHigh << "%";
    ptStream << std::fixed << std::setprecision(2) << selectedRange.ptLow << " #leq Diphoton p_{T} < " << selectedRange.ptHigh << " GeV";

    latex.SetTextSize(0.043);
    
    latex.DrawLatex(0.2, 0.77, Form("Asymmetry < %.2f", globalCutValues.asymmetry));
    latex.DrawLatex(0.2, 0.71, Form("Cluster E_{core} #geq %.2f GeV", globalCutValues.clusEA));
    latex.DrawLatex(0.2, 0.89, mbdStream.str().c_str());
    latex.DrawLatex(0.2, 0.83, ptStream.str().c_str());

    double ratioFor2Sigma = sbRatios.ratios.at(2.0);
    double errorFor2Sigma = sbRatios.errors.at(2.0);
    
    //change legend depending on background size to not overlap data
    if (currentIndex == 4 || currentIndex == 5) {
        latex.DrawLatex(0.69, 0.37, Form("#mu_{Gaussian} = %.2f GeV", fitMean));
        latex.DrawLatex(0.69, 0.32, Form("#sigma_{Gaussian} = %.2f GeV", fitSigma));

    } else {
        latex.DrawLatex(0.69, 0.27, Form("#mu_{Gaussian} = %.2f GeV", fitMean));
        latex.DrawLatex(0.69, 0.22, Form("#sigma_{Gaussian} = %.2f GeV", fitSigma));
    }
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
        yAxisMaxBuffer = 1.5;
    }

    lineHeight = chosenMax * (1 + marginFactor);
    
    // Return the y-axis max, scaled with an additional buffer
    return lineHeight * yAxisMaxBuffer;
}
struct Data {
    std::vector<double> sb_0_20, sb_20_40, sb_40_60;
    std::vector<double> sb_0_20_Errors, sb_20_40_Errors, sb_40_60_Errors;
    
    std::vector<double> gaussMean_0_20, gaussMean_20_40, gaussMean_40_60;
    std::vector<double> gaussMean_0_20_Errors, gaussMean_20_40_Errors, gaussMean_40_60_Errors;
    
    std::vector<double> gaussSigma_0_20, gaussSigma_20_40, gaussSigma_40_60;
    std::vector<double> gaussSigma_0_20_Errors, gaussSigma_20_40_Errors, gaussSigma_40_60_Errors;
    
    std::vector<double> gaussMeanDividedBySigma_0_20, gaussMeanDividedBySigma_20_40, gaussMeanDividedBySigma_40_60;
    std::vector<double> gaussMeanDividedBySigma_0_20_Errors, gaussMeanDividedBySigma_20_40_Errors, gaussMeanDividedBySigma_40_60_Errors;
    
};

double CalculateError(double mean, double sigma, double meanError, double sigmaError) {
    // Assuming mean and sigma are uncorrelated (no covariance term)
    double value = mean / sigma;
    double fractionalErrorMean = meanError / mean;
    double fractionalErrorSigma = sigmaError / sigma;
    double error = std::abs(value) * std::sqrt(fractionalErrorMean * fractionalErrorMean + fractionalErrorSigma * fractionalErrorSigma);
    return error;
}
//function to read PlotByPlot output after fits are completed
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
        
        double sb = std::stod(rowData.at(17));
        double sbError = std::stod(rowData.at(18));
        
        double gaussMean = std::stod(rowData.at(7));
        double gaussMeanError = std::stod(rowData.at(8));

        double gaussSigma = std::stod(rowData.at(9));
        double gaussSigmaError = std::stod(rowData.at(10));
        
        double value = gaussSigma / gaussMean;
        double error = CalculateError(gaussMean, gaussSigma, gaussMeanError, gaussSigmaError);
        
        int index = std::stoi(rowData[0]);
        if (index >= 0 && index <= 5) {
            data.sb_40_60.push_back(sb);
            data.sb_40_60_Errors.push_back(sbError);
            
            data.gaussMean_40_60.push_back(gaussMean);
            data.gaussMean_40_60_Errors.push_back(gaussMeanError);
            
            data.gaussSigma_40_60.push_back(gaussSigma);
            data.gaussSigma_40_60_Errors.push_back(gaussSigmaError);
            
            data.gaussMeanDividedBySigma_40_60.push_back(value);
            data.gaussMeanDividedBySigma_40_60_Errors.push_back(error);
            
            
        } else if (index >= 6 && index <= 11) {
            data.sb_20_40.push_back(sb);
            data.sb_20_40_Errors.push_back(sbError);
            
            data.gaussMean_20_40.push_back(gaussMean);
            data.gaussMean_20_40_Errors.push_back(gaussMeanError);
            
            data.gaussSigma_20_40.push_back(gaussSigma);
            data.gaussSigma_20_40_Errors.push_back(gaussSigmaError);
            
            data.gaussMeanDividedBySigma_20_40.push_back(value);
            data.gaussMeanDividedBySigma_20_40_Errors.push_back(error);
            
        } else if (index >= 12 && index <= 17) {
            data.sb_0_20.push_back(sb);
            data.sb_0_20_Errors.push_back(sbError);
            
            data.gaussMean_0_20.push_back(gaussMean);
            data.gaussMean_0_20_Errors.push_back(gaussMeanError);
            
            data.gaussSigma_0_20.push_back(gaussSigma);
            data.gaussSigma_0_20_Errors.push_back(gaussSigmaError);
            
            data.gaussMeanDividedBySigma_0_20.push_back(value);
            data.gaussMeanDividedBySigma_0_20_Errors.push_back(error);
            
        }
    }
    file.close();
}
void PrintVectorContents(const std::vector<double>& vec, const std::vector<double>& vecErrors, const std::string& name) {
    std::cout << "\033[1m\033[31m" // Red and bold text
              << "Contents of " << name << " and its Errors:" << "\033[0m" << std::endl; // Reset formatting at the end
    std::cout << std::left << std::setw(20) << "Value"
              << std::left << std::setw(20) << "Error" << std::endl;
    for (size_t i = 0; i < 6; ++i) {
        std::cout << std::left << std::setw(20) << vec[i]
                  << std::left << std::setw(20) << vecErrors[i] << std::endl;
    }
    std::cout << std::endl;
}
// Function to create and return a TGraphErrors pointer
TGraphErrors* CreateGraph(const std::vector<double>& ptCenters, const std::vector<double>& values, const std::vector<double>& errors) {
    return new TGraphErrors(ptCenters.size(), &ptCenters[0], &values[0], nullptr, &errors[0]);
}
void Create_sPHENIX_legend(TCanvas* canvas, float x1, float y1, float x2, float y2, const std::string& centrality, float textSize) {
    if (!canvas) return; // Check if canvas is valid
    canvas->cd(); // Switch to the canvas context
    TLegend *leg = new TLegend(x1, y1, x2, y2);
    leg->SetFillStyle(0);
    leg->SetTextSize(textSize);
    leg->AddEntry("", "#it{#bf{sPHENIX}} Internal", "");
    leg->AddEntry("", "Au+Au #sqrt{s_{NN}} = 200 GeV", "");
    leg->AddEntry("", centrality.c_str(), "");
    leg->Draw("same");
}
struct GraphProperties {
    int markerColor;
    int markerStyle;
    double markerSize;
};
void setGraphProperties(TGraph* graph, int markerColor, int lineColor, float markerSize, int markerStyle) {
    graph->SetMarkerColor(markerColor);
    graph->SetLineColor(lineColor);
    graph->SetMarkerSize(markerSize);
    graph->SetMarkerStyle(markerStyle);
}
//plots Gaussian Mean, Sigma, S/B, and fit Resolution as a fun of pT
void plot_FitParameterOutput(const Data& data1) {
    std::vector<double> ptCenters = {2.25, 2.75, 3.25, 3.75, 4.25, 4.75}; // Mid-points of pT ranges for plotting
    
    PrintVectorContents(data1.sb_0_20, data1.sb_0_20_Errors, "SB 0-20% Data1");
    PrintVectorContents(data1.sb_20_40, data1.sb_20_40_Errors, "SB 20-40% Data1");
    PrintVectorContents(data1.sb_40_60, data1.sb_40_60_Errors, "SB 40-60% Data1");
    
    PrintVectorContents(data1.gaussMean_0_20, data1.gaussMean_0_20_Errors, "Gauss Mean 0-20% Data1");
    PrintVectorContents(data1.gaussMean_20_40, data1.gaussMean_20_40_Errors, "Gauss Mean 20-40% Data1");
    PrintVectorContents(data1.gaussMean_40_60, data1.gaussMean_40_60_Errors, "Gauss Mean 40-60% Data1");
    
    PrintVectorContents(data1.gaussSigma_0_20, data1.gaussSigma_0_20_Errors, "gaussSigma 0-20% Data1");
    PrintVectorContents(data1.gaussSigma_20_40, data1.gaussSigma_20_40_Errors, "gaussSigma 20-40% Data1");
    PrintVectorContents(data1.gaussSigma_40_60, data1.gaussSigma_40_60_Errors, "gaussSigma 40-60% Data1");
    
    PrintVectorContents(data1.gaussMeanDividedBySigma_0_20, data1.gaussMeanDividedBySigma_0_20_Errors, "gaussMeanDividedBySigma 0-20% Data1");
    PrintVectorContents(data1.gaussMeanDividedBySigma_20_40, data1.gaussMeanDividedBySigma_20_40_Errors, "gaussMeanDividedBySigma 20-40% Data1");
    PrintVectorContents(data1.gaussMeanDividedBySigma_40_60, data1.gaussMeanDividedBySigma_40_60_Errors, "gaussMeanDividedBySigma 40-60% Data1");
    
    TGraphErrors* sb_0_20_graph = CreateGraph(ptCenters, data1.sb_0_20, data1.sb_0_20_Errors);
    TGraphErrors* sb_20_40_graph = CreateGraph(ptCenters, data1.sb_20_40, data1.sb_20_40_Errors);
    TGraphErrors* sb_40_60_graph = CreateGraph(ptCenters, data1.sb_40_60, data1.sb_40_60_Errors);

    TGraphErrors* gaussMean_0_20_graph = CreateGraph(ptCenters, data1.gaussMean_0_20, data1.gaussMean_0_20_Errors);
    TGraphErrors* gaussMean_20_40_graph = CreateGraph(ptCenters, data1.gaussMean_20_40, data1.gaussMean_20_40_Errors);
    TGraphErrors* gaussMean_40_60_graph = CreateGraph(ptCenters, data1.gaussMean_40_60, data1.gaussMean_40_60_Errors);
    
    TGraphErrors* gaussSigma_0_20_graph = CreateGraph(ptCenters, data1.gaussSigma_0_20, data1.gaussSigma_0_20_Errors);
    TGraphErrors* gaussSigma_20_40_graph = CreateGraph(ptCenters, data1.gaussSigma_20_40, data1.gaussSigma_20_40_Errors);
    TGraphErrors* gaussSigma_40_60_graph = CreateGraph(ptCenters, data1.gaussSigma_40_60, data1.gaussSigma_40_60_Errors);
    
    TGraphErrors* gaussMeanDividedBySigma_0_20_graph = CreateGraph(ptCenters, data1.gaussMeanDividedBySigma_0_20, data1.gaussMeanDividedBySigma_0_20_Errors);
    TGraphErrors* gaussMeanDividedBySigma_20_40_graph = CreateGraph(ptCenters, data1.gaussMeanDividedBySigma_20_40, data1.gaussMeanDividedBySigma_20_40_Errors);
    TGraphErrors* gaussMeanDividedBySigma_40_60_graph = CreateGraph(ptCenters, data1.gaussMeanDividedBySigma_40_60, data1.gaussMeanDividedBySigma_40_60_Errors);
    
    setGraphProperties(sb_0_20_graph, kBlack, kBlack, 1.0, 20);
    setGraphProperties(sb_20_40_graph, kBlack, kBlack, 1.0, 20);
    setGraphProperties(sb_40_60_graph, kBlack, kBlack, 1.0, 20);
    
    setGraphProperties(gaussMean_0_20_graph, kBlack, kBlack, 1.0, 20);
    setGraphProperties(gaussMean_20_40_graph, kBlack, kBlack, 1.0, 20);
    setGraphProperties(gaussMean_40_60_graph, kBlack, kBlack, 1.0, 20);
    
    setGraphProperties(gaussSigma_0_20_graph, kBlack, kBlack, 1.0, 20);
    setGraphProperties(gaussSigma_20_40_graph, kBlack, kBlack, 1.0, 20);
    setGraphProperties(gaussSigma_40_60_graph, kBlack, kBlack, 1.0, 20);
    
    // Loop over all points in the graph and set the errors to zero
    for (int i = 0; i < gaussMeanDividedBySigma_0_20_graph->GetN(); ++i) {
        gaussMeanDividedBySigma_0_20_graph->SetPointError(i, 0, 0); // Set both x and y errors to 0
    }
    // Repeat the process for the other centrality graphs
    for (int i = 0; i < gaussMeanDividedBySigma_20_40_graph->GetN(); ++i) {
        gaussMeanDividedBySigma_20_40_graph->SetPointError(i, 0, 0);
    }
    for (int i = 0; i < gaussMeanDividedBySigma_40_60_graph->GetN(); ++i) {
        gaussMeanDividedBySigma_40_60_graph->SetPointError(i, 0, 0); // Set errors to 0
    }
    setGraphProperties(gaussMeanDividedBySigma_0_20_graph, kBlack, kBlack, 1.0, 20);
    setGraphProperties(gaussMeanDividedBySigma_20_40_graph, kBlack, kBlack, 1.0, 20);
    setGraphProperties(gaussMeanDividedBySigma_40_60_graph, kBlack, kBlack, 1.0, 20);

    auto createGraph = [&data1](
        TCanvas* canvas,
        TGraphErrors* graph1,
        const std::string& centrality,
        double minYaxis,
        double maxYaxis,
        double ndcX1,
        double ndcY1,
        double ndcX2,
        double ndcY2,
        const std::string& yAxisTitle) {
        graph1->Draw("AP");
        graph1->GetXaxis()->SetTitle("p_{T} [GeV]");
        graph1->GetYaxis()->SetTitle(yAxisTitle.c_str());
        graph1->SetMinimum(minYaxis);
        graph1->SetMaximum(maxYaxis);
        graph1->GetXaxis()->SetLimits(2.0, 5.0);
        Create_sPHENIX_legend(canvas, ndcX1, ndcY1, ndcX2, ndcY2, centrality.c_str(), 0.045);
    };
    
    TCanvas *c_SBratio_0_20 = new TCanvas("c_SBratio_0_20", "#pi^{0} S/B vs #it{p}_{T} 0-20% Centrality", 800, 600);
    createGraph(c_SBratio_0_20, sb_0_20_graph, "0-20% Centrality", 0, 1.2, 0.14,.72,0.34,.92, "S/B"); //use lambda function
    c_SBratio_0_20->SaveAs((globalPlotOutput + "/Overlay_0_20_SB.png").c_str());
    TCanvas *c_SBratio_20_40 = new TCanvas("c_SBratio_20_40", "#pi^{0} S/B vs #it{p}_{T} 0-20% Centrality", 800, 600);
    createGraph(c_SBratio_20_40, sb_20_40_graph, "20-40% Centrality", 0, 2.2, 0.14,.72,0.34,.92, "S/B");
    c_SBratio_20_40->SaveAs((globalPlotOutput + "/Overlay_20_40_SB.png").c_str());
    TCanvas *c_SBratio_40_60 = new TCanvas("c_SBratio_40_60", "#pi^{0} S/B vs #it{p}_{T} 0-20% Centrality", 800, 600);
    createGraph(c_SBratio_40_60, sb_40_60_graph, "40-60% Centrality", 0, 6.5, 0.14,.72,0.34,.92, "S/B");
    c_SBratio_40_60->SaveAs((globalPlotOutput + "/Overlay_40_60_SB.png").c_str());
    TCanvas *c_gaussMean_0_20 = new TCanvas("c_gaussMean_0_20", "#pi^{0} Gaussian Mean vs #it{p}_{T} 0-20% Centrality", 800, 600);
    createGraph(c_gaussMean_0_20, gaussMean_0_20_graph, "0-20% Centrality", 0.14, 0.21, 0.16,.7,0.34,.9, "#mu_{Gauss} [GeV]");
    c_gaussMean_0_20->SaveAs((globalPlotOutput + "/Overlay_0_20_gaussMean.png").c_str());
    TCanvas *c_gaussMean_20_40 = new TCanvas("c_gaussMean_20_40", "#pi^{0} Gaussian Mean vs #it{p}_{T} 0-20% Centrality", 800, 600);
    createGraph(c_gaussMean_20_40, gaussMean_20_40_graph, "20-40% Centrality", 0.14, 0.21, 0.16,.7,0.34,.9, "#mu_{Gauss} [GeV]");
    c_gaussMean_20_40->SaveAs((globalPlotOutput + "/Overlay_20_40_gaussMean.png").c_str());
    TCanvas *c_gaussMean_40_60 = new TCanvas("c_gaussMean_40_60", "#pi^{0} Gaussian Mean vs #it{p}_{T} 0-20% Centrality", 800, 600);
    createGraph(c_gaussMean_40_60, gaussMean_40_60_graph, "40-60% Centrality", 0.14, 0.21, 0.16,.7,0.34,.9, "#mu_{Gauss} [GeV]");
    c_gaussMean_40_60->SaveAs((globalPlotOutput + "/Overlay_40_60_gaussMean.png").c_str());
    TCanvas *c_gaussSigma_0_20 = new TCanvas("c_gaussSigma_0_20", "#pi^{0} S/B vs #it{p}_{T} 0-20% Centrality", 800, 600);
    createGraph(c_gaussSigma_0_20, gaussSigma_0_20_graph, "0-20% Centrality", 0.01, 0.04, 0.14,.72,0.34,.92, "#sigma_{Gauss} [GeV]");
    c_gaussSigma_0_20->SaveAs((globalPlotOutput + "/Overlay_0_20_gaussSigma.png").c_str());
    TCanvas *c_gaussSigma_20_40 = new TCanvas("c_gaussSigma_20_40", "#pi^{0} S/B vs #it{p}_{T} 0-20% Centrality", 800, 600);
    createGraph(c_gaussSigma_20_40, gaussSigma_20_40_graph, "20-40% Centrality", 0.01, 0.04, 0.14,.72,0.34,.92, "#sigma_{Gauss} [GeV]");
    c_gaussSigma_20_40->SaveAs((globalPlotOutput + "/Overlay_20_40_gaussSigma.png").c_str());
    TCanvas *c_gaussSigma_40_60 = new TCanvas("c_gaussSigma_40_60", "#pi^{0} S/B vs #it{p}_{T} 0-20% Centrality", 800, 600);
    createGraph(c_gaussSigma_40_60, gaussSigma_40_60_graph, "40-60% Centrality", 0.01, 0.04, 0.14,.72,0.34,.92, "#sigma_{Gauss} [GeV]");
    c_gaussSigma_40_60->SaveAs((globalPlotOutput + "/Overlay_40_60_gaussSigma.png").c_str());
    TCanvas *c_gaussMeanDividedBySigma_0_20 = new TCanvas("c_gaussMeanDividedBySigma_0_20", "#pi^{0} S/B vs #it{p}_{T} 0-20% Centrality", 800, 600);
    createGraph(c_gaussMeanDividedBySigma_0_20, gaussMeanDividedBySigma_0_20_graph, "0-20% Centrality", 0, 0.3, 0.14,.72,0.34,.92, "Resolution");
    c_gaussMeanDividedBySigma_0_20->SaveAs((globalPlotOutput + "/Overlay_0_20_gaussMeanDividedBySigma.png").c_str());
    TCanvas *c_gaussMeanDividedBySigma_20_40 = new TCanvas("c_gaussMeanDividedBySigma_20_40", "#pi^{0} S/B vs #it{p}_{T} 0-20% Centrality", 800, 600);
    createGraph(c_gaussMeanDividedBySigma_20_40, gaussMeanDividedBySigma_20_40_graph, "20-40% Centrality", 0, 0.3, 0.14,.72,0.34,.92, "Resolution");
    c_gaussMeanDividedBySigma_20_40->SaveAs((globalPlotOutput + "/Overlay_20_40_gaussMeanDividedBySigma.png").c_str());
    TCanvas *c_gaussMeanDividedBySigma_40_60 = new TCanvas("c_gaussMeanDividedBySigma_40_60", "#pi^{0} S/B vs #it{p}_{T} 0-20% Centrality", 800, 600);
    createGraph(c_gaussMeanDividedBySigma_40_60, gaussMeanDividedBySigma_40_60_graph, "40-60% Centrality", 0, 0.3, 0.14,.72,0.34,.92, "Resolution");
    c_gaussMeanDividedBySigma_40_60->SaveAs((globalPlotOutput + "/Overlay_40_60_gaussMeanDividedBySigma.png").c_str());
}
void initializePaths() {
    std::cout << "Please enter the user path: ";
    std::getline(std::cin, userPath);  // Get user input for the path
    
    basePath = userPath + "/p015/InvMass/";
    globalDataPath = basePath + "CSV/";
    globalPlotOutput = basePath + "Plots/";
    globalFilename = basePath + "/hPi0Mass_EA1_EB1_Asym0point5_DelrMin0_DelrMax1_Chi4.root";
    csv_filename = globalDataPath + "PlotByPlotOutput_P015.csv";
}
void AnalyzePi0() {
    initializePaths();
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
        hPi0Mass->GetXaxis()->SetRangeUser(0, 0.5);
        
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
        
        
        
        TLegend *leg = new TLegend(0.55,.8,0.75,.92);
        leg->SetFillStyle(0);
        leg->AddEntry("", "#it{#bf{sPHENIX}} Internal", "");
        leg->AddEntry("", "Au+Au #sqrt{s_{NN}} = 200 GeV", "");
        // Draw the legend on the same plot
        leg->Draw("same");
        
        TLegend *legLines = new TLegend(0.68,.65,0.88,.78);
        // Dummy TLine objects to represent the fits in the legend with specific colors
        TLine *lineFit1 = new TLine();
        TLine *lineFit2 = new TLine();
        TLine *lineFit3 = new TLine();
        lineFit1->SetLineColor(kRed);
        lineFit1->SetLineWidth(2);
        legLines->AddEntry(lineFit1, "Combined Fit", "l");

        lineFit2->SetLineColor(kOrange+2);
        lineFit2->SetLineWidth(2);
        legLines->AddEntry(lineFit2, "Gaussian", "l");

        lineFit3->SetLineColor(kBlue);
        lineFit3->SetLineWidth(2);
        legLines->AddEntry(lineFit3, "Polynomial", "l");
        legLines->Draw("same");
        
        
        
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
    //for fit parameter plots following PlotByPlotOutput
    Data data1;
    Read_DataSet(csv_filename, data1);

    plot_FitParameterOutput(data1);
}
