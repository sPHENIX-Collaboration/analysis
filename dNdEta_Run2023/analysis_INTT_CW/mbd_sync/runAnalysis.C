#include "Analyze.h"
#include <fstream>

using namespace std;

R__LOAD_LIBRARY(libAnalyze.so)

void runAnalysis(const char * listfilename="")
{
/*
  //std::string save_dir = "/home/phnxrc/INTT/commissioning_5_23/hit_files/";
  std::string save_dir = "./";

  ifstream fin(listfilename);
  if(!fin) {
      cout<<"failed to open file : "<<listfilename<<endl;
  }

  char filename[2048];
  fin>>filename;
  fin.close();

  TString out_file_name = gSystem->BaseName(filename);
  out_file_name.ReplaceAll(".root", "_ana.root");
  out_file_name.Replace(out_file_name.Index("intt", 0), 5, "inttall");
  out_file_name = save_dir + out_file_name;

  std::cout << out_file_name << std::endl;

  // read and generate InttTree

  if ( listfilename != NULL)
    {
      InitAnalysis(out_file_name.Data());
      std::cout<<listfilename<<std::endl;
      RunAnalysis(listfilename);
    }
*/
      InitAnalysis("test.root");
      RunAnalysis("");

}
