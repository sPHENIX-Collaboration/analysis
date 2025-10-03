void comparePNcharge() 
{
    // 输入文件路径
    TString file1P_name = "/mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/ParticleGen/output/calo_position_positron.root";
    TString file2N_name = "/mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/ParticleGen/output/calo_position_electron.root";
  
    // 打开输入文件
    TFile *file1_Pcharge = TFile::Open(file1P_name, "READ");
    TFile *file2_Ncharge = TFile::Open(file2N_name, "READ");
  
    if (!file1_Pcharge || file1_Pcharge->IsZombie()) 
    {
      std::cerr << "无法打开 " << file1P_name << std::endl;
      return;
    }
    if (!file2_Ncharge || file2_Ncharge->IsZombie()) 
    {
      std::cerr << "无法打开 " << file2N_name << std::endl;
      return;
    }
  
    // 从两个文件中读取 TH1D
    TH1F *h1_DR_pr1_clustinnr_P = nullptr;
    TH1F *h1_DR_pr1_clustinnr_N = nullptr;
    file1_Pcharge->GetObject("h1_DR_pr1_clustinnr",h1_DR_pr1_clustinnr_P);
    file2_Ncharge->GetObject("h1_DR_pr1_clustinnr",h1_DR_pr1_clustinnr_N);

    // // 从两个文件中读取 TH1D
    // TH1* h_temp = nullptr;
    // file1_Pcharge->GetObject("h1_dphi_distri_pr1_clustinnr", h_temp);

    // 获取两个 TProfile
    TProfile *TP_DR_pr1_clustinnr_pt_P = nullptr;
    TProfile *TP_DR_pr1_clustinnr_pt_N = nullptr;
    file1_Pcharge->GetObject("TP_DR_pr1_clustinnr_pt", TP_DR_pr1_clustinnr_pt_P);
    file2_Ncharge->GetObject("TP_DR_pr1_clustinnr_pt", TP_DR_pr1_clustinnr_pt_N);

    // 准备新 profile，结构与 prof1 相同
    auto TP_DR_pr1_clustinnr_pt_PN  = new TProfile("TP_DR_pr1_clustinnr_pt_PN", "TP_DR_pr1_clustinnr_pt_PN;charge(-e)*pt(GeV);R(cm)", 22, -11, 11, 0, 20);

    // 合并 P charge
    for (int i = 1; i <= TP_DR_pr1_clustinnr_pt_P->GetNbinsX(); ++i) 
    {
        double x = TP_DR_pr1_clustinnr_pt_P->GetBinCenter(i);
        double y = TP_DR_pr1_clustinnr_pt_P->GetBinContent(i);
        double ey = TP_DR_pr1_clustinnr_pt_P->GetBinError(i);
        double n  = TP_DR_pr1_clustinnr_pt_P->GetBinEntries(i);

        if (n <= 0) continue;

        // 反推 RMS²，模拟填充
        double rms2 = ey * ey * n;
        double rms = std::sqrt(rms2 / n); // RMS
    
        // 模拟填入 n 次，带有 RMS 的偏移
        for (int j = 0; j < (int)n; ++j) 
        {
            double y_j = gRandom->Gaus(y, rms);  // 模拟原始测量值
            TP_DR_pr1_clustinnr_pt_PN->Fill(x, y);
        }
    }

    TProfile *flipped = new TProfile("flipped_profile", "Flipped profile", 11, 0, 11, 0, 20);
    // 合并 N charge（反转X轴）
    for (int i = 1; i <= TP_DR_pr1_clustinnr_pt_N->GetNbinsX(); ++i) 
    {
        // 翻转 X
        int flipped_bin = TP_DR_pr1_clustinnr_pt_N->GetNbinsX() - i + 1;

        double x = TP_DR_pr1_clustinnr_pt_N->GetBinCenter(i);
        double flipped_x = -x;
        double y = TP_DR_pr1_clustinnr_pt_N->GetBinContent(i);
        double ey = TP_DR_pr1_clustinnr_pt_N->GetBinError(i);
        double n  = TP_DR_pr1_clustinnr_pt_N->GetBinEntries(i);
       
        if (n <= 0) continue;

        double rms2 = ey * ey * n;
        // double rms = std::sqrt(rms2 / n);
        double rms = ey * ey * n;


        for (int j = 0; j < (int)n; ++j) 
        {
            double y_j = gRandom->Gaus(y, rms);  // 模拟原始测量值
            TP_DR_pr1_clustinnr_pt_PN->Fill(flipped_x, y);
        }
    }
    TP_DR_pr1_clustinnr_pt_PN->SetMarkerStyle(29);


    // 创建画布并画图 ---------------------------------------------------------------------------------------------------
    TCanvas *c1 = new TCanvas("h1_DR_pr1_clustinnr_PN", "P vs N", 800, 600);
    c1->cd();
    
    // 设置样式
    h1_DR_pr1_clustinnr_P->SetLineColor(kRed);
    h1_DR_pr1_clustinnr_P->SetLineWidth(2);
    h1_DR_pr1_clustinnr_P->SetTitle("Comparison of e+ and e-");
    h1_DR_pr1_clustinnr_P->Draw("");

    h1_DR_pr1_clustinnr_N->SetLineColor(kBlue);
    h1_DR_pr1_clustinnr_N->SetLineWidth(2);
    h1_DR_pr1_clustinnr_N->Draw("SAME");

    // 添加图例
    TLegend *leg1 = new TLegend(0.65, 0.75, 0.88, 0.88);
    leg1->AddEntry(h1_DR_pr1_clustinnr_P, "Positron", "l");
    leg1->AddEntry(h1_DR_pr1_clustinnr_N, "Electron", "l");
    leg1->Draw();
  
    // 保存图像
    c1->SaveAs("comparePNcharge_R.pdf");

    TCanvas *c2 = new TCanvas("h1_dphi_distri_pr1_clustinnr_PN", "P vs N", 800, 600);
    c2->cd();
    // // 设置样式
    // h1_dphi_distri_pr1_clustinnr_P->SetLineColor(kRed);
    // h1_dphi_distri_pr1_clustinnr_P->SetLineWidth(2);
    // h1_dphi_distri_pr1_clustinnr_P->SetTitle("Comparison of e+ and e-");
    // h1_dphi_distri_pr1_clustinnr_P->Draw("");

    // h1_dphi_distri_pr1_clustinnr_N->SetLineColor(kBlue);
    // h1_dphi_distri_pr1_clustinnr_N->SetLineWidth(2);
    // h1_dphi_distri_pr1_clustinnr_N->Draw("SAME");

    // // 添加图例
    // TLegend *leg2 = new TLegend(0.65, 0.75, 0.88, 0.88);
    // leg2->AddEntry(h1_dphi_distri_pr1_clustinnr_P, "Positron", "l");
    // leg2->AddEntry(h1_dphi_distri_pr1_clustinnr_N, "Electron", "l");
    // leg2->Draw();
  
    // 保存图像
    c2->SaveAs("comparePNcharge_dphi.pdf");

    // 保存到新的文件
    TFile *output = new TFile("PNchargeComp.root", "RECREATE");
    c1->Write();
    c2->Write();
    TP_DR_pr1_clustinnr_pt_PN->Write();
    output->Close();
  
    std::cout << "完成！新直方图已保存到 output.root 中。" << std::endl;
  
    // 清理
    file1_Pcharge->Close();
    file2_Ncharge->Close();
  }
  