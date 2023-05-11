int
create_pythia_dummy()
{
  /* Open output for dummy file */
  ofstream ofs("pythia_dummy_new.dat");

  /* Number of particles per event in dummy file */
  unsigned npart = 100;

  /* Number of events */
  unsigned nevents = 10000;

  /* first record after header and scattered electron */
  unsigned I0 = 14;

  /* Add Pythia style header */
  ofs << "  PYTHIA EVENT FILE " << endl;
  ofs << " ============================================"  << endl;
  ofs << "    I, ievent, genevent, subprocess, nucleon, targetparton, xtargparton, beamparton, xbeamparton, thetabeamprtn, truey, trueQ2, truex, trueW2, trueNu, leptonphi, s_hat, t_hat, u_hat, pt2_hat, Q2_hat, F2, F1, R, sigma_rad, SigRadCor, EBrems, photonflux, t-diff, nrTracks" << endl;
  ofs << " ============================================" << endl;
  ofs << "    I  K(I,1)  K(I,2)  K(I,3)  K(I,4)  K(I,5)  P(I,1)  P(I,2)  P(I,3)  P(I,4)  P(I,5)  V(I,1)  V(I,2)  V(I,3)" << endl;
  ofs << " ============================================" << endl;


  /* Loop over events */
  for ( unsigned event = 0; event < nevents; event++ )
    {
      ofs << "   0          " << event << "    1   28 2212          2  0.626588           21     0.021799     0.000606      0.11143372116        1.29649562992        0.00058761008     2205.97153388399     1175.77442684038        0.45626715567       30.13077585993          -6.512950687         -23.617825173           5.105136735           5.105136735           0.000000000           0.000000000           0.000000000           0.000000000     0.000000000           0.000000000           0.093679142          -6.512950687            " << npart << endl;
      ofs << " ============================================" << endl;
      ofs << "     1     21         11        0        3        4        0.000000        0.000000      -18.000000       18.000000        0.000510        0.000000        0.000000        0.000000" << endl;
      ofs << "     2     21       2212        0        5        0       -0.000000        0.000000      275.000000      275.001601        0.938270        0.000000        0.000000        0.000000" << endl;
      ofs << "     3     21         11        1        0        0       -0.963526        0.472906      -15.976186       16.012200        0.000510        0.000000        0.000000        0.000000" << endl;
      ofs << "     4     21         22        1        0        0        0.963526       -0.472906       -2.023814        1.987800       -1.138638        0.000000        0.000000        0.000000" << endl;
      ofs << "     5     21       2212        2        0        0       -0.000000       -0.000000      275.000000      275.001601        0.938270        0.000000        0.000000        0.000000" << endl;
      ofs << "     6     21         -2        4        0        0        0.575223       -0.495057       -1.333090        1.533980       -0.000000        0.000000        0.000000        0.000000" << endl;
      ofs << "     7     21          2        5        0        0        0.104280       -0.005342      172.232299      172.232331       -0.000000        0.000000        0.000000        0.000000" << endl;
      ofs << "     8     21         21        6        0        0       -0.493961       -0.724549        3.813575        3.913096        0.000000        0.000000        0.000000        0.000000" << endl;
      ofs << "     9     21          2        7        0        0        0.091399       -0.004682      150.957731      150.957759        0.000000        0.000000        0.000000        0.000000" << endl;
      ofs << "    10     21         21        0        0        0        1.874941       -0.450971       23.470606       23.549694        0.000000        0.000000        0.000000        0.000000" << endl;
      ofs << "    11     21          2        0        0        0       -2.277502       -0.278261      131.300700      131.321161        0.330000        0.000000        0.000000        0.000000" << endl;
      ofs << "    12     11        213        4       25       26        0.233938       -0.337111       -0.113230        0.880819        0.771135        0.000000        0.000000        0.000000" << endl;
      ofs << "    13      1         11        3        0        0       -0.963526        0.472906      -15.976186       16.012200        0.000510        0.000000        0.000000        0.000000" << endl;

      /* Add additional particles */
      for ( unsigned I = I0; I <= npart; I++ )
	{
	  /* Create random particle */
	  double mass = 0.000510;
	  double ptotal = (rand() % 4000 + 1) / 100.; // random from 1 to 40
	  double eta = (rand() % 900) / 100. - 4.5;
	  double phi = (rand() % 100) / 100. * 2 * TMath::Pi();

	  //      cout << eta << " " << ptotal << endl;

	  double pt = ptotal / cosh(eta);

	  /* Create Lorentz vector */
	  TLorentzVector v1;
	  v1.SetPtEtaPhiE(pt, eta, phi, ptotal);

	  /* Pythia line format:
	   * I  K(I,1)  K(I,2)  K(I,3)  K(I,4)  K(I,5)  P(I,1)  P(I,2)  P(I,3)  P(I,4)  P(I,5)  V(I,1)  V(I,2)  V(I,3)
	   *
	   * Scattered electron line example:
	   *     13      1         11        3        0        0       -0.963526        0.472906      -15.976186       16.012200        0.000510        0.000000        0.000000        0.000000
	   */

	  /* New particle parameters for output */
	  unsigned status = 1;
	  int pid = 11;
	  unsigned parentI = 0;
	  unsigned daughterfirstI = 0;
	  unsigned daughterlastI = 0;
	  double px = v1.Px();
	  double py = v1.Py();
	  double pz = v1.Pz();
	  double E  = v1.E();
	  double m  = mass;
	  double vx = 0.0;
	  double vy = 0.0;
	  double vz = 0.0;

	  ofs << "\t" << I
	      << "\t" << status << "\t" << pid << "\t" << parentI << "\t" << daughterfirstI << "\t" << daughterlastI << "\t"
	      << px << "\t" << py << "\t" << pz << "\t" << E << "\t" << m
	      << "\t" << vx << "\t" << vy << "\t" << vz << endl;
	}

      /* Add Pythia style end-of-event record (footer) */
      ofs << " =============== Event finished ===============" << endl;

    } // end event loop

  /* Close file */
  ofs.close();
  return 0;
}
