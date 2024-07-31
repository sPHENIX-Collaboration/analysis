void display(const string &input) {
    TFile file (input.c_str());

    auto h2Default      = (TH2F*)file.Get("h2TowEtaPhiWeightedDefault");
    auto h2Syst         = (TH2F*)file.Get("h2TowEtaPhiWeighted");
    auto h2RelativeDiff = (TH2F*)file.Get("h2TowEtaPhiWeightedRelativeDiff");
    auto hTotalCaloE    = (TH1F*)file.Get("hTotalCaloE");

    UInt_t bins_eta = 96;
    UInt_t bins_phi = 256;
    string output   = "plots.pdf";

    UInt_t nEvents = hTotalCaloE->GetEntries();
    h2RelativeDiff->Scale(1./nEvents);
    h2Default->Scale(1./nEvents);
    h2Syst->Scale(1./nEvents);

    Float_t min_val[3] = {9999,9999,9999};

    for(UInt_t i = 1; i <= bins_phi; ++i) {
        for(UInt_t j = 1; j <= bins_eta; ++j) {
            Float_t val = h2RelativeDiff->GetBinContent(i,j);
            if(val) min_val[0] = min(min_val[0], val);

            val = h2Default->GetBinContent(i,j);
            if(val) min_val[1] = min(min_val[1], val);

            val = h2Syst->GetBinContent(i,j);
            if(val) min_val[2] = min(min_val[2], val);
        }
    }

    cout << "Events: " << nEvents << endl << endl
         << "min RelativeDiff: " << min_val[0] << endl
         << "min Default: "      << min_val[1] << endl
         << "min Syst: "         << min_val[2] << endl;

    TCanvas* c1 = new TCanvas();
    c1->SetTickx();
    c1->SetTicky();

    c1->SetCanvasSize(1500, 1000);
    c1->SetLeftMargin(.09);
    c1->SetRightMargin(.13);

    c1->Print((output + "[").c_str(), "pdf portrait");

    c1->cd();

    h2Default->SetStats(0);
    h2Default->SetTitle("Run 23566, Average Tower Energy [GeV]");
    h2Default->SetMinimum(0.014);
    h2Default->SetMaximum(0.034);
    h2Default->Draw("COLZ1");

    c1->Print((string(h2Default->GetName()) + ".png").c_str());
    c1->Print((output).c_str(), "pdf portrait");

    h2Syst->SetStats(0);
    h2Syst->SetTitle("Run 23566, Average Tower Energy SYST2 (Absolute Scale) [GeV]");
    h2Syst->SetMinimum(0.014);
    h2Syst->SetMaximum(0.034);
    h2Syst->Draw("COLZ1");

    c1->Print((string(h2Syst->GetName()) + ".png").c_str());
    c1->Print((output).c_str(), "pdf portrait");

    h2RelativeDiff->SetStats(0);

    h2RelativeDiff->SetTitle("Run 23566, Average of |(E_{nom}-E_{sys})/E_{nom}|");
    h2RelativeDiff->SetMinimum(7e-3);
    h2RelativeDiff->SetMaximum(1.1e-2);
    h2RelativeDiff->Draw("COLZ1");

    c1->Print((string(h2RelativeDiff->GetName()) + ".png").c_str());
    c1->Print((output).c_str(), "pdf portrait");


    c1->SetLeftMargin(.12);
    c1->SetRightMargin(.03);

    auto h2RelativeDiff_py = h2RelativeDiff->ProjectionY();

    h2RelativeDiff_py->SetStats(0);
    h2RelativeDiff_py->Scale(1./bins_phi);
    h2RelativeDiff_py->GetYaxis()->SetTitle("Average of |(E_{nom}-E_{sys})/E_{nom}|");
    h2RelativeDiff_py->Draw("HIST E1");

    c1->Print((string(h2RelativeDiff->GetName()) + "-profile.png").c_str());
    c1->Print((output).c_str(), "pdf portrait");

    c1->Print((output + "]").c_str(), "pdf portrait");

    file.Close();
}
