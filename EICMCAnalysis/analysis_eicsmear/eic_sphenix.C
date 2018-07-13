/**
   See link for documentation:

   https://wiki.bnl.gov/eic/index.php/Smearing
*/

/** Convert pseudorapidity eta to polar angle theta */
float eta2theta( double eta )
{
  float theta = 2.0 * atan( exp( -1 * eta ) );
  return theta;
}

/** Build EIC detector */
Smear::Detector BuildEicSphenix() {

  gSystem->Load("libeicsmear");

  /* CALORIMETERS
   *
   * Calorimeter resolution usually given as sigma_E/E = const% + stocastic%/Sqrt{E}
   * EIC Smear needs absolute sigma: sigma_E = Sqrt{const*const*E*E + stoc*stoc*E}
   *
   * Genre == 1 (third argument) means only photons and electrons are smeared
   */

  /* Create EM calorimeters. */
  /* Create "electron-going" EMCal (PbWO4)*/
  Smear::Device eemc(Smear::kE,  "sqrt(0.01*0.01*E*E + 0.025*0.025*E)");

  /* Create "central rapidity" electromagnetic calorimeter (W-SciFi) */
  Smear::Device cemc(Smear::kE,  "sqrt(0.05*0.05*E*E + 0.16*0.16*E)");

  /* Create "hadron-going" (forward) EMCal (PbScint) */
  Smear::Device femc(Smear::kE,  "sqrt(0.02*0.02*E*E + 0.08*0.08*E)");


  /* Create hadron calorimeters. */
  /* Create "central rapidity" HCAL (inner+outer) (Fe Scint + Steel Scint) */
  Smear::Device chcal(Smear::kE,  "sqrt(0.12*0.12*E*E + 0.81*0.81*E)");

  /* Create "hadron-going" (forward) HCal (Fe Scint) */
  Smear::Device fhcal(Smear::kE,  "sqrt(0.0*0.0*E*E + 0.70*0.70*E)");

  /* Create Forward/Backward Hadron Calorimeter to Measure Charged Hadrons after the Tracker */
  Smear::Device chcalTrack(Smear::kE, "sqrt(0.12*0.12*E*E + 0.81*0.81*E)");
  Smear::Device fhcalTrack(Smear::kE, "sqrt(0.00*0.00*E*E + 0.70*0.70*E)");


  /* Create our tracking capabilities, by a combination of mometum, theta and phi Devices.
   * The momentum parametrization (a*p + b) gives sigma_P/P in percent.
   * So Multiply through by P and divide by 100 to get absolute sigma_P
   * Theta and Phi parametrizations give absolute sigma in miliradians
   */
  /* Create tracking systems.
   * @TODO Below numbers are for tracking with BeAST. We need to get the parametrization for EIC-sPHENIX!!!
   */
  Smear::Device trackMomentum(Smear::kP, "(P*P*(0.0182031 + 0.00921047*pow((-log(tan(theta/2.0))), 2) - 0.00291243*pow((-log(tan(theta/2.0))), 4) + 0.000264353*pow((-log(tan(theta/2.0))), 6)) + P*(0.209681 + 0.275144*pow((-log(tan(theta/2.0))), 2) - 0.0436536*pow((-log(tan(theta/2.0))), 4) + 0.00367412*pow((-log(tan(theta/2.0))), 6)))*0.01");
  Smear::Device trackTheta(Smear::kTheta, "((1.0/(1.0*P))*(0.752935 + 0.280370*pow((-log(tan(theta/2.0))), 2) - 0.0359713*pow((-log(tan(theta/2.0))), 4) + 0.00200623*pow((-log(tan(theta/2.0))), 6)) + 0.0282315 - 0.00998623*pow((-log(tan(theta/2.0))), 2) + 0.00117487*pow((-log(tan(theta/2.0))), 4) - 0.0000443918*pow((-log(tan(theta/2.0))), 6))*0.001");
  Smear::Device trackPhi(Smear::kPhi, "((1.0/(1.0*P))*(0.743977 + 0.753393*pow((-log(tan(theta/2.0))), 2) + 0.0634184*pow((-log(tan(theta/2.0))), 4) + 0.0128001*pow((-log(tan(theta/2.0))), 6)) + 0.0308753 + 0.0480770*pow((-log(tan(theta/2.0))), 2) - 0.0129859*pow((-log(tan(theta/2.0))), 4) + 0.00109374*pow((-log(tan(theta/2.0))), 6))*0.001");


  /* Need these to keep the components below NOT smeared
   */

  /* Momentum for EM
   */
  Smear::Device emTrackMomentum(Smear::kP, "0");
  Smear::Device emTrackTheta(Smear::kTheta, "0");
  Smear::Device emTrackPhi(Smear::kPhi, "0");

  /* Momentum for Neutral Hadrons
   */
  Smear::Device hadTrackMomentum(Smear::kP, "0");
  Smear::Device hadTrackTheta(Smear::kTheta, "0");
  Smear::Device hadTrackPhi(Smear::kPhi, "0");

  /* Momentum for Charged Hadrons After the Tracker
   */
  Smear::Device chcalTrackMomentum(Smear::kP, "0");
  Smear::Device chcalTrackTheta(Smear::kTheta, "0");
  Smear::Device chcalTrackPhi(Smear::kPhi, "0");

  Smear::Device fhcalTrackMomentum(Smear::kP, "0");
  Smear::Device fhcalTrackTheta(Smear::kTheta, "0");
  Smear::Device fhcalTrackPhi(Smear::kPhi, "0");


  /* Energy for Tracks
   */
  Smear::Device trackEnergy(Smear::kE, "0");


  /* Create PID systems.
   */
  /* Create RICH based on Hermes RICH.
   */
  //  Smear::ParticleID rich("PIDMatrix.dat");


   /* Define spatial extensions of devices */

   /* EMCal zones */
   Smear::Acceptance::Zone zone_eemc( eta2theta( -1.55 ), eta2theta( -4.00 ) );
   Smear::Acceptance::Zone zone_cemc( eta2theta(  1.24 ), eta2theta( -1.55 ) );
   Smear::Acceptance::Zone zone_femc( eta2theta(  4.00 ), eta2theta(  1.24 ) );

   Smear::Acceptance::Zone zone_emTotal( eta2theta(  4.00 ), eta2theta(  -4.00 ) );

   /* HCal zones */
   Smear::Acceptance::Zone zone_chcal( eta2theta(  1.10 ), eta2theta( -1.10 ) );
   Smear::Acceptance::Zone zone_fhcal( eta2theta(  4.00 ), eta2theta(  1.24 ) );

   Smear::Acceptance::Zone zone_hadTotal( eta2theta(  4.00 ), eta2theta(  -4.00 ) );

   /* Hcal After Tracker Zone */
   /* @TODO Do we need these? What's the acceptance? */
   Smear::Acceptance::Zone zone_chcalTrack( 0, 0 );
   Smear::Acceptance::Zone zone_fhcalTrack( 0, 0 );


   /* Tracking zones */
   Smear::Acceptance::Zone zone_track( eta2theta(  4.00 ), eta2theta( -4.00 ) );

   /* Set acceptances for detectors
    */
   eemc.Accept.SetGenre(Smear::kElectromagnetic);
   cemc.Accept.SetGenre(Smear::kElectromagnetic);
   femc.Accept.SetGenre(Smear::kElectromagnetic);

   eemc.Accept.AddZone(zone_eemc);
   cemc.Accept.AddZone(zone_cemc);
   femc.Accept.AddZone(zone_femc);

   /* Accept only neutral hadrons in HCAL- use tracking
    * to measure charged hadrons */
   chcal.Accept.AddParticle(2112);
   chcal.Accept.AddParticle(-2112);
   chcal.Accept.AddParticle(130);

   fhcal.Accept.AddParticle(2112);
   fhcal.Accept.AddParticle(-2112);
   fhcal.Accept.AddParticle(130);

   chcal.Accept.AddZone(zone_chcal);
   fhcal.Accept.AddZone(zone_fhcal);

   /* Assign acceptance to calorimeters for charged hadrons past tracker */
   /* @TODO: Do we need these? */
   //chcalTrack.Accept.SetGenre(Smear::kHadronic);
   //fhcalTrack.Accept.SetGenre(Smear::kHadronic);
   //
   //chcalTrack.Accept.SetCharge(Smear::kCharged);
   //fhcalTrack.Accept.SetCharge(Smear::kCharged);
   //
   //chcalTrack.Accept.AddZone(zone_chcalTrack);
   //fhcalTrack.Accept.AddZone(zone_fhcalTrack);

   /* Acceptance for tracker */
   trackMomentum.Accept.SetGenre(Smear::kHadronic);
   trackTheta.Accept.SetGenre(Smear::kHadronic);
   trackPhi.Accept.SetGenre(Smear::kHadronic);

   trackMomentum.Accept.SetCharge(Smear::kCharged);
   trackTheta.Accept.SetCharge(Smear::kCharged);
   trackPhi.Accept.SetCharge(Smear::kCharged);

   trackMomentum.Accept.AddZone(zone_track);
   trackTheta.Accept.AddZone(zone_track);
   trackPhi.Accept.AddZone(zone_track);

   /* Acceptance for "calorimeter momentum" */
   emTrackMomentum.Accept.SetGenre(Smear::kElectromagnetic);
   emTrackTheta.Accept.SetGenre(Smear::kElectromagnetic);
   emTrackPhi.Accept.SetGenre(Smear::kElectromagnetic);

   emTrackMomentum.Accept.AddZone(zone_emTotal);
   emTrackTheta.Accept.AddZone(zone_emTotal);
   emTrackPhi.Accept.AddZone(zone_emTotal);

   hadTrackMomentum.Accept.AddParticle(2112);
   hadTrackTheta.Accept.AddParticle(2112);
   hadTrackPhi.Accept.AddParticle(2112);

   hadTrackMomentum.Accept.AddParticle(-2112);
   hadTrackTheta.Accept.AddParticle(-2112);
   hadTrackPhi.Accept.AddParticle(-2112);

   hadTrackMomentum.Accept.AddParticle(130);
   hadTrackTheta.Accept.AddParticle(130);
   hadTrackPhi.Accept.AddParticle(130);

   hadTrackMomentum.Accept.AddZone(zone_hadTotal);
   hadTrackTheta.Accept.AddZone(zone_hadTotal);
   hadTrackPhi.Accept.AddZone(zone_hadTotal);

   /* Assign acceptance for calorimeter momentum for charged hadrons past tracker */
   chcalTrackMomentum.Accept.SetGenre(Smear::kHadronic);
   chcalTrackTheta.Accept.SetGenre(Smear::kHadronic);
   chcalTrackPhi.Accept.SetGenre(Smear::kHadronic);

   chcalTrackMomentum.Accept.SetCharge(Smear::kCharged);
   chcalTrackTheta.Accept.SetCharge(Smear::kCharged);
   chcalTrackPhi.Accept.SetCharge(Smear::kCharged);

   fhcalTrackMomentum.Accept.SetGenre(Smear::kHadronic);
   fhcalTrackTheta.Accept.SetGenre(Smear::kHadronic);
   fhcalTrackPhi.Accept.SetGenre(Smear::kHadronic);

   fhcalTrackMomentum.Accept.SetCharge(Smear::kCharged);
   fhcalTrackTheta.Accept.SetCharge(Smear::kCharged);
   fhcalTrackPhi.Accept.SetCharge(Smear::kCharged);

   chcalTrackMomentum.Accept.AddZone(zone_chcal);
   chcalTrackTheta.Accept.AddZone(zone_chcal);
   chcalTrackPhi.Accept.AddZone(zone_chcal);

   fhcalTrackMomentum.Accept.AddZone(zone_fhcal);
   fhcalTrackTheta.Accept.AddZone(zone_fhcal);
   fhcalTrackPhi.Accept.AddZone(zone_fhcal);

   /* Acceptance for "track energy" */
   trackEnergy.Accept.SetGenre(Smear::kHadronic);
   trackEnergy.Accept.SetCharge(Smear::kCharged);
   trackEnergy.Accept.AddZone(zone_track);


   /* Create a detector and add the devices
    */
   Smear::Detector det;

   det.AddDevice(eemc);
   det.AddDevice(cemc);
   det.AddDevice(femc);

   det.AddDevice(chcal);
   det.AddDevice(fhcal);

   /* @TODO Do we need these? */
   //det.AddDevice(chcalTrack);
   //det.AddDevice(fhcalTrack);

   det.AddDevice(trackMomentum);
   det.AddDevice(trackTheta);
   det.AddDevice(trackPhi);

   det.AddDevice(emTrackMomentum);
   det.AddDevice(emTrackTheta);
   det.AddDevice(emTrackPhi);
   det.AddDevice(hadTrackMomentum);
   det.AddDevice(hadTrackTheta);
   det.AddDevice(hadTrackPhi);

   det.AddDevice(chcalTrackMomentum);
   det.AddDevice(chcalTrackTheta);
   det.AddDevice(chcalTrackPhi);
   det.AddDevice(fhcalTrackMomentum);
   det.AddDevice(fhcalTrackTheta);
   det.AddDevice(fhcalTrackPhi);

   det.AddDevice(trackEnergy);

   //  det.AddDevice(rich);

   det.SetEventKinematicsCalculator("NM JB DA"); // The detector will calculate event kinematics from smeared values

   return det;
}
