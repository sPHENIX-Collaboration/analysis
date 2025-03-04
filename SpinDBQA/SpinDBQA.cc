#include "SpinDBQA.h"

#include <TFile.h>
#include <TH1D.h>
#include <TH1F.h>
#include <TGraphErrors.h>

#include <boost/format.hpp>

#include <regex>
#include <filesystem>
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
  locpolbf = new TGraphErrors();
  locpolbf->SetName("locpolbf");
  locpolbb = new TGraphErrors();
  locpolbb->SetName("locpolbb");
  locpolyf = new TGraphErrors();
  locpolyf->SetName("locpolyf");
  locpolyb = new TGraphErrors();
  locpolyb->SetName("locpolyb");

  locpolphasebf = new TGraphErrors();
  locpolphasebf->SetName("locpolphasebf");
  locpolphasebb = new TGraphErrors();
  locpolphasebb->SetName("locpolphasebb");
  locpolphaseyf = new TGraphErrors();
  locpolphaseyf->SetName("locpolphaseyf");
  locpolphaseyb = new TGraphErrors();
  locpolphaseyb->SetName("locpolphaseyb");

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
  std::cout << "Reading data from spin database." << std::endl;
  std::cout << "There are " << runlistvect.size() << " runs. This may take several minutes..." << std::endl;
  int ite = 0;

  if (b_defaultQA)
  {
    spin_out.StoreDBContent(runlistvect.front(),runlistvect.back());
  }
  else
  {
    spin_out.StoreDBContent(runlistvect.front(),runlistvect.back(), qalevel);
  }

  for (const auto &run : runlistvect)
  {
    runnumber = run;
    int runexists = spin_out.GetDBContentStore(spin_cont,runnumber);
    if (runexists == 0)
    {
      map_fillnumber[runnumber] = -999;
      continue;
    }
    //map_defaultQA[runnumber] = spin_out.GetDefaultQA(runnumber);
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
    
    ++ite;
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
    std::string stringHtml = "";
    if (map_fillnumber[runnumber] == -999)
    {
      stringHtml += "Run does not exist at this qa_level";
    }
    else
    {
      CNIHjetQA(stringMarkdown,stringHtml);
      if (runnumber >= 45236){GL1pScalersQA(stringMarkdown,stringHtml);}
      else{stringHtml += "GL1p Scalers: <font color=\"#800080\">NOT AVAILABLE</font>; ";}
      LocalPolQA(stringMarkdown,stringHtml);
      CrossingAngleQA(stringMarkdown,stringHtml);
      map_spindbqa_markdown[stringMarkdown].push_back(runnumber);
    }
    
    map_spindbqa_html[stringHtml].push_back(runnumber);
  }
  return;
}

void SpinDBQA::CNIHjetQA(std::string &stringMarkdown, std::string &stringHtml)
{
  // ============== CNI H-jet results ============== //
  std::string filename = (boost::format("%s/fill_%d/DST_%d.root") % _cnipathname % map_fillnumber[runnumber] % map_fillnumber[runnumber]).str();
  std::ifstream file(filename.c_str());
  if (!file) // for qa_level = 0xFFFF cases
  {
    std::cerr << "File does not exist: " << filename << std::endl;
    stringMarkdown +=  ("\u274C CNI measurement exists ");
    stringHtml +=  ("Beam polarization: <font color=\"#800080\">NOT AVAILABLE</font>; Spin patterns: <font color=\"#800080\">NOT AVAILABLE</font>; ");
  }
  else if (map_bluepol[runnumber] == -999 || map_yellpol[runnumber] == -999) // for qa_level = 0 cases
  {
    stringMarkdown +=  ("\u274C CNI measurement exists ");
    stringHtml +=  ("Beam polarization: <font color=\"#800080\">NOT AVAILABLE</font>; Spin patterns: <font color=\"#800080\">NOT AVAILABLE</font>; ");
  }
  else
  {
    TFile *cnipol = new TFile(filename.c_str());
    if (!cnipol || cnipol->IsZombie()) 
    {
      std::cout << "Failed to open file: " << filename << std::endl;
      stringMarkdown +=  ("\u274C CNI measurement exists ");
      stringHtml +=  ("Beam polarization: <font color=\"#800080\">NOT AVAILABLE</font>; Spin patterns: <font color=\"#800080\">NOT AVAILABLE</font>; ");
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
      stringHtml +=  (cnibeampol_yellow < 0 || cnibeampol_yellow > 1 || cnibeampol_blue < 0 || cnibeampol_blue > 0) ?
                          "Beam polarization: <font color=\"#11cf17\">GOOD</font>; " : " Beam polarization: <font color=\"#ff0000\">BAD</font>; ";
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
          stringHtml += "Spin patterns: <font color=\"#ff0000\">BAD</font>; ";
          break;
        }
        else if (i == 119)
        {
          stringMarkdown += "\u2705 CNI spin pattern match ";
          stringHtml += "Spin patterns: <font color=\"#11cf17\">GOOD</font>; ";
        }
      }
    }
  }
  return;

}


void SpinDBQA::GL1pScalersQA(std::string &stringMarkdown, std::string &stringHtml)
{

  if (map_crossingshift[runnumber] == -999)
  {
    stringMarkdown += "\u274C GL1p abort gaps ";
    stringHtml += "GL1p Scalers: <font color=\"#ff0000\">BAD</font>; ";
    return;
  }

  // these are 28x28, 6x6 fills where the crossingshift has been correctly determined at 0xFFFF level
  if (map_fillnumber[runnumber] ==  34897 || map_fillnumber[runnumber] ==  34886 || map_fillnumber[runnumber] ==  35020)
  {
    stringMarkdown += "\u2705 Crossing shift ";
    stringHtml += "GL1p Scalers: <font color=\"#11cf17\">GOOD</font>; ";
    return;
  }

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
    stringHtml += "GL1p Scalers: <font color=\"#ff0000\">BAD</font>; ";
  }
  else
  {
    stringMarkdown += "\u2705 GL1p abort gaps ";
    int end_of_ag = (min_index_mbdns+9)%120;
    int xingshift_needed = (120-end_of_ag)%120;
    stringMarkdown += (map_crossingshift[runnumber] == xingshift_needed) ? "\u2705 Crossing shift " : "\u274C Crossing shift ";
    stringHtml += (map_crossingshift[runnumber] == xingshift_needed) ? "GL1p Scalers: <font color=\"#11cf17\">GOOD</font>; " : "GL1p Scalers: <font color=\"#ff0000\">BAD</font>;  ";
  } 
  return;
}

void SpinDBQA::LocalPolQA(std::string &stringMarkdown, std::string &stringHtml)
{
  if (map_asymbf[runnumber] == 0 && map_asymbb[runnumber] == 0 && map_asymyf[runnumber] == 0 
        && map_asymyb[runnumber] == 0 && map_phasebf[runnumber] == 0 && map_phasebb[runnumber] == 0 
        && map_phaseyf[runnumber] == 0 && map_phaseyb[runnumber] == 0)
  {
    stringMarkdown += "\u274C Local polarimetry exists ";
    stringHtml += "Local Polarimetry: <font color=\"#800080\">NOT AVAILABLE</font>; ";
  }
  else
  {
    stringMarkdown += "\u2705 Local polarimetry exists ";

    locpolbf->SetPoint(locpolbf->GetN(),runnumber,map_asymbf[runnumber]);
    locpolbf->SetPointError(locpolbf->GetN()-1,0,map_asymerrbf[runnumber]);
    locpolbb->SetPoint(locpolbb->GetN(),runnumber,map_asymbb[runnumber]);
    locpolbb->SetPointError(locpolbb->GetN()-1,0,map_asymerrbb[runnumber]);
    locpolyf->SetPoint(locpolyf->GetN(),runnumber,map_asymyf[runnumber]);
    locpolyf->SetPointError(locpolyf->GetN()-1,0,map_asymerryf[runnumber]);
    locpolyb->SetPoint(locpolyb->GetN(),runnumber,map_asymyb[runnumber]);
    locpolyb->SetPointError(locpolyb->GetN()-1,0,map_asymerryb[runnumber]);

    locpolphasebf->SetPoint(locpolphasebf->GetN(),runnumber,map_phasebf[runnumber]);
    locpolphasebf->SetPointError(locpolphasebf->GetN()-1,0,map_phaseerrbf[runnumber]);
    locpolphasebb->SetPoint(locpolphasebb->GetN(),runnumber,map_phasebb[runnumber]);
    locpolphasebb->SetPointError(locpolphasebb->GetN()-1,0,map_phaseerrbb[runnumber]);
    locpolphaseyf->SetPoint(locpolphaseyf->GetN(),runnumber,map_phaseyf[runnumber]);
    locpolphaseyf->SetPointError(locpolphaseyf->GetN()-1,0,map_phaseerryf[runnumber]);
    locpolphaseyb->SetPoint(locpolphaseyb->GetN(),runnumber,map_phaseyb[runnumber]);
    locpolphaseyb->SetPointError(locpolphaseyb->GetN()-1,0,map_phaseerryb[runnumber]);

    // Does asymmetry magnitude fall out of mean +- RMSE from Run24?
    float bf_distancefromband = map_asymbf[runnumber] > bf_mean ? map_asymbf[runnumber]-(bf_mean+bf_rmse) : (bf_mean-bf_rmse)-map_asymbf[runnumber];
    float yf_distancefromband = map_asymyf[runnumber] > yf_mean ? map_asymyf[runnumber]-(yf_mean+yf_rmse) : (yf_mean-yf_rmse)-map_asymyf[runnumber];

    // Does asymmetry phase fall out of mean +- RMSE from Run24?
    float bf_phase_distancefromband = map_phasebf[runnumber] > bf_phase_mean ? map_phasebf[runnumber]-(bf_phase_mean+bf_phase_rmse) : (bf_phase_mean-bf_phase_rmse)-map_phasebf[runnumber];
    float yf_phase_distancefromband = map_phaseyf[runnumber] > yf_phase_mean ? map_phaseyf[runnumber]-(yf_phase_mean+yf_phase_rmse) : (yf_phase_mean-yf_phase_rmse)-map_phaseyf[runnumber];

    
    if (bf_distancefromband > map_asymerrbf[runnumber] || yf_distancefromband > map_asymerryf[runnumber]
         || bf_phase_distancefromband > map_phaseerrbf[runnumber] || yf_phase_distancefromband > map_phaseerryf[runnumber])
    {
      stringMarkdown += "\u274C Local Polarimetry ";
      stringHtml += "Local Polarimetry: <font color=\"#ff0000\">BAD</font>; ";
    }
    else
    {
      stringMarkdown += "\u2705 Local Polarimetry ";
      stringHtml += "Local Polarimetry: <font color=\"#11cf17\">GOOD</font>; ";
    }
  }
    // if CNI does not have beam polarizations (see if you can get polarizations from local pol)
}

void SpinDBQA::CrossingAngleQA(std::string &stringMarkdown, std::string &stringHtml) {
  // Run crossing angle QA. Simply check the standard deviation per run and cut on a hard threshold.
  // Currently set to cut three most egregious runs where beams switch from 1.5 mrad to/from head on.

  // Check if crossing angle std exists and if it is above the threshold mark it as bad, good otherwise
  if (map_crossanglestd[runnumber] > _crossanglestdthreshold)
  {
    stringMarkdown += "\u274C Crossing angle variation ";
    stringHtml += "Crossing angle variation: <font color=\"#ff0000\">BAD</font>; ";
  }
  else
  {
    stringMarkdown += "\u2705 Crossing angle variation ";
    stringHtml += "Crossing angle variation: <font color=\"#11cf17\">GOOD</font>; ";
  }
}

void SpinDBQA::WriteNewQALevel(int newqalevel)
{
  std::cout << "Writing new QA level..." << std::endl;
  if (newqalevel < 0)
  {
    std::cout << "Please select a new qa level > 0 to write to!" << std::endl;
    return;
  }


  PrepareUpdatedBeamPolarizations();
  PrepareUpdatedCrossingShifts();
  PrepareUpdatedSpinPatterns();

  
  for (const auto &run : runlistvect)
  {
    int badrunqa = 0;
    runnumber = run;
    
    ////////////////////////////////////////////////////
    // Set new qa level
    // If no row for the run,qa_level create it.
    if(spin_in.CheckRunRow(runnumber,newqalevel)==0)
    {
      spin_in.CreateRunRow(runnumber,newqalevel);
      printf("\n\nCreated Row for run %d with qa level %d\n\n",runnumber,newqalevel);
    }
    spin_cont.SetRunNumber(runnumber);
    spin_cont.SetQALevel(newqalevel);
    ////////////////////////////////////////////////////

    ////////////////////////////////////////////////////
    // Set fill number
    spin_cont.SetFillNumber(map_fillnumber[runnumber]);
    ////////////////////////////////////////////////////

    ////////////////////////////////////////////////////
    // Set crossing shift
    if (map_crossingshift[runnumber] == -999)
    {
      badrunqa = 1;
    }
    spin_cont.SetCrossingShift(map_crossingshift[runnumber]);
    ////////////////////////////////////////////////////

    ////////////////////////////////////////////////////
    // *** "Bad" spin patterns reset to match CNI webpage *** //
    // Set spin patterns
    for (int i = 0; i < 120; i++)
    {
      spin_cont.SetSpinPatternBlue(i,map_bluespin[runnumber].at(i));
    }
    for (int i = 0; i < 120; i++)
    {
      spin_cont.SetSpinPatternYellow(i,map_yellspin[runnumber].at(i));
    }
    ////////////////////////////////////////////////////
    
    ////////////////////////////////////////////////////
    // *** Scraped from https://www.cnipol.bnl.gov/fills/ (although not final) *** //
    // Set beam polarizations
    if (map_bluepolbyfill.count(map_fillnumber[runnumber]) == 0 || map_yellpolbyfill.count(map_fillnumber[runnumber]) == 0)
    {
      badrunqa = 1;
      spin_cont.SetPolarizationBlue(0, -999., -999.);
      spin_cont.SetPolarizationYellow(0, -999., -999.);
    }
    else
    {
      spin_cont.SetPolarizationBlue(0, map_bluepolbyfill[map_fillnumber[runnumber]], map_bluepolbyfillerr[map_fillnumber[runnumber]]);
      spin_cont.SetPolarizationYellow(0, map_yellpolbyfill[map_fillnumber[runnumber]], map_yellpolbyfillerr[map_fillnumber[runnumber]]);
    }
    
    ////////////////////////////////////////////////////

    ////////////////////////////////////////////////////
    // Set MBD NS GL1p scalers
    for (int i = 0; i < 120; i++)
    {
      spin_cont.SetScalerMbdNoCut(i,map_mbdns[runnumber].at(i));
    }
    ////////////////////////////////////////////////////

    ///////////////////////////////////////////////////
    // Set MBD VTX GL1p scalers
    for (int i = 0; i < 120; i++)
    {
      spin_cont.SetScalerMbdVertexCut(i,map_mbdvtx[runnumber].at(i));
    }
    ////////////////////////////////////////////////////

    ////////////////////////////////////////////////////
    // Set ZDC NS GL1p scalers
    for (int i = 0; i < 120; i++)
    {
      spin_cont.SetScalerZdcNoCut(i,map_zdcns[runnumber].at(i));
    }
    ////////////////////////////////////////////////////

    ////////////////////////////////////////////////////
    // Set crossing angle
    if (map_crossanglestd[runnumber] > _crossanglestdthreshold)
    {
      badrunqa = 1;
    }
    spin_cont.SetCrossAngle(map_crossingangle[runnumber]);
    spin_cont.SetCrossAngleStd(map_crossanglestd[runnumber]);
    spin_cont.SetCrossAngleMin(map_crossanglemin[runnumber]);
    spin_cont.SetCrossAngleMax(map_crossanglemax[runnumber]);
    ////////////////////////////////////////////////////

    ////////////////////////////////////////////////////
    // Set local polarimetry stuff
    float bf_distancefromband = map_asymbf[runnumber] > bf_mean ? map_asymbf[runnumber]-(bf_mean+bf_rmse) : (bf_mean-bf_rmse)-map_asymbf[runnumber];
    float yf_distancefromband = map_asymyf[runnumber] > yf_mean ? map_asymyf[runnumber]-(yf_mean+yf_rmse) : (yf_mean-yf_rmse)-map_asymyf[runnumber];
    float bf_phase_distancefromband = map_phasebf[runnumber] > bf_phase_mean ? map_phasebf[runnumber]-(bf_phase_mean+bf_phase_rmse) : (bf_phase_mean-bf_phase_rmse)-map_phasebf[runnumber];
    float yf_phase_distancefromband = map_phaseyf[runnumber] > yf_phase_mean ? map_phaseyf[runnumber]-(yf_phase_mean+yf_phase_rmse) : (yf_phase_mean-yf_phase_rmse)-map_phaseyf[runnumber];
    
    // make sure that local polarimetry exists in the first place
    if (map_asymbf[runnumber] != 0 || map_asymbb[runnumber] != 0 || map_asymyf[runnumber] != 0 
        || map_asymyb[runnumber] != 0 || map_phasebf[runnumber] != 0 || map_phasebb[runnumber] != 0 
        || map_phaseyf[runnumber] != 0 || map_phaseyb[runnumber] != 0)
    {
      if (bf_distancefromband > map_asymerrbf[runnumber] || yf_distancefromband > map_asymerryf[runnumber]
         || bf_phase_distancefromband > map_phaseerrbf[runnumber] || yf_phase_distancefromband > map_phaseerryf[runnumber])
      {
        badrunqa = 1;
      }
    }


    
    spin_cont.SetAsymBlueForward(map_asymbf[runnumber], map_asymerrbf[runnumber]);
    spin_cont.SetAsymBlueBackward(map_asymbb[runnumber], map_asymerrbb[runnumber]);
    spin_cont.SetAsymYellowForward(map_asymyf[runnumber], map_asymerryf[runnumber]);
    spin_cont.SetAsymYellowBackward(map_asymyb[runnumber], map_asymerryb[runnumber]);
    spin_cont.SetPhaseBlueForward(map_phasebf[runnumber], map_phaseerrbf[runnumber]);
    spin_cont.SetPhaseBlueBackward(map_phasebb[runnumber], map_phaseerrbb[runnumber]);
    spin_cont.SetPhaseYellowForward(map_phaseyf[runnumber], map_phaseerryf[runnumber]);
    spin_cont.SetPhaseYellowBackward(map_phaseyb[runnumber], map_phaseerryb[runnumber]);
    ////////////////////////////////////////////////////

    ////////////////////////////////////////////////////
    // Set badrun flag
    spin_cont.SetBadRunFlag(badrunqa);
    ////////////////////////////////////////////////////

    // Update the database and set new default QA
    spin_in.UpdateDBContent(spin_cont);
    spin_in.SetDefaultQA(spin_cont);
    
    // ************************************ // 
    
  }
  
}


void SpinDBQA::PrepareUpdatedCrossingShifts()
{
  std::ifstream shiftsFile("../updatedcrossingshifts.dat");
  if (!shiftsFile.is_open()) 
  {
    std::cout << "Error: Could not open updatedcrossingshifts.dat" << std::endl;
    return;
  }

  std::string line;
  while (getline(shiftsFile, line)) 
  {
    if (line.empty())
    {
      continue;
    } 
    std::istringstream iss(line);
    int RunNumber;
    int crossingshift;

    if (!(iss >> RunNumber >> crossingshift)) 
    {
      std::cout << "Skipping invalid line: " << line << std::endl;
      continue;
    }
    
    map_crossingshift[RunNumber] = crossingshift;

  }
  shiftsFile.close();
}

void SpinDBQA::PrepareUpdatedBeamPolarizations()
{
  std::ifstream polarizationsFile("../updatedbeampolarizations.dat");
  if (!polarizationsFile.is_open()) 
  {
    std::cout << "Error: Could not open updatedbeampolarizations.dat" << std::endl;
    return;
  }

  std::string line;
  while (getline(polarizationsFile, line)) 
  {
    if (line.empty())
    {
      continue;
    } 
    std::istringstream iss(line);
    int FillNumber;
    float bluepol;
    float bluepolerr;
    float yellpol;
    float yellpolerr;

    if (!(iss >> FillNumber >> bluepol >> bluepolerr >> yellpol >> yellpolerr)) 
    {
      std::cout << "Skipping invalid line: " << line << std::endl;
      continue;
    }
    
    map_bluepolbyfill[FillNumber] = bluepol;
    map_bluepolbyfillerr[FillNumber] = bluepolerr;
    map_yellpolbyfill[FillNumber] = yellpol;
    map_yellpolbyfillerr[FillNumber] = yellpolerr;

  }
  polarizationsFile.close();
}

void SpinDBQA::PrepareUpdatedSpinPatterns()
{
  std::ifstream patternsFile("../updatedspinpatterns.dat");
  if (!patternsFile.is_open()) 
  {
    std::cout << "Error: Could not open correctedspinpatterns.dat" << std::endl;
    return;
  }

  std::string line;
  while (getline(patternsFile, line)) 
  {
    if (line.empty())
    {
      continue;
    } 
    std::istringstream iss(line);
    int RunNumber;
    std::string s_pattern_blue;
    std::string s_pattern_yell;

    if (!(iss >> RunNumber >> s_pattern_blue >> s_pattern_yell)) 
    {
      std::cout << "Skipping invalid line: " << line << std::endl;
      continue;
    }

    for (int i = 0; i < 120; ++i)
    {
      if (s_pattern_blue.at(i) == '-')
      {
        map_bluespin[RunNumber].at(i) = -1;
      }
      else if (s_pattern_blue.at(i) == '+')
      {
        map_bluespin[RunNumber].at(i) = 1;
      }
      else if (s_pattern_blue.at(i) == 'o')
      {
        map_bluespin[RunNumber].at(i) = 10;
      }

      if (s_pattern_yell.at(i) == '-')
      {
        map_yellspin[RunNumber].at(i) = -1;
      }
      else if (s_pattern_yell.at(i) == '+')
      {
        map_yellspin[RunNumber].at(i) = 1;
      }
      else if (s_pattern_yell.at(i) == 'o')
      {
        map_yellspin[RunNumber].at(i) = 10;
      }
    }
  }
  patternsFile.close();
}


void SpinDBQA::WriteRootFile()
{
  TFile *outfile = new TFile(_rootfilename.c_str(),"RECREATE");
  locpolbf->Write();
  locpolbb->Write();
  locpolyf->Write();
  locpolyb->Write();
  locpolphasebf->Write();
  locpolphasebb->Write();
  locpolphaseyf->Write();
  locpolphaseyb->Write();
  outfile->Write();
  outfile->Close();
}

void SpinDBQA::WriteMarkdown()
{
  std::cout << "Writing to Markdown!" << std::endl;
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

void SpinDBQA::WriteHtml()
{
  //============== Write HTML of spin db QA ============== //
  std::cout << "Writing to HTML!" << std::endl;
  if (!std::filesystem::exists(_htmlfilename)) 
  {
    std::cerr << "HTML file does not exist! Preparing new HTML." << std::endl;
    PrepareHtml();
  }

  // read in html file to string
  std::ifstream f_html(_htmlfilename);
  if (!f_html.is_open()) 
  {
      std::cerr << "WriteHtml: Could not open HTML file!" << std::endl;
      return;
  }

  std::string s_html((std::istreambuf_iterator<char>(f_html)), std::istreambuf_iterator<char>());
  f_html.close();

  std::string tabButtonOld = R"REGEX(<button[^>]*class="tab-button active"[^>]*onclick="openTab\(event,\s*'default'\)"[^>]*>QA Level: Default</button>)REGEX";
  std::string tabButtonNew = R"BUTTON(<button class="tab-button active" onclick="openTab(event, 'default')">QA Level: Default</button>)BUTTON";
  std::string tabContentOld = R"CONTENT(<div\s+id="default"[^>]*>([\s\S]*?)</div>)CONTENT";
  std::string tabContentNew = HtmlContent();

  std::string placeholderButton = "<!-- TAB BUTTONS WILL GO HERE -->";
  std::string placeholderContent = "<!-- TAB CONTENT WILL GO HERE -->";

  if (!b_defaultQA) 
  {
    std::regex patternButtonOld(tabButtonOld);
    std::smatch matchResultButtonOld;
    std::regex patternContentOld(tabContentOld);
    std::smatch matchResultContentOld;
    if (std::regex_search(s_html, matchResultButtonOld, patternButtonOld)) 
    {
      placeholderButton = matchResultButtonOld[0].str();
    }
    if (std::regex_search(s_html, matchResultContentOld, patternContentOld))
    {
      placeholderContent = matchResultContentOld[0].str();
    }

    std::string s_qa = "qa" + std::to_string(qalevel);
    std::string s_title = "QA Level: " + std::to_string(qalevel);
    if (qalevel == 0xFFFF){
      std::stringstream ss;
      ss << std::hex << qalevel;
      s_title = "QA Level: 0x" + ss.str();
    }
    tabButtonNew  = std::regex_replace(tabButtonNew,  std::regex("default"), s_qa);
    tabButtonOld  = std::regex_replace(tabButtonOld,  std::regex("default"), s_qa);
    tabContentOld = std::regex_replace(tabContentOld, std::regex("default"), s_qa);
    tabButtonNew  = std::regex_replace(tabButtonNew,  std::regex("QA Level: Default"), s_title);
    tabButtonOld  = std::regex_replace(tabButtonOld,  std::regex("QA Level: Default"), s_title);
    tabButtonNew  = std::regex_replace(tabButtonNew,  std::regex("tab-button active"), "tab-button");
    tabButtonOld  = std::regex_replace(tabButtonOld,  std::regex("tab-button active"), "tab-button");
    tabContentNew  = std::regex_replace(tabContentNew,  std::regex("tab-content active"), "tab-content");
  }

  // write tab button
  std::regex defaultButtonRegex(tabButtonOld + R"(\s*)");
  s_html = std::regex_replace(s_html, defaultButtonRegex, "");
  size_t posButton = s_html.find(placeholderButton);
  if (posButton != std::string::npos) 
  {
    s_html.insert(posButton + placeholderButton.length(), "\n  " + tabButtonNew);
  }

  // write tab content
  std::regex defaultContentRegex(tabContentOld + R"(\s*)");
  s_html = std::regex_replace(s_html, defaultContentRegex, "");
  size_t posContent = s_html.find(placeholderContent);
  if (posContent != std::string::npos) 
  {
    s_html.insert(posContent + placeholderContent.length(), "\n  " + tabContentNew);
  }

  // write html file
  std::ofstream f_html_out(_htmlfilename);
  if (!f_html_out.is_open()) {
      std::cerr << "Error: Could not open the HTML file for writing!" << std::endl;
      return;
  }

  f_html_out << s_html;
  f_html_out.close();

  return;
}

std::string SpinDBQA::HtmlContent()
{
  std::ostringstream stream_html;

  size_t totalRuns = 0;
  for (const auto &pair : map_spindbqa_html)
  {
    totalRuns += pair.second.size();
  }
  
  stream_html << R"(<div id="default" class="tab-content active">)" << "\n"
      << "  <table style=\"text-align: center;\">\n"
      << "    <tr>\n"
      << "      <th style=\"width: 1500px;\">Run Status</th>\n"
      << "      <th style=\"width: 50px;\">Number of Runs</th>\n"
      << "      <th style=\"width: 50px;\">Fraction</th>\n"
      << "    </tr>\n"
      << "    <tr>\n"
      << "      <td>" << "Total number of runs" << "</td>\n"
      << "      <td>" << totalRuns << "</td>\n"
      << "      <td>" << "-" << "</td>\n"
      << "    </tr>\n";

  for (auto& [statement, runs] : map_spindbqa_html)
  {
    size_t count = runs.size();

    double fraction = 0.0;
    if (totalRuns > 0) 
    {
        fraction = (static_cast<double>(count) / static_cast<double>(totalRuns)) * 100.0;
    }

    std::ostringstream fractionStr;
    fractionStr << std::fixed << std::setprecision(2) << fraction << "%";

    stream_html << "    <tr>\n"
          << "      <td>" << statement << "</td>\n"
          << "      <td>" << count << "</td>\n"
          << "      <td>" << fractionStr.str() << "</td>\n"
          << "    </tr>\n";

  }
  stream_html << "  </table>\n";

  for (auto& [statement, runs] : map_spindbqa_html)
    {
      std::sort(runs.begin(), runs.end());
      stream_html << "<strong>" << statement << "</strong><br>\n";
      for (int run : runs) {
        stream_html << run << " ";
      }
      stream_html << "<br><br>\n";
    } 

  stream_html  << "</div>";
  
  std::string s_stream_html = stream_html.str();
  std::string s_qa = "qa" + std::to_string(qalevel);
  if (!b_defaultQA)
  {
    s_stream_html  = std::regex_replace(s_stream_html,  std::regex("default"), s_qa);
    s_stream_html  = std::regex_replace(s_stream_html,  std::regex("tab-content active"), "tab-content");
  }

  return s_stream_html;
  
}

void SpinDBQA::PrepareHtml()
{
  std::ofstream f_html(_htmlfilename);
  if (!f_html.is_open()) 
  {
    std::cerr << "PrepareHtml: Could not open HTML file!" << std::endl;
  }
  else
  {
    f_html << R"(<!DOCTYPE html>
    <html lang="en">
  <head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Spin QA Run Summary</title>
  <style>
    body {
      font-family: 'Arial', sans-serif;
      margin: 0;
      padding: 20px;
      background-color: #f4f7f9;
      color: #333;
    }
    h2 {
      text-align: center;
      font-size: 2em;
      color: #4D89C6;
      margin-bottom: 20px;
    }

    .top-right-image {
      position: absolute;
      top: 10px;
      right: 10px;
    }

    .tabs {
      display: flex;
      justify-content: center;
      margin-bottom: 20px;
    }
    .tab-button {
      background-color: #ddd;
      border: none;
      padding: 10px 20px;
      cursor: pointer;
      font-size: 1em;
      transition: background-color 0.3s ease;
    }
    .tab-button.active {
      background-color: #4D89C6;
      color: white;
    }
    .tab-button:hover {
      background-color: #bbb;
    }
    .tab-content {
      display: none;
      padding: 20px;
      background-color: white;
      box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
    }
    .tab-content.active {
      display: block;
    }
    .run-list {
      font-size: 1em;
      margin-bottom: 10px;
      padding: 10px;
      border: 1px solid #ddd;
      background-color: #f9f9f9;
    }
  </style>
  </head>
  <body>

  <img src="sphenix-logo-white-bg.png" width="200" alt="Logo" class="top-right-image">

  <h2>Spin QA Run Summary</h2>

  <!-- Placeholder for the tab buttons -->
  <div class="tabs">
    <!-- TAB BUTTONS WILL GO HERE -->
  </div>

  <!-- Placeholder for the tab content -->
  <style>
    table {
      border-collapse: collapse;
      text-align: center;
    }
    th, td {
      padding: 8px;
    }
    tr {
      border-bottom: 1px solid black;
    }
    </style>
  <!-- TAB CONTENT WILL GO HERE -->

  <script>
    function openTab(event, tabName) {
      var tabContent = document.getElementsByClassName("tab-content");
      for (var i = 0; i < tabContent.length; i++) {
        tabContent[i].style.display = "none";
        tabContent[i].classList.remove("active");
      }
      var tabButtons = document.getElementsByClassName("tab-button");
      for (var i = 0; i < tabButtons.length; i++) {
        tabButtons[i].classList.remove("active");
      }
      document.getElementById(tabName).style.display = "block";
      document.getElementById(tabName).classList.add("active");
      event.currentTarget.classList.add("active");
    }
  </script>

  </body>
  </html>)";
  
  }
  f_html.close();
  return;
}