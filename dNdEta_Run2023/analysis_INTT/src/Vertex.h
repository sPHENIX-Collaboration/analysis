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
    bool isdata;
    int event, NClusLayer1, NTruthVtx;
    float TruthPV_x, TruthPV_y, TruthPV_z;
    float PV_x, PV_y, PV_z;
    float Centrality_bimp, Centrality_impactparam, Centrality_mbd, Centrality_mbdquantity;
};

void SetVtxMinitree(TTree *outTree, VtxData &vtxdata)
{
    outTree->Branch("event", &vtxdata.event);
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
    outTree->Branch("Centrality_mbd", &vtxdata.Centrality_mbd);
    outTree->Branch("Centrality_mbdquantity", &vtxdata.Centrality_mbdquantity);
    outTree->Branch("PV_x", &vtxdata.PV_x);
    outTree->Branch("PV_y", &vtxdata.PV_y);
    outTree->Branch("PV_z", &vtxdata.PV_z);
}

std::map<int, vector<float>> EvtVtx_map_tklcluster(const char *vtxfname)
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

#endif
