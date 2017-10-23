#include <Riostream.h>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <string>

#include <TMath.h>
#include <TEnv.h>
#include <TH2F.h>
#include <TH1F.h>
#include <TTree.h>
#include <TDatabasePDG.h>
#include <TStyle.h>
#include <TLine.h>
#include <TEllipse.h>
#include <TCanvas.h>
#include <TProfile.h>
#include <TH1D.h>
#include <TH3F.h>
#include <TF1.h>
#include <TText.h>
#include <TLegend.h>
#include <TChain.h>
#include <TString.h>
#include <TLorentzVector.h>
#include <TRandom.h>
#include <TMatrixTBase.h>
#include <TMatrixT.h>
#include "TPaveText.h"
#include <TGraphErrors.h>
#include <TFile.h>
#include <TTree.h>
#include <TSpline.h>
#include <TFrame.h>

using namespace std;

class eic_bnl_rich {

 private:

 public:

  Double_t ind_ray(double Ex, double Ey, double Ez, double Dx, double Dy, double Dz, double cx, double cy, double cz, double vx, double vy, double vz);
  
  void acq(string input_filename, int ind);
  Double_t c_mean, c_rms, p_ang;
  Int_t sec;

  void run();

};

Double_t eic_bnl_rich::ind_ray(double Ex, double Ey, double Ez, double Dx, double Dy, double Dz, double cx, double cy, double cz, double vx, double vy, double vz){

  //Double_t cx = 100.;
  //Double_t cy = 0.;
  //Double_t cz = 134.5;

  //Double_t cx,cy,cz;
  //Double_t sx,sy,sz;
  LongDouble_t cex,cey,cez;
  LongDouble_t cdx,cdy,cdz;

  Int_t i,iwhere;

  LongDouble_t th,a,d;
     LongDouble_t x,dx;

  LongDouble_t y,y1;

  LongDouble_t eps = 0.00000000001;
  LongDouble_t R = 195.;  //rich
  //LongDouble_t R = 299.9;    //rich3

  //eic_dual_rich *f = new eic_dual_rich();

  Double_t sx,sy,sz;
  Double_t esx,esy,esz;
  Double_t es, theta_c;

  cex = -cx+Ex;
  cey = -cy+Ey;
  cez = -cz+Ez;

  cdx = -cx+Dx;
  cdy = -cy+Dy;
  cdz = -cz+Dz;

  //cout<<"ce is: "<<cex<<"  "<<cey<<"  "<<cez<<endl;
  //cout<<"cd is: "<<cdx<<"  "<<cdy<<"  "<<cdz<<endl;

  a = TMath::Sqrt(cex*cex+cey*cey+cez*cez);
  d = TMath::Sqrt(cdx*cdx+cdy*cdy+cdz*cdz);
  th = TMath::ACos((cdx*cex+cdy*cey+cdz*cez)/a/d);
  
  //cout<<"a,d,th is: "<<a<<"  "<<d<<"  "<<th<<endl;

  i = 0;
  x = th/2.;  
  //cout<<"x, sinx, sin(th-x) is:  "<<x<<"  "<<sin(x)<<"  "<<sin(th-x)<<endl;
  y = R*(a*sin(x)-d*sin(th-x))+a*d*sin(th-2*x);
  y1 = R*(a*cos(x)+d*cos(th-x))-2*a*d*cos(th-2*x);
  //cout<<"y, y1 is:  "<<y<<"  "<<y1<<endl;
  //dx = -(f->newp(x, th, a, d, R)/f->newpp(x, th, a, d, R));
  dx = -y/y1;

  while(TMath::Abs(dx)>eps && i<100){

    x+=dx;
    y = R*(a*sin(x)-d*sin(th-x))+a*d*sin(th-2*x);
    y1 = R*(a*cos(x)+d*cos(th-x))-2*a*d*cos(th-2*x);
    //dx = -(f->newp(x, th, a, d, R)/f->newpp(x, th, a, d, R));
    dx = -y/y1;
    i++;

  }

  //if(i>=100) cout<<"Not convergent"<<endl;

  if(i<100){
    sx = cx + (R*cos(x)/a-R*sin(x)/tan(th)/a)*cex + (R*sin(x)/d/sin(th))*cdx;
    sy = cy + (R*cos(x)/a-R*sin(x)/tan(th)/a)*cey + (R*sin(x)/d/sin(th))*cdy;
    sz = cz + (R*cos(x)/a-R*sin(x)/tan(th)/a)*cez + (R*sin(x)/d/sin(th))*cdz;

  }
  
  esx = sx - Ex;
  esy = sy - Ey;
  esz = sz - Ez;

  //cout<<"S: "<<sx<<"  "<<sy<<"  "<<sz<<endl;

  es = sqrt(esx*esx+esy*esy+esz*esz);

  esx = esx/es;
  esy = esy/es;
  esz = esz/es;

  theta_c = TMath::ACos((esx*vx+esy*vy+esz*vz));

  return theta_c;
  
}

void eic_bnl_rich::acq(string input_filename, int ind){

  c_mean = 0.;
  c_rms = 0.;
  p_ang = 0.;

  Double_t rtd = 180./TMath::Pi();
  
  TFile *file=new TFile(input_filename.c_str());
  if (file->IsZombie()) {
    cout << "Error opening file" << input_filename << endl;
    exit(-1);
  }
  //else cout << "open file " << input_filename << endl;

  TTree *eic_rich = (TTree*) file->Get("tree_rich");
  
  Int_t eic_rich_event=0,eic_rich_bankid=0,eic_rich_volumeid=0,eic_rich_hitid=0,*eic_rich_pid=0,eic_rich_mpid=0,eic_rich_trackid=0,eic_rich_mtrackid=0,eic_rich_otrackid=0;
  Double_t eic_rich_hit_x=0,eic_rich_hit_y=0,eic_rich_hit_z=0,eic_rich_lhit_x=0,eic_rich_lhit_y=0,eic_rich_lhit_z=0, eic_rich_emi_x=0, eic_rich_emi_y=0, eic_rich_emi_z=0,eic_rich_px=0,eic_rich_py=0,eic_rich_pz=0,eic_rich_mpx=0,eic_rich_mpy=0,eic_rich_mpz=0,eic_rich_e=0,eic_rich_me=0,eic_rich_edep=0;

  eic_rich->SetBranchAddress("event",&eic_rich_event);
  eic_rich->SetBranchAddress("hit_x",&eic_rich_hit_x);
  eic_rich->SetBranchAddress("hit_y",&eic_rich_hit_y);
  eic_rich->SetBranchAddress("hit_z",&eic_rich_hit_z);
  eic_rich->SetBranchAddress("lhit_x",&eic_rich_lhit_x);
  eic_rich->SetBranchAddress("lhit_y",&eic_rich_lhit_y);
  eic_rich->SetBranchAddress("lhit_z",&eic_rich_lhit_z);
  eic_rich->SetBranchAddress("emi_x",&eic_rich_emi_x);
  eic_rich->SetBranchAddress("emi_y",&eic_rich_emi_y);
  eic_rich->SetBranchAddress("emi_z",&eic_rich_emi_z);
  eic_rich->SetBranchAddress("px",&eic_rich_px);
  eic_rich->SetBranchAddress("py",&eic_rich_py);
  eic_rich->SetBranchAddress("pz",&eic_rich_pz);
  eic_rich->SetBranchAddress("mpx",&eic_rich_mpx);
  eic_rich->SetBranchAddress("mpy",&eic_rich_mpy);
  eic_rich->SetBranchAddress("mpz",&eic_rich_mpz);
  eic_rich->SetBranchAddress("e",&eic_rich_e);
  eic_rich->SetBranchAddress("me",&eic_rich_me);
  eic_rich->SetBranchAddress("edep",&eic_rich_edep);
  eic_rich->SetBranchAddress("bankid",&eic_rich_bankid);
  eic_rich->SetBranchAddress("volumeid",&eic_rich_volumeid);
  eic_rich->SetBranchAddress("hitid",&eic_rich_hitid);
  eic_rich->SetBranchAddress("pid",&eic_rich_pid);
  eic_rich->SetBranchAddress("mpid",&eic_rich_mpid);
  eic_rich->SetBranchAddress("trackid",&eic_rich_trackid);
  eic_rich->SetBranchAddress("mtrackid",&eic_rich_mtrackid);
  eic_rich->SetBranchAddress("otrackid",&eic_rich_otrackid);

  TRandom rran;
  rran.SetSeed(0);
  Int_t ph_count = 0;
  
  TH1F *h = new TH1F("","",1000,-70,70);
  TH1F *ch_ang = new TH1F("","",1000,0.,0.3);
  Double_t ang_cut = 0.;
  Double_t momv[3] = {0.,0.,0.};
  Double_t m_emi[3] = {0.,0.,0.};
  Double_t d_hit[3] = {0.,0.,0.};
  Double_t theta = 0.;
  Double_t phi = 0.;
  Double_t rr[3] = {0.,0.,0.};
  Double_t ch = 0.;
  Double_t theta_cc = 0.;
  Double_t m[4]={0.000511, 0.13957018, 0.493677, 0.938272};
  theta_cc = acos(sqrt(30.*30.+m[1]*m[1])/30./1.000482);
  Double_t em[3][500];
  Double_t vv = 0.;
 
  //theta_cc = 0.02;
  
    //cout<<theta_cc<<endl;

  for(Int_t i=0;(i<eic_rich->GetEntries());i++){
    
    eic_rich->GetEntry(i);

    if(eic_rich_event==ind){
	  
      ph_count++;

      em[0][ph_count-1] = eic_rich_emi_x;
      em[1][ph_count-1] = eic_rich_emi_y;
      em[2][ph_count-1] = eic_rich_emi_z;

      //cout<<em[2][0]<<"  "<<em[2][11]<<endl;

      vv = sqrt((em[0][11]-em[0][0])*(em[0][11]-em[0][0])+(em[1][11]-em[1][0])*(em[1][11]-em[1][0])+(em[2][11]-em[2][0])*(em[2][11]-em[2][0]));
      
      if(ph_count>=2){
	momv[0]=(em[0][11]-em[0][0])/vv;
	momv[1]=(em[1][11]-em[1][0])/vv;
	momv[2]=(em[2][11]-em[2][0])/vv;
      }
    }

  }
  
  for(Int_t i=0;(i<eic_rich->GetEntries());i++){
    
    eic_rich->GetEntry(i);
    //if(eic_rich_event==1){
    //h->Fill(eic_rich_mpz);
    //}

    ang_cut = TMath::ACos(eic_rich_mpz/sqrt(eic_rich_mpx*eic_rich_mpx+eic_rich_mpy*eic_rich_mpy+eic_rich_mpz*eic_rich_mpz))*rtd;
    
    //if(eic_rich_event<=100 && ang_cut<8 && ang_cut>7){
    if(eic_rich_event==ind){

      //ph_count++;
      
      //momv[0] = eic_rich_mpx/sqrt(eic_rich_mpx*eic_rich_mpx+eic_rich_mpy*eic_rich_mpy+eic_rich_mpz*eic_rich_mpz);
      //momv[1] = eic_rich_mpy/sqrt(eic_rich_mpx*eic_rich_mpx+eic_rich_mpy*eic_rich_mpy+eic_rich_mpz*eic_rich_mpz);
      //momv[2] = eic_rich_mpz/sqrt(eic_rich_mpx*eic_rich_mpx+eic_rich_mpy*eic_rich_mpy+eic_rich_mpz*eic_rich_mpz);
      
      /*momv[0] = eic_rich_emi_x/sqrt(eic_rich_emi_x*eic_rich_emi_x+eic_rich_emi_y*eic_rich_emi_y+eic_rich_emi_z*eic_rich_emi_z);
      momv[1] = eic_rich_emi_y/sqrt(eic_rich_emi_x*eic_rich_emi_x+eic_rich_emi_y*eic_rich_emi_y+eic_rich_emi_z*eic_rich_emi_z);
      momv[2] = eic_rich_emi_z/sqrt(eic_rich_emi_x*eic_rich_emi_x+eic_rich_emi_y*eic_rich_emi_y+eic_rich_emi_z*eic_rich_emi_z);*/  

      //cout<<sqrt(eic_rich_mpx*eic_rich_mpx+eic_rich_mpy*eic_rich_mpy+eic_rich_mpz*eic_rich_mpz)<<endl;
      
      theta = TMath::ACos(momv[2]);
      phi = TMath::ATan2(momv[1],momv[0]);

      p_ang = theta*rtd;
      
      //cout<<theta*rtd<<endl;
      //cout<<1240./(eic_rich_e*1000000000.)<<endl;
      
      /*m_emi[0] = ((220.+rran.Uniform(-40.,40.))/momv[2])*momv[0];
      m_emi[1] = ((220.+rran.Uniform(-40.,40.))/momv[2])*momv[1];
      m_emi[2] = ((220.+rran.Uniform(-40.,40.))/momv[2])*momv[2];*/

      /*m_emi[0] = ((220.)/momv[2])*momv[0];  //mean emission point
      m_emi[1] = ((220.)/momv[2])*momv[1];
      m_emi[2] = ((220.)/momv[2])*momv[2];*/

      m_emi[0] = eic_rich_emi_x; //real emission point
      m_emi[1] = eic_rich_emi_y;
      m_emi[2] = eic_rich_emi_z;
      
      d_hit[0] = eic_rich_hit_x;
      d_hit[1] = eic_rich_hit_y;
      d_hit[2] = eic_rich_hit_z;
      
      //cout<<theta*rtd<<"  "<<phi*rtd<<endl;
      //cout<<eic_rich_volumeid<<endl;

      rr[0] = -18.5*TMath::Sin(eic_rich_volumeid*TMath::Pi()/4.);
      rr[1] = 18.5*TMath::Cos(eic_rich_volumeid*TMath::Pi()/4.);
      rr[2] = 75.;
      sec = eic_rich_volumeid;

      ch = ind_ray(m_emi[0], m_emi[1], m_emi[2], d_hit[0], d_hit[1], d_hit[2], rr[0], rr[1], rr[2], momv[0], momv[1], momv[2]);

      cout<<"V (momv):  "<<momv[0]<<"  "<<momv[1]<<"  "<<momv[2]<<endl;
      cout<<"E (m_emi): "<<m_emi[0]<<"  "<<m_emi[1]<<"  "<<m_emi[2]<<endl;
      cout<<"D (d_hit): "<<d_hit[0]<<"  "<<d_hit[1]<<"  "<<d_hit[2]<<endl;
      cout<<"ctr:       "<<rr[0]<<"  "<<rr[1]<<"  "<<rr[2]<<endl;
      cout<<"theta_c:   "<<ch<<endl;
      cout << "=====" << endl;

      //cout<<ch<<endl;
      if(ch<(theta_cc+0.01) && ch>(theta_cc-0.01))ch_ang->Fill(ch);
    }
    
  }

  //cout<<ph_count<<endl;
  
  ch_ang->Draw();

  c_mean = ch_ang->GetMean();
  c_rms = ch_ang->GetRMS();

  //delete ch_ang;
}
  
void eic_bnl_rich::run(){

  ofstream myfile;
  myfile.open ("prova.txt");
  Double_t sigma[300], pang[300];
  Int_t ii = 0;
  Int_t imax = 1;

  for(Int_t i=1;i<=imax;i++){
    acq("eval_RICH.root",i);
    if(i<=100 && c_rms!=0){
      sigma[ii] = c_rms;
      pang[ii] = p_ang;
      ii++;
    }
    //cout<<c_mean<<"  "<<c_rms<<"  "<<p_ang<<endl;
    myfile <<i<<"  "<<c_mean<<"  "<<c_rms<<"  "<<p_ang<<endl;
  }

  TGraph *gr = new TGraph(ii,pang,sigma);
  gr->SetTitle("");
  gr->Draw("AP*");
  
  myfile.close();
  
}
