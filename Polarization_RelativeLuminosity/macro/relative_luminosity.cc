#include "polarization.cc"

int relative_luminosity()
{
  SetsPhenixStyle();
  string golden_run_list = "list/Full_ppGoldenRunList_Version2.list";
  vector<int> golden_runs;
  GetGoldenRuns(golden_run_list, golden_runs);
  cout << "- Golden run list: " << golden_run_list << endl;
  cout << "- " << golden_runs.size() << " golden runs taken" << endl;

  string output_base = "results/relative_luminosity";
  string output_root = output_base + ".root";

  TFile* tf = new TFile(output_root.c_str(), "RECREATE");

  /////////////////////////////////////////////////////////////////////////////////////
  // variables
  /////////////////////////////////////////////////////////////////////////////////////
  int run = 0;

  ////////////////////////
  // for Blue ring
  double pol_b[kBunch_num_max] = {-9999.9};        // blue beam polarization for each bunch
  double pol_error_b[kBunch_num_max] = {-9999.9};  // Error of blue beam polarization for each bunch
  double pol_sys_error_b[kBunch_num_max] = {0};
  int pattern_b[kBunch_num_max] = {-9999};  // spin pattern of blue beam

  ////////////////////////
  // for Yellow ring
  double pol_y[kBunch_num_max] = {-9999.9};        // yellow beam polarization for each bunch
  double pol_error_y[kBunch_num_max] = {-9999.9};  // Error of yellow beam polarization for each bunch
  double pol_sys_error_y[kBunch_num_max] = {-9999};
  int pattern_y[kBunch_num_max] = {-9999};  // spin pattern of yellow beam
  
  ////////////////////////
  // for both beam
  Long64_t mbd_scalers_no_cut[kBunch_num_max] = {0};
  double mean_mbd_scalers_no_cut = 0, sigma_mbd_scalers_no_cut = 0;

  /////////////////////////////////////////////////////////////////////////////////////
  // Database setup
  /////////////////////////////////////////////////////////////////////////////////////
  // Get the spin patterns from the spin DB
  //  0xffff is the qa_level from XingShiftCal //////
  unsigned int qa_level = 0;  // 0xffff; // <--- old version
  // qa_level = 0xffff;

  SpinDBContent spin_db_content;
  SpinDBOutput spin_db_output = SpinDBOutput("phnxrc");

  int run_min = *min_element(golden_runs.begin(), golden_runs.end());
  int run_max = *max_element(golden_runs.begin(), golden_runs.end());

  cout << "- Retrieving spin infomation from run " << run_min << " to " << run_max << "...... ";
  // spin_db_output.StoreDBContent(run, run, qa_level);
  spin_db_output.StoreDBContent(run_min, run_max);  // the default QA Lv. is used, <--- use this
  cout << "done" << endl;

  /////////////////////////////////////////////////////////////////////////////////////
  // Histograms setup
  /////////////////////////////////////////////////////////////////////////////////////
  double min_x = 0.8, max_x = 1.2;
  int bin = 80;
  TH1D* hist_lumi_b = new TH1D("hist_lumi_b", "Blue beam relative luminosity;Relative luminosity for TSSA;Integrated luminosity", bin, min_x, max_x);
  HistSetting(hist_lumi_b, GetSphenixColor());

  TH1D* hist_weighted_lumi_b = new TH1D("hist_weighted_lumi_b", "Blue beam relative luminosity with luminosity weight;Relative luminosity = #mathcal{L};Integrated luminosity [a.u.]", bin, min_x, max_x);
  HistSetting(hist_weighted_lumi_b, GetSphenixColor());

  TH1D* hist_lumi_y = new TH1D("hist_lumi_y", "Yellow beam relative luminosity;Relative luminosity for TSSA;Integrated luminosity", bin, min_x, max_x);
  HistSetting(hist_lumi_y, kOrange + 1);

  TH1D* hist_weighted_lumi_y = new TH1D("hist_weighted_lumi_y", "Yellow beam relative luminosity with luminosity weight;Relative luminosity for TSSA;Integrated luminosity [a.u.]", bin, min_x, max_x); // #equiv#font[12]{L}^{#uparrow}/#font[12]{L}^{#downarrow}
  HistSetting(hist_weighted_lumi_y, kOrange + 1);

  Long64_t luminosity_sum = 0;
  for (auto& run : golden_runs)
  {
    auto status = spin_db_output.GetDBContentStore(spin_db_content, run);
    if (status == 0)
    {
      continue;
    }

    bool is_bad_run = bool(spin_db_content.GetBadRunFlag());  // int 0 -> false, int 1 -> true

    if (is_bad_run == true)
    {
      continue;
    }     

    for (int i = 0; i < kBunch_num_max; i++)
    {
      spin_db_content.GetPolarizationBlue(i, pol_b[i], pol_error_b[i], pol_sys_error_b[i]);
      spin_db_content.GetPolarizationYellow(i, pol_y[i], pol_error_y[i], pol_sys_error_y[i]);
      pattern_b[i] = spin_db_content.GetSpinPatternBlue(i);
      pattern_y[i] = spin_db_content.GetSpinPatternYellow(i);

      long long mbd_scaler = spin_db_content.GetScalerMbdNoCut(i);
      mbd_scalers_no_cut[i] = mbd_scaler;
       /*
      cout << setw(4) << i << " "
           << setw(3) << pattern_b[i] << "\t"
           << pol_b[i] << "\t"
           << setw(3) << pattern_y[i] << "\t"
           << pol_y[i] << "\t"
           << mbd_scalers_no_cut[i] << endl;
           */
    }

    /////////////////////////////////////////////////////////////////////////////////////
    // luminosity calc for blue/yellow depending on polarization direction
    // blue beam
    Long64_t lumi_pos_b = GetPolarizedLuminosity(mbd_scalers_no_cut, pattern_b, 1);
    Long64_t lumi_nega_b = GetPolarizedLuminosity(mbd_scalers_no_cut, pattern_b, -1);
    double lumi_ratio_b = 0;
    if (lumi_nega_b != 0)
      lumi_ratio_b = 1. * lumi_pos_b / lumi_nega_b;

    auto lumi_b = lumi_pos_b + lumi_nega_b;

    // yellow beam
    Long64_t lumi_pos_y = GetPolarizedLuminosity(mbd_scalers_no_cut, pattern_y, 1);
    Long64_t lumi_nega_y = GetPolarizedLuminosity(mbd_scalers_no_cut, pattern_y, -1);
    double lumi_ratio_y = 0;
    if (lumi_nega_y != 0)
      lumi_ratio_y = 1. * lumi_pos_y / lumi_nega_y;

    auto lumi_y = lumi_pos_y + lumi_nega_y;

    luminosity_sum += lumi_b;

    hist_lumi_b->Fill( lumi_ratio_b );
    hist_weighted_lumi_b->Fill(lumi_ratio_b, lumi_b);
    hist_lumi_y->Fill( lumi_ratio_y );
    hist_weighted_lumi_y->Fill( lumi_ratio_y , lumi_y);

  } // end of loop over runs

  hist_weighted_lumi_b->Scale(1.0 / luminosity_sum);
  hist_weighted_lumi_y->Scale(1.0 / luminosity_sum);
  cout << hist_lumi_b->GetEntries() << " good runs" << endl;

  ////////////////////////////////////////////////////////////////////
  // Draw!
  // string output_pdf = output_base + ".pdf";
  TCanvas* c = new TCanvas("canvas", "title", 800, 800);
  c->SetFillStyle( 0 );
  gPad->SetFrameFillStyle( 0 );

  hist_weighted_lumi_y->GetXaxis()->SetNdivisions( 1005 );
  hist_weighted_lumi_y->GetXaxis()->SetRangeUser( 0.9, 1.1);
  hist_weighted_lumi_y->GetXaxis()->SetLabelOffset( 0.0125 );
  hist_weighted_lumi_y->GetXaxis()->SetLabelSize( 0.04 );
  hist_weighted_lumi_y->GetYaxis()->SetTitleOffset( 1.5 );
  hist_weighted_lumi_y->GetYaxis()->SetLabelSize( 0.04 );
  hist_weighted_lumi_y->GetYaxis()->SetRangeUser( 0.0, 0.2 );

  //for( int mode=0; mode<4; mode++ ) // loop over modes: preliminary, internal, work in progress
  {
    int mode = 3;
    // polarization with luminosity weight
    hist_weighted_lumi_y->Draw("HIST");
    hist_weighted_lumi_b->Draw("HIST same");

    string output_pdf = output_base;
    if( mode == 0 )
      output_pdf += "_internal.pdf";
    else if( mode == 1 )
      output_pdf += "_preliminary.pdf";
    else if( mode == 2 )
      output_pdf += "_wip.pdf";
    else if( mode == 3 )
      output_pdf += "_performance.pdf";

    int digits = 2;
    WriteDate( 0.775, 0.955, 0.04, "9/19/2025" );
    WritesPhenix( mode );
    double dy = 0.05, y = 0.875 + dy;

    y -= dy;
    WriteRunCondition( 0.2, y, false, 0.04, true );

    y += dy*3/4;
    TLegend* leg = new TLegend( 0.675, y - 0.1, 0.875, y );
    leg->SetTextSize( 0.04 );
    leg->SetBorderSize( 0 );
    leg->SetFillStyle( 0 );
    leg->AddEntry( hist_weighted_lumi_b, "Blue beam", "l" );
    leg->AddEntry( hist_weighted_lumi_y, "Yellow beam", "l" );
  
    leg->Draw();
    c->Print( output_pdf.c_str() );
  }

  tf->WriteTObject(hist_lumi_b, hist_lumi_b->GetName());
  tf->WriteTObject(hist_weighted_lumi_b, hist_weighted_lumi_b->GetName());
  tf->WriteTObject(hist_lumi_y, hist_lumi_y->GetName());
  tf->WriteTObject(hist_weighted_lumi_y, hist_weighted_lumi_y->GetName());
  tf->Close();
  // tr->Print();
  return 0;
}
