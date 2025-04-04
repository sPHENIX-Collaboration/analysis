void Draw1D(TH1 *h1, TH1 *h2, const char *name, const char *title, const char *xtitle, const char *ytitle)
{
    // normalize the histograms
    h1->Scale(1.0 / h1->Integral(-1, -1));
    h2->Scale(1.0 / h2->Integral(-1, -1));
    TCanvas *c = new TCanvas(name, title, 800, 700);
    h1->SetMarkerStyle(20);
    h1->SetMarkerSize(0.8);
    h1->SetLineColor(kBlack);
    h2->SetLineColor(kRed);
    h1->SetLineWidth(2);
    h2->SetLineWidth(2);
    h2->SetMarkerSize(0);
    h2->Draw("histe");
    h1->Draw("PE same");
    h2->GetXaxis()->SetTitle(xtitle);
    h2->GetYaxis()->SetTitle(ytitle);
    h2->GetYaxis()->SetTitleOffset(1.5);
    TLegend *leg = new TLegend(0.7, 0.7, 0.9, 0.9);
    leg->AddEntry(h1, "Truth", "lep");
    leg->AddEntry(h2, "Sampled", "le");
    leg->Draw();
    c->Draw();
    c->SaveAs(Form("%s.png", name));
    c->SaveAs(Form("%s.pdf", name));
};

void KshortMomentum()
{
    ROOT::EnableImplicitMT();
    ROOT::RDataFrame df("HFTrackEfficiency", "/sphenix/user/cdean/public/forHaoRen/outputHFEffFile_Kshort.root");

    // a 3D histogram for true_mother_px vs true_mother_py vs true_mother_pz
    auto h_motherpx_motherpy_motherpz = df.Histo3D({"h_motherpx_motherpy_motherpz", ";p_{x} [GeV];p_{y} [GeV];p_{z} [GeV]", 200, -10, 10, 200, -10, 10, 200, -10, 10}, "true_mother_px", "true_mother_py", "true_mother_pz");
    auto h_motherP_motherPt = df.Histo2D({"h_motherP_motherPt", ";p [GeV];p_{T} [GeV]", 100, 0, 10, 100, 0, 10}, "true_mother_p", "true_mother_pT");
    auto h_motherP = df.Histo1D({"h_motherP", ";K_{S}^{0} p [GeV]", 100, 0, 10}, "true_mother_p");
    auto h_motherPt = df.Histo1D({"h_motherPt", ";K_{S}^{0} p_{T} [GeV]", 100, 0, 10}, "true_mother_pT");
    auto h_motherPz = df.Histo1D({"h_motherPz", ";K_{S}^{0} p_{z} [GeV]", 100, 0, 10}, "true_mother_pz");
    auto h_motherEta = df.Histo1D({"h_motherEta", ";K_{S}^{0} #eta", 40, -2, 2}, "true_mother_eta");
    auto h_motherPxPy = df.Histo2D({"h_motherPxPy", ";K_{S}^{0} p_{x} [GeV];K_{S}^{0} p_{y} [GeV]", 100, -10, 10, 100, -10, 10}, "true_mother_px", "true_mother_py");

    // Randomly generate by sampling from the 2D histogram
    TH1F *h_sampled_P = new TH1F("h_sampled_P", ";K_{S}^{0} p [GeV]", 100, 0, 10);
    TH1F *h_sampled_pT = new TH1F("h_sampled_pT", ";K_{S}^{0} p_{T} [GeV]", 100, 0, 10);
    TH1F *h_sampled_pz = new TH1F("h_sampled_pz", ";K_{S}^{0} p_{z} [GeV]", 100, 0, 10);
    TH1F *h_sampled_Eta = new TH1F("h_sampled_Eta", ";K_{S}^{0} #eta", 40, -2, 2);
    TH1F *h_sampled_y = new TH1F("h_sampled_y", ";K_{S}^{0} Rapidity y", 40, -2, 2);
    TH2F *h_sampled_p_pt = new TH2F("h_sampled_p_pt", ";K_{S}^{0} p [GeV];K_{S}^{0} p_{T} [GeV]", 100, 0, 10, 100, 0, 10);
    TH2F *h_sampled_px_py = new TH2F("h_sampled_px_py", ";K_{S}^{0} p_{x} [GeV];K_{S}^{0} p_{y} [GeV]", 100, -10, 10, 100, -10, 10);
    int Nrandom = 1E5;
    double M_kshort = 0.497611; // GeV
    for (int i = 0; i < Nrandom; i++)
    {
        // GetRandom3 from the 3D histogram
        double px, py, pz, p, pT, phi, eta, y, E;
        h_motherpx_motherpy_motherpz->GetRandom3(px, py, pz);
        // Randomly generate phi between -pi to pi
        phi = gRandom->Uniform(-TMath::Pi(), TMath::Pi());
        p = std::sqrt(px * px + py * py + pz * pz);
        pT = std::sqrt(px * px + py * py);
        eta = std::atanh(pz / p);
        // energy 
        E = std::sqrt(p*p + M_kshort*M_kshort);
        // rapidity
        y = 0.5 * std::log((E + pz) / (E - pz));
        // cout << "p: " << p << ", pT: " << pT << ", pz: " << pz << ", eta: " << eta << endl;
        TLorentzVector lvec_rand;
        lvec_rand.SetPtEtaPhiM(pT, eta, phi, M_kshort);

        h_sampled_P->Fill(p);
        h_sampled_pT->Fill(pT);
        h_sampled_pz->Fill(pz);
        h_sampled_Eta->Fill(eta);
        h_sampled_y->Fill(y);
        h_sampled_px_py->Fill(lvec_rand.Px(), lvec_rand.Py());
        h_sampled_p_pt->Fill(p, pT);
    }

    // Draw
    TCanvas *c = new TCanvas("c", "c", 800, 700);
    c->SetLogz();
    c->cd();
    gPad->SetRightMargin(0.13);
    h_motherP_motherPt->Draw("colz");
    c->Draw();
    c->SaveAs("Kshort_motherP_motherPt.png");
    c->SaveAs("Kshort_motherP_motherPt.pdf");

    c->Clear();
    c->cd();
    h_motherPxPy->Draw("colz");
    c->Draw();
    c->SaveAs("Kshort_motherPxPy.png");
    c->SaveAs("Kshort_motherPxPy.pdf");

    // 2D histograms for the sampled values
    c->Clear();
    c->cd();
    h_sampled_p_pt->Draw("colz");
    c->Draw();
    c->SaveAs("Kshort_TH3sampled_p_pt.png");
    c->SaveAs("Kshort_TH3sampled_p_pt.pdf");

    c->Clear();
    c->cd();
    h_sampled_px_py->Draw("colz");
    c->Draw();
    c->SaveAs("Kshort_TH3sampled_pxpy.png");
    c->SaveAs("Kshort_TH3sampled_pxpy.pdf");

    Draw1D(h_motherP.GetPtr(), h_sampled_P, "Kshort_motherP_TH3sampled", "Kshort Mother p", "p [GeV]", "Counts");
    Draw1D(h_motherPt.GetPtr(), h_sampled_pT, "Kshort_motherPt_TH3sampled", "Kshort Mother pT", "p_{T} [GeV]", "Counts");
    Draw1D(h_motherPz.GetPtr(), h_sampled_pz, "Kshort_motherPz_TH3sampled", "Kshort Mother pz", "p_{z} [GeV]", "Counts");
    Draw1D(h_motherEta.GetPtr(), h_sampled_Eta, "Kshort_motherEta_TH3sampled", "Kshort Mother eta", "#eta", "Counts");
}