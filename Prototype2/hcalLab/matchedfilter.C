///////////////////////////////////////////////////////////////////////////////
// ROOT macro for simulating the signal of different shapes, superimposed with the 
// gaussian noise and to process resulting sequence using matching filter, 
// extracting amplitude and time of the signal.
//
// Usage:
// To load the tools
//	.L matchedfilter.C
// To generate signal with unipolar shape with 16 samples and calculate corrections
//	 init(16,0)
// The filter coefficients can be manually modified using, for example:
//	 g[2] = 0.5;	// to set filter coeffcient 2 to 0.5
// To generate signal and noise, 
// for example signal positioned at sample 50 with amplitude 100 and rms noise 10:
//	 series(50.,10.,100);
//
#define    PRECISION_COEFF 1./64. // Precision of the filter coefficients. 1/64 is good enough
//#define    PRECISION_SAMPLING 1.e-9 // High precision of the ADC sampling
//#define    PRECISION_SAMPLING 1./10. // 14-bit precision of the ADC sampling

#define    SMAL_UNDEF 1.0e-99
#define    ALevel 100.0               // Amplitude level

#define    NNX   400
Int_t      NX  = NNX;
Double_t   x[NNX],y[NNX],yma[NNX],dy[NNX],dyma[NNX],s[NNX],v[NNX],ni[NNX];
//Double_t y_s[NNX];
Double_t   gmax, gmaxx;
Double_t   tau  =0.;
Double_t   ymax =0.;
Int_t      quiet = 0;
#define    NN    128;
Int_t N ;                             // filter length, set during initialization
Double_t   fN;                        // widely used (Double_t)N;
Double_t   g[NN], gma[NN];
Double_t   norm,  norm_ma;
Double_t   gpos,  gnoise;
Int_t      npars  = 2;
Double_t   pars[] = {1.,0.,0,0};
#define    grf_NPTS 15
#define    grf_NY 8
Double_t   serY[grf_NY];
Double_t   grf_x[grf_NPTS] = {2000.,1000.,500.,200.,80.,60.,40.,30.,20.,10.,7.,5.,4.,2.5,2.0};
Double_t   grf_y[grf_NY][grf_NPTS];
Double_t   grf_ey[grf_NY][grf_NPTS];
Double_t   grf_ex[grf_NPTS];
Char_t   * grf_name[grf_NY] = 
  {	
    "T/SigmaT, T/ErrorT",
    "A/SigmaA, A/ErrorA",
    "Mean(Time)",
    "Mean(A)",
    "1/(Systematic Error) of Time",
    "1/(Systematic Error) of Amplitude",
    "T/StDev(Time)",
    "A/StDev(A)"
  };
TGraph   * gr     = 0;
TF1      * gf;

#define NCorr 256 //number of correction points
Double_t   serCorr[2][NCorr];
Double_t   aCorr[NCorr],tCorr[NCorr];
TCanvas  * gCnv=NULL;

//shapes
Int_t      SigShape;
#define    UNIPOLAR 0
#define    BIPOLAR 1
#define    RECTANGULAR 2
#define    GAUSS 3
#define    STRIANGLE 4 //symmetric triangle
#define    RTRIANGLE 5 //rigth triangle
#define    UTRIANGLE 6 
#define	   UNKNOWN_SHAPE 7

#define    NEED_FILTER_NOISE            // define when you are interested in the noise output of the filter

//#define  TRY_WEIGHTED                 // to try weighted average finding of the max of the filtered signal
#ifdef TRY_WEIGHTED
Double_t tauw=0.;
#endif

// **********************************************************************************************

TCanvas *gCanv[5];
void init(UChar_t nsamples=16, UChar_t sigshape=0)
{
// This macro loads necessary functions and initialize the system.
// To simulate the series of measurements:
// 	root[] doSeries(pos, ntry);
// where the 'pos' is the position of the pulse in sample numbers
// the ntry is number of tries in each series
	Int_t ii;
	TCanvas *canv;

	gCanv[0] = new TCanvas();
	gCanv[1] = new TCanvas(); 
	gCnv=gCanv[1]; //standard canvas for show 
	gCanv[2] = new TCanvas(); 
	gCanv[3] = new TCanvas(); 
	gCanv[4] = new TCanvas(); 
	gCanv[5] = new TCanvas(); 
	
	// consistency check
	if(sigshape >= UNKNOWN_SHAPE)
		{cout<<"Unknown shape, >= "<<UNKNOWN_SHAPE<<"\n"; exit(1);}
	SigShape = (Int_t) sigshape;
	if(nsamples > NN)
		{cout<<"The filter length too long, >= "<<NN<<"\n"; exit(1);}
	N = (Int_t)nsamples;
	fN = (Double_t)N;

	cout<<"Preparing simulation of ";
	switch (SigShape)
	{
		case UNIPOLAR: cout<<"UNIPOLAR"; break;
		case BIPOLAR: cout<<"BIPOLAR"; break;
		case RECTANGULAR: cout<<"RECTANGULAR"; break;
		case GAUSS: cout<<"GAUSS"; break;
	}
	cout<<" shape["<<N<<"]\n";

	for(ii=0;ii<NX;ii++) ni[ii] = (Double_t) ii;
	
	getCoeffs();

	cout<<"Round coefficients to precision "<<PRECISION_COEFF<<endl;
	roundCoeffs(PRECISION_COEFF);

	getCorrections();

	showShape();
	
	showAutocorrelation();
		
	#ifdef NEED_FILTER_NOISE
	cout<<"Attention, NEED_FILTER_NOISE is active, it slows down the calculations\n";
	#endif
	cout<<"The filter coefficients g[i] can be modified now."<<endl;
	cout<<"If they have been modified, execute:  showShape(); getCorrections();"<<endl;
	cout<<"To simulate, execute: doSeries(pos,ntry)\n";
}

// **********************************************************************************************

Double_t shape(Double_t xx)
{
	return fshape(&xx,pars);
}

// ----------------------------------------------------------------------------------------------
Double_t fshape(Double_t *xx, Double_t *par)
{
    //Double_t r;
    Double_t x1 = xx[0] - par[1],v1,v;
    if(x1 < 0.) x1 = 0.;
	switch (SigShape)
	  {
	  case UNIPOLAR:
	    // Landau-like
	    return par[0]*pow(x1,4.)*(exp(-x1*16./fN));    //  fN is set to (float)NSAMPLES
0	  case BIPOLAR:
	    // bipolar shape, simply derivative of the UNIPOLAR
	    return par[0]*pow(x1,3.)*exp(-x1*16./fN) * (4.-16./fN*x1);
	  case RECTANGULAR:// symmetrical trapezoidal
	    v1 = 1.;
	    if(x1<v1) v = par[0]*x1/v1;
	    else if(x1<=fN-v1) v = par[0];
	    else v = par[0]*(fN-x1)/v1;
	    return v;
	  case GAUSS:
	    x1 -= fN/2.;
	    return par[0]*exp(-(x1*x1)/1.);
	  case STRIANGLE: // symmetrical triangle
	    return (x1<fN/2.) ? par[0]*x1 : par[0]*(fN-x1);
	  case RTRIANGLE:
	    v1 = 1.;
	    return (x1<v1) ? par[0]*(fN-v1)*x1/v1 : par[0]*(fN-x1);
	  case UTRIANGLE: // asimmetrical triangle
	    v1 = 1.1;
	    return (x1<v1) ? par[0]*(fN-v1)*x1/v1 : par[0]*(fN-x1);
	  default:
	    cout<<"Unknown shape\n";
	    exit(1);
	  }
}

// ----------------------------------------------------------------------------------------------

Double_t fshapeACorr(Double_t *xx, Double_t *par)
{// AutoCorrelation at shift par[0]
	Double_t par0[10],vv;
	for(Int_t ii=0;ii<10;ii++) par0[ii] = par[ii];
	par0[2] = par[0]; // the shift parameter for fshape
	vv = fshape(xx,par0+1)*fshape(xx,par+1);
	//cout<<"fshapeACorr("<<*xx<<","<<par[0]<<")="<<vv<<"\n";
	return vv;
}

// **********************************************************************************************

// ----------------------------------------------------------------------------------------------

// first derivative of the unipolar shape
Double_t difup(Double_t *xx, Double_t *par)
{
    Double_t x1 = xx[0] - par[1];
    if(x1 < 0.) x1 = 0.;
	return par[0]*pow(x1,3.)*exp(-x1*16./fN) * (4.-16./fN*x1);
}

// ----------------------------------------------------------------------------------------------

// **********************************************************************************************

void show(Double_t *xx, Int_t nn=200, Char_t *xname=NULL, Char_t *yname=NULL, Int_t lcolor=kBlue, Int_t mstyle=kStar, Double_t msize=0.75)
{
	gr = new TGraph(nn,ni,xx);
	gr->SetLineColor(lcolor);
	gr->SetLineWidth(2);
	gr->SetMarkerColor(kBlack);
	gr->SetMarkerStyle(mstyle);
        gr->SetMarkerSize(msize);
	gr->SetTitle("");
	if (xname) {gr->GetXaxis()->SetTitle(xname);}
	else gr->GetXaxis()->SetTitle("Sample");
	//if (yname) {gr->GetYaxis()->SetTitle(yname);}
	if (yname) {gr->SetTitle(yname);}
	//gr->Draw("ACP");
	gr->Draw("APL");
	if(gCnv == NULL) gCnv = c1;
	gCnv->SetGridx(); gCnv->SetGridy();
	gCnv->SetCrosshair();gCnv->SetTickx(); gCnv->SetTicky();
	gCnv = NULL;
}

// ----------------------------------------------------------------------------------------------

void showsig(Double_t ampl, Double_t position, Double_t length, Char_t *fname=NULL)
{
	if(fname==NULL) fname="f1_";
	cout<<"drawing "<<fname<<"\n";
    TF1 *tf = new TF1(fname,fshape,position,position+length,2);
    tf->SetParameter(0,ampl);
    tf->SetParameter(1,position);
    tf->SetLineStyle(1);
    tf->SetLineWidth(2);
    tf->Draw("same");
}

// ----------------------------------------------------------------------------------------------

Double_t rms(Double_t *xx, Int_t nn=NX)
{
	Double_t sum=0., sum2=0., xmax=0.,fN=(Double_t)nn;
	for(Int_t ii=0;ii<nn;ii++)
	{
		if(xx[ii]>xmax) xmax = xx[ii];
		sum += xx[ii];
		sum2 += xx[ii]*xx[ii];
	}
	sum2 = sqrt(sum2/fN);
	cout<<"srsq="<<sum2*sqrt(fN)<<", rms="<<sum2<<", mean="<<sum/fN<<", max="<<xmax<<"\n";
	return sum2;
}


// ----------------------------------------------------------------------------------------------

Double_t meanw(Double_t *xx, Int_t pos, Int_t nn)
{
	Double_t sumw=0.,sumwin=0.;
	for(Int_t ii=pos-nn+1;ii<=pos;ii++)
	{
		sumwin += xx[ii];
		sumw += xx[ii]*((Double_t)ii);
	}
	return sumw/sumwin;
}

// ----------------------------------------------------------------------------------------------

TRandom grand;
void setSample(Double_t shift, Double_t noise_rms=10.)
{
	Double_t vv;
	Int_t ishift = (Int_t) shift;
	Int_t ii;
        gpos = shift; gnoise = noise_rms;
	for(ii=0;ii<NX;ii++)
	{
		v[ii] = grand.Gaus(0.,noise_rms);
		x[ii] = v[ii];
		s[ii] = 0.;
		if(ii>=shift && ii<shift+N)
		{
			s[ii] = shape((Double_t) (ii-shift))*ALevel/gmax;
			x[ii] += s[ii];
		}
	}
	if(!quiet)
	{
		cout<<"Signal: ";
		vv = rms((s+ishift),N);
		cout<<"Noise:  ";
		vv = rms(v+ishift,N);
		cout<<"S+N:    ";
		vv = rms(x+ishift,N);
	}
}

// ----------------------------------------------------------------------------------------------

//Double_t round(Double_t val, Double_t precision = PRECISION_SAMPLING)
//{
//  return = floor(val/precision)*precision;
//}
Double_t round(Double_t x) {return x;}
void go()
{
	// Calculate filter output and t/T
	Int_t ii,i1,imax;
	Int_t timewin = 10;
	ymax = 0.;
	for (ii=N;ii<NX;ii++) {y[ii] = 0.;yma[ii] = 0.;}//y_s[ii] = 0.;}
	#ifdef NEED_FILTER_NOISE
		for (ii=N;ii<NX;ii++)
	#else
		for (ii=N;ii<=((Int_t)gpos+1+3*N);ii++)
	#endif
	{
		for(i1=0;i1<N;i1++)
		{
			y[ii] += round(x[ii-i1])*g[N-i1];	// calculate filter output
			yma[ii] += x[ii-i1]*gma[N-i1]; // the same for moving average, for comparison 
		}
		if(ymax<y[ii]) {ymax=y[ii]; imax=ii;}
		dy[ii] = round(y[ii]) - round(y[ii-1]);
		dyma[ii] = round(yma[ii]) - round(yma[ii-1]);
	}
	if(dy[imax]-dy[imax+1] <= 0.) {cout<<"Computational error!\n";exit();}
	//tau = imax + dy[imax]/(dy[imax]-dy[imax+1]);
	// parabolic approximation
	tau = round(dy[imax])/(round(dy[imax])-round(dy[imax+1])) - 0.5;
	//cout<<tau<<" - tau\n";
	//correction line for parabolic approximation of amplitude is broken 
	//at point tau=1/2. The corrected max(Y) estimator is more stable and
	//it is more easy implement in hardware. 
	//ymax += (tau/4.)*(dy[imax]+dy[imax+1]);;
	tau += (Double_t)imax - fN;
	#ifdef TRY_WEIGHTED
		tauw = meanw(y,imax+timewin/2.,timewin);
	#endif
	if(!quiet)
	{
		cout<<"ymax["<<tau<<"]= "<<ymax<<"\n";
		#ifdef TRY_WEIGHTED
			cout<<"weigthed mean = "<<tauw<<"\n";
		#endif
	}
}

// ----------------------------------------------------------------------------------------------


// Generate set of 'ntry' sequences with signal positioned at 'pos', 
// superimposed with 'noise'
// If nscan > 1 then collect the correction coeffcients
// If acorr > 0 - do the amplitude correction
// Time correseries(ction is always performed.
// Evidently, if pos is integral number then corrections would be zero 
Double_t series(Double_t pos, Double_t noise, Int_t ntry, Int_t nscans=1, Int_t acorr=1)
{
  cout<<"series("<<pos<<","<<noise<<","<<ntry<<","<<nscans<<","<<acorr<<")"<<endl;
  Int_t ii,iscan,i1;//,previ;
  Double_t sumt,sumt2,ty[4];
  #ifdef TRY_WEIGHTED
    Double_t	sumw=0.,sumw2=0.;
  #endif
  Double_t suma, suma2;
  Double_t sumdevt2,sumdeva2;
  //Double_t meana=ALevel/gmax*(1.+aCorr[(Int_t)(((pos-floor(pos))*NCorr))]);
  Double_t meana=ALevel/gmax;
  Double_t mean,sigma,ns,rc,poss;
  Double_t v1,v2;
  TCanvas *CnvAcc[2];
  quiet = 1;
  //cout<<"Attention. quiet set to 1\n";
  //if(ntry>1000) return;
  for(iscan=0;iscan<nscans;iscan++)
    {
      poss = pos + (Double_t)iscan / (Double_t)nscans;
      suma = 0.; suma2=0.; sumt=0.; sumt2=0.; sumdevt2=0.; sumdeva2=0.;
      for(ii = 0; ii<ntry; ii++)
      {
	// generate signal and noise
	setSample(poss,noise);
	// process the signal
	go();
	// the rest is to get the deviations of the corrected paremeters from the actual ones
	sumt += tau; sumt2 += tau*tau;
	#ifdef TRY_WEIGHTED
	  sumw += tauw; sumw2 += tauw*tauw;
	#endif
	suma += ymax; suma2 += ymax*ymax;
	// first, get the index from the correction table
	i1 = TMath::Nint((tau-floor(tau))*NCorr);
	if(i1>=NCorr) {cout<<"Index error "<<i1<<endl; continue;}
	//cout<<"poss="<<poss<<", tau="<<tau<<", Corr["<<i1<<"]="<<tCorr[i1]<<","<<aCorr[i1]<<"\n";
	v2 = tau - tCorr[i1];
	v1 = v2 - pos;
	sumdevt2 += v1*v1;
	// for amplitude use corrected index
	v1 = ymax/(1.+aCorr[i1]) - meana;
	//v1 = ymax - meana;
	//cout<<ii<<": nc="<<v1<<", corrected="<<ymax*(1.-aCorr[i1]) - meana<<"\n";
	sumdeva2 += v1*v1;
	// periodical print
	if(ii%1000==999) 
	{
	  //cout<<"["<<ii<<"]"<<tau<<", "<<tauw<<"\n";
	  ns=(Double_t)(ii+1);
	  mean = sumt/ns; sigma = sqrt(fabs(sumt2/ns - mean*mean));
	  cout<<"["<<ii<<"] mean="<<mean<<", s="<<sigma;
	  mean = suma/ns; sigma = sqrt(fabs(suma2/ns - mean*mean));
	  cout<<", F="<<mean<<" sF="<<sigma
	  <<", A="<<mean*gmax<<" sA="<<sigma*gmax<<"\n";
	  //cout<<"srt="<<sqrt(sumdevt2/ns)<<","<<1./sqrt(sumdevt2/ns)
	  //	<<". sra="<<sqrt(sumdeva2/ns)<<","<<1./sqrt(sumdeva2/ns)<<"\n";
	}
      }
      ns=(Double_t)ntry;
      //cout<<"lindif: ";
      ty[2] = sumt/ns;
      ty[3] = suma/ns;
      ty[0] = sqrt(fabs(sumt2/ns-ty[2]*ty[2]));
      //ty[1] = gmax*sqrt(fabs(suma2/ns - (ty[3]*ty[3])));
      ty[1] = sqrt(fabs(suma2/ns - (ty[3]*ty[3])));
      if(ty[0]!=0.) ty[0] = 1./ty[0]; else ty[0] = 1.e100;
      if(ty[1]!=0.) ty[1] = 1./ty[1]; else ty[1] = 1.e100;
      ty[1] *= ty[3];
      ty[3] *= gmax;
      if(nscans>1)
      {// collect corrections
	if(iscan==0) v1= 0.; // often having floor problem here
	else v1 = (ty[2] - floor(ty[2]))*(Double_t)NCorr;
	i1 = TMath::Nint(v1); if(i1>=NCorr) i1=NCorr-1; if(i1<0) i1=0;
	//if(i1==previ) cout<<"Replaced element "<<i1<<", try to increase corr. table\n"; previ = i1;
	//i1 = (Int_t)v1;
	//cout<<"p["<<iscan<<"]="<<poss<<" t="<<ty[2]<<","<<v1<<" i="<<i1<<" tc="<<(ty[2] - poss)<<" ac="<<(ty[3]/ALevel - 1.)<<"\n";
	serCorr[0][i1] = ty[2] - poss;
	serCorr[1][i1] = ty[3]/ALevel - 1.;
      }
      //record only the first scan
      if(iscan==0)
      {
	serY[0]=ty[0]; serY[1]=ty[1]; serY[2]=ty[2]; serY[3]=ty[3];
	serY[6] = 1./sqrt(sumdevt2/ns);
	serY[7] = meana/sqrt(sumdeva2/ns);
	// make correction if requested
	if(acorr!=0)
	{
	  // index of correction element calculated from the time/period
	  //i1 = (Int_t)((ty[2]-floor(ty[2]))*NCorr);
	  i1 = TMath::Nint((ty[2]-floor(ty[2]))*NCorr);
	  cout<<"Corr["<<i1<<"]="<<tCorr[i1]<<","<<aCorr[i1]<<"\n";
	  serY[2] -= tCorr[i1];
	  cout<<"time correction["<<i1<<"]="<<tCorr[i1]<<" 1/stdev="<<1./(gpos-serY[2])<<" from "<<ty[2]<<" to "<<serY[2]<<"\n";
	  serY[3] /= (1.+aCorr[i1]);// correct amplitudes
	  cout<<"ampl. correction="<<aCorr[i1]<<" from "<<ty[3]<<" to "<<serY[3]<<"\n";
	} 
      }
      /*
       * cout<<"time="<<ty[2]<<", 1/s="<<ty[0]<<", Acc="<<serCorr[0][iscan]<<"\n";
       * cout<<"ampl="<<ty[3]<<", M/s="<<ty[1]<<", Acc="<<serCorr[1][iscan]<<"\n";
       */
    }
    #ifdef TRY_WEIGHTED
	cout<<"weighted:";
	mean = sumw/ns; sigma = sqrt(fabs(sumw2/ns-mean*mean));
	cout<<"mean="<<mean<<", "<<sigma<<"\n";
    #endif
    quiet = 0;
    return rc;
}

// ----------------------------------------------------------------------------------------------

void nfits(Double_t pos, Double_t noise, Int_t ntry)
{
    Double_t sumt=0.,sumt2=0.,sum=0.,sum2=0.;
    Double_t v,vi,ca=gmax/pars[0];
    Double_t mean,sigma,ns=(Double_t)ntry;
    quiet = 1;
    for(Int_t ii=0;ii<ntry;ii++)
    {
        setSample(pos,noise);
        gf->SetParameter(0,ALevel/gmax);
        gf->SetParameter(1,pos);
        if(gr) delete gr;
        gr = new TGraph(200,ni,x);
        gr->Fit(gf,"Q");
        v = gf->GetParameter(0);
        sum += v; sum2 += v*v;
        v = gf->GetParameter(1);
        sumt += v; sumt2 += v*v;
        if(ii%100==99)
        {
            vi = (Double_t)(ii+1.);
            mean = sumt/vi; sigma = sqrt(fabs(sumt2/vi - mean*mean));
            cout<<"["<<ii<<"] Pos="<<mean<<", sigma="<<sigma
            <<". ParError="<<gf->GetParError(1)<<"\n";
        }
    }
    mean = sumt/ns; sigma = sqrt(fabs(sumt2/ns - mean*mean));
    cout<<"Pos="<<mean<<", sPos="<<sigma<<", ePos="<<gf->GetParError(1);
	mean = sum/ns; sigma = sqrt(fabs(sum2/ns - mean*mean));
	cout<<". A="<<mean*ca<<", sA="<<sigma*ca<<", eA="<<ca*gf->GetParError(0)<<"\n";
    quiet = 0;
}

// ----------------------------------------------------------------------------------------------

void saveCorrections()
{ 
	Double_t v1,v2;
	Int_t ii,kk,i1;
	for(kk=0;kk<NCorr;kk++) 
	{
		//if((serCorr[0][kk])==SMAL_UNDEF)
		if((serCorr[0][kk])==SMAL_UNDEF)
		{// some correction could be undefined, then use average from neighbors
			//linear interpolation till next defined index
			for(ii=1;ii<10;ii++) if((serCorr[0][kk+ii])!=SMAL_UNDEF) break;
			//cout<<"kk="<<kk<<" ii="<<ii<<"\n";
			for(i1=0;i1<ii;i1++)
			{
				tCorr[kk+i1] = serCorr[0][kk-1] + (serCorr[0][kk+ii] - serCorr[0][kk-1])*(Double_t)(i1+1.)/(Double_t)ii;
				aCorr[kk+i1] = serCorr[1][kk-1] + (serCorr[1][kk+ii] - serCorr[1][kk-1])*(Double_t)(i1+1.)/(Double_t)ii;
			}
			kk += ii-1; 
		}
		else
		{
			aCorr[kk] = serCorr[1][kk]; 
			tCorr[kk] = serCorr[0][kk];
		}
	}
	v1 = fabs(TMath::MinElement(NCorr,serCorr[0]));
	v2 = fabs(TMath::MaxElement(NCorr,serCorr[0]));
	serY[4] = 1./TMath::Max(v1,v2);
	v1 = fabs(TMath::MinElement(NCorr,serCorr[1]));
	v2 = fabs(TMath::MaxElement(NCorr,serCorr[1]));
	serY[5] = 1./TMath::Max(v1,v2);
	cout<<"1/syserr for t,A = "<<serY[4]<<", "<<serY[5]<<"\n";
}

// ----------------------------------------------------------------------------------------------

TCanvas *fc[grf_NY];
TGraphErrors	*fg[grf_NY];
void doSeries(Double_t pos=50., Int_t ntry=100)
{
	Int_t ii,kk;

	//for(ii=0;ii<4;ii++) if(fc[ii]) {cout<<"Deleting canvas "<<ii<<"\n";delete fc[ii];}
	for(ii=0;ii<grf_NPTS;ii++)
	{
		grf_ex[ii] = 0.;
		cout<<"Series "<<ii<<" out of "<<grf_NPTS
			<<" for "<<ntry<<" points with S/N="<<grf_x[ii]<<"\n";
		series(pos, ALevel/grf_x[ii],ntry,1,1);
		for(kk=0;kk<grf_NY;kk++)
		{
			grf_y[kk][ii] = serY[kk];
			if(kk==2)		grf_ey[kk][ii] = 1./serY[kk-2];
			else if(kk==3)	grf_ey[kk][ii] = serY[kk]/serY[kk-2];
			else grf_ey[kk][ii] = 0.;
		}
	}
	// draw graphs
	for(kk=0;kk<grf_NY;kk++)
	{
		Int_t lcolor,lwidth,lstyle,lmcolor,lmstyle;
		Double_t lmsize;
		//if(kk==5)continue; // skip "1/(Systematic Error) of Amplitude"
		if(kk<4) 
		{	
			//cout<<"Creating canvas "<<kk<<"\n";
			if(fc[kk]==NULL)	fc[kk] = new TCanvas();
			else fc[kk]->Clear();
			//lstyle = (kk<2) ? kBlack : kBlack;
			lstyle = 1;
			lcolor = (kk<2) ? kBlack : kBlue;
			lmstyle = kStar;
			lwidth=2; lmcolor=lcolor; lmsize=1.;
			fc[kk]->SetLogx(1);
			if(kk<2)
			{
				fc[kk]->SetLogy(1);
			}
		}
		else if(kk<6)
		{//graph 4 and 5	
			// basic graphs
			fc[kk-4]->cd();
			lcolor=kRed; lwidth=4; lstyle=7; lmcolor=2; lmstyle=kDot; lmsize=.75;
		}
		else 
		{//graph 6 and 7
			fc[kk-6]->cd();
			lcolor=kBlue; lwidth=2; lstyle=2; lmcolor=lcolor; lmstyle=kCircle; lmsize=1.;
		}
		fg[kk] = new TGraphErrors(grf_NPTS,grf_x,grf_y[kk],grf_ex,grf_ey[kk]);
		//gr = new TGraph(nn,ni,xx);
		fg[kk]->SetLineColor(lcolor);
		fg[kk]->SetLineWidth(lwidth);
		fg[kk]->SetLineStyle(lstyle);
		fg[kk]->SetMarkerColor(lmcolor);
		fg[kk]->SetMarkerStyle(lmstyle);
		fg[kk]->SetMarkerSize(lmsize);
		//fg[kk]->SetTitle("");
		(fg[kk]->GetXaxis())->SetTitle("A/RMS(noise)");
		//fg[kk]->Draw("ACP");
		if(kk<4)
		{			
			fg[kk]->Draw("APL");
			fg[kk]->SetTitle(grf_name[kk]);
			/**/
			fc[kk]->SetGridx();
			fc[kk]->SetGridy();
			fc[kk]->SetCrosshair();
			fc[kk]->SetTickx();
			fc[kk]->SetTicky();
			/**/
		}
		else	fg[kk]->Draw("PL");
	}
}

// ----------------------------------------------------------------------------------------------

void getCoeffs()
{
	Int_t ii;
	pars[0] = 1.;
	
	// calculate maximum of the shape
	gf = new TF1("gf",fshape,0,N,npars);
	gf->SetParameter(0,pars[0]);
	gmax = gf->GetMaximum();
	gmaxx = gf->GetMaximumX();
	cout<<"Filter max["<<gmaxx<<"]="<<gmax<<"\n";
	// set up the matching filter
	// Use L2 normalization
	norm = 0.; norm_ma = 0.;
	for(ii=0;ii<N;ii++)
	{ 
		g[ii] = shape(ii);
		norm += g[ii]*g[ii];
		gma[ii] = 1.;
		norm_ma += gma[ii]*gma[ii];
	}
	//norm = sqrt(norm/N);
	norm = sqrt(norm);
	//norm_ma = sqrt(norm_ma/N);
	norm_ma = sqrt(norm_ma);
	cout<<"normalization = "<<norm<<", "<<norm_ma<<"\n";
	for(ii=0;ii<N;ii++)
	{
		g[ii] = g[ii]/norm;
		gma[ii] /= norm_ma;
		y[ii]=0.; dy[ii]=0.; dyma[ii]=0.;
	}
	// Lets have the function normalized the same way
	pars[0] = 1./norm;
	gf->SetParameter(0,pars[0]);
	gmax = gf->GetMaximum();
	gmaxx = gf->GetMaximumX();
	cout<<"Filter max["<<gmaxx<<"]="<<gmax<<"\n";
}

// ----------------------------------------------------------------------------------------------

void roundCoeffs(Double_t precision=1./64.)
{
  for(Int_t ii=0;ii<N;ii++)
  {
    g[ii] = floor(g[ii]/precision)*precision;
  }
}

// ----------------------------------------------------------------------------------------------

void showShape()
{
	// show the shape
	gCanv[0]->cd();
	show(g,N,0,0,0,21,2.); gr->GetYaxis()->SetTitle("Filter Coefficients"); 
	gr->Draw(); showsig(pars[0],0.,N,"f1_shape"); 

	// show the input signal superposed with noise
	gCanv[1]->cd();
	Double_t rmsnoise = ALevel/10.;
	series(50.,rmsnoise,100);
	TString txt = "Signal (A="; 
	txt+=ALevel; txt+=" plus noise (RMS="; txt+=rmsnoise; txt+=")";
	show(x); 
	//gr->GetYaxis()->SetTitle("Signal (A=100) plus noise (RMS=10)"); 
	gr->GetYaxis()->SetTitle(txt);
	gr->Draw();
	showsig(pars[0]*ALevel/gmax,gpos,N,"f1_signal");

	gCanv[2]->cd();
	show(y); gr->GetYaxis()->SetTitle("Filter output");
	gr->Draw();
}

// ----------------------------------------------------------------------------------------------

void	clearCorrections()
{
	Int_t ii;
	for(ii=0;ii<NCorr;ii++){ aCorr[ii]=0.; tCorr[ii]=0.;}
	for(ii=0;ii<NCorr;ii++) {serCorr[0][ii] = SMAL_UNDEF; serCorr[1][ii] = SMAL_UNDEF;}
}

// ----------------------------------------------------------------------------------------------

void	getCorrections()
{
	Int_t ii;
	// Calculate the systematic errors and amplitude corrections
	// by running series with zero noise and 100-point scan inside one clock
	// period
	clearCorrections();
	cout<<"Collecting corrections...\n";
	gCanv[3]->cd();
	series(NN,0.,1,NCorr,0); 
	saveCorrections();
	// show corrections
	Double_t t1[NCorr];
	TGraph *corrGraph0,*corrGraph1;
	for(ii=0;ii<NCorr;ii++) t1[ii] = (Double_t)ii/(Double_t)NCorr;
	corrGraph0 = new TGraph(NCorr,t1,tCorr); 
	corrGraph0->SetLineColor(kBlue);corrGraph0->SetLineWidth(2);
	corrGraph0->SetMarkerColor(kBlack);corrGraph0->SetMarkerStyle(kDot);
	corrGraph0->GetXaxis()->SetTitle("t/T");
	corrGraph0->SetTitle("Time Corrections"); 
	gCanv[3]->SetGridx(); gCanv[3]->SetGridy();
	gCanv[3]->SetTickx(); gCanv[3]->SetTicky();
	corrGraph0->Draw("APL");
	gCanv[4]->cd();
	canv = gCanv[4];
	corrGraph1 = new TGraph(NCorr,t1,aCorr); 
	corrGraph1->SetLineColor(kRed);corrGraph1->SetLineWidth(2);
	corrGraph1->SetMarkerColor(kBlack);corrGraph1->SetMarkerStyle(kDot);
	corrGraph1->GetXaxis()->SetTitle("t/T");
	corrGraph1->SetTitle("Amplitude Corrections"); 
	canv->SetGridx(); canv->SetGridy();
	canv->SetTickx(); canv->SetTicky();
	corrGraph1->Draw("APL"); 
	//corrGraph1->Draw("SAME"); 
}

// ----------------------------------------------------------------------------------------------

void	showAutocorrelation()
{
	Int_t ii;
	cout<<"Drawing the autocorrelation function in the interval [0:1]\n";
	gCanv[5]->cd();
	Double_t xacrl[NCorr],yacrl[NCorr],vg;
	TF1 *facrl = new TF1("facrl",fshapeACorr,0.,NCorr,3);
	facrl->SetParameter(1,1.);
	facrl->SetParameter(0,0.);
	vg = facrl->Integral(0.,fN);
	for(ii=0;ii<NCorr;ii++)
	{
		xacrl[ii] = (Double_t)ii/(Double_t)NCorr;
		facrl->SetParameter(0,xacrl[ii]);
		yacrl[ii] = facrl->Integral(0.,fN)/vg;
		//cout<<"aCorr("<<xacrl[ii]<<"="<<yacrl[ii]<<"\n";
	}
	TGraph *gACorr = new TGraph(NCorr,xacrl,yacrl);
	gACorr->SetLineStyle(1);
	gACorr->SetLineWidth(2);
	gACorr->SetTitle("AutoCorrelation");
	gACorr->GetXaxis()->SetTitle("t/T");
	gACorr->Draw("AL");
}

///////////////////////////////////////////////////////////////////////////////
