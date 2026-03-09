#include <TFile.h>
#include <TH1I.h>
#include <TGraphErrors.h>
#include <TCanvas.h>
#include <TMath.h>
#include <iostream>
#include <string>

#include <uspin/SpinDBContent.h>
#include <uspin/SpinDBOutput.h>

R__LOAD_LIBRARY(libuspin.so)
static const int NBUNCHES = 120;
float zdc_adc_cut_[2][3] = {0.}; // ADC cut for each ZDC module ::array[side][module]
float smd_adc_cut_[2][2] = {0.}; // ADC cut for each smd  ::array[side][direction]
int spinPatternBlue[NBUNCHES] = {0};
int spinPatternYellow[NBUNCHES] = {0};
int crossingShift = 1;

void GetSpinPatterns(int runNum)
{
  // Get the spin patterns from the spin DB

  //  0xffff is the qa_level from XingShiftCal //////
  unsigned int qa_level = 0xffff;
  SpinDBContent spin_cont;
  SpinDBOutput spin_out("phnxrc");

  spin_out.StoreDBContent(runNum, runNum, qa_level);
  spin_out.GetDBContentStore(spin_cont, runNum);

  // Get crossing shift
  crossingShift = spin_cont.GetCrossingShift();
  std::cout << "Crossing shift: " << crossingShift << std::endl;

  std::cout << "Blue spin pattern: [";
  for (int i = 0; i < 120; i++)
  {
    spinPatternBlue[i] = spin_cont.GetSpinPatternBlue(i);
    std::cout << spinPatternBlue[i];
    if (i < 119)
      std::cout << ", ";
  }
  std::cout << "]" << std::endl;

  std::cout << "Yellow spin pattern: [";
  for (int i = 0; i < 120; i++)
  {
    spinPatternYellow[i] = spin_cont.GetSpinPatternYellow(i);
    std::cout << spinPatternYellow[i];
    if (i < 119)
      std::cout << ", ";
  }
  std::cout << "]" << std::endl;

  if (false)
  {
    // for testing -- if we couldn't find the spin pattern, fill it with a dummy pattern
    // +-+-+-+- ...
    std::cout << "Could not find spin pattern packet for blue beam! Using dummy pattern" << std::endl;
    for (int i = 0; i < NBUNCHES; i++)
    {
      int mod = i % 2;
      if (mod == 0)
        spinPatternBlue[i] = 1;
      else
        spinPatternBlue[i] = -1;
    }
    // for testing -- if we couldn't find the spin pattern, fill it with a dummy pattern
    // ++--++-- ,,,
    std::cout << "Could not find spin pattern packet for yellow beam! Using dummy pattern" << std::endl;
    for (int i = 0; i < NBUNCHES; i++)
    {
      int mod = i % 4;
      if (mod == 0 || mod == 1)
        spinPatternYellow[i] = 1;
      else
        spinPatternYellow[i] = -1;
    }
  }
}
void DrawBunchAysm_UD(int runnumber = 42797)
{
//    std::string filename = "result_count_000" + std::to_string(runnumber) + "-"+std::to_string(cutvalue)+".root";
    std::string filename = "result_count_00042797-5-30M-merged.root";
    // Open the ROOT file
    TFile* file = TFile::Open(filename.c_str());
    if (!file || file->IsZombie()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }
    GetSpinPatterns(runnumber);
    crossingShift = 0;
    // Get the histograms
    TH1I *n_smd_v12 = dynamic_cast<TH1I *>(file->Get("h1_NSMD_h_12"));
    TH1I *n_smd_v78 = dynamic_cast<TH1I *>(file->Get("h1_NSMD_h_78"));
    TH1I *s_smd_v12 = dynamic_cast<TH1I *>(file->Get("h1_SSMD_h_12"));
    TH1I *s_smd_v78 = dynamic_cast<TH1I *>(file->Get("h1_SSMD_h_78"));
    if (!n_smd_v12 || !n_smd_v78 || !s_smd_v12 || !s_smd_v78)
    {
        std::cerr << "Error: could not retrieve histograms" << std::endl;
        file->Close();
        return;
    }

    // Number of bins
    int nBins = n_smd_v12->GetNbinsX();
    if (nBins != n_smd_v78->GetNbinsX())
    {
        std::cerr << "Error: histograms have different number of bins" << std::endl;
        file->Close();
        return;
    }

    // Prepare arrays for TGraphErrors
    std::vector<double> x_spinup_blue, y_spinup_blue, ex_spinup_blue, ey_spinup_blue;
    std::vector<double> x_spindown_blue, y_spindown_blue, ex_spindown_blue, ey_spindown_blue;
    std::vector<double> x_spinup_yellow, y_spinup_yellow, ex_spinup_yellow, ey_spinup_yellow;
    std::vector<double> x_spindown_yellow, y_spindown_yellow, ex_spindown_yellow, ey_spindown_yellow;

    // Calculate the ratio and errors
    for (int i = 1; i <= nBins; ++i)
    {
      double A = n_smd_v12->GetBinContent(i);
      double B = n_smd_v78->GetBinContent(i);
      double A_y = s_smd_v12->GetBinContent(i);
      double B_y = s_smd_v78->GetBinContent(i);
      std::cout << "A : " << A << " //// B : " << B << std::endl;
      int nbunch = i - 1;
      nbunch = (nbunch + crossingShift) % 120;
      int b_spin = spinPatternBlue[nbunch];
      int y_spin = spinPatternYellow[nbunch];
      if (b_spin!=1 && b_spin!=-1)
        continue;
      if (A != 0 || B != 0)
      {
        double aym = (B - A) / (A + B);
        double error = TMath::Sqrt(4 * (A * B) / ((A + B) * (A + B) * (A + B)));
        if (b_spin == 1)
        { // spinup_blue bin
          x_spinup_blue.push_back(nbunch);
          y_spinup_blue.push_back(aym);
          ex_spinup_blue.push_back(0); // No error on x
          ey_spinup_blue.push_back(error);
        }
        else if (b_spin == -1)
        { // spindown_blue bin
          x_spindown_blue.push_back(nbunch);
          y_spindown_blue.push_back(aym);
          ex_spindown_blue.push_back(0); // No error on x
          ey_spindown_blue.push_back(error);
        }
      }
      if (A_y != 0 || B_y != 0)
      {
        double aym = (B_y - A_y) / (A_y + B_y);
        double error = TMath::Sqrt(4 * (A_y * B_y) / ((A_y + B_y) * (A_y + B_y) * (A_y + B_y)));
        if (y_spin == 1)
        { // spinup_blue bin
          x_spinup_yellow.push_back(nbunch);
          y_spinup_yellow.push_back(aym);
          ex_spinup_yellow.push_back(0); // No error on x
          ey_spinup_yellow.push_back(error);
        }
        else if (y_spin == -1)
        { // spindown_yellow bin
          x_spindown_yellow.push_back(nbunch);
          y_spindown_yellow.push_back(aym);
          ex_spindown_yellow.push_back(0); // No error on x
          ey_spindown_yellow.push_back(error);
        }
      }
    }

    // Create the TGraphErrors for spinup_blue bins
    TGraphErrors *graph_spinup_blue = new TGraphErrors(x_spinup_blue.size(), x_spinup_blue.data(), y_spinup_blue.data(), ex_spinup_blue.data(), ey_spinup_blue.data());
    graph_spinup_blue->SetMarkerStyle(21);
    graph_spinup_blue->SetMarkerColor(kRed);

    // Create the TGraphErrors for spindown_blue bins
    TGraphErrors *graph_spindown_blue = new TGraphErrors(x_spindown_blue.size(), x_spindown_blue.data(), y_spindown_blue.data(), ex_spindown_blue.data(), ey_spindown_blue.data());
    graph_spindown_blue->SetMarkerStyle(21);
    graph_spindown_blue->SetMarkerColor(kBlue);

    // Create the TGraphErrors for spinup_yellow bins
    TGraphErrors *graph_spinup_yellow = new TGraphErrors(x_spinup_yellow.size(), x_spinup_yellow.data(), y_spinup_yellow.data(), ex_spinup_yellow.data(), ey_spinup_yellow.data());
    graph_spinup_yellow->SetMarkerStyle(21);
    graph_spinup_yellow->SetMarkerColor(kRed);

    // Create the TGraphErrors for spindown_yellow bins
    TGraphErrors *graph_spindown_yellow = new TGraphErrors(x_spindown_yellow.size(), x_spindown_yellow.data(), y_spindown_yellow.data(), ex_spindown_yellow.data(), ey_spindown_yellow.data());
    graph_spindown_yellow->SetMarkerStyle(21);
    graph_spindown_yellow->SetMarkerColor(kBlue);




    // Draw the graphs
    TCanvas *canvas = new TCanvas("canvas_blue", "North SMD", 800, 600);
    graph_spinup_blue->SetTitle(Form("NSMD North Crossing Shift : %d;Bunch number;(U-D)/(U+D)",crossingShift));
    double x_min = 0;
    double x_max = 128;
    double y_min = -0.05;
    double y_max = 0.05; // Adjust as necessary

    graph_spinup_blue->GetXaxis()->SetLimits(x_min, 110);
    graph_spinup_blue->SetMinimum(y_min);
    graph_spinup_blue->SetMaximum(y_max);
    graph_spinup_blue->Draw("AP");
    graph_spindown_blue->Draw("P SAME");

    // Add legend
    TLegend *legend = new TLegend(0.6, 0.8, 0.9, 0.9);
    legend->AddEntry(graph_spinup_blue, "blue spin up Bunches", "p");
    legend->AddEntry(graph_spindown_blue, "blue spin down Bunches", "p");
    legend->Draw();

TCanvas *canvas2 = new TCanvas("canvas_yellow", "South SMD", 800, 600);
    graph_spinup_yellow->SetTitle(Form("SSMD South Crossing Shift : %d;Bunch number;(U-D)/(U+D)",crossingShift));
    double x_min2 = 0;
    double x_max2 = 128;
    double y_min2 = -0.05;
    double y_max2 = 0.05; // Adjust as necessary

    graph_spinup_yellow->GetXaxis()->SetLimits(x_min2, 110);
    graph_spinup_yellow->SetMinimum(y_min2);
    graph_spinup_yellow->SetMaximum(y_max2);
    graph_spinup_yellow->Draw("AP");
    graph_spindown_yellow->Draw("P SAME");

    // Add legend
    TLegend *legend2 = new TLegend(0.6, 0.8, 0.9, 0.9);
    legend2->AddEntry(graph_spinup_yellow, "yellow spin up Bunches", "p");
    legend2->AddEntry(graph_spindown_yellow, "yellow spin down Bunches", "p");
    legend2->Draw();


    std::size_t pos = filename.find(".root");
    
    if (pos == std::string::npos) {
        return filename;
    }
    std::string result = filename;
    result.insert(pos, "-plot-UD-crossing"+std::to_string(crossingShift));
    canvas->SaveAs(Form("run_%d_crossing_%d_blue.png",runnumber,crossingShift));
    canvas2->SaveAs(Form("run_%d_crossing_%d_yellow.png",runnumber,crossingShift));
    TFile* sf = new TFile(result.c_str(),"RECREATE");
    sf->cd();
    canvas->Write();
    canvas2->Write();
    sf->Close();


    // Cleanup
    file->Close();
    delete file;
    delete canvas;
    delete graph_spinup_yellow;
    delete graph_spindown_yellow;
}
