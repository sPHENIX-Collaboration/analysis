struct EfficiencyCorrection
{
  std::vector<float> pt_center = {0.65,      0.95,     1.25,      1.6,       2.,        2.6,       3.5     };
  std::vector<float> pt_low    = {0.5,       0.8,      1.1,       1.4,       1.8,       2.2,       3.      };
  std::vector<float> pt_high   = {0.8,       1.1,      1.4,       1.8,       2.2,       3.,        4.      };
  std::vector<float> eff       = {0.975164,  1.02684,  1.09048,   1.11564,   1.05061,   1.02402,   1.03747 };
  std::vector<float> eff_err   = {0.0167643, 0.013714, 0.0173775, 0.0218095, 0.0325241, 0.0452745, 0.111922};
/*
  // get efficiency for a particular bin
  float get_bin_eff(float bin_low, float bin_high)
  {
    int low_index = -1;
    int high_index = -1;
    for(int i=0; i<pt_center.size(); i++)
    {
      if(pt_low[i]<bin_low) low_index = i;
    }

    for(int i=pt_center.size()-1; i>=0; i--)
    {
      if(pt_high[i]>bin_high) high_index = i;
    }
    
    if(low_index==high_index)
    {
      if(bin_low<pt_center[low_index] && bin_high<pt_center[low_index])
      {
        
      }
    }
    else
    {
      float low_bin_frac, high_bin_frac;
      if(bin_low<pt_center[low_index])
      {
        low_bin_frac = 0.5 + 0.5*(pt_center[low_index]-bin_low)/(pt_center[low_index]-pt_low[low_index]);
      }
      else
      {
        low_bin_frac = 0.5*(bin_low-pt_center[low_index])/(pt_high[low_index]-pt_center[low_index]);
      }
      if(bin_high>pt_center[high_index])
      {
        high_bin_frac = 0.5 + 0.5*(bin_high-pt_center[high_index])/(pt_high[high_index]-pt_center[high_index]);
      }
      else
      {
        high_bin_frac = 0.5*(bin_high-pt_center[high_index]
      }
  }
*/

  float get_binval(float pt, std::vector<float>& v)
  {
    int index = -1;
    for(int i=0;i<v.size();i++)
    {
      if(pt>pt_low[i]) index = i;
    }
    if(index==-1 && pt<pt_high.back()) index = v.size()-1;
    if(index!=-1)
    {
      return v[index];
    }
    else return 0.;
  }

  float get_eff(float pt)
  {
    return get_binval(pt,eff);
  }

  float get_eff_error(float pt)
  {
    return get_binval(pt,eff_err);
  }
};

/* from Tony:

pT 0.65 ptlow 0.5 pthigh 0.8 double ratio 0.975164 error 0.0167643
pT 0.95 ptlow 0.8 pthigh 1.1 double ratio 1.02684 error 0.013714
pT 1.25 ptlow 1.1 pthigh 1.4 double ratio 1.09048 error 0.0173775
pT 1.6 ptlow 1.4 pthigh 1.8 double ratio 1.11564 error 0.0218095
pT 2 ptlow 1.8 pthigh 2.2 double ratio 1.05061 error 0.0325241
pT 2.6 ptlow 2.2 pthigh 3 double ratio 1.02402 error 0.0452745
pT 3.5 ptlow 3 pthigh 4 double ratio 1.03747 error 0.111922

*/
