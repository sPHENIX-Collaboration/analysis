#include <cstdlib>
#include <iostream>
#include <string>

#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TROOT.h"

#include <boost/format.hpp>
#include <boost/math/special_functions/sign.hpp>
/*************************************************************/
/*                   TPC Event Display                       */
/*               Thomas Marshall,Aditya Dash                 */
/*        rosstom@ucla.edu,aditya55@physics.ucla.edu         */
/*************************************************************/

void TPCEventDisplay_Clusters(const string &outfile = "./TPCEventDisplay_cosmics25926_clusters", 
                     const string &infile = "/sphenix/u/rosstom/macros/TrackingProduction/cosmics25926_ntup.root",
                     const string &runNumber = "25926",
                     int nEventsToRun = 5){
 
  TFile *ntpFile = TFile::Open(infile.c_str());
  TTree *hitTree = (TTree*)ntpFile->Get("ntp_cluster");

  Float_t event;
  Float_t x, y, z, adc;
  hitTree->SetBranchAddress("event",&event);
  hitTree->SetBranchAddress("x",&x);
  hitTree->SetBranchAddress("y",&y);
  hitTree->SetBranchAddress("z",&z);
  hitTree->SetBranchAddress("adc",&adc);

  Int_t currentEvent = 0; 
  bool firstHit = true;

  std::ofstream outdata;
  outdata.open((outfile+"_event0.json").c_str(),std::ofstream::out | std::ofstream::trunc);
  if( !outdata ) { // file couldn't be opened
      cerr << "ERROR: file could not be opened" << endl;
      exit(1);
  }
  outdata << "{\n    \"EVENT\": {\n        \"runid\":" << runNumber << ", \n        \"evtid\": 1, \n        \"time\": 0, \n \"timeStr\": \"2023-08-23, 15:23:30 EST\", \n       \"type\": \"Cosmics\", \n        \"s_nn\": 0, \n        \"B\": 0.0,\n        \"pv\": [0,0,0],\n  \"runstats\": [ \n  \"sPHENIX Time Projection Chamber\", \"2023-08-23, Run 25926 - Event 1, Clusters\", \"Cosmics Data\"] \n   },\n" << endl;

  outdata << "    \"META\": {\n       \"HITS\": {\n          \"INNERTRACKER\": {\n              \"type\": \"3D\",\n              \"options\": {\n              \"size\": 2,\n              \"color\": 16777215\n              } \n          },\n" << endl;
  outdata << "          \"TRACKHITS\": {\n              \"type\": \"3D\",\n              \"options\": {\n              \"size\": 2,\n              \"transparent\": 0.5,\n              \"color\": 16777215\n              } \n          },\n" << endl;
  outdata << "    \"JETS\": {\n        \"type\": \"JET\",\n        \"options\": {\n            \"rmin\": 0,\n            \"rmax\": 78,\n            \"emin\": 0,\n            \"emax\": 30,\n            \"color\": 16777215,\n            \"transparent\": 0.5 \n        }\n    }\n        }\n    }\n," << endl;
  outdata << "    \"HITS\": {\n        \"CEMC\":[{\"eta\": 0, \"phi\": 0, \"e\": 0}\n            ],\n        \"HCALIN\": [{\"eta\": 0, \"phi\": 0, \"e\": 0}\n            ],\n        \"HCALOUT\": [{\"eta\": 0, \"phi\": 0, \"e\": 0}\n \n            ],\n\n" << endl;
  outdata << "    \"TRACKHITS\": [\n\n "; 

  for(int i = 0; i < hitTree->GetEntries(); i++)
  {
    hitTree->GetEntry(i);
 
    if (event != currentEvent)
    {  
      if (event == currentEvent + 1) 
      {
        outdata << "],\n    \"JETS\": [\n         ]\n    }," << endl;
        outdata << "\"TRACKS\": {" << endl;
        outdata <<"\""<<"INNERTRACKER"<<"\": [";
        outdata << "]" << endl;
        outdata << "}" << endl;
        outdata << "}" << endl;
        outdata.close();

        currentEvent = event;
        firstHit = true;
        if (currentEvent >= nEventsToRun) break;

        outdata.open((outfile+"_event"+std::to_string(currentEvent)+".json").c_str(),std::ofstream::out | std::ofstream::trunc);
        if( !outdata ) 
        { // file couldn't be opened
          cerr << "ERROR: file could not be opened" << endl;
          exit(1);
        }
        outdata << "{\n    \"EVENT\": {\n        \"runid\":" << runNumber << ", \n        \"evtid\": 1, \n        \"time\": 0, \n \"timeStr\": \"2023-08-23, 15:23:30 EST\", \n       \"type\": \"Cosmics\", \n        \"s_nn\": 0, \n        \"B\": 0.0,\n        \"pv\": [0,0,0],\n  \"runstats\": [ \n  \"sPHENIX Time Projection Chamber\", \"2023-08-23, Run 25926 - Event" << currentEvent+1 << ", Clusters\", \"Cosmics Data\"] \n   },\n" << endl;

        outdata << "    \"META\": {\n       \"HITS\": {\n          \"INNERTRACKER\": {\n              \"type\": \"3D\",\n              \"options\": {\n              \"size\": 2,\n              \"color\": 16777215\n              } \n          },\n" << endl;
        outdata << "          \"TRACKHITS\": {\n              \"type\": \"3D\",\n              \"options\": {\n              \"size\": 2,\n              \"transparent\": 0.5,\n              \"color\": 16777215\n              } \n          },\n" << endl;
        outdata << "    \"JETS\": {\n        \"type\": \"JET\",\n        \"options\": {\n            \"rmin\": 0,\n            \"rmax\": 78,\n            \"emin\": 0,\n            \"emax\": 30,\n            \"color\": 16777215,\n            \"transparent\": 0.5 \n        }\n    }\n        }\n    }\n," << endl;
        outdata << "    \"HITS\": {\n        \"CEMC\":[{\"eta\": 0, \"phi\": 0, \"e\": 0}\n            ],\n        \"HCALIN\": [{\"eta\": 0, \"phi\": 0, \"e\": 0}\n            ],\n        \"HCALOUT\": [{\"eta\": 0, \"phi\": 0, \"e\": 0}\n \n            ],\n\n" << endl;
        outdata << "    \"TRACKHITS\": [\n\n ";
      }
      else 
      {
        std::cout << "Error: hit for a previous event placed later in the ntuple" << std::endl;
        break;
      }
    }
    
    stringstream spts;

    if (firstHit) firstHit = false;
    else spts << ",";

    spts << "{ \"x\": ";
    spts << x;
    spts << ", \"y\": ";
    spts << y;
    spts << ", \"z\": ";
    spts << z;
    spts << ", \"e\": ";
    spts << adc;
    spts << "}";

    outdata << (boost::format("%1%") % spts.str());
    spts.clear();
    spts.str("");
  }  
  std::cout << "Done" << std::endl; 
}
