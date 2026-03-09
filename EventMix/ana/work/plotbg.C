void plotbg()
{
gStyle->SetOptStat(0);
TH1F* hmass4 = new TH1F("hmass4","",60,5.,11.);
TH1F* hmass23 = new TH1F("hmass23","",60,5.,11.);
TH1F* hmass56 = new TH1F("hmass56","",60,5.,11.);

float mass,type,pt,eta,rap,pt1,pt2,eta1,eta2,bimp;
float chisq1,chisq2,dca2d1,dca2d2,e3x31,e3x32,p1,p2;
float nmvtx1,nmvtx2,ntpc1,ntpc2;

TChain* ntp1 =  new TChain("ntp1");
ntp1->Add("test.root");

cout << "number of entries = " << ntp1->GetEntries() << endl;

ntp1->SetBranchAddress("type",&type);
ntp1->SetBranchAddress("mass",&mass);
ntp1->SetBranchAddress("pt",&pt);
ntp1->SetBranchAddress("eta",&eta);
ntp1->SetBranchAddress("rap",&rap);
ntp1->SetBranchAddress("p1",&p1);
ntp1->SetBranchAddress("p2",&p2);
ntp1->SetBranchAddress("pt1",&pt1);
ntp1->SetBranchAddress("pt2",&pt2);
ntp1->SetBranchAddress("chisq1",&chisq1);
ntp1->SetBranchAddress("dca2d1",&dca2d1);
ntp1->SetBranchAddress("chisq2",&chisq2);
ntp1->SetBranchAddress("dca2d2",&dca2d2);
ntp1->SetBranchAddress("nmvtx1",&nmvtx1);
ntp1->SetBranchAddress("nmvtx2",&nmvtx2);
ntp1->SetBranchAddress("ntpc1",&ntpc1);
ntp1->SetBranchAddress("ntpc2",&ntpc2);
ntp1->SetBranchAddress("e3x31",&e3x31);
ntp1->SetBranchAddress("e3x32",&e3x32);

cout << "start loop..." << endl;
 for(int j=0; j<ntp1->GetEntries(); j++) {
   ntp1->GetEvent(j);
   if(p1==0. || p2==0.) continue;
   if(e3x31/p1<0.7 || e3x32/p2<0.7) continue;
   if(nmvtx1<2 || nmvtx2<2) continue;
   if(ntpc1<20 || ntpc2<20) continue;
   if(chisq1>10 || chisq2>10) continue;
   if(type==5 || type==6) { hmass56->Fill(mass); }
   if(type==2 || type==3) { hmass23->Fill(mass); }
   if(ntpc1<26 || ntpc2<26) continue;
   if(chisq1>3 || chisq2>3) continue;
   if(type==4) { hmass4->Fill(mass); }
 }

//hmass56->Divide(hmass23);

hmass4->Draw();

}
