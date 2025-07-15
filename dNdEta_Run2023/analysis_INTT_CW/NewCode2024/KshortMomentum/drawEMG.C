// construct a Exponentially modified Gaussian distribution function
double EMG(double *x, double *par)
{
    double mu = par[0];     // Mean of the Gaussian
    double sigma = par[1];  // Width of the Gaussian
    double lambda = par[2]; // Decay constant of the Exponential

    double t = x[0];
    double z = ( mu + lambda * sigma * sigma - t) / (sqrt(2) * sigma);

    double prefactor = lambda / 2.0;
    double exp_part = exp((lambda / 2.0) * (2.0 * mu + lambda * sigma * sigma - 2.0 * t));
    double erfc_part = TMath::Erfc(z);

    return prefactor * exp_part * erfc_part;
}

void drawEMG()
{
    // Create a histogram
    TF1 *fEMG = new TF1("fEMG", EMG, -10, 10, 3);
    fEMG->SetParameters(0, 1, 1);
    fEMG->SetParNames("mu", "sigma", "lambda");
    
    TCanvas *c = new TCanvas("c", "c", 800, 700);
    c->cd();
    fEMG->Draw();
    c->Draw();
    c->SaveAs("EMG.png");
    c->SaveAs("EMG.pdf");
}
