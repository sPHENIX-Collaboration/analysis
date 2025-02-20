#include <TFile.h>
#include <TH1F.h>
#include <TTree.h>
#include <TSystemDirectory.h>
#include <TSystemFile.h>
#include <TF1.h>
#include <vector>
#include <string>
#include <regex>

const int NUM_CHANNELS = 744;
const double FIT_MIN = 50;
const double FIT_MAX = 1000;
const bool verbose = false;

struct ChannelData {
    float max_mpv = -1;
    float min_mpv = 10000;
    TH1F* max_hist = nullptr;
    TH1F* min_hist = nullptr;
    TF1* minFitFunc = nullptr;
    TF1* maxFitFunc = nullptr;
};

// Lorentzian fit function
Double_t mipFitFunction(Double_t* x, Double_t* par) {
    return par[0] * (par[1]/TMath::Pi()) * (1/((x[0]-par[2])*(x[0]-par[2]) + par[1]*par[1]));
}

// Extract run number from filename
/*
int extract_run_number(const std::string& filename) {
    std::regex pattern("run_(\\d+)\\.root_histos\\.root");
    std::smatch match;
    if (std::regex_search(filename, match, pattern)) {
        return std::stoi(match[1]);
    }
    return -1;
}*/


int extract_run_number(const std::string& filename) {
    // Enhanced pattern to match different file naming conventions
    std::regex pattern("run[_-]?(\\d+).*\\.root");  // Matches:
                                                     // run123.root
                                                     // run_123_histos.root
                                                     // RUN-123.root
    std::smatch match;
    
    if (std::regex_search(filename, match, pattern)) {
        try {
            return std::stoi(match[1].str());
        } catch (...) {
            std::cerr << "Invalid run number in: " << filename << std::endl;
            return -1;
        }
    }
    std::cerr << "No run number found in: " << filename << std::endl;
    return -1;
}


int count_root_files(const std::string& path) {
    int count = 0;
    TSystemDirectory dir("", path.c_str());
    TList* entries = dir.GetListOfFiles();
    TIter next(entries);
    TSystemFile* entry;

    while ((entry = static_cast<TSystemFile*>(next()))) {
        std::string name = entry->GetName();
        if (name == "." || name == "..") continue;
        
        std::string full_path = path + "/" + name;
        
        if (entry->IsDirectory()) {
            count += count_root_files(full_path);
        } else if (name.find("_histos.root") != std::string::npos) {
            count++;
        }
    }
    delete entries;
    return count;
}

void update_progress(int processed, int total) {
    const int bar_width = 50;
    float progress = static_cast<float>(processed)/total;
    int pos = static_cast<int>(bar_width * progress);
    
    std::cerr << "\r[";
    for(int i = 0; i < bar_width; ++i) {
        if(i < pos) std::cerr << "=";
        else if(i == pos) std::cerr << ">";
        else std::cerr << " ";
    }
    std::cerr << "] " << int(progress * 100.0) << "% "
              << "(" << processed << "/" << total << ")";
    std::cerr.flush();
}

// Recursive directory scanner
void process_directory(const std::string& path, 
                      std::vector<ChannelData>& channels,
                      TTree* data_tree,
                      int& run_number,
                      std::vector<float>& current_mpv,
                      int& processed_files,
                      int total_files) {
    TSystemDirectory dir("", path.c_str());
    TList* entries = dir.GetListOfFiles();
    TIter next(entries);
    TSystemFile* entry;

    while ((entry = static_cast<TSystemFile*>(next()))) {
        std::string name = entry->GetName();
        if (name == "." || name == "..") continue;

        std::string full_path = path + "/" + name;
        
        if (entry->IsDirectory()) {
            process_directory(full_path, channels, data_tree, run_number, 
                             current_mpv, processed_files, total_files);
        }
        else if (name.find("_histos.root") != std::string::npos) {
            TFile* file = TFile::Open(full_path.c_str(), "READ");
            if (!file || file->IsZombie()) {
                std::cerr << "\nError opening: " << full_path << std::endl;
                continue;
            }

            run_number = extract_run_number(name);

            if(verbose) {
                std::cout << "Run number = " << run_number  << std::endl;
            }

            std::fill(current_mpv.begin(), current_mpv.end(), -1.0f);


            for (int ch = 0; ch < NUM_CHANNELS; ch++) {
                TH1F* hist = dynamic_cast<TH1F*>(file->Get(Form("channel%d", ch)));
                if (!hist || hist->GetEntries() < 100) continue;
                

                //TF1 fitfunc("mipfit", mipFitFunction, FIT_MIN, FIT_MAX, 3);
                //try a landau distribution
                TF1 fitfunc("mipfit","landau",FIT_MIN,FIT_MAX);
                fitfunc.SetParameters(2200, 100, 100);     
                hist->Fit(&fitfunc, "QRN0");
                //current_mpv[ch] = fitfunc.GetParameter(2);
                current_mpv[ch] = fitfunc.GetParameter(1);


                //float mpv = fitfunc.GetParameter(2);
                float mpv = fitfunc.GetParameter(1);

                if(verbose){
                    std::cout << "MPV for channel " << ch << " = " << mpv << std::endl;
                }


                // Clone and manage histograms properly
                TH1F* hist_clone = static_cast<TH1F*>(hist->Clone());
                hist_clone->SetDirectory(0);

                TF1* fit_clone = static_cast<TF1*>(fitfunc.Clone(
                    Form("ch%d_fit_clone", ch)));


                if (mpv > channels[ch].max_mpv) {
                    delete channels[ch].max_hist;
                    delete channels[ch].maxFitFunc;

                    channels[ch].max_mpv = mpv;
                    channels[ch].max_hist = hist_clone;
                    channels[ch].maxFitFunc = fit_clone;
                } else if (mpv < channels[ch].min_mpv) {
                    delete channels[ch].min_hist;
                    delete channels[ch].minFitFunc;

                    channels[ch].min_mpv = mpv;
                    channels[ch].min_hist = hist_clone;
                    channels[ch].minFitFunc = fit_clone;
                } else {
                    delete hist_clone; // Only keep needed clones
                    delete fit_clone;
                }
            }

            data_tree->Fill();

            file->Close();
            delete file;

            processed_files++;
            update_progress(processed_files, total_files);
        }
    }
    // Do NOT delete entries - ROOT manages this memory
}


void analyze_channels() {
    std::vector<ChannelData> channels(NUM_CHANNELS);
    TFile output("/sphenix/user/ecroft/channel_analysis_landau.root", "RECREATE");
    
    // Create main data tree
    TTree* data_tree = new TTree("rundata", "Per-run channel MPV values");
    int run_number;
    std::vector<float> current_mpv(NUM_CHANNELS, -1.0f);
    
    data_tree->Branch("run", &run_number);
    data_tree->Branch("mpv_values", &current_mpv);

    // Count files first for progress tracking
    const std::string input_dir = "/sphenix/tg/tg01/bulk/ecroft/sEPD/test/";
    int total_files = count_root_files(input_dir);
    int processed_files = 0;

    if(total_files == 0) {
        std::cerr << "No files found!" << std::endl;
        return;
    }

    std::cerr << "Processing " << total_files << " files..." << std::endl;
    
    // Process files recursively from main directory
    process_directory(input_dir, channels, data_tree, run_number, 
                     current_mpv, processed_files, total_files);

    std::cerr << std::endl;
    // Save results
    output.cd();
    data_tree->Write();
    
    // Create summary histograms
    TH1F h_maxmpv("max_mpv", "Maximum MPV per channel", NUM_CHANNELS, 0, NUM_CHANNELS);
    TH1F h_minmpv("min_mpv", "Minimum MPV per channel", NUM_CHANNELS, 0, NUM_CHANNELS);
    
    // Save extremal histograms and fill summary
    for (int ch = 0; ch < NUM_CHANNELS; ch++) {
        h_maxmpv.SetBinContent(ch+1, channels[ch].max_mpv);
        h_minmpv.SetBinContent(ch+1, channels[ch].min_mpv);
        
        if (channels[ch].max_hist) {
            channels[ch].max_hist->Write(Form("ch%d_max_hist", ch));
            delete channels[ch].max_hist;
        }
        if (channels[ch].min_hist) {
            channels[ch].min_hist->Write(Form("ch%d_min_hist", ch));
            delete channels[ch].min_hist;
        }

        if (channels[ch].minFitFunc) {
            channels[ch].minFitFunc->Write(Form("ch%d_min_fit", ch));
            delete channels[ch].minFitFunc;
        }

        if (channels[ch].maxFitFunc) {
            channels[ch].maxFitFunc->Write(Form("ch%d_max_fit", ch));
            delete channels[ch].maxFitFunc;
        }
    }
    
    h_maxmpv.Write();
    h_minmpv.Write();
    output.Close();
}
