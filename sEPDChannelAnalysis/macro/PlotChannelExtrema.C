#include <TFile.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <vector>
#include <sPhenixStyle.C>
#include <sPhenixStyle.h>

void PlotChannelExtrema(const char* inputFile = "/sphenix/user/ecroft/channel_analysis_zs20.root", 
                       const char* outputFile = "/sphenix/user/ecroft/channel_extrema_plots_zs20.pdf") {
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
        TF1* fmax = dynamic_cast<TF1*>(f->Get(Form("ch%d_max_fit", ch)));
        TF1* fmin = dynamic_cast<TF1*>(f->Get(Form("ch%d_min_fit", ch)));

        // Skip channels with missing histograms
        if (!hmax || !hmin) {
            std::cout << "Skipping channel " << ch << " — missing hmax or hmin.\n";
            continue;
        }
        if (!fmin || ! fmax) {
            std::cout << "Skipping channel " << ch << " — missing fmin or fmax.\n";
            continue;
        }
        // Configure histograms
        hmax->SetLineColor(kRed);
        hmax->SetLineWidth(2);
        hmax->SetTitle(Form("Channel %d Extreme MPV Histograms;ADC;Counts", ch));
        
        hmin->SetLineColor(kBlue);
        hmin->SetLineWidth(2);
        hmin->SetLineStyle(2);

	fmax->SetLineColor(kGreen);
        fmin->SetLineColor(kMagenta);

        // Find common scale
        double ymax = std::max(hmax->GetMaximum(), hmin->GetMaximum());
        hmax->SetMaximum(ymax * 1.1);

        // Draw and configure
        canvas.cd();
        hmax->Draw("HIST");
        hmin->Draw("HIST SAME");
        fmax->Draw("SAME");
        fmin->Draw("SAME");

        // Add legend
        legend.Clear();
        legend.AddEntry(hmax, "Max MPV", "L");
        legend.AddEntry(hmin, "Min MPV", "L");
        legend.AddEntry(fmax, "Max MPV fit", "L");
        legend.AddEntry(fmin, "Min MPV fit", "L");
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
