void plotpirej()
{
gStyle->SetOptStat(0);

const int nbins = 5;
double bins[nbins+1];
for(int i=0; i<=nbins; i++) { bins[i] = 2. + 2.*double(i); }
//cout << "bin edges:" << endl;
//for(int i=0; i<=nbins; i++) { cout <<  bins[i] << endl; }

int neopbins = 100;
char hname[999];
TH1D* heop_pi[nbins+1];
TH1D* heop_e[nbins+1];
for(int i=0; i<=nbins; i++) {
  sprintf(hname,"heop_pi_%d",i);
  heop_pi[i] = new TH1D(hname,hname,neopbins,0.,2.);
  heop_pi[i]->Sumw2();
  sprintf(hname,"heop_e_%d",i);
  heop_e[i] = new TH1D(hname,hname,neopbins,0.,2.);
  heop_e[i]->Sumw2();
}
  TH1D* hhoe_e = new TH1D("hhoe_e","hhoe_e",150,0.,1.5);
  TH1D* hhoe_pi = new TH1D("hhoe_pi","hhoe_pi",150,0.,1.5);
  TH2D* hhhoe_e = new TH2D("hhhoe_e","hhhoe_e",100,0.,2.0,100,0.,2.0);
  TH2D* hhhoe_pi = new TH2D("hhhoe_pi","hhhoe_pi",100,0.,2.0,100,0.,2.0);

  TH1D* hprob_e = new TH1D("hprob_e","hprob_e",100,0.,1.);
  TH1D* hprob_pi = new TH1D("hprob_pi","hprob_pi",100,0.,1.);
  TH1D* hchi2_e = new TH1D("hchi2_e","hchi2_e",100,0.,10.);
  TH1D* hchi2_pi = new TH1D("hchi2_pi","hchi2_pi",100,0.,10.);

TChain* ntp_track_pi = new TChain("ntppid");
  ntp_track_pi->AddFile("pions0.root");
  ntp_track_pi->AddFile("pions1.root");
  ntp_track_pi->AddFile("pions2.root");
  ntp_track_pi->AddFile("pions4.root");

//string infname_piminus;
//ifstream ifs_piminus;
//ifs_piminus.open("piminuslist.txt");
//if ( ifs_piminus.fail() ) { cout << "FAIL TO READ INPUT FILE 1" << endl; ifs_piminus.close(); return; }
//int tmpcount=0;
//while(!ifs_piminus.eof()) {
//  ifs_piminus >> infname_piminus;
//  ntp_track_pi->AddFile(infname_piminus.c_str());
//  tmpcount++;
//  //if(tmpcount>20) break;
//}
//ifs_piminus.close();

TChain* ntp_track_e = new TChain("ntppid");
  ntp_track_e->AddFile("electrons0.root");
  ntp_track_e->AddFile("electrons1.root");
  ntp_track_e->AddFile("electrons2.root");
  ntp_track_e->AddFile("electrons3.root");

//string infname_e;
//ifstream ifs_e;
//ifs_e.open("electronlist.txt");
//if ( ifs_e.fail() ) { cout << "FAIL TO READ INPUT FILE 2" << endl; ifs_e.close(); return; }
//tmpcount=0;
//while(!ifs_e.eof()) {
//  ifs_e >> infname_e;
//  ntp_track_e->AddFile(infname_e.c_str());
//  tmpcount++;
//  //if(tmpcount>20) break;
//}
//ifs_e.close();

cout << "number of entries for pions = " << ntp_track_pi->GetEntries() << endl;
cout << "number of entries for electrons = " << ntp_track_e->GetEntries() << endl;

float px,py,pz,pt,quality,nmvtx,ntpc,eta,chi2,ndf,mom;
float cemc_e,cemc_e3x3,cemc_dphi,cemc_deta,cemc_ecore,cemc_prob,cemc_chi2;
float dca2d,hcalin_e3x3;

ntp_track_pi->SetBranchAddress("nmvtx",&nmvtx);
ntp_track_pi->SetBranchAddress("ntpc",&ntpc);
ntp_track_pi->SetBranchAddress("pt",&pt);
ntp_track_pi->SetBranchAddress("eta",&eta);
ntp_track_pi->SetBranchAddress("mom",&mom);
ntp_track_pi->SetBranchAddress("quality",&quality);
ntp_track_pi->SetBranchAddress("dca2d",&dca2d);
ntp_track_pi->SetBranchAddress("cemc_ecore",&cemc_ecore);
ntp_track_pi->SetBranchAddress("cemc_prob",&cemc_prob);
ntp_track_pi->SetBranchAddress("cemc_chi2",&cemc_chi2);
ntp_track_pi->SetBranchAddress("cemc_e3x3",&cemc_e3x3);
ntp_track_pi->SetBranchAddress("cemc_dphi",&cemc_dphi);
ntp_track_pi->SetBranchAddress("cemc_deta",&cemc_deta);
ntp_track_pi->SetBranchAddress("hcalin_e3x3",&hcalin_e3x3);

  for(int j=0; j<ntp_track_pi->GetEntries(); j++) {
    ntp_track_pi->GetEvent(j);
    if(quality>5) continue;
    if(nmvtx<1) continue;
    if(ntpc<20) continue;
    if(mom<=0.) continue;
    //if(fabs(cemc_dphi)>0.015) continue;
    //if(fabs(cemc_deta)>0.015) continue;
    if(cemc_ecore==0.) continue;
    hhoe_pi->Fill(hcalin_e3x3/cemc_ecore);
    float eop = cemc_ecore/mom; 
    hhhoe_pi->Fill(eop,hcalin_e3x3/cemc_ecore);
    if(eop>=2.0) eop = 1.999;
    heop_pi[nbins]->Fill(eop);
    for(int j=0; j<nbins; j++) { if(pt>bins[j] && pt<bins[j+1]) { heop_pi[j]->Fill(eop); } }
    hprob_pi->Fill(cemc_prob);
    hchi2_pi->Fill(cemc_chi2);
  }

//----------------------------------------------------------------------------------------------

ntp_track_e->SetBranchAddress("nmvtx",&nmvtx);
ntp_track_e->SetBranchAddress("ntpc",&ntpc);
ntp_track_e->SetBranchAddress("pt",&pt);
ntp_track_e->SetBranchAddress("eta",&eta);
ntp_track_e->SetBranchAddress("mom",&mom);
ntp_track_e->SetBranchAddress("quality",&quality);
ntp_track_e->SetBranchAddress("dca2d",&dca2d);
ntp_track_e->SetBranchAddress("cemc_ecore",&cemc_ecore);
ntp_track_e->SetBranchAddress("cemc_prob",&cemc_prob);
ntp_track_e->SetBranchAddress("cemc_chi2",&cemc_chi2);
ntp_track_e->SetBranchAddress("cemc_e3x3",&cemc_e3x3);
ntp_track_e->SetBranchAddress("cemc_dphi",&cemc_dphi);
ntp_track_e->SetBranchAddress("cemc_deta",&cemc_deta);
ntp_track_e->SetBranchAddress("hcalin_e3x3",&hcalin_e3x3);

  for(int j=0; j<ntp_track_e->GetEntries(); j++) {
    ntp_track_e->GetEvent(j);
    if(quality>5) continue;
    if(nmvtx<1) continue;
    if(ntpc<20) continue;
    if(mom<=0.) continue;
    //if(fabs(cemc_dphi)>0.015) continue;
    //if(fabs(cemc_deta)>0.015) continue;
    if(cemc_ecore==0.) continue;
    hhoe_e->Fill(hcalin_e3x3/cemc_ecore);
    float eop = cemc_ecore/mom;
    hhhoe_e->Fill(eop,hcalin_e3x3/cemc_ecore);
    if(eop>=2.0) eop = 1.999;
    heop_e[nbins]->Fill(eop);
    for(int j=0; j<nbins; j++) { if(pt>bins[j] && pt<bins[j+1]) { heop_e[j]->Fill(eop); } }
    hprob_e->Fill(cemc_prob);
    hchi2_e->Fill(cemc_chi2);
  }

//--------------------------------------------------------------------------

TCanvas* choe = new TCanvas("choe","hcalin/cemc",50,50,600,600);
choe->SetLogy();
hhoe_e->SetLineColor(kRed);
hhoe_pi->Draw();
hhoe_e->Draw("same");

TCanvas* choe2 = new TCanvas("choe2","hcalin/cemc vs e/p",100,100,600,600);
hhhoe_e->SetLineColor(kRed);
hhhoe_pi->Draw("box");
hhhoe_e->Draw("boxsame");

TCanvas* cprob = new TCanvas("cprob","prob",150,150,600,500);
hprob_e->SetLineColor(kRed);
hprob_pi->Draw();
hprob_e->Draw("same");

TCanvas* cchi2 = new TCanvas("cchi2","cemc chi2",200,200,600,500);
hchi2_e->SetLineColor(kRed);
hchi2_pi->Draw();
hchi2_e->Draw("same");

TCanvas* ceop = new TCanvas("ceop","E/p",250,250,600,500);
ceop->SetLogy();
heop_pi[nbins]->Draw();
heop_e[nbins]->SetLineColor(kRed);
heop_e[nbins]->Draw("same");

double vsume[nbins];
double vsumpi[nbins];
for(int i=0; i<nbins; i++) {vsume[i]=0.; vsumpi[i]=0.;}
double sume = 0.;
double sumpi = 0.;
int uplim = heop_e[nbins]->GetNbinsX();
for(int i=1; i<=uplim; i++) {
  sume += heop_e[nbins]->GetBinContent(i);
  sumpi += heop_pi[nbins]->GetBinContent(i);
}
cout << "total number of pions = " << sumpi << endl;
cout << "total number of electrons = " << sume << endl;

  for(int j=0; j<nbins; j++) {
    for(int i=1; i<=uplim; i++) {
      vsume[j] += heop_e[j]->GetBinContent(i);
      vsumpi[j] += heop_pi[j]->GetBinContent(i);
    }
    cout << "bin " << j << " " << vsume[j] << " " << vsumpi[j] << endl;
  }

double eleff = 0.9;

double vsumecut[nbins];
int vbincut[nbins];
for(int i = 0; i<nbins; i++) { vsumecut[i] = 0.; vbincut[i] = 0; }
double sumecut = 0.;
int bincut = 0;
for(int i=uplim; i>=1; i--) {
  sumecut += heop_e[nbins]->GetBinContent(i);
  if(sumecut>sume*eleff) {
    cout << "90% cut: " << i << " " << heop_e[nbins]->GetBinLowEdge(i) << endl;
    bincut = i;
    break;
  }
}
  for(int j=0; j<nbins; j++) {
    for(int i=uplim; i>=1; i--) {
    vsumecut[j] += heop_e[j]->GetBinContent(i);
      if(vsumecut[j]>vsume[j]*eleff) {
        cout << "bin " << j << " 90% cut: " << i << " " << heop_e[j]->GetBinLowEdge(i) << endl;
        vbincut[j] = i;
        break;
      }
    }
  }

double vsumpicut[nbins];
for(int i=0; i<nbins; i++) {vsumpicut[i]=0.;}
double sumpicut = 0.;
for(int i=bincut; i<=heop_pi[nbins]->GetNbinsX(); i++) {
  sumpicut += heop_pi[nbins]->GetBinContent(i);
}
cout << "number of misidentified pions = " << sumpicut << endl;
cout << "rejection factor = " << sumpi/sumpicut << " +- " << (sumpi/sumpicut)*(1./sqrt(sumpicut)) << endl;

  for(int j=0; j<nbins; j++) {
    for(int i=vbincut[j]; i<=heop_pi[j]->GetNbinsX(); i++) {
      vsumpicut[j] += heop_pi[j]->GetBinContent(i);
    }
    cout << "bin " << j << " number of misidentified pions = " << vsumpicut[j] << endl;
  }

double rejpi[nbins];
double errejpi[nbins];
double xx[nbins];
for(int i=0; i<nbins; i++) {
 rejpi[i] = vsumpicut[i]/vsumpi[i];
 errejpi[i] = rejpi[i]/sqrt(vsumpicut[i]);
 xx[i] = (bins[i]+bins[i+1])/2.;
}

TCanvas* c4 = new TCanvas("c4","pion rejection",120,120,600,500);
c4->SetLogy();
double lowlim = 0.001;
if(eleff==0.7) lowlim = 0.002;
TH2D* hh = new TH2D("hh","",10,1.,13.,10,lowlim,0.1);
hh->SetTitle(";Transverse momentum, [GeV] ; Inverse pion rejection factor");
hh->Draw();

TGraphErrors* gr1 = new TGraphErrors(nbins,xx,rejpi,0,errejpi);
gr1->SetMarkerStyle(20);
gr1->SetMarkerColor(kBlack);
gr1->SetMarkerSize(1.5);
gr1->SetLineColor(kBlack);
gr1->SetLineWidth(2);
gr1->Draw("p");

}

