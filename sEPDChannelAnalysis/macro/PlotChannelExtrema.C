#include <TFile.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <vector>
#include <sPhenixStyle.C>
#include <sPhenixStyle.h>

void PlotChannelExtrema(const char* inputFile = "channel_analysis.root", 
                       const char* outputFile = "channel_extrema_plots.pdf") {
    // Open input file
    TFile* f = TFile::Open(inputFile, "READ");
    if (!f || f->IsZombie()) {
        std::cerr << "Error opening input file: " << inputFile << std::endl;
        return;
    }

    // Create output canvas and PDF
    TCanvas canvas("canvas", "Channel Extrema", 800, 600);
    canvas.Print(Form("%s[", outputFile)); // Open multi-page PDF

    const int numChannels = 744;
    TLegend legend(0.7, 0.7, 0.9, 0.9);
    
    for (int ch = 0; ch < numChannels; ch++) {
        // Get histograms
        TH1F* hmax = dynamic_cast<TH1F*>(f->Get(Form("ch%d_max_hist", ch)));
        TH1F* hmin = dynamic_cast<TH1F*>(f->Get(Form("ch%d_min_hist", ch)));

        // Skip channels with missing histograms
        if (!hmax || !hmin) continue;

        // Configure histograms
        hmax->SetLineColor(kRed);
        hmax->SetLineWidth(2);
        hmax->SetTitle(Form("Channel %d Extreme MPV Histograms;ADC;Counts", ch));
        
        hmin->SetLineColor(kBlue);
        hmin->SetLineWidth(2);
        hmin->SetLineStyle(2);

        // Find common scale
        double ymax = std::max(hmax->GetMaximum(), hmin->GetMaximum());
        hmax->SetMaximum(ymax * 1.1);

        // Draw and configure
        canvas.cd();
        hmax->Draw("HIST");
        hmin->Draw("HIST SAME");

        // Add legend
        legend.Clear();
        legend.AddEntry(hmax, "Max MPV", "L");
        legend.AddEntry(hmin, "Min MPV", "L");
        legend.Draw();

        // Save to PDF
        canvas.Print(outputFile);

        // Progress indicator
        if (ch % 50 == 0) {
            std::cout << "Processed channel " << ch << "/" << numChannels-1 
                      << " (" << (ch*100/numChannels) << "%)\n";
        }
    }

    // Close PDF
    canvas.Print(Form("%s]", outputFile));
    f->Close();
    
    std::cout << "Saved plots to: " << outputFile << std::endl;
}