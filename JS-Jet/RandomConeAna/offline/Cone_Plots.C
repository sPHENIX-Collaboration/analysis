#include <iostream>
#include <fstream>
#include <vector>
#include <utility>

#include "sPhenixStyle.h"
#include "sPhenixStyle.C"

using namespace std;


Int_t Cone_Plots()
{

    TString infile = "../macro/output.root";
    TString outputfile = "ConePlots.root";
    
    SetsPhenixStyle();
    TH1::SetDefaultSumw2();
    TH2::SetDefaultSumw2();

    // open input file
    TFile fin(infile.Data(), "READ");
    if(!fin.IsOpen())
    {
        std::cout << "Error: could not open input file" << std::endl;
        return -1;
    }
    TTree *tree = (TTree*)fin.Get("T");
    if(!tree)
    {
        std::cout << "Error: could not find tree" << std::endl;
        return -1;
    }

    int event = 0;
    int seed = 0;
    int centrality = 0;
    float weight = 0;
    float rho_area = 0;
    float rho_area_sigma = 0;
    float rho_mult = 0;
    float rho_mult_sigma = 0;
    float cone_area = 0;
    int cone_nclustered = 0;
    float cone_pt_raw = 0;
    float cone_pt_iter_sub = 0;
    float randomized_cone_pt_raw = 0;
    float randomized_cone_pt_iter_sub = 0;
    float avoid_leading_cone_pt_raw = 0;
    float avoid_leading_cone_pt_iter_sub = 0;

    tree->SetBranchAddress("event", &event);
    tree->SetBranchAddress("seed", &seed);
    tree->SetBranchAddress("centrality", &centrality);
    tree->SetBranchAddress("weight", &weight);
    tree->SetBranchAddress("rho_area", &rho_area);
    tree->SetBranchAddress("rho_area_sigma", &rho_area_sigma);
    tree->SetBranchAddress("rho_mult", &rho_mult);
    tree->SetBranchAddress("rho_mult_sigma", &rho_mult_sigma);
    tree->SetBranchAddress("cone_area", &cone_area);
    tree->SetBranchAddress("cone_nclustered", &cone_nclustered);
    tree->SetBranchAddress("cone_pt_raw", &cone_pt_raw);
    tree->SetBranchAddress("cone_pt_iter_sub", &cone_pt_iter_sub);
    tree->SetBranchAddress("randomized_cone_pt_raw", &randomized_cone_pt_raw);
    tree->SetBranchAddress("randomized_cone_pt_iter_sub", &randomized_cone_pt_iter_sub);
    tree->SetBranchAddress("avoid_leading_cone_pt_raw", &avoid_leading_cone_pt_raw);
    tree->SetBranchAddress("avoid_leading_cone_pt_iter_sub", &avoid_leading_cone_pt_iter_sub);


    const int resp_N = 200;
    double resp_bins[resp_N+1];
    for(int i = 0; i < resp_N+1; i++){
        resp_bins[i] = -15 + (65.0/resp_N * i);
    }

    const double cent_bins[] = {0, 10, 30, 50, 80, 100}; //the first bin is for inclusive in centrality/pp events
    const int cent_N = sizeof(cent_bins)/sizeof(double) - 1;

    // set up resolution histogram
    TH2D * h2_reso_pt_area = new TH2D("h2_reso_pt_area", "", resp_N, resp_bins, cent_N, cent_bins);
    TH1D * h1_reso_pt_area = new TH1D("h1_reso_pt_area", "", cent_N, cent_bins);

    TH2D * h2_reso_pt_mult = new TH2D("h2_reso_pt_mult", "", resp_N, resp_bins, cent_N, cent_bins);
    TH1D * h1_reso_pt_mult = new TH1D("h1_reso_pt_mult", "", cent_N, cent_bins);

    TH2D * h2_reso_pt_iter_sub = new TH2D("h2_reso_pt_iter_sub", "", resp_N, resp_bins, cent_N, cent_bins);
    TH1D * h1_reso_pt_iter_sub = new TH1D("h1_reso_pt_iter_sub", "", cent_N, cent_bins);

    TH2D * h2_reso_pt_area_randomized = new TH2D("h2_reso_pt_area_randomized", "", resp_N, resp_bins, cent_N, cent_bins);
    TH1D * h1_reso_pt_area_randomized = new TH1D("h1_reso_pt_area_randomized", "", cent_N, cent_bins);

    TH2D * h2_reso_pt_mult_randomized = new TH2D("h2_reso_pt_mult_randomized", "", resp_N, resp_bins, cent_N, cent_bins);
    TH1D * h1_reso_pt_mult_randomized = new TH1D("h1_reso_pt_mult_randomized", "", cent_N, cent_bins);

    TH2D * h2_reso_pt_iter_sub_randomized = new TH2D("h2_reso_pt_iter_sub_randomized", "", resp_N, resp_bins, cent_N, cent_bins);
    TH1D * h1_reso_pt_iter_sub_randomized = new TH1D("h1_reso_pt_iter_sub_randomized", "", cent_N, cent_bins);

    // set up resolution histogram
    TH2D * h2_reso_pt_area_avoidleading = new TH2D("h2_reso_pt_area_avoidleading", "", resp_N, resp_bins, cent_N, cent_bins);
    TH1D * h1_reso_pt_area_avoidleading = new TH1D("h1_reso_pt_area_avoidleading", "", cent_N, cent_bins);

    TH2D * h2_reso_pt_mult_avoidleading = new TH2D("h2_reso_pt_mult_avoidleading", "", resp_N, resp_bins, cent_N, cent_bins);
    TH1D * h1_reso_pt_mult_avoidleading = new TH1D("h1_reso_pt_mult_avoidleading", "", cent_N, cent_bins);

    TH2D * h2_reso_pt_iter_sub_avoidleading = new TH2D("h2_reso_pt_iter_sub_avoidleading", "", resp_N, resp_bins, cent_N, cent_bins);
    TH1D * h1_reso_pt_iter_sub_avoidleading = new TH1D("h1_reso_pt_iter_sub_avoidleading", "", cent_N, cent_bins);


    // get number of entries
    int nEntries = tree->GetEntries();
    for (int iEvent = 0; iEvent < nEntries; iEvent++){
        // get entry
        tree->GetEntry(iEvent);
        // fill resolution histograms
        float pt_area = cone_pt_raw - rho_area * cone_area;
        float pt_mult = cone_pt_raw - rho_mult * cone_nclustered;
        float pt_iter_sub = cone_pt_iter_sub;

        float pt_area_randomized = randomized_cone_pt_raw - rho_area * cone_area;
        float pt_mult_randomized = randomized_cone_pt_raw - rho_mult * cone_nclustered;
        float pt_iter_sub_randomized = randomized_cone_pt_iter_sub;

        float pt_area_avoidleading = avoid_leading_cone_pt_raw - rho_area * cone_area;
        float pt_mult_avoidleading = avoid_leading_cone_pt_raw - rho_mult * cone_nclustered;
        float pt_iter_sub_avoidleading = avoid_leading_cone_pt_iter_sub;

        h2_reso_pt_area->Fill(pt_area, centrality);
        h2_reso_pt_mult->Fill(pt_mult, centrality);
        h2_reso_pt_iter_sub->Fill(pt_iter_sub, centrality);
        // h2_reso_pt_area->Fill(pt_area, -1);
        // h2_reso_pt_mult->Fill(pt_mult, -1);
        // h2_reso_pt_iter_sub->Fill(pt_iter_sub, -1);

        h2_reso_pt_area_randomized->Fill(pt_area_randomized, centrality);
        h2_reso_pt_mult_randomized->Fill(pt_mult_randomized, centrality);
        h2_reso_pt_iter_sub_randomized->Fill(pt_iter_sub_randomized, centrality);
        // h2_reso_pt_area_randomized->Fill(pt_area_randomized, -1);
        // h2_reso_pt_mult_randomized->Fill(pt_mult_randomized, -1);
        // h2_reso_pt_iter_sub_randomized->Fill(pt_iter_sub_randomized, -1);

        h2_reso_pt_area_avoidleading->Fill(pt_area_avoidleading, centrality);
        h2_reso_pt_mult_avoidleading->Fill(pt_mult_avoidleading, centrality);
        h2_reso_pt_iter_sub_avoidleading->Fill(pt_iter_sub_avoidleading, centrality);
        // h2_reso_pt_area_avoidleading->Fill(pt_area_avoidleading, -1);
        // h2_reso_pt_mult_avoidleading->Fill(pt_mult_avoidleading, -1);
        // h2_reso_pt_iter_sub_avoidleading->Fill(pt_iter_sub_avoidleading, -1);
    }
    

    // create output file
    TFile * fout = new TFile(outputfile.Data(), "RECREATE");

    TCanvas * c = new TCanvas("c", "c", 800, 600);
    for(int icent = 0; icent < cent_N; ++icent)
    {
        
        h2_reso_pt_area->GetYaxis()->SetRange(icent+1,icent+1);
        TH1D * h_temp = (TH1D*)h2_reso_pt_area->ProjectionX();
        h_temp->GetXaxis()->SetTitle("#delta p_{T} = p_{T}^{reco} - #rho A");
        h_temp->GetYaxis()->SetTitle("Counts");
        // fit
        TF1 *func = new TF1("func","gaus",-15, 60);
        h_temp->Fit(func,"Q","",-15, 60);
        h_temp->Fit(func,"Q","",func->GetParameter(1)-2*func->GetParameter(2),func->GetParameter(1)+2*func->GetParameter(2));
        func->SetLineColor(kRed);


        // draw
        c->cd();
        gPad->SetLeftMargin(0.15);
        gPad->SetRightMargin(0.05);
        gPad->SetBottomMargin(0.05);
        gPad->SetTopMargin(0.05);
        h_temp->Draw();
        func->Draw("SAME");
        c->Update();
        c->SaveAs(Form("plots/area_cent%.0f-%.0f.png", cent_bins[icent], cent_bins[icent+1]));

        h1_reso_pt_area->SetBinContent(icent+1, func->GetParameter(2));                
        
    }

    for(int icent = 0; icent < cent_N; ++icent)
    {
        
       h2_reso_pt_mult->GetYaxis()->SetRange(icent+1,icent+1);
        TH1D * h_temp = (TH1D*)h2_reso_pt_mult->ProjectionX();
        h_temp->GetXaxis()->SetTitle("#delta p_{T} = p_{T}^{reco} - #rho M");
        h_temp->GetYaxis()->SetTitle("Counts");
        // fit
        TF1 *func = new TF1("func","gaus",-15, 60);
        h_temp->Fit(func,"Q","",-15, 60);
        h_temp->Fit(func,"Q","",func->GetParameter(1)-2*func->GetParameter(2),func->GetParameter(1)+2*func->GetParameter(2));
        func->SetLineColor(kRed);


        // draw
        c->cd();
        gPad->SetLeftMargin(0.15);
        gPad->SetRightMargin(0.05);
        gPad->SetBottomMargin(0.05);
        gPad->SetTopMargin(0.05);
        h_temp->Draw();
        func->Draw("SAME");
        c->Update();
        c->SaveAs(Form("plots/mult_cent%.0f-%.0f.png", cent_bins[icent], cent_bins[icent+1]));

        h1_reso_pt_mult->SetBinContent(icent+1, func->GetParameter(2));              
        
    }

    for(int icent = 0; icent < cent_N; ++icent)
    {
        
            h2_reso_pt_iter_sub->GetYaxis()->SetRange(icent+1,icent+1);
        TH1D * h_temp = (TH1D*)h2_reso_pt_iter_sub->ProjectionX();
        h_temp->GetXaxis()->SetTitle("#delta p_{T} = p_{T}^{Iter}");
        h_temp->GetYaxis()->SetTitle("Counts");
        // fit
        TF1 *func = new TF1("func","gaus",-15, 60);
        h_temp->Fit(func,"Q","",-15, 60);
        h_temp->Fit(func,"Q","",func->GetParameter(1)-2*func->GetParameter(2),func->GetParameter(1)+2*func->GetParameter(2));
        func->SetLineColor(kRed);


        // draw
        c->cd();
        gPad->SetLeftMargin(0.15);
        gPad->SetRightMargin(0.05);
        gPad->SetBottomMargin(0.05);
        gPad->SetTopMargin(0.05);
        h_temp->Draw();
        func->Draw("SAME");
        c->Update();
        c->SaveAs(Form("plots/iter_sub_cent%.0f-%.0f.png", cent_bins[icent], cent_bins[icent+1]));

        h1_reso_pt_iter_sub->SetBinContent(icent+1, func->GetParameter(2));            
        
    }

    for(int icent = 0; icent < cent_N; ++icent)
    {
        
        h2_reso_pt_area_randomized->GetYaxis()->SetRange(icent+1,icent+1);
        TH1D * h_temp = (TH1D*)h2_reso_pt_area_randomized->ProjectionX();
        h_temp->GetXaxis()->SetTitle("#delta p_{T} = p_{T}^{reco, randomized} - #rho A");
        h_temp->GetYaxis()->SetTitle("Counts");
        // fit
        TF1 *func = new TF1("func","gaus",-15, 60);
        h_temp->Fit(func,"Q","",-15, 60);
        h_temp->Fit(func,"Q","",func->GetParameter(1)-2*func->GetParameter(2),func->GetParameter(1)+2*func->GetParameter(2));
        func->SetLineColor(kRed);


        // draw
        c->cd();
        gPad->SetLeftMargin(0.15);
        gPad->SetRightMargin(0.05);
        gPad->SetBottomMargin(0.05);
        gPad->SetTopMargin(0.05);
        h_temp->Draw();
        func->Draw("SAME");
        c->Update();
        c->SaveAs(Form("plots/area_randomized_cent%.0f-%.0f.png", cent_bins[icent], cent_bins[icent+1]));

        h1_reso_pt_area_randomized->SetBinContent(icent+1, func->GetParameter(2));                
        
    }

    for(int icent = 0; icent < cent_N; ++icent)
    {
        
        h2_reso_pt_mult_randomized->GetYaxis()->SetRange(icent+1,icent+1);
        TH1D * h_temp = (TH1D*)h2_reso_pt_mult_randomized->ProjectionX();
        h_temp->GetXaxis()->SetTitle("#delta p_{T} = p_{T}^{reco, randomized} - #rho M");
        h_temp->GetYaxis()->SetTitle("Counts");
        // fit
        TF1 *func = new TF1("func","gaus",-15, 60);
        h_temp->Fit(func,"Q","",-15, 60);
        h_temp->Fit(func,"Q","",func->GetParameter(1)-2*func->GetParameter(2),func->GetParameter(1)+2*func->GetParameter(2));
        func->SetLineColor(kRed);


        // draw
        c->cd();
        gPad->SetLeftMargin(0.15);
        gPad->SetRightMargin(0.05);
        gPad->SetBottomMargin(0.05);
        gPad->SetTopMargin(0.05);
        h_temp->Draw();
        func->Draw("SAME");
        c->Update();
        c->SaveAs(Form("plots/mult_randomized_cent%.0f-%.0f.png", cent_bins[icent], cent_bins[icent+1]));

        h1_reso_pt_mult_randomized->SetBinContent(icent+1, func->GetParameter(2));                
        
    }

    for(int icent = 0; icent < cent_N; ++icent)
    {
        
        h2_reso_pt_iter_sub_randomized->GetYaxis()->SetRange(icent+1,icent+1);
        TH1D * h_temp = (TH1D*)h2_reso_pt_iter_sub_randomized->ProjectionX();
        h_temp->GetXaxis()->SetTitle("#delta p_{T} = p_{T}^{reco, randomized} - #rho M");
        h_temp->GetYaxis()->SetTitle("Counts");
        // fit
        TF1 *func = new TF1("func","gaus",-15, 60);
        h_temp->Fit(func,"Q","",-15, 60);
        h_temp->Fit(func,"Q","",func->GetParameter(1)-2*func->GetParameter(2),func->GetParameter(1)+2*func->GetParameter(2));
        func->SetLineColor(kRed);


        // draw
        c->cd();
        gPad->SetLeftMargin(0.15);
        gPad->SetRightMargin(0.05);
        gPad->SetBottomMargin(0.05);
        gPad->SetTopMargin(0.05);
        h_temp->Draw();
        func->Draw("SAME");
        c->Update();
        c->SaveAs(Form("plots/iter_sub_randomized_cent%.0f-%.0f.png", cent_bins[icent], cent_bins[icent+1]));

        h1_reso_pt_iter_sub_randomized->SetBinContent(icent+1, func->GetParameter(2));                
        
    }

    for(int icent = 0; icent < cent_N; ++icent)
    {
        
        h2_reso_pt_area_avoidleading->GetYaxis()->SetRange(icent+1,icent+1);
        TH1D * h_temp = (TH1D*)h2_reso_pt_area_avoidleading->ProjectionX();
        h_temp->GetXaxis()->SetTitle("#delta p_{T} = p_{T}^{reco, avoidleading} - #rho A");
        h_temp->GetYaxis()->SetTitle("Counts");
        // fit
        TF1 *func = new TF1("func","gaus",-15, 60);
        h_temp->Fit(func,"Q","",-15, 60);
        h_temp->Fit(func,"Q","",func->GetParameter(1)-2*func->GetParameter(2),func->GetParameter(1)+2*func->GetParameter(2));
        func->SetLineColor(kRed);


        // draw
        c->cd();
        gPad->SetLeftMargin(0.15);
        gPad->SetRightMargin(0.05);
        gPad->SetBottomMargin(0.05);
        gPad->SetTopMargin(0.05);
        h_temp->Draw();
        func->Draw("SAME");
        c->Update();
        c->SaveAs(Form("plots/area_avoidleading_cent%.0f-%.0f.png", cent_bins[icent], cent_bins[icent+1]));

        h1_reso_pt_area_avoidleading->SetBinContent(icent+1, func->GetParameter(2));                
        
    }

    for(int icent = 0; icent < cent_N; ++icent)
    {
        
        h2_reso_pt_mult_avoidleading->GetYaxis()->SetRange(icent+1,icent+1);
        TH1D * h_temp = (TH1D*)h2_reso_pt_mult_avoidleading->ProjectionX();
        h_temp->GetXaxis()->SetTitle("#delta p_{T} = p_{T}^{reco, avoidleading} - #rho M");
        h_temp->GetYaxis()->SetTitle("Counts");
        // fit
        TF1 *func = new TF1("func","gaus",-15, 60);
        h_temp->Fit(func,"Q","",-15, 60);
        h_temp->Fit(func,"Q","",func->GetParameter(1)-2*func->GetParameter(2),func->GetParameter(1)+2*func->GetParameter(2));
        func->SetLineColor(kRed);


        // draw
        c->cd();
        gPad->SetLeftMargin(0.15);
        gPad->SetRightMargin(0.05);
        gPad->SetBottomMargin(0.05);
        gPad->SetTopMargin(0.05);
        h_temp->Draw();
        func->Draw("SAME");
        c->Update();
        c->SaveAs(Form("plots/mult_avoidleading_cent%.0f-%.0f.png", cent_bins[icent], cent_bins[icent+1]));

        h1_reso_pt_mult_avoidleading->SetBinContent(icent+1, func->GetParameter(2));                
        
    }

    for(int icent = 0; icent < cent_N; ++icent)
    {
        
        h2_reso_pt_iter_sub_avoidleading->GetYaxis()->SetRange(icent+1,icent+1);
        TH1D * h_temp = (TH1D*)h2_reso_pt_iter_sub_avoidleading->ProjectionX();
        h_temp->GetXaxis()->SetTitle("#delta p_{T} = p_{T}^{reco, avoidleading} - #rho M");
        h_temp->GetYaxis()->SetTitle("Counts");
        // fit
        TF1 *func = new TF1("func","gaus",-15, 60);
        h_temp->Fit(func,"Q","",-15, 60);
        h_temp->Fit(func,"Q","",func->GetParameter(1)-2*func->GetParameter(2),func->GetParameter(1)+2*func->GetParameter(2));
        func->SetLineColor(kRed);


        // draw
        c->cd();
        gPad->SetLeftMargin(0.15);
        gPad->SetRightMargin(0.05);
        gPad->SetBottomMargin(0.05);
        gPad->SetTopMargin(0.05);
        h_temp->Draw();
        func->Draw("SAME");
        c->Update();
        c->SaveAs(Form("plots/iter_sub_avoidleading_cent%.0f-%.0f.png", cent_bins[icent], cent_bins[icent+1]));

        h1_reso_pt_iter_sub_avoidleading->SetBinContent(icent+1, func->GetParameter(2));                
        
    }


    TCanvas *c1 = new TCanvas("c1","c1");
    //   gPad->SetLogy();

    TLegend *leg = new TLegend(.2,.7,.55,.9);
    leg->SetFillStyle(0);
    leg->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    leg->AddEntry("","Au+Au #sqrt{s_{NN}}=200 GeV","");
    leg->AddEntry("","Random Cone #it{R} = 0.4, |#eta| < 0.4","");
    leg->AddEntry("","p_{T, hard} = 30 GeV. Dijet sim.");

    TLegend *cleg = new TLegend(.6,.75,.9,.9);
    cleg->SetFillStyle(0);
    int colors[] = {1,2,4};

    h1_reso_pt_area->SetLineColor(colors[0]);
    h1_reso_pt_area->SetMarkerColor(colors[0]);
    h1_reso_pt_area->Draw();
    cleg->AddEntry(h1_reso_pt_area,"Area","l");
    h1_reso_pt_mult->SetLineColor(colors[1]);
    h1_reso_pt_mult->SetMarkerColor(colors[1]);
    h1_reso_pt_mult->Draw("SAME");
    cleg->AddEntry(h1_reso_pt_mult,"Mult","l");
    h1_reso_pt_iter_sub->SetLineColor(colors[2]);
    h1_reso_pt_iter_sub->SetMarkerColor(colors[2]);
    h1_reso_pt_iter_sub->Draw("SAME");
    cleg->AddEntry(h1_reso_pt_iter_sub,"Iter","l");
    cleg->Draw();
    // leg->Draw();
    c1->SaveAs("plots/cent_reso.png");

     TCanvas *c2 = new TCanvas("c2","c2");
    //   gPad->SetLogy();

    cleg->Clear();
    h1_reso_pt_area_randomized->SetLineColor(colors[0]);
    h1_reso_pt_area_randomized->SetMarkerColor(colors[0]);
    h1_reso_pt_area_randomized->Draw();
    cleg->AddEntry(h1_reso_pt_area_randomized,"Area","l");
    h1_reso_pt_mult_randomized->SetLineColor(colors[1]);
    h1_reso_pt_mult_randomized->SetMarkerColor(colors[1]);
    h1_reso_pt_mult_randomized->Draw("SAME");
    cleg->AddEntry(h1_reso_pt_mult_randomized,"Multiplicity","l");
    h1_reso_pt_iter_sub_randomized->SetLineColor(colors[2]);
    h1_reso_pt_iter_sub_randomized->SetMarkerColor(colors[2]);
    h1_reso_pt_iter_sub_randomized->Draw("SAME");
    cleg->AddEntry(h1_reso_pt_iter_sub_randomized,"Iterative Subtraction","l");
    cleg->Draw();
    // leg->Draw();
    c2->SaveAs("plots/cent_reso_randomized.png");

    TCanvas *c3 = new TCanvas("c3","c3");
    //   gPad->SetLogy();

    cleg->Clear();
    h1_reso_pt_area_avoidleading->SetLineColor(colors[0]);
    h1_reso_pt_area_avoidleading->SetMarkerColor(colors[0]);
    h1_reso_pt_area_avoidleading->Draw();
    cleg->AddEntry(h1_reso_pt_area_avoidleading,"Area","l");
    h1_reso_pt_mult_avoidleading->SetLineColor(colors[1]);
    h1_reso_pt_mult_avoidleading->SetMarkerColor(colors[1]);
    h1_reso_pt_mult_avoidleading->Draw("SAME");
    cleg->AddEntry(h1_reso_pt_mult_avoidleading,"Multiplicity","l");
    h1_reso_pt_iter_sub_avoidleading->SetLineColor(colors[2]);
    h1_reso_pt_iter_sub_avoidleading->SetMarkerColor(colors[2]);
    h1_reso_pt_iter_sub_avoidleading->Draw("SAME");
    cleg->AddEntry(h1_reso_pt_iter_sub_avoidleading,"Iterative Subtraction","l");
    cleg->Draw();
    // leg->Draw();
    c3->SaveAs("plots/cent_reso_avoidleading.png");

    TCanvas *c4 = new TCanvas("c4","c4");
    //   gPad->SetLogy();

    cleg->Clear();
    h1_reso_pt_area->SetLineColor(colors[0]);
    h1_reso_pt_area->SetMarkerColor(colors[0]);
    
    
    h1_reso_pt_area_randomized->SetLineColor(colors[1]);
    h1_reso_pt_area_randomized->SetMarkerColor(colors[1]);
    h1_reso_pt_area_randomized->Draw();
    h1_reso_pt_area->Draw("SAME");
    cleg->AddEntry(h1_reso_pt_area,"RC","l");
    cleg->AddEntry(h1_reso_pt_area_randomized,"Randomized","l");
    h1_reso_pt_area_avoidleading->SetLineColor(colors[2]);
    h1_reso_pt_area_avoidleading->SetMarkerColor(colors[2]);
    h1_reso_pt_area_avoidleading->Draw("SAME");
    cleg->AddEntry(h1_reso_pt_area_avoidleading,"Avoid Leading","l");
    cleg->Draw();
    // leg->Draw();
    c4->SaveAs("plots/cent_reso_area.png");

    TCanvas *c5 = new TCanvas("c5","c5");
    //   gPad->SetLogy();

    cleg->Clear();
    h1_reso_pt_mult->SetLineColor(colors[0]);
    h1_reso_pt_mult->SetMarkerColor(colors[0]);
    h1_reso_pt_mult_randomized->SetLineColor(colors[1]);
    h1_reso_pt_mult_randomized->SetMarkerColor(colors[1]);
     h1_reso_pt_mult_randomized->Draw();
    h1_reso_pt_mult->Draw("SAME");
    cleg->AddEntry(h1_reso_pt_mult,"RC","l");
    cleg->AddEntry(h1_reso_pt_mult_randomized,"Randomized","l");
    h1_reso_pt_mult_avoidleading->SetLineColor(colors[2]);
    h1_reso_pt_mult_avoidleading->SetMarkerColor(colors[2]);
    h1_reso_pt_mult_avoidleading->Draw("SAME");
    cleg->AddEntry(h1_reso_pt_mult_avoidleading,"Avoid Leading","l");
    cleg->Draw();

    // leg->Draw();
    c5->SaveAs("plots/cent_reso_mult.png");

    TCanvas *c6 = new TCanvas("c6","c6");
    //   gPad->SetLogy();

    cleg->Clear();
    h1_reso_pt_iter_sub->SetLineColor(colors[0]);
    h1_reso_pt_iter_sub->SetMarkerColor(colors[0]);
  
    h1_reso_pt_iter_sub_randomized->SetLineColor(colors[1]);
    h1_reso_pt_iter_sub_randomized->SetMarkerColor(colors[1]);
    h1_reso_pt_iter_sub_randomized->Draw();
      h1_reso_pt_iter_sub->Draw("SAME");
    cleg->AddEntry(h1_reso_pt_iter_sub,"RC","l");
    cleg->AddEntry(h1_reso_pt_iter_sub_randomized,"Randomized","l");
    h1_reso_pt_iter_sub_avoidleading->SetLineColor(colors[2]);

    h1_reso_pt_iter_sub_avoidleading->SetMarkerColor(colors[2]);
    h1_reso_pt_iter_sub_avoidleading->Draw("SAME");
    cleg->AddEntry(h1_reso_pt_iter_sub_avoidleading,"Avoid Leading","l");
    cleg->Draw();
    // leg->Draw();
    c6->SaveAs("plots/cent_reso_iter_sub.png");


    // write histograms to file
    fout->cd();
    h2_reso_pt_area->Write();
    h1_reso_pt_area->Write();
    h2_reso_pt_mult->Write();
    h1_reso_pt_mult->Write();
    h2_reso_pt_iter_sub->Write();
    h1_reso_pt_iter_sub->Write();

    h2_reso_pt_area_randomized->Write();
    h1_reso_pt_area_randomized->Write();
    h2_reso_pt_mult_randomized->Write();
    h1_reso_pt_mult_randomized->Write();
    h2_reso_pt_iter_sub_randomized->Write();
    h1_reso_pt_iter_sub_randomized->Write();

    h2_reso_pt_area_avoidleading->Write();
    h1_reso_pt_area_avoidleading->Write();
    h2_reso_pt_mult_avoidleading->Write();
    h1_reso_pt_mult_avoidleading->Write();
    h2_reso_pt_iter_sub_avoidleading->Write();
    h1_reso_pt_iter_sub_avoidleading->Write();

    // close output file
    fout->Close();

    // close input file
    fin.Close();

  
    return 0;



}

