//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Jan 18 00:52:36 2024 by ROOT version 6.26/06
// from TTree tree/tree
// found on file: beam_intt2-00020869-0000_event_base.root
//////////////////////////////////////////////////////////

#ifndef ReadEventBase_h
#define ReadEventBase_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include "TObject.h"

class ReadEventBase {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   static constexpr Int_t kMaxfhitArray = 5955;

   // Declaration of leaf types
 //InttEvent       *event;
   UInt_t          fUniqueID;
   UInt_t          fBits;
   Int_t           evtSeq;
   Long64_t        bco;
   Int_t           fNhits;
   Int_t           fhitArray_;
   UInt_t          fhitArray_fUniqueID[kMaxfhitArray];   //[fhitArray_]
   UInt_t          fhitArray_fBits[kMaxfhitArray];   //[fhitArray_]
   Int_t           fhitArray_pid[kMaxfhitArray];   //[fhitArray_]
   Int_t           fhitArray_adc[kMaxfhitArray];   //[fhitArray_]
   Int_t           fhitArray_ampl[kMaxfhitArray];   //[fhitArray_]
   Int_t           fhitArray_chip_id[kMaxfhitArray];   //[fhitArray_]
   Int_t           fhitArray_module[kMaxfhitArray];   //[fhitArray_]
   Int_t           fhitArray_chan_id[kMaxfhitArray];   //[fhitArray_]
   Int_t           fhitArray_bco[kMaxfhitArray];   //[fhitArray_]
   Long64_t        fhitArray_bco_full[kMaxfhitArray];   //[fhitArray_]
   Int_t           fhitArray_evt[kMaxfhitArray];   //[fhitArray_]
   Int_t           fhitArray_roc[kMaxfhitArray];   //[fhitArray_]
   Int_t           fhitArray_barrel[kMaxfhitArray];   //[fhitArray_]
   Int_t           fhitArray_layer[kMaxfhitArray];   //[fhitArray_]
   Int_t           fhitArray_ladder[kMaxfhitArray];   //[fhitArray_]
   Int_t           fhitArray_arm[kMaxfhitArray];   //[fhitArray_]
   Int_t           fhitArray_full_fphx[kMaxfhitArray];   //[fhitArray_]
   Int_t           fhitArray_full_roc[kMaxfhitArray];   //[fhitArray_]

   // List of branches
   TBranch        *b_event_fUniqueID;   //!
   TBranch        *b_event_fBits;   //!
   TBranch        *b_event_evtSeq;   //!
   TBranch        *b_event_bco;   //!
   TBranch        *b_event_fNhits;   //!
   TBranch        *b_event_fhitArray_;   //!
   TBranch        *b_fhitArray_fUniqueID;   //!
   TBranch        *b_fhitArray_fBits;   //!
   TBranch        *b_fhitArray_pid;   //!
   TBranch        *b_fhitArray_adc;   //!
   TBranch        *b_fhitArray_ampl;   //!
   TBranch        *b_fhitArray_chip_id;   //!
   TBranch        *b_fhitArray_module;   //!
   TBranch        *b_fhitArray_chan_id;   //!
   TBranch        *b_fhitArray_bco;   //!
   TBranch        *b_fhitArray_bco_full;   //!
   TBranch        *b_fhitArray_evt;   //!
   TBranch        *b_fhitArray_roc;   //!
   TBranch        *b_fhitArray_barrel;   //!
   TBranch        *b_fhitArray_layer;   //!
   TBranch        *b_fhitArray_ladder;   //!
   TBranch        *b_fhitArray_arm;   //!
   TBranch        *b_fhitArray_full_fphx;   //!
   TBranch        *b_fhitArray_full_roc;   //!

   ReadEventBase(TTree *tree=0);
   virtual ~ReadEventBase();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef ReadEventBase_cxx
ReadEventBase::ReadEventBase(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("beam_intt2-00020869-0000_event_base.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("beam_intt2-00020869-0000_event_base.root");
      }
      f->GetObject("tree",tree);

   }
   Init(tree);
}

ReadEventBase::~ReadEventBase()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t ReadEventBase::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t ReadEventBase::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void ReadEventBase::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain -> SetBranchStatus("*",0);
   fChain -> SetBranchStatus("fNhits",1);
   fChain -> SetBranchStatus("fhitArray.chan_id",1);

   // fChain->SetBranchAddress("fUniqueID", &fUniqueID, &b_event_fUniqueID);
   // fChain->SetBranchAddress("fBits", &fBits, &b_event_fBits);
   // fChain->SetBranchAddress("evtSeq", &evtSeq, &b_event_evtSeq);
   // fChain->SetBranchAddress("bco", &bco, &b_event_bco);
   fChain->SetBranchAddress("fNhits", &fNhits, &b_event_fNhits);
   fChain->GetEntry(0);
   // fChain->SetBranchAddress("fhitArray", &fhitArray_, &b_event_fhitArray_);
   // fChain->SetBranchAddress("fhitArray.fUniqueID", fhitArray_fUniqueID, &b_fhitArray_fUniqueID);
   // fChain->SetBranchAddress("fhitArray.fBits", fhitArray_fBits, &b_fhitArray_fBits);
   // fChain->SetBranchAddress("fhitArray.pid", fhitArray_pid, &b_fhitArray_pid);
   // fChain->SetBranchAddress("fhitArray.adc", fhitArray_adc, &b_fhitArray_adc);
   // fChain->SetBranchAddress("fhitArray.ampl", fhitArray_ampl, &b_fhitArray_ampl);
   // fChain->SetBranchAddress("fhitArray.chip_id", fhitArray_chip_id, &b_fhitArray_chip_id);
   // fChain->SetBranchAddress("fhitArray.module", fhitArray_module, &b_fhitArray_module);
   fChain->SetBranchAddress("fhitArray.chan_id", &fhitArray_chan_id[0], &b_fhitArray_chan_id);
   // fChain->SetBranchAddress("fhitArray.bco", fhitArray_bco, &b_fhitArray_bco);
   // fChain->SetBranchAddress("fhitArray.bco_full", fhitArray_bco_full, &b_fhitArray_bco_full);
   // fChain->SetBranchAddress("fhitArray.evt", fhitArray_evt, &b_fhitArray_evt);
   // fChain->SetBranchAddress("fhitArray.roc", fhitArray_roc, &b_fhitArray_roc);
   // fChain->SetBranchAddress("fhitArray.barrel", fhitArray_barrel, &b_fhitArray_barrel);
   // fChain->SetBranchAddress("fhitArray.layer", fhitArray_layer, &b_fhitArray_layer);
   // fChain->SetBranchAddress("fhitArray.ladder", fhitArray_ladder, &b_fhitArray_ladder);
   // fChain->SetBranchAddress("fhitArray.arm", fhitArray_arm, &b_fhitArray_arm);
   // fChain->SetBranchAddress("fhitArray.full_fphx", fhitArray_full_fphx, &b_fhitArray_full_fphx);
   // fChain->SetBranchAddress("fhitArray.full_roc", fhitArray_full_roc, &b_fhitArray_full_roc);
   Notify();
}

Bool_t ReadEventBase::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void ReadEventBase::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t ReadEventBase::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef ReadEventBase_cxx
