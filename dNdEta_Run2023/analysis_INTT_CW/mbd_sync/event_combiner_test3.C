#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>

void AddBranchFromFileAToFileB(const char* fileAName, const char* fileBName) {
    // Open fileB.root
    TFile* fileB = TFile::Open(fileBName, "UPDATE");
    if (!fileB || fileB->IsZombie()) {
        std::cerr << "Error: Could not open fileB.root" << std::endl;
        return;
    }

    // Open fileA.root
    TFile* fileA = TFile::Open(fileAName, "READ");
    if (!fileA || fileA->IsZombie()) {
        std::cerr << "Error: Could not open fileA.root" << std::endl;
        fileB->Close();
        return;
    }

    // Get the TTree from fileB.root
    TTree* treeB = dynamic_cast<TTree*>(fileB->Get("EventTree"));
    if (!treeB) {
        std::cerr << "Error: Could not retrieve TTree from fileB.root" << std::endl;
        fileA->Close();
        fileB->Close();
        return;
    }

    // Get the TBranch from fileA.root
    TTree* treeA = dynamic_cast<TTree*>(fileA->Get("EventTree"));
    if (!treeA) {
        std::cerr << "Error: Could not retrieve TTree from fileA.root" << std::endl;
        fileA->Close();
        fileB->Close();
        return;
    }

    TBranch* branchA = treeA->GetBranch("clk");
    if (!branchA) {
        std::cerr << "Error: Could not retrieve TBranch from fileA.root" << std::endl;
        fileA->Close();
        fileB->Close();
        return;
    }

    // Clone the TBranch from fileA.root and add it to treeB
    // TBranch* newBranch = treeA->CloneTree()->GetBranch("clk");
    treeB -> Branch(branchA)
    // TBranch* newBranch = branchA->Branch("new_branch_name", 0);

    // Loop over entries in fileA.root, read the value of the branch,
    // and fill it into the corresponding branch in treeB
    Long64_t numEntries = treeA->GetEntries();
    for (Long64_t i = 0; i < 2; ++i) {
        treeA->GetEntry(i);
        newBranch->Fill();
    }

    // Write the changes to fileB.root
    fileB->Write("", TObject::kOverwrite);

    // Clean up
    fileA->Close();
    fileB->Close();
}


int event_combiner_test3() {
    const char* file1Name = "/sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/20869/folder_INTTMBD_EventCombiner/intt_run20869_test.root";
    const char* file2Name = "/sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/20869/folder_INTTMBD_EventCombiner/centrality_run20869_test.root";

    AddBranchFromFileAToFileB(file2Name, file1Name);

    return 0;
}




// int event_combiner_test3() {
//     // const char* inputFile1 = "input1.root";
//     // const char* inputFile2 = "input2.root";
//     // const char* outputFile = "output.root";

//     const char* inputFile1 = "/sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/20869/folder_INTTMBD_EventCombiner/intt_run20869.root";
//     const char* inputFile2  = "/sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/20869/folder_INTTMBD_EventCombiner/centrality_run20869.root";
//     const char* outputFile      = "/sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/20869/folder_INTTMBD_EventCombiner/Final_combine_merge_test.root";

//     MergeTTrees(inputFile1, inputFile2, outputFile);

//     return 0;
// }
