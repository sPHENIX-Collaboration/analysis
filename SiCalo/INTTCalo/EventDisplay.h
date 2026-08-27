#ifndef EVENTDISPLAY_H
#define EVENTDISPLAY_H

#include <vector>
#include <TCanvas.h>
#include <TH1F.h>
#include <TArc.h>
#include <TArrow.h>
#include <TBox.h>

void EventSelection(int& ievent);
void EventDisplay(int iframe);
//bool Is_Hot_EMC(int);

// display frame;
static int iframe=-1; //0: All view 1: Si View
static TH1F *frame1 = nullptr;
static TH1F *frame2 = nullptr;
static TH1F *frame3 = nullptr;
static TH1F *frame4 = nullptr;
static TCanvas *c1=nullptr;
static TCanvas *c2=nullptr;
static TCanvas *c3=nullptr;
static TCanvas *c4=nullptr;

static TArc *Circle2 = nullptr;
static TArc *Circle4 = nullptr;
static TArc *Circle8 = nullptr;
static TArc *Circle10 = nullptr;

// buffer for hits to display
const int nMax=5000;
float x[nMax], y[nMax];
TBox *hits[nMax];
TBox *hitsRZ[nMax];
static int nhits = 0;
static int nhitsRZ = 0;

// ZVTX maker
TArrow *ZvtxMarker = nullptr;

// buffer for tracks to display
const int nMaxTrk=300;
TArrow *seedtrk[nMaxTrk];
TArrow *seedtrkRZ[nMaxTrk];

static int ntrks = 0;
static int ntrksRZ = 0;

// buffer for EMC-INTT tracklet orbit
vector<TArc*> vEmcINTTorbit;   //Circular Orbit in R-phi
vector<TArrow*> vEmcINTT_RZ;   //RZ view of the track

// constants
const float pt0min=0.3; // GeV/c. minimum pT0 for tracks
const float EMScale=0.14;  //EMC hit scaling factor

// RZ slice angle
static float phiSlice = 3.141592/2;
const float DphiSlice = 0.5;
static float Rmax = 12.0;
static TArc *dphiArc = nullptr;

#endif
