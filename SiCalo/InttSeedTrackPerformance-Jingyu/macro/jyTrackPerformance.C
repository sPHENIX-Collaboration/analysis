#include "../src/InttSeedTrackPerformance.h"
#include "../src/InttSeedTrackPerformance.cxx"

#include "../src/InttSeedTracking.h"
#include "../src/InttSeedTracking.cxx"
#include "../src/SPHTracKuma.h"

#include <typeinfo>

void jyTrackPerformance(Int_t runNum=0)
{
    // work area
    // std::string fDir = "/mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/ParticleGen/output";
    std::string fDir = "/sphenix/user/jzhang1/INTT-EMCAL/InttSeedingTrackDev/InttSeedTrackPerformance/output";

    // input file
    TChain *tc = new TChain("tree");  
    // std::string fInputName = fDir + "/ana457_e_1_10GeV.root";
    tc->Add(fInputName.c_str()); 

    // output file
    std::string fOutputName = fDir+"/trackingWInttCalClu1000to10000MeVEtaWide_v2"; 

    TTree *tt = (TTree*)tc;
    InttSeedTrackPerformance *h = new InttSeedTrackPerformance(tt, fInputName, fOutputName, runNum);

    h->Loop(runNum);

    return;
}