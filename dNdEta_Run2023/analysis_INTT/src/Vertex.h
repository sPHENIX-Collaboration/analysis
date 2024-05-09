#ifndef VERTEX_H
#define VERTEX_H

#include <Riostream.h>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <stdlib.h>
#include <vector>

#include <TCanvas.h>
#include <TFile.h>
#include <TGaxis.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TLine.h>
#include <TMarker.h>
#include <TRandom3.h>
#include <TText.h>
#include <TTree.h>
#include <TVector3.h>

#include "/sphenix/user/hjheng/TrackletAna/analysis/plot/sPHENIXStyle/sPhenixStyle.C"

#define NZGAP 0

using namespace std;

struct VtxData
{
    bool isdata, is_min_bias;
    int event, NClusLayer1, NTruthVtx;
    uint64_t INTT_BCO;
    float TruthPV_x, TruthPV_y, TruthPV_z;
    float PV_x, PV_y, PV_z;
    float PV_z_rand; // For acceptance correction
    float Centrality_bimp, Centrality_impactparam, Centrality_mbd;
    float mbd_south_charge_sum, mbd_north_charge_sum, mbd_charge_sum, mbd_charge_asymm, mbd_z_vtx;
};

void SetVtxMinitree(TTree *outTree, VtxData &vtxdata)
{
    outTree->Branch("event", &vtxdata.event);
    outTree->Branch("INTT_BCO", &vtxdata.INTT_BCO);
    outTree->Branch("is_min_bias", &vtxdata.is_min_bias);
    outTree->Branch("NClusLayer1", &vtxdata.NClusLayer1);
    if (!vtxdata.isdata)
    {
        outTree->Branch("NTruthVtx", &vtxdata.NTruthVtx);
        outTree->Branch("TruthPV_x", &vtxdata.TruthPV_x);
        outTree->Branch("TruthPV_y", &vtxdata.TruthPV_y);
        outTree->Branch("TruthPV_z", &vtxdata.TruthPV_z);
        outTree->Branch("Centrality_bimp", &vtxdata.Centrality_bimp);
        outTree->Branch("Centrality_impactparam", &vtxdata.Centrality_impactparam);
    }
    outTree->Branch("MBD_centrality", &vtxdata.Centrality_mbd);
    outTree->Branch("mbd_south_charge_sum", &vtxdata.mbd_south_charge_sum);
    outTree->Branch("mbd_north_charge_sum", &vtxdata.mbd_north_charge_sum);
    outTree->Branch("mbd_charge_sum", &vtxdata.mbd_charge_sum);
    outTree->Branch("mbd_charge_asymm", &vtxdata.mbd_charge_asymm);
    outTree->Branch("mbd_z_vtx", &vtxdata.mbd_z_vtx);
    outTree->Branch("PV_x", &vtxdata.PV_x);
    outTree->Branch("PV_y", &vtxdata.PV_y);
    outTree->Branch("PV_z", &vtxdata.PV_z);
}

std::map<int, vector<float>> EvtVtx_map_event(const char *vtxfname)
{
    std::map<int, vector<float>> EvtVtx_map;

    TFile *f = new TFile(vtxfname, "READ");
    TTree *t = (TTree *)f->Get("minitree");
    int event;
    float PV_x, PV_y, PV_z;
    t->SetBranchAddress("event", &event);
    t->SetBranchAddress("PV_x", &PV_x);
    t->SetBranchAddress("PV_y", &PV_y);
    t->SetBranchAddress("PV_z", &PV_z);
    for (int ev = 0; ev < t->GetEntriesFast(); ev++)
    {
        t->GetEntry(ev);
        EvtVtx_map[event] = {PV_x, PV_y, PV_z};
    }

    return EvtVtx_map;
}

std::map<uint64_t, vector<float>> EvtVtx_map_inttbco(const char *vtxfname)
{
    std::map<uint64_t, vector<float>> EvtVtx_map;

    TFile *f = new TFile(vtxfname, "READ");
    TTree *t = (TTree *)f->Get("minitree");
    uint64_t intt_bco;
    float PV_x, PV_y, PV_z;
    t->SetBranchAddress("INTT_BCO", &intt_bco);
    t->SetBranchAddress("PV_x", &PV_x);
    t->SetBranchAddress("PV_y", &PV_y);
    t->SetBranchAddress("PV_z", &PV_z);
    for (int ev = 0; ev < t->GetEntriesFast(); ev++)
    {
        t->GetEntry(ev);
        EvtVtx_map[intt_bco] = {PV_x, PV_y, PV_z};
    }

    return EvtVtx_map;
}

TH1F *VtxZ_ReweiHist(const char *filename = "/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/RecoPV_ana/VtxZ_reweight.root")
{
    TFile *f = new TFile(filename, "READ");
    TH1F *h = (TH1F *)f->Get("VtxZ_reweight");
    h->SetDirectory(0);
    f->Close();
    
    return h;
}

#endif
