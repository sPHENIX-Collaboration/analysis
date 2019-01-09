#ifndef ConversionClusterTreeMaker_h 
#define ConversionClusterTreeMaker_h

#include <fun4all/SubsysReco.h>

#include "TTree.h"
#include "TFile.h"
#include "TruthConversionEval.h"

#include <string>
#include <vector>

class PHCompositeNode;

class ConversionClusterTreeMaker: public SubsysReco
{

 public:
  ConversionClusterTreeMaker(): SubsysReco("ConversionClusterTree"){}
  ConversionClusterTreeMaker(const std::string &name);
  ConversionClusterTreeMaker(const std::string &name,TruthConversionEval* conversionModule);

  ~ConversionClusterTreeMaker(){}//don't delete the Single* b/c the server will 

  int Init(PHCompositeNode*);
  int process_event(PHCompositeNode*);
  int End(PHCompositeNode*);

 private:

  TFile *_f;
  TTree *_tree;

  std::string _foutname;

  TruthConversionEval *conversionModule=NULL;

  float vx;
  float vy;
  float vz;

  int _b_cluster_n;
  double _b_cluster_et[500];
  double _b_cluster_eta[500];
  double _b_cluster_phi[500];
  double _b_cluster_prob[500];

  int _b_NTowers[500];

  int _b_clusterTower_towers;
  double _b_clusterTower_eta[500];
  double _b_clusterTower_phi[500];
  double _b_clusterTower_energy[500];

};
#endif


