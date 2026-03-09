//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Sat Oct 28 06:25:34 2023 by ROOT version 6.26/06
// from TTree tree_clu/clustering info.
// found on file: beam_inttall-00020869-0000_event_base_ana_cluster_full_survey_3.32_excludeR40000_200kEvent_3HotCut.root
//////////////////////////////////////////////////////////

#ifndef PrivateCluReader_h
#define PrivateCluReader_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// note : Header file for the classes stored in the TTree if any.
#include "vector"

class PrivateCluReader {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // note :  Fixed size dimensions of array or collections stored in the TTree if any.

   // note : Declaration of leaf types
   Int_t           nhits;
   Int_t           nclu_inner;
   Int_t           nclu_outer;
   Long64_t        bco_full;
   vector<int>     *column;
   vector<double>  *avg_chan;
   vector<int>     *sum_adc;
   vector<int>     *sum_adc_conv;
   vector<int>     *size;
   vector<double>  *x;
   vector<double>  *y;
   vector<double>  *z;
   vector<int>     *layer;
   vector<double>  *phi;
   // vector<vector<double> > *bco_diff_vec;
   vector<int>     *server;
   vector<int>     *module;

   // note : List of branches
   TBranch        *b_nhits;   //!
   TBranch        *b_nclu_inner;   //!
   TBranch        *b_nclu_outer;   //!
   TBranch        *b_bco_full;   //!
   TBranch        *b_column;   //!
   TBranch        *b_avg_chan;   //!
   TBranch        *b_sum_adc;   //!
   TBranch        *b_sum_adc_conv;   //!
   TBranch        *b_size;   //!
   TBranch        *b_x;   //!
   TBranch        *b_y;   //!
   TBranch        *b_z;   //!
   TBranch        *b_layer;   //!
   TBranch        *b_phi;   //!
   // TBranch        *b_bco_diff_vec;   //!
   TBranch        *b_server;   //!
   TBranch        *b_module;   //!

   PrivateCluReader(TTree *tree=0);
   virtual ~PrivateCluReader();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef PrivateCluReader_cxx
PrivateCluReader::PrivateCluReader(TTree *tree) : fChain(0) 
{
   // note : if parameter tree is not specified (or zero), connect the file
   // note : used to generate this class and read the Tree.
   if (tree == 0) {
      // TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("beam_inttall-00020869-0000_event_base_ana_cluster_full_survey_3.32_excludeR40000_200kEvent_3HotCut.root");
      // if (!f || !f->IsOpen()) {
      //    f = new TFile("beam_inttall-00020869-0000_event_base_ana_cluster_full_survey_3.32_excludeR40000_200kEvent_3HotCut.root");
      // }
      // f->GetObject("tree_clu",tree);

      cout<<"Class PrivateCluReader, No tree input, exit "<<endl;
      exit(1);
   }


   Init(tree);
}

PrivateCluReader::~PrivateCluReader()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t PrivateCluReader::GetEntry(Long64_t entry)
{
   // note : Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t PrivateCluReader::LoadTree(Long64_t entry)
{
   // note : Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void PrivateCluReader::Init(TTree *tree)
{
   // note : The Init() function is called when the selector needs to initialize
   // note : a new tree or chain. Typically here the branch addresses and branch
   // note : pointers of the tree will be set.
   // note : It is normally not necessary to make changes to the generated
   // note : code, but the routine can be extended by the user if needed.
   // note : Init() will be called many times when running on PROOF
   // note : (once per file to be processed).

   // note : Set object pointer
   column = 0;
   avg_chan = 0;
   sum_adc = 0;
   sum_adc_conv = 0;
   size = 0;
   x = 0;
   y = 0;
   z = 0;
   layer = 0;
   phi = 0;
   // bco_diff_vec = 0;
   server = 0;
   module = 0;
   // note : Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain -> SetBranchStatus("*",0);
   fChain -> SetBranchStatus("nhits",1);
   fChain -> SetBranchStatus("nclu_inner",1);
   fChain -> SetBranchStatus("nclu_outer",1);
   fChain -> SetBranchStatus("bco_full",1);
   fChain -> SetBranchStatus("column",1);
   fChain -> SetBranchStatus("avg_chan",1);
   fChain -> SetBranchStatus("sum_adc",1);
   fChain -> SetBranchStatus("sum_adc_conv",1);
   fChain -> SetBranchStatus("size",1);
   fChain -> SetBranchStatus("x",1);
   fChain -> SetBranchStatus("y",1);
   fChain -> SetBranchStatus("z",1);
   fChain -> SetBranchStatus("layer",1);
   fChain -> SetBranchStatus("phi",1);
   fChain -> SetBranchStatus("server", 1);
   fChain -> SetBranchStatus("module", 1);

   fChain->SetBranchAddress("nhits", &nhits, &b_nhits);
   fChain->SetBranchAddress("nclu_inner", &nclu_inner, &b_nclu_inner);
   fChain->SetBranchAddress("nclu_outer", &nclu_outer, &b_nclu_outer);
   fChain->SetBranchAddress("bco_full", &bco_full, &b_bco_full);
   fChain->SetBranchAddress("column", &column, &b_column);
   fChain->SetBranchAddress("avg_chan", &avg_chan, &b_avg_chan);
   fChain->SetBranchAddress("sum_adc", &sum_adc, &b_sum_adc);
   fChain->SetBranchAddress("sum_adc_conv", &sum_adc_conv, &b_sum_adc_conv);
   fChain->SetBranchAddress("size", &size, &b_size);
   fChain->SetBranchAddress("x", &x, &b_x);
   fChain->SetBranchAddress("y", &y, &b_y);
   fChain->SetBranchAddress("z", &z, &b_z);
   fChain->SetBranchAddress("layer", &layer, &b_layer);
   fChain->SetBranchAddress("phi", &phi, &b_phi);
   // fChain->SetBranchAddress("bco_diff_vec", &bco_diff_vec, &b_bco_diff_vec);
   fChain->SetBranchAddress("server", &server, &b_server);
   fChain->SetBranchAddress("module", &module, &b_module);
   Notify();
}

Bool_t PrivateCluReader::Notify()
{
   // note : The Notify() function is called when a new file is opened. This
   // note : can be either for a new TTree in a TChain or when when a new TTree
   // note : is started when using PROOF. It is normally not necessary to make changes
   // note : to the generated code, but the routine can be extended by the
   // note : user if needed. The return value is currently not used.

   return kTRUE;
}

void PrivateCluReader::Show(Long64_t entry)
{
   // note : Print contents of entry.
   // note : If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t PrivateCluReader::Cut(Long64_t entry)
{
   // note : This function may be called from Loop.
   // note : returns  1 if entry is accepted.
   // note : returns -1 otherwise.
   return 1;
}
#endif // note :  #ifdef PrivateCluReader_cxx
