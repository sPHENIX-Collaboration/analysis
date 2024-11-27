#define Analysis_cc

#include "Analysis.hh"

/////////////////////////////////////////////////////////////////////////
// Constractor                                                         //
/////////////////////////////////////////////////////////////////////////
Analysis::Analysis(int run_no, int fphx_bco, bool mag_on, bool debug, bool is_preliminary  ) :
  run_no_( run_no ),
  fphx_bco_( fphx_bco ),
  mag_on_( mag_on ),
  debug_( debug ),
  is_preliminary_( is_preliminary )
{
}

/////////////////////////////////////////////////////////////////////////
// private functions                                                    //
/////////////////////////////////////////////////////////////////////////
void Analysis::Init()
{

  //  this->InitResetVariables();
  this->InitIO();
  this->InitHist();
  this->InitTree();
}

void Analysis::InitIO()
{
  this->InitInput();
  this->InitOutput();
}

void Analysis::InitInput()
{

  if( fphx_bco_ == -1 )
    fname_input_ = data_dir_ + Form("InttAna_run%d.root", run_no_ ); // with no any cut
  else
    fname_input_ = data_dir_  + Form("InttAna_run%d_FPHX_BCO_%d.root", run_no_, fphx_bco_ ); // FPHX BCO selection
  
  // if (mag_on)
  //   fname = Form("AnaTutorial_inttana_%d_mag.root", run_no_); // with no any cut

  if (is_geant_)
    {
      if (mag_on_)
        {
	  fname_input_ = "/sphenix/tg/tg01/commissioning/INTT/work/tsujibata/tracking/F4AInttRead/macro/macros/detectors/sPHENIX/AnaTutorial_inttana_zvtxwidth10_ptmin0.root";
        }

      else
	{
	  fname_input_ = "/sphenix/tg/tg01/commissioning/INTT/work/tsujibata/tracking/F4AInttRead/macro/macros/detectors/sPHENIX/AnaTutorial_inttana_vtx0.root";
	}
    }

  cout << fname_input_ << endl;
  f_input_ = TFile::Open( fname_input_.c_str() );

  // from input file
  ntp_clus_ = (TNtuple *)f_input_->Get( "ntp_clus" );
  ntp_clus_->SetBranchAddress( "nclus"	, &nclus_	);
  ntp_clus_->SetBranchAddress( "nclus2"	, &nclus2_	);
  ntp_clus_->SetBranchAddress( "bco_full", &bco_full_	);
  ntp_clus_->SetBranchAddress( "evt"	, &evt_		);
  ntp_clus_->SetBranchAddress( "size"	, &size_	);
  ntp_clus_->SetBranchAddress( "adc"	, &adc_		);
  ntp_clus_->SetBranchAddress( "x"	, &x_		);
  ntp_clus_->SetBranchAddress( "y"	, &y_		);
  ntp_clus_->SetBranchAddress( "z"	, &z_		);
  ntp_clus_->SetBranchAddress( "lay"	, &lay_		);
  ntp_clus_->SetBranchAddress( "lad"	, &lad_		);
  ntp_clus_->SetBranchAddress( "sen"	, &sen_		);
  ntp_clus_->SetBranchAddress( "x_vtx"	, &x_vtx_	);
  ntp_clus_->SetBranchAddress( "y_vtx"	, &y_vtx_	);
  ntp_clus_->SetBranchAddress( "z_vtx"	, &z_vtx_	);

  ntp_evt_ = (TNtuple*)f_input_->Get( "ntp_evt" );
  ntp_evt_->SetBranchAddress( "zv", &vertex_z_ );

  bco_tree_ = (TTree*)f_input_->Get( "t_evt_bco" );
  bco_tree_->SetBranchAddress( "bco_intt", &bco_intt_ );
  
  hepmctree_ = (TTree *)f_input_->Get( "hepmctree" );
  hepmctree_->SetBranchAddress( "m_evt"		, &m_evt_		);
  hepmctree_->SetBranchAddress( "m_status"	, &m_status_		);
  hepmctree_->SetBranchAddress( "m_truthpx"	, &m_truthpx_		);
  hepmctree_->SetBranchAddress( "m_truthpy"	, &m_truthpy_		);
  hepmctree_->SetBranchAddress( "m_truthpz"	, &m_truthpz_		);
  hepmctree_->SetBranchAddress( "m_truthpt"	, &m_truthpt_		);
  hepmctree_->SetBranchAddress( "m_trutheta"	, &m_trutheta_		);
  hepmctree_->SetBranchAddress( "m_truthpid"	, &m_truthpid_		);
  hepmctree_->SetBranchAddress( "m_truththeta"	, &m_truththeta_	);
  hepmctree_->SetBranchAddress( "m_truthphi"	, &m_truthphi_		);
  hepmctree_->SetBranchAddress( "m_xvtx"	, &m_xvtx_		);
  hepmctree_->SetBranchAddress( "m_yvtx"	, &m_yvtx_		);
  hepmctree_->SetBranchAddress( "m_zvtx"	, &m_zvtx_		);

  no_mc_ = true;
  if( hepmctree_ == nullptr )
    no_mc_ = true;
  else if( hepmctree_->GetEntries() == 0 )    
    no_mc_ = true;

}

void Analysis::InitOutput()
{
  string fname_output_basename = "tracking_run" + to_string( run_no_ );
  if( fphx_bco_ != -1 )
    fname_output_basename += "_fphx_bco_" + to_string( fphx_bco_ );
  
  fname_output_ = dir_output_ + fname_output_basename;  
  fname_output_ += ".root";
  
  f_output_ = new TFile(fname_output_, "recreate" );
  
  c_ = new TCanvas(fname_output_.ReplaceAll(".root", ".pdf" ), "c", 1200, 600);
  output_pdf_ = dir_output_ + fname_output_basename + ".pdf";
  output_good_pdf_ = dir_output_ + fname_output_basename + "_good.pdf";

  this->InitCanvas();
  c_->cd(1);
  c_->Print( (output_pdf_+"[").c_str() );
  //  c_->Print( (output_good_pdf_+"[").c_str() );

}
void Analysis::InitCanvas()
{
  c_->Clear();
  
  c_->Divide(2, 1);
  double margin = 0.1;
  auto pad1 = c_->cd( 1 );
  pad1->SetRightMargin( margin );
  pad1->SetTopMargin( margin );

  auto pad2 = c_->cd( 2 );
  pad2->SetRightMargin( margin );
  pad2->SetTopMargin( margin );
}

void Analysis::InitHist()
{
  
  h_dcaz_one_		= new TH1F("h_dcaz_one", "", 100, -200, 200);
  h_nclus_		= new TH1F("h_nclus", "", 100, 0, 100);
  h_dphi_nocut_		= new TH2F("h_dphi_nocut", "d_phi : phi_in;phi_in;d_phi", 200, -3.2, 3.2, 200, -0.1, 0.1);
  h_dcaz_sigma_one_	= new TH1F("h_dcaz_sigma_one", "h_dcaz_sigma_one;dca_z sigma of one event;entries", 100, 0, 15);
  h_xvtx_		= new TH1D("h_xvtx", "h_xvtx", 100, -0.01, 0.01);
  h_yvtx_		= new TH1D("h_yvtx", "h_yvtx", 100, -0.01, 0.01);
  h_zvtx_		= new TH1D("h_zvtx", "h_zvtx", 100, -20, 20);
  h_dcax_		= new TH1F("h_dcax", "h_dcax", 100, -0.4, 0.4);
  h_dcay_		= new TH1F("h_dcay", "h_dcay", 100, -0.4, 0.4);
  h_dcaz_		= new TH1F("h_dcaz", "h_dcaz;DCA_z[cm]", 60, -150, 150);
  h_dtheta_		= new TH1F("h_dtheta", "dtheta{inner - outer};dtheta;entries", 100, -3.2, 3.2);
  h_dphi_		= new TH1F("h_dphi", "#Delta #phi_{AB};#Delta #phi_{AB}", 100, -1, 1);
  h_dca2d_		= new TH1F("h_dca2d", "h_dca", 100, -10, 10);

  // for (int i = 0; i < 2; i++)
  //   h_zr_[i] = new TH2D(Form("h_zr_%d", i), "h_zr;z;r", 100, -20, 20, 100, -10, 10);

}

void Analysis::InitTree()
{
  
  temp_tree_ = new TTree( "temp_tree", "temp_tree" );
  temp_tree_->Branch( "evt_temp_"	, &evt_temp_, "evt_temp_/I"		);
  temp_tree_->Branch( "d_phi_"		, &d_phi_				);
  temp_tree_->Branch( "d_theta_"	, &d_theta_				);
  temp_tree_->Branch( "phi_in_"		, &phi_in_				);
  temp_tree_->Branch( "dca_x_"		, &dca_x_				);
  temp_tree_->Branch( "dca_y_"		, &dca_y_				);
  temp_tree_->Branch( "dca_z_"		, &dca_z_				);
  temp_tree_->Branch( "dca_2d_"		, &dca_2d_				);
  temp_tree_->Branch( "zvtx_one_"	, &zvtx_one_, "zvtx_one_/D"		);
  temp_tree_->Branch( "zvtx_sigma_one_"	, &zvtx_sigma_one_, "zvtx_sigma_one_/D"	);
  
  clus_tree_ = new TTree( "clus_tree", "clus_tree" );
  clus_tree_->Branch( "evt_clus"		, &evt_clus_, "evt_clus/I"	);
  clus_tree_->Branch( "x_in"			, &x_in_			); // x of clusters on the inner barrel
  clus_tree_->Branch( "y_in"			, &y_in_			); // y of clusters on the inner barrel
  clus_tree_->Branch( "z_in"			, &z_in_			); // z of clusters on the inner barrel
  clus_tree_->Branch( "r_in"			, &r_in_			); // r of clusters on the inner barrel
  clus_tree_->Branch( "size_in"			, &size_in_			); // size of inner cluster
  clus_tree_->Branch( "phi_in"			, &phi_in_			);
  clus_tree_->Branch( "theta_in"       		, &theta_in_			);
  clus_tree_->Branch( "adc_in"			, &adc_in_			);
  clus_tree_->Branch( "is_associated_in"	, &is_associated_in_		);
  clus_tree_->Branch( "track_incoming_theta_in"	, &track_incoming_theta_in_	);  
  clus_tree_->Branch( "x_out"			, &x_out_			); // x of clusters on the inner barrel
  clus_tree_->Branch( "y_out"			, &y_out_			); // y of clusters on the inner barrel
  clus_tree_->Branch( "z_out"			, &z_out_			); // z of clusters on the inner barrel
  clus_tree_->Branch( "r_out"			, &r_out_			); // r of clusters on the inner barrel
  clus_tree_->Branch( "size_out"		, &size_out_			); // size of outer cluster
  clus_tree_->Branch( "phi_out"			, &phi_out_			);
  clus_tree_->Branch( "theta_out"		, &theta_out_			);
  clus_tree_->Branch( "adc_out"			, &adc_out_			);
  clus_tree_->Branch( "is_associated_out"	, &is_associated_out_		);
  clus_tree_->Branch( "track_incoming_theta_out", &track_incoming_theta_out_	);
  clus_tree_->Branch( "z_vertex"		, &z_vertex_, "z_vertex/D"	);

  track_tree_ = new TTree( "track_tree", "track_tree" );
  track_tree_->Branch( "evt_track"	, &evt_track_, "evt_track/I"	);
  track_tree_->Branch( "ntrack"		, &ntrack_, "ntrack/I"		);
  track_tree_->Branch( "phi_tracklets"	, &phi_tracklets_		);
  track_tree_->Branch( "slope_rz"	, &slope_rz_			);
  track_tree_->Branch( "theta_tracklets"	, &theta_tracklets_		);
  track_tree_->Branch( "phi_track"	, &phi_track_			);
  track_tree_->Branch( "theta_track"	, &theta_track_			);
  track_tree_->Branch( "z_tracklets_out"	, &z_tracklets_out_		);
  track_tree_->Branch( "dca_2d"		, &dca_2d_			);
  track_tree_->Branch( "dca_z"		, &dca_z_			);
  track_tree_->Branch( "is_deleted"	, &is_deleted_			);
  track_tree_->Branch( "dca_range_out"	, &dca_range_out_		);
  track_tree_->Branch( "dcaz_range_out"	, &dcaz_range_out_		);
  track_tree_->Branch( "dca2d_range_out"	, &dca2d_range_out_		);
  track_tree_->Branch( "pT"		, &pT_				);
  track_tree_->Branch( "px"		, &px_				);
  track_tree_->Branch( "py"		, &py_				);
  track_tree_->Branch( "pz"		, &pz_				);
  track_tree_->Branch( "px_truth"	, &px_truth_			);
  track_tree_->Branch( "py_truth"	, &py_truth_			);
  track_tree_->Branch( "pz_truth"	, &pz_truth_			);
  track_tree_->Branch( "pT_truth"	, &pT_truth_			);
  track_tree_->Branch( "charge"		, &charge_			);
  track_tree_->Branch( "rad"		, &rad_				);
  // track_tree_->Branch( "x_vertex"	, &x_vertex, "x_vertex/D"	);
  // track_tree_->Branch( "y_vertex"	, &y_vertex, "y_vertex/D"	);
  // track_tree_->Branch( "z_vertex"	, &z_vertex, "z_vertex/D"	);
  track_tree_->Branch( "vertex"	, &vertex_ ); // , "z_vertex/D"	);
  
  truth_tree_ = new TTree( "truth_tree", "truth_tree" );
  truth_tree_->Branch( "evt_truth"	, &evt_truth_, "evt_truth/I"	);
  truth_tree_->Branch( "ntruth_cut"	, &ntruth_cut_, "ntruth_cut/I"	);
  truth_tree_->Branch( "truthpx"		, &truthpx_			);
  truth_tree_->Branch( "truthpy"		, &truthpy_			);
  truth_tree_->Branch( "truthpz"		, &truthpz_			);
  truth_tree_->Branch( "truthpt"		, &truthpt_			);
  truth_tree_->Branch( "trutheta"	, &trutheta_			);
  truth_tree_->Branch( "truththeta"	, &truththeta_			);
  truth_tree_->Branch( "truthphi"	, &truthphi_			);
  truth_tree_->Branch( "truthxvtx"	, &truthxvtx_			);
  truth_tree_->Branch( "truthyvtx"	, &truthyvtx_			);
  truth_tree_->Branch( "truthzvtx"	, &truthzvtx_			);
  truth_tree_->Branch( "truthzout"	, &truthzout_			);
  truth_tree_->Branch( "truthpid"	, &truthpid_			);
  truth_tree_->Branch( "status"		, &status_			);

}

void Analysis::ResetVariables()
{

  this->ResetTempTreeVariables();
  this->ResetClustTreeVariables();
  this->ResetTruthTreeVariables();
  this->ResetTrackTreeVariables();
  this->ResetHepMcTreeVariables();
}

void Analysis::ResetTempTreeVariables()
{
  zvtx_one_     = zvtx_sigma_one_     = -9999;

  dca_x_.clear();
  dca_y_.clear();
  dca_z_.clear();
  dca_2d_.clear();
  d_phi_.clear();
  d_theta_.clear();
  phi_in_.clear();
}

void Analysis::ResetClustTreeVariables()
{
  x_in_.clear();
  y_in_.clear();
  z_in_.clear();
  r_in_.clear();
  size_in_.clear();
  phi_in_.clear();
  theta_in_.clear();
  adc_in_.clear();
  is_associated_in_.clear();
  track_incoming_theta_in_.clear();
     
  x_out_.clear();
  y_out_.clear();
  z_out_.clear();
  r_out_.clear();
  size_out_.clear();
  phi_out_.clear();
  theta_out_.clear();
  adc_out_.clear();
  is_associated_out_.clear();
  track_incoming_theta_out_.clear();
}

void Analysis::ResetTruthTreeVariables()
{

  evt_truth_ = ntruth_cut_ = -9999;
  truthpx_.clear();
  truthpy_.clear();
  truthpz_.clear();
  truthpt_.clear();
  trutheta_.clear();
  truththeta_.clear();
  truthphi_.clear();
  truthxvtx_.clear();
  truthyvtx_.clear();
  truthzvtx_.clear();
  truthzout_.clear();
  truthpid_.clear();
  status_.clear();
}

void Analysis::ResetTrackTreeVariables() // doesn't work at all!
{
  evt_track_ =  ntrack_ = -9999;
  
  phi_tracklets_.clear();
  slope_rz_.clear();
  theta_tracklets_.clear();
  phi_track_.clear();
  theta_track_.clear();
  z_tracklets_out_.clear();
  dca_2d_.clear();
  dca_z_.clear();
  is_deleted_.clear();
  dca_range_out_.clear();
  dcaz_range_out_.clear();
  dca2d_range_out_.clear();
  pT_.clear();
  px_.clear();
  py_.clear();
  pz_.clear();
  px_truth_.clear();
  py_truth_.clear();
  pz_truth_.clear();
  pT_truth_.clear();
  charge_.clear();
  rad_.clear();
  vertex_ = TVector3();

}

void Analysis::ResetHepMcTreeVariables()
{
  m_evt_ =  m_status_ =   m_truthpid_ = -9999;
  
  m_truthpx_ = m_truthpy_ = m_truthpz_ = m_truthpt_
    = m_trutheta_ = m_truththeta_ = m_truthphi_
    = m_xvtx_ = m_yvtx_ = m_zvtx_
    = -9999;

}

void Analysis::ProcessEvent()
{
  
  int sum_event = 0;
  int ntrack_ = 0;
  for (int icls = 0; icls < ntp_clus_->GetEntries(); icls++)
    {
      cout << Form("  -----  event %d {event gene.}  -----  ", sum_event) << endl;

      clustEvent event{};
      cluster clust{};

      ntp_clus_->GetEntry(icls); // first cluster in one event

      clust.set(evt_, 0, x_, y_, z_, adc_, size_, lay_, x_vtx_, y_vtx_, z_vtx_ );
      event.vclus.push_back(clust);

      for (int j = 1; j < nclus_; j++) // second cluster in one event
        {
	  ntp_clus_->GetEntry(++icls);
	  cluster clust2{};
	  clust2.set(evt_, 0, x_, y_, z_, adc_, size_, lay_, x_vtx_, y_vtx_, z_vtx_ );
	  event.vclus.push_back(clust2);
        }

      dotracking(event, h_dphi_nocut_ );
      
      ntrack_ = event.vtrack.size();
      h_dcaz_one_->Reset();
      for (int itrk = 0; itrk < ntrack_; itrk++)
        {
	  h_dcaz_one_->Fill(event.vtrack[itrk]->dca[2]);
        }
      zvtx_one_       = h_dcaz_one_->GetMean();
      zvtx_sigma_one_ = h_dcaz_one_->GetStdDev();

      evt_temp_ = sum_event;
      for (int itrk = 0; itrk < ntrack_; itrk++)
        {
	  dca_x_.push_back( event.vtrack[itrk]->dca[0] );
	  dca_y_.push_back( event.vtrack[itrk]->dca[1] );
	  dca_z_.push_back( event.vtrack[itrk]->dca[2] );
	  dca_2d_.push_back( event.vtrack[itrk]->dca_2d );
	  d_phi_.push_back( event.vtrack[itrk]->d_phi );
	  d_theta_.push_back( event.vtrack[itrk]->d_theta );
	  phi_in_.push_back( event.vtrack[itrk]->phi_in );
        }
      
      temp_tree_->Fill();      
      this->ResetTempTreeVariables();
      
      event.clear();
      sum_event++;
      
      if (debug_ && sum_event > nloop_)
	break;
      
    } // end of for (int icls = 0; icls < ntp_clus_->GetEntries(); icls++)
  
  n_dotracking++;
  
  temp_tree_->Draw( "dca_x_>>h_dcax" );
  temp_tree_->Draw( "dca_y_>>h_dcay" );
  temp_tree_->Draw( "dca_z_>>h_dcaz" );
  temp_tree_->Draw( "d_theta_>>h_dtheta" );
  temp_tree_->Draw( "d_phi_>>h_dphi" );
  temp_tree_->Draw( "dca_2d_>>h_dca2d" );

  //vector < TH1F* > h_ = {h_dcax, h_dcay, h_dcaz, h_dphi, h_dtheta, h_dca2d};

  // the mean of DCA in all event
  double dcax_mean = h_dcax_->GetMean();
  double dcay_mean = h_dcay_->GetMean();
  if (!(is_geant_))
    {
      dcax_mean = -0.019;
      dcay_mean = 0.198;
    }
  double dcaz_mean = h_dcaz_->GetMean();
  double dca2d_mean = h_dca2d_->GetMean();

  double dcax_sigma = h_dcax_->GetStdDev();
  double dcay_sigma = h_dcay_->GetStdDev();
  double dcaz_sigma = h_dcaz_->GetStdDev();
  double dca2d_sigma = h_dca2d_->GetStdDev();

  dca2d_sigma *= sigma_;
  dcaz_sigma *= sigma_;

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
      ntp_evt_->GetEntry( ievt );
      bco_tree_->GetEntry( ievt );
      
      cout << "\r" << "event " << setw(10) << right << ievt;
      ntruth_ = 0;
      ntruth_cut_ = 0;
	
      clustEvent event;
      event.ievt = ievt;
      event.mag_on = mag_on_;
      event.run_no = run_no_;
      event.bco_intt = bco_intt_;

      if(ihit < ntp_clus_->GetEntries() )
        {
	  ntp_clus_->GetEntry(ihit);

	  if (!( no_mc_ ))
            {
	      hepmctree_->GetEntry(itruth);
	      while ( m_evt_ != evt_ )
                {
		  itruth++;
		  hepmctree_->GetEntry(itruth);
                }
	      temp_evt = m_evt_;
            }

	  cluster clust{};
	  clust.set(evt_, 0, x_, y_, z_, adc_, size_, lay_, x_vtx_, y_vtx_, z_vtx_ );
	  event.vclus.push_back(clust);

	  for (int j = 1; j < nclus_; j++) // second~ cluster in one event
            {
	      ntp_clus_->GetEntry(++ihit);
	      cluster clust2{};

	      clust2.set(evt_, 0, x_, y_, z_, adc_, size_, lay_, x_vtx_, y_vtx_, z_vtx_ );
	      event.vclus.push_back(clust2);
            }

	  if (!( no_mc_ ))
            {
	      while (m_evt_ == temp_evt)
                {
		  ntruth_++;
		  truth *tru = new truth();
		  tru->set_truth(m_truthpx_, m_truthpy_, m_truthpz_, m_truthpt_,
				 m_status_, m_trutheta_, m_truthpid_,
				 m_truththeta_, m_truthphi_,
				 m_xvtx_, m_yvtx_, m_zvtx_);
		  
		  event.vtruth.push_back(tru);
		  
		  itruth++;
		  if (itruth == hepmctree_->GetEntries())
		    break;

		  hepmctree_->GetEntry(itruth);
                }
	      itruth--;
            }
        }

      event.dca_mean[0] = dcax_mean;
      event.dca_mean[1] = dcay_mean;
      event.dca_mean[2] = zvtx_one_;

      // from dca_mean
      dotracking(event, h_dphi_nocut_ );
      // event.Print();
      // cout << zvtx_one_ << "\t" << z_vtx_ << endl;
      
      ntrack_ = event.vtrack.size();
      evt_clus_ = ievt;
      evt_track_ = ievt;
      evt_truth_ = ievt;
      h_dcaz_one_->Reset();
      for (int itrk = 0; itrk < ntrack_; itrk++) // from dca_mean
        {
	  h_dcaz_one_->Fill(event.vtrack[itrk]->dca[2]);
        }
      
      double dcaz_mean_one = h_dcaz_one_->GetMean();    // one event

      // if( fabs(vertex_z_) < 300 )
      // 	cout << dcaz_mean << "\t"
      // 	     << dcaz_mean_one << "\t"
      // 	     << vertex_z_ << endl;
      
      double dcaz_sigma_one = h_dcaz_one_->GetStdDev(); // one event
      h_dcaz_sigma_one_->Fill(dcaz_sigma_one);
      
      dcaz_sigma_one *= sigma_;
      double dcaz_max_one = dcaz_mean_one + dcaz_sigma;
      double dcaz_min_one = dcaz_mean_one - dcaz_sigma;
      zvtx_sigma_one_ *= sigma_;
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

      if (!( no_mc_ ))
        {
	  for (int itrt = 0; itrt < ntruth_; itrt++)
            {
	      event.vtruth[itrt]->dca_mean[0] = dcax_mean;
	      event.vtruth[itrt]->dca_mean[1] = dcay_mean;
	      event.vtruth[itrt]->dca_mean[2] = dcaz_mean_one;

	      event.vtruth[itrt]->calc_line();
	      event.vtruth[itrt]->calc_center();
            }
        }
      h_nclus_->Fill(event.vclus.size());

      for (int iclus = 0; iclus < event.vclus.size(); iclus++)
        {
	  event.vclus[iclus].dca_mean[0] = dcax_mean;
	  event.vclus[iclus].dca_mean[1] = dcay_mean;
	  event.vclus[iclus].dca_mean[2] = dcaz_mean_one;
	  z_vertex_ = dcaz_mean_one;

	  if (event.vclus[iclus].layer < 2)
            {
	      x_in_.push_back				( event.vclus[iclus].x				);
	      y_in_.push_back				( event.vclus[iclus].y				);
	      z_in_.push_back				( event.vclus[iclus].z				);
	      r_in_.push_back				( event.vclus[iclus].r				);
	      size_in_.push_back			( event.vclus[iclus].size			);
	      phi_in_.push_back				( event.vclus[iclus].getphi_clus()		);
	      theta_in_.push_back			( event.vclus[iclus].gettheta_clus()		);
	      adc_in_.push_back				( event.vclus[iclus].adc			);
	      is_associated_in_.push_back		( event.vclus[iclus].is_associated		);
	      track_incoming_theta_in_.push_back	( event.vclus[iclus].track_incoming_theta	);
	      //event.vclus[iclus]_.print(								);
	      
            }
	  if (1 < event.vclus[iclus].layer)
            {
	      x_out_.push_back				( event.vclus[iclus].x				);
	      y_out_.push_back				( event.vclus[iclus].y				);
	      z_out_.push_back				( event.vclus[iclus].z				);
	      r_out_.push_back				( event.vclus[iclus].r				);
	      size_out_.push_back			( event.vclus[iclus].size			);
	      phi_out_.push_back			( event.vclus[iclus].getphi_clus()		);
	      theta_out_.push_back			( event.vclus[iclus].gettheta_clus()		);
	      adc_out_.push_back			( event.vclus[iclus].adc			);
	      is_associated_out_.push_back		( event.vclus[iclus].is_associated		);
	      track_incoming_theta_out_.push_back	( event.vclus[iclus].track_incoming_theta	);
            }
        }

      clus_tree_->Fill();
      this->ResetClustTreeVariables();

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
      
      event.dca_check(debug_);
      event.makeonetrack();

      event.charge_check();
      event.truth_check();

      event.cluster_check();
      event.track_check();
      ntrack_ = 0;
      for (int itrk = 0; itrk < ntrack_; itrk++)
        {
	  slope_rz_.push_back		(event.vtrack[itrk]->track_rz->GetParameter(1));
	  phi_tracklets_.push_back	(event.vtrack[itrk]->getphi_tracklet());
	  theta_tracklets_.push_back	(event.vtrack[itrk]->gettheta_tracklet());
	  phi_track_.push_back		(event.vtrack[itrk]->getphi());
	  theta_track_.push_back		(event.vtrack[itrk]->gettheta());
	  dca_z_.push_back		(event.vtrack[itrk]->dca[2]);
	  dca_2d_.push_back		(event.vtrack[itrk]->dca_2d);
	  z_tracklets_out_.push_back	(event.vtrack[itrk]->p2.z());
	  pT_.push_back			(event.vtrack[itrk]->pT);
	  px_.push_back			(event.vtrack[itrk]->p_reco[0]);
	  py_.push_back			(event.vtrack[itrk]->p_reco[1]);
	  pz_.push_back			(event.vtrack[itrk]->p_reco[2]);
	  rad_.push_back			(event.vtrack[itrk]->rad);
	  charge_.push_back		(event.vtrack[itrk]->charge);
	  if (!( no_mc_ ))
            {
	      pT_truth_.push_back(event.vtruth[0]->m_truthpt);
	      px_truth_.push_back(event.vtruth[0]->m_truthpx);
	      py_truth_.push_back(event.vtruth[0]->m_truthpy);
	      pz_truth_.push_back(event.vtruth[0]->m_truthpz);
            }

	  is_deleted_.push_back(event.vtrack[itrk]->is_deleted);
	  dca2d_range_out_.push_back(event.vtrack[itrk]->dca2d_range_out);
	  dcaz_range_out_.push_back(event.vtrack[itrk]->dcaz_range_out);
	  dca_range_out_.push_back(event.vtrack[itrk]->dca_range_out);

	  // x_vertex_ = event.vtrack[itrk]->dca_mean[0];
	  // y_vertex_ = event.vtrack[itrk]->dca_mean[1];
	  z_vertex_ = event.vtrack[itrk]->dca_mean[2];
	  vertex_.SetXYZ( event.vtrack[itrk]->dca_mean[0],
			  event.vtrack[itrk]->dca_mean[1],
			  event.vtrack[itrk]->dca_mean[2] );

	  if (event.vtrack[itrk]->is_deleted == true)
	    continue;
	  if (event.vtrack[itrk]->dca_range_out == true)
	    continue;

	  ntrack_++;

	  // h_xvtx->Fill(x_vertex_);
	  // h_yvtx->Fill(y_vertex_);
	  // h_zvtx->Fill(z_vertex_);

	  h_xvtx_->Fill( vertex_.X() );
	  h_yvtx_->Fill( vertex_.Y() );
	  h_zvtx_->Fill( vertex_.Z() );
        }

      track_tree_->Fill();
      double temp = z_vertex_;
      this->ResetTrackTreeVariables();
      z_vertex_ = temp;
      
      if ( !( no_mc_ ) )
        {
	  for (int itrt = 0; itrt < ntruth_; itrt++)
            {

	      if (event.vtruth[itrt]->is_charged == false)
		continue;

	      if (event.vtruth[itrt]->is_intt == false)
		continue;
	      
	      ntruth_cut_++;

	      truthpx_.push_back	( event.vtruth[itrt]->m_truthpx		);
	      truthpy_.push_back	( event.vtruth[itrt]->m_truthpy		);
	      truthpz_.push_back	( event.vtruth[itrt]->m_truthpz		);
	      truthpt_.push_back	( event.vtruth[itrt]->m_truthpt		);
	      trutheta_.push_back	( event.vtruth[itrt]->m_trutheta	);
	      truththeta_.push_back	( event.vtruth[itrt]->m_truththeta	);
	      truthphi_.push_back	( event.vtruth[itrt]->m_truthphi	);
	      truthpid_.push_back	( event.vtruth[itrt]->m_truthpid	);
	      status_.push_back		( event.vtruth[itrt]->m_status		);
	      truthxvtx_.push_back	( event.vtruth[itrt]->m_xvtx		);
	      truthyvtx_.push_back	( event.vtruth[itrt]->m_yvtx		);
	      truthzvtx_.push_back	( event.vtruth[itrt]->m_zvtx		);
	      truthzout_.push_back	( event.vtruth[itrt]->m_truthz_out	);
            }

	  truth_tree_->Fill();
	  this->ResetTruthTreeVariables();

        }

      //      if( event.ievt > 39600 )
      this->ProcessEvent_Draw( event );
      
      event.clear();
      event_counter_++;

      this->ResetVariables(); // variables need to be reset at the end of event
      // if( ievt >20 )
      // 	break;
      
      // if( page_counter_ > page_num_limit_ )
      // 	break;
      
    } // End of  for (int ievt = 0; ievt < sum_event; ievt++, ihit++, itruth++)

}


void Analysis::ProcessEvent_Draw( clustEvent event )
{
  // drawing
  // if (fabs(event.vclus.size() - 2 * ntrack) < 5 && /*debug_ &&*/ 10 < event.vclus.size() && event.vclus.size() < 50 /* && && is_geant_ &&*/ /*ievt < 500*/)
  // if (ievt < nloop && ievt == 56 /*&& ievt==56*/ /*&& fabs(event.dca_mean[2]) < 2. && ntrack < 20*/)
  
  {
    if( page_counter_ > page_num_limit_ )
      return;

    double vertex_range_z = 15;
    if( event.dca_mean[2] < -vertex_range_z || event.dca_mean[2] > vertex_range_z )
      return;

    int track_num_min = 5, track_num_max = 200;
    int cluster_num_tolerance = 5;
    if( event.vclus.size() < track_num_min * 2 || event.vclus.size() > track_num_max * 2 + cluster_num_tolerance )
      return;
    
    int good_track_num = event.GetGoodTrackNum( true, true, false ); // dca_range_cut, is_deleted, reverse
    if( good_track_num < track_num_min || track_num_max < good_track_num )
      return;

    double ratio = event.GetAssociatedClusterRatio();
    //    if( ratio < 0.8 || 0.95 < ratio )
    if( ratio < 0.7 || 0.95 < ratio )
      return;

    // double up_down_asymmetry = event.GetTrackUpDownAsymmetry();
    // if( up_down_asymmetry < -0.25 || 0.25 < up_down_asymmetry )
    //   return;
    
    // double left_right_asymmetry = event.GetTrackLeftRightAsymmetry();
    // // if( left_right_asymmetry < -0.25 || 0.25 < left_right_asymmetry )
    // //   return;
    
    cout << endl
	 << "Event " << setw(10) << event_counter_ + 1 << " "
	 << "Page " << setw(3) << page_counter_ + 1 << " "
	 << event.vclus.size() << "\t"
	 << good_track_num << "\t"
	 << ratio << "\t"
	 << event.dca_mean[2] << "\t"
	 // << event.GetTrackUpDownAsymmetry() << "\t"
	 // << event.GetTrackLeftRightAsymmetry()
	 << endl;
    
    this->InitCanvas();
    c_->cd(1);

    event.draw_frame(0, is_preliminary_);
    event.draw_intt(0);
    event.draw_clusters(0, true, kBlack);
    
    if (mag_on_)
      {
	// if (!( no_mc_ ))
	//     event.draw_truthcurve(0, true, kGray + 1);
	// event.draw_trackcurve(0, true, TColor::GetColor(88, 171, 141), true, true, false);
	event.draw_trackcurve(0, true, true, true, false);
      }
    else
      {
	// if (!( no_mc_ ))
	//     event.draw_truthline(0, true, kGray + 1);
	// TColor::GetColor(232, 85, 98), // TColor::GetColor(88, 171, 141),
	event.draw_trackline(0, true, 
			     true, true, false);
	
	// event.draw_truthline(0, true, TColor::GetColor(232, 85, 98));
      }
    
    // event.draw_tracklets(0, true, kGray + 1, true, true);

    c_->cd(2);
    event.draw_frame(1, is_preliminary_);
    event.draw_intt(1);

    // event.draw_tracklets(1, false, kGray + 1, false, false);
    // event.draw_tracklets(1, true, TColor::GetColor(0, 118, 186), true, true);
    event.draw_clusters(1, true, kBlack);
    
    // if (!( no_mc_ ))
    //     event.draw_truthline(1, true, kGray + 1);
    event.draw_trackline(1, true, true, true, false);    
    
    if( page_counter_ < page_num_limit_ )
      {
	c_->Print( output_pdf_.c_str() );

	string output_individual = output_pdf_.substr( 0, output_pdf_.size() - 4 );
	output_individual += "_bco_" + to_string( event.bco_intt );

	if( is_preliminary_ == true )
	  output_individual += "_preliminary.pdf";
	else
	  output_individual += "_internal.pdf";

	cout << "individual output : " << output_individual << endl;
	c_->Print( output_individual.c_str() );
      }

    string canvas_name = string("canvas_") + to_string( event.ievt );
    f_output_->WriteTObject( c_, canvas_name.c_str() );

    page_counter_++;
  }
  
}

void Analysis::EndProcess()
{
  // c_->Print(c_->GetName());
  c_->Print( (output_pdf_+"]").c_str() );
  //  c_->Print( (output_good_pdf_+"]").c_str() );
  f_output_->Write();

  cout << endl
       << page_counter_ << " pages of "
       << event_counter_ << " events "
       << "(" << 100.0 * page_counter_ / event_counter_ << "%) "
       << "were saved." << endl;
  
  vector < TH1F* > h_ = {h_dcax_, h_dcay_, h_dcaz_, h_dphi_, h_dtheta_, h_dca2d_};
  for (int ihst = 0; ihst < h_.size(); ihst++)
    {
      f_output_->WriteTObject(h_[ihst], h_[ihst]->GetName());
    }

  f_output_->Close();
  f_input_->Close();
}

/////////////////////////////////////////////////////////////////////////
// public functions                                                    //
/////////////////////////////////////////////////////////////////////////

void Analysis::Begin()
{

  this->Init();
  this->ProcessEvent();
  this->EndProcess();
}
