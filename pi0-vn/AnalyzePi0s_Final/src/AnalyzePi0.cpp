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

void RunCode(const std::string& filename); // Forward declaration

struct CutValues {
    float clusEA, clusEB, asymmetry, deltaRMin, deltaRMax, chi;
};

double globalNumEntries;

// Global variable
std::string globalDataPath = "/Users/patsfan753/Desktop/Analysis_3_29/CSVoutput/";


int histIndex = 0;

std::vector<std::tuple<double, double, double>> globalYAxisRanges = {
    // Format: {y-axis min, y-axis max, lineHeight scale factor}
    {0, 175000, 0.4},  // Index 0
    {0, 54000, 0.35},   // Index 1
    {0, 19000, 0.25},   // Index 2
    {0, 7000, 0.25},    // Index 3
    {0, 2750, 0.2},    // Index 4
    {0, 1200, 0.2},    // Index 5
    
    {0, 1200000, 0.4},  // Index 6
    {0, 375000, 0.4},   // Index 7
    {0, 100000, 0.4},   // Index 8
    {0, 25000, 0.35},    // Index 9
    {0, 8500, 0.3},    // Index 10
    {0, 3500, 0.28},    // Index 11
    
    {0, 85000, 0.5},  // Index 12
    {0, 160000, 0.45},   // Index 13
    {0, 54000, 0.45},   // Index 14
    {0, 24000, 0.4},    // Index 15
    {0, 10000, 0.4},    // Index 16
    {0, 3800, 0.4}     // Index 17
};


double globalYAxisRange[2];  // This will store the current y-axis range
double globalLineHeight;     // This will store the current line height based on histIndex

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


CutValues globalCutValues;
/*
 Function to parse the filename and extract cut values
 */
CutValues parseFileName(const std::string& filename) {
    CutValues cuts = {0, 0, 0, 0}; // Initialize cut values to zero
    
    std::regex re("hPi0Mass_EA([0-9]+(?:point[0-9]*)?)_EB([0-9]+(?:point[0-9]*)?)_Asym([0-9]+(?:point[0-9]*)?)_DelrMin([0-9]+(?:point[0-9]*)?)_DelrMax([0-9]+(?:point[0-9]*)?)_Chi([0-9]+(?:point[0-9]*)?)\\.root");

    // Regular expression to match the filename pattern and extract cut values
    //std::regex re("hPi0Mass_E([0-9]+(?:point[0-9]*)?)_Asym([0-9]+(?:point[0-9]*)?)_Delr([0-9]+(?:point[0-9]*)?)_Chi([0-9]+(?:point[0-9]*)?)\\.root");
    std::smatch match; // Object to store the results of regex search
    
    // Perform a regex (Regular Expression) search on the global filename.
    /*
     Regex is a method used for matching text patterns.
     --- Here, it is used to parse the filename and extract numerical values representing cut values.
     The pattern defined in 're' looks for sequences in the filename that match the expected format (e.g., 'E1point25', 'Asym0point5').
      In the pattern:
      - "hPi0Mass_E" etc., are literal texts to match.
      - "([0-9]+(?:point[0-9]*)?)" is a regex pattern where:
        - "[0-9]+" matches one or more digits.
        - "(?:point[0-9]*)?" is a non-capturing group matching the word 'point' followed by any number of digits, making 'point' optional.
      - "\\root" matches the file extension.
     The 'regex_search' function scans 'filename' to find these patterns.
     If a match is found, the 'match' object holds the extracted values (e.g., numeric parts of 'E', 'Asym', etc.).
     These are then converted into floating-point values representing the cut parameters (e.g., energy cut, asymmetry).
     */
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
/*
 Automatic printing of MBD values onto canvas of invar mass histograms, switches when histIndex is switched, located above the Fitting Function above
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
    latex.DrawLatex(0.13, 0.62, mbdStream.str().c_str());
    latex.DrawLatex(0.13, 0.58, ptStream.str().c_str());

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

void RunCode(const std::string& filename) {
    
    // Open a log file for skipped histograms
    std::ofstream logFile("/Users/patsfan753/Desktop/Analysis_3_29/SkippedHistogramsLog.txt", std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "Failed to open log file for writing skipped histogram info." << std::endl;
        return; // Optionally handle more gracefully
    }
    
    char runOption;
    std::cout << "Run for all indices? (A) or Single Index? (S): ";
    std::cin >> runOption;


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
        // User interaction to set global isFitGood
        char userInput;
        
        // For batch run, set isFitGood to true without asking the user
        if (isBatchRun) {
            isFitGood = true;
        } else {
            std::cout << "Is fit ready to be finalized? (Y/N): ";
            std::cin >> userInput;
            isFitGood = (userInput == 'Y' || userInput == 'y'); // Directly setting the global variable
        }
                    
        // Validate histIndex is within the range of defined y-axis ranges
        if (currentIndex < 0 || currentIndex >= globalYAxisRanges.size()) {
            std::cerr << "Error: histIndex is out of range. Valid index range is 0 to "
            << globalYAxisRanges.size() - 1 << "." << std::endl;
            return; // Exit the function or handle error as appropriate
        }
        
        // Extracting y-axis range and scale factor for the current histIndex
        auto selectedRange = globalYAxisRanges[currentIndex];
        globalYAxisRange[0] = std::get<0>(selectedRange); // y-axis min, though always 0 in your setup
        globalYAxisRange[1] = std::get<1>(selectedRange); // y-axis max
        double scaleFactor = std::get<2>(selectedRange);  // scale factor for lineHeight
        
        // Calculating globalLineHeight based on the extracted scale factor
        globalLineHeight = scaleFactor * globalYAxisRange[1];
        
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
        
        hPi0Mass->GetYaxis()->SetRangeUser(globalYAxisRange[0], globalYAxisRange[1]); // Use global variable for Y-axis range
        
        
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
        chi2Text.SetTextSize(0.05); // Set text size (you can adjust as necessary)
        chi2Text.DrawLatex(0.55, 0.2, Form("Chi2/NDF: %.2f / %d", chi2, fitResult->Ndf())); // Adjust the position and text as needed

        
        // ANSI escape code for bold red text
        const char* redBold = "\033[1;31m";
        // ANSI escape code to reset formatting
        const char* reset = "\033[0m";
        
        // Printing the calculated values in bold red
        std::cout << redBold;
        std::cout << "lowerSignalBound: " << lowerSignalBound << std::endl;
        std::cout << "upperSignalBound: " << upperSignalBound << std::endl;
        std::cout << "Chi2: " << chi2 << reset << std::endl;
        
        TLine *line1 = new TLine(fitMean + 2*fitSigma, 0, fitMean + 2*fitSigma, amplitude+globalLineHeight);
        TLine *line2 = new TLine(fitMean - 2*fitSigma, 0, fitMean - 2*fitSigma, amplitude+globalLineHeight);
        line1->SetLineColor(kBlack);
        line1->SetLineStyle(1);
        line2->SetLineColor(kBlack);
        line2->SetLineStyle(1);
        line1->Draw("same");
        line2->Draw("same");

        // Check if currentIndex is within the desired range before saving as PNG
        if (currentIndex >= 12 && currentIndex <= 17) {
            // Constructing the filename for the PNG file dynamically
            std::ostringstream pngFilenameStream;
            pngFilenameStream << "/Users/patsfan753/Desktop/Analysis_3_29/Plots/LabeledByCuts_ByHighestS_Boutput/EA1point5_EB1point75/"
                              << "hPi0Mass_"
                              << "EA" << globalCutValues.clusEA
                              << "_EB" << globalCutValues.clusEB
                              << "_Asym" << globalCutValues.asymmetry
                              << "_DelrMin" << globalCutValues.deltaRMin
                              << "_DelrMax" << globalCutValues.deltaRMax
                              << "_Chi" << globalCutValues.chi
                              << "_Index" << currentIndex // Adding the current index to the filename
                              << "_fit.png"; // Extension for PNG files
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
        // If 'N', call RunCode with the global filename or ask for a specific file
        RunCode("/Users/patsfan753/Desktop/Analysis_3_29/RootFiles/hPi0Mass_EA1point5_EB1point75_Asym0point5_DelrMin0_DelrMax1_Chi4.root");
    }
}
