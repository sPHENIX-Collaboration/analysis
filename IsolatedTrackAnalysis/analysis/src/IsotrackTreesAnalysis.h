//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Fri Jan 13 11:10:55 2023 by ROOT version 6.26/10
// from TTree tracktree/Tree of svtx tracks
// found on file: isotrack_output.root
//////////////////////////////////////////////////////////

#pragma once
#ifndef IsotrackTreesAnalysis_h
#define IsotrackTreesAnalysis_h

#include "ClusterContainer.h"
#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TEfficiency.h>
#include <TRandom3.h>
#include <iostream>
#include <fstream>
#include <string> 

// Header file for the classes stored in the TTree if any.

class IsotrackTreesAnalysis {
    private :

        ////////////////////////////
        // Input option variables //
        ////////////////////////////
       
        std::string OUTPUT_FILENAME;

        bool USE_TOWER_INFO;
        bool USE_TRUTH_INFO;
        bool USE_CENTRALITY;
        bool USE_PARTICLE_GUN;

        float CENTRALITY_CUT;

        // track cuts
        float D0_CUT; // m
        float Z0_CUT; // m
        float PT_CUT;  // GeV

        // cuts for matching to other tracks
        float MATCHED_PT_CUT; // GeV
        float MATCHED_DR_CUT;
        float MATCHED_ETA_CUT; 

        // cuts for matching to neutral truth particles
        float MATCHED_NEUTRAL_TRUTH_PT_CUT; // GeV
        float MATCHED_NEUTRAL_TRUTH_ETA_CUT;
        float MATCHED_NEUTRAL_TRUTH_DR_CUT;

        // cuts for cluster matching
        float CEMC_MATCHING_DR_CUT;
        float IHCAL_MATCHING_DR_CUT;
        float OHCAL_MATCHING_DR_CUT;

        // MIP cuts
        float CEMC_MIP_ENERGY; // GeV
        float IHCAL_MIP_ENERGY; // GeV

        /////////////
        // TChains //
        /////////////

        TChain          *fChainTracks;   //!pointer to the analyzed TTree or TChain
        TChain          *fChainClusters;
        TChain          *fChainTowers;
        TChain          *fChainSimTowers;
        TChain          *fChainHepMC;
        TChain          *fChainG4;
        TChain          *fChainCentrality;
        TChain          *fChainAddTruth;

        Int_t           fCurrent; //!current Tree number in a TChain

        // Fixed size dimensions of array or collections stored in the TTree if any.

        ///////////////////////////////////////
        // Track tree variables and branches //
        ///////////////////////////////////////

        // Declaration of leaf types
        Int_t           m_trkmult;
        Float_t         m_tr_p[2000];   //[m_trkmult]
        Float_t         m_tr_pt[2000];   //[m_trkmult]
        Float_t         m_tr_eta[2000];   //[m_trkmult]
        Float_t         m_tr_phi[2000];   //[m_trkmult]
        Int_t           m_tr_charge[2000];   //[m_trkmult]
        Float_t         m_tr_chisq[2000];   //[m_trkmult]
        Int_t           m_tr_ndf[2000];   //[m_trkmult]
        Float_t         m_tr_dca_xy[2000];   //[m_trkmult]
        Float_t         m_tr_dca_xy_error[2000];   //[m_trkmult]
        Float_t         m_tr_dca_z[2000];   //[m_trkmult]
        Float_t         m_tr_dca_z_error[2000];   //[m_trkmult]
        Float_t         m_tr_x[2000];   //[m_trkmult]
        Float_t         m_tr_y[2000];   //[m_trkmult]
        Float_t         m_tr_z[2000];   //[m_trkmult]
        Int_t           m_tr_vertex_id[2000];   //[m_trkmult]
        Int_t           m_vtxmult;
        Int_t           m_vertex_id[10];   //[m_vtxmult]
        Float_t         m_vx[10];   //[m_vtxmult]
        Float_t         m_vy[10];   //[m_vtxmult]
        Float_t         m_vz[10];   //[m_vtxmult]
        Float_t         m_tr_cemc_eta[2000];   //[m_trkmult]
        Float_t         m_tr_cemc_phi[2000];   //[m_trkmult]
        Float_t         m_tr_ihcal_eta[2000];   //[m_trkmult]
        Float_t         m_tr_ihcal_phi[2000];   //[m_trkmult]
        Float_t         m_tr_ohcal_eta[2000];   //[m_trkmult]
        Float_t         m_tr_ohcal_phi[2000];   //[m_trkmult]
        Int_t           m_tr_truth_pid[2000];   //[m_trkmult]
        Int_t           m_tr_truth_is_primary[2000];   //[m_trkmult]
        Float_t         m_tr_truth_e[2000];   //[m_trkmult]
        Float_t         m_tr_truth_pt[2000];   //[m_trkmult]
        Float_t         m_tr_truth_eta[2000];   //[m_trkmult]
        Float_t         m_tr_truth_phi[2000];   //[m_trkmult]
        Int_t           m_tr_truth_track_id[2000];

        // List of branches
        TBranch        *b_m_trkmult;   //!
        TBranch        *b_m_tr_p;   //!
        TBranch        *b_m_tr_pt;   //!
        TBranch        *b_m_tr_eta;   //!
        TBranch        *b_m_tr_phi;   //!
        TBranch        *b_m_tr_charge;   //!
        TBranch        *b_m_tr_chisq;   //!
        TBranch        *b_m_tr_ndf;   //!
        TBranch        *b_m_tr_dca_xy;   //!
        TBranch        *b_m_tr_dca_xy_error;   //!
        TBranch        *b_m_tr_dca_z;   //!
        TBranch        *b_m_tr_dca_z_error;   //!
        TBranch        *b_m_tr_x;   //!
        TBranch        *b_m_tr_y;   //!
        TBranch        *b_m_tr_z;   //!
        TBranch        *b_m_tr_vertex_id;   //!
        TBranch        *b_m_vtxmult;   //!
        TBranch        *b_m_vertex_id;   //!
        TBranch        *b_m_vx;   //!
        TBranch        *b_m_vy;   //!
        TBranch        *b_m_vz;   //!
        TBranch        *b_m_tr_cemc_eta;   //!
        TBranch        *b_m_tr_cemc_phi;   //!
        TBranch        *b_m_tr_ihcal_eta;   //!
        TBranch        *b_m_tr_ihcal_phi;   //!
        TBranch        *b_m_tr_ohcal_eta;   //!
        TBranch        *b_m_tr_ohcal_phi;   //!
        TBranch        *b_m_tr_truth_pid;   //!
        TBranch        *b_m_tr_truth_is_primary;   //!
        TBranch        *b_m_tr_truth_e;   //!
        TBranch        *b_m_tr_truth_pt;   //!
        TBranch        *b_m_tr_truth_eta;   //!
        TBranch        *b_m_tr_truth_phi;   //!
        TBranch        *b_m_tr_truth_track_id;

        /////////////////////////////////////////
        // Cluster tree variables and branches //
        /////////////////////////////////////////

        // Declaration of leaf types
        Int_t           m_clsmult_cemc;
        Float_t         m_cl_cemc_e[5000];   //[m_clsmult_cemc]
        Float_t         m_cl_cemc_eta[5000];   //[m_clsmult_cemc]
        Float_t         m_cl_cemc_phi[5000];   //[m_clsmult_cemc]
        Float_t         m_cl_cemc_r[5000];   //[m_clsmult_cemc]
        Float_t         m_cl_cemc_z[5000];   //[m_clsmult_cemc]
        Int_t           m_clsmult_ihcal;
        Float_t         m_cl_ihcal_e[1000];   //[m_clsmult_ihcal]
        Float_t         m_cl_ihcal_eta[1000];   //[m_clsmult_ihcal]
        Float_t         m_cl_ihcal_phi[1000];   //[m_clsmult_ihcal]
        Float_t         m_cl_ihcal_r[1000];   //[m_clsmult_ihcal]
        Float_t         m_cl_ihcal_z[1000];   //[m_clsmult_ihcal]
        Int_t           m_clsmult_ohcal;
        Float_t         m_cl_ohcal_e[1000];   //[m_clsmult_ohcal]
        Float_t         m_cl_ohcal_eta[1000];   //[m_clsmult_ohcal]
        Float_t         m_cl_ohcal_phi[1000];   //[m_clsmult_ohcal]
        Float_t         m_cl_ohcal_r[1000];   //[m_clsmult_ohcal]
        Float_t         m_cl_ohcal_z[1000];   //[m_clsmult_ohcal]

        // List of branches
        TBranch        *b_m_clsmult_cemc;   //!
        TBranch        *b_m_cl_cemc_e;   //!
        TBranch        *b_m_cl_cemc_eta;   //!
        TBranch        *b_m_cl_cemc_phi;   //!
        TBranch        *b_m_cl_cemc_r;   //!
        TBranch        *b_m_cl_cemc_z;   //!
        TBranch        *b_m_clsmult_ihcal;   //!
        TBranch        *b_m_cl_ihcal_e;   //!
        TBranch        *b_m_cl_ihcal_eta;   //!
        TBranch        *b_m_cl_ihcal_phi;   //!
        TBranch        *b_m_cl_ihcal_r;   //!
        TBranch        *b_m_cl_ihcal_z;   //!
        TBranch        *b_m_clsmult_ohcal;   //!
        TBranch        *b_m_cl_ohcal_e;   //!
        TBranch        *b_m_cl_ohcal_eta;   //!
        TBranch        *b_m_cl_ohcal_phi;   //!
        TBranch        *b_m_cl_ohcal_r;   //!
        TBranch        *b_m_cl_ohcal_z;   //!

        ///////////////////////////////////////
        // Tower tree variables and branches //
        ///////////////////////////////////////

        // Declaration of leaf types
        Int_t           m_twrmult_cemc;
        Float_t         m_twr_cemc_e[25000];   //[m_twrmult_cemc]
        Float_t         m_twr_cemc_eta[25000];   //[m_twrmult_cemc]
        Float_t         m_twr_cemc_phi[25000];   //[m_twrmult_cemc]
        Int_t           m_twr_cemc_ieta[25000];   //[m_twrmult_cemc]
        Int_t           m_twr_cemc_iphi[25000];   //[m_twrmult_cemc]
        Int_t           m_twrmult_ihcal;
        Float_t         m_twr_ihcal_e[2000];   //[m_twrmult_ihcal]
        Float_t         m_twr_ihcal_eta[2000];   //[m_twrmult_ihcal]
        Float_t         m_twr_ihcal_phi[2000];   //[m_twrmult_ihcal]
        Int_t           m_twr_ihcal_ieta[2000];   //[m_twrmult_ihcal]
        Int_t           m_twr_ihcal_iphi[2000];   //[m_twrmult_ihcal]
        Int_t           m_twrmult_ohcal;
        Float_t         m_twr_ohcal_e[2000];   //[m_twrmult_ohcal]
        Float_t         m_twr_ohcal_eta[2000];   //[m_twrmult_ohcal]
        Float_t         m_twr_ohcal_phi[2000];   //[m_twrmult_ohcal]
        Int_t           m_twr_ohcal_ieta[2000];   //[m_twrmult_ohcal]
        Int_t           m_twr_ohcal_iphi[2000];   //[m_twrmult_ohcal]

        // List of branches
        TBranch        *b_m_twrmult_cemc;   //!
        TBranch        *b_m_twr_cemc_e;   //!
        TBranch        *b_m_twr_cemc_eta;   //!
        TBranch        *b_m_twr_cemc_phi;   //!
        TBranch        *b_m_twr_cemc_ieta;   //!
        TBranch        *b_m_twr_cemc_iphi;   //!
        TBranch        *b_m_twrmult_ihcal;   //!
        TBranch        *b_m_twr_ihcal_e;   //!
        TBranch        *b_m_twr_ihcal_eta;   //!
        TBranch        *b_m_twr_ihcal_phi;   //!
        TBranch        *b_m_twr_ihcal_ieta;   //!
        TBranch        *b_m_twr_ihcal_iphi;   //!
        TBranch        *b_m_twrmult_ohcal;   //!
        TBranch        *b_m_twr_ohcal_e;   //!
        TBranch        *b_m_twr_ohcal_eta;   //!
        TBranch        *b_m_twr_ohcal_phi;   //!
        TBranch        *b_m_twr_ohcal_ieta;   //!
        TBranch        *b_m_twr_ohcal_iphi;   //!

        //////////////////////////////////////
        // Sim tower variables and branches //
        //////////////////////////////////////

        // Declaration of leaf types
        Int_t           m_simtwrmult_cemc;
        Float_t         m_simtwr_cemc_e[20871];   //[m_simtwrmult_cemc]
        Float_t         m_simtwr_cemc_eta[20871];   //[m_simtwrmult_cemc]
        Float_t         m_simtwr_cemc_phi[20871];   //[m_simtwrmult_cemc]
        Int_t           m_simtwr_cemc_ieta[20871];   //[m_simtwrmult_cemc]
        Int_t           m_simtwr_cemc_iphi[20871];   //[m_simtwrmult_cemc]
        Int_t           m_simtwrmult_ihcal;
        Float_t         m_simtwr_ihcal_e[1536];   //[m_simtwrmult_ihcal]
        Float_t         m_simtwr_ihcal_eta[1536];   //[m_simtwrmult_ihcal]
        Float_t         m_simtwr_ihcal_phi[1536];   //[m_simtwrmult_ihcal]
        Int_t           m_simtwr_ihcal_ieta[1536];   //[m_simtwrmult_ihcal]
        Int_t           m_simtwr_ihcal_iphi[1536];   //[m_simtwrmult_ihcal]
        Int_t           m_simtwrmult_ohcal;
        Float_t         m_simtwr_ohcal_e[1507];   //[m_simtwrmult_ohcal]
        Float_t         m_simtwr_ohcal_eta[1507];   //[m_simtwrmult_ohcal]
        Float_t         m_simtwr_ohcal_phi[1507];   //[m_simtwrmult_ohcal]
        Int_t           m_simtwr_ohcal_ieta[1507];   //[m_simtwrmult_ohcal]
        Int_t           m_simtwr_ohcal_iphi[1507];   //[m_simtwrmult_ohcal]

        // List of branches
        TBranch        *b_m_simtwrmult_cemc;   //!
        TBranch        *b_m_simtwr_cemc_e;   //!
        TBranch        *b_m_simtwr_cemc_eta;   //!
        TBranch        *b_m_simtwr_cemc_phi;   //!
        TBranch        *b_m_simtwr_cemc_ieta;   //!
        TBranch        *b_m_simtwr_cemc_iphi;   //!
        TBranch        *b_m_simtwrmult_ihcal;   //!
        TBranch        *b_m_simtwr_ihcal_e;   //!
        TBranch        *b_m_simtwr_ihcal_eta;   //!
        TBranch        *b_m_simtwr_ihcal_phi;   //!
        TBranch        *b_m_simtwr_ihcal_ieta;   //!
        TBranch        *b_m_simtwr_ihcal_iphi;   //!
        TBranch        *b_m_simtwrmult_ohcal;   //!
        TBranch        *b_m_simtwr_ohcal_e;   //!
        TBranch        *b_m_simtwr_ohcal_eta;   //!
        TBranch        *b_m_simtwr_ohcal_phi;   //!
        TBranch        *b_m_simtwr_ohcal_ieta;   //!
        TBranch        *b_m_simtwr_ohcal_iphi;   //!

        ////////////////////////////////////////
        // HepMC truth variables and branches //
        ////////////////////////////////////////

        // Declaration of leaf types
        Int_t           m_hepmc;
        Int_t           m_hepmc_pid[20000];   //[m_hepmc]
        Float_t         m_hepmc_e[20000];   //[m_hepmc]
        Float_t         m_hepmc_pt[20000];   //[m_hepmc]
        Float_t         m_hepmc_eta[20000];   //[m_hepmc]
        Float_t         m_hepmc_phi[20000];   //[m_hepmc]

        // List of branches
        TBranch        *b_m_hepmc;   //!
        TBranch        *b_m_hepmc_pid;   //!
        TBranch        *b_m_hepmc_e;   //!
        TBranch        *b_m_hepmc_pt;   //!
        TBranch        *b_m_hepmc_eta;   //!
        TBranch        *b_m_hepmc_phi;   //!

        /////////////////////////////////////
        // G4 truth variables and branches //
        /////////////////////////////////////

        // Declaration of leaf types
        Int_t           m_g4;
        Int_t           m_g4_pid[20000];   //[m_g4]
        Float_t         m_g4_e[20000];   //[m_g4]
        Float_t         m_g4_pt[20000];   //[m_g4]
        Float_t         m_g4_eta[20000];   //[m_g4]
        Float_t         m_g4_phi[20000];   //[m_g4]
        Int_t           m_g4_track_id[20000];
        Int_t           m_g4_parent_id[20000];

        // List of branches
        TBranch        *b_m_g4;   //!
        TBranch        *b_m_g4_pid;   //!
        TBranch        *b_m_g4_e;   //!
        TBranch        *b_m_g4_pt;   //!
        TBranch        *b_m_g4_eta;   //!
        TBranch        *b_m_g4_phi;   //!
        TBranch        *b_m_g4_track_id;
        TBranch        *b_m_g4_parent_id;


        ///////////////////////////////////////
        // Centrality variables and branches //
        ///////////////////////////////////////

        // Declaration of leaf types
        Float_t         centrality;

        // List of branches
        TBranch        *b_centrality;   //!


        /////////////////////////////////////////////
        // Additional Truth variables and branches //
        /////////////////////////////////////////////

        Int_t           n_child;
        Int_t           child_pid[100000];
        Int_t           child_parent_id[100000];
        Int_t           child_vertex_id[100000];
        Float_t         child_px[100000];
        Float_t         child_py[100000];
        Float_t         child_pz[100000];
        Float_t         child_energy[100000];
        Int_t           n_vertex;
        Int_t           vertex_id[100000];
        Float_t         vertex_x[100000];
        Float_t         vertex_y[100000];
        Float_t         vertex_z[100000];
        Int_t           _nBH;
        Float_t         BH_e[20000];
        Float_t         BH_px[20000];
        Float_t         BH_py[20000];
        Float_t         BH_pz[20000];
        Int_t           BH_track_id[20000];

        TBranch         *b_n_child;
        TBranch         *b_child_pid;
        TBranch         *b_child_parent_id;
        TBranch         *b_child_vertex_id;
        TBranch         *b_child_px;
        TBranch         *b_child_py;
        TBranch         *b_child_pz;
        TBranch         *b_child_energy;
        TBranch         *b_n_vertex;
        TBranch         *b_vertex_id;
        TBranch         *b_vertex_x;
        TBranch         *b_vertex_y;
        TBranch         *b_vertex_z;
        TBranch         *b__nBH;
        TBranch         *b_BH_e;
        TBranch         *b_BH_px;
        TBranch         *b_BH_py;
        TBranch         *b_BH_pz;
        TBranch         *b_BH_track_id;


        enum caloType{cemc=0, ihcal=1, ohcal=2};

        ////////////////
        // Histograms //
        ////////////////

        // EOverPModule
        TH2F* histEoverP[8];
        TH2F* histEoverPRaw[8];
        TH2F* histEoverPBkg[8];
        // TrackRatesModule
        TH1F* histTrackTotal;
        TH1F* histTrackRate[8];
        // ChecksModule
        TH1F* histTowerNumber[3];
        
        /////////////////////////////
        // Random number generator //
        /////////////////////////////
    
        TRandom3* rand = new TRandom3(123);

    public:

        IsotrackTreesAnalysis(std::string inputFilename, std::string outputFilename, bool useTowerInfo, bool useTruthInfo, bool useCentrality, bool useParticleGun, float centralityCut, float d0Cut, float z0Cut, float ptCut, float matchedPtCut, float matchedDrCut, float matchedNeutralTruthPtCut, float matchedNeutralTruthEtaCut, float matchedNeutralTruthDrCut, float cemcMatchingDrCut, float ihcalMatchingDrCut, float ohcalMatchingDrCut, float cemcMipEnergy, float ihcalMipEnergy);
        virtual ~IsotrackTreesAnalysis();
        virtual Int_t    Cut(Long64_t entry);
        virtual void    GetEntry(Long64_t entry);
        virtual Long64_t LoadTree(Long64_t entry);
        virtual void     Init();
        virtual void     Loop();
        virtual Bool_t   Notify();
        virtual void     Show(Long64_t entry = -1);

        void processEvent();
        void processTrack(int id);
        bool basicTrackSelection(int id);
        bool truthIsolatedTrackSelection(int id);

        MatchedClusterContainer getMatchedClusters(int id, caloType type, float dRThreshold);
        MatchedClusterContainer getMatchedTowers(int id, caloType type, float dRThreshold);
        MatchedClusterContainer getMatchedSimTowers(int id, caloType type, float dRThreshold);

        // Modules

        void initEOverPModule();
        void eOverPModule(int id, float totalEnergy, MatchedClusterContainer cemcClusters, MatchedClusterContainer ihcalClusters, MatchedClusterContainer ohcalClusters);

        void initTrackRatesModule();
        void trackRatesModule(int id);

        void initChecksModule();
        void checksModule(MatchedClusterContainer cemcClusters, MatchedClusterContainer ihcalClusters, MatchedClusterContainer ohcalClusters);

};

#endif

#ifdef IsotrackTreesAnalysis_cxx
IsotrackTreesAnalysis::IsotrackTreesAnalysis(std::string inputFilename, std::string outputFilename, bool useTowerInfo, bool useTruthInfo, bool useCentrality, bool useParticleGun, float centralityCut, float d0Cut, float z0Cut, float ptCut, float matchedPtCut, float matchedDrCut, float matchedNeutralTruthPtCut, float matchedNeutralTruthEtaCut, float matchedNeutralTruthDrCut, float cemcMatchingDrCut, float ihcalMatchingDrCut, float ohcalMatchingDrCut, float cemcMipEnergy, float ihcalMipEnergy) :
  USE_TOWER_INFO(useTowerInfo),
  USE_TRUTH_INFO(useTruthInfo),
  USE_CENTRALITY(useCentrality),
  USE_PARTICLE_GUN(useParticleGun),
  CENTRALITY_CUT(centralityCut),
  D0_CUT(d0Cut),
  Z0_CUT(z0Cut),
  PT_CUT(ptCut),
  MATCHED_PT_CUT(matchedPtCut),
  MATCHED_DR_CUT(matchedDrCut),
  MATCHED_NEUTRAL_TRUTH_PT_CUT(matchedNeutralTruthPtCut),
  MATCHED_NEUTRAL_TRUTH_ETA_CUT(matchedNeutralTruthEtaCut),
  MATCHED_NEUTRAL_TRUTH_DR_CUT(matchedNeutralTruthDrCut),
  CEMC_MATCHING_DR_CUT(cemcMatchingDrCut),
  IHCAL_MATCHING_DR_CUT(ihcalMatchingDrCut),
  OHCAL_MATCHING_DR_CUT(ohcalMatchingDrCut),
  CEMC_MIP_ENERGY(cemcMipEnergy),
  IHCAL_MIP_ENERGY(ihcalMipEnergy)
{
  OUTPUT_FILENAME = outputFilename;

// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
  std::ifstream infile(inputFilename);
  TString filename;

  fChainTracks      = new TChain("tracktree");
  fChainClusters    = new TChain("clustertree");
  fChainTowers      = new TChain("towertree");
  fChainSimTowers   = new TChain("simtowertree");
  fChainHepMC       = new TChain("hepmctree");
  fChainG4          = new TChain("g4tree");
  fChainCentrality  = new TChain("centraltree");
  if (USE_PARTICLE_GUN) fChainAddTruth    = new TChain("addtruthtree");

  while(infile >> filename){
    fChainTracks->Add(filename);
    fChainClusters->Add(filename);
    fChainTowers->Add(filename);
    fChainSimTowers->Add(filename);
    fChainHepMC->Add(filename);
    fChainG4->Add(filename);
    fChainCentrality->Add(filename);
    if (USE_PARTICLE_GUN) fChainAddTruth->Add(filename);
  }

  Init();
}

IsotrackTreesAnalysis::~IsotrackTreesAnalysis()
{
  delete fChainTracks;
  delete fChainClusters;
  delete fChainTowers;
  delete fChainSimTowers;
  delete fChainHepMC;
  delete fChainG4;
  delete fChainCentrality;
  if (USE_PARTICLE_GUN) delete fChainAddTruth;
}

void IsotrackTreesAnalysis::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if(fChainTracks)
     fChainTracks->GetEntry(entry);
   if(fChainClusters)
     fChainClusters->GetEntry(entry);
   if(fChainTowers)
     fChainTowers->GetEntry(entry);
   if(fChainSimTowers)
     fChainSimTowers->GetEntry(entry);
   if(fChainHepMC)
     fChainHepMC->GetEntry(entry);
   if(fChainG4)
     fChainG4->GetEntry(entry);
   if(fChainCentrality)
     fChainCentrality->GetEntry(entry);
   if(USE_PARTICLE_GUN && fChainAddTruth)
     fChainAddTruth->GetEntry(entry);
}
Long64_t IsotrackTreesAnalysis::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   /*if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;*/

  return 0;
}

void IsotrackTreesAnalysis::Init()
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   fCurrent = -1;
   //fChainTracks->SetMakeClass(1);

   fChainTracks->SetBranchAddress("m_trkmult", &m_trkmult, &b_m_trkmult);
   fChainTracks->SetBranchAddress("m_tr_p", m_tr_p, &b_m_tr_p);
   fChainTracks->SetBranchAddress("m_tr_pt", m_tr_pt, &b_m_tr_pt);
   fChainTracks->SetBranchAddress("m_tr_eta", m_tr_eta, &b_m_tr_eta);
   fChainTracks->SetBranchAddress("m_tr_phi", m_tr_phi, &b_m_tr_phi);
   fChainTracks->SetBranchAddress("m_tr_charge", m_tr_charge, &b_m_tr_charge);
   fChainTracks->SetBranchAddress("m_tr_chisq", m_tr_chisq, &b_m_tr_chisq);
   fChainTracks->SetBranchAddress("m_tr_ndf", m_tr_ndf, &b_m_tr_ndf);
   fChainTracks->SetBranchAddress("m_tr_dca_xy", m_tr_dca_xy, &b_m_tr_dca_xy);
   fChainTracks->SetBranchAddress("m_tr_dca_xy_error", m_tr_dca_xy_error, &b_m_tr_dca_xy_error);
   fChainTracks->SetBranchAddress("m_tr_dca_z", m_tr_dca_z, &b_m_tr_dca_z);
   fChainTracks->SetBranchAddress("m_tr_dca_z_error", m_tr_dca_z_error, &b_m_tr_dca_z_error);
   fChainTracks->SetBranchAddress("m_tr_x", m_tr_x, &b_m_tr_x);
   fChainTracks->SetBranchAddress("m_tr_y", m_tr_y, &b_m_tr_y);
   fChainTracks->SetBranchAddress("m_tr_z", m_tr_z, &b_m_tr_z);
   fChainTracks->SetBranchAddress("m_tr_vertex_id", m_tr_vertex_id, &b_m_tr_vertex_id);
   fChainTracks->SetBranchAddress("m_vtxmult", &m_vtxmult, &b_m_vtxmult);
   fChainTracks->SetBranchAddress("m_vertex_id", m_vertex_id, &b_m_vertex_id);
   fChainTracks->SetBranchAddress("m_vx", m_vx, &b_m_vx);
   fChainTracks->SetBranchAddress("m_vy", m_vy, &b_m_vy);
   fChainTracks->SetBranchAddress("m_vz", m_vz, &b_m_vz);
   fChainTracks->SetBranchAddress("m_tr_cemc_eta", m_tr_cemc_eta, &b_m_tr_cemc_eta);
   fChainTracks->SetBranchAddress("m_tr_cemc_phi", m_tr_cemc_phi, &b_m_tr_cemc_phi);
   fChainTracks->SetBranchAddress("m_tr_ihcal_eta", m_tr_ihcal_eta, &b_m_tr_ihcal_eta);
   fChainTracks->SetBranchAddress("m_tr_ihcal_phi", m_tr_ihcal_phi, &b_m_tr_ihcal_phi);
   fChainTracks->SetBranchAddress("m_tr_ohcal_eta", m_tr_ohcal_eta, &b_m_tr_ohcal_eta);
   fChainTracks->SetBranchAddress("m_tr_ohcal_phi", m_tr_ohcal_phi, &b_m_tr_ohcal_phi);
   fChainTracks->SetBranchAddress("m_tr_truth_pid", m_tr_truth_pid, &b_m_tr_truth_pid);
   fChainTracks->SetBranchAddress("m_tr_truth_is_primary", m_tr_truth_is_primary, &b_m_tr_truth_is_primary);
   fChainTracks->SetBranchAddress("m_tr_truth_e", m_tr_truth_e, &b_m_tr_truth_e);
   fChainTracks->SetBranchAddress("m_tr_truth_pt", m_tr_truth_pt, &b_m_tr_truth_pt);
   fChainTracks->SetBranchAddress("m_tr_truth_eta", m_tr_truth_eta, &b_m_tr_truth_eta);
   fChainTracks->SetBranchAddress("m_tr_truth_phi", m_tr_truth_phi, &b_m_tr_truth_phi);
   fChainTracks->SetBranchAddress("m_tr_truth_track_id", m_tr_truth_track_id, &b_m_tr_truth_track_id);

   fChainClusters->SetBranchAddress("m_clsmult_cemc", &m_clsmult_cemc, &b_m_clsmult_cemc);
   fChainClusters->SetBranchAddress("m_cl_cemc_e", m_cl_cemc_e, &b_m_cl_cemc_e);
   fChainClusters->SetBranchAddress("m_cl_cemc_eta", m_cl_cemc_eta, &b_m_cl_cemc_eta);
   fChainClusters->SetBranchAddress("m_cl_cemc_phi", m_cl_cemc_phi, &b_m_cl_cemc_phi);
   fChainClusters->SetBranchAddress("m_cl_cemc_r", m_cl_cemc_r, &b_m_cl_cemc_r);
   fChainClusters->SetBranchAddress("m_cl_cemc_z", m_cl_cemc_z, &b_m_cl_cemc_z);
   fChainClusters->SetBranchAddress("m_clsmult_ihcal", &m_clsmult_ihcal, &b_m_clsmult_ihcal);
   fChainClusters->SetBranchAddress("m_cl_ihcal_e", m_cl_ihcal_e, &b_m_cl_ihcal_e);
   fChainClusters->SetBranchAddress("m_cl_ihcal_eta", m_cl_ihcal_eta, &b_m_cl_ihcal_eta);
   fChainClusters->SetBranchAddress("m_cl_ihcal_phi", m_cl_ihcal_phi, &b_m_cl_ihcal_phi);
   fChainClusters->SetBranchAddress("m_cl_ihcal_r", m_cl_ihcal_r, &b_m_cl_ihcal_r);
   fChainClusters->SetBranchAddress("m_cl_ihcal_z", m_cl_ihcal_z, &b_m_cl_ihcal_z);
   fChainClusters->SetBranchAddress("m_clsmult_ohcal", &m_clsmult_ohcal, &b_m_clsmult_ohcal);
   fChainClusters->SetBranchAddress("m_cl_ohcal_e", m_cl_ohcal_e, &b_m_cl_ohcal_e);
   fChainClusters->SetBranchAddress("m_cl_ohcal_eta", m_cl_ohcal_eta, &b_m_cl_ohcal_eta);
   fChainClusters->SetBranchAddress("m_cl_ohcal_phi", m_cl_ohcal_phi, &b_m_cl_ohcal_phi);
   fChainClusters->SetBranchAddress("m_cl_ohcal_r", m_cl_ohcal_r, &b_m_cl_ohcal_r);
   fChainClusters->SetBranchAddress("m_cl_ohcal_z", m_cl_ohcal_z, &b_m_cl_ohcal_z);

   fChainTowers->SetBranchAddress("m_twrmult_cemc", &m_twrmult_cemc, &b_m_twrmult_cemc);
   fChainTowers->SetBranchAddress("m_twr_cemc_e", m_twr_cemc_e, &b_m_twr_cemc_e);
   fChainTowers->SetBranchAddress("m_twr_cemc_eta", m_twr_cemc_eta, &b_m_twr_cemc_eta);
   fChainTowers->SetBranchAddress("m_twr_cemc_phi", m_twr_cemc_phi, &b_m_twr_cemc_phi);
   fChainTowers->SetBranchAddress("m_twr_cemc_ieta", m_twr_cemc_ieta, &b_m_twr_cemc_ieta);
   fChainTowers->SetBranchAddress("m_twr_cemc_iphi", m_twr_cemc_iphi, &b_m_twr_cemc_iphi);
   fChainTowers->SetBranchAddress("m_twrmult_ihcal", &m_twrmult_ihcal, &b_m_twrmult_ihcal);
   fChainTowers->SetBranchAddress("m_twr_ihcal_e", m_twr_ihcal_e, &b_m_twr_ihcal_e);
   fChainTowers->SetBranchAddress("m_twr_ihcal_eta", m_twr_ihcal_eta, &b_m_twr_ihcal_eta);
   fChainTowers->SetBranchAddress("m_twr_ihcal_phi", m_twr_ihcal_phi, &b_m_twr_ihcal_phi);
   fChainTowers->SetBranchAddress("m_twr_ihcal_ieta", m_twr_ihcal_ieta, &b_m_twr_ihcal_ieta);
   fChainTowers->SetBranchAddress("m_twr_ihcal_iphi", m_twr_ihcal_iphi, &b_m_twr_ihcal_iphi);
   fChainTowers->SetBranchAddress("m_twrmult_ohcal", &m_twrmult_ohcal, &b_m_twrmult_ohcal);
   fChainTowers->SetBranchAddress("m_twr_ohcal_e", m_twr_ohcal_e, &b_m_twr_ohcal_e);
   fChainTowers->SetBranchAddress("m_twr_ohcal_eta", m_twr_ohcal_eta, &b_m_twr_ohcal_eta);
   fChainTowers->SetBranchAddress("m_twr_ohcal_phi", m_twr_ohcal_phi, &b_m_twr_ohcal_phi);
   fChainTowers->SetBranchAddress("m_twr_ohcal_ieta", m_twr_ohcal_ieta, &b_m_twr_ohcal_ieta);
   fChainTowers->SetBranchAddress("m_twr_ohcal_iphi", m_twr_ohcal_iphi, &b_m_twr_ohcal_iphi);

   fChainSimTowers->SetBranchAddress("m_simtwrmult_cemc", &m_simtwrmult_cemc, &b_m_simtwrmult_cemc);
   fChainSimTowers->SetBranchAddress("m_simtwr_cemc_e", m_simtwr_cemc_e, &b_m_simtwr_cemc_e);
   fChainSimTowers->SetBranchAddress("m_simtwr_cemc_eta", m_simtwr_cemc_eta, &b_m_simtwr_cemc_eta);
   fChainSimTowers->SetBranchAddress("m_simtwr_cemc_phi", m_simtwr_cemc_phi, &b_m_simtwr_cemc_phi);
   fChainSimTowers->SetBranchAddress("m_simtwr_cemc_ieta", m_simtwr_cemc_ieta, &b_m_simtwr_cemc_ieta);
   fChainSimTowers->SetBranchAddress("m_simtwr_cemc_iphi", m_simtwr_cemc_iphi, &b_m_simtwr_cemc_iphi);
   fChainSimTowers->SetBranchAddress("m_simtwrmult_ihcal", &m_simtwrmult_ihcal, &b_m_simtwrmult_ihcal);
   fChainSimTowers->SetBranchAddress("m_simtwr_ihcal_e", m_simtwr_ihcal_e, &b_m_simtwr_ihcal_e);
   fChainSimTowers->SetBranchAddress("m_simtwr_ihcal_eta", m_simtwr_ihcal_eta, &b_m_simtwr_ihcal_eta);
   fChainSimTowers->SetBranchAddress("m_simtwr_ihcal_phi", m_simtwr_ihcal_phi, &b_m_simtwr_ihcal_phi);
   fChainSimTowers->SetBranchAddress("m_simtwr_ihcal_ieta", m_simtwr_ihcal_ieta, &b_m_simtwr_ihcal_ieta);
   fChainSimTowers->SetBranchAddress("m_simtwr_ihcal_iphi", m_simtwr_ihcal_iphi, &b_m_simtwr_ihcal_iphi);
   fChainSimTowers->SetBranchAddress("m_simtwrmult_ohcal", &m_simtwrmult_ohcal, &b_m_simtwrmult_ohcal);
   fChainSimTowers->SetBranchAddress("m_simtwr_ohcal_e", m_simtwr_ohcal_e, &b_m_simtwr_ohcal_e);
   fChainSimTowers->SetBranchAddress("m_simtwr_ohcal_eta", m_simtwr_ohcal_eta, &b_m_simtwr_ohcal_eta);
   fChainSimTowers->SetBranchAddress("m_simtwr_ohcal_phi", m_simtwr_ohcal_phi, &b_m_simtwr_ohcal_phi);
   fChainSimTowers->SetBranchAddress("m_simtwr_ohcal_ieta", m_simtwr_ohcal_ieta, &b_m_simtwr_ohcal_ieta);
   fChainSimTowers->SetBranchAddress("m_simtwr_ohcal_iphi", m_simtwr_ohcal_iphi, &b_m_simtwr_ohcal_iphi);

   fChainHepMC->SetBranchAddress("m_hepmc", &m_hepmc, &b_m_hepmc);
   fChainHepMC->SetBranchAddress("m_hepmc_pid", &m_hepmc_pid, &b_m_hepmc_pid);
   fChainHepMC->SetBranchAddress("m_hepmc_e", &m_hepmc_e, &b_m_hepmc_e);
   fChainHepMC->SetBranchAddress("m_hepmc_pt", &m_hepmc_pt, &b_m_hepmc_pt);
   fChainHepMC->SetBranchAddress("m_hepmc_eta", &m_hepmc_eta, &b_m_hepmc_eta);
   fChainHepMC->SetBranchAddress("m_hepmc_phi", &m_hepmc_phi, &b_m_hepmc_phi);

   fChainG4->SetBranchAddress("m_g4", &m_g4, &b_m_g4);
   fChainG4->SetBranchAddress("m_g4_pid", m_g4_pid, &b_m_g4_pid);
   fChainG4->SetBranchAddress("m_g4_e", m_g4_e, &b_m_g4_e);
   fChainG4->SetBranchAddress("m_g4_pt", m_g4_pt, &b_m_g4_pt);
   fChainG4->SetBranchAddress("m_g4_eta", m_g4_eta, &b_m_g4_eta);
   fChainG4->SetBranchAddress("m_g4_phi", m_g4_phi, &b_m_g4_phi);
   fChainG4->SetBranchAddress("m_g4_track_id", m_g4_track_id, &b_m_g4_track_id);
   fChainG4->SetBranchAddress("m_g4_parent_id", m_g4_parent_id, &b_m_g4_parent_id);

   fChainCentrality->SetBranchAddress("centrality", &centrality, &b_centrality);

   if (USE_PARTICLE_GUN) {
        fChainAddTruth->SetBranchAddress("n_child", &n_child, &b_n_child);
       fChainAddTruth->SetBranchAddress("child_pid", child_pid, &b_child_pid);
       fChainAddTruth->SetBranchAddress("child_parent_id", child_parent_id, &b_child_parent_id);
       fChainAddTruth->SetBranchAddress("child_vertex_id", child_vertex_id, &b_child_vertex_id);
       fChainAddTruth->SetBranchAddress("child_px", child_px, &b_child_px);
       fChainAddTruth->SetBranchAddress("child_py", child_py, &b_child_py);
       fChainAddTruth->SetBranchAddress("child_pz", child_pz, &b_child_pz);
       fChainAddTruth->SetBranchAddress("child_energy", child_energy, &b_child_energy);
       fChainAddTruth->SetBranchAddress("n_vertex", &n_vertex, &b_n_vertex);
       fChainAddTruth->SetBranchAddress("vertex_id", vertex_id, &b_vertex_id);
       fChainAddTruth->SetBranchAddress("vertex_x", vertex_x, &b_vertex_x);
       fChainAddTruth->SetBranchAddress("vertex_y", vertex_y, &b_vertex_y);
       fChainAddTruth->SetBranchAddress("vertex_z", vertex_z, &b_vertex_z);
       fChainAddTruth->SetBranchAddress("_nBH", &_nBH, &b__nBH);
       fChainAddTruth->SetBranchAddress("BH_e", BH_e, &b_BH_e);
       fChainAddTruth->SetBranchAddress("BH_px", BH_px, &b_BH_px);
       fChainAddTruth->SetBranchAddress("BH_py", BH_py, &b_BH_py);
       fChainAddTruth->SetBranchAddress("BH_pz", BH_pz, &b_BH_pz);
       fChainAddTruth->SetBranchAddress("BH_track_id", BH_track_id, &b_BH_track_id);
   }

   Notify();
}

Bool_t IsotrackTreesAnalysis::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void IsotrackTreesAnalysis::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
//   if (!fChain) return;
//   fChain->Show(entry);
}
Int_t IsotrackTreesAnalysis::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}


#endif // #ifdef IsotrackTreesAnalysis_cxx
