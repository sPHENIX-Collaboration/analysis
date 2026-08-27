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

constexpr bool verbose = false;

//Cut values we want to probe
constexpr float min_DIRA = 0.99;
constexpr float min_IP_xy = 0.05;
constexpr float min_decayLength = 0.05; //Is this a bias or a cut...

//Other cuts, to enforce on data that may not have this enforced
constexpr int min_MVTX_nHits = 1;
constexpr int min_INTT_nHits = 1;
constexpr int min_TPC_nHits = 20;
constexpr float max_pv_quality = 20.;
constexpr float max_track_1_track_2_DCA = 0.5;

vector<float> v_tpVal_TAU;
vector<float> v_tpVal_ACCEPT;
vector<float> v_tpVal_FD;

void reset()
{
  v_tpVal_TAU.clear();
  v_tpVal_ACCEPT.clear();
  v_tpVal_FD.clear();
}

void printAll(KFParticle& kfp)
{
  kfpTupleTools.identify(kfp);
  cout << "parameters: ";
  for(int i=0;i<8;i++) cout << kfp.GetParameter(i) << ", ";
  cout << endl;
  cout << "covariance matrix: ";
  for(int i=0;i<36;i++) cout << kfp.GetCovariance(i) << ", ";
  cout << endl;
  cout << "mass: ";
  float m,dm;
  kfp.GetMass(m,dm);
  cout << m << " +- " << dm << endl;
  cout << endl;
}

KFParticle makeParticle(float pos[3], float mom[3], float cov[21], char charge, float mass, float chi2, float ndof)
{
  KFParticle kfp_particle;

  kfp_particle.SetConstructMethod(2);

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

  if(verbose)
  {
    cout << "orig PV:" << endl;
    printAll(PV);
  }

  KFParticle moved_PV = PV;
  moved_PV.X() = new_pv[0];
  moved_PV.Y() = new_pv[1];
  moved_PV.Z() = new_pv[2];

  if(verbose)
  {
    cout << "new PV:" << endl;
    printAll(moved_PV);
  }

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
    cout << "New decay length is " << decayLength << " +- " << decayLengthErr << endl;
    cout << "New decay time is " << decayTime << " +- " << decayTimeErr << endl;
  }

  //Could probably do some push back into a vector then use std::all_of to check if they're all true

  bool acceptDIRA = DIRA >= min_DIRA ? true : false;
  if(verbose && !acceptDIRA) std::cout << "failed DIRA cut" << std::endl;
  bool acceptIPXY = min(abs(track_1_IP_XY), abs(track_2_IP_XY)) >= min_IP_xy ? true : false;
  if(verbose && !acceptIPXY) std::cout << "failed IPxy cut" << std::endl;
  bool acceptDecayLength = decayLength >= min_decayLength ? true : false;
  if(verbose && !acceptDecayLength) std::cout << "failed decay length cut" << std::endl;

  return acceptDIRA && acceptIPXY && acceptDecayLength;
}

std::array<float,3> movePV_alongMotherP(KFParticle PV, KFParticle mother, float displace_dist)
{
  float mother_p = sqrt(pow(mother.GetPx(),2)+pow(mother.GetPy(),2)+pow(mother.GetPz(),2));
  std::array<float,3> mother_p_unit = {mother.GetPx()/mother_p, mother.GetPy()/mother_p, mother.GetPz()/mother_p};

  std::array<float,3> new_pv;
  std::array<float,3> original_pv = {PV.GetX(), PV.GetY(), PV.GetZ()};
  for (int k = 0; k < 3; ++k) new_pv[k] = original_pv[k] + mother_p_unit[k]*displace_dist;

  return new_pv;
}

std::array<float,3> movePV_alongPVSVLine(KFParticle PV, KFParticle mother, float displace_dist)
{
  float pv_sv_dist = sqrt(pow(mother.GetX()-PV.GetX(),2)+pow(mother.GetY()-PV.GetY(),2)+pow(mother.GetZ()-PV.GetZ(),2));
  std::array<float,3> pv_sv_unit = { (mother.GetX()-PV.GetX())/pv_sv_dist, (mother.GetY()-PV.GetY())/pv_sv_dist, (mother.GetZ()-PV.GetZ())/pv_sv_dist };

  std::array<float,3> new_pv;
  std::array<float,3> original_pv = {PV.GetX(), PV.GetY(), PV.GetZ()};
  for (int k = 0; k < 3; ++k) new_pv[k] = original_pv[k] + pv_sv_unit[k]*displace_dist;

  return new_pv;
}

// Moves vertex from (PV position + displace_low) to (PV position + displace_high) in increments of displace_step
// Returns range of displacement values between which there are turning points
std::vector<std::pair<float,float>> swimPV(KFParticle mother, std::vector<KFParticle> tracks, KFParticle PV,
                                           float displace_close, float displace_far, float displace_step)
{
  std::vector<std::pair<float,float>> tp_PV_pos;

  int nSteps = ceil((displace_close-displace_far)/displace_step);

  bool lastWasAccepted = false;

  for(int i=0; i<=nSteps; i++)
  {
    if(verbose) cout << "\nSwimming step " << i << endl;

    float displace_dist = displace_close - i*displace_step;

    std::array<float,3> new_pv_motherP = movePV_alongMotherP(PV,mother,displace_dist);
    std::array<float,3> new_pv_svline = movePV_alongPVSVLine(PV,mother,displace_dist);

    if(verbose)
    {
      cout << "PV moved along mother p vector is now at (" << new_pv_motherP[0] << ", " << new_pv_motherP[1] << ", " << new_pv_motherP[2] << ")" << endl;
      cout << "PV moved along PV->SV vector is now at (" << new_pv_svline[0] << ", " << new_pv_svline[1] << ", " << new_pv_svline[2] << ")" << endl;
      cout << "distance between two swum locations: " << sqrt(pow(new_pv_motherP[0]-new_pv_svline[0],2)+pow(new_pv_motherP[1]-new_pv_svline[1],2)+pow(new_pv_motherP[2]-new_pv_svline[2],2)) << endl;
    }
      
    bool didWeAccept = calculateValues(mother, tracks[0], tracks[1], PV, new_pv_motherP);
    string result = didWeAccept ? "We would accept the candidate at this PV" : "We would reject the candidate at this PV";
    if(i>0)
    {
      if(didWeAccept != lastWasAccepted)
      {
        if(verbose)
        {
          cout << "Turning point found:" << std::endl;
          cout << "step " << i-1 << " -> " << i << std::endl;
          cout << "displacement distance " << displace_close - (i-1)*displace_step << " -> " << displace_dist << endl;
        }
        tp_PV_pos.push_back(std::make_pair(displace_close-(i-1)*displace_step,displace_dist));
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
  KFParticle::SetField(14.);// Mag field in kiloGuass (is this negative, should print out the number in KFParticle...)

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
  unsigned int track_1_MVTX_nHits;
  unsigned int track_1_INTT_nHits;
  unsigned int track_1_TPC_nHits;
  char track_1_charge;
  float track_1_mass;

  float track_2_pos[3], track_2_mom[3], track_2_cov[21], track_2_chi2;
  unsigned int track_2_ndof;
  unsigned int track_2_MVTX_nHits;
  unsigned int track_2_INTT_nHits;
  unsigned int track_2_TPC_nHits;
  char track_2_charge;
  float track_2_mass;

  float track_1_track_2_DCA;

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
  outTree->SetBranchAddress("track_1_MVTX_nHits", &track_1_MVTX_nHits);
  outTree->SetBranchAddress("track_1_INTT_nHits", &track_1_INTT_nHits);
  outTree->SetBranchAddress("track_1_TPC_nHits", &track_1_TPC_nHits);

  outTree->SetBranchAddress("track_2_Covariance", &track_2_cov);
  outTree->SetBranchAddress("track_2_charge", &track_2_charge);
  outTree->SetBranchAddress("track_2_mass", &track_2_mass);
  outTree->SetBranchAddress("track_2_chi2", &track_2_chi2);
  outTree->SetBranchAddress("track_2_nDoF", &track_2_ndof);
  outTree->SetBranchAddress("track_2_MVTX_nHits", &track_2_MVTX_nHits);
  outTree->SetBranchAddress("track_2_INTT_nHits", &track_2_INTT_nHits);
  outTree->SetBranchAddress("track_2_TPC_nHits", &track_2_TPC_nHits);

  outTree->SetBranchAddress("primary_vertex_Covariance", &pv_cov);
  outTree->SetBranchAddress("primary_vertex_chi2", &pv_chi2);
  outTree->SetBranchAddress("primary_vertex_nDoF", &pv_ndof);

  outTree->SetBranchAddress("track_1_track_2_DCA", &track_1_track_2_DCA);

  //This is just for checking values
  float original_DIRA; outTree->SetBranchAddress(TString(motherName) + "_DIRA", &original_DIRA);
  float original_decayLength; outTree->SetBranchAddress(TString(motherName) + "_decayLength", &original_decayLength);
  float original_decayTime; outTree->SetBranchAddress(TString(motherName) + "_decayTime", &original_decayTime);
  float original_track_1_IP_xy; outTree->SetBranchAddress("track_1_IP_xy", &original_track_1_IP_xy);
  float original_track_2_IP_xy; outTree->SetBranchAddress("track_2_IP_xy", &original_track_2_IP_xy);

  TBranch *turningPoints_TAU = outTree->Branch("turningPoints_TAU", &v_tpVal_TAU);
  TBranch *turningPoints_ACCEPT = outTree->Branch("turningPoints_ACCEPT",&v_tpVal_ACCEPT);
  TBranch *turningPoints_FD  = outTree->Branch("turningPoints_FD",   &v_tpVal_FD);

  int nEntries = outTree->GetEntries();
  for (int i = 0; i < nEntries; ++i)
  {
    reset();

    if(i % 1000 == 0) cout << "Candidate " << i << endl;
    outTree->GetEntry(i);

    // enforce existing cuts
    if(original_DIRA<min_DIRA || 
       fabs(original_track_1_IP_xy)<min_IP_xy || fabs(original_track_2_IP_xy)<min_IP_xy || 
       original_decayLength<min_decayLength ||
       track_1_MVTX_nHits<min_MVTX_nHits || track_2_MVTX_nHits<min_MVTX_nHits ||
       track_1_INTT_nHits<min_INTT_nHits || track_2_INTT_nHits<min_INTT_nHits ||
       track_1_TPC_nHits<min_TPC_nHits || track_2_TPC_nHits<min_TPC_nHits ||
       pv_chi2/pv_ndof>max_pv_quality ||
       track_1_track_2_DCA>max_track_1_track_2_DCA) continue;

    KFParticle mother = makeParticle(mother_pos, mother_mom, mother_cov, mother_charge, mother_mass, mother_chi2, mother_ndof);
    KFParticle PV = makeVertex(pv_pos, pv_cov, pv_chi2, pv_ndof);
    std::vector<KFParticle> tracks;
    tracks.push_back(makeParticle(track_1_pos, track_1_mom, track_1_cov, track_1_charge, track_1_mass, track_1_chi2, track_1_ndof));
    tracks.push_back(makeParticle(track_2_pos, track_2_mom, track_2_cov, track_2_charge, track_2_mass, track_2_chi2, track_2_ndof));

    KFParticle original_mother = mother;
    if(verbose)
    {
      cout << "init mother" << endl;
      printAll(original_mother);
    }

    original_mother.SetProductionVertex(PV);
    if(verbose)
    {
      cout << "after SetProductionVertex" << endl;
      printAll(original_mother);
    }
    original_mother.TransportToDecayVertex();
    if(verbose)
    {
      cout << "after TransportToDecayVertex" << endl;
      printAll(original_mother);
    }

    float original_ctau, original_ctau_err;
    original_mother.GetLifeTime(original_ctau,original_ctau_err);
    // malformed covariance matrix sets ctau to 0 and sets ctau error to 1e20
    // we don't want to use those candidates
    if(verbose)
    {
      cout << "ctau " << original_ctau << " err " << original_ctau_err << endl;
    }
/*
    bool nan_detected = false;

    for(int ip=0; ip<3; ip++)
    {
      if(std::isnan(mother_pos[ip]) || std::isnan(mother_mom[ip]) || std::isnan(track_1_pos[ip]) || std::isnan(track_2_pos[ip]) ||
         std::isnan(track_1_mom[ip]) || std::isnan(track_2_mom[ip]) || std::isnan(pv_pos[ip]))
      {
        nan_detected = true;
      }
    }

    for(int ic=0;ic<21; ic++)
    {
      if(std::isnan(mother_cov[ic]) || std::isnan(track_1_cov[ic]) || std::isnan(track_2_cov[ic]))
      {
        nan_detected = true;
      }
    }

    for(int iv=0;iv<6;iv++)
    {
      if(std::isnan(pv_cov[iv])) nan_detected = true;
    }

    if(std::isnan(mother_charge) || std::isnan(mother_mass) || std::isnan(mother_chi2) || std::isnan(mother_ndof) ||
       std::isnan(track_1_charge) || std::isnan(track_1_mass) || std::isnan(track_1_chi2) || std::isnan(track_1_ndof) ||
       std::isnan(track_2_charge) || std::isnan(track_2_mass) || std::isnan(track_2_chi2) || std::isnan(track_2_ndof) ||
       std::isnan(pv_chi2) || std::isnan(pv_ndof))
    {
      nan_detected = true;
    }

    if(nan_detected)
    {
      cout << "NaN detected, continuing" << std::endl;
      continue;
    }
*/
    if(verbose)
    {

      cout << "PV information" << endl; printAll(PV);
      cout << "Mother information" << endl; printAll(mother);
      cout << "Track 1 information" << endl; printAll(tracks[0]);
      cout << "Track 2 information" << endl; printAll(tracks[1]);

      cout << "Beginning coarse swimming" << endl;
    }

    float original_decaylengthXY, original_decaylengthXYerr;
    float original_decaylength3D, original_decaylength3Derr;
    float original_pt, original_pterr;
    int retval_dxy = original_mother.GetDecayLengthXY(original_decaylengthXY,original_decaylengthXYerr);
    int retval_d3d = original_mother.GetDecayLength(original_decaylength3D,original_decaylength3Derr);

    float mother_pt, mother_pterr;
    original_mother.GetPt(mother_pt,mother_pterr);
    //std::cout << "mother pt: " << mother_pt << " +- " << mother_pterr << std::endl;

    // swim so that 2D decay length goes down to zero and up to 10cm
    // we swim along the mother momentum vector, so the distance is affected by pz
    // ds^2 = dR^2 + dz^2
    // dz/dR = pz/pt, so ds^2 = dR^2(1+pz^2/pt^2)
    if(verbose) std::cout << "original decaylengthXY: " << original_decaylengthXY << std::endl;
    float coarse_swimdistance_far = -(50.-original_decaylengthXY)*sqrt(1.+pow(mother.GetPz()/mother_pt,2.));
    float coarse_swimdistance_close = original_decaylength3D + 2.*original_decaylength3Derr;
    if(verbose)
    {
      std::cout << "close_displacement: " << coarse_swimdistance_close << std::endl;
      std::cout << "far_displacement: " << coarse_swimdistance_far << std::endl;
    }

    std::vector<std::pair<float,float>> turning_points = swimPV(mother,tracks,PV,coarse_swimdistance_close,coarse_swimdistance_far,large_step_size);
    if(verbose) cout << "Beginning fine swimming" << endl;
    std::vector<float> turning_points_final;
    for(std::pair<float,float> tp : turning_points)
    {
      std::vector<std::pair<float,float>> fine_turning_points = swimPV(mother,tracks,PV,tp.first,tp.second,small_step_size);
      for(std::pair<float,float> fine_tp : fine_turning_points)
      {
        turning_points_final.push_back(fine_tp.first);
        turning_points_final.push_back(fine_tp.second);
      }
    }
    if(verbose) cout << "found " << turning_points_final.size() << " turning points" << std::endl;
    for(float disp : turning_points_final)
    {
      std::array<float,3> new_pv = movePV_alongMotherP(PV,mother,disp);
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
      float DIRA = kfpTupleTools.eventDIRA(mother, moved_PV, true);
      float track_1_IP_XY = tracks[0].GetDistanceFromVertexXY(moved_PV);
      float track_2_IP_XY = tracks[1].GetDistanceFromVertexXY(moved_PV);
      const float speed = 2.99792458e-2;
      decayTime /= speed;

      bool acceptDIRA = DIRA >= min_DIRA;
      bool acceptIPXY = min(abs(track_1_IP_XY), abs(track_2_IP_XY)) >= min_IP_xy;
      bool acceptDecayLength = decayLength >= min_decayLength;

      if(verbose) std::cout << "turning point decay time " << decayTime << " decay length " << decayLength << " accept " << ((acceptDIRA && acceptIPXY && acceptDecayLength)? 1:0) << std::endl;

      float signDIRA = DIRA > 0. ? 1 : -1;

      v_tpVal_TAU.push_back(decayTime*signDIRA);
      v_tpVal_ACCEPT.push_back((acceptDIRA && acceptIPXY && acceptDecayLength)? 1:0);
      v_tpVal_FD.push_back(decayLength*signDIRA);
     
    }

    turningPoints_TAU->Fill();
    turningPoints_ACCEPT->Fill();
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
