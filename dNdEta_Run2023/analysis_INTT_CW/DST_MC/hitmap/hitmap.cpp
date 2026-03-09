#include "hitmap.h"
#include <iostream>
// #include "sPhenixStyle.C"
// #include "InttEvent.cc"

HITMAP::HITMAP(std::string input_file_directory, std::string input_tree_name, std::string output_directory)
: input_file_directory(input_file_directory), input_tree_name(input_tree_name), output_directory(output_directory)
{
    SetsPhenixStyle();
    gStyle->SetOptStat(0); // Turn off the statistics box
    
    Init();
    
    return;
}

HITMAP::~HITMAP()
{
    return;
}

void HITMAP::Init()
{
    file_in = TFile::Open((input_file_directory).c_str());
    tree_in = (TTree*)file_in->Get((input_tree_name).c_str());
    inttEvt_ = nullptr;
    tree_in->SetBranchAddress("event", &inttEvt_);

    std::cout<<"Total events : "<<tree_in->GetEntries()<<std::endl;

    return;
}