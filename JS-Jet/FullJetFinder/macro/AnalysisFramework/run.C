
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wundefined-internal"
#include <JetAnalysis.h>
#pragma GCC diagnostic pop



R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libjetbase.so)
R__LOAD_LIBRARY(libjetbackground.so)
R__LOAD_LIBRARY(libg4centrality.so)
R__LOAD_LIBRARY(libg4dst.so)
R__LOAD_LIBRARY(libcalo_reco.so)
R__LOAD_LIBRARY(libparticleflow.so)
R__LOAD_LIBRARY(libglobalvertex.so)
R__LOAD_LIBRARY(libg4jets.so)
R__LOAD_LIBRARY(libFullJetFinder.so)
R__LOAD_LIBRARY(libJetAnalysis.so)


void run(){
    JetAnalysis b;
    //b.setWorkDir("/sphenix/tg/tg01/hf/jkvapil/JET30_r11_v2/");
    b.processCondor("/sphenix/tg/tg01/hf/jkvapil/JET30_R22_npile_full/condorJob/myTestJets/outputData_*.root");  
    //b.setWorkDir("/sphenix/tg/tg01/hf/jkvapil/JET10_r11_10GeV/");
    //b.processCondor("/sphenix/tg/tg01/hf/jkvapil/JET10_r11_10GeV/condorJob/myTestJets/outputData_0*.root"); 
    //b.scanPurityEfficiency();
  return;
}
