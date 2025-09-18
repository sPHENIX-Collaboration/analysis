//#include <uspin/SpinDBContentv1.h> // for ana.505 or later
#include <uspin/SpinDBContent.h>  // for ana.504 or earlier
#include <uspin/SpinDBOutput.h>

#include "functions.hh"
#include "sPhenixStyle.C"

const int kBunch_num_max = 120;

// It writes, for example,
//     <Pb> = 50%
// on the canvas
void WriteAveragePolarization( double value, string beam, double pos_x, double pos_y, int digits=2 )
{
  TLatex* tex = new TLatex();
  tex->SetTextSize( 0.04 );
  stringstream ss;
  ss << "#LTP_{" << beam << "}#GT = " << setprecision( digits ) << value << "%";

  tex->DrawLatexNDC( pos_x, pos_y, ss.str().c_str() );
  delete tex;
}

void WriteAdditionalMessage( double pos_x, double pos_y )
{
  TLatex* tex = new TLatex();
  tex->SetTextSize( 0.04 );
  stringstream ss;
  ss << "Preliminary RHIC Polarimetry Group Results";
  //ss << "Preliminary CNI Polarimetry Group Results";

  tex->DrawLatexNDC( pos_x, pos_y, ss.str().c_str() );
  delete tex;
}

long long GetPolarizedLuminosity(const long long scalers[kBunch_num_max], const int pattern[kBunch_num_max], const int polarity)
{
  long long sum = 0;
  for (int i = 0; i < kBunch_num_max; i++)
  {
    if (pattern[i] == polarity)
    {
      sum += scalers[i];
    }
  }

  return sum;
}

bool GetGoldenRuns(string list, vector<int>& runs)
{
  ifstream ifs(list.c_str());
  if (ifs.fail())
  {
    cerr << "Fail to open " << list << endl;
    return false;
  }

  int temp;
  while (ifs >> temp)
    runs.push_back(temp);

  return true;
}

int polarization()
{
  SetsPhenixStyle();
  string golden_run_list = "list/Full_ppGoldenRunList_Version2.list";
  vector<int> golden_runs;
  GetGoldenRuns(golden_run_list, golden_runs);
  cout << "- Golden run list: " << golden_run_list << endl;
  cout << "- " << golden_runs.size() << " golden runs taken" << endl;

  string output_base = "results/polarization";
  string output_root = output_base + ".root";

  TFile* tf = new TFile(output_root.c_str(), "RECREATE");

  /////////////////////////////////////////////////////////////////////////////////////
  // variables
  /////////////////////////////////////////////////////////////////////////////////////
  int run = 150045;  // 0;
  bool is_bad_run = true;

  ////////////////////////
  // for Blue ring
  double pol_b[kBunch_num_max] = {-9999.9};        // blue beam polarization for each bunch
  double pol_error_b[kBunch_num_max] = {-9999.9};  // Error of blue beam polarization for each bunch
  double pol_sys_error_b[kBunch_num_max] = {0};
  int pattern_b[kBunch_num_max] = {-9999};  // spin pattern of blue beam
  Long64_t lumi_b = 0, lumi_pos_b = 0, lumi_nega_b = 0;
  double lumi_ratio_b = 0;

  ////////////////////////
  // for Yellow ring
  double pol_y[kBunch_num_max] = {-9999.9};        // yellow beam polarization for each bunch
  double pol_error_y[kBunch_num_max] = {-9999.9};  // Error of yellow beam polarization for each bunch
  double pol_sys_error_y[kBunch_num_max] = {-9999};
  int pattern_y[kBunch_num_max] = {-9999};  // spin pattern of yellow beam
  Long64_t lumi_y = 0, lumi_pos_y = 0, lumi_nega_y = 0;
  double lumi_ratio_y = 0;

  ////////////////////////
  // for both beam
  Long64_t mbd_scalers_no_cut[kBunch_num_max] = {0};
  double mean_mbd_scalers_no_cut = 0, sigma_mbd_scalers_no_cut = 0;

  /////////////////////////////////////////////////////////////////////////////////////
  // Database setup
  /////////////////////////////////////////////////////////////////////////////////////
  int spin_db_status = 0;

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
  TH1D* hist_pol_b = new TH1D("hist_pol_b", "Blue beam polarization;Polarization [%];Counts", 100, 0, 100);
  HistSetting(hist_pol_b, GetSphenixColor());

  TH1D* hist_weighted_pol_b = new TH1D("hist_weighted_pol_b", "Blue beam polarization with luminosity weight;Polarization [%];Counts [a.u.]", 100, 0, 100);
  HistSetting(hist_weighted_pol_b, GetSphenixColor());

  TH1D* hist_pol_y = new TH1D("hist_pol_y", "Yellow beam polarization;Polarization [%];Counts", 100, 0, 100);
  HistSetting(hist_pol_y, kOrange + 1);

  TH1D* hist_weighted_pol_y = new TH1D("hist_weighted_pol_y", "Yellow beam polarization with luminosity weight;Polarization [%];Counts [a.u.]", 100, 0, 100);
  HistSetting(hist_weighted_pol_y, kOrange + 1);

  Long64_t luminosity_sum = 0;
  for (auto& run : golden_runs)
  {
    auto status = spin_db_output.GetDBContentStore(spin_db_content, run);
    if (status == 0)
    {
      continue;
    }

    is_bad_run = bool(spin_db_content.GetBadRunFlag());  // int 0 -> false, int 1 -> true

    if (is_bad_run == true)
      continue;

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
    lumi_pos_b = GetPolarizedLuminosity(mbd_scalers_no_cut, pattern_b, 1);
    lumi_nega_b = GetPolarizedLuminosity(mbd_scalers_no_cut, pattern_b, -1);
    if (lumi_nega_b != 0)
      lumi_ratio_b = 1. * lumi_pos_b / lumi_nega_b;
    else
      lumi_ratio_b = 0;

    lumi_b = lumi_pos_b + lumi_nega_b;

    // yellow beam
    lumi_pos_y = GetPolarizedLuminosity(mbd_scalers_no_cut, pattern_y, 1);
    lumi_nega_y = GetPolarizedLuminosity(mbd_scalers_no_cut, pattern_y, -1);
    if (lumi_nega_y != 0)
      lumi_ratio_y = 1. * lumi_pos_y / lumi_nega_y;
    else
      lumi_ratio_y = 0;

    lumi_y = lumi_pos_y + lumi_nega_y;

    luminosity_sum += lumi_b;
    hist_pol_b->Fill(pol_b[0]);
    hist_weighted_pol_b->Fill(pol_b[0], lumi_b);
    hist_pol_y->Fill(pol_y[0]);
    hist_weighted_pol_y->Fill(pol_y[0], lumi_y);

  }

  hist_weighted_pol_b->Scale(1.0 / luminosity_sum);
  hist_weighted_pol_y->Scale(1.0 / luminosity_sum);
  cout << hist_pol_b->GetEntries() << " good runs" << endl;

  ////////////////////////////////////////////////////////////////////
  // Draw!
  // string output_pdf = output_base + ".pdf";
  TCanvas* c = new TCanvas("canvas", "title", 800, 800);
  c->SetFillStyle( 0 );
  gPad->SetFrameFillStyle( 0 );

  hist_weighted_pol_b->GetXaxis()->SetRangeUser(35, 65);
  hist_weighted_pol_b->GetXaxis()->SetLabelOffset( 0.0125 );
  hist_weighted_pol_b->GetXaxis()->CenterTitle();
  hist_weighted_pol_b->GetYaxis()->SetTitleOffset( 1.5 );
  hist_weighted_pol_b->GetYaxis()->CenterTitle();
  hist_weighted_pol_b->GetYaxis()->SetRangeUser( 0, 0.25 );


  for( int mode=0; mode<3; mode++ ) // loop over modes: preliminary, internal, work in progress
  {
    // polarization with luminosity weight
    hist_weighted_pol_b->Draw("HIST");
    hist_weighted_pol_y->Draw("HIST same");

    string output_pdf = output_base;
    if( mode == 0 )
      output_pdf += "_internal.pdf";
    else if( mode == 1 )
      output_pdf += "_preliminary.pdf";
    else if( mode == 2 )
      output_pdf += "_wip.pdf";

    int digits = 2;
    WriteDate();
    WritesPhenix( mode );
    double dy = 0.05, y = 0.89 + dy;
        
    y -= dy;
    WriteRunCondition( 0.2, y); // p+p 2024 sqrt{s} = 200 GeV

    y -= dy;//*1.25;
    WriteAdditionalMessage( 0.2, y); // Preliminary RHIC Polarimetry Group Results

    y -= dy/2/2;
    TLegend* leg = new TLegend( 0.2, y-0.1, 0.4, y );
    leg->SetTextSize( 0.04 );
    leg->SetBorderSize( 0 );
    leg->SetFillStyle( 0 );
    leg->AddEntry( hist_weighted_pol_b, "Blue beam", "l" );
    leg->AddEntry( hist_weighted_pol_y, "Yellow beam", "l" );

    y -= dy*2.75;
    WriteAveragePolarization( hist_weighted_pol_b->GetMean(), "b", 0.2, y, digits );

    y -= dy;
    WriteAveragePolarization( hist_weighted_pol_y->GetMean(), "y", 0.2, y, digits );
  
    leg->Draw();
    c->Print( output_pdf.c_str() );
  }

  tf->WriteTObject(hist_pol_b, hist_pol_b->GetName());
  tf->WriteTObject(hist_weighted_pol_b, hist_weighted_pol_b->GetName());
  tf->WriteTObject(hist_pol_y, hist_pol_y->GetName());
  tf->WriteTObject(hist_weighted_pol_y, hist_weighted_pol_y->GetName());
  tf->Close();
  // tr->Print();
  return 0;
}
