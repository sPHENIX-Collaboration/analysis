template <typename T>
T *getObjectFromFile(const std::string& filename, const std::string& objectname)
{
    TFile file(filename.c_str(), "READ");
    if (file.IsZombie())
    {
        std::cerr << "Error opening file: " << filename << std::endl;
        return nullptr;
    }

    T* obj = dynamic_cast<T*>(file.Get(objectname.c_str()));
    if (!obj)
    {
        std::cerr << "Error retrieving object: " << objectname << " from file: " << filename << std::endl;
        file.Close();
        return nullptr;
    }

    // Clone the object to keep it in memory after closing the file
    T* obj_clone = dynamic_cast<T*>(obj->Clone());
    obj_clone->SetDirectory(nullptr); // Detach from any file directory
    file.Close();
    return obj_clone;
}

void draw1Dhistogram(TH1 *hist, bool logx, bool logy, std::string xtitle, std::string ytitle, std::vector<std::string> addinfo, std::string filename)
{
    TCanvas *c = new TCanvas("c", "c", 800, 700);
    gPad->SetTopMargin(0.07);
    c->cd();
    c->SetLogx(logx);
    c->SetLogy(logy);
    hist->GetXaxis()->SetTitle(xtitle.c_str());
    hist->GetYaxis()->SetTitle(ytitle.c_str());
    hist->GetYaxis()->SetTitleOffset(1.6);
    hist->GetYaxis()->SetRangeUser((logy) ? hist->GetMinimum(0) * 0.5 : 0, (logy) ? hist->GetMaximum() * 50 : hist->GetMaximum() * 1.4);
    hist->Draw("e1");

    TLatex *latex = new TLatex();
    latex->SetTextSize(0.04);
    latex->SetTextAlign(12);
    latex->SetNDC();
    for (size_t i = 0; i < addinfo.size(); ++i)
    {
        latex->DrawLatex(0.2, 0.88 - i * 0.05, addinfo[i].c_str());
    }
    c->SaveAs(Form("%s.png", filename.c_str()));
    c->SaveAs(Form("%s.pdf", filename.c_str()));
    delete latex;
    delete c;
}

void KshortLambdaRawRatio()
{
    const std::string kshortFile = "./figure_kshort_crosscheck/kshort_yield_pt.root";
    const std::string lambdaFile = "./figure_lambda_crosscheck/lambda_yield_pt.root";

    TH1* hKshort = getObjectFromFile<TH1>(kshortFile, "hkshortRawYieldPt");
    TH1* hLambda = getObjectFromFile<TH1>(lambdaFile, "hlambdaRawYieldPt");

    if (!hKshort || !hLambda)
    {
        std::cerr << "Error: Failed to retrieve histograms from files." << std::endl;
        return;
    }

    // scale kshort yield by 2
    hKshort->Scale(2.0);

    TH1* hRatio = dynamic_cast<TH1*>(hLambda->Clone("hRatio"));
    hRatio->SetTitle("; p_{T} [GeV]; #Lambda^{0}/2K_{S}^{0}");
    hRatio->Divide(hKshort);

    draw1Dhistogram(hRatio, false, false, "p_{T} [GeV]", "Raw yield ratio #Lambda^{0}/2K_{S}^{0}", {}, "RawYield_LFRatio");
}