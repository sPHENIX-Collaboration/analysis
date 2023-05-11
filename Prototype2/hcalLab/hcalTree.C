
// =============================================================================================


hcalTree::hcalTree(){
  hLabHelper * hlHelper = hLabHelper::getInstance();
  hcalHelper * hHelper  = hcalHelper::getInstance();

  //  hlHelper->fhcl = new TFile(hlHelper->rootfName,"recreate");
  hlHelper->thcl = new TTree("thcl","HCal Data Tree");
  cout<<"hcalTree::hcalTree> HCal Data Tree is created in the directory of "<<hlHelper->rootTempFileName<<endl;

  hlHelper->thcl ->SetMaxTreeSize(1000000000);

  hlHelper->thcl->Branch("runnumber"  ,&hHelper->runnumber,   "runnumber/I"); 
  hlHelper->thcl->Branch("eventseq",   &hHelper->eventseq,    "eventseq/I"); 
  hlHelper->thcl->Branch("reject",     &hHelper->eventReject, "reject/I"); 
  hlHelper->thcl->Branch("trigger",    &hHelper->eventTrigger,"trigger/I"); 
  TString rawbranch = "adc["; rawbranch += ACTIVECHANNELS; rawbranch += "]["; rawbranch += NSAMPLES; rawbranch += "]/F";
  hlHelper->thcl->Branch("adc", hHelper->adc, rawbranch);  
  //  from hcal object
  hlHelper->thcl->Branch("amplTotal", &hHelper->t1044->amplTotal, "amplTotal/D"); 
  hlHelper->thcl->Branch("eTotal",    &hHelper->t1044->eTotal,    "eTotal/D"); 
  hlHelper->thcl->Branch("calLCG",    &hHelper->t1044->calLCG,    "calLCG/D"); 
  hlHelper->thcl->Branch("scintLCG",  &hHelper->t1044->scintLCG,  "scintLCG/D"); 

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

  //  HODOSCOPE branch
  //   if(psON)  {
  //     initPreshowerTree();
  //     initPreshowerUnpacker();
  //     cout<<"PRESHOWER branch initialized for Run "<<rn<<endl;
  //   }

}

//  -----------------------------------------------------------------------------------------------
		       
void hcalTree::updateRootFile(){
  // whatever summary histograms what not
  //  hLabHelper * hlHelper = hLabHelper::getInstance();
  //  hlHelper->thcl->AutoSave("Overwrite");
  // fhcl -> Close();
  // thcl -> Write();
  // fhcl -> Write();
  ;
}
