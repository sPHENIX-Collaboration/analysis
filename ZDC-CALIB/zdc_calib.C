//get zdc weigthts per side for 3 modules
//updated for sPHENIX ZDC
//Ejiro Umaka, Peter Steinberg


#define Nmodules 3
float E_n;

float no_booster_weight[3] = {1,1,1};
std::vector<float> skip_zdc_mod = {1,1,1};

//change per distribution
float uncorr_lo = 133.138;
float uncorr_hi = 224.234;


bool calculateWeights(float energy,
		      std::vector< std::vector<float> >& in_data,
		      TVectorD& out_weights,
		      TMatrixD& out_matrix
		      )
{
  
  int n_zdc = in_data.size();
  int dim = n_zdc+1;

  std::cout << "Calculate weights for " << n_zdc << " modules!" << std::endl;

  TVectorD mom1(dim);
  
  size_t nevt = in_data.at(0).size();
  for (size_t i = 0;i<nevt;i++)
    {
      for (size_t ir=0;ir<n_zdc;ir++)
	{
	  mom1(ir) += in_data.at(ir).at(i)/double(nevt);
	  for (size_t ic=0;ic<n_zdc;ic++)
	    {
	      out_matrix(ir,ic) += 2*in_data.at(ir).at(i)*in_data.at(ic).at(i)/double(nevt);
	    }
	}
    }


  for (size_t ir=0;ir<n_zdc;ir++)
    {
      out_matrix(ir,n_zdc)=mom1(ir);
      out_matrix(n_zdc,ir)=mom1(ir);
    }

  TVectorD solution(dim);
  solution(n_zdc) = energy;

  TDecompLU lu(out_matrix);
  lu.Decompose();
  bool status = lu.Solve(solution);

  if (!status) 
   {
     std::cout << "Fail!" << endl;
    }
  else
    {
      std::cout << "Success!" << endl;
    }

  solution.Print();

  out_weights = solution;

  return status;
}


//put input file and set constraint energy to 100 GeV
void zdc_calib(TString filename = "", float e = 100)

{
   
   TString s_zdc = "";
   for (int iz=0;iz<Nmodules;iz++)
    {
      s_zdc += TString::Itoa(skip_zdc_mod[iz],10);
    }
  
  TFile fout("output"+filename+"_"+s_zdc+"_fout.root","RECREATE");
  TH1F h_e_uncorr("h_e_uncorr","Uncorr. energy;E_{uncorr}",3000,0,6000);
  TH2F h_e_uncorr2D("h_e_uncorr2D","Uncorr. energy;E_{ZDC} [GeV]};E_{EM} [GeV];",100,0,300,100,0,300);
  TH1F h_e_corr("h_e_corr","corr. energy;E_{corr}",3000,0,6000);
  TH2F h_e_corr2D("h_e_corr2D","corr. energy;E_{ZDC} [GeV];E_{EM} [GeV]",100,0,600,100,0,600);
  
  int n_zdc = std::accumulate(skip_zdc_mod.begin(),skip_zdc_mod.end(),0);
  std::cout << "Working with " << n_zdc << " modules!" << std::endl;

  TMatrixD* zdcMatrix = new TMatrixD(n_zdc+1,n_zdc+1);
  TVectorD* zdcWeights = new TVectorD(n_zdc+1);

  
  std::vector< std::vector<float> > data;
  data.resize(n_zdc);
  std::vector< std::vector<float> > all_data;
  all_data.resize(n_zdc); 

  ifstream ifs(filename);

  int event;
  float x,y;
  std::vector<float> z(Nmodules);
  int Nevents;

 
    
float thissum = 0.;
  while (!ifs.eof())
    {
                
      ifs >> z[0] >> z[1] >> z[2];
                
      float sum = 0;
      float sum_had = 0;
      bool fail_event = false;
      
      for (int iz=0;iz<Nmodules;iz++)
	{
	  z[iz] = (z[iz]<4000?z[iz]:0)/no_booster_weight[iz];
        
	  if (!skip_zdc_mod[iz] && z[iz]>10) fail_event = true;
	  
	  sum += z[iz]*skip_zdc_mod[iz];
	  if (iz>0) sum_had += z[iz]*skip_zdc_mod[iz]; 
	}
      if (fail_event) continue;
     
      int ind = 0;
      if (sum>uncorr_lo&&sum<uncorr_hi)
	{
	  for (int iz=0;iz<Nmodules;iz++)
	    {
	      if (skip_zdc_mod[iz])
		{
		  data.at(ind).push_back(z[iz]);
		  ind++;
		}
	    }
	}

      ind = 0;
      for (int iz=0;iz<Nmodules;iz++)
	{
	  if (skip_zdc_mod[iz])
	    {
	      all_data.at(ind).push_back(z[iz]);
	      ind++;
	    }
	}
      
      h_e_uncorr.Fill(sum);
      h_e_uncorr2D.Fill(sum_had,sum-sum_had);
    }

  calculateWeights(e, data, *zdcWeights, *zdcMatrix);

  int Nevt = all_data.at(0).size();
  std::cout << Nevt  << " events used!" << std::endl;
  for (int i = 0;i<Nevt;i++)
    {

      float e_corr = 0;
      float e_corr_had = 0;
      int ind = 0;
      for (int iz=0;iz<Nmodules;iz++)
	{
	  if (skip_zdc_mod[iz])
	    {
	      e_corr += all_data.at(ind).at(i) * (*zdcWeights)(ind);
	      if (iz>0) e_corr_had += all_data.at(ind).at(i) * (*zdcWeights)(ind);
	      ind++;
        }
	}
        
      h_e_corr.Fill(e_corr);
      h_e_corr2D.Fill(e_corr_had,e_corr-e_corr_had);

    }
    
  fout.cd();
  zdcWeights->Write("zdcTBWeights");
  fout.Write();
  fout.Close();

}

