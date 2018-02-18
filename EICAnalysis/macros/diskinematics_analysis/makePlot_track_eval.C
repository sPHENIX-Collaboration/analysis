int
makePlot_track_eval()
{
  gStyle->SetOptStat(1);

  TCut cut_primary( "gtrackID==1" );
  TCut cut_eta( "abs( (-1*log( tan( atan2( sqrt(px*px+py*py), pz ) / 2.0 ) )) - (-1*log( tan( atan2( sqrt(gpx*gpx+gpy*gpy), gpz ) / 2.0 ) ) ) ) < 1 ");
  TCut cut_p( "abs( sqrt(px*px+py*py+pz*pz) - sqrt(gpx*gpx+gpy*gpy+gpz*gpz) ) < 1" );

  vector< string > suffixes;
  suffixes.push_back( "muon_10GeV_-0.1Eta" );
  suffixes.push_back( "muon_10GeV_-0.2Eta" );
  suffixes.push_back( "muon_10GeV_-0.3Eta" );
  suffixes.push_back( "muon_10GeV_-0.4Eta" );
  suffixes.push_back( "muon_10GeV_-0.5Eta" );
  suffixes.push_back( "muon_10GeV_-0.6Eta" );
  suffixes.push_back( "muon_10GeV_-0.7Eta" );
  suffixes.push_back( "muon_10GeV_-0.8Eta" );
  suffixes.push_back( "muon_10GeV_-0.9Eta" );
  suffixes.push_back( "muon_10GeV_-1.1Eta" );
  suffixes.push_back( "muon_10GeV_-1.2Eta" );
  suffixes.push_back( "muon_10GeV_-1.3Eta" );
  suffixes.push_back( "muon_10GeV_-1.4Eta" );
  suffixes.push_back( "muon_10GeV_-1.5Eta" );
  suffixes.push_back( "muon_10GeV_-1.6Eta" );
  suffixes.push_back( "muon_10GeV_-1.7Eta" );
  suffixes.push_back( "muon_10GeV_-1.8Eta" );
  suffixes.push_back( "muon_10GeV_-1.9Eta" );
  suffixes.push_back( "muon_10GeV_-1Eta" );
  suffixes.push_back( "muon_10GeV_-2.1Eta" );
  suffixes.push_back( "muon_10GeV_-2.2Eta" );
  suffixes.push_back( "muon_10GeV_-2.3Eta" );
  suffixes.push_back( "muon_10GeV_-2.4Eta" );
  suffixes.push_back( "muon_10GeV_-2.5Eta" );
  suffixes.push_back( "muon_10GeV_-2.6Eta" );
  suffixes.push_back( "muon_10GeV_-2.7Eta" );
  suffixes.push_back( "muon_10GeV_-2.8Eta" );
  suffixes.push_back( "muon_10GeV_-2.9Eta" );
  suffixes.push_back( "muon_10GeV_-2Eta" );
  suffixes.push_back( "muon_10GeV_-3Eta" );
  suffixes.push_back( "muon_10GeV_0.1Eta" );
  suffixes.push_back( "muon_10GeV_0.2Eta" );
  suffixes.push_back( "muon_10GeV_0.3Eta" );
  suffixes.push_back( "muon_10GeV_0.4Eta" );
  suffixes.push_back( "muon_10GeV_0.5Eta" );
  suffixes.push_back( "muon_10GeV_0.6Eta" );
  suffixes.push_back( "muon_10GeV_0.7Eta" );
  suffixes.push_back( "muon_10GeV_0.8Eta" );
  suffixes.push_back( "muon_10GeV_0.9Eta" );
  suffixes.push_back( "muon_10GeV_0Eta" );
  suffixes.push_back( "muon_10GeV_1.1Eta" );
  suffixes.push_back( "muon_10GeV_1.2Eta" );
  suffixes.push_back( "muon_10GeV_1.3Eta" );
  suffixes.push_back( "muon_10GeV_1.4Eta" );
  suffixes.push_back( "muon_10GeV_1.5Eta" );
  suffixes.push_back( "muon_10GeV_1.6Eta" );
  suffixes.push_back( "muon_10GeV_1.7Eta" );
  suffixes.push_back( "muon_10GeV_1.8Eta" );
  suffixes.push_back( "muon_10GeV_1.9Eta" );
  suffixes.push_back( "muon_10GeV_1Eta" );
  suffixes.push_back( "muon_10GeV_2.1Eta" );
  suffixes.push_back( "muon_10GeV_2.2Eta" );
  suffixes.push_back( "muon_10GeV_2.3Eta" );
  suffixes.push_back( "muon_10GeV_2.4Eta" );
  suffixes.push_back( "muon_10GeV_2.5Eta" );
  suffixes.push_back( "muon_10GeV_2.6Eta" );
  suffixes.push_back( "muon_10GeV_2.7Eta" );
  suffixes.push_back( "muon_10GeV_2.8Eta" );
  suffixes.push_back( "muon_10GeV_2.9Eta" );
  suffixes.push_back( "muon_10GeV_2Eta" );
  suffixes.push_back( "muon_10GeV_3Eta" );

  //suffixes.push_back( "ele_10GeV_m05eta" );
  //suffixes.push_back( "ele_10GeV_m2eta" );
  //suffixes.push_back( "ele_10GeV_m3eta" );
  //suffixes.push_back( "ele_10GeV_p05eta" );
  //suffixes.push_back( "ele_10GeV_p2eta" );



  TChain chain("tracks");

  //TString base("../diskinematics_fun4all/eval_track_fastsim_");
  TString base("../../data/data_trackeval/eval_track_fastsim_");

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
  chain.Draw("sqrt(px*px+py*py+pz*pz) - sqrt(gpx*gpx+gpy*gpy+gpz*gpz) : -1*log( tan( atan2( sqrt(gpx*gpx+gpy*gpy), gpz ) / 2.0 ) )  >> h3(81, -4.05, 4.05, 50, -1, 1)", cut_primary && cut_eta && cut_p, "colz");
  h3->GetXaxis()->SetTitle("#eta_{true}");
  h3->GetYaxis()->SetTitle("p_{reco} - p_{true} (GeV)");
  c3->Print("plots/track_eval_c3.eps");


  TCanvas *c4 = new TCanvas();
  chain.Draw("(-1*log( tan( atan2( sqrt(px*px+py*py), pz ) / 2.0 ) )) - (-1*log( tan( atan2( sqrt(gpx*gpx+gpy*gpy), gpz ) / 2.0 ) )) : -1*log( tan( atan2( sqrt(gpx*gpx+gpy*gpy), gpz ) / 2.0 ) )", cut_primary && cut_eta, "");
  htemp->GetXaxis()->SetTitle("#eta_{true}");
  htemp->GetYaxis()->SetTitle("#eta_{reco} - #eta_{true}");
  c4->Print("plots/track_eval_c4.eps");


  TCanvas *c5 = new TCanvas();
  chain.Draw("-1*log( tan( atan2( sqrt(gpx*gpx+gpy*gpy), gpz ) / 2.0 ) ) >> h5(81 , -4.05, 4.05)", cut_primary && cut_eta && cut_p, "");
  h5->GetXaxis()->SetTitle("#eta_{true}");
  h5->GetYaxis()->SetTitle("# Events (|#eta - #eta_{true}| < 1 && |#p - #p_{true}| < 1 GeV)");
  c5->Print("plots/track_eval_c5.eps");


  return 1;
}
