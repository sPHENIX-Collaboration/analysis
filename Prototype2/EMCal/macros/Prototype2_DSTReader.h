//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Fri Apr 15 01:31:23 2016 by ROOT version 5.34/34
// from TChain T/
//////////////////////////////////////////////////////////

#ifndef Prototype2_DSTReader_h
#define Prototype2_DSTReader_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>

// Header file for the classes stored in the TTree if any.
#include <TClonesArray.h>
#include <TObject.h>
//#include "/phenix/u/jinhuang/work/sPHENIX/simulation//.//coresoftware/offline/packages/Prototype2/RawTower_Prototype2.h"

#include <fstream>

// Fixed size dimensions of array or collections stored in the TTree if any.
   const Int_t kMaxTOWER_RAW_LG_HCALIN = 16;
   const Int_t kMaxTOWER_RAW_HG_HCALIN = 16;
   const Int_t kMaxTOWER_RAW_LG_HCALOUT = 16;
   const Int_t kMaxTOWER_RAW_HG_HCALOUT = 16;
   const Int_t kMaxTOWER_CALIB_LG_HCALIN = 16;
   const Int_t kMaxTOWER_CALIB_HG_HCALIN = 16;
   const Int_t kMaxTOWER_CALIB_LG_HCALOUT = 16;
   const Int_t kMaxTOWER_CALIB_HG_HCALOUT = 16;
   const Int_t kMaxTOWER_RAW_CEMC = 64;
   const Int_t kMaxTOWER_CALIB_CEMC = 64;
   const Int_t kMaxTOWER_RAW_HODO_VERTICAL = 8;
   const Int_t kMaxTOWER_RAW_HODO_HORIZONTAL = 8;
   const Int_t kMaxTOWER_CALIB_HODO_VERTICAL = 8;
   const Int_t kMaxTOWER_CALIB_HODO_HORIZONTAL = 8;
   const Int_t kMaxTOWER_RAW_C1 = 1;
   const Int_t kMaxTOWER_CALIB_C1 = 1;
   const Int_t kMaxTOWER_RAW_C2 = 2;
   const Int_t kMaxTOWER_CALIB_C2 = 2;
   const Int_t kMaxTOWER_RAW_HCAL_SCINT = 3;
   const Int_t kMaxTOWER_CALIB_HCAL_SCINT = 3;
   const Int_t kMaxTOWER_RAW_PbGL = 1;
   const Int_t kMaxTOWER_CALIB_PbGL = 1;
   const Int_t kMaxTOWER_RAW_TRIGGER_VETO = 4;
   const Int_t kMaxTOWER_CALIB_TRIGGER_VETO = 4;
   const Int_t kMaxTOWER_RAW_TILE_MAPPER = 8;
   const Int_t kMaxTOWER_CALIB_TILE_MAPPER = 8;

class Prototype2_DSTReader : public TSelector {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain

   // Declaration of leaf types
   Int_t           n_TOWER_RAW_LG_HCALIN;
   Int_t           TOWER_RAW_LG_HCALIN_;
   UInt_t          TOWER_RAW_LG_HCALIN_fUniqueID[kMaxTOWER_RAW_LG_HCALIN];   //[TOWER_RAW_LG_HCALIN_]
   UInt_t          TOWER_RAW_LG_HCALIN_fBits[kMaxTOWER_RAW_LG_HCALIN];   //[TOWER_RAW_LG_HCALIN_]
   UInt_t          TOWER_RAW_LG_HCALIN_towerid[kMaxTOWER_RAW_LG_HCALIN];   //[TOWER_RAW_LG_HCALIN_]
   Double_t        TOWER_RAW_LG_HCALIN_energy[kMaxTOWER_RAW_LG_HCALIN];   //[TOWER_RAW_LG_HCALIN_]
   Float_t         TOWER_RAW_LG_HCALIN_time[kMaxTOWER_RAW_LG_HCALIN];   //[TOWER_RAW_LG_HCALIN_]
   Float_t         TOWER_RAW_LG_HCALIN_signal_samples[kMaxTOWER_RAW_LG_HCALIN][24];   //[TOWER_RAW_LG_HCALIN_]
   Int_t           TOWER_RAW_LG_HCALIN_HBD_channel[kMaxTOWER_RAW_LG_HCALIN];   //[TOWER_RAW_LG_HCALIN_]
   Int_t           n_TOWER_RAW_HG_HCALIN;
   Int_t           TOWER_RAW_HG_HCALIN_;
   UInt_t          TOWER_RAW_HG_HCALIN_fUniqueID[kMaxTOWER_RAW_HG_HCALIN];   //[TOWER_RAW_HG_HCALIN_]
   UInt_t          TOWER_RAW_HG_HCALIN_fBits[kMaxTOWER_RAW_HG_HCALIN];   //[TOWER_RAW_HG_HCALIN_]
   UInt_t          TOWER_RAW_HG_HCALIN_towerid[kMaxTOWER_RAW_HG_HCALIN];   //[TOWER_RAW_HG_HCALIN_]
   Double_t        TOWER_RAW_HG_HCALIN_energy[kMaxTOWER_RAW_HG_HCALIN];   //[TOWER_RAW_HG_HCALIN_]
   Float_t         TOWER_RAW_HG_HCALIN_time[kMaxTOWER_RAW_HG_HCALIN];   //[TOWER_RAW_HG_HCALIN_]
   Float_t         TOWER_RAW_HG_HCALIN_signal_samples[kMaxTOWER_RAW_HG_HCALIN][24];   //[TOWER_RAW_HG_HCALIN_]
   Int_t           TOWER_RAW_HG_HCALIN_HBD_channel[kMaxTOWER_RAW_HG_HCALIN];   //[TOWER_RAW_HG_HCALIN_]
   Int_t           n_TOWER_RAW_LG_HCALOUT;
   Int_t           TOWER_RAW_LG_HCALOUT_;
   UInt_t          TOWER_RAW_LG_HCALOUT_fUniqueID[kMaxTOWER_RAW_LG_HCALOUT];   //[TOWER_RAW_LG_HCALOUT_]
   UInt_t          TOWER_RAW_LG_HCALOUT_fBits[kMaxTOWER_RAW_LG_HCALOUT];   //[TOWER_RAW_LG_HCALOUT_]
   UInt_t          TOWER_RAW_LG_HCALOUT_towerid[kMaxTOWER_RAW_LG_HCALOUT];   //[TOWER_RAW_LG_HCALOUT_]
   Double_t        TOWER_RAW_LG_HCALOUT_energy[kMaxTOWER_RAW_LG_HCALOUT];   //[TOWER_RAW_LG_HCALOUT_]
   Float_t         TOWER_RAW_LG_HCALOUT_time[kMaxTOWER_RAW_LG_HCALOUT];   //[TOWER_RAW_LG_HCALOUT_]
   Float_t         TOWER_RAW_LG_HCALOUT_signal_samples[kMaxTOWER_RAW_LG_HCALOUT][24];   //[TOWER_RAW_LG_HCALOUT_]
   Int_t           TOWER_RAW_LG_HCALOUT_HBD_channel[kMaxTOWER_RAW_LG_HCALOUT];   //[TOWER_RAW_LG_HCALOUT_]
   Int_t           n_TOWER_RAW_HG_HCALOUT;
   Int_t           TOWER_RAW_HG_HCALOUT_;
   UInt_t          TOWER_RAW_HG_HCALOUT_fUniqueID[kMaxTOWER_RAW_HG_HCALOUT];   //[TOWER_RAW_HG_HCALOUT_]
   UInt_t          TOWER_RAW_HG_HCALOUT_fBits[kMaxTOWER_RAW_HG_HCALOUT];   //[TOWER_RAW_HG_HCALOUT_]
   UInt_t          TOWER_RAW_HG_HCALOUT_towerid[kMaxTOWER_RAW_HG_HCALOUT];   //[TOWER_RAW_HG_HCALOUT_]
   Double_t        TOWER_RAW_HG_HCALOUT_energy[kMaxTOWER_RAW_HG_HCALOUT];   //[TOWER_RAW_HG_HCALOUT_]
   Float_t         TOWER_RAW_HG_HCALOUT_time[kMaxTOWER_RAW_HG_HCALOUT];   //[TOWER_RAW_HG_HCALOUT_]
   Float_t         TOWER_RAW_HG_HCALOUT_signal_samples[kMaxTOWER_RAW_HG_HCALOUT][24];   //[TOWER_RAW_HG_HCALOUT_]
   Int_t           TOWER_RAW_HG_HCALOUT_HBD_channel[kMaxTOWER_RAW_HG_HCALOUT];   //[TOWER_RAW_HG_HCALOUT_]
   Int_t           n_TOWER_CALIB_LG_HCALIN;
   Int_t           TOWER_CALIB_LG_HCALIN_;
   UInt_t          TOWER_CALIB_LG_HCALIN_fUniqueID[kMaxTOWER_CALIB_LG_HCALIN];   //[TOWER_CALIB_LG_HCALIN_]
   UInt_t          TOWER_CALIB_LG_HCALIN_fBits[kMaxTOWER_CALIB_LG_HCALIN];   //[TOWER_CALIB_LG_HCALIN_]
   UInt_t          TOWER_CALIB_LG_HCALIN_towerid[kMaxTOWER_CALIB_LG_HCALIN];   //[TOWER_CALIB_LG_HCALIN_]
   Double_t        TOWER_CALIB_LG_HCALIN_energy[kMaxTOWER_CALIB_LG_HCALIN];   //[TOWER_CALIB_LG_HCALIN_]
   Float_t         TOWER_CALIB_LG_HCALIN_time[kMaxTOWER_CALIB_LG_HCALIN];   //[TOWER_CALIB_LG_HCALIN_]
   Float_t         TOWER_CALIB_LG_HCALIN_signal_samples[kMaxTOWER_CALIB_LG_HCALIN][24];   //[TOWER_CALIB_LG_HCALIN_]
   Int_t           TOWER_CALIB_LG_HCALIN_HBD_channel[kMaxTOWER_CALIB_LG_HCALIN];   //[TOWER_CALIB_LG_HCALIN_]
   Int_t           n_TOWER_CALIB_HG_HCALIN;
   Int_t           TOWER_CALIB_HG_HCALIN_;
   UInt_t          TOWER_CALIB_HG_HCALIN_fUniqueID[kMaxTOWER_CALIB_HG_HCALIN];   //[TOWER_CALIB_HG_HCALIN_]
   UInt_t          TOWER_CALIB_HG_HCALIN_fBits[kMaxTOWER_CALIB_HG_HCALIN];   //[TOWER_CALIB_HG_HCALIN_]
   UInt_t          TOWER_CALIB_HG_HCALIN_towerid[kMaxTOWER_CALIB_HG_HCALIN];   //[TOWER_CALIB_HG_HCALIN_]
   Double_t        TOWER_CALIB_HG_HCALIN_energy[kMaxTOWER_CALIB_HG_HCALIN];   //[TOWER_CALIB_HG_HCALIN_]
   Float_t         TOWER_CALIB_HG_HCALIN_time[kMaxTOWER_CALIB_HG_HCALIN];   //[TOWER_CALIB_HG_HCALIN_]
   Float_t         TOWER_CALIB_HG_HCALIN_signal_samples[kMaxTOWER_CALIB_HG_HCALIN][24];   //[TOWER_CALIB_HG_HCALIN_]
   Int_t           TOWER_CALIB_HG_HCALIN_HBD_channel[kMaxTOWER_CALIB_HG_HCALIN];   //[TOWER_CALIB_HG_HCALIN_]
   Int_t           n_TOWER_CALIB_LG_HCALOUT;
   Int_t           TOWER_CALIB_LG_HCALOUT_;
   UInt_t          TOWER_CALIB_LG_HCALOUT_fUniqueID[kMaxTOWER_CALIB_LG_HCALOUT];   //[TOWER_CALIB_LG_HCALOUT_]
   UInt_t          TOWER_CALIB_LG_HCALOUT_fBits[kMaxTOWER_CALIB_LG_HCALOUT];   //[TOWER_CALIB_LG_HCALOUT_]
   UInt_t          TOWER_CALIB_LG_HCALOUT_towerid[kMaxTOWER_CALIB_LG_HCALOUT];   //[TOWER_CALIB_LG_HCALOUT_]
   Double_t        TOWER_CALIB_LG_HCALOUT_energy[kMaxTOWER_CALIB_LG_HCALOUT];   //[TOWER_CALIB_LG_HCALOUT_]
   Float_t         TOWER_CALIB_LG_HCALOUT_time[kMaxTOWER_CALIB_LG_HCALOUT];   //[TOWER_CALIB_LG_HCALOUT_]
   Float_t         TOWER_CALIB_LG_HCALOUT_signal_samples[kMaxTOWER_CALIB_LG_HCALOUT][24];   //[TOWER_CALIB_LG_HCALOUT_]
   Int_t           TOWER_CALIB_LG_HCALOUT_HBD_channel[kMaxTOWER_CALIB_LG_HCALOUT];   //[TOWER_CALIB_LG_HCALOUT_]
   Int_t           n_TOWER_CALIB_HG_HCALOUT;
   Int_t           TOWER_CALIB_HG_HCALOUT_;
   UInt_t          TOWER_CALIB_HG_HCALOUT_fUniqueID[kMaxTOWER_CALIB_HG_HCALOUT];   //[TOWER_CALIB_HG_HCALOUT_]
   UInt_t          TOWER_CALIB_HG_HCALOUT_fBits[kMaxTOWER_CALIB_HG_HCALOUT];   //[TOWER_CALIB_HG_HCALOUT_]
   UInt_t          TOWER_CALIB_HG_HCALOUT_towerid[kMaxTOWER_CALIB_HG_HCALOUT];   //[TOWER_CALIB_HG_HCALOUT_]
   Double_t        TOWER_CALIB_HG_HCALOUT_energy[kMaxTOWER_CALIB_HG_HCALOUT];   //[TOWER_CALIB_HG_HCALOUT_]
   Float_t         TOWER_CALIB_HG_HCALOUT_time[kMaxTOWER_CALIB_HG_HCALOUT];   //[TOWER_CALIB_HG_HCALOUT_]
   Float_t         TOWER_CALIB_HG_HCALOUT_signal_samples[kMaxTOWER_CALIB_HG_HCALOUT][24];   //[TOWER_CALIB_HG_HCALOUT_]
   Int_t           TOWER_CALIB_HG_HCALOUT_HBD_channel[kMaxTOWER_CALIB_HG_HCALOUT];   //[TOWER_CALIB_HG_HCALOUT_]
   Int_t           n_TOWER_RAW_CEMC;
   Int_t           TOWER_RAW_CEMC_;
   UInt_t          TOWER_RAW_CEMC_fUniqueID[kMaxTOWER_RAW_CEMC];   //[TOWER_RAW_CEMC_]
   UInt_t          TOWER_RAW_CEMC_fBits[kMaxTOWER_RAW_CEMC];   //[TOWER_RAW_CEMC_]
   UInt_t          TOWER_RAW_CEMC_towerid[kMaxTOWER_RAW_CEMC];   //[TOWER_RAW_CEMC_]
   Double_t        TOWER_RAW_CEMC_energy[kMaxTOWER_RAW_CEMC];   //[TOWER_RAW_CEMC_]
   Float_t         TOWER_RAW_CEMC_time[kMaxTOWER_RAW_CEMC];   //[TOWER_RAW_CEMC_]
   Float_t         TOWER_RAW_CEMC_signal_samples[kMaxTOWER_RAW_CEMC][24];   //[TOWER_RAW_CEMC_]
   Int_t           TOWER_RAW_CEMC_HBD_channel[kMaxTOWER_RAW_CEMC];   //[TOWER_RAW_CEMC_]
   Int_t           n_TOWER_CALIB_CEMC;
   Int_t           TOWER_CALIB_CEMC_;
   UInt_t          TOWER_CALIB_CEMC_fUniqueID[kMaxTOWER_CALIB_CEMC];   //[TOWER_CALIB_CEMC_]
   UInt_t          TOWER_CALIB_CEMC_fBits[kMaxTOWER_CALIB_CEMC];   //[TOWER_CALIB_CEMC_]
   UInt_t          TOWER_CALIB_CEMC_towerid[kMaxTOWER_CALIB_CEMC];   //[TOWER_CALIB_CEMC_]
   Double_t        TOWER_CALIB_CEMC_energy[kMaxTOWER_CALIB_CEMC];   //[TOWER_CALIB_CEMC_]
   Float_t         TOWER_CALIB_CEMC_time[kMaxTOWER_CALIB_CEMC];   //[TOWER_CALIB_CEMC_]
   Float_t         TOWER_CALIB_CEMC_signal_samples[kMaxTOWER_CALIB_CEMC][24];   //[TOWER_CALIB_CEMC_]
   Int_t           TOWER_CALIB_CEMC_HBD_channel[kMaxTOWER_CALIB_CEMC];   //[TOWER_CALIB_CEMC_]
   Int_t           n_TOWER_RAW_HODO_VERTICAL;
   Int_t           TOWER_RAW_HODO_VERTICAL_;
   UInt_t          TOWER_RAW_HODO_VERTICAL_fUniqueID[kMaxTOWER_RAW_HODO_VERTICAL];   //[TOWER_RAW_HODO_VERTICAL_]
   UInt_t          TOWER_RAW_HODO_VERTICAL_fBits[kMaxTOWER_RAW_HODO_VERTICAL];   //[TOWER_RAW_HODO_VERTICAL_]
   UInt_t          TOWER_RAW_HODO_VERTICAL_towerid[kMaxTOWER_RAW_HODO_VERTICAL];   //[TOWER_RAW_HODO_VERTICAL_]
   Double_t        TOWER_RAW_HODO_VERTICAL_energy[kMaxTOWER_RAW_HODO_VERTICAL];   //[TOWER_RAW_HODO_VERTICAL_]
   Float_t         TOWER_RAW_HODO_VERTICAL_time[kMaxTOWER_RAW_HODO_VERTICAL];   //[TOWER_RAW_HODO_VERTICAL_]
   Float_t         TOWER_RAW_HODO_VERTICAL_signal_samples[kMaxTOWER_RAW_HODO_VERTICAL][24];   //[TOWER_RAW_HODO_VERTICAL_]
   Int_t           TOWER_RAW_HODO_VERTICAL_HBD_channel[kMaxTOWER_RAW_HODO_VERTICAL];   //[TOWER_RAW_HODO_VERTICAL_]
   Int_t           n_TOWER_RAW_HODO_HORIZONTAL;
   Int_t           TOWER_RAW_HODO_HORIZONTAL_;
   UInt_t          TOWER_RAW_HODO_HORIZONTAL_fUniqueID[kMaxTOWER_RAW_HODO_HORIZONTAL];   //[TOWER_RAW_HODO_HORIZONTAL_]
   UInt_t          TOWER_RAW_HODO_HORIZONTAL_fBits[kMaxTOWER_RAW_HODO_HORIZONTAL];   //[TOWER_RAW_HODO_HORIZONTAL_]
   UInt_t          TOWER_RAW_HODO_HORIZONTAL_towerid[kMaxTOWER_RAW_HODO_HORIZONTAL];   //[TOWER_RAW_HODO_HORIZONTAL_]
   Double_t        TOWER_RAW_HODO_HORIZONTAL_energy[kMaxTOWER_RAW_HODO_HORIZONTAL];   //[TOWER_RAW_HODO_HORIZONTAL_]
   Float_t         TOWER_RAW_HODO_HORIZONTAL_time[kMaxTOWER_RAW_HODO_HORIZONTAL];   //[TOWER_RAW_HODO_HORIZONTAL_]
   Float_t         TOWER_RAW_HODO_HORIZONTAL_signal_samples[kMaxTOWER_RAW_HODO_HORIZONTAL][24];   //[TOWER_RAW_HODO_HORIZONTAL_]
   Int_t           TOWER_RAW_HODO_HORIZONTAL_HBD_channel[kMaxTOWER_RAW_HODO_HORIZONTAL];   //[TOWER_RAW_HODO_HORIZONTAL_]
   Int_t           n_TOWER_CALIB_HODO_VERTICAL;
   Int_t           TOWER_CALIB_HODO_VERTICAL_;
   UInt_t          TOWER_CALIB_HODO_VERTICAL_fUniqueID[kMaxTOWER_CALIB_HODO_VERTICAL];   //[TOWER_CALIB_HODO_VERTICAL_]
   UInt_t          TOWER_CALIB_HODO_VERTICAL_fBits[kMaxTOWER_CALIB_HODO_VERTICAL];   //[TOWER_CALIB_HODO_VERTICAL_]
   UInt_t          TOWER_CALIB_HODO_VERTICAL_towerid[kMaxTOWER_CALIB_HODO_VERTICAL];   //[TOWER_CALIB_HODO_VERTICAL_]
   Double_t        TOWER_CALIB_HODO_VERTICAL_energy[kMaxTOWER_CALIB_HODO_VERTICAL];   //[TOWER_CALIB_HODO_VERTICAL_]
   Float_t         TOWER_CALIB_HODO_VERTICAL_time[kMaxTOWER_CALIB_HODO_VERTICAL];   //[TOWER_CALIB_HODO_VERTICAL_]
   Float_t         TOWER_CALIB_HODO_VERTICAL_signal_samples[kMaxTOWER_CALIB_HODO_VERTICAL][24];   //[TOWER_CALIB_HODO_VERTICAL_]
   Int_t           TOWER_CALIB_HODO_VERTICAL_HBD_channel[kMaxTOWER_CALIB_HODO_VERTICAL];   //[TOWER_CALIB_HODO_VERTICAL_]
   Int_t           n_TOWER_CALIB_HODO_HORIZONTAL;
   Int_t           TOWER_CALIB_HODO_HORIZONTAL_;
   UInt_t          TOWER_CALIB_HODO_HORIZONTAL_fUniqueID[kMaxTOWER_CALIB_HODO_HORIZONTAL];   //[TOWER_CALIB_HODO_HORIZONTAL_]
   UInt_t          TOWER_CALIB_HODO_HORIZONTAL_fBits[kMaxTOWER_CALIB_HODO_HORIZONTAL];   //[TOWER_CALIB_HODO_HORIZONTAL_]
   UInt_t          TOWER_CALIB_HODO_HORIZONTAL_towerid[kMaxTOWER_CALIB_HODO_HORIZONTAL];   //[TOWER_CALIB_HODO_HORIZONTAL_]
   Double_t        TOWER_CALIB_HODO_HORIZONTAL_energy[kMaxTOWER_CALIB_HODO_HORIZONTAL];   //[TOWER_CALIB_HODO_HORIZONTAL_]
   Float_t         TOWER_CALIB_HODO_HORIZONTAL_time[kMaxTOWER_CALIB_HODO_HORIZONTAL];   //[TOWER_CALIB_HODO_HORIZONTAL_]
   Float_t         TOWER_CALIB_HODO_HORIZONTAL_signal_samples[kMaxTOWER_CALIB_HODO_HORIZONTAL][24];   //[TOWER_CALIB_HODO_HORIZONTAL_]
   Int_t           TOWER_CALIB_HODO_HORIZONTAL_HBD_channel[kMaxTOWER_CALIB_HODO_HORIZONTAL];   //[TOWER_CALIB_HODO_HORIZONTAL_]
   Int_t           n_TOWER_RAW_C1;
   Int_t           TOWER_RAW_C1_;
   UInt_t          TOWER_RAW_C1_fUniqueID[kMaxTOWER_RAW_C1];   //[TOWER_RAW_C1_]
   UInt_t          TOWER_RAW_C1_fBits[kMaxTOWER_RAW_C1];   //[TOWER_RAW_C1_]
   UInt_t          TOWER_RAW_C1_towerid[kMaxTOWER_RAW_C1];   //[TOWER_RAW_C1_]
   Double_t        TOWER_RAW_C1_energy[kMaxTOWER_RAW_C1];   //[TOWER_RAW_C1_]
   Float_t         TOWER_RAW_C1_time[kMaxTOWER_RAW_C1];   //[TOWER_RAW_C1_]
   Float_t         TOWER_RAW_C1_signal_samples[kMaxTOWER_RAW_C1][24];   //[TOWER_RAW_C1_]
   Int_t           TOWER_RAW_C1_HBD_channel[kMaxTOWER_RAW_C1];   //[TOWER_RAW_C1_]
   Int_t           n_TOWER_CALIB_C1;
   Int_t           TOWER_CALIB_C1_;
   UInt_t          TOWER_CALIB_C1_fUniqueID[kMaxTOWER_CALIB_C1];   //[TOWER_CALIB_C1_]
   UInt_t          TOWER_CALIB_C1_fBits[kMaxTOWER_CALIB_C1];   //[TOWER_CALIB_C1_]
   UInt_t          TOWER_CALIB_C1_towerid[kMaxTOWER_CALIB_C1];   //[TOWER_CALIB_C1_]
   Double_t        TOWER_CALIB_C1_energy[kMaxTOWER_CALIB_C1];   //[TOWER_CALIB_C1_]
   Float_t         TOWER_CALIB_C1_time[kMaxTOWER_CALIB_C1];   //[TOWER_CALIB_C1_]
   Float_t         TOWER_CALIB_C1_signal_samples[kMaxTOWER_CALIB_C1][24];   //[TOWER_CALIB_C1_]
   Int_t           TOWER_CALIB_C1_HBD_channel[kMaxTOWER_CALIB_C1];   //[TOWER_CALIB_C1_]
   Int_t           n_TOWER_RAW_C2;
   Int_t           TOWER_RAW_C2_;
   UInt_t          TOWER_RAW_C2_fUniqueID[kMaxTOWER_RAW_C2];   //[TOWER_RAW_C2_]
   UInt_t          TOWER_RAW_C2_fBits[kMaxTOWER_RAW_C2];   //[TOWER_RAW_C2_]
   UInt_t          TOWER_RAW_C2_towerid[kMaxTOWER_RAW_C2];   //[TOWER_RAW_C2_]
   Double_t        TOWER_RAW_C2_energy[kMaxTOWER_RAW_C2];   //[TOWER_RAW_C2_]
   Float_t         TOWER_RAW_C2_time[kMaxTOWER_RAW_C2];   //[TOWER_RAW_C2_]
   Float_t         TOWER_RAW_C2_signal_samples[kMaxTOWER_RAW_C2][24];   //[TOWER_RAW_C2_]
   Int_t           TOWER_RAW_C2_HBD_channel[kMaxTOWER_RAW_C2];   //[TOWER_RAW_C2_]
   Int_t           n_TOWER_CALIB_C2;
   Int_t           TOWER_CALIB_C2_;
   UInt_t          TOWER_CALIB_C2_fUniqueID[kMaxTOWER_CALIB_C2];   //[TOWER_CALIB_C2_]
   UInt_t          TOWER_CALIB_C2_fBits[kMaxTOWER_CALIB_C2];   //[TOWER_CALIB_C2_]
   UInt_t          TOWER_CALIB_C2_towerid[kMaxTOWER_CALIB_C2];   //[TOWER_CALIB_C2_]
   Double_t        TOWER_CALIB_C2_energy[kMaxTOWER_CALIB_C2];   //[TOWER_CALIB_C2_]
   Float_t         TOWER_CALIB_C2_time[kMaxTOWER_CALIB_C2];   //[TOWER_CALIB_C2_]
   Float_t         TOWER_CALIB_C2_signal_samples[kMaxTOWER_CALIB_C2][24];   //[TOWER_CALIB_C2_]
   Int_t           TOWER_CALIB_C2_HBD_channel[kMaxTOWER_CALIB_C2];   //[TOWER_CALIB_C2_]
   Int_t           n_TOWER_RAW_HCAL_SCINT;
   Int_t           TOWER_RAW_HCAL_SCINT_;
   UInt_t          TOWER_RAW_HCAL_SCINT_fUniqueID[kMaxTOWER_RAW_HCAL_SCINT];   //[TOWER_RAW_HCAL_SCINT_]
   UInt_t          TOWER_RAW_HCAL_SCINT_fBits[kMaxTOWER_RAW_HCAL_SCINT];   //[TOWER_RAW_HCAL_SCINT_]
   UInt_t          TOWER_RAW_HCAL_SCINT_towerid[kMaxTOWER_RAW_HCAL_SCINT];   //[TOWER_RAW_HCAL_SCINT_]
   Double_t        TOWER_RAW_HCAL_SCINT_energy[kMaxTOWER_RAW_HCAL_SCINT];   //[TOWER_RAW_HCAL_SCINT_]
   Float_t         TOWER_RAW_HCAL_SCINT_time[kMaxTOWER_RAW_HCAL_SCINT];   //[TOWER_RAW_HCAL_SCINT_]
   Float_t         TOWER_RAW_HCAL_SCINT_signal_samples[kMaxTOWER_RAW_HCAL_SCINT][24];   //[TOWER_RAW_HCAL_SCINT_]
   Int_t           TOWER_RAW_HCAL_SCINT_HBD_channel[kMaxTOWER_RAW_HCAL_SCINT];   //[TOWER_RAW_HCAL_SCINT_]
   Int_t           n_TOWER_CALIB_HCAL_SCINT;
   Int_t           TOWER_CALIB_HCAL_SCINT_;
   UInt_t          TOWER_CALIB_HCAL_SCINT_fUniqueID[kMaxTOWER_CALIB_HCAL_SCINT];   //[TOWER_CALIB_HCAL_SCINT_]
   UInt_t          TOWER_CALIB_HCAL_SCINT_fBits[kMaxTOWER_CALIB_HCAL_SCINT];   //[TOWER_CALIB_HCAL_SCINT_]
   UInt_t          TOWER_CALIB_HCAL_SCINT_towerid[kMaxTOWER_CALIB_HCAL_SCINT];   //[TOWER_CALIB_HCAL_SCINT_]
   Double_t        TOWER_CALIB_HCAL_SCINT_energy[kMaxTOWER_CALIB_HCAL_SCINT];   //[TOWER_CALIB_HCAL_SCINT_]
   Float_t         TOWER_CALIB_HCAL_SCINT_time[kMaxTOWER_CALIB_HCAL_SCINT];   //[TOWER_CALIB_HCAL_SCINT_]
   Float_t         TOWER_CALIB_HCAL_SCINT_signal_samples[kMaxTOWER_CALIB_HCAL_SCINT][24];   //[TOWER_CALIB_HCAL_SCINT_]
   Int_t           TOWER_CALIB_HCAL_SCINT_HBD_channel[kMaxTOWER_CALIB_HCAL_SCINT];   //[TOWER_CALIB_HCAL_SCINT_]
   Int_t           n_TOWER_RAW_PbGL;
   Int_t           TOWER_RAW_PbGL_;
   UInt_t          TOWER_RAW_PbGL_fUniqueID[kMaxTOWER_RAW_PbGL];   //[TOWER_RAW_PbGL_]
   UInt_t          TOWER_RAW_PbGL_fBits[kMaxTOWER_RAW_PbGL];   //[TOWER_RAW_PbGL_]
   UInt_t          TOWER_RAW_PbGL_towerid[kMaxTOWER_RAW_PbGL];   //[TOWER_RAW_PbGL_]
   Double_t        TOWER_RAW_PbGL_energy[kMaxTOWER_RAW_PbGL];   //[TOWER_RAW_PbGL_]
   Float_t         TOWER_RAW_PbGL_time[kMaxTOWER_RAW_PbGL];   //[TOWER_RAW_PbGL_]
   Float_t         TOWER_RAW_PbGL_signal_samples[kMaxTOWER_RAW_PbGL][24];   //[TOWER_RAW_PbGL_]
   Int_t           TOWER_RAW_PbGL_HBD_channel[kMaxTOWER_RAW_PbGL];   //[TOWER_RAW_PbGL_]
   Int_t           n_TOWER_CALIB_PbGL;
   Int_t           TOWER_CALIB_PbGL_;
   UInt_t          TOWER_CALIB_PbGL_fUniqueID[kMaxTOWER_CALIB_PbGL];   //[TOWER_CALIB_PbGL_]
   UInt_t          TOWER_CALIB_PbGL_fBits[kMaxTOWER_CALIB_PbGL];   //[TOWER_CALIB_PbGL_]
   UInt_t          TOWER_CALIB_PbGL_towerid[kMaxTOWER_CALIB_PbGL];   //[TOWER_CALIB_PbGL_]
   Double_t        TOWER_CALIB_PbGL_energy[kMaxTOWER_CALIB_PbGL];   //[TOWER_CALIB_PbGL_]
   Float_t         TOWER_CALIB_PbGL_time[kMaxTOWER_CALIB_PbGL];   //[TOWER_CALIB_PbGL_]
   Float_t         TOWER_CALIB_PbGL_signal_samples[kMaxTOWER_CALIB_PbGL][24];   //[TOWER_CALIB_PbGL_]
   Int_t           TOWER_CALIB_PbGL_HBD_channel[kMaxTOWER_CALIB_PbGL];   //[TOWER_CALIB_PbGL_]
   Int_t           n_TOWER_RAW_TRIGGER_VETO;
   Int_t           TOWER_RAW_TRIGGER_VETO_;
   UInt_t          TOWER_RAW_TRIGGER_VETO_fUniqueID[kMaxTOWER_RAW_TRIGGER_VETO];   //[TOWER_RAW_TRIGGER_VETO_]
   UInt_t          TOWER_RAW_TRIGGER_VETO_fBits[kMaxTOWER_RAW_TRIGGER_VETO];   //[TOWER_RAW_TRIGGER_VETO_]
   UInt_t          TOWER_RAW_TRIGGER_VETO_towerid[kMaxTOWER_RAW_TRIGGER_VETO];   //[TOWER_RAW_TRIGGER_VETO_]
   Double_t        TOWER_RAW_TRIGGER_VETO_energy[kMaxTOWER_RAW_TRIGGER_VETO];   //[TOWER_RAW_TRIGGER_VETO_]
   Float_t         TOWER_RAW_TRIGGER_VETO_time[kMaxTOWER_RAW_TRIGGER_VETO];   //[TOWER_RAW_TRIGGER_VETO_]
   Float_t         TOWER_RAW_TRIGGER_VETO_signal_samples[kMaxTOWER_RAW_TRIGGER_VETO][24];   //[TOWER_RAW_TRIGGER_VETO_]
   Int_t           TOWER_RAW_TRIGGER_VETO_HBD_channel[kMaxTOWER_RAW_TRIGGER_VETO];   //[TOWER_RAW_TRIGGER_VETO_]
   Int_t           n_TOWER_CALIB_TRIGGER_VETO;
   Int_t           TOWER_CALIB_TRIGGER_VETO_;
   UInt_t          TOWER_CALIB_TRIGGER_VETO_fUniqueID[kMaxTOWER_CALIB_TRIGGER_VETO];   //[TOWER_CALIB_TRIGGER_VETO_]
   UInt_t          TOWER_CALIB_TRIGGER_VETO_fBits[kMaxTOWER_CALIB_TRIGGER_VETO];   //[TOWER_CALIB_TRIGGER_VETO_]
   UInt_t          TOWER_CALIB_TRIGGER_VETO_towerid[kMaxTOWER_CALIB_TRIGGER_VETO];   //[TOWER_CALIB_TRIGGER_VETO_]
   Double_t        TOWER_CALIB_TRIGGER_VETO_energy[kMaxTOWER_CALIB_TRIGGER_VETO];   //[TOWER_CALIB_TRIGGER_VETO_]
   Float_t         TOWER_CALIB_TRIGGER_VETO_time[kMaxTOWER_CALIB_TRIGGER_VETO];   //[TOWER_CALIB_TRIGGER_VETO_]
   Float_t         TOWER_CALIB_TRIGGER_VETO_signal_samples[kMaxTOWER_CALIB_TRIGGER_VETO][24];   //[TOWER_CALIB_TRIGGER_VETO_]
   Int_t           TOWER_CALIB_TRIGGER_VETO_HBD_channel[kMaxTOWER_CALIB_TRIGGER_VETO];   //[TOWER_CALIB_TRIGGER_VETO_]
   Int_t           n_TOWER_RAW_TILE_MAPPER;
   Int_t           TOWER_RAW_TILE_MAPPER_;
   UInt_t          TOWER_RAW_TILE_MAPPER_fUniqueID[kMaxTOWER_RAW_TILE_MAPPER];   //[TOWER_RAW_TILE_MAPPER_]
   UInt_t          TOWER_RAW_TILE_MAPPER_fBits[kMaxTOWER_RAW_TILE_MAPPER];   //[TOWER_RAW_TILE_MAPPER_]
   UInt_t          TOWER_RAW_TILE_MAPPER_towerid[kMaxTOWER_RAW_TILE_MAPPER];   //[TOWER_RAW_TILE_MAPPER_]
   Double_t        TOWER_RAW_TILE_MAPPER_energy[kMaxTOWER_RAW_TILE_MAPPER];   //[TOWER_RAW_TILE_MAPPER_]
   Float_t         TOWER_RAW_TILE_MAPPER_time[kMaxTOWER_RAW_TILE_MAPPER];   //[TOWER_RAW_TILE_MAPPER_]
   Float_t         TOWER_RAW_TILE_MAPPER_signal_samples[kMaxTOWER_RAW_TILE_MAPPER][24];   //[TOWER_RAW_TILE_MAPPER_]
   Int_t           TOWER_RAW_TILE_MAPPER_HBD_channel[kMaxTOWER_RAW_TILE_MAPPER];   //[TOWER_RAW_TILE_MAPPER_]
   Int_t           n_TOWER_CALIB_TILE_MAPPER;
   Int_t           TOWER_CALIB_TILE_MAPPER_;
   UInt_t          TOWER_CALIB_TILE_MAPPER_fUniqueID[kMaxTOWER_CALIB_TILE_MAPPER];   //[TOWER_CALIB_TILE_MAPPER_]
   UInt_t          TOWER_CALIB_TILE_MAPPER_fBits[kMaxTOWER_CALIB_TILE_MAPPER];   //[TOWER_CALIB_TILE_MAPPER_]
   UInt_t          TOWER_CALIB_TILE_MAPPER_towerid[kMaxTOWER_CALIB_TILE_MAPPER];   //[TOWER_CALIB_TILE_MAPPER_]
   Double_t        TOWER_CALIB_TILE_MAPPER_energy[kMaxTOWER_CALIB_TILE_MAPPER];   //[TOWER_CALIB_TILE_MAPPER_]
   Float_t         TOWER_CALIB_TILE_MAPPER_time[kMaxTOWER_CALIB_TILE_MAPPER];   //[TOWER_CALIB_TILE_MAPPER_]
   Float_t         TOWER_CALIB_TILE_MAPPER_signal_samples[kMaxTOWER_CALIB_TILE_MAPPER][24];   //[TOWER_CALIB_TILE_MAPPER_]
   Int_t           TOWER_CALIB_TILE_MAPPER_HBD_channel[kMaxTOWER_CALIB_TILE_MAPPER];   //[TOWER_CALIB_TILE_MAPPER_]

   // List of branches
   TBranch        *b_n_TOWER_RAW_LG_HCALIN;   //!
   TBranch        *b_TOWER_RAW_LG_HCALIN_;   //!
   TBranch        *b_TOWER_RAW_LG_HCALIN_fUniqueID;   //!
   TBranch        *b_TOWER_RAW_LG_HCALIN_fBits;   //!
   TBranch        *b_TOWER_RAW_LG_HCALIN_towerid;   //!
   TBranch        *b_TOWER_RAW_LG_HCALIN_energy;   //!
   TBranch        *b_TOWER_RAW_LG_HCALIN_time;   //!
   TBranch        *b_TOWER_RAW_LG_HCALIN_signal_samples;   //!
   TBranch        *b_TOWER_RAW_LG_HCALIN_HBD_channel;   //!
   TBranch        *b_n_TOWER_RAW_HG_HCALIN;   //!
   TBranch        *b_TOWER_RAW_HG_HCALIN_;   //!
   TBranch        *b_TOWER_RAW_HG_HCALIN_fUniqueID;   //!
   TBranch        *b_TOWER_RAW_HG_HCALIN_fBits;   //!
   TBranch        *b_TOWER_RAW_HG_HCALIN_towerid;   //!
   TBranch        *b_TOWER_RAW_HG_HCALIN_energy;   //!
   TBranch        *b_TOWER_RAW_HG_HCALIN_time;   //!
   TBranch        *b_TOWER_RAW_HG_HCALIN_signal_samples;   //!
   TBranch        *b_TOWER_RAW_HG_HCALIN_HBD_channel;   //!
   TBranch        *b_n_TOWER_RAW_LG_HCALOUT;   //!
   TBranch        *b_TOWER_RAW_LG_HCALOUT_;   //!
   TBranch        *b_TOWER_RAW_LG_HCALOUT_fUniqueID;   //!
   TBranch        *b_TOWER_RAW_LG_HCALOUT_fBits;   //!
   TBranch        *b_TOWER_RAW_LG_HCALOUT_towerid;   //!
   TBranch        *b_TOWER_RAW_LG_HCALOUT_energy;   //!
   TBranch        *b_TOWER_RAW_LG_HCALOUT_time;   //!
   TBranch        *b_TOWER_RAW_LG_HCALOUT_signal_samples;   //!
   TBranch        *b_TOWER_RAW_LG_HCALOUT_HBD_channel;   //!
   TBranch        *b_n_TOWER_RAW_HG_HCALOUT;   //!
   TBranch        *b_TOWER_RAW_HG_HCALOUT_;   //!
   TBranch        *b_TOWER_RAW_HG_HCALOUT_fUniqueID;   //!
   TBranch        *b_TOWER_RAW_HG_HCALOUT_fBits;   //!
   TBranch        *b_TOWER_RAW_HG_HCALOUT_towerid;   //!
   TBranch        *b_TOWER_RAW_HG_HCALOUT_energy;   //!
   TBranch        *b_TOWER_RAW_HG_HCALOUT_time;   //!
   TBranch        *b_TOWER_RAW_HG_HCALOUT_signal_samples;   //!
   TBranch        *b_TOWER_RAW_HG_HCALOUT_HBD_channel;   //!
   TBranch        *b_n_TOWER_CALIB_LG_HCALIN;   //!
   TBranch        *b_TOWER_CALIB_LG_HCALIN_;   //!
   TBranch        *b_TOWER_CALIB_LG_HCALIN_fUniqueID;   //!
   TBranch        *b_TOWER_CALIB_LG_HCALIN_fBits;   //!
   TBranch        *b_TOWER_CALIB_LG_HCALIN_towerid;   //!
   TBranch        *b_TOWER_CALIB_LG_HCALIN_energy;   //!
   TBranch        *b_TOWER_CALIB_LG_HCALIN_time;   //!
   TBranch        *b_TOWER_CALIB_LG_HCALIN_signal_samples;   //!
   TBranch        *b_TOWER_CALIB_LG_HCALIN_HBD_channel;   //!
   TBranch        *b_n_TOWER_CALIB_HG_HCALIN;   //!
   TBranch        *b_TOWER_CALIB_HG_HCALIN_;   //!
   TBranch        *b_TOWER_CALIB_HG_HCALIN_fUniqueID;   //!
   TBranch        *b_TOWER_CALIB_HG_HCALIN_fBits;   //!
   TBranch        *b_TOWER_CALIB_HG_HCALIN_towerid;   //!
   TBranch        *b_TOWER_CALIB_HG_HCALIN_energy;   //!
   TBranch        *b_TOWER_CALIB_HG_HCALIN_time;   //!
   TBranch        *b_TOWER_CALIB_HG_HCALIN_signal_samples;   //!
   TBranch        *b_TOWER_CALIB_HG_HCALIN_HBD_channel;   //!
   TBranch        *b_n_TOWER_CALIB_LG_HCALOUT;   //!
   TBranch        *b_TOWER_CALIB_LG_HCALOUT_;   //!
   TBranch        *b_TOWER_CALIB_LG_HCALOUT_fUniqueID;   //!
   TBranch        *b_TOWER_CALIB_LG_HCALOUT_fBits;   //!
   TBranch        *b_TOWER_CALIB_LG_HCALOUT_towerid;   //!
   TBranch        *b_TOWER_CALIB_LG_HCALOUT_energy;   //!
   TBranch        *b_TOWER_CALIB_LG_HCALOUT_time;   //!
   TBranch        *b_TOWER_CALIB_LG_HCALOUT_signal_samples;   //!
   TBranch        *b_TOWER_CALIB_LG_HCALOUT_HBD_channel;   //!
   TBranch        *b_n_TOWER_CALIB_HG_HCALOUT;   //!
   TBranch        *b_TOWER_CALIB_HG_HCALOUT_;   //!
   TBranch        *b_TOWER_CALIB_HG_HCALOUT_fUniqueID;   //!
   TBranch        *b_TOWER_CALIB_HG_HCALOUT_fBits;   //!
   TBranch        *b_TOWER_CALIB_HG_HCALOUT_towerid;   //!
   TBranch        *b_TOWER_CALIB_HG_HCALOUT_energy;   //!
   TBranch        *b_TOWER_CALIB_HG_HCALOUT_time;   //!
   TBranch        *b_TOWER_CALIB_HG_HCALOUT_signal_samples;   //!
   TBranch        *b_TOWER_CALIB_HG_HCALOUT_HBD_channel;   //!
   TBranch        *b_n_TOWER_RAW_CEMC;   //!
   TBranch        *b_TOWER_RAW_CEMC_;   //!
   TBranch        *b_TOWER_RAW_CEMC_fUniqueID;   //!
   TBranch        *b_TOWER_RAW_CEMC_fBits;   //!
   TBranch        *b_TOWER_RAW_CEMC_towerid;   //!
   TBranch        *b_TOWER_RAW_CEMC_energy;   //!
   TBranch        *b_TOWER_RAW_CEMC_time;   //!
   TBranch        *b_TOWER_RAW_CEMC_signal_samples;   //!
   TBranch        *b_TOWER_RAW_CEMC_HBD_channel;   //!
   TBranch        *b_n_TOWER_CALIB_CEMC;   //!
   TBranch        *b_TOWER_CALIB_CEMC_;   //!
   TBranch        *b_TOWER_CALIB_CEMC_fUniqueID;   //!
   TBranch        *b_TOWER_CALIB_CEMC_fBits;   //!
   TBranch        *b_TOWER_CALIB_CEMC_towerid;   //!
   TBranch        *b_TOWER_CALIB_CEMC_energy;   //!
   TBranch        *b_TOWER_CALIB_CEMC_time;   //!
   TBranch        *b_TOWER_CALIB_CEMC_signal_samples;   //!
   TBranch        *b_TOWER_CALIB_CEMC_HBD_channel;   //!
   TBranch        *b_n_TOWER_RAW_HODO_VERTICAL;   //!
   TBranch        *b_TOWER_RAW_HODO_VERTICAL_;   //!
   TBranch        *b_TOWER_RAW_HODO_VERTICAL_fUniqueID;   //!
   TBranch        *b_TOWER_RAW_HODO_VERTICAL_fBits;   //!
   TBranch        *b_TOWER_RAW_HODO_VERTICAL_towerid;   //!
   TBranch        *b_TOWER_RAW_HODO_VERTICAL_energy;   //!
   TBranch        *b_TOWER_RAW_HODO_VERTICAL_time;   //!
   TBranch        *b_TOWER_RAW_HODO_VERTICAL_signal_samples;   //!
   TBranch        *b_TOWER_RAW_HODO_VERTICAL_HBD_channel;   //!
   TBranch        *b_n_TOWER_RAW_HODO_HORIZONTAL;   //!
   TBranch        *b_TOWER_RAW_HODO_HORIZONTAL_;   //!
   TBranch        *b_TOWER_RAW_HODO_HORIZONTAL_fUniqueID;   //!
   TBranch        *b_TOWER_RAW_HODO_HORIZONTAL_fBits;   //!
   TBranch        *b_TOWER_RAW_HODO_HORIZONTAL_towerid;   //!
   TBranch        *b_TOWER_RAW_HODO_HORIZONTAL_energy;   //!
   TBranch        *b_TOWER_RAW_HODO_HORIZONTAL_time;   //!
   TBranch        *b_TOWER_RAW_HODO_HORIZONTAL_signal_samples;   //!
   TBranch        *b_TOWER_RAW_HODO_HORIZONTAL_HBD_channel;   //!
   TBranch        *b_n_TOWER_CALIB_HODO_VERTICAL;   //!
   TBranch        *b_TOWER_CALIB_HODO_VERTICAL_;   //!
   TBranch        *b_TOWER_CALIB_HODO_VERTICAL_fUniqueID;   //!
   TBranch        *b_TOWER_CALIB_HODO_VERTICAL_fBits;   //!
   TBranch        *b_TOWER_CALIB_HODO_VERTICAL_towerid;   //!
   TBranch        *b_TOWER_CALIB_HODO_VERTICAL_energy;   //!
   TBranch        *b_TOWER_CALIB_HODO_VERTICAL_time;   //!
   TBranch        *b_TOWER_CALIB_HODO_VERTICAL_signal_samples;   //!
   TBranch        *b_TOWER_CALIB_HODO_VERTICAL_HBD_channel;   //!
   TBranch        *b_n_TOWER_CALIB_HODO_HORIZONTAL;   //!
   TBranch        *b_TOWER_CALIB_HODO_HORIZONTAL_;   //!
   TBranch        *b_TOWER_CALIB_HODO_HORIZONTAL_fUniqueID;   //!
   TBranch        *b_TOWER_CALIB_HODO_HORIZONTAL_fBits;   //!
   TBranch        *b_TOWER_CALIB_HODO_HORIZONTAL_towerid;   //!
   TBranch        *b_TOWER_CALIB_HODO_HORIZONTAL_energy;   //!
   TBranch        *b_TOWER_CALIB_HODO_HORIZONTAL_time;   //!
   TBranch        *b_TOWER_CALIB_HODO_HORIZONTAL_signal_samples;   //!
   TBranch        *b_TOWER_CALIB_HODO_HORIZONTAL_HBD_channel;   //!
   TBranch        *b_n_TOWER_RAW_C1;   //!
   TBranch        *b_TOWER_RAW_C1_;   //!
   TBranch        *b_TOWER_RAW_C1_fUniqueID;   //!
   TBranch        *b_TOWER_RAW_C1_fBits;   //!
   TBranch        *b_TOWER_RAW_C1_towerid;   //!
   TBranch        *b_TOWER_RAW_C1_energy;   //!
   TBranch        *b_TOWER_RAW_C1_time;   //!
   TBranch        *b_TOWER_RAW_C1_signal_samples;   //!
   TBranch        *b_TOWER_RAW_C1_HBD_channel;   //!
   TBranch        *b_n_TOWER_CALIB_C1;   //!
   TBranch        *b_TOWER_CALIB_C1_;   //!
   TBranch        *b_TOWER_CALIB_C1_fUniqueID;   //!
   TBranch        *b_TOWER_CALIB_C1_fBits;   //!
   TBranch        *b_TOWER_CALIB_C1_towerid;   //!
   TBranch        *b_TOWER_CALIB_C1_energy;   //!
   TBranch        *b_TOWER_CALIB_C1_time;   //!
   TBranch        *b_TOWER_CALIB_C1_signal_samples;   //!
   TBranch        *b_TOWER_CALIB_C1_HBD_channel;   //!
   TBranch        *b_n_TOWER_RAW_C2;   //!
   TBranch        *b_TOWER_RAW_C2_;   //!
   TBranch        *b_TOWER_RAW_C2_fUniqueID;   //!
   TBranch        *b_TOWER_RAW_C2_fBits;   //!
   TBranch        *b_TOWER_RAW_C2_towerid;   //!
   TBranch        *b_TOWER_RAW_C2_energy;   //!
   TBranch        *b_TOWER_RAW_C2_time;   //!
   TBranch        *b_TOWER_RAW_C2_signal_samples;   //!
   TBranch        *b_TOWER_RAW_C2_HBD_channel;   //!
   TBranch        *b_n_TOWER_CALIB_C2;   //!
   TBranch        *b_TOWER_CALIB_C2_;   //!
   TBranch        *b_TOWER_CALIB_C2_fUniqueID;   //!
   TBranch        *b_TOWER_CALIB_C2_fBits;   //!
   TBranch        *b_TOWER_CALIB_C2_towerid;   //!
   TBranch        *b_TOWER_CALIB_C2_energy;   //!
   TBranch        *b_TOWER_CALIB_C2_time;   //!
   TBranch        *b_TOWER_CALIB_C2_signal_samples;   //!
   TBranch        *b_TOWER_CALIB_C2_HBD_channel;   //!
   TBranch        *b_n_TOWER_RAW_HCAL_SCINT;   //!
   TBranch        *b_TOWER_RAW_HCAL_SCINT_;   //!
   TBranch        *b_TOWER_RAW_HCAL_SCINT_fUniqueID;   //!
   TBranch        *b_TOWER_RAW_HCAL_SCINT_fBits;   //!
   TBranch        *b_TOWER_RAW_HCAL_SCINT_towerid;   //!
   TBranch        *b_TOWER_RAW_HCAL_SCINT_energy;   //!
   TBranch        *b_TOWER_RAW_HCAL_SCINT_time;   //!
   TBranch        *b_TOWER_RAW_HCAL_SCINT_signal_samples;   //!
   TBranch        *b_TOWER_RAW_HCAL_SCINT_HBD_channel;   //!
   TBranch        *b_n_TOWER_CALIB_HCAL_SCINT;   //!
   TBranch        *b_TOWER_CALIB_HCAL_SCINT_;   //!
   TBranch        *b_TOWER_CALIB_HCAL_SCINT_fUniqueID;   //!
   TBranch        *b_TOWER_CALIB_HCAL_SCINT_fBits;   //!
   TBranch        *b_TOWER_CALIB_HCAL_SCINT_towerid;   //!
   TBranch        *b_TOWER_CALIB_HCAL_SCINT_energy;   //!
   TBranch        *b_TOWER_CALIB_HCAL_SCINT_time;   //!
   TBranch        *b_TOWER_CALIB_HCAL_SCINT_signal_samples;   //!
   TBranch        *b_TOWER_CALIB_HCAL_SCINT_HBD_channel;   //!
   TBranch        *b_n_TOWER_RAW_PbGL;   //!
   TBranch        *b_TOWER_RAW_PbGL_;   //!
   TBranch        *b_TOWER_RAW_PbGL_fUniqueID;   //!
   TBranch        *b_TOWER_RAW_PbGL_fBits;   //!
   TBranch        *b_TOWER_RAW_PbGL_towerid;   //!
   TBranch        *b_TOWER_RAW_PbGL_energy;   //!
   TBranch        *b_TOWER_RAW_PbGL_time;   //!
   TBranch        *b_TOWER_RAW_PbGL_signal_samples;   //!
   TBranch        *b_TOWER_RAW_PbGL_HBD_channel;   //!
   TBranch        *b_n_TOWER_CALIB_PbGL;   //!
   TBranch        *b_TOWER_CALIB_PbGL_;   //!
   TBranch        *b_TOWER_CALIB_PbGL_fUniqueID;   //!
   TBranch        *b_TOWER_CALIB_PbGL_fBits;   //!
   TBranch        *b_TOWER_CALIB_PbGL_towerid;   //!
   TBranch        *b_TOWER_CALIB_PbGL_energy;   //!
   TBranch        *b_TOWER_CALIB_PbGL_time;   //!
   TBranch        *b_TOWER_CALIB_PbGL_signal_samples;   //!
   TBranch        *b_TOWER_CALIB_PbGL_HBD_channel;   //!
   TBranch        *b_n_TOWER_RAW_TRIGGER_VETO;   //!
   TBranch        *b_TOWER_RAW_TRIGGER_VETO_;   //!
   TBranch        *b_TOWER_RAW_TRIGGER_VETO_fUniqueID;   //!
   TBranch        *b_TOWER_RAW_TRIGGER_VETO_fBits;   //!
   TBranch        *b_TOWER_RAW_TRIGGER_VETO_towerid;   //!
   TBranch        *b_TOWER_RAW_TRIGGER_VETO_energy;   //!
   TBranch        *b_TOWER_RAW_TRIGGER_VETO_time;   //!
   TBranch        *b_TOWER_RAW_TRIGGER_VETO_signal_samples;   //!
   TBranch        *b_TOWER_RAW_TRIGGER_VETO_HBD_channel;   //!
   TBranch        *b_n_TOWER_CALIB_TRIGGER_VETO;   //!
   TBranch        *b_TOWER_CALIB_TRIGGER_VETO_;   //!
   TBranch        *b_TOWER_CALIB_TRIGGER_VETO_fUniqueID;   //!
   TBranch        *b_TOWER_CALIB_TRIGGER_VETO_fBits;   //!
   TBranch        *b_TOWER_CALIB_TRIGGER_VETO_towerid;   //!
   TBranch        *b_TOWER_CALIB_TRIGGER_VETO_energy;   //!
   TBranch        *b_TOWER_CALIB_TRIGGER_VETO_time;   //!
   TBranch        *b_TOWER_CALIB_TRIGGER_VETO_signal_samples;   //!
   TBranch        *b_TOWER_CALIB_TRIGGER_VETO_HBD_channel;   //!
   TBranch        *b_n_TOWER_RAW_TILE_MAPPER;   //!
   TBranch        *b_TOWER_RAW_TILE_MAPPER_;   //!
   TBranch        *b_TOWER_RAW_TILE_MAPPER_fUniqueID;   //!
   TBranch        *b_TOWER_RAW_TILE_MAPPER_fBits;   //!
   TBranch        *b_TOWER_RAW_TILE_MAPPER_towerid;   //!
   TBranch        *b_TOWER_RAW_TILE_MAPPER_energy;   //!
   TBranch        *b_TOWER_RAW_TILE_MAPPER_time;   //!
   TBranch        *b_TOWER_RAW_TILE_MAPPER_signal_samples;   //!
   TBranch        *b_TOWER_RAW_TILE_MAPPER_HBD_channel;   //!
   TBranch        *b_n_TOWER_CALIB_TILE_MAPPER;   //!
   TBranch        *b_TOWER_CALIB_TILE_MAPPER_;   //!
   TBranch        *b_TOWER_CALIB_TILE_MAPPER_fUniqueID;   //!
   TBranch        *b_TOWER_CALIB_TILE_MAPPER_fBits;   //!
   TBranch        *b_TOWER_CALIB_TILE_MAPPER_towerid;   //!
   TBranch        *b_TOWER_CALIB_TILE_MAPPER_energy;   //!
   TBranch        *b_TOWER_CALIB_TILE_MAPPER_time;   //!
   TBranch        *b_TOWER_CALIB_TILE_MAPPER_signal_samples;   //!
   TBranch        *b_TOWER_CALIB_TILE_MAPPER_HBD_channel;   //!

   Prototype2_DSTReader(TTree * /*tree*/ =0) : fChain(0) { }
   virtual ~Prototype2_DSTReader() { }
   virtual Int_t   Version() const { return 2; }
   virtual void    Begin(TTree *tree);
   virtual void    SlaveBegin(TTree *tree);
   virtual void    Init(TTree *tree);
   virtual Bool_t  Notify();
   virtual Bool_t  Process(Long64_t entry);
   virtual Int_t   GetEntry(Long64_t entry, Int_t getall = 0) { return fChain ? fChain->GetTree()->GetEntry(entry, getall) : 0; }
   virtual void    SetOption(const char *option) { fOption = option; }
   virtual void    SetObject(TObject *obj) { fObject = obj; }
   virtual void    SetInputList(TList *input) { fInput = input; }
   virtual TList  *GetOutputList() const { return fOutput; }
   virtual void    SlaveTerminate();
   virtual void    Terminate();

   std::fstream fout;

   ClassDef(Prototype2_DSTReader,0);
};

#endif

#ifdef Prototype2_DSTReader_cxx
void Prototype2_DSTReader::Init(TTree *tree)
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
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("n_TOWER_RAW_LG_HCALIN", &n_TOWER_RAW_LG_HCALIN, &b_n_TOWER_RAW_LG_HCALIN);
   fChain->SetBranchAddress("TOWER_RAW_LG_HCALIN", &TOWER_RAW_LG_HCALIN_, &b_TOWER_RAW_LG_HCALIN_);
   fChain->SetBranchAddress("TOWER_RAW_LG_HCALIN.fUniqueID", TOWER_RAW_LG_HCALIN_fUniqueID, &b_TOWER_RAW_LG_HCALIN_fUniqueID);
   fChain->SetBranchAddress("TOWER_RAW_LG_HCALIN.fBits", TOWER_RAW_LG_HCALIN_fBits, &b_TOWER_RAW_LG_HCALIN_fBits);
   fChain->SetBranchAddress("TOWER_RAW_LG_HCALIN.towerid", TOWER_RAW_LG_HCALIN_towerid, &b_TOWER_RAW_LG_HCALIN_towerid);
   fChain->SetBranchAddress("TOWER_RAW_LG_HCALIN.energy", TOWER_RAW_LG_HCALIN_energy, &b_TOWER_RAW_LG_HCALIN_energy);
   fChain->SetBranchAddress("TOWER_RAW_LG_HCALIN.time", TOWER_RAW_LG_HCALIN_time, &b_TOWER_RAW_LG_HCALIN_time);
   fChain->SetBranchAddress("TOWER_RAW_LG_HCALIN.signal_samples[24]", TOWER_RAW_LG_HCALIN_signal_samples, &b_TOWER_RAW_LG_HCALIN_signal_samples);
   fChain->SetBranchAddress("TOWER_RAW_LG_HCALIN.HBD_channel", TOWER_RAW_LG_HCALIN_HBD_channel, &b_TOWER_RAW_LG_HCALIN_HBD_channel);
   fChain->SetBranchAddress("n_TOWER_RAW_HG_HCALIN", &n_TOWER_RAW_HG_HCALIN, &b_n_TOWER_RAW_HG_HCALIN);
   fChain->SetBranchAddress("TOWER_RAW_HG_HCALIN", &TOWER_RAW_HG_HCALIN_, &b_TOWER_RAW_HG_HCALIN_);
   fChain->SetBranchAddress("TOWER_RAW_HG_HCALIN.fUniqueID", TOWER_RAW_HG_HCALIN_fUniqueID, &b_TOWER_RAW_HG_HCALIN_fUniqueID);
   fChain->SetBranchAddress("TOWER_RAW_HG_HCALIN.fBits", TOWER_RAW_HG_HCALIN_fBits, &b_TOWER_RAW_HG_HCALIN_fBits);
   fChain->SetBranchAddress("TOWER_RAW_HG_HCALIN.towerid", TOWER_RAW_HG_HCALIN_towerid, &b_TOWER_RAW_HG_HCALIN_towerid);
   fChain->SetBranchAddress("TOWER_RAW_HG_HCALIN.energy", TOWER_RAW_HG_HCALIN_energy, &b_TOWER_RAW_HG_HCALIN_energy);
   fChain->SetBranchAddress("TOWER_RAW_HG_HCALIN.time", TOWER_RAW_HG_HCALIN_time, &b_TOWER_RAW_HG_HCALIN_time);
   fChain->SetBranchAddress("TOWER_RAW_HG_HCALIN.signal_samples[24]", TOWER_RAW_HG_HCALIN_signal_samples, &b_TOWER_RAW_HG_HCALIN_signal_samples);
   fChain->SetBranchAddress("TOWER_RAW_HG_HCALIN.HBD_channel", TOWER_RAW_HG_HCALIN_HBD_channel, &b_TOWER_RAW_HG_HCALIN_HBD_channel);
   fChain->SetBranchAddress("n_TOWER_RAW_LG_HCALOUT", &n_TOWER_RAW_LG_HCALOUT, &b_n_TOWER_RAW_LG_HCALOUT);
   fChain->SetBranchAddress("TOWER_RAW_LG_HCALOUT", &TOWER_RAW_LG_HCALOUT_, &b_TOWER_RAW_LG_HCALOUT_);
   fChain->SetBranchAddress("TOWER_RAW_LG_HCALOUT.fUniqueID", TOWER_RAW_LG_HCALOUT_fUniqueID, &b_TOWER_RAW_LG_HCALOUT_fUniqueID);
   fChain->SetBranchAddress("TOWER_RAW_LG_HCALOUT.fBits", TOWER_RAW_LG_HCALOUT_fBits, &b_TOWER_RAW_LG_HCALOUT_fBits);
   fChain->SetBranchAddress("TOWER_RAW_LG_HCALOUT.towerid", TOWER_RAW_LG_HCALOUT_towerid, &b_TOWER_RAW_LG_HCALOUT_towerid);
   fChain->SetBranchAddress("TOWER_RAW_LG_HCALOUT.energy", TOWER_RAW_LG_HCALOUT_energy, &b_TOWER_RAW_LG_HCALOUT_energy);
   fChain->SetBranchAddress("TOWER_RAW_LG_HCALOUT.time", TOWER_RAW_LG_HCALOUT_time, &b_TOWER_RAW_LG_HCALOUT_time);
   fChain->SetBranchAddress("TOWER_RAW_LG_HCALOUT.signal_samples[24]", TOWER_RAW_LG_HCALOUT_signal_samples, &b_TOWER_RAW_LG_HCALOUT_signal_samples);
   fChain->SetBranchAddress("TOWER_RAW_LG_HCALOUT.HBD_channel", TOWER_RAW_LG_HCALOUT_HBD_channel, &b_TOWER_RAW_LG_HCALOUT_HBD_channel);
   fChain->SetBranchAddress("n_TOWER_RAW_HG_HCALOUT", &n_TOWER_RAW_HG_HCALOUT, &b_n_TOWER_RAW_HG_HCALOUT);
   fChain->SetBranchAddress("TOWER_RAW_HG_HCALOUT", &TOWER_RAW_HG_HCALOUT_, &b_TOWER_RAW_HG_HCALOUT_);
   fChain->SetBranchAddress("TOWER_RAW_HG_HCALOUT.fUniqueID", TOWER_RAW_HG_HCALOUT_fUniqueID, &b_TOWER_RAW_HG_HCALOUT_fUniqueID);
   fChain->SetBranchAddress("TOWER_RAW_HG_HCALOUT.fBits", TOWER_RAW_HG_HCALOUT_fBits, &b_TOWER_RAW_HG_HCALOUT_fBits);
   fChain->SetBranchAddress("TOWER_RAW_HG_HCALOUT.towerid", TOWER_RAW_HG_HCALOUT_towerid, &b_TOWER_RAW_HG_HCALOUT_towerid);
   fChain->SetBranchAddress("TOWER_RAW_HG_HCALOUT.energy", TOWER_RAW_HG_HCALOUT_energy, &b_TOWER_RAW_HG_HCALOUT_energy);
   fChain->SetBranchAddress("TOWER_RAW_HG_HCALOUT.time", TOWER_RAW_HG_HCALOUT_time, &b_TOWER_RAW_HG_HCALOUT_time);
   fChain->SetBranchAddress("TOWER_RAW_HG_HCALOUT.signal_samples[24]", TOWER_RAW_HG_HCALOUT_signal_samples, &b_TOWER_RAW_HG_HCALOUT_signal_samples);
   fChain->SetBranchAddress("TOWER_RAW_HG_HCALOUT.HBD_channel", TOWER_RAW_HG_HCALOUT_HBD_channel, &b_TOWER_RAW_HG_HCALOUT_HBD_channel);
   fChain->SetBranchAddress("n_TOWER_CALIB_LG_HCALIN", &n_TOWER_CALIB_LG_HCALIN, &b_n_TOWER_CALIB_LG_HCALIN);
   fChain->SetBranchAddress("TOWER_CALIB_LG_HCALIN", &TOWER_CALIB_LG_HCALIN_, &b_TOWER_CALIB_LG_HCALIN_);
   fChain->SetBranchAddress("TOWER_CALIB_LG_HCALIN.fUniqueID", TOWER_CALIB_LG_HCALIN_fUniqueID, &b_TOWER_CALIB_LG_HCALIN_fUniqueID);
   fChain->SetBranchAddress("TOWER_CALIB_LG_HCALIN.fBits", TOWER_CALIB_LG_HCALIN_fBits, &b_TOWER_CALIB_LG_HCALIN_fBits);
   fChain->SetBranchAddress("TOWER_CALIB_LG_HCALIN.towerid", TOWER_CALIB_LG_HCALIN_towerid, &b_TOWER_CALIB_LG_HCALIN_towerid);
   fChain->SetBranchAddress("TOWER_CALIB_LG_HCALIN.energy", TOWER_CALIB_LG_HCALIN_energy, &b_TOWER_CALIB_LG_HCALIN_energy);
   fChain->SetBranchAddress("TOWER_CALIB_LG_HCALIN.time", TOWER_CALIB_LG_HCALIN_time, &b_TOWER_CALIB_LG_HCALIN_time);
   fChain->SetBranchAddress("TOWER_CALIB_LG_HCALIN.signal_samples[24]", TOWER_CALIB_LG_HCALIN_signal_samples, &b_TOWER_CALIB_LG_HCALIN_signal_samples);
   fChain->SetBranchAddress("TOWER_CALIB_LG_HCALIN.HBD_channel", TOWER_CALIB_LG_HCALIN_HBD_channel, &b_TOWER_CALIB_LG_HCALIN_HBD_channel);
   fChain->SetBranchAddress("n_TOWER_CALIB_HG_HCALIN", &n_TOWER_CALIB_HG_HCALIN, &b_n_TOWER_CALIB_HG_HCALIN);
   fChain->SetBranchAddress("TOWER_CALIB_HG_HCALIN", &TOWER_CALIB_HG_HCALIN_, &b_TOWER_CALIB_HG_HCALIN_);
   fChain->SetBranchAddress("TOWER_CALIB_HG_HCALIN.fUniqueID", TOWER_CALIB_HG_HCALIN_fUniqueID, &b_TOWER_CALIB_HG_HCALIN_fUniqueID);
   fChain->SetBranchAddress("TOWER_CALIB_HG_HCALIN.fBits", TOWER_CALIB_HG_HCALIN_fBits, &b_TOWER_CALIB_HG_HCALIN_fBits);
   fChain->SetBranchAddress("TOWER_CALIB_HG_HCALIN.towerid", TOWER_CALIB_HG_HCALIN_towerid, &b_TOWER_CALIB_HG_HCALIN_towerid);
   fChain->SetBranchAddress("TOWER_CALIB_HG_HCALIN.energy", TOWER_CALIB_HG_HCALIN_energy, &b_TOWER_CALIB_HG_HCALIN_energy);
   fChain->SetBranchAddress("TOWER_CALIB_HG_HCALIN.time", TOWER_CALIB_HG_HCALIN_time, &b_TOWER_CALIB_HG_HCALIN_time);
   fChain->SetBranchAddress("TOWER_CALIB_HG_HCALIN.signal_samples[24]", TOWER_CALIB_HG_HCALIN_signal_samples, &b_TOWER_CALIB_HG_HCALIN_signal_samples);
   fChain->SetBranchAddress("TOWER_CALIB_HG_HCALIN.HBD_channel", TOWER_CALIB_HG_HCALIN_HBD_channel, &b_TOWER_CALIB_HG_HCALIN_HBD_channel);
   fChain->SetBranchAddress("n_TOWER_CALIB_LG_HCALOUT", &n_TOWER_CALIB_LG_HCALOUT, &b_n_TOWER_CALIB_LG_HCALOUT);
   fChain->SetBranchAddress("TOWER_CALIB_LG_HCALOUT", &TOWER_CALIB_LG_HCALOUT_, &b_TOWER_CALIB_LG_HCALOUT_);
   fChain->SetBranchAddress("TOWER_CALIB_LG_HCALOUT.fUniqueID", TOWER_CALIB_LG_HCALOUT_fUniqueID, &b_TOWER_CALIB_LG_HCALOUT_fUniqueID);
   fChain->SetBranchAddress("TOWER_CALIB_LG_HCALOUT.fBits", TOWER_CALIB_LG_HCALOUT_fBits, &b_TOWER_CALIB_LG_HCALOUT_fBits);
   fChain->SetBranchAddress("TOWER_CALIB_LG_HCALOUT.towerid", TOWER_CALIB_LG_HCALOUT_towerid, &b_TOWER_CALIB_LG_HCALOUT_towerid);
   fChain->SetBranchAddress("TOWER_CALIB_LG_HCALOUT.energy", TOWER_CALIB_LG_HCALOUT_energy, &b_TOWER_CALIB_LG_HCALOUT_energy);
   fChain->SetBranchAddress("TOWER_CALIB_LG_HCALOUT.time", TOWER_CALIB_LG_HCALOUT_time, &b_TOWER_CALIB_LG_HCALOUT_time);
   fChain->SetBranchAddress("TOWER_CALIB_LG_HCALOUT.signal_samples[24]", TOWER_CALIB_LG_HCALOUT_signal_samples, &b_TOWER_CALIB_LG_HCALOUT_signal_samples);
   fChain->SetBranchAddress("TOWER_CALIB_LG_HCALOUT.HBD_channel", TOWER_CALIB_LG_HCALOUT_HBD_channel, &b_TOWER_CALIB_LG_HCALOUT_HBD_channel);
   fChain->SetBranchAddress("n_TOWER_CALIB_HG_HCALOUT", &n_TOWER_CALIB_HG_HCALOUT, &b_n_TOWER_CALIB_HG_HCALOUT);
   fChain->SetBranchAddress("TOWER_CALIB_HG_HCALOUT", &TOWER_CALIB_HG_HCALOUT_, &b_TOWER_CALIB_HG_HCALOUT_);
   fChain->SetBranchAddress("TOWER_CALIB_HG_HCALOUT.fUniqueID", TOWER_CALIB_HG_HCALOUT_fUniqueID, &b_TOWER_CALIB_HG_HCALOUT_fUniqueID);
   fChain->SetBranchAddress("TOWER_CALIB_HG_HCALOUT.fBits", TOWER_CALIB_HG_HCALOUT_fBits, &b_TOWER_CALIB_HG_HCALOUT_fBits);
   fChain->SetBranchAddress("TOWER_CALIB_HG_HCALOUT.towerid", TOWER_CALIB_HG_HCALOUT_towerid, &b_TOWER_CALIB_HG_HCALOUT_towerid);
   fChain->SetBranchAddress("TOWER_CALIB_HG_HCALOUT.energy", TOWER_CALIB_HG_HCALOUT_energy, &b_TOWER_CALIB_HG_HCALOUT_energy);
   fChain->SetBranchAddress("TOWER_CALIB_HG_HCALOUT.time", TOWER_CALIB_HG_HCALOUT_time, &b_TOWER_CALIB_HG_HCALOUT_time);
   fChain->SetBranchAddress("TOWER_CALIB_HG_HCALOUT.signal_samples[24]", TOWER_CALIB_HG_HCALOUT_signal_samples, &b_TOWER_CALIB_HG_HCALOUT_signal_samples);
   fChain->SetBranchAddress("TOWER_CALIB_HG_HCALOUT.HBD_channel", TOWER_CALIB_HG_HCALOUT_HBD_channel, &b_TOWER_CALIB_HG_HCALOUT_HBD_channel);
   fChain->SetBranchAddress("n_TOWER_RAW_CEMC", &n_TOWER_RAW_CEMC, &b_n_TOWER_RAW_CEMC);
   fChain->SetBranchAddress("TOWER_RAW_CEMC", &TOWER_RAW_CEMC_, &b_TOWER_RAW_CEMC_);
   fChain->SetBranchAddress("TOWER_RAW_CEMC.fUniqueID", TOWER_RAW_CEMC_fUniqueID, &b_TOWER_RAW_CEMC_fUniqueID);
   fChain->SetBranchAddress("TOWER_RAW_CEMC.fBits", TOWER_RAW_CEMC_fBits, &b_TOWER_RAW_CEMC_fBits);
   fChain->SetBranchAddress("TOWER_RAW_CEMC.towerid", TOWER_RAW_CEMC_towerid, &b_TOWER_RAW_CEMC_towerid);
   fChain->SetBranchAddress("TOWER_RAW_CEMC.energy", TOWER_RAW_CEMC_energy, &b_TOWER_RAW_CEMC_energy);
   fChain->SetBranchAddress("TOWER_RAW_CEMC.time", TOWER_RAW_CEMC_time, &b_TOWER_RAW_CEMC_time);
   fChain->SetBranchAddress("TOWER_RAW_CEMC.signal_samples[24]", TOWER_RAW_CEMC_signal_samples, &b_TOWER_RAW_CEMC_signal_samples);
   fChain->SetBranchAddress("TOWER_RAW_CEMC.HBD_channel", TOWER_RAW_CEMC_HBD_channel, &b_TOWER_RAW_CEMC_HBD_channel);
   fChain->SetBranchAddress("n_TOWER_CALIB_CEMC", &n_TOWER_CALIB_CEMC, &b_n_TOWER_CALIB_CEMC);
   fChain->SetBranchAddress("TOWER_CALIB_CEMC", &TOWER_CALIB_CEMC_, &b_TOWER_CALIB_CEMC_);
   fChain->SetBranchAddress("TOWER_CALIB_CEMC.fUniqueID", TOWER_CALIB_CEMC_fUniqueID, &b_TOWER_CALIB_CEMC_fUniqueID);
   fChain->SetBranchAddress("TOWER_CALIB_CEMC.fBits", TOWER_CALIB_CEMC_fBits, &b_TOWER_CALIB_CEMC_fBits);
   fChain->SetBranchAddress("TOWER_CALIB_CEMC.towerid", TOWER_CALIB_CEMC_towerid, &b_TOWER_CALIB_CEMC_towerid);
   fChain->SetBranchAddress("TOWER_CALIB_CEMC.energy", TOWER_CALIB_CEMC_energy, &b_TOWER_CALIB_CEMC_energy);
   fChain->SetBranchAddress("TOWER_CALIB_CEMC.time", TOWER_CALIB_CEMC_time, &b_TOWER_CALIB_CEMC_time);
   fChain->SetBranchAddress("TOWER_CALIB_CEMC.signal_samples[24]", TOWER_CALIB_CEMC_signal_samples, &b_TOWER_CALIB_CEMC_signal_samples);
   fChain->SetBranchAddress("TOWER_CALIB_CEMC.HBD_channel", TOWER_CALIB_CEMC_HBD_channel, &b_TOWER_CALIB_CEMC_HBD_channel);
   fChain->SetBranchAddress("n_TOWER_RAW_HODO_VERTICAL", &n_TOWER_RAW_HODO_VERTICAL, &b_n_TOWER_RAW_HODO_VERTICAL);
   fChain->SetBranchAddress("TOWER_RAW_HODO_VERTICAL", &TOWER_RAW_HODO_VERTICAL_, &b_TOWER_RAW_HODO_VERTICAL_);
   fChain->SetBranchAddress("TOWER_RAW_HODO_VERTICAL.fUniqueID", TOWER_RAW_HODO_VERTICAL_fUniqueID, &b_TOWER_RAW_HODO_VERTICAL_fUniqueID);
   fChain->SetBranchAddress("TOWER_RAW_HODO_VERTICAL.fBits", TOWER_RAW_HODO_VERTICAL_fBits, &b_TOWER_RAW_HODO_VERTICAL_fBits);
   fChain->SetBranchAddress("TOWER_RAW_HODO_VERTICAL.towerid", TOWER_RAW_HODO_VERTICAL_towerid, &b_TOWER_RAW_HODO_VERTICAL_towerid);
   fChain->SetBranchAddress("TOWER_RAW_HODO_VERTICAL.energy", TOWER_RAW_HODO_VERTICAL_energy, &b_TOWER_RAW_HODO_VERTICAL_energy);
   fChain->SetBranchAddress("TOWER_RAW_HODO_VERTICAL.time", TOWER_RAW_HODO_VERTICAL_time, &b_TOWER_RAW_HODO_VERTICAL_time);
   fChain->SetBranchAddress("TOWER_RAW_HODO_VERTICAL.signal_samples[24]", TOWER_RAW_HODO_VERTICAL_signal_samples, &b_TOWER_RAW_HODO_VERTICAL_signal_samples);
   fChain->SetBranchAddress("TOWER_RAW_HODO_VERTICAL.HBD_channel", TOWER_RAW_HODO_VERTICAL_HBD_channel, &b_TOWER_RAW_HODO_VERTICAL_HBD_channel);
   fChain->SetBranchAddress("n_TOWER_RAW_HODO_HORIZONTAL", &n_TOWER_RAW_HODO_HORIZONTAL, &b_n_TOWER_RAW_HODO_HORIZONTAL);
   fChain->SetBranchAddress("TOWER_RAW_HODO_HORIZONTAL", &TOWER_RAW_HODO_HORIZONTAL_, &b_TOWER_RAW_HODO_HORIZONTAL_);
   fChain->SetBranchAddress("TOWER_RAW_HODO_HORIZONTAL.fUniqueID", TOWER_RAW_HODO_HORIZONTAL_fUniqueID, &b_TOWER_RAW_HODO_HORIZONTAL_fUniqueID);
   fChain->SetBranchAddress("TOWER_RAW_HODO_HORIZONTAL.fBits", TOWER_RAW_HODO_HORIZONTAL_fBits, &b_TOWER_RAW_HODO_HORIZONTAL_fBits);
   fChain->SetBranchAddress("TOWER_RAW_HODO_HORIZONTAL.towerid", TOWER_RAW_HODO_HORIZONTAL_towerid, &b_TOWER_RAW_HODO_HORIZONTAL_towerid);
   fChain->SetBranchAddress("TOWER_RAW_HODO_HORIZONTAL.energy", TOWER_RAW_HODO_HORIZONTAL_energy, &b_TOWER_RAW_HODO_HORIZONTAL_energy);
   fChain->SetBranchAddress("TOWER_RAW_HODO_HORIZONTAL.time", TOWER_RAW_HODO_HORIZONTAL_time, &b_TOWER_RAW_HODO_HORIZONTAL_time);
   fChain->SetBranchAddress("TOWER_RAW_HODO_HORIZONTAL.signal_samples[24]", TOWER_RAW_HODO_HORIZONTAL_signal_samples, &b_TOWER_RAW_HODO_HORIZONTAL_signal_samples);
   fChain->SetBranchAddress("TOWER_RAW_HODO_HORIZONTAL.HBD_channel", TOWER_RAW_HODO_HORIZONTAL_HBD_channel, &b_TOWER_RAW_HODO_HORIZONTAL_HBD_channel);
   fChain->SetBranchAddress("n_TOWER_CALIB_HODO_VERTICAL", &n_TOWER_CALIB_HODO_VERTICAL, &b_n_TOWER_CALIB_HODO_VERTICAL);
   fChain->SetBranchAddress("TOWER_CALIB_HODO_VERTICAL", &TOWER_CALIB_HODO_VERTICAL_, &b_TOWER_CALIB_HODO_VERTICAL_);
   fChain->SetBranchAddress("TOWER_CALIB_HODO_VERTICAL.fUniqueID", TOWER_CALIB_HODO_VERTICAL_fUniqueID, &b_TOWER_CALIB_HODO_VERTICAL_fUniqueID);
   fChain->SetBranchAddress("TOWER_CALIB_HODO_VERTICAL.fBits", TOWER_CALIB_HODO_VERTICAL_fBits, &b_TOWER_CALIB_HODO_VERTICAL_fBits);
   fChain->SetBranchAddress("TOWER_CALIB_HODO_VERTICAL.towerid", TOWER_CALIB_HODO_VERTICAL_towerid, &b_TOWER_CALIB_HODO_VERTICAL_towerid);
   fChain->SetBranchAddress("TOWER_CALIB_HODO_VERTICAL.energy", TOWER_CALIB_HODO_VERTICAL_energy, &b_TOWER_CALIB_HODO_VERTICAL_energy);
   fChain->SetBranchAddress("TOWER_CALIB_HODO_VERTICAL.time", TOWER_CALIB_HODO_VERTICAL_time, &b_TOWER_CALIB_HODO_VERTICAL_time);
   fChain->SetBranchAddress("TOWER_CALIB_HODO_VERTICAL.signal_samples[24]", TOWER_CALIB_HODO_VERTICAL_signal_samples, &b_TOWER_CALIB_HODO_VERTICAL_signal_samples);
   fChain->SetBranchAddress("TOWER_CALIB_HODO_VERTICAL.HBD_channel", TOWER_CALIB_HODO_VERTICAL_HBD_channel, &b_TOWER_CALIB_HODO_VERTICAL_HBD_channel);
   fChain->SetBranchAddress("n_TOWER_CALIB_HODO_HORIZONTAL", &n_TOWER_CALIB_HODO_HORIZONTAL, &b_n_TOWER_CALIB_HODO_HORIZONTAL);
   fChain->SetBranchAddress("TOWER_CALIB_HODO_HORIZONTAL", &TOWER_CALIB_HODO_HORIZONTAL_, &b_TOWER_CALIB_HODO_HORIZONTAL_);
   fChain->SetBranchAddress("TOWER_CALIB_HODO_HORIZONTAL.fUniqueID", TOWER_CALIB_HODO_HORIZONTAL_fUniqueID, &b_TOWER_CALIB_HODO_HORIZONTAL_fUniqueID);
   fChain->SetBranchAddress("TOWER_CALIB_HODO_HORIZONTAL.fBits", TOWER_CALIB_HODO_HORIZONTAL_fBits, &b_TOWER_CALIB_HODO_HORIZONTAL_fBits);
   fChain->SetBranchAddress("TOWER_CALIB_HODO_HORIZONTAL.towerid", TOWER_CALIB_HODO_HORIZONTAL_towerid, &b_TOWER_CALIB_HODO_HORIZONTAL_towerid);
   fChain->SetBranchAddress("TOWER_CALIB_HODO_HORIZONTAL.energy", TOWER_CALIB_HODO_HORIZONTAL_energy, &b_TOWER_CALIB_HODO_HORIZONTAL_energy);
   fChain->SetBranchAddress("TOWER_CALIB_HODO_HORIZONTAL.time", TOWER_CALIB_HODO_HORIZONTAL_time, &b_TOWER_CALIB_HODO_HORIZONTAL_time);
   fChain->SetBranchAddress("TOWER_CALIB_HODO_HORIZONTAL.signal_samples[24]", TOWER_CALIB_HODO_HORIZONTAL_signal_samples, &b_TOWER_CALIB_HODO_HORIZONTAL_signal_samples);
   fChain->SetBranchAddress("TOWER_CALIB_HODO_HORIZONTAL.HBD_channel", TOWER_CALIB_HODO_HORIZONTAL_HBD_channel, &b_TOWER_CALIB_HODO_HORIZONTAL_HBD_channel);
   fChain->SetBranchAddress("n_TOWER_RAW_C1", &n_TOWER_RAW_C1, &b_n_TOWER_RAW_C1);
   fChain->SetBranchAddress("TOWER_RAW_C1", &TOWER_RAW_C1_, &b_TOWER_RAW_C1_);
   fChain->SetBranchAddress("TOWER_RAW_C1.fUniqueID", TOWER_RAW_C1_fUniqueID, &b_TOWER_RAW_C1_fUniqueID);
   fChain->SetBranchAddress("TOWER_RAW_C1.fBits", TOWER_RAW_C1_fBits, &b_TOWER_RAW_C1_fBits);
   fChain->SetBranchAddress("TOWER_RAW_C1.towerid", TOWER_RAW_C1_towerid, &b_TOWER_RAW_C1_towerid);
   fChain->SetBranchAddress("TOWER_RAW_C1.energy", TOWER_RAW_C1_energy, &b_TOWER_RAW_C1_energy);
   fChain->SetBranchAddress("TOWER_RAW_C1.time", TOWER_RAW_C1_time, &b_TOWER_RAW_C1_time);
   fChain->SetBranchAddress("TOWER_RAW_C1.signal_samples[24]", TOWER_RAW_C1_signal_samples, &b_TOWER_RAW_C1_signal_samples);
   fChain->SetBranchAddress("TOWER_RAW_C1.HBD_channel", TOWER_RAW_C1_HBD_channel, &b_TOWER_RAW_C1_HBD_channel);
   fChain->SetBranchAddress("n_TOWER_CALIB_C1", &n_TOWER_CALIB_C1, &b_n_TOWER_CALIB_C1);
   fChain->SetBranchAddress("TOWER_CALIB_C1", &TOWER_CALIB_C1_, &b_TOWER_CALIB_C1_);
   fChain->SetBranchAddress("TOWER_CALIB_C1.fUniqueID", TOWER_CALIB_C1_fUniqueID, &b_TOWER_CALIB_C1_fUniqueID);
   fChain->SetBranchAddress("TOWER_CALIB_C1.fBits", TOWER_CALIB_C1_fBits, &b_TOWER_CALIB_C1_fBits);
   fChain->SetBranchAddress("TOWER_CALIB_C1.towerid", TOWER_CALIB_C1_towerid, &b_TOWER_CALIB_C1_towerid);
   fChain->SetBranchAddress("TOWER_CALIB_C1.energy", TOWER_CALIB_C1_energy, &b_TOWER_CALIB_C1_energy);
   fChain->SetBranchAddress("TOWER_CALIB_C1.time", TOWER_CALIB_C1_time, &b_TOWER_CALIB_C1_time);
   fChain->SetBranchAddress("TOWER_CALIB_C1.signal_samples[24]", TOWER_CALIB_C1_signal_samples, &b_TOWER_CALIB_C1_signal_samples);
   fChain->SetBranchAddress("TOWER_CALIB_C1.HBD_channel", TOWER_CALIB_C1_HBD_channel, &b_TOWER_CALIB_C1_HBD_channel);
   fChain->SetBranchAddress("n_TOWER_RAW_C2", &n_TOWER_RAW_C2, &b_n_TOWER_RAW_C2);
   fChain->SetBranchAddress("TOWER_RAW_C2", &TOWER_RAW_C2_, &b_TOWER_RAW_C2_);
   fChain->SetBranchAddress("TOWER_RAW_C2.fUniqueID", TOWER_RAW_C2_fUniqueID, &b_TOWER_RAW_C2_fUniqueID);
   fChain->SetBranchAddress("TOWER_RAW_C2.fBits", TOWER_RAW_C2_fBits, &b_TOWER_RAW_C2_fBits);
   fChain->SetBranchAddress("TOWER_RAW_C2.towerid", TOWER_RAW_C2_towerid, &b_TOWER_RAW_C2_towerid);
   fChain->SetBranchAddress("TOWER_RAW_C2.energy", TOWER_RAW_C2_energy, &b_TOWER_RAW_C2_energy);
   fChain->SetBranchAddress("TOWER_RAW_C2.time", TOWER_RAW_C2_time, &b_TOWER_RAW_C2_time);
   fChain->SetBranchAddress("TOWER_RAW_C2.signal_samples[24]", TOWER_RAW_C2_signal_samples, &b_TOWER_RAW_C2_signal_samples);
   fChain->SetBranchAddress("TOWER_RAW_C2.HBD_channel", TOWER_RAW_C2_HBD_channel, &b_TOWER_RAW_C2_HBD_channel);
   fChain->SetBranchAddress("n_TOWER_CALIB_C2", &n_TOWER_CALIB_C2, &b_n_TOWER_CALIB_C2);
   fChain->SetBranchAddress("TOWER_CALIB_C2", &TOWER_CALIB_C2_, &b_TOWER_CALIB_C2_);
   fChain->SetBranchAddress("TOWER_CALIB_C2.fUniqueID", TOWER_CALIB_C2_fUniqueID, &b_TOWER_CALIB_C2_fUniqueID);
   fChain->SetBranchAddress("TOWER_CALIB_C2.fBits", TOWER_CALIB_C2_fBits, &b_TOWER_CALIB_C2_fBits);
   fChain->SetBranchAddress("TOWER_CALIB_C2.towerid", TOWER_CALIB_C2_towerid, &b_TOWER_CALIB_C2_towerid);
   fChain->SetBranchAddress("TOWER_CALIB_C2.energy", TOWER_CALIB_C2_energy, &b_TOWER_CALIB_C2_energy);
   fChain->SetBranchAddress("TOWER_CALIB_C2.time", TOWER_CALIB_C2_time, &b_TOWER_CALIB_C2_time);
   fChain->SetBranchAddress("TOWER_CALIB_C2.signal_samples[24]", TOWER_CALIB_C2_signal_samples, &b_TOWER_CALIB_C2_signal_samples);
   fChain->SetBranchAddress("TOWER_CALIB_C2.HBD_channel", TOWER_CALIB_C2_HBD_channel, &b_TOWER_CALIB_C2_HBD_channel);
   fChain->SetBranchAddress("n_TOWER_RAW_HCAL_SCINT", &n_TOWER_RAW_HCAL_SCINT, &b_n_TOWER_RAW_HCAL_SCINT);
   fChain->SetBranchAddress("TOWER_RAW_HCAL_SCINT", &TOWER_RAW_HCAL_SCINT_, &b_TOWER_RAW_HCAL_SCINT_);
   fChain->SetBranchAddress("TOWER_RAW_HCAL_SCINT.fUniqueID", TOWER_RAW_HCAL_SCINT_fUniqueID, &b_TOWER_RAW_HCAL_SCINT_fUniqueID);
   fChain->SetBranchAddress("TOWER_RAW_HCAL_SCINT.fBits", TOWER_RAW_HCAL_SCINT_fBits, &b_TOWER_RAW_HCAL_SCINT_fBits);
   fChain->SetBranchAddress("TOWER_RAW_HCAL_SCINT.towerid", TOWER_RAW_HCAL_SCINT_towerid, &b_TOWER_RAW_HCAL_SCINT_towerid);
   fChain->SetBranchAddress("TOWER_RAW_HCAL_SCINT.energy", TOWER_RAW_HCAL_SCINT_energy, &b_TOWER_RAW_HCAL_SCINT_energy);
   fChain->SetBranchAddress("TOWER_RAW_HCAL_SCINT.time", TOWER_RAW_HCAL_SCINT_time, &b_TOWER_RAW_HCAL_SCINT_time);
   fChain->SetBranchAddress("TOWER_RAW_HCAL_SCINT.signal_samples[24]", TOWER_RAW_HCAL_SCINT_signal_samples, &b_TOWER_RAW_HCAL_SCINT_signal_samples);
   fChain->SetBranchAddress("TOWER_RAW_HCAL_SCINT.HBD_channel", TOWER_RAW_HCAL_SCINT_HBD_channel, &b_TOWER_RAW_HCAL_SCINT_HBD_channel);
   fChain->SetBranchAddress("n_TOWER_CALIB_HCAL_SCINT", &n_TOWER_CALIB_HCAL_SCINT, &b_n_TOWER_CALIB_HCAL_SCINT);
   fChain->SetBranchAddress("TOWER_CALIB_HCAL_SCINT", &TOWER_CALIB_HCAL_SCINT_, &b_TOWER_CALIB_HCAL_SCINT_);
   fChain->SetBranchAddress("TOWER_CALIB_HCAL_SCINT.fUniqueID", TOWER_CALIB_HCAL_SCINT_fUniqueID, &b_TOWER_CALIB_HCAL_SCINT_fUniqueID);
   fChain->SetBranchAddress("TOWER_CALIB_HCAL_SCINT.fBits", TOWER_CALIB_HCAL_SCINT_fBits, &b_TOWER_CALIB_HCAL_SCINT_fBits);
   fChain->SetBranchAddress("TOWER_CALIB_HCAL_SCINT.towerid", TOWER_CALIB_HCAL_SCINT_towerid, &b_TOWER_CALIB_HCAL_SCINT_towerid);
   fChain->SetBranchAddress("TOWER_CALIB_HCAL_SCINT.energy", TOWER_CALIB_HCAL_SCINT_energy, &b_TOWER_CALIB_HCAL_SCINT_energy);
   fChain->SetBranchAddress("TOWER_CALIB_HCAL_SCINT.time", TOWER_CALIB_HCAL_SCINT_time, &b_TOWER_CALIB_HCAL_SCINT_time);
   fChain->SetBranchAddress("TOWER_CALIB_HCAL_SCINT.signal_samples[24]", TOWER_CALIB_HCAL_SCINT_signal_samples, &b_TOWER_CALIB_HCAL_SCINT_signal_samples);
   fChain->SetBranchAddress("TOWER_CALIB_HCAL_SCINT.HBD_channel", TOWER_CALIB_HCAL_SCINT_HBD_channel, &b_TOWER_CALIB_HCAL_SCINT_HBD_channel);
   fChain->SetBranchAddress("n_TOWER_RAW_PbGL", &n_TOWER_RAW_PbGL, &b_n_TOWER_RAW_PbGL);
   fChain->SetBranchAddress("TOWER_RAW_PbGL", &TOWER_RAW_PbGL_, &b_TOWER_RAW_PbGL_);
   fChain->SetBranchAddress("TOWER_RAW_PbGL.fUniqueID", TOWER_RAW_PbGL_fUniqueID, &b_TOWER_RAW_PbGL_fUniqueID);
   fChain->SetBranchAddress("TOWER_RAW_PbGL.fBits", TOWER_RAW_PbGL_fBits, &b_TOWER_RAW_PbGL_fBits);
   fChain->SetBranchAddress("TOWER_RAW_PbGL.towerid", TOWER_RAW_PbGL_towerid, &b_TOWER_RAW_PbGL_towerid);
   fChain->SetBranchAddress("TOWER_RAW_PbGL.energy", TOWER_RAW_PbGL_energy, &b_TOWER_RAW_PbGL_energy);
   fChain->SetBranchAddress("TOWER_RAW_PbGL.time", TOWER_RAW_PbGL_time, &b_TOWER_RAW_PbGL_time);
   fChain->SetBranchAddress("TOWER_RAW_PbGL.signal_samples[24]", TOWER_RAW_PbGL_signal_samples, &b_TOWER_RAW_PbGL_signal_samples);
   fChain->SetBranchAddress("TOWER_RAW_PbGL.HBD_channel", TOWER_RAW_PbGL_HBD_channel, &b_TOWER_RAW_PbGL_HBD_channel);
   fChain->SetBranchAddress("n_TOWER_CALIB_PbGL", &n_TOWER_CALIB_PbGL, &b_n_TOWER_CALIB_PbGL);
   fChain->SetBranchAddress("TOWER_CALIB_PbGL", &TOWER_CALIB_PbGL_, &b_TOWER_CALIB_PbGL_);
   fChain->SetBranchAddress("TOWER_CALIB_PbGL.fUniqueID", TOWER_CALIB_PbGL_fUniqueID, &b_TOWER_CALIB_PbGL_fUniqueID);
   fChain->SetBranchAddress("TOWER_CALIB_PbGL.fBits", TOWER_CALIB_PbGL_fBits, &b_TOWER_CALIB_PbGL_fBits);
   fChain->SetBranchAddress("TOWER_CALIB_PbGL.towerid", TOWER_CALIB_PbGL_towerid, &b_TOWER_CALIB_PbGL_towerid);
   fChain->SetBranchAddress("TOWER_CALIB_PbGL.energy", TOWER_CALIB_PbGL_energy, &b_TOWER_CALIB_PbGL_energy);
   fChain->SetBranchAddress("TOWER_CALIB_PbGL.time", TOWER_CALIB_PbGL_time, &b_TOWER_CALIB_PbGL_time);
   fChain->SetBranchAddress("TOWER_CALIB_PbGL.signal_samples[24]", TOWER_CALIB_PbGL_signal_samples, &b_TOWER_CALIB_PbGL_signal_samples);
   fChain->SetBranchAddress("TOWER_CALIB_PbGL.HBD_channel", TOWER_CALIB_PbGL_HBD_channel, &b_TOWER_CALIB_PbGL_HBD_channel);
   fChain->SetBranchAddress("n_TOWER_RAW_TRIGGER_VETO", &n_TOWER_RAW_TRIGGER_VETO, &b_n_TOWER_RAW_TRIGGER_VETO);
   fChain->SetBranchAddress("TOWER_RAW_TRIGGER_VETO", &TOWER_RAW_TRIGGER_VETO_, &b_TOWER_RAW_TRIGGER_VETO_);
   fChain->SetBranchAddress("TOWER_RAW_TRIGGER_VETO.fUniqueID", TOWER_RAW_TRIGGER_VETO_fUniqueID, &b_TOWER_RAW_TRIGGER_VETO_fUniqueID);
   fChain->SetBranchAddress("TOWER_RAW_TRIGGER_VETO.fBits", TOWER_RAW_TRIGGER_VETO_fBits, &b_TOWER_RAW_TRIGGER_VETO_fBits);
   fChain->SetBranchAddress("TOWER_RAW_TRIGGER_VETO.towerid", TOWER_RAW_TRIGGER_VETO_towerid, &b_TOWER_RAW_TRIGGER_VETO_towerid);
   fChain->SetBranchAddress("TOWER_RAW_TRIGGER_VETO.energy", TOWER_RAW_TRIGGER_VETO_energy, &b_TOWER_RAW_TRIGGER_VETO_energy);
   fChain->SetBranchAddress("TOWER_RAW_TRIGGER_VETO.time", TOWER_RAW_TRIGGER_VETO_time, &b_TOWER_RAW_TRIGGER_VETO_time);
   fChain->SetBranchAddress("TOWER_RAW_TRIGGER_VETO.signal_samples[24]", TOWER_RAW_TRIGGER_VETO_signal_samples, &b_TOWER_RAW_TRIGGER_VETO_signal_samples);
   fChain->SetBranchAddress("TOWER_RAW_TRIGGER_VETO.HBD_channel", TOWER_RAW_TRIGGER_VETO_HBD_channel, &b_TOWER_RAW_TRIGGER_VETO_HBD_channel);
   fChain->SetBranchAddress("n_TOWER_CALIB_TRIGGER_VETO", &n_TOWER_CALIB_TRIGGER_VETO, &b_n_TOWER_CALIB_TRIGGER_VETO);
   fChain->SetBranchAddress("TOWER_CALIB_TRIGGER_VETO", &TOWER_CALIB_TRIGGER_VETO_, &b_TOWER_CALIB_TRIGGER_VETO_);
   fChain->SetBranchAddress("TOWER_CALIB_TRIGGER_VETO.fUniqueID", TOWER_CALIB_TRIGGER_VETO_fUniqueID, &b_TOWER_CALIB_TRIGGER_VETO_fUniqueID);
   fChain->SetBranchAddress("TOWER_CALIB_TRIGGER_VETO.fBits", TOWER_CALIB_TRIGGER_VETO_fBits, &b_TOWER_CALIB_TRIGGER_VETO_fBits);
   fChain->SetBranchAddress("TOWER_CALIB_TRIGGER_VETO.towerid", TOWER_CALIB_TRIGGER_VETO_towerid, &b_TOWER_CALIB_TRIGGER_VETO_towerid);
   fChain->SetBranchAddress("TOWER_CALIB_TRIGGER_VETO.energy", TOWER_CALIB_TRIGGER_VETO_energy, &b_TOWER_CALIB_TRIGGER_VETO_energy);
   fChain->SetBranchAddress("TOWER_CALIB_TRIGGER_VETO.time", TOWER_CALIB_TRIGGER_VETO_time, &b_TOWER_CALIB_TRIGGER_VETO_time);
   fChain->SetBranchAddress("TOWER_CALIB_TRIGGER_VETO.signal_samples[24]", TOWER_CALIB_TRIGGER_VETO_signal_samples, &b_TOWER_CALIB_TRIGGER_VETO_signal_samples);
   fChain->SetBranchAddress("TOWER_CALIB_TRIGGER_VETO.HBD_channel", TOWER_CALIB_TRIGGER_VETO_HBD_channel, &b_TOWER_CALIB_TRIGGER_VETO_HBD_channel);
   fChain->SetBranchAddress("n_TOWER_RAW_TILE_MAPPER", &n_TOWER_RAW_TILE_MAPPER, &b_n_TOWER_RAW_TILE_MAPPER);
   fChain->SetBranchAddress("TOWER_RAW_TILE_MAPPER", &TOWER_RAW_TILE_MAPPER_, &b_TOWER_RAW_TILE_MAPPER_);
   fChain->SetBranchAddress("TOWER_RAW_TILE_MAPPER.fUniqueID", TOWER_RAW_TILE_MAPPER_fUniqueID, &b_TOWER_RAW_TILE_MAPPER_fUniqueID);
   fChain->SetBranchAddress("TOWER_RAW_TILE_MAPPER.fBits", TOWER_RAW_TILE_MAPPER_fBits, &b_TOWER_RAW_TILE_MAPPER_fBits);
   fChain->SetBranchAddress("TOWER_RAW_TILE_MAPPER.towerid", TOWER_RAW_TILE_MAPPER_towerid, &b_TOWER_RAW_TILE_MAPPER_towerid);
   fChain->SetBranchAddress("TOWER_RAW_TILE_MAPPER.energy", TOWER_RAW_TILE_MAPPER_energy, &b_TOWER_RAW_TILE_MAPPER_energy);
   fChain->SetBranchAddress("TOWER_RAW_TILE_MAPPER.time", TOWER_RAW_TILE_MAPPER_time, &b_TOWER_RAW_TILE_MAPPER_time);
   fChain->SetBranchAddress("TOWER_RAW_TILE_MAPPER.signal_samples[24]", TOWER_RAW_TILE_MAPPER_signal_samples, &b_TOWER_RAW_TILE_MAPPER_signal_samples);
   fChain->SetBranchAddress("TOWER_RAW_TILE_MAPPER.HBD_channel", TOWER_RAW_TILE_MAPPER_HBD_channel, &b_TOWER_RAW_TILE_MAPPER_HBD_channel);
   fChain->SetBranchAddress("n_TOWER_CALIB_TILE_MAPPER", &n_TOWER_CALIB_TILE_MAPPER, &b_n_TOWER_CALIB_TILE_MAPPER);
   fChain->SetBranchAddress("TOWER_CALIB_TILE_MAPPER", &TOWER_CALIB_TILE_MAPPER_, &b_TOWER_CALIB_TILE_MAPPER_);
   fChain->SetBranchAddress("TOWER_CALIB_TILE_MAPPER.fUniqueID", TOWER_CALIB_TILE_MAPPER_fUniqueID, &b_TOWER_CALIB_TILE_MAPPER_fUniqueID);
   fChain->SetBranchAddress("TOWER_CALIB_TILE_MAPPER.fBits", TOWER_CALIB_TILE_MAPPER_fBits, &b_TOWER_CALIB_TILE_MAPPER_fBits);
   fChain->SetBranchAddress("TOWER_CALIB_TILE_MAPPER.towerid", TOWER_CALIB_TILE_MAPPER_towerid, &b_TOWER_CALIB_TILE_MAPPER_towerid);
   fChain->SetBranchAddress("TOWER_CALIB_TILE_MAPPER.energy", TOWER_CALIB_TILE_MAPPER_energy, &b_TOWER_CALIB_TILE_MAPPER_energy);
   fChain->SetBranchAddress("TOWER_CALIB_TILE_MAPPER.time", TOWER_CALIB_TILE_MAPPER_time, &b_TOWER_CALIB_TILE_MAPPER_time);
   fChain->SetBranchAddress("TOWER_CALIB_TILE_MAPPER.signal_samples[24]", TOWER_CALIB_TILE_MAPPER_signal_samples, &b_TOWER_CALIB_TILE_MAPPER_signal_samples);
   fChain->SetBranchAddress("TOWER_CALIB_TILE_MAPPER.HBD_channel", TOWER_CALIB_TILE_MAPPER_HBD_channel, &b_TOWER_CALIB_TILE_MAPPER_HBD_channel);
}

Bool_t Prototype2_DSTReader::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

#endif // #ifdef Prototype2_DSTReader_cxx
