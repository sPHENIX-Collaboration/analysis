#include "KshortMomentum.C"

// construct a Exponentially modified Gaussian distribution function
double EMG(double *x, double *par)
{
    double N = par[0];      // Normalization
    double mu = par[1];     // Mean of the Gaussian
    double sigma = par[2];  // Width of the Gaussian
    double lambda = par[3]; // Decay constant of the exponential

    double t = x[0];
    double z = (mu + lambda * sigma * sigma - t) / (sqrt(2) * sigma);

    double prefactor = lambda / 2.0;
    double exp_part = exp((lambda / 2.0) * (2.0 * mu + lambda * sigma * sigma - 2.0 * t));
    double erfc_part = TMath::Erfc(z);

    return N * prefactor * exp_part * erfc_part;
}

// Double Gaussian function: simplfied model, fraction is fixed to 0.5, gaussian widths are the same for both Gaussians
double DBGaussian(double *x, double *par)
{
    double N = par[0];     // Normalization
    double mu1 = par[1];   // Mean of the first Gaussian
    double mu2 = par[2];   // Mean of the second Gaussian
    double sigma = par[3]; // Width of the Gaussian

    return N * (TMath::Gaus(x[0], mu1, sigma) + TMath::Gaus(x[0], mu2, sigma));
}

void Kshort_sample()
{
    ROOT::EnableImplicitMT();
    ROOT::RDataFrame df("HFTrackEfficiency", "/sphenix/user/cdean/public/forHaoRen/outputHFEffFile_Kshort.root");

    auto h_motherP = df.Histo1D({"h_motherP", ";K_{S}^{0} p [GeV]", 100, 0, 10}, "true_mother_p");
    auto h_motherPt = df.Histo1D({"h_motherPt", ";K_{S}^{0} p_{T} [GeV]", 100, 0, 10}, "true_mother_pT");
    auto h_motherPz = df.Histo1D({"h_motherPz", ";K_{S}^{0} p_{z} [GeV]", 50, 0, 5}, "true_mother_pz");
    auto h_motherEta = df.Histo1D({"h_motherEta", ";K_{S}^{0} #eta", 40, -2, 2}, "true_mother_eta");
    auto h_mother_Pt_Eta = df.Histo2D({"h_mother_Pt_Eta", ";Truth K_{S}^{0} p_{T} [GeV];Truth K_{S}^{0} #eta", 100, 0, 4, 44, -1.1, 1.1}, "true_mother_pT", "true_mother_eta");

    // TF1 for the Exponentially modified Gaussian for pt and Double Gaussian for eta
    TF1 *fpt = new TF1("fpt", EMG, -10, 10, 4);
    fpt->SetParameters(h_motherPt->GetMaximum(), 0.5, 1, 1);
    fpt->SetParName(0, "N");
    fpt->SetParName(1, "#mu");
    fpt->SetParName(2, "#sigma");
    fpt->SetParName(3, "#lambda");
    fpt->SetParLimits(0, 0, h_motherPt->GetMaximum() * 2);
    fpt->SetParLimits(1, 0, 2);
    fpt->SetParLimits(2, -100, 100);
    fpt->SetParLimits(3, -100, 100);
    fpt->SetLineColor(kRed);
    fpt->SetLineStyle(2);
    // Fit the histogram
    h_motherPt->Scale(1.0 / h_motherPt->Integral(-1, -1));
    h_motherPt->Fit(fpt, "R");

    TF1 *feta = new TF1("feta", DBGaussian, -1, 1, 4);
    feta->SetParameters(1, -0.5, 0.5, 0.5);
    feta->SetParName(0, "N");
    feta->SetParName(1, "#mu_{1}");
    feta->SetParName(2, "#mu_{2}");
    feta->SetParName(3, "#sigma");
    feta->SetParLimits(0, 0, 1);
    feta->SetParLimits(1, -1, 0);
    feta->SetParLimits(2, 0, 1);
    feta->SetParLimits(3, 0, 10);
    // Fit the histogram
    h_motherEta->Scale(1.0 / h_motherEta->Integral(-1, -1));
    h_motherEta->Fit(feta, "R");

    // Fit and draw the histograms
    TCanvas *c = new TCanvas("c", "c", 800, 700);
    c->cd();
    h_motherPt->GetYaxis()->SetTitle("Normalized counts");
    h_motherPt->GetYaxis()->SetTitleOffset(1.5);
    h_motherPt->Draw("hist");
    fpt->Draw("same");
    TLatex *pt = new TLatex();
    pt->SetNDC();
    pt->SetTextFont(42);
    pt->SetTextSize(0.04);
    pt->DrawLatex(0.4, 0.85, "Exponentially-modified Gaussian");
    pt->DrawLatex(0.4, 0.8, Form("#mu = %.3e#pm%.3e", fpt->GetParameter(1), fpt->GetParError(1)));
    pt->DrawLatex(0.4, 0.75, Form("#sigma = %.3e#pm%.3e", fpt->GetParameter(2), fpt->GetParError(2)));
    pt->DrawLatex(0.4, 0.7, Form("#lambda = %.3e#pm%.3e", fpt->GetParameter(3), fpt->GetParError(3)));
    pt->Draw();
    c->Draw();
    c->SaveAs("Kshort_motherPt_fit.png");
    c->SaveAs("Kshort_motherPt_fit.pdf");

    c->Clear();
    c->cd();
    h_motherEta->GetYaxis()->SetRangeUser(0, h_motherEta->GetMaximum() * 1.65);
    h_motherEta->GetYaxis()->SetTitle("Normalized counts");
    h_motherEta->GetYaxis()->SetTitleOffset(1.5);
    h_motherEta->Draw("hist");
    feta->SetLineColor(kRed);
    feta->SetLineStyle(2);
    feta->Draw("same");
    pt->DrawLatex(0.55, 0.85, "Double Gaussian");
    pt->DrawLatex(0.55, 0.8, Form("#mu_{1} = %.3e#pm%.3e", feta->GetParameter(1), feta->GetParError(1)));
    pt->DrawLatex(0.55, 0.75, Form("#mu_{2} = %.3e#pm%.3e", feta->GetParameter(2), feta->GetParError(2)));
    pt->DrawLatex(0.55, 0.7, Form("#sigma = %.3e#pm%.3e", feta->GetParameter(3), feta->GetParError(3)));
    pt->Draw();
    c->Draw();
    c->SaveAs("Kshort_sampled_eta_fit.png");
    c->SaveAs("Kshort_sampled_eta_fit.pdf");

    // sample pt and eta from the fitted functions
    // and calculate the corresponding kinematics
    int Nsample = h_mother_Pt_Eta->Integral(-1, -1, -1, -1);
    double M_kshort = 0.497611; // GeV
    TH1F *h_sampled_p = new TH1F("h_sampled_p", ";Sampled K_{S}^{0} p [GeV]", 100, 0, 10);
    TH1F *h_sampled_pz = new TH1F("h_sampled_pz", ";Sampled K_{S}^{0} p_{z} [GeV]", 50, 0, 5);
    TH2F *h_sampled_pt_eta = new TH2F("h_sampled_pt_eta", ";Sampled K_{S}^{0} p_{T} [GeV];Sampled K_{S}^{0} #eta", 100, 0, 4, 44, -1.1, 1.1);
    for (int i = 0; i < Nsample; i++)
    {
        double pt = fpt->GetRandom();
        double eta = feta->GetRandom();
        double phi = gRandom->Uniform(-TMath::Pi(), TMath::Pi());
        TLorentzVector lvec_rand;
        lvec_rand.SetPtEtaPhiM(pt, eta, phi, M_kshort);
        h_sampled_pt_eta->Fill(pt, eta);
        h_sampled_p->Fill(lvec_rand.P());
        h_sampled_pz->Fill(lvec_rand.Pz());
    }
    c->Clear();
    gPad->SetRightMargin(0.13);
    c->cd();
    h_sampled_pt_eta->Draw("colz");
    c->Draw();
    c->SaveAs("Kshort_sampled_pt_eta.png");
    c->SaveAs("Kshort_sampled_pt_eta.pdf");

    c->Clear();
    gPad->SetRightMargin(0.13);
    c->cd();
    h_mother_Pt_Eta->Draw("colz");
    c->Draw();
    c->SaveAs("Kshort_truth_pt_eta.png");
    c->SaveAs("Kshort_truth_pt_eta.pdf");

    // Draw the sampled histograms
    Draw1D(h_motherP.GetPtr(), h_sampled_p, "Kshort_motherP_TF1sampled", "Kshort p", "K_{S}^{0} p [GeV]", "Normalized counts");
    Draw1D(h_motherPz.GetPtr(), h_sampled_pz, "Kshort_motherPz_TF1sampled", "Kshort pz", "K_{S}^{0} p_{z} [GeV]", "Normalized counts");
}