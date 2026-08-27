void fitter()
{
    // gStyle->SetOptStat(1111);
    // gStyle->SetOptFit(1111);
    TFile *f = new TFile("../../qa_results.root");

    TGraphErrors *g_locpolbf = (TGraphErrors*)f->Get("locpolphasebf");

    TF1 *fline = new TF1("fline","pol0",45000,52000);

    g_locpolbf->GetYaxis()->SetRangeUser(-5,5);
    g_locpolbf->GetYaxis()->SetTitle("Blue asymmetry phase (rad)");
    g_locpolbf->GetXaxis()->SetTitle("Run number");
    g_locpolbf->Draw("ape");

    g_locpolbf->Fit(fline,"rob=0.8");

    float constant = fline->GetParameter(0);

    float RMSE = 0.;
    for (int i = 0; i < g_locpolbf->GetN(); ++i)
    {
        RMSE += pow(g_locpolbf->GetY()[i]-constant,2);
    }

    RMSE /= g_locpolbf->GetN();
    RMSE = sqrt(RMSE);
    std::cout << constant << " " << RMSE << std::endl;

    TLine *uband = new TLine(42000,constant+RMSE,54000,constant+RMSE);
    uband->SetLineStyle(2);
    uband->SetLineColor(kRed);
    uband->Draw();

    TLine *lband = new TLine(42000,constant-RMSE,54000,constant-RMSE);
    lband->SetLineStyle(2);
    lband->SetLineColor(kRed);
    lband->Draw();


}