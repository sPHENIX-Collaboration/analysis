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
/*
 when compiling this code, SCROLL TO BOTTOM, see you can either select 'Y' for run code over all hPi0Mass root files in your path, where folders auto generate with the cut value combiantion for each index
--can also select no and use hard coded file path
 */
void RunCode(const std::string& filename); // Forward declaration

struct CutValues {
    float clusEA, clusEB, asymmetry, deltaRMin, deltaRMax, chi;
};

CutValues globalCutValues;

// Global variable
std::string globalDataPath = "/Users/patsfan753/Desktop/Analysis_3_29/CSVoutput/";
std::string globalPlotOutput = "/Users/patsfan753/Desktop/Analysis_3_29/Plots/"; //Note: Folder auto created with name of cut variation to this


int histIndex = 0;

TFitResultPtr PerformFitting(TH1F* hPi0Mass, TF1*& totalFit) {
    double fitStart = 0.1;
    double fitEnd = 0.35;

    double findBin1Value = 0.1; // Value in FindBin for bin1
    double findBin2Value = 0.2; // Value in FindBin for bin2

    double sigmaEstimate = 0.024; // sigmaEstimate value

    
    // Define totalFit
    totalFit = new TF1("totalFit", "gaus(0) + pol2(3)", fitStart, fitEnd);

    int bin1 = hPi0Mass->GetXaxis()->FindBin(findBin1Value);
    int bin2 = hPi0Mass->GetXaxis()->FindBin(findBin2Value);
    int maxBin = bin1;
    double maxBinContent = hPi0Mass->GetBinContent(bin1);
    for (int i = bin1 + 1; i <= bin2; ++i) {
        if (hPi0Mass->GetBinContent(i) > maxBinContent) {
            maxBinContent = hPi0Mass->GetBinContent(i);
            maxBin = i;
        }
    }
    double maxBinCenter = hPi0Mass->GetXaxis()->GetBinCenter(maxBin);
    
    totalFit->SetParameter(0, maxBinContent);
    
    totalFit->SetParameter(1, maxBinCenter);
    
    totalFit->SetParameter(2, sigmaEstimate);

    TFitResultPtr fitResult = hPi0Mass->Fit("totalFit", "SR+");
    return fitResult; // Return the fit result
}
/*
 Function to parse the filename and extract cut values
 */
CutValues parseFileName(const std::string& filename) {
    CutValues cuts = {0, 0, 0, 0}; // Initialize cut values to zero
    
    std::regex re("hPi0Mass_EA([0-9]+(?:point[0-9]*)?)_EB([0-9]+(?:point[0-9]*)?)_Asym([0-9]+(?:point[0-9]*)?)_DelrMin([0-9]+(?:point[0-9]*)?)_DelrMax([0-9]+(?:point[0-9]*)?)_Chi([0-9]+(?:point[0-9]*)?)\\.root");

    std::smatch match; // Object to store the results of regex search

    if (std::regex_search(filename, match, re) && match.size() > 4) {
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
        std::cout << "Matched Filename: " << filename << std::endl;
        for (size_t i = 1; i < match.size(); ++i) {
            std::cout << "Match[" << i << "]: " << match[i].str() << " --> " << convert(match[i].str()) << std::endl;
        }
        cuts.clusEA = convert(match[1].str());
        cuts.clusEB = convert(match[2].str());
        cuts.asymmetry = convert(match[3].str());
        cuts.deltaRMin = convert(match[4].str());
        cuts.deltaRMax = convert(match[5].str());
        cuts.chi = convert(match[6].str());
    } else {
        // If regex does not match, log an error message
        std::cout << "Regex did not match the filename: " << filename << std::endl;
    }

    return cuts;// Return the populated cuts structure
}
bool isFitGood; // No initial value needed here
struct Range {
    double ptLow, ptHigh, mbdLow, mbdHigh;
};
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
 Signal to background ratio calculation and terminal output
 */
double CalculateSignalToBackgroundRatio(TH1F* hPi0Mass, TF1* polyFit, double fitMean, double fitSigma, double& signalToBackgroundError) {
    // Cloning histograms for signal and background
    TH1F *hSignal = (TH1F*)hPi0Mass->Clone("hSignal");
    TH1F *hBackground = (TH1F*)hPi0Mass->Clone("hBackground");

    int firstBinSignal = hPi0Mass->FindBin(fitMean - 2*fitSigma);
    int lastBinSignal = hPi0Mass->FindBin(fitMean + 2*fitSigma);

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

    double signalYield_, signalError_, backgroundYield, backgroundError;
    signalYield_ = hSignal->IntegralAndError(firstBinSignal, lastBinSignal, signalError_, "");
    backgroundYield = hBackground->IntegralAndError(firstBinSignal, lastBinSignal, backgroundError, "");

    double signalToBackgroundRatio = signalYield_ / backgroundYield;

    // Error propagation for division
    signalToBackgroundError = signalToBackgroundRatio * sqrt(pow(signalError_ / signalYield_, 2) + pow(backgroundError / backgroundYield, 2));


    // Cleanup
    delete hSignal;
    delete hBackground;

    return signalToBackgroundRatio;
}
/*
 Calculation for signal yield and error, simiarly to above method outputs to text file upon user input in terminal when happy with fit
 */
double CalculateSignalYieldAndError(TH1F* hPi0Mass, TF1* polyFit, double fitMean, double fitSigma, double& signalError) {
    TH1F *hSignal = (TH1F*)hPi0Mass->Clone("hSignal");
    double binCenter, binContent, bgContent, binError;
    int firstBinSignal = hPi0Mass->FindBin(fitMean - 2*fitSigma);
    int lastBinSignal = hPi0Mass->FindBin(fitMean + 2*fitSigma);
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
 Draws on canvas for invariant mass histograms, no need for manual input automatically updates according to root file and plot generated
 */
void DrawCanvasText(TLatex& latex, const Range& selectedRange, double fitMean, double fitSigma, double signalToBackgroundRatio, double signalToBackgroundRatioError) {
    // Drawing text related to the range and cuts
    std::ostringstream mbdStream, ptStream;
    mbdStream << std::fixed << std::setprecision(0) << "Centrality: " << selectedRange.mbdLow << " - " << selectedRange.mbdHigh << "%";
    ptStream << std::fixed << std::setprecision(2) << selectedRange.ptLow << " #leq Diphoton p_{T} < " << selectedRange.ptHigh << " GeV";

    latex.SetTextSize(0.03);
    latex.DrawLatex(0.13, 0.86, "Cuts (Inclusive):");
    latex.DrawLatex(0.13, 0.82, Form("Asymmetry < %.3f", globalCutValues.asymmetry));
    latex.DrawLatex(0.13, 0.78, Form("#chi^{2} < %.3f", globalCutValues.chi));
//    latex.DrawLatex(0.13, 0.74, Form("Cluster E #geq %.3f GeV", globalCutValues.clusE));
    
    // Updating to display both Cluster EA and EB cuts
    latex.DrawLatex(0.13, 0.74, Form("Cluster E_{A} #geq %.3f GeV", globalCutValues.clusEA));
    latex.DrawLatex(0.13, 0.70, Form("Cluster E_{B} #geq %.3f GeV", globalCutValues.clusEB));
//    latex.DrawLatex(0.13, 0.7, Form("#Delta R #geq %.3f", globalCutValues.deltaR));
    
    // Displaying minimum and maximum Delta R cuts in a single line
    latex.DrawLatex(0.13, 0.66, Form("%.3f < #Delta R #leq %.3f", globalCutValues.deltaRMin, globalCutValues.deltaRMax));
    latex.DrawLatex(0.64, 0.22, mbdStream.str().c_str());
    latex.DrawLatex(0.64, 0.18, ptStream.str().c_str());

    // Drawing text related to Gaussian parameters and S/B ratio
    latex.SetTextSize(0.036);
    latex.DrawLatex(0.43, 0.86, Form("Mean of Gaussian: %.4f GeV", fitMean));
    latex.DrawLatex(0.43, 0.81, Form("Std. Dev. of Gaussian: %.4f GeV", fitSigma));
    latex.DrawLatex(0.43, 0.76, Form("S/B Ratio: %.4f #pm %.4f", signalToBackgroundRatio, signalToBackgroundRatioError));
}
/*
 method to globally keep track of parameters as you go index by index and switch between root files with different cuts
 */
void WriteDataToCSV(int histIndex, const CutValues& cutValues, double fitMean, double fitMeanError, double fitSigma, double fitSigmaError, double signalToBackgroundRatio, double signalToBackgroundError, double signalYield, double signalError, double numEntries, double chi2) {
    // Check if the fit is good before proceeding
    if (!isFitGood) {
        std::cout << "Fit is not good. Skipping CSV write." << std::endl;
        return;
    }

    std::string csv_filename = globalDataPath + "PlotByPlotOutput_3_29.csv";
    std::ifstream checkFile(csv_filename);
    bool fileIsEmpty = checkFile.peek() == std::ifstream::traits_type::eof();
    checkFile.close();

    std::ofstream file(csv_filename, std::ios::app); // Open the file in append mode

    if (!file.is_open()) {
        std::cerr << "Unable to open CSV file for writing." << std::endl;
        return;
    }

    // Write column headers if file is empty
    if (fileIsEmpty) {
        file << "Index,EnergyA,EnergyB,Asymmetry,Chi2,DeltaRMin,DeltaRMax,GaussMean,GaussMeanError,GaussSigma,GaussSigmaError,S/B,S/Berror,NumEntry,Yield,YieldError,RelativeSignalError,chi2,LowerSignalBound,UpperSignalBound\n";
    }

    // Calculate relativeSignalError (ensure we don't divide by zero)
    double relativeSignalError = signalYield != 0 ? signalError / signalYield : 0;

    // Calculate lowerSignalBound and upperSignalBound
    double lowerSignalBound = fitMean - 2 * fitSigma;
    double upperSignalBound = fitMean + 2 * fitSigma;
    
    
    // Updated data writing to include clusEB and deltaRMax
    file << histIndex << ","
         << cutValues.clusEA << ","  // Note: Assuming clusE is now clusEA
         << cutValues.clusEB << ","  // New value
         << cutValues.asymmetry << ","
         << cutValues.chi << ","
         << cutValues.deltaRMin << ","  // Note: Assuming deltaR is now deltaRMin
         << cutValues.deltaRMax << ","  // New value
         << fitMean << ","
         << fitMeanError << ","
         << fitSigma << ","
         << fitSigmaError << ","
         << signalToBackgroundRatio << ","
         << signalToBackgroundError << ","
         << numEntries << ","
         << signalYield << ","
         << signalError << ","
         << relativeSignalError << ","
         << chi2 << ","
         << lowerSignalBound << ","
         << upperSignalBound << "\n";


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
        marginFactor = 0.005; // Tight margin for background
        yAxisMaxBuffer = 1.5; // Buffer to fit under the stats box
    } else {
        chosenMax = peakMaxValue;
        marginFactor = 0.3; // Increased margin for peak
        yAxisMaxBuffer = 1.5; // Buffer to provide space above TLatex
    }

    lineHeight = chosenMax * (1 + marginFactor);
    
    // Return the y-axis max, scaled with an additional buffer
    return lineHeight * yAxisMaxBuffer;
}
// Function to check if a directory exists for plot funneling
bool directoryExists(const std::string& dirPath) {
    struct stat info;
    if(stat(dirPath.c_str(), &info) != 0)
        return false;
    else
        return (info.st_mode & S_IFDIR) != 0;
}

// Function to create a directory
void createDirectory(const std::string& dirPath) {
    const int dir_err = mkdir(dirPath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (dir_err == -1) {
        std::cerr << "Error creating directory!" << std::endl;
        exit(1);
    }
}
bool runForAllFiles = false; // Global flag to indicate running for all files without further input


void RunCode(const std::string& filename) {
    
    // Open a log file for skipped histograms
    std::ofstream logFile("/Users/patsfan753/Desktop/Analysis_3_29/SkippedHistogramsLog.txt", std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "Failed to open log file for writing skipped histogram info." << std::endl;
        return; // Optionally handle more gracefully
    }
    
    char runOption;
    if (!runForAllFiles) { // Only ask if not in run for all files mode
        std::cout << "Run for all indices? (A) or Single Index? (S): ";
        std::cin >> runOption;
    } else {
        runOption = 'A'; // Automatically consider 'A' when running for all files
    }


    std::vector<int> indices;
    bool isBatchRun = false; // Flag to check if it's a batch run

    if (runOption == 'A' || runOption == 'a') {
        isBatchRun = true; // Set flag to true for batch run
        for (int i = 0; i <= 17; ++i) {
            indices.push_back(i);
        }
    } else {
        indices.push_back(histIndex); // Current functionality for a single index
    }

    for (int currentIndex : indices) {
        
        
        TFile *file = new TFile(filename.c_str(), "READ");
        if (!file || file->IsZombie()) {
            std::cerr << "Failed to open file: " << filename << std::endl;
            continue; // Skip this file
        }
        // Initialize global cut values
        globalCutValues = parseFileName(filename);

        char userInput;
        
        // For batch run, set isFitGood to true without asking the user
        if (isBatchRun || runForAllFiles) {
            isFitGood = true;
        } else {
            std::cout << "Is fit ready to be finalized? (Y/N): ";
            std::cin >> userInput;
            isFitGood = (userInput == 'Y' || userInput == 'y'); // Directly setting the global variable
        }

        // Fetch histogram based on index
        std::string histName = "hPi0Mass_" + std::to_string(currentIndex);
        TH1F *hPi0Mass = (TH1F*)file->Get(histName.c_str());
        if (!hPi0Mass) {
            std::cerr << "Histogram not found in file: " << filename << std::endl;
            continue; // Skip this histogram
        }
        if (!hPi0Mass || hPi0Mass->GetEntries() <= 20) {
            std::cerr << "Skipping due to empty or missing histogram: " << histName << " in file: " << filename << std::endl;
            logFile << "Skipped histogram: " << histName << " from file: " << filename << std::endl;
            continue; // Skip this histogram
        }
        bool isBackgroundHigher;
        double lineHeight;
        double dynamicYAxisMax = CalculateDynamicYAxisMax(hPi0Mass, lineHeight, isBackgroundHigher);

        hPi0Mass->GetYaxis()->SetRangeUser(0, dynamicYAxisMax);
        
        hPi0Mass->SetTitle("Reconstructed Diphoton, MB + Central Events");
        
        // Declare variables for fit parameters
        TF1 *totalFit;
        double fitStart, fitEnd;
        
        // Call PerformFitting to execute the fitting procedure
        TFitResultPtr fitResult = PerformFitting(hPi0Mass, totalFit);
        
        std::string canvasName = "canvas_" + std::to_string(currentIndex);
        TCanvas *canvas = new TCanvas(canvasName.c_str(), "Pi0 Mass Distribution", 900, 600);
        hPi0Mass->SetMarkerStyle(20);
        hPi0Mass->SetMarkerSize(1.0);
        hPi0Mass->SetMarkerColor(kBlack);
        hPi0Mass->Draw("PE");
        
        double fitMean = totalFit->GetParameter(1);
        double fitMeanError = totalFit->GetParError(1);
        double fitSigma = totalFit->GetParameter(2);
        double fitSigmaError = totalFit->GetParError(2);
        double numEntries = hPi0Mass->GetEntries();
        
        
        TF1 *gaussFit = new TF1("gaussFit", "gaus", fitStart, .3);
        TF1 *polyFit = new TF1("polyFit", "pol2", 0, .3);
        gaussFit->SetParameter(0, totalFit->GetParameter(0));
        gaussFit->SetParameter(1, totalFit->GetParameter(1));
        gaussFit->SetParameter(2, totalFit->GetParameter(2));
        
        for (int i = 3; i < 6; i++) {
            polyFit->SetParameter(i - 3, totalFit->GetParameter(i));
            polyFit->SetParError(i - 3, totalFit->GetParError(i));
        }
        
        gaussFit->SetLineColor(kOrange+2);
        gaussFit->SetLineStyle(2);
        gaussFit->Draw("SAME");
        polyFit->SetLineColor(kBlue);
        polyFit->SetLineWidth(3);
        polyFit->SetLineStyle(2);
        polyFit->Draw("SAME");
        
        TLatex latex;
        latex.SetNDC();
        
        double signalToBackgroundError;
        double signalToBackgroundRatio = CalculateSignalToBackgroundRatio(hPi0Mass, polyFit, fitMean, fitSigma, signalToBackgroundError);
        
        DrawCanvasText(latex, ranges[currentIndex], fitMean, fitSigma, signalToBackgroundRatio, signalToBackgroundError);
        
        double amplitude = totalFit->GetParameter(0);
        
        // Calculate lowerSignalBound and upperSignalBound
        double lowerSignalBound = fitMean - 2 * fitSigma;
        double upperSignalBound = fitMean + 2 * fitSigma;
        
        double chi2 = fitResult->Chi2(); // to retrieve the fit chi2
        
        // Draw chi2 value on the canvas
        TLatex chi2Text;
        chi2Text.SetNDC(); // Set coordinates to normalized
        chi2Text.SetTextSize(0.03); // Set text size (you can adjust as necessary)
        chi2Text.DrawLatex(0.64, 0.14, Form("Chi2: %.2f", chi2)); // Adjust the position and text as needed
        
        TLine *line1 = new TLine(fitMean + 2*fitSigma, 0, fitMean + 2*fitSigma, lineHeight);
        TLine *line2 = new TLine(fitMean - 2*fitSigma, 0, fitMean - 2*fitSigma, lineHeight);
        line1->SetLineColor(kBlack);
        line1->SetLineStyle(1);
        line2->SetLineColor(kBlack);
        line2->SetLineStyle(1);
        line1->Draw("same");
        line2->Draw("same");

        if (currentIndex >= 0 && currentIndex <= 17) {
            std::ostringstream dirPathStream;
            dirPathStream << globalPlotOutput // Use the global variable here
                          << "EA" << globalCutValues.clusEA
                          << "_EB" << globalCutValues.clusEB
                          << "_Asym" << globalCutValues.asymmetry
                          << "_DelrMin" << globalCutValues.deltaRMin
                          << "_DelrMax" << globalCutValues.deltaRMax
                          << "_Chi" << globalCutValues.chi;
            std::string dirPath = dirPathStream.str();


            // Check if the directory exists, if not, create it
            if (!directoryExists(dirPath)) {
                createDirectory(dirPath);
            }

            // Now construct the filename
            std::ostringstream pngFilenameStream;
            pngFilenameStream << dirPath << "/hPi0Mass_"
                              << "Index" << currentIndex
                              << "_fit.png"; // You can adjust this naming scheme
            std::string pngFilename = pngFilenameStream.str();

            // Save the canvas to the constructed filename
            canvas->SaveAs(pngFilename.c_str());
        }

        
        delete canvas;
        
        double signalError; // Declare once outside the function call, ensuring it's not redeclared in the local scope where it's used
        double signalYield = CalculateSignalYieldAndError(hPi0Mass, polyFit, fitMean, fitSigma, signalError);
        
        
        
        // Call the new method to write to CSV if the fit is good
        WriteDataToCSV(currentIndex, globalCutValues, fitMean, fitMeanError, fitSigma, fitSigmaError, signalToBackgroundRatio, signalToBackgroundError, signalYield, signalError, numEntries, chi2);
        
        // After finishing with the file
        if (file) {
            file->Close();
            delete file; // Also consider deleting the pointer if you're done with the file
        }

        
    }
}
void AnalyzePi0() {
    char runOption;
    std::cout << "Run for all files? (Y/N): ";
    std::cin >> runOption;

    std::string folderPath = "/Users/patsfan753/Desktop/Analysis_3_29/RootFiles";

    
    if (runOption == 'Y' || runOption == 'y') {
        runForAllFiles = true; // Set the flag when running for all files
        TSystemDirectory dir("dir", folderPath.c_str());
        TList* files = dir.GetListOfFiles();
        if (files) {
            TSystemFile* file;
            TString fname;
            TIter next(files);
            std::cout << "Processing files:" << std::endl; // Indicates the start of processing files
            while ((file=(TSystemFile*)next())) {
                fname = file->GetName();
                if (!file->IsDirectory() && fname.EndsWith(".root") && fname.BeginsWith("hPi0Mass")) {
                    TString fullPath = TString(folderPath.c_str()) + "/" + fname;
                    std::cout << "Opening file: " << fullPath << std::endl;
                    TFile *file = TFile::Open(fullPath.Data(), "READ");
                    if (!file || file->IsZombie()) {
                        std::cerr << "Failed to open file or file is corrupted: " << fullPath << std::endl;
                        continue; // Skip this file
                    }
                    isFitGood = true;
                    RunCode(std::string(fullPath.Data())); // Ensure RunCode can accept a TString or std::string

            
                }
            }
        } else {
            std::cerr << "No files found in directory: " << folderPath << std::endl;
        }
    } else {
        runForAllFiles = false; // Ensure the flag is false when not running for all files
        // If 'N', call RunCode with the global filename or ask for a specific file
        RunCode("/Users/patsfan753/Desktop/Analysis_3_29/RootFiles/hPi0Mass_EA1point5_EB1point75_Asym0point5_DelrMin0_DelrMax1_Chi4.root");
    }
}
