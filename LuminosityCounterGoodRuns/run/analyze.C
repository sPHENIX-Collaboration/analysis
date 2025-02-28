int analyze(int rn, int nseg)
{
  
  long long unsigned int firstlivescaler[64] = {0};
  long long unsigned int lastlivescaler[64] = {0};
  long long unsigned int sumgoodlive[64] = {0};
  long long unsigned int totlive[64] = {0};

  long long unsigned int firstscaledscaler[64] = {0};
  long long unsigned int lastscaledscaler[64] = {0};
  long long unsigned int sumgoodscaled[64] = {0};
  long long unsigned int totscaled[64] = {0};

  long long unsigned int trigcounts[3][64] = {0};
  double embdlive[3] = {0};

  long long unsigned int nevt = 0;

  double avgPS[64] = {0};
  long long unsigned int tottrigcounts[3][64] = {0};
  double totembdlive[3] = {0};

  TH1D* zhist;
  bool gothist = false;
  for(int i=1; i<nseg+1; ++i)
    {
      TFile* file = TFile::Open(("srces/triggercounter_"+to_string(rn)+"_"+to_string(i)+".root").c_str());
      TTree* tree = (TTree*)file->Get("_tree");
      long long unsigned int segloscaled[64] = {0};
      long long unsigned int seghiscaled[64] = {0};
      long long unsigned int seglolive[64] = {0};
      long long unsigned int seghilive[64] = {0};
      int badflag = 0;
      
      long long unsigned int segevt = 0;
      tree->SetBranchAddress("badFlag",&badflag);
      tree->SetBranchAddress("startScal",segloscaled);
      tree->SetBranchAddress("endScal",seghiscaled);
      tree->SetBranchAddress("startLive",seglolive);
      tree->SetBranchAddress("endLive",seghilive);
      tree->SetBranchAddress("nevt",&segevt);
      tree->SetBranchAddress("trigCounts",trigcounts);
      tree->SetBranchAddress("eMBDlive",embdlive);
      tree->GetEntry(0);
      //cout << badflag << endl;
      nevt += segevt;
      
      if(i==1)
	{
	  for(int j=0; j<64; ++j)
	    {
	      firstlivescaler[j] = seglolive[j];
	      firstscaledscaler[j] = segloscaled[j];
	    }
	}
      for(int j=0; j<64; ++j)
	{
	  if(seghilive[j] != 0)
	    {
	      lastlivescaler[j] = seghilive[j];
	      lastscaledscaler[j] = seghiscaled[j];
	    }
	}
      
      if(!gothist && !badflag)
	{
	  zhist = (TH1D*)file->Get("mbzhist");
	  gothist = true;
	}
      else if(!badflag)
	{
	  zhist->Add((TH1D*)file->Get("mbzhist"));
	}
      else
	{
	  continue;
	}


      for(int j=0; j<3; ++j)
	{
	  totembdlive[j] += embdlive[j];
	}
      for(int j=0; j<64; ++j)
	{
	  for(int k=0; k<3; ++k)
	    {
	      tottrigcounts[k][j] += trigcounts[k][j];
	    }
	  sumgoodscaled[j] += (seghiscaled[j] - segloscaled[j]);
	  sumgoodlive[j] += (seghilive[j] - seglolive[j]);
	}
    }

  int totalgood = zhist->Integral();
  for(int i=0; i<64; ++i)
    {
      avgPS[i] = sumgoodscaled[i]==0?0:((double)sumgoodlive[i])/sumgoodscaled[i];
      totlive[i] = lastlivescaler[i] - firstlivescaler[i];
      totscaled[i] = lastscaledscaler[i] - firstscaledscaler[i];
    }

  TFile* outfile = TFile::Open(("triggeroutput_"+to_string(rn)+".root").c_str(),"RECREATE");
  outfile->cd();
  TTree* outt = new TTree("outt","A persevering date tree");
  outt->Branch("totlive",totlive,"totlive[64]/l");
  outt->Branch("totscaled",totscaled,"totscaled[64]/l");
  outt->Branch("sumgoodscaled",sumgoodscaled,"sumgoodscaled[64]/l");
  outt->Branch("sumgoodlive",sumgoodlive,"sumgoodlive[64]/l");
  outt->Branch("avgPS",avgPS,"avgPS[64]/D");
  outt->Branch("tottrigcounts",tottrigcounts,"tottrigcounts[3][64]/l");
  outt->Branch("totembdlive",totembdlive,"totembdlive[3]/D");
  outt->Branch("nevt",&nevt,"nevt/l");
  outt->Fill();
  outt->Write();
  zhist->Write();
  outfile->Close();
  return 0;
}
