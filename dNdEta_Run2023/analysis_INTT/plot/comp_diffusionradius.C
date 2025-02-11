#include <TCanvas.h>
#include <TFile.h>
#include <TH2F.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TTree.h>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

float TickSize = 0.03;
float AxisTitleSize = 0.05;
float AxisLabelSize = 0.04;
float LeftMargin = 0.15;
float RightMargin = 0.08;
float TopMargin = 0.08;
float BottomMargin = 0.13;
float textscale = 2.7;

std::string plotdir = "ClusPhiSize";

template <typename T> T *gethist(bool dohadd, std::string histdir, std::string histname)
{
    std::string mergedfile = Form("%s/hists_merged.root", histdir.c_str());
    if (dohadd)
    {
        std::string haddcmd = Form("hadd -f -j 20 %s %s/hists_00*.root", mergedfile.c_str(), histdir.c_str());
        std::cout << haddcmd << std::endl;
        system(haddcmd.c_str());
    }

    TFile *f = new TFile(mergedfile.c_str(), "READ");
    T *h = (T *)f->Get(histname.c_str());
    return h;
}

void draw_comparison(vector<TH1F *> vhist, bool logy, std::string xAxisName, vector<std::string> vleg, std::string plotname)
{
    // colors to cycle through
    // std::vector<int> colors = {kTBriBlue, kTBriCyan, kTBriGreen, kTBriYellow, kTVibOrange, kTBriRed, kTBriPurple};
    gStyle->SetPalette(kLightTemperature);

    float r = vhist[1]->Integral(-1, -1) / vhist[0]->Integral(-1, -1);
    vhist[0]->Sumw2();
    vhist[0]->Scale(1. / vhist[0]->Integral(-1, -1));
    vhist[1]->Sumw2();
    vhist[1]->Scale(r / vhist[1]->Integral(-1, -1)); // normalize relative to default 
    // normalize histograms of simulation to 1
    for (int i = 2; i < vhist.size(); i++)
    {
        vhist[i]->Sumw2();
        vhist[i]->Scale(1. / vhist[i]->Integral(-1, -1));
    }

    float maxbinc = -1, minbinc = 1e9;
    for (int i = 0; i < vhist.size(); i++)
    {
        if (vhist[i]->GetMaximum() > maxbinc)
        {
            maxbinc = vhist[i]->GetMaximum();
        }
        if (vhist[i]->GetMinimum(0) < minbinc)
        {
            minbinc = vhist[i]->GetMinimum(0);
        }
    }

    // two pads: top pad for the histogram, bottom pad for the ratio plot
    TCanvas *c = new TCanvas("c", "c", 800, 700);
    TPad *pad1 = new TPad("pad1", "pad1", 0, 0.3, 1, 1);
    pad1->SetBottomMargin(0);
    pad1->SetRightMargin(RightMargin);
    pad1->Draw();
    TPad *pad2 = new TPad("pad2", "pad2", 0, 0, 1, 0.3);
    pad2->SetRightMargin(RightMargin);
    pad2->SetTopMargin(0);
    pad2->SetBottomMargin(0.31);
    pad2->Draw();
    pad1->cd();
    pad1->SetLogy(logy);
    // vhist[0]->GetXaxis()->SetTitle(xAxisName.c_str());
    vhist[0]->GetXaxis()->SetTitleSize(0);
    vhist[0]->GetXaxis()->SetLabelSize(0);
    // draw overflow bin
    vhist[0]->GetXaxis()->SetRange(1, vhist[0]->GetNbinsX());
    vhist[0]->GetYaxis()->SetTitle("Normalized counts");
    vhist[0]->GetYaxis()->SetTitleSize(AxisTitleSize);
    vhist[0]->GetYaxis()->SetLabelSize(AxisTitleSize);
    float yscale = (logy) ? 750 : 2.0;
    vhist[0]->GetYaxis()->SetRangeUser(minbinc*0.5, maxbinc * yscale);
    vhist[0]->SetLineWidth(2);
    vhist[0]->SetLineColor(kBlack);
    vhist[0]->SetMarkerStyle(20);
    vhist[0]->SetMarkerSize(0.8);
    vhist[0]->SetMarkerColor(kBlack);
    vhist[0]->Draw("PE");
    vhist[1]->SetLineWidth(2);
    vhist[1]->SetLineColor(kGray+2);
    vhist[1]->SetMarkerStyle(21);
    vhist[1]->SetMarkerSize(0.8);
    vhist[1]->SetMarkerColor(kGray+2);
    vhist[1]->Draw("PE same");
    for (int i = 2; i < vhist.size(); i++)
    {
        vhist[i]->SetLineWidth(2);
        // vhist[i]->SetLineColor(colors[i - 1]);
        vhist[i]->Draw("hist PLC same");
    }
    vhist[0]->Draw("PE same");

    TLegend *leg = new TLegend(0.55, 0.81, 0.8, 0.91);
    leg->AddEntry((TObject *)0, "#it{#bf{sPHENIX}} Internal", "");
    leg->AddEntry((TObject *)0, "Au+Au #sqrt{s_{NN}}=200 GeV", "");
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.04);
    leg->Draw();
    float leg_ysize = 0.04 * vleg.size();
    TLegend *leg2 = new TLegend(0.55, 0.8 - leg_ysize, 0.8, 0.80);
    for (int i = 0; i < vleg.size(); i++)
    {
        if (i<2)
        {
            leg2->AddEntry(vhist[i], vleg[i].c_str(), "PE");
        }
        else
        {
            leg2->AddEntry(vhist[i], vleg[i].c_str(), "l");
        }
    }
    leg2->SetBorderSize(0);
    leg2->SetFillStyle(0);
    leg2->SetTextSize(0.035);
    leg2->Draw();

    pad2->cd();
    for (int i = 1; i < vhist.size(); i++)
    {
        TH1F *hratio = (TH1F *)vhist[i]->Clone(Form("hratio%d", i));
        hratio->Divide(vhist[0]);
        hratio->SetLineWidth(2);
        hratio->GetYaxis()->SetRangeUser(0.001, 1.999);
        hratio->GetYaxis()->SetTitle("Data/Sim.");
        hratio->GetYaxis()->SetTitleSize(AxisTitleSize*textscale);
        hratio->GetYaxis()->SetTitleOffset(0.5);
        hratio->GetYaxis()->SetLabelSize(AxisTitleSize*textscale);
        hratio->GetXaxis()->SetTitle(xAxisName.c_str());
        hratio->GetXaxis()->SetTitleSize(AxisTitleSize*textscale);
        hratio->GetXaxis()->SetTitleOffset(1.1);
        hratio->GetXaxis()->SetLabelSize(AxisTitleSize*textscale);
        if (i == 1)
        {
            hratio->GetYaxis()->SetNdivisions(505);
            hratio->GetXaxis()->SetRange(1, hratio->GetNbinsX());
            hratio->Draw("hist L");
        }
        else
        {
            hratio->Draw("hist L PLC same");
        }
    }
    // redraw axis
    pad2->RedrawAxis();
    // draw a line at y=1
    TLine *line = new TLine(vhist[0]->GetXaxis()->GetXmin(), 1, vhist[0]->GetXaxis()->GetXmax(), 1);
    line->SetLineStyle(2);
    line->Draw();

    c->SaveAs(Form("%s.pdf", plotname.c_str()));
    c->SaveAs(Form("%s.png", plotname.c_str()));
}

void comp_diffusionradius()
{
    std::vector<std::string> infiledirs = {
        "/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/hists/Data_Run54280_20241113/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0/",
        "/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/hists/Data_Run54280_20241114_Zclustering/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0/",
        "/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/hists/Sim_HIJING_ana443_20241102/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0/",
        "/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/hists/Sim_HIJING_ana443_20241105_diffusionradius10um/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0/",
        "/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/hists/Sim_HIJING_ana443_20241107_diffusionradius20um/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0/",
        "/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/hists/Sim_HIJING_ana443_20241107_diffusionradius30um/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0/",
        "/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/hists/Sim_HIJING_ana443_20241108_diffusionradius50um/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0/",
        "/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/hists/Sim_HIJING_ana443_20241109_diffusionradius100um/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0/",
    };

    // std::vector<std::string> infiledirs = {
    //     // "/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/hists/Data_CombinedNtuple_Run54280_HotChannel_BCO/Cluster/",
    //     "/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/hists/Data_Run54280_20241113/Cluster/",
    //     "/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/hists/Data_Run54280_20241114_Zclustering/Cluster/",
    //     "/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/hists/Sim_HIJING_ana443_20241102/Cluster/",
    //     // "/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/hists/Sim_HIJING_ana443_20241105_diffusionradius10um/Cluster/",
    //     "/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/hists/Sim_HIJING_ana443_20241107_diffusionradius20um/Cluster/",
    //     // "/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/hists/Sim_Ntuple_HIJING_ana443_20241107_diffusionradius30um/Cluster/",
    //     "/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/hists/Sim_HIJING_ana443_20241108_diffusionradius50um/Cluster/",
    //     "/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/hists/Sim_HIJING_ana443_20241109_diffusionradius100um/Cluster/",
    //     "/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/hists/Sim_HIJING_ana443_20241109_diffusionradius200um/Cluster/",
    //     "/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/hists/Sim_HIJING_ana443_20241114_diffusionradius5um_Zclustering/Cluster/",
    // };

    std::vector<std::string> leg = {
        "Data, Run54280", 
        "Data, Run54280, Z-clustering",
        "HIJING, diffusion radius 5#mum", 
        "HIJING, diffusion radius 10#mum",
        "HIJING, diffusion radius 20#mum",
        "HIJING, diffusion radius 30#mum",
        "HIJING, diffusion radius 50#mum",
        "HIJING, diffusion radius 100#mum",
        // "HIJING, diffusion radius 200#mum",
        // "HIJING, diffusion radius 5#mum, Z-clustering",
    };

    std::vector<std::pair<std::string, std::string>> histvarnames = {
        // {"hM_ClusPhiSize_all", "Cluster #phi size"}, 
        // {"hM_ClusADC_all", "Cluster ADC"},
        // {"hM_ClusPhiSize_all_etale0p1", "Cluster #phi size, |#eta|<0.1"},
        // {"hM_ClusADC_all_etale0p1", "Cluster ADC, |#eta|<0.1"},
        // {"hM_ClusPhiSize_all_etale0p1_ClusZSize1", "Cluster #phi size, |#eta|<0.1, cluster Z size=1"},
        // {"hM_ClusADC_all_etale0p1_ClusZSize1", "Cluster ADC, |#eta|<0.1, cluster Z size=1"}
        // {"hM_ClusZ_all", "Cluster Z [cm]"}
        {"hM_Eta_reco", "Reco-tracklet #eta"},
        {"hM_dPhi_reco_altrange", "Reco-tracklet #Delta#phi"},
        {"hM_dEta_reco_altrange", "Reco-tracklet #Delta#eta"},
        {"hM_cluseta", "Cluster #eta"}
    };

    std::map<std::pair<std::string, std::string>, std::vector<TH1F *>> histmap;
    for (int i = 0; i < infiledirs.size(); i++)
    {
        for (int j = 0; j < histvarnames.size(); j++)
        {
            std::string histname = histvarnames[j].first;
            bool dohadd = (j == 0);
            TH1F *h = gethist<TH1F>(dohadd, infiledirs[i], histname);

            if (histvarnames[j].first == "hM_ClusADC_all" || histvarnames[j].first == "hM_ClusADC_all_etale0p1" || histvarnames[j].first == "hM_ClusADC_all_etale0p1_ClusZSize1")
            {
                h->Rebin(20);
                h->GetXaxis()->SetMaxDigits(3);
                h->GetXaxis()->SetNdivisions(-9);
            }
            histmap[histvarnames[j]].push_back(h);
        }
    }

    for (auto varname : histvarnames)
    {
        if (varname.first == "hM_ClusZ_all" || varname.first == "hM_Eta_reco" || varname.first == "hM_cluseta_zvtxwei")
            draw_comparison(histmap[varname], false, varname.second, leg, Form("%s/%s_compdiffusionradius", plotdir.c_str(), varname.first.c_str()));
        else
            draw_comparison(histmap[varname], true, varname.second, leg, Form("%s/%s_compdiffusionradius", plotdir.c_str(), varname.first.c_str()));
    }
}