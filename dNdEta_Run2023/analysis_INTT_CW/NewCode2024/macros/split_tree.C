void split_tree() {

    string input_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_Ntuple_HIJING_ana443_20241102";
    string input_filename = "ntuple_merged.root"; 
    string TreeName = "EventTree";
    int nEvent_EachFile = 500;

    // gSystem->Load("libg4dst");

    // string input_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_23911/INTTRawHit/completed";
    // string input_filename = "inttrawhit_00023911_00000_syncDST_600k.root"; 
    // string TreeName = "T";
    // int nEvent_EachFile = 1000;

    string input_filename_NoSuffix = input_filename.substr(0, input_filename.find("."));

    // note : Open the input ROOT file
    TFile *inputFile = TFile::Open((input_directory + "/" + input_filename).c_str(), "READ");
    if (!inputFile || inputFile->IsZombie()) {
        std::cerr << "Error: Could not open input file!" << std::endl;
        return;
    }

    // note : Get the TTree from the file
    TTree *inputTree = dynamic_cast<TTree*>(inputFile->Get(TreeName.c_str())); // note : Replace "tree" with your TTree's name
    if (!inputTree) {
        std::cerr << "Error: TTree not found in the input file!" << std::endl;
        inputFile->Close();
        return;
    }

    // note : Get the total number of events
    Long64_t nTotalEvents = inputTree->GetEntries();

    // note : Calculate the number of full files
    int nFiles = nTotalEvents / nEvent_EachFile;
    // int remainingEvents = nTotalEvents % nEvent_EachFile;

    std::cout << "Total events: " << nTotalEvents << std::endl;
    std::cout << "Events per file: " << nEvent_EachFile << std::endl;
    std::cout << "n output files: " << nFiles << std::endl;
    // std::cout << "Remaining events: " << remainingEvents << std::endl;

    // note : Loop to create output files
    for (int i = 0; i < nFiles; ++i) {
        // note : Define the event range for this file
        Long64_t startEvent = i * nEvent_EachFile;
        Long64_t endEvent = (i == nFiles - 1) ? nTotalEvents - 1 : startEvent + nEvent_EachFile - 1;

        std::cout<<"In core: "<<i<<" startEvent: "<<startEvent<<" endEvent: "<<endEvent<<std::endl;

        // note : Skip the last file if it has no events
        if (startEvent >= nTotalEvents) break;

        std::string job_index = std::to_string( i );
        int job_index_len = 5;
        job_index.insert(0, job_index_len - job_index.size(), '0');

        // note : Create output file name
        string output_filename = input_filename_NoSuffix + "_" + job_index + ".root";
        TFile *outputFile = TFile::Open((input_directory+"/"+output_filename).c_str(), "RECREATE");

        // note : Create a new TTree with the subset of events
        TTree *outputTree = inputTree->CopyTree(Form("Entry$ >= %lld && Entry$ <= %lld", startEvent, endEvent));

        // note : Write the new TTree to the output file
        outputTree->Write();
        outputFile->Close();
        std::cout << "Created file: " << output_filename << " with events [" << startEvent << " - " << endEvent << "]" << std::endl;
    }

    // note : Close the input file
    inputFile->Close();
}
