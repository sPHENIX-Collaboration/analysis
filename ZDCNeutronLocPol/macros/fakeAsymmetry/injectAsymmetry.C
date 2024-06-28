#include "TRandom3.h"


void injectAsymmetry()
{
  //============================= Get spin pattern manually - use '111x111_P4' ================================//
  int bpat[120] = {0};
  int ypat[120] = {0};
  std::string preset_pattern_blue = "-+-++-+-+-+--+-++-+--+-+-+-++-+-+-+--+-++-+--+-+-+-++-+-+-+--+-++-+--+-+-+-++-+-+-+--+-++-+--+-+-+-++-+-+-+--+-*********";
  std::string preset_pattern_yellow = "--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--+*********";
  
  for (int i = 0; i < 120; i++)
  {
    if (preset_pattern_blue.at(i) == '+') {bpat[i] = 1;}
    else if (preset_pattern_blue.at(i) == '-'){bpat[i] = -1;}
    else if (preset_pattern_blue.at(i) == '*'){bpat[i] = 10;}

    if (preset_pattern_yellow.at(i) == '+'){ypat[i] = 1;}
    else if (preset_pattern_yellow.at(i) == '-'){ypat[i] = -1;}
    else if (preset_pattern_yellow.at(i) == '*'){ypat[i] = 10;}

  }


  int bunchnumber;
  int showerCutN, showerCutS;
  float n_x, n_y, s_x, s_y;
  float zdcN1_adc, zdcN2_adc, zdcS1_adc, zdcS2_adc;
  float veto_NF, veto_NB, veto_SF, veto_SB;

  TTree *smdHits = new TTree();
  smdHits  = new TTree("smdHits","smdHits");
  smdHits -> SetDirectory(0);
  smdHits -> Branch("bunchnumber",  &bunchnumber, "bunchnumber/I");
  smdHits -> Branch("showerCutN",  &showerCutN, "showerCutN/I");
  smdHits -> Branch("showerCutS",  &showerCutS, "showerCutS/I");
  smdHits -> Branch("n_x", &n_x, "n_x/F");
  smdHits -> Branch("n_y", &n_y, "n_y/F");
  smdHits -> Branch("s_x", &s_x, "s_x/F");
  smdHits -> Branch("s_y", &s_y, "s_y/F");
  smdHits -> Branch("zdcN1_adc", &zdcN1_adc, "zdcN1_adc/F");
  smdHits -> Branch("zdcN2_adc", &zdcN2_adc, "zdcN2_adc/F");
  smdHits -> Branch("zdcS1_adc", &zdcS1_adc, "zdcS1_adc/F");
  smdHits -> Branch("zdcS2_adc", &zdcS2_adc, "zdcS2_adc/F");
  smdHits -> Branch("veto_NF", &veto_NF, "veto_NF/F");
  smdHits -> Branch("veto_NB", &veto_NB, "veto_NB/F");
  smdHits -> Branch("veto_SF", &veto_SF, "veto_SF/F");
  smdHits -> Branch("veto_SB", &veto_SB, "veto_SB/F");


  TRandom3 Rand; 
  for (int i = 0; i < 1000000; i++)
  {

    if (i % 100000 == 0){std::cout << "entry " << i << std::endl;}
    bunchnumber = Rand.Uniform(0,111);
    //n_y = Rand.Gaus(0, 1.5);
    //s_y = Rand.Gaus(0, 1.5);

    n_y = Rand.Gaus(0.8, 1.5);
    s_y = Rand.Gaus(0, 1.5);
    
    if (bpat[bunchnumber] == 1)
    {
      n_x = Rand.Gaus(-0.01, 1.5);
    }
    else if (bpat[bunchnumber] == -1)
    {
      n_x = Rand.Gaus(0.01, 1.5);
    }

    if (ypat[bunchnumber] == 1)
    {
      s_x = Rand.Gaus(-0.01, 1.5);
    }
    else if (ypat[bunchnumber] == -1)
    {
      s_x = Rand.Gaus(0.01, 1.5);
    }

    zdcN1_adc = Rand.Uniform(0,1000); //keep all events above zdc cuts
    zdcN2_adc = Rand.Uniform(0,1000);
    zdcS1_adc = Rand.Uniform(0,1000);
    zdcS2_adc = Rand.Uniform(0,1000);
    veto_NF = Rand.Uniform(0,1); //keep all events within veto cut
    veto_NB = Rand.Uniform(0,1);
    veto_SF = Rand.Uniform(0,1);
    veto_SB = Rand.Uniform(0,1);

    showerCutN = Rand.Uniform(0,1) > 0.5 ? 1 : 1; //keep all shower cuts 
    showerCutS = Rand.Uniform(0,1) > 0.5 ? 1 : 1; 
   
    smdHits->Fill();

  }

  TFile *outfile = new TFile("FakeAsymmetry.root","RECREATE");
  
  smdHits->Write();
  outfile->Write();
  outfile->Close();

}
