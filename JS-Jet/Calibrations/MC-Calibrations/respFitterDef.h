const int verbosity = 0;

const float etaStart = -0.7;
const float etaEnd = 0.7;
const float etaGap = 0.35;

const int extraBins = 2; //two inclusive eta bins: 1) Has the differential eta-jes calibration, one has the integrated eta-jes calibration

const Float_t pt_range[]  = {10,15,20,25,30,35,40,45,50,60,80};
//const Float_t pt_range[]  = {10,12.5,15,17.5,20,22.5,25,27.5,30,32.5,35,37.5,40,42.5,45,47.5,50,55,60,65,70,80};
const int pt_N = sizeof(pt_range)/sizeof(float)-1;

const int resp_N = 50;
const int pt_N_reco = 60;

const int pt_N_truth = 75;

const int eta_N = 40;
const int phi_N = 40;
const int subet_N = 400;
const float nSigmaLin = 1;
const float nSigmaRat = 0.25;
//float trigEntries[4] = {71076872, 28939622, 0, 28524294};

