//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Sat Oct 21 04:13:40 2023 by ROOT version 6.26/06
// from TTree EventTree/EventTree
// found on file: INTTRecoClusters_test_400_0.root
//////////////////////////////////////////////////////////

#ifndef INTTDSTchain_h
#define INTTDSTchain_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include "vector"

class INTTDSTchain {
public :
   TChain          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           event;
   Int_t           NTruthVtx;
   vector<float>   *TruthPV_x;
   vector<float>   *TruthPV_y;
   vector<float>   *TruthPV_z;
   vector<int>     *TruthPV_Npart;
   vector<int>     *TruthPV_Nhits;
   vector<int>     *TruthPV_NClus;
   vector<float>   *TruthPV_t;
   vector<float>   *TruthPV_embed;
   Float_t         TruthPV_trig_x;
   Float_t         TruthPV_trig_y;
   Float_t         TruthPV_trig_z;
   Int_t           TruthPV_trig_Npart;

   Float_t centrality_bimp;
   Float_t centrality_mbd;  
   Float_t centrality_mbdquantity;

   // vector<float>   *UniqueAncG4P_Px;
   // vector<float>   *UniqueAncG4P_Py;
   // vector<float>   *UniqueAncG4P_Pz;
   // vector<float>   *UniqueAncG4P_Pt;
   vector<float>   *UniqueAncG4P_Eta;
   vector<float>   *UniqueAncG4P_Phi;
   // vector<float>   *UniqueAncG4P_E;
   vector<int>     *UniqueAncG4P_PID;
   // vector<int>     *UniqueAncG4P_TrackPID;
   // vector<int>     *UniqueAncG4P_VtxPID;
   // vector<int>     *UniqueAncG4P_ParentPID;
   // vector<int>     *UniqueAncG4P_PrimaryPID;
   // vector<double>  *UniqueAncG4P_IonCharge;
   vector<int>     *UniqueAncG4P_TrackID;
   // vector<int>     *UniqueAncG4P_NClus;
   Int_t           Layer1_occupancy;
   Int_t           NClus;
   Int_t           NTrkrhits;
   vector<int>     *ClusLayer;
   vector<float>   *ClusX;
   vector<float>   *ClusY;
   vector<float>   *ClusZ;
   vector<float>   *ClusR;
   vector<float>   *ClusPhi;
   vector<float>   *ClusEta;
   vector<unsigned int> *ClusAdc;
   vector<float>   *ClusPhiSize;
   vector<float>   *ClusZSize;
   vector<unsigned char> *ClusLadderZId;
   vector<unsigned char> *ClusLadderPhiId;

   // List of branches
   TBranch        *b_event;   //!
   TBranch        *b_NTruthVtx;   //!
   TBranch        *b_TruthPV_x;   //!
   TBranch        *b_TruthPV_y;   //!
   TBranch        *b_TruthPV_z;   //!
   TBranch        *b_TruthPV_Npart;   //!
   TBranch        *b_TruthPV_Nhits;   //!
   TBranch        *b_TruthPV_NClus;   //!
   TBranch        *b_TruthPV_t;   //!
   TBranch        *b_TruthPV_embed;   //!
   TBranch        *b_TruthPV_trig_x;   //!
   TBranch        *b_TruthPV_trig_y;   //!
   TBranch        *b_TruthPV_trig_z;   //!
   TBranch        *b_TruthPV_trig_Npart;   //!
   // TBranch        *b_UniqueAncG4P_Px;   //!
   // TBranch        *b_UniqueAncG4P_Py;   //!
   // TBranch        *b_UniqueAncG4P_Pz;   //!
   // TBranch        *b_UniqueAncG4P_Pt;   //!
   TBranch        *b_UniqueAncG4P_Eta;   //!
   TBranch        *b_UniqueAncG4P_Phi;   //!
   // TBranch        *b_UniqueAncG4P_E;   //!
   TBranch        *b_UniqueAncG4P_PID;   //!
   // TBranch        *b_UniqueAncG4P_TrackPID;   //!
   // TBranch        *b_UniqueAncG4P_VtxPID;   //!
   // TBranch        *b_UniqueAncG4P_ParentPID;   //!
   // TBranch        *b_UniqueAncG4P_PrimaryPID;   //!
   // TBranch        *b_UniqueAncG4P_IonCharge;   //!
   TBranch        *b_UniqueAncG4P_TrackID;   //!
   // TBranch        *b_UniqueAncG4P_NClus;   //!
   TBranch        *b_Layer1_occupancy;   //!
   TBranch        *b_NClus;   //!
   TBranch        *b_NTrkrhits;   //!
   TBranch        *b_ClusLayer;   //!
   TBranch        *b_ClusX;   //!
   TBranch        *b_ClusY;   //!
   TBranch        *b_ClusZ;   //!
   TBranch        *b_ClusR;   //!
   TBranch        *b_ClusPhi;   //!
   TBranch        *b_ClusEta;   //!
   TBranch        *b_ClusAdc;   //!
   TBranch        *b_ClusPhiSize;   //!
   TBranch        *b_ClusZSize;   //!
   TBranch        *b_ClusLadderZId;   //!
   TBranch        *b_ClusLadderPhiId;   //!

   TBranch        *b_centrality_bimp;
   TBranch        *b_centrality_mbd;
   TBranch        *b_centrality_mbdquantity;

   INTTDSTchain(TChain *chain_in = 0, string folder_direction = "", vector<string> file_list = {});
   INTTDSTchain(TChain *chain_in = 0, string folder_direction = "", string MC_list_name = "");
   virtual ~INTTDSTchain();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TChain *chain_in);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
private : 
   string folder_direction;
   string MC_list_name;
   vector<string> file_list;
   void read_list();
};

#endif

#ifdef INTTDSTchain_cxx
INTTDSTchain::INTTDSTchain(TChain *chain_in, string folder_direction, vector<string> file_list) : 
fChain(0), 
folder_direction(folder_direction), 
file_list(file_list) 
{
   // if parameter tree is not specified (or zero), connect the file
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
   if (file_list.size() == 0) {std::cout<<"There is no file_list input ?, run exit"<<std::endl; std::exit(1);}

   // std::cout<<"correct input format"<<std::endl; // note : was used
   // std::cout<<"file_list size : "<<file_list.size()<<std::endl; // note : was used

   Init(chain_in);
}

INTTDSTchain::INTTDSTchain(TChain *chain_in, string folder_direction, string MC_list_name) : 
fChain(0), 
folder_direction(folder_direction), 
MC_list_name(MC_list_name) 
{
   // if parameter tree is not specified (or zero), connect the file
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

   // cout<<"the input list : "<<MC_list_name<<endl;

   read_list(); // note : read the list

   // std::cout<<"correct input format"<<std::endl; // note : was used 
   // std::cout<<"Read the list by class"<<std::endl; // note : was used 
   // std::cout<<"file_list size : "<<file_list.size()<<std::endl; // note : was used 

   Init(chain_in);
}

void INTTDSTchain::read_list()
{
    string list_buffer;
    ifstream data_list;
	data_list.open((folder_direction + "/" + MC_list_name).c_str());

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
	// cout<<"size : "<<file_list.size()<<endl; // note : was used 
}

INTTDSTchain::~INTTDSTchain()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t INTTDSTchain::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t INTTDSTchain::LoadTree(Long64_t entry)
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

void INTTDSTchain::Init(TChain *chain_in)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   TruthPV_x = 0;
   TruthPV_y = 0;
   TruthPV_z = 0;
   TruthPV_Npart = 0;
   TruthPV_Nhits = 0;
   TruthPV_NClus = 0;
   TruthPV_t = 0;
   TruthPV_embed = 0;
   // UniqueAncG4P_Px = 0;
   // UniqueAncG4P_Py = 0;
   // UniqueAncG4P_Pz = 0;
   // UniqueAncG4P_Pt = 0;
   UniqueAncG4P_Eta = 0;
   UniqueAncG4P_Phi = 0;
   // UniqueAncG4P_E = 0;
   UniqueAncG4P_PID = 0;
   // UniqueAncG4P_TrackPID = 0;
   // UniqueAncG4P_VtxPID = 0;
   // UniqueAncG4P_ParentPID = 0;
   // UniqueAncG4P_PrimaryPID = 0;
   // UniqueAncG4P_IonCharge = 0;
   UniqueAncG4P_TrackID = 0;
   // UniqueAncG4P_NClus = 0;
   ClusLayer = 0;
   ClusX = 0;
   ClusY = 0;
   ClusZ = 0;
   ClusR = 0;
   ClusPhi = 0;
   ClusEta = 0;
   ClusAdc = 0;
   ClusPhiSize = 0;
   ClusZSize = 0;
   ClusLadderZId = 0;
   ClusLadderPhiId = 0;

   centrality_bimp = 0;
   centrality_mbd = 0;
   centrality_mbdquantity = 0;

   // Set branch addresses and branch pointers
   if (!chain_in) return;
   fChain = chain_in;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   // note : fill the root files
   for (auto each_file : file_list){ fChain -> Add((folder_direction + "/" + each_file).c_str()); }

   fChain -> SetBranchStatus("*",0);
   fChain -> SetBranchStatus("event",1);
   fChain -> SetBranchStatus("NTruthVtx",1);
   // fChain -> SetBranchStatus("TruthPV_x",1);
   // fChain -> SetBranchStatus("TruthPV_y",1);
   // fChain -> SetBranchStatus("TruthPV_z",1);
   // fChain -> SetBranchStatus("TruthPV_Npart",1);
   // fChain -> SetBranchStatus("TruthPV_Nhits",1);
   // fChain -> SetBranchStatus("TruthPV_NClus",1);
   // fChain -> SetBranchStatus("TruthPV_t",1);
   // fChain -> SetBranchStatus("TruthPV_embed",1);
   fChain -> SetBranchStatus("TruthPV_trig_x",1);  
   fChain -> SetBranchStatus("TruthPV_trig_y",1);  
   fChain -> SetBranchStatus("TruthPV_trig_z",1);  
   // fChain -> SetBranchStatus("TruthPV_trig_Npart",1); 
   fChain -> SetBranchStatus("ClusLayer",1);
   fChain -> SetBranchStatus("ClusX",1);
   fChain -> SetBranchStatus("ClusY",1);
   fChain -> SetBranchStatus("ClusZ",1);
   fChain -> SetBranchStatus("ClusR",1);
   fChain -> SetBranchStatus("ClusPhi",1);
   fChain -> SetBranchStatus("ClusEta",1);
   fChain -> SetBranchStatus("ClusAdc",1);
   fChain -> SetBranchStatus("ClusPhiSize",1);
   fChain -> SetBranchStatus("ClusZSize",1);
   fChain -> SetBranchStatus("ClusLadderZId",1);
   fChain -> SetBranchStatus("ClusLadderPhiId",1);
   // fChain -> SetBranchStatus("Layer1_occupancy",1); 
   fChain -> SetBranchStatus("NClus",1);           
   fChain -> SetBranchStatus("NTrkrhits",1);

   // fChain -> SetBranchStatus("UniqueAncG4P_TrackID", 1);
   fChain -> SetBranchStatus("UniqueAncG4P_Eta", 1);
   fChain -> SetBranchStatus("UniqueAncG4P_Phi", 1);
   fChain -> SetBranchStatus("centrality_bimp",1);
   fChain -> SetBranchStatus("centrality_mbd",1);
   fChain -> SetBranchStatus("centrality_mbdquantity",1);

   fChain->SetBranchAddress("event", &event, &b_event);
   fChain->SetBranchAddress("NTruthVtx", &NTruthVtx, &b_NTruthVtx);
   // fChain->SetBranchAddress("TruthPV_x", &TruthPV_x, &b_TruthPV_x);
   // fChain->SetBranchAddress("TruthPV_y", &TruthPV_y, &b_TruthPV_y);
   // fChain->SetBranchAddress("TruthPV_z", &TruthPV_z, &b_TruthPV_z);
   // fChain->SetBranchAddress("TruthPV_Npart", &TruthPV_Npart, &b_TruthPV_Npart);
   // fChain->SetBranchAddress("TruthPV_Nhits", &TruthPV_Nhits, &b_TruthPV_Nhits);
   // fChain->SetBranchAddress("TruthPV_NClus", &TruthPV_NClus, &b_TruthPV_NClus);
   // fChain->SetBranchAddress("TruthPV_t", &TruthPV_t, &b_TruthPV_t);
   // fChain->SetBranchAddress("TruthPV_embed", &TruthPV_embed, &b_TruthPV_embed);
   fChain->SetBranchAddress("TruthPV_trig_x", &TruthPV_trig_x, &b_TruthPV_trig_x);
   fChain->SetBranchAddress("TruthPV_trig_y", &TruthPV_trig_y, &b_TruthPV_trig_y);
   fChain->SetBranchAddress("TruthPV_trig_z", &TruthPV_trig_z, &b_TruthPV_trig_z);
   // fChain->SetBranchAddress("TruthPV_trig_Npart", &TruthPV_trig_Npart, &b_TruthPV_trig_Npart);
   // fChain->SetBranchAddress("UniqueAncG4P_Px", &UniqueAncG4P_Px, &b_UniqueAncG4P_Px);
   // fChain->SetBranchAddress("UniqueAncG4P_Py", &UniqueAncG4P_Py, &b_UniqueAncG4P_Py);
   // fChain->SetBranchAddress("UniqueAncG4P_Pz", &UniqueAncG4P_Pz, &b_UniqueAncG4P_Pz);
   // fChain->SetBranchAddress("UniqueAncG4P_Pt", &UniqueAncG4P_Pt, &b_UniqueAncG4P_Pt);
   fChain->SetBranchAddress("UniqueAncG4P_Eta", &UniqueAncG4P_Eta, &b_UniqueAncG4P_Eta);
   fChain->SetBranchAddress("UniqueAncG4P_Phi", &UniqueAncG4P_Phi, &b_UniqueAncG4P_Phi);
   // fChain->SetBranchAddress("UniqueAncG4P_E", &UniqueAncG4P_E, &b_UniqueAncG4P_E);
   fChain->SetBranchAddress("UniqueAncG4P_PID", &UniqueAncG4P_PID, &b_UniqueAncG4P_PID);
   // fChain->SetBranchAddress("UniqueAncG4P_TrackPID", &UniqueAncG4P_TrackPID, &b_UniqueAncG4P_TrackPID);
   // fChain->SetBranchAddress("UniqueAncG4P_VtxPID", &UniqueAncG4P_VtxPID, &b_UniqueAncG4P_VtxPID);
   // fChain->SetBranchAddress("UniqueAncG4P_ParentPID", &UniqueAncG4P_ParentPID, &b_UniqueAncG4P_ParentPID);
   // fChain->SetBranchAddress("UniqueAncG4P_PrimaryPID", &UniqueAncG4P_PrimaryPID, &b_UniqueAncG4P_PrimaryPID);
   // fChain->SetBranchAddress("UniqueAncG4P_IonCharge", &UniqueAncG4P_IonCharge, &b_UniqueAncG4P_IonCharge);
   // fChain->SetBranchAddress("UniqueAncG4P_TrackID", &UniqueAncG4P_TrackID, &b_UniqueAncG4P_TrackID);
   // fChain->SetBranchAddress("UniqueAncG4P_NClus", &UniqueAncG4P_NClus, &b_UniqueAncG4P_NClus);
   // fChain->SetBranchAddress("Layer1_occupancy", &Layer1_occupancy, &b_Layer1_occupancy);
   fChain->SetBranchAddress("NClus", &NClus, &b_NClus);
   fChain->SetBranchAddress("NTrkrhits", &NTrkrhits, &b_NTrkrhits);
   fChain->SetBranchAddress("ClusLayer", &ClusLayer, &b_ClusLayer);
   fChain->SetBranchAddress("ClusX", &ClusX, &b_ClusX);
   fChain->SetBranchAddress("ClusY", &ClusY, &b_ClusY);
   fChain->SetBranchAddress("ClusZ", &ClusZ, &b_ClusZ);
   fChain->SetBranchAddress("ClusR", &ClusR, &b_ClusR);
   fChain->SetBranchAddress("ClusPhi", &ClusPhi, &b_ClusPhi);
   fChain->SetBranchAddress("ClusEta", &ClusEta, &b_ClusEta);
   fChain->SetBranchAddress("ClusAdc", &ClusAdc, &b_ClusAdc);
   fChain->SetBranchAddress("ClusPhiSize", &ClusPhiSize, &b_ClusPhiSize);
   fChain->SetBranchAddress("ClusZSize", &ClusZSize, &b_ClusZSize);
   fChain->SetBranchAddress("ClusLadderZId", &ClusLadderZId, &b_ClusLadderZId);
   fChain->SetBranchAddress("ClusLadderPhiId", &ClusLadderPhiId, &b_ClusLadderPhiId);

   fChain->SetBranchAddress("centrality_bimp", &centrality_bimp, &b_centrality_bimp);
   fChain->SetBranchAddress("centrality_mbd", &centrality_mbd, &b_centrality_mbd);
   fChain->SetBranchAddress("centrality_mbdquantity", &centrality_mbdquantity, &b_centrality_mbdquantity);

   Notify();
}

Bool_t INTTDSTchain::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void INTTDSTchain::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t INTTDSTchain::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef INTTDSTchain_cxx
