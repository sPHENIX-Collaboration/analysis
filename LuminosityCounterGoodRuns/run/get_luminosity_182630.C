bool get_is15_mrad(int runnumber)
{


  TSQLServer *db = TSQLServer::Connect("pgsql://sphnxdbreplica:5432/spinDB","phnxrc","");
  if(!db)
    {
      cout << "FAILED TO CONNECT TO DB!" << endl;
      throw exception();
    }
  TSQLRow *row;
  TSQLResult *res;
  char sql[1000];



  sprintf(sql, "select crossingangle, runnumber from spin where runnumber = %d;", runnumber);
  res = db->Query(sql);
  if(!res) return -1;
  row = res->Next();
  if(!row) return -1;



  string xing(row->GetField(0));
  float xingangle = stof(xing);
  if(db) delete db;
  return fabs(xingangle)>0.75;
}

int get_luminosity_182630(string rnlist, int zsel, int clt, int checkxing = 0)
{
  gStyle->SetOptStat(0);

  if(zsel > 5 || zsel < 0)
    {
      cout << "z selection must be between 0 and 5 (inclusive) for |zvtx| < 30/60/200/10/1000/none, respectively! Exiting." << endl;
      return 2;
    }
  const int ntrig = 4;
  float mbsig = 25.2;
  int upperrun = 54000;
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
  int trigs[ntrig] = {10,18,22,30};
  cout << "RN   Bit"<<to_string(trigs[0])<<"Corr  Bit"<<to_string(trigs[0])<<"UC   Bit"<<to_string(trigs[1])<<"Corr  Bit"<<to_string(trigs[1])<<"UC   Bit"<<to_string(trigs[2])<<"Corr  Bit"<<to_string(trigs[2])<<"UC   Bit"<<to_string(trigs[3])<<"Corr  Bit"<<to_string(trigs[3])<<"UC (units pb^-1)" << endl << endl;
  while(getline(is, rnstr))
    {
      TFile* file;
      file = TFile::Open(("output/added_509/triggeroutput_nblair_"+rnstr+(clt?"_clt":"")+".root").c_str(),"READ");
      if(checkxing==2 && !get_is15_mrad(stoi(rnstr))) continue;
      if(checkxing==1 && get_is15_mrad(stoi(rnstr))) continue;
	
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
      float nocorN;
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
      tree->SetBranchAddress("nocorN",&nocorN);
      tree->SetBranchAddress("ttseg",&ttseg);
      tree->SetBranchAddress("nblair",&nblair);
      tree->GetEntry(0);


      if(nmbdc/((double)nocorN) < 1)
	{
	  cerr << rnstr << " " << nmbdc << " " << nocorN << " " << nmbdc / (double)nocorN << endl;
	  continue;
	}
      cout << rnstr;

      
      
      for(int i=0; i<ntrig; ++i)
	{
	  int trigger = trigs[i];
	  if(avgPS[trigger] > 0 && !isnan(avgPS[trigger]) && !isnan(avgPS[10]) && !isnan((tottrigcounts[zsel][10])) && !isinf(avgPS[10]) && !isinf(avgPS[trigger]) && !isinf((tottrigcounts[zsel][10])) && avgPS[10] > 0)
	    {
	      
	      float clumiseg = (tottrigcounts[zsel][10])*avgPS[10]*((((nmbdc/((double)nocorN)))))/avgPS[trigger]/(mbsig*1e9);
	      float uclumiseg = (tottrigcounts[zsel][10])*((double)avgPS[10])/avgPS[trigger]/(mbsig*1e9);//((double)(trigger==22?sumgoodscaled[10]:tottrigcounts[zsel][10]))*avgPS[10]/avgPS[trigger]/(mbdsig*1e9;
	      float blumiseg = (tottrigcounts[zsel][10])*avgPS[10]*((((nblair/((double)nocorN))-1)/2)+1)/avgPS[trigger]/(mbsig*1e9);
	      uclumi[i] += uclumiseg;
	      lumi[i] += clumiseg;
	      cout << " " << fixed << setprecision(7) << clumiseg << " " << uclumiseg;// << " " << blumiseg ;
	    }
	  else
	    {
	      cout << " " << 0 << " " << 0;// << " " << 0 ;
	    }
	}
      cout << endl;
      file->Close();
    }
  cout << "RN   Bit"<<to_string(trigs[0])<<"Corr  Bit"<<to_string(trigs[0])<<"UC   Bit"<<to_string(trigs[1])<<"Corr  Bit"<<to_string(trigs[1])<<"UC   Bit"<<to_string(trigs[2])<<"Corr  Bit"<<to_string(trigs[2])<<"UC (units pb^-1)" << endl << endl;
  cout << "Totals: " << fixed << setprecision(5)<<  lumi[0] << " " << uclumi[0] << " " << lumi[1] << " " << uclumi[1] << " " << lumi[2] << " " << uclumi[2] << " " << " pb^-1" << endl;
  return 0;
}
