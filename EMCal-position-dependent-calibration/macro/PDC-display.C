namespace myAnalysis {
    void process();
    Bool_t fit(const string &name, const string& output, TFile* input, TCanvas* c, Float_t &mu, Float_t &mu_error);
    void PDC_display();

    TFile* input1;
    TFile* input2;
    TCanvas* c1;
    TCanvas* c2;

    TH1F* hResponse;
    TH1F* hResponseCalib;
    TH2F* h2Response;
    TH2F* h2ResponseCalib;

    string outDir = "output/12-21-23";
    string inputFile1 = "data/test-3-5.root";
    string output1    = outDir + "/" + "fits-3-5.pdf";

    string inputFile2 = "data/test-5-25.root";
    string output2    = outDir + "/" + "fits-5-25.pdf";

    string output3    = outDir + "/" + "response.pdf";

    Float_t min_mu = 9999;
    Float_t max_mu = 0;
}

Bool_t myAnalysis::fit(const string &name, const string& output, TFile* input, TCanvas* c, Float_t &mu, Float_t &mu_error) {
    auto h1 = (TH1F*)input->Get(name.c_str());
    if(!h1) return false;
    c->cd();
    // cout << name << endl;
    h1 = (TH1F*)(h1->Clone());

    // check first that there is a
    // t least one entry within the fitting range provided to perform the fit.
    Int_t bin_min = h1->FindBin(0);
    Int_t bin_max = h1->FindBin(2);
    Int_t counts = h1->Integral(bin_min, bin_max);
    if(!counts) return false;

    TFitResultPtr fit;

    auto f1 = new TF1("f1","gaus",0.88,1.5);

    // f1->SetParameter(0, 3000);
    f1->SetParameter(1, 1.05);
    f1->SetParameter(2, 0.09);
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

    h1->Draw();

    c->Print((output).c_str(), "pdf portrait");

    mu = fit->Parameter(1);
    mu_error = fit->ParError(1);

    min_mu = min(min_mu, mu);
    max_mu = max(max_mu, mu);

    c->Print((outDir + "/fits/" + string(h1->GetName()) + ".png").c_str());
    return true;
}

void myAnalysis::process() {

    input1 = TFile::Open(inputFile1.c_str());
    input2 = TFile::Open(inputFile2.c_str());

    c1 = new TCanvas();
    c2 = new TCanvas();

    hResponse       = new TH1F("hResponse", "Gaussian Fit E_{Core}/E_{Truth}; E_{Core}/E_{Truth}; Counts", 35, 0.99, 1.06);
    hResponseCalib  = new TH1F("hResponseCalib", "Gaussian Fit E_{Core}/E_{Truth}; E_{Core}/E_{Truth}; Counts", 35, 0.99, 1.06);
    h2Response      = new TH2F("h2Response", "Gaussian Fit E_{Core}/E_{Truth}; p_{T}^{Truth} [GeV]; #eta_{Truth}", 22, 3, 25, 10, -1, 1);
    h2ResponseCalib = new TH2F("h2ResponseCalib", "Gaussian Fit Calib E_{Core}/E_{Truth}; p_{T}^{Truth} [GeV]; #eta_{Truth}", 22, 3, 25, 10, -1, 1);

    c1->SetTickx();
    c1->SetTicky();

    c2->SetTickx();
    c2->SetTicky();

    c1->SetCanvasSize(1500, 1000);
    c1->SetLeftMargin(.12);
    c1->SetRightMargin(.05);

    c2->SetCanvasSize(1500, 1000);
    c2->SetLeftMargin(.12);
    c2->SetRightMargin(.05);

    c1->Print((output1 + "[").c_str(), "pdf portrait");
    c2->Print((output2 + "[").c_str(), "pdf portrait");

    Float_t mu       = 0;
    Float_t mu_error = 0;

    for(UInt_t i = 0; i < 22; ++i) {
        for(UInt_t j = 0; j < 10; ++j) {
            string name = "response/hResponse_"+to_string(i)+"_"+to_string(j);
            Bool_t fitStatus = fit(name, output1, input1, c1, mu, mu_error);

            if(fitStatus) {
                h2Response->SetBinContent(i+1, j+1, mu);
                h2Response->SetBinError(i+1, j+1, mu_error);
                hResponse->Fill(mu);
            }

            fitStatus = fit(name, output2, input2, c2, mu, mu_error);

            if(fitStatus) {
                h2Response->SetBinContent(i+1, j+1, mu);
                h2Response->SetBinError(i+1, j+1, mu_error);
                hResponse->Fill(mu);
            }

            name = "response_calib/hResponseCalib_"+to_string(i)+"_"+to_string(j);
            fitStatus = fit(name, output1, input1, c1, mu, mu_error);

            if(fitStatus) {
                h2ResponseCalib->SetBinContent(i+1, j+1, mu);
                h2ResponseCalib->SetBinError(i+1, j+1, mu_error);
                hResponseCalib->Fill(mu);
            }

            fitStatus = fit(name, output2, input2, c2, mu, mu_error);

            if(fitStatus) {
                h2ResponseCalib->SetBinContent(i+1, j+1, mu);
                h2ResponseCalib->SetBinError(i+1, j+1, mu_error);
                hResponseCalib->Fill(mu);
            }
        }
    }

    cout << "min mu: " << min_mu << ", max mu: " << max_mu << endl;

    c1->Print((output1 + "]").c_str(), "pdf portrait");
    c2->Print((output2 + "]").c_str(), "pdf portrait");

    c2->Print((output3 + "[").c_str(), "pdf portrait");
    c2->SetLeftMargin(.1);
    c2->SetRightMargin(.11);
    c2->cd();

    h2Response->SetMinimum(0.99);
    h2Response->SetMaximum(1.06);
    h2Response->SetStats(0);
    h2Response->Draw("COLZ1");

    c2->Print((output3).c_str(), "pdf portrait");
    c2->Print((outDir + "/h2Response.png").c_str());

    h2ResponseCalib->SetMinimum(0.99);
    h2ResponseCalib->SetMaximum(1.06);
    h2ResponseCalib->SetStats(0);
    h2ResponseCalib->Draw("COLZ1");

    c2->Print((output3).c_str(), "pdf portrait");
    c2->Print((outDir + "/h2ResponseCalib.png").c_str());


    c2->SetRightMargin(.06);
    auto legend = new TLegend(0.7,0.6,0.94,0.9);
    hResponse->SetStats(0);
    hResponse->SetLineColor(kBlue);
    hResponse->GetYaxis()->SetRangeUser(0,30);
    legend->AddEntry(hResponse,"Uncalibrated", "PE");
    hResponse->Draw("PE");

    hResponseCalib->SetStats(0);
    hResponseCalib->SetLineColor(kRed);
    legend->AddEntry(hResponseCalib,"Calibrated", "PE");
    hResponseCalib->Draw("same PE");

    legend->SetTextSize(0.05);
    legend->Draw("same");

    c2->Print((output3).c_str(), "pdf portrait");
    c2->Print((outDir + "/hResponse.png").c_str());

    c2->Print((output3 + "]").c_str(), "pdf portrait");

    input1->Close();
    input2->Close();
}

void PDC_display() {
    myAnalysis::process();
}
