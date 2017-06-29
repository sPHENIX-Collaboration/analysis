void MakeFile(){

  gSystem->Load("libg4eval.so");
  gSystem->Load("libqa_modules.so");
  gSystem->Load("libPrototype3.so");
 gStyle->SetOptFit(0);
   gStyle->SetOptStat(0);


//TFile *fin = new TFile("2nd_positionscan.lst_EMCalCalib.root");


   TFile *fin = new TFile("3rd_positionscan.lst_EMCalCalib.root");

TTree *t = (TTree *)fin->Get("T");

  t->SetAlias("C2_Inner_e", "1*abs(TOWER_RAW_C2[2].energy)");
  t->SetAlias("C2_Outer_e", "1*abs(TOWER_RAW_C2[3].energy)");
t->SetAlias("Average_column", "Sum$(TOWER_CALIB_CEMC.get_column() * TOWER_CALIB_CEMC.get_energy())/Sum$(TOWER_CALIB_CEMC.get_energy())");
  t->SetAlias("Average_HODO_HORIZONTAL", "Sum$(TOWER_CALIB_HODO_HORIZONTAL.towerid * (abs(TOWER_CALIB_HODO_HORIZONTAL.energy)>30) * abs(TOWER_CALIB_HODO_HORIZONTAL.energy))/Sum$((abs(TOWER_CALIB_HODO_HORIZONTAL.energy)>30) * abs(TOWER_CALIB_HODO_HORIZONTAL.energy))");
  t->SetAlias("Valid_HODO_HORIZONTAL", "Sum$(abs(TOWER_CALIB_HODO_HORIZONTAL.energy)>30) > 0");
  t->SetAlias("No_Triger_VETO", "Sum$(abs(TOWER_RAW_TRIGGER_VETO.energy)>15)==0");
 t->SetAlias("Valid_HODO_VERTICAL", "Sum$(abs(TOWER_CALIB_HODO_VERTICAL.energy)>30) > 0");
  t->SetAlias("C2_Sum_e", "C2_Inner_e + C2_Outer_e");
  t->SetAlias("Average_HODO_VERTICAL","Sum$(TOWER_CALIB_HODO_VERTICAL.towerid * (abs(TOWER_CALIB_HODO_VERTICAL.energy)>30) * abs(TOWER_CALIB_HODO_VERTICAL.energy))/Sum$((abs(TOWER_CALIB_HODO_VERTICAL.energy)>30) * abs(TOWER_CALIB_HODO_VERTICAL.energy))");
//return;
  t->SetAlias("Energy_Sum_CEMC", "1*Sum$(TOWER_CALIB_CEMC.get_energy())");

TH3F *Energyhis = new TH3F("Energyhis","",170,170,340,185,60,245,200,0,10);

 t->Draw("clus_5x5_prod.sum_E:info.beam_2CV_mm + 5*info.hodo_v:info.beam_2CH_mm - 5*info.hodo_h>>Energyhis","good_e");



//TFile *fout =  new TFile("His2.root","RECREATE");
TFile *fout =  new TFile("His3.root","RECREATE");
	
 
 Energyhis->Write();

	double mean = Energyhis->GetMean();

	cout << "Mean is =  "<< mean << endl;


}
