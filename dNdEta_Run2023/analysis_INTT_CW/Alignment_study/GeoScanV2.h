//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Jan 18 21:15:42 2024 by ROOT version 6.26/06
// from TTree tree_geo_scan/tree_geo_scan
// found on file: geo_scan_212_13_3186500.root
//////////////////////////////////////////////////////////

#ifndef GeoScanV2_h
#define GeoScanV2_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include "vector"

class GeoScanV2 {
public :
   TChain          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   vector<double>  *offset_x_vec;
   vector<double>  *offset_y_vec;
   Double_t        DCA_inner_fitYpos;
   Double_t        DCA_inner_fitE;
   Double_t        angle_diff_inner_fitYpos;
   Double_t        angle_diff_inner_fitE;
   Double_t        DCA_outer_fitYpos;
   Double_t        DCA_outer_fitE;
   Double_t        angle_diff_outer_fitYpos;
   Double_t        angle_diff_outer_fitE;
   Int_t           random_seed;
   Double_t        vtxX;
   Double_t        vtxY;
   Double_t        trial_originX;
   Double_t        trial_originY;

   // List of branches
   TBranch        *b_offset_x_vec;   //!
   TBranch        *b_offset_y_vec;   //!
   TBranch        *b_DCA_inner_fitYpos;   //!
   TBranch        *b_DCA_inner_fitE;   //!
   TBranch        *b_angle_diff_inner_fitYpos;   //!
   TBranch        *b_angle_diff_inner_fitE;   //!
   TBranch        *b_DCA_outer_fitYpos;   //!
   TBranch        *b_DCA_outer_fitE;   //!
   TBranch        *b_angle_diff_outer_fitYpos;   //!
   TBranch        *b_angle_diff_outer_fitE;   //!
   TBranch        *b_random_seed;   //!
   TBranch        *b_vtxX;   //!
   TBranch        *b_vtxY;   //!
   TBranch        *b_trial_originX;   //!
   TBranch        *b_trial_originY;   //!

   vector<string> file_list;

   GeoScanV2(TChain *chain_in = 0, string folder_direction = "", string MC_list_name = "", int Nfile = -1);
   virtual ~GeoScanV2();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TChain *chain_in);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);

private:
   string folder_direction;
   string MC_list_name;
   int Nfile;
   void read_list();
};

#endif

#ifdef GeoScanV2_cxx
GeoScanV2::GeoScanV2(TChain *chain_in, string folder_direction, string MC_list_name, int Nfile) : 
fChain(0),
folder_direction(folder_direction), 
MC_list_name(MC_list_name),
Nfile(Nfile) 
{
   file_list.clear();
   
   // if parameter tree is not specified (o}r zero), connect the file
   // used to generate this class and read the Tree.
   if (chain_in == 0) {
      // TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("INTTRecoClusters_test_400_0.root");
      // if (!f || !f->IsOpen()) {
      //    f = new TFile("INTTRecoClusters_test_400_0.root");
      // }
      // f->GetObject("EventTree",chain_in);
      std::cout<<"There is no TChain input ?, run exit"<<std::endl;
      std::exit(1);
   }
   if (folder_direction.size() == 0) {std::cout<<"There is no folder_direction input ?, run exit"<<std::endl; std::exit(1);}
   if (MC_list_name.size() == 0) {std::cout<<"There is no MC_list_name input ?, run exit"<<std::endl; std::exit(1);}

   read_list(); // note : read the list

   std::cout<<"correct input format"<<std::endl;
   std::cout<<"Read the list by class"<<std::endl;
   std::cout<<"file_list size : "<<file_list.size()<<std::endl;

   Init(chain_in);
}

void GeoScanV2::read_list()
{
   string list_buffer;
   ifstream data_list;
	data_list.open((folder_direction + "/" + MC_list_name).c_str());

   file_list.clear();

   if (Nfile == -1){
      while (1)
      {
         data_list >> list_buffer;
         if (!data_list.good())
         {
            break;
         }
         file_list.push_back(list_buffer);
      }
   }
   else 
   {
      while (1)
      {
         data_list >> list_buffer;
         if (!data_list.good())
         {
            break;
         }
         file_list.push_back(list_buffer);

         // note : Just in case you don't want to read all the files, it takes time
         if (file_list.size() == Nfile) {break;}
      }
   }	
	cout<<"size : "<<file_list.size()<<endl;
}

GeoScanV2::~GeoScanV2()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t GeoScanV2::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t GeoScanV2::LoadTree(Long64_t entry)
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

void GeoScanV2::Init(TChain *chain_in)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   offset_x_vec = 0;
   offset_y_vec = 0;
   // Set branch addresses and branch pointers
   if (!chain_in) return;
   fChain = chain_in;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   // note : fill the root files
   for (auto each_file : file_list){ fChain -> Add((folder_direction + "/" + each_file).c_str()); }

   fChain->SetBranchAddress("offset_x_vec", &offset_x_vec, &b_offset_x_vec);
   fChain->SetBranchAddress("offset_y_vec", &offset_y_vec, &b_offset_y_vec);
   fChain->SetBranchAddress("DCA_inner_fitYpos", &DCA_inner_fitYpos, &b_DCA_inner_fitYpos);
   fChain->SetBranchAddress("DCA_inner_fitE", &DCA_inner_fitE, &b_DCA_inner_fitE);
   fChain->SetBranchAddress("angle_diff_inner_fitYpos", &angle_diff_inner_fitYpos, &b_angle_diff_inner_fitYpos);
   fChain->SetBranchAddress("angle_diff_inner_fitE", &angle_diff_inner_fitE, &b_angle_diff_inner_fitE);
   fChain->SetBranchAddress("DCA_outer_fitYpos", &DCA_outer_fitYpos, &b_DCA_outer_fitYpos);
   fChain->SetBranchAddress("DCA_outer_fitE", &DCA_outer_fitE, &b_DCA_outer_fitE);
   fChain->SetBranchAddress("angle_diff_outer_fitYpos", &angle_diff_outer_fitYpos, &b_angle_diff_outer_fitYpos);
   fChain->SetBranchAddress("angle_diff_outer_fitE", &angle_diff_outer_fitE, &b_angle_diff_outer_fitE);
   fChain->SetBranchAddress("random_seed", &random_seed, &b_random_seed);
   fChain->SetBranchAddress("vtxX", &vtxX, &b_vtxX);
   fChain->SetBranchAddress("vtxY", &vtxY, &b_vtxY);
   fChain->SetBranchAddress("trial_originX", &trial_originX, &b_trial_originX);
   fChain->SetBranchAddress("trial_originY", &trial_originY, &b_trial_originY);
   Notify();
}

Bool_t GeoScanV2::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void GeoScanV2::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t GeoScanV2::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef GeoScanV2_cxx
