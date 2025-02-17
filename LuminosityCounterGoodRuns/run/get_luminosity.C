int get_luminosity(string rnlist, int trigger, int zsel)
{

  if(trigger < 0 || trigger > 63)
    {
      cout << "Trigger must be between 0 and 63 (inclusive)! Exiting." << endl;
      return 1;
    }
  if(zsel > 2 || zsel < 0)
    {
      cout << "z selection must be between 0 and 2 (inclusive) for |zvtx| < 30/60/200, respectively! Exiting." << endl;
      return 2;
    }

  ifstream is(rnlist);
  string rnstr;
  double lumi = 0;

  while(getline(is, rnstr))
    {
      TFile* file = TFile::Open(("output/added/triggeroutput_"+rnstr+".root").c_str(),"READ");
      if(!file)
	{
	  cout << "RN " << rnstr << " not found! Continuing." << endl;
	  continue;
	}
      long long unsigned int tottrigcounts[3][64];
      double avgPS[64];
      TTree* tree = (TTree*)file->Get("outt");
      if(!tree)
	{
	  cout << "RN " << rnstr << " does not contain tree outt! Continuing." << endl;
	  continue;
	}
      tree->SetBranchAddress("avgPS",avgPS);
      tree->SetBranchAddress("tottrigcounts",tottrigcounts);
      tree->GetEntry(0);
      if(avgPS[trigger] !=0) lumi += ((double)tottrigcounts[zsel][10])*avgPS[10]/avgPS[trigger];
      //cout << lumi << " " << tottrigcounts[zsel][10] << " " << avgPS[10] << " " << avgPS[trigger] << endl;
    }
  lumi /= (23e9);
  cout << "Trigger " << trigger << ": " << lumi << " pb^-1" << endl;
  return 0;
}
