#ifndef HIT_H
#define HIT_H

#include <cmath>
#include <iostream>
#include <stdio.h>

#include <TObject.h>
#include <TRandom3.h>
#include <TVector3.h>

using namespace std;

class Hit : public TObject
{
  public:
    Hit();
    Hit(float, float, float, float, float, float, int);
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
    pair<float, float> Edge();

    void Update();
    void SetPos(float, float, float);
    void SetVtx(float, float, float);
    void SetEdge(float, float);
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
    pair<float, float> _edge;
    bool _matched_tkl;
    TVector3 vechit;
    TVector3 vecvtx;
    TVector3 vecrel;
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

void Hit::Print() { printf("[Hit::Print()] (posX, posY, posZ) = (%f, %f, %f), (vtxX, vtxY, vtxZ) = (%f, %f, %f), (eta, phi) = (%f, %f) \n", vechit.X(), vechit.Y(), vechit.Z(), vecvtx.X(), vecvtx.Y(), vecvtx.Z(), vecrel.Eta(), vecrel.Phi()); }

void UpdateHits(vector<Hit *> &Hits, vector<float> PV)
{
    for (auto &hit : Hits)
    {
        hit->SetVtx(PV[0], PV[1], PV[2]);
        hit->Update();
    }
}

Hit *RandomHit(float etaMin, float etaMax, float phiMin, float phiMax)
{
    float eta = etaMin + (etaMax - etaMin) * gRandom->Rndm();
    float phi = phiMin + (phiMax - phiMin) * gRandom->Rndm();
    Hit *randhit = new Hit(eta, phi);
    randhit->SetPos(-999., -999., -999.);
    randhit->SetVtx(0., 0., 0.);
    return randhit;
}

// For mis-alignment

#endif