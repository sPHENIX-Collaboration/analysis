#include <TTree.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>



void DrawAsymmetries(int Verbosity = 0)
{
    gStyle->SetOptStat(0);

    TFile *infile = TFile::Open("raw_trackTSSA.root");

    // Raw Asymmetries
    TH1D *h1pt_blue_rawANfit = (TH1D*)infile->Get("h1pt_blue_rawANfit");
    TH1D *h1pt_blue_pos_rawANfit = (TH1D*)infile->Get("h1pt_blue_pos_rawANfit");
    TH1D *h1pt_blue_neg_rawANfit = (TH1D*)infile->Get("h1pt_blue_neg_rawANfit");

    TH1D *h1pt_yellow_rawANfit = (TH1D*)infile->Get("h1pt_yellow_rawANfit");
    TH1D *h1pt_yellow_pos_rawANfit = (TH1D*)infile->Get("h1pt_yellow_pos_rawANfit");
    TH1D *h1pt_yellow_neg_rawANfit = (TH1D*)infile->Get("h1pt_yellow_neg_rawANfit");

    TH1D *h1xf_blue_rawANfit = (TH1D*)infile->Get("h1xf_blue_rawANfit");
    TH1D *h1xf_blue_pos_rawANfit = (TH1D*)infile->Get("h1xf_blue_pos_rawANfit");
    TH1D *h1xf_blue_neg_rawANfit = (TH1D*)infile->Get("h1xf_blue_neg_rawANfit");

    TH1D *h1xf_yellow_rawANfit = (TH1D*)infile->Get("h1xf_yellow_rawANfit");
    TH1D *h1xf_yellow_pos_rawANfit = (TH1D*)infile->Get("h1xf_yellow_pos_rawANfit");
    TH1D *h1xf_yellow_neg_rawANfit = (TH1D*)infile->Get("h1xf_yellow_neg_rawANfit");

    TCanvas *cAN_pt = new TCanvas("cAN_pt");
    cAN_pt->cd();

    h1pt_blue_rawANfit->SetMarkerStyle(8);
    h1pt_blue_rawANfit->SetMarkerColor(kBlue);
    h1pt_blue_rawANfit->SetLineColor(kBlue);

    h1pt_yellow_rawANfit->SetMarkerStyle(8);
    h1pt_yellow_rawANfit->SetMarkerColor(kYellow+2);
    h1pt_yellow_rawANfit->SetLineColor(kYellow+2);

    h1pt_blue_rawANfit->GetYaxis()->SetTitleOffset(1.3);

    h1pt_blue_rawANfit->Draw();
    h1pt_yellow_rawANfit->Draw("same");
    cAN_pt->SaveAs("rawAN_pt.png");

    TCanvas *cAN_pos_pt = new TCanvas("cAN_pos_pt");
    cAN_pos_pt->cd();

    h1pt_blue_pos_rawANfit->SetMarkerStyle(8);
    h1pt_blue_pos_rawANfit->SetMarkerColor(kBlue);
    h1pt_blue_pos_rawANfit->SetLineColor(kBlue);

    h1pt_yellow_pos_rawANfit->SetMarkerStyle(8);
    h1pt_yellow_pos_rawANfit->SetMarkerColor(kYellow+2);
    h1pt_yellow_pos_rawANfit->SetLineColor(kYellow+2);

    h1pt_blue_pos_rawANfit->GetYaxis()->SetTitleOffset(1.3);

    h1pt_blue_pos_rawANfit->Draw();
    h1pt_yellow_pos_rawANfit->Draw("same");
    cAN_pos_pt->SaveAs("rawAN_pos_pt.png");

    TCanvas *cAN_neg_pt = new TCanvas("cAN_neg_pt");
    cAN_neg_pt->cd();

    h1pt_blue_neg_rawANfit->SetMarkerStyle(8);
    h1pt_blue_neg_rawANfit->SetMarkerColor(kBlue);
    h1pt_blue_neg_rawANfit->SetLineColor(kBlue);

    h1pt_yellow_neg_rawANfit->SetMarkerStyle(8);
    h1pt_yellow_neg_rawANfit->SetMarkerColor(kYellow+2);
    h1pt_yellow_neg_rawANfit->SetLineColor(kYellow+2);

    h1pt_blue_neg_rawANfit->GetYaxis()->SetTitleOffset(1.3);

    h1pt_blue_neg_rawANfit->Draw();
    h1pt_yellow_neg_rawANfit->Draw("same");
    cAN_neg_pt->SaveAs("rawAN_neg_pt.png");

    TCanvas *cAN_xf = new TCanvas("cAN_xf");
    cAN_xf->cd();

    h1xf_blue_rawANfit->SetMarkerStyle(8);
    h1xf_blue_rawANfit->SetMarkerColor(kBlue);
    h1xf_blue_rawANfit->SetLineColor(kBlue);

    h1xf_yellow_rawANfit->SetMarkerStyle(8);
    h1xf_yellow_rawANfit->SetMarkerColor(kYellow+2);
    h1xf_yellow_rawANfit->SetLineColor(kYellow+2);

    h1xf_blue_rawANfit->GetYaxis()->SetTitleOffset(1.3);

    h1xf_blue_rawANfit->Draw();
    h1xf_yellow_rawANfit->Draw("same");
    cAN_xf->SaveAs("rawAN_xf.png");

    TCanvas *cAN_pos_xf = new TCanvas("cAN_pos_xf");
    cAN_pos_xf->cd();

    h1xf_blue_pos_rawANfit->SetMarkerStyle(8);
    h1xf_blue_pos_rawANfit->SetMarkerColor(kBlue);
    h1xf_blue_pos_rawANfit->SetLineColor(kBlue);

    h1xf_yellow_pos_rawANfit->SetMarkerStyle(8);
    h1xf_yellow_pos_rawANfit->SetMarkerColor(kYellow+2);
    h1xf_yellow_pos_rawANfit->SetLineColor(kYellow+2);

    h1xf_blue_pos_rawANfit->GetYaxis()->SetTitleOffset(1.3);

    h1xf_blue_pos_rawANfit->Draw();
    h1xf_yellow_pos_rawANfit->Draw("same");
    cAN_pos_xf->SaveAs("rawAN_pos_xf.png");

    TCanvas *cAN_neg_xf = new TCanvas("cAN_neg_xf");
    cAN_neg_xf->cd();

    h1xf_blue_neg_rawANfit->SetMarkerStyle(8);
    h1xf_blue_neg_rawANfit->SetMarkerColor(kBlue);
    h1xf_blue_neg_rawANfit->SetLineColor(kBlue);

    h1xf_yellow_neg_rawANfit->SetMarkerStyle(8);
    h1xf_yellow_neg_rawANfit->SetMarkerColor(kYellow+2);
    h1xf_yellow_neg_rawANfit->SetLineColor(kYellow+2);

    h1xf_blue_neg_rawANfit->GetYaxis()->SetTitleOffset(1.3);

    h1xf_blue_neg_rawANfit->Draw();
    h1xf_yellow_neg_rawANfit->Draw("same");
    cAN_neg_xf->SaveAs("rawAN_neg_xf.png");

}