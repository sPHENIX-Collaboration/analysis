// dca_meanx, dca_meany set by hand
#include "analysis_tracking.hh"

using namespace std;

void analysis_tracking(int run_no = 41981, bool mag_on = true, bool debug = false )
{
  int sigma = 3;
  int nloop = 60;

  bool geant = false;
  if (run_no == 1)
    geant = true;

  bool does_reverse = false;
  // does_reverse = true;

  string data_dir = "/sphenix/u/nukazuka/work_now/analysis/tracking/hinakos/work/F4AInttRead/macro/";
  string fname = data_dir + Form("InttAna_run%d.root", run_no); // with no any cut
  // if (mag_on)
  //   fname = Form("AnaTutorial_inttana_%d_mag.root", run_no); // with no any cut

  if (geant)
    {
      if (mag_on)
        {
	  fname = "/sphenix/tg/tg01/commissioning/INTT/work/tsujibata/tracking/F4AInttRead/macro/macros/detectors/sPHENIX/AnaTutorial_inttana_zvtxwidth10_ptmin0.root";
        }

      else
	{
	  fname = "/sphenix/tg/tg01/commissioning/INTT/work/tsujibata/tracking/F4AInttRead/macro/macros/detectors/sPHENIX/AnaTutorial_inttana_vtx0.root";
	}
    }
  
  TFile *f = TFile::Open(fname.c_str());
  cout << "opened file : " << fname << endl;

  string dir_out = "results/tracking/";
  TString fname_out = "tracking_run" + to_string( run_no ) + ".root";
  // fname_out.ReplaceAll("AnaTutorial_inttana", "tracking");

  // if (does_reverse)
  //   fname_out.ReplaceAll(".root", "_reverse.root");
  // if (debug)
  //   fname_out.ReplaceAll(".root", "_debug.root");

  // fname_out.ReplaceAll(".root", "_test.root");

  fname_out = dir_out + fname_out;
  cout << "out put file : " << fname_out << endl;

  TH1F *h_dcaz_one_ = new TH1F("h_dcaz_one_", "", 100, -200, 200);

  TFile *froot = new TFile(fname_out, "recreate");
  TH1F *h_nclus = new TH1F("h_nclus", "", 100, 0, 100);

  TTree *temp_tree_ = new TTree("temp_tree_", "temp_tree_");
  TTree *clus_tree = new TTree("clus_tree", "clus_tree");
  TTree *truth_tree = new TTree("truth_tree", "truth_tree");
  TTree *track_tree = new TTree("track_tree", "track_tree");

  h_dphi_nocut = new TH2F("h_dphi_nocut", "d_phi : phi_in;phi_in;d_phi", 200, -3.2, 3.2, 200, -0.1, 0.1);

  TH1F *h_dcaz_sigma_one = new TH1F("h_dcaz_sigma_one", "h_dcaz_sigma_one;dca_z sigma of one event;entries", 100, 0, 15);
  TH1D *h_xvtx = new TH1D("h_xvtx", "h_xvtx", 100, -0.01, 0.01);
  TH1D *h_yvtx = new TH1D("h_yvtx", "h_yvtx", 100, -0.01, 0.01);
  TH1D *h_zvtx = new TH1D("h_zvtx", "h_zvtx", 100, -20, 20);
  TH2D *h_zr[2];
  for (int i = 0; i < 2; i++)
    h_zr[i] = new TH2D(Form("h_zr_%d", i), "h_zr;z;r", 100, -20, 20, 100, -10, 10);

  c = new TCanvas(fname_out.ReplaceAll(".root", ".pdf"), "c", 1200, 600);
  c->Divide(2, 1);
  c->cd(1);

  int page_counter = 0;
  c->Print(((string)c->GetName() + "[").c_str());

  double x_vertex;
  double y_vertex;
  double z_vertex;
  
  int evt_temp_;
  vector<double> d_phi_;
  vector<double> d_theta_;
  vector<double> phi_in_;
  vector<double> dca_x_;
  vector<double> dca_y_;
  vector<double> dca_z_;
  vector<double> dca_2d_;
  double zvtx_one_;
  double zvtx_sigma_one_;
  temp_tree_->Branch("evt_temp_", &evt_temp_, "evt_temp_/I");
  temp_tree_->Branch("d_phi_", &d_phi_);
  temp_tree_->Branch("d_theta_", &d_theta_);
  temp_tree_->Branch("phi_in_", &phi_in_);
  temp_tree_->Branch("dca_x_", &dca_x_);
  temp_tree_->Branch("dca_y_", &dca_y_);
  temp_tree_->Branch("dca_z_", &dca_z_);
  temp_tree_->Branch("dca_2d_", &dca_2d_);
  temp_tree_->Branch("zvtx_one_", &zvtx_one_, "zvtx_one_/D");
  temp_tree_->Branch("zvtx_sigma_one_", &zvtx_sigma_one_, "zvtx_sigma_one_/D");

  int evt_clus;
  vector<double> x_in;
  vector<double> y_in;
  vector<double> z_in;
  vector<double> r_in;
  vector<int> size_in;
  vector<double> phi_in;
  vector<double> theta_in;
  vector<double> adc_in;
  vector<bool>   is_associated_in;
  vector<double> track_incoming_theta_in;
  vector<double> x_out;
  vector<double> y_out;
  vector<double> z_out;
  vector<double> r_out;
  vector<int> size_out;
  vector<double> phi_out;
  vector<double> theta_out;
  vector<double> adc_out;
  vector<bool>   is_associated_out;
  vector<double> track_incoming_theta_out;  
  clus_tree->Branch("evt_clus", &evt_clus, "evt_clus/I");
  clus_tree->Branch("x_in", &x_in); // x coordinate of clusters on the inner barrel
  clus_tree->Branch("y_in", &y_in); // y coordinate of clusters on the inner barrel
  clus_tree->Branch("z_in", &z_in); // z coordinate of clusters on the inner barrel
  clus_tree->Branch("r_in", &r_in); // r coordinate of clusters on the inner barrel
  clus_tree->Branch( "size_in", &size_in ); // size of inner cluster
  clus_tree->Branch("phi_in", &phi_in);
  clus_tree->Branch("theta_in", &theta_in);
  clus_tree->Branch("adc_in", &adc_in);
  clus_tree->Branch("is_associated_in", &is_associated_in);
  clus_tree->Branch("track_incoming_theta_in", &track_incoming_theta_in);
  
  clus_tree->Branch("x_out", &x_out); // x coordinate of clusters on the inner barrel
  clus_tree->Branch("y_out", &y_out); // x coordinate of clusters on the inner barrel
  clus_tree->Branch("z_out", &z_out); // x coordinate of clusters on the inner barrel
  clus_tree->Branch("r_out", &r_out); // x coordinate of clusters on the inner barrel
  clus_tree->Branch("size_out", &size_out ); // size of outer cluster
  clus_tree->Branch("phi_out", &phi_out);
  clus_tree->Branch("theta_out", &theta_out);
  clus_tree->Branch("adc_out", &adc_out);
  clus_tree->Branch("is_associated_out", &is_associated_out);
  clus_tree->Branch("track_incoming_theta_out", &track_incoming_theta_out);
  clus_tree->Branch("z_vertex", &z_vertex, "z_vertex/D");

  int evt_track;
  int ntrack = 0;
  vector<double> slope_rz;
  vector<double> phi_tracklets;
  vector<double> theta_tracklets;
  vector<double> phi_track;
  vector<double> theta_track;
  vector<double> z_tracklets_out;
  vector<double> dca_2d;
  vector<double> dca_z;
  vector<int> is_deleted;
  vector<int> dca_range_out;
  vector<int> dca2d_range_out;
  vector<int> dcaz_range_out;
  vector<double> pT;
  vector<double> px_truth;
  vector<double> py_truth;
  vector<double> pz_truth;
  vector<double> px;
  vector<double> py;
  vector<double> pz;
  vector<double> rad;
  vector<double> pT_truth;
  vector<double> charge;
  track_tree->Branch("evt_track", &evt_track, "evt_track/I");
  track_tree->Branch("ntrack", &ntrack, "ntrack/I");
  track_tree->Branch("phi_tracklets", &phi_tracklets);
  track_tree->Branch("slope_rz", &slope_rz);
  track_tree->Branch("theta_tracklets", &theta_tracklets);
  track_tree->Branch("phi_track", &phi_track);
  track_tree->Branch("theta_track", &theta_track);
  track_tree->Branch("z_tracklets_out", &z_tracklets_out);
  track_tree->Branch("dca_2d", &dca_2d);
  track_tree->Branch("dca_z", &dca_z);
  track_tree->Branch("is_deleted", &is_deleted);
  track_tree->Branch("dca_range_out", &dca_range_out);
  track_tree->Branch("dcaz_range_out", &dcaz_range_out);
  track_tree->Branch("dca2d_range_out", &dca2d_range_out);
  track_tree->Branch("pT", &pT);
  track_tree->Branch("px", &px);
  track_tree->Branch("py", &py);
  track_tree->Branch("pz", &pz);
  track_tree->Branch("px_truth", &px_truth);
  track_tree->Branch("py_truth", &py_truth);
  track_tree->Branch("pz_truth", &pz_truth);
  track_tree->Branch("pT_truth", &pT_truth);
  track_tree->Branch("charge", &charge);
  track_tree->Branch("rad", &rad);
  track_tree->Branch("x_vertex", &x_vertex, "x_vertex/D");
  track_tree->Branch("y_vertex", &y_vertex, "y_vertex/D");
  track_tree->Branch("z_vertex", &z_vertex, "z_vertex/D");

  int evt_truth;
  int ntruth = 0;
  int ntruth_cut = 0;
  vector<double> truthpx;
  vector<double> truthpy;
  vector<double> truthpz;
  vector<double> truthpt;
  vector<double> trutheta;
  vector<double> truththeta;
  vector<double> truthphi;
  vector<double> truthxvtx;
  vector<double> truthyvtx;
  vector<double> truthzvtx;
  vector<double> truthzout;
  vector<int> truthpid;
  vector<int> status;
  truth_tree->Branch("evt_truth", &evt_truth, "evt_truth/I");
  truth_tree->Branch("ntruth_cut", &ntruth_cut, "ntruth_cut/I");
  truth_tree->Branch("truthpx", &truthpx);
  truth_tree->Branch("truthpy", &truthpy);
  truth_tree->Branch("truthpz", &truthpz);
  truth_tree->Branch("truthpt", &truthpt);
  truth_tree->Branch("trutheta", &trutheta);
  truth_tree->Branch("truththeta", &truththeta);
  truth_tree->Branch("truthphi", &truthphi);
  truth_tree->Branch("truthxvtx", &truthxvtx);
  truth_tree->Branch("truthyvtx", &truthyvtx);
  truth_tree->Branch("truthzvtx", &truthzvtx);
  truth_tree->Branch("truthzout", &truthzout);
  truth_tree->Branch("truthpid", &truthpid);
  truth_tree->Branch("status", &status);

  // from input file
  TNtuple *ntp_clus = (TNtuple *)f->Get("ntp_clus");
  TTree *hepmctree = (TTree *)f->Get("hepmctree");
  bool no_mc = true;
  if( hepmctree == nullptr )
    no_mc = true;
  else if( hepmctree->GetEntries() == 0 )    
    no_mc = true;
  
  cout << "no_mc : " << no_mc << endl;

  Float_t nclus, nclus2, bco_full, evt, size, adc, x, y, z, lay, lad, sen, x_vtx, y_vtx, z_vtx;

  ntp_clus->SetBranchAddress("nclus", &nclus);
  ntp_clus->SetBranchAddress("nclus2", &nclus2);
  ntp_clus->SetBranchAddress("bco_full", &bco_full);
  ntp_clus->SetBranchAddress("evt", &evt);
  ntp_clus->SetBranchAddress("size", &size);
  ntp_clus->SetBranchAddress("adc", &adc);
  ntp_clus->SetBranchAddress("x", &x);
  ntp_clus->SetBranchAddress("y", &y);
  ntp_clus->SetBranchAddress("z", &z);
  ntp_clus->SetBranchAddress("lay", &lay);
  ntp_clus->SetBranchAddress("lad", &lad);
  ntp_clus->SetBranchAddress("sen", &sen);
  ntp_clus->SetBranchAddress("x_vtx", &x_vtx);
  ntp_clus->SetBranchAddress("y_vtx", &y_vtx);
  ntp_clus->SetBranchAddress("z_vtx", &z_vtx);

  double m_truthpx, m_truthpy, m_truthpz, m_truthpt, m_trutheta, m_truththeta, m_truthphi, m_xvtx, m_yvtx, m_zvtx;
  int m_evt, m_status, m_truthpid;
  if( !no_mc )
    {
      hepmctree->SetBranchAddress("m_evt", &m_evt);
      hepmctree->SetBranchAddress("m_status", &m_status);
      hepmctree->SetBranchAddress("m_truthpx", &m_truthpx);
      hepmctree->SetBranchAddress("m_truthpy", &m_truthpy);
      hepmctree->SetBranchAddress("m_truthpz", &m_truthpz);
      hepmctree->SetBranchAddress("m_truthpt", &m_truthpt);
      hepmctree->SetBranchAddress("m_trutheta", &m_trutheta);
      hepmctree->SetBranchAddress("m_truthpid", &m_truthpid);
      hepmctree->SetBranchAddress("m_truththeta", &m_truththeta);
      hepmctree->SetBranchAddress("m_truthphi", &m_truthphi);
      hepmctree->SetBranchAddress("m_xvtx", &m_xvtx);
      hepmctree->SetBranchAddress("m_yvtx", &m_yvtx);
      hepmctree->SetBranchAddress("m_zvtx", &m_zvtx);
    }
  
  int sum_event = 0;
  int ntrack_ = 0;
  for (int icls = 0; icls < ntp_clus->GetEntries(); icls++)
    {
      cout << Form("  -----  event %d {event gene.}  -----  ", sum_event) << endl;

      clustEvent event{};
      cluster clust{};

      ntp_clus->GetEntry(icls); // first cluster in one event
      clust.set(evt, 0, x, y, z, adc, size, lay, x_vtx, y_vtx, z_vtx);
      event.vclus.push_back(clust);

      for (int j = 1; j < nclus; j++) // second~ cluster in one event
        {
	  ntp_clus->GetEntry(++icls);
	  cluster clust2{};
	  clust2.set(evt, 0, x, y, z, adc, size, lay, x_vtx, y_vtx, z_vtx);
	  event.vclus.push_back(clust2);
        }

      dotracking(event, h_dphi_nocut );
      ntrack_ = event.vtrack.size();
      h_dcaz_one_->Reset();
      for (int itrk = 0; itrk < ntrack_; itrk++)
        {
	  h_dcaz_one_->Fill(event.vtrack[itrk]->dca[2]);
        }
      zvtx_one_ = h_dcaz_one_->GetMean();
      zvtx_sigma_one_ = h_dcaz_one_->GetStdDev();

      evt_temp_ = sum_event;
      for (int itrk = 0; itrk < ntrack_; itrk++)
        {
	  dca_x_.push_back(event.vtrack[itrk]->dca[0]);
	  dca_y_.push_back(event.vtrack[itrk]->dca[1]);
	  dca_z_.push_back(event.vtrack[itrk]->dca[2]);
	  dca_2d_.push_back(event.vtrack[itrk]->dca_2d);
	  d_phi_.push_back(event.vtrack[itrk]->d_phi);
	  d_theta_.push_back(event.vtrack[itrk]->d_theta);
	  phi_in_.push_back(event.vtrack[itrk]->phi_in);
        }
      if (does_reverse)
        {
	  reverse(dca_x_);
	  reverse(dca_y_);
	  reverse(dca_z_);
	  reverse(dca_2d_);
	  reverse(d_phi_);
	  reverse(d_theta_);
	  reverse(phi_in_);
        }
      temp_tree_->Fill();
      erase(dca_x_);
      erase(dca_y_);
      erase(dca_z_);
      erase(dca_2d_);
      erase(d_phi_);
      erase(d_theta_);
      erase(phi_in_);

      event.clear();
      sum_event++;

      if (debug && sum_event > nloop)
	break;
    } // end of for (int icls = 0; icls < ntp_clus->GetEntries(); icls++)
  
  n_dotracking++;
  TH1F *h_dcax = new TH1F("h_dcax", "h_dcax", 100, -0.4, 0.4);
  temp_tree_->Draw("dca_x_>>h_dcax");
  TH1F *h_dcay = new TH1F("h_dcay", "h_dcay", 100, -0.4, 0.4);
  temp_tree_->Draw("dca_y_>>h_dcay");
  TH1F *h_dcaz = new TH1F("h_dcaz", "h_dcaz;DCA_z[cm]", 60, -150, 150);
  temp_tree_->Draw("dca_z_>>h_dcaz");
  TH1F *h_dtheta = new TH1F("h_dtheta", "dtheta{inner - outer};dtheta;entries", 100, -3.2, 3.2);
  temp_tree_->Draw("d_theta_>>h_dtheta");
  TH1F *h_dphi = new TH1F("h_dphi", "#Delta #phi_{AB};#Delta #phi_{AB}", 100, -1, 1);
  temp_tree_->Draw("d_phi_>>h_dphi");

  TH1F *h_dca2d = new TH1F("h_dca2d", "h_dca", 100, -10, 10);
  temp_tree_->Draw("dca_2d_>>h_dca2d");

  vector < TH1F* > h_ = {h_dcax, h_dcay, h_dcaz, h_dphi, h_dtheta, h_dca2d};

  // the mean of DCA in all event
  double dcax_mean = h_dcax->GetMean();
  double dcay_mean = h_dcay->GetMean();
  if (!(geant))
    {
      dcax_mean = -0.019;
      dcay_mean = 0.198;
    }
  double dcaz_mean = h_dcaz->GetMean();
  double dca2d_mean = h_dca2d->GetMean();

  double dcax_sigma = h_dcax->GetStdDev();
  double dcay_sigma = h_dcay->GetStdDev();
  double dcaz_sigma = h_dcaz->GetStdDev();
  double dca2d_sigma = h_dca2d->GetStdDev();

  dca2d_sigma *= sigma;
  dcaz_sigma *= sigma;

  double dca2d_max = dca2d_mean + dca2d_sigma;
  double dca2d_min = dca2d_mean - dca2d_sigma;
  double dcaz_max = dcaz_mean + dcaz_sigma;
  double dcaz_min = dcaz_mean - dcaz_sigma;

  // tracking
  int ihit = 0;
  int itruth = 0;
  int temp_evt = 0;
  for (int ievt = 0; ievt < sum_event; ievt++, ihit++, itruth++)
  //  for (int ievt = 0; ievt < 3; ievt++, ihit++, itruth++)
    {
      temp_tree_->GetEntry(ievt);
      cout << Form("  -----  event %d  -----  ", ievt) << endl;
      ntruth = 0;
      ntruth_cut = 0;

      clustEvent event;
      event.ievt = ievt;
      event.mag_on = mag_on;
      event.run_no = run_no;
      // event.bco_full = bco_full;

      if (ihit < ntp_clus->GetEntries())
        {
	  ntp_clus->GetEntry(ihit);

	  if (!(no_mc))
            {
	      hepmctree->GetEntry(itruth);
	      while (m_evt != evt)
                {
		  itruth++;
		  hepmctree->GetEntry(itruth);
                }
	      temp_evt = m_evt;
            }
	  cluster clust{};

	  clust.set(evt, 0, x, y, z, adc, size, lay, x_vtx, y_vtx, z_vtx);
	  event.vclus.push_back(clust);

	  for (int j = 1; j < nclus; j++) // second~ cluster in one event
            {
	      ntp_clus->GetEntry(++ihit);
	      cluster clust2{};

	      clust2.set(evt, 0, x, y, z, adc, size, lay, x_vtx, y_vtx, z_vtx);
	      event.vclus.push_back(clust2);
            }

	  if (!(no_mc))
            {
	      while (m_evt == temp_evt)
                {
		  ntruth++;
		  truth *tru = new truth();
		  tru->set_truth(m_truthpx, m_truthpy, m_truthpz, m_truthpt, m_status, m_trutheta, m_truthpid, m_truththeta, m_truthphi, m_xvtx, m_yvtx, m_zvtx);
		  event.vtruth.push_back(tru);
		  itruth++;
		  if (itruth == hepmctree->GetEntries())
		    break;

		  hepmctree->GetEntry(itruth);
                }
	      itruth--;
            }
        }

      event.dca_mean[0] = dcax_mean;
      event.dca_mean[1] = dcay_mean;
      event.dca_mean[2] = zvtx_one_;

      // from dca_mean
      dotracking(event, h_dphi_nocut );
      ntrack_ = event.vtrack.size();
      evt_clus = ievt;
      evt_track = ievt;
      evt_truth = ievt;
      h_dcaz_one_->Reset();
      for (int itrk = 0; itrk < ntrack_; itrk++) // from dca_mean
        {
	  h_dcaz_one_->Fill(event.vtrack[itrk]->dca[2]);
        }
      double dcaz_mean_one = h_dcaz_one_->GetMean();    // one event
      double dcaz_sigma_one = h_dcaz_one_->GetStdDev(); // one event
      h_dcaz_sigma_one->Fill(dcaz_sigma_one);
      dcaz_sigma_one *= sigma;
      double dcaz_max_one = dcaz_mean_one + dcaz_sigma;
      double dcaz_min_one = dcaz_mean_one - dcaz_sigma;
      zvtx_sigma_one_ *= sigma;
      // double dcaz_max_one = zvtx_one_ + zvtx_sigma_one_;
      // double dcaz_min_one = zvtx_one_ - zvtx_sigma_one_;

      event.dca2d_max = dca2d_max;
      event.dca2d_min = dca2d_min;
      event.dcaz_max = dcaz_max_one;
      event.dcaz_min = dcaz_min_one;

      for (int itrk = 0; itrk < ntrack_; itrk++)
        {
	  event.vtrack[itrk]->dca_mean[0] = dcax_mean;
	  event.vtrack[itrk]->dca_mean[1] = dcay_mean;
	  event.vtrack[itrk]->dca_mean[2] = dcaz_mean_one;
	  event.vtrack[itrk]->calc_line();
	  event.vtrack[itrk]->calc_inv();
	  event.vtrack[itrk]->calc_pT();

        }
      
      event.SetTrackInfoToCluster(); // set track information to associated clusters

      if (!(no_mc))
        {
	  for (int itrt = 0; itrt < ntruth; itrt++)
            {
	      event.vtruth[itrt]->dca_mean[0] = dcax_mean;
	      event.vtruth[itrt]->dca_mean[1] = dcay_mean;
	      event.vtruth[itrt]->dca_mean[2] = dcaz_mean_one;

	      event.vtruth[itrt]->calc_line();
	      event.vtruth[itrt]->calc_center();
            }
        }
      h_nclus->Fill(event.vclus.size());

      for (int iclus = 0; iclus < event.vclus.size(); iclus++)
        {
	  event.vclus[iclus].dca_mean[0] = dcax_mean;
	  event.vclus[iclus].dca_mean[1] = dcay_mean;
	  event.vclus[iclus].dca_mean[2] = dcaz_mean_one;

	  if (event.vclus[iclus].layer < 2)
            {
	      x_in.push_back( event.vclus[iclus].x );
	      y_in.push_back( event.vclus[iclus].y );
	      z_in.push_back( event.vclus[iclus].z );
	      r_in.push_back( event.vclus[iclus].r );
	      size_in.push_back( event.vclus[iclus].size );
	      phi_in.push_back( event.vclus[iclus].getphi_clus() );
	      theta_in.push_back( event.vclus[iclus].gettheta_clus() );
	      adc_in.push_back( event.vclus[iclus].adc );
	      is_associated_in.push_back( event.vclus[iclus].is_associated );
	      track_incoming_theta_in.push_back( event.vclus[iclus].track_incoming_theta );
	      //event.vclus[iclus].print();
	      
            }
	  if (1 < event.vclus[iclus].layer)
            {
	      x_out.push_back(event.vclus[iclus].x);
	      y_out.push_back(event.vclus[iclus].y);
	      z_out.push_back(event.vclus[iclus].z);
	      r_out.push_back(event.vclus[iclus].r);
	      size_out.push_back( event.vclus[iclus].size );
	      phi_out.push_back(event.vclus[iclus].getphi_clus());
	      theta_out.push_back(event.vclus[iclus].gettheta_clus());
	      adc_out.push_back( event.vclus[iclus].adc );
	      is_associated_out.push_back( event.vclus[iclus].is_associated );
	      track_incoming_theta_out.push_back( event.vclus[iclus].track_incoming_theta );
            }
        }
      if (does_reverse)
        {
	  reverse(x_out);
	  reverse(y_out);
	  reverse(z_out);
	  reverse(r_out);
	  reverse(phi_out);
	  reverse(theta_out);

	  reverse(x_in);
	  reverse(y_in);
	  reverse(z_in);
	  reverse(r_in);
	  reverse(phi_in);
	  reverse(theta_in);
        }
      clus_tree->Fill();

      erase(x_in);
      erase(y_in);
      erase(z_in);
      erase(r_in);
      erase(size_in);
      erase(phi_in);
      erase(theta_in);
      erase( adc_in );
      erase( is_associated_in );
      erase( track_incoming_theta_in );

      erase(x_out);
      erase(y_out);
      erase(z_out);
      erase(r_out);
      erase(size_out);
      erase(phi_out);
      erase(theta_out);
      erase( adc_out );
      erase( is_associated_out );
      erase( track_incoming_theta_out );

      /*
      for (int itrk = 0; itrk < ntrack_; itrk++)
        {
	  event.vtrack[itrk]->dca_mean[0] = dcax_mean;
	  event.vtrack[itrk]->dca_mean[1] = dcay_mean;
	  event.vtrack[itrk]->dca_mean[2] = dcaz_mean_one;
	  event.vtrack[itrk]->calc_line();
	  event.vtrack[itrk]->calc_inv();
	  event.vtrack[itrk]->calc_pT();

	  event.vtrack[itrk]->print();	  
        }
      
      event.SetTrackInfoToCluster();
      */
      
      event.dca_check(debug);
      event.makeonetrack();

      event.charge_check();
      event.truth_check();

      event.cluster_check();
      event.track_check();
      ntrack = 0;
      for (int itrk = 0; itrk < ntrack_; itrk++)
        {
	  slope_rz.push_back		(event.vtrack[itrk]->track_rz->GetParameter(1));
	  phi_tracklets.push_back	(event.vtrack[itrk]->getphi_tracklet());
	  theta_tracklets.push_back	(event.vtrack[itrk]->gettheta_tracklet());
	  phi_track.push_back		(event.vtrack[itrk]->getphi());
	  theta_track.push_back		(event.vtrack[itrk]->gettheta());
	  dca_z.push_back		(event.vtrack[itrk]->dca[2]);
	  dca_2d.push_back		(event.vtrack[itrk]->dca_2d);
	  z_tracklets_out.push_back	(event.vtrack[itrk]->p2.z());
	  pT.push_back			(event.vtrack[itrk]->pT);
	  px.push_back			(event.vtrack[itrk]->p_reco[0]);
	  py.push_back			(event.vtrack[itrk]->p_reco[1]);
	  pz.push_back			(event.vtrack[itrk]->p_reco[2]);
	  rad.push_back			(event.vtrack[itrk]->rad);
	  charge.push_back		(event.vtrack[itrk]->charge);
	  if (!(no_mc))
            {
	      pT_truth.push_back(event.vtruth[0]->m_truthpt);
	      px_truth.push_back(event.vtruth[0]->m_truthpx);
	      py_truth.push_back(event.vtruth[0]->m_truthpy);
	      pz_truth.push_back(event.vtruth[0]->m_truthpz);
            }

	  is_deleted.push_back(event.vtrack[itrk]->is_deleted);
	  dca2d_range_out.push_back(event.vtrack[itrk]->dca2d_range_out);
	  dcaz_range_out.push_back(event.vtrack[itrk]->dcaz_range_out);
	  dca_range_out.push_back(event.vtrack[itrk]->dca_range_out);

	  x_vertex = event.vtrack[itrk]->dca_mean[0];
	  y_vertex = event.vtrack[itrk]->dca_mean[1];
	  z_vertex = event.vtrack[itrk]->dca_mean[2];

	  if (event.vtrack[itrk]->is_deleted == true)
	    continue;
	  if (event.vtrack[itrk]->dca_range_out == true)
	    continue;
	  ntrack++;
	  h_xvtx->Fill(x_vertex);
	  h_yvtx->Fill(y_vertex);
	  h_zvtx->Fill(z_vertex);
        }

      if (does_reverse)
        {
	  reverse(slope_rz);
	  reverse(phi_tracklets);
	  reverse(theta_tracklets);
	  reverse(phi_track);
	  reverse(theta_track);
	  reverse(dca_z);
	  reverse(dca_2d);
	  reverse(is_deleted);
	  reverse(dca_range_out);
	  reverse(dca2d_range_out);
	  reverse(dcaz_range_out);
	  reverse(z_tracklets_out);
	  reverse(pT);
	  reverse(px);
	  reverse(py);
	  reverse(pz);
	  reverse(pT_truth);
	  reverse(px_truth);
	  reverse(py_truth);
	  reverse(pz_truth);
	  reverse(charge);
	  reverse(rad);
        }

      track_tree->Fill();

      erase(slope_rz);
      erase(phi_tracklets);
      erase(theta_tracklets);
      erase(phi_track);
      erase(theta_track);
      erase(dca_z);
      erase(dca_2d);
      erase(is_deleted);
      erase(dca_range_out);
      erase(dca2d_range_out);
      erase(dcaz_range_out);
      erase(z_tracklets_out);
      erase(pT);
      erase(px);
      erase(py);
      erase(pz);
      erase(pT_truth);
      erase(px_truth);
      erase(py_truth);
      erase(pz_truth);
      erase(charge);
      erase(rad);
      if (!(no_mc))
        {
	  for (int itrt = 0; itrt < ntruth; itrt++)
            {

	      if (event.vtruth[itrt]->is_charged == false)
		continue;

	      if (event.vtruth[itrt]->is_intt == false)
		continue;
	      ntruth_cut++;

	      truthpx.push_back(event.vtruth[itrt]->m_truthpx);
	      truthpy.push_back(event.vtruth[itrt]->m_truthpy);
	      truthpz.push_back(event.vtruth[itrt]->m_truthpz);
	      truthpt.push_back(event.vtruth[itrt]->m_truthpt);
	      trutheta.push_back(event.vtruth[itrt]->m_trutheta);
	      truththeta.push_back(event.vtruth[itrt]->m_truththeta);
	      truthphi.push_back(event.vtruth[itrt]->m_truthphi);
	      truthpid.push_back(event.vtruth[itrt]->m_truthpid);
	      status.push_back(event.vtruth[itrt]->m_status);
	      truthxvtx.push_back(event.vtruth[itrt]->m_xvtx);
	      truthyvtx.push_back(event.vtruth[itrt]->m_yvtx);
	      truthzvtx.push_back(event.vtruth[itrt]->m_zvtx);
	      truthzout.push_back(event.vtruth[itrt]->m_truthz_out);
            }
	  if (does_reverse)
            {
	      reverse(truthpx);
	      reverse(truthpy);
	      reverse(truthpz);
	      reverse(truthpt);
	      reverse(trutheta);
	      reverse(truththeta);
	      reverse(truthphi);
	      reverse(truthpid);
	      reverse(status);
	      reverse(truthxvtx);
	      reverse(truthyvtx);
	      reverse(truthzvtx);
	      reverse(truthzout);
            }
	  truth_tree->Fill();

	  erase(truthpx);
	  erase(truthpy);
	  erase(truthpz);
	  erase(truthpt);
	  erase(trutheta);
	  erase(truththeta);
	  erase(truthphi);
	  erase(truthpid);
	  erase(status);
	  erase(truthxvtx);
	  erase(truthyvtx);
	  erase(truthzvtx);
	  erase(truthzout);
        }

      // drawing
      // if (fabs(event.vclus.size() - 2 * ntrack) < 5 && /*debug &&*/ 10 < event.vclus.size() && event.vclus.size() < 50 /* && && geant &&*/ /*ievt < 500*/)
      // if (ievt < nloop && ievt == 56 /*&& ievt==56*/ /*&& fabs(event.dca_mean[2]) < 2. && ntrack < 20*/)
      {
	if( ievt > 100 )
	  continue;

	// c->cd(1);
	// event.draw_truthline(0, false, TColor::GetColor(232, 85, 98));
	// event.draw_tracklets(0, true, kBlack, false, false);
	// event.draw_clusters(0, true, kBlack);
	// c->Print(c->GetName());
	// c->cd(2);
	// event.draw_truthline(1, false, TColor::GetColor(232, 85, 98));
	// event.draw_tracklets(1, true, kBlack, false, false);
	// event.draw_clusters(1, true, kBlack);
	// c->Print(c->GetName());

	// c->cd(1);
	// // // event.draw_truthline(0, false, TColor::GetColor(232, 85, 98));
	// event.draw_tracklets(0, false, kBlack, false, false);
	// event.draw_clusters(0, true, kBlack);

	// c->cd(2);
	// // // event.draw_truthline(1, false, TColor::GetColor(232, 85, 98));
	// event.draw_tracklets(1, false, kBlack, false, false);
	// event.draw_clusters(1, true, kBlack);
	// c->Print(c->GetName());

	// c->cd(1);
	// event.draw_tracklets(0, false, kRed, true, false);
	// // event.draw_truthline(0, true, TColor::GetColor(232, 85, 98));
	// c->cd(2);
	// event.draw_tracklets(1, false, kRed, true, false);
	// // event.draw_truthline(1, true, TColor::GetColor(232, 85, 98));
	// c->Print(c->GetName());

	// c->cd(1);
	// event.draw_tracklets(0, false, kAzure + 1, true, true);
	// // event.draw_truthline(0, true, TColor::GetColor(232, 85, 98));
	// c->cd(2);
	// event.draw_tracklets(1, false, kAzure + 1, true, true);
	// // event.draw_truthline(1, true, TColor::GetColor(232, 85, 98));
	// c->Print(c->GetName());

	// c->cd(1);
	// event.draw_tracklets(0, false, kPink, false, true, true);
	// // event.draw_truthline(0, true, TColor::GetColor(232, 85, 98));
	// c->cd(2);
	// event.draw_tracklets(1, false, kPink, false, true, true);
	// // event.draw_truthline(1, true, TColor::GetColor(232, 85, 98));
	// c->Print(c->GetName());

	// c->cd(1);
	// // event.draw_truthline(0, false, TColor::GetColor(232, 85, 98));
	// event.draw_truthcurve(0, false, TColor::GetColor(232, 85, 98));
	// // event.draw_tracklets(0, false, kGray + 1, false, false);
	// // event.draw_tracklets(0, true, TColor::GetColor(0, 118, 186), true, true);
	// event.draw_clusters(0, true, kBlack);
	// // event.draw_trackcurve(0, true, TColor::GetColor(88, 171, 141), true, true, false);
	// // event.draw_trackline(0, true, TColor::GetColor(88, 171, 141), true, true, false);
	// c->cd(2);
	// event.draw_truthline(1, false, TColor::GetColor(232, 85, 98));
	// // event.draw_tracklets(1, false, kGray + 1, false, false);
	// // event.draw_tracklets(1, true, TColor::GetColor(0, 118, 186), true, true);
	// event.draw_clusters(1, true, kBlack);
	// // event.draw_trackline(1, true, TColor::GetColor(88, 171, 141), true, true, false);
	// c->Print(c->GetName());
	// TCanvas *c0 = new TCanvas("c0", "c0", 1200, 600);
	// c0->Divide(2, 1);

	c->cd(1);
	// c0->cd(1);
	event.draw_frame(0);
	event.draw_intt(0);
	// event.draw_truthline(0, false, TColor::GetColor(232, 85, 98));
	// event.draw_truthcurve(0, false, TColor::GetColor(232, 85, 98));
	// event.draw_tracklets(0, false, kGray + 1, false, false);
	// event.draw_tracklets(0, true, TColor::GetColor(0, 118, 186), true, true);
	event.draw_clusters(0, true, kBlack);
	// event.draw_trackcurve(0, true, TColor::GetColor(88, 171, 141), true, true, false);
	if (!(no_mc))
	  cout << "draw_truth" << endl;
	if (mag_on)
	  {
	    // if (!(no_mc))
	    //     event.draw_truthcurve(0, true, kGray + 1);
	    // event.draw_trackcurve(0, true, TColor::GetColor(88, 171, 141), true, true, false);
	    event.draw_trackcurve(0, true, TColor::GetColor(232, 85, 98), true, true, false);
	  }
	else
	  {
	    // if (!(no_mc))
	    //     event.draw_truthline(0, true, kGray + 1);
	    event.draw_trackline(0, true, TColor::GetColor(232, 85, 98) /*TColor::GetColor(88, 171, 141)*/, true, true, false);

	    // event.draw_truthline(0, true, TColor::GetColor(232, 85, 98));
	  }
	// event.draw_tracklets(0, true, kGray + 1, true, true);
	c->cd(2);
	// c0->cd(2);
	event.draw_frame(1);
	event.draw_intt(1);
	// event.draw_tracklets(1, false, kGray + 1, false, false);
	// event.draw_tracklets(1, true, TColor::GetColor(0, 118, 186), true, true);
	event.draw_clusters(1, true, kBlack);
	// if (!(no_mc))
	//     event.draw_truthline(1, true, kGray + 1);
	event.draw_trackline(1, true, TColor::GetColor(232, 85, 98), true, true, false);
	// event.draw_trackline(1, true, TColor::GetColor(88, 171, 141), true, true, false);

	// event.draw_truthline(1, false, TColor::GetColor(232, 85, 98));
	// event.draw_tracklets(1, true, kGray + 1, true, true);
	// if(ievt == 56)
	// c->Write();
	// c0->Write();

	c->Print(c->GetName());

	/*c->cd(1);
	  event.draw_clusters(0, false, kBlack);
	  if(mag_on)
	  {
	  event.draw_trackcurve(0, true, TColor::GetColor(88, 171, 141), true, true, false);
	  event.draw_truthcurve(0, true, TColor::GetColor(232, 85, 98));
	  }
	  else{
	  event.draw_trackline(0, true, TColor::GetColor(88, 171, 141), true, true, false);
	  event.draw_truthline(0, true, TColor::GetColor(232, 85, 98));
	  }
	  c->cd(2);
	  event.draw_truthline(1, false, TColor::GetColor(232, 85, 98));
	  event.draw_clusters(1, true, kBlack);
	  event.draw_trackline(1, true, TColor::GetColor(88, 171, 141), true, true, false);

	  c->Print(c->GetName());*/

	// c->cd(1);
	// event.draw_clusters(0, false, kBlack);
	// event.draw_trackline(0, true, TColor::GetColor(88, 171, 141), true, true, false);
	// c->cd(2);
	// event.draw_clusters(1, false, kBlack);
	// event.draw_trackline(1, true, TColor::GetColor(88, 171, 141), true, true, false);
	// c->Print(c->GetName());
	// c->cd(1);
	// event.draw_clusters(0, false, kBlack);
	// event.draw_trackline(0, true, TColor::GetColor(88, 171, 141), true, true, false);
	// c->cd(2);
	// event.draw_clusters(1, false, kBlack);
	// event.draw_trackline(1, true, TColor::GetColor(88, 171, 141), true, true, false);
	// c->Print(c->GetName());

	// c->cd(1);
	// // event.draw_trackline(0, false, TColor::GetColor(88, 171, 141), true, true, false);
	// event.draw_truthline(0, false, kRed);
	// event.draw_clusters(0, true, kBlack);

	// c->cd(2);
	// // event.draw_trackline(1, false, TColor::GetColor(88, 171, 141), true, true, false);
	// event.draw_truthline(1, false,kRed);
	// event.draw_clusters(1, true, kBlack);
	// c->Print(c->GetName());

	// c->cd(1);
	// event.draw_truthline(0, false, kBlue, true);
	// event.draw_clusters(0, true, kBlack);

	// c->cd(2);
	// event.draw_truthline(1, false, kBlue, true);
	// event.draw_clusters(1, true, kBlack);
	// c->Print(c->GetName());
      }
      event.clear();


    }

  // c->Print(c->GetName());
  c->Print(((string)c->GetName() + "]").c_str());

  froot->Write();
  for (int ihst = 0; ihst < h_.size(); ihst++)
    {
      froot->WriteTObject(h_[ihst], h_[ihst]->GetName());
    }
  froot->Close();
}
