#include <iostream>           
#include <string>             
#include <vector>
#include <TChain.h>           
#include <TTree.h>            
#include <TH1F.h>             
#include <TFile.h>            
#include <TMath.h> 
#include <TProfile.h>

void Pt_reso_vspt()
{
    // 输入 ROOT file 名
    // TFile *infile = TFile::Open("/mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/InttSeedTrackPerformance/output/Ptfunc_Performance_positron.root");
    // TFile *infile = TFile::Open("/mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/InttSeedTrackPerformance/output/Ptfunc_Performance_positron_wTa.root");
    // electron
    // TFile *infile = TFile::Open("/mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/InttSeedTrackPerformance/output/Ptfunc_Performance.root");
    // TFile *infile = TFile::Open("/mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/InttSeedTrackPerformance/output/Ptfunc_Performance_electron_wTa.root");
    
    // ML electron
    // TFile *infile = TFile::Open("/mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/ML4Reco/version2/outputFile/pt_relative_error_INTT_CaloI.root");
    // TFile *infile = TFile::Open("/mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/ML4Reco/version4/outputFile/pt_relative_error_INTT_CaloI.root");
    // TFile *infile = TFile::Open("/mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/ML4Reco/combine2/outputFile/pt_relative_error_combined.root");
    TFile *infile = TFile::Open("/mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/ML4Reco/combine2_gate/outputFile/pt_relative_error_combined.root");

    // 获取 TH2D
    // TH2D *h2 = (TH2D *)infile->Get("h2_ptreso");
    // TH2D *h2 = (TH2D *)infile->Get("h2_pt_vs_relerr_all");
    // TH2D *h2 = (TH2D *)infile->Get("h2_pt_vs_relerr_combined");
    TH2D *h2 = (TH2D *)infile->Get("h2_pt_vs_relerr_0.0_10.0");
    if (!h2)
    {
        std::cerr << "Error: Cannot find h2_ptreso!" << std::endl;
        infile->Close();
        return;
    }

    // 输出 ROOT file
    // TFile *outfile = new TFile("../output/projected_p_ptreso_TA.root", "RECREATE");
    // TFile *outfile = new TFile("../output/projected_e_ptreso.root", "RECREATE");

    // TFile *outfile = new TFile("../output/ML/projected_e_ptreso_ML_E_woDropout.root", "RECREATE");
    // TFile *outfile = new TFile("../output/ML/projected_e_ptreso_ML_Phi.root", "RECREATE");
    TFile *outfile = new TFile("../output/ML/projected_e_ptreso_ML_Combined.root", "RECREATE");

    // 准备循环
    int nxbins = h2->GetNbinsX();
    int group_size = 1;
    int n_groups = 100 / group_size;
    if (nxbins % group_size != 0) n_groups += 1;  // 处理非整除情况

    // 准备 TGraphErrors 存 mean 和 sigma
    std::vector<double> vec_pt_center;
    std::vector<double> vec_mean, vec_mean_err;
    std::vector<double> vec_sigma, vec_sigma_err;

    for (int i = 5; i < n_groups; ++i)
    {
        int first_bin = i * group_size + 1; // bin 从 1 开始
        int last_bin = std::min((i + 1) * group_size, nxbins);

        // 投影
        TString proj_name = Form("proj_y_binGroup%d", i);
        TString proj_title = Form("Phi_truth - Phi_reco %d to %d GeV", i, i+1);
        TH1D *proj = h2->ProjectionY(proj_name, first_bin, last_bin);
        proj->SetTitle(proj_title);

        // 寻峰
        int peak_bin = proj->GetMaximumBin();
        double peak_center = proj->GetBinCenter(peak_bin);

        // 找出 content 最高的 3 个 bin，并计算它们的 bin center 平均值
        int nBins = proj->GetNbinsX();
        std::vector<std::pair<double,int>> bins;
        bins.reserve(nBins);

        for (int i = 1; i <= nBins; ++i) {
            bins.emplace_back(proj->GetBinContent(i), i);
        }

        // 按 content 从大到小排序
        std::sort(bins.begin(), bins.end(),
                  [](auto &a, auto &b){ return a.first > b.first; });

        // 取前 3 个峰，累加它们的 bin center
        int nPeaks = 3;
        double sumCenters = 0;
        for (int i = 0; i < nPeaks && i < (int)bins.size(); ++i) {
            int bin = bins[i].second;
            sumCenters += proj->GetBinCenter(bin);
        }

        // 计算平均 peak center
        double peak_center_avg = sumCenters / nPeaks;

        // 拟合
        // TF1 *fit_func = new TF1(Form("gaus_fit_%d", i), "gaus", peak_center - 0.10, peak_center + 0.10);
        TF1 *fit_func = new TF1(Form("gaus_fit_%d", i), "gaus", peak_center_avg - 0.10, peak_center_avg + 0.10);
        proj->Fit(fit_func, "RQ");

        // 取拟合参数
        double fit_mean = fit_func->GetParameter(1);
        double fit_mean_err = fit_func->GetParError(1);
        double fit_sigma = fit_func->GetParameter(2);
        double fit_sigma_err = fit_func->GetParError(2);

        // 获取 pt bin 中心
        double pt_low = h2->GetXaxis()->GetBinLowEdge(first_bin);
        double pt_high = h2->GetXaxis()->GetBinUpEdge(last_bin);
        double pt_center = 0.5 * (pt_low + pt_high);

        // 存入 vector
        vec_pt_center.push_back(pt_center);
        vec_mean.push_back(fit_mean);
        vec_mean_err.push_back(fit_mean_err);
        vec_sigma.push_back(fit_sigma);
        vec_sigma_err.push_back(fit_sigma_err);

        // 保存 proj 和拟合
        proj->Write();
        // fit_func->Write();
    }

    // 生成 TGraphErrors
    int npoints = vec_pt_center.size();
    TGraphErrors *gr_mean = new TGraphErrors(npoints);
    TGraphErrors *gr_sigma = new TGraphErrors(npoints);

    for (int i = 0; i < npoints; ++i)
    {
        gr_mean->SetPoint(i, vec_pt_center[i], vec_mean[i]);
        gr_mean->SetPointError(i, 0.0, vec_mean_err[i]);

        gr_sigma->SetPoint(i, vec_pt_center[i], vec_sigma[i]);
        gr_sigma->SetPointError(i, 0.0, vec_sigma_err[i]);
    }

    gr_mean->SetTitle("Peak value vs Pt;Pt (GeV);Peak Mean");
    gr_mean->SetMarkerStyle(20);
    gr_mean->SetMarkerColor(kBlue);

    gr_sigma->SetMinimum(0.0);
    gr_sigma->SetMaximum(0.2);
    gr_sigma->SetTitle("Resolution (Sigma) vs Pt;Pt (GeV);Sigma");
    gr_sigma->SetMarkerStyle(21);
    gr_sigma->SetMarkerColor(kRed);

    // 保存 TGraphErrors
    gr_mean->Write("gr_mean_vs_pt");
    gr_sigma->Write("gr_sigma_vs_pt");

    // 可以画出来
    TCanvas *c1 = new TCanvas("c1", "Peak Mean vs Pt", 800, 600);
    gr_mean->Draw("AP");
    c1->SaveAs("../output/mean_vs_pt.pdf");

    TCanvas *c2 = new TCanvas("c2", "Sigma vs Pt", 800, 600);
    gr_sigma->Draw("AP");
    c2->SaveAs("../output/sigma_vs_pt.pdf");

    // 关闭文件
    outfile->Close();
    infile->Close();

    std::cout << "Done. Projected TH1D saved to root file" << std::endl;
}
