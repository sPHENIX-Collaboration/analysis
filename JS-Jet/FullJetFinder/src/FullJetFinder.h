// Tell emacs that this is a C++ source
//  -*- C++ -*-.
/**
 * @file FullJetFinder.cc
 *
 * @brief module for producing a TTree with full jets (tracks + calos) studies
 *        Based on JetValidation macro and HF group macros
 *
 * @ingroup FullJetFinder
 *
 * @author Jakub Kvapil
 * Contact: jakub.kvapil@cern.ch
 *
 */
#ifndef FULLJETFINDER_H
#define FULLJETFINDER_H

#include <fun4all/SubsysReco.h>
#include <jetbase/Jetv1.h>
#include <globalvertex/GlobalVertex.h>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <HepMC/GenEvent.h>
#include <HepMC/GenVertex.h>
#pragma GCC diagnostic pop
#include <particleflowreco/ParticleFlowElementv1.h>
#include <trackbase_historic/SvtxTrack.h>

#include <string>
#include <TH1.h>
#include <vector>

class PHCompositeNode;
class TTree;

class FullJetFinder : public SubsysReco {
  public:

enum TYPE {
  FULLJET,
  CHARGEJET,
  CALOJET
}; 

  //note max 10 inputs allowed
  FullJetFinder(	const std::string &outputfilename = "myjetanalysis.root", FullJetFinder::TYPE jet_type = FullJetFinder::TYPE::FULLJET);

  ~FullJetFinder() override;

  /** Called during initialization.
      Typically this is where you can book histograms, and e.g.
      register them to Fun4AllServer (so they can be output to file
      using Fun4AllServer::dumpHistos() method).
   */
  int Init(PHCompositeNode *topNode) override;

  /** Called for first event when run number is known.
      Typically this is where you may want to fetch data from
      database, because you know the run number. A place
      to book histograms which have to know the run number.
   */
  int InitRun(PHCompositeNode *topNode) override;

  /** Called for each event.
      This is where you do the real work.
   */
  int process_event(PHCompositeNode *topNode) override;

  /// Clean up internals after each event.
  int ResetEvent(PHCompositeNode *topNode) override;

  /// Called at the end of each run.
  int EndRun(const int runnumber) override;

  /// Called at the end of all processing.
  int End(PHCompositeNode *topNode) override;

  /// Reset
  int Reset(PHCompositeNode * /*topNode*/) override;

  void Print(const std::string &what = "ALL") const override;

  void GetDistanceFromVertex(SvtxTrack *m_dst_track, GlobalVertex *m_dst_vertex,float &val_xy, float &err_xy, float &val_3d, float &chi2_3d);

  void add_input(const std::string &recojetname = "AntiKt_Tower_r04", const std::string &truthjetname = "AntiKt_Truth_r04", const std::string &outputtreename = "AntiKt_r04"){
      m_recoJetName.push_back(recojetname);
      m_truthJetName.push_back(truthjetname);
      m_TreeNameCollection.push_back(outputtreename);
      m_inputs += 1;
      jetR.push_back(std::stof(recojetname.substr(recojetname.find("r0") + 2))/10);
      std::cout<<"pushing name "<<recojetname<<" "<<truthjetname<<" "<<outputtreename<<" jet R"<<std::stof(recojetname.substr(recojetname.find("r0") + 2))/10<<" total inputs "<<m_inputs<<std::endl;
    }

  void doFiducialAcceptance(bool doF){doFiducial = doF;}

  void setPtRangeReco(double low, double high){m_ptRangeReco.first = low; m_ptRangeReco.second = high;}
  void setPtRangeTruth(double low, double high){m_ptRangeTruth.first = low; m_ptRangeTruth.second = high;}
  void doTruth(int flag){m_doTruthJets = flag; }

  class CutSelection: public PHObject{
    public:
      float jetptmin;
      float jetptmax;
  };

  struct neConstituent{
    ParticleFlowElement::PFLOWTYPE pflowtype;
    float e;
    float eta;
    float phi;
  } ;

  struct chConstituent{
    ParticleFlowElement::PFLOWTYPE pflowtype;
    int vtx_id;
    float e;
    float eta;
    float phi;
    float pt;
    int charge;
    float DCA_xy;
    float DCA_xy_unc;
    float sDCA_xy;
    float DCA3d;
    float sDCA3d;
    int n_mvtx;
    int n_intt;
    int n_tpc;
    float chisq;
    int ndf;
  } ;

  //! reconstructed jets
  class RecoJets{
    public:
      int id;
      float area;
      int num_Constituents;
      int num_ChConstituents;
      float px;
      float py;
      float pz;
      float pt;
      float eta;
      float phi;
      float m;
      float e;
      std::vector<neConstituent> neConstituents;
      std::vector<chConstituent> chConstituents;

    using List = std::vector<RecoJets>;
  };

   struct quark{
    int vtx_barcode;
    int pdgid;
    float px;
    float py;
    float pz;
    float e;
  } ;

  //! truth jets
  class TruthJets{
    public:
      int id;
      float area;
      int num_Constituents;
      int num_ChConstituents;
      float px;
      float py;
      float pz;
      float pt;
      float eta;
      float phi;
      float m;
      float e;
      std::vector<int> constituents_PDG_ID;
      std::vector<quark> constituents_origin_quark;

    using List = std::vector<TruthJets>;
  };

   class PrimaryVertex{
    public:
      float id;
      float x;
      float y;
      float z;
      float x_unc;
      float y_unc;
      float z_unc;
      float chisq;
      int ndf;
      GlobalVertex::VTXTYPE vtxtype;
      using List = PrimaryVertex;
  };

  class Container: public PHObject
  {
    public:
    void Reset();
    PrimaryVertex::List primaryVertex;
    int reco_jet_n;
    int truth_jet_n;
    int centrality;
    float impactparam;
    RecoJets::List recojets;
    TruthJets::List truthjets;
   
    ClassDef(Container,1)
  };

 private:
  int m_inputs = 0;
  std::vector<std::string> m_TreeNameCollection;
  std::vector<std::string> m_recoJetName;
  std::vector<std::string> m_truthJetName;
  std::string m_outputFileName;
  std::pair<double, double> m_etaRange;
  std::pair<double, double> m_ptRangeReco;
  std::pair<double, double> m_ptRangeTruth;
  int m_doTruthJets;

  bool doFiducial = false;

  //! Output Tree variables
  TTree *m_T[10];
  std::vector<float>jetR;
  TH1I *m_stat;
  FullJetFinder::TYPE m_jet_type;

  //! main branch
  Container* m_container[10];

  TH1D *m_chi2ndf[10];
  TH1I *m_mvtxcl[10];
  TH1I *m_inttcl[10];
  TH1I *m_mtpccl[10];
};

#endif // FULLJETFINDER_H
