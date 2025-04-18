draw_phenix(float an)
{
  const int nn=1;
  float xx[nn] = {4.4};
  float sx[nn] = {2.6};
  float yy[nn] = {3.14e-2};
  //  float yy[nn] = {1e-2};
  float sy[nn] = {0.37e-2};

  yy[0] = an;

  TGraphErrors* gr = new TGraphErrors(nn,xx,yy,sx,sy);
  gr->SetFillColor(3);
  gr->SetFillStyle(3001);
  //  gr->Draw("A2");
  //  gr->Draw("P");

  int icol = 29;
  int iw = 10; // Line width
  float wa = 0.025; // Arrow width
  TArrow *arx = new TArrow(xx[0]-sx[0],yy[0],xx[0]+sx[0],yy[0],wa,"<|>");
  arx->SetLineWidth(iw);
  arx->SetLineColor(icol);
  arx->SetFillColor(icol);
  arx->Draw();
  
  TArrow *ary = new TArrow(xx[0],yy[0]-sy[0],xx[0],yy[0]+sy[0],wa,"<|>");
  ary->SetLineWidth(iw);
  ary->SetLineColor(icol);
  ary->SetFillColor(icol);
  ary->Draw();
}


han()
{
  const int nxf = 20;
  TFile* f = new TFile("han.root");
  TGraphErrors* gan[nxf];

  const float dy = 0.01; // to plot different xF bins

  char hname[100];
  for( int ix=0; ix<nxf; ix++ ) {
    sprintf(hname,"gan%d",ix);
    gan[ix] = (TGraphErrors*)f->Get(hname);
  }

  gStyle->SetOptStat(0);
  c3 = new TCanvas("c3","The Ntuple canvas",200,200,500,450);
  c3->Range(0,0,1,1);
  pad31 = new TPad("pad31","This is pad1",0.0,0.0,1.0,1.0);
  pad31->Draw();

  TH1F* h = new TH1F("h"," ",10,0,11);
  //  h->SetTitle("#pi^{+} #delta(A_{N}) proj.");
  h->SetYTitle("A_{N}");
  h->SetXTitle("p_{T} (GeV/c)");
  h->SetMinimum(0.);
  h->SetMaximum(0.053);

  pad31->cd();
  h->Draw();
  draw_phenix(0.025);
  TLatex *tex = new TLatex();
  tex->SetTextSize(0.035);
  char str[100];
  float xfbin = 1./float(nxf);
  for( int i=1; i<=4; i++ ) {
    gan[i]->SetMarkerStyle(20);
    gan[i]->SetMarkerColor(2);
    gan[i]->SetLineColor(2);
    gan[i]->Draw("P");
    sprintf(str,"x_{F}=%4.2f-%4.2f",xfbin*i,xfbin*(i+1));
    tex->DrawLatex(8.5,dy*i-0.0003,str);
  }

  tex->DrawLatex(0.4,0.048,"sPHENIX:");
  tex->DrawLatex(0.4,0.044,"p+p, #sqrt{s}=200 GeV");
  tex->DrawLatex(0.4,0.040,"L=10/pb P=0.57");
  //  tex->DrawLatex(0.4,0.0265,"0<#eta<3");
  tex->DrawLatex(0.4,0.0025,"Shown only points with #delta(A_{N})<1%");

}
