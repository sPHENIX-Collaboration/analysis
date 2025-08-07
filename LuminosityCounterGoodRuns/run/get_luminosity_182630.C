/*
int get_scaler(int runnumber, int bit)
{
  TSQLServer *db = TSQLServer::Connect("pgsql://sphnxdaqdbreplica:5432/daq","","");

  if (db)
    {
      printf("Server info: %s\n", db->ServerInfo());
    }
  else
    {
      printf("bad\n");
    }

  TSQLRow *row;
  TSQLResult *res;
  TString cmd = "";
  char sql[1000];

  sprintf(sql, "select scaled from gl1_scalers where runnumber = %d and index = %d;", runnumber, bit);
  res = db->Query(sql);
  row = res->Next();
  try
    {
      int retval = stoi(row->GetField(0));
      delete row;
      delete res;
      delete db;
      return retval;
    }
  catch(...)
    {
      delete row;
      delete res;
      delete db;
      return 0;
    }
}
*/
int get_luminosity_182630(string rnlist, int zsel, int clt)
{
  gStyle->SetOptStat(0);

  if(zsel > 5 || zsel < 0)
    {
      cout << "z selection must be between 0 and 5 (inclusive) for |zvtx| < 30/60/200/10/1000/none, respectively! Exiting." << endl;
      return 2;
    }
  const int ntrig = 3;
  float mbsig = 25.2;
  int upperrun = 53000;
  int lowerrun = 47200;
  ifstream is(rnlist);
  string rnstr;
  TH1D* mbdrate = new TH1D("mbdrate","",upperrun-lowerrun,lowerrun-.5,upperrun-.5);
  TH1D* nSB10 = new TH1D("nSB10","",upperrun-lowerrun,lowerrun-0.5,upperrun-0.5);
  TH1D* nSB18 = new TH1D("nSB18","",upperrun-lowerrun,lowerrun-0.5,upperrun-0.5);
  TH1D* fsb10l30 = new TH1D("fsb10l30","",upperrun-lowerrun,lowerrun-0.5,upperrun-0.5);
  TH1D* fsb18l30 = new TH1D("fsb18l30","",upperrun-lowerrun,lowerrun-0.5,upperrun-0.5);
  //int it = 0;
  float lumi[ntrig] = {0};
  float uclumi[ntrig] = {0};
  int trigs[ntrig] = {18,22,30};
  cout << "RN   Bit"<<to_string(trigs[0])<<"Corr  Bit"<<to_string(trigs[0])<<"UC   Bit"<<to_string(trigs[1])<<"Corr  Bit"<<to_string(trigs[1])<<"UC   Bit"<<to_string(trigs[2])<<"Corr  Bit"<<to_string(trigs[2])<<"UC (units pb^-1)" << endl << endl;
  while(getline(is, rnstr))
    {
      //cerr << "RN: " << rnstr << endl;
      /*
      try
	{
	  if(stoi(rnstr) > 48862 && stoi(rnstr) < 48868) continue;
	}
      catch(...)
	{
	  continue;
	}
      */
      TFile* file;
      file = TFile::Open(("output/added_nblair/triggeroutput_nblair_"+rnstr+(clt?"_clt":"")+".root").c_str(),"READ");
      if(!file)
	{
	  cerr << "RN " << rnstr << " not found! Continuing." << endl;
	  cout << rnstr << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << endl;
	  continue;
	}
      long long unsigned int tottrigcounts[6][64];
      double avgPS[64];
      long long unsigned int sumgoodscaled[64];
      long long unsigned int sumgoodraw[64];
      long long unsigned int sumgoodlive[64];
      float nmbdc;
      float ttseg;
      float nblair;
      TTree* tree = (TTree*)file->Get("outt");

      if(!tree)
	{
	  cerr << "RN " << rnstr << " does not contain tree outt! Continuing." << endl;
	  cout << rnstr << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << endl;
	  continue;
	}
      tree->SetBranchAddress("avgPS",avgPS);
      tree->SetBranchAddress("sumgoodscaled",sumgoodscaled);
      tree->SetBranchAddress("sumgoodlive",sumgoodlive);
      tree->SetBranchAddress("sumgoodraw",sumgoodraw);
      tree->SetBranchAddress("tottrigcounts",tottrigcounts);
      tree->SetBranchAddress("nmbdc",&nmbdc);
      tree->SetBranchAddress("ttseg",&ttseg);
      tree->SetBranchAddress("nblair",&nblair);
      tree->GetEntry(0);


      /*
      try
	{
	  int scaleddb18 = get_scaler(stoi(rnstr),18);
	  /*
	  if(sumgoodscaled[18] < 0.99*scaleddb18)
	    {
	      cerr << sumgoodscaled[18] << " " << scaleddb18 << endl;
	      cerr << "< 99% of db" << endl;
	      cout << "Run " << rnstr << " " << 0 << " " <<0 << endl;
	      continue;
	    }
	}
      
      catch(...)
	{
	  cout << "Run " << rnstr << " " << 0 << " " << 0 << endl;
	  cerr << "error with getting db counts" << endl;
	  continue;
	}
      */
      //tree->SetBranchAddress("runLumi",runLumi);
      /*
	if(sumgoodlive[10]/sumgoodscaled[10] > avgPS[10]+0.0000000001)
	{
	  cout << rnstr << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << endl;
	  continue;
	}
      */
      //cout << rnstr << " " << ((double)tottrigcounts[zsel][10])*avgPS[10]/avgPS[trigger] << endl;
      if(nmbdc/((double)sumgoodraw[10]) < 1)
	{
	  cerr << rnstr << " " << nmbdc << " " << sumgoodraw[10] << " " << nmbdc / (double)sumgoodraw[10] << endl;
	  continue;
	}
      cout << rnstr;

      
      
      for(int i=0; i<3; ++i)
	{
	  int trigger = trigs[i];
	  if(avgPS[trigger] > 0 && !isnan(avgPS[trigger]) && !isnan(avgPS[10]) && !isnan((tottrigcounts[zsel][10])) && !isinf(avgPS[10]) && !isinf(avgPS[trigger]) && !isinf((tottrigcounts[zsel][10])) && avgPS[10] > 0)
	    {
	      
	      float clumiseg = (tottrigcounts[zsel][10])*avgPS[10]*((((nmbdc/((double)sumgoodraw[10])))))/avgPS[trigger]/(mbsig*1e9);
	      float uclumiseg = (tottrigcounts[zsel][10])*((double)avgPS[10])/avgPS[trigger]/(mbsig*1e9);//((double)(trigger==22?sumgoodscaled[10]:tottrigcounts[zsel][10]))*avgPS[10]/avgPS[trigger]/(mbdsig*1e9;
	      float blumiseg = (tottrigcounts[zsel][10])*avgPS[10]*((((nblair/((double)sumgoodraw[10]))-1)/2)+1)/avgPS[trigger]/(mbsig*1e9);
	      uclumi[i] += uclumiseg;
	      lumi[i] += clumiseg;
	      cout << " " << fixed << setprecision(7) << clumiseg << " " << uclumiseg;// << " " << blumiseg ;
	    }
	  else
	    {
	      cout << " " << 0 << " " << 0;// << " " << 0 ;
	    }
	  //if(runLumi > 0) lumi += runLumi;
	  //cout << lumi << " " << (trigger==22?sumgoodscaled[10]:tottrigcounts[zsel][10]) << " " << avgPS[10] << " " << avgPS[trigger] << endl;
	}
      cout << endl;
    }
  /*
  lumi[0] /= ((mbdsig*1e9);
  lumi[1] /= (mbdsig*1e9;
  lumi[2] /= (mbdsig*1e9;
  uclumi[0] /= ((mbdsig*1e9);
  uclumi[1] /= (mbdsig*1e9;
  uclumi[2] /= (mbdsig*1e9;
  */
  /*
  formatHist(nSB10,0,nSB10->GetMaximum()*1.1,"Run Number","N_{event} Firing MBDNS>=1 Trigger",kBlack,1.5);
  formatHist(nSB18,0,nSB18->GetMaximum()*1.1,"Run Number","N_{event} Firing Jet10+MBDNS>=1 Trigger",kBlack,1.5);
  formatHist(fsb10l30,0,1,"Run Number","N_{ev}MBDNS>=1 w/ |z_{vtx}|<30cm / N_{ev}MBDNS>=1",kBlack,1.5);
  formatHist(fsb18l30,0,1,"Run Number","N_{ev}Jet10+MBDNS>=1 w/ |z_{vtx}|<30cm / N_{ev}Jet10+MBDNS>=1",kBlack,1.5);
  */
  cout << "RN   Bit"<<to_string(trigs[0])<<"Corr  Bit"<<to_string(trigs[0])<<"UC   Bit"<<to_string(trigs[1])<<"Corr  Bit"<<to_string(trigs[1])<<"UC   Bit"<<to_string(trigs[2])<<"Corr  Bit"<<to_string(trigs[2])<<"UC (units pb^-1)" << endl << endl;
  cout << "Totals: " << fixed << setprecision(5)<<  lumi[0] << " " << uclumi[0] << " " << lumi[1] << " " << uclumi[1] << " " << lumi[2] << " " << uclumi[2] << " " << " pb^-1" << endl;
  /*
  mbdrate->SetMarkerStyle(20);
  mbdrate->SetMarkerSize(2);
  TCanvas* c1 = new TCanvas("","",2000,1000);
  gPad->SetLeftMargin(0.15);
  gPad->SetRightMargin(0.01);
  gPad->SetTopMargin(0.05);
  gPad->SetBottomMargin(0.15);
  mbdrate->GetYaxis()->SetTitle("MBD Live Rate [Hz]");
  mbdrate->GetXaxis()->SetTitle("Run Number");
  mbdrate->Draw("P HIST");
  c1->SaveAs("mbdrate_full.png");
  nSB10->Draw("P HIST");
  c1->SaveAs("nsb10.png");
  nSB18->Draw("P HIST");
  c1->SaveAs("nsb18.png");
  fsb10l30->Draw("P HIST");
  c1->SaveAs("fsb10l30.png");
  fsb18l30->Draw("P HIST");
  c1->SaveAs("fsb18l30.png");
  */
  return 0;
}
