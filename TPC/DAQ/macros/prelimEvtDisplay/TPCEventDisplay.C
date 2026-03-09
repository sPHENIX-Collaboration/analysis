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

TVector2 getBinPosition(int sector, int inFee, int channel, TTree* R1, TTree* R2, TTree* R3)
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
      TVector2 pos;
      pos.SetX(x); pos.SetY(y);
      return pos; 
    }
  }
  TVector2 pos;
  pos.SetX(0); pos.SetY(0);
  return pos;
}


void TPCEventDisplay(const float adcCut = 100., 
                     const string &outfile = "/sphenix/user/rosstom/test/TPCEventDisplay_10684", 
                     const string &indir = "/sphenix/user/rosstom/test/testFiles/", 
                     const string &runName = "beam-00010684",
		     const bool writeAllHits = true){

  //output nTuple
  TString* outputfilename=new TString(outfile+".root");
  TFile* outputfile=new TFile(outputfilename->Data(),"recreate");
  TTree* hitTree=new TTree("hitTree","x,y,z position and max ADC value for all samples passing cut");
  double x_Pos, y_Pos, z_Pos;
  int max_adc;
  hitTree->Branch("x_Pos",&x_Pos,"x_Pos/D");
  hitTree->Branch("y_Pos",&y_Pos,"y_Pos/D");
  hitTree->Branch("z_Pos",&z_Pos,"z_Pos/D");
  hitTree->Branch("max_adc",&max_adc,"max_adc/I"); 
 
  std::ofstream outdata;
  outdata.open((outfile+".json").c_str());
  if( !outdata ) { // file couldn't be opened
      cerr << "ERROR: file could not be opened" << endl;
      exit(1);
  }

  string runNumberString = runName;
  size_t pos = runNumberString.find("000");
  runNumberString.erase(runNumberString.begin(),runNumberString.begin()+pos+3);

  outdata << "{\n    \"EVENT\": {\n        \"runid\":" << runNumberString << ", \n        \"evtid\": 1, \n        \"time\": 0, \n \"timeStr\": \"2023-05-30, 15:23:30 EST\", \n       \"type\": \"Cosmics\", \n        \"s_nn\": 0, \n        \"B\": 0.0,\n        \"pv\": [0,0,0]  \n    },\n" << endl;

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

  //int framevals[12] = {77,67,69,71,70,79,77,70,69,71,64,77}; 

  for (int q = 0; q < 24; q++)
  {
    cout << "Processing sector " << q << endl;

    string sectorNumber;
    if (q < 10) sectorNumber = "0"+std::to_string(q);
    else sectorNumber = std::to_string(q);   

    string fileName = indir+"TPC_ebdc"+sectorNumber+"_"+runName+"-0005.prdf_TPCRawDataTree.root";

    TFile *TPCFile = TFile::Open(fileName.c_str());
    TTreeReader myReader("SampleTree", TPCFile);

    TTreeReaderValue<Int_t> nSamples(myReader, "nSamples");
    TTreeReaderValue<Int_t> fee(myReader, "fee");
    TTreeReaderArray<UShort_t> adcSamples(myReader, "adcSamples");
    TTreeReaderValue<Int_t> Channel(myReader, "Channel");
    TTreeReaderValue<Int_t> BCO(myReader, "BCO");
    TTreeReaderValue<Int_t> frame(myReader, "frame");
    TTreeReaderValue<Int_t> checksumError(myReader, "checksumError");
  
    while(myReader.Next())
    {
      //if (*frame != framevals[q] && *frame != framevals[q]+1) continue;

      int mod_arr[26]={2,2,1,1,1,3,3,3,3,3,3,2,2,1,2,2,1,1,2,2,3,3,3,3,3,3};
      
      if (*nSamples <= 1) continue;
      
      float adcMax = 0;
      float adcMaxPos = 0;
      if (writeAllHits && *checksumError == 0)
      {
        for(int adc_sam_no=0;adc_sam_no<*nSamples;adc_sam_no++)
        {
          if (adcSamples[adc_sam_no] - adcSamples[0] > adcCut)
          {
            max_adc = adcSamples[adc_sam_no];
            double zPos;
            if (q < 12)
            { //0.4 cm per sample, 50 ns/sample, 8 cm/us drift speed
              zPos = 105. - 0.4*(double)adc_sam_no; // - (*BCO - globalEventBCO)*(8./9.4);
              //if (zPos < 0. || zPos > 105.) continue;
	    }
            else
            {
              zPos = -105. + 0.4*(double)adc_sam_no; // + (*BCO - globalEventBCO)*(8./9.4);
              //if (zPos > 0. || zPos < 105.) continue;
            }
            
            TVector2 binXY = getBinPosition(q,*fee,*Channel,R1,R2,R3);
    
            if (binXY.X() == 0 || binXY.Y() == 0) continue;

            x_Pos=binXY.X();
            y_Pos=binXY.Y();
            z_Pos=zPos;
            max_adc=adcMax;
            hitTree->Fill();

            stringstream spts;

            if (firstClus) firstClus = false;
            else spts << ","; 
            
	    spts << "{ \"x\": ";
            spts << binXY.X();
            spts << ", \"y\": ";
            spts << binXY.Y();
            spts << ", \"z\": ";
            spts << zPos;
            spts << ", \"e\": ";
            spts << adcMax;
            spts << "}";

            outdata << (boost::format("%1%") % spts.str());
            spts.clear();
            spts.str("");
          }
        }
      }
      else
      {
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
          //adcMaxPos = 0;
          if (q < 12)
          { //0.4 cm per sample, 50 ns/sample, 8 cm/us drift speed
            zPos = 105. - 0.4*(double)adcMaxPos; // - (*BCO - globalEventBCO)*(8./9.4);
            if (zPos < 0. || zPos > 105.) continue;
	  }
          else
          {
            zPos = -105. + 0.4*(double)adcMaxPos; // + (*BCO - globalEventBCO)*(8./9.4);
            if (zPos > 0. || zPos < -105.) continue;
          }
          
          TVector2 binXY = getBinPosition(q,*fee,*Channel,R1,R2,R3);

          if (binXY.X() == 0 || binXY.Y() == 0) continue;    
   
          x_Pos=binXY.X();
          y_Pos=binXY.Y();
          z_Pos=zPos;
          max_adc=adcMax;
          hitTree->Fill();
 
          stringstream spts;
        
          if (firstClus) firstClus = false;
          else spts << ",";

          spts << "{ \"x\": ";
          spts << binXY.X();
          spts << ", \"y\": ";
          spts << binXY.Y();
          spts << ", \"z\": ";
          spts << zPos;
          spts << ", \"e\": ";
          spts << adcMax;
          spts << "}";

          outdata << (boost::format("%1%") % spts.str());
          spts.clear();
          spts.str("");
        }
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

  outputfile->cd();
  hitTree->Write();
  outputfile->Close(); 
}
