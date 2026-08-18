#include "HerwigSTARUECheck.h"

#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>
#include <phool/phool.h>

#include <phhepmc/PHHepMCGenEvent.h>
#include <phhepmc/PHHepMCGenEventMap.h>

#include <g4main/PHG4Particle.h>
#include <g4main/PHG4TruthInfoContainer.h>

#include <HepMC/GenEvent.h>
#include <HepMC/GenParticle.h>
#include <HepMC/SimpleVector.h>
#include <HepMC/Units.h>

#include <fastjet/ClusterSequence.hh>
#include <fastjet/JetDefinition.hh>
#include <fastjet/PseudoJet.hh>

#include <TDatabasePDG.h>
#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TMath.h>
#include <TParticlePDG.h>

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <vector>

namespace
{
  // STAR PRD 101, 052004 leading-jet pT binning (HEPData d01-x01-y03)
  const int NBINS = 8;
  const double BINS[NBINS + 1] = {5., 7., 9., 11., 15., 20., 25., 35., 45.};

  const int NBINS_R04 = 9;
  const double BINS_R04[NBINS_R04 + 1] = {5., 7., 9., 11., 15., 21., 26., 32.5, 40.5, 63.5};

  // STAR PLB 637 (2006) pion spectra binning
  const int NBINS_PI = 23;
  const double BINS_PI[NBINS_PI + 1] = {0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1., 1.1, 1.2, 1.4, 1.6, 1.8, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0, 6.0, 7.0, 8.0, 10.0};
}  // namespace

// ====================================================================
//  UEHistSet -- one full set of observables per particle source
// ====================================================================

TH2 *HerwigSTARUECheck::UEHistSet::mk2(const std::string &name, const std::string &title,
                                       int nx, const double *xbins,
                                       int ny, double ylo, double yhi)
{
  TH2D *h = new TH2D(name.c_str(), title.c_str(), nx, xbins, ny, ylo, yhi);
  h->Sumw2();
  all.push_back(h);
  return h;
}

TH1 *HerwigSTARUECheck::UEHistSet::mk1(const std::string &name, const std::string &title,
                                       int nx, const double *xbins)
{
  TH1D *h = new TH1D(name.c_str(), title.c_str(), nx, xbins);
  h->Sumw2();
  all.push_back(h);
  return h;
}

TH1 *HerwigSTARUECheck::UEHistSet::mk1(const std::string &name, const std::string &title,
                                       int nx, double xlo, double xhi)
{
  TH1D *h = new TH1D(name.c_str(), title.c_str(), nx, xlo, xhi);
  h->Sumw2();
  all.push_back(h);
  return h;
}

void HerwigSTARUECheck::UEHistSet::book(const std::string &tag, const std::string &label)
{
  const std::string t = "_" + tag;          // name suffix, e.g. "_hepmc"
  const std::string l = " (" + label + ")";  // title suffix, e.g. " (HepMC)"

  dens_trans_02 = mk2("p_dens_trans_pt02" + t,
                      "Transverse #LTdN_{ch}/d#etad#phi#GT, p_{T}>0.2" + l +
                          ";leading jet p_{T} [GeV/c];#LTdN_{ch}/d#etad#phi#GT",
                      NBINS, BINS, 101, -0.5, 100.5);
  dens_toward_02 = mk2("p_dens_toward_pt02" + t,
                       "Toward #LTdN_{ch}/d#etad#phi#GT, p_{T}>0.2" + l +
                           ";leading jet p_{T} [GeV/c];#LTdN_{ch}/d#etad#phi#GT",
                       NBINS, BINS, 101, -0.5, 100.5);
  dens_away_02 = mk2("p_dens_away_pt02" + t,
                     "Away #LTdN_{ch}/d#etad#phi#GT, p_{T}>0.2" + l +
                         ";leading jet p_{T} [GeV/c];#LTdN_{ch}/d#etad#phi#GT",
                     NBINS, BINS, 101, -0.5, 100.5);
  dens_trans_05 = mk2("p_dens_trans_pt05" + t,
                      "Transverse #LTdN_{ch}/d#etad#phi#GT, p_{T}>0.5" + l +
                          ";leading jet p_{T} [GeV/c];#LTdN_{ch}/d#etad#phi#GT",
                      NBINS, BINS, 101, -0.5, 100.5);

  avgpt_trans_02 = mk2("p_avgpt_trans_pt02" + t,
                       "Transverse #LTp_{T,ch}#GT, p_{T}>0.2" + l +
                           ";leading jet p_{T} [GeV/c];#LTp_{T}#GT [GeV/c]",
                       NBINS, BINS, 1000, 0.0, 70.0);
  avgpt_toward_02 = mk2("p_avgpt_toward_pt02" + t,
                        "Toward #LTp_{T,ch}#GT, p_{T}>0.2" + l +
                            ";leading jet p_{T} [GeV/c];#LTp_{T}#GT [GeV/c]",
                        NBINS, BINS, 1000, 0.0, 70.0);
  avgpt_away_02 = mk2("p_avgpt_away_pt02" + t,
                      "Away #LTp_{T,ch}#GT, p_{T}>0.2" + l +
                          ";leading jet p_{T} [GeV/c];#LTp_{T}#GT [GeV/c]",
                      NBINS, BINS, 1000, 0.0, 70.0);
  avgpt_trans_05 = mk2("p_avgpt_trans_pt05" + t,
                       "Transverse #LTp_{T,ch}#GT, p_{T}>0.5" + l +
                           ";leading jet p_{T} [GeV/c];#LTp_{T}#GT [GeV/c]",
                       NBINS, BINS, 1000, 0.0, 70.0);

  sumET_trans_02 = mk2("p_sumET_trans_02" + t,
                       "Transverse #LT#Sigma E_{T}#GT, p_{T}>0.2" + l +
                           ";leading jet p_{T} [GeV/c];#LT#Sigma E_{T}/#delta#eta#delta#phi#GT [GeV]",
                       NBINS, BINS, 1000, 0.0, 70.0);
  sumET_toward_02 = mk2("p_sumET_toward_02" + t,
                        "Toward #LT#Sigma E_{T}#GT, p_{T}>0.2" + l +
                            ";leading jet p_{T} [GeV/c];#LT#Sigma E_{T}/#delta#eta#delta#phi#GT [GeV]",
                        NBINS, BINS, 1000, 0.0, 70.0);
  sumET_away_02 = mk2("p_sumET_away_02" + t,
                      "Away #LT#Sigma E_{T}#GT, p_{T}>0.2" + l +
                          ";leading jet p_{T} [GeV/c];#LT#Sigma E_{T}/#delta#eta#delta#phi#GT [GeV]",
                      NBINS, BINS, 1000, 0.0, 70.0);
  sumET_trans_05 = mk2("p_sumET_trans_05" + t,
                       "Transverse #LT#Sigma E_{T}#GT, p_{T}>0.5" + l +
                           ";leading jet p_{T} [GeV/c];#LT#Sigma E_{T}/#delta#eta#delta#phi#GT [GeV]",
                       NBINS, BINS, 1000, 0.0, 70.0);

  sumET_trans_R04 = mk2("p_sumET_trans_R04" + t,
                        "Transverse #LT#Sigma E_{T}/#delta#eta#delta#phi#GT, p_{T,ch}>0.5, p_{T,neut}>0.2" + l +
                            ";leading R=0.4 jet p_{T} [GeV/c];#LT#Sigma E_{T}/#delta#eta#delta#phi#GT [GeV]",
                        NBINS_R04, BINS_R04, 1000, 0.0, 70.0);

  pi_spec = mk1("pip_spec" + t,
                "#pi^{+} spectrum" + l +
                    ";p_{T} [GeV/c];#frac{1}{2#pi p_{T}} d^{2}N/dp_{T}dy [GeV^{-2}]",
                NBINS_PI, BINS_PI);

  leadjet_pt = mk1("h_leadjet_pt" + t,
                   "accepted leading jet p_{T}" + l + ";p_{T} [GeV/c];events",
                   80, 0., 80.);
  leadjet_pt_R04 = mk1("h_leadjet_pt_R04" + t,
                       "accepted leading R=0.4 jet p_{T}" + l + ";p_{T} [GeV/c];events",
                       80, 0., 80.);
  leadjet_eta = mk1("h_leadjet_eta" + t,
                    "accepted leading jet #eta" + l + ";#eta;events",
                    40, -1., 1.);
  dphi = mk1("h_dphi" + t,
             "|#Delta#phi(part, leading jet)|, p_{T}>0.2" + l + ";|#Delta#phi|;particles",
             60, 0., M_PI);

  cutflow = mk1("h_cutflow" + t, "cutflow" + l + ";;events", 3, 0.5, 3.5);
  cutflow->GetXaxis()->SetBinLabel(1, "particles found");
  cutflow->GetXaxis()->SetBinLabel(2, "leading R=0.6 jet accepted");
  cutflow->GetXaxis()->SetBinLabel(3, "leading R=0.4 jet accepted");
}

void HerwigSTARUECheck::UEHistSet::write()
{
  for (TH1 *h : all)
  {
    if (h) h->Write();
  }
}

// ====================================================================
//  module
// ====================================================================

HerwigSTARUECheck::HerwigSTARUECheck(const std::string &name,
                                     const std::string &outfile)
  : SubsysReco(name)
  , m_outfileName(outfile)
{
}

int HerwigSTARUECheck::Init(PHCompositeNode * /*topNode*/)
{
  if (!m_doHepMC && !m_doG4Truth)
  {
    std::cerr << Name() << ": both particle sources are disabled, nothing to do" << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  m_outfile = new TFile(m_outfileName.c_str(), "RECREATE");

  if (m_doHepMC) m_hHepMC.book("hepmc", "HepMC");
  if (m_doG4Truth) m_hG4.book("g4", "G4 primary");

  m_h_nevents = new TH1D("h_nevents", "cutflow;;events", 3, 0.5, 3.5);
  m_h_nevents->GetXaxis()->SetBinLabel(1, "processed");
  m_h_nevents->GetXaxis()->SetBinLabel(2, "HepMC found");
  m_h_nevents->GetXaxis()->SetBinLabel(3, "G4TruthInfo found");

  const bool isHerwig = (m_sampleName.find("Herwig") != std::string::npos);
  for (int s = 0; s < 8; s++)
  {
    if (!isHerwig && m_sampleName == sampleNames[s])
    {
      sampleNumber = s;
      break;
    }
    if (isHerwig && s < 7 && m_sampleName == HerwigsampleNames[s])
    {
      sampleNumber = s;
      break;
    }
  }

  if (sampleNumber == -999)
  {
    std::cerr << "sample number is still default. Sample name " << m_sampleName
              << " is not valid. Exiting" << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  // one weight per event, shared by both branches so the comparison is
  // apples-to-apples
  m_weight = m_useCSWeights ? (isHerwig ? HerwigCS[sampleNumber] : cs[sampleNumber]) : 1.0;

  return Fun4AllReturnCodes::EVENT_OK;
}

// --------------------------------------------------------------------
//  particle collection
// --------------------------------------------------------------------

bool HerwigSTARUECheck::collect_hepmc(PHCompositeNode *topNode,
                                      std::vector<fastjet::PseudoJet> &parts,
                                      std::vector<int> &pids)
{
  PHHepMCGenEventMap *genmap =
      findNode::getClass<PHHepMCGenEventMap>(topNode, "PHHepMCGenEventMap");
  if (!genmap)
  {
    static bool once = true;
    if (once)
    {
      std::cout << Name() << ": no PHHepMCGenEventMap node found -- "
                << "is this a generator-level DST?" << std::endl;
      once = false;
    }
    return false;
  }

  PHHepMCGenEvent *genevt = genmap->get(m_embeddingId);
  if (!genevt && !genmap->empty())
  {
    genevt = genmap->begin()->second;  // fall back to first embedded event
  }
  if (!genevt || !genevt->getEvent()) return false;

  HepMC::GenEvent *evt = genevt->getEvent();

  // sPHENIX convention is GeV, but guard against MeV productions
  const double mom2gev = (evt->momentum_unit() == HepMC::Units::MEV) ? 1.e-3 : 1.0;

  for (HepMC::GenEvent::particle_const_iterator it = evt->particles_begin();
       it != evt->particles_end(); ++it)
  {
    const HepMC::GenParticle *p = *it;
    if (p->status() != 1) continue;

    const int pid = p->pdg_id();
    const int apid = std::abs(pid);
    if (!m_includeNeutrinos && (apid == 12 || apid == 14 || apid == 16)) continue;

    const HepMC::FourVector &mom = p->momentum();
    fastjet::PseudoJet pj(mom.px() * mom2gev, mom.py() * mom2gev,
                          mom.pz() * mom2gev, mom.e() * mom2gev);

    if (pj.pt() <= m_constPtMin) continue;
    if (std::fabs(pj.pseudorapidity()) >= m_partEtaMax) continue;

    parts.push_back(pj);
    pids.push_back(pid);
  }

  return true;
}

bool HerwigSTARUECheck::collect_g4truth(PHCompositeNode *topNode,
                                        std::vector<fastjet::PseudoJet> &parts,
                                        std::vector<int> &pids)
{
  PHG4TruthInfoContainer *truthinfo =
      findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
  if (!truthinfo)
  {
    static bool once = true;
    if (once)
    {
      std::cout << PHWHERE << "PHG4TruthInfoContainer node is missing, "
                << "can't collect G4 truth particles" << std::endl;
      once = false;
    }
    return false;
  }

  PHG4TruthInfoContainer::Range range = truthinfo->GetSPHENIXPrimaryParticleRange();
  for (PHG4TruthInfoContainer::ConstIterator iter = range.first; iter != range.second; ++iter)
  {
    const PHG4Particle *truth = iter->second;

    const int pid = truth->get_pid();
    const int apid = std::abs(pid);
    if (!m_includeNeutrinos && (apid == 12 || apid == 14 || apid == 16)) continue;

    fastjet::PseudoJet pj(truth->get_px(), truth->get_py(), truth->get_pz(), truth->get_e());

    if (pj.pt() <= m_constPtMin) continue;
    if (std::fabs(pj.pseudorapidity()) >= m_partEtaMax) continue;

    parts.push_back(pj);
    pids.push_back(pid);
  }

  return true;
}

// --------------------------------------------------------------------
//  analysis, run once per particle source
// --------------------------------------------------------------------

void HerwigSTARUECheck::analyze(UEHistSet &H,
                                const std::vector<fastjet::PseudoJet> &parts,
                                const std::vector<int> &pids,
                                double w)
{
  // pion spectrum: every event, no jet requirement (as before)
  for (int p = 0; p < (int) parts.size(); p++)
  {
    if (pids[p] == 211 && std::fabs(parts[p].rap()) < 0.5)
    {
      H.pi_spec->Fill(parts[p].pt(), w / (2.0 * TMath::Pi() * parts[p].pt()));
    }
  }

  if (parts.empty()) return;
  H.cutflow->Fill(1);

  // ------------------------------------------------------------------
  // jet finding: anti-kT R = m_jetR (0.6) and R = 0.4 on ALL selected
  // final-state particles
  // ------------------------------------------------------------------
  fastjet::JetDefinition jetdef(fastjet::antikt_algorithm, m_jetR);
  fastjet::ClusterSequence cseq(parts, jetdef);
  std::vector<fastjet::PseudoJet> jets = fastjet::sorted_by_pt(cseq.inclusive_jets(m_jetPtMin));

  fastjet::JetDefinition jetdefR04(fastjet::antikt_algorithm, 0.4);
  fastjet::ClusterSequence cseqR04(parts, jetdefR04);
  std::vector<fastjet::PseudoJet> jetsR04 = fastjet::sorted_by_pt(cseqR04.inclusive_jets(m_jetPtMin));

  const fastjet::PseudoJet *lead = nullptr;
  for (const auto &j : jets)
  {
    if (std::fabs(j.pseudorapidity()) < m_jetEtaMax && j.pt() > m_jetPtMin && j.pt() < m_jetPtMax)
    {
      lead = &j;
      break;  // jets are pT-sorted, so this is the hardest jet in acceptance
    }
  }

  // pThat-slice stitching window
  const double loR06 = truthJet_min_pT[sampleNumber];
  const double hiR06 = (sampleNumber + 1 < 8) ? truthJet_min_pT[sampleNumber + 1]
                                              : std::numeric_limits<double>::max();
  const double loR04 = truthJet_min_pT_R04[sampleNumber];
  const double hiR04 = (sampleNumber + 1 < 8) ? truthJet_min_pT_R04[sampleNumber + 1]
                                              : std::numeric_limits<double>::max();

  bool goodR06 = (lead && lead->pt() >= loR06 && lead->pt() < hiR06);

  const fastjet::PseudoJet *leadR04 = nullptr;
  for (const auto &j : jetsR04)
  {
    if (std::fabs(j.pseudorapidity()) < 0.7 && j.pt() > m_jetPtMin && j.pt() < m_jetPtMax)
    {
      leadR04 = &j;
      break;
    }
  }

  bool goodR04 = (leadR04 && leadR04->pt() >= loR04 && leadR04->pt() < hiR04);

  if (!goodR06 && !goodR04) return;

  if (goodR06)
  {
    H.cutflow->Fill(2);
    H.leadjet_pt->Fill(lead->pt(), w);
    H.leadjet_eta->Fill(lead->pseudorapidity(), w);
  }
  if (goodR04)
  {
    H.cutflow->Fill(3);
    H.leadjet_pt_R04->Fill(leadR04->pt(), w);
  }

  // ------------------------------------------------------------------
  // region assignment and densities
  // ------------------------------------------------------------------
  int nToward = 0;
  int nAway = 0;
  int nTrans = 0;
  int nTrans05 = 0;

  double sumET_Toward = 0.0;
  double sumET_Away = 0.0;
  double sumET_Trans = 0.0;
  double sumET_Trans05 = 0.0;
  double sumET_TransR04 = 0.0;

  if (goodR06)
  {
    for (int p = 0; p < (int) parts.size(); p++)
    {
      const fastjet::PseudoJet &ch = parts[p];
      const double adphi = std::fabs(ch.delta_phi_to(*lead));
      H.dphi->Fill(adphi, w);

      const bool charged = (pdg_charge(pids[p]) != 0.);

      if (adphi < M_PI / 3.)  // Toward
      {
        if (charged) ++nToward;
        sumET_Toward += ch.Et();
        H.avgpt_toward_02->Fill(lead->pt(), ch.pt(), w);
      }
      else if (adphi > 2. * M_PI / 3.)  // Away
      {
        if (charged) ++nAway;
        sumET_Away += ch.Et();
        H.avgpt_away_02->Fill(lead->pt(), ch.pt(), w);
      }
      else  // Transverse: pi/3 < |dphi| < 2pi/3
      {
        if (charged) ++nTrans;
        sumET_Trans += ch.Et();
        H.avgpt_trans_02->Fill(lead->pt(), ch.pt(), w);
        if (ch.pt() > 0.5)
        {
          if (charged) ++nTrans05;
          sumET_Trans05 += ch.Et();
          H.avgpt_trans_05->Fill(lead->pt(), ch.pt(), w);
        }
      }
    }
  }

  if (goodR04)
  {
    for (int p = 0; p < (int) parts.size(); p++)
    {
      const fastjet::PseudoJet &part = parts[p];
      const double adphiR04 = std::fabs(part.delta_phi_to(*leadR04));
      if (adphiR04 >= M_PI / 3. && adphiR04 <= 2. * M_PI / 3.)
      {
        const bool em = (pids[p] == 22 || std::abs(pids[p]) == 11);
        if ((!em && part.pt() > 0.5) || (em && part.pt() > 0.2)) sumET_TransR04 += part.Et();
      }
    }
  }

  // area normalization: deta * dphi = (2*etaMax) * (2pi/3), identical for
  // all three regions (each spans a total of 2pi/3 in azimuth)
  const double area = (2. * m_partEtaMax) * (2. * M_PI / 3.);
  const double areaR04 = (2. * 1.1) * (2. * M_PI / 3.);

  if (goodR06)
  {
    H.dens_toward_02->Fill(lead->pt(), nToward, w);
    H.dens_away_02->Fill(lead->pt(), nAway, w);
    H.dens_trans_02->Fill(lead->pt(), nTrans, w);
    H.dens_trans_05->Fill(lead->pt(), nTrans05, w);

    H.sumET_toward_02->Fill(lead->pt(), sumET_Toward / area, w);
    H.sumET_away_02->Fill(lead->pt(), sumET_Away / area, w);
    H.sumET_trans_02->Fill(lead->pt(), sumET_Trans / area, w);
    H.sumET_trans_05->Fill(lead->pt(), sumET_Trans05 / area, w);
  }
  if (goodR04) H.sumET_trans_R04->Fill(leadR04->pt(), sumET_TransR04 / areaR04, w);
}

// --------------------------------------------------------------------

int HerwigSTARUECheck::process_event(PHCompositeNode *topNode)
{
  m_h_nevents->Fill(1);

  if (m_doHepMC)
  {
    std::vector<fastjet::PseudoJet> parts;
    std::vector<int> pids;
    if (collect_hepmc(topNode, parts, pids))
    {
      m_h_nevents->Fill(2);
      analyze(m_hHepMC, parts, pids, m_weight);
    }
    else if (m_requireBoth)
    {
      return Fun4AllReturnCodes::ABORTEVENT;
    }
  }

  if (m_doG4Truth)
  {
    std::vector<fastjet::PseudoJet> parts;
    std::vector<int> pids;
    if (collect_g4truth(topNode, parts, pids))
    {
      m_h_nevents->Fill(3);
      analyze(m_hG4, parts, pids, m_weight);
    }
    else if (m_requireBoth)
    {
      return Fun4AllReturnCodes::ABORTEVENT;
    }
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

int HerwigSTARUECheck::End(PHCompositeNode * /*topNode*/)
{
  m_outfile->cd();

  if (m_doHepMC) m_hHepMC.write();
  if (m_doG4Truth) m_hG4.write();
  m_h_nevents->Write();

  m_outfile->Close();

  std::cout << Name() << ": wrote " << m_outfileName << " ("
            << (long long) m_h_nevents->GetBinContent(1) << " events processed";
  if (m_doHepMC)
  {
    std::cout << "; HepMC: " << (long long) m_hHepMC.cutflow->GetBinContent(2)
              << " with an accepted leading R=0.6 jet";
  }
  if (m_doG4Truth)
  {
    std::cout << "; G4 primary: " << (long long) m_hG4.cutflow->GetBinContent(2)
              << " with an accepted leading R=0.6 jet";
  }
  std::cout << ")" << std::endl;

  return Fun4AllReturnCodes::EVENT_OK;
}

double HerwigSTARUECheck::pdg_charge(int pid)
{
  auto it = m_chargeCache.find(pid);
  if (it != m_chargeCache.end()) return it->second;

  double q = 0.;
  TParticlePDG *pdgp = TDatabasePDG::Instance()->GetParticle(pid);
  if (pdgp)
  {
    q = pdgp->Charge() / 3.;  // TParticlePDG stores charge in |e|/3
  }
  else if (Verbosity() > 0)
  {
    std::cout << Name() << ": PDG id " << pid
              << " unknown to TDatabasePDG, treating as neutral" << std::endl;
  }
  m_chargeCache[pid] = q;
  return q;
}