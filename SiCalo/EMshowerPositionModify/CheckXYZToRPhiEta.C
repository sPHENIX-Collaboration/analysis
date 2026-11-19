#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TMath.h>

#ifdef __CLING__
#include <TInterpreter.h>
#endif

#include <iostream>
#include <vector>
#include <cmath>

double CalcR(double x, double y)
{
    return std::sqrt(x * x + y * y);
}

double CalcPhi(double x, double y)
{
    return std::atan2(y, x);
}

double CalcEtaFromXYZ(double x, double y, double z)
{
    double r = std::sqrt(x * x + y * y);
    if (r <= 0.0) return 0.0;
    return std::asinh(z / r);
}

double DeltaPhi(double phi1, double phi2)
{
    double dphi = phi1 - phi2;
    while (dphi >= TMath::Pi()) dphi -= 2.0 * TMath::Pi();
    while (dphi < -TMath::Pi()) dphi += 2.0 * TMath::Pi();
    return dphi;
}

void CheckXYZToRPhiEta(
    const char* inputFileName  = "Dataset/ECPM_ana527_Electron_1GeV_100k.root",
    const char* outputFileName = "check_xyz_to_rphieta.root"
)
{
#ifdef __CLING__
    gInterpreter->GenerateDictionary("vector<vector<float> >", "vector");
#endif

    const char* treeName = "tree";

    TFile* fin = TFile::Open(inputFileName, "READ");
    if (!fin || fin->IsZombie())
    {
        std::cerr << "Cannot open input file: " << inputFileName << std::endl;
        return;
    }

    TTree* tree = dynamic_cast<TTree*>(fin->Get(treeName));
    if (!tree)
    {
        std::cerr << "Cannot find tree: " << treeName << std::endl;
        fin->ls();
        fin->Close();
        delete fin;
        return;
    }

    std::vector<int>* caloClus_system = nullptr;

    std::vector<double>* caloClus_X = nullptr;
    std::vector<double>* caloClus_Y = nullptr;
    std::vector<double>* caloClus_Z = nullptr;
    std::vector<double>* caloClus_R = nullptr;
    std::vector<double>* caloClus_Phi = nullptr;

    std::vector<std::vector<float>>* calo_tower_x = nullptr;
    std::vector<std::vector<float>>* calo_tower_y = nullptr;
    std::vector<std::vector<float>>* calo_tower_z = nullptr;
    std::vector<std::vector<float>>* calo_tower_r = nullptr;
    std::vector<std::vector<float>>* calo_tower_eta = nullptr;
    std::vector<std::vector<float>>* calo_tower_phi = nullptr;

    std::vector<float>* CEMC_Hit_CoG_x = nullptr;
    std::vector<float>* CEMC_Hit_CoG_y = nullptr;
    std::vector<float>* CEMC_Hit_CoG_z = nullptr;
    std::vector<float>* CEMC_Hit_CoG_R = nullptr;

    tree->SetBranchAddress("caloClus_system", &caloClus_system);

    tree->SetBranchAddress("caloClus_X",   &caloClus_X);
    tree->SetBranchAddress("caloClus_Y",   &caloClus_Y);
    tree->SetBranchAddress("caloClus_Z",   &caloClus_Z);
    tree->SetBranchAddress("caloClus_R",   &caloClus_R);
    tree->SetBranchAddress("caloClus_Phi", &caloClus_Phi);

    tree->SetBranchAddress("calo_tower_x",   &calo_tower_x);
    tree->SetBranchAddress("calo_tower_y",   &calo_tower_y);
    tree->SetBranchAddress("calo_tower_z",   &calo_tower_z);
    tree->SetBranchAddress("calo_tower_r",   &calo_tower_r);
    tree->SetBranchAddress("calo_tower_eta", &calo_tower_eta);
    tree->SetBranchAddress("calo_tower_phi", &calo_tower_phi);

    tree->SetBranchAddress("CEMC_Hit_CoG_x", &CEMC_Hit_CoG_x);
    tree->SetBranchAddress("CEMC_Hit_CoG_y", &CEMC_Hit_CoG_y);
    tree->SetBranchAddress("CEMC_Hit_CoG_z", &CEMC_Hit_CoG_z);
    tree->SetBranchAddress("CEMC_Hit_CoG_R", &CEMC_Hit_CoG_R);

    TH1D* h_clus_dR = new TH1D(
        "h_clus_dR",
        "cluster R_{calc}-R_{read};#DeltaR [cm];Counts",
        200, -1e-4, 1e-4
    );

    TH1D* h_clus_dphi = new TH1D(
        "h_clus_dphi",
        "cluster #phi_{calc}-#phi_{read};#Delta#phi;Counts",
        200, -1e-8, 1e-8
    );

    TH1D* h_tower_dR = new TH1D(
        "h_tower_dR",
        "tower R_{calc}-R_{read};#DeltaR [cm];Counts",
        200, -1e-4, 1e-4
    );

    TH1D* h_tower_dphi = new TH1D(
        "h_tower_dphi",
        "tower #phi_{calc}-#phi_{read};#Delta#phi;Counts",
        200, -1e-8, 1e-8
    );

    TH1D* h_tower_deta = new TH1D(
        "h_tower_deta",
        "tower #eta_{calc}-#eta_{read};#Delta#eta;Counts",
        200, -1e-8, 1e-8
    );

    TH1D* h_cog_dR = new TH1D(
        "h_cog_dR",
        "CoG R_{calc}-R_{read};#DeltaR [cm];Counts",
        200, -1e-4, 1e-4
    );

    Long64_t nentries = tree->GetEntries();

    Long64_t nCluster = 0;
    Long64_t nTower = 0;
    Long64_t nCoG = 0;
    Long64_t nNull = 0;
    Long64_t nBadTower = 0;

    for (Long64_t iev = 0; iev < nentries; ++iev)
    {
        tree->GetEntry(iev);

        if (!caloClus_X || !caloClus_Y || !caloClus_Z ||
            !caloClus_R || !caloClus_Phi ||
            !calo_tower_x || !calo_tower_y || !calo_tower_z ||
            !calo_tower_r || !calo_tower_eta || !calo_tower_phi ||
            !CEMC_Hit_CoG_x || !CEMC_Hit_CoG_y ||
            !CEMC_Hit_CoG_z || !CEMC_Hit_CoG_R)
        {
            nNull++;
            continue;
        }

        for (size_t ic = 0; ic < caloClus_X->size(); ++ic)
        {
            double x = caloClus_X->at(ic);
            double y = caloClus_Y->at(ic);

            double r_calc = CalcR(x, y);
            double phi_calc = CalcPhi(x, y);

            h_clus_dR->Fill(r_calc - caloClus_R->at(ic));
            h_clus_dphi->Fill(DeltaPhi(phi_calc, caloClus_Phi->at(ic)));

            nCluster++;
        }

        if (
            calo_tower_x->size() != calo_tower_y->size() ||
            calo_tower_x->size() != calo_tower_z->size() ||
            calo_tower_x->size() != calo_tower_r->size() ||
            calo_tower_x->size() != calo_tower_eta->size() ||
            calo_tower_x->size() != calo_tower_phi->size()
        )
        {
            nBadTower++;
            continue;
        }

        for (size_t isys = 0; isys < calo_tower_x->size(); ++isys)
        {
            const std::vector<float>& vx = calo_tower_x->at(isys);
            const std::vector<float>& vy = calo_tower_y->at(isys);
            const std::vector<float>& vz = calo_tower_z->at(isys);

            const std::vector<float>& vr   = calo_tower_r->at(isys);
            const std::vector<float>& veta = calo_tower_eta->at(isys);
            const std::vector<float>& vphi = calo_tower_phi->at(isys);

            if (
                vx.size() != vy.size() ||
                vx.size() != vz.size() ||
                vx.size() != vr.size() ||
                vx.size() != veta.size() ||
                vx.size() != vphi.size()
            )
            {
                nBadTower++;
                continue;
            }

            for (size_t it = 0; it < vx.size(); ++it)
            {
                double x = vx.at(it);
                double y = vy.at(it);
                double z = vz.at(it);

                double r_calc = CalcR(x, y);
                double phi_calc = CalcPhi(x, y);
                double eta_calc = CalcEtaFromXYZ(x, y, z);

                h_tower_dR->Fill(r_calc - vr.at(it));
                h_tower_dphi->Fill(DeltaPhi(phi_calc, vphi.at(it)));
                h_tower_deta->Fill(eta_calc - veta.at(it));

                nTower++;
            }
        }

        for (size_t ih = 0; ih < CEMC_Hit_CoG_x->size(); ++ih)
        {
            double x = CEMC_Hit_CoG_x->at(ih);
            double y = CEMC_Hit_CoG_y->at(ih);

            double r_calc = CalcR(x, y);

            h_cog_dR->Fill(r_calc - CEMC_Hit_CoG_R->at(ih));

            nCoG++;
        }
    }

    std::cout << "Checked entries : " << nentries << std::endl;
    std::cout << "Checked clusters: " << nCluster << std::endl;
    std::cout << "Checked towers  : " << nTower << std::endl;
    std::cout << "Checked CoG     : " << nCoG << std::endl;
    std::cout << "Null entries    : " << nNull << std::endl;
    std::cout << "Bad tower events: " << nBadTower << std::endl;

    std::cout << "\nMean / RMS:" << std::endl;
    std::cout << "cluster dR   mean = " << h_clus_dR->GetMean()
              << ", RMS = " << h_clus_dR->GetRMS() << std::endl;
    std::cout << "cluster dphi mean = " << h_clus_dphi->GetMean()
              << ", RMS = " << h_clus_dphi->GetRMS() << std::endl;
    std::cout << "tower dR     mean = " << h_tower_dR->GetMean()
              << ", RMS = " << h_tower_dR->GetRMS() << std::endl;
    std::cout << "tower dphi   mean = " << h_tower_dphi->GetMean()
              << ", RMS = " << h_tower_dphi->GetRMS() << std::endl;
    std::cout << "tower deta   mean = " << h_tower_deta->GetMean()
              << ", RMS = " << h_tower_deta->GetRMS() << std::endl;
    std::cout << "CoG dR       mean = " << h_cog_dR->GetMean()
              << ", RMS = " << h_cog_dR->GetRMS() << std::endl;

    TFile* fout = new TFile(outputFileName, "RECREATE");

    h_clus_dR->Write();
    h_clus_dphi->Write();
    h_tower_dR->Write();
    h_tower_dphi->Write();
    h_tower_deta->Write();
    h_cog_dR->Write();

    fout->Close();
    fin->Close();

    std::cout << "\nOutput written to: " << outputFileName << std::endl;

    gSystem->Exec("rm -f AutoDict_vector_vector_float* AutoDict_vector_vector_int*");

}