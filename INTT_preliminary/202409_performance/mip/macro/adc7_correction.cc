
// DAC scan (Test beam III): https://indico2.riken.jp/event/4289/contributions/20429/attachments/11760/16995/20230208_sugiyama.pdf
// DAC scan (Au+Au): http://webhepl.cc.nara-wu.ac.jp/old_HP/thesis/master/2023sugiyama/2023sugiyama_master_presentation.pdf
int adc7_correction( int mode = 0)
{
  // mode 0: for test beam, Landau MPV 71.13
  // mode 1: theoritical, Landau MPV 90
  gStyle->SetOptStat( 111111 );
  gStyle->SetOptFit( true );

  int event_num = 1e7;
  double range_max = 650;
  TF1* f = new TF1( "f", "landau", 0, range_max );
  f->SetLineColor( kAzure + 1 );
  f->SetLineStyle( 7 );

  f->SetParameter( 0, 1.0 );
  if( mode == 0 )
    f->FixParameter( 1, 71.13 ); // from test beam experiment
  else if( mode == 1 )
    f->FixParameter( 1, 90 ); // from caclulation, 85.7 keV -> 589 mV -> DAC 90
  
  f->FixParameter( 2, 3.251 );
  f->SetNpx( 1e4 );

  double bins[22]
    = {
    0,    35,  45,  60 , 90,
    120, 150, 180, 210, 255,
    285, 315, 345, 375, 405,
    435, 465, 495, 525, 555,
    585, 600
  };

  TH1D* hist = new TH1D( "DAC",
			 "Landau distribution and DAC distributions;DAC [arb. units];Counts", 21, bins );
  hist->SetLineWidth( 3 );
  hist->SetLineColor( kBlack );
  hist->Sumw2();

  TRandom3* randamer = new TRandom3( 0 );
  for( int i=0; i<event_num; i++ )
    {
      hist->Fill( f->GetRandom( 0, range_max, randamer ) );
    }

  hist->Scale( 1.0 / hist->GetEntries() );
  
  int digits = TMath::Log10( event_num ) + 2;
  double sum = 0.0;
  double sum_partial = 0.0;
  double content_adc7 = 0;
  for( int i=1; i<hist->GetNbinsX()+1; i++ )
    {
      double content = hist->GetBinContent(i);
      sum += content;

      double edge = hist->GetBinLowEdge(i);
      if( edge >200 )
	sum_partial += content;

      if( fabs(edge - 210) < 1e-3 )
	content_adc7 = content;
      
      cout << setw(3) << i << " "
	   << setw(3) << edge << " "
	   << setw( digits ) << content << " "
	   << setw( digits ) << setprecision(3) << 100. * sum / hist->GetEntries() << "% "
	   << setw( digits ) << setprecision(3) << 100. * content_adc7 / sum_partial << "% "
	   << endl;

    }

  TF1* f_fit = new TF1( "f_fit", "landau", 0, 600 );
  f_fit->SetNpx( 1e4 );
  f_fit->SetParameters( f->GetParameter(0), f->GetParameter(1), f->GetParameter(2) );
  hist->Fit( f_fit );

  if( mode == 0 )
    f->SetParameter( 0, f_fit->GetParameter(0) * 2 );
  else if( mode == 1 )
    f->SetParameter( 0, f_fit->GetParameter(0) );

  TCanvas* c = new TCanvas( "canvas", "title", 800, 800 );

  hist->GetYaxis()->SetRangeUser( 1e-7, 10 );
  hist->Draw( "HISTE" );
  f->Draw( "same" );
  f_fit->Draw( "same" );

  string output_basename = "results/adc7_correction_";
  if( mode == 0 )
    output_basename += "LandauMPV_71";
  else if( mode == 1 )
    output_basename += "LandauMPV_90";

  c->Print( (output_basename + ".pdf").c_str() );
  gPad->SetLogy( true );
  c->Print( (output_basename + "_logy.pdf").c_str() );

  return 0;
}
