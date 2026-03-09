#include <iostream>
#include "InttEvent.h"
#include "TFile.h"
#include "TTree.h"

using namespace std;

class InttEvent;

int main ()
{
    string input_file_directory = "/sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/20869/beam_inttall-00020869-0000_event_base_ana.root";
    string input_tree_name = "input_tree_name";
    TFile * file_in = TFile::Open((input_file_directory).c_str());
    TTree * tree_in = (TTree*)file_in->Get((input_tree_name).c_str());
    InttEvent * inttEvt_ = nullptr;
    tree_in->SetBranchAddress("event", &inttEvt_);

    std::cout<<"Total events : "<<tree_in->GetEntries()<<std::endl;

}