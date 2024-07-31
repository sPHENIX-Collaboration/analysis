// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef CALOTREEGEN_H
#define CALOTREEGEN_H

#include <fun4all/SubsysReco.h>

#include <string>
#include <vector>

#include <TH2F.h>

class PHCompositeNode;
class TFile;
class TH1F;
class TH2F;

class caloTreeGen : public SubsysReco
{
  public:

    caloTreeGen(const std::string &name = "caloTreeGen");

    ~caloTreeGen() override;

    /** Called during initialization.
        Typically this is where you can book histograms, and e.g.
        register them to Fun4AllServer (so they can be output to file
        using Fun4AllServer::dumpHistos() method).
    */
    Int_t Init(PHCompositeNode *topNode) override;

    /** Called for each event.
        This is where you do the real work.
    */
    Int_t process_event(PHCompositeNode *topNode) override;

    /// Called at the end of all processing.
    Int_t End(PHCompositeNode *topNode) override;

    void set_vtx_z_max(Float_t vtx_z_max) {
      this->vtx_z_max = vtx_z_max;
    }

    void set_output_QA(const std::string &Outfile) {
      this->Outfile = Outfile;
    }

    void set_towerNode(const std::string &towerNode) {
      this->towerNode = towerNode;
    }

  private:

    TFile *out;
    std::string Outfile          = "test.root";
    std::string towerNode        = "TOWERINFO_CALIB_CEMC";
    std::string towerNodeDefault = "TOWERINFO_CALIB_CEMC";

    Int_t iEvent = 0;
    Int_t iEventGood = 0;
    Float_t avg_goodTowers = 0;

    Double_t min_towE         = 9999;
    Float_t  min_totalCaloE   = 9999;
    Int_t    min_goodTowers   = 9999;
    Float_t  min_vtx_z        = 9999;
    Double_t min_relativeDiff = 9999;

    Double_t max_towE         = 0;
    Float_t  max_totalCaloE   = 0;
    Int_t    max_goodTowers   = 0;
    Float_t  max_vtx_z        = 0;
    Double_t max_relativeDiff = 0;

    TH1F* hTotalCaloE;
    TH1F* hVtxZ;
    TH1F* hVtxZv2;

    TH2F* h2TowEtaPhiWeighted;
    TH2F* h2TowEtaPhiWeightedDefault;
    TH2F* h2TowEtaPhiWeightedRelativeDiff;

    UInt_t  bins_eta = 96;
    UInt_t  bins_phi = 256;

    UInt_t  bins_totalcaloE = 2500;
    Float_t low_totalcaloE  = -500;
    Float_t high_totalcaloE = 2000;

    UInt_t  bins_vtx_z = 400;
    Float_t low_vtx_z  = -100;
    Float_t high_vtx_z = 100;
    // Define z-vertex cut
    Float_t vtx_z_max;

    // MB Info Cuts
    const Int_t   _mbd_tube_cut{2};
    const Float_t _mbd_north_cut{10.};
    const Float_t _mbd_south_cut{150};
    const Float_t _z_vtx_cut{60.};
};
#endif
