#ifndef TRACKLET_H
#define TRACKLET_H

#include <set>

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

    float vtxzwei;

    int event, NClusLayer1, NClusLayer2, NPrototkl, NRecotkl_Raw, NRecotkl_GenMatched, NGenHadron;
    uint64_t INTT_BCO;
    float PV_x, PV_y, PV_z, TruthPV_x, TruthPV_y, TruthPV_z;
    float centrality_mbd;
    float mbd_south_charge_sum, mbd_north_charge_sum, mbd_charge_sum, mbd_charge_asymm, mbd_z_vtx;
    bool pu0_sel, is_min_bias;
    int process; // single diffractive process
    bool firedTrig10_MBDSNgeq2, firedTrig12_vtxle10cm, firedTrig13_vtxle30cm, MbdNSge0, MbdZvtxle10cm, validMbdVtx, InttBco_IsToBeRemoved;

    vector<int> cluslayer;
    vector<float> clusphi, cluseta, clusphisize, cluszsize;
    vector<unsigned int> clusadc;

    vector<float> tklclus1x, tklclus1y, tklclus1z, tklclus2x, tklclus2y, tklclus2z;                     // detailed info
    vector<float> tklclus1phi, tklclus1eta, tklclus2phi, tklclus2eta, tklclus1phisize, tklclus2phisize; // 1=inner, 2=outer
    vector<unsigned int> tklclus1adc, tklclus2adc;

    vector<float> prototkl_eta, prototkl_phi, prototkl_deta, prototkl_dphi, prototkl_dR;
    vector<float> recotklraw_eta, recotklraw_phi, recotklraw_deta, recotklraw_dphi, recotklraw_dR;
    vector<float> recotklraw_dca3dvtx;

    vector<bool> recotkl_isMatchedGChadron;
    vector<int> recotkl_clus1_matchedtrackID, recotkl_clus2_matchedtrackID;
    vector<int> recotkl_clus1_matchedAncestorTrackID, recotkl_clus2_matchedAncestorTrackID;
    vector<int> matchedGChadron_trackID;
    vector<float> matchedGChadron_pt, matchedGChadron_eta, matchedGChadron_phi;

    vector<int> PrimaryG4P_trackID;
    vector<float> PrimaryG4P_Pt, PrimaryG4P_eta, PrimaryG4P_phi;
    vector<bool> PrimaryG4P_IsRecotkl;

    vector<int> GenHadron_PID, GenHadron_trackID;
    vector<float> GenHadron_Pt, GenHadron_eta, GenHadron_phi;
    vector<bool> GenHadron_IsRecotkl;
};

void SetMinitree(TTree *outTree, TrackletData &tkldata, bool detailed = false)
{
    outTree->Branch("event", &tkldata.event);
    outTree->Branch("INTT_BCO", &tkldata.INTT_BCO);
    outTree->Branch("NClusLayer1", &tkldata.NClusLayer1);
    outTree->Branch("NClusLayer2", &tkldata.NClusLayer2);
    outTree->Branch("NPrototkl", &tkldata.NPrototkl);
    outTree->Branch("NRecotkl_Raw", &tkldata.NRecotkl_Raw);
    outTree->Branch("MBD_centrality", &tkldata.centrality_mbd);
    outTree->Branch("MBD_south_charge_sum", &tkldata.mbd_south_charge_sum);
    outTree->Branch("MBD_north_charge_sum", &tkldata.mbd_north_charge_sum);
    outTree->Branch("MBD_charge_sum", &tkldata.mbd_charge_sum);
    outTree->Branch("MBD_charge_asymm", &tkldata.mbd_charge_asymm);
    outTree->Branch("MBD_z_vtx", &tkldata.mbd_z_vtx);
    outTree->Branch("PV_x", &tkldata.PV_x);
    outTree->Branch("PV_y", &tkldata.PV_y);
    outTree->Branch("PV_z", &tkldata.PV_z);
    outTree->Branch("vtxzwei", &tkldata.vtxzwei);
    outTree->Branch("firedTrig10_MBDSNgeq2", &tkldata.firedTrig10_MBDSNgeq2);
    outTree->Branch("firedTrig12_vtxle10cm", &tkldata.firedTrig12_vtxle10cm);
    outTree->Branch("firedTrig13_vtxle30cm", &tkldata.firedTrig13_vtxle30cm);
    outTree->Branch("MbdNSge0", &tkldata.MbdNSge0);
    outTree->Branch("MbdZvtxle10cm", &tkldata.MbdZvtxle10cm);
    outTree->Branch("validMbdVtx", &tkldata.validMbdVtx);
    outTree->Branch("InttBco_IsToBeRemoved", &tkldata.InttBco_IsToBeRemoved);

    outTree->Branch("is_min_bias", &tkldata.is_min_bias);
    outTree->Branch("clusLayer", &tkldata.cluslayer);
    outTree->Branch("clusPhi", &tkldata.clusphi);
    outTree->Branch("clusEta", &tkldata.cluseta);
    outTree->Branch("clusPhiSize", &tkldata.clusphisize);
    outTree->Branch("clusZSize", &tkldata.cluszsize);
    outTree->Branch("clusADC", &tkldata.clusadc);
    if (detailed)
    {
        outTree->Branch("tklclus1x", &tkldata.tklclus1x);
        outTree->Branch("tklclus1y", &tkldata.tklclus1y);
        outTree->Branch("tklclus1z", &tkldata.tklclus1z);
        outTree->Branch("tklclus2x", &tkldata.tklclus2x);
        outTree->Branch("tklclus2y", &tkldata.tklclus2y);
        outTree->Branch("tklclus2z", &tkldata.tklclus2z);
    }
    outTree->Branch("tklclus1Phi", &tkldata.tklclus1phi);
    outTree->Branch("tklclus1Eta", &tkldata.tklclus1eta);
    outTree->Branch("tklclus2Phi", &tkldata.tklclus2phi);
    outTree->Branch("tklclus2Eta", &tkldata.tklclus2eta);
    outTree->Branch("tklclus1PhiSize", &tkldata.tklclus1phisize);
    outTree->Branch("tklclus2PhiSize", &tkldata.tklclus2phisize);
    outTree->Branch("tklclus1ADC", &tkldata.tklclus1adc);
    outTree->Branch("tklclus2ADC", &tkldata.tklclus2adc);
    outTree->Branch("recotklraw_eta", &tkldata.recotklraw_eta);
    outTree->Branch("recotklraw_phi", &tkldata.recotklraw_phi);
    outTree->Branch("recotklraw_deta", &tkldata.recotklraw_deta);
    outTree->Branch("recotklraw_dphi", &tkldata.recotklraw_dphi);
    outTree->Branch("recotklraw_dR", &tkldata.recotklraw_dR);
    outTree->Branch("recotklraw_dca3dvtx", &tkldata.recotklraw_dca3dvtx);

    if (!tkldata.isdata)
    {
        outTree->Branch("recotkl_isMatchedGChadron", &tkldata.recotkl_isMatchedGChadron);
        outTree->Branch("recotkl_clus1_matchedtrackID", &tkldata.recotkl_clus1_matchedtrackID);
        outTree->Branch("recotkl_clus2_matchedtrackID", &tkldata.recotkl_clus2_matchedtrackID);
        outTree->Branch("recotkl_clus1_matchedAncestorTrackID", &tkldata.recotkl_clus1_matchedAncestorTrackID);
        outTree->Branch("recotkl_clus2_matchedAncestorTrackID", &tkldata.recotkl_clus2_matchedAncestorTrackID);
        outTree->Branch("matchedGChadron_pt", &tkldata.matchedGChadron_pt);
        outTree->Branch("matchedGChadron_eta", &tkldata.matchedGChadron_eta);
        outTree->Branch("matchedGChadron_phi", &tkldata.matchedGChadron_phi);
        outTree->Branch("TruthPV_x", &tkldata.TruthPV_x);
        outTree->Branch("TruthPV_y", &tkldata.TruthPV_y);
        outTree->Branch("TruthPV_z", &tkldata.TruthPV_z);
        outTree->Branch("pu0_sel", &tkldata.pu0_sel);
        outTree->Branch("process", &tkldata.process);
        outTree->Branch("PrimaryG4P_Pt", &tkldata.PrimaryG4P_Pt);
        outTree->Branch("PrimaryG4P_eta", &tkldata.PrimaryG4P_eta);
        outTree->Branch("PrimaryG4P_phi", &tkldata.PrimaryG4P_phi);
        outTree->Branch("PrimaryG4P_IsRecotkl", &tkldata.PrimaryG4P_IsRecotkl);
        outTree->Branch("GenHadron_PID", &tkldata.GenHadron_PID);
        outTree->Branch("GenHadron_Pt", &tkldata.GenHadron_Pt);
        outTree->Branch("GenHadron_eta", &tkldata.GenHadron_eta);
        outTree->Branch("GenHadron_phi", &tkldata.GenHadron_phi);
        outTree->Branch("GenHadron_IsRecotkl", &tkldata.GenHadron_IsRecotkl);
    }
}

void ResetVec(TrackletData &tkldata)
{
    for (size_t i = 0; i < tkldata.layers.size(); i++)
    {
        for (auto &hit : tkldata.layers[i])
        {
            delete hit;
        }
        CleanVec(tkldata.layers[i]);
    }
    // delete the pointers and then clear the vector; otherwise, the memory will is not released and will cause memory leak
    for (auto &tkl : tkldata.ProtoTkls)
    {
        delete tkl;
    }
    CleanVec(tkldata.ProtoTkls);
    CleanVec(tkldata.RecoTkls); // Don't need to delete the reco tracklet pointers because they are just pointers to the proto tracklets

    CleanVec(tkldata.prototkl_eta);
    CleanVec(tkldata.prototkl_phi);
    CleanVec(tkldata.prototkl_deta);
    CleanVec(tkldata.prototkl_dphi);
    CleanVec(tkldata.prototkl_dR);

    CleanVec(tkldata.cluslayer);
    CleanVec(tkldata.clusphi);
    CleanVec(tkldata.cluseta);
    CleanVec(tkldata.clusphisize);
    CleanVec(tkldata.cluszsize);
    CleanVec(tkldata.clusadc);

    CleanVec(tkldata.tklclus1x);
    CleanVec(tkldata.tklclus1y);
    CleanVec(tkldata.tklclus1z);
    CleanVec(tkldata.tklclus2x);
    CleanVec(tkldata.tklclus2y);
    CleanVec(tkldata.tklclus2z);
    CleanVec(tkldata.tklclus1phi);
    CleanVec(tkldata.tklclus1eta);
    CleanVec(tkldata.tklclus2phi);
    CleanVec(tkldata.tklclus2eta);
    CleanVec(tkldata.tklclus1phisize);
    CleanVec(tkldata.tklclus2phisize);
    CleanVec(tkldata.tklclus1adc);
    CleanVec(tkldata.tklclus2adc);

    CleanVec(tkldata.recotklraw_eta);
    CleanVec(tkldata.recotklraw_phi);
    CleanVec(tkldata.recotklraw_deta);
    CleanVec(tkldata.recotklraw_dphi);
    CleanVec(tkldata.recotklraw_dR);
    CleanVec(tkldata.recotklraw_dca3dvtx);

    CleanVec(tkldata.recotkl_isMatchedGChadron);
    CleanVec(tkldata.recotkl_clus1_matchedtrackID);
    CleanVec(tkldata.recotkl_clus2_matchedtrackID);
    CleanVec(tkldata.recotkl_clus1_matchedAncestorTrackID);
    CleanVec(tkldata.recotkl_clus2_matchedAncestorTrackID);
    CleanVec(tkldata.matchedGChadron_trackID);
    CleanVec(tkldata.matchedGChadron_pt);
    CleanVec(tkldata.matchedGChadron_eta);

    CleanVec(tkldata.PrimaryG4P_trackID);
    CleanVec(tkldata.PrimaryG4P_Pt);
    CleanVec(tkldata.PrimaryG4P_eta);
    CleanVec(tkldata.PrimaryG4P_phi);
    CleanVec(tkldata.PrimaryG4P_IsRecotkl);

    for (auto &genhad : tkldata.GenHadrons)
    {
        delete genhad;
    }
    CleanVec(tkldata.GenHadrons);
    CleanVec(tkldata.GenHadron_PID);
    CleanVec(tkldata.GenHadron_trackID);
    CleanVec(tkldata.GenHadron_Pt);
    CleanVec(tkldata.GenHadron_eta);
    CleanVec(tkldata.GenHadron_phi);
    CleanVec(tkldata.GenHadron_IsRecotkl);
}

bool compare_dR(Tracklet *a, Tracklet *b) { return a->dR() < b->dR(); }

void ProtoTracklets(TrackletData &tkldata, float cutdr)
{
    float Cut_dR = cutdr;

    for (auto ihitl1 : tkldata.layers[0])
    {
        for (auto ihitl2 : tkldata.layers[1])
        {
            // float dEta = ihitl2->Eta() - ihitl1->Eta();
            // float dPhi = deltaPhi(ihitl2->Phi(), ihitl1->Phi());
            // float dR = sqrt(dEta * dEta + dPhi * dPhi);

            Tracklet *tmptkl = new Tracklet(ihitl1, ihitl2);

            if (tmptkl->dR() < Cut_dR)
            {
                tkldata.ProtoTkls.push_back(tmptkl);

                // tkldata.prototkl_eta.push_back(tmptkl->Hit1()->Eta());
                // tkldata.prototkl_phi.push_back(tmptkl->Hit1()->Phi());
                // tkldata.prototkl_deta.push_back(tmptkl->dEta());
                // tkldata.prototkl_dphi.push_back(tmptkl->dPhi());
                // tkldata.prototkl_dR.push_back(tmptkl->dR());
            }
            else
            {
                delete tmptkl;
            }
        }
    }

    tkldata.NPrototkl = tkldata.ProtoTkls.size();
}

// 3D DCA calculation
struct Point3D
{
    double x, y, z;
};

float computeDCA(const Point3D &p1, const Point3D &p2, const Point3D &p3) // p1 and p2 are the coordinates of clusters on tracklets, p3 is the event vertex
{
    // Direction vector of the line
    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;
    float dz = p2.z - p1.z;

    // Vector from p1 to p3
    float vx = p3.x - p1.x;
    float vy = p3.y - p1.y;
    float vz = p3.z - p1.z;

    float denominator = dx * dx + dy * dy + dz * dz;
    float numerator = vx * dx + vy * dy + vz * dz;

    float t = numerator / denominator; // Projection parameter

    // Compute the closest point on the line
    float x_c = p1.x + t * dx;
    float y_c = p1.y + t * dy;
    float z_c = p1.z + t * dz;

    // Compute the Euclidean distance between p3 and the closest point
    float dca = std::sqrt((p3.x - x_c) * (p3.x - x_c) + (p3.y - y_c) * (p3.y - y_c) + (p3.z - z_c) * (p3.z - z_c));

    return dca;
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
            if (!tkldata.isdata)
                tkldata.RecoTkls.push_back(tkl);
            tkl->Hit1()->SetMatchedTkl();
            tkl->Hit2()->SetMatchedTkl();

            tkldata.recotklraw_eta.push_back((tkl->Hit1()->Eta()));
            tkldata.recotklraw_phi.push_back(tkl->Hit1()->Phi());
            tkldata.recotklraw_deta.push_back(tkl->dEta());
            tkldata.recotklraw_dphi.push_back(tkl->dPhi());
            tkldata.recotklraw_dR.push_back(tkl->dR());

            tkldata.tklclus1x.push_back(tkl->Hit1()->posX());
            tkldata.tklclus1y.push_back(tkl->Hit1()->posY());
            tkldata.tklclus1z.push_back(tkl->Hit1()->posZ());
            tkldata.tklclus2x.push_back(tkl->Hit2()->posX());
            tkldata.tklclus2y.push_back(tkl->Hit2()->posY());
            tkldata.tklclus2z.push_back(tkl->Hit2()->posZ());
            tkldata.tklclus1phi.push_back(tkl->Hit1()->Phi());
            tkldata.tklclus1eta.push_back(tkl->Hit1()->Eta());
            tkldata.tklclus2phi.push_back(tkl->Hit2()->Phi());
            tkldata.tklclus2eta.push_back(tkl->Hit2()->Eta());
            tkldata.tklclus1phisize.push_back(tkl->Hit1()->PhiSize());
            tkldata.tklclus2phisize.push_back(tkl->Hit2()->PhiSize());
            tkldata.tklclus1adc.push_back(tkl->Hit1()->ClusADC());
            tkldata.tklclus2adc.push_back(tkl->Hit2()->ClusADC());

            // calculate DCA w.r.t the event vertex PV_x, PV_y, PV_z
            // set up Point3D objects for the 3D DCA calculation
            Point3D p1 = {tkl->Hit1()->posX(), tkl->Hit1()->posY(), tkl->Hit1()->posZ()};
            Point3D p2 = {tkl->Hit2()->posX(), tkl->Hit2()->posY(), tkl->Hit2()->posZ()};
            Point3D p3 = {tkldata.PV_x, tkldata.PV_y, tkldata.PV_z};
            tkldata.recotklraw_dca3dvtx.push_back(computeDCA(p1, p2, p3));
        }
    }

    tkldata.NRecotkl_Raw = tkldata.recotklraw_eta.size();
}

// function to print the size of all the vectors in the TrackletData object
void PrintVecSize(TrackletData &tkldata)
{
    cout << "Size of the vectors in the TrackletData object:" << endl;
    cout << "layers[0]: " << tkldata.layers[0].size() << endl;
    cout << "layers[1]: " << tkldata.layers[1].size() << endl;
    cout << "ProtoTkls: " << tkldata.ProtoTkls.size() << endl;
    cout << "RecoTkls: " << tkldata.RecoTkls.size() << endl;
    cout << "GenHadrons: " << tkldata.GenHadrons.size() << endl;
    cout << "cluslayer: " << tkldata.cluslayer.size() << endl;
    cout << "clusphi: " << tkldata.clusphi.size() << endl;
    cout << "cluseta: " << tkldata.cluseta.size() << endl;
    cout << "clusphisize: " << tkldata.clusphisize.size() << endl;
    cout << "cluszsize: " << tkldata.cluszsize.size() << endl;
    cout << "clusadc: " << tkldata.clusadc.size() << endl;
    cout << "tklclus1phi: " << tkldata.tklclus1phi.size() << endl;
    cout << "tklclus1eta: " << tkldata.tklclus1eta.size() << endl;
    cout << "tklclus2phi: " << tkldata.tklclus2phi.size() << endl;
    cout << "tklclus2eta: " << tkldata.tklclus2eta.size() << endl;
    cout << "tklclus1phisize: " << tkldata.tklclus1phisize.size() << endl;
    cout << "tklclus2phisize: " << tkldata.tklclus2phisize.size() << endl;
    cout << "tklclus1adc: " << tkldata.tklclus1adc.size() << endl;
    cout << "tklclus2adc: " << tkldata.tklclus2adc.size() << endl;
    cout << "prototkl_eta: " << tkldata.prototkl_eta.size() << endl;
    cout << "prototkl_phi: " << tkldata.prototkl_phi.size() << endl;
    cout << "prototkl_deta: " << tkldata.prototkl_deta.size() << endl;
    cout << "prototkl_dphi: " << tkldata.prototkl_dphi.size() << endl;
    cout << "prototkl_dR: " << tkldata.prototkl_dR.size() << endl;
    cout << "recotklraw_eta: " << tkldata.recotklraw_eta.size() << endl;
    cout << "recotklraw_phi: " << tkldata.recotklraw_phi.size() << endl;
    cout << "recotklraw_deta: " << tkldata.recotklraw_deta.size() << endl;
    cout << "recotklraw_dphi: " << tkldata.recotklraw_dphi.size() << endl;
    cout << "recotklraw_dR: " << tkldata.recotklraw_dR.size() << endl;
    cout << "GenHadron_PID: " << tkldata.GenHadron_PID.size() << endl;
    cout << "GenHadron_Pt: " << tkldata.GenHadron_Pt.size() << endl;
    cout << "GenHadron_eta: " << tkldata.GenHadron_eta.size() << endl;
    cout << "GenHadron_phi: " << tkldata.GenHadron_phi.size() << endl;
}

// check if the two clusters in a tracklet are matched to the same G4 particle (primary charged particle, GenHadron_trackID)
// if they are, set the matchedGChadron values in the Tracklet object
void RecoTkl_isG4P(TrackletData &tkldata)
{
    // std::set for the matched G4P track ID
    std::set<int> G4PTrackID_tklmatched;
    std::set<int> G4PAncestorTrackID_tklmatched;
    for (auto &tkl : tkldata.RecoTkls)
    {
        tkldata.recotkl_clus1_matchedtrackID.push_back(tkl->Hit1()->MatchedG4P_trackID());
        tkldata.recotkl_clus2_matchedtrackID.push_back(tkl->Hit2()->MatchedG4P_trackID());
        tkldata.recotkl_clus1_matchedAncestorTrackID.push_back(tkl->Hit1()->MatchedG4P_ancestor_trackID());
        tkldata.recotkl_clus2_matchedAncestorTrackID.push_back(tkl->Hit2()->MatchedG4P_ancestor_trackID());
        // std::cout << "Tracklet (delta phi, delta eta, delta R) = (" << tkl->dPhi() << ", " << tkl->dEta() << ", " << tkl->dR() << ")" << std::endl;
        // std::cout << "Tracklet cluster 1: matched G4P track ID = " << tkl->Hit1()->MatchedG4P_trackID() << " ; cluster 2: matched G4P track ID = " << tkl->Hit2()->MatchedG4P_trackID() << std::endl;
        // std::cout << "Tracklet cluster 1: matched G4P ancestor track ID = " << tkl->Hit1()->MatchedG4P_ancestor_trackID() << " ; cluster 2: matched G4P ancestor track ID = " << tkl->Hit2()->MatchedG4P_ancestor_trackID() << std::endl //
                //   << "--------------------------------" << std::endl;                                                                                                                                                                    //
        if ((tkl->Hit1()->MatchedG4P_trackID() == tkl->Hit2()->MatchedG4P_trackID()) && (tkl->Hit1()->MatchedG4P_trackID() != std::numeric_limits<int>::max()))
        {
            // now we have to see if the matched G4P track ID is in the GenHadron_trackID vector
            auto it = std::find(tkldata.GenHadron_trackID.begin(), tkldata.GenHadron_trackID.end(), tkl->Hit1()->MatchedG4P_trackID());
            if (it != tkldata.GenHadron_trackID.end())
            {
                tkl->SetMatchedGenHardon();
                tkldata.recotkl_isMatchedGChadron.push_back(true);
                tkldata.matchedGChadron_pt.push_back(tkl->Hit1()->MatchedG4P_Pt());
                tkldata.matchedGChadron_eta.push_back(tkl->Hit1()->MatchedG4P_Eta());
                tkldata.matchedGChadron_phi.push_back(tkl->Hit1()->MatchedG4P_Phi());

                // add the matched G4P track ID to the set
                G4PTrackID_tklmatched.insert(tkl->Hit1()->MatchedG4P_trackID());
                G4PAncestorTrackID_tklmatched.insert(tkl->Hit1()->MatchedG4P_ancestor_trackID());
            }
            else // it's not generated charged hadron
            {
                tkldata.recotkl_isMatchedGChadron.push_back(false);
                // if the matched G4P track ID is not in the GenHadron_trackID vector, set the matchedGChadron values to -1
                tkldata.matchedGChadron_pt.push_back(-1);
                tkldata.matchedGChadron_eta.push_back(-1);
                tkldata.matchedGChadron_phi.push_back(-1);

                G4PAncestorTrackID_tklmatched.insert(tkl->Hit1()->MatchedG4P_ancestor_trackID());
            }
        }
        else
        {
            tkldata.recotkl_isMatchedGChadron.push_back(false);
            // if the two clusters in a tracklet are not matched to the same G4 particle, set the matchedGChadron values to -1
            tkldata.matchedGChadron_pt.push_back(-1);
            tkldata.matchedGChadron_eta.push_back(-1);
            tkldata.matchedGChadron_phi.push_back(-1);
        }
    }

    // Check for each element in the set if it is in the PrimaryG4P_trackID vector. If it is, set the corresponding element in PrimaryG4P_IsRecotkl to true
    std::cout << "G4PTrackID_tklmatched size: " << G4PTrackID_tklmatched.size() << std::endl;
    for (auto &trackID : G4PTrackID_tklmatched)
    {
        auto it = std::find(tkldata.PrimaryG4P_trackID.begin(), tkldata.PrimaryG4P_trackID.end(), trackID);
        if (it != tkldata.PrimaryG4P_trackID.end())
        {
            int index = std::distance(tkldata.PrimaryG4P_trackID.begin(), it);
            tkldata.PrimaryG4P_IsRecotkl[index] = true;
        }
    }

    // Check for each element in the set if it is in the GenHadron_trackID vector. If it is, set the corresponding element in GenHadron_IsRecotkl to true
    std::cout << "G4PAncestorTrackID_tklmatched size: " << G4PAncestorTrackID_tklmatched.size() << std::endl;
    for (auto &trackID : G4PAncestorTrackID_tklmatched)
    {
        auto it = std::find(tkldata.GenHadron_trackID.begin(), tkldata.GenHadron_trackID.end(), trackID);
        if (it != tkldata.GenHadron_trackID.end())
        {
            int index = std::distance(tkldata.GenHadron_trackID.begin(), it);
            tkldata.GenHadron_IsRecotkl[index] = true;
        }
    }

    // clear the set
    G4PTrackID_tklmatched.clear();
    G4PAncestorTrackID_tklmatched.clear();
}

#endif
