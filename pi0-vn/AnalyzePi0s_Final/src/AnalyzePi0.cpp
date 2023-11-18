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

struct CutValues {
    float deltaR;
    float asymmetry;
    float chi;
    float clusE;
};
// Global variables
std::string globalFilename = "/Users/patsfan753/Desktop/AnalyzePi0s_Final/histRootFiles/hPi0Mass_E1point25_Asym0point5_Delr0point075_Chi4.root";
CutValues globalCutValues;
CutValues parseFileName() {
    CutValues cuts = {0, 0, 0, 0};
    // Adjusted regex to match the file name format correctly
    std::regex re("hPi0Mass_E([0-9]+(?:point[0-9]*)?)_Asym([0-9]+(?:point[0-9]*)?)_Delr([0-9]+(?:point[0-9]*)?)_Chi([0-9]+(?:point[0-9]*)?)\\.root");
    std::smatch match;

    if (std::regex_search(globalFilename, match, re) && match.size() > 4) {
        auto convert = [](const std::string& input) -> float {
            std::string temp = input;
            size_t pointPos = temp.find("point");
            if (pointPos != std::string::npos) {
                temp.replace(pointPos, 5, ".");
            }
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

        cuts.clusE = convert(match[1].str());
        cuts.asymmetry = convert(match[2].str());
        cuts.deltaR = convert(match[3].str());
        cuts.chi = convert(match[4].str());
    } else {
        std::cout << "Regex did not match the filename: " << globalFilename << std::endl;
    }

    return cuts;
}


bool CreateSignalYieldAndErrorPlot = false; //control flag for plotting signal and signal error
bool isFitGood; // No initial value needed here
struct Range {
    double ptLow, ptHigh, mbdLow, mbdHigh;
};
Range ranges[] = {
    {2.0, 3.0, 0.0, 21395.5},       // index 0
    {3.0, 4.0, 0.0, 21395.5},       // index 1
    {4.0, 5.0, 0.0, 21395.5},       // index 2
    
    {2.0, 3.0, 21395.5, 53640.9},   // index 3
    {3.0, 4.0, 21395.5, 53640.9},   // index 4
    {4.0, 5.0, 21395.5, 53640.9},   // index 5
    
    {2.0, 3.0, 53640.9, 109768},   // index 6
    {3.0, 4.0, 53640.9, 109768},   // index 7
    {4.0, 5.0, 53640.9, 109768},   // index 8
    
    {2.0, 3.0, 109768, 250000},     // index 9
    {3.0, 4.0, 109768, 250000},     // index 10
    {4.0, 5.0, 109768, 250000}      // index 11
};
void PerformFitting(TH1F* hPi0Mass, bool setFitManual, TF1*& totalFit, double& fitStart, double& fitEnd) {
    // Define the start of the fit
    int binThreshold = 1;
    int firstBinAboveThreshold = 0;
    for (int i = 1; i <= hPi0Mass->GetNbinsX() - 1; ++i) {
        double derivative = hPi0Mass->GetBinContent(i + 1) - hPi0Mass->GetBinContent(i);
        if (derivative > binThreshold && firstBinAboveThreshold == 0) {
            firstBinAboveThreshold = i;
            break;
        }
    }
    fitStart = hPi0Mass->GetBinLowEdge(firstBinAboveThreshold);
//    fitStart = 0.11;
    fitEnd = 0.5;

    // Define totalFit
    totalFit = new TF1("totalFit", "gaus(0) + pol4(3)", fitStart, fitEnd);

    // Set fitting parameters
    if (setFitManual) {
        totalFit->SetParameter(0, 100000);
        totalFit->SetParameter(1, 0.14);
        totalFit->SetParameter(2, 0.04);
        totalFit->SetParLimits(1, 0.13, 0.15);
//        totalFit->SetParLimits(2, 0.001, 0.03);
    } else {
        int bin1 = hPi0Mass->GetXaxis()->FindBin(0.1);
        int bin2 = hPi0Mass->GetXaxis()->FindBin(0.17);
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
        double sigmaEstimate = 0.02;
        totalFit->SetParameter(2, sigmaEstimate);
        totalFit->SetParLimits(1, maxBinCenter - sigmaEstimate, maxBinCenter + sigmaEstimate);
        totalFit->SetParLimits(2, sigmaEstimate - .1*sigmaEstimate, sigmaEstimate + .1*sigmaEstimate);
    }

    // Apply the fit
    hPi0Mass->Fit("totalFit", "R+");
}
double CalculateSignalToBackgroundRatio(TF1* totalFit, TF1* polyFit, double fitMean, double fitSigma) {
    double signalPlusBackground = totalFit->Integral(fitMean - 2*fitSigma, fitMean + 2*fitSigma);
    double background = polyFit->Integral(fitMean - 2*fitSigma, fitMean + 2*fitSigma);
    double netSignal = signalPlusBackground - background;
    double signalToBackgroundRatio = netSignal / background;

    std::cout << "Area Under Total Fit: " << signalPlusBackground << std::endl;
    std::cout << "Area Under Background Fit: " << background << std::endl;
    std::cout << "Net Signal: " << netSignal << std::endl;
    std::cout << "Signal-to-Background Ratio: " << signalToBackgroundRatio << std::endl;

    return signalToBackgroundRatio;
}
void WriteGaussianParametersToFile(int histIndex, double fitMean, double fitMeanError, double fitSigma, double fitSigmaError) {
    if (isFitGood) {
        std::ofstream meanFile("/Users/patsfan753/Desktop/AnalyzePi0s_Final/dataOutput/GaussianMean.txt", std::ios::app);
        std::ofstream gaussianErrorFile("/Users/patsfan753/Desktop/AnalyzePi0s_Final/dataOutput/GaussianError.txt", std::ios::app);

        if (meanFile.is_open() && gaussianErrorFile.is_open()) {
            meanFile << "Index " << histIndex << ": Mean: " << fitMean << ", Mean Error: " << fitMeanError << std::endl;
            gaussianErrorFile << "Index " << histIndex << ": Sigma: " << fitSigma << ", Sigma Error: " << fitSigmaError << std::endl;
        } else {
            std::cerr << "Unable to open file(s) for writing Gaussian parameters." << std::endl;
        }

        meanFile.close();
        gaussianErrorFile.close();
        std::cout << "Fit good. Added Gaussian parameters to files for Index " << histIndex << std::endl;
    } else {
        std::cout << "Fit not good for Index " << histIndex << ". No Gaussian parameters added to files." << std::endl;
    }
}

void CalculateSignalYieldAndError(TH1F* hPi0Mass, TF1* polyFit, double fitMean, double fitSigma, int histIndex) {
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
    double signalYield, signalError;
    signalYield = hSignal->IntegralAndError(firstBinSignal, lastBinSignal, signalError, "");
    std::cout << "isFitGood status before appending to array: " << (isFitGood ? "true" : "false") << std::endl;
    if (isFitGood) {
        std::ofstream yieldFile("/Users/patsfan753/Desktop/AnalyzePi0s_Final/dataOutput/signalYield.txt", std::ios::app);
        std::ofstream errorFile("/Users/patsfan753/Desktop/AnalyzePi0s_Final/dataOutput/signalError.txt", std::ios::app);
        if (yieldFile.is_open() && errorFile.is_open()) {
            yieldFile << "Index " << histIndex << ": " << signalYield << std::endl;
            errorFile << "Index " << histIndex << ": " << signalError << std::endl;
        } else {
            std::cerr << "Unable to open file(s) for writing." << std::endl;
        }
        yieldFile.close();
        errorFile.close();
        std::cout << "Fit good. Added to files for Index " << histIndex << " - Signal Yield: " << signalYield << ", Signal Error: " << signalError << std::endl;
    } else {
        std::cout << "Fit not good for Index " << histIndex << ". No values added to files." << std::endl;
    }
    std::cout << "Signal Yield: " << signalYield << " +/- " << signalError << std::endl;
}
void DrawCanvasTextSignalAndGaussPlots(TLatex& latex){
    latex.SetTextSize(0.03);
    latex.DrawLatex(0.68, 0.86, "Cuts (Inclusive):");
    latex.DrawLatex(0.13, 0.82, Form("#Delta R #geq %.3f", globalCutValues.deltaR));
    latex.DrawLatex(0.13, 0.78, Form("Asymmetry < %.3f", globalCutValues.asymmetry));
    latex.DrawLatex(0.13, 0.74, Form("#chi^{2} < %.3f", globalCutValues.chi));
    latex.DrawLatex(0.13, 0.615, Form("Cluster E #geq %.3f GeV", globalCutValues.clusE));
}
void GenerateSignalAndGaussParPlots() {
    const int nPoints = 12; // Total number of points in each file
    double yield[nPoints], error[nPoints];
    // Additional arrays to store Gaussian Mean and Sigma values
    double gaussianMean[nPoints], gaussianMeanError[nPoints];
    double gaussianSigma[nPoints], gaussianSigmaError[nPoints];
    // Define a small jitter amount for horizontal offset of points
    const double jitterAmount = 0.04;
    
    double xPoints[nPoints] = {2.5, 3.5, 4.5, 2.5, 3.5, 4.5, 2.5, 3.5, 4.5, 2.5, 3.5, 4.5};
    std::ifstream yieldFile("/Users/patsfan753/Desktop/AnalyzePi0s_Final/dataOutput/signalYield.txt");
    std::ifstream errorFile("/Users/patsfan753/Desktop/AnalyzePi0s_Final/dataOutput/signalError.txt");
    std::string line;
    int index;
    double value, errorValue;
    if (!yieldFile.is_open() || !errorFile.is_open()) {
        std::cerr << "Error opening files." << std::endl;
        return;
    }
    while (getline(yieldFile, line)) {
        sscanf(line.c_str(), "Index %d: %lf", &index, &value);
        yield[index] = value;
    }
    while (getline(errorFile, line)) {
        sscanf(line.c_str(), "Index %d: %lf", &index, &value);
        error[index] = value;
    }
    yieldFile.close();
    errorFile.close();
    
    
    // Reading Gaussian Mean values
    std::ifstream gaussianMeanFile("/Users/patsfan753/Desktop/AnalyzePi0s_Final/dataOutput/GaussianMean.txt");
    if (!gaussianMeanFile.is_open()) {
        std::cerr << "Error opening GaussianMean.txt." << std::endl;
        return;
    }
    while (getline(gaussianMeanFile, line)) {
        sscanf(line.c_str(), "Index %d: Mean: %lf, Mean Error: %lf", &index, &value, &errorValue);
        gaussianMean[index] = value;
        gaussianMeanError[index] = errorValue;
    }
    gaussianMeanFile.close();

    // Reading Gaussian Sigma values
    std::ifstream gaussianSigmaFile("/Users/patsfan753/Desktop/AnalyzePi0s_Final/dataOutput/GaussianError.txt");
    if (!gaussianSigmaFile.is_open()) {
        std::cerr << "Error opening GaussianError.txt." << std::endl;
        return;
    }
    while (getline(gaussianSigmaFile, line)) {
        sscanf(line.c_str(), "Index %d: Sigma: %lf, Sigma Error: %lf", &index, &value, &errorValue);
        gaussianSigma[index] = value;
        gaussianSigmaError[index] = errorValue;
    }
    gaussianSigmaFile.close();
    
    
    TGraphErrors *graphs[4];
    int indices[4][3] = {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {9, 10, 11}};
    int colors[4] = {kRed, kBlue, kGreen, kMagenta}; // Colors for different graphs
    const char* labels[] = {
        "60-100%",
        "40-60%",
        "20-40%",
        "0-20 %"
    };
    
    TH1F *hDummy = new TH1F("hDummy", "", 4, 1.5, 5.5);
    hDummy->GetYaxis()->SetTitle("Signal Yield");
    hDummy->GetXaxis()->CenterTitle(true);
    hDummy->SetTitle("#pi^{0} Signal Yield");
    hDummy->GetXaxis()->SetTitle("#pi^{0} p_{T} [GeV]");
    hDummy->GetXaxis()->SetTitleOffset(1.1);
    hDummy->GetXaxis()->SetLimits(2, 5); // This sets the user-defined limits for the axis
    hDummy->GetXaxis()->SetLabelSize(0.04); // You can adjust the size as needed
    // Manually setting the x-axis labels
    hDummy->GetXaxis()->SetNdivisions(004); // This will create four primary divisions (2, 3, 4, 5)
    for (int i = 0; i < 4; ++i) {
        graphs[i] = new TGraphErrors(3);
        for (int j = 0; j < 3; ++j) {
            int idx = indices[i][j];
            graphs[i]->SetPoint(j, xPoints[idx], yield[idx]);
            graphs[i]->SetPointError(j, 0, error[idx]);
            graphs[i]->SetLineColor(colors[i]);
            graphs[i]->SetMarkerColor(colors[i]);
            graphs[i]->SetMarkerSize(1.2);
            
            // Print statement for verification
            std::cout << "Graph " << i << ", Point " << j << ": "
                      << "Index = " << idx << ", "
                      << "X = " << xPoints[idx] << ", "
                      << "Yield = " << yield[idx] << ", "
                      << "Error = " << error[idx] << std::endl;
            
        }
    }
    TCanvas *c = new TCanvas("c", "Overlayed Curves", 800, 600);
    c->cd(); // Ensure we are drawing on the canvas
    c->SetLogy();
    double maxYield = *std::max_element(yield, yield + nPoints);
    double minYield = *std::min_element(yield, yield + nPoints); // Find the minimum yield
    double minError = *std::min_element(error, error + nPoints); // Find the smallest error
    hDummy->SetMaximum(maxYield * 10); // Set to 10 times the max for log scale buffer
    hDummy->SetMinimum(minYield / 10); // Set to 1/10th of the min for log scale buffer
    hDummy->SetMinimum(0.1); // Set a non-zero minimum for log scale
    hDummy->SetStats(0); // Remove the statistics box
    hDummy->GetYaxis()->SetTitle("#pi^{0} Signal Yield");
    hDummy->Draw(); // Draw the dummy histogram to define axis ranges
    TLegend *leg = new TLegend(0.14, 0.14, 0.34, 0.34);
    // Set the header with a bigger, bolder font
    leg->SetHeader("Centrality:");
    leg->SetMargin(0.15);
    leg->SetBorderSize(0);
    leg->SetTextSize(0.04);
    for (int i = 0; i < 4; ++i) {
        graphs[i]->SetMarkerStyle(20 + i);
        graphs[i]->Draw("P SAME");
        leg->AddEntry(graphs[i], labels[i], "ep");
    }
    leg->Draw();
    TLatex latex;
    latex.SetNDC();
    DrawCanvasTextSignalAndGaussPlots(latex);
    c->Update(); // Update the canvas
    c->SaveAs("/Users/patsfan753/Desktop/AnalyzePi0s_Final/plotOutput/signal/signalYield.pdf");
    
    TCanvas *cError = new TCanvas("cError", "Signal Error Plot", 800, 600);
    cError->cd(); // Ensure we are drawing on the new canvas
    TH1F *hDummyError = new TH1F("hDummyError", "", 4, 1.5, 5.5);
    hDummyError->GetYaxis()->SetTitle("Relative Error = Signal Error / Signal Yield");
    hDummyError->GetXaxis()->CenterTitle(true);
    hDummyError->GetXaxis()->SetTitle("#pi^{0} p_{T} [GeV]");
    hDummyError->GetXaxis()->SetTitleOffset(1.1); // Adjust this value as needed to position the title
    hDummyError->SetTitle("Relative Error of #pi^{0} Signal Yield");
    hDummyError->GetXaxis()->SetLimits(2, 5); // This sets the user-defined limits for the axis
    hDummyError->GetXaxis()->SetLabelSize(0.04); // You can adjust the size as needed
    hDummyError->GetXaxis()->SetNdivisions(004); // This will create four primary divisions (2, 3, 4, 5)
    hDummyError->SetMaximum(0.7); // Adjust if necessary for your error values
    hDummyError->SetMinimum(0); // The minimum is 0
    hDummyError->SetStats(0); // Remove the statistics box
    hDummyError->Draw(); // Draw the dummy histogram to define axis ranges
    
    std::vector<double> relativeErrors(nPoints);
    for (int i = 0; i < nPoints; ++i) {
        if (yield[i] != 0) {
            relativeErrors[i] = error[i] / yield[i];
        } else {
            relativeErrors[i] = 0;
        }
    }
    TGraph *errorGraphs[4];
    for (int i = 0; i < 4; ++i) {
        errorGraphs[i] = new TGraph(3);
        for (int j = 0; j < 3; ++j) {
            int idx = indices[i][j];
            double adjustedX = xPoints[idx] + (i - 1.5) * jitterAmount; // Apply jittering
            errorGraphs[i]->SetPoint(j, adjustedX, relativeErrors[idx]);
            errorGraphs[i]->SetLineColor(colors[i]);
            errorGraphs[i]->SetMarkerColor(colors[i]);
            errorGraphs[i]->SetMarkerSize(1.2); // Smaller marker size
            errorGraphs[i]->SetMarkerStyle(20 + i); // Different marker style for error plot
            errorGraphs[i]->Draw("P SAME"); // Draw graphs with points
            
            // Print statement for verification
            std::cout << "Error Graph " << i << ", Point " << j << ": "
                      << "Index = " << idx << ", "
                      << "X = " << xPoints[idx] << ", "
                      << "Relative Error = " << relativeErrors[idx] << std::endl;
        }
    }
    TLegend *legError = new TLegend(0.12, 0.68, 0.32, 0.88); // Use the same coordinates for consistency
    legError->SetHeader("Centrality:","C"); // 'C' option centers the header
    legError->SetMargin(0.4);
    legError->SetBorderSize(0);
    legError->SetTextSize(0.04);
    for (int i = 0; i < 4; ++i) {
        errorGraphs[i]->SetMarkerStyle(20 + i);
        errorGraphs[i]->Draw("P SAME");
        legError->AddEntry(errorGraphs[i], labels[i], "p");
    }
    legError->Draw();
    TLatex latexError;
    latexError.SetNDC();
    DrawCanvasTextSignalAndGaussPlots(latexError);

    
    cError->Update();
    cError->SaveAs("/Users/patsfan753/Desktop/AnalyzePi0s_Final/plotOutput/signal/signalError_curve.pdf");
    
    
    TCanvas *cGaussMean = new TCanvas("cGaussMean", "Gaussian Mean", 800, 600);
    cGaussMean->cd(); // Ensure we are drawing on the canvas
    
    TH1F *hDummyGausMean = new TH1F("hDummyGausMean", "", 4, 1.5, 5.5);
    hDummyGausMean->GetYaxis()->SetTitle("Gaussian Mean");
    hDummyGausMean->GetXaxis()->CenterTitle(true);
    hDummyGausMean->SetTitle("Gaussian Mean");
    hDummyGausMean->GetXaxis()->SetTitle("Diphoton p_{T} [GeV]");
    hDummyGausMean->GetXaxis()->SetTitleOffset(1.1);
    hDummyGausMean->GetXaxis()->SetLimits(2, 5); // This sets the user-defined limits for the axis
    hDummyGausMean->GetXaxis()->SetLabelSize(0.04); // You can adjust the size as needed
    hDummyGausMean->GetXaxis()->SetNdivisions(004); // This will create four primary divisions (2, 3, 4, 5)
    hDummyGausMean->SetMinimum(.06); // Replace yourMinValue with the desired minimum value
    hDummyGausMean->SetMaximum(.25); // Replace yourMaxValue with the desired maximum value
    hDummyGausMean->SetStats(0); // Remove the statistics box
    hDummyGausMean->GetYaxis()->SetTitle("Gaussian Mean [GeV]");
    hDummyGausMean->Draw(); // Draw the dummy histogram to define axis ranges

    TGraphErrors *GaussMeanGraphs[4];
    for (int i = 0; i < 4; ++i) {
        GaussMeanGraphs[i] = new TGraphErrors(3);
        for (int j = 0; j < 3; ++j) {
            int idx = indices[i][j];
            double adjustedX = xPoints[idx] + (i - 1.5) * jitterAmount; // Apply jittering
            GaussMeanGraphs[i]->SetPoint(j, adjustedX, gaussianMean[idx]);
            GaussMeanGraphs[i]->SetPointError(j, 0, gaussianMeanError[idx]);
            GaussMeanGraphs[i]->SetLineColor(colors[i]);
            GaussMeanGraphs[i]->SetMarkerColor(colors[i]);
            GaussMeanGraphs[i]->SetMarkerSize(1.3);
            
            // Print statement for verification
            std::cout << "Graph " << i << ", Point " << j << ": "
                      << "Index = " << idx << ", "
                      << "X = " << xPoints[idx] << ", "
                      << "Gaussian Mean = " << gaussianMean[idx] << ", "
                      << "Gaussian Mean Error = " << gaussianMeanError[idx] << std::endl;
            
        }
    }
    // Draw the horizontal line at y = 0.135
    double linePosition = 0.135;
    TLine *meanLine = new TLine(hDummyGausMean->GetXaxis()->GetXmin(), linePosition, hDummyGausMean->GetXaxis()->GetXmax(), linePosition);
    meanLine->SetLineColor(kRed); // Choose a color that stands out
    meanLine->SetLineStyle(2); // Set line style to dashed (2)
    meanLine->SetLineWidth(1); // Set a thinner line width
    meanLine->Draw();


    // Add label for the line
    TLatex label;
    label.SetNDC();
    label.SetTextSize(0.03);
    label.SetTextColor(kRed); // Match the line color
    // Position the label near the line, adjust the coordinates as necessary
    label.DrawLatex(0.82, 0.42, "#pi^{0} mass");

    // Position the legend in the top-left corner
    TLegend *legGaussMean = new TLegend(0.11, 0.68, 0.31, 0.88);
    // Set the header with a bigger, bolder font
    legGaussMean->SetHeader("Centrality:");
    legGaussMean->SetMargin(0.15);
    legGaussMean->SetBorderSize(0);
    legGaussMean->SetTextSize(0.04);
    for (int i = 0; i < 4; ++i) {
        GaussMeanGraphs[i]->SetMarkerStyle(20 + i);
        GaussMeanGraphs[i]->Draw("P SAME");
        legGaussMean->AddEntry(graphs[i], labels[i], "ep");
    }
    legGaussMean->Draw();
    // Position the cuts text in the top-right corner
    
    TLatex latexGaussMean;
    latexGaussMean.SetNDC();
    DrawCanvasTextSignalAndGaussPlots(latexGaussMean);
    
    cGaussMean->Update(); // Update the canvas
    cGaussMean->SaveAs("/Users/patsfan753/Desktop/AnalyzePi0s_Final/plotOutput/gaussPars/mean.pdf");
    
    
    TCanvas *cGaussSigma = new TCanvas("cGaussSigma", "Gaussian Sigma", 800, 600);
    cGaussSigma->cd(); // Ensure we are drawing on the canvas

    TH1F *hDummyGausSigma = new TH1F("hDummyGausSigma", "", 4, 1.5, 5.5);
    hDummyGausSigma->GetYaxis()->SetTitle("Gaussian Sigma");
    hDummyGausSigma->GetXaxis()->CenterTitle(true);
    hDummyGausSigma->SetTitle("Gaussian Sigma");
    hDummyGausSigma->GetXaxis()->SetTitle("Diphoton p_{T} [GeV]");
    hDummyGausSigma->GetXaxis()->SetTitleOffset(1.1);
    hDummyGausSigma->GetXaxis()->SetLimits(2, 5); // This sets the user-defined limits for the axis
    hDummyGausSigma->GetXaxis()->SetLabelSize(0.04); // You can adjust the size as needed
    hDummyGausSigma->GetXaxis()->SetNdivisions(004); // This will create four primary divisions (2, 3, 4, 5)
    hDummyGausSigma->SetMinimum(0); // Replace yourMinValue with the desired minimum value
    hDummyGausSigma->SetMaximum(.07); // Replace yourMaxValue with the desired maximum value
    hDummyGausSigma->SetStats(0); // Remove the statistics box
    hDummyGausSigma->GetYaxis()->SetTitle("Gaussian Sigma [GeV]");
    hDummyGausSigma->Draw(); // Draw the dummy histogram to define axis ranges

    TGraphErrors *GaussSigmaGraphs[4];
    for (int i = 0; i < 4; ++i) {
        GaussSigmaGraphs[i] = new TGraphErrors(3);
        for (int j = 0; j < 3; ++j) {
            int idx = indices[i][j];
            double adjustedX = xPoints[idx] + (i - 1.5) * jitterAmount; // Apply jittering
            GaussSigmaGraphs[i]->SetPoint(j, adjustedX, gaussianSigma[idx]);
            GaussSigmaGraphs[i]->SetPointError(j, 0, gaussianSigmaError[idx]);
            GaussSigmaGraphs[i]->SetLineColor(colors[i]);
            GaussSigmaGraphs[i]->SetMarkerColor(colors[i]);
            GaussSigmaGraphs[i]->SetMarkerSize(1.2);
            
            // Print statement for verification
            std::cout << "Graph " << i << ", Point " << j << ": "
                      << "Index = " << idx << ", "
                      << "X = " << xPoints[idx] << ", "
                      << "Gaussian Sigma = " << gaussianSigma[idx] << ", "
                      << "Gaussian Sigma Error = " << gaussianSigmaError[idx] << std::endl;
            
        }
    }
    TLegend *legGaussSigma = new TLegend(0.11, 0.68, 0.31, 0.88);
    // Set the header with a bigger, bolder font
    legGaussSigma->SetHeader("Centrality:");
    legGaussSigma->SetMargin(0.15);
    legGaussSigma->SetBorderSize(0);
    legGaussSigma->SetTextSize(0.04);
    for (int i = 0; i < 4; ++i) {
        GaussSigmaGraphs[i]->SetMarkerStyle(20 + i);
        GaussSigmaGraphs[i]->Draw("P SAME");
        legGaussSigma->AddEntry(GaussSigmaGraphs[i], labels[i], "ep");
    }
    legGaussSigma->Draw();
    TLatex latexGaussSigma;
    latexGaussSigma.SetNDC();
    DrawCanvasTextSignalAndGaussPlots(latexGaussSigma);
    cGaussSigma->Update(); // Update the canvas
    cGaussSigma->SaveAs("/Users/patsfan753/Desktop/AnalyzePi0s_Final/plotOutput/gaussPars/sigma.pdf");
}
void DrawCanvasText(TLatex& latex, const Range& selectedRange, double fitMean, double fitSigma, double signalToBackgroundRatio) {
    // Drawing text related to the range and cuts
    std::ostringstream mbdStream, ptStream;
    mbdStream << std::fixed << std::setprecision(1) << selectedRange.mbdLow << " #leq MBD Charge < " << selectedRange.mbdHigh;
    ptStream << std::fixed << std::setprecision(1) << selectedRange.ptLow << " #leq Diphoton p_{T} < " << selectedRange.ptHigh << " GeV";

    latex.SetTextSize(0.035);
    latex.DrawLatex(0.13, 0.86, "Cuts (Inclusive):");
    latex.DrawLatex(0.13, 0.82, Form("#Delta R #geq %.3f", globalCutValues.deltaR));
    latex.DrawLatex(0.13, 0.78, Form("Asymmetry < %.3f", globalCutValues.asymmetry));
    latex.DrawLatex(0.13, 0.74, Form("#chi^{2} < %.3f", globalCutValues.chi));
    latex.DrawLatex(0.13, 0.70, mbdStream.str().c_str());
    latex.DrawLatex(0.13, 0.66, ptStream.str().c_str());
    latex.DrawLatex(0.13, 0.615, Form("Cluster E #geq %.3f GeV", globalCutValues.clusE));

    // Drawing text related to Gaussian parameters and S/B ratio
    latex.SetTextSize(0.036);
    latex.DrawLatex(0.43, 0.86, Form("Mean of Gaussian: %.4f GeV", fitMean));
    latex.DrawLatex(0.43, 0.81, Form("Std. Dev. of Gaussian: %.4f GeV", fitSigma));
    latex.DrawLatex(0.43, 0.76, Form("S/B Ratio: %.4f", signalToBackgroundRatio));
}
void WriteDataToCSV(int histIndex, const CutValues& cutValues, double fitMean, double fitMeanError, double fitSigma, double fitSigmaError, double signalToBackgroundRatio, double signalYield, double signalError) {
    // Check if the fit is good before proceeding
    if (!isFitGood) {
        std::cout << "Fit is not good. Skipping CSV write." << std::endl;
        return;
    }

    std::string filename = "/Users/patsfan753/Desktop/AnalyzePi0s_Final/dataOutput/PlotByPlotOutput.csv";
    std::ifstream checkFile(filename);
    bool fileIsEmpty = checkFile.peek() == std::ifstream::traits_type::eof();
    checkFile.close();

    std::ofstream file(filename, std::ios::app); // Open the file in append mode

    if (!file.is_open()) {
        std::cerr << "Unable to open CSV file for writing." << std::endl;
        return;
    }

    // Write column headers if file is empty
    if (fileIsEmpty) {
        file << "Index,Energy,Asymmetry,Chi2,DeltaR,GaussMean,GaussMeanError,GaussSigma,GaussSigmaError,S/B,Yield,YieldError\n";
    }

    // Write data to CSV
    file << histIndex << ",";
    file << cutValues.clusE << ",";
    file << cutValues.asymmetry << ",";
    file << cutValues.chi << ",";
    file << cutValues.deltaR << ",";
    file << fitMean << ",";
    file << fitMeanError << ",";
    file << fitSigma << ",";
    file << fitSigmaError << ",";
    file << signalToBackgroundRatio << ",";
    file << signalYield << ",";
    file << signalError << "\n";

    file.close();
}
void AnalyzePi0() {
    // Load the root file
    TFile *file = new TFile(globalFilename.c_str(), "READ");
    // Initialize global cut values
    globalCutValues = parseFileName();
    
    int histIndex = 0;  // This is the variable you can change of which histogram to extract
    
    // User interaction to set global isFitGood
    char userInput;
    std::cout << "Is fit ready to be finalized? (Y/N): ";
    std::cin >> userInput;
    isFitGood = (userInput == 'Y' || userInput == 'y'); // Directly setting the global variable

    
    // Fetch histogram based on index
    std::string histName = "hPi0Mass_" + std::to_string(histIndex);
    TH1F *hPi0Mass = (TH1F*)file->Get(histName.c_str());
    hPi0Mass->GetYaxis()->SetRangeUser(0, 800);

    // Declare variables for fit parameters
    TF1 *totalFit;
    double fitStart, fitEnd;

    // Call PerformFitting to execute the fitting procedure
    PerformFitting(hPi0Mass, false, totalFit, fitStart, fitEnd);//SET FALSE TO TRUE FOR MANUAL FITTING CODE
    
    
    TCanvas *canvas = new TCanvas("canvas", "Pi0 Mass Distribution", 900, 600);
    hPi0Mass->SetMarkerStyle(20);
    hPi0Mass->SetMarkerSize(1.0);
    hPi0Mass->SetMarkerColor(kBlack);
    hPi0Mass->Draw("PE");

    double fitMean = totalFit->GetParameter(1);
    double fitMeanError = totalFit->GetParError(1); // Corrected index to 1 for mean
    double fitSigma = totalFit->GetParameter(2);
    double fitSigmaError = totalFit->GetParError(2);
    
    // Call the new method to write Gaussian parameters to files
    WriteGaussianParametersToFile(histIndex, fitMean, fitMeanError, fitSigma, fitSigmaError);

    
    TF1 *gaussFit = new TF1("gaussFit", "gaus", fitStart, .28);
    TF1 *polyFit = new TF1("polyFit", "pol4", fitStart, .28);
    gaussFit->SetParameter(0, totalFit->GetParameter(0));
    gaussFit->SetParameter(1, totalFit->GetParameter(1));
    gaussFit->SetParameter(2, totalFit->GetParameter(2));
    for (int i = 3; i < 8; i++) {
        polyFit->SetParameter(i - 3, totalFit->GetParameter(i));
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

    double signalToBackgroundRatio = CalculateSignalToBackgroundRatio(totalFit, polyFit, fitMean, fitSigma);

    // Call the new method to draw text on the canvas
    DrawCanvasText(latex, ranges[histIndex], fitMean, fitSigma, signalToBackgroundRatio);

    double amplitude = totalFit->GetParameter(0);
    TLine *line1 = new TLine(fitMean + 2*fitSigma, 0, fitMean + 2*fitSigma, amplitude+50);
    TLine *line2 = new TLine(fitMean - 2*fitSigma, 0, fitMean - 2*fitSigma, amplitude+50);
    line1->SetLineColor(kBlack);
    line1->SetLineStyle(1);
    line2->SetLineColor(kBlack);
    line2->SetLineStyle(1);
    line1->Draw("same");
    line2->Draw("same");
    std::string imageName = "/Users/patsfan753/Desktop/AnalyzePi0s_Final/plotOutput/InvMassPlots/" + histName + "_fit.pdf";
    canvas->SaveAs(imageName.c_str());
    CalculateSignalYieldAndError(hPi0Mass, polyFit, fitMean, fitSigma, histIndex);
    // Read the signal yield and error from text files
    double signalYield = 0.0, signalError = 0.0;
    if (isFitGood) {
        std::ifstream yieldFile("/Users/patsfan753/Desktop/AnalyzePi0s_Final/dataOutput/signalYield.txt");
        std::ifstream errorFile("/Users/patsfan753/Desktop/AnalyzePi0s_Final/dataOutput/signalError.txt");
        std::string line;

        while (getline(yieldFile, line)) {
            if (line.find("Index " + std::to_string(histIndex) + ":") != std::string::npos) {
                std::istringstream iss(line.substr(line.find(':') + 1));
                iss >> signalYield;
                break;
            }
        }

        while (getline(errorFile, line)) {
            if (line.find("Index " + std::to_string(histIndex) + ":") != std::string::npos) {
                std::istringstream iss(line.substr(line.find(':') + 1));
                iss >> signalError;
                break;
            }
        }
    }
    // Call the new method to write to CSV if the fit is good
    WriteDataToCSV(histIndex, globalCutValues, fitMean, fitMeanError, fitSigma, fitSigmaError, signalToBackgroundRatio, signalYield, signalError);
    // Conditional plotting
    if (CreateSignalYieldAndErrorPlot) {
        GenerateSignalAndGaussParPlots();
    }
}
