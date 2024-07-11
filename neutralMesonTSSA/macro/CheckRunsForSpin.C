#ifndef READSPINDB_C
#define READSPINDB_C

#include <string>
#include <iostream>
#include <fstream>

#include <uspin/SpinDBContent.h>
#include <uspin/SpinDBOutput.h>


R__LOAD_LIBRARY(libuspin.so)

int ReadSpinDB(int runnumber = 42158, unsigned int qa_level = 0xffff);

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
    
    int run;
    while (infile >> run) {
	bool has_spin = ReadSpinDB(run);
	/* std::cout << "Greg info: run number = " << run << ", has_spin = " << has_spin << std::endl; */
	if (has_spin) outfile << run << std::endl;
    }

    infile.close();
    outfile.close();
}

//////////////////////////////////////////////////////
/////  0xffff is the qa_level from XingShiftCal //////
//////////////////////////////////////////////////////
int ReadSpinDB(int runnumber, unsigned int qa_level)
{
  
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

  if (bluespin[0] == -999) return false;
  else return true;

}

#endif
