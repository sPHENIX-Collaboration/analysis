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

  // Create barrel electromagnetic calorimeter with sigma(E) = 20% * sqrt(E)
  // Genre == 1 (third argument) means only photons and electrons are smeared
  Smear::Device cemc(Smear::kE,  "0.2*sqrt(E)", 1);

  // Create hadron-going EMCal
  Smear::Device femc(Smear::kE,  "0.2*sqrt(E)", 1);

  // Create electron-going EMCal
  Smear::Device eemc(Smear::kE,  "0.02*sqrt(E)", 1);


  // Create our tracking capabilities, by a combination of mometum, theta and phi Devices.

  // Create a smearer for momentum - central arm
  Smear::Device c_momentum(Smear::kP, "0.01 * P");
  Smear::Device c_theta(Smear::kTheta, "0.05 * P");
  Smear::Device c_phi(Smear::kPhi, "0"); // "0" indicates perfect performance i.e. sigma(phi) = 0

  // Create a smearer for momentum - hadron going direction
  Smear::Device h_momentum(Smear::kP, "0.01 * P");
  Smear::Device h_theta(Smear::kTheta, "0.05 * P");
  Smear::Device h_phi(Smear::kPhi, "0"); // "0" indicates perfect performance i.e. sigma(phi) = 0

  // Create a smearer for momentum - electron going direction
  Smear::Device e_momentum(Smear::kP, "0.01 * P");
  Smear::Device e_theta(Smear::kTheta, "0.05 * P");
  Smear::Device e_phi(Smear::kPhi, "0"); // "0" indicates perfect performance i.e. sigma(phi) = 0

  // Create a based on Hermes RICH.
  //  Smear::ParticleID rich("PIDMatrix.dat");

  // central region
  float thetamax_central = eta2theta( -1.1 );
  float thetamin_central = eta2theta(  1.1 );
  cout << "Theta acceptance central from " << thetamin_central << " to " << thetamax_central << endl;
  Smear::Acceptance::Zone central( thetamin_central, thetamax_central );

  // hadron-going region
  float thetamax_hforward = eta2theta(  1.3 );
  float thetamin_hforward = eta2theta(  4.0 );
  cout << "Theta acceptance hforward from " << thetamin_hforward << " to " << thetamax_hforward << endl;
  Smear::Acceptance::Zone hforward( thetamin_hforward, thetamax_hforward );

  // electron-going region
  float thetamax_eforward = eta2theta( -4.0 );
  float thetamin_eforward = eta2theta( -1.2 );
  cout << "Theta acceptance eforward from " << thetamin_eforward << " to " << thetamax_eforward << endl;
  Smear::Acceptance::Zone eforward( thetamin_eforward, thetamax_eforward );

  // set acceptances for detectors
  cemc.Accept.AddZone(central);
  c_momentum.Accept.AddZone(central);
  c_theta.Accept.AddZone(central);
  c_phi.Accept.AddZone(central);
  //  rich.Accept.AddZone(central);

  femc.Accept.AddZone(hforward);
  h_momentum.Accept.AddZone(hforward);
  h_theta.Accept.AddZone(hforward);
  h_phi.Accept.AddZone(hforward);

  eemc.Accept.AddZone(eforward);
  e_momentum.Accept.AddZone(eforward);
  e_theta.Accept.AddZone(eforward);
  e_phi.Accept.AddZone(eforward);

  // Create a detector and add the devices
  Smear::Detector det;

  det.AddDevice(cemc);
  det.AddDevice(c_momentum);
  det.AddDevice(c_theta);
  det.AddDevice(c_phi);

  //  det.AddDevice(rich);

  det.AddDevice(femc);
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
