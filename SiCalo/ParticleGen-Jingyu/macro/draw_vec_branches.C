void draw_vec_branches(const char* infile = "/mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/ParticleGen/output/electron_PrTT_1GeV.root")
{
    // 打开文件
    TFile *f = TFile::Open(infile, "READ");
    if (!f || f->IsZombie()) {
        std::cerr << "Error opening file: " << infile << std::endl;
        return;
    }

    // 获取 tree
    TTree *tree = dynamic_cast<TTree*>(f->Get("tree"));
    if (!tree) {
        std::cerr << "Cannot find tree 'tree' in file." << std::endl;
        return;
    }

    // 定义变量
    std::vector<float> *_PrG4_TTPRO_dD = nullptr;
    std::vector<float> *_PrG4_TTPRO_dR = nullptr;
    std::vector<float> *_PrG4_TTPRO_dphi = nullptr;

    // 设置 branch address
    tree->SetBranchAddress("PrG4_TTPRO_dD", &_PrG4_TTPRO_dD);
    tree->SetBranchAddress("PrG4_TTPRO_dR", &_PrG4_TTPRO_dR);
    tree->SetBranchAddress("PrG4_TTPRO_dphi", &_PrG4_TTPRO_dphi);

    // 创建直方图
    TH1D *h_dD = new TH1D("h_dD", "PrG4_TTPRO_dD;dD(cm)", 300, 0, 30);
    TH1D *h_dR = new TH1D("h_dR", "PrG4_TTPRO_dR;dR(cm)", 200, -5, 5);
    TH1D *h_dphi = new TH1D("h_dphi", "PrG4_TTPRO_dphi;dphi(rad)", 1000, -1, 1); // dphi 通常是角度范围

    // 创建 2D 直方图：dR vs dphi
    TH2D *h2_dR_dphi = new TH2D("h2_dR_dphi", "dR vs dphi;dR;dphi", 200, -5, 5, 1000, -1, 1);

    // 遍历事件
    Long64_t nentries = tree->GetEntries();
    for (Long64_t i = 0; i < nentries; ++i)
    {
        tree->GetEntry(i);
        
        // 填入直方图
        for (float val : *_PrG4_TTPRO_dD)     h_dD->Fill(val);
        for (float val : *_PrG4_TTPRO_dR)     h_dR->Fill(val);
        for (float val : *_PrG4_TTPRO_dphi)   h_dphi->Fill(val);

        // 填入 2D hist
        size_t nvals = std::min(_PrG4_TTPRO_dR->size(), _PrG4_TTPRO_dphi->size());
        for (size_t j = 0; j < nvals; ++j)
        {
            h2_dR_dphi->Fill(_PrG4_TTPRO_dR->at(j), _PrG4_TTPRO_dphi->at(j));
        }
    }

    // 保存到新文件
    TFile *fout = new TFile("draw_ePrTT_1GeV.root", "RECREATE");
    h_dD->Write();
    h_dR->Write();
    h_dphi->Write();
    h2_dR_dphi->Write();
    fout->Close();

    std::cout << "Histograms saved to output_hist.root" << std::endl;
}
