#include "sPhenixStyle.h"
#include "sPhenixStyle.C"


#include <TFile.h>
#include <TH2F.h>
#include <TCanvas.h>
#include <vector>
#include <string>
/*
 This macro generates distributions for each run number in respective MB/Central triggered event folders for the following:
 --EMCal MBD Correlations
 --EMCal MBD Correlations where entries have negative calorimeter energy
 --MBD Charge, centrality correlations
 --1D Centrality Distributions
 --z vertex distributions
 --Normalized eta-phi 2D cluster energy distributions
 */
void GenerateAdditionalQA() {
    gROOT->LoadMacro("sPhenixStyle.C");
    SetsPhenixStyle();
    std::vector<std::string> runNumbers_MB =    {
        "23020", "23671", "23672", "23676", "23681", "23682", "23687", "23690", "23693", "23694",
        "23695", "23696", "23697", "23699", "23702", "23714", "23726", "23727", "23728", "23735",
        "23737", "23738", "23739", "23740", "23743", "23745", "23746"
    };
    std::vector<std::string> runNumbers_Central =    {
        "23536",  "23539", "23543", "23547", "23550",  "23552", "23557", "23558",
        "23561", "23562", "23563", "23566", "23568",  "23572",  "23574", "23575", "23582",
        "23590", "23592", "23594", "23604", "23605", "23617", "23618", "23619"
    };

    double totalEntriesE_MB = 0.0, totalEntriesE_Central = 0.0;
    double totalEntriesEv2_MB = 0.0, totalEntriesEv2_Central = 0.0;
    
    
    // Merge the two vectors for a unified loop
    std::vector<std::string> allRuns;
    allRuns.insert(allRuns.end(), runNumbers_MB.begin(), runNumbers_MB.end());
    allRuns.insert(allRuns.end(), runNumbers_Central.begin(), runNumbers_Central.end());

    
    for (const auto& run : allRuns) {
        std::string baseDir = "/Users/patsfan753/Desktop/p015/QA/";
        
        std::string category = "MB";
        
        // Determine which category the run belongs to
        if (std::find(runNumbers_Central.begin(), runNumbers_Central.end(), run) != runNumbers_Central.end()) {
            category = "Central";
        }
        
        std::string filePath = baseDir + "RootFiles/" + run + "/qa-" + run + ".root";
        TFile *file = TFile::Open(filePath.c_str());
        if (!file || file->IsZombie()) continue;
        
        // Get the first histogram
        TH2F *hist = (TH2F *)file->Get("h2TotalMBDCaloE");
        if (!hist) continue;
        
        std::cout << "\033[1m\033[31m" << "Run " << run << ", h2TotalMBDCaloE entries: " << hist->GetEntries() << "\033[0m" << std::endl;
        
        // Get the second histogram
        TH2F *hist_v2 = (TH2F *)file->Get("h2TotalMBDCaloEv2");
        if (!hist_v2) continue;
        
        if (hist && hist_v2) {
            double entriesE = hist->GetEntries();
            double entriesEv2 = hist_v2->GetEntries();
            
            if (category == "MB") {
                totalEntriesE_MB += entriesE;
                totalEntriesEv2_MB += entriesEv2;
            } else {
                totalEntriesE_Central += entriesE;
                totalEntriesEv2_Central += entriesEv2;
            }
        }
        // Calculate the ratio of the number of entries
        double ratio = std::max(hist_v2->GetEntries(), 1.0) / hist->GetEntries(); // Avoid division by zero
    
        /*
         Plotting Total MBD, Centrality Correlations
         */
        TH2F *hist_MBD_Centrality = (TH2F *)file->Get("h2TotalMBDCentrality");
        if (!hist_MBD_Centrality) continue;

        // Common setup for canvases
        TCanvas *c1 = new TCanvas("c1", "CaloE vs MBD", 900, 600);
        TCanvas *c2 = new TCanvas("c2", "CaloE vs MBD v2", 900, 600);
        TCanvas *c_MBD_centrality = new TCanvas("c_MBD_centrality", "MBD, Centrality Correlations", 900, 600);
        
        
        // Plotting for h2TotalMBDCaloE
        c1->cd();
        c1->SetRightMargin(0.15);
        gStyle->SetOptStat(0);
        hist->GetXaxis()->SetTitle("Total EMCal Energy [Arb]");
        hist->GetYaxis()->SetTitle("Total MBD Charge [Arb]");
        hist->GetZaxis()->SetTitle("Counts"); // Set the Z-axis title
        hist->SetTitle(Form("p013, Central Triggered CaloE vs MBD (Run: %s)", run.c_str()));
        hist->Draw("COLZ");
        hist->GetZaxis()->SetRangeUser(1, 100000);
        c1->SetLogz();
        TLegend *leg = new TLegend(0.41,.8,0.61,.9);
        leg->SetFillStyle(0);
        leg->AddEntry("","#it{#bf{sPHENIX}} Internal","");
        leg->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
        leg->Draw("SAME");
        TLatex latex;
        latex.SetNDC();
        latex.SetTextSize(0.05);
        std::string runEntry = "Run: " + run;
        latex.DrawLatex(0.56, 0.34, Form("Run: %s", run.c_str()));
        latex.DrawLatex(0.56, 0.29, Form("Entries: %.0f", hist->GetEntries()));
        std::string outFilePath = baseDir + category + "/EMcal_MBD_correlations/CaloE_MBD_Correlation_" + run + ".png";
        c1->SaveAs(outFilePath.c_str(), "PNG");

        
        
        // Plotting for h2TotalMBDCaloEv2
        c2->cd();
        c2->SetRightMargin(0.15); // Increase the left margin if necessary
        hist_v2->Draw("COLZ");
        hist_v2->GetXaxis()->SetTitle("Total EMCal Energy [GeV]");
        hist->GetYaxis()->SetTitle("Total MBD Charge");
        hist_v2->SetTitle(Form("p013, no PDC, w/ z vtx, CaloE vs MBD negative output (Run: %s)", run.c_str()));
        hist_v2->GetZaxis()->SetRangeUser(.1, 100);
        hist_v2->GetYaxis()->SetRangeUser(0, 5000);
        hist_v2->GetZaxis()->SetTitle("Counts"); // Set the Z-axis title
        TLegend *leg2 = new TLegend(0.44,.81,0.64,.91);
        leg2->SetFillStyle(0);
        leg2->AddEntry("","#it{#bf{sPHENIX}} Internal","");
        leg2->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
        leg2->Draw("SAME");
        TLatex latex2;
        latex2.SetNDC();
        latex2.SetTextSize(0.05);
        latex2.DrawLatex(0.2, 0.78, Form("Run: %s", run.c_str()));
        latex2.DrawLatex(0.2, 0.73, Form("Entries: %.0f", hist_v2->GetEntries()));
        latex2.DrawLatex(0.2, 0.68, Form("Ratio: %.5f", ratio));
        c2->SetLogz();
        std::string outFilePath_v2 = baseDir + category + "/EMcal_MBD_correlations/CaloE_MBD_Correlation_v2_" + run + ".png";
        c2->SaveAs(outFilePath_v2.c_str(), "PNG");
        
        
        
        c_MBD_centrality->cd();
        c_MBD_centrality->SetRightMargin(0.15);
        gStyle->SetOptStat(0);
        hist_MBD_Centrality->GetZaxis()->SetTitle("Counts"); // Set the Z-axis title
        hist_MBD_Centrality->SetTitle(Form("p014, MBD Charge, Centrality Correlations (Run: %s)", run.c_str()));
        hist_MBD_Centrality->Draw("COLZ");
        hist_MBD_Centrality->GetZaxis()->SetRangeUser(1, 100000);
        c_MBD_centrality->SetLogz();
        TLegend *leg_MBD_Centrality = new TLegend(0.13,.18,0.33,0.28);
        leg_MBD_Centrality->SetFillStyle(0);
        leg_MBD_Centrality->AddEntry("","#it{#bf{sPHENIX}} Internal","");
        leg_MBD_Centrality->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
        leg_MBD_Centrality->Draw("SAME");
        TLatex latex_MBD_centrality;
        latex_MBD_centrality.SetNDC();
        latex_MBD_centrality.SetTextSize(0.05);
        latex_MBD_centrality.DrawLatex(0.66, 0.89, Form("Run: %s", run.c_str()));
        std::string outFilePath_MBD_centrality = baseDir + category + "/MBD_Centrality/MBD_Centrality_Correlations" + run + ".png";
        c_MBD_centrality->SaveAs(outFilePath_MBD_centrality.c_str(), "PNG");
        
        
        
        // Handling hCentrality histogram
        TH1F *hCentrality = (TH1F *)file->Get("hCentrality");
        if (hCentrality) {
            TCanvas *c_1Dcentrality = new TCanvas("c_1Dcentrality", "Centrality Distribution", 800, 600);
            hCentrality->Draw(); // Draw the histogram
            hCentrality->SetTitle(Form("Centrality Distribution (Run: %s)", run.c_str()));
            hCentrality->GetXaxis()->SetTitle("Centrality");
            hCentrality->GetYaxis()->SetTitle("Counts");
            TLegend *leg_Centrality = new TLegend(.18,.3,.38 ,.45);
            leg_Centrality->SetFillStyle(0);
            leg_Centrality->AddEntry("","#it{#bf{sPHENIX}} Internal","");
            leg_Centrality->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
            leg_Centrality->Draw("SAME");
            TLatex latexCentrality;
            latexCentrality.SetNDC();
            latexCentrality.SetTextSize(0.05);
            latexCentrality.DrawLatex(0.23, 0.255, Form("p013, Run: %s", run.c_str()));
            latexCentrality.DrawLatex(0.23, 0.19, Form("Entries: %.0f", hCentrality->GetEntries()));
            
            std::string centralityOutFilePath = baseDir + category + "/1D_Centrality/Centrality_Distribution_" + run + ".png";
            c_1Dcentrality->SaveAs(centralityOutFilePath.c_str(), "PNG"); // Save the canvas to a file
            delete c_1Dcentrality; // Cleanup
        }

        //Plotting z-vertex Distributions
        TH1F *hVtxZv = (TH1F *)file->Get("hVtxZv2");
        if (hVtxZv) {
            TCanvas *c_zvtx = new TCanvas("c_zvtx", "Centrality Distribution", 800, 600);
            hVtxZv->Draw(); // Draw the histogram
            hVtxZv->SetTitle(Form("Z-vertex Distribution (Run: %s)", run.c_str())); // Set title with run number
            hVtxZv->GetXaxis()->SetTitle("z [cm]");
            hVtxZv->GetYaxis()->SetTitle("Counts");
            hVtxZv->GetXaxis()->SetRangeUser(-20, 20);
            
            TLegend *leg_zvtx = new TLegend(0.53,0.8,0.7,.9);
            leg_zvtx->SetFillStyle(0);
            leg_zvtx->AddEntry("","#it{#bf{sPHENIX}} Internal","");
            leg_zvtx->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
            leg_zvtx->Draw("SAME");
            TLatex latexZvtx;
            latexZvtx.SetNDC();
            latexZvtx.SetTextSize(0.05);
            latexZvtx.DrawLatex(0.68, 0.7, Form("Run: %s", run.c_str()));
            latexZvtx.DrawLatex(0.68, 0.65, Form("Entries: %.0f", hVtxZv->GetEntries()));
            latexZvtx.DrawLatex(0.68, 0.6, Form("Mean: %.2f", hVtxZv->GetMean())); // Include the mean value
             
            
            std::string zVtx_OutFilePath = baseDir + category + "/Zvertex/Zvtx_Distribution_" + run + ".png";
            c_zvtx->SaveAs(zVtx_OutFilePath.c_str(), "PNG"); // Save the canvas to a file
            delete c_zvtx; // Cleanup
        }
        
        TH2F* h2ClusterEtaPhiWeighted = static_cast<TH2F*>(file->Get("h2ClusterEtaPhiWeighted"));
        TH1F* hNClusters = static_cast<TH1F*>(file->Get("hNClusters"));
        // Validate histograms
        if (!h2ClusterEtaPhiWeighted || !hNClusters) {
            std::cerr << "Failed to get histogram for run " << run << '\n';
            file->Close();
            return;
        }
        // Compute normalization factor
        int nEvents = hNClusters->GetEntries();
        h2ClusterEtaPhiWeighted->Scale(1.0 / nEvents);
        if (h2ClusterEtaPhiWeighted) {
            TCanvas *c2D = new TCanvas("c2D", "Eta-Phi Cluster Energy Distributions", 800, 600);
            h2ClusterEtaPhiWeighted->Draw("COLZ1");
            gPad->SetRightMargin(0.22);
            h2ClusterEtaPhiWeighted->SetStats(kFALSE);
            h2ClusterEtaPhiWeighted->GetZaxis()->SetRangeUser(0, 0.01);  // Fixed Z-axis range for Cluster energy
            h2ClusterEtaPhiWeighted->GetXaxis()->SetTitle("Cluster Id #eta");
            h2ClusterEtaPhiWeighted->GetYaxis()->SetTitle("Cluster Id #phi");
            h2ClusterEtaPhiWeighted->GetZaxis()->SetTitle("Cluster Energy/Event [GeV]");
            h2ClusterEtaPhiWeighted->GetZaxis()->SetTitleOffset(1.3);
            TLatex latex;
            latex.SetNDC();
            latex.SetTextSize(0.045);
            latex.SetTextAlign(23);
            latex.DrawLatex(0.28, 0.99, ("Run Number: " + run).c_str()); // Position the text at the top center of the canvas
            std::string zVtx_OutFilePath = baseDir + category + "/ClusterEnergyDistributions/h2ClusterEtaPhiWeighted_2D_" + run + ".png";
            c2D->SaveAs(zVtx_OutFilePath.c_str(), "PNG");
            delete c2D;
        }
        
        delete c1;
        delete c2;
        file->Close();
        delete file;
    }

    // Output accumulated information for MB and Central
    std::cout << "Accumulated Information for MB Runs:\n";
    std::cout << "Total entries in h2TotalMBDCaloE: " << totalEntriesE_MB << std::endl;
    std::cout << "Total entries in h2TotalMBDCaloEv2: " << totalEntriesEv2_MB << std::endl;
    std::cout << "Total ratio of entries (h2TotalMBDCaloEv2/h2TotalMBDCaloE): "
              << (totalEntriesE_MB > 0 ? totalEntriesEv2_MB / totalEntriesE_MB : 0) << std::endl;

    std::cout << "\nAccumulated Information for Central Runs:\n";
    std::cout << "Total entries in h2TotalMBDCaloE: " << totalEntriesE_Central << std::endl;
    std::cout << "Total entries in h2TotalMBDCaloEv2: " << totalEntriesEv2_Central << std::endl;
    std::cout << "Total ratio of entries (h2TotalMBDCaloEv2/h2TotalMBDCaloE): "
              << (totalEntriesE_Central > 0 ? totalEntriesEv2_Central / totalEntriesE_Central : 0) << std::endl;

    // Print the tables
    std::cout << "\nRun Numbers for MB (Total: " << runNumbers_MB.size() << "):\n";
    for (const auto& run : runNumbers_MB) {
        std::cout << run << " ";
    }
    std::cout << "\n\nRun Numbers for Central (Total: " << runNumbers_Central.size() << "):\n";
    for (const auto& run : runNumbers_Central) {
        std::cout << run << " ";
    }
    std::cout << std::endl;
}

