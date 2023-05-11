
// *************************************************************************
hcalHelper::hcalHelper(){
  if(ACTIVECHANNELS) {
    cout<<"<hcalHelper::hcalHelper>  Wrong instantiation  sequence  - hLabHelper already exists.  EXITING"<<endl;
    delete this;
  }
  ACTIVECHANNELS = 192;   // just to insure everything works as it must
  hLabHelper::getInstance();
  eventseq    = 0;
  eventsread  = 0;
  displayMode = 3;   //   default - only final summary is displayed
  //  TODO:  needs better understanding - how to handle HG Gains
  //  create calorimeter

  //  we create all graphic objects in the directory of temporary file (rootTempFileName)

  t1044 = new hcal();    //  it does stacks configuring at this time
  //  ONLY ACTIVE channels will be copyed from .prdf to adc
  CHTOTAL        = detchannels;
  ACTIVECHANNELS = CHTOTAL;
  channels       = ACTIVECHANNELS;
  samples        = NSAMPLES;
  activeCh    = new Int_t   [ACTIVECHANNELS];
  adc         = new Int_t * [ACTIVECHANNELS];
  for (int ich=0; ich<ACTIVECHANNELS; ich++) {
    adc[ich]        = new Int_t  [NSAMPLES];
  }
  //  parse calibration information
  t1044->setCalibration();
  //  make a look-up table for easy access to adc channels
  Int_t  ilive = 0;
  for(int istk = 0; istk<CALSTACKS; istk++) {
    if(t1044->alive[istk]) {
      t1044->stacks[istk]->updateMap(ilive);
      Int_t stackid   = t1044->stacks[istk]->stackId;
      Int_t chtocp    = t1044->stacks[istk]->twrsInStack*t1044->stacks[istk]->gains;
      for(int ich = 0; ich < chtocp; ich++)  {
	activeCh[ilive] = (stackid==EMC?     emcCh[ich] : 
			   (stackid==HINNER?  hcalInnerCh[ich]  :
			    (stackid==HOUTER?  hcalOuterCh[ich]  :
			     (stackid==HODO?    hodoCh[ich]      :
			      counters[ich]))));
	ilive ++;
      } 
      //      t1044->stacks[istk]->print();
    }
  }
  cout<<"<hcalHelper::hcalHelper>:  "<<ACTIVECHANNELS<<" Channels Initialized"<<endl;
  rdata        = new TH2F("rdata", "Raw Peak Values for all active channels",ACTIVECHANNELS, 0, ACTIVECHANNELS, 1024, -2048., 2048.);

}


// **************************************************************************
void hcalHelper::setRunKind(TString kind) {
  hLabHelper  * hlHelper = hLabHelper ::getInstance();
  hcalHelper  * hHelper  = hcalHelper ::getInstance();
  hlHelper -> setRunKind(kind);
  if(kind=="cosmics") {
    hHelper->t1044->alive[EMC]   = kFALSE;
    hHelper->t1044->alive[HODO]  = kFALSE;
    hHelper->t1044->alive[SCINT] = kFALSE;
    delete hHelper->t1044->stacks[EMC]; 
    delete hHelper->t1044->stacks[HODO]; 
    delete hHelper->t1044->stacks[SCINT]; 
    hHelper->t1044->stacks[EMC]   = NULL;
    hHelper->t1044->stacks[HODO]  = NULL;
    hHelper->t1044->stacks[SCINT] = NULL;
  }
}

// **************************************************************************

int hcalHelper::evLoop(int run, int evToProcess, int fToProcess){
  runnumber      = run;
  if(runnumber<=0) return 0;
  hLabHelper  * hlHelper = hLabHelper ::getInstance();
  hlHelper->runnumber = runnumber;
  hlHelper -> setPRDFRun(runnumber, kFALSE);
  hlHelper -> makeCanvasDirectory();
  hcalTree::getInstance();
  //  hlHelper-> fhcl->cd();

  int OK;

  Eventiterator *it =  new fileEventiterator(hlHelper->prdfName, OK);
  if (OK)
    {
      cout <<"<evLoop> Couldn't open input file " << hlHelper->prdfName << endl;
      delete(it);
      return 0;
    }
  Event *evt;
  // Loop over events
  while ( (evt = it->getNextEvent()) != 0 )
    {
      t1044 -> readyForEvent = kFALSE;
      eventseq = evt->getEvtSequence();
      eventReject  = 0;
      eventTrigger = 0;
      if ( evt->getEvtType() != 1 ) {
	cout << "eventseq " << eventseq << " event type = " << evt->getEvtType() << endl;
	continue;
      }

      // Get sPHENIX Packet
      Packet_hbd_fpgashort *sp21101 =
	dynamic_cast<Packet_hbd_fpgashort*>( evt->getPacket(21101));
      Packet_hbd_fpgashort *sp21102 =
	dynamic_cast<Packet_hbd_fpgashort*>( evt->getPacket(21102));
      if(sp21101&&sp21102){
	sp21101->setNumSamples( NSAMPLES );  sp21102->setNumSamples( NSAMPLES );
	for (int ich=0; ich<ACTIVECHANNELS; ich++)    {
	  Int_t spCh  = activeCh[ich]<144?  activeCh[ich] :activeCh[ich]-144; 
	  Packet_hbd_fpgashort* spacket = activeCh[ich]<144? sp21101  :  sp21102;
	  for (int is=0; is<NSAMPLES; is++)   	{
	    adc[ich][is] =  spacket->iValue(spCh,is);
	    //	      cout<<"<evLoop  Event "<< hlHelper->eventseq<<"  ich/is/adc  "<<ich<<" - "<<is<<" - "<< hlHelper->adc[ich][is]<<" active "<<hlHelper->active[ich]<<endl;
	  }
	}
	delete sp21101; delete sp21102;
      } else {
	cout << "EventSeq " << eventseq << " type = " << evt->getEvtType() <<" Missing packets " <<(int)sp21101<<"/"<<(int)sp21102<<endl;
	continue;
      }
      eventsread++;
      if(eventsread%1000==0)  cout <<"RUN  "<<runnumber<<"  EVENT "<<eventseq <<" - "<<eventsread<<endl;
      //  invert raw data in non-calorimeter stacks to make all signals negative
      //  subtract 2048 everywhere
      for (int istk = 0; istk<CALSTACKS; istk++){
	if(!t1044->alive[istk]) continue;
	for (int itw=0; itw<t1044->stacks[istk]->twrsInStack; itw++){
	  for(int ig=0; ig<t1044->stacks[istk]->gains; ig++){
	    int iadc = t1044->stacks[istk]->towers[itw]->adcChannel[ig];
	    for(int is=0; is<NSAMPLES; is++){
	      if(t1044->stacks[istk]->stackKind==CALOR) {
		adc[iadc][is] =  adc[iadc][is]-2048.;
	      } else {	
		adc[iadc][is] = (activeCh[iadc]%2? -(adc[iadc][is]-2048.)   :  adc[iadc][is]-2048.);
	      }
	      //  knowing where this value goes we may immediately set satFlags
	      t1044->stacks[istk]->towers[itw]->graph[ig]->SetPoint(is,(Double_t)is,adc[t1044->stacks[istk]->towers[itw]->adcChannel[ig]][is]);
	      if(adc[iadc][is]<undflow||adc[iadc][is]>ovrflow) 
		t1044->stacks[istk]->towers[itw]->satFlag[ig] += 1;	     
	    } //  is
	  } //  ig
	} //  itw
      } //  istk
      //  amount of data displayed in per event and per run display
      //  0  - nothing displayed except Summary at the end of processing
      //  1  - digitizations for everything
      //  2  - lego plots of individual amplitudes in calorimeter and digitization for counter
      Int_t rejectST = t1044->getStackTiming();
      if(rejectST<3 || !triggerOn)  {  //  HCal is not entirely empty or I want to see all events
	Int_t rejectCR = collectRaw();
	Int_t rejectCL = t1044->update(displayMode, kFALSE);  
	eventReject    = rejectST*1000 + rejectCR*100 + rejectCL;
	eventTrigger   = t1044->collectTrPrimitives();
	if(displayMode<3) cout<<"Event "<<eventseq<<"  Trigger "<<eventTrigger<<"  eventReject "<<eventReject<<"  "<<rejectST<<"/"<<rejectCR<<"/"<<rejectCL<<endl;;
	t1044->displayRaw(displayMode);   
      }
    
      delete evt;
      if(displayMode<3){
	//	if(std::cin.get()==(Int_t)"q") break;
	if(std::cin.get()==113) break;
	//      htree -> thcl -> Fill();
      }
      t1044->clean();
      if ((evToProcess>0&&eventsread>=evToProcess)||eventsread>270000) break;
    }
  //  display (print) un summary. Verbosity mode varies
  t1044->displaySummary(displayMode);
  
  
  cout<<"ALLDONE for RUN  "<<runnumber<<"  Events "<<eventsread<<endl;
  delete it;
  hlHelper -> thcl -> Write();
  hlHelper -> fhcl -> Write();
  //  TODO - if multiple files are processed closing the root file should be done somewhere else
  //  TODO - at the moment 
  //  hlHelper -> renameAndCloseRF();
  //  closeLoop();
  return eventseq;
}


// **************************************************************************
//  amount of per event and per run display
//  0  - nothing displayed except Summary at the end of processing
//  1  - digitizations for everything
//  2  - lego plots of individual amplitudes in calorimeter and digitization for counters

void hcalHelper::setDisplayMode(Int_t mode){
  displayMode = mode;
  cout<<"<hcalHelper::setDisplayMode>  Mode is set to "<<displayMode<<endl;
}

// **************************************************************************

void hcalHelper::setCTriggerOn(Bool_t  ON){
  triggerOn = ON;
  cout<<"<hcalHelper::setCTriggerOn>  triggerOn is set to "<<triggerOn<<endl;
}

// **************************************************************************

void hcalHelper::setCTriggerGRange(Int_t gain){
  triggerGain = gain;
  TRGAINRANGE = gain;
  cout<<"<hcalHelper::setDCTriggerGRange>  Gaine is set to "<<triggerGain<<endl;
}

// **************************************************************************
void hcalHelper::updateCalibration(){
  //  hLabHelper  * hlHelper = hLabHelper ::getInstance();
  ;
}

// **************************************************************************

void  hcalHelper::hcalPattern(Int_t nx, Int_t ny, Int_t run, Int_t mod){;}

// **************************************************************************

//  To call after global timing is extracted for individual stacks (note that scintillating counters are considered "stacks" - they are probably individually timed. Scintillating hodoscope is stack in it own rights
Int_t  hcalHelper::collectRaw(){
  // currently no rejection at this stage
  rejectRaw = 0;
  //  stacks first
  for(int istk = 0; istk<CALSTACKS; istk++){
    if(!(t1044->stacks[istk])) continue;
    for(int ig = 0; ig<t1044->stacks[istk]->gains; ig++){
      //     Double_t fPeak = t1044->stacks[istk]->fitPeak[ig];
      Int_t  fTime = (int)(t1044->stacks[istk]->fitTime[ig]+0.5);
      for (int itw = 0; itw<t1044->stacks[istk]->twrsInStack; itw++){
	Int_t ich = t1044->stacks[istk]->towers[itw]->adcChannel[ig];
	//  extract raw peak position and peak value for this channel
	Int_t iss = max((fTime-2), 1);// Int_t ise = min((fTime+3), NSAMPLES); 
	Double_t psum(0.); float sUsed(0.);
	for (int is = 0;  is<iss; is++) { if(adc[ich][is]>=undflow&&adc[ich][is]<=ovrflow) {psum += adc[ich][is]; sUsed += 1.;}}
  
	if(sUsed) psum /= sUsed; else psum=0.;
	Double_t rVal = adc[ich][fTime] - psum;
	t1044->stacks[istk]->towers[itw]->rawPed[ig]  = psum;
	t1044->stacks[istk]->towers[itw]->rawAmpl[ig] = rVal;
	t1044->stacks[istk]->towers[itw]->rawTime[ig] = fTime;    // rTime;
	rdata -> Fill(t1044->stacks[istk]->towers[itw]->adcChannel[ig], rVal);
      }
    
    }
  //  all kinds of scintillators etc 
  }
  return rejectRaw;
}
// **************************************************************************
void  hcalHelper::hcalImpact(){;}
// **************************************************************************
void  hcalHelper::fitHCalSignal(){;}
// **************************************************************************
void  hcalHelper::hcalDisplay(){;}
// **************************************************************************
//  FIXES to data 
void hcalHelper::dofixes(){
  // if(runnumber>=1125 && runnumber<1152){
  //   //  invert pulse in channel 134 (number 9 in todays parlance
  //   for (int is = 0; is < NSAMPLES; is++) {adc[8][is] -= 2*PEDESTAL; adc[8][is] = abs(adc[8][is]);}
  // }
  ;
}

// **************************************************************************

Int_t hcalHelper::reject(){
  return false;
}

// **************************************************************************
hcal::hcal(){
  
  maxStacks    = CALSTACKS;                                //  CALSTACKS
  stacks       = new stack *  [maxStacks];
  alive        = new Bool_t   [maxStacks];
  kinds        = new Int_t    [maxStacks];
  kinds[EMC]    = CALOR;      alive[EMC]     = kTRUE;    //  EMC
  kinds[HINNER] = CALOR;      alive[HINNER]  = kTRUE;    //  HInner
  kinds[HOUTER] = CALOR;      alive[HOUTER]  = kTRUE;    //  Outer HCal
  kinds[HODO]   = COUNTER;    alive[HODO]    = kTRUE;    //  HODOSCOPE
  kinds[SCINT]  = COUNTER;    alive[SCINT]   = kTRUE;    //  Beam counters
  kinds[CHER]   = COUNTER;    alive[CHER]    = kFALSE;   //  Cherenkov's
  activeStacks    = 0;
  activeCalStacks = 0;

  for (int istk = 0; istk < maxStacks; istk++) stacks[istk] = NULL;

  //  this is just a proyotype, it has well defined configuration
  detchannels      = 0;
  hgDetChannels    = 0;
  lgDetChannels    = 0;
  if(alive[EMC])      {
    stacks[EMC]     = new stack(EMC,    EMCTOWERS,   EMCCOLUMNS, EMCROWS);
    //    cout<<"MMM "<<stacks[EMC]->mapUpdated<<endl;
    stacks[EMC]->stackKind = kinds[EMC];
    activeStacks ++;
    activeCalStacks ++;
    detchannels    += EMCTOWERS*EMCGAINS;    //   only one gain (treat as LG)
    lgDetChannels  += EMCTOWERS*EMCGAINS;
    //    cout<<"MMM "<<stacks[EMC]->mapUpdated<<endl;
  }
  if(alive[HINNER])   {
    stacks[HINNER]  = new stack(HINNER, HCALTOWERS, HCALCOLUMNS, HCALROWS);
    stacks[HINNER]->stackKind = kinds[HINNER];
    activeStacks ++;
    activeCalStacks ++;
    detchannels    += HCALTOWERS*HCALGAINS;  //   dual gain readout in HCal
    hgDetChannels  += HCALTOWERS;
    lgDetChannels  += HCALTOWERS;
  }     
  if(alive[HOUTER])   {
    stacks[HOUTER]  = new stack(HOUTER, HCALTOWERS, HCALCOLUMNS, HCALROWS);
    stacks[HOUTER]->stackKind = kinds[HOUTER];
    activeStacks ++;
    activeCalStacks ++;
    detchannels    += HCALTOWERS*HCALGAINS;
    hgDetChannels  += HCALTOWERS;
    lgDetChannels  += HCALTOWERS;
  }    
  if(alive[HODO])   {
    stacks[HODO]  = new stack(HODO, T1044HODO, T1044HODO/2, 2);
    stacks[HODO]->stackKind = kinds[HODO];
    activeStacks ++;
    detchannels    += T1044HODO;
    lgDetChannels  += T1044HODO;
  }    
  if(alive[SCINT])   {
    stacks[SCINT]  = new stack(SCINT, T1044TRIGGERCH, T1044TRIGGERCH/2, 2);
    stacks[SCINT]->stackKind = kinds[SCINT];
    activeStacks ++;
    detchannels    += T1044TRIGGERCH;
    lgDetChannels  += T1044TRIGGERCH;
  }   
  for(int istk = 0; istk<CALSTACKS; istk++) {
    if(alive[istk]&&stacks[istk]->mapUpdated) cout<<"Stack "<<stacks[istk]->stackId<<" mapUpdated already set "<<stacks[istk]->mapUpdated<<endl;
  } 
  readyForEvent = kTRUE;
  clean();

  //  Summary histos for the whole calorimeter
  TString  totaN  = "tota";     
  TString  totaT  = "Total Amplitude in Calorimeter System";
  totalamp = new TH1F(totaN,  totaT, 10000, 0., 10000.);
  TString  toteN  = "tote";     
  TString  toteT  = "Total Energy in Calorimeter System";
  totale   = new TH1F(toteN,  toteT, 1000, 0., 60.);
  TString  tlcgN  = "tlcg";     
  TString  tlcgT  = "Shower Center of gravity (units of Stacks)";
  totallcg = new TH1F(tlcgN,  tlcgT,  200, 0., 2.);
  TString  sclcgN  = "sclcg";     
  TString  sclcgT  = "Shower Center in scintillators (units of Stacks)";
  scintlcg = new TH1F(sclcgN, sclcgT, 200, 0., 2.);
}

// **************************************************************************

void hcal::setCalibration(){
  //  set energy scales
  for(int istk = 0; istk<CALSTACKS; istk++){
    if(!alive[istk]) continue;
    stacks[istk]->stackECalib = stECalib[stacks[istk]->stackId];
  //  TODO:   set correction coefficients in towers (currently unity)
    if(stacks[istk]->stackKind == CALOR) {
      for (int itw=0; itw< stacks[istk]->twrsInStack; itw++) {
	stacks[istk]->towers[itw]->twrECalib = stECalib[stacks[istk]->stackId]; 
	//  must be scaled by the Muon peak positions
	stacks[istk]->towers[itw]->twrEScale  = 1.;
      }
    }
  }
  //  
}

// **************************************************************************
hcal::~hcal(){
  if(totalamp)  delete totalamp;
  if(totale)    delete totale;
  if(totallcg)  delete totallcg;
  if(scintlcg)  delete scintlcg;
  
}

// **************************************************************************

Int_t hcal::getStackTiming(){
  for(int istk = 0; istk<CALSTACKS; istk++){
    if(!stacks[istk]) continue;
    Int_t stackReject = stacks[istk]->getStackTime();
    if(kinds[istk] == CALOR && stackReject>10) rejectEvent ++ ;
  }
  //  raise reject flag 
  return rejectEvent;
}

// **************************************************************************

Int_t hcal::collectTrPrimitives(){
  int accept = 0; 

  //  CALOR primitives
  for(int istk = 0; istk<CALSTACKS; istk++){
    if(!stacks[istk]) continue;
    if(!stacks[istk]->stackKind == CALOR) continue;
    //  kill LED events at this time
    Int_t triggerFlag = stacks[istk]->getTrigger();
    accept += triggerFlag*pow(100,stacks[istk]->stackId);
  }
  return accept;
}

 
// **************************************************************************
//  done after collectRaw already collected and updated initial values for peaks and times for all objects (towers, counters)
//  It also updates graphical objects in towers so if you need individual towers visible - take care
Int_t hcal::update(Int_t displayMode, Bool_t fitShapes){
  //  updates & compute unique amplitudes (in objects with multiple ranges), times and energies
  for(int istk = 0; istk<CALSTACKS; istk++){
    if(!stacks[istk]) continue;
    //  update adds 1000 to whatever is already stored in reject (saturation in towers, below zero threshold). Returns reject
    Int_t sr  = stacks[istk]->update(fitShapes);     //  stack reject
    if(stacks[istk]->stackKind==CALOR&&sr) rejectEvent += pow(10,stacks[istk]->stackId);
  }
  //  TODO:  collect global event information for presentation 
  
  for(int istk = 0; istk<CALSTACKS; istk++){
    if(!alive[istk]||stacks[istk]->stackKind!=CALOR) continue;
    //    stacks[istk]->print();
    amplTotal += (stacks[istk]->totCorAmpl)*stAScale[istk];
    eTotal    += stacks[istk]->E;
    calLCG    += (float)(stacks[istk]->stackId)*stacks[istk]->totCorAmpl*stAScale[istk];
  }
  calLCG /= amplTotal;
  totalamp -> Fill(amplTotal);
  totale   -> Fill(eTotal);
  totallcg -> Fill(calLCG);
  //  now scintillators
  if(!alive[SCINT]) return 0;
  Double_t tcampl(0.);
  for (int sc = 0; sc<3; sc++){
    scintLCG += (sc+1)*stacks[SCINT]->towers[sc+3]->campl;
    tcampl   += stacks[SCINT]->towers[sc+4]->campl;
  }
  scintLCG /= tcampl;
  scintlcg -> Fill(scintLCG);
  //   
  return 0;
} 
// **************************************************************************
void hcal::displayRaw(Int_t mode){
  if(!mode || mode>2) return;
  for(int istk = 0; istk<CALSTACKS; istk++){
    if(!stacks[istk]) continue;
    //  kill LED
    //    if(stacks[istk]->triggerFlag && stacks[istk]->triggerHits<12) {
    if(mode==1||(mode==2&&stacks[istk]->stackKind==CALOR)) {
      stacks[istk]->displayEvent(mode);
    //      if(mode==1) 
      stacks[istk]->displayADCSum();
    }
    //  }
  }
}
 
// **************************************************************************
void hcal::displaySummary(Int_t mode){
  hcalHelper  * hHelper  = hcalHelper ::getInstance();
  hLabHelper  * hlHelper = hLabHelper ::getInstance();
  for(int istk = 0; istk<CALSTACKS; istk++){
    if(!stacks[istk]) continue;
    stacks[istk]->displayTowerSummary(mode);
    stacks[istk]->displayStackSummary(mode);
  }
  TCanvas * calSum;
  TString csN = "csSum";
  TString csT = "Calorimeter Summary for Run "; csT += hHelper->runnumber;
  calSum = (TCanvas *)(gROOT->FindObject(csN));
  if(!calSum) calSum  = new TCanvas(csN, csT, 50, 50, 500, 250);
  else calSum->Clear();
  calSum   -> SetEditable(kTRUE);
  calSum   -> Divide(4,1);
  calSum   -> cd(1);
  totalamp -> Draw();
  calSum   -> cd(2);
  totale   -> Draw();
  calSum   -> cd(3);
  totallcg -> Draw();
  calSum   -> cd(4);
  scintlcg -> Draw();
  calSum   -> Update();

  TCanvas * satSum;
  TString satsN = "satsum"; 
  TString satsT = "Saturation in Calorimeters   Run "; satsT += hHelper->runnumber;
  satSum = (TCanvas *)(gROOT->FindObject(satsN));
  if(!satSum) satSum  = new TCanvas(satsN, satsT, 50, 50, 600, 400);
  else satSum->Clear();
  satSum->Divide(2,3);
  for(int istk=0; istk<CALSTACKS; istk++)  {
    if(!alive[istk]||stacks[istk]->stackKind != CALOR)  continue;
    if(stacks[istk]->stackId==EMC) {
      satSum -> cd(1);
      satSum -> GetPad(1)->SetLogy();
      stacks[istk]->satProb[0]->Draw();
      satSum -> cd(2);
      totale -> Draw();
    }
    if(stacks[istk]->stackId==HINNER) {
      satSum -> cd(3);
      satSum -> GetPad(3)->SetLogy();
      stacks[istk]->satProb[0]->Draw();
      satSum -> cd(4);
      satSum -> GetPad(4)->SetLogy();
      stacks[istk]->satProb[1]->Draw();
    }
    if(stacks[istk]->stackId==HOUTER) {
      satSum -> cd(5);
      satSum -> GetPad(5)->SetLogy();
      stacks[istk]->satProb[0]->Draw();
      satSum -> cd(6);
      satSum -> GetPad(6)->SetLogy();
      stacks[istk]->satProb[1]->Draw();
    }
  }
  satSum->Update();
  if(!hlHelper->cDirectory.IsNull()){
    TString cName  = hlHelper->cDirectory;
    cName += "calSum_";  cName += hHelper->runnumber; cName += ".gif";
    calSum->SaveAs(cName);
    cName  = hlHelper->cDirectory; cName += "satSum_"; cName += hHelper->runnumber; cName += ".gif";
    satSum->SaveAs(cName);
  }
}
// **************************************************************************

void hcal::clean(){
  for(int istk = 0; istk<CALSTACKS; istk++){
    if(!stacks[istk]) continue;
    stacks[istk]->clean();
  }
  rejectEvent   = 0;
  readyForEvent = true;
  amplTotal = 0.;
  eTotal    = 0.;
  calLCG    = 0.;
  scintLCG  = 0.;
}

// **************************************************************************

stack::stack(const Int_t sId, const Int_t twrs, const Int_t xch, const Int_t ych){
  reject      =  0;
  mapUpdated  =  kFALSE;
  eventsSeen  =  0;
  //  cout<<"SSS STACK "<<mapUpdated<<endl;
  triggerFlag =  0;
  triggerHits =  0;
  triggerSum  =  0;
  stackECalib =  0.;
  trAmp       =  NULL;
  stackId      = sId;
  twrsInStack  = twrs;
  stackLoc     = -1;      //  location of this stack in ADC still not defined
  nTwrsX       = xch;
  nTwrsY       = ych;
  gains        = (stackId==HINNER || stackId==HOUTER)?  2  :  1;
  towers       = new tower * [twrsInStack];
  for (int itw = 0; itw<twrsInStack; itw++){
    int xSt = itw%nTwrsX;
    int ySt = itw/nTwrsX;
    towers[itw] = new tower(stackId, itw, xSt, ySt);
    towers[itw]->adcChannel[0]  = itw;
    towers[itw]->adcChannel[1]  = (stackId==HINNER || stackId==HOUTER)? itw+HCALTOWERS : -1;
  }
  //  cout<<"SSS0 "<<mapUpdated<<endl;
  for (int ig = 0; ig<gains; ig++){
    adcsum[ig]  = new Double_t [NSAMPLES];
    fitPar[ig]  = new Double_t [NPARAMETERS];
    TString gT  = "Stack ";  gT += stackId; gT += (ig==0? " HIGH " : " LOW "); gT += "gain"; 
    graph[ig]   = new TGraph(NSAMPLES);
    graph[ig] -> SetTitle(gT);
    TString sum = "adcsum_"; sum += (ig==HIGH)? "HG"  :  "LG";
    TString sig = "signal_"; sig += (ig==HIGH)? "HG"  :  "LG";
    shape[ig]   = new TF1(sum, &signalShape, 0., (Double_t)NSAMPLES, NPARAMETERS);
    signal[ig]  = new TF1(sig, &signalShape, 0., (Double_t)NSAMPLES, NPARAMETERS);
    //    Double_t    times[ACTIVECHANNELS];
    //    Double_t    chi2 [ACTIVECHANNELS];
  }

  //  cout<<"SSS1 "<<mapUpdated<<endl;

  E            = 0.;
  //  summaries from FIT
  // Float_t  maxAmpSum = (stackId==EMC&&emcGainSelection==0)? 2000.  :  1000.;
  // Float_t  maxPedSum = (stackId==EMC&&emcGainSelection==0)? 250.   :  25.;
  Float_t maxPedSum = 400.;
  Float_t maxAmpSum = 16000.;
  TString  stfpN = "st_fped"; stfpN += stackId; 
  TString  stfpT = "Stack ";  stfpT += stackId;  stfpT += " Pedestals - Fit [LG]";
  stFPed  = new TH1F(stfpN, stfpT, 100, -maxPedSum, maxPedSum);
  TString  stfaN = "st_famp"; stfaN += stackId;
  TString  stfaT = "Stack ";  stfaT += stackId;  stfaT += " Amplitudes - Fit [LG]";
  stFAmpl = new TH1F(stfaN, stfaT, 550, -0.1*maxAmpSum, maxAmpSum);
  TString  stftN = "st_ftime"; stftN += stackId;
  TString  stftT = "Stack ";   stftT += stackId; stftT += " Time - Fit [ADC ticks]";
  stFTime = new TH1F(stftN, stftT, NSAMPLES, 0., NSAMPLES);
  TString  stc2N = "st_c2";   stc2N += stackId;
  TString  stc2T = "Stack ";  stc2T += stackId;  stc2T += " Fit Chi2 ";
  stChi2  = new TH1F(stc2N, stc2T, 100, 0., 200.);  
  TString  stgrN = "st_gr";   stgrN += stackId;
  TString  stgrT = "Stack ";  stgrT += stackId;  stgrT += " Ratio HG/LG ";
  stFGR   = new TH1F(stgrN, stgrT, 100, -20., 20.);  

  //  cout<<"SSS2 "<<mapUpdated<<endl;
  //  summaries from contributing towers
  TString  stspN = "st_sped"; stspN += stackId; 
  TString  stspT = "Stack ";  stspT += stackId;  stspT += " Stack Pedestals Sum [LG]";
  stSPed  = new TH1F(stspN, stspT, 100, -maxPedSum, maxPedSum);
  TString  stsaN = "st_samp"; stsaN += stackId;
  TString  stsaT = "Stack ";  stsaT += stackId;  stsaT += " Stack Amplitude Sum  [LG]";
  stSAmpl = new TH1F(stsaN, stsaT, 550, -0.1*maxAmpSum, maxAmpSum);
  TString  stseN = "st_sen";  stseN += stackId;
  TString  stseT = "Stack ";  stseT += stackId; stseT += " Stack Energy Sum [MeV]";
  stSE    = new TH1F(stseN, stseT, 1100, -5., 50.);
  TString  ststN = "st_stime"; ststN += stackId;
  TString  ststT = "Stack ";   ststT += stackId; ststT += " <Stack Time> [clock ticks]";
  stAvT   = new TH1F(ststN, ststT, NSAMPLES, 0., NSAMPLES);
  TString  ststsN = "st_stsig"; ststsN += stackId;
  TString  ststsT = "Stack ";   ststsT += stackId; ststsT += " Time - RMS [clock ticks]";
  stTRMS  = new TH1F(ststsN, ststsT, NSAMPLES, 0., NSAMPLES);
  //  Hit impacts in the stack
  TString hitCGN   = "hitSt";  hitCGN += stackId; 
  TString hitCGT   = "Stack "; hitCGT += stackId; hitCGT += "  Impact X vs Y [towers]";
  hitCG    = new TH2F(hitCGN, hitCGT, nTwrsX, 0, nTwrsX, nTwrsY, 0, nTwrsY); 

  //  SATURATION
  for(int ig=0; ig<gains; ig++) {
    TString satN = "stsat";  satN += stackId; satN += ig;
    TString satT = "Stack "; satT += stackId; satT += ig==0? " HG " : " LG "; satT += "saturation probability vs # of sat. samples";
    satProb[ig] = new TH1F(satN, satT, 25, 0., 25.);
  }


  if(stackKind != CALOR/*||hlHelper->runKind!="cosmics"*/) return;
  //  cosmic related summaries
  TString trAmpN = "trAmp";    trAmpN += stackId;
  TString trAmpT = "Stack ";   trAmpT += stackId; trAmpT += "  Track Amplitude"; 
  trAmp    = new TH1F(trAmpN,    trAmpT,   300,  0., 300.);
  TString trCh2N = "trCh2";    trCh2N += stackId;
  TString trCh2T = "Stack ";   trCh2T += stackId; trCh2T += "  Track Chi2"; 
  trChi2   = new TH1F(trCh2N,    trCh2T,   100,  0., 10.);
  TString trCrN = "trCr";     trCrN   += stackId;
  TString trCrT = "Stack ";   trCrT   += stackId; trAmpT += "  Track Crossing"; 
  trCr     = new TH1F(trCrN,     trCrT,     40,  0., 4.);
  TString trSlN = "trSl";     trSlN   += stackId;
  TString trSlT = "Stack ";   trSlT   += stackId; trAmpT += "  Track Slope"; 
  trSl     = new TH1F(trSlN,   trSlT,     50, -1., 1.);
  TString trACh2N = "trACh2";  trACh2N += stackId;
  TString trACh2T = "Stack ";  trACh2T += stackId; trACh2T += "  Track Amplitude vs Chi2"; 
  trAmpCh2 = new TH2F(trACh2N, trACh2T, 300, 0,  300, 100, 0., 2.);
  TString trAstHN = "trAstH";  trAstHN += stackId;
  TString trAstHT = "Stack ";  trAstHT += stackId; trAstHT += "  Track Amplitude vs # of Hits in Stack"; 
  trAstH   = new TH2F(trAstHN, trAstHT,300, 0,  300, 100, 0., 16.);
  TString trASlN = "trASl";    trASlN += stackId;
  TString trASlT = "Stack ";   trASlT += stackId; trASlT += "  Track Amplitude vs # Track Slope"; 
  trASl    = new TH2F(trASlN,  trASlT, 300, 0,  300, 100, 0., 1.);
  //  Next scatterplot is the basis of calibration
  //  Its data are HG data divided by H/L gain ratio and so to make this 
  //  picture representative the number of bins is chose equal to
  //  amplimit*hgratio[stackId]
  //  cout<<"SSS4 "<<mapUpdated<<endl;
  Float_t amplimit = 100;  //  LG ADC counts
  Int_t   nx       = amplimit*hlgratios[stackId];
  TString trATwrN = "trATwrN"; trATwrN += stackId;
  TString trATwrT = "Stack ";  trATwrT += stackId; trATwrT += "  Tagged Twr Amplitude vs Twr # "; 
  trATwr   = new TH2F(trATwrN,  trATwrT, nx, 0, 100, twrsInStack, 0., twrsInStack);
  //  cout<<"SSSExit "<<mapUpdated<<endl;

}

// **************************************************************************
stack::~stack(){
  if(stackId < 0 || !towers) return;
  for (int itw = 0; itw < twrsInStack; itw++){
    if(towers[itw])  delete towers[itw];
  } 
  delete stFGR;
  delete stFPed;
  delete stFAmpl;
  delete stFTime;
  delete stChi2;
  delete stSPed;
  delete stSAmpl; 
  delete stSE;
  delete stAvT; 
  delete stTRMS;
  delete hitCG;
  if(trAmp) {
    delete trAmp;
    delete trChi2;
    delete trCr;
    delete trSl;
    delete trAmpCh2;
    delete trAstH;
    delete trASl;
    delete trATwr;
  }

}

// **************************************************************************

void stack::displayEvent(Int_t mode){
  if(!mode || mode>2)  return;
  hcalHelper * hHelper = hcalHelper::getInstance();
  TCanvas * pattern[2];
  for(int ig = 0; ig < gains; ig++){
    TString stckN = "StPat";   stckN += stackId; stckN += "_G_";    stckN += ig;
    TString stckT = "Stack ";  stckT += stackId; stckT += (ig==HIGH)? " HGain" : " LGain"; stckT += "  Event Pattern Display  Run "; stckT += hHelper->runnumber; 
    pattern[ig] = (TCanvas *)(gROOT->FindObject(stckN));
    if(!pattern[ig]) pattern[ig]  = new TCanvas(stckN, stckT, 50*(stackId+ig), 50*(stackId+ig), 100*nTwrsX, 100*nTwrsY);
    else pattern[ig]->Clear();
    pattern[ig] -> SetEditable(kTRUE);
    //  display digitizations
    if(mode == 1 || stackKind != CALOR) {
      pattern[ig] -> Divide(nTwrsX,nTwrsY);
      for (int itw = 0; itw<twrsInStack; itw++){
	Int_t ipad = (nTwrsY-(towers[itw]->y)-1)*nTwrsX + (towers[itw]->x)+1;
	pattern[ig] -> cd(ipad);
	//      cout<<"<stack::displayPattern "<<nTwrsX<<" "<<nTwrsY<<"  "<<x<<" "<<y<<"  "<<ipad<<endl;
	towers[itw]->graph[ig]->SetMarkerStyle(20);
	towers[itw]->graph[ig]->SetMarkerSize(0.4);
	towers[itw]->graph[ig]->Draw("acp");
	//  towers[itw]->shape[ig]->Draw("same");
      }
      pattern[ig]->Update();
    } else {
      TH2 * stackLego;
      //  display stack lego plot - only for calorimeter stacks
      TString slN  = "st"; slN += stackId; slN += "_G_"; slN += ig;
      TString slT  = "Mixed Amplitudes [LG] in Stack "; slT += stackId; 
      if(!(stackLego = (TH2F*) (gROOT->FindObject(slN)))) stackLego = new TH2F(slN, slT, nTwrsX, 0, nTwrsX, nTwrsY, 0, nTwrsY);
      for (int itw = 0; itw<twrsInStack; itw++){
	//	towers[itw]->print();
	//	cout<<"stack "<<stackId<<"  itw/x/y/E  "<<itw<<" / "<<towers[itw]->x<<" / "<< towers[itw]->y<<" / "<< towers[itw]->campl<<endl;
	stackLego->SetBinContent(1+towers[itw]->x, 1+towers[itw]->y, towers[itw]->campl);
      }
      stackLego  -> Draw("lego");
      pattern[ig]->Update();
      if(ig==0) break;
   }
  }
}

// **************************************************************************

void stack::displayTowerSummary(Int_t mode){
  if(mode<=2)  return;
  hLabHelper  * hlHelper = hLabHelper ::getInstance();
  hcalHelper  * hHelper = hcalHelper ::getInstance();
  TCanvas * corTwrsA;
  TString cTAN = "rtA_st";  cTAN += stackId;
  TString cTAT = "Stack ";  cTAT += stackId;  cTAT += " Hit Amplitudes in Towers [LG units]  Run ";  cTAT += hHelper->runnumber;
  corTwrsA = (TCanvas *)(gROOT->FindObject(cTAN));
  if(!corTwrsA) corTwrsA  = new TCanvas(cTAN, cTAT, 50*stackId, 50*stackId, 100*nTwrsX, 100*nTwrsY);
    else corTwrsA->Clear();
  corTwrsA -> SetEditable(kTRUE);
  corTwrsA -> Divide(nTwrsX,nTwrsY);
  for (int itw = 0; itw<twrsInStack; itw++){
    Int_t ipad = (nTwrsY-(towers[itw]->y)-1)*nTwrsX + (towers[itw]->x)+1;
    corTwrsA -> cd(ipad);
    corTwrsA -> GetPad(ipad)->SetLogy();
    towers[itw]->twrAmpl->SetMarkerStyle(20);
    towers[itw]->twrAmpl->SetMarkerSize(0.4);
    towers[itw]->twrAmpl->Draw();
    // rawTwrsT -> cd(ipad);
    // //    rawTwrsA -> GetPad(ipad)->SetLogy();
    // towers[itw]->twrTime->SetMarkerStyle(20);
    // towers[itw]->twrTime->SetMarkerSize(0.4);
    // towers[itw]->twrTime->Draw();    
  }
  corTwrsA->Update();
  if(!hlHelper->cDirectory.IsNull()){
    TString cName  = hlHelper->cDirectory;
    cName += "corTwrsA_";  cName += hHelper->runnumber;   cName += ".gif";
    corTwrsA->SaveAs(cName);
  }

  //  rawTwrsT->Update();
}
// **************************************************************************

void stack::displayStackSummary(Int_t mode){
  if(mode != 3 || stackKind != CALOR) return;
  hcalHelper  * hHelper  = hcalHelper ::getInstance();
  hLabHelper  * hlHelper = hLabHelper ::getInstance();
  TCanvas * stackSum(NULL), * muTracks(NULL);
  TString stN = "stSum";  stN += stackId;
  TString stT = "Stack "; stT += stackId;  stT += "  Summary for Run "; stT += hHelper->runnumber;
  stackSum = (TCanvas *)(gROOT->FindObject(stN));
  if(!stackSum) stackSum  = new TCanvas(stN, stT, 50*stackId, 50*stackId, 400, 400);
    else stackSum->Clear();
  stackSum -> SetEditable(kTRUE);
  stackSum -> Divide(4,2);
  //  TH1        * stFPed, * stFAmpl, * stFTime, * stChi2;
  //  TH1        * stSPed, * stSAmpl, * stSE,    * stAvT,   * stTRMS;
  stackSum ->cd(5);
  stSPed   ->Draw();
  stackSum ->cd(6);
  stackSum -> GetPad(6)->SetLogy();
  stSAmpl  ->Draw();
  stackSum ->cd(7);
  stAvT    ->Draw();
  stackSum ->cd(8);
  stackSum -> GetPad(8)->SetLogy();
  stSE     ->Draw();

  stackSum ->cd(1);
  stFPed   ->Draw();
  stackSum ->cd(2);
  stackSum -> GetPad(2)->SetLogy();
  stFAmpl  ->Draw();
  stackSum ->cd(3);
  stFGR    ->Draw();
  //  stFTime  ->Draw();
  stackSum ->cd(4);
  stChi2   ->Draw();
  stackSum ->Update();
  //  saturation probability
  if(eventsSeen) {
    for (int ig=0; ig<gains; ig++) {
      satProb[ig]->Scale(1./(float)eventsSeen);
      satProb[ig]->Scale(1./(float)twrsInStack);
    }
  }
  
  if(!hlHelper->cDirectory.IsNull()){
    TString cName  = hlHelper->cDirectory;
    cName += "stackSum_"; cName += stackId; cName += "_"; cName += hHelper->runnumber;  cName += ".gif";
    stackSum->SaveAs(cName);
  } 
  if(hlHelper->runKind == "cosmics"){
    TString muN = "muTr";   muN += stackId;
    TString muT = "Stack "; muT += stackId;  muT += "  Cosmic Muons in Run "; muT += hHelper->runnumber;
    muTracks = (TCanvas *)(gROOT->FindObject(muN));
    if(!muTracks) muTracks  = new TCanvas(muN, muT, 50*stackId, 50*stackId, 400, 400);
    else muTracks->Clear();
    muTracks -> SetEditable(kTRUE);
    muTracks -> Divide(3,2);
    muTracks -> cd(1);
    trAmp    -> Draw();
    muTracks -> cd(2);
    trCr     -> Draw();
    muTracks -> cd(3);
    trSl     -> Draw();
    muTracks -> cd(4);
    trASl    -> Draw();
    //   trChi2   -> Draw();
    muTracks -> cd(5);
    trAmpCh2 -> Draw();
    muTracks -> cd(6);
    trAstH   -> Draw();
    muTracks -> Update(); 

    if(!hlHelper->cDirectory.IsNull()){
      TString cName  = hlHelper->cDirectory;
      cName += "muTracks_"; cName += stackId; cName += "_"; cName += hHelper->runnumber;  cName += ".gif";
      muTracks->SaveAs(cName);
    }
  }
}

// **************************************************************************

void stack::displayADCSum(){
  hcalHelper  * hHelper  = hcalHelper ::getInstance();
  hLabHelper  * hlHelper = hLabHelper ::getInstance();
  TString stckN = "StEvS";    stckN += stackId;
  TString stckT = "Stack "; stckT += stackId; stckT += "  Stack Event Sum Display  Run "; stckT += hHelper->runnumber;
  TCanvas * stckEvSumDisplay = (TCanvas *)(gROOT->FindObject(stckN));
  if(!stckEvSumDisplay) stckEvSumDisplay  = new TCanvas(stckN, stckT, 25*stackId, 25*stackId, 600, 400);
  else stckEvSumDisplay->Clear();
  stckEvSumDisplay -> Divide(1,gains);
  for(int ig = 0; ig < gains; ig++){
    stckEvSumDisplay->cd(ig+1);
    //    for (int is = 0; is < NSAMPLES; is++)  evtGraph[ig]->SetPoint(is,(Double_t)is,evtadcsum[ig][is]);
    //  cout<<"<tileDisplay>   call to fitTileSignal"<<endl;
    //  eventSum->Divide(1,2);
    //  eventSum->cd(1);
    //  eventSum->SetEditable(kTRUE);
    graph[ig]->SetMarkerStyle(20);
    graph[ig]->SetMarkerSize(0.4);
    graph[ig]->Draw("acp");
    //  eventSum->cd(2);
    shape[ig]->Draw("same");
    // evtShape[ig]->Draw();
    // evtGraph[ig]->Draw("same");
    //    stckDisplay->Update();
  }
  stckEvSumDisplay->Update();
  if(!hlHelper->cDirectory.IsNull()){
    TString cName  = hlHelper->cDirectory;
    cName += "stackEvSum_"; cName += stackId; cName += "_"; cName += hHelper->runnumber;  cName += ".gif";
    stckEvSumDisplay->SaveAs(cName);
  }
}
// **************************************************************************

//   clean stack for a new event
void stack::print(){
  cout<<"<stack::print>  stackId "<<stackId<<"  Gains "<<gains<<"  twrsInStack "<<twrsInStack<<"  stackLoc "<<stackLoc<<"  TwrsXY "<<nTwrsX<<"/"<<nTwrsY<<"  mapUpdated "<<mapUpdated<<endl; 
  for (int itw = 0; itw < twrsInStack; itw++){
    if(towers[itw])  {
      towers[itw]->print();
    }
  }
}



// **************************************************************************
void stack::updateMap(Int_t stckloc){
  //  print();
  if(mapUpdated){
    cout<<"<stack::updateMap>  Already updated - EXIT"<<endl;
    return;
  }
  stackLoc     = stckloc;
  for (int itw = 0; itw<twrsInStack; itw++){
    towers[itw]->adcChannel[0] += stckloc;
    if(gains==2) towers[itw]->adcChannel[1] += stckloc;
  }
  mapUpdated = kTRUE;
}


// **************************************************************************

//   collectdata from towers in stack for a new event
Int_t stack::update(Bool_t fitShape){
  eventsSeen ++;
  Int_t  used = 0;
  for (int itw = 0; itw < twrsInStack; itw++){
    if(towers[itw])  {
      Int_t rejected = towers[itw]->update(fitShape);
      if(rejected<=2){
	used++;
	totPed     += towers[itw]->cped;
	totAmpl    += towers[itw]->rampl;
	totCorAmpl += towers[itw]->campl;
	//  with fit time used in every tower to identify event time 
	//  all towers in the stack will now have identical times
	avTwrTime  += towers[itw]->ctime;
	rmsTwrTime += towers[itw]->ctime*towers[itw]->ctime;
	E          += towers[itw]->E; 
      } //  else  if(rejected>5) towers[itw]->display();
      for(int ig = 0; ig<gains; ig++){
	satProb[ig] -> Fill(towers[itw]->satFlag[ig]);
      }     
    }
  }
  if(!used) {
    //  everything in saturation
    reject += 1000;
    return reject;
  }
  avTwrTime /= used;
  rmsTwrTime = 0.;    // (twrsUsed==0? 0.  :  sqrt(rmsTwrTime/twrsUsed-avTwrTime*avTwrTime));
  stSE    -> Fill(E);
  stSPed  -> Fill(totPed);
  stSAmpl -> Fill(totCorAmpl);
  stAvT   -> Fill(avTwrTime);
  stTRMS  -> Fill(rmsTwrTime);

  //  Compute impact CG and find cosmic track
  if(stackKind == CALOR) {
    getStackImpact();
    hLabHelper  * hlHelper = hLabHelper ::getInstance();
    if(hlHelper->runKind=="cosmics") getStackTrack();
  }
  return reject;
}


// **************************************************************************

//   getStackTime  creates summary event (one per gain) 
Int_t stack::getStackTime(){
  hcalHelper * hHelper = hcalHelper::getInstance();
  for(int ig = 0; ig < gains; ig++){
    //  build eventsum (adc sum without pedestal subtruction)
    //  do fitting etc (HG/LG separately)
    twrsUsed[ig] = 0.;
    for (int itw = 0; itw < twrsInStack; itw++) {
      if(towers[itw]->satFlag[ig]>2) continue;
      twrsUsed[ig] ++;
      for(int is = 0; is < NSAMPLES; is++)	
	adcsum[ig][is] += hHelper->adc[towers[itw]->adcChannel[ig]][is];
    }
    if(twrsUsed[ig]>=1) {
      for (int is = 0; is < NSAMPLES; is++) 
	graph[ig]->SetPoint(is,(Double_t)is,adcsum[ig][is]);
    } else {
      reject += 10;
      continue;
    }
    
    Double_t rVal, rTime;
    Int_t iss(RISETIME), ise(NSAMPLES-FALLTIME); 
    Int_t rMaxVal  = adcsum[ig][iss]; Int_t maxsmpl = iss; 
    Int_t rMinVal  = adcsum[ig][iss]; Int_t minsmpl = iss;

    for (int is = iss; is<=ise; is++) {
      if(adcsum[ig][is]>rMaxVal) {
	rMaxVal  = adcsum[ig][is];
	maxsmpl  = is;   
      }
      if(adcsum[ig][is]<rMinVal) {
	rMinVal  = adcsum[ig][is];
	minsmpl  = is;   
      }
    }
    if(abs(rMinVal) > abs(rMaxVal)) {
      rVal = rMinVal;  rTime = minsmpl;
    } else {
      rVal = rMaxVal;  rTime = maxsmpl;
    }
    //    cout<<"<stack::getStackTime>:  Raw Signal at "<<rTime<<"  Raw Amplitude "<<rVal<<endl;

    Double_t  par0[6];
    par0[0] = rMinVal;   //rVal;      //3.;
    par0[1] = minsmpl;   //rTime;  //max(0.,(Double_t)(rTime-RISETIME));
    par0[2] = 4.;
    par0[3] = 1.6;
    par0[4] = 0.; //PEDESTAL*twrsInStack;  //   we do not do pedestal subtrastion at this time
    par0[5] = 0.;                     //   slope of the pedestals is initially set to "0"
    shape[ig]->SetParameters(par0);
    for(int parn=0; parn<4; parn++) shape[ig]->SetParLimits(parn, par0Min[parn], par0Max[parn]);
    shape[ig]->SetParLimits(1, minsmpl-RISETIME, minsmpl+FALLTIME);// rTime-RISETIME, rTime+FALLTIME/2.);
    shape[ig]->SetParLimits(4, -256., 256.);
    shape[ig]->SetParLimits(5, -0.5,  0.5);
    graph[ig]->Fit(shape[ig], "RQWM", "NQ", 0., (Double_t)NSAMPLES);
    shape[ig]->GetParameters(fitPar[ig]);
    //   cout<<"<stack::getStackTime>: Gain "<<ig<<" minsmpl "<<rTime<<"  Parameters (0) "<<fitPar[ig][0]<<" (1) "<<fitPar[ig][1]<<" (2) "<<fitPar[ig][2]<<" (3) "<<fitPar[ig][3]<<" (4) "<<fitPar[ig][4]<<" (5) "<<fitPar[ig][5]<<endl;
    //  Signal is whatever left after fitted pedestal subtraction (removal)
    signal[ig]->SetParameters(fitPar[ig]);
    signal[ig]->SetParameter(4, 0.);
    signal[ig]->SetParameter(5, 0.);
    // Double_t sgp  = signal[ig]->GetMaximum(par0Min[1], par0Max[1]);
    // Double_t tmp  = signal[ig]->GetMaximumX(par0Min[1], par0Max[1]);
    Double_t sgn  = signal[ig]->GetMinimum(par0Min[1], par0Max[1]);
    Double_t tmn  = signal[ig]->GetMinimumX(par0Min[1], par0Max[1]);
    fitPeak[ig]  = sgn;
    fitTime[ig]  = tmn;
    fitPed[ig]  = (fitPar[ig][4]+fitPar[ig][5]*fitTime[ig]);  
    fitChi2[ig] = shape[ig]->GetChisquare()/shape[ig]->GetNDF();
    //    cout<<"<stack::getStackTime>: Gain "<<ig<<"  Peak "<<  fitPeak[ig]<<"  Time "<< fitTime[ig]<<"  Chi2 "<<fitChi2[ig]<<endl;
  }  //  end of loop over gain ranges

  //  select gain range to use for Summaries 
  //  TODO - study chi2 vs ????? - can it really signal overflow
  gainToUse = 0;
  Float_t  gratio(0.);
  Float_t  scale = 1.;
  if(stackKind == CALOR) {
    if(reject==20||(stackId==EMC&&reject==10)) return reject;  // saturation
    if(stackId==EMC) {
      gainToUse      = 0;
      if(emcGainSelection == HIGH) scale = HLGRATIO;
    } else {
      if(!reject) {
	gainToUse = 0;
	gratio = fitPeak[0]/fitPeak[1];
	scale  = hlgratios[stackId];
      } else {
	gainToUse = 1;
      }
    }
    if(twrsUsed[gainToUse]==twrsInStack){
      Double_t amp    = -fitPeak[gainToUse]/scale;
      if(amp < STZEROSUPTHR) reject += 100;  //  stack empty
      if(stackId!=EMC&&gainToUse==0)  stFGR  -> Fill(gratio);
      stFPed -> Fill(fitPed[gainToUse]/scale);
      stFAmpl-> Fill(amp);
      stFTime-> Fill(fitTime[gainToUse]);
      stChi2 -> Fill(fitChi2[gainToUse]/scale);
    }
  }
  return reject;
}

// **************************************************************************

//   compute impact positions in units of towers
//   return 
Int_t  stack::getStackImpact(){
  if(stackKind!=CALOR) return twrsInStack;
  xImpact = -1.;
  yImpact = -1.;
  Int_t used(0);
  double sxw(0.), syw(0.), sw(0.); 
  for (int itw=0; itw < twrsInStack; itw++) {
    if(towers[itw]->satFlag[towers[itw]->gainToUse]<=2){
      used++;
      sw   += towers[itw]->campl;
      sxw  += towers[itw]->campl*(Float_t)towers[itw]->x;
      syw  += towers[itw]->campl*(Float_t)towers[itw]->y;
    }
  }
  if(used&&sw>0.) {
    xImpact = sxw/sw;
    yImpact = syw/sw;
  }
  //  cout<<"<stack::getStackImpact> Stack "<<stackId<<"  sw "<<sw<<" X/Y "<<xImpact<<"/"<<yImpact<<endl;
  hitCG -> Fill(xImpact, yImpact);
  return twrsInStack - used;
}

// **************************************************************************

//   clean stack for a new eventcompute impact positions in units of towers
//   return kFALSE if 
Int_t  stack::getStackTrack(){
  if(stackKind!=CALOR) return 0;
  double ixs(0.), iys(0.), ix2s(0.), iy2s(0.), ixys(0.);
  double crossing(0.), slope(0.);
  Int_t  hits[nTwrsY];
  //  tag maximum amplitudes in each row
  //  it is possibly better to compute CG's in every row and use those 
  //  for tracking muons
  for(int r =0; r<nTwrsY; r++){   //  loop over raws
    hits[r]  = 0;   Double_t hitAmpl = 0.;
    for (int c = 0; c<nTwrsX; c++) {
      int itw = nTwrsX*r+c;
      if(towers[itw]->campl>hitAmpl) {hits[r] = itw;  hitAmpl = towers[itw]->campl;}
      if(towers[itw]->campl>TWRAMPTHR&&towers[itw]->satFlag[towers[itw]->gainToUse]<=2) stackHits ++;
    }
  }
  for (int r =0; r<nTwrsY; r++){    //  loop over towers peaking in rows 
    Double_t amp  = towers[hits[r]]->campl;
    if(amp>TWRAMPTHR) {
      trackHits ++;
      trackAmpl +=  amp;
      Float_t  x = (Float_t)towers[hits[r]]->x;
      Float_t  y = (Float_t)towers[hits[r]]->y;
      ixs       += (amp*x);
      iys       += (amp*y);
      ix2s      += (amp*x*x);
      iy2s      += (amp*y*y);
      ixys      += (amp*x*y);
    }
  } 
  if(trackHits<=2) return trackHits;
  //  we solve for x=F(y) - cosmics is vertical
  if(trackAmpl>0.) {
    //      icr    = (ixs*ixys - ix2s*iys)/(ixs*ixs-is*ix2s);
    //      islope = (-is*ixys+ixs*iys)   /(ixs*ixs-is*ix2s);
    crossing  = (iys*ixys - iy2s*ixs)/(iys*iys-trackAmpl*iy2s);
    slope     = (-trackAmpl*ixys+iys*ixs)   /(iys*iys-trackAmpl*iy2s);
  }
  
  //  convert back to y = F(x)
  
  //  now estimator 
  for (int itw = 0; itw < twrsInStack; itw++){
    Double_t amp  = towers[itw]->campl;;
    Float_t  x = (Float_t)towers[itw]->x;
    Float_t  y = (Float_t)towers[itw]->y;
    float  est(0.), estx(0.), esty(0.);
    if(slope!=0.) {
      est     = abs((y - x/slope+crossing/slope)/sqrt(pow(1./slope,2)+1.)); 
      //	  estx    = ((x-(islope*y+icr))*(x-(islope*y+icr)));
      //	  esty    = islope!=0? (y-(x-icr)/islope)*(y-(x-icr)/islope)  : 0.;
    } else {
      //  crossing and slope are just tower coordinate in this case
      estx = (x-crossing);
      esty = (y-slope);
      est  = sqrt(estx*estx+esty*esty)/2;
    }
    trchi2 += est*amp;
  }
  trchi2 /= totAmpl;
  trAmp   -> Fill(trackAmpl);
  trChi2  -> Fill(trchi2);
  trCr    -> Fill(crossing);
  trSl    -> Fill(slope);
  trAmpCh2-> Fill(trackAmpl, trchi2);
  trAstH  -> Fill(trackAmpl, stackHits);
  trASl   -> Fill(trackAmpl, abs(slope));
  if(abs(slope)<0.45||abs(slope)>0.55) {
    for (int itw = 0; itw < twrsInStack; itw++){
      if(towers[itw]->campl>TWRAMPTHR) 
	trATwr->Fill(towers[itw]->campl, (float)itw);
    }    
  }
  //  cout<<"<stack::getStackTrack> Stack "<<stackId<<"  Hits "<<stackHits<<"/"<<trackHits<<"  Cr/Sl "<<crossing<<"/"<<slope<<"  Chi2 "<<trchi2<<"  Tr Ampl "<<trackAmpl<<endl;
  return trackHits;
}

// **************************************************************************

//   clean stack for a new event
Int_t  stack::getTrigger(){
  
  for (int itw = 0; itw < twrsInStack; itw++) {  
    Float_t twrAmp = (gains==1)?  -(towers[itw]->rawAmpl[0])  :  -(towers[itw]->rawAmpl[TRGAINRANGE]);
    if((int)(twrAmp)/8>TWRAMPTHR) triggerHits ++;
    triggerSum += twrAmp;
  }
  if((int)triggerSum/8>STTOTAMPTHR)                             triggerFlag += 1;
  if(triggerHits >= STHITMIN && triggerHits <=STHITMAX)         triggerFlag += 10;
  return triggerFlag;
}
// **************************************************************************

//   clean stack for a new event
void stack::clean(){
  for (int itw = 0; itw < twrsInStack; itw++) {  if(towers[itw])  towers[itw]->clean();}
  for (int ig = 0; ig<gains; ig++){
    for (int is=0; is<NSAMPLES; is++) {
      adcsum[ig][is] = 0.;
    }
    for(int ip=0; ip<NPARAMETERS; ip++){
      fitPar[ig][ip] = 0.;
    }
    fitPed[ig]  = 0.;
    fitPeak[ig] = 0.;
    fitChi2[ig] = 0.;
  }
  reject      = 0;
  totPed      = 0.;
  totAmpl     = 0.;
  totCorAmpl  = 0.;
  avTwrTime   = 0.;
  rmsTwrTime  = 0.;
  twrsUsed[0] = 0;
  twrsUsed[1] = 0;
  triggerHits = 0;
  triggerSum  = 0.;
  triggerFlag = 0;
  stackHits   = 0;
  trackHits   = 0;
  crossing    = 0.;
  slope       = 0.;
  trchi2      = 0.;
  trackAmpl   = 0.;
  E           = 0.;
}


// ******************************** TOWER ***********************************

tower::tower(Int_t stk, Int_t ind, Int_t xSt, Int_t ySt){
  gainToUse =  0;
  reject    =  kFALSE;
  attached  =  kFALSE;
  stackId   = stk;
  index     = ind;
  x         = xSt;
  y         = ySt;
  twrECalib = 1.;
  twrEScale = 1.;
  gains        = (stackId==HINNER || stackId==HOUTER)?  2  :  1;
  if(gains==1) gainToUse = 0;
  for(int ig = 0; ig < gains; ig++) {
    graph[ig]   = new TGraph(NSAMPLES);
    TString title = "Stack ";     title += stackId; title += " Tower "; title += index; 
    title += "(";     title += x; title += "/";     title += y;         title += ") "; 
    title += (ig==HIGH)? " HGain"  :  " LGain";
    TString sig = "sig_"; sig += (ig==HIGH)? "HG_"  :  "LG_"; 
    sig += stackId;   sig   += "_";                sig   += index;
    TString shp = "shp_"; shp += (ig==HIGH)? "HG_"  :  "LG_"; 
    shp += stackId;   shp   += "_";                shp   += index;
    graph[ig]->SetTitle(title);
    shape[ig]   = new TF1(shp, &signalShape, 0., (Double_t)NSAMPLES, NPARAMETERS);
    signal[ig]  = new TF1(sig, &signalShape, 0., (Double_t)NSAMPLES, NPARAMETERS);
  }
  //  run summaries
  TString  cpN = "twr_ped"; cpN += stackId; cpN += index;
  TString  cpT = "Stack "; cpT += stackId; cpT += " Tower "; cpT += index; cpT += " Mixed pedestals [LG]";
  twrPed  = new TH1F(cpN, cpT, 100, -5., 5.);
  TString  caN = "twr_amp"; caN += stackId; caN += index;
  TString  caT = "Stack "; caT += stackId; caT += " Tower "; caT += index; caT += " Mixed Amplitudes [LG]";
  twrAmpl = new TH1F(caN, caT, 250, -50., 200.);
  TString  ctN = "twr_time"; ctN += stackId; ctN += index;
  TString  ctT = "Stack "; ctT += stackId; ctT += " Tower "; ctT += index; ctT += " Time [ADC ticks]";
  twrTime = new TH1F(ctN, ctT, NSAMPLES, 0., NSAMPLES);
  TString  ceN = "twr_en"; ceN += stackId; ceN += index;
  TString  ceT = "Stack "; ceT += stackId; ceT += " Tower "; ceT += index; ceT += " Energy [MeV]";
  twrE    = new TH1F(ceN, ceT, 250, -500., 20000.);

  clean();
}

// **************************************************************************

//   update event information from hcalHelper
//   must be called after stackTiming (Bool_t fit) and collectRaw()
Int_t tower::update(Bool_t fitShape){
  //  hcalHelper * hHelper = hcalHelper::getInstance(); 

  //  decide on gainToUse for this tower
  gainToUse = 0;
  if(gains==2&&satFlag[0]>0)  gainToUse = 1;     //  all gain ranges saturated

  //  compute unique values (bridge the gain ranges). Keep raw amplitudes as they came from collectRaw (signed and unscaled - we still need those for trigger emulator
  rped  = rawPed[gainToUse];   
  rampl = -rawAmpl[gainToUse]; 
  if(gains>1&&gainToUse==0) {
    rped  /= hlgratios[stackId];
    rampl /= hlgratios[stackId];
  }
  rtime = rawTime[gainToUse];
  cped  = rped; campl = rampl; ctime = rtime;   //  no corrections applied yet
  //  if this is EMC and it has emcGainSelection = 0 correct it for HLGRATIO=16
  if(stackId == EMC && emcGainSelection==0) {
    rped  /= HLGRATIO;
    rampl /= HLGRATIO;
    cped  /= HLGRATIO;
    campl /= HLGRATIO;
  }
  if(campl<TWRAMPTHR) return NSAMPLES;
  E     = campl*twrECalib;
  //  store event data
  twrPed  -> Fill(cped);
  twrAmpl -> Fill(campl);
  twrTime -> Fill(ctime);
  twrE    -> Fill(E);
  //  trigger primitive (thresholding)
  // if(stackId==HOUTER) {
  //    display();
  // }
  //  if(!fitShape) return reject;
  return satFlag[gainToUse];
}
// **************************************************************************

//   clean tower for a new event
void tower::clean(){
  for(int ig = 0; ig<gains; ig++)    {rawAmpl[ig] = 0.; rawTime[ig] = 0.; rawPed[ig] = 0.; satFlag[ig]=0;}
  reject   =  kFALSE;  
  rped     = 0.;
  rampl    = 0.;
  rtime    = 0.;
  cped     = 0.; 
  campl    = 0.;
  ctime    = 0.;
  E        = 0.;
  gainToUse  = 0;
  attached   =  kFALSE;
}

// **************************************************************************

//   clean stack for a new event
void tower::print(){
  cout<<"<tower::print>: St/twr "<<stackId<<"/"<<index<<"  X/Y "<<x<<"/"<<y<<" gainToUse "<<gainToUse<<"  Channels "<<adcChannel[0]<<"/"<<adcChannel[1]<<"  SatFlag "<<satFlag[0]<<"/"<<satFlag[1]<<"  Raw ampl "<<rawAmpl[0]<<" / "<<rawAmpl[1]<<"  Raw time "<<rawTime[0]<<" / "<<rawTime[1]<<"  Corrected ampl/time "<<campl<<"/"<<ctime<<"  Reject "<<reject<<endl;
}


// **************************************************************************

void tower::display(){
  hcalHelper * hHelper = hcalHelper::getInstance();
  TString twrN = "twr";    twrN += stackId; twrN += index;
  TString twrT = "Stack "; twrT += stackId; twrT += " Tower "; twrT += index; twrT += "Run "; twrT += hHelper->runnumber;
  TCanvas * twrDisplay = (TCanvas *)(gROOT->FindObject(twrN));
  if(!twrDisplay) twrDisplay  = new TCanvas(twrN, twrT, 0, 0, 400, 400);
  else twrDisplay->Clear();
  twrDisplay -> Divide(2,2);
  for(int ig=0; ig<gains;ig++){
    twrDisplay -> cd(ig*2+1);
    graph[ig]->SetMarkerStyle(20);
    graph[ig]->SetMarkerSize(0.4);
    graph[ig]->Draw("acp");
    twrDisplay -> cd(ig*2+2);
    twrAmpl  ->Draw();
  }
  twrDisplay->Update();
  print();
}

