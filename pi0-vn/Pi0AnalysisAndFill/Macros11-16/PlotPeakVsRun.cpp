#include <TCanvas.h>
#include <TFile.h>
#include <TH1F.h>
#include <TF1.h>
#include <TLatex.h>
#include <iomanip> // for std::setw and std::setprecision

void PlotPeakVsRun() {
    // Load the root file
    TFile *file = new TFile("/Users/patsfan753/Desktop/Pi0_Analysis/IndividualInvarSpect_output.root", "READ");

    // Extract hPi0Mass_0
    TH1F *hPi0Mass21518 = (TH1F*)file->Get("21518");
    TH1F *hPi0Mass21520 = (TH1F*)file->Get("21520");
    TH1F *hPi0Mass21598 = (TH1F*)file->Get("21598");
    TH1F *hPi0Mass21599 = (TH1F*)file->Get("21599");
    TH1F *hPi0Mass21615 = (TH1F*)file->Get("21615");
    TH1F *hPi0Mass21796 = (TH1F*)file->Get("21796");
    TH1F *hPi0Mass21813 = (TH1F*)file->Get("21813");
    TH1F *hPi0Mass21889 = (TH1F*)file->Get("21889");
    TH1F *hPi0Mass21891 = (TH1F*)file->Get("21891");
    TH1F *hPi0Mass22949 = (TH1F*)file->Get("22949");
    TH1F *hPi0Mass22950 = (TH1F*)file->Get("22950");
    TH1F *hPi0Mass22951 = (TH1F*)file->Get("22951");
    TH1F *hPi0Mass22979 = (TH1F*)file->Get("22979");
    TH1F *hPi0Mass22982 = (TH1F*)file->Get("22982");
    

    // Set y-axis range
    hPi0Mass21518->GetYaxis()->SetRangeUser(0,60);
    hPi0Mass21520->GetYaxis()->SetRangeUser(0,100);
    hPi0Mass21598->GetYaxis()->SetRangeUser(0,150);
    hPi0Mass21599->GetYaxis()->SetRangeUser(0,150);
    hPi0Mass21615->GetYaxis()->SetRangeUser(0,200);
    hPi0Mass21796->GetYaxis()->SetRangeUser(0,150);
    hPi0Mass21813->GetYaxis()->SetRangeUser(0,350);
    hPi0Mass21889->GetYaxis()->SetRangeUser(0,250);
    hPi0Mass21891->GetYaxis()->SetRangeUser(0,120);
    hPi0Mass22949->GetYaxis()->SetRangeUser(0,100);
    hPi0Mass22950->GetYaxis()->SetRangeUser(0,120);
    hPi0Mass22951->GetYaxis()->SetRangeUser(0,120);
    hPi0Mass22979->GetYaxis()->SetRangeUser(0,120);
    hPi0Mass22982->GetYaxis()->SetRangeUser(0,100);

    TF1 *totalFit21518 = new TF1("totalFit21518", "gaus(0) + pol4(3)", 0.08, 0.4);
    TF1 *totalFit21520 = new TF1("totalFit21520", "gaus(0) + pol4(3)", 0.08, 0.4);
    TF1 *totalFit21598 = new TF1("totalFit21598", "gaus(0) + pol4(3)", 0.08, 0.4);
    TF1 *totalFit21599 = new TF1("totalFit21599", "gaus(0) + pol4(3)", 0.08, 0.4);
    TF1 *totalFit21615 = new TF1("totalFit21615", "gaus(0) + pol4(3)", 0.08, 0.4);
    TF1 *totalFit21796 = new TF1("totalFit21796", "gaus(0) + pol4(3)", 0.08, 0.4);
    TF1 *totalFit21813 = new TF1("totalFit21813", "gaus(0) + pol4(3)", 0.08, 0.4);
    TF1 *totalFit21889 = new TF1("totalFit21889", "gaus(0) + pol4(3)", 0.08, 0.4);
    TF1 *totalFit21891 = new TF1("totalFit21891", "gaus(0) + pol4(3)", 0.08, 0.4);
    TF1 *totalFit22949 = new TF1("totalFit22949", "gaus(0) + pol4(3)", 0.08, 0.4);
    TF1 *totalFit22950 = new TF1("totalFit22950", "gaus(0) + pol4(3)", 0.08, 0.4);
    TF1 *totalFit22951 = new TF1("totalFit22951", "gaus(0) + pol4(3)", 0.08, 0.4);
    TF1 *totalFit22979 = new TF1("totalFit22979", "gaus(0) + pol4(3)", 0.08, 0.4);
    TF1 *totalFit22982 = new TF1("totalFit22982", "gaus(0) + pol4(3)", 0.08, 0.4);
    
    
    // Initialize fit parameters based on the histogram, will need to vary for different runs which is why created seperate fits
    totalFit21518->SetParameter(0, 20); // amplitude, around the peak height
    totalFit21518->SetParameter(1, 0.14); // mean, centered around the peak
    totalFit21518->SetParameter(2, 0.03); // sigma, roughly estimating from plot
    totalFit21518->SetParLimits(1, 0.13, 0.15); // limits for mean
    
    totalFit21520->SetParameter(0, 20); // amplitude, around the peak height
    totalFit21520->SetParameter(1, 0.148); // mean, centered around the peak
    totalFit21520->SetParameter(2, 0.03); // sigma, roughly estimating from plot
    totalFit21520->SetParLimits(1, 0.14, 0.16); // limits for mean
    
    totalFit21598->SetParameter(0, 58); // amplitude, around the peak height
    totalFit21598->SetParameter(1, 0.142); // mean, centered around the peak
    totalFit21598->SetParameter(2, 0.02); // sigma, roughly estimating from plot
    totalFit21598->SetParLimits(1, 0.135, 0.145); // limits for mean
    
    totalFit21599->SetParameter(0, 20); // amplitude, around the peak height
    totalFit21599->SetParameter(1, 0.148); // mean, centered around the peak
    totalFit21599->SetParameter(2, 0.03); // sigma, roughly estimating from plot
    totalFit21599->SetParLimits(1, 0.14, 0.16); // limits for mean
    
    totalFit21615->SetParameter(0, 20); // amplitude, around the peak height
    totalFit21615->SetParameter(1, 0.148); // mean, centered around the peak
    totalFit21615->SetParameter(2, 0.03); // sigma, roughly estimating from plot
    totalFit21615->SetParLimits(1, 0.14, 0.16); // limits for mean
    
    totalFit21796->SetParameter(0, 100); // amplitude, around the peak height
    totalFit21796->SetParameter(1, 0.16); // mean, centered around the peak
    totalFit21796->SetParameter(2, 0.03); // sigma, roughly estimating from plot
    totalFit21796->SetParLimits(1, 0.15, 0.17); // limits for mean
    
    totalFit21813->SetParameter(0, 100); // amplitude, around the peak height
    totalFit21813->SetParameter(1, 0.16); // mean, centered around the peak
    totalFit21813->SetParameter(2, 0.03); // sigma, roughly estimating from plot
    totalFit21813->SetParLimits(1, 0.15, 0.17); // limits for mean
    
    totalFit21889->SetParameter(0, 98); // amplitude, around the peak height
    totalFit21889->SetParameter(1, 0.148); // mean, centered around the peak
    totalFit21889->SetParameter(2, 0.03); // sigma, roughly estimating from plot
    totalFit21889->SetParLimits(1, 0.14, 0.16); // limits for mean
    
    totalFit21891->SetParameter(0, 100); // amplitude, around the peak height
    totalFit21891->SetParameter(1, 0.16); // mean, centered around the peak
    totalFit21891->SetParameter(2, 0.03); // sigma, roughly estimating from plot
    totalFit21891->SetParLimits(1, 0.15, 0.17); // limits for mean
    
    totalFit22949->SetParameter(0, 100); // amplitude, around the peak height
    totalFit22949->SetParameter(1, 0.16); // mean, centered around the peak
    totalFit22949->SetParameter(2, 0.03); // sigma, roughly estimating from plot
    totalFit22949->SetParLimits(1, 0.15, 0.17); // limits for mean
    
    totalFit22950->SetParameter(0, 100); // amplitude, around the peak height
    totalFit22950->SetParameter(1, 0.16); // mean, centered around the peak
    totalFit22950->SetParameter(2, 0.03); // sigma, roughly estimating from plot
    totalFit22950->SetParLimits(1, 0.15, 0.17); // limits for mean
    
    totalFit22951->SetParameter(0, 100); // amplitude, around the peak height
    totalFit22951->SetParameter(1, 0.16); // mean, centered around the peak
    totalFit22951->SetParameter(2, 0.03); // sigma, roughly estimating from plot
    totalFit22951->SetParLimits(1, 0.15, 0.17); // limits for mean
    
    totalFit22979->SetParameter(0, 100); // amplitude, around the peak height
    totalFit22979->SetParameter(1, 0.16); // mean, centered around the peak
    totalFit22979->SetParameter(2, 0.03); // sigma, roughly estimating from plot
    totalFit22979->SetParLimits(1, 0.15, 0.17); // limits for mean
    
    totalFit22982->SetParameter(0, 100); // amplitude, around the peak height
    totalFit22982->SetParameter(1, 0.16); // mean, centered around the peak
    totalFit22982->SetParameter(2, 0.03); // sigma, roughly estimating from plot
    totalFit22982->SetParLimits(1, 0.15, 0.17); // limits for mean

    
    // Fit
    hPi0Mass21518->Fit("totalFit21518", "R+");
    hPi0Mass21520->Fit("totalFit21520", "R+");
    hPi0Mass21598->Fit("totalFit21598", "R+");
    hPi0Mass21599->Fit("totalFit21599", "R+");
    hPi0Mass21615->Fit("totalFit21615", "R+");
    hPi0Mass21796->Fit("totalFit21796", "R+");
    hPi0Mass21813->Fit("totalFit21813", "R+");
    hPi0Mass21889->Fit("totalFit21889", "R+");
    hPi0Mass21891->Fit("totalFit21891", "R+");
    hPi0Mass22949->Fit("totalFit22949", "R+");
    hPi0Mass22950->Fit("totalFit22950", "R+");
    hPi0Mass22951->Fit("totalFit22951", "R+");
    hPi0Mass22979->Fit("totalFit22979", "R+");
    hPi0Mass22982->Fit("totalFit22982", "R+");
    
    
    
    // Extract the mean (parameter 1) of the Gaussian fit
    double fitMean21518 = totalFit21518->GetParameter(1);
    // Get the standard deviation (RMS) for the Gaussian component of the fit
    double fitSigma21518 = totalFit21518->GetParameter(2);
    
    double fitMean21520 = totalFit21520->GetParameter(1);
    double fitSigma21520 = totalFit21520->GetParameter(2);
    
    double fitMean21598 = totalFit21598->GetParameter(1);
    double fitSigma21598 = totalFit21598->GetParameter(2);
    
    double fitMean21599 = totalFit21599->GetParameter(1);
    double fitSigma21599 = totalFit21599->GetParameter(2);
    
    double fitMean21615 = totalFit21615->GetParameter(1);
    double fitSigma21615 = totalFit21615->GetParameter(2);
    
    double fitMean21796 = totalFit21796->GetParameter(1);
    double fitSigma21796 = totalFit21796->GetParameter(2);
    
    double fitMean21813 = totalFit21813->GetParameter(1);
    double fitSigma21813 = totalFit21813->GetParameter(2);
    
    double fitMean21889 = totalFit21889->GetParameter(1);
    double fitSigma21889 = totalFit21889->GetParameter(2);
    
    double fitMean21891 = totalFit21891->GetParameter(1);
    double fitSigma21891 = totalFit21891->GetParameter(2);
    
    double fitMean22949 = totalFit22949->GetParameter(1);
    double fitSigma22949 = totalFit22949->GetParameter(2);
    
    double fitMean22950 = totalFit22950->GetParameter(1);
    double fitSigma22950 = totalFit22950->GetParameter(2);
    
    double fitMean22951 = totalFit22951->GetParameter(1);
    double fitSigma22951 = totalFit22951->GetParameter(2);
    
    double fitMean22979 = totalFit22979->GetParameter(1);
    double fitSigma22979 = totalFit22979->GetParameter(2);
    
    double fitMean22982 = totalFit22982->GetParameter(1);
    double fitSigma22982 = totalFit22982->GetParameter(2);
    

    // Create canvas and draw
    TCanvas *canvas21518 = new TCanvas("canvas21518", "Pi0 Mass Distribution 21518", 900, 600);
    hPi0Mass21518->SetMarkerStyle(20);
    hPi0Mass21518->SetMarkerSize(1.0);
    hPi0Mass21518->SetMarkerColor(kBlack);
    hPi0Mass21518->Draw("PE");

    // Display information with TLatex LABELLING IS FOR INCLUSIVE CUTS (WHAT IS BEING INCLUDED)
    TLatex latex21518;
    latex21518.SetNDC();
    latex21518.SetTextSize(0.037);
    latex21518.DrawLatex(0.13, 0.86, "Cuts (Inclusive):");
    latex21518.DrawLatex(0.13, 0.82, "#Delta R > 0.08");
    latex21518.DrawLatex(0.13, 0.78, "Asymmetry < 0.5");
    latex21518.DrawLatex(0.13, 0.74, "#chi^{2} < 4");
    latex21518.DrawLatex(0.13, 0.70, "53640.9 <= MBD Charge < 109768");
    latex21518.DrawLatex(0.13, 0.66, "2 <= p_{T} < 3 GeV");
    latex21518.DrawLatex(0.13, 0.61, "E > 1.0 GeV");
    
    
    latex21518.SetTextSize(0.032);
    latex21518.DrawLatex(0.43, 0.86, Form("Mean of Gaussian: %.4f GeV", fitMean21518));
    latex21518.DrawLatex(0.43, 0.81, Form("Std. Dev. of Gaussian: %.4f GeV", fitSigma21518));
    
    // Save as PNG
    canvas21518->SaveAs("/Users/patsfan753/Desktop/Pi0_Analysis/IndividualRunMassPlots/hPi0Mass_21518_fit.png");
    
    
    
    
    
    
    // Create canvas and draw
    TCanvas *canvas21520 = new TCanvas("canvas21520", "Pi0 Mass Distribution 21520", 900, 600);
    hPi0Mass21520->SetMarkerStyle(20);
    hPi0Mass21520->SetMarkerSize(1.0);
    hPi0Mass21520->SetMarkerColor(kBlack);
    hPi0Mass21520->Draw("PE");

    // Display information with TLatex LABELLING IS FOR INCLUSIVE CUTS (WHAT IS BEING INCLUDED)
    TLatex latex21520;
    latex21520.SetNDC();
    latex21520.SetTextSize(0.037);
    latex21520.DrawLatex(0.13, 0.86, "Cuts (Inclusive):");
    latex21520.DrawLatex(0.13, 0.82, "#Delta R > 0.08");
    latex21520.DrawLatex(0.13, 0.78, "Asymmetry < 0.5");
    latex21520.DrawLatex(0.13, 0.74, "#chi^{2} < 4");
    latex21520.DrawLatex(0.13, 0.70, "53640.9 <= MBD Charge < 109768");
    latex21520.DrawLatex(0.13, 0.66, "2 <= p_{T} < 3 GeV");
    latex21520.DrawLatex(0.13, 0.61, "E > 1.0 GeV");
    
    
    latex21520.SetTextSize(0.032);
    latex21520.DrawLatex(0.43, 0.86, Form("Mean of Gaussian: %.4f GeV", fitMean21520));
    latex21520.DrawLatex(0.43, 0.81, Form("Std. Dev. of Gaussian: %.4f GeV", fitSigma21520));
    
    // Save as PNG
    canvas21520->SaveAs("/Users/patsfan753/Desktop/Pi0_Analysis/IndividualRunMassPlots/hPi0Mass_21520_fit.png");
    
    // Create canvas and draw
    TCanvas *canvas21598 = new TCanvas("canvas21598", "Pi0 Mass Distribution 21598", 900, 600);
    hPi0Mass21598->SetMarkerStyle(20);
    hPi0Mass21598->SetMarkerSize(1.0);
    hPi0Mass21598->SetMarkerColor(kBlack);
    hPi0Mass21598->Draw("PE");

    // Display information with TLatex LABELLING IS FOR INCLUSIVE CUTS (WHAT IS BEING INCLUDED)
    TLatex latex21598;
    latex21598.SetNDC();
    latex21598.SetTextSize(0.037);
    latex21598.DrawLatex(0.13, 0.86, "Cuts (Inclusive):");
    latex21598.DrawLatex(0.13, 0.82, "#Delta R > 0.08");
    latex21598.DrawLatex(0.13, 0.78, "Asymmetry < 0.5");
    latex21598.DrawLatex(0.13, 0.74, "#chi^{2} < 4");
    latex21598.DrawLatex(0.13, 0.70, "53640.9 <= MBD Charge < 109768");
    latex21598.DrawLatex(0.13, 0.66, "2 <= p_{T} < 3 GeV");
    latex21598.DrawLatex(0.13, 0.61, "E > 1.0 GeV");
    
    
    latex21598.SetTextSize(0.032);
    latex21598.DrawLatex(0.43, 0.86, Form("Mean of Gaussian: %.4f GeV", fitMean21598));
    latex21598.DrawLatex(0.43, 0.81, Form("Std. Dev. of Gaussian: %.4f GeV", fitSigma21598));
    
    // Save as PNG
    canvas21598->SaveAs("/Users/patsfan753/Desktop/Pi0_Analysis/IndividualRunMassPlots/hPi0Mass_21598_fit.png");
    
    
    
    
    // Create canvas and draw
    TCanvas *canvas21599 = new TCanvas("canvas21599", "Pi0 Mass Distribution 21599", 900, 600);
    hPi0Mass21599->SetMarkerStyle(20);
    hPi0Mass21599->SetMarkerSize(1.0);
    hPi0Mass21599->SetMarkerColor(kBlack);
    hPi0Mass21599->Draw("PE");

    // Display information with TLatex LABELLING IS FOR INCLUSIVE CUTS (WHAT IS BEING INCLUDED)
    TLatex latex21599;
    latex21599.SetNDC();
    latex21599.SetTextSize(0.037);
    latex21599.DrawLatex(0.13, 0.86, "Cuts (Inclusive):");
    latex21599.DrawLatex(0.13, 0.82, "#Delta R > 0.08");
    latex21599.DrawLatex(0.13, 0.78, "Asymmetry < 0.5");
    latex21599.DrawLatex(0.13, 0.74, "#chi^{2} < 4");
    latex21599.DrawLatex(0.13, 0.70, "53640.9 <= MBD Charge < 109768");
    latex21599.DrawLatex(0.13, 0.66, "2 <= p_{T} < 3 GeV");
    latex21599.DrawLatex(0.13, 0.61, "E > 1.0 GeV");
    
    
    latex21599.SetTextSize(0.032);
    latex21599.DrawLatex(0.43, 0.86, Form("Mean of Gaussian: %.4f GeV", fitMean21599));
    latex21599.DrawLatex(0.43, 0.81, Form("Std. Dev. of Gaussian: %.4f GeV", fitSigma21599));
    
    // Save as PNG
    canvas21599->SaveAs("/Users/patsfan753/Desktop/Pi0_Analysis/IndividualRunMassPlots/hPi0Mass_21599_fit.png");
    
    
    
    
    // Create canvas and draw
    TCanvas *canvas21615 = new TCanvas("canvas21615", "Pi0 Mass Distribution 21615", 900, 600);
    hPi0Mass21615->SetMarkerStyle(20);
    hPi0Mass21615->SetMarkerSize(1.0);
    hPi0Mass21615->SetMarkerColor(kBlack);
    hPi0Mass21615->Draw("PE");

    // Display information with TLatex LABELLING IS FOR INCLUSIVE CUTS (WHAT IS BEING INCLUDED)
    TLatex latex21615;
    latex21615.SetNDC();
    latex21615.SetTextSize(0.037);
    latex21615.DrawLatex(0.13, 0.86, "Cuts (Inclusive):");
    latex21615.DrawLatex(0.13, 0.82, "#Delta R > 0.08");
    latex21615.DrawLatex(0.13, 0.78, "Asymmetry < 0.5");
    latex21615.DrawLatex(0.13, 0.74, "#chi^{2} < 4");
    latex21615.DrawLatex(0.13, 0.70, "53640.9 <= MBD Charge < 109768");
    latex21615.DrawLatex(0.13, 0.66, "2 <= p_{T} < 3 GeV");
    latex21615.DrawLatex(0.13, 0.61, "E > 1.0 GeV");
    
    
    latex21615.SetTextSize(0.032);
    latex21615.DrawLatex(0.43, 0.86, Form("Mean of Gaussian: %.4f GeV", fitMean21615));
    latex21615.DrawLatex(0.43, 0.81, Form("Std. Dev. of Gaussian: %.4f GeV", fitSigma21615));
    
    // Save as PNG
    canvas21615->SaveAs("/Users/patsfan753/Desktop/Pi0_Analysis/IndividualRunMassPlots/hPi0Mass_21615_fit.png");
    
    
    
    
    
    
    
    
    // Create canvas and draw
    TCanvas *canvas21796 = new TCanvas("canvas21796", "Pi0 Mass Distribution 21796", 900, 600);
    hPi0Mass21796->SetMarkerStyle(20);
    hPi0Mass21796->SetMarkerSize(1.0);
    hPi0Mass21796->SetMarkerColor(kBlack);
    hPi0Mass21796->Draw("PE");

    // Display information with TLatex LABELLING IS FOR INCLUSIVE CUTS (WHAT IS BEING INCLUDED)
    TLatex latex21796;
    latex21796.SetNDC();
    latex21796.SetTextSize(0.037);
    latex21796.DrawLatex(0.13, 0.86, "Cuts (Inclusive):");
    latex21796.DrawLatex(0.13, 0.82, "#Delta R > 0.08");
    latex21796.DrawLatex(0.13, 0.78, "Asymmetry < 0.5");
    latex21796.DrawLatex(0.13, 0.74, "#chi^{2} < 4");
    latex21796.DrawLatex(0.13, 0.70, "53640.9 <= MBD Charge < 109768");
    latex21796.DrawLatex(0.13, 0.66, "2 <= p_{T} < 3 GeV");
    latex21796.DrawLatex(0.13, 0.61, "E > 1.0 GeV");
    
    
    latex21796.SetTextSize(0.032);
    latex21796.DrawLatex(0.43, 0.86, Form("Mean of Gaussian: %.4f GeV", fitMean21796));
    latex21796.DrawLatex(0.43, 0.81, Form("Std. Dev. of Gaussian: %.4f GeV", fitSigma21796));
    
    // Save as PNG
    canvas21796->SaveAs("/Users/patsfan753/Desktop/Pi0_Analysis/IndividualRunMassPlots/hPi0Mass_21796_fit.png");
    
    
    
    
    // Create canvas and draw
    TCanvas *canvas21813 = new TCanvas("canvas21813", "Pi0 Mass Distribution 21813", 900, 600);
    hPi0Mass21813->SetMarkerStyle(20);
    hPi0Mass21813->SetMarkerSize(1.0);
    hPi0Mass21813->SetMarkerColor(kBlack);
    hPi0Mass21813->Draw("PE");

    // Display information with TLatex LABELLING IS FOR INCLUSIVE CUTS (WHAT IS BEING INCLUDED)
    TLatex latex21813;
    latex21813.SetNDC();
    latex21813.SetTextSize(0.037);
    latex21813.DrawLatex(0.13, 0.86, "Cuts (Inclusive):");
    latex21813.DrawLatex(0.13, 0.82, "#Delta R > 0.08");
    latex21813.DrawLatex(0.13, 0.78, "Asymmetry < 0.5");
    latex21813.DrawLatex(0.13, 0.74, "#chi^{2} < 4");
    latex21813.DrawLatex(0.13, 0.70, "53640.9 <= MBD Charge < 109768");
    latex21813.DrawLatex(0.13, 0.66, "2 <= p_{T} < 3 GeV");
    latex21813.DrawLatex(0.13, 0.61, "E > 1.0 GeV");
    
    
    latex21813.SetTextSize(0.032);
    latex21813.DrawLatex(0.43, 0.86, Form("Mean of Gaussian: %.4f GeV", fitMean21813));
    latex21813.DrawLatex(0.43, 0.81, Form("Std. Dev. of Gaussian: %.4f GeV", fitSigma21813));
    
    // Save as PNG
    canvas21813->SaveAs("/Users/patsfan753/Desktop/Pi0_Analysis/IndividualRunMassPlots/hPi0Mass_21813_fit.png");
    
    
    
    
    // Create canvas and draw
    TCanvas *canvas21889 = new TCanvas("canvas21889", "Pi0 Mass Distribution 21889", 900, 600);
    hPi0Mass21889->SetMarkerStyle(20);
    hPi0Mass21889->SetMarkerSize(1.0);
    hPi0Mass21889->SetMarkerColor(kBlack);
    hPi0Mass21889->Draw("PE");

    // Display information with TLatex LABELLING IS FOR INCLUSIVE CUTS (WHAT IS BEING INCLUDED)
    TLatex latex21889;
    latex21889.SetNDC();
    latex21889.SetTextSize(0.037);
    latex21889.DrawLatex(0.13, 0.86, "Cuts (Inclusive):");
    latex21889.DrawLatex(0.13, 0.82, "#Delta R > 0.08");
    latex21889.DrawLatex(0.13, 0.78, "Asymmetry < 0.5");
    latex21889.DrawLatex(0.13, 0.74, "#chi^{2} < 4");
    latex21889.DrawLatex(0.13, 0.70, "53640.9 <= MBD Charge < 109768");
    latex21889.DrawLatex(0.13, 0.66, "2 <= p_{T} < 3 GeV");
    latex21889.DrawLatex(0.13, 0.61, "E > 1.0 GeV");
    
    latex21889.SetTextSize(0.032);
    latex21889.DrawLatex(0.43, 0.86, Form("Mean of Gaussian: %.4f GeV", fitMean21889));
    latex21889.DrawLatex(0.43, 0.81, Form("Std. Dev. of Gaussian: %.4f GeV", fitSigma21889));
    
    
    // Save as PNG
    canvas21889->SaveAs("/Users/patsfan753/Desktop/Pi0_Analysis/IndividualRunMassPlots/hPi0Mass_21889_fit.png");
    
    
    
    // Create canvas and draw
    TCanvas *canvas21891 = new TCanvas("canvas21891", "Pi0 Mass Distribution 21891", 900, 600);
    hPi0Mass21891->SetMarkerStyle(20);
    hPi0Mass21891->SetMarkerSize(1.0);
    hPi0Mass21891->SetMarkerColor(kBlack);
    hPi0Mass21891->Draw("PE");

    // Display information with TLatex LABELLING IS FOR INCLUSIVE CUTS (WHAT IS BEING INCLUDED)
    TLatex latex21891;
    latex21891.SetNDC();
    latex21891.SetTextSize(0.037);
    latex21891.DrawLatex(0.13, 0.86, "Cuts (Inclusive):");
    latex21891.DrawLatex(0.13, 0.82, "#Delta R > 0.08");
    latex21891.DrawLatex(0.13, 0.78, "Asymmetry < 0.5");
    latex21891.DrawLatex(0.13, 0.74, "#chi^{2} < 4");
    latex21891.DrawLatex(0.13, 0.70, "53640.9 <= MBD Charge < 109768");
    latex21891.DrawLatex(0.13, 0.66, "2 <= p_{T} < 3 GeV");
    latex21891.DrawLatex(0.13, 0.61, "E > 1.0 GeV");
    
    latex21891.SetTextSize(0.032);
    latex21891.DrawLatex(0.43, 0.86, Form("Mean of Gaussian: %.4f GeV", fitMean21891));
    latex21891.DrawLatex(0.43, 0.81, Form("Std. Dev. of Gaussian: %.4f GeV", fitSigma21891));
    
    
    // Save as PNG
    canvas21891->SaveAs("/Users/patsfan753/Desktop/Pi0_Analysis/IndividualRunMassPlots/hPi0Mass_21891_fit.png");
    
    
    
    
    
    
    
    // Create canvas and draw
    TCanvas *canvas22949 = new TCanvas("canvas22949", "Pi0 Mass Distribution 22949", 900, 600);
    hPi0Mass22949->SetMarkerStyle(20);
    hPi0Mass22949->SetMarkerSize(1.0);
    hPi0Mass22949->SetMarkerColor(kBlack);
    hPi0Mass22949->Draw("PE");

    // Display information with TLatex LABELLING IS FOR INCLUSIVE CUTS (WHAT IS BEING INCLUDED)
    TLatex latex22949;
    latex22949.SetNDC();
    latex22949.SetTextSize(0.037);
    latex22949.DrawLatex(0.13, 0.86, "Cuts (Inclusive):");
    latex22949.DrawLatex(0.13, 0.82, "#Delta R > 0.08");
    latex22949.DrawLatex(0.13, 0.78, "Asymmetry < 0.5");
    latex22949.DrawLatex(0.13, 0.74, "#chi^{2} < 4");
    latex22949.DrawLatex(0.13, 0.70, "53640.9 <= MBD Charge < 109768");
    latex22949.DrawLatex(0.13, 0.66, "2 <= p_{T} < 3 GeV");
    latex22949.DrawLatex(0.13, 0.61, "E > 1.0 GeV");
    
    latex22949.SetTextSize(0.032);
    latex22949.DrawLatex(0.43, 0.86, Form("Mean of Gaussian: %.4f GeV", fitMean22949));
    latex22949.DrawLatex(0.43, 0.81, Form("Std. Dev. of Gaussian: %.4f GeV", fitSigma22949));
    
    
    // Save as PNG
    canvas22949->SaveAs("/Users/patsfan753/Desktop/Pi0_Analysis/IndividualRunMassPlots/hPi0Mass_22949_fit.png");
    
    
    
    // Create canvas and draw
    TCanvas *canvas22950 = new TCanvas("canvas22950", "Pi0 Mass Distribution 22950", 900, 600);
    hPi0Mass22950->SetMarkerStyle(20);
    hPi0Mass22950->SetMarkerSize(1.0);
    hPi0Mass22950->SetMarkerColor(kBlack);
    hPi0Mass22950->Draw("PE");

    // Display information with TLatex LABELLING IS FOR INCLUSIVE CUTS (WHAT IS BEING INCLUDED)
    TLatex latex22950;
    latex22950.SetNDC();
    latex22950.SetTextSize(0.037);
    latex22950.DrawLatex(0.13, 0.86, "Cuts (Inclusive):");
    latex22950.DrawLatex(0.13, 0.82, "#Delta R > 0.08");
    latex22950.DrawLatex(0.13, 0.78, "Asymmetry < 0.5");
    latex22950.DrawLatex(0.13, 0.74, "#chi^{2} < 4");
    latex22950.DrawLatex(0.13, 0.70, "53640.9 <= MBD Charge < 109768");
    latex22950.DrawLatex(0.13, 0.66, "2 <= p_{T} < 3 GeV");
    latex22950.DrawLatex(0.13, 0.61, "E > 1.0 GeV");
    
    latex22950.SetTextSize(0.032);
    latex22950.DrawLatex(0.43, 0.86, Form("Mean of Gaussian: %.4f GeV", fitMean22950));
    latex22950.DrawLatex(0.43, 0.81, Form("Std. Dev. of Gaussian: %.4f GeV", fitSigma22950));
    
    
    // Save as PNG
    canvas22950->SaveAs("/Users/patsfan753/Desktop/Pi0_Analysis/IndividualRunMassPlots/hPi0Mass_22950_fit.png");
    
    
    
    
    // Create canvas and draw
    TCanvas *canvas22951 = new TCanvas("canvas22951", "Pi0 Mass Distribution 22951", 900, 600);
    hPi0Mass22951->SetMarkerStyle(20);
    hPi0Mass22951->SetMarkerSize(1.0);
    hPi0Mass22951->SetMarkerColor(kBlack);
    hPi0Mass22951->Draw("PE");

    // Display information with TLatex LABELLING IS FOR INCLUSIVE CUTS (WHAT IS BEING INCLUDED)
    TLatex latex22951;
    latex22951.SetNDC();
    latex22951.SetTextSize(0.037);
    latex22951.DrawLatex(0.13, 0.86, "Cuts (Inclusive):");
    latex22951.DrawLatex(0.13, 0.82, "#Delta R > 0.08");
    latex22951.DrawLatex(0.13, 0.78, "Asymmetry < 0.5");
    latex22951.DrawLatex(0.13, 0.74, "#chi^{2} < 4");
    latex22951.DrawLatex(0.13, 0.70, "53640.9 <= MBD Charge < 109768");
    latex22951.DrawLatex(0.13, 0.66, "2 <= p_{T} < 3 GeV");
    latex22951.DrawLatex(0.13, 0.61, "E > 1.0 GeV");
    
    latex22951.SetTextSize(0.032);
    latex22951.DrawLatex(0.43, 0.86, Form("Mean of Gaussian: %.4f GeV", fitMean22951));
    latex22951.DrawLatex(0.43, 0.81, Form("Std. Dev. of Gaussian: %.4f GeV", fitSigma22951));
    
    
    // Save as PNG
    canvas22951->SaveAs("/Users/patsfan753/Desktop/Pi0_Analysis/IndividualRunMassPlots/hPi0Mass_22951_fit.png");
    
    
    
    // Create canvas and draw
    TCanvas *canvas22979 = new TCanvas("canvas22979", "Pi0 Mass Distribution 22979", 900, 600);
    hPi0Mass22979->SetMarkerStyle(20);
    hPi0Mass22979->SetMarkerSize(1.0);
    hPi0Mass22979->SetMarkerColor(kBlack);
    hPi0Mass22979->Draw("PE");

    // Display information with TLatex LABELLING IS FOR INCLUSIVE CUTS (WHAT IS BEING INCLUDED)
    TLatex latex22979;
    latex22979.SetNDC();
    latex22979.SetTextSize(0.037);
    latex22979.DrawLatex(0.13, 0.86, "Cuts (Inclusive):");
    latex22979.DrawLatex(0.13, 0.82, "#Delta R > 0.08");
    latex22979.DrawLatex(0.13, 0.78, "Asymmetry < 0.5");
    latex22979.DrawLatex(0.13, 0.74, "#chi^{2} < 4");
    latex22979.DrawLatex(0.13, 0.70, "53640.9 <= MBD Charge < 109768");
    latex22979.DrawLatex(0.13, 0.66, "2 <= p_{T} < 3 GeV");
    latex22979.DrawLatex(0.13, 0.61, "E > 1.0 GeV");
    
    latex22979.SetTextSize(0.032);
    latex22979.DrawLatex(0.43, 0.86, Form("Mean of Gaussian: %.4f GeV", fitMean22979));
    latex22979.DrawLatex(0.43, 0.81, Form("Std. Dev. of Gaussian: %.4f GeV", fitSigma22979));
    
    
    // Save as PNG
    canvas22979->SaveAs("/Users/patsfan753/Desktop/Pi0_Analysis/IndividualRunMassPlots/hPi0Mass_22979_fit.png");
    
    
    
    // Create canvas and draw
    TCanvas *canvas22982 = new TCanvas("canvas22982", "Pi0 Mass Distribution 22982", 900, 600);
    hPi0Mass22982->SetMarkerStyle(20);
    hPi0Mass22982->SetMarkerSize(1.0);
    hPi0Mass22982->SetMarkerColor(kBlack);
    hPi0Mass22982->Draw("PE");

    // Display information with TLatex LABELLING IS FOR INCLUSIVE CUTS (WHAT IS BEING INCLUDED)
    TLatex latex22982;
    latex22982.SetNDC();
    latex22982.SetTextSize(0.037);
    latex22982.DrawLatex(0.13, 0.86, "Cuts (Inclusive):");
    latex22982.DrawLatex(0.13, 0.82, "#Delta R > 0.08");
    latex22982.DrawLatex(0.13, 0.78, "Asymmetry < 0.5");
    latex22982.DrawLatex(0.13, 0.74, "#chi^{2} < 4");
    latex22982.DrawLatex(0.13, 0.70, "53640.9 <= MBD Charge < 109768");
    latex22982.DrawLatex(0.13, 0.66, "2 <= p_{T} < 3 GeV");
    latex22982.DrawLatex(0.13, 0.61, "E > 1.0 GeV");
    
    latex22982.SetTextSize(0.032);
    latex22982.DrawLatex(0.43, 0.86, Form("Mean of Gaussian: %.4f GeV", fitMean22982));
    latex22982.DrawLatex(0.43, 0.81, Form("Std. Dev. of Gaussian: %.4f GeV", fitSigma22982));
    
    
    // Save as PNG
    canvas22982->SaveAs("/Users/patsfan753/Desktop/Pi0_Analysis/IndividualRunMassPlots/hPi0Mass_22982_fit.png");
    
    
    std::vector<int> runNumbers = {
        21518, 21520, 21598, 21599, 21615, 21796, 21813, 21889, 21891, 22949, 22950, 22951, 22979, 22982
    };
    std::vector<double> fitMeans = {
        fitMean21518, fitMean21520, fitMean21598, fitMean21599, fitMean21615, fitMean21796, fitMean21813, fitMean21889, fitMean21891, fitMean22949, fitMean22950, fitMean22951, fitMean22979, fitMean22982
    };
    

    std::vector<double> fitSigmas = {
        fitSigma21518, fitSigma21520, fitSigma21598, fitSigma21599, fitSigma21615, fitSigma21796, fitSigma21813, fitSigma21889, fitSigma21891, fitSigma22949, fitSigma22950, fitSigma22951, fitSigma22979, fitSigma22982
    };
    
    std::cout << "\033[1m\033[31m" << std::left << std::setw(12) << "Run Number" << "Mean Value" << "\033[0m" << std::endl;
    std::cout << std::string(24, '-') << std::endl; // creates a separator line

    for (size_t i = 0; i < runNumbers.size(); ++i) {
        std::cout << std::left << std::setw(12) << runNumbers[i]
                  << std::setprecision(5) << fitMeans[i] << std::endl;
    }

    // Create a histogram for setting the run numbers as labels on the x-axis
    TH1F *h = new TH1F("h", "Fit Mean vs Run Number", runNumbers.size(), 0, runNumbers.size());
    h->SetCanExtend(TH1::kAllAxes);
    for (size_t i = 0; i < runNumbers.size(); ++i) {
        h->GetXaxis()->SetBinLabel(i+1, std::to_string(runNumbers[i]).c_str());
        h->SetBinContent(i+1, fitMeans[i]);
        h->SetBinError(i+1, fitSigmas[i]);
    }
    gStyle->SetErrorX(0);

    // Set the marker style for visualization
    h->SetStats(0);
    h->SetMarkerStyle(20);
    h->SetMarkerSize(1.0);
    h->SetMarkerColor(kBlack);
    
    // Set the axis titles
    h->GetXaxis()->SetTitle("Run Number");
    h->GetYaxis()->SetTitle("Mean of Gaussian Signal [GeV]");

    // Create the canvas
    TCanvas *c = new TCanvas("c", "Fit Mean vs Run Number", 900, 600);
    h->Draw("P");

    // Set y-axis range if needed
    h->SetMinimum(0);
    h->SetMaximum(0.4);

    // Create a legend at the top right
    TLegend *leg = new TLegend(0.12, 0.78, 0.42, 0.88); // Adjust coordinates as needed
    leg->AddEntry(h, "Error Bars = Standard Deviation of Gaussian [GeV]", "pe");
    leg->SetTextSize(0.035); // Increase text size; adjust as needed
    leg->SetBorderSize(0);  // Remove border
    // leg->SetTextFont(42); // Optional: Set text font; uncomment if needed
    leg->Draw();
    
    // Save the canvas
    c->SaveAs("/Users/patsfan753/Desktop/Pi0_Analysis/IndividualRunMassPlots/FitMeanVsRun.png");

}

