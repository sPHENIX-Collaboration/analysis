#include <TFile.h>
#include <TDirectory.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TFitResult.h>
#include <TPaveText.h>
#include <vector>
#include <string>
#include <regex>
#include <TSystemDirectory.h>
#include <TSystemFile.h>

void plot_channel_mpvs_fit(const char* input_dir = "/sphenix/tg/tg01/bulk/ecroft/sEPD/analysis_output",
                           const char* output_filename = "channel_mpvs.pdf") {
    // Data storage: vector per channel containing (run, mpv) pairs
    const int num_channels = 744;
    std::vector<std::vector<std::pair<int, float>>> channel_data(num_channels);

    // Get list of run files
    TSystemDirectory dir("input_dir", input_dir);
    TList* files = dir.GetListOfFiles();
    TIter next(files);
    TSystemFile* file;
    int processed_runs = 0;

    std::regex run_pattern("run_(\\d+)_analysis.root");

    while ((file = (TSystemFile*)next())) {
        TString fname = file->GetName();
        std::string filename = fname.Data();
        
        // Match run files
        std::smatch match;
        if (!std::regex_search(filename, match, run_pattern)) continue;
        
        int run_number = std::stoi(match[1].str());
        TString full_path = Form("%s/%s", input_dir, fname.Data());

        // Open run file
        TFile* run_file = TFile::Open(full_path, "READ");
        if (!run_file || run_file->IsZombie()) continue;

        // Get run directory
        TDirectory* run_dir = (TDirectory*)run_file->Get(Form("run_%d", run_number));
        if (!run_dir) {
            run_file->Close();
            continue;
        }

        // Process channels
        TIter ch_next(run_dir->GetListOfKeys());
        TKey* ch_key;
        while ((ch_key = (TKey*)ch_next())) {
            if (!TString(ch_key->GetName()).BeginsWith("ch_")) continue;

            // Extract channel number
            TString ch_name = ch_key->GetName();
            int channel = TString(ch_name(3, 4)).Atoi();

            // Get fit parameters
            TDirectory* ch_dir = (TDirectory*)ch_key->ReadObj();
            TVectorD* params = (TVectorD*)ch_dir->Get("fit_parameters");
            if (!params || params->GetNoElements() < 1) continue;

            double mpv = (*params)[0];
            if (mpv > 0) {
                channel_data[channel].emplace_back(run_number, mpv);
            }
        }

        run_file->Close();
        delete run_file;

        // Progress
        if (++processed_runs % 10 == 0) {
            std::cout << "Processed " << processed_runs << " runs (" 
                      << channel_data[0].size() << " entries for channel 0)" << std::endl;
        }
    }

    // Create plots
    TCanvas canvas("canvas", "Channel MPV Plots", 1200, 800);
    canvas.Print(Form("%s[", output_filename));

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

        bool has_fit = false;
        double intercept = 0.0, slope = 0.0;
        double intercept_err = 0.0, slope_err = 0.0;

        if (graph.GetN() >= 2) {
            // Perform linear fit
            TFitResultPtr fit_result = graph.Fit("pol1", "QS");
            if (fit_result->Status() == 0) {
                has_fit = true;
                intercept = fit_result->Parameter(0);
                intercept_err = fit_result->ParError(0);
                slope = fit_result->Parameter(1);
                slope_err = fit_result->ParError(1);
            }
        }


        canvas.Clear();
        graph.Draw("AP");

        if (has_fit) {
            // Draw the fit function
            TF1 *fit_func = graph.GetFunction("pol1");
            if (fit_func) {
                fit_func->SetLineColor(kRed);
                fit_func->Draw("same");
            }

            // Add text box with fit parameters
            TPaveText *pt = new TPaveText(0.15, 0.7, 0.45, 0.9, "NDC");
            pt->SetFillColor(0);
            pt->SetBorderSize(1);
            pt->SetTextAlign(12);
            pt->SetTextSize(0.03);
            pt->AddText(Form("Intercept: %.2f #pm %.2f", intercept, intercept_err));
            pt->AddText(Form("Slope: %.4f #pm %.4f", slope, slope_err));
            pt->Draw();
        }

        canvas.Update();


        canvas.Print(output_filename);

        if (ch % 50 == 0) {
            std::cout << "Plotted " << ch << "/" << num_channels 
                      << " channels (" << (ch*100/num_channels) << "%)" << std::endl;
        }
    }

    canvas.Print(Form("%s]", output_filename));

    // Cleanup
    //input_file->Close();
    //delete input_file;
    std::cout << "Saved plots to: " << output_filename << std::endl;
}