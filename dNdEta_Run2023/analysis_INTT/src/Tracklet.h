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
    bool isdata;
    vector<vector<Hit *>> layers = {{}, {}};
    vector<Tracklet *> ProtoTkls, RecoTkls, RecoTkls_GenMatched;
    vector<GenHadron *> GenHadrons;

    int event, NClusLayer1, NPrototkl, NRecotkl_Raw, NRecotkl_GenMatched, NGenHadron;
    float PV_x, PV_y, PV_z, TruthPV_x, TruthPV_y, TruthPV_z;
    float Centrality_mbd, Centrality_mbdquantity;
    bool pu0_sel, trig;
    int process; // single diffractive process
    vector<float> clusphi, cluseta, clusphisize, cluszsize;
    vector<float> prototkl_eta, prototkl_phi, prototkl_deta, prototkl_dphi, prototkl_dR;
    vector<float> recotklraw_eta, recotklraw_phi, recotklraw_deta, recotklraw_dphi, recotklraw_dR;
    vector<float> recotklgm_eta, recotklgm_phi, recotklgm_deta, recotklgm_dphi, recotklgm_dR;
    vector<float> GenHadron_Pt, GenHadron_eta, GenHadron_phi, GenHadron_E, GenHadron_matched_Pt, GenHadron_matched_eta, GenHadron_matched_phi, GenHadron_matched_E;
};

void SetMinitree(TTree *outTree, TrackletData &tkldata)
{
    outTree->Branch("event", &tkldata.event);
    outTree->Branch("NClusLayer1", &tkldata.NClusLayer1);
    outTree->Branch("NPrototkl", &tkldata.NPrototkl);
    outTree->Branch("NRecotkl_Raw", &tkldata.NRecotkl_Raw);
    outTree->Branch("Centrality_mbd", &tkldata.Centrality_mbd);
    outTree->Branch("Centrality_mbdquantity", &tkldata.Centrality_mbdquantity);
    outTree->Branch("PV_x", &tkldata.PV_x);
    outTree->Branch("PV_y", &tkldata.PV_y);
    outTree->Branch("PV_z", &tkldata.PV_z);
    outTree->Branch("trig", &tkldata.trig);
    outTree->Branch("clusPhi", &tkldata.clusphi);
    outTree->Branch("clusEta", &tkldata.cluseta);
    outTree->Branch("clusPhiSize", &tkldata.clusphisize);
    outTree->Branch("clusZSize", &tkldata.cluszsize);    
    outTree->Branch("prototkl_eta", &tkldata.prototkl_eta);
    outTree->Branch("prototkl_phi", &tkldata.prototkl_phi);
    outTree->Branch("prototkl_deta", &tkldata.prototkl_deta);
    outTree->Branch("prototkl_dphi", &tkldata.prototkl_dphi);
    outTree->Branch("prototkl_dR", &tkldata.prototkl_dR);
    outTree->Branch("recotklraw_eta", &tkldata.recotklraw_eta);
    outTree->Branch("recotklraw_phi", &tkldata.recotklraw_phi);
    outTree->Branch("recotklraw_deta", &tkldata.recotklraw_deta);
    outTree->Branch("recotklraw_dphi", &tkldata.recotklraw_dphi);
    outTree->Branch("recotklraw_dR", &tkldata.recotklraw_dR);

    if (!tkldata.isdata)
    {
        outTree->Branch("NRecotkl_GenMatched", &tkldata.NRecotkl_GenMatched);
        outTree->Branch("TruthPV_x", &tkldata.TruthPV_x);
        outTree->Branch("TruthPV_y", &tkldata.TruthPV_y);
        outTree->Branch("TruthPV_z", &tkldata.TruthPV_z);
        outTree->Branch("pu0_sel", &tkldata.pu0_sel);
        outTree->Branch("process", &tkldata.process);
        outTree->Branch("recotklgm_eta", &tkldata.recotklgm_eta);
        outTree->Branch("recotklgm_phi", &tkldata.recotklgm_phi);
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
}

void ResetVec(TrackletData &tkldata)
{
    for (size_t i = 0; i < tkldata.layers.size(); i++)
    {
        CleanVec(tkldata.layers[i]);
    }
    CleanVec(tkldata.ProtoTkls);
    CleanVec(tkldata.RecoTkls);
    CleanVec(tkldata.prototkl_eta);
    CleanVec(tkldata.prototkl_phi);
    CleanVec(tkldata.prototkl_deta);
    CleanVec(tkldata.prototkl_dphi);
    CleanVec(tkldata.prototkl_dR);
    CleanVec(tkldata.recotklraw_eta);
    CleanVec(tkldata.recotklraw_phi);
    CleanVec(tkldata.recotklraw_deta);
    CleanVec(tkldata.recotklraw_dphi);
    CleanVec(tkldata.recotklraw_dR);
    CleanVec(tkldata.recotklgm_eta);
    CleanVec(tkldata.recotklgm_phi);
    CleanVec(tkldata.recotklgm_deta);
    CleanVec(tkldata.recotklgm_dphi);
    CleanVec(tkldata.recotklgm_dR);
    CleanVec(tkldata.GenHadrons);
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
    for (auto ihitl1 : tkldata.layers[0])
    {
        for (auto ihitl2 : tkldata.layers[1])
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
            if (deltaR(tkl->Eta(), tkl->Phi(), ghadron->Eta(), ghadron->Phi()) > 0.1)
                continue;
            else
            {
                tkl->SetMatchedGenHardon();
                ghadron->SetMatchedToRecotkl();
                tkl->SetGenHadron(ghadron);
                tkldata.RecoTkls_GenMatched.push_back(tkl);
                tkldata.recotklgm_eta.push_back(tkl->Hit1()->Eta());
                tkldata.recotklgm_phi.push_back(tkl->Hit1()->Phi());
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
