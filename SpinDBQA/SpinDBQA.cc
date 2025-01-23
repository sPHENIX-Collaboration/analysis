#include "SpinDBQA.h"

#include <TFile.h>
#include <TH1D.h>
#include <TH1F.h>

#include <boost/format.hpp>

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iterator>


SpinDBQA::SpinDBQA()
    : spin_cont(),
      spin_out("phnxrc"),
      spin_in()
{
}


void SpinDBQA::SetRunList(std::string runlist)
{
  std::string line;
  std::ifstream irunlist;
  irunlist.open(runlist);
  if(!irunlist)
  {
    std::cout << "failed to open runlist " << std::endl;
    exit(1);
  }
  while (std::getline(irunlist, line)) {
    runlistvect.push_back(std::stoi(line));
  }
  irunlist.close(); 
}

void SpinDBQA::ReadSpinDBData()
{
  std::cout << "Reading data from spin database. This may take several minutes..." << std::endl;
  for (const auto &run : runlistvect)
  {
    runnumber = run;
    spin_out.StoreDBContent(runnumber,runnumber);
    spin_out.GetDBContentStore(spin_cont,runnumber);

    map_defaultQA[runnumber] = spin_out.GetDefaultQA(runnumber);

    map_fillnumber[runnumber] = spin_cont.GetFillNumber();
    map_qa_level[runnumber] = spin_cont.GetQALevel();
    map_crossingshift[runnumber] = spin_cont.GetCrossingShift();
    for (int i = 0; i < 120; i++)
    {
      map_bluespin[runnumber].push_back(spin_cont.GetSpinPatternBlue(i));
      map_yellspin[runnumber].push_back(spin_cont.GetSpinPatternYellow(i));
      map_mbdns[runnumber].push_back(spin_cont.GetScalerMbdNoCut(i));
      map_mbdvtx[runnumber].push_back(spin_cont.GetScalerMbdVertexCut(i));
      map_zdcns[runnumber].push_back(spin_cont.GetScalerZdcNoCut(i));
    }
    spin_cont.GetPolarizationBlue(0, map_bluepol[runnumber], map_bluepolerr[runnumber]);
    spin_cont.GetPolarizationYellow(0, map_yellpol[runnumber], map_yellpolerr[runnumber]);
    map_badrunqa[runnumber] = spin_cont.GetBadRunFlag();
    map_crossingangle[runnumber] = spin_cont.GetCrossAngle();
    map_crossanglestd[runnumber] = spin_cont.GetCrossAngleStd();
    map_crossanglemin[runnumber] = spin_cont.GetCrossAngleMin();
    map_crossanglemax[runnumber] = spin_cont.GetCrossAngleMax();
    spin_cont.GetAsymBlueForward(map_asymbf[runnumber], map_asymerrbf[runnumber]);
    spin_cont.GetAsymBlueBackward(map_asymbb[runnumber], map_asymerrbb[runnumber]);
    spin_cont.GetAsymYellowForward(map_asymyf[runnumber], map_asymerryf[runnumber]);
    spin_cont.GetAsymYellowBackward(map_asymyb[runnumber], map_asymerryb[runnumber]);
    spin_cont.GetPhaseBlueForward(map_phasebf[runnumber], map_phaseerrbf[runnumber]);
    spin_cont.GetPhaseBlueBackward(map_phasebb[runnumber], map_phaseerrbb[runnumber]);
    spin_cont.GetPhaseYellowForward(map_phaseyf[runnumber], map_phaseerryf[runnumber]);
    spin_cont.GetPhaseYellowBackward(map_phaseyb[runnumber], map_phaseerryb[runnumber]);
  }
  return;
}

void SpinDBQA::doQA()
{
  for (const auto &run : runlistvect)
  {
    runnumber = run;
    std::cout << "Processing QA on run: " << runnumber << std::endl;
    std::string stringMarkdown = "";
    CNIHjetQA(stringMarkdown);
    if (runnumber >= 45236)
    {
      GL1pScalersQA(stringMarkdown);
    }
    LocalPolQA(stringMarkdown);

    map_spindbqa_markdown[stringMarkdown].push_back(runnumber);
    //cout << "Bad run: " << badrunqa << endl;
  }
  return;
}

void SpinDBQA::CNIHjetQA(std::string &stringMarkdown)
{
  // ============== CNI H-jet results ============== //
  std::string filename = (boost::format("%s/fill_%d/DST_%d.root") % _cnipathname % map_fillnumber[runnumber] % map_fillnumber[runnumber]).str();
  std::ifstream file(filename.c_str());
  if (!file) 
  {
    std::cerr << "File does not exist: " << filename << std::endl;
    stringMarkdown +=  ("\u274C CNI measurement exists ");
  }
  else
  {
    TFile *cnipol = new TFile(filename.c_str());
    if (!cnipol || cnipol->IsZombie()) 
    {
      std::cerr << "Failed to open file: " << filename << std::endl;
      stringMarkdown +=  ("\u274C CNI measurement exists ");
    }
    else
    {
      stringMarkdown +=  ("\u2705 CNI measurement exists ");
      // ============== Get Hjet CNI beam polarizations and check them ============== //
      TH1D *h_results = (TH1D*)cnipol->Get("h_results");
      h_results->SetDirectory(0);
      // ============== Get CNI pol spin patterns ============== //
      TH1D *hcnipatternYellow = (TH1D*)cnipol->Get("h_polYellow");
      TH1D *hcnipatternBlue = (TH1D*)cnipol->Get("h_polBlue");
      hcnipatternYellow->SetDirectory(0);
      hcnipatternBlue->SetDirectory(0);

      cnipol->Close();
      delete cnipol;

      // ============== Check Hjet CNI beam polarizations ============== //
      float cnibeampol_yellow = h_results->GetBinContent(9);
      float cnibeampol_blue = h_results->GetBinContent(10);
      //float cnibeampolerr_yellow = h_results->GetBinError(9);
      //float cnibeampolerr_blue = h_results->GetBinError(10);
      // ============== set bad run conditions ============== //
      //badrunqa = (cnibeampol_yellow < 0 || cnibeampol_yellow > 1 || cnibeampol_blue < 0 || cnibeampol_blue > 0) ? 1 : badrunqa;
      stringMarkdown +=  (cnibeampol_yellow < 0 || cnibeampol_yellow > 1 || cnibeampol_blue < 0 || cnibeampol_blue > 0) ? 
                          "\u2705 CNI beam polarization " : "\u274C CNI beam polarization ";

      // ============== Check that spin pattern matches CNI pol spin patterns ============== //
      // ============== Find abort gap in CNI spin patterns to find CNI crossing shift //
      int abortgaplength = 8; 
      int currentCountYell = 0;
      int currentCountBlue = 0;
      int lastAgBunchIndexYell = -1;
      int lastAgBunchIndexBlue = -1;

      for (size_t i = 0; i < 120; ++i) {
          if (hcnipatternYellow->GetBinContent(i+1) == 0) {
              currentCountYell++; 
              if (currentCountYell > abortgaplength) {
                  abortgaplength = currentCountYell;
                  lastAgBunchIndexYell = i;
              }
          } else {
              currentCountYell = 0;
          }

          if (hcnipatternBlue->GetBinContent(i+1) == 0) {
              currentCountBlue++; 
              if (currentCountBlue > abortgaplength) {
                  abortgaplength = currentCountBlue;
                  lastAgBunchIndexBlue = i; 
              }
          } else {
              currentCountBlue = 0;
          }
      }

      int xingshift_cniYell = 119-lastAgBunchIndexYell;
      int xingshift_cniBlue = 119-lastAgBunchIndexBlue;

      for (int i = 0; i < 120; i++)
      {
        int cniyellowbunchspin = hcnipatternYellow->GetBinContent((i+120-xingshift_cniYell)%120+1);
        int cnibluebunchspin = hcnipatternBlue->GetBinContent((i+120-xingshift_cniBlue)%120+1);
        if (cniyellowbunchspin == 0)
        {
          cniyellowbunchspin = 10;
        }
        if (cnibluebunchspin == 0)
        {
          cnibluebunchspin = 10;
        }
        if (cniyellowbunchspin != map_yellspin[runnumber].at(i) || cnibluebunchspin != map_bluespin[runnumber].at(i))
        {
          //badrunqa = 1;
          stringMarkdown += "\u274C CNI spin pattern match ";
          break;
        }
        else if (i == 119)
        {
          stringMarkdown += "\u2705 CNI spin pattern match ";
        }
      }
    }
  }
  return;

}


void SpinDBQA::GL1pScalersQA(std::string &stringMarkdown)
{
  int sum_mbdns = 0; int sum_mbdvtx = 0; int sum_zdcns = 0;
  int min_sum_mbdns = std::numeric_limits<int>::max();
  int min_sum_mbdvtx = std::numeric_limits<int>::max();
  int min_sum_zdcns = std::numeric_limits<int>::max();
  int min_index_mbdns = -1; int min_index_mbdvtx = -1; int min_index_zdcns = -1;

  for (int i = 0; i < 120 + 9; ++i)
  {
    int ite = i % 120;
    sum_mbdns += map_mbdns[runnumber].at(ite);
    sum_mbdvtx += map_mbdvtx[runnumber].at(ite);
    sum_zdcns += map_zdcns[runnumber].at(ite);

    if (i >= 8) 
    {
      if (sum_mbdns < min_sum_mbdns) 
      {
        min_sum_mbdns = sum_mbdns;
        min_index_mbdns = (i - 8) % 120;
      }
      if (sum_mbdvtx < min_sum_mbdvtx) 
      {
        min_sum_mbdvtx = sum_mbdvtx;
        min_index_mbdvtx = (i - 8) % 120;
      }
      if (sum_zdcns < min_sum_zdcns) 
      {
        min_sum_zdcns = sum_zdcns;
        min_index_zdcns = (i - 8) % 120;
      }
      sum_mbdns -= map_mbdns[runnumber].at((i - 8) % 120);
      sum_mbdvtx -= map_mbdvtx[runnumber].at((i - 8) % 120);
      sum_zdcns -= map_zdcns[runnumber].at((i - 8) % 120);
    }
  }
  
  if (min_index_mbdns != min_index_mbdvtx || min_index_mbdns != min_index_zdcns)
  {
    stringMarkdown += "\u274C GL1p abort gaps ";
  }
  else
  {
    stringMarkdown += "\u2705 GL1p abort gaps ";
    int end_of_ag = (min_index_mbdns+9)%120;
    int xingshift_needed = (120-end_of_ag)%120;
    stringMarkdown += (map_crossingshift[runnumber] == xingshift_needed) ? "\u2705 Crossing shift " : "\u274C Crossing shift ";
  } 
  return;
}

void SpinDBQA::LocalPolQA(std::string &stringMarkdown)
{
  if (map_asymbf[runnumber] == 0 && map_asymbb[runnumber] == 0 && map_asymyf[runnumber] == 0 
        && map_asymyb[runnumber] == 0 && map_phasebf[runnumber] == 0 && map_phasebb[runnumber] == 0 
        && map_phaseyf[runnumber] == 0 && map_phaseyb[runnumber] == 0)
  {
    stringMarkdown += "\u274C Local polarimetry exists ";
  }
  else
  {
    stringMarkdown += "\u2705 Local polarimetry exists ";

    // Check magnitudes of forward asymmetries (with associated errors)
    // Check phase terms of forward asymmetries (with associated errors)
    //check ratio(?) of lrblue and udblue (with associated errors)
    //check ratio(?) of lryellow and uryellow (with associated errors)
    //should probably have markdown for local polarimetry blue beam and yellow beam separate
  }
    // if CNI does not have beam polarizations (see if you can get polarizations from local pol)
}


/*
//Write spin db info to new qa_level (and set this to default QA) //
void SpinDBQA::WriteSpinDBData()
{
  
    ////////////////////////////////////////////////////
    // Set new qa level
    int new_qa_level = 0;

    // If no row for the run,qa_level create it.
    if(spin_in.CheckRunRow(runnumber,new_qa_level)==0)
    {
      spin_in.CreateRunRow(runnumber,new_qa_level);
      printf("\n\nCreated Row for run %d with qa level %d\n\n",runnumber,new_qa_level);
    }

    spin_cont.SetQALevel(new_qa_level);
    ////////////////////////////////////////////////////

    ////////////////////////////////////////////////////
    // Set fill number
    spin_cont.SetFillNumber(fillnumber);
    ////////////////////////////////////////////////////

    ////////////////////////////////////////////////////
    // Set crossing shift
    spin_cont.SetCrossingShift(crossingshift);
    ////////////////////////////////////////////////////

    ////////////////////////////////////////////////////
    // Set spin patterns
    for (int i = 0; i < 120; i++)
    {
      spin_cont.SetSpinPatternBlue(i,bluespin[i]);
    }
    for (int i = 0; i < 120; i++)
    {
      spin_cont.SetSpinPatternYellow(i,yellspin[i]);
    }
    ////////////////////////////////////////////////////
    
    ////////////////////////////////////////////////////
    // Set beam polarizations
    spin_cont.SetPolarizationBlue(0, bluepol, bluepolerr);
    spin_cont.SetPolarizationYellow(0, yellpol, yellpolerr);
    ////////////////////////////////////////////////////

    ////////////////////////////////////////////////////
    // Set MBD NS GL1p scalers
    for (int i = 0; i < 120; i++)
    {
      spin_cont.SetScalerMbdNoCut(i,mbdns[i]);
    }
    ////////////////////////////////////////////////////

    ///////////////////////////////////////////////////
    // Set MBD VTX GL1p scalers
    for (int i = 0; i < 120; i++)
    {
      spin_cont.SetScalerMbdVertexCut(i,mbdvtx[i]);
    }
    ////////////////////////////////////////////////////

    ////////////////////////////////////////////////////
    // Set ZDC NS GL1p scalers
    for (int i = 0; i < 120; i++)
    {
      spin_cont.SetScalerZdcNoCut(i,zdcns[i]);
    }
    ////////////////////////////////////////////////////
    

    ////////////////////////////////////////////////////
    // Set badrun flag
    spin_cont.SetBadRunFlag(badrunqa);
    ////////////////////////////////////////////////////


    ////////////////////////////////////////////////////
    // Set crossing angle
    spin_cont.SetCrossAngle(crossingangle);
    spin_cont.SetCrossAngleStd(crossanglestd);
    spin_cont.SetCrossAngleMin(crossanglemin);
    spin_cont.SetCrossAngleMax(crossanglemax);
    ////////////////////////////////////////////////////

    ////////////////////////////////////////////////////
    // Set local polarimetry stuff
    spin_cont.SetAsymBlueForward(asymbf, asymerrbf);
    spin_cont.SetAsymBlueBackward(asymbb, asymerrbb);
    spin_cont.SetAsymYellowForward(asymyf, asymerryf);
    spin_cont.SetAsymYellowBackward(asymyb, asymerryb);
    spin_cont.SetPhaseBlueForward(phasebf, phaseerrbf);
    spin_cont.SetPhaseBlueBackward(phasebb, phaseerrbb);
    spin_cont.SetPhaseYellowForward(phaseyf, phaseerryf);
    spin_cont.SetPhaseYellowBackward(phaseyb, phaseerryb);
    ////////////////////////////////////////////////////


    // Update the database and set new default QA
    spin_in.UpdateDBContent(spin_cont);
    spin_in.SetDefaultQA(spin_cont);
    
    // ************************************ // 

}
*/

/*
void SpinDBQA::PrintSpinDBData(int runnumber)
{
    // Get default QA
    //int defaultQA = spin_out.GetDefaultQA(runnumber);
    ///////////////////////////////////////////////////

    //Get all default QA level spin information //

    std::cout << "Run number: " << runnumber << std::endl;
    int fillnumber = spin_cont.GetFillNumber();
    std::cout << "Fill number: " << fillnumber << std::endl;

    ////////////////////////////////////////////////////
    // Get qa level (this will also be default QA)
    int qa_level = spin_cont.GetQALevel();
    std::cout << "QA level: " << qa_level << std::endl;
    ////////////////////////////////////////////////////

    
    ////////////////////////////////////////////////////
    // Get crossing shift
    int crossingshift = spin_cont.GetCrossingShift();
    std::cout << "Crossing shift: " << crossingshift << std::endl;
    ////////////////////////////////////////////////////

    ////////////////////////////////////////////////////
    // Get spin patterns
    int bluespin[120] = {0};
    int yellspin[120] = {0};
    std::cout << "Blue spin pattern: [";
    for (int i = 0; i < 120; i++)
    {
      bluespin[i] = spin_cont.GetSpinPatternBlue(i);
      std::cout << bluespin[i];
      if (i < 119)std::cout << ", ";
    }
    std::cout << "]" << std::endl;

    std::cout << "Yellow spin pattern: [";
    for (int i = 0; i < 120; i++)
    {
      yellspin[i] = spin_cont.GetSpinPatternYellow(i);
      std::cout << yellspin[i];
      if (i < 119)std::cout << ", ";
    }
    std::cout << "]" << std::endl;
    ////////////////////////////////////////////////////

    
    ////////////////////////////////////////////////////
    // Get beam polarizations
    double bluepol;
    double yellpol;
    double bluepolerr;
    double yellpolerr;
    spin_cont.GetPolarizationBlue(0, bluepol, bluepolerr);
    spin_cont.GetPolarizationYellow(0, yellpol, yellpolerr);
    std::cout << "Blue polarization: " << bluepol << " +/- "  << bluepolerr << std::endl;
    std::cout << "Yellow polarization: " << yellpol << " +/- "  << yellpolerr << std::endl;
    ////////////////////////////////////////////////////

    ////////////////////////////////////////////////////
    // Get MBD NS GL1p scalers
    long long mbdns[120] = {0};
    std::cout << "MBDNS GL1p scalers: [";
    for (int i = 0; i < 120; i++)
    {
      mbdns[i] = spin_cont.GetScalerMbdNoCut(i);
      std::cout << mbdns[i];
      if (i < 119)std::cout << ", ";
    }
    std::cout << "]" << std::endl;
    ////////////////////////////////////////////////////

    ////////////////////////////////////////////////////
    // Get MBD VTX GL1p scalers
    long long mbdvtx[120] = {0};
    std::cout << "MBD VTX GL1p scalers: [";
    for (int i = 0; i < 120; i++)
    {
      mbdvtx[i] = spin_cont.GetScalerMbdVertexCut(i);
      std::cout << mbdvtx[i];
      if (i < 119)std::cout << ", ";
    }
    std::cout << "]" << std::endl;
    ////////////////////////////////////////////////////


    ////////////////////////////////////////////////////
    // Get ZDC NS GL1p scalers
    long long zdcns[120] = {0};
    std::cout << "ZDC NS GL1p scalers: [";
    for (int i = 0; i < 120; i++)
    {
      zdcns[i] = spin_cont.GetScalerZdcNoCut(i);
      std::cout << zdcns[i];
      if (i < 119)std::cout << ", ";
    }
    std::cout << "]" << std::endl;
    ////////////////////////////////////////////////////

    
    ////////////////////////////////////////////////////
    // Get badrun flag
    int badrunqa = spin_cont.GetBadRunFlag();
    std::cout << "Bad run flag: " << badrunqa << std::endl;
    ////////////////////////////////////////////////////


    ////////////////////////////////////////////////////
    // Get crossing angle
    float crossingangle = spin_cont.GetCrossAngle();
    float crossanglestd= spin_cont.GetCrossAngleStd();
    float crossanglemin= spin_cont.GetCrossAngleMin();
    float crossanglemax= spin_cont.GetCrossAngleMax();
    std::cout << "Crossing angle: " << crossingangle << " +/- " << crossanglestd << std::endl;
    ////////////////////////////////////////////////////

    ////////////////////////////////////////////////////
    // Get local polarimetry stuff
    float asymbf, asymbb, asymyf, asymyb;
    float asymerrbf, asymerrbb, asymerryf, asymerryb;
    float phasebf, phasebb, phaseyf, phaseyb;
    float phaseerrbf, phaseerrbb, phaseerryf, phaseerryb;

    spin_cont.GetAsymBlueForward(asymbf, asymerrbf);
    spin_cont.GetAsymBlueBackward(asymbb, asymerrbb);
    spin_cont.GetAsymYellowForward(asymyf, asymerryf);
    spin_cont.GetAsymYellowBackward(asymyb, asymerryb);
    spin_cont.GetPhaseBlueForward(phasebf, phaseerrbf);
    spin_cont.GetPhaseBlueBackward(phasebb, phaseerrbb);
    spin_cont.GetPhaseYellowForward(phaseyf, phaseerryf);
    spin_cont.GetPhaseYellowBackward(phaseyb, phaseerryb);
    std::cout << "Blue forward asymmetry: " << asymbf << " +/- " << asymerrbf << std::endl;
    std::cout << "Blue backward asymmetry: " << asymbb << " +/- " << asymerrbb << std::endl;
    std::cout << "Yellow forward asymmetry: " << asymyf << " +/- " << asymerryf << std::endl;
    std::cout << "Yellow backward asymmetry: " << asymyb << " +/- " << asymerryb << std::endl;
    std::cout << "Blue forward phase: " << phasebf << " +/- " << phaseerrbf << std::endl;
    std::cout << "Blue backward phase: " << phasebb << " +/- " << phaseerrbb << std::endl;
    std::cout << "Yellow forward phase: " << phaseyf << " +/- " << phaseerryf << std::endl;
    std::cout << "Yellow backward phase: " << phaseyb << " +/- " << phaseerryb << std::endl;
    ////////////////////////////////////////////////////
}
*/

void SpinDBQA::WriteMarkdown()
{
  //============== Write markdown of spin db QA ============== //
  std::ofstream f_md(_markdownfilename);
  if (!f_md.is_open()) 
  {
    std::cerr << "Error: Could not open the file for writing!" << std::endl;
  }
  else
  {
    for (auto& [statements, runs] : map_spindbqa_markdown)
    {
      std::sort(runs.begin(), runs.end());
      f_md << "**" << statements << "**\n\n> ";
      for (int run : runs) {
        f_md << run << " ";
      }
      f_md << "\n\n";
    } 
  }
  f_md.close();
  return;
}