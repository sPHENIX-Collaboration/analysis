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
#include "pdgidfunc.h"

int main(int argc, char *argv[])
{
    // Usage: ./TrackletAna [isdata] [evt-vtx map file] [infile] [outfile] [NevtToRun] [skip] [dRCut]
    bool IsData = (TString(argv[1]).Atoi() == 1) ? true : false;
    TString EvtVtx_map_filename = TString(argv[2]);
    TString infilename = TString(argv[3]);
    TString outfilename = TString(argv[4]);
    int NevtToRun_ = TString(argv[5]).Atoi();
    float dRCut = TString(argv[6]).Atof();
    
    // int iniEvt = skip;

    cout << "[Run Info] Event-vertex map file = " << EvtVtx_map_filename << endl
         << "           Input file = " << infilename << endl
         << "           Output file = " << outfilename << endl
         << "           Number of events to run = " << NevtToRun_ << endl
         << "           dRCut = " << dRCut << endl
         << "-----------" << endl;

    cout << "[Run Info] NevtToRun: " << NevtToRun_ << ", dRCut: " << dRCut << endl;

    TrackletData tkldata = {};

    std::map<int, vector<float>> EvtVtx_map = EvtVtx_map_tklcluster(EvtVtx_map_filename.Data());

    TFile *f = new TFile(infilename, "READ");
    TTree *t = (TTree *)f->Get("EventTree");
    t->BuildIndex("event"); // Reference: https://root-forum.cern.ch/t/sort-ttree-entries/13138
    TTreeIndex *index = (TTreeIndex *)t->GetTreeIndex();
    int event, NTruthVtx;
    float TruthPV_trig_x, TruthPV_trig_y, TruthPV_trig_z, centrality_mbd, centrality_mbdquantity;
    vector<int> *ClusLayer = 0;
    vector<float> *ClusX = 0, *ClusY = 0, *ClusZ = 0, *ClusPhiSize = 0, *ClusZSize = 0;
    vector<int> *UniqueAncG4P_PID = 0;
    vector<float> *UniqueAncG4P_Pt = 0, *UniqueAncG4P_Eta = 0, *UniqueAncG4P_Phi = 0, *UniqueAncG4P_E = 0;
    t->SetBranchAddress("event", &event);
    if (!IsData)
    {
        t->SetBranchAddress("NTruthVtx", &NTruthVtx);
        t->SetBranchAddress("TruthPV_trig_x", &TruthPV_trig_x);
        t->SetBranchAddress("TruthPV_trig_y", &TruthPV_trig_y);
        t->SetBranchAddress("TruthPV_trig_z", &TruthPV_trig_z);
        t->SetBranchAddress("UniqueAncG4P_PID", &UniqueAncG4P_PID);
        t->SetBranchAddress("UniqueAncG4P_Pt", &UniqueAncG4P_Pt);
        t->SetBranchAddress("UniqueAncG4P_Eta", &UniqueAncG4P_Eta);
        t->SetBranchAddress("UniqueAncG4P_Phi", &UniqueAncG4P_Phi);
        t->SetBranchAddress("UniqueAncG4P_E", &UniqueAncG4P_E);
    }
    t->SetBranchAddress("centrality_mbd", &centrality_mbd);
    t->SetBranchAddress("centrality_mbdquantity", &centrality_mbdquantity);
    t->SetBranchAddress("ClusLayer", &ClusLayer);
    t->SetBranchAddress("ClusX", &ClusX);
    t->SetBranchAddress("ClusY", &ClusY);
    t->SetBranchAddress("ClusZ", &ClusZ);
    t->SetBranchAddress("ClusPhiSize", &ClusPhiSize);
    t->SetBranchAddress("ClusZSize", &ClusZSize);

    TFile *outfile = new TFile(outfilename, "RECREATE");
    TTree *minitree = new TTree("minitree", "Minitree of Reconstructed Tracklets");
    SetMinitree(minitree, tkldata);

    for (int i = 0; i < NevtToRun_; i++)
    {
        Long64_t local = t->LoadTree(index->GetIndex()[i]);
        t->GetEntry(local);

        // cout << "event = " << event << " local = " << local << endl;
        vector<float> PV = EvtVtx_map[event];
        tkldata.event = event;
        tkldata.PV_x = PV[0];
        tkldata.PV_y = PV[1];
        tkldata.PV_z = PV[2];
        if (!IsData)
        {
            tkldata.TruthPV_x = TruthPV_trig_x;
            tkldata.TruthPV_y = TruthPV_trig_y;
            tkldata.TruthPV_z = TruthPV_trig_z;
        }
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
            if (ClusLayer->at(ihit) < 3 || ClusLayer->at(ihit) > 6)
            {
                cout << "[WARNING] Unknown layer: " << ClusLayer->at(ihit) << endl; // Should not happen
                continue;
            }

            int layer = (ClusLayer->at(ihit) == 3 || ClusLayer->at(ihit) == 4) ? 0 : 1;
            Hit *hit = new Hit(ClusX->at(ihit), ClusY->at(ihit), ClusZ->at(ihit), PV[0], PV[1], PV[2], 0);
            tkldata.layers[layer].push_back(hit);
            tkldata.clusphi.push_back(hit->Phi());
            tkldata.cluseta.push_back(hit->Eta());
            tkldata.clusphisize.push_back(ClusPhiSize->at(ihit));
            tkldata.cluszsize.push_back(ClusZSize->at(ihit));
        }

        tkldata.NClusLayer1 = tkldata.layers[0].size();

        // Tracklet reconstruction: proto-tracklets -> reco-tracklets -> gen-hadron matching
        ProtoTracklets(tkldata, dRCut);
        RecoTracklets(tkldata);
        if (!IsData)
        {
            // Generated charged hadrons
            for (size_t ihad = 0; ihad < UniqueAncG4P_PID->size(); ihad++)
            {
                if (is_chargedHadron(UniqueAncG4P_PID->at(ihad)) == false)
                    continue;

                GenHadron *genhadron = new GenHadron(UniqueAncG4P_Pt->at(ihad), UniqueAncG4P_Eta->at(ihad), UniqueAncG4P_Phi->at(ihad), UniqueAncG4P_E->at(ihad));
                tkldata.GenHadrons.push_back(genhadron);
                tkldata.GenHadron_Pt.push_back(UniqueAncG4P_Pt->at(ihad));
                tkldata.GenHadron_eta.push_back(UniqueAncG4P_Eta->at(ihad));
                tkldata.GenHadron_phi.push_back(UniqueAncG4P_Phi->at(ihad));
                tkldata.GenHadron_E.push_back(UniqueAncG4P_E->at(ihad));
            }
            tkldata.NGenHadron = tkldata.GenHadrons.size();

            GenMatch_Recotkl(tkldata);

            cout << "NCluster layer 1 = " << tkldata.NClusLayer1 << "; NRecotkl_Raw = " << tkldata.NRecotkl_Raw << "; NRecotkl_GenMatched = " << tkldata.NRecotkl_GenMatched << endl;
        }
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
