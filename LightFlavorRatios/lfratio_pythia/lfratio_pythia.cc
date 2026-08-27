#include "HepMC3/GenEvent.h"
#include "HepMC3/GenParticle.h"
#include "HepMC3/GenVertex.h"
#include "HepMC3/ReaderAscii.h"

#include "TFile.h"
#include "TH1.h"
#include "TH1D.h"
#include "TH2F.h"
#include "TTree.h"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "/sphenix/user/hjheng/sPHENIXRepo/analysis/LightFlavorRatios/util/binning.h"

// Return true if this particle is the last copy in a same-PID chain
// This avoids double counting intermediate copies in HepMC records
bool is_last_copy(const HepMC3::ConstGenParticlePtr &p)
{
    if (!p)
        return false;

    auto end_vtx = p->end_vertex();
    if (!end_vtx)
        return true;

    for (const auto &d : end_vtx->particles_out())
    {
        if (!d)
            continue;

        if (d->pid() == p->pid())
            return false;
    }

    return true;
}

bool has_charm_or_bottom_digit(const int apid)
{
    int x = apid / 10;

    for (int i = 0; i < 3; ++i)
    {
        const int digit = x % 10;

        if (digit == 4 || digit == 5)
            return true;

        x /= 10;
    }

    return false;
}

// trace back through same-PID, single-step copies to the earliest copy
HepMC3::ConstGenParticlePtr trace_to_first_copy(HepMC3::ConstGenParticlePtr p)
{
    while (p)
    {
        auto prod = p->production_vertex();
        if (!prod)
            break;

        HepMC3::ConstGenParticlePtr same_pid_parent = nullptr;
        for (const auto &in : prod->particles_in())
        {
            if (in && in->pid() == p->pid())
            {
                same_pid_parent = in;
                break;
            }
        }

        if (!same_pid_parent)
            break;

        p = same_pid_parent;
    }

    return p;
}

// Prompt definition used here:
//
// Keep K0S / Lambda / anti-Lambda if it is not produced from
// weak strange-hadron feed-down or charm/bottom feed-down
//
// For K0S, do NOT reject K0/K0bar parents, since the HepMC record may
// represent neutral-kaon mixing as K0/K0bar -> K0S
bool is_from_feeddown_parent(const HepMC3::ConstGenParticlePtr &p)
{
    if (!p)
        return false;

    auto first = trace_to_first_copy(p);
    if (!first)
        return false;

    auto prod_vtx = first->production_vertex();
    if (!prod_vtx)
        return false;

    for (const auto &parent : prod_vtx->particles_in())
    {
        if (!parent)
            continue;

        const int apid = std::abs(parent->pid());

        // Xi-, Xi0, Omega-
        if (apid == 3312 || apid == 3322 || apid == 3334)
            return true;

        // charm/bottom hadron feed-down
        if (has_charm_or_bottom_digit(apid))
            return true;
    }

    return false;
}

int rapidity_region(const double y)
{
    const double absy = std::abs(y);

    if (absy <= 0.5)
        return 0;

    if (absy <= 1.0)
        return 1;

    return 2;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " input.hepmc output.root\n";
        return 1;
    }

    const char *hepmc_file = argv[1];
    const char *root_file = argv[2];

    HepMC3::ReaderAscii reader(hepmc_file);
    HepMC3::GenEvent event;

    TFile *fout = new TFile(root_file, "RECREATE");

    if (!fout || fout->IsZombie())
    {
        std::cerr << "Error: cannot create output ROOT file: " << root_file << std::endl;
        return 1;
    }

    const auto &pt_bins = BinInfo::final_pt_bins.bins;
    const auto &eta_bins = BinInfo::final_eta_bins.bins;
    const auto &rapidity_bins = BinInfo::final_rapidity_bins.bins;
    const auto &phi_bins = BinInfo::final_phi_bins.bins;

    TH1D *h_event_counter = new TH1D("h_event_counter", ";dummy;Events", 1, 0.5, 1.5);
    TH1D *h_prompt_kshort_count = new TH1D("h_prompt_kshort_count", ";dummy;Prompt K^{0}_{S}", 1, 0.5, 1.5);
    TH1D *h_prompt_lambda_count = new TH1D("h_prompt_lambda_count", ";dummy;Prompt #Lambda^{0}(+c.c.)", 1, 0.5, 1.5);

    TH1D *h_prompt_kshort_pt = new TH1D("h_prompt_kshort_pt", ";K^{0}_{S} p_{T} [GeV];Counts", pt_bins.size() - 1, pt_bins.data());
    TH1D *h_prompt_lambda_pt = new TH1D("h_prompt_lambda_pt", ";#Lambda^{0}(+c.c.) p_{T} [GeV];Counts", pt_bins.size() - 1, pt_bins.data());

    TH1D *h_prompt_kshort_eta = new TH1D("h_prompt_kshort_eta", ";K^{0}_{S} #eta;Counts", eta_bins.size() - 1, eta_bins.data());
    TH1D *h_prompt_lambda_eta = new TH1D("h_prompt_lambda_eta", ";#Lambda^{0}(+c.c.) #eta;Counts", eta_bins.size() - 1, eta_bins.data());

    TH1D *h_prompt_kshort_y = new TH1D("h_prompt_kshort_y", ";K^{0}_{S} rapidity;Counts", rapidity_bins.size() - 1, rapidity_bins.data());
    TH1D *h_prompt_lambda_y = new TH1D("h_prompt_lambda_y", ";#Lambda^{0}(+c.c.) rapidity;Counts", rapidity_bins.size() - 1, rapidity_bins.data());

    TH1D *h_prompt_kshort_phi = new TH1D("h_prompt_kshort_phi", ";K^{0}_{S} #phi;Counts", phi_bins.size() - 1, phi_bins.data());
    TH1D *h_prompt_lambda_phi = new TH1D("h_prompt_lambda_phi", ";#Lambda^{0}(+c.c.) #phi;Counts", phi_bins.size() - 1, phi_bins.data());

    TH1D *h_prompt_kshort_pt_y_le_0p5 = new TH1D("h_prompt_kshort_pt_y_le_0p5", ";K^{0}_{S} p_{T} [GeV];Counts, |y| #leq 0.5", pt_bins.size() - 1, pt_bins.data());

    TH1D *h_prompt_lambda_pt_y_le_0p5 = new TH1D("h_prompt_lambda_pt_y_le_0p5", ";#Lambda^{0}(+c.c.) p_{T} [GeV];Counts, |y| #leq 0.5", pt_bins.size() - 1, pt_bins.data());

    TH1D *h_prompt_kshort_pt_y_0p5_1p0 = new TH1D("h_prompt_kshort_pt_y_0p5_1p0", ";K^{0}_{S} p_{T} [GeV];Counts, 0.5 < |y| #leq 1.0", pt_bins.size() - 1, pt_bins.data());

    TH1D *h_prompt_lambda_pt_y_0p5_1p0 = new TH1D("h_prompt_lambda_pt_y_0p5_1p0", ";#Lambda^{0}(+c.c.) p_{T} [GeV];Counts, 0.5 < |y| #leq 1.0", pt_bins.size() - 1, pt_bins.data());

    TH1D *h_prompt_kshort_pt_y_gt_1p0 = new TH1D("h_prompt_kshort_pt_y_gt_1p0", ";K^{0}_{S} p_{T} [GeV];Counts, |y| > 1.0", pt_bins.size() - 1, pt_bins.data());

    TH1D *h_prompt_lambda_pt_y_gt_1p0 = new TH1D("h_prompt_lambda_pt_y_gt_1p0", ";#Lambda^{0}(+c.c.) p_{T} [GeV];Counts, |y| > 1.0", pt_bins.size() - 1, pt_bins.data());

    TTree *tree = new TTree("prompt_strange_tree", "prompt_strange_tree");

    int event_id = -1;
    int particle_pid = 0;
    int particle_y_region = -1;
    double particle_pt = -999.;
    double particle_eta = -999.;
    double particle_y = -999.;
    double particle_phi = -999.;
    double particle_mass = -999.;

    tree->Branch("event", &event_id, "event/I");
    tree->Branch("pid", &particle_pid, "pid/I");
    tree->Branch("y_region", &particle_y_region, "y_region/I");
    tree->Branch("pt", &particle_pt, "pt/D");
    tree->Branch("eta", &particle_eta, "eta/D");
    tree->Branch("y", &particle_y, "y/D");
    tree->Branch("phi", &particle_phi, "phi/D");
    tree->Branch("mass", &particle_mass, "mass/D");

    int iev = 0;

    while (!reader.failed())
    {
        reader.read_event(event);

        if (reader.failed())
            break;

        ++iev;

        if (iev % 5000 == 0)
        {
            std::cout << "Processing event " << iev << std::endl;
        }

        event_id = iev;
        h_event_counter->Fill(1.0);

        for (const auto &p : event.particles())
        {
            if (!p)
                continue;

            const int pid = p->pid();
            const int apid = std::abs(pid);

            // select K0S, Lambda, and anti-Lambda
            if (pid != 310 && apid != 3122)
                continue;

            // avoid double counting same-PID copies
            if (!is_last_copy(p))
                continue;

            if (apid == 3122 && is_from_feeddown_parent(p))
                continue;

            particle_pid = pid;
            particle_pt = p->momentum().pt();
            particle_eta = p->momentum().eta();
            particle_y = p->momentum().rap();
            particle_phi = p->momentum().phi();
            particle_mass = p->momentum().m();
            particle_y_region = rapidity_region(particle_y);

            if (pid == 310)
            {
                h_prompt_kshort_count->Fill(1.0);
                h_prompt_kshort_pt->Fill(particle_pt);
                h_prompt_kshort_eta->Fill(particle_eta);
                h_prompt_kshort_y->Fill(particle_y);
                h_prompt_kshort_phi->Fill(particle_phi);

                if (particle_y_region == 0)
                    h_prompt_kshort_pt_y_le_0p5->Fill(particle_pt);
                else if (particle_y_region == 1)
                    h_prompt_kshort_pt_y_0p5_1p0->Fill(particle_pt);
                else
                    h_prompt_kshort_pt_y_gt_1p0->Fill(particle_pt);
            }
            else if (apid == 3122)
            {
                h_prompt_lambda_count->Fill(1.0);
                h_prompt_lambda_pt->Fill(particle_pt);
                h_prompt_lambda_eta->Fill(particle_eta);
                h_prompt_lambda_y->Fill(particle_y);
                h_prompt_lambda_phi->Fill(particle_phi);

                if (particle_y_region == 0)
                    h_prompt_lambda_pt_y_le_0p5->Fill(particle_pt);
                else if (particle_y_region == 1)
                    h_prompt_lambda_pt_y_0p5_1p0->Fill(particle_pt);
                else
                    h_prompt_lambda_pt_y_gt_1p0->Fill(particle_pt);
            }

            tree->Fill();
        }

        event.clear();
    }

    TH1D *h_ratio_lambda_over_kshort_pt = static_cast<TH1D *>(h_prompt_lambda_pt->Clone("h_ratio_lambda_over_kshort_pt"));
    h_ratio_lambda_over_kshort_pt->SetTitle(";p_{T} [GeV];#Lambda^{0}(+c.c.) / K^{0}_{S}");
    h_ratio_lambda_over_kshort_pt->Divide(h_prompt_lambda_pt, h_prompt_kshort_pt, 1.0, 1.0);

    TH1D *h_ratio_lambda_over_2kshort_pt = static_cast<TH1D *>(h_prompt_lambda_pt->Clone("h_ratio_lambda_over_2kshort_pt"));
    h_ratio_lambda_over_2kshort_pt->SetTitle(";p_{T} [GeV];#Lambda^{0}(+c.c.) / 2K^{0}_{S}");
    h_ratio_lambda_over_2kshort_pt->Divide(h_prompt_lambda_pt, h_prompt_kshort_pt, 1.0, 2.0);

    TH1D *h_ratio_lambda_over_kshort_eta = static_cast<TH1D *>(h_prompt_lambda_eta->Clone("h_ratio_lambda_over_kshort_eta"));
    h_ratio_lambda_over_kshort_eta->SetTitle(";#eta;#Lambda^{0}(+c.c.) / K^{0}_{S}");
    h_ratio_lambda_over_kshort_eta->Divide(h_prompt_lambda_eta, h_prompt_kshort_eta, 1.0, 1.0);

    TH1D *h_ratio_lambda_over_2kshort_eta = static_cast<TH1D *>(h_prompt_lambda_eta->Clone("h_ratio_lambda_over_2kshort_eta"));
    h_ratio_lambda_over_2kshort_eta->SetTitle(";#eta;#Lambda^{0}(+c.c.) / 2K^{0}_{S}");
    h_ratio_lambda_over_2kshort_eta->Divide(h_prompt_lambda_eta, h_prompt_kshort_eta, 1.0, 2.0);

    TH1D *h_ratio_lambda_over_kshort_y = static_cast<TH1D *>(h_prompt_lambda_y->Clone("h_ratio_lambda_over_kshort_y"));
    h_ratio_lambda_over_kshort_y->SetTitle(";rapidity;#Lambda^{0}(+c.c.) / K^{0}_{S}");
    h_ratio_lambda_over_kshort_y->Divide(h_prompt_lambda_y, h_prompt_kshort_y, 1.0, 1.0);

    TH1D *h_ratio_lambda_over_2kshort_y = static_cast<TH1D *>(h_prompt_lambda_y->Clone("h_ratio_lambda_over_2kshort_y"));
    h_ratio_lambda_over_2kshort_y->SetTitle(";rapidity;#Lambda^{0}(+c.c.) / 2K^{0}_{S}");
    h_ratio_lambda_over_2kshort_y->Divide(h_prompt_lambda_y, h_prompt_kshort_y, 1.0, 2.0);

    TH1D *h_ratio_lambda_over_kshort_pt_y_le_0p5 = static_cast<TH1D *>(h_prompt_lambda_pt_y_le_0p5->Clone("h_ratio_lambda_over_kshort_pt_y_le_0p5"));
    h_ratio_lambda_over_kshort_pt_y_le_0p5->SetTitle(";p_{T} [GeV];#Lambda^{0}(+c.c.) / K^{0}_{S}, |y| #leq 0.5");
    h_ratio_lambda_over_kshort_pt_y_le_0p5->Divide(h_prompt_lambda_pt_y_le_0p5, h_prompt_kshort_pt_y_le_0p5, 1.0, 1.0);

    TH1D *h_ratio_lambda_over_2kshort_pt_y_le_0p5 = static_cast<TH1D *>(h_prompt_lambda_pt_y_le_0p5->Clone("h_ratio_lambda_over_2kshort_pt_y_le_0p5"));
    h_ratio_lambda_over_2kshort_pt_y_le_0p5->SetTitle(";p_{T} [GeV];#Lambda^{0}(+c.c.) / 2K^{0}_{S}, |y| #leq 0.5");
    h_ratio_lambda_over_2kshort_pt_y_le_0p5->Divide(h_prompt_lambda_pt_y_le_0p5, h_prompt_kshort_pt_y_le_0p5, 1.0, 2.0);

    TH1D *h_ratio_lambda_over_kshort_pt_y_0p5_1p0 = static_cast<TH1D *>(h_prompt_lambda_pt_y_0p5_1p0->Clone("h_ratio_lambda_over_kshort_pt_y_0p5_1p0"));
    h_ratio_lambda_over_kshort_pt_y_0p5_1p0->SetTitle(";p_{T} [GeV];#Lambda^{0}(+c.c.) / K^{0}_{S}, 0.5 < |y| #leq 1.0");
    h_ratio_lambda_over_kshort_pt_y_0p5_1p0->Divide(h_prompt_lambda_pt_y_0p5_1p0, h_prompt_kshort_pt_y_0p5_1p0, 1.0, 1.0);

    TH1D *h_ratio_lambda_over_2kshort_pt_y_0p5_1p0 = static_cast<TH1D *>(h_prompt_lambda_pt_y_0p5_1p0->Clone("h_ratio_lambda_over_2kshort_pt_y_0p5_1p0"));
    h_ratio_lambda_over_2kshort_pt_y_0p5_1p0->SetTitle(";p_{T} [GeV];#Lambda^{0}(+c.c.) / 2K^{0}_{S}, 0.5 < |y| #leq 1.0");
    h_ratio_lambda_over_2kshort_pt_y_0p5_1p0->Divide(h_prompt_lambda_pt_y_0p5_1p0, h_prompt_kshort_pt_y_0p5_1p0, 1.0, 2.0);

    TH1D *h_ratio_lambda_over_kshort_pt_y_gt_1p0 = static_cast<TH1D *>(h_prompt_lambda_pt_y_gt_1p0->Clone("h_ratio_lambda_over_kshort_pt_y_gt_1p0"));
    h_ratio_lambda_over_kshort_pt_y_gt_1p0->SetTitle(";p_{T} [GeV];#Lambda^{0}(+c.c.) / K^{0}_{S}, |y| > 1.0");
    h_ratio_lambda_over_kshort_pt_y_gt_1p0->Divide(h_prompt_lambda_pt_y_gt_1p0, h_prompt_kshort_pt_y_gt_1p0, 1.0, 1.0);

    TH1D *h_ratio_lambda_over_2kshort_pt_y_gt_1p0 = static_cast<TH1D *>(h_prompt_lambda_pt_y_gt_1p0->Clone("h_ratio_lambda_over_2kshort_pt_y_gt_1p0"));
    h_ratio_lambda_over_2kshort_pt_y_gt_1p0->SetTitle(";p_{T} [GeV];#Lambda^{0}(+c.c.) / 2K^{0}_{S}, |y| > 1.0");
    h_ratio_lambda_over_2kshort_pt_y_gt_1p0->Divide(h_prompt_lambda_pt_y_gt_1p0, h_prompt_kshort_pt_y_gt_1p0, 1.0, 2.0);

    const double n_prompt_kshort = h_prompt_kshort_count->GetBinContent(1);
    const double n_prompt_lambda = h_prompt_lambda_count->GetBinContent(1);

    const double ratio_lambda_over_kshort = (n_prompt_kshort > 0.0) ? n_prompt_lambda / n_prompt_kshort : 0.0;
    const double ratio_lambda_over_2kshort = (n_prompt_kshort > 0.0) ? n_prompt_lambda / (2.0 * n_prompt_kshort) : 0.0;

    std::cout << "Processed events              = " << iev << "\n";
    std::cout << "Prompt K0S                    = " << n_prompt_kshort << "\n";
    std::cout << "Prompt Lambda(+c.c.)          = " << n_prompt_lambda << "\n";
    std::cout << "Lambda(+c.c.) / K0S           = " << ratio_lambda_over_kshort << "\n";
    std::cout << "Lambda(+c.c.) / 2K0S          = " << ratio_lambda_over_2kshort << "\n";
    std::cout << "Saved ROOT file               = " << root_file << "\n";

    fout->Write();
    fout->Close();

    delete fout;

    return 0;
}