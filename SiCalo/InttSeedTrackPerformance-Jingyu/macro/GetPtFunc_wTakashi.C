#include <TFile.h>
#include <TTree.h>
#include <TH3D.h>
#include <TCanvas.h>
#include <TVector2.h>
#include <vector>
#include <cmath>

void GetPtFunc_wTakashi(
    // const char* filename="/mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/ML4Reco/dataset/merged_last500kNew.root",
    const char* filename="/mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/ML4Reco/dataset/positron_last500kNew.root",
    // const char* filename="/mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/ParticleGen/output/electron_1M.root",

    // const char* funcfile="/mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/InttSeedTrackPerformance/output/Ptfunc_rough_takashi_e.root",
    const char* funcfile="/mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/InttSeedTrackPerformance/output/Ptfunc_rough_takashi_p.root",

    const char* treename="tree",

    // const char* outputname="/mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/InttSeedTrackPerformance/output/Ptfunc_rough_takashi_p.root"
    const char* outputname="/mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/InttSeedTrackPerformance/output/Ptfunc_fitCofEta_takashi_p.root"
) 
{
    TFile outfile(outputname, "RECREATE");

    TFile *file_in = TFile::Open(filename);
    if (!file_in || file_in->IsZombie()) {
        std::cerr<<"Error: cannot open "<<filename<<std::endl;
        return;
    }

    TTree *tree_data = (TTree*)file_in->Get(treename);
    if (!tree_data) {
        std::cerr<<"Error: cannot find tree "<<treename<<std::endl;
        return;
    }

    TFile* funcFile = TFile::Open(funcfile, "READ");
    if (!funcFile || funcFile->IsZombie()) {
        std::cerr << "Error: cannot open function file " << funcfile << std::endl;
        return;
    }
    TF1* rough_funcTP = nullptr;
    TF1* rough_funcTG = nullptr;
    funcFile->GetObject("poly_func_profile", rough_funcTP);
    funcFile->GetObject("poly_func_graph", rough_funcTG);

    int NClus;
    std::vector <int> *clus_system = nullptr;
    std::vector <int> *clus_layer = nullptr;
    std::vector <int> *clus_adc = nullptr;
    std::vector <double> *clus_X = nullptr;
    std::vector <double> *clus_Y = nullptr;
    std::vector <double> *clus_Z = nullptr;

    std::vector<double> *CEMC_Pr_Hit_x = nullptr; 
    std::vector<double> *CEMC_Pr_Hit_y = nullptr; 
    std::vector<double> *CEMC_Pr_Hit_z = nullptr;
    std::vector<double> *CEMC_Pr_Hit_R = nullptr;

    std::vector<double> *PrimaryG4P_Pt_ = nullptr;
    std::vector<double> *PrimaryG4P_Eta_ = nullptr;

    tree_data->SetBranchAddress("trk_NClus", & NClus);
    tree_data->SetBranchAddress("trk_system", & clus_system);
    tree_data->SetBranchAddress("trk_layer", & clus_layer);
    tree_data->SetBranchAddress("trk_X", & clus_X);
    tree_data->SetBranchAddress("trk_Y", & clus_Y);
    tree_data->SetBranchAddress("trk_Z", & clus_Z);

    tree_data->SetBranchAddress("CEMC_Pr_Hit_x",  &CEMC_Pr_Hit_x);
    tree_data->SetBranchAddress("CEMC_Pr_Hit_y",  &CEMC_Pr_Hit_y);
    tree_data->SetBranchAddress("CEMC_Pr_Hit_z",  &CEMC_Pr_Hit_z);
    tree_data->SetBranchAddress("CEMC_Pr_Hit_R",  &CEMC_Pr_Hit_R);

    tree_data->SetBranchAddress("PrimaryG4P_Pt", &PrimaryG4P_Pt_);
    tree_data->SetBranchAddress("PrimaryG4P_Eta", &PrimaryG4P_Eta_);

    TProfile* profC = new TProfile("C_vs_eta","C(eta);#eta;C(eta) [GeV·rad]", 120, -1.2, 1.2);

    TH2D* h2etac = new TH2D("h2etac","C(eta);#eta;C(eta) [GeV·rad]", 48, -1.2, 1.2, 200, 0, 0.4);

    std::vector<double> eta_vals, C_vals;

    Long64_t nentries = tree_data->GetEntries();
    for (Long64_t i=0; i<nentries; ++i) 
    {
        tree_data->GetEntry(i);

        // 选事件：层4/5 恰有1个、层6/7 恰有1个
        int idx34 = -1, idx56 = -1;
        int cnt34 = 0, cnt56 = 0;
        for (int j=0; j<(int)clus_layer->size(); ++j) {
            int L = (*clus_layer)[j];
            if (L==3 || L==4) { ++cnt34; idx34=j; }
            if (L==5 || L==6) { ++cnt56; idx56=j; }
        }
        if (cnt34!=1 || cnt56!=1) continue;

        // 要求 CEMC 记录为1
        if (CEMC_Pr_Hit_x->size()!=1) continue;

        // 要求 prim particle 为1
        if (PrimaryG4P_Pt_->size()!=1) continue;

        // 取出所有量
        double trk34_X = (*clus_X)[idx34];
        double trk34_Y = (*clus_Y)[idx34];
        double trk56_X = (*clus_X)[idx56];
        double trk56_Y = (*clus_Y)[idx56];
        double cemc_Pr_X  = (*CEMC_Pr_Hit_x)[0];
        double cemc_Pr_Y  = (*CEMC_Pr_Hit_y)[0];
        double Pr_pt = (*PrimaryG4P_Pt_)[0];
        double Pr_eta = (*PrimaryG4P_Eta_)[0];

        double cemc_Pr_R = std::sqrt(cemc_Pr_X * cemc_Pr_X + cemc_Pr_Y * cemc_Pr_Y);
        // if (cemc_Pr_R > 102) continue; // 限制 CEMC 半径范围

        // 计算 Δφ
        double phi1 = std::atan2(trk56_Y - trk34_Y, trk56_X - trk34_X);
        double phi2 = std::atan2(cemc_Pr_Y - trk56_Y, cemc_Pr_X - trk56_X);
        double dphi = TVector2::Phi_mpi_pi(phi2 - phi1);

        // calculate C(η) = 〈pT·Δφ〉
        dphi = abs(dphi); // 确保 Δφ 为正值
        // double Cval = Pr_pt * dphi;
        double Cval = Pr_pt * std::pow(dphi, 0.986);

        double C_rough_tp = rough_funcTP->Eval(Pr_eta);
        double C_rough_tg = rough_funcTG->Eval(Pr_eta);
        const double Cval_max_tp = C_rough_tp + 0.01;  
        const double Cval_min_tp = C_rough_tp - 0.01;  
        const double Cval_max_tg = C_rough_tg + 0.01;  
        const double Cval_min_tg = C_rough_tg - 0.01;  

        if (Cval < Cval_max_tp && Cval > Cval_min_tp) {
            profC->Fill(Pr_eta, Cval); 
            h2etac->Fill(Pr_eta, Cval);
        }
        if (Cval < Cval_max_tg && Cval > Cval_min_tg) {
            eta_vals.push_back(Pr_eta);
            C_vals.push_back(Cval);
        }

        // rough fit function
        // const double Cval_max = 0.26;  
        // const double Cval_min = 0.17; 

        // if (Cval > Cval_max || Cval < Cval_min) {
        //     continue;
        // }

        // profC->Fill(Pr_eta, Cval); 
        // h2etac->Fill(Pr_eta, Cval);

        // eta_vals.push_back(Pr_eta);
        // C_vals.push_back(Cval);
    }

    // 用 TGraph 做无损散点拟合
    int N = eta_vals.size();
    TGraph* gr = new TGraph(N, eta_vals.data(), C_vals.data());
    gr->SetTitle("C(eta);#eta;C=pt*#Delta#phi");

    // Fit TGraph
    // TF1* poly_func_graph = new TF1("poly_func_graph","([0]+[1]*x+[2]*x*x+[3]*x*x*x+[4]*x*x*x*x)", -1.1, 1.1);
    TF1* poly_func_graph = new TF1("poly_func_graph","([0]+[1]*x*x+[2]*x*x*x*x)", -1.1, 1.1);
    gr->Fit(poly_func_graph,"");

    // Fit TProfile
    const double thr = 0.205;
    for (int ib = 1; ib <= profC->GetNbinsX(); ++ib) {
        if (profC->GetBinContent(ib) > thr) {
            profC->SetBinError(ib, 2e-1);
        }
    }
    // TF1* poly_func_profile = new TF1("poly_func_profile","([0]+[1]*x+[2]*x*x+[3]*x*x*x+[4]*x*x*x*x)", -1.1, 1.1);
    TF1* poly_func_profile = new TF1("poly_func_profile","([0]+[1]*x*x+[2]*x*x*x*x)", -1.1, 1.1);
    profC->Fit(poly_func_profile, "R");

    // 画图看效果                          
    TCanvas* c1 = new TCanvas;              
    gr->Draw("AP");       // A=画轴，P=画点
    poly_func_graph->Draw("same");  // 叠加画拟合曲线
    c1->SaveAs("/mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/InttSeedTrackPerformance/output/fitCofEta.pdf");

    TCanvas* c2 = new TCanvas;
    poly_func_profile->SetLineColor(kRed);
    poly_func_profile->SetLineStyle(1);  // 1=实线

    poly_func_graph->SetLineColor(kBlue);
    poly_func_graph->SetLineStyle(2);    // 2=虚线

    // X 轴：η，范围 [-1.2, 1.2]
    profC->GetXaxis()->SetTitle("#eta");
    profC->GetXaxis()->SetLimits(-1.2, 1.2);  
    // Y 轴：C = p_{T} * Δφ，范围 [0.196, 0.206]
    profC->GetYaxis()->SetTitle("C = p_{T} #times #Delta#phi");
    profC->GetYaxis()->SetRangeUser(0.196, 0.206);

    profC->Draw("AP");       // A=画轴，P=画点
    poly_func_profile->Draw("same");  // 叠加画拟合曲线
    poly_func_graph->Draw("same"); // 叠加画拟合曲线
    c2->SaveAs("/mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/InttSeedTrackPerformance/output/TPRfitfuncS.pdf");

    outfile.cd();
    profC->Write();
    h2etac->Write();
    // gr->Write();
    poly_func_profile->Write();
    poly_func_graph->Write();

    file_in->Close();
}


