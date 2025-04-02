float TickSize = 0.03;
float AxisTitleSize = 0.05;
float AxisLabelSize = 0.045;
float LeftMargin = 0.15;
float RightMargin = 0.05;
float TopMargin = 0.08;
float BottomMargin = 0.13;
float textscale = 2.6;

std::string plotdir = "./countLepton";

bool checkfile(const std::string &filename)
{
    TFile *f = TFile::Open(filename.c_str());
    if (!f || f->IsZombie() || f->GetSize() <= 0)
    {
        std::cout << "Skipping invalid or empty file: " << filename << std::endl;
        if (f)
            f->Close();
        return false;
    }

    if (!f->GetListOfKeys()->Contains("minitree"))
    {
        std::cout << "Skipping file without minitree: " << filename << std::endl;
        f->Close();
        return false;
    }

    // Check if PrimaryG4P_PID branch exists
    TTree *tree = (TTree*)f->Get("minitree");
    if (!tree || !tree->GetBranch("PrimaryG4P_PID"))
    {
        std::cout << "Skipping file without PrimaryG4P_PID branch: " << filename << std::endl;
        f->Close();
        return false;
    }

    f->Close();
    return true;
}

// return std::tuple<float, float, float> for the strange fraction of HIJING, EPOS, and AMPT
std::tuple<float, float, float> getStrangeFrac(int fileidx = 0, bool makeplot = false)
{
    // make sure all the three files exist and is valid, and contains the required tree
    std::string file_HIJING = "/sphenix/tg/tg01/hf/hjheng/ppg02/minitree/TrackletMinitree_Sim_HIJING_MDC2_ana467_20250225/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0/minitree_" + std::string(TString::Format("%05d", fileidx).Data()) + ".root";
    std::string file_EPOS = "/sphenix/tg/tg01/hf/hjheng/ppg02/minitree/TrackletMinitree_Sim_EPOS_MDC2_ana467_20250301/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0/minitree_" + std::string(TString::Format("%05d", fileidx).Data()) + ".root";
    std::string file_AMPT = "/sphenix/tg/tg01/hf/hjheng/ppg02/minitree/TrackletMinitree_Sim_AMPT_MDC2_ana467_20250301/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0/minitree_" + std::string(TString::Format("%05d", fileidx).Data()) + ".root";

    bool valid_HIJING = checkfile(file_HIJING);
    bool valid_EPOS = checkfile(file_EPOS);
    bool valid_AMPT = checkfile(file_AMPT);

    if (!valid_HIJING || !valid_EPOS || !valid_AMPT)
    {
        return std::make_tuple(std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN());
    }

    ROOT::EnableImplicitMT();
    ROOT::RDataFrame df_HIJING("minitree", file_HIJING);
    ROOT::RDataFrame df_EPOS("minitree", file_EPOS);
    ROOT::RDataFrame df_AMPT("minitree", file_AMPT);

    // Define a helper function to calculate NStrange and NPrimaryG4P
    auto defineNStrange = [](ROOT::RDF::RNode df)
    {
        return df.Define("NPrimaryG4P", [](const std::vector<int> &PrimaryG4P_PID) { return static_cast<int>(PrimaryG4P_PID.size()); }, {"PrimaryG4P_PID"})
            .Define("NStrange",
                    [](const std::vector<int> &PrimaryG4P_PID, const std::vector<float> &PrimaryG4P_eta)
                    {
                        int NStrange = 0;
                        for (size_t i = 0; i < PrimaryG4P_PID.size(); i++)
                        {
                            if ((PrimaryG4P_PID[i] == 310 || abs(PrimaryG4P_PID[i]) == 3122) && fabs(PrimaryG4P_eta[i]) < 1.5)
                            {
                                NStrange++;
                            }
                        }
                        return NStrange;
                    },
                    {"PrimaryG4P_PID", "PrimaryG4P_eta"});
    };

    // Apply the function to each dataframe
    auto df_HIJING_NStrange = defineNStrange(df_HIJING);
    auto df_EPOS_NStrange = defineNStrange(df_EPOS);
    auto df_AMPT_NStrange = defineNStrange(df_AMPT);

    // Get the vector of NStrange and NPrimaryG4P
    auto v_NStrange_HIJING = df_HIJING_NStrange.Take<int>("NStrange");
    auto v_NStrange_EPOS = df_EPOS_NStrange.Take<int>("NStrange");
    auto v_NStrange_AMPT = df_AMPT_NStrange.Take<int>("NStrange");
    auto v_NPrimaryG4P_HIJING = df_HIJING_NStrange.Take<int>("NPrimaryG4P");
    auto v_NPrimaryG4P_EPOS = df_EPOS_NStrange.Take<int>("NPrimaryG4P");
    auto v_NPrimaryG4P_AMPT = df_AMPT_NStrange.Take<int>("NPrimaryG4P");

    float avg_strange_HIJING = 0, avg_strange_EPOS = 0, avg_strange_AMPT = 0;
    for (size_t i = 0; i < v_NStrange_HIJING->size(); i++)
    {
        avg_strange_HIJING += (float)(*v_NStrange_HIJING)[i] / (*v_NPrimaryG4P_HIJING)[i] * 100.;
        avg_strange_EPOS += (float)(*v_NStrange_EPOS)[i] / (*v_NPrimaryG4P_EPOS)[i] * 100.;
        avg_strange_AMPT += (float)(*v_NStrange_AMPT)[i] / (*v_NPrimaryG4P_AMPT)[i] * 100.;
    }
    avg_strange_HIJING /= v_NStrange_HIJING->size();
    avg_strange_EPOS /= v_NStrange_EPOS->size();
    avg_strange_AMPT /= v_NStrange_AMPT->size();

    if (makeplot)
    {
        // Create 3 histograms, one for the number of strange particles, one for the number of primary G4P, and one for the ratio of the two. All these three are as a function of the event index
        auto h_NStrange_HIJING = new TH1F("h_NStrange_HIJING", "h_NStrange_HIJING;Event index;NStrange", v_NStrange_HIJING->size(), 0, v_NStrange_HIJING->size());
        auto h_NStrange_EPOS = new TH1F("h_NStrange_EPOS", "h_NStrange_EPOS;Event index;NStrange", v_NStrange_EPOS->size(), 0, v_NStrange_EPOS->size());
        auto h_NStrange_AMPT = new TH1F("h_NStrange_AMPT", "h_NStrange_AMPT;Event index;NStrange", v_NStrange_AMPT->size(), 0, v_NStrange_AMPT->size());
        auto h_NPrimaryG4P_HIJING = new TH1F("h_NPrimaryG4P_HIJING", "h_NPrimaryG4P_HIJING;Event index;NPrimaryG4P", v_NPrimaryG4P_HIJING->size(), 0, v_NPrimaryG4P_HIJING->size());
        auto h_NPrimaryG4P_EPOS = new TH1F("h_NPrimaryG4P_EPOS", "h_NPrimaryG4P_EPOS;Event index;NPrimaryG4P", v_NPrimaryG4P_EPOS->size(), 0, v_NPrimaryG4P_EPOS->size());
        auto h_NPrimaryG4P_AMPT = new TH1F("h_NPrimaryG4P_AMPT", "h_NPrimaryG4P_AMPT;Event index;NPrimaryG4P", v_NPrimaryG4P_AMPT->size(), 0, v_NPrimaryG4P_AMPT->size());
        auto h_NStrange_ratio_HIJING = new TH1F("h_NStrange_ratio_HIJING", "h_NStrange_ratio_HIJING;Event index;NStrange/NPrimaryG4P", v_NStrange_HIJING->size(), 0, v_NStrange_HIJING->size());
        auto h_NStrange_ratio_EPOS = new TH1F("h_NStrange_ratio_EPOS", "h_NStrange_ratio_EPOS;Event index;NStrange/NPrimaryG4P", v_NStrange_EPOS->size(), 0, v_NStrange_EPOS->size());
        auto h_NStrange_ratio_AMPT = new TH1F("h_NStrange_ratio_AMPT", "h_NStrange_ratio_AMPT;Event index;NStrange/NPrimaryG4P", v_NStrange_AMPT->size(), 0, v_NStrange_AMPT->size());

        float avg_strange_HIJING = 0, avg_strange_EPOS = 0, avg_strange_AMPT = 0;
        for (size_t i = 0; i < v_NStrange_HIJING->size(); i++)
        {
            h_NStrange_HIJING->SetBinContent(i + 1, (*v_NStrange_HIJING)[i]);
            h_NStrange_EPOS->SetBinContent(i + 1, (*v_NStrange_EPOS)[i]);
            h_NStrange_AMPT->SetBinContent(i + 1, (*v_NStrange_AMPT)[i]);
            h_NPrimaryG4P_HIJING->SetBinContent(i + 1, (*v_NPrimaryG4P_HIJING)[i]);
            h_NPrimaryG4P_EPOS->SetBinContent(i + 1, (*v_NPrimaryG4P_EPOS)[i]);
            h_NPrimaryG4P_AMPT->SetBinContent(i + 1, (*v_NPrimaryG4P_AMPT)[i]);
            h_NStrange_ratio_HIJING->SetBinContent(i + 1, (float)(*v_NStrange_HIJING)[i] / (*v_NPrimaryG4P_HIJING)[i] * 100.);
            h_NStrange_ratio_EPOS->SetBinContent(i + 1, (float)(*v_NStrange_EPOS)[i] / (*v_NPrimaryG4P_EPOS)[i] * 100.);
            h_NStrange_ratio_AMPT->SetBinContent(i + 1, (float)(*v_NStrange_AMPT)[i] / (*v_NPrimaryG4P_AMPT)[i] * 100.);
        }

        // directory to save the output plots; if the directory does not exist, create it using
        system(Form("mkdir -p %s", plotdir.c_str()));

        float pad1_ymax = std::max({h_NPrimaryG4P_HIJING->GetMaximum(), h_NPrimaryG4P_EPOS->GetMaximum(), h_NPrimaryG4P_AMPT->GetMaximum()}) * 1.2;
        float pad2_ymax = std::max({h_NStrange_ratio_HIJING->GetMaximum(), h_NStrange_ratio_EPOS->GetMaximum(), h_NStrange_ratio_AMPT->GetMaximum()}) * 1.2;
        float pad1_ymin = std::min({h_NStrange_HIJING->GetMinimum(0) * 0.3, h_NStrange_EPOS->GetMinimum(0) * 0.3, h_NStrange_AMPT->GetMinimum(0) * 0.3});
        float pad2_ymin = 0;
        TCanvas *c = new TCanvas("c", "c", 1000, 700);
        TPad *pad1 = new TPad("pad1", "pad1", 0, 0.3, 1, 1);
        pad1->SetBottomMargin(0);
        pad1->SetTopMargin(0.33);
        pad1->SetRightMargin(RightMargin);
        pad1->Draw();
        TPad *pad2 = new TPad("pad2", "pad2", 0, 0, 1, 0.3);
        pad2->SetRightMargin(RightMargin);
        pad2->SetTopMargin(0);
        pad2->SetBottomMargin(0.33);
        pad2->Draw();
        pad1->cd();
        pad1->SetLogy(1);
        h_NPrimaryG4P_HIJING->SetLineColor(30);
        h_NPrimaryG4P_HIJING->SetLineWidth(2);
        h_NPrimaryG4P_HIJING->GetYaxis()->SetTitle("Counts");
        h_NPrimaryG4P_HIJING->GetYaxis()->SetTitleOffset(1.4);
        h_NPrimaryG4P_HIJING->GetYaxis()->SetRangeUser(pad1_ymin, pad1_ymax);
        h_NPrimaryG4P_HIJING->GetYaxis()->SetTitleSize(AxisTitleSize);
        h_NPrimaryG4P_HIJING->GetYaxis()->SetLabelSize(AxisLabelSize);
        h_NPrimaryG4P_HIJING->GetXaxis()->SetTitleSize(0);
        h_NPrimaryG4P_HIJING->GetXaxis()->SetLabelSize(0);
        h_NPrimaryG4P_HIJING->Draw("hist");
        h_NPrimaryG4P_EPOS->SetLineColor(38);
        h_NPrimaryG4P_EPOS->SetLineWidth(2);
        h_NPrimaryG4P_EPOS->Draw("hist same");
        h_NPrimaryG4P_AMPT->SetLineColor(46);
        h_NPrimaryG4P_AMPT->SetLineWidth(2);
        h_NPrimaryG4P_AMPT->Draw("hist same");
        h_NStrange_HIJING->SetLineColor(30);
        h_NStrange_HIJING->SetLineWidth(2);
        h_NStrange_HIJING->SetLineStyle(2);
        h_NStrange_HIJING->Draw("hist same");
        h_NStrange_EPOS->SetLineColor(38);
        h_NStrange_EPOS->SetLineWidth(2);
        h_NStrange_EPOS->SetLineStyle(2);
        h_NStrange_EPOS->Draw("hist same");
        h_NStrange_AMPT->SetLineColor(46);
        h_NStrange_AMPT->SetLineWidth(2);
        h_NStrange_AMPT->SetLineStyle(2);
        h_NStrange_AMPT->Draw("hist same");
        TLegend *leg = new TLegend(pad1->GetLeftMargin(),           //
                                   1 - pad1->GetTopMargin() + 0.01, //
                                   pad1->GetLeftMargin() + 0.25,    //
                                   0.98);
        leg->AddEntry(h_NPrimaryG4P_HIJING, "HIJING", "l");
        leg->AddEntry(h_NPrimaryG4P_EPOS, "EPOS", "l");
        leg->AddEntry(h_NPrimaryG4P_AMPT, "AMPT", "l");
        leg->AddEntry((TObject *)0, "Solid - Total number of primary particles", "");
        leg->AddEntry((TObject *)0, "Dash - Number of K_{s}^{0} and #Lambda", "");
        leg->SetBorderSize(0);
        leg->Draw();
        pad2->cd();
        h_NStrange_ratio_HIJING->SetLineColor(30);
        h_NStrange_ratio_HIJING->SetLineWidth(1);
        h_NStrange_ratio_HIJING->GetYaxis()->SetTitle("Fraction (%)");
        h_NStrange_ratio_HIJING->GetYaxis()->SetTitleOffset(0.5);
        h_NStrange_ratio_HIJING->GetYaxis()->SetRangeUser(pad2_ymin, pad2_ymax);
        h_NStrange_ratio_HIJING->GetYaxis()->SetNdivisions(505);
        h_NStrange_ratio_HIJING->GetYaxis()->SetTitleSize(AxisTitleSize * textscale);
        h_NStrange_ratio_HIJING->GetYaxis()->SetLabelSize(AxisLabelSize * textscale);
        h_NStrange_ratio_HIJING->GetXaxis()->SetTitleSize(AxisTitleSize * textscale);
        h_NStrange_ratio_HIJING->GetXaxis()->SetLabelSize(AxisLabelSize * textscale);
        h_NStrange_ratio_HIJING->GetXaxis()->SetTitle("Event index");
        h_NStrange_ratio_HIJING->GetXaxis()->SetTitleOffset(1.2);
        h_NStrange_ratio_HIJING->Draw("l");
        h_NStrange_ratio_EPOS->SetLineColor(38);
        h_NStrange_ratio_EPOS->SetLineWidth(1);
        h_NStrange_ratio_EPOS->Draw("l same");
        h_NStrange_ratio_AMPT->SetLineColor(46);
        h_NStrange_ratio_AMPT->SetLineWidth(1);
        h_NStrange_ratio_AMPT->Draw("l same");
        TLine *l_strange_HIJING = new TLine(0, avg_strange_HIJING, v_NStrange_HIJING->size(), avg_strange_HIJING);
        l_strange_HIJING->SetLineColor(30);
        l_strange_HIJING->SetLineStyle(2);
        l_strange_HIJING->Draw();
        TLine *l_strange_EPOS = new TLine(0, avg_strange_EPOS, v_NStrange_EPOS->size(), avg_strange_EPOS);
        l_strange_EPOS->SetLineColor(38);
        l_strange_EPOS->SetLineStyle(2);
        l_strange_EPOS->Draw();
        TLine *l_strange_AMPT = new TLine(0, avg_strange_AMPT, v_NStrange_AMPT->size(), avg_strange_AMPT);
        l_strange_AMPT->SetLineColor(46);
        l_strange_AMPT->SetLineStyle(2);
        l_strange_AMPT->Draw();
        TLatex *l_avg = new TLatex();
        l_avg->SetTextAlign(kHAlignLeft + kVAlignTop);
        l_avg->SetTextSize(AxisLabelSize * textscale * 0.8);
        l_avg->DrawLatex(0.1, pad2_ymax - 0.02, Form("HIJING: %.3g%%, EPOS: %.3g%%, AMPT: %.3g%%", avg_strange_HIJING, avg_strange_EPOS, avg_strange_AMPT));
        c->RedrawAxis();
        c->SaveAs(Form("%s/compStrange.pdf", plotdir.c_str()));
        c->SaveAs(Form("%s/compStrange.png", plotdir.c_str()));
    }

    return std::make_tuple(avg_strange_HIJING, avg_strange_EPOS, avg_strange_AMPT);
}

void compStrange()
{
    system(Form("mkdir -p %s", plotdir.c_str()));

    std::vector<float> avg_strange_HIJING, avg_strange_EPOS, avg_strange_AMPT;
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::min();

    for (int i = 0; i < 5000; i++)
    {
        if (avg_strange_HIJING.size() >= 1000)
        {
            break;
        }

        if (i == 0)
        {
            std::tuple<float, float, float> avg_strange = getStrangeFrac(i, true);

            // if either of the elements is NaN, skip this event
            if (std::isnan(std::get<0>(avg_strange)) || std::isnan(std::get<1>(avg_strange)) || std::isnan(std::get<2>(avg_strange)))
            {
                continue;
            }
            avg_strange_HIJING.push_back(std::get<0>(avg_strange));
            avg_strange_EPOS.push_back(std::get<1>(avg_strange));
            avg_strange_AMPT.push_back(std::get<2>(avg_strange));
        }
        else
        {
            std::tuple<float, float, float> avg_strange = getStrangeFrac(i, false);
            if (std::isnan(std::get<0>(avg_strange)) || std::isnan(std::get<1>(avg_strange)) || std::isnan(std::get<2>(avg_strange)))
            {
                continue;
            }
            avg_strange_HIJING.push_back(std::get<0>(avg_strange));
            avg_strange_EPOS.push_back(std::get<1>(avg_strange));
            avg_strange_AMPT.push_back(std::get<2>(avg_strange));
        }

        minY = std::min({minY, avg_strange_HIJING.back(), avg_strange_EPOS.back(), avg_strange_AMPT.back()});
        maxY = std::max({maxY, avg_strange_HIJING.back(), avg_strange_EPOS.back(), avg_strange_AMPT.back()});

        std::cout << "File idx: " << i << ", HIJING: " << avg_strange_HIJING.back() << ", EPOS: " << avg_strange_EPOS.back() << ", AMPT: " << avg_strange_AMPT.back() << std::endl;
    }

    // create TGraphs for the average strange fraction
    std::vector<float> x_vals(avg_strange_HIJING.size());
    for (size_t i = 0; i < x_vals.size(); ++i)
    {
        x_vals[i] = i;
    }
    TGraph *g_avg_strange_HIJING = new TGraph(avg_strange_HIJING.size(), x_vals.data(), avg_strange_HIJING.data());
    TGraph *g_avg_strange_EPOS = new TGraph(avg_strange_EPOS.size(), x_vals.data(), avg_strange_EPOS.data());
    TGraph *g_avg_strange_AMPT = new TGraph(avg_strange_AMPT.size(), x_vals.data(), avg_strange_AMPT.data());
    TCanvas *c = new TCanvas("c", "c", 800, 600);
    gPad->SetTopMargin(0.17);
    c->cd();
    g_avg_strange_HIJING->SetLineColor(30);
    g_avg_strange_HIJING->SetLineWidth(2);
    g_avg_strange_HIJING->GetYaxis()->SetTitle("Strange fraction (%)");
    g_avg_strange_HIJING->GetYaxis()->SetTitleOffset(1.4);
    g_avg_strange_HIJING->GetYaxis()->SetTitleSize(AxisTitleSize);
    g_avg_strange_HIJING->GetYaxis()->SetLabelSize(AxisLabelSize);
    g_avg_strange_HIJING->GetXaxis()->SetTitle("File index");
    g_avg_strange_HIJING->GetXaxis()->SetTitleSize(AxisTitleSize);
    g_avg_strange_HIJING->GetXaxis()->SetLabelSize(AxisLabelSize);
    g_avg_strange_HIJING->GetYaxis()->SetRangeUser(minY * 0.9, maxY * 1.1);
    g_avg_strange_HIJING->Draw("al");
    g_avg_strange_EPOS->SetLineColor(38);
    g_avg_strange_EPOS->SetLineWidth(2);
    g_avg_strange_EPOS->Draw("l same");
    g_avg_strange_AMPT->SetLineColor(46);
    g_avg_strange_AMPT->SetLineWidth(2);
    g_avg_strange_AMPT->Draw("l same");
    TLegend *leg = new TLegend(gPad->GetLeftMargin(), 1 - gPad->GetTopMargin() + 0.01, gPad->GetLeftMargin() + 0.25, 0.98);
    leg->AddEntry(g_avg_strange_HIJING, Form("HIJING (avg. strange fraction = %.3g)",g_avg_strange_HIJING->GetMean(2)), "l");
    leg->AddEntry(g_avg_strange_EPOS, Form("EPOS (avg. strange fraction = %.3g)",g_avg_strange_EPOS->GetMean(2)), "l");
    leg->AddEntry(g_avg_strange_AMPT, Form("AMPT (avg. strange fraction = %.3g)",g_avg_strange_AMPT->GetMean(2)), "l");
    leg->SetTextSize(0.04);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->Draw();
    c->RedrawAxis();
    c->SaveAs(Form("%s/compStrange_avgfileidx.pdf", plotdir.c_str()));
    c->SaveAs(Form("%s/compStrange_avgfileidx.png", plotdir.c_str()));
}