#include <TFile.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TLegend.h>

void QQ_flattness_Overlay() {
    // Open the ROOT file
    TFile *file = new TFile("/Users/patsfan753/Desktop/Q-vec-corr-ana402-2023p009-z-10.root");

    // Define the centrality folders
    const char* centralities[] = {"0-20", "20-40", "40-60"};

    // Titles for the histograms
    /*
     Replace 3's with 2's for n = 2 corrections
     */
    const char* titles[] = {
        "3#Psi_{3}^{S}, Centrality: 0-20%",
        "3#Psi_{3}^{N}, Centrality: 0-20%",
        "3#Psi_{3}^{S}, Centrality: 20-40%",
        "3#Psi_{3}^{N}, Centrality: 20-40%",
        "3#Psi_{3}^{S}, Centrality: 40-60%",
        "3#Psi_{3}^{N}, Centrality: 40-60%"
    };

    int titleIndex = 0; // Index to access titles

    // Loop over each centrality folder
    for (const auto& centrality : centralities) {
        // Define histograms for South and North
        const char* directions[] = {"S", "N"};
        
        for (const auto& dir : directions) {
            // Create a canvas for each plot
            TCanvas *c = new TCanvas(Form("c_%s_%s", centrality, dir), titles[titleIndex], 800, 600);
            c->SetLeftMargin(0.15);
            // Instantiate the legend with coordinates for bottom right corner
            TLegend *legend = new TLegend(0.74, 0.69, 0.94, 0.89); // Bottom right corner
            legend->SetBorderSize(0); // No border
            legend->SetFillStyle(0);  // Transparent
            
            legend->SetHeader("#bf{Corrections}", "L"); // "C" centers the header

            // Colors for different orders
            int colors[] = {kRed, kBlue, kGreen+4};

            // Loop over histogram orders
            for (int i = 0; i < 3; i++) {
                //replace 3 with 2 for n = 2
                TH1F *hist = (TH1F*)file->Get(Form("%s/hPsi3_%s_%s_%d", centrality, dir, centrality, i));
                hist->SetLineColor(colors[i]);
                hist->SetLineWidth(2);
                hist->SetStats(false);

                // Set individual titles for the histograms
                hist->SetTitle(titles[titleIndex]);

                if (i == 0) {
                    hist->Draw();
                } else {
                    hist->Draw("same");
                }

                legend->AddEntry(hist, Form("Order %d", i), "l");
            }

            legend->Draw();
            c->SaveAs(Form("/Users/patsfan753/Desktop/plot_%s_%s.png", centrality, dir));
            
            // Increment title index after each plot
            titleIndex++;
        }
    }
    file->Close();
}
