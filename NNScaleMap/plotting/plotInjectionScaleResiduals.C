#include <TCanvas.h>
#include <TFile.h>
#include <TGaxis.h>
#include <TH2.h>
#include <TLatex.h>
#include <TObject.h>
#include <TPad.h>
#include <TStyle.h>
#include <TSystem.h>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

void drawResidualPadLabel(const double pt, const int charge, const bool grid)
{
    TLatex label;
    label.SetNDC();
    label.SetTextFont(42);
    label.SetTextSize(grid ? 0.045 : 0.035);

    const double x = gPad->GetLeftMargin();
    const double y = 1 - gPad->GetTopMargin() + 0.025;
    label.DrawLatex(x, y, Form("p_{T} = %.2f GeV, 1/p_{T} = %.2f GeV^{-1}, q = %+d", pt, 1.0 / pt, charge));
}

void plotInjectionScaleResiduals(std::string injectionClosureFile = "/home/hjheng/Documents/sphenix/NN-Momentum-Calibration/Injection-Pythia/closure_diagnostics-kshort/kshort_closure.root",
    std::string learnedMapFile = "/home/hjheng/Documents/sphenix/NN-Momentum-Calibration/NN-training/calib_out_pythiaInjection_20260805/kappa_map.root", std::string outdir = "")
{
    const std::vector<double> plotPt = {0.3, 0.5, 1.0, 2.0, 3.0};
    const std::vector<std::string> ptTag = {"pt0p30", "pt0p50", "pt1p00", "pt2p00", "pt3p00"};
    const int charge[2] = {+1, -1};
    const int injectionChargeIndex[2] = {0, 1};
    const std::string learnedChargeName[2] = {"qplus", "qminus"};

    if (gSystem->AccessPathName(learnedMapFile.c_str(), kReadPermission))
    {
        const std::string singularName = "kappa_map.root";
        const std::size_t pos = learnedMapFile.rfind(singularName);
        if (pos != std::string::npos && pos + singularName.size() == learnedMapFile.size())
        {
            std::string fallback = learnedMapFile;
            fallback.replace(pos, singularName.size(), "kappa_maps.root");
            if (!gSystem->AccessPathName(fallback.c_str(), kReadPermission))
            {
                std::cout << "Requested learned map " << learnedMapFile << " was not found; using " << fallback << " instead." << std::endl;
                learnedMapFile = fallback;
            }
        }
    }

    if (outdir.empty())
    {
        std::string learnedDir = ".";
        const std::size_t slash = learnedMapFile.find_last_of('/');
        if (slash != std::string::npos)
            learnedDir = learnedMapFile.substr(0, slash);
        outdir = learnedDir + "/injection_scale_residuals";
    }
    gSystem->mkdir(outdir.c_str(), true);

    TGaxis::SetMaxDigits(3);
    gStyle->SetOptStat(0);
    gStyle->SetPalette(kLightTemperature);

    std::unique_ptr<TFile> injection(TFile::Open(injectionClosureFile.c_str(), "READ"));
    if (!injection || injection->IsZombie())
    {
        std::cerr << "Could not open injection closure file: " << injectionClosureFile << std::endl;
        return;
    }

    std::unique_ptr<TFile> learned(TFile::Open(learnedMapFile.c_str(), "READ"));
    if (!learned || learned->IsZombie())
    {
        std::cerr << "Could not open learned scale-map file: " << learnedMapFile << std::endl;
        return;
    }

    const std::string residualRootFile = outdir + "/injection_scale_residuals.root";
    std::unique_ptr<TFile> output(TFile::Open(residualRootFile.c_str(), "RECREATE"));
    if (!output || output->IsZombie())
    {
        std::cerr << "Could not create residual output file: " << residualRootFile << std::endl;
        return;
    }

    std::vector<TH2 *> residuals;
    std::vector<int> residualCharge;
    std::vector<double> residualPt;
    std::vector<std::string> residualStem;
    double maxAbsResidual = 0.0;

    for (std::size_t ipt = 0; ipt < plotPt.size(); ++ipt)
    {
        for (int icharge = 0; icharge < 2; ++icharge)
        {
            const std::string injectionKey = Form("diagnostics/h_pt_scale_q%d_pt%d", injectionChargeIndex[icharge], static_cast<int>(ipt));
            const std::string learnedKey = "kappa_" + learnedChargeName[icharge] + "_" + ptTag[ipt];
            TH2 *injectedPtScale = dynamic_cast<TH2 *>(injection->Get(injectionKey.c_str()));
            TH2 *learnedPtScale = dynamic_cast<TH2 *>(learned->Get(learnedKey.c_str()));
            if (!injectedPtScale || !learnedPtScale)
            {
                std::cerr << "Missing histogram pair: " << injectionKey << " and " << learnedKey << std::endl;
                return;
            }

            if (injectedPtScale->GetNbinsX() != learnedPtScale->GetNbinsX() || injectedPtScale->GetNbinsY() != learnedPtScale->GetNbinsY() ||
                std::abs(injectedPtScale->GetXaxis()->GetXmin() - learnedPtScale->GetXaxis()->GetXmin()) > 1.0e-9 ||
                std::abs(injectedPtScale->GetXaxis()->GetXmax() - learnedPtScale->GetXaxis()->GetXmax()) > 1.0e-9 ||
                std::abs(injectedPtScale->GetYaxis()->GetXmin() - learnedPtScale->GetYaxis()->GetXmin()) > 1.0e-9 ||
                std::abs(injectedPtScale->GetYaxis()->GetXmax() - learnedPtScale->GetYaxis()->GetXmax()) > 1.0e-9)
            {
                std::cerr << "Injection and learned scale maps do not have matching eta-phi binning for " << learnedKey << std::endl;
                return;
            }

            const std::string stem = "residual_" + learnedChargeName[icharge] + "_" + ptTag[ipt];
            TH2 *residual = static_cast<TH2 *>(learnedPtScale->Clone(stem.c_str()));
            residual->SetDirectory(nullptr);
            residual->Reset("ICES");
            residual->SetStats(false);
            residual->SetTitle(Form("Injected #times learned p_{T} scale residual, q = %+d, p_{T} = %.2f GeV;#eta;#phi;injected scale #times learned scale - 1", charge[icharge], plotPt[ipt]));

            for (int ix = 1; ix <= residual->GetNbinsX(); ++ix)
            {
                for (int iy = 1; iy <= residual->GetNbinsY(); ++iy)
                {
                    const double injectedMultiplier = 1.0 + injectedPtScale->GetBinContent(ix, iy) / 100.0;
                    const double value = injectedMultiplier * learnedPtScale->GetBinContent(ix, iy) - 1.0;
                    residual->SetBinContent(ix, iy, value);
                    maxAbsResidual = std::max(maxAbsResidual, std::abs(value));
                }
            }

            output->cd();
            residual->Write("", TObject::kOverwrite);
            residuals.push_back(residual);
            residualCharge.push_back(charge[icharge]);
            residualPt.push_back(plotPt[ipt]);
            residualStem.push_back(stem);
        }
    }

    if (maxAbsResidual <= 0.0)
        maxAbsResidual = 1.0e-12;
    // const double zmax = (maxAbsResidual > 1.0e-2) ? 1.05 * maxAbsResidual : 1.0e-2;
    const double zmax = maxAbsResidual;

    for (std::size_t i = 0; i < residuals.size(); ++i)
    {
        TCanvas canvas(Form("c_%s", residualStem[i].c_str()), residuals[i]->GetTitle(), 850, 740);
        canvas.SetTicks(1, 1);
        canvas.SetLeftMargin(0.15);
        canvas.SetRightMargin(0.22);
        // canvas.SetBottomMargin(0.13);
        canvas.SetTopMargin(0.07);

        residuals[i]->SetMinimum(-zmax);
        residuals[i]->SetMaximum(+zmax);
        residuals[i]->SetContour(255);
        residuals[i]->GetZaxis()->SetTitle("scale residual");
        residuals[i]->GetZaxis()->SetTitleOffset(1.6);
        residuals[i]->Draw("COLZ");
        drawResidualPadLabel(residualPt[i], residualCharge[i], false);

        canvas.SaveAs((outdir + "/" + residualStem[i] + ".png").c_str());
        canvas.SaveAs((outdir + "/" + residualStem[i] + ".pdf").c_str());
    }

    TCanvas grid("c_injection_scale_residuals", "Injection scale residuals", 360 * plotPt.size(), 680);
    grid.Divide(static_cast<int>(plotPt.size()), 2, 0.001, 0.001);
    for (std::size_t i = 0; i < residuals.size(); ++i)
    {
        int ptIndex = 0;
        for (std::size_t ipt = 0; ipt < plotPt.size(); ++ipt)
        {
            if (std::abs(plotPt[ipt] - residualPt[i]) < 1.0e-9)
            {
                ptIndex = static_cast<int>(ipt);
                break;
            }
        }

        const int row = residualCharge[i] > 0 ? 0 : 1;
        grid.cd(row * static_cast<int>(plotPt.size()) + ptIndex + 1);
        gPad->SetTicks(1, 1);
        gPad->SetLeftMargin(0.13);
        gPad->SetRightMargin(0.18);
        // gPad->SetTopMargin(0.12);
        gPad->SetBottomMargin(0.12);

        residuals[i]->SetMinimum(-zmax);
        residuals[i]->SetMaximum(+zmax);
        residuals[i]->GetZaxis()->SetTitleOffset(1.5);
        residuals[i]->Draw("COLZ");
        drawResidualPadLabel(residualPt[i], residualCharge[i], true);
    }

    grid.SaveAs((outdir + "/all_injection_scale_residuals.png").c_str());
    grid.SaveAs((outdir + "/all_injection_scale_residuals.pdf").c_str());
    output->cd();
    grid.Write("", TObject::kOverwrite);
    output->Close();

    std::cout << "Saved " << residuals.size() << " residual maps to " << outdir << std::endl;
    std::cout << "Residual ROOT file: " << residualRootFile << std::endl;
    std::cout << "Compared (1 + injected p_{T} scale[%]/100) * learned p_{T} scale - 1." << std::endl;

    for (TH2 *residual : residuals)
        delete residual;
}
