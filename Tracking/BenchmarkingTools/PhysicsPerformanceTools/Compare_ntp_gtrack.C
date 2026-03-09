#include "../CommonTools.h"

/*
 * This macro compares two versions of ntp_gtrack and makes plots that
 * compare typical tracking physics performance. It is called with the
 * two file names as arguments, e.g.
 * root -l -b -q Compare_ntp_gtrack.C'("file1.root","file2.root")'
 * where both root files are the SvtxEvaluator output with ntp_gtrack
 *
 * The output is some canvases which compare benchmark physics performance
 *
 */

void Compare_ntp_gtrack(std::string file1, std::string file2)
{
  gStyle->SetOptStat(0);
  TFile *df = TFile::Open(file1.c_str());

  TFile *nf = TFile::Open(file2.c_str());

  TNtuple *dt = (TNtuple*)df->Get("ntp_gtrack");
  TNtuple *nt = (TNtuple*)nf->Get("ntp_gtrack");
  
  dt->Draw("gpt>>dtruth(40,0,20)");
  nt->Draw("gpt>>ntruth(40,0,20)");
  dt->Draw("gpt>>dreco(40,0,20)","quality<10 && ntpc>30");
  nt->Draw("gpt>>nreco(40,0,20)","quality<10 && ntpc>30");
  dt->Draw("gpt>>drecomaps(40,0,20)","quality<10 && nmaps>2 && ntpc>30");
  nt->Draw("gpt>>nrecomaps(40,0,20)","quality<10 && nmaps>2 && ntpc>30");
  
  TH1* dtruth = (TH1F*)gDirectory->Get("dtruth");
  TH1 *ntruth = (TH1F*)gDirectory->Get("ntruth");
  TH1 *dreco = (TH1F*)gDirectory->Get("dreco");
  TH1 *nreco = (TH1F*)gDirectory->Get("nreco");
  TH1 *drecomaps = (TH1F*)gDirectory->Get("drecomaps");
  TH1 *nrecomaps = (TH1F*)gDirectory->Get("nrecomaps");

  dt->Draw("pt/gpt:gpt>>dpt(40,0,40,400,0.5,1.5)","quality<10","colz");
  nt->Draw("pt/gpt:gpt>>npt(40,0,40,400,0.5,1.5)","quality<10","colz");
  
  TH2* dpt = (TH2F*)gDirectory->Get("dpt");
  TH2* npt = (TH2F*)gDirectory->Get("npt");


  dpt->FitSlicesY();
  npt->FitSlicesY();
  TH1* dptw = (TH1F*)gDirectory->Get("dpt_2");
  TH1* dpts = (TH1F*)gDirectory->Get("dpt_1");
  TH1* nptw = (TH1F*)gDirectory->Get("npt_2");
  TH1 *npts = (TH1F*)gDirectory->Get("npt_1");
  
  dt->Draw("dca3dxy:gpt>>ddcax(60,0,20,50,-0.01,0.01)","quality<10","colz");
  nt->Draw("dca3dxy:gpt>>ndcax(60,0,20,50,-0.01,0.01)","quality<10","colz");
  dt->Draw("dca3dz:gpt>>ddcaz(60,0,20,50,-0.01,0.01)","quality<10","colz");
  nt->Draw("dca3dz:gpt>>ndcaz(60,0,20,50,-0.01,0.01)","quality<10","colz");
  
  dt->Draw("ntpc:gpt>>dntpc(20,0,20,60,0,60)","quality<10","colz");
  dt->Draw("nintt:gpt>>dnintt(20,0,20,8,0,8)","quality<10","colz");
  dt->Draw("nmaps:gpt>>dnmaps(20,0,20,5,0,5)","quality<10","colz");
  nt->Draw("ntpc:gpt>>nntpc(20,0,20,60,0,60)","quality<10","colz");
  nt->Draw("nintt:gpt>>nnintt(20,0,20,8,0,8)","quality<10","colz");
  nt->Draw("nmaps:gpt>>nnmaps(20,0,20,5,0,5)","quality<10","colz");
  
  dt->Draw("ntpc>>dntpc1(60,0,60)","quality<10","colz");
  dt->Draw("nintt>>dnintt1(6,0,6)","quality<10","colz");
  dt->Draw("nmaps>>dnmaps1(6,0,6)","quality<10","colz");
  nt->Draw("ntpc>>nntpc1(60,0,60)","quality<10","colz");
  nt->Draw("nintt>>nnintt1(6,0,6)","quality<10","colz");
  nt->Draw("nmaps>>nnmaps1(6,0,6)","quality<10","colz");
  
  TH1 *dntpc1 = (TH1F*)gDirectory->Get("dntpc1");
  TH1 *dnintt1 = (TH1F*)gDirectory->Get("dnintt1");
  TH1 *dnmaps1 = (TH1F*)gDirectory->Get("dnmaps1");
  TH1 *nntpc1 = (TH1F*)gDirectory->Get("nntpc1");
  TH1 *nnintt1 = (TH1F*)gDirectory->Get("nnintt1");
  TH1 *nnmaps1 = (TH1F*)gDirectory->Get("nnmaps1");

  TH2 *dntpc = (TH2F*)gDirectory->Get("dntpc");
  TH2 *dnintt = (TH2F*)gDirectory->Get("dnintt");
  TH2 *dnmaps = (TH2F*)gDirectory->Get("dnmaps");
  TH2 *nntpc = (TH2F*)gDirectory->Get("nntpc");
  TH2 *nnintt = (TH2F*)gDirectory->Get("nnintt");
  TH2 *nnmaps = (TH2F*)gDirectory->Get("nnmaps");

  TCanvas *tpc1can = new TCanvas("tpc1can","tpc1can",200,200,600,600);
  nntpc1->SetLineColor(kRed);
  dntpc1->Scale(1./dntpc1->GetEntries());
  nntpc1->Scale(1./nntpc1->GetEntries());
  dntpc1->Draw("hist");
  nntpc1->Draw("histsame");
  
  TCanvas *intt1can = new TCanvas("intt1can","intt1can",200,200,600,600);
  nnintt1->SetLineColor(kRed);
  dnintt1->Scale(1./dnintt1->GetEntries());
  nnintt1->Scale(1./nnintt1->GetEntries());
  dnintt1->Draw("hist");
  nnintt1->Draw("histsame");

  TCanvas *maps1can = new TCanvas("maps1can","maps1can",200,200,600,600);
  nnmaps1->SetLineColor(kRed);
  dnmaps1->Scale(1./dnmaps1->GetEntries());
  nnmaps1->Scale(1./nnmaps1->GetEntries());
  dnmaps1->Draw("hist");
  nnmaps1->Draw("histsame");

  TCanvas *tpccan = new TCanvas("tpccan","tpccan",200,200,600,600);
  tpccan->Divide(2,1);
  tpccan->cd(1);
  dntpc->GetYaxis()->SetTitle("ntpc");
  dntpc->GetXaxis()->SetTitle("gpt");
  dntpc->Draw("colz");
  tpccan->cd(2);
  nntpc->Draw("colz");

  TCanvas *inttcan = new TCanvas("inttcan","inttcan",200,200,600,600);
  inttcan->Divide(2,1);
  inttcan->cd(1);
  dnintt->GetYaxis()->SetTitle("nintt");
  dnintt->GetXaxis()->SetTitle("gpt");
  dnintt->Draw("colz");
  inttcan->cd(2);
  nnintt->Draw("colz");

  TCanvas *mapscan = new TCanvas("mapscan","mapscan",200,200,600,600);
  mapscan->Divide(2,1);
  mapscan->cd(1);
  dnmaps->GetYaxis()->SetTitle("nmaps");
  dnmaps->GetXaxis()->SetTitle("gpt");
  dnmaps->Draw("colz");
  mapscan->cd(2);
  nnmaps->Draw("colz");

  TH2 *ddcax = (TH2F*)gDirectory->Get("ddcax");
  TH2 *ddcaz = (TH2F*)gDirectory->Get("ddcaz");
  TH2 *ndcax = (TH2F*)gDirectory->Get("ndcax");
  TH2 *ndcaz = (TH2F*)gDirectory->Get("ndcaz");
  
  ddcax->FitSlicesY();
  ddcaz->FitSlicesY();
  ndcax->FitSlicesY();
  ndcaz->FitSlicesY();
  
  TH1 *ddcaxw = (TH1F*)gDirectory->Get("ddcax_2");
  TH1 *ddcazw = (TH1F*)gDirectory->Get("ddcaz_2");
  TH1 *ndcaxw = (TH1F*)gDirectory->Get("ndcax_2");
  TH1 *ndcazw = (TH1F*)gDirectory->Get("ndcaz_2");
  
  nptw->SetMarkerColor(kRed); nptw->SetLineColor(kRed);
  npts->SetMarkerColor(kRed); npts->SetLineColor(kRed);
  ndcaxw->SetMarkerColor(kRed); ndcaxw->SetLineColor(kRed);
  ndcazw->SetMarkerColor(kRed); ndcazw->SetLineColor(kRed);
  
  TCanvas *ptcan = new TCanvas("ptcan","ptcan",200,200,600,600);
  ptcan->Divide(2,1);
  ptcan->cd(1);
  dpt->Draw("colz");
  myText(0.1,0.03,kBlack,"File 1");
  ptcan->cd(2);
  npt->Draw("colz");
  myText(0.1,0.03,kBlack,"File 2");
  
  TCanvas *dcaxcan2 = new TCanvas("dcaxcan2","dcaxcan2",200,200,600,600);
  dcaxcan2->Divide(2,1);
  dcaxcan2->cd(1);

  ddcax->Draw("colz");
  myText(0.1,0.03,kBlack,"File 1");
  dcaxcan2->cd(2);
  ndcax->Draw("colz");
  myText(0.1,0.03,kBlack,"File 2");

  TCanvas *dcazcan2 = new TCanvas("dcazcan2","dcazcan2",200,200,600,600);
  dcazcan2->Divide(2,1);
  dcazcan2->cd(1);
  ddcaz->Draw("colz");
  myText(0.1,0.03,kBlack,"File 1");
  dcazcan2->cd(2);
  ndcaz->Draw("colz");
  myText(0.1,0.03,kBlack,"File 2");

  TCanvas *ptwcan = new TCanvas("ptwcan","ptwcan",200,200,600,600);
  nptw->GetYaxis()->SetTitle("#sigma(p_{T})/p_{T}");
  nptw->GetXaxis()->SetTitle("p_{T} [GeV]");
  //nptw->Divide(dptw);
  nptw->Draw("ep");
  dptw->Draw("epsame");
  myText(0.21,0.85,kBlack,"File 1");
  myText(0.21,0.77,kRed,"File 2");
  
  
  TCanvas *ptscan = new TCanvas("ptscan","ptscan",200,200,600,600);
  npts->GetYaxis()->SetTitle("#LTp_{T}^{true}/p_{T}^{reco}#GT");
  npts->GetXaxis()->SetTitle("p_{T} [GeV]");
  npts->Draw("ep");
  dpts->Draw("epsame");
  myText(0.21,0.85,kBlack,"File 1");
  myText(0.21,0.77,kRed,"File 2");
 

  TCanvas *dcaxcan = new TCanvas("dcaxcan","dcaxcan",200,200,600,600);
  ndcaxw->GetYaxis()->SetTitle("#sigma(DCA_{xy}) [cm]");
  ndcaxw->GetXaxis()->SetTitle("p_{T} [GeV]");
  ndcaxw->Draw("ep");
  ddcaxw->Draw("epsame");
  myText(0.21,0.85,kBlack,"File 1");
  myText(0.21,0.77,kRed,"File 2");
 

  TCanvas *dcazcan = new TCanvas("dcazcan","dcazcan",200,200,600,600);
  ndcazw->GetYaxis()->SetTitle("#sigma(DCA_{z}) [cm]");
  ndcazw->GetXaxis()->SetTitle("p_{T} [GeV]");
  ndcazw->Draw("ep");
  ddcazw->Draw("epsame");
  myText(0.21,0.85,kBlack,"File 1");
  myText(0.21,0.77,kRed,"File 2");



  dtruth->GetYaxis()->SetTitle("Eff");
  dtruth->GetXaxis()->SetTitle("p_{T} [GeV]");
  dtruth->GetYaxis()->SetRangeUser(0,1);
  TEfficiency *deff = new TEfficiency(*dreco,*dtruth);
  TEfficiency *neff = new TEfficiency(*nreco,*ntruth);
  TEfficiency *deffmaps = new TEfficiency(*drecomaps,*dtruth);
  TEfficiency *neffmaps = new TEfficiency(*nrecomaps,*ntruth);
  

  TCanvas *effcan = new TCanvas("effcan","effcan",200,200,600,600);
  neff->SetMarkerColor(kRed); neff->SetLineColor(kRed);
  neffmaps->SetMarkerColor(kRed); neffmaps->SetLineColor(kRed);
  neffmaps->SetMarkerStyle(24); deffmaps->SetMarkerStyle(24);

  deff->Draw("ap");
  neff->Draw("psame");
  deffmaps->Draw("psame");
  neffmaps->Draw("psame");
  TLegend *effleg = new TLegend(0.4,0.4,0.6,0.7);
  effleg->AddEntry(deff,"File 1 qual<10","P");
  effleg->AddEntry(neff,"File 2 qual<10","p");
  effleg->AddEntry(deffmaps,"File 1 nmaps>2","P");
  effleg->AddEntry(neffmaps,"File 2 nmaps>2","P");
  
  effleg->Draw("same");




  TFile *outfile = new TFile("compoutfile.root","recreate");
  dpt->Write();
  npt->Write();
  outfile->Close();
}
