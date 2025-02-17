#include <TDatabasePDG.h>
#include <TFile.h>
#include <TMath.h>
#include <TObjString.h>
#include <TParticle.h>
#include <TParticlePDG.h>
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

bool verbose_debug = false;
bool useetadepadccut = false;
bool savedetail = true;

int main(int argc, char *argv[])
{
    if (argc != 11)
    {
        cout << "Usage: ./TrackletAna [isdata] [evt-vtx map file] [infile] [outfile] [NevtToRun] [dRCut] [random zvtx] [random cluster set number] [clusadccutset] [clusphisizecutset]" << endl;
        exit(1);
    }

    // print out the input parameters
    for (int i = 0; i < argc; i++)
    {
        cout << "argv[" << i << "] = " << argv[i] << endl;
    }

    bool IsData = (TString(argv[1]).Atoi() == 1) ? true : false;
    TString EvtVtx_map_filename = TString(argv[2]);
    TString infilename = TString(argv[3]);
    TString outfilename = TString(argv[4]);
    int NevtToRun_ = TString(argv[5]).Atoi();
    float dRCut = TString(argv[6]).Atof();
    bool userandomzvtx = (TString(argv[7]).Atoi() == 1) ? true : false;
    int randclusset = TString(argv[8]).Atoi();
    int clusadccutset = TString(argv[9]).Atoi();
    int clusphisizecutset = TString(argv[10]).Atoi();

    TString idxstr = (IsData) ? "INTT_BCO" : "event";

    TRandom3 *rnd = new TRandom3(0); // Random number generator

    TrackletData tkldata = {};

    std::map<int, vector<float>> EvtVtxMap_event = EvtVtx_map_event(EvtVtx_map_filename.Data());          // use with simulation
    std::map<uint64_t, vector<float>> EvtVtxMap_inttbco = EvtVtx_map_inttbco(EvtVtx_map_filename.Data()); // use with data

    // Vertex Z reweighting
    TH1F *hM_vtxzweight = VtxZ_ReweiHist("/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/RecoPV_ana/HIJING_ananew_20250210/VtxZ_reweight_HIJING_ananew_20250210.root", "VtxZ_reweight_VtxZm10to10");

    // Random cluster
    int Nrandclus = RandomHit(randclusset);
    std::cout << "Number of random clusters: " << Nrandclus << std::endl;

    // cluster adc cut
    int clusadccut = ConstADCCut(clusadccutset);
    // cluster phi-size cut
    float clusphisizecut = ConstClusPhiCut(clusphisizecutset);
    // print out cut info
    std::cout << "Cluster ADC cut (greater than): " << clusadccut << "; Cluster Phi-size cut (smaller than): " << clusphisizecut << std::endl;

    TFile *f = new TFile(infilename, "READ");
    TTree *t = (TTree *)f->Get("EventTree");
    t->BuildIndex(idxstr); // Reference: https://root-forum.cern.ch/t/sort-ttree-entries/13138
    TTreeIndex *index = (TTreeIndex *)t->GetTreeIndex();
    int event, NTruthVtx, signal_process_id;
    vector<int> *firedTriggers = 0;
    uint64_t INTT_BCO;
    bool is_min_bias, InttBco_IsToBeRemoved;
    float TruthPV_trig_x, TruthPV_trig_y, TruthPV_trig_z, centrality_mbd;
    float mbd_south_charge_sum, mbd_north_charge_sum, mbd_charge_sum, mbd_charge_asymm, mbd_z_vtx;
    vector<int> *ClusLayer = 0;
    vector<float> *ClusX = 0, *ClusY = 0, *ClusZ = 0, *ClusPhiSize = 0, *ClusZSize = 0;
    vector<unsigned int> *ClusAdc = 0;
    vector<int> *ClusMatchedG4P_MaxE_trackID = 0, *ClusMatchedG4P_MaxClusE_ancestorTrackID = 0;
    vector<float> *ClusMatchedG4P_MaxE_Pt = 0, *ClusMatchedG4P_MaxE_Eta = 0, *ClusMatchedG4P_MaxE_Phi = 0;
    vector<int> *PrimaryG4P_PID = 0, *PrimaryG4P_trackID = 0;
    vector<float> *PrimaryG4P_Pt = 0, *PrimaryG4P_Eta = 0, *PrimaryG4P_Phi = 0, *PrimaryG4P_E = 0;
    vector<bool> *PrimaryG4P_isChargeHadron = 0;
    t->SetBranchAddress("event", &event);
    t->SetBranchAddress("INTT_BCO", &INTT_BCO);
    if (!IsData)
    {
        t->SetBranchAddress("NTruthVtx", &NTruthVtx);
        t->SetBranchAddress("signal_process_id", &signal_process_id);
        t->SetBranchAddress("TruthPV_trig_x", &TruthPV_trig_x);
        t->SetBranchAddress("TruthPV_trig_y", &TruthPV_trig_y);
        t->SetBranchAddress("TruthPV_trig_z", &TruthPV_trig_z);
        t->SetBranchAddress("ClusMatchedG4P_MaxE_trackID", &ClusMatchedG4P_MaxE_trackID);
        t->SetBranchAddress("ClusMatchedG4P_MaxClusE_ancestorTrackID", &ClusMatchedG4P_MaxClusE_ancestorTrackID);
        t->SetBranchAddress("ClusMatchedG4P_MaxE_Pt", &ClusMatchedG4P_MaxE_Pt);
        t->SetBranchAddress("ClusMatchedG4P_MaxE_Eta", &ClusMatchedG4P_MaxE_Eta);
        t->SetBranchAddress("ClusMatchedG4P_MaxE_Phi", &ClusMatchedG4P_MaxE_Phi);
        t->SetBranchAddress("PrimaryG4P_PID", &PrimaryG4P_PID);
        t->SetBranchAddress("PrimaryG4P_isChargeHadron", &PrimaryG4P_isChargeHadron);
        t->SetBranchAddress("PrimaryG4P_trackID", &PrimaryG4P_trackID);
        t->SetBranchAddress("PrimaryG4P_Pt", &PrimaryG4P_Pt);
        t->SetBranchAddress("PrimaryG4P_Eta", &PrimaryG4P_Eta);
        t->SetBranchAddress("PrimaryG4P_Phi", &PrimaryG4P_Phi);
        t->SetBranchAddress("PrimaryG4P_E", &PrimaryG4P_E);
        InttBco_IsToBeRemoved = false;
    }
    if (IsData)
    {
        t->SetBranchAddress("firedTriggers", &firedTriggers);
        t->SetBranchAddress("InttBco_IsToBeRemoved", &InttBco_IsToBeRemoved);
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
    SetMinitree(minitree, tkldata, savedetail);

    for (int i = 0; i < NevtToRun_; i++)
    {
        Long64_t local = t->LoadTree(index->GetIndex()[i]);
        t->GetEntry(local);

        cout << "i = " << i << " event = " << event << " INTT BCO = " << INTT_BCO << " NevtToRun_ = " << NevtToRun_ << " local = " << local << " InttBco_IsToBeRemoved = " << InttBco_IsToBeRemoved << endl;

        // Discard events with a small mutual BCO difference between adjacent events
        if (InttBco_IsToBeRemoved)
            continue;

        vector<float> PV = (IsData) ? EvtVtxMap_inttbco[INTT_BCO] : EvtVtxMap_event[event];
        tkldata.event = event;
        tkldata.INTT_BCO = INTT_BCO;
        tkldata.PV_x = PV[0];
        tkldata.PV_y = PV[1];
        float PVz = (userandomzvtx) ? rnd->Uniform(-30, 30) : PV[2];
        tkldata.PV_z = PVz;

        if (!IsData)
        {
            tkldata.TruthPV_x = TruthPV_trig_x;
            tkldata.TruthPV_y = TruthPV_trig_y;
            tkldata.TruthPV_z = TruthPV_trig_z;
        }

        if (PV[2] < -10 || PV[2] > 10)
        {
            tkldata.vtxzwei = 1.;
        }
        else
        {
            if (IsData)
                tkldata.vtxzwei = 1;
            else
                tkldata.vtxzwei = hM_vtxzweight->GetBinContent(hM_vtxzweight->FindBin(PV[2]));
        }

        tkldata.firedTrig10_MBDSNgeq2 = (IsData) ? (find(firedTriggers->begin(), firedTriggers->end(), 10) != firedTriggers->end()) : true;
        tkldata.firedTrig12_vtxle10cm = (IsData) ? (find(firedTriggers->begin(), firedTriggers->end(), 12) != firedTriggers->end()) : true;
        tkldata.firedTrig13_vtxle30cm = (IsData) ? (find(firedTriggers->begin(), firedTriggers->end(), 13) != firedTriggers->end()) : true;
        tkldata.MbdNSge0 = (mbd_north_charge_sum > 0 || mbd_south_charge_sum > 0);
        tkldata.MbdZvtxle10cm = (mbd_z_vtx > -10 && mbd_z_vtx < 10);
        tkldata.validMbdVtx = (mbd_z_vtx == mbd_z_vtx);
        tkldata.InttBco_IsToBeRemoved = (IsData) ? InttBco_IsToBeRemoved : false;

        // Centrality
        tkldata.centrality_mbd = centrality_mbd; 
        tkldata.mbd_z_vtx = mbd_z_vtx;
        tkldata.mbd_south_charge_sum = mbd_south_charge_sum;
        tkldata.mbd_north_charge_sum = mbd_north_charge_sum;
        tkldata.mbd_charge_sum = mbd_charge_sum;
        tkldata.mbd_charge_asymm = mbd_charge_asymm;

        // A selection to select events -> mimic 0-pileup environment
        tkldata.pu0_sel = (NTruthVtx == 1) ? true : false;
        tkldata.is_min_bias = is_min_bias;
        tkldata.process = (IsData) ? -1E9 : signal_process_id; // Exclude single diffractive events

        cout << "[Event info] Event " << event << " firedTrig10_MBDSNgeq2 = " << tkldata.firedTrig10_MBDSNgeq2 << " firedTrig12_vtxle10cm = " << tkldata.firedTrig12_vtxle10cm << "; firedTrig13_vtxle30cm = " << tkldata.firedTrig13_vtxle30cm << "; MbdNSge0 = " << tkldata.MbdNSge0 << "; MbdZvtxle10cm = " << tkldata.MbdZvtxle10cm << "; validMbdVtx = " << tkldata.validMbdVtx << endl;

        // Prepare clusters
        for (size_t ihit = 0; ihit < ClusLayer->size(); ihit++)
        {
            if (ClusLayer->at(ihit) < 3 || ClusLayer->at(ihit) > 6)
            {
                cout << "[WARNING] Unknown layer: " << ClusLayer->at(ihit) << endl; // Should not happen
                continue;
            }

            int layer = (ClusLayer->at(ihit) == 3 || ClusLayer->at(ihit) == 4) ? 0 : 1;
            Hit *hit = new Hit(ClusX->at(ihit), ClusY->at(ihit), ClusZ->at(ihit), PV[0], PV[1], PVz, layer, ClusPhiSize->at(ihit), ClusAdc->at(ihit));

            if (ClusAdc->at(ihit) <= clusadccut)
                continue;

            if (ClusPhiSize->at(ihit) > clusphisizecut)
                continue;

            if (!IsData)
            {
                hit->SetMatchedG4P(ClusMatchedG4P_MaxE_trackID->at(ihit), ClusMatchedG4P_MaxClusE_ancestorTrackID->at(ihit), ClusMatchedG4P_MaxE_Pt->at(ihit), ClusMatchedG4P_MaxE_Eta->at(ihit), ClusMatchedG4P_MaxE_Phi->at(ihit));
            }

            tkldata.layers[layer].push_back(hit);
            tkldata.cluslayer.push_back(layer);
            tkldata.clusphi.push_back(hit->Phi());
            tkldata.cluseta.push_back(hit->Eta());
            tkldata.clusphisize.push_back(ClusPhiSize->at(ihit));
            tkldata.cluszsize.push_back(ClusZSize->at(ihit));
            tkldata.clusadc.push_back(ClusAdc->at(ihit));
        }

        std::cout << "Number of clusters before selections: " << ClusLayer->size() << ", after selections: " << tkldata.cluslayer.size() << std::endl;

        // For systematic: adding random clusters (after selections)
        // int Nrandclus = RandomHit(randclusset);
        if (Nrandclus > 0)
        {
            for (int irand = 0; irand < Nrandclus; irand++)
            {
                // Hit *RandomHit(float vx, float vy, float vz, int layer)
                Hit *randhit_inner = RandomHit(PV[0], PV[1], PVz, 0);
                Hit *randhit_outer = RandomHit(PV[0], PV[1], PVz, 1);
                tkldata.layers[0].push_back(randhit_inner);
                tkldata.layers[1].push_back(randhit_outer);
            }
        }

        tkldata.NClusLayer1 = tkldata.layers[0].size();
        tkldata.NClusLayer2 = tkldata.layers[1].size();

        ProtoTracklets(tkldata, dRCut);
        RecoTracklets(tkldata);

        if (!IsData)
        {
            std::map<int, unsigned int> PrimaryG4PPID_count, absPrimaryG4PPID_count;
            PrimaryG4PPID_count.clear();
            absPrimaryG4PPID_count.clear();

            // Generated charged hadrons
            for (size_t ihad = 0; ihad < PrimaryG4P_PID->size(); ihad++)
            {
                TString particleclass = TString(TDatabasePDG::Instance()->GetParticle(PrimaryG4P_PID->at(ihad))->ParticleClass());
                bool isStable = (TDatabasePDG::Instance()->GetParticle(PrimaryG4P_PID->at(ihad))->Stable() == 1) ? true : false;
                double charge = TDatabasePDG::Instance()->GetParticle(PrimaryG4P_PID->at(ihad))->Charge();
                double lifetime = TDatabasePDG::Instance()->GetParticle(PrimaryG4P_PID->at(ihad))->Lifetime(); // proper lifetime (https://root.cern.ch/doc/master/classTParticlePDG.html#a10fd025a1e867ec3b27903c1b7d2f899)
                double decaylength = lifetime * TMath::C() * 1E2;                                              // decay length in cm
                bool isHadron = (particleclass.Contains("Baryon") || particleclass.Contains("Meson"));
                bool isChargeHadron = (isStable && (charge != 0) && isHadron);
                bool isChargeHadron_alt = is_chargedHadron(PrimaryG4P_PID->at(ihad));
                bool twodefsame = (isChargeHadron == isChargeHadron_alt) ? true : false;

                if (verbose_debug)
                {
                    cout << std::left << std::setw(5) << "PID = " << std::setw(5) << PrimaryG4P_PID->at(ihad);
                    cout << std::left << std::setw(20) << " Particle class = " << std::setw(15) << particleclass;
                    cout << std::left << std::setw(12) << " Stable = " << std::setw(5) << isStable;
                    cout << std::left << std::setw(20) << " Proper lifetime = " << std::setw(15) << lifetime;
                    cout << std::left << std::setw(18) << " Decay length = " << std::setw(15) << decaylength;
                    cout << std::left << std::setw(12) << " Charge = " << std::setw(5) << charge;
                    cout << std::left << std::setw(15) << " Is hadron = " << std::setw(5) << isHadron;
                    cout << std::left << std::setw(23) << " Is charged hadron = " << std::setw(5) << isChargeHadron;
                    cout << std::left << std::setw(30) << " Is charged hadron (alt) = " << std::setw(5) << isChargeHadron_alt;
                    cout << std::left << std::setw(34) << " Two definitions are the same = " << std::setw(5) << twodefsame << endl;
                }

                if (verbose_debug)
                {
                    PrimaryG4PPID_count[PrimaryG4P_PID->at(ihad)]++;
                    absPrimaryG4PPID_count[abs(PrimaryG4P_PID->at(ihad))]++;
                }
                

                tkldata.PrimaryG4P_trackID.push_back(PrimaryG4P_trackID->at(ihad));
                tkldata.PrimaryG4P_Pt.push_back(PrimaryG4P_Pt->at(ihad));
                tkldata.PrimaryG4P_eta.push_back(PrimaryG4P_Eta->at(ihad));
                tkldata.PrimaryG4P_phi.push_back(PrimaryG4P_Phi->at(ihad));
                tkldata.PrimaryG4P_IsRecotkl.push_back(false); // default

                if (is_chargedHadron(PrimaryG4P_PID->at(ihad)) == false)
                    continue;

                // GenHadron *genhadron = new GenHadron(PrimaryG4P_Pt->at(ihad), PrimaryG4P_Eta->at(ihad), PrimaryG4P_Phi->at(ihad), PrimaryG4P_E->at(ihad));
                // tkldata.GenHadrons.push_back(genhadron);
                tkldata.GenHadron_PID.push_back(PrimaryG4P_PID->at(ihad));
                tkldata.GenHadron_trackID.push_back(PrimaryG4P_trackID->at(ihad));
                tkldata.GenHadron_Pt.push_back(PrimaryG4P_Pt->at(ihad));
                tkldata.GenHadron_eta.push_back(PrimaryG4P_Eta->at(ihad));
                tkldata.GenHadron_phi.push_back(PrimaryG4P_Phi->at(ihad));
                tkldata.GenHadron_IsRecotkl.push_back(false); // default
            }
            tkldata.NGenHadron = tkldata.GenHadron_PID.size();

            if (verbose_debug)
            {
                // print PrimaryG4PPID_count
                cout << "--------------------------------------------------------------------" << endl;
                for (auto it = PrimaryG4PPID_count.begin(); it != PrimaryG4PPID_count.end(); it++)
                {
                    cout << "PID = " << it->first << "; Count = " << it->second << endl;
                }
                cout << "--------------------------------------------------------------------" << endl;
                for (auto it = absPrimaryG4PPID_count.begin(); it != absPrimaryG4PPID_count.end(); it++)
                {
                    cout << "|PID| = " << it->first << "; Count = " << it->second << endl;
                }
                cout << "--------------------------------------------------------------------" << endl;

            }

            RecoTkl_isG4P(tkldata); 
        }

        cout << "Number of clusters = " << tkldata.cluslayer.size() << "; Number of reco tracklets = " << tkldata.NRecotkl_Raw << endl;

        if (verbose_debug)
            PrintVecSize(tkldata);

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
