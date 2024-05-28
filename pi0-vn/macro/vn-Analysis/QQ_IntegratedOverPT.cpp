#include <TFile.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TLegend.h>
#include "../utils/sPhenixStyle.h"
#include "../utils/sPhenixStyle.C"
void QQ_IntegratedOverPT() {
    gROOT->LoadMacro("../utils/sPhenixStyle.C");
    SetsPhenixStyle();
    // Open the ROOT file
    TFile *file = new TFile("/Users/patsfan753/Desktop/p015/MonteCarlo/Integrated_OverPT/Q-vec-corr-anaType-1-hijing-0-20fm.root");

    // Define the centrality folders
    const char* centralities[] = {"0-10", "10-20", "20-30", "30-40", "40-50", "50-60"};
 //   const char* centralities[] = {"0-4.88", "4.88-6.81", "6.81-8.40", "8.40-9.71", "9.71-10.81", "10.81-11.84"};

    // Titles for the histograms
    /*
     Replace 3's with 2's for n = 2 corrections
     */
    const char* titles[] = {
        "2#Psi_{2}^{S}, Centrality: 0-10%",
        "2#Psi_{2}^{N}, Centrality: 0-10%",
        "2#Psi_{2}^{S}, Centrality: 10-20%",
        "2#Psi_{2}^{N}, Centrality: 10-20%",
        "2#Psi_{2}^{S}, Centrality: 20-30%",
        "2#Psi_{2}^{N}, Centrality: 20-30%",
        "2#Psi_{2}^{S}, Centrality: 30-40%",
        "2#Psi_{2}^{N}, Centrality: 30-40%",
        "2#Psi_{2}^{S}, Centrality: 40-50%",
        "2#Psi_{2}^{N}, Centrality: 40-50%",
        "2#Psi_{2}^{S}, Centrality: 50-60%",
        "2#Psi_{2}^{N}, Centrality: 50-60%"
        
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
            TLegend *legend = new TLegend(0.5, 0.69, 0.7, 0.89); // Bottom right corner
            legend->SetBorderSize(0); // No border
            legend->SetFillStyle(0);  // Transparent
            
            legend->SetHeader("#bf{Corrections}", "L"); // "C" centers the header

            // Colors for different orders
            int colors[] = {kRed, kBlue, kGreen+4};

            // Loop over histogram orders
            for (int i = 0; i < 3; i++) {
                //replace 3 with 2 for n = 2
                TH1F *hist = (TH1F*)file->Get(Form("%s/hPsi2_%s_%s_%d", centrality, dir, centrality, i));
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
            c->SaveAs(Form("/Users/patsfan753/Desktop/p015/MonteCarlo/Integrated_OverPT/p015/QQdistributions/plot_%s_%s.png", centrality, dir));
            
            // Increment title index after each plot
            titleIndex++;
        }
    }
    file->Close();
}

