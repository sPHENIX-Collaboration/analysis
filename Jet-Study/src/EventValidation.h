// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef EVENTVALIDATION_H
#define EVENTVALIDATION_H

#include <fun4all/SubsysReco.h>

#include <string>
#include <vector>

using std::string;
using std::vector;

#include <TH2.h>

class PHCompositeNode;
class TFile;

class EventValidation : public SubsysReco
{
 public:
  EventValidation();

  ~EventValidation() override;

  /** Called during initialization.
      Typically this is where you can book histograms, and e.g.
      register them to Fun4AllServer (so they can be output to file
      using Fun4AllServer::dumpHistos() method).
   */
  Int_t Init(PHCompositeNode *topNode) override;

  /** Called for each event.
      This is where you do the real work.
   */
  Int_t process_event(PHCompositeNode *topNode) override;

  /// Clean up internals after each event.
  Int_t ResetEvent(PHCompositeNode *topNode) override;

  /// Called at the end of all processing.
  Int_t End(PHCompositeNode *topNode) override;

  void set_outputFile(const std::string &m_outputFile) {
    this->m_outputFile = m_outputFile;
  }

 private:

  string m_emcTowerNode;
  string m_ihcTowerNode;
  string m_ohcTowerNode;
  string m_emcGeomNode;
  string m_ihcGeomNode;
  string m_ohcGeomNode;
  string m_recoJetName_r04;

  string m_outputFile;

  UInt_t  m_bins_phi;
  Float_t m_phi_low;
  Float_t m_phi_high;

  UInt_t  m_bins_eta;
  Float_t m_eta_low;
  Float_t m_eta_high;

  Float_t m_R;

  vector<TH2*> h2TowerPx;
  vector<TH2*> h2TowerPy;
  vector<TH2*> h2TowerEnergy;
};

#endif  // EVENTVALIDATION_H
