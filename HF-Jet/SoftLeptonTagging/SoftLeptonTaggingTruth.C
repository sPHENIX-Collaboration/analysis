#include "SoftLeptonTaggingTruth.h"

#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/PHTFileServer.h>
#include <phool/PHCompositeNode.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/getClass.h>

#include <phool/PHCompositeNode.h>

#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4VtxPoint.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4HitDefs.h>

#include <g4eval/JetEvalStack.h>
#include <g4eval/JetTruthEval.h>

#include <TString.h>
#include <TFile.h>
#include <TMath.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TVector3.h>
#include <TLorentzVector.h>
#include <TAxis.h>
#include <TLine.h>
#include <TDatabasePDG.h>

#include <exception>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <cassert>
#include <cmath>

using namespace std;

SoftLeptonTaggingTruth::SoftLeptonTaggingTruth(const std::string & truth_jet,
    enu_flags flags) :
    SubsysReco("SoftLeptonTaggingTruth_" + truth_jet), //
    _jetevalstacks(), //
    _truth_jet(truth_jet), _reco_jets(), _truth_container(NULL), _flags(flags), //
    eta_range(-1, 1), //
    _jet_match_dR(.7), _jet_match_dca(.2), _jet_match_E_Ratio(.0)
{

}

SoftLeptonTaggingTruth::~SoftLeptonTaggingTruth()
{
}

int
SoftLeptonTaggingTruth::InitRun(PHCompositeNode *topNode)
{
  _truth_container = findNode::getClass<PHG4TruthInfoContainer>(topNode,
      "G4TruthInfo");
  if (!_truth_container)
    {
      cout << "SoftLeptonTaggingTruth::InitRun - Fatal Error - "
          << "unable to find DST node " << "G4TruthInfo" << endl;
      assert(_truth_container);
    }

  if (flag(kProcessTruthSpectrum))
    {
      if (not _jettrutheval)
        _jettrutheval = shared_ptr < JetTruthEval
            > (new JetTruthEval(topNode, _truth_jet));

      assert(_jettrutheval);
      _jettrutheval->set_strict(true);
      _jettrutheval->set_verbosity(verbosity + 1);
    }

  return Fun4AllReturnCodes::EVENT_OK;
}

int
SoftLeptonTaggingTruth::End(PHCompositeNode *topNode)
{

  return Fun4AllReturnCodes::EVENT_OK;
}

int
SoftLeptonTaggingTruth::Init(PHCompositeNode *topNode)
{

  Fun4AllHistoManager *hm = getHistoManager();
  assert(hm);

  if (flag(kProcessTruthSpectrum))
    {
      if (verbosity >= 1)
        cout << "SoftLeptonTaggingTruth::Init - Process TruthSpectrum "
            << _truth_jet << endl;
      Init_Spectrum(topNode, _truth_jet);
    }

  return Fun4AllReturnCodes::EVENT_OK;
}

int
SoftLeptonTaggingTruth::process_event(PHCompositeNode *topNode)
{

  if (verbosity > 2)
    cout << "SoftLeptonTaggingTruth::process_event() entered" << endl;

  for (jetevalstacks_map::iterator it_jetevalstack = _jetevalstacks.begin();
      it_jetevalstack != _jetevalstacks.end(); ++it_jetevalstack)
    {
      assert(it_jetevalstack->second);
      it_jetevalstack->second->next_event(topNode);
    }
  if (_jettrutheval)
    _jettrutheval->next_event(topNode);

  if (flag(kProcessTruthSpectrum))
    {
      if (verbosity >= 1)
        cout << "SoftLeptonTaggingTruth::process_event - Process TruthSpectrum "
            << _truth_jet << endl;
      process_Spectrum(topNode, _truth_jet, false);
    }

  return Fun4AllReturnCodes::EVENT_OK;
}

//! set eta range
void
SoftLeptonTaggingTruth::set_eta_range(double low, double high)
{
  if (low > high)
    swap(low, high);
  assert(low < high);
  // eliminate zero range

  eta_range.first = low;
  eta_range.second = high;
}

//! string description of eta range
//! @return TString as ROOT likes
TString
SoftLeptonTaggingTruth::get_eta_range_str(const char * eta_name) const
{
  assert(eta_name);
  return TString(
      Form("%.1f < %s < %.1f", eta_range.first, eta_name, eta_range.second));
}

//! acceptance cut on jet object
bool
SoftLeptonTaggingTruth::jet_acceptance_cut(const Jet * jet) const
{
  assert(jet);
  bool eta_cut = (jet->get_eta() >= eta_range.first)
      and (jet->get_eta() <= eta_range.second);
  return eta_cut;
}

string
SoftLeptonTaggingTruth::get_histo_prefix(const std::string & src_jet_name,
    const std::string & reco_jet_name)
{
  std::string histo_prefix = "h_QAG4SimJet_";

  if (src_jet_name.length() > 0)
    {
      histo_prefix += src_jet_name;
      histo_prefix += "_";
    }
  if (reco_jet_name.length() > 0)
    {
      histo_prefix += reco_jet_name;
      histo_prefix += "_";
    }

  return histo_prefix;
}

int
SoftLeptonTaggingTruth::Init_Spectrum(PHCompositeNode *topNode,
    const std::string & jet_name)
{

  Fun4AllHistoManager *hm = getHistoManager();
  assert(hm);

  // normalization plot with counts
  const int norm_size = 3;

  TH1D * h_norm = new TH1D(
      TString(get_histo_prefix(jet_name)) + "Normalization",
      " Normalization;Item;Count", norm_size, .5, norm_size + .5);
  int i = 1;

  h_norm->GetXaxis()->SetBinLabel(i++, "Event");
  h_norm->GetXaxis()->SetBinLabel(i++, "Inclusive Jets");
  h_norm->GetXaxis()->SetBinLabel(i++, "Leading Jets");
  assert(norm_size >= i - 1);
  h_norm->GetXaxis()->LabelsOption("v");
  hm->registerHisto(h_norm);

  hm->registerHisto(
      new TH1F(
          //
          TString(get_histo_prefix(jet_name)) + "Leading_Et", //
          TString(jet_name) + " leading jet Et, " + get_eta_range_str()
              + ";E_{T} (GeV)", 100, 0, 100));
  hm->registerHisto(
      new TH1F(
          //
          TString(get_histo_prefix(jet_name)) + "Leading_eta", //
          TString(jet_name) + " leading jet #eta, " + get_eta_range_str()
              + ";#eta", 50, -1, 1));
  hm->registerHisto(
      new TH1F(
          //
          TString(get_histo_prefix(jet_name)) + "Leading_phi", //
          TString(jet_name) + " leading jet #phi, " + get_eta_range_str()
              + ";#phi", 50, -M_PI, M_PI));

  TH1F * lcomp = new TH1F(
      //
      TString(get_histo_prefix(jet_name)) + "Leading_CompSize", //
      TString(jet_name) + " leading jet # of component, " + get_eta_range_str()
          + ";Number of component;", 100, 1, 3000);
  useLogBins(lcomp->GetXaxis());
  hm->registerHisto(lcomp);

  hm->registerHisto(
      new TH1F(
          //
          TString(get_histo_prefix(jet_name)) + "Leading_Mass", //
          TString(jet_name) + " leading jet mass, " + get_eta_range_str()
              + ";Jet Mass (GeV);", 100, 0, 20));
  hm->registerHisto(
      new TH1F(
          //
          TString(get_histo_prefix(jet_name)) + "Leading_CEMC_Ratio", //
          TString(jet_name) + " leading jet EMCal ratio, " + get_eta_range_str()
              + ";Energy ratio CEMC/Total;", 100, 0, 1.01));
  hm->registerHisto(
      new TH1F(
          //
          TString(get_histo_prefix(jet_name)) + "Leading_CEMC_HCalIN_Ratio", //
          TString(jet_name) + " leading jet EMCal+HCal_{IN} ratio, "
              + get_eta_range_str() + ";Energy ratio (CEMC + HCALIN)/Total;",
          100, 0, 1.01));

  // reco jet has no definition for leakages, since leakage is not reconstructed as part of jet energy.
  // It is only available for truth jets
  hm->registerHisto(
      new TH1F(
          //
          TString(get_histo_prefix(jet_name)) + "Leading_Leakage_Ratio", //
          TString(jet_name) + " leading jet leakage ratio, "
              + get_eta_range_str() + ";Energy ratio, Back leakage/Total;", 100,
          0, 1.01));

  TH1F * h = new TH1F(
      //
      TString(get_histo_prefix(jet_name)) + "Inclusive_E", //
      TString(jet_name) + " inclusive jet E, " + get_eta_range_str()
          + ";Total jet energy (GeV)", 100, 1e-3, 100);
  useLogBins(h->GetXaxis());
  hm->registerHisto(h);
  hm->registerHisto(
      new TH1F(
          //
          TString(get_histo_prefix(jet_name)) + "Inclusive_eta", //
          TString(jet_name) + " inclusive jet #eta, " + get_eta_range_str()
              + ";#eta;Jet energy density", 50, -1, 1));
  hm->registerHisto(
      new TH1F(
          //
          TString(get_histo_prefix(jet_name)) + "Inclusive_phi", //
          TString(jet_name) + " inclusive jet #phi, " + get_eta_range_str()
              + ";#phi;Jet energy density", 50, -M_PI, M_PI));

  TH2F * h2 = NULL;

  h2 = new TH2F(
      //
      TString(get_histo_prefix(jet_name)) + "Leading_Trk_PtRel", //
      TString(jet_name) + " leading jet, " + get_eta_range_str()
          + ";j_{T} (GeV/c);Particle Selection", 100, 0, 10, 4, 0.5, 4.5);
  i = 1;
  h2->GetYaxis()->SetBinLabel(i++, "Electron");
  h2->GetYaxis()->SetBinLabel(i++, "Muon");
  h2->GetYaxis()->SetBinLabel(i++, "Other Charged");
  h2->GetYaxis()->SetBinLabel(i++, "Other Neutral");
  hm->registerHisto(h2);

  h2 = new TH2F(
      //
      TString(get_histo_prefix(jet_name)) + "Leading_Trk_DCA2D", //
      TString(jet_name) + " leading jet, " + get_eta_range_str()
          + ";DCA_{2D} (cm);Particle Selection", 200, -.2, .2, 4, 0.5, 4.5);
  i = 1;
  h2->GetYaxis()->SetBinLabel(i++, "Electron");
  h2->GetYaxis()->SetBinLabel(i++, "Muon");
  h2->GetYaxis()->SetBinLabel(i++, "Other Charged");
  h2->GetYaxis()->SetBinLabel(i++, "Other Neutral");
  hm->registerHisto(h2);

  h2 = new TH2F(
      //
      TString(get_histo_prefix(jet_name)) + "Leading_Trk_PoverETotal", //
      TString(jet_name) + " leading jet, " + get_eta_range_str()
          + ";|P_{part.}|c/E_{Jet};Particle Selection", 120, 0, 1.2, 4, 0.5,
      4.5);
  i = 1;
  h2->GetYaxis()->SetBinLabel(i++, "Electron");
  h2->GetYaxis()->SetBinLabel(i++, "Muon");
  h2->GetYaxis()->SetBinLabel(i++, "Other Charged");
  h2->GetYaxis()->SetBinLabel(i++, "Other Neutral");
  hm->registerHisto(h2);

  h2 = new TH2F(
      //
      TString(get_histo_prefix(jet_name)) + "Leading_Trk_dR", //
      TString(jet_name) + " leading jet, " + get_eta_range_str()
          + ";#DeltaR;Particle Selection", 100, 0, 1, 4, 0.5, 4.5);
  i = 1;
  h2->GetYaxis()->SetBinLabel(i++, "Electron");
  h2->GetYaxis()->SetBinLabel(i++, "Muon");
  h2->GetYaxis()->SetBinLabel(i++, "Other Charged");
  h2->GetYaxis()->SetBinLabel(i++, "Other Neutral");
  hm->registerHisto(h2);

  return Fun4AllReturnCodes::EVENT_OK;
}

int
SoftLeptonTaggingTruth::process_Spectrum(PHCompositeNode *topNode,
    const std::string & jet_name, const bool is_reco_jet)
{
  JetMap* jets = findNode::getClass<JetMap>(topNode, jet_name.c_str());
  if (!jets)
    {
      cout
          << "SoftLeptonTaggingTruth::process_Spectrum - Error can not find DST JetMap node "
          << jet_name << endl;
      exit(-1);
    }

  Fun4AllHistoManager *hm = getHistoManager();
  assert(hm);

  TH1D* h_norm = dynamic_cast<TH1D*>(hm->getHisto(
      get_histo_prefix(jet_name) + "Normalization"));
  assert(h_norm);
  h_norm->Fill("Event", 1);
  h_norm->Fill("Inclusive Jets", jets->size());

  TH1F * ie = dynamic_cast<TH1F*>(hm->getHisto(
      (get_histo_prefix(jet_name)) + "Inclusive_E" //
          ));
  assert(ie);
  TH1F * ieta = dynamic_cast<TH1F*>(hm->getHisto(
      (get_histo_prefix(jet_name)) + "Inclusive_eta" //
          ));
  assert(ieta);
  TH1F * iphi = dynamic_cast<TH1F*>(hm->getHisto(
      (get_histo_prefix(jet_name)) + "Inclusive_phi" //
          ));
  assert(iphi);

  Jet* leading_jet = NULL;
  double max_et = 0;
  for (JetMap::Iter iter = jets->begin(); iter != jets->end(); ++iter)
    {
      Jet* jet = iter->second;
      assert(jet);

      if (not jet_acceptance_cut(jet))
        continue;

      if (jet->get_et() > max_et)
        {
          leading_jet = jet;
          max_et = jet->get_et();
        }

      ie->Fill(jet->get_e());
      ieta->Fill(jet->get_eta());
      iphi->Fill(jet->get_phi());
    }

  if (leading_jet)
    {
      if (verbosity)
        {
          cout
              << "SoftLeptonTaggingTruth::process_Spectrum - processing leading jet with # comp = "
              << leading_jet->size_comp() << endl;
          leading_jet->identify();
        }

      h_norm->Fill("Leading Jets", 1);

      TH1F * let = dynamic_cast<TH1F*>(hm->getHisto(
          (get_histo_prefix(jet_name)) + "Leading_Et" //
              ));
      assert(let);
      TH1F * leta = dynamic_cast<TH1F*>(hm->getHisto(
          (get_histo_prefix(jet_name)) + "Leading_eta" //
              ));
      assert(leta);
      TH1F * lphi = dynamic_cast<TH1F*>(hm->getHisto(
          (get_histo_prefix(jet_name)) + "Leading_phi" //
              ));
      assert(lphi);

      TH1F * lcomp = dynamic_cast<TH1F*>(hm->getHisto(
          (get_histo_prefix(jet_name)) + "Leading_CompSize" //
              ));
      assert(lcomp);
      TH1F * lmass = dynamic_cast<TH1F*>(hm->getHisto(
          (get_histo_prefix(jet_name)) + "Leading_Mass" //
              ));
      assert(lmass);
      TH1F * lcemcr = dynamic_cast<TH1F*>(hm->getHisto(
          (get_histo_prefix(jet_name)) + "Leading_CEMC_Ratio" //
              ));
      assert(lcemcr);
      TH1F * lemchcalr = dynamic_cast<TH1F*>(hm->getHisto(
          (get_histo_prefix(jet_name)) + "Leading_CEMC_HCalIN_Ratio" //
              ));
      assert(lemchcalr);
      TH1F * lleak = dynamic_cast<TH1F*>(hm->getHisto(
          (get_histo_prefix(jet_name)) + "Leading_Leakage_Ratio" //
              ));
      assert(lleak);

      let->Fill(leading_jet->get_et());
      leta->Fill(leading_jet->get_eta());
      lphi->Fill(leading_jet->get_phi());
      lcomp->Fill(leading_jet->size_comp());
      lmass->Fill(leading_jet->get_mass());

      if (is_reco_jet)
        { // this is a reco jet
          jetevalstacks_map::iterator it_stack = _jetevalstacks.find(jet_name);
          assert(it_stack != _jetevalstacks.end());
          shared_ptr<JetEvalStack> eval_stack = it_stack->second;
          assert(eval_stack);
          JetRecoEval* recoeval = eval_stack->get_reco_eval();
          assert(recoeval);

          lcemcr->Fill( //
              (recoeval->get_energy_contribution(leading_jet, Jet::CEMC_TOWER) //
              +//
                  recoeval->get_energy_contribution(leading_jet,
                      Jet::CEMC_CLUSTER) //
              ) / leading_jet->get_e());
          lemchcalr->Fill( //
              (recoeval->get_energy_contribution(leading_jet, Jet::CEMC_TOWER) //
              +//
                  recoeval->get_energy_contribution(leading_jet,
                      Jet::CEMC_CLUSTER) //
                  +//
                  recoeval->get_energy_contribution(leading_jet,
                      Jet::HCALIN_TOWER) //
                  +//
                  recoeval->get_energy_contribution(leading_jet,
                      Jet::HCALIN_CLUSTER) //
              ) / leading_jet->get_e());
          // reco jet has no definition for leakages, since leakage is not reconstructed as part of jet energy.
          // It is only available for truth jets
        }
      else
        { // this is a truth jet
          assert(_jettrutheval);

          double cemc_e = 0;
          double hcalin_e = 0;
          double bh_e = 0;

          set<PHG4Shower*> showers = _jettrutheval->all_truth_showers(
              leading_jet);

          for (set<PHG4Shower*>::const_iterator it = showers.begin();
              it != showers.end(); ++it)
            {
              cemc_e += (*it)->get_edep(
                  PHG4HitDefs::get_volume_id("G4HIT_CEMC"));
              cemc_e += (*it)->get_edep(
                  PHG4HitDefs::get_volume_id("G4HIT_CEMC_ELECTRONICS"));
              cemc_e += (*it)->get_edep(
                  PHG4HitDefs::get_volume_id("G4HIT_ABSORBER_CEMC"));

              hcalin_e += (*it)->get_edep(
                  PHG4HitDefs::get_volume_id("G4HIT_HCALIN"));
              hcalin_e += (*it)->get_edep(
                  PHG4HitDefs::get_volume_id("G4HIT_ABSORBER_HCALIN"));

              bh_e += (*it)->get_edep(PHG4HitDefs::get_volume_id("G4HIT_BH_1"));
              bh_e += (*it)->get_edep(
                  PHG4HitDefs::get_volume_id("G4HIT_BH_FORWARD_PLUS"));
              bh_e += (*it)->get_edep(
                  PHG4HitDefs::get_volume_id("G4HIT_BH_FORWARD_NEG"));
            }

          lcemcr->Fill(cemc_e / leading_jet->get_e());
          lemchcalr->Fill((cemc_e + hcalin_e) / leading_jet->get_e());
          lleak->Fill(bh_e / leading_jet->get_e());

        }

      TH2F * lptrel = dynamic_cast<TH2F*>(hm->getHisto(
          (get_histo_prefix(jet_name)) + "Leading_Trk_PtRel" //
              ));
      assert(lptrel);
      TH2F * ldca2d = dynamic_cast<TH2F*>(hm->getHisto(
          (get_histo_prefix(jet_name)) + "Leading_Trk_DCA2D" //
              ));
      assert(ldca2d);
      TH2F * lpoe = dynamic_cast<TH2F*>(hm->getHisto(
          (get_histo_prefix(jet_name)) + "Leading_Trk_PoverETotal" //
              ));
      assert(lpoe);
      TH2F * ldr = dynamic_cast<TH2F*>(hm->getHisto(
          (get_histo_prefix(jet_name)) + "Leading_Trk_dR" //
              ));
      assert(ldr);

      // jet vector
      TVector3 j_v(leading_jet->get_px(), leading_jet->get_py(),
          leading_jet->get_pz());

      PHG4TruthInfoContainer::ConstRange primary_range =
          _truth_container->GetPrimaryParticleRange();
      for (PHG4TruthInfoContainer::ConstIterator particle_iter =
          primary_range.first; particle_iter != primary_range.second;
          ++particle_iter)
        {
          PHG4Particle *particle = particle_iter->second;
          assert(particle);
          const PHG4VtxPoint* primary_vtx = //
              _truth_container->GetPrimaryVtx(particle->get_vtx_id());
          assert(primary_vtx);

          TVector3 p_v(particle->get_px(), particle->get_py(),
              particle->get_pz());

          TVector3 v_v(primary_vtx->get_x(), primary_vtx->get_y(),
              primary_vtx->get_z());

          const double dR = sqrt(
              (p_v.Eta() - j_v.Eta()) * (p_v.Eta() - j_v.Eta())
                  + (p_v.Phi() - j_v.Phi()) * (p_v.Phi() - j_v.Phi()));
          const double E_Ratio = p_v.Mag() / leading_jet->get_e();

          const double charge3 = TDatabasePDG::Instance()->GetParticle(
              particle->get_pid())->Charge();
          TVector3 dca_v(cos(p_v.Phi() + TMath::Pi() / 2),
              sin(p_v.Phi() + TMath::Pi() / 2), 0);
          const double dca2d = v_v.Dot(dca_v) * copysign(1, charge3);

          if (dR > _jet_match_dR)
            continue;
          if (abs(dca2d) > _jet_match_dca)
            continue;
          if (E_Ratio < _jet_match_E_Ratio)
            continue;


          int histo_pid = 0;
          if (abs(particle->get_pid())
              == TDatabasePDG::Instance()->GetParticle("e-")->PdgCode())
            histo_pid = ldr->GetYaxis()->FindBin("Electron");
          else if (abs(particle->get_pid())
              == TDatabasePDG::Instance()->GetParticle("mu-")->PdgCode())
            histo_pid = ldr->GetYaxis()->FindBin("Muon");
          else if (charge3 != 0)
            histo_pid = ldr->GetYaxis()->FindBin("Other Charged");
          else
            histo_pid = ldr->GetYaxis()->FindBin("Other Neutral");
          assert(histo_pid);

          if (verbosity)
            {
              cout
                  << "SoftLeptonTaggingTruth::process_Spectrum - particle hist ID "
                  << histo_pid << ", charge x 3 = " << charge3 << ", dR = "
                  << dR << ", E_Ratio = " << E_Ratio << "/"
                  << _jet_match_E_Ratio << " for ";
              particle->identify();
            }

          const double pT_rel = p_v.Dot(j_v) / j_v.Mag();

          ldr->Fill(dR, histo_pid);
          lpoe->Fill(E_Ratio, histo_pid);
          lptrel->Fill(pT_rel, histo_pid);
          ldca2d->Fill(dca2d, histo_pid);
        } //       for (PHG4TruthInfoContainer::ConstIterator particle_iter =

    } //   if (leading_jet)

  return Fun4AllReturnCodes::EVENT_OK;
}

//! Get a pointer to the default hist manager for QA modules
Fun4AllHistoManager *
SoftLeptonTaggingTruth::getHistoManager()
{

  Fun4AllServer *se = Fun4AllServer::instance();
  Fun4AllHistoManager *hm = se->getHistoManager("histSoftLeptonTaggingTruth");

  if (not hm)
    {
//        cout
//            << "QAHistManagerDef::get_HistoManager - Making Fun4AllHistoManager EMCalAna_HISTOS"
//            << endl;
      hm = new Fun4AllHistoManager("histSoftLeptonTaggingTruth");
      se->registerHistoManager(hm);
    }

  assert(hm);

  return hm;
}

//! utility function to
void
SoftLeptonTaggingTruth::useLogBins(TAxis * axis)
{
  assert(axis);
  assert(axis->GetXmin()>0);
  assert( axis->GetXmax()>0);

  const int bins = axis->GetNbins();

  Axis_t from = log10(axis->GetXmin());
  Axis_t to = log10(axis->GetXmax());
  Axis_t width = (to - from) / bins;
  vector<Axis_t> new_bins(bins + 1);

  for (int i = 0; i <= bins; i++)
    {
      new_bins[i] = TMath::Power(10, from + i * width);
    }
  axis->Set(bins, new_bins.data());

}
