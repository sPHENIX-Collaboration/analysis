#include "./util_combine.h"

std::vector<float> v_sphenix_centralitybin = {0, 3, 6, 10, 15, 20, 25, 30, 35, 40, 45, 50};
int nsphenix = v_sphenix_centralitybin.size() - 1;
float rho = 2537.15 / (sqrt(4476.51) * sqrt(1974.34));
bool drawall = false;
int cent_low = 20, cent_high = 25;

float LeftMargin = 0.17;
float RightMargin = 0.05;
float TopMargin = 0.08;
float BottomMargin = 0.15;

void ratio_dNdEta()
{
    // gStyle->SetPalette(kLightTemperature);

    std::string plotdir = "./dNdEtaFinal";
    system(Form("mkdir -p %s", plotdir.c_str()));

    std::vector<TH1 *> v_sphenix_dNdeta_ratio; // ratio of the two approaches
    for (int i = 0; i < nsphenix; i++)
    {
        if (!drawall && !(v_sphenix_centralitybin[i] == cent_low && v_sphenix_centralitybin[i + 1] == cent_high))
            continue;

        int Mbin = GetMbinNum(Form("Centrality%dto%d", (int)v_sphenix_centralitybin[i], (int)v_sphenix_centralitybin[i + 1]));

        // CMS-inspired approach
        TFile *f = new TFile(Form("./systematics/Centrality%dto%d_Zvtxm10p0to10p0_noasel/finalhists_systematics_Centrality%dto%d_Zvtxm10p0to10p0_noasel.root", (int)v_sphenix_centralitybin[i], (int)v_sphenix_centralitybin[i + 1], (int)v_sphenix_centralitybin[i], (int)v_sphenix_centralitybin[i + 1]), "READ");
        TH1D *h = (TH1D *)f->Get("hM_final");
        h->SetDirectory(0);
        f->Close();

        // PHOBOS-inspired approach
        TFile *fphobos = new TFile(Form("/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_HIJING_MDC2_ana472_20250307/Run7/EvtVtxZ/FinalResult_10cm_Pol2BkgFit_DeltaPhi0p026/completed/vtxZ_-10_10cm_MBin%d/Final_Mbin%d_00054280/Final_Mbin%d_00054280.root", Mbin, Mbin, Mbin), "READ");
        TH1D *hphobos = (TH1D *)fphobos->Get("h1D_dNdEta_reco");
        hphobos->SetDirectory(0);
        fphobos->Close();

        // ratio
        TH1 *hratio = (TH1 *)h->Clone(Form("hratio_centraliy%dto%d", (int)v_sphenix_centralitybin[i], (int)v_sphenix_centralitybin[i + 1]));
        // loop over bins between |eta|<=1.1
        for (int j = 1; j <= hratio->GetNbinsX(); j++)
        {
            if (fabs(hratio->GetBinCenter(j)) > 1.1)
                continue;

            float dNdeta_cms = h->GetBinContent(j);
            float dNdeta_phobos = hphobos->GetBinContent(j);
            float dNdeta_ratio = dNdeta_cms / dNdeta_phobos;
            // error propagation for the ratio taking into account the correlation between the two approaches
            float dNdeta_cms_err = h->GetBinError(j);
            float dNdeta_phobos_err = hphobos->GetBinError(j);
            float dNdeta_ratio_err = dNdeta_ratio * sqrt(pow(dNdeta_cms_err / dNdeta_cms, 2) + pow(dNdeta_phobos_err / dNdeta_phobos, 2) - 2 * rho * dNdeta_cms_err * dNdeta_phobos_err / (dNdeta_cms * dNdeta_phobos));
            std::cout << "rho = " << rho << std::endl;
            std::cout << "dNdeta_cms = " << dNdeta_cms << " +/- " << dNdeta_cms_err << std::endl;
            std::cout << "dNdeta_phobos = " << dNdeta_phobos << " +/- " << dNdeta_phobos_err << std::endl;
            std::cout << "dNdeta_ratio = " << dNdeta_ratio << " +/- " << dNdeta_ratio_err << std::endl;
            hratio->SetBinContent(j, dNdeta_ratio);
            hratio->SetBinError(j, dNdeta_ratio_err);
        }
        v_sphenix_dNdeta_ratio.push_back(hratio);
    }

    TCanvas *c = new TCanvas("c", "c", 800, 700);
    gPad->SetTopMargin(TopMargin);
    gPad->SetLeftMargin(LeftMargin);
    gPad->SetRightMargin(RightMargin);
    gPad->SetBottomMargin(BottomMargin);
    c->cd();
    for (size_t i = 0; i < v_sphenix_dNdeta_ratio.size(); i++)
    {
        if (i == 0)
        {
            v_sphenix_dNdeta_ratio[i]->GetXaxis()->SetTitle("Pseudorapidity #eta");
            v_sphenix_dNdeta_ratio[i]->GetXaxis()->SetRangeUser(-1.15, 1.15);
            v_sphenix_dNdeta_ratio[i]->GetYaxis()->SetTitle("The closest-match method / The combinatoric method");
            v_sphenix_dNdeta_ratio[i]->GetYaxis()->SetTitleSize(0.035);
            v_sphenix_dNdeta_ratio[i]->GetYaxis()->SetTitleOffset(2.1);
            v_sphenix_dNdeta_ratio[i]->GetYaxis()->SetRangeUser(0.7, 1.3);
            v_sphenix_dNdeta_ratio[i]->GetXaxis()->SetNdivisions(505);
            // v_sphenix_dNdeta_ratio[i]->GetYaxis()->SetNdivisions(505);
            if (drawall)
                v_sphenix_dNdeta_ratio[i]->Draw("PLC PMC");
            else
            {
                v_sphenix_dNdeta_ratio[i]->SetLineColor(kBlack);
                v_sphenix_dNdeta_ratio[i]->SetMarkerColor(kBlack);
                v_sphenix_dNdeta_ratio[i]->Draw("PE");
            }
        }
        else
        {
            if (drawall)
                v_sphenix_dNdeta_ratio[i]->Draw("PLC PMC same");
        }
    }

    c->RedrawAxis();

    TLegend *leg = new TLegend(gPad->GetLeftMargin() + 0.05,    //
                               (drawall) ? 1 - gPad->GetTopMargin() - 0.27 : 1 - gPad->GetTopMargin() - 0.14, //
                               (drawall) ? gPad->GetLeftMargin() + 0.8 : gPad->GetLeftMargin() + 0.3,     //
                               1 - gPad->GetTopMargin() - 0.04  //
    );
    leg->SetHeader("Centrality interval");
    if (drawall)
        leg->SetNColumns(2);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.03);
    if (drawall)
    {
        for (size_t i = 0; i < v_sphenix_dNdeta_ratio.size(); i++)
        {
            leg->AddEntry(v_sphenix_dNdeta_ratio[i], Form("%d-%d%%", (int)v_sphenix_centralitybin[i], (int)v_sphenix_centralitybin[i + 1]), "l");
        }
    }
    else
        leg->AddEntry(v_sphenix_dNdeta_ratio[0], Form("%d-%d%%", cent_low, cent_high), "pl");

    leg->Draw();
    c->SaveAs(Form("%s/ratio_dNdEta.pdf", plotdir.c_str()));
    c->SaveAs(Form("%s/ratio_dNdEta.png", plotdir.c_str()));
}