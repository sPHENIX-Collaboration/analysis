#include "ReadEvtXY_Chain.h"

ReadEvtXY_Chain::ReadEvtXY_Chain(string mother_folder_directory, string input_file_list): 
fChain(0),
mother_folder_directory(mother_folder_directory),
input_file_list(input_file_list)
{
   read_list();
   std::cout<<" file list read, number of files : "<<file_list.size()<<std::endl;

   TChain * chain = new TChain("tree","");
   for (string file_name : file_list){
      chain->Add(Form("%s/%s", mother_folder_directory.c_str(), file_name.c_str()));
   }
   
   TTree * tree = chain;

   Init(tree);
}

ReadEvtXY_Chain::~ReadEvtXY_Chain()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t ReadEvtXY_Chain::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t ReadEvtXY_Chain::LoadTree(Long64_t entry)
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

void ReadEvtXY_Chain::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   reco_vtx_x = 0;
   reco_vtx_y = 0;
   reco_vtx_x_stddev = 0;
   reco_vtx_y_stddev = 0;
   binwidth_x = 0;
   binwidth_y = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("eID", &eID, &b_eID);
   fChain->SetBranchAddress("NClus", &NClus, &b_NClus);
   fChain->SetBranchAddress("bco_full", &bco_full, &b_bco_full);
   fChain->SetBranchAddress("true_vtx_x", &true_vtx_x, &b_true_vtx_x);
   fChain->SetBranchAddress("true_vtx_y", &true_vtx_y, &b_true_vtx_y);
   fChain->SetBranchAddress("true_vtx_z", &true_vtx_z, &b_true_vtx_z);
   fChain->SetBranchAddress("reco_vtx_x", &reco_vtx_x, &b_reco_vtx_x);
   fChain->SetBranchAddress("reco_vtx_y", &reco_vtx_y, &b_reco_vtx_y);
   fChain->SetBranchAddress("reco_vtx_z", &reco_vtx_z, &b_reco_vtx_z);
   fChain->SetBranchAddress("reco_vtx_z_width", &reco_vtx_z_width, &b_reco_vtx_z_width);
   fChain->SetBranchAddress("reco_vtx_x_stddev", &reco_vtx_x_stddev, &b_reco_vtx_x_stddev);
   fChain->SetBranchAddress("reco_vtx_y_stddev", &reco_vtx_y_stddev, &b_reco_vtx_y_stddev);
   fChain->SetBranchAddress("binwidth_x", &binwidth_x, &b_binwidth_x);
   fChain->SetBranchAddress("binwidth_y", &binwidth_y, &b_binwidth_y);
   Notify();
}

Bool_t ReadEvtXY_Chain::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

Long64_t ReadEvtXY_Chain::GetEntries()
{
   return fChain->GetEntries();
}

void ReadEvtXY_Chain::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t ReadEvtXY_Chain::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}

void ReadEvtXY_Chain::read_list()
{
   string list_buffer;
   ifstream data_list;
	data_list.open( Form("%s/%s", mother_folder_directory.c_str(), input_file_list.c_str()) );

   file_list.clear();

	while (1)
	{
		data_list >> list_buffer;
		if (!data_list.good())
		{
			break;
		}
		file_list.push_back(list_buffer);
	}
	cout<<"size : "<<file_list.size()<<endl;
}

void ReadEvtXY_Chain::Loop()
{
//   In a ROOT session, you can do:
//      root> .L ReadEvtXY_Chain.C
//      root> ReadEvtXY_Chain t
//      root> t.GetEntry(12); // Fill t data members with entry number 12
//      root> t.Show();       // Show values of entry 12
//      root> t.Show(16);     // Read and show values of entry 16
//      root> t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;
   }
}
