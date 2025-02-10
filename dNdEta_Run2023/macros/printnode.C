#include <filesystem>
#include <iostream>
#include <string>

void printnode(                                     //
    const string dstfile = "./DST_INTTCluster.root" //
)
{
    TFile *f = new TFile(dstfile.c_str(), "read");
    TTree *T = (TTree *)f->Get("T");
    T->Print("toponly");
    std::cout << "Number of entries: " << T->GetEntries() << std::endl;
}