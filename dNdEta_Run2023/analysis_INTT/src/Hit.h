#ifndef HIT_H
#define HIT_H

#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdio.h>

#include <TF1.h>
#include <TH1F.h>
#include <TObject.h>
#include <TRandom3.h>
#include <TVector3.h>

using namespace std;

class Hit : public TObject
{
  public:
    Hit();
    Hit(float, float, float, float, float, float, int);
    Hit(float, float, float, float, float, float, int, float);
    Hit(float, float, float, float, float, float, int, float, unsigned int);
    Hit(float, float); // simple construct
    ~Hit();

    float posX();
    float posY();
    float posZ();
    float rho();
    float vtxX();
    float vtxY();
    float vtxZ();
    float Eta();
    float Phi();
    float R();
    int Layer();
    float PhiSize() { return _phisize; }
    unsigned int ClusADC() { return _clusadc; }
    pair<float, float> Edge();
    int MatchedG4P_trackID() { return matchedG4P_trackID; };
    int MatchedG4P_ancestor_trackID() { return matchedG4P_ancestor_trackID; };
    float MatchedG4P_Pt() { return matchedG4P_Pt; };
    float MatchedG4P_Eta() { return matchedG4P_Eta; };
    float MatchedG4P_Phi() { return matchedG4P_Phi; };

    void Update();
    void SetPos(float, float, float);
    void SetVtx(float, float, float);
    void SetEdge(float, float);
    void SetMatchedG4P(int, int, int, int, int);
    void SetMatchedTkl();
    bool IsMatchedTkl();
    void Print();

    TVector3 VecPos();
    TVector3 VecVtx();
    TVector3 VecRel();

  private:
    float _x;
    float _y;
    float _z;
    float _vtxX;
    float _vtxY;
    float _vtxZ;
    float _eta;
    float _phi;
    float _R;
    int _layer;
    float _phisize;
    unsigned int _clusadc;
    pair<float, float> _edge;
    bool _matched_tkl;
    TVector3 vechit;
    TVector3 vecvtx;
    TVector3 vecrel;
    int matchedG4P_trackID; // only for simulation (matching)
    int matchedG4P_ancestor_trackID;
    int matchedG4P_Pt;
    int matchedG4P_Eta;
    int matchedG4P_Phi;
};

Hit::Hit()
{
    vechit.SetXYZ(0, 0, 0);
    vecvtx.SetXYZ(0, 0, 0);
    _layer = -999;
    vecrel = vechit - vecvtx;
    _eta = vecrel.Eta();
    _phi = vecrel.Phi();
    _R = vecrel.Mag();
    _matched_tkl = false;
}

Hit::Hit(float x, float y, float z, float vtxX, float vtxY, float vtxZ, int layer)
{
    vechit.SetXYZ(x, y, z);
    vecvtx.SetXYZ(vtxX, vtxY, vtxZ);
    _layer = layer;
    vecrel = vechit - vecvtx;
    _eta = vecrel.Eta();
    _phi = vecrel.Phi();
    _R = vecrel.Mag();
    _matched_tkl = false;
}

Hit::Hit(float x, float y, float z, float vtxX, float vtxY, float vtxZ, int layer, float phisize)
{
    vechit.SetXYZ(x, y, z);
    vecvtx.SetXYZ(vtxX, vtxY, vtxZ);
    _layer = layer;
    _phisize = phisize;
    vecrel = vechit - vecvtx;
    _eta = vecrel.Eta();
    _phi = vecrel.Phi();
    _R = vecrel.Mag();
    _matched_tkl = false;
}

Hit::Hit(float x, float y, float z, float vtxX, float vtxY, float vtxZ, int layer, float phisize, unsigned int clusadc)
{
    vechit.SetXYZ(x, y, z);
    vecvtx.SetXYZ(vtxX, vtxY, vtxZ);
    _layer = layer;
    _phisize = phisize;
    _clusadc = clusadc;
    vecrel = vechit - vecvtx;
    _eta = vecrel.Eta();
    _phi = vecrel.Phi();
    _R = vecrel.Mag();
    _matched_tkl = false;
}

Hit::Hit(float eta, float phi)
{
    _eta = eta;
    _phi = phi;
    _matched_tkl = false;
}

Hit::~Hit() {}

float Hit::posX() { return (vechit.X()); }

float Hit::posY() { return (vechit.Y()); }

float Hit::posZ() { return (vechit.Z()); }

float Hit::rho() { return (sqrt(vechit.X() * vechit.X() + vechit.Y() * vechit.Y())); }

float Hit::vtxX() { return (vecvtx.X()); }

float Hit::vtxY() { return (vecvtx.Y()); }

float Hit::vtxZ() { return (vecvtx.Z()); }

float Hit::Eta() { return (_eta); } // with respect to the vertex that it associates to

float Hit::Phi() { return (_phi); } // with respect to the vertex that it associates to

float Hit::R() { return (_R); } // with respect to the vertex that it associates to

int Hit::Layer() { return (_layer); }

pair<float, float> Hit::Edge() { return (_edge); }

void Hit::SetPos(float x, float y, float z) { vechit.SetXYZ(x, y, z); }

void Hit::SetVtx(float vtxX, float vtxY, float vtxZ) { vecvtx.SetXYZ(vtxX, vtxY, vtxZ); }

void Hit::SetEdge(float edge1, float edge2) { _edge = make_pair(edge1, edge2); }

void Hit::Update()
{
    vecrel = vechit - vecvtx;
    _eta = vecrel.Eta();
    _phi = vecrel.Phi();
    _R = vecrel.Mag();
}

void Hit::SetMatchedTkl() { _matched_tkl = true; }

bool Hit::IsMatchedTkl() { return _matched_tkl; }

TVector3 Hit::VecPos() { return (vechit); }

TVector3 Hit::VecVtx() { return (vecvtx); }

TVector3 Hit::VecRel() { return (vecrel); }

void Hit::SetMatchedG4P(int trackID, int ancestor_trackID, int Pt, int Eta, int Phi)
{
    matchedG4P_trackID = trackID;
    matchedG4P_ancestor_trackID = ancestor_trackID;
    matchedG4P_Pt = Pt;
    matchedG4P_Eta = Eta;
    matchedG4P_Phi = Phi;
}

void Hit::Print()
{
    printf("[Hit::Print()] (posX, posY, posZ) = (%f, %f, %f), (vtxX, vtxY, vtxZ) = (%f, %f, %f), (eta, phi) = (%f, %f) \n", vechit.X(), vechit.Y(), vechit.Z(), vecvtx.X(), vecvtx.Y(), vecvtx.Z(),
           vecrel.Eta(), vecrel.Phi());
}

void UpdateHits(vector<Hit *> &Hits, vector<float> PV)
{
    for (auto &hit : Hits)
    {
        hit->SetVtx(PV[0], PV[1], PV[2]);
        hit->Update();
    }
}

int RandomHit(int set)
{
    return static_cast<int>(35 * set);
}

Hit *RandomHit(float vx, float vy, float vz, int layer)
{
    gRandom->SetSeed(0);
    // The 26 unique z positions for INTT
    vector<float> zpos = {-22.57245, -20.57245, -18.57245, -16.57245, -14.57245, -12.57245, -10.97245, -9.372450, -7.772450, -6.172450, -4.572450, -2.972450, -1.372450,
                          0.4275496, 2.0275495, 3.6275494, 5.2275495, 6.8275494, 8.4275493, 10.027549, 11.627549, 13.627549, 15.627549, 17.627550, 19.627550, 21.627550};
    int zpos_idx = gRandom->Integer(26);

    float layer_radius[4] = {7.453, 8.046, 9.934, 10.569};
    float phiMin = -TMath::Pi();
    float phiMax = TMath::Pi();
    float phi = phiMin + (phiMax - phiMin) * gRandom->Rndm();
    float x = layer_radius[layer] * cos(phi);
    float y = layer_radius[layer] * sin(phi);
    float z = zpos[zpos_idx];                                 // layer_radius[layer] / tan(2 * atan(exp(-eta)));
    Hit *randhit = new Hit(x, y, z, vx, vy, vz, layer, 1, 1); // assign the phisize and clusadc to 1 for random clusters, which should be ok

    return randhit;
}

float theta2pseudorapidity(float theta) { return -1. * TMath::Log(TMath::Tan(theta / 2)); }

TF1 *ClusADCCut(int constscale, float etascale)
{
    TF1 *f = new TF1("f", Form("%d*TMath::CosH(%f*x)", constscale, etascale), -10, 10);
    return f;
}

TH1F *ClusADCCut_StepFunc_INTTPrivate()
{
    std::vector<float> thetastep = {0.001, 15, 20, 25, 30, 35, 45, 55, 125, 135, 145, 150, 155, 160, 165, 179.999};
    std::reverse(thetastep.begin(), thetastep.end());
    std::vector<float> adccut_theta = {225, 165, 135, 120, 105, 90, 75, 60, 75, 90, 105, 120, 135, 165, 225};
    float etastep_array[thetastep.size()];
    for (int i = 0; i < thetastep.size(); i++)
    {
        etastep_array[i] = theta2pseudorapidity(thetastep[i] * TMath::Pi() / 180);
    }

    TH1F *hm_cut_inttprivate = new TH1F("hm_cut_inttprivate", "hm_cut_inttprivate", thetastep.size() - 1, etastep_array);
    for (int j = 0; j < hm_cut_inttprivate->GetNbinsX(); j++)
    {
        hm_cut_inttprivate->SetBinContent(j + 1, adccut_theta[j]);
    }

    return hm_cut_inttprivate;
}

TH1F *ClusADCCut_StepFunc(int constscale, float etascale)
{
    TF1 *f_cut = ClusADCCut(constscale, etascale);

    std::vector<float> adcstep;
    for (int i = 0; i < 20; i++)
    {
        adcstep.push_back(20 + i * 30);
    }
    std::vector<float> etastep;
    for (int i = 0; i < 20; i++)
    {
        etastep.insert(etastep.begin(), f_cut->GetX(adcstep[i], -10, 0));
        etastep.push_back(f_cut->GetX(adcstep[i], 0, 10));
    }

    etastep.erase(std::remove_if(etastep.begin(), etastep.end(), [](float x) { return std::isnan(x); }), etastep.end());

    float etastep_array[etastep.size()];
    for (int i = 0; i < etastep.size(); i++)
    {
        etastep_array[i] = etastep[i];
    }
    TH1F *hm_cut = new TH1F("hm_cut", "hm_cut", etastep.size() - 1, etastep_array);
    for (int j = 0; j < hm_cut->GetNbinsX(); j++)
    {
        if (hm_cut->GetBinLowEdge(j + 1) < 0)
        {
            hm_cut->SetBinContent(j + 1, f_cut->Eval(hm_cut->GetBinLowEdge(j + 1)));
        }
        else
        {
            hm_cut->SetBinContent(j + 1, f_cut->Eval(hm_cut->GetBinCenter(j + 1) + hm_cut->GetBinWidth(j + 1) / 2));
        }
    }

    return hm_cut;
}

int ConstADCCut(int set)
{
    float cut = 0;
    switch (set)
    {
    case 0:
        cut = 35;
        break;
    case 1:
        cut = 0;
        break;
    case 2:
        cut = 50;
        break;
    }
    return cut;
}

int ConstClusPhiCut(int set)
{
    float cut = 0;
    switch (set)
    {
    case 0:
        cut = 40;
        break;
    case 1:
        cut = 1E6;
        break;
    }
    return cut;
}

#endif
