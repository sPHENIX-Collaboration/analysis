int Fe55_main (  const char * dir, const char * outName = "Fe_50V" );
#ifndef __CINT__

#include "../Bias.cpp"

#include <assert.h>
#include <time.h>
#include <TRandom.h>
#include "TMinuit.h"


class rPad {
private:
	TVirtualPad *fPad;
public:
	rPad(): fPad(gPad){}
	~rPad(){ if (fPad) fPad->cd(); }
};
// CTE parameters *****************************************************
//                    1 3 5 7 
const double CteX = 0.9999999; //0.999997;
const double CteY = 0.9999999;

// other constants ++++++++++++++++++++++++++++++++++++++++++++++++++++

class Co {
public:
// canvas constants
	static const float left_margin;
	static const float right_margin;
	static const float top_margin;
	static const float bot_margin;
    
//what to do flags
    static const int doFit;
    static const int doCTE;

// Fe55 lines and related constants
	static const int Nsrch;
	static const int Nxtail;
	static const int Nytail;
	static const int NXpix;
	static const int NYpix;
	static const int NBtot;
	static const int Nsumh;
	static const double KcutL;
	static const double KcutR;
	static const double Ea;
	static const double Eb;
	static const double w_pair;
	static const double Fano;
	static const double NeKa;
	static const double NeKb;
// geometry & absorbtion
	static const double Depth; //sensor thickness in micron
	static const double Dchan; //N-channel thickness in microns
	static const double DelD;  // thickness ratio
	static const double Tau_a; //absorbtion 
	static const double Tau_b; //absorbtion
//electrical
	static const double Iqmu;  // 1/q*mu in (kOhm*cm)*micron**-3
	static const double Qtke;  // q/(2*k*e_0)in V*micron
	static const double Ro;   // Si resistivity in kOhm*cm
	static const double Na;   // acceptor density in microm^-3
	static const double Nd;   // dopant density in microm^-3
	static const double Cpn;  // pn-junction correction factor
	static const double Vde;  // depletion voltage
	static const double mobil_e; 
	static const double v_sat_e; 

// run constant
	static const double NTlow;
	static const double NTcnt;
    static const double Gain;  // representative gain
    static const double ConvGain[16];

};

const float Co::left_margin =  (float)0.06;
const float Co::right_margin = (float)0.006;
const float Co::top_margin =   (float)0.01;
const float Co::bot_margin =   (float)0.04;

const int Co::doFit = 1;
const int Co::doCTE = 0;

const int Co::Nsrch = 3;
const int Co::Nxtail = 0;
const int Co::Nytail = 0;
const int Co::NXpix = Co::Nsrch + Co::Nxtail;
const int Co::NYpix = Co::Nsrch + Co::Nytail;
const int Co::NBtot = Co::NXpix*Co::NYpix;
const int Co::Nsumh = Co::NBtot + 1;

const double Co::KcutL =  1500.;  // e-
const double Co::KcutR =  1700.;  // e-
const double Co::Ea= 5897.; //in eV
const double Co::Eb = 6490.; //in eV
const double Co::w_pair = 3.68; //in eV/e-
const double Co::Fano = 0.12;
const double Co::NeKa = Co::Ea/Co::w_pair; //in electrons
const double Co::NeKb = Co::Eb/Co::w_pair; //in electrons

const double Co::Depth = 100.; //sensor thickness in microns
const double Co::Dchan = 1.;    //N-channel thickness in microns
const double Co::DelD = Co::Dchan/Co::Depth;  // thickness ratio
const double Co::Tau_a = 28.8/Co::Depth; //absorbtion (in microns)
const double Co::Tau_b = 37.9/Co::Depth; //absorbtion (in microns)

const double Co::Iqmu=4.6296;  // 1/q*mu in (kOhm*cm)*micron**-3
const double Co::Qtke=0.7717e-3;  // q/(2*k*e_0)in V*micron
const double Co::Ro=3.;  // Si resistivity in kOhm*cm
const double Co::Na=Co::Iqmu/Co::Ro; // 1.54 for 3 kOhm*cm, density in microm^-3
//const double Co::Na=1.54;   // 1.54 for 3 kOhm*cm, density in microm^-3
//const double Co::Na=1.155;  // for 4 kOhm*cm, density in microm^-3
//const double Co::Na=0.924;  // for 5 kOhm*cm, density in microm^-3
const double Co::Nd=1.6e+3;  //dopant density in microm^-3
const double Co::Cpn=1.-Co::DelD*Co::DelD-Co::DelD*Co::DelD*Co::Nd/Co::Na;  //junction correction factor
const double Co::Vde=Co::Qtke*Co::Na*Co::Depth*Co::Depth*Co::Cpn;  //depletion voltage

const double Co::mobil_e = 900.;  //in micron**2/(ns*V) 
const double Co::v_sat_e = 118.;  //in micron/ns 

const double Co::NTlow = -3.0;  //1.2;   // Noise Threshold low
const double Co::NTcnt = 5.0;   // Noise Threshold count
const double Co::Gain  = 3.0;   // in e-/adu
const double Co::ConvGain[16]={
    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,    //2.32, 2.93,
    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 };
// 112-09 TS3-SK
//1.9946, 1.9831, 1.9521, 1.9543, 1.9677, 1.9709, 1.9733, 1.9844,
//2.0202, 1.9952, 2.0233, 1.9689, 2.0233, 1.9845, 2.0416, 2.0035  };
// 112-01 TS3-JWY
//3.0159, 3.0040, 3.5802, 3.0541, 2.9406, 3.0012, 3.0181, 2.9704,      // ch2 with bad CTE
//2.9568, 3.0146, 2.9646, 2.9576, 2.9881, 3.0488, 3.0515, 3.1203 };

	//int NKhit=0;
	//double AvHit[Co::NXpix][Co::NYpix]={{0.}};
// Fit Functions ******************************************************
//*********************************************************************
// par[]:
// 0 - total/integral amplitude
// 1 - x-center 
// 2 - y-center
// 3 - sigma 

double  G2DI ( double *v, double *par )  
{
	const double bin_w2 = 0.5;
	double x = v[0];
	double y = v[1];
	double sigma;
	if ( par[3] <  0.000001 ) { sigma = 1.e-6; }  //&& par[3] > -0.000001 
	else { sigma = par[3]; }

	double Xmin = (x - bin_w2 - par[1])/sigma;
	double Xmax = (x + bin_w2 - par[1])/sigma;
	double Ymin = (y - bin_w2 - par[2])/sigma;
	double Ymax = (y + bin_w2 - par[2])/sigma;
	double fitval  =	par[0];
		  fitval *= TMath::Freq(Xmax) - TMath::Freq(Xmin);
		  fitval *= TMath::Freq(Ymax) - TMath::Freq(Ymin);
	return fitval;
}

// cluster finder *******************************************
// sequential clusters **************************************
// individual cluster
class Hits;
class OneHit{
public:
    int Flag;
    
    static int Nrpix1;
    static double Acut1;
    static double AcutL1;
    static int jy_c;         // current row index
    static double * p_crow;  // current row (internal) pointer
    static double * p_arow;  // antecedent row (internal) pointer
    static int * Seed;       // pointer to Hits Seed array
    //    double Amp[Wpad][Wtime];
    
    int    ixfirst,  jyfirst;  //first pixel in the seed
    int    ixmax,  jymax;      //pixel with max amplitude
    int    ixl, ixr;           //left and right index of the zone
    int    jylast;
    int    ixcntr, jycntr;     //zone center
    double xhit, yhit;         //centroid coordinates
    double Amax;
    double SumA, xA, yA;
    int Npix;          //number of pixels abouve threshold
    int Nxpix;         //number of pixels in X-direction
    int Nypix;         //number of pixels in Y-direction
    
    OneHit(){Flag=0;};
    void Init( int ix, int jy );
    int AddPix( int ix, int jy );
    void Done( void ){Flag=1;};
    ~OneHit(){};
};
int OneHit::Nrpix1=0;
double OneHit::Acut1=0.;
double OneHit::AcutL1=0.;
int OneHit::jy_c=0;       // current row index
double * OneHit::p_crow=0;  //for current row array
double * OneHit::p_arow=0;  //for antecedent row array
int * OneHit::Seed=0;     //pointer to Hits Seed array

// cluster collection and management
class Hits{
public:
    static const int Wpad;  //x coord
    static const int Wtime; //y coord
    const int Nrpix;
    const int maxHit;
    const int maxSeed;
    const double Noise;
    const double Threshold;
    const double Acut;
    const double AcutL;

    int NSeeds;
    int NHits;
    int jy_c;       // current row index
    double * prow;  // current row pointer
    double * crow;  // current row array
    double * arow;  // antecedent row array

    OneHit * hit1;  //for hit array[maxHit];
    int * Seed;     //for seed array[maxSeed];
    
void GetRow(double * p_row, int yc);
void ClusterMatch(void);
void ClusterSeed(void);
Hits( int nx, int ny, double ANoise, double thresh);
//~Hits(){};
~Hits(){
    delete [] crow; crow=0;
    delete [] arow; arow=0;
    delete [] hit1; hit1=0;
    delete [] Seed; Seed=0;
}

};
const int Hits::Wpad=3;
const int Hits::Wtime=3;

// OneHit & Hits implementation
void OneHit::Init( int ix, int jy )
{
    double amp;
    ixfirst=ix;
    ixmax=ix;
    printf(" OneHit::Init  %i %i \n", ix, jy);
    if (ix>0) {
        amp=*(p_crow+ix-1);
        if (amp > AcutL1) ixfirst=ix-1;
        printf(" OneHit::Init  %i %f %f \n", ixfirst, AcutL1, amp );
    }
    jyfirst=jy;
    jymax=jy;
    jylast=jyfirst+Hits::Wtime-1;
    jycntr=jyfirst+Hits::Wtime/2;

    Npix=1;
    Nxpix=1;
    Nypix=1;
    Amax=SumA=0.;
    xA=0.;
    int nn=ixfirst+Hits::Wpad;
    nn = nn<Nrpix1 ? nn : Nrpix1;
    for (int i=ixfirst; i<nn; i++){
        amp=*(p_crow+i);
        //if (amp<AcutL1) break;
        if (amp > Amax) {Amax=amp; ixmax=i;}
        Nxpix++;
        SumA+= amp;
        xA+=i*amp;
        *(p_crow+i)=0.;
    }
    yA=jy*SumA;
    // check the antedecent row
    double aSumA=0;
    Nypix++;
    for (int i=ixfirst; i<nn; i++){
        amp=*(p_arow+i);
        //if (amp<AcutL1) break;
        if (amp > Amax) {Amax=amp; ixmax=i;}
        Nxpix++;
        aSumA+= amp;
        xA+=i*amp;
        *(p_arow+i)=0.;
    }
    if (aSumA>SumA) {jymax=jy-1;}
    yA+=(jy-1)*aSumA;
    SumA+=aSumA;
    xhit=xA/SumA;
    yhit=yA/SumA;
    jyfirst=jy-1;
    

    ixl=ixfirst;
    ixr=ixfirst+Hits::Wpad;
    ixcntr=ixl+Hits::Wpad/2;
    if (ixr>Nrpix1) {ixr=Nrpix1;}
    printf(" OneHit::Init  %i %i %f %f %f \n", ixl,ixr, SumA, xhit, yhit );
}

int OneHit::AddPix( int ix, int jy)
{
    if (jy>jylast) {Flag=1; return 1;}
    double amp = *(p_crow+ix);
    SumA += amp;
    if (amp > Amax) {Amax=amp;}
    xA+=ix*amp;
    yA+=jy*amp;
    *(p_crow+ix)=0;
    return 0;
}


Hits::Hits( int nx, int ny, double ANoise, double thresh):
Nrpix(nx),
maxHit((nx/Wpad+1)*(ny/Wtime+1)),
maxSeed(Wtime*nx/Wpad),
Noise(ANoise),
Threshold(thresh),
Acut(Noise*Threshold),
AcutL(Noise*2.0),
NSeeds(0),
NHits(0),
jy_c(0),
prow(0)
{
    crow = new double[Nrpix];
    arow = new double[Nrpix];
    hit1 = new OneHit[maxHit];
    Seed = new int[maxSeed];
    memset( crow, 0, Nrpix);
    memset( arow, 0, Nrpix);
    memset( Seed, 0, maxSeed);
}; // end of Hits constructor

void Hits::GetRow(double * p_row, int yc)
{
    if (!prow) { for (int i=0; i<Nrpix; i++) {arow[i]= crow[i];} }
    prow=p_row;
    jy_c=yc;
    for (int i=0; i<Nrpix; i++) {crow[i]= *(prow+i);}
}

void Hits::ClusterMatch(void)
{
    int full=0;
    if (!NSeeds) return;
//    printf(" Hits::ClusterMatch NSeeds= %i  \n", NSeeds);
    for (int i=0; i<NSeeds; i++){
        int hind=Seed[i];
        int lind=hit1[hind].ixl;
        int rind=hit1[hind].ixr;
        int ixcnt=0;
//        printf(" Hits::ClusterMatch l,r= %i %i %i %i \n", lind, rind, NSeeds, i);
       for (int ii=lind; ii<rind; ii++) {
//           printf(" Hits::ClusterMatch ii,crow= %i %f \n", ii, crow[ii]);
            if (crow[ii]<AcutL) {ixcnt++;}
            full=hit1[hind].AddPix(ii,jy_c);
            if (full) break;
            crow[ii]=0.; //clear the pixel
        }
        if (!ixcnt)  {
            hit1[hind].Done();
            for (int ii=i+1; ii<NSeeds; ii++) {Seed[ii-1]=Seed[ii];}
            NSeeds--;
            i--;
        }
    }
}

void Hits::ClusterSeed(void)
{
    for (int i=0; i<Nrpix; i++) {
        if (crow[i]<Acut) continue;
        if (NSeeds >= maxSeed) break;
        if (NHits >= maxHit) break;
        Seed[NSeeds]=NHits;  //Seed points to a Hit index
        hit1[NHits].Init(i,jy_c);
        NSeeds++;
        NHits++;
 //       printf(" Hits::ClusterSeed i,NSeeds,NHits= %i %i %i \n", i, Nseeds, NHits);
    }
}

// High Amplitude Seed order ******************************************
//*********************************************************************
class Qhit{
public:
	int ixb,   jyb;    
	void Set(int ix, int jy){ixb=ix; jyb=jy;};
	Qhit(){};
	~Qhit(){};
};

class Ohit{
public:
	const int Xmax;
	const int Xmin;
	const int Ymax;
	const int Ymin;
    const double Noise;
    const double lowT;
    const double cntT;
    const double Cntr;           // = Ncore/2.;
	const int nxb,   nyb;
	const long Jmax;
	double * pbuf;
	char * flagbuf;
	int Flag;
    int isoFlag; //isolated pixel flag

	enum { Ncore   = Co::Nsrch };
	enum { NXsrch  = Co::NXpix };
	enum { NYsrch  = Co::NYpix };
	double Amp[NXsrch][NYsrch];
    int    ixb,  jyb;    //center of the search area in the buffer
	double xhit, yhit;   //centroid local coordinates, relative zone corner
    double xhitG, yhitG;   //centroid global/image coordinates
    double xfitG, yfitG;   //centroid global fitted coordinates
	double Amax;
	double Aseed;
	double Sum;
	double SumCTE;
    double SumOne; //sum of isolated pixels
	double rms;
	double MRatio;
	double ARatio;
	int Npix;          //number of pixels abouve threshold
	int NpixH;         //number of pixels abouve High threshold
	int Nxpix;         //number of pixels in X-direction
	int Nypix;         //number of pixels in Y-direction 

	enum { NZ   = 4 };
	int Zind;
	int NKhit[NZ];
	double AvHit[NZ][NXsrch][NYsrch];
	double A2Hit[NZ][NXsrch][NYsrch];

	TCanvas *cF;
	TH2F * f2D;
	TF2 * fit;
	enum { Npar2DI = 4};
	double FitPar[Npar2DI];
	double FitErr[Npar2DI];
	int fitflag;
	const int Nfree;
	double chi2;
	double chiR;
	double Sumf;
	double xfit;
	double yfit;
	double sfit;
	double efit;

	void FitX( void );  // 2D cluster fit TCanvas *cF 
	void Cluster( int ix, int jy, const int pass=0  );
	void Clear( void );
	Ohit(int max_X, int min_X, int max_Y, int min_Y, double ANoise,
		    int nx, int ny, double * buf, 	
			TCanvas * pcF, TH2F * pf2D, TF2 * pfit );	
	//Ohit(int max_X, int min_X, int max_Y, int min_Y, double ANoise,
	//	    int nx, int ny, double * buf);
	~Ohit(){delete [] flagbuf; flagbuf = 0;};
};

Ohit::Ohit( int max_X, int min_X, int max_Y, int min_Y, double ANoise,
		    int nx, int ny, double * buf, 	
			TCanvas * pcF, TH2F * pf2D, TF2 * pfit ):
	Xmax(max_X),
	Xmin(min_X),
	Ymax(max_Y),
	Ymin(min_Y),
	Noise(ANoise),
    lowT(Co::NTlow*Noise),
    cntT(Co::NTcnt*Noise),
    Cntr(Ncore/2.),
	nxb(nx),
	nyb(ny),
    Jmax(nxb*nyb),
	pbuf(buf),
	Flag(-100),
    isoFlag(-100),
	cF(pcF),
	f2D(pf2D),
	fit(pfit),
	fitflag(-1),
	Nfree(NXsrch*NYsrch-Npar2DI),
	chi2(-1.0),
	chiR(-1.0),
	Sumf(-1.0),
	xfit(-1.0),
	yfit(-1.0),
	sfit(-1.0),
	efit(-1.0)
{
	if ( pcF==0)  cF = new TCanvas("Fit2D", "Fit2D", 300,10,700,650);
	if ( f2D==0) f2D = new TH2F("f2D","fit2", NXsrch, 0, NXsrch, NYsrch, 0, NYsrch);
	if ( fit==0) fit = new TF2("fitG2DI", G2DI, 0., NXsrch, 0., NYsrch, Npar2DI);
	for (int i=0;i<NZ;i++){
		NKhit[i]=0;
		for (int y=0; y<NYsrch; y++){
			for (int x=0; x<NXsrch; x++){
					AvHit[i][x][y] =0.;
					A2Hit[i][x][y] =0.;
			}
		}
	}
	flagbuf =new char[Jmax];
	memset( flagbuf, 0, Jmax);

} // end of Ohit constructor

void Ohit::Cluster(int ix, int jy, const int pass )
{
	ixb=ix;
	jyb=jy;
	Flag = -1000;
    isoFlag = -1000;
	xhit = 0.;
	yhit = 0.;
    xhitG = 0.;
    yhitG = 0.;
    xfitG = 0.;
    yfitG = 0.;
	Amax = 0.;
	Sum  = 0.;
	SumCTE = 0.;
    SumOne = 0.;
	rms = 0.;
	ARatio = 0.;
	Npix = 0;
	NpixH= 0;
	fitflag=-1;
	chi2=-1.0;
	chiR=-1.0;
	Sumf=-1.0;
	xfit=-1.0;
	yfit=-1.0;
	sfit=-1.0;
	efit=-1.0;

	int js = jyb*nxb + ixb;
	if (js < 0 || js >= Jmax) return; 
	Aseed = pbuf[js];
	if ( Aseed < 3.*Noise ) return;

    Flag = 0;

    char pileup=0;
    const int center = Ncore/2;
	for (int y=0; y<NYsrch; y++){
        int ybuf = jyb + y - center;
		if ( ybuf < Ymin || ybuf >= Ymax) {Flag = -1; continue;}
		for (int x=0; x<NXsrch; x++){
            int xbuf = ixb + x - center;
			if ( xbuf < Xmin || xbuf >= Xmax) {Flag = -1; continue;}
			int jb = ybuf*nxb + xbuf;
			double amp = 0.;
			if (Flag == 0 ) { amp = pbuf[jb]; }  
			Sum += amp;  
            if (pass) { flagbuf[jb]++;}
            if ( flagbuf[jb] >1 ) {pileup = 1;}
			if (  amp > cntT ) { NpixH++; }
			if ( (amp > lowT) || pass ) {
				Npix++;
				//Sum += amp;
				xhit += x*amp;
				yhit += y*amp;
				double ampC = amp;
				ampC /=(TMath::Power(CteX, xbuf)*TMath::Power(CteY, ybuf));
				SumCTE += ampC; 
			}
			if ( amp > Amax ) { Amax = amp;	}
			Amp[x][y] = amp;
		}
	}

	if (pass) { return; }
	if ( pileup ) {Flag = -200;  return;}  //printf("Pileup detected \n");
	if ( Sum < 100.) {Flag = -100;  return;}  //printf("Sum<100 \n");
	MRatio = Amax/Sum;
	ARatio = 0.125*(Sum/Amax - 1.);
	xhit /=Sum; 
	yhit /=Sum;
    xhitG= ixb + xhit - Cntr;
    yhitG= jyb + yhit - Cntr;

	double rmsx2 = 0.;
	double rmsy2 = 0.;
	for (int y=0; y<NYsrch; y++){
		for (int x=0; x<NXsrch; x++){
			double amp=Amp[x][y];
			if (amp <0.) continue;
			rmsx2 += (x - xhit)*(x-xhit)*amp/Sum;
			rmsy2 += (y - xhit)*(y-xhit)*amp/Sum;
		}
	}
	rms = sqrt(rmsx2+rmsy2);

// isolated pixel row flag
    isoFlag=0;
    SumOne = 0.;
    
    for(int x = 0; x < NXsrch; x++){
        for(int y = 0; y < NYsrch; y++) {
            if (y == center ) continue;
            if (Amp[x][y] > cntT) {isoFlag++;}
        }
    }
    
    int preC = center-1;
    if (preC<0) preC=0;
    
    for(int x = 0; x < preC; x++) {
        if (Amp[x][center] > cntT) {isoFlag++;}
    }
    
    if (!isoFlag) {
        for (int x = preC; x < NXsrch; x++) {
            SumOne += Amp[x][center];
        }
    }
    
	//if ( Sum < Co::KcutL && Sum > Co::KcutR ) { return; }
	//Zind=0;
	//if (ixb > nxb/2) Zind=1; 
	//NKhit[Zind]++;
	//for (int y=0; y<NYsrch; y++){
	//	for (int x=0; x<NXsrch; x++){
	//		double amp=Amp[x][y];  ///Sum
	//		AvHit[Zind][x][y] += amp;
	//		A2Hit[Zind][x][y] += amp*amp;
	//	}
	//}
	//Zind=2;
	//if (jyb > nyb/2) Zind=3; 
	//NKhit[Zind]++;
	//for (int y=0; y<NYsrch; y++){
	//	for (int x=0; x<NXsrch; x++){
	//		double amp=Amp[x][y];  ///Sum
	//		AvHit[Zind][x][y] += amp;
	//		A2Hit[Zind][x][y] += amp*amp;
	//	}
	//}
	//

	return;
}

void Ohit::FitX( void )
{
// 2D fit 

	for (int y=1; y<=NYsrch; y++){
			//cout << " y= " << y ;
		for (int x=1; x<=NXsrch; x++){
			double amp = Amp[x-1][y-1];
			if (amp < 0.) amp=0.;
			f2D->SetBinContent(x, y, amp);
			double StErr  = Noise;
			double StErr2 = Noise*Noise; 
			if ( amp > 3.*Noise ){
                StErr2 += amp * Co::Gain * Co::Fano;
				StErr = sqrt(StErr2);
			}
			f2D->SetBinError  (x, y, StErr);
			//cout << "  "   << amp;
		}
			//cout << endl;
	}
	//cF->cd(1);
	//f2D->Draw("lego");

	fit->SetParameter(0, Sum);
	//fit->SetParLimits(0,0.,2000.);
	fit->SetParameter(1, xhit);  
	fit->SetParameter(2, yhit); //fit->SetParLimits(2,0.,1000.);
	fit->SetParameter(3, rms);  
	//fit->SetParLimits(3,0.,8.);
	//cout<<" "<<endl;
	//cout << " Initial parameters: " << endl; 
	//cout << " N="<<Ncore<<" Npix="<<Npix<<" Aseed="<<Aseed<<endl;
	//for (int jp = 0; jp < Npar2DI; jp++){
	//	cout <<" par "<<jp<<" = "  << fit->GetParameter(jp)<< endl;
	//}

	fitflag = f2D->Fit("fitG2DI","Q"); //LNQ
	chi2 = fit->GetChisquare();
	chiR = chi2/(double)Nfree;
    Sumf = fit->GetParameter(0);
    xfit = fit->GetParameter(1);
    yfit = fit->GetParameter(2);
    sfit = fit->GetParameter(3);
    if (sfit<0.) sfit = -sfit;
    efit = fit->GetParError(3);
    if (efit<0.) efit = -efit;
    if ( fitflag == 0 ) {
        xhitG= ixb + xhit - Cntr;
        yhitG= jyb + yhit - Cntr;
        xfitG= ixb + xfit - Cntr;
        yfitG= jyb + yfit - Cntr;
    }

	//cout << " fit status = " << fitflag << endl;
	//cout << " Chi**2 = " << chi2  << endl;
	//cout << " Npar/Npix = "   << Npar2DI << "/" << Npix << endl;
	//for (int jp = 0; jp < Npar2DI; jp++){
	//	FitPar[jp]=fit->GetParameter(jp);
	//	FitErr[jp]=fit->GetParError(jp);
	//	cout <<" par "<<jp<<" = "<<FitPar[jp]<<" +- "<<FitErr[jp]<< endl;
	//	if ( FitPar[jp] < 0.) cout << " WARNING! " << FitPar[jp] << endl;
	//}
	//if ( fitflag != 0 ) { 
	//	fit->SetParameter(0, Sum);	
	//	//fit->SetParLimits(0,0.,2000.);
	//	fit->SetParameter(1, xhit);  
	//	fit->SetParameter(2, yhit); 
	//	fit->SetParameter(3, rms);  
	//	fit->SetParLimits(3,0.,8.);
	//	cout<<" *** Debug failed fit ***"<<endl;
	//	cout << " Initial parameters: " << endl; 
	//	cout << " N="<<Ncore<<" Npix="<<Npix<<" Aseed="<<Aseed<<endl;
	//	for (int jp = 0; jp < Npar2DI; jp++){
	//		cout <<" par "<<jp<<" = "  << fit->GetParameter(jp)<< endl;
	//	}
	//	fitflag = f2D->Fit("fitG2DI","LNV"); //Q
	//	TH2F * f2Df= new TH2F("f2Df","fit2f", NXsrch, 0, NXsrch, NYsrch, 0, NYsrch);
	//	double q[2];
	//	for (int y=1; y<=NYsrch; y++){
	//			cout << " y= " << y ;
	//			q[1] = (double)y-0.5; // bin center
	//		for (int x=1; x<=NXsrch; x++){
	//			q[0] = (double)x-0.5; // bin center
	//			double AmD = Amp[x-1][y-1];
	//			AmD -= G2DI(q, FitPar);  
	//			f2Df->SetBinContent(x, y, AmD);
	//			cout <<"  "<<Amp[x-1][y-1]<<" / "<<AmD;
	//		}
	//			cout << endl;
	//	}
	//	cF->cd(2);
	//	f2Df->Draw("lego");

	//	cF->Update();
	//	while(!gSystem->ProcessEvents()){gSystem->Sleep(400);};
	//	delete f2Df;
	//} //  end fit flag IF 

	//cF->Update();
	//while(!gSystem->ProcessEvents()){gSystem->Sleep(400);};

	return;
}  // end Ohit::FitX		 
		 
void Ohit::Clear()
{
	if (Flag < -500 ) return;
	int Nc = Ncore/2;
	for (int i=0; i<NYsrch; i++){
			int xbuf = ixb + i - Nc;
		for (int j=0; j<NXsrch; j++){
			int ybuf = jyb + j - Nc;
			int jb = ybuf*nxb + xbuf;
			if ( (xbuf >= Xmin && xbuf < Xmax) &&
			     (ybuf >= Ymin && ybuf < Ymax)) pbuf[jb]=0.;
		}
	}
}

//*********************************************************************
int PeakRange(TH1D * hist, double * maxpos, double * Rmin, double * Rmax) 
{
	const double Fract = 0.04;
	int maxbin =    hist->GetMaximumBin();
	*maxpos =	    hist->GetBinCenter(maxbin);
	double maxval = hist->GetBinContent(maxbin);	

	cout << " maxbin=" << maxbin << " maxval=" <<maxval <<endl;
	cout << " max/maxpos=" << *maxpos << endl;

	int bin = maxbin;
	while (bin>0 && hist->GetBinContent(bin) > Fract*maxval) bin--;
	*Rmin = hist->GetBinCenter(bin);
	cout << " Rmin=" << *Rmin << " A(Rmin)=" << hist->GetBinContent(bin) << endl;

	int xlast   = hist->GetXaxis()->GetLast();
	bin = maxbin;
	while (bin<xlast && hist->GetBinContent(bin) > Fract*maxval) bin++;
	bin++;
	*Rmax = hist->GetBinCenter(bin);
	cout << " Rmax=" << *Rmax << " A(Rmax)=" << hist->GetBinContent(bin) << endl;

	return 0;
}
//---------------------------------------------------------------------
// SimX class ---------------------------------------------------------
class SimX: public FileF {
public:
// canvas constants
	static const float left_margin;
	static const float right_margin;
	static const float top_margin;
	static const float bot_margin;

// constants for simulation
	static const int bend;
	const int Nmatr;
	const int Msz;
	const int Centr;
	double * matrix;

	static const int Nray;     // number of X-rays
	static const double SiDi;      // in pixels
	static const double Sigma0;      // in pixels
	 double SiPzS2;    // SiDi*sqrt(2.)
	static const double Vop;      // in V
	static const double Vde;      // in V
	//static const double Na;      // acceptor micron^-3
	//static const double Nd;      // donor micron^-3
	//static const double Ra;      // ratio del/d
	static const double Cpn;      // in V
	static const double Vplus;      // in V
	static const double Vmins;      // in V
	static const double Tmax; 
	static const double t_sat;

	const int tbl_sz;

// some constants (initiated from Dev)
	int XminSearch;
	int XmaxSearch;
	int YminSearch;
	int YmaxSearch;
	double PixSz;

	int Flag;

	static SimX * ptrSimA;  //needed for FCN 

    string dname;
	string filename;
	string OutFN;

	unsigned short * bufsi;

	TH1D * hsiAmp;
	TH1D * hsiAmx;
    TH1D * hsiAle;
	TH1D * hsiXl;
	TH1D * hsiYl;
	TH1D * hsiZl;
	TH1D * hsiTl;

public:
	SimX (	string dir_name );
	void Simulator( void );
	void PlotSim( void );
	//int create_Table();
	int OutSimF( const char * fOutName  );
	~SimX(){ delete []bufsi; bufsi = 0; };
};

// initialization of static members
const float SimX::left_margin =  (float)0.06;
const float SimX::right_margin = (float)0.006;
const float SimX::top_margin =   (float)0.01;
const float SimX::bot_margin =   (float)0.04;

const int	 SimX::bend = 1;
const int    SimX::Nray = 8000; //number of X-rays
const double SimX::SiDi = 0.355;   //diffusion sigma in pixels
const double SimX::Sigma0 = 0.07;  //initial size in pixels
//const double SimX::SiPzS2 = SimX::SiDi*sqrt(2.);    // SiDi*sqrt(2.)
const double SimX::Vop=81.;      // in V
const double SimX::Vde=Co::Vde;      // 10.648 in V
//const double SimX::Na=1.54;      // acceptor concentration
//const double SimX::Nd=1.6e+3;     // donor concentration
//const double SimX::Ra=1.e-4;      // (del/d)**2
const double SimX::Cpn=Co::Cpn;      
const double SimX::Vplus=SimX::Vop +SimX::Vde*(2.*Co::Cpn -1.); // in V
const double SimX::Vmins=SimX::Vop -SimX::Vde;      // in V
const double SimX::Tmax=log(SimX::Vplus/SimX::Vmins);      
const double SimX::t_sat=2.*Co::mobil_e*SimX::Vde/(Co::v_sat_e*Co::Depth);      

//pixels:   1.0  0.593 0.555 0.519 0.481 0.444 0.407 0.370 0.333 0.296 0.259 0.222
//microns: 13.5  8.0   7.5   7.0   6.5   6.0   5.5   5.0   4.5   4.0   3.5   3.0


	SimX * SimX::ptrSimA = 0; //needed for FCN 

SimX::SimX(	string dir_name ):
	Nmatr(2* bend +1),        
	Msz(Nmatr*Nmatr),
	Centr(Msz/2),
	tbl_sz(Nmatr+4),
	dname(dir_name),
	bufsi(0)
{
	ptrSimA = this;

	cout<<" Sim Param Sigma Diffusion: "<<SiDi<<'\n';
	cout<<" Sim Param Gain:            "<<Co::Gain<<'\n';
	cout<<" Sim Param Ne Ka:           "<<Co::NeKa<<'\n';
	cout<<" Sim Param Ne Kb:           "<<Co::NeKb<<'\n';
	cout<<" Sim Cluster matrix bend:   "<<bend <<'\n';
	cout<<" Sim Cluster matrix Nmatr:  "<<Nmatr <<'\n';
	cout<<" Sim Vop:                   "<<Vop <<'\n';     
	cout<<" Sim Vde:                   "<<Vde <<'\n';     
	//cout<<" Sim Na:                    "<<Co::Na <<'\n';
	//cout<<" Sim Nd:                    "<<Co::Nd <<'\n';
	cout<<" Sim DelD:                  "<<Co::DelD <<'\n';
	cout<<" Sim Cpn:                   "<<Co::Cpn<<'\n';      
	cout<<" Sim Vplus:                 "<<Vplus  <<'\n';
	cout<<" Sim Vmins:                 "<<Vmins  <<'\n';
	cout<<" Sim Tmax:                  "<<Tmax   <<'\n';     
	cout<<" Sim t_sat:                 "<<t_sat  <<'\n';      
//
	hsiAmp = new TH1D( "siAmp", "siAmp", 1000, 0., 1000.);
	hsiAmp->SetStats(1);
	hsiAmx = new TH1D( "siAmx", "siAmx", 1000, 0., 1000.);
	hsiAmx->SetStats(1);
    hsiAle = new TH1D( "siAle", "siAle", 1000, 0., 1.);
    hsiAle->SetStats(1);
    hsiXl = new TH1D( "siXl", "siXl", 200, -0.5, 1.5);
	hsiXl->SetStats(1);
	hsiYl = new TH1D( "siYl", "siYl", 200, -0.5, 1.5);
	hsiYl->SetStats(1);
	hsiZl = new TH1D( "siZl", "siZl", 120, 0., 1.20);
	hsiZl->SetStats(1);
	hsiTl = new TH1D( "siTl", "siTl", 120, 0., 1.20);
	hsiTl->SetStats(1);

//
		matrix = new double[Msz];

// file list:  ********************************************************
	FileList FL(dname);
	if (FL.Nfile <= 0) {Flag= -10; return;}
	

// bias file loop: ****************************************************
	FL.FName_Iter=FL.FName.begin();
	cout << "Sim starts with bias file:" << *FL.FName_Iter << endl;
	for (FL.FName_Iter=FL.FName.begin(); FL.FName_Iter != FL.FName.end(); FL.FName_Iter++ ) 
	{
		filename = *FL.FName_Iter;  // dname + "/" +
		cout << "SimX File: " << *FL.FName_Iter << endl;
		if ( Open(filename.c_str()) ) continue;

		while( !iEOF ){              
			if ( hdu > MaxP )   break;
			if ( Read() )    continue;
			if ( naxis != 2) continue; 

// get device description ************************

			DataStr * Dev = DataStr::Instance(nx,ny);
			XminSearch = Dev->minX() + 8;
			XmaxSearch = Dev->maxX() - 8;
			YminSearch = Dev->minY() + 8;
			YmaxSearch = Dev->maxY() - 8;
			PixSz = Dev->PixSz();
			cout<<" Sim Param Pixel Size: "<<PixSz<<'\n';
			cout<<" Sim Param SigmDiff:   "<<SiDi*PixSz<<'\n';

			if (bufsi == 0) bufsi = new unsigned short[npixels];
			for (unsigned long i=0; i<npixels; i++){
							//bufsi[i] = buffer[i];
							bufsi[i] = 5700. + gRandom->Gaus(0.,5.137);
			}

			Simulator();

			//output fits file 
			//char tmpstr[200];
			//sprintf( tmpstr, "SimGV_%i_", (int)(SiDi*PixSz*10));
			//OutFN =  tmpstr + *FL.FName_Iter;
			//printf(" Output file: %s \n", OutFN.c_str());


			//OutSimF( OutFN.c_str());

		} //end while

	} //end file loop

}; //end SimX constuctor 

//*********************************************************************
void SimX::PlotSim( void )
{
// X-ray simulation plots ******************************************
	 TCanvas	*	SimP = new TCanvas( "SimP", "SimP", 55, 15, 900, 800);
	 SimP->SetBorderMode  (0);      //to remove border
	 SimP->SetLeftMargin  (left_margin);   //to move plot to the right 0.14
	 SimP->SetRightMargin (right_margin);   //to move plot to the right 0.05
	 SimP->SetTopMargin   (top_margin);   //to use more space 0.07
	 SimP->SetBottomMargin(bot_margin);   //default
	 SimP->SetFrameFillColor(0);
	 SimP->Divide(2,4);
	 SimP->Update();

	 SimP->SetFillStyle(4100);
	 SimP->SetFillColor(0);

	int Entry;

	Entry = (int)hsiAmp->GetEntries();
	if ( Entry > 1 ) {
		SimP->cd(1);
		hsiAmp->Draw();
	}

	Entry = (int)hsiAmx->GetEntries();
	if ( Entry > 1 ) {
		SimP->cd(2);
		hsiAmx->Draw();
	}

	Entry = (int)hsiXl->GetEntries();
	if ( Entry > 1 ) {
		SimP->cd(3);
		hsiXl->Draw();
	}

	Entry = (int)hsiYl->GetEntries();
	if ( Entry > 1 ) {
		SimP->cd(4);
		hsiYl->Draw();
	}

	Entry = (int)hsiZl->GetEntries();
	if ( Entry > 1 ) {
		SimP->cd(5);
		hsiZl->Draw();
	}

	Entry = (int)hsiTl->GetEntries();
	if ( Entry > 1 ) {
		SimP->cd(6);
		hsiTl->Draw();
	}

    Entry = (int)hsiAle->GetEntries();
    if ( Entry > 1 ) {
        SimP->cd(7);
        hsiAle->Draw();
    }

	 SimP->Update();
	 SimP->Print("SimP.pdf");

	return;
} //end PlotSim function
	

//int SimX::create_Table()
//{
//   
//   const char *ttype1[tbl_sz] = {"id", "Amp", "Xd", "Yd","Matrix"};
//   const char *tform1[tbl_sz] = {"1I", "1E", "1E", "1E", "NmatrE"};  //NmatrE
//   const char *tunit1[tbl_sz] = {"", "", "","",""};
// 
//     // int fits_create_tbl(fitsfile *fptr, int tbltype, long nrows, int tfields,
//    //char *ttype[],char *tform[], char *tunit[], char *extname, int *status)
//
//
//   /* Add the first binary table extension. */
//   fits_create_tbl(fptr, BINARY_TBL, 1L, tbl_sz, (char **)ttype1, (char **)tform1,
//       (char **)tunit1, NULL, &status);
//
//	fits_write_key (fptr, TSTRING, "TDISP1", "I8", 
//                "recommended display format", &status);
//	fits_write_key (fptr, TSTRING, "TDISP2", "F8.3", 
//                "recommended display format", &status);
//	fits_write_key (fptr, TSTRING, "TDISP3", "F8.3", 
//                "recommended display format", &status);
//	fits_write_key (fptr, TSTRING, "TDISP4", "F8.3", 
//                "recommended display format", &status);
//	fits_write_key (fptr, TSTRING, "TDISP5", "F8.3", 
//                "recommended display format", &status);
//
///* stuff data one row at a time into a binary table in the FITS file */
///* Count3 and Count4 ignored */
//// int fits_write_col / ffpcl
////      (fitsfile *fptr, int datatype, int colnum, long firstrow,
////       long firstelem, long nelements, DTYPE *array, > int *status)
//   long nx =1;
//   for (int row = 1; row <= Nmatr; row++)
//   {
//	fits_write_col (fptr, TINT, 1, row, 1, 1, row, &status);
//	fits_write_col (fptr, TFLOAT, 2, row, 1, 1, Amp, &status);
//	fits_write_col (fptr, TFLOAT, 3, row, 1, 1, Xd, &status);
//	fits_write_col (fptr, TFLOAT, 4, row, 1, 1, Yd, &status);
//	fits_write_col (fptr, TUSHORT, 5, row, nx, Nmatr,matrix, &status);
//	nx += Nmatr;
//   }
//
//}
//

void SimX::Simulator( void )
{
const double Xrange = XmaxSearch-XminSearch;
const double Yrange = YmaxSearch-YminSearch;

for (int ray=0; ray<Nray; ray++){

// X-ray coordinates (in pixels):
	double xs = (double)XminSearch + Xrange*gRandom->Rndm();
	double ys = (double)YminSearch + Yrange*gRandom->Rndm();
	int ixs=(int)xs;
	int jys=(int)ys;
	//int Jind = jys *nx + ixs;
	double lxs = xs - ixs; //local x
	double lys = ys - jys; //local y
	hsiXl->Fill(lxs);
	hsiYl->Fill(lys);
		
// cluster amplitude in e-
	double tau;
	double Amp;
	double ab = gRandom->Rndm();
	if (ab < 0.12) {Amp = Co::NeKb; tau=Co::Tau_b;}
	else           {Amp = Co::NeKa; tau=Co::Tau_a;}
    Amp = Co::NeKa; tau=Co::Tau_a; //only Ka

// z/d - coordinate (depth of conversion), z=0 window
	double lz = 2.; 
	while (lz > 1.){
		lz = gRandom->Exp(tau);
	}
	hsiZl->Fill(lz);

	double drti = log( Vplus/(Vmins+2.*Vde*lz*Cpn) ) + t_sat*(1.-lz);
	drti /= (Tmax+t_sat);
	if (drti < 0.) {drti = 0.;}
	else           {drti = sqrt (drti);}
	hsiTl->Fill(drti);
	double sigma_z = SiDi*drti;
	sigma_z = sqrt (sigma_z*sigma_z + Sigma0*Sigma0);
	SiPzS2 = sigma_z*sqrt(2.);    

	
	// Diffuse Cluster

	for (int i=0; i<Msz; i++) {matrix[i]=0.;}  // Cluster Matrix Init

	double Asim=0.;
    double dyl;
    double dyh;
    double sigY;
    double dxl;
    double dxh;
    double sigX;
	for(int i = 0; i < Nmatr ; i++){
		int ly = i - bend;
            dyl = (double)ly - lys;
            dyh = dyl + 1.;
            dyl /= SiPzS2;
            dyh /= SiPzS2;
            sigY = TMath::Erf(dyh)-TMath::Erf(dyl);
            sigY /= 2.;
		//printf("  iy:%i, %i ", i,ly);
		//printf("  iy:%i, %i %f %f %f \n", i,ly, dyh, dyl, sigY);
		for(int k = 0; k < Nmatr ; k++){
			int lx = k - bend;
                dxl = (double)lx - lxs;
                dxh = dxl + 1.;
                dxl /= SiPzS2;
                dxh /= SiPzS2;
                sigX = TMath::Erf(dxh)-TMath::Erf(dxl);
                sigX /= 2.;
			double signal = Amp*sigX*sigY;
			double sim_sig = signal;
			if (signal > 5.) {  //signal Poisson variation 
				double ran_sig = signal * Co::Fano;
				sim_sig = gRandom->PoissonD(ran_sig);
				sim_sig += signal*(1.-Co::Fano);
			} //signal Poisson variation 
			if ( sim_sig < 0.0) sim_sig=0.;
			sim_sig /= Co::Gain;
			Asim += sim_sig;
			int gx = lx + ixs;
			int gy = ly + jys;
			int jj = gy *nx + gx;
			int idx = i * Nmatr + k;
			matrix[idx]=sim_sig;
			bufsi[jj] +=sim_sig; 
			//printf(" ix=%i %f %f %f %f %f \n ", lx, dxl, dxh, matrix[idx], signal, sigX);
			//printf("  %f %d %d ", matrix[idx], buffer[jj], bufsi[jj]);
		}
			//printf(" \n ");
	}
	//printf("  Asim =%fe- Xamp=%f \n", Asim, matrix[Msz/2]);
	hsiAmp->Fill(Asim);
	hsiAmx->Fill(matrix[Msz/2]);
    
    dyl = (-bend - lys)/SiPzS2;
    dyh = (Nmatr - bend -lys)/SiPzS2;
    sigY = TMath::Erf(dyh)-TMath::Erf(dyl);
    sigY /= 2.;
    dxl = (-bend - lxs)/SiPzS2;
    dxh = (Nmatr - bend -lys)/SiPzS2;
    sigX = TMath::Erf(dyh)-TMath::Erf(dyl);
    sigX /= 2.;
    double sigLF = (1.-sigX*sigY)*100.; // leaked fraction in %
    hsiAle->Fill(sigLF);

} //end of ray loop

	return;
}  //end SimX::Simulator
//-----------------------------------------------------------------------------

int SimX::OutSimF( const char * fOutName  )
{

	fitsfile *fSimOut;		/* pointer to the output FITS file */ 
	//int status = 0;
	status = 0;

	char foutname[200]; // = "Output.fits";      
	//strcpy (foutname,  dname.c_str());  
	//strcat (foutname, "\\");
	strcpy (foutname, fOutName);
	//strcat (foutname, ".fits");

    fits_create_file(&fSimOut, foutname, &status); /* create new FITS file */
    if ( status ) {printf(" create_file status error: %i \n", status); return -6;}
     
	//  Copy keywords from existing HDU into empty HDU of fOutName file
	fits_copy_header( fptr, fSimOut, &status);
	if ( status ) {printf(" copy header error: %i \n", status); return -6;}
	// don't copy, just bare header
	//const int naxis =2;
	//long naxes[naxis];
	//naxes[0]=nx;
	//naxes[1]=ny;
	//npixels = nx*ny;
	//fits_create_img( fSimOut, USHORT_IMG, naxis, naxes, &status);
	//if ( status ) {printf(" write_img status error: %i \n", status); return -6;}

	/* write the array of unsigned short to the FITS file */
	fits_write_img( fSimOut, TUSHORT, fpixel, npixels, bufsi, &status);
	if ( status ) {printf(" write_img status error: %i \n", status); return -6;}
     
    fits_close_file(fSimOut, &status);  
	if ( status ) {printf(" close_file status error: %i \n", status); return -6;}


	return 0;
} //end SimX::OutSimF
//*********************************************************************

//---------------------------------------------------------------------
// Fe55 class ---------------------------------------------------------
class Fe55: public FileF {
public:
// canvas constants
	static const float left_margin;
	static const float right_margin;
	static const float top_margin;
	static const float bot_margin;

// Fe55 lines and related constants
	static const int Npar;  //fit parameters

// some "constants" (initiated from Dev)
	int XminSearch;
	int XmaxSearch;
	int YminSearch;
	int YmaxSearch;
	double PixSz;
	double AminSearch[MaxP];
	double ANoise[MaxP];  
	const double HighCut;

	int Nchan;
	int ch_idx;

	int Flag;

    string dname;
	string filename;
	const char * OutExt;
    FILE * pFlist;
    FILE * pFreg;
    FILE * pFcat;


	const vector<double> * avbuf;
	//vector <double> bufzs;
	double * bufzs;
	double * bz_save;

// FFT 
	const int doFFT;
	FFT * Col;
	FFT * Row;

//histos
	TH1D * hblc;
	enum { Nhs = 16 };
	TH1D * hs[MaxP][Nhs];
	TH1D * hfe[MaxP];
	TH1D * hmap[MaxP];
	TH1D * hpile[MaxP];
    TH1D * hclean[MaxP];
    TH1D * hYpro[MaxP];
    TH1D * hXpro[MaxP];
    TH1D * hXtst[MaxP];
    TH2D * hYX[MaxP];
	TH1F * hNp[MaxP];
	TH1F * hNph[MaxP];
	TH1F * horm[MaxP];
	TH1F * hra[MaxP];
	TH1F * hspr[MaxP];

	TH1D * hXf[MaxP][7];
	TH1D * hSig;

	enum { Nham = Co::Nsumh };  // 10 };
	TH1D * ham[MaxP][Nham];
 	TH1D * hXstat[MaxP];

    enum { Ncte = 33 };
	TH1F * xcte[MaxP][Ncte];
	TH1F * ycte[MaxP][Ncte];
	enum { Zone = 2 };
	int NgrX;
	int NgrY;

	enum { NZ   = 4 };
	int Zind;
	double NKhit[MaxP][NZ];
	double AvHit[MaxP][NZ][Co::NXpix][Co::NYpix];
	double A2Hit[MaxP][NZ][Co::NXpix][Co::NYpix];
	TH2D * AvrHit[MaxP];  //[Zone*Zone] 
    TH1D * hpixA[MaxP][Co::NXpix][Co::NYpix];

	TH2F * ClustQ[MaxP];
	TH2F * Xcte[MaxP];
	TH2F * Ycte[MaxP];

    TH1D * hitSumA[MaxP];  //test clustering
public:
	Fe55 (	string dir_name, 
			const char * outName, 
			const double * Noise, 
			const vector<double> * b_avbuf,
			const int dofft );
	void BaLiC ( DataStr * Dev);
	void BaLiS ( DataStr * Dev);
	void PlotBase( void );
    void Catalogs( void );
	static double  GGF ( double *v, double *par );  //fiting function
	static double  Gauss ( double *v, double *par );  //fiting function
	void FitG(TH1D * hm, double * Amp, double * Shift, double * Noise);
	void PlotXfit( void );
	void PlotSpectra( void );
	void PlotXlines( void );
	void PlotCTE( void );
	void PlotAvHit(void);
    void PlotProfiles(void);
	void Plot_FFT();
    void PlotHits(void);
	//int OpenOutF ( char * fOutName = "Out_bufzs" );
	//int OutFitsF ( void );
	~Fe55();
};

// initialization of static members
const float Fe55::left_margin =  (float)0.06;
const float Fe55::right_margin = (float)0.006;
const float Fe55::top_margin =   (float)0.01;
const float Fe55::bot_margin =   (float)0.04;

const int    Fe55::Npar = 4; 

Fe55::~Fe55(){ 
	//delete [] bufzs; bufzs = 0;
	//if (doFFT) {
	//	delete    Col; Col = 0;
	//	delete    Row; Row = 0;
	//}
	//Col = 0;
	//Row = 0;
}

void Fe55::Plot_FFT()
{
 	 //const float left_margin = (float)0.04;
	 //const float right_margin = (float)0.001;
	 //const float top_margin = (float)0.01;
	 //const float bot_margin = (float)0.04;

	TCanvas	*	Tr[MaxP];
for (int u=0; u<2; u++){  //Nchan
	//if ( avbuf[u].size() < npixels) continue;
	char tiname[50];
	char PDFname[50];
	sprintf(tiname, "FeFFT %i", u);
	sprintf(PDFname, "Fe55FFT_%i.pdf", u);
	if ( u > MaxP )   break;
 	 Tr[u] = new TCanvas( tiname, tiname, 200+4*u, 10+2*u, 800, 600);
	 Tr[u]->SetBorderMode  (1);			//to remove border
	 Tr[u]->SetLeftMargin  (left_margin);  //to move plot to the right 0.14
	 Tr[u]->SetRightMargin (right_margin); //to move plot to the right 0.05
	 Tr[u]->SetTopMargin   (top_margin);   //to use more space 0.07
	 Tr[u]->SetBottomMargin(bot_margin);   //default
	 Tr[u]->SetFrameFillColor(0);
	 Tr[u]->Divide(1,2); 

/***************** Plot Power Spectrum for Rows *************/
     printf(" Start draw Power Spectrum for Rows\n\n");
	 Tr[u]->cd(1);
	 gPad->SetLogy();
	 gPad->SetLogx();

	 do {
     int Npsd = Row->PSDav[u].size();
     int Nfrq = Row->Freq.size();
 	 printf(" RowPSD points =%i RowFreq size=%i *** \n", Npsd, Nfrq);
	 if ( Npsd <= 3 ) continue;
	 int nn = Npsd -3;
	 if ( Nfrq < nn ) continue;
	 TGraph * std6 = new TGraph( nn, &Row->Freq[0], &Row->PSDav[u][0]);
	 std6->SetTitle("Rows");
	 std6->GetYaxis()->SetTitle("PSD ");
	 std6->GetXaxis()->SetTitle("k/N, 1/pixel");
	 std6->SetMarkerColor(4);
	 std6->SetMarkerStyle(21);
	 std6->Draw("AP");
		 cout << "i" << " - " << "Freq" << " - " << " RowPSD" << endl;
	 for (int i=0; i<Npsd; i++){
		 cout << i << ". - " << Row->Freq[i] << " - " << Row->PSDav[u][i] << endl;
	 }
	 } while(0);

/*********************Plot Power Spectrum for Columns*********/
	 Tr[u]->cd(2);
	 gPad->SetLogy();
	 gPad->SetLogx();
 	 do {
     int Npsd = Col->PSDav[u].size();
     int Nfrq = Col->Freq.size();
 	 printf(" ColPSD points =%i ColFreq size=%i *** \n", Npsd, Nfrq);
	 if ( Npsd <= 3 ) continue;
	 int nn = Npsd -3;
	 if ( Nfrq < nn ) continue;
	 TGraph * std7 = new TGraph( nn, &Col->Freq[0], &Col->PSDav[u][0]);
	 std7->SetTitle("Columns");
	 std7->GetYaxis()->SetTitle("PSD ");
	 std7->GetXaxis()->SetTitle("k/N, 1/pixel");
	 std7->SetMarkerColor(6);
	 std7->SetMarkerStyle(21);
	 std7->Draw("AP");
	 } while(0);

	Tr[u]->Update();
	Tr[u]->Print(PDFname);
}

/***************** Plot subtraction histos *************/

	TCanvas	*	Tb = new TCanvas( "subtract", "subtract", 250, 20, 800, 600);
	 Tb->SetBorderMode  (1);			//to remove border
	 Tb->SetLeftMargin  (left_margin);  //to move plot to the right 0.14
	 Tb->SetRightMargin (right_margin); //to move plot to the right 0.05
	 Tb->SetTopMargin   (top_margin);   //to use more space 0.07
	 Tb->SetBottomMargin(bot_margin);   //default
	 Tb->SetFrameFillColor(0);
	 Tb->Divide(2,2); 

	int Entry = (int)Row->hbuf[0]->GetEntries();
	if ( Entry >1 ) {
		Tb->cd(1);
		gPad->SetLogy();
		gStyle->SetOptFit(1);
		Row->hbuf[0]->Draw();
		Row->hbuf[0]->Fit("gaus");

	}
	
	Entry = (int)Row->hbuf[1]->GetEntries();
	if ( Entry >1 ) {
		Tb->cd(2);
		gPad->SetLogy();
		gStyle->SetOptFit(1);
		Row->hbuf[1]->Draw();
		Row->hbuf[1]->Fit("gaus");
	}

	Entry = (int)Row->hbuf[2]->GetEntries();
	if ( Entry >1 ) {
		Tb->cd(3);
		gPad->SetLogy();
		Row->hbuf[2]->Draw();
	}

	Entry = (int)Row->hbuf[3]->GetEntries();
	if ( Entry >1 ) {
		Tb->cd(4);
		gPad->SetLogy();
		Row->hbuf[3]->Draw();
	}

	 return;
} // end Fe55::Plot_FFT


void Fe55::BaLiC ( DataStr * Dev )
{
	//	if (avbuf == 0){avbuf = new double[npixels]; memset(avbuf,0,npixels);} 
    printf( "\n");
    printf( "******* BaLiC  is invoked for HDU=%i channel=%i ******\n",hdu, ch_idx);
	double NoiseCut = 5.*ANoise[ch_idx];
	double act = 0;
	double b_shift = 0;
	double b_shift1 = 0;
	double b_rms = 0.;
	double peak = 0.;
    hblc->Reset();
    for (int iy=Dev->minY(); iy<Dev->maxY(); iy++) {
		for (int ix=Dev->minX(); ix<Dev->maxX(); ix++) {
		  int j = iy*nx + ix;
		  bufzs[j] = (double)buffer[j] - avbuf[ch_idx][j];
		  if ( bufzs[j] < NoiseCut ) {  //TMath::Abs(bufzs[j])
			  hblc->Fill( bufzs[j] );
			  b_shift += bufzs[j];
			  act+=1.;
		  }
		}
	}
	if (act > 0.2*npixels ) {
		b_shift /= act;
		FitG(hblc, &peak, &b_shift1, &b_rms);
		printf ("GausAv=%f rms=%f \n", b_shift1, b_rms);
		b_shift = b_shift1;
	}
	else b_shift=0.;

	printf(" shift= %f act=%f \n",  b_shift, act);

	for (int iy=0; iy<ny; iy++) {
		for (int ix=0; ix<nx; ix++) {
		  int j = iy*nx + ix;
		  bufzs[j] -= b_shift;
		  if ( Dev->IsOver(ix, iy) ) continue;
		  hfe[ch_idx]->Fill( bufzs[j] );
		}
	}
} // end Base Line Correction 

void Fe55::BaLiS ( DataStr * Dev )
{
  	printf("\n");
	printf("*********# BaLiS  is invoked for HDU=%i ch_idx=%i #********\n", hdu, ch_idx);
    printf("*********# BaLiS: Dev->: Ominx=%i Omax=%i minX=%i maxX=%i NoverX=%i \n",
           Dev->OmiX(), Dev->OmaX(), Dev->minX(), Dev->maxX(), Dev->NoverX() );

	double b_shift = 0.;
	double b_rms   = 0.;
	double peak = 0.;

// Fill input histo 
	for (int iy=0; iy<ny; iy++) {
		for (int ix=0; ix<nx; ix++) {
			long j = iy*nx + ix;
			hs[ch_idx][0]->Fill( bufzs[j] );
		}
	}
// Overscan subtraction 
    hs[ch_idx][2]->Reset();
	for (int iy=0; iy<ny; iy++) {
		double a_over = 0.;
		for (int ix=Dev->OmiX(); ix<Dev->OmaX(); ix++) {
			long j = iy*nx + ix;
			a_over += bufzs[j]/Dev->NoverX();
		}
		for (int ix=0; ix<nx; ix++) {
			long j = iy*nx + ix;
				bufzs[j] = bufzs[j] - a_over;
			if ( Dev->IsOverX(ix) ) hs[ch_idx][1]->Fill( bufzs[j] );
			else {
				hs[ch_idx][2]->Fill( bufzs[j] );
			}
		}
	}
	////b_shift = hs[ch_idx][1]->GetMean();
	////b_rms   = hs[ch_idx][1]->GetRMS();
	////printf (" OS: shift=%f rms=%f \n", b_shift, b_rms);
	FitG(hs[ch_idx][2], &peak, &b_shift, &b_rms);
    printf (" OS: GausAv=%f rms=%f \n", b_shift, b_rms);
    
   // goto skipIter;
	
// Iterative Part
	for ( int it=0; it<4; it++){
        hs[ch_idx][3+2*it]->Reset();
        hs[ch_idx][4+2*it]->Reset();

  // active row pixel average subtraction 
	double RowCut = 4.*b_rms;
	//if (it == 0) RowCut = 60000.;  //10.*b_rms;
	for (int iy=0; iy<ny; iy++) {
		double a_row = 0.;
		double n_row = 0.;
        // row loops, active pixels only
		for (int ix=Dev->minX(); ix<Dev->maxX(); ix++) {
			int j = iy*nx + ix;
			bufzs[j] -= b_shift;
			if ( !it || (bufzs[j] < RowCut) ) {a_row += bufzs[j]; n_row +=1.;}
		}
		if (n_row > 0.5*nx ) a_row /= n_row;
		else a_row = 0.;

        for (int ix=Dev->minX(); ix<Dev->maxX(); ix++) {
			int j = iy*nx + ix;
			bufzs[j] -= a_row;
			hs[ch_idx][4+2*it]->Fill( bufzs[j]);
		}
	}
	//b_shift = hs[ch_idx][4+2*it]->GetMean();  
	//b_rms   = hs[ch_idx][4+2*it]->GetRMS();
	//printf (" Column: shift=%f rms=%f \n", b_shift, b_rms);
	FitG(hs[ch_idx][4+2*it], &peak, &b_shift, &b_rms);
    printf ("Fit Noise Itr%d : Row: shift=%f rms=%f \n",it, b_shift,b_rms);

// all column pixel average subtraction 
	double ColCut = 4.*b_rms;
	//if (it == 0) ColCut = 60000.;  //10.*b_rms;
	for (int ix=0; ix<nx; ix++) {
		double a_col = 0.;
		double n_col = 0.;
       // column loops, without column overscan
		for (int iy=Dev->minY(); iy<Dev->maxY(); iy++) {
			long j = iy*nx + ix;
			bufzs[j] -= b_shift;
			if ( !it || (bufzs[j] < ColCut) ) {a_col += bufzs[j]; n_col +=1.;}
		}
		if (n_col > 0.5*ny ) a_col /= n_col;
		else a_col = 0.;

        for (int iy=Dev->minY(); iy<Dev->maxY(); iy++) {
			long j = iy*nx + ix;
			bufzs[j] -= a_col;
			//if ( Dev->IsOver(ix,iy) ) continue;  //for SIX
            hs[ch_idx][3+2*it]->Fill( bufzs[j]);
		}
        //end column loops 	
	}

	FitG(hs[ch_idx][3+2*it], &peak, &b_shift, &b_rms);
    printf ("Fit Noise Itr%d : Col: shift=%f rms=%f \n",it, b_shift,b_rms);

	} //end of "itteration" loop

//skipIter:
//    cout <<" Iterations are skipped "<< endl;
	cout <<" BaLiS noise for hdu="<<hdu<< " : " <<b_rms << endl;
	ANoise[ch_idx] = b_rms; 
    	for (int iy=0; iy<ny; iy++) {
			for (int ix=0; ix<nx; ix++) {
                long j = iy*nx + ix;
                if ( Dev->IsOver(ix, iy) ) continue;
                double amp = bufzs[j] - b_shift;
                bufzs[j] = amp;
                hfe[ch_idx]->Fill( bufzs[j]);
                hs[ch_idx][13]->Fill( bufzs[j]);
			}
		}

	// Prepare to record in the file
	//for (int ix=0; ix<nx; ix++) {
	//	for (int iy=0; iy<ny; iy++) { 
	//		long j = iy*nx + ix;
	//		bufzs[j] += 1000.;
	//		if ( bufzs[j] < 0. ) bufzs[j] = 0.;
	//		if ( bufzs[j] > 30000.) bufzs[j] = 30000.;
	//		//if ( bufzs[j] < 0.5*b_rms ) bufzs[j] = 0.;
	//		//if ( bufzs[j] > 2000. ) bufzs[j] = 0.;
	//	}
	//}

} // end Base Line Subtraction

void Fe55::PlotBase( void )
{
	printf("AIK in PlotBase\n");
	TCanvas	*	BaL[MaxP]={0};
	rPad __f;
int Entry;
for (int u=0; u<Nchan; u++){
		//if ( avbuf[u].size() < npixels) continue;
		char tiname[50];
		sprintf(tiname, "BaLiSu %i", u);
		if ( u > MaxP )   break;

// set bias canvas 1 ****************************************************
	BaL[u] = new TCanvas( tiname, tiname, 100+5*u, 100+5*u, 900, 800);
 	//fprintf(stderr," PlotBase -- %d -Canvas ID --------%d ------- \n",u,BaL[u]->GetCanvasID());
	BaL[u]->SetFrameFillColor(0);
	BaL[u]->SetBorderMode  (0);			//to remove border
	BaL[u]->SetLeftMargin  (left_margin);  //to move plot to the right 0.14
	BaL[u]->SetRightMargin (right_margin); //to move plot to the right 0.05
	BaL[u]->SetTopMargin   (top_margin);   //to use more space 0.07
	BaL[u]->SetBottomMargin(bot_margin);   //default
	BaL[u]->Divide(2,6);

	BaL[u]->SetFillStyle(4100);
	BaL[u]->SetFillColor(0);
	
	for (int i = 0; i<3; i++){
		if ( hs[u][i] == 0) continue;
		Entry = (int)hs[u][i]->GetEntries();
		if ( Entry < 1 ) continue;
  		BaL[u]->cd(i+1);
		gPad->SetLogy(); 
		hs[u][i]->Draw();
	}
    
    Entry = (int)hs[u][13]->GetEntries();
	if ( Entry >1 ) {
		BaL[u]->cd(4);
		gPad->SetLogy();
		hs[u][13]->Draw();
	}

	for (int i = 3; i<11; i++){
		if ( hs[u][i] == 0) continue;
		Entry = (int)hs[u][i]->GetEntries();
		if ( Entry < 1 ) continue;
  		BaL[u]->cd(i+2);
		gPad->SetLogy();
		hs[u][i]->Draw();
	}
	BaL[u]->Update();
	char uname[40];
	sprintf(uname,"BaLiSu_%i.pdf", u);
	//BaL[u]->Print(uname);

} // end "section" canvas loop

	return;
} //end PlotBase	

void Fe55::Catalogs( void )
{
    // create x-ray list file
    string listName = filename;
    basic_string <char>::size_type indexL;
    indexL = listName.find ( ".fits");
    listName = listName.replace ( indexL , 5 , "-xray.txt" );
    printf("Output list File: %s \n", listName.c_str());
    if (access(listName.c_str(), F_OK) != -1) {
        remove(listName.c_str());
    }
    pFlist = fopen(listName.c_str(),"a+t");
    if(pFlist == 0){
        printf("File %s can't be open \n", listName.c_str());
        Flag = -1;
        return;
    }
    fprintf(pFlist, "No  x   y   x-fit  y-fit  y-fcorr sum-reg XIP \n");

// create region file
    string regName = filename;
    basic_string <char>::size_type indexF;
    indexF = regName.find ( ".fits");
    regName = regName.replace ( indexF , 5 , ".reg" );
    printf("Output reg File: %s \n", regName.c_str());
    if (access(regName.c_str(), F_OK) != -1) {
        remove(regName.c_str());
    }
    pFreg = fopen(regName.c_str(),"a+t");
    if(pFreg == 0){
        printf("File %s can't be open \n", regName.c_str());
        Flag = -1;
        return;
    }
    
// create catalog file
    string catName = filename;
    basic_string <char>::size_type indexC;
    indexC =  catName.find ( ".fits");
    catName = catName.replace ( indexF , 5 , "_catalog.xml" );
    printf("Output cat File: %s \n", catName.c_str());
    if (access(catName.c_str(), F_OK) != -1) {
        remove(catName.c_str());
    }
    pFcat = fopen(catName.c_str(),"a+t");
    if(pFcat == 0){
        printf("File %s can't be open \n", catName.c_str());
        Flag = -1;
        return;
    }
    fprintf(pFcat, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<VOTABLE version=\"1.1\">\n<RESOURCE>\n");
    fprintf(pFcat, "<TABLE>\n");
    fprintf(pFcat, "<DESCRIPTION># X-ray catalog\n</DESCRIPTION>\n");
    fprintf(pFcat, "<FIELD name=\"PhysicalX\" >\n<DESCRIPTION>Physical X</DESCRIPTION>\n</FIELD>\n");
    fprintf(pFcat, "<FIELD name=\"PhysicalY\" >\n<DESCRIPTION>Physical Y</DESCRIPTION>\n</FIELD>\n");
    fprintf(pFcat, "<FIELD name=\"ImageX\" >\n<DESCRIPTION>Image X</DESCRIPTION>\n</FIELD>\n");
    fprintf(pFcat, "<FIELD name=\"ImageY\" >\n<DESCRIPTION>Image y coordinates</DESCRIPTION>\n</FIELD>\n");
    fprintf(pFcat, "<FIELD name=\"Amp Sum\" >\n<DESCRIPTION>Amplitude Sum</DESCRIPTION>\n</FIELD>\n");
    fprintf(pFcat, "<FIELD name=\"ChannelNumber\" >\n<DESCRIPTION>CCD Segment</DESCRIPTION>\n</FIELD>\n");
    fprintf(pFcat, "<FIELD name=\"Amp0,0\" >\n<DESCRIPTION>Amplitude 0,0</DESCRIPTION>\n</FIELD>\n");
    fprintf(pFcat, "<FIELD name=\"Amp1,0\" >\n<DESCRIPTION>Amplitude 1,0</DESCRIPTION>\n</FIELD>\n");
    fprintf(pFcat, "<FIELD name=\"Amp2,0\" >\n<DESCRIPTION>Amplitude 2,0</DESCRIPTION>\n</FIELD>\n");
    fprintf(pFcat, "<FIELD name=\"Amp0,1\" >\n<DESCRIPTION>Amplitude 0,1</DESCRIPTION>\n</FIELD>\n");
    fprintf(pFcat, "<FIELD name=\"Amp1,1\" >\n<DESCRIPTION>Amplitude 1,1</DESCRIPTION>\n</FIELD>\n");
    fprintf(pFcat, "<FIELD name=\"Amp2,1\" >\n<DESCRIPTION>Amplitude 2,1</DESCRIPTION>\n</FIELD>\n");
    fprintf(pFcat, "<FIELD name=\"Amp0,2\" >\n<DESCRIPTION>Amplitude 0,2</DESCRIPTION>\n</FIELD>\n");
    fprintf(pFcat, "<FIELD name=\"Amp1,2\" >\n<DESCRIPTION>Amplitude 1,2</DESCRIPTION>\n</FIELD>\n");
    fprintf(pFcat, "<FIELD name=\"Amp2,2\" >\n<DESCRIPTION>Amplitude 2,2</DESCRIPTION>\n</FIELD>\n");
    fprintf(pFcat, "<DATA>\n<TABLEDATA>\n");
    
    return;
}  //end Catalogs

Fe55::Fe55 (	string dir_name,
		const char * outName,
		const double * Noise,
		const vector<double> * b_avbuf,
		const int dofft ):
	HighCut(1200.),
	Nchan(0),
	Flag(-1),
	dname(dir_name),
	OutExt(outName),
	avbuf(b_avbuf),
	bufzs(0),
	doFFT(dofft),
	Col(0),
	Row(0)
{
    for (int i=0; i<MaxP; i++) { ANoise[i] =0.;}
	if (b_avbuf !=0) {              // for BaLiC and not for BaLiS
		//avbuf[0].assign(b_avbuf->begin(),b_avbuf->end()); 
		for (int i=0; i<MaxP; i++) {
			//avbuf[i].assign(b_avbuf[i].begin(),b_avbuf[i].end()); 
			AminSearch[i] = 10.*Noise[i];
			ANoise[i] = Noise[i];
			printf(" ANoise[%i](noise rms) = %f \n", i, ANoise[i]);
		}
	}

	for (int i=0; i<MaxP; i++) {
		for (int j=0; j<NZ; j++) {
			NKhit[i][j]=0.;
			for (int ix=0; ix<Co::NXpix; ix++) {
				for (int iy=0; iy<Co::NYpix; iy++) {
							AvHit[i][j][ix][iy] = 0.;
							A2Hit[i][j][ix][iy] = 0.;
				}
			}
		}
	}
// Base Line Subtraction Histograms ***********************************
	char title[40];
	char hname[50];
    sprintf(title, "base line correction");
    hblc = new TH1D( title, title, 800, -400., 400.);
    hblc->SetStats(1);
	for (int u=0; u<MaxP; u++){
		for (int i=0; i<Nhs; i++) {hs[u][i] = 0;}
		sprintf(title, "hs %i 0", u);
		sprintf(hname, "Raw Data");
		hs[u][0] = new TH1D( title, hname, 2000, 0., 4000.);
		hs[u][0]->GetXaxis()->SetNdivisions(505);
		hs[u][0]->SetStats(1);

		sprintf(title, "hs %i 1", u);
		sprintf(hname, "Pre+Overscan after overscan subtr");
		hs[u][1] = new TH1D( title, hname, 800, -400., 400.);
		hs[u][1]->SetStats(1);

		sprintf(title, "hs %i 2", u);
		sprintf(hname, "Active after overscan subtr");
		hs[u][2] = new TH1D( title, hname, 2000, -400., 1600.);
		hs[u][2]->SetStats(1);

		for (int it=0; it<4; it++){
			 sprintf(title, "hs %i %i", u, 3+2*it);
			 sprintf(hname, "Column %i", it+1);
			 hs[u][3+2*it] = new TH1D( title, hname, 2000, -400., 1600.);
			 hs[u][3+2*it]->GetXaxis()->SetNdivisions(505);
 			 hs[u][3+2*it]->SetStats(1);

			 sprintf(title, "hs %i %i", u, 4+2*it);
			 sprintf(hname, "Row %i ", it+1);
			 hs[u][4+2*it] = new TH1D( title, hname, 2000, -400., 1600.);
			 hs[u][4+2*it]->GetXaxis()->SetNdivisions(505);
			 hs[u][4+2*it]->SetStats(1);

		 }

		 sprintf(title, "hs13 %i",u);
		 sprintf(hname, "Active final");
		 hs[u][13] = new TH1D( title, hname, 2000, -400., 1600.);
		 hs[u][13]->GetXaxis()->SetNdivisions(505);
		 hs[u][13]->SetStats(1);

// histos for map sorted clusters	******************************
		sprintf(title, "hNp_%i", u);
		sprintf(hname, "Npix %i", u);
		hNp[u] = new TH1F(title,hname,32,0.,32.);
		hNp[u]->SetStats(1); 
		sprintf(title, "hNph_%i", u);
		sprintf(hname, "NpHT %i", u);
		hNph[u] = new TH1F(title,hname,32,0.,32.);
		hNph[u]->SetStats(1); 
		sprintf(title, "hOrm_%i", u);
		sprintf(hname, "Orm %i", u);
		horm[u] = new TH1F(title,hname, 40,0.,2.);
		horm[u]->SetStats(1);
		sprintf(title, "hra_%i", u);
		sprintf(hname, "Ratio %i", u);
		hra[u] = new TH1F(title,hname, 240,0.,1.05);
		//hra[u]->SetStats(1111);
		hra[u]->SetStats(0);
		sprintf(title, "hspr_%i", u);
		sprintf(hname, "SPR %i", u);
		hspr[u] = new TH1F(title,hname, 240,0.,1.2);
		//hspr[u]->SetStats(1111);
		hspr[u]->SetStats(0);

		 for (int i=0; i<Nham; i++){
			sprintf(title, "Ch%i_cluster_%i", u, i);
		switch( i )
		{
			case 0: 
				sprintf(hname, "Ch%i all pixels", u);
				break;
			case 1: 
				sprintf(hname, "Ch%i 1 pixel", u);
				break;
			case 5: 
				sprintf(hname, "Ch%i 5-%i pixels", u, Nham-1);
				break;
			default:
				sprintf(hname, "Ch%i %i pixels", u, i);
				break;
		}
            ham[u][i] = new TH1D( title, hname, 750, 0., 1500.);
			ham[u][i]->GetXaxis()->SetNdivisions(505);
			ham[u][i]->SetStats(1);
		}
        
        sprintf(title, "Seq_hit_Ch%i_cluster_SumA", u);
        sprintf(hname, "Ch%i_SumA", u);
        hitSumA[u] = new TH1D( title, hname, 150, 0., 1500.); //1000, 0., 10000.
        hitSumA[u]->GetXaxis()->SetNdivisions(505);
        hitSumA[u]->SetStats(1);

        // Fe55 Analysis histograms: ******************************************
        sprintf(hname, "pix_amp_%i", u);
        hfe[u] = new TH1D(hname,hname,2000,-200.,9800.);
        hfe[u]->SetStats(1);
        sprintf(hname, "map_amp_%i", u);
        hmap[u] = new TH1D(hname,hname,2000,-200.,9800.);
        hmap[u]->SetStats(1);
        sprintf(hname, "pileup_sum_%i", u);
        hpile[u] = new TH1D(hname,hname,1000, 0., 10000.);
        hpile[u]->SetStats(1);
        sprintf(hname, "clean_sum_%i", u);
        hclean[u] = new TH1D(hname,hname,150, 0., 1500.); //1000, 0., 10000.
        hclean[u]->SetStats(1);
        sprintf(hname, "Y_profile_%i", u);
        hYpro[u] = new TH1D(hname,hname,1600, 400., 1200.);
        hYpro[u]->SetStats(1);
        sprintf(hname, "X_profile_%i", u);
        hXpro[u] = new TH1D(hname,hname,800, 0., 1600.);
        hXpro[u]->SetStats(1);
        sprintf(hname, "X_fit_%i", u);
        hXtst[u] = new TH1D(hname,hname,200, 0.5, 2.5);
        hXtst[u]->SetStats(1);
        sprintf(hname, "YX_plot_%i", u);
        hYX[u] = new TH2D(hname,hname,80, 600., 920., 100, 500, 1500.);
        hYX[u]->SetStats(0);
        //*************
        
		sprintf(title, "X-loss %i", u);
		sprintf(hname, "X-loss %i", u);
		hXstat[u] = new TH1D( title, hname, 10, 0., 10.);
		hXstat[u]->GetXaxis()->SetNdivisions(505);
		hXstat[u]->SetStats(0);
	
		sprintf(title, "Serial Transfer %i", u);
		sprintf(hname, "Serial Transfer %i", u);
		Xcte[u] = new TH2F( title, hname, 50, 0., 550., 400, 0., 2000.);
		Xcte[u]->SetStats(0);
		sprintf(title, "Parallel Transfer %i", u);
		sprintf(hname, "Parallel Transfer %i", u);
		Ycte[u] = new TH2F( title, hname,101, 0., 2020., 400, 0., 2000.);
		Ycte[u]->SetStats(0);
		sprintf(title, "ClustQ %i", u);
		sprintf(hname, "ClustQ %i", u);
		ClustQ[u] = new TH2F( title, hname, 50, 0., 550., 101, 0., 2020.);
		ClustQ[u]->SetStats(0);

		for (int i=0; i<Ncte; i++){
			char xtitle[40];
			char ytitle[40];
			sprintf(xtitle, " %i binx %i", u, i);
			sprintf(ytitle, " %i biny %i", u, i);
			xcte[u][i] = new TH1F( xtitle, xtitle, 400, 0., 2000.);
			ycte[u][i] = new TH1F( ytitle, ytitle, 400, 0., 2000.);
			xcte[u][i]->SetStats(1);
			ycte[u][i]->SetStats(1);
		}

// average cluster and pixel amp distributions
		//for (int i=0; i<Zone*Zone; i++){
			sprintf(title, "AvHit %i", u);  //, i)
			AvrHit[u] = new TH2D(title,title, Co::NXpix, 0., Co::NXpix, Co::NYpix, 0., Co::NYpix);
			AvrHit[u]->SetStats(1);
		//}
        for (int x=0; x<Co::NXpix; x++) {
            for (int y=0; y<Co::NYpix; y++) {
                sprintf(title, "ClusterPix_%i_%i%i", u, x, y);
                hpixA[u][x][y] = new TH1D(title,title,750,-50,700);
                hpixA[u][x][y]->GetXaxis()->SetNdivisions(505);
                hpixA[u][x][y]->SetStats(1);
            }
        }

	 hXf[u][0] = new TH1D("chi2","chi2", 100, 0., 100.);
	 hXf[u][0]->SetStats(0);
	 hXf[u][6] = new TH1D("chiR","chiR", 100, 0., 50.);
	 hXf[u][6]->SetStats(0);
	 hXf[u][1] = new TH1D("SumF","SumF", 500, 0., 2000.);
	 hXf[u][1]->SetStats(1);
	 hXf[u][2] = new TH1D("xfit","xfit", 200, 0.5, 2.5);
	 hXf[u][2]->SetStats(0);
	 hXf[u][3] = new TH1D("yfit","yfit", 200, 0.5, 2.5);
	 hXf[u][3]->SetStats(0);
	 hXf[u][4] = new TH1D("sigma","sigma", 200, 0., 20.0);
	 hXf[u][4]->SetStats(0);
	 hXf[u][5] = new TH1D("SigErr","SigErr", 200, 0., 1.);
	 hXf[u][5]->SetStats(0);

	} // MaxP loop
    
    hSig = new TH1D("sigma","sigma", 200, 0., 20.0);
    hSig->SetStats(0);


// Fe55 files:  *******************************************************
	FileList FL(dname);
	if (FL.Nfile <= 0) {Flag= -1; return;}
	
// file loop: *********************************************************
    Hits * SegHits[MaxP];
    TCanvas * cF = new TCanvas("Fit2D", "Fit2D", 300,10,700,650);
	TH2F   * f2D = new TH2F("f2D","fit2", Co::NXpix, 0, Co::NXpix, Co::NYpix, 0, Co::NYpix);
	enum { Npar2DI = 4};
	TF2    * fit = new TF2("fitG2DI", G2DI, 0., Co::NXpix, 0., Co::NYpix, Npar2DI);
	cF->Divide(1,2); 

	int Nf=0;
	int FFTinit=0;
	const char *colr[8]={"white","black","red","green","blue","cyan","magenta","yellow"};

	for (FL.FName_Iter=FL.FName.begin(); FL.FName_Iter != FL.FName.end(); FL.FName_Iter++, Nf++)
	{
        int Nray = 0;
		ch_idx=-1;
		filename = *FL.FName_Iter; //  dname + "/" +
		printf(" Fe55: opening file %s \n",filename.c_str());
		if ( Open(filename.c_str()) ) continue;
		//OpenOutF();
		Catalogs();
        
		while( !iEOF ){
			if ( hdu > MaxP )   break;
			if ( Read() )    continue;
			if ( hdu == 1){
				for (int i=0; i<Nkey; i++){
					double v; 
					int ir = getValue( KeyList[i], &v );
					if (!i) v -= T1995; //convert CTIME to root time
					if (!ir) Vval[i].push_back( v ); 
				}
			}
			if ( naxis != 2) continue;

			DataStr * Dev = DataStr::Instance(nx,ny);
			XminSearch = Dev->minX() + 4;
			XmaxSearch = Dev->maxX() - 4;
			YminSearch = Dev->minY() + 4;
			YmaxSearch = Dev->maxY() - 4;
			PixSz = Dev->PixSz();

			if ( !Nf ){
				Nchan++;
				if (doFFT && (!FFTinit) ) {
					Col = new FFT(	"Fe55 Col",
									Dev->minY(), Dev->maxY(),
									Dev->minX(), Dev->maxX(),
									nx, 1 );
					Row = new FFT(	"Fe55 Row",
									Dev->minX(), Dev->maxX(),
									Dev->minY(), Dev->maxY(),
									1, nx, 87 );  //, 3.944 );  
					FFTinit=1;
				}
			}

			if (bufzs == 0) {
				bufzs = new double[npixels];
				memset(bufzs, 0, npixels * sizeof(double));
				bz_save = new double[npixels];
				memset(bz_save, 0, npixels * sizeof(double));
			}
			ch_idx++;
			if (ch_idx != 0) continue;
			//if (ch_idx>7) break;

//			BaLiC ( Dev );

			for (unsigned long i=0; i<npixels; i++){
							bufzs[i] = (double)buffer[i];}
			printf(" before BaLiS File: %s  \n", filename.c_str() );
			BaLiS ( Dev );
			printf(" ANoise[%i](noise rms) = %f \n", ch_idx, ANoise[ch_idx]);
			//break;

			if (doFFT) { 
				Col->DTrans(bufzs, ch_idx, 0); 
				Row->DTrans(bufzs, ch_idx, 1); 
				//Col->DTrans(bufzs, 1, 0); 
				Row->DTrans(bufzs, 0, 0); 
			}

			double AminSrch = 20.*ANoise[ch_idx];
			//OutFitsF( bufzs );

//*************  gain correction ***************************************
//
			 double gainC = Co::ConvGain[ch_idx];
			//AminSrch *= gainC;
			//for (unsigned long i=0; i<npixels; i++){
			//				bufzs[i] *= gainC;}

            //*************  multimap ordering ***************************************
			Qhit qhi;
			typedef pair <double, Qhit> AmapPair;
			multimap <double, Qhit, greater<double> >  Amap;
			multimap <double, Qhit, greater<double> >::iterator AIter;
			multimap <double, Qhit>::size_type Asize;
			Amap.clear();

            printf(" Start sequentional clustering: ch=%i  \n", ch_idx );
            SegHits[ch_idx]= new Hits(nx, ny, ANoise[ch_idx], 20.0 );
            OneHit::Nrpix1=SegHits[ch_idx]->Nrpix;
            OneHit::Acut1 =SegHits[ch_idx]->Acut;
            OneHit::AcutL1 =SegHits[ch_idx]->AcutL;
            for (int iy=YminSearch; iy<YmaxSearch; iy++) {
                int j0 = iy*nx;
                double * pb = &bufzs[j0];
                SegHits[ch_idx]->GetRow(pb,iy);
                OneHit::p_crow=SegHits[ch_idx]->crow;  // current row pointer
                OneHit::p_arow=SegHits[ch_idx]->arow;  // antecedent row pointer
                OneHit::jy_c=iy;    // current row index
                SegHits[ch_idx]->ClusterMatch();
                SegHits[ch_idx]->ClusterSeed();
            }
            printf(" Done sequentional clustering: ch=%i Nhits=%i \n", ch_idx, SegHits[ch_idx]->NHits);
            printf(" Hits Dump: %i %i %i %i %i \n", Hits::Wpad, Hits::Wtime,
                   SegHits[ch_idx]->Nrpix, SegHits[ch_idx]->maxHit, SegHits[ch_idx]->maxSeed);
            printf(" Hits Dump: %f %f %f %f \n", SegHits[ch_idx]->Noise,
                   SegHits[ch_idx]->Threshold, SegHits[ch_idx]->Acut, SegHits[ch_idx]->AcutL);
            printf(" Hits Dump: %i %i %i  \n", SegHits[ch_idx]->NSeeds, SegHits[ch_idx]->NHits,
            SegHits[ch_idx]->jy_c);
            
            for (int ih=0; ih<SegHits[ch_idx]->NHits; ih++){
                hitSumA[ch_idx]->Fill( SegHits[ch_idx]->hit1[ih].SumA );
                fprintf(pFreg, "# tile %i\nimage; box %i %i %i %i 0 # color = %s text={%6.0f}\n", ch_idx+1, SegHits[ch_idx]->hit1[ih].ixcntr + 1, SegHits[ch_idx]->hit1[ih].jycntr + 1, Hits::Wpad, Hits::Wtime, colr[2], SegHits[ch_idx]->hit1[ih].SumA);

            }
            delete SegHits[ch_idx]; SegHits[ch_idx]=0;
            
            for (int iy=YminSearch; iy<YmaxSearch; iy++) {
				for (int ix=XminSearch; ix<XmaxSearch; ix++) {
				  int j = iy*nx + ix;
				  double amp = bufzs[j];
				  if (amp < AminSrch ) continue;
				  qhi.Set(ix, iy);
				  Amap.insert( AmapPair( amp, qhi ) );
				}
			}
			Asize = Amap.size();
            printf(" File: %s  Ch=%i MapSize:%lu \n", filename.c_str(), ch_idx, (long unsigned)Asize);

			NgrX = (Dev->maxX() - Dev->minX())/(Ncte-1);
			NgrY = (Dev->maxY() - Dev->minY())/(Ncte-1);
			Ohit hit( XmaxSearch, XminSearch, YmaxSearch, YminSearch, ANoise[ch_idx],
						nx, ny, bufzs, cF, f2D, fit);
			for (unsigned long i=0; i<npixels; i++){ bz_save[i]=bufzs[i]; }
			for ( AIter = Amap.begin(); AIter != Amap.end(); AIter++ ){
				int x = AIter->second.ixb;
				int y = AIter->second.jyb;
				hit.Cluster(x, y, 1);
				hit.Clear();
			}
			for (unsigned long i=0; i<npixels; i++){ bufzs[i]=bz_save[i]; }

            printf(" ** second pass \n");
            for ( AIter = Amap.begin(); AIter != Amap.end(); AIter++ ){
				double amp = AIter -> first;
				int x = AIter->second.ixb;
				int y = AIter->second.jyb;
 				hmap[ch_idx]->Fill( amp );
				hit.Cluster(x, y);
                int lHit = (hit.Npix > 0) && (hit.Npix<=Ohit::NXsrch*Ohit::NYsrch); //(hit.Npix<4);
                int lMnK = 0;  //(hit.Sum*gainC > Co::KcutL) && (hit.Sum*gainC < Co::KcutR);
				if (hit.Flag == -200) hpile[ch_idx]->Fill( hit.Sum );
				if (hit.Flag < 0) { hit.Clear(); continue;}
                //assert(lHit);
                if (!lHit) continue;
                hclean[ch_idx]->Fill( hit.Sum );
                hXstat[ch_idx]->Fill(0.1);
                hNp[ch_idx]->Fill(hit.NpixH );
                //ham[ch_idx][0]->Fill( hit.Sum*gainC  );
                //ham[ch_idx][hit.NpixH]->Fill( hit.Sum*gainC  );
                ham[ch_idx][0]->Fill( hit.Sum*gainC );
                ham[ch_idx][hit.NpixH]->Fill( hit.Sum*gainC );
                Xcte[ch_idx]->Fill( x, hit.SumCTE*gainC );
                Ycte[ch_idx]->Fill( y, hit.SumCTE*gainC );
                // CTE histos
                if ( Co::doCTE ) {  //6
                    int indx=x/NgrX;
                    int indy=y/NgrY;
                    xcte[ch_idx][indx]->Fill( hit.SumCTE*gainC );
                    ycte[ch_idx][indy]->Fill( hit.SumCTE*gainC );
                }
               if ( lMnK ) {
                    hXstat[ch_idx]->Fill(1.1);
                    hNph[ch_idx]->Fill(hit.NpixH );
                    horm[ch_idx]->Fill(hit.rms);
                    hra[ch_idx]->Fill(hit.MRatio);
                    ClustQ[ch_idx]->Fill( hit.ixb, hit.jyb );
                    for (int i=0; i<Ohit::NXsrch; i++) {
                    	  for (int j=0; j<Ohit::NYsrch; j++) {
                    		if (hit.Amp[i][j]>0.1) hspr[ch_idx]->Fill(hit.Amp[i][j]/hit.Sum);
                    	  }
                      }

                    // Average Hit
						Zind=0;
						if (x > nx/2) Zind=1; 
						NKhit[ch_idx][Zind]++;
						for (int iy=0; iy<hit.NYsrch; iy++){
							for (int ix=0; ix<hit.NXsrch; ix++){
								double ai=hit.Amp[ix][iy]*gainC;  ///Sum
								AvHit[ch_idx][Zind][ix][iy] += ai;
								A2Hit[ch_idx][Zind][ix][iy] += ai*ai;
							}
						}
						Zind=2;
						if (y > ny/2) Zind=3; 
						NKhit[ch_idx][Zind]++;
						for (int iy=0; iy<hit.NYsrch; iy++){
							for (int ix=0; ix<hit.NXsrch; ix++){
								double aj=hit.Amp[ix][iy]*gainC;  ///Sum
								AvHit[ch_idx][Zind][ix][iy] += aj;
								A2Hit[ch_idx][Zind][ix][iy] += aj*aj;
							}
						}

                }  // if Ka,b
                //  cluster profile histos
//                if (hit.NpixH>4) {  //!hit.isoFlag
//                    //ham[ch_idx][0]->Fill( hit.Sum*gainC );  //hit.SumOne*gainC );
//                    for (int i=0;i<Ohit::NXsrch;i++) {
//                        for (int j=0;j<Ohit::NYsrch;j++) {
//                            hpixA[ch_idx][i][j]->Fill(hit.Amp[i][j]*gainC);
//                        }
//                    }
//                }

                if ( Co::doFit
                     && hit.Npix > 3
                     && hit.Npix<=Ohit::NXsrch*Ohit::NYsrch ) {
                	hit.FitX();
                	hXstat[ch_idx]->Fill(2.1);
                    if (hit.fitflag == 0) {
                        hXstat[ch_idx]->Fill(3.1);
                        hXf[ch_idx][0]->Fill( hit.chi2);
                        hXf[ch_idx][6]->Fill( hit.chiR);
                        if ( hit.chiR    < 40.     ) {  //chi2
                            hXstat[ch_idx]->Fill(4.1);
                            hXf[ch_idx][1]->Fill( hit.Sumf);
                            if ( (hit.Sumf >  500.) &&
                                 (hit.Sumf < 1500.) ) {
                                hXf[ch_idx][2]->Fill( hit.xfit);
                                hXf[ch_idx][3]->Fill( hit.yfit);
                                hXf[ch_idx][4]->Fill( hit.sfit*PixSz);
                                hXf[ch_idx][5]->Fill( hit.efit*PixSz);
                                if ( hit.sfit*PixSz > 3.){
                                    hYpro[ch_idx]->Fill( hit.yfitG);
                                    hXpro[ch_idx]->Fill( hit.xfitG);
                                    hXtst[ch_idx]->Fill( hit.xfitG);
                                    hYX[ch_idx]->Fill(hit.yfitG, hit.xfitG);
                                    for (int i=0;i<Ohit::NXsrch;i++) {
                                        for (int j=0;j<Ohit::NYsrch;j++) {
                                            hpixA[ch_idx][i][j]->Fill(hit.Amp[i][j]*gainC);
                                        }
                                    }
                                    //Output to list file
                                    Nray++;
                                    fprintf(pFlist, " %i %f %f %f %f %f %f %i \n",
                                            Nray, hit.xhitG+ch_idx*nx, hit.yhitG, hit.xfitG+ch_idx*nx, hit.yfitG, hit.yfitG, hit.Sum, 0);

                                }
                            }
                        }
                    }
                }  // cluster fit

                //Output to region file
                fprintf(pFreg, "# tile %i\nimage; box %i %i %i %i 0 # color = %s text={%6.0f}\n", ch_idx+1, hit.ixb + 1, hit.jyb + 1, Ohit::NXsrch, Ohit::NYsrch, colr[3], hit.Sum);
                //Output to catalog file
                double lX=(double)hit.xhit+1.0;  //hit.ixb + 1.0;  +1.0; <- to ds9 coordinate
                double lY=(double)hit.yhit+1.0;  //hit.jyb + 1.0;
                int lRegion = (lX > 1) && (lY > 1) && (lX < nx) && (lY < ny);
                if ( (abs(ltm1_1) > 0.001) && (abs(ltm2_2) > 0.001) ) {   //&& lRegion
                    double physX = (lX - ltv1)/ltm1_1;
                    double physY = (lY - ltv2)/ltm2_2;
                    if(ch_idx + 1 > 8){physX--; physY++;} //correction for channels 8- 16
                    fprintf(pFcat, "<TR><TD>%f</TD><TD>%f</TD><TD>%i</TD><TD>%i</TD><TD>%6.0f</TD><TD>%i</TD><TD>%6.0f</TD><TD>%6.0f</TD><TD>%6.0f</TD><TD>%6.0f</TD><TD>%6.0f</TD><TD>%6.0f</TD><TD>%6.0f</TD><TD>%6.0f</TD><TD>%6.0f</TD></TR>\n", physX, physY, hit.ixb + 1, hit.jyb + 1, hit.Sum, ch_idx + 1, hit.Amp[0][0], hit.Amp[1][0], hit.Amp[2][0], hit.Amp[0][1], hit.Amp[1][1], hit.Amp[2][1], hit.Amp[0][2], hit.Amp[1][2], hit.Amp[2][2]);
                    //Nfree, fitflag, chi2, chiR, Sumf, xfit, yfit, sfit, efit
                }
               
                hit.Clear();
			}
            printf(" ** segment %i done \n", ch_idx);
            
            //hit.~Ohit();
			//for (int y=0; y<Co::NYpix; y++){
			//	for (int x=0; x<Co::NXpix; x++){
			//		hit.AvHit[x][y] /= hit.NKhit;
			//		AvrHit[ch_idx]->SetBinContent(x+1,y+1,hit.AvHit[x][y]);
			//	}
			//}

		} //end HDU while loop

		//OutFitsF( bufzs );
		Close();
        fclose(pFlist);    //Close list file
        pFlist = 0;
        
        fclose(pFreg);	//Close region file
		pFreg = 0;
        
		//print last part of catalog
		fprintf(pFcat,"</TABLEDATA>\n</DATA>\n</TABLE>\n</RESOURCE>\n</VOTABLE>");
		fclose(pFcat);  //Close catalog file
		pFcat = 0;
        

		//CloseOut();

	} //end files loop

// Average FFT vectors +++++++++++++++++++++++++++++++++++++++++++++++++++++

	if (doFFT) {
		for (int u=0; u<2; u++){   //Nchan
			for(int i =1; i < Col->Nfreq; i++) {Col->PSDav[u][i] /= Nf;}
			for(int i =1; i < Row->Nfreq; i++) {Row->PSDav[u][i] /= Nf;}
		}
	}		
//  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	Flag = 0;
	return;
} // end Fe55 constructor



//*********************************************************************
// par[]
// 0 - amplitude of Gauss1
// 1 - center of Gauss1
// 2 - sigma noise in electrons
// 3 - amplitude of Gaus2
// 4 - center of Gaus2 <----- calculated from par[1]

double  Fe55::GGF ( double *v, double *par )  
{
	static const double GNorm = 1./TMath::Sqrt(2.*TMath::Pi());
	static const double sa = Co::Ea * Co::Fano/Co::w_pair;
	static const double sb = Co::Eb * Co::Fano/Co::w_pair;
	double scale = par[1]*Co::w_pair/Co::Ea;  //in adu/e-
	double par4 = par[1]*Co::Eb/Co::Ea;

	double x = v[0];
	double N2 = par[2]*par[2];   //noise in e- squared
	double s1 = TMath::Sqrt(N2 + sa)*scale;
	double s2 = TMath::Sqrt(N2 + sb)*scale;
	double argG1 = (x - par[1])/s1;
	double argG2 = (x - par4)/s2;
	double fitval =	par[0]*GNorm*TMath::Exp(-0.5*argG1*argG1)/s1 +
			par[3]*GNorm*TMath::Exp(-0.5*argG2*argG2)/s2; 

	//printf(" %f %f %f %f %f %f %f %f %f %f %f  \n", 
	//	x, fitval, par[0], par[1], par[2], par[3], 
	//	scale, s1, s2, argG1, argG2 );

	return fitval;
}

void Fe55::PlotSpectra( void )
{
	rPad __f;

    TCanvas	*	Zs[MaxP]={0};
    for (int ch_idx=0; ch_idx<Nchan; ch_idx++){
        char tiname[50];
        sprintf(tiname, "Spectra_%i", ch_idx);
        if ( ch_idx > MaxP )   break;
// zero subtracted X-ray hits ******************************************
         Zs[ch_idx] = new TCanvas( tiname, tiname, 30, 10, 900, 800);
         Zs[ch_idx]->SetBorderMode  (0);      //to remove border
         Zs[ch_idx]->SetLeftMargin  (left_margin);   //to move plot to the right 0.14
         Zs[ch_idx]->SetRightMargin (right_margin);   //to move plot to the right 0.05
         Zs[ch_idx]->SetTopMargin   (top_margin);   //to use more space 0.07
         Zs[ch_idx]->SetBottomMargin(bot_margin);   //default
         Zs[ch_idx]->SetFrameFillColor(0);
         Zs[ch_idx]->Divide(2,3);

         //TColor * c150 = new TColor(150,1.0,0.0,0.0); 
         Zs[ch_idx]->SetFillStyle(4100);
         Zs[ch_idx]->SetFillColor(0);

         Zs[ch_idx]->cd(1);
         if ( hfe[ch_idx]->GetEntries() > 1. ) {
             gPad->SetLogy(); 
             gStyle->SetOptFit(1);
             hfe[ch_idx]->SetLineColor(4);
             hfe[ch_idx]->Draw();
             hfe[ch_idx]->Fit("gaus");
             if ( hmap[ch_idx]->GetEntries() > 1. ) {
                 //gPad->SetLogy();
                 hmap[ch_idx]->SetLineColor(6);
                 hmap[ch_idx]->Draw("same");
             }
         }

         Zs[ch_idx]->cd(2);
         if ( hclean[ch_idx]->GetEntries() > 1. ) {
             gPad->SetLogy();
             hclean[ch_idx]->SetLineColor(8);
             hclean[ch_idx]->Draw();
         }

         Zs[ch_idx]->cd(3);
         if ( hpile[ch_idx]->GetEntries() > 1. ) {
             gPad->SetLogy(); 
             hpile[ch_idx]->SetLineColor(6);
             hpile[ch_idx]->Draw();
         }
        
        Zs[ch_idx]->cd(4);
        if ( hYpro[ch_idx]->GetEntries() > 1. ) {
            gPad->SetLogy();
            hYpro[ch_idx]->SetLineColor(6);
            hYpro[ch_idx]->Draw();
        }
        
        Zs[ch_idx]->cd(5);
        if ( hXpro[ch_idx]->GetEntries() > 1. ) {
            //gPad->SetLogy();
            hXpro[ch_idx]->SetLineColor(6);
            hXpro[ch_idx]->Draw();
        }

        Zs[ch_idx]->cd(6);
        if ( hYX[ch_idx]->GetEntries() > 1. ) {
            //gPad->SetLogy();
            hYpro[ch_idx]->SetLineColor(4);
            hYX[ch_idx]->Draw();
        }
        
//        Zs[ch_idx]->cd(7);
//        if ( hXtst[ch_idx]->GetEntries() > 1. ) {
//            //gPad->SetLogy();
//            hXtst[ch_idx]->SetLineColor(6);
//            hXtst[ch_idx]->Draw();
//        }
        
         Zs[ch_idx]->Update();
        sprintf(tiname,"Spectra_%i.pdf", ch_idx);
         Zs[ch_idx]->Print(tiname);
    }
} //end PlotSpectra function
	
double  Fe55::Gauss ( double *v, double *par )  
{

	double x = v[0];
	double s1 = par[2];
	if (s1 < 0.000001) s1=0.000001;
	double GNorm = 1./(TMath::Sqrt(2.*TMath::Pi())*s1);
	double argG1 = (x - par[1])/s1;
	double fitval =	par[0]*GNorm*TMath::Exp(-0.5*argG1*argG1);
	return fitval;
}

void Fe55::FitG (TH1D * hm, double * Amp, double * Shift, double * Noise)
{
//fit range *******************************************************************
	double maxpos;
	double Rmin;
	double Rmax;
	PeakRange( hm, &maxpos, &Rmin, &Rmax); 
	cout << " maxpos=" << maxpos << " Rmin=" << Rmin << " Rmax" << Rmax <<endl;
	cout << endl;
//*****************************************************************************

	const char fitname[] = "fitGauss";
	TF1 * fit = new TF1(fitname, Fe55::Gauss, Rmin, Rmax, 3);

	double maxval = hm->GetEntries(); 
	if (maxval < 1.) return;
		printf("\n Fit of %s \n",hm->GetTitle());
//		printf(" : Sum of weights = %f  Entries = %f \n", i, sw, maxval);	
	double par0 = maxval*0.75;
	double par2 = (Rmax - Rmin)/4.;  //ANoise[0];
	fit->SetParameter(0, par0); 
	fit->SetParameter(1, maxpos);   
	fit->SetParameter(2, par2);
	cout << " par0=" << par0 <<  endl;
	cout << " par1=" << maxpos << " par2=" << par2 << endl;
	cout << "fitname=" << fitname << endl;
	int fitflag = hm->Fit(fitname,"NS");
	cout << " FitFlag=" << fitflag << endl;
	*Amp   = fit->GetParameter(0);
	*Shift   = fit->GetParameter(1);
	*Noise = fit->GetParameter(2);

	return;
}


void Fe55::PlotAvHit(void)
{
	string foName = "AvHit_";
	foName += OutExt;
	foName += ".txt";
	printf(" Output File: %s  \n", foName.c_str());
	FILE * pFile = fopen (foName.c_str(),"wt");
	if (pFile == 0) {printf(" File %s cann't be open \n", foName.c_str()); Flag=-1; return;}
	fprintf (pFile,"// %s  input files from: %s \n", foName.c_str(), dname.c_str());

for (int u=0; u<Nchan; u++){
		if ( u > MaxP )   break;
		// Average Hit
		fprintf (pFile," \n *** channel: %i  \n", u);

	for (int zind=0; zind<NZ; zind++) {
			double Nev=NKhit[u][zind];
			fprintf (pFile," \n Zone: %i  Nev=%f \n", zind, Nev);
			if (Nev < 100) continue;
			for (int y=0; y<Co::NYpix; y++){
				fprintf (pFile," y=%i: ", y);
				for (int x=0; x<Co::NXpix; x++){
					double amp=AvHit[u][zind][x][y]/Nev;
					double am2=A2Hit[u][zind][x][y]/Nev;
					double rms = am2-amp*amp;
					if (rms <0.) rms=0.;
					rms=sqrt(rms);
					rms/=sqrt(Nev);
				fprintf (pFile," %f+/-%f  ", amp, rms);
				}
				fprintf (pFile,"\n");
			}
				fprintf (pFile,"\n");

	}

}
	fclose( pFile );

//TCanvas	*	Ch[MaxP]={0};
//		char tiname[50];
//		sprintf(tiname, "Ch %i", u);
//
//// cluster canvas ************************************************* 
//	 Ch[u] = new TCanvas( tiname, tiname, 50+5*u, 50+5*u, 1000, 800);
//	 Ch[u]->SetBorderMode  (0);            //to remove border
//	 Ch[u]->SetLeftMargin  (left_margin);  //to move plot to the right 0.14
//	 Ch[u]->SetRightMargin (right_margin); //to move plot to the right 0.05
//	 Ch[u]->SetTopMargin   (top_margin);   //to use more space 0.07
//	 Ch[u]->SetBottomMargin(bot_margin);   //default
//	 Ch[u]->SetFrameFillColor(0);
//	 Ch[u]->Divide(1,1);
//
//	 Ch[u]->SetFillStyle(4100);
//	 Ch[u]->SetFillColor(0);
//
//// average hit canvas ************************************************* 
//	 //for (int i=0; i<Zone*Zone; i++){
//		// Ch[u]->cd(1);  //2*i+1
//		////gPad->SetLogy(); 
//		//gPad->SetGrid(1,0);
//		//gPad->SetGridy();
//		//gPad->SetFillColor(0);
//		//gPad->SetBorderSize(2);
//		//AvrHit[u]->GetXaxis()->SetNdivisions(505);
//		//AvrHit[u]->GetYaxis()->SetNdivisions(505);
//		//AvrHit[u]->Draw("colz");
//
//		Ch[u]->cd(2);  //2*i+2
//		gPad->SetFillColor(0);
//		gPad->SetBorderSize(2);
//		AvrHit[u]->Draw("lego");
//	 //}
//
//		Ch[u]->Update();


	return;

} //end PlotAvHit	 

void Fe55::PlotXlines()
{
	double maxpos;
	double Rmin;
	double Rmax;
	
//    double NKa =0.;
//    double EKa   = 0.;
//    double Noise = 0.;
//    double NKb   =0.;
//    double era   = 0.;
//    double ca = 0.;
//    double rab = 0.;
//    int fitflag = -100.;

    // output file: *******************************************************
	string foName = "fit4_";
	foName += OutExt;
	foName += ".txt";
	printf(" Output File: %s  \n", foName.c_str());
	FILE * pFile = fopen (foName.c_str(),"wt");
	if (pFile == 0) {printf(" File %s cann't be open \n", foName.c_str()); Flag=-1; return;}
	fprintf (pFile,"// %s  input files from: %s \n", foName.c_str(), dname.c_str());

// Fe55 Segments Summary  ************************************************* 
	 TCanvas	* Cs = new TCanvas( "Fe55", "Fe55", 145, 5, 1000, 800);
	 Cs->SetBorderMode  (0);            //to remove border
	 Cs->SetLeftMargin  (left_margin);  //to move plot to the right 0.14
	 Cs->SetRightMargin (right_margin); //to move plot to the right 0.05
	 Cs->SetTopMargin   (top_margin);   //to use more space 0.07
	 Cs->SetBottomMargin(bot_margin);   //default
	 Cs->SetFrameFillColor(0);
	 Cs->Divide(4,4);

	 Cs->SetFillStyle(4100);
	 Cs->SetFillColor(0);

	for (int u=0; u < Nchan; u++){
		//fit range********************************************************************
		PeakRange( ham[u][0], &maxpos, &Rmin, &Rmax);
		
		double iGain = 1.;
		if ( maxpos > 0.000001) iGain = Co::NeKa/maxpos;
		double par2 = ANoise[u]*iGain;  //for STA --> 78.; e2v --> 20.
		cout << " maxpos=" << maxpos << " Rmin=" << Rmin << " Rmax" << Rmax << endl;
		cout << " gain estimate =" << iGain << " e-/adu" << endl;
		cout << " noise =" << par2 << " e-" << endl;
		double pKb = maxpos*Co::Eb/Co::Ea;
		if ( pKb > Rmax ) {
			Rmax = pKb + (Rmax - maxpos);
			cout <<  " new Rmax= " << Rmax << endl;
		}
		fprintf (pFile,"//Peak:  maxpos= %f  Rmin=%f  Rmax=%f \n", maxpos, Rmin, Rmax);
		fprintf (pFile,"// Channel %d estimates:  gain= %f e-/adu; noise=%f e-; rms = %f adu  \n",
                                   u, iGain, par2, ANoise[u]);
        
		const char * fitname = "fitGG";
		TF1 * fit = new TF1(fitname, Fe55::GGF, Rmin, Rmax, Npar);
        
		double maxval = ham[u][0]->GetEntries(); //ham[u][i]->GetBinContent(maxbin);
		if (maxval < 1.) continue;
		double sw = ham[u][0]->GetSumOfWeights();
		double bw = ham[u][0]->GetBinWidth(1);
		printf("\n Fit of %s \n",ham[u][0]->GetTitle());
		printf(" ham%i[0]: Sum of weights = %f  Entries = %f  BinW = %f\n",
               u, sw, maxval, bw);
		double par0 = maxval*0.75*bw;  //bw because of the root "feature"
		double par3 = maxval*0.25*bw;
		fit->SetParameter(0, par0);
		fit->SetParameter(1, maxpos);
		fit->SetParameter(2, par2);
		fit->SetParameter(3, par3);
		cout << " par0=" << par0 <<  endl;
		cout << " par1=" << maxpos << " par2=" << par2 << endl;
		cout << " par3=" << par3 <<  endl;
		cout << "fitname=" << fitname << endl;
        
		double NKa =0.;
		double EKa   = 0.;
		double Noise = 0.;
		double NKb   =0.;
		double era   = 0.;
		double ca = 0.;
		double rab = 0.;
		int fitflag = -100.; 
        
		fprintf (pFile,"//flag   N Ka     N Kb    Ka,adu   er(Ka) noise,e   Ca    Ra/b \n");
		if ( ham[u][0]->GetEntries() > 1 ) {
			Cs->cd(u+1);
			gPad->SetLogy(1); 
			gPad->SetGrid(1,0);
			gPad->SetFillColor(0);
			gPad->SetBorderSize(2);
 			ham[u][0]->SetStats(0);
			ham[u][0]->SetTitle("");
			ham[u][0]->GetXaxis()->SetNdivisions(505);
			ham[u][0]->GetXaxis()->CenterTitle();
			ham[u][0]->GetXaxis()->SetTitle("Cluster Total Amplitude, a.d.u.");
			ham[u][0]->GetXaxis()->SetLabelSize(0.05);
			ham[u][0]->GetXaxis()->SetTitleSize(0.05);
			ham[u][0]->GetYaxis()->SetNdivisions(505);
			ham[u][0]->GetYaxis()->CenterTitle();
			ham[u][0]->GetYaxis()->SetTitle("Number of clusters");
			ham[u][0]->GetYaxis()->SetLabelSize(0.05);
			ham[u][0]->GetYaxis()->SetTitleSize(0.05);
			ham[u][0]->GetYaxis()->SetTitleOffset(1.3);
 
			//ham[u][0]->Draw();
			gPad->SetBit(kMustCleanup);
            gPad->GetListOfPrimitives()->Add(ham[u][0],"");
			gPad->Modified(kTRUE);

			fitflag = ham[u][0]->Fit(fitname,"LR");   //IB 
			cout << " FitFlag=" << fitflag << endl;
			NKa   = fit->GetParameter(0)/bw;
			EKa   = fit->GetParameter(1);
			Noise = fit->GetParameter(2);
			NKb   = fit->GetParameter(3)/bw;
			era   = fit->GetParError(1);
			ca = EKa > 0.1 ? (Co::Ea/Co::w_pair)/EKa : 0.;
			rab = NKa > 1  ? (double)NKb/NKa : 0.;
		}
		fprintf (pFile," {  %i, %7.1f, %7.1f, %7.2f, %6.2f, %6.2f, %7.4f, %5.2f,", 
				   fitflag,   NKa,   NKb,   EKa,   era, Noise,    ca,   rab);
		//fprintf (pFile," {  %i, %7.1f, %7.1f, %7.2f, %7.2f, %5.2f, %5.2f, %5.2f, %5.2f,", 
		//	           fitflag,   NKa,   NKb,   EKa,   EKb, Noise,    ca,    cb,   rab);
		fprintf (pFile," }  // all clusters \n");
	}

	Cs->Update();
	Cs->Print("Fe55_all.pdf");
//** End Fe55 Segments Summary  *********************************************** 
	
    rPad __f;
	printf("In PlotXlines\n");
	TCanvas	*	Cl[MaxP]={0};
	TCanvas	*	Xbin[MaxP]={0}; 
	TCanvas	*	Ybin[MaxP]={0};
	TCanvas	*	CTEgr[MaxP]={0};

	for (int u=0; u<Nchan; u++){
		char tiname[50];
		sprintf(tiname, "Fe55_%i", u);
		if ( u > MaxP )   break;

// cluster canvas ************************************************* 
	 Cl[u] = new TCanvas( tiname, tiname, 50+5*u, 50+5*u, 1000, 800);
	 Cl[u]->SetBorderMode  (0);            //to remove border
	 Cl[u]->SetLeftMargin  (left_margin);  //to move plot to the right 0.14
	 Cl[u]->SetRightMargin (right_margin); //to move plot to the right 0.05
	 Cl[u]->SetTopMargin   (top_margin);   //to use more space 0.07
	 Cl[u]->SetBottomMargin(bot_margin);   //default
	 Cl[u]->SetFrameFillColor(0);
	 Cl[u]->Divide(4,3);

	 Cl[u]->SetFillStyle(4100);
	 Cl[u]->SetFillColor(0);

//fit range ********************************************************************
    PeakRange( ham[u][0], &maxpos, &Rmin, &Rmax); 
	
	double iGain = 1.;
	if ( maxpos > 0.000001) iGain = Co::NeKa/maxpos;
	double par2 = ANoise[u]*iGain;  //for STA --> 78.; e2v --> 20. 
	cout << " maxpos=" << maxpos << " Rmin=" << Rmin << " Rmax" << Rmax << endl;
	cout << " gain estimate =" << iGain << " e-/adu" << endl;
	cout << " noise =" << par2 << " e-" << endl;
	double pKb = maxpos*Co::Eb/Co::Ea;
	if ( pKb > Rmax ) {
		Rmax = pKb + (Rmax - maxpos);
		cout <<  " new Rmax= " << Rmax << endl;
	}
	fprintf (pFile,"//Peak:  maxpos= %f  Rmin=%f  Rmax=%f \n", maxpos, Rmin, Rmax);
	fprintf (pFile,"//Estimates:  gain= %f  noise=%f   \n", iGain, par2);

	const char * fitname = "fitGG";
	TF1 * fit = new TF1(fitname, Fe55::GGF, Rmin, Rmax, Npar); 
//*****************************************************************************

	fprintf (pFile,"// Channel %d Noise rms = %f, %f e- \n", u, ANoise[u], par2);
	fprintf (pFile,"//flag   N Ka     N Kb    Ka,adu   er(Ka) noise,e   Ca    Ra/b \n");

	for (int i=10; i<Nham; i++) ham[u][9]->Add(ham[u][i]);

	for (int i=0; i < 10; i++){
		double maxval = ham[u][i]->GetEntries(); //ham[u][i]->GetBinContent(maxbin);
		if (maxval < 1.) continue;
		double sw = ham[u][i]->GetSumOfWeights();
		double bw = ham[u][i]->GetBinWidth(1);
		printf("\n Fit of %s \n",ham[u][i]->GetTitle());
		printf(" ham%i[%i]: Sum of weights = %f  Entries = %f  BinW = %f\n", 
						u, i, sw, maxval, bw);	
		double par0 = maxval*0.75*bw;  //bw because of the root "feature"
		double par3 = maxval*0.25*bw;
		fit->SetParameter(0, par0); 
		fit->SetParameter(1, maxpos);   
		fit->SetParameter(2, par2);
		fit->SetParameter(3, par3);  
		cout << " par0=" << par0 <<  endl;
		cout << " par1=" << maxpos << " par2=" << par2 << endl;
		cout << " par3=" << par3 <<  endl;
		cout << "fitname=" << fitname << endl;

		double NKa =0.;
		double EKa   = 0.;
		double Noise = 0.;
		double NKb   =0.;
		double era   = 0.;
		double ca = 0.;
		double rab = 0.;
		int fitflag = -100.; 

		if ( ham[u][i]->GetEntries() > 1 ) {
			Cl[u]->cd(i+1);
			gPad->SetLogy(1); 
			gPad->SetGrid(1,0);
			gPad->SetFillColor(0);
			gPad->SetBorderSize(2);
			ham[u][i]->SetStats(0);
			ham[u][i]->SetTitle("");
			ham[u][i]->GetXaxis()->SetNdivisions(505);
			ham[u][i]->GetXaxis()->CenterTitle();
			ham[u][i]->GetXaxis()->SetTitle("Cluster Total Amplitude, a.d.u.");
			ham[u][i]->GetXaxis()->SetLabelSize(0.05);
			ham[u][i]->GetXaxis()->SetTitleSize(0.05);
			ham[u][i]->GetYaxis()->SetNdivisions(505);
			ham[u][i]->GetYaxis()->CenterTitle();
			ham[u][i]->GetYaxis()->SetTitle("Number of clusters");
			ham[u][i]->GetYaxis()->SetLabelSize(0.05);
			ham[u][i]->GetYaxis()->SetTitleSize(0.05);
			ham[u][i]->GetYaxis()->SetTitleOffset(1.3);
 
			// ham[u][i]->Draw();
			gPad->SetBit(kMustCleanup);
			gPad->GetListOfPrimitives()->Add(ham[u][i],"");
			gPad->Modified(kTRUE);

			fitflag = ham[u][i]->Fit(fitname,"LR");   //IB 
			cout << " FitFlag=" << fitflag << endl;
			NKa   = fit->GetParameter(0)/bw;
			EKa   = fit->GetParameter(1);
			Noise = fit->GetParameter(2);
			if ( (i>1) & (i<10) ) Noise /= TMath::Sqrt((double)i);
			NKb   = fit->GetParameter(3)/bw;
			era   = fit->GetParError(1);
			ca = EKa > 0.1 ? (Co::Ea/Co::w_pair)/EKa : 0.;
			rab = NKa > 1  ? (double)NKb/NKa : 0.;
		}
		fprintf (pFile," {  %i, %7.1f, %7.1f, %7.2f, %6.2f, %6.2f, %7.4f, %5.2f,",
				   fitflag,   NKa,   NKb,   EKa,   era, Noise,    ca,   rab);
		//fprintf (pFile," {  %i, %7.1f, %7.1f, %7.2f, %7.2f, %5.2f, %5.2f, %5.2f, %5.2f,", 
		//	           fitflag,   NKa,   NKb,   EKa,   EKb, Noise,    ca,    cb,   rab);
		if (i) fprintf (pFile," }  // %i pixel clusters \n", i);
		else fprintf (pFile," }  // all clusters \n");
	}
	 
	if ( hNp[u]->GetEntries() > 1 ) {
		Cl[u]->cd(11);
		gPad->SetLogy(0);
		gPad->SetGrid(1,0);
		gPad->SetFillColor(0);
		gPad->SetBorderSize(2);
		//gPad->SetLogy(); 
		hNp[u]->SetStats(0);
		hNp[u]->SetTitle("");
		//hNp[u]->SetLineColor(4); 
		//hNp[u]->SetMarkerColor(4);
		//hNp[u]->SetMarkerStyle(21);
		hNp[u]->GetXaxis()->SetNdivisions(505);
		hNp[u]->GetXaxis()->CenterTitle();
		hNp[u]->GetXaxis()->SetTitle("Number of fired pixels");
		hNp[u]->GetXaxis()->SetLabelSize(0.05);
		hNp[u]->GetXaxis()->SetTitleSize(0.06);
		hNp[u]->GetYaxis()->SetNdivisions(505);
		hNp[u]->GetYaxis()->CenterTitle();
		hNp[u]->GetYaxis()->SetTitle("Number of clusters");
		hNp[u]->GetYaxis()->SetLabelSize(0.05);
		hNp[u]->GetYaxis()->SetTitleSize(0.06);
		hNp[u]->GetYaxis()->SetTitleOffset(1.7);

		hNp[u]->Draw();
	}
	fprintf (pFile,"\n Npix LT: entries=%f mean=%f rms=%f \n", 
		hNp[u]->GetEntries(), hNp[u]->GetMean(), hNp[u]->GetRMS() );

	//if ( hNph[u]->GetEntries() > 1 ) {
	//	Cl[u]->cd(8);
	//	gPad->SetLogy(0);
	//	gPad->SetGrid(1,0);
	//	gPad->SetFillColor(0);
	//	gPad->SetBorderSize(2);
	//	//gPad->SetLogy(); 
	//	hNph[u]->SetStats(0);
	//	hNph[u]->SetTitle("");
	//	//hNph[u]->SetLineColor(4); 
	//	//hNph[u]->SetMarkerColor(4);
	//	//hNph[u]->SetMarkerStyle(21);
	//	hNph[u]->GetXaxis()->SetNdivisions(505);
	//	hNph[u]->GetXaxis()->CenterTitle();
	//	hNph[u]->GetXaxis()->SetTitle("Number of fired pixels");
	//	hNph[u]->GetXaxis()->SetLabelSize(0.05);
	//	hNph[u]->GetXaxis()->SetTitleSize(0.06);
	//	hNph[u]->GetYaxis()->SetNdivisions(505);
	//	hNph[u]->GetYaxis()->CenterTitle();
	//	hNph[u]->GetYaxis()->SetTitle("Number of HT clusters");
	//	hNph[u]->GetYaxis()->SetLabelSize(0.05);
	//	hNph[u]->GetYaxis()->SetTitleSize(0.06);
	//	hNph[u]->GetYaxis()->SetTitleOffset(1.7);

	//	hNph[u]->Draw();
	//}
	fprintf (pFile,"\n Npix HT: entries=%f mean=%f rms=%f \n", 
		hNph[u]->GetEntries(), hNph[u]->GetMean(), hNph[u]->GetRMS() );

	fprintf (pFile,"\n Xstat: %f, %f, %f, %f, %f \n", 
		hXstat[u]->GetBinContent(1),  hXstat[u]->GetBinContent(2),
		hXstat[u]->GetBinContent(3),  hXstat[u]->GetBinContent(4),
		hXstat[u]->GetBinContent(5));
	printf ("\n Xstat: %f, %f, %f, %f, %f \n", 
		hXstat[u]->GetBinContent(1),  hXstat[u]->GetBinContent(2),
		hXstat[u]->GetBinContent(3),  hXstat[u]->GetBinContent(4),
		hXstat[u]->GetBinContent(5) );

	//Cl[u]->cd(9);
	//gPad->SetLogy(0); 
	//gPad->SetGrid(1,0);
	//hspr[u]->Draw();

	// Cl[u]->Modified();
	Cl[u]->Update();
	char uname[40];
	sprintf(uname,"Cl_Fe55_%i.pdf", u);
	Cl[u]->Print(uname);

    if ( Co::doCTE) {
    // CTE_X fit canvas *************************************************
	sprintf(tiname, "Xbin_%i", u);
	Xbin[u] = new TCanvas( tiname, tiname, 100+2*u, 75+2*u, 1000, 800);
	Xbin[u]->SetBorderMode  (0);            //to remove border
	Xbin[u]->SetLeftMargin  (left_margin);  //to move plot to the right 0.14
	Xbin[u]->SetRightMargin (right_margin); //to move plot to the right 0.05
	Xbin[u]->SetTopMargin   (top_margin);   //to use more space 0.07
	Xbin[u]->SetBottomMargin(bot_margin);   //default
	Xbin[u]->SetFrameFillColor(0);
	Xbin[u]->Divide(8,4);

	Xbin[u]->SetFillStyle(4100);
	Xbin[u]->SetFillColor(0);

	cout << " maxpos=" << maxpos << " Rmin=" << Rmin << " Rmax" << Rmax << endl;

	double Sa = 0.;
	double X  = 0.;
	double X2 = 0.;
	double Y  = 0.;
	double XY = 0.;
	fprintf (pFile,"  \n");
	double xptX[Ncte]={0.};
	double xerX[Ncte]={0.};
	double yptX[Ncte]={0.};
	double yerX[Ncte]={0.};
    int NptX = 0;
	for (int i=0; i<(Ncte-1); i++){
		double maxval = xcte[u][i]->GetEntries(); //xcte[i]->GetBinContent(maxbin);
		if (maxval < 1.) continue;
		double sw = xcte[u][i]->GetSumOfWeights();
		printf("\n Fit of %s \n",xcte[u][i]->GetTitle());
		printf(" xcte[%i]: Sum of weights = %f  Entries = %f \n", i, sw, maxval);	
		double par0 = maxval*0.75;
        double par3 = par0/10.;  //maxval/4.;
		fit->SetParameter(0, par0); 
        fit->SetParLimits(0,0.,par0*100.);
		fit->SetParameter(1, maxpos);
        fit->SetParLimits(1,0.,maxpos*10.);
		fit->SetParameter(2, par2);
        fit->SetParLimits(2,0.,par2*100.);
		fit->SetParameter(3, par3);
        //fit->SetParLimits(3,0.,par3*100.);
		cout << " par0=" << par0 <<  endl;
		cout << " par1=" << maxpos << " par2=" << par2 << endl;
		cout << " par3=" << par3 << endl;

		Xbin[u]->cd(1+i);
		gPad->SetLogy(); 
		gPad->SetGrid(1,0);
		gPad->SetFillColor(0);
		gPad->SetBorderSize(2);
		xcte[u][i]->Draw();
		int fitflag = xcte[u][i]->Fit("fitGG","LIBr");
		cout << " FitFlag=" << fitflag << endl;
		double NKa   = fit->GetParameter(0);
		double EKa   = fit->GetParameter(1);
		double Noise = fit->GetParameter(2);
		double NKb   = fit->GetParameter(3);
		//double EKb   = fit->GetParameter(4);
		double era   = fit->GetParError(1);
		//double erb   = fit->GetParError(4);
		double ca = EKa > 0.1 ? (Co::Ea/Co::w_pair)/EKa : 0.;
		//double cb = EKb > 0.1 ? (Co::Eb/Co::w_pair)/EKb : 0.;
		double rab = NKa > 1  ? (double)NKb/NKa : 0.;
		fprintf (pFile," {  %i, %7.1f, %7.1f, %7.2f, %6.2f, %6.2f, %5.2f, %5.2f,", 
			       fitflag,   NKa,   NKb,   EKa,   era, Noise,    ca,   rab);
		//fprintf (pFile," {  %i, %7.1f, %7.1f, %7.2f, %7.2f, %5.2f, %5.2f, %5.2f, %5.2f,", 
		//	           fitflag,   NKa,   NKb,   EKa,   EKb, Noise,    ca,    cb,   rab);
		fprintf (pFile," }  // Xbin# %i %i\n", u, i);
		double s2 =(EKa/era)*(EKa/era); 
        if (fitflag == 0 ) {
            Sa += s2;
            X  += (i*NgrX+1)*s2;
            X2 += (i*NgrX+1)*(i*NgrX+1)*s2;
            Y  += log(EKa)*s2;
            XY += (i*NgrX+1)*log(EKa)*s2;
            xptX[NptX]=i*NgrX+1;
            yptX[NptX]=EKa;
            yerX[NptX]=era;
            NptX++;
        }
	}
	double detX = Sa*X2 - X*X;
	double aX=0.;
	double bX=0.;
    double ebX=0.;
	if ( TMath::Abs(detX) > 0.000001) {
		aX = (Y*X2 - X*XY)/detX;
		bX = (Sa*XY - X*Y)/detX;
        ebX = Sa/detX;
        if (ebX > 0) {ebX = sqrt(ebX);}
	}
	double Xcte_fit = exp(bX);
    double eCTEx = TMath::Abs(Xcte_fit)*ebX;
	double A0X = exp(aX)*NgrX*(1.-Xcte_fit)/(1.-TMath::Power(Xcte_fit,NgrX));
    fprintf (pFile," \n CTEX %12.8f +/- %12.8f  \n", Xcte_fit, eCTEx );
	fprintf (pFile," \n NgrX=%i NptX=%i A0=%f \n", NgrX, NptX, A0X );
	fprintf (pFile," \n CteXin=%f CteCorr=%f CteXnew=%f \n", CteX, Xcte_fit, CteX*Xcte_fit);

	Xbin[u]->Update();
	sprintf(uname,"Xbin_Fe55_%i.pdf", u);
	Xbin[u]->Print(uname);
    //delete Xbin[u];
//
// CTE_Y fit canvas ************************************************* 
	sprintf(tiname, "Ybin_%i", u);
	Ybin[u] = new TCanvas( tiname, tiname, 150+2*u, 100+2*u, 1000, 800);
	Ybin[u]->SetBorderMode  (0);            //to remove border
	Ybin[u]->SetLeftMargin  (left_margin);  //to move plot to the right 0.14
	Ybin[u]->SetRightMargin (right_margin); //to move plot to the right 0.05
	Ybin[u]->SetTopMargin   (top_margin);   //to use more space 0.07
	Ybin[u]->SetBottomMargin(bot_margin);   //default
	Ybin[u]->SetFrameFillColor(0);
	Ybin[u]->Divide(8,4);

	Ybin[u]->SetFillStyle(4100);
	Ybin[u]->SetFillColor(0);

	cout << " maxpos=" << maxpos << " Rmin=" << Rmin << " Rmax" << Rmax << endl;

	Sa = 0.;
	X  = 0.;
	X2 = 0.;
	Y  = 0.;
	XY = 0.;
	fprintf (pFile,"  \n");
	double xptY[Ncte]={0.};
	double xerY[Ncte]={0.};
	double yptY[Ncte]={0.};
	double yerY[Ncte]={0.};
    int NptY=0;
	for (int i=0; i<(Ncte-1); i++){
		double maxval = ycte[u][i]->GetEntries(); //ycte[u][i]->GetBinContent(maxbin);
		if (maxval < 1.) continue;
		double sw = ycte[u][i]->GetSumOfWeights();
		printf("\n Fit of %s \n",ycte[u][i]->GetTitle());
		printf(" ycte[%i]: Sum of weights = %f  Entries = %f \n", i, sw, maxval);	
		double par0 = maxval*0.75;
        double par3 = par0/10.;  //maxval/4.;
        fit->SetParameter(0, par0);
        fit->SetParLimits(0, 0.,par0*100.);
        fit->SetParameter(1, maxpos);
        fit->SetParLimits(1, 0.,maxpos*10.);
        fit->SetParameter(2, par2);
        fit->SetParLimits(2, 0.,par2*100.);
        fit->SetParameter(3, par3);
        //fit->SetParLimits(3, 0.,par3*100.);
		cout << " par0=" << par0 <<  endl;
		cout << " par1=" << maxpos << " par2=" << par2 << endl;
		cout << " par3=" << par3 << endl;

		Ybin[u]->cd(1+i);
		gPad->SetLogy(); 
		gPad->SetGrid(1,0);
		gPad->SetFillColor(0);
		gPad->SetBorderSize(2);
		ycte[u][i]->Draw();
		int fitflag = ycte[u][i]->Fit("fitGG","LIBr");
		cout << " FitFlag=" << fitflag << endl;
		double NKa   = fit->GetParameter(0);
		double EKa   = fit->GetParameter(1);
		double Noise = fit->GetParameter(2);
		double NKb   = fit->GetParameter(3);
		//double EKb   = fit->GetParameter(4);
		double era   = fit->GetParError(1);
		//double erb   = fit->GetParError(4);
		double ca = EKa > 0.1 ? (Co::Ea/Co::w_pair)/EKa : 0.;
		//double cb = EKb > 0.1 ? (Co::/Co::w_pair)/EKb : 0.;
		double rab = NKa > 1  ? (double)NKb/NKa : 0.;
		fprintf (pFile," {  %i, %7.1f, %7.1f, %7.2f, %6.2f, %6.2f, %5.2f, %5.2f,", 
			       fitflag,   NKa,   NKb,   EKa,   era, Noise,    ca,   rab);
		//fprintf (pFile," {  %i, %7.1f, %7.1f, %7.2f, %7.2f, %5.2f, %5.2f, %5.2f, %5.2f,", 
		//	           fitflag,   NKa,   NKb,   EKa,   EKb, Noise,    ca,    cb,   rab);
		fprintf (pFile," }  // Xbin# %i \n", i);
		double s2 =(EKa/era)*(EKa/era); 
        if (fitflag == 0 ) {
            Sa += s2;
            X  += (i*NgrY+1)*s2;
            X2 += (i*NgrY+1)*(i*NgrY+1)*s2;
            Y  += log(EKa)*s2;
            XY += (i*NgrY+1)*log(EKa)*s2;
            xptY[NptY]=i*NgrY+1;
            yptY[NptY]=EKa;
            yerY[NptY]=era;
            NptY++;
        }
	}
	double detY = Sa*X2 - X*X;
	double aY=0.;
	double bY=0.;
    double ebY=0.;
	if ( TMath::Abs(detY) > 0.000001) {
		aY = (Y*X2 - X*XY)/detY;
		bY = (Sa*XY - X*Y)/detY;
        ebY = Sa/detY;
        if (ebY > 0.) {ebY =sqrt(ebY);}
	}
	double Ycte_fit = exp(bY);
    double eCTEy = TMath::Abs(Ycte_fit)*ebY;
	double A0Y = exp(aY)*NgrY*(1.-Ycte_fit)/(1.-TMath::Power(Ycte_fit,NgrY));
    fprintf (pFile," \n CTEY %12.8f +/- %12.8f  \n", Ycte_fit, eCTEy );
    fprintf (pFile," \n NgrY=%i NptY=%i A0y=%f \n", NgrY, NptX, A0Y );
	fprintf (pFile," \n CteYin=%f CteCorr=%f CteYnew=%f \n", CteY, Ycte_fit, CteY*Ycte_fit);

	delete Ybin[u];
	//Ybin[u]->Update();
	//sprintf(uname,"Ybin_Fe55_%i.pdf", u);
	//Ybin[u]->Print(uname);

// CTE Graph canvas ************************************************* 
	sprintf(tiname, "CTE_%i", u);
	CTEgr[u] = new TCanvas( tiname, tiname, 110+2*u, 80+2*u, 1000, 800);
	CTEgr[u]->SetBorderMode  (0);            //to remove border
	CTEgr[u]->SetLeftMargin  (left_margin);  //to move plot to the right 0.14
	CTEgr[u]->SetRightMargin (right_margin); //to move plot to the right 0.05
	CTEgr[u]->SetTopMargin   (top_margin);   //to use more space 0.07
	CTEgr[u]->SetBottomMargin(bot_margin);   //default
	CTEgr[u]->SetFrameFillColor(0);
	CTEgr[u]->Divide(2,2);

	CTEgr[u]->SetFillStyle(4100);
	CTEgr[u]->SetFillColor(0);
	gStyle->SetPalette(1);

	CTEgr[u]->cd(1);
	gPad->SetGrid(1,0);
	gPad->SetGridy();
	gPad->SetFillColor(0);
	gPad->SetBorderSize(2);
	Xcte[u]->GetXaxis()->SetNdivisions(505);
	Xcte[u]->GetXaxis()->CenterTitle();
	Xcte[u]->GetXaxis()->SetTitle("Column number");
	Xcte[u]->GetXaxis()->SetLabelSize(0.04);
	Xcte[u]->GetXaxis()->SetTitleSize(0.04);
	Xcte[u]->GetYaxis()->SetNdivisions(505);
	Xcte[u]->GetYaxis()->CenterTitle();
	Xcte[u]->GetYaxis()->SetTitle("X-ray signal, a.d.u.");
	Xcte[u]->GetYaxis()->SetLabelSize(0.04);
	Xcte[u]->GetYaxis()->SetTitleSize(0.04);
	Xcte[u]->GetYaxis()->SetTitleOffset(1.4);
	Xcte[u]->Draw("colz");

	CTEgr[u]->cd(3);
	gPad->SetFillColor(0);
	gPad->SetBorderSize(2);
	TGraph * Xgr = new TGraphErrors(NptX, xptX, yptX, xerX, yerX);  // Ncte-1
 	Xgr->SetTitle("serial transfer graph"); 
	Xgr->SetMarkerColor(4);
	Xgr->SetMarkerStyle(21);
	Xgr->GetXaxis()->SetNdivisions(505);
	Xgr->GetXaxis()->CenterTitle();
	Xgr->GetXaxis()->SetTitle("Column number");
	Xgr->GetXaxis()->SetLabelSize(0.04);
	Xgr->GetXaxis()->SetTitleSize(0.04);
	Xgr->GetYaxis()->SetNdivisions(505);
	Xgr->GetYaxis()->CenterTitle();
	Xgr->GetYaxis()->SetTitle("K_{#alpha} signal, a.d.u.");
	Xgr->GetYaxis()->SetLabelSize(0.04);
	Xgr->GetYaxis()->SetTitleSize(0.04);
	Xgr->GetYaxis()->SetTitleOffset(1.4);
	Xgr->Draw("ALP");
//fit X curve
	double xmin = 1.;		
	TF1 * fitX = new TF1("fitX","exp([0]+[1]*x)", xmin, XmaxSearch);
    fitX->SetParameter(0, aX);
    fitX->SetParameter(1, bX);
	fitX->SetLineWidth(2);
	fitX->Draw("SAME");

	CTEgr[u]->cd(2);
	gPad->SetGrid(1,0);
	gPad->SetGridy();
	gPad->SetFillColor(0);
	gPad->SetBorderSize(2);
	Ycte[u]->GetXaxis()->SetNdivisions(505);
	Ycte[u]->GetXaxis()->CenterTitle();
	Ycte[u]->GetXaxis()->SetTitle("Row number");
	Ycte[u]->GetXaxis()->SetLabelSize(0.04);
	Ycte[u]->GetXaxis()->SetTitleSize(0.04);
	Ycte[u]->GetYaxis()->SetNdivisions(505);
	Ycte[u]->GetYaxis()->CenterTitle();
	Ycte[u]->GetYaxis()->SetTitle("X-ray signal, a.d.u.");
	Ycte[u]->GetYaxis()->SetLabelSize(0.04);
	Ycte[u]->GetYaxis()->SetTitleSize(0.04);
	Ycte[u]->GetYaxis()->SetTitleOffset(1.4);
	Ycte[u]->Draw("colz");

	CTEgr[u]->cd(4);
	gPad->SetFillColor(0);
	gPad->SetBorderSize(2);
	TGraph * Ygr = new TGraphErrors(NptY, xptY, yptY, xerY, yerY);
	Ygr->SetMarkerColor(4);
	Ygr->SetMarkerStyle(21);
 	Ygr->SetTitle("parallel transfer graph"); 
	Ygr->GetXaxis()->SetNdivisions(505);
	Ygr->GetXaxis()->CenterTitle();
	Ygr->GetXaxis()->SetTitle("Row number");
	Ygr->GetXaxis()->SetLabelSize(0.04);
	Ygr->GetXaxis()->SetTitleSize(0.04);
	Ygr->GetYaxis()->SetNdivisions(505);
	Ygr->GetYaxis()->CenterTitle();
	Ygr->GetYaxis()->SetTitle("K_{#alpha} signal, a.d.u.");
	Ygr->GetYaxis()->SetLabelSize(0.04);
	Ygr->GetYaxis()->SetTitleSize(0.04);
	Ygr->GetYaxis()->SetTitleOffset(1.4);
	Ygr->Draw("ALP");
//fit Y curve
	TF1 * fitY = new TF1("fitY","exp([0]+[1]*x)", xmin, YmaxSearch);
	fitY->SetParameter(0, aY);
	fitY->SetParameter(1, bY);
	fitY->SetLineWidth(2);
	fitY->Draw("SAME");

	CTEgr[u]->Update();
//	CTEgr[u]->Print("CTEgr.pdf");
////*****************************************************************************
}
//	End: printf( "Jumped to End. \n" );

} //end channel loop

	fclose( pFile );
    
    return;
} //end PlotXlines function


//*********************************************************************
void Fe55::PlotXfit( void )
{
    double maxpos;
	double Rmin;
	double Rmax;
    
//    double NKa =0.;
//    double EKa   = 0.;
//    double Noise = 0.;
//    double NKb   =0.;
//    double era   = 0.;
//    double ca = 0.;
//    double rab = 0.;
    int fitflag = -100.;
    
	rPad __f;
	printf("In PlotXfit\n");
    // Xfit Segments Summary  *************************************************
    TCanvas	* Cx = new TCanvas( "Xfit", "Xfit", 145, 5, 1000, 800);
    Cx->SetBorderMode  (0);            //to remove border
    Cx->SetLeftMargin  (left_margin);  //to move plot to the right 0.14
    Cx->SetRightMargin (right_margin); //to move plot to the right 0.05
    Cx->SetTopMargin   (top_margin);   //to use more space 0.07
    Cx->SetBottomMargin(bot_margin);   //default
    Cx->SetFrameFillColor(0);
    Cx->Divide(6,3);
    
    Cx->SetFillStyle(4100);
    Cx->SetFillColor(0);
    
	for (int u=0; u <16 ; u++){  //Nchan
        hSig->Add(hXf[u][4]);
		if ( hXf[u][4]->GetEntries() > 1 ) {
			Cx->cd(u+1);
			//gPad->SetLogy();
			gPad->SetGrid(1,0);
			gPad->SetFillColor(0);
			gPad->SetBorderSize(0);
			hXf[u][4]->SetStats(0);
			hXf[u][4]->SetTitle("");
			//hXf[u][4]->SetLineColor(4);
			//hXf[u][4]->SetMarkerColor(4);
			//hXf[u][4]->SetMarkerStyle(21);
			//hXf[u][4]->GetXaxis()->SetNdivisions(505);
			hXf[u][4]->GetXaxis()->SetTitle("#sigma, #mum");
			hXf[u][4]->GetXaxis()->SetLabelSize(0.05);
			hXf[u][4]->GetXaxis()->SetTitleSize(0.05);
			hXf[u][4]->GetYaxis()->SetNdivisions(505);
			hXf[u][4]->GetYaxis()->SetTitle("Number of clusters");
			hXf[u][4]->GetYaxis()->SetLabelSize(0.05);
			hXf[u][4]->GetYaxis()->SetTitleSize(0.05);
			hXf[u][4]->GetYaxis()->SetTitleOffset(1.3);
            
			hXf[u][4]->Draw();
            
			//gPad->SetBit(kMustCleanup);
            //gPad->GetListOfPrimitives()->Add(ham[u][0],"");
			//gPad->Modified(kTRUE);
            
		} //entries
	}  //channels
    
    Cx->cd(17);
    //gPad->SetLogy();
    gPad->SetGrid(1,0);
    gPad->SetFillColor(0);
    gPad->SetBorderSize(0);
    hSig->SetStats(0);
    hSig->SetTitle("");
    //hSig->SetLineColor(4);
    //hSig->SetMarkerColor(4);
    //hSig->SetMarkerStyle(21);
    //hSig->GetXaxis()->SetNdivisions(505);
    hSig->GetXaxis()->SetTitle("#sigma, #mum");
    hSig->GetXaxis()->SetLabelSize(0.05);
    hSig->GetXaxis()->SetTitleSize(0.05);
    hSig->GetYaxis()->SetNdivisions(505);
    hSig->GetYaxis()->SetTitle("Number of clusters");
    hSig->GetYaxis()->SetLabelSize(0.05);
    hSig->GetYaxis()->SetTitleSize(0.05);
    hSig->GetYaxis()->SetTitleOffset(1.3);
    
    hSig->Draw();
    
	Cx->Update();
	Cx->Print("Xfit_all.pdf");
    
    // individual segments ***********************************************
	char tiname[50];
	TCanvas	*	Cfit[MaxP]={0};
    
	for (int u=0; u<Nchan; u++){
		sprintf(tiname, "Fit %i", u);
		if ( u > MaxP )   break;
		sprintf(tiname, "Xfit_%i", u);
        
        // X-ray cluster fit canvas ******************************************
        Cfit[u] = new TCanvas( tiname, tiname, 50+2*u, 10+2*u, 900, 800);
        Cfit[u]->SetBorderMode  (0);      //to remove border
        Cfit[u]->SetLeftMargin  (left_margin);   //to move plot to the right 0.14
        Cfit[u]->SetRightMargin (right_margin);   //to move plot to the right 0.05
        Cfit[u]->SetTopMargin   (top_margin);   //to use more space 0.07
        Cfit[u]->SetBottomMargin(bot_margin);   //default
        Cfit[u]->SetFrameFillColor(0);
        Cfit[u]->Divide(2,3);
        Cfit[u]->Update();
        
        Cfit[u]->SetFillStyle(4100);
        Cfit[u]->SetFillColor(0);
        gStyle->SetOptFit();
        
        Cfit[u]->cd(1);
        gPad->SetFillColor(0);
        gPad->SetBorderSize(2);
        //	if ( hXf[u][0]->GetEntries() > 1 ) {
        //		//gPad->SetLogy();
        //		hXf[u][0]->SetStats(0);
        //		hXf[u][0]->SetTitle("");
        //		//hXf[u][0]->SetLineColor(4);
        //		//hXf[u][0]->SetMarkerColor(4);
        //		//hXf[u][0]->SetMarkerStyle(21);
        //		hXf[u][0]->GetXaxis()->SetNdivisions(505);
        //		hXf[u][0]->GetXaxis()->SetTitle("#chi^{2}");
        //		hXf[u][0]->GetXaxis()->SetLabelSize(0.05);
        //		hXf[u][0]->GetXaxis()->SetTitleSize(0.05);
        //		hXf[u][0]->GetYaxis()->SetNdivisions(505);
        //		hXf[u][0]->GetYaxis()->SetTitle("Number of clusters");
        //		hXf[u][0]->GetYaxis()->SetLabelSize(0.05);
        //		hXf[u][0]->GetYaxis()->SetTitleSize(0.05);
        //		hXf[u][0]->GetYaxis()->SetTitleOffset(1.3);
        //
        //		hXf[u][0]->Draw();
        //	}
        
        if ( hXf[u][6]->GetEntries() > 1 ) {
            //gPad->SetLogy();
            hXf[u][6]->SetStats(0);
            hXf[u][6]->SetTitle("");
            //hXf[u][6]->SetLineColor(4);
            //hXf[u][6]->SetMarkerColor(4);
            //hXf[u][6]->SetMarkerStyle(21);
            hXf[u][6]->GetXaxis()->SetNdivisions(505);
            hXf[u][6]->GetXaxis()->SetTitle("#chi^{2}R");
            hXf[u][6]->GetXaxis()->SetLabelSize(0.05);
            hXf[u][6]->GetXaxis()->SetTitleSize(0.05);
            hXf[u][6]->GetYaxis()->SetNdivisions(505);
            hXf[u][6]->GetYaxis()->SetTitle("Number of clusters");
            hXf[u][6]->GetYaxis()->SetLabelSize(0.05);
            hXf[u][6]->GetYaxis()->SetTitleSize(0.05);
            hXf[u][6]->GetYaxis()->SetTitleOffset(1.3);
            
            hXf[u][6]->Draw();
        }
        
        Cfit[u]->cd(2);
        gPad->SetFillColor(0);
        gPad->SetBorderSize(2);
        if ( hXf[u][1]->GetEntries() > 1 ) {
            //gPad->SetLogy();
            hXf[u][1]->SetStats(0);
            hXf[u][1]->SetTitle("");
            //hXf[u][1]->SetLineColor(4);
            //hXf[u][1]->SetMarkerColor(4);
            //hXf[u][1]->SetMarkerStyle(21);
            hXf[u][1]->GetXaxis()->SetNdivisions(505);
            hXf[u][1]->GetXaxis()->SetTitle("Fitted cluster amplitude, a.d.u.");
            hXf[u][1]->GetXaxis()->SetLabelSize(0.05);
            hXf[u][1]->GetXaxis()->SetTitleSize(0.05);
            hXf[u][1]->GetYaxis()->SetNdivisions(505);
            hXf[u][1]->GetYaxis()->SetTitle("Number of clusters");
            hXf[u][1]->GetYaxis()->SetLabelSize(0.05);
            hXf[u][1]->GetYaxis()->SetTitleSize(0.05);
            hXf[u][1]->GetYaxis()->SetTitleOffset(1.3);
            
            hXf[u][1]->Draw();
            
            //fit *************************************************************************
            PeakRange( hXf[u][1], &maxpos, &Rmin, &Rmax);
            
            double iGain = 1.;
            if ( maxpos > 0.000001) iGain = Co::NeKa/maxpos;
            double par2 = ANoise[u]*iGain;  //for STA --> 78.; e2v --> 20.
            cout << " maxpos=" << maxpos << " Rmin=" << Rmin << " Rmax" << Rmax << endl;
            cout << " gain estimate =" << iGain << " e-/adu" << endl;
            cout << " noise =" << par2 << " e-" << endl;
            double pKb = maxpos*Co::Eb/Co::Ea;
            if ( pKb > Rmax ) {
                Rmax = pKb + (Rmax - maxpos);
                cout <<  " new Rmax= " << Rmax << endl;
            }
            //fprintf (pFile,"//Peak:  maxpos= %f  Rmin=%f  Rmax=%f \n", maxpos, Rmin, Rmax);
            //fprintf (pFile,"//Estimates:  gain= %f  noise=%f   \n", iGain, par2);
            
            const char * fitname = "fitGG";
            TF1 * fit = new TF1(fitname, Fe55::GGF, Rmin, Rmax, Npar);
            
            //fprintf (pFile,"// Channel %d Noise rms = %f, %f e- \n", u, ANoise[u], par2);
            //fprintf (pFile,"//flag   N Ka     N Kb    Ka,adu   er(Ka) noise,e   Ca    Ra/b \n");
            
            
            double maxval = hXf[u][1]->GetEntries(); //ham[u][i]->GetBinContent(maxbin);
            if (maxval < 1.) continue;
            double sw = hXf[u][1]->GetSumOfWeights();
            double bw = hXf[u][1]->GetBinWidth(1);
            printf("\n Fit of %s \n",hXf[u][0]->GetTitle());
            printf(" hXf%i[0]: Sum of weights = %f  Entries = %f  BinW = %f\n",
                   u, sw, maxval, bw);
            double par0 = maxval*0.75*bw;  //bw because of the root "feature"
            double par3 = maxval*0.25*bw;
            fit->SetParameter(0, par0);
            fit->SetParameter(1, maxpos);
            fit->SetParameter(2, par2);
            fit->SetParameter(3, par3);
            cout << " par0=" << par0 <<  endl;
            cout << " par1=" << maxpos << " par2=" << par2 << endl;
            cout << " par3=" << par3 <<  endl;
            cout << "fitname=" << fitname << endl;
            
            fitflag = hXf[u][1]->Fit(fitname,"LR");   //IB
            cout << " FitFlag=" << fitflag << endl;
//            NKa   = fit->GetParameter(0)/bw;
//            EKa   = fit->GetParameter(1);
//            Noise = fit->GetParameter(2);
//            NKb   = fit->GetParameter(3)/bw;
//            era   = fit->GetParError(1);
//            ca = EKa > 0.1 ? (Co::Ea/Co::w_pair)/EKa : 0.;
//            rab = NKa > 1  ? (double)NKb/NKa : 0.;
            
        }
        
        Cfit[u]->cd(3);
        gPad->SetFillColor(0);
        gPad->SetBorderSize(2);
        if ( hXf[u][2]->GetEntries() > 1 ) {
            //gPad->SetLogy();
            hXf[u][2]->SetStats(0);
            hXf[u][2]->SetTitle("");
            //hXf[u][2]->SetLineColor(4);
            //hXf[u][2]->SetMarkerColor(4);
            //hXf[u][2]->SetMarkerStyle(21);
            hXf[u][2]->GetXaxis()->SetTitle("X coordinate, sub pixel");
            hXf[u][2]->GetXaxis()->SetLabelSize(0.05);
            hXf[u][2]->GetXaxis()->SetTitleSize(0.05);
            hXf[u][2]->GetYaxis()->SetNdivisions(505);
            hXf[u][2]->GetYaxis()->SetTitle("Number of clusters");
            hXf[u][2]->GetYaxis()->SetLabelSize(0.05);
            hXf[u][2]->GetYaxis()->SetTitleSize(0.05);
            hXf[u][2]->GetYaxis()->SetTitleOffset(1.3);
            
            hXf[u][2]->Draw();
        }
        
        Cfit[u]->cd(4);
        gPad->SetFillColor(0);
        gPad->SetBorderSize(2);
        if ( hXf[u][3]->GetEntries() > 1 ) {
            //gPad->SetLogy();
            hXf[u][3]->SetStats(0);
            hXf[u][3]->SetTitle("");
            //hXf[u][3]->SetLineColor(4);
            //hXf[u][3]->SetMarkerColor(4);
            //hXf[u][3]->SetMarkerStyle(21);
            hXf[u][3]->GetXaxis()->SetTitle("Y coordinate, sub pixel");
            hXf[u][3]->GetXaxis()->SetLabelSize(0.05);
            hXf[u][3]->GetXaxis()->SetTitleSize(0.05);
            hXf[u][3]->GetYaxis()->SetNdivisions(505);
            hXf[u][3]->GetYaxis()->SetTitle("Number of clusters");
            hXf[u][3]->GetYaxis()->SetLabelSize(0.05);
            hXf[u][3]->GetYaxis()->SetTitleSize(0.05);
            hXf[u][3]->GetYaxis()->SetTitleOffset(1.3);
            
            hXf[u][3]->Draw();
        }
        
        Cfit[u]->cd(5);
        gPad->SetFillColor(0);
        gPad->SetBorderSize(2);
        if ( hXf[u][4]->GetEntries() > 1 ) {
            //gPad->SetLogy(); 
            hXf[u][4]->SetStats(0);
            hXf[u][4]->SetTitle("");
            //hXf[u][4]->SetLineColor(4); 
            //hXf[u][4]->SetMarkerColor(4);
            //hXf[u][4]->SetMarkerStyle(21);
            hXf[u][4]->GetXaxis()->SetTitle("#sigma, #mum");
            hXf[u][4]->GetXaxis()->SetLabelSize(0.05);
            hXf[u][4]->GetXaxis()->SetTitleSize(0.05);
            hXf[u][4]->GetYaxis()->SetNdivisions(505);
            hXf[u][4]->GetYaxis()->SetTitle("Number of clusters");
            hXf[u][4]->GetYaxis()->SetLabelSize(0.05);
            hXf[u][4]->GetYaxis()->SetTitleSize(0.05);
            hXf[u][4]->GetYaxis()->SetTitleOffset(1.3);
            
            hXf[u][4]->Draw();
        }
        
        Cfit[u]->cd(6);
        if ( hXf[u][5]->GetEntries() > 1 ) {
            //gPad->SetLogy(); 
            hXf[u][5]->SetStats(0);
            hXf[u][5]->SetTitle("");
            //hXf[u][5]->SetLineColor(4); 
            //hXf[u][5]->SetMarkerColor(4);
            //hXf[u][5]->SetMarkerStyle(21);
            hXf[u][5]->GetXaxis()->SetTitle("#sigma error, #mum");
            hXf[u][5]->GetXaxis()->SetLabelSize(0.05);
            hXf[u][5]->GetXaxis()->SetTitleSize(0.05);
            hXf[u][5]->GetYaxis()->SetNdivisions(505);
            hXf[u][5]->GetYaxis()->SetTitle("Number of clusters");
            hXf[u][5]->GetYaxis()->SetLabelSize(0.05);
            hXf[u][5]->GetYaxis()->SetTitleSize(0.05);
            hXf[u][5]->GetYaxis()->SetTitleOffset(1.3);
            hXf[u][5]->Draw();
        }
        
        Cfit[u]->Update();
        //Cfit->Print("Fe55_Xfit.pdf");
	}
    
	return;
} //end PlotXfit function

void Fe55::PlotCTE()
{
     rPad __f;

// XCTE canvas ************************************************* 
	 TCanvas	*	CteXc = new TCanvas( "CteXc", "CteXc", 150, 150, 800, 800);
	 CteXc->SetBorderMode  (0);      //to remove border
	 CteXc->SetLeftMargin  (left_margin);   //to move plot to the right 0.14
	 CteXc->SetRightMargin (right_margin);   //to move plot to the right 0.05
	 CteXc->SetTopMargin   (top_margin);   //to use more space 0.07
	 CteXc->SetBottomMargin(bot_margin);   //default
	 CteXc->SetFrameFillColor(0);
	 CteXc->Divide(4,4);

	 gStyle->SetPalette(1);
	 CteXc->SetFillStyle(4100);
	 CteXc->SetFillColor(0);

for (int u=0; u<Nchan; u++){
	if ( u > MaxP )   break;
	 CteXc->cd(u+1);
	 gPad->SetGrid(1,0);
	 gPad->SetGridy();
	 gPad->SetFillColor(0);
	 gPad->SetBorderSize(2);
	 Xcte[u]->Draw("colz");
}
	 CteXc->Update();
	 CteXc->Print("Fe55_CteXc.pdf");

// YCTE canvas ************************************************* 
	 TCanvas	*	CteYc = new TCanvas( "CteYc", "CteYc", 200, 200, 900, 900);
	 CteYc->SetBorderMode  (0);      //to remove border
	 CteYc->SetLeftMargin  (left_margin);   //to move plot to the right 0.14
	 CteYc->SetRightMargin (right_margin);   //to move plot to the right 0.05
	 CteYc->SetTopMargin   (top_margin);   //to use more space 0.07
	 CteYc->SetBottomMargin(bot_margin);   //default
	 CteYc->SetFrameFillColor(0);
	 CteYc->Divide(4,4);

	 gStyle->SetPalette(1);
	 CteYc->SetFillStyle(4100);
	 CteYc->SetFillColor(0);

for (int u=0; u<Nchan; u++){
	if ( u > MaxP )   break;
	 CteYc->cd(u+1);
	 gPad->SetGrid(1,0);
	 gPad->SetGridy();
	 gPad->SetFillColor(0);
	 gPad->SetBorderSize(2);
	 Ycte[u]->Draw("colz");
}
	 CteYc->Modified();
	 CteYc->Update();
	 CteYc->Print("Fe55_CteYc.pdf");

// Clusters coordinates canvas ************************************************* 
	 TCanvas	*	CQ = new TCanvas( "CQ", "CQ", 210, 210, 900, 900);
	 CQ->SetBorderMode  (0);      //to remove border
	 CQ->SetLeftMargin  (left_margin);   //to move plot to the right 0.14
	 CQ->SetRightMargin (right_margin);   //to move plot to the right 0.05
	 CQ->SetTopMargin   (top_margin);   //to use more space 0.07
	 CQ->SetBottomMargin(bot_margin);   //default
	 CQ->SetFrameFillColor(0);
	 CQ->Divide(4,4);

	 gStyle->SetPalette(1);
	 CQ->SetFillStyle(4100);
	 CQ->SetFillColor(0);

for (int u=0; u<Nchan; u++){
	if ( u > MaxP )   break;
	 CQ->cd(u+1);
	 gPad->SetGrid(1,0);
	 gPad->SetGridy();
	 gPad->SetFillColor(0);
	 gPad->SetBorderSize(2);
	 ClustQ[u]->Draw("colz");
}
	 CQ->Modified();
	 CQ->Update();
	 CQ->Print("Fe55_Q.pdf");

} //end PlotCTE function

void Fe55::PlotProfiles()
{
    rPad __f;
    printf("In PlotProfiles\n");
    TCanvas	*	Prof[MaxP]={0};
    //TCanvas	*	Xbin[MaxP]={0};
    //TCanvas	*	Ybin[MaxP]={0};
    
    for (int u=0; u<Nchan; u++){
        char tiname[50];
        sprintf(tiname, "Profile_%i", u);
        if ( u > MaxP )   break;
        
        // cluster canvas *************************************************
        Prof[u] = new TCanvas( tiname, tiname, 150+5*u, 150+5*u, 1000, 800);
        Prof[u]->SetBorderMode  (0);            //to remove border
        Prof[u]->SetLeftMargin  (left_margin);  //to move plot to the right 0.14
        Prof[u]->SetRightMargin (right_margin); //to move plot to the right 0.05
        Prof[u]->SetTopMargin   (top_margin);   //to use more space 0.07
        Prof[u]->SetBottomMargin(bot_margin);   //default
        Prof[u]->SetFrameFillColor(0);
        Prof[u]->Divide(Co::NXpix,Co::NYpix);
        
        Prof[u]->SetFillStyle(4100);
        Prof[u]->SetFillColor(0);
        
        for (int x=0; x<Co::NXpix;x++){
            for (int y=0; y<Co::NYpix; y++){
                Prof[u]->cd(y*Co::NXpix+x+1);
                hpixA[u][x][y]->Draw();
            }
        }

        Prof[u]->Update();
        sprintf(tiname,"Profile_%i.pdf", u);
        Prof[u]->Print(tiname);

    }

} //

void Fe55::PlotHits()
{
    rPad __f;
    printf("In PlotHits\n");
    // cluster canvas *************************************************
    TCanvas * cHits = new TCanvas("Hits", "Hits", 400,20,800,750);
    cHits->SetBorderMode  (0);            //to remove border
    cHits->SetLeftMargin  (left_margin);  //to move plot to the right 0.14
    cHits->SetRightMargin (right_margin); //to move plot to the right 0.05
    cHits->SetTopMargin   (top_margin);   //to use more space 0.07
    cHits->SetBottomMargin(bot_margin);   //default
    cHits->SetFrameFillColor(0);
    cHits->Divide(1,Nchan);
    
    cHits->SetFillStyle(4100);
    cHits->SetFillColor(0);
        
    for (int u=0; u<Nchan; u++){
        if ( u > MaxP )   break;
        cHits->cd(u+1);
        if ( hitSumA[u]->GetEntries() > 1 ) {
            hitSumA[u]->Draw();
        }
    }
    
    cHits->Update();
    cHits->Print("Hits.pdf");
    
} // end PlotHits

int Fe55_main ( const char * dir,
		const char * outName )  //= "Fe_50V" )
{
// clean up previous histograms
	gDirectory->GetList()->Delete();

// ********************************************************************
	for (int i=0; KeyList[i]; i++) {printf(" %i.%s \n", i, KeyList[i]);}
	for (int i=0; i<Nkey; i++) Vval[i].erase( Vval[i].begin(), Vval[i].end() );

//input directory +++++++++++++++++++++++++++++++++++++++++++++++++++++
	string dir_name = dir; // = "C:/DATA/Fe55/20080122-161251/";
	//dir_name += outName;

// Bias files analysis ************************************************

	string  bias_dir = dir_name;
	bias_dir += "/bias";
	
	int doFFt=0;
//	Bias B( bias_dir, outName, doFFt );
//	if (!B.Flag) {
//		B.Plot();
//		if (doFFt) { B.Plot_FFT(); }
//		//B.PrintVal();
//	}
	//else {if (B.Flag > -10) return -1;} //flag=-10 if there is no bias data

// ********************************************************************
// X-ray simulations   ************************************************

//  SimX SimFe55( bias_dir );
//  SimFe55.PlotSim();
//  return 0;
// ********************************************************************
// Fe55 files analysis ************************************************
	
	//dir_name += "/Sim_375V2";

	doFFt=0;
//	Fe55 Fe( dir_name, outName, B.bias_rms, B.trbuf, doFFt  );  //for BaLiC
	Fe55 Fe( dir_name, outName, 0, 0, doFFt  );   // for BaLiS

	if (Fe.Flag != 0) return -1;
	Fe.PlotBase();   //if Base Line Subtraction is used
	Fe.PlotSpectra();
	Fe.PlotXlines();
    //Fe.PlotAvHit();
    Fe.PlotProfiles();
    if ( Co::doCTE ) {Fe.PlotCTE();}
    if ( Co::doFit ) {Fe.PlotXfit();}
	if (doFFt) { Fe.Plot_FFT(); }
    Fe.PlotHits();


	
	unsigned int VNpnt = Vval[0].size();
	unsigned int VVpnt;
	if ( !VNpnt ) return 0;
// Vector canvas 1 ****************************************************
	 const float left_margin = (float)0.01;
	 const float right_margin = (float)0.001;
	 const float top_margin = (float)0.00;
	 const float bot_margin = (float)0.00;

	 rPad __f;
	 TCanvas	*	CT = new TCanvas( "CT", "Bias", 100, 70, 900, 800);
	 CT->SetBorderMode  (0);      //to remove border
	 CT->SetLeftMargin  (left_margin);   //to move plot to the right 0.14
	 CT->SetRightMargin (right_margin);  //to move plot to the right 0.05
	 CT->SetTopMargin   (top_margin);    //to use more space 0.07
	 CT->SetBottomMargin(bot_margin);    //default
	 CT->SetFrameFillColor(0);
	 CT->Divide(1,3);
	 //TColor * c150 = new TColor(150,1.0,0.0,0.0); 

	 gPad->SetFillStyle(4100);
	 gPad->SetFillColor(0);

	 CT->cd(1);
	 do {
	 VVpnt = Vval[2].size();
	 printf(" Vector's Vpoints =%i *** \n", VVpnt);
	 if ( !VVpnt ) continue;
	 TGraph * pGe = new TGraph( VNpnt, &Vval[0][0], &Vval[2][0]);
	 pGe->SetTitle("exposure"); 
	 pGe->GetXaxis()->SetTimeDisplay(1);
	 pGe->GetXaxis()->SetLabelOffset((float)0.02);
	 pGe->GetXaxis()->SetTimeFormat("#splitline{%Y-%m-%d}{%H:%M}"); 
	 pGe->SetMarkerColor (4);
	 pGe->SetMarkerStyle (8);
	 pGe->SetMarkerSize  (1);
	 pGe->SetLineColor   (4);
	 pGe->SetLineWidth   (2);
	 pGe->Draw("ALP");
	 } while(0);

	 CT->cd(2);
	 do {
	 VVpnt = Vval[3].size();
	 printf(" Vector's Vpoints =%i *** \n", VVpnt);
	 if ( !VVpnt ) continue;
	 TGraph * pGv = new TGraph( VNpnt, &Vval[0][0], &Vval[3][0]);
	 pGv->SetTitle("Bias"); 
	 pGv->GetXaxis()->SetTimeDisplay(1);
	 pGv->GetXaxis()->SetLabelOffset((float)0.02);
	 pGv->GetXaxis()->SetTimeFormat("#splitline{%Y-%m-%d}{%H:%M}"); 
	 pGv->SetMarkerColor (4);
	 pGv->SetMarkerStyle (8);
	 pGv->SetMarkerSize  (1);
	 pGv->SetLineColor   (4);
	 pGv->SetLineWidth   (2);
	 pGv->Draw("ALP");
	 } while(0);

	 CT->cd(3);
	 do {
	 VVpnt = Vval[4].size();
	 printf(" Vector's Vpoints =%i *** \n", VVpnt);
	 if ( !VVpnt ) continue;
	 TGraph * pGa = new TGraph( VNpnt, &Vval[0][0], &Vval[4][0]);
	 pGa->SetTitle("Bias current, pA"); 
	 pGa->GetXaxis()->SetTimeDisplay(1);
	 pGa->GetXaxis()->SetLabelOffset((float)0.02);
	 pGa->GetXaxis()->SetTimeFormat("#splitline{%Y-%m-%d}{%H:%M}"); 
	 pGa->SetMarkerColor (6);
	 pGa->SetMarkerStyle (8);
	 pGa->SetMarkerSize  (1);
	 pGa->SetLineColor   (6);
	 pGa->SetLineWidth   (2);
	 pGa->Draw("ALP");
	 } while(0);

 	 CT->Modified();
	 CT->Update();
	 CT->Print("CT_main.pdf");

// Vector canvas 2 ***************************************************
	 TCanvas	*	C2 = new TCanvas( "C2", "Temperature", 150, 100, 900, 800);
	 C2->SetBorderMode  (0);      //to remove border
	 C2->SetLeftMargin  (left_margin);   //to move plot to the right 0.14
	 C2->SetRightMargin (right_margin);  //to move plot to the right 0.05
	 C2->SetTopMargin   (top_margin);    //to use more space 0.07
	 C2->SetBottomMargin(bot_margin);    //default
	 C2->SetFrameFillColor(0);
	 C2->Divide(1,4);

	 gPad->SetFillStyle(4100);
	 gPad->SetFillColor(0);

	 C2->cd(1);
	 do {
	 VVpnt = Vval[5].size();
	 printf(" Vector's Vpoints =%i *** \n", VVpnt);
	 if ( !VVpnt ) continue;
	 TGraph * pGTA = new TGraph( VNpnt, &Vval[0][0], &Vval[5][0]);
	 pGTA->SetTitle("A"); 
	 pGTA->GetXaxis()->SetTimeDisplay(1);
	 pGTA->GetXaxis()->SetLabelOffset((float)0.02);
	 pGTA->GetXaxis()->SetTimeFormat("#splitline{%Y-%m-%d}{%H:%M}"); 
	 pGTA->SetMarkerColor (4);
	 pGTA->SetMarkerStyle (8);
	 pGTA->SetMarkerSize  (1);
	 pGTA->SetLineColor   (4);
	 pGTA->SetLineWidth   (2);
	 pGTA->Draw("ALP");
	 } while(0);

	 C2->cd(2);
	 do {
	 VVpnt = Vval[6].size();
	 printf(" Vector's Vpoints =%i *** \n", VVpnt);
	 if ( !VVpnt ) continue;
	 TGraph * pGTB = new TGraph( VNpnt, &Vval[0][0], &Vval[6][0]);
	 pGTB->SetTitle("B"); 
	 pGTB->GetXaxis()->SetTimeDisplay(1);
	 pGTB->GetXaxis()->SetLabelOffset((float)0.02);
	 pGTB->GetXaxis()->SetTimeFormat("#splitline{%Y-%m-%d}{%H:%M}"); 
	 pGTB->SetMarkerColor (8);
	 pGTB->SetMarkerStyle (8);
	 pGTB->SetMarkerSize  (1);
	 pGTB->SetLineColor   (8);
	 pGTB->SetLineWidth   (2);
	 pGTB->Draw("ALP");
	 } while(0);

	 C2->cd(3);
	 do {
	 VVpnt = Vval[7].size();
	 printf(" Vector's Vpoints =%i *** \n", VVpnt);
	 if ( !VVpnt ) continue;
	 TGraph * pGTC = new TGraph( VNpnt, &Vval[0][0], &Vval[7][0]);
	 pGTC->SetTitle("C"); 
	 pGTC->GetXaxis()->SetTimeDisplay(1);
	 pGTC->GetXaxis()->SetLabelOffset((float)0.02);
	 pGTC->GetXaxis()->SetTimeFormat("#splitline{%Y-%m-%d}{%H:%M}"); 
	 pGTC->SetMarkerColor (4);
	 pGTC->SetMarkerStyle (8);
	 pGTC->SetMarkerSize  (1);
	 pGTC->SetLineColor   (4);
	 pGTC->SetLineWidth   (2);
	 pGTC->Draw("ALP");
	 } while(0);

	 C2->cd(4);
	 do {
	 VVpnt = Vval[8].size();
	 printf(" Vector's Vpoints =%i *** \n", VVpnt);
	 if ( !VVpnt ) continue;
	 TGraph * pGTD = new TGraph( VNpnt, &Vval[0][0], &Vval[8][0]);
	 pGTD->SetTitle("D"); 
	 pGTD->GetXaxis()->SetTimeDisplay(1);
	 pGTD->GetXaxis()->SetLabelOffset((float)0.02);
	 pGTD->GetXaxis()->SetTimeFormat("#splitline{%Y-%m-%d}{%H:%M}"); 
	 pGTD->SetMarkerColor (14);
	 pGTD->SetMarkerStyle (8);
	 pGTD->SetMarkerSize  (1);
	 pGTD->SetLineColor   (14);
	 pGTD->SetLineWidth   (2);
	 pGTD->Draw("ALP");
	 } while(0);

	 C2->Modified();
	 C2->Update();
	 C2->Print("C2_main.pdf");
	
	return 0;
}


#endif //__CINT__

