#ifndef TRACKLET_H
#define TRACKLET_H

#include <TTree.h>

#include "GenHadron.h"
#include "Hit.h"
#include "Utilities.h"

using namespace std;

class Tracklet : public TObject
{
  public:
    Tracklet();
    Tracklet(Hit *hit1, Hit *hit2);
    ~Tracklet();

    void SetHits(Hit *hit1, Hit *hit2);
    Hit *Hit1();
    Hit *Hit2();
    float dEta();
    float dPhi();
    float dR();
    float Eta();
    float Phi();
    float tklVtxZ();
    int LayerComb(); // Layer combination: can only be 12, 23, 13
    void SetMatchedGenHardon();
    bool IsMatchedGenHadron();
    void SetGenHadron(GenHadron *genhad);
    GenHadron *MatchedGenHadron();

  private:
    Hit *_hit1;
    Hit *_hit2;
    float _deta;
    float _dphi;
    bool _matched_genhadron;
    GenHadron *_genhad;
};

Tracklet::Tracklet()
{
    _hit1 = nullptr;
    _hit2 = nullptr;
    _matched_genhadron = false;
    _genhad = nullptr;
}

Tracklet::Tracklet(Hit *hit1, Hit *hit2)
{
    _hit1 = hit1;
    _hit2 = hit2;
    _deta = _hit2->Eta() - _hit1->Eta();
    _dphi = deltaPhi(_hit2->Phi(), _hit1->Phi());
    _matched_genhadron = false;
    _genhad = nullptr;
}

Tracklet::~Tracklet() {}

void Tracklet::SetHits(Hit *hit1, Hit *hit2)
{
    _hit1 = hit1;
    _hit2 = hit2;
    _deta = _hit2->Eta() - _hit1->Eta();
    _dphi = deltaPhi(_hit2->Phi(), _hit1->Phi());
    _matched_genhadron = false;
    _genhad = nullptr;
}

Hit *Tracklet::Hit1() { return (_hit1); }

Hit *Tracklet::Hit2() { return (_hit2); }

float Tracklet::dEta() { return (_deta); }

float Tracklet::dPhi() { return (_dphi); }

float Tracklet::dR() { return (sqrt(_deta * _deta + _dphi * _dphi)); }

float Tracklet::Eta() { return (0.5 * (_hit1->Eta() + _hit2->Eta())); }

float Tracklet::Phi() { return (0.5 * (_hit1->Phi() + _hit2->Phi())); }

float Tracklet::tklVtxZ() { return (_hit1->posZ() - _hit1->rho() * (_hit2->posZ() - _hit1->posZ()) / (_hit2->rho() - _hit1->rho())); }

int Tracklet::LayerComb() { return (_hit1->Layer()) * 10 + (_hit2->Layer()); }

void Tracklet::SetMatchedGenHardon() { _matched_genhadron = true; }

bool Tracklet::IsMatchedGenHadron() { return _matched_genhadron; }

void Tracklet::SetGenHadron(GenHadron *genhad)
{
    _matched_genhadron = true;
    _genhad = genhad;
}

GenHadron *Tracklet::MatchedGenHadron() { return _genhad; }

/*------------------------------------------------------------------------------------------*/

class TrackletData
{
  public:
    vector<vector<Hit *>> MVTXlayers = {{}, {}, {}};
    vector<Tracklet *> ProtoTkls, RecoTkls, RecoTkls_GenMatched;
    vector<GenHadron *> GenHadrons;
    int MVTXl1, MVTXl2;

    int event, NhitsLayer1, NPrototkl, NRecotkl_Raw, NRecotkl_GenMatched, NGenHadron, CentralityBin;
    float PV_z, TruthPV_trig_z, TruthPV_mostNpart_z;
    // bool pu0_sel, vz_sel, o_sel, proc_sel, evt_sel, gen_sel;
    bool pu0_sel, trig;
    int process; // single diffractive process
    vector<float> recoclus_eta_l1, recoclus_eta_l2, recoclus_eta_l3, recoclus_phi_l1, recoclus_phi_l2, recoclus_phi_l3;
    vector<float> prototkl_eta, prototkl_phi, prototkl_clus2eta, prototkl_clus2phi, prototkl_deta, prototkl_dphi, prototkl_dR;
    vector<float> recotklraw_eta, recotklraw_phi, recotklraw_clus2eta, recotklraw_clus2phi, recotklraw_deta, recotklraw_dphi, recotklraw_dR;
    vector<float> recotklgm_eta, recotklgm_phi, recotklgm_clus2eta, recotklgm_clus2phi, recotklgm_deta, recotklgm_dphi, recotklgm_dR;
    vector<float> GenHadron_Pt, GenHadron_eta, GenHadron_phi, GenHadron_E, GenHadron_matched_Pt, GenHadron_matched_eta, GenHadron_matched_phi, GenHadron_matched_E;
};

void RecoclusToMinitree(TrackletData &tkldata)
{
    for (size_t i = 0; i < tkldata.MVTXlayers[0].size(); i++)
    {
        tkldata.recoclus_eta_l1.push_back(tkldata.MVTXlayers[0][i]->Eta());
        tkldata.recoclus_phi_l1.push_back(tkldata.MVTXlayers[0][i]->Phi());
    }

    for (size_t i = 0; i < tkldata.MVTXlayers[1].size(); i++)
    {
        tkldata.recoclus_eta_l2.push_back(tkldata.MVTXlayers[1][i]->Eta());
        tkldata.recoclus_phi_l2.push_back(tkldata.MVTXlayers[1][i]->Phi());
    }

    for (size_t i = 0; i < tkldata.MVTXlayers[2].size(); i++)
    {
        tkldata.recoclus_eta_l3.push_back(tkldata.MVTXlayers[2][i]->Eta());
        tkldata.recoclus_phi_l3.push_back(tkldata.MVTXlayers[2][i]->Phi());
    }
}

void SetMinitree(TTree *outTree, TrackletData &tkldata)
{
    outTree->Branch("event", &tkldata.event);
    outTree->Branch("NhitsLayer1", &tkldata.NhitsLayer1);
    outTree->Branch("NPrototkl", &tkldata.NPrototkl);
    outTree->Branch("NRecotkl_Raw", &tkldata.NRecotkl_Raw);
    outTree->Branch("NRecotkl_GenMatched", &tkldata.NRecotkl_GenMatched);
    outTree->Branch("NGenHadron", &tkldata.NGenHadron);
    outTree->Branch("CentralityBin", &tkldata.CentralityBin);
    outTree->Branch("PV_z", &tkldata.PV_z);
    outTree->Branch("TruthPV_trig_z", &tkldata.TruthPV_trig_z);
    outTree->Branch("TruthPV_mostNpart_z", &tkldata.TruthPV_mostNpart_z);
    outTree->Branch("pu0_sel", &tkldata.pu0_sel);
    outTree->Branch("trig", &tkldata.trig);
    outTree->Branch("process", &tkldata.process);
    // outTree->Branch("vz_sel", &tkldata.vz_sel);
    // outTree->Branch("o_sel", &tkldata.o_sel);
    // outTree->Branch("proc_sel", &tkldata.proc_sel);
    // outTree->Branch("evt_sel", &tkldata.evt_sel);
    // outTree->Branch("gen_sel", &tkldata.gen_sel);
    outTree->Branch("recoclus_eta_l1", &tkldata.recoclus_eta_l1);
    outTree->Branch("recoclus_phi_l1", &tkldata.recoclus_phi_l1);
    outTree->Branch("recoclus_eta_l2", &tkldata.recoclus_eta_l2);
    outTree->Branch("recoclus_phi_l2", &tkldata.recoclus_phi_l2);
    outTree->Branch("recoclus_eta_l3", &tkldata.recoclus_eta_l3);
    outTree->Branch("recoclus_phi_l3", &tkldata.recoclus_phi_l3);
    outTree->Branch("prototkl_eta", &tkldata.prototkl_eta);
    outTree->Branch("prototkl_phi", &tkldata.prototkl_phi);
    outTree->Branch("prototkl_clus2eta", &tkldata.prototkl_clus2eta);
    outTree->Branch("prototkl_clus2phi", &tkldata.prototkl_clus2phi);
    outTree->Branch("prototkl_deta", &tkldata.prototkl_deta);
    outTree->Branch("prototkl_dphi", &tkldata.prototkl_dphi);
    outTree->Branch("prototkl_dR", &tkldata.prototkl_dR);
    outTree->Branch("recotklraw_eta", &tkldata.recotklraw_eta);
    outTree->Branch("recotklraw_phi", &tkldata.recotklraw_phi);
    outTree->Branch("recotklraw_clus2eta", &tkldata.recotklraw_clus2eta);
    outTree->Branch("recotklraw_clus2phi", &tkldata.recotklraw_clus2phi);
    outTree->Branch("recotklraw_deta", &tkldata.recotklraw_deta);
    outTree->Branch("recotklraw_dphi", &tkldata.recotklraw_dphi);
    outTree->Branch("recotklraw_dR", &tkldata.recotklraw_dR);
    outTree->Branch("recotklgm_eta", &tkldata.recotklgm_eta);
    outTree->Branch("recotklgm_phi", &tkldata.recotklgm_phi);
    outTree->Branch("recotklgm_clus2eta", &tkldata.recotklgm_clus2eta);
    outTree->Branch("recotklgm_clus2phi", &tkldata.recotklgm_clus2phi);
    outTree->Branch("recotklgm_deta", &tkldata.recotklgm_deta);
    outTree->Branch("recotklgm_dphi", &tkldata.recotklgm_dphi);
    outTree->Branch("recotklgm_dR", &tkldata.recotklgm_dR);
    outTree->Branch("GenHadron_Pt", &tkldata.GenHadron_Pt);
    outTree->Branch("GenHadron_eta", &tkldata.GenHadron_eta);
    outTree->Branch("GenHadron_phi", &tkldata.GenHadron_phi);
    outTree->Branch("GenHadron_E", &tkldata.GenHadron_E);
    outTree->Branch("GenHadron_matched_Pt", &tkldata.GenHadron_matched_Pt);
    outTree->Branch("GenHadron_matched_eta", &tkldata.GenHadron_matched_eta);
    outTree->Branch("GenHadron_matched_phi", &tkldata.GenHadron_matched_phi);
    outTree->Branch("GenHadron_matched_E", &tkldata.GenHadron_matched_E);
}

void ResetVec(TrackletData &tkldata)
{
    for (size_t i = 0; i < tkldata.MVTXlayers.size(); i++)
    {
        CleanVec(tkldata.MVTXlayers[i]);
    }
    CleanVec(tkldata.recoclus_eta_l1);
    CleanVec(tkldata.recoclus_phi_l1);
    CleanVec(tkldata.recoclus_eta_l2);
    CleanVec(tkldata.recoclus_phi_l2);
    CleanVec(tkldata.recoclus_eta_l3);
    CleanVec(tkldata.recoclus_phi_l3);
    CleanVec(tkldata.ProtoTkls);
    CleanVec(tkldata.RecoTkls);
    CleanVec(tkldata.RecoTkls_GenMatched);
    CleanVec(tkldata.GenHadrons);
    CleanVec(tkldata.prototkl_eta);
    CleanVec(tkldata.prototkl_phi);
    CleanVec(tkldata.prototkl_clus2eta);
    CleanVec(tkldata.prototkl_clus2phi);
    CleanVec(tkldata.prototkl_deta);
    CleanVec(tkldata.prototkl_dphi);
    CleanVec(tkldata.prototkl_dR);
    CleanVec(tkldata.recotklraw_eta);
    CleanVec(tkldata.recotklraw_phi);
    CleanVec(tkldata.recotklraw_clus2eta);
    CleanVec(tkldata.recotklraw_clus2phi);
    CleanVec(tkldata.recotklraw_deta);
    CleanVec(tkldata.recotklraw_dphi);
    CleanVec(tkldata.recotklraw_dR);
    CleanVec(tkldata.recotklgm_eta);
    CleanVec(tkldata.recotklgm_phi);
    CleanVec(tkldata.recotklgm_clus2eta);
    CleanVec(tkldata.recotklgm_clus2phi);
    CleanVec(tkldata.recotklgm_deta);
    CleanVec(tkldata.recotklgm_dphi);
    CleanVec(tkldata.recotklgm_dR);
    CleanVec(tkldata.GenHadron_Pt);
    CleanVec(tkldata.GenHadron_eta);
    CleanVec(tkldata.GenHadron_phi);
    CleanVec(tkldata.GenHadron_E);
    CleanVec(tkldata.GenHadron_matched_Pt);
    CleanVec(tkldata.GenHadron_matched_eta);
    CleanVec(tkldata.GenHadron_matched_phi);
    CleanVec(tkldata.GenHadron_matched_E);
}

bool compare_dR(Tracklet *a, Tracklet *b) { return a->dR() < b->dR(); }

void ProtoTracklets(TrackletData &tkldata, float cutdr)
{
    float Cut_dR = cutdr; // Nominal: 0.5

    int iComb = 0;
    // Get all combinations
    for (auto ihitl1 : tkldata.MVTXlayers[tkldata.MVTXl1])
    {
        for (auto ihitl2 : tkldata.MVTXlayers[tkldata.MVTXl2])
        {
            // iComb++;
            // if (iComb % 500000 == 0)
            //     fprintf(stderr, "processing %d of %d combinations (%.3f %%)\n", iComb, nhits1 * nhits2, (float)iComb / (nhits1 * nhits2) * 100.);

            float dEta = ihitl2->Eta() - ihitl1->Eta();
            float dPhi = deltaPhi(ihitl2->Phi(), ihitl1->Phi());
            float dR = sqrt(dEta * dEta + dPhi * dPhi);

            if (dR < Cut_dR)
            {
                Tracklet *tmptkl = new Tracklet(ihitl1, ihitl2);
                tkldata.ProtoTkls.push_back(tmptkl);

                tkldata.prototkl_eta.push_back(tmptkl->Hit1()->Eta());
                tkldata.prototkl_phi.push_back(tmptkl->Hit1()->Phi());
                tkldata.prototkl_clus2eta.push_back(tmptkl->Hit2()->Eta());
                tkldata.prototkl_clus2phi.push_back(tmptkl->Hit2()->Phi());
                tkldata.prototkl_deta.push_back(tmptkl->dEta());
                tkldata.prototkl_dphi.push_back(tmptkl->dPhi());
                tkldata.prototkl_dR.push_back(tmptkl->dR());
            }
            else
                continue;
        }
    }

    tkldata.NPrototkl = tkldata.ProtoTkls.size();
}

void RecoTracklets(TrackletData &tkldata)
{
    sort(tkldata.ProtoTkls.begin(), tkldata.ProtoTkls.end(), compare_dR);

    int itkl = 0;
    for (auto &tkl : tkldata.ProtoTkls)
    {
        if (tkl->Hit1()->IsMatchedTkl() || tkl->Hit2()->IsMatchedTkl())
        {
            continue;
        }
        else
        {
            tkldata.RecoTkls.push_back(tkl);
            tkl->Hit1()->SetMatchedTkl();
            tkl->Hit2()->SetMatchedTkl();

            tkldata.recotklraw_eta.push_back(tkl->Hit1()->Eta());
            tkldata.recotklraw_phi.push_back(tkl->Hit1()->Phi());
            tkldata.recotklraw_clus2eta.push_back(tkl->Hit2()->Eta());
            tkldata.recotklraw_clus2phi.push_back(tkl->Hit2()->Phi());
            tkldata.recotklraw_deta.push_back(tkl->dEta());
            tkldata.recotklraw_dphi.push_back(tkl->dPhi());
            tkldata.recotklraw_dR.push_back(tkl->dR());
        }
    }

    tkldata.NRecotkl_Raw = tkldata.RecoTkls.size();
}

void GenMatch_Recotkl(TrackletData &tkldata)
{
    for (auto &tkl : tkldata.RecoTkls)
    {
        if (tkl->IsMatchedGenHadron())
            continue;
        for (auto &ghadron : tkldata.GenHadrons)
        {
            if (ghadron->IsMatchedToRecotkl() || tkl->IsMatchedGenHadron())
                continue;
            // Matching criteria
            if (deltaR(tkl->Eta(), tkl->Phi(), ghadron->Eta(), ghadron->Phi()) > 0.05)
                continue;
            else
            {
                tkl->SetMatchedGenHardon();
                ghadron->SetMatchedToRecotkl();
                tkl->SetGenHadron(ghadron);
                tkldata.RecoTkls_GenMatched.push_back(tkl);
                tkldata.recotklgm_eta.push_back(tkl->Hit1()->Eta());
                tkldata.recotklgm_phi.push_back(tkl->Hit1()->Phi());
                tkldata.recotklgm_clus2eta.push_back(tkl->Hit2()->Eta());
                tkldata.recotklgm_clus2phi.push_back(tkl->Hit2()->Phi());
                tkldata.recotklgm_deta.push_back(tkl->dEta());
                tkldata.recotklgm_dphi.push_back(tkl->dPhi());
                tkldata.recotklgm_dR.push_back(tkl->dR());
                tkldata.GenHadron_matched_Pt.push_back(ghadron->Pt());
                tkldata.GenHadron_matched_eta.push_back(ghadron->Eta());
                tkldata.GenHadron_matched_phi.push_back(ghadron->Phi());
                tkldata.GenHadron_matched_E.push_back(ghadron->E());
            }
        }
    }

    tkldata.NRecotkl_GenMatched = tkldata.RecoTkls_GenMatched.size();
}

#endif