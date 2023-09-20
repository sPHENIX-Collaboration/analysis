#ifndef __HIJBKG_UPC_H__
#define __HIJBKG_UPC_H__

#include <Rtypes.h>
#include <fun4all/SubsysReco.h>

#include <string>
#include <vector>
#include <TParticle.h>

class PHCompositeNode;
class PHG4Particle;
class TTree;
class TFile;

class hijbkg_upc : public SubsysReco
{
 public:

  hijbkg_upc(const std::string &name = "hijbkg_upc");

  ~hijbkg_upc() override;

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

 private:

  TTree *T;
  int m_evt {0};            // event number
  Float_t m_cent = 0;       // centrality
  Float_t m_b = 0;          // impact parameter
  TParticle m_part[2];      // particle 1 and 2

  std::vector<int> m_pid = {0};
  std::vector<Float_t> m_pt =  {0};
  std::vector<Float_t> m_eta = {0};
  std::vector<Float_t> m_phi = {0};
  std::vector<Float_t> m_e = {0};
  std::vector<Float_t> m_p = {0};
  Float_t m_psi2 = 0;

  std::string Outfile;
  TFile *out;

  int isStableCharged(int pid);
  float getpT(PHG4Particle *particle);
  float getPhi(PHG4Particle *particle);
  float getEta(PHG4Particle *particle);
  float getP(PHG4Particle *particle);
  
};

#endif // __HIJBKG_UPC_H__
