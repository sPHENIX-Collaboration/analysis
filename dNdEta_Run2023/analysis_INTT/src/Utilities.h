#ifndef UTILITIES_H
#define UTILITIES_H

#include <vector>

#include <TFile.h>
#include <TMath.h>
#include <TTree.h>

using namespace std;

#define PI 3.14159265358979323846

template <class T> void CleanVec(std::vector<T> &v)
{
    std::vector<T>().swap(v);
    v.shrink_to_fit();
}

float deltaPhi(float phi1, float phi2)
{
    float dPhi = phi1 - phi2;
    if (dPhi > PI)
        dPhi -= 2. * PI;
    if (dPhi < -1. * PI)
        dPhi += 2. * PI;
    return dPhi;
}

float deltaR(float eta1, float phi1, float eta2, float phi2)
{
    float dEta, dPhi;
    dEta = eta1 - eta2;
    dPhi = deltaPhi(phi1, phi2);
    return sqrt(dEta * dEta + dPhi * dPhi);
}

template <typename T> std::string number_to_string(T param_)
{
    std::string str;
    if (param_ < 0)
    {
        str += "M";
        param_ = std::abs(param_);
    }
    str += std::to_string(param_);
    std::size_t found = str.find('.');
    if (found == std::string::npos)
        return str;
    str.replace(found, 1, "p");
    while (*(str.end() - 1) == '0' && *(str.end() - 2) != 'p' && !str.empty())
        str.erase(str.end() - 1);
    if (*(str.end() - 1) == '0' && *(str.end() - 2) == 'p')
        str.erase(str.size() - 2, 2);
    return str;
}

// MVTX stave position
std::vector<tuple<float, float, float, float>> MVTXStavePositionXY()
{
    std::vector<tuple<float, float, float, float>> v; // x1, y1, x2, y2
    TFile *f = new TFile("./MVTX_geo.root", "READ");
    f->cd();
    TTree *t = (TTree *)f->Get("tree");
    float x1, y1, x2, y2;
    t->SetBranchAddress("x1", &x1);
    t->SetBranchAddress("y1", &y1);
    t->SetBranchAddress("x2", &x2);
    t->SetBranchAddress("y2", &y2);

    for (int i = 0; i < t->GetEntries(); i++)
    {
        t->GetEntry(i);

        v.push_back(make_tuple(x1, y1, x2, y2));
    }

    return v;
}

#endif