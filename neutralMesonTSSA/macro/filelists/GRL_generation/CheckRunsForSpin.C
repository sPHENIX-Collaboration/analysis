#ifndef READSPINDB_C
#define READSPINDB_C

#include <string>
#include <iostream>
#include <fstream>

#include <uspin/SpinDBContent.h>
#include <uspin/SpinDBOutput.h>


R__LOAD_LIBRARY(libuspin.so)

int ReadSpinDB(int runnumber, TH1D* h_spinQA, unsigned int qa_level = 0xffff);

void CheckRunsForSpin(std::string infilename)
{
    std::ifstream infile;
    infile.open(infilename.c_str());

    /* std::string outfilename = "validspin_" + infilename; */
    std::string outfilename = "validspinruns.txt";
    std::ofstream outfile;
    std::cout << "outfilename = " << outfilename << std::endl;
    outfile.open(outfilename.c_str());
    if (outfile.is_open()) std::cout << "outfile is open!";
    else return -1;
    /* outfile << "Test line"; */

    std::string rootfilename = "spinQAlog.root";
    TFile* rootfile = new TFile(rootfilename.c_str(), "RECREATE");
    rootfile->cd();
    TH1D* h_spinQA = new TH1D("h_spinQA", "Reason for Run Failing Spin QA;Reason;# Runs", 4, -0.5, 3.5);
    h_spinQA->GetXaxis()->SetBinLabel(1, "All Runs");
    h_spinQA->GetXaxis()->SetBinLabel(2, "Good Runs");
    h_spinQA->GetXaxis()->SetBinLabel(3, "BadRunFlag");
    h_spinQA->GetXaxis()->SetBinLabel(4, "GL1P Empty");
    
    
    int run;
    while (infile >> run) {
	bool has_spin = ReadSpinDB(run, h_spinQA);
	/* std::cout << "Greg info: run number = " << run << ", has_spin = " << has_spin << std::endl; */
	if (has_spin) outfile << run << std::endl;
    }

    rootfile->Write();
    TCanvas* c1 = new TCanvas("c1", "c1", 1600, 900);
    h_spinQA->Draw();
    c1->SaveAs("spinQAlog.pdf");
    rootfile->Close();
    delete c1;

    infile.close();
    outfile.close();
}

//////////////////////////////////////////////////////
/////  0xffff is the qa_level from XingShiftCal //////
//////////////////////////////////////////////////////
int ReadSpinDB(int runnumber, TH1D* h_spinQA, unsigned int qa_level)
{
  h_spinQA->Fill(0);
  
  SpinDBContent spin_cont;
  SpinDBOutput spin_out("phnxrc");

  spin_out.StoreDBContent(runnumber,runnumber,qa_level);
  spin_out.GetDBContentStore(spin_cont,runnumber);

  /* std::cout << "Run Number: " << runnumber << std::endl; */

  // Get Bad Run QA (bad run != 0)
  int badrun = spin_cont.GetBadRunFlag();
  /* std::cout << "badrun = " << badrun << std::endl; */

  // Get spin patterns
  int bluespin[120] = {0};
  int yellspin[120] = {0};
  /* std::cout << "Blue spin pattern: ["; */
  for (int i = 0; i < 120; i++)
  {
    bluespin[i] = spin_cont.GetSpinPatternBlue(i);
    /* std::cout << bluespin[i]; */
    /* if (i < 119)std::cout << ", "; */
  }
  /* std::cout << "]" << std::endl; */

  /* std::cout << "Yellow spin pattern: ["; */
  for (int i = 0; i < 120; i++)
  {
    yellspin[i] = spin_cont.GetSpinPatternYellow(i);
    /* std::cout << yellspin[i]; */
    /* if (i < 119)std::cout << ", "; */
  }
  /* std::cout << "]" << std::endl; */
  ////////////////////////////////////////////////////
  

  // Get GL1p scalers
  long int gl1pScalers[120] = {0};
  long int gl1pSum = 0;
  /* std::cout << "MBDNS GL1p scalers: ["; */
  for (int i = 0; i < 120; i++)
  {
      gl1pScalers[i] = spin_cont.GetScalerMbdNoCut(i);
      gl1pSum += gl1pScalers[i];
      /* std::cout << gl1pScalers[i]; */
      /* if (i < 119) std::cout << ", "; */
  }
  /* std::cout << "]" << std::endl; */

  /* if (bluespin[0] == -999) return false; */
  if (badrun) {
    h_spinQA->Fill(2);
    return false;
  }
  if (gl1pSum == 0) {
    h_spinQA->Fill(3);
    return false;
  }
  h_spinQA->Fill(1);
  return true;

}

#endif
