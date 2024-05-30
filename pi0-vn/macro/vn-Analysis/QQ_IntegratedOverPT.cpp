#include <TFile.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TLegend.h>
#include "sPhenixStyle.h"
#include "sPhenixStyle.C"

//void generate2Dhists() {
//    // Define the run numbers
//    std::vector<TString> runNumbers = {"23020", "23671", "23672", "23676", "23681", "23682",
//                                       "23687", "23690", "23693", "23694", "23695", "23696",
//                                       "23697", "23699", "23702", "23714", "23726", "23727",
//                                       "23728", "23735", "23737", "23738", "23739", "23740",
//                                       "23743", "23745", "23746", "23536", "23539", "23543",
//                                       "23547", "23550", "23552", "23557", "23558", "23561",
//                                       "23562", "23563", "23566", "23568", "23572", "23574",
//                                       "23575", "23582", "23590", "23592", "23594", "23604",
//                                       "23605", "23617", "23618", "23619"};
//
//    // Define the centrality folders
//    std::vector<TString> centralities = {"50-60", "40-50", "30-40", "20-30", "10-20", "0-10"};
//
//    // Define the histogram names
//    std::vector<TString> histNames = {"h2Q2N2_", "h2Q2S2_"};
//
//    // Base directory for the output PNGs
//    TString baseOutputDir = "/Users/patsfan753/Desktop/p015/IntegratedOverPt/QvectorAnalysis";
//
//    for (const auto& run : runNumbers) {
//        // Open the root file
//        TString filePath = baseOutputDir + "/" + run + "/test-" + run + ".root";
//        TFile *file = TFile::Open(filePath);
//        if (!file || file->IsZombie()) {
//            std::cerr << "Error opening file: " << filePath << std::endl;
//            continue;
//        }
//
//        // Loop over centrality folders
//        for (const auto& centrality : centralities) {
//            TString centralityPath = "QA/MBD/" + centrality;
//
//            // Loop over histogram names
//            for (const auto& histName : histNames) {
//                TString histPath = centralityPath + "/" + histName + centrality;
//                TH2* hist = dynamic_cast<TH2*>(file->Get(histPath));
//                if (!hist) {
//                    std::cerr << "Error retrieving histogram: " << histPath << std::endl;
//                    continue;
//                }
//
//                // Create a canvas and draw the histogram
//                TCanvas *canvas = new TCanvas("canvas", "canvas", 800, 600);
//                hist->Draw("COLZ");
//                hist->GetXaxis()->SetRangeUser(-0.3, 0.3);
//                hist->GetYaxis()->SetRangeUser(-0.3, 0.3);
//
//                TLatex latex;
//                latex.SetNDC();
//                latex.SetTextSize(0.04);
//                latex.DrawLatex(0.22, 0.92, "Run: " + run);
//                latex.DrawLatex(0.22, 0.88, "Centrality: " + centrality + "%");
//
//                // Get the number of entries and add to the canvas
//                TString entriesText = TString::Format("Entries: %.0f", hist->GetEntries());
//                latex.DrawLatex(0.22, 0.84, entriesText);
//
//                // Save the histogram as a PNG file
//                TString outputDir = baseOutputDir + "/All/";
//                TString outputPath = outputDir + histName + centrality + "_" + run + ".png";
//                canvas->SaveAs(outputPath);
//
//                // Clean up
//                delete canvas;
//            }
//        }
//
//        // Close the root file
//        file->Close();
//        delete file;
//    }
//
//    std::cout << "Processing completed." << std::endl;
//}

void QQ_IntegratedOverPT() {
    gROOT->LoadMacro("sPhenixStyle.C");
    SetsPhenixStyle();
    // Open the ROOT file
    TFile *file = new TFile("/Users/patsfan753/Desktop/p015/IntegratedOverPt/Q-vec-corr-anaType-1-ana412-2023p015-v2.root");

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
            TLegend *legend = new TLegend(0.62, 0.71, 0.82, 0.91); // Bottom right corner
            legend->SetBorderSize(0); // No border
            legend->SetFillStyle(0);  // Transparent
            legend->SetTextSize(.042);  // Transparent
            

            // Descriptions for different orders
            const char* descriptions[] = {"Uncorrected", "Recentered", "Flattened"};



            // Colors for different orders
            int colors[] = {kRed, kCyan - 3, kBlack};

            TH1F* histograms[3];
            // Loop over histogram orders
            for (int i = 0; i < 3; i++) {
                histograms[i] = (TH1F*)file->Get(Form("%s/hPsi2_%s_%s_%d", centrality, dir, centrality, i));
                histograms[i]->SetLineColor(colors[i]);
                histograms[i]->SetLineWidth(2);
                histograms[i]->SetStats(false);

                // Set individual titles for the histograms
                histograms[i]->SetTitle(titles[titleIndex]);
            }

            
            // Draw the histograms after normalization
            for (int i = 0; i < 3; i++) {
                if (i == 0) {
                    histograms[i]->Draw("HIST");
                } else {
                    histograms[i]->Draw("HIST SAME");
                }

                legend->AddEntry(histograms[i], descriptions[i], "l");
            }

            legend->Draw();
            TLegend *leg = new TLegend(0.2, 0.75, 0.4, 0.9);
            leg->SetFillStyle(0);
            leg->SetTextSize(0.042);
            leg->AddEntry("", "#it{#bf{sPHENIX}} Preliminary", "");
            leg->AddEntry("", "Au+Au #sqrt{s_{NN}} = 200 GeV", "");
            leg->AddEntry("", Form("Centrality = %s%%", centrality), "");
            leg->Draw("same");


            c->SaveAs(Form("/Users/patsfan753/Desktop/p015/IntegratedOverPt/QQdistributions/plot_%s_%s.png", centrality, dir));

            // Increment title index after each plot
            titleIndex++;
        }
    }
    file->Close();
//    generate2Dhists();
}

