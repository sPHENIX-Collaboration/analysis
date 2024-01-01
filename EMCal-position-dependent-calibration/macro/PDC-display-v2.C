//
// Code to process validation sample 20-21 GeV
// input: /sphenix/user/anarde/output/validation-plots-20-21-GeV.root
// output: /sphenix/user/anarde/output/12-29-23
//

namespace myAnalysis {
    void process();
    Bool_t fit(const string &name, const string& output, TFile* input, TCanvas* c, Float_t &mu, Float_t &mu_error);
    void PDC_display();

    TFile* input1;
    TCanvas* c1;

    TH1F* hResponse;
    TH1F* hResponseCalib;

    TH2F* h2Response;
    TH2F* h2ResponseCalib;

    vector<TH1F*> hResponseEta;
    vector<TH1F*> hResponseCalibEta;

    vector<TH1F*> hResponsePt;
    vector<TH1F*> hResponseCalibPt;

    string outDir = "output/12-29-23";
    string inputFile1 = "output/validation-plots-20-21-GeV.root";
    string output1    = outDir + "/" + "fits-20-21.pdf";
    string output2    = outDir + "/" + "qa.pdf";
    string output3    = outDir + "/" + "response.pdf";

    Float_t min_mu = 9999;
    Float_t max_mu = 0;

    UInt_t  bins_pt = 10;
    Float_t low_pt  = 20;
    Float_t high_pt = 21;

    UInt_t  bins_eta = 40;
    Float_t low_eta  = -1;
    Float_t high_eta = 1;
}

Bool_t myAnalysis::fit(const string &name, const string& output, TFile* input, TCanvas* c, Float_t &mu, Float_t &mu_error) {
    auto h1 = (TH1F*)input->Get(name.c_str());
    if(!h1) return false;
    c->cd();
    h1 = (TH1F*)(h1->Clone());

    // check first that there is a
    // t least one entry within the fitting range provided to perform the fit.
    Float_t low_fit  = 0.9;
    Float_t high_fit = 1.15;
    Int_t bin_min = h1->FindBin(low_fit);
    Int_t bin_max = h1->FindBin(high_fit);
    Int_t counts = h1->Integral(bin_min, bin_max);
    if(counts < 10) return false;

    TFitResultPtr fit;

    auto f1 = new TF1("f1","gaus",low_fit, high_fit);

    // f1->SetParameter(0, 3000);
    f1->SetParameter(1, 1.05);
    f1->SetParameter(2, 0.07);
    // f1->SetParameter(3, -1);
    // f1->SetParLimits(1, 200, 5000);
    // f1->SetParLimits(1, 1.07, 1.1);
    // f1->SetParLimits(2, 0.02, 0.1);

    // fitting options being Used
    // S: The full result of the fit is returned in the TFitResultPtr. This is needed to get the covariance matrix of the fit. See TFitResult and the base class ROOT::Math::FitResult.
    // Q: Quiet mode (minimum printing)
    // L: Uses a log likelihood method (default is chi-square method). To be used when the histogram represents counts.
    // R: Fit using a fitting range specified in the function range with TF1::SetRange.
    fit = h1->Fit(f1,"BQRS");
    // fitStatus = fit;

    gStyle->SetOptFit(1111);

    auto st = (TPaveStats*)h1->GetListOfFunctions()->FindObject("stats");
    st->SetX1NDC(0.65); //new x start position
    st->SetX2NDC(0.95); //new x end position

    st->SetY1NDC(0.6); //new y start position
    st->SetY2NDC(0.9); //new y end position

    h1->GetXaxis()->SetRangeUser(0,2);
    // gPad->SetLogy();

    h1->Draw();

    mu = fit->Parameter(1);
    mu_error = fit->ParError(1);

    min_mu = min(min_mu, mu);
    max_mu = max(max_mu, mu);

    c->Print((output).c_str(), "pdf portrait");
    // if(mu < 1 || mu > 1.5) c->Print((output).c_str(), "pdf portrait");
    // c->Print((outDir + "/fits/" + string(h1->GetName()) + ".png").c_str());
    return true;
}

void myAnalysis::process() {

    input1 = TFile::Open(inputFile1.c_str());

    c1 = new TCanvas();

    hResponse       = new TH1F("hResponse", "Gaussian Fit E_{Core}/E_{Truth}; E_{Core}/E_{Truth}; Counts", 50, 1, 1.06);
    hResponseCalib  = new TH1F("hResponseCalib", "Gaussian Fit E_{Core}/E_{Truth}; E_{Core}/E_{Truth}; Counts", 50, 1, 1.06);
    h2Response      = new TH2F("h2Response", "Gaussian Fit E_{Core}/E_{Truth}; p^{Truth} [GeV]; #eta_{Truth}", bins_pt, low_pt, high_pt, bins_eta, low_eta, high_eta);
    h2ResponseCalib = new TH2F("h2ResponseCalib", "Gaussian Fit Calib E_{Core}/E_{Truth}; p^{Truth} [GeV]; #eta_{Truth}", bins_pt, low_pt, high_pt, bins_eta, low_eta, high_eta);

    c1->SetTickx();
    c1->SetTicky();

    c1->SetCanvasSize(1500, 1000);
    c1->SetLeftMargin(.12);
    c1->SetRightMargin(.05);

    c1->Print((output1 + "[").c_str(), "pdf portrait");

    Float_t mu       = 0;
    Float_t mu_error = 0;

    for(UInt_t i = 0; i < bins_pt; ++i) {
        // UInt_t pt = i+3;
        for(UInt_t j = 0; j < bins_eta; ++j) {
            string name = "response/hResponse_"+to_string(i)+"_"+to_string(j);
            Bool_t fitStatus = fit(name, output1, input1, c1, mu, mu_error);

            // // i = 0 and i = 1
            if(fitStatus) {
                h2Response->SetBinContent(i+1, j+1, mu);
                h2Response->SetBinError(i+1, j+1, mu_error);
                hResponse->Fill(mu);

                if(mu < 1 || mu > 1.5) cout << "mu: " << mu << ", i: " << i << ", j: " << j << endl;
            }

            name = "response_calib/hResponseCalib_"+to_string(i)+"_"+to_string(j);
            fitStatus = fit(name, output1, input1, c1, mu, mu_error);

            // // i = 0 and i = 1
            if(fitStatus) {
                h2ResponseCalib->SetBinContent(i+1, j+1, mu);
                h2ResponseCalib->SetBinError(i+1, j+1, mu_error);
                hResponseCalib->Fill(mu);
            }
        }
    }

    cout << "min mu: " << min_mu << ", max mu: " << max_mu << endl;

    c1->Print((output1 + "]").c_str(), "pdf portrait");

    c1->Print((output2 + "[").c_str(), "pdf portrait");
    c1->SetLeftMargin(.1);
    c1->SetRightMargin(.06);

    // ----------------------------------------------

    auto h1 = (TH1F*)input1->Get("hClusECore");
    auto legend = new TLegend(0.75,0.6,0.94,0.9);

    h1->SetStats(0);
    h1->SetLineColor(kBlue);
    h1->GetXaxis()->SetRangeUser(0,60);
    legend->AddEntry(h1,"Uncalibrated", "PE");
    gPad->SetLogy();
    h1->Draw();

    h1 = (TH1F*)input1->Get("hClusECoreCalib");

    h1->SetLineColor(kRed);
    legend->AddEntry(h1,"Calibrated", "PE");
    h1->Draw("same");

    legend->SetTextSize(0.04);
    legend->Draw("same");

    c1->Print((output2).c_str(), "pdf portrait");
    c1->Print((outDir + "/qa-ClusterEnergy.png").c_str());

    gPad->SetLogy(0);

    // ----------------------------------------------

    h1 = (TH1F*)input1->Get("hPhotonGE");

    h1->SetStats(0);
    h1->GetXaxis()->SetRangeUser(19.8,21.2);
    h1->Draw();

    c1->Print((output2).c_str(), "pdf portrait");
    c1->Print((outDir + "/qa-PhotonGE.png").c_str());

    // ----------------------------------------------

    c1->SetLeftMargin(.11);
    c1->SetRightMargin(.06);

    h1 = (TH1F*)input1->Get("hPhotonPt");

    h1->SetStats(0);
    h1->GetXaxis()->SetRangeUser(10,22);
    h1->Draw();

    c1->Print((output2).c_str(), "pdf portrait");
    c1->Print((outDir + "/qa-PhotonPt.png").c_str());

    // ----------------------------------------------

    c1->Print((output2 + "]").c_str(), "pdf portrait");

    c1->Print((output3 + "[").c_str(), "pdf portrait");

    c1->SetLeftMargin(.1);
    c1->SetRightMargin(.12);

    // ----------------------------------------------

    h2Response->SetMinimum(1);
    h2Response->SetMaximum(1.06);
    h2Response->SetStats(0);
    h2Response->Draw("COLZ1");

    c1->Print((output3).c_str(), "pdf portrait");
    c1->Print((outDir + "/h2Response.png").c_str());

    h2ResponseCalib->SetMinimum(1);
    h2ResponseCalib->SetMaximum(1.06);
    h2ResponseCalib->SetStats(0);
    h2ResponseCalib->Draw("COLZ1");

    // ----------------------------------------------

    c1->Print((output3).c_str(), "pdf portrait");
    c1->Print((outDir + "/h2ResponseCalib.png").c_str());

    cout << "hResponse max: " << hResponse->GetMaximum() << ", hResponseCalib max: " << hResponseCalib->GetMaximum() << endl;
    c1->SetRightMargin(.06);
    legend = new TLegend(0.75,0.6,0.94,0.9);
    hResponse->SetStats(0);
    hResponse->SetLineColor(kBlue);
    hResponse->GetYaxis()->SetRangeUser(0,120);
    legend->AddEntry(hResponse,"Uncalibrated", "PE");
    hResponse->Draw("PE");

    hResponseCalib->SetStats(0);
    hResponseCalib->SetLineColor(kRed);
    legend->AddEntry(hResponseCalib,"Calibrated", "PE");
    hResponseCalib->Draw("same PE");

    legend->SetTextSize(0.04);
    legend->Draw("same");

    c1->Print((output3).c_str(), "pdf portrait");
    c1->Print((outDir + "/hResponse.png").c_str());

    c1->Print((output3 + "]").c_str(), "pdf portrait");

    input1->Close();
}

void PDC_display_v2() {
    myAnalysis::process();
}
