hLabHelper:: hLabHelper(){
  //  whenever it is instanciated manually (not from detector helpers) it sets all dimensions to 
  //  defaults one can change to get access to something in the data
  //  if request for Helper is from one of the project specific helpers then hLabHelper job is 
  //  only to handle input/output related functions. The problem - all my detector specific 
  //  helpers want to create detector with all presentation tools (and there are many) first 
  //  and would like to have all that material available in the root file. Unfortunately it 
  //  can't be created unique before the run number is known (it's a part of its name). 
  //  I want all my presentation material to be in the TFile directory so they will go outside
  //   by default and so I create a temporary storage TFile with "host" dependent name for 
  //  the period the program runs and use system tools to rename that file when program is 
  //  ready for termination.
    runnumber      = 0;
    runKind        = "";     //   default
    eventseq       = 0;
    eventsread     = 0;
    fhcl           = NULL;
    thcl           = NULL;
    rootTempFileName = "";
    host = gSystem->HostName();
    host.Remove(host.First('.'));

    if(ACTIVECHANNELS==0){
      CHTOTAL        = chinspected;
      ACTIVECHANNELS = CHTOTAL;
      detchannels    = CHTOTAL - triggerinsp;
      //  compute total number of HG &  LG channels in detector setup
      hgDetChannels = 0;
      lgDetChannels = 0;
      for(int ich = 0; ich<detchannels; ich++){
	if(feechinsp[ich]%2==HIGH) hgDetChannels ++;  else lgDetChannels ++; 
      }
      cout<<"<hLabHelper:: hLabHelper >  Initializing as primary with "<<ACTIVECHANNELS<<" active channels"<<endl; 
    } else {
      //  detectorHelper will taks care of all its data - it needs only file handling utilities from labHelper
      //  create and open buffer TFile so detector helper will be able to use it to place all its presentation
      //  data in its directory
      cout<<"<hLabHelper:: hLabHelper>  Initializing as secondary"; 
      rootTempFileName = (host.Contains("rcas"))?   RCFrootDir : HLABrootDir; 
      TDatime T;
      TString fN = "hcalHelper_"; fN += T.GetDate(); fN += "_"; fN += T.GetTime(); fN += ".root";
      rootTempFileName += fN;
      cout<<"<hLabHelper:: hLabHelper> ROOT graphic objects will be written into TFile "<<rootTempFileName<<endl;
      fhcl = new TFile(rootTempFileName, "recreate");
      FileStat_t buf;
      if(gSystem->GetPathInfo(rootTempFileName, buf)) {
	cout<<"<runToRootFile> : File  "<<rootTempFileName<<"  still does not exist"<<endl;
      } else {cout<<"<hLabHelper:: hLabHelper> ROOT file "<<rootTempFileName<<"  created"<<endl;}
      fhcl->cd();
      return;   
    }    
    active     = new Int_t      [ACTIVECHANNELS];
    adc        = new Float_t  * [ACTIVECHANNELS];
    fitResults = new Double_t * [ACTIVECHANNELS];
    for (int ich=0; ich<ACTIVECHANNELS; ich++) {
      adc[ich]        = new Float_t  [NSAMPLES];
      fitResults[ich] = new Double_t [NPARAMETERS];
    }
    pedestal   = new Float_t  [ACTIVECHANNELS];   //     [ACTIVECHANNELS] */
    rawPeak    = new Float_t  [ACTIVECHANNELS];
    rawTime    = new Float_t  [ACTIVECHANNELS];
    fitPeak    = new Float_t  [ACTIVECHANNELS];
    fitTime    = new Float_t  [ACTIVECHANNELS];
    fitInt     = new Float_t  [ACTIVECHANNELS];
    fitChi2    = new Float_t  [ACTIVECHANNELS];
    fitPeakRMS = new Float_t  [ACTIVECHANNELS];
    fitTimeRMS = new Float_t  [ACTIVECHANNELS];
    rdata        = new TH2F("rdata", "Raw Peak Values for all active channels",ACTIVECHANNELS, 0, ACTIVECHANNELS, 1024, -2048., 2048.);
    shapes       = new TF1 *  [ACTIVECHANNELS];
    sigFit       = new TF1 *  [ACTIVECHANNELS];
    rvsc         = new TF1 *  [ACTIVECHANNELS];
    fmsc         = new TF1 *  [ACTIVECHANNELS];
    gpulse       = new TGraph * [ACTIVECHANNELS];
    ft           = new TH1 *  [ACTIVECHANNELS];
    fm           = new TH1 *  [ACTIVECHANNELS];
    fw           = new TH1 *  [ACTIVECHANNELS];
    fint         = new TH1 *  [ACTIVECHANNELS];
    fpd          = new TH1 *  [ACTIVECHANNELS];
    fchi2        = new TH1 *  [ACTIVECHANNELS];
    fitPar       = new TH1 ** [ACTIVECHANNELS];
    rpeak        = new TH1 *  [ACTIVECHANNELS];
    rtm          = new TH1 *  [ACTIVECHANNELS];
    for (int ich = 0; ich < ACTIVECHANNELS; ich++) {
      fitPar[ich] = new TH1 * [NPARAMETERS];
      shapes[ich] = NULL;
      gpulse[ich] = NULL;
    }
    for (int ig = 0; ig<2; ig++){
      evtadcsum[ig]  = new Double_t  [NSAMPLES];
      evtGraph[ig]   = new TGraph;
      TString sum    = "adcsum_"; sum += (ig==HIGH)? "HG"  :  "LG";
      TString sig    = "signal_"; sig += (ig==HIGH)? "HG"  :  "LG";
      evtShape[ig]   = new TF1(sum, &signalShape, 0., (Double_t)NSAMPLES, NPARAMETERS);
      evtSignal[ig]  = new TF1(sig, &signalShape, 0., (Double_t)NSAMPLES, NPARAMETERS);
    //    Double_t    times[ACTIVECHANNELS];
    //    Double_t    chi2 [ACTIVECHANNELS];
      evtfitpar[ig]  = new Double_t [NPARAMETERS];
    }
  }
// **************************************************************************

bool hLabHelper::evDisplay(Int_t rn){
  if(rn<=0)    return false;
  runnumber    = rn;
  if(!runToPRDFFile(rn)) {
    cout<<"<setPRDFRun>  Failed to find the .prdf file for run number "<<rn<<endl;
    return false;
  }
  // Open up PRDFF
  int status;
  Eventiterator *it =  new fileEventiterator(prdfName, status);
  if (status)
  {
    cout << "Couldn't open input file " << prdfName << endl;
    delete(it);
    return false;
  }
  //  update channel map
  updateMap();
  int chperpanel = DISPLAYX*DISPLAYX;
  int panels     = (ACTIVECHANNELS%chperpanel==0?  ACTIVECHANNELS/chperpanel : ACTIVECHANNELS/chperpanel+1); 
  int nx_c       = DISPLAYX;
  int ny_c(0);
  if(panels>1||ACTIVECHANNELS%chperpanel==0) {
    ny_c       = DISPLAYX;
  } else {
    ny_c       = ACTIVECHANNELS/DISPLAYX + (ACTIVECHANNELS%DISPLAYX==0? 0 : 1);
  }
  
  //  cout<<"Display "<<panels<<"  "<<nx_c<<"/"<<ny_c<<endl;
  
  gStyle->SetOptFit(111);
  gStyle->SetFitFormat("5.2g");
  //  creating Canvases
  TCanvas * evdisplay[panels];
  TString cnvsName  = "EvDisplay_";
  TString cnvsTitle = "sPhenix Channel display  for CHANNELS ";
  for (int iC = 0; iC < panels; iC++) {
    TString cN  = cnvsName;
    cN          += iC;
    TString cT  = cnvsTitle;
    cT          += (chperpanel*iC);
    cT  += " - ";
    cT  += (chperpanel*(iC+1)-1);
    evdisplay[iC]  = new TCanvas(cN, cT, 300*nx_c, 150*ny_c);
    evdisplay[iC]->SetEditable(kTRUE);
    evdisplay[iC]->Divide(nx_c,ny_c);
    //    if(iC==panels) {
    //    evdisplay[iC]->SetBit(TPad::kClearAfterCR);
    //    }
  }
  //   ====================================================================================
  //  now TGraph for every channel
  for (int ich = 0; ich<ACTIVECHANNELS;         ich++)
    {
      gpulse[ich] = new TGraph(NSAMPLES);
      TString name = "gch"; name += ich;
      gpulse[ich]->SetName(name);
      name = "Channel  "; name += ich; name += " / "; name += feechinsp[ich];  name += (feechinsp[ich]%2==HIGH? "  HG" : "  LG"); 
      gpulse[ich]->SetTitle(name);
      gpulse[ich]->SetMarkerStyle(20);
      gpulse[ich]->SetMarkerSize(0.4);
      //gpulse[ich]->SetMarkerColor(ich+1);
      //gpulse[ich]->SetLineColor(ich+1);
    }
  Event *evt;

  // Loop over events
  while ( (evt = it->getNextEvent()) != 0 )
    {
      eventseq = evt->getEvtSequence();

      if ( evt->getEvtType() != 1 ) {
	cout << "eventseq " << eventseq << " event type = " << evt->getEvtType() <<"  ignored "<< endl;
      } else {
	// Get sPHENIX Packet
	Packet_hbd_fpgashort *spacket = dynamic_cast<Packet_hbd_fpgashort*>( evt->getPacket(21101) );
	if ( spacket ) {
	  spacket->setNumSamples( NSAMPLES );
	  cout <<"PRDF FILE "<<prdfName<<"  EVENT "<<eventseq <<endl;
	  for (int ich=0; ich<ACTIVECHANNELS; ich++) {
	    for (int is=0; is<NSAMPLES; is++) {
	      adc[ich][is] = spacket->iValue(active[ich],is);
	      //	      cout<<"<evDisplay  Event "<< eventseq<<"  ich/is/adc  "<<ich<<" - "<<is<<" - "<<adc[ich][is]<<endl;	    
	    }
	  }
	  dofixes();
	  getDetectorTiming();
	  //	  cout<<"<TileFit>  Peak  "<< evtsum.evtpeak<<"  Time "<<evtsum.evttime<<"  Chi2 "<<evtsum.tChi2 <<endl;
	  displaySumEvent();
	  //  we do generic display of square TCanvases DISPLAYX x DISPLAYX size
	  //	  for (int evD = 0; evD<panels; evD++){
	  //	    TString evDName  = cnvsName  + evD;
	  //	    TString evDTitle = cnvsTitle + evD; 
	  //	    evdisplay->SetBit(TPad::kClearAfterCR);
	  //	    evdisplay->Divide(nx_c,ny_c);

	    for (int ich=0; ich<ACTIVECHANNELS; ich++) {
	      for (int is=0; is<NSAMPLES; is++)  gpulse[ich]->SetPoint(is,(Double_t)is, adc[ich][is]);
	      int iC   = ich/chperpanel;
	      int iPad = (ich-chperpanel*iC);
	      int iPx  = iPad%nx_c;
	      int iPy  = ny_c - iPad/nx_c - 1;
	      iPad     = iPy*nx_c + iPx + 1;
	      evdisplay[iC] ->cd(iPad);
	      //   cout<<iC<< "  "<<ich<<"   "<<iPx<<"  "<<iPy<<"  "<<iPad<<endl;
	      gpulse[ich]->Draw("acp");
	      // Double_t peakVal;
	      // Int_t    peakPos;
	      // fitShape(ich, peakVal, peakPos, (ich<detchannels? 1 : 0));
	      // Double_t fMax  = shapes[ich]->GetMaximum(par0Min[1], par0Max[1]);
	      // Double_t fPos  = shapes[ich]->GetMaximumX(par0Min[1], par0Max[1]);
	      // Double_t fChi2 =  shapes[ich]->GetChisquare()/shapes[ich]->GetNDF();
	      // cout<<"EVENT  "<<eventseq<<" SHAPE  "<<(char*)(shapes[ich]->GetName())<<"  peakVal  "<<peakVal<<"  peakPos  "<<peakPos<<"  fitMax  "<<fMax<<"  fitPos  "<<fPos<<"   fChi2  "<<fChi2<<endl;
	      // shapes[ich]->Draw("same");
	      //	      evdisplay[iC]->Update();
	    }
	    for (int iC = 0; iC < panels; iC++) evdisplay[iC]->Update();
	    char dummy;
	    cin>>dummy;
	    //	  }
	  delete spacket;
	  //	  for (int iC = 0; iC < panels; iC++) evdisplay[iC]->Clear();
	} else cout<<"<evDisplay> Packet hbd not found. EventSeq "<<eventseq<<" EventType "<< evt->getEvtType()<<endl;
      }
      delete evt;
    }
  cout<<"<evDisplay> EoF reached "<<endl;
  
  return true;
}

// **************************************************************************
void hLabHelper::updateMap(){
  for (int ich = 0; ich<ACTIVECHANNELS;   ich++) active[ich] = feechinsp[ich]  ;
}

// **************************************************************************
void hLabHelper::setRunKind(TString kind){
  runKind = kind;
  cout<<"hLabHelper::setRunKind> will look for run data in "<<runKind<<endl;
}



// **************************************************************************
void hLabHelper::getFileLists(){
  //  cout<<"<getFileLists>  hostname  "<<host<<endl;
  //   ====================================================================================
  //  parsing .root file name from runnumber
  rootdirname  = (host.Contains("rcas"))?   RCFrootDir : HLABrootDir;  
  prdfdirname  = (host.Contains("rcas"))?   RCFdataDir : HLABdataDir; 
  //  in t1044 we have "junk", "cosmics", "led"  and "beam" kinds of events 
  cout<<"<hLabHelper::getFileLists> "<<rootdirname<<endl;
  cout<<"<hLabHelper::getFileLists> "<<prdfdirname<<endl;
  if(!runKind.IsNull()) {
    prdfdirname += runKind; prdfdirname +="/";
    cout<<"hLabHelper::getFileLists> PRDF Directory name updated "<<prdfdirname<<endl;
  }
  TSystemDirectory rdir(rootdirname, rootdirname);
  TSystemDirectory pdir(prdfdirname, prdfdirname);
  TList * rootfiles = rdir.GetListOfFiles();
  TList * prdffiles = pdir.GetListOfFiles();
  Int_t   rn;
  TString rid;
  if(rootfiles){
    TSystemFile * file;
    TString       fname;
    TIter         next(rootfiles);
    while((file = (TSystemFile *) next())) {
      fname = file->GetName();
      if(!file->IsDirectory() && fname.EndsWith(".root")) {
	//  .root file found - get run number
	decoderun(fname, rn, rid);
	TString name = rootdirname;
	name += fname;
	rootfile rf(name, rn, rid);
	roots.push_back(rf);
	//	cout<<".root entry "<<name<<endl;
      } else {
	//	cout<<"<getFileList>  candidate root  "<<fname<<"  ignored "<<endl;
	
      }
    }
  }
  if(prdffiles){
    TSystemFile * file;
    TString       fname;
    TIter         next(prdffiles);
    while((file = (TSystemFile *) next())) {
      fname = file->GetName();
      //      cout<<fname<<endl;
      if(!file->IsDirectory() && fname.EndsWith(".prdf")) {
	//  .prdf file found - get run number
	decoderun(fname, rn, rid);
	TString name = prdfdirname;
	name += fname;
	prdffile rf(name, rn, rid);
	prdfs.push_back(rf);
	//	cout<<".prdf entry "<<name<<endl;
      } else {
	//	cout<<"<getFileList>  candidate prdf  "<<fname<<"  ignored "<<endl;
	
      }
    }
    
  }
  cout<<"<getFileLists>  .root files found "<<roots.size()<<"   .prdf files found "<<prdfs.size()<<endl;
}

// **************************************************************************
void hLabHelper::decoderun(TString & fname, Int_t & rn, TString & rid){
  if(fname.Contains(".root") || fname.Contains(".prdf")) {
    //    cout<<"<decoderun> : HCAL file "<<fname<<"  confirmed"<<endl;
    rid          = fname;   // get run number from root file name
    rid.ReplaceAll(".root",""); rid.ReplaceAll(".prdf","");
    Ssiz_t lastslash = rid.Last('/');
    rid.Remove(0,lastslash+1);
    rid.ReplaceAll("HClab_","");   rid.ReplaceAll("rc-","");  rid.ReplaceAll("cosmics_",""); rid.ReplaceAll("beam_","");

    TString id       = rid;
    id.Remove(rid.Last('-'));
    rn               = id.Atoi();
    //    cout<<"<decoderun> "<<fname<<"  runnumber "<<rn<<"  runId "<<rid<<endl;
  }
}

// **************************************************************************
TFile * hLabHelper::attachrootrun(Int_t rn){
  if(!roots.size()) {
    cout<<"<attachrootrun>  No .root files in the list: Collecting "<<endl;
    getFileLists();
  } 
  //  find the rootfile rn in the local list of files kept by Helper
  std::list<rootfile>::iterator it = roots.begin();

  while (it!=roots.end())  {
    //    it->print();
    if(it->runnumber==rn) {
      runnumber    = rn;
      runId        = it->runid;
      cout<<"<hLabHelper::attachrootrun> ROOT file with "<<runId<<" identifier found: "<<it->name<<endl;
      // this is dangerous 
      return new TFile(it->name);    
    } else it++;
  }
  cout<<"<hLabHelper::attachrootrun> Requested ROOT file for Run "<<rn<<" not found"<<endl;
  return NULL;
  // auto it = std::find_if( std::begin( roots ),
  // 			  std::end(   roots ),
  //                           [&]( const rootfile rf ){ return 1 == ( rf.runnumber==rn ); } );
  // if(roots.end() == it)  return NULL;
  //  return new TFile(it->name); 
}



//         const int pos = std::distance( myList.begin(), it ) + 1;
//         std::cout << "item is found at position " << pos << std::endl;


// **************************************************************************
Bool_t hLabHelper::setPRDFRun(int run, Bool_t bookH){
  if(run<=0)   return false;
  runnumber    = run;
  if(!runToPRDFFile(run)) {
    cout<<"<setPRDFRun>  Failed to find the .prdf file for run number "<<run<<endl;
    return false;
  }
  initPRDFRun(bookH);
  return true;
}



// **************************************************************************
Bool_t hLabHelper::setROOTRun(int run){
  if(!runToRootFile(run)) {
    cout<<"<setROOTRun>  Failed to find the .root file for run number "<<run<<endl;
    return false;
  }
  return true;
}


// **************************************************************************

// The assumption hear is that the .root file is either already attached to this ROOT session or 
// must be looked for using its runnumber
Int_t hLabHelper:: runToRootFile(Int_t run){
  if(run<=0) {
    cout<<"<runToRootFile> WARNING  :  Requested run number = "<<run<<endl;
    TSeqCollection * sq = gROOT->GetListOfFiles();
    if(!sq->IsEmpty()) {
      //  there are already attached files - let's see if we may find something over there
      //  now it is really .rootf into runId not what is in the name
      TString current = ((TObject *)(sq->First()))->GetName();
      //  check the extension first
      if(current.Contains(".root")) {
	cout<<"<hLabHelper::runToRootFile> :  attached .root file "<<current<<"  found"<<endl;
	rootfName   = current;
	runId       = current;   // get run number from root file name
	runId.ReplaceAll(".root","");
	Ssiz_t lastslash = runId.Last('/');
	runId.Remove(0,lastslash+1);
	runId.ReplaceAll("HClab_","");
	TString rn       = runId;
	rn.Remove(rn.Last('-'));
	runnumber        = rn.Atoi();
	return(runnumber);
      }
    } else    return 0;
  } else {
    //  TODO:  check if TFile for requested run is already attached
    //  let's try to find and attach file with the right runnumber
    //  find where I am
    cout<<"<runToRootFile> :  looking into .root directory for runnumber "<<run<<endl;
    //   ====================================================================================
    //  parsing .root file name from runnumber
    rootfName  = (host.Contains("rcas"))?   RCFrootDir : HLABrootDir;
    TString fname = rootfName;
    FileStat_t buf;
    runId = "00"; runId += run;  runId += "-0";
    fname += "HClab_";  fname += runId;  fname += ".root";
    if(gSystem->GetPathInfo(fname, buf)) {
      fname = rootfName;
      runId = "0"; runId += run;  runId += "-0";
      fname += "HClab_";  fname += runId;  fname += ".root";
      rootfName  = fname;
      if(gSystem->GetPathInfo(fname, buf)) {
	cout<<"<runToRootFile> : File  "<<fname<<"  does not exist"<<endl;
      } else  cout<<"<runToRootFile> : File  "<<fname<<"  already  exist and will be recreated"<<endl;
    } 
    runnumber  = run;
  }
  if(!rootTempFileName.IsNull()) {
    cout<<"<hLabHelper:: runToRootFile> found temporaly root file "<<rootTempFileName<<endl; } else {
    fhcl = new TFile(rootfName,"recreate");
    cout<<"hLabHelper:: runToRootFile> "<<rootfName<<"  opened"<<endl; 
  }
  return runnumber;
}

// **************************************************************************

Int_t  hLabHelper::runToPRDFFile(Int_t run){
  //   ====================================================================================
  //  find where I am
  //   ====================================================================================
  //  parsing .prdf file name from runnumber
  prdfName  = (host.Contains("rcas"))?   RCFdataDir : HLABdataDir;
  if(!runKind.IsNull()) {
    prdfName += runKind; prdfName += "/";
    cout<<"hLabHelper::runToPRDFFile> PRDF Directory name updated "<<prdfName<<endl;
  }
  TString fname = prdfName;
  FileStat_t buf;
  // runId = "00"; runId += run;  runId += "-0";
  // fname += "rc-";  fname += runId;  fname += ".prdf";
  runId = "0000"; runId += run;  runId += "-0000";
  fname += runKind; fname += "_";
  fname += runId;  fname += ".prdf";
  if(gSystem->GetPathInfo(fname, buf)) {
    fname = prdfName;
    // runId = "00"; runId += run;  runId += "-0";
    // fname += "rc-";  fname += runId;  fname += ".prdf";
    runId = "0000"; runId += runnumber;  runId += "-0000";
    fname += runKind;   fname += "_"; 
    fname += runId;  fname += ".prdf";
    if(gSystem->GetPathInfo(fname, buf)) {
      cout<<"<runToPRDFFile> File  "<<fname<<"  with runId  "<<runId<<"  does not exist"<<endl;
      return 0;
    }     
  }
  prdfName  = fname;
  //   ====================================================================================
  cout<<"<runToPRDFFile> Parsed prdf file name  "<<prdfName<<endl;
  //   ====================================================================================

  cout << "<runToPRDFFile>  prdfName "<<prdfName << "  RunId  "<<runId<<"  Run Number  "<<runnumber<<endl;

  //   ====================================================================================
  return  runnumber;
}


// **************************************************************************
void hLabHelper::initPRDFRun(Bool_t bookH){
  runToRootFile(runnumber);
  //   ====================================================================================
  // initialize root file and TTree (the idea is to save all data and histos at the end of run)
  if(runnumber<=0) {
    cout<<"<hLabHelper::initPRDFRun> Cant instantiate run with runnumber = 0.  Exiting"<<endl;
    return;
  }
  //   ====================================================================================
  //  TApplication theApp("theApp",&argc,argv);

  gROOT->SetStyle("Plain");
  /*
    gStyle->SetLabelSize(0.1,"xyz");
    gStyle->SetTitleSize(0.08,"xyz");
    gStyle->SetStatH(0.4);
  */
  if(!bookH) return;
  //  fit parameters (for templating)
  for (int ich = 0; ich<ACTIVECHANNELS; ich ++){    
    TString fhn  = "ft_"; fhn += ich; TString fht = "Fitted pulse peak time CH="; fht += ich;
    ft[ich]    = new TH1F(fhn, fht,   100, 0., NSAMPLES);
    fhn        = "fm_"; fhn += ich; fht = "Fitted pulse amplitude CH="; fht += ich;
    fm[ich]    = new TH1F(fhn, fht,   4100, 0.,    4100.);
    fhn        = "fw_"; fhn += ich; fht = "Fitted pulse width CH="; fht += ich;
    fw[ich]    = new TH1F(fhn, fht,   100, 0.,    5.);
    fhn        = "fpd_"; fhn += ich; fht = "Fitted pedestal at t=PEAK CH="; fht += ich;
    fpd[ich]   = new TH1F(fhn, fht,   200, 2000., 2100.);
    fhn        = "fint_"; fhn += ich; fht = "Integral of fitted curve (pedestal suppressed) CH="; fht += ich;
    fint[ich]  = new TH1F(fhn, fht,   4000, 0., 4000.);
    fhn        = "fchi2_";fhn += ich; fht = "Chi2 of fit  CH="; fht += ich;
    fchi2[ich] = new TH1F(fhn, fht,   500, 0., 100.);

    for(int ip=0; ip<NPARAMETERS; ip++){      TString phn = "p_"; phn += ip; phn += "_"; phn += ich;
      TString pht = "Fit Parameter "; pht += ip; pht += " Channel "; pht += ich;
      fitPar[ich][ip] = new TH1F(phn, pht, 100, par0Min[ip], par0Max[ip]);
    }

    TString rvn = "rv_"; rvn += ich; TString rvt = "Raw peak value CH = "; rvt += ich;
    rpeak[ich] = new TH1F(rvn, rvt, 4100, 0., 4100.);
    TString rtn = "rt_"; rtn += ich; TString rtt = "Raw peak time CH = "; rtt += ich;
    rtm[ich]   = new TH1F(rtn, rtt, NSAMPLES, 0., NSAMPLES);
  }
  for (int ich=0; ich<ACTIVECHANNELS; ich++) {
    TString fn = "S_"; fn += ich;
    sigFit[ich] =  new TF1(fn, &signalShape, 0., (Double_t)NSAMPLES, 6);
    //    sigFit[ich]->AbsValue(true);
  }  //  This version of plotting is for High/Low gain preamplifier sending its outputs into two sequential readout channels
  //  int nx_c = 2;             
  //  int ny_c = ACTIVECHANNELS/2+ACTIVECHANNELS%2;
  //  ev_display = new TCanvas("sphenix","sphenix display",400*nx_c,200*ny_c);

  //  fitFunc   = new TCanvas("fitFunc","Fit Functions",  400*nx_c,200*ny_c);
  //  ev_display->SetEditable(kTRUE);
  //  ev_display->Divide(nx_c,ny_c);
  //  ev_display->SetBit(TPad::kClearAfterCR);
  //  fitFunc->SetEditable(kTRUE);
  //  fitFunc->Divide(nx_c,ny_c);

  TString name, title;
  for (int ich=0; ich<ACTIVECHANNELS; ich++)
    {
      gpulse[ich] = new TGraph(NSAMPLES);
      name = "gch"; name += ich;
      gpulse[ich]->SetName(name);
      gpulse[ich]->SetMarkerStyle(20);
      gpulse[ich]->SetMarkerSize(0.4);
      //gpulse[ich]->SetMarkerColor(ich+1);
      //gpulse[ich]->SetLineColor(ich+1);
    }
  
}

//  -----------------------------------------------------------------------------------------------
void hLabHelper::renameAndCloseRF(){
  if(rootTempFileName.IsNull()) return;
  fhcl -> Close();
  gSystem->Rename(rootTempFileName, rootfName);
  cout<<"<hLabHelper::copyAndCloseRootFile> Renamed  "<<endl;
  cout<<"           "<<rootTempFileName<<endl;
  cout<<"into  "<<endl;
  cout<<"           "<<rootfName<<endl;
}


//  -----------------------------------------------------------------------------------------------
void hLabHelper::makeCanvasDirectory(){
  rootdirname  = (host.Contains("rcas"))?   RCFrootDir : HLABrootDir;  
  cDirectory = rootdirname; cDirectory += "canvases/r";
  cDirectory += runnumber;  cDirectory += "/";
  FileStat_t buf;
  if(!gSystem->GetPathInfo(cDirectory, buf)) {
    cout<<"<hLabHelper::makeCanvasDirectory>   Directory "<<cDirectory<<" already exists "<<endl;
  } else {
    //    gSystem->MakeDirectory(cDirectory);
    TString command = "mkdir "; command += cDirectory;
    gSystem->Exec(command);
    cout<<"<hLabHelper::makeCanvasDirectory>  Directory "<<cDirectory<<" created "<<endl;
    
  }
}
//  -----------------------------------------------------------------------------------------------

void hLabHelper::fitShape(int ich, Double_t & rVal, Int_t & rTime, Int_t mode){
  if(!shapes[ich]){
    TString fitName  = active[ich]%2? "HG_" : "LG_";
    fitName += ich/2;
    fitName += "(";
    fitName += ich;
    fitName += ")";
    shapes[ich] = new TF1(fitName, &signalShape, 0., (Double_t)NSAMPLES, 6);
  }
  //  GAIN range
  Int_t ig = feechinsp[ich]%2==HIGH? HIGH  :  LOW; 
  //  zero approximation to fit parameters
  //  use pulse data to find location of maximum
  //  cout<<"<fitShape> Event "<<hclt->eventseq<<" channel "<<ich<<endl;
  rTime = 0;
  rVal  = 0.;
  Int_t iss(RISETIME-2), ise(NSAMPLES-FALLTIME); 
  Int_t rMaxVal  = adc[ich][iss]; Int_t maxsmpl = iss; 
  Int_t rMinVal  = adc[ich][iss]; Int_t minsmpl = iss;

  for (int is = iss; is<=ise; is++) {
    if(adc[ich][is]>rMaxVal) {
      rMaxVal  = adc[ich][is];
      maxsmpl  = is;   
    }
    if(adc[ich][is]<rMinVal) {
      rMinVal  = adc[ich][is];
      minsmpl  = is;   
    }
    //    cout<<ich<<"  "<<is<<"  "<<adc[ich][is]<<"  "<<rVal<<"  "<<rTime<<endl;
  }
  rMinVal  -= PEDESTAL;   rMaxVal -= PEDESTAL;
  if(abs(rMinVal) > abs(rMaxVal)) {
    rVal = rMinVal;  rTime = minsmpl;
  } else {
    rVal = rMaxVal;  rTime = maxsmpl;
  }
     
  //  cout<<"  rVal-ped  "<<rVal<<endl;

  par0[0] = rVal/3.;
  if(mode == 0 ) {
    par0[1] = max(0.,(Double_t)(rTime-RISETIME));
    par0[2] = 4.;
    par0[3] = 1.6;
    par0[4] = PEDESTAL;
  //   we do not do pedestal subtrastion at this time
    par0[5] = 0.;      //   slope of the pedestals is initially set to "0"
    shapes[ich]->SetParameters(par0);
    for(int parn=0; parn<6; parn++) shapes[ich]->SetParLimits(parn, par0Min[parn], par0Max[parn]);
  } else {
    //  channels which belong to detector
    par0[1] = evtfitpar[ig][1];
    par0[2] = evtfitpar[ig][2];
    par0[3] = evtfitpar[ig][3];
    par0[4] = PEDESTAL;
    par0[5] = 0.;     
    shapes[ich]->SetParameters(par0);
    shapes[ich]->SetParLimits(2, evtfitpar[ig][2], evtfitpar[ig][2]);
    shapes[ich]->SetParLimits(3, evtfitpar[ig][3], evtfitpar[ig][3]);
  }
  //  fitFunc->cd(ich+1);  //  shapes[ich]->Draw();
  //  gPad->Modified();
  gpulse[ich]->Fit(shapes[ich], "RQ", "NQ", 0., (Double_t)NSAMPLES);
  shapes[ich]->GetParameters(fitResults[ich]);
  //  ev_display->cd(ich+1);
  //  shapes[hHelper->ich]->Draw("same");

}


// **************************************************************************
//   The next few functions are to process data from HCal Lab recorded via HBD electronics

void hLabHelper::collect(Bool_t fitshape){
  //  Fit parameters

  //  Double_t uSum(0.), cSum(0.), luSum(0.), lcSum(0.), ruSum(0.), rcSum(0.);
  //  Double_t YU(0.), YC(0.),  XU(0.), XC(0.);
  //  hits above threshold in this event depending on the threshold (binned in Chi units - 8 counts)
  Double_t  peakVal;
  Int_t     peakPos;
  for (int ich=0; ich<ACTIVECHANNELS; ich++) {
    for (int is=0; is<NSAMPLES; is++){
      gpulse[ich]->SetPoint(is,(Double_t)is, adc[ich][is]);
    }
    if(fitshape) fitShape(ich, peakVal, peakPos, (ich<detchannels? 1 : 0));
    // fitShape(ich, peakVal, peakPos, 1);
  }

  for (int ich=0; ich<ACTIVECHANNELS; ich++){
    int      rTime = 0;
    Double_t rVal  = 0.;
    //    Int_t iss(RISETIME-2), ise(NSAMPLES-FALLTIME); 
    Int_t iss(0), ise(NSAMPLES-1); 
    Int_t rMaxVal  = adc[ich][iss]; Int_t maxsmpl = iss; 
    Int_t rMinVal  = adc[ich][iss]; Int_t minsmpl = iss;

    for (int is = iss; is<=ise; is++) {
      if(adc[ich][is]>rMaxVal) {
	rMaxVal  = adc[ich][is];
	maxsmpl  = is;   
      }
      if(adc[ich][is]<rMinVal) {
	rMinVal  = adc[ich][is];
	minsmpl  = is;   
      }
    }
    rMinVal  -= PEDESTAL;   rMaxVal -= PEDESTAL;
    if(abs(rMinVal) > abs(rMaxVal)) {
      rVal = rMinVal;  rTime = minsmpl;
    } else {
      rVal = rMaxVal;  rTime = maxsmpl;
    }
    //   cout<<"<collect>  "<<ich<<"  "<<rTime<<"  "<<adc[ich][(int)rTime]<<endl;
    rawTime[ich] = rTime;
    rawPeak[ich] = rVal;
    rawPeak[ich] -= (fitshape? (fitResults[ich][4]+fitResults[ich][5]*rTime) : PEDESTAL);   //  
    rpeak[ich]  -> Fill(rawPeak[ich]);    rtm[ich]  -> Fill(rawTime[ich]);
    rdata       -> Fill(ich, rawPeak[ich]);
    //  Below only in case if fitshape is required
    if(fitshape){
      sigFit[ich]->SetParameters(fitResults[ich]);
      sigFit[ich]->SetParameter(4, 0.);
      sigFit[ich]->SetParameter(5, 0.);

      //      Double_t pVal  = sigFit[ich]->GetMaximum(par0Min[1], par0Max[1]);
      //      Double_t pTime = sigFit[ich]->GetMaximumX(par0Min[1], par0Max[1]);
      Double_t sgp  =  sigFit[ich]->GetMaximum(par0Min[1], par0Max[1]);
      Double_t tmp  =  sigFit[ich]->GetMaximumX(par0Min[1], par0Max[1]);
      Double_t sgn  =  sigFit[ich]->GetMinimum(par0Min[1], par0Max[1]);
      Double_t tmn  =  sigFit[ich]->GetMinimumX(par0Min[1], par0Max[1]);
      Double_t pVal    = rVal;
      Double_t pTime   = rTime;
      if(tmp!=0&&abs(sgp)>abs(sgn)) {
	pVal   = sgp;
	pTime  = tmp;
	//	cout<<"<getDetectorTimingT>: Positive peak selected"<<endl;
      } else {
	pVal   = sgn;
	pTime  = tmn;
	//	cout<<"<getDetectorTimingT>: Negative peak selected"<<endl;
      }
      Double_t xmin    = max(0.,pTime-RISETIME);
      Double_t xmax    = min(Double_t(NSAMPLES), pTime+FALLTIME);
      Double_t pInt    = (pVal!=0?  sigFit[ich]->Integral(xmin, xmax)  : 0);
      // if(pInt==0) {
      //   cout<<"<collect> Integral zero Event "<<hclt->eventseq<<"  Ch "<<ich<<" pVal/pTime/xmin/xmax "<<pVal<<"  "<<pTime<<"  "<<xmin<<"  "<<xmax<<endl;
      // }
      //    pVal          -= fped;
      fitPeak[ich]   = pVal;
      fitTime[ich]   = pTime;
      fitInt[ich]    = pInt;
      fitChi2[ich]   = shapes[ich]->GetChisquare()/shapes[ich]->GetNDF();
    //  trigger contribution
      ft[ich]      -> Fill(pTime);
      fm[ich]      -> Fill(pVal);
    //    Double_t mean = sigFit[ich]->Mean(xmin, xmax);
      Double_t mom2 = (pVal!=0?  sigFit[ich]->CentralMoment(2, xmin, xmax)  : 0.);
      // if(mom2==0) {
      //   cout<<"<collect> CentralMoment zero Event "<<hclt->eventseq<<"  Ch "<<ich<<" pVal/pTime/pInt/xmin/xmax "<<pVal<<"  "<<pTime<<"  "<<pInt<<"  "<<xmin<<"  "<<xmax<<endl;
      // }
      Double_t rms  = sqrt(mom2);
      fw[ich]      -> Fill(rms);
      fpd[ich]     -> Fill(fitResults[ich][4]+pTime*fitResults[ich][5]);
      fint[ich]    -> Fill(pInt);
      fchi2[ich]   -> Fill(fitChi2[ich]);

      for (int ip=0; ip<NPARAMETERS; ip++)  fitPar[ich][ip]->Fill(fitResults[ich][ip]);    
      //    Double_t fVal = GetMaximum( par0Min[2], par0Max[2]);
    }
  }

  if(!reject()){
    //  few histogramms for clean sample
  };
}

// **************************************************************************

void hLabHelper::getDetectorTiming(){
  //  build eventsum (adc sum without pedestal subtruction)
  //  do fitting etc (HG/LG separately)
  
  for(int is = 0; is < NSAMPLES; is++){
    evtadcsum[0][is] = 0.;
    evtadcsum[1][is] = 0.;
    for (int ich = 0; ich < detchannels; ich++) {
      Int_t ig = feechinsp[ich]%2==HIGH? HIGH  :  LOW; 
      evtadcsum[ig][is] += adc[ich][is];
    }
  }
  for (int is = 0; is < NSAMPLES; is++) {
    evtGraph[HIGH]->SetPoint(is,(Double_t)is,evtadcsum[HIGH][is]);
    evtGraph[LOW] ->SetPoint(is,(Double_t)is,evtadcsum[LOW][is]);
  }
  Double_t rVal, rTime;
  for(int ig = 0; ig<2; ig++) {
    Int_t iss(RISETIME-2), ise(NSAMPLES-FALLTIME); 
    Int_t rMaxVal  = evtadcsum[ig][iss]; Int_t maxsmpl = iss; 
    Int_t rMinVal  = evtadcsum[ig][iss]; Int_t minsmpl = iss;

    for (int is = iss; is<=ise; is++) {
      if(evtadcsum[ig][is]>rMaxVal) {
	rMaxVal  = evtadcsum[ig][is];
	maxsmpl  = is;   
      }
      if(evtadcsum[ig][is]<rMinVal) {
	rMinVal  = evtadcsum[ig][is];
	minsmpl  = is;   
      }
      //    cout<<ich<<"  "<<is<<"  "<<adc[ich][is]<<"  "<<rVal<<"  "<<rTime<<endl;
    }
    cout<<"getDetectorTiming:  Event "<<eventseq<<"  Gain "<<ig<<"  Min "<<minsmpl<<" / "<<rMinVal<<"  Max "<<maxsmpl<<" / "<<rMaxVal<<endl;
    rMinVal  -= (ig==HIGH?  PEDESTAL*hgDetChannels   :  PEDESTAL*lgDetChannels);;
    rMaxVal  -= (ig==HIGH?  PEDESTAL*hgDetChannels   :  PEDESTAL*lgDetChannels);;
    if(abs(rMinVal) > abs(rMaxVal)) {
      rVal = rMinVal;  rTime = minsmpl;
    } else {
      rVal = rMaxVal;  rTime = maxsmpl;
    }
    cout<<"getDetectorTimingT:  Signal at "<<rTime<<"  Amplitude "<<rVal<<endl;

    //    if(rVal<0.) rVal = 0.;
    Double_t  par0[6];
    par0[0] = rVal;      //3.;
    par0[1] = max(0.,(Double_t)(rTime-RISETIME));
    par0[2] = 4.;
    par0[3] = 1.6;
    par0[4] = (ig==HIGH?  PEDESTAL*hgDetChannels   :  PEDESTAL*lgDetChannels);  //   we do not do pedestal subtrastion at this time
    par0[5] = 0;      //   slope of the pedestals is initially set to "0"
    evtShape[ig]->SetParameters(par0);
    for(int parn=0; parn<6; parn++) evtShape[ig]->SetParLimits(parn, par0Min[parn], par0Max[parn]);
    evtShape[ig]->SetParLimits(4, par0[4]-PEDESTAL, par0[4]+PEDESTAL);
    //  fitFunc->cd(chan+1);  //  shapes[chan]->Draw();
    //  gPad->Modified();
    evtGraph[ig]->Fit(evtShape[ig], "RQWM", "NQ", 0., (Double_t)NSAMPLES);
    evtShape[ig]->GetParameters(evtfitpar[ig]);
    cout<<"<getDetectorTiming>: Gain "<<ig<<"  Parameters (0) "<<evtfitpar[ig][0]<<" (1) "<<evtfitpar[ig][1]<<" (2) "<<evtfitpar[ig][2]<<" (3) "<<evtfitpar[ig][3]<<" (4) "<<evtfitpar[ig][4]<<" (5) "<<evtfitpar[ig][5]<<endl;
    evtpedestal[ig]  = (evtfitpar[ig][4]+evtfitpar[ig][5]*rTime);   //  
    evtSignal[ig]->SetParameters(evtfitpar[ig]);
    evtSignal[ig]->SetParameter(4, 0.);
    evtSignal[ig]->SetParameter(5, 0.);
    Double_t sgp  = evtSignal[ig]->GetMaximum(par0Min[1], par0Max[1]);
    Double_t tmp  = evtSignal[ig]->GetMaximumX(par0Min[1], par0Max[1]);
    Double_t sgn  = evtSignal[ig]->GetMinimum(par0Min[1], par0Max[1]);
    Double_t tmn  = evtSignal[ig]->GetMinimumX(par0Min[1], par0Max[1]);
    evtpeak[ig]  = rVal;
    evttime[ig]  = rTime;
    cout<<"<getDetectorTimingT>: Gain "<<ig<<"  Peak0 "<<  evtpeak[ig]<<"  Time0 "<< evttime[ig]<<endl;
    if(tmp!=0&&abs(sgp)>abs(sgn)) {
      evtpeak[ig]  = sgp;
      evttime[ig]  = tmp;
      cout<<"<getDetectorTimingT>: Positive peak selected"<<endl;
    } else {
      evtpeak[ig]  = sgn;
      evttime[ig]  = tmn;
      cout<<"<getDetectorTimingT>: Negative peak selected"<<endl;
    }
    tChi2[ig]    = evtShape[ig]->GetChisquare()/evtShape[ig]->GetNDF();
    cout<<"<getDetectorTimingT>: Gain "<<ig<<"  Peak "<<  evtpeak[ig]<<"  Time "<< evttime[ig]<<"  Chi2 "<<tChi2[ig]<<endl;
  }

}
// **************************************************************************

void hLabHelper::displaySumEvent(){

  Int_t ng           = (lgDetChannels!=0&&hgDetChannels!=0)? 2  :  1;
  TCanvas * eventSum = (TCanvas *)(gROOT->FindObject("eventSum"));
  if(!eventSum) eventSum  = new TCanvas("eventSum","sPhenix eventSum display", 300, 300, 800, 500);
  else eventSum->Clear();
  eventSum -> Divide(1,ng);
  for(int ig = 0; ig < ng; ig++){
    eventSum->cd(ig+1);
    //    for (int is = 0; is < NSAMPLES; is++)  evtGraph[ig]->SetPoint(is,(Double_t)is,evtadcsum[ig][is]);
    //  cout<<"<tileDisplay>   call to fitTileSignal"<<endl;
    //  eventSum->Divide(1,2);
    //  eventSum->cd(1);
    //  eventSum->SetEditable(kTRUE);
    evtGraph[ig]->SetMarkerStyle(20);
    evtGraph[ig]->SetMarkerSize(0.4);
    eventSum->cd(ig+1);
    evtGraph[ig]->Draw("acp");
    //  eventSum->cd(2);
    evtShape[ig]->Draw("same");
    // evtShape[ig]->Draw();
    // evtGraph[ig]->Draw("same");
    eventSum->Update();
  }
  eventSum->Update();
  //  eventSum->Clear();
}
// **************************************************************************
//  FIXES to data 
void hLabHelper::dofixes(){
  // if(runnumber>=1125 && runnumber<1152){
  //   //  invert pulse in channel 134 (number 9 in todays parlance
  //   for (int is = 0; is < NSAMPLES; is++) {adc[8][is] -= 2*PEDESTAL; adc[8][is] = abs(adc[8][is]);}
  // }
  ;
}

// **************************************************************************

Bool_t hLabHelper::reject(){
  return false;
}
// **************************************************************************
