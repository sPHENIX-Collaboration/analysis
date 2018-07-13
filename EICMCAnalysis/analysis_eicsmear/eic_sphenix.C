/**
   See link for documentation:

   https://wiki.bnl.gov/eic/index.php/Smearing
*/

float eta2theta( double eta )
{
  float theta = 2.0 * atan( exp( -1 * eta ) );
  return theta;
}

Smear::Detector BuildEicSphenix() {

  gSystem->Load("libeicsmear");

  /* Create calorimeters.
   * Genre == 1 (third argument) means only photons and electrons are smeared */

  /* Create electron-going EMCal */
  Smear::Device eemc(Smear::kE,  "0.02*sqrt(E)", 1);

  /* Create barrel electromagnetic calorimeter with sigma(E) = 20% * sqrt(E) */
  Smear::Device cemc(Smear::kE,  "0.2*sqrt(E)", 1);

  /* Create barrel HCAL (inner+outer) */
  Smear::Device chcal(Smear::kE,  "1.0*sqrt(E)", 1);

  /* Create hadron-going EMCal */
  Smear::Device femc(Smear::kE,  "0.2*sqrt(E)", 1);

  /* Create hadron-going HCal */
  Smear::Device fhcal(Smear::kE,  "1.0*sqrt(E)", 1);


  /* Create our tracking capabilities, by a combination of mometum, theta and phi Devices. */

  /* Create a smearer for momentum - central arm */
  Smear::Device c_momentum(Smear::kP, "0.01 * P");
  Smear::Device c_theta(Smear::kTheta, "0.05 * P");
  Smear::Device c_phi(Smear::kPhi, "0"); // "0" indicates perfect performance i.e. sigma(phi) = 0

  /* Create a smearer for momentum - hadron going direction */
  Smear::Device h_momentum(Smear::kP, "0.01 * P");
  Smear::Device h_theta(Smear::kTheta, "0.05 * P");
  Smear::Device h_phi(Smear::kPhi, "0"); // "0" indicates perfect performance i.e. sigma(phi) = 0

  /* Create a smearer for momentum - electron going direction */
  Smear::Device e_momentum(Smear::kP, "0.01 * P");
  Smear::Device e_theta(Smear::kTheta, "0.05 * P");
  Smear::Device e_phi(Smear::kPhi, "0"); // "0" indicates perfect performance i.e. sigma(phi) = 0

  // Create a based on Hermes RICH.
  //  Smear::ParticleID rich("PIDMatrix.dat");

  // central region
  float thetamax_central = eta2theta( -1.1 );
  float thetamin_central = eta2theta(  1.1 );
  cout << "Theta acceptance central from " << thetamin_central << " to " << thetamax_central << endl;
  Smear::Acceptance::Zone zone_central( thetamin_central, thetamax_central );

  float thetamax_central_cemc = eta2theta( -1.55 );
  float thetamin_central_cemc = eta2theta(  1.242 );
  cout << "Theta acceptance CEMC central from " << thetamin_central_cemc << " to " << thetamax_central_cemc << endl;
  Smear::Acceptance::Zone zone_cemc( thetamin_central_cemc, thetamax_central_cemc );

  // hadron-going region
  float thetamax_hside = eta2theta(  1.1 );
  float thetamin_hside = eta2theta(  4.0 );
  cout << "Theta acceptance hside from " << thetamin_hside << " to " << thetamax_hside << endl;
  Smear::Acceptance::Zone zone_hside( thetamin_hside, thetamax_hside );

  float thetamax_femc = eta2theta(  1.242 );
  float thetamin_femc = eta2theta(  4.0 );
  cout << "Theta acceptance femc from " << thetamin_femc << " to " << thetamax_femc << endl;
  Smear::Acceptance::Zone zone_femc( thetamin_femc, thetamax_femc );

  // electron-going region
  float thetamax_eside = eta2theta(  -4.0 );
  float thetamin_eside = eta2theta(  -1.1 );
  cout << "Theta acceptance eside from " << thetamin_eside << " to " << thetamax_eside << endl;
  Smear::Acceptance::Zone zone_eside( thetamin_eside, thetamax_eside );

  float thetamax_eemc = eta2theta( -4.0 );
  float thetamin_eemc = eta2theta( -1.55 );
  cout << "Theta acceptance eemc from " << thetamin_eemc << " to " << thetamax_eemc << endl;
  Smear::Acceptance::Zone zone_eemc( thetamin_eemc, thetamax_eemc );

  // set acceptances for detectors
  cemc.Accept.AddZone(zone_cemc);
  chcal.Accept.AddZone(zone_central);
  c_momentum.Accept.AddZone(zone_central);
  c_theta.Accept.AddZone(zone_central);
  c_phi.Accept.AddZone(zone_central);
  //  rich.Accept.AddZone(central);

  femc.Accept.AddZone(zone_femc);
  fhcal.Accept.AddZone(zone_femc);
  h_momentum.Accept.AddZone(zone_hside);
  h_theta.Accept.AddZone(zone_hside);
  h_phi.Accept.AddZone(zone_hside);

  eemc.Accept.AddZone(zone_eemc);
  e_momentum.Accept.AddZone(zone_eside);
  e_theta.Accept.AddZone(zone_eside);
  e_phi.Accept.AddZone(zone_eside);

  // Create a detector and add the devices
  Smear::Detector det;

  det.AddDevice(cemc);
  det.AddDevice(chcal);
  det.AddDevice(c_momentum);
  det.AddDevice(c_theta);
  det.AddDevice(c_phi);

  //  det.AddDevice(rich);

  det.AddDevice(femc);
  det.AddDevice(fhcal);
  det.AddDevice(h_momentum);
  det.AddDevice(h_theta);
  det.AddDevice(h_phi);

  det.AddDevice(eemc);
  det.AddDevice(e_momentum);
  det.AddDevice(e_theta);
  det.AddDevice(e_phi);

  det.SetEventKinematicsCalculator("NM JB DA"); // The detector will calculate event kinematics from smeared values

  return det;
}
