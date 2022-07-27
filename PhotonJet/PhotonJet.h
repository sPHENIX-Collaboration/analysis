#ifndef __PHOTONJET_H__
#define __PHOTONJET_H__

#include <fun4all/SubsysReco.h>
#include <vector>

#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TMath.h>
#include <TTree.h>

class PHCompositeNode;
class RawClusterContainer;
class RawTowerContainer;
class RawCluster;
class SvtxTrackMap;
class JetMap;
class GlobalVertex;
class PHHepMCGenEventMap;
class JetEvalStack;
class JetRecoEval;
class SvtxTrackEval;
class PHG4TruthInfoContainer;
class PHHepMCGenEvent;
class CaloTriggerInfo;
class JetTruthEval;

class PhotonJet : public SubsysReco
{
 public:
  PhotonJet(const std::string &name = "photonjet.root");
  double isoconeradius, mincluspt;
  double minjetpt;
  int jet_cone_size;
  int use_isocone;
  int eval_tracked_jets;
  int etalow;
  int etahigh;
  int usetrigger;
  int use_pos_cor_cemc;
  int is_AA;
  double mindp_pt;

  int Init(PHCompositeNode *);
  int process_event(PHCompositeNode *);
  int End(PHCompositeNode *);

  void use_trigger_emulator(int yesorno) { usetrigger = yesorno; };
  void set_jetpt_mincut(double pt) { minjetpt = pt; };
  void Set_Isocone_radius(double rad) { isoconeradius = rad; };
  void set_cluspt_mincut(double pt) { mincluspt = pt; };
  void set_directphotonpt_mincut(double pt) { mindp_pt = pt; };
  void use_isocone_algorithm(int yes) { use_isocone = yes; };  //1 is use it, 0 is don't use it
  void set_jetcone_size(int size) { jet_cone_size = size; }    //float for jet cone size, i.e. 0.2,0.3,0.4, etc.
  void SetFirstEventNum(int eventnum) { nevents = eventnum; }  //setting the first event based on job number so that each event has an individual eventnum
  void use_tracked_jets(int useit) { eval_tracked_jets = useit; };
  void set_eta_lowhigh(float low, float high)
  {
    etalow = low;
    etahigh = high;
  };
  void use_positioncorrection_CEMC(int useit) { use_pos_cor_cemc = useit; };
  void set_AA_collisions(int isaa) { is_AA = isaa; };
 private:
  TH1 *ntruthconstituents_h;
  TH1 *percent_photon_h;
  TFile *file;
  TTree *tree;
  TTree *cluster_tree;
  TTree *truth_g4particles;
  TTree *truthtree;
  TTree *isolated_clusters;
  TTree *tracktree;
  TTree *truthjettree;
  TTree *recojettree;
  TTree *isophot_jet_tree;
  TTree *isophot_trackjet_tree;
  TTree *isophot_had_tree;
  TTree *event_tree;
  std::string outfilename;

  float mpi;
  int ntruthconstituents;
  int nevents;
  TH1F *histo;

  float beam_energy;
  float x1;
  float x2;
  int partid1;
  int partid2;

  float hardest_jetpt;
  float hardest_jetphi;
  float hardest_jeteta;
  float hardest_jetenergy;

  //cluster info
  float clus_energy;
  float clus_eta;
  float clus_phi;
  float clus_pt;
  float clus_px;
  float clus_py;
  float clus_pz;
  float clus_theta;
  float clus_x, clus_y, clus_z, clus_t;

  //track info
  float tr_px, tr_py, tr_pz;
  float tr_p;
  float tr_pt;
  float tr_phi;
  float tr_eta;
  int charge;
  float chisq;
  unsigned int ndf;
  float dca;
  float tr_x, tr_y, tr_z;
  float truthtrackpx, truthtrackpy, truthtrackpz;
  float truthtrackp;
  float truthtracke;
  float truthtrackpt;
  float truthtrackphi;
  float truthtracketa;
  int truthtrackpid;
  bool truth_is_primary;

  //all truth jets
  float truthjetpt;
  float truthjetpx, truthjetpy, truthjetpz;
  float truthjetphi;
  float truthjeteta;
  float truthjetmass;
  float truthjetp;
  float truthjetenergy;

  float dR;
  //all reco jets
  float recojetpt;
  float recojetpx, recojetpy, recojetpz;
  float recojetphi;
  float recojeteta;
  float recojetmass;
  float recojetp;
  float recojetenergy;
  float recojetid;
  float truthjetid;

  //isophot+reco jet
  float _recojetid;
  float _recojetpt;
  float _recojetpx, _recojetpy, _recojetpz;
  float _recojetphi;
  float _recojeteta;
  float _recojetmass;
  float _recojetp;
  float _recojetenergy;
  float jetdphi;
  float jetpout;
  float jetdeta;
  float _truthjetid;
  float _truthjetpt;
  float _truthjetpx, _truthjetpy, _truthjetpz;
  float _truthjetphi;
  float _truthjeteta;
  float _truthjetmass;
  float _truthjetp;
  float _truthjetenergy;

  //isophot+reco track jet
  float _trecojetid;
  float _trecojetpt;
  float _trecojetpx, _trecojetpy, _trecojetpz;
  float _trecojetphi;
  float _trecojeteta;
  float _trecojetmass;
  float _trecojetp;
  float _trecojetenergy;
  float tjetdphi;
  float tjetpout;
  float tjetdeta;
  float _ttruthjetid;
  float _ttruthjetpt;
  float _ttruthjetpx, _ttruthjetpy, _ttruthjetpz;
  float _ttruthjetphi;
  float _ttruthjeteta;
  float _ttruthjetmass;
  float _ttruthjetp;
  float _ttruthjetenergy;

  //isophot+reco hadron
  float _tr_px, _tr_py, _tr_pz;
  float _tr_p;
  float _tr_pt;
  float _tr_phi;
  float _tr_eta;
  int _charge;
  float _chisq;
  unsigned int _ndf;
  float _dca;
  float _tr_x, _tr_y, _tr_z;
  float haddphi;
  float hadpout;
  float haddeta;
  bool _truth_is_primary;
  float _truthtrackpx;
  float _truthtrackpy;
  float _truthtrackpz;
  float _truthtrackp;
  float _truthtracke;
  float _truthtrackpt;
  float _truthtrackphi;
  float _truthtracketa;
  int _truthtrackpid;

  //all truth particles
  float truthpx, truthpy, truthpz;
  float truthp;
  float truthphi;
  float trutheta;
  float truthpt;
  float truthenergy;
  int truthpid;
  int numparticlesinevent;
  int process_id;

  //clust truth variables
  float clustruthpx;
  float clustruthpy;
  float clustruthpz;
  float clustruthenergy;
  float clustruthpt;
  float clustruthphi;
  float clustrutheta;
  int clustruthpid;

  float cluseventenergy;
  float cluseventphi;
  float cluseventeta;
  float cluseventpt;

  //trigger emulator info (emcal trigger)
  float E_4x4;
  float phi_4x4;
  float eta_4x4;
  float E_2x2;
  float phi_2x2;
  float eta_2x2;

  void Set_Tree_Branches();


  std::vector<float> constituent_dphis;
  std::vector<float> constituent_detas;


  float ConeSum(RawCluster *cluster,
		RawClusterContainer *cluster_container,
                SvtxTrackMap *trackmap, float coneradius,
                GlobalVertex *vtx);
  void GetRecoHadronsAndJets(RawCluster *trig,
                             SvtxTrackMap *tracks,
                             JetMap *jets,
                             JetMap *trackedjets,
                             JetRecoEval *recoeval,
                             SvtxTrackEval *trackeval,
                             PHG4TruthInfoContainer *alltruth,
                             JetTruthEval *trutheval,
                             JetMap *truthjets,
                             GlobalVertex *vtx);

  void GetRecoHadronsAndJetsAA(RawCluster *trig,
                               SvtxTrackMap *tracks,
                               JetMap *recojets,
                               PHG4TruthInfoContainer *alltruth,
                               JetMap *truthjets,
                               GlobalVertex *vtx);

  void initialize_values();

  const float pi2 = -1.5707963;
  const float threepi2 = 4.71238898;
  const float pi = 3.1415926;
};

#endif  // __PHOTONJET_H__
