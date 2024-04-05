#include <TFile.h>
#include <TTree.h>

void RemoveFirstTwoEvents(const char* inputFile, const char* outputFile, const char* treeName = "EventTree") {
    // Open the input ROOT file
    TFile* inFile = TFile::Open(inputFile, "READ");
    if (!inFile || inFile->IsZombie()) {
        std::cerr << "Error: Could not open input file." << std::endl;
        return;
    }

    // Get the input TTree
    TTree* inTree = dynamic_cast<TTree*>(inFile->Get(treeName));
    if (!inTree) {
        std::cerr << "Error: Could not retrieve input TTree." << std::endl;
        inFile->Close();
        return;
    }

    // Create a new output ROOT file
    TFile* outFile = TFile::Open(outputFile, "RECREATE");
    if (!outFile || outFile->IsZombie()) {
        std::cerr << "Error: Could not create output file." << std::endl;
        inFile->Close();
        return;
    }

    // Clone the input TTree to create a new TTree in the output file
    TTree* outTree = inTree->CloneTree(0); // Create new tree with default buffer size

    // Get the total number of events in the input tree
    Long64_t totalEvents = inTree->GetEntries();

    // Loop over the events in the input tree, skipping the first two events
    for (Long64_t i = 2; i < totalEvents; ++i) {
        inTree->GetEntry(i);
        outTree->Fill();
    }

    // Write the modified tree to the output file
    outTree->Write();

    // Close the input and output files
    inFile->Close();
    outFile->Close();

    // Clean up
    delete inFile;
    delete outFile;
}

int event_combiner_test() {
    const char*  inputFile = "/sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/20869/folder_INTTMBD_EventCombiner/centrality_run20869.root";
    const char* outputFile = "/sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/20869/folder_INTTMBD_EventCombiner/centrality_run20869_out.root";

    RemoveFirstTwoEvents(inputFile, outputFile);

    return 0;
}
