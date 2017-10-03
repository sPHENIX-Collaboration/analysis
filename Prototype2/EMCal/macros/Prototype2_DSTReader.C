#define Prototype2_DSTReader_cxx
// The class definition in Prototype2_DSTReader.h has been generated automatically
// by the ROOT utility TTree::MakeSelector(). This class is derived
// from the ROOT class TSelector. For more information on the TSelector
// framework see $ROOTSYS/README/README.SELECTOR or the ROOT User Manual.

// The following methods are defined in this file:
//    Begin():        called every time a loop on the tree starts,
//                    a convenient place to create your histograms.
//    SlaveBegin():   called after Begin(), when on PROOF called only on the
//                    slave servers.
//    Process():      called for each event, in this function you decide what
//                    to read and fill your histograms.
//    SlaveTerminate: called at the end of the loop on the tree, when on PROOF
//                    called only on the slave servers.
//    Terminate():    called at the end of the loop on the tree,
//                    a convenient place to draw/fit your histograms.
//
// To use this file, try the following session on your Tree T:
//
// Root > T->Process("Prototype2_DSTReader.C")
// Root > T->Process("Prototype2_DSTReader.C","some options")
// Root > T->Process("Prototype2_DSTReader.C+")
//

#include "Prototype2_DSTReader.h"
#include <TH2.h>
#include <TStyle.h>
#include <iostream>

using namespace std;

void
Prototype2_DSTReader::Begin(TTree * /*tree*/)
{
  // The Begin() function is called at the start of the query.
  // When running with PROOF Begin() is only called on the client.
  // The tree argument is deprecated (on PROOF 0 is passed).

  TString option = GetOption();

  fout.open(fOption, ios_base::out);
}

void
Prototype2_DSTReader::SlaveBegin(TTree * /*tree*/)
{
  // The SlaveBegin() function is called after the Begin() function.
  // When running with PROOF SlaveBegin() is called on each slave server.
  // The tree argument is deprecated (on PROOF 0 is passed).

  TString option = GetOption();

}

Bool_t
Prototype2_DSTReader::Process(Long64_t entry)
{
  // The Process() function is called for each entry in the tree (or possibly
  // keyed object in the case of PROOF) to be processed. The entry argument
  // specifies which entry in the currently loaded tree is to be processed.
  // It can be passed to either Prototype2_DSTReader::GetEntry() or TBranch::GetEntry()
  // to read either all or the required parts of the data. When processing
  // keyed objects with PROOF, the object is already loaded and is available
  // via the fObject pointer.
  //
  // This function should contain the "body" of the analysis. It can contain
  // simple or elaborate selection criteria, run algorithms on the data
  // of the event and typically fill histograms.
  //
  // The processing can be stopped by calling Abort().
  //
  // Use fStatus to set the return value of TTree::Process().
  //
  // The return value is currently not used.

  if (entry % 100 == 0)
    cout << "Prototype2_DSTReader::Process - " << entry << ", output : "
        << fOption << endl;

  GetEntry(entry);

//  for (int t = 0; t < kMaxTOWER_CALIB_CEMC; ++t)
//    {
//      typedef unsigned int keytype;
//      static unsigned int calo_idbits = 8;
//      static unsigned int tower_idbits = sizeof(keytype) * 8 - calo_idbits;
//      static unsigned int index1_idbits = tower_idbits / 2;
//      const int calo_tower_id = TOWER_CALIB_CEMC_towerid[t];
//      const int col = (calo_tower_id >> index1_idbits) & 0xFFF;
//      const int row = calo_tower_id & 0xFFF;
//
////      (abs(TOWER_CALIB_CEMC.get_column()-2)<=2 && abs(TOWER_CALIB_CEMC.get_row()-1)<=2 )
//      if ((abs(col - 2) <= 2 && abs(row - 1) <= 2))
//        {
//          fout << TOWER_CALIB_CEMC_energy[t] << "\t";
//        }
//
//    }

  for (int t = 0; t < kMaxTOWER_CALIB_CEMC; ++t)
    {
      fout << TOWER_CALIB_CEMC_energy[t] << "\t";
    }
  for (int t = 0; t < kMaxTOWER_CALIB_LG_HCALIN; ++t)
    {
      fout << TOWER_CALIB_LG_HCALIN_energy[t] << "\t";
    }
  for (int t = 0; t < kMaxTOWER_CALIB_LG_HCALOUT; ++t)
    {
      fout << TOWER_CALIB_LG_HCALOUT_energy[t] << "\t";
    }
  fout << endl;

  return kTRUE;
}

void
Prototype2_DSTReader::SlaveTerminate()
{
  // The SlaveTerminate() function is called after all entries or objects
  // have been processed. When running with PROOF SlaveTerminate() is called
  // on each slave server.

}

void
Prototype2_DSTReader::Terminate()
{
  // The Terminate() function is the last function to be called during
  // a query. It always runs on the client, it can be used to present
  // the results graphically or save the results to file.

  fout.close();
}
