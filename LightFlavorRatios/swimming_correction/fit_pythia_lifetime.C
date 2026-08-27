void fit_pythia_lifetime()
{
  TFile* f = TFile::Open("output/merged_fout.root");

  TH1F* truth_lifetime = (TH1F*)f->Get("lifetime_labframe");
  std::cout << "h entries " << truth_lifetime->GetEntries() << std::endl;
  RooRealVar tau("tau","tau",0.,0.5);

  RooDataHist tlh("tlh","truth lifetime hist",tau,RooFit::Import(*truth_lifetime));
  float lifetime_guess = 0.26; // [ns]
  RooRealVar k("k","k",-1./lifetime_guess,-1000.,-0.01);
  RooExponential expmodel("expmodel","exponential fit",tau,k);

  RooPlot* frame = tau.frame(RooFit::Title("Truth lifetime"));
  tlh.plotOn(frame);

  expmodel.fitTo(tlh,RooFit::SumW2Error(true));

  std::cout << "fit lifetime: " << -1./k.getVal() << " ns" << std::endl;

  TCanvas* ctruth = new TCanvas("ctruth","ctruth",600,600);
  expmodel.plotOn(frame);
  frame->Draw();


  TH1F* truth_smeared = (TH1F*)f->Get("lifetime_smeared");
  RooDataHist lsh("lsh","smeared lifetime hist",tau,RooFit::Import(*truth_smeared));

  RooPlot* sm_frame = tau.frame(RooFit::Title("Resolution-smeared lifetime"));
  lsh.plotOn(sm_frame);

  RooRealVar gmean("gmean","gmean",0.02,0.,0.1);
  RooRealVar gsigma("gsigma","gsigma",0.02,0.,0.1);
  RooRealVar lt("lt","lifetime",0.26,0.,10.);
  RooRealVar ltd("ltd","dlifetime",0.26,0.2,0.3);

  RooGExpModel gm("gm","gm",tau,gmean,gsigma,lt,RooFit::RooConst(1.),RooFit::RooConst(1.),RooFit::RooConst(1.),true);
  RooDecay dc("dc","dc",tau,ltd,gm,RooDecay::DecayType::DoubleSided);

  TCanvas* cmod = new TCanvas("cmod","cmod",600,600);
  dc.fitTo(lsh,RooFit::SumW2Error(true));
  dc.plotOn(sm_frame);
  sm_frame->Draw();

  TH1F* t_res = (TH1F*)f->Get("lifetime_smear_amount");
  RooRealVar dt("dt","dt",-0.05,0.05);
  RooDataHist lres("lres","lifetime resolution",dt,RooFit::Import(*t_res));

  RooRealVar rmean("rmean","rmean",0.,-0.1,0.1);
  RooRealVar rsigma("rmean","rmean",0.,-0.1,0.1);
  RooRealVar rk("rk","rk",0.,-100.,100.);
  RooGExpModel gmr("gmr","gmr",dt,rmean,rsigma,rk,RooFit::RooConst(1.),RooFit::RooConst(1.),RooFit::RooConst(1.),true);
  gmr.fitTo(lres);

  TCanvas* cres = new TCanvas("cres","cres",600,600);
  RooPlot* res_frame = dt.frame(RooFit::Title("lifetime resolution"));
  lres.plotOn(res_frame);
  gmr.plotOn(res_frame);
  res_frame->Draw();
 

/*
  RooRealVar gmean("gmean","gmean",0.06,0.04,0.08);
  RooRealVar gsigma("gsigma","gsigma",0.08,0.04,0.12);
  RooRealVar lt("lt","lifetime",0.05,0.,1.);
  RooGExpModel gexpmodel("gexpmodel","Exponential-modified Gaussian model",tau,gmean,gsigma,lt,RooFit::RooConst(1.),RooFit::RooConst(1.),RooFit::RooConst(1.),true);

  gexpmodel.fitTo(lsh,RooFit::SumW2Error(true));
  gexpmodel.plotOn(sm_frame);
*/
  //sm_frame->Draw();
}
