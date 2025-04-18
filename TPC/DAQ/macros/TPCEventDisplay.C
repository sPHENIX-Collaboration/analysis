#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TTreeReaderArray.h"
#include "TVector3.h"

#include <boost/format.hpp>
#include <boost/math/special_functions/sign.hpp>

/*************************************************************/
/*               TPC Raw Data Event Display                  */
/*               Thomas Marshall,Aditya Dash                 */
/*        rosstom@ucla.edu,aditya55@physics.ucla.edu         */
/*************************************************************/

TVector3 getBinPosition(int sector, int inFee, int channel, double zPos, TTree* R1, TTree* R2, TTree* R3)
{
  int mod_arr[26] = {2,2,1,1,1,3,3,3,3,3,3,2,2,1,2,2,1,1,2,2,3,3,3,3,3,3};
  float slot[26] = {5,6,1,3,2,12,10,11,9,8,7,1,2,4,8,7,6,5,4,3,1,3,2,4,6,5};
  TTreeReader* myReader;
  if (mod_arr[inFee] == 1)
  {
    myReader = new TTreeReader(R1);
  }
  else if (mod_arr[inFee] == 2)
  {
    myReader = new TTreeReader(R2);
  }
  else if (mod_arr[inFee] == 3)
  {
    myReader = new TTreeReader(R3);
  }
  TTreeReaderValue<Float_t> FEE(*myReader, "FEE");
  TTreeReaderValue<Int_t> FEE_Chan(*myReader, "FEE_Chan");
  //TTreeReaderValue<Double_t> x(myReader, "x");
  //TTreeReaderValue<Double_t> y(myReader, "y");
  TTreeReaderValue<Double_t> phi(*myReader, "phi");
  //TTreeReaderValue<Double_t> PadX(myReader, "PadX");
  //TTreeReaderValue<Double_t> PadY(myReader, "PadY");
  TTreeReaderValue<Double_t> PadR(*myReader, "PadR");
  //TTreeReaderValue<Double_t> PadPhi(myReader, "PadPhi");

  double phiOffset;
  if (sector < 12) phiOffset = 2*M_PI*((double)sector/12.) - 2*M_PI*(1./12.)/2;
  else phiOffset = 2*M_PI*(((double)sector-12.)/12.) - 2*M_PI*(1./12.)/2;

  while(myReader->Next())
  {
    if (slot[inFee]-1 == *FEE && channel == *FEE_Chan)
    {
      double x = (*PadR/10)*std::cos(*phi+phiOffset);
      double y = (*PadR/10)*std::sin(*phi+phiOffset);
      TVector3 pos;
      pos.SetX(x); pos.SetY(y); pos.SetZ(zPos);
      return pos; 
    }
  }
  TVector3 pos;
  pos.SetX(0); pos.SetY(0); pos.SetZ(0);
  return pos;
}


void TPCEventDisplay(const float adcCut = 85., 
                     const string &outfile = "/sphenix/user/rosstom/test/TPCEventDisplay_10169.json", 
                     const string &indir = "/sphenix/user/rosstom/test/testFiles/", 
                     const string &runName = "pedestal-00010169"){
  
  std::ofstream outdata;
  outdata.open(outfile.c_str());
  if( !outdata ) { // file couldn't be opened
      cerr << "ERROR: file could not be opened" << endl;
      exit(1);
  }

  outdata << "{\n    \"EVENT\": {\n        \"runid\": 1, \n        \"evtid\": 1, \n        \"time\": 0, \n        \"type\": \"Collision\", \n        \"s_nn\": 0, \n        \"B\": 3.0,\n        \"pv\": [0,0,0]  \n    },\n" << endl;

  outdata << "    \"META\": {\n       \"HITS\": {\n          \"INNERTRACKER\": {\n              \"type\": \"3D\",\n              \"options\": {\n              \"size\": 5,\n              \"color\": 16777215\n              } \n          },\n" << endl;
  outdata << "          \"TRACKHITS\": {\n              \"type\": \"3D\",\n              \"options\": {\n              \"size\": 5,\n              \"transparent\": 0.5,\n              \"color\": 16777215\n              } \n          },\n" << endl;
  outdata << "    \"JETS\": {\n        \"type\": \"JET\",\n        \"options\": {\n            \"rmin\": 0,\n            \"rmax\": 78,\n            \"emin\": 0,\n            \"emax\": 30,\n            \"color\": 16777215,\n            \"transparent\": 0.5 \n        }\n    }\n        }\n    }\n," << endl;
  outdata << "    \"HITS\": {\n        \"CEMC\":[{\"eta\": 0, \"phi\": 0, \"e\": 0}\n            ],\n        \"HCALIN\": [{\"eta\": 0, \"phi\": 0, \"e\": 0}\n            ],\n        \"HCALOUT\": [{\"eta\": 0, \"phi\": 0, \"e\": 0}\n \n            ],\n\n" << endl;
  outdata << "    \"TRACKHITS\": [\n\n ";

  TTree *R1 = new TTree("R1","TPC Sector Mapping for R1");
  TTree *R2 = new TTree("R2","TPC Sector Mapping for R1");
  TTree *R3 = new TTree("R3","TPC Sector Mapping for R1");

  R1->ReadFile("/sphenix/u/rosstom/calibrations/TPC/Mapping/PadPlane/AutoPad-R1-RevA.sch.ChannelMapping.csv",
               "Entry/I:Radius/I:Pad/I:U/I:G/I:Pin/C:PinColID/I:PinRowID/I:PadName/C:FEE/F:FEE_Connector/C:FEE_Chan/I:phi/D:x/D:y/D:PadX/D:PadY/D:PadR/D:PadPhi/D",','); 
  R2->ReadFile("/sphenix/u/rosstom/calibrations/TPC/Mapping/PadPlane/AutoPad-R2-RevA-Pads.sch.ChannelMapping.csv",
               "Entry/I:Radius/I:Pad/I:U/I:G/I:Pin/C:PinColID/I:PinRowID/I:PadName/C:FEE/F:FEE_Connector/C:FEE_Chan/I:phi/D:x/D:y/D:PadX/D:PadY/D:PadR/D:PadPhi/D",','); 
  R3->ReadFile("/sphenix/u/rosstom/calibrations/TPC/Mapping/PadPlane/AutoPad-R3-RevA.sch.ChannelMapping.csv",
               "Entry/I:Radius/I:Pad/I:U/I:G/I:Pin/C:PinColID/I:PinRowID/I:PadName/C:FEE/F:FEE_Connector/C:FEE_Chan/I:phi/D:x/D:y/D:PadX/D:PadY/D:PadR/D:PadPhi/D",','); 
 
  bool firstClus = true;

  for (int q = 0; q < 24; q++)
  {
    cout << "Processing sector " << q << endl;

    string sectorNumber;
    if (q < 10) sectorNumber = "0"+std::to_string(q);
    else sectorNumber = std::to_string(q);   

    string fileName = indir+"TPC_ebdc"+sectorNumber+"_"+runName+"-0000.prdf_TPCRawDataTree.root";

    TFile *TPCFile = TFile::Open(fileName.c_str());
    TTreeReader myReader("SampleTree", TPCFile);

    TTreeReaderValue<Int_t> nSamples(myReader, "nSamples");
    TTreeReaderValue<Int_t> fee(myReader, "fee");
    TTreeReaderArray<UShort_t> adcSamples(myReader, "adcSamples");
    TTreeReaderValue<Int_t> Channel(myReader, "Channel");

    while(myReader.Next())
    {
      int mod_arr[26]={2,2,1,1,1,3,3,3,3,3,3,2,2,1,2,2,1,1,2,2,3,3,3,3,3,3};
      
      if (*nSamples <= 1) continue;
      
      float adcMax = 0;
      float adcMaxPos = 0;
      for(int adc_sam_no=0;adc_sam_no<*nSamples;adc_sam_no++)
      {
        if (adc_sam_no == 0)
	{
	  adcMax = adcSamples[adc_sam_no];
  	  adcMaxPos = adc_sam_no;
 	}
        else
        {
	  if (adcSamples[adc_sam_no] > adcMax)
	  {
 	    adcMax = adcSamples[adc_sam_no];
            adcMaxPos = adc_sam_no;
	  }
        }
      }
      if (adcMax > adcCut)
      {
	double zPos;
	if (q < 12)
	{
	  zPos = 105. - (adcMaxPos/((double)*nSamples-1.))*105.; 
	}
        else
	{
          zPos = -105. + (adcMaxPos/((double)*nSamples-1.))*105.;
	}
        TVector3 binXYZ = getBinPosition(q,*fee,*Channel,zPos,R1,R2,R3);

        TVector3 zeroVec;
        zeroVec.SetX(0);zeroVec.SetY(0);zeroVec.SetZ(0);
        if (binXYZ == zeroVec) continue;    
    
        stringstream spts;
        
        if (firstClus) firstClus = false;
        else spts << ",";

        spts << "{ \"x\": ";
        spts << binXYZ.x();
        spts << ", \"y\": ";
        spts << binXYZ.y();
        spts << ", \"z\": ";
        spts << binXYZ.z();
        spts << ", \"e\": ";
        spts << adcMax;
        spts << "}";

        outdata << (boost::format("%1%") % spts.str());
        spts.clear();
        spts.str("");
      } 
    }
  }
  outdata << "],\n    \"JETS\": [\n         ]\n    }," << endl;
  outdata << "\"TRACKS\": {" << endl;
  outdata <<"\""<<"INNERTRACKER"<<"\": [";
  outdata << "]" << endl;
  outdata << "}" << endl;
  outdata << "}" << endl; 
 
  outdata.close();     
}
