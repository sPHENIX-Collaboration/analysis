#ifndef GENHADRON_H
#define GENHADRON_H

#include <stdio.h>
#include <iostream>
#include <cmath>

#include <TObject.h>
#include <TVector3.h>
#include <TRandom3.h>

using namespace std;

class GenHadron : public TObject
{
public:
    GenHadron();
    GenHadron(float, float, float, float); // simple construct
    ~GenHadron();

    float Pt();
    float Eta();
    float Phi();
    float E();

    void SetMatchedToRecotkl();
    bool IsMatchedToRecotkl();

private:
    float _pt;
    float _eta;
    float _phi;
    float _en;
    bool _ismatched;
};

GenHadron::GenHadron()
{
    _pt = 0;
    _eta = 0;
    _phi = 0;
    _en = 0;
    _ismatched = false;
}

GenHadron::GenHadron(float pt, float eta, float phi, float E)
{
    _pt = pt;
    _eta = eta;
    _phi = phi;
    _en = E;
    _ismatched = false;
}

GenHadron::~GenHadron() {}

float GenHadron::Pt() { return (_pt); }

float GenHadron::Eta() { return (_eta); } 

float GenHadron::Phi() { return (_phi); }

float GenHadron::E() { return (_en); }

void GenHadron::SetMatchedToRecotkl() { _ismatched = true; }

bool GenHadron::IsMatchedToRecotkl() { return _ismatched; }

#endif