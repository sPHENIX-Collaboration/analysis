#include <TCut.h>
#include <TFile.h>
#include <TMatrixD.h>
#include <TString.h>
#include <TTree.h>

#include <kfparticle_sphenix/KFParticle_sPHENIX.h>
#include <kfparticle_sphenix/KFParticle_Tools.h>

#include <iostream>
#include <limits>

R__LOAD_LIBRARY(libKFParticle.so)

#include <KFParticle.h>

using namespace std;
KFParticle_Tools kfpTupleTools;

bool verbose = false;

//Cut values we want to probe
float min_DIRA = 0.99;
float min_IP_xy = 0.05;
float min_decayLength = 0.05; //Is this a bias or a cut...
vector<float> v_tpVal_TAU;
vector<float> v_tpVal_FD;

void reset()
{
  v_tpVal_TAU.clear();
  v_tpVal_FD.clear();
}

KFParticle makeParticle(float pos[3], float mom[3], float cov[21], char charge, float mass, float chi2, float ndof)
{
  KFParticle kfp_particle;

  float f_trackParameters[6] = {pos[0], pos[1], pos[2], mom[0], mom[1], mom[2]};

  kfp_particle.Create(f_trackParameters, cov, (int) charge, mass);
  kfp_particle.Chi2() = chi2;
  kfp_particle.NDF() = ndof;

  return kfp_particle;
}

KFParticle makeVertex(float pv_pos[3], float pv_cov[6], float pv_chi2, unsigned int pv_ndof)
{
  float f_vertexParameters[6] = {pv_pos[0], pv_pos[1], pv_pos[2], 0, 0, 0};

  float f_vertexCovariance[21] = {0};
  for (int i = 0; i < 6; ++i) f_vertexCovariance[i] = pv_cov[i];

  KFParticle kfp_vertex;
  kfp_vertex.Create(f_vertexParameters, f_vertexCovariance, 0, -1);
  kfp_vertex.Chi2() = pv_chi2;
  kfp_vertex.NDF() = pv_ndof;

  return kfp_vertex;
}

bool calculateValues(KFParticle mother, KFParticle track_1, KFParticle track_2, KFParticle PV, std::array<float,3>& new_pv)
{
  //Note, this is still missing a check as to whether the track would've hit an MVTX sensor or not

  KFParticle moved_PV = PV;
  moved_PV.X() = new_pv[0];
  moved_PV.Y() = new_pv[1];
  moved_PV.Z() = new_pv[2];

  float track_1_IP_XY = track_1.GetDistanceFromVertexXY(moved_PV);
  float track_2_IP_XY = track_2.GetDistanceFromVertexXY(moved_PV);

  float DIRA = kfpTupleTools.eventDIRA(mother, moved_PV, true);

  float decayTime, decayTimeErr, decayLength, decayLengthErr; 

  KFParticle particleCopy = mother;
  particleCopy.SetProductionVertex(moved_PV);
  particleCopy.TransportToDecayVertex();
  particleCopy.GetLifeTime(decayTime, decayTimeErr);
  particleCopy.GetDecayLengthXY(decayLength, decayLengthErr);
  const float speed = 2.99792458e-2;
  decayTime /= speed;

  if(verbose)
  {
    cout << "New DIRA is " << DIRA << endl;
    cout << "New track 1 IP XY is " << track_1_IP_XY << endl;
    cout << "New track 2 IP XY is " << track_2_IP_XY << endl;
    cout << "New decay length is " << decayLength << endl;
    cout << "New decay time is " << decayTime << endl;
  }

  //Could probably do some push back into a vector then use std::all_of to check if they're all true

  bool acceptDIRA = DIRA >= min_DIRA ? true : false;
  bool acceptIPXY = min(abs(track_1_IP_XY), abs(track_2_IP_XY)) >= min_IP_xy ? true : false;
  bool acceptDecayLength = decayLength >= min_decayLength ? true : false;

  // Must decay before it leaves the MVTX; this is a crude check on that quality
  bool acceptnMVTX = (decayLength <= 5.);

  return acceptDIRA && acceptIPXY && acceptDecayLength && acceptnMVTX;
}

std::array<float,3> movePV(KFParticle PV, KFParticle mother, float displace_dist)
{
  float mother_p = sqrt(pow(mother.GetPx(),2)+pow(mother.GetPy(),2)+pow(mother.GetPz(),2));
  std::array<float,3> mother_p_unit = {mother.GetPx()/mother_p, mother.GetPy()/mother_p, mother.GetPz()/mother_p};

  std::array<float,3> new_pv;
  std::array<float,3> original_pv = {PV.GetX(), PV.GetY(), PV.GetZ()};
  for (int k = 0; k < 3; ++k) new_pv[k] = original_pv[k] + mother_p_unit[k]*displace_dist;

  return new_pv;
}

// Moves vertex from (PV position + displace_low) to (PV position + displace_high) in increments of displace_step
// Returns range of displacement values between which there are turning points
std::vector<std::pair<float,float>> swimPV(KFParticle mother, std::vector<KFParticle> tracks, KFParticle PV,
                                           float displace_low, float displace_high, float displace_step)
{
  std::vector<std::pair<float,float>> tp_PV_pos;

  int nSteps = ceil((displace_high-displace_low)/displace_step);

  bool lastWasAccepted = false;

  for(int i=0; i<=nSteps; i++)
  {
    if(verbose) cout << "\nSwimming step " << i << endl;

    float displace_dist = displace_low + i*displace_step;

    std::array<float,3> new_pv = movePV(PV,mother,displace_dist);

    if(verbose) cout << "PV is now at (" << new_pv[0] << ", " << new_pv[1] << ", " << new_pv[2] << ")" << endl;
    bool didWeAccept = calculateValues(mother, tracks[0], tracks[1], PV, new_pv);
    string result = didWeAccept ? "We would accept the candidate at this PV" : "We would reject the candidate at this PV";
    if(i>0)
    {
      if(didWeAccept != lastWasAccepted)
      {
        if(verbose)
        {
          cout << "Turning point found:" << std::endl;
          cout << "step " << i-1 << " -> " << i << std::endl;
          cout << "displacement distance " << displace_low + (i-1)*displace_step << " -> " << displace_dist << endl;
        }
        tp_PV_pos.push_back(std::make_pair(displace_low+(i-1)*displace_step,displace_dist));
      }
    }
    lastWasAccepted = didWeAccept;
    if(verbose) cout << result << endl;
  }

  return tp_PV_pos;
}

//int main(int argc, char* argv[]) 
int swimLF(int nCandidates, std::string fileName, std::string motherName)
{
/*
  if (argc != 3) 
  {
      cerr << "Usage: " << argv[0] << " <input file> <mother name>\n";
      return 1;
  }
*/
  KFParticle::SetField(15.);// Mag field in kiloGuass (is this negative, should print out the number in KFParticle...)

  float large_step_size = 0.5; //Make large steps first to figure out where the turning points roughly are
  float small_step_size = 0.01; //Our PV resolution should be better than 100 microns but maybe not by much. Probably good for a first attempt

  //string fileName = argv[1];
  //string motherName = argv[2];

  TFile* inFile = new TFile(fileName.c_str());
  TTree* inTree = (TTree*) inFile->Get("DecayTree");

  string outputName = fileName.substr(0,fileName.size() - 5);
  outputName += "_swum.root";
  TFile* outFile = new TFile(outputName.c_str(),"RECREATE");

  TCut cut = "";

  TTree* midTree = inTree->CopyTree(cut);
  TTree* outTree = midTree->CloneTree(nCandidates); //copying and cloning the input tree
  //TTree* outTree = midTree->CloneTree(-1); //copying and cloning the input tree

  float mother_pos[3], mother_mom[3], mother_cov[21], mother_chi2;
  unsigned int mother_ndof;
  char mother_charge;
  float mother_mass;

  float track_1_pos[3], track_1_mom[3], track_1_cov[21], track_1_chi2;
  unsigned int track_1_ndof;
  char track_1_charge;
  float track_1_mass;

  float track_2_pos[3], track_2_mom[3], track_2_cov[21], track_2_chi2;
  unsigned int track_2_ndof;
  char track_2_charge;
  float track_2_mass;

  float pv_pos[3], pv_cov[6], pv_chi2;
  unsigned int pv_ndof;

  string xyz[3] = {"x", "y", "z"};
  for (int i = 0; i < 3; ++i)
  {
    outTree->SetBranchAddress(TString(motherName) + "_"  + TString(xyz[i]), &mother_pos[i]);
    outTree->SetBranchAddress(TString(motherName) + "_p" + TString(xyz[i]), &mother_mom[i]);
    outTree->SetBranchAddress("track_1_"  + TString(xyz[i]), &track_1_pos[i]);
    outTree->SetBranchAddress("track_1_p" + TString(xyz[i]), &track_1_mom[i]);
    outTree->SetBranchAddress("track_2_"  + TString(xyz[i]), &track_2_pos[i]);
    outTree->SetBranchAddress("track_2_p" + TString(xyz[i]), &track_2_mom[i]);
    outTree->SetBranchAddress("primary_vertex_" + TString(xyz[i]), &pv_pos[i]);
  }

  outTree->SetBranchAddress(TString(motherName) + "_mass", &mother_mass);
  outTree->SetBranchAddress(TString(motherName) + "_Covariance", &mother_cov);
  outTree->SetBranchAddress(TString(motherName) + "_charge", &mother_charge);
  outTree->SetBranchAddress(TString(motherName) + "_chi2", &mother_chi2);
  outTree->SetBranchAddress(TString(motherName) + "_nDoF", &mother_ndof);

  outTree->SetBranchAddress("track_1_Covariance", &track_1_cov);
  outTree->SetBranchAddress("track_1_charge", &track_1_charge);
  outTree->SetBranchAddress("track_1_mass", &track_1_mass);
  outTree->SetBranchAddress("track_1_chi2", &track_1_chi2);
  outTree->SetBranchAddress("track_1_nDoF", &track_1_ndof);

  outTree->SetBranchAddress("track_2_Covariance", &track_2_cov);
  outTree->SetBranchAddress("track_2_charge", &track_2_charge);
  outTree->SetBranchAddress("track_2_mass", &track_2_mass);
  outTree->SetBranchAddress("track_2_chi2", &track_2_chi2);
  outTree->SetBranchAddress("track_2_nDoF", &track_2_ndof);

  outTree->SetBranchAddress("primary_vertex_Covariance", &pv_cov);
  outTree->SetBranchAddress("primary_vertex_chi2", &pv_chi2);
  outTree->SetBranchAddress("primary_vertex_nDoF", &pv_ndof);

  //This is just for checking values
  float original_DIRA; outTree->SetBranchAddress(TString(motherName) + "_DIRA", &original_DIRA);
  float original_decayLength; outTree->SetBranchAddress(TString(motherName) + "_decayLength", &original_decayLength);
  float original_decayTime; outTree->SetBranchAddress(TString(motherName) + "_decayTime", &original_decayTime);
  float original_track_1_IP_xy; outTree->SetBranchAddress("track_1_IP_xy", &original_track_1_IP_xy);
  float original_track_2_IP_xy; outTree->SetBranchAddress("track_2_IP_xy", &original_track_2_IP_xy);

  TBranch *turningPoints_TAU = outTree->Branch("turningPoints_TAU", &v_tpVal_TAU);
  TBranch *turningPoints_FD  = outTree->Branch("turningPoints_FD",   &v_tpVal_FD);

  int nEntries = outTree->GetEntries();
  for (int i = 0; i < nEntries; ++i)
  {
    reset();

    if(i % 1000 == 0) cout << "Candidate " << i << endl;
    outTree->GetEntry(i);

    KFParticle mother = makeParticle(mother_pos, mother_mom, mother_cov, mother_charge, mother_mass, mother_chi2, mother_ndof);
    KFParticle PV = makeVertex(pv_pos, pv_cov, pv_chi2, pv_ndof);
    std::vector<KFParticle> tracks;
    tracks.push_back(makeParticle(track_1_pos, track_1_mom, track_1_cov, track_1_charge, track_1_mass, track_1_chi2, track_1_ndof));
    tracks.push_back(makeParticle(track_2_pos, track_2_mom, track_2_cov, track_2_charge, track_2_mass, track_2_chi2, track_2_ndof));

    mother.SetProductionVertex(PV);
    mother.TransportToDecayVertex();

    if(verbose)
    {

      cout << "PV information" << endl; kfpTupleTools.identify(PV);
      cout << "Mother information" << endl; kfpTupleTools.identify(mother);
      cout << "Track 1 information" << endl; kfpTupleTools.identify(tracks[0]);
      cout << "Track 2 information" << endl; kfpTupleTools.identify(tracks[1]);

      cout << "Beginning coarse swimming" << endl;
    }

    float original_decaylengthXY, original_decaylengthXYerr;
    float original_pt, original_pterr;
    mother.GetDecayLengthXY(original_decaylengthXY,original_decaylengthXYerr);

    float mother_pt, mother_pterr;
    mother.GetPt(mother_pt,mother_pterr);

    // swim so that 2D decay length goes down to zero and up to 10cm
    // we swim along the mother momentum vector, so the distance is affected by pz
    // ds^2 = dR^2 + dz^2
    // dz/dR = pz/pt, so ds^2 = dR^2(1+pz^2/pt^2)
    if(verbose) std::cout << "original decaylengthXY: " << original_decaylengthXY << std::endl;
    float coarse_swimdistance_min = -(10.-original_decaylengthXY)*sqrt(1.+pow(mother.GetPz()/mother.GetPt(),2.));
    float coarse_swimdistance_max = original_decaylengthXY*sqrt(1.+pow(mother.GetPz()/mother.GetPt(),2.));
    if(verbose)
    {
      std::cout << "min_displacement: " << coarse_swimdistance_min << std::endl;
      std::cout << "max_displacement: " << coarse_swimdistance_max << std::endl;
    }

    std::vector<std::pair<float,float>> turning_points = swimPV(mother,tracks,PV,coarse_swimdistance_min,coarse_swimdistance_max,large_step_size);
    if(verbose) cout << "Beginning fine swimming" << endl;
    std::vector<float> turningpoint_displacement;
    for(std::pair<float,float> tp : turning_points)
    {
      std::vector<std::pair<float,float>> fine_turning_points = swimPV(mother,tracks,PV,tp.first,tp.second,small_step_size);
      for(std::pair<float,float> fine_tp : fine_turning_points)
      {
        turningpoint_displacement.push_back((fine_tp.first+fine_tp.second)/2.);
      }
    }

    for(float disp : turningpoint_displacement)
    {
      std::array<float,3> new_pv = movePV(PV,mother,disp);
      KFParticle moved_PV = PV;
      moved_PV.X() = new_pv[0];
      moved_PV.Y() = new_pv[1];
      moved_PV.Z() = new_pv[2];
      KFParticle particleCopy = mother;
      particleCopy.SetProductionVertex(moved_PV);
      particleCopy.TransportToDecayVertex();
      float decayTime, decayTimeErr, decayLength, decayLengthErr;
      particleCopy.GetLifeTime(decayTime, decayTimeErr);
      particleCopy.GetDecayLengthXY(decayLength, decayLengthErr);
      const float speed = 2.99792458e-2;
      decayTime /= speed;
      if(verbose) std::cout << "turning point decay time " << decayTime << " decay length " << decayLength << std::endl;
      v_tpVal_TAU.push_back(decayTime);
      v_tpVal_FD.push_back(decayLength);
    }

    turningPoints_TAU->Fill();
    turningPoints_FD->Fill();
    

/*
    KFParticle track_1 = makeParticle(track_1_pos, track_1_mom, track_1_cov, track_1_charge, track_1_chi2, track_1_ndof);
    KFParticle track_2 = makeParticle(track_2_pos, track_2_mom, track_2_cov, track_2_charge, track_2_chi2, track_2_ndof);
    KFParticle PV = makeVertex(pv_pos, pv_cov, pv_chi2, pv_ndof);

    cout << "PV information" << endl; kfpTupleTools.identify(PV);
    cout << "Mother information" << endl; kfpTupleTools.identify(mother);
    cout << "Track 1 information" << endl; kfpTupleTools.identify(track_1);
    cout << "Track 2 information" << endl; kfpTupleTools.identify(track_2);

    float total_mother_momentum = sqrt(pow(mother_mom[0], 2) + pow(mother_mom[1], 2) + pow(mother_mom[2], 2));
    float mom_unit_vector[3]; for (int j = 0; j < 3; ++j) mom_unit_vector[j] = mother_mom[j]/total_mother_momentum; //Need a unit vector to swim along

    //Super quick swimming
    //lets just bring us back and forward by 5cm
    float jump_back = 5, jump_forward = jump_back; 
    float start_position[3], end_position[3];
    for (int j = 0; j < 3; ++j)
    { 
      start_position[j] = pv_pos[j] - mom_unit_vector[j]*jump_back;
      end_position[j] = pv_pos[j] + mom_unit_vector[j]*jump_forward;
    }

    //Check some values
    cout << "PV (x, y, z) =  (" << PV.GetX() << ", " << PV.GetY() << ", " << PV.GetZ() << ")" << endl;
    cout << "Start pos (x, y, z) =  (" << start_position[0] << ", " << start_position[1] << ", " << start_position[2] << ")" << endl;
    cout << "End pos (x, y, z) =  (" << end_position[0] << ", " << end_position[1] << ", " << end_position[2] << ")" << endl;
    cout << "PV radius = " << sqrt(pow(PV.GetX(), 2) + pow(PV.GetY(), 2));
    cout << ", Start radius = " << sqrt(pow(start_position[0], 2) + pow(start_position[1], 2));
    cout << ", End radius = " << sqrt(pow(end_position[0], 2) + pow(end_position[1], 2)) << endl;
    cout << "Unit vector value = (" << mom_unit_vector[0] << ", " << mom_unit_vector[1] << ", " << mom_unit_vector[2] << "), size: " << sqrt(pow(mom_unit_vector[0], 2) + pow(mom_unit_vector[1], 2) + pow(mom_unit_vector[2], 2)) << endl;

    //Check original values
    cout << "Original DIRA was " << original_DIRA << endl;
    cout << "Original track 1 IP XY was " << original_track_1_IP_xy << endl;
    cout << "Original track 2 IP XY was " << original_track_2_IP_xy << endl;
    cout << "Original decay length was " << original_decayLength << endl;
    cout << "Original decay time was " << original_decayTime << endl;

    int nBackSteps = (int) sqrt(pow(start_position[0], 2) + pow(start_position[1], 2) + pow(start_position[2], 2))/large_step_size;
    int nForwardSteps = (int) sqrt(pow(end_position[0], 2) + pow(end_position[1], 2) + pow(end_position[2], 2))/large_step_size;

    std::vector<std::pair<int,int>> coarse_tp_index;

    bool lastWasAccepted = false;

    for (int j = -1*nBackSteps; j < nForwardSteps; ++j)
    {
      cout << "\nSwimming step " << j << endl;

      float new_pv[3], original_pv[3] = {PV.GetX(), PV.GetY(), PV.GetZ()};
      for (int k = 0; k < 3; ++k) new_pv[k] = original_pv[k] + j*mom_unit_vector[k]*large_step_size;
      cout << "PV is now at (" << new_pv[0] << ", " << new_pv[1] << ", " << new_pv[2] << ")" << endl;
      bool didWeAccept = calculateValues(mother, track_1, track_2, PV, new_pv);
      string result = didWeAccept ? "We would accept the candidate at this PV" : "We would reject the candidate at this PV";
      if(j == -1*nBackSteps)
      {
        lastWasAccepted = didWeAccept;
      }
      else
      {
        if(didWeAccept != lastWasAccepted)
        {
          cout << "Turning point found:" << std::endl;
          cout << "step " << j-1 << " -> " << j << std::endl;
          coarse_tp_index.push_back(std::make_pair(j-1,j));
        }
      }
      lastWasAccepted = didWeAccept;
      cout << result << endl;

      //Once we find the transition for the acceptance, we need to jump back and do fine grained steps to get a more precise number
      //This value for the decay length and decay time should be stored into the vectors we'll write to the nTuple and sorted into some readable order for turn on and turn off
    }
*/

  }

  outFile->Write();
  outFile->Close();
 
  return 0;
}
