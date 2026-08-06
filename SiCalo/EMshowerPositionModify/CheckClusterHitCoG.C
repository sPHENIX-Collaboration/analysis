#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TMath.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TSystem.h>

#ifdef __CLING__
#include <TInterpreter.h>
#endif

#include <vector>
#include <iostream>
#include <cmath>

double DeltaPhi(double phi1, double phi2)
{
    double dphi = phi1 - phi2;
    while (dphi > TMath::Pi())  dphi -= 2.0 * TMath::Pi();
    while (dphi < -TMath::Pi()) dphi += 2.0 * TMath::Pi();
    return dphi;
}

void CheckClusterHitCoG(const char* inputFileName  = "Dataset/ECPM_ana527_Electron_1GeV_100k.root",
                        const char* outputFileName = "check_cluster_hitcog.root",
                        double fitDphiMin = -0.005,
                        double fitDphiMax =  0.005,
                        double fitDrMin   = 1.5,
                        double fitDrMax   = 4.0)
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
        return;
    }

    std::vector<int>* caloClus_system = nullptr;
    std::vector<double>* caloClus_edep = nullptr;
    std::vector<double>* caloClus_Z = nullptr;
    std::vector<double>* caloClus_R = nullptr;
    std::vector<double>* caloClus_Phi = nullptr;

    std::vector<std::vector<float>>* calo_tower_e = nullptr;

    std::vector<float>* CEMC_Hit_CoG_x = nullptr;
    std::vector<float>* CEMC_Hit_CoG_y = nullptr;
    std::vector<float>* CEMC_Hit_CoG_z = nullptr;
    std::vector<float>* CEMC_Hit_CoG_R = nullptr;

    std::vector<float>* PrimaryG4P_Pt = nullptr;
    std::vector<float>* PrimaryG4P_Eta = nullptr;
    std::vector<float>* PrimaryG4P_Phi = nullptr;
    std::vector<float>* PrimaryG4P_E = nullptr;
    std::vector<int>* PrimaryG4P_PID = nullptr;

    tree->SetBranchAddress("caloClus_system", &caloClus_system);
    tree->SetBranchAddress("caloClus_edep",   &caloClus_edep);
    tree->SetBranchAddress("caloClus_Z",      &caloClus_Z);
    tree->SetBranchAddress("caloClus_R",      &caloClus_R);
    tree->SetBranchAddress("caloClus_Phi",    &caloClus_Phi);

    tree->SetBranchAddress("calo_tower_e", &calo_tower_e);

    tree->SetBranchAddress("CEMC_Hit_CoG_x", &CEMC_Hit_CoG_x);
    tree->SetBranchAddress("CEMC_Hit_CoG_y", &CEMC_Hit_CoG_y);
    tree->SetBranchAddress("CEMC_Hit_CoG_z", &CEMC_Hit_CoG_z);
    tree->SetBranchAddress("CEMC_Hit_CoG_R", &CEMC_Hit_CoG_R);

    tree->SetBranchAddress("PrimaryG4P_Pt",  &PrimaryG4P_Pt);
    tree->SetBranchAddress("PrimaryG4P_Eta", &PrimaryG4P_Eta);
    tree->SetBranchAddress("PrimaryG4P_Phi", &PrimaryG4P_Phi);
    tree->SetBranchAddress("PrimaryG4P_E",   &PrimaryG4P_E);
    tree->SetBranchAddress("PrimaryG4P_PID", &PrimaryG4P_PID);

    TH1D* h_dE = new TH1D(
        "h_dE",
        "Cluster E - sum associated tower E;E_{cluster}-#Sigma E_{tower} [GeV];Counts",
        200, -1.0, 1.0
    );

    TH1D* h_dPhi = new TH1D(
        "h_dPhi",
        "Cluster #phi - CEMC Hit CoG #phi;#Delta#phi [rad];Counts",
        200, -0.1, 0.1
    );

    TH1D* h_dPhi_1tower = new TH1D(
        "h_dPhi_1tower",
        "Cluster #phi - CEMC Hit CoG #phi, N_{tower}=1;#Delta#phi [rad];Counts",
        200, -0.1, 0.1
    );

    TH1D* h_dPhi_2tower = new TH1D(
        "h_dPhi_2tower",
        "Cluster #phi - CEMC Hit CoG #phi, N_{tower}=2;#Delta#phi [rad];Counts",
        200, -0.1, 0.1
    );

    TH1D* h_dPhi_3tower = new TH1D(
        "h_dPhi_3tower",
        "Cluster #phi - CEMC Hit CoG #phi, N_{tower}=3;#Delta#phi [rad];Counts",
        200, -0.1, 0.1
    );

    TH1D* h_dPhi_4plustower = new TH1D(
        "h_dPhi_4plustower",
        "Cluster #phi - CEMC Hit CoG #phi, N_{tower}#geq4;#Delta#phi [rad];Counts",
        200, -0.1, 0.1
    );

    TH1D* h_dZ = new TH1D(
        "h_dZ",
        "Cluster Z - CEMC Hit CoG Z;#DeltaZ [cm];Counts",
        200, -20.0, 20.0
    );

    TH1D* h_dr = new TH1D(
        "h_dr",
        "Cluster radial R - CEMC Hit CoG radial R;R_{cluster}-R_{CEMC Hit CoG} [cm];Counts",
        200, -20.0, 20.0
    );

    Long64_t nentries = tree->GetEntries();

    Long64_t nUsed = 0;
    Long64_t nUsed1Tower = 0;
    Long64_t nUsed2Tower = 0;
    Long64_t nUsed3Tower = 0;
    Long64_t nUsed4PlusTower = 0;

    Long64_t nSkipCluster = 0;
    Long64_t nSkipHitCoG = 0;
    Long64_t nSkipPrimary = 0;
    Long64_t nBadTower = 0;
    Long64_t nNull = 0;

    for (Long64_t iev = 0; iev < nentries; ++iev)
    {
        tree->GetEntry(iev);

        if (!caloClus_system || !caloClus_edep || !caloClus_Z ||
            !caloClus_R || !caloClus_Phi || !calo_tower_e ||
            !CEMC_Hit_CoG_x || !CEMC_Hit_CoG_y ||
            !CEMC_Hit_CoG_z || !CEMC_Hit_CoG_R ||
            !PrimaryG4P_Pt || !PrimaryG4P_Eta || !PrimaryG4P_Phi ||
            !PrimaryG4P_E || !PrimaryG4P_PID)
        {
            ++nNull;
            continue;
        }

        int emcalClusIndexInCaloClus = -1;
        int emcalClusIndexInTowerVec = -1;
        int nEMCalClus = 0;

        for (size_t ic = 0; ic < caloClus_system->size(); ++ic)
        {
            if (caloClus_system->at(ic) == 0)
            {
                emcalClusIndexInCaloClus = ic;
                emcalClusIndexInTowerVec = nEMCalClus;
                ++nEMCalClus;
            }
        }

        if (nEMCalClus != 1)
        {
            ++nSkipCluster;
            continue;
        }

        if (CEMC_Hit_CoG_x->size() != 1 ||
            CEMC_Hit_CoG_y->size() != 1 ||
            CEMC_Hit_CoG_z->size() != 1 ||
            CEMC_Hit_CoG_R->size() != 1)
        {
            ++nSkipHitCoG;
            continue;
        }

        if (PrimaryG4P_Pt->size() != 1 ||
            PrimaryG4P_Eta->size() != 1 ||
            PrimaryG4P_Phi->size() != 1 ||
            PrimaryG4P_E->size() != 1 ||
            PrimaryG4P_PID->size() != 1)
        {
            ++nSkipPrimary;
            continue;
        }

        if (emcalClusIndexInTowerVec < 0 ||
            emcalClusIndexInTowerVec >= (int) calo_tower_e->size())
        {
            ++nBadTower;
            continue;
        }

        const auto& vTowerE = calo_tower_e->at(emcalClusIndexInTowerVec);
        const int nTower = (int) vTowerE.size();

        double sumTowerE = 0.0;
        for (size_t it = 0; it < vTowerE.size(); ++it)
        {
            const double e = vTowerE.at(it);
            if (!std::isfinite(e)) continue;
            sumTowerE += e;
        }

        const double clusE = caloClus_edep->at(emcalClusIndexInCaloClus);
        const double clusZ = caloClus_Z->at(emcalClusIndexInCaloClus);
        const double clusR = caloClus_R->at(emcalClusIndexInCaloClus);
        const double clusPhi = caloClus_Phi->at(emcalClusIndexInCaloClus);

        const double cogX = CEMC_Hit_CoG_x->at(0);
        const double cogY = CEMC_Hit_CoG_y->at(0);
        const double cogZ = CEMC_Hit_CoG_z->at(0);
        const double cogR = CEMC_Hit_CoG_R->at(0);
        const double cogPhi = std::atan2(cogY, cogX);

        const double dPhi = DeltaPhi(clusPhi, cogPhi);

        h_dE->Fill(clusE - sumTowerE);
        h_dPhi->Fill(dPhi);
        h_dZ->Fill(clusZ - cogZ);
        h_dr->Fill(clusR - cogR);

        if (nTower == 1)
        {
            h_dPhi_1tower->Fill(dPhi);
            ++nUsed1Tower;
        }
        else if (nTower == 2)
        {
            h_dPhi_2tower->Fill(dPhi);
            ++nUsed2Tower;
        }
        else if (nTower == 3)
        {
            h_dPhi_3tower->Fill(dPhi);
            ++nUsed3Tower;
        }
        else if (nTower >= 4)
        {
            h_dPhi_4plustower->Fill(dPhi);
            ++nUsed4PlusTower;
        }

        ++nUsed;
    }

    TFile* fout = TFile::Open(outputFileName, "RECREATE");

    gStyle->SetOptStat(1110);
    gStyle->SetOptFit(1111);

    TF1* f_dPhi = new TF1("f_dPhi", "gaus", fitDphiMin, fitDphiMax);
    TF1* f_dPhi_1tower = new TF1("f_dPhi_1tower", "gaus", fitDphiMin, fitDphiMax);
    TF1* f_dPhi_2tower = new TF1("f_dPhi_2tower", "gaus", fitDphiMin, fitDphiMax);
    TF1* f_dPhi_3tower = new TF1("f_dPhi_3tower", "gaus", fitDphiMin, fitDphiMax);
    TF1* f_dPhi_4plustower = new TF1("f_dPhi_4plustower", "gaus", fitDphiMin, fitDphiMax);

    TF1* f_dr = new TF1("f_dr", "gaus", fitDrMin, fitDrMax);

    h_dPhi->Fit(f_dPhi, "R");
    h_dPhi_1tower->Fit(f_dPhi_1tower, "R");
    h_dPhi_2tower->Fit(f_dPhi_2tower, "R");
    h_dPhi_3tower->Fit(f_dPhi_3tower, "R");
    h_dPhi_4plustower->Fit(f_dPhi_4plustower, "R");

    h_dr->Fit(f_dr, "R");

    TCanvas* c_dPhi = new TCanvas("c_dPhi", "dPhi Gaussian fit", 800, 600);
    h_dPhi->Draw();
    gPad->Update();
    c_dPhi->Write();

    TCanvas* c_dPhi_1tower = new TCanvas("c_dPhi_1tower", "dPhi Gaussian fit, N tower = 1", 800, 600);
    h_dPhi_1tower->Draw();
    gPad->Update();
    c_dPhi_1tower->Write();

    TCanvas* c_dPhi_2tower = new TCanvas("c_dPhi_2tower", "dPhi Gaussian fit, N tower = 2", 800, 600);
    h_dPhi_2tower->Draw();
    gPad->Update();
    c_dPhi_2tower->Write();

    TCanvas* c_dPhi_3tower = new TCanvas("c_dPhi_3tower", "dPhi Gaussian fit, N tower = 3", 800, 600);
    h_dPhi_3tower->Draw();
    gPad->Update();
    c_dPhi_3tower->Write();

    TCanvas* c_dPhi_4plustower = new TCanvas("c_dPhi_4plustower", "dPhi Gaussian fit, N tower >= 4", 800, 600);
    h_dPhi_4plustower->Draw();
    gPad->Update();
    c_dPhi_4plustower->Write();

    TCanvas* c_dr = new TCanvas("c_dr", "dR Gaussian fit", 800, 600);
    h_dr->Draw();
    gPad->Update();
    c_dr->Write();

    h_dE->Write();
    h_dPhi->Write();
    h_dPhi_1tower->Write();
    h_dPhi_2tower->Write();
    h_dPhi_3tower->Write();
    h_dPhi_4plustower->Write();
    h_dZ->Write();
    h_dr->Write();

    f_dPhi->Write();
    f_dPhi_1tower->Write();
    f_dPhi_2tower->Write();
    f_dPhi_3tower->Write();
    f_dPhi_4plustower->Write();
    f_dr->Write();

    fout->Close();

    std::cout << "Done." << std::endl;
    std::cout << "Input file              = " << inputFileName << std::endl;
    std::cout << "Output file             = " << outputFileName << std::endl;
    std::cout << "Used events             = " << nUsed << std::endl;
    std::cout << "  N tower = 1           = " << nUsed1Tower << std::endl;
    std::cout << "  N tower = 2           = " << nUsed2Tower << std::endl;
    std::cout << "  N tower = 3           = " << nUsed3Tower << std::endl;
    std::cout << "  N tower >= 4          = " << nUsed4PlusTower << std::endl;
    std::cout << "Skip cluster != 1       = " << nSkipCluster << std::endl;
    std::cout << "Skip CEMC CoG != 1      = " << nSkipHitCoG << std::endl;
    std::cout << "Skip PrimaryG4P != 1    = " << nSkipPrimary << std::endl;
    std::cout << "Bad tower count         = " << nBadTower << std::endl;
    std::cout << "Null pointer count      = " << nNull << std::endl;

    gSystem->Exec("rm -f AutoDict_vector_vector_float* AutoDict_vector_vector_int*");
}