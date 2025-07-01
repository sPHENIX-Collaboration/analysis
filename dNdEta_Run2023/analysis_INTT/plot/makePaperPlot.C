#include "./plotutil.h"

float TickSize = 0.03;
float AxisTitleSize = 0.06;
float AxisLabelSize = 0.05;
float LeftMargin = 0.15;
float RightMargin = 0.05;
float TopMargin = 0.08;
float BottomMargin = 0.13;
float textscale_pad1 = 1.2;
float textscale_pad2 = 2.6;

std::vector<std::string> histfnames = {"./hists/Data_Run54280_20250210_ProdA2024/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0/hists_merged.root", //
                                       "./hists/Sim_HIJING_MDC2_ana472_20250307/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0/hists_merged.root",  //
                                       "./hists/Sim_EPOS_MDC2_ana472_20250310/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0/hists_merged.root",    //
                                       "./hists/Sim_AMPT_MDC2_ana472_20250310/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0/hists_merged.root"};

std::vector<TH1 *> hists;

std::vector<std::string> histlegend = {"Data", "HIJING", "EPOS", "AMPT"};

std::vector<int> histlinestyle = {1, 1, 6, 7};

std::vector<std::string> histcolor = {"#09122C", "#99cc99", "#6699cc", "#f2777a"};

// std::string prelimtext = "Internal";
std::string prelimtext = "";

void drawcompare_ratio(std::vector<TH1 *> hists, //
                       bool logy,                //
                       float ymaxscale,          //
                       std::string xtitle,       //
                       std::string ytitle,       //
                       std::string legtitle,     //
                       std::string plotName      //
)
{
    float ymin = 1E10, ymax = -1E10;
    float ratio_min = 999., ratio_max = -1.;
    // normalize the histogram and take ratio
    std::vector<TH1 *> v_ratio;
    v_ratio.clear();

    for (size_t i = 0; i < hists.size(); ++i)
    {
        if (hists[i])
        {
            hists[i]->Scale(1. / hists[i]->Integral(-1, -1), "width");

            if (hists[i]->GetMinimum(0) < ymin)
                ymin = hists[i]->GetMinimum(0);

            if (hists[i]->GetMaximum() > ymax)
                ymax = hists[i]->GetMaximum();

            hists[i]->SetLineColor(TColor::GetColor(histcolor[i].c_str()));
            hists[i]->SetLineWidth(2);
            hists[i]->SetLineStyle(histlinestyle[i]);

            if (i == 0)
            {
                hists[i]->SetMarkerStyle(20);
                hists[i]->SetMarkerSize(0.8);
            }
            else
            {
                hists[i]->SetMarkerSize(0);
                // calculate ratio to the first histogram
                TH1 *h_ratio = (TH1 *)hists[i]->Clone(Form("h_ratio_%s", histlegend[i].c_str()));
                h_ratio->Divide(hists[0]);
                v_ratio.push_back(h_ratio);
                h_ratio->SetLineColor(TColor::GetColor(histcolor[i].c_str()));
                h_ratio->SetLineWidth(2);
                h_ratio->SetLineStyle(histlinestyle[i]);
                h_ratio->SetMarkerSize(0);

                if (h_ratio->GetMinimum(0) < ratio_min)
                    ratio_min = h_ratio->GetMinimum(0);

                if (h_ratio->GetMaximum() > ratio_max)
                    ratio_max = h_ratio->GetMaximum();
            }
        }
    }

    TCanvas *c = new TCanvas("c", "c", 800, 700);
    TPad *pad1 = new TPad("pad1", "pad1", 0, 0.3, 1, 1);
    pad1->SetBottomMargin(0);
    pad1->SetTopMargin(0.08);
    pad1->SetRightMargin(RightMargin);
    pad1->Draw();
    TPad *pad2 = new TPad("pad2", "pad2", 0, 0, 1, 0.3);
    pad2->SetRightMargin(RightMargin);
    pad2->SetTopMargin(0);
    pad2->SetBottomMargin(0.4);
    pad2->Draw();
    pad1->cd();
    pad1->SetLogy(logy);
    hists[0]->GetYaxis()->SetTitle(ytitle.c_str());
    hists[0]->GetYaxis()->SetTitleOffset(1.);
    hists[0]->GetYaxis()->SetTitleSize(AxisTitleSize * textscale_pad1);
    hists[0]->GetYaxis()->SetLabelSize(AxisLabelSize * textscale_pad1);
    hists[0]->GetYaxis()->SetRangeUser(ymin * 0.7, ymax * ymaxscale);
    // hists[0]->GetYaxis()->SetMoreLogLabels();
    hists[0]->GetXaxis()->SetTitleSize(0);
    hists[0]->GetXaxis()->SetLabelSize(0);
    hists[0]->Draw("PE");
    for (size_t i = 1; i < hists.size(); ++i)
    {
        if (hists[i])
        {
            hists[i]->Draw("hist same");
        }
    }

    TLegend *sphnxleg = new TLegend(1 - pad1->GetRightMargin() - 0.47, //
                                    1 - pad1->GetTopMargin() - 0.23,    //
                                    1 - pad1->GetRightMargin() - 0.1,  //
                                    1 - pad1->GetTopMargin() - 0.07    //
    );
    sphnxleg->SetTextAlign(kHAlignLeft + kVAlignCenter);
    sphnxleg->SetTextSize(0.06);
    sphnxleg->SetFillStyle(0);
    sphnxleg->AddEntry("", Form("#it{#bf{sPHENIX}} %s", prelimtext.c_str()), "");
    sphnxleg->AddEntry("", "Au+Au #sqrt{s_{NN}}=200 GeV", "");
    sphnxleg->Draw();
    TLegend *histleg = new TLegend(1 - pad1->GetRightMargin() - 0.382, //
                                   1 - pad1->GetTopMargin() - 0.47,   //
                                   1 - pad1->GetRightMargin(),        //
                                   1 - pad1->GetTopMargin() - 0.25    //
    );
    histleg->SetHeader(Form("%s", legtitle.c_str()));
    histleg->SetNColumns(2);
    histleg->AddEntry(hists[0], histlegend[0].c_str(), "pe");
    for (size_t i = 1; i < hists.size(); ++i)
    {
        histleg->AddEntry(hists[i], histlegend[i].c_str(), "l");
    }
    histleg->SetBorderSize(0);
    histleg->SetFillStyle(0);
    histleg->SetTextSize(0.055);
    histleg->Draw();

    pad2->cd();
    // pad2->SetLogy(logy);
    for (size_t i = 0; i < v_ratio.size(); ++i)
    {
        if (i == 0)
        {
            v_ratio[i]->GetYaxis()->SetTitle("Model/data");
            v_ratio[i]->GetYaxis()->SetTitleOffset(0.5);
            v_ratio[i]->GetYaxis()->SetTitleSize(AxisTitleSize * textscale_pad2);
            v_ratio[i]->GetYaxis()->SetLabelSize(AxisLabelSize * textscale_pad2);
            v_ratio[i]->GetYaxis()->SetRangeUser(0.501, 1.499);
            v_ratio[i]->GetYaxis()->SetNdivisions(5, 3, 0);
            v_ratio[i]->GetXaxis()->SetTitle(xtitle.c_str());
            v_ratio[i]->GetXaxis()->SetTitleSize(AxisTitleSize * textscale_pad2);
            v_ratio[i]->GetXaxis()->SetLabelSize(AxisLabelSize * textscale_pad2);
            v_ratio[i]->GetXaxis()->SetTitleOffset(1.25);
            v_ratio[i]->Draw("hist l");
        }
        else
        {
            v_ratio[i]->Draw("hist l same");
        }
    }

    TLine *line = new TLine(v_ratio[0]->GetXaxis()->GetXmin(), 1, v_ratio[0]->GetXaxis()->GetXmax(), 1);
    line->SetLineColor(kBlack);
    line->SetLineWidth(2);
    line->SetLineStyle(2);
    line->Draw("same");
    c->RedrawAxis();
    c->SaveAs(Form("%s.pdf", plotName.c_str()));
    c->SaveAs(Form("%s.png", plotName.c_str()));
}

void makePaperPlot()
{
    std::string plotdir = "./PaperPlot";

    for (auto &fname : histfnames)
    {
        TH1 *h = getHist<TH1>(fname, "hM_dR_reco_Centrality0to70_MBDAsymLe0p75_VtxZm20to20");
        if (h)
        {
            hists.push_back(h);
        }
    }

    drawcompare_ratio(hists, true, 2, "Tracklet #DeltaR", "#frac{1}{N} #frac{dN}{d(#DeltaR)}", "Centrality 0-70%", Form("%s/Paperplot_RecotklDeltaR", plotdir.c_str()));
}