#include "/sphenix/user/jocl/projects/chi2checker/src/dlUtility.h"

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
  float onehitprob = 0.175; //old: sqrt(mbdprob) - mbdprob; //probability of firing only one side
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

float p_from_truerate(float truerate, float beamrate, int n)
{
  float ratio = truerate/beamrate;
  return exp(-ratio)*pow(ratio,n)/factorial(n);
}

float get_true_rate(float colrate, float beamrate)
{
  float sum = 0;
  for(int i=1; i<10; ++i)
    {
      sum += pow(-log(1-colrate/beamrate),i)/factorial(i-1);
    }
  sum *= (beamrate-colrate);
  return sum;
}
int get_rate_map()
{
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  // for a single collision...
  float mbdprob = 0.562; //from PYTHIA or use 25.2/42; //MBD xsec / pp inel xsec
  const int ncount = 5;
  float beamrate = 111*78.2e3;
  float p_mbd_given_ncol[ncount];
  float p_of_n[ncount];
  const int nrate = 400000;
  float mbd_rate[nrate];
  float mbd_r2[nrate];
  float col_rate[nrate];
  float true_rate[nrate];
  float true_rate_sparse[nrate/100];
  float mbd_rate_sparse[nrate/100];
  for(int i=0; i<ncount; ++i)
    {
      p_mbd_given_ncol[i] = get_prob_given_ncol(i+1, mbdprob);
      cout << p_mbd_given_ncol[i] << endl;
    }
  for(int r=0; r<nrate; ++r)
    {
      col_rate[r] = 10*r;
      float total = 0;
      float total2 = 0;
      true_rate[r] = get_true_rate(col_rate[r],beamrate);
      for(int i=0; i<ncount; ++i)
	{
	  total += p_mbd_given_ncol[i]*p_from_rate(col_rate[r], beamrate, i+1);
	  total2 += p_mbd_given_ncol[i]*p_from_truerate(true_rate[r], beamrate, i+1);
	}
      if(total > 0.5) cout << total << endl;
      mbd_rate[r] = total*beamrate;
      mbd_r2[r] = total*beamrate;
      if(r%100==0)
	{
	  mbd_rate_sparse[r/100]=mbd_rate[r];
	  true_rate_sparse[r/100]=true_rate[r];
	  cout << "total p: " << total - total2 << endl;
	  cout << "rate diff [Hz]: "  << mbd_rate[r] - mbd_r2[r] << endl;
	}
    }
  TGraph* g = new TGraph(nrate, mbd_rate, col_rate);
  g->SetName("mbdtogr1");
  g->GetHistogram()->GetXaxis()->SetTitle("R_{MBD} [Hz]");
  g->GetHistogram()->GetYaxis()->SetTitle("R_{#geq1} [Hz]");
  TGraph* gr = new TGraph(nrate, mbd_rate, true_rate);
  gr->SetName("mbdtotrue");
  gr->GetHistogram()->GetXaxis()->SetTitle("R_{MBD} [Hz]");
  gr->GetHistogram()->GetYaxis()->SetTitle("R_{true} [Hz]");
  TGraph* sparse = new TGraph(nrate/100,mbd_rate_sparse,true_rate_sparse);
  TH1D* temphist = new TH1D("temphist",";R_{MBD} [Hz];R_{true} [Hz]",1,0,3000e3);
  temphist->GetYaxis()->SetRangeUser(0,6000e3);
  temphist->GetXaxis()->SetRangeUser(0,3000e3);
  
  sparse->SetMarkerStyle(20);
  sparse->SetMarkerSize(1);
  sparse->SetMarkerColor(kRed+2);
  gr->SetMarkerStyle(20);
  gr->SetMarkerSize(1);
  gr->SetMarkerColor(kBlue+2);
  TCanvas* c = new TCanvas("","",1000,1000);
  c->SetLeftMargin(0.15);
  c->SetBottomMargin(0.15);
  temphist->Draw();
  sparse->Draw("SAME P");
  //gr->Draw("SAME P");
  sphenixtext();
  TLegend* leg = new TLegend(0.5,0.2,0.8,0.3);
  leg->SetFillStyle(0);
  leg->SetFillColor(0);
  leg->SetBorderSize(0);
  leg->AddEntry(g,"R_{#geq1}","p");
  leg->AddEntry(gr,"R_{true}","p");
  //leg->Draw();
  c->SaveAs("ratevsrate.pdf");
  TFile* outf = new TFile("mbd_to_col_map.root","RECREATE");
  outf->cd();
  g->Write();
  gr->Write();
  outf->Close();
  return 0;
}
