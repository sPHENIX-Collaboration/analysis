

// **************************************************************************

tileHelper::tileHelper(){
  if(ACTIVECHANNELS) {
    cout<<"<tileHelper>:  Wrong instantiation  sequence  - hLabHelper already exists.  EXITING"<<endl;
    //    return;
  }
  //  ONLY ACTIVE channels will be copyed from .prdf to adc
  CHTOTAL        = TILECHANNELS+TILETRIGGERCH;
  ACTIVECHANNELS = CHTOTAL;
  channels       = ACTIVECHANNELS;
  samples        = NSAMPLES;
  fibers         = TILEFIBERS;
  detchannels    = TILECHANNELS;
    //  assumption is that out of all stored ADC channels the first detchannels 
    //  correspon to detector we study, the rest is everything else

  tileCalib    = new Double_t [detchannels];
  tileCalPeak  = new Double_t [detchannels];
  fiberLY      = new Double_t [fibers];
  fiberY       = new Double_t [fibers];
  yFit         = NULL;
  fLY          = NULL;
  for(int ifb = 0;   ifb<fibers;   ifb++) {fiberY[ifb] = 15.-2.5*(ifb+1); if(ifb) fiberY[ifb] -= 2.5;}

}

// **************************************************************************

int tileHelper::evLoop(int run, int evToProcess, int fToProcess){
  hLabHelper * hlHelper = hLabHelper::getInstance();
  hlHelper             -> setPRDFRun(run);
  tileTree::getInstance();

  int OK;


  Eventiterator *it =  new fileEventiterator(hlHelper->prdfName, OK);
  if (OK)
    {
      cout <<"<evLoop> Couldn't open input file " << hlHelper->prdfName << endl;
      delete(it);
      return 0;
    }

  Event *evt;
  int eventseq(0);
  // Loop over events
  while ( (evt = it->getNextEvent()) != 0 )
    {
      hlHelper->eventseq = evt->getEvtSequence();

      if ( evt->getEvtType() != 1 ) {
	cout << "eventseq " << hlHelper->eventseq << " event type = " << evt->getEvtType() << endl;
	continue;
      }
      // ev_display->Divide(nx_c,ny_c);
      // ev_display->SetBit(TPad::kClearAfterCR);

      // Get sPHENIX Packet
      Packet_hbd_fpgashort *spacket = dynamic_cast<Packet_hbd_fpgashort*>( evt->getPacket(21101) );
      if ( spacket )
	{
	  spacket->setNumSamples( NSAMPLES );
	  //	  int nmod_per_fem = spacket->iValue(0,"NRMODULES");
	  //cout << "nmod_per_fem " << nmod_per_fem << endl;
	  if(hlHelper->eventsread%1000==0)  cout <<"RUN  "<<hlHelper->runnumber<<"  EVENT "<<hlHelper->eventseq <<" - "<<hlHelper->eventsread<<endl;
	  hlHelper->eventsread++;
	  for (int ich=0; ich<ACTIVECHANNELS; ich++)    {
	    for (int is=0; is<NSAMPLES; is++)   	{
	      hlHelper->adc[ich][is] =  spacket->iValue(hlHelper->active[ich],is);
	      //	      cout<<"<evLoop  Event "<< hclt->eventseq<<"  ich/is/adc  "<<ich<<" - "<<is<<" - "<< hlHelper->adc[ich][is]<<" active "<<hlHelper->active[ich]<<endl;
	    }
	  }
	  hlHelper->collect();
	  collectTileSummary();
	  tileTiming();
	  delete spacket;
	}

      delete evt;
      hlHelper-> thcl -> Fill();
      if ((evToProcess>0&&hlHelper->eventsread>=evToProcess)||hlHelper->eventsread>270000) break;
    }
  // Normalize trigger histogramm to get per event hit count (number of channels above threshold) 
  int nbs = runsum.trhits->GetNbinsX()-1;
  for(int bx = 1; bx<=nbs; bx++){
    float normx = runsum.trhits->GetBinContent(bx);
    if(normx<=0.) continue;
    for (int threshold = 1; threshold<=CHANNELTHRESHOLDS; threshold++) {
      float cont = runsum.hits_tpc->GetBinContent(bx,threshold);
      runsum.hits_tpc->SetBinContent(bx, threshold, cont/normx);    
    }
  }
  
  if (hlHelper->eventsread>1) runsum.hits_tpc->Scale(1./(Double_t)hlHelper->eventsread);
  // status();
  cout<<"ALLDONE for RUN  "<<hlHelper->runnumber<<"  Events "<<hlHelper->eventsread<<endl;
  delete it;
  hlHelper->thcl -> Write();
  hlHelper->fhcl -> Write();
  hlHelper->fhcl -> Close();

  //  closeLoop();
  return eventseq;

}  

// **************************************************************************
void tileHelper::status(){
  hLabHelper * hlHelper = hLabHelper::getInstance();
  int nx_c = ACTIVECHANNELS<=12? 2  :  4;             
  int ny_c = ACTIVECHANNELS/nx_c+ACTIVECHANNELS%nx_c;
  if(!(fiberDisplay=(TCanvas*)(gROOT->FindObject("fiberDisplay")))) {

    TString fD = "Fiber Amplitudes (black - raw, red - fit) Run "; fD += runId.Data();
    //  TString fD = "Fiber Amplitudes (black - raw, red - fit, blue - integral) Run "; fD += hlHelper->prdfName;
    fiberDisplay = new TCanvas("fiberDisplay",fD,400*nx_c,200*ny_c);
    fiberDisplay->Divide(nx_c, ny_c);
  }
  Double_t ymx[ACTIVECHANNELS], xmx[ACTIVECHANNELS];
  for (int ich=0; ich<ACTIVECHANNELS; ich++){
    Double_t rvmax, fvmax, ivmax(0), rvrms, fvrms, ivrms(0), rvmean, fvmean, ivmean(0);
    rvmax = hlHelper->rpeak[ich]->GetMaximum(); 
    fvmax = hlHelper->fm[ich]->GetMaximum();
    //    ivmax = hlHelper->fint[ich]->GetMaximum();  
    rvrms = hlHelper->rpeak[ich]->GetRMS(); 
    fvrms = hlHelper->fm[ich]->GetRMS();
    //    ivrms = hlHelper->fint[ich]->GetRMS();
    rvmean = hlHelper->rpeak[ich]->GetMean(); 
    fvmean = hlHelper->fm[ich]->GetMean();
    //    ivmean = hlHelper->fint[ich]->GetMean();

    Double_t ym = std::max(max(rvmax, fvmax), ivmax);                            
    ymx[ich] = (int)(log10(ym)); ymx[ich] = pow(10., ymx[ich]);
    while(ymx[ich]<ym) ymx[ich] *=2;
    Double_t xm = std::max(max(rvmean+4*rvrms, fvmean+4*fvrms), ivmean+4*ivrms); xmx[ich] = (int)(log10(xm)); xmx[ich] = pow(10., xmx[ich]);
    while(xmx[ich]<xm) xmx[ich] *=2;
    cout<<"chan  "<<ich<<"  rvmax  "<<rvmax<<" fvmax "<<fvmax<<" ivmax "<<ivmax<<" xm "<<xm<<" xmx "<<xmx[ich]<<endl;  
  }
  Double_t xm(0.);
  for (int ich=0; ich<ACTIVECHANNELS; ich++){
    //  just to make x-scales on left and right for the same fiber identical
    hlHelper->rpeak[ich]->SetLineColor(1);    hlHelper->rpeak[ich]->SetMaximum(ymx[ich]);
    hlHelper->fm[ich]   ->SetLineColor(2);    hlHelper->fm[ich]    ->SetMaximum(ymx[ich]);
    hlHelper->fint[ich] ->SetLineColor(4);    hlHelper->fint[ich]  ->SetMaximum(ymx[ich]);
    if(!(ich%2)) xm = max(xmx[ich], xmx[ich+1]);
    cout<<"chan  "<<ich<<" xrange "<<xm<<endl;
    fiberDisplay->cd(ich+1); hlHelper->rpeak[ich]->SetAxisRange(0., xm); hlHelper->rpeak[ich]->Draw();
    hlHelper->fm[ich]->Draw("same");
    //    hlHelper->fint[ich]->Draw("same");
  }
  fiberDisplay->Update();
}

// **************************************************************************
void tileHelper::updateMap(){
  hLabHelper  * hlHelper = hLabHelper ::getInstance();
  int runnumber = hlHelper->runnumber;
  ACTIVECHANNELS  = 0;
  for (int ich = 0; ich<TILECHANNELS;    ich++) hlHelper->active[ich] = runnumber<803?   feech1[ich]   :   feech2[ich];
  ACTIVECHANNELS  += TILECHANNELS;
    for (int ich = 0; ich<TILETRIGGERCH; ich++) {
      if(runnumber<1125)                      {
	hlHelper->active[TILECHANNELS+ich] = -1;
      }      else if(runnumber>=1125&&runnumber<1152) {
	hlHelper->active[TILECHANNELS+ich]    =    trch1125[ich];
	if(hlHelper->active[TILECHANNELS+ich]>=0)  ACTIVECHANNELS ++;
      }      else                                     {
	hlHelper->active[TILECHANNELS+ich] = trch1152[ich];
	if(hlHelper->active[TILECHANNELS+ich]>=0)  ACTIVECHANNELS ++;
      }
    }
}
// **************************************************************************
void tileHelper::updateCalibration(){
  hLabHelper  * hlHelper = hLabHelper ::getInstance();
  int runnumber = hlHelper->runnumber;
  if(runnumber<864) {
    for (int ich = 0; ich<TILECHANNELS; ich++) tileCalib[ich] = sc_779[ich];
  } else if (runnumber<1061)  {
    for (int ich = 0; ich<TILECHANNELS; ich++) tileCalib[ich] = sc_900[ich];
  } else if (runnumber<1123)  {
    for (int ich = 0; ich<TILECHANNELS; ich++) tileCalib[ich] = sc_1061[ich];
  } else {
    for (int ich = 0; ich<TILECHANNELS; ich++) tileCalib[ich] = sc_1123[ich];
  }
}

// **************************************************************************

void tileHelper::evreset(){
  evtsum.reset();
  uSum   = 0.;     cSum   = 0.;
  luSum  = 0.;     lcSum  = 0.;   
  ruSum  = 0.;     rcSum  = 0.;
  YF     = 0.;
  YU     = 0.;     YC     = 0.;
  XU     = 0.;     XC     = 0.;
  sumFU  = 0.;     sumFC = 0.;
  muxU   = -100.;  muxC  = -100.;  muxFU  = -100.;  muxFC  = -100.;
  muxUCh = 0;      muxCCh = 0;     muxUFiber = 0;   muxCFiber = 0;
  for(int ith = 0;   ith<CHANNELTHRESHOLDS; ith++)     hitcnt[ith]  = 0;
  for(int ifb = 0;   ifb<fibers;            ifb++)     fiberLY[ifb] = 0.;
  if(yFit) {delete yFit; yFit = NULL;}
  if(fLY)  {delete fLY;  fLY  = NULL;}
  
}


// **************************************************************************

void tileHelper::tileTrigger(){
  hLabHelper  * hlHelper = hLabHelper ::getInstance();
  for(int ich = 0; ich<detchannels; ich++) { 
    for(int ith = 0; ith<CHANNELTHRESHOLDS; ith++) {
      if(int(hlHelper->rawPeak[ich]/TRIGGERRES) >= ith) hitcnt[ith]++; else break;
    }
  }
  //  trigger hit multiplicity plot
  runsum.trhits -> Fill(uSum/TRIGGERRES);
  for(int ith=0; ith<CHANNELTHRESHOLDS; ith++) {if(hitcnt[ith]) runsum.hits_tpc->Fill(uSum/TRIGGERRES, ith,  hitcnt[ith]) ; else break;}


}

// **************************************************************************

void tileHelper::tileTiming(){
  //  build eventsum (adc sum without pedestal subtruction)
  hLabHelper  * hlHelper = hLabHelper ::getInstance();
  for(int is = 0; is < NSAMPLES; is++){
    evtsum.evtadcsum[is] = 0.;
    for (int ich = 0; ich < detchannels; ich++) { evtsum.evtadcsum[is] += hlHelper->adc[ich][is];}
  }
  for (int is = 0; is < NSAMPLES; is++)  evtsum.evtGraph->SetPoint(is,(Double_t)is,evtsum.evtadcsum[is]);
  fitTileSignal();
  //  do the fit and compare parameter distribution in individual channels and total sum
  //  minimize the number of free parameters and compare timing RMS for "all free" anf "Some constrained"
  ;
}
// **************************************************************************

void tileHelper::tileDisplay(){
  hLabHelper  * hlHelper = hLabHelper ::getInstance();
  //  build eventsum (adc sum without pedestal subtruction)
  for(int is = 0; is < NSAMPLES; is++){ 
    evtsum.evtadcsum[is] = 0.;
    for (int ich = 0; ich < detchannels; ich++) {
      evtsum.evtadcsum[is] += hlHelper->adc[ich][is];
    }
  }
  for (int is = 0; is < NSAMPLES; is++)  evtsum.evtGraph->SetPoint(is,(Double_t)is,evtsum.evtadcsum[is]);
  //  cout<<"<tileDisplay>   call to fitTileSignal"<<endl;
  fitTileSignal();
  TCanvas * tiledisplay = (TCanvas *)(gROOT->FindObject("tiledisplay"));
  if(!tiledisplay) tiledisplay  = new TCanvas("tiledisplay","sPhenix tile display", 300, 300, 800, 500);
  else tiledisplay->Clear();
  //  tiledisplay->Divide(1,2);
  //  tiledisplay->cd(1);
    //  tiledisplay->SetEditable(kTRUE);
  evtsum.evtGraph->SetMarkerStyle(20);
  evtsum.evtGraph->SetMarkerSize(0.4);

  evtsum.evtGraph->Draw("acp");
  tiledisplay->Update();
   //  tiledisplay->cd(2);
  evtsum.evtShape->Draw("same");
  tiledisplay->Update();
  //  tiledisplay->Clear();
}

// **************************************************************************

void tileHelper::tileTriggerDisplay(){
  //  hLabHelper  * hlHelper = hLabHelper ::getInstance();
  int nx_c = ACTIVECHANNELS<=12?   2   :  4;             
  int ny_c = ACTIVECHANNELS/nx_c+ACTIVECHANNELS%nx_c;
  if(!(triggerDisplay=(TCanvas*)(gROOT->FindObject("triggerDisplay")))) {
    TString fD = "Fiber Amplitudes (black - raw, red - fit) Run "; fD += runId.Data();
    //  TString fD = "Fiber Amplitudes (black - raw, red - fit, blue - integral) Run "; fD += hlHelper->prdfName;
    triggerDisplay = new TCanvas("triggerDisplay",fD,400*nx_c,200*ny_c);
    triggerDisplay->Divide(nx_c, ny_c);
  }
}
// **************************************************************************

//  let's now see if all this game with patterning was worth trying
void tileHelper::tilePattern(Int_t nx, Int_t ny, Int_t run, Int_t mod){
  TH2  * pcPat(NULL);           //  pattern of pixel conts measured in different tile areas
  TH1 ** pcPatH(NULL);          //  Collections of  hits from different tile areas 
//TF1 ** pcPatF(NULL);          //  Fits to the histograms of hits in different tile areas
  xdivisions            = nx;
  ydivisions            = ny;
  hLabHelper  * hlHelper = hLabHelper ::getInstance();
  TFile *f;
  if(!(f=hlHelper->attachrootrun(run))) {
    cout<<"<ERROR> pattern(): root file for runNumber "<<hlHelper->runnumber<<" not in the list"<<endl;
    return;
  }
  f->cd();
  gStyle         ->SetOptFit();
  // hn         = "Total Pixel Count vs impact position  RunId = "; hn += Id;      hn += ";Calibated X [cm];Calibrated Y [cm];Pixels";
  // pc_imp     = new TH3F("pc_imp",  hn,   25, 0., 25., 15, 0., 15., 1000, 0, 200.);  
  if((lyPattern  = (TCanvas*)(gROOT->FindObject("lyPattern")))) delete lyPattern;
  if((lyFits     = (TCanvas*)(gROOT->FindObject("lyFits"))))    delete lyFits;

  TString lyp = "Pattern of light yield "; lyp += runId.Data();
  TString lyd = "LYD "; lyd += runId.Data();
  lyPattern = new TCanvas(lyd,   lyp, 400, 400);
  TString lyv = "LIV "; lyv += runId.Data();
  TString lyf = "Fits to the measurenments of light yield "; lyf += (mod==0? "(pc_imp) " : "(pc_fimp)"); lyf += runId.Data();
  lyFits    = new TCanvas("lyv", lyf, 200*nx, 200*ny);
  lyFits    ->Divide(xdivisions, ydivisions);

  TH3 * pcimp = (TH3F *) (gROOT->FindObject(mod==0? "pc_imp" : "pc_fimp"));
    //  TH3 * pc_imp = (TH3F *) (gROOT->FindObject("pc_imp"));
  if(!pcimp) {
    cout<<"<ERROR> pattern(): "<<(mod==0? "(pc_imp) " : "(pc_fimp)")<<" Scatterplot is not found"<<endl;
    return;
  }
  Int_t nximp  = pcimp->GetNbinsX();                Int_t    nyimp = pcimp->GetNbinsY(); 
  Int_t nzimp  = pcimp->GetNbinsZ();
  //  Double_t dx  = pcimp->GetXaxis()->GetBinWidth(1); Double_t dy    = pcimp->GetYaxis()->GetBinWidth(1);
  Double_t dxr = tileSizeX / nx;                     Double_t dyr   = tileSizeY / ny;
  //  total number of cells to combine 
  Int_t dnx    = nximp/nx;                              Int_t    dny   = nyimp/ny;
  if(dnx*nx!=nximp || dny*ny!=nyimp) {
    cout<<"Patterning request does not match your binning in the pixel count distribution pcimp"<<endl;
    return;
  }
  pcPat = (TH2F *) (gROOT->FindObject("pcPat"));
  if(pcPat) delete pcPat;
  pcPat       = new TH2F("pcPat", " fitted positions of maxima in pcimp pixel distributions", nx, 0., 25., ny, 0., 15.);
  pcPatH      = new TH1 * [nx*ny];
  for (int iy = 0; iy<ny; iy++){
    for (int ix = 0; ix<nx; ix++){
      Int_t  cpad = nx*(ny-iy-1) + ix +1;
      TString hn = "h_"; hn += ix; hn += "_"; hn += iy;
      TString ht = "Pixel count. Range X = "; ht += (dxr*ix);  ht += " | ";  ht += (dxr*(ix+1)); 
      ht += " Y = ";  ht += (dyr*iy);  ht += " | "; ht += (dyr*(iy+1));
      if((pcPatH[iy*nx+ix]=(TH1 *) (gROOT->FindObject(hn)))) delete pcPatH[iy*nx+ix];
      pcPatH[iy*nx+ix] = new TH1D(hn, ht, nzimp, 0., pcimp->GetZaxis()->GetXmax()); 
      //  fill that histogramm
      pcimp->ProjectionZ(hn, 1+dnx*ix, dnx*(ix+1), 1+dny*iy, dny*(iy+1));
      // draw that histogramm
      lyFits          -> cd(cpad);
      pcPatH[iy*nx+ix]-> Draw();
      // check for total number of entries in the histogram, if less then minimum - go to next one
      Int_t   entries = pcPatH[iy*nx+ix]->GetEntries();
      if(entries<minProjEntries) continue;
      //  rebin the histogramm to get at least 50 hits at the peak
      Double_t norm;
      while (((norm=pcPatH[iy*nx+ix]->GetMaximum())<50.||
	      pcPatH[iy*nx+ix]->GetBinCenter(pcPatH[iy*nx+ix]->GetMaximumBin())<=10)&&
	     pcPatH[iy*nx+ix]->GetNbinsX()>50 ) {
	Int_t combine = 2; 
	while((pcPatH[iy*nx+ix]->GetNbinsX()/combine)*combine!=pcPatH[iy*nx+ix]->GetNbinsX()) combine++;
	pcPatH[iy*nx+ix] = pcPatH[iy*nx+ix]->Rebin(combine);
      }
      //  redrow the histogramm
      pcPatH[iy*nx+ix]-> Draw();
      //  fit that histogramm
      Double_t mean = pcPatH[iy*nx+ix]->GetMean();
      Double_t rms  = pcPatH[iy*nx+ix]->GetRMS();
      Double_t p[6];
      p[0]    = norm;  p[1]  = mean;  p[2]  = rms;  p[3] = 0.;  p[4] = 0.;  p[5] = 0.;
      TF1 * fg  = new TF1("fg","[0]*TMath::Gaus(x,[1],[2])", 15., pcimp->GetZaxis()->GetXmax());
      fg       -> SetParameters(p);
      fg       -> SetParLimits(0, 0.1*norm,10.*norm);
      fg       -> SetParLimits(1, mean-rms, mean+rms);
      fg       -> SetParLimits(2, rms/2., 2.*rms);
      pcPatH[iy*nx+ix]->Fit(fg, "r");
      fg       ->GetParameters(p);
      TF1 * fgb = new TF1("fgb","[0]*TMath::Gaus(x,[1],[2])+[3]+[4]*x+[5]*x*x", 15., pcimp->GetZaxis()->GetXmax());
      fgb      ->SetParameters(p);
      fgb      ->SetParLimits(1, p[1]-rms,p[1]+rms);
      fgb      ->SetParLimits(2, rms/2,2.*rms);
      fgb      ->SetParLimits(3, 0., p[0]);
      pcPatH[iy*nx+ix]->Fit(fgb, "r");
      lyFits          ->Update();
      fgb       ->GetParameters(p);
      //  fill the patten 2D plot
      pcPat->Fill(dxr*(ix+0.5), dyr*(iy+0.5), p[1]);      
    }
  }
  lyPattern->cd();
  pcPat->Draw("lego2");
}

// **************************************************************************

void tileHelper::collectTileSummary(){
  hLabHelper  * hlHelper = hLabHelper ::getInstance();
  //  tile specific data
  for (int ich=0;  ich<detchannels;  ich++) {
    tileCalPeak[ich]      = hlHelper->fitPeak[ich]/tileCalib[ich];
    evtsum.times[ich]     = hlHelper->fitTime[ich];
    evtsum.chi2[ich]      = hlHelper->fitChi2[ich];
    uSum  += hlHelper->rawPeak[ich];
    cSum  += tileCalPeak[ich];
    runsum.cdata ->Fill(ich, tileCalPeak[ich]);
    if(ich%2) {

      // right ends
      ruSum += hlHelper->rawPeak[ich];
      rcSum += tileCalPeak[ich];
    } else {
      // lefy ends
      luSum += hlHelper->rawPeak[ich];
      lcSum += tileCalPeak[ich];
    }
  }
  //  Maximum amplitude in detector
  for (int ich=0; ich<detchannels; ich++){
    if(hlHelper->rawPeak[ich] >=muxU)   {muxUCh = ich; muxU = hlHelper->rawPeak[ich];}
    if(tileCalPeak[ich]       >=muxC)   {muxCCh = ich; muxC = tileCalPeak[ich];}
    if(ich%2) { 
      // hard wired: fiber Y's in the tiles
      int fiber = ich/2;
      float y   = fiberY[fiber];
      sumFU    += hlHelper->rawPeak[ich]; sumFC += tileCalPeak[ich];
      YU       += y*sumFU;      YC    += y*sumFC;
      if(sumFU >= muxFU) {muxUFiber = ich/2; muxFU = sumFU;}
      if(sumFC >= muxFC) {muxCFiber = ich/2; muxFC = sumFC;}
      fiberLY[ich/2]  = sumFC;
    } else {
      sumFU =  hlHelper->rawPeak[ich]; sumFC  = tileCalPeak[ich];
    }
  }
  YU /= uSum;   YC /= cSum;  YF = getYFit();
  evtsum.hitfiber   = muxCFiber;
  evtsum.hitfiberpc = muxFC;
  evtsum.yChi2      = yFit->GetChisquare()/yFit->GetNDF();
  evtsum.ySigma     = yFit->GetParameter(2);
  runsum.rmax  -> Fill(muxUCh, muxU);  runsum.cmax  -> Fill(muxCCh, muxC);
  runsum.rfsum -> Fill(muxUFiber, muxFU);
  runsum.cfsum -> Fill(muxCFiber, muxFC);
  runsum.rtsum -> Fill(muxUFiber, uSum);
  runsum.ctsum -> Fill(muxCFiber, cSum);
  ras   = ((hlHelper->rawPeak[muxUFiber*2+1]-hlHelper->rawPeak[muxUFiber*2])  /(hlHelper->rawPeak[muxUFiber*2]+hlHelper->rawPeak[muxUFiber*2+1]));
  cas   = ((tileCalPeak[muxCFiber*2+1]-tileCalPeak[muxCFiber*2])/(tileCalPeak[muxCFiber*2]+tileCalPeak[muxCFiber*2+1]));
  runsum.rfasym-> Fill(muxUFiber, ras);
  runsum.cfasym-> Fill(muxCFiber, cas);
  rtas  = (ruSum - luSum)  /(ruSum + luSum);
  ctas  = (rcSum - lcSum)  /(rcSum + lcSum);
  runsum.rtasym-> Fill(rtas);
  runsum.ctasym-> Fill(ctas);
  XU = 12.5+12.5*rtas; /* XC = 12.5+12.5*ctas; */   XC = AsymToX(ctas);
  runsum.XY    -> Fill(XC, YF);
  runsum.ry    -> Fill(YU);
  runsum.cy    -> Fill(YC);
  runsum.fy    -> Fill(YF);
  runsum.rx    -> Fill(XU);
  runsum.cx    -> Fill(XC);
  runsum.rimp  -> Fill(XU, YU);
  runsum.cimp  -> Fill(XC, YC);
  runsum.cx_pc -> Fill(XC, cSum);
  runsum.cy_pc -> Fill(YC, cSum);
  runsum.fy_pc -> Fill(YF, cSum);
  runsum.cimpW -> Fill(XC, YC, cSum);
  for(int ith=0; ith<CHANNELTHRESHOLDS; ith++) {if(hitcnt[ith]) runsum.treff->Fill(hitcnt[ith], ith, cSum) ; else break;}
  runsum.pc_imp ->Fill(XC, YC, cSum);
  runsum.pc_fimp->Fill(XC, YF, cSum);
  evtsum.evtpc      = cSum;
  evtsum.hitfibertd = evtsum.times[muxCFiber*2+1]-evtsum.times[muxCFiber*2];
  //  cout<<"<collectTileSummary> YF/tChi2/yChi2  "<<YF<<" * "<<evtsum.tChi2<<" * "<< evtsum.yChi2<<endl;
  runsum.y_yChi2 -> Fill(YF,           evtsum.yChi2);
  runsum.s_y_Chi2-> Fill(evtsum.tChi2, evtsum.yChi2);
  reject();
  runsum.trcode  -> Fill(evtsum.rejectCode);
  runsum.y_ys    -> Fill(YF,           evtsum.ySigma);
  if(evtsum.ySigma<4.5) {
    runsum.yCleaned -> Fill(YF);
    runsum.yc_rcode -> Fill(YF,evtsum.rejectCode); 
    if(!evtsum.rejectCode<100) runsum.yKept -> Fill(YF);
  }
  if(ACTIVECHANNELS>=TILECHANNELS+2){
    Double_t tower_pc = hlHelper->fitPeak[9]/0.05;     //  pixels in the tower
    runsum.td_r_l   -> Fill(evtsum.hitfibertd, evtsum.hitfiberpc);
    runsum.td_t_tw  -> Fill(evtsum.evttime-hlHelper->fitTime[9], evtsum.evtpc, tower_pc);
  }
  tileTrigger();
}

// **************************************************************************

Double_t tileHelper::getYFit(){
  //  Let's do Y first
  yFit = new TF1("yFit", "[0]*exp(-abs(x-[1])/[2])", 0., tileSizeY);
  Double_t    par[]  =  {20., tileSizeY/2., 1.};
  yFit -> SetParameters(par);
  yFit -> SetParLimits(1, 0., tileSizeY);
  yFit -> SetParLimits(2, 0., 5.);
  fLY  = new TGraph(fibers, fiberY, fiberLY); 
  fLY -> Fit(yFit, "rq", "nq", 0., tileSizeY);
  yFit->GetParameters(par);
  runsum.Y0 -> Fill(par[0]);
  runsum.Y1 -> Fill(par[1]);
  runsum.Y2 -> Fill(par[2]);
  
  return par[1];
}

// **************************************************************************

void tileHelper::tileImpact(){
  //  Let's do Y first
  yFit = new TF1("yFit", "[0]*exp(-abs(x-[1])/[2])", 0., tileSizeY);
  Double_t    par[]  =  {20., tileSizeY/2., 1.};
  yFit -> SetParameters(par);
  yFit -> SetParLimits(1, 0., tileSizeY);
  yFit -> SetParLimits(2, 0., 4.);
  fLY  = new TGraph(fibers, fiberY, fiberLY); 
  fLY -> Fit(yFit, "rq", "nq", 0., tileSizeY);
  yFit->GetParameters(par);
  runsum.Y0 -> Fill(par[0]);
  runsum.Y1 -> Fill(par[1]);
  runsum.Y2 -> Fill(par[2]);
  //  Now X
  Float_t slope(0.);
  Float_t dsl  = 0.5/20;
  for (int isl = 0;  isl< 20; isl++){
    slope = dsl/2.+dsl*isl;
    Double_t X = 12.5*(1 + ctas/slope); 
    runsum.XvsSl->Fill(X, slope);
    
  }
}
// **************************************************************************

void tileHelper::fitTileSignal(){
  //  zero approximation to fit parameters
  //  use pulse data to find location of maximum
  //  hLabHelper  * hlHelper = hLabHelper ::getInstance();
  int      rTime = 0;
  Double_t rVal  = 0.;
  for (int iSample = RISETIME-1; iSample <= NSAMPLES-FALLTIME; iSample++) {
    if(evtsum.evtadcsum[iSample]>rVal) {
      rVal  = evtsum.evtadcsum[iSample];
      rTime = iSample;    
    }
  }
  rVal         -= PEDESTAL*detchannels;
  if(rVal<0.) rVal = 0.;
  Double_t  par0[6];
  par0[0] = rVal/3.;
  par0[1] = max(0.,(Double_t)(rTime-RISETIME));
  par0[2] = 4.;
  par0[3] = 1.6;
  par0[4] = PEDESTAL*detchannels;  //   we do not do pedestal subtrastion at this time
  par0[5] = 0;      //   slope of the pedestals is initially set to "0"
  evtsum.evtShape->SetParameters(par0);
  for(int parn=0; parn<6; parn++) evtsum.evtShape->SetParLimits(parn, par0Min[parn], par0Max[parn]);
  evtsum.evtShape->SetParLimits(4, PEDESTAL*(detchannels-1), PEDESTAL*(detchannels+1));
  //  fitFunc->cd(chan+1);  //  shapes[chan]->Draw();
  //  gPad->Modified();
  evtsum.evtGraph->Fit(evtsum.evtShape, "RQWM", "NQ", 0., (Double_t)NSAMPLES);
  evtsum.evtShape->GetParameters(evtsum.evtfitpar);
  evtsum.evtpedestal  = (evtsum.evtfitpar[4]+evtsum.evtfitpar[5]*rTime);   //  
  evtsum.evtSignal->SetParameters(evtsum.evtfitpar);
  evtsum.evtSignal->SetParameter(4, 0.);
  evtsum.evtSignal->SetParameter(5, 0.);
  evtsum.evtpeak  = evtsum.evtSignal->GetMaximum(par0Min[1], par0Max[1]);
  evtsum.evttime  = evtsum.evtSignal->GetMaximumX(par0Min[1], par0Max[1]);
  evtsum.tChi2    = evtsum.evtShape->GetChisquare()/evtsum.evtShape->GetNDF();
  //  cout<<"<fitTileSignal>  Peak  "<< evtsum.evtpeak<<"  Time "<<evtsum.evttime<<"  Chi2 "<<evtsum.tChi2 <<endl;
  //  ev_display->cd(chan+1);
  //  shapes[hlHelper->chan]->Draw("same");

}

// **************************************************************************

tileHelper::runtilesummary::runtilesummary(){
  hLabHelper  * hlHelper = hLabHelper ::getInstance();
  hlHelper-> fhcl-> cd();
  TString hn         = "Raw data: Events peaking in channel  RunId = ";  hn += runId;      hn += ";Channel #;ADC Peak (events with max in this channel) [counts]";
  rmax       = new TH2F("rmax",    hn,   TILECHANNELS, 0, TILECHANNELS,   2050, 0., 2050.);
  hn         = "Total Pixel Count with tighter triggering  RunId = "; hn += runId; hn += ";HitMultThr;HitAmplThr [trigger counts];Pixels";
  treff      = new TH3F("treff",   hn, HITMULTTHRESHOLDS, 0, HITMULTTHRESHOLDS, CHANNELTHRESHOLDS, 0, CHANNELTHRESHOLDS, 1000, 0., 200.); 
  hn         = "Total Pixel Count vs impact position  RunId = "; hn += runId;      hn += ";Calibated X [cm];Calibrated Y [cm];Pixels";
  pc_imp     = new TH3F("pc_imp",   hn,   250, 0., 25., 150, 0., 15., 1000, 0, 200.);  
  hn        += " Y Fitted";
  pc_fimp    = new TH3F("pc_fimp",  hn,   250, 0., 25., 150, 0., 15., 1000, 0, 200.);  
  hn         = "Fit to the Pixel Count vs impact position  RunId = "; hn += runId; hn += ";Calibated X [cm];Calibrated Y [cm];Pixels";
  pc_pat     = new TH2F("pc_pat",  hn,   25, 0., 25., 15, 0., 15.);  
  hn        += " Y Fitted";  
  pc_fpat    = new TH2F("pc_fpat", hn,   25, 0., 25., 15, 0., 15.);  
  hn         = "Raw Fiber sum (R+L), Peak in this fiber  RunId = ";   hn += runId; hn += ";Fiber #;Fiber ADC Sum (events with max in this fiber) [counts]";
  rfsum      = new TH2F("rfsum",   hn,   TILEFIBERS, 0, TILEFIBERS, 2050, 0., 2050.);  
  hn         = "Raw Total sum, Peak in this fiber  RunId = ";  hn += runId;        hn += ";Fiber #;Event ADC Sum (events with max in this fiber) [counts]";
  rtsum      = new TH2F("rtsum",   hn,   TILEFIBERS, 0, TILEFIBERS, 2050, 0., 2050.); 
  hn         = "Raw Fiber Asymmetry  RunId = ";  hn += runId;                      hn += ";Fiber #;Fiber R/L Asymmetry";
  rfasym     = new TH2F("rfasym",  hn,   TILEFIBERS, 0, TILEFIBERS, 200, -1., 1.);  
  hn         = "Raw Event Asymmetry  RunId = ";  hn += runId;                      hn += ";Event Raw R/L Event Asymmetry";
  rtasym     = new TH1F("rtasym",  hn,   200, -1., 1.);
  hn         = "Raw X  RunId = ";   hn += runId;                                   hn += ";Event Raw X-coordinate [cm]";
  rx         = new TH1F("rx",      hn,   250, 0., 25.);
  hn         = "Raw Y  RunId = ";   hn += runId;                                   hn += ";Event Raw Y-coordinate [cm]";
  ry         = new TH1F("ry",      hn,   150, 0., 15.);
  hn         = "Raw impact position  RunId = ";  hn += runId;                      hn += ";Raw X [cm];Raw Y [cm]";
  rimp       = new TH2F("rimp",    hn,   250,  0., 25., 150,  0., 15.); 

  hn         = "Calibrated data: Fitted Maxima  RunId = ";  hn += runId;           hn += ";Channel #; Calibrated Fit Peak [pixels]";
  cdata      = new TH2F("cdata",   hn,   TILECHANNELS, 0, TILECHANNELS,   1000, 0., 200.);
  hn         = "Calibrated data: Events peaking in channel  RunId = ";  hn += runId;  hn += ";Channel #;Calibrated Fit Peak (evens with max in this channel) [pixels]";
  cmax       = new TH2F("cmax",    hn,   TILECHANNELS, 0, TILECHANNELS,   1000, 0., 200.);
  hn         = "Calibrated Fiber sum (R+L), Peak in the fiber  RunId = ";  hn += runId;  hn += ";Fiber #;Calibrated Fiber Sum (events with max in this fiber) [pixels]";
  cfsum      = new TH2F("cfsum",   hn,   TILEFIBERS, 0, TILEFIBERS, 1000, 0., 200.);    
  hn         = "Calibrated Total sum, Peak in this fiber  RunId = ";  hn += runId;    hn += ";Fiber #;Calibrated Total Sum (events with max in this fiber) [pixels]";
  ctsum      = new TH2F("ctsum",   hn,   TILEFIBERS, 0, TILEFIBERS, 1000, 0., 200.);  
  hn         = "Calibrated Fiber Asymmetry  RunId = ";  hn += runId;               hn += ";Fiber #;Fiber R/L Asymmetry (calibrated)";
  cfasym     = new TH2F("cfasym",  hn,   TILEFIBERS, 0, TILEFIBERS, 200, -1., 1.);  
  hn         = "Calibrated Event Asymmetry  RunId = ";  hn += runId;               hn += ";Event R/L Event Asymmetry (calibrated)";
  ctasym     = new TH1F("ctasym",  hn,   200, -1., 1.);
  hn         = "Calibrated X  RunId = ";   hn += runId;                            hn += ";Calibrated X [cm]";
  cx         = new TH1F("cx",      hn,   250, 0., 25.);
  hn         = "Calibrated Y  RunId = ";   hn += runId;                            hn += ";Calibrated Y [cm]";
  cy         = new TH1F("cy",      hn,   150, 0., 15.);
  hn         = "Fitted Y  RunId = ";   hn += runId;                                hn += ";Fitted Y [cm]";
  fy         = new TH1F("fy",      hn,   150, 0., 15.);
  hn         = "Calibrated Impact Position  RunId = ";  hn += runId;               hn += ";Calibrated X [cm];Calibrated Y [cm]";
  cimp       = new TH2F("cimp",    hn,   250,  0., 25., 150,  0., 15.); 
  hn         = "Calibrated Pixel Count (Y) vs X-impact  RunId = ";  hn += runId;   hn +=";Calibrated X [cm];Calibrated Total Sum [pixels]";
  cx_pc      = new TH2F("cx_pc",   hn,   250,  0., 25., 1000, 0., 200.);
  hn         = "Calibrated Pixel Count (Y) vs Y-impact  RunId = ";  hn += runId;   hn +=";Calibrated Y [cm];Calibrated Total Sum [pixels]";
  cy_pc      = new TH2F("cy_pc",   hn,   150,  0., 15., 1000, 0., 200.);
  hn         = "Calibrated Pixel Count (Y) vs Fitted Y-impact  RunId = ";  hn += runId;   hn +=";Fitted Y [cm];Calibrated Total Sum [pixels]";
  fy_pc      = new TH2F("fy_pc",   hn,   150,  0., 15., 1000, 0., 200.);
  hn         = "Calibrated Weighted Impact Position  RunId = ";  hn += runId;      hn +=";Calibrated X [cm];Calibrated Y [cm]";
  cimpW      = new TH2F("cimpW",   hn,   250,  0., 25., 150,  0., 15.); 
  
  //  Trigger related data
  //  Pixel count above threshold vs total pixel count (based on calibrated data)
  hn         = "Uncalibrated trigger sum [trigger counts]  RunId = ";  hn += runId; hn += runId; hn += ";Trigger hits;";
  trhits     = new TH1F("trhits",  hn, 500, 0., 500.);
  hn         = "Number of hits in triggered events vs channel threshold(Y) and total trigger sum (X)  RunId = ";  hn += runId; hn += ";TotalSum [trigger counts];Trigger THr [trigger counts]";
  hits_tpc   = new TH2F("hits_tpc",hn, 500, 0., 100., CHANNELTHRESHOLDS, 0, CHANNELTHRESHOLDS); 

  //  
  //   debugging muon impact computations
  //  TH2        * XvsSl, * XY;     //  X value vs assumption about slope in the X vs Asymmetry 
  //  TH1        * y0, *y1, *y2;    //  Parameters from the tile-Y fit
  Y0       = new TH1F("Y0", "Y-coordinate fit P(0)", 200, 0., 200.);
  Y1       = new TH1F("Y1", "Y-coordinate fit P(1)", 100, 0., 15.);
  Y2       = new TH1F("Y2", "Y-coordinate fit P(2)", 100, 0., 10.);
  y_yChi2  = new TH2F("y_yChi2",  "yFit: Chi2 at different fitted Y values", 150,0.,15., 100,0.,50.);
  s_y_Chi2 = new TH2F("s_y_Chi2", "Chi2 of tile signal vs Chi2 of Y coordinate", 100, 0., 500., 100, 0., 50.);
  y_ys     = new TH2F("y_ys",     "yFit sigma vs fitted Y", 150,0.,15. ,100, 0., 5.);
  yCleaned = new TH1F("yCleaned", "Fitted Y after event rejection (fit sigma <4.5)", 150,0.,15.);
  yKept    = new TH1F("yKept",    "Fitted Y after event rejection (s<4.5, rejectcode=0)", 150,0.,15.);
  XvsSl    = new TH2F("XvsSl",   "X-Coordinate computed using different asymmentry slopes", 275, -15., 40., 20, 0., 0.5);
  XY       = new TH2F("XY",      "Muon Impact:  x  vs  Y. Fit Y vs Best X", 225, -15., 40., 75, 0., 15.);
  td_r_l   = new TH2F("td_r_l",  "R/L Timing difference in hit fiber vs PixelCount in Hit Fiber", 200, -1, 1., 200, 0., 100.);
  td_t_tw  = new TH3F("td_t_tw", "Tile-to-tower time difference(X) vs Pixel Counts in tile(Y) and tower(Z)", 200, -1. ,1., 100, 0., 100., 100, 0., 100.);
  trcode   = new TH1F("trcode",  "Tile reject codes ", 1000, 0., 1000.);
  yc_rcode = new TH2F("yc_rcode","Tile reject code vs Cleaned Y ", 150, 0., 15., 1000, 0., 1000.);

}
// **************************************************************************

Int_t tileHelper::reject(){
  //  test fit results in individual channels, compare to fit of the tile sum
  //  hLabHelper  * hlHelper = hLabHelper ::getInstance();
  hLabHelper * hlHelper = hLabHelper::getInstance();
  evtsum.rejectCode = 0;
  for(int ich = 0; ich< TILECHANNELS; ich++) {if(evtsum.chi2[ich]>10.) evtsum.rejectCode ++;}
  for(int ich = 0; ich< TILECHANNELS; ich++) {if(abs(evtsum.times[ich]-evtsum.evttime)>2.) evtsum.rejectCode += 10;}
  for(int ich = 0; ich< TILECHANNELS; ich++) {if(hlHelper->rawPeak[ich]<2.) evtsum.rejectCode += 100;}
  return  evtsum.rejectCode;
}


// **************************************************************************

tileLightYield::tileLightYield(){
  hLabHelper  * hlHelper = hLabHelper ::getInstance();
  hlHelper->getFileLists();
  //  rootfile * rfile;
  list<rootfile>::iterator    next;
  for (next = hlHelper->roots.begin(); next != hlHelper->roots.end(); ++next){
    ;
  }
}
