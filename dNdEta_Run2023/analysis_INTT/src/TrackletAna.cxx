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
    // Usage: ./TrackletAna [isdata] [evt-vtx map file] [infile] [outfile] [NevtToRun] [dRCut]
    bool IsData = (TString(argv[1]).Atoi() == 1) ? true : false;
    TString EvtVtx_map_filename = TString(argv[2]);
    TString infilename = TString(argv[3]);
    TString outfilename = TString(argv[4]);
    int NevtToRun_ = TString(argv[5]).Atoi();
    float dRCut = TString(argv[6]).Atof();
    bool userandomzvtx = (TString(argv[7]).Atoi() == 1) ? true : false;

    TString idxstr = (IsData) ? "INTT_BCO" : "event";

    TRandom3 *rnd = new TRandom3(0); // Random number generator

    // int iniEvt = skip;

    cout << "[Run Info] Event-vertex map file = " << EvtVtx_map_filename << endl
         << "           Input file = " << infilename << endl
         << "           Output file = " << outfilename << endl
         << "           Number of events to run = " << NevtToRun_ << endl
         << "           dRCut = " << dRCut << endl
         << "-----------" << endl;

    cout << "[Run Info] NevtToRun: " << NevtToRun_ << ", dRCut: " << dRCut << endl;

    TrackletData tkldata = {};

    std::map<int, vector<float>> EvtVtxMap_event = EvtVtx_map_event(EvtVtx_map_filename.Data());          // use with simulation
    std::map<uint64_t, vector<float>> EvtVtxMap_inttbco = EvtVtx_map_inttbco(EvtVtx_map_filename.Data()); // use with data

    // Vertex Z reweighting
    TH1F *hM_vtxzweight = VtxZ_ReweiHist();

    TFile *f = new TFile(infilename, "READ");
    TTree *t = (TTree *)f->Get("EventTree");
    t->BuildIndex(idxstr); // Reference: https://root-forum.cern.ch/t/sort-ttree-entries/13138
    TTreeIndex *index = (TTreeIndex *)t->GetTreeIndex();
    int event, NTruthVtx, signal_process_id;
    uint64_t INTT_BCO;
    bool is_min_bias;
    float TruthPV_trig_x, TruthPV_trig_y, TruthPV_trig_z, centrality_mbd;
    float mbd_south_charge_sum, mbd_north_charge_sum, mbd_charge_sum, mbd_charge_asymm, mbd_z_vtx;
    vector<int> *ClusLayer = 0;
    vector<float> *ClusX = 0, *ClusY = 0, *ClusZ = 0, *ClusPhiSize = 0, *ClusZSize = 0;
    vector<unsigned int> *ClusAdc = 0;
    vector<int> *PrimaryG4P_PID = 0;
    vector<float> *PrimaryG4P_Pt = 0, *PrimaryG4P_Eta = 0, *PrimaryG4P_Phi = 0, *PrimaryG4P_E = 0;
    t->SetBranchAddress("event", &event);
    t->SetBranchAddress("INTT_BCO", &INTT_BCO);
    if (!IsData)
    {
        t->SetBranchAddress("NTruthVtx", &NTruthVtx);
        t->SetBranchAddress("signal_process_id", &signal_process_id);
        t->SetBranchAddress("TruthPV_trig_x", &TruthPV_trig_x);
        t->SetBranchAddress("TruthPV_trig_y", &TruthPV_trig_y);
        t->SetBranchAddress("TruthPV_trig_z", &TruthPV_trig_z);
        t->SetBranchAddress("PrimaryG4P_PID", &PrimaryG4P_PID);
        t->SetBranchAddress("PrimaryG4P_Pt", &PrimaryG4P_Pt);
        t->SetBranchAddress("PrimaryG4P_Eta", &PrimaryG4P_Eta);
        t->SetBranchAddress("PrimaryG4P_Phi", &PrimaryG4P_Phi);
        t->SetBranchAddress("PrimaryG4P_E", &PrimaryG4P_E);
    }
    t->SetBranchAddress("is_min_bias", &is_min_bias);
    t->SetBranchAddress("MBD_centrality", &centrality_mbd);
    t->SetBranchAddress("MBD_z_vtx", &mbd_z_vtx);
    t->SetBranchAddress("MBD_south_charge_sum", &mbd_south_charge_sum);
    t->SetBranchAddress("MBD_north_charge_sum", &mbd_north_charge_sum);
    t->SetBranchAddress("MBD_charge_sum", &mbd_charge_sum);
    t->SetBranchAddress("MBD_charge_asymm", &mbd_charge_asymm);
    t->SetBranchAddress("ClusLayer", &ClusLayer);
    t->SetBranchAddress("ClusX", &ClusX);
    t->SetBranchAddress("ClusY", &ClusY);
    t->SetBranchAddress("ClusZ", &ClusZ);
    t->SetBranchAddress("ClusPhiSize", &ClusPhiSize);
    t->SetBranchAddress("ClusZSize", &ClusZSize);
    t->SetBranchAddress("ClusAdc", &ClusAdc);

    TFile *outfile = new TFile(outfilename, "RECREATE");
    TTree *minitree = new TTree("minitree", "Minitree of Reconstructed Tracklets");
    SetMinitree(minitree, tkldata);

    for (int i = 0; i < NevtToRun_; i++)
    {
        Long64_t local = t->LoadTree(index->GetIndex()[i]);
        t->GetEntry(local);

        cout << "i = " << i << " event = " << event << " INTT BCO = " << INTT_BCO << " NevtToRun_ = " << NevtToRun_ << " local = " << local << endl;
        vector<float> PV = (IsData) ? EvtVtxMap_inttbco[INTT_BCO] : EvtVtxMap_event[event];
        tkldata.event = event;
        tkldata.INTT_BCO = INTT_BCO;
        tkldata.PV_x = PV[0];
        tkldata.PV_y = PV[1];
        // float randomzvtx = rnd->Uniform(-50, 50);
        float PVz = (userandomzvtx) ? rnd->Uniform(-30, -10) : PV[2];
        tkldata.PV_z = PVz;
        
        if (!IsData)
        {
            tkldata.TruthPV_x = TruthPV_trig_x;
            tkldata.TruthPV_y = TruthPV_trig_y;
            tkldata.TruthPV_z = TruthPV_trig_z;
        }

        if (PV[2] < -40 || PV[2] > 0)
        {
            tkldata.vtxzwei = 0;
        }
        else
        {
            if (IsData)
                tkldata.vtxzwei = 1;
            else
                tkldata.vtxzwei = hM_vtxzweight->GetBinContent(hM_vtxzweight->FindBin(PV[2]));
        }

        // Centrality
        tkldata.centrality_mbd = centrality_mbd;
        tkldata.mbd_z_vtx = mbd_z_vtx;
        tkldata.mbd_south_charge_sum = mbd_south_charge_sum;
        tkldata.mbd_north_charge_sum = mbd_north_charge_sum;
        tkldata.mbd_charge_sum = mbd_charge_sum;
        tkldata.mbd_charge_asymm = mbd_charge_asymm;

        cout << "[Event info] Event = " << event << endl;

        // A selection to select events -> mimic 0-pileup environment
        tkldata.pu0_sel = (NTruthVtx == 1) ? true : false;
        tkldata.is_min_bias = is_min_bias;
        tkldata.process = (IsData) ? -1E9 : signal_process_id; // Exclude single diffractive events

        // Prepare clusters
        for (size_t ihit = 0; ihit < ClusLayer->size(); ihit++)
        {
            if (ClusLayer->at(ihit) < 3 || ClusLayer->at(ihit) > 6)
            {
                cout << "[WARNING] Unknown layer: " << ClusLayer->at(ihit) << endl; // Should not happen
                continue;
            }

            if (ClusAdc->at(ihit) < 35)
                continue;

            int layer = (ClusLayer->at(ihit) == 3 || ClusLayer->at(ihit) == 4) ? 0 : 1;
            // Hit::Hit(float x, float y, float z, float vtxX, float vtxY, float vtxZ, int layer, float phisize, unsigned int clusadc)
            Hit *hit = new Hit(ClusX->at(ihit), ClusY->at(ihit), ClusZ->at(ihit), PV[0], PV[1], PVz, layer, ClusPhiSize->at(ihit), ClusAdc->at(ihit));
            tkldata.layers[layer].push_back(hit);
            tkldata.cluslayer.push_back(layer);
            tkldata.clusphi.push_back(hit->Phi());
            tkldata.cluseta.push_back(hit->Eta());
            tkldata.clusphisize.push_back(ClusPhiSize->at(ihit));
            tkldata.cluszsize.push_back(ClusZSize->at(ihit));
            tkldata.clusadc.push_back(ClusAdc->at(ihit));
        }

        tkldata.NClusLayer1 = tkldata.layers[0].size();

        // Tracklet reconstruction: proto-tracklets -> reco-tracklets
        ProtoTracklets(tkldata, dRCut);
        RecoTracklets(tkldata);
        if (!IsData)
        {
            // Generated charged hadrons
            for (size_t ihad = 0; ihad < PrimaryG4P_PID->size(); ihad++)
            {
                if (is_chargedHadron(PrimaryG4P_PID->at(ihad)) == false)
                    continue;

                GenHadron *genhadron = new GenHadron(PrimaryG4P_Pt->at(ihad), PrimaryG4P_Eta->at(ihad), PrimaryG4P_Phi->at(ihad), PrimaryG4P_E->at(ihad));
                tkldata.GenHadrons.push_back(genhadron);
                tkldata.GenHadron_PID.push_back(PrimaryG4P_PID->at(ihad));
                tkldata.GenHadron_Pt.push_back(PrimaryG4P_Pt->at(ihad));
                tkldata.GenHadron_eta.push_back(PrimaryG4P_Eta->at(ihad));
                tkldata.GenHadron_phi.push_back(PrimaryG4P_Phi->at(ihad));
                tkldata.GenHadron_E.push_back(PrimaryG4P_E->at(ihad));
            }
            tkldata.NGenHadron = tkldata.GenHadrons.size();

            // GenMatch_Recotkl(tkldata);

            // cout << "NCluster layer 1 = " << tkldata.NClusLayer1 << "; NRecotkl_Raw = " << tkldata.NRecotkl_Raw << "; NRecotkl_GenMatched = " << tkldata.NRecotkl_GenMatched << endl;
        }

        cout << "Number of total inner clusters = " << tkldata.NClusLayer1 << "; Number of tracklet inner clusters = " << tkldata.tklclus1phi.size()
             << "; Number of reco-tracklets = " << tkldata.NRecotkl_Raw << ";" << endl;

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
