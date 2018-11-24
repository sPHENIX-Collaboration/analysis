/** Parametrization for F2(x,Q2)
 */
double
get_parametrization_F2(double x, double Q2)
{
  /* based on reference from Ernst's thesis */
  double a1 = -0.24997;
  double a2 = 2.3963;
  double a3 = 0.23643;
  double a4 = 0.08498;
  double a5 = 3.8608;
  double a6 = -7.4143;
  double a7 = 3.4342;
  double b1 = 0.11411;
  double b2 = -2.2356;
  double b3 = 0.03115;
  double b4 = 0.02135;
  double c1 = -1.4517;
  double c2 = 8.4745;
  double c3 = -34.379;
  double c4 = 45.888;

  double A = pow(x, a1) * pow((1 - x), a2) * (a3 * pow((1 - x), 0) +
                                              a4 * pow((1 - x), 1) +
                                              a5 * pow((1 - x), 2) +
                                              a6 * pow((1 - x), 3) +
                                              a7 * pow((1 - x), 4));
  double B = b1 + b2 * x + (b3 / (x + b4));
  double C = ( c1 * pow(x, 1) +
               c2 * pow(x, 2) +
               c3 * pow(x, 3) +
               c4 * pow(x, 4) );
  double Q02 = 20;
  double lambda2 = 1;

  double F2_fit = A * pow( ( log(Q2 / lambda2) / log(Q02 / lambda2) ), B) * (1 + (C / Q2));

  return F2_fit;
}

/** Parametrization for R(x,Q2)
 */
double
get_parametrization_R(double x, double Q2)
{
  double R_tmp = 1;

  if ( x < 0.12 )
    {
      R_tmp = 1.509 * pow(x, -0.0458) * pow((1-x), -0.0084) / log((Q2 / 0.04)) - 0.203;
    }
  else
    {
      /* below parametrization returns NAN- debug and use instead of this line for x > 0.12 */
      R_tmp = 1.509 * pow(x, -0.0458) * pow((1-x), -0.0084) / log((Q2 / 0.04)) - 0.203;
    }
  //    {
  //      // R from fit to SLAC data
  //      double b1 = 0.635;
  //      double b2 = 0.5747;
  //      double b3 = -0.3534;
  //      double lambda = 0.2;
  //      double Theta = 1 + 12 * (Q2 / (Q2 + 1)) * (pow(0.125, 2) / (pow(0.125, 2) + pow(x, 2)));
  //      R_tmp = b1 / log( Q2 / pow(lambda, 2)) * Theta + b2 / Q2 + b3 / (pow(Q2, 2) + pow(0.3, 2)));
  //    }

  return R_tmp;
}

/** Calculate depolarization factor D(x,Q2,y,m_lepton)
 */
double
get_depolarization_factor(double x, double Q2, double y, double mlepton=0.000511)
{
  /* Parameters needed */
  double M = 0.938; // nucleon mass in GeV
  double ml = mlepton; // lepton mass in GeV

  double gamma2 = pow(( 2 * M * x ), 2) / Q2; // ?

  double R = get_parametrization_R(x, Q2); // ratio of longitudinal and transverse photoabsorption cross section, R = simga_L / sigma_T

  //double D_num = (
  //                y * ( 2 - y ) * ( 1 + gamma2 * y / 2. )
  //                );
  double D_num = (
                  y * ( 2 - y ) * ( 1 + gamma2 * y / 2. ) - 2 * pow(y, 3) * pow(ml, 2) / Q2
                  ); // Ernst's thesis has the extra y^3 term
  double D_denom = (
                    pow(y, 2) * ( 1 + gamma2 ) * (1 - 2 * pow(ml, 2) / Q2 )
                    + 2 * (1 - y - gamma2 * pow(y, 2) / 4 ) * (1 + R )
                    );
  double D = D_num / D_denom; // Depolarization factor

  return D;
}

/** Calculate beam polarization
 */
double
get_beam_polarization()
{
  /* assumed beam polarizarion factors */
  double pol_electron = 0.7;
  double pol_proton = 0.7;
  double pol_prod = pol_electron * pol_proton;

  return pol_prod;
}

/** Calculate value of g1
 */
double
get_g1_value( double x_val, double Q2_val, double y_val, double mlepton=0.000511 )
{
  /* F2 and R parametrizations (evaluated at kinmatics of given data point) */
  double F2 = get_parametrization_F2(x_val, Q2_val);
  double R = get_parametrization_R(x_val, Q2_val); // ratio of longitudinal and transverse photoabsorption cross section, R = simga_L / sigma_T

  /* Depolarization factor */
  double D = get_depolarization_factor( x_val, Q2_val, y_val, mlepton );

  /* Asymmetry measured in parallel spin configuration */
  // double A_parralel = D * A1; // assuming A2 = 0  // Ernst's thesis Eq. 3.41

  /* A_raw = (N++ - N+-) / (N++ + N+-) */
  //double A_raw = ?
  //double A_parallel = 1. / pol_prod * A_raw;

  double A1_Ernst = 0.033;
  double A_parallel = D * A1_Ernst;

  double g1_val = 1. / (2 * x_val * (1 + R)) * F2 * A_parallel / D;

  //  return g1_val;
  return 0;
}

/** Caluclate g1 statistical uncertainty
 */
double
get_g1_sigma( double x_val, double Q2_val, double y_val, double N_val, double mlepton=0.000511 )
{
  /* F2 and R parametrizations (evaluated at kinmatics of given data point) */
  double F2 = get_parametrization_F2(x_val, Q2_val);
  double R = get_parametrization_R(x_val, Q2_val); // ratio of longitudinal and transverse photoabsorption cross section, R = simga_L / sigma_T

  /* Depolarization factor */
  double D = get_depolarization_factor( x_val, Q2_val, y_val, mlepton );

  /* Beam polarization */
  double pol_prod = get_beam_polarization();

  double A_parallel_sig = (sqrt(N_val) / N_val) * (1. / pol_prod);
  double g1_sig =  1. / (2 * x_val * (1 + R)) * F2 * A_parallel_sig / D;

  return g1_sig;
}

/** Caluclate A1 statistical uncertainty
 */
double
get_A1_sigma( double x_val, double Q2_val, double y_val, double N_val, double mlepton=0.000511 )
{
  /* Calculate F1 */
  double M = 0.938; // nucleon mass in GeV

  double gamma2 = pow(( 2 * M * x_val ), 2) / Q2_val; // ?

  double F2 = get_parametrization_F2(x_val, Q2_val);
  double R = get_parametrization_R(x_val, Q2_val); // ratio of longitudinal and transverse photoabsorption cross section, R = simga_L / sigma_T

  double F1 = ( ( 1 + gamma2 ) / ( 2 * x_val * ( 1 + R ) ) ) * F2;

  /* Calculate A1 sigma (note: A1 = g1 / F1) */
  double g1_sig = get_g1_sigma( x_val, Q2_val, y_val, N_val, mlepton );
  double A1_sig = g1_sig / F1;

  return A1_sig;
}

/** Printing values on screen
 */
void
calculate_g1( double x_val, double Q2_val, double y_val, double N_val, double mlepton=0.000511 )
{
  double g1_val = get_g1_value(x_val, Q2_val, y_val, mlepton);
  double g1_sig = get_g1_sigma(x_val, Q2_val, y_val, N_val, mlepton);
  double F2 = get_parametrization_F2(x_val, Q2_val);
  double R = get_parametrization_R(x_val, Q2_val);
  double D = get_depolarization_factor( x_val, Q2_val, y_val, mlepton );

  /* print parameters */
  cout << "-----------------------------------------------------------------------" << endl;
  cout << "x = " << x_val << " , Q2 = " << Q2_val << " , y = " << y_val << " , N = " << N_val << endl;
  cout << "F2 = " << F2 << ", R = " << R << ", D = " << D << endl;
  cout << "==> g1 = " << g1_val << " +/- " << g1_sig << endl;
  cout << "-----------------------------------------------------------------------" << endl;

  return;
}
