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

double
get_parametrization_R(double x, double Q2)
{
  double R_tmp = 1;

  if ( x < 0.12 )
    {
      R_tmp = 1.509 * pow(x, -0.0458) * pow((1-x), -0.0084) / log(Q2 / 0.04) - 0.203;
    }
  else
    {
      /* below parametrization returns NAN- debug and use instead of this line for x > 0.12 */
      R_tmp = 1.509 * pow(x, -0.0458) * pow((1-x), -0.0084) / log(Q2 / 0.04) - 0.203;
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


int
calculate_g1( double x_val = 7.94e-05, double Q2_val = 1.33e+00, double y_val = 0.84, double N_val = 1.07e+04 )
{
  /* calculate statistical uncertainty on number of recorded events */
  double N_sig = 1./sqrt(N_val);

  /* F2 and R parametrizations (evaluated at kinmatics of given data point) */
  double F2 = get_parametrization_F2(x_val, Q2_val);
  double R = get_parametrization_R(x_val, Q2_val); // ratio of longitudinal and transverse photoabsorption cross section, R = simga_L / sigma_T

  /* Parameters needed */
  double M = 0.938; // nuclean mass in GeV
  double ml = 0.000511; // electron mass in GeV
  double gamma = ( 2 * M * x_val ) / sqrt(Q2_val); // ?
  double D = ( ( y_val * ( 2 - y_val ) * ( 1 + gamma * gamma * y_val / 2. ) ) /
	       ( y_val * y_val * ( 1 + gamma * gamma ) * (1 - 2 * ml * ml / ( Q2_val ) )
		 + 2 * (1 - y_val - gamma * gamma * y_val * y_val / 4 ) * (1 + R ))); // Depolarization factor
  double F1 = (1 + gamma * gamma) / (2 * x_val * (1 + R) ) * F2;

  /* Asymmetry measured in parallel spin configuration */
  // double A_parralel = D * A1; // assuming A2 = 0  // Ernst's thesis Eq. 3.41
  double A_parallel = 0.033;

  /* Calculate g1 */
  double g1_val = (1. / (1. + gamma * gamma)) * A_parallel / D * F1;
  // double g1_val = F2 / (2 * x * (1 + R)) * A1; // Ernst's thesis Eq. 3.47

  /* Calculate uncertainty of g1 */
  double g1_sig = 0;

  /* print parameters */
  cout << "x = " << x_val << " , Q2 = " << Q2_val << " , y = " << y_val << " , N = " << N_val << " +/- " << N_sig << endl;
  cout << "F1 = " << F1 << ", F2 = " << F2 << ", R = " << R << ", D = " << D << endl;
  cout << "==> g1 = " << g1_val << " +/- " << g1_sig << endl;

  return 0;
}
