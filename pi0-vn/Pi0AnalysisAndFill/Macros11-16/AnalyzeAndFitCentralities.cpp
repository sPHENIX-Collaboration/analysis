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

TF1* CreateFit(TH1F* hPi0Mass, double& fitStart, double& fitEnd) {
    int binThreshold = 50;
    int firstBinAboveThreshold = 0;
    for (int i = 1; i <= hPi0Mass->GetNbinsX() - 1; ++i) {
        double derivative = hPi0Mass->GetBinContent(i + 1) - hPi0Mass->GetBinContent(i);
        if (derivative > binThreshold && firstBinAboveThreshold == 0) {
            firstBinAboveThreshold = i;
            break;
        }
    }
    fitStart = hPi0Mass->GetBinLowEdge(firstBinAboveThreshold);
    fitEnd = 0.52;

    TF1* fit = new TF1("totalFit", "gaus(0) + pol4(3)", fitStart, fitEnd);

    int bin1 = hPi0Mass->GetXaxis()->FindBin(0.1);
    int bin2 = hPi0Mass->GetXaxis()->FindBin(0.2);
    int maxBin = bin1;
    double maxBinContent = hPi0Mass->GetBinContent(bin1);
    for (int i = bin1 + 1; i <= bin2; ++i) {
        if (hPi0Mass->GetBinContent(i) > maxBinContent) {
            maxBinContent = hPi0Mass->GetBinContent(i);
            maxBin = i;
        }
    }

    double maxBinCenter = hPi0Mass->GetXaxis()->GetBinCenter(maxBin);
    fit->SetParameter(0, maxBinContent);
    fit->SetParameter(1, maxBinCenter);
    double sigmaEstimate = 0.02;
    fit->SetParameter(2, sigmaEstimate);
    fit->SetParLimits(1, maxBinCenter - sigmaEstimate, maxBinCenter + sigmaEstimate);

    hPi0Mass->Fit(fit, "R+");
    return fit;
}

void AnalyzeAndFitCentralities() {
    TFile* file = new TFile("/Users/patsfan753/Desktop/Pi0_Organized/RootHistOutput/HistOutputCutCombos.root", "READ");
    int value = 11;
    std::string histName = "hPi0Mass_" + std::to_string(value);
    TH1F* hPi0Mass = (TH1F*)file->Get(histName.c_str());
    hPi0Mass->GetYaxis()->SetRangeUser(0, 5000);
    

    double fitStart, fitEnd;
    TF1* totalFit = CreateFit(hPi0Mass, fitStart, fitEnd);
    
    TCanvas *canvas = new TCanvas("canvas", "Pi0 Mass Distribution", 900, 600);
    hPi0Mass->SetMarkerStyle(20);
    hPi0Mass->SetMarkerSize(1.0);
    hPi0Mass->SetMarkerColor(kBlack);
    hPi0Mass->Draw("PE");
    Range selectedRange = ranges[value];
    std::ostringstream mbdStream;
    std::ostringstream ptStream;


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

    double fitMean = totalFit->GetParameter(1);
    double fitSigma = totalFit->GetParameter(2);
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
    polyFit->SetLineStyle(2); // Dotted line
    polyFit->Draw("SAME");
    double signalPlusBackground = totalFit->Integral(fitMean - 2*fitSigma, fitMean+2*fitSigma);
    double background = polyFit->Integral(fitMean - 2*fitSigma, fitMean+2*fitSigma);
    double netSignal = signalPlusBackground - background;
    double signalToBackgroundRatio = netSignal / background;
    latex.SetTextSize(0.036);
    latex.DrawLatex(0.43, 0.86, Form("Mean of Gaussian: %.4f GeV", fitMean));
    latex.DrawLatex(0.43, 0.81, Form("Std. Dev. of Gaussian: %.4f GeV", fitSigma));
    latex.DrawLatex(0.43, 0.76, Form("S/B Ratio: %.4f", signalToBackgroundRatio));
    double amplitude = totalFit->GetParameter(0);
    TLine *line1 = new TLine(fitMean + 2*fitSigma, 0, fitMean + 2*fitSigma, maxBinContent+200);
    TLine *line2 = new TLine(fitMean - 2*fitSigma, 0, fitMean - 2*fitSigma, maxBinContent+200);
    line1->SetLineColor(kBlack); // Setting the color to red for better visibility
    line1->SetLineStyle(1); // Dashed line
    line2->SetLineColor(kBlack);
    line2->SetLineStyle(1);
    line1->Draw("same");
    line2->Draw("same");

    std::string imageName = "/Users/patsfan753/Desktop/Pi0_Organized/Invariant_Mass_Plots/" + histName + "_fit.png";
    canvas->SaveAs(imageName.c_str());
    
    TH1F *hSignal = (TH1F*)hPi0Mass->Clone("hSignal");
    double binCenter, binContent, bgContent;
    int firstBinSignal = hPi0Mass->FindBin(fitMean - 2*fitSigma);
    int lastBinSignal = hPi0Mass->FindBin(fitMean + 2*fitSigma);
    for (int i = firstBinSignal; i <= lastBinSignal; ++i) {
        binCenter = hPi0Mass->GetBinCenter(i);
        binContent = hPi0Mass->GetBinContent(i);
        bgContent = polyFit->Eval(binCenter);
        bgContent = std::max(bgContent, 0.0);
        hSignal->SetBinContent(i, binContent - bgContent);
    }
    double signalYield, signalError;
    signalYield = hSignal->IntegralAndError(firstBinSignal, lastBinSignal, signalError, "");

    std::ofstream yieldFile("/Users/patsfan753/Desktop/Pi0_Organized/Invariant_Mass_Plots/signalYieldTests.txt", std::ios::app);
    std::ofstream errorFile("/Users/patsfan753/Desktop/Pi0_Organized/Invariant_Mass_Plots/signalErrorTests.txt", std::ios::app);
    if (yieldFile.is_open() && errorFile.is_open()) {
        yieldFile << "Index " << value << ": " << signalYield << std::endl;
        errorFile << "Index " << value << ": " << signalError << std::endl;
    } else {
        std::cerr << "Unable to open file(s) for writing." << std::endl;
    }
    yieldFile.close();
    errorFile.close();
}
