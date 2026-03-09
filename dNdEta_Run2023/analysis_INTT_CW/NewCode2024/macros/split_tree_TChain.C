#include <iostream>
#include <fstream>
#include <vector>
#include <string>

std::vector<std::string> readFileToVector(const std::string& filePath) {
    std::vector<std::string> lines; // Vector to store lines
    std::ifstream inputFile(filePath); // Open the file

    if (!inputFile) {
        std::cerr << "Error: Unable to open file at " << filePath << std::endl;
        return lines; // Return an empty vector
    }

    std::string line;
    while (std::getline(inputFile, line)) { // Read file line by line
        lines.push_back(line); // Add each line to the vector
    }

    inputFile.close(); // Close the file

    for (const auto& string_line : lines) {
        std::cout << string_line << std::endl; // Print each line
    }

    return lines; // Return the vector of lines
}



void split_tree_TChain(std::string generator_name, int FileList_index = 1, int File_shift_index = 0) {

    std::map<std::string,std::string> sample_directory ={
        {"HIJING", "Sim_HIJING_MDC2_ana472_20250307"}, // note : new 
        {"AMPT", "Sim_AMPT_MDC2_ana472_20250310"}, // note : new
        {"EPOS", "Sim_EPOS_MDC2_ana472_20250310"}, // note : new 
        {"HStrange", "Sim_HIJING_strangeness_MDC2_ana472_20250310"} // note : new 
    };

    string input_directory = Form("/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/%s", sample_directory[generator_name].c_str());
    string output_directory = Form("/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/%s/per5k", sample_directory[generator_name].c_str()); // note : auto
    
    std::cout << "Input directory: " << input_directory << std::endl;
    std::cout << "Output directory: " << output_directory << std::endl;
    
    system (Form("mkdir -p %s", output_directory.c_str()));
    
    string input_filelist = Form("FileList_00%d.txt",FileList_index); 
    string TreeName = "EventTree";
    int nEvent_EachFile = 5000;
    string input_filename_NoSuffix = "ntuple_per5k";

    std::vector<std::string> file_list = readFileToVector(input_directory + "/" + input_filelist);
    // std::string first_filename = file_list[0].substr(file_list[0].find_last_of("/")+1);
    // string input_filename_NoSuffix = first_filename.substr(0, first_filename.find("."));

    TChain * inputTree = new TChain(TreeName.c_str());
    for (const auto& file : file_list) {
        inputTree->Add((file).c_str());
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

        std::string job_index = std::to_string( i + File_shift_index );
        int job_index_len = 5;
        job_index.insert(0, job_index_len - job_index.size(), '0');

        // note : Create output file name
        string output_filename = input_filename_NoSuffix + "_" + job_index + ".root";
        TFile *outputFile = TFile::Open((output_directory+"/"+output_filename).c_str(), "RECREATE");

        // note : Create a new TTree with the subset of events
        TTree *outputTree = inputTree->CopyTree(Form("Entry$ >= %lld && Entry$ <= %lld", startEvent, endEvent));

        // note : Write the new TTree to the output file
        outputTree->Write();
        outputFile->Close();
        std::cout << "Created file: " << output_filename << " with events [" << startEvent << " - " << endEvent << "]" << std::endl;
    }

    // note : Close the input file
    // inputFile->Close();
}
