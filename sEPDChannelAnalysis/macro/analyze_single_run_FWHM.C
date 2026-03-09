#include <TFile.h>
#include <TH1F.h>
#include <TTree.h>
#include <TSystem.h>
#include <vector>
#include <string>
#include <regex>

// -- RooFit
#include "RooRealVar.h"
#include "RooDataHist.h"
#include "RooLandau.h"
#include "RooGaussian.h"
#include "RooFFTConvPdf.h"
#include "RooPlot.h"
#include "RooFitResult.h"

using namespace RooFit;

const int NUM_CHANNELS = 744;
const double MIN_ENTRIES = 50;

struct ChannelResult {
    TH1F* hist = nullptr;
    RooPlot* plot = nullptr;
    RooRealVar* x = nullptr;
    RooRealVar* ml = nullptr;
    RooRealVar* sl = nullptr;
    RooRealVar* mg = nullptr;
    RooRealVar* sg = nullptr;
    RooLandau* landau = nullptr;
    RooGaussian* gauss = nullptr;
    RooFFTConvPdf* pdf = nullptr;
    double mpv = -1.0;
    double gauss_sigma = -1.0;
    double chi2_ndf = -1.0;
};

int extract_run_number(const std::string& filename) {
    std::regex pattern("run[_-]?(\\d+).*\\.root"); 
    std::smatch match;
    if (std::regex_search(filename, match, pattern)) {
        try {
            return std::stoi(match[1].str());
        } catch (...) {
            return -1;
        }
    }
    return -1;
}

ChannelResult process_channel(TH1* hist, int ch) {
    ChannelResult result;
    if (!hist || hist->GetEntries() < MIN_ENTRIES) return result;

    // Find peak bin with ADC > 40 to avoid dark current
    int max_bin = -1;
    double max_content = -1;
    const double ADC_MIN = 40.0;

    for(int bin = 1; bin <= hist->GetNbinsX(); bin++) {
        double center = hist->GetXaxis()->GetBinCenter(bin);
        double content = hist->GetBinContent(bin);
        if(center >= ADC_MIN && content > max_content) {
            max_content = content;
            max_bin = bin;
        }
    }

    if(max_bin == -1) return result;

    double x_peak = hist->GetXaxis()->GetBinCenter(max_bin);
    double y_peak = hist->GetBinContent(max_bin);
    
    // Calculate FWHM to estimate sigma
    double half_max = y_peak/2.0;
    double x_left = x_peak, x_right = x_peak;

    // Find left half-max point
    for(int bin = max_bin; bin >= 1; bin--) {
        double center = hist->GetXaxis()->GetBinCenter(bin);
        if(center < ADC_MIN) break;
        
        if(hist->GetBinContent(bin) < half_max) {
            x_left = center;
            break;
        }
    }
    

    // Find right half-max point
    for(int bin = max_bin; bin <= hist->GetNbinsX(); bin++) {
        if(hist->GetBinContent(bin) < half_max) {
            x_right = hist->GetXaxis()->GetBinCenter(bin);
            break;
        }
    }
    
    // Calculate initial sigma estimate
    double fwhm = x_right - x_left;
    double sigma_est = fwhm/2.355;  // Convert FWHM to sigma
    

    double fit_lo = x_peak - 2*sigma_est;
    double fit_hi = x_peak + 3*sigma_est;

    fit_lo = std::max(fit_lo, ADC_MIN);
    
    // Clamp to histogram limits
    double h_min = hist->GetXaxis()->GetXmin();
    double h_max = hist->GetXaxis()->GetXmax();
    fit_lo = std::max(fit_lo, h_min);
    fit_hi = std::min(fit_hi, h_max);
    
    // Fallback for bad FWHM estimates
    if((fit_hi - fit_lo) < 20) {
        fit_lo = x_peak - 40;
        fit_hi = x_peak + 40;
        fit_lo = std::max(fit_lo, ADC_MIN);
        fit_lo = std::max(fit_lo, h_min);
        fit_hi = std::min(fit_hi, h_max);
    }


    result.x = new RooRealVar(Form("x_%d", ch), "ADC", fit_lo, fit_hi);
    result.ml = new RooRealVar(Form("ml_%d", ch), "Landau MPV", x_peak, fit_lo, fit_hi);
    result.sl = new RooRealVar(Form("sl_%d", ch), "Landau #sigma", 5, 0.1, 50);
    result.mg = new RooRealVar(Form("mg_%d", ch), "Gauss #mu", 0, -50, 50);
    result.sg = new RooRealVar(Form("sg_%d", ch), "Gauss #sigma", 3, 0.1, 20);

    RooDataHist dataHist(Form("data_%d", ch), "Data", *result.x, hist);
    result.landau = new RooLandau(Form("landau_%d", ch), "", *result.x, *result.ml, *result.sl);
    result.gauss = new RooGaussian(Form("gauss_%d", ch), "", *result.x, *result.mg, *result.sg);
    
    result.x->setBins(4000, "cache");
    result.pdf = new RooFFTConvPdf(Form("pdf_%d", ch), "Landau#otimesGauss", 
                                 *result.x, *result.landau, *result.gauss);

    RooFitResult* fit_res = result.pdf->fitTo(dataHist, 
        Save(true), 
        PrintLevel(-1), 
        Range(fit_lo, fit_hi),
        NumCPU(4));

    if (fit_res && fit_res->status() == 0) {
        result.plot = result.x->frame(Title("Pulse Height Spectrum"));
        dataHist.plotOn(result.plot);
        result.pdf->plotOn(result.plot);
        
        // Calculate MPV using final parameters
        const int steps = 500;
        double step = (fit_hi - fit_lo)/steps;
        double best_x = fit_lo;
        double max_val = 0;
        
        for(int i=0; i<steps; i++) {
            double x_val = fit_lo + i*step;
            result.x->setVal(x_val);
            double val = result.pdf->getVal();
            
            if(val > max_val) {
                max_val = val;
                best_x = x_val;
            }
        }


        //chi-squared/ndf
        
        double fit_lo = result.x->getMin();
        double fit_hi = result.x->getMax();

        int bin_lo = hist->FindBin(fit_lo);
        int bin_hi = hist->FindBin(fit_hi);

        int nBins = bin_hi - bin_lo + 1;


        double chi2 = 0;
        
        int nParams = fit_res->floatParsFinal().getSize();
        
        for(int bin=bin_lo; bin<=bin_hi; bin++) {
            double x = hist->GetXaxis()->GetBinCenter(bin);
            double data = hist->GetBinContent(bin);
            double error = hist->GetBinError(bin);
            result.x->setVal(x);
            double model = result.pdf->getVal() * hist->GetBinWidth(x) * hist->GetEntries();
            
            if(error > 0) {
                chi2 += pow(data - model, 2) / pow(error, 2);
            }
        }
        
        int ndf = nBins - nParams;
        double chi2_ndf = (ndf > 0) ? chi2/ndf : -1;


        result.chi2_ndf = chi2_ndf;

        result.mpv = best_x;
        result.gauss_sigma = result.sg->getVal();
    }
    
    delete fit_res;
    return result;
}

void analyze_single_run_FWHM(const char* input_file = "/sphenix/tg/tg01/bulk/ecroft/sEPD/spectra_zvtx_cut/run_47869/run_47869.root_histos.root", const char* output_file = "/sphenix/user/ecroft/sEPDChannelAnalysis/analyze_single_channel_test.root") {
    TFile* in_file = TFile::Open(input_file, "READ");
    if (!in_file || in_file->IsZombie()) return;

    TFile out_file(output_file, "RECREATE");
    int run_number = extract_run_number(input_file);
    
    if (run_number >= 0) {
        out_file.mkdir(Form("run_%d", run_number));
        out_file.cd(Form("run_%d", run_number));

        for (int ch = 0; ch < NUM_CHANNELS; ch++) {
            TH1F* hist = dynamic_cast<TH1F*>(in_file->Get(Form("channel%d", ch)));
            if (!hist) continue;

            ChannelResult result = process_channel(hist, ch);
            if (!result.plot) continue;

            out_file.mkdir(Form("run_%d/ch_%04d", run_number, ch));
            out_file.cd(Form("run_%d/ch_%04d", run_number, ch));

            hist->Write("original_hist");
            result.plot->Write("fit_plot");
            
            
            TVectorD params(3);
            params[0] = result.mpv;
            params[1] = result.gauss_sigma;
            params[2] = result.chi2_ndf;
            params.Write("fit_parameters");

            // Cleanup
            delete result.plot;
            delete result.x;
            delete result.ml;
            delete result.sl;
            delete result.mg;
            delete result.sg;
            delete result.landau;
            delete result.gauss;
            delete result.pdf;
        }
    }

    std::cout << "Done!" << std::endl;

    in_file->Close();
    delete in_file;
    out_file.Close();
}
