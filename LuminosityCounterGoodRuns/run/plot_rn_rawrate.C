int plot_rn_rawrate(int lo, int hi, int bit, float max)
{
  gStyle->SetOptStat(0);
  int rn;
  double rawrate;

  vector<int> rnv = {};
  vector<double> rrv = {};
  
  ifstream ratefile;
  ratefile.open(("rn_rawrate_"+to_string(bit)+".txt").c_str());

  while(true)
    {
      if(ratefile.eof()) break;
      ratefile >> rn >> rawrate;
      if(rn > hi || rn < lo) continue;
      rnv.push_back(rn);
      rrv.push_back(rawrate);
    }

  TH1D* ratehist = new TH1D("ratehist","",hi-lo,lo,hi);


  for(int i=0; i<rnv.size(); ++i)
    {
      if(rrv.at(i) < max) /*&& rrv.at(i) > 50)*/ ratehist->Fill(rnv.at(i),rrv.at(i));
    }

  TCanvas* c1 = new TCanvas("","",2000,1000);
  float maxval=max;
  ratehist->SetMarkerStyle(20);
  ratehist->SetMarkerSize(1);
  ratehist->GetYaxis()->SetRangeUser(0,maxval);
  c1->cd();

  TF1* flatfunc = new TF1("flat","[0]",lo,hi);
  flatfunc->SetParameter(0,200);
  flatfunc->SetParameter(1,0);
  ratehist->Fit(flatfunc,"W");

  string trigger;
  if(bit==18) trigger = "Jet10&MBDNS>=1 ";
  else if(bit==10) trigger = "MBDNS>=1 ";
  else trigger = "";
  
  ratehist->GetYaxis()->SetTitle((trigger+"Raw Rate [Hz]").c_str());
  ratehist->GetXaxis()->SetTitle("Run Number");

  ratehist->Draw("HIST P");
  ratehist->GetFunction("flat")->Draw("SAME");
  vector<int> cosmics = {48795,49170,49782,49478,50572,51020,51020,52930,53321};
  vector<TLine*> lines = {};
  for(int i=0; i<cosmics.size(); ++i)
    {
      if(cosmics.at(i) > hi || cosmics.at(i) < lo) continue;
      TLine* mynewline = new TLine(cosmics.at(i),0,cosmics.at(i),maxval);
      mynewline->SetLineColor(kBlue);
      mynewline->SetLineWidth(1);
      lines.push_back(mynewline);
      lines.at(i)->Draw();
    }
  
  c1->SaveAs(("ratehist"+to_string(bit)+".png").c_str());

  return 0;
};
