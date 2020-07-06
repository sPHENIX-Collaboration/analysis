#include "TFile.h"
#include "TStyle.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TTreeReaderArray.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TLegend.h"


void draw_acts_eval()
{
  gStyle->SetOptStat(1);

  unsigned int minhits = 1;
  bool verbose1 = true;
  bool verbose2 = false;
  
  TFile *fin = TFile::Open("G4sPHENIX.root_g4svtx_eval.root_acts.root");
  double maxtracks = 100;
  TH1D *hitr[2];
  hitr[0] = new TH1D("hitr0","track number",10000,0,maxtracks);
  hitr[1] = new TH1D("hitr1","track number",10000,0,maxtracks);

  TH1D *hpt = new TH1D("hpt","truth_pt",100,0,20);
  TH1D *hpt_fit_seed = new TH1D("hpt_fit_seed","fit p_{T} - seed p_{T}",100,-1.0, 1.0);
  TH1D *hpt_fit_minus_truth = new TH1D("hpt_fit_minus_truth","fit p_{T} - truth pT",100,-10, 5.0);
  TH2D *hpt_seed_truth = new TH2D("hpt_seed_truth","seed/fit p_{T} vs truth p_{T}",100, 0, 20, 100, 0, 20);
  TH2D *hpt_fit_truth = new TH2D("hpt_fit_truth","fit p_{T} vs truth p_{T}",100,0,20, 100, 0, 20);

  TH2D *hxdiff_KF_truth_xlocal = new TH2D("hxdiff_KF_truth_xlocal","local KF x vs truth x",100, 0, 20, 100,-2.0,2.0);
  TH2D *hxdiff_KF_truth_ylocal = new TH2D("hxdiff_KF_truth_ylocal","local KF y vs truth y",100, 0, 20, 100,-2.0,2.0);
  TH2D *hxdiff_KF_truth_xglobal = new TH2D("hxdiff_KF_truth_xglobal","global KF x vs truth x",100, 0, 20, 100,-2.0,2.0);
  TH2D *hxdiff_KF_truth_yglobal = new TH2D("hxdiff_KF_truth_yglobal","global KF y vs truth y",100, 0, 20, 100,-2.0,2.0);
  TH2D *hxdiff_KF_truth_zglobal = new TH2D("hxdiff_KF_truth_zglobal","global KF z vs truth z",100, 0, 20, 100,-2.0,2.0);

  //hxdiff_KF_truth_xlocal->Fill(*t_pT, xdiff_local_last);

  TH1D *hxerr_KF = new TH1D("hxerr_KF"," err_eLOC0_flt", 100, -3, 3);
  TH1D *hxpull_KF = new TH1D("hxpull_KF"," pull_eLOC0_flt t outermost", 100, -50, 50);
  TH2D *hxpull_KF_radius = new TH2D("hxpull_KF_radius"," pull_eLOC0_flt vs radius", 800, 0, 800, 100, -100, 100);
  TH2D *hzpull_KF_radius = new TH2D("hzpull_KF_radius"," pull_eLOC1_flt vs radius", 800, 0, 800, 100, -100, 100);

  TH1D *hpt_smt = new TH1D("hpt_smt","state/truth p_{T}",100,0,20);
  TH2D *htxy = new TH2D("htxy","y vs x truth",4000,-800,800,4000,-800,800);
  TH2D *hxy_flt = new TH2D("hxy_flt","hxy_flt",4000,-800,800,4000,-800,800);
  TH2D *hxy_smt = new TH2D("hxy_smt","hxy_smt",4000,-800,800,4000,-800,800);
  TH2D *hxy_prt = new TH2D("hxy_prt","hxy_prt",4000,-800,800,4000,-800,800);

  TH1D *hdcaxy[2];
  TH1D *hdcaz[2];
  hdcaxy[0] = new TH1D("hdcaxy0","DCA xy", 100, -2.0, 2);
  hdcaxy[0]->GetXaxis()->SetTitle("DCA (mm)");
  hdcaxy[1] = new TH1D("hdcaxy1","DCA xy", 100, -2.0, 2);
  hdcaz[0] = new TH1D("hdcaz0","DCA z", 100, -2.0, 2.0);
  hdcaz[1] = new TH1D("hdcaz1","DCA z", 100, -2.0, 2.0);


  TH1D *hprotox[2];
  TH1D *hprotoy[2];
  TH1D *hprotoz[2];
  double range = 3.0;
  hprotox[0] = new TH1D("hprotox0","proto x - vertex x", 100, -range, range);
  hprotox[1] = new TH1D("hprotox1","proto x - vertex x", 100, -range, range);
  hprotoy[0] = new TH1D("hprotoy0","proto y - vertex y", 100, -range, range);
  hprotoy[1] = new TH1D("hprotoy1","proto y - vertex y", 100, -range, range);
  hprotoz[0] = new TH1D("hprotoz0","proto z - vertex z", 100, -range, range);
  hprotoz[1] = new TH1D("hprotoz1","proto z - vertex z", 100, -range, range);

  TH1D *hhit[5];
  hhit[0] = new TH1D("hhitx_global","global x hit - truth", 200, -0.6,0.6);
  hhit[1] = new TH1D("hhity_global","global y hit - truth", 200, -0.6,0.6);
  hhit[2] = new TH1D("hhitz_global","global z hit - truth", 200, -3,3);
  hhit[3] = new TH1D("hhitx_local","local x hit - truth", 200, -0.6,0.6);
  hhit[4] = new TH1D("hhity_local","local y hit - truth", 200, -3,3);

  TTree* theTree = nullptr;
  fin->GetObject("tracktree",theTree);
  //theTree->Print();

  TTreeReader theReader("tracktree",fin);

  TTreeReaderValue<int> event(theReader, "event_nr");

  // Truth
  TTreeReaderArray<float> t_x(theReader, "t_x");
  TTreeReaderArray<float> t_y(theReader, "t_y");
  TTreeReaderArray<float> t_z(theReader, "t_z");
  TTreeReaderValue<float> t_pT(theReader, "t_pT");
  TTreeReaderValue<float> t_vx(theReader, "t_vx");
  TTreeReaderValue<float> t_vy(theReader, "t_vy");
  TTreeReaderValue<float> t_vz(theReader, "t_vz");
  TTreeReaderArray<float> t_eLOC0(theReader, "t_eLOC0");
  TTreeReaderArray<float> t_eLOC1(theReader, "t_eLOC1");
  //TTreeReaderArray<float> layer(theReader, "layer_id");

  // proto track
  TTreeReaderValue<float> x_proto(theReader, "g_protoTrackX");
  TTreeReaderValue<float> y_proto(theReader, "g_protoTrackY");
  TTreeReaderValue<float> z_proto(theReader, "g_protoTrackZ");
  TTreeReaderValue<float> px_proto(theReader, "g_protoTrackPx");
  TTreeReaderValue<float> py_proto(theReader, "g_protoTrackPy");
  TTreeReaderValue<float> pz_proto(theReader, "g_protoTrackPz");

  // Predicted states 
  TTreeReaderArray<float> pT_prt(theReader, "pT_prt");
  TTreeReaderArray<float> g_x_prt(theReader, "g_x_prt");
  TTreeReaderArray<float> g_y_prt(theReader, "g_y_prt");
  TTreeReaderArray<float> g_z_prt(theReader, "g_z_prt");
  TTreeReaderArray<float> eLOC0_prt(theReader, "eLOC0_prt");
  TTreeReaderArray<float> eLOC1_prt(theReader, "eLOC1_prt");
  TTreeReaderArray<float> pull_eLOC0_prt(theReader, "pull_eLOC0_prt");
  TTreeReaderArray<float> pull_eLOC1_prt(theReader, "pull_eLOC1_prt");
  TTreeReaderArray<float> err_eLOC0_prt(theReader, "err_eLOC0_prt");
  TTreeReaderArray<float> err_eLOC1_prt(theReader, "err_eLOC1_prt");
  TTreeReaderArray<float> res_eLOC0_prt(theReader, "res_eLOC0_prt");
  TTreeReaderArray<float> res_eLOC1_prt(theReader, "res_eLOC1_prt");
  TTreeReaderArray<float> px_prt(theReader, "px_prt");
  TTreeReaderArray<float> py_prt(theReader, "py_prt");
  TTreeReaderArray<float> pz_prt(theReader, "pz_prt");

  // Filtered states
  TTreeReaderArray<float> g_x_flt(theReader, "g_x_flt");
  TTreeReaderArray<float> g_y_flt(theReader, "g_y_flt");
  TTreeReaderArray<float> g_z_flt(theReader, "g_z_flt");
  TTreeReaderArray<float> eLOC0_flt(theReader, "eLOC0_flt");
  TTreeReaderArray<float> eLOC1_flt(theReader, "eLOC1_flt");
  TTreeReaderArray<float> pull_eLOC0_flt(theReader, "pull_eLOC0_flt");
  TTreeReaderArray<float> pull_eLOC1_flt(theReader, "pull_eLOC1_flt");
  TTreeReaderArray<float> err_eLOC0_flt(theReader, "err_eLOC0_flt");
  TTreeReaderArray<float> err_eLOC1_flt(theReader, "err_eLOC1_flt");
  TTreeReaderArray<float> res_eLOC0_flt(theReader, "res_eLOC0_flt");
  TTreeReaderArray<float> res_eLOC1_flt(theReader, "res_eLOC1_flt");
  TTreeReaderArray<float> pT_flt(theReader, "pT_flt");
  TTreeReaderArray<float> px_flt(theReader, "px_flt");
  TTreeReaderArray<float> py_flt(theReader, "py_flt");
  TTreeReaderArray<float> pz_flt(theReader, "pz_flt");

  // Smoothed states
  TTreeReaderArray<float> g_x_smt(theReader, "g_x_smt");
  TTreeReaderArray<float> g_y_smt(theReader, "g_y_smt");
  TTreeReaderArray<float> g_z_smt(theReader, "g_z_smt");
  TTreeReaderArray<float> eLOC0_smt(theReader, "eLOC0_smt");
  TTreeReaderArray<float> eLOC1_smt(theReader, "eLOC1_smt");
  TTreeReaderArray<float> pull_eLOC0_smt(theReader, "pull_eLOC0_smt");
  TTreeReaderArray<float> pull_eLOC1_smt(theReader, "pull_eLOC1_smt");
  TTreeReaderArray<float> err_eLOC0_smt(theReader, "err_eLOC0_smt");
  TTreeReaderArray<float> err_eLOC1_smt(theReader, "err_eLOC1_smt");
  TTreeReaderArray<float> res_eLOC0_smt(theReader, "res_eLOC0_smt");
  TTreeReaderArray<float> res_eLOC1_smt(theReader, "res_eLOC1_smt");
  TTreeReaderArray<float> pT_smt(theReader, "pT_smt");

  // Fitted
  TTreeReaderValue<float> dcaxy_fit(theReader, "g_dca3Dxy_fit");
  TTreeReaderValue<float> dcaz_fit(theReader, "g_dca3Dz_fit");
  TTreeReaderValue<float> x_fit(theReader, "g_x_fit");
  TTreeReaderValue<float> y_fit(theReader, "g_y_fit");
  TTreeReaderValue<float> z_fit(theReader, "g_z_fit");
  TTreeReaderValue<float> px_fit(theReader, "g_px_fit");
  TTreeReaderValue<float> py_fit(theReader, "g_py_fit");
  TTreeReaderValue<float> pz_fit(theReader, "g_pz_fit");


  // Actual hits
  TTreeReaderArray<float> g_x_hit(theReader, "g_x_hit");
  TTreeReaderArray<float> g_y_hit(theReader, "g_y_hit");
  TTreeReaderArray<float> g_z_hit(theReader, "g_z_hit");
  TTreeReaderArray<float> l_x_hit(theReader, "l_x_hit");
  TTreeReaderArray<float> l_y_hit(theReader, "l_y_hit");
  TTreeReaderArray<float> err_x_hit(theReader, "err_x_hit");
  TTreeReaderArray<float> err_y_hit(theReader, "err_y_hit");
  TTreeReaderArray<float> res_x_hit(theReader, "res_x_hit");
  TTreeReaderArray<float> res_y_hit(theReader, "res_y_hit");
  TTreeReaderArray<float> pull_x_hit(theReader, "pull_x_hit");
  TTreeReaderArray<float> pull_y_hit(theReader, "pull_y_hit");

  int itr = 0;
   while(theReader.Next()){

     //if(*event == 2) continue;
     /*
     if((itr !=2)) 
       {
	 itr++;
	 continue;
       }
     */

     if(pT_flt.GetSize() < 1)
       {
	 cout << " pT_smt not found " << endl;
	 continue;
       }
     
     if(pT_smt.GetSize() < minhits) continue;
     
     double inner_radius = sqrt(pow(t_x[pT_flt.GetSize()-1], 2) + pow(t_y[pT_flt.GetSize()-1], 2));
     //if(inner_radius < 80)   continue;

     float pT_prt_avge = 0.0;
     float pT_flt_avge = 0.0;
     float pT_smt_avge = 0.0;
     float pT_smt_avge_wt = 0.0;

     float pT_fit =sqrt(pow(*px_fit,2) + pow(*py_fit,2));
     float pT_proto =sqrt(pow(*px_proto,2) + pow(*py_proto,2));

     double xdiff_global_last = g_x_flt[0] - t_x[0];
     double ydiff_global_last = g_y_flt[0] - t_y[0];
     double zdiff_global_last = g_z_flt[0] - t_z[0];
     double xdiff_local_last = eLOC0_flt[0] - t_eLOC0[0];
     double ydiff_local_last = eLOC1_flt[0] - t_eLOC1[0];

     if(verbose1)
       {
	 cout << " new track: " << itr << " truth Z vertex " <<  *t_vz  << " inner radius " << inner_radius <<  " nhits " << pT_flt.GetSize() << endl;   
	 cout << "    Truth pT " << *t_pT << " seed pT " << pT_prt[pT_flt.GetSize() - 1] <<  " KF pT " << pT_flt[0] << " SM pT " << pT_smt[0] 
	   //<< " xdiff_last " << xdiff_last << endl;
	      << endl;
	 cout << "           px_proto " << *px_proto << " py_proto " << *py_proto << " pz_proto " << *pz_proto << " pT_proto " << pT_proto << endl;
	 cout << "           px_prt " << px_prt[pT_prt.GetSize() - 1] << "  py_prt " << py_prt[pT_prt.GetSize() - 1]  << " pz_prt " << pz_prt[pT_prt.GetSize() - 1] 
	      << " pT_prt " << pT_prt[pT_prt.GetSize() - 1] << endl;
	 cout << "           px_flt " << px_flt[0] << "  py_flt " << py_flt[0]  << " pz_flt " << pz_flt[0] << " pT_flt " << pT_flt[0] << endl;
	 cout << "           px_fit " << *px_fit << "  py_fit " << *py_fit  << " pz_fit " << *pz_fit << " pT_fit " << pT_fit << endl;
	 cout << "           x_proto " << *x_proto << " y_proto " << *y_proto << " z_proto " << *z_proto << endl;
	 cout << "           x_fit   " << *x_fit << " y_fit   " << *y_fit << " z_fit   " << *z_fit << endl;
       }

     hpt_seed_truth->Fill(*t_pT, pT_fit);
     hpt_fit_truth->Fill(*t_pT, pT_fit);
     hpt_fit_seed->Fill(pT_fit -  pT_prt[pT_flt.GetSize() - 1]);
     hpt_fit_minus_truth->Fill(pT_fit -  *t_pT);
     hxdiff_KF_truth_xlocal->Fill(*t_pT, xdiff_local_last);
     hxdiff_KF_truth_ylocal->Fill(*t_pT, ydiff_local_last);
     hxdiff_KF_truth_xglobal->Fill(*t_pT, xdiff_global_last);
     hxdiff_KF_truth_yglobal->Fill(*t_pT, ydiff_global_last);
     hxdiff_KF_truth_zglobal->Fill(*t_pT, zdiff_global_last);
     hhit[0]->Fill(g_x_hit[0] - t_x[0]);
     hhit[1]->Fill(g_y_hit[0] - t_y[0]);
     hhit[2]->Fill(g_z_hit[0] - t_z[0]);
     hhit[3]->Fill(l_x_hit[0] - t_eLOC0[0]);
     hhit[4]->Fill(l_y_hit[0] - t_eLOC1[0]);

     hxerr_KF->Fill( err_eLOC0_flt[0]);
     hxpull_KF->Fill( pull_eLOC0_flt[0]);
    if(inner_radius < 80) 
      {
	hitr[0]->Fill(itr);
	hdcaxy[0]->Fill(*dcaxy_fit);
	hdcaz[0]->Fill(*dcaz_fit-*t_vz);
	
	hprotox[0]->Fill(*x_proto - *t_vx);
	hprotoy[0]->Fill(*y_proto - *t_vy);
	hprotoz[0]->Fill(*z_proto - *t_vz);
      }
    else
      {
	hitr[1]->Fill(itr);
	hdcaxy[1]->Fill(*dcaxy_fit);
	hdcaz[1]->Fill(*dcaz_fit-*t_vz);
	
	hprotox[1]->Fill(*x_proto - *t_vx);
	hprotoy[1]->Fill(*y_proto - *t_vy);
	hprotoz[1]->Fill(*z_proto - *t_vz);

      }

     for(unsigned int i = 0; i < pT_smt.GetSize(); ++i)
       {
	 pT_prt_avge += pT_prt[i];
	 pT_flt_avge += pT_flt[i];
	 pT_smt_avge += pT_smt[i];
	 pT_smt_avge_wt += 1.0;
	 
	 htxy->Fill(t_x[i], t_y[i]);
	 hxy_flt->Fill(g_x_flt[i], g_y_flt[i]);
	 hxy_smt->Fill(g_x_smt[i], g_y_smt[i]);
	 hxy_prt->Fill(g_x_prt[i], g_y_prt[i]);
	 
	 hpt_smt->Fill(pT_smt[i]);
	 hpt->Fill(*t_pT);

	 double radius = sqrt(pow(t_x[i], 2) + pow(t_y[i], 2));
	 hxpull_KF_radius->Fill(radius, pull_eLOC0_flt[i]);
	 hzpull_KF_radius->Fill( radius, pull_eLOC1_flt[i]);
	 //if(i == pT_smt.GetSize()-1)	 
	   if(verbose2)
	   {    
	     // Identify the hit
	     cout << " hit " << i << " rad " << radius
		  << " pull " << pull_eLOC0_flt[i]
		  << endl;
	     cout << "    pT_prt " << pT_prt[i] << " pT_flt " << pT_flt[i] << endl;
	     
	     // global hit positions
	     cout << "    truth global hit : "  << "t_x     " << t_x[i] << " t_y     " << t_y[i] << " t_z      " << t_z[i]  << endl;
	     cout << "    Global hit:        g_x_hit " << g_x_hit[i] << " g_y_hit " << g_y_hit[i]  << " g_z_hit " << g_z_hit[i]  
		  << "    dx " << g_x_hit[i] - t_x[i] 
		  << " dy " << g_y_hit[i] - t_y[i]
		  << " dz " << g_z_hit[i] - t_z[i]
		  << endl;	 
	     cout << "    Global predicted:  g_x_prt " << g_x_prt[i] << " g_y_prt " << g_y_prt[i] << " g_z_prt " << g_z_prt[i]
		  << "     dx " << g_x_prt[i] - t_x[i] 
		  << " dy " << g_y_prt[i] - t_y[i]
		  << " dz " << g_z_prt[i] - t_z[i]
		  << endl;
	     cout << "    Global filter:     g_x_flt " << g_x_flt[i] << " g_y_flt " << g_y_flt[i]  << " g_z_flt " << g_z_flt[i]  
		  << "     dx " << g_x_flt[i] - t_x[i] 
		  << " dy " << g_y_flt[i] - t_y[i]
		  << " dy " << g_z_flt[i] - t_z[i]
		  << endl;
	     
	     // local hit positions
	     cout << "    X:      local truth :       t_eLOC0 " << t_eLOC0[i] << endl;  
	     
	     cout << "            Local hit:          l_x_hit " << l_x_hit[i] << "       err_x_hit " << err_x_hit[i] << "    res_x_hit " << res_x_hit[i] << " pull_x_hit " << pull_x_hit[i] << endl; 
	     cout << "            Local predicted:  elOC0_prt " << eLOC0_prt[i] << " err_eLOC0_prt " << err_eLOC0_prt[i] << " res_eLOC0_prt " << res_eLOC0_prt[i] << " pull_eLOC0_prt " << pull_eLOC0_prt[i] << endl;
	     cout << "            Local filter:     elOC0_flt " << eLOC0_flt[i] << " err_eLOC0_flt " << err_eLOC0_flt[i] << " res_eLOC0_flt " << res_eLOC0_flt[i] << " pull_eLOC0_flt " << pull_eLOC0_flt[i] << endl;
	     
	     
	     cout << "    Y:     local truth :        t_eLOC1 " << t_eLOC1[i] << endl;  
	     cout << "            Local hit:          l_y_hit " << l_y_hit[i] <<  " err_y_hit " << err_y_hit[i] << " res_y_hit " << res_y_hit[i] << " pull_y_hit " << pull_y_hit[i] << endl; 
	     cout << "            Local predicted:  eLOC1_prt " << eLOC1_prt[i] << " err_eLOC1_prt " << err_eLOC1_prt[i] << " res_eLOC1_prt " << res_eLOC1_prt[i] << " pull_eLOC1_prt " << pull_eLOC1_prt[i] << endl;
	     cout << "            Local filter:     eLOC1_flt " << eLOC1_flt[i] << " err_eLOC1_flt " << err_eLOC1_flt[i] << " res_eLOC1_smt " << res_eLOC1_flt[i] << " pull_eLOC1_flt " << pull_eLOC1_flt[i] << endl;
	     
	     /*
	     // Smoothed track projuections
	     cout << "    Global smoothed: g_x_smt " << g_x_smt[i] << " g_y_smt " << g_y_smt[i] << " g_z_smt " << g_z_smt[i]
	     << " dx " << g_x_smt[i] - t_x[i] 
	     << " dy " << g_y_smt[i] - t_y[i]
	     << " dz " << g_z_smt[i] - t_z[i]
	     << endl;
	     cout << "    Local smoothed: elOC0_smt " << eLOC0_smt[i] << " eLOC1_smt " << eLOC1_smt[i] << endl;
	     cout << "                  Local smoothed: err_eLOC0_smt " << err_eLOC0_smt[i] << " res_eLOC0_smt " << res_eLOC0_smt[i] << " pull_eLOC0_smt " << pull_eLOC0_smt[i] << endl;
	     cout << "                  Local smoothed: err_eLOC1_smt " << err_eLOC1_smt[i] << " res_eLOC1_smt " << res_eLOC1_smt[i] << " pull_eLOC1_smt " << pull_eLOC1_smt[i] << endl;
	     */
	   }
       }
     pT_smt_avge /= pT_smt_avge_wt;
     pT_flt_avge /= pT_smt_avge_wt;
     pT_prt_avge /= pT_smt_avge_wt;
          
     // track info
     if(verbose2)
       {
	 cout << " track " << itr 
	   //<< " vertex truth x " << *t_vx << " vertex truth y " << *t_vy 
	      << " truth pT = " << *t_pT 
	      << "  pT_prt_avge = " << pT_prt_avge 
	      << "  pT_flt_avge = " << pT_flt_avge 
	      << "  pT_smt_avge = " << pT_smt_avge 
	      << "  pT_smt size = " << pT_smt.GetSize() 
	      << endl;

	 cout << " dcaxy_fit " << *dcaxy_fit << " dcaz_fit " << *dcaz_fit << endl;
       }

   
     itr++;
   }
   
   // track vertex position

   TCanvas *c = new TCanvas("c","c",5,5,800,800);
   
  TH2D *hvxy = new TH2D("hvxy","y vs x track vertex",4000,-800,800,4000,-800,800);
  hvxy->Fill(*t_vx, *t_vy);


   // now get the truth clusters and plot those too
  TChain* ntp_g4cluster = new TChain("ntp_g4cluster","truth clusters");
  ntp_g4cluster->Add("G4sPHENIX.root_g4svtx_eval.root");

  TH2D *hclusxy = new TH2D("hclusxy","y vs x (magenta=all-hits, black=acts-hits,  red=prt, green=flt)",4000,-800,800,4000,-800,800);
  ntp_g4cluster->Draw("(10.0*gy):(10.0)*gx>>hclusxy");

   hpt_smt->Draw();
   hpt->SetLineColor(kRed);
   hpt->Draw("same");
   
   TCanvas *ch= new TCanvas("ch","ch",5,5,800,800);
   ch->SetLeftMargin(0.15);

   hclusxy->SetMarkerStyle(20);
   hclusxy->SetMarkerSize(0.5);
   hclusxy->SetMarkerColor(kMagenta);
   hclusxy->GetYaxis()->SetTitle("y (mm)");
   hclusxy->GetXaxis()->SetTitle("x (mm)");
   hclusxy->Draw();

   hvxy->SetMarkerStyle(20);
   hvxy->SetMarkerSize(1.0);
   hvxy->SetMarkerColor(kCyan);
   hvxy->Draw("same");
      
   htxy->SetMarkerStyle(20);
   htxy->SetMarkerSize(0.5);
   htxy->Draw("same");
   hxy_prt->SetMarkerColor(kRed);
   hxy_prt->SetMarkerStyle(20);
   hxy_prt->SetMarkerSize(0.5);
   hxy_prt->Draw("same");
   hxy_flt->SetMarkerColor(kGreen+2);
   hxy_flt->SetMarkerStyle(20);
   hxy_flt->SetMarkerSize(0.5);
   hxy_flt->Draw("same");
   hxy_smt->SetMarkerColor(kBlue);
   hxy_smt->SetMarkerStyle(20);
   hxy_smt->SetMarkerSize(0.5);
   //hxy_smt->Draw("same");

   // 
   TCanvas *ctruth = new TCanvas("ctruth","ctruth",5,5,1500,800);
   ctruth->Divide(3,1);

   ctruth->cd(1);
   gPad->SetLeftMargin(0.15);
   hpt_seed_truth->GetXaxis()->SetTitle("Truth p_{T}");
   hpt_seed_truth->GetYaxis()->SetTitle("Seed or fit p_{T}");
   hpt_seed_truth->SetMarkerStyle(20);
   hpt_seed_truth->SetMarkerSize(0.4);
   hpt_seed_truth->GetYaxis()->SetTitleSize(0.05);
   hpt_seed_truth->GetXaxis()->SetTitleSize(0.05);
   hpt_seed_truth->GetYaxis()->SetLabelSize(0.05);
   hpt_seed_truth->GetXaxis()->SetLabelSize(0.05);
   hpt_seed_truth->GetXaxis()->SetNdivisions(505);
   hpt_seed_truth->Draw();

   TLegend *trleg = new TLegend(0.2, 0.75, 0.55, 0.85, "", "NDC");
   trleg->AddEntry(hpt_seed_truth,"seed", "p");
   trleg->AddEntry(hpt_fit_truth, "fit ","p");
   trleg->SetTextSize(0.05);
   trleg->Draw();

   hpt_fit_truth->SetMarkerStyle(20);
   hpt_fit_truth->SetMarkerSize(0.4);
   hpt_fit_truth->SetMarkerColor(kRed);
   hpt_fit_truth->Draw("same");

   ctruth->cd(2);
   hpt_fit_seed->GetXaxis()->SetTitle("fit p_{T} - seed p_{T}");
   hpt_fit_seed->SetTitleSize(0.05);
   hpt_fit_seed->GetYaxis()->SetLabelSize(0.05);
   hpt_fit_seed->GetXaxis()->SetLabelSize(0.05);
   hpt_fit_seed->GetXaxis()->SetNdivisions(505);
   hpt_fit_seed->Draw();

   TLegend *tkleg = new TLegend(0.15, 0.79, 0.65, 0.85, "", "NDC");
   tkleg->AddEntry(hpt_fit_seed, "fit - seed","l");
   tkleg->SetTextSize(0.05);
   tkleg->Draw();

   ctruth->cd(3);
   hpt_fit_minus_truth->SetMarkerStyle(20);
   hpt_fit_minus_truth->SetMarkerSize(0.2);
   hpt_fit_minus_truth->SetLineColor(kRed);
   hpt_fit_minus_truth->GetXaxis()->SetTitle("fit p_{T} - truth p_{T}");
   hpt_fit_minus_truth->GetYaxis()->SetTitleSize(0.05);
   hpt_fit_minus_truth->GetXaxis()->SetTitleSize(0.05);
   hpt_fit_minus_truth->GetYaxis()->SetLabelSize(0.05);
   hpt_fit_minus_truth->GetXaxis()->SetLabelSize(0.05);
   hpt_fit_minus_truth->GetXaxis()->SetNdivisions(505);
   hpt_fit_minus_truth->Draw();

   TLegend *tfleg = new TLegend(0.15, 0.79, 0.65, 0.85, "", "NDC");
   tfleg->AddEntry(hpt_fit_minus_truth, "fit - truth","l");
   tfleg->SetTextSize(0.05);
   tfleg->Draw();

   TCanvas *cdiff = new TCanvas("cdiff","cdiff",5,5,1600,800);
   cdiff->Divide(3,2);
   TH1D *hxdiff[5];


   hxdiff[0] = hxdiff_KF_truth_xglobal->ProjectionY();
   hxdiff[0]->SetTitle("global KF filt x - truth x");
   hxdiff[0]->GetXaxis()->SetTitle("global KF filt x - truth x (mm)");

   hxdiff[1] = hxdiff_KF_truth_yglobal->ProjectionY();
   hxdiff[1]->SetTitle("global KF filt y - truth y");
   hxdiff[1]->GetXaxis()->SetTitle("global KF filt y - truth y (mm)");

   hxdiff[2] = hxdiff_KF_truth_zglobal->ProjectionY();
   hxdiff[2]->SetTitle("global KF filt z - truth z");
   hxdiff[2]->GetXaxis()->SetTitle("global KF filt z - truth z (mm)");

   hxdiff[3] = hxdiff_KF_truth_xlocal->ProjectionY();
   hxdiff[3]->SetTitle("local KF filt x - truth x");
   hxdiff[3]->GetXaxis()->SetTitle("local KF filt x - truth x (mm)");

   hxdiff[4] = hxdiff_KF_truth_ylocal->ProjectionY();
   hxdiff[4]->SetTitle("local KF filt y - truth y");
   hxdiff[4]->GetXaxis()->SetTitle("local KF filt y - truth y (mm)");


   for(int i=0;i<5;++i)
     {
       cdiff->cd(i+1);

       hxdiff[i]->SetTitleSize(0.05);
       hxdiff[i]->GetYaxis()->SetTitleSize(0.05);
       hxdiff[i]->GetXaxis()->SetTitleSize(0.05);
       hxdiff[i]->GetYaxis()->SetLabelSize(0.05);
       hxdiff[i]->GetXaxis()->SetLabelSize(0.05);
       hxdiff[i]->GetXaxis()->SetNdivisions(505);
       hxdiff[i]->DrawCopy();
       cout << " hxdiff " << i << " RMS =  " << hxdiff[i]->GetRMS() << endl;
     } 


   TLegend *txleg = new TLegend(0.15, 0.75, 0.85, 0.85, "", "NDC");
   txleg->AddEntry(hxdiff[0],"KF  - truth  outer", "l");
   //txleg->AddEntry(hxerr_KF, "KF  error outer","l");
   txleg->SetTextSize(0.05);
   //   txleg->Draw();

   TCanvas *chit = new TCanvas("chit","chit",5,5,1600,800);
   chit->Divide(3,2);

   for(int i=0;i<5;++i)
     {
       chit->cd(i+1);
       hhit[i]->DrawCopy();

     }

   TCanvas *cpull = new TCanvas("cpull","local X Pull",5,5,1600,800);
   cpull->Divide(3,1);
   cpull->cd(1);
   hxpull_KF->Draw();
   cpull->cd(2);
   hxpull_KF_radius->Draw();
   cpull->cd(3);
   hzpull_KF_radius->Draw();


   TCanvas *cdca = new TCanvas("cdca","DCA",5,5,1200,800);
   cdca->Divide(2,1);

   TLegend *dcaleg = new TLegend(0.15, 0.75, 0.45, 0.85, "", "NDC");
   dcaleg->AddEntry(hdcaxy[0],"MVTX match", "l");
   dcaleg->AddEntry(hdcaxy[1],"no MVTX match", "l");
   dcaleg->SetTextSize(0.03);

   cdca->cd(1);
   hdcaxy[0]->Draw();
   hdcaxy[1]->SetLineColor(kRed);
   hdcaxy[1]->Draw("same");
   dcaleg->Draw();
   cdca->cd(2);
   hdcaz[0]->Draw();
   hdcaz[1]->SetLineColor(kRed);
   hdcaz[1]->Draw("same");
   dcaleg->Draw();

   TCanvas *cproto = new TCanvas("cproto","proto pos - truth pos",5,5,1200,800);
   cproto->Divide(3,1);
   cproto->cd(1);
   hprotox[0]->GetXaxis()->SetTitle("mm");
   hprotox[0]->Draw();
   hprotox[1]->SetLineColor(kRed);
   hprotox[1]->Draw("same");
   dcaleg->Draw();

   cproto->cd(2);
   hprotoy[0]->GetXaxis()->SetTitle("mm");
   hprotoy[0]->Draw();
   hprotoy[1]->SetLineColor(kRed);
   hprotoy[1]->Draw("same");
   dcaleg->Draw();

   cproto->cd(3);
   hprotoz[0]->GetXaxis()->SetTitle("mm");
   hprotoz[0]->Draw();
   hprotoz[1]->SetLineColor(kRed);
   hprotoz[1]->Draw("same");
   dcaleg->Draw();

   TCanvas *citr = new TCanvas("citr","track number",5,5,1200,800);
   hitr[0]->Draw();
   hitr[1]->SetLineColor(kRed);
   hitr[1]->Draw("same");

}
