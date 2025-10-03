#include <iostream>           
#include <string>             
#include <vector>
#include <cmath> 
#include <TChain.h>           
#include <TTree.h>            
#include <TH1F.h>             
#include <TFile.h>            
#include <TMath.h> 
#include <TProfile.h>

double Cal_Distance_R(const double xyz_1[3], const double xyz_2[3]);
double Cal_dR(const double xyz_1[3], const double xyz_2[3]);
double Cal_dphiR(const double xyz_1[3], const double xyz_2[3]);
TGraph* Cor_var_Z(TH2D* h2, double xrange_min = -0.05, double xrange_max = 0.05);
Double_t crystalball(Double_t* x, Double_t* par);

TGraph* FitPeakVsX(TH2D* h2_XY, TF1* projectionFit = nullptr, TF1* peakVsXFit = nullptr, const char* graphName = "grPeakVsX");


void CaloInfo()
{
    // 工作目录
    std::string fDir = "/mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/ParticleGen/output";

    TFile* fin2M = new TFile("/mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/ParticleGen/output/Var_Z_electron.root", "READ");
    TGraph* g_dphiM_z = (TGraph*)fin2M->Get("g_cor_z_dphi_distri_test");                
    TGraph* g_dRadM_z = (TGraph*)fin2M->Get("g_cor_z_dRad_distri_test");                

    TFile* file_C = new TFile("/mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/ParticleGen/output/calo_positron_dphi_ptbin_woC.root", "READ");
    // TFile* file_C = new TFile("/mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/ParticleGen/output/calo_electron_dphi_ptbin_woC.root", "READ");
    TF1* f1_dphi_C = (TF1*)file_C->Get("fitCurve");
    TGraph* g1_dphi_C = (TGraph*)file_C->Get("grPeakVsX");                

    // 输入文件
    TChain *tc = new TChain("tree");                                      
    // std::string fInputName = fDir + "/ana457_Electron_5GeV_10k.root";  
    // std::string fInputName = fDir + "/ana457_Positron_5GeV_10k.root";  
    // std::string fInputName = "/mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/ML4Reco/dataset/positron_last500kNew.root";
    // std::string fInputName = "/mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/ML4Reco/dataset/merged_last500kNew.root";
    std::string fInputName = "/mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/ParticleGen/output/electron_1M.root";

    tc->Add(fInputName.c_str());                                            

    // 输出文件
    // std::string fOutputName = fDir + "/Var_Z_electron.root";  
    // std::string fOutputName = fDir + "/calo_positron_dphi_ptbin_wC.root";  
    std::string fOutputName = fDir + "/calo_electron_dphi_ptbin_woC.root";  

    TTree *caloinfotree = (TTree*)tc;
    if (!caloinfotree)
    {
        std::cerr << "Error: Input tree is null!" << std::endl;
        return;
    }

    // 定义变量
    std::vector<double> *CEMC_Pr_Hit_x = nullptr; 
    std::vector<double> *CEMC_Pr_Hit_y = nullptr; 
    std::vector<double> *CEMC_Pr_Hit_z = nullptr;
    std::vector<double> *CEMC_Pr_Hit_R = nullptr;

    std::vector<double> *CEMC_Hit_x = nullptr; 
    std::vector<double> *CEMC_Hit_y = nullptr; 
    std::vector<double> *CEMC_Hit_z = nullptr;
    std::vector<double> *CEMC_Hit_R = nullptr;
    std::vector<double> *CEMC_Hit_Edep = nullptr;

    std::vector<double> *tower_innr_x = nullptr;
    std::vector<double> *tower_innr_y = nullptr;
    std::vector<double> *tower_innr_z = nullptr;
    std::vector<double> *tower_innr_R = nullptr;
    std::vector<double> *tower_innr_phi = nullptr;
    std::vector<double> *tower_innr_eta = nullptr;
    std::vector<double> *tower_innr_edep = nullptr;

    std::vector<double> *tower_system = nullptr;
    std::vector<double> *tower_x = nullptr;
    std::vector<double> *tower_y = nullptr;
    std::vector<double> *tower_z = nullptr;
    std::vector<double> *tower_R = nullptr;
    std::vector<double> *tower_phi = nullptr;
    std::vector<double> *tower_eta = nullptr;
    std::vector<double> *tower_edep = nullptr;

    std::vector<double> *caloClus_innr_x  = nullptr; 
    std::vector<double> *caloClus_innr_y  = nullptr;
    std::vector<double> *caloClus_innr_z  = nullptr;
    std::vector<double> *caloClus_innr_R  = nullptr;
    std::vector<double> *caloClus_innr_phi  = nullptr;
    std::vector<double> *caloClus_innr_edep = nullptr;

    std::vector<double> *caloClus_system = nullptr;
    std::vector<double> *caloClus_x  = nullptr; 
    std::vector<double> *caloClus_y  = nullptr;
    std::vector<double> *caloClus_z  = nullptr;
    std::vector<double> *caloClus_R  = nullptr;
    std::vector<double> *caloClus_phi  = nullptr;
    std::vector<double> *caloClus_edep = nullptr;

    std::vector<double> *PrimaryG4P_Pt = nullptr;
    std::vector<double> *PrimaryG4P_E  = nullptr;

    // 定义直方图
    auto h_g4hitphi = new TH1F("h_g4hitphi", "Azimuthal angle;Phi (rad);Counts", 100, -TMath::Pi(), TMath::Pi());
    auto h_towerphi = new TH1F("h_towerphi", "Azimuthal angle;Phi (rad);Counts", 100, -TMath::Pi(), TMath::Pi());
    auto h_clustphi = new TH1F("h_clustphi", "Azimuthal angle;Phi (rad);Counts", 100, -TMath::Pi(), TMath::Pi());
    auto h_innr_towerphi = new TH1F("h_innr_towerphi", "Azimuthal angle;Phi (rad);Counts", 100, -TMath::Pi(), TMath::Pi());
    auto h_innr_clustphi = new TH1F("h_innr_clustphi", "Azimuthal angle;Phi (rad);Counts", 100, -TMath::Pi(), TMath::Pi());

    auto h1_DR_pr1_clustinnr   = new TH1F("h1_DR_pr1_clustinnr", "h_DR_pr1_clustinnr;R(cm);Counts", 100, 0, 5);
    auto h1_DR_pr1_towerinnr   = new TH1F("h1_DR_pr1_towerinnr", "h_DR_pr1_towerinnr;R(cm);Counts", 100, 0, 20);
    auto h1_DR_g4hit_clustgeom = new TH1F("h1_DR_g4hit_clustgeom", "h_DR_g4hit_clustgeom;R(cm);Counts", 200, -20, 20);
    auto h1_DR_g4hit_towergeom = new TH1F("h1_DR_g4hit_towergeom", "h_DR_g4hit_towergeom;R(cm);Counts", 100, 0, 20);

    auto h1_dRadius_pr1_clustinnr  = new TH1F("h1_dRadius_pr1_clustinnr", "h1_dRadius_pr1_clustinnr;R(cm);Counts", 50, -2, 2);
    auto h1_dRadius_g4hit_clustgeom= new TH1F("h1_dRadius_g4hit_clustgeom", "h1_dRadius_g4hit_clustgeom;R(cm);Counts", 50, -20, 20);
    auto h1_dphiR_pr1_clustinnr    = new TH1F("h1_dphiR_pr1_clustinnr", "h1_dphiR_pr1_clustinnr;R(cm);Counts", 50, -2, 2);
    auto h1_dphiR_g4hit_clustgeom  = new TH1F("h1_dphiR_g4hit_clustgeom", "h1_dphiR_g4hit_clustgeom;R(cm);Counts", 50, -20, 20);

    auto h2_DR_pr1_clustinnr_pt   = new TH2F("h2_DR_pr1_clustinnr_pt", "h2_DR_pr1_clustinnr_pt;pt(GeV);R(cm);Counts", 11, -0.5, 10.5, 100, 0, 20);
    auto h2_DR_g4hit_clustgeom_pt = new TH2F("h2_DR_g4hit_clustgeom_pt", "h2_DR_g4hit_clustgeom_pt;pt(GeV);R(cm);Counts", 11, -0.5, 10.5, 100, 0, 20);
    auto h2_DR_pr1_clustinnr_E    = new TH2F("h2_DR_pr1_clustinnr_E", "h2_DR_pr1_clustinnr_E;E(GeV);R(cm);Counts", 15, -0.5, 14.5, 100, 0, 20);
    auto h2_DR_g4hit_clustgeom_E  = new TH2F("h2_DR_g4hit_clustgeom_E", "h2_DR_g4hit_clustgeom_E;E(GeV);R(cm);Counts", 15, -0.5, 14.5, 100, 0, 20);

    auto h2_zr_g4hit = new TH2D("h2_zr_g4hit", "g4hit energy-weighted RZ distribution;Z (cm);R (cm);Weighted Counts", 100, -150, 150, 200, 0, 200);
    auto h2_zr_tower = new TH2D("h2_zr_tower", "tower energy-weighted RZ distribution;Z (cm);R (cm);Weighted Counts", 100, -150, 150, 200, 0, 200);
    auto h2_zr_clust = new TH2D("h2_zr_clust", "clust energy-weighted RZ distribution;Z (cm);R (cm);Weighted Counts", 100, -150, 150, 200, 0, 200);
    auto h2_zr_tower_innr = new TH2D("h2_zr_tower_innr", "tower energy-weighted RZ distribution;Z (cm);R (cm);Weighted Counts", 100, -150, 150, 200, 0, 200);
    auto h2_zr_clust_innr = new TH2D("h2_zr_clust_innr", "clust energy-weighted RZ distribution;Z (cm);R (cm);Weighted Counts", 100, -150, 150, 200, 0, 200);

    auto h2_corr_dR_dphiR = new TH2D("h2_corr_dR_dphiR", "h2_corr_dR_dphiR; dR; dphiR; Weighted Counts", 30, -150, 150, 100, -5, 5);

    auto h2_weird_XY = new TH2D("h2_weird_XY", "h2_weird; X; Y; Weighted Counts", 300, -150, 150, 300, -150, 150);
    auto h2_weird_pt_E = new TH2D("h2_weird_pt_E", "h2_weird_pt_E; pt(GeV); E(GeV); Weighted Counts", 10, 0, 10, 20, 0, 20);
    auto h1_weird_pt = new TH1D("h1_weird_pt", "h1_weird_pt; pt(GeV); Weighted Counts", 50, 0, 10);
    auto h1_weird_E  = new TH1D("h1_weird_E", "h1_weird_E; E(GeV); Weighted Counts", 100, 0, 20);

    // delta phi distribution
    auto h1_dphi_distri_pr1_clustinnr = new TH1D(" h1_dphi_distri_pr1_clustinnr", "h1_dphi_distri_pr1_clustinnr; dphi; Counts", 50, -0.02, 0.02);
    auto h1_dphi_distri_pr1_clustgeom = new TH1D(" h1_dphi_distri_pr1_clustgeom", "h1_dphi_distri_pr1_clustgeom; dphi; Counts", 50, -0.02, 0.02);
    auto h1_dphi_distri_g4h_clustinnr = new TH1D(" h1_dphi_distri_g4h_clustinnr", "h1_dphi_distri_g4h_clustinnr; dphi; Counts", 50, -0.02, 0.02);
    auto h1_dphi_distri_g4h_clustgeom = new TH1D(" h1_dphi_distri_g4h_clustgeom", "h1_dphi_distri_g4h_clustgeom; dphi; Counts", 50, -0.02, 0.02);

    auto h2_dphi_distri_test = new TH2D("h2_dphi_distri_test", "h2_dphi_distri_test; pt(GeV); dphi", 55, 0, 11, 40, -0.02, 0.02);
    auto h2_dphi_Pr_innr_Etruth = new TH2D("h2_dphi_Pr_innr_Etruth", "h2_dphi_Pr_innr_Etruth; E(GeV); dphi", 100, 0, 20, 60, -0.03, 0.03);
    auto h2_dphi_Pr_geom_Etruth = new TH2D("h2_dphi_Pr_geom_Etruth", "h2_dphi_Pr_geom_Etruth; E(GeV); dphi", 100, 0, 20, 60, -0.03, 0.03);
    auto h2_dphi_Pr_innr_Ereco = new TH2D("h2_dphi_Pr_innr_Ereco", "h2_dphi_Pr_innr_Ereco; E(GeV); dphi", 100, 0, 20, 60, -0.03, 0.03);
    auto h2_dphi_Pr_geom_Ereco = new TH2D("h2_dphi_Pr_geom_Ereco", "h2_dphi_Pr_geom_Ereco; E(GeV); dphi", 100, 0, 20, 60, -0.03, 0.03);

    auto h2_dphi_Pr_innr_eta = new TH2D("h2_dphi_Pr_innr_eta", "h2_dphi_Pr_innr_eta; #eta; dphi", 120, -1.2, 1.2, 60, -0.03, 0.03);
    auto h2_dphi_Pr_geom_eta = new TH2D("h2_dphi_Pr_geom_eta", "h2_dphi_Pr_geom_eta; #eta; dphi", 120, -1.2, 1.2, 60, -0.03, 0.03);

    // delta R distribution
    auto h2_dRadius_Pr_innr_Ereco = new TH2D("h2_dRadius_Pr_innr_Ereco", "h2_dRadius_Pr_innr_Ereco; E(GeV); dRadius", 100, 0, 20, 200, -10, 10);
    auto h2_dRadius_Pr_geom_Ereco = new TH2D("h2_dRadius_Pr_geom_Ereco", "h2_dRadius_Pr_geom_Ereco; E(GeV); dRadius", 100, 0, 20, 200, -15, 5);

    // TProfile setting
    auto TP_DR_pr1_clustinnr_pt   = new TProfile("TP_DR_pr1_clustinnr_pt", "TP_DR_pr1_clustinnr_pt;pt(GeV);R(cm)", 11, 0, 11, 0, 20);
    auto TP_DR_g4hit_clustgeom_pt = new TProfile("TP_DR_g4hit_clustgeom_pt", "TP_DR_g4hit_clustgeom_pt;pt(GeV);R(cm)", 11, -0.5, 10.5, 0, 20);
    auto TP_DR_pr1_clustinnr_E    = new TProfile("TP_DR_pr1_clustinnr_E", "TP_DR_pr1_clustinnr_E;E(GeV);R(cm)", 15, -0.5, 14.5, 0, 20);
    auto TP_DR_g4hit_clustgeom_E  = new TProfile("TP_DR_g4hit_clustgeom_E", "TP_DR_g4hit_clustgeom_E;E(GeV);R(cm)", 15, -0.5, 14.5, 0, 20);

    // check eta/z dependence
    auto h2_z_dphi_distri_test = new TH2D("h2_z_dphi_distri_test", "h2_z_dphi_distri_test; Z(cm); dphi(rad)",30, -150, 150, 40, -0.02, 0.02);
    auto h2_z_dRad_distri_test = new TH2D("h2_z_dRad_distri_test", "h2_z_dRad_distri_test; Z(cm); dRad(cm)", 30, -150, 150, 50, -5, 5);
    auto h2_z_dDis_distri_test = new TH2D("h2_z_dDis_distri_test", "h2_z_dDis_distri_test; Z(cm); dDis(cm)", 30, -150, 150, 50, -5, 5);

    TGraph* g_cor_z_dphi_distri_test = nullptr;
    TGraph* g_cor_z_dRad_distri_test = nullptr;
    TGraph* g_cor_z_dDis_distri_test = nullptr;

    // 定义 TGraph
    TGraph *g_pr1hit = new TGraph();
    g_pr1hit->SetMarkerStyle(20);  // 实心圆点
    g_pr1hit->SetMarkerColor(kRed); // 红色
    g_pr1hit->SetMarkerSize(0.5);  // 设置红色点大小

    TGraph *g_clustinnr = new TGraph();
    g_clustinnr->SetMarkerStyle(21);  // 实心方点
    g_clustinnr->SetMarkerColor(kBlue); // 蓝色
    g_clustinnr->SetMarkerSize(0.5);  // 设置蓝色点大小

    int pointIndex = 0;

    // 设置分支地址
    caloinfotree->SetBranchAddress("CEMC_Pr_Hit_x", &CEMC_Pr_Hit_x);
    caloinfotree->SetBranchAddress("CEMC_Pr_Hit_y", &CEMC_Pr_Hit_y);
    caloinfotree->SetBranchAddress("CEMC_Pr_Hit_z", &CEMC_Pr_Hit_z);
    caloinfotree->SetBranchAddress("CEMC_Pr_Hit_R", &CEMC_Pr_Hit_R);

    caloinfotree->SetBranchAddress("CEMC_Hit_x", &CEMC_Hit_x);
    caloinfotree->SetBranchAddress("CEMC_Hit_y", &CEMC_Hit_y);
    caloinfotree->SetBranchAddress("CEMC_Hit_z", &CEMC_Hit_z);
    // caloinfotree->SetBranchAddress("CEMC_Hit_R", &CEMC_Hit_R);
    caloinfotree->SetBranchAddress("CEMC_Hit_Edep", &CEMC_Hit_Edep);

    caloinfotree->SetBranchAddress("tower_system", &tower_system);
    caloinfotree->SetBranchAddress("tower_X", &tower_x);
    caloinfotree->SetBranchAddress("tower_Y", &tower_y);
    caloinfotree->SetBranchAddress("tower_Z", &tower_z);
    caloinfotree->SetBranchAddress("tower_R", &tower_R);
    caloinfotree->SetBranchAddress("tower_Phi", &tower_phi);
    caloinfotree->SetBranchAddress("tower_Eta", &tower_eta);
    caloinfotree->SetBranchAddress("tower_edep", &tower_edep);

    caloinfotree->SetBranchAddress("tower_int_X", &tower_innr_x);
    caloinfotree->SetBranchAddress("tower_int_Y", &tower_innr_y);
    caloinfotree->SetBranchAddress("tower_int_Z", &tower_innr_z);
    caloinfotree->SetBranchAddress("tower_int_R", &tower_innr_R);
    // caloinfotree->SetBranchAddress("tower_int_Phi", &tower_innr_phi);
    // caloinfotree->SetBranchAddress("tower_int_Eta", &tower_innr_eta);
    // caloinfotree->SetBranchAddress("tower_int_edep", &tower_innr_edep);

    caloinfotree->SetBranchAddress("caloClus_system", &caloClus_system);
    caloinfotree->SetBranchAddress("caloClus_X", &caloClus_x);
    caloinfotree->SetBranchAddress("caloClus_Y", &caloClus_y);
    caloinfotree->SetBranchAddress("caloClus_Z", &caloClus_z);
    caloinfotree->SetBranchAddress("caloClus_R", &caloClus_R);
    caloinfotree->SetBranchAddress("caloClus_Phi", &caloClus_phi);
    caloinfotree->SetBranchAddress("caloClus_edep", &caloClus_edep); 

    caloinfotree->SetBranchAddress("caloClus_innr_X", &caloClus_innr_x);
    caloinfotree->SetBranchAddress("caloClus_innr_Y", &caloClus_innr_y);
    caloinfotree->SetBranchAddress("caloClus_innr_Z", &caloClus_innr_z);
    caloinfotree->SetBranchAddress("caloClus_innr_R", &caloClus_innr_R);
    caloinfotree->SetBranchAddress("caloClus_innr_Phi", &caloClus_innr_phi);
    caloinfotree->SetBranchAddress("caloClus_innr_edep", &caloClus_innr_edep);

    // moment pt and energy E
    caloinfotree->SetBranchAddress("PrimaryG4P_Pt", &PrimaryG4P_Pt);
    caloinfotree->SetBranchAddress("PrimaryG4P_E", &PrimaryG4P_E);

    int num_of_clus = 0;

    // 遍历树的条目
    Long64_t nentries = caloinfotree->GetEntries();
    // for (Long64_t i = 0; i < nentries; i++)
    for (Long64_t i = 0; i < nentries; i++)
    {
        caloinfotree->GetEntry(i);

        num_of_clus = 0; 

        // CEMC_Pr_Hit ---------------------------------------------------------------------------------------
        if(CEMC_Pr_Hit_x->size() != 1)
        {
            // std::cerr << "Error: CEMC_Pr_Hit_x size is: "<< CEMC_Pr_Hit_x->size() << std::endl;
            continue;
        }
        double PrHit_x = CEMC_Pr_Hit_x->at(0);
        double PrHit_y = CEMC_Pr_Hit_y->at(0);
        double PrHit_z = CEMC_Pr_Hit_z->at(0);
        double PrHit_R = CEMC_Pr_Hit_R->at(0);
        h2_zr_g4hit->Fill(PrHit_z, PrHit_R);
        double pr1cemc_xyz[3] = {PrHit_x, PrHit_y, PrHit_z};
        double PrHit_phi = TMath::ATan2(PrHit_y, PrHit_x);
        if (PrHit_phi < 0)
        {
            PrHit_phi += 2 * TMath::Pi();
        }

        // 遍历 CEMC_Hit vector 中的每个元素 --------------------------------------------------------------------
        Double_t g4hit_TotEMCalE = 0.;
        Double_t g4hit_ModifEMCal_x = 0.;
        Double_t g4hit_ModifEMCal_y = 0.;
        Double_t g4hit_ModifEMCal_z = 0.;

        for (size_t j = 0; j < CEMC_Hit_x->size(); j++)
        {            
            double hitval_x = CEMC_Hit_x->at(j);
            double hitval_y = CEMC_Hit_y->at(j);
            double hitval_z = CEMC_Hit_z->at(j);
            double hitval_R = TMath::Sqrt(hitval_x * hitval_x + hitval_y * hitval_y);
            double hitval_phi = TMath::ATan2(hitval_y, hitval_x);
            double hitval_eta = TMath::ATan2(hitval_R, hitval_z);
            double hitval_edep = CEMC_Hit_Edep->at(j);

            if(hitval_R<200)
            {
                g4hit_TotEMCalE += hitval_edep;

                g4hit_ModifEMCal_x += hitval_edep * hitval_x;
                g4hit_ModifEMCal_y += hitval_edep * hitval_y;
                g4hit_ModifEMCal_z += hitval_edep * hitval_z;

                // h2_zr_g4hit->Fill(hitval_z, hitval_R, hitval_edep);
            }
        }
        g4hit_ModifEMCal_x /= g4hit_TotEMCalE;
        g4hit_ModifEMCal_y /= g4hit_TotEMCalE;
        g4hit_ModifEMCal_z /= g4hit_TotEMCalE;
        double g4hit_xyz[3] = {g4hit_ModifEMCal_x, g4hit_ModifEMCal_y, g4hit_ModifEMCal_z};

        double g4hit_ModifEMCal_phi = TMath::ATan2(g4hit_ModifEMCal_y, g4hit_ModifEMCal_x);
        h_g4hitphi->Fill(g4hit_ModifEMCal_phi);
        if (g4hit_ModifEMCal_phi < 0)
        {
            g4hit_ModifEMCal_phi += 2 * TMath::Pi();
        }

        // 遍历 tower vector 中的每个元素 -----------------------------------------------------------------------------------
        Double_t tower_TotEMCalE = 0.;
        Double_t tower_ModifEMCal_x = 0.;
        Double_t tower_ModifEMCal_y = 0.;
        Double_t tower_ModifEMCal_z = 0.;

        Double_t tower_innr_ModifEMCal_x = 0.;
        Double_t tower_innr_ModifEMCal_y = 0.;
        Double_t tower_innr_ModifEMCal_z = 0.;
        int j_innr = 0; 

        for (size_t j = 0; j < tower_x->size(); j++)
        {    
            if ((tower_system->at(j)) != 0) 
            {
                j_innr = j_innr - 1;
                continue;
            }
            double towerval_x = tower_x->at(j);
            double towerval_y = tower_y->at(j);
            double towerval_z = tower_z->at(j);
            double towerval_R = tower_R->at(j);
            double towerval_phi = tower_phi->at(j);
            double towerval_eta = tower_eta->at(j);
            double towerval_edep = tower_edep->at(j);
            
            double towerval_innr_x = tower_innr_x->at(j_innr);
            double towerval_innr_y = tower_innr_y->at(j_innr);
            double towerval_innr_z = tower_innr_z->at(j_innr);
            j_innr += 1;
            double towerval_innr_R = sqrt(towerval_innr_x*towerval_innr_x + towerval_innr_y*towerval_innr_y);

            double Ecalo_threshold = 0.07;

            if(towerval_edep > Ecalo_threshold)
            {
                tower_TotEMCalE += towerval_edep;

                // tower geom center
                tower_ModifEMCal_x += towerval_edep * towerval_x;
                tower_ModifEMCal_y += towerval_edep * towerval_y;
                tower_ModifEMCal_z += towerval_edep * towerval_z;

                h2_zr_tower->Fill(towerval_z, towerval_R, towerval_edep);

                // innr face center
                tower_innr_ModifEMCal_x += towerval_edep * towerval_innr_x;
                tower_innr_ModifEMCal_y += towerval_edep * towerval_innr_y;
                tower_innr_ModifEMCal_z += towerval_edep * towerval_innr_z;

                // cout<<"towerval_x is: "<<towerval_x<<", "<<towerval_y<<", "<<towerval_z<<endl;
                // cout<<"towerval_innr_x is: "<<towerval_innr_x<<", "<<towerval_innr_y<<", "<<towerval_innr_z<<endl;

                h2_zr_tower_innr->Fill(towerval_innr_z, towerval_innr_R, towerval_edep);
            }
        }
        // geom center
        tower_ModifEMCal_x /= tower_TotEMCalE;
        tower_ModifEMCal_y /= tower_TotEMCalE;
        tower_ModifEMCal_z /= tower_TotEMCalE;
        double towergeom_xyz[3] = {tower_ModifEMCal_x, tower_ModifEMCal_y, tower_ModifEMCal_z};
        
        double tower_ModifEMCal_phi = TMath::ATan2(tower_ModifEMCal_y, tower_ModifEMCal_x);
        h_towerphi->Fill(tower_ModifEMCal_phi);

        // innr center 
        tower_innr_ModifEMCal_x /= tower_TotEMCalE;
        tower_innr_ModifEMCal_y /= tower_TotEMCalE;
        tower_innr_ModifEMCal_z /= tower_TotEMCalE;
        double towerinnr_xyz[3] = {tower_innr_ModifEMCal_x, tower_innr_ModifEMCal_y, tower_innr_ModifEMCal_z};
        
        double tower_innr_ModifEMCal_phi = TMath::ATan2(tower_innr_ModifEMCal_y, tower_innr_ModifEMCal_x);
        h_innr_towerphi->Fill(tower_innr_ModifEMCal_phi);

        // 遍历 cluster innersurface vector 中的每个元素 --------------------------------------------------------------------
        Double_t cluster_innr_TotEMCalE = 0.;
        Double_t cluster_innr_ModifEMCal_x = 0.;
        Double_t cluster_innr_ModifEMCal_y = 0.;
        Double_t cluster_innr_ModifEMCal_z = 0.;

        for (size_t j = 0; j < caloClus_innr_x->size(); j++)
        {   
            double clusterval_innr_x = caloClus_innr_x->at(j);
            double clusterval_innr_y = caloClus_innr_y->at(j);
            double clusterval_innr_z = caloClus_innr_z->at(j);
            double clusterval_innr_R = caloClus_innr_R->at(j);
            double clusterval_innr_phi = caloClus_innr_phi->at(j);
            double clusterval_innr_edep = caloClus_innr_edep->at(j);

            double Ecalo_threshold = 0.5;
            if((clusterval_innr_edep > Ecalo_threshold)&&(clusterval_innr_R<140))
            {
                cluster_innr_ModifEMCal_x += clusterval_innr_x;
                cluster_innr_ModifEMCal_y += clusterval_innr_y;
                cluster_innr_ModifEMCal_z += clusterval_innr_z;
                cluster_innr_TotEMCalE += clusterval_innr_edep;

                num_of_clus += 1;
                // cout<<"cluster_innr_xyz: "<<clusterval_innr_x<<" "<<clusterval_innr_y<<" "<<clusterval_innr_z<<endl;

                h2_zr_clust_innr->Fill(clusterval_innr_z, clusterval_innr_R, clusterval_innr_edep);
            }           
        }
        cluster_innr_ModifEMCal_x /= cluster_innr_TotEMCalE;
        cluster_innr_ModifEMCal_y /= cluster_innr_TotEMCalE;
        cluster_innr_ModifEMCal_z /= cluster_innr_TotEMCalE;
        double cluster_innr_phi = TMath::ATan2(cluster_innr_ModifEMCal_y, cluster_innr_ModifEMCal_x);

        // double dphi_C = g_dphiM_z->Eval(cluster_innr_ModifEMCal_z);
        // double dRad_C = g_dRadM_z->Eval(cluster_innr_ModifEMCal_z);
        // double dphi_C = f1_dphi_C->Eval(cluster_innr_TotEMCalE);
        double dphi_C = g1_dphi_C->Eval(cluster_innr_TotEMCalE);

        if (cluster_innr_phi < 0)
        {
            cluster_innr_phi += 2 * TMath::Pi();
        }
        // cluster_innr_phi = cluster_innr_phi + dphi_C; // ave = - 0.0083
        
        double R_temp = sqrt(cluster_innr_ModifEMCal_x*cluster_innr_ModifEMCal_x + cluster_innr_ModifEMCal_y*cluster_innr_ModifEMCal_y);
        // R_temp = R_temp + dRad_C; // ave = 0.4
        cluster_innr_ModifEMCal_x = R_temp*cos(cluster_innr_phi);
        cluster_innr_ModifEMCal_y = R_temp*sin(cluster_innr_phi);

        double clustinnr_xyz[3] = {cluster_innr_ModifEMCal_x, cluster_innr_ModifEMCal_y, cluster_innr_ModifEMCal_z};

        // 遍历 cluster vector 中的每个元素  --------------------------------------------------------------------
        Double_t cluster_TotEMCalE = 0.;
        Double_t cluster_ModifEMCal_x = 0.;
        Double_t cluster_ModifEMCal_y = 0.;
        Double_t cluster_ModifEMCal_z = 0.;

        for (size_t j = 0; j < caloClus_x->size(); j++)
        {   
            if ((caloClus_system->at(j)) != 0) continue;

            double clusterval_x = caloClus_x->at(j);
            double clusterval_y = caloClus_y->at(j);
            double clusterval_z = caloClus_z->at(j);
            double clusterval_R = sqrt(clusterval_x*clusterval_x + clusterval_y*clusterval_y);
            double clusterval_phi = caloClus_phi->at(j);
            double clusterval_edep = caloClus_edep->at(j);

            double Ecalo_threshold = 0.3;
            if((clusterval_edep > Ecalo_threshold)&&(clusterval_R<200))
            {
                h_clustphi->Fill(clusterval_phi);
                cluster_ModifEMCal_x += clusterval_x;
                cluster_ModifEMCal_y += clusterval_y;
                cluster_ModifEMCal_z += clusterval_z;
                cluster_TotEMCalE += 1;

                // cout<<"cluster_geom_xyz: "<<clusterval_x<<" "<<clusterval_y<<" "<<clusterval_z<<endl;

                h2_zr_clust->Fill(clusterval_z, clusterval_R, clusterval_edep);
            }           
        }
        cluster_ModifEMCal_x /= cluster_TotEMCalE;
        cluster_ModifEMCal_y /= cluster_TotEMCalE;
        cluster_ModifEMCal_z /= cluster_TotEMCalE;
        double clustgeom_xyz[3]  = {cluster_ModifEMCal_x, cluster_ModifEMCal_y, cluster_ModifEMCal_z};
        
        double cluster_geom_phi = TMath::ATan2(cluster_ModifEMCal_y, cluster_ModifEMCal_x);
        if (cluster_geom_phi < 0)
        {
            cluster_geom_phi += 2 * TMath::Pi();
        }

        // calculate delta truth tower cluster  --------------------------------------------------------------------
        double DR_pr1_clustinnr = Cal_Distance_R(pr1cemc_xyz, clustinnr_xyz);
        double DR_pr1_towerinnr = Cal_Distance_R(pr1cemc_xyz, towerinnr_xyz);
        double DR_g4hit_clustgeom = Cal_Distance_R(g4hit_xyz, clustgeom_xyz);
        double DR_g4hit_towergeom = Cal_Distance_R(g4hit_xyz, towergeom_xyz);

        double dRadius_pr1_clustinnr   = Cal_dR(pr1cemc_xyz, clustinnr_xyz);
        double dRadius_g4hit_clustgeom = Cal_dR(g4hit_xyz, clustgeom_xyz);

        double dphiR_pr1_clustinnr   = Cal_dphiR(pr1cemc_xyz, clustinnr_xyz);
        double dphiR_g4hit_clustgeom = Cal_dphiR(g4hit_xyz, clustgeom_xyz);

        // distance between reco and truth
        h1_DR_pr1_clustinnr->Fill(DR_pr1_clustinnr);
        h1_DR_pr1_towerinnr->Fill(DR_pr1_towerinnr);
        h1_DR_g4hit_clustgeom->Fill(DR_g4hit_clustgeom);
        h1_DR_g4hit_towergeom->Fill(DR_g4hit_towergeom);

        // num of clus checking
        if(DR_pr1_clustinnr>5)
        {
            // cout<<"DR_pr1_clustinnr is: "<< DR_pr1_clustinnr <<", number of clus is: "<< num_of_clus <<endl;
            if(num_of_clus==1)
            {
                double clus_R_tem = sqrt(clustinnr_xyz[0]*clustinnr_xyz[0] + clustinnr_xyz[1]*clustinnr_xyz[1]);
                h2_weird_XY->Fill(clustinnr_xyz[2], clus_R_tem);
                h2_weird_pt_E->Fill(PrimaryG4P_Pt->at(0), PrimaryG4P_E->at(0));
                h1_weird_pt->Fill(PrimaryG4P_Pt->at(0));
                h1_weird_E ->Fill(PrimaryG4P_E->at(0)); 

                // 添加点
                g_pr1hit->SetPoint(pointIndex, pr1cemc_xyz[0], pr1cemc_xyz[1]);
                g_clustinnr->SetPoint(pointIndex, clustinnr_xyz[0], clustinnr_xyz[1]);
                pointIndex++;
            }
        }

        h1_dRadius_pr1_clustinnr  ->Fill(dRadius_pr1_clustinnr);
        h1_dRadius_g4hit_clustgeom->Fill(dRadius_g4hit_clustgeom);

        PrHit_phi - cluster_innr_phi > 0 ? h1_dphiR_pr1_clustinnr->Fill(dphiR_pr1_clustinnr) : h1_dphiR_pr1_clustinnr->Fill(-dphiR_pr1_clustinnr);
        // h1_dphiR_pr1_clustinnr    ->Fill(dphiR_pr1_clustinnr);
        h1_dphiR_g4hit_clustgeom  ->Fill(dphiR_g4hit_clustgeom);

        PrHit_phi - cluster_innr_phi > 0 ? h2_corr_dR_dphiR->Fill(dRadius_pr1_clustinnr,dphiR_pr1_clustinnr) : h2_corr_dR_dphiR->Fill(dRadius_pr1_clustinnr,-dphiR_pr1_clustinnr);

        // right or left?
        // PrHit_phi - cluster_innr_phi > 0 ? h1_DR_pr1_clustinnr->Fill(DR_pr1_clustinnr) : h1_DR_pr1_clustinnr->Fill(-DR_pr1_clustinnr);
        // g4hit_ModifEMCal_phi - cluster_innr_phi > 0 ? h1_DR_g4hit_clustgeom->Fill(DR_g4hit_clustgeom) : h1_DR_g4hit_clustgeom->Fill(-DR_g4hit_clustgeom);
        h1_dphi_distri_pr1_clustinnr->Fill(PrHit_phi - cluster_innr_phi);
        h1_dphi_distri_pr1_clustgeom->Fill(PrHit_phi - cluster_geom_phi);
        h1_dphi_distri_g4h_clustinnr->Fill(g4hit_ModifEMCal_phi - cluster_innr_phi);
        h1_dphi_distri_g4h_clustgeom->Fill(g4hit_ModifEMCal_phi - cluster_geom_phi);

        // h2_dphi_distri_test->Fill(abs(cluster_innr_ModifEMCal_z),(PrHit_phi - cluster_innr_phi));

        // 2d map
        double pr_electron_pt = PrimaryG4P_Pt->at(0);
        double pr_electron_E  = PrimaryG4P_E->at(0);

        h2_DR_pr1_clustinnr_pt  ->Fill(pr_electron_pt, DR_pr1_clustinnr);
        h2_DR_g4hit_clustgeom_pt->Fill(pr_electron_pt, DR_g4hit_clustgeom);
        h2_DR_pr1_clustinnr_E   ->Fill(pr_electron_E, DR_pr1_clustinnr);
        h2_DR_g4hit_clustgeom_E ->Fill(pr_electron_E, DR_g4hit_clustgeom);

        TP_DR_pr1_clustinnr_pt  ->Fill(pr_electron_pt, DR_pr1_clustinnr);
        TP_DR_g4hit_clustgeom_pt->Fill(pr_electron_pt, DR_g4hit_clustgeom);
        TP_DR_pr1_clustinnr_E   ->Fill(pr_electron_E, DR_pr1_clustinnr);
        TP_DR_g4hit_clustgeom_E ->Fill(pr_electron_E, DR_g4hit_clustgeom);

        // 2d map for dphi
        h2_dphi_distri_test->Fill(pr_electron_pt,(PrHit_phi - cluster_innr_phi));
        h2_dphi_Pr_innr_Etruth->Fill(pr_electron_E,(PrHit_phi - cluster_innr_phi));
        h2_dphi_Pr_geom_Etruth->Fill(pr_electron_E,(PrHit_phi - cluster_geom_phi));

        if (caloClus_innr_edep->size() == 1)
        {
            // 计算 inner‐surface cluster 的 pseudorapidity and 计算 geometric cluster 的 pseudorapidity
            double clust_innr_x = caloClus_innr_x->at(0);
            double clust_innr_y = caloClus_innr_y->at(0);
            double clust_innr_z = caloClus_innr_z->at(0);
            double clust_innr_R = std::sqrt(clust_innr_x*clust_innr_x + clust_innr_y*clust_innr_y);
            double clust_innr_eta = std::asinh(clust_innr_z / clust_innr_R);
            
            double clust_geo_x = caloClus_x->at(0);
            double clust_geo_y = caloClus_y->at(0);
            double clust_geo_z = caloClus_z->at(0);
            double clust_geo_R = std::sqrt(clust_geo_x*clust_geo_x + clust_geo_y*clust_geo_y);
            double clust_geo_eta  = std::asinh(clust_geo_z  / clust_geo_R);

            double reco_electron_E = caloClus_innr_edep->at(0);
            h2_dphi_Pr_innr_Ereco->Fill(reco_electron_E,(PrHit_phi - cluster_innr_phi));
            h2_dphi_Pr_geom_Ereco->Fill(reco_electron_E,(PrHit_phi - cluster_geom_phi));

            h2_dphi_Pr_innr_eta->Fill(clust_innr_eta,(PrHit_phi - cluster_innr_phi));
            h2_dphi_Pr_geom_eta->Fill(clust_geo_eta,(PrHit_phi - cluster_geom_phi));

            h2_dRadius_Pr_innr_Ereco->Fill(reco_electron_E,(PrHit_R - clust_innr_R));
            h2_dRadius_Pr_geom_Ereco->Fill(reco_electron_E,(PrHit_R - clust_geo_R));
        }
        
        // z dependent check
        h2_z_dphi_distri_test->Fill(cluster_innr_ModifEMCal_z,(PrHit_phi - cluster_innr_phi));
        h2_z_dRad_distri_test->Fill(cluster_innr_ModifEMCal_z,dRadius_pr1_clustinnr);
        h2_z_dDis_distri_test->Fill(cluster_innr_ModifEMCal_z,DR_pr1_clustinnr);

        // g_cor_z_dphi_distri_test = Cor_var_Z(h2_z_dphi_distri_test, -0.012, -0.004);
        // g_cor_z_dRad_distri_test = Cor_var_Z(h2_z_dRad_distri_test, -1, 2);
        // g_cor_z_dDis_distri_test = Cor_var_Z(h2_z_dDis_distri_test);
    }
    cout<<"pointIndex is: "<<pointIndex<<endl;

    // 定义一个用于二次曲线拟合的 TF1
    TF1* fitCurve_dphi = new TF1("fitCurve_dphi", "[0] + [1]*x + [2]*x*x + [3]*x*x*x + [4]*x*x*x*x + [5]*x*x*x*x", 0.5, 16);
    TF1* projectionFit_dphi = new TF1("projectionFit_dphi","gaus", -1, 1);
    TGraph* g1fitPeakVsX_dphi = FitPeakVsX(h2_dphi_Pr_innr_Ereco, projectionFit_dphi, fitCurve_dphi, "g1fitPeakVsX_dphi");

    TF1* fitCurve_dRadius = new TF1("fitCurve_dRadius", "[0] + [1]*x + [2]*x*x + [3]*x*x*x + [4]*x*x*x*x + [5]*x*x*x*x", 0.5, 16);
    TF1* projectionFit_dRadius = new TF1("projectionFit_dRadius","gaus", -1, 1);
    TGraph* g1fitPeakVsX_dRadius = FitPeakVsX(h2_dRadius_Pr_innr_Ereco, projectionFit_dRadius, fitCurve_dRadius, "g1fitPeakVsX_dRadius");

    // 保存直方图到文件
    TFile outfile(fOutputName.c_str(), "RECREATE");
    outfile.cd();

    h_g4hitphi->Write();
    h_towerphi->Write();
    h_innr_towerphi->Write();

    h1_DR_pr1_clustinnr->Write();
    h1_DR_pr1_towerinnr->Write();
    h1_DR_g4hit_clustgeom->Write();
    h1_DR_g4hit_towergeom->Write();

    h1_dRadius_pr1_clustinnr  ->Write();
    h1_dRadius_g4hit_clustgeom->Write();
    h1_dphiR_pr1_clustinnr    ->Write();
    h1_dphiR_g4hit_clustgeom  ->Write();

    h2_zr_g4hit->Write();
    TH1D* h1_zr_g4Pr = h2_zr_g4hit->ProjectionY("h1_zr_g4Pr");
    h1_zr_g4Pr->Write();
    h2_zr_tower->Write();
    h2_zr_clust->Write();

    h2_zr_tower_innr->Write();
    h2_zr_clust_innr->Write();

    h2_DR_pr1_clustinnr_pt->Write();
    h2_DR_g4hit_clustgeom_pt->Write();
    h2_DR_pr1_clustinnr_E->Write();
    h2_DR_g4hit_clustgeom_E->Write();

    // profile write
    TP_DR_pr1_clustinnr_pt  ->Write();
    TP_DR_g4hit_clustgeom_pt->Write();
    TP_DR_pr1_clustinnr_E   ->Write();
    TP_DR_g4hit_clustgeom_E ->Write();

    // weird hit pos
    h2_weird_XY->Write();
    h2_weird_pt_E->Write();
    h1_weird_pt->Write();
    h1_weird_E->Write(); 

    h1_dphi_distri_pr1_clustinnr->Write();
    h1_dphi_distri_pr1_clustgeom->Write();
    h1_dphi_distri_g4h_clustinnr->Write();
    h1_dphi_distri_g4h_clustgeom->Write();

    h2_dphi_distri_test->Write();
    h2_dphi_Pr_innr_Etruth->Write();
    h2_dphi_Pr_geom_Etruth->Write();
    h2_dphi_Pr_innr_Ereco->Write();
    h2_dphi_Pr_geom_Ereco->Write();

    h2_dphi_Pr_innr_eta->Write();
    h2_dphi_Pr_geom_eta->Write();

    h2_dRadius_Pr_innr_Ereco->Write();
    h2_dRadius_Pr_geom_Ereco->Write();

    h2_corr_dR_dphiR->Write();

    h2_z_dphi_distri_test->Write();
    h2_z_dRad_distri_test->Write();
    h2_z_dDis_distri_test->Write();

    g1fitPeakVsX_dphi->Write();
    fitCurve_dphi->Write();
    g1fitPeakVsX_dRadius->Write();
    fitCurve_dRadius->Write();

    // g_cor_z_dphi_distri_test->Write("g_cor_z_dphi_distri_test");
    // g_cor_z_dRad_distri_test->Write("g_cor_z_dRad_distri_test");
    // g_cor_z_dDis_distri_test->Write("g_cor_z_dDis_distri_test");

    // Draw the results
    TCanvas *c1 = new TCanvas("c1", "Hit Position", 800, 600);
    g_pr1hit->Draw("AP");
    g_clustinnr->Draw("P SAME"); // 在同一图层绘制
    c1->SaveAs("pr1hit_vs_clustinnr_large.pdf"); 

    std::cout << "Histograms saved to " << fOutputName << std::endl;
}

double Cal_Distance_R(const double xyz_1[3], const double xyz_2[3])
{
    double delta_x = xyz_1[0] - xyz_2[0];
    double delta_y = xyz_1[1] - xyz_2[1];
    double delta_z = xyz_1[2] - xyz_2[2];

    double distance_R = sqrt( delta_x*delta_x + delta_y*delta_y);
    // double distance_R = sqrt( delta_x*delta_x + delta_y*delta_y + delta_z*delta_z);

    return distance_R;
}

double Cal_dR(const double xyz_1[3], const double xyz_2[3])
{
    double R_1 = sqrt(xyz_1[0]*xyz_1[0] + xyz_1[1]*xyz_1[1]);
    double R_2 = sqrt(xyz_2[0]*xyz_2[0] + xyz_2[1]*xyz_2[1]);

    double dR = R_1 - R_2;

    return dR;
}

double Cal_dphiR(const double xyz_1[3], const double xyz_2[3])
{
    double R_1 = sqrt(xyz_1[0]*xyz_1[0] + xyz_1[1]*xyz_1[1]);
    double R_2 = sqrt(xyz_2[0]*xyz_2[0] + xyz_2[1]*xyz_2[1]);
    double R_ratio = R_2 / R_1;

    double x1 = xyz_1[0] * R_ratio;
    double y1 = xyz_1[1] * R_ratio;
    double x2 = xyz_2[0];
    double y2 = xyz_2[1];

    double delta_x = x1 - x2;
    double delta_y = y1 - y2;

    double dphiR = sqrt(delta_x*delta_x + delta_y*delta_y);
    // double dphiR = sqrt(x1*x1 + y1*y1) - sqrt(x2*x2 + y2*y2);
    // double dphiR = R_2 / R_1;


    return dphiR;
}

TGraph* Cor_var_Z(TH2D* h2, double xrange_min = -0.05, double xrange_max = 0.05)
{
    const int nBinsX = h2->GetNbinsX();  // Z 的 bin 数
    std::vector<double> z_vals;
    std::vector<double> dphi_means;

    int binstep = 3; // bin 步长
    for (int i = 1; i <= nBinsX; i+=binstep) 
    {
        double z1 = h2->GetXaxis()->GetBinCenter(i);
        double z2 = h2->GetXaxis()->GetBinCenter(i + 1);
        double z3 = h2->GetXaxis()->GetBinCenter(i + 2);
        double z_center = (z1 + z2 + z3)/binstep;

        // 在每个 Z bin 上做投影（对 dphi 轴）
        TH1D* projY = h2->ProjectionY(Form("projY_%d", i), i, i + binstep - 1);
        if (projY->GetEntries() < 10)
        {
            delete projY;
            continue; // 跳过统计太少的 bin
        }
    
        // TF1* cbfit = new TF1("cbfit", crystalball, xrange_min, xrange_max, 5);
        // // cbfit->SetParameters(1, 1.5, 2, 0, 0.01);
        // projY->Fit(cbfit, "RQ");
        // double dphi_peak = cbfit->GetParameter(3);

        TF1* gausfit = new TF1("gausfit", "gaus", xrange_min, xrange_max);
        // gausfit->SetParameters(projY->GetMaximum(), projY->GetMean(), projY->GetRMS());
        projY->Fit(gausfit, "RQ");
        double dphi_peak = gausfit->GetParameter(1);
        
        double dphi_mean = projY->GetMean();

        z_vals.push_back(z_center);
        dphi_means.push_back(dphi_peak);
        delete projY; // 清理内存
    }

    // 用 TGraph 存储 correction 曲线
    TGraph* gr_correction = new TGraph(z_vals.size(), &z_vals[0], &dphi_means[0]);
    gr_correction->SetTitle("Z-dependent var correction;Z (cm)");
    gr_correction->SetMarkerStyle(20);

    return gr_correction;
}

Double_t crystalball(Double_t* x, Double_t* par)
{
    // par: [0]=norm, [1]=alpha, [2]=n, [3]=mean, [4]=sigma
    Double_t t = (x[0]-par[3])/par[4];
    if (par[1] < 0) t = -t;
    Double_t abs_alpha = fabs(par[1]);
    if (t >= -abs_alpha)
        return par[0]*exp(-0.5*t*t);
    else {
        Double_t a = pow(par[2]/abs_alpha, par[2]) * exp(-0.5*abs_alpha*abs_alpha);
        Double_t b = par[2]/abs_alpha - abs_alpha;
        return par[0]*a / pow(b - t, par[2]);
    }
}


TGraph* FitPeakVsX(TH2D* h2_XY,
                   TF1* projectionFit = nullptr, // 可选：对每个 projY 拟合用，若不用可设为 nullptr
                   TF1* peakVsXFit = nullptr,    // 用来拟合峰值随 x 变化的函数
                   const char* graphName = "grPeakVsX")
{
    std::vector<double> vx, vy;
    int nBinsX = h2_XY->GetNbinsX();
    
    for (int ix = 1; ix <= nBinsX; ++ix) {
        // 投影第 ix 个 bin 到 Y 轴
        TH1D* projY = h2_XY->ProjectionY(
            Form("projY_%d", ix),
            ix, ix, "e"
        );
        if (projY->GetEntries() < 500) { 
            delete projY;
            continue;
        }

        double yPeak = 0;
        int maxBin = projY->GetMaximumBin();
        yPeak = projY->GetBinCenter(maxBin);

        double fitMin = yPeak - 0.004;
        double fitMax = yPeak + 0.004;
        projectionFit->SetRange(fitMin, fitMax);
        projY->Fit(projectionFit, "RQ");

        // yPeak update!
        yPeak = projectionFit->GetParameter(1);

        double xCenter = h2_XY->GetXaxis()->GetBinCenter(ix);
        vx.push_back(xCenter);
        vy.push_back(yPeak);

        delete projY;
    }

    // 构造 TGraph 并拟合
    int n = vx.size();
    TGraph* gr = new TGraph(n, vx.data(), vy.data());
    gr->SetName(graphName);
    gr->SetTitle("Peak position vs X;X;Y_{peak}");
    gr->Fit(peakVsXFit, "RQ");

    return gr;
}
