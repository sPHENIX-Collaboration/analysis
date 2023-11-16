#include <TGraphErrors.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TAxis.h>
#include <TH1F.h>
#include <iostream>
#include <fstream>
#include <string>

void PlotGaussianMeanError() {
    // Arrays to hold data points and errors
    const int nPoints = 12; // Total number of points in each file
    double yield[nPoints], error[nPoints];
    // Adjust the xPoints to the center of the labeled bins
    double xPoints[nPoints] = {2.5, 3.5, 4.5, 2.5, 3.5, 4.5, 2.5, 3.5, 4.5, 2.5, 3.5, 4.5};

    // Read data from the files
    std::ifstream yieldFile("/Users/patsfan753/Desktop/Pi0_Organized/Invariant_Mass_Plots/mean.txt");
    std::ifstream errorFile("/Users/patsfan753/Desktop/Pi0_Organized/Invariant_Mass_Plots/GaussianError.txt");
    std::string line;
    int index;
    double value;

    if (!yieldFile.is_open() || !errorFile.is_open()) {
        std::cerr << "Error opening files." << std::endl;
        return;
    }

    // Reading signal yield
    while (getline(yieldFile, line)) {
        sscanf(line.c_str(), "Index %d: %lf", &index, &value);
        yield[index] = value;
    }

    // Reading signal error
    while (getline(errorFile, line)) {
        sscanf(line.c_str(), "Index %d: %lf", &index, &value);
        error[index] = value;
    }

    yieldFile.close();
    errorFile.close();


     // Create graphs
    TGraphErrors *graphs[4];
    int indices[4][3] = {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {9, 10, 11}};
    int colors[4] = {kRed, kBlue, kGreen, kMagenta}; // Colors for different graphs
    const char* labels[] = {
        "60-100%",
        "40-60%",
        "20-40%",
        "0-20 %"
    };

    // Create a dummy histogram for x-axis labels with the adjusted range
    TH1F *hDummy = new TH1F("hDummy", "", 4, 1.5, 5.5);
    hDummy->GetYaxis()->SetTitle("Gaussian Mean");
    hDummy->GetXaxis()->CenterTitle(true);
    hDummy->SetTitle("Gaussian Mean of #pi^{0}");
    hDummy->GetXaxis()->SetTitle("#pi^{0} p_{T} [GeV]");
    hDummy->GetXaxis()->SetTitleOffset(1.1);
    
    // Adjust the x-axis to show the desired tick marks at 2, 3, 4, and 5
    hDummy->GetXaxis()->SetLimits(2, 5); // This sets the user-defined limits for the axis
    hDummy->GetXaxis()->SetLabelSize(0.04); // You can adjust the size as needed
    // Manually setting the x-axis labels
    hDummy->GetXaxis()->SetNdivisions(004); // 4 primary divisions, 0 secondary divisions





    
    // Inside the loop where you draw the graphs, ensure you set the points correctly
    for (int i = 0; i < 4; ++i) {
        graphs[i] = new TGraphErrors(3);
        for (int j = 0; j < 3; ++j) {
            int idx = indices[i][j];
            // The x-value is now set to the correct centered position
            graphs[i]->SetPoint(j, xPoints[idx], yield[idx]);
//            graphs[i]->SetPointError(j, 0, error[idx]);
            graphs[i]->SetLineColor(colors[i]);
            graphs[i]->SetMarkerColor(colors[i]);
            graphs[i]->SetMarkerSize(1.2);
            
        }
    }

    // Setting up canvas
    TCanvas *c = new TCanvas("c", "Overlayed Curves", 800, 600);
    c->cd(); // Ensure we are drawing on the canvas
//    c->SetLogy();
    double minYield = *std::min_element(yield, yield + nPoints); // Find the minimum yield
    double maxYield = *std::max_element(yield, yield + nPoints); // Find the maximum yield
    double range = maxYield - minYield; // Calculate the range of your data
    double padding = range * 0.1; // Add 10% padding at the top and bottom

    // Set the y-axis limits, including the padding
    hDummy->GetYaxis()->SetRangeUser(.11, .24);
    hDummy->SetStats(0); // Remove the statistics box
    hDummy->GetYaxis()->SetTitle("Gaussian Mean");
    hDummy->Draw(); // Draw the dummy histogram to define axis ranges


    TLegend *leg = new TLegend(0.14, 0.68, 0.34, 0.88);
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
    latex.SetTextSize(0.033);
    latex.DrawLatex(0.68, 0.86, "Cuts (Inclusive):");
    latex.DrawLatex(0.68, 0.82, "#Delta R #geq 0.08");
    latex.DrawLatex(0.68, 0.78, "Asymmetry < 0.5");
    latex.DrawLatex(0.68, 0.74, "#chi^{2} < 4");
    latex.DrawLatex(0.68, 0.7, "Cluster E #geq 1.0 GeV");
    
    c->Update(); // Update the canvas

    // Save the canvas as a PNG file
    c->SaveAs("/Users/patsfan753/Desktop/Pi0_Organized/GaussianMean_curves1GeV.png");
    
    
    
    // Now, start creating the signalError plot
    TCanvas *cError = new TCanvas("cError", "Signal Error Plot", 800, 600);
    cError->cd(); // Ensure we are drawing on the new canvas

    // Create a new dummy histogram for the signalError x-axis labels
    TH1F *hDummyError = new TH1F("hDummyError", "", 4, 1.5, 5.5);
    hDummyError->GetYaxis()->SetTitle("Gaussian Error");
    hDummyError->GetXaxis()->CenterTitle(true);
    hDummyError->GetXaxis()->SetTitle("#pi^{0} p_{T} [GeV]");
    hDummyError->GetXaxis()->SetTitleOffset(1.1);
    hDummyError->SetTitle("Error of #pi^{0} Gaussian Mean");
    hDummyError->GetYaxis()->SetRangeUser(0, .06);
    // Adjust the x-axis to show the desired tick marks at 2, 3, 4, and 5
    hDummyError->GetXaxis()->SetLimits(2, 5); // This sets the user-defined limits for the axis
    hDummyError->GetXaxis()->SetLabelSize(0.04);
    hDummyError->GetXaxis()->SetNdivisions(004); // 4 primary divisions, 0 secondary divisions


    hDummyError->SetStats(0); // Remove the statistics box
    hDummyError->Draw(); // Draw the dummy histogram to define axis ranges

    // Create graphs for signalError
    TGraphErrors *errorGraphs[4];
    for (int i = 0; i < 4; ++i) {
        errorGraphs[i] = new TGraphErrors(3);
        for (int j = 0; j < 3; ++j) {
            int idx = indices[i][j];
            errorGraphs[i]->SetPoint(j, xPoints[idx], error[idx]); // Here, use the absolute error
            errorGraphs[i]->SetPointError(j, 0, 0); // No error in x, no additional error in y
            errorGraphs[i]->SetLineColor(colors[i]);
            errorGraphs[i]->SetMarkerColor(colors[i]);
            errorGraphs[i]->SetMarkerSize(1.2);
            errorGraphs[i]->SetMarkerStyle(20 + i);
            errorGraphs[i]->Draw("P SAME");
        }
    }

    // Create legends at the top right of the canvas for the signalError plot
    TLegend *legError = new TLegend(0.12, 0.68, 0.32, 0.88);
    legError->SetHeader("Centrality:","C");
    legError->SetMargin(0.4);
    legError->SetBorderSize(0);
    legError->SetTextSize(0.04);
    for (int i = 0; i < 4; ++i) {
        legError->AddEntry(errorGraphs[i], labels[i], "p");
    }
    legError->Draw();

    cError->Update(); // Update the canvas

    // Save the error plot as a PNG file
    cError->SaveAs("/Users/patsfan753/Desktop/Pi0_Organized/GaussianError_curves1Gev.png");

}

