
void calTowerCalib()
{
  const double energy_in[16] = {0.00763174, 0.00692298, 0.00637355, 0.0059323, 0.00762296, 0.00691832, 0.00636611, 0.0059203, 0.00762873, 0.00693594, 0.00637791, 0.00592433, 0.00762898, 0.00691679, 0.00636373, 0.00592433};
  const double adc_in[16] = {2972.61, 2856.43, 2658.19, 2376.10, 3283.39, 2632.81, 2775.77, 2491.68, 2994.11, 3385.70, 3258.01, 2638.31, 3479.97, 3081.41, 2768.36, 2626.77};
  const double gain_factor_in = 32.0;
  const double samplefrac_in = 0.09267;

  const double energy_out[16] = {0.00668176, 0.00678014, 0.00687082, 0.00706854, 0.00668973, 0.00678279, 0.00684794, 0.00705448, 0.00668976, 0.0068013, 0.00685931, 0.00704985, 0.0066926, 0.00678282, 0.00684403, 0.00704143};
  // const double adc_out[16] = {666.378, 1488.15, 1493.36, 1816.82, 666.378, 1488.15, 1493.36, 1816.82, 666.378, 1488.15, 1493.36, 1816.82, 666.378, 1488.15, 1493.36, 1816.82}; // use 1st column for whole HCALOUT
  const double adc_out[16] = {276.9, 290.0, 280.7, 272.1, 309.5, 304.8, 318.5, 289.6, 289.9, 324.2, 297.9, 294.6, 292.7, 310.5, 302.3, 298.5}; // Songkyo's number
  const double adc_amp[16] = {2.505, 5.330, 5.330, 6.965, 2.505, 5.330, 5.330, 6.965, 2.505, 5.330, 5.330, 6.965, 2.505, 5.330, 5.330, 6.965}; // amplify from 2017 to 2018
  const double gain_factor_out = 16.0;
  const double samplefrac_out = 0.02862;

  double towercalib_lg_in[16];
  double towercalib_lg_out[16];
  double towercalib_hg_out[16];
  for(int i_tower = 0; i_tower < 16; ++i_tower)
  {
    towercalib_lg_in[i_tower] = gain_factor_in*energy_in[i_tower]/(adc_in[i_tower]*samplefrac_in);
    towercalib_lg_out[i_tower] = gain_factor_out*energy_out[i_tower]/(adc_amp[i_tower]*adc_out[i_tower]*samplefrac_out);
    towercalib_hg_out[i_tower] = energy_out[i_tower]/(adc_amp[i_tower]*adc_out[i_tower]*samplefrac_out);
    // towercalib_lg_out[i_tower] = gain_factor_out*energy_out[i_tower]/(adc_out[i_tower]*samplefrac_out);
    // towercalib_hg_out[i_tower] = energy_out[i_tower]/(adc_out[i_tower]*samplefrac_out);

    cout << "i_tower = " << i_tower << ", towercalib_lg_in = " << towercalib_lg_in[i_tower] << ", towercalib_lg_out = " << towercalib_lg_out[i_tower] << endl;
  }
}
