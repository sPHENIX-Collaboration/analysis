
// **************************************************************************


tileTree::tileTree(){
  hLabHelper * hlHelper = hLabHelper::getInstance();


  //  hlHelper->fhcl = new TFile(rootfName,"recreate");
  hlHelper->thcl = new TTree("thcl","Tile Data Tree");
  hlHelper->thcl ->SetMaxTreeSize(1000000000);

  hlHelper->thcl->Branch("runnumber"  ,&hlHelper->runnumber,   "runnumber/I"); 
  hlHelper->thcl->Branch("eventseq",   &hlHelper->eventseq,    "eventseq/I"); 
  //  hlHelper->thcl->Branch("reject",     &hlHelper->rejectCode,  "reject/I"); 

  
  TString rawbranch = "adc["; rawbranch += ACTIVECHANNELS; rawbranch += "]["; rawbranch += NSAMPLES; rawbranch += "]/F";
  hlHelper->thcl -> Branch("adc", hlHelper->adc, rawbranch);  

  //   TString adcbranch = "adc[";
  //   adcbranch += CHTOTAL;
  //   adcbranch += "][";
  //   adcbranch += NSAMPLES;
  //   adcbranch += "]/I";
  //   thcl->Branch("adc",hHelper->adc,adcbranch); 

  //   TString pedestalbranch = "pedestal[";
  //   pedestalbranch += CHTOTAL;
  //   pedestalbranch += "]/F";
  //   thcl->Branch("pedestal",hHelper->pedestal,pedestalbranch); 

  //   TString rawpeakbranch = "rpeak[";
  //   rawpeakbranch += CHTOTAL;
  //   rpeakbranch += "]/F";
  //   thcl->Branch("rpeak",hHelper->rpeak,rawpeakbranch); 

  //   TString rawtimebranch ="rtime[";
  //   rawtimebranch += CHTOTAL;
  //   rawtimebranch += "]/F";
  //   thcl->Branch("rtime",hHelper->rawTime, rawtimebranch);

  //   TString fitpeakbranch = "fpeak[";
  //   calpeakbranch += CHTOTAL;
  //   calpeakbranch += "]/F";
  //   thcl->Branch("fpeak",hHelper->fpeak,fitpeakbranch); 

  //   TString fittimebranch ="ftime[";
  //   fittimebranch += CHTOTAL;
  //   fittimebranch += "]/F";
  //   thcl->Branch("ftime",hHelper->fitTime, fittimebranch);

  //   TString fittimermsbranch ="ftrms[";
  //   fittimermsbranch += CHTOTAL;
  //   fittimermsbranch += "]/F";
  //   thcl->Branch("ftime",hHelper->fitTime, fittimermsbranch);

  //  Pressing time histograms





//   TString triggerbranch = "tag[";
//   triggerbranch += TAGS;
//   triggerbranch +="]/F";
//   W->Branch("tags", wd->tags,triggerbranch); 
//   W->Branch("hits", wd->hits,"hit[4]/F"); 
//   //  clusters found in inner and outer sections
//   TString ihcclbranch = "ihccl[6]/F";
//   W->Branch("ihccl",wd->ihccl,ihcclbranch); 
//   TString ohcclbranch = "ohccl[6]/F";
//   W->Branch("ohccl",wd->ohccl,ohcclbranch); 
//   //  tracks found in inner and outer sections
//   TString itrackbranch = "itrack[";
//   itrackbranch += TRLENGTH;
//   itrackbranch +="]/F";
//   W->Branch("itrack",wd->itrack,itrackbranch); 
//   TString otrackbranch = "otrack[";  
//   otrackbranch += TRLENGTH;
//   otrackbranch +="]/F";
//   W->Branch("otrack",wd->otrack,otrackbranch); 



//   TString ecsumbranch = "ecsum[";
//   ecsumbranch += ECSUMS;
//   ecsumbranch +="]/F";
//   W->Branch("ec_sum",wd->ecadcsum,ecsumbranch); 

//   TString hcsumbranch = "hcsum[";
//   hcsumbranch += HCSUMS;
//   hcsumbranch +="]/F";
//   W->Branch("hcsum",wd->hcadcsum,hcsumbranch); 

//   //  Preshower branch
//   if(psON)  {
//     initPreshowerTree();
//     initPreshowerUnpacker();
//     cout<<"PRESHOWER branch initialized for Run "<<rn<<endl;
//   }

}

//  -----------------------------------------------------------------------------------------------
		       
void tileTree::updateRootFile(){
  // whatever summary histograms what not
  hLabHelper * hlHelper = hLabHelper::getInstance();
  hlHelper->thcl->AutoSave("Overwrite");
  // fhcl -> Close();
  // thcl -> Write();
  // fhcl -> Write();
}
