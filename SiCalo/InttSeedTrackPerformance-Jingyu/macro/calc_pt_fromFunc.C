#include <TFile.h>
#include <TTree.h>
#include <TH3D.h>
#include <TCanvas.h>
#include <TVector2.h>
#include <vector>
#include <cmath>

void calc_pt_fromFunc(
    // const char* filename="/mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/ML4Reco/dataset/merged_last500kNew.root",
    const char* filename="/mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/ML4Reco/dataset/positron_last500kNew.root",
    // const char* filename="/mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/ParticleGen/output/electron_1M.root",
    const char* treename="tree",

    // const char* ptfuncfilename="/mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/InttSeedTrackPerformance/output/Ptfunc_fitCofEta_positron.root",
    const char* ptfuncfilename="/mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/InttSeedTrackPerformance/output/Ptfunc_fitCofEta_takashi_p.root",
    // const char* ptfuncfilename="/mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/InttSeedTrackPerformance/output/Ptfunc_fitCofEta.root",
    // const char* ptfuncfilename="/mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/InttSeedTrackPerformance/output/Ptfunc_fitCofEta_takashi_e.root",

    const char* outputname="/mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/InttSeedTrackPerformance/output/Ptfunc_Performance_positron_wTa.root"
    // const char* outputname="/mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/InttSeedTrackPerformance/output/Ptfunc_Performance_electron_wTa.root"
) 
{
    TFile outfile(outputname, "RECREATE");
    TH2D *h2_ptreso = new TH2D("h2_ptreso", "h2_ptreso; p_{T}^{reco} [GeV]; (p_{T}^{reco} - p_{T}^{truth}) / p_{T}^{truth}", 110, 0, 11, 200, -2, 2);

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

    TFile* ptfuncfile = TFile::Open(ptfuncfilename, "READ");
    if (!ptfuncfile || ptfuncfile->IsZombie()) {
        std::cerr << "Error: cannot open function file " << ptfuncfilename << std::endl;
        return;
    }
    TF1* ptfuncTP = nullptr;
    TF1* ptfuncTG = nullptr;
    ptfuncfile->GetObject("poly_func_profile", ptfuncTP);
    ptfuncfile->GetObject("poly_func_graph", ptfuncTG);

    TFile* file_C = new TFile("/mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/ParticleGen/output/calo_positron_dphi_ptbin_woC.root", "READ");
    // TFile* file_C = new TFile("/mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/ParticleGen/output/calo_electron_dphi_ptbin_woC.root", "READ");
    TF1* f1_dphi_C = (TF1*)file_C->Get("fitCurve");
    TGraph* g1_dphi_C = (TGraph*)file_C->Get("grPeakVsX");                

    // 定义变量
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

    std::vector<double> *caloClus_innr_x  = nullptr;
    std::vector<double> *caloClus_innr_y = nullptr;
    std::vector<double> *caloClus_innr_z = nullptr;
    std::vector<double> *caloClus_innr_phi  = nullptr;
    std::vector<double> *caloClus_innr_edep = nullptr;

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

    tree_data->SetBranchAddress("caloClus_innr_X", &caloClus_innr_x);
    tree_data->SetBranchAddress("caloClus_innr_Y", &caloClus_innr_y);
    tree_data->SetBranchAddress("caloClus_innr_Z", &caloClus_innr_z);
    tree_data->SetBranchAddress("caloClus_innr_Phi", &caloClus_innr_phi);
    tree_data->SetBranchAddress("caloClus_innr_edep", &caloClus_innr_edep);

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

        for (size_t j = 0; j < caloClus_innr_x->size(); j++)
        {   
            double cluster_innr_x = caloClus_innr_x->at(j);
            double cluster_innr_y = caloClus_innr_y->at(j);
            double cluster_innr_z = caloClus_innr_z->at(j);
            double cluster_innr_phi = caloClus_innr_phi->at(j);
            double cluster_innr_edep = caloClus_innr_edep->at(j);
            double cluster_innr_R = std::sqrt(cluster_innr_x*cluster_innr_x + cluster_innr_y*cluster_innr_y);
            double cluster_innr_eta = std::asinh(cluster_innr_z / cluster_innr_R);
            
            double Ecalo_threshold = 0.5;
            if((cluster_innr_edep < Ecalo_threshold)&&(cluster_innr_R>140)) continue;
            
            // clus innr position correction
            // double dphi_C = f1_dphi_C->Eval(cluster_innr_edep);
            double dphi_C = g1_dphi_C->Eval(cluster_innr_edep);

            if (cluster_innr_phi < 0)
            {
                cluster_innr_phi += 2 * TMath::Pi();
            }
            cluster_innr_phi = cluster_innr_phi + dphi_C;
            cluster_innr_x = cluster_innr_R*cos(cluster_innr_phi); // update position xy
            cluster_innr_y = cluster_innr_R*sin(cluster_innr_phi); // update position xy

            double phi2_reco = std::atan2(cluster_innr_y - trk56_Y, cluster_innr_x - trk56_X);
            double dphi_reco = TVector2::Phi_mpi_pi(phi2_reco - phi1);

            dphi_reco = std::abs(dphi_reco); // 确保 Δφ 为正值

            // double Cval_eval = ptfuncTP->Eval(cluster_innr_eta);
            double Cval_eval = ptfuncTG->Eval(cluster_innr_eta);
            // double pt_reco = Cval_eval / dphi_reco;
            double pt_reco = Cval_eval / (std::pow(dphi_reco, 0.986));
            
            h2_ptreso->Fill(Pr_pt, (pt_reco-Pr_pt)/Pr_pt);
        }
    }

    gStyle->SetOptFit(1111);
    TH1D* projY = h2_ptreso->ProjectionY("projY", 10, 11, "e");
    // Fit with Gaussian
    TF1* gausFit = new TF1("gausFit", "gaus", -1, 1);
    projY->Fit(gausFit, "R", "", -0.02, 0.1);
    
    TCanvas* c1 = new TCanvas("c1","Pt reso about 1 GeV",800,600);
    projY->Draw();
    gausFit->SetRange(-0.02, 0.1);
    gausFit->Draw("same");
    c1->SaveAs("Pt_reso_fromFunc_1GeV.pdf");

    outfile.cd();
    h2_ptreso->Write();
    projY->Write();
    gausFit->Write();
}


