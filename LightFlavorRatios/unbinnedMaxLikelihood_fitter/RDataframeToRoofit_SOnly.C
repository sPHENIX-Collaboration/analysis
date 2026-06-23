#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
#ifndef __CINT__
#include <RooGlobalFunc.h>
#endif
#include <RooBifurGauss.h>
#include <RooDataHist.h>
#include <RooDataSet.h>
#include <RooGaussian.h>
#include <RooRealVar.h>
// #include <RooDoubleCB.h>
#include <RooAbsDataHelper.h>
#include <RooAddPdf.h>
#include <RooExponential.h>
#include <RooHist.h>
#include <RooPlot.h>
#include <RooStats/SPlot.h>

#include <TBranch.h>
#include <TCanvas.h>
#include <TChain.h>
#include <TCut.h>
#include <TFile.h>
#include <TH1F.h>
#include <TMath.h>
#include <TPad.h>
#include <TTree.h>

using namespace RooFit;
using namespace std;

void RDataframeToRoofit_SOnly(const bool doSnapshot,                 //
                          std::string snapshotName,              //
                          std::string inputfilename,             //
                          TCut selections,                       //
                          fitparam_config &fit_conf,             //
                          const std::string plotdir = "./figure" //
)
{
    RooMsgService::instance().setGlobalKillBelow(RooFit::ERROR);

    system(Form("mkdir -p %s", plotdir.c_str()));

    std::string dirName = snapshotName;
    std::string::size_type pos = dirName.find_last_of("/");
    if (pos != std::string::npos)
    {
        dirName = dirName.substr(0, pos);
        system(Form("mkdir -p %s", dirName.c_str()));
    }

    auto start = std::chrono::high_resolution_clock::now();

    ROOT::EnableImplicitMT();
    ROOT::RDataFrame df("DecayTree", inputfilename.c_str());

    auto filtered_df = df.Filter(selections.GetTitle());
    if (doSnapshot)
    {
        filtered_df.Snapshot("DecayTree", Form("%s", snapshotName.c_str()));
    }

    // print the number of entries in the dataframe
    int nEntries = filtered_df.Count().GetValue();
    std::cout << "Number of entries in the dataframe: " << nEntries << std::endl;

    FitParams::branch = fit_conf.branch;
    std::string xAxisTitle = fit_conf.decaystring + " candidate mass [GeV]";

    FitParams::minMass = fit_conf.minMass;
    FitParams::maxMass = fit_conf.maxMass;
    FitParams::mass.SetName(FitParams::branch.c_str());
    FitParams::mass.SetTitle("mass");
    FitParams::mass.setRange(FitParams::minMass, FitParams::maxMass);

    auto dataset_result = filtered_df.Book<float>(RooDataSetHelper("dataset", "dataset", RooArgSet(FitParams::mass)), {FitParams::branch.c_str()});
    RooDataSet *dataset = dataset_result.GetPtr();

    if (fit_conf.sigmodel == "Gaussian")
    {
        FitParams::mean.setVal(fit_conf.mean);
        FitParams::mean.setRange(fit_conf.mean_low, fit_conf.mean_high);
        FitParams::sigma.setVal(fit_conf.sigma);
        FitParams::sigma.setRange(fit_conf.sigma_low, fit_conf.sigma_high);
    }
    else if (fit_conf.sigmodel == "Voigtian")
    {
        FitParams::mean.setVal(fit_conf.mean);
        FitParams::mean.setRange(fit_conf.mean_low, fit_conf.mean_high);
        FitParams::sigma.setVal(fit_conf.sigma);
        FitParams::sigma.setRange(fit_conf.sigma_low, fit_conf.sigma_high);
        FitParams::width.setVal(fit_conf.width);
        FitParams::width.setRange(fit_conf.width_low, fit_conf.width_high);
    }
    else if (fit_conf.sigmodel == "CrystalBall")
    {
        FitParams::mean.setVal(fit_conf.mean);
        FitParams::mean.setRange(fit_conf.mean_low, fit_conf.mean_high);
        FitParams::sigma.setVal(fit_conf.sigma);
        FitParams::sigma.setRange(fit_conf.sigma_low, fit_conf.sigma_high);
        FitParams::alpha1.setVal(fit_conf.alpha1);
        FitParams::alpha1.setRange(fit_conf.alpha1_low, fit_conf.alpha1_high);
        FitParams::n1.setVal(fit_conf.n1);
        FitParams::n1.setRange(fit_conf.n1_low, fit_conf.n1_high);
    }
    else if (fit_conf.sigmodel == "DoubleCrystalBall")
    {
        FitParams::mean.setVal(fit_conf.mean);
        FitParams::mean.setRange(fit_conf.mean_low, fit_conf.mean_high);
        FitParams::sigma.setVal(fit_conf.sigma);
        FitParams::sigma.setRange(fit_conf.sigma_low, fit_conf.sigma_high);
        FitParams::alpha1.setVal(fit_conf.alpha1);
        FitParams::alpha1.setRange(fit_conf.alpha1_low, fit_conf.alpha1_high);
        FitParams::n1.setVal(fit_conf.n1);
        FitParams::n1.setRange(fit_conf.n1_low, fit_conf.n1_high);
        FitParams::alpha2.setVal(fit_conf.alpha2);
        FitParams::alpha2.setRange(fit_conf.alpha2_low, fit_conf.alpha2_high);
        FitParams::n2.setVal(fit_conf.n2);
        FitParams::n2.setRange(fit_conf.n2_low, fit_conf.n2_high);
        FitParams::frac.setVal(fit_conf.frac);
        FitParams::frac.setRange(0, 1);
    }
    else
    {
        std::cerr << "Unknown signal model type: " << fit_conf.sigmodel << ", returning nullptr. Please define the model." << std::endl;
    }

    RooAbsPdf *signal = signalModel(fit_conf.sigmodel, FitParams::mass);
    FitParams::nSig.setVal(float(nEntries));
    FitParams::nSig.setRange(float(nEntries)*0.95, float(nEntries)*1.05);
    RooAddPdf *model = new RooAddPdf("model", "model", RooArgList(*signal), RooArgList(FitParams::nSig));

    RooFitResult *m_fitres(0);
    m_fitres = model->fitTo(*dataset, Save(kTRUE), Extended(kTRUE), PrintLevel(-1));
    m_fitres->Print();
    // Save the fit result to a file
    TFile *fitresfile = new TFile(Form("%s/fitresults_signal.root", plotdir.c_str()), "RECREATE");
    fitresfile->cd();
    m_fitres->Write("fitres");
    fitresfile->Close();

    std::cout << "Making plots..." << std::endl;

    // make plot
    std::string plotTitle = "";
    RooPlot *frame = FitParams::mass.frame(Title(plotTitle.c_str())); // creating the frame
    RooBinning bins(FitParams::minMass, FitParams::maxMass);
    bins.addUniform(fit_conf.nBins, FitParams::minMass, FitParams::maxMass);
    dataset->plotOn(frame, DrawOption("PE1"), Binning(bins), XErrorSize(0), DataError(RooAbsData::SumW2), RooFit::Name("data")); // plotting the raw unbinned data on the frame
    signal->plotOn(frame, LineColor(TColor::GetColor("#FF7F0E")), RooFit::Name("signalpdf"));                                                           // plotting the fit onto the frame

    RooHist *pull = frame->pullHist();
    RooPlot *frame2 = FitParams::mass.frame(Title(""));
    frame2->addPlotable(pull, "PE1");

    std::cout << "Creating canvas..." << std::endl;

    TCanvas *c = new TCanvas("massFitCanvas", "massFitCanvas", 800, 800);
    TPad mainPad("mainPad", "mainPad", 0., 0.3, 1., 1.);
    mainPad.SetTopMargin(TopMargin);
    mainPad.SetBottomMargin(0);
    mainPad.Draw();
    TPad pullPad("pullPad", "pullPad", 0., 0.0, 1., 0.3);
    pullPad.SetBottomMargin(0.5);
    pullPad.SetTopMargin(0);
    pullPad.Draw();
    mainPad.cd();
    frame->SetMarkerStyle(kCircle);
    frame->SetMarkerSize(0.02);
    frame->SetLineWidth(1);
    frame->GetXaxis()->SetTitleSize(0);
    frame->GetXaxis()->SetLabelSize(0);
    frame->GetYaxis()->SetTitleSize(AxisTitleSize * textscale_pad1);
    frame->GetYaxis()->SetLabelSize(AxisLabelSize * textscale_pad1);
    frame->GetYaxis()->SetTitleOffset(1.2);
    frame->GetYaxis()->SetTitleFont(42);
    frame->GetYaxis()->SetLabelFont(42);
    TH1 *hdataset = dataset->createHistogram("hdataset", FitParams::mass, Binning(fit_conf.nBins, FitParams::minMass, FitParams::maxMass));
    // frame->GetYaxis()->SetRangeUser(hdataset->GetMinimum(0) * 0.85, hdataset->GetMaximum() * 1.4);
    frame->GetYaxis()->SetRangeUser(0.1, hdataset->GetMaximum() * 1.8);
    float binWidth = 1000. * (FitParams::maxMass - FitParams::minMass) / fit_conf.nBins;
    string yAxisTitle = "Candidates / (" + to_string_with_precision(binWidth, 1) + " MeV)";
    frame->GetYaxis()->SetTitle(yAxisTitle.c_str());
    frame->Draw();
    frame->Print();
    c->RedrawAxis();

    TLatex *datestamp = new TLatex();
    datestamp->SetTextSize(0.06);
    datestamp->SetTextAlign(kHAlignRight + kVAlignBottom);
    datestamp->SetNDC();
    datestamp->DrawLatex(1 - mainPad.GetRightMargin(),      //
                         1 - mainPad.GetTopMargin() + 0.01, //
                         getTime().c_str()                  //
    );

    TLegend *sphnxleg = new TLegend(mainPad.GetLeftMargin() + 0.03,   //
                                    1 - mainPad.GetTopMargin() - 0.2, //
                                    mainPad.GetLeftMargin() + 0.2,    //
                                    1 - mainPad.GetTopMargin() - 0.05 //
    );
    sphnxleg->SetTextAlign(kHAlignLeft + kVAlignCenter);
    sphnxleg->SetTextSize(0.06);
    sphnxleg->SetFillStyle(0);
    sphnxleg->AddEntry("", Form("#it{#bf{sPHENIX}} %s", prelimtext.c_str()), "");
    sphnxleg->AddEntry("", "p+p #sqrt{s_{NN}}=200 GeV", "");
    sphnxleg->Draw();

    TLegend *leg = new TLegend(1 - mainPad.GetRightMargin() - 0.27, //
                               1 - mainPad.GetTopMargin() - 0.2,   //
                               1 - mainPad.GetRightMargin() - 0.1,  //
                               1 - mainPad.GetTopMargin() - 0.06    //
    );
    leg->AddEntry(frame->findObject("data"), "Data", "PE2");
    leg->AddEntry(frame->findObject("signalpdf"), "Fit", "L");
    leg->SetFillColor(0);
    leg->SetFillStyle(0);
    leg->SetBorderSize(0);
    leg->SetTextSize(0.055);
    leg->Draw();

    // plot the significance
    float spacing = 0.05;
    TLatex *fitparams = new TLatex();
    fitparams->SetTextSize(0.045);
    fitparams->SetTextAlign(kHAlignLeft + kVAlignCenter);
    fitparams->SetNDC();
    fitparams->DrawLatex(mainPad.GetLeftMargin() + 0.07,                                                                     //
                         1 - mainPad.GetTopMargin() - 0.15 - spacing - 0.06,                                                  //
                         Form("#mu = %.0f #pm %.0g MeV", FitParams::mean.getVal() * 1000, FitParams::mean.getError() * 1000) //
    );
    fitparams->DrawLatex(mainPad.GetLeftMargin() + 0.07,                                                                          //
                         1 - mainPad.GetTopMargin() - 0.15 - spacing - 0.06 - 0.055,                                               //
                         Form("#sigma = %.2f #pm %.1g MeV", FitParams::sigma.getVal() * 1000, FitParams::sigma.getError() * 1000) //
    );

    gPad->Modified();
    pullPad.cd();
    frame2->SetMarkerStyle(kCircle);
    frame2->SetMarkerSize(0.02);
    frame2->SetTitle("");
    frame2->GetXaxis()->SetTitle(xAxisTitle.c_str());
    frame2->GetXaxis()->SetTitleOffset(1.3);
    frame2->GetXaxis()->SetTitleFont(42);
    frame2->GetXaxis()->SetTitleSize(AxisTitleSize * textscale_pad2);
    frame2->GetXaxis()->SetLabelFont(42);
    frame2->GetXaxis()->SetLabelSize(AxisLabelSize * textscale_pad2);
    frame2->GetYaxis()->SetTitle("Pull");
    frame2->GetYaxis()->SetTitleOffset(0.5);
    frame2->GetYaxis()->SetTitleFont(42);
    frame2->GetYaxis()->SetTitleSize(AxisTitleSize * textscale_pad2);
    frame2->GetYaxis()->SetLabelFont(42);
    frame2->GetYaxis()->SetLabelSize(AxisLabelSize * textscale_pad2);
    frame2->GetYaxis()->SetRangeUser(-6, 6);
    frame2->GetYaxis()->SetNdivisions(5);
    TLine *plusThreeLine = new TLine(FitParams::minMass, 3, FitParams::maxMass, 3);
    plusThreeLine->SetLineColor(1);
    plusThreeLine->SetLineStyle(2);
    plusThreeLine->SetLineWidth(2);
    TLine *zeroLine = new TLine(FitParams::minMass, 0, FitParams::maxMass, 0);
    zeroLine->SetLineColor(1);
    zeroLine->SetLineStyle(2);
    zeroLine->SetLineWidth(2);
    TLine *minusThreeLine = new TLine(FitParams::minMass, -3, FitParams::maxMass, -3);
    minusThreeLine->SetLineColor(1);
    minusThreeLine->SetLineStyle(2);
    minusThreeLine->SetLineWidth(2);
    frame2->Draw();
    plusThreeLine->Draw("same");
    zeroLine->Draw("same");
    minusThreeLine->Draw("same");

    std::cout << "Saving plots to " << plotdir << std::endl;

    vector<string> extensions = {".C", ".pdf", ".png"};
    for (auto &extension : extensions)
    {
        c->SaveAs(Form("%s/RDataframeToRoofit_%s_signal%s", plotdir.c_str(), FitParams::branch.c_str(), extension.c_str()));
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Elapsed time: " << elapsed.count() / 60. << " minutes" << std::endl;
}