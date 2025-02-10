#include <TCanvas.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TLine.h>
#include <TMath.h>
#include <TObjString.h>
#include <TRandom3.h>
#include <TTree.h>
#include <TTreeIndex.h>
#include <TString.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TText.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#define INCLUDE_VZ_RANGE
#define INCLUDE_ETA_RANGE
#include "bins.h"

#include "/cvmfs/sphenix.sdcc.bnl.gov/gcc-12.1.0/release/release_new/new/rootmacros/sPhenixStyle.C"

void convert(TH2 *h1)
{
    TH1D *hvz = (TH1D *)h1->ProjectionY("hvz");

    for (int i = 1; i <= h1->GetNbinsX(); ++i)
    {
        for (int j = 1; j <= h1->GetNbinsY(); ++j)
        {
            // double data_pdf = TMath::Gaus(hvz->GetBinCenter(j), -20.72, 6.390, 1);
            double data_pdf = TMath::Gaus(hvz->GetBinCenter(j), -4.03, 9.7, 1);
            if (h1->GetBinContent(i, j))
            {
                h1->SetBinContent(i, j, data_pdf);
            }
        }
    }

    delete hvz;
}

TLine *drawline(Double_t x1, Double_t y1, Double_t x2, Double_t y2)
{
    TLine *l = new TLine(x1, y1, x2, y2);
    l->SetLineColor(kRed);
    l->SetLineWidth(2);
    l->Draw();
    return l;
}

void drawhoutline(TH2 *h)
{
    auto xaxis = h->GetXaxis(), yaxis = h->GetYaxis();
    auto nx = xaxis->GetNbins(), ny = yaxis->GetNbins();
    int b[4][2] = {{0, 1}, {-1, 0}, {0, -1}, {1, 0}};
    for (int i = 0; i < nx; i++)
        for (int j = 0; j < ny; j++)
        {
            auto content = h->GetBinContent(i + 1, j + 1);
            for (int k = 0; k < 4; k++)
            {
                auto neighbor = h->GetBinContent(i + 1 + b[k][0], j + 1 + b[k][1]);
                if ((content || neighbor) && !(content && neighbor))
                {
                    auto cx = xaxis->GetBinCenter(i + 1), wx = xaxis->GetBinWidth(i + 1), cy = yaxis->GetBinCenter(j + 1), wy = yaxis->GetBinWidth(j + 1), fx = (double)b[k][0], fy = (double)b[k][1];
                    drawline(cx + wx * (fx + fy) / 2., cy + wy * (fy + fx) / 2, cx + wx * (fx - fy) / 2., cy + wy * (fy - fx) / 2.);
                }
            }
        }
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        std::cout << "Usage: ./GeoAccepCorr [datatreefile (minitree)] [simtreefile (minitree)] [outfilename]" << std::endl;
        return 0;
    }

    for (int i = 0; i < argc; i++)
    {
        std::cout << "argv[" << i << "] = " << argv[i] << std::endl;
    }

    SetsPhenixStyle();

    TString datatreefile = TString(argv[1]);
    TString simtreefile = TString(argv[2]);
    TString outfilename = TString(argv[3]);

    TFile *fdata = new TFile(datatreefile, "READ");
    TTree *tdata = (TTree *)fdata->Get("minitree");

    TFile *fsim = new TFile(simtreefile, "READ");
    TTree *tsim = (TTree *)fsim->Get("minitree");

    int nfeta = neta * 100;
    int nfvz = nvz * 100;

    auto evtsel = "PV_z>=-10&&PV_z<=10";

    TH2D *hM_data = new TH2D("hM_data", "hM_data", nfeta, etamin, etamax, nfvz, vzmin, vzmax);
    tdata->Project("hM_data", "PV_z:recotklraw_eta", evtsel, "");
    convert(hM_data);
    TH2D *hM_data_coarse = (TH2D *)hM_data->Clone("hM_data_coarse");
    hM_data_coarse->RebinX(nfeta / neta);
    hM_data_coarse->RebinY(nfvz / nvz);

    TH2D *hM_sim = new TH2D("hM_sim", "hM_sim", nfeta, etamin, etamax, nfvz, vzmin, vzmax);
    tsim->Project("hM_sim", "PV_z:recotklraw_eta", evtsel, "");
    convert(hM_sim);
    TH2D *hM_sim_coarse = (TH2D *)hM_sim->Clone("hM_sim_coarse");
    hM_sim_coarse->RebinX(nfeta / neta);
    hM_sim_coarse->RebinY(nfvz / nvz);

    TH2D *hM_ratio = (TH2D *)hM_sim_coarse->Clone("hM_ratio");
    hM_ratio->Divide(hM_data_coarse);
    // hM_ratio->SetStats(0);
    // TH2D *hM_ratio = new TH2D("hM_ratio", "hM_ratio", neta, etamin, etamax, nvz, vzmin, vzmax);

    auto ext_accep_map = (TH2D *)hM_ratio->Clone("ext_accep_map");
    for (int i = 1; i <= neta; i++)
    {
        for (int j = 1; j <= nvz; j++)
        {
            // calculate uncertainty
            double data = hM_data_coarse->GetBinContent(i, j);
            double sim = hM_sim_coarse->GetBinContent(i, j);
            double data_err = hM_data_coarse->GetBinError(i, j);
            double sim_err = hM_sim_coarse->GetBinError(i, j);
            double ratio = -1E9;
            double ratio_err = -1E9;
            if (data == 0 || sim == 0)
            {
                ratio = 0;
                ratio_err = 0;
            }
            else
            {
                ratio = sim / data;
                ratio_err = ratio * sqrt(pow(sim_err / sim, 2) + pow(data_err / data, 2));
            }
            // hM_ratio->SetBinContent(i, j, ratio);
            hM_ratio->SetBinError(i, j, ratio_err);
            std::cout << "Bin (eta, vtxz) = (" << hM_data_coarse->GetXaxis()->GetBinCenter(i) << ", " << hM_data_coarse->GetYaxis()->GetBinCenter(j) << "): data = " << data << " +/- " << data_err << ", sim = " << sim << " +/- " << sim_err << ", ratio = " << ratio << " +/- " << ratio_err << std::endl;

            if (hM_ratio->GetBinContent(i, j) < 0.75 || hM_ratio->GetBinContent(i, j) > 1.25)
            {
                ext_accep_map->SetBinContent(i, j, 0);
            }
            else
            {
                ext_accep_map->SetBinContent(i, j, 1);
            }
            ext_accep_map->SetBinError(i, j, 0);
        }
    }

    // print out for acceptance.h
    for (int z = nvz; z >= 1; z--)
    {
        for (int x = 1; x <= neta; x++)
        {
            std::cout << ext_accep_map->GetBinContent(x, z) << ",";
        }
        std::cout << std::endl;
    }

    TCanvas *c = new TCanvas("c", "c", 800, 700);
    gPad->SetRightMargin(0.16);
    gPad->SetTopMargin(0.08);
    c->cd();
    hM_ratio->GetXaxis()->SetTitle("#eta");
    hM_ratio->GetYaxis()->SetTitle("vtx_{Z} [cm]");
    hM_ratio->GetZaxis()->SetRangeUser(hM_ratio->GetMinimum(0)*0.9, hM_ratio->GetMaximum()*1.1);
    gStyle->SetPaintTextFormat("1.3f");
    hM_ratio->SetContour(1000);
    hM_ratio->SetMarkerSize(0.5);
    hM_ratio->Draw("colztext");
    drawhoutline(ext_accep_map);
    c->SaveAs(outfilename+".pdf");

    c->Clear();
    c->cd();
    hM_data->GetXaxis()->SetTitle("#eta");
    hM_data->GetYaxis()->SetTitle("vtx_{Z} [cm]");
    hM_data->SetContour(1000);
    hM_data->Draw("colz");
    // TLegend *l = new TLegend(0.2, 0.2, 0.4, 0.4);
    // l->SetTextSize(0.04);
    // l->SetFillStyle(0);
    // l->AddEntry("", "Data", "");
    // l->Draw();
    TText *t = new TText();
    // right and bottom adjusted
    t->SetTextAlign(31);
    t->SetTextSize(0.04);
    t->DrawTextNDC(1-gPad->GetRightMargin(), (1 - gPad->GetTopMargin()) + 0.01, "Data");
    c->SaveAs(outfilename+"_hM_data.pdf");
    c->SaveAs(outfilename+"_hM_data.png");

    // l->Clear();
    c->Clear();
    c->cd();
    hM_sim->GetXaxis()->SetTitle("#eta");
    hM_sim->GetYaxis()->SetTitle("vtx_{Z} [cm]");
    hM_sim->SetContour(1000);
    hM_sim->Draw("colz");
    // l->AddEntry("", "Simulation", "");
    // l->Draw();
    t->DrawTextNDC(1-gPad->GetRightMargin(), (1 - gPad->GetTopMargin()) + 0.01, "Simulation");
    c->SaveAs(outfilename+"_hM_sim.pdf");
    c->SaveAs(outfilename+"_hM_sim.png");

    // l->Clear();
    c->Clear();
    c->cd();
    hM_data_coarse->GetXaxis()->SetTitle("#eta");
    hM_data_coarse->GetYaxis()->SetTitle("vtx_{Z} [cm]");
    hM_data_coarse->SetContour(1000);
    hM_data_coarse->Draw("colz");
    // l->AddEntry("", "Data", "");
    // l->Draw();
    t->DrawTextNDC(1-gPad->GetRightMargin(), (1 - gPad->GetTopMargin()) + 0.01, "Data");
    c->SaveAs(outfilename+"_hM_data_coarse.pdf");

    // l->Clear();
    c->Clear();
    c->cd();
    hM_sim_coarse->GetXaxis()->SetTitle("#eta");
    hM_sim_coarse->GetYaxis()->SetTitle("vtx_{Z} [cm]");
    hM_sim_coarse->SetContour(1000);
    hM_sim_coarse->Draw("colz");
    // l->AddEntry("", "Simulation", "");
    // l->Draw();
    t->DrawTextNDC(1-gPad->GetRightMargin(), (1 - gPad->GetTopMargin()) + 0.01, "Simulation");
    c->SaveAs(outfilename+"_hM_sim_coarse.pdf");

    TFile *fout = new TFile(outfilename+".root", "RECREATE");
    fout->cd();
    c->Write();
    hM_data->Write();
    hM_sim->Write();
    hM_data_coarse->Write();
    hM_sim_coarse->Write();
    hM_ratio->Write();
    ext_accep_map->Write();
    fout->Close();
}