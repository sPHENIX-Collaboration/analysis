void draw_sicalo_corr(){
  //TFile *f = TFile::Open("dphi_distribution_em.root");
  //TFile *f = TFile::Open("dphi_distribution_jpsi.root");
  //TFile *f = TFile::Open("dphi_distribution_pythia.root");
  //TFile *f = TFile::Open("dphi_distribution_pythia.root");
  TFile *f = TFile::Open("dphi_distribution.root");

//  TNtuple *ntp_sicalo = (TNtuple*)f->Get("ntp_sicalo");
  TNtuple *ntp_sicalo = (TNtuple*)f->Get("ntp_sicalotrk");

  TCanvas *c1 = new TCanvas("c1", "c1", 1000, 750);
  c1->Divide(3,2);
  c1->cd(1);
  ntp_sicalo->Draw("z_emc:z_pemc>>h(100,-150,150,100,-150,150)","e_emc>0.2&&pt>0.5","colz");
  c1->cd(2);
  ntp_sicalo->Draw("z_emc-z_pemc:z_pemc>>h2(100,-150,150, 100,-50,50)","e_emc>0.2&&pt>0.5","colz");
  c1->cd(4);
  ntp_sicalo->Draw("phi_emc:phi_pemc>>h3(100,-3.2,3.2,100,-3.2,3.2)","e_emc>0.2&&pt>0.5","colz");
  c1->cd(5);
  ntp_sicalo->Draw("phi_emc:phi_pemc>>h4(100,-3.2,3.2,100,-3.2,3.2)","e_emc>0.2&&pt>0.5&&abs(z_emc-z_pemc)<6","colz");

  c1->cd(3);
  ntp_sicalo->Draw("e_emc/sqrt(pt*pt+pz*pz):sqrt(pt*pt+pz*pz)>>h5(50,0,5,100,0,3)","abs(z_emc-z_pemc)<5","colz");
  c1->cd(6);
  ntp_sicalo->Draw("phi_emc-phi_pemc:pt>>h6(100,0,5,100,-2,2)","e_emc>0.2&&pt>0.&&abs(z_emc-z_pemc)<5","colz");


  TCanvas *c2 = new TCanvas("c2", "c2", 1000, 750);
  c2->Divide(2,2);
  c2->cd(1);
  ntp_sicalo->Draw("phi_emc-phi_pemc:pt>>h7(100,0,5,100,-2,2)","e_emc>0.2&&pt>0.&&abs(z_emc-z_pemc)<5&&abs(phi_pemc)<1.6&&c>0","colz");
  c2->cd(2);
  ntp_sicalo->Draw("phi_emc-phi_pemc:pt>>h8(100,0,5,100,-2,2)","e_emc>0.2&&pt>0.&&abs(z_emc-z_pemc)<5&&abs(phi_pemc)<1.6&&c<0","colz");
  c2->cd(3);
  ntp_sicalo->Draw("phi_emc-phi_pemc:pt>>h9(100,0,5,100,-2,2)","e_emc>0.2&&pt>0.&&abs(z_emc-z_pemc)<5&&abs(phi_pemc)>1.6&&c>0","colz");
  c2->cd(4);
  ntp_sicalo->Draw("phi_emc-phi_pemc:pt>>h10(100,0,5,100,-2,2)","e_emc>0.2&&pt>0.&&abs(z_emc-z_pemc)<5&&abs(phi_pemc)>1.6&&c<0","colz");

  TCanvas *c3 = new TCanvas("c3", "c3", 1000, 750);
  c3->Divide(2,2);
  c3->cd(1);
  ntp_sicalo->Draw("c*(phi_emc-phi_pemc):pt>>h11(100,0,5,100,-2,2)","e_emc>0.2&&pt>0.&&abs(z_emc-z_pemc)<5","colz");
  c3->cd(2);
  gPad->SetLogx();
  gPad->SetLogy();
  TH2* hh = (TH2*)gROOT->FindObject("h11");
  hh->Draw("colz");

  TF1* ff = new TF1("ff", "[0]*pow(x,[1])");
  //ff->SetParameters(0.18, -0.986);
  ff->SetParameters(0.18, -0.988);
  ff->Draw("same");

  c3->cd(3);
  ntp_sicalo->Draw("c*(phi_emc-phi_pemc)-(0.18*pow(pt,-0.986)):pt>>h12(100,0,5,100,-2,2)","e_emc>0.2&&pt>0.&&abs(z_emc-z_pemc)<5","colz");

  c3->cd(4);
ntp_sicalo->Draw("c*(phi_emc-phi_pemc):pt>>h13(100,0,5,100,-2,2)","e_emc>0.2&&pt>0.&&abs(z_emc-z_pemc)<5&&abs(c*(phi_emc-phi_pemc)-(0.21*pow(pt,-0.986)))<0.2","colz");

//
//--  ntp_sicalo->Draw("phi_emc-phi_pemc:phi_pemc>>h(100,-3.2,3.2,100,-1,1)","e_emc>0.2&&pt>0.5&&abs(z_emc-z_pemc)<5","colz")
//--  ntp_sicalo->Draw("phi_emc-phi_pemc:phi_pemc>>h(100,-3.2,3.2,100,-1,1)","e_emc>0.2&&pt>0.5&&pt<2&&abs(z_emc-z_pemc)<5","colz")
//--  ntp_sicalo->Draw("phi_emc-phi_pemc>>h(100,-2,2)","e_emc>0.2&&pt>0.5&&pt<2&&abs(z_emc-z_pemc)<5","colz")
//--  ntp_sicalo->Draw("phi_emc-phi_pemc+(-1.6<phi_pemc&&phi_pemc<1.6?-0.2:0.2):phi_pemc>>h(100,-3.2,3.2,100,-2,2)","e_emc>0.2&&pt>0.5&&pt<2&&abs(z_emc-z_pemc)<5","colz")
//--  ntp_sicalo->Draw("phi_emc-phi_pemc+(-1.6<phi_pemc&&phi_pemc<1.6?-0.2:0.2)>>h(100,-2,2)","e_emc>0.2&&pt>0.5&&pt<2&&abs(z_emc-z_pemc)<5","colz")
//--  ntp_sicalo->Draw("phi_emc-phi_pemc+(-1.6<phi_pemc&&phi_pemc<1.6?-0.2:0.2):pt>>h(100,0,5,100,-2,2)","e_emc>0.2&&pt>0.&&abs(z_emc-z_pemc)<5","colz")
//--  ntp_sicalo->Draw("phi_emc-phi_pemc>>h(100,-2,2)","e_emc>0.2&&pt>0.5&&pt<2&&abs(z_emc-z_pemc)<5","colz")
//--  ntp_sicalo->Draw("phi_emc-phi_pemc:phi_pemc>>h(100,-3.2,3.2,100,-2,2)","e_emc>0.2&&pt>0.5&&pt<2&&abs(z_emc-z_pemc)<5","colz")
//--  ntp_sicalo->Draw("phi_emc-phi_pemc>>h(100,-2,2)","e_emc>0.2&&pt>0.5&&pt<2&&abs(z_emc-z_pemc)<5","colz")
//--  ntp_sicalo->Draw("phi_emc-phi_pemc+(-1.6<phi_pemc&&phi_pemc<1.6?-0.15:0.25):pt>>h(100,0,5,100,-2,2)","e_emc>0.2&&pt>0.&&abs(z_emc-z_pemc)<5","colz")
//--  ntp_sicalo->Draw("phi_emc-phi_pemc+(-1.6<phi_pemc&&phi_pemc<1.6?-0.15:0.25):phi_pemc>>h(100,-3.2,3.2,100,-2,2)","e_emc>0.2&&pt>0.&&abs(z_emc-z_pemc)<5","colz")
//--  ntp_sicalo->Draw("phi_emc-phi_pemc+(-1.6<phi_pemc&&phi_pemc<1.6?-0.:0.):phi_pemc>>h(100,-3.2,3.2,100,-2,2)","e_emc>0.2&&pt>0.&&abs(z_emc-z_pemc)<5","colz")
//--  ntp_sicalo->Draw("phi_emc-phi_pemc+(-1.6<phi_pemc&&phi_pemc<1.6?-0.15:0.25):phi_pemc>>h(100,-3.2,3.2,100,-2,2)","e_emc>0.2&&pt>0.&&abs(z_emc-z_pemc)<5","colz")
//--  ntp_sicalo->Draw("phi_emc-phi_pemc+(-1.6<phi_pemc&&phi_pemc<1.6?-0.15:0.25)>>h(100,-2,2)","e_emc>0.2&&pt>0.&&abs(z_emc-z_pemc)<5","colz")
//--  ntp_sicalo->Draw("phi_emc-phi_pemc+(-1.6<phi_pemc&&phi_pemc<1.6?-0.15:0.25):phi_pemc>h(100,-3.2,3.2,100,-2,2)","e_emc>0.2&&pt>0.&&abs(z_emc-z_pemc)<5","colz")
//--  ntp_sicalo->Draw("phi_emc-phi_pemc+(-1.6<phi_pemc&&phi_pemc<1.6?-0.15:0.25):phi_pemc>h(100,-3.2,3.2,100,-2,2)","e_emc>0.2&&pt>0.&&abs(z_emc-z_pemc)<5","colz")
//--  ntp_sicalo->Draw("phi_emc-phi_pemc+(-1.6<phi_pemc&&phi_pemc<1.6?-0.15:0.25):phi_pemc>>h(100,-3.2,3.2,100,-2,2)","e_emc>0.2&&pt>0.&&abs(z_emc-z_pemc)<5","colz")
//--  ntp_sicalo->Draw("phi_emc-phi_pemc+(-1.6<phi_pemc&&phi_pemc<1.6?-0.15:0.25):pt>>h(100,0,5,100,-2,2)","e_emc>0.2&&pt>0.&&abs(z_emc-z_pemc)<5","colz")
//--  ntp_sicalo->Draw("c*(phi_emc-phi_pemc+(-1.6<phi_pemc&&phi_pemc<1.6?-0.15:0.25)):pt>>h(100,0,5,100,-2,2)","e_emc>0.2&&pt>0.&&abs(z_emc-z_pemc)<5","colz")
//--  ntp_sicalo->Draw("(phi_emc-phi_pemc+(-1.6<phi_pemc&&phi_pemc<1.6?-0.15:0.25)):pt>>h(100,0,5,100,-2,2)","e_emc>0.2&&pt>0.&&abs(z_emc-z_pemc)<5&&c>0","colz")
//--  ntp_sicalo->Draw("(phi_emc-phi_pemc+(-1.6<phi_pemc&&phi_pemc<1.6?-0.15:0.25)):pt>>h(100,0,5,100,-2,2)","e_emc>0.2&&pt>0.&&abs(z_emc-z_pemc)<5&&c<0","colz")
//--  ntp_sicalo->Draw("(phi_emc-phi_pemc+(-1.6<phi_pemc&&phi_pemc<1.6?-0.15:0.25)):pt>>h(100,0,5,100,-2,2)","e_emc>0.2&&pt>0.&&abs(z_emc-z_pemc)<5&&c>0","colz")
//--  ntp_sicalo->Draw("(phi_emc-phi_pemc+(-1.6<phi_pemc&&phi_pemc<1.6?-0.15:0.25)):pt>>h(100,0,5,100,-2,2)","e_emc>0.2&&pt>0.&&abs(z_emc-z_pemc)<5&&c<0","colz")
//--  ntp_sicalo->Draw("(phi_emc-phi_pemc+(-1.6<phi_pemc&&phi_pemc<1.6?-0.15:0.25)):pt>>h(100,0,5,100,-2,2)","e_emc>0.2&&pt>0.&&abs(z_emc-z_pemc)<5&&c>0","colz")
//--  ntp_sicalo->Draw("(phi_emc-phi_pemc+(-1.6<phi_pemc&&phi_pemc<1.6?-0.15:0.25)):pt>>h(100,0,5,100,-2,2)","e_emc>0.2&&pt>0.&&abs(z_emc-z_pemc)<5&&c<0","colz")
//--  ntp_sicalo->Draw("(phi_emc-phi_pemc+(-1.6<phi_pemc&&phi_pemc<1.6?-0.15:0.25)):pt>>h(100,0,5,100,-2,2)","e_emc>0.2&&pt>0.&&abs(z_emc-z_pemc)<5&&c>0","colz")
//--  ntp_sicalo->Draw("(phi_emc-phi_pemc+(-1.6<phi_pemc&&phi_pemc<1.6?-0.15:0.25)):pt>>h(100,0,5,100,-2,2)","e_emc>0.2&&pt>0.&&abs(z_emc-z_pemc)<5&&c<0","colz")
//--  ntp_sicalo->Draw("(phi_emc-phi_pemc+(-1.6<phi_pemc&&phi_pemc<1.6?-0.15:0.25)):pt>>h(100,0,5,100,-2,2)","e_emc>0.2&&pt>0.&&abs(z_emc-z_pemc)<5&&c<0&&abs(phi_pemc)<1.6","colz")
//--  ntp_sicalo->Draw("(phi_emc-phi_pemc+(-1.6<phi_pemc&&phi_pemc<1.6?-0.15:0.25)):pt>>h(100,0,5,100,-2,2)","e_emc>0.2&&pt>0.&&abs(z_emc-z_pemc)<5&&c>0&&abs(phi_pemc)<1.6","colz")
//--  ntp_sicalo->Draw("(phi_emc-phi_pemc+(-1.6<phi_pemc&&phi_pemc<1.6?-0.15:0.25)):pt>>h(100,0,5,100,-2,2)","e_emc>0.2&&pt>0.&&abs(z_emc-z_pemc)<5&&c>0&&abs(phi_pemc)>1.6","colz")
//--  ntp_sicalo->Draw("(phi_emc-phi_pemc+(-1.6<phi_pemc&&phi_pemc<1.6?-0.15:0.25)):pt>>h(100,0,5,100,-2,2)","e_emc>0.2&&pt>0.&&abs(z_emc-z_pemc)<5&&c<0&&abs(phi_pemc)>1.6","colz")
//--  ntp_sicalo->Draw("c(phi_emc-phi_pemc+(-1.6<phi_pemc&&phi_pemc<1.6?-0.15:0.25)):pt>>h(100,0,5,100,-2,2)","e_emc>0.2&&pt>0.&&abs(z_emc-z_pemc)<5&&c<0&&abs(phi_pemc)>1.6","colz")
//--  ntp_sicalo->Draw("c*(phi_emc-phi_pemc+(-1.6<phi_pemc&&phi_pemc<1.6?-0.15:0.25)):pt>>h(100,0,5,100,-2,2)","e_emc>0.2&&pt>0.&&abs(z_emc-z_pemc)<5&&c<0&&abs(phi_pemc)>1.6","colz")
//--  ntp_sicalo->Draw("c*(phi_emc-phi_pemc+(-1.6<phi_pemc&&phi_pemc<1.6?-0.15:0.25)):pt>>h(100,0,5,100,-2,2)","e_emc>0.2&&pt>0.&&abs(z_emc-z_pemc)<5&&abs(phi_pemc)>1.6","colz")
//--  ntp_sicalo->Draw("c*(phi_emc-phi_pemc+(-1.6<phi_pemc&&phi_pemc<1.6?-0.15:0.25)):pt>>h(100,0,5,100,-2,2)","e_emc>0.2&&pt>0.&&abs(z_emc-z_pemc)<5&&c<0&&abs(phi_pemc)<1.6","colz")
//--  ntp_sicalo->Draw("c*(phi_emc-phi_pemc+(-1.6<phi_pemc&&phi_pemc<1.6?-0.15:0.25)):pt>>h(100,0,5,100,-2,2)","e_emc>0.2&&pt>0.&&abs(z_emc-z_pemc)<5&&c<0&&abs(phi_pemc)>1.6","colz")
//--  ntp_sicalo->Draw("(phi_emc-phi_pemc+(-1.6<phi_pemc&&phi_pemc<1.6?-0.15:0.25)):pt>>h(100,0,5,100,-2,2)","e_emc>0.2&&pt>0.&&abs(z_emc-z_pemc)<5&&c<0&&abs(phi_pemc)>1.6&&c>0","colz")
//--  ntp_sicalo->Draw("(phi_emc-phi_pemc+(-1.6<phi_pemc&&phi_pemc<1.6?-0.15:0.25)):pt>>h(100,0,5,100,-2,2)","e_emc>0.2&&pt>0.&&abs(z_emc-z_pemc)<5&&abs(phi_pemc)>1.6&&c>0","colz")
//--  ntp_sicalo->Draw("(phi_emc-phi_pemc+(-1.6<phi_pemc&&phi_pemc<1.6?-0.15:0.25)):pt>>h(100,0,5,100,-2,2)","e_emc>0.2&&pt>0.&&abs(z_emc-z_pemc)<5&&abs(phi_pemc)>1.6&&c<0","colz")
//--  ntp_sicalo->Draw("(phi_emc-phi_pemc+(-1.6<phi_pemc&&phi_pemc<1.6?-0.15:0.25)):pt>>h(100,0,5,100,-2,2)","e_emc>0.2&&pt>0.&&abs(z_emc-z_pemc)<5&&abs(phi_pemc)<1.6&&c<0","colz")
//--  ntp_sicalo->Draw("(phi_emc-phi_pemc+(-1.6<phi_pemc&&phi_pemc<1.6?-0.15:0.25)):pt>>h(100,0,5,100,-2,2)","e_emc>0.2&&pt>0.&&abs(z_emc-z_pemc)<5&&abs(phi_pemc)<3.2&&c<0","colz")
//--  ntp_sicalo->Draw("(phi_emc-phi_pemc+(-1.6<phi_pemc&&phi_pemc<1.6?-0.15:0.25)):pt>>h(100,0,5,100,-2,2)","e_emc>0.2&&pt>0.&&abs(z_emc-z_pemc)<5&&abs(phi_pemc)<1.6&&c<0","colz")
//--  ntp_sicalo->Draw("(phi_emc-phi_pemc+(-1.6<phi_pemc&&phi_pemc<1.6?-0.15:0.25)):pt>>h(100,0,5,100,-2,2)","e_emc>0.2&&pt>0.&&abs(z_emc-z_pemc)<5&&abs(phi_pemc)>1.6&&c<0","colz")
//--  ntp_sicalo->Draw("(phi_emc-phi_pemc+(-1.6<phi_pemc&&phi_pemc<1.6?-0.15:0.25)):pt>>h(100,0,5,100,-2,2)","e_emc>0.2&&pt>0.&&abs(z_emc-z_pemc)<5&&abs(phi_pemc)<1.6&&c<0","colz")
//--  ntp_sicalo->Draw("(phi_emc-phi_pemc+(-1.6<phi_pemc&&phi_pemc<1.6?-0.15:0.25)):pt>>h(100,0,5,100,-2,2)","e_emc>0.2&&pt>0.&&abs(z_emc-z_pemc)<5&&abs(phi_pemc)>1.6&&c<0","colz")
}

//--.x Fun4All_DataDST_SiliconSeedAna.C
//--TFile *_file0 = TFile::Open("test_ana.root")
//--evtTree->Draw("nmaps:nintt50","", "colz")
//--.q
//--.x Fun4All_DataDST_SiliconSeedAna.C
//--TFile *_file0 = TFile::Open("test_ana.root")
//--evtTree->Draw("nmaps:nintt50","", "colz")
//--.ls
//--evtTree->Print()
//--evtTree->Draw("xvtx")
//--evtTree->Draw("yvtx")
//--evtTree->Draw("zvtx")
//--evtTree->Draw("yvtx:xvtx>>h(100,-1,1,100,-1,1)","","colz")
//--evtTree->Draw("yvtx:xvtx>>h(200,-1,1,200,-1,1)","","colz")
//--evtTree->Draw("yvtx:xvtx>>h(200,-0.5,0.5,200,-0.5,0.5)","","colz")
//--evtTree->Draw("xvtx>>h(200,-0.5,0.5)","","colz")
//--evtTree->Draw("xvtx>>h(400,-0.5,0.5)","","colz")
//--evtTree->Draw("yvtx>>h(400,-0.5,0.5)","","colz")
//--evtTree->Draw("zvtx>>h(400,-20,20)","","colz")
//--evtTree->Draw("zvtx>>h(100,-20,20)","","colz")
//--evtTree->Draw("nmaps:nintt50","abs(zvtx)<10", "colz")
//--evtTree->Draw("nmaps:nintt50>>h(100,0,500,100,0,500)","abs(zvtx)<10", "colz")
//--evtTree->Draw("nmaps:nintt50>>h(100,0,1000,100,0,1000)","abs(zvtx)<10", "colz")
//--evtTree->Draw("nmaps:nintt50>>h(100,0,1000,100,0,1000)","abs(zvtx)<5", "colz")
//--evtTree->Draw("nmaps:nintt50>>h(100,0,1000,100,0,1000)","abs(zvtx)<10", "colz")
//--evtTree->Draw("nmaps:nintt50>>h(100,0,1000,100,0,1000)","abs(zvtx)<30", "colz")
//--.q
//--.x Fun4All_DataDST_SiliconSeedAna.C
//--.x analyze_SiSeedPair.C
//--.x analyze_SiSeedPair.C
//--TFile *_file0 = TFile::Open("dphi_distribution.root")
//--.ls
//--ntp_sicalo->Print()
//--ntp_sicalo->Draw("phi_emc:phi_pemc")
//--ntp_sicalo->Draw("phi_emc:phi_pemc>>h(100,-3.2,3.2,100,-3.2,3.2)","","colz")
//--ntp_sicalo->Draw("phi","","colz")
//--ntp_sicalo->Draw("phi_emc:phi_pemc>>h(100,-3.2,3.2,100,-3.2,3.2)","","colz")
//--ntp_sicalo->Draw("phi_emc:phi_pemc>>h(100,-3.2,3.2,100,-3.2,3.2)","e_emc>0.2","colz")
//--ntp_sicalo->Draw("e_emc","","colz")
//--ntp_sicalo->Draw("phi_emc:phi_pemc>>h(100,-3.2,3.2,100,-3.2,3.2)","e_emc>0.2","colz")
//--ntp_sicalo->Draw("phi_emc:phi_pemc>>h(320,-3.2,3.2,320,-3.2,3.2)","e_emc>0.2","colz")
//--ntp_sicalo->Draw("phi_emc:phi_pemc>>h(160,-3.2,3.2,160,-3.2,3.2)","e_emc>0.2","colz")
//--ntp_sicalo->Draw("phi_emc>>h(160,-3.2,3.2,160,-3.2,3.2)","e_emc>0.2","colz")
//--.ls
//--.q
//--TFile *_file0 = TFile::Open("../macro/data//test_ana2k.root")
//--.ls
//--caloTree->Print()
//--caloTree->Draw("phi")
//--caloTree->Draw("phi>>h(160,-3,2,3,2)")
//--caloTree->Draw("phi>>h(175,-3,5,3,5)")
//--caloTree->Draw("phi>>h(175,-3.5,3.5)")
//--caloTree->Draw("phi:z>>h(100,-200,200,175,-3.5,3.5)","","colz")
//--caloTree->Draw("z>>h(100,-200,200)","","colz")
//--.q
//--TFile *_file0 = TFile::Open("dphi_distribution.root")
//--ntp_sicalo->Print()
//--ntp_sicalo->Draw("phi_emc:phi_pemc>>h(320,-3.2,3.2,320,-3.2,3.2)","e_emc>0.2&&pt>0.5","colz")
//--ntp_sicalo->Draw("phi_emc:phi_pemc>>h(160,-3.2,3.2,160,-3.2,3.2)","e_emc>0.2&&pt>0.5","colz")
//--ntp_sicalo->Draw("phi_emc-phi_pemc","e_emc>0.2&&pt>0.5","colz")
//--ntp_sicalo->Draw("z_emc-z_pemc","e_emc>0.2&&pt>0.5","colz")
//--ntp_sicalo->Draw("z_emc:z_pemc","e_emc>0.2&&pt>0.5","colz")
//--ntp_sicalo->Draw("z_emc:z_pemc>>h(100,-150,150,100,-150,150)","e_emc>0.2&&pt>0.5","colz")
//--ntp_sicalo->Draw("z_emc:z_pemc>>h(150,-150,150,150,-150,150)","e_emc>0.2&&pt>0.5","colz")
//--ntp_sicalo->Draw("z_emc:z_pemc>>h(100,-150,150,100,-150,150)","e_emc>0.2&&pt>0.5","colz")
//--ntp_sicalo->Draw("phi_emc:phi_pemc>>h(160,-3.2,3.2,160,-3.2,3.2)","e_emc>0.2&&pt>0.5","colz")
//--ntp_sicalo->Draw("phi_emc:phi_pemc>>h(80,-3.2,3.2,80,-3.2,3.2)","e_emc>0.2&&pt>0.5","colz")
//--ntp_sicalo->Draw("phi_emc:phi_pemc>>h(160,-3.2,3.2,160,-3.2,3.2)","e_emc>0.2&&pt>0.5","colz")
//--ntp_sicalo->Draw("z_emc-z_pemc:z_pemc>>h(100,-150,150,100,-150,150)","e_emc>0.2&&pt>0.5","colz")
//--ntp_sicalo->Draw("z_emc-z_pemc:z_pemc>>h(100,-150,150,100,-20,20)","e_emc>0.2&&pt>0.5","colz")
//--ntp_sicalo->Draw("z_emc-z_pemc:z_pemc>>h(100,-150,150,100,-50,50)","e_emc>0.2&&pt>0.5","colz")
//--ntp_sicalo->Draw("z_emc-z_pemc>>h(100,-50,50)","e_emc>0.2&&pt>0.5","colz")
//--ntp_sicalo->Draw("phi_emc:phi_pemc>>h(160,-320,320,160,-320,320)","e_emc>0.2&&pt>0.5","colz")
//--ntp_sicalo->Draw("phi_emc:phi_pemc>>h(160,-3.20,3.20,160,-3.20,3.20)","e_emc>0.2&&pt>0.5","colz")
//--ntp_sicalo->Draw("phi_emc:phi_pemc>>h(160,-3.20,3.20,160,-3.20,3.20)","e_emc>0.2&&pt>0.5&&abs(z_emc-z_pemc)<5","colz")
//--ntp_sicalo->Draw("phi_emc-phi_pemc>>h(100,-1,1)","e_emc>0.2&&pt>0.5&&abs(z_emc-z_pemc)<5","colz")
//--ntp_sicalo->Draw("phi_emc-phi_pemc>>h(100,-2,2)","e_emc>0.2&&pt>0.5&&abs(z_emc-z_pemc)<5","colz")
//--ntp_sicalo->Draw("phi_emc-phi_pemc:phi_pemc>>h(100,-3.2,3.2,100,-2,2)","e_emc>0.2&&pt>0.5&&abs(z_emc-z_pemc)<5","colz")
//--ntp_sicalo->Print()
//--ntp_sicalo->Draw("e_emc:sqrt(pt*pt+pz*pz)","","colz"
//--.@
//--ntp_sicalo->Draw("e_emc:sqrt(pt*pt+pz*pz)","","colz")
//--ntp_sicalo->Draw("e_emc:sqrt(pt*pt+pz*pz)","pt<10","colz")
//--ntp_sicalo->Draw("e_emc:sqrt(pt*pt+pz*pz)","pt<10&&pz<10","colz")
//--ntp_sicalo->Draw("sqrt(pt*pt+pz*pz)","pt<10&&pz<10","colz")
//--ntp_sicalo->Draw("e_emc:sqrt(pt*pt+pz*pz)","pt<10&&pz<10","colz")
//--ntp_sicalo->Draw("e_emc/sqrt(pt*pt+pz*pz)","pt<10&&pz<10","colz")
//--ntp_sicalo->Draw("e_emc/sqrt(pt*pt+pz*pz)>>h(100,0,3)","pt<10&&pz<10","colz")
//--ntp_sicalo->Draw("e_emc/sqrt(pt*pt+pz*pz):pt>>h(100,0,10,100,0,3)","pt<10&&pz<10","colz")
//--ntp_sicalo->Draw("e_emc/sqrt(pt*pt+pz*pz):sqrt(pt*pt+pz*pz)>>h(100,0,10,100,0,3)","pt<10&&pz<10","colz")
//--ntp_sicalo->Draw("e_emc/sqrt(pt*pt+pz*pz):sqrt(pt*pt+pz*pz)>>h(500,0,50,100,0,3)","pt<10&&pz<10","colz")
//--ntp_sicalo->Draw("e_emc/sqrt(pt*pt+pz*pz):sqrt(pt*pt+pz*pz)>>h(50,0,50,100,0,3)","pt<10&&pz<10","colz")
//--ntp_sicalo->Draw("e_emc/sqrt(pt*pt+pz*pz):sqrt(pt*pt+pz*pz)>>h(50,0,5,100,0,3)","pt<10&&pz<10","colz")
//--ntp_sicalo->Draw("e_emc/sqrt(pt*pt+pz*pz):sqrt(pt*pt+pz*pz)>>h(50,0,50,100,0,3)","pt<10&&pz<10&&abs(z_emc-z_pemc)<5","colz")
//--ntp_sicalo->Draw("e_emc/sqrt(pt*pt+pz*pz):sqrt(pt*pt+pz*pz)>>h(50,0,5,100,0,3)","pt<10&&pz<10&&abs(z_emc-z_pemc)<5","colz")
//--.q
//--TFile *_file0 = TFile::Open("../macro/data/test_ana.root")
//--.ls
//--SiClusTree->Print()
//--SiClusTree->Draw("Siclus_z:layer","","colz")
//--SiClusTree->Draw("Siclus_z:Siclus_layer","","colz")
//--SiClusTree->Draw("Siclus_z:Siclus_layer>>h(8,0,8,100,-30,30)","","colz")
//--SiClusTree->Draw("Siclus_z:Siclus_layer>>h(8,0,8,300,-30,30)","","colz")
//--SiClusTree->Draw("Siclus_y:Siclus_x>>h(300,-30,30,300,-30,30)","","colz")
//--SiClusTree->Draw("Siclus_y:Siclus_x>>h(300,-15,15,300,-15,15)","","colz")
//--SiClusTree->Draw("Siclus_z:Siclus_layer>>h(8,0,8,300,-30,30)","","colz")
//--.q
//--TFile *_file0 = TFile::Open("/sphenix/lustre01/sphnxpro/production2/run2pp/physics/new_nocdbtag_v007/run_00052000_00052100/DST_TRIGGERED_EVENT_seb00_run2pp_new_nocdbtag_v007-00052077-00000.root")
//--.q
//--TFile *_file0 = TFile::Open("test_ana.root")
//--ls
//--.ls
//--evtTree->Print()
//--.q
//--.x Fun4All_DataDST_SiliconSeedAna.C
//--.x analyze_SiSeedPair.C
//--TFile *_file0 = TFile::Open("dphi_distribution.root")
//--SiClusTree->Draw("Siclus_y:Siclus_x>>h(300,-15,15,300,-15,15)","","colz")
//--.q
