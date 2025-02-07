void print_with_significant_digits(double value, int sig_digits = 4)
{
    // Determine the order of magnitude
    int magnitude = (value == 0.0) ? 0 : std::floor(std::log10(std::abs(value)));

    // Adjust precision to ensure exactly `sig_digits`
    int decimal_places = sig_digits - magnitude - 1;

    // Print using either fixed or scientific notation based on the magnitude
    if (magnitude >= -3 && magnitude <= 6)
    {
        // Use fixed notation for numbers in a reasonable range
        std::cout << std::fixed << std::setprecision(decimal_places) << value;
    }
    else
    {
        // Use scientific notation for very small or very large numbers
        std::cout << std::scientific << std::setprecision(sig_digits - 1) << value;
    }

    // Restore default formatting
    std::cout << std::defaultfloat;
}

void draw2Dhist(TH2 *hM, const char *xtitle, const char *ytitle, bool logz, vector<const char *> plotinfo, const char *plotopt, const char *plotname)
{
    float ytitleoffset = 1.6;

    TCanvas *c = new TCanvas("c", "c", 800, 700);
    gPad->SetRightMargin(0.15);
    gPad->SetTopMargin(0.07);
    c->cd();
    if (logz)
        c->SetLogz();
    hM->GetXaxis()->SetTitle(xtitle);
    hM->GetYaxis()->SetTitle(ytitle);
    hM->GetYaxis()->SetTitleOffset(ytitleoffset);
    // only maximal 3 digits for the axis labels
    TGaxis::SetMaxDigits(3);
    hM->Draw(plotopt);
    TLatex *t = new TLatex();
    t->SetTextSize(0.035);
    for (size_t i = 0; i < plotinfo.size(); ++i)
    {
        t->DrawLatexNDC(0.25, 0.85 - i * 0.045, plotinfo[i]);
    }
    c->SaveAs(Form("%s.pdf", plotname));
    c->SaveAs(Form("%s.png", plotname));
}