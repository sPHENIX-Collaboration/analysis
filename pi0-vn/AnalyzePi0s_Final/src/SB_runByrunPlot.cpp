#include <TGraphErrors.h>
#include <TCanvas.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

void SB_runByrunPlot() {
    std::vector<std::string> RunNumbers = {"23020", "23671", "23672", "23676", "23681", "23682", "23687", "23690", "23693", "23694", "23695", "23696", "23697", "23699", "23702", "23714", "23726", "23727", "23728", "23735", "23737", "23738", "23739", "23740", "23743", "23745", "23746"};
    std::string readCSVBasePath = "/Users/patsfan753/Desktop/vN_AnalysisFinal/data/PlotByPlotOutput_FromFits/p009/RunByRun/";

    for (int idx = 0; idx <= 17; ++idx) { // Loop over indices
        std::vector<double> x, y, yerr; // Vectors for x values, y values, and y errors
        std::vector<std::string> xLabels; // Vector for storing run numbers as x-axis labels

        int pointIndex = 0;
        for (const auto& runNumber : RunNumbers) {
            std::string filePath = readCSVBasePath + "fit-stats-" + runNumber + ".csv";
            std::ifstream file(filePath);
            std::string line;
            
            std::getline(file, line);//skip header

            while (std::getline(file, line)) {
                std::istringstream ss(line);
                std::string token;
                std::vector<std::string> tokens;
                while (std::getline(ss, token, ',')) {
                    tokens.push_back(token);
                }

                if (tokens.size() > 8) {
                    try {
                        if (std::stoi(tokens[0]) == idx) {
                            double sb = std::stod(tokens[9]);
                            double sb_error = std::stod(tokens[10]);
                            // Check for infinity or NaN and replace with 0
                            if (std::isinf(sb) || std::isnan(sb)) sb = 0.0;
                            if (std::isinf(sb_error) || std::isnan(sb_error)) sb_error = 0.0;

                            x.push_back(static_cast<double>(x.size() + 1)); // Incremental value for x
                            y.push_back(sb);
                            yerr.push_back(sb_error);
                            xLabels.push_back(runNumber); // Add run number for labeling

                            // Print statement for debugging
                            std::cout << "Reading Index: " << idx << ", Run Number: " << runNumber
                                      << ", S/B: " << sb << ", S/Berror: " << sb_error << std::endl;
                            break; // Found the correct index, move to next file
                        }
                    } catch (const std::invalid_argument& e) {
                        std::cout << "Invalid argument for stoi: " << tokens[0] << " in file " << filePath << std::endl;
                    }
                } else {
                    std::cout << "Not enough tokens found or index mismatch. Tokens found: " << tokens.size() << std::endl;
                }
            }
        }

        TCanvas *c1 = new TCanvas(Form("c1_%d", idx), Form("Index %d", idx), 800, 600);
        c1->SetRightMargin(0.1);
        c1->SetLeftMargin(0.1);

        c1->SetGrid(); // This will add a grid to the canvas
        TGraphErrors *graph = new TGraphErrors(x.size(), &x[0], &y[0], nullptr, &yerr[0]);
        graph->SetTitle(Form("S/B vs Run Number for Index %d;Run Number;S/B", idx));
        graph->GetXaxis()->SetTitleOffset(1.5); // Adjust this value as needed

        graph->SetMarkerStyle(22);
        graph->SetMarkerSize(1.3);
        graph->SetMarkerColor(kBlue);
        graph->SetLineColor(kBlue);
        // Automatically adjust y-axis to fit all data points with a buffer
        double y_min = *std::min_element(y.begin(), y.end()) - .05; // Add buffer below min
        double y_max = *std::max_element(y.begin(), y.end()) + .05; // Add buffer above max
        graph->GetYaxis()->SetRangeUser(y_min, y_max);
        graph->Draw("AP");
        double extendFactor = 0.2; // This factor controls how much we extend the axis range by (e.g., 20%)
        double x_low = 0.5 - extendFactor; // Assuming your x values start from 1, we reduce this by extendFactor
        double x_high = x.size() + 0.5 + extendFactor; // Add extendFactor to the highest x value

        graph->GetXaxis()->SetLimits(x_low, x_high); // Set the new limits for the x-axis

        graph->GetXaxis()->SetNdivisions(x.size(), kFALSE); // kFALSE disables the secondary tick marks
        
        // Adjusting x-axis to have run numbers as labels
        for (size_t i = 0; i < x.size(); ++i) {
            graph->GetXaxis()->SetBinLabel(graph->GetXaxis()->FindBin(i+1), xLabels[i].c_str());
            TLine *line = new TLine(x[i], y_min, x[i], y_max); // Lines at each x position from bottom to top of plot
            line->SetLineColor(kGray); // Set the line color to gray (or any color you prefer)
            line->SetLineStyle(2); // Set the line style to dashed (2)
            line->Draw("same"); // Draw the line on the same canvas as the graph
        }
        
        double y_average = std::accumulate(y.begin(), y.end(), 0.0) / y.size();
        // Find the value that deviates the most from the average (either max or min)
        double max_deviation = std::max(abs(*std::max_element(y.begin(), y.end()) - y_average), abs(*std::min_element(y.begin(), y.end()) - y_average));
        double percent_difference = (max_deviation / y_average) * 100;
        
        // Get the x-axis range from the graph
        double x_min = graph->GetXaxis()->GetXmin();
        double x_max = graph->GetXaxis()->GetXmax();

        // After creating the graph and before the loop drawing lines
        TLine *averageLine = new TLine(x_min, y_average, x_max, y_average);
        averageLine->SetLineColor(kRed); // Red color for the average line
        averageLine->SetLineWidth(2); // Set line width
        averageLine->Draw("same");

        // For average S/B and percent difference text box
        char infoText[100];
        sprintf(infoText, "Avg S/B: %.3f, Max %% Diff: %.2f%%", y_average, percent_difference);
        TLatex *text = new TLatex(0.12, 0.8, infoText); // Adjust these coordinates as needed
        text->SetTextFont(42);
        text->SetTextSize(0.04);
        text->SetNDC(); // To use relative positioning instead of the axis scale
        text->Draw();
        
        // Creating a dummy TGraph for the legend entry for the average line
        TGraph *gr_average = new TGraph();
        gr_average->SetLineColor(kRed);
        gr_average->SetLineWidth(2);
        
        TLegend *leg = new TLegend(0.7, 0.75, 0.9, 0.9); // Adjust these numbers to position your legend
        leg->SetBorderSize(1); // Set border size (set to 0 for no border)
        leg->SetFillColor(0); // Set fill color (set to 0 for transparent)
        leg->SetTextFont(42); // Set the text font to match your style
        leg->SetTextSize(0.03); // Set the text size
        leg->AddEntry(graph, "S/B", "pe"); // Add your graph to the legend with the label 'S/B'
        leg->AddEntry(gr_average, "Average S/B", "l"); // Add the dummy graph as a line to the legend
        leg->Draw();
    
        
        std::string savePath = Form("/Users/patsfan753/Desktop/vN_AnalysisFinal/plotOutput/RunByRunSb_p009/SBPlot_Index_%d.png", idx);
        c1->SaveAs(savePath.c_str());

        delete c1; // Clean up
    }
}
