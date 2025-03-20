const int n_hcal_etabin = 24;
const int n_hcal_phibin = 64;

const float PI = TMath::Pi();
const float jet_radius = 0.4;

double calibptbins[] = {15, 19, 23.5, 28.5, 34, 40, 47, 54.5, 63, 72};
double truthptbins[] = {7, 11, 15, 19, 23.5, 28.5, 34, 40, 47, 54.5, 63, 72, 82};

int calibnpt = sizeof(calibptbins) / sizeof(calibptbins[0]) - 1;
int truthnpt = sizeof(truthptbins) / sizeof(truthptbins[0]) - 1;

double n_MB_events = 21.885464;
double n_Jet10GeV_events = 2.664378;
double n_Jet30GeV_events = 2.194879;

double MB_cross_section = 4.197e-2;
double Jet10GeV_cross_section = 3.646e-6;
double Jet30GeV_cross_section = 2.505e-9;

double MB_scale = MB_cross_section/(double)n_MB_events;
double Jet10GeV_scale = Jet10GeV_cross_section/(double)n_Jet10GeV_events;
double Jet30GeV_scale = Jet30GeV_cross_section/(double)n_Jet30GeV_events;