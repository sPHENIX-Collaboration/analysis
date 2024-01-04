//
// Code to process validation sample 0.5-25 GeV
// input: /sphenix/u/anarde/scratch/data/validation-0.3-25-GeV.root
// output: /sphenix/user/anarde/output/validation-plots-0.3-25-GeV.root
//
namespace myAnalysis {
    void process();
    Int_t fit(const string &name, const string& output, TFile* input, TCanvas* c, Float_t &mu, Float_t &mu_error);
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

    string outDir     = "output/1-3-24";
    string inputFile1 = "output/validation-plots-0.3-25-GeV.root";
    string output1    = outDir + "/" + "fits-0.3-25.pdf";
    string output2    = outDir + "/" + "qa.pdf";
    string output3    = outDir + "/" + "response.pdf";
    string output4    = outDir + "/" + "response.root";

    Float_t min_mu = 9999;
    Float_t max_mu = 0;

    UInt_t  bins_ecore = 49;
    Float_t low_ecore  = 0.5;
    Float_t high_ecore = 25;

    UInt_t  bins_eta = 22;
    Float_t low_eta  = -1.1;
    Float_t high_eta = 1.1;

    Bool_t saveFits = true;
}

Int_t myAnalysis::fit(const string &name, const string& output, TFile* input, TCanvas* c, Float_t &mu, Float_t &mu_error) {
    auto h1 = (TH1F*)input->Get(name.c_str());
    if(!h1) return -1;
    c->cd();
    h1 = (TH1F*)(h1->Clone());

    // check first that there is at least one entry within the fitting range provided to perform the fit.
    Float_t low_fit  = max(0.9, h1->GetBinLowEdge(h1->FindFirstBinAbove(0)));
    Float_t high_fit = 1.2;
    Int_t bin_min = h1->FindBin(low_fit);
    Int_t bin_max = h1->FindBin(high_fit);
    Int_t counts = h1->Integral(bin_min, bin_max);
    if(counts < 10) return -2;

    TFitResultPtr fit;

    auto f1 = new TF1("f1","gaus",low_fit, high_fit);

    f1->SetParameter(0, 3500);
    f1->SetParameter(1, 1);
    f1->SetParameter(2, 0.1);
    // f1->SetParameter(3, -1);
    // f1->SetParLimits(1, 200, 5000);
    // f1->SetParLimits(1, 1.07, 1.1);
    f1->SetParLimits(2, 0, 0.2);

    // fitting options being Used
    // S: The full result of the fit is returned in the TFitResultPtr. This is needed to get the covariance matrix of the fit. See TFitResult and the base class ROOT::Math::FitResult.
    // Q: Quiet mode (minimum printing)
    // L: Uses a log likelihood method (default is chi-square method). To be used when the histogram represents counts.
    // R: Fit using a fitting range specified in the function range with TF1::SetRange.
    fit = h1->Fit(f1,"BQRS");
    // status = 0: success
    // status < 0: there is an error not connected with the minimization procedure, for example when a wrong function is used.
    // status > 0: return status from Minimizer, depends on used Minimizer
    Int_t fitStatus = fit;

    // if fit fails then try again
    if(fitStatus == 3) {
        fit = h1->Fit(f1,"BQRS");
        fitStatus = fit;
    }
    if(fitStatus) return fitStatus;

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
    Float_t sigma = fit->Parameter(2);

    min_mu = min(min_mu, mu);
    max_mu = max(max_mu, mu);

    if(saveFits) c->Print((output).c_str(), "pdf portrait");
    return 0;
}

void myAnalysis::process() {

    input1 = TFile::Open(inputFile1.c_str());

    c1 = new TCanvas();

    hResponseCalib  = new TH1F("hResponseCalib", "Gaussian Fit E_{Core}/E_{Truth}; E_{Core}/E_{Truth}; Counts", 36, 0.96, 1.14);
    h2ResponseCalib = new TH2F("h2ResponseCalib", "Gaussian Fit Calib E_{Core}/E_{Truth}; E_{Core} Calib [GeV]; Cluster #eta", bins_ecore, low_ecore, high_ecore, bins_eta, low_eta, high_eta);

    c1->SetTickx();
    c1->SetTicky();

    c1->SetCanvasSize(1500, 1000);
    c1->SetLeftMargin(.12);
    c1->SetRightMargin(.05);

    if(saveFits) c1->Print((output1 + "[").c_str(), "pdf portrait");

    Float_t mu       = 0;
    Float_t mu_error = 0;

    for(UInt_t i = 0; i < bins_ecore; ++i) {
        for(UInt_t j = 0; j < bins_eta; ++j) {

            string name = "response_calib/hResponseCalib_"+to_string(i)+"_"+to_string(j);
            Int_t fitStatus = fit(name, output1, input1, c1, mu, mu_error);

            if(!fitStatus) {
                h2ResponseCalib->SetBinContent(i+1, j+1, mu);
                h2ResponseCalib->SetBinError(i+1, j+1, mu_error);
                hResponseCalib->Fill(mu);

                if(mu < 1 || mu > 1.05) cout << "mu: " << mu << ", i: " << i << ", j: " << j << endl;
            }
            else cout << "i: " << i << ", j: " << j << ", status: " << fitStatus << endl;
        }
    }

    cout << "min mu: " << min_mu << ", max mu: " << max_mu << endl;

    if(saveFits) c1->Print((output1 + "]").c_str(), "pdf portrait");

    TFile output(output4.c_str(), "recreate");
    output.cd();
    h2ResponseCalib->Write();
    output.Close();

    c1->Print((output2 + "[").c_str(), "pdf portrait");
    c1->SetLeftMargin(.1);
    c1->SetRightMargin(.06);

    // ----------------------------------------------

    auto h1 = (TH1F*)input1->Get("hClusECoreCalib");

    h1->Draw();

    c1->Print((output2).c_str(), "pdf portrait");
    c1->Print((outDir + "/qa-ClusterECoreCalib.png").c_str());

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

    h2ResponseCalib->SetMinimum(0.96);
    h2ResponseCalib->SetMaximum(1.14);
    h2ResponseCalib->GetXaxis()->SetRangeUser(0.5,25);
    h2ResponseCalib->SetStats(0);
    h2ResponseCalib->Draw("COLZ1");

    c1->Print((output3).c_str(), "pdf portrait");
    c1->Print((outDir + "/h2ResponseCalib.png").c_str());

    // ----------------------------------------------

    cout << "hResponseCalib max: " << hResponseCalib->GetMaximum() << endl;
    c1->SetRightMargin(.06);
    hResponseCalib->Draw("PE");

    c1->Print((output3).c_str(), "pdf portrait");
    c1->Print((outDir + "/hResponse.png").c_str());

    c1->Print((output3 + "]").c_str(), "pdf portrait");

    input1->Close();
}

void PDC_display_v3() {
    myAnalysis::process();
}
