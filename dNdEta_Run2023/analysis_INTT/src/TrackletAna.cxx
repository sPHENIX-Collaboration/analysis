#include <TFile.h>
#include <TMath.h>
#include <TObjString.h>
#include <TRandom3.h>
#include <TTree.h>
#include <TTreeIndex.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "GenHadron.h"
#include "Tracklet.h"
#include "Vertex.h"
#include "misalignment.h"
#include "pdgidfunc.h"

int main(int argc, char *argv[])
{
    // Usage: ./TrackletAna [NevtToRun] [skip] [layer] [randhit_case] [clusplit_case] [dRCut]
    // Example: ./TrackletAna 2000 0 12 0 0 0 0.5
    int NevtToRun_ = TString(argv[1]).Atoi();
    int skip = TString(argv[2]).Atoi();
    float dRCut = TString(argv[3]).Atof(); // Nominal: 0.5, variation \pm 0.1

    int iniEvt = skip;

    cout << "[Run Info] NevtToRun: " << NevtToRun_ << ", skip: " << skip << ", dRCut: " << dRCut << endl;

    // Optimized cut values for vertex finding algorithm
    TString EvtVtx_map_filename = "/sphenix/user/hjheng/TrackletAna/minitree/INTT/VtxEvtMap_ana382_zvtx-20cm/INTTVtxZ.root";
    TString infilename = "/sphenix/user/hjheng/TrackletAna/data/INTT/ana382_zvtx-20cm/INTTRecoClusters_sim_merged.root";
    TString outfilename = Form("/sphenix/user/hjheng/TrackletAna/minitree/INTT/TrackletMinitree_ana382_zvtx-20cm/TrackletAna_minitree_Evt%dto%d_dRcut%s.root", iniEvt, iniEvt + NevtToRun_, number_to_string(dRCut).c_str());

    TrackletData tkldata = {};

    std::map<int, vector<float>> EvtVtx_map = EvtVtx_map_tklcluster(EvtVtx_map_filename.Data());

    TFile *f = new TFile(infilename, "READ");
    TTree *t = (TTree *)f->Get("EventTree");
    t->BuildIndex("event"); // Reference: https://root-forum.cern.ch/t/sort-ttree-entries/13138
    TTreeIndex *index = (TTreeIndex *)t->GetTreeIndex();
    int event, NTruthVtx;
    float TruthPV_trig_x, TruthPV_trig_y, TruthPV_trig_z, centrality_mbd, centrality_mbdquantity;
    vector<int> *ClusLayer = 0;
    vector<float> *ClusX = 0, *ClusY = 0, *ClusZ = 0;
    vector<int> *UniqueAncG4P_PID = 0;
    vector<float> *UniqueAncG4P_Pt = 0, *UniqueAncG4P_Eta = 0, *UniqueAncG4P_Phi = 0, *UniqueAncG4P_E = 0;
    t->SetBranchAddress("event", &event);
    t->SetBranchAddress("NTruthVtx", &NTruthVtx);
    t->SetBranchAddress("TruthPV_trig_x", &TruthPV_trig_x);
    t->SetBranchAddress("TruthPV_trig_y", &TruthPV_trig_y);
    t->SetBranchAddress("TruthPV_trig_z", &TruthPV_trig_z);
    t->SetBranchAddress("centrality_mbd", &centrality_mbd);
    t->SetBranchAddress("centrality_mbdquantity", &centrality_mbdquantity);
    t->SetBranchAddress("ClusLayer", &ClusLayer);
    t->SetBranchAddress("ClusX", &ClusX);
    t->SetBranchAddress("ClusY", &ClusY);
    t->SetBranchAddress("ClusZ", &ClusZ);

    TFile *outfile = new TFile(outfilename, "RECREATE");
    TTree *minitree = new TTree("minitree", "Minitree of Reconstructed Tracklets");
    SetMinitree(minitree, tkldata);

    for (int i = skip; i < skip + NevtToRun_; i++)
    {
        Long64_t local = t->LoadTree(index->GetIndex()[i]);
        t->GetEntry(local);

        // cout << "event = " << event << " local = " << local << endl;
        vector<float> PV = EvtVtx_map[event];
        tkldata.event = event;
        tkldata.PV_x = PV[0];
        tkldata.PV_y = PV[1];
        tkldata.PV_z = PV[2];
        tkldata.TruthPV_x = TruthPV_trig_x;
        tkldata.TruthPV_y = TruthPV_trig_y;
        tkldata.TruthPV_z = TruthPV_trig_z;
        // Centrality
        tkldata.Centrality_mbd = centrality_mbd;
        tkldata.Centrality_mbdquantity = centrality_mbdquantity;

        cout << "[Event info] Event = " << event << endl;

        // A selection to select events -> mimic 0-pileup environment
        tkldata.pu0_sel = (NTruthVtx == 1) ? true : false;
        tkldata.trig = true;
        tkldata.process = 0; // Exclude single diffractive events

        // Prepare clusters
        for (size_t ihit = 0; ihit < ClusLayer->size(); ihit++)
        {
            if (ClusLayer->at(ihit) == 3 || ClusLayer->at(ihit) == 4)
            {
                Hit *hit = new Hit(ClusX->at(ihit), ClusY->at(ihit), ClusZ->at(ihit), PV[0], PV[1], PV[2], 0);
                tkldata.layers[0].push_back(hit);
            }
            else if (ClusLayer->at(ihit) == 5 || ClusLayer->at(ihit) == 6)
            {
                Hit *hit = new Hit(ClusX->at(ihit), ClusY->at(ihit), ClusZ->at(ihit), PV[0], PV[1], PV[2], 1);
                tkldata.layers[1].push_back(hit);
            }
            else
            {
                cout << "[WARNING] Unknown layer: " << ClusLayer->at(ihit) << endl; // Should not happen
                continue;
            }
        }

        tkldata.NClusLayer1 = tkldata.layers[0].size();

        // Tracklet reconstruction: proto-tracklets -> reco-tracklets -> gen-hadron matching
        ProtoTracklets(tkldata, dRCut);
        RecoTracklets(tkldata);

        minitree->Fill();
        ResetVec(tkldata);
        cout << "----------" << endl;
    }

    outfile->cd();
    minitree->Write("", TObject::kOverwrite);
    outfile->Close();

    f->Close();

    return 0;
}
