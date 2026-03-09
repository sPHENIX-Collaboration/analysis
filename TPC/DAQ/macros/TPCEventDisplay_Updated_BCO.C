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

#include <tpc/TpcMap.h>

#include <boost/format.hpp>
#include <boost/math/special_functions/sign.hpp>

R__LOAD_LIBRARY(libtpc.so)
/*************************************************************/
/*               TPC Raw Data Event Display                  */
/*               Thomas Marshall,Aditya Dash                 */
/*        rosstom@ucla.edu,aditya55@physics.ucla.edu         */
/*************************************************************/

void TPCEventDisplay_Updated_BCO(const float adcCut = 10., 
                     const string &outfile = "TPCEventDisplay_25926_BCO", 
                     const string &indir = "./", 
                     const string &runName = "cosmics-00025926",
		     const bool writeAllHits = true){

  //output nTuple
  TString* outputfilename=new TString(outfile+".root");
  TFile* outputfile=new TFile(outputfilename->Data(),"recreate");
  TTree* hitTree=new TTree("hitTree","x,y,z position and max ADC value for all samples passing cut");
  double x_Pos, y_Pos, z_Pos, frameVal;
  int adcMinusPedestal, sectorNum, layer, side, phiElement, timeBin, pad;
  hitTree->Branch("x_Pos",&x_Pos,"x_Pos/D");
  hitTree->Branch("y_Pos",&y_Pos,"y_Pos/D");
  hitTree->Branch("z_Pos",&z_Pos,"z_Pos/D");
  hitTree->Branch("frameVal",&frameVal,"frameVal/D");
  hitTree->Branch("adcMinusPedestal",&adcMinusPedestal,"adcMinusPedestal/I"); 
  hitTree->Branch("sectorNum",&sectorNum,"sectorNum/I");
  hitTree->Branch("layer",&layer,"layer/I");
  hitTree->Branch("side",&side,"side/I");
  hitTree->Branch("phiElement",&phiElement,"phiElement/I");
  hitTree->Branch("timeBin",&timeBin,"timeBin/I");
  hitTree->Branch("pad",&pad,"pad/I");
 
  std::ofstream outdata;
  outdata.open((outfile+".json").c_str());
  if( !outdata ) { // file couldn't be opened
      cerr << "ERROR: file could not be opened" << endl;
      exit(1);
  }

  string runNumberString = runName;
  size_t pos = runNumberString.find("000");
  runNumberString.erase(runNumberString.begin(),runNumberString.begin()+pos+3);

  outdata << "{\n    \"EVENT\": {\n        \"runid\":" << runNumberString << ", \n        \"evtid\": 1, \n        \"time\": 0, \n \"timeStr\": \"2023-05-30, 15:23:30 EST\", \n       \"type\": \"Cosmics\", \n        \"s_nn\": 0, \n        \"B\": 0.0,\n        \"pv\": [0,0,0],\n  \"runstats\": [ \n  \"sPHENIX Time Projection Chamber\", \"2023-08-23, Run 25926\", \"Cosmics Data\"] \n   },\n" << endl;

  outdata << "    \"META\": {\n       \"HITS\": {\n          \"INNERTRACKER\": {\n              \"type\": \"3D\",\n              \"options\": {\n              \"size\": 2,\n              \"color\": 16777215\n              } \n          },\n" << endl;
  outdata << "          \"TRACKHITS\": {\n              \"type\": \"3D\",\n              \"options\": {\n              \"size\": 2,\n              \"transparent\": 0.5,\n              \"color\": 16777215\n              } \n          },\n" << endl;
  outdata << "    \"JETS\": {\n        \"type\": \"JET\",\n        \"options\": {\n            \"rmin\": 0,\n            \"rmax\": 78,\n            \"emin\": 0,\n            \"emax\": 30,\n            \"color\": 16777215,\n            \"transparent\": 0.5 \n        }\n    }\n        }\n    }\n," << endl;
  outdata << "    \"HITS\": {\n        \"CEMC\":[{\"eta\": 0, \"phi\": 0, \"e\": 0}\n            ],\n        \"HCALIN\": [{\"eta\": 0, \"phi\": 0, \"e\": 0}\n            ],\n        \"HCALOUT\": [{\"eta\": 0, \"phi\": 0, \"e\": 0}\n \n            ],\n\n" << endl;
  outdata << "    \"TRACKHITS\": [\n\n ";

  bool firstClus = true;

  //int framevals[12] = {77,67,69,71,70,79,77,70,69,71,64,77}; 
  //int framevals[24] = {922,843,839,875,863,890,834,849,853,815,837,855,0,0,0,0,0,0,0,0,0,0,0,0}; //sector 3 
  //int framevals[24] = {0,0,0,875,0,0,0,0,0,0,0,0,1152,1071,1076,1083,1211,1137,1054,1158,1095,67,1532,1169}; //sector 12
  //int framevals[24] = {23,21,23,21,21,20,20,23,22,19,24,24,23,22,23,24,24,23,25,21,25,22,-1,-1};
  //int framevals[24] = {88,87,91,92,86,91,91,91,90,88,90,88,98,96,-1,87,112,92,89,86,105,95,280,251};
  //int framevals[24] = {9,16,15,13,9,13,16,14,13,13,11,14,14,12,10,14,13,14,13,14,14,14,13,13};
  //int framevals[24] = {20,30,27,25,19,31,36,27,26,25,22,24,24,24,22,25,23,26,23,26,24,24,25,25};
  //int framevals[24] = {38,48,-1,43,37,52,64,50,44,41,39,40,47,40,47,43,41,46,39,48,42,42,50,44};
  //int framevals[24] = {26,36,33,31,25,38,47,34,33,31,28,30,34,30,-1,31,29,34,29,36,32,32,34,32};
  //int framevals[24] = {4,4,6,4,4,4,6,6,4,4,4,4,4,4,4,5,5,5,4,4,6,4,4,4};
  //int framevals[24] = {11,18,17,16,11,17,19,16,16,16,14,16,16,14,12,16,15,16,15,16,16,16,15,15};
  //int framevals[24] = {28,38,35,34,27,40,51,38,36,33,30,32,37,32,36,33,31,36,31,38,34,34,38,34};
  //int framevals[24] = {30,40,37,37,29,42,54,41,38,35,32,34,39,34,38,35,33,40,33,40,36,36,42,36};
  //int framevals[24] = {38,48,37,43,37,52,64,50,44,41,39,40,47,40,47,43,41,46,39,48,42,42,50,44};
  //int framevals[24] = {44,55,50,52,43,62,76,58,51,48,45,46,58,46,54,53,47,52,45,54,50,50,57,53};
  //int framevals[24] = {80,87,83,85,75,93,76,91,83,81,75,74,92,74,91,84,77,80,85,85,82,82,93,86};
  //int framevals[24] = {84,91,89,89,79,97,76,97,87,85,79,74,96,80,97,90,83,86,90,90,86,86,97,90};

  TpcMap M;
  M.setMapNames("AutoPad-R1-RevA.sch.ChannelMapping.csv", "AutoPad-R2-RevA-Pads.sch.ChannelMapping.csv", "AutoPad-R3-RevA.sch.ChannelMapping.csv");

  int mod_arr[26] = {2,2,1,1,1,3,3,3,3,3,3,2,2,1,2,2,1,1,2,2,3,3,3,3,3,3};

  //int FEE_map[26] = {3, 2, 5, 3, 4, 0, 2, 1, 3, 4, 5, 7, 6, 2, 0, 1, 0, 1, 4, 5, 11, 9, 10, 8, 6, 7};
  // updated FEE mapping from Tom's description
  int FEE_map[26] = {
    4, 
    5, 
    0, 
    2, 
    1, 
    11, 
    9, 
    10, 
    8, 
    7, 
    6, 
    0, 
    1, 
    3, 
    7, 
    6, 
    5, 
    4, 
    3, 
    2, 
    0, 
    2, 
    1, 
    3, 
    5, 
    4
  };


  float pads_per_sector[3] = {96, 128, 192};  

  //ULong64_t BCOVal = 128330850912;
  //  ULong64_t BCOVal = 128330850911;
  ULong64_t BCOVal =128434038131;

  for (int q = 0; q < 24; q++)
  {
    //if (q == 17) continue;
    cout << "Processing sector " << q << endl;
  
    sectorNum = q;

    string sectorNumber;
    if (q < 10) sectorNumber = "0"+std::to_string(q);
    else sectorNumber = std::to_string(q);   

    string fileName = indir+"TPC_ebdc"+sectorNumber+"_"+runName+"-0001.prdf_test.root";

    TFile *TPCFile = TFile::Open(fileName.c_str());
    TTree *sampleTree = (TTree*)TPCFile->Get("SampleTree");
    TTree *taggerTree = (TTree*)TPCFile->Get("TaggerTree");

    Int_t nSamples, fee, Channel, frame, packet, checksumError, taggerFrame;
    ULong64_t BCO;
    Double_t xPos, yPos;
    UShort_t adcSamples[5000];
    sampleTree->SetBranchAddress("nSamples",&nSamples);
    sampleTree->SetBranchAddress("fee",&fee);
    sampleTree->SetBranchAddress("adcSamples",&adcSamples);
    sampleTree->SetBranchAddress("Channel",&Channel);
    //sampleTree->SetBranchAddress("BCO",&BCO);
    sampleTree->SetBranchAddress("frame",&frame);
    sampleTree->SetBranchAddress("checksumError",&checksumError);
    sampleTree->SetBranchAddress("xPos",&xPos);
    sampleTree->SetBranchAddress("yPos",&yPos);

    taggerTree->SetBranchAddress("bco",&BCO);
    taggerTree->SetBranchAddress("packet",&packet);
    taggerTree->SetBranchAddress("frame",&taggerFrame);

    double channelMean[26][256];
    double channelSigma[26][256];
    double channelSamples[26][256];
    int nHitsChannel[26][256];

    for(int fee_no=0;fee_no<26;fee_no++)
    {
      for(int channel_no=0;channel_no<256;channel_no++)
      {
        channelMean[fee_no][channel_no] = 0.;
        channelSigma[fee_no][channel_no] = 0.;
        channelSamples[fee_no][channel_no] = 0.;
        nHitsChannel[fee_no][channel_no] = 0;
      }
    }

    //TTreeReader myReader("SampleTree", TPCFile);

    /*
    TTreeReaderValue<Int_t> nSamples(myReader, "nSamples");
    TTreeReaderValue<Int_t> fee(myReader, "fee");
    TTreeReaderArray<UShort_t> adcSamples(myReader, "adcSamples");
    TTreeReaderValue<Int_t> Channel(myReader, "Channel");
    TTreeReaderValue<Int_t> BCO(myReader, "BCO");
    TTreeReaderValue<Int_t> frame(myReader, "frame");
    TTreeReaderValue<Int_t> checksumError(myReader, "checksumError");
    TTreeReaderValue<double> xPosition(myReader, "xPos");
    TTreeReaderValue<double> yPosition(myReader, "yPos");
    */ 
    
    std::vector<int> frameValsToCheck;

    for(int i = 0; i < taggerTree->GetEntries(); i++)
    {
      taggerTree->GetEntry(i);
      if ((BCO < BCOVal + 5) && (BCO > BCOVal - 5))
      //if (BCO == BCOVal)
      {
	cout <<"Sector "<<q<<" found packet = "<<packet<<" taggerFrame = "<<taggerFrame<<" with BCO = "<<BCO<<endl;
        frameValsToCheck.push_back(taggerFrame);
      } 
    } 

    for(int i = 0; i < sampleTree->GetEntries(); i++)
    {
      sampleTree->GetEntry(i);
      if (checksumError == 0)
      {
        for(int adc_sam_no=0;adc_sam_no<10;adc_sam_no++)
        {
          channelSamples[fee][Channel] += 1.;
          channelMean[fee][Channel] += adcSamples[adc_sam_no]; 
          channelSigma[fee][Channel] += pow(adcSamples[adc_sam_no],2); 
        } 
      } 
    }
   
    for(int fee_no=0;fee_no<26;fee_no++)
    {
      for(int channel_no=0;channel_no<256;channel_no++)
      {
        double temp1 = channelMean[fee_no][channel_no]/channelSamples[fee_no][channel_no];
        double temp2 = channelSigma[fee_no][channel_no]/channelSamples[fee_no][channel_no];
        channelMean[fee_no][channel_no] = temp1;
        channelSigma[fee_no][channel_no] = sqrt(temp2 - pow(temp1,2));
      }
    }

    //int nHitsInEntry[1000];
    for(int i = 0; i < sampleTree->GetEntries(); i++)
    {
      sampleTree->GetEntry(i);
      //if (frame != framevals[q] && frame != framevals[q]+1) continue;
      bool recordHits = false;
      for (auto & vals : frameValsToCheck)
      {
        if (frame < vals + 2 && frame > vals - 1) 
	{
          recordHits = true;
	  break;
	}
      }
      //if (!recordHits) continue;
      if (checksumError == 0 && channelMean[fee][Channel] > 20. && channelMean[fee][Channel] < 200.)
      {
        for(int adc_sam_no=0;adc_sam_no<nSamples;adc_sam_no++)
        {
          if ((adcSamples[adc_sam_no] - channelMean[fee][Channel] > channelSigma[fee][Channel]*4) && (adcSamples[adc_sam_no] - channelMean[fee][Channel] > adcCut))
          {
            nHitsChannel[fee][Channel] += 1;
            //nHitsInEntry[frame] += 1;
          }
        }
      }
    }

    int sample_count = 0;
    for(int i = 0; i < sampleTree->GetEntries(); i++)
    {
      sampleTree->GetEntry(i);
      //if (*frame > 100) continue;
      //if (q == 2 && (*frame < framevals[q] - 5 || *frame > framevals[q]+5)) continue;
      //if (frame != framevals[q] && frame != framevals[q]+1) continue;
      if (nHitsChannel[fee][Channel] > 50) continue;
      //if (nHitsInEntry[frame] > 100) continue;
      if (nSamples <= 1) continue;
      bool recordHits = false;
      for (auto & vals : frameValsToCheck)
      {
        if (frame < vals + 2 && frame > vals - 1) 
	{
          recordHits = true;
	  break;
	}
      }
      if (!recordHits) continue;
      ++sample_count;

      float adcMax = 0;
      float adcMaxPos = 0;
      if (checksumError == 0 && channelMean[fee][Channel] > 20. && channelMean[fee][Channel] < 200.)
      {
        for(int adc_sam_no=0;adc_sam_no<nSamples;adc_sam_no++)
        {
          if ((adcSamples[adc_sam_no] - channelMean[fee][Channel] > channelSigma[fee][Channel]*5) && (adcSamples[adc_sam_no] - channelMean[fee][Channel] > adcCut))
          {
            adcMinusPedestal = adcSamples[adc_sam_no] - channelMean[fee][Channel];
            double zPos;
            if (q < 12)
            { //0.4 cm per sample, 50 ns/sample, 8 cm/us drift speed
              zPos = 105. - (105./230.)*((double)adc_sam_no - 10.); // - (*BCO - globalEventBCO)*(8./9.4);
              //if (zPos < 0. || zPos > 105.) continue;
              side = 0;
              phiElement = q;
	    }
            else
            {
              zPos = -105. + (105./230.)*((double)adc_sam_no - 10.); // + (*BCO - globalEventBCO)*(8./9.4);
              //if (zPos > 0. || zPos < 105.) continue;
              side = 1;
              phiElement = q - 12;
            }

            int feeM = FEE_map[fee];
            if(mod_arr[fee]==2) feeM += 6;
            else if(mod_arr[fee]==3) feeM += 14;
            layer = M.getLayer(feeM, Channel);
            pad = M.getPad(feeM, Channel) + (phiElement)*pads_per_sector[mod_arr[fee]-1]; 

	    
	    if(layer!=0)
	      {
		double R = M.getR(feeM, Channel);
		double phi = (sectorNum<12? -M.getPhi(feeM, Channel) : M.getPhi(feeM, Channel) ) + (sectorNum - side*12. )* M_PI / 6. ;
		R /= 10.; //convert mm to cm  
 
		xPos = R*cos(phi);
		yPos = R*sin(phi);
	      }

            x_Pos = xPos;
            y_Pos = yPos;            
            z_Pos = zPos;
            frameVal = frame;
            timeBin = adc_sam_no;
            hitTree->Fill();

            stringstream spts;

            if (firstClus) firstClus = false;
            else spts << ","; 
            
	    spts << "{ \"x\": ";
            spts << x_Pos;
            spts << ", \"y\": ";
            spts << y_Pos;
            spts << ", \"z\": ";
            spts << z_Pos;
            spts << ", \"e\": ";
            spts << adcMinusPedestal;
            spts << "}";

            outdata << (boost::format("%1%") % spts.str());
            spts.clear();
            spts.str("");
          }
        }
      }

    }

    cout <<"sample_count = "<<sample_count<<endl;

    TPCFile->Close();
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
  std::cout << "Done" << std::endl; 
}
