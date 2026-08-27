// HerwigSTARUECheck
//
// Truth-level cross-check of a HERWIG (Nashville tune) sPHENIX production
// against the STAR underlying-event measurement,
// PRD 101, 052004 (2020) [arXiv:1912.08187].
//
// Reproduces the observable in Fig. 4 (top right) of the HERWIG7 RHIC tune
// paper [arXiv:2411.16897]: mean charged-particle multiplicity density
// <dNch/(deta dphi)> in the Transverse region vs leading-jet pT.
//
// Two independent particle sources are analyzed in the same pass and written
// to *separate* histograms so they can be compared directly:
//   "hepmc" -- generator-level HepMC record (PHHepMCGenEventMap), status==1
//   "g4"    -- PHG4TruthInfoContainer sPHENIX primary particles
// Either can be switched off with set_do_hepmc() / set_do_g4truth().
// Histogram names carry the suffix "_hepmc" / "_g4".
//
// Selections follow the Rivet reference implementation STAR_2019_I1771348
// (github.com/star-bnl/star-pythia8-tune), which is what both the Detroit
// (PYTHIA8) and Nashville/New Haven (HERWIG7) tune papers used:
//   - final-state particles: pT > 0.2 GeV/c, |eta| < 1.0
//   - jets: anti-kT, R = 0.6, clustered from ALL such particles
//   - leading jet: hardest jet with 5 < pT < 45 GeV/c and |eta| < 0.4
//   - regions w.r.t. leading jet: Toward |dphi| < pi/3,
//     Transverse pi/3 < |dphi| < 2pi/3, Away |dphi| > 2pi/3
//   - density normalization: Nch / (deta * dphi) = Nch / (2 * 2pi/3)

#ifndef HERWIGSTARUECHECK_H
#define HERWIGSTARUECHECK_H

#include <fun4all/SubsysReco.h>

#include <fastjet/PseudoJet.hh>

#include <map>
#include <string>
#include <vector>

class PHCompositeNode;
class TFile;
class TH1;
class TH2;

class HerwigSTARUECheck : public SubsysReco
{
 public:
  explicit HerwigSTARUECheck(const std::string &name = "HerwigSTARUECheck",
                             const std::string &outfile = "herwig_ue_check.root");
  ~HerwigSTARUECheck() override = default;

  int Init(PHCompositeNode *topNode) override;
  int process_event(PHCompositeNode *topNode) override;
  int End(PHCompositeNode *topNode) override;

  //! STAR PRD 101, 052004 defaults; setters provided for variations
  void set_jet_R(double r) { m_jetR = r; }
  void set_jet_pt_window(double lo, double hi)
  {
    m_jetPtMin = lo;
    m_jetPtMax = hi;
  }
  void set_jet_abs_eta_max(double e) { m_jetEtaMax = e; }
  void set_constituent_pt_min(double p) { m_constPtMin = p; }
  void set_particle_abs_eta_max(double e) { m_partEtaMax = e; }

  //! The Rivet reference uses a plain FinalState projection, which keeps
  //! neutrinos in the jet clustering. Default matches that; set false to
  //! cluster visible particles only (per-mille level effect here).
  //! NOTE: only acts on the HepMC branch -- PHG4TruthInfoContainer primaries
  //! do not contain neutrinos to begin with.
  void set_include_neutrinos(bool b) { m_includeNeutrinos = b; }

  //! Which embedded HepMC event to analyze (sPHENIX embedding id, default 0)
  void set_embedding_id(int id) { m_embeddingId = id; }

  //! enable/disable each particle source independently
  void set_do_hepmc(bool b) { m_doHepMC = b; }
  void set_do_g4truth(bool b) { m_doG4Truth = b; }

  //! if true, a missing PHHepMCGenEventMap / G4TruthInfo node is a hard error
  //! rather than a silently skipped branch
  void set_require_both_sources(bool b) { m_requireBoth = b; }

  void SetSimSample(std::string sampleName) { m_sampleName = sampleName; };
  std::string GetSimSample() { return m_sampleName; }

  void useCSWeights(bool use) { m_useCSWeights = use; }

 private:
  //! One complete set of observables. Booked once per particle source, with
  //! a name suffix (tag) and a title suffix (label).
  struct UEHistSet
  {
    void book(const std::string &tag, const std::string &label);
    void write();

    //! profiles vs leading-jet pT (STAR binning {5,7,9,11,15,20,25,35,45})
    TH2 *dens_trans_02{nullptr};   //!< <dNch/detadphi>, Transverse, pT>0.2 -- THE fig.4 (top right) observable
    TH2 *dens_toward_02{nullptr};  //!< Toward, pT>0.2
    TH2 *dens_away_02{nullptr};    //!< Away, pT>0.2
    TH2 *dens_trans_05{nullptr};   //!< Transverse, pT>0.5

    TH2 *avgpt_trans_02{nullptr};  //!< <pT_ch>, Transverse, pT>0.2
    TH2 *avgpt_toward_02{nullptr};
    TH2 *avgpt_away_02{nullptr};
    TH2 *avgpt_trans_05{nullptr};

    TH2 *sumET_trans_02{nullptr};
    TH2 *sumET_toward_02{nullptr};
    TH2 *sumET_away_02{nullptr};
    TH2 *sumET_trans_05{nullptr};
    TH2 *sumET_trans_R04{nullptr};

    TH1 *pi_spec{nullptr};

    //! diagnostics for judging whether the production is "sufficient"
    TH1 *leadjet_pt{nullptr};
    TH1 *leadjet_pt_R04{nullptr};
    TH1 *leadjet_eta{nullptr};
    TH1 *dphi{nullptr};      //!< |dphi(particle, leading jet)|, pT>0.2
    TH1 *cutflow{nullptr};   //!< particles found / R=0.6 lead / R=0.4 lead

    std::vector<TH1 *> all;  //!< everything booked, in creation order

   private:
    TH2 *mk2(const std::string &name, const std::string &title,
             int nx, const double *xbins, int ny, double ylo, double yhi);
    TH1 *mk1(const std::string &name, const std::string &title,
             int nx, const double *xbins);
    TH1 *mk1(const std::string &name, const std::string &title,
             int nx, double xlo, double xhi);
  };

  //! fill parts/pids from the generator record; false if the node is absent
  bool collect_hepmc(PHCompositeNode *topNode,
                     std::vector<fastjet::PseudoJet> &parts,
                     std::vector<int> &pids);

  //! fill parts/pids from the G4 primary particles; false if node is absent
  bool collect_g4truth(PHCompositeNode *topNode,
                       std::vector<fastjet::PseudoJet> &parts,
                       std::vector<int> &pids);

  //! the actual UE analysis, run once per particle source
  void analyze(UEHistSet &H,
               const std::vector<fastjet::PseudoJet> &parts,
               const std::vector<int> &pids,
               double w);

  double pdg_charge(int pid);

  std::string m_outfileName;

  double m_jetR = 0.6;
  double m_jetPtMin = 5.0;
  double m_jetPtMax = 45.0;
  double m_jetEtaMax = 0.4;
  double m_constPtMin = 0.2;
  double m_partEtaMax = 1.0;
  bool m_includeNeutrinos = true;
  int m_embeddingId = 0;

  bool m_doHepMC{true};
  bool m_doG4Truth{true};
  bool m_requireBoth{false};

  std::string m_sampleName{"Jet20"};
  int sampleNumber{-999};

  bool m_useCSWeights{false};
  double m_weight{1.0};  //!< per-event weight, identical for both sources

  const std::string sampleNames[8] = {"MB", "Jet5", "Jet12", "Jet20", "Jet30", "Jet40", "Jet50", "Jet60"};
  const std::string HerwigsampleNames[7] = {"HerwigMB", "HerwigJet5", "HerwigJet12", "HerwigJet20", "HerwigJet30", "HerwigJet40", "HerwigJet50"};
  const float truthJet_min_pT[8] = {0, 12, 22, 29, 41, 53, 63, 72};
  const float truthJet_min_pT_R04[8] = {0, 7, 14, 21, 32, 42, 52, 62};
  const double cs[8] = {4.1970e+10, 1.3878e+08, 1.4903e+06, 6.2623e+04, 2.5298e+03, 1.3553e+02, 7.3113, 3.3261e-01};
  const double HerwigCS[7] = {3.1909e+10, 1.8437e+08, 6.7108e+05, 5.2613e+04, 2.0694e+03, 1.0510e+02, 5.2089};

  std::map<int, double> m_chargeCache;

  TFile *m_outfile{nullptr};

  UEHistSet m_hHepMC;  //!< generator-level (HepMC) observables
  UEHistSet m_hG4;     //!< G4 primary-particle observables

  TH1 *m_h_nevents{nullptr};  //!< processed / HepMC node / G4 node
};

#endif  // HERWIGSTARUECHECK_H