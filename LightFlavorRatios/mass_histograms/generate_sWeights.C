void generate_sWeights()
{
  TFile* Ks_file = TFile::Open("Kshort_3runs.root");
  TFile* lambda_file = TFile::Open("Lambda_3runs.root");

  TTree* Ks_tree = (TTree*)Ks_file->Get("DecayTree");
  TTree* lambda_tree = (TTree*)lambda_file->Get("DecayTree");

  TFile* Ks_SWfile = new TFile("Kshort_SW_3runs.root","RECREATE");
  TTree* Ks_SWtree = Ks_tree->CloneTree(-1);

  TFile* lambda_SWfile = new TFile("lambda_SW_3runs.root","RECREATE");
  TTree* lambda_SWtree = lambda_tree->CopyTree("Lambda0_mass>=1.1&&Lambda0_mass<1.13");

  RooRealVar m_ks("K_S0_mass","K_S0_mass",0.45,0.55);
  RooRealVar m_lambda("Lambda0_mass","Lambda0_mass",1.1,1.13);

  RooDataSet Ks_ds("K_S0","K_S0",RooArgSet(m_ks),RooFit::Import(*Ks_tree));
  RooDataSet lambda_ds("Lambda0","Lambda0",RooArgSet(m_lambda),RooFit::Import(*lambda_tree));
  
  // Kshort signal and background models
  RooRealVar ks_mean("ks_mean","mean",0.49,0.48,0.5);
  RooRealVar ks_width("ks_width","width",0.01,0.0001,0.2);
  RooGaussian ks_signal("ks_signal","signal",m_ks,ks_mean,ks_width);

  RooRealVar ks_lin_coef("ks_lin_coef","lin_coef",0.,0.,1.);
  RooRealVar ks_quad_coef("ks_quad_coef","quad_coef",0.,0.,1.);
  RooRealVar ks_cubic_coef("ks_cubic_coef","cubic_coef",0.,0.,1.);
  RooRealVar ks_quartic_coef("ks_quartic_coef","quartic_coef",0.,0.,1.);
  RooChebychev ks_bkg("ks_bkg","background",m_ks,RooArgList(ks_lin_coef,ks_quad_coef,ks_cubic_coef,ks_quartic_coef));

  RooRealVar ks_nsignal("ks_nsignal","nSignal",0.,2*Ks_ds.numEntries());
  RooRealVar ks_nbkg("ks_nbkg","nBackground",0.,2*Ks_ds.numEntries());
  RooAddPdf ks_model("ks_model","combined signal and background",RooArgList(ks_signal,ks_bkg),RooArgList(ks_nsignal,ks_nbkg));

  // Lambda signal and background models
  RooRealVar lambda_mean("lambda_mean","mean",1.113,1.11,1.12);
  RooRealVar lambda_width("lambda_width","width",0.01,0.0001,0.2);
  RooGaussian lambda_signal("lambda_signal","signal",m_lambda,lambda_mean,lambda_width);

  RooRealVar lambda_lin_coef("lambda_lin_coef","lin_coef",0.,0.,1.);
  RooRealVar lambda_quad_coef("lambda_quad_coef","quad_coef",0.,0.,1.);
  RooRealVar lambda_cubic_coef("lambda_cubic_coef","cubic_coef",0.,0.,1.);
  RooRealVar lambda_quartic_coef("lambda_quartic_coef","quartic_coef",0.,0.,1.);
  RooChebychev lambda_bkg("lambda_bkg","background",m_lambda,RooArgList(lambda_lin_coef,lambda_quad_coef,lambda_cubic_coef));

  RooRealVar lambda_nsignal("lambda_nsignal","nSignal",0.,2*Ks_ds.numEntries());
  RooRealVar lambda_nbkg("lambda_nbkg","nBackground",0.,2*Ks_ds.numEntries());
  RooAddPdf lambda_model("lambda_model","combined signal and background",RooArgList(lambda_signal,lambda_bkg),RooArgList(lambda_nsignal,lambda_nbkg));

  ks_model.fitTo(Ks_ds);
  lambda_model.fitTo(lambda_ds);

  RooStats::SPlot* Ks_sData = new RooStats::SPlot("Ks_sData","Ks_sPlot",Ks_ds,&ks_model,RooArgList(ks_nsignal,ks_nbkg));
  RooStats::SPlot* lambda_sData = new RooStats::SPlot("lambda_sData","lambda_sPlot",lambda_ds,&lambda_model,RooArgList(lambda_nsignal,lambda_nbkg));

  double ks_SW;
  double lambda_SW;

  TBranch* Ks_SWbranch = Ks_SWtree->Branch("sWeight",&ks_SW,"sWeight/D");
  TBranch* lambda_SWbranch = lambda_SWtree->Branch("sWeight",&lambda_SW,"sWeight/D");

  std::cout << "Check SWeights:" << std::endl;

  std::cout << "Yield of signal is "  << ks_nsignal.getVal() << ", " << lambda_nsignal.getVal()
              << ".  From sWeights it is " << Ks_sData->GetYieldFromSWeight("ks_nsignal") << ", " << lambda_sData->GetYieldFromSWeight("lambda_nsignal") << std::endl;

  for(int i=0; i<Ks_ds.numEntries(); i++)
  {
    const RooArgSet* row = Ks_ds.get(i);
    ks_SW = (double)row->getRealValue("ks_nsignal_sw");
    Ks_SWbranch->Fill();
  }

  std::cout << "Kshort done" << std::endl;

  for(int i=0; i<lambda_ds.numEntries(); i++)
  {
    const RooArgSet* row = lambda_ds.get(i);
    lambda_SW = (double)row->getRealValue("lambda_nsignal_sw");
    lambda_SWbranch->Fill();
  }

  Ks_SWfile->Write();
  Ks_SWfile->Close();

  lambda_SWfile->Write();
  lambda_SWfile->Close();
}
