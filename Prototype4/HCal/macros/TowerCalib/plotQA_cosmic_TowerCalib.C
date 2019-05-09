#include "TFile.h"
#include "TString.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TF1.h"
#include <string>

void plotQA_cosmic_TowerCalib(const string det = "HCALOUT")
{
  int hbdchanIHC[4][4] = {{16, 12, 8, 4},
                          {15, 11, 7, 3},
                          {14, 10, 6, 2},
                          {13,  9, 5, 1}};

  float sim_out_start[4] = {6.1,5.9,5.5,5.5};
  float sim_out_stop[4] = {15.0,15.0,15.0,15.0};
  float raw_out_start[4] = {1000.0,500.0,1000.0,1000.0};
  float raw_out_stop[4] = {5000.0,2500.0,6000.0,5000.0};

  float energy[16] = {-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0};
  float adc[16] = {-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0};
  // float tower_calib[16] = {-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0};

  /*
  string inputfile_sim = Form("/sphenix/user/xusun/software/data/cosmic/TowerCalibAna/Proto4TowerInfoSIM_%s.root",det.c_str()); 
  TFile *File_InPut_SIM = TFile::Open(inputfile_sim.c_str());

  TH1F *h_hcal_tower_sim[16];
  TF1 *f_landau_hcal_sim[16];
  for(int i_tower = 0; i_tower < 16; ++i_tower)
  {
    string HistName = Form("h_m%s_SIM_twr_%d",det.c_str(),i_tower);
    h_hcal_tower_sim[i_tower] = (TH1F*)File_InPut_SIM->Get(HistName.c_str())->Clone();
    string FuncName = Form("f_m%s_SIM_twr_%d",det.c_str(),i_tower);
    f_landau_hcal_sim[i_tower] = new TF1(FuncName.c_str(),"landau",0,20);
  }

  TCanvas *c_hcal_sim = new TCanvas("c_hcal_sim","c_hcal_sim",10,10,1600,1600);
  c_hcal_sim->Divide(4,4);
  for(int i_row = 0; i_row < 4; ++i_row)
  {
    for(int i_col = 0; i_col < 4; ++i_col)
    {
      int i_pad = 4*i_row+i_col+1;
      int i_tower = hbdchanIHC[i_row][i_col]-1;
      c_hcal_sim->cd(i_pad);
      h_hcal_tower_sim[i_tower]->GetXaxis()->SetRangeUser(2,20);
      h_hcal_tower_sim[i_tower]->Draw();

      for(int i_par = 0; i_par < 3; ++i_par)
      {
	f_landau_hcal_sim[i_tower]->ReleaseParameter(i_par);
      }
      f_landau_hcal_sim[i_tower]->SetParameter(0,10.0);
      f_landau_hcal_sim[i_tower]->SetParameter(1,10.0);
      f_landau_hcal_sim[i_tower]->SetParameter(2,10.0);
      if(det == "HCALIN") f_landau_hcal_sim[i_tower]->SetRange(5.0,15.0);
      if(det == "HCALOUT") f_landau_hcal_sim[i_tower]->SetRange(sim_out_start[i_row],sim_out_stop[i_row]);
      if(h_hcal_tower_sim[i_tower]->GetEntries() > 100)
      {
	h_hcal_tower_sim[i_tower]->Fit(f_landau_hcal_sim[i_tower],"R");
	energy[i_tower] = f_landau_hcal_sim[i_tower]->GetParameter(1)/1000.0; // convert to GeV
	string lable = Form("MPV %1.2f",f_landau_hcal_sim[i_tower]->GetParameter(1));
	TLegend *leg = new TLegend(0.4,0.5,0.8,0.8);
	leg->SetFillColor(0);
	leg->SetBorderSize(0);
	leg->AddEntry(h_hcal_tower_sim[i_tower],lable.c_str(),"L");
	leg->Draw("same");
      }
    }
  }
  string fig_sim = Form("./figures/sPHENIX_CollMeeting/c_%s_sim.eps",det.c_str());
  c_hcal_sim->SaveAs(fig_sim.c_str());
  */

  string inputfile_raw = Form("/sphenix/user/xusun/TestBeam/TowerCalibAna/Proto4TowerInfoRAW_%s.root",det.c_str()); 
  TFile *File_InPut_RAW = TFile::Open(inputfile_raw.c_str());

  TH1F *h_hcal_tower_raw[16];
  TF1 *f_landau_hcal_raw[16];
  for(int i_tower = 0; i_tower < 16; ++i_tower)
  {
    string HistName = Form("h_m%s_RAW_twr_%d",det.c_str(),i_tower);
    h_hcal_tower_raw[i_tower] = (TH1F*)File_InPut_RAW->Get(HistName.c_str())->Clone();
    string FuncName = Form("f_m%s_RAW_twr_%d",det.c_str(),i_tower);
    f_landau_hcal_raw[i_tower] = new TF1(FuncName.c_str(),"landau",0,16000);
  }

  TCanvas *c_hcal_raw = new TCanvas("c_hcal_raw","c_hcal_raw",10,10,1600,1600);
  c_hcal_raw->Divide(4,4);
  for(int i_row = 0; i_row < 4; ++i_row)
  {
    for(int i_col = 0; i_col < 4; ++i_col)
    {
      int i_pad = 4*i_row+i_col+1;
      int i_tower = hbdchanIHC[i_row][i_col]-1;
      c_hcal_raw->cd(i_pad);
      h_hcal_tower_raw[i_tower]->Draw();

      for(int i_par = 0; i_par < 3; ++i_par)
      {
	f_landau_hcal_raw[i_tower]->ReleaseParameter(i_par);
      }
      f_landau_hcal_raw[i_tower]->SetParameter(0,10.0);
      f_landau_hcal_raw[i_tower]->SetParameter(1,10.0);
      f_landau_hcal_raw[i_tower]->SetParameter(2,10.0);
      if(det == "HCALIN") f_landau_hcal_raw[i_tower]->SetRange(2000.0,8000.0);
      if(det == "HCALOUT") f_landau_hcal_raw[i_tower]->SetRange(raw_out_start[i_row],raw_out_stop[i_row]);
      if(h_hcal_tower_raw[i_tower]->GetEntries() > 100)
      {
	h_hcal_tower_raw[i_tower]->Fit(f_landau_hcal_raw[i_tower],"R");
	adc[i_tower] = f_landau_hcal_raw[i_tower]->GetParameter(1);
	string lable = Form("MPV %4.1f",f_landau_hcal_raw[i_tower]->GetParameter(1));
	TLegend *leg = new TLegend(0.4,0.5,0.8,0.8);
	leg->SetFillColor(0);
	leg->SetBorderSize(0);
	leg->AddEntry(h_hcal_tower_raw[i_tower],lable.c_str(),"L");
	leg->Draw("same");
      }
    }
  }
  string fig_raw = Form("./figures/HCAL_TowerMap/c_%s_raw.eps",det.c_str());
  c_hcal_raw->SaveAs(fig_raw.c_str());

  /*
  string outputfile = Form("towercalib_%s.txt",det.c_str());
  ofstream File_OutPut(outputfile.c_str());
  for(int i_tower = 0; i_tower < 16; ++i_tower)
  {
    File_OutPut << det.c_str() << "_Tower_" << i_tower << ": energy = " << energy[i_tower] << ", adc = " << adc[i_tower]<< endl;
  }
  File_OutPut.close();
  */
}
