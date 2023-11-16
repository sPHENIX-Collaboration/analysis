#include <TCanvas.h>
#include <TFile.h>
#include <TH1F.h>
#include <TF1.h>
#include <TLatex.h>
#include <iomanip> // Include this for std::setprecision
#include <sstream> // Include this for std::stringstream


struct Range {
    double ptLow;
    double ptHigh;
    double mbdLow;
    double mbdHigh;
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

bool isFitGood = true; // Initially set to false

void PlottingMass() {
    // Load the root file
    TFile *file = new TFile("/Users/patsfan753/Desktop/Pi0_Organized/HistOutput1GeVallRuns.root", "READ");
    
    
    int value = 11;  // This is the variable you can change of which histogram to extract

    
    
    
    // Fetch histogram based on index
    std::string histName = "hPi0Mass_" + std::to_string(value);
    TH1F *hPi0Mass = (TH1F*)file->Get(histName.c_str());

    // Set y-axis range
    hPi0Mass->GetYaxis()->SetRangeUser(0, 5000);

    /*
     Idea to fix this with fixed delta R calculation
     --dynamically find first bin with content, set fitStart to binCenter
     */
    int binThreshold = 1; // Set an appropriate threshold for your data, the max difference between the two starting points that you want to start the fit at, this should increase as counts increases drastically
    int firstBinAboveThreshold = 0;

    for (int i = 1; i <= hPi0Mass->GetNbinsX() - 1; ++i) {
        // Calculate the derivative (difference between adjacent bins)
        double derivative = hPi0Mass->GetBinContent(i + 1) - hPi0Mass->GetBinContent(i);
        
        // Check if the derivative is above the threshold and if we haven't already found a bin
        if (derivative > binThreshold && firstBinAboveThreshold == 0) {
            firstBinAboveThreshold = i; // This is the first bin where the slope starts to rise
            break;
        }
    }

    double xValueToStartFit = hPi0Mass->GetBinLowEdge(firstBinAboveThreshold);


    // Set fit range and function
    double fitStart = xValueToStartFit;
//    double fitStart = xValueToStartFit;
    double fitEnd = 0.5;

    
    TF1 *totalFit = new TF1("totalFit", "gaus(0) + pol4(3)", fitStart, fitEnd);
    
//    /*
//     Manually set fit parameters
//     */
    // Initialize fit parameters based on the histogram
//    totalFit->SetParameter(0, 41000); // amplitude, around the peak height
//    totalFit->SetParameter(1, 0.155); // mean, centered around the peak
//    totalFit->SetParameter(2, 0.04); // sigma, roughly estimating from plot
//    totalFit->SetParLimits(1, 0.15, 0.16); // limits for mean
//    totalFit->SetParLimits(2, 0.01, 0.02); // limits for sigma
//    
    /*
     Dynamic setting of fit parameters
     Idea--find the first highest point as loop through bins from fitStart to the right
     */
    // Locate the bin corresponding to x-axis value of 0.1 in the histogram
    int bin1 = hPi0Mass->GetXaxis()->FindBin(0.1);

    // Locate the bin corresponding to x-axis value of 0.2 in the histogram
    int bin2 = hPi0Mass->GetXaxis()->FindBin(0.21);

    // Initialize maxBin with the first bin's position in the range to start comparison
    int maxBin = bin1;

    // Initialize maxBinContent with the content (y-value) of the first bin in the range
    double maxBinContent = hPi0Mass->GetBinContent(bin1);

    // Loop over the bins from the bin right after bin1 up to and including bin2
    for(int i = bin1 + 1; i <= bin2; ++i) {
        // Check if the content of the current bin is greater than the current maxBinContent
        if(hPi0Mass->GetBinContent(i) > maxBinContent) {
            // If it is greater, update maxBinContent to the current bin's content
            maxBinContent = hPi0Mass->GetBinContent(i);
            // Also update maxBin to the current bin's position as this is the new maximum
            maxBin = i;
        }
    }

    // Calculate the center of the bin that has the maximum content, this is likely the peak position
    double maxBinCenter = hPi0Mass->GetXaxis()->GetBinCenter(maxBin);

    // Set the 0th parameter of the fit function (amplitude of Gaussian) to the maximum bin content
    totalFit->SetParameter(0, maxBinContent);

    // Set the 1st parameter of the fit function (mean of Gaussian) to the center of the max bin
    totalFit->SetParameter(1, maxBinCenter);

    // Define a fixed value for the estimated standard deviation of the Gaussian
    double sigmaEstimate = 0.01;

    // Set the 2nd parameter of the fit function (standard deviation of Gaussian) to the estimated value
    totalFit->SetParameter(2, sigmaEstimate);

    // Set the limits for the 1st parameter (mean of Gaussian) around the estimated peak within ±sigmaEstimate
    totalFit->SetParLimits(1, maxBinCenter - sigmaEstimate, maxBinCenter + sigmaEstimate); // limits for mean
    // Set the limits for the 1st parameter (mean of Gaussian) around the estimated peak within ±sigmaEstimate
    totalFit->SetParLimits(2, .01, .02); // limits for mean
    
    
    
    // Fit
    hPi0Mass->Fit("totalFit", "R+");

    // Create canvas and draw
    TCanvas *canvas = new TCanvas("canvas", "Pi0 Mass Distribution", 900, 600);
    hPi0Mass->SetMarkerStyle(20);
    hPi0Mass->SetMarkerSize(1.0);
    hPi0Mass->SetMarkerColor(kBlack);
    hPi0Mass->Draw("PE");

    // Display information with TLatex LABELLING IS FOR INCLUSIVE CUTS (WHAT IS BEING INCLUDED)
    
    // Write correct ranges based on index
    Range selectedRange = ranges[value];
    std::ostringstream mbdStream;
    std::ostringstream ptStream;

    // Set fixed number of decimal places for MBD Charge and Diphoton p_T
    mbdStream << std::fixed << std::setprecision(1) << selectedRange.mbdLow << " #leq MBD Charge < " << std::setprecision(1) << selectedRange.mbdHigh;
    ptStream << std::fixed << std::setprecision(1) << selectedRange.ptLow << " #leq Diphoton p_{T} < " << std::setprecision(1) << selectedRange.ptHigh << " GeV";

    std::string mbdText = mbdStream.str();
    std::string ptText = ptStream.str();

    TLatex latex;
    latex.SetNDC();
    latex.SetTextSize(0.035);
    latex.DrawLatex(0.13, 0.86, "Cuts (Inclusive):");
    latex.DrawLatex(0.13, 0.82, "#Delta R #geq 0.08");
    latex.DrawLatex(0.13, 0.78, "Asymmetry < 0.5");
    latex.DrawLatex(0.13, 0.74, "#chi^{2} < 4");
    latex.DrawLatex(0.13, 0.70, mbdText.c_str());
    latex.DrawLatex(0.13, 0.66, ptText.c_str());
    latex.DrawLatex(0.13, 0.615, "Cluster E #geq 1.0 GeV");


    // Extract the mean (parameter 1) of the Gaussian fit
    double fitMean = totalFit->GetParameter(1);
    
    // Get the standard deviation (RMS) for the Gaussian component of the fit
    double fitSigma = totalFit->GetParameter(2);
    double fitSigmaError = totalFit->GetParError(2); // Get the error on the Gaussian width

    
    // Debug print to check the status of isFitGood
    std::cout << "isFitGood status for Gaussian parameters: " << (isFitGood ? "true" : "false") << std::endl;

    // Check if the fit is good
    if (isFitGood) {
        // Append the Gaussian mean and error to the text files
        std::ofstream meanFile("/Users/patsfan753/Desktop/Pi0_Organized/Invariant_Mass_Plots/mean.txt", std::ios::app);
        std::ofstream gaussianErrorFile("/Users/patsfan753/Desktop/Pi0_Organized/Invariant_Mass_Plots/GaussianError.txt", std::ios::app);

        if (meanFile.is_open() && gaussianErrorFile.is_open()) {
            meanFile << "Index " << value << ": " << fitMean << std::endl;
            gaussianErrorFile << "Index " << value << ": " << fitSigma << std::endl;
        } else {
            std::cerr << "Unable to open file(s) for writing Gaussian parameters." << std::endl;
        }

        // Close the files
        meanFile.close();
        gaussianErrorFile.close();

        // Print confirmation message with index
        std::cout << "Fit good. Added Gaussian parameters to files for Index " << value << " - Mean: " << fitMean << ", Error: " << fitSigma << std::endl;
    } else {
        // Print a message indicating no Gaussian parameters were added
        std::cout << "Fit not good for Index " << value << ". No Gaussian parameters added to files." << std::endl;
    }

    
    // Create separate TF1 for Gaussian and Polynomial parts
    TF1 *gaussFit = new TF1("gaussFit", "gaus", fitStart, .28);
    TF1 *polyFit = new TF1("polyFit", "pol4", fitStart, .28);
    
    // Initialize fit parameters based on the histogram
    gaussFit->SetParameter(0, totalFit->GetParameter(0)); // amplitude, around the peak height
    gaussFit->SetParameter(1, totalFit->GetParameter(1)); // mean, centered around the peak
    gaussFit->SetParameter(2, totalFit->GetParameter(2)); // sigma, estimated from giving best qualitative total fit

    
    for (int i = 3; i < 8; i++) {
        polyFit->SetParameter(i - 3, totalFit->GetParameter(i));
    }

    gaussFit->SetLineColor(kOrange+2);
    gaussFit->SetLineStyle(2); // Dashed line
    gaussFit->Draw("SAME");

    polyFit->SetLineColor(kBlue);
    polyFit->SetLineWidth(3);
    polyFit->SetLineStyle(2); // Dotted line
    polyFit->Draw("SAME");

    // Integrate the Gaussian peak from the limits of the fit range to get the total signal
    double signalPlusBackground = totalFit->Integral(fitMean - 2*fitSigma, fitMean+2*fitSigma);

    // Integrate the Polynomial from the limits of the fit range to get the background
    double background = polyFit->Integral(fitMean - 2*fitSigma, fitMean+2*fitSigma);

    // Subtract background from the total to get the net signal
    double netSignal = signalPlusBackground - background;


    // Calculate the signal-to-background ratio
    double signalToBackgroundRatio = netSignal / background;

    // Print the calculated ratio
    std::cout << "Area Under Total Fit: " << signalPlusBackground << std::endl;
    std::cout << "Area Under Background Fit: " << background << std::endl;
    std::cout << "Net Signal: " << netSignal << std::endl;
    std::cout << "Signal-to-Background Ratio: " << signalToBackgroundRatio << std::endl;
    
    
    latex.SetTextSize(0.036);
    latex.DrawLatex(0.43, 0.86, Form("Mean of Gaussian: %.4f GeV", fitMean));
    latex.DrawLatex(0.43, 0.81, Form("Std. Dev. of Gaussian: %.4f GeV", fitSigma));
//    latex.DrawLatex(0.45, 0.76, "Number of Events: 2802");
    latex.DrawLatex(0.43, 0.76, Form("S/B Ratio: %.4f", signalToBackgroundRatio));

    // Get the amplitude parameter from the fit function
    double amplitude = totalFit->GetParameter(0);
    
    // Create vertical lines at fitMean + fitSigma/2 and fitMean - fitSigma/2 and to go to a height some value past the amplitude of gaussian
    
    
    TLine *line1 = new TLine(fitMean + 2*fitSigma, 0, fitMean + 2*fitSigma, 2000);
    TLine *line2 = new TLine(fitMean - 2*fitSigma, 0, fitMean - 2*fitSigma, 2000);
    
    // Set line styles
    line1->SetLineColor(kBlack); // Setting the color to red for better visibility
    line1->SetLineStyle(1); // Dashed line
    line2->SetLineColor(kBlack);
    line2->SetLineStyle(1);


    line1->Draw("same");
    line2->Draw("same");
 
    // Save data
    std::string imageName = "/Users/patsfan753/Desktop/Pi0_Organized/Invariant_Mass_Plots/" + histName + "_fit.png";
    canvas->SaveAs(imageName.c_str());
    
    
    // Clone the original histogram to store the background-subtracted signal
    TH1F *hSignal = (TH1F*)hPi0Mass->Clone("hSignal");

    double binCenter, binContent, bgContent, binError;
    int firstBinSignal = hPi0Mass->FindBin(fitMean - 2*fitSigma);
    int lastBinSignal = hPi0Mass->FindBin(fitMean + 2*fitSigma);

    // Go bin by bin and subtract the background
    for (int i = firstBinSignal; i <= lastBinSignal; ++i) {
        binCenter = hPi0Mass->GetBinCenter(i);
        binContent = hPi0Mass->GetBinContent(i);
        binError = hPi0Mass->GetBinError(i);
        
        // Evaluate the background fit at the bin center
        bgContent = polyFit->Eval(binCenter);
        
        
        bgContent = std::max(bgContent, 0.0);
        
        // Subtract the background from the signal
        hSignal->SetBinContent(i, binContent - bgContent);
        
        hSignal->SetBinError(i, binError);
    }
    

    // Now use IntegralAndError to get the integrated signal and its error
    double signalYield, signalError;
    // Now use IntegralAndError to get the integrated signal and its error
    signalYield = hSignal->IntegralAndError(firstBinSignal, lastBinSignal, signalError, "");

    
    // Debug print to check the status of isFitGood
    std::cout << "isFitGood status before appending to array: " << (isFitGood ? "true" : "false") << std::endl;

    // Check if the fit is good
    if (isFitGood) {
        // Append the values to the text files
        std::ofstream yieldFile("/Users/patsfan753/Desktop/Pi0_Organized/Invariant_Mass_Plots/signalYield.txt", std::ios::app);
        std::ofstream errorFile("/Users/patsfan753/Desktop/Pi0_Organized/Invariant_Mass_Plots/signalError.txt", std::ios::app);

        if (yieldFile.is_open() && errorFile.is_open()) {
            yieldFile << "Index " << value << ": " << signalYield << std::endl;
            errorFile << "Index " << value << ": " << signalError << std::endl;
        } else {
            std::cerr << "Unable to open file(s) for writing." << std::endl;
        }

        // Close the files
        yieldFile.close();
        errorFile.close();

        // Print confirmation message with index
        std::cout << "Fit good. Added to files for Index " << value << " - Signal Yield: " << signalYield << ", Signal Error: " << signalError << std::endl;
    } else {
        // Print a message indicating no values were added
        std::cout << "Fit not good for Index " << value << ". No values added to files." << std::endl;
    }
    
    // Print the yield and its error
    std::cout << "Signal Yield: " << signalYield << " +/- " << signalError << std::endl;
}
