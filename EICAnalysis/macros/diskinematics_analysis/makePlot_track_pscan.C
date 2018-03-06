int
makePlot_track_pscan()
{
  gStyle->SetOptStat(0);

  TCut cut_primary( "gtrackID==1" );
  TCut cut_eta( "abs( (-1*log( tan( atan2( sqrt(px*px+py*py), pz ) / 2.0 ) )) - (-1*log( tan( atan2( sqrt(gpx*gpx+gpy*gpy), gpz ) / 2.0 ) ) ) ) < 1 ");
  TCut cut_p( "abs( sqrt(px*px+py*py+pz*pz) - sqrt(gpx*gpx+gpy*gpy+gpz*gpz) ) < 1" );

  vector< string > suffixes;
  suffixes.push_back( "muon_10GeV_-2.5Eta" );
  suffixes.push_back( "muon_11GeV_-2.5Eta" );
  suffixes.push_back( "muon_12GeV_-2.5Eta" );
  suffixes.push_back( "muon_13GeV_-2.5Eta" );
  suffixes.push_back( "muon_14GeV_-2.5Eta" );
  suffixes.push_back( "muon_15GeV_-2.5Eta" );
  suffixes.push_back( "muon_16GeV_-2.5Eta" );
  suffixes.push_back( "muon_17GeV_-2.5Eta" );
  suffixes.push_back( "muon_18GeV_-2.5Eta" );
  suffixes.push_back( "muon_19GeV_-2.5Eta" );
  suffixes.push_back( "muon_1GeV_-2.5Eta" );
  suffixes.push_back( "muon_20GeV_-2.5Eta" );
  suffixes.push_back( "muon_21GeV_-2.5Eta" );
  suffixes.push_back( "muon_22GeV_-2.5Eta" );
  suffixes.push_back( "muon_23GeV_-2.5Eta" );
  suffixes.push_back( "muon_24GeV_-2.5Eta" );
  suffixes.push_back( "muon_25GeV_-2.5Eta" );
  suffixes.push_back( "muon_26GeV_-2.5Eta" );
  suffixes.push_back( "muon_27GeV_-2.5Eta" );
  suffixes.push_back( "muon_28GeV_-2.5Eta" );
  suffixes.push_back( "muon_29GeV_-2.5Eta" );
  suffixes.push_back( "muon_2GeV_-2.5Eta" );
  suffixes.push_back( "muon_30GeV_-2.5Eta" );
  suffixes.push_back( "muon_3GeV_-2.5Eta" );
  suffixes.push_back( "muon_4GeV_-2.5Eta" );
  suffixes.push_back( "muon_5GeV_-2.5Eta" );
  suffixes.push_back( "muon_6GeV_-2.5Eta" );
  suffixes.push_back( "muon_7GeV_-2.5Eta" );
  suffixes.push_back( "muon_8GeV_-2.5Eta" );
  suffixes.push_back( "muon_9GeV_-2.5Eta" );

  TString base("../../data/data_trackeval_pscan/eval_track_fastsim_");

  vector< float > v_ptrue;
  vector< float > v_deltap_sigma;
  vector< float > v_deltap_over_p_sigma;

  for ( unsigned i = 0; i < suffixes.size(); i++ )
    {
      TString file = base;
      file.Append( suffixes.at(i) );
      file.Append(".root");

      TFile* fin = new TFile(file, "OPEN");
      TTree *tin = (TTree*)fin->Get("tracks");

      TH1F* h_delta = new TH1F( "h_delta", "", 200, -1, 1 );
      TH1F* h_delta_rel = new TH1F( "h_delta_rel", "", 200, -1, 1 );
      TH1F* h_ptrue = new TH1F( "h_ptrue", "", 31, -0.5, 30 );

      tin->Draw("( sqrt(px*px+py*py+pz*pz) - sqrt(gpx*gpx+gpy*gpy+gpz*gpz) ) >> h_delta", cut_primary );
      tin->Draw("( sqrt(px*px+py*py+pz*pz) - sqrt(gpx*gpx+gpy*gpy+gpz*gpz) ) / sqrt(gpx*gpx+gpy*gpy+gpz*gpz) >> h_delta_rel",  cut_primary );
      tin->Draw("sqrt(gpx*gpx+gpy*gpy+gpz*gpz) >> h_ptrue",  cut_primary );

      v_ptrue.push_back( h_ptrue->GetMean() );
      v_deltap_sigma.push_back( h_delta->GetRMS() );
      v_deltap_over_p_sigma.push_back( h_delta_rel->GetRMS() );
    }

  TGraphErrors *g1 = new TGraphErrors( v_ptrue.size(), &(v_ptrue[0]), &(v_deltap_over_p_sigma[0]) );

  TCanvas *c1 = new TCanvas();

  TH1F* hframe = new TH1F("hframe","",31,-0.5,30.5);
  hframe->GetXaxis()->SetTitle("p_{true} (GeV)");
  hframe->GetYaxis()->SetTitle("#sigma ( ( p_{reco} - p_{true} ) / p_{true} )");
  hframe->GetYaxis()->SetRangeUser(0,0.15);
  hframe->Draw();
  g1->Draw("Psame");

//  TCanvas *c3 = new TCanvas();
//  chain.Draw("( sqrt(px*px+py*py+pz*pz) - sqrt(gpx*gpx+gpy*gpy+gpz*gpz) ) / sqrt(gpx*gpx+gpy*gpy+gpz*gpz) : sqrt(gpx*gpx+gpy*gpy+gpz*gpz)  >> h3(31, -0.5, 30.5, 50, -0.5, 0.5)", cut_primary, "colz");
//  h3->GetXaxis()->SetTitle("#p_{true}");
//  h3->GetYaxis()->SetTitle("( p_{reco} - p_{true} ) / p_{true}");
//  c3->Print("plots/track_pscan_c3.eps");

  return 1;
}
