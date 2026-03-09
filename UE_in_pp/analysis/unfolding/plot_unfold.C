#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TLegend.h>
#include <TLine.h>
#include <TColor.h>
#include <TROOT.h>
#include <vector>
#include <string>
#include <sstream>
#include <iterator>

void normalize_hist2D(TH2D* h) {
    const int nX = h->GetNbinsX();
    const int nY = h->GetNbinsY();

    double integral = 0;

    // Temporary storage
    std::vector<std::vector<double>> contents(nX, std::vector<double>(nY));
    std::vector<std::vector<double>> errors(nX, std::vector<double>(nY));

    // First pass: normalize by bin area and compute total
    for (int ix = 1; ix <= nX; ++ix) {
        double wX = h->GetXaxis()->GetBinWidth(ix);
        for (int iy = 1; iy <= nY; ++iy) {
            double wY = h->GetYaxis()->GetBinWidth(iy);
            double area = wX * wY;

            double value = h->GetBinContent(ix, iy);
            double error = h->GetBinError(ix, iy);

            double norm_value = value / area;
            double norm_error = error / area;

            contents[ix-1][iy-1] = norm_value;
            errors[ix-1][iy-1] = norm_error;

            integral += value;
        }
    }

    // Second pass: divide by total integral if desired
    for (int ix = 1; ix <= nX; ++ix) {
        for (int iy = 1; iy <= nY; ++iy) {
            double norm_value = contents[ix-1][iy-1] / integral;
            double norm_error = errors[ix-1][iy-1] / integral;

            h->SetBinContent(ix, iy, norm_value);
            h->SetBinError(ix, iy, norm_error);
        }
    }
}

void draw_unfolded_spectra(TH1D* truth, TH1D* measure, std::vector<TH1D*> unfold, bool jet, std::vector<std::string> leg_tags, int max_iter, std::pair<double, double> x_range, const char* output_name = nullptr, bool first = true)
{
    TH1D* truth_scale = dynamic_cast<TH1D*>(truth->Clone("truth_scale"));
    TCanvas* canvas = new TCanvas("canvas", "", 600, 800);

    TPad* pad1 = new TPad("pad1", "", 0, 0.5, 1, 1.0);
    pad1->SetBottomMargin(0.02);
    pad1->Draw();
    pad1->cd();
    if (jet) pad1->SetLogy(1);
    else pad1->SetLogx(1);

    vector<double> bins;
    vector<double> bin_size;
    for (int i = 1; i <= truth->GetNbinsX() + 1; i++) {
        bins.push_back(truth->GetBinLowEdge(i));
    }
    for (int i = 1; i < bins.size(); i++) {
        bin_size.push_back(bins[i] - bins[i-1]);
    }

    for (int i = 1; i < truth->GetNbinsX() + 1; i++) {
        if (first) truth->SetBinContent(i,truth->GetBinContent(i)/bin_size[i-1]);
        if (first) truth->SetBinError(i,truth->GetBinError(i)/bin_size[i-1]);
        for (int u = 0; u < max_iter; u++) {
            unfold[u]->SetBinContent(i,unfold[u]->GetBinContent(i)/bin_size[i-1]);
            unfold[u]->SetBinError(i,unfold[u]->GetBinError(i)/bin_size[i-1]);
        }
    }

    if (first) {
        bins.clear();
        bin_size.clear();
        for (int i = 1; i <= measure->GetNbinsX() + 1; i++) {
            bins.push_back(measure->GetBinLowEdge(i));
        }
        for (int i = 1; i < bins.size(); i++) {
            bin_size.push_back(bins[i] - bins[i-1]);
        }

        for (int i = 1; i < measure->GetNbinsX() + 1; i++) {
            measure->SetBinContent(i,measure->GetBinContent(i)/bin_size[i-1]);
            measure->SetBinError(i,measure->GetBinError(i)/bin_size[i-1]);
        }
    }

    if (first) {
        truth->Scale(1.0/truth->GetEntries());
        measure->Scale(1.0/measure->GetEntries());
    }
    for (int i = 0; i < max_iter; i++) {
        unfold[i]->Scale(1.0/unfold[i]->GetEntries());
    }

    truth->SetStats(0);
    measure->SetStats(0);
    truth->SetLineColor(2);
    truth->SetMarkerColor(2);
    measure->SetLineColor(1);
    measure->SetMarkerColor(1);
    truth->GetXaxis()->SetLabelSize(0);
    measure->GetXaxis()->SetLabelSize(0);
    truth->GetXaxis()->SetRangeUser(x_range.first, x_range.second);
    //if (!jet) truth->GetYaxis()->SetRangeUser(0,30000);

    for (int i = 0; i < max_iter; ++i) {
        unfold[i]->SetStats(0);
        unfold[i]->SetLineColor(i + 3); // Use TColor::GetColor if needed
        unfold[i]->SetMarkerColor(i + 3);
        unfold[i]->GetXaxis()->SetLabelSize(0);
    }

    measure->Draw();
    truth->Draw("same");
    for (int i = 0; i < max_iter; ++i) {
        unfold[i]->Draw("same");
    }

    TLegend* leg = new TLegend(0.77, 0.65, 0.9, 0.9);
    leg->AddEntry(truth, "Truth", "lp");
    leg->AddEntry(measure, "Measured", "lp");
    for (int i = 0; i < max_iter; ++i) {
        leg->AddEntry(unfold[i], leg_tags[i].c_str(), "lp");
    }
    leg->SetTextSize(0.04);
    leg->Draw();

    canvas->cd();
    TPad* pad2 = new TPad("pad2", "", 0, 0.0, 1, 0.5);
    pad2->SetTopMargin(0.02);
    pad2->SetBottomMargin(0.2);
    pad2->Draw();
    pad2->cd();
    if (!jet) pad2->SetLogx(1);

    std::vector<TH1D*> ratios;
    for (int i = 0; i < max_iter; ++i) {
        TH1D* ratio = (TH1D*)unfold[i]->Clone(Form("ratio%d", i));
        ratio->Divide(truth);
        ratios.push_back(ratio);
    }

    TH1D* r0 = ratios[0];
    r0->GetYaxis()->SetTitle("Unfolded/Truth Ratio");
    r0->GetYaxis()->SetNdivisions(510);
    r0->GetYaxis()->SetRangeUser(0, 2);
    r0->GetYaxis()->SetTitleSize(25);
    r0->GetYaxis()->SetTitleFont(43);
    r0->GetYaxis()->SetTitleOffset(1.5);
    r0->GetYaxis()->SetLabelFont(43);
    r0->GetYaxis()->SetLabelSize(25);
    r0->GetXaxis()->SetTitle(jet ? "p_{T} [GeV]" : "#SigmaE_{T} [GeV]");
    r0->GetXaxis()->SetTitleSize(25);
    r0->GetXaxis()->SetTitleFont(43);
    r0->GetXaxis()->SetTitleOffset(0);
    r0->GetXaxis()->SetLabelFont(43);
    r0->GetXaxis()->SetLabelSize(25);
    r0->GetXaxis()->SetRangeUser(x_range.first, x_range.second);

    r0->Draw("ep");
    for (int i = 1; i < max_iter; ++i) {
        ratios[i]->Draw("ep same");
    }

    TLine* line0 = new TLine(x_range.first, 1.0, x_range.second, 1.0);
    TLine* line1 = new TLine(x_range.first, 0.95, x_range.second, 0.95);
    TLine* line2 = new TLine(x_range.first, 1.05, x_range.second, 1.05);
    line0->SetLineStyle(1);
    line1->SetLineStyle(2);
    line2->SetLineStyle(2);
    line0->Draw("same");
    line1->Draw("same");
    line2->Draw("same");

    canvas->Update();
    canvas->Draw();
    if (output_name) canvas->SaveAs(output_name);
}

void draw_iteration_graph(std::vector<TGraph*> graphs, std::vector<std::string> leg_tags, std::string syst_tag, std::string syst) {
    TCanvas* c1 = new TCanvas("c1", "Val Diff and Errors", 800, 600);
    graphs[0]->GetYaxis()->SetRangeUser(0,0.012);
    graphs[0]->GetXaxis()->SetTitle("Iteration");
    graphs[0]->GetYaxis()->SetTitle("#sigma");
    graphs[0]->Draw("ALP");
    for (int i = 1; i < graphs.size(); i++) {
        graphs[i]->Draw("LP SAME");
    }

    double y_min = 0.92-0.035*(graphs.size()+2);
    TLegend* leg1 = new TLegend(0.2, y_min, 0.5, 0.92);
    leg1->SetTextSize(0.035);
    leg1->AddEntry("","#it{#bf{sPHENIX}} Internal");
    if (syst_tag != "") leg1->AddEntry("",syst_tag.c_str(),"");
    for (int i = 0; i < graphs.size(); i++) {
        leg1->AddEntry(graphs[i], leg_tags[i].c_str(), "lp");
    }
    leg1->Draw();

    string outfile = "run21_figure/h_unfolding_iterations_all_respmatrices.png";
    if (syst != "") outfile = "run21_figure/h_unfolding_iterations_" + syst + ".png";

    c1->SaveAs(outfile.c_str());
}

void plot_unfold() {
    gROOT->LoadMacro("/sphenix/u/egm2153/spring_2023/sPhenixStyle.C");
    gROOT->ProcessLine("SetsPhenixStyle()");

    std::vector<std::string> leg_tags = { "Unfold, iter 1", "Unfold, iter 2", "Unfold, iter 3", "Unfold, iter 4", "Unfold, iter 5", "Unfold, iter 6", 
    "Unfold, iter 7", "Unfold, iter 8", "Unfold, iter 9", "Unfold, iter 10", "Unfold, iter 11", "Unfold, iter 12", "Unfold, iter 13", 
    "Unfold, iter 14", "Unfold, iter 15", "Unfold, iter 16", "Unfold, iter 17", "Unfold, iter 18", "Unfold, iter 19", "Unfold, iter 20"};
    std::vector<std::string> syst = {"calib_dijet","calib_dijet_jesdown","calib_dijet_jesup","calib_dijet_jerdown","calib_dijet_jerup","calib_dijet_half1","calib_dijet_half2"};
    std::vector<std::string> trim = {"","_trim_5","_trim_10","_reweight","_reweight_trim_5","_reweight_trim_10"};

    TFile* f = TFile::Open("output_unfolded_data_calib_dijet_run21_iter_3_1000toys.root");

    std::vector<std::vector<TH2D*>> h_truth_2D(1, std::vector<TH2D*>(6, nullptr));
    std::vector<std::vector<TH1D*>> hj_truth(1, std::vector<TH1D*>(6, nullptr));
    std::vector<std::vector<TH1D*>> hc_truth(1, std::vector<TH1D*>(6, nullptr));
    std::vector<std::vector<std::vector<TH2D*>>> h_unfold_2D(1, std::vector<std::vector<TH2D*>>(6, std::vector<TH2D*>(20, nullptr)));
    std::vector<std::vector<std::vector<TH1D*>>> hj_unfold(1, std::vector<std::vector<TH1D*>>(6, std::vector<TH1D*>(20, nullptr)));
    std::vector<std::vector<std::vector<TH1D*>>> hc_unfold(1, std::vector<std::vector<TH1D*>>(6, std::vector<TH1D*>(20, nullptr)));

    TH2D* h_measure_2D = (TH2D*)f->Get("h_calibjet_pt_dijet_eff");
    TH1D* hj_measure = (TH1D*)h_measure_2D->ProjectionX("hj_measure");
    TH1D* hc_measure = (TH1D*)h_measure_2D->ProjectionY("hc_measure");
    for (int i = 0; i < 1; i++) {
        for (int j = 0; j < 6; j++) {
            h_truth_2D[i][j] = (TH2D*)f->Get(("h_truth_"+syst[i]+trim[j]).c_str());
            hj_truth[i][j] = h_truth_2D[i][j]->ProjectionX(("hj_truth_"+syst[i]+trim[j]).c_str());
            hc_truth[i][j] = h_truth_2D[i][j]->ProjectionY(("hc_truth_"+syst[i]+trim[j]).c_str());
            for (int n = 0; n < 20; n++) {
                h_unfold_2D[i][j][n] = (TH2D*)f->Get(("h_unfold_"+syst[i]+trim[j]+"_"+to_string(n+1)).c_str());
                hj_unfold[i][j][n] = h_unfold_2D[i][j][n]->ProjectionX(("hj_unfold_"+syst[i]+trim[j]+"_"+to_string(n+1)).c_str());
                hc_unfold[i][j][n] = h_unfold_2D[i][j][n]->ProjectionY(("hc_unfold_"+syst[i]+trim[j]+"_"+to_string(n+1)).c_str());
            }
        }
    }
    /*
    for (int i = 0; i < 1; i++) {
        for (int j = 0; j < 6; j++) {
            string jet_outfile = "figure/h_unfolded_jet_spectrum_" + syst[i] + trim[j] + "_iter_3_1000toys.png";
            string et_outfile = "figure/h_unfolded_et_spectrum_" + syst[i] + trim[j] + "_iter_3_1000toys.png";
            draw_unfolded_spectra(hj_truth[i][j], hj_measure, hj_unfold[i][j], true, leg_tags, 6, std::make_pair(14.0, 82.0), jet_outfile.c_str() ,true);
            draw_unfolded_spectra(hc_truth[i][j], hc_measure, hc_unfold[i][j], false, leg_tags, 6, std::make_pair(0.1, 35), et_outfile.c_str() ,true);
        }
    }
    */  
    // For iteration optmization need to find: 
    // Sum of the bin error of unfolded distribution 
    // Variation of the sum of bin content between current unfolding distribution and last unfolding distribution 
    for (int it = 0; it < 20; it++) {
        for (int i = 0; i < 6; i++) {
            normalize_hist2D(h_unfold_2D[0][i][it]);
        }
    }

    std::vector<double> iteration;
    std::vector<std::vector<double>> sigma_error(6, std::vector<double>(19, 0.0));
    std::vector<std::vector<double>> sigma_val_diff(6, std::vector<double>(19, 0.0));
    std::vector<std::vector<double>> total_error(6, std::vector<double>(19, 0.0));
    for (int it = 1; it < 20; it++) {
        iteration.push_back(it);
        for (int i = 0; i < 6; i++) {
            for (int x = 2; x < h_unfold_2D[0][i][it]->GetNbinsX(); x++) {
                for (int y = 1; y < h_unfold_2D[0][i][it]->GetNbinsY(); y++) {
                    sigma_error[i][it-1] += pow(h_unfold_2D[0][i][it]->GetBinError(x,y),2);
                    sigma_val_diff[i][it-1] += pow(h_unfold_2D[0][i][it]->GetBinContent(x,y) - h_unfold_2D[0][i][it-1]->GetBinContent(x,y),2);
                }
            }
        }
    }

    for (int i = 0; i < 6; i++) {
        for (int it = 0; it < sigma_val_diff[i].size(); it++) {
            total_error[i][it] = std::sqrt(sigma_val_diff[i][it] + sigma_error[i][it]);
        }
    }

    for (int i = 0; i < 6; i++) {
        for (int it = 0; it < sigma_val_diff[i].size(); it++) {
            sigma_val_diff[i][it] = sqrt(sigma_val_diff[i][it]);
            sigma_error[i][it] = sqrt(sigma_error[i][it]);
        }
    }    

    for (int i = 0; i < 6; i++) {
        std::cout << "it \t val \t error \t total" << std::endl;
        for (int it = 0; it < 19; it++) {
            std::cout << iteration[it] << "\t" << sigma_val_diff[i][it] << "\t" << sigma_error[i][it] << "\t" << total_error[i][it] << std::endl;
        }
        std::cout << std::endl;
    }

    int n = iteration.size();
    TGraph* g_val_diff[6]; TGraph* g_error[6]; TGraph* g_total[6];
    for (int i = 0; i < 6; i++) {
        g_val_diff[i] = new TGraph(n, &iteration[0], &sigma_val_diff[i][0]);
        g_error[i] = new TGraph(n, &iteration[0], &sigma_error[i][0]);
        g_total[i] = new TGraph(n, &iteration[0], &total_error[i][0]);
        g_val_diff[i]->SetLineColor(kRed); g_val_diff[i]->SetMarkerColor(kRed);
        g_error[i]->SetLineColor(kBlue); g_error[i]->SetMarkerColor(kBlue);
        g_total[i]->SetLineColor(kBlack); g_total[i]->SetMarkerColor(kBlack);
    }

    std::vector<std::string> iter_leg_tags = {"#Sigma#delta_{it}", "#Sigma(#sigma_{stat}^{2} #oplus #sigma_{unfold}^{2})", "#Sigma#delta_{it} #oplus #Sigma(#sigma_{stat}^{2} #oplus #sigma_{unfold}^{2})"};
    std::vector<std::string> trim_iter_leg_tags = {"Untrimmed resp matrix", "Trim < 5 entries resp matrix", "Trim < 10 entries resp matrix", "Untrimmed reweighted resp matrix", "Trim < 5 entries reweighted resp matrix", "Trim < 10 entries reweighted resp matrix"};

    for (int i = 0; i < 6; i++) {
        draw_iteration_graph({g_val_diff[i], g_error[i], g_total[i]}, iter_leg_tags, trim_iter_leg_tags[i], trim[i]);
    }
    std::string total_syst_tag = "#Sigma#delta_{it} #oplus #Sigma(#sigma_{stat}^{2} #oplus #sigma_{unfold}^{2})";
    std::string total_syst = "";
    for (int i = 0; i < 6; i++) {
        if (i < 4) { g_total[i]->SetLineColor(i+1); g_total[i]->SetMarkerColor(i+1); }
        else { g_total[i]->SetLineColor(i+2); g_total[i]->SetMarkerColor(i+2); }
    }
    std::vector<TGraph*> graphs(std::begin(g_total), std::end(g_total));
    draw_iteration_graph(graphs, trim_iter_leg_tags, total_syst_tag, total_syst);


}
