int
makePlot_track_eval()
{
  gStyle->SetOptStat(1);

  TCut cut_primary( "gtrackID==1" );
  TCut cut_eta( "abs( (-1*log( tan( atan2( sqrt(px*px+py*py), pz ) / 2.0 ) )) - (-1*log( tan( atan2( sqrt(gpx*gpx+gpy*gpy), gpz ) / 2.0 ) ) ) ) < 1 ");

  vector< string > suffixes;
  suffixes.push_back( "ele_10GeV_m05eta" );
  suffixes.push_back( "ele_10GeV_m2eta" );
  suffixes.push_back( "ele_10GeV_m3eta" );
  suffixes.push_back( "ele_10GeV_p05eta" );
  suffixes.push_back( "ele_10GeV_p2eta" );

  TChain chain("tracks");

  TString base("../diskinematics_fun4all/eval_track_fastsim_");

  for ( unsigned i = 0; i < suffixes.size(); i++ )
    {
      TString file = base;
      file.Append( suffixes.at(i) );
      file.Append(".root");

      cout << "Adding file: " << file << endl;
      chain.Add( file );
    }

  TCanvas *c1 = new TCanvas();
  chain.Draw("-1*log( tan( atan2( sqrt(gpx*gpx+gpy*gpy), gpz ) / 2.0 ) )  >> h1(81 , -4.05, 4.05)", cut_primary );
  h1->GetXaxis()->SetTitle("#eta_{true}");
  h1->GetYaxis()->SetTitle("# Events");
  c1->Print("plots/track_eval_c1.eps");


  TCanvas *c2 = new TCanvas();
  chain.Draw("sqrt(px*px+py*py+pz*pz) - sqrt(gpx*gpx+gpy*gpy+gpz*gpz) : -1*log( tan( atan2( sqrt(gpx*gpx+gpy*gpy), gpz ) / 2.0 ) )", cut_primary, "");
  htemp->GetXaxis()->SetTitle("#eta_{true}");
  htemp->GetYaxis()->SetTitle("p_{reco} - p_{true} (GeV)");
  c2->Print("plots/track_eval_c2.eps");


  TCanvas *c3 = new TCanvas();
  chain.Draw("sqrt(px*px+py*py+pz*pz) - sqrt(gpx*gpx+gpy*gpy+gpz*gpz) : -1*log( tan( atan2( sqrt(gpx*gpx+gpy*gpy), gpz ) / 2.0 ) )", cut_primary && cut_eta, "");
  htemp->GetXaxis()->SetTitle("#eta_{true}");
  htemp->GetYaxis()->SetTitle("p_{reco} - p_{true} (GeV)");
  c3->Print("plots/track_eval_c3.eps");


  TCanvas *c4 = new TCanvas();
  chain.Draw("(-1*log( tan( atan2( sqrt(px*px+py*py), pz ) / 2.0 ) )) - (-1*log( tan( atan2( sqrt(gpx*gpx+gpy*gpy), gpz ) / 2.0 ) )) : -1*log( tan( atan2( sqrt(gpx*gpx+gpy*gpy), gpz ) / 2.0 ) )", cut_primary && cut_eta, "");
  htemp->GetXaxis()->SetTitle("#eta_{true}");
  htemp->GetYaxis()->SetTitle("#eta_{reco} - #eta_{true}");
  c4->Print("plots/track_eval_c4.eps");


  TCanvas *c5 = new TCanvas();
  chain.Draw("-1*log( tan( atan2( sqrt(gpx*gpx+gpy*gpy), gpz ) / 2.0 ) ) >> h5(81 , -4.05, 4.05)", cut_primary && cut_eta, "");
  h5->GetXaxis()->SetTitle("#eta_{true}");
  h5->GetYaxis()->SetTitle("# Events (|#eta - #eta_{true}| < 1)");
  c5->Print("plots/track_eval_c5.eps");


  return 1;
}
