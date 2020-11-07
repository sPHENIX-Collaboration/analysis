void compare() {

// From ppg30

//Differential Invariant Cross Section
//Rapididty coverage measured: |Dy| = 0.7    
//The data have been corrected to |Dy|=1 making a 
//flat extrapolation from 0.7 to 1.
//-----------------------------------------------------------------
//E d3sigma/ dp3 = 1/2pi 1/pt d2sigma/dpt dy      UNITS [mb/GeV^2]
//-----------------------------------------------------------------
// If you need to convert this to yield-per-event you must divide by sigma_inel(sqrt_s = 200GeV) = 42 mb

float x[18],y[18],ex[18],ey[18];
float x1[18],y1[18],ey1[18];
float x2[18],y2[18],ey2[18];

//piplus
x1[0]=0.35, y1[0]=25.6287, ex[0]=0, ey1[0]=0.246673;
x1[1]=0.45, y1[1]=14.6531, ex[1]=0, ey1[1]=0.14773;
x1[2]=0.55, y1[2]=8.62473, ex[2]=0, ey1[2]=0.0943008;
x1[3]=0.65, y1[3]=4.80858, ex[3]=0, ey1[3]=0.0579757;
x1[4]=0.75, y1[4]=2.85599, ex[4]=0, ey1[4]=0.03862;
x1[5]=0.85, y1[5]=1.71022, ex[5]=0, ey1[5]=0.0258761;
x1[6]=0.95, y1[6]=1.08056, ex[6]=0, ey1[6]=0.0183583;
x1[7]=1.05, y1[7]=0.68761, ex[7]=0, ey1[7]=0.013222;
x1[8]=1.15, y1[8]=0.431937, ex[8]=0, ey1[8]=0.00938769;
x1[9]=1.25, y1[9]=0.308209, ex[9]=0, ey1[9]=0.00732418;
x1[10]=1.35, y1[10]=0.221919, ex[10]=0, ey1[10]=0.00582217;
x1[11]=1.45, y1[11]=0.147961, ex[11]=0, ey1[11]=0.00437927;
x1[12]=1.6, y1[12]=0.0825923, ex[12]=0, ey1[12]=0.00205954;
x1[13]=1.8, y1[13]=0.0410802, ex[13]=0, ey1[13]=0.00131989;
x1[14]=2, y1[14]=0.0211058, ex[14]=0, ey1[14]=0.000876262;
x1[15]=2.2, y1[15]=0.0116705, ex[15]=0, ey1[15]=0.000638873;
x1[16]=2.4, y1[16]=0.00787318, ex[16]=0, ey1[16]=0.000557839;
x1[17]=2.6, y1[17]=0.00447392, ex[17]=0, ey1[17]=0.000458999;

// piminus
x2[0]=0.35, y2[0]=25.8453, ex[0]=0, ey2[0]=0.21199;
x2[1]=0.45, y2[1]=14.5864, ex[1]=0, ey2[1]=0.127527;
x2[2]=0.55, y2[2]=8.27867, ex[2]=0, ey2[2]=0.0789266;
x2[3]=0.65, y2[3]=4.72545, ex[3]=0, ey2[3]=0.0498388;
x2[4]=0.75, y2[4]=2.756, ex[4]=0, ey2[4]=0.032014;
x2[5]=0.85, y2[5]=1.66385, ex[5]=0, ey2[5]=0.0215601;
x2[6]=0.95, y2[6]=1.05331, ex[6]=0, ey2[6]=0.0152551;
x2[7]=1.05, y2[7]=0.648492, ex[7]=0, ey2[7]=0.0105207;
x2[8]=1.15, y2[8]=0.431804, ex[8]=0, ey2[8]=0.0079342;
x2[9]=1.25, y2[9]=0.286134, ex[9]=0, ey2[9]=0.00585618;
x2[10]=1.35, y2[10]=0.199094, ex[10]=0, ey2[10]=0.004624;
x2[11]=1.45, y2[11]=0.133006, ex[11]=0, ey2[11]=0.003509;
x2[12]=1.6, y2[12]=0.0795068, ex[12]=0, ey2[12]=0.001787;
x2[13]=1.8, y2[13]=0.0422071, ex[13]=0, ey2[13]=0.001195;
x2[14]=2, y2[14]=0.0208344, ex[14]=0, ey2[14]=0.000781061;
x2[15]=2.2, y2[15]=0.0131348, ex[15]=0, ey2[15]=0.000625054;
x2[16]=2.4, y2[16]=0.0070146, ex[16]=0, ey2[16]=0.000479789;
x2[17]=2.6, y2[17]=0.00425537, ex[17]=0, ey2[17]=0.000408;

// Kplus
float x3[13],y3[13],ey3[13];
x3[0]=0.45, y3[0]=1.95293, ex[0]=0, ey3[0]=0.0724524;
x3[1]=0.55, y3[1]=1.40404, ex[1]=0, ey3[1]=0.0466304;
x3[2]=0.65, y3[2]=0.925078, ex[2]=0, ey3[2]=0.0296139;
x3[3]=0.75, y3[3]=0.624339, ex[3]=0, ey3[3]=0.020472;
x3[4]=0.85, y3[4]=0.403909, ex[4]=0, ey3[4]=0.0142644;
x3[5]=0.95, y3[5]=0.281567, ex[5]=0, ey3[5]=0.0107128;
x3[6]=1.05, y3[6]=0.199669, ex[6]=0, ey3[6]=0.00826777;
x3[7]=1.15, y3[7]=0.137853, ex[7]=0, ey3[7]=0.00620796;
x3[8]=1.25, y3[8]=0.101317, ex[8]=0, ey3[8]=0.00483626;
x3[9]=1.35, y3[9]=0.0694105, ex[9]=0, ey3[9]=0.00362463;
x3[10]=1.45, y3[10]=0.0526523, ex[10]=0, ey3[10]=0.00301769;
x3[11]=1.6, y3[11]=0.0324873, ex[11]=0, ey3[11]=0.00151623;
x3[12]=1.8, y3[12]=0.0179392, ex[12]=0, ey3[12]=0.00100964;

// Kminus
float x4[13],y4[13],ey4[13];
x4[0]=0.45, y4[0]=1.66952, ex[0]=0, ey4[0]=0.0568774;
x4[1]=0.55, y4[1]=1.25238, ex[1]=0, ey4[1]=0.0374863;
x4[2]=0.65, y4[2]=0.828785, ex[2]=0, ey4[2]=0.0243554;
x4[3]=0.75, y4[3]=0.604526, ex[3]=0, ey4[3]=0.0177572;
x4[4]=0.85, y4[4]=0.406696, ex[4]=0, ey4[4]=0.0125949;
x4[5]=0.95, y4[5]=0.261138, ex[5]=0, ey4[5]=0.00878815;
x4[6]=1.05, y4[6]=0.189842, ex[6]=0, ey4[6]=0.00685834;
x4[7]=1.15, y4[7]=0.118611, ex[7]=0, ey4[7]=0.00483604;
x4[8]=1.25, y4[8]=0.0977945, ex[8]=0, ey4[8]=0.00407271;
x4[9]=1.35, y4[9]=0.0673309, ex[9]=0, ey4[9]=0.00315524;
x4[10]=1.45, y4[10]=0.0439384, ex[10]=0, ey4[10]=0.00239946;
x4[11]=1.6, y4[11]=0.0283855, ex[11]=0, ey4[11]=0.00125486;
x4[12]=1.8, y4[12]=0.0181311, ex[12]=0, ey4[12]=0.000911162;

// protons
float x5[15],y5[15],ey5[15];
x5[0]=0.65, y5[0]=0.467596, ex[0]=0, ey5[0]=0.0124957;
x5[1]=0.75, y5[1]=0.361103, ex[1]=0, ey5[1]=0.0100168;
x5[2]=0.85, y5[2]=0.250201, ex[2]=0, ey5[2]=0.00749516;
x5[3]=0.95, y5[3]=0.181417, ex[3]=0, ey5[3]=0.00593336;
x5[4]=1.05, y5[4]=0.126166, ex[4]=0, ey5[4]=0.00445335;
x5[5]=1.15, y5[5]=0.0902262, ex[5]=0, ey5[5]=0.00351448;
x5[6]=1.25, y5[6]=0.0651584, ex[6]=0, ey5[6]=0.00275751;
x5[7]=1.35, y5[7]=0.0448966, ex[7]=0, ey5[7]=0.00214243;
x5[8]=1.45, y5[8]=0.0328418, ex[8]=0, ey5[8]=0.00172412;
x5[9]=1.6, y5[9]=0.0203516, ex[9]=0, ey5[9]=0.000881656;
x5[10]=1.8, y5[10]=0.0111527, ex[10]=0, ey5[10]=0.000593141;
x5[11]=2, y5[11]=0.00477154, ex[11]=0, ey5[11]=0.000353859;
x5[12]=2.2, y5[12]=0.00309521, ex[12]=0, ey5[12]=0.000265977;
x5[13]=2.4, y5[13]=0.0018804, ex[13]=0, ey5[13]=0.0001962;
x5[14]=2.6, y5[14]=0.00122164, ex[14]=0, ey5[14]=0.000150502;

// anti-protons
float x6[15],y6[15],ey6[15];
x6[0]=0.65, y6[0]=0.306682, ex[0]=0, ey6[0]=0.00840118;
x6[1]=0.75, y6[1]=0.241129, ex[1]=0, ey6[1]=0.00669015;
x6[2]=0.85, y6[2]=0.175801, ex[2]=0, ey6[2]=0.00512706;
x6[3]=0.95, y6[3]=0.124961, ex[3]=0, ey6[3]=0.00389379;
x6[4]=1.05, y6[4]=0.0902856, ex[4]=0, ey6[4]=0.00301377;
x6[5]=1.15, y6[5]=0.0675941, ex[5]=0, ey6[5]=0.00250251;
x6[6]=1.25, y6[6]=0.0451574, ex[6]=0, ey6[6]=0.00186547;
x6[7]=1.35, y6[7]=0.0329953, ex[7]=0, ey6[7]=0.00152812;
x6[8]=1.45, y6[8]=0.0219945, ex[8]=0, ey6[8]=0.00118843;
x6[9]=1.6, y6[9]=0.0151914, ex[9]=0, ey6[9]=0.000644838;
x6[10]=1.8, y6[10]=0.00740469, ex[10]=0, ey6[10]=0.000417221;
x6[11]=2, y6[11]=0.0037345, ex[11]=0, ey6[11]=0.000274817;
x6[12]=2.2, y6[12]=0.00251093, ex[12]=0, ey6[12]=0.000213118;
x6[13]=2.4, y6[13]=0.00116624, ex[13]=0, ey6[13]=0.000138554;
x6[14]=2.6, y6[14]=0.000707305, ex[14]=0, ey6[14]=0.000103754;






// calculate average and divide by cross-section, by 2pi and by 2 units of rapidity, and by pT

float yk[99],eyk[99];
for(int i=0; i<18; i++) { y[i]=(y1[i]+y2[i])/2./40.*2.*3.141592654*2.*x1[i]; ey[i]=sqrt( (ey1[i]*ey1[i])+(ey2[1]*ey2[i]) )/2./40.*2.*3.141592654*2.*x1[i]; }
for(int i=0; i<13; i++) { yk[i]=(y3[i]+y4[i])/2./40.*2.*3.141592654*2.*x3[i]; eyk[i]=sqrt( (ey3[i]*ey3[i])+(ey4[1]*ey4[i]) )/2./40.*2.*3.141592654*2.*x3[i]; }
for(int i=0; i<15; i++) { y5[i]=y5[i]/2./40.*2.*3.141592654*2.*x5[i]; ey5[i]=ey5[i]/2./40.*2.*3.141592654*2.*x5[i]; }
for(int i=0; i<15; i++) { y6[i]=y6[i]/2./40.*2.*3.141592654*2.*x6[i]; ey6[i]=ey6[i]/2./40.*2.*3.141592654*2.*x6[i]; }


gStyle->SetOptStat(0);
TCanvas* c1 = new TCanvas("c1","",20,20,600,600);
c1->SetLogy();

TH1F* hh = new TH1F("hh"," ",20,0.,5.);
hh->GetXaxis()->SetTitle("p_{T} (GeV/c)");
hh->SetYTitle("dN/dP_{T}");
hh->GetYaxis()->SetTitleOffset(1.0);
hh->GetXaxis()->SetTitleOffset(1.0);
hh->SetMinimum(0.00005);
hh->SetMaximum(10.);
hh->Draw();

TGraphErrors* gr1 = new TGraphErrors(18,x1,y,0,ey);
gr1->SetMarkerStyle(20);
gr1->SetMarkerColor(kBlue);
gr1->SetLineColor(kBlue);
gr1->SetMarkerSize(1);

TGraphErrors* gr2 = new TGraphErrors(13,x3,yk,0,eyk);
gr2->SetMarkerStyle(20);
gr2->SetMarkerColor(kMagenta);
gr2->SetLineColor(kMagenta);
gr2->SetMarkerSize(1);

TGraphErrors* gr3 = new TGraphErrors(15,x5,y5,0,ey5);
gr3->SetMarkerStyle(20);
gr3->SetMarkerColor(kGreen+2);
gr3->SetLineColor(kGreen+2);
gr3->SetMarkerSize(1);

TGraphErrors* gr4 = new TGraphErrors(15,x6,y6,0,ey6);
gr4->SetMarkerStyle(20);
gr4->SetMarkerColor(kBlack);
gr4->SetLineColor(kBlack);
gr4->SetMarkerSize(1);



float start = 1.;
float stop = 20.0;
float  ncoll = 955.; // number of binary collisions for 0-10% Au+Au

// Suppression in AuAu
// this is taken from suppressionAuAu.C macro
string str_supppi = "(0.157654+1.947663*pow(x,-2.581256))";
TF1 supppi = TF1("supppi",str_supppi.c_str(),start,stop);
string str_suppp = "(1.013817*exp(-(x-2.413264)*(x-2.413264)/2./1.423838/1.423838)+0.157654)";
TF1 suppp = TF1("suppp",str_suppp.c_str(),start,stop);
string str_suppk = str_supppi; // kaon suppression in AuAu same as pion
TF1 suppk = TF1("suppk",str_suppk.c_str(),start,stop);

char srejpi[999],srejk[999],srejp[999],srejap[999];

TF1 frejpi,frejk,frejp,frejantiprot;
double eideff=0.7;
string str_srejpi;
string str_srejk;
string str_srejp;
string str_srejap;

if(eideff==0.9) {
// Hadron inverse rejection factors for eid efficiency 90%

float pirejpar0 = -3.17086e-03;
float pirejpar1 = 2.02062e-01;
float pirejpar2 = -2.41365e+00;
float pirejpar3 = 5.81442e-04;
sprintf(srejpi,"(%f+%f*pow(x,%f)+%f*x)",pirejpar0,pirejpar1,pirejpar2,pirejpar3);
str_srejpi = srejpi;
frejpi = TF1("frejpi",str_srejpi.c_str(),start,stop);

float krejpar0 = -1.78362e-03;
float krejpar1 = 2.77532e-01;
float krejpar2 = -2.72674e+00;
float krejpar3 = 2.19957e-04;
sprintf(srejk,"(%f+%f*pow(x,%f)+%f*x)",krejpar0,krejpar1,krejpar2,krejpar3);
str_srejk = srejk;
frejk = TF1("frejk",str_srejk.c_str(),start,stop);

float prejpar0 = -1.24742e-04;
float prejpar1 = 1.82736e-01;
float prejpar2 = -3.66287e+00;
float prejpar3 = 2.30891e-05;
sprintf(srejp,"(%f+%f*pow(x,%f)+%f*x)",prejpar0,prejpar1,prejpar2,prejpar3);
string str_srejp = srejp;
frejp = TF1("frejp",str_srejp.c_str(),start,stop);

float aprejpar0 = 3.04653e-03;
float aprejpar1 = 7.17479e-01;
float aprejpar2 = -1.26446e+00;
sprintf(srejap,"(%f+%f*exp(x/%f))",aprejpar0,aprejpar1,aprejpar2);
str_srejap = srejap;
frejantiprot = TF1("frejantiprot",str_srejap.c_str(),start,stop);

} 
else if (eideff==0.7) {
// Hadron inverse rejection factors for eid efficiency 90%

float pirejpar0_eid70 = -4.71224e-04;
float pirejpar1_eid70 = 1.07319e-01;
float pirejpar2_eid70 = -2.72607e+00;
float pirejpar3_eid70 = 1.51839e-04;
sprintf(srejpi,"(%f+%f*pow(x,%f)+%f*x)",pirejpar0_eid70,pirejpar1_eid70,pirejpar2_eid70,pirejpar3_eid70);
str_srejpi = srejpi;
frejpi = TF1("frejpi",str_srejpi.c_str(),start,stop);

// just divide by 2 for lack of statistics
float krejpar0 = -1.78362e-03;
float krejpar1 = 2.77532e-01;
float krejpar2 = -2.72674e+00;
float krejpar3 = 2.19957e-04;
sprintf(srejk,"(%f+%f*pow(x,%f)+%f*x)/2.",krejpar0,krejpar1,krejpar2,krejpar3);
str_srejk = srejk;
frejk = TF1("frejk",str_srejk.c_str(),start,stop);

// just divide by 2 for lack of statistics
float prejpar0 = -1.24742e-04;
float prejpar1 = 1.82736e-01;
float prejpar2 = -3.66287e+00;
float prejpar3 = 2.30891e-05;
sprintf(srejp,"(%f+%f*pow(x,%f)+%f*x)/2.",prejpar0,prejpar1,prejpar2,prejpar3);
str_srejp = srejp;
frejp = TF1("frejp",str_srejp.c_str(),start,stop);

float aprejpar0_eid70 = 1.08465e-03;
float aprejpar1_eid70 = 5.22870e-01;
float aprejpar2_eid70 = -1.11407e+00;
sprintf(srejap,"(%f+%f*exp(x/%f))",aprejpar0_eid70,aprejpar1_eid70,aprejpar2_eid70);
str_srejap = srejap;
frejantiprot = TF1("frejantiprot",str_srejap.c_str(),start,stop);

} else {
  cerr << "ERROR: eid efficiency must be 0.9 or 0.7 !!!" << endl;
}


string multiply = "*";
string divide = "/";
string myplus = "+";

//================ pi-zero ============================================================


// Invariant cross-section of pi-zero in p+p from ppg063
// The units are mb * GeV^-2 * c^3
char invdistr[999];
float a0 = 3.13729e+00;
float a1 = 1.45571e+00;
float a2 = -3.02276e+00;
float a3 = -2.35733e+00;
sprintf(invdistr,"(%f*pow(x,%f)*pow((1.+(x/%f)*(x/%f)),%f))",a0,a3,a1,a1,a2);
TF1 invptdistr = TF1("invptdistr",invdistr,start,stop);

// Multiply by pT, divide by p+p total crossection (40mb), and multiply by 2pi 
// This should be dN/dpT per event per unit of rapidity
// Then multiply by 2 since we are looking at +-1 unit of rapidity

a0 = a0 * 2.*3.141592654 / 40. * 2.;

char piondistribution[999];
sprintf(piondistribution,"(%f*pow(x,%f)*pow((1.+(x/%f)*(x/%f)),%f)*x)",a0,a3,a1,a1,a2);
TF1* pionptdistr = new TF1("pionptdistr",piondistribution,start,stop); // this is pion dN/dpT in p+p
string str_pionptdistr = piondistribution;

// convert to AuAu

char piondistributionAuAu[999];
float a0auau = a0 * ncoll; // scale to central AuAu

sprintf(piondistributionAuAu,"(%f*pow(x,%f)*pow((1.+(x/%f)*(x/%f)),%f)*x)",a0auau,a3,a1,a1,a2);
string str_piondistributionAuAu = piondistributionAuAu;
str_piondistributionAuAu = str_piondistributionAuAu + multiply + str_supppi; // pion suppression in AuAu
TF1* pionptdistrAuAu_norej = new TF1("pionptdistrAuAu_norej",str_piondistributionAuAu.c_str(),start,stop);

string str_piondistributionAuAu_withrej = str_piondistributionAuAu + multiply + str_srejpi; // multiply by inverse pion rejection factor

TF1* pionptdistrAuAu = new TF1("pionptdistrAuAu",str_piondistributionAuAu_withrej.c_str(),start,stop); // this is dN/dpT for fake electrons from pions in AuAu

char piondistributionAuAu_rej90[999];
string str_piondistributionAuAu_rej90 = str_piondistributionAuAu + divide + "90.";
TF1* pionptdistrAuAu_rej90 = new TF1("pionptdistrAuAu_rej90",str_piondistributionAuAu_rej90.c_str(),start,stop);

cout << "CONSTANT REJECTION = 90:" << endl;
cout << str_piondistributionAuAu_rej90 << endl << endl;



//============ kaons ============================================

char kaondistribution[999];
float scalekaon = 0.29; 
sprintf(kaondistribution,"(%f*pow(x,%f)*pow((1.+(x/%f)*(x/%f)),%f)*x*sqrt(0.1396*0.1396+x*x)/sqrt(0.4937*0.4937+x*x)*%f)",a0,a3,a1,a1,a2,scalekaon);
string str_kaondistribution = kaondistribution;
TF1* kaonptdistr = new TF1("kaonptdistr",str_kaondistribution.c_str(),start,stop);

char kaondistributionAuAu[999];
sprintf(kaondistributionAuAu,"((%f*pow(x,%f)*pow((1.+(x/%f)*(x/%f)),%f)*x)*sqrt(0.1396*0.1396+x*x)/sqrt(0.4937*0.4937+x*x)*%f)",a0auau,a3,a1,a1,a2,scalekaon);
string str_kaondistributionAuAu = kaondistributionAuAu;
str_kaondistributionAuAu = str_kaondistributionAuAu + multiply + str_suppk; // kaon suppression in AuAu
TF1* kaonptdistrAuAu_norej = new TF1("kaonptdistrAuAu_norej",str_kaondistributionAuAu.c_str(),start,stop);

string str_kaondistributionAuAu_withrej = str_kaondistributionAuAu + multiply + str_srejk; // multiply by inverse kaon rejection factor

TF1* kaonptdistrAuAu = new TF1("kaonptdistrAuAu",str_kaondistributionAuAu_withrej.c_str(),start,stop); // this is dN/dpT for fake electrons from Kaons in AuAu


//=========== protons ============================================

char protondistribution[999];
float scalep = 0.14;
sprintf(protondistribution,"(%f*pow(x,%f)*pow((1.+(x/%f)*(x/%f)),%f)*x*sqrt(0.1396*0.1396+x*x)/sqrt(0.9383*0.9383+x*x)*%f)",a0,a3,a1,a1,a2,scalep);
string str_protondistribution = protondistribution;
TF1* protonptdistr = new TF1("protonptdistr",str_protondistribution.c_str(),start,stop);

char protondistributionAuAu[999];
sprintf(protondistributionAuAu,"((%f*pow(x,%f)*pow((1.+(x/%f)*(x/%f)),%f)*x)*sqrt(0.1396*0.1396+x*x)/sqrt(0.9383*0.9383+x*x)*%f)",a0auau,a3,a1,a1,a2,scalep);
string str_protondistributionAuAu = protondistributionAuAu;
str_protondistributionAuAu = str_protondistributionAuAu + multiply + str_suppp; // proton suppression in AuAu
TF1* protonptdistrAuAu_norej = new TF1("protonptdistrAuAu_norej",str_protondistributionAuAu.c_str(),start,stop); 

string str_protondistributionAuAu_withrej = str_protondistributionAuAu + multiply + str_srejp; // multiply by inverse proton rejection factor

TF1* protonptdistrAuAu = new TF1("protonptdistrAuAu",str_protondistributionAuAu_withrej.c_str(),start,stop); // this is dN/dpT for fake electrons from protons in AuAu


//=========== anti-protons =======================================

char aprotondistribution[999];
float scaleap = 0.09;
sprintf(aprotondistribution,"(%f*pow(x,%f)*pow((1.+(x/%f)*(x/%f)),%f)*x*sqrt(0.1396*0.1396+x*x)/sqrt(0.9383*0.9383+x*x)*%f)",a0,a3,a1,a1,a2,scaleap);
string str_aprotondistribution = aprotondistribution;
TF1* aprotonptdistr = new TF1("aprotonptdistr",str_aprotondistribution.c_str(),start,stop);

char aprotondistributionAuAu[999];
sprintf(aprotondistributionAuAu,"((%f*pow(x,%f)*pow((1.+(x/%f)*(x/%f)),%f)*x)*sqrt(0.1396*0.1396+x*x)/sqrt(0.9383*0.9383+x*x)*%f)",a0auau,a3,a1,a1,a2,scaleap);
string str_aprotondistributionAuAu = aprotondistributionAuAu;
str_aprotondistributionAuAu = str_aprotondistributionAuAu + multiply + str_suppp; // proton suppression in AuAu
TF1* aprotonptdistrAuAu_norej = new TF1("aprotonptdistrAuAu_norej",str_aprotondistributionAuAu.c_str(),start,stop); 

string str_aprotondistributionAuAu_withrej = str_aprotondistributionAuAu + multiply + str_srejap; // multiply by inverse anti-proton rejection factor

TF1* aprotonptdistrAuAu = new TF1("aprotonptdistrAuAu",str_aprotondistributionAuAu_withrej.c_str(),start,stop); // this is dN/dpT for fake electrons from anti-protons in AuAu



gr1->Draw("P");
gr2->Draw("P");
gr3->Draw("P");
gr4->Draw("P");

pionptdistr->SetLineColor(kBlue);
pionptdistr->Draw("same");
kaonptdistr->SetLineColor(kMagenta);
kaonptdistr->Draw("same");
protonptdistr->SetLineColor(kGreen+2);
protonptdistr->Draw("same");
aprotonptdistr->SetLineColor(kBlack);
aprotonptdistr->Draw("same");




TCanvas* c2 = new TCanvas("c2","",120,120,600,600);
c2->SetLogy();

TH1F* hh2 = new TH1F("hh2"," ",20,0.,5.);
hh2->GetXaxis()->SetTitle("p_{T} (GeV/c)");
hh2->SetYTitle("dN/dP_{T}");
hh2->GetYaxis()->SetTitleOffset(1.0);
hh2->GetXaxis()->SetTitleOffset(1.0);
hh2->SetMinimum(0.00005);
hh2->SetMaximum(500.);
hh2->Draw();

pionptdistrAuAu->SetLineColor(kBlue);
pionptdistrAuAu->Draw();
kaonptdistrAuAu->SetLineColor(kMagenta);
kaonptdistrAuAu->Draw("same");
protonptdistrAuAu->SetLineColor(kGreen+2);
protonptdistrAuAu->Draw("same");
aprotonptdistrAuAu->SetLineColor(kBlack);
aprotonptdistrAuAu->Draw("same");

pionptdistrAuAu_rej90->SetLineColor(kBlue);
pionptdistrAuAu_rej90->SetLineStyle(2);
//pionptdistrAuAu_rej90->Draw("same");

//
// Now add up everything
//

string str_plusdistributionAuAu_withrej = str_piondistributionAuAu_withrej + myplus + str_kaondistributionAuAu_withrej + myplus + str_protondistributionAuAu_withrej;
TF1* plusptdistrAuAu = new TF1("plusptdistrAuAu",str_plusdistributionAuAu_withrej.c_str(),start,stop); // this is dN/dpT for fake positrons from all sources in AuAu
plusptdistrAuAu->SetLineColor(kRed);
plusptdistrAuAu->Draw("same");

cout << str_plusdistributionAuAu_withrej << endl;


string str_minusdistributionAuAu_withrej = str_piondistributionAuAu_withrej + myplus + str_kaondistributionAuAu_withrej + myplus + str_aprotondistributionAuAu_withrej;
TF1* minusptdistrAuAu = new TF1("minusptdistrAuAu",str_minusdistributionAuAu_withrej.c_str(),start,stop); // this is dN/dpT for fake electrons from all sources in AuAu
minusptdistrAuAu->SetLineColor(kRed+2);
minusptdistrAuAu->Draw("same");

cout << str_minusdistributionAuAu_withrej << endl;


/*
TCanvas* c3 = new TCanvas("c3","",220,220,600,600);
c3->SetLogy();
hh3 = new TH1F("hh2"," ",20,0.,5.);
hh3->SetMinimum(0.00005);
hh3->SetMaximum(500.);
hh3->Draw();

pionptdistrAuAu_norej->SetLineColor(kBlue);
pionptdistrAuAu_norej->Draw();
kaonptdistrAuAu_norej->SetLineColor(kMagenta);
kaonptdistrAuAu_norej->Draw("same");
protonptdistrAuAu_norej->SetLineColor(kGreen+2);
protonptdistrAuAu_norej->Draw("same");
aprotonptdistrAuAu_norej->SetLineColor(kBlack);
aprotonptdistrAuAu_norej->Draw("same");
*/


}
