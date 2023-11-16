void PlotPi0Pt() {
    TFile *file = new TFile("/Users/patsfan753/Desktop/Pi0_Analysis/HistOutput.root", "READ");
    TH1F *hPi0Pt_1 = (TH1F*)file->Get("hPi0Pt_1");
    TH1F *hPi0Pt_2 = (TH1F*)file->Get("hPi0Pt_2");
    TH1F *hPi0Pt_3 = (TH1F*)file->Get("hPi0Pt_3");
    TH1F *hPi0Pt_4 = (TH1F*)file->Get("hPi0Pt_4");

    // Set x and y axis labels
    hPi0Pt_1->GetXaxis()->SetTitle("Diphoton p_{T}");
    hPi0Pt_1->GetYaxis()->SetTitle("counts");

    hPi0Pt_2->GetXaxis()->SetTitle("Diphoton p_{T}");
    hPi0Pt_2->GetYaxis()->SetTitle("counts");

    hPi0Pt_3->GetXaxis()->SetTitle("Diphoton p_{T}");
    hPi0Pt_3->GetYaxis()->SetTitle("counts");

    hPi0Pt_4->GetXaxis()->SetTitle("Diphoton p_{T}");
    hPi0Pt_4->GetYaxis()->SetTitle("counts");


    // Draw histograms in separate canvases and save as PNG
    TCanvas* c1 = new TCanvas("c1", "Pi0 pT (0 <= MBD < 21395.5)", 800, 600);
    c1->SetLogy();  // Set logarithmic scale on y-axis
    // Set stats box to false
    hPi0Pt_1->SetStats(1);
    
    hPi0Pt_1->Draw();
    
    // Display information with TLatex LABELLING IS FOR INCLUSIVE CUTS (WHAT IS BEING INCLUDED)
    TLatex latex;
    latex.SetNDC();
    latex.SetTextSize(0.04);
    latex.DrawLatex(0.6, 0.66, "Cuts (Inclusive):");
    latex.DrawLatex(0.6, 0.62, "#Delta R #geq 0.08");
    latex.DrawLatex(0.6, 0.58, "Asymmetry < 0.5");
    latex.DrawLatex(0.6, 0.54, "#chi^{2} < 4");
    latex.DrawLatex(0.6, 0.5, "0 #leq Diphoton Mass < 1");
    latex.DrawLatex(0.6, 0.46, "Cluster E #geq 1.0 GeV");
    
    c1->SaveAs("/Users/patsfan753/Desktop/Pi0_Analysis/pT_plots/Pi0PtDist_0-21395.5.png");

    TCanvas* c2 = new TCanvas("c2", "Pi0 pT (21395.5 <= MBD < 53640.9)", 900, 600);
    c2->SetLogy();  // Set logarithmic scale on y-axis
    hPi0Pt_2->SetStats(1);
    hPi0Pt_2->Draw();
    // Display information with TLatex LABELLING IS FOR INCLUSIVE CUTS (WHAT IS BEING INCLUDED)
    TLatex latex1;
    latex1.SetNDC();
    latex1.SetTextSize(0.04);
    latex1.DrawLatex(0.6, 0.66, "Cuts (Inclusive):");
    latex1.DrawLatex(0.6, 0.62, "#Delta R #geq 0.08");
    latex1.DrawLatex(0.6, 0.58, "Asymmetry < 0.5");
    latex1.DrawLatex(0.6, 0.54, "#chi^{2} < 4");
    latex1.DrawLatex(0.6, 0.5, "0 #leq Diphoton Mass < 1");
    latex1.DrawLatex(0.6, 0.46, "Cluster E #geq 1.0 GeV");
    c2->SaveAs("/Users/patsfan753/Desktop/Pi0_Analysis/pT_plots/Pi0PtDist_21395.5-53640.9.png");

    TCanvas* c3 = new TCanvas("c3", "Pi0 pT (53640.9 <= MBD < 109768)", 800, 600);
    c3->SetLogy();  // Set logarithmic scale on y-axis
    hPi0Pt_3->SetStats(1);
    hPi0Pt_3->Draw();
    
    // Display information with TLatex LABELLING IS FOR INCLUSIVE CUTS (WHAT IS BEING INCLUDED)
    TLatex latex2;
    latex2.SetNDC();
    latex2.SetTextSize(0.04);
    latex2.DrawLatex(0.6, 0.66, "Cuts (Inclusive):");
    latex2.DrawLatex(0.6, 0.62, "#Delta R #geq 0.08");
    latex2.DrawLatex(0.6, 0.58, "Asymmetry < 0.5");
    latex2.DrawLatex(0.6, 0.54, "#chi^{2} < 4");
    latex2.DrawLatex(0.6, 0.5, "0 #leq Diphoton Mass < 1");
    latex2.DrawLatex(0.6, 0.46, "Cluster E #geq 1.0 GeV");
    c3->SaveAs("/Users/patsfan753/Desktop/Pi0_Analysis/pT_plots/Pi0PtDist_53640.9-109768.png");

    TCanvas* c4 = new TCanvas("c4", "Pi0 pT (109768 <= MBD < 250000)", 800, 600);
    c4->SetLogy();  // Set logarithmic scale on y-axis
    hPi0Pt_4->SetStats(1);
    hPi0Pt_4->Draw();
    // Display information with TLatex LABELLING IS FOR INCLUSIVE CUTS (WHAT IS BEING INCLUDED)
    TLatex latex3;
    latex3.SetNDC();
    latex3.SetTextSize(0.04);
    latex3.DrawLatex(0.6, 0.66, "Cuts (Inclusive):");
    latex3.DrawLatex(0.6, 0.62, "#Delta R #geq 0.08");
    latex3.DrawLatex(0.6, 0.58, "Asymmetry < 0.5");
    latex3.DrawLatex(0.6, 0.54, "#chi^{2} < 4");
    latex3.DrawLatex(0.6, 0.5, "0 #leq Diphoton Mass < 1");
    latex3.DrawLatex(0.6, 0.46, "Cluster E #geq 1.0 GeV");
    c4->SaveAs("/Users/patsfan753/Desktop/Pi0_Analysis/pT_plots/Pi0PtDist_109768-250000.png");
}
