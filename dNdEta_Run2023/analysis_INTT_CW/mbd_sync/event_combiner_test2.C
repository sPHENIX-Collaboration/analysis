#include <TFile.h>
#include <TTree.h>

void RemoveFirstTwoEvents(const char* inputFile_INTT, const char* treeName_INTT, const char* treeName_INTT, const char* inputFile_MBD, const char* treeName_MBD, const char* outputFile, const char* treeName_out = "EventTree") {
    // Open the input ROOT file
    TFile* inFile_INTT = TFile::Open(inputFile_INTT, "READ");
    if (!inFile_INTT || inFile_INTT->IsZombie()) {
        std::cerr << "Error: inFile_INTT, could not open input file." << std::endl;
        return;
    }

    // Get the input TTree
    TTree* inTree_INTT = dynamic_cast<TTree*>(inFile_INTT->Get(treeName_INTT));
    if (!inTree_INTT) {
        std::cerr << "Error: inTree_INTT, could not retrieve input TTree." << std::endl;
        inFile_INTT->Close();
        return;
    }

    TFile* inFile_MBD = TFile::Open(inputFile_MBD, "READ");
    if (!inFile_MBD || inFile_MBD->IsZombie()) {
        std::cerr << "Error: inFile_MBD, could not open input file." << std::endl;
        return;
    }

    // Get the input TTree
    TTree* inTree_MBD = dynamic_cast<TTree*>(inFile_MBD->Get(treeName_MBD));
    if (!inTree_MBD) {
        std::cerr << "Error: inTree_MBD, could not retrieve input TTree." << std::endl;
        inFile_MBD->Close();
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

int event_combiner_test2() {
    const char* inputFile_INTT = "/sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/20869/folder_INTTMBD_EventCombiner/intt_run20869.root";
    const char* inputFile_MBD  = "/sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/20869/folder_INTTMBD_EventCombiner/centrality_run20869.root";
    const char* outputFile      = "/sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/20869/folder_INTTMBD_EventCombiner/Final_combine_test.root";

    RemoveFirstTwoEvents(inputFile, outputFile);

    return 0;
}
