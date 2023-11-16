void PlotTiming() {
    TFile *file = new TFile("/Users/patsfan753/Desktop/Pi0_Analysis/variedCutsRootFiles/HistOutput.root", "READ");
    TH1F *hLeading_GoodPeak = (TH1F*)file->Get("hLeading_GoodPeak");
    TH1F *hSubLeading_GoodPeak = (TH1F*)file->Get("hSubLeading_GoodPeak");
    TH1F *hLeading_BadPeak = (TH1F*)file->Get("hLeading_BadPeak");
    TH1F *hSubLeading_BadPeak = (TH1F*)file->Get("hSubLeading_BadPeak");
    
    // Set x and y axis labels
    hLeading_GoodPeak->GetXaxis()->SetTitle("Timing Peak Location");
    hLeading_GoodPeak->GetYaxis()->SetTitle("counts");

    hSubLeading_GoodPeak->GetXaxis()->SetTitle("Timing Peak Location");
    hSubLeading_GoodPeak->GetYaxis()->SetTitle("counts");

    hLeading_BadPeak->GetXaxis()->SetTitle("Timing Peak Location");
    hLeading_BadPeak->GetYaxis()->SetTitle("counts");

    hSubLeading_BadPeak->GetXaxis()->SetTitle("Timing Peak Location");
    hSubLeading_BadPeak->GetYaxis()->SetTitle("counts");
    
    
    
    // Draw histograms in separate canvases and save as PNG
    TCanvas* leadingGoodPeak = new TCanvas("leadingGoodPeak", "hLeading_GoodPeak", 800, 600);
    leadingGoodPeak->SetLogy();  // Set logarithmic scale on y-axis
    // Set stats box to false
    hLeading_GoodPeak->SetStats(1);
    
    hLeading_GoodPeak->Draw();
    
    // Display information with TLatex LABELLING IS FOR INCLUSIVE CUTS (WHAT IS BEING INCLUDED)
//    TLatex latex;
//    latex.SetNDC();
//    latex.SetTextSize(0.04);
//    latex.DrawLatex(0.6, 0.66, "Cuts (Inclusive):");
//    latex.DrawLatex(0.6, 0.62, "#Delta R #geq 0.08");
//    latex.DrawLatex(0.6, 0.58, "Asymmetry < 0.5");
//    latex.DrawLatex(0.6, 0.54, "#chi^{2} < 4");
//    latex.DrawLatex(0.6, 0.5, "0 #leq Diphoton Mass < 1");
//    latex.DrawLatex(0.6, 0.46, "Cluster E #geq 1.0 GeV");
    
    leadingGoodPeak->SaveAs("/Users/patsfan753/Desktop/Pi0_Analysis/timing_plots/hLeading_GoodPeak.png");

    TCanvas* subLeadingGood = new TCanvas("subLeadingGood", "hSubLeading_GoodPeak", 900, 600);
    subLeadingGood->SetLogy();  // Set logarithmic scale on y-axis
    hSubLeading_GoodPeak->SetStats(1);
    hSubLeading_GoodPeak->Draw();
    // Display information with TLatex LABELLING IS FOR INCLUSIVE CUTS (WHAT IS BEING INCLUDED)
//    TLatex latex1;
//    latex1.SetNDC();
//    latex1.SetTextSize(0.04);
//    latex1.DrawLatex(0.6, 0.66, "Cuts (Inclusive):");
//    latex1.DrawLatex(0.6, 0.62, "#Delta R #geq 0.08");
//    latex1.DrawLatex(0.6, 0.58, "Asymmetry < 0.5");
//    latex1.DrawLatex(0.6, 0.54, "#chi^{2} < 4");
//    latex1.DrawLatex(0.6, 0.5, "0 #leq Diphoton Mass < 1");
//    latex1.DrawLatex(0.6, 0.46, "Cluster E #geq 1.0 GeV");
    subLeadingGood->SaveAs("/Users/patsfan753/Desktop/Pi0_Analysis/timing_plots/hSubLeading_GoodPeak.png");

    TCanvas* leadingBadPeak = new TCanvas("leadingBadPeak", "hLeading_BadPeak", 800, 600);
    leadingBadPeak->SetLogy();  // Set logarithmic scale on y-axis
    hLeading_BadPeak->SetStats(1);
    hLeading_BadPeak->Draw();
    
    // Display information with TLatex LABELLING IS FOR INCLUSIVE CUTS (WHAT IS BEING INCLUDED)
//    TLatex latex2;
//    latex2.SetNDC();
//    latex2.SetTextSize(0.04);
//    latex2.DrawLatex(0.6, 0.66, "Cuts (Inclusive):");
//    latex2.DrawLatex(0.6, 0.62, "#Delta R #geq 0.08");
//    latex2.DrawLatex(0.6, 0.58, "Asymmetry < 0.5");
//    latex2.DrawLatex(0.6, 0.54, "#chi^{2} < 4");
//    latex2.DrawLatex(0.6, 0.5, "0 #leq Diphoton Mass < 1");
//    latex2.DrawLatex(0.6, 0.46, "Cluster E #geq 1.0 GeV");
    leadingBadPeak->SaveAs("/Users/patsfan753/Desktop/Pi0_Analysis/timing_plots/hLeading_BadPeak.png");

    TCanvas* subLeadingBad = new TCanvas("subLeadingBad", "hSubLeading_BadPeak", 800, 600);
    subLeadingBad->SetLogy();  // Set logarithmic scale on y-axis
    hSubLeading_BadPeak->SetStats(1);
    hSubLeading_BadPeak->Draw();
    // Display information with TLatex LABELLING IS FOR INCLUSIVE CUTS (WHAT IS BEING INCLUDED)
//    TLatex latex3;
//    latex3.SetNDC();
//    latex3.SetTextSize(0.04);
//    latex3.DrawLatex(0.6, 0.66, "Cuts (Inclusive):");
//    latex3.DrawLatex(0.6, 0.62, "#Delta R #geq 0.08");
//    latex3.DrawLatex(0.6, 0.58, "Asymmetry < 0.5");
//    latex3.DrawLatex(0.6, 0.54, "#chi^{2} < 4");
//    latex3.DrawLatex(0.6, 0.5, "0 #leq Diphoton Mass < 1");
//    latex3.DrawLatex(0.6, 0.46, "Cluster E #geq 1.0 GeV");
    subLeadingBad->SaveAs("/Users/patsfan753/Desktop/Pi0_Analysis/timing_plots/hSubLeading_BadPeak.png");
}



