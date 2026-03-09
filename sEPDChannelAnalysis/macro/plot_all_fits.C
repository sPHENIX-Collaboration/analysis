#include <TFile.h>
#include <TDirectory.h>
#include <TCanvas.h>
#include <TLatex.h>
#include <RooPlot.h>
#include <TVectorD.h>

void plot_all_fits(const char* input_file = "/sphenix/tg/tg01/bulk/ecroft/sEPD/analysis_output/run_51154_analysis.root",
                   const char* output_file = "run_51154_fits.pdf") {
    TFile* file = TFile::Open(input_file, "READ");
    if (!file || file->IsZombie()) {
        std::cerr << "Error opening file: " << input_file << std::endl;
        return;
    }

    TCanvas* c = new TCanvas("c", "Fit Results", 1200, 800);

    c->Print(Form("%s[", output_file));  // Start multi-page PDF

    std::regex run_pattern("run_(\\d+)");

    TIter next(file->GetListOfKeys());
    TKey* key;
    while ((key = (TKey*)next())) {
        if (TString(key->GetClassName()) != "TDirectoryFile") continue;
        
        TDirectory* run_dir = (TDirectory*)key->ReadObj();
        std::string dir_name = run_dir->GetName();

        std::smatch match;
        int run_number = -1;
        if (std::regex_search(dir_name, match, run_pattern)) {
            run_number = std::stoi(match[1].str());
        }

        TIter ch_next(run_dir->GetListOfKeys());
        TKey* ch_key;
        while ((ch_key = (TKey*)ch_next())) {
            if (!TString(ch_key->GetName()).BeginsWith("ch_")) continue;

            TDirectory* ch_dir = (TDirectory*)ch_key->ReadObj();
            ch_dir->cd();

            RooPlot* plot = (RooPlot*)gDirectory->Get("fit_plot");
            TVectorD* params = (TVectorD*)gDirectory->Get("fit_parameters");
            TH1F* hist = (TH1F*)gDirectory->Get("original_hist");

            if (!plot || !params || !hist) continue;

            c->Clear();
            c->Divide(1,2);
            
            c->cd(1);
            hist->SetStats(0);
            hist->SetLineColor(kBlack);
            hist->SetTitle(Form("Run %d - Channel %s (Raw Spectrum)", 
                              run_number, ch_key->GetName()));
            hist->Draw();
            
            c->cd(2);
            plot->SetTitle(Form("Run %d - Channel %s (Fit Result)", 
                               run_number, ch_key->GetName()));
            plot->Draw();
            
            TLatex tex;
            tex.SetNDC();
            tex.SetTextSize(0.04);
            
            c->cd(2);
            tex.DrawLatex(0.65, 0.80, Form("MPV = %.1f", (*params)[0]));
            tex.DrawLatex(0.65, 0.75, Form("#sigma_{Gauss} = %.1f", (*params)[1]));
            tex.DrawLatex(0.65, 0.70, Form("Entries = %d", hist->GetEntries()));

            // Add page to PDF with specified filename
            c->Print(output_file);
        }
    }

    // Close PDF with specified filename
    c->Print(Form("%s]", output_file));
    file->Close();
    delete c;
}
