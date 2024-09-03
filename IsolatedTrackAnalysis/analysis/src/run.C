#include "IsotrackTreesAnalysis.h"
#include <boost/program_options.hpp>

int main(int argc, char **argv){

    std::string filename;
    std::string output_filename = "test_output.root";
    
    // options
    int useTowerInfo = true;
    int useTruthInfo = true;
    int useCentrality = true;
    int useParticleGun = false;

    // event cuts
    float centrality = 20.0; // %

    // track cuts
    float d0 = 0.15; // m
    float z0 = 0.15; // m
    float pT = 0.5; // GeV

    // isolation criteria
    float matchedPt = 0.2; // GeV
    float matchedDr = 0.4;

    // isolation criteria using neutral truth particles
    float matchedNeutralTruthPt = 0.2; // GeV
    float matchedNeutralTruthEta = 1.0;
    float matchedNeutralTruthDr = 0.4;

    // track calo matching criteria
    float cemcMatchingDr = 0.2;
    float ihcalMatchingDr = 0.2;
    float ohcalMatchingDr = 0.2;

    // MIP cuts
    float cemcMipEnergy = 0.35; // GeV
    float ihcalMipEnergy = 0.1; // GeV
    
    namespace po = boost::program_options;

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("file", po::value<std::string>(&filename), "path to txt file containing list of ROOT files (required)")
        ("out_file", po::value<std::string>(&output_filename), "filename for output of analysis, default = test_output.root")
        ("use_towers", po::value<int>(&useTowerInfo), "use towers instead of clusters, default = 1")
        ("use_truth", po::value<int>(&useTruthInfo), "use truth information, default = 1")
        ("use_centrality", po::value<int>(&useCentrality), "use centrality information, default = 1")
        ("use_particle_gun", po::value<int>(&useParticleGun),"analyze a particle gun run, default = 0")
        ("centrality", po::value<float>(&centrality), "centrality cut, default = 20%")
        ("d0", po::value<float>(&d0), "d0 cut, default = 0.15 m")
        ("z0", po::value<float>(&z0), "z0 cut, default = 0.15 m")
        ("pt", po::value<float>(&pT), "pT cut, default = 0.5 GeV")
        ("matched_pt", po::value<float>(&matchedPt), "pT cut on matched tracks, default = 0.2 GeV")
        ("matched_dr", po::value<float>(&matchedDr), "dR cut for matched tracks, default = 0.4")
        ("neutral_pt", po::value<float>(&matchedNeutralTruthPt), "pT cut on matched neutral truth particles, default = 0.2 GeV")
        ("neutral_eta", po::value<float>(&matchedNeutralTruthEta), "eta cut on matched neutral truth particles, default = abs(1.0)")
        ("neutral_dr", po::value<float>(&matchedNeutralTruthDr), "dR cut for matched neutral truth particles, default = 0.4")
        ("cemc_dr", po::value<float>(&cemcMatchingDr), "dR cut for track-calo matching in CEMC, default = 0.2")      
        ("ihcal_dr", po::value<float>(&ihcalMatchingDr), "dR cut for track-calo matching in IHCal, default = 0.2")      
        ("ohcal_dr", po::value<float>(&ohcalMatchingDr), "dR cut for track-calo matching in OHCal, default = 0.2")      
        ("cemc_mip_energy", po::value<float>(&cemcMipEnergy), "MIP energy cut for CEMC, default = 0.35 GeV")
        ("ihcal_mip_energy", po::value<float>(&ihcalMipEnergy), "MIP energy cut for IHCal, default = 0.1 GeV")    
        ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
  
    if(vm.count("help") || argc == 1){
        std::cout << desc << std::endl;
        return 1;
    }

    if(vm.count("file") == 0){
      std::cout << "ERROR: Filename is required!" << std::endl;
      std::cout << "" << std::endl;
      std::cout << desc << std::endl;
      return 2;
    }
    
    IsotrackTreesAnalysis a(filename, output_filename, useTowerInfo, useTruthInfo, useCentrality, useParticleGun, centrality, d0, z0, pT, matchedPt, matchedDr, matchedNeutralTruthPt, matchedNeutralTruthEta, matchedNeutralTruthDr, cemcMatchingDr, ihcalMatchingDr, ohcalMatchingDr, cemcMipEnergy, ihcalMipEnergy);

    a.Loop();

    return 0;
}
