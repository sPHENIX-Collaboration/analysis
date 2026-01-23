unsigned long long int factorial(int n)
{
  if(n < 2) return 1;
  return n*factorial(n-1);
}

void increment_in_base(vector<unsigned int> &counter, int base)
{
  
  for(int i = 0; i<counter.size(); ++i)
    {
      ++counter[i];
      if(counter[i] > base)
	{
	  cout << "SOMETHING WRONG - ENTRY > BASE" << endl;
	  exit(1);
	}
      if(counter[i] == base)
	{
	  counter[i] = 0;
	}
      else
	{
	  return;
	}
    }
}

float get_prob_given_ncol(int n, float mbdprob)
{
  float onehitprob = sqrt(mbdprob) - mbdprob; //probability of firing only one side
  float nohitsprob = 1 - mbdprob - 2*onehitprob; //
  vector<unsigned int> counter = {};
  float prob = 0;
  for(int i=0; i<n; ++i)
    {
      counter.push_back(0);
    }
  for(int i=0; i<pow(4,n); ++i)
    {
      int nnone = std::count(counter.begin(), counter.end(), 0);
      int nnorth = std::count(counter.begin(), counter.end(), 1);
      int nsouth = std::count(counter.begin(), counter.end(), 2);
      int nboth = std::count(counter.begin(), counter.end(), 3);
      if((nnorth && nsouth) || nboth)
	{
	  prob += pow(nohitsprob,nnone)*pow(onehitprob,nnorth)*pow(onehitprob,nsouth)*pow(mbdprob,nboth);
	}
      increment_in_base(counter, 4);
    }
  return prob;
}

float p_from_rate(float colrate, float beamrate, int n)
{
  float num = (1-colrate/beamrate)*pow(-log(1-colrate/beamrate),n);
  float den = factorial(n);
  return num/den;
}
int get_rate_map()
{
  // for a single collision...
  float mbdprob = 25.2/42; //MBD xsec / pp inel xsec
  const int ncount = 4;
  float beamrate = 111*78.2e3;
  float p_mbd_given_ncol[ncount];
  float p_of_n[ncount];
  const int nrate = 400000;
  float mbd_rate[nrate];
  float col_rate[nrate];
  for(int i=0; i<ncount; ++i)
    {
      p_mbd_given_ncol[i] = get_prob_given_ncol(i+1, mbdprob);
      cout << p_mbd_given_ncol[i] << endl;
    }
  for(int r=0; r<nrate; ++r)
    {
      col_rate[r] = 10*r;
      float total = 0;
      for(int i=0; i<ncount; ++i)
	{
	  total += p_mbd_given_ncol[i]*p_from_rate(col_rate[r], beamrate, i+1);
	}
      if(total > 0.5) cout << total << endl;
      mbd_rate[r] = total*beamrate;
    }
  TGraph* g = new TGraph(nrate, mbd_rate, col_rate);
  TFile* outf = new TFile("mbd_to_col_map.root","RECREATE");
  outf->cd();
  g->Write();
  outf->Close();
  return 0;
}
