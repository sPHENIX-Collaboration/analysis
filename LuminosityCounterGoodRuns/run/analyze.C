static const float radius_EM = 93.5;
static const float minz_EM = -130.23;
static const float maxz_EM = 130.23;

static const float radius_IH = 127.503;
static const float minz_IH = -170.299;
static const float maxz_IH = 170.299;

static const float radius_OH = 225.87;
static const float minz_OH = -301.683;
static const float maxz_OH = 301.683;

float get_emcal_mineta_zcorrected(float zvertex) {
  float z = minz_EM - zvertex;
  float eta_zcorrected = asinh(z / (float)radius_EM);
  return eta_zcorrected;
}

float get_emcal_maxeta_zcorrected(float zvertex) {
  float z = maxz_EM - zvertex;
  float eta_zcorrected = asinh(z / (float)radius_EM);
  return eta_zcorrected;
}

float get_ihcal_mineta_zcorrected(float zvertex) {
  float z = minz_IH - zvertex;
  float eta_zcorrected = asinh(z / (float)radius_IH);
  return eta_zcorrected;
}

float get_ihcal_maxeta_zcorrected(float zvertex) {
  float z = maxz_IH - zvertex;
  float eta_zcorrected = asinh(z / (float)radius_IH);
  return eta_zcorrected;
}

float get_ohcal_mineta_zcorrected(float zvertex) {
  float z = minz_OH - zvertex;
  float eta_zcorrected = asinh(z / (float)radius_OH);
  return eta_zcorrected;
}

float get_ohcal_maxeta_zcorrected(float zvertex) {
  float z = maxz_OH - zvertex;
  float eta_zcorrected = asinh(z / (float)radius_OH);
  return eta_zcorrected;
}

bool check_bad_jet_eta(float jet_eta, float zertex, float jet_radius) {
  float emcal_mineta = get_emcal_mineta_zcorrected(zertex);
  float emcal_maxeta = get_emcal_maxeta_zcorrected(zertex);
  float ihcal_mineta = get_ihcal_mineta_zcorrected(zertex);
  float ihcal_maxeta = get_ihcal_maxeta_zcorrected(zertex);
  float ohcal_mineta = get_ohcal_mineta_zcorrected(zertex);
  float ohcal_maxeta = get_ohcal_maxeta_zcorrected(zertex);
  float minlimit = emcal_mineta;
  if (ihcal_mineta > minlimit) minlimit = ihcal_mineta;
  if (ohcal_mineta > minlimit) minlimit = ohcal_mineta;
  float maxlimit = emcal_maxeta;
  if (ihcal_maxeta < maxlimit) maxlimit = ihcal_maxeta;
  if (ohcal_maxeta < maxlimit) maxlimit = ohcal_maxeta;
  minlimit += jet_radius;
  maxlimit -= jet_radius;
  return jet_eta < minlimit || jet_eta > maxlimit;
}

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
int get_fillnum(int runnumber)
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



  sprintf(sql, "select fillnumber, runnumber from spin where runnumber = %d;", runnumber);
  res = db->Query(sql);
  if(!res) return -1;
  row = res->Next();
  if(!row) return -1;

  string fillnum(row->GetField(0));
  return stoi(fillnum);
}

unsigned int factorial(int n)
{
  if(n>10)
    {
      cout << "Don't do that." << endl;
      throw exception();
    }
  if(n==0) return 1;
  if(n==1) return 1;
  return n*factorial(n-1);
}
  
int analyze(int rn, int nseg, int clt)
{

  ifstream fillnumfile("/sphenix/user/jocl/projects/analysis/LuminosityCounterGoodRuns/run/fillnumbers.txt");
  int fills[719][2];

  for(int i = 0; i<719; ++i)
    {
      fillnumfile >> fills[i][0] >> fills[i][1];
    }
  
  long long unsigned int firstlivescaler[64] = {0};
  long long unsigned int lastlivescaler[64] = {0};
  long long unsigned int sumgoodlive[64] = {0};
  long long unsigned int totlive[64] = {0};

  long long unsigned int firstscaledscaler[64] = {0};
  long long unsigned int lastscaledscaler[64] = {0};
  long long unsigned int sumgoodscaled[64] = {0};
  long long unsigned int totscaled[64] = {0};

  long long unsigned int firstrawscaler[64] = {0};
  long long unsigned int lastrawscaler[64] = {0};
  long long unsigned int sumgoodraw[64] = {0};
  long long unsigned int totraw[64] = {0};
  
  long long unsigned int trigcounts[6][64] = {0};
  double embdlive[5] = {0};

  long long unsigned int nevt = 0;

  double avgPS[64] = {0};
  long long unsigned int tottrigcounts[6][64] = {0};
  double totembdlive[5] = {0};
  //TH1D* zhist;
  bool gothist = false;
  float nmbdc = 0;
  float nblair = 0;
  float ttseg = 0;
  int fillnum = get_fillnum(rn);
  for(int i=1; i<nseg+1; ++i)
    {
      //cout << "start" << endl;
      //cout << "test1" << endl;
      TFile* file = TFile::Open(("/sphenix/user/jocl/projects/trigcount_files/"+to_string(rn)+"/triggercounter_5z_"+to_string(rn)+"_"+to_string(i)+(clt?"_clt":"")+".root").c_str());
      //cout << "test2" << endl;
      //cout << "gettree" << endl;
      TTree* tree = (TTree*)file->Get("_tree");
      //cout << "test3" << endl;
      //cout << "gottree" << endl;
      long long unsigned int segloraw[64] = {0};
      long long unsigned int seghiraw[64] = {0};
      long long unsigned int segloscaled[64] = {0};
      long long unsigned int seghiscaled[64] = {0};
      long long unsigned int seglolive[64] = {0};
      long long unsigned int seghilive[64] = {0};
      long long unsigned int startBCO = 0;
      long long unsigned int endBCO = 0;
      int badflag = 0;

      long long unsigned int segevt = 0;
      //cout << "setbranches" << endl;
      //cout << "test4" << endl;
      tree->SetBranchAddress("badFlag",&badflag);
      tree->SetBranchAddress("startScal",segloscaled);
      tree->SetBranchAddress("endScal",seghiscaled);
      tree->SetBranchAddress("startRaw",segloraw);
      tree->SetBranchAddress("endRaw",seghiraw);
      tree->SetBranchAddress("startLive",seglolive);
      tree->SetBranchAddress("endLive",seghilive);
      tree->SetBranchAddress("nevt",&segevt);
      tree->SetBranchAddress("trigCounts",trigcounts);
      tree->SetBranchAddress("eMBDlive",embdlive);
      tree->SetBranchAddress("startBCO",&startBCO);
      tree->SetBranchAddress("endBCO",&endBCO);
      //cout << "test5" << endl;
      //cout << "getentry" << endl;
      tree->GetEntry(0);
      //cout << "test6" << endl;
      //cout << "gotentry" << endl;
      float tSeg = (endBCO-startBCO)/(9.4e6);
      ttseg += tSeg;
      if(tSeg == 0)
	{
	  cout << "rn " << rn << " seg " << i << " tseg = 0" << endl;
	  continue;
	}
      //cout << endBCO << " " << startBCO << endl;
      //cout << badflag << endl;
      nevt += segevt;

      //cout << "test7" << endl;
      int nBunch = -1;
      for(int i=0; i<719; ++i)
        {
          if(fillnum == fills[i][0])
            {
              nBunch = fills[i][1];
              break;
            }
        }

      if(fillnum == -999 || nBunch == -1)
	{
	  nBunch = 111;
	}

      if(nBunch != 111)
	{
	  cout << "NBunch/rn: " << nBunch << " " << rn << endl;
	}
      
      if((rn >= 48863 && rn <= 48867) || (rn <= 51508 && rn >= 51506)) nBunch = 111;
      if(i==1)
	{
	  for(int j=0; j<64; ++j)
	    {
	      firstrawscaler[j] = segloraw[j];
	      firstlivescaler[j] = seglolive[j];
	      firstscaledscaler[j] = segloscaled[j];
	    }
	}
      for(int j=0; j<64; ++j)
	{
	  if(seghilive[j] != 0)
	    {
	      lastrawscaler[j] = seghiraw[j];
	      lastlivescaler[j] = seghilive[j];
	      lastscaledscaler[j] = seghiscaled[j];
	    }
	}
      for(int j=0; j<5; ++j)
	{
	  totembdlive[j] += embdlive[j];
	}
      float avgTot[64] = {0};
      float rB = nBunch*78.2e3; //nBunch obtained from CAD tarball provided by Kin Yip
      float rM = (seghiraw[10]-segloraw[10])/tSeg; //tSeg from BCO difference between start and end of segment * beam clock (9.4 MHz)
      if(rM > rB)
	{
	  cout << "rM > rB!! " << rB << " " << rM << " " << tSeg << endl;
	}
      if(rM > 0.82*rB)
	{
	  cout << "rM too high!" << rB << " " << rM << " " << tSeg << endl;
	}
      float pSum = 0;
      for(int k=1; k<10; ++k)
	{
	  pSum += pow(-log(1-rM/rB),k) / factorial(k-1); //sum probability * number of collisions for rate
	}
      //cout << endl;
      for(int j=0; j<64; ++j)
	{
	  for(int k=0; k<6; ++k)
	    {
	      tottrigcounts[k][j] += trigcounts[k][j]; //number of triggers actually in segment for three zvtx selections, all trigger bits
	    }
	  
	  sumgoodscaled[j] += (seghiscaled[j] - segloscaled[j]); //scaled counts as taken from difference of scaled scaler at start and end of segment
	  sumgoodraw[j] += (seghiraw[j] - segloraw[j]);
	  sumgoodlive[j] += (seghilive[j] - seglolive[j]); //use regular live counts for all triggers except 10
	  if(j==10)
	    {
	      nmbdc += tSeg * (rB-rM) * pSum; //N_{coll} for trigger 10
	      nblair += (seghiraw[j]-segloraw[j])*(1-log(1-rM/rB));
	    }
	}
      //cout << i << " " << sumgoodscaled[10] << " " << sumgoodscaled[18] << endl;
      if(i%10 == 0)
	{
	  cout << "Segment " << i << " " << tSeg << " " << (rB - rM) << " " << pSum << " " << seghiraw[10] - segloraw[10] << endl;
	}
      //cout << "Run " << rn << " Segment " << i << " time: " << tSeg << " beam rate: " << rB << " bunches: " << nBunch <<" MBD live rate: " << rM << " Sum(n*p(n)): " << pSum << " lambda: " << -log(1-rM/rB) << endl
      //cout << i << endl;
      file->Close();
      //cout << "closed" << endl;
    }
      
  cout << "sumgoodraw / nmbdc " << sumgoodraw[10] << " " << nmbdc << endl;
  
  //int totalgood = zhist->Integral();
  for(int i=0; i<64; ++i)
    {
      avgPS[i] = sumgoodscaled[i]==0?0:((double)sumgoodlive[i])/sumgoodscaled[i]; //calculate average prescale for each trigger
      totlive[i] = lastlivescaler[i] - firstlivescaler[i];
      totscaled[i] = lastscaledscaler[i] - firstscaledscaler[i];
      totraw[i] = lastrawscaler[i] - firstrawscaler[i];
    }


  TFile* outfile = TFile::Open(("triggeroutput_nblair_"+to_string(rn)+(clt?"_clt":"")+".root").c_str(),"RECREATE");
  outfile->cd();
  TTree* outt = new TTree("outt","A persevering date tree");
  
  outt->Branch("totlive",totlive,"totlive[64]/l");
  outt->Branch("totscaled",totscaled,"totscaled[64]/l");
  outt->Branch("totraw",totraw,"totraw[64]/l");
  outt->Branch("sumgoodscaled",sumgoodscaled,"sumgoodscaled[64]/l");
  outt->Branch("sumgoodraw",sumgoodraw,"sumgoodraw[64]/l");
  outt->Branch("sumgoodlive",sumgoodlive,"sumgoodlive[64]/l");
  outt->Branch("avgPS",avgPS,"avgPS[64]/D");
  outt->Branch("tottrigcounts",tottrigcounts,"tottrigcounts[6][64]/l");
  outt->Branch("totembdlive",totembdlive,"totembdlive[5]/D");
  outt->Branch("nevt",&nevt,"nevt/l");
  outt->Branch("nmbdc",&nmbdc,"nmbdc/F");
  outt->Branch("nblair",&nblair,"nblair/F");
  outt->Branch("ttseg",&ttseg,"ttseg/F");
  outt->Fill();
  outt->Write();
  //zhist->Write();
  outfile->Close();
  return 0;
}
