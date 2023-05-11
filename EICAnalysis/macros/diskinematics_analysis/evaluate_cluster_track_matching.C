int
evaluate_cluster_track_matching()
{
  TString fname( "eicana_disreco_test.root" );

  TFile *fin = new TFile( fname , "OPEN" );

  TTree *event = (TTree*)fin->Get("event");

  event->Print();

  TCanvas *ctemp = new TCanvas();
  event->Draw("1","em_cluster_e>0");
  cout << "EM candidates total: " << htemp->GetEntries() << endl;
  event->Draw("1","em_cluster_e>1 && em_cluster_prob > 0.01");
  cout << "EM candidates E_cluster > 1 GeV && Prob_EM > 0.01: " << htemp->GetEntries() << endl;
  event->Draw("1","em_cluster_e>1 && em_cluster_prob > 0.01 && em_track_cluster_dr < 0.01");
  cout << "EM candidates E_cluster > 1 GeV && Prob_EM > 0.01 && dr_cluster_track < 0.01: " << htemp->GetEntries() << endl;

  TCanvas *c0 = new TCanvas();
  event->Draw("em_track_cluster_dr",
	      "em_cluster_e > 1");

  TCanvas *c1 = new TCanvas();
  event->Draw("1",//(-1*log( tan( em_cluster_theta / 2. ) ) )",// - em_evtgen_eta",
	      "em_cluster_e>1 && em_cluster_prob > 0.01");

  TCanvas *c2 = new TCanvas();
  event->Draw("(-1*log( tan( em_track_theta / 2. ) ) ) - em_evtgen_eta : em_evtgen_eta",
	      "em_cluster_e>1 && em_cluster_prob > 0.01 && em_track_cluster_dr < 0.01");

//.x Fun4All_EICAnalysis_DISReco.C
//TFile *_file0 = TFile::Open()
//event->Scan("-1*log(tan(em_cluster_theta/2.)):em_cluster_e:-1*log(tan(em_track_theta/2.)):em_track_ptotal","em_cluster_e>2")
//event->Scan("-1*log(tan(em_cluster_theta/2.)):em_cluster_e:-1*log(tan(em_track_theta/2.)):em_track_ptotal","em_cluster_e>2&&em_evtgen_pid==-11")
//event->Scan("-1*log(tan(em_cluster_theta/2.)):em_cluster_e:-1*log(tan(em_track_theta/2.)):em_track_ptotal","em_cluster_e>2&&em_evtgen_pid==11")
//event->Draw("-1*log(tan(em_cluster_theta/2.)):-1*log(tan(em_track_theta/2.))","em_cluster_e>2&&em_evtgen_pid==11")
//event->Draw("-1*log(tan(em_cluster_theta/2.)):em_evtgen_theta","em_cluster_e>2&&em_evtgen_pid==11")
//event->Draw("-1*log(tan(em_cluster_theta/2.)):em_evtgen_eta","em_cluster_e>2&&em_evtgen_pid==11")
//event->Draw("-1*log(tan(em_cluster_theta/2.))-em_evtgen_eta","em_cluster_e>2&&em_evtgen_pid==11")
//event->Draw("-1*log(tan(em_track_theta/2.))-em_evtgen_eta","em_cluster_e>2&&em_evtgen_pid==11")
//event->Draw("-1*log(tan(em_track_theta/2.))-em_evtgen_eta>>h1(100,0,1)","em_cluster_e>2&&em_evtgen_pid==11")
//event->Draw("-1*log(tan(em_track_theta/2.))-em_evtgen_eta>>h1(100,-0.2,0.2)","em_cluster_e>2&&em_evtgen_pid==11")
//event->Draw("-1*log(tan(em_cluster_theta/2.))-(-1*log(tan(em_track_theta/2.)))","em_cluster_e>0")
//event->Draw("-1*log(tan(em_cluster_theta/2.))-(-1*log(tan(em_track_theta/2.)))","em_cluster_e>0&& abs(-1*log(tan(em_cluster_theta/2.))-(-1*log(tan(em_track_theta/2.)))) < 0.1")
//event->Draw("-1*log(tan(em_cluster_theta/2.))-(-1*log(tan(em_track_theta/2.)))","em_cluster_e>0&& abs(-1*log(tan(em_cluster_theta/2.))-(-1*log(tan(em_track_theta/2.)))) < 0.1")
//event->Draw("-1*log(tan(em_cluster_theta/2.))-(-1*log(tan(em_track_theta/2.)))","em_cluster_e>0&& abs(-1*log(tan(em_cluster_theta/2.))-(-1*log(tan(em_track_theta/2.)))) < 0.01")
//event->Draw("(-1*log(tan(em_track_theta/2.)))-em_evtgen_eta","em_cluster_e>0&& abs(-1*log(tan(em_cluster_theta/2.))-(-1*log(tan(em_track_theta/2.)))) < 0.01")

//.q

  return 0;
}
