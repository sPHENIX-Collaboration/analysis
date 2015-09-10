
TH1D* get_momentum_shift(std::string filename) {

  TFile* file = TFile::Open(filename.c_str());
  file->cd();  
  
  TH1D* output = new TH1D("mom_shift","mom_shift",
			  truept_dptoverpt->GetXaxis()->GetNbins(),
			  truept_dptoverpt->GetXaxis()->GetXmin(),
			  truept_dptoverpt->GetXaxis()->GetXmax());

  for (int i=1; i<=truept_dptoverpt->GetXaxis()->GetNbins(); ++i) {

    TH1D* strip = new TH1D("strip","strip",
			   truept_dptoverpt->GetYaxis()->GetNbins(),
			   truept_dptoverpt->GetYaxis()->GetXmin(),
			   truept_dptoverpt->GetYaxis()->GetXmax());
    
    for (int j=1; j<=truept_dptoverpt->GetYaxis()->GetNbins(); ++j) {
      strip->SetBinContent(j,truept_dptoverpt->GetBinContent(i,j));
      strip->SetBinError(j,truept_dptoverpt->GetBinError(i,j));
    }

    output->SetBinContent(i,strip->GetMean());
    output->SetBinError(i,strip->GetMeanError());

    delete strip;
  }

  return output;
}

TH1D* get_momentum_reso(std::string filename) {

  TFile* file = TFile::Open(filename.c_str());
  file->cd();  
  
  TH1D* output = new TH1D("mom_reso","mom_reso",
			  truept_dptoverpt->GetXaxis()->GetNbins(),
			  truept_dptoverpt->GetXaxis()->GetXmin(),
			  truept_dptoverpt->GetXaxis()->GetXmax());

  for (int i=1; i<=truept_dptoverpt->GetXaxis()->GetNbins(); ++i) {

    TH1D* strip = new TH1D("strip","strip",
			   truept_dptoverpt->GetYaxis()->GetNbins(),
			   truept_dptoverpt->GetYaxis()->GetXmin(),
			   truept_dptoverpt->GetYaxis()->GetXmax());
    
    for (int j=1; j<=truept_dptoverpt->GetYaxis()->GetNbins(); ++j) {
      strip->SetBinContent(j,truept_dptoverpt->GetBinContent(i,j));
      strip->SetBinError(j,truept_dptoverpt->GetBinError(i,j));
    }

    output->SetBinContent(i,strip->GetRMS());
    output->SetBinError(i,strip->GetRMSError());

    delete strip;
  }

  return output;
}

TH1D* get_dca_shift(std::string filename) {

  TFile* file = TFile::Open(filename.c_str());
  file->cd();  
  
  TH1D* output = new TH1D("mom_shift","reso_shift",
			  truept_dca->GetXaxis()->GetNbins(),
			  truept_dca->GetXaxis()->GetXmin(),
			  truept_dca->GetXaxis()->GetXmax());

  for (int i=1; i<=truept_dca->GetXaxis()->GetNbins(); ++i) {

    TH1D* strip = new TH1D("strip","strip",
			   truept_dca->GetYaxis()->GetNbins(),
			   truept_dca->GetYaxis()->GetXmin(),
			   truept_dca->GetYaxis()->GetXmax());
    
    for (int j=1; j<=truept_dca->GetYaxis()->GetNbins(); ++j) {
      strip->SetBinContent(j,truept_dca->GetBinContent(i,j));
      strip->SetBinError(j,truept_dca->GetBinError(i,j));
    }

    output->SetBinContent(i,strip->GetMean());
    output->SetBinError(i,strip->GetMeanError());

    delete strip;
  }

  return output;
}

TH1D* get_dca_reso(std::string filename) {

  TFile* file = TFile::Open(filename.c_str());
  file->cd();  
  
  TH1D* output = new TH1D("dca_reso","dca_reso",
			  truept_dca->GetXaxis()->GetNbins(),
			  truept_dca->GetXaxis()->GetXmin(),
			  truept_dca->GetXaxis()->GetXmax());

  for (int i=1; i<=truept_dca->GetXaxis()->GetNbins(); ++i) {

    TH1D* strip = new TH1D("strip","strip",
			   truept_dca->GetYaxis()->GetNbins(),
			   truept_dca->GetYaxis()->GetXmin(),
			   truept_dca->GetYaxis()->GetXmax());
    
    for (int j=1; j<=truept_dca->GetYaxis()->GetNbins(); ++j) {
      strip->SetBinContent(j,truept_dca->GetBinContent(i,j));
      strip->SetBinError(j,truept_dca->GetBinError(i,j));
    }

    output->SetBinContent(i,strip->GetRMS());
    output->SetBinError(i,strip->GetRMSError());

    delete strip;
  }
      
  return output;
}


TH1D* get_pattern_eff_nhit(std::string filename, int option) {

  TFile* file = TFile::Open(filename.c_str());
  file->cd();  

  TH1D* output = NULL;
  if (option == 0) output = (TH1D*) truept_particles_recoWithExactHits->Clone("patterneff0");
  else if (option == 1) output = (TH1D*) truept_particles_recoWithin1Hit->Clone("patterneff1");
  else if (option == 2) output = (TH1D*) truept_particles_recoWithin2Hits->Clone("patterneff2");
  else cout << "illegal option" << endl;

  //output->Sumw2();
  //truept_particles_leaving7Hits->Sumw2();
  
  output->Divide(truept_particles_leaving7Hits);
  
  return output;
}


TH1D* get_pattern_eff_percent(std::string filename, int option) {

  TFile* file = TFile::Open(filename.c_str());
  file->cd();  

  TH1D* output = NULL;
  if (option == 0) output = (TH1D*) truept_particles_recoWithin3Percent->Clone("patterneff0a");
  else if (option == 1) output = (TH1D*) truept_particles_recoWithin4Percent->Clone("patterneff1a");
  else if (option == 2) output = (TH1D*) truept_particles_recoWithin5Percent->Clone("patterneff2a");
  else cout << "illegal option" << endl;

  //output->Sumw2();
  //truept_particles_leaving7Hits->Sumw2();
  
  output->Divide(truept_particles_leaving7Hits);
  
  return output;
}


TH1D* get_purity_nhit(std::string filename, int option) {

  TFile* file = TFile::Open(filename.c_str());
  file->cd();  

  TH1D* output = NULL;
  if (option == 0) output = (TH1D*) recopt_tracks_recoWithExactHits->Clone("purity0");
  else if (option == 1) output = (TH1D*) recopt_tracks_recoWithin1Hit->Clone("purity1");
  else if (option == 2) output = (TH1D*) recopt_tracks_recoWithin2Hits->Clone("purity2");
  else cout << "illegal option" << endl;

  //output->Sumw2();
  //recopt_tracks_all->Sumw2();
  
  output->Divide(recopt_tracks_all);
  
  return output;
}

TH1D* get_purity_percent(std::string filename, int option) {

  TFile* file = TFile::Open(filename.c_str());
  file->cd();  

  TH1D* output = NULL;
  if (option == 0) output = (TH1D*) recopt_tracks_recoWithin3Percent->Clone("purity0");
  else if (option == 1) output = (TH1D*) recopt_tracks_recoWithin4Percent->Clone("purity1");
  else if (option == 2) output = (TH1D*) recopt_tracks_recoWithin5Percent->Clone("purity2");
  else cout << "illegal option" << endl;

  //output->Sumw2();
  //recopt_tracks_all->Sumw2();
  
  output->Divide(recopt_tracks_all);
  
  return output;
}

TH1D* get_quality_mean(std::string filename) {

  TFile* file = TFile::Open(filename.c_str());
  file->cd();  
  
  TH1D* output = new TH1D("quality","quality",
			  recopt_quality->GetXaxis()->GetNbins(),
			  recopt_quality->GetXaxis()->GetXmin(),
			  recopt_quality->GetXaxis()->GetXmax());

  for (int i=1; i<=recopt_quality->GetXaxis()->GetNbins(); ++i) {

    TH1D* strip = new TH1D("strip","strip",
			   recopt_quality->GetYaxis()->GetNbins(),
			   recopt_quality->GetYaxis()->GetXmin(),
			   recopt_quality->GetYaxis()->GetXmax());
    
    for (int j=1; j<=recopt_quality->GetYaxis()->GetNbins(); ++j) {
      strip->SetBinContent(j,recopt_quality->GetBinContent(i,j));
      strip->SetBinError(j,recopt_quality->GetBinError(i,j));
    }

    output->SetBinContent(i,strip->GetMean());
    output->SetBinError(i,strip->GetMeanError());

    delete strip;
  }
      
  return output;
}

TH1D* get_quality_rms(std::string filename) {

  TFile* file = TFile::Open(filename.c_str());
  file->cd();  
  
  TH1D* output = new TH1D("quality_rms","quality_rms",
			  recopt_quality->GetXaxis()->GetNbins(),
			  recopt_quality->GetXaxis()->GetXmin(),
			  recopt_quality->GetXaxis()->GetXmax());

  for (int i=1; i<=recopt_quality->GetXaxis()->GetNbins(); ++i) {

    TH1D* strip = new TH1D("strip","strip",
			   recopt_quality->GetYaxis()->GetNbins(),
			   recopt_quality->GetYaxis()->GetXmin(),
			   recopt_quality->GetYaxis()->GetXmax());
    
    for (int j=1; j<=recopt_quality->GetYaxis()->GetNbins(); ++j) {
      strip->SetBinContent(j,recopt_quality->GetBinContent(i,j));
      strip->SetBinError(j,recopt_quality->GetBinError(i,j));
    }

    output->SetBinContent(i,strip->GetRMS());
    output->SetBinError(i,strip->GetRMSError());

    delete strip;
  }
      
  return output;
}


TH1D* get_vertex(std::string filename, int option) {

  TFile* file = TFile::Open(filename.c_str());
  file->cd();  

  TH1D* output = NULL;
  if (option == 0) output = (TH1D*) dx_vertex->Clone("vertex_x");
  else if (option == 1) output = (TH1D*) dy_vertex->Clone("vertex_y");
  else if (option == 2) output = (TH1D*) dz_vertex->Clone("vertex_z");
  else cout << "illegal option" << endl;
  
  float max = output->GetMaximum();
  output->Scale(1.0/max);
  
  return output;
}

void apply_markup(TPad* pad) {
  pad->SetTopMargin(0.02);
  pad->SetBottomMargin(0.12);
  pad->SetLeftMargin(0.15);
  pad->SetRightMargin(0.02);
}

void apply_markup(TH1D* histo, int option, int kcolor) {

  if (option==0) histo->SetMarkerStyle(24);
  if (option==1) histo->SetMarkerStyle(20);
  if (option==0) histo->SetMarkerSize(1.4);
  if (option==1) histo->SetMarkerSize(1.0);
  histo->SetMarkerColor(kcolor);
  histo->SetLineColor(kcolor);
  histo->GetXaxis()->CenterTitle();
  histo->GetXaxis()->SetTitleSize(0.05);
  histo->GetXaxis()->SetTitleOffset(1.1);
  histo->GetYaxis()->CenterTitle();
  histo->GetYaxis()->SetTitleSize(0.05);
  histo->GetYaxis()->SetTitleOffset(1.55);

  return;
}

void plot1_markup(TH1D* histo, int option=0) {
  histo->GetXaxis()->SetTitle("true p_{T}");
  histo->GetYaxis()->SetTitle("dp_{T} / p_{T}");
  histo->SetMinimum(-0.01);
  histo->SetMaximum(0.06);
  if (option == 1) {
    histo->SetMinimum(0.9);
    histo->SetMaximum(1.1);
  }
}

void plot2_markup(TH1D* histo, int option=0) {
  histo->GetXaxis()->SetTitle("true p_{T}");
  histo->GetYaxis()->SetTitle("DCA");
  histo->SetMinimum(-0.0020);
  histo->SetMaximum(0.0200);
  if (option == 1) {
    histo->SetMinimum(0.9);
    histo->SetMaximum(1.1);
  }
}

void plot3_markup(TH1D* histo, int option=0) {
  histo->GetXaxis()->SetTitle("true p_{T}");
  histo->GetYaxis()->SetTitle("finding efficiency");
  histo->SetMinimum(0.0);
  histo->SetMaximum(1.1);
  if (option == 1) {
    histo->SetMinimum(0.9);
    histo->SetMaximum(1.1);
  }
}

void plot4_markup(TH1D* histo, int option=0) {
  histo->GetXaxis()->SetTitle("true p_{T}");
  histo->GetYaxis()->SetTitle("finding efficiency");
  histo->SetMinimum(0.0);
  histo->SetMaximum(1.1);
  if (option == 1) {
    histo->SetMinimum(0.9);
    histo->SetMaximum(1.1);
  }
}

void plot5_markup(TH1D* histo, int option=0) {
  histo->GetXaxis()->SetTitle("reco p_{T}");
  histo->GetYaxis()->SetTitle("quality");
  histo->SetMinimum(0.0);
  histo->SetMaximum(2.0);
  if (option == 1) {
    histo->SetMinimum(0.9);
    histo->SetMaximum(1.1);
  }
}

void plot6_markup(TH1D* histo, int option=0) {
  histo->GetXaxis()->SetTitle("vertex residual (cm)");
  histo->GetYaxis()->SetTitle("");
  histo->SetMinimum(0.01);
  histo->SetMaximum(2.0);
  if (option == 1) {
    histo->SetMinimum(0.9);
    histo->SetMaximum(1.1);
  }
}

void plot7_markup(TH1D* histo, int option=0) {
  histo->GetXaxis()->SetTitle("reco p_{T}");
  histo->GetYaxis()->SetTitle("purity");
  histo->SetMinimum(0.0);
  histo->SetMaximum(1.1);
  if (option == 1) {
    histo->SetMinimum(0.9);
    histo->SetMaximum(1.1);
  }
}

void plot8_markup(TH1D* histo, int option=0) {
  histo->GetXaxis()->SetTitle("reco p_{T}");
  histo->GetYaxis()->SetTitle("purity");
  histo->SetMinimum(0.0);
  histo->SetMaximum(1.1);
  if (option == 1) {
    histo->SetMinimum(0.9);
    histo->SetMaximum(1.1);
  }
}
