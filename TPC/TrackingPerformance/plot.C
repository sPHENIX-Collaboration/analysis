void NormByBinWidth(TH1F *inh) {
  for(int i=0; i!=inh->GetXaxis()->GetNbins(); ++i) {
    float bef = inh->GetBinContent( i+1 );
    float err = inh->GetBinError( i+1 );
    float bw = inh->GetXaxis()->GetBinWidth( i+1 );
    inh->SetBinContent( i+1, bef/bw );
    inh->SetBinError( i+1, err/bw );
  }
}

TH1F* getHist(TString nom, TFile *fi, const char *na, int co, int mk=20, float mks=1) {
  TH1F *ori = (TH1F*) fi->Get(na);
  TH1F *ret = (TH1F*) ori->Clone( nom.Data() );
  ret->SetLineColor( co );
  ret->SetMarkerColor( co );
  ret->SetMarkerStyle( mk );
  ret->SetMarkerSize( mks );
  ret->Sumw2();
  return ret;
}

TH2F* getHist2(TString nom, TFile *fi, const char *na) {
  TH2F *ori = (TH2F*) fi->Get(na);
  TH2F *ret = (TH2F*) ori->Clone( nom.Data() );
  ret->Sumw2();
  return ret;
}

TCanvas* MakeEfficiency(TFile *fa, TString ti, float c0=0.01250, float c1=0.001858, bool plotOld=false) {
  TCanvas *cTN = new TCanvas();
  cTN->SetGridx(1);
  cTN->SetGridy(1);
  TString geem = "Truths2_Pt";
  TString gere = "Truths3_Pt";
  TString re = "Tracks2_ResPt";
  TH1F *fa_GEEM = getHist("EffGenEM", fa, geem.Data(), kBlue-3);
  TH1F *fa_GERE = getHist("EffGenRE", fa, gere.Data(), kGreen-3);
  TH1F *fa_RE = (TH1F*) fa_GEEM->Clone("EffRec");
  fa_RE->Reset();
  TH2F *fa_RE2D = getHist2("EffRec2D", fa, re.Data());
  for(int b=1; b!=fa_RE->GetXaxis()->GetNbins()+1; ++b) {
    float pt = fa_RE->GetXaxis()->GetBinCenter(b);
    float sg = TMath::Sqrt(c0*c0+c1*pt*c1*pt);
    int x = fa_RE2D->GetXaxis()->FindBin( pt );
    int ya = fa_RE2D->GetYaxis()->FindBin( -5*sg );
    int yb = fa_RE2D->GetYaxis()->FindBin( +5*sg );
    float inte = fa_RE2D->Integral(x,x,ya,yb);
    cout << x << " " << ya << " " << yb << " " << inte << endl;
    fa_RE->SetBinContent(b,inte);
  }
  TGraphAsymmErrors *eff1 = new TGraphAsymmErrors();
  TGraphAsymmErrors *eff2 = new TGraphAsymmErrors();
  eff1->Divide( fa_RE, fa_GEEM );
  eff2->Divide( fa_RE, fa_GERE );
  eff1->SetLineColor( kBlue-3 );
  eff1->SetMarkerColor( kBlue-3 );
  eff1->SetMarkerStyle( 20 );
  eff2->SetLineColor( kGreen-3 );
  eff2->SetMarkerColor( kGreen-3 );
  eff2->SetMarkerStyle( 20 );

  TH2F *axis = new TH2F("axis",";p_{T}  [GeV];Efficiency",10,0.15,36.1,10,0,1.05);
  axis->Draw();
  axis->GetXaxis()->SetMoreLogLabels();
  if(plotOld) eff1->Draw( "PSAME" );
  eff2->Draw( "PSAME" );

  if(0) { //debug
    fa_GE->Draw();
    fa_RE->Draw("SAME");
    TLegend *leg = new TLegend(0.6,0.6,0.9,0.9);
    leg->AddEntry(fa_GE, Form("GEN %.0f",fa_GE->GetEntries()) );
    leg->AddEntry(fa_RE, Form("REC %.0f",fa_RE->GetEntries()) );
    leg->Draw();
  }
  return cTN;
}

TCanvas* MakeEfficiencyFast(TFile *fa, TString ti, TString hw, bool plotOld=false) {
  TCanvas *cTN = new TCanvas();
  cTN->SetGridx(1);
  cTN->SetGridy(1);
  TString geem = Form("Truths2%s",hw.Data());
  TString gere = Form("Truths3%s",hw.Data());
  TString re = Form("Truths4%s",hw.Data());
  TH1F *fa_RE = getHist("EffRec", fa, re.Data(), kOrange);
  TH1F *fa_GEEM = getHist("EffGenEM", fa, geem.Data(), kRed-3);
  TH1F *fa_GERE = getHist("EffGenRE", fa, gere.Data(), kBlue-3);
  TGraphAsymmErrors *eff1 = new TGraphAsymmErrors();
  TGraphAsymmErrors *eff2 = new TGraphAsymmErrors();
  eff1->Divide( fa_RE, fa_GEEM );
  eff2->Divide( fa_RE, fa_GERE );
  eff1->SetLineColor( kBlue-3 );
  eff1->SetMarkerColor( kBlue-3 );
  eff1->SetMarkerStyle( 20 );
  eff2->SetLineColor( kOrange-3 );
  eff2->SetMarkerColor( kOrange-3 );
  eff2->SetMarkerStyle( 20 );

  TH2F *axis = new TH2F("axis",";p_{T}  [GeV];Efficiency",10,0.15,36.1,10,0,1.05);
  axis->Draw();
  axis->GetXaxis()->SetMoreLogLabels();
  if(plotOld) eff1->Draw( "PSAME" );
  eff2->Draw( "PSAME" );
  return cTN;
}

TCanvas* MakePtResolution(TFile *fa, TString ti, bool showBoth=false) {
  TString re1 = "Tracks2_ResPt"; //dpt/pt
  TString re2 = "Tracks2_Res2Pt"; //dpt/pt
  TH2F *fa_RE1 = getHist2("EffRec1", fa, re1.Data());
  TH2F *fa_RE2 = getHist2("EffRec2", fa, re2.Data());

  TCanvas *qa = new TCanvas();
  qa->Divide(5,5);
  TCanvas *qb = new TCanvas();
  qb->Divide(5,5);
  const int nbx1 = fa_RE1->GetXaxis()->GetNbins();
  float x1[60], ex1[60];
  float mu1[60], emu1[60], sg1[60], esg1[60];
  float mu2[60], emu2[60], sg2[60], esg2[60];
  for( int b=0; b!=nbx1; ++b) {
    x1[b] = fa_RE1->GetXaxis()->GetBinCenter(b+1);
    ex1[b] = fa_RE1->GetXaxis()->GetBinWidth(b+1)/2.0;
    TH1D *tmp;
    TF1 *fun;

    tmp = (TH1D*) fa_RE1->ProjectionY( Form("bin1_%d",b),b+1,b+1 );
    if( tmp->GetEntries() < 30 ) continue;
    if(b<25) qa->cd(b+1);
    tmp->GetXaxis()->SetRangeUser(-0.2,+0.2);
    tmp->Fit("gaus","RWQI","",-0.5,+0.5);
    fun = (TF1*) tmp->GetListOfFunctions()->At(0);
    mu1[b] = fun->GetParameter(1);
    emu1[b] = fun->GetParError(1);
    sg1[b] = fun->GetParameter(2);
    esg1[b] = fun->GetParError(2);

    tmp = (TH1D*) fa_RE2->ProjectionY( Form("bin2_%d",b),b+1,b+1 );
    if( tmp->GetEntries() < 30 ) continue;
    if(b<25) qb->cd(b+1);
    tmp->GetXaxis()->SetRangeUser(-0.2,+0.2);
    tmp->Fit("gaus","RWQI","",-0.5,+0.5);
    fun = (TF1*) tmp->GetListOfFunctions()->At(0);
    mu2[b] = fun->GetParameter(1)/x1[b];
    emu2[b] = fun->GetParError(1)/x1[b];
    sg2[b] = fun->GetParameter(2)/x1[b];
    esg2[b] = fun->GetParError(2)/x1[b];
    //cout << mu1[b] << " " << emu1[b] << " || " << sg1[b] << " " << esg1[b] << endl;
  }

  TCanvas *cTN = new TCanvas();
  cTN->SetGridx(1);
  cTN->SetGridy(1);

  TH2F *axis = new TH2F("axisPtRes",";p_{T}  [GeV]",10,0.15,36.1,10,-0.01,0.09);
  axis->Draw();

  TGraphErrors *gRes1 = new TGraphErrors(nbx1,x1,sg1,ex1,esg1);
  gRes1->Draw("PSAME");
  gRes1->SetLineColor(kRed-3);
  gRes1->SetMarkerColor(kRed-3);
  gRes1->SetMarkerStyle(20);

  TGraphErrors *gRes1M = new TGraphErrors(nbx1,x1,mu1,ex1,emu1);
  gRes1M->Draw("PSAME");
  gRes1M->SetLineColor(kRed-3);
  gRes1M->SetMarkerColor(kRed-3);
  gRes1M->SetMarkerStyle(24);

  TGraphErrors *gRes2 = new TGraphErrors(20,x1,sg2,ex1,esg2);
  if(showBoth) gRes2->Draw("PSAME");
  gRes2->SetLineColor(kBlue-3);
  gRes2->SetMarkerColor(kBlue-3);
  gRes2->SetMarkerStyle(20);

  TGraphErrors *gRes2M = new TGraphErrors(20,x1,mu2,ex1,emu2);
  if(showBoth) gRes2M->Draw("PSAME");
  gRes2M->SetLineColor(kBlue-3);
  gRes2M->SetMarkerColor(kBlue-3);
  gRes2M->SetMarkerStyle(24);

  TF1 *fun = new TF1("fun","TMath::Sqrt([0]*[0]+[1]*x*[1]*x)",0.2,36);
  //fun->SetParameter(0,0.01); fun->SetParLimits(0,0.001,0.02);
  fun->SetParameter(0,1.23e-2); fun->SetParLimits(0,+1,-1);
  fun->SetParameter(1,0.01); fun->SetParLimits(1,0.001,0.02);
  fun->SetLineColor(kOrange-3);
  gRes1->Fit(fun,"R","",0.2,36);

  gRes1->SetFillColor(kWhite);
  gRes2->SetFillColor(kWhite);
  gRes1M->SetFillColor(kWhite);
  gRes2M->SetFillColor(kWhite);

  TLegend *leg = new TLegend(0.1,0.7,0.4,0.9);
  leg->AddEntry(gRes1,"p_{T} Res #sigma_{1}");
  if(showBoth) leg->AddEntry(gRes2,"p_{T} Res #sigma_{0}");
  leg->AddEntry(gRes1M,"p_{T} Res #mu_{1}");
  if(showBoth) leg->AddEntry(gRes2M,"p_{T} Res #mu_{0}");
  leg->Draw();
  return cTN;
}

TCanvas* MakeDCA2DResolution(TFile *fa, TString ti) {
  TString re1 = "Tracks2_Dca2D";
  TH2F *fa_RE1 = getHist2("Dca2DRec1", fa, re1.Data());

  TCanvas *qa = new TCanvas();
  qa->Divide(5,5);
  const int nbx1 = fa_RE1->GetXaxis()->GetNbins();
  float x1[60], ex1[60];
  float mu1[60], emu1[60], sg1[60], esg1[60];
  for( int b=0; b!=nbx1; ++b) {
    x1[b] = fa_RE1->GetXaxis()->GetBinCenter(b+1);
    ex1[b] = fa_RE1->GetXaxis()->GetBinWidth(b+1)/2.0;
    TH1D *tmp;
    TF1 *fun;

    tmp = (TH1D*) fa_RE1->ProjectionY( Form("dca2d_bin1_%d",b),b+1,b+1 );
    if( tmp->GetEntries() < 30 ) continue;
    if(b<25) qa->cd(b+1);
    tmp->Fit("gaus","RWQI","",-0.5,+0.5);
    fun = (TF1*) tmp->GetListOfFunctions()->At(0);
    mu1[b] = fun->GetParameter(1)*1e4;
    emu1[b] = fun->GetParError(1)*1e4;
    sg1[b] = fun->GetParameter(2)*1e4;
    esg1[b] = fun->GetParError(2)*1e4;
  }

  TCanvas *cTN = new TCanvas();
  cTN->SetGridx(1);
  cTN->SetGridy(1);

  TH2F *axis = new TH2F("axisDCA2DRes",";p_{T}^{reco}  [GeV];[#mu m]",10,0.15,36.1,10,-10,+80);
  axis->Draw();

  TGraphErrors *gRes1 = new TGraphErrors(nbx1,x1,sg1,ex1,esg1);
  gRes1->Draw("PSAME");
  gRes1->SetLineColor(kRed-3);
  gRes1->SetMarkerColor(kRed-3);
  gRes1->SetMarkerStyle(20);

  TGraphErrors *gRes1M = new TGraphErrors(nbx1,x1,mu1,ex1,emu1);
  gRes1M->Draw("PSAME");
  gRes1M->SetLineColor(kRed-3);
  gRes1M->SetMarkerColor(kRed-3);
  gRes1M->SetMarkerStyle(24);

  gRes1->SetFillColor(kWhite);
  gRes1M->SetFillColor(kWhite);

  TLegend *leg = new TLegend(0.55,0.7,0.9,0.9);
  leg->AddEntry(gRes1,"DCA2D Gaus #sigma");
  leg->AddEntry(gRes1M,"DCA2D Gaus #mu");
  leg->Draw();
  return cTN;
}

TCanvas* MakeTH1F(TFile *fa,TString ti,TString wh,TString hw, bool norm=true, bool bw=false, bool log=false) {
  TCanvas *cTN = new TCanvas();
  TString c0 = Form("%s0%s",wh.Data(),hw.Data());
  TString c1 = Form("%s1%s",wh.Data(),hw.Data());
  TString c2 = Form("%s2%s",wh.Data(),hw.Data());
  TString c3 = Form("%s3%s",wh.Data(),hw.Data());
  TH1F *fa_T0N = getHist(Form("%s0",ti.Data()), fa, c0.Data(), kBlack,   20, 1.3);
  TH1F *fa_T1N = getHist(Form("%s1",ti.Data()), fa, c1.Data(), kRed-3,   20, 1.3);
  TH1F *fa_T2N = getHist(Form("%s2",ti.Data()), fa, c2.Data(), kBlue-3,  20, 1.3);
  TH1F *fa_T3N = getHist(Form("%s3",ti.Data()), fa, c3.Data(), kGreen-3, 20, 1.3);
  if(norm) {
    int evA = ((TH1F*) fa->Get("Events"))->GetBinContent(1);
    fa_T0N->Scale(1.0/evA);
    fa_T1N->Scale(1.0/evA);
    fa_T2N->Scale(1.0/evA);
    fa_T3N->Scale(1.0/evA);
  }
  if(bw) {
    NormByBinWidth(fa_T0N);
    NormByBinWidth(fa_T1N);
    NormByBinWidth(fa_T2N);
    NormByBinWidth(fa_T3N);
  }
  if(log) cTN->SetLogy(1);
  fa_T0N->Draw();
  fa_T1N->Draw("SAME");
  fa_T2N->Draw("SAME");
  fa_T3N->Draw("SAME");
  TLegend *leg = new TLegend(0.6,0.6,0.9,0.9);
  leg->AddEntry(fa_T0N,fa_T0N->GetTitle());
  leg->AddEntry(fa_T1N,fa_T1N->GetTitle());
  leg->AddEntry(fa_T2N,fa_T2N->GetTitle());
  leg->AddEntry(fa_T3N,fa_T3N->GetTitle());
  leg->Draw();
  fa_T0N->SetTitle( ti.Data() );
  return cTN;
}

void plot() {
  gStyle->SetOptStat(0);
  //TString file = "newTPCgeo";
  TString file = "oldUpdate";

  TFile *fa = new TFile( Form("%s.root",file.Data()) );

  fa->ls();
  MakeTH1F(fa,"Eta","Truths","_Eta",true,false,true)->Draw();
  MakeTH1F(fa,"Pt [GeV]","Truths","_Pt",true,true,true)->Draw();

  MakeTH1F(fa,"Eta","Tracks","_Eta",true,false,true)->Draw();
  MakeTH1F(fa,"Pt [GeV]","Tracks","_Pt",true,true,true)->Draw();

  TCanvas *cres = MakePtResolution(fa,"PtResolution");
  TCanvas *cdca = MakeDCA2DResolution(fa,"DCA2DResolution");
  //TCanvas *cef1 = MakeEfficiency(fa,"Efficiency",1.23e-2,2.17e-03);
  TCanvas *cef1 = MakeEfficiency(fa,"Efficiency",1.23e-2,1.85e-3);
  TCanvas *cef2 = MakeEfficiencyFast(fa,"Efficiency","_Pt");

  cres->SaveAs( Form("%s_cres.png",file.Data()), "png"); cres->Draw();
  cdca->SaveAs( Form("%s_cdca.png",file.Data()), "png"); cdca->Draw();
  cef1->SaveAs( Form("%s_cef1.png",file.Data()), "png"); cef1->Draw();
  cef2->SaveAs( Form("%s_cef2.png",file.Data()), "png"); cef2->Draw();
}
