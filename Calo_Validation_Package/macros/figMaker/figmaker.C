
#include "sPhenixStyle.C"

void myText(Double_t x,Double_t y,Color_t color, const char *text, Double_t tsize=0.04) {

  TLatex l; l.SetTextAlign(12); l.SetTextSize(tsize);
  l.SetNDC();
  l.SetTextColor(color);
  l.DrawLatex(x,y,text);
}

TH1F* proj(TH2F* h2);

TH1F* FBratio(TH1F* h){

   const int Nbin =  h->GetXaxis()->GetNbins();
   TH1F* hfb = new TH1F("temp32","",Nbin/2,0,Nbin/2);

   for(int i=0; i<Nbin/2; i++){
     int b1 = i+1;
     int b2 = Nbin-i;
     float ratio = h->GetBinContent(b1)/h->GetBinContent(b2);
     float err = sqrt( pow(h->GetBinError(b1)/h->GetBinContent(b1),2)+ pow(h->GetBinError(b2)/h->GetBinContent(b2),2))*pow(ratio,2);
     hfb->SetBinContent(i,ratio); 
     hfb->SetBinError(i,err);
   }
   return hfb;
}



void figmaker(){


  SetsPhenixStyle();

  std::ifstream infile("../condor/runList.txt");
  if (!infile) { std::cerr << "Error: Unable to open the file\n"; return 1;}
  std::vector<int> runList;
  int num;
  while (infile >> num)  runList.push_back(num);

  int Nruns= runList.size();

  for (int ir=0; ir<Nruns; ir++){

    int run = runList[ir];

    TFile* fin = new TFile(Form("../condor/combine_out/out_%d.root",run));

    TH2F* h_emcal_mbd_correlation  = (TH2F*) fin->Get("h_emcal_mbd_correlation" );
    TH2F* h_ihcal_mbd_correlation  = (TH2F*) fin->Get("h_ihcal_mbd_correlation" );
    TH2F* h_ohcal_mbd_correlation  = (TH2F*) fin->Get("h_ohcal_mbd_correlation" );
    TH2F* h_emcal_hcal_correlation = (TH2F*) fin->Get("h_emcal_hcal_correlation");
    TH1F* h_InvMass                = (TH1F*) fin->Get("h_InvMass"               );
    TH2F* h_cemc_etaphi            = (TH2F*) fin->Get("h_cemc_etaphi"           );
    TH2F* h_ihcal_etaphi           = (TH2F*) fin->Get("h_ihcal_etaphi"          );
    TH2F* h_ohcal_etaphi           = (TH2F*) fin->Get("h_ohcal_etaphi"          );
    TH2F* h_zdc_emcal_correlation  = (TH2F*) fin->Get("h_zdc_emcal_correlation");
    TH1F* hzdcNorthcalib           = (TH1F*) fin->Get("hzdcNorthcalib");
    TH1F* hzdcSouthcalib           = (TH1F*) fin->Get("hzdcSouthcalib");

    TCanvas* c1 = new TCanvas("c1", "c1", 400, 400);
    h_emcal_mbd_correlation ->Draw("COLZ");
    h_emcal_mbd_correlation ->SetXTitle("#Sigma #it{E}^{EMCal} [Arb]");
    h_emcal_mbd_correlation ->SetYTitle("#Sigma #it{E}^{MBD} [Arb]");
    myText(0.22, 0.9, 1, "#it{#bf{sPHENIX}} Internal");
    myText(0.22, 0.85, 1, Form("run %d", run));
    gPad->SetRightMargin(0.15);
    gPad->SetLogz();

    c1->SaveAs(Form("../plots/emcal_mbd_correlation_%d.pdf",run));


    TCanvas* c2 = new TCanvas("c2", "c2", 400, 400);
    h_ihcal_mbd_correlation ->Draw("COLZ");
    h_ihcal_mbd_correlation ->SetXTitle("#Sigma #it{E}^{ihcal} [Arb]");
    h_ihcal_mbd_correlation ->SetYTitle("#Sigma #it{E}^{MBD} [Arb]");
    myText(0.22, 0.9, 1, "#it{#bf{sPHENIX}} Internal");
    myText(0.22, 0.85, 1, Form("run %d", run));
    gPad->SetRightMargin(0.15);
    gPad->SetLogz();

    c2->SaveAs(Form("../plots/ihcal_mbd_correlation_%d.pdf",run));

    TCanvas* c3 = new TCanvas("c3", "c3", 400, 400);
    h_ohcal_mbd_correlation ->Draw("COLZ");
    h_ohcal_mbd_correlation ->SetXTitle("#Sigma #it{E}^{ohcal} [Arb]");
    h_ohcal_mbd_correlation ->SetYTitle("#Sigma #it{E}^{MBD} [Arb]");
    myText(0.22, 0.9, 1, "#it{#bf{sPHENIX}} Internal");
    myText(0.22, 0.85, 1, Form("run %d", run));
    gPad->SetRightMargin(0.15);
    gPad->SetLogz();

    c3->SaveAs(Form("../plots/ohcal_mbd_correlation_%d.pdf",run));


    TCanvas* c4 = new TCanvas("c4", "c4", 400, 400);
    h_emcal_hcal_correlation->Draw("COLZ");
    h_emcal_hcal_correlation->SetXTitle("#Sigma #it{E}^{EMCal} [Arb]");
    h_emcal_hcal_correlation->SetYTitle("#Sigma #it{E}^{HCal} [Arb]");
    myText(0.22, 0.9, 1, "#it{#bf{sPHENIX}} Internal");
    myText(0.22, 0.85, 1, Form("run %d", run));
    gPad->SetRightMargin(0.15);
    gPad->SetLogz();

    c3->SaveAs(Form("../plots/emcal_hcal_correlation_%d.pdf",run));


    TCanvas* c5 = new TCanvas("c5", "c5", 400, 400);
    h_InvMass               ->Draw("");
    h_InvMass               ->SetXTitle("#it{M}_{#gamma#gamma}");
    h_InvMass               ->SetYTitle("counts");
    myText(0.22, 0.9, 1, "#it{#bf{sPHENIX}} Internal");
    myText(0.22, 0.85, 1, Form("run %d", run));

    c5->SaveAs(Form("../plots/InvMass_%d.pdf",run));


    TCanvas* c6 = new TCanvas("c6", "c6", 400, 400);
    h_cemc_etaphi           ->Draw("COLZ");
    h_cemc_etaphi           ->SetXTitle("#it{#eta}_{i} EMCal");
    h_cemc_etaphi           ->SetYTitle("#it{#phi}_{i} EMCal");
    myText(0.22, 0.9, 1, "#it{#bf{sPHENIX}} Internal");
    myText(0.22, 0.85, 1, Form("run %d", run));
    myText(0.22, 0.80,1,"EMCal");
    gPad->SetRightMargin(0.15);

    c6->SaveAs(Form("../plots/cemc_etaphi_%d.pdf",run));




    TCanvas* c7 = new TCanvas("c7", "c7", 400, 400);
    h_ihcal_etaphi          ->Draw("COLZ");
    h_ihcal_etaphi          ->SetXTitle("#it{#eta}_{i} iHcal");
    h_ihcal_etaphi          ->SetYTitle("#it{#phi}_{i} iHcal");
    myText(0.22, 0.9, 1, "#it{#bf{sPHENIX}} Internal");
    myText(0.22, 0.85, 1, Form("run %d", run));
    myText(0.22, 0.80,1,"iHCal");
    gPad->SetRightMargin(0.15);

    c7->SaveAs(Form("../plots/ihcal_etaphi_%d.pdf",run));


    TCanvas* c8 = new TCanvas("c8", "c8", 400, 400);
    h_ohcal_etaphi          ->Draw("COLZ");
    h_ohcal_etaphi          ->SetXTitle("#it{#eta}_{i} oHcal");
    h_ohcal_etaphi          ->SetYTitle("#it{#phi}_{i} oHcal");
    myText(0.22, 0.9, 1, "#it{#bf{sPHENIX}} Internal");
    myText(0.22, 0.85, 1, Form("run %d", run));
    myText(0.22, 0.80,1,"oHCal");
    gPad->SetRightMargin(0.15);

    c8->SaveAs(Form("../plots/ohcal_etaphi_%d.pdf",run));


    TH1F* h_emcal_proj = (TH1F*) proj(h_cemc_etaphi)->Clone("h_emcal_proj");

    TCanvas* c9 = new TCanvas("c9", "c9", 400, 200);
    h_emcal_proj->Draw("hist");
    h_emcal_proj->SetYTitle("N^{twr}(E_{T} > 1 GeV)");

    myText(0.22, 0.9, 1, "#it{#bf{sPHENIX}} Internal");
    myText(0.22, 0.85, 1, Form("run %d", run));
    myText(0.22, 0.80,1,"EMCal");
    
    c9->SaveAs(Form("../plots/cemc_proj_%d.pdf",run));


    TH1F* h_ohcal_proj = (TH1F*) proj(h_ohcal_etaphi)->Clone("h_ohcal_proj");

    TCanvas* c10 = new TCanvas("c10", "c10", 400, 200);
    h_ohcal_proj->Draw("hist");
    h_ohcal_proj->SetYTitle("N^{twr}(E_{T} > 1 GeV)");
    h_ohcal_proj->GetYaxis()->SetRangeUser(0, h_ohcal_proj->GetMaximum()*1.05);

    myText(0.22, 0.9, 1, "#it{#bf{sPHENIX}} Internal");
    myText(0.22, 0.85, 1, Form("run %d", run));
    myText(0.22, 0.80,1,"oHCal");
    
    c10->SaveAs(Form("../plots/ohcal_proj_%d.pdf",run));


    TH1F* h_ihcal_proj = (TH1F*) proj(h_ihcal_etaphi)->Clone("h_ihcal_proj");

    TCanvas* c11 = new TCanvas("c11", "c10", 400, 200);
    h_ihcal_proj->Draw("hist");
    h_ihcal_proj->SetYTitle("N^{twr}(E_{T} > 1 GeV)");
    h_ihcal_proj->SetXTitle("#eta_{i}");

    myText(0.22, 0.9, 1, "#it{#bf{sPHENIX}} Internal");
    myText(0.22, 0.85, 1, Form("run %d", run));
    myText(0.22, 0.80,1,"iHCal");
    
    c11->SaveAs(Form("../plots/ihcal_proj_%d.pdf",run));


    TH1F* h_fb_ratio_emcal = FBratio(h_emcal_proj);
    TH1F* h_fb_ratio_ihcal = FBratio(h_ihcal_proj);
    TH1F* h_fb_ratio_ohcal = FBratio(h_ohcal_proj);

    TCanvas* c12 = new TCanvas("c12", "c12", 400, 200);
    h_fb_ratio_emcal->Draw("ex0");
    h_fb_ratio_emcal->SetYTitle("N^{twr}(#eta_{i})/N^{twr}(#eta_{N-i})");
    h_fb_ratio_emcal->SetXTitle("#eta_{i}");
    h_fb_ratio_emcal->GetYaxis()->SetRangeUser(0.1,2);

    h_fb_ratio_ohcal->Draw("ex0 same");
    h_fb_ratio_ohcal->SetLineColor(kBlue);
    h_fb_ratio_ohcal->SetMarkerColor(kBlue);
    h_fb_ratio_ohcal->SetMarkerStyle(22);

    h_fb_ratio_ihcal->Draw("ex0 same");
    h_fb_ratio_ihcal->SetLineColor(kRed);
    h_fb_ratio_ihcal->SetMarkerColor(kRed);
    h_fb_ratio_ihcal->SetMarkerStyle(33);

    myText(0.22, 0.9, 1, "#it{#bf{sPHENIX}} Internal");
    myText(0.22, 0.85, 1, Form("run %d", run));
    myText(0.22, 0.80,1,"EMCal");
    myText(0.32,0.80,kBlue,"oHCal");
    myText(0.42,0.80,kRed,"iHCal");
    
    c12->SaveAs(Form("../plots/h_fb_ratio_emcal_%d.pdf",run));


    TCanvas* c13 = new TCanvas("c13", "c13", 400, 200);* 
    h_zdc_emcal_correlation->Draw("COLZ");
    h_zdc_emcal_correlation->SetXTitle("#Sigma #it{E}^{EMCal} [Arb]");
    h_zdc_emcal_correlation->SetYTitle("#Sigma #it{E}^{ZDC} [Arb]");
    myText(0.22, 0.9, 1, "#it{#bf{sPHENIX}} Internal");
    myText(0.22, 0.85, 1, Form("run %d", run));
    gPad->SetRightMargin(0.15);
    gPad->SetLogz();

    c13->SaveAs(Form("../plots/zdc_emcal_correlation_%d.pdf",run));


    TCanvas* c14 = new TCanvas("c14", "c14", 400, 200);* 
    hzdcNorthcalib->Draw();
    hzdcNorthcalib->GetXaxis()->SetRangeUser(0.25,15000);
    hzdcNorthcalib->SetXTitle("#Sigma #it{E}^{ZDC Side}");
    hzdcNorthcalib->SetYTitle("Events");

    hzdcSouthcalib->Draw("same");
    hzdcSouthcalib->SetLineColor(kBlack);
    gPad->SetLogx();

    myText(0.22, 0.9, 1, "#it{#bf{sPHENIX}} Internal");
    myText(0.22, 0.85, 1, Form("run %d", run));

    c3->SaveAs(Form("../plots/zdc_emcal_correlation_%d.pdf",run));

  }




}


TH1F* proj(TH2F* h2){

  int x = h2->GetXaxis()->GetNbins();
  int y = h2->GetYaxis()->GetNbins();
  TH1F* h = (TH1F*) h2->ProjectionX("temp");
  h->Reset();
  for (int ix=1; ix<x+1; ix++){
    float sum = 0;
    float cc = 0;
    for(int iy=1; iy<y+1; iy++){
      float bc = h2->GetBinContent(ix,iy);
      if (bc==0) cc+=1;
      sum += bc;
    }
    if (cc==y) continue;
    float sum_cor = sum*y/(y-cc);
    h->SetBinContent(ix,sum_cor);
    h->SetBinError(ix,sqrt(sum_cor));
  }

  return h;
}

