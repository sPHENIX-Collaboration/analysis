#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TObject.h"
#include "TTree.h"
#include <iostream>

void merge_trees(TFile *outputfile, TTree *tree1, TTree *tree2)
{
    outputfile->cd();
    TObjArray *branches = tree2->GetListOfBranches();
    TIter next(branches);
    while (TObject *obj = next())
    {
        TBranch *branch = dynamic_cast<TBranch *>(obj);
        branch->SetTree(tree1);
        tree1->GetListOfBranches()->Add(branch);
        tree1->GetListOfLeaves()->Add(branch->GetLeaf(branch->GetName()));
        tree2->GetListOfBranches()->Remove(branch);
    }
    tree1->Write("", TObject::kWriteDelete);
}

void sync_mbd_intt_DST(const char *outputfname, const char *f_mbd_name, const char *t_mbd_name, const char *f_intt_name, const char *t_intt_name, int Nevent)
{
    TFile *f_mbd = TFile::Open(f_mbd_name);
    TTree *t_mbd = dynamic_cast<TTree *>(f_mbd->Get(t_mbd_name));
    int t_mbd_Nevt = t_mbd->GetEntries();
    UShort_t femclk;
    t_mbd->SetBranchAddress("femclk", &femclk);
    std::cout << "--> MBD tree entries:" << t_mbd_Nevt << std::endl;

    TFile *f_intt = TFile::Open(f_intt_name);
    TTree *t_intt = dynamic_cast<TTree *>(f_intt->Get(t_intt_name));
    int t_intt_Nevt = t_intt->GetEntries();
    uint64_t INTT_BCO;
    t_intt->SetBranchAddress("INTT_BCO", &INTT_BCO);
    std::cout << "--> INTT tree entries:" << t_intt_Nevt << std::endl;

    TFile *outfile = TFile::Open(outputfname, "recreate");
    TTree *t_intt_clone = t_intt->CloneTree(0);
    TTree *t_mbd_clone = t_mbd->CloneTree(0);

    int run_event = (Nevent > 0 && Nevent < t_mbd_Nevt) ? Nevent : t_mbd_Nevt;

    TH2F *intt_mbd_bco_presync = new TH2F("intt_mbd_bco_presync", "INTT - MBD", 100, 0, run_event * 1.5, 100, -10, 100000);
    intt_mbd_bco_presync->GetXaxis()->SetTitle("evt");
    intt_mbd_bco_presync->GetYaxis()->SetTitle("clock_diff");

    TH1F *intt_mbd_bco_presync_1D = new TH1F("intt_mbd_bco_presync_1D", "INTT - MBD", 100, -10, 100000);
    intt_mbd_bco_presync_1D->GetXaxis()->SetTitle("clock_diff");
    intt_mbd_bco_presync_1D->GetYaxis()->SetTitle("entry");

    TH2F *intt_mbd_bco_postsync = new TH2F("intt_mbd_bco_postsync", "INTT - MBD", 100, 0, run_event * 1.5, 100, -10, 100000);
    intt_mbd_bco_postsync->GetXaxis()->SetTitle("evt");
    intt_mbd_bco_postsync->GetYaxis()->SetTitle("clock_diff");

    TH1F *intt_mbd_bco_postsync_1D = new TH1F("intt_mbd_bco_postsync_1D", "INTT - MBD", 100, -10, 100000);
    intt_mbd_bco_postsync_1D->GetXaxis()->SetTitle("clock_diff");
    intt_mbd_bco_postsync_1D->GetYaxis()->SetTitle("entry");

    int prev_mbdclk = 0;
    int prev_inttbcofull = 0;

    int mbd_evt_offset = 0;
    int intt_evt_offset = 0;

    std::cout << "--> start to synchronize the events" << std::endl;
    for (int entry = 0; entry < run_event; entry++)
    {
        t_mbd->GetEntry(entry + mbd_evt_offset);
        t_intt->GetEntry(entry + intt_evt_offset);

        int mbdclk = femclk;
        int inttbcofull = INTT_BCO;
        int inttbco16bit = inttbcofull & 0xFFFF;
        int mbd_prvdiff = (mbdclk - prev_mbdclk) & 0xFFFF;
        int intt_prvdiff = inttbcofull - prev_inttbcofull;

        if (entry % 1000 == 0)
        {
            std::cout << std::dec << entry << " mbdclk=" << mbdclk << " inttbco16bit=" << inttbco16bit << " (mbd-intt) " << ((mbdclk - inttbco16bit) & 0xFFFF) << " (MBD, femclk-prev) " << mbd_prvdiff
                      << " (INTT, bco-prev) " << intt_prvdiff << " intt_evt_offset " << intt_evt_offset << std::endl;
        }

        intt_mbd_bco_presync->Fill(entry, (mbdclk - inttbco16bit) & 0xFFFF);
        intt_mbd_bco_presync_1D->Fill((mbdclk - inttbco16bit) & 0xFFFF);

        prev_mbdclk = mbdclk;
        prev_inttbcofull = inttbcofull;

        bool is_synced = false;

        while (entry + 1 + intt_evt_offset < t_intt_Nevt)
        {
            t_intt->GetEntry(entry + 1 + intt_evt_offset);
            int next_inttbco16bit = INTT_BCO & 0xFFFF;

            t_mbd->GetEntry(entry + 1);
            int next_mbdclk = femclk;

            if (((next_mbdclk - next_inttbco16bit) & 0xFFFF) != ((mbdclk - inttbco16bit) & 0xFFFF))
            {
                std::cout << "Not synced: intt entry(entry+1+intt_evt_offset)=" << entry + 1 + intt_evt_offset << ", mbd entry(entry+1)=" << entry + 1 << ", intt_evt_offset=" << intt_evt_offset
                          << ", ((next_mbdclk - next_inttbco16bit) & 0xFFFF)=" << ((next_mbdclk - next_inttbco16bit) & 0xFFFF)
                          << ", ((mbdclk - inttbco16bit) & 0xFFFF)=" << ((mbdclk - inttbco16bit) & 0xFFFF)
                          << ", ((next_mbdclk - next_inttbco16bit) & 0xFFFF)-((mbdclk - inttbco16bit) & 0xFFFF)=" << ((next_mbdclk - next_inttbco16bit) & 0xFFFF) - ((mbdclk - inttbco16bit) & 0xFFFF)
                          << std::endl;

                intt_evt_offset += 1;
            }
            else
            {
                is_synced = true;
                break;
            }
        }

        if (is_synced)
        {
            intt_mbd_bco_postsync->Fill(entry, (mbdclk - inttbco16bit) & 0xFFFF);
            intt_mbd_bco_postsync_1D->Fill((mbdclk - inttbco16bit) & 0xFFFF);
            t_intt_clone->Fill();
            t_mbd_clone->Fill();
        }
    }

    std::cout << "--> INTT Nevent post-sync: " << t_intt_clone->GetEntries() << std::endl << "--> MBD Nevent post-sync: " << t_mbd_clone->GetEntries() << std::endl;

    merge_trees(outfile, t_intt_clone, t_mbd_clone);

    intt_mbd_bco_presync->Write("", TObject::kWriteDelete);
    intt_mbd_bco_presync_1D->Write("", TObject::kWriteDelete);
    intt_mbd_bco_postsync->Write("", TObject::kWriteDelete);
    intt_mbd_bco_postsync_1D->Write("", TObject::kWriteDelete);
    f_intt->Close();
    f_mbd->Close();
    outfile->Close();

    std::cout << "--> Synchronization done" << std::endl
              << "--> Merged file:" << outputfname << ", Tree name: " << t_intt_name << std::endl
              << "--> TH2F plot: intt_mbd_bco_pre/postsync, can be checked in the merged file" << std::endl
              << "--> TH1F plot: intt_mbd_bco_pre/postsync_1D, can be checked in the merged file" << std::endl;
}

void intt_mbd_evt_combiner(int Nevent = -1,                         //
                           const char *outputfname = "output.root", //
                           const char *f_intt_name = "intt.root",   //
                           const char *t_intt_name = "EventTree",   //
                           const char *f_mbd_name = "mbd.root",     //
                           const char *t_mbd_name = "EventTree")
{
    gBenchmark->Start("InttMbdEvtCombiner");

    sync_mbd_intt_DST(outputfname, f_mbd_name, t_mbd_name, f_intt_name, t_intt_name, Nevent);

    gBenchmark->Show("InttMbdEvtCombiner");
}
