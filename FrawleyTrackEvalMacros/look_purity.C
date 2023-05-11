#include <TH2D.h>
#include <TFile.h>
#include <TF1.h>
#include <TGraph.h>
#include  <TCanvas.h>
#include  <TColor.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TLine.h>
#include <TEfficiency.h>
#include <TGraphAsymmErrors.h>

#include "sPhenixStyle.C"

void look_purity()
{
  SetsPhenixStyle();
  /*
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);
  gStyle->SetOptTitle(0);
  */

  //gStyle->SetOptStat(1);

  // determines if plots are labeled "HIJING"
  bool hijing = false;

  //int n_maps_layer = 3;
  int n_maps_layer = 0;
  //int n_intt_layer = 4;
  int n_intt_layer = 0;
  int n_tpc_layer = 48;
    
  double ptmax = 40.0;
  double hptmax = 40.0;
  //double ptstep = 0.25;
  //double ptstep = 1.0;
  //double ptstep = 2.0;
  //double ptstep = 0.5;
  double ptstep = 0.4;

  // set to false only to generate pT resolution plots without fits
  // BEWARE: false means that the 4 sigma cuts are meaningless - they are not done with fitted parameters
  bool pt_resolution_fit = true;
  //bool pt_resolution_fit = false;
  
  TFile *fin = new TFile("root_files/purity_out.root"); 
  //TFile *fin = new TFile("root_files/purity_out_100pions_noINTT_primvert.root"); 


  if(!fin)
    {
      cout << "Failed to find input file" << endl;
      exit(1);
    }

  TCanvas *c1 = new TCanvas("c1","c1",5,5,900,500);
  c1->Divide(3,1);

  //==========================
  // 2D histo for embedded pions
  TH2D *hpt_dca2d = 0;
  fin->GetObject("hpt_dca2d",hpt_dca2d);
  c1->cd(1);
  gPad->SetLeftMargin(0.12);
  hpt_dca2d->GetYaxis()->SetTitleOffset(1.5);
  hpt_dca2d->SetMarkerStyle(20);
  hpt_dca2d->SetMarkerSize(0.3);
  hpt_dca2d->Draw();

  // 2D histo for embedded pions
  TH2D *hpt_dcaZ = 0;
  fin->GetObject("hpt_dcaZ",hpt_dcaZ);
  c1->cd(2);
  gPad->SetLeftMargin(0.12);
  hpt_dcaZ->SetMarkerStyle(20);
  hpt_dcaZ->SetMarkerSize(0.3);
  hpt_dcaZ->GetYaxis()->SetTitleOffset(1.5);
  hpt_dcaZ->Draw();

  TH2D *hpt_compare = 0;
  fin->GetObject("hpt_compare",hpt_compare);
  c1->cd(3);
  gPad->SetLeftMargin(0.12);
  hpt_compare->GetYaxis()->SetTitleOffset(1.5);
  hpt_compare->SetMarkerStyle(20);
  hpt_compare->SetMarkerSize(0.3);
  hpt_compare->Draw();
  
  //========================================
  // extract DCA resolution vs pT from 2D histo hpt_dca2d
  // by fitting NPT pT slices

  TCanvas *c2 = new TCanvas("c2","c2",20,20,800,600);

  int NPT = (int) (ptmax/ptstep);

  double pT[200];
  double dca2d[200];
  for(int i = 0;i<NPT;i++)
    {
      // divide pT range into bins of width ptstep GeV/c at 0.25, 0.75, 1.25, .....
      // bins will be (e.g.)from 0-0.5, 0.5-1.0, 1.0-1.5, .....
      double ptlo = (double) i * ptstep + 0.0;
      double pthi = (double) i * ptstep + ptstep;

      int binlo = hpt_dca2d->GetXaxis()->FindBin(ptlo);
      int binhi = hpt_dca2d->GetXaxis()->FindBin(pthi);

      std::cout << "ptlo " << ptlo << " binlo " << binlo << " pthi " << pthi << " binhi " << binhi << std::endl;

      TH1D *h = new TH1D("h","dca2d resolution",2000, -0.1, 0.1);
      hpt_dca2d->ProjectionY("h",binlo,binhi);
      h->GetXaxis()->SetTitle("p_{T} (GeV/c)");
      h->GetXaxis()->SetTitle("#Delta dca2d (cm)");
      h->GetXaxis()->SetTitleOffset(1.0);
      //if(i<8) h->Rebin(4);
      h->DrawCopy();

      double mean = h->GetMean();
      double sigma = h->GetRMS();
      double yield = h->Integral();
  
     pT[i] = (ptlo + pthi) / 2.0;
    
      double low = -0.01, high=0.01;
      if(n_maps_layer == 0)
	{
	  low = 3.0 * low;
	  high = 3.0 * high;
	} 
      if(pT[i] < 6.0)
	{
	  low = 3.0*low;
	  high = 3.0*high;
	}

      TF1 *f = new TF1("f","gaus",low, high);
      f->SetParameter(1, yield/100.0);
      f->SetParameter(2, 0.0);
      f->SetParameter(3,0.002);
      h->Fit(f,"R");

 

      dca2d[i] = f->GetParameter(2);
      cout << " pT " << pT[i] << " dca2d " << dca2d[i] << " counts " << h->Integral() << " hist mean " << h->GetMean() << " hist RMS " << h->GetRMS() << endl;
    }

  //============================================
  // plot the dca2d resolution extracted above for embedded pions

  TCanvas *c3 = new TCanvas("c3","c3",100,100,800,600);
  TGraph *grdca2d = new TGraph(NPT,pT,dca2d);
  grdca2d->SetMarkerStyle(20);
  grdca2d->SetMarkerSize(1.2);
  grdca2d->SetMarkerColor(kRed);
  grdca2d->SetName("dca2d_resolution");
  grdca2d->SetTitle("dca2d resolution");

  TH1D *hdummy = new TH1D("hdummy","#Delta dca2d vs p_{T}",100,0.0,hptmax);
  hdummy->SetMinimum(0);
  if(n_maps_layer == 0)
    hdummy->SetMaximum(0.015);
  else
    hdummy->SetMaximum(0.0051);
  hdummy->GetXaxis()->SetTitle("p_{T} (GeV/c)");
  hdummy->GetYaxis()->SetTitle("DCA(r#phi) resolution (cm)");
  hdummy->GetYaxis()->SetTitleOffset(1.5);
  gPad->SetLeftMargin(0.15);
  hdummy->Draw();
  grdca2d->Draw("p");

  char dcalab[500];
  if(hijing)
    //sprintf(dcalab, "sPHENIX   HIJING   b=0-4 fm");
    sprintf(dcalab, "sPHENIX   HIJING   b=0-12 fm, 100 kHz pileup");
  else
  sprintf(dcalab, "sPHENIX  100 pions");
  TLegend *ldca = new TLegend(0.3, 0.75, 1.05, 0.90,dcalab,"NDC");
  ldca->SetBorderSize(0);
  ldca->SetFillColor(0);
  ldca->SetFillStyle(0);
  char lstr1[500];
  //sprintf(lstr1,"MVTX+INTT+TPC",n_maps_layer,n_intt_layer,n_tpc_layer);
  sprintf(lstr1,"INTT+TPC",n_maps_layer,n_intt_layer,n_tpc_layer);
  ldca->AddEntry(grdca2d, lstr1, "p");
  ldca->Draw();

  //===================================

  //========================================
  // extract DCA Z resolution vs pT from 2D histo hpt_dcaZ
  // by fitting NPT pT slices

  TCanvas *c2a = new TCanvas("c2a","c2a",20,20,800,600);

  double dcaZ[200];
  for(int i = 0;i<NPT;i++)
    {
      // divide pT range into bins of width 0.5 GeV/c at 0.25, 0.75, 1.25, .....
      // bins will be from 0-0.5, 0.5-1.0, 1.0-1.5, .....
      double ptlo = (double) i * ptstep + 0.0;
      double pthi = (double) i * ptstep + ptstep;

      int binlo = hpt_dcaZ->GetXaxis()->FindBin(ptlo);
      int binhi = hpt_dcaZ->GetXaxis()->FindBin(pthi);

      std::cout << "ptlo " << ptlo << " binlo " << binlo << " pthi " << pthi << " binhi " << binhi << std::endl;
      TH1D *h = new TH1D("h","DCA (Z) resolution (cm)",2000, -0.1, 0.1);
      hpt_dcaZ->ProjectionY("h",binlo,binhi);
      h->GetXaxis()->SetTitle("p_{T} (GeV/c)");
      h->GetYaxis()->SetTitle("DCA(Z) resolution (cm)");
      h->GetYaxis()->SetTitleOffset(1.0);
      //if(i<8) h->Rebin(4);
      h->DrawCopy();

      double mean = h->GetMean();
      double sigma = h->GetRMS();
      double yield = h->Integral();
  
     pT[i] = (ptlo + pthi) / 2.0;
    
      double low = -0.01, high=0.01;
      if(n_maps_layer == 0)
	{
	  low = 3.0 * low;
	  high = 3.0 * high;
	}
  
      if(pT[i] < 6.0)
	{
	  low = 3.0*low;
	  high = 3.0*high;
	}

      TF1 *f = new TF1("f","gaus",low, high);
      f->SetParameter(1, yield/100.0);
      f->SetParameter(2, 0.0);
      f->SetParameter(3,0.002);
      h->Fit(f,"R"); 

      dcaZ[i] = f->GetParameter(2);
      cout << " pT " << pT[i] << " dcaZ " << dcaZ[i] << " counts " << h->Integral() << " hist mean " << h->GetMean() << " hist RMS " << h->GetRMS() << endl;
    }

  //============================================
  // plot the dcaZ resolution extracted above for embedded pions

  TCanvas *c3a = new TCanvas("c3a","c3a",100,100,800,600);
  TGraph *grdcaZ = new TGraph(NPT,pT,dcaZ);
  grdcaZ->SetMarkerStyle(20);
  grdcaZ->SetMarkerSize(1.2);
  grdcaZ->SetMarkerColor(kRed);
  grdcaZ->SetName("dcaZ_resolution");
  grdcaZ->SetTitle("dcaZ resolution");

  TH1D *hdummya = new TH1D("hdummya","#Delta dcaZ vs p_{T}",100,0.0,hptmax);
  hdummya->SetMinimum(0);

  if(n_maps_layer == 0)  
    hdummya->SetMaximum(0.051);
  else
    hdummya->SetMaximum(0.0051);
  hdummya->GetXaxis()->SetTitle("p_{T} (GeV/c)");
  hdummya->GetYaxis()->SetTitle("DCA(Z) resolution  (cm)");
  hdummya->GetYaxis()->SetTitleOffset(1.5);
  gPad->SetLeftMargin(0.15);
  hdummya->Draw();
  grdcaZ->Draw("p");

  TLegend *ldcaZ = new TLegend(0.3, 0.75, 1.05, 0.90, dcalab,"NDC");
  ldcaZ->SetBorderSize(0);
  ldcaZ->SetFillColor(0);
  ldcaZ->SetFillStyle(0);
  //char lstr1[500];
  //sprintf(lstr1,"MVTX+INTT+TPC",n_maps_layer,n_intt_layer,n_tpc_layer);
  sprintf(lstr1,"INTT+TPC",n_maps_layer,n_intt_layer,n_tpc_layer);
  ldcaZ->AddEntry(grdcaZ, lstr1, "p");
  ldcaZ->Draw();

  //===================================


  // extract pT resolution vs pT from hpt_compare

  TCanvas *c4 = new TCanvas("c4","c4",60,60,800,600);

  double dpT[200];

  dpT[0] = 1.0; // throw it away, stats are too poor
  for(int i = 1;i<NPT;i++)
    //for(int i = 0;i<1;i++)
    {
      // divide pT range into bins of width 0.5 GeV/c at 0.25, 0.75, 1.25, .....
      // bins will be from 0-0.5, 0.5-1.0, 1.0-1.5, .....
      double ptlo = (double) i * ptstep + 0.0;
      double pthi = (double) i * ptstep + ptstep;

      int binlo = hpt_compare->GetXaxis()->FindBin(ptlo);
      int binhi = hpt_compare->GetXaxis()->FindBin(pthi);

      TH1D *hpt = new TH1D("hpt","pT resolution ",200, 0, 2.0);    
      hpt_compare->ProjectionY("hpt",binlo,binhi);
      hpt->GetXaxis()->SetTitle("#Delta p_{T}/p_{T}");
      hpt->GetXaxis()->SetTitleOffset(1.0);
      if(i>30) hpt->Rebin(4);
      hpt->DrawCopy();

      std::cout << "ptlo " << ptlo << " binlo " << binlo << " pthi " << pthi << " binhi " << binhi << " integral " << hpt->Integral() << std::endl;

      TF1 *f = new TF1("f","gaus",0.8,1.2);
      hpt->Fit(f,"R");

      pT[i] = (ptlo + pthi) / 2.0;

      dpT[i] = f->GetParameter(2);
      cout << " pT " << pT[i] << " dpT " << dpT[i] << " integral " << hpt->Integral() << std::endl;
    }

  //==========================================
  // Plot pT resolution extracted above for embedded pions

  TCanvas *c5 = new TCanvas("c5","c5",100,100,800,800);
  c5->SetLeftMargin(0.14);
  TGraph *grdpt = new TGraph(NPT,pT,dpT);
  grdpt->SetMarkerStyle(20);
  grdpt->SetMarkerSize(1.1);
  grdpt->SetName("pt_resolution");
  grdpt->SetTitle("pT resolution");

  TH1D *hdummy2 = new TH1D("hdummy2","#Delta p_{T} vs p_{T}",100,0.0,hptmax);
  hdummy2->SetMinimum(0);
  //hdummy2->SetMaximum(0.05);
  //hdummy2->SetMaximum(0.12);
  hdummy2->SetMaximum(0.3);
  hdummy2->GetXaxis()->SetTitle("p_{T}");
  hdummy2->GetYaxis()->SetTitle("#Delta p_{T}/p_{T}");
  hdummy2->GetYaxis()->SetTitleOffset(1.5);
  hdummy2->Draw();
  grdpt->Draw("p");

 TLegend *ldpt= new TLegend(0.25, 0.75, 0.95, 0.90, dcalab, "NDC");
  ldpt->SetBorderSize(0);
  ldpt->SetFillColor(0);
  ldpt->SetFillStyle(0);
  //char lstr1[500];
  //sprintf(lstr1,"MVTX+INTT+TPC",n_maps_layer,n_intt_layer,n_tpc_layer);
  sprintf(lstr1,"INTT+TPC",n_maps_layer,n_intt_layer,n_tpc_layer);
  ldpt->AddEntry(grdpt, lstr1, "p");
  ldpt->Draw();

  // Parameterize pT resolution
  
  //TF1 *fpt = new TF1("fpt","sqrt([0]*[0] + [1]*[1]*x*x)", 0, 35.0);
  TF1 *fpt = new TF1("fpt","[0] + [1]*x", 2.0, hptmax);
  fpt->SetParameter(0,0.005);
  //fpt->FixParameter(0,0.005);
  fpt->SetParameter(1,0.0015);
  if(pt_resolution_fit)  
    grdpt->Fit(fpt,"R");

  char lab[1000];
  //sprintf(lab,"#frac{#Deltap_{T}}{p_{T}} = #sqrt{%.4f^{2} + (%.6f #times p_{T})^{2}}", fpt->GetParameter(0), fpt->GetParameter(1))
  sprintf(lab,"#frac{#Deltap_{T}}{p_{T}} = %.4f + %.6f #times p_{T}", fpt->GetParameter(0), fpt->GetParameter(1));
  TLatex *mres = new TLatex(0.45,0.25,lab);
  //mres->SetTextSize(0.1);
  mres->SetNDC();
  if(pt_resolution_fit)  
    mres->Draw();

  // For making a cut on the momentum difference between rgpT and rpT 
  double pT_sigmas = 6.0;
  double const_term =  fpt->GetParameter(0);
  double linear_term = fpt->GetParameter(1);

  TH1D *hpt_truth;
  fin->GetObject("hpt_truth",hpt_truth);
  if(!hpt_truth)
    {
      cout << "Failed to get hpt_truth, quit!" << endl;
      exit(1);
    }
  
  //TCanvas *ctruth = new TCanvas("ctruth","ctruth", 5,5,800,600);

  TH1D *hpt_matched = new TH1D("hpt_matched","hpt_matched", 500, 0.0, hptmax);
  double eff_pt[200];

  for(int i = 0;i<NPT;i++)
    {
      // divide pT range into bins of width 0.5 GeV/c at 0.25, 0.75, 1.25, .....
      // bins will be from 0-0.5, 0.5-1.0, 1.0-1.5, .....
      double ptlo = (double) i * ptstep + 0.0;
      double pthi = (double) i * ptstep + ptstep;
      double ptval = (ptlo+pthi)/2.0;

      int binlo = hpt_dca2d->GetXaxis()->FindBin(ptlo);
      int binhi = hpt_dca2d->GetXaxis()->FindBin(pthi);

      TH1D *hpt1 = new TH1D("hpt1","pT resolution ",2000, 0.0, 2.0);    
      hpt_compare->ProjectionY("hpt1",binlo,binhi);

      double ptres = sqrt(pow(const_term,2) + pow(linear_term * ptval,2)); 
      double ptreslo = 1.0 - ptres * pT_sigmas;
      double ptreshi = 1.0 + ptres * pT_sigmas;

      int momlo = hpt1->GetXaxis()->FindBin(ptreslo);
      int momhi = hpt1->GetXaxis()->FindBin(ptreshi);

      hpt_matched->Fill(ptval, hpt1->Integral(momlo, momhi));

      int tlo = hpt_truth->FindBin(ptlo);
      int thi = hpt_truth->FindBin(pthi);
      double truth_yield = hpt_truth->Integral(tlo, thi);;
      eff_pt[i] = hpt1->Integral(momlo, momhi) / truth_yield;
      //eff_pt[i] = hpt1->Integral() / truth_yield;
      cout << " pT " << ptval << " ptres " << ptres << " ptreslo " << ptreslo << " ptreshi " << ptreshi << " momlo " << momlo << " momhi " << momhi << endl;
      cout << "      truth_yield " << truth_yield << " yield " << hpt1->Integral(momlo,momhi) << " eff_pt " << eff_pt[i] << endl;

      /*
      if(i == 20)
	{
	  ctruth->cd(0);
	  hpt1->DrawCopy();
	  TF1 *fres = new TF1("fres","gaus");
	  fres->SetParameter(0, 20.0);
	  fres->FixParameter(1, 1.0);
	  fres->FixParameter(2, ptres);
	  fres->SetLineColor(kRed);
	  hpt1->Fit(fres);
	  fres->DrawCopy("same");
	  ctruth->Update();
	  cout << hpt1->Integral() << endl;
	  cout << ptres << endl;
	  int k = 0;
	  int y=0;
	  cin >> k >> y;
	}
      */

      cout << " ptval " << ptval 
	   << " ptreslo " << ptreslo
	   << " ptreshi " << ptreshi
	   << " total " << hpt1->Integral()
	   << " momlo " << momlo
	   << " momhi " << momhi
	   << "  cut " << hpt1->Integral(momlo,momhi)
	   << " tlo  " << tlo
	   << " thi " << thi
	   << " truth " << truth_yield
	   << " eff_pt " << eff_pt[i]
	   << endl;

      delete hpt1;

    }  
  
  cout << " create canvas c7" << endl;
  TCanvas *c7 = new TCanvas("c7","c7",60,60,800,800);

  TH1F *hd = new TH1F("hd","hd",100, 0.0, hptmax);
  hd->SetMinimum(0.0);
  hd->SetMaximum(1.0);
  hd->GetXaxis()->SetTitle("p_{T} (GeV/c)");
  hd->GetYaxis()->SetTitle("Single track efficiency");
  hd->GetYaxis()->SetTitleOffset(1.0);
  hd->Draw();

  TGraph *gr_eff = new TGraph(NPT,pT,eff_pt);
  gr_eff->SetName("single_track_efficiency");
  gr_eff->SetMarkerStyle(20);
  gr_eff->SetMarkerSize(1);
  gr_eff->SetMarkerColor(kRed);

  gr_eff->Draw("p");

 TLegend *leff = new TLegend(0.40, 0.35, 0.99, 0.50, dcalab, "NDC");
  leff->SetBorderSize(0);
  leff->SetFillColor(0);
  leff->SetFillStyle(0);
  char lstr[500];
  //sprintf(lstr,"MVTX+INTT+TPC",n_maps_layer,n_intt_layer,n_tpc_layer);
  sprintf(lstr,"INTT+TPC",n_maps_layer,n_intt_layer,n_tpc_layer);
  leff->AddEntry(gr_eff, lstr, "p");
  //leff->AddEntry(lmax, "max possible efficiency", "l");
  leff->Draw();

  /*
  //=======================
  // Get track purity for Hijing events by 
  // finding tracks within pt_sigmas of the 
  // truth pT

  TH2D *hpt_hijing_compare = 0;
  fin->GetObject("hpt_hijing_compare",hpt_hijing_compare);
  if(!hpt_hijing_compare)
    {
      cout << "Did not get hpt_hijing_compare - quit!" << endl;
      exit(1);
    }

  static const int NVARBINS = 36;
  double xbins[NVARBINS+1] = {0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0,
			      1.1, 1.2,1.3,1.4,1.5,1.6,1.7,1.8,1.9,2.0,
			      2.2,2.4,2.6,2.8,3.0,3.2,3.4,3.6,3.8,4.0,
			      4.5,5.0,5.5,6.0,7.0,8.0,10.0};
  
  TH1D *hpt_hijing_allreco = new TH1D("hpt_hijing_allreco","hpt_hijing_allreco",NVARBINS,xbins);
  TH1D *hpt_hijing_matched = new TH1D("hpt_hijing_matched","hpt_hijing_matched",NVARBINS,xbins);
  hpt_hijing_matched->GetYaxis()->SetTitle("Purity");

  //double purity_pt[NVARBINS];
  
  for (int i=0;i<NVARBINS;i++)  
    {
      double ptlo = xbins[i];
      double pthi = xbins[i+1];
      double ptval = (ptlo+pthi)/2.0;

      int binlo = hpt_hijing_compare->GetXaxis()->FindBin(ptlo);
      int binhi = hpt_hijing_compare->GetXaxis()->FindBin(pthi);

      TH1D *hpt1 = new TH1D("hpt1","pT resolution ",2000, 0.0, 2.0);    
      hpt_hijing_compare->ProjectionY("hpt1",binlo,binhi);

      double ptres = sqrt(pow(const_term,2) + pow(linear_term * ptval,2)); 
      double ptreslo = 1.0 - ptres * pT_sigmas;
      double ptreshi = 1.0 + ptres * pT_sigmas;

      int momlo = hpt1->GetXaxis()->FindBin(ptreslo);
      int momhi = hpt1->GetXaxis()->FindBin(ptreshi);

      // This to avoid TEfficiency's stupid insistence that the histograms cannot be filled with weights
      for (int j=0;j<(int)hpt1->Integral(momlo,momhi);j++)
	hpt_hijing_matched->Fill(ptval);
      for (int j=0;j<(int)hpt1->Integral();j++)
	hpt_hijing_allreco->Fill(ptval);

      //purity_pt[i] = hpt1->Integral(momlo, momhi) / hpt1->Integral();

      delete hpt1;
    }

  cout << " create canvas cpur " << endl;
  TCanvas *cpur = new TCanvas("cpur","cpur",60,60,1000,600);

  TEfficiency* pEff = 0;
  //if(TEfficiency::CheckConsistency(*hpt_hijing_matched,*hpt_hijing_allreco))
    {
      pEff = new TEfficiency(*hpt_hijing_matched,*hpt_hijing_allreco);
      char tname[500];
      sprintf(tname,"Reconstruction efficiency (%.1f#sigma p_{T}) ; p_{T} (GeV/c) ; reco'd tracks within %.0f #sigma p_{T}",pT_sigmas,pT_sigmas);
      //pEff->SetTitle("Reconstruction efficiency (3.0#sigma p_{T}) ; p_{T} (GeV/c) ; reco'd tracks within 3 #sigma p_{T}");
      pEff->SetTitle(tname);
      pEff->SetMarkerStyle(20);
      pEff->SetMarkerColor(kBlack);
      pEff->SetMarkerSize(1);
      pEff->Draw();
      gPad->Update();
      pEff->GetPaintedGraph()->GetYaxis()->SetTitleOffset(1.0);
      pEff->GetPaintedGraph()->GetYaxis()->SetRangeUser(0.0,1.1);
      pEff->GetPaintedGraph()->GetXaxis()->SetTitleOffset(1.1);
    }
  */

  //=========================
  // plot quality for Hijing tracks

  TCanvas *cpq = new TCanvas("cpq","cpq",40,40,1200,600);
  TH1D * hquality;
  fin->GetObject("hquality",hquality);
  hquality->Draw();

  //======================
  // Plot number of hits per track
  TCanvas *c8 = new TCanvas("c8","c8",40,40,1200,600);
  TH1D * hnhits;
  fin->GetObject("hnhits",hnhits);
  hnhits->GetXaxis()->SetTitle("hits per track");
  hnhits->Draw();
  cout << "hnhits integral = " << hnhits->Integral() << endl;


  //================================
  // extract three DCA 2d distributions inside pT bins

  TCanvas *cdcadist = new TCanvas("cdcadist","cdcadist",5,20,1200,800);
  cdcadist->Divide(3,1);

  double dcaptbinlo[3] = {0.5, 1.0, 2.0};
  double dcaptbinhi[3] = {1.0, 2.0, 50.0};
  for(int i = 0;i<3;i++)
    {
      double ptlo = dcaptbinlo[i];
      double pthi = dcaptbinhi[i];

      int binlo = hpt_dca2d->GetXaxis()->FindBin(ptlo);
      int binhi = hpt_dca2d->GetXaxis()->FindBin(pthi);

      std::cout << "DCA distr: ptlo " << ptlo << " binlo " << binlo << " pthi " << pthi << " binhi " << binhi << std::endl;

      TH1D *h = new TH1D("h","dca2d resolution",2000, -0.1, 0.1);
      hpt_dca2d->ProjectionY("h",binlo,binhi);
      h->GetXaxis()->SetTitle("DCA (r#phi) (cm)");
      h->GetXaxis()->SetTitleOffset(1.0);
      h->SetMinimum(0.5);

      cdcadist->cd(i+1);
      gPad->SetLogy(1);
      h->Sumw2();
      h->Rebin(4);
      //h->SetMarkerStyle(20);
      h->SetMarkerSize(0.8);
      h->GetXaxis()->SetNdivisions(505);
      h->DrawCopy("E");

      double mean = h->GetMean();
      double sigma = h->GetRMS();
      double yield = h->Integral();
  
      double low = -0.01, high=0.01;
      if(n_maps_layer == 0)
	{
	  low = 3.0 * low;
	  high = 3.0 * high;
	}

      if(pthi < 6.0)
	{
	  low = 3.0*low;
	  high = 3.0*high;
	}

      TF1 *f = new TF1("f","gaus",low, high);
      f->SetParameter(1, yield/100.0);
      f->SetParameter(2, 0.0);
      f->SetParameter(3,0.002);
      f->SetLineColor(kRed);
      h->Fit(f,"R");

      char fitr[500];
      sprintf(fitr,"p_{T} = %.1f-%.1f GeV/c",ptlo,pthi);
      TLatex *l1 = new TLatex(0.2,0.971,fitr);
      l1->SetNDC();
      l1->SetTextSize(0.07);
      l1->Draw();

      sprintf(fitr,"#sigma = %.1f #mum", f->GetParameter(2)*10000);
      TLatex *l1a = new TLatex(0.2,0.892,fitr);
      l1a->SetNDC();
      l1a->SetTextSize(0.07);
      l1a->Draw();

       cout << " pT range " << ptlo << " " << pthi << " counts " << h->Integral() << " hist mean " << h->GetMean() << " hist RMS " << h->GetRMS() << endl;
    }

  //================================
  // extract three DCA Z distributions inside pT bins

  TCanvas *cdcazdist = new TCanvas("cdcazdist","cdcazdist",5,20,1200,800);
  cdcazdist->Divide(3,1);

  for(int i = 0;i<3;i++)
    {
      double ptlo = dcaptbinlo[i];
      double pthi = dcaptbinhi[i];

      int binlo = hpt_dcaZ->GetXaxis()->FindBin(ptlo);
      int binhi = hpt_dcaZ->GetXaxis()->FindBin(pthi);

      std::cout << "DCA Z distr: ptlo " << ptlo << " binlo " << binlo << " pthi " << pthi << " binhi " << binhi << std::endl;

      TH1D *h = new TH1D("h","dcaZ resolution",2000, -0.1, 0.1);
      hpt_dcaZ->ProjectionY("h",binlo,binhi);
      h->GetXaxis()->SetTitle("DCA (Z) (cm)");
      h->GetXaxis()->SetTitleOffset(1.0);

      cdcazdist->cd(i+1);
      gPad->SetLogy(1);
      h->Sumw2();
      h->Rebin(4);
      h->SetMarkerSize(0.8);
      h->GetXaxis()->SetNdivisions(505);
      h->SetMinimum(0.5);
      h->DrawCopy("E");

      double mean = h->GetMean();
      double sigma = h->GetRMS();
      double yield = h->Integral();
  
      double low = -0.01, high=0.01;
      if(n_maps_layer == 0)
	{
	  low = 3.0 * low;
	  high = 3.0 * high;
	}

      if(pthi < 6.0)
	{
	  low = 3.0*low;
	  high = 3.0*high;
	}

      TF1 *f = new TF1("f","gaus",low, high);
      f->SetParameter(1, yield/100.0); 
     f->SetParameter(2, 0.0);
      f->SetParameter(3,0.002);
      f->SetLineColor(kRed);
      h->Fit(f,"R");

      char fitr[500];
      sprintf(fitr,"p_{T} = %.1f-%.1f GeV/c",ptlo,pthi);
      TLatex *l1 = new TLatex(0.2,0.971,fitr);
      l1->SetNDC();
      l1->SetTextSize(0.07);
      l1->Draw();

      sprintf(fitr,"#sigma = %.1f #mum", f->GetParameter(2)*10000);
      TLatex *l1a = new TLatex(0.2,0.892,fitr);
      l1a->SetNDC();
      l1a->SetTextSize(0.07);
      l1a->Draw();

       cout << " pT range " << ptlo << " " << pthi << " counts " << h->Integral() << " hist mean " << h->GetMean() << " hist RMS " << h->GetRMS() << endl;
    }

  /*

  //=========================

  TCanvas *cdca = new TCanvas("cdca","cdca",5,20,1200,800);
  //cdca->SetTopMargin(0.2);
  cdca->Divide(3,1);

  TH1D *hdca2d[3];
  fin->GetObject("hdca2d0",hdca2d[0]);
  fin->GetObject("hdca2d1",hdca2d[1]);
  fin->GetObject("hdca2d2",hdca2d[2]);

  cdca->cd(1);
  gPad->SetLogy(1);
  //gPad->SetRightMargin(0.1);
  hdca2d[0]->GetXaxis()->SetNdivisions(505);

  hdca2d[0]->Draw();

  cdca->cd(2);
  gPad->SetLogy(1);
  hdca2d[1]->GetXaxis()->SetNdivisions(505);
  hdca2d[1]->Draw();

  cdca->cd(3);
  gPad->SetLogy(1);
  hdca2d[2]->GetXaxis()->SetNdivisions(505);
  hdca2d[2]->Draw();

  TF1 *fdca = new TF1("fdca","gaus",-0.1,0.1);
  fdca->SetLineColor(kRed);
  cdca->cd(1);
  hdca2d[0]->Fit(fdca);

  TLatex *l1a = new TLatex(0.2,0.971,"p_{T} = 0.5-1.0 GeV/c");
  l1a->SetNDC();
  l1a->SetTextSize(0.07);
  l1a->Draw();
  char fitr[500];
  sprintf(fitr,"#sigma = %.1f #mum",fdca->GetParameter(2)*10000);
  TLatex *l1 = new TLatex(0.2,0.892,fitr);
  l1->SetNDC();
  l1->SetTextSize(0.07);
  l1->Draw();

  cdca->cd(2);
  hdca2d[1]->Fit(fdca);

  TLatex *l2a = new TLatex(0.2,0.971,"p_{T} = 1.0-2.0 GeV/c");
  l2a->SetNDC();
  l2a->SetTextSize(0.07);
  l2a->Draw();

  sprintf(fitr,"#sigma = %.1f #mum",fdca->GetParameter(2)*10000);
  TLatex *l2 = new TLatex(0.2,0.892,fitr);
  l2->SetNDC();
  l2->SetTextSize(0.07);
  l2->Draw();


  cdca->cd(3);
  hdca2d[2]->Fit(fdca);
  sprintf(fitr,"#splitline{p_{T} > 2.0 GeV/c}{#sigma = %.1f #mum}",fdca->GetParameter(2)*10000);

  TLatex *l3a = new TLatex(0.2,0.971,"p_{T} = 1.0-2.0 GeV/c");
  l3a->SetNDC();
  l3a->SetTextSize(0.07);
  l3a->Draw();

  sprintf(fitr,"#sigma = %.1f #mum",fdca->GetParameter(2)*10000);
  TLatex *l3 = new TLatex(0.2,0.892,fitr);
  l3->SetNDC();
  l3->SetTextSize(0.07);
  l3->Draw();
  */

  TCanvas *ceta = new TCanvas("ceta","ceta",10,10,600,600);

  TH1D *hgeta = 0;
  fin->GetObject("hgeta",hgeta);
  if(!hgeta)
    {
      cout << "Did not get hgeta" << endl;
      exit(1);
    }
  TH1D *hreta = 0;
  fin->GetObject("hreta",hreta);

  hreta->GetXaxis()->SetTitle("#eta");
  hreta->GetYaxis()->SetNdivisions(505);
  hreta->SetLineColor(kRed);
  hreta->SetMinimum(0.0);
  hreta->Draw();

  hgeta->Draw("same");


 TLegend *leta = new TLegend(0.3, 0.35, 1.0, 0.60, dcalab, "NDC");
  leta->SetBorderSize(0);
  leta->SetFillColor(0);
  leta->SetFillStyle(0);
  leta->AddEntry(hgeta,"Truth","l");
  leta->AddEntry(hreta,"Reconstructed","l");
  leta->Draw();
  ceta->Update();

  TCanvas *cfake = new TCanvas("cfake","cfake",4,4,800,600);
  cfake->SetLeftMargin(0.15);
  TH2D *hpt_nfake = 0;
  fin->GetObject("hpt_nfake",hpt_nfake);
  if(!hpt_nfake)
    {
      cout << "Did not get hpt_nfake" << endl;
      exit(1);
    }

  int n_noise = 4;
  double ynoise[10][200];
  double ynoise_ref[200];
  //double dptfake = 1.0;
  double dptfake = 0.2;
  double fake_ptmax = 20.0;
  int nptfake = (int) (fake_ptmax/dptfake);
  double ptfake[200];
  for(int ipt=0;ipt<nptfake;ipt++)
    {
      double ptlo = dptfake * ipt;
      double  pthi = dptfake*ipt + dptfake;
      ptfake[ipt] = (ptlo+pthi) / 2.0;
      int binlo = hpt_nfake->GetXaxis()->FindBin(ptlo);
      int binhi = hpt_nfake->GetXaxis()->FindBin(pthi);
      
      TH1D *hnoise = new TH1D("hnoise","",73,-5,68);      
      hpt_nfake->ProjectionY("hnoise",binlo,binhi);
      double ynoise_ref = hnoise->Integral();
      for(int j=0;j<n_noise;j++)
	{
	  int bin = hnoise->FindBin(j);
	  ynoise[j][ipt]= hnoise->GetBinContent(bin);
	  ynoise[j][ipt] /= ynoise_ref;
	}
      hnoise->Delete();
    }
  

  TH1D *hfdummy = new TH1D("hfdummy","",100,0.0,fake_ptmax);
  hfdummy->SetMaximum(1.05);
  hfdummy->SetMinimum(0.001);
  hfdummy->GetXaxis()->SetTitle("p_{T} (GeV/c)");
  hfdummy->GetYaxis()->SetTitle("Fraction of tracks");
  gPad->SetLogy(1);
  hfdummy->Draw();

  TLegend *lfake = new TLegend(0.35,0.55,0.87,0.89,"","NDC");
  lfake->SetBorderSize(1);
  TGraph *hnoise[10];
  int fake_col[5] = {kBlack,kRed,kBlue,kMagenta,kViolet};
  for(int j=0;j<n_noise;j++)
    {
      hnoise[j] = new TGraph(nptfake,ptfake,ynoise[j]);      
      hnoise[j]->SetMarkerStyle(20);
      hnoise[j]->SetMarkerSize(1.0);
      hnoise[j]->SetMarkerColor(fake_col[j]);
      if(j == 0)
	hnoise[j]->Draw("p");
      else
	hnoise[j]->Draw("p same");	
      char lab[500];
      sprintf(lab,"%i mismatched hits",j);
      lfake->AddEntry(hnoise[j],lab,"p");
    }
  lfake->Draw();

  TH2D *hcorr_nfake_nmaps = 0;
  fin->GetObject("hcorr_nfake_nmaps",hcorr_nfake_nmaps);
  if(hcorr_nfake_nmaps)
    {
      TCanvas *cmm = new TCanvas("cmm","cmm",4,4,800,600);
      cmm->SetLeftMargin(0.15);
      
      gPad->SetLogy(1);
      
      TLegend *lmm = new TLegend(0.35,0.62,0.9,0.90,"","NDC");
      lmm->SetBorderSize(1);
      for(int imaps =0;imaps<n_maps_layer+1;imaps++)
	{
	  double nmapslo = -0.5  + 1.0 * imaps;
	  double nmapshi = nmapslo + 1.0;
	  
	  int binlo = hcorr_nfake_nmaps->GetYaxis()->FindBin(nmapslo);
	  int binhi = hcorr_nfake_nmaps->GetYaxis()->FindBin(nmapshi);
	  
	  TH1D *h = new TH1D("h","h",40,-1,4);
	  hcorr_nfake_nmaps->ProjectionX("h",binlo,binhi);
	  
	  h->SetMarkerStyle(20);
	  h->SetMarkerSize(1.5);
	  h->SetMarkerColor(fake_col[imaps]);
	  h->SetMinimum(10);	  
	  h->SetMaximum(5.0e6);	  
	  char lab[500];
	  sprintf(lab,"%i missed maps layers",imaps);
	  lmm->AddEntry(h,lab,"p");
	  
	  if(imaps == 0)
	    {
	      h->GetXaxis()->SetTitle("Mismatched hits");
	      h->GetYaxis()->SetTitle("Tracks");
	      h->DrawCopy("p");
	      
	    }
	  else
	    h->DrawCopy("same p");
	}
      
      lmm->Draw();
    }
    
  TCanvas *cvtx = new TCanvas("cvtx","cvtx",4,4,800,600);
  TH1D *hzevt = 0;
  fin->GetObject("hzevt",hzevt);
  if(!hzevt)
    {
      cout << "Did not get hzevt" << endl;
      exit(1);
    }
  hzevt->Draw();

  TCanvas *cvtx_res = new TCanvas("cvtx_res","cvtx_res",4,4,1200,800);
  cvtx_res->Divide(2,2);

  TH1D *hzvtx_res = 0;
  fin->GetObject("hzvtx_res",hzvtx_res);
  if(!hzvtx_res)
    {
      cout << "Did not get hzvtx_res" << endl;
    }

  if(hzvtx_res)
    {
      cvtx_res->cd(1);

      hzvtx_res->SetNdivisions(505);
      hzvtx_res->Draw();
      
      TF1 *fvtxres = new TF1("fvtxres","gaus");
      //hzvtx_res->Fit(fvtxres);
    }

  TH1D *hxvtx_res = 0;
  fin->GetObject("hxvtx_res",hxvtx_res);
  if(!hxvtx_res)
    {
      cout << "Did not get hxvtx_res" << endl;
    }

  if(hxvtx_res)
    {
      cvtx_res->cd(2);

      hxvtx_res->SetNdivisions(505);
      hxvtx_res->Draw();
      
      TF1 *fvtxres = new TF1("fvtxres","gaus");
      //hxvtx_res->Fit(fvtxres);
    }

  TH1D *hyvtx_res = 0;
  fin->GetObject("hyvtx_res",hyvtx_res);
  if(!hyvtx_res)
    {
      cout << "Did not get hyvtx_res" << endl;
    }

  if(hyvtx_res)
    {
      cvtx_res->cd(3);

      hyvtx_res->SetNdivisions(505);
      hyvtx_res->Draw();
      
      TF1 *fvtxres = new TF1("fvtxres","gaus");
      //hyvtx_res->Fit(fvtxres);
    }

  TH1D *hdcavtx_res = 0;
  fin->GetObject("hdcavtx_res",hdcavtx_res);
  if(!hdcavtx_res)
    {
      cout << "Did not get hdcavtx_res" << endl;
    }

  if(hdcavtx_res)
    {
      cvtx_res->cd(4);

      hdcavtx_res->SetNdivisions(505);
      hdcavtx_res->Draw();
      
      TF1 *fvtxres = new TF1("fvtxres","gaus");
      //hdcavtx_res->Fit(fvtxres);
    }


  TCanvas *g4ntr = new TCanvas("g4ntr","g4ntr",5,10,1200,800);
  g4ntr->Divide(3,1);
  g4ntr->cd(1);
  TH2D *hg4ntrack = 0;
  fin->GetObject("hg4ntrack",hg4ntrack);
  if(hg4ntrack)
    {
      hg4ntrack->GetXaxis()->SetTitle(" Tracks/event from truth");
      hg4ntrack->GetYaxis()->SetTitle(" Tracks/event reconstructed");
      hg4ntrack->Draw();
    }
  else
    cout << "Did not get hgn4track" << endl;

  g4ntr->cd(2);
  TH1D *hg4ntr = new TH1D("hg4ntr","hg4ntr",200, 0.0, 2000.0);
  hg4ntrack->ProjectionX("hg4ntr");
  hg4ntr->GetXaxis()->SetTitle(" Tracks/event from truth");
  hg4ntr->GetYaxis()->SetTitle(" Yield");
  hg4ntr->Draw();

  g4ntr->cd(3);
  double cent[20];
  double nhits[20];
  nhits[0] = 0;
  double ytot = hg4ntr->Integral(); 
  for(int i=1;i<20;i++)
    {
      nhits[i] = i*120.0;
      int binhi = hg4ntr->FindBin(nhits[i]);
      double y = hg4ntr->Integral(1,binhi);
      cent[i] = y / ytot;
      cout << "nhits = " << nhits[i] << " binhi = " << binhi << " y = " << y << " ytot = " << ytot << " cent = " << cent[i] << endl;
    }
  TGraph *gy = new TGraph(20,nhits,cent);
  gy->GetXaxis()->SetTitle(" Tracks/event from truth");
  gy->GetYaxis()->SetTitle(" integral fraction of events");
  gy->Draw();

  TF1 *fgy = new TF1("fgy","[0]*x+[1]*x*x+[2]*x*x*x",0,2300);
  gy->Fit(fgy,"R");

  // Output some graphs for comparison plots

 TFile *fout = new TFile("root_files/look_purity_out.root","recreate");
  gr_eff->Write();
  grdca2d->Write();
  grdcaZ->Write();
  grdpt->Write();

}
