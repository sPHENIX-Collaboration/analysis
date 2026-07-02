#include <TFile.h>
#include <TTree.h>
#include <TRandom3.h>
#include <iostream>

void SplitRootTrainTest(
    const char* infile = "/mnt/e/sphenix/EMshowerPositionModify/Dataset/ECPM_ana527_electron_10GeV_2M_test_corr.root",
    const char* treename = "tree",
    const char* trainfile = "/mnt/e/sphenix/EMshowerPositionModify/Dataset/ECPM_ana527_electron_10GeV_2M_test1_corr.root",
    const char* testfile = "/mnt/e/sphenix/EMshowerPositionModify/Dataset/ECPM_ana527_electron_10GeV_2M_test2_corr.root",
    double train_frac = 0.6,
    int seed = 12345
)
{
    TFile* fin = TFile::Open(infile, "READ");
    if (!fin || fin->IsZombie())
    {
        std::cout << "Cannot open input file: " << infile << std::endl;
        return;
    }

    TTree* tin = (TTree*)fin->Get(treename);
    if (!tin)
    {
        std::cout << "Cannot find tree: " << treename << std::endl;
        return;
    }

    TFile* fout_train = new TFile(trainfile, "RECREATE");
    TTree* ttrain = tin->CloneTree(0);

    TFile* fout_test = new TFile(testfile, "RECREATE");
    TTree* ttest = tin->CloneTree(0);

    TRandom3 rng(seed);

    Long64_t nentries = tin->GetEntries();
    Long64_t ntrain = 0;
    Long64_t ntest = 0;

    for (Long64_t i = 0; i < nentries; ++i)
    {
        tin->GetEntry(i);

        if (rng.Uniform() < train_frac)
        {
            fout_train->cd();
            ttrain->Fill();
            ntrain++;
        }
        else
        {
            fout_test->cd();
            ttest->Fill();
            ntest++;
        }
    }

    fout_train->cd();
    ttrain->Write();

    fout_test->cd();
    ttest->Write();

    fout_train->Close();
    fout_test->Close();
    fin->Close();

    std::cout << "Done." << std::endl;
    std::cout << "Input entries : " << nentries << std::endl;
    std::cout << "Train entries : " << ntrain << std::endl;
    std::cout << "Test entries  : " << ntest << std::endl;
}