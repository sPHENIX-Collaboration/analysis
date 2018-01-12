#include "EMCalLikelihood.h"
#include "EMCalTrk.h"

#include <phool/getClass.h>
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/PHTFileServer.h>
#include <phool/PHCompositeNode.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/Fun4AllHistoManager.h>

#include <phool/PHCompositeNode.h>

#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4VtxPoint.h>
#include <g4main/PHG4Particle.h>

#include <g4hough/SvtxVertexMap.h>

#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTower.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawCluster.h>

#include <g4eval/CaloEvalStack.h>
#include <g4eval/CaloRawClusterEval.h>
#include <g4eval/CaloRawTowerEval.h>
#include <g4eval/CaloTruthEval.h>
#include <g4eval/SvtxEvalStack.h>

#include <TNtuple.h>
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TVector3.h>
#include <TLorentzVector.h>

#include <exception>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <cassert>
#include <cmath>

using namespace std;

EMCalLikelihood::EMCalLikelihood(const std::string &filename) :
    SubsysReco("EMCalLikelihood"), _filename(filename), _ievent(0), _trk(NULL) //
        , center_cemc_iphi(NAN), center_cemc_ieta(NAN), //
    center_hcalin_iphi(NAN), center_hcalin_ieta(NAN), //
    width_cemc_iphi(NAN), width_cemc_ieta(NAN), //
    width_hcalin_iphi(NAN), width_hcalin_ieta(NAN), //
    h2_Edep_Distribution_e(NULL), h2_Edep_Distribution_pi(NULL), //
    h1_ep_Distribution_e(NULL), h1_ep_Distribution_pi(NULL), //
    _do_ganging(false), _ganging_size(1, 1)
{

}

EMCalLikelihood::~EMCalLikelihood()
{

}

int
EMCalLikelihood::InitRun(PHCompositeNode *topNode)
{
  _ievent = 0;

  PHNodeIterator iter(topNode);
  PHCompositeNode *dstNode = static_cast<PHCompositeNode*>(iter.findFirst(
      "PHCompositeNode", "DST"));
  if (!dstNode)
    {
      std::cerr << PHWHERE << "DST Node missing, doing nothing." << std::endl;
      throw runtime_error(
          "Failed to find DST node in EmcRawTowerBuilder::CreateNodes");
    }

    {
      _trk = findNode::getClass<EMCalTrk>(dstNode, "EMCalTrk");

      if (not _trk)
        {

          cout << __PRETTY_FUNCTION__ << "::" << Name() << " - Error - "
              << "Can not find the EMCalTrk node" << endl;

          return Fun4AllReturnCodes::ABORTRUN;

        }

      assert(_trk);
    }

  return Fun4AllReturnCodes::EVENT_OK;
}

int
EMCalLikelihood::End(PHCompositeNode *topNode)
{

  cout << "EMCalLikelihood::End - write to " << _filename << endl;
  PHTFileServer::get().cd(_filename);

  Fun4AllHistoManager *hm = get_HistoManager();
  assert(hm);
  for (unsigned int i = 0; i < hm->nHistos(); i++)
    hm->getHisto(i)->Write();

  return Fun4AllReturnCodes::EVENT_OK;
}

int
EMCalLikelihood::Init(PHCompositeNode *topNode)
{

  cout << "EMCalLikelihood::Init - Making PHTFileServer " << _filename << endl;
  PHTFileServer::get().open(_filename, "RECREATE");

  Fun4AllHistoManager *hm = get_HistoManager();
  assert(hm);

  TH2F * h_merge_direction = new TH2F("h_merge_direction",
      "h_merge_direction;merge eta shift;merge phi shift", 10, -.5, 9.5, 10,
      -.5, 9.5);
  hm->registerHisto(h_merge_direction);

  if (h2_Edep_Distribution_e)
    {
      h2_Edep_Distribution_e =
          dynamic_cast<TH2 *>(h2_Edep_Distribution_e->Clone(
              "h2_Edep_Distribution_e"));
      hm->registerHisto(h2_Edep_Distribution_e);

      h1_ep_Distribution_e = h2_Edep_Distribution_e->ProjectionX(
          "h1_ep_Distribution_e");
      assert(h1_ep_Distribution_e);
      hm->registerHisto(h1_ep_Distribution_e);

      // regulate minimal probability value
      double min_prob = 1;
      for (int x = 1; x <= h2_Edep_Distribution_e->GetNbinsX(); ++x)
        for (int y = 1; y <= h2_Edep_Distribution_e->GetNbinsX(); ++y)
          {
            const double binc = h2_Edep_Distribution_e->GetBinContent(x, y);
            assert(binc >= 0);

            if (binc > 0 and binc < min_prob)
              min_prob = binc;
          }
      assert(min_prob > 0);

      for (int x = 0; x <= h2_Edep_Distribution_e->GetNbinsX() + 1; ++x)
        for (int y = 0; y <= h2_Edep_Distribution_e->GetNbinsX() + 1; ++y)
          {
            const double binc = h2_Edep_Distribution_e->GetBinContent(x, y);

            if (binc == 0)
              h2_Edep_Distribution_e->SetBinContent(x, y, min_prob);
          }

      for (int x = 0; x <= h1_ep_Distribution_e->GetNbinsX() + 1; ++x)
        {
          const double binc = h1_ep_Distribution_e->GetBinContent(x);

          if (binc == 0)
            h1_ep_Distribution_e->SetBinContent(x, min_prob);
        }
    }
  if (h2_Edep_Distribution_pi)
    {
      h2_Edep_Distribution_pi =
          dynamic_cast<TH2 *>(h2_Edep_Distribution_pi->Clone(
              "h2_Edep_Distribution_pi"));
      hm->registerHisto(h2_Edep_Distribution_pi);

      h1_ep_Distribution_pi = h2_Edep_Distribution_pi->ProjectionX(
          "h1_ep_Distribution_pi");
      assert(h1_ep_Distribution_pi);
      hm->registerHisto(h1_ep_Distribution_pi);

      // regulate minimal probability value
      double min_prob = 1;
      for (int x = 1; x <= h2_Edep_Distribution_pi->GetNbinsX(); ++x)
        for (int y = 1; y <= h2_Edep_Distribution_pi->GetNbinsX(); ++y)
          {
            const double binc = h2_Edep_Distribution_pi->GetBinContent(x, y);
            assert(binc >= 0);

            if (binc > 0 and binc < min_prob)
              min_prob = binc;
          }
      assert(min_prob > 0);

      for (int x = 0; x <= h2_Edep_Distribution_pi->GetNbinsX() + 1; ++x)
        for (int y = 0; y <= h2_Edep_Distribution_pi->GetNbinsX() + 1; ++y)
          {
            const double binc = h2_Edep_Distribution_pi->GetBinContent(x, y);

            if (binc == 0)
              h2_Edep_Distribution_pi->SetBinContent(x, y, min_prob);
          }

      for (int x = 0; x <= h1_ep_Distribution_pi->GetNbinsX() + 1; ++x)
        {
          const double binc = h1_ep_Distribution_pi->GetBinContent(x);

          if (binc == 0)
            h1_ep_Distribution_pi->SetBinContent(x, min_prob);
        }
    }
  return Fun4AllReturnCodes::EVENT_OK;
}

int
EMCalLikelihood::process_event(PHCompositeNode *topNode)
{
//  const double significand = _ievent / TMath::Power(10, (int) (log10(_ievent)));

//  if (fmod(significand, 1.0) == 0 && significand <= 10)
//    cout << "EMCalLikelihood::process_event - " << _ievent << endl;

  _ievent++;

  if (_do_ganging)
    ApplyEMCalGanging(_trk);

  UpdateEnergyDeposition(_trk);

  if (h2_Edep_Distribution_e and h2_Edep_Distribution_pi)
    UpdateEnergyDepositionLikelihood(_trk);

  return Fun4AllReturnCodes::EVENT_OK;
}

Fun4AllHistoManager *
EMCalLikelihood::get_HistoManager()
{

  Fun4AllServer *se = Fun4AllServer::instance();
  Fun4AllHistoManager *hm = se->getHistoManager("EMCalLikelihood_HISTOS");

  if (not hm)
    {
      cout
          << "EMCalLikelihood::get_HistoManager - Making Fun4AllHistoManager EMCalLikelihood_HISTOS"
          << endl;
      hm = new Fun4AllHistoManager("EMCalLikelihood_HISTOS");
      se->registerHistoManager(hm);
    }

  assert(hm);

  return hm;
}

void
EMCalLikelihood::ApplyEMCalGanging(EMCalTrk * trk)
{
  assert(trk);
  assert(_ganging_size.first > 1 or _ganging_size.second > 1);
  assert(_ganging_size.first > 0 and _ganging_size.second > 0);

  static bool once = true;
  if (once)
    {
      once = false;

      cout << "EMCalLikelihood::ApplyEMCalGanging - apply EMCal ganging "
          << _ganging_size.first << "x" << _ganging_size.second << endl;
    }

  // estimate an eta-phi bin
  TVector3 vertex(trk->gvx, trk->gvy, trk->gvz);
  TVector3 momentum(trk->gpx, trk->gpy, trk->gpz);
  const double radius = 100; // center of CEMC.
  const double eta_bin_cm = 2.5;
  const double phi_bin_rad = 0.025;

  assert(momentum.Pt() > 0);

  TVector3 proj = vertex + radius / momentum.Pt() * momentum;
  const int eta_bin = floor(proj.Z() / eta_bin_cm);
  const int phi_bin = floor(proj.Phi() / phi_bin_rad);

  const int merge_plus_eta = (eta_bin % _ganging_size.first);
  const bool merge_plus_phi = (phi_bin % _ganging_size.second);

  Fun4AllHistoManager *hm = get_HistoManager();
  assert(hm);

  TH2F * h_merge_direction = (TH2F *) hm->getHisto("h_merge_direction");
  assert(h_merge_direction);

  h_merge_direction->Fill(merge_plus_eta, merge_plus_phi);

  for (int ieta = 0; ieta < trk->Max_N_Tower; ++ieta)
    {
      for (int iphi = 0; iphi < trk->Max_N_Tower; ++iphi)
        {
          bool out_of_edge = false;

          double cemc_ieta = 0;
          double cemc_iphi = 0;
          double cemc_energy = 0;

          for (unsigned int dieta = 0; dieta < _ganging_size.first; ++dieta)
            {
              const unsigned int fetch_eta = _ganging_size.first * ieta + dieta + merge_plus_eta;

              if ( fetch_eta>= trk->Max_N_Tower or out_of_edge)
                {
                  out_of_edge = true;
                  break;
                }

              for (unsigned int diphi = 0; diphi < _ganging_size.second; ++diphi)
                {
                  const unsigned int fetch_phi = _ganging_size.first * iphi + diphi + merge_plus_phi;

                  if (fetch_phi>= trk->Max_N_Tower or out_of_edge)
                    {
                      out_of_edge = true;
                      break;
                    }

                  cemc_ieta += trk->cemc_ieta[fetch_eta][fetch_phi];
                  cemc_iphi += trk->cemc_iphi[fetch_eta][fetch_phi];
                  cemc_energy += trk->cemc_energy[fetch_eta][fetch_phi];
                }
            }

          if (out_of_edge)
            {
              trk->cemc_ieta[ieta][iphi] = -9999;
              trk->cemc_iphi[ieta][iphi] = -9999;
              trk->cemc_energy[ieta][iphi] = -0;
            }
          else
            {
              trk->cemc_ieta[ieta][iphi] = cemc_ieta / _ganging_size.first
                  / _ganging_size.second;
              trk->cemc_iphi[ieta][iphi] = cemc_iphi / _ganging_size.first
                  / _ganging_size.second;
              trk->cemc_energy[ieta][iphi] = cemc_energy;
            }
        }
    }
}

void
EMCalLikelihood::UpdateEnergyDeposition(EMCalTrk * trk)
{
  assert(trk);

  trk->cemc_sum_energy = 0;
  trk->cemc_sum_n_tower = 0;
  trk->hcalin_sum_energy = 0;
  trk->hcalin_sum_n_tower = 0;

  for (int ieta = 0; ieta < trk->Max_N_Tower; ++ieta)
    {
      for (int iphi = 0; iphi < trk->Max_N_Tower; ++iphi)
        {

          //cemc
          const double cemc_radius2 = pow(
              (trk->cemc_ieta[ieta][iphi] - center_cemc_ieta) / width_cemc_ieta,
              2.)
              + pow(
                  (trk->cemc_iphi[ieta][iphi] - center_cemc_iphi)
                      / width_cemc_iphi, 2.);
          trk->cemc_radius[ieta][iphi] = sqrt(cemc_radius2);

          if (cemc_radius2 <= 1.0)
            {
              trk->cemc_sum_energy += trk->cemc_energy[ieta][iphi];
              trk->cemc_sum_n_tower++;
            }

          // hcalin
          const double hcalin_radius2 = pow(
              (trk->hcalin_ieta[ieta][iphi] - center_hcalin_ieta)
                  / width_hcalin_ieta, 2.)
              + pow(
                  (trk->hcalin_iphi[ieta][iphi] - center_hcalin_iphi)
                      / width_hcalin_iphi, 2.);
          trk->hcalin_radius[ieta][iphi] = sqrt(hcalin_radius2);

          if (hcalin_radius2 <= 1.0)
            {
              trk->hcalin_sum_energy += trk->hcalin_energy[ieta][iphi];
              trk->hcalin_sum_n_tower++;
            }
        }
    }
}

void
EMCalLikelihood::UpdateEnergyDepositionLikelihood(EMCalTrk * trk)
{
  assert(trk);
  assert(h2_Edep_Distribution_e);
  assert(h2_Edep_Distribution_pi);
  assert(h1_ep_Distribution_e);
  assert(h1_ep_Distribution_pi);

  assert(
      h2_Edep_Distribution_e->GetNbinsX() == h2_Edep_Distribution_pi->GetNbinsX());
  assert(
      h2_Edep_Distribution_e->GetNbinsY() == h2_Edep_Distribution_pi->GetNbinsY());
  assert(
      h2_Edep_Distribution_e->GetNbinsX() == h1_ep_Distribution_e->GetNbinsX());
  assert(
      h1_ep_Distribution_pi->GetNbinsX() == h2_Edep_Distribution_pi->GetNbinsX());

  const int binx = h2_Edep_Distribution_e->GetXaxis()->FindBin(trk->get_ep());
  const int biny = h2_Edep_Distribution_e->GetYaxis()->FindBin(
      trk->hcalin_sum_energy);

    {
      double prob_e = h2_Edep_Distribution_e->GetBinContent(binx, biny);
      if (!prob_e > 0)
        {
          cout << __PRETTY_FUNCTION__ << Name()
              << " - Error - invalid likelihood value prob_e = " << prob_e
              << " @ bin " << binx << ", " << biny << endl;
        }
      assert(prob_e > 0);

      double prob_pi = h2_Edep_Distribution_pi->GetBinContent(binx, biny);
      if (!prob_pi > 0)
        {
          cout << __PRETTY_FUNCTION__ << Name()
              << " - Error - invalid likelihood value prob_pi = " << prob_pi
              << " @ bin " << binx << ", " << biny << endl;
        }
      assert(prob_pi > 0);

      trk->ll_edep_e = log(prob_e);
      trk->ll_edep_h = log(prob_pi);
    }

    {
      double prob_e = h1_ep_Distribution_e->GetBinContent(binx);
      assert(prob_e > 0);

      double prob_pi = h1_ep_Distribution_pi->GetBinContent(binx);
      assert(prob_pi > 0);

      trk->ll_ep_e = log(prob_e);
      trk->ll_ep_h = log(prob_pi);
    }
}
