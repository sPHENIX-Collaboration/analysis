#include <TFile.h>
#include <TTree.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <vector>
#include <string>
#include <sPhenixStyle.C>
#include <sPhenixStyle.h>

void plot_channel_mpvs(const char* input_filename = "/sphenix/user/ecroft/channel_analysis_zs20.root", 
                      const char* output_filename = "/sphenix/user/ecroft/channel_mpv_plots_zs20.pdf") {
    // Open input file
    TFile* input_file = TFile::Open(input_filename, "READ");
    if (!input_file || input_file->IsZombie()) {
        std::cerr << "Error opening input file: " << input_filename << std::endl;
        return;
    }

    // Get the TTree
    TTree* tree = dynamic_cast<TTree*>(input_file->Get("rundata"));
    if (!tree) {
        std::cerr << "Error: Could not find 'rundata' tree in input file" << std::endl;
        input_file->Close();
        return;
    }

    // Set up branch addresses
    int run_number;
    std::vector<float> *mpv_values = 0;
    tree->SetBranchAddress("run", &run_number);
    tree->SetBranchAddress("mpv_values", &mpv_values);

    // Data storage: vector per channel containing (run, mpv) pairs
    const int num_channels = 744;
    std::vector<std::vector<std::pair<int, float>>> channel_data(num_channels);

    // Read all data
    const Long64_t n_entries = tree->GetEntries();
    for (Long64_t i = 0; i < n_entries; i++) {
        tree->GetEntry(i);
        
        // Skip bad runs
        if (run_number < 0) continue;

        // Store data for each channel
        for (int ch = 0; ch < num_channels; ch++) {
            if (mpv_values->at(ch) > 0) { // Skip invalid MPVs
                channel_data[ch].emplace_back(run_number, mpv_values->at(ch));
            }
        }

        // Progress update
        if (i % 100 == 0) {
            std::cout << "\rProcessed " << i << "/" << n_entries 
                      << " runs (" << (i*100/n_entries) << "%)" << std::flush;
        }
    }
    std::cout << "\nFinished processing " << n_entries << " runs" << std::endl;


    TCanvas canvas("canvas", "Channel MPV Plots", 1200, 800);
    canvas.Print(Form("%s[", output_filename)); // Open multi-page PDF

    for (int ch = 0; ch < num_channels; ch++) {
        const auto& data = channel_data[ch];
        if (data.empty()) continue;

 
        std::vector<double> runs, mpvs;
        for (const auto& point : data) {
            runs.push_back(point.first);
            mpvs.push_back(point.second);
        }

        TGraph graph(runs.size(), runs.data(), mpvs.data());
        graph.SetTitle(Form("Channel %d MPV History;Run Number;MPV [ADC]", ch));
        graph.SetMarkerStyle(20);
        graph.SetMarkerSize(0.8);
        graph.SetMarkerColor(kBlue);

      
        canvas.Clear();
        graph.Draw("AP");
        canvas.Update();

        // Add to PDF
        canvas.Print(output_filename);

        // Clean up
        if (ch % 50 == 0) {
            std::cout << "Plotted " << ch << "/" << num_channels 
                      << " channels (" << (ch*100/num_channels) << "%)" << std::endl;
        }
    }

    // Close PDF
    canvas.Print(Form("%s]", output_filename));

    // Cleanup
    input_file->Close();
    delete input_file;

    std::cout << "Saved plots to: " << output_filename << std::endl;
}
