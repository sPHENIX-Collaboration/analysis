#include <iostream>
#include <cstdlib>  // For std::system

void createDirectories(const std::string& basePath) {
    std::string p015Path = basePath + "/p015";
    std::string qaPath = p015Path + "/QA";
    std::string invMassPath = p015Path + "/InvMass";
    std::string rootFilesPath = qaPath + "/RootFiles"; // Path for 'RootFiles' directory
    std::string systematicsPath = p015Path + "/Systematics_Analysis-v2-Checks"; // Path for 'Systematics_Analysis-v2-Checks' directory
    std::string v2FinalResultsPath = p015Path + "/v2_FinalResults"; // Path for 'v2_FinalResults' directory

    // Directories for v2_FinalResults
    std::string v2SubDirs = "'" + v2FinalResultsPath + "/v2_Overlay_withPHENIX'";

    std::string systematicsSubDirs = "'"
        + systematicsPath + "/AsymmetryCutVariations' '"
        + systematicsPath + "/BackgroundWindow_Variations' '"
        + systematicsPath + "/EMCal_systematics' '"
        + systematicsPath + "/SampleSize_Variation' '"
        + systematicsPath + "/SignalWindow_Variations'";

    std::string additionalDirs = "'"
        + p015Path + "/vN-CSV-files' '"
        + systematicsPath + "' '"
        + p015Path + "/v2_FinalResults' '"
        + p015Path + "/v3_FinalResults' '"
        + p015Path + "/ProductionComparisons' '"
        + p015Path + "/QQdistributions'";
    
    std::vector<std::string> folderNumbers = {
        "23020", "23671", "23672", "23676", "23681", "23682", "23687", "23690", "23693", "23694",
        "23695", "23696", "23697", "23699", "23702", "23714", "23726", "23727", "23728", "23735",
        "23737", "23738", "23739", "23740", "23743", "23745", "23746", "23536",  "23539", "23543", 
        "23547", "23550",  "23552", "23557", "23558","23561", "23562", "23563", "23566", "23568",
        "23572",  "23574", "23575", "23582", "23590", "23592", "23594", "23604", "23605", "23617",
        "23618", "23619"
    };

    std::string baseDirs = "mkdir -p '" + qaPath + "/RootFiles' '" + qaPath + "/Diphoton' '"
    + invMassPath + "/CSV' '" + invMassPath + "/Plots' " + additionalDirs + " " + systematicsSubDirs + " " + v2SubDirs;

    std::string mbAndCentralDirs = "'"
        + qaPath + "/MB/1D_Centrality' '"
        + qaPath + "/MB/ClusterEnergyDistributions' '"
        + qaPath + "/MB/EMcal_MBD_correlations' '"
        + qaPath + "/MB/InvidualRunByRunOutput/chi2' '"
        + qaPath + "/MB/InvidualRunByRunOutput/ECore' '"
        + qaPath + "/MB/InvidualRunByRunOutput/MBD' '"
        + qaPath + "/MB/InvidualRunByRunOutput/TotalCaloE' '"
        + qaPath + "/MB/MBD_Centrality' '"
        + qaPath + "/MB/Overlayed_RunByRunOutput' '"
        + qaPath + "/MB/OverlayedWithAverage' '"
        + qaPath + "/MB/RatioPlotOutput' '"
        + qaPath + "/MB/TowerEnergyDistributions' '"
        + qaPath + "/MB/Zvertex' '"
        + qaPath + "/Central/1D_Centrality' '"
        + qaPath + "/Central/ClusterEnergyDistributions' '"
        + qaPath + "/Central/EMcal_MBD_correlations' '"
        + qaPath + "/Central/InvidualRunByRunOutput/chi2' '"
        + qaPath + "/Central/InvidualRunByRunOutput/ECore' '"
        + qaPath + "/Central/InvidualRunByRunOutput/MBD' '"
        + qaPath + "/Central/InvidualRunByRunOutput/TotalCaloE' '"
        + qaPath + "/Central/MBD_Centrality' '"
        + qaPath + "/Central/Overlayed_RunByRunOutput' '"
        + qaPath + "/Central/OverlayedWithAverage' '"
        + qaPath + "/Central/RatioPlotOutput' '"
        + qaPath + "/Central/TowerEnergyDistributions' '"
        + qaPath + "/Central/Zvertex'";

    std::string numberSpecificDirs;
    for (const auto& number : folderNumbers) {
        numberSpecificDirs += "'" + rootFilesPath + "/" + number + "' ";
    }
    
    std::string command = baseDirs + " " + mbAndCentralDirs + " " + numberSpecificDirs;

    std::cout << "Executing command: " << command << std::endl;
    int result = std::system(command.c_str());

    if (result != 0) {
        std::cerr << "Error: Failed to create directories." << std::endl;
    } else {
        std::cout << "Directories created successfully: " << qaPath << std::endl;
        std::cout << "Additional directories created under QA: RootFiles, MB, Central, Diphoton" << std::endl;
        std::cout << "Subdirectories for MB and Central also created." << std::endl;
    }
}
void ConfigureAnalysis() {
    std::string basePath;
    std::cout << "Enter the base path: ";
    std::cin >> basePath;
    createDirectories(basePath);
}
