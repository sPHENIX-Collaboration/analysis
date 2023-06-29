// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef HCALJETPHISHIFT_H
#define HCALJETPHISHIFT_H

#include <fun4all/SubsysReco.h>

#include <string>
#include <vector>

class PHCompositeNode;
class TTree;
class TFile;

class HCalJetPhiShift : public SubsysReco
{
public:
  
  HCalJetPhiShift(const std::string &name = "HCalJetPhiShift", const std::string &outputFile = "HCalJetPhiShift_test.root");
  
  ~HCalJetPhiShift() override;
  
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
  
  void SetEventNumber(int event_number)
  {
    m_event = event_number;
  };
    
private:
  std::string m_outputFileName;
  
  //! Output Tree variables
  TTree *m_T;
  
  //! eventwise quantities
  int m_event;
  int m_nTow_in;
  int m_nTow_out;
  float m_eta;
  float m_phi;
  float m_e;
  float m_pt;
  float m_vx;
  float m_vy;
  float m_vz;

  //! towers
  std::vector<int> m_id;
  std::vector<float> m_eta_in;
  std::vector<float> m_phi_in;
  std::vector<float> m_e_in;
  std::vector<int> m_ieta_in;
  std::vector<int> m_iphi_in;
  
  std::vector<float> m_eta_out;
  std::vector<float> m_phi_out;
  std::vector<float> m_e_out;
  std::vector<int> m_ieta_out;
  std::vector<int> m_iphi_out;

  int FillTTree(PHCompositeNode *topNode);
};

#endif // HCALJETPHISHIFT_H
