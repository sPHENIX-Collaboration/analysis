/*
  Adjustments made to original:
  Fixed a |= somewhere that should have been !=
  Increased size of em[][] array
  Added if statement to make sure imom != 0
  Changed the i/o to append to existing output file
  Changed read() to read(string input_filename)
  Changed "tree_rich" to updated "eval_rich"
  
  Personal preference: Changed the bounds on polar angle to [15,16] instead of [12,13]
*/

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
  
  void acq(string input_filename, int ind, int pix_lab, int part);
  Double_t momentum, ch, ch_v[500];
  Double_t c_mean, c_rms, p_ang;
  Int_t sec;

  double qe(double energy);
  
  void read(string input_filename);

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

double eic_bnl_rich::qe(double energy){

  Double_t qe = 0.;

  qe = 12.5*(energy-6.2);
  if(energy>10.3) qe = 0.; // no photons below 120 nm = 10.3 eV
  
  return qe;
  
}

void eic_bnl_rich::acq(string input_filename, int ind, int pix_lab, int part){
  
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

  TTree *eic_rich = (TTree*) file->Get("eval_rich");
  
  Int_t eic_rich_event=0,eic_rich_bankid=0,eic_rich_volumeid=0,eic_rich_hitid=0,*eic_rich_pid=0,eic_rich_mpid=0,eic_rich_trackid=0,eic_rich_mtrackid=0,eic_rich_otrackid=0;
  Double_t eic_rich_hit_x=0,eic_rich_hit_y=0,eic_rich_hit_z=0, eic_rich_emi_x=0, eic_rich_emi_y=0, eic_rich_emi_z=0,eic_rich_px=0,eic_rich_py=0,eic_rich_pz=0,eic_rich_mpx=0,eic_rich_mpy=0,eic_rich_mpz=0,eic_rich_e=0,eic_rich_me=0,eic_rich_edep=0;

  eic_rich->SetBranchAddress("event",&eic_rich_event);
  eic_rich->SetBranchAddress("hit_x",&eic_rich_hit_x);
  eic_rich->SetBranchAddress("hit_y",&eic_rich_hit_y);
  eic_rich->SetBranchAddress("hit_z",&eic_rich_hit_z);
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
  Int_t ev_count[61];
  for(Int_t i=0;i<61;i++) ev_count[i]=0;
  Int_t ph_count = 0;
  Int_t ph_ch_count = 0;

  TH2F *ph_det = new TH2F("ph_det","", 1666, -250., 250., 1666, -250., 250.);
  TH2F *ph_det1 = new TH2F("ph_det1","", 1666, -250., 250., 1666, -250., 250.);
  Double_t momvv_x[10000];
  Double_t momvv_y[10000];
  Double_t momvv_z[10000];
  
  TH1F *h = new TH1F("","",1000,-70,70);
  TH1F *spectrum_ev = new TH1F("","",1000,4.,12.);
  TH1F *spectrum_nm = new TH1F("","",1000,100.,300.);
  TH1F *ch_ang = new TH1F("","",1000,0.,0.3);
  Double_t ang_cut = 0.;
  Double_t momv[3] = {0.,0.,0.};
  //Double_t momvv[10000][3];
  Double_t m_emi[3] = {0.,0.,0.};
  Double_t d_hit[3] = {0.,0.,0.};
  Double_t xbin = 0.;
  Double_t ybin = 0.;
  Double_t theta = 0.;
  Double_t phi = 0.;
  Double_t rr[3] = {0.,0.,0.};
  Double_t theta_cc = 0.;
  Double_t m[4]={0.000511, 0.13957018, 0.493677, 0.938272};
  Double_t em[3][10000];
  Double_t vv = 0.;
  Int_t imom = 0;
  Double_t mom_check = 999.;
  Int_t event_check = 1.;
  Int_t event_check1 = 1.;
  Int_t nph_count = 0;
 
  //theta_cc = 0.02;
  
    //cout<<theta_cc<<endl;

  TH1F **ch_pi_h;
  ch_pi_h=new TH1F*[61];
  for(Int_t i=0;i<61;i++){
    ch_pi_h[i] = new TH1F(Form("ch_pi_h%d",i),"",10000,0,0.3);
  }
  TH1F *h_mom = new TH1F("","",100,0.,100.);
  

  for(Int_t i=0;(i<eic_rich->GetEntries());i++){
    
    eic_rich->GetEntry(i);

    if(eic_rich_event<=ind){
	  
      ph_count++;

      em[0][ph_count-1] = eic_rich_emi_x;
      em[1][ph_count-1] = eic_rich_emi_y;
      em[2][ph_count-1] = eic_rich_emi_z;

      //cout<<em[2][0]<<"  "<<em[2][11]<<endl;

      vv = sqrt((em[0][11]-em[0][0])*(em[0][11]-em[0][0])+(em[1][11]-em[1][0])*(em[1][11]-em[1][0])+(em[2][11]-em[2][0])*(em[2][11]-em[2][0]));
      
      if(ph_count>50){
	momvv_x[eic_rich_event-1]=(em[0][11]-em[0][0])/vv;
	momvv_y[eic_rich_event-1]=(em[1][11]-em[1][0])/vv;
	momvv_z[eic_rich_event-1]=(em[2][11]-em[2][0])/vv;
      }
      if(eic_rich_event!=event_check){
	ph_count=0;
	event_check = eic_rich_event;
	//cout<<event_check<<endl;
      }
    }
  }

  for(Int_t i=0;(i<eic_rich->GetEntries());i++){
    
    eic_rich->GetEntry(i);
    //if(eic_rich_event==1){
    //h->Fill(eic_rich_mpz);
    //}
    
    ang_cut = TMath::ACos(eic_rich_mpz/sqrt(eic_rich_mpx*eic_rich_mpx+eic_rich_mpy*eic_rich_mpy+eic_rich_mpz*eic_rich_mpz))*rtd;
    

    
    //if(eic_rich_event<=ind && rran.Uniform(0,100)<qe(eic_rich_edep*1000000000.)){
    if(eic_rich_event<=ind){

      momentum = sqrt(eic_rich_mpx*eic_rich_mpx+eic_rich_mpy*eic_rich_mpy+eic_rich_mpz*eic_rich_mpz);
      imom = momentum;
      if (imom == 0)
	continue;
      theta_cc = acos(sqrt(imom*imom+m[part]*m[part])/imom/1.00054);

      momv[0] = momvv_x[eic_rich_event-1];
      momv[1] = momvv_y[eic_rich_event-1];
      momv[2] = momvv_z[eic_rich_event-1];

      //if(eic_rich_event==1)cout<<momv[2]<<"  "<<eic_rich_event<<endl;
      
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

      m_emi[0] = ((220.)/momv[2])*momv[0];  //mean emission point
      m_emi[1] = ((220.)/momv[2])*momv[1];
      m_emi[2] = ((220.)/momv[2])*momv[2];

      /*m_emi[0] = eic_rich_emi_x; //real emission point
      m_emi[1] = eic_rich_emi_y;
      m_emi[2] = eic_rich_emi_z;*/
      
      d_hit[0] = eic_rich_hit_x;
      d_hit[1] = eic_rich_hit_y;
      d_hit[2] = eic_rich_hit_z;

      xbin = ph_det->GetXaxis()->FindBin(eic_rich_hit_x);
      ybin = ph_det->GetYaxis()->FindBin(eic_rich_hit_y);

      if(pix_lab!=0)d_hit[0] = ph_det->GetXaxis()->GetBinCenter(xbin);
      if(pix_lab!=0)d_hit[1] = ph_det->GetYaxis()->GetBinCenter(ybin);
      
      ph_det->Fill(d_hit[0],d_hit[1]);
      
      //cout<<theta*rtd<<"  "<<phi*rtd<<endl;
      //cout<<"V: "<<momv[0]<<"  "<<momv[1]<<"  "<<momv[2]<<endl;
      //cout<<"E: "<<m_emi[0]<<"  "<<m_emi[1]<<"  "<<m_emi[2]<<endl;
      //cout<<"D: "<<d_hit[0]<<"  "<<d_hit[1]<<"  "<<d_hit[2]<<endl;
      //cout<<eic_rich_volumeid<<endl;

      rr[0] = -18.5*TMath::Sin(eic_rich_volumeid*TMath::Pi()/4.);
      rr[1] = 18.5*TMath::Cos(eic_rich_volumeid*TMath::Pi()/4.);
      rr[2] = 75.;
      sec = eic_rich_volumeid;

      //cout<<rr[0]<<"  "<<rr[1]<<"  "<<rr[2]<<endl;

      ch = ind_ray(m_emi[0], m_emi[1], m_emi[2], d_hit[0], d_hit[1], d_hit[2], rr[0], rr[1], rr[2], momv[0], momv[1], momv[2]);
      //ch_v[ph_ch_count] = ch;
      ph_ch_count++;
      //cout<<ch<<endl;
      //if(ch<(theta_cc+0.01) && ch>(theta_cc-0.01))ch_ang->Fill(ch);
      
      //if(ch<(theta_cc+0.008) && ch>(theta_cc-0.008) && theta*rtd>15. && theta*rtd<16. && (ph_det->GetBinContent(xbin,ybin) == 1)){ //here are the cuts: central polar angle [12,13] (changed to [15,16]) and the cuts on theta_cc are about 5 sigma (assuming the measured 1 p.e. resulution) interval around the central value of theta_cc, the last cut takes just 1 p.e. per pixel  ####################################################
	//if(rran.Uniform(0,100)<qe(eic_rich_edep*1000000000.))ch_pi_h[imom]->Fill(ch);
      if(ch<(theta_cc+0.008) && ch>(theta_cc-0.008) && theta*rtd>15. && theta*rtd<16.){

	ch_pi_h[imom]->Fill(ch);
	spectrum_nm->Fill(1240./(eic_rich_edep*1000000000.));
	//nph_count++;
	ph_det1->Fill(d_hit[0],d_hit[1]);

	//if((eic_rich_event-1)==0)cout<<nph_v[0]<<endl;
	
	if(momentum!=mom_check){ 
	  h_mom->Fill(imom);
	  ev_count[imom]++;
	  //if(imom==14) cout<<eic_rich_mpid<<endl;
	  //if(imom==60)cout<<momentum<<"  "<<imom<<endl;
	  //nph_count++;
	  mom_check=momentum;
	}
      }
      //if(imom==14)cout<<ch<<"  "<<eic_rich_mpid<<"  "<<theta*rtd<<"  "<<phi*rtd<<endl;
      if(eic_rich_event!=event_check1){
	//if(eic_rich_event<100)cout<<event_check1<<endl;
	//cout<<ph_det_nosup->GetEntries()<<endl;
	ph_det->Reset();
	event_check1 = eic_rich_event;
	//nph_count=0;
    }
    }
    
  }
  
  //cout<<ph_count<<endl;
  
  //ch_ang->Draw();
  //ch_pi_h[32]->Draw();
  //spectrum_nm->Draw();
  //cout<<ev_count[9]<<endl;
  //h_mom->Draw();

  Int_t summm=0;

  ofstream out("pid_test.txt", ios::app);
  streambuf *coutbuf = cout.rdbuf(); 
  cout.rdbuf(out.rdbuf());
  
  for(Int_t i=0;i<61;i++){
    if(ch_pi_h[i]->GetEntries()!=0)cout<<ch_pi_h[i]->GetMean()<<"  "<<ch_pi_h[i]->GetRMS()<<"  "<<(int)(ch_pi_h[i]->GetEntries()/ev_count[i])<<"  "<<i<<"  "<<part<<endl;
    else if(i<60)cout<<ch_pi_h[i]->GetMean()<<"  "<<ch_pi_h[i]->GetRMS()<<"  "<<0<<"  "<<i<<"  "<<part<<endl;
    summm = summm + ev_count[i];
  }

  //cout<<nph_count<<endl;
  
  ph_det1->Draw("colz");
  ch_pi_h[45]->Draw();
  
  //c_mean = ch_ang->GetMean();
  //c_rms = ch_ang->GetRMS();

  //delete ch_ang;
}
  

void eic_bnl_rich::read(string input_filename){

  ifstream inputFile;
  inputFile.open(input_filename);
  if (!inputFile) {
    cerr << "Error in opening the file";
    exit(1);
  }

  Double_t theta_ch, sigma_theta, nph, momentum, ptype;
  Double_t ch_e_v[60],rms_e_v[60],nph_e_v[60],mom_e_v[60];
  Double_t ch_pi_v[60],rms_pi_v[60],nph_pi_v[60],mom_pi_v[60];
  Double_t ch_k_v[60],rms_k_v[60],nph_k_v[60],mom_k_v[60];
  Double_t ch_p_v[60],rms_p_v[60],nph_p_v[60],mom_p_v[60];
  Double_t n_sig_e_pi[60],n_sig_pi_k[60],n_sig_k_p[60],mm_e_pi[60],mm_pi_k[60],mm_k_p[60];
  Int_t idx[4] = {0,0,0,0};
  
  while (inputFile>>theta_ch>>sigma_theta>>nph>>momentum>>ptype){

    if(ptype==0){
      ch_e_v[idx[0]]=theta_ch;
      rms_e_v[idx[0]]=sigma_theta;
      nph_e_v[idx[0]]=nph*0.6;
      if(theta_ch==0) nph_e_v[idx[0]]=0;
      mom_e_v[idx[0]]=momentum;
      idx[0]++;
    }
    if(ptype==1){
      ch_pi_v[idx[1]]=theta_ch;
      rms_pi_v[idx[1]]=sigma_theta;
      nph_pi_v[idx[1]]=nph*0.6; //0.6 is a scale factor to normalize the MC value on expwerimantal data
      if(theta_ch==0) nph_pi_v[idx[1]]=0;
      mom_pi_v[idx[1]]=momentum;
      idx[1]++;
    }
    if(ptype==2){
      ch_k_v[idx[2]]=theta_ch;
      rms_k_v[idx[2]]=sigma_theta;
      nph_k_v[idx[2]]=nph*0.6;
      if(theta_ch==0) nph_k_v[idx[2]]=0;
      mom_k_v[idx[2]]=momentum;
      idx[2]++;
    }
    if(ptype==3){
      ch_p_v[idx[3]]=theta_ch;
      rms_p_v[idx[3]]=sigma_theta;
      nph_p_v[idx[3]]=nph*0.6;
      if(theta_ch==0) nph_p_v[idx[3]]=0;
      mom_p_v[idx[3]]=momentum;
      idx[3]++;
    }
    //cout<<theta_ch<<"  "<<sigma_theta<<"  "<<nph<<"  "<<momentum<<"  "<<ptype<<endl;
    
  }

  Int_t ii=0;
  Int_t iii=0;
  Int_t iiii=0;
  
  for(Int_t i=0;i<idx[0];i++){

    if(ch_e_v[i]!=0 && ch_pi_v[i]!=0 && i>2){
      n_sig_e_pi[ii] = (ch_e_v[i]-ch_pi_v[i])*sqrt((nph_e_v[i]+nph_pi_v[i])*0.5)/((rms_e_v[i]+rms_pi_v[i])*0.5);
      mm_e_pi[ii] = mom_e_v[i]+0.5;
      //cout<<"e/pi: "<<n_sig_e_pi[ii]<<"  "<<mm_e_pi[ii]<<endl;
      ii++;
    }
    if(ch_pi_v[i]!=0 && ch_k_v[i]!=0 && i>16){
      n_sig_pi_k[iii] = (ch_pi_v[i]-ch_k_v[i])*sqrt((nph_pi_v[i]+nph_k_v[i])*0.5)/((rms_pi_v[i]+rms_k_v[i])*0.5);
      mm_pi_k[iii] = mom_pi_v[i]+0.5;
      //cout<<ch_pi_v[i]<<"  "<<ch_k_v[i]<<endl;
      //cout<<"pi/k: "<<n_sig_pi_k[iii]<<"  "<<mm_pi_k[iii]<<endl;
      iii++;
    }
    if(ch_k_v[i]!=0 && ch_p_v[i]!=0 && i>30){
      n_sig_k_p[iiii] = (ch_k_v[i]-ch_p_v[i])*sqrt((nph_k_v[i]+nph_p_v[i])*0.5)/((rms_k_v[i]+rms_p_v[i])*0.5);
      mm_k_p[iiii] = mom_k_v[i]+0.5;
      //cout<<"k/p: "<<n_sig_k_p[iiii]<<"  "<<mm_k_p[iiii]<<endl;
      iiii++;
    }
    
    //cout<<ch_e_v[i]<<"  "<<nph_e_v[i]<<endl;
    
  }

  TGraph *sig_pi = new TGraph(idx[1],mom_pi_v,rms_pi_v);
  TGraph *nph_pi = new TGraph(idx[1],mom_pi_v,nph_pi_v);

  TGraph *gr_e_pi = new TGraph(ii,mm_e_pi,n_sig_e_pi);
  TGraph *gr_pi_k = new TGraph(iii,mm_pi_k,n_sig_pi_k);
  TGraph *gr_k_p = new TGraph(iiii,mm_k_p,n_sig_k_p);

  TCanvas *c1 = new TCanvas("c1","",1000,600);
  c1->Divide(1,1);
  c1->cd(1);
  gr_e_pi->SetLineStyle(2);
  gr_e_pi->SetLineWidth(2);
  gr_e_pi->SetLineColor(kGreen);
  gr_e_pi->SetMarkerColor(kGreen);
  gr_e_pi->SetTitle("");
  gr_e_pi->GetXaxis()->SetTitle("Momentum [Gev/c]");
  gr_e_pi->GetYaxis()->SetTitle("N_{#sigma}^{Ring}");
  gr_e_pi->Draw("AC*");
  gr_pi_k->SetLineStyle(3);
  gr_pi_k->SetLineWidth(2);
  gr_pi_k->SetLineColor(kRed);
  gr_pi_k->SetMarkerColor(kRed);
  gr_pi_k->SetTitle("");
  gr_pi_k->Draw("C*");
  gr_k_p->SetLineStyle(4);
  gr_k_p->SetLineWidth(2);
  gr_pi_k->SetLineColor(kBlue);
  gr_pi_k->SetMarkerColor(kBlue);
  gr_k_p->SetTitle("");
  gr_k_p->Draw("C*");
  c1->Print("test1.pdf","pdf");
  
  for(Int_t j=0;j<iiii;j++) cout<<n_sig_k_p[j]<<"  "<<mm_k_p[j]<<endl;

  TCanvas *c2 = new TCanvas("c2","",1000,600);
  c2->Divide(1,1);
  c2->cd(1);
  sig_pi->SetTitle("");
  sig_pi->GetXaxis()->SetTitle("Momentum [Gev/c]");
  sig_pi->GetYaxis()->SetTitle("#sigma_{#theta_{C}}");
  sig_pi->Draw("AP*");
  c2->Print("test2.pdf","pdf");

  TCanvas *c3 = new TCanvas("c3","",1000,600);
  c3->Divide(1,1);
  c3->cd(1);
  nph_pi->SetTitle("");
  nph_pi->GetXaxis()->SetTitle("Momentum [Gev/c]");
  nph_pi->GetYaxis()->SetTitle("N_{p.e.}");
  nph_pi->Draw("AP*");
  c3->Print("test3.pdf","pdf");
  
}
